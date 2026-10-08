# June effect completion - C8 step 2 - port the hit paths and the base/user/NPC layers

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C8b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C8a (done 2026-10-06). The per-class overrides are **not** this phase: D8 (2026-10-06) handed them, as C8c, to the character ports. |
| Flag | `SERV_IRUHADEV_JUNE_COLLISION` (hit paths), `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` (new, the layers; append to `Always.h`) |
| Log tag | `[JUNE_FX]`. The hit trace goes to `offline_server.log` (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C8b-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C8 - Hit path" "C8a - Done"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context"
```

Also read:
- the funcmap rows tagged `Effect C8a` (`grep "Effect C8a" june2017_re/funcmap.tsv`), and rows 194-196;
- `MODS.md`'s `SERV_IRUHADEV_JUNE_COLLISION` entry ("Known gap");
- `X2Lib/X2DamageManager.cpp`: `JuneEffectCollisionDataCheck` and the `JUNE_FX HIT` trace (Phase 9's port, the template for this phase).

Edit CP949 files with `tools/bytepatch.py` only. Build with `tools/build.py --deploy`, and check hygiene with `tools/flagcheck.py`. Never launch X2_offline.exe: ask the user.

## Task

Port exactly items 1-5 of the C8a Done note's "C8b port list", citing the June address on each new code path. In short:

1. **Hit paths** (`_JUNE_COLLISION`):
   - UnitToUnit (`CollisionDataCheckFromUnitToUnit`, three calls);
   - UnitToEffect (three calls);
   - MeshToUnit;
   - CollisionDataToUnit (a set × datum variant without the guard).

   All take June's pair math. The advance branch stays in EffectToUnit only. March's calls stay in the `#else`. Tag the COLLIDE/DIFF lines with the path. Add the owner UID and the effect's age to the traced `TEST` line.
2. **`+0x4108`** (`_JUNE_DAMAGE_EXCEPTION`):
   - the member, with ctor 2 and `ResetDamageType` -1;
   - the user-layer gate.

   It is inert on March data. The `= 2` write is the Raven port's, not yours.
3. **User vslot `0x758` hook (`3202ac40`), as June has it.**
   - `31fdbbb0` is a plan-rule-5 stub: it returns null and logs `JUNE_FX unsupported skill memo` once.
   - Map each case's DD offset with the list in funcmap row `3202ac40` (`Effect C8a`). Skip exactly the offsets that row marks SKIP, with a comment naming each. Do not map offsets yourself: never port an offset.
   - Call order in the user layer: Raven block, then `0x758`, then `0x780`.
4. **NPC override `32759f30`**: the base, then the master's `0x758`, under row 196's conditions.
5. **An empty `CX2GUUser` vslot `0x780` hook `(DD, fPowerRate)`**, called last.

**Do not re-add** what the buff port already ported in `CX2GameUnit::SetDamageExceptionProcess`:
- AT_COMMAND 85/86;
- `GetChargeMpRateByBuff`;
- the state/serial tail.

**Do not port:**
- DD `+0x638`;
- the socket copy to DD `+0x3b4` (june-buff 16-step3c's).

Update MODS.md: close the "Known gap", and add the `_DAMAGE_EXCEPTION` entry. Every `fPowerRate` multiply is unchanged, so no damage number moves; say so.

**Play-test hand-off.** One check per path. All four trigger functions are in the current exe (`X2/US_SERVICE/X2.map`). The characters are in `els_db.sql`, checked 2026-10-06:
- MeshToUnit: `jtVoidPrincess` (uid 28, Aisha / Void Princess, lv 80), `EvasionStandupAttack` or `ComboX3down` (`AT_MESH`).
- CollisionDataToUnit: the same character's `ComboX3front`, or `ravbe` (uid 23, Raven / Reckless Fist, lv 67) `COMBO_ZZZX` (`AT_MANUAL_COLLISION_DATA`).
- UnitToEffect: `/CM "LIZARDMAN_SHAMAN" 0 70`, then melee its `LIZARDMAN_SHAMAN_MAGIC_MISSILE`, an `ATTACKED_BY_UNIT` templet used by `data037 Lizardman_Shaman.lua`.
- UnitToUnit: any melee on that shaman.

Each check expects a `JUNE_FX HIT` line with its path tag. Also add the Mag Charger re-run for T6 (`/CM "MAG_CHARGER" 0 70`), and ask how many Mag Chargers were on screen.

## Added by C3 (2026-10-08)

C3 added `DamageData::m_iAppliedMemoID` (`APPLIED_MEMO_ID`, June DD `+0x5c8`, default 0; parsed in `SetDamageDataFromLUA`, logged once per templet as unsupported). Its one June reader is the skill-memo DamageData switch `3202ac40` (the `0x758` memo hook this phase stubs): the memo is skipped when `+0x5c8` is set and differs from the entry's type. Read the member instead of re-parsing the key, and drop the unsupported log line when the switch is wired (`X2DamageManager.cpp`, the `APPLIED_MEMO_ID` block).

## Added by the C3 review (2026-10-08)

Memo case 2 of `3202ac40` writes DD `+0x5d0`. That field is the CEffect ctor's scale: `@31f7c795-31f7c838` with IGNORE_MI_HUGE false calls
SetScale( s, s, s ) and then stores s into the per-instance **base** `+0xb24..`; June's SetScale `31f6fdc0` multiplies by that base, March's
`CEffect::SetScale` is absolute. If this phase (or a later one) maps `+0x5d0` to a March member **and** ports the ctor step, it must also port
the base and make `CEffect::SetScale` multiply by it, all in the same change. Otherwise C3's four `SetScale( CREATE_DAMAGE_EFFECT_NAME_SCALE )`
calls (default 1,1,1; `X2DamageEffect.cpp` x2, `X2GUUser.cpp` x2) reset a memo-scaled child to 1.0. With `31fdbbb0` stubbed (item 3) the
switch never runs, so nothing changes while the stub is in place; say so in the Done note. The matrix row `IGNORE_MI_HUGE` ("no writer of that field
found") is wrong on this point: the writer is this memo case. Same dependency for modifier tag 15: checklist 7a.
