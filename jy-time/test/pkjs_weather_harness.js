#!/usr/bin/env node
'use strict';
/*
 * PKJS weather pipeline harness for jy-time.
 *
 * Runs src/pkjs/index.js inside a vm sandbox with fake Pebble, XMLHttpRequest,
 * navigator.geolocation, localStorage and timers, then drives the weather
 * refresh pipeline end to end. No dependencies.
 *
 *   node test/pkjs_weather_harness.js          # run all scenarios
 *   node test/pkjs_weather_harness.js -v       # also print PKJS console output
 *
 * Fixtures in test/fixtures are real API responses captured with curl.
 */
var fs = require('fs');
var path = require('path');
var vm = require('vm');

var ROOT = path.join(__dirname, '..');
var VERBOSE = process.argv.indexOf('-v') !== -1;
var INDEX_SRC = fs.readFileSync(path.join(ROOT, 'src/pkjs/index.js'), 'utf8');
var PKG = JSON.parse(fs.readFileSync(path.join(ROOT, 'package.json'), 'utf8'));
var CONFIG = JSON.parse(fs.readFileSync(path.join(ROOT, 'src/pkjs/config.json'), 'utf8'));

function fixture(name) {
  return fs.readFileSync(path.join(__dirname, 'fixtures', name), 'utf8');
}

// Mirror the SDK's message_keys numbering: sequential from 10000, array keys
// reserve their declared length. Exact numbers do not matter to the tests.
function buildMessageKeys() {
  var keys = {};
  var next = 10000;
  PKG.pebble.messageKeys.forEach(function(k) {
    var m = k.match(/^(\w+)(?:\[(\d+)\])?$/);
    keys[m[1]] = next;
    next += m[2] ? parseInt(m[2], 10) : 1;
  });
  return keys;
}
var KEYS = buildMessageKeys();

// ---------- fake clock / timers ----------
function FakeClock(startMs) {
  this.now = startMs;
  this.timers = [];
  this.nextId = 1;
}
FakeClock.prototype.add = function(fn, ms, repeat) {
  var t = { id: this.nextId++, fn: fn, ms: Math.max(0, Number(ms) || 0), repeat: !!repeat };
  t.at = this.now + t.ms;
  this.timers.push(t);
  return t.id;
};
FakeClock.prototype.remove = function(id) {
  this.timers = this.timers.filter(function(t) { return t.id !== id; });
};
FakeClock.prototype.advance = function(ms) {
  var target = this.now + ms;
  for (;;) {
    var due = null;
    for (var i = 0; i < this.timers.length; i++) {
      var t = this.timers[i];
      if (t.at <= target && (!due || t.at < due.at)) due = t;
    }
    if (!due) break;
    this.now = due.at;
    if (due.repeat) {
      due.at += due.ms;
    } else {
      this.remove(due.id);
    }
    due.fn();
  }
  this.now = target;
};

