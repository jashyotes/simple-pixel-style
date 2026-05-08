# Simple Pixel Style — Pebble Time 2 Watchface Project Status

## ▶ Pixel Watch Style 1.0 saved — 2026-05-08

Approved baseline snapshot:
- `snapshots/pixel-watch-style-1.0-2026-05-08/jy-time.c`
- `snapshots/pixel-watch-style-1.0-2026-05-08/package.json`
- `snapshots/pixel-watch-style-1.0-2026-05-08/STEP_BOOT.PNG`
- `snapshots/pixel-watch-style-1.0-2026-05-08/jy-time.pbw`
- `snapshots/pixel-watch-style-1.0-2026-05-08/screenshot.png`

Pixel Watch Style 1.0 visual state:
- Pixel-watch-style black face with top-left battery number plus measured watch icon placement.
- Top-right Bluetooth icon.
- Full date centered.
- Centered time, with small pixel AM/PM placed to the left from measured time bounds.
- Four circular complications: steps, weather/temp, rain chance, BPM.
- Bottom divider and next-event line.

## ▶ Pixel Watch Style 1.1 saved — 2026-05-08

Saved snapshot:
- `snapshots/pixel-watch-style-1.1-2026-05-08/jy-time.c`
- `snapshots/pixel-watch-style-1.1-2026-05-08/package.json`
- `snapshots/pixel-watch-style-1.1-2026-05-08/STEP_BOOT.PNG`
- `snapshots/pixel-watch-style-1.1-2026-05-08/W800_STEPS_SPRITE.PNG`
- `snapshots/pixel-watch-style-1.1-2026-05-08/W800_WALKING_MAN.PNG`
- `snapshots/pixel-watch-style-1.1-2026-05-08/jy-time.pbw`
- `snapshots/pixel-watch-style-1.1-2026-05-08/screenshot.png`

Latest screenshot:
- `screenshot-pixel-watch-style-1.1-final-preview-2026-05-08.png`

Status:
- Top W800 step bar is hard-enabled for placement review.
- W800 walking-man and step-number assets are 1-bit alpha and drawn at native size, so they should not blur.
- Lower row uses three evenly spaced centered complications while the top step bar mode is enabled.

## ▶ Pixel Watch Style 1.1 functional toggle implemented — 2026-05-08

Status:
- Pixel Watch Style 1.0 is now the default/off state again.
- A new persisted top-step-counter setting controls Pixel Watch Style 1.1 behavior.
- When the setting is off, the face renders the approved 1.0 structure with four lower circular complications.
- When the setting is on, the W800-style step counter moves above the date between the battery/watch cluster and Bluetooth icon, the lower steps circle is removed, and the remaining three lower circular complications are centered evenly.

Implementation:
- Watch AppMessage key: `TOP_STEPS`, numeric key `10005`.
- Watch persistence key: `PERSIST_KEY_TOP_STEPS`.
- Android companion preference: `PREF_TOP_STEPS`.
- Android settings UI now includes `Top W800 step counter`.
- Android companion includes `TOP_STEPS` in every sync payload and can trigger an immediate sync when the toggle changes.
- Pebble-app settings now include `Top W800 step counter` through Clay, so the 1.1 layout can be toggled without a separate native Android app.
- The top W800 step number is dynamic now. It renders the real HealthService step count using native 12x24 digit sprites generated from the W800 `FONT_SECOND_25.TTF` reference, right-aligned and without runtime scaling.
- The top step count is capped visually at five digits (`99999`) to avoid overflowing the reserved top bar.

