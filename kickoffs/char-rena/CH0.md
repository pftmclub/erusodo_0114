# June character Rena (Stage 7) - Phase CH0 - Rena map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH0` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each CH1 review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-11-07)" "Phases" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH0 - Done (2026-11-05)" "CH0b - Done (2026-11-05)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
```
Then `june2017_re/char_rena/plan_*` (planning digests: verify before building on a line). Pull checklist sections by heading as each Rena row needs
them; never the whole file.

## Task

Do "Phase CH0" step 0 and items 1-11 (class `CX2GULire_ElvenRanger`, June range `0x32c6a8d0`-`0x32cb6c6d`: 467 carved + 24 uncarved overrides). Ghidra is
read-only. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known addresses and the scripted runs; you keep the June-vs-March
verdicts, the sub-phase assignment, the `inside` / `new` column and the natural-force and trap maps.

- **Step 0 first:** registration map from `charport_states.py` (`InitState` `32cadc90`, `SetEquippedSkillFuncBySkillSlotIndex` `32cb06d0`), then
  `ch0c_listing_check.py` on it. A failing row is fixed in the tool, not by hand in the tsv. Copy Raven's builders (`june2017_re/char_raven/ch0_*.py`) into
  `char_rena/` with Rena's range; add `rena` to `charport_adis.py` (`CLASS_RANGE`) and make sure it reads the 24 uncarved overrides from the raw
  bytes (never create functions in Ghidra). Map every switch in `32cb06d0` (skill type, hyper switch, skill switch).
- **Assign every function to the named sub-phases** (CH1a-CH1q, CH2a-c / CH2d0 / CH2d, CH3a-d, CH4a-b, CH5). A group over ~45 functions is split with a
  digit suffix (`CH1e2`): write its kickoff and STATUS row. Unmapped helpers (item 6) get `CH<n>0` Opus kickoffs and rows.
- **The `inside` / `new` column decides how CH1a-CH1k write each function**; propose the plain name for every `new` row.
- **The asset list is the top priority** (CH0b runs next): routes a / b / c unioned, effect closure, `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER`, the
  class-change guide dialogs, the trap NPC meshes, the motion target kom (no March `data201.kom`) and the motion question (which of the 11 differing
  class motions lose a name March plays: held for CH1 final, else copied by CH0b).
- **Natural force (item 7) and trap map (item 8):** map them, size CH1q and CH2d0 from the maps, say which June-port arms `ADD_RENA_SYSTEM` flips and
  which branch is June's. Open decision 1 is the user's: map natural force either way (start from `plan_add_rena_system.md`).
- **Passive ids (item 9):** every `SI_*` id March's passive / skill code reads, with June's `RNSI_*` id (D8 pair 3405 / 3400 included).
- **S3 baseline (item 10):** `s3_no_state.py --family X2GULire_ElvenRanger.cpp --preprocess` (260 expected). Fill each kickoff's "Amended by CH0"
  section (CH0b-CH5: filters with real column numbers, counts, addresses).
- Planning digests in `june2017_re/char_rena/` (`plan_march_side.md`, `plan_june_side.md`, `plan_checklist_rows.md`, `plan_charport_keys.txt`,
  `plan_numerical_rows.tsv`): verify before building on a line. Drifted findings go to their owner with a brief (scope rule).
- Write the Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Rena CH0`. List plan-doc inconsistencies you meet.

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