// ---------- environment ----------
function makeEnv(opts) {
  opts = opts || {};
  var env = {
    clock: new FakeClock(opts.startMs || Date.UTC(2026, 7, 25, 14, 0, 0)),
    logs: [],
    requests: [],
    sent: [],
    openedUrls: [],
    listeners: {},
    storage: {},
    routes: [],
    geoMode: opts.geoMode || { coords: { latitude: 40.7128, longitude: -74.006 } },
    sendModes: opts.sendModes || [],
    sendCalls: 0
  };

  env.route = function(prefix, response) {
    env.routes.push({ prefix: prefix, response: response });
  };
  env.findRoute = function(url) {
    for (var i = env.routes.length - 1; i >= 0; i--) {
      if (url.indexOf(env.routes[i].prefix) === 0) return env.routes[i].response;
    }
    return null;
  };
  env.requestsTo = function(prefix) {
    return env.requests.filter(function(r) { return r.url.indexOf(prefix) === 0; });
  };
  env.sentWith = function(key) {
    return env.sent.filter(function(d) { return typeof d[key] !== 'undefined'; });
  };
  env.logsMatching = function(re) {
    return env.logs.filter(function(l) { return re.test(l); });
  };
  env.emit = function(type, event) {
    (env.listeners[type] || []).slice().forEach(function(fn) { fn(event); });
  };
  env.diag = function() {
    try { return JSON.parse(env.storage['weather-diag'] || 'null'); } catch (e) { return null; }
  };

  var localStorage = {
    getItem: function(k) { return Object.prototype.hasOwnProperty.call(env.storage, k) ? env.storage[k] : null; },
    setItem: function(k, v) { env.storage[k] = String(v); },
    removeItem: function(k) { delete env.storage[k]; },
    clear: function() { env.storage = {}; },
    key: function(i) { return Object.keys(env.storage)[i] || null; }
  };
  Object.defineProperty(localStorage, 'length', { get: function() { return Object.keys(env.storage).length; } });

  var fakeConsole = {};
  ['log', 'warn', 'error', 'info', 'debug'].forEach(function(level) {
    fakeConsole[level] = function() {
      var msg = Array.prototype.slice.call(arguments).map(String).join(' ');
      env.logs.push(msg);
      if (VERBOSE) process.stdout.write('    [pkjs:' + level + '] ' + msg + '\n');
    };
  });

  function XHR() {
    this.readyState = 0;
    this.status = 0;
    this.responseText = '';
    this.timeout = 0;
    this._headers = {};
  }
  XHR.prototype.open = function(method, url) { this._method = method; this._url = url; this.readyState = 1; };
  XHR.prototype.setRequestHeader = function(k, v) { this._headers[k] = v; };
  XHR.prototype.send = function() {
    var self = this;
    env.requests.push({ url: self._url, headers: self._headers, method: self._method });
    var route = env.findRoute(self._url) || { status: 404, body: '' };
    if (typeof route === 'function') route = route(self._url);
    if (route.hang) {
      if (self.timeout > 0) {
        env.clock.add(function() { self.readyState = 4; if (self.ontimeout) self.ontimeout(); }, self.timeout);
      }
      return;
    }
    env.clock.add(function() {
      self.readyState = 4;
      if (route.error) { if (self.onerror) self.onerror(); return; }
      self.status = route.status;
      self.responseText = typeof route.body === 'string' ? route.body : JSON.stringify(route.body);
      if (self.onload) self.onload();
    }, route.delay || 10);
  };

  var navigator = {
    geolocation: {
      getCurrentPosition: function(ok, err) {
        var mode = env.geoMode;
        if (mode === 'hang') return;
        env.clock.add(function() {
          if (mode.error) err(mode.error); else ok({ coords: mode.coords });
        }, 5);
      }
    }
  };

  var Pebble = {
    platform: 'android',
    addEventListener: function(type, fn) { (env.listeners[type] = env.listeners[type] || []).push(fn); },
    removeEventListener: function() {},
    sendAppMessage: function(dict, ok, fail) {
      var mode = env.sendModes[env.sendCalls++] || 'ack';
      if (mode === 'throw') throw new Error('Java exception was raised during method invocation');
      env.sent.push(dict);
      if (mode === 'silent') return 1;
      env.clock.add(function() {
        if (mode === 'nack') { if (fail) fail({ error: 'nack' }); } else if (ok) ok({});
      }, 5);
      return 1;
    },
    openURL: function(url) { env.openedUrls.push(url); },
    getActiveWatchInfo: function() { return { platform: 'emery' }; }
  };

  var RealDate = Date;
  function FakeDate() {
    if (arguments.length === 0) return new RealDate(env.clock.now);
    var args = [null].concat(Array.prototype.slice.call(arguments));
    return new (Function.prototype.bind.apply(RealDate, args))();
  }
  FakeDate.now = function() { return env.clock.now; };
  FakeDate.UTC = RealDate.UTC;
  FakeDate.parse = RealDate.parse;
  FakeDate.prototype = RealDate.prototype;

  function FakeClay(config) {
    this.config = JSON.parse(JSON.stringify(config));
  }
  FakeClay.prototype.generateUrl = function() {
    return 'data:text/html,' + encodeURIComponent(JSON.stringify(this.config));
  };
  FakeClay.prototype.getSettings = function(response) {
    var settings = JSON.parse(response);
    localStorage.setItem('clay-settings', JSON.stringify(settings));
    return settings;
  };

  function fakeRequire(name) {
    if (name === '@rebble/clay') return FakeClay;
    if (name === './config.json') return JSON.parse(JSON.stringify(CONFIG));
    if (name === 'message_keys') return KEYS;
    throw new Error('harness: unexpected require ' + name);
  }

  var sandbox = {
    Pebble: Pebble,
    XMLHttpRequest: XHR,
    navigator: navigator,
    localStorage: localStorage,
    console: fakeConsole,
    Date: FakeDate,
    setTimeout: function(fn, ms) { return env.clock.add(fn, ms, false); },
    setInterval: function(fn, ms) { return env.clock.add(fn, ms, true); },
    clearTimeout: function(id) { env.clock.remove(id); },
    clearInterval: function(id) { env.clock.remove(id); }
  };
  vm.createContext(sandbox);
  env.sandbox = sandbox;

  env.load = function() {
    var wrapper = '(function(require, module, exports) {\n' + INDEX_SRC + '\n})';
    var fn = vm.runInContext(wrapper, sandbox, { filename: 'index.js' });
    fn(fakeRequire, { exports: {} }, {});
  };

  env.start = function(settings) {
    env.storage['clay-settings'] = JSON.stringify(settings || {});
    env.load();
    env.emit('ready', { ready: true });
    env.clock.advance(200);
  };

  // Default routes: healthy Open-Meteo, NWS, ZIP lookups.
  env.route('https://api.open-meteo.com/v1/forecast', { status: 200, body: fixture('open_meteo.json') });
  env.route('https://api.weather.gov/points/', { status: 200, body: fixture('nws_points.json') });
  // Routes match by prefix, last registered wins, so the more specific
  // hourly URL must be registered after the plain forecast URL.
  env.route('https://api.weather.gov/gridpoints/OKX/33,42/forecast', { status: 200, body: fixture('nws_forecast.json') });
  env.route('https://api.weather.gov/gridpoints/OKX/33,42/forecast/hourly', { status: 200, body: fixture('nws_hourly.json') });
  env.route('https://api.weather.gov/alerts/active', { status: 200, body: fixture('nws_alerts.json') });
  env.route('https://api.zippopotam.us/us/', { status: 200, body: fixture('zippopotam_10001.json') });
  return env;
}

