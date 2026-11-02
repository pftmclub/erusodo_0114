# June character Lu/Ciel (Stage 8) - Phase CH0 - Lu/Ciel class map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH0` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything, beside NU0) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each CH1 review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Context" "Planning evidence (2026-11-02)" "Phases" "Phase CH0 - Lu/Ciel class map (read-only)" "Phase CH1 - Deploy unit: the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH0 - Chung map (read-only)" "CH0 - Done (2026-10-31)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH0c - Done (2026-10-20)" "Template for Stage 7"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do "Phase CH0" step 0 and items 1-10. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known addresses, the capstone
runs and the scripted tools; you keep the verdicts, the names, the sub-phase assignment and the stat-row question.

- **Step 0 first:** decide the carving (Ghidra create-function only, or capstone dumps; say which), then the registration map and
  `ch0c_listing_check.py`. A failing row is fixed in the tool, not by hand in the tsv.
- **Every function gets a plain name and a sub-phase** (CH1a-CH1p, CH2a0-CH2d, CH3a-CH3d, CH4a-CH4c, CH5). Split a group over ~45 functions
  with a digit suffix and write its kickoff and STATUS row. Unmapped helpers get `CH<n>0` Opus kickoffs and rows.
- **The asset list comes first** (CH0b runs next): routes a / b / c unioned, effect closure, `POPUP_TR_*`, the icon sheets.
- **Item 8 (stat rows) ends as a question to the user** with a recommendation (user, 2026-11-02). Don't decide it.
- Agree the checklist-row split with NU0 (engine sites are NU0's).
- Fill each kickoff's "Amended by CH0" section. Drifted findings go to their owner with a brief (scope rule).
- Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Luciel CH0`.

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
