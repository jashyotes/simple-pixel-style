# Pebble Appstore listing — Simple Pixel Style

Last updated: 1.53 (overlay list and counts; the other sections still date from 0.67)

## Short tagline (one line, for the Appstore card)

Pebble Time 2 watchface with a pixel-art layout, three configurable complications, eleven shake-to-reveal overlays, and a full Light/Dark or per-section Color theme.

## Long description (Appstore body)

Pebble Time 2 (emery) watchface with a pixel-art base layout and a deep settings menu.

- Pixel-watch-style black-and-white layout with a single Light/Dark toggle that drives the entire face and every shake overlay.
- Optional full Color mode with per-section background and foreground color pickers for the top bar, date bar, time, weather row, and meeting bar.
- Black & White inversion toggles per section (top bar, date bar, time, weather, meeting bar) for high-contrast variants without leaving the BW theme.
- Conditional settings menu — the configuration form hides the controls that don't apply to your current mode, so BW mode doesn't show color pickers and Color mode doesn't show inversion toggles.
- Watch battery, Bluetooth status, date, AM/PM (or military time), large time, and a meeting/event row down the layout.
- Optional top Casio-inspired W800-style step counter above the date, with thousands comma.
- Three configurable complication circles. Each circle picks from:
  - Weather: current conditions, temperature, feels-like, high, high/low, rain chance, wind, UV index.
  - Fitness: steps, heart rate, active minutes, active calories, sleep last night, distance today.
  - Device: watch battery.
  - Calendar: next event countdown.
  - Rich visuals: fitness rings, weather circle, battery ring.
- Verbose weather option that replaces the three circles with a centered weather line, in one-line or large size.
- Open-Meteo weather via PebbleKit JS — phone GPS or manual coordinates, Fahrenheit or Celsius, refresh every 15/30/60 minutes.
- Multiple calendar support — two iCal/ICS feed URLs, configurable 24h / 48h / 7d lookahead, recurring events (including ordinal weekday rules like "second Tuesday"), and timezone-aware time labels.
- Quiet Time mouse indicator on the top bar, opposite the AM/PM label.
- Shake the wrist to reveal one of eleven overlays (or leave it off):
  - Fitness rings: steps, active minutes, active calories with custom targets and ring colors.
  - Next calendar events: three or five upcoming events with their countdowns.
  - Next calendar events - Large month & day: the same list with each event's month and day drawn large for reading at a glance.
  - Your Day: visual timeline of busy hours across yesterday/today/tomorrow with optional half-hour granularity (events at X:00 to X:29 fill the left half of the hour pip, events at X:30 to X:59 fill the right half, full hours stay solid).
  - Detailed weather.
  - Alt timezone: custom label and UTC offset (in minutes).
  - Big heart rate.
  - Stocks & Crypto Prices: up to two Yahoo Finance tickers and one CoinGecko coin with their deltas; the layout adapts to one, two or three tickers.
  - Tide chart: NOAA Tides & Currents station curve over a 24h or 48h window.
  - Step history: the last seven days of steps, ending with today.
  - NWS Forecast: 24h temperature and precipitation chart with the current period narrative, or a narrative-only style.
- Customizable shake overlay duration (3 / 5 / 7 / 10 seconds).
- Customizable "no-events" label for empty calendar states on the meeting bar, the Your Day Next: line, and the Upcoming overlay.

Android companion support is deferred and is not required for this Pebble Appstore PBW. The watchface uses PebbleKit JS for weather and calendar.

## GitHub repo "About" description (one line)

Pebble Time 2 (emery) watchface — pixel-art layout, three configurable complications, eleven shake-to-reveal overlays (fitness, calendar, calendar with large dates, timeline, weather, alt timezone, heart rate, stocks & crypto, tides, step history, NWS forecast), Open-Meteo weather and ICS calendar via PebbleKit JS, Light/Dark or per-section Color theme.

## GitHub repo topics (suggested)

`pebble` `pebble-watchface` `pebble-time-2` `emery` `pebblekit-js` `watchface` `clay-config` `ical` `open-meteo`
