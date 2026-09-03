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

## 0.81 CASIO fix pass — planned 2026-05-17

Plan: `Plan To Implement - 0.81 CASIO Fix Pass.md` (full audit, line-numbered fix surgery, verification checklist).

0.80 audit findings:
- Quiet-time mouse icon moved to top-left in CASIO mode. Reverting to bottom-right (matches non-CASIO behavior).
- AM/PM bitmap path uses non-standard compositing modes that render invisibly. Swapping to the proven pixel-art `draw_ampm_label` helper.
- Drop shadow code present but defaults OFF and the gray-on-gray color is nearly invisible. Defaulting ON, stronger contrast color, 2 px offset.
- Digits bottom-anchored instead of vertically centered. Fixing.
- Three digit sizes declared but Medium and Large clip in compact mode. Hiding from Clay dropdown for 0.81; C-side handles stay loaded so they can return when fit envelope is reworked.

0.81 adds:
- Optional `CASIO_SECONDS` toggle (default OFF) with explicit battery-drain warning. Subscribes to SECOND_UNIT only when CASIO mode AND seconds toggle are both on; otherwise stays on MINUTE_UNIT.
- New `FONT_WV58A_DIGITS_25` font resource backed by the already-vendored `DIGITAL_BOLD.TTF` (pattern lifted from Casio WV-58DE by Firefox42). Seconds render in this CASIO LCD font at point 25 immediately left of the mouse-icon column for visual coherence with the main time digits.

0.79 features that must remain intact in 0.81: forecast graph, sun icon swap, all complications, bubbles, weather, meeting bar.

Sun and moon position indicator bar (chat brainstorm, not in 0.81): saved to `FUTURE_FEATURES.md` as a candidate meeting-bar replacement mode. 24-dot bar, sun by day and moon by night with proper phase glyph, vendored from pebble-mss `moon_phases.ttf`. Lift 200 to 300 lines plus one vendored font; moderate.

## 0.82 CASIO cleanup — planned 2026-05-17

Plan: `Plan To Implement - 0.82 CASIO Cleanup (Phantom Segments + Anchor Fixes).md`

0.81 audit findings:
- "Drop shadow" Codex shipped is NOT what the user asked for. Real Casio look is **LCD phantom segments**: render "88:88" in muted color first, real time on top in foreground. Inactive segments stay visible as a faded backdrop. Sometimes called "ghost digits" or "8:88 backdrop."
- Mouse icon in CASIO mode anchored to `slot_bottom - 18` (Y = 120 in compact mode, 144 in verbose-one-line). Non-CASIO baseline is Y = 93 (TIME_VISUAL_BOTTOM - QUIET_TIME_ICON_SIZE). 27 to 51 px off.
- AM/PM bottom anchored to `slot_bottom - CASIO_GAP`, not to the digit bottom. Bottom pixels don't line up.
- Vertical centering formula is line-box centered, which looks bottom-heavy because DIGITAL_BOLD's font leading mostly lives above the visible ink.
- Seconds toggle renders invisibly below the time. Pulling entirely; revive in a future release after main render is solid.
- Drop shadow and digit-size selectors are now Clay clutter.

0.82 scope:
- Replace `LARGE_CASIO_TIME` toggle + `CASIO_DIGIT_SIZE` select with a single `TIME_FONT` select at the TOP of the Layout section. Options: Roboto (default, FONT_KEY_ROBOTO_BOLD_SUBSET_49) or CASIO style. Designed to grow (ForecasWatch 2, others).
- Install proper LCD phantom segments in CASIO mode.
- Hard-code mouse icon Y to TIME_VISUAL_BOTTOM - QUIET_TIME_ICON_SIZE in both render paths. Always 93.
- Anchor AM/PM bottom to digit bottom. Same pixel line.
- Vertically center digits with empirical leading correction (Codex tunes in emulator).
- Delete drop shadow code + Clay option.
- Delete seconds code + Clay option (back to MINUTE_UNIT unconditionally).
- Delete digit size Clay option (Size 70 and 90 font handles stay loaded for future).
- 0.79 features intact: forecast graph, sun icon, all complications, weather, meeting bar.

Future feature notes appended to `FUTURE_FEATURES.md`: additional time fonts (ForecasWatch 2 et al.), bring-back conditions for seconds, bring-back conditions for medium/large digit sizes.

## 0.87 shipped — 2026-05-17

Final release of the day. Commit `3901d09`, artifacts at `release-assets/simple-pixel-style-0.87-emery.pbw` and `release-assets/RELEASE_NOTES_0.87.md`.

The CASIO iteration (0.79 through 0.86) consolidated into a stable 0.87 with these visible changes vs the prior shipped state:

