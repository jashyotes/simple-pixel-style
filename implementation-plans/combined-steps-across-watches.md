# Combined step count across two watches

Approved 2026-11-09 (Josh: "as long as we make it optional and don't break other shit"). Design + verified phone-app facts: FUTURE_FEATURES.md, "Combined step count across two watches".

Constraints: optional (off by default, three-place default rule), no change to any existing behaviour when the toggle is off, no release until Josh says so.

## Batches

1. Message keys: append COMBINE_STEPS, MY_STEPS, MY_STEPS_DATE, OTHER_STEPS to the END of package.json messageKeys (keeps every existing numeric key stable).
2. Watch (src/c/jy-time.c):
   - persist keys 285 COMBINE_STEPS, 286 OTHER_STEPS, 287 OTHER_STEPS_DAY (YYYYMMDD stamp).
   - statics: s_combine_steps=false, s_other_steps, s_other_steps_day, s_own_steps, s_last_sent_steps, s_last_steps_send_t, s_steps_sync_due.
   - other_steps_today(): 0 unless toggle on and stamp == today.
   - update_stats(): own from HealthService, s_steps_count = own + other_steps_today(). Goal vibe, s_steps_buf, W800 bar, pip row, steps bubble all follow s_steps_count already.
   - fitness_read_health_values(): s_fitness_steps_value = own + other (rings overlay, rings bubble, step-history "today" bar).
   - send_steps_sync(): MY_STEPS + MY_STEPS_DATE over the outbox; skipped silently when busy, retried next minute. Sent only when toggle on and phone connected; cadence = first tick after launch/reconnect, then whenever own count changed and >= 2 min since last send.
   - inbox: COMBINE_STEPS (bool, persist, mark sync due, update_stats), OTHER_STEPS (int, stamp today, persist both, update_stats).
   - init: read the three persisted values. connection_handler: reconnect marks sync due.
3. Phone (src/pkjs/index.js): DEFAULT_SETTINGS.COMBINE_STEPS=false; sendLayoutSetting carries COMBINE_STEPS; store 'xwatch-steps' = { token: {d, s, t} } keyed by Pebble.getWatchToken() (fallback 'local'); appmessage MY_STEPS -> record + reply OTHER_STEPS for that date; ready/webviewclosed -> send OTHER_STEPS for the phone's local date when the toggle is on; prune records older than 3 days.
4. Clay (src/pkjs/config.json): toggle "Combine steps from my other watch" under Layout, right after the Top W800 step counter, default false.
5. Tests: harness fake gains getWatchToken + shared-storage option; new scenarios cover record/reply, cross-watch sum on ready, stale-date exclusion, toggle off = no message. Existing 13 must stay green.
6. Verify: three-place default audit (config.json defaultValue vs DEFAULT_SETTINGS vs C statics), `pebble build`, emulator smoke if qemu runs. Status entry in PROJECT_STATUS.md. No version bump, no commit, no PBW copy until Josh says release.
