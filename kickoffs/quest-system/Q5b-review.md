# June quest system - Phase Q5b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q5b review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q5b done (its Done note and commits), user play-test of the quest scenes |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_SCENE` |
| Log tag | `[JUNE_QUEST] SCENE` (`python tools/logs.py --tag JUNE_QUEST`) |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Starting context" "Phase Q5b - Quest UI: scenes, clear window, scene info" "Q5b - Done"
```
Then `june2017_re/quest_plan/q0b_ui_map.tsv` (rows targeted Q5b) and the new code at the end of `X2Lib/X2UIQuestNewJune.cpp` (`JunePlayQuestScene`).

## Task

Q5b ported June's scene starter `323cb2e0` / lookup `323cb190` onto G3's `CX2GFXEventScene`, the quest BGM `3232df20` / `3232e000`, the deferred
scene (`g_pInstanceData +0xaf8 / +0xafc`) and the four call sites. The port was read from a Sonnet session's decompile + disassembly, so the review
is a second reading. What it could get wrong:
- **`323cb2e0`** (re-read in Ghidra, `disassemble_function`, not the decompile): the order inside each mode block (ment register `3226ac80` -> Play ->
  BGM), that the BGM test sits inside the "name non-empty" branch, the additional-quest loop (names and ment vectors cleared, frames NOT reset, both
  registered then both played, BGM after), the first lines clearing `+0xaf8 / +0xafc` before the templet test.
- **`323cb190`**: DEFAULT_* first, then the unit-type row overriding start (name + ment + frame) only when its start name is non-empty and end only when
  its end name is. The key is the unit TYPE (first dword of the unit templet), checked against `CHAR = UNIT_TYPE.UT_*` in `Quest_Event_Scene_Info`.
- **`3226ac80`** = map::insert (name kept if present): check the lower_bound / compare / insert branch. Also that nothing else writes `+0xc0` and that
  G3's `GetMentID` (index = frame - 1) is how June reads the ids.
- **BGM**: `3232df20` needs `g_pTFieldGame` with a world, a non-empty name and flag `+0x148 == 0`; `3232e000` needs the saved name non-empty and the flag;
  only caller is `3226b734` (End, after the cursor reset). JUNE_BUGS 353.
- **Call sites**: NEW_QUEST_NOT `323e4cb0` @323e4e0b-323e4e54 (state 5 = deferral; **March has no ELSWORD_NEW_BEGINNING, so the slot is XS_SERVER_SELECT**:
  is a NEW_QUEST_NOT ever received there offline? if June's 5 is not the beginning, say so), the epic `+0x70` write (JUNE_BUGS 352),
  QUEST_COMPLETE_ACK / ALL_COMPLETED mode 1, Remind mode 2. The deferred reader in StateChange `31d88bf0` @31d896bd-31d896fb: the two June blocks were
  mapped to `XS_VILLAGE_MAP` / `XS_BATTLE_FIELD` by their object sizes (0x630 field game / 0x5f8 state + 0x680 game): confirm with the case labels.
- **Lua scenes dropped** (the kickoff's re-proof): a whole-image sweep `june2017_re/dispsweep.py 1d8 1f4` + the 64 callers of `GetQuestTemplet` found no read
  of `m_wstrStartScene` / `m_wstrEndScene` (the Done note lists the 8 intersecting functions and why each is another struct). Re-check the verdict.
- **Clear window**: `323d2c60` was read whole against March `OpenQuestRewardWindow`; the claim is "identical except an unreachable dialog-manager `+0x111`
  gate". Re-read it, and confirm `+0x111` has no writer. `DLG_UI_NEW_Quest_Clear` is June's `ljconv` bytecode (data034).

Hygiene: `python tools/flagcheck.py JUNE_QUEST_SCENE --range <first Q5b commit>^..HEAD`. Expected: Always.h OTHER (peers' defines in the same hunk range), `X2UIQuestNewJune.cpp`
OTHER (it sits in the `JUNE_QUEST_UI` file). Explain every other row.

Play-test log check: `python tools/logs.py --grep "JUNE_QUEST SCENE"` and `--grep "JUNE_GFX SCENE"` - the `[JUNE_QUEST] SCENE` lines name the quest, mode, scene names and frames.
