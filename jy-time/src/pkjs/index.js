'use strict';

var Clay = require('@rebble/clay');
var clayConfig = require('./config.json');
var keys = require('message_keys');

function customClay() {
  var clayCfg = this;

  var BW_ITEM_KEYS = [
    'bw-heading',
    'INVERT_TOP_BAR',
    'INVERT_DATE_BAR',
    'INVERT_TIME',
    'INVERT_WEATHER',
    'INVERT_MEETING_BAR'
  ];

  var COLOR_ITEM_KEYS = [
    'color-heading',
    'COLOR_SECTION_BG_TOP_BAR',
    'COLOR_SECTION_FG_TOP_BAR',
    'COLOR_SECTION_BG_DATE_BAR',
    'COLOR_SECTION_FG_DATE_BAR',
    'COLOR_SECTION_BG_TIME',
    'COLOR_SECTION_FG_TIME',
    'COLOR_SECTION_BG_WEATHER',
    'COLOR_SECTION_FG_WEATHER',
    'COLOR_SECTION_BG_MEETING_BAR',
    'COLOR_SECTION_FG_MEETING_BAR'
  ];

  var SHAKE_FITNESS_ITEM_KEYS = [
    'shake-fitness-heading',
    'FITNESS_RING_STEPS_ON',
    'FITNESS_RING_ACTIVE_ON',
    'FITNESS_RING_CALORIES_ON',
    'FITNESS_TARGET_STEPS',
    'FITNESS_TARGET_ACTIVE_MIN',
    'FITNESS_TARGET_CALORIES',
    'FITNESS_COLOR_STEPS',
    'FITNESS_COLOR_ACTIVE',
    'FITNESS_COLOR_CALORIES'
  ];

  var SHAKE_CALENDAR_ITEM_KEYS = [
    'shake-calendar-heading',
    'CALENDAR_SHAKE_EVENT_COUNT'
  ];

  var SHAKE_YOURDAY_ITEM_KEYS = [
    'shake-yourday-heading',
    'YOUR_DAY_WINDOW_MODE',
    'YOUR_DAY_WINDOW_HOURS',
    'YOUR_DAY_START_HOUR',
    'YOUR_DAY_END_HOUR',
    'YOUR_DAY_HALF_HOUR_PIPS'
  ];

  var SHAKE_ALTTZ_ITEM_KEYS = [
    'shake-alttz-heading',
    'ALT_TZ_LABEL',
    'ALT_TZ_OFFSET_MIN'
  ];

  var SHAKE_TIDE_ITEM_KEYS = [
    'shake-tide-heading',
    'TIDE_STATION_ID',
    'TIDE_UNITS'
  ];

  function getItem(key) {
    return clayCfg.getItemById(key) || clayCfg.getItemByMessageKey(key);
  }

  function setGroupVisible(keysList, visible) {
    keysList.forEach(function(key) {
      var item = getItem(key);
      if (!item) return;
      if (visible) {
        item.show();
      } else {
        item.hide();
      }
    });
  }

  clayCfg.on(clayCfg.EVENTS.AFTER_BUILD, function() {
    var colorModeItem = clayCfg.getItemByMessageKey('COLOR_MODE');
    var shakeItem = clayCfg.getItemByMessageKey('SHAKE_BEHAVIOR');

    function syncColorMode() {
      var v = colorModeItem.get();
      if (v === 'color') {
        setGroupVisible(BW_ITEM_KEYS, false);
        setGroupVisible(COLOR_ITEM_KEYS, true);
      } else {
        setGroupVisible(BW_ITEM_KEYS, true);
        setGroupVisible(COLOR_ITEM_KEYS, false);
      }
    }

    function syncShake() {
      var v = shakeItem.get();
      setGroupVisible(SHAKE_FITNESS_ITEM_KEYS, v === 'fitness_rings');
      setGroupVisible(SHAKE_CALENDAR_ITEM_KEYS, v === 'calendar_events');
      setGroupVisible(SHAKE_YOURDAY_ITEM_KEYS, v === 'your_day');
      setGroupVisible(SHAKE_ALTTZ_ITEM_KEYS, v === 'alt_timezone');
      setGroupVisible(SHAKE_TIDE_ITEM_KEYS, v === 'tide_chart');
    }

    colorModeItem.on('change', syncColorMode);
    shakeItem.on('change', syncShake);
    syncColorMode();
    syncShake();
  });
}

var clay = new Clay(clayConfig, customClay, { autoHandleEvents: false });
var weatherTimer = null;
var calendarTimer = null;
var CURRENT_EVENT_DISPLAY_MINUTES = 15;
var messageQueue = [];
var messageInFlight = false;

var DEFAULT_SETTINGS = {
  COLOR_MODE: 'bw',
  COLOR_SECTION_BG_TOP_BAR: 0,
  COLOR_SECTION_FG_TOP_BAR: 16777215,
  COLOR_SECTION_BG_DATE_BAR: 0,
  COLOR_SECTION_FG_DATE_BAR: 16777215,
  COLOR_SECTION_BG_TIME: 0,
  COLOR_SECTION_FG_TIME: 16777215,
  COLOR_SECTION_BG_WEATHER: 0,
  COLOR_SECTION_FG_WEATHER: 16777215,
  COLOR_SECTION_BG_MEETING_BAR: 0,
  COLOR_SECTION_FG_MEETING_BAR: 16777215,
  LIGHT_MODE: false,
  INVERT_TOP_BAR: false,
  INVERT_DATE_BAR: false,
  INVERT_TIME: false,
  MILITARY_TIME: false,
  REMOVE_LEADING_ZERO: false,
  INVERT_WEATHER: false,
  INVERT_MEETING_BAR: false,
  TOP_STEPS: true,
  VERBOSE_WEATHER: false,
  VERBOSE_WEATHER_STYLE: 'one_line',
  COMPLICATION_1: 'weather',
  COMPLICATION_2: 'rain',
  COMPLICATION_3: 'heart_rate',
  WEATHER_ENABLED: true,
  WEATHER_SOURCE: 'gps',
  WEATHER_LAT: '',
  WEATHER_LON: '',
  TEMPERATURE_UNIT: 'fahrenheit',
  WEATHER_REFRESH_MIN: '30',
  CALENDAR_ENABLED: false,
  CALENDAR_ICS_URL: '',
  CALENDAR_ICS_URL_2: '',
  CALENDAR_LOOKAHEAD_HOURS: '48',
  EMPTY_EVENT_LABEL: '[None]',
  SHAKE_BEHAVIOR: 'off',
  CALENDAR_SHAKE_EVENT_COUNT: '3',
  FITNESS_RING_STEPS_ON: true,
  FITNESS_RING_ACTIVE_ON: true,
  FITNESS_RING_CALORIES_ON: true,
  FITNESS_TARGET_STEPS: '10000',
  FITNESS_TARGET_ACTIVE_MIN: '30',
  FITNESS_TARGET_CALORIES: '500',
  FITNESS_COLOR_STEPS: 0x00FF00,
  FITNESS_COLOR_ACTIVE: 0x00AAFF,
  FITNESS_COLOR_CALORIES: 0xFF0000,
  FITNESS_OVERLAY_DURATION_S: '5',
  ALT_TZ_LABEL: 'LONDON',
  ALT_TZ_OFFSET_MIN: '0',
  YOUR_DAY_WINDOW_MODE: 'rolling',
  YOUR_DAY_WINDOW_HOURS: '10',
  YOUR_DAY_START_HOUR: '8',
  YOUR_DAY_END_HOUR: '17',
  YOUR_DAY_HALF_HOUR_PIPS: false,
  TIDE_STATION_ID: '',
  TIDE_UNITS: 'feet'
};

var COMPLICATION_IDS = {
  temperature: 0,
  rain: 1,
  heart_rate: 2,
  steps: 3,
  watch_battery: 4,
  feels_like: 6,
  high_temp: 7,
  wind: 8,
  uv: 9,
  next_event: 10,
  weather: 11,
  weather_icon: 12,
  fitness_rings: 13,
  weather_circle: 14,
  battery_ring: 15,
  battery_circle: 15,
  high_low_combined: 16,
  active_minutes: 17,
  active_calories: 18,
  sleep_last_night: 19,
  distance_today: 20
};

