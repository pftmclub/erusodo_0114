# June effect completion - C3b - identify the armour-level check, the bounding back-speed state and two unread keys (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3 done (commit named in its Done note) |
| Flag | none (no code); the port phase C3c will extend `SERV_IRUHADEV_JUNE_DAMAGEDATA` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | none (its port, C3c, is reviewed) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3 - Done"
grep -n "(C3)" june2017_re/funcmap.tsv
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context"
```

The rebuild plan's Starting context predates `tools/`. Ghidra work uses the `ghidra` skill (read-only);
verdicts go into `june2017_re/funcmap.tsv` with the tag `Effect C3b`.

## Why this phase exists

C3 parsed and stored every straightforward DamageData key and ported each consumer it could prove.
Four pieces were left because their June function is not `confirmed` (identification, not transcription),
and they are June branches no STATUS row owns, so they get their own phase (standing rule: follow June).

1. **`31fd86b0` = CX2GUUser vslot `0x81c( bool, DamageData* )`**, the armour-level check. Called six times by
   `CX2GUUser::DamageReactStateChange` (`31feabd0`), each guarded by `FORCE_BACK_SPEED` (DD `+0x3ec`):
   `@31feae21`, `@31feaf93`, `@31feb13b`, `@31feb2e8`, `@31feb41f`, `@31feb585`. C3's funcmap row has the body:
   `+0x42a0` list non-empty -> true; protected bool `+0x7ff4` false -> false; else the armour level `+0x8034` is
   compared with `32345340( 0 / 2 / 1 )` and DD `+8`. Identify the system: what writes `+0x7ff4` / `+0x8034`, the
   enum behind `32345340`, whether March has any of it (`IsSuperArmor`, `m_bSuperArmorNotRed`, ...), and the
   March counterpart of each of the six call sites in `CX2GUUser::DamageReactStateChange` (`X2GUUser.cpp`
   ~12011). **FORCE_BACK_SPEED's second consumer** is those six `cmp byte [EDI+0x3ec]` tests.
2. **`BOUNDING_BACK_SPEED_X/Y`'s consumer**: `31feabd0 @31feb6a7-31feb6de` writes unit `+0x40f0` / `+0x40f4`
   (X sign-flipped by the facing test `vslot 0x170`), then `StateChange( 0x78 or 0x79, true )` by byte `+0x7e1c`.
   Identify the two states (are `0x78` / `0x79` states March has? the `RT_BOUNDING` case of March's
   `DamageReactStateChange` at `X2GUUser.cpp` ~12535 uses `USI_DAMAGE_AIR_UP`), the readers of `+0x40f0` / `+0x40f4`
   (the bounce state's frame move) and the NPC twin (`CX2GUNPC::DamageReactStateChange`).
3. **`ADD_MEMBER_COUNT` / `_DISTANCE` (ExtraDamageData `+0x104` / `+0x108`)**: parsed (C3), no reader found by a
   whole-`.text` displacement sweep inside the ExtraDamage code. They are used by two June DamageEffects
   (`DAMAGE_EFFECT_CSI_CIP_BURST_WOLF_DEBUFF`). Find the reader: the ExtraDamage is copied into the defender's
   `ExtraDamagePack` (unit `+0x50c` family, stride `0x11c`), so look for the copy (`ExtraDamageData::Copy` June
   twin) and for any reader of the copy's `+0x104` / `+0x108`, including class code (`CX2GUChung`, the CIP skills).
4. **`31fcee40`** (a `short` hit counter at unit `+0x7e2e` / `+0x80ce`, incremented by every user-owned effect hit
   from `CEffect::AttackResult @31f85b2d`): find its readers. If a reader exists in engine code, port the counter
   with C3c; if only class code reads it, route it to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7.

## Done means

Each of the four has a funcmap row (verdict `confirmed`, named, March counterpart or "none in March: <what is
missing>") and the plan's C3b Done note names the port phase C3c's scope (or says no port is possible and why).
Write `kickoffs/june-effect/C3c.md` from the template, add the C3c row to STATUS.md, and commit. No code.
