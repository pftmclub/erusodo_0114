# June effect completion - C4c - port June's NPC Fury mode

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C4c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C4b (identification; it rewrites this kickoff's "C4b findings" section) |
| Flag | `SERV_IRUHADEV_JUNE_NPC_FURY_MODE` (new, in `KTDXLIB/Always.h`) |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C4c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C4 - Review (2026-10-07, Opus 5.5 high)" "C4b - Done"
grep -n "Effect C4b\|Effect C4 review" june2017_re/funcmap.tsv
```

## C4b findings

(C4b fills this in. Until then this phase is blocked.)

## Task

Port June's NPC Fury mode as C4b lists it, under `SERV_IRUHADEV_JUNE_NPC_FURY_MODE`: the three
state keys in the `CX2GUNPC` state parse, whatever sets and clears the NPC's Fury byte (`+0x8fb7`),
the `GetFinalDamage` multiply (June `32284aa0 @32285757-322857e8`: physic and magic locals each
multiplied by the state's `ATTACK_POWER_RATE_FURY` when the local is nonzero, placed after the two
damage locals are loaded and **before** the C4 `JuneApplyDivideDamageFromRange` call) and every other
reader C4b found. Every formula cited; June quirks kept verbatim with `JUNE_BUGS.md` rows.

`CX2GameUnit::GetFinalDamage` is also C10b-1's: do not run in parallel with it.