var SHAKE_BEHAVIOR_IDS = {
  off: 0,
  fitness_rings: 1,
  calendar_events: 2,
  your_day: 3,
  detailed_weather: 4,
  alt_timezone: 5,
  heart_rate: 6,
  tide_chart: 8
};

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function isNumber(value) {
  return typeof value === 'number' && isFinite(value);
}

function complicationId(value, fallback) {
  return typeof COMPLICATION_IDS[value] !== 'undefined'
      ? COMPLICATION_IDS[value]
      : fallback;
}

function shakeBehaviorId(value) {
  return typeof SHAKE_BEHAVIOR_IDS[value] !== 'undefined'
      ? SHAKE_BEHAVIOR_IDS[value]
      : SHAKE_BEHAVIOR_IDS.off;
}

function colorModeId(value) {
  return value === 'color' ? 1 : 0;
}

function yourDayWindowModeId(value) {
  return value === 'fixed' ? 1 : 0;
}

function temperatureUnit(settings) {
  return settings.TEMPERATURE_UNIT === 'celsius' ? 'celsius' : 'fahrenheit';
}

function numberSetting(value, fallback, min, max) {
  var parsed = Number(value);
  if (!isFinite(parsed)) {
    parsed = fallback;
  }
  return clamp(Math.round(parsed), min, max);
}

function colorSetting(value, fallback) {
  return numberSetting(value, fallback, 0, 0xFFFFFF);
}

function readSettings() {
  var saved = {};
  try {
    saved = JSON.parse(localStorage.getItem('clay-settings')) || {};
  } catch (e) {
    console.log('Settings parse failed: ' + e);
  }

  var settings = {};
  Object.keys(DEFAULT_SETTINGS).forEach(function(key) {
    settings[key] = typeof saved[key] !== 'undefined' ? saved[key] : DEFAULT_SETTINGS[key];
  });
  return settings;
}

function sendNextMessage() {
  if (messageInFlight || !messageQueue.length) {
    return;
  }

  var message = messageQueue.shift();
  messageInFlight = true;
  Pebble.sendAppMessage(message.dict, function() {
    console.log((message.label || 'Message') + ' sent');
    messageInFlight = false;
    sendNextMessage();
  }, function(error) {
    console.log((message.label || 'Message') + ' failed: ' + JSON.stringify(error));
    messageInFlight = false;
    sendNextMessage();
  });
}

function sendToWatch(dict, label) {
  messageQueue.push({
    dict: dict,
    label: label
  });
  sendNextMessage();
}

function sendLayoutSetting(settings) {
  var dict = {};
  dict[keys.LIGHT_MODE] = settings.LIGHT_MODE ? 1 : 0;
  dict[keys.INVERT_TOP_BAR] = settings.INVERT_TOP_BAR ? 1 : 0;
  dict[keys.INVERT_DATE_BAR] = settings.INVERT_DATE_BAR ? 1 : 0;
  dict[keys.INVERT_TIME] = settings.INVERT_TIME ? 1 : 0;
  dict[keys.MILITARY_TIME] = settings.MILITARY_TIME ? 1 : 0;
  dict[keys.REMOVE_LEADING_ZERO] = settings.REMOVE_LEADING_ZERO ? 1 : 0;
  dict[keys.INVERT_WEATHER] = settings.INVERT_WEATHER ? 1 : 0;
  dict[keys.INVERT_MEETING_BAR] = settings.INVERT_MEETING_BAR ? 1 : 0;
  dict[keys.TOP_STEPS] = settings.TOP_STEPS ? 1 : 0;
  dict[keys.VERBOSE_WEATHER] = settings.VERBOSE_WEATHER ? 1 : 0;
  dict[keys.VERBOSE_WEATHER_STYLE] = settings.VERBOSE_WEATHER_STYLE === 'large' ? 1 : 0;
  dict[keys.COMPLICATION_1] = complicationId(settings.COMPLICATION_1, COMPLICATION_IDS.weather);
  dict[keys.COMPLICATION_2] = complicationId(settings.COMPLICATION_2, COMPLICATION_IDS.rain);
  dict[keys.COMPLICATION_3] = complicationId(settings.COMPLICATION_3, COMPLICATION_IDS.heart_rate);
  dict[keys.TEMPERATURE_UNIT] = temperatureUnit(settings) === 'celsius' ? 1 : 0;
  var emptyLabel = String(settings.EMPTY_EVENT_LABEL || '').trim();
  if (!emptyLabel) emptyLabel = '[None]';
  dict[keys.EMPTY_EVENT_LABEL] = emptyLabel.slice(0, 24);
  sendToWatch(dict, 'Layout setting');
}

function sendColorSetting(settings) {
  var dict = {};
  dict[keys.COLOR_MODE] = colorModeId(settings.COLOR_MODE);
  dict[keys.COLOR_SECTION_BG_TOP_BAR] = colorSetting(settings.COLOR_SECTION_BG_TOP_BAR, 0);
  dict[keys.COLOR_SECTION_FG_TOP_BAR] = colorSetting(settings.COLOR_SECTION_FG_TOP_BAR, 16777215);
  dict[keys.COLOR_SECTION_BG_DATE_BAR] = colorSetting(settings.COLOR_SECTION_BG_DATE_BAR, 0);
  dict[keys.COLOR_SECTION_FG_DATE_BAR] = colorSetting(settings.COLOR_SECTION_FG_DATE_BAR, 16777215);
  dict[keys.COLOR_SECTION_BG_TIME] = colorSetting(settings.COLOR_SECTION_BG_TIME, 0);
  dict[keys.COLOR_SECTION_FG_TIME] = colorSetting(settings.COLOR_SECTION_FG_TIME, 16777215);
  dict[keys.COLOR_SECTION_BG_WEATHER] = colorSetting(settings.COLOR_SECTION_BG_WEATHER, 0);
  dict[keys.COLOR_SECTION_FG_WEATHER] = colorSetting(settings.COLOR_SECTION_FG_WEATHER, 16777215);
  dict[keys.COLOR_SECTION_BG_MEETING_BAR] =
      colorSetting(settings.COLOR_SECTION_BG_MEETING_BAR, 0);
  dict[keys.COLOR_SECTION_FG_MEETING_BAR] =
      colorSetting(settings.COLOR_SECTION_FG_MEETING_BAR, 16777215);
  sendToWatch(dict, 'Color setting');
}

function sendShakeSetting(settings) {
  var dict = {};
  dict[keys.SHAKE_BEHAVIOR] = shakeBehaviorId(settings.SHAKE_BEHAVIOR);
  dict[keys.FITNESS_RING_STEPS_ON] = settings.FITNESS_RING_STEPS_ON ? 1 : 0;
  dict[keys.FITNESS_RING_ACTIVE_ON] = settings.FITNESS_RING_ACTIVE_ON ? 1 : 0;
  dict[keys.FITNESS_RING_CALORIES_ON] = settings.FITNESS_RING_CALORIES_ON ? 1 : 0;
  dict[keys.FITNESS_TARGET_STEPS] = numberSetting(settings.FITNESS_TARGET_STEPS, 10000, 1, 999999);
  dict[keys.FITNESS_TARGET_ACTIVE_MIN] = numberSetting(settings.FITNESS_TARGET_ACTIVE_MIN, 30, 1, 9999);
  dict[keys.FITNESS_TARGET_CALORIES] = numberSetting(settings.FITNESS_TARGET_CALORIES, 500, 1, 999999);
  dict[keys.FITNESS_COLOR_STEPS] = numberSetting(settings.FITNESS_COLOR_STEPS, 0x00FF00, 0, 0xFFFFFF);
  dict[keys.FITNESS_COLOR_ACTIVE] = numberSetting(settings.FITNESS_COLOR_ACTIVE, 0x00AAFF, 0, 0xFFFFFF);
  dict[keys.FITNESS_COLOR_CALORIES] = numberSetting(settings.FITNESS_COLOR_CALORIES, 0xFF0000, 0, 0xFFFFFF);
  dict[keys.FITNESS_OVERLAY_DURATION_S] = numberSetting(settings.FITNESS_OVERLAY_DURATION_S, 5, 3, 30);
  dict[keys.CALENDAR_SHAKE_EVENT_COUNT] = numberSetting(settings.CALENDAR_SHAKE_EVENT_COUNT, 3, 3, 5) >= 5 ? 5 : 3;
  dict[keys.ALT_TZ_LABEL] = String(settings.ALT_TZ_LABEL || 'LONDON').slice(0, 16);
  dict[keys.ALT_TZ_OFFSET_MIN] = numberSetting(settings.ALT_TZ_OFFSET_MIN, 0, -720, 840);
  dict[keys.YOUR_DAY_WINDOW_MODE] = yourDayWindowModeId(settings.YOUR_DAY_WINDOW_MODE);
  dict[keys.YOUR_DAY_WINDOW_HOURS] = numberSetting(settings.YOUR_DAY_WINDOW_HOURS, 10, 2, 10);
  dict[keys.YOUR_DAY_START_HOUR] = numberSetting(settings.YOUR_DAY_START_HOUR, 8, 0, 23);
  dict[keys.YOUR_DAY_END_HOUR] = numberSetting(settings.YOUR_DAY_END_HOUR, 17, 0, 23);
  dict[keys.YOUR_DAY_HALF_HOUR_PIPS] = settings.YOUR_DAY_HALF_HOUR_PIPS ? 1 : 0;
  sendToWatch(dict, 'Shake setting');
}

