# June effect completion - C3d-2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3d-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3d-2 done (its Done note and commits) and its play-test passed |
| Flag | `SERV_IRUHADEV_JUNE_USER_STATE_ID16`, `SERV_IRUHADEV_JUNE_USER_ACTION_ID16` |
| Log tag | none (the play-test reads `0 EXCEPTION` and the census) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-2 - Done (2026-10-08, Sonnet 5.5 high)"
cat june2017_re/c3d1_state_id_sites.tsv
grep -n "(C3d-1)" june2017_re/funcmap.tsv
```

## Task

Review C3d-2's commit against the C3d-1 site list and the June binary.

1. **Every site in the tsv, case by case.** Rows 1-10 and 16: the header edits (`X2GUUser.h`, `X2GameUnit.h`, `X2UDPPacket.h`) must match the
   row's "C3d-2 action". Rows 11-15, 17-19 must be untouched. Check `Enum.src.lua` in `kom_contents/data036_kom/` (three rows at `+83..85`,
   `USI_END` `+86`) against June's `USI_BOUNDING_*` = `GUSI_END + 115 / 116 / 117` (relative order only).
2. **Completeness, again, with the compiler.** The Done note's warning diff compared flag off vs flag on by file:line. Note that
   `cl`'s reported line numbers in `X2GUUser.cpp` were 35 higher with the flag on (equal to the lines added to `X2GUUser.h`) from the first
   warning on; the diff was matched with that offset. Re-run `/Zs /w14244 /w14242` on the 14 TUs (the Done note names the script) and
   look for any 16 -> 8-bit conversion on a state-id expression that is not the three intended action sites
   (`IsAvaliableActionDuringDamageReact( UCHAR )` x3) and the NPC narrowing at `X2GUNPC.cpp:7617`.
3. **Search for readers the compiler cannot see**: a state id stored in a `char` array, a `%c`/`%d` printf of `GetNowStateID()` passing a byte,
   a Lua bind, or an `X2OfflineDB` column. C3d-1 found none (row 19); confirm with a repo grep over `X2Lib`.
4. **The Lua side.** `Enum.lua` in the packed data036 holds the new rows (`grep -ac USI_BOUNDING_DOWN`). Spot-check that the literal tables
   (row 17) were left alone.
5. **Spot-check two June sites in Ghidra (read-only)**: `320348c0 @32034a79` (the 16-bit `nextState` write) and `32d5be30` (bits 1 / 2 read 2 bytes).
6. **Hygiene.** `python tools/flagcheck.py JUNE_USER_STATE_ID16 --range <first C3d-2 commit>^..HEAD` and the same for `JUNE_USER_ACTION_ID16`.
   Explain every OTHER row and deleted line.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "C3d-2 - Review" under the phase in the plan, update
the STATUS.md row, check the C3d-3 kickoff, and commit. Never launch X2_offline.exe; ask the user.
