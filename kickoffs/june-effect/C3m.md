# June effect completion - C3m - June's world-physics scale with a PhysicParam backup (identify)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3m` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3h (found there) |
| Flag | none (identify only); the port, if any, is a new phase |
| Log tag | - |

## Read first

```sh
grep -n "32296290 (C3h)\|322a5f90 / 322886a0" june2017_re/funcmap.tsv
```

## Task

C3h found `32296290`, a CX2GameUnit function with no March match yet. If the object behind `3372c0fc` (its vslot `0x90`) exists, the function:

- takes a factor from that object's `+0x184`;
- copies the whole PhysicParam (`+0x300`) once to a June-only block at `+0x334`, guarded by the byte `+0x368`;
- sets `fReloadAccel`, `fGAccel` and `fMaxGSpeed` to backup x factor.

`ResetPhysicParam` (`322886a0`) reads the same global after its Org copy.

1. Name the global's class, the `+0x184` factor and every caller of `32296290`. Compare it with March's `ResetPhysicParam` world block
   (`X2GameUnit.cpp`, `GetGravityAndSpeedRate`, `DEEP_WATER_DUNGEON_TEST`): is this March's mechanism moved, or a June-only one?
2. Say whether anything in March data or June data reaches it, and what the player would see.
3. Verdicts into `june2017_re/funcmap.tsv`, tagged `Effect C3m`. Write a Done note. If June-only behaviour exists, plan the port as a new
   phase (STATUS row + kickoff), per the standing "plan unowned June branches" rule.
