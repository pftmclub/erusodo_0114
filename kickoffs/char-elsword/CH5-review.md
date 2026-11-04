# June character Elsword (Stage 7) - Phase CH5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH5 review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH5 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI` |
| Log tag | `[JUNE_CHAR]` (`ELSWORD UI`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH5 - Done (2026-11-04)"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH5 - Review (2026-11-02, Opus 5.5)"
```
Then `june2017_re/char_elsword/ch0_unittemplet_elsword.tsv` (the 13 changed keys) and the `CH5` rows of `ch0_checklist_rows.tsv`.

## Task

Check CH5 against June (read-only in Ghidra and the exe bytes). In order of risk:
- **The table** `333ac120` (static init) / `337aecc0`: 12 `SkillCutIn` entries at `337aecc0 + i * 0x30`, 4 sets of 3 at stride 0x90. Read the string each entry
  pushes (`33525d2c` = `HQ_CutIn_Elsword_01`, `d04` = `_02`, `cdc` = `_03`, `cac` = `_LK_01`, `c7c` = `_RS_01`, `c4c` = `_IS_01`) and the constructor stores
  (`334367ac` / `33435598`, uninitialised z = March's `SkillCutIn( name )` at `X2GUUser.h:830`). Does the port's order (set 0 = 01 / 02 / 03, 1 = LK x3, 2 = RS x3,
  3 = IS x3) equal June's row for row? The set index: CH1's InitState switch (0 / 1 / 2 / 3 for the four groups) against June `32d23ff0`.
- **The reader** `32d09050` (CH1, `ShowActiveSkillCutInAndLight`): entry `cutIn[uiCutInIndex_]`, the 0x8b3 override; the CH5 log line prints after the override.
- **`UnitTemplet.lua`**: block-wise compare against June's `UnitTemplet.src.lua` on `unitClass` (BOM and CRLF normalised only): the 7 Elsword blocks must equal June's text, the
  other 48 the pre-phase kom (`kom_backup_tools/data036.kom.<stamp>` is the backup the copy writes). `luasrc.py ledger --check`, `core --check`, `luabind.py check UnitTemplet`.
  The three `name` ids `STR_ID_4641` / `4642` / `17425` and the three `m_HyperCutIn` names exist (`.gfx` in data195).
- **Dev pieces kept**: `ElswordWSPCheat` and `RenderName`'s `WSP: %d` (no June counterpart) are left as March dev tools by CH5; agree or drop them under `_UI`.
- **Routing**: the guide dialog `DLG_Transcendental_Change_Guide_Elsowrd.lua`, `LevelUpEvent.lua` and the `POPUP_TR_*` pictures are CH3d's (its kickoff names them).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI --range <CH5 commit>^..<CH5 commit>`;
`python tools/junediverge.py --check-all` = `RESULT: PASS`; `enumscan.py --check X2Lib/X2GUElsword_SwordMan.cpp`. Paste the summary lines.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH5 - Review" into the plan doc, update the
phase's STATUS.md row, check that CH6's kickoff names the CH5 walk (the Elsword select-screen pass and the hyper cut-in check), and commit.
