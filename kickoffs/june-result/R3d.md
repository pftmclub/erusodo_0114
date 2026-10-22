# June dungeon result - Phase R3d - Retry flow fixes

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-result R3d` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R3c (read-only investigation) done: its Done note's port list is this phase's spec. First check STATUS for a running row on `X2State.cpp` / `X2StateDungeonGame.cpp` / `X2Main.cpp` / `KTDGGFXManager.cpp` |
| Flags | fixes under `SERV_IRUHADEV_JUNE_RESULT_FLOW` or `_RETRY_DIRECT`, or the new `_JUNE_RESULT_*` flag R3c names |
| Log tag | `[JUNE_RESULT] FLOW` (`python tools/logs.py --tag JUNE_RESULT`) |
| Review | `kickoffs/june-result/R3-review.md` runs after this phase and covers R3 + R3b + R3c + R3d |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Rules" "Phase R3d - Retry flow fixes (user, 2026-10-22)" "Phase R3c - Retry flow investigation, read-only (user, 2026-10-22)"
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md --list    # then pull R3c's Done note by its exact heading
grep -a "(Result R3c)" june2017_re/funcmap.tsv
```

and `june2017_re/result_r3c_map.tsv` (the rows marked `missing` / `differs`).

## Task

Do "Phase R3d": R3c's port list, item by item, at the March sites it names; the `_DEBUG` line first if R3c asked for one. No Ghidra
work: a June question the list does not answer goes to the user as "needs an Opus look". X2Lib files are often CP949: edit them with
`bytepatch.py` only.

**Hand-off checks:** clear -> widget normal; Retry -> loading -> dungeon; during play ESC -> March's leave dialog (the ED / durability
warning is right here); clear again -> widget normal (log `notifyInitialize`, `fill`); ESC on the result -> June's exit box with the
result text; Cancel / ESC / Close reopen; OK -> field; a third run after another Retry the same.

**Done gate:** as in the section.
