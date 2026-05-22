// US Tidemaps PebbleKit JS companion
//
// Fetches NOAA tide predictions for the user-configured station and sends
// them to the watch app. Same NOAA endpoints the JY Time tide overlay uses.

var Clay = require('@rebble/clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var keys = require('message_keys');

var TIDE_HOURS_BEFORE_NOW = 24;
var TIDE_WINDOW_HOURS = 48;

var DEFAULT_SETTINGS = {
  TIDE_STATION_ID: '',
  TIDE_UNITS: 'feet',
  REFRESH_MINUTES: 60
};

var refreshIntervalId = null;

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function readSettings() {
  var raw = localStorage.getItem('clay-settings');
  var settings = {};
  if (raw) {
    try {
      settings = JSON.parse(raw);
    } catch (e) {
      settings = {};
    }
  }
  return Object.assign({}, DEFAULT_SETTINGS, settings);
}

function tideStationId(settings) {
  return String(settings.TIDE_STATION_ID || '').trim().slice(0, 15);
}

function tideUnitsId(settings) {
  return settings.TIDE_UNITS === 'meters' ? 1 : 0;
}

function refreshMinutes(settings) {
  var allowed = [30, 60, 120];
  var minutes = Number(settings.REFRESH_MINUTES) || 60;
  for (var i = 0; i < allowed.length; i++) {
    if (allowed[i] === minutes) return minutes;
  }
  return 60;
}

function twoDigit(value) {
  return value < 10 ? '0' + value : String(value);
}

function encodeTideLevelFeet(value) {
  var level = Number(value);
  if (!isFinite(level)) return 0xFF;
  return clamp(Math.round((level + 10) * 8), 0, 255);
}

function noaaTimeKey(date) {
  return date.getUTCFullYear() + '-'
      + twoDigit(date.getUTCMonth() + 1) + '-'
      + twoDigit(date.getUTCDate()) + ' '
      + twoDigit(date.getUTCHours()) + ':00';
}

function parseNoaaTime(value) {
  if (!value) return null;
  var match = String(value).match(/^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2})$/);
  if (!match) return null;
  return new Date(Date.UTC(+match[1], +match[2] - 1, +match[3],
                            +match[4], +match[5], 0));
}

function noaaDateParam(date) {
  return date.getUTCFullYear()
      + twoDigit(date.getUTCMonth() + 1)
      + twoDigit(date.getUTCDate());
}

function tidePredictionsUrl(stationId, interval) {
  // NOAA wants a 3-day window in UTC so a 48-hour packing window (24 past +
  // 24 future) is always fully covered.
  var now = new Date();
  var begin = new Date(now.getTime() - 36 * 60 * 60 * 1000);
  var end   = new Date(now.getTime() + 36 * 60 * 60 * 1000);
  return 'https://api.tidesandcurrents.noaa.gov/api/prod/datagetter'
      + '?begin_date=' + noaaDateParam(begin)
      + '&end_date=' + noaaDateParam(end)
      + '&station=' + encodeURIComponent(stationId)
      + '&product=predictions'
      + '&datum=MLLW'
      + '&time_zone=gmt'
      + '&units=english'
      + '&interval=' + encodeURIComponent(interval)
      + '&format=json';
}

function packTideHourlyLevels(predictions, nowDate) {
  var byHour = {};
  (predictions || []).forEach(function(prediction) {
    if (prediction && prediction.t) {
      byHour[prediction.t] = prediction.v;
    }
  });

  var HOUR_MS = 60 * 60 * 1000;
  var nowMs = nowDate.getTime();
  var nowHourMs = Math.floor(nowMs / HOUR_MS) * HOUR_MS;
  var startMs = nowHourMs - TIDE_HOURS_BEFORE_NOW * HOUR_MS;

  // PebbleKit JS expects a plain Array of integers 0..255 for byte-array
  // tuples. Uint8Array silently tanks the whole AppMessage.
  var levels = [];
  var matched = 0;
  for (var i = 0; i < TIDE_WINDOW_HOURS; i++) {
    var hour = new Date(startMs + i * HOUR_MS);
    var key = noaaTimeKey(hour);
    var value = byHour[key];
    if (typeof value === 'undefined') {
      levels.push(0xFF);
    } else {
      levels.push(encodeTideLevelFeet(value));
      matched++;
    }
  }
  console.log('Tidemaps: packed ' + matched + '/' + TIDE_WINDOW_HOURS
              + ' hourly levels');
  return levels;
}

function findNextTideEvent(predictions, type, nowDate) {
  var nowMs = nowDate.getTime();
  for (var i = 0; i < (predictions || []).length; i++) {
    var prediction = predictions[i];
    if (!prediction || prediction.type !== type) continue;
    var when = parseNoaaTime(prediction.t);
    if (when && when.getTime() > nowMs) {
      return {
        time: Math.floor(when.getTime() / 1000),
        level: encodeTideLevelFeet(prediction.v)
      };
    }
  }
  return { time: 0, level: 0xFF };
}

