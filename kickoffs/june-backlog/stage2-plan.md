# June port backlog - Stage 2 planning - engine remainder and measurements

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-backlog stage2-plan` |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 2), output `JUNE_ENGINE_REMAINDER_PHASES.md` (new) |
| Branch | main Trunk, current branch (`mods/june-buff-port`); docs and RE only, so no branch question needs asking |
| Depends on | nothing (backlog concurrency note: planning is read-only and runs any time); its code phases run after june-effect C12 |
| Runs with | june-effect C11 (that session play-tests and may rebuild; this one never builds, deploys or copies a kom) |
| Flag | none in this session (no code); propose one family per planned phase, e.g. `SERV_IRUHADEV_JUNE_RENDER_*` |
| Log tag | propose one in the plan (e.g. `JUNE_FX RENDER`) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "The order at a glance" "Facts behind the order" "Stage 0: close the open loops" "Stage 2: engine remainder and measurements" "Standing rules (every stage, every kickoff)" "Stage 2 planning (full), Opus 5.5 high"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "Definition of complete" "Where things stand (census 2026-10-04)"
python tools/docsection.py JUNE_ASSET_PORT_PHASES.md "Phase P10 - XRenderer.fx"
python tools/docsection.py ATLAS_PORT_PHASES.md "Investigation ledger (measured 2026-10-03 — do not redo; the census re-verifies numbers)" "Assets"
grep -n "CTT_RIM_LIGHT_BLUE\|\.fx" RANOX_PORT_PLAN.md
```

`JUNE_KOM_CONTENT_GUIDE.md` and `JUNE2017_CHARACTER_PORT_CHECKLIST.md`: `--list` the outline and pull only the sections a
measurement needs. Never read a plan doc in full (the inline prompt's "in full" predates `tools/docsection.py`).

## Task

The inline prompt ("Stage 2 planning (full)") is the task: measurements 1-5, then `JUNE_ENGINE_REMAINDER_PHASES.md` in the
`new-plan` shape of the x2-phase skill. Changes since it was written:

- **Output shape:** the plan doc, one kickoff per phase in `kickoffs/june-engine/` (from `kickoffs/TEMPLATE.md`), and a
  `june-engine` block in `STATUS.md`, not inline kickoff prompts.
- **Measurement 1 (shaders):** `XRenderer.fx` adoption is already owned by june-asset-port P10, and the June-only `.fx` files
  ship inert in P4. Measure every `.fx` anyway; the plan's phases cover only the C++ side (what June's setter code sets that
  March's does not) and say which P-phase the data copy belongs to. Extract June files with `tools/junekom.py`, never by hand.
- **Measurement 4 (UI):** `june2017_re/region_census.py` does not exist; write the census as a script under `june2017_re/`.
- **Measurement 5 (effect engine):** june-effect C11 is that verification and may be running at the same time. Use its
  Done note if it has landed; otherwise record "pending C11" and do not rerun its census or trial load.
- **Usage recording:** drop the inline prompt's "record this session's usage" line (no cost steps in plans).

Ghidra is read-only. Funcmap rows for June addresses found here go in `june2017_re/funcmap.tsv`, tagged `Engine S2`.
If a measurement contradicts `JUNE_PORT_BACKLOG_ORDER.md` (e.g. base models changed, a UI framework gap), update that file's
affected rows with a dated note. Commit only your own files, with explicit pathspecs.

No code, no build, no kom copy, no game launch. Open design questions go to the user at the end, each with a recommendation.
