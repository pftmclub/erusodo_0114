# June effect completion - C1 step 2 - transcribe the numerical data loader and override

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C1b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C1a (its funcmap.tsv verdicts and the user's answer on NewSkillTempletVer2.lua) |
| Flag | `SERV_IRUHADEV_JUNE_NUMERICAL_DATA` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C1b-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C1 - Numerical"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 8c"
```

Also read: `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1, `june2017_re/funcmap.tsv` (C1a's rows)

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**Census contract (C0, 2026-10-05):** `census.py` counts the numerical-data source as landed when
`SERV_IRUHADEV_JUNE_NUMERICAL_DATA` is defined in `KTDXLIB/Always.h` and tested by an `#if` in `X2Lib`/`KTDXLIB` code, so use
exactly that name. Its item-1 line then reads 0 blocks (it counts 1,913 today).
`--final`'s item 4 passes all four `APPLIED_SKILL_ID` rows (`DE.DAMAGE_DATA`, `DE.top`, `ES.templet`, nested `BUFF_FACTOR`) once that
flag exists, so confirm each of the four has a consumer, or record which ones June itself never consumes.

**This session does step 2 only**, from C1a's verdicts. If a verdict it needs is missing or `candidate`, stop and say so (escalate to Opus) rather than identify it. The loader follows CLAUDE.md's Lua rule (shipped load path, degrade-if-missing, name the file to pack). After it lands, re-adopt June's effect data per D1 (`stage_june_fx.py --apply`) and name what the user must repack.

Verbatim kickoff prompt from the plan:

```
June2017 effect completion C1 (numerical skill data). Read JUNE2017_EFFECT_COMPLETION_PHASES.md
(C1), JUNE2017_CHARACTER_PORT_CHECKLIST.md section 1, the rebuild plan's Starting context and
Phase 8c, june2017_re/README.md and funcmap.tsv. Branch: current.

Step 1 on Opus: identify FUN_32632390 @3263fe41-32640054's callees (32890620, 31fca8e0,
320717e0, 32078060), the table they read (lead: June NewSkillTempletVer2.lua NUMERICAL_DATA1-12)
and how skill level selects a row; write verdicts to funcmap.tsv; diff March vs June
NewSkillTempletVer2.lua and stop for me if adopting June's changes more than numerical data.
Step 2 (after /model to Sonnet): transcribe the loader and the override under
SERV_IRUHADEV_JUNE_NUMERICAL_DATA, store APPLIED_SKILL_ID/DAMAGE_SKILL_ID where it needs them.
CP949 byte-level edits; build US_SERVICE, deploy, ASK before launching. Done when census.py's numerical-skill-data item-1 line
passes and three of the 492 named skills show damage in play.
```
