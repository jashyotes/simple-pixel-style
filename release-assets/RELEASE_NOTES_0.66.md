# Simple Pixel Style 0.66

- Reorganized the settings menu so the Theme section (Light/Dark + Color mode) sits at the top of the form, with the rest of the menu flowing from there. Light/Dark is now framed clearly as the single switch that drives both the Black & White theme and every shake overlay's coloring.
- Settings page now hides the inversion toggles when Color mode is selected and hides the per-section color pickers when Black & White is selected, so only the controls that actually affect the current mode are visible.
- The Shake section now shows only the sub-settings for the overlay you've selected — picking Fitness rings hides Calendar/Your Day/Alt timezone controls, and vice versa.
- Fixed Your Day and Detailed Weather overlays drawing the wrong weather icon variant in Light mode when an Invert toggle was active on the main face. Shake overlays now always pick their bitmap theme from the Light/Dark setting alone.