function fetchJson(url, label, callback) {
  console.log(label + ' GET ' + url.slice(0, 100));
  var xhr = new XMLHttpRequest();
  var finished = false;
  function finishOnce(result) {
    if (finished) return;
    finished = true;
    callback(result);
  }
  xhr.timeout = 15000;
  xhr.onload = function() {
    if (xhr.status && xhr.status !== 200) {
      console.log(label + ' HTTP ' + xhr.status);
      finishOnce(null);
      return;
    }
    try {
      var data = JSON.parse(xhr.responseText);
      if (data && data.error) {
        console.log(label + ' error: ' + JSON.stringify(data.error));
        finishOnce(null);
        return;
      }
      finishOnce(data);
    } catch (e) {
      console.log(label + ' parse failed: ' + e);
      finishOnce(null);
    }
  };
  xhr.onerror = function() {
    console.log(label + ' request failed');
    finishOnce(null);
  };
  xhr.ontimeout = function() {
    console.log(label + ' timeout after 15s');
    finishOnce(null);
  };
  try {
    xhr.open('GET', url);
    xhr.send();
  } catch (e) {
    console.log(label + ' send threw: ' + e);
    finishOnce(null);
  }
}

function tideStationNameFromMetadata(data, stationId) {
  if (!data || !data.stations || !data.stations.length) {
    return stationId;
  }
  var station = data.stations[0] || {};
  var name = station.name || station.publicname || stationId;
  return String(name).toUpperCase().slice(0, 23);
}

function fetchTideStationName(stationId, callback) {
  var cacheKey = 'tidemaps-station-name-' + stationId;
  var cached = localStorage.getItem(cacheKey);
  if (cached) {
    callback(cached.slice(0, 23));
    return;
  }
  var url = 'https://api.tidesandcurrents.noaa.gov/mdapi/prod/webapi/stations/'
      + encodeURIComponent(stationId) + '.json';
  fetchJson(url, 'Tidemaps station metadata', function(data) {
    var name = tideStationNameFromMetadata(data, stationId);
    localStorage.setItem(cacheKey, name);
    callback(name);
  });
}

function sendToWatch(dict, label) {
  Pebble.sendAppMessage(dict, function() {
    console.log(label + ' sent');
  }, function(error) {
    console.log(label + ' send failed: ' + JSON.stringify(error));
  });
}

function sendTideData(settings, hourlyData, hiloData, stationName) {
  if (!hourlyData || !hourlyData.predictions) return;
  var now = new Date();
  var high = findNextTideEvent(hiloData ? hiloData.predictions : [],
                               'H', now);
  var low = findNextTideEvent(hiloData ? hiloData.predictions : [],
                              'L', now);
  var dict = {};
  dict[keys.TIDE_HOURLY_LEVELS] =
      packTideHourlyLevels(hourlyData.predictions, now);
  dict[keys.TIDE_NEXT_HIGH_T] = high.time;
  dict[keys.TIDE_NEXT_HIGH_LEVEL] = high.level;
  dict[keys.TIDE_NEXT_LOW_T] = low.time;
  dict[keys.TIDE_NEXT_LOW_LEVEL] = low.level;
  dict[keys.TIDE_STATION_NAME] =
      String(stationName || tideStationId(settings)).slice(0, 23);
  dict[keys.TIDE_UNITS] = tideUnitsId(settings);
  dict[keys.TIDE_STATION_ID] = tideStationId(settings);
  sendToWatch(dict, 'Tide data');
}

function fetchTidesForStation(stationId, settings, attempt) {
  attempt = attempt || 1;
  console.log('Tidemaps: fetch attempt ' + attempt + ' for station '
              + stationId);
  fetchJson(tidePredictionsUrl(stationId, 'h'), 'Tidemaps hourly',
      function(hourlyData) {
    if (!hourlyData || !hourlyData.predictions
        || !hourlyData.predictions.length) {
      if (attempt < 3) {
        setTimeout(function() {
          fetchTidesForStation(stationId, settings, attempt + 1);
        }, 8000);
      } else {
        console.log('Tidemaps: gave up after 3 hourly attempts');
      }
      return;
    }
    fetchJson(tidePredictionsUrl(stationId, 'hilo'), 'Tidemaps hilo',
        function(hiloData) {
      fetchTideStationName(stationId, function(stationName) {
        sendTideData(settings, hourlyData, hiloData || { predictions: [] },
                     stationName);
      });
    });
  });
}

function refreshTides() {
  var settings = readSettings();
  var stationId = tideStationId(settings);
  if (!stationId) {
    console.log('Tidemaps: no station configured');
    return;
  }
  fetchTidesForStation(stationId, settings);
}

function scheduleRefresh() {
  if (refreshIntervalId) {
    clearInterval(refreshIntervalId);
    refreshIntervalId = null;
  }
  var settings = readSettings();
  var minutes = refreshMinutes(settings);
  refreshIntervalId = setInterval(refreshTides, minutes * 60 * 1000);
  console.log('Tidemaps: refresh scheduled every ' + minutes + ' min');
}

function sendStationConfig() {
  var settings = readSettings();
  var dict = {};
  dict[keys.TIDE_STATION_ID] = tideStationId(settings);
  dict[keys.TIDE_UNITS] = tideUnitsId(settings);
  sendToWatch(dict, 'Tide config');
}

Pebble.addEventListener('ready', function() {
  console.log('Tidemaps PKJS ready');
  sendStationConfig();
  refreshTides();
  scheduleRefresh();
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response) return;
  var messageKeys = clay.getSettings(e.response, false);
  var settings = {};
  Object.keys(messageKeys).forEach(function(k) {
    settings[k] = messageKeys[k];
  });
  // Persist with the same key Clay reads back from on next launch.
  localStorage.setItem('clay-settings', JSON.stringify(settings));
  sendStationConfig();
  refreshTides();
  scheduleRefresh();
});
