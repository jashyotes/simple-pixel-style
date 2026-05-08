# Simple Pixel Style

Pebble Time 2 watchface with a pixel-watch layout: watch battery, Bluetooth state, full date, Bitham time with AM/PM, a top W800-style step counter, three configurable complication circles, and an optional bottom calendar event.

## Current Release

Version `0.2.0` ships with:

- Three evenly spaced complication circles only.
- Top walking-man step counter enabled by default.
- Configurable left, middle, and right complication slots.
- Built-in choices for temperature, rain chance, heart rate, steps, watch battery, phone battery, feels-like temperature, high temperature, wind speed, UV index, and next-event countdown.
- Open-Meteo weather through PebbleKit JS.
- Optional iCal/ICS calendar feed through PebbleKit JS.

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
