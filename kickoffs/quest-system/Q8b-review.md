# June quest system - Phase Q8b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q8b review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q8b done (its Done note and commit), user play-test of the quest window's Video tab |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB` |
| Log tag | `[JUNE_QUEST] MOVIE` (`python tools/logs.py --grep "JUNE_QUEST] MOVIE"`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q8b - The quest window's scenario-movie tab" "Q8b - Done"
```
Then `june2017_re/funcmap.tsv` rows tagged `(Quest Q8b)`, JUNE_BUGS 360-365, and the new code: the `JUNE_QUEST_MOVIE_TAB` block at the end of
`X2Lib/X2Main.cpp` (before `LoadLuaEnum`) and at the end of `X2Lib/X2UIQuestNewJune.cpp`, plus the flag's inserts in `X2UIQuestNew.cpp`,
`X2QuestManager.cpp` (`SetUnitQuest`), `X2Unit.cpp` (`Reset`) and `X2StateMenu.cpp`.

## Task

Q8b ported the Video tab from a single Opus reading. Re-read in Ghidra (raw disassembly: `june2017_re/disasm_range.py`, not the decompile):
- **List `323dfc00`:** the slot loop (no id step without a button), the id-10 skip (`g_pInstanceData +0xa18` codes 0x8d4 / 0x8d5), the seen branch
  (texture +0x30 with the ScenarioMovie_01 default, key +0x4c, `"_Overlay"` for Over and Down, `+0x1d0` = `m_GuideDesc` L"", `+0x35e` =
  `m_bEnableClick`), the NEW mark (`31d75b80`, `ChangeSequence( 1 )`, `31c1a5b0( 1, 1, 0, 1 )` = `SetFlicker`), the locked branch (`Overlay_Lock`
  Normal / Over only, string 31168 / `323ddb00` / hide), and the HorizonLine / Village / Area tail (rows = `+0x778 - skip - +0x770 + 1`).
- **Lock text `323ddb00`:** the dungeon / quest split, `+0x184 == 2` = `DT_HENIR`, 33030 / 33031, the `"LL"` and `"iL"` argument order.
- **Cases 41038-41042** (jump table `323e4504`): the refusal order (`32481fa0` left out, `32482650 == 2` re-expressed as `GetIsBossRaidCurrentField`),
  `323c8820` = `InitUIQuestSlotInfo`, `323c9200` = `InitRewardItemSlot`; play and hover index maths.
- **Branches:** scroll buttons (> 13), navi (> 12, the -0.68 formula, list( 0 )), wheel `323e45c0` (> 12), `UpdateNavi` (< 13), ResetQuestUI's
  early return, the ctor values, and every June caller of the hide `323ca6d0` against the port.
- **Seen list:** `31d79ac0` at the end of `SetUnitQuest`, `32362730 @323629d7` in `CX2Unit::Reset`, the clear `324625b0 @32462657` (vslot 155 =
  `ProcessStateChangeUnitSelectWithOptionWindow`), and the `+0x84c` sweep (`dispsweep.py 84c 850 854`).

Hygiene: `python tools/flagcheck.py JUNE_QUEST_MOVIE_TAB --range <Q8b commit>^..<Q8b commit>` (peers share these files; `..HEAD` can time out).
Explain every row; a FAIL or ERROR is a finding.

Play-test log check: `python tools/logs.py --grep "JUNE_QUEST] MOVIE"` - the seen-list lines at character select / SetUnitQuest, `tab open`,
`list from 1: N seen ...`, `tab play <id>`, then Q8's start / end / stop lines with no complete request (quest 0).
