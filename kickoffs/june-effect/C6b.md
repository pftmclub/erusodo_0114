# June effect completion - C6 step 2 - port the five tail callees

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C6b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C6a (done 2026-10-06), C6a2 (done 2026-10-07: added the EffectSet stamps below, moved the buff list to C6c), C5 (`31f64c00` needs C5's `ADDITIONAL_ATTACK_TRIGGER_TYPE` list member) |
| Flag | `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C6b-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C6 - Spawner"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context" "Phase 10c"
```

Also read: `june2017_re/funcmap.tsv`, the rows tagged `Effect C6a` (`grep "Effect C6a" june2017_re/funcmap.tsv`).
The "C6a findings" section at the end of this file supersedes the rebuild plan's Phase 10c text where
the two disagree.

The inline prompt below predates `tools/`: wherever it says to build, deploy, check the exe,
make CP949 edits, run a unifdef or read logs by hand, use `tools/build.py --deploy`,
`tools/bytepatch.py`, `tools/flagcheck.py` and `tools/logs.py --tag JUNE_FX` instead (the x2-phase
skill's Tools and Finish lists). Never launch X2_offline.exe; ask the user. Ghidra work uses the
`ghidra` skill; identification verdicts go into `june2017_re/funcmap.tsv`.

## Task

**This session does step 2 only**, from C6a's verdicts. Under the no-partial-June-updates rule it also corrects March's existing hit and die spawners.

Verbatim kickoff prompt from the plan:

```
June2017 effect completion C6 (= rebuild plan Phase 10c steps 1-2). Read the rebuild plan's
Starting context and Phase 10c, JUNE2017_EFFECT_COMPLETION_PHASES.md (C6), funcmap.tsv. Step 1 on
Opus: identify the CEffect flags, entry fields and invoke-rate gate Phase 10c lists, verdicts into
funcmap.tsv. Step 2 on Sonnet: port the five tail callees as CEffect methods, call them from the
hit and die spawners under SERV_IRUHADEV_JUNE_SPAWN_INHERIT. Never port an offset. CP949
byte-level; build, deploy, ASK before launching; update matrix and MODS.md.
```

## C6a findings (2026-10-06; rows tagged `Effect C6a` in `june2017_re/funcmap.tsv`)

These supersede the rebuild plan's Phase 10c text where they differ.

- **Who is `this`.** `31f8fdb0` and `31f64c00` run on the parent with the child as the argument. `31f649d0`, `31f65e40` and `31f65f90` run on the child: the rate is the spawning entry's `+0xb0`, and the bool is the parent's `m_bIgnoreDefence` / `m_bIgnoreStateDefence`. `31f65e40` and `31f65f90` run only when the entry's `+0xb4` / `+0xb5` is set.
- **Tail order** (row `31f810c0 @31f81984-31f81b35`):
  - In the hit and die spawners: CreateInstance, `31f8af50`, `31f8fdb0`, the existing blocks (INHERIT_BUFF_FACTOR ... max life from entry `+0x84`, then the `m_eFirstAttack`/`m_RelateSkillData` copy), then `31f649d0`, `31f64c00`, `31f65e40`, `31f65f90`.
  - Keep that order. Max life from `+0x84` runs after `31f8fdb0`, so it overwrites the `b5c` life-rate multiply.
- **Call sites: five, not two.**
  - The hit spawner (`X2DamageEffect.cpp` ~:6910) and the die spawner (~:7266).
  - The `CREATE_DAMAGE_EFFECT` loop in OnFrameMove (~:2880). June calls all five callees there: `31f8fdb0 @31f89d8b`, then `31f64c00 @31f89dbc` right after INHERIT_BUFF_FACTOR, and the other three at the end.
  - `RT_CREATE_DAMAGE_EFFECT` in AttackResult calls `31f8fdb0` only (`@31f86140`).
  - Touch-land is C7's, and it reuses these methods.
  - The no-partial rule covers the loop and RT_CREATE.
  - 7d inlined `31f8fdb0`'s INHERIT_DAMAGE branch in the loop (`:2891`). Replace that copy with the method under the new flag, keeping the old code in `#else`; do not apply it twice.
- **March names (never port an offset):**
  - Already in March, no new member needed:
    - `+0xb00` = `m_iIndex`
    - `+0xb04` = `m_iType`, not copied by `31f8fdb0`
    - `+0xb66` = `m_bInheritDamageFromMaster`
    - `+0xb32` = `m_iApplyEffectAlphaConditionByOption` (int in March, byte in June)
    - DD `+0x528/+0x529/+0x541` = `m_bIgnoreDefence` / `m_bIgnoreDefenceOnPvp` / `m_bIgnoreStateDefence`
    - DD `+0x40` = the INHERIT_BUFF_FACTOR vector
    - DD `+0x53c` = `m_eDamageTrigger`
    - CEffect `+0xd8` = `m_pDamageDataNext` plus `m_vecDamageDataNextChain`
  - New, with proposed names:
    - `+0xb44` `m_bInheritIgnoreStateDefence`
    - `+0xb45` `m_bInheritCountAsOwnerSkillHit`, with DamageData `+0x5fd` `m_bCountAsOwnerSkillHit`
    - `+0xb46` `m_bInheritBuffFactorToChild`
    - `+0xb47` `m_bInheritIgnoreDefence`
    - `+0xb5c`/`+0xb60` `m_bInheritMainEffectLifeTimeRate` / `m_fMainEffectLifeTimeRate`
    - `+0xb64` `m_bHyper`
    - `+0xb65` `m_bInheritAttackTriggerType`
    - DamageEffectData `+0xb0` `fInheritDamageRate` (default 1.0), `+0xb4`/`+0xb5` `bInheritIgnoreDefence` / `bInheritIgnoreStateDefence`
  - Ctor defaults: 0 / false.
- **Dead in the hit and die spawners.** The child's own `+0xb66` is set only by the CREATE loop, so `31f8fdb0`'s INHERIT_DAMAGE branch never fires there. That matches June.
- **Already in March, nothing to port:**
  - `+0x7a4`/`+0x7a8` are `m_eFirstAttack` / `m_RelateSkillData`.
  - The invoke-rate gate: June gates on `m_iType != DET_ARROW_OF_EXPLOSION` exactly as March does, with the same `-1` default.
- **June quirks to keep verbatim** (each gets a `JUNE_BUGS.md` row):
  1. `31f8fdb0`'s `b47` branch copies `m_bIgnoreDefence` but not `m_bIgnoreDefenceOnPvp`.
  2. `31f65e40` and `31f65f90` mark the entries 1 even when the value passed is false, so a child can lose its own Lua IGNORE_DEFENCE.
  3. `31f649d0`, `31f65e40` and `31f65f90` do not null-test the DAMAGE_DATA_NEXT entries of June's `+0xd8` vector. That vector simply has no element when an effect has no NEXT table, but March's `m_DamageEffectDataInLua.m_pDamageDataNext` is a pointer that is NULL then (`SAFE_DELETE` at X2DamageEffect.cpp ~:2033). So in March: apply to `m_pDamageDataNext` only when it is non-null, then to each `m_vecDamageDataNextChain` entry with no test. The quirk to record is only the untested chain entries. Without the `m_pDamageDataNext` check, the first ported Eve or Add skill crashes on every effect that has no NEXT table.
- **`31f64c00` needs C5.**
  - It copies values 29/30 (`DTT_RCR_TRANSCENDENCE_*`, June-only, Rose) of the parent's `ADDITIONAL_ATTACK_TRIGGER_TYPE` list, unconditionally. Then, under `b65`, it copies `m_eDamageTrigger` and the whole list.
  - The list member is C5's, and so is any June DTT enumerator. `31f64c00` itself belongs to C6b (C5's kickoff says so). Run C6b after C5. For 29/30, use C5's enumerators if C5 added them; otherwise use literals with the June Enum.lua names in a comment, since March's enum ends at 6.
  - **C5 landed (2026-10-06):** the member is `CX2DamageManager::DamageData::m_vecAdditionalAttackTriggerType` (`std::vector<int>`, cleared on every parse and by `SimpleInit`), and `32610fb0` is `DamageData::JuneIsAttackTrigger( int )`. C5 added no DTT enumerators: use literals 29/30 with the June Enum.lua names in a comment. `m_eDamageTrigger` is March's `+0x53c`.
- **`+0xb64` writer (EffectSet).**
  - `OnFrameMove_CreateEffect @321ef6d5-321ef6d8` sets the damage effect's `m_bHyper` from `EffectSetInstance::m_bHyper`.
  - It sits right after the `USE_IS_RIGHT_WHEN_CREATE` block that Phase 4a ported (`X2EffectSet.cpp` ~:1088). Port it with the copy, so `X2EffectSet.cpp` is in Touches.
- **The rest of that EffectSet block was identified by C6a2.** See "C6a2 findings" below: the three stamps are this phase's; the buff list is C6c's; the runtime modifier list `321db1b0` (instance `+0x170`) stays with the character ports (checklist 7a).
- **Expected play-test.**
  - No March code sets any gate flag (all setters are Add, Eve, Rose, Elsword and Ain class code: see the checklist section 7 addendum), and every entry's rate is 1.0. So the play-test can only show that the tail runs and changes nothing.
  - **The one check that can fail:** `m_iIndex` now reaches hit, CREATE and RT_CREATE children, and Rena Night Watcher looks up her effects by index (`CheckDamageEffectType`, arrow detonation). Play a Night Watcher with Arrow of Explosion, Delayed Firing and Explosion Trap, the skills whose effects have hit children. Look for detonations behaving as before, and nothing triggering twice. That is June parity, but it is the one place a regression would show.
  - Four things do change, in every case the same as June:
    - Hit and CREATE children now inherit the parent's `m_iIndex`, as die children already did.
    - The `m_bHyper` flag is carried down.
    - (C6a2) Every EffectSet-made damage effect gets `m_iIndex` 0 instead of -1 (safe: see "C6a2 findings" item 6).
    - (C6a2) Its `m_iJuneAttackerStateID` is the owner's state when the set was **played**, not when the effect was created. The only reader is buff 9b's Sinister Mind seed (`X2BuffTemplet.cpp:2406`, BTI 767), which only June class code applies, so this is inert in March data.
  - The `JUNE_FX` line should print the entry rate, the four gate flags and the index per spawn, throttled.

## C6a2 findings (2026-10-07; rows tagged `Effect C6a2` in `june2017_re/funcmap.tsv`)

C6a2 identified the rest of EffectSet `OnFrameMove_CreateEffect`'s damage block (`321ee600 @321ef5e6-321ef6d8`).
This phase ports the three instance stamps below, with `m_bHyper`. The buff list (`321ee480` x2) moved to **C6c**
(see the end of this file). All of it goes under `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`. It is the same logical change as the
tail: what a June damage effect carries from whatever created it. `31f8fdb0` then copies `m_iIndex` and `m_bHyper` on to
the children, so splitting it would leave a flag state that matches neither March nor June.

**Port (June order: SetLockOn, the stamps, the buff list (C6c), velocity tag 1, IS_RIGHT, `m_bHyper`):**
1. **`EffectSetInstance` members** (`X2EffectSet.h`, ctor initialiser near `X2EffectSet.cpp:4465`):
   - `m_iDamageEffectIndex`: int, ctor **0** (`321df230 @321df510`; not -1). June `+0x1b8`.
   - `m_iOwnerStateIDOnPlay`: int, ctor **-1** (`@321df4a9`). June `+0x15c`.
2. **Two `CX2GameUnit` virtuals, base only** (`X2GameUnit.h`):
   - `JuneStampEffectSetIndex( EffectSetInstance* )`: June vslot `0x248` `3226e220`, base no-op.
   - `GetJuneDamageEffectIndex() const`: June vslot `0x2e0` `31abe300`, base `return -1`.
   - The Luciel and Ain overrides go to checklist 7a.
   - `X2GameUnit.h` already sees `CX2EffectSet` (it holds `CX2EffectSet::Handle` members), so declare the parameter as
     `CX2EffectSet::EffectSetInstance*`.
3. **`PlayEffectSet`, the game-unit overload** (`X2EffectSet.cpp:3142`):
   - After the `m_vScale` / `GIANT_UNIT_GIANT_EFFECT_TEST` block and before `m_bCustomPosition`:
     `pGameUnit->JuneStampEffectSetIndex( pEffectSetInstance )` (`@321e2029-321e2033`).
   - After the custom-position block, the last store before the push_back / return:
     `pEffectSetInstance->m_iOwnerStateIDOnPlay = pGameUnit->GetNowStateID()` (`@321e20bf-321e20ce`; unit vslot `0x10c` =
     `3208ec80` = GetNowStateID).
   - The square and pet overloads write neither (June), so their sets stamp 0 / -1.
4. **`DamageData::m_iJuneDamageEffectIndex`**, a new int (`X2DamageManager.h`; June `+0x638`).
   - `SimpleInit`: -1 (`31f8f47b`).
   - `CX2GameUnit::SetDamageExceptionProcess`, after the Phase 9b serial wrap (`X2GameUnit.cpp:~17059`):
     `= GetJuneDamageEffectIndex()` (`@32299712-3229971e`).
   - `CEffect::SetDamageDataNext` (`X2DamageEffect.cpp:~9649`, the field-by-field copy into `m_pDamageDataNext`): copy it
     (`31f799c0 @31f79b87/8d`). June copies neither `+0x5e0` nor `+0x5e8` there, which matches Phase 9b.
   - The `DAMAGE_DATA_NEXT` parse (`X2DamageEffect.cpp:~1635`, June `31f79c20 @31f79ce9`): reset it to -1 after the
     parent copy and before `SetDamageDataFromLUA`.
   - The copy ctor (implicit `operator=`) carries it already (June `31e9c190`).
   - The only readers are Luciel's (checklist 7a). In March it is written and never read.
   - **June also resets `+0x5e8` (`m_iJuneAttackSerial`) to -1 there (`@31f79ce3`); Phase 9b did not.** That member is
     june-buff's: name it in the Done note as an open item and leave it alone.
5. **The stamps**, in `OnFrameMove_CreateEffect`'s `ET_DAMAGE_EFFECT` case (`X2EffectSet.cpp:1039-1123`, inside `if( NULL != pEffect )`).
   - Put one block after `#endif SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME` (`:1097`) and before `#ifdef RIDING_SYSTEM`.
   - The block is unconditional for every owner (`@321ef639-321ef66d`):
     - `pEffect->SetIndex( m_iDamageEffectIndex )`;
     - `GetDamageData().m_iJuneDamageEffectIndex = m_iDamageEffectIndex`;
     - `GetDamageData().m_iJuneAttackerStateID = m_iOwnerStateIDOnPlay`, nested in `#ifdef SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` (the member's flag);
     - then `m_bHyper` (`@321ef6d5`), already in this phase's scope.
   - These overwrite what `SetDamageExceptionProcess` wrote during `CreateInstance` (the state at creation, and -1). That is June's behaviour.
   - **Order:** June runs SetLockOn first. March's SetLockOn writes neither velocity nor follow-line, so this order is equivalent (funcmap `321ef5ea-321ef632`).
6. **`m_iIndex` = 0 on every EffectSet-made effect is safe in March** (funcmap `3226e220`):
   - Rena's indices are `unitIndex*100 + 1..100`.
   - `GetInstanceByIndex` is Rena-only and fed only from `DET_*_GROUND` effects.
   - No code compares a CEffect index with -1.
   - The Night Watcher check in "Expected play-test" above still applies.

**Not this phase:**
- **The first-attack / relate-skill copies are compiled out.**
  - This covers `@321ef60a-321ef632`, the spawners' `m_eFirstAttack` / `m_RelateSkillData` copies in the tail order above, and SetDamageDataNext's.
  - They sit under `ADD_RENA_SYSTEM`, which is **off** (`KTDXLIB/Always.h:238`), so C6a's "already in March" verdict was wrong.
  - They go with the `ADD_RENA_SYSTEM` decision (checklist section 3). Leave those `#ifdef`s untouched.
- **EffectData `+0x118`, the `321ee480` call `@321ef676`, is dead in June.** No writer exists anywhere. Add no member and no call.
- **The runtime modifier list `321db1b0`** (instance `+0x170`) stays in checklist 7a.

**Touches added by C6a2:**
- `X2EffectSet.h` (two instance members)
- `X2GameUnit.h/.cpp` (two virtuals, the SetDamageExceptionProcess tail)
- `X2DamageManager.h` (member, SimpleInit)
- `X2DamageEffect.cpp` (SetDamageDataNext, NEXT parse)

`X2GUUser.cpp` is no longer this phase's (it moved to C6c).

**JUNE_FX line** (under `SERV_IRUHADEV_JUNE_FX_DEBUG`, throttled): the stamped index, the state ID and `m_bHyper` per EffectSet damage effect.

## Moved to C6c (C6a2, 2026-10-07)

The effect-set buff list is no longer this phase's: the buff port's "Added from the buff port" section, `321e3650` /
`321ee480` and the SIEE 15 call site in `X2GUUser.cpp`. They are `kickoffs/june-effect/C6c.md`, with their own flag.
C6c runs after this phase because both edit the same `OnFrameMove_CreateEffect` block.

## Added by the C5 review (2026-10-07; the user: "C6b handles it")

AttackResult's `RT_CREATE_EFFECT_SET` case (`31f85910 @31f86158-31f8672a`) differs from March's single `PlayEffectSet` call in two ways.
Funcmap rows `31f85910 @31f861e2-31f8672a` and `31f85910 @31f8662d-31f86711`. Identification is done (Opus, C5 review); this phase transcribes.
Own flag: `SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET` (a separate logical change from `_SPAWN_INHERIT`). Keep the C5 TRACE_NEAREST_BONE block and the LIFTTIME/DELAYTIME blocks where they are.

**June's shape** (handle starts at -1, `@31f86193`; every path then falls through to the `GetEffectSetInstance` block at `@31f8672a`):
1. **Trace unit** (`CREATE_EFFECT_SET_TRACE_UNIT` and a defender, `[ESP+0x24]`):
   - `life` = the LIFTTIME delta, else -1 (`@31f861a1-31f861de`; today's code).
   - **Raid retarget gate** (`@31f861e2-31f8627f`), all of these must hold:
     - `32481fa0`: June state `0x15` = `XS_BATTLE_FIELD` (funcmap rows 171/1058: June's state numbers differ from March's), and
       `g_pMain +0x115c`->`3266cdc0( current battle-field ID )` is true. That ID is `[g_pData +0x124 +0x10]`, i.e.
       `GetBattleFieldManager().GetBattleFieldIdWhereIam()`. `3266cdc0` is a switch on exactly 40033, 40034, 40039, 40040,
       40041, 40042, 40043 and 40044, June's `VMI_BOSS_RAID_PERKISAS*` / `VMI_BOSS_RAID_ELTRION_MK2_*` (June Enum.lua).
     - The unit at `g_pMain +0x115c` `+0x8` (`3266cb90`, an observer pointer, `31e56700`) exists, and its UID (vslot `0x54`)
       equals the trace unit's.
   - **If the gate holds:** find the nearest unit in the game's unit list (`31aada50 +0x304..+0x308`) that is an NPC
     (`+0x264 == 1`) whose NPC ID is `0x621` = `NUI_FIELD_BOSS_COLLISION_BOX_1` (March has the enum, `X2UnitManager.h:2154`).
     - Distance `31c7b560( impactPoint (CEffect +0x660 = DamageData +0x460), unit vslot 0x138( 0 ) )`, strict `<`, from 99999 (`33441c60`).
     - No alive check.
     - If found (`@31f86321`): March's call with the trace argument = that NPC. If not (`@31f8642d`): March's call unchanged.
   - **If the gate fails** (`@31f86521`): March's call.
   - The C5 call `@31f867ad` still passes the original trace unit (`[ESP+0x24]`), not the box NPC.
2. **No trace unit** (`@31f8662d-31f86711`): **only with the main-effect mesh** (EBP = `pMeshInst`); without one, no effect set at all.
   - The call is `PlayEffectSet( name, owner, NULL, false, m_fPowerRate, -1.f, (1,1,1), true, mesh->GetPos() (+0x3f0),
     rot, dir )`, plus June's trailing 1.0.
   - `rot` = the mesh's `+0x5b4/+0x5b8/+0x5bc` (CXMeshInstance rotation degrees).
   - `dir` = `31c82e40` on the mesh: (1,0,0) rotated by `(+0x5b4+0x5a8, +0x5b8+0x5ac, +0x5bc+0x5b0) * 33441c58`. Identify the March equivalent (likely a mesh-instance rotation helper) or transcribe it.
   - **LIFTTIME is not applied here** (lifetime is the constant -1). Today's March port applies it on both paths, so it must move under the trace-unit branch. Add a `JUNE_BUGS.md` row only if you judge it a quirk.

**Reach in March:**
- Branch 1's retarget never fires: March has none of those eight maps and no June raid object at `g_pMain +0x115c`. Port the gate verbatim, and stub the boss-unit source under plan rule 5: return NULL, log once to `[JUNE_FX]`, so March's call runs.
- Branch 2 is live: 2 of 17 `CREATE_EFFECT_SET_NAME` blocks in March `data036/DamageEffect.src.lua` have no TRACE_UNIT, `EffectSet_RAVEN_NUCLEAR` and `EffectSet_SP_B_MASK`. Their sets move from the owner to the main effect's position and rotation.
  - Play-test: Raven's Nuclear (skill) and whatever uses `SP_B_MASK` (grep the DamageEffect name). The set should appear where the effect is, not on the caster.
