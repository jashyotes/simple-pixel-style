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

  // Pip color pickers visible iff the user can actually affect the pip
  // gradient: either FITNESS_PIP_COLOR_SOURCE='color' (always color
  // override), or 'theme' + global COLOR_MODE='color'.
  var FITNESS_PIP_COLOR_PICKER_KEYS = [
    'FITNESS_PIP_COLOR_LOW',
    'FITNESS_PIP_COLOR_MID',
    'FITNESS_PIP_COLOR_HIGH'
  ];

  // Pip row sub-settings hidden entirely when the FuelBand pip row is off.
  // (The three color pickers above are gated separately by syncPipColorPickers.)
  var FITNESS_PIP_BASE_KEYS = [
    'FITNESS_PIP_SHAPE',
    'FITNESS_PIP_SIZE',
    'FITNESS_PIP_DIRECTION',
    'FITNESS_PIP_STYLE',
    'FITNESS_PIP_COLOR_SOURCE',
    'FITNESS_PIP_COLOR_DIST'
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

  var SHAKE_NWS_ITEM_KEYS = [
    'shake-nws-heading',
    'NWS_FORECAST_STYLE',
    'NWS_ZIP'
  ];

  var SHAKE_PRICES_ITEM_KEYS = [
    'shake-prices-heading',
    'PRICES_STOCK_1_SYMBOL',
    'PRICES_STOCK_2_SYMBOL',
    'PRICES_CRYPTO_SYMBOL',
    'PRICES_SHOW_STOCK_1',
    'PRICES_SHOW_STOCK_2',
    'PRICES_SHOW_CRYPTO',
    'PRICES_CADENCE_MIN',
    'PRICES_POSITIVE_COLOR_LIGHT',
    'PRICES_POSITIVE_COLOR_DARK',
    'PRICES_NEGATIVE_COLOR_LIGHT',
    'PRICES_NEGATIVE_COLOR_DARK'
  ];

  var MULTI_TZ_DEPENDENT_KEYS = [
    'multi-tz-1',
    'multi-tz-2',
    'multi-tz-style'
  ];

  var CAL_EVENT_DATE_DEPENDENT_KEYS = [
    'CAL_EVENT_DATE_ORDER',
    'CAL_EVENT_DATE_POSITION',
    'CAL_EVENT_DATE_NO_ZERO'
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
    var pipColorSourceItem = clayCfg.getItemByMessageKey('FITNESS_PIP_COLOR_SOURCE');
    var multiTzEnabledItem = clayCfg.getItemByMessageKey('MULTI_TZ_ENABLED');
    var calEventDatesItem = clayCfg.getItemByMessageKey('CAL_EVENT_DATES_ON');
    var pipRowItem = clayCfg.getItemByMessageKey('FITNESS_PIP_ROW_ON');

    function syncColorMode() {
      var v = colorModeItem.get();
      if (v === 'color') {
        setGroupVisible(BW_ITEM_KEYS, false);
        setGroupVisible(COLOR_ITEM_KEYS, true);
      } else {
        setGroupVisible(BW_ITEM_KEYS, true);
        setGroupVisible(COLOR_ITEM_KEYS, false);
      }
      syncPipColorPickers();
    }

    function syncPipColorPickers() {
      // No pickers when the pip row itself is off.
      if (pipRowItem && !pipRowItem.get()) {
        setGroupVisible(FITNESS_PIP_COLOR_PICKER_KEYS, false);
        return;
      }
      var globalIsColor = colorModeItem.get() === 'color';
      var pipSource = pipColorSourceItem ? pipColorSourceItem.get() : 'theme';
      var showPickers = (pipSource === 'color') || globalIsColor;
      setGroupVisible(FITNESS_PIP_COLOR_PICKER_KEYS, showPickers);
    }

    function syncPipRow() {
      var on = pipRowItem ? !!pipRowItem.get() : true;
      setGroupVisible(FITNESS_PIP_BASE_KEYS, on);
      syncPipColorPickers();
    }

    function syncShake() {
      var v = shakeItem.get();
      setGroupVisible(SHAKE_FITNESS_ITEM_KEYS, v === 'fitness_rings');
      setGroupVisible(SHAKE_CALENDAR_ITEM_KEYS, v === 'calendar_events');
      setGroupVisible(SHAKE_YOURDAY_ITEM_KEYS, v === 'your_day');
      setGroupVisible(SHAKE_ALTTZ_ITEM_KEYS, v === 'alt_timezone');
      setGroupVisible(SHAKE_TIDE_ITEM_KEYS, v === 'tide_chart');
      setGroupVisible(SHAKE_PRICES_ITEM_KEYS, v === 'prices');
      setGroupVisible(SHAKE_NWS_ITEM_KEYS, v === 'nws_forecast');
    }

    function syncMultiTz() {
      if (!multiTzEnabledItem) return;
      setGroupVisible(MULTI_TZ_DEPENDENT_KEYS, !!multiTzEnabledItem.get());
    }

    function syncCalEventDates() {
      if (!calEventDatesItem) return;
      setGroupVisible(CAL_EVENT_DATE_DEPENDENT_KEYS, !!calEventDatesItem.get());
    }

    colorModeItem.on('change', syncColorMode);
    shakeItem.on('change', syncShake);
    if (pipColorSourceItem) {
      pipColorSourceItem.on('change', syncPipColorPickers);
    }
    if (pipRowItem) {
      pipRowItem.on('change', syncPipRow);
    }
    if (multiTzEnabledItem) {
      multiTzEnabledItem.on('change', syncMultiTz);
    }
    if (calEventDatesItem) {
      calEventDatesItem.on('change', syncCalEventDates);
    }
    syncColorMode();
    syncShake();
    syncPipRow();
    syncMultiTz();
    syncCalEventDates();
  });
}

var clay = new Clay(clayConfig, customClay, { autoHandleEvents: false });
var weatherTimer = null;
var calendarTimer = null;
var pricesTimer = null;
var multiTzTimer = null;
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
  INVERT_TOP_BAR: true,
  INVERT_DATE_BAR: true,
  INVERT_TIME: true,
  MILITARY_TIME: false,
  REMOVE_LEADING_ZERO: false,
  DATE_LANGUAGE: 'en',
  TIME_FONT: '1',
  CASIO_PHANTOM: true,
  INVERT_WEATHER: false,
  INVERT_MEETING_BAR: false,
  TOP_STEPS: true,
  VIBRATE_ON_DISCONNECT: true,
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
  WEATHER_PROVIDER: 'open_meteo',
  NWS_FORECAST_STYLE: 'chart_heavy',
  NWS_ZIP: '',
  CALENDAR_ENABLED: false,
  CALENDAR_ICS_URL: '',
  CALENDAR_ICS_URL_2: '',
  CALENDAR_LOOKAHEAD_HOURS: '48',
  EMPTY_EVENT_LABEL: '[None]',
  CAL_EVENT_DATES_ON: false,
  CAL_EVENT_DATE_ORDER: 'month_day',
  CAL_EVENT_DATE_POSITION: 'before',
  CAL_EVENT_DATE_NO_ZERO: false,
  SHAKE_BEHAVIOR: 'fitness_rings',
  CALENDAR_SHAKE_EVENT_COUNT: '3',
  FITNESS_RING_STEPS_ON: true,
  FITNESS_RING_ACTIVE_ON: true,
  FITNESS_RING_CALORIES_ON: true,
  FITNESS_TARGET_STEPS: '10000',
  FITNESS_TARGET_ACTIVE_MIN: '30',
  FITNESS_TARGET_CALORIES: '500',
  FITNESS_VIBRATE_ON_GOAL: false,
  FITNESS_GOAL_VIBE_PATTERN: 'short',
  FITNESS_COLOR_STEPS: 0x00FF00,
  FITNESS_COLOR_ACTIVE: 0x00AAFF,
  FITNESS_COLOR_CALORIES: 0xFF0000,
  FITNESS_OVERLAY_DURATION_S: '5',
  FITNESS_PIP_ROW_ON: true,
  FITNESS_PIP_DIRECTION: 'ltr',
  FITNESS_PIP_STYLE: 'hollow',
  FITNESS_PIP_COLOR_LOW: 0xFF0000,
  FITNESS_PIP_COLOR_MID: 0xFFAA00,
  FITNESS_PIP_COLOR_HIGH: 0x005500,
  FITNESS_PIP_COLOR_DIST: 'asymptotic',
  FITNESS_PIP_COLOR_SOURCE: 'color',
  FITNESS_PIP_SHAPE: 'circle',
  FITNESS_PIP_SIZE: 'large',
  ALT_TZ_LABEL: 'LONDON',
  ALT_TZ_OFFSET_MIN: '0',
  YOUR_DAY_WINDOW_MODE: 'rolling',
  YOUR_DAY_WINDOW_HOURS: '10',
  YOUR_DAY_START_HOUR: '8',
  YOUR_DAY_END_HOUR: '17',
  YOUR_DAY_HALF_HOUR_PIPS: false,
  TIDE_STATION_ID: '',
  TIDE_UNITS: 'feet',
  TIDE_VIEW_HOURS: '24',
  PRICES_STOCK_1_SYMBOL: 'SPY',
  PRICES_STOCK_2_SYMBOL: 'QQQ',
  PRICES_CRYPTO_SYMBOL: 'bitcoin',
  PRICES_CADENCE_MIN: '30',
  PRICES_POSITIVE_COLOR_LIGHT: 0x000000,
  PRICES_POSITIVE_COLOR_DARK: 0xFFFFFF,
  PRICES_NEGATIVE_COLOR_LIGHT: 0x000000,
  PRICES_NEGATIVE_COLOR_DARK: 0xFFFFFF,
  PRICES_SHOW_STOCK_1: true,
  PRICES_SHOW_STOCK_2: true,
  PRICES_SHOW_CRYPTO: true,
  MULTI_TZ_ENABLED: false,
  MULTI_TZ_1: 'none',
  MULTI_TZ_2: 'none',
  MULTI_TZ_STYLE: 'text'
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
  distance_today: 20,
  stock_1: 21,
  stock_2: 22,
  bitcoin: 23
};