Build/test status:
- `pebble build` succeeds for `jy-time`.
- PBW includes `pebble-js-app.js`, `enableMultiJS: true`, and `capabilities: ["configurable", "location"]` for Pebble-app settings and weather location.
- Android companion debug build succeeds with local `/tmp/jdk17` and `/tmp/android-sdk`.
- Emulator default/off screenshot: `screenshot-pixel-watch-style-toggle-default-off-2026-05-08.png`.
- Emulator on-state screenshot: `screenshot-pixel-watch-style-toggle-on-dynamic-2026-05-08.png`.
- The emulator accepted a direct AppMessage `TOP_STEPS=1` test and rendered the on-state. Emulator health data showed `0` steps, so the top count screenshot shows `0`.

Pebble Appstore packaging note:
- Josh approved the default/off and top-step/on screenshots.
- The native Android companion is not required for the Pebble Appstore PBW and is deferred as an optional phone-data sender.
- `jy-time/package.json` no longer advertises a native Android `companionApp`, so the Pebble install should not prompt for a separate APK.
- The PBW is configurable in the Pebble app through Clay and currently exposes layout, weather, and calendar feed settings.
- Current release PBW: `release-assets/simple-pixel-style-0.1.0-emery.pbw`.
- Current store screenshots: `release-assets/screenshot-emery-default.png` and `release-assets/screenshot-emery-top-steps.png`.

Orphans / cleanup candidates:
- `jy-time/resources/images/W800_STEPS_SPRITE.PNG` is now an unused static preview asset. It is no longer packaged or used by the watchface. Delete only after Josh approves cleanup.

## ▶ In-progress layout scale pass — 2026-05-08

Current requested order:
1. Move the event divider line down toward the bottom event text and confirm.
2. After confirmation, move the circles down until they hover 1-2 black pixels above that line.
3. After that alignment is approved, make the time larger while keeping the AM/PM pixel label small.
4. Then enlarge circles, icons, and circle values.

Current source change:
- Only the event divider line moved, from `y=187` to `y=200`.
- Circles, time size, event text, and icon centering have not been changed yet.
- Static previews generated from the approved screenshots because the emulator install timed out:
  - `screenshot-simple-pixel-style-event-line-y200-preview-2026-05-08.png`
  - `screenshot-simple-pixel-style-event-line-y200-top-steps-preview-2026-05-08.png`
- `pebble build` succeeds after the divider-line change.
- Release PBW has not been refreshed with this layout change yet; wait for Josh's approval before packaging.

Deferred note:
- Weather icon centering is visibly off, but Josh wants to revisit that after circle resizing.

## ▶ Paused 2026-05-07 — Pixel-style face only, no more autonomous icon design

Josh paused work after a bad autonomous step-icon decision. The attempted two-footprint / gumshoe-style step icon was not approved and must not be used.

Critical collaboration correction:
- Do not invent replacement iconography or visual language when Josh asks for an icon issue to be fixed.
- Do not substitute a "simple" or "readable" icon for the requested icon direction without showing it first.
- For future icon work, use only a user-provided reference, an already-approved mockup element, or an explicit Josh-approved option.
- The next session should start by confirming the step icon direction before editing the icon again.
- Do not treat "the current icon is bad" as permission to pick a new metaphor or style. It means ask/reference/implement precisely.

What was happening immediately before pause:
- We were finalizing the Pixel-style watchface, not W800/Casio.
- The active app is still `jy-time/` and the target visual reference is still `design-mockups/watchface-options.html`, specifically Option 1 / Pixel Style.
- The watchface was updated to use the Pixel-style information hierarchy: top-left watch battery + watch icon, Bluetooth status top-right, full date, AM/PM + time, four circular complications, divider, and bottom event row.
- The Android companion was updated to send formatted calendar events, phone battery, weather, temperature, and rain chance.
- The Pebble app and Android companion both built successfully before the final icon dispute.

Specific latest source state:
- `jy-time/src/c/jy-time.c` has been changed back from the rejected footprint icon to the prior line-drawn shoe/step icon implementation.
- The rejected footprint icon is no longer present in source.
- The AM/PM indicator is currently placed lower so it aligns more closely with the bottom of the time numerals.
- Time rendering is forced to 12-hour format with a leading zero, regardless of the watch/emulator 24-hour setting.
- The top-left watch icon was adjusted toward the selected "C" watch icon geometry from `design-mockups/watchface-options.html`, but Josh called out that the C icon still needs scrutiny.

