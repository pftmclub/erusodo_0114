# June character Rena (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH5` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_UI` |
| Log tag | `[JUNE_CHAR]` (`RENA UI`) |
| Review | `kickoffs/char-rena/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Planning evidence (2026-11-07)" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH5 - Done"
```
Then `june2017_re/char_rena/plan_june_side.md` and the `CH5` rows of `ch0_checklist_rows.tsv` (7t, 7ah).

## Task

Do "Phase CH5": Rena's 7 `UnitTemplet.lua` rows become June's (core-list BOM source; `luasrc.py refresh` after; every other row stays as it is):
`m_fLobbyRotationY` (30 / 30 / 25 / 1 / 30 / 20 / 10 for 3 / 12 / 13 / 24 / 102 / 103 / 114), `m_HyperCutIn` for 102 / 103 / 114, the names `STR_ID_4783` /
`_4784` / `_17429` for 102 / 103 / 114 (CH0 confirms the strings exist; March has `STR_ID_616` on all 7). The cut-in table `X2GULire_ElvenRanger.cpp:50-82`
and the InitState index switch `:1672-1715` become June's names and layout under `SERV_IRUHADEV_JUNE_CHAR_RENA_UI` (CP949: `bytepatch.py`; 7t cut-in
`.gfx` names: June's 33 `HQ_CutIn_Rena_*.gfx` and 3 `HQ_Cutin_R{WS,GA,NW}_Transcendence.gfx` are already in kom_contents). 7ah and the gauge pieces CH1q
did not take. Pack data036, ask before the copy. Hand-off: the select screen with all nine March characters and no new error, then each Rena job's skill
and hyper cut-in, and the three second-job names showing June's strings.

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

(CH0 fills this: filters with real column numbers, addresses, counts, any split.)