// ---------- tiny test runner ----------
var results = [];
function scenario(name, fn) {
  var failures = [];
  function check(cond, msg) { if (!cond) failures.push(msg); }
  try {
    fn(check);
  } catch (e) {
    failures.push('threw: ' + (e && e.stack || e));
  }
  results.push({ name: name, failures: failures });
  process.stdout.write((failures.length ? 'FAIL ' : 'ok   ') + name + '\n');
  failures.forEach(function(f) { process.stdout.write('       - ' + f + '\n'); });
}

var MIN = 60 * 1000;

// ---------- scenarios ----------
scenario('baseline: GPS ok, Open-Meteo ok, weather reaches the watch', function(check) {
  var env = makeEnv();
  env.start({});
  check(env.requestsTo('https://api.open-meteo.com').length === 1, 'one Open-Meteo request');
  check(env.sentWith(KEYS.TEMPERATURE).length === 1, 'TEMPERATURE dict sent to watch');
  var d = env.diag();
  check(d && d.okAt > 0, 'diag okAt recorded');
  check(d && d.source === 'gps', 'diag source is gps, got ' + (d && d.source));
  var cached = JSON.parse(env.storage['weather-last-coords'] || 'null');
  check(cached && Math.abs(cached.lat - 40.7128) < 1e-6, 'GPS coords cached for later fallback');
});

scenario('manual coordinates are used without asking for GPS', function(check) {
  var env = makeEnv({ geoMode: 'hang' });
  env.start({ WEATHER_SOURCE: 'manual', WEATHER_LAT: '30.246', WEATHER_LON: '-87.700' });
  var reqs = env.requestsTo('https://api.open-meteo.com');
  check(reqs.length === 1, 'one Open-Meteo request');
  check(reqs[0] && reqs[0].url.indexOf('latitude=30.246') !== -1, 'manual latitude in URL');
  check(env.sentWith(KEYS.TEMPERATURE).length === 1, 'TEMPERATURE dict sent');
});

scenario('queue recovers when Pebble.sendAppMessage throws synchronously', function(check) {
  // First send (the layout settings dict) throws like the Android bridge does
  // when the Kotlin side raises; everything after must still flow.
  var env = makeEnv({ sendModes: ['throw'] });
  env.start({});
  check(env.sentWith(KEYS.TEMPERATURE).length === 1, 'TEMPERATURE dict still delivered after a throw');
  check(env.logsMatching(/threw/i).length >= 1, 'the throw is logged');
});

