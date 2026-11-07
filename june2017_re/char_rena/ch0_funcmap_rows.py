"""char-rena CH0: append the `Char Rena CH0` rows to june2017_re/funcmap.tsv (idempotent: skips when the tag is already there).
    python june2017_re/char_rena/ch0_funcmap_rows.py [--dry]
Rows: the class's registration and dispatch functions, the 24 uncarved overrides that change a phase's reading, the natural-force pieces
(class, gauge data / UI, engine callers), the trap spawns and summon-info setters, the Night Watcher engine hook, the secondary vtables
and the shared callees CH0 identified (ch0_unmapped_identified.tsv). Copy of char_raven/ch0_funcmap_rows.py's purpose; rows written by hand.
"""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
FM = os.path.join(HERE, '..', 'funcmap.tsv')
TAG = 'Char Rena CH0'
C = 'CX2GULire_ElvenRanger::'
R = [
 ('32ca9dc0', C + 'InitStateCommon', 'confirmed', 'registers the common states and every line state (ch0_reg.tsv; ch0c_listing_check 202 / 202); LESI_DASH per-arm row @32caa647 (class 114: StartFuture 32c6e0c0 / FrameMoveFuture 32c6e0f0); CH1m'),
 ('32cadc90', C + 'InitState (vslot 0x7a4)', 'confirmed', 'class switch -> +0x8ac8 line index (3/12/13/24 -> 0, 102 1, 103 2, 114 3), InitStateCommon 32ca9dc0, base 3202b110, START / WIN / LOSE rolls (9 / 11 / 12; LESI_RNW_WIN2 sets byte +0x9714), inline line states, tail vslot 0x690 / 0x684( 0 ) / 31fd8f00 / 32c96d60 / 32c85200( 1 ) when NF > 0; CH1m'),
 ('32cb06d0', C + 'SetEquippedSkillFuncBySkillSlotIndex (vslot 0x7b4)', 'confirmed', 'three switches: type (byte 32cb3004 / jump 32cb2ff4), 6-case hyper (jump 32cb31e0), skill id (byte 32cb30fc / jump 32cb3010); ch0_skill_listing_rows.tsv 157 rows; default -> LESI_SI_LE_POWER_ATTACK1; CH1n'),
 ('32ca78b0', C + 'InitPassiveSkillState (vslot 0x690, uncarved)', 'confirmed', 'registers no state; pushes the delegate thunks @32ca7d17 / @32ca7e83 / @32ca8111 / @32ca8169 (checklist 7am names 32ca7860, which is Win); CH4a'),
 ('32ca7860', C + 'Win (vslot 0x90)', 'confirmed', 'ends with ret 4; InitPassiveSkillState 32ca78b0 follows uncarved'),
 ('32ca81b0', C + 'CommonStateStart (vslot 0x7c4)', 'confirmed', 'reads BOW_SKILL_STATE (+0x981d @32ca82c2), CREATE_TRAP_BLADE, DAMAGEDATA_CHANGE_TIME*; 31fe28f0 with a skill-templet byte @32ca898c; CH1j'),
 ('32ca2b60', C + 'CX2GULire_ElvenRanger (ctor)', 'confirmed', 'vtable 335185bc store 32ca2bc4; only caller 3230f289 (unit factory); CH1k'),
 ('32ca42f0', C + '~CX2GULire_ElvenRanger (dtor body)', 'confirmed', 'vtable store 32ca431f; scalar deleting dtor 32cb6c50 (slot 0); adjustor thunks 32cb6a00 (ecx-0x190) / 32cb6a10 (ecx-0x1a0) in the secondary vtables @335185b0 / @335185a8'),
 ('32c6da50', C + 'DamageReact (vslot 0x60, uncarved)', 'candidate', 'role from the vtable slot (sibling class maps); CH1h'),
 ('32c9b3d0', C + 'HyperModeBuffEffectStart (vslot 0xc4, uncarved)', 'candidate', 'transcendence row 7ao (CH3d); function CH1h'),
 ('32c6a970', C + 'SpecialAttackEventProcess (vslot 0x7f4, uncarved)', 'candidate', 'parts 32cac200 / 32ca50b0 / 32c89a20; CH1k'),
 ('32c83320', C + 'vslot 0x824 (uncarved): state animation rate', 'candidate', 'base 3208dd70; reads a skill level / templet value (Agile Movement, checklist 7av); CH1k'),
 ('3351666c', 'CX2GULire_ElvenRanger::FriendshipNatureData vtable', 'candidate', 'COL reads TimeLimited (shared layout with Arme 3351d090); slots 32cb4270 (scalar dtor, writes the vtable @32cb427a), 32cb3500, 32cb4200; March X2GULire_ElvenRanger.h:265 (live); CH1h'),
 ('33516854', 'CX2GULire_ElvenRanger::MemoConditionRena vtable', 'confirmed', 'TD 3373d730; slots 32c69d80 (shared base), 32c6d4d0 (decide, CH4b), 32c6d4f0 (thunk)'),
 ('32cb50c0', C + 'GetNaturalForceCount', 'confirmed', 'gage +0x114 protected int; callers ApplyHyperModeBuff 322985fa, NPC AttackResultByType 327745ec / 327745f7; March .cpp:23722 (__forceinline, ADD_RENA_SYSTEM); CH1q'),
 ('32cb50e0', C + 'IsEmptyNaturalForce', 'confirmed', 'caller NPC AttackResultByType 327745b9; March .cpp:23741; CH1q'),
 ('32cb6290', C + 'UpNaturalForce', 'confirmed', '+n, +1 more when GetRandomFloat (vslot 0x1cc) < protected float +0x6c50, SetChanged, SetNow 32a36df0, SetNaturalForceBuff( 1 ); callers ApplyHyperModeBuff 3229862d (5), F7 key 322ed57f (1); March .cpp:23730; CH1q'),
 ('32c85200', C + 'SetNaturalForceBuff', 'confirmed', 'June stacks BFI 355 per point (March: on / off at 3); March .cpp:23766; CH1q'),
 ('32c9f7a0', C + 'ConsumeNaturalForce', 'confirmed', 'June also UpNowMp( n x float +0x97e0 ); March .cpp:23710; CH1q'),
 ('32c89200', C + 'SetNaturalForceCount', 'confirmed', 'March .cpp:23757; CH1q'),
 ('32c9f830', C + 'SetNaturalForceEffect', 'confirmed', 'EffectSet_Rena_NF_Loop01..04 / _ALL; March .cpp:23871; CH1q'),
 ('32ca7130', C + 'AttackResultByType (vslot 0x4e0)', 'confirmed', 'NATURAL_FORCE_VALUE_PER_HIT 3.0 @32ca73fd (March 6); March .cpp:23599 (ADD_RENA_SYSTEM only); CH1q'),
 ('32ca7060', C + 'CreateGageData (vslot 0xd0)', 'confirmed', 'new CX2RenaGageData for my unit; March .cpp:23580 (ADD_RENA_SYSTEM only); CH1q'),
 ('3249b244', 'CX2RenaGageData vtable', 'confirmed', 'TD 33734c20, size 0x12c; ctor 32498a30, copy ctor 32a37e00, dtor 32498e00; Update 32a34f20, InitWhenGameIsOver 32a34f80, GetMyPlayStatus 32a32500, SetMyPlayStatus 32a34fa0 (CAC_NATURAL_FORCE 5 @32a34fae), Clone 32a36060, Copy 32a34fe0; SetChanged 32a317d0, SetNow 32a36df0, SetNFBuffMode 32a317b0; March X2GageData.* (ADD_RENA_SYSTEM); CH1q2'),
 ('3353363c', 'CX2RenaMyGageUI vtable', 'confirmed', 'TD 3373e378, second vtable 33533624; ctor 32e64760, dtor 32e725a0, InitUI 32e6e190 (DLG_Rena_NaturalForce.lua), OnFrameMove 32e71970, UpdateNaturalForceUI 32e5ec10; created by CX2GageManager::CreateMyGageUI 3248e290 case 3248e531; CH1q2'),
 ('32c99690', C + 'CreateEvoke', 'confirmed', 'CreateNPCReq( NUI_EVOKE_TRAP_LIRE 1020 ) @32c99a1a (eTeam -1, eAllyTeam = team); callers 32ca6810 / 32ca6860; March .cpp:20053; CH2d0'),
 ('32c9e250', C + 'CreateAngerOfElfTrap', 'confirmed', 'CreateNPCReq( NUI_RENA_ANGER_OF_ELF 1420 ) @32c9e42e; caller 32ca6fb0; March .cpp:22750; CH2d0'),
 ('32c95790', C + 'CreateEntangleTrap (dead in June)', 'confirmed', 'CreateNPCReq( 457 ) @32c95882; only caller 32c95890 has no reference in the exe; June Entangle uses damage effects (32c9e7b0)'),
 ('32ca0e60', C + 'LESI_RNW_DASH_XZZ_FULL_CHARGE_END End', 'confirmed', 'CreateNPCReq( NUI_RENA_TARGETING_GUNGNIR 2083, fDelay 0.3f ) @32ca11f2; CH1g2 (spawn rule CH2d0)'),
 ('32c85900 32c86340 32c863a0 32c87860 32c878f0', C + 'trap summon-info setters', 'candidate', 'called only from CX2Game summon code 3230bbbd / 3230c423 / 3230c8b6 / 3230c712 / 3230bc03 (SetUserSummonedNPCInfo 3230c1a0 region); CH2d0'),
 ('31fcd700', 'CX2GUUser: Night Watcher hook (class 114 -> 32ca6cd0)', 'confirmed', 'if unit class == 0x72 call 32ca6cd0; callers 32037afb, 322739d9, 3279bbd8; no March counterpart found; CH3c0'),
 ('32ca6cd0', C + 'Night Watcher engine hook body', 'candidate', 'reached only from 31fcd700; calls 32c9aae0, 32cb5df0, 32c9a810, 32c9aa60; CH3c0 identifies it'),
 ('31f64770', 'CX2DamageEffect::GetInstance( CEffectHandle )', 'confirmed', 'June handle lookup on g_pX2Game->GetDamageEffect() (+0x3b4); March X2DamageEffect.h:1785'),
 ('32cb6dd0', 'none in March: copy of the DamageData member at this+0x763c into the argument', 'confirmed', 'called by vslot 0x318 32cb3200; 31e9cb70 = DamageData copy ctor; sits between Rena and Arme'),
]


def main():
    s = open(FM, 'rb').read()
    if TAG.encode() in s:
        print('already has %s rows' % TAG)
        return
    add = ''.join('%s\t%s\t%s\t%s: %s\n' % (a, n, c, TAG, e) for a, n, c, e in R)
    if '--dry' in sys.argv:
        print(add)
        return
    if not s.endswith(b'\n'):
        s += b'\n'
    open(FM, 'wb').write(s + add.encode('ascii'))
    print('funcmap.tsv: %d rows %s' % (len(R), TAG))


if __name__ == '__main__':
    main()