- **Time font select** at the top of Layout. Four options: Default (`FONT_KEY_BITHAM_42_BOLD`, the original; AM/PM keeps its `visual_bottom` anchor), CASIO style (vendored WV-58A DIGITAL_BOLD at point 55, drawn in the same time_frame as the other fonts with no special centering math), Roboto (`FONT_KEY_ROBOTO_BOLD_SUBSET_49`, AM/PM anchored to digit bottom), LECO (`FONT_KEY_LECO_32_BOLD_NUMBERS`, AM/PM anchored to digit bottom). Replaces the LARGE_CASIO_TIME boolean. Persist key 212 reused with backward-compatible mapping (0 = Default, 1 = CASIO).
- **CASIO phantom 88:88 backdrop toggle.** Defaults ON. The JS sender bug from prior 0.84 builds (used `=== false` which missed Clay's integer payloads) is fixed; toggling off actually disables the backdrop now. `CASIO_PHANTOM: true` added to `defaultSettings`.
- **Forecast graph in Detailed Weather shake overlay rebuilt** as a closer port of forecaswatch2 by mattrossman. Correct draw order: night-hatch background, precip filled area, sunrise/sunset boundary lines, temp line on top with stroke width 3. Hour labels using the local start hour, tick marks every hour, no plot border. Night hatching uses forecaswatch2's diagonal-aligned algorithm (x + y_start modulo spacing) so diagonals stay continuous across segments.
- **Sunrise/sunset labels widened** from 48/58 px to 72/76 px and icons re-spaced so "12:45AM" and "12:45PM" no longer truncate the AM/PM letters.
- **Mouse icon hard-anchored** to `TIME_VISUAL_BOTTOM - QUIET_TIME_ICON_SIZE` (Y=93) in every font mode and every layout. Decoupled from weather-bar `slot_bottom` which was causing position drift in verbose modes.
- **Color picker label** "Black & White - Tuxedo" renamed to "B&W - Tuxdeo".
- **Removed from Clay**: LARGE_CASIO_TIME toggle, CASIO_DIGIT_SIZE select, CASIO_DROPSHADOW toggle, CASIO_SECONDS toggle. CASIO 70 and 90 pt font handles stay loaded on the C side so the size selector can return when its fit envelope is reworked.
- **Step counter font stays W800 sprite only.** Experimental Roboto and CASIO step font options from intermediate 0.84 builds were pulled (user found them broken / hard to rationalize). The W800 sprite already adds a comma separator for 4+ digit counts.

Bundled at commit time but not yet pushed:
- 7 commits ahead of `origin/main` (the 6 prior unpushed releases 0.63 through 0.76 plus the new 0.87). Push blocked by `Bash(git push*)` deny rule in `~/.claude/settings.json`; needs manual push from terminal or removal of the deny rule.

Vendored deps committed to repo (were untracked before):
- `jy-time/resources/fonts/DIGITAL_BOLD.TTF`
- `jy-time/resources/images/wv58a-am.png`, `wv58a-pm.png`
- `jy-time/resources/images/icon-bitcoin-bubble.png`, `icon-stocks-bubble.png`
- `jy-time/LICENSES/WV-58A-ATTRIBUTION.txt`

Source of truth for the iteration: `Plan To Implement - 0.80 CASIO Fix.md`, `Plan To Implement - 0.81 CASIO Fix Pass.md`, `Plan To Implement - 0.82 CASIO Cleanup (Phantom Segments + Anchor Fixes).md`. Plans drove the work but the final 0.87 ship deviated from the per-version plans in places (e.g., the step font experiment was pulled mid-iteration rather than carried forward).

`FUTURE_FEATURES.md` still tracks the queue. Two items worth flagging here for the next planning pass:

- **NetHack-style Your Day shake overlay.** Plan: `Plan To Implement - Your NetHack Day Shake Overlay.md`. Roguelike-styled variant of the existing Your Day overlay (ASCII / dungeon-map vibe with events along the day's timeline). Has its own dedicated plan file; not yet scheduled.
- **Sun and moon day/night progress bar** as a meeting-bar replacement mode. 24 dots, one per hour, full-day window. **The center of the bar is noon** (bar runs midnight on the left, 11pm on the right, so the geometric midpoint lands on the noon mark — gives a left=AM / right=PM read at a glance). Sun icon by day, moon icon (with vendored `moon_phases.ttf` from pebble-mss) by night. Past dots filled, future dots hollow. Lift 200 to 300 lines plus one vendored font.

Other queued items in `FUTURE_FEATURES.md`: additional time fonts beyond LECO, bring-back conditions for the CASIO seconds toggle, bring-back conditions for medium/large CASIO digit sizes.

## 0.99 shipped — 2026-05-23

Artifact: `release-assets/simple-pixel-style-0.99.0-emery.pbw`.

Three weather-path changes bundled together:

### 1. NWS overrides for the Your Day overlay (carried from 0.98)

When `WEATHER_PROVIDER = nws`, the NWS path now also writes `RAIN_CHANCE`, `WEATHER_CODE`, `HIGH_TEMP`, and `LOW_TEMP` from NWS hourly + forecast data, not just `WEATHER_SUMMARY`. Previously these stayed on Open-Meteo even when NWS was selected, which caused the Your Day overlay to display 6% rain while NWS reported 20% for the same ZIP. Two new helpers: `nwsShortForecastToWmoCode` (maps NWS shortForecast strings to the WMO codes the C-side icon mapper expects) and `nwsTodayHiLo` (extracts today's high/low from `forecast.properties.periods` with the late-evening case handled — when periods[0] is "Tonight", low comes from periods[0] and high from periods[1]).

### 2. NWS verbose-weather summary v2 — certainty-aware, pixel-fit aware

Replaces the NWS path's `nwsVerboseWeatherSummary` with a new builder that respects NWS's confidence prefixes ("Chance", "Slight Chance", "Likely", bare) instead of treating any `/thunderstorm/` regex hit + PoP ≥ 30 as actively storming. The old logic produced strings like "STORMS TIL 1P" for a 31% chance morning, which mismatches what NWS itself describes as "Mostly sunny, with a slight chance of showers and thunderstorms before 4pm."

New per-hour classifier returns ACTIVE / CHANCE / SLIGHT / CLEAR:
- ACTIVE: PoP ≥ 60, or bare precip term, or phrase contains "Likely"
- CHANCE: PoP 30–59 or phrase contains "Chance" (not "Slight Chance")
- SLIGHT: PoP 10–29 or phrase contains "Slight Chance" / "Areas of" / "Patchy"
- CLEAR: PoP < 10 and no precip term

Tiered output (TIL/AT only at real ACTIVE transitions):
- Tier 1 (currently ACTIVE): "STORMS TIL 4P" or "STORMS ALL DAY"
- Tier 2 (ACTIVE later): "STORMS AT 2P"
- Tier 3/4 (CHANCE/SLIGHT, no future ACTIVE): "{base}, {pop}% {kind} {bucket}" e.g. "SUNNY, 40% STORMS LATE" — base condition pulled from the first CLEAR hour in the next 18h, falling back to parsing `forecast.periods[0].detailedForecast` for the base phrase when no hour in the window classifies CLEAR
- Tier 5 (all CLEAR): just the base condition

Pixel-aware string fitting: per-character widths for `FONT_KEY_GOTHIC_18_BOLD` were measured empirically in the emery emulator (one-shot `graphics_text_layout_get_content_size` loop, ASCII 32–126) and stored as `RG18B_W` in pkjs. `pickFittingSummary` evaluates candidates longest-first and returns the longest that fits the budget. Budgets:
- Two-line layout: 184 px (the full summary row in 18_BOLD)
- One-line layout: 125 px (after worst-case "100°F " temp on the shared row)

New `WEATHER_SUMMARY_COMPACT` AppMessage key + persist key 238 + 32-byte buffer on the C side. `draw_verbose_weather_row` reads the compact buffer in one-line mode and the full buffer in two-line, falling back to full when compact is empty so Open-Meteo's path still renders unchanged.

Open-Meteo's `verboseWeatherSummary` is intentionally untouched as the control group — switch `WEATHER_PROVIDER` between `nws` and `open_meteo` via Clay config to A/B compare.

### 3. NWS weather icon respects the v2 classifier

QA pass after the first 0.99 build caught a mismatch on the NWS path: the icon mapper called `nwsShortForecastToWmoCode(periods[0].shortForecast)` directly, which would match `/thunderstorm/` on "Chance Showers And Thunderstorms" and render a thunderstorm icon next to "SUNNY, 30% STORMS AM" text. New helper `nwsDisplayWeatherCode` routes through the same classifier the verbose summary uses:

- Current hour ACTIVE → icon from current `shortForecast` (storm icon when storming)
- Current hour CHANCE / SLIGHT / CLEAR → icon from the dominant base condition (sun / cloud icon, matches text)

Only changes behavior in the specific mismatched case; other tiers' icons unchanged.

### 4. Detailed Weather shake graph — "100%" → "100"

The rain-axis label at the top of the forecast chart in the Detailed Weather shake overlay was truncating to "10…" because its 26-px-wide GRect couldn't fit "100%" in `FONT_KEY_GOTHIC_14_BOLD`. Dropped the `%` only from the 100 label since the axis context (the 50% and 0% labels still carry the unit) makes it unambiguous.

### Files changed

- `jy-time/src/pkjs/index.js` — width table, `measureG18B`, `pickFittingSummary`, NWS classifier + base extractor + summary builder v2, NWS shared-key overrides for Your Day
- `jy-time/src/c/jy-time.c` — `PERSIST_KEY_WEATHER_SUMMARY_COMPACT = 238`, compact buffer, inbox handler, persist load, render selector, "100" label
- `jy-time/package.json` — version 0.99.0, added `WEATHER_SUMMARY_COMPACT` to messageKeys

### Verification

- `pebble build` succeeds for emery
- Synthetic + live NWS test cases (in throwaway `/tmp/test_nws_v2.js`) cover all five tiers; outputs documented in chat transcript
- 36542 live data: old "STORMS TIL 1P" → new "SUNNY, 40% STORMS LATE" (two-line) / "MOSTLY SUNNY" (one-line)
- Pixel widths captured by booting the watchface in the emery emulator with a one-shot measurement loop in `init()`; instrumentation reverted before ship

### Emulator install gotcha (worth flagging)

`pebble install --emulator emery` hangs silently when there's stale state in `~/.pebble-sdk/<ver>/emery/qemu_spi_flash.bin` from a previous half-started qemu-pebble run. Fix: `pkill -9 -f "qemu-pebble|pebble-tool|pypkjs"` then `pebble wipe` then retry. Memory note `feedback_pebble_emulator_install.md` updated with the wipe path.

## 1.01 shipped — 2026-05-23

Artifact: `release-assets/simple-pixel-style-1.01.0-emery.pbw`.

Tufte-aligned cleanups across both weather graphs plus the Your Day overlay and the fitness ring overlay. All changes ran through the `tufte-viz` skill applied to existing visualizations.

### Weather graph changes

**NWS Chart Heavy (`nws_draw_dual_chart` / `nws_forecast_chart_heavy_draw`):**
- Deleted the chart frame border (data lines now define the chart extent themselves — range-frame principle).
- Deleted the dashed 25 / 50 / 75 % horizontal gridlines (moiré + redundant ink).
- Dropped the mid-axis labels on both temp and precip axes. Labels now show only data extremes (max/min temp, 100/0 precip), with `"0%"` simplified to `"0"` for symmetry with `"100"`.
- Muted both axis label colors from saturated `GColorDarkCandyAppleRed` / `GColorPictonBlue` to `fitness_muted_text_color()` so the colored data lines visually dominate (Tufte layering: data > labels).
- Added sunrise/sunset vertical lines on the chart (ported pattern from the Detailed Weather graph), drawn in muted color at stroke 1 so they recede behind the stroke-2 data lines. Bounds-checked to skip drawing if the event falls outside the chart's 24h window. `int64_t` time arithmetic to avoid overflow on Pebble's 32-bit `time_t`.

**Detailed Weather graph (`draw_forecast_graph`):**
- Dropped minor tick marks. Previously drew a tick at every hour (height 2 unlabeled, height 4 labeled). Now ticks only render at the labeled positions (every 3rd hour) at height 4. The 21 unlabeled minor ticks were pure non-data ink.

### Your Day overlay change

- Title ("FUTURE" / "X HOUR WORKDAY") font dropped from `FONT_KEY_GOTHIC_18_BOLD` to `FONT_KEY_GOTHIC_14_BOLD`, GRect height reduced from 24 to 18. Position, color (`fitness_muted_text_color()`), and divider line unchanged. The pip row now reads as the primary information; the title recedes as a mode label.

### Fitness ring overlay changes (`fitness_draw_ring`)

- **12 o'clock reference notch on each track ring.** Small outward tick at the loop-closes position, drawn in track color at stroke 2. Sized so it sits in the gap between rings (or just outside the outermost ring) where it's actually visible — first-pass implementation had the notch landing inside the gray ring stroke and rendering invisible; fixed by moving offsets from -2/-5 to -6/-9 and bumping stroke 1 → 2.
- **Zero-state indicator dot.** When `value < 1` and `target > 0`, a small color-matched filled circle (radius 3) draws at the 12 o'clock start position so "data loaded, you're at zero" is visually distinguishable from "data missing." Uses the metric color so each ring's zero state is clearly tied to its specific metric.

Explicitly out of scope for this release (queued in `FUTURE_FEATURES.md` or rejected):
- Current-time dot + inline value annotation on chart data lines (rejected, scope).
- 12-hour-format hour labels on the Detailed Weather graph (rejected, separate from Tufte cleanup).
- Cross-graph consistency between the two charts' hour-label formats (rejected, separate concern).
- Fitness ring sparklines showing historical comparison (queued for a future release; requires daily persist).
- Overflow visualization on completed fitness rings (text row already shows raw numbers).

### Files changed

- `jy-time/src/c/jy-time.c` — `draw_forecast_graph`, `nws_draw_dual_chart`, `nws_forecast_chart_heavy_draw`, `your_day_draw_overlay`, `fitness_draw_ring` function bodies only. No new helpers, no new statics, no new `#define`s, no new message keys.
- `jy-time/package.json` — version bumped to 1.01.0.

### Verification

- `pebble build` succeeds. No new warnings (existing pre-existing warnings for unused `forecast_x_for_time`, `snprintf` format-truncation, and linker RWX all carried over unchanged).
- Sample screenshots captured during planning (under repo root: `tufte-nws-sample.png`, `tufte-detailed-weather-sample.png`) verified the graph cleanups before commit. Fitness ring notch / zero-dot changes were math-verified against the ring stroke geometry (stroke 11 centered on radius → outer edge at radius + 5.5) but not screenshot-verified — visible inspection on watch is the final verification step.

### Process notes

- `tufte-viz` skill installed at `~/.claude/skills/tufte-viz/` from a downloaded SKILL.md + references bundle. Used here for the first time on this project.
- Codex implementation of the chart cleanups + ring additions went cleanly once the sample-build prompt was tightened with explicit DO NOT guard rails (no test-data injection, no emulator drive, no version bump, no commit). One math bug in the notch offsets was on my spec, not on Codex's implementation — fixed in a follow-up patch before commit.

## 1.02 shipped — 2026-05-23

Artifact: `release-assets/simple-pixel-style-1.02.0-emery.pbw`.

Cross-provider parity: whichever weather source the user selects now drives every weather-related complication on the watch, and both providers produce verbose-summary text from the same v2 taxonomy.

### NWS override coverage extended

The NWS path (`nwsSendData`) now also overrides these shared keys when WEATHER_PROVIDER=nws:
- `TEMPERATURE` — current temp from NWS hourly `periods[0].temperature`
- `WIND_SPEED` — first integer parsed from NWS hourly `periods[0].windSpeed` (NWS returns strings like "10 mph" or "10 to 20 mph")
- `FORECAST_TEMP_F` / `FORECAST_PRECIP_PCT` / `FORECAST_START_T` / `FORECAST_LAST_UPDATE_T` — the 24h arrays the Detailed Weather graph reads from. Same pack helpers (`nwsHourlyPackTemps`, `nwsHourlyPackPrecip`, `nwsHourlyStartEpoch`) we already use for the dedicated NWS_HOURLY_* keys, now routed to FORECAST_* too. The Detailed Weather chart reflects NWS forecast data instead of Open-Meteo's when NWS is selected.

Already covered in 0.99: `HIGH_TEMP`, `LOW_TEMP`, `RAIN_CHANCE`, `WEATHER_CODE`, `WEATHER_SUMMARY`, `WEATHER_SUMMARY_COMPACT`.

Intentionally not overridden by NWS (keeps Open-Meteo even when NWS is provider):
- `FEELS_LIKE` — NWS doesn't compute apparent temperature. Per user decision, no point computing a synthetic version; real temp is what matters.
- `UV_INDEX` — NWS doesn't expose UV index in the forecast endpoint.
- `SUNRISE_T` / `SUNSET_T` — purely astronomical, identical regardless of provider.

### Open-Meteo verbose summary ported to v2

The original Open-Meteo `verboseWeatherSummary` (the "STORMS TIL 1P" logic that conflated 31% chance with active storms) is removed. Replaced with `omBuildVerboseSummariesV2`, which mirrors the NWS v2 design:

- `omClassifyHourState(code, pop)` — same ACTIVE/CHANCE/SLIGHT/CLEAR tiers. PoP-only classification (>= 60 ACTIVE, 30-59 CHANCE, 10-29 SLIGHT, < 10 CLEAR). WMO code separately determines the precip kind via the existing `weatherEventLabel`.
- `omDominantBase(hourly, currentCode, utcOffsetSeconds)` — scans hourly weather codes for first CLEAR hour in next 18h, falls back to current code's base label (if it's a 0-48 clear/cloud/fog code) or "CLOUDY" (for all-day-precip-chance days).
- `omBuildSummaryCandidates` — produces the same five-tier candidate list as `nwsBuildSummaryCandidates`, using the same vocabulary: "MOSTLY SUNNY, 30% STORMS AM" for chance tiers, "STORMS TIL 4P" / "STORMS AT 2P" for ACTIVE transitions.
- `omBuildVerboseSummariesV2` — returns both two-line and one-line variants via the shared `pickFittingSummary` + RG18B width table from 0.99.
- `omDisplayWeatherCode(hourly, currentCode, utcOffsetSeconds)` — icon respects the v2 classifier. When current hour is CHANCE/SLIGHT, the icon shows the dominant base condition (sun/cloud) instead of the precip kind, matching the text. ACTIVE current still gets the precip icon. Same fix as `nwsDisplayWeatherCode` from 0.99.

Open-Meteo now sends both `WEATHER_SUMMARY` and `WEATHER_SUMMARY_COMPACT`, so the one-line vs two-line verbose-weather layout works identically across providers.

### Dead code removed

Now-orphaned helpers deleted from `src/pkjs/index.js`:
- `verboseWeatherSummary` (replaced by `omBuildVerboseSummariesV2`)
- `significantWeatherEvent` (only used by the deleted `verboseWeatherSummary`)
- `nwsVerboseWeatherSummary` (replaced by `nwsBuildVerboseSummariesV2` in 0.99 but kept as control through 1.01; removed in 1.02 now that v2 is on both providers)
- `nwsSignificantWeatherEvent` (only used by the deleted `nwsVerboseWeatherSummary`)

No more provider-asymmetric verbose-summary behavior. Switching WEATHER_PROVIDER between `nws` and `open_meteo` toggles the data source but the phrasing taxonomy stays identical.

### Files changed

- `jy-time/src/pkjs/index.js` — Open-Meteo v2 helpers added (`omClassifyHourState`, `omHourPrecipKind`, `omBaseLabelSimple`, `omDominantBase`, `omFirstActiveIdx`, `omEndOfActiveRun`, `omPeakChanceInfo`, `omDisplayWeatherCode`, `omBuildSummaryCandidates`, `omBuildVerboseSummariesV2`); Open-Meteo callsite in `fetchWeatherForCoordinates` updated; NWS override list in `nwsSendData` extended for TEMPERATURE/WIND_SPEED/FORECAST_*; orphan old verbose-summary helpers deleted on both providers.
- `jy-time/package.json` — version bumped to 1.02.0.

### Verification

- `pebble build` succeeds. No new warnings.
- Manual emulator verification was attempted but the emulator install hung repeatedly (the documented `pkill + pebble wipe` fix didn't resolve cleanly in this session). Skipped emulator verification. Code-path verification only: tracing through `fetchWeatherForCoordinates` and `nwsSendData` confirms each provider's override pattern writes the new keys, and the v2 helpers were validated against live NWS data for ZIP 36542 during the 1.01 development cycle. Visual verification on actual watch is the final step.

## 1.03 shipped — 2026-05-24

Artifact: `release-assets/simple-pixel-style-1.03.0-emery.pbw`.

Tufte-aligned cleanups on the tide chart shake overlay. Three small changes inside `tide_chart_draw_overlay` and `format_time_short`:

- **Colon alignment in stacked time rows.** `format_time_short` previously stripped the leading "0" from single-digit hours via `memmove`, producing variable-width times ("5:30PM" vs "11:45AM") whose colons landed at different X positions when rendered in stacked footer rows. Replaced the strip with `tmp[0] = ' '` so single-digit hours pad to ` 5:30PM` and align with `11:45AM`. The minute digits now line up vertically across the High / Low rows on the tide overlay.

- **"Now" indicator drawn behind the data lines.** The dotted vertical reference at the current-hour column used to draw after the past/future tide curve, putting it on top of the data line at that column. Moved the dotted-line block to draw before the tide curve so the data dominates visually (Tufte layering: secondary annotations recede behind primary data).

- **Dropped colon from "Now %s" footer row** for consistency with the unprefixed "High" / "Low " rows.

Bundled in the same release: the equivalent changes on the standalone US Tidemaps app (see that repo for the parallel diff — colon alignment in its `format_time_short`, chart frame border deleted, past/future stroke encoding ported, "now" indicator moved behind data lines, "Now" colon dropped).

### Files changed

- `jy-time/src/c/jy-time.c` — `format_time_short`, `tide_chart_draw_overlay` only.
- `jy-time/package.json` — version bumped to 1.03.0.

### Verification

- `pebble build` succeeds with no new warnings.
- Codex implemented the Pebble watchface side; Claude reviewed the diff for spec correctness and scope before commit. No deviations.

## 1.04 shipped — 2026-05-24

Artifact: `release-assets/simple-pixel-style-1.04.0-emery.pbw`.

Feature parity with the US Tidemaps standalone app: a 24/48-hour view toggle for the tide chart shake overlay, controlled via a new Clay setting.

### What changed

**Clay setting**: new `TIDE_VIEW_HOURS` select in the tide settings section (right after Tide units). Options:
- "24 hours (12h past + 12h future)" — the prior behavior, default
- "48 hours (24h past + 24h future)" — matches US Tidemaps' wider window

Default is 24, so existing users see no change unless they opt in. The setting persists across watchface restarts.

**Data layer**: storage doubled from 24 hours to 48 hours centered on now (`TIDE_NOW_INDEX` 12 → 24, `TIDE_WINDOW_HOURS` 24 → 48). NOAA already returns a ±24h window for the existing fetch — only the packing slice widened, no new network requests. `TIDE_DATA_VERSION` bumped 2 → 3 so the existing version-mismatch wipe logic handles the layout migration cleanly on first boot of 1.04.

**Render path**: `tide_chart_draw_overlay` now slices the stored 48-hour array down to the user's chosen view window before computing min/max, building points, and drawing. The "now" indicator and past/future stroke encoding still anchor on `TIDE_NOW_INDEX` but the rendered slot positions are relative to `view_start`. Switching the Clay setting redraws against the same data; no re-fetch needed.

**Message keys**: `TIDE_HOURLY_LEVELS[24]` → `TIDE_HOURLY_LEVELS[48]`, plus new `TIDE_VIEW_HOURS`. Persist key `PERSIST_KEY_TIDE_VIEW_HOURS = 239` added.

### Files changed

- `jy-time/package.json` — version 1.04.0, message keys updated.
- `jy-time/src/pkjs/config.json` — Clay tide-view-hours select added.
- `jy-time/src/pkjs/index.js` — `TIDE_HOURS_BEFORE_NOW` / `TIDE_WINDOW_HOURS` doubled, `tideViewHoursValue` helper, `sendTideSetting` updated.
- `jy-time/src/c/jy-time.c` — `TIDE_WINDOW_HOURS` / `TIDE_NOW_INDEX` / `TIDE_DATA_VERSION` updated, 48-entry array initializer, new persist key + static + inbox handler + persist load, `tide_chart_draw_overlay` rewritten to slice based on `s_tide_view_hours`.

### Verification

- `pebble build` succeeds with no new warnings.
- Data layout migration is handled by the existing TIDE_DATA_VERSION mismatch path (wipes stale 24-byte data on first boot of 1.04 so the renderer doesn't draw a misleading partial chart while waiting for the fresh 48-byte fetch).

## 1.05–1.13 shipped — May 2026

Range covers the FuelBand pip row feature build-out and early tuning. Status entries for these individual releases were not added at the time; the current state of the feature is captured in 1.14–1.16 below.

## 1.14 shipped — 2026-05-26

Renamed "Banded FuelBand" color distribution to "Asymptotic FuelBand" and reshaped the band breakpoints to first 10 pips red, middle 6 yellow, last 4 green (was 5/7/8). The asymptotic shape loads the bar toward red so the green payoff at the right edge feels earned. Helper rewritten:

```c
int first_break  = count / 2;
int second_break = count - 4;
```

`FITNESS_PIP_COLOR_DIST` default flipped from linear to asymptotic in both the C side and Clay default. Linear gradient still available as an option.

## 1.15 shipped — 2026-05-26

Lap carryover: when the pip row enters lap 1+ (past the first daily step goal), pre-earned slots in the new lap now show the previous lap's filled shape underneath the current lap's outline. Passing the goal no longer visually wipes the row back to empty outlines.

`prev_shape` state added to `draw_fitness_pip_row`. Each slot's render path checks whether the current lap has earned that slot yet; if not, draws `prev_shape` filled. The active mid-fill slot still draws the current-lap outline with a left/right half-fill.

## 1.16 shipped — 2026-05-26

Pip row geometry moved down 2px across all four shapes (Rectangle, Pyramid, Square, Circle) and both pip sizes (Large, Small). Pure y-coordinate adjustment, no logic change.

## 1.17 shipped — 2026-05-26

Vibrate-on-goal: new Clay toggle under Fitness rings settings that fires a vibration the first time the daily step goal is crossed. Once-per-calendar-day enforcement via persisted `tm_yday`. Defaults to off.

New keys: `FITNESS_VIBRATE_ON_GOAL`, persist key 250 (toggle), persist key 251 (last-fired yday).

## 1.18 shipped — 2026-05-26

Goal vibe pattern selector: companion Clay select to the goal-vibrate toggle. Seven pattern options:

| ID | Label | API |
|----|-------|-----|
| 0 | Short pulse | `vibes_short_pulse()` |
| 1 | Long pulse | `vibes_long_pulse()` |
| 2 | Double pulse | `vibes_double_pulse()` |
| 3 | Heartbeat | custom pattern |
| 4 | Mario 1-up | custom pattern |
| 5 | SOS (Morse) | custom pattern |
| 6 | Rising | custom pattern |

Custom patterns use `vibes_enqueue_custom_pattern` with alternating on/off durations in ms. Pebble's native API ships only short/long/double pulses; everything else is a single-motor rhythmic approximation (no pitch).

New keys: `FITNESS_GOAL_VIBE_PATTERN`, persist key 252.

## 1.19 shipped — 2026-05-27

Added an 8th goal vibe pattern: **FF Victory Fanfare**. Cadence sourced from an XDA Android haptic-pattern thread, converted to Pebble's on/off form:

```c
{ 50, 100, 50, 100, 50, 100, 400, 100,
  300, 100, 350, 50, 200, 100, 100, 50, 600 }
```

Nine pulses (triplet + four sustained tones + flourish + finale). Around 2.7 seconds end to end.

## 1.20 shipped — 2026-05-27

Two unrelated bug fixes bundled in a single release.

**Fitness Rings stroke width regression.** `fitness_draw_ring` was drawing the 11px empty track, then setting stroke width to 2 for the 12-o'clock notch tick, then drawing the progress arc without resetting. Progress inherited the 2px stroke and rendered as a thin sliver inside the wider track. Fixed by resetting stroke width to `stroke_width` before the progress arc draw. (jy-time.c:935.)

**Calendar TZIDs Detroit / Toronto / London / Paris.** PebbleKit JS `timezoneOffsetMinutes` only recognized New York / Chicago / Denver / Phoenix / Los Angeles / Sydney. Other TZIDs fell through to local-floating interpretation, so a Detroit 4pm Nikki invite shown to a Central-timezone phone displayed as 4pm Central instead of 3pm Central. Added Detroit and Toronto as Eastern Time aliases, plus a shared `isEuropeanDst` helper for London (UTC+0/+1) and Paris (UTC+1/+2). EU DST transition rule is last Sunday of March 01:00 UTC through last Sunday of October 01:00 UTC.

The private ICS feeds for `joshtyates@gmail.com` and `josh@uniqueand.com` were pulled to `/home/jates/calendar-debug/` (outside any git repo) to confirm the diagnosis against real data. They are not tracked or pushed.

## 1.21 shipped — 2026-05-28

FuelBand pip lap progression rework, pass one. Circle base now goes Circle → Rectangle → Pyramid instead of Circle → Pyramid → Square. Rectangle base still Rectangle → Pyramid → Square at this point.

## 1.22 shipped — 2026-05-28

FuelBand pip lap progression rework, pass two. Rectangle base now also routes through Circle before Pyramid. Both bases converge at Pyramid for lap 2+:

| Base | Lap 0 | Lap 1 | Lap 2+ |
|------|-------|-------|--------|
| Rectangle | Rectangle | Circle | Pyramid |
| Circle | Circle | Rectangle | Pyramid |

Carry-over (pre-earned slots in higher lap showing previous-lap shape) updated to follow the new order. Square shape is no longer used in the default lap progression but the rendering code is still present (could be reused later).

## 1.23 shipped — 2026-05-29

New out-of-the-box defaults for fresh installs. A brand-new install now lands on an inverted black-and-white CASIO FuelBand layout instead of the previous plain dark default, so it looks finished without opening Clay.

Defaults were changed in **both** places that govern first-run appearance: the C static initializers in `jy-time.c` (what renders on a fresh install before any Clay Save, since `load_persisted()` only overrides a value when its persist key exists) and the matching `config.json` `defaultValue`s (what the Clay page pre-selects and writes on first Save). Keeping them in sync means the watch and the settings page agree before the user ever opens Clay.

| Setting | Old default | New default |
|---|---|---|
| Invert top bar / date bar / time | off | on |
| Time font | Default (Bitham) | CASIO style |
| FuelBand-style pip row | off | on |
| Pip color source | Match watch theme | Always color |
| Pip color (middle) | yellow `0xFFFF00` | mustardy orange `0xFFAA00` |
| Pip color (high end) | green `0x00FF00` | dark green `0x005500` |
| Vibrate on disconnect | off | on |
| Shake behavior | Off | Fitness rings |

Left unchanged because they were already the requested default: Light mode off, Color mode B&W Tuxedo, CASIO phantom 88:88 backdrop on, top W800 step counter on, pip shape Circle, pip size Large, pip fill Left-to-right, pip style Hollow track, pip color distribution Asymptotic FuelBand, pip low-end color red, military time off, remove-leading-zero off, verbose-weather/centered-time off, and all Weather-section defaults.

Pure default-value change, no logic touched; builds clean for emery (45 KB RAM footprint, 86 KB free heap). Emulator screenshot was skipped because `qemu-pebble` can't load `libsndio.so.7` in this environment; verified instead by clean compile and a config.json default audit. Artifact: `release-assets/simple-pixel-style-1.23.0-emery.pbw`.

## 1.24 shipped — 2026-05-31

Added a ninth goal vibe pattern, **Mario Theme** (`mario_theme`, pattern id 8), the Super Mario Bros. overworld opening riff (E E E, C E, G, low G). This is distinct from the existing **Mario 1-up** (id 4); both now sit in the Clay dropdown. The note sequence and durations were transcribed from `robsoncouto/arduino-songs` (tempo 200, so eighth = 150 ms and quarter = 300 ms) rather than guessed, then converted to the one-motor on/off form with motor-perceptible inter-note gaps (a literal 15 ms musical gap is imperceptible on the motor), the same adaptation the FF Victory pattern uses. Array: `{100, 50, 100, 200, 100, 200, 100, 50, 100, 50, 250, 350, 100}` (~1.75 s).

Preview-on-change: the watch now fires the selected pattern once when the goal vibe pattern is changed in Clay and saved, so each pattern can be felt without waiting to hit a step goal. Because settings load from persist at boot before any AppMessage arrives, an unchanged value (every launch and every settings sync) stays silent; only an actual change buzzes. Fires regardless of the "vibrate on goal" toggle, since it is an audition. The selector's Clay description now calls this behavior out.

Also fixed a pre-existing bug: the boot-time load in `load_persisted()` clamped the pattern to 6 (`if (v > 6) v = 6;`), so FF Victory (id 7) silently reverted to Rising on every watch reboot. Both clamps (inbox handler and boot load) now allow 0–8.

Files: `jy-time/src/c/jy-time.c` (pattern index comment, `fire_goal_vibe()` case 8, inbox-handler change detection + clamp, boot-load clamp), `jy-time/src/pkjs/config.json` (new dropdown option + description), `jy-time/src/pkjs/index.js` (`mario_theme` → 8 mapping), `jy-time/package.json` (1.23 → 1.24).

Builds clean for emery (no new warnings; same 45 KB RAM footprint, 86 KB free heap). Verified by clean compile and confirming `mario_theme` and `versionLabel 1.24` are present in the bundled `.pbw`. Haptic feel of the Mario rhythm and the preview trigger are not emulator-verifiable (no haptics; no phone testing) and are to be confirmed on-device after upload; if the rhythm feels off, the array can be retuned. Artifact: `release-assets/simple-pixel-style-1.24.0-emery.pbw`.

## 1.25 — 2026-06-01

Bottom meeting bar now advances on the watch's own clock. Previously the bar was a frozen, phone-formatted string (`11:29A | Title` / `NOW | Title`) that only changed when the PebbleKit JS calendar refresh ran (a 10-min `setInterval` plus on launch/config). Because pkjs is suspended in the background, a finished meeting could sit on screen indefinitely (observed: a meeting that ended at 11:00 still showing at 11:11). The watch had no times attached to the string, so it could not tell the meeting was over.

Fix: the phone now also sends, for each of the next 5 events, the raw title plus start/end epoch (new keys `MEETING_TITLE_1..5`, `MEETING_START_1..5`, `MEETING_END_1..5`). The watch stores and persists these and re-picks the bar on every `MINUTE_UNIT` tick (and on each calendar message, and on launch) via `recompute_meeting_bar()`: current meeting = started and not yet ended (on overlap, the one ending soonest), shown as `NOW | Title`; otherwise the soonest future event, shown as `H:MMA | Title` with a live countdown; otherwise `[None]`. Half-open interval (`start <= now < end`) so back-to-back meetings hand off cleanly at the boundary minute.

Additive and backward-safe: the legacy `NEXT_EVENT` / `NEXT_EVENT_DELTA` keys and the shake "calendar events" overlay (events 2-5) are unchanged. `recompute_meeting_bar()` is a no-op until the phone has sent the structured feed at least once (`s_meeting_feed_active`), so older or companion-only setups keep current behavior. The start-time prefix matches the phone's `formatHour()` exactly so the bar looks identical.

Files: `jy-time/package.json` (15 new messageKeys; version 1.24 to 1.25), `jy-time/src/pkjs/index.js` (`rawEventTitle` / `eventStartEpoch` / `eventEndEpoch` / `setMeetingSlot` helpers wired into the 3 calendar messages), `jy-time/src/c/jy-time.c` (persist keys 253-267, `s_meeting_*` state, `format_meeting_prefix()`, `recompute_meeting_bar()`, inbox ingest loop, tick + window_load hooks, persist load).

Builds clean for emery (46.6 KB RAM footprint, 84 KB free heap). Verified by clean compile, `versionLabel 1.25` + MEETING keys present in the bundled `.pbw`, and a logic trace of the current/next/none transitions. The emulator cannot run in this environment (`qemu-pebble` missing `libsndio.so.7`), so the minute-by-minute advance is to be confirmed on-device after upload. Artifact: `release-assets/simple-pixel-style-1.25.0-emery.pbw` (also in `jy-time/release-assets/`).

## 1.26: multi-timezone display (2026-06-02)

New optional feature: show up to two secondary timezones beneath the main time, as a text line (`GMT:5:24PM | AEDT:4:24AM`) or as circle complications. Off by default; when off the face is unchanged. Labels and DST-correct offsets are resolved on the phone and pushed to the watch (the watch has no tz database). 12/24h follows the existing `MILITARY_TIME` setting. CASIO/Pebble prior art review confirmed the right model is a curated named-zone dropdown (not free city search, not manual UTC-offset entry), which reuses the phone's existing DST resolver.

Claude implemented the full LOGIC layer (builds clean for emery, ~46 KB RAM footprint):
- `package.json`: 8 new messageKeys (`MULTI_TZ_ENABLED`, `MULTI_TZ_1`, `MULTI_TZ_2`, `MULTI_TZ_STYLE`, `MULTI_TZ_1_LABEL`, `MULTI_TZ_1_OFFSET_MIN`, `MULTI_TZ_2_LABEL`, `MULTI_TZ_2_OFFSET_MIN`). Version NOT bumped (stays 1.25).
- `src/pkjs/config.json`: new "Multi-timezone" section (enable toggle, two zone selects of ~15 IANA zones, display-style select). Dependent items hide when disabled.
- `src/pkjs/index.js`: extended `timezoneOffsetMinutes` with Perth/Tokyo/Singapore/Hong Kong/India/Dubai; added `timezoneLabel` (DST-aware abbreviation), `utcComponentsNow`, `sendMultiTzSetting`; wired into `ready`, `webviewclosed`, and a new 30-min refresh timer so DST flips propagate.
- `src/c/jy-time.c`: persist keys 268-273, six globals, `format_tz_time` (reuses the alt-tz overlay math, honors military time), `multi_tz_active_count`, inbox handlers, boot-load, and a gated render hook at the end of `face_update_proc`. Functional-but-rough `draw_multi_tz_line` and `draw_complication_timezone` are in place so the build links and the data path is verifiable.

Disabled-state invariant holds by construction: every new code path sits behind `if (s_multi_tz_enabled ...)`, which only flips via the brand-new keys no existing install has.

Codex implemented the visual layer; Claude reviewed and integrated it. `draw_multi_tz_line` uses GOTHIC_14 with a no-space `|` separator and section-aware color; `draw_complication_timezone` matches the complication circles (2px outline, COMPLICATION_RADIUS, stacked label over time, meridiem dropped inside the circle for 12h fit); `face_update_proc` forces verbose weather to compact for the draw pass in text mode (s_verbose_weather_large saved and restored in-memory, never persisted) and lets circle mode take precedence over verbose weather by rendering the zones in the complication row (slots 0 and 1, slot 2 keeps its normal complication).

Disabled state verified byte-identical: with the feature off, `draw_verbose_weather` equals `s_verbose_weather_enabled`, so every refactored expression collapses to the original, and the two inlined literals match the helpers (compact `weather_y` == `weather_band_y()` verbose-off == `COMPLICATION_CENTER_Y - COMPLICATION_RADIUS - 1`; compact `weather_h` == `weather_band_h()` verbose-off == `(COMPLICATION_RADIUS*2)+4` == 52). Clean build, only the three pre-existing warnings.

Cut as 1.26: package.json and package-lock bumped to 1.26, `release-assets/simple-pixel-style-1.26.0-emery.pbw` built (versionLabel 1.26 confirmed in the bundle) for upload. Git commit pending an explicit go (the working tree also has unrelated user deletions that must be kept out of a feature commit). On-device visual confirmation pending (emulator can't run here, `qemu-pebble` missing `libsndio.so.7`): eyeball CASIO-font text mode placement, the worst-case 12h two-zone width at 200px, and circle legibility/color in inverted themes. The existing shake `alt_timezone` overlay was left untouched.

## 1.27: calendar event dates (2026-06-03)

New optional feature from user feedback ("add the ability to display the dates of calendar events"): show each event's date (e.g. `06/04`) on the bottom event bar and in the Upcoming shake overlay. Off by default; when off, event rendering is byte-identical to 1.26.

Four Clay settings, all in the existing Calendar section, with the three sub-options hidden until the master toggle is on (same `customClay` AFTER_BUILD show/hide pattern as multi-timezone):
- `CAL_EVENT_DATES_ON` (toggle, default off) — master gate for both surfaces.
- `CAL_EVENT_DATE_ORDER` (month-first `06/04` default / day-first `04/06`) — bar + shake.
- `CAL_EVENT_DATE_POSITION` (before-time default / after-time) — bar only; the shake list always shows the date on each event's countdown line.
- `CAL_EVENT_DATE_NO_ZERO` (toggle, default off) — strips date leading zeros (`6/4`). Deliberately separate from the time-only `REMOVE_LEADING_ZERO` so the main clock is untouched.
Defaults match the user's own `06/04` example (month-first, leading zeros kept, date before the time).

Implemented entirely in C with no change to how calendar data is fetched or sent: the watch already holds each event's start epoch in `s_meeting_start[]` (added in 1.25), and the shake overlay's row i maps to the same slot i because both the `CALENDAR_EVENT_*` strings and the `MEETING_START_*` epochs come from the same sorted `displayEvents` list in `sendCalendarEvents()`. A shared `format_event_date()` helper (localtime → `MM/DD` or `DD/MM`, honoring order + leading-zero) feeds both `recompute_meeting_bar()` and `calendar_draw_overlay()`. Only the four new settings travel over AppMessage.

Disabled-state invariant by construction: when `s_cal_event_dates_on` is false the date string stays empty, so both render functions take their original `snprintf`/draw branches verbatim; the time, date bar, and `REMOVE_LEADING_ZERO` paths are not touched. The only unconditional changes are two buffer-size bumps (`s_event_buf` and the local meeting-bar `line[]`, 80 → 96) plus the four unused-until-enabled settings.

Files: `jy-time/package.json` (4 messageKeys; 1.26 → 1.27), `jy-time/src/pkjs/config.json` (4 Calendar items), `jy-time/src/pkjs/index.js` (`DEFAULT_SETTINGS`, `sendLayoutSetting` mapping, `customClay` visibility group), `jy-time/src/c/jy-time.c` (persist keys 274-277, four `s_cal_event_date_*` globals, `format_event_date()` + forward declaration, gated date composition in `recompute_meeting_bar()` and `calendar_draw_overlay()`, inbox handlers, boot-load).

Builds clean for emery (49 KB RAM footprint, 82 KB free heap; same pre-existing warnings). A clean rebuild was needed so the SDK regenerated `message_keys.auto.h` from the new package.json keys. Verified by clean compile, defaults aligning across C statics / config.json / `DEFAULT_SETTINGS`, and a logic trace of the on/off branches. Emulator can't run here (`qemu-pebble` missing `libsndio.so.7`); on-device confirmation pending after upload: bar width with the date prefix at a worst-case long title, and date legibility on the shake countdown line. Artifact: `release-assets/simple-pixel-style-1.27.0-emery.pbw` (also in `jy-time/release-assets/`).

**Also folded into 1.27 — Clay menu reorganization** (user request: group like-features, keep high-priority settings higher). Pure reorder of `config.json`, no behavior/default/messageKey changes. The 21-item "Layout" grab-bag was split into a new **Time** section (font, CASIO phantom, military time, leading zero) placed high, a slimmed **Layout** (top steps, the three complications, verbose weather, vibrate-on-disconnect), and a dedicated **Fitness pip row** section; **Multi-timezone** moved up next to Time (time-family, off by default so it collapses to one toggle). New top-level order: Theme, B&W inversions, Per-section colors, Time, Multi-timezone, Layout, Weather, Calendar, Fitness pip row, Shake, then the seven shake sub-panels (left in place — `syncShake()` shows only the one matching the selector, so reordering them is invisible). Implemented as exact text-span surgery so every control block is byte-preserved; verified by an identical 95-key messageKey set, zero `defaultValue` changes, and only "Time"/"Fitness pip row" headings added. `customClay()` visibility logic needed no change for the reorder itself (it resolves items by id/messageKey). Additionally, the **Fitness pip row sub-settings now collapse when the row is off**: `customClay` gained `FITNESS_PIP_BASE_KEYS` (shape/size/direction/style/color-source/color-distribution) hidden via a new `syncPipRow()` when `FITNESS_PIP_ROW_ON` is off, and `syncPipColorPickers()` now also returns hidden when the row is off so the three pip color pickers compose correctly (when the row is on they still follow color source / global color mode). So with the row off (the default) the section is just one toggle. The 1.27 PBW above was rebuilt to include the calendar-dates feature, the menu reorganization, and this collapse.

**Also in 1.27 — fresh-install default sync fix.** `DEFAULT_SETTINGS` in `index.js` (the per-key fallback that `readSettings()` pushes to the watch on every `ready`, i.e. every launch) had drifted out of sync with `config.json` and the firmware static initializers on **10 keys**, so a fresh / never-configured install booted the wrong look as soon as the phone connected: `TIME_FONT` (Bitham instead of CASIO), `INVERT_TOP_BAR` / `INVERT_DATE_BAR` / `INVERT_TIME` (off instead of on), `SHAKE_BEHAVIOR` (off instead of fitness rings), `FITNESS_PIP_ROW_ON` (off instead of on), `FITNESS_PIP_COLOR_SOURCE` (theme instead of color), `VIBRATE_ON_DISCONNECT` (off instead of on), and the pip gradient `FITNESS_PIP_COLOR_MID` / `FITNESS_PIP_COLOR_HIGH` (yellow/green instead of the intended mustardy-orange / dark-green). For every one, `config.json` and the C initializer agreed and `DEFAULT_SETTINGS` was the lone stale value, so it was aligned to them. Users who have opened Clay and saved are unaffected (saved settings always win in `readSettings()`); only fresh / never-configured installs change, and they change *to* the intended look. Verified by an automated three-way default audit: all 95 shared keys now agree across `config.json`, firmware, and `DEFAULT_SETTINGS`.

## 1.28: Japanese date language (2026-06-03)

Optional Japanese rendering for the date line (day-of-week / month / day), requested by a Japanese user. New **"Date language"** select in Settings → Time: English (default, unchanged), 日本語 full (`6月2日 火曜日`), or 日本語 compact (`6月2日（火）`).

Key realization from a user-supplied on-watch screenshot: this PebbleOS build already renders arbitrary CJK — Japanese calendar event titles (古川総…, プラ容器, 生ゴミ, 缶・ビン) display cleanly on the meeting bar and the Upcoming shake overlay. The event bar uses `FONT_KEY_GOTHIC_18_BOLD` and the date bar uses `FONT_KEY_GOTHIC_24_BOLD` — both Pebble system Gothic fonts — so the firmware CJK fallback that draws the event kanji also draws date kanji. That means **no bundled/subsetted font, no resource, no layout change**: just format the date string with kanji (numbers + 月/日 + a 7-kanji weekday lookup) and let the existing date font fall back. The Japanese string is fewer glyphs than "Tuesday, June 2", so it fits the existing date slot (no need for the date-band vertical room, which stays available if the kanji ever want to be sized up).

English is byte-identical (the original formatter just moved into the `else`); only the date string changes, and only when a Japanese option is selected. Plumbing: `DATE_LANGUAGE` message key, the Clay select, `DEFAULT_SETTINGS: 'en'`, C `s_date_language` (0=EN / 1=JP full / 2=JP compact), persist key 278, an inbox handler that re-renders the date immediately, and boot-load. Three-way default audit still clean (96 keys).

Shipped without local on-device verification, by the user's call ("let er rip") — the emulator can't run here and kanji rendering depends on the device firmware fallback, so the Japanese user confirms or reports. If the full form clips at the widest dates (e.g. 12月31日 水曜日), the compact option is already there. Artifact: `release-assets/simple-pixel-style-1.28.0-emery.pbw` (also in `jy-time/release-assets/`).

## 1.31: Invert pip bar background, Tuxedo (2026-06-08)

New **"Invert pip bar background (Tuxedo only)"** toggle in Settings → Fitness pip row (off by default). Under the Tuxedo (B&W) theme it fills the pip strip with the date-bar foreground color and flips the pips to the date-bar background color, so the row reads as an inverted stripe inside the date bar. No effect under the Poor & Irish color theme.

The fill is sized to the exact pip bounding box, full screen width: small pips → `y=33..37` (h=5), large pips → `y=31..39` (h=9), matched off `s_fitness_pip_size`. Top of the bar is the topmost pip pixel, bottom is the bottommost. Gating is `s_fitness_pip_invert_bar && s_color_mode != ColorModeColor`; the pip color only flips in the theme-mode (`!use_color`) branch, so the "Always color" pip pickers are untouched.

Plumbing: `FITNESS_PIP_INVERT_BAR` message key, persist key 279, C `s_fitness_pip_invert_bar` (default false), inbox handler + boot-load, the Clay toggle (hidden when the pip row is off, via `FITNESS_PIP_BASE_KEYS`), `DEFAULT_SETTINGS: false`, and the `sendLayoutSetting` line. Three-way default agrees for the new key (config.json / firmware static / `DEFAULT_SETTINGS` all false). The rendering (two changes in `draw_fitness_pip_row`) was handed to Codex against an exact spec; the setting plumbing was written directly.

Shipped without local on-device verification — the emulator can't run here, so the look is confirmed by uploading the PBW. Artifact: `release-assets/simple-pixel-style-1.31.0-emery.pbw`.

## 1.32: Pip-bar invert moved to Inversions menu, +2px (2026-06-08)

Follow-up to 1.31. The **"Invert pip bar"** toggle moved out of the Fitness pip row section into the **Black & White inversions** section, positioned between "Invert top bar" and "Invert date bar" (it sits there physically on the face too). It now hides under the Poor & Irish color theme like its sibling invert toggles (added to `BW_ITEM_KEYS`, removed from `FITNESS_PIP_BASE_KEYS`), and is no longer gated by the pip-row collapse. Relabeled from "Invert pip bar background (Tuxedo only)" to plain "Invert pip bar" to match the section's voice.

Rendering: the swapped band now extends 2px below the pips — small `y=33..39` (h=7), large `y=31..41` (h=11) — so the inverted stripe carries into the gap above the date instead of stopping exactly at the pip bottoms. One value in `draw_fitness_pip_row` (`bar_h` 9/5 → 11/7); no other geometry changed. No C plumbing change (same message/persist key, same static).

Artifact: `release-assets/simple-pixel-style-1.32.0-emery.pbw`.

## 1.33: Date line nudged down 4px under Casio (2026-06-08)

When the time font is Casio (the default), the weekday + month/date line shifts down 4px so it reads a touch closer to the time row. One offset in the main render: `date_frame` Y gets `+4` when `s_time_font == TIME_FONT_CASIO` (non-Casio fonts unchanged). Only the date text frame moves; the date-bar background band (`y=31..time_frame_y`) is unchanged, so inversions and the pip strip are unaffected.

Artifact: `release-assets/simple-pixel-style-1.33.0-emery.pbw`.

## 1.34: Pip bar inverts relative to the theme, like every other section (2026-06-08)

Fixes the inconsistency from 1.31/1.32: the pip bar was flipping relative to the **date bar** (it borrowed `ColorSectionDateBar`'s resolved colors via `set_draw_section`), so on an inverted (white) date bar "Invert pip bar ON" produced a *dark* band — the opposite direction of every other invert toggle, which flip relative to the **global theme**.

Now the pip bar is its own theme-relative section, identical rule to top/date/time/weather/meeting: not inverted = matches the base theme (dark in dark mode, light in light mode), inverted = the opposite. `draw_fitness_pip_row` computes `pip_band_bg`/`pip_theme_fg` from `theme_fg_color()`/`theme_bg_color()` and the pip's own flag (not the date bar's), and always paints the band in Tuxedo so it carries its own theme color (it blends when it matches the date bar). All six invert toggles already live in one "Black & White inversions" Clay section (1.32).

Default flipped `false → true` (all three sync points: C static, config.json, `DEFAULT_SETTINGS`) to match the date bar's default-inverted state, so the band blends out of the box and the shipped look is unchanged. To make the pip strip stand out as a dark band on the default white date bar, turn **Invert pip bar OFF** (OFF = dark, matching the base theme). No effect under the color theme.

Artifact: `release-assets/simple-pixel-style-1.34.0-emery.pbw`.

## 1.35: French date language + larger battery number (2026-06-11)

Two additive options. (The watch-icon fit that makes the larger battery number read cleanly is its own release, 1.36, below.)

### French date language

Third option on the **Date language** select (Settings → Time), extending the 1.28 Japanese feature: English (default) / 日本語 full / 日本語 compact / **Français (`mardi 2 juin`)**. (This was written in a prior session but left uncommitted and undocumented; verified, built, and shipped in 1.35.)

Simpler than the Japanese path: where Japanese leans on the firmware CJK fallback, French accented Latin (`février`, `août`, `décembre`) is already carried by the system Gothic font the date bar uses (`FONT_KEY_GOTHIC_24_BOLD`), so no bundled font, resource, or layout change. New `s_date_language == 3` branch in `update_time_date` with `fr_days[]` / `fr_months[]` tables, formatted `"%s %d %s"`. The inbox-handler valid-range check widened `<= 2` → `<= 3`. `s_date_buf[32]` holds the longest French string (`mercredi 28 septembre`, ~22 bytes incl. null) with room to spare. English output is byte-identical (unchanged `else` branch). No new message key (reuses `DATE_LANGUAGE`); `index.js` adds the `'fr' → 3` mapping, `'en'` default unchanged.

### Larger battery number

New **"Larger battery number"** toggle (Settings → Layout, under the top step-counter toggle; default off). When on, the always-on top-left watch battery % renders in `FONT_KEY_GOTHIC_24_BOLD` instead of the default `s_font_top` (`FONT_KEY_GOTHIC_18_BOLD`). Targets the top-bar watch battery readout specifically, not the optional battery complications in the circle row.

Plumbing mirrors the `MILITARY_TIME` toggle exactly: `BATTERY_NUMBER_LARGE` message key, persist key 280, C `s_battery_number_large` (default false), inbox handler, boot-load, the Clay toggle, `DEFAULT_SETTINGS: false`, and the `sendLayoutSetting` mapping. `draw_watch_battery` picks the font off the flag; the small case is byte-identical (same font, icon at `origin.y = 5`). A clean rebuild was required so the SDK regenerated `message_keys.auto.h` from the new key (the first incremental build failed with `MESSAGE_KEY_BATTERY_NUMBER_LARGE` undeclared).

### Files changed

- `jy-time/package.json` / `package-lock.json` — version 1.34 → 1.35; added `BATTERY_NUMBER_LARGE` to messageKeys (French reuses the existing `DATE_LANGUAGE` key).
- `jy-time/src/pkjs/config.json` — French dropdown option + description; "Larger battery number" toggle.
- `jy-time/src/pkjs/index.js` — `'fr' → 3` date mapping; `BATTERY_NUMBER_LARGE` default + mapping.
- `jy-time/src/c/jy-time.c` — French `s_date_language == 3` branch + widened range check; battery persist key 280, static, `draw_watch_battery` font branch, inbox handler, boot-load.

### Verification

- `pebble build` succeeds clean (clean rebuild for the new message key); embedded `versionLabel 1.35` confirmed in the bundle. Only the three pre-existing warnings (`forecast_x_for_time` unused, two `snprintf` format-truncation notes, linker RWX) carry over.
- Emulator can't run here (`qemu-pebble` missing `libsndio.so.7`) — build-only. On-device confirmation pending via upload: the French date line renders.
- Committed locally `16b1104`; not pushed. Staged artifact: `release-assets/simple-pixel-style-1.35.0-emery.pbw` (default-off).

## 1.36: Larger battery number — watch icon fit (2026-06-11)

Follow-up to the 1.35 "Larger battery number" toggle: with the large number on, the small watch icon next to it (fixed `y=5`, ~8×16 px, tuned for the 18 px number) looked undersized and sat too high. 1.36 draws a dedicated larger icon in the large case only.

Codex implemented per the handoff (`Plan To Implement - Large Battery Icon Fit.md`): a new `draw_watch_icon_large_c` (11×21 px, same 1px line/lug/body language), called only in `draw_watch_battery`'s large branch at `origin = (7 + battery_size.w, 7)`. The shared `draw_watch_icon_c` (also used by the battery complication via `draw_watch_icon_centered`) is untouched, and the small case stays byte-identical. Reviewed for scope (clean — only the new fn + the branch) and rebuilt clean (no new warnings). Icon bounds y=8..28, under the 31 px top-bar ceiling.

**Open eyeball — battery at 100%.** Clearance from the step-counter bar (x=56) depends on the unverified Gothic-24 width of "100": icon right edge = `19 + battery_size.w`, so it clears only if "100" renders under ~37 px (estimated ~36, but near the frame's 42 px cap). 2-digit values clear comfortably. Check 100% with the step counter on after upload; if it touches, a 1-2 px nudge fixes it. Minor cosmetic: the large icon has a two-segment (L) hand vs the small icon's single-line hand (the handoff said "two hands") — large mode only.

### Files changed

- `jy-time/src/c/jy-time.c` — new `draw_watch_icon_large_c`; large/small icon branch in `draw_watch_battery`.
- `jy-time/package.json` / `package-lock.json` — version 1.35 → 1.36.

### Verification

- `pebble build` succeeds clean; embedded `versionLabel 1.36` confirmed. Only the three pre-existing warnings carry over.
- Committed locally after review; not pushed. Staged artifact: `release-assets/simple-pixel-style-1.36.0-emery.pbw`. Emulator can't run here, so the icon fit (and the 100% case above) confirm on upload.

## 1.37: Heart-rate battery drain fix (2026-06-12)

User report: ~15%/day battery drain on Simple Pixel Style that stopped when switching to a different watchface. Root-caused to the watchface forcing the optical heart-rate sensor on far more than needed.

Two bugs in the HR sample-period handling:
- **Unconditional 60s forcing (every user).** `init()` called `health_service_set_heart_rate_sample_period(60)` at every launch regardless of whether any heart-rate feature was in use, pinning the sensor to a reading every 60s, 24/7. Firmware default (period 0) samples adaptively, roughly every ~10 min at rest. Each forced reading also woke the app (`health_handler` → mark dirty), which is the "Heavy app" tag. The customer's swap test confirms it: switching watchfaces runs `deinit()`'s `set_heart_rate_sample_period(0)`, releasing the sensor and halting the drain.
- **"Heart rate big" shake stuck at 15s.** `shake_show_overlay()` dropped the period to 15s for the big-BPM overlay but `shake_hide_overlay()` never restored it, so one shake pinned 15s sampling permanently until the app reloaded (4x worse than the 60s baseline).

Fix: new `update_hr_sample_period()` makes sampling demand-based. Period = 0 (sensor released, firmware adaptive) unless the Heart Rate complication is selected in a slot (then 600s, ~10 min resting refresh for the glanceable BPM), and 10s only while the big-BPM shake overlay is actually on screen. Called from `init()` (after `load_persisted()`), the complication inbox handler, and `shake_show_overlay()` / `shake_hide_overlay()` (the latter restoring the resting period, fixing the stuck-15s leak). `deinit()` still releases to 0 on unload. Users with no HR feature now run the sensor at the firmware's light default instead of a forced 60s.

Ruled out as causes: the fitness pip bar and fitness rings read steps/active/calories (motion data, no optical-sensor power); the main tick is `MINUTE_UNIT`; phone-side weather/calendar/prices timers cost Bluetooth (3.3% in the report), not watch CPU. The user's battery screen also showed system Health "heart rate during activities" on, which is a watch setting separate from the watchface and adds HR cost on top of these bugs.

### Files changed

- `jy-time/src/c/jy-time.c` — new `update_hr_sample_period()`; init / complication inbox / shake-show / shake-hide call it; removed the unconditional 60s and the stuck 15s.
- `jy-time/package.json` / `package-lock.json` — version 1.36 → 1.37.

### Verification

- `pebble build` succeeds clean; `versionLabel 1.37` confirmed. Only the three pre-existing warnings carry over.
- Committed locally; not pushed. Staged artifact: `release-assets/simple-pixel-style-1.37.0-emery.pbw`. Actual battery improvement can't be measured here (emulator can't run; no device); confirm on the reporting user's watch after upload.

## 1.38: Distance units (metric/imperial) (2026-06-15)

User report: the daily distance always renders in km even when the watch's system units are set to imperial. `format_distance_km()` hard-coded `"%d.%02d km"`, so both distance surfaces (the Distance today complication and the fitness-rings "Distance while active" row) were metric-only with no control.

Fix: new **"Distance units"** select (Settings, in the complications section under the three complication slots; default "Match watch setting"), with three values: Auto / Metric (km) / Imperial (mi). Auto resolves at draw time via `health_service_get_measurement_system_for_display(HealthMetricWalkedDistanceMeters)`, so it follows the watch's own measurement-system preference (Imperial gives miles). Metric and Imperial force the unit regardless, which also covers the case where the API returns `MeasurementSystemUnknown`. `format_distance_km` was renamed `format_distance` and now branches: imperial converts meters to miles at two decimals (1 mi = 1609.344 m, rounded), metric keeps the existing km path byte-identical. The measurement-system lookup is a cached setting read, not an optical-sensor activation, so it has no battery cost (unrelated to the 1.37 HR work).

Plumbing mirrors the `BATTERY_NUMBER_LARGE` toggle: `DISTANCE_UNITS` message key, persist key 281, C `s_distance_units` int (0 auto / 1 metric / 2 imperial, default 0), inbox handler (validates 0..2, persists, `mark_face_dirty()` plus `fitness_settings_changed` for the live overlay), boot-load, the Clay select, `DEFAULT_SETTINGS: 'auto'`, and the `distanceUnitsId()` mapping in `sendLayoutSetting`. As with 1.35, a clean rebuild was required so the SDK regenerated `message_keys.auto.h` from the new key (the first incremental build failed with `MESSAGE_KEY_DISTANCE_UNITS` undeclared).

### Files changed

- `jy-time/src/c/jy-time.c` — persist key 281; `s_distance_units`; `distance_use_imperial()` helper + `format_distance` (renamed from `format_distance_km`, both call sites updated); inbox handler; boot-load.
- `jy-time/src/pkjs/config.json` — "Distance units" select.
- `jy-time/src/pkjs/index.js` — `DISTANCE_UNITS` default, `distanceUnitsId()`, `sendLayoutSetting` mapping.
- `jy-time/package.json` / `package-lock.json` — version 1.37 → 1.38; added `DISTANCE_UNITS` to messageKeys.

### Verification

- `pebble clean && pebble build` succeeds; `versionLabel 1.38` confirmed. Only the three pre-existing warnings carry over (no new ones). Staged: `release-assets/simple-pixel-style-1.38.0-emery.pbw`.
- Auto-path rendering (km vs mi based on the watch setting) can't be measured here (emulator can't run; no device). Confirm on the reporting user's watch after upload: with the watch on imperial and Distance units on "Match watch setting", the distance should read e.g. `1.34 mi`.

## Outstanding / parked (as of 1.38, 2026-06-15)

Current live to-dos. (The "Next actions you take" section higher up is historical 0.1.0-era and no longer current; this is the up-to-date list.)

- **1.35 + 1.36 + 1.37 committed locally, not pushed; 1.38 built but not committed.** French date + larger battery number (1.35, `16b1104`), watch-icon fit (1.36), and the HR battery-drain fix (1.37) are committed on `main` but not pushed. Distance units (1.38) is built and staged but not yet committed. PBWs for all four staged in `release-assets/`. Push + upload `1.35.0` / `1.36.0` / `1.37.0` / `1.38.0` when ready.
- **HR battery fix (1.37), confirm with the reporting user.** Drain fix can't be measured locally (no device). After they upload 1.37, confirm the drain drops. Update (2026-06-15): the reporting user confirmed they were using the **"Heart rate big"** shake behavior. That is exactly bug #2 in 1.37, the stuck-15s leak (one shake pinned 15s HR sampling permanently because `shake_hide_overlay()` never restored the period), which is the more severe of the two 1.37 bugs. So their drain should drop noticeably once they are on 1.37, where the overlay now releases the sensor (period back to 0) after the ~3s window. One factor still outside the watchface: their battery screen showed system Health "heart rate during activities" on, which adds HR cost regardless of the face. Still worth confirming which complication slots they run (a Heart Rate complication holds the sensor at a ~10-min refresh).
- **Large battery icon at 100% — eyeball on upload.** The 1.36 large icon clears the step bar (x=56) only if "100" renders under ~37 px in Gothic 24 Bold (unverified; emulator down). 2-digit values clear. If it touches at full charge with the step counter on, a 1-2 px nudge in `draw_watch_icon_large_c` or its call fixes it.
- **French date — on-device confirmation pending.** Like Japanese (1.28), 1.35's French option shipped without local verification (emulator can't run here). Confirm `mardi 2 juin` renders; the accented months (`février`, `août`, `décembre`) rely on the system Gothic font carrying Latin-1.
- **Japanese date — on-device confirmation pending.** 1.28 shipped without local verification (emulator can't run here; kanji rely on the device firmware CJK fallback). The Japanese user is to confirm the date line renders and that the full form doesn't clip at the widest dates (e.g. `12月31日 水曜日`); the compact `日本語 compact` option is the built-in fallback if it does.
- **Clay menu declutters (5, deferred).** Same `customClay` show/hide pattern as the pip-row collapse — hide controls that have no effect in the current state: (1) CASIO phantom backdrop → only when Time font = CASIO; (2) collapse the Calendar section (ICS URLs, look-ahead, no-events label, event-date options) when "Enable bottom event" is off; (3) manual latitude/longitude → only when Location = Manual; (4) Verbose weather style → only when Verbose weather is on; (5) Your Day → rolling-window hours in Rolling mode, fixed start/end in Fixed mode.
- **Shake "Upcoming" list time in Japanese (optional).** Each row's time (e.g. `8:30am`) is phone-formatted in `formatEvent()` (`src/pkjs/index.js`); the 1.28 date-language feature covers the main date line only. Extending Japanese to the shake list's per-event time would be a phone-side formatter change.

## 1.40 packaged 2026-07-15: Pebble Time Round 2 (gabbro) support

Codex's round implementation reviewed and accepted: per-platform layout constant blocks + `overlay_safe_frame()` chord helper, emery `#else` branch byte-identical to 1.39, all ten shake overlays made round-safe, only `jy-time/src/c/jy-time.c` touched (+322 lines). Clean build both platforms, only the three pre-existing warnings, gabbro free heap ~80K. Emery/gabbro emulator screenshots verified in `round-preview/` (emery matches shipped 1.39; gabbro renders everything inside the circle). PBW staged at `release-assets/simple-pixel-style-1.40.0-emery-gabbro.pbw`.

## Round support handoff notes (2026-07-15, superseded by the 1.40 release above)

Adding the face to the Pebble Time Round 2. Split per the usual scoping: Claude = plumbing, Codex = round layout design.

- Plumbing DONE (working tree, not committed): `jy-time/package.json` targetPlatforms now emery + gabbro; `SCREEN_W`/`SCREEN_H` (~line 225 of `jy-time.c`) now derive from `PBL_DISPLAY_WIDTH`/`PBL_DISPLAY_HEIGHT`. Both platforms build green on SDK 4.9.169; gabbro (260x260 round, 128K RAM) reports ~80K free heap. Version stays 1.39 until the round work ships.
- Codex handoff doc: `CODEX_HANDOFF_jytime_gabbro_round.md` (this folder). Codex owns the round band re-map behind `#if defined(PBL_ROUND)` with a hard 13-item element-visibility checklist (never-remove-features rule baked in); emery must stay pixel-identical; screenshots to `round-preview/`; chalk explicitly out of scope (64K RAM class).
- Sibling: US Tidemaps gets gabbro + chalk, handoff doc at `Project - US Tidemaps/CODEX_HANDOFF_round_support.md`. Visual-sync rule applies once both round layouts exist.
- After Codex returns: Claude reviews diff (correctness + scope), then release packaging.

## CASIO dark-mode drop shadow legibility, Codex three-variant handoff (2026-07-21)

User report: with the CASIO time style in dark mode, the drop-shadow treatment behind the digits is illegible (the phantom "88:88" backdrop renders GColorDarkGray under white digits on black and reads as a smeared shadow). Direction confirmed by the user: KEEP the shadow effect and make the time legible, do not remove it (removal already exists as the phantom toggle-off state). Codex is to submit three genuinely distinct best-effort treatments as screenshots; user picks the winner. Fallback if all three miss: survey open-source Pebble faces for a shadow style to copy.

- Checked first (user thought the request may have gone to OpenClaw): no prior commit, handoff doc, status entry, or OpenClaw workspace trace of this request existed. Fresh work as of today.
- Codex handoff doc: `CODEX_HANDOFF_casio_dark_phantom.md` (this folder). Visual exploration pass, NOT a ship: three variants, each edit confined to `draw_casio_time_row_at` / `casio_phantom_color`, dark palette only (light mode pixel-identical), build both platforms, emery emulator screenshot per variant, exact patch saved per variant, then file reverted to baseline. Deliverables land in `casio-dark-variants/` (variant-{a,b,c}.png + .patch). Emulator boots straight into the repro state (defaults: dark mode, CASIO font, phantom on), so no scaffolding. No version bump, no commit.
- Candidate directions seeded in the doc: true offset drop shadow of the time string itself; black separation halo cutting the digits out of the 88:88 backdrop; near-black tinted backdrop tones (no neutral gray darker than DarkGray exists in the 64-color palette).
- Outcome: Codex delivered all three variants (screenshots + patches in `casio-dark-variants/`). Josh picked TWO: Variant B (phantom + crisp halo) ships as default, Variant A (true offset shadow) ships as a Clay-selectable alternative, Variant C (Oxford Blue) rejected. Codex wrote the production spec back as `CLAUDE_HANDOFF_casio_dark_shadow_styles.md`; implemented by Claude as 1.41 below.

## 1.41: CASIO dark shadow styles (halo default + offset option) (2026-07-21)

Implements the approved two-variant decision. Behavior matrix: effect off = clean foreground-only time (any palette); light sections = classic LightGray 88:88 backdrop, pixel-identical to 1.40; dark sections + "halo" (default) = DarkGray 88:88 backdrop plus a 1 px black moat cut around the lit digits (Variant B); dark sections + "offset" = no backdrop, the time's own shadow in DarkGray at +2/+2 px (Variant A). "Dark/light" means `section_uses_light_palette(s_draw_section)`, so inverted time bands behave correctly; `casio_phantom_color()` unchanged.

Plumbing follows the DISTANCE_UNITS pattern: `CasioDarkShadowStyle` enum (halo 0 / offset 1), C static default halo, persist key **282** (Codex's doc suggested 238 but that is `WEATHER_SUMMARY_COMPACT`; 282 is the true next-free after DISTANCE_UNITS 281), inbox handler sanitizing to halo for any value other than 1, boot-load with same sanitize, `CASIO_DARK_SHADOW_STYLE` message key **appended at the end** of package.json messageKeys (id 10315; append-only proven by diff, no existing key renumbered), `DEFAULT_SETTINGS: 'halo'` + `casioDarkShadowStyleId()` + sendLayoutSetting in index.js. Three-place defaults audit passes (C halo / config.json "halo" / DEFAULT_SETTINGS 'halo').

Clay: CASIO_PHANTOM relabeled "CASIO shadow/backdrop effect" (still the master off switch, description updated), new "Dark CASIO shadow style" select below it (halo/offset), hidden via `customClay` unless Time font = CASIO AND the effect toggle is on (same show/hide pattern as pip row + calendar dependents; also knocks out item 1 of the parked Clay-declutter list for the new select). TIME_FONT description no longer promises only an 88:88 backdrop.

### Files changed

- `jy-time/src/c/jy-time.c` (+60): enum + static; persist define 282; three-branch shadow block in `draw_casio_time_row_at` (light 88:88 / dark offset / dark halo, real-time pass untouched); inbox handler; boot-load.
- `jy-time/src/pkjs/config.json` (+23): phantom toggle relabel + description; new style select; TIME_FONT description.
- `jy-time/src/pkjs/index.js` (+27): DEFAULT_SETTINGS, converter, sendLayoutSetting, customClay visibility sync.
- `jy-time/package.json` / `package-lock.json`: 1.40 to 1.41; messageKey appended last.

### Verification

- `pebble clean && pebble build` green for emery AND gabbro (clean rebuild required for the new message key). Warning set identical to 1.40: the two format-truncation sites, the unused `forecast_x_for_time`, and the linker RWX notice per platform. Nothing new. Emery free heap 80,098 bytes (unchanged ~80K; halo adds draw calls, no RAM).
- 1.41 PBW appinfo verified: versionLabel 1.41, targetPlatforms emery+gabbro, new appKey present, staged at `release-assets/simple-pixel-style-1.41.0-emery-gabbro.pbw`. (No 1.40 PBW existed in release-assets to overwrite.)
- Emulator steps from the Codex doc (dark-palette screenshots vs variant PNGs, Clay persistence toggling, light-regression capture) cannot run here (qemu-pebble libsndio failure, standing limitation). On-watch checks for Josh after upload: default dark CASIO time should match `casio-dark-variants/variant-b.png`; switching the new select to "True offset shadow" should match `variant-a.png` and survive a restart; light/inverted time band should look exactly like 1.40; effect toggle off = clean digits under both styles.
- Not committed, not pushed, per the handoff. Working-tree deletions + package-lock user state preserved.
- **SUPERSEDED same day, never shipped.** Josh reviewed a fourth Codex exploration (Variant D, hollow gray outline) and replaced filled Variant B as the default before 1.41 was committed or uploaded. The staged 1.41 PBW in `release-assets/` is dead; 1.42 below is the shipping release. All 1.41 plumbing (keys, persist 282, enum, Clay select, pkjs) carries into 1.42 unchanged.

## 1.42: CASIO dark default becomes hollow gray outline (Variant D) (2026-07-21)

Final form of the dark CASIO shadow work, per Codex's `CLAUDE_HANDOFF_casio_hollow_outline_final.md` (supersedes the B-default doc) with Josh's overrides: version 1.42 instead of the doc's 1.41, plus document + commit + push (the doc's no-commit guard was Codex's default, overridden by Josh directly).

Only the value-0/default dark rendering and Clay copy changed vs the 1.41 working tree. Behavior matrix now: effect off = clean foreground time; light sections = filled LightGray 88:88, pixel-identical to 1.40; dark + "halo"/0 (default) = **Variant D hollow outline**: "88:88" drawn in DarkGray at the four cardinal 1 px offsets, then punched back to black at center (thin gray perimeter only), then the real time's 8-neighbor black moat, then white foreground time; dark + "offset"/1 = Variant A unchanged (time's own DarkGray shadow at +2/+2). The four-offset outline is deliberate (sparse pixel density for backlight readability); internal enum name `CASIO_DARK_SHADOW_HALO` and Clay value `'halo'` intentionally kept to avoid migration risk, only rendering + label changed. No blue/tinted colors anywhere (Variant C direction fully rejected).

### Files changed (vs 1.40 HEAD, includes carried 1.41 plumbing)

- `jy-time/src/c/jy-time.c`: applied `casio-dark-variants/variant-d-hollow-outline.patch` verbatim (apply-check passed; +12/-4 over the 1.41 tree) on top of the 1.41 enum/persist/inbox/boot/render work.
- `jy-time/src/pkjs/config.json`: first option label now "Hollow gray 88:88 (default)", select description per the handoff. Offset option and visibility logic untouched.
- `jy-time/package.json` / `package-lock.json`: 1.41 to 1.42 (lockfile both project-version fields).
- `jy-time/src/pkjs/index.js`: unchanged this delta (1.41 plumbing only).

### Verification

- `pebble clean && pebble build` green for emery AND gabbro. Warning set identical to 1.40/1.41 (two format-truncation sites + unused `forecast_x_for_time`, line numbers shifted by the patch; linker RWX per platform). Emery free heap 80,010 bytes.
- PBW staged: `release-assets/simple-pixel-style-1.42.0-emery-gabbro.pbw`; appinfo verified (versionLabel 1.42, emery+gabbro, `CASIO_DARK_SHADOW_STYLE` = 10315 unchanged).
- Emulator visual pass not runnable here (standing qemu limitation); Codex's variant-d screenshot is the reference. On-watch after upload: dark CASIO default should match `casio-dark-variants/variant-d-hollow-outline.png` (hollow gray outlines, no blue), offset option should match `variant-a.png` and persist across restart, light/inverted band identical to 1.40, effect off = clean digits.
- Committed and pushed to `main` per Josh's instruction (release commit, source files + this status doc; PBWs remain gitignored).

## 1.43: CASIO dark default becomes "Stippled 88:88" always-on ghost (2026-07-25)

Third dark CASIO shadow style `CASIO_DARK_SHADOW_STIPPLE` (value 2), now the fresh-install default in all three default sites (C static initializer, config.json `defaultValue: "stipple"`, `DEFAULT_SETTINGS: 'stipple'`). Halo and offset rendering byte-identical and still selectable ("True offset shadow", "Hollow gray 88:88"); saved Clay choices preserved by the existing settings flow; unknown decode values still fall back to HALO at both decode sites (inbox + boot persist).

Rendering (color platforms only via `#if defined(PBL_COLOR)`; gabbro falls through to hollow because a 1-bit framebuffer has no sentinel color): "88:88" drawn in sentinel GColorShockingPink into the standard time frame, framebuffer captured, sentinel pixels rewritten to a 50% checkerboard of `GColorLightGrayARGB8` with the rest restored to `draw_bg_color()`, released before the solid foreground time draws on top. Planned tuning if too faint on-wrist: LightGray to White at the single assignment site (no config knob).

Implemented by Codex per the in-chat handoff; Claude reviewed the diff against spec. Two cosmetic wording drifts from the exact spec: C comments (content-identical, kept) and the Clay select description (restored to spec text by Claude). All functional hunks landed verbatim, including the offset-closes-into-stipple `#if` bracing.

### Files changed (vs 1.42 HEAD)

- `jy-time/src/c/jy-time.c`: enum value + STIPPLE static default, three-way decode at both sites (~5374 inbox, ~6439 boot), stipple branch in `draw_casio_time_row_at` (~1470).
- `jy-time/src/pkjs/config.json`: select gains "Stippled 88:88 (default)" first, defaultValue "stipple", description covers all three styles.
- `jy-time/src/pkjs/index.js`: `DEFAULT_SETTINGS` stipple + `casioDarkShadowStyleId` maps stipple to 2. Visibility logic untouched.
- `jy-time/package.json` / `package-lock.json`: 1.42 to 1.43 (Claude; lockfile both project-version fields).

### Verification

- `pebble clean && pebble build` green for emery AND gabbro. Warning set identical to 1.42 baseline (format-truncation at 3025/3897, unused `forecast_x_for_time`, linker RWX per platform); zero new warnings. Emery free heap 79,754 bytes (80,010 at 1.42; stipple branch costs ~256 bytes of code).
- Sanity greps: `CASIO_DARK_SHADOW_STIPPLE` x5, `ShockingPink` x2 in jy-time.c.
- PBW staged: `release-assets/simple-pixel-style-1.43.0-emery-gabbro.pbw`; appinfo verified (versionLabel 1.43, emery+gabbro, `CASIO_DARK_SHADOW_STYLE` = 10315 unchanged).
- On-watch after upload: dark CASIO default = full "88:88" ghost as a dim gray checkerboard behind solid digits, always visible; offset and hollow selectable and pixel-identical to 1.42; light band identical; effect off = clean digits.
- NOT committed. Release commit + push happen after on-wrist sign-off (an ink tweak would belong in the same release commit).

### Revision landed (2026-07-25, Codex handoff 2, reviewed and restaged)

On-wrist feedback round: stipple density dropped from the 50% checkerboard to a 5-of-16 ordered dither (31.25%, the closest clean tile step to the requested 40% cut; keep-mask 0x0525 indexed by ((y & 3) << 2) | (x & 3), a strict subset of the old checkerboard so it only dims). The stipple style now ALSO draws the time's own 2 px offset shadow between ghost and foreground (draw order: stippled ghost, solid DarkGray shadow via the six lines duplicated verbatim from the offset branch after the framebuffer release, then white digits; the shadow is never dithered). Clay renames: "Stippled 88:88 + Offset Shadow (default)" and "True Offset Shadow Only"; enum values, Clay values, and pkjs plumbing unchanged ('stipple' stays 2), so the round-2 scope was jy-time.c + config.json only.

Round-2 review: all six hunks landed verbatim, zero deviations; offset and halo branches byte-untouched. Independent clean build green emery + gabbro, warning set identical to baseline (line numbers shifted by the new hunks), emery free heap 79,702 bytes. Sanity greps: 0x0525 x1, shadow_frame x8, ShockingPink x2, CASIO_DARK_SHADOW_STIPPLE x5. PBW restaged over `release-assets/simple-pixel-style-1.43.0-emery-gabbro.pbw` (appinfo re-verified: 1.43, emery+gabbro, key 10315). Still NOT committed; awaiting on-wrist sign-off of the revised ghost.

### Revision 2 landed (2026-07-25, Codex handoff 3, reviewed and restaged): outlined ghost segments

On-wrist verdict on the round-2 build: dither reads clumpy (the 5-of-16 Bayer tile has an off-lattice fifth dot plus a blank fourth row, so it bands) and segment breaks of the 8 dissolve into blur (sparse fill cannot articulate 1-2 px inter-segment gaps). Fix: (1) mask 0x0525 to 0x0505, a perfect 2 px lattice at 25% density; (2) layer the halo-style 4-offset DarkGray outline dilate UNDER the sentinel fill so each ghost segment keeps a 1 px solid rim (sentinel covers the glyph body, so the rim survives the dither rewrite); (3) punch a 1 px moat around the lit digits in draw_bg_color() (halo technique, bg color instead of hardcoded black) before the shadow so digit edges stay crisp. Offset shadow unchanged, drawn last before the foreground. Whole stipple branch replaced in one hunk; Clay description first sentence updated to "outlined ghost segments with a sparse even dot fill". Scope: jy-time.c + config.json.

Round-3 review: branch matches the spec line for line (verified by direct read, not just Codex's report); all seven sanity greps match (0x0505 x1, 0x0525 x0, stipple_outline_offsets x4, stipple_moat_offsets x4, shadow_frame x8, ShockingPink x2, CASIO_DARK_SHADOW_STIPPLE x5); halo/offset/light branches byte-untouched. Independent clean build green emery + gabbro, baseline warning set only, emery free heap 79,514 bytes (79,702 at round 2). PBW restaged over `release-assets/simple-pixel-style-1.43.0-emery-gabbro.pbw` (appinfo re-verified: 1.43, emery+gabbro, key 10315). Still NOT committed; awaiting on-wrist sign-off of the outlined ghost.

### Fourth style: Hollow 88:88 + Offset Shadow, new default (2026-07-25, implemented by Claude directly)

`CASIO_DARK_SHADOW_HALO_SHADOW` (value 3, Clay value 'halo_shadow'): the halo branch duplicated verbatim (outline dilate, black center punch, black moat, same local names in a sibling scope) plus the standard six shadow lines before the foreground. No framebuffer pass, so it compiles for all platforms and matches halo's existing BW behavior. Now the fresh-install default in all three default sites (C static initializer, config.json defaultValue "halo_shadow", DEFAULT_SETTINGS 'halo_shadow'); casioDarkShadowStyleId maps it to 3; both decode sites accept value 3 with unknown still falling back to HALO. Clay select order: Hollow 88:88 + Offset Shadow (default) / Stippled 88:88 + Offset Shadow / True Offset Shadow Only / Hollow gray 88:88.

Done by Claude (pure duplication of proven blocks plus wiring, no visual judgment, so no Codex round trip). Verification: HALO_SHADOW x5, shadow_frame x12, halo_shadow x2 in index.js and x2 in config.json, config.json parses as valid JSON, clean build green emery + gabbro with baseline warnings only, emery free heap 79,182 bytes. PBW restaged (appinfo: 1.43, emery+gabbro, key 10315). Note: if the watch does not show the new default after upload, previously saved Clay settings are pinning the old choice; pick "Hollow 88:88 + Offset Shadow" in the config page once.

RELEASED: commit `6d41f93` "Release simple-pixel-style 1.43" pushed to main 2026-07-25 (five source files + this status doc; PBWs remain gitignored). User uploads `release-assets/simple-pixel-style-1.43.0-emery-gabbro.pbw` to the Pebble Appstore.

## 1.44: Vibrate when a meeting starts (2026-07-29, shipped)

New Clay toggle in the Calendar section ("Vibrate when a meeting starts", default ON): the watch fires `vibes_double_pulse()` the minute a calendar event's start time arrives. No new data path; the trigger rides the existing `recompute_meeting_bar()` NOW decision over the already-persisted `s_meeting_start[]` epochs.

Trigger design: helper `maybe_fire_meeting_start_vibe(now, chosen)` called as the first statement of the `is_now` branch. Dedupe is keyed on the meeting's start epoch and persisted (`PERSIST_KEY_LAST_MEETING_VIBE_START` 284), so none of the seven `recompute_meeting_bar()` call sites (minute tick, calendar push, four event-date settings blocks, window_load at boot) can double-fire, including across restarts. A 120s freshness window keeps cold entry silent for meetings that started earlier (launching the face or a calendar refresh mid-meeting does not buzz). Back-to-back meetings buzz at each start; identical starts buzz once; a rescheduled meeting buzzes again at its new start. No `quiet_time_is_active()` gate, matching the disconnect and fitness-goal vibes. All-day events never reach the meeting slots (pkjs drops VALUE=DATE events), so no midnight buzzes.

### Files changed (vs 1.43 HEAD)

- `jy-time/package.json`: version 1.44; `VIBRATE_ON_MEETING_START` appended LAST in messageKeys (existing ordinals preserved; new key = 10316, `CASIO_DARK_SHADOW_STYLE` still 10315).
- `jy-time/src/pkjs/config.json`: toggle between "Look ahead" and "No-events label", defaultValue true.
- `jy-time/src/pkjs/index.js`: `DEFAULT_SETTINGS.VIBRATE_ON_MEETING_START: true` + 0/1 send in `sendLayoutSetting()` (runs on both ready and webviewclosed; webviewclosed handler itself untouched per the clay-settings rule).
- `jy-time/src/c/jy-time.c`: persist keys 283 (toggle bool) + 284 (dedupe int32, appended after prior max 282), statics `s_vibrate_on_meeting_start` / `s_last_meeting_vibe_start`, the helper + call site, inbox receive+persist block after VIBRATE_ON_DISCONNECT, both `load_persisted()` reads (`persist_exists`-guarded; loads run before window_load's recompute, so the dedupe value is in place before the first vibe evaluation).

### Verification

- `pebble clean && pebble build` green for emery AND gabbro (the first build failed only because waf served a stale `message_keys.auto.h`; clean fixed it). Baseline warning set only; zero new warnings.
- Emulator install stalled twice (240s timeouts, processes killed with bracketed pkill). Cap of 2 reached, so per project rule: build green + full diff review stand in for the render check. Diff reviewed clean (types, scope of `t`, non-negative window math guaranteed by `is_now`, persist write only on actual fire).
- PBW staged: `release-assets/simple-pixel-style-1.44.0-emery-gabbro.pbw`; appinfo verified (versionLabel 1.44, emery+gabbro, ordinals above).
- SHIPPED: confirmed live on-watch 2026-07-31. Release commit `Release simple-pixel-style 1.44: Vibrate when a meeting starts` made 2026-07-31 (four source files + package-lock + this status doc; PBW stays gitignored in `release-assets/`). Commit is local, not yet pushed.

## 1.45: Clay settings declutter, Batches 1 + 2 + 4 (2026-07-31)

Full scope of `Plan To Implement - Clay Settings Declutter.md` except Batch 3 (Vibration section), which was dropped on explicit instruction. All changes are config.json + the customClay visibility function in index.js; no message keys, no C changes. Hidden Clay items keep their saved values on submit, and the webviewclosed handler is untouched.

Batch 4 (copy fixes):
- WEATHER_PROVIDER label "NWS Forecast provider" renamed to "Forecast provider". Description rewritten: the old text predated 1.02 and claimed the main face and Detailed Weather always use Open-Meteo. New text states that NWS, when selected, drives temperature, summary, icon, rain chance, high/low, wind, and the Detailed Weather graph, with feels like, UV index, and sunrise/sunset staying on Open-Meteo.
- The seven shake-overlay section headings renamed with a "Shake: " prefix. Heading ids (`shake-*-heading`) unchanged, so the hide groups still resolve.

Batch 1 (feature-off rows now hide, same mechanic the overlay sections already used):
- CASIO_PHANTOM shows only when Time font = CASIO (new syncCasio; the dark-style child logic carried over unchanged).
- Calendar children (both ICS URLs, Look ahead, Vibrate when a meeting starts, No-events label, Show event dates + its date children) hide when "Enable bottom event" is off (CALENDAR_CHILD_KEYS + syncCalendarSection; syncCalEventDates now also factors calendar-on).
- Weather children (Location, Temperature unit, Refresh, Forecast provider, plus the refined rows below) hide when "Enable weather" is off (WEATHER_BASE_KEYS + syncWeatherSection). Honest gate: pkjs refreshWeather returns early on !WEATHER_ENABLED (only the tide fetch continues), so no weather setting does anything while off. This master gate was not in the written plan; added to complete the Batch 1 pattern.
- Manual latitude/longitude show only when Location = Manual coordinates (and weather on).
- Verbose weather style shows only when Verbose weather is on.
- Your Day mode fields: Rolling window hours only in Rolling mode, Fixed start/end only in Fixed mode (composed with the existing shake = your_day gate).
- Shake overlay duration hides when Shake behavior = Off.

Batch 2 (gating bug fixes):
- TIDE_VIEW_HOURS added to SHAKE_TIDE_ITEM_KEYS (was orphaned, visible headingless under every other overlay selection).
- Step-goal vibe pair (FITNESS_VIBRATE_ON_GOAL + pattern) stays always-visible but gets a new "Step goal vibration" heading (config heading id `goal-vibe-heading`) that shows only when the Shake: Fitness rings heading above it is hidden. No rows moved (Batch 3 stayed dropped).
- Fitness targets and ring colors now follow every consumer: steps target visible when shake = fitness_rings OR pip row on OR any bubble complication = fitness_rings; active/calories targets + three ring colors visible when shake = fitness_rings OR any bubble = fitness_rings (C verified: draw_complication_fitness_rings reads colors + targets; pip fill reads s_fitness_target_steps; ring on/off toggles are overlay-only and stay shake-gated).
- NWS_ZIP moved from the shake section into Weather, directly under Forecast provider (it pins the gridpoint for the provider too, and in the shake section it orphaned whenever provider = NWS but the overlay wasn't selected). Visible when weather on AND (provider = NWS OR shake = NWS Forecast). Description updated ("the manual coordinates above"). This was the plan's listed Batch 2 alternative; the row is conditional either way so there is no muscle-memory cost.

Verification: node --check green on index.js; jq validates config.json; scripted cross-check that every key referenced in the customClay groups exists in config.json as messageKey or id (none missing); NWS_ZIP appears exactly once, in Weather, after WEATHER_PROVIDER; build green emery + gabbro; PBW inspected (versionLabel 1.45, both targets, new strings present, stale strings absent); zero stale syncCasioDarkStyle references; every sync function defined once with live callers. Emulator not used (settings-page-only change; nothing on the face renders differently).

PBW staged: `release-assets/simple-pixel-style-1.45.0-emery-gabbro.pbw`. NOT committed; release commit after on-watch sign-off. To test: upload, open settings, and check that turning Calendar/Weather off folds their sections, picking a non-CASIO font hides the CASIO toggle, Your Day shows only its active mode's fields, and a "Step goal vibration" heading appears over the vibe pair when the shake overlay is not Fitness rings.

Addendum (2026-08-01), also in this 1.45 build:
- Dark CASIO shadow default flipped from `halo_shadow` to `offset` ("True Offset Shadow Only": drop shadow with no ghost 88:88) at all three default sites (C static initializer, config.json defaultValue, DEFAULT_SETTINGS), on the instruction to use CASIO with a drop shadow and no phantom 8s wherever numerals render white on black. Option labels updated ("(default)" moved to True Offset Shadow Only). Saved Clay choices are unaffected; existing installs keep whatever they picked. Verified in the PBW: `"defaultValue":"offset"` in the bundled config, labels swapped, declutter strings intact, version 1.45 emery+gabbro. Restaged.
- Screenshot re-snap run (six emery looks with pips, two gabbro incl. a color face) is BLOCKED: two emulator install attempts stalled past 240s/300s with no output (qemu boots, install handshake hangs). Hard 2-attempt cap reached; stopped per project rule. Full look matrix and per-look sed deltas saved in `Screenshot Resnap Matrix.md` for when the emulator works again; pebble-tool v5.0.39 is available (v5.0.34 installed) and upgrading is the likeliest fix, user's call.

Addendum 2 (2026-08-01, later the same day): emulator fixed, screenshots delivered, plus one more C fix in 1.45.

- EMULATOR ROOT CAUSE FOUND: SDK 4.9.169 (installed Jul 21) breaks the install handshake on this box; the pebble-tool upgrade to 5.0.39 did not help. Fix was `pebble sdk install 4.9.148 && pebble sdk activate 4.9.148` (the SDK the May screenshot sessions ran). Installs now complete in seconds on both emery and gabbro. The 1.44-era stalls and the 2026-07-15 gabbro weirdness were the same rot. Memory updated (feedback_pebble_emulator_install).
- COLOR-MODE PHANTOM FIX (in 1.45, C change): `section_uses_light_palette` returned light/dark from `LIGHT_MODE != inverted`, ignoring that Color mode does not use the inversion toggles. Result: a black or dark picked background in Color mode took the light branch and the CASIO time drew its faded gray "88:88" backdrop over black (illegible, reported on the round face; affected emery identically). Now, in Color mode only, light/dark derives from the picked background's 2-bit-channel luminance (sum >= 5 of 9 = light). BW mode behavior is byte-identical. Bitmap themes and contrast helpers inherit the same correction.
- Canonical look configs pinned by pixel-sampling the old snaps: DARK looks = LIGHT_MODE off with the top/date/time inversion trio OFF (all-black bars); LIGHT looks = LIGHT_MODE on with the trio ON (black bars on light). The DEFAULT_SETTINGS trio defaults to true, so dark re-snaps need the three inversion-off overrides. First re-snap pass had this wrong (white bars); redone.
- Delivered re-snaps (SDK 4.9.148, build with all 1.45 changes): `Current Screenshots/` current-dark-default-3-circles, current-dark-large-verbose-weather, current-dark-small-verbose-weather, current-light-inverted-bars-default-3-circles, current-light-large-verbose-weather, current-light-small-verbose-weather (all emery, FuelBand pips on, dark looks on the offset shadow). `round-preview/` gabbro-dark-default-pips.png, gabbro-color-sections.png (palette A: teal/cream/navy/green), gabbro-color-sections-b.png (palette B: teal/gray/black/blue/red), and gabbro-watchface-loop.gif (6 frames at 1.2s: dark circles, dark verbose, light with inverted weather+meeting boxes, light clean, color A, color B; pips on every frame; palettes sampled from the old design-mockups watchface-loop.gif and quantized to Pebble 64-color).
- All screenshots verified by pixel sampling section backgrounds, not thumbnail eyeballing (two misreads this session prompted the rule; sample points in the emulator memory).
- PBW rebuilt with the C fix and restaged: `release-assets/simple-pixel-style-1.45.0-emery-gabbro.pbw` (1.45, emery+gabbro, offset default, declutter intact). Still NOT committed; on-watch sign-off gates the release commit as before.

Addendum 3 (2026-08-01): pip band follows the top bar (also in 1.45).

- Reported: the pip strip rendered as a white band on the black face, its slivers reading as white bars above and below the pips (ugly, per review of the re-snaps). Cause: `draw_fitness_pip_row` painted the band inverted-vs-global-theme with `FITNESS_PIP_INVERT_BAR` defaulting true.
- Fix (C + JS + config): the band now shares the TOP STATUS BAR's tuxedo color (the steps/BT/battery bar it extends), per the design instruction that a cutoff must share that bar's color. The invert toggle still exists but now flips relative to the top bar, for a deliberate contrasting band; its default is false at all three sites. The small-band seam pull-up condition updated to the new semantics (`!s_fitness_pip_invert_bar`). Color mode paints no band, unchanged, so the approved round color frames were not re-taken (kept on explicit instruction).
- All pip-bearing screenshots re-taken with the fix (six emery stills, round dark default, round GIF frames F1-F4; GIF reassembled with the kept F5/F6 color frames). Band pixels verified black on dark looks.
- PBW rebuilt and restaged with this fix included. Existing installs keep their saved invert choice; the on-wrist default look changes only for fresh installs or if the toggle is set off in settings.

Addendum 4 (2026-08-01): color-mode section gap diagnosed, handed to Codex; round Roboto shots added.

- Reported on the round color screenshot: a black bar between the weather section and the meeting bar, contacting the circles' bottom edge. Diagnosis (pixel-verified, exact numbers): in the compact 3-circle layout the weather fill ends at weather_y + weather_h (y=207 on gabbro: 180-24-1 start, height 2*24+4) while the meeting bar starts at EVENT_SEPARATOR_Y (216), leaving unpainted base (black) at y=207..215; the circles (center 183, radius 24) bottom out at exactly 207. Emery has the identical latent gap (190..199), invisible in B&W. The verbose path already fills to the separator (why the approved verbose color frame is clean).
- Fix is one expression (compact_weather_bottom also uses meeting_bar_y when s_color_mode == ColorModeColor); handed off per instruction as `CODEX_HANDOFF_gabbro_color_weather_gap.md` (surgical spec, screenshot repro recipe with the palette seds, pixel check at (130,212) expecting green, revert steps, dirty-tree git guards). Claude reviews the diff when Codex returns; the fix then joins 1.49 (or the then-current number) before release.
- New round Roboto (non-CASIO time) screenshots in `round-preview/`: gabbro-roboto-dark-default-pips.png, gabbro-roboto-dark-verbose-pips.png, gabbro-roboto-light-clean-pips.png (TIME_FONT '0', canonical black-bar dark and clean light, pips on). Note: gabbro emulator screenshots capture the square 260x260 framebuffer; the physical round display crops the corners, most visible on light looks.

Version note (2026-08-01, on instruction): everything in this section SHIPS AS 1.49, not 1.45. During this session's iteration the user made direct Pebble Appstore pushes of intermediate builds themselves, consuming version numbers through 1.48; those pushes were not individually logged here (contents = the day's builds as they stood at each upload). The consolidated build with the full set (Clay declutter Batches 1+2+4, provider copy fixes, offset-shadow dark default, color-mode phantom fix, pip band follows the top bar) is versioned 1.49 and staged at `release-assets/simple-pixel-style-1.49.0-emery-gabbro.pbw`. The 1.45-labeled staged PBW was deleted (same code, superseded label; 1.45 on the store is one of the user's own pushes). Release commit for 1.49 after on-watch sign-off, as usual.

## 1.50: Codex round work blessed, LECO round fallback, showcase GIF v2 (2026-08-01, released on instruction)

- Codex's diff reviewed and blessed. Exactly two foreign hunks in `src/c/jy-time.c` beyond this session's own uncommitted work: (1) `VERBOSE_WEATHER_CENTER_Y` 188 to 195 on PBL_ROUND (drops the centered one-line weather text 7 px on the round face; verified visually in the re-shot dark verbose frame, reads properly centered); (2) the color-mode weather-gap fix, landed verbatim per `CODEX_HANDOFF_gabbro_color_weather_gap.md` (verified live: the color compact look now tiles green straight into the meeting bar, former gap zone samples green, circles fully contained). All other diff hunks are this session's own 1.49-era work. No scope violations.
- LECO time font disabled on round only (cheap option per instruction): both TIME_FONT decode sites (inbox + persist load) map value 3 to TIME_FONT_DEFAULT under `#if defined(PBL_ROUND)`; rect platforms keep LECO. Verified on the gabbro emulator: TIME_FONT '3' renders the default Bitham digits. Clay option remains visible (platform-conditional Clay options are not worth the machinery); the TIME_FONT description now states the round fallback. Saved settings untouched.
- Font-mapping note for the screenshot archive: TIME_FONT values are 0 Default (Bitham), 1 CASIO, 2 Roboto, 3 LECO. The `gabbro-roboto-*` files from earlier today were staged with '0' and actually show the default Bitham face (they are the approved sharp reference looks; filenames kept).
- Showcase GIF v2 built to the requested frame list: `round-preview/gabbro-watchface-loop-v2.gif`, 10 frames at 1.2 s, 38 KB: dark CASIO circles, dark CASIO verbose, light inverted boxes, light clean (frames 1-4 re-shot on this build so the centering and gap fixes are in), Lagoon CASIO compact, color palette B verbose (re-shot), Sunset Roboto compact, Ocean Bitham one-line, Berry CASIO large weather, Citrus Roboto one-line. The rejected Arcade LECO frame is excluded per instruction, as is LECO from any promotional set. The parallel session's `gabbro-color-showcase-loop.gif` remains an audit artifact, not promotional.
- Emulator hiccup during verification: one "[Errno 111] Connection refused" on gabbro after a long-lived emulator session; resolved by the memory playbook (pkill, `pebble wipe`, single retry). SDK stays 4.9.148.
- Released: version 1.50 in package.json; PBW verified (1.50, emery+gabbro, offset default, pips default, declutter strings, LECO fallback description) and staged at `release-assets/simple-pixel-style-1.50.0-emery-gabbro.pbw`. Release commit made on the "stand up a release" instruction (source + this status doc; screenshots and working docs stay uncommitted for user curation). 1.49 was never released under that label; its staged PBW remains for reference.

## Weather stopped refreshing (user report, Andy) + watch-driven refresh, diagnostics, Large font plumbing (2026-08-25, unreleased)

User report: weather froze; NWS and Open-Meteo both affected; phone location normally off; toggling location on did not help. Investigation read the phone side too (Core Devices' open-source Pebble app, `libpebble3` PKJS runtime, cloned to the session scratchpad).

Findings:
- All three endpoints healthy from here (Open-Meteo 200 with `current`, NWS points 200 with our UA and 403 with an empty UA, zippopotam 200). No weather-path change in `index.js` since 1.30 (June 7); 1.50 only touched the Clay declutter for the weather rows.
- Phone runtime (`AndroidSystemGeolocation.kt`): `getCurrentPosition` returns "Location permission not granted" unless the Pebble app holds ACCESS_FINE_LOCATION (Android's "Precise" toggle), and "Location not available" when location services are off and no provider has a cached fix. iOS returns "Location access denied". The runtime caps the timeout at 15 s and returns a stale last-known fix when it has one (that is how weather can "work" with location off until the cache is gone).
- Watchface defects that turned any of those into a permanent, silent freeze: (1) refresh relied only on PKJS `setInterval` (Pebble's own guidance: don't; have the watch send a message), the watch never sent anything; (2) `messageInFlight` stuck forever if `Pebble.sendAppMessage` threw or never called back; (3) GPS failure with no manual pair did nothing at all, the NWS ZIP path was unreachable in that case and there was no last-known-coordinates fallback; (4) the Open-Meteo XHR had no timeout and no status check; (5) the face keeps rendering persisted weather with no age indication and nothing tells the user why.

Changes (built, `pebble build` clean on emery + gabbro, version still 1.50, nothing in release-assets):
- C: `send_refresh_request()` sends `REFRESH_REQUEST` (new message key, appended last, id 10317) from `tick_handler` at :00/:15/:30/:45 when the phone is connected; `outbox_failed_handler` registered. Uses the existing 64-byte outbox.
- pkjs: `appmessage` listener routes the ping through `maybeRefreshWeather()` (due by interval, 4-minute spacing, 5-minute retry after a failure) and `maybeRefreshCalendar()` (9-minute spacing); the interval timer now goes through the same gate. Message queue has a 20 s watchdog and try/catch. `resolveWeatherCoords()` order: manual (when selected), phone location, manual pair, NWS ZIP, last known coordinates (`weather-last-coords`, saved on every successful fix). Open-Meteo fetch gets `timeout` 15 s, HTTP status check, single-fire `finish(error)`. `weather-diag` in localStorage records okAt/source/lastError/lastErrorAt.
- Settings page: new `weather-status` text item at the top of the Weather section, filled at `showConfiguration` from `weather-diag`: last update time and source, last error with a plain-words fix hint (Precise location / location services / manual coordinates). This is the "logs" a user can send: a screenshot of the Weather section.
- Harness: `jy-time/test/pkjs_weather_harness.js` (node, no deps, `node test/pkjs_weather_harness.js`) runs `index.js` in a vm sandbox with fake Pebble/XHR/geolocation/localStorage/timers against real API fixtures in `test/fixtures/`. 12 scenarios, all passing; 10 of them failed before the fix.
- Large time font plumbing (for the second user request): `TIME_FONT_LARGE = 4` in the enum and both decode sites, Clay option "Large" (value 4), pkjs clamp 0..4. Until the font lands, value 4 renders the default Bitham digits. `resources/fonts/Montserrat-Bold.ttf` (OFL, `LICENSES/Montserrat-OFL.txt`) staged as the Gotham-style candidate. Rendering is Codex's slice: `CODEX_HANDOFF_large_time_font.md`.

Open:
- Andy's platform/app version unknown; reply drafted in the session summary asks for it plus a screenshot of the new status line once the release is out.
- No stale indicator on the face itself (visual decision, not made).
- Version bump + release packaging on instruction only.

## 1.51: weather refresh driven by the watch, location fallbacks, settings-page weather status (2026-08-25, released on instruction)

- Everything in the 2026-08-25 entry above, plus, from the battery review: consecutive-failure backoff in `maybeRefreshWeather` (retry after 5, 10, 20 minutes, then capped at the configured interval). Harness cadence scenario: 30-minute interval plus a watch ping every 15 minutes over 2 hours gives 5 Open-Meteo fetches healthy (identical to 1.50) and 6 when every fetch fails (one early retry, then the old cadence). Watch side adds only one 1-byte outbox send at :00/:15/:30/:45 while connected; no new subscriptions or sensors.
- Clay option "Large" for TIME_FONT withheld from this release (value 4 plumbing ships inert; the option comes back when the Codex font work lands, see `CODEX_HANDOFF_large_time_font.md`).
- Emulator (emery, SDK 4.9.148): install first try, startup path clean (all settings sent, Open-Meteo fetched, "Weather sent"), face renders as before. A `pebble screenshot` during `pebble logs` drops the log socket (QemuInboundPacket.footer warnings, then "Connection to remote host was lost"); re-attach logs afterwards rather than running both at once.
- Harness 13/13 (`node test/pkjs_weather_harness.js`). Clean `pebble build` on emery + gabbro. Version 1.51. PBW at `release-assets/simple-pixel-style-1.51.0-emery-gabbro.pbw`.

## 1.52: "Large" time font option (2026-08-25, released on instruction)

- New Time font option "Large" (TIME_FONT value 4): bundled Montserrat Bold digits at 56 px (`FONT_TIME_LARGE_56`, digits + colon only, +3 KB resources, +84 B RAM), the closest permissively licensed match to the Default Bitham/Gotham look. Default stays CASIO (value 1); nothing else on the face moves.
- Codex delivered the visual slice per `CODEX_HANDOFF_large_time_font.md`: font resource entry, `s_font_time_large` load/unload beside the CASIO fonts, a Large branch in `draw_time_row_at` with its own draw frame (frame_y - 1, height TIME_FRAME_H + 10) and AM/PM anchored to the digit bottom like Roboto/LECO. 27 lines, in spec, scaffolding reverted. "10:08" measures 150x56 on emery, so the AM/PM label fits with 25 px spare; gabbro renders Large without clipping, no round fallback needed. Screenshots: `Current Screenshots/large-font/` (uncommitted, user curation).
- Clay option re-added with the description sentence; pkjs clamp and both C decode sites already accepted 4 since 1.51. Harness 13/13. Clean `pebble build` emery + gabbro. PBW at `release-assets/simple-pixel-style-1.52.0-emery-gabbro.pbw`.

## 1.53: shake option "Next calendar events - Large month & day" + Time font option rename (2026-09-03, released on instruction)

- New shake option "Next calendar events - Large month & day" (`ShakeBehaviorCalendarEventsLargeDate = 12`, Clay value `calendar_events_large_date`), answering the watchface user who could not make out the 06/04 month/day on the Upcoming list. The existing "Next calendar events" list is unchanged; 9 stays reserved.
- Codex delivered the visual slice per `CODEX_HANDOFF_calendar_large_month_day.md`: `calendar_draw_overlay_large_date` draws each row as the title, then the date large in the foreground color (Gothic 28 Bold at 3 rows, Gothic 18 Bold at 5) flush left, with the countdown in the plain list's small muted Gothic flush right, bottom-aligned to the date digits; a slot with no epoch (older companion build) falls back to the plain countdown. Always shows the date (the Show event dates toggle gates only the plain list and the meeting bar), honors date order, leading zeros and the 3 / 5 count. Row pitch and dividers match the plain list. Scaffolding reverted. Screenshots: `Current Screenshots/calendar-large-date/` (uncommitted, user curation); gabbro renders without clipping.
- Clay: option directly under "Next calendar events"; the "Show event dates" description says the large list always shows dates; pkjs `SHAKE_BEHAVIOR_IDS.calendar_events_large_date = 12`; the calendar shake group (event count) shows for both calendar options; date order + leading-zero rows stay reachable while Show event dates is off and the large list is selected (date position stays with the toggle, it is bottom-line only).
- Time font option renamed "Default (Large)" and moved to second place under "Default" (value 4 unchanged; default still CASIO).
- Release checks: default audit 101 config.json `defaultValue`s vs `DEFAULT_SETTINGS`, 0 drift, no C static initializer touched. Harness 13/13. Clean `pebble build` emery + gabbro (emery RAM 53,314 B, resources 17,090 B). Emulator smoke install on emery: first attempt Connection refused, pkill + `pebble wipe` + one retry installed and the face booted (screenshot checked). PBW at `release-assets/simple-pixel-style-1.53.0-emery-gabbro.pbw`.
- Appstore listing: `release-assets/APPSTORE_DESCRIPTION.md` overlay list and counts refreshed to eleven overlays (was six, 0.67-era). The other sections of that file still date from 0.67.

## Still pending: Default (Large) digits vs timezone text line (handoff open since 2026-08-25)

- Bug: Default (Large) digits (ink bottom ~124 on emery) cover the secondary-timezone text line, which is anchored for the Default digits at TIME_VISUAL_BOTTOM (line at 115..131). Only the text-line style is affected; circle style and every other font are fine. Present in 1.52 and 1.53.
- `CODEX_HANDOFF_large_font_multi_tz_overlap.md` has NOT been run: as of the 1.53 commit there are no hunks in `draw_time_row_at` or the timezone line code. Expected lifts once it lands: 8 px emery circles layout, 0 px verbose one-line layout, 11 px gabbro. Ships in the next release.
- Handoff refreshed 2026-09-03 against the 1.53 tree (every code claim re-verified: draw_time_row_at 1658, multi-timezone block 4699-4713, statics 510-515 / 541, weather_band_y 844; four pre-existing warnings listed; date-text clearance rule added to the acceptance criteria; T1 scaffolding line numbers 432 / 440 / 509-512). Ready to hand to Codex as-is.

## Pending: Large month & day overlay, three layout options (sample build, 2026-09-03)

- Verdict on the 1.53 overlay: reads awkwardly. Diagnosis: the 28 Bold numeric "09/03" outweighs the title and has to be decoded; each row is three pieces floating apart (title, big date, tiny countdown far right); row 0 carries the "2:30P | " prefix on the title line while the date sits on the next line; the 5-row date is barely larger than the title.
- Handoff `CODEX_HANDOFF_calendar_large_date_options.md`: sample build with three renderers behind a compile-time `LARGE_DATE_SAMPLE_OPTION` selector. A = calendar block (month abbreviation over a large day number on the left, title + countdown stacked on the right). B = date headline ("Sep 3" / "3 Sep" in 24 Bold, weekday and countdown small on the same line, title beneath). C = tightened numeric (1.53 structure, date 24 Bold, countdown moved next to the date). English-only sample helpers; date-language wiring is Claude's slice for the chosen option. Ten screenshots to `Current Screenshots/calendar-large-date/options/`; scaffolding reverted; losers stripped by Claude after the pick.
- Independent of `CODEX_HANDOFF_large_font_multi_tz_overlap.md` (different functions); either can run first.

## Large month & day: emery layout picked, round plumbing, both Codex handoffs run (2026-09-03, unreleased)

- Codex ran `CODEX_HANDOFF_large_font_multi_tz_overlap.md`: Default (Large) digits lift when the timezone text line is on (8 px emery circles, 0 px verbose, 11 px gabbro), line anchored to the lifted digit bottom. Screenshots in `Current Screenshots/large-font/` (emery-large-tzline-circles / -verbose, emery-default-tzline-control, gabbro-large-tzline-circles). Off-spec extra in the same run: the timezone line separator changed from "|" to " | " in draw_multi_tz_line (kept; one-line revert if unwanted).
- Codex ran `CODEX_HANDOFF_calendar_large_date_options.md`; the ten option screenshots are in `Current Screenshots/calendar-large-date/options/`. Pick (made in the Codex session): 3 rows = Option B date headline with the date at 28 Bold, weekday + event start time (18 Bold) + countdown on the date line, title 24 Bold on the second line, row 0's "h:mm | " prefix stripped; 5 rows = Option C tightened numeric unchanged. `selected-emery-3rows-large.png` / `selected-emery-5rows.png`. Committed as delivered (commit "Large month & day: emery layout picked...").
- Follow-up commit (Claude): sample selector and unpicked Option A removed; English-only sample helpers replaced by date-language helpers (`large_date_headline` "Sep 3" / "3 Sep" per date order, "3 sept." French, "9月3日" Japanese; `large_date_weekday` THU / JEU / 木; `large_date_event_time` honors 24-hour); rows factored into `large_date_draw_row_headline` and `large_date_draw_row_numeric`, shared by the emery panel (`calendar_draw_overlay_large_date`, rect only) and a new round view. Round plumbing: `shake_overlay_frame` makes the overlay layer full screen on round for this behavior only (applied in shake_show_overlay), the update proc fills `layer_get_bounds`, `round_overlay_safe_frame` gives chord-aware row frames in screen coordinates, `calendar_draw_overlay_large_date_round` is a PLACEHOLDER layout (title y 16/12, 3 rows from y 43 pitch 58, 5 rows from y 35 pitch 38, chord separators). Builds clean on both platforms (four pre-existing warnings only), harness 13/13. Japanese kanji at Gothic 28 Bold and the French abbreviations are unverified on screen.
- Why the round view needed plumbing: every shake overlay is a 200x228 layer centered on gabbro's 260x260 display, and the face shows through around it (that is what Josh saw on the PTR2 screenshot). Only this view goes full circle; the other overlays still show the panel. A backdrop layer that fills the ring for every overlay is a separate ~15-line follow-up if wanted.
- Handoff written: `CODEX_HANDOFF_calendar_large_date_round.md` (tune the placeholder within stated bounds, five screenshots incl. two emery regression checks, scaffolding reverted). Release 1.54 after it lands: version bump, default audit, PBW, Appstore description untouched (overlay list already current).

## Large month & day: round layout landed (2026-09-03, unreleased, ready for 1.54)

- Codex ran `CODEX_HANDOFF_calendar_large_date_round.md`: one-line tune (row stacks start 3 px higher: 3 rows from y=40, 5 rows from y=32), otherwise the placeholder numbers held. Final geometry: title y 16 / 12; 3 rows pitch 58 band 50 margin 10, rows at (37,186) (10,240) (24,212), separators at y 94 and 152; 5 rows pitch 38 band 34 margin 10, rows at (45,170) (15,230) (10,240) (10,240) (34,192), separators at 68 / 106 / 144 / 182; empty state y 112. Outermost ink about 2 px inside the circle, nothing clipped (the 6 px target in the handoff was not reachable with the fixed bands; accepted). T3 not needed.
- Screenshots in `Current Screenshots/calendar-large-date/round/`: gabbro-3rows, gabbro-5rows, gabbro-empty, gabbro-3rows-ja, emery-3rows-check, emery-5rows-check. The two emery shots match the selected emery references (layout identical, clock-derived times differ). Committed with the stale placeholder comment above the function rewritten to the final numbers.
- Japanese: the ja shot shows missing-glyph boxes for 月 / 日 and the weekday kanji. That is the emulator (no CJK language pack); the date bar and event titles rely on the same firmware fallback and were confirmed on a Japanese user's watch at 1.28. Unverified on-device for this overlay: how the fallback kanji size sits next to the Gothic 28 Bold digits in "9月3日". Japanese users report; the numeric 5-row layout is unaffected.
- Sample-data caveat from the run: the long title sits in row 1 (narrow on round), so the screenshots do not literally show a long title in the wide middle rows; the chord widths are visible in the geometry table above.
- 1.54 release contents when cut: emery Large month & day pick, round full-circle Large month & day view, Default (Large) digits vs timezone text line fix, timezone separator " | ", date-language headline text. Version bump + PBW on instruction only.

## 1.54: Large month & day redesign (emery + round), Default (Large) vs timezone line (2026-09-03, released on instruction)

- "Next calendar events - Large month & day" redrawn: emery 3 rows = date headline (Gothic 28 Bold "Sep 3", weekday, event start time, countdown at the right, Gothic 24 Bold title beneath; row 0's "h:mm | " prefix stripped in favor of the start time); emery 5 rows = tightened numeric (title, MM/DD date with the countdown beside it). Round watches draw the list on the full 260x260 circle with chord-aware row widths and chord separators (overlay layer goes full screen for this view only). Month, weekday and start-time text follow the Date language setting (English honors date order; French "3 sept." / JEU; Japanese "9月3日" / 木, on-device only, the emulator has no CJK pack). All three Codex handoffs of the day ran; see the three entries above for geometry and screenshots.
- Default (Large) digits now lift when the timezone text line is on (8 px emery circles, 0 px verbose, 11 px gabbro) and the line anchors to the lifted digit bottom; timezone line separator is " | ".
- Release checks: default audit 101 config.json `defaultValue`s vs `DEFAULT_SETTINGS`, 0 drift; the only new C static is the internal `s_large_time_ink_bottom` (no user-facing default; config.json and index.js untouched since 1.53). Harness 13/13. Clean `pebble build` emery + gabbro (four pre-existing warnings only; emery RAM 56,114 B, gabbro 57,062 B, resources 17,090 B). Emulator smoke install on emery (after `pebble wipe`): installed first try, face boots (screenshot checked). PBW at `release-assets/simple-pixel-style-1.54.0-emery-gabbro.pbw`. Appstore description unchanged (overlay list already current as of 1.53).
