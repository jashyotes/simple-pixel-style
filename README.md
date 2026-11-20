# Simple Pixel Style

Pebble watchface for the Pebble Time 2 (emery) and Pebble Round 2 (gabbro) with a pixel-watch layout: watch battery, Bluetooth state, full date, large time with AM/PM, a top W800-style step counter, a FuelBand-style fitness pip row, three configurable complication circles, an optional bottom calendar event, and eleven shake-to-reveal overlays.

Listed on the Pebble Appstore as [Pixel Style w. Calendar](https://apps.repebble.com/95e29b8577814a36a22799ef).

![Simple Pixel Style on the Pebble Time 2](Appstore%20Kit%201.50/emery-watchface-loop.gif)
![Simple Pixel Style on the Pebble Round 2](Appstore%20Kit%201.50/gabbro-watchface-loop-v2.gif)

## Current Release

Version `1.57` ships with:

- Light or dark theme, in black and white with an inversion toggle for each section (top bar, pip bar, date bar, time, weather, meeting bar), or in color with a background and text color picker for each section.
- Five time fonts: Default, Default (Large), CASIO style with an optional shadow effect, Roboto, and LECO. 12-hour or military time, with an option to remove the leading zero.
- Date in English, Japanese (two formats), or French.
- Up to two extra timezones under the clock, shown as a text line or as circle complications.
- Top walking-man W800 step counter with a Casio-style thousands comma, and an option to combine the day's steps with a second watch running the face.
- FuelBand-style fitness pip row: circle or rectangle pips, two sizes, three fill directions, and theme-matched or custom colors.
- Three configurable complication circles. Each circle picks from:
  - Rich visuals: fitness rings, weather circle, battery ring.
  - Weather: weather, weather icon, temperature, feels like, high temp, high/low temp, rain chance, wind, UV index.
  - Fitness: steps, heart rate, active minutes, active calories, sleep last night, distance today.
  - Device: watch battery.
  - Calendar: next event.
  - Markets: two stock deltas and a Bitcoin delta.
- Verbose weather option that replaces the circles with a centered weather line, in one-line or large size.
- Weather through PebbleKit JS from Open-Meteo (global, the default) or the US National Weather Service, by phone location or manual coordinates, in Fahrenheit or Celsius, refreshed every 15, 30, or 60 minutes.
- Optional bottom calendar event from one or two iCalendar/ICS feed URLs, sorted so the soonest event wins, with a 24-hour, 48-hour, or 7-day look ahead, recurring events (including ordinal weekday rules such as second Tuesday), timezone handling, optional event dates, a custom no-events label, and a vibration when a meeting starts.
- Shake the wrist for one of eleven overlays, shown for 3, 5, 7, or 10 seconds:
  - Fitness rings: steps, active time, and active calories, with custom targets and ring colors.
  - Next calendar events: three or five upcoming events with their countdowns.
  - Next calendar events - Large month & day: the same list with each event's date drawn large.
  - Your Day: a timeline of busy hours, with optional half-hour pips.
  - Detailed weather.
  - Alt timezone: custom label and UTC offset.
  - Heart rate big.
  - Stocks & Crypto Prices: up to two stock tickers and one coin with their deltas.
  - Tide chart: a NOAA Tides & Currents station curve over 24 or 48 hours.
  - Step history: the last seven days of steps.
  - NWS Forecast: a temperature and precipitation chart with the forecast narrative, or a narrative-only style.
- Optional vibration when the daily step goal is reached, with nine patterns to pick from.
- Vibrate on disconnect, a larger battery number option, and metric or imperial distance.
- Quiet Time indicator using the mouse icon, aligned opposite the AM/PM label.

Current release artifacts:

- The 1.57 build installs from the Pebble Appstore listing linked above.
- `Appstore Kit 1.50/` holds the store screenshots and both loop GIFs.
- `release-assets/APPSTORE_DESCRIPTION.md` holds the store listing text.
- `PROJECT_STATUS.md` holds the release log.

## Project Layout

```text
simple-pixel-style/
|-- jy-time/                  Pebble watchface source
|-- jy-time-companion/        Optional Android companion source
|-- Appstore Kit 1.50/        Store screenshots and loop GIFs
|-- Current Screenshots/      Dark and light screenshots
|-- release-assets/           Icons, older PBWs and screenshots, release notes, store listing text
|-- reference-screenshots/    Visual references
|-- PROJECT_STATUS.md         Working notes and release log
`-- README.md
```

## Build The Watchface

Building needs the Pebble SDK and its `pebble` command line tool.

```sh
cd jy-time
npm install
pebble build
pebble install --emulator emery
pebble screenshot --emulator emery
```

Use `gabbro` in place of `emery` for the Pebble Round 2. The built PBW is `jy-time/build/jy-time.pbw`.

The phone-side weather and step-sync code has a fixture-driven harness:

```sh
cd jy-time
node test/pkjs_weather_harness.js
```

## Companion

The Pebble Appstore PBW does not require the native Android companion. The watchface already uses PebbleKit JS for weather and optional ICS calendar data. The Android companion remains in `jy-time-companion/` as an optional native sync path.
