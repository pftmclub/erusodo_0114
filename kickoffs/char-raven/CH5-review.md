# June character Raven (Stage 7) - Phase CH5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH5 review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH5 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI` |
| Log tag | `[JUNE_CHAR]` (`RAVEN UI`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH5 - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH5 - Review (2026-11-05, Opus 5.5)"
```
Then `june2017_re/char_raven/ch0_unittemplet_raven.tsv` (the 11 changed keys) and the `CH5` rows of `ch0_checklist_rows.tsv`.

## Task

Check CH5 against June (read-only in Ghidra and the exe bytes). In order of risk:
- **The table** `333aa400` (static init) / `337ae1d0`: 12 `SkillCutIn` entries at `337ae1d0 + i * 0x30`, 4 sets of 3 at stride 0x90. Read the string each entry pushes
  (`33515268` `HQ_CutIn_Raven_01`, `5244` `_02`, `5220` `_03`, `51f4` `_BM_01`, `51c8` `_RF_01`, `519c` `_VC_01`) and the constructor stores (`334367ac` / `33435598`,
  uninitialised z = March's `SkillCutIn( name )`). Does the port's order (set 0 = 01 / 02 / 03, 1 = BM x3, 2 = RF x3, 3 = VC x3) equal June's row for row? The set index: CH1's InitState
  switch (`X2GURaven.cpp:855-890`) against June `32c66fe0` (decode its jump table from the exe bytes if Ghidra's body stops short).
- **The reader** `32c3e450` against the port's `ShowActiveSkillCutInAndLight` (`_UI` branch): gate order, `cutIn[ index + set * 3 ]`, the item override (not ported), the 0x8b3 pair `337ae170` /
  `337ae1a0` (even `rand()` -> CASH_21), the `ActiveSkillShow` arguments (bone name, `IsHyperState()`); the CH5 log line prints after the swap. Is `ShowActiveSkillCutInAndLightByScript` (bone `Dummy2_Lhand`,
  `m_iSkillCutInSetSubIndex`) June's `32c3e770`-area override (CH1k)?
- **`UnitTemplet.lua`**: block-wise compare against June's `UnitTemplet.src.lua` on `unitClass` (BOM and CRLF normalised only): the 7 Raven blocks must equal June's text, the other 48 the pre-phase kom
  (`kom_backup_tools/data036.kom.20261106_155516` is the backup the copy wrote). `luasrc.py ledger --check`, `core --check`, `luabind.py check UnitTemplet`.
  `STR_ID_17431` and the three `m_HyperCutIn` names exist (`.gfx` in data195, same case as the rows).
- **Routing**: the guide dialog `DLG_Transcendental_Change_Guide_Raven.lua`, `LevelUpEvent.lua` and the `POPUP_TR_B_MASTER / R_FIST / V_COMMANDER` pictures are CH3d's (its kickoff names them).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RAVEN_UI --range <CH5 commit>^..<CH5 commit>`;
`python tools/junediverge.py --check-all` = `RESULT: PASS`; `enumscan.py --check X2Lib/X2GURaven.cpp`. Paste the summary lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH5 - Review" into the plan doc, update the
phase's STATUS.md row, check that CH6's kickoff names the CH5 walk (the select-screen pass and the hyper cut-in check), and commit.
