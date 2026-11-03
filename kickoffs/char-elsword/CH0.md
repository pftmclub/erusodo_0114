# June character Elsword (Stage 7) - Phase CH0 - Elsword map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH0` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each CH1 review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-11-03)" "Phases" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH0 - Done (2026-10-31)" "CH0b - Done (2026-10-31)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
```
Then `june2017_re/char_elsword/plan_march_side.md`, `plan_june_side.md`, `plan_checklist_rows.md`, `plan_charport_keys.txt` (planning digests:
verify before building on a line). Pull checklist sections by heading as each Elsword row needs them; never the whole file.

## Task

Do "Phase CH0" step 0 and items 1-10. Ghidra is read-only. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known
addresses and the scripted runs; you keep the June-vs-March verdicts, the sub-phase assignment, the `inside` / `new` column and the Way of the
Sword map.

- **Step 0 first:** registration map from `charport_states.py`, then `ch0c_listing_check.py` on it. A failing row is fixed in the tool, not by
  hand in the tsv. Reuse Chung's / Eve's builders with a character argument, or copy them into `char_elsword/` with Elsword's range; add
  `elsword` to `charport_adis.py` and make sure it reads the 25 uncarved overrides from the raw bytes (never create functions in Ghidra).
- **Assign every function to the named sub-phases** (CH1a-CH1q, CH2a-d, CH3a-d, CH4a-c, CH5). A group over ~45 functions is split with a digit
  suffix (`CH1e2`): write its kickoff and STATUS row. Unmapped helpers (item 6) get `CH<n>0` Opus kickoffs and rows.
- **The `inside` / `new` column decides how CH1a-CH1k write each function**; propose the plain name for every `new` row.
- **The asset list is the top priority** (CH0b runs next): routes a / b / c unioned, effect closure, `POPUP_TR_*`, the `data199` target kom,
  the motion question answered.
- **Way of the Sword (item 7):** map it, say which pieces Elesis shares, size CH1q from the map.
- Fill each kickoff's "Amended by CH0" section (filters with real column numbers, counts, addresses).
- Drifted findings go to their owner with a brief (scope rule); say which bucket in the hand-off.
- Write the Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Elsword CH0`.

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
