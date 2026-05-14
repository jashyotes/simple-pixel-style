# Shake-to-Show Fitness Rings Plan

## Scope

Add an optional watch-shake overlay to Simple Pixel Style that shows a Samsung Health-inspired fitness summary. The normal watchface render path remains unchanged.

Reference image:

- `jy-time/sample-screenshots/shake-fitness-rings-reference.png`

## Files Touched

- `jy-time/package.json`
  - Append new `messageKeys`.
  - No version bump.
- `jy-time/src/pkjs/config.json`
  - Append one new Fitness Rings section after Calendar.
  - Do not reorder existing sections.
- `jy-time/src/pkjs/index.js`
  - Add defaults.
  - Add `sendFitnessSetting(settings)` mirroring the existing settings send pattern.
  - Call it from the same path that sends layout settings.
- `jy-time/src/c/jy-time.c`
  - Pure additions for new persistence keys, state, overlay layer, draw proc, tap handler, timer, and inbox branches.
  - Do not modify `face_update_proc`, existing draw helpers, complications, weather, meeting bar, inversion code, or anything between the current `face_update_proc` lines.
- `jy-time/sample-screenshots/shake-fitness-rings-reference.png`
  - Copy of `/home/jates/Downloads/sample of samsung.png`.

## Clay Settings

| Key | Type | Default | Label |
| --- | --- | --- | --- |
| `FITNESS_SHAKE_ENABLED` | toggle | false | Enable shake to show fitness rings |
| `FITNESS_RING_STEPS_ON` | toggle | true | Show steps ring |
| `FITNESS_RING_ACTIVE_ON` | toggle | true | Show active time ring |
| `FITNESS_RING_CALORIES_ON` | toggle | true | Show active calories ring |
| `FITNESS_TARGET_STEPS` | number input | 10000 | Steps target |
| `FITNESS_TARGET_ACTIVE_MIN` | number input | 30 | Active minutes target |
| `FITNESS_TARGET_CALORIES` | number input | 500 | Active calories target |
| `FITNESS_COLOR_STEPS` | color | green palette match | Steps ring color |
| `FITNESS_COLOR_ACTIVE` | color | blue palette match | Active ring color |
| `FITNESS_COLOR_CALORIES` | color | red palette match | Calories ring color |

## Watch-Side Data Flow

- Load all new persisted settings with defaults.
- Subscribe to `accel_tap_service` only when `FITNESS_SHAKE_ENABLED` is true.
- If `FITNESS_SHAKE_ENABLED` is disabled from settings while the overlay is visible, hide it and unsubscribe.
- On tap:
  - Log axis and direction for first build tuning.
  - Read:
    - `HealthMetricStepCount`
    - `HealthMetricActiveSeconds`, displayed as minutes
    - `HealthMetricActiveKCalories`
    - `HealthMetricRestingKCalories`, used only for total calories
    - `HealthMetricWalkedDistanceMeters`, displayed as km
  - Show a full-screen second-layer overlay.
  - Register or reschedule a single timer for `FITNESS_OVERLAY_VISIBLE_MS`.
- On timer fire:
  - Hide the overlay.
  - Null the timer handle.
- In `deinit()`:
  - Unsubscribe from `accel_tap_service` if subscribed.

## Overlay Layout

- Full-screen background uses `theme_bg_color()` so the overlay respects existing light/dark theme.
- Top cluster is three overlapping circle progress arcs in a heart-like composition:
  - Steps: upper-left green lobe.
  - Activity calories: upper-right red lobe.
  - Active time: lower-center blue lobe.
- Each ring is drawn as a dim full circle, then a bright progress arc from top, clockwise, clamped to 0-100%.
- Disabled rings are skipped in their fixed positions; remaining rings do not recenter or shuffle.
- Below the rings:
  - Steps row with procedural leaf icon.
  - Active time row with procedural clock icon.
  - Activity calories row with procedural flame icon.
  - Disabled metric rows collapse out.
- Separator line.
- Always show:
  - Total burned calories: active + resting, in Cal.
  - Distance while active: walked meters converted to km.
- No new bitmap resources; icons are procedural pixel glyphs.

## Explicit Non-Goals

- No changes to `face_update_proc` or anything it calls.
- No changes to complication slots, weather rendering, inversion, calendar, or any existing setting.
- No AppMessage inbox size increase.
- No accelerometer subscription while shake mode is disabled.
- No default enablement.
- No Android companion changes.
- No version bump, commit, or push.
- No ring-fill animation.

## Verification

1. `pebble build` succeeds from `jy-time/`.
2. `message_keys.h` contains the 10 new keys.
3. Default `FITNESS_SHAKE_ENABLED=false` leaves the normal face unchanged.
4. When enabled, tap/shake shows the overlay and it auto-dismisses after `FITNESS_OVERLAY_VISIBLE_MS`.
5. Tapping again while visible reschedules the same timer.
6. Each ring toggle hides that ring and row only.
7. Target changes alter the matching progress percentage.
8. Color changes recolor the matching ring and metric row.
9. Disabling shake makes tap do nothing.
10. The overlay visually follows the saved reference image: heart composition, green/red/blue lobes, row order, separator, and two bottom rows.
11. Diff check confirms no hunks touch `face_update_proc`.