Important artifact warning:
- I paused before rebuilding after undoing the rejected footprint icon.
- Therefore `jy-time/src/c/jy-time.c` is the source of truth, but `jy-time/build/jy-time.pbw`, `screenshot-samsung-shipping-pass.png`, and `release-assets/` may be stale and may still reflect the rejected footprint build or earlier alignment passes.
- Do not ship or judge the visual from those artifacts until the next session rebuilds and recaptures after the icon direction is approved.

Next-session restart checklist:
1. Do not touch W800/Casio.
2. Open `design-mockups/watchface-options.html` and the latest emulator screenshot only for Pixel-style structure context.
3. Decide the step icon from an explicit reference/option before code changes.
4. Fix only the step icon and any explicitly approved AM/PM/watch-icon alignment issue.
5. Rebuild `jy-time`, reinstall on emery emulator, capture screenshot, then update `release-assets/` only after Josh approves the screenshot.
6. Rebuild/copy Android companion only if companion code changes; otherwise leave it alone.

## ▶ Current Direction — Pixel-style app, W800H Casio replacement

Important correction: the Pixel-style watchface structure remains the active `jy-time` app direction. W800H/Casio work is only allowed in two places: the separate W800H clone exploration, and the now-approved optional top-step-counter toggle inside the Pixel-style face.

Current rules for this direction:
- Keep the active watchface app aligned with `design-mockups/watchface-options.html`.
- Do not replace or overwrite the Pixel-style `jy-time/src/c/jy-time.c` layout unless Josh explicitly asks for that.
- Treat JohnEdwa/W800 as the source of truth only for the separate W800H/Casio replacement exploration and the approved optional top W800 step counter.
- Preserve the W800H LCD/watchface feel in mockups first; only promote it into app code after explicit approval.
- Do not design or swap icons without an explicit approved reference.

Current Pixel-style app files:
- `design-mockups/watchface-options.html`
- `design-mockups/watchface-options.png`
- `jy-time/src/c/jy-time.c` is the active Pixel-style app layout.
- `jy-time/package.json` packages only the W800 resources required for the optional top-step-counter toggle: `W800_WALKING_MAN` plus ten W800 digit sprites.

Current W800H clone exploration artifacts:
- `design-mockups/w800h-emery-clone.png`
- `design-mockups/w800h-emery-clone-3x.png`
- `design-mockups/render-w800h-emery-clone.sh`
- W800 references remain under `design-mockups/vendor/` and W800/Casio mockup files. The only W800 resources intentionally promoted into `jy-time/resources/` are the approved top-step-counter assets.

Build status:
- `pebble build` succeeds for emery after the Pixel Watch Style 1.1 functional toggle pass.
- Build artifact: `jy-time/build/jy-time.pbw`.
- Current PBW release copy: `release-assets/simple-pixel-style-0.1.0-emery.pbw`.
- Current emulator screenshots: `screenshot-pixel-watch-style-toggle-default-off-2026-05-08.png` and `screenshot-pixel-watch-style-toggle-on-dynamic-2026-05-08.png`.
- Current resource size: `5537 bytes / 256.0KB`; RAM footprint: `5962 bytes / 128.0KB`.
- Android companion debug build succeeds using local JDK/SDK under `/tmp`.
- Native Android companion APK is deferred at `release-assets/deferred/jy-time-companion-0.1.0.apk` and is not part of the current Pebble Appstore watchface release.

