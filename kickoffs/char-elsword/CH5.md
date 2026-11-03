# June character Elsword (Stage 7) - Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH5` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1 review; may run beside CH2-CH4 (different files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI` |
| Log tag | `[JUNE_CHAR]` (`ELSWORD UI`) |
| Review | `kickoffs/char-elsword/CH5-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Planning evidence (2026-11-03)" "Phase CH5 - UI and data: UnitTemplet rows, cut-ins, dialog pieces"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH5 - Done (2026-11-02)"
```
Then `june2017_re/char_elsword/plan_june_side.md` §7 and the `CH5` rows of `ch0_checklist_rows.tsv` (7t, 7ah).

## Task

Do "Phase CH5": Elsword's 7 `UnitTemplet.lua` rows become June's (core-list BOM source; `luasrc.py refresh` after; every other row and June's
`UC_150` row stay as they are): `m_fLobbyRotationY` (35 / 25 / 25 / 10 / 3 / 10 / 10), `m_HyperCutIn` `HQ_Cutin_ELK/ERS/EIS_Transcendence` for
100 / 101 / 112, names `STR_ID_4641` / `4642` / `17425` (CH0 confirmed the strings exist). The cut-in table `X2GUElsword_SwordMan.cpp:14-44` and
the InitState index switch `:873-898` become June's names and layout under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_UI` (CP949: `bytepatch.py`). 7ah and the dialog pieces CH1q did
not take. Pack data036, ask before the copy. Hand-off: the select screen with all nine March characters, then each Elsword job's skill and hyper
cut-in.

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

(CH0 fills this: filters with real column numbers, function counts, addresses, any split.)
