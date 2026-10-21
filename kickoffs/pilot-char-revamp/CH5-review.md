# June character Aisha (Stage 5 pilot) - Phase CH5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH5 review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH5 done (its Done note and commit) and, for the in-game legs, the user's CH5 play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_UI` (on top of `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`) |
| Log tag | `[JUNE_CHAR]` (`python tools/logs.py --tag JUNE_CHAR`, lines `ARME UI`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH5 - UI and data: UnitTemplet, cut-ins, dialog and tree-UI class pieces" "CH5 - Done"
```

## Task

Review Phase CH5 against the Done gate:
- **`UnitTemplet.lua` (core-list source):** the 7 `UC_ARME_*` blocks are June's text and every other block is byte-identical to the pre-edit file
  (`scratchpad` copy is gone: compare against `kom_contents_bytecode\data036_kom\UnitTemplet.lua` decompiled, or the ledger's reference);
  `luasrc.py ledger --check` PASS; the data036 pack lists only `UnitTemplet.lua` as changed.
- **The cut-in table:** `s_SkillCutInSet` against June `333ab660` (4 sets of 3: `_01/_02/_03`, `EM_01` x3, `VP_01` x3, `DW_01` x3; 30 rows) and
  `s_SkillCutInSetEvent8b3` against `333ab4d0` (CASH_21 / CASH_22); the `.gfx` of every name exists in the deployed koms (data152 / data195).
  The `0x8b3` branch against `32cc9680` (even `rand()` -> CASH_21). Size 512x512 and position (256, 512) = `32078b70` (check the two float
  constants `334367ac` / `33435598` in Ghidra).
- **Routed:** the cut-in item override (`31fd8a50` + `3264e800`, `CX2CustomizeManager`) -> Stage 12 bullet; 7ah / 7aj were CH0-assigned to CH4.
- **Play-test result** (`logs.py --tag JUNE_CHAR`): `ARME UI cut-in` names are June's; `ARME UI hyper cut-in` names `HQ_Cutin_A*_Transcendence` for
  104 / 105 / 113 only if the class script sets `SHOW_HYPER_CUT_IN_TIME` (June's does); no new `KEM_ERROR` for the select screen.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARME_UI --range <first CH5 commit>^..HEAD`. Fix transcription errors under the same flag, then
`tools/build.py --deploy`. Write "CH5 - Review" into the plan doc, update the STATUS row, check the CH6 kickoff, commit.
