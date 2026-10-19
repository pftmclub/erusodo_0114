# June skill system core - Phase S7h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7h review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7h done (its Done note and commits) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_SKILL_CLASS_CHANGE` (`Always.h` + `ServerDefine.h`), `SERV_IRUHADEV_JUNE_SKILL_RELATION_UNEQUIP`, `SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP`, `SERV_IRUHADEV_JUNE_SKILL_CLASS_ITEM_CHOICE` |
| Log tag | `[JUNE_SKILL] CLASS` (`python tools/logs.py --tag JUNE_SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7h - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7h`; `JUNE_BUGS.md` rows 347-348.

## Task

Review Phase S7h against Ghidra (read-only), from the raw disassembly:
- The packet: Get `320ed4e0` (field order and types; `+0x2a` = `KCompleteQuestInfo`, `+0x3a` = `KQuestInstance`, `+0x3bf` = `KSkillData`) against
  `KEGS_BUY_UNIT_CLASS_CHANGE_NOT` and its Put / Get in `ClientPacket.h/.cpp`; the dispatcher case ( `31f2ab80`, table `31f2c8a4`, case 0x363 ).
- `CX2State::Handler_EGS_UNIT_CLASS_CHANGE_CASH_ITEM_NOT` ( `X2State.cpp` ) against `31f276b0` call for call: the double cash-shop test, the state branch
  ( June 0x15 = BATTLE_FIELD by name, not number ), the tree block ( `+0x494`: merge `32892410`, `32891560`, `32895c40( 0, 1, 0, 0 )` read as March
  `Reset( false, false, true, false, false )`, `328929e0`, `32891560` ), `3220f740`, `32892370( { 900, relation state } )`, the UI block, inventory / quest,
  the gage pair ( vslots 0x4c / 0x58 ), `3223c7f0` before `3235a160`, and the two calls left out ( `32161070`, vslot 0x104 ).
- `ResetJuneAcquiredSkills` against `328929e0` ( the acquired map = the page in use, S7's equivalence ).
- `UnequipRelationSkill` against `3220f740` ( bar B + 5 ).
- `CX2ClassChangePopup::SetShow` mode 0 against `324886b0` ( `@32488b01` scale, the `+0xd84` write left out ), `JuneGetClassChangeDialogName` against
  `32486e60` ( spot-check 6 cases and the default bounds ), `PlaySoundChangeClass` against `32486c10` / `32486ae0`.
- `RemoveUnitClassChangeItemInChoice` / `GetUnitClassChangeItemID` / `JuneGetUnitClassChangeItemID2` against `3223c7f0` / `32227080` / `32226c60`
  ( spot-check 8 ids per table ).
- The emulator ( `Handlers_Shop.cpp` ): the acquired list is the whole list after the grant, the unsealed list is sent.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..<last phase commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7h -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (S7m), and commit.
