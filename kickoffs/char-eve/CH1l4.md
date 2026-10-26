# June character Eve (Stage 7) - Phase CH1l4 - Class core transcription: CommonFrameMove and the helpers only it reaches (split from CH1l by CH0)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH1l4` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l40 review (Opus helper phase). One at a time with the other CH1 groups: they all edit `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (defined from CH1a on; what it guards stays unreferenced until CH1m-CH1 wire it) |
| Log tag | none (inert code, no play-test) |
| Review | `kickoffs/char-eve/CH1l4-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$6=="CH1l4"' june2017_re/char_eve/ch0_class_map.tsv
awk -F'\t' '$8=="CH1l4"' june2017_re/char_eve/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1l4/' june2017_re/char_eve/ch0_checklist_rows.tsv
```
Then CH0's Done note. Pull a checklist section by heading only when a row above names it.

## Task

Transcribe every function the class map gives phase `CH1l4` (CommonFrameMove 329c5bc0 and its helpers: Dimension Link / summon upkeep, Thousand Star modifiers (7a), the 3299b7f9 stack-count call (7)) from Ghidra, **whole**, as an inert member of `CX2GUEve` named `June_<March name>` (or `June_<STATE>_<Slot>` where March has no pair), under `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`. The plan's "Transcription rules (CH1a-CH1l)" apply:
- Inert by design: register nothing, leave `InitState*` and every virtual body alone.
- A class-range callee (`0x32965dd0`-`0x329cf86f`) owned by a later group is never a bare declaration (it does not link, CH1c review): transcribe it here and move its class-map row to this phase, or, when it is large, move the caller to the callee's phase and amend that kickoff (as CH1c did with `329ac330`); an engine callee is called through its March counterpart (`funcmap.tsv`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai, `s3e_bybfi_callers.tsv`); the 7c alpha-tag calls inside these functions (`c10a_alpha_sites.tsv`).
- `X2GuEve.*` are CP949 with mixed line endings: `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1l4` has a `June_` member (the Done note lists them with June addresses); the build passes. Nothing to play-test: the hand-off says so and names the next phase.

## Amended by CH0 (2026-10-26)

- **Functions: 37** (`awk -F'\t' '$6=="CH1l4"' june2017_re/char_eve/ch0_class_map.tsv`; helper 36, override 1).
  June addresses: `329673a0` `32968c50` `32975ac0` `32979350` `3297a500` `3297b370` `3297b720` `3297bad0` `32991090` `32991ac0` `32994a10` `32994cd0` `32996c50` `329986e0` `3299ad60` `3299b000` `3299b440` `3299b790` `3299f470` `3299fe60` `329a3040` `329a3760` `329a7d00` `329ae6e0` `329ae9a0` `329aea60` `329b2f60` `329b4a60` `329bc6e0` `329c12a0` `329c3420` `329c3bb0` `329c5bc0` `329cb2f0` `329cc200` `329cc4e0` `329cd740`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L237 (322a7660 set-stack-count, Eve caller 3299b7f9 (BFI 812)); §7a L296 (AddDamageEffectModifier list); §7a L297 (Eve projectile modifiers); §7c L323-343 (167 alpha-tag sites 32982623..329c84f8); §7ai L806-819 (BFI call arguments, 3 s skill-name string).
