# June effect completion - C3f-1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3f-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3f-1 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH` (+ `_DEBUG`) |
| Log tag | `JUNE_FX C3f-1` (`python tools/logs.py --grep "JUNE_FX C3f-1"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3f - Done (2026-10-09, Opus 5.5 high; no code, no flag, no build)" "C3f-1 - port: June's force-down flush on leaving a down state (user + NPC)" "C3f-1 - Done"
grep -n "(C3f)" june2017_re/funcmap.tsv
```

## Task

Review C3f-1's commit(s) against the C3f rows. The things it could get wrong:
- the user flush's position: before the IsLocalUnit / shock / frozen returns, after the riding-pet clear (`320348c0 @3203492b`);
- the compared state is the one being **left** (Future nowState), and the set is DOWN_FRONT / DOWN_BACK / AIR_DOWN_LANDING, not
  AIR_DOWN_INVINCIBLE;
- `3202a9c0`'s order (UpdateDamageFlushMP first, then the `+0xf8` test, `nowSpeed.y = 0` only on the flag path, the clears on either);
- the NPC flush before the bounds / cool-time test (`327577d0`), byte compares;
- CommonFrameMove's reset in `#else`; the DRSC guard skips the whole branch (`@3276a1f3` -> `@3276a290`);
- the JUNE_BUGS rows and the corrected RT_DOWN comment.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH --range <first C3f-1 commit>^..HEAD --with SERV_IRUHADEV_JUNE_FORCE_DOWN_FLUSH_DEBUG --files <its code files>`.
Paste its `files:` / `RESULT:` lines; explain every OTHER row and deleted line (the RT_DOWN comment line is expected).

Spot-check 2-3 sites in Ghidra (read-only); if one fails, re-check the whole phase. Read the play-test's `JUNE_FX C3f-1` lines against
the expectation. Turn `_DEBUG` off (then `tools/build.py --deploy`). Fix clear transcription errors under the same flag; hand design
decisions back to the user. Write "C3f-1 - Review" into the plan doc, update the STATUS row, commit.
