# June character Eve (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1m` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (undefined until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-eve/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1 split (2026-10-21, user)"
```
Then CH0's Done note, the earlier CH1 Done notes / reviews (their routed gaps), and the `CH1m` rows of `june2017_re/char_eve/ch0_*.tsv`. Aisha's matching sub-phase (CH1j / CH1k / CH1l / CH1m) is the worked example.

## Task

Port June's `InitState` (vslot 0x7a4) and `InitStateCommon` whole: cut-in set index, base event states, the START / WIN / LOSE roll `31fe22d0`, the hyper-mode table name (`31fdd490`), the per-job blocks for 5 / 18 / 19 / 26 / 108 / 109 / 116, every callback pointed at its `June_*` member, all from `ch0_state_skill.tsv` only (CH0 checked it against the listing). March's registration stays in the `#else`. Everything new is under `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE`, which you add to `Always.h` **commented out** (`//#define`): it stays undefined until CH1 final. March-only states June drops are not registered; June quirks (a registration with no script table) are kept with a `JUNE_BUGS.md` row.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GuEve.*` / `X2GUUser.*` are CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit (the `SKILL_SLOT_10` / `UPGRADE_SKILL_SYSTEM_2013` pairs). Done gate: the build passes with `_LIVE` undefined, and a `cl /P` probe with it defined compiles the new path; no play-test.

## Amended by CH0

(CH0 fills this: the tsv filter, the function count, June addresses, any split.)