- **7c alpha-tag sites inside these functions: 30** (`c10a_alpha_sites.tsv`, by containing function).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed); `31d006b0` CKTDGParticleSystem::CreateSequence( obj, name, const D3DXVECTOR3& pos, ... ) inline overload (KTDGParticleSystem.h:2007) (confirmed); `31c19f50` CKTDGParticleSystem::CParticleEventSequence::SetAxisAngle (KTDGParticleSystem.h:1385) (confirmed); `32290490` CX2GameUnit::UpDownCrashCameraNoReset (X2GameUnit.cpp:13104) (confirmed); `31f63d10` CX2DamageEffect::CEffect::SetLaserGroupID( int ) (X2DamageEffect.cpp:10757) (confirmed); `31f70040` CX2DamageEffect::CEffect::GetMainEffectScaleByUnit (X2DamageEffect.cpp:6206) (confirmed); `31c1a3d0` CKTDGXMeshPlayer::CXMeshInstance::SetMoveAxisAngleDegree (KTDGXMeshPlayer.h:1236) (confirmed); `31e56810` KObserverPtr<T>::reset / ~KObserverPtr (KTDXLIB/KObserverPtr.h) (confirmed); `31f83a80` CX2DamageEffect::DestroyInstanceHandleSilently( CEffectHandle& ) (X2DamageEffect.h:1764) (confirmed); `33086b70` D3DXPlaneIntersectLine (d3dx9 import thunk) (confirmed); `31c1a440` CKTDGXMeshPlayer::CXMeshInstance::SetScale( D3DXVECTOR3 ) (KTDGXMeshPlayer.h:1276) (candidate); `31cdd340` CKTDXDeviceManager::IsPlaying( const WCHAR* ) (KTDXDeviceManager.cpp:1095) (confirmed); `31e4c430` CX2DamageEffect::CEffect::GetPos (X2DamageEffect.h:974) (confirmed); `31e4cd60` CX2GameUnit::GetZVector (X2GameUnit.h:2545) (confirmed); `31f83a90` CX2DamageEffect::DestroyInstanceSilently( CEffect*& ) (X2DamageEffect.h:1762) (confirmed); `31f8adb0` CX2DamageEffect::CEffect vec3 getter (+0x510, unnamed) (candidate); `321cd590` ~(June CX2CustomizeManager item data struct) (candidate); `321cdbb0` June CX2CustomizeManager item data struct ctor (candidate); `32371c00` engine (unknown); `31f8ae10` engine (unknown); `3295cd30` CX2Eqip::SetShowAttachedParticle (X2Eqip.cpp:3212, June adds a 2nd arg) (candidate).

## Amended by CH1d (2026-10-26)

- `32994a10` / `3299f470` call `m_EveElectraSystem.June_DestroyPlaneAndCrystalEffect` (`32991a10`) and `32996c50` calls `June_AddEffectToSystem` (`32980580`) (all CH1d). June's `EveElectraEffectData` has two members March's lacks (+0x8c int, -1; +0x90 `std::vector<int>`; written by ctor `3297b6b0`, copied by `329cdf80`): CH1d found no reader in the class range, so check them while porting `ProcessSystem` `329c3bb0` and the effect data's frame move, and add them to the struct only if read.

## Amended by CH1e (2026-10-26)

- `3297b370` writes the Empress dash X's follow target into `m_pJuneDashXTargetUnit` (+0x9c24, field added by CH1e) from the effect in `m_pJuneDashXFireBall` (+0x9c20); use those names.

## Amended by CH1j review (2026-10-26)

- `329bc6e0`'s induction fire (March `X2GuEve.cpp` ~23337) takes the laser data's `+0xc8` `m_wstrJuneInductionLaserName` (empty = `EVE_EBS_INDUCTION_LASER`) or, when the main effect's attack type is `AT_SPECIAL` (`+0x208 == 1`), `+0xe4` `m_wstrJuneInductionLaserSpecialName` (empty = `EVE_EBS_INDUCTION_LASER_SPECIAL`); both fields were added by CH1j, use those names.

## Amended by CH1k (2026-10-26)

- The constructor map (`june2017_re/char_eve/ch1k_ctor_offsets.tsv`) shows June's `CX2GUEve` has no `m_timerSummoned` / `m_fSummonedLifeTime` / `m_ChargingBoosterSkillDataBasePtr` and no Atomic Shield pair (the mesh arrays start at `+0x95f0`). March's `CommonFrameMove` tail that destroys the Nasod Watch after `m_fSummonedLifeTime` has nothing to read in June: read `329c5bc0` and drop that block unless June keeps the timer elsewhere.

## Amended by CH1l3 (2026-10-26)

- **Functions: 35.** `329aea60` and `329cc200` (Psyker Kinesis release and its only callee) moved to CH3a (class map rows changed). `329b4a60` leaves its two calls out
  (`329aea60`, then the tail jump `32976fe0`) with `// June 329aea60 / 32976fe0: calls added in CH3a`; CH1l3's `June_NoStateChangeActionFrameMove` case 0x40 does the same.