function tideStationId(settings) {
  return String(settings.TIDE_STATION_ID || '').trim().slice(0, 15);
}

function tideUnitsId(settings) {
  return settings.TIDE_UNITS === 'meters' ? 1 : 0;
}

function sendTideSetting(settings) {
  var dict = {};
  dict[keys.TIDE_STATION_ID] = tideStationId(settings);
  dict[keys.TIDE_UNITS] = tideUnitsId(settings);
  sendToWatch(dict, 'Tide setting');
  refreshTidesForSettings(settings);
}

function encodeTideLevelFeet(value) {
  var level = Number(value);
  if (!isFinite(level)) {
    return 0xFF;
  }
  return clamp(Math.round((level + 10) * 8), 0, 255);
}

function twoDigit(value) {
  return value < 10 ? '0' + value : String(value);
}

// Tide hourly levels are sent as a 24-byte array centered on "now":
// indices 0..11 are the past 12 hours, index TIDE_NOW_INDEX = 12 is the
// current hour, indices 13..23 are the next 11 hours. The C side knows
// where "now" sits by the same TIDE_NOW_INDEX constant.
var TIDE_HOURS_BEFORE_NOW = 12;
var TIDE_WINDOW_HOURS = 24;

function noaaTimeKey(date) {
  // UTC components — keys must be TZ-independent so they line up with NOAA's
  // gmt-formatted predictions regardless of which TZ the user's phone is in
  // OR which TZ their chosen station is in.
  return date.getUTCFullYear() + '-'
      + twoDigit(date.getUTCMonth() + 1) + '-'
      + twoDigit(date.getUTCDate()) + ' '
      + twoDigit(date.getUTCHours()) + ':00';
}

function parseNoaaTime(value) {
  if (!value) {
    return null;
  }
  // NOAA returns "YYYY-MM-DD HH:MM" in GMT (we requested time_zone=gmt).
  // PebbleKit JS's ISO string parsing has been historically unreliable, so
  // construct via Date.UTC with explicit numeric components instead of any
  // string parsing. Pattern lifted from the working community tide watchface.
  var match = String(value).match(/^(\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2})$/);
  if (!match) {
    return null;
  }
  var utcMs = Date.UTC(
    parseInt(match[1], 10),
    parseInt(match[2], 10) - 1,
    parseInt(match[3], 10),
    parseInt(match[4], 10),
    parseInt(match[5], 10),
    0
  );
  return isFinite(utcMs) ? new Date(utcMs) : null;
}

