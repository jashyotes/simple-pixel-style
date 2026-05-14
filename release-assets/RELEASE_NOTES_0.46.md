Simple Pixel Style 0.46

- Fixes the large verbose-weather to meeting-bar boundary when those sections use different colors.
- Extends the weather section background through the 2px buffer below the weather description instead of leaving an exposed fill edge at y=200.
- Suppresses the explicit separator line for that mismatched-color large-weather case.
- Leaves circle complications, small verbose weather, and same-color large weather/meeting layouts on their existing separator behavior.
