# Simple Pixel Style 0.76

## Halcyon Day View — removed

The 0.72 "Halcyon Day View" shake overlay never looked right and the 0.73 redo wasn't worth the menu surface it cost. Removed outright. Gone with it: the `weather_ring` shake option, the four `WEATHER_RING_*` message keys + settings sub-section, the four color presets, the perimeter / sun / center draw helpers, and the `HOURLY_CODES` byte-array message key that fed it. The seven other shake overlays (Fitness rings, Calendar events, Your Day, Detailed weather, Alt timezone, Heart rate big, Tide chart) are untouched.

## Tide chart — "single spike" bug fixed

Two compounding causes:

1. **NOAA's `&range=N` is silently ignored when paired with `&date=today`** — the API always returns exactly one calendar day's 24 hourly entries regardless of the requested range. With our 24h-forward window starting at "now", that left about six future hours populated and the rest of the window empty.
2. **No historical data was sent at all** — the chart had no context for "where the tide came from", only "where it's going."

Fix:

- Switched to explicit `begin_date=YYYYMMDD&end_date=YYYYMMDD` (which NOAA does respect for ranges) spanning yesterday → tomorrow in UTC: 72 entries returned.
- The 24-hour visible window is now centered on "now": twelve hours of history followed by twelve hours of forecast. The dotted vertical now-indicator sits at the chart midpoint instead of stuck to the left edge.
- Past portion of the curve draws in a muted color; future portion draws in the strong theme foreground — the eye reads "history → now → forecast" naturally.
- Tide key matching is now timezone-safe. Both the request (`time_zone=gmt`) and the lookup keys are UTC throughout, so a cross-timezone station pick (e.g. a Pacific user choosing an Atlantic station) still produces 24/24 hourly matches.
- Verified against real NOAA data for the Pensacola station (8729840) in four timezone scenarios: all return 24/24 matched hourly values.

**Tide schema migration** (added after the first 0.73 build was reported as still showing stale data): the watchface now stamps a `TIDE_DATA_VERSION` value in persistent storage and wipes the persisted hourly bytes + next-high / next-low on any version mismatch. Upgrading from 0.72 to 0.73 therefore guarantees you see the "Waiting for tide data…" placeholder until the next fetch lands — no more partial chart from a stale storage layer. The chart is also stricter about what counts as "having data": it requires the current-hour sample plus at least 12 of the 24 window slots to be populated before drawing, so the leftover-from-old-build sliver can't render as a misleading single-spike again.

**Tide fetch hardening** (added after a second post-install report of the chart staying stuck on "Waiting…"). Three things on the JS side:

- **Portable date parsing.** `parseNoaaTime` no longer uses `new Date("ISO-string")` — PebbleKit JS's string-to-Date parsing has a history of silently returning Invalid Date. The new implementation regexes the `"YYYY-MM-DD HH:MM"` format into explicit numeric components and builds the epoch via `Date.UTC(...)`. Pattern lifted from a working community tide watchface. Same change applied to the hourly-window math: no more mutating `setUTCMinutes(0,0,0)`, just integer math on epoch milliseconds.
- **15-second XHR timeout + retries.** If a NOAA request stalls without firing `onerror`, the watch used to wait forever. Now the request times out after 15 seconds and `fetchTidesForStation` retries up to 3 times spaced 8 seconds apart. If the hilo (high/low) endpoint comes back empty but the hourly endpoint succeeded, the chart still draws — just with `--:--` next-high / next-low fields until the retry lands.
- **Verbose console logging.** Every step of the tide pipeline now logs to the Pebble JS console (`pebble logs` from the CLI, or the Pebble app's JS log view on the phone): "GET …", "ok (N bytes)", "packed X/24 hourly levels", "hilo got N events", "gave up after 3 hourly attempts", etc. If the chart still won't populate after installing this build, grab those logs and I can pin down the failing step.