function packTideHourlyLevels(predictions, nowDate) {
  var byHour = {};
  (predictions || []).forEach(function(prediction) {
    if (prediction && prediction.t) {
      byHour[prediction.t] = prediction.v;
    }
  });

  // Center the 24-hour window on "now". All math in epoch ms — no mutating
  // setUTCMinutes calls (some embedded JS engines fail those silently).
  var HOUR_MS = 60 * 60 * 1000;
  var nowMs = nowDate.getTime();
  var nowHourMs = Math.floor(nowMs / HOUR_MS) * HOUR_MS;
  var startMs = nowHourMs - TIDE_HOURS_BEFORE_NOW * HOUR_MS;

  // PebbleKit JS serializes byte-array tuples from a plain Array of integers
  // (0..255). Uint8Array fails to round-trip and tanks the whole AppMessage,
  // which breaks every other key in the same dict.
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
  console.log('Tide: packed ' + matched + '/' + TIDE_WINDOW_HOURS + ' hourly levels');
  return levels;
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
      console.log(label + ' ok (' + (xhr.responseText ? xhr.responseText.length : 0) + ' bytes)');
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

function noaaDateParam(date) {
  return date.getUTCFullYear() + twoDigit(date.getUTCMonth() + 1) + twoDigit(date.getUTCDate());
}

function tidePredictionsUrl(stationId, interval) {
  // begin_date / end_date span a 3-day window in UTC: yesterday, today, tomorrow.
  // NOAA ignores &range when paired with &date=today (only returns 24 entries),
  // but respects it with begin_date. Using explicit begin/end is the most
  // predictable. UTC throughout so the keys line up with our UTC matching.
  var now = new Date();
  var begin = new Date(now.getTime() - 24 * 60 * 60 * 1000);
  var end   = new Date(now.getTime() + 24 * 60 * 60 * 1000);
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

function tideStationNameFromMetadata(data, stationId) {
  if (!data || !data.stations || !data.stations.length) {
    return stationId;
  }
  var station = data.stations[0] || {};
  var name = station.name || station.publicname || stationId;
  return String(name).toUpperCase().slice(0, 23);
}

function fetchTideStationName(stationId, callback) {
  var cacheKey = 'tide-station-name-' + stationId;
  var cached = localStorage.getItem(cacheKey);
  if (cached) {
    callback(cached.slice(0, 23));
    return;
  }

  var url = 'https://api.tidesandcurrents.noaa.gov/mdapi/prod/webapi/stations/'
      + encodeURIComponent(stationId) + '.json';
  fetchJson(url, 'Tide station metadata', function(data) {
    var name = tideStationNameFromMetadata(data, stationId);
    localStorage.setItem(cacheKey, name);
    callback(name);
  });
}

function findNextTideEvent(predictions, type, nowDate) {
  var nowMs = nowDate.getTime();
  for (var i = 0; i < (predictions || []).length; i++) {
    var prediction = predictions[i];
    if (!prediction || prediction.type !== type) {
      continue;
    }
    var when = parseNoaaTime(prediction.t);
    if (when && when.getTime() > nowMs) {
      return {
        time: Math.floor(when.getTime() / 1000),
        level: encodeTideLevelFeet(prediction.v)
      };
    }
  }
  return {
    time: 0,
    level: 0xFF
  };
}

function sendTideData(settings, hourlyData, hiloData, stationName) {
  if (!hourlyData || !hourlyData.predictions || !hiloData || !hiloData.predictions) {
    return;
  }

  var now = new Date();
  var high = findNextTideEvent(hiloData.predictions, 'H', now);
  var low = findNextTideEvent(hiloData.predictions, 'L', now);
  var dict = {};
  dict[keys.TIDE_HOURLY_LEVELS] = packTideHourlyLevels(hourlyData.predictions, now);
  dict[keys.TIDE_NEXT_HIGH_T] = high.time;
  dict[keys.TIDE_NEXT_HIGH_LEVEL] = high.level;
  dict[keys.TIDE_NEXT_LOW_T] = low.time;
  dict[keys.TIDE_NEXT_LOW_LEVEL] = low.level;
  dict[keys.TIDE_STATION_NAME] = String(stationName || tideStationId(settings)).slice(0, 23);
  dict[keys.TIDE_UNITS] = tideUnitsId(settings);
  dict[keys.TIDE_STATION_ID] = tideStationId(settings);
  sendToWatch(dict, 'Tide data');
}

function fetchTidesForStation(stationId, settings, attempt) {
  attempt = attempt || 1;
  console.log('Tide: fetch attempt ' + attempt + ' for station ' + stationId);
  fetchJson(tidePredictionsUrl(stationId, 'h'), 'Tide hourly', function(hourlyData) {
    if (!hourlyData || !hourlyData.predictions || !hourlyData.predictions.length) {
      console.log('Tide: hourly empty (attempt ' + attempt + ')');
      if (attempt < 3) {
        setTimeout(function() { fetchTidesForStation(stationId, settings, attempt + 1); }, 8000);
      } else {
        console.log('Tide: gave up after 3 hourly attempts');
      }
      return;
    }
    console.log('Tide: hourly got ' + hourlyData.predictions.length + ' predictions');
    fetchJson(tidePredictionsUrl(stationId, 'hilo'), 'Tide hilo', function(hiloData) {
      if (!hiloData || !hiloData.predictions || !hiloData.predictions.length) {
        console.log('Tide: hilo empty — sending hourly-only data');
        fetchTideStationName(stationId, function(stationName) {
          sendTideData(settings, hourlyData, { predictions: [] }, stationName);
        });
        return;
      }
      console.log('Tide: hilo got ' + hiloData.predictions.length + ' events');
      fetchTideStationName(stationId, function(stationName) {
        sendTideData(settings, hourlyData, hiloData, stationName);
      });
    });
  });
}

function refreshTidesForSettings(settings) {
  var stationId = tideStationId(settings);
  if (!stationId) {
    console.log('Tide: no station configured, skipping fetch');
    return;
  }
  fetchTidesForStation(stationId, settings);
}

function nearestRainChance(hourly) {
  if (!hourly || !hourly.time || !hourly.precipitation_probability) {
    return 0;
  }

  var now = Date.now();
  var bestIndex = 0;
  var bestDistance = Infinity;
  hourly.time.forEach(function(timeValue, index) {
    var distance = Math.abs(new Date(timeValue).getTime() - now);
    if (distance < bestDistance) {
      bestDistance = distance;
      bestIndex = index;
    }
  });

  return clamp(Math.round(hourly.precipitation_probability[bestIndex] || 0), 0, 100);
}

function nearestHourlyIndex(hourly) {
  if (!hourly || !hourly.time || !hourly.time.length) {
    return -1;
  }

  var now = Date.now();
  var bestIndex = 0;
  var bestDistance = Infinity;
  hourly.time.forEach(function(timeValue, index) {
    var distance = Math.abs(new Date(timeValue).getTime() - now);
    if (distance < bestDistance) {
      bestDistance = distance;
      bestIndex = index;
    }
  });
  return bestIndex;
}

function weatherBaseLabel(code) {
  if (code === 0) return 'CLEAR';
  if (code === 1) return 'MOSTLY CLR';
  if (code === 2) return 'PARTLY CLDY';
  if (code === 3) return 'CLOUDY';
  if (code === 45 || code === 48) return 'FOG';
  if (code >= 51 && code <= 55) return 'DRIZZLE';
  if (code === 56 || code === 57) return 'FRZ DRZL';
  if (code >= 61 && code <= 65) return 'RAIN';
  if (code === 66 || code === 67) return 'FRZ RAIN';
  if ((code >= 71 && code <= 77) || code === 85 || code === 86) return 'SNOW';
  if (code >= 80 && code <= 82) return 'SHWRS';
  if (code >= 95 && code <= 99) return 'STORMS';
  return 'CLOUDY';
}

function weatherEventLabel(code) {
  if (code >= 95 && code <= 99) return 'STORMS';
  if ((code >= 71 && code <= 77) || code === 85 || code === 86) return 'SNOW';
  if (code >= 80 && code <= 82) return 'SHWRS';
  if (code === 66 || code === 67) return 'FRZ RAIN';
  if (code >= 61 && code <= 65) return 'RAIN';
  if (code === 56 || code === 57) return 'FRZ DRZL';
  if (code >= 51 && code <= 55) return 'DRIZZLE';
  return null;
}

function formatWeatherHour(date) {
  var hours = date.getHours();
  var suffix = hours >= 12 ? 'P' : 'A';
  var hour = hours % 12 || 12;
  return hour + suffix;
}

function isSameLocalDay(a, b) {
  return a.getFullYear() === b.getFullYear()
      && a.getMonth() === b.getMonth()
      && a.getDate() === b.getDate();
}

function hourlyWeatherCode(hourly, index) {
  if (!hourly || !hourly.weather_code || index < 0 || index >= hourly.weather_code.length) {
    return null;
  }
  var code = Number(hourly.weather_code[index]);
  return isFinite(code) ? code : null;
}

function hourlyRainChance(hourly, index) {
  if (!hourly || !hourly.precipitation_probability ||
      index < 0 || index >= hourly.precipitation_probability.length) {
    return 0;
  }
  var value = Number(hourly.precipitation_probability[index]);
  return isFinite(value) ? clamp(Math.round(value), 0, 100) : 0;
}

function significantWeatherEvent(hourly, index) {
  var code = hourlyWeatherCode(hourly, index);
  if (code === null) {
    return null;
  }

  var label = weatherEventLabel(code);
  if (!label) {
    return null;
  }

  var rainChance = hourlyRainChance(hourly, index);
  return rainChance >= 30 || code >= 71 ? label : null;
}

function verboseWeatherSummary(hourly, currentCode) {
  var dryHoursRequired = 3;
  var fallback = weatherBaseLabel(Number(currentCode));
  var startIndex = nearestHourlyIndex(hourly);
  if (startIndex < 0 || !hourly.time) {
    return fallback;
  }

  var now = new Date();
  var currentEvent = weatherEventLabel(Number(currentCode)) ||
      significantWeatherEvent(hourly, startIndex);

  if (currentEvent) {
    var dryRun = 0;
    var firstDryIndex = -1;
    for (var i = startIndex + 1; i < hourly.time.length; i++) {
      var stopDate = new Date(hourly.time[i]);
      if (!isSameLocalDay(now, stopDate)) {
        return currentEvent + ' ALL DAY';
      }
      if (!significantWeatherEvent(hourly, i)) {
        if (dryRun === 0) {
          firstDryIndex = i;
        }
        dryRun++;
        if (dryRun >= dryHoursRequired) {
          return currentEvent + ' TIL ' + formatWeatherHour(new Date(hourly.time[firstDryIndex]));
        }
      } else {
        dryRun = 0;
        firstDryIndex = -1;
      }
    }
    return currentEvent + ' ALL DAY';
  }

  var latest = now.getTime() + (18 * 60 * 60 * 1000);
  for (var j = startIndex + 1; j < hourly.time.length; j++) {
    var eventDate = new Date(hourly.time[j]);
    if (eventDate.getTime() > latest) {
      break;
    }
    var nextEvent = significantWeatherEvent(hourly, j);
    if (nextEvent) {
      return nextEvent + ' AT ' + formatWeatherHour(eventDate);
    }
  }

  return fallback;
}

function firstDailyValue(daily, field) {
  if (!daily || !daily[field] || !daily[field].length) {
    return null;
  }
  var value = Number(daily[field][0]);
  return isFinite(value) ? value : null;
}

function firstDailyTimestamp(daily, field) {
  if (!daily || !daily[field] || !daily[field].length) {
    return null;
  }
  var timestamp = Math.floor(new Date(daily[field][0]).getTime() / 1000);
  return isFinite(timestamp) ? timestamp : null;
}

function addRounded(dict, key, value, min, max) {
  if (isNumber(value)) {
    dict[key] = clamp(Math.round(value), min, max);
  }
}

function fetchWeatherForCoordinates(lat, lon, unit, done) {
  var unitParam = unit === 'celsius' ? 'celsius' : 'fahrenheit';
  var url = 'https://api.open-meteo.com/v1/forecast'
      + '?latitude=' + encodeURIComponent(lat)
      + '&longitude=' + encodeURIComponent(lon)
      + '&current=temperature_2m,apparent_temperature,weather_code,wind_speed_10m'
      + '&hourly=precipitation_probability,weather_code'
      + '&daily=temperature_2m_max,temperature_2m_min,uv_index_max,sunrise,sunset'
      + '&temperature_unit=' + encodeURIComponent(unitParam)
      + '&wind_speed_unit=mph'
      + '&forecast_days=1'
      + '&timezone=auto';

  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    try {
      var data = JSON.parse(xhr.responseText);
      if (!data.current) {
        if (done) done();
        return;
      }

      var dict = {};
      addRounded(dict, keys.TEMPERATURE, data.current.temperature_2m, -99, 127);
      addRounded(dict, keys.FEELS_LIKE, data.current.apparent_temperature, -99, 127);
      addRounded(dict, keys.WEATHER_CODE, data.current.weather_code, 0, 255);
      addRounded(dict, keys.WIND_SPEED, data.current.wind_speed_10m, 0, 255);
      addRounded(dict, keys.HIGH_TEMP, firstDailyValue(data.daily, 'temperature_2m_max'), -99, 127);
      addRounded(dict, keys.LOW_TEMP, firstDailyValue(data.daily, 'temperature_2m_min'), -99, 127);
      addRounded(dict, keys.UV_INDEX, firstDailyValue(data.daily, 'uv_index_max'), 0, 255);
      addRounded(dict, keys.SUNRISE_T, firstDailyTimestamp(data.daily, 'sunrise'), 0, 2147483647);
      addRounded(dict, keys.SUNSET_T, firstDailyTimestamp(data.daily, 'sunset'), 0, 2147483647);
      dict[keys.RAIN_CHANCE] = nearestRainChance(data.hourly);
      dict[keys.WEATHER_SUMMARY] = verboseWeatherSummary(data.hourly, data.current.weather_code);
      sendToWatch(dict, 'Weather');
      if (done) done();
    } catch (e) {
      console.log('Weather parse failed: ' + e);
      if (done) done();
    }
  };
  xhr.onerror = function() {
    console.log('Weather request failed');
    if (done) done();
  };
  xhr.open('GET', url);
  xhr.send();
}

function refreshWeather(skipTide) {
  var settings = readSettings();
  var refreshTide = function() {
    if (!skipTide) {
      refreshTidesForSettings(settings);
    }
  };

  if (!settings.WEATHER_ENABLED) {
    refreshTide();
    return;
  }

  var manualLat = parseFloat(settings.WEATHER_LAT);
  var manualLon = parseFloat(settings.WEATHER_LON);
  var hasManual = isFinite(manualLat) && isFinite(manualLon);
  var unit = temperatureUnit(settings);

  if (settings.WEATHER_SOURCE === 'manual' && hasManual) {
    fetchWeatherForCoordinates(manualLat, manualLon, unit, refreshTide);
    return;
  }

  navigator.geolocation.getCurrentPosition(function(position) {
    fetchWeatherForCoordinates(position.coords.latitude, position.coords.longitude, unit,
        refreshTide);
  }, function(error) {
    console.log('Location failed: ' + JSON.stringify(error));
    if (hasManual) {
      fetchWeatherForCoordinates(manualLat, manualLon, unit, refreshTide);
    } else {
      refreshTide();
    }
  }, {
    enableHighAccuracy: false,
    timeout: 15000,
    maximumAge: 30 * 60 * 1000
  });
}

function unfoldIcs(text) {
  return text.replace(/\r\n[ \t]/g, '').replace(/\n[ \t]/g, '').split(/\r?\n/);
}

function parseIcsProperty(line) {
  var colon = line.indexOf(':');
  var meta = colon === -1 ? line : line.slice(0, colon);
  var value = colon === -1 ? '' : line.slice(colon + 1);
  var parts = meta.split(';');
  var name = parts.shift() || '';
  var params = {};

  parts.forEach(function(part) {
    var equals = part.indexOf('=');
    if (equals !== -1) {
      params[part.slice(0, equals).toUpperCase()] = part.slice(equals + 1).replace(/^"|"$/g, '');
    }
  });

  return {
    name: name.toUpperCase(),
    meta: meta,
    value: value,
    params: params
  };
}

function parseIcsDateComponents(value) {
  var match = value.match(/^(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})(\d{2})(Z?)$/);
  if (!match) {
    return null;
  }

  return {
    year: Number(match[1]),
    month: Number(match[2]),
    day: Number(match[3]),
    hour: Number(match[4]),
    minute: Number(match[5]),
    second: Number(match[6]),
    isUtc: match[7] === 'Z'
  };
}

