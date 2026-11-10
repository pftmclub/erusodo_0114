# June character Ara (Stage 7) - Phase CH0 - Ara map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH0` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs now, alongside char-rena) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each CH1 review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-11-10)" "Phases" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Phase CH-N - Ara's June-only class path: Little Specter (34) and Asura (124)"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH0 - Done (2026-11-07)" "CH0b - Done (2026-11-07)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7" "Phase CH-N - A June-only later class path (template; not run for Aisha)"
```

## Task

Do "Phase CH0" step 0 and items 1-12 (class `CX2GUAra`, June range `0x32b90b70`-`0x32bd86e0`: 540 carved + 32 uncarved overrides, vtable
`33505314`). Ghidra is read-only. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known addresses and the scripted runs; you keep
the June-vs-March verdicts, the sub-phase assignment, the `inside` / `new` column and the force-power, Specter and CH-N maps.

- **Step 0 first:** registration map from `charport_states.py` (InitState wrapper `32bcf4a0` -> body `32bca580`, `SetEquippedSkillFuncBySkillSlotIndex`
  `32bd09d0`), then `ch0c_listing_check.py` on it. A failing row is fixed in the tool, not by hand in the tsv. Copy Rena's builders
  (`june2017_re/char_rena/ch0_*.py`) into `char_ara/` with Ara's range; add `ara` to `charport_adis.py` (`CLASS_RANGE` `(0x32b90b70, 0x32bd86e0)`)
  and make sure it reads the 32 uncarved overrides from the raw bytes (never create functions in Ghidra). Map every switch in `32bd09d0`.
- **Assign every function to the named sub-phases** (CH1a-CH1q2, CH2a-CH2d / CH2b0 / CH2c0, CH3a-CH3d, CH4a-CH4b, CH5, CHNa / CHNb). A group over
  ~45 functions is split with a digit suffix (`CH1e2`): write its kickoff and STATUS row. Unmapped helpers (item 6) get `CH<n>0` Opus kickoffs and rows.
- **The `inside` / `new` column decides how CH1a-CH1k write each function**; propose the plain name for every `new` row.
- **The asset list is the top priority** (CH0b runs after char-rena CH6): routes a / b / c unioned, effect closure, `POPUP_TR_S_DEVANAM` /
  `_Y_RAJA` / `_ASURA`, the class-change dialogs, the 34 / 124 sets, the tail meshes, the motion target kom (no March `data205.kom`) and the motion
  question (which of the 7 differing class motions lose a name March plays: held for CH1 final, else copied by CH0b).
- **Force-power map (item 7), Specter / tail map (item 8), CH-N map (item 9):** map them, size CH1q, CH1q2 and CHNa / CHNb from the maps.
- **Passive ids (item 10):** every `SI_*` id March's passive / skill code reads, with June's `RNSI_*` id.
- **S3 baseline (item 11):** `s3_no_state.py --family X2GUAra.cpp --preprocess`, with 34 / 124 rows (regenerate the S3 tsv with them if absent).
- Fill each kickoff's "Amended by CH0" section (CH0b-CH5, CHNa, CHNb: filters with real column numbers, counts, addresses).
- Planning digests in `june2017_re/char_ara/` (`plan_march_side.md`, `plan_june_side.md`, `plan_checklist_rows.md`, `plan_charport_keys.txt`,
  `plan_numerical_rows.tsv`): verify before building on a line. Drifted findings go to their owner with a brief (scope rule).
- Write the Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Ara CH0`. List plan-doc inconsistencies you meet.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).
