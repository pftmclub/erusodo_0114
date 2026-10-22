# June dungeon result - Phase R3b - R3 play-test fixes

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-result R3b` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R3 (done, play-tested 2026-10-22). First check STATUS for a running row on `X2State.cpp` / `X2StateDungeonGame.cpp` / `KTDGGFXManager.cpp` (june-gfx) |
| Flags | fixes under `SERV_IRUHADEV_JUNE_RESULT_FLOW` (or a new `_JUNE_RESULT_*` flag for a separate change); retry decision `SERV_IRUHADEV_JUNE_RESULT_RETRY_DIRECT` |
| Log tag | `[JUNE_RESULT] FLOW` (`python tools/logs.py --tag JUNE_RESULT`) |
| Review | none of its own: `kickoffs/june-result/R3-review.md` runs after this phase and covers R3 + R3b |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Scope fence" "Rules" "Starting context" "Phase R3b - R3 play-test fixes (user, 2026-10-22)" "R3 - Done (2026-10-22, Opus 5.5 high; flag \`SERV_IRUHADEV_JUNE_RESULT_FLOW\`, log \`[JUNE_RESULT] FLOW\`)"
grep -a "(Result R3)\|(GFx G5)\|(R4c)\|(R8)" june2017_re/funcmap.tsv
```

R3's code is in `X2Lib/X2UIDungeonResultAdapter.cpp` (the `SERV_IRUHADEV_JUNE_RESULT_FLOW` block), `X2State.cpp` (`HandleMsgByESC`,
`UICustomEventProc`, `OnFrameMove`) and `X2StateDungeonGame.cpp`.

## Task

Do "Phase R3b", items 1-4:
1. **Shortcuts Tab / ESC / F8** do nothing while the result shows. Find June's handling (Tab: `GameResult.onTabKeyProcess` and its
   caller; F8: June's `DIK_F8` / action test during the result; ESC: R3's gate once the box can be seen) and port it.
2. **Layering:** the exit box and the solo retry box are drawn behind GFx UI 8 and the clicks go to the widget. Find how June draws
   and routes input to a KTDGUI dialog over the widget (flagged-only mode + after-frame dialog pass, dialog `+0x1e0`, GFx render
   order and mouse capture) and fix it.
3. **Retry restarts at once** (user decision 2026-10-22): solo and auto-party Retry sends the REQ directly, no ready box; loading screen,
   then the dungeon start. Own flag `SERV_IRUHADEV_JUNE_RESULT_RETRY_DIRECT`, June's solo box in `#else`. Do not change the emulator's
   answer (R2's stand-in) without asking.
4. **Level-cap fill:** `fill: ... level 99 ( level up )` with rate 0.000 / 0.000; compare June `324d60e0` at max level.

Opus delegates the reads, greps, build and log reads. X2Lib files are often CP949: edit them with `bytepatch.py` only.

**Hand-off checks:** Tab, ESC and F8 during the result; ESC -> exit box visible and clickable (OK leaves, Cancel closes);
widget Close -> same box; Retry solo -> loading screen -> same dungeon start; Retry with the AI party (auto-party button) -> same,
bots back; let the countdown run out once -> field.

**Done gate:** as in the section.
