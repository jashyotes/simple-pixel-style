'use strict';

var Clay = require('@rebble/clay');
var clayConfig = require('./config.json');
var keys = require('message_keys');

var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var weatherTimer = null;
var calendarTimer = null;

var DEFAULT_SETTINGS = {
  TOP_STEPS: true,
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
  CALENDAR_LOOKAHEAD_HOURS: '48'
};

var COMPLICATION_IDS = {
  temperature: 0,
  rain: 1,
  heart_rate: 2,
  steps: 3,
  watch_battery: 4,
  phone_battery: 5,
  feels_like: 6,
  high_temp: 7,
  wind: 8,
  uv: 9,
  next_event: 10,
  weather: 11,
  weather_icon: 12
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

function temperatureUnit(settings) {
  return settings.TEMPERATURE_UNIT === 'celsius' ? 'celsius' : 'fahrenheit';
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

function sendToWatch(dict, label) {
  Pebble.sendAppMessage(dict, function() {
    console.log((label || 'Message') + ' sent');
  }, function(error) {
    console.log((label || 'Message') + ' failed: ' + JSON.stringify(error));
  });
}

function sendLayoutSetting(settings) {
  var dict = {};
  dict[keys.TOP_STEPS] = settings.TOP_STEPS ? 1 : 0;
  dict[keys.COMPLICATION_1] = complicationId(settings.COMPLICATION_1, COMPLICATION_IDS.weather);
  dict[keys.COMPLICATION_2] = complicationId(settings.COMPLICATION_2, COMPLICATION_IDS.rain);
  dict[keys.COMPLICATION_3] = complicationId(settings.COMPLICATION_3, COMPLICATION_IDS.heart_rate);
  dict[keys.TEMPERATURE_UNIT] = temperatureUnit(settings) === 'celsius' ? 1 : 0;
  sendToWatch(dict, 'Layout setting');
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

function firstDailyValue(daily, field) {
  if (!daily || !daily[field] || !daily[field].length) {
    return null;
  }
  var value = Number(daily[field][0]);
  return isFinite(value) ? value : null;
}

function addRounded(dict, key, value, min, max) {
  if (isNumber(value)) {
    dict[key] = clamp(Math.round(value), min, max);
  }
}

function fetchWeatherForCoordinates(lat, lon, unit) {
  var unitParam = unit === 'celsius' ? 'celsius' : 'fahrenheit';
  var url = 'https://api.open-meteo.com/v1/forecast'
      + '?latitude=' + encodeURIComponent(lat)
      + '&longitude=' + encodeURIComponent(lon)
      + '&current=temperature_2m,apparent_temperature,weather_code,wind_speed_10m'
      + '&hourly=precipitation_probability'
      + '&daily=temperature_2m_max,uv_index_max'
      + '&temperature_unit=' + encodeURIComponent(unitParam)
      + '&wind_speed_unit=mph'
      + '&forecast_days=1'
      + '&timezone=auto';

  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    try {
      var data = JSON.parse(xhr.responseText);
      if (!data.current) {
        return;
      }

      var dict = {};
      addRounded(dict, keys.TEMPERATURE, data.current.temperature_2m, -99, 127);
      addRounded(dict, keys.FEELS_LIKE, data.current.apparent_temperature, -99, 127);
      addRounded(dict, keys.WEATHER_CODE, data.current.weather_code, 0, 255);
      addRounded(dict, keys.WIND_SPEED, data.current.wind_speed_10m, 0, 255);
      addRounded(dict, keys.HIGH_TEMP, firstDailyValue(data.daily, 'temperature_2m_max'), -99, 127);
      addRounded(dict, keys.UV_INDEX, firstDailyValue(data.daily, 'uv_index_max'), 0, 255);
      dict[keys.RAIN_CHANCE] = nearestRainChance(data.hourly);
      sendToWatch(dict, 'Weather');
    } catch (e) {
      console.log('Weather parse failed: ' + e);
    }
  };
  xhr.onerror = function() {
    console.log('Weather request failed');
  };
  xhr.open('GET', url);
  xhr.send();
}

function refreshWeather() {
  var settings = readSettings();
  if (!settings.WEATHER_ENABLED) {
    return;
  }

  var manualLat = parseFloat(settings.WEATHER_LAT);
  var manualLon = parseFloat(settings.WEATHER_LON);
  var hasManual = isFinite(manualLat) && isFinite(manualLon);
  var unit = temperatureUnit(settings);

  if (settings.WEATHER_SOURCE === 'manual' && hasManual) {
    fetchWeatherForCoordinates(manualLat, manualLon, unit);
    return;
  }

  navigator.geolocation.getCurrentPosition(function(position) {
    fetchWeatherForCoordinates(position.coords.latitude, position.coords.longitude, unit);
  }, function(error) {
    console.log('Location failed: ' + JSON.stringify(error));
    if (hasManual) {
      fetchWeatherForCoordinates(manualLat, manualLon, unit);
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

function parseIcsDate(line) {
  var parts = line.split(':');
  var meta = parts.shift() || '';
  var value = parts.join(':');
  if (!value) {
    return null;
  }

  if (meta.indexOf('VALUE=DATE') !== -1 || /^\d{8}$/.test(value)) {
    return null;
  }

  var match = value.match(/^(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})(\d{2})(Z?)$/);
  if (!match) {
    return null;
  }

  if (match[7] === 'Z') {
    return new Date(Date.UTC(
      Number(match[1]), Number(match[2]) - 1, Number(match[3]),
      Number(match[4]), Number(match[5]), Number(match[6])
    ));
  }

  return new Date(
    Number(match[1]), Number(match[2]) - 1, Number(match[3]),
    Number(match[4]), Number(match[5]), Number(match[6])
  );
}

function unescapeIcsText(value) {
  return value
    .replace(/\\n/g, ' ')
    .replace(/\\,/g, ',')
    .replace(/\\;/g, ';')
    .replace(/\\\\/g, '\\')
    .trim();
}

function parseNextEvent(icsText, lookaheadHours) {
  var lines = unfoldIcs(icsText);
  var events = [];
  var current = null;

  lines.forEach(function(line) {
    if (line === 'BEGIN:VEVENT') {
      current = {};
    } else if (line === 'END:VEVENT' && current) {
      if (current.start && current.summary) {
        events.push(current);
      }
      current = null;
    } else if (current) {
      if (line.indexOf('DTSTART') === 0) {
        current.start = parseIcsDate(line);
      } else if (line.indexOf('DTEND') === 0) {
        current.end = parseIcsDate(line);
      } else if (line.indexOf('SUMMARY') === 0) {
        current.summary = unescapeIcsText(line.split(':').slice(1).join(':'));
      }
    }
  });

  var now = new Date();
  var maxTime = now.getTime() + (Number(lookaheadHours) || 48) * 60 * 60 * 1000;
  var upcoming = events.filter(function(event) {
    var endTime = event.end ? event.end.getTime() : event.start.getTime() + 60 * 60 * 1000;
    return event.start.getTime() <= maxTime && endTime >= now.getTime();
  }).sort(function(a, b) {
    return a.start.getTime() - b.start.getTime();
  });

  return upcoming[0] || null;
}

function formatHour(date) {
  var hours = date.getHours();
  var suffix = hours >= 12 ? 'PM' : 'AM';
  var hour = hours % 12;
  if (hour === 0) {
    hour = 12;
  }
  return hour + suffix;
}

function formatEvent(event) {
  if (!event) {
    return '[None]';
  }

  var now = new Date();
  var endTime = event.end ? event.end.getTime() : event.start.getTime() + 60 * 60 * 1000;
  var prefix = event.start.getTime() <= now.getTime() && endTime >= now.getTime()
      ? 'NOW'
      : formatHour(event.start);
  var text = prefix + ' | ' + event.summary;
  return text.length > 72 ? text.slice(0, 69) + '...' : text;
}

function formatEventDelta(event) {
  if (!event) {
    return '--';
  }

  var now = new Date().getTime();
  var start = event.start.getTime();
  var end = event.end ? event.end.getTime() : start + 60 * 60 * 1000;
  if (start <= now && end >= now) {
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

function refreshCalendar() {
  var settings = readSettings();
  var url = (settings.CALENDAR_ICS_URL || '').trim();
  if (!settings.CALENDAR_ENABLED || !url) {
    return;
  }

  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    var event = parseNextEvent(xhr.responseText, settings.CALENDAR_LOOKAHEAD_HOURS);
    var dict = {};
    dict[keys.NEXT_EVENT] = formatEvent(event);
    dict[keys.NEXT_EVENT_DELTA] = formatEventDelta(event);
    sendToWatch(dict, 'Calendar');
  };
  xhr.onerror = function() {
    console.log('Calendar request failed');
  };
  xhr.open('GET', url);
  xhr.send();
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
  refreshWeather();
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
  refreshWeather();
  refreshCalendar();
  scheduleRefreshes();
});
