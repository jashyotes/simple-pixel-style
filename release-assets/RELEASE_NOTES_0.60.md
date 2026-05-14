# Simple Pixel Style 0.60

Emergency cleanup release that restores the intended Your Day and Color Mode work on top of the stable 0.56/0.59 watchface.

- Restores Your Day window modes.
- Defaults Your Day to Fixed, 8AM-5PM, with a 10-hour Workday view.
- Adds optional Rolling window mode, 2-10 hours, starting at the previous hour.
- Uses dynamic Your Day titles: `N HOUR WORKDAY` or `N HOUR WINDOW`.
- Replaces the broken current-hour line marker with a concentric current-hour pip.
- Adds Color Mode: Black & White default, or per-section foreground/background colors.
- Keeps Light Mode and all invert toggles intact for Black & White mode.
- Keeps shake overlays monochrome and Light Mode driven.
- Restores the Step graph today shake option.

Verification:

- `pebble build` passed for emery.
- Build version label is `0.60`.
- No screenshot seed or banner seed code is present.
- `HOUR WORKDAY` and `HOUR WINDOW` are present in the built binary.