Latest shipping-pass changes:
- Active watchface now matches the Pixel-style structure more closely: top-left watch battery with watch icon, small Bluetooth status at top-right, full date, AM/PM plus time, four circular complications, divider, and bottom event row.
- Time now always renders in 12-hour format with a leading zero and AM/PM indicator before the time, independent of the emulator/watch 24-hour setting.
- Complication icons were repositioned to center visually within their circles.
- The top-left watch icon was corrected toward the selected C icon geometry from `design-mockups/watchface-options.html`.
- Added `RAIN_CHANCE` AppMessage key.
- Watchface persists and displays rain chance alongside temperature/weather, steps, BPM, and next calendar event.
- Added PebbleKit JS weather sync using Open-Meteo. Settings support current phone location, manual latitude/longitude fallback, and 15/30/60 minute refresh.
- Added optional bottom calendar event sync from a private iCal/ICS feed URL. PebbleKit JS cannot read the phone calendar directly, so this avoids requiring the native Android companion for calendar text.
- Android companion formats calendar events as `3PM | Event Title` or `NOW | Event Title` for active events.
- Android companion now sends rain chance from Open-Meteo hourly precipitation probability.
- Android companion writes last sync status to shared preferences so the status screen can refresh visible sent values.
- Added an original `JY` menu icon at `jy-time/resources/images/RES_ICON.PNG`.
- Added optional W800 top-step-counter assets: `W800_WALKING_MAN` and digit sprites `W800_STEP_DIGIT_0` through `W800_STEP_DIGIT_9`.
- Added Clay-based Pebble-app settings for the top W800 step-counter toggle.
- Pixel-style baseline snapshot saved at `snapshots/samsung-baseline-2026-05-07/`.

## ▶ Resumed — new visual direction selected for exploration

The functional skeleton is built (see "Components built" below), but the Business Time visual direction is abandoned. The main miss was typography: the day-of-week/date relationship does not match the original Business Time reference, and the overall look is not where Josh wants it.

New visual references:
- `reference-screenshots/Old Samsung Watch Face.jpg` — preferred information hierarchy: battery/widgets top, day/date, AM/PM + time, lower complications, next event line.
- `reference-screenshots/Casio style.png` — preferred Casio/quartz styling cues: segmented time numerals, dense boxes, red/white/gold/blue accents, step-number presentation.
- `reference-screenshots/Mockup for Casio.png` — current Casio target layout annotated by Josh.

Current mockups:
- `design-mockups/watchface-options.html`
- `design-mockups/watchface-options.png`
- `design-mockups/casio-target.html`
- `design-mockups/casio-target.png`
- `design-mockups/casio-frame-base.png`
- `design-mockups/casio-w800-lcd-pass.png`
- `design-mockups/casio-w800-lcd-pass-3x.png`
- `design-mockups/snapshots/casio-w800-lcd-pass-step5-saved.png`
- `design-mockups/snapshots/casio-w800-lcd-pass-step5-saved-3x.png`
- Latest mockup pass incorporates Josh's feedback:
  - Option 1: Pixel-style structure is the active approved direction for now. Top-left battery displays `100` plus watch icon C, top-right is reserved for a future complication with a small Bluetooth indicator, date/time divider rules are removed, date/time is enlarged, AM/PM is aligned low, complications are circular icon/value controls with no labels, and the next meeting is a plain text line below the complication rule.
  - Known Option 1 concern: the running shoe/step icon still feels awkward and should be simplified again during implementation.
  - Option 2: old hand-rolled Casio attempt is reset. Target is Josh's annotated `Mockup for Casio.png`: Royale/Quartz frame language, `QUARTZ` replaced by a high-quality Pebble logo, `TIME 2` retained, top/bottom angular rails changed from blue to red, Quartz-style featured steps module above the main time, stylized AM/PM beside the time, three circular lower complications, and bottom next-meeting row.
  - Current Option 2 mockup pass lives in `design-mockups/casio-target.html`: the central time/steps cluster is now the design anchor and is built from explicit segmented SVG digits with ghost 8s underneath and black active segments. The rest of the face is arranged around that cluster only as pixel space allows.

