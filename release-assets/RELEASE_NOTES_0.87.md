# Simple Pixel Style 0.87.0

## Step counter font reverted to W800 sprite only

All step counter font options are removed. The W800 pixel sprite digits are the only step counter rendering. No Clay select, no alternative fonts. This rolls back the experimental Roboto / CASIO / Default step font additions from prior 0.84 builds.

Cleaned up:
- Removed STEPS_FONT Clay select
- Removed STEPS_FONT messageKey from package.json
- Removed FONT_WV58A_DIGITS_20 font resource
- Removed s_steps_font enum and state
- Removed format_steps_with_comma helper
- Removed Roboto and CASIO step font handles
- Restored the simple draw_w800_steps_top_bar that always calls draw_w800_steps_digits

## Carried forward from prior 0.84 builds

- Time font select at the top of Layout: Default (Bitham 42 Bold), CASIO style with phantom 88:88 backdrop, Roboto, LECO.
- CASIO phantom 88:88 backdrop toggle. The JS sender bug from the prior 0.84 build is fixed; toggling off now actually disables the backdrop.
- Forecast graph rebuilt as a closer port of forecaswatch2 (correct draw order: night hatch, precip area, sun boundaries, temp line with stroke 3; hour labels using the local start hour; tick marks per hour; no plot border).
- Sunrise / sunset labels widened so "12:45AM" and "12:45PM" no longer truncate.
- "Black & White - Tuxedo" renamed to "B&W - Tuxdeo".
- Quiet-time mouse icon anchored to `TIME_VISUAL_BOTTOM - QUIET_TIME_ICON_SIZE` in every font mode and every layout.

## Attribution

DIGITAL_BOLD.TTF and the AM/PM bitmaps are vendored from the Casio WV-58A with AM/PM Pebble watchface by Aardvark (https://github.com/Whobeu/WV-58A). The upstream repository has no LICENSE file; attribution is provided in `LICENSES/WV-58A-ATTRIBUTION.txt`.

The forecast graph rendering technique is ported from ForecasWatch 2 by Matt Rossman (https://github.com/mattrossman/forecaswatch2).
