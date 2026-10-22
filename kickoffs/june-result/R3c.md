# June dungeon result - Phase R3c - Retry flow investigation (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-result R3c` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R3b (done 2026-10-22, commits `b7b42dba` + `03f59bdd`). Runs alongside anything: read-only |
| Code | **none**: no source edit, no build, no deploy, no data change. Ghidra read-only (no renames) |
| Output | `june2017_re/result_r3c_map.tsv`, funcmap rows `(Result R3c)`, the Done note ending in R3d's port list, `kickoffs/june-result/R3d.md` checked |
| Next | `kickoffs/june-result/R3d.md` (Sonnet 5.5 high) codes the port list |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Scope fence" "Rules" "Phase R3c - Retry flow investigation, read-only (user, 2026-10-22)" "R3b - Done (2026-10-22, Opus 5.5 high; flags \`_JUNE_RESULT_FLOW\` (Tab / F8), \`_DIALOG_MODAL\`, \`_RETRY_DIRECT\`, \`_OLD_EXP\`)"
grep -a "(Result R1)\|(Result R2)\|(Result R3)\|(Result R3b)" june2017_re/funcmap.tsv
```

R3 / R3b's code to compare against: `X2Lib/X2UIDungeonResultAdapter.cpp` (the `SERV_IRUHADEV_JUNE_RESULT_FLOW` block: adapter
create / delete / recreate, mode machine, retry, retry ACK, exit box), `X2State.cpp` (ctor / dtor resets, `HandleMsgByESC`),
`X2StateDungeonGame.cpp` (ctor / dtor), `KTDXLIB/KTDGGFXManager.cpp` (`CKTDGGFXResultBridge`, listener, `gameDataResultFunctionInit`),
the emulator's `Handler_EGS_JUNE_DUNGEON_RETRY_REQ` (`X2Lib/Offline/Handlers_Room.cpp`). The evidence run's log lines are quoted in the
plan section (`ErrorLog.txt` has been overwritten since).

## Task

Do "Phase R3c", items 1-3, read-only:
1. **June's retry flow end to end:** what June closes, deletes and resets from the retry ACK to the next dungeon start, against March's
   path; one map row per step / member.
2. **Why the second widget gets no `notifyInitialize`:** the cause, or the `_DEBUG` line that will settle it.
3. **The exit box:** what `39778` / `43515` / `43487` read in March's table and June's; which ESC handler June reaches during play after a
   retry, and what in our port sends ESC to the result box.

Finish with R3d's port list in the Done note (June address, March site, flag, what to write), STATUS rows (R3c done, R3d ready), and
`kickoffs/june-result/R3d.md` updated if the port list changes its read list. Opus delegates the greps and reads; no build.
