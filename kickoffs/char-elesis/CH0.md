# June character Elesis (Stage 7) - Phase CH0 - Elesis map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH0` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs now, alongside char-ara) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (each CH1 review re-checks what it builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-11-11)" "Phases" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Phase CH-N - Elesis's June-only class path: Dark Knight (35) and Crimson Avenger (125)"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH0 - Done (2026-11-10)" "CH0b - Done (2026-11-10)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1q - Done (2026-11-04)" "CH1q2 - Done (2026-11-04)"   # what June changed in the shared Way of the Sword
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7" "Phase CH-N - A June-only later class path (template; not run for Aisha)"
```

## Task

Do "Phase CH0" step 0 and items 1-13 (class `CX2GUEL`, June range `0x32b4e770`-`0x32b90b6f`: 274 carved functions, 409 thunks with 234 uncarved
targets, 33 uncarved overrides, vtable `334fa61c`). Ghidra is read-only. You are on Opus: subagents do the greps, tsv joins, Ghidra dumps of known
addresses and the scripted runs; you keep the June-vs-March verdicts, the sub-phase assignment, the `inside` / `new` column and the Way of the
Sword, summon, blood-hook and CH-N maps.

- **Step 0 first:** registration map from `charport_states.py` (InitState wrapper `32b87a60` -> parts `32b81be0` + `32b864f0`,
  `SetEquippedSkillFuncBySkillSlotIndex` `32b8c470`), then `ch0c_listing_check.py` on it. A failing row is fixed in the tool, not by hand in the tsv.
  Copy Ara's builders (`june2017_re/char_ara/ch0_*.py`) into `char_elesis/` with her range; add `elesis` to `charport_adis.py` (`CLASS_RANGE`
  `(0x32b4e770, 0x32b90b6f)`) and make sure it reads every uncarved thunk target and override from the raw bytes (never create functions in Ghidra).
  Map all four switches in `32b8c470` case by case and the 22 id blocks of `SetEquippedSkillLevel` `32b4e790`.
- **Assign every function to the named sub-phases** (CH1a-CH1q, CH2a-CH2d, CH3a-CH3d / CH3b0, CH4a-CH4c, CH5, CHNa / CHNb). A group over ~45
  functions (~25 where most rows are listing-only) is split with a digit suffix (`CH1e2`): write its kickoff and STATUS row. Unmapped helpers
  (item 6) get `CH<n>0` Opus kickoffs and rows.
- **The `inside` / `new` and `carved` / `listing-only` columns decide how CH1a-CH1k write and review each function**; propose the plain name for
  every `new` row. Roles of her overrides come from the body: she shares only 46 of Ara's 77 slots.
- **The asset list is the top priority** (CH0b runs after char-ara CH6): routes a / b / c unioned, effect closure, `POPUP_TR_G_MASTER` /
  `_B_HEART` / `_C_AVENGER`, the class-change dialogs and guides, the creation-screen button dialog, the 35 / 125 sets, the summon NPCs' files, the
  motion target kom and the motion question (which of the 6 differing class motions lose a name March plays: held for CH1 final, else CH0b).
- **Way of the Sword map (item 7), summon map (item 8), blood-hook map (item 9), CH-N map (item 10):** map them, size CH1q, CH3b0, CH4c and
  CHNa / CHNb from the maps. In the CH-N map mark every site `covered by Ara CHNa` or `Elesis`.
- **Passive ids (item 11):** every `SI_*` id March's passive / skill code reads, with June's `RNSI_*` id.
- **S3 baseline (item 12):** `s3_no_state.py --family X2GUEL.cpp --preprocess`, with 35 / 125 rows (`--append-missing`).
- Fill each kickoff's "Amended by CH0" section (CH0b-CH5, CHNa, CHNb: filters with real column numbers, counts, addresses).
- Planning digests in `june2017_re/char_elesis/` (`plan_march_side.md`, `plan_june_side_binary.md`, `plan_june_side_data.md`,
  `plan_checklist_rows.md`, `plan_charport_keys.txt`, `plan_numerical_rows.tsv`): verify before building on a line. Drifted findings go to their
  owner with a brief (scope rule).
- Write the Done note (at most ~60 lines; tables live in the tsvs) and funcmap rows `Char Elesis CH0`. List plan-doc inconsistencies you meet.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).

## Amended by char-ara CH1 (2026-11-11): the enum gate in the map

Run `python tools/luabind.py check ELESIS_KNIGHT` and list every `ENUM` / `STATE` failure in the CH0 note with its owner (CH1 adds June's members
to `Enum.lua`; a missing C++ state-enum member goes to the phase that adds the class's enum names). Ara found hers only at CH1's play-test.
