# June transcendence - Phase T2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T2 review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T2 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE` |
| Log tag | `[JUNE_TRANSCEND] CHANGE` (`python tools/logs.py --grep "JUNE_TRANSCEND CHANGE"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T2 - Becoming transcended: quest, notify, guide" "T2 - Done"
```

T0's map: `june2017_re/transcend/t0_readers.tsv` (owner T2 rows) and funcmap rows `Transcend T0` / `Transcend T2`.

## Task

Review T2's commit against June in Ghidra (read-only):
- **The NOT.** `31ef82f0` call by call against `X2State.cpp` `Handler_EGS_CHANGE_TRANSCENDENTAL_CLASS_NOT`: order (flag, `InitSkillTreeUI`, popup, `UpdateUI`,
  item-stat key, cash shop, `3232ecd0`), the `UpdateUI( 1, 1, 0 )` argument mapping (the class-change handler uses `( true, true, true, false )` for the same
  call), and the ACK `31ef82a0`.
- **The popup.** `324886b0` guide branch, `32486790` (class popup) and `32710710` (level-up guide) against `JuneInitTranscendGuide`: the class -> picture
  tables, the popup statics, the four buttons; the unit-type -> file table (1 Elsowrd .. 12 Ain). The popup's `+4 / +8` members are not ported: confirm no reader.
- **The level-up guide.** `32712790` cases 1537 / 1549 / 1550, `32710b60`, `32344ab0( 9, 0 )` (the loop, the class-set test, the notice box), `3222a4f0( 0 )`
  (menu type 0x1d, item 0x4663f), and the name test `32711490` in `CheckLevel` (`327141e0`). `LevelUpEvent.lua` entry 6 carries Aisha's row only.
- **The image table.** The Aisha rows of `GetCharacterImageName` against the June records (`june2017_re/funcmap.tsv`, `Transcend T2`; the 870 rows were read
  off the static initializer with the scratch script described in the Done note).
- **GetNewQuest.** `32337320 @3233743b` (the funcmap row of T0 pointed at `32336e80` by mistake).
- **Emulator.** `JuneTranscendCheck` (class 100-199, level 70, not already transcended, allow-list 104 / 105 / 113), NetError 1687 / 1688 / 1726 and their texts in
  `X2NetError.cpp`, the `/tc` handler, the quest-completion path (`m_bTranscended`, the NOT after the ACK). Stand-ins labelled in `MODS.md`.

Hygiene: `python tools/flagcheck.py JUNE_TRANSCEND_CHANGE --range <T2 commit>^..<T2 commit>`; paste its `files:` / `in this flag's files:` / `RESULT:` lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "T2 - Review" into the plan
doc, update the T2 row of `STATUS.md`, check that `kickoffs/june-transcend/T3.md` still reads right, and commit.