function nthWeekdayOfMonth(year, monthIndex, weekday, nth) {
  var firstWeekday = new Date(Date.UTC(year, monthIndex, 1)).getUTCDay();
  return 1 + ((weekday - firstWeekday + 7) % 7) + ((nth - 1) * 7);
}

function localComponentTime(components) {
  return Date.UTC(components.year, components.month - 1, components.day,
      components.hour || 0, components.minute || 0, components.second || 0);
}

function isUsDst(components) {
  var startDay = nthWeekdayOfMonth(components.year, 2, 0, 2);
  var endDay = nthWeekdayOfMonth(components.year, 10, 0, 1);
  var current = localComponentTime(components);
  var start = Date.UTC(components.year, 2, startDay, 2, 0, 0);
  var end = Date.UTC(components.year, 10, endDay, 2, 0, 0);
  return current >= start && current < end;
}

function isSydneyDst(components) {
  var current = localComponentTime(components);
  var year = components.year;
  var month = components.month;

  if (month <= 3 || month >= 11) {
    return true;
  }
  if (month >= 5 && month <= 9) {
    return false;
  }

  if (month === 4) {
    var endDay = nthWeekdayOfMonth(year, 3, 0, 1);
    return current < Date.UTC(year, 3, endDay, 3, 0, 0);
  }

  var startDay = nthWeekdayOfMonth(year, 9, 0, 1);
  return current >= Date.UTC(year, 9, startDay, 2, 0, 0);
}

function normalizeTzid(tzid) {
  return (tzid || '').replace(/^"|"$/g, '');
}

function timezoneOffsetMinutes(tzid, components) {
  tzid = normalizeTzid(tzid);
  if (!tzid) {
    return null;
  }
  if (tzid === 'UTC' || tzid === 'Etc/UTC' || tzid === 'GMT' || tzid === 'Z') {
    return 0;
  }
  if (tzid === 'America/New_York') {
    return isUsDst(components) ? -240 : -300;
  }
  if (tzid === 'America/Chicago') {
    return isUsDst(components) ? -300 : -360;
  }
  if (tzid === 'America/Denver') {
    return isUsDst(components) ? -360 : -420;
  }
  if (tzid === 'America/Phoenix') {
    return -420;
  }
  if (tzid === 'America/Los_Angeles') {
    return isUsDst(components) ? -420 : -480;
  }
  if (tzid === 'Australia/Sydney') {
    return isSydneyDst(components) ? 660 : 600;
  }
  return null;
}

function dateFromIcsComponents(components, tzid) {
  if (components.isUtc) {
    return new Date(Date.UTC(
      components.year, components.month - 1, components.day,
      components.hour, components.minute, components.second
    ));
  }

  var offset = timezoneOffsetMinutes(tzid, components);
  if (offset !== null) {
    return new Date(Date.UTC(
      components.year, components.month - 1, components.day,
      components.hour, components.minute, components.second
    ) - (offset * 60 * 1000));
  }

  return new Date(
    components.year, components.month - 1, components.day,
    components.hour, components.minute, components.second
  );
}

function parseIcsDateInfo(line) {
  var property = parseIcsProperty(line);
  if (!property.value) {
    return null;
  }

  if (property.meta.indexOf('VALUE=DATE') !== -1 || /^\d{8}$/.test(property.value)) {
    return null;
  }

  var components = parseIcsDateComponents(property.value);
  if (!components) {
    return null;
  }

  var tzid = normalizeTzid(property.params.TZID);
  return {
    date: dateFromIcsComponents(components, tzid),
    components: components,
    tzid: tzid
  };
}

function parseIcsDate(line) {
  var info = parseIcsDateInfo(line);
  return info ? info.date : null;
}

function unescapeIcsText(value) {
  return value
    .replace(/\\n/g, ' ')
    .replace(/\\,/g, ',')
    .replace(/\\;/g, ';')
    .replace(/\\\\/g, '\\')
    .trim();
}

function parseRRule(line) {
  var property = parseIcsProperty(line);
  var rule = {};
  property.value.split(';').forEach(function(part) {
    var equals = part.indexOf('=');
    if (equals !== -1) {
      rule[part.slice(0, equals).toUpperCase()] = part.slice(equals + 1);
    }
  });
  return rule;
}

