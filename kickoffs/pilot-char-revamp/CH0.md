# June character Aisha (Stage 5 pilot) - Phase CH0 - Aisha map (read-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH0` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything) |
| Flag | none (no code, no build) |
| Log tag | none |
| Review | none (CH1's review re-checks what CH1 builds on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Context" "Rules" "Starting context" "Planning evidence (2026-10-20)" "Phases" "Phase CH0 - Aisha map (read-only)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "Why this document exists" "5. What no scan here covers" "6. Order of work before the first character port"
```
Then `june2017_re/char_arme/plan_march_side.md` and `plan_june_side.md` (planning digests: verify before building on a line).
Pull other checklist sections by heading (`--list` for the outline) as each Aisha row needs them; never the whole file.

## Task

Do "Phase CH0" items 1-7. Ghidra is read-only. You are on Opus, so hand the mechanical work to subagents: greps, tsv joins, Ghidra dumps of
addresses you already know, and the `charport_keys.py` script once you have specified it. Keep the June-vs-March verdicts and the phase
assignment for yourself.

- Write the outputs to `june2017_re/char_arme/` (`ch0_class_map.tsv`, `ch0_state_skill.tsv`, `ch0_checklist_rows.tsv`, `ch0_assets.tsv`,
  `ch0_effect_closure.tsv`) and `june2017_re/charport_keys.py`. Add funcmap rows tagged `Char CH0`.
- The effect closure (item 5b) is a requirement from the user: every particle, mesh-player entry, texture, mesh and sound Aisha's June states
  reach, including run-time-built names. Make the closure walker reusable, because Stage 7 runs it for every character.
- Third jobs (`UC_150`-`UC_186`) are out of scope: skip them.
- Write the Done note under the phase (at most ~60 lines; the tables live in the tsvs).
- Amend `kickoffs/pilot-char-revamp/CH1.md`-`CH5.md`: each has an "Amended by CH0" section; fill it with the tsv filter, the addresses and
  any split you made (e.g. CH2a / CH2b, with new kickoff files and STATUS rows).
- Drifted findings (not Aisha's, not needed to play her) go to their owner with a brief, under the backlog's scope rule; say which bucket in
  the hand-off.
- Stop and ask the user only for real choices, e.g. the `UnitTemplet.lua` adoption if it changes other characters' behaviour.