var SHAKE_BEHAVIOR_IDS = {
  off: 0,
  fitness_rings: 1,
  calendar_events: 2,
  your_day: 3,
  detailed_weather: 4,
  alt_timezone: 5,
  heart_rate: 6,
  prices: 7,
  tide_chart: 8,
  step_history: 10,
  nws_forecast: 11
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

function fitnessPipDirectionId(value) {
  if (value === 'center') return 1;
  if (value === 'rtl') return 2;
  return 0; // 'ltr' default
}

function fitnessPipStyleId(value) {
  return value === 'populate' ? 1 : 0;
}

function fitnessPipColorSourceId(value) {
  return value === 'color' ? 1 : 0;
}

function fitnessPipShapeId(value) {
  return value === 'rectangle' ? 1 : 0;  // 0=circle (default), 1=rectangle
}

function fitnessPipSizeId(value) {
  return value === 'small' ? 0 : 1;  // 0=small, 1=large (default)
}

function fitnessPipColorDistId(value) {
  return value === 'linear' ? 0 : 1;  // 0=linear, 1=asymptotic FuelBand (default)
}

function fitnessGoalVibePatternId(value) {
  switch (value) {
    case 'long':       return 1;
    case 'double':     return 2;
    case 'heartbeat':  return 3;
    case 'mario':      return 4;
    case 'mario_theme': return 8;
    case 'sos':        return 5;
    case 'rising':     return 6;
    case 'ff_victory': return 7;
    case 'short':
    default:           return 0;
  }
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
  dict[keys.DATE_LANGUAGE] = settings.DATE_LANGUAGE === 'ja' ? 1
      : (settings.DATE_LANGUAGE === 'ja_compact' ? 2 : 0);
  dict[keys.TIME_FONT] = numberSetting(settings.TIME_FONT, 0, 0, 3);
  dict[keys.CASIO_PHANTOM] = settings.CASIO_PHANTOM ? 1 : 0;
  dict[keys.INVERT_WEATHER] = settings.INVERT_WEATHER ? 1 : 0;
  dict[keys.INVERT_MEETING_BAR] = settings.INVERT_MEETING_BAR ? 1 : 0;
  dict[keys.TOP_STEPS] = settings.TOP_STEPS ? 1 : 0;
  dict[keys.FITNESS_PIP_ROW_ON] = settings.FITNESS_PIP_ROW_ON ? 1 : 0;
  dict[keys.FITNESS_PIP_DIRECTION] = fitnessPipDirectionId(settings.FITNESS_PIP_DIRECTION);
  dict[keys.FITNESS_PIP_STYLE] = fitnessPipStyleId(settings.FITNESS_PIP_STYLE);
  dict[keys.FITNESS_PIP_COLOR_LOW] = colorSetting(settings.FITNESS_PIP_COLOR_LOW, 0xFF0000);
  dict[keys.FITNESS_PIP_COLOR_MID] = colorSetting(settings.FITNESS_PIP_COLOR_MID, 0xFFFF00);
  dict[keys.FITNESS_PIP_COLOR_HIGH] = colorSetting(settings.FITNESS_PIP_COLOR_HIGH, 0x00FF00);
  dict[keys.FITNESS_PIP_COLOR_DIST] = fitnessPipColorDistId(settings.FITNESS_PIP_COLOR_DIST);
  dict[keys.FITNESS_PIP_COLOR_SOURCE] = fitnessPipColorSourceId(settings.FITNESS_PIP_COLOR_SOURCE);
  dict[keys.FITNESS_PIP_SHAPE] = fitnessPipShapeId(settings.FITNESS_PIP_SHAPE);
  dict[keys.FITNESS_PIP_SIZE] = fitnessPipSizeId(settings.FITNESS_PIP_SIZE);
  dict[keys.VIBRATE_ON_DISCONNECT] = settings.VIBRATE_ON_DISCONNECT ? 1 : 0;
  dict[keys.VERBOSE_WEATHER] = settings.VERBOSE_WEATHER ? 1 : 0;
  dict[keys.VERBOSE_WEATHER_STYLE] = settings.VERBOSE_WEATHER_STYLE === 'large' ? 1 : 0;
  dict[keys.COMPLICATION_1] = complicationId(settings.COMPLICATION_1, COMPLICATION_IDS.weather);
  dict[keys.COMPLICATION_2] = complicationId(settings.COMPLICATION_2, COMPLICATION_IDS.rain);
  dict[keys.COMPLICATION_3] = complicationId(settings.COMPLICATION_3, COMPLICATION_IDS.heart_rate);
  dict[keys.TEMPERATURE_UNIT] = temperatureUnit(settings) === 'celsius' ? 1 : 0;
  dict[keys.WEATHER_PROVIDER] = weatherProviderId(settings);
  dict[keys.NWS_FORECAST_STYLE] =
      settings.NWS_FORECAST_STYLE === 'narrative' ? 1 : 0;
  var emptyLabel = String(settings.EMPTY_EVENT_LABEL || '').trim();
  if (!emptyLabel) emptyLabel = '[None]';
  dict[keys.EMPTY_EVENT_LABEL] = emptyLabel.slice(0, 24);
  // Calendar event dates. POSITION: 0 = before time, 1 = after. ORDER: 0 = month-first, 1 = day-first.
  dict[keys.CAL_EVENT_DATES_ON] = settings.CAL_EVENT_DATES_ON ? 1 : 0;
  dict[keys.CAL_EVENT_DATE_POSITION] = settings.CAL_EVENT_DATE_POSITION === 'after' ? 1 : 0;
  dict[keys.CAL_EVENT_DATE_ORDER] = settings.CAL_EVENT_DATE_ORDER === 'day_month' ? 1 : 0;
  dict[keys.CAL_EVENT_DATE_NO_ZERO] = settings.CAL_EVENT_DATE_NO_ZERO ? 1 : 0;
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
  dict[keys.FITNESS_VIBRATE_ON_GOAL] = settings.FITNESS_VIBRATE_ON_GOAL ? 1 : 0;
  dict[keys.FITNESS_GOAL_VIBE_PATTERN] = fitnessGoalVibePatternId(settings.FITNESS_GOAL_VIBE_PATTERN);
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

// The watch has no timezone database, so the phone resolves each selected zone
// to a DST-correct offset + abbreviation and sends those. An unset/unknown zone
// is sent as an empty label, which the watch treats as inactive.
function multiTzZoneToDict(tzid, components, labelKey, offsetKey, dict) {
  var id = normalizeTzid(tzid);
  if (!id || id === 'none') {
    dict[labelKey] = '';
    dict[offsetKey] = 0;
    return;
  }
  var offset = timezoneOffsetMinutes(id, components);
  var label = timezoneLabel(id, components);
  if (offset === null || !label) {
    dict[labelKey] = '';
    dict[offsetKey] = 0;
    return;
  }
  dict[labelKey] = label.slice(0, 7);
  dict[offsetKey] = offset;
}

function sendMultiTzSetting(settings) {
  var dict = {};
  var now = utcComponentsNow();
  dict[keys.MULTI_TZ_ENABLED] = settings.MULTI_TZ_ENABLED ? 1 : 0;
  dict[keys.MULTI_TZ_STYLE] = settings.MULTI_TZ_STYLE === 'circle' ? 1 : 0;
  multiTzZoneToDict(settings.MULTI_TZ_1, now, keys.MULTI_TZ_1_LABEL, keys.MULTI_TZ_1_OFFSET_MIN, dict);
  multiTzZoneToDict(settings.MULTI_TZ_2, now, keys.MULTI_TZ_2_LABEL, keys.MULTI_TZ_2_OFFSET_MIN, dict);
  sendToWatch(dict, 'Multi-timezone setting');
}

function tideStationId(settings) {
  return String(settings.TIDE_STATION_ID || '').trim().slice(0, 15);
}

function tideUnitsId(settings) {
  return settings.TIDE_UNITS === 'meters' ? 1 : 0;
}

function tideViewHoursValue(settings) {
  return String(settings.TIDE_VIEW_HOURS) === '48' ? 48 : 24;
}

function sendTideSetting(settings) {
  var dict = {};
  dict[keys.TIDE_STATION_ID] = tideStationId(settings);
  dict[keys.TIDE_UNITS] = tideUnitsId(settings);
  dict[keys.TIDE_VIEW_HOURS] = tideViewHoursValue(settings);
  sendToWatch(dict, 'Tide setting');
  refreshTidesForSettings(settings);
}

function normalizeStockSymbol(value, fallback) {
  var symbol = String(value || '').trim().toUpperCase();
  return (symbol || fallback).slice(0, 11);
}

function normalizeCryptoSymbol(value) {
  var symbol = String(value || '').trim().toLowerCase();
  return (symbol || 'bitcoin').slice(0, 15);
}

function pricesCadenceMinutes(value) {
  var minutes = Number(value) || 30;
  var allowed = [1, 5, 10, 30, 60, 1440];
  return allowed.indexOf(minutes) === -1 ? 30 : minutes;
}

function sendPricesSetting(settings) {
  var dict = {};
  dict[keys.PRICES_STOCK_1_SYMBOL] = normalizeStockSymbol(
      settings.PRICES_STOCK_1_SYMBOL, 'SPY');
  dict[keys.PRICES_STOCK_2_SYMBOL] = normalizeStockSymbol(
      settings.PRICES_STOCK_2_SYMBOL, 'QQQ');
  dict[keys.PRICES_CRYPTO_SYMBOL] = normalizeCryptoSymbol(settings.PRICES_CRYPTO_SYMBOL);
  dict[keys.PRICES_SHOW_STOCK_1] = settings.PRICES_SHOW_STOCK_1 ? 1 : 0;
  dict[keys.PRICES_SHOW_STOCK_2] = settings.PRICES_SHOW_STOCK_2 ? 1 : 0;
  dict[keys.PRICES_SHOW_CRYPTO] = settings.PRICES_SHOW_CRYPTO ? 1 : 0;
  dict[keys.PRICES_CADENCE_MIN] = pricesCadenceMinutes(settings.PRICES_CADENCE_MIN);
  dict[keys.PRICES_POSITIVE_COLOR_LIGHT] =
      colorSetting(settings.PRICES_POSITIVE_COLOR_LIGHT, 0x000000);
  dict[keys.PRICES_POSITIVE_COLOR_DARK] =
      colorSetting(settings.PRICES_POSITIVE_COLOR_DARK, 0xFFFFFF);
  dict[keys.PRICES_NEGATIVE_COLOR_LIGHT] =
      colorSetting(settings.PRICES_NEGATIVE_COLOR_LIGHT, 0x000000);
  dict[keys.PRICES_NEGATIVE_COLOR_DARK] =
      colorSetting(settings.PRICES_NEGATIVE_COLOR_DARK, 0xFFFFFF);
  sendToWatch(dict, 'Prices setting');
  schedulePricesRefresh();
  refreshPricesOnce();
}

function yahooPriceUrl(symbol) {
  return 'https://query1.finance.yahoo.com/v8/finance/chart/'
      + encodeURIComponent(symbol) + '?interval=1d&range=2d';
}

function parseYahooChart(data) {
  try {
    var meta = data.chart.result[0].meta;
    var current = Number(meta.regularMarketPrice);
    var previous = Number(meta.chartPreviousClose);
    if (!isFinite(current) || !isFinite(previous) || previous === 0) {
      return null;
    }
    return {
      priceStr: current.toFixed(2),
      deltaX100: Math.round(((current - previous) / previous) * 10000)
    };
  } catch (e) {
    console.log('Prices Yahoo parse failed: ' + e);
    return null;
  }
}

function formatCryptoPrice(usd) {
  var price = Number(usd);
  if (!isFinite(price)) {
    return null;
  }
  if (price >= 1000) {
    return Math.round(price).toString();
  }
  return price.toFixed(2);
}

function fetchPrices(settings) {
  var stock1 = normalizeStockSymbol(settings.PRICES_STOCK_1_SYMBOL, 'SPY');
  var stock2 = normalizeStockSymbol(settings.PRICES_STOCK_2_SYMBOL, 'QQQ');
  var crypto = normalizeCryptoSymbol(settings.PRICES_CRYPTO_SYMBOL);
  var dict = {};
  var pending = 3;

  function done() {
    pending--;
    if (pending !== 0) {
      return;
    }
    dict[keys.PRICES_LAST_UPDATE_T] = Math.floor(Date.now() / 1000);
    sendToWatch(dict, 'Prices fetch');
  }

  fetchJson(yahooPriceUrl(stock1), 'Prices stock ' + stock1, function(data) {
    var result = data ? parseYahooChart(data) : null;
    if (result) {
      dict[keys.PRICES_STOCK_1_PRICE] = result.priceStr;
      dict[keys.PRICES_STOCK_1_DELTA_X100] = result.deltaX100;
    }
    done();
  });

  fetchJson(yahooPriceUrl(stock2), 'Prices stock ' + stock2, function(data) {
    var result = data ? parseYahooChart(data) : null;
    if (result) {
      dict[keys.PRICES_STOCK_2_PRICE] = result.priceStr;
      dict[keys.PRICES_STOCK_2_DELTA_X100] = result.deltaX100;
    }
    done();
  });

  var cryptoUrl = 'https://api.coingecko.com/api/v3/simple/price?ids='
      + encodeURIComponent(crypto)
      + '&vs_currencies=usd&include_24hr_change=true';
  fetchJson(cryptoUrl, 'Prices crypto ' + crypto, function(data) {
    if (data && data[crypto]) {
      var priceStr = formatCryptoPrice(data[crypto].usd);
      var change = Number(data[crypto].usd_24h_change);
      if (priceStr && isFinite(change)) {
        dict[keys.PRICES_CRYPTO_PRICE] = priceStr;
        dict[keys.PRICES_CRYPTO_DELTA_X100] = Math.round(change * 100);
      }
    }
    done();
  });
}

function schedulePricesRefresh() {
  var minutes = pricesCadenceMinutes(readSettings().PRICES_CADENCE_MIN);
  if (pricesTimer) {
    clearInterval(pricesTimer);
  }
  pricesTimer = setInterval(function() {
    fetchPrices(readSettings());
  }, minutes * 60 * 1000);
}

function refreshPricesOnce() {
  fetchPrices(readSettings());
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

// Tide hourly levels are sent as a 48-byte array centered on "now":
// indices 0..23 are the past 24 hours, index TIDE_NOW_INDEX = 24 is the
// current hour, indices 25..47 are the next 23 hours. The C side knows
// where "now" sits by the same TIDE_NOW_INDEX constant and renders either
// the centered 24-hour window or the full 48-hour window based on the
// TIDE_VIEW_HOURS Clay setting.
var TIDE_HOURS_BEFORE_NOW = 24;
var TIDE_WINDOW_HOURS = 48;

function noaaTimeKey(date) {
  // UTC components. Keys must be TZ-independent so they line up with NOAA's
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

  // Center the 24-hour window on "now". All math in epoch ms, no mutating
  // setUTCMinutes calls (some embedded JS engines fail those silently).
  var HOUR_MS = 60 * 60 * 1000;
  var nowMs = nowDate.getTime();
  var nowHourMs = Math.floor(nowMs / HOUR_MS) * HOUR_MS;
  var startMs = nowHourMs - TIDE_HOURS_BEFORE_NOW * HOUR_MS;

  // PebbleKit JS serializes byte-array tuples from a plain Array of integers
  // (0..255). Typed arrays fail to round-trip and tank the whole AppMessage,
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
        console.log('Tide: hilo empty, sending hourly-only data');
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

function nearestRainChance(hourly, utcOffsetSeconds) {
  if (!hourly || !hourly.time || !hourly.precipitation_probability) {
    return 0;
  }

  var now = Math.floor(Date.now() / 1000);
  var bestIndex = 0;
  var bestDistance = Infinity;
  hourly.time.forEach(function(timeValue, index) {
    var epochSeconds = openMeteoEpochSeconds(timeValue, utcOffsetSeconds);
    if (epochSeconds === null) {
      return;
    }
    var distance = Math.abs(epochSeconds - now);
    if (distance < bestDistance) {
      bestDistance = distance;
      bestIndex = index;
    }
  });

  return clamp(Math.round(hourly.precipitation_probability[bestIndex] || 0), 0, 100);
}

function openMeteoEpochSeconds(value, utcOffsetSeconds) {
  if (typeof value !== 'string') {
    return null;
  }
  var match = value.match(
      /^(\d{4})-(\d{2})-(\d{2})T(\d{2})(?::(\d{2}))?(?::(\d{2}))?$/);
  if (!match) {
    return null;
  }
  var offset = Number(utcOffsetSeconds) || 0;
  var epochMs = Date.UTC(
      Number(match[1]),
      Number(match[2]) - 1,
      Number(match[3]),
      Number(match[4]),
      Number(match[5] || 0),
      Number(match[6] || 0));
  if (!isFinite(epochMs)) {
    return null;
  }
  return Math.floor(epochMs / 1000) - offset;
}

function hourlyEpochSeconds(hourly, index, utcOffsetSeconds) {
  if (!hourly || !hourly.time || index < 0 || index >= hourly.time.length) {
    return null;
  }
  return openMeteoEpochSeconds(hourly.time[index], utcOffsetSeconds);
}

function nearestHourlyIndex(hourly, utcOffsetSeconds) {
  if (!hourly || !hourly.time || !hourly.time.length) {
    return -1;
  }

  var now = Math.floor(Date.now() / 1000);
  var bestIndex = 0;
  var bestDistance = Infinity;
  hourly.time.forEach(function(timeValue, index) {
    var epochSeconds = openMeteoEpochSeconds(timeValue, utcOffsetSeconds);
    if (epochSeconds === null) {
      return;
    }
    var distance = Math.abs(epochSeconds - now);
    if (distance < bestDistance) {
      bestDistance = distance;
      bestIndex = index;
    }
  });
  return bestIndex;
}

function localDayNumber(epochSeconds, utcOffsetSeconds) {
  return Math.floor((epochSeconds + (Number(utcOffsetSeconds) || 0)) / 86400);
}

function formatWeatherHourFromEpoch(epochSeconds, utcOffsetSeconds) {
  var offset = Number(utcOffsetSeconds) || 0;
  var date = new Date((epochSeconds + offset) * 1000);
  var hours = date.getUTCHours();
  var suffix = hours >= 12 ? 'P' : 'A';
  var hour = hours % 12 || 12;
  return hour + suffix;
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

// === Open-Meteo verbose summary v2 ===
// Same certainty-aware, pixel-fit aware design as the NWS v2 builder
// (nwsBuildSummaryCandidates / nwsBuildVerboseSummariesV2 farther below).
// WMO weather codes drive the classifier instead of NWS phrasing prefixes:
// PoP alone determines certainty (ACTIVE/CHANCE/SLIGHT/CLEAR); the WMO
// code determines the precip kind (STORMS/RAIN/SNOW/etc) when there is one.

function omClassifyHourState(code, pop) {
  pop = Number(pop) || 0;
  if (pop >= 60) return 'ACTIVE';
  if (pop >= 30) return 'CHANCE';
  if (pop >= 10) return 'SLIGHT';
  return 'CLEAR';
}

function omHourPrecipKind(hourly, index) {
  var code = hourlyWeatherCode(hourly, index);
  return code !== null ? weatherEventLabel(code) : null;
}

function omBaseLabelSimple(label) {
  switch (label) {
    case 'MOSTLY CLR': return 'CLEAR';
    case 'PARTLY CLDY': return 'CLOUDY';
    default: return label;
  }
}

// Scan the next 18 hourly periods for the first CLEAR hour and return its
// base label. If no CLEAR hour exists in the window, fall back to the
// current code's base label (if it's a clear/cloud code) or "CLOUDY"
// (sensible default for a chance-of-precip-throughout day).
function omDominantBase(hourly, currentCode, utcOffsetSeconds) {
  if (hourly && hourly.weather_code && hourly.precipitation_probability) {
    var startIdx = nearestHourlyIndex(hourly, utcOffsetSeconds);
    if (startIdx >= 0) {
      var maxScan = Math.min(hourly.weather_code.length, startIdx + 18);
      for (var i = startIdx; i < maxScan; i++) {
        var pop = hourlyRainChance(hourly, i);
        var code = hourlyWeatherCode(hourly, i);
        if (omClassifyHourState(code, pop) === 'CLEAR' && code !== null) {
          return weatherBaseLabel(code);
        }
      }
    }
  }
  var currCode = Number(currentCode);
  if (isFinite(currCode) && currCode >= 0 && currCode <= 48) {
    return weatherBaseLabel(currCode);
  }
  return 'CLOUDY';
}

function omFirstActiveIdx(hourly, utcOffsetSeconds) {
  if (!hourly || !hourly.weather_code) return -1;
  var startIdx = nearestHourlyIndex(hourly, utcOffsetSeconds);
  if (startIdx < 0) return -1;
  var maxScan = Math.min(hourly.weather_code.length, startIdx + 18);
  for (var i = startIdx; i < maxScan; i++) {
    if (omClassifyHourState(hourlyWeatherCode(hourly, i),
                            hourlyRainChance(hourly, i)) === 'ACTIVE') {
      return i;
    }
  }
  return -1;
}

function omEndOfActiveRun(hourly, startIdx) {
  if (!hourly || !hourly.weather_code) return -1;
  var maxScan = Math.min(hourly.weather_code.length, startIdx + 18);
  var dryRun = 0;
  var firstDry = -1;
  for (var i = startIdx + 1; i < maxScan; i++) {
    var state = omClassifyHourState(hourlyWeatherCode(hourly, i),
                                    hourlyRainChance(hourly, i));
    if (state !== 'ACTIVE') {
      if (dryRun === 0) firstDry = i;
      dryRun++;
      if (dryRun >= 3) return firstDry;
    } else {
      dryRun = 0;
      firstDry = -1;
    }
  }
  return -1;
}

function omPeakChanceInfo(hourly, utcOffsetSeconds) {
  var info = { pop: 0, kind: null, bucket: null, epoch: null };
  if (!hourly || !hourly.precipitation_probability) return info;
  var startIdx = nearestHourlyIndex(hourly, utcOffsetSeconds);
  if (startIdx < 0) return info;
  var maxScan = Math.min(hourly.precipitation_probability.length, startIdx + 18);
  for (var i = startIdx; i < maxScan; i++) {
    var pop = hourlyRainChance(hourly, i);
    if (pop > info.pop) {
      info.pop = pop;
      info.kind = omHourPrecipKind(hourly, i);
      info.epoch = hourlyEpochSeconds(hourly, i, utcOffsetSeconds);
      info.bucket = info.epoch !== null ? nwsTimeBucket(info.epoch, utcOffsetSeconds) : null;
    }
  }
  return info;
}

// Map current hour to the WMO code that should drive the icon, using the
// same certainty classifier as the verbose summary. Without this an OM
// current.weather_code of 63 (rain) with a 25% PoP would render a rain
// icon next to "CLOUDY, 25% RAIN PM" text.
function omDisplayWeatherCode(hourly, currentCode, utcOffsetSeconds) {
  if (!hourly || !hourly.weather_code) return Number(currentCode) || 3;
  var startIdx = nearestHourlyIndex(hourly, utcOffsetSeconds);
  if (startIdx < 0) return Number(currentCode) || 3;
  var state = omClassifyHourState(hourlyWeatherCode(hourly, startIdx),
                                  hourlyRainChance(hourly, startIdx));
  if (state === 'ACTIVE') {
    var code = hourlyWeatherCode(hourly, startIdx);
    return code !== null ? code : (Number(currentCode) || 3);
  }
  // CHANCE/SLIGHT/CLEAR: use dominant base for the icon so it matches the
  // text's base condition. Map the label back to a representative WMO code.
  var base = omDominantBase(hourly, currentCode, utcOffsetSeconds);
  switch (base) {
    case 'CLEAR': return 0;
    case 'MOSTLY CLR': return 1;
    case 'PARTLY CLDY': return 2;
    case 'CLOUDY': return 3;
    case 'FOG': return 45;
    default: return Number(currentCode) || 3;
  }
}

function omBuildSummaryCandidates(hourly, currentCode, utcOffsetSeconds) {
  if (!hourly || !hourly.weather_code || !hourly.precipitation_probability) {
    return [weatherBaseLabel(Number(currentCode))];
  }
  var startIdx = nearestHourlyIndex(hourly, utcOffsetSeconds);
  if (startIdx < 0) {
    return [weatherBaseLabel(Number(currentCode))];
  }
  var currentState = omClassifyHourState(hourlyWeatherCode(hourly, startIdx),
                                         hourlyRainChance(hourly, startIdx));
  var currentKind = omHourPrecipKind(hourly, startIdx);
  var firstActiveIdx = omFirstActiveIdx(hourly, utcOffsetSeconds);

  // Tier 1: currently ACTIVE.
  if (currentState === 'ACTIVE') {
    var kind = currentKind || weatherEventLabel(Number(currentCode)) || 'RAIN';
    var endIdx = omEndOfActiveRun(hourly, startIdx);
    if (endIdx > 0) {
      var endEpoch = hourlyEpochSeconds(hourly, endIdx, utcOffsetSeconds);
      var endTime = endEpoch !== null ? formatWeatherHourFromEpoch(endEpoch, utcOffsetSeconds) : '';
      return [kind + ' TIL ' + endTime, kind + ' TIL ' + endTime, kind];
    }
    return [kind + ' ALL DAY', kind + ' ALL DAY', kind];
  }

  // Tier 2: not active now, ACTIVE somewhere in next 18h.
  if (firstActiveIdx > startIdx) {
    var startEpoch = hourlyEpochSeconds(hourly, firstActiveIdx, utcOffsetSeconds);
    var startTime = startEpoch !== null ? formatWeatherHourFromEpoch(startEpoch, utcOffsetSeconds) : '';
    var kind2 = omHourPrecipKind(hourly, firstActiveIdx) || 'RAIN';
    var bucket2 = startEpoch !== null ? nwsTimeBucket(startEpoch, utcOffsetSeconds) : '';
    return [kind2 + ' AT ' + startTime, kind2 + ' AT ' + startTime, kind2 + ' ' + bucket2];
  }

  // Tier 3/4: chance window present, no ACTIVE in 18h. Base + qualifier.
  var peak = omPeakChanceInfo(hourly, utcOffsetSeconds);
  if (peak.pop >= 10) {
    var base = omDominantBase(hourly, currentCode, utcOffsetSeconds);
    var baseSimple = omBaseLabelSimple(base) || base;
    var kind3 = peak.kind || currentKind || 'RAIN';
    var pop = nwsRoundPop(peak.pop);
    var bucket3 = peak.bucket || '';
    var pctClause = pop + '% ' + kind3;
    var pctWithTime = pctClause + (bucket3 ? ' ' + bucket3 : '');
    var timeClause = (bucket3 ? bucket3 + ' ' : '') + kind3;

    var candidates = [];
    if (base) {
      candidates.push(base + ', ' + pctWithTime);
      candidates.push(base + ', ' + pctClause);
      candidates.push(base + ', ' + timeClause);
      candidates.push(baseSimple + ', ' + pctWithTime);
      candidates.push(baseSimple + ', ' + pctClause);
      candidates.push(baseSimple + ', ' + timeClause);
      candidates.push(base);
      candidates.push(baseSimple);
    } else {
      candidates.push(pctWithTime);
      candidates.push(pctClause);
    }
    return candidates;
  }

  // Tier 5: nothing precip in 18h.
  var currentHourCode = hourlyWeatherCode(hourly, startIdx);
  var fallbackBase = currentHourCode !== null
      ? weatherBaseLabel(currentHourCode)
      : (omDominantBase(hourly, currentCode, utcOffsetSeconds) || 'CLEAR');
  return [fallbackBase, omBaseLabelSimple(fallbackBase) || fallbackBase];
}

function omBuildVerboseSummariesV2(hourly, currentCode, utcOffsetSeconds) {
  var candidates = omBuildSummaryCandidates(hourly, currentCode, utcOffsetSeconds);
  var trimmed = candidates.map(function(s) { return String(s || '').replace(/\s+/g, ' ').trim(); });
  return {
    twoLine: pickFittingSummary(trimmed, SUMMARY_BUDGET_TWO_LINE),
    oneLine: pickFittingSummary(trimmed, SUMMARY_BUDGET_ONE_LINE)
  };
}

function firstDailyValue(daily, field) {
  if (!daily || !daily[field] || !daily[field].length) {
    return null;
  }
  var value = Number(daily[field][0]);
  return isFinite(value) ? value : null;
}

function firstDailyTimestamp(daily, field, utcOffsetSeconds) {
  if (!daily || !daily[field] || !daily[field].length) {
    return null;
  }
  var timestamp = openMeteoEpochSeconds(daily[field][0], utcOffsetSeconds);
  return isFinite(timestamp) ? timestamp : null;
}

function buildForecastPayload(hourly, utcOffsetSeconds) {
  if (!hourly || !hourly.temperature_2m || !hourly.precipitation_probability ||
      !hourly.time) {
    return null;
  }

  var startIndex = nearestHourlyIndex(hourly, utcOffsetSeconds);
  var startEpoch = hourlyEpochSeconds(hourly, startIndex, utcOffsetSeconds);
  if (startIndex < 0 || startEpoch === null) {
    return null;
  }

  var tempBytes = [];
  var precipBytes = [];
  for (var i = 0; i < 24; i++) {
    var sourceIndex = startIndex + i;
    if (sourceIndex >= hourly.time.length ||
        sourceIndex >= hourly.temperature_2m.length ||
        sourceIndex >= hourly.precipitation_probability.length) {
      return null;
    }

    var temp = Number(hourly.temperature_2m[sourceIndex]);
    if (!isFinite(temp)) {
      temp = 0;
    }
    temp = clamp(Math.round(temp), -128, 127);
    tempBytes.push(temp < 0 ? temp + 256 : temp);

    var precip = Number(hourly.precipitation_probability[sourceIndex]);
    if (!isFinite(precip)) {
      precip = 0;
    }
    precipBytes.push(clamp(Math.round(precip), 0, 100));
  }

  return {
    tempBytes: tempBytes,
    precipBytes: precipBytes,
    startEpoch: startEpoch
  };
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
      + '&hourly=temperature_2m,precipitation_probability,weather_code'
      + '&daily=temperature_2m_max,temperature_2m_min,uv_index_max,sunrise,sunset'
      + '&temperature_unit=' + encodeURIComponent(unitParam)
      + '&wind_speed_unit=mph'
      + '&forecast_days=2'
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
      // WEATHER_CODE goes through omDisplayWeatherCode so the icon respects
      // the v2 certainty classifier — a 31% chance of rain shouldn't render
      // a rain icon next to "CLOUDY, 30% RAIN AM" text. Same fix as on the
      // NWS path via nwsDisplayWeatherCode.
      addRounded(dict, keys.WEATHER_CODE,
                 omDisplayWeatherCode(data.hourly, data.current.weather_code, data.utc_offset_seconds),
                 0, 255);
      addRounded(dict, keys.WIND_SPEED, data.current.wind_speed_10m, 0, 255);
      addRounded(dict, keys.HIGH_TEMP, firstDailyValue(data.daily, 'temperature_2m_max'), -99, 127);
      addRounded(dict, keys.LOW_TEMP, firstDailyValue(data.daily, 'temperature_2m_min'), -99, 127);
      addRounded(dict, keys.UV_INDEX, firstDailyValue(data.daily, 'uv_index_max'), 0, 255);
      addRounded(dict, keys.SUNRISE_T,
                 firstDailyTimestamp(data.daily, 'sunrise', data.utc_offset_seconds),
                 0, 2147483647);
      addRounded(dict, keys.SUNSET_T,
                 firstDailyTimestamp(data.daily, 'sunset', data.utc_offset_seconds),
                 0, 2147483647);
      dict[keys.RAIN_CHANCE] = nearestRainChance(data.hourly, data.utc_offset_seconds);
      // v2 verbose summary: certainty-aware classifier + pixel-fit aware
      // candidate picker, same as the NWS path. Sends both two-line and
      // one-line variants so the renderer can pick the right one for the
      // current verbose-weather layout.
      var omSummaries = omBuildVerboseSummariesV2(data.hourly, data.current.weather_code, data.utc_offset_seconds);
      if (omSummaries.twoLine) {
        dict[keys.WEATHER_SUMMARY] = omSummaries.twoLine;
      }
      if (omSummaries.oneLine) {
        dict[keys.WEATHER_SUMMARY_COMPACT] = omSummaries.oneLine;
      }
      var forecast = buildForecastPayload(data.hourly, data.utc_offset_seconds);
      if (forecast) {
        dict[keys.FORECAST_TEMP_F] = forecast.tempBytes;
        dict[keys.FORECAST_PRECIP_PCT] = forecast.precipBytes;
        dict[keys.FORECAST_START_T] = forecast.startEpoch;
        dict[keys.FORECAST_LAST_UPDATE_T] = Math.floor(Date.now() / 1000);
      }
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

// ---------- NWS (National Weather Service) provider ----------

var NWS_USER_AGENT = 'jy-time-watchface (joshtyates@gmail.com)';
var NWS_HOURLY_HOURS = 24;
var NWS_LABEL_LEN = 23;
var NWS_SHORT_LEN = 39;
var NWS_DETAILED_LEN = 191;
var NWS_ALERT_LEN = 63;
var NWS_LOCATION_LEN = 31;

function weatherProviderId(settings) {
  return settings.WEATHER_PROVIDER === 'nws' ? 1 : 0;
}

function nwsClampInt8(value) {
  var v = Math.round(Number(value));
  if (!isFinite(v)) return 0;
  if (v < -127) return -127;
  if (v > 127) return 127;
  return v;
}

function nwsToShortString(value, max) {
  var s = String(value || '').replace(/\s+/g, ' ').trim();
  if (s.length > max) s = s.slice(0, max);
  return s;
}

function nwsLocationFromPoints(data, lat, lon) {
  if (!data || !data.properties) {
    return Number(lat).toFixed(2) + ',' + Number(lon).toFixed(2);
  }
  var rel = data.properties.relativeLocation;
  if (rel && rel.properties) {
    var city = rel.properties.city || '';
    var state = rel.properties.state || '';
    var label = (city && state) ? (city + ', ' + state) : (city || state);
    if (label) return label.toUpperCase();
  }
  return Number(lat).toFixed(2) + ',' + Number(lon).toFixed(2);
}

function nwsHourlyStartEpoch(hourly) {
  if (!hourly || !hourly.properties || !hourly.properties.periods
      || !hourly.properties.periods.length) {
    return 0;
  }
  var first = hourly.properties.periods[0].startTime;
  if (!first) return 0;
  var ms = Date.parse(first);
  return isFinite(ms) ? Math.floor(ms / 1000) : 0;
}

function nwsHourlyPackTemps(hourly) {
  var arr = [];
  if (hourly && hourly.properties && hourly.properties.periods) {
    var periods = hourly.properties.periods.slice(0, NWS_HOURLY_HOURS);
    for (var i = 0; i < periods.length; i++) {
      arr.push(nwsClampInt8(periods[i].temperature) & 0xFF);
    }
  }
  while (arr.length < NWS_HOURLY_HOURS) arr.push(0);
  return arr;
}

function nwsHourlyPackPrecip(hourly) {
  var arr = [];
  if (hourly && hourly.properties && hourly.properties.periods) {
    var periods = hourly.properties.periods.slice(0, NWS_HOURLY_HOURS);
    for (var i = 0; i < periods.length; i++) {
      var pp = periods[i].probabilityOfPrecipitation;
      var v = (pp && typeof pp.value !== 'undefined' && pp.value !== null)
          ? Number(pp.value) : 0;
      if (!isFinite(v)) v = 0;
      if (v < 0) v = 0;
      if (v > 100) v = 100;
      arr.push(Math.round(v));
    }
  }
  while (arr.length < NWS_HOURLY_HOURS) arr.push(0);
  return arr;
}

function nwsHttpGet(url, label, callback) {
  console.log(label + ' GET ' + url.slice(0, 110));
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
      finishOnce(JSON.parse(xhr.responseText));
    } catch (e) {
      console.log(label + ' parse failed: ' + e);
      finishOnce(null);
    }
  };
  xhr.onerror = function() { console.log(label + ' request failed'); finishOnce(null); };
  xhr.ontimeout = function() { console.log(label + ' timeout'); finishOnce(null); };
  try {
    xhr.open('GET', url);
    xhr.setRequestHeader('User-Agent', NWS_USER_AGENT);
    xhr.setRequestHeader('Accept', 'application/geo+json');
    xhr.send();
  } catch (e) {
    console.log(label + ' send threw: ' + e);
    finishOnce(null);
  }
}

function nwsPointsUrl(lat, lon) {
  // NWS rounds to 4 decimal places; reduce coordinate churn so the points
  // cache stays hot for typical phone-GPS jitter.
  var la = Number(lat).toFixed(4);
  var lo = Number(lon).toFixed(4);
  return 'https://api.weather.gov/points/' + la + ',' + lo;
}

function nwsAlertsUrl(lat, lon) {
  var la = Number(lat).toFixed(4);
  var lo = Number(lon).toFixed(4);
  return 'https://api.weather.gov/alerts/active?point=' + la + ',' + lo
      + '&status=actual';
}

function nwsCachedPoints(lat, lon) {
  var key = 'nws-points-' + Number(lat).toFixed(2) + ',' + Number(lon).toFixed(2);
  try {
    var raw = localStorage.getItem(key);
    if (!raw) return null;
    return JSON.parse(raw);
  } catch (e) { return null; }
}

function nwsStorePoints(lat, lon, data) {
  var key = 'nws-points-' + Number(lat).toFixed(2) + ',' + Number(lon).toFixed(2);
  try {
    localStorage.setItem(key, JSON.stringify(data));
  } catch (e) { /* ignore quota */ }
}

// Map NWS shortForecast text to the same base-label vocabulary Open-Meteo's
// weatherBaseLabel produces from WMO codes. Returns one of: CLEAR,
// MOSTLY CLR, PARTLY CLDY, CLOUDY, FOG, DRIZZLE, FRZ DRZL, RAIN, FRZ RAIN,
// SNOW, SHWRS, STORMS. Order of checks is precip-first so a phrase like
// "Mostly Sunny with Thunderstorms" resolves to STORMS.
function nwsBaseLabelFromShortForecast(text) {
  var s = String(text || '').toLowerCase();
  if (/thunderstorm/.test(s)) return 'STORMS';
  if (/freezing\s+rain/.test(s)) return 'FRZ RAIN';
  if (/freezing\s+drizzle/.test(s)) return 'FRZ DRZL';
  if (/snow|flurries|sleet/.test(s)) return 'SNOW';
  if (/shower/.test(s)) return 'SHWRS';
  if (/drizzle/.test(s)) return 'DRIZZLE';
  if (/rain/.test(s)) return 'RAIN';
  if (/fog|mist|haze|smoke/.test(s)) return 'FOG';
  if (/overcast|mostly\s+cloudy/.test(s)) return 'CLOUDY';
  if (/partly\s+(cloudy|sunny)/.test(s)) return 'PARTLY CLDY';
  if (/mostly\s+(sunny|clear)/.test(s)) return 'MOSTLY CLR';
  if (/sunny|clear/.test(s)) return 'CLEAR';
  if (/cloudy/.test(s)) return 'CLOUDY';
  return 'CLOUDY';
}

// Empirically measured pixel widths of FONT_KEY_GOTHIC_18_BOLD for printable
// ASCII (chars 32-126), captured by booting the watchface with a one-shot
// graphics_text_layout_get_content_size loop and reading the APP_LOG values
// from `pebble logs --emulator emery`. The summary row in two-line mode
// always uses this font, and in one-line mode it's the fallback when 24_BOLD
// doesn't fit — so sizing summaries to 18_BOLD's width budget guarantees
// they'll render in either mode. The degree symbol (°, UTF-8 0xC2 0xB0) is
// derived from string measurements: "75\xC2\xB0F " - "75F " = 7 px.
var RG18B_W = {
  32:4, 33:4, 34:7, 35:9, 36:9, 37:14, 38:10, 39:4, 40:6, 41:6,
  42:8, 43:10, 44:4, 45:7, 46:4, 47:8, 48:8, 49:8, 50:8, 51:8,
  52:8, 53:8, 54:8, 55:8, 56:8, 57:8, 58:4, 59:4, 60:8, 61:10,
  62:8, 63:8, 64:12, 65:9, 66:9, 67:9, 68:9, 69:8, 70:8, 71:9,
  72:9, 73:4, 74:7, 75:8, 76:8, 77:12, 78:9, 79:9, 80:9, 81:9,
  82:9, 83:9, 84:8, 85:9, 86:9, 87:12, 88:9, 89:10, 90:8, 91:5,
  92:8, 93:5, 94:8, 95:9, 96:6, 97:8, 98:8, 99:8, 100:8, 101:8,
  102:6, 103:8, 104:8, 105:4, 106:3, 107:7, 108:4, 109:12, 110:8, 111:8,
  112:8, 113:8, 114:6, 115:7, 116:6, 117:8, 118:8, 119:12, 120:8, 121:8,
  122:7, 123:6, 124:4, 125:6, 126:7
};
var RG18B_DEGREE_W = 7;
var RG18B_DEFAULT_W = 9; // safe upper bound for any unmeasured char

function measureG18B(text) {
  if (!text) return 0;
  var total = 0;
  var s = String(text);
  for (var i = 0; i < s.length; i++) {
    var c = s.charCodeAt(i);
    if (c === 0xC2 && i + 1 < s.length && s.charCodeAt(i + 1) === 0xB0) {
      total += RG18B_DEGREE_W;
      i++;
      continue;
    }
    total += (typeof RG18B_W[c] !== 'undefined') ? RG18B_W[c] : RG18B_DEFAULT_W;
  }
  return total;
}

// Pick the WMO weather code that should drive the C-side icon, using the
// same certainty-aware classifier as the v2 verbose summary. Without this,
// a current hour with shortForecast = "Chance Showers And Thunderstorms"
// would render a thunderstorm icon next to a "MOSTLY SUNNY" text — the
// icon mapper's /thunderstorm/ regex doesn't respect the "Chance" qualifier
// any more than the old text logic did. Match the text:
//   - ACTIVE current  → current shortForecast (storm icon when storming)
//   - CHANCE/SLIGHT/CLEAR → dominant base (sun/cloud icon, matches text)
function nwsDisplayWeatherCode(hourly, forecast) {
  if (!hourly || !hourly.properties || !hourly.properties.periods
      || !hourly.properties.periods.length) {
    return 3;
  }
  var p0 = hourly.properties.periods[0];
  if (nwsClassifyHourState(p0) === 'ACTIVE') {
    return nwsShortForecastToWmoCode(p0.shortForecast);
  }
  var base = nwsDominantBase(hourly, forecast);
  if (base) {
    return nwsShortForecastToWmoCode(base);
  }
  return nwsShortForecastToWmoCode(p0.shortForecast);
}

// Verbose-weather summary pixel budgets in 18_BOLD:
//   - Two-line layout: GRect(8, ..., SCREEN_W - 16, 20) = 184 px hard cap.
//   - One-line layout: row text = "<temp>°F <summary>" shares
//     max_row_width(192) - icon(18) - gap(4) = 170 px. Worst-case temp
//     "100°F " = 43 px; so summary needs to fit in 170 - 43 = 127 px in
//     18_BOLD. Round down to 125 for safety margin. (24_BOLD is still tried
//     first by the C-side renderer when the whole row fits there.)
var SUMMARY_BUDGET_TWO_LINE = 184;
var SUMMARY_BUDGET_ONE_LINE = 125;

// Pick the widest candidate that fits within budgetPx. Candidates should
// already be ordered most-detailed first, so this prefers fuller summaries
// whenever they fit. Returns the last candidate (most-truncated) if none fit
// — that one was designed for a tight budget and should always render.
function pickFittingSummary(candidates, budgetPx) {
  for (var i = 0; i < candidates.length; i++) {
    var s = candidates[i];
    if (!s) continue;
    if (measureG18B(s) <= budgetPx) return s;
  }
  return candidates[candidates.length - 1] || '';
}

// Map NWS shortForecast to a WMO weather code so the C-side icon mapper
// (which speaks Open-Meteo's WMO codes) can render the right glyph when
// NWS is the primary provider. Mirrors the regex order in
// nwsBaseLabelFromShortForecast so "Mostly Sunny with Thunderstorms"
// resolves to a storm icon.
function nwsShortForecastToWmoCode(text) {
  var s = String(text || '').toLowerCase();
  if (/thunderstorm/.test(s)) return 95;
  if (/freezing\s+rain/.test(s)) return 66;
  if (/freezing\s+drizzle/.test(s)) return 57;
  if (/snow|flurries|sleet/.test(s)) return 73;
  if (/shower/.test(s)) return 81;
  if (/drizzle/.test(s)) return 53;
  if (/rain/.test(s)) return 63;
  if (/fog|mist|haze|smoke/.test(s)) return 45;
  if (/overcast|mostly\s+cloudy/.test(s)) return 3;
  if (/partly\s+(cloudy|sunny)/.test(s)) return 2;
  if (/mostly\s+(sunny|clear)/.test(s)) return 1;
  if (/sunny|clear/.test(s)) return 0;
  if (/cloudy/.test(s)) return 3;
  return 3;
}

// Map NWS shortForecast to Open-Meteo's event-label subset (precip events
// only). Returns null when no precip is present in the phrase.
function nwsEventLabelFromShortForecast(text) {
  var s = String(text || '').toLowerCase();
  if (/thunderstorm/.test(s)) return 'STORMS';
  if (/freezing\s+rain/.test(s)) return 'FRZ RAIN';
  if (/freezing\s+drizzle/.test(s)) return 'FRZ DRZL';
  if (/snow|flurries|sleet/.test(s)) return 'SNOW';
  if (/shower/.test(s)) return 'SHWRS';
  if (/rain/.test(s)) return 'RAIN';
  if (/drizzle/.test(s)) return 'DRIZZLE';
  return null;
}

function nwsPeriodEpoch(period) {
  if (!period || !period.startTime) return null;
  var ms = Date.parse(period.startTime);
  return isFinite(ms) ? Math.floor(ms / 1000) : null;
}

// Pull today's high / low from NWS's 12-hour forecast periods. Looks at
// the first three periods so the late-evening case (periods[0] is
// "Tonight") still surfaces a sensible high from tomorrow's daytime
// period. Returns nulls when a side is missing from a partial response.
function nwsTodayHiLo(forecast) {
  var result = { high: null, low: null };
  var periods = (forecast && forecast.properties && forecast.properties.periods) || [];
  for (var i = 0; i < periods.length && i < 3; i++) {
    var p = periods[i];
    if (!p || typeof p.temperature === 'undefined') continue;
    var temp = Number(p.temperature);
    if (!isFinite(temp)) continue;
    if (p.isDaytime && result.high === null) {
      result.high = temp;
    } else if (!p.isDaytime && result.low === null) {
      result.low = temp;
    }
  }
  return result;
}

function nwsPeriodPrecipChance(period) {
  if (!period || !period.probabilityOfPrecipitation) return 0;
  var v = Number(period.probabilityOfPrecipitation.value);
  return isFinite(v) ? clamp(Math.round(v), 0, 100) : 0;
}

function nwsHourlyOffsetSeconds(hourly) {
  if (!hourly || !hourly.properties || !hourly.properties.periods
      || !hourly.properties.periods.length) {
    return 0;
  }
  var t = String(hourly.properties.periods[0].startTime || '');
  var m = t.match(/([+-])(\d{2}):(\d{2})$/);
  if (!m) return 0;
  var sign = m[1] === '-' ? -1 : 1;
  return sign * ((Number(m[2]) * 3600) + (Number(m[3]) * 60));
}

function nwsLocalDayNumber(epochSeconds, offsetSeconds) {
  return Math.floor((epochSeconds + (Number(offsetSeconds) || 0)) / 86400);
}

function nwsFormatHourFromEpoch(epochSeconds, offsetSeconds) {
  var offset = Number(offsetSeconds) || 0;
  var date = new Date((epochSeconds + offset) * 1000);
  var hours = date.getUTCHours();
  var suffix = hours >= 12 ? 'P' : 'A';
  var hour = hours % 12 || 12;
  return hour + suffix;
}

// === NWS verbose summary (v2): certainty-aware, pixel-fit aware ===
//
// Classifies each hour into ACTIVE / CHANCE / SLIGHT / CLEAR using NWS's
// own phrasing prefix ("Chance", "Slight Chance", "Likely", bare) AND the
// explicit PoP. "@ [Time]" / "TIL [Time]" only appears at true ACTIVE
// transitions (active precip starting / ending); chance levels get a
// "+X% KIND" qualifier on top of the dominant base condition instead. The
// dominant base label comes from the first non-chance hour in the next 18h,
// falling back to the 12-hour forecast period's detailedForecast prose if
// no clear hour shows up in that window. Mirror of omBuildSummaryCandidates
// for the Open-Meteo path.

// Classifier states used by the new logic.
// ACTIVE: precip is happening or essentially certain (PoP>=60 or bare phrase
//   like "Showers And Thunderstorms" with no Chance/Slight Chance qualifier
//   or phrase contains "Likely").
// CHANCE: PoP 30-59 OR phrase has "Chance " (but not "Slight Chance").
// SLIGHT: PoP 10-29 OR phrase has "Slight Chance"/"Areas of"/"Patchy".
// CLEAR : PoP<10 AND no precip term in shortForecast.
function nwsClassifyHourState(period) {
  if (!period) return 'CLEAR';
  var s = String(period.shortForecast || '').toLowerCase();
  var pop = nwsPeriodPrecipChance(period);
  var hasPrecipTerm = /thunderstorm|rain|shower|snow|flurries|sleet|drizzle|freezing/.test(s);
  var hasSlight = /slight\s+chance|areas\s+of|patchy/.test(s);
  var hasChance = /\bchance\b/.test(s) && !/slight\s+chance/.test(s);
  var hasLikely = /\blikely\b/.test(s);
  var hasBarePrecip = hasPrecipTerm && !hasSlight && !hasChance && !hasLikely;
  if (hasLikely || hasBarePrecip || pop >= 60) return 'ACTIVE';
  if (hasChance || pop >= 30) return 'CHANCE';
  if (hasSlight || pop >= 10) return 'SLIGHT';
  return 'CLEAR';
}

// Map an hour's shortForecast to the precip kind being forecast (independent
// of certainty). Returns null when no precip term is present at all.
function nwsHourPrecipKind(period) {
  if (!period) return null;
  return nwsEventLabelFromShortForecast(period.shortForecast);
}

// Compact base-condition labels suited to the verbose-weather pixel budgets.
// Mirrors nwsBaseLabelFromShortForecast but uses shorter forms ("CLR" for
// CLEAR, "M CLDY" for MOSTLY CLOUDY) so the base alone leaves room for a
// qualifier suffix in tight budgets.
function nwsBaseLabelCompact(text) {
  var s = String(text || '').toLowerCase();
  if (/mostly\s+sunny/.test(s)) return 'MOSTLY SUNNY';
  if (/mostly\s+clear/.test(s)) return 'MOSTLY CLR';
  if (/partly\s+(cloudy|sunny)/.test(s)) return 'PARTLY CLDY';
  if (/mostly\s+cloudy|overcast/.test(s)) return 'MOSTLY CLDY';
  if (/fog|mist|haze|smoke/.test(s)) return 'FOG';
  if (/sunny/.test(s)) return 'SUNNY';
  if (/clear/.test(s)) return 'CLEAR';
  if (/cloudy/.test(s)) return 'CLOUDY';
  return '';
}

// When the full base + qualifier doesn't fit, drop the "MOSTLY"/"PARTLY"
// modifier and keep a plain word. "M SUNNY" / "PT CLDY" reads as a typo on
// a watch face; "SUNNY" or "CLOUDY" reads as a natural sentence. This loses
// some nuance ("partly cloudy" → "cloudy") but the qualifier (chance + kind
// + time bucket) is the load-bearing information.
function nwsBaseLabelSimple(text) {
  var full = nwsBaseLabelCompact(text);
  switch (full) {
    case 'MOSTLY SUNNY': return 'SUNNY';
    case 'MOSTLY CLR':   return 'CLEAR';
    case 'PARTLY CLDY':  return 'CLOUDY';
    case 'MOSTLY CLDY':  return 'CLOUDY';
    default: return full;
  }
}

// Parse the detailedForecast prose for the dominant base condition. NWS
// detailed text typically reads: "A chance of showers and thunderstorms
// before 4pm. Mostly sunny, with a high near 86. [...]" — the chance
// qualifier is the lead, the base condition is the second sentence. Scan
// the full string for any known base phrase and return the first match.
function nwsExtractBaseFromDetailed(text) {
  var s = String(text || '').toLowerCase();
  var patterns = [
    [/mostly\s+sunny/, 'MOSTLY SUNNY'],
    [/mostly\s+clear/, 'MOSTLY CLR'],
    [/partly\s+(cloudy|sunny)/, 'PARTLY CLDY'],
    [/mostly\s+cloudy/, 'MOSTLY CLDY'],
    [/\bovercast\b/, 'MOSTLY CLDY'],
    [/\bsunny\b/, 'SUNNY'],
    [/\bclear\b/, 'CLEAR'],
    [/\bcloudy\b/, 'CLOUDY'],
    [/\bfog|\bmist|\bhaze/, 'FOG']
  ];
  for (var i = 0; i < patterns.length; i++) {
    if (patterns[i][0].test(s)) return patterns[i][1];
  }
  return '';
}

// Find the dominant base condition for use in CHANCE/SLIGHT-tier summaries.
// Strategy: scan the next 18 hourly periods looking for the first CLEAR
// hour; use its shortForecast. If no CLEAR hour is found in the window
// (e.g., the whole day has at least chance of precip threaded through),
// fall back to the 12-hour forecast.periods[0].detailedForecast and pull
// the base condition phrase out of its prose. Returns '' if neither yields
// a known base.
function nwsDominantBase(hourly, forecast) {
  if (hourly && hourly.properties && hourly.properties.periods) {
    var periods = hourly.properties.periods;
    var maxScan = Math.min(periods.length, 18);
    for (var i = 0; i < maxScan; i++) {
      if (nwsClassifyHourState(periods[i]) === 'CLEAR') {
        var label = nwsBaseLabelCompact(periods[i].shortForecast);
        if (label) return label;
      }
    }
  }
  var fp = forecast && forecast.properties && forecast.properties.periods;
  if (fp && fp.length) {
    return nwsExtractBaseFromDetailed(fp[0].detailedForecast);
  }
  return '';
}

// Bucket an epoch into AM (0-11), PM (12-17), EVE (18-23), LATE (0-5 next).
// Used as the time hint on CHANCE/SLIGHT qualifiers where a specific time
// would overstate precision.
function nwsTimeBucket(epochSec, offsetSec) {
  var offset = Number(offsetSec) || 0;
  var d = new Date((epochSec + offset) * 1000);
  var h = d.getUTCHours();
  if (h < 6)  return 'LATE';
  if (h < 12) return 'AM';
  if (h < 18) return 'PM';
  return 'EVE';
}

// Find first hour in next 18 that classifies ACTIVE; returns index or -1.
function nwsFirstActiveIdx(hourly) {
  if (!hourly || !hourly.properties || !hourly.properties.periods) return -1;
  var periods = hourly.properties.periods;
  var maxScan = Math.min(periods.length, 18);
  for (var i = 0; i < maxScan; i++) {
    if (nwsClassifyHourState(periods[i]) === 'ACTIVE') return i;
  }
  return -1;
}

// Find the end of an ACTIVE run starting at startIdx: the first hour after
// startIdx where state != ACTIVE AND the next 2 hours are also not ACTIVE
// (3-consecutive-non-ACTIVE rule). Returns the index of the first
// non-ACTIVE hour, or -1 if ACTIVE persists through the end of the window.
function nwsEndOfActiveRun(hourly, startIdx) {
  if (!hourly || !hourly.properties || !hourly.properties.periods) return -1;
  var periods = hourly.properties.periods;
  var maxScan = Math.min(periods.length, 18);
  var dryRun = 0;
  var firstDry = -1;
  for (var i = startIdx + 1; i < maxScan; i++) {
    if (nwsClassifyHourState(periods[i]) !== 'ACTIVE') {
      if (dryRun === 0) firstDry = i;
      dryRun++;
      if (dryRun >= 3) return firstDry;
    } else {
      dryRun = 0;
      firstDry = -1;
    }
  }
  return -1;
}

// Find peak PoP and its time bucket across the chance window in next 18h.
function nwsPeakChanceInfo(hourly) {
  var info = { pop: 0, kind: null, bucket: null, epoch: null };
  if (!hourly || !hourly.properties || !hourly.properties.periods) return info;
  var periods = hourly.properties.periods;
  var offset = nwsHourlyOffsetSeconds(hourly);
  var maxScan = Math.min(periods.length, 18);
  for (var i = 0; i < maxScan; i++) {
    var pop = nwsPeriodPrecipChance(periods[i]);
    if (pop > info.pop) {
      info.pop = pop;
      info.kind = nwsHourPrecipKind(periods[i]);
      info.epoch = nwsPeriodEpoch(periods[i]);
      info.bucket = info.epoch !== null ? nwsTimeBucket(info.epoch, offset) : null;
    }
  }
  return info;
}

// Round a probability down to the nearest 5 so the displayed number doesn't
// drift hour-to-hour on noise (e.g., 31% vs 33% renders identically).
function nwsRoundPop(pop) {
  return Math.max(0, Math.floor(Number(pop) / 5) * 5);
}

// Build a list of candidate summary strings for the current weather state.
// Ordered most-detailed first so pickFittingSummary returns the longest
// candidate that fits the caller's pixel budget.
function nwsBuildSummaryCandidates(hourly, forecast) {
  if (!hourly || !hourly.properties || !hourly.properties.periods
      || !hourly.properties.periods.length) {
    return [''];
  }
  var periods = hourly.properties.periods;
  var offset = nwsHourlyOffsetSeconds(hourly);
  var currentState = nwsClassifyHourState(periods[0]);
  var currentKind = nwsHourPrecipKind(periods[0]);
  var firstActiveIdx = nwsFirstActiveIdx(hourly);

  // Tier 1: currently ACTIVE.
  if (currentState === 'ACTIVE') {
    var kind = currentKind || 'RAIN';
    var endIdx = nwsEndOfActiveRun(hourly, 0);
    if (endIdx > 0) {
      var endEpoch = nwsPeriodEpoch(periods[endIdx]);
      var endTime = endEpoch !== null ? nwsFormatHourFromEpoch(endEpoch, offset) : '';
      return [
        kind + ' TIL ' + endTime,
        kind + ' TIL ' + endTime,
        kind
      ];
    }
    return [kind + ' ALL DAY', kind + ' ALL DAY', kind];
  }

  // Tier 2: not active now, ACTIVE somewhere in next 18h.
  if (firstActiveIdx > 0) {
    var startEpoch = nwsPeriodEpoch(periods[firstActiveIdx]);
    var startTime = startEpoch !== null ? nwsFormatHourFromEpoch(startEpoch, offset) : '';
    var kind2 = nwsHourPrecipKind(periods[firstActiveIdx]) || 'RAIN';
    var bucket2 = startEpoch !== null ? nwsTimeBucket(startEpoch, offset) : '';
    return [
      kind2 + ' AT ' + startTime,
      kind2 + ' AT ' + startTime,
      kind2 + ' ' + bucket2
    ];
  }

  // Tier 3/4: chance window present, no ACTIVE in 18h. Build base + qualifier.
  // Phrasing rules (chosen for naturalness over information density):
  //   - "30% STORMS AM" (number then kind then time bucket) reads as a
  //     percentage statement; "+30%" reads as offset notation.
  //   - Base names stay full ("MOSTLY SUNNY") or simplify whole-word
  //     ("SUNNY") rather than abbreviating to "M SUN" which reads as a typo.
  //   - Comma separates the base clause from the chance clause so the line
  //     parses as English ("Mostly sunny, with 30% storms in the morning").
  //   - When even base + chance doesn't fit, drop the chance clause rather
  //     than mangling either side — at low PoP (~30%) the base condition is
  //     the dominant truth, so falling back to just the base is honest.
  var peak = nwsPeakChanceInfo(hourly);
  if (peak.pop >= 10) {
    var base = nwsDominantBase(hourly, forecast);
    var baseSimple = nwsBaseLabelSimple(base) || base;
    var kind3 = peak.kind || currentKind || 'RAIN';
    var pop = nwsRoundPop(peak.pop);
    var bucket3 = peak.bucket || '';
    var pctClause = pop + '% ' + kind3;          // "30% STORMS"
    var pctWithTime = pctClause + (bucket3 ? ' ' + bucket3 : '');
    var timeClause = (bucket3 ? bucket3 + ' ' : '') + kind3;  // "AM STORMS"

    var candidates = [];
    if (base) {
      candidates.push(base + ', ' + pctWithTime);              // MOSTLY SUNNY, 30% STORMS AM
      candidates.push(base + ', ' + pctClause);                // MOSTLY SUNNY, 30% STORMS
      candidates.push(base + ', ' + timeClause);               // MOSTLY SUNNY, AM STORMS
      candidates.push(baseSimple + ', ' + pctWithTime);        // SUNNY, 30% STORMS AM
      candidates.push(baseSimple + ', ' + pctClause);          // SUNNY, 30% STORMS
      candidates.push(baseSimple + ', ' + timeClause);         // SUNNY, AM STORMS
      candidates.push(base);                                    // MOSTLY SUNNY
      candidates.push(baseSimple);                              // SUNNY
    } else {
      candidates.push(pctWithTime);
      candidates.push(pctClause);
    }
    return candidates;
  }

  // Tier 5: nothing precip in 18h. Just the base condition. Prefer the full
  // name; the simple form is the fallback if the full one ever stretches.
  var fallbackBase = nwsBaseLabelCompact(periods[0].shortForecast)
                  || nwsDominantBase(hourly, forecast)
                  || 'CLEAR';
  return [
    fallbackBase,
    nwsBaseLabelSimple(fallbackBase) || fallbackBase
  ];
}

// Build the two-line and one-line verbose summaries from the candidate list.
function nwsBuildVerboseSummariesV2(hourly, forecast) {
  var candidates = nwsBuildSummaryCandidates(hourly, forecast);
  var trimmed = candidates.map(function(s) { return String(s || '').replace(/\s+/g, ' ').trim(); });
  return {
    twoLine: pickFittingSummary(trimmed, SUMMARY_BUDGET_TWO_LINE),
    oneLine: pickFittingSummary(trimmed, SUMMARY_BUDGET_ONE_LINE)
  };
}

function nwsSendData(lat, lon, points, forecast, hourly, alerts) {
  // The full NWS payload runs ~1200 bytes when serialized as one AppMessage
  // dict (three ~192-char detailed narratives plus hourly arrays). Pebble's
  // default inbox is 512 bytes, so a single dict that large is dropped
  // silently. Splitting into 4 frames keeps each well under 512 bytes; the
  // C-side inbox handler processes them independently and the static state
  // converges to the full picture once all frames land.

  var alertTitle = '';
  if (alerts && alerts.features && alerts.features.length) {
    var first = alerts.features[0];
    if (first && first.properties) {
      alertTitle = first.properties.event || first.properties.headline || '';
    }
  }

  // Frame A — header / hourly / alert / verbose-weather override. ~240 bytes.
  // WEATHER_SUMMARY here overrides whatever Open-Meteo's
  // verboseWeatherSummary() previously wrote, because NWS runs after the
  // Open-Meteo fetch in refreshWeather's chain.
  var dictA = {};
  dictA[keys.NWS_LOCATION_LABEL] =
      nwsToShortString(nwsLocationFromPoints(points, lat, lon), NWS_LOCATION_LEN);
  if (hourly) {
    dictA[keys.NWS_HOURLY_TEMPS_F] = nwsHourlyPackTemps(hourly);
    dictA[keys.NWS_HOURLY_PRECIP_PCT] = nwsHourlyPackPrecip(hourly);
    dictA[keys.NWS_HOURLY_START_T] = nwsHourlyStartEpoch(hourly);
    // v2 verbose summary: certainty-aware (respects "Chance" vs "Likely" vs
    // bare phrasing) and pixel-fit aware (picks the longest candidate that
    // fits the budget for the user's verbose-weather layout). Open-Meteo
    // runs the same v2 algorithm via omBuildVerboseSummariesV2 on its own
    // hourly arrays — both providers produce strings from the same taxonomy.
    var summaries = nwsBuildVerboseSummariesV2(hourly, forecast);
    if (summaries.twoLine) {
      dictA[keys.WEATHER_SUMMARY] = summaries.twoLine;
    }
    if (summaries.oneLine) {
      dictA[keys.WEATHER_SUMMARY_COMPACT] = summaries.oneLine;
    }
    // Override the shared current-conditions keys so the main face, "Your
    // day" overlay, and any complication that reads RAIN_CHANCE /
    // WEATHER_CODE follows NWS when it's the primary provider. Same
    // override-after-Open-Meteo pattern as WEATHER_SUMMARY above.
    // WEATHER_CODE goes through nwsDisplayWeatherCode so the icon respects
    // the same certainty classifier as the verbose summary (a 31% chance
    // shouldn't render a storm icon next to "MOSTLY SUNNY" text).
    var periods0 = hourly.properties && hourly.properties.periods;
    if (periods0 && periods0.length) {
      var p0 = periods0[0];
      dictA[keys.RAIN_CHANCE] = nwsPeriodPrecipChance(p0);
      dictA[keys.WEATHER_CODE] = nwsDisplayWeatherCode(hourly, forecast);
      // Current temperature and wind speed from NWS hourly's current hour,
      // so the main-face temp complication and wind complication follow
      // NWS too. windSpeed comes as a string like "10 mph" or "10 to 20 mph"
      // — pull the first integer.
      addRounded(dictA, keys.TEMPERATURE, p0.temperature, -99, 127);
      var windMatch = String(p0.windSpeed || '').match(/(\d+)/);
      if (windMatch) {
        addRounded(dictA, keys.WIND_SPEED, parseInt(windMatch[1], 10), 0, 255);
      }
    }
    // Detailed Weather graph (24h temp + 24h precip) reflects NWS data.
    // Same pack helpers we already use for the NWS_HOURLY_* keys, just
    // routed to the FORECAST_* keys the Open-Meteo Detailed Weather
    // overlay reads from. FEELS_LIKE / UV_INDEX / SUNRISE_T / SUNSET_T
    // intentionally stay on Open-Meteo (NWS doesn't have them).
    dictA[keys.FORECAST_TEMP_F] = nwsHourlyPackTemps(hourly);
    dictA[keys.FORECAST_PRECIP_PCT] = nwsHourlyPackPrecip(hourly);
    dictA[keys.FORECAST_START_T] = nwsHourlyStartEpoch(hourly);
    dictA[keys.FORECAST_LAST_UPDATE_T] = Math.floor(Date.now() / 1000);
  }
  var hiLo = nwsTodayHiLo(forecast);
  addRounded(dictA, keys.HIGH_TEMP, hiLo.high, -99, 127);
  addRounded(dictA, keys.LOW_TEMP, hiLo.low, -99, 127);
  dictA[keys.NWS_ALERT_TITLE] = nwsToShortString(alertTitle, NWS_ALERT_LEN);
  dictA[keys.NWS_LAST_UPDATE_T] = Math.floor(Date.now() / 1000);
  sendToWatch(dictA, 'NWS A (hourly + alert)');

  // Frames B/C/D — one period each. ~300 bytes apiece.
  var periods = (forecast && forecast.properties && forecast.properties.periods) || [];
  var slots = [
    { lbl: keys.NWS_P1_LABEL, sh: keys.NWS_P1_SHORT, det: keys.NWS_P1_DETAILED, tmp: keys.NWS_P1_TEMP, label: 'NWS B (period 1)' },
    { lbl: keys.NWS_P2_LABEL, sh: keys.NWS_P2_SHORT, det: keys.NWS_P2_DETAILED, tmp: keys.NWS_P2_TEMP, label: 'NWS C (period 2)' },
    { lbl: keys.NWS_P3_LABEL, sh: keys.NWS_P3_SHORT, det: keys.NWS_P3_DETAILED, tmp: keys.NWS_P3_TEMP, label: 'NWS D (period 3)' }
  ];
  for (var i = 0; i < slots.length; i++) {
    var p = periods[i] || {};
    var dictP = {};
    dictP[slots[i].lbl] = nwsToShortString(p.name || '', NWS_LABEL_LEN).toUpperCase();
    dictP[slots[i].sh] = nwsToShortString(p.shortForecast || '', NWS_SHORT_LEN);
    dictP[slots[i].det] = nwsToShortString(p.detailedForecast || '', NWS_DETAILED_LEN);
    dictP[slots[i].tmp] = nwsClampInt8(p.temperature || 0);
    sendToWatch(dictP, slots[i].label);
  }
}

function nwsCachedZipCoords(zip) {
  try {
    var raw = localStorage.getItem('nws-zip-' + zip);
    if (!raw) return null;
    var parsed = JSON.parse(raw);
    if (parsed && isFinite(parsed.lat) && isFinite(parsed.lon)) {
      return parsed;
    }
  } catch (e) { /* fall through */ }
  return null;
}

function resolveZipToLatLon(zip, cb) {
  var clean = String(zip || '').trim();
  if (!/^\d{5}$/.test(clean)) {
    cb(null);
    return;
  }
  var cached = nwsCachedZipCoords(clean);
  if (cached) {
    cb(cached);
    return;
  }
  var url = 'https://api.zippopotam.us/us/' + clean;
  var xhr = new XMLHttpRequest();
  var finished = false;
  function finishOnce(result) {
    if (finished) return;
    finished = true;
    cb(result);
  }
  xhr.timeout = 12000;
  xhr.onload = function() {
    if (xhr.status && xhr.status !== 200) {
      console.log('ZIP lookup HTTP ' + xhr.status + ' for ' + clean);
      finishOnce(null);
      return;
    }
    try {
      var data = JSON.parse(xhr.responseText);
      if (!data || !data.places || !data.places.length) {
        finishOnce(null);
        return;
      }
      var place = data.places[0];
      var lat = Number(place.latitude);
      var lon = Number(place.longitude);
      if (!isFinite(lat) || !isFinite(lon)) {
        finishOnce(null);
        return;
      }
      var resolved = { lat: lat, lon: lon };
      try {
        localStorage.setItem('nws-zip-' + clean, JSON.stringify(resolved));
      } catch (e) { /* quota — ignore */ }
      finishOnce(resolved);
    } catch (e) {
      console.log('ZIP parse failed: ' + e);
      finishOnce(null);
    }
  };
  xhr.onerror = function() { console.log('ZIP request failed for ' + clean); finishOnce(null); };
  xhr.ontimeout = function() { console.log('ZIP timeout for ' + clean); finishOnce(null); };
  try {
    xhr.open('GET', url);
    xhr.send();
  } catch (e) {
    console.log('ZIP send threw: ' + e);
    finishOnce(null);
  }
}

function nwsLocateAndFetch(settings, fallbackLat, fallbackLon, done) {
  var zip = String(settings.NWS_ZIP || '').trim();
  if (zip) {
    resolveZipToLatLon(zip, function(coords) {
      if (coords) {
        nwsFetchForCoordinates(coords.lat, coords.lon, done);
      } else {
        console.log('ZIP ' + zip + ' could not resolve, falling back to GPS/manual lat-lon');
        if (isFinite(fallbackLat) && isFinite(fallbackLon)) {
          nwsFetchForCoordinates(fallbackLat, fallbackLon, done);
        } else if (done) {
          done();
        }
      }
    });
    return;
  }
  if (isFinite(fallbackLat) && isFinite(fallbackLon)) {
    nwsFetchForCoordinates(fallbackLat, fallbackLon, done);
  } else if (done) {
    done();
  }
}

function nwsFetchForCoordinates(lat, lon, done) {
  function withPoints(points) {
    if (!points || !points.properties || !points.properties.forecast
        || !points.properties.forecastHourly) {
      console.log('NWS: invalid points response — fallback gracefully');
      if (done) done();
      return;
    }
    nwsStorePoints(lat, lon, points);

    var forecastUrl = points.properties.forecast;
    var hourlyUrl = points.properties.forecastHourly;
    nwsHttpGet(forecastUrl, 'NWS forecast', function(forecast) {
      nwsHttpGet(hourlyUrl, 'NWS hourly', function(hourly) {
        nwsHttpGet(nwsAlertsUrl(lat, lon), 'NWS alerts', function(alerts) {
          nwsSendData(lat, lon, points, forecast, hourly, alerts);
          if (done) done();
        });
      });
    });
  }

  var cached = nwsCachedPoints(lat, lon);
  if (cached) {
    withPoints(cached);
    return;
  }
  nwsHttpGet(nwsPointsUrl(lat, lon), 'NWS points', withPoints);
}

// ---------- end NWS provider ----------

function refreshWeather(skipTide) {
  var settings = readSettings();
  var refreshTide = function() {
    if (!skipTide) {
      refreshTidesForSettings(settings);
    }
  };
  var refreshNws = function(lat, lon, andThen) {
    if (settings.WEATHER_PROVIDER === 'nws') {
      nwsLocateAndFetch(settings, lat, lon, andThen);
    } else if (andThen) {
      andThen();
    }
  };

  // Always send the current provider id to the watch so the overlay knows
  // which provider is authoritative (and shows the right placeholder text
  // when not configured).
  var providerDict = {};
  providerDict[keys.WEATHER_PROVIDER] = weatherProviderId(settings);
  sendToWatch(providerDict, 'Weather provider');

  if (!settings.WEATHER_ENABLED) {
    refreshTide();
    return;
  }

  var manualLat = parseFloat(settings.WEATHER_LAT);
  var manualLon = parseFloat(settings.WEATHER_LON);
  var hasManual = isFinite(manualLat) && isFinite(manualLon);
  var unit = temperatureUnit(settings);

  var nwsThenTide = function(lat, lon) {
    return function() {
      refreshNws(lat, lon, refreshTide);
    };
  };

  if (settings.WEATHER_SOURCE === 'manual' && hasManual) {
    fetchWeatherForCoordinates(manualLat, manualLon, unit,
                               nwsThenTide(manualLat, manualLon));
    return;
  }

  navigator.geolocation.getCurrentPosition(function(position) {
    var lat = position.coords.latitude;
    var lon = position.coords.longitude;
    fetchWeatherForCoordinates(lat, lon, unit, nwsThenTide(lat, lon));
  }, function(error) {
    console.log('Location failed: ' + JSON.stringify(error));
    if (hasManual) {
      fetchWeatherForCoordinates(manualLat, manualLon, unit,
                                 nwsThenTide(manualLat, manualLon));
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

function lastSundayOfMonth(year, monthIndex) {
  var lastDay = new Date(Date.UTC(year, monthIndex + 1, 0));
  return lastDay.getUTCDate() - lastDay.getUTCDay();
}

function isEuropeanDst(components) {
  // EU summer time: last Sunday of March 01:00 UTC -> last Sunday of October 01:00 UTC.
  var year = components.year;
  var month = components.month;
  if (month >= 4 && month <= 9) return true;
  if (month <= 2 || month >= 11) return false;
  var current = localComponentTime(components);
  if (month === 3) {
    var marStart = lastSundayOfMonth(year, 2);
    return current >= Date.UTC(year, 2, marStart, 1, 0, 0);
  }
  var octEnd = lastSundayOfMonth(year, 9);
  return current < Date.UTC(year, 9, octEnd, 1, 0, 0);
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
  if (tzid === 'America/New_York'
      || tzid === 'America/Detroit'
      || tzid === 'America/Toronto') {
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
  if (tzid === 'Europe/London') {
    return isEuropeanDst(components) ? 60 : 0;
  }
  if (tzid === 'Europe/Paris') {
    return isEuropeanDst(components) ? 120 : 60;
  }
  if (tzid === 'Asia/Dubai') {
    return 240;
  }
  if (tzid === 'Asia/Kolkata') {
    return 330;
  }
  if (tzid === 'Asia/Singapore' || tzid === 'Asia/Hong_Kong') {
    return 480;
  }
  if (tzid === 'Asia/Tokyo') {
    return 540;
  }
  if (tzid === 'Australia/Perth') {
    return 480;
  }
  return null;
}

// Returns the DST-correct short abbreviation for a supported zone, or '' if the
// zone is unknown. Mirrors timezoneOffsetMinutes so the label and offset always
// agree (e.g. London is GMT in winter, BST in summer; Sydney is AEST/AEDT).
function timezoneLabel(tzid, components) {
  tzid = normalizeTzid(tzid);
  if (tzid === 'UTC' || tzid === 'Etc/UTC' || tzid === 'GMT' || tzid === 'Z') return 'UTC';
  if (tzid === 'America/New_York' || tzid === 'America/Detroit' || tzid === 'America/Toronto')
    return isUsDst(components) ? 'EDT' : 'EST';
  if (tzid === 'America/Chicago') return isUsDst(components) ? 'CDT' : 'CST';
  if (tzid === 'America/Denver') return isUsDst(components) ? 'MDT' : 'MST';
  if (tzid === 'America/Phoenix') return 'MST';
  if (tzid === 'America/Los_Angeles') return isUsDst(components) ? 'PDT' : 'PST';
  if (tzid === 'Europe/London') return isEuropeanDst(components) ? 'BST' : 'GMT';
  if (tzid === 'Europe/Paris') return isEuropeanDst(components) ? 'CEST' : 'CET';
  if (tzid === 'Australia/Sydney') return isSydneyDst(components) ? 'AEDT' : 'AEST';
  if (tzid === 'Australia/Perth') return 'AWST';
  if (tzid === 'Asia/Tokyo') return 'JST';
  if (tzid === 'Asia/Singapore') return 'SGT';
  if (tzid === 'Asia/Hong_Kong') return 'HKT';
  if (tzid === 'Asia/Kolkata') return 'IST';
  if (tzid === 'Asia/Dubai') return 'GST';
  return '';
}

// Current UTC time in the {year, month, day, ...} component shape the DST
// helpers expect (month is 1-based, matching the timezoneOffsetMinutes callers).
function utcComponentsNow() {
  var d = new Date();
  return {
    year: d.getUTCFullYear(),
    month: d.getUTCMonth() + 1,
    day: d.getUTCDate(),
    hour: d.getUTCHours(),
    minute: d.getUTCMinutes(),
    second: d.getUTCSeconds()
  };
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

function rawEventTitle(event) {
  if (!event || !event.summary) {
    return '';
  }
  return String(event.summary).slice(0, 60);
}

function eventStartEpoch(event) {
  return event && event.start ? Math.floor(event.start.getTime() / 1000) : 0;
}

function eventEndEpoch(event) {
  return event ? Math.floor(eventEndTime(event) / 1000) : 0;
}

// Bottom meeting bar: send raw title + start/end epoch (seconds) so the watch
// can re-pick current-vs-next on its own minute tick instead of displaying a
// frozen, phone-formatted string between calendar refreshes. Missing slots are
// sent as ''/0 so the watch clears them when the day's event count shrinks.
function setMeetingSlot(dict, index, event) {
  dict[keys['MEETING_TITLE_' + index]] = rawEventTitle(event);
  dict[keys['MEETING_START_' + index]] = eventStartEpoch(event);
  dict[keys['MEETING_END_' + index]] = eventEndEpoch(event);
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
  setMeetingSlot(summaryDict, 1, displayEvents[0]);
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
  setMeetingSlot(middleDict, 2, displayEvents[1]);
  setMeetingSlot(middleDict, 3, displayEvents[2]);
  sendToWatch(middleDict, 'Calendar events 2-3');

  var endDict = {};
  endDict[keys.CALENDAR_EVENT_TITLE_4] = optionalEventTitle(displayEvents[3]);
  endDict[keys.CALENDAR_EVENT_DELTA_4] = optionalEventDelta(displayEvents[3]);
  endDict[keys.CALENDAR_EVENT_TITLE_5] = optionalEventTitle(displayEvents[4]);
  endDict[keys.CALENDAR_EVENT_DELTA_5] = optionalEventDelta(displayEvents[4]);
  setMeetingSlot(endDict, 4, displayEvents[3]);
  setMeetingSlot(endDict, 5, displayEvents[4]);
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
  if (multiTzTimer) {
    clearInterval(multiTzTimer);
  }

  weatherTimer = setInterval(refreshWeather, weatherMinutes * 60 * 1000);
  calendarTimer = setInterval(refreshCalendar, 10 * 60 * 1000);
  // Re-resolve secondary-timezone offsets periodically so a DST transition is
  // picked up within the interval without needing the config page reopened.
  multiTzTimer = setInterval(function() {
    sendMultiTzSetting(readSettings());
  }, 30 * 60 * 1000);
}

Pebble.addEventListener('ready', function() {
  var settings = readSettings();
  sendLayoutSetting(settings);
  sendColorSetting(settings);
  sendShakeSetting(settings);
  sendTideSetting(settings);
  sendPricesSetting(settings);
  sendMultiTzSetting(settings);
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
  sendPricesSetting(settings);
  sendMultiTzSetting(settings);
  refreshWeather(true);
  refreshCalendar();
  scheduleRefreshes();
});