function weekdayIndex(value) {
  var code = (value || '').slice(-2);
  var days = ['SU', 'MO', 'TU', 'WE', 'TH', 'FR', 'SA'];
  return days.indexOf(code);
}

function localDateOnly(components) {
  return Date.UTC(components.year, components.month - 1, components.day);
}

function componentsFromDateOnly(dateOnly) {
  var date = new Date(dateOnly);
  return {
    year: date.getUTCFullYear(),
    month: date.getUTCMonth() + 1,
    day: date.getUTCDate()
  };
}

function localWeekday(components) {
  return new Date(Date.UTC(components.year, components.month - 1, components.day)).getUTCDay();
}

function startOfWeekDateOnly(components, weekStart) {
  var diff = (localWeekday(components) - weekStart + 7) % 7;
  return localDateOnly(components) - (diff * 24 * 60 * 60 * 1000);
}

function parseByDay(rule, fallbackComponents) {
  var days = [];
  if (rule.BYDAY) {
    rule.BYDAY.split(',').forEach(function(day) {
      var index = weekdayIndex(day);
      if (index !== -1) {
        days.push(index);
      }
    });
  }
  if (!days.length && fallbackComponents) {
    days.push(localWeekday(fallbackComponents));
  }
  return days;
}

function parseByDayEntries(rule) {
  var entries = [];
  if (!rule.BYDAY) {
    return entries;
  }

  rule.BYDAY.split(',').forEach(function(day) {
    var match = day.match(/^([+-]?\d+)?([A-Z]{2})$/);
    var index = weekdayIndex(day);
    if (match && index !== -1) {
      entries.push({
        ordinal: match[1] ? Number(match[1]) : null,
        weekday: index
      });
    }
  });
  return entries;
}

function daysInMonth(year, monthIndex) {
  return new Date(Date.UTC(year, monthIndex + 1, 0)).getUTCDate();
}

function nthWeekdayFromEndOfMonth(year, monthIndex, weekday, nthFromEnd) {
  var lastDay = daysInMonth(year, monthIndex);
  var lastWeekday = new Date(Date.UTC(year, monthIndex, lastDay)).getUTCDay();
  return lastDay - ((lastWeekday - weekday + 7) % 7) - ((nthFromEnd - 1) * 7);
}

function matchesMonthlyByDayEntry(dateComponents, entry) {
  if (localWeekday(dateComponents) !== entry.weekday) {
    return false;
  }

  if (!entry.ordinal) {
    return true;
  }

  var monthIndex = dateComponents.month - 1;
  if (entry.ordinal > 0) {
    return dateComponents.day === nthWeekdayOfMonth(
      dateComponents.year, monthIndex, entry.weekday, entry.ordinal
    );
  }

  return dateComponents.day === nthWeekdayFromEndOfMonth(
    dateComponents.year, monthIndex, entry.weekday, Math.abs(entry.ordinal)
  );
}

function componentsForOccurrence(startInfo, dateComponents) {
  return {
    year: dateComponents.year,
    month: dateComponents.month,
    day: dateComponents.day,
    hour: startInfo.components.hour,
    minute: startInfo.components.minute,
    second: startInfo.components.second,
    isUtc: startInfo.components.isUtc
  };
}

function eventDurationMs(event) {
  if (event.end && event.start) {
    return Math.max(0, event.end.getTime() - event.start.getTime());
  }
  return 60 * 60 * 1000;
}

function fallbackEventEndTime(start) {
  return start.getTime() + (CURRENT_EVENT_DISPLAY_MINUTES * 60 * 1000);
}

function eventEndTime(event) {
  if (event && event.end && event.start && event.end.getTime() > event.start.getTime()) {
    return event.end.getTime();
  }
  return event && event.start ? fallbackEventEndTime(event.start) : 0;
}

function compareCalendarTitles(a, b) {
  var aTitle = (a && a.summary) || '';
  var bTitle = (b && b.summary) || '';
  if (aTitle < bTitle) return -1;
  if (aTitle > bTitle) return 1;
  return 0;
}

function chooseBestCalendarEvent(events, nowMs, maxTime) {
  var candidates = events.filter(function(event) {
    return event && event.start
        && event.start.getTime() <= maxTime;
  });

  var started = candidates.filter(function(event) {
    return event.start.getTime() <= nowMs;
  }).sort(function(a, b) {
    var startDiff = b.start.getTime() - a.start.getTime();
    if (startDiff !== 0) return startDiff;
    var endDiff = eventEndTime(b) - eventEndTime(a);
    if (endDiff !== 0) return endDiff;
    return compareCalendarTitles(a, b);
  });
  if (started.length && eventEndTime(started[0]) > nowMs) {
    return started[0];
  }

  return candidates.filter(function(event) {
    return event.start.getTime() >= nowMs;
  }).sort(function(a, b) {
    var startDiff = a.start.getTime() - b.start.getTime();
    if (startDiff !== 0) return startDiff;
    var endDiff = eventEndTime(a) - eventEndTime(b);
    if (endDiff !== 0) return endDiff;
    return compareCalendarTitles(a, b);
  })[0] || null;
}

function parseIcsDateList(line, fallbackTzid) {
  var property = parseIcsProperty(line);
  if (property.meta.indexOf('VALUE=DATE') !== -1) {
    return [];
  }

  var tzid = normalizeTzid(property.params.TZID || fallbackTzid);
  var dates = [];
  property.value.split(',').forEach(function(value) {
    if (/^\d{8}$/.test(value)) {
      return;
    }
    var components = parseIcsDateComponents(value);
    if (components) {
      dates.push(dateFromIcsComponents(components, tzid));
    }
  });
  return dates;
}

function recurrenceUntil(rule, fallbackTzid) {
  if (!rule.UNTIL) {
    return null;
  }

  var components = parseIcsDateComponents(rule.UNTIL);
  if (!components) {
    return null;
  }
  return dateFromIcsComponents(components, components.isUtc ? null : fallbackTzid);
}

function eventExceptionKey(uid, date) {
  return uid + '|' + date.getTime();
}

function matchesRecurrenceDate(dateComponents, startComponents, rule) {
  var frequency = rule.FREQ || '';
  var interval = Math.max(1, Number(rule.INTERVAL) || 1);
  var dayMs = 24 * 60 * 60 * 1000;
  var startDate = localDateOnly(startComponents);
  var candidateDate = localDateOnly(dateComponents);

  if (candidateDate < startDate) {
    return false;
  }

  if (frequency === 'DAILY') {
    var days = Math.floor((candidateDate - startDate) / dayMs);
    var dailyByDays = rule.BYDAY ? parseByDay(rule, null) : [];
    return days % interval === 0
        && (!dailyByDays.length || dailyByDays.indexOf(localWeekday(dateComponents)) !== -1);
  }

  if (frequency === 'WEEKLY') {
    var byDays = parseByDay(rule, startComponents);
    var weekStart = weekdayIndex(rule.WKST || 'SU');
    if (weekStart === -1) {
      weekStart = 0;
    }
    var weeks = Math.floor(
      (startOfWeekDateOnly(dateComponents, weekStart) - startOfWeekDateOnly(startComponents, weekStart))
      / (7 * dayMs)
    );
    return weeks >= 0
        && weeks % interval === 0
        && byDays.indexOf(localWeekday(dateComponents)) !== -1;
  }

  if (frequency === 'MONTHLY') {
    var months = ((dateComponents.year - startComponents.year) * 12)
        + (dateComponents.month - startComponents.month);
    if (months < 0 || months % interval !== 0) {
      return false;
    }
    if (rule.BYMONTHDAY) {
      return rule.BYMONTHDAY.split(',').indexOf(String(dateComponents.day)) !== -1;
    }
    if (rule.BYDAY) {
      var monthlyByDays = parseByDayEntries(rule);
      return monthlyByDays.some(function(entry) {
        return matchesMonthlyByDayEntry(dateComponents, entry);
      });
    }
    return dateComponents.day === startComponents.day;
  }

  if (frequency === 'YEARLY') {
    var years = dateComponents.year - startComponents.year;
    return years >= 0
        && years % interval === 0
        && dateComponents.month === startComponents.month
        && dateComponents.day === startComponents.day;
  }

  return false;
}

