# JY Time

Pebble Time 2 watchface modeled on Business Time, with a calendar event line replacing the AM/PM bar and phone battery % replacing the bluetooth checkbox.

## Project layout

```
Project - Pebble Watchface/
├── jy-time/                  ← watchface (C, Pebble SDK)
├── jy-time-companion/        ← Android app that feeds calendar+battery+weather to the watch
├── reference-screenshots/    ← Business Time + Pebble app config (visual references)
├── PROJECT_STATUS.md         ← phase tracking
└── README.md                 ← this file
```

## What's built

- ✅ **Watchface** — installs on emery emulator, renders the full layout (top widgets, divider, weekday, date, big time, event line, divider, ST/BPM stats), wires real watch data (time, watch battery, BT, steps, BPM), accepts AppMessage data from the companion (next event, phone battery, weather code, temp F).
- ✅ **Companion app** — Kotlin/Compose, foreground service, calendar provider query, battery broadcast receiver, Open-Meteo weather fetcher, PebbleKit Android 2 sender. Code complete, not yet built/tested on the phone.
- ⏳ **Companion verification on Pixel 10 XL** — needs Android Studio.
- ⏳ **App icon and store screenshots** — placeholders only.
- ⏳ **Pebble Appstore submission** — not yet submitted.

## Current screenshot (watchface alone, no companion)

`screenshot-phaseC-with-appmessage.png` — real watch data (date, time, battery, steps), placeholders for companion-fed data ("PH ...", "WX --", "No event").

---

## Building the watchface

The Pebble SDK is already installed at `~/.pebble-sdk/`. The `pebble` CLI is at `~/.local/bin/pebble`.

```bash
export PATH="$HOME/.local/bin:$PATH"
cd "Project - Pebble Watchface/jy-time"
pebble build                                    # produces build/jy-time.pbw
pebble install --emulator emery                 # install on emulator
pebble screenshot --emulator emery shot.png     # capture
```

To install on the physical Pebble Time 2:

1. Open the Pebble mobile app on the Pixel 10 XL.
2. Transfer `build/jy-time.pbw` to the phone (USB, Drive, AirDroid, etc.).
3. Open the PBW from the phone's file browser — Pebble app catches the intent and sideloads.

## Building the companion app

Requires Android Studio (Hedgehog or newer) and a JDK 17+.

```bash
cd "Project - Pebble Watchface/jy-time-companion"
# Open in Android Studio:
#   File → Open → select the jy-time-companion directory
# Or from command line if gradle is installed:
./gradlew assembleDebug          # debug APK at app/build/outputs/apk/debug/app-debug.apk
./gradlew installDebug           # install on connected USB phone
```

The first build will pull `io.rebble.pebblekit2:client:1.1.0` from JitPack — needs internet.

### Companion app permissions

On first launch, grant:
- **Calendar** — required, so the app can read your next upcoming event
- **Coarse location** — required for weather lookup via Open-Meteo
- **Notifications** — required so the foreground service can show its sync notification

### How the companion works

A foreground service (`WatchSyncService`) wakes every 60 seconds. Each cycle:
1. `CalendarReader` queries Android's Calendar Provider for the next non-all-day event in the next 24h.
2. `BatteryReader` reads the phone battery % from the sticky `ACTION_BATTERY_CHANGED` intent.
3. `WeatherFetcher` fetches Open-Meteo's current weather (cached for 30 min).
4. All four values are bundled into one `PebbleDictionary` and sent via `DefaultPebbleSender.sendDataToPebble(WATCHFACE_UUID, data)`.

The watchface receives them in `inbox_received_handler` and persists each value across launches.

### Companion app architecture map

| File | Purpose |
|---|---|
| `Constants.kt` | UUID, message keys, intervals |
| `MainActivity.kt` | Compose UI, permissions, service toggle, status display |
| `WatchSyncService.kt` | Foreground service, 60s loop, PebbleSender |
| `CalendarReader.kt` | CalendarContract.Instances query |
| `BatteryReader.kt` | Phone battery % from ACTION_BATTERY_CHANGED |
| `WeatherFetcher.kt` | FusedLocation + OkHttp + Open-Meteo |
| `BootReceiver.kt` | Auto-start service on boot if user previously enabled |

## Submitting to the Pebble Appstore

When ready:
1. Sign up at https://developer.repebble.com (one-time).
2. From `jy-time/`, run `pebble publish`. The CLI auto-generates per-platform screenshots and uploads.
3. Description should mention: Android-only (companion required), link to companion APK release.
4. Publish companion APK at https://github.com/jashyotes/jy-time-companion-android/releases (private repo by default per CLAUDE.md). Link from the watchface description.

## Not done in v1 (deferred to v2 per plan)

- Shake-action handler (`accel_tap_service_subscribe`)
- Configurable widgets (Clay)
- Light theme / theme picker
- iOS companion
- Custom WMO weather icons (current code uses 4-letter abbreviations: CLR, CLD, FOG, RAIN, SNOW, SHWR, STRM)
- Custom font (currently uses Pebble system fonts)
- Bluetooth-disconnected phone-icon variant (currently shows "PH X" text)

## References

- Plan: `~/.claude/plans/okay-enter-plan-mode-refactored-wand.md`
- Reference repos in `/tmp/`: `pebble-mesh`, `glanceface`, `glancefacecompanion`, `clean_and_smart`, `pebblekit2`
- Pebble SDK docs: https://developer.repebble.com/sdk
- PebbleKit Android 2: https://github.com/pebble-dev/PebbleKitAndroid2
