# June dungeon result - Phase R3d - Retry flow fixes

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-result R3d` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R3c done 2026-10-22: its Done note's port list is this phase's spec. First check STATUS for a running row on `X2StateDungeonGame.cpp` / `X2UIDungeonResultAdapter.cpp` / `KTDGGFXManager.cpp` / `KTDGUIDialogManager.cpp` |
| Flags | new `SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL` (item 1) and `SERV_IRUHADEV_JUNE_RESULT_STATE_RESET` (item 2), each with an `#error` guard (R3c names the prerequisites); item 0 under `SERV_IRUHADEV_JUNE_RESULT_DEBUG` |
| Log tag | `[JUNE_RESULT] FLOW` (`python tools/logs.py --tag JUNE_RESULT`) |
| Review | `kickoffs/june-result/R3-review.md` runs after this phase and covers R3 + R3b + R3c + R3d |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Rules" "Phase R3d - Retry flow fixes (user, 2026-10-22)" "Phase R3c - Retry flow investigation, read-only (user, 2026-10-22)"
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "R3c - Done (2026-10-22, Opus 5.5 high; read-only, no code, no build)"
grep -a "(Result R3c)" june2017_re/funcmap.tsv
```

and `june2017_re/result_r3c_map.tsv` (the rows marked `missing` / `differs`). Code to read before editing: `KTDXLIB/KTDGGFXManager.cpp:685-707`
(owner link, function handler) and `:2115-2296` (`CKTDGGFXResultBridge`), `KTDXLIB/KTDGUIDialogManager.h:136-146` + `.cpp:1528-1556` (hide mode),
`X2Lib/X2UIDungeonResultAdapter.cpp:18-140` (adapter callbacks), `X2Lib/X2StateDungeonGame.cpp:142-155` (ctor) and `:481-487` (dtor).

## Task

Do "Phase R3d": R3c's port list, item by item, at the March sites it names; the `_DEBUG` line first if R3c asked for one. No Ghidra
work: a June question the list does not answer goes to the user as "needs an Opus look". X2Lib files are often CP949: edit them with
`bytepatch.py` only.

**Hand-off checks:** clear -> widget normal; Retry -> loading -> dungeon; during play ESC -> the ED / durability leave box (June's too;
log `ESC: leave box ( result mode 0 )`); clear again -> widget normal (log `init: ... kept` or `attached`, `notifyInitialize`, `fill`);
ESC on the result -> June's exit box ("Do you want to exit the dungeon ?"); Cancel / ESC / Close reopen; OK -> field; a third run after
another Retry the same; DELETE on a result, then Retry -> the next run's GFx is visible (item 2's `+0x108`).

**Done gate:** as in the section.
