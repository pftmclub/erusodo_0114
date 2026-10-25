# June character Eve (Stage 7) - Phase CH1p - Pressed-index space, hooks, June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1p` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (undefined until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-eve/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1 split (2026-10-21, user)"
```
Then CH0's Done note, the earlier CH1 Done notes / reviews (their routed gaps), and the `CH1p` rows of `june2017_re/char_eve/ch0_*.tsv`. Aisha's matching sub-phase (CH1j / CH1k / CH1l / CH1m) is the worked example.

## Task

7ag: convert Eve's three slot encoders (Mega Electronball, Taser Pilum, Kugelblitz: `X2GuEve.cpp:3391-3473`, `:5529-5537`, `:5590-5598` at planning time), every reader (key switches, `iSkillSlotIndexPressed_ =`, the `GetSkillSlotData*` decoders near `:5481`, `:22110`, `:29793`, `:32358`, the state-array lookup) and the decode `:6151-6177` to June's 5 + 5 numbering **together** (checklist 7ag). Hooks whose engine side waits: `JuneInitWaitStateChange` + the WIN callback (7h), `JuneInitBoundingStates( "ENSI_" )` (7l, the 3 `DAMAGE_BOUNDING_*`), `SET_LIMIT_MAX_G_SPEED` (7o), the 3 `TIME_STOP_IMMUNE_STATE` states (7p), `AT_COMMAND` (7x, 23 uses). The June-only script keys from `ch0_keys.tsv`. Under `_LIVE`.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GuEve.*` / `X2GUUser.*` are CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit (the `SKILL_SLOT_10` / `UPGRADE_SKILL_SYSTEM_2013` pairs). Done gate: the build passes with `_LIVE` undefined, and a `cl /P` probe with it defined compiles the new path; no play-test.

## Amended by CH0

(CH0 fills this: the tsv filter, the function count, June addresses, any split.)
