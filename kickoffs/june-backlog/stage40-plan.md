# June port backlog - Stage 40 planning - triage of the June code no address can place

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-triage` |
| Output | plan key `june-triage` (`kickoffs/june-triage/`, a `june-triage` STATUS block), plan doc `JUNE_TRIAGE_PHASES.md` (new) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 40) |
| Depends on | june-census Z (the user's answer on the parked row); runs with anything (read-only, no code) |
| Flag | none (read-only: Ghidra, census tables, docs) |
| Log tag | none |
| Review | none (a planning session); the plan's triage phases are read-only and need no review |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 40: triage of the June code no address can place (june-census V2, 2026-11-09)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE_COVERAGE_CENSUS_PHASES.md "Phase V2 - Exclusions, new stages, estimate (Opus)"
```

and `june2017_re/census/SUMMARY.md`; the rows of `june2017_re/census/exclusions.tsv` whose `stage` is `S40` (191 groups, 9,641 functions).

## Task

The census placed every unowned June studio function by the March `.obj` of its neighbours. 191 address gaps sit between files of
different systems (or between strings naming several systems), so their owner is unknown. Plan a read-only triage that names them.
1. **Phases.** Split the 191 gaps into Opus phases of about 10-20 gaps each, largest first (`gap:X2GUUser..X2Data@0x32092b20` 1,133,
   `gap:X2UnitSlashTraceManager..X2NPCUnitViewerUI@0x32d5dd70` 938, `gap:KTDGParticleSystem..X2SkillTree@0x31f31950` 299, ...).
   Sonnet subagents do the dumps; Opus names the classes.
2. **Method per gap.** Decompile the heads of each run; a ctor that stores a `PTR_` vtable starts a class; callers outside the gap name
   its user; `func_strings.tsv` / `xorstrings.tsv` name the files; March's `X2.map` names the neighbours. Classify each run as a March class
   (which `.obj`), a June-only class (which system), serializer / STL glue (exclude), or still unknown (say so).
3. **Output.** `june2017_re/census/triage.tsv` (`gap`, `first_va`, `last_va`, `functions`, `class_or_system`, `decision`, `stage`,
   `evidence`), funcmap rows for each class anchor found (tag `Triage S40`), and a change to `v2_exclusions.py` so a routed run takes its
   stage from `triage.tsv`; re-run it and `tools/junebinary.py --check`.
4. **Routing.** A run whose system has a stage goes there (attach); a June-only system with no stage becomes a new backlog stage (scope rule);
   glue and server-only code is excluded with the reason. Boss raid / world mission / wedding / mentor follow the user's answer to june-census V2.

Write the plan in the x2-phase `new-plan` shape (one `##` section per phase, kickoffs, STATUS block), mark this brief's STATUS row done,
and commit with explicit pathspecs.
