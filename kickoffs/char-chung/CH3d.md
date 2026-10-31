# June character Chung (Stage 7) - Phase CH3d - Hyper actives and transcendence rows

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH3d` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3c review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_TRANSCEND` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`) |
| Review | `kickoffs/char-chung/CH3d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH3d - Done (2026-10-28, Sonnet 5.5)" "CH3d - Review (2026-10-28, Opus 5.5): PASS, 1 fix (transcended 160by160 pictures)"
```
Then the 7ao / 7t rows of `ch0_checklist_rows.tsv` and the `CH3d` rows of `ch0_state_skill.tsv`.

## Task

Do the CH3d row of the plan's CH3 table: hyper actives (type 13, 6 rows), the 3 `SHOW_HYPER_CUT_IN_TIME` states (7t), and the transcendence rows
(Aisha's "Template for Stage 7", "Transcendence rows every class port carries"): 110 / 111 / 117 on `CX2OfflineServer::TranscendCheck`'s allow-list,
`LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Chung.lua`, 7ao whole (hyper effect start `32c063d0`; the four base helpers),
`SPRT_HYPER_CLASS` skills and `TRANSCENDENTAL_SKILL_DATA` rows, `GetHyperModeTableName` and the `HQ_Cutin_CIP/CDC/CTT[_1]_Transcendence` names,
`list_TR_*` rows in `GetCharacterImageName` with the picture sheet read from June's row pushes, `POPUP_TR_*` textures present (stage if not).
`python tools/skillicons.py check <Chung's skill-id range>` = PASS (stage what it lists, `DLG_SKILL_Passive_*` included).
- Hand-off: `python tools/elsdb.py settranscend <unit> 1` on one unit per second job (after a backup), slots A-5 / B-5, each hyper-class skill, the
  transcended picture and cut-in.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0

(CH0 fills this: the filters with real column numbers, function counts, addresses, checklist rows, callees, any split.)
