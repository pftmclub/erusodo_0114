# June character Rena (Stage 7) - Phase CH3b2 - fix the two CH3b play-test failures: Wind Blast never hits, Gungnir never leaves its ready state

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH3b2` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`); read "CH3b - Done (2026-11-10)" and its play-test note |
| Depends on | CH3b committed (`47559185`); one at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`, still on) and, for the engine fix, its own `SERV_IRUHADEV_JUNE_*` flag in `Always.h` |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `JUNE_FX CE_HIT` / `CE_LOCKON` |
| Review | its own, `kickoffs/char-rena/CH3b2-review.md` (engine file + core-list Lua keep their own review; amended 2026-11-10) |

## What the user saw (play-test of unit 92 `GrandArcherT`, set 1, 2026-11-10 10:46-10:48)

- **Wind Blast (23050, slot Q).** The arrow chases the enemy but never explodes on contact. The other eight skills passed.
- **Gungnir (23049, slot R).** The cast stays in its first animation; the user thinks an aim like Chung's Artillery Strike is needed (it is: hold the skill key, arrow keys move the aim, release fires, see `LESI_RGA_GUNGNIR_CONTROL_IDLE_EventProcess`).

## Findings so far (Sonnet, read-only; verify before acting)

**Wind Blast.**
- `offline_server.log`: the cast logs `wind blast lock on npc uid 1000011` ( casts 2-4; cast 1 found no target: uid -1 ), the arrow's `CE_TEMPLET` appears, and **no `CE_HIT DamageEffect_SA_RSR_WindBlast_Arrow` line ever follows** ( One Target Shot, Freezing Arrow, Aero Strafe, Entangle all log theirs ). The NPC 1000011 was in melee range ( it hit Rena at 10:46:53 ).
- June's template ( `june_src_fixed/data036_kom/DamageEffect.src.lua:46224` ) has `LOCK_ON_DATA` with `LOT_NEARST_UID_VECTOR_IN_RANGE`, `NOT_TARGET_PRE_LOCK_ON_TARGET = NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT`, **`ONLY_TARGET_ATTACK = TRUE`**, `TIME_UNLOCK_FROM_START = 0.3`, `HIT_DAMAGE_EFFECT0 = ..._Arrow_Explosion`.
- **Lead:** `X2Lib/X2DamageManager.cpp:2920-2928` ( `LOCK_ON_USER_ONLY_ON` ): for `m_bIsOnlyTargetAttack` it `static_cast`s **every defender, NPCs included,** to `CX2GUUser` and skips it unless `GetUnitUID() == pAttacker->GetLockOnUnitUID()`. An NPC target is locked through `m_LockOnNPCUID` ( the unit uid stays -1 ), so no NPC can ever pass: the arrow flies through them. March only used this flag for PvP users. Read June's `EffectToUnit` `3262b6e0` ( the funcmap row exists, `@3262b8..` region near the `ONLY_RELATIONSHIP` test ) for what June does with this flag and an NPC defender ( likely: compare the NPC uid with `m_LockOnNPCUID` ), then port it whole behind its own flag, March's block in `#else`. Shared engine file: the review for this change is its own.
- Also check, in the same pass: whether `m_bLockOnUserUnit` ( `LockOnData` +0x930 ) must be cleared when the NPC uid is written ( `CX2DamageEffect::SetLockOnTargetUID( uid, bUserUnit )` does it; CH3b's Wind Blast block calls `SetLockOnNPCUID` only ), and whether the CH3b lock-on block should call `SetLockOnTargetUID` instead ( June writes `+0x938/+0x93c` for a user, `+0x940` for an NPC, nothing else, `32c9d7b0 @32c9d9ac-@32c9d9e1` ).

**Gungnir.**
- Log: `equip skill 23049 slot 3 bank B state 65`; at cast `spawning 1 NPC(s), UIDs 1000028`, `ES_CREATE EffectSet_SA_RGA_Gungnir_Ready` ( ownerState 65 ), `CE_TEMPLET DamageEffect_SA_RSR_Gungnir_Square`; then nothing: **no `ES_CREATE EffectSet_SA_RGA_Gungnir_Loop`** ( the first line of `LESI_RGA_GUNGNIR_CONTROL_IDLE_StateStart` ), so the ready state never reached `LESI_RGA_GUNGNIR_CONTROL_IDLE`. The game was closed 10 s after the cast, so the 10 s timeout back to `USI_WAIT` may simply not have been reached.
- `LESI_RGA_GUNGNIR_READY_EventProcess` ( `X2GULire_ElvenRanger.cpp`, search the name ) leaves only when `GetNPCUnitByUID( m_iGungnirTargetNPCUID )` is non-null after 0.6 s. The link is written by `X2Game.cpp` `SetUserSummonedNPCInfo` ( `case NUI_RENA_TARGETING_GUNGNIR`, CH2d0 ), whose debug line is under `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP_DEBUG`: **retired, so the log cannot say whether the link ran.**
- **Plan:** first add `_SKILL_DEBUG` lines ( the link case; `READY_EventProcess` once per second while the NPC is missing, with `m_iGungnirTargetNPCUID` and whether the NPC exists; the NPC's state names it is sent ), build once, ask the user for one Gungnir cast ( hold the skill key 2 s, then release ), read the log, then fix the cause. Suspects: the link case not reached ( `SetUserSummonedNPCInfo` gate: `NAT_ALLY` + owner uid, or `GetAllyTeam()` ); the NPC created but killed or never ready ( check the `Targeting` NPC script `START` state and `SHOT_GUNGNIR_IDLE` / `_FIRE` / `_CANCEL` state names against `npc` Lua ); `m_iGungnirTargetNPCUID` reset by a ReInit between the cast and the check; the aim NPC needs a June engine feature March lacks ( then say "this needs ..." and route it ).
- The READY state's Lua ( `LESI_RGA_GUNGNIR_READY` in `Lire_ElvenRanger.src.lua` ) and the effect set / mesh names in the OpenXET errors ( `RENA_GUNGNIRMAGICSQUAREM01.XET`, `LIRE_SI_SA_GUNGNIR_MESH05.XET`, ... ) are the optional mesh companions CH2d's review already judged harmless.

## Task

1. Wind Blast: read `3262b6e0` in Ghidra for the only-target test, port it ( own flag, `#else` March ), prove with a `CE_HIT DamageEffect_SA_RSR_WindBlast_Arrow` + `..._Explosion` in the next log.
2. Gungnir: diagnostics as above, then the fix. If the cause is outside Rena's class code, a routed item with the owning stage.
3. One build for both; one hand-off: unit 92, set 1 ( slot Q Wind Blast, slot R Gungnir ); the hand-off names the log lines to read.
4. Record JUNE_BUGS rows for any June quirk found, the funcmap rows, a Done note in the plan doc ( under CH3b ), and keep `STATUS.md` current.

No hardcoded enum literals, no `June` in names, one-line comments with the phase tag `// char-rena CH3b2`, CP949 files through `bytepatch.py` only, `flagcheck` / `junediverge --check` / `enumscan --check` before the commit ( CLAUDE.md ).
