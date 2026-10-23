# June transcendence - Phase T5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T5 review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | T5 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE` (+ `SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE_DEBUG`, retired here) |
| Log tag | `[JUNE_TRANSCEND] ENGINE` (`python tools/logs.py --grep "JUNE_TRANSCEND] ENGINE"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Starting context" "Phase T5 - Transcended combat engine" "T5 - Done"
grep -n "Transcend T5" june2017_re/funcmap.tsv
```

## Task

Review T5's commit against June in Ghidra (read-only):
- **Awakening.** `31ffd490` calls `31fc8c20` only in the gauge <= 0 branch; `31fc8c20`'s roll is vslot `0x1cc( 0 )` < 0.5 (`3343777c`) and both
  BFIs go through vslot `0x21c` with args ( 0, 1, 0, 1 ). `3206eaf0 @3206fed1`: the re-roll test reads `31fce5f0` and the `fcom` / `test ah, 5` /
  `jp` means "roll when the timer is <= 0". Check `31fce5f0`'s BTIs (0x336 / 0x337, Ain 0x3c6-0x3c8).
- **ApplyHyperModeBuff** `32298590`: BFI 845 (`@322988d3`), the WakeUp name (`33488078`), `32296f30( h, 1, 0 )`, and that the transcended branch
  stores no handle in `+0x9f8` (`@32298ab4` against `@32298c4d`), which corrects the S6 / plan text. The Rena, customize-set and socket parts are
  routed or unchanged (Done note).
- **RenderName** `3201a8f0`: the 41450 / state 0xb colour case, the 24.0 shift (`33455c6c`) after a drawn rank, `ICON_TR`'s draw arguments
  (`31ff6920`), and that dropping the couple line is June's (no such block in `3201a8f0`).
- **Numbers.** `322ece70`'s 1.2 / 1.3 (`3344ce94` / `33472a10`) at March's `fHyperModeStatScale`; the `31fceab0` gate at `X2DamageManager.cpp`
  tests `== 1`; `328d6c90` picks the `+0x30` map for transcendence != 0 and use level (`+0xd1`) >= 70 (`JUNE_BUGS` 416 is the `+0x24` compare).
- **Data.** If the user let T5's data change through: `Enum.lua` has `UC_150 = 150`, data036 `CharacterStatTable.lua` is June's ljconv bytecode
  and its manifest/ledger rows are right; the log shows `TRANSCENDENTAL_EQIP_POSITION_RELATIVE_STAT_TABLE loaded 5 row(s)`.
- **Retire `_DEBUG`.** Remove the `SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE_DEBUG` define and its blocks after reading the play-test lines.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE --range <T5 commit>^..<T5 commit> --with SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE_DEBUG`;
paste its `files:` / `in this flag's files:` / `RESULT:` lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "T5 - Review" into
the plan doc, update the T5 row of `STATUS.md`, check that `kickoffs/june-transcend/T6.md` still reads right, and commit.
