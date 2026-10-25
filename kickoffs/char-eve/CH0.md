# June character Eve (Stage 7) - Phase CH0 - Eve map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH0` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each CH1 review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-10-25)" "Phases" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH0 - Done (2026-10-20)" "CH0c - Done (2026-10-20)" "Template for Stage 7"
```
Then `june2017_re/char_eve/plan_march_side.md`, `plan_june_side.md`, `plan_checklist_rows.md` (planning digests: verify before building on a line).
Pull checklist sections by heading as each Eve row needs them; never the whole file.

## Task

Do "Phase CH0" step 0 and items 1-10. Ghidra is read-only. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known
addresses and the scripted runs; you keep the June-vs-March verdicts, the sub-phase assignment and the Nasod Core map.

- **Step 0 first:** registration map from `charport_states.py`, then `ch0c_listing_check.py` on it. A failing row is fixed in the tool, not by hand
  in the tsv (the pilot's CH0c lesson). Make the pilot's builder scripts take a character (or copy them into `char_eve/` with the range changed).
- **Assign every function to the named sub-phases** (CH1a-CH1q, CH2a-c, CH3a-d, CH4a-c, CH5). A group over ~45 functions is split with a digit
  suffix (`CH1c2`): write its kickoff and STATUS row. Unmapped helpers (item 6) get `CH<n>0` Opus kickoffs and rows.
- **The asset list is the top priority** (CH0b runs next): routes a / b / c unioned, effect closure included, the motion question answered.
- **Nasod Core (item 7):** map it whatever decision 1 says; if the user has not answered decision 1 yet, ask it at the end with the map in hand.
- Fill each kickoff's "Amended by CH0" section (filters with real column numbers, counts, addresses).
- Drifted findings go to their owner with a brief (scope rule); say which bucket in the hand-off.
- Write the Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Eve CH0`.
