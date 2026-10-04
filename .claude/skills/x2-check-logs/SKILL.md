---
name: x2-check-logs
description: Read the offline client's game logs (ErrorLog.txt, offline_server.log, offline_packets.log) for the current phase with tools/logs.py. Use when the user says "check logs", "tested", "passed, check logs", "closed the game", or after a play-test hand-off.
---

# Check the play-test logs

```sh
python tools/logs.py --tag JUNE_BUFF                # the phase's own tag; add --grep to narrow
python tools/logs.py --tag JUNE_BUFF --grep "BBT (75|91|103)"
python tools/logs.py                                # status + UNHANDLED/EXCEPTION summary only
```

Read the header first; it decides whether the lines mean anything:
- `exe: ... = tools/build.py deploy at HEAD abc1234` - the deployed exe is the one built. If it
  says `NOT the last tools/build.py deploy`, someone deployed by hand or a later build replaced it:
  find out which build the user played before judging.
- `run BEFORE deploy (stale ...)` - the user has not played the new build; the lines are from the
  old exe. Say so and ask them to run the test again. Do not judge stale lines.
- `game: RUNNING` - logs are still being written; fine for a mid-test peek, re-read after they close it.

Then judge **only this phase's lines** (memory: scope play-test log checks to the phase):
- Compare against the "Log lines to read afterwards" list from the phase's play-test hand-off.
- Ignore unrelated errors already present (missing particles, TutorSystem, E_DISABLE_HB_CHECK_REQ).
  Mention a new one in one line at most; do not chase it.
- Report per check: passed / failed / not reached, quoting the decisive line with its line number.

Tag conventions: `[JUNE_BUFF]` buff port, `[JUNE_FX]` effect engine (ErrorLog), `CX2OfflineLog::Server`
lines in offline_server.log for offline-emulator work. A new plan picks one tag and names it in its
STATUS row.
