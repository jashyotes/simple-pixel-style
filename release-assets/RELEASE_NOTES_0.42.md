Simple Pixel Style 0.42

- Fixes bottom calendar event selection when meetings overlap or roll over close together.
- The watchface now treats the calendar as a cursor: if an event has started and has not reached its real end time, that latest-started event wins; otherwise the next future event wins.
- Prevents an older overlapping event from masking the current meeting title, such as showing a previous 2 PM event during a 3 PM meeting.
- Keeps the next-event countdown and bottom event title selected from the same event record across one or two iCalendar feeds.
- Uses each event's real `DTEND` when present, while keeping the existing 15-minute fallback for feeds without an end time.
