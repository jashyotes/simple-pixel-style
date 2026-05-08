# Simple Pixel Style Watchface

Pebble SDK watchface source for Simple Pixel Style.

## Build

```sh
/home/jates/.local/bin/pebble build
/home/jates/.local/bin/pebble install --emulator emery
```

## Main Files

- `src/c/jy-time.c`: watch rendering, health/battery/Bluetooth data, AppMessage handling, persistent settings.
- `src/pkjs/index.js`: PebbleKit JS weather, calendar, and Clay settings bridge.
- `src/pkjs/config.json`: Pebble app settings UI.
- `package.json`: app metadata, message keys, resources, and target platform.

The current target platform is `emery` for Pebble Time 2.