function expandRecurringEvent(event, now, maxTime, exceptionKeys) {
  if (!event.rrule || !event.startInfo) {
    return [event];
  }

  var rule = parseRRule(event.rrule);
  var until = recurrenceUntil(rule, event.startInfo.tzid);
  var duration = eventDurationMs(event);
  var exdates = {};
  var occurrences = [];
  var generated = 0;
  var count = Number(rule.COUNT) || 0;
  var dayMs = 24 * 60 * 60 * 1000;
  var dateOnly = localDateOnly(event.startInfo.components);
  var hardStop = dateOnly + (dayMs * 12000);
  var latestStartedOccurrence = null;

  (event.exdates || []).forEach(function(line) {
    parseIcsDateList(line, event.startInfo.tzid).forEach(function(date) {
      exdates[date.getTime()] = true;
    });
  });

  while (dateOnly <= hardStop) {
    var dateComponents = componentsFromDateOnly(dateOnly);
    var occurrenceComponents = componentsForOccurrence(event.startInfo, dateComponents);
    var occurrenceStart = dateFromIcsComponents(occurrenceComponents, event.startInfo.tzid);
    var occurrenceEnd = new Date(occurrenceStart.getTime() + duration);

    if (occurrenceStart.getTime() > maxTime && dateOnly > localDateOnly(event.startInfo.components)) {
      break;
    }

    if (until && occurrenceStart.getTime() > until.getTime()) {
      break;
    }

    if (matchesRecurrenceDate(dateComponents, event.startInfo.components, rule)) {
      generated++;
      if (count && generated > count) {
        break;
      }

      if (occurrenceStart.getTime() <= maxTime && occurrenceEnd.getTime() > now.getTime()
          && !exdates[occurrenceStart.getTime()]
          && !(event.uid && exceptionKeys[eventExceptionKey(event.uid, occurrenceStart)])) {
        var occurrence = {
          start: occurrenceStart,
          end: occurrenceEnd,
          summary: event.summary
        };
        occurrences.push(occurrence);
        if (occurrenceStart.getTime() <= now.getTime()) {
          latestStartedOccurrence = occurrence;
        }
      } else if (occurrenceStart.getTime() <= now.getTime()
          && !exdates[occurrenceStart.getTime()]
          && !(event.uid && exceptionKeys[eventExceptionKey(event.uid, occurrenceStart)])) {
        latestStartedOccurrence = {
          start: occurrenceStart,
          end: occurrenceEnd,
          summary: event.summary
        };
      }
    }

    dateOnly += dayMs;
  }

  if (latestStartedOccurrence && !occurrences.some(function(occurrence) {
    return occurrence.start.getTime() === latestStartedOccurrence.start.getTime();
  })) {
    occurrences.push(latestStartedOccurrence);
  }

  return occurrences;
}

function parseCalendarEvents(icsText, lookaheadHours) {
  var lines = unfoldIcs(icsText);
  var rawEvents = [];
  var events = [];
  var current = null;

  lines.forEach(function(line) {
    if (line === 'BEGIN:VEVENT') {
      current = {};
    } else if (line === 'END:VEVENT' && current) {
      rawEvents.push(current);
      current = null;
    } else if (current) {
      if (line.indexOf('DTSTART') === 0) {
        current.startInfo = parseIcsDateInfo(line);
        current.start = current.startInfo ? current.startInfo.date : null;
      } else if (line.indexOf('DTEND') === 0) {
        current.end = parseIcsDate(line);
      } else if (line.indexOf('SUMMARY') === 0) {
        current.summary = unescapeIcsText(parseIcsProperty(line).value);
      } else if (line.indexOf('UID') === 0) {
        current.uid = parseIcsProperty(line).value;
      } else if (line.indexOf('RRULE') === 0) {
        current.rrule = line;
      } else if (line.indexOf('EXDATE') === 0) {
        current.exdates = current.exdates || [];
        current.exdates.push(line);
      } else if (line.indexOf('RECURRENCE-ID') === 0) {
        current.recurrenceIdInfo = parseIcsDateInfo(line);
      } else if (line.indexOf('STATUS') === 0) {
        current.status = parseIcsProperty(line).value;
      }
    }
  });

  var now = new Date();
  var maxTime = now.getTime() + (Number(lookaheadHours) || 48) * 60 * 60 * 1000;
  var exceptionKeys = {};

  rawEvents.forEach(function(event) {
    if (event.uid && event.recurrenceIdInfo) {
      exceptionKeys[eventExceptionKey(event.uid, event.recurrenceIdInfo.date)] = true;
    }
  });

  rawEvents.forEach(function(event) {
    if (!event.start || !event.summary || event.status === 'CANCELLED') {
      return;
    }
    expandRecurringEvent(event, now, maxTime, exceptionKeys).forEach(function(occurrence) {
      events.push(occurrence);
    });
  });

  return events.filter(function(event) {
    return event && event.start && eventEndTime(event) > now.getTime()
        && event.start.getTime() <= maxTime;
  }).sort(function(a, b) {
    var startDiff = a.start.getTime() - b.start.getTime();
    if (startDiff !== 0) return startDiff;
    var endDiff = eventEndTime(a) - eventEndTime(b);
    if (endDiff !== 0) return endDiff;
    return compareCalendarTitles(a, b);
  });
}

function parseNextEvent(icsText, lookaheadHours) {
  var events = parseCalendarEvents(icsText, lookaheadHours);
  var now = new Date();
  var maxTime = now.getTime() + (Number(lookaheadHours) || 48) * 60 * 60 * 1000;
  return chooseBestCalendarEvent(events, now.getTime(), maxTime);
}

function nextSortedEvent(events) {
  return chooseBestCalendarEvent(events, new Date().getTime(), Number.MAX_VALUE);
}

function formatHour(date) {
  var hours = date.getHours();
  var suffix = hours >= 12 ? 'P' : 'A';
  var hour = hours % 12;
  if (hour === 0) {
    hour = 12;
  }
  return hour + ':' + (date.getMinutes() < 10 ? '0' : '') + date.getMinutes() + suffix;
}

function formatEvent(event) {
  if (!event) {
    return '[None]';
  }

  var prefix = formatHour(event.start);
  var separator = ' | ';
  var maxLength = 72;
  var maxSummaryLength = maxLength - prefix.length - separator.length;
  var summary = event.summary;
  if (summary.length > maxSummaryLength) {
    summary = summary.slice(0, Math.max(0, maxSummaryLength - 3)) + '...';
  }
  return prefix + separator + summary;
}

function formatEventDelta(event) {
  if (!event) {
    return '--';
  }

  var now = new Date().getTime();
  var start = event.start.getTime();
  var end = eventEndTime(event);
  if (start <= now && end > now) {
    return 'NOW';
  }

  var minutes = Math.max(0, Math.ceil((start - now) / 60000));
  if (minutes < 100) {
    return minutes + 'm';
  }

  var hours = Math.ceil(minutes / 60);
  if (hours < 100) {
    return hours + 'h';
  }

  return '99+';
}

function calendarUrls(settings) {
  var urls = [];
  [
    settings.CALENDAR_ICS_URL,
    settings.CALENDAR_ICS_URL_2
  ].forEach(function(value) {
    var url = (value || '').trim();
    if (url && urls.indexOf(url) === -1) {
      urls.push(url);
    }
  });
  return urls;
}

function startOfLocalDay(date) {
  return new Date(date.getFullYear(), date.getMonth(), date.getDate());
}

function dayBusyHoursBitmap(events, day) {
  var bitmap = 0;
  events.forEach(function(event) {
    if (!event || !event.start) {
      return;
    }
    for (var hour = 0; hour < 24; hour++) {
      var slotStart = new Date(day.getFullYear(), day.getMonth(), day.getDate(), hour, 0, 0);
      var slotEnd = new Date(day.getFullYear(), day.getMonth(), day.getDate(), hour + 1, 0, 0);
      if (event.start.getTime() < slotEnd.getTime() && eventEndTime(event) > slotStart.getTime()) {
        bitmap |= (1 << hour);
      }
    }
  });
  return bitmap;
}

