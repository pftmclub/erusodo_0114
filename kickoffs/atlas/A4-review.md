# Atlas/Elysion port - Phase A4 review (Atlas-half script engine)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A4 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A4 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_ELYSION_SCRIPT_A` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Phase A4 — Atlas-half script engine" "Phase A4 — Done"
grep -a "Atlas A4" june2017_re/funcmap.tsv        # the 19 June addresses A4 mapped
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch.

## Task

Review A4's commit against June (read-only in Ghidra). The places A4 could have got wrong:

- **Bone push** (`CX2GUUser::ProcessCanPushUnit` bone form, June `31fe8ad0`): the prologue's early `false`, Y span
  (bone.y - d, height 2d), half width `myW*0.5 + d` in both branches, the direction override (`327788b0`) only in the
  dir 1/2 branch, and the loop in `PhysicProcess` (June `32010060`) ignoring the per-bone results. Re-read the x87 compares
  in `327788b0` (1/2 frame vs 3/4 frame, JUNE_BUGS 205).
- **UV change** (`31c9a7d0`, `31c95150`, `31c95580` tail, `31c9eea0`): record order LT/RT/LB/RB = `uvOrgTexture` slots,
  the seed record, speed sign, the end order (callback, drop, snap). A4 added three guards June lacks (NULL texture,
  direction above 2): confirm each is unreachable from June data.
- **Die particle** (`3277ec60` head, `32778d40`): condition, emit / trigger values, land Y, no sound.
- **Indicator** (`32f099f0` @32f09f05 / @32f09f2a) and `INIT_AI NO_INDICATOR` (`32f2b4c0`).
- **Line retype** (`327efc90`, hooks in `327f0c10` cases 0 and 5): once per clear condition, stage / sub-stage filter.

Hygiene is one command: `python tools/flagcheck.py ELYSION_SCRIPT_A --range <A4 commit>^..HEAD --with ELYSION_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line.

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the
same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase A4 — Review" into the plan
doc, update A4's STATUS.md row, check `kickoffs/atlas/A5.md` exists, and commit.
