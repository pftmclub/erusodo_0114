# June quest system - Phase Q5c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (Ghidra first), then type `/x2-phase quest-system Q5c review` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q5c done (its Done note and commit), user play-test of the Tree of El run |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_NOT_CASES`, `SERV_IRUHADEV_JUNE_QUEST_NOT_NO_LINEBREAK` |
| Log tag | none (client UI only); `python tools/logs.py --grep "QUEST "` for the emulator's quest lines |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q5c - UPDATE_QUEST_NOT: June's quest-window handler vs March's state handler" "Q5c - Done (2026-10-20, Opus 5.5 high)"
grep -n "(Quest Q5c)" june2017_re/funcmap.tsv
grep -n "^| 372 \|^| 373 " JUNE_BUGS.md
```
Then `X2Lib/X2State.cpp` `Handler_EGS_UPDATE_QUEST_NOT` (both flags' blocks).

## Task

Q5c diffed June's live progress handler `323e08c0` against March's `CX2State` handler and ported two differences. Second reading, in Ghidra
(`disassemble_function`, not only the decompile):
- **The in-game jump table.** Re-decode `323e15a0` / `323e1584` (SQT - 2, bound 0x1a) from the image: cases 2 / 8 / 0xb / 0xc / 0x19 / 0x1a / 0x1c, 0x16 / 0x17 to the
  default `323e116a`, 0x24 out of range. Confirm each case's count field (+0x34 / +0x80 / +0x74 / +0x80 / +0x6c) is the March member the port compares
  (`m_iKillNum`, `m_iDungeonClearCount`, `m_iPVPKill`, `m_iDungeonClearCount`, `m_iPVPPlay`), and that June's SQT numbers equal March's enum there.
- **Case 8** (`@323e1026`): text then string free, no `3214f020` / `32150230`. JUNE_BUGS 373.
- **NPC_HUNT** (`@323e0ca3`): no call between `32337db0` and `322db770` other than the game-handle reads. JUNE_BUGS 372.
- **The "equal" rows**: `ResetQuestUI( 1, 0, 1 )`, `UpdateQuickQuestDLG( 0, 1 )`, `+0x54` = `m_bUpdateQuest`, `MakeSubQuestString`'s 5th argument = `bShowCount`,
  the PushText constants, the now-state 0xe / 0x15 tail.
- **The tutorial-guide step** is routed to june-dungeons C3z-4t, not ported: check that the STATUS row carries it.
- **The 0x167 / 0x27c verdict**: the note keeps Q4e's 0x27c. Check the new evidence (identical deserializers `320e9e60` / `320e9c70`, the state copy's June-only
  clear types) and say whether anything changes it.

Done = PASS / findings, with fixes under the same flags. No data change.
