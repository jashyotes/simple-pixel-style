'use strict';

var Clay = require('@rebble/clay');
var clayConfig = require('./config.json');
var keys = require('message_keys');

var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var weatherTimer = null;
var calendarTimer = null;

var DEFAULT_SETTINGS = {
  TOP_STEPS: false,
  WEATHER_ENABLED: true,
  WEATHER_SOURCE: 'gps',
  WEATHER_LAT: '',
  WEATHER_LON: '',
  WEATHER_REFRESH_MIN: '30',
  CALENDAR_ENABLED: false,
  CALENDAR_ICS_URL: '',
  CALENDAR_LOOKAHEAD_HOURS: '48'
};

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
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

function fetchWeatherForCoordinates(lat, lon) {
  var url = 'https://api.open-meteo.com/v1/forecast'
      + '?latitude=' + encodeURIComponent(lat)
      + '&longitude=' + encodeURIComponent(lon)
      + '&current=temperature_2m,weather_code'
      + '&hourly=precipitation_probability'
      + '&temperature_unit=fahrenheit'
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
      dict[keys.TEMPERATURE] = clamp(Math.round(data.current.temperature_2m), -99, 127);
      dict[keys.WEATHER_CODE] = clamp(Math.round(data.current.weather_code), 0, 255);
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

  if (settings.WEATHER_SOURCE === 'manual' && hasManual) {
    fetchWeatherForCoordinates(manualLat, manualLon);
    return;
  }

  navigator.geolocation.getCurrentPosition(function(position) {
    fetchWeatherForCoordinates(position.coords.latitude, position.coords.longitude);
  }, function(error) {
    console.log('Location failed: ' + JSON.stringify(error));
    if (hasManual) {
      fetchWeatherForCoordinates(manualLat, manualLon);
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
    return 'No event';
  }

  var now = new Date();
  var endTime = event.end ? event.end.getTime() : event.start.getTime() + 60 * 60 * 1000;
  var prefix = event.start.getTime() <= now.getTime() && endTime >= now.getTime()
      ? 'NOW'
      : formatHour(event.start);
  var text = prefix + ' | ' + event.summary;
  return text.length > 72 ? text.slice(0, 69) + '...' : text;
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