function dayBusyHalfHoursBitmap(events, day, secondHalf) {
  var bitmap = 0;
  var halfStartMin = secondHalf ? 30 : 0;
  var halfEndMin = secondHalf ? 60 : 30;
  events.forEach(function(event) {
    if (!event || !event.start) {
      return;
    }
    for (var hour = 0; hour < 24; hour++) {
      var slotStart = new Date(day.getFullYear(), day.getMonth(), day.getDate(), hour, halfStartMin, 0);
      var slotEnd = new Date(day.getFullYear(), day.getMonth(), day.getDate(), hour, halfEndMin, 0);
      if (event.start.getTime() < slotEnd.getTime() && eventEndTime(event) > slotStart.getTime()) {
        bitmap |= (1 << hour);
      }
    }
  });
  return bitmap;
}

function yourDayFixedHourCount(settings) {
  var startHour = numberSetting(settings.YOUR_DAY_START_HOUR, 8, 0, 23);
  var endHour = numberSetting(settings.YOUR_DAY_END_HOUR, 17, 0, 23);
  var span = endHour - startHour;
  if (span < 0) {
    span += 24;
  }
  return numberSetting(span + 1, 10, 2, 10);
}

function yourDayWindowRange(settings) {
  var now = new Date();
  var start;
  var hourCount;
  if (settings.YOUR_DAY_WINDOW_MODE === 'fixed') {
    var startHour = numberSetting(settings.YOUR_DAY_START_HOUR, 8, 0, 23);
    hourCount = yourDayFixedHourCount(settings);
    var today = startOfLocalDay(now);
    start = new Date(today.getFullYear(), today.getMonth(), today.getDate(), startHour, 0, 0);
  } else {
    hourCount = numberSetting(settings.YOUR_DAY_WINDOW_HOURS, 10, 2, 10);
    start = new Date(now.getFullYear(), now.getMonth(), now.getDate(), now.getHours() - 1, 0, 0);
  }
  return {
    start: start,
    end: new Date(start.getTime() + (hourCount * 60 * 60 * 1000))
  };
}

function yourDayWindowEventCount(events, settings) {
  var range = yourDayWindowRange(settings);
  return events.filter(function(event) {
    return event && event.start
        && event.start.getTime() < range.end.getTime()
        && eventEndTime(event) > range.start.getTime();
  }).length;
}

function optionalEventTitle(event) {
  return event ? formatEvent(event) : '';
}

function optionalEventDelta(event) {
  return event ? formatEventDelta(event) : '';
}

function sendCalendarEvents(events, settings) {
  settings = settings || readSettings();
  var sorted = (events || []).slice().sort(function(a, b) {
    var startDiff = a.start.getTime() - b.start.getTime();
    if (startDiff !== 0) return startDiff;
    var endDiff = eventEndTime(a) - eventEndTime(b);
    if (endDiff !== 0) return endDiff;
    return compareCalendarTitles(a, b);
  });
  var displayEvents = sorted.slice(0, 5);

  var summaryDict = {};
  var today = startOfLocalDay(new Date());
  var yesterday = new Date(today.getTime() - 24 * 60 * 60 * 1000);
  var tomorrow = new Date(today.getTime() + 24 * 60 * 60 * 1000);
  summaryDict[keys.NEXT_EVENT] = formatEvent(displayEvents[0]);
  summaryDict[keys.NEXT_EVENT_DELTA] = formatEventDelta(displayEvents[0]);
  summaryDict[keys.DAY_EVENT_HOURS_BITMAP_YESTERDAY] = dayBusyHoursBitmap(sorted, yesterday);
  summaryDict[keys.DAY_EVENT_HOURS_BITMAP] = dayBusyHoursBitmap(sorted, today);
  summaryDict[keys.DAY_EVENT_HOURS_BITMAP_TOMORROW] = dayBusyHoursBitmap(sorted, tomorrow);
  summaryDict[keys.DAY_EVENT_HALF_HOURS_FIRST_BITMAP_YESTERDAY] = dayBusyHalfHoursBitmap(sorted, yesterday, false);
  summaryDict[keys.DAY_EVENT_HALF_HOURS_FIRST_BITMAP] = dayBusyHalfHoursBitmap(sorted, today, false);
  summaryDict[keys.DAY_EVENT_HALF_HOURS_FIRST_BITMAP_TOMORROW] = dayBusyHalfHoursBitmap(sorted, tomorrow, false);
  summaryDict[keys.DAY_EVENT_HALF_HOURS_SECOND_BITMAP_YESTERDAY] = dayBusyHalfHoursBitmap(sorted, yesterday, true);
  summaryDict[keys.DAY_EVENT_HALF_HOURS_SECOND_BITMAP] = dayBusyHalfHoursBitmap(sorted, today, true);
  summaryDict[keys.DAY_EVENT_HALF_HOURS_SECOND_BITMAP_TOMORROW] = dayBusyHalfHoursBitmap(sorted, tomorrow, true);
  summaryDict[keys.DAY_EVENT_COUNT_TODAY] = yourDayWindowEventCount(sorted, settings);
  sendToWatch(summaryDict, 'Calendar summary');

  var middleDict = {};
  middleDict[keys.CALENDAR_EVENT_TITLE_2] = optionalEventTitle(displayEvents[1]);
  middleDict[keys.CALENDAR_EVENT_DELTA_2] = optionalEventDelta(displayEvents[1]);
  middleDict[keys.CALENDAR_EVENT_TITLE_3] = optionalEventTitle(displayEvents[2]);
  middleDict[keys.CALENDAR_EVENT_DELTA_3] = optionalEventDelta(displayEvents[2]);
  sendToWatch(middleDict, 'Calendar events 2-3');

  var endDict = {};
  endDict[keys.CALENDAR_EVENT_TITLE_4] = optionalEventTitle(displayEvents[3]);
  endDict[keys.CALENDAR_EVENT_DELTA_4] = optionalEventDelta(displayEvents[3]);
  endDict[keys.CALENDAR_EVENT_TITLE_5] = optionalEventTitle(displayEvents[4]);
  endDict[keys.CALENDAR_EVENT_DELTA_5] = optionalEventDelta(displayEvents[4]);
  sendToWatch(endDict, 'Calendar events 4-5');
}

function refreshCalendar() {
  var settings = readSettings();
  if (!settings.CALENDAR_ENABLED) {
    return;
  }

  var urls = calendarUrls(settings);
  if (!urls.length) {
    sendCalendarEvents([], settings);
    return;
  }

  var pending = urls.length;
  var successCount = 0;
  var events = [];

  function finish() {
    pending--;
    if (pending > 0) {
      return;
    }

    if (!successCount) {
      console.log('Calendar refresh failed for all feeds');
      return;
    }

    sendCalendarEvents(events, settings);
  }

  urls.forEach(function(url) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
      if (xhr.status && (xhr.status < 200 || xhr.status >= 400)) {
        console.log('Calendar request failed with status ' + xhr.status);
        finish();
        return;
      }

      successCount++;
      try {
        parseCalendarEvents(xhr.responseText, settings.CALENDAR_LOOKAHEAD_HOURS)
          .forEach(function(event) {
            events.push(event);
          });
      } catch (e) {
        console.log('Calendar parse failed: ' + e);
      }
      finish();
    };
    xhr.onerror = function() {
      console.log('Calendar request failed');
      finish();
    };
    xhr.open('GET', url);
    xhr.send();
  });
}

function scheduleRefreshes() {
  var settings = readSettings();
  var weatherMinutes = clamp(Number(settings.WEATHER_REFRESH_MIN) || 30, 15, 60);

  if (weatherTimer) {
    clearInterval(weatherTimer);
  }
  if (calendarTimer) {
    clearInterval(calendarTimer);
  }

  weatherTimer = setInterval(refreshWeather, weatherMinutes * 60 * 1000);
  calendarTimer = setInterval(refreshCalendar, 10 * 60 * 1000);
}

Pebble.addEventListener('ready', function() {
  var settings = readSettings();
  sendLayoutSetting(settings);
  sendColorSetting(settings);
  sendShakeSetting(settings);
  sendTideSetting(settings);
  refreshWeather(true);
  refreshCalendar();
  scheduleRefreshes();
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(event) {
  if (!event || !event.response) {
    return;
  }

  clay.getSettings(event.response, false);
  var settings = readSettings();
  sendLayoutSetting(settings);
  sendColorSetting(settings);
  sendShakeSetting(settings);
  sendTideSetting(settings);
  refreshWeather(true);
  refreshCalendar();
  scheduleRefreshes();
});