scenario('queue recovers when an AppMessage callback never fires (watchdog)', function(check) {
  var env = makeEnv({ sendModes: ['silent'] });
  env.start({});
  check(env.sentWith(KEYS.TEMPERATURE).length === 0, 'nothing else delivered while the first send is stuck');
  env.clock.advance(45 * 1000);
  check(env.sentWith(KEYS.TEMPERATURE).length === 1, 'TEMPERATURE dict delivered after the watchdog reset');
});

scenario('GPS failure falls back to the last known coordinates', function(check) {
  var env = makeEnv({ geoMode: { error: { code: 1, message: 'Location permission not granted' } } });
  env.storage['weather-last-coords'] = JSON.stringify({ lat: 30.246, lon: -87.7, t: env.clock.now - 3 * 24 * 3600 * 1000 });
  env.start({});
  var reqs = env.requestsTo('https://api.open-meteo.com');
  check(reqs.length === 1, 'Open-Meteo still requested');
  check(reqs[0] && reqs[0].url.indexOf('latitude=30.246') !== -1, 'cached latitude used');
  var d = env.diag();
  check(d && d.source === 'cached', 'diag source is cached, got ' + (d && d.source));
  check(d && /permission/i.test(d.lastError || ''), 'location error text kept for diagnostics: ' + (d && d.lastError));
});

scenario('GPS failure with no fallback records the runtime error text', function(check) {
  var env = makeEnv({ geoMode: { error: { code: 1, message: 'Location not available' } } });
  env.start({});
  check(env.requestsTo('https://api.open-meteo.com').length === 0, 'no weather request possible');
  var d = env.diag();
  check(d && /Location not available/.test(d.lastError || ''), 'runtime error text recorded: ' + (d && d.lastError));
  check(!d || !d.okAt, 'no okAt recorded');
});

scenario('NWS ZIP still works when GPS fails and no manual coordinates exist', function(check) {
  var env = makeEnv({ geoMode: { error: { code: 1, message: 'Location permission not granted' } } });
  env.start({ WEATHER_PROVIDER: 'nws', NWS_ZIP: '10001' });
  check(env.requestsTo('https://api.zippopotam.us').length === 1, 'ZIP resolved');
  check(env.requestsTo('https://api.weather.gov/points/').length === 1, 'NWS points requested');
  check(env.sentWith(KEYS.NWS_LOCATION_LABEL).length === 1, 'NWS frame A delivered');
  var om = env.requestsTo('https://api.open-meteo.com');
  check(om.length === 1 && om[0].url.indexOf('latitude=40.7484') !== -1, 'Open-Meteo used the ZIP coordinates');
  var d = env.diag();
  check(d && d.source === 'zip', 'diag source is zip, got ' + (d && d.source));
});

scenario('watch REFRESH_REQUEST refreshes when due and is ignored when not', function(check) {
  var env = makeEnv();
  env.start({ WEATHER_REFRESH_MIN: '15' });
  check(env.requestsTo('https://api.open-meteo.com').length === 1, 'initial fetch');
  // Simulate the phone runtime dropping the JS timers (throttled or dead).
  env.clock.timers = env.clock.timers.filter(function(t) { return !t.repeat; });
  env.clock.advance(5 * MIN);
  env.emit('appmessage', { payload: { REFRESH_REQUEST: 1 } });
  env.clock.advance(200);
  check(env.requestsTo('https://api.open-meteo.com').length === 1, 'ping 5 min after success does not refetch');
  env.clock.advance(11 * MIN);
  env.emit('appmessage', { payload: { REFRESH_REQUEST: 1 } });
  env.clock.advance(200);
  check(env.requestsTo('https://api.open-meteo.com').length === 2, 'ping 16 min after success refetches');
  check(env.sentWith(KEYS.TEMPERATURE).length === 2, 'second TEMPERATURE dict delivered');
  // Legacy app delivers numeric key ids instead of names.
  env.clock.advance(16 * MIN);
  var numeric = {};
  numeric[KEYS.REFRESH_REQUEST] = 1;
  env.emit('appmessage', { payload: numeric });
  env.clock.advance(200);
  check(env.requestsTo('https://api.open-meteo.com').length === 3, 'numeric-key ping also refetches');
});