Option 2 / Casio status:
- Active again after Option 1 framing lock. The hand-rolled seven-segment approximation was not acceptable and should stay abandoned.
- Use `/tmp/W800` / https://github.com/JohnEdwa/W800 as the future template/font reference. Useful assets were copied for reference under `design-mockups/vendor/w800/`.
- W800 is GPL-3.0, so directly deriving from its fonts/code means the watchface should be treated as GPL-compatible if distributed.
- Most relevant W800 assets: `FONT_HOUR_BIG_42.TTF`, `FONT_HOUR_35.TTF`, `FONT_SECOND_25.TTF`, `FONT_INFO_BIG_16.TTF`, `FONT_INFO_SMALL_16.TTF`, `FONT_FOURCHAR_16.TTF`, `FONT_BAT_5.TTF`, `IMAGE_BG_TINY.PNG`, `IMAGE_BUTTON_LABELS.PNG`, `IMAGE_SHEET_BRANDING.PNG`, and `IMAGE_SHEET_TOGGLES.PNG`.
- Use `/tmp/zig-pebble-face` / https://github.com/qt-dork/zig-pebble-face as a visual framing reference only unless licensing is explicitly acceptable. Its license is the Anti-Capitalist Software License v1.4, so do not vendor or ship its assets by default.
- RePebble W800H/W96H feature reference: five customizable slots, optional shake/tap alternate info, battery/Bluetooth/quiet-time toggles, optional weather boxes, W800/W96H background styles, and configurable branding images. Source: https://apps.repebble.com/w800h-w96h-casio_593ede1eb67f9fb68e000e1e
- Reset after bad mockup pass: do not use the full Royale face as the base. Use only the north/south red rail frame language first (`design-mockups/casio-frame-base.png`), then place W800/Quartz assets into that clean frame.
- Current W800 LCD pass: `design-mockups/casio-w800-lcd-pass.png` starts from a clean black 200x228 canvas with only the north/south red rails, then adds W800-derived pieces: Pebble logo crop from `IMAGE_SHEET_BRANDING.PNG`, W800 `FONT_HOUR_BIG_42.TTF` for the main time, W800 `FONT_SECOND_25.TTF` for the step counter, pale ghost 8s underneath, and black active LCD digits. The date, step counter, and main time are right-aligned within their windows. The walking-man icon now uses `reference-screenshots/walking-man.png`, trimmed, flipped left, and shrunk to a watchface-scale icon.
- Latest step-counter-only fix: restored the ghost placeholder digits in the step counter, rendered them in fixed cells so they stay inside the border, and moved/shrank the left-facing walking-man icon so it no longer touches the step box. No unrelated layout changes should be bundled with this kind of fix.
- Step counter now supports five digits in the mockup (`11105`) so values above 9,999 fit. Saved rollback snapshots: `design-mockups/snapshots/casio-w800-lcd-pass-step5-saved.png` and `design-mockups/snapshots/casio-w800-lcd-pass-step5-saved-3x.png`.
- Follow-up step-counter scale fix: five-digit step counter was scaled up and repositioned so it uses more of the existing box while staying inside the border.
- Follow-up step-counter artifact fix: the font-rendered step layer still produced a semicolon-like artifact, so it was replaced with a composed sprite layer using actual digit crops from Royale `type-med.png`. Saved as `design-mockups/vendor/w800/images/steps-sprite-11105.png`; only the step-box interior was changed.

