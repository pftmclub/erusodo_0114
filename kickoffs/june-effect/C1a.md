# June effect completion - C1 step 1 - identify June's numerical-skill-data override (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C1a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C0 |
| Flag | none (no code) |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | none (C1b is reviewed) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C1 - Numerical"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 8c"
```

Also read: `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1, `june2017_re/README.md`, `june2017_re/funcmap.tsv`

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**Census contract (C0, 2026-10-05):** `census_completion.py`'s `_src_numerical` copies the plan's wording (numerical index>0
or name, plus `APPLIED_SKILL_ID` or `DAMAGE_SKILL_ID`) and counts 1,913 blocks. Confirm June's real gate from
`32632390@3263fe41-32640054` (is `USED_SKILL_ID` a third trigger? it appears in 378 blocks); if it differs, fix the predicate and
record the corrected count here.

**This session does step 1 only** (identify, verdicts into funcmap.tsv, March vs June NewSkillTempletVer2.lua diff, stop for the user if adopting June's file changes more than numerical data). Step 2 is C1b, a new Sonnet conversation.

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