- **Reuse, never copy again** (CH1l3, all `CX2GUEve` members): `June_GetSkillSlotDataMegaElectronBall` / `June_GetSkillSlotDataTaserPilum`, `MEGA_ELECTRONBALL_Data::June_Reset` (Taser and Kugelblitz: March's `Reset`),
  `EveElectraSystem::June_GetFormationFactor` / `June_StartFormationEffect`, `June_StartElCrystalSystem`, `June_IsPhotonBlinkSkill`, and the Eve-local copies `June_MarkSkillSlotUsed`, `June_ReserveSkillUse`,
  `June_StateChangeBySkillSlot`, `June_SkillMemoMpRefund`, `June_CheckSkillUseNotPve` / `NotDungeon`, `June_UserNoStateChangeActionFrameMove`; the fields `m_iJune8ea0` / `8ea4` / `8ea8` / `8eb0`, `m_abJune90d3` / `90d0`.
  `329cc370` is the existing `CX2GUUser::IsRidingKeySkillWaiting`; `32972a70` / `329919a0` / the three `ResetParticle`s are March's members.

## Amended by CH1l40 (2026-10-26)

The helper phase named every unmapped engine callee CH0 left open for you (funcmap rows `Char Eve CH1l40`). Call them by these March names:
- `31f649d0` / `31f65e40` / `31f65f90` = `CX2DamageEffect::CEffect::JuneApplyInheritDamageRate( float )` / `JuneSetIgnoreDefenceDown( bool )` / `JuneSetIgnoreStateDefenceDown( bool )` (C6b, protected; `CEffect` declares `friend class CX2GuEve` under `_CORE`). Call them on the child, as June does.
- `31f8ae10` = `CKTDGXMeshPlayer::CXMeshInstance::SetManualDirDegree( D3DXVECTOR3 )` on the child's **main effect** (`GetMainEffect()`, CH0 said "the CEffect": a slip); the `+0x494 = 1` store after it is `UseManualDirDegree( true )`.
- `32371c00` = `CX2DamageEffect::CEffect::SetAttackTime( D3DXVECTOR2 )` (`+0x840` is `m_AttackTime`); `329bc6e0 @329c0ebb` passes `( 0, [334355b8] )`, read the float from the binary.
- `31f8adb0` = `CXMeshInstance::GetScale()` on the main effect (`m_vScale`, `+0x510`); `31c1a440` = `CXMeshInstance::SetScale( D3DXVECTOR3 )` (confirmed by the same offset).
- `3297b720` does not touch the customize struct (CH0 slip): it is the INHERIT_CRITICAL_RATE helper (skill-memo test, `m_fCriticalRate` of the effect and its NEXT tables, then `m_bInheritCriticalRate = true`); reach the NEXT tables with `JuneGetNextTables` (the friend covers it). The customize struct `321cdbb0` / `321cd590` belongs to `3297b840` (CH2a2) and goes to Stage 12.
- The modifier list applies tags 5 / 12 / 16 / 20 (`CX2EffectSet::ApplyDamageEffectModifiers`); none of your callers pushes a tag.

## Amended by CH1l40 review (2026-10-26)

`3297b720` in detail (read from Ghidra; the bullet above said "skill-memo test", which is loose): skill templet `32206450( [arg +0x7c4], 1 )`, its type - 6 through a byte table; the value comes from `32007ce0` = `June_TryGetEquippedSkillAbilityValue( 0x33, 0x6343, 0, out )`; then for the effect's own DamageData (`+0x200`) and each non-NULL NEXT table: if `32610fb0( 0x27 )` (= `DamageData::JuneIsAttackTrigger`) is false, `m_fCriticalRate += value` and `31f8b860` pushes `0x27` onto `m_vecAdditionalAttackTriggerType` (`+0x544`); last, `+0xad4 = 1`. `0x27`, `0x33` and `0x6343` have Enum names to look up (trigger type, skill ability, skill id): the no-literal rule applies.