scenario('Open-Meteo HTTP error is recorded, does not stall, and retries on the next ping', function(check) {
  var env = makeEnv();
  env.route('https://api.open-meteo.com/v1/forecast', { status: 429, body: { error: true, reason: 'Minutely API request limit exceeded' } });
  env.start({ WEATHER_REFRESH_MIN: '60' });
  check(env.sentWith(KEYS.TEMPERATURE).length === 0, 'no weather dict on HTTP 429');
  var d = env.diag();
  check(d && /429/.test(d.lastError || ''), 'HTTP status recorded in diag: ' + (d && d.lastError));
  check(env.sentWith(KEYS.WEATHER_PROVIDER).length >= 1, 'provider dict still sent');
  env.route('https://api.open-meteo.com/v1/forecast', { status: 200, body: fixture('open_meteo.json') });
  env.clock.advance(6 * MIN);
  env.emit('appmessage', { payload: { REFRESH_REQUEST: 1 } });
  env.clock.advance(200);
  check(env.sentWith(KEYS.TEMPERATURE).length === 1, 'retry after failure succeeds on the next ping');
  var d2 = env.diag();
  check(d2 && d2.okAt > 0 && d2.okAt > d2.lastErrorAt, 'okAt newer than lastErrorAt after recovery');
});

scenario('Open-Meteo request that hangs times out instead of blocking the chain', function(check) {
  var env = makeEnv();
  env.route('https://api.open-meteo.com/v1/forecast', { hang: true });
  env.start({ WEATHER_PROVIDER: 'nws' });
  env.clock.advance(20 * 1000);
  check(env.logsMatching(/Weather timeout/i).length === 1, 'timeout logged');
  check(env.requestsTo('https://api.weather.gov/points/').length === 1, 'NWS chain still ran after the timeout');
  var d = env.diag();
  check(d && /timeout/i.test(d.lastError || ''), 'timeout recorded in diag');
});

scenario('settings page shows last update and last error', function(check) {
  var env = makeEnv({ geoMode: { error: { code: 1, message: 'Location permission not granted' } } });
  env.start({});
  env.emit('showConfiguration', {});
  check(env.openedUrls.length === 1, 'config URL opened');
  var html = decodeURIComponent(env.openedUrls[0] || '');
  check(/Location permission not granted/.test(html), 'last error text is in the settings page');
  check(/Precise/.test(html), 'hint about Precise location is in the settings page');
});

scenario('cadence: watch pings add no fetches when healthy, and back off when failing', function(check) {
  // Two hours of 30-minute interval plus a watch ping every 15 minutes.
  function run(failing) {
    var env = makeEnv();
    if (failing) {
      env.route('https://api.open-meteo.com/v1/forecast', { status: 500, body: 'nope' });
    }
    env.start({ WEATHER_REFRESH_MIN: '30' });
    for (var m = 15; m <= 120; m += 15) {
      env.clock.advance(15 * MIN);
      env.emit('appmessage', { payload: { REFRESH_REQUEST: 1 } });
      env.clock.advance(200);
    }
    return {
      fetches: env.requestsTo('https://api.open-meteo.com').length,
      locations: env.logsMatching(/^Weather refresh \(/).length
    };
  }
  var healthy = run(false);
  var failing = run(true);
  check(healthy.fetches === 5, 'healthy: 5 fetches in 2 h (start + 4 timer ticks), got ' + healthy.fetches);
  check(failing.fetches === 6, 'failing: 6 attempts in 2 h (one 5-min retry, then interval), got ' + failing.fetches);
});

scenario('interval timer still refreshes on its own', function(check) {
  var env = makeEnv();
  env.start({ WEATHER_REFRESH_MIN: '15' });
  env.clock.advance(15 * MIN + 500);
  check(env.requestsTo('https://api.open-meteo.com').length === 2, 'timer refetch at 15 min');
});

// ---------- summary ----------
var failed = results.filter(function(r) { return r.failures.length; });
process.stdout.write('\n' + (results.length - failed.length) + '/' + results.length + ' scenarios passed\n');
process.exit(failed.length ? 1 : 0);
