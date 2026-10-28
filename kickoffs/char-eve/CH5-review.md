# June character Eve (Stage 7) - Phase CH5 review - UnitTemplet rows and cut-in table

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH5 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH5 done and play-tested |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_UI` |
| Log tag | `[JUNE_CHAR]` (`EVE UI`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH5 - Done (2026-10-28, Sonnet 5.5)"
```
Then `june2017_re/char_eve/ch0_unittemplet_eve.tsv` and the `CH5` rows of `june2017_re/char_eve/ch0_checklist_rows.tsv`.

## Task

Review the CH5 port against June (Ghidra for `3297b840`, `333a4940`, `333a47b0`; `june_src_fixed/data036_kom/UnitTemplet.src.lua` for the rows):
- **UnitTemplet.** Eve's 7 blocks in `kom_contents/data036_kom/UnitTemplet.lua` equal June's blocks (text, per `unitClass`), the other 48 blocks equal the pre-phase file (`git show` the ledger row or compare the live kom backup `kom_backup_tools/data036.kom.20261028_185657`), `luasrc.py ledger --check` PASS.
- **Table.** `s_SkillCutInSet` = `337abc00`: 4 sets x 3 in the order `01 02 03 / NM_01 x3 / EP_01 x3 / BS_01 x3`; the `SkillCutIn( name )` constructor size / position equal what `333a4940` stores (`[334367ac]` x3, `[33435598]`); the event pair `337abba0` = `CASH_21`, `337abbd0` = `CASH_22`, even `rand()` -> the first (`3297b9a7`); the `#else` still holds March's table.
- **Set index.** `m_iSkillCutInSetIndex` is 0 / 1 / 2 / 3 for 5, 18, 19, 26 / 108 / 109 / 116 (`329ba9c0`; Electra keeps 0), so no set index reaches past the 4 sets.
- **Play-test lines.** The `EVE UI cut-in set` and `EVE UI hyper cut-in class` lines the hand-off lists.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_UI --range <first CH5 commit>^..HEAD`; `enumscan.py --check X2Lib/X2GuEve.cpp`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH5 - Review" into the plan doc, update the CH5 STATUS row, check `kickoffs/char-eve/CH6.md`, and commit.