Casio target details from Josh:
- Final watchface renders only the 200x228 screen, not the outer Pebble Time 2 case/strap from the mockup.
- Replace the `QUARTZ` wordmark with a high-quality Pebble logo/wordmark.
- Retain the Quartz-style featured steps module above the main time, including the small walking-man icon and step count treatment.
- Default lower circular complications: watch battery as a number plus watch icon C, rain percentage, and weather icon plus temperature.
- Date row should use the full weekday plus month/day, following the annotated mockup (example: `Thursday, May 7`).
- Main time and steps are the primary design features and should closely recreate the Quartz watchface treatment: ghost 8s underneath, black active LCD segments on top, and the step count above the time with the walking-man icon.
- Everything else should be built around the time/steps cluster as the 200x228 pixel budget allows.
- The next meeting belongs in the bottom `E-PAPER DISPLAY` position, not inside the empty red accent bar.
- Avoid browser/default fonts where possible. Current mockup uses W800/Royale assets for logo/digit inspiration, explicit segmented SVG for the LCD digits, and Noto Sans Mono Condensed as a legible open-source fallback for text labels where the W800 letter fonts render incorrectly in Chromium.

Collaboration rule from Josh:
- Do not remove requested features as a shortcut.
- Fix exactly the issue called out unless Josh explicitly asks for broader redesign.
- Do not falsely substitute a different made-up goal as equivalent to the stated request.

Backend direction:
- Borrow the Business Time settings model where possible: Clay-style configuration, appearance options, Bluetooth/weather/widget/health sections, user-selectable complication slots, persisted settings delivered over AppMessage.
- Business Time source was not found locally. If source is found later, inspect licensing before copying code directly. Until then, use the visible settings screenshots as the functional spec and prefer MIT/official Pebble patterns such as Clay.

Current watchface C code still contains the prior Business Time visual pass in `jy-time/src/c/jy-time.c`; do not treat that as the target design.

---

## Goal
Pebble Time 2 watchface using the Pixel-style watchface's practical information hierarchy and optional Casio/quartz styling, with phone-fed calendar/weather/battery data from the optional Android companion.

**Approved plan:** `/home/jates/.claude/plans/okay-enter-plan-mode-refactored-wand.md`

## Phone battery: companion app confirmed required (verified 2026-04-30)
Re-verified after Pebble's Core Devices relaunch (repebble.com, apps.repebble.com). No SDK path exists to read phone battery from the watch directly. A custom Android companion is the only viable option.

