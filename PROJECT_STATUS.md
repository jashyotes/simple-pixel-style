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
