# June dungeon result - Phase R3c - Retry leaves stale state; the exit box's text

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-result R3c` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R3b (done 2026-10-22, commits `b7b42dba` + `03f59bdd`). First check STATUS for a running row on `X2State.cpp` / `X2StateDungeonGame.cpp` / `X2Main.cpp` / `KTDGGFXManager.cpp` |
| Flags | fixes under `SERV_IRUHADEV_JUNE_RESULT_FLOW` or `_RETRY_DIRECT` (a new `_JUNE_RESULT_*` flag for a separate change) |
| Log tag | `[JUNE_RESULT] FLOW` (`python tools/logs.py --tag JUNE_RESULT`) |
| Review | none of its own: `kickoffs/june-result/R3-review.md` runs after this phase and covers R3 + R3b + R3c |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Scope fence" "Rules" "Phase R3c - Retry leaves stale state; the exit box's text (user, 2026-10-22)" "R3b - Done (2026-10-22, Opus 5.5 high; flags \`_JUNE_RESULT_FLOW\` (Tab / F8), \`_DIALOG_MODAL\`, \`_RETRY_DIRECT\`, \`_OLD_EXP\`)"
grep -a "(Result R3)\|(Result R3b)\|(Result R1)\|(Result R2)" june2017_re/funcmap.tsv
```

R3 / R3b's code: `X2Lib/X2UIDungeonResultAdapter.cpp` (the `SERV_IRUHADEV_JUNE_RESULT_FLOW` block: adapter create / delete / recreate,
mode machine, retry, retry ACK), `X2State.cpp` (ctor / dtor resets, `HandleMsgByESC`), `X2StateDungeonGame.cpp`, the emulator's
`Handler_EGS_JUNE_DUNGEON_RETRY_REQ` (`X2Lib/Offline/Handlers_Room.cpp`). The evidence run is the R3b play-test, `ErrorLog.txt`
16:23-16:26 (overwritten by the next launch: the plan section quotes the lines).

## Task

Do "Phase R3c", items 1-3:
1. **June's retry flow end to end:** what June closes, deletes and resets from the retry ACK to the next dungeon start; port what March
   is missing so a retried run starts clean.
2. **The second widget gets no `notifyInitialize`:** prove why from the log (add a `_DEBUG` line if needed) and fix it the June way.
3. **The exit box:** check June's string IDs (`39778` / `43515` / `43487`, `31f17e60`) against March's table, and that ESC during play after
   a retry gets March's own leave dialog, not the result box.

Opus delegates the reads, greps, build and log reads. X2Lib files are often CP949: edit them with `bytepatch.py` only.

**Hand-off checks:** clear -> widget normal; Retry -> loading -> dungeon; during play ESC -> March's leave dialog (the ED / durability
warning is right here); clear again -> widget normal (log `notifyInitialize`, `fill`); ESC on the result -> June's exit box with the
result text; Cancel / ESC / Close reopen; OK -> field; a third run after another Retry the same.

**Done gate:** as in the section.
