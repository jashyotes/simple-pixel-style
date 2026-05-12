# Simple Pixel Style

Pebble Time 2 watchface with a pixel-watch layout: watch battery, Bluetooth state, full date, Bitham time with AM/PM, a top W800-style step counter, three configurable complication circles, and an optional bottom calendar event.

## Current Release

Version `0.41` ships with:

- Color customization toggles for independently inverting the top bar, date bar, time, weather area, and meeting bar.
- Cleaner date and time inversion bars, with contiguous edges when both are inverted.
- Tighter weather-area inversion around the three complication circles.
- Optional light mode with a white background and black text/icons.
- Three configurable complication circles by default, with weather, rain chance, and heart rate selected out of the box.
- Top walking-man W800 step counter enabled by default, including a Casio-style thousands comma.
- Optional small or large verbose weather mode that replaces the circles with a centered weather line.
- Built-in choices for weather, weather icon-only, temperature, rain chance, heart rate, steps, watch battery, phone battery, feels-like temperature, high temperature, wind speed, UV index, and next-event countdown.
- Fahrenheit and Celsius selection for temperature-based complications.
- Open-Meteo weather through PebbleKit JS.
- Optional bottom calendar event from one or two iCalendar/ICS feed URLs, sorted so the soonest event wins.
- Google Calendar recurring event expansion, timezone handling, full event time labels, and 15-minute current-meeting rollover.
- Monthly recurring events that use ordinal weekdays, such as second Tuesday or fourth Tuesday.
- Quiet Time indicator using the mouse icon, aligned opposite the AM/PM label.
- Current screenshots organized into the `Current Screenshots/` folder, including dark and light variants.

Current release artifacts:

- `release-assets/simple-pixel-style-0.41-emery.pbw`
- `Current Screenshots/current-dark-default-3-circles-quiet-time.png`
- `Current Screenshots/current-light-default-3-circles.png`
- `Current Screenshots/current-light-inverted-bars-default-3-circles.png`
- `Current Screenshots/current-dark-weather-meeting-inverted.png`
- `Current Screenshots/current-light-weather-meeting-inverted.png`
- `Current Screenshots/current-light-small-verbose-weather.png`
- `Current Screenshots/current-light-large-verbose-weather.png`
- `release-assets/RELEASE_NOTES_0.41.md`

## Project Layout

```text
Project - Pebble Watchface/
|-- jy-time/                  Pebble watchface source
|-- jy-time-companion/        Optional Android companion source
|-- release-assets/           PBWs, screenshots, icons, release notes
|-- reference-screenshots/    Visual references
|-- PROJECT_STATUS.md         Working notes
`-- README.md
```

## Build The Watchface

The Pebble CLI is installed at `~/.local/bin/pebble`.

```sh
cd "Project - Pebble Watchface/jy-time"
/home/jates/.local/bin/pebble build
/home/jates/.local/bin/pebble install --emulator emery
/home/jates/.local/bin/pebble screenshot --emulator emery
```

The built PBW is `jy-time/build/jy-time.pbw`. Release copies live in `release-assets/`.

## Companion

The Pebble Appstore PBW does not require the native Android companion. The watchface already uses PebbleKit JS for weather and optional ICS calendar data. The Android companion remains in `jy-time-companion/` as an optional native sync path.