What was checked:
- **Watch C SDK `BatteryStateService`**. Watch battery only, no phone hooks.
- **PebbleKit JS** (sandbox inside the official Pebble app). Exposed APIs are AppMessage, `localStorage`, `XMLHttpRequest`, `navigator.geolocation`, `getAccountToken`, `getWatchToken`, `getActiveWatchInfo`, notifications. No `navigator.getBattery()`, no phone hardware sensors. Battery Status web API is also deprecated in modern browsers for fingerprinting reasons, so even if Core Devices modernizes the sandbox, it likely won't appear.
- **Official Pebble app (`coredevices/mobileapp`, built on `libpebble3`)**. Open source, no built-in phone-battery-to-watchapp pipe. Forwards health, notifications, calls, music. Not battery.
- **AutoPebble + Tasker**. Only no-custom-companion alternative. Phone battery only displays while AutoPebble is the foreground app on the watch (can't sit under a watchface), and recent reviews report it broken on Pebble Time models. Not viable for a watchface.

Decision: custom Android companion using PebbleKit Android 2, send phone battery over AppMessage. (Already the chosen architecture, see Components built.)

Brand note: "Core Devices" (Eric Migicovsky's new company, makes Pebble Time 2, runs repebble.com and apps.repebble.com) is distinct from "Rebble" (the older community service). The two are in an open dispute over the appstore. "Repebble" matches Core Devices' own marketing.

Sources:
- https://developer.repebble.com/guides/communication/using-pebblekit-js/
- https://developer.repebble.com/guides/communication/using-pebblekit-android/
- https://github.com/pebble-dev/PebbleKitAndroid2
- https://github.com/coredevices/mobileapp
- https://joaoapps.com/autopebble/

## Hardware / target
- **Watch:** 2025 Pebble Time 2 (emery platform, 200×228 color e-paper)
- **Phone:** Pixel 10 XL (Android 14+)

## Toolchain installed
- Pebble SDK 4.9.148 at `~/.pebble-sdk/`
- `pebble` CLI at `~/.local/bin/pebble` (via uv → pebble-tool)
- `pipx`, `libsdl1.2debian`, `libfdt1` installed via apt

## Components built

### Watchface (`jy-time/`) — code complete, tested on emery emulator
| Aspect | Status |
|---|---|
| Layout (3 widgets, divider, weekday, date, time, event, divider, stats) | ✅ |
| Black bg + white fg theme lock | ✅ |
| Real time/date/weekday via tick_timer_service | ✅ |
| Real watch battery via battery_state_service | ✅ |
| Bluetooth state via connection_service | ✅ |
| Steps + BPM via HealthService | ✅ |
| AppMessage inbox handlers (NEXT_EVENT, PHONE_BATTERY, WEATHER_CODE, TEMPERATURE, RAIN_CHANCE, TOP_STEPS) | ✅ |
| Persistence across launches (last-known values) | ✅ |
| Native companion metadata in package.json | Deferred; removed from Pebble Appstore PBW |
| Build artifact | `jy-time/build/jy-time.pbw` (5537 B resources, 5934 B RAM footprint) |

### Android companion (`jy-time-companion/`) — optional/deferred phone-data sender
| File | Status |
|---|---|
| Constants.kt | ✅ |
| MainActivity.kt (Compose UI + permissions) | ✅ |
| WatchSyncService.kt (foreground service, 60s loop) | ✅ |
| CalendarReader.kt (CalendarContract.Instances query) | ✅ |
| BatteryReader.kt (ACTION_BATTERY_CHANGED) | ✅ |
| WeatherFetcher.kt (FusedLocation + OkHttp + Open-Meteo, 30min cache) | ✅ |
| BootReceiver.kt | ✅ |
| AndroidManifest.xml (permissions, services, dataSync foreground type) | ✅ |
| build.gradle.kts + libs.versions.toml | ✅ (PebbleKit2 1.1.0 from jitpack) |

## Phase progress

| Phase | Status | Notes |
|---|---|---|
| **A** Visual skeleton | ✅ done | `screenshot-phaseA-static.png` |
| **B** Watch-side data | ✅ done | `screenshot-phaseB-realtime.png` |
| **C** Watchface AppMessage handlers | ✅ done | `screenshot-phaseC-with-appmessage.png` |
| **C** Android companion app | ⏸ deferred | Source exists and debug build passed, but it is not part of the current Pebble Appstore PBW |
| **D** Polish | ✅ approved for 0.1.0 | Default/off and top-step/on screenshots approved |
| **E** Submit | ⏳ pending | Upload `release-assets/simple-pixel-style-0.1.0-emery.pbw` and approved screenshots through the Pebble Developer Dashboard |

## Next actions you take

1. Log into the Pebble Developer Dashboard.
2. Add a watchface listing for `Simple Pixel Style`.
3. Upload `release-assets/simple-pixel-style-0.1.0-emery.pbw`.
4. Add the approved emery screenshots from `release-assets/`.
5. Do not add a required companion app for this 0.1.0 Pebble Appstore submission.

## Future enhancements (v2+)
- **Shake action.** `accel_tap_service_subscribe()` handler. Behavior TBD when v1 ships.
- Configurable widgets (Clay), theme picker, OpenWeatherMap fallback via user-supplied key, iOS companion.
- Custom WMO icons (currently 4-letter labels), custom bold font.

## Risks / known limitations
- **PebbleKit Android 2 is work-in-progress** (v1.1.0). API may break. Mitigation: pinned to 1.1.0 via libs.versions.toml.
- BPM widget shows "BPM 0" on emulator (no health data). Real device needs HRM enabled.
- Companion uses 60s foreground service polling — battery hit is small but real. CalendarContract change observers (more efficient) deferred to v2.
