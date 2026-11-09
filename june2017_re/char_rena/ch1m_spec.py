"""Write the bytepatch spec that puts Rena's June registrars into X2GULire_ElvenRanger.cpp under _LIVE (char-rena CH1m).

    python june2017_re/char_rena/ch1m_spec.py <gen dir> <spec out>   # run from Trunk; <gen dir> = ch1m_gen.py's output

Raven's ch1m_spec.py with Rena's names. InitState (32cadc90) head, START / WIN / LOSE and tail are written here; the class part and
InitStateCommon (32ca9dc0) come from ch1m_gen.py. Each function gets `#ifdef _LIVE` June `#else` March `#endif` around its whole body.
Also: the id-278 enum member (X2GULire_ElvenRanger.h, _CORE) and the six registration guards of the _LIVE block (Always.h).
"""
import os, sys
F = 'SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE'
NF = 'SERV_IRUHADEV_JUNE_CHAR_RENA_NF'
gd, out = sys.argv[1], sys.argv[2]


def inc(name):
    return open(os.path.join(gd, name), 'rb').read().decode('ascii').replace('\r\n', '\n').rstrip('\n').split('\n')


C = 'CX2Unit::'
head = [
    '#ifdef ' + F,
    '// June 32cadc90 ( vslot 0x7a4 )',
    '\tconst CX2Unit::UNIT_CLASS eClass = ( NULL != GetUnit() ) ? GetUnit()->GetUnitData().m_UnitClass : CX2Unit::UC_NONE;',
    '',
    '\tswitch( eClass )',
    '\t{',
    '\tcase %sUC_LIRE_ELVEN_RANGER:' % C,
    '\tcase %sUC_LIRE_COMBAT_RANGER:' % C,
    '\tcase %sUC_LIRE_SNIPING_RANGER:' % C,
    '\tcase %sUC_LIRE_TRAPPING_RANGER:' % C,
    '\t\tm_iSkillCutInSetIndex = 0;',
    '\t\tbreak;',
    '\tcase %sUC_LIRE_WIND_SNEAKER:' % C,
    '\t\tm_iSkillCutInSetIndex = 1;',
    '\t\tbreak;',
    '\tcase %sUC_LIRE_GRAND_ARCHER:' % C,
    '\t\tm_iSkillCutInSetIndex = 2;',
    '\t\tbreak;',
    '\tcase %sUC_LIRE_NIGHT_WATCHER:' % C,
    '\t\tm_iSkillCutInSetIndex = 3;',
    '\t\tbreak;',
    '\tdefault:',
    '\t\tbreak;',
    '\t}',
    '',
    '\tswitch( eClass )',
    '\t{',
    '\tcase %sUC_LIRE_COMBAT_RANGER:' % C,
    '\t\tm_iSkillCutInSetSubIndex = 1;',
    '\t\tbreak;',
    '\tcase %sUC_LIRE_SNIPING_RANGER:' % C,
    '\t\tm_iSkillCutInSetSubIndex = 2;',
    '\t\tbreak;',
    '\tdefault:',
    '\t\tm_iSkillCutInSetSubIndex = 0;',
    '\t\tbreak;',
    '\t}',
    '',
    '\tInitStateID();				/// June inlines it ( +0x4088 = 0xd, +0x7d04 = 0xa0 )',
    '\tInitStateCommon();			/// June 32ca9dc0',
    '',
    '\tstd::string strStartTableNameUTF8;',
    '\tstd::string strWinTableNameUTF8;',
    '\tstd::string strLoseTableNameUTF8;',
    '\tInitStateCommonRandom( strStartTableNameUTF8, strWinTableNameUTF8, strLoseTableNameUTF8 );	/// June 3202b110 then 31fe22d0',
    '',
    '\tUserUnitStateData stateData;',
    '',
    '\tstateData.Init();',
    '\tstateData.stateID\t\t\t= USI_START;',
    '\tm_LuaManager.MakeTableReference( strStartTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart\t\t= SET_CB_FUNC( CX2GUUser, StartStateStart );',
    '\tstateData.OnEventProcess\t= SET_CB_FUNC( CX2GUUser, StartEventProcess );',
    '\tstateData.StateEnd\t\t\t= SET_CB_FUNC( CX2GUUser, StartEnd );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\tstateData.Init();',
    '\tstateData.stateID\t\t\t= USI_WIN;',
    '\tm_LuaManager.MakeTableReference( strWinTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart\t\t= SET_CB_FUNC( CX2GUUser, WinStateStart );',
    '\tstateData.OnEventProcess\t= SET_CB_FUNC( CX2GUUser, RandomLoseEventProcess );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\tif( 0 == strWinTableNameUTF8.compare( "LESI_RNW_WIN2" ) )		/// June @32cadfac, byte +0x9714',
    '\t\tm_bShowTrapBladeToWinMotion = true;',
    '',
    '\tstateData.Init();',
    '\tstateData.stateID\t\t\t= USI_LOSE;',
    '\tm_LuaManager.MakeTableReference( strLoseTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart\t\t= SET_CB_FUNC( CX2GUUser, LoseStateStart );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\t/// June @32cae131 ( id 0x82, checklist 7h )',
    '\tInitWaitStateChange( "LESI_WAIT_STATE_CHANGE" );',
    '',
]
cls = inc('InitStateClass.inc')
tail = [
    '',
    '\tInitPassiveSkillState();					/// June vslot 0x690',
    '\tInitEquippedActiveSkillState( false );		/// June vslot 0x684',
    '\tCX2GUUser::InitState();						/// June 31fd8f00',
    '\tStateInitTrapBlade();						/// June 32c96d60 @32cb0480, every class',
    '#ifdef ' + NF,
    '\tif( 0 < GetNaturalForceCount() )			/// June 32c85200( 1 ) when the count is above 0',
    '\t\tSetNaturalForceBuff( true );',
    '#endif ' + NF,
    '#else ' + F,
]
state_body = head + cls + tail
common_body = ['#ifdef ' + F, '// June 32ca9dc0', '\tconst CX2Unit::UNIT_CLASS eClass = ( NULL != GetUnit() ) ? GetUnit()->GetUnitData().m_UnitClass : CX2Unit::UC_NONE;',
               '\tconst std::string strHyperModeTable = GetHyperModeTableName( false );	/// June 31fdd490, called at the USI_HYPER_MODE registration',
               '\tUserUnitStateData stateData;', ''] + inc('InitStateCommon.inc') + ['#else ' + F]
END = ['#endif ' + F]
spec = []


def ins(op, anchor, text):
    spec.extend(['@@ ' + op, '@@ anchor'] + anchor + ['@@ text'] + text + ['@@ end'])


spec.append('@@ file X2Lib/X2GULire_ElvenRanger.cpp')
ins('insert_after', ['void CX2GULire_ElvenRanger::InitStateCommon()', '{'], common_body)
ins('insert_before', ['}', '', '', 'void CX2GULire_ElvenRanger::InitState()'], END)
ins('insert_after', ['void CX2GULire_ElvenRanger::InitState()', '{'], state_body)
ins('insert_before', ['}', '', '', '', 'void CX2GULire_ElvenRanger::InitEquippedActiveSkillState(bool bOnlySkillLevel /* = false  */)'], END)
spec.append('@@ file X2Lib/X2GULire_ElvenRanger.h')
ins('insert_after', ['\t\t\tLESI_SA_RNW_ERADICATION_ATTACK,\t/// June script id 302 ( Eradication\'s attack; CH1m maps, CH3c owns the state )'],
    ['\t\t\tLESI_RNW_DASH_JUMP_XZ_LOOP,\t/// June script id 278 ( the XZ chain\'s loop slot; June registers it with the XXZ_LOOP table and enters it nowhere )'])
spec.append('@@ file KTDXLIB/Always.h')
G = [('SERV_IRUHADEV_JUNE_CHAR_EVE_CORE', "June's HyperModeEnd body"), ('HYPER_MODE_FIX', 'HyperModeStartFuture, HyperModeEnd'),
     ('SERV_IRUHADEV_JUNE_COMMON_RANDOM_STATE', 'RandomLoseEventProcess'), ('SERV_IRUHADEV_JUNE_EVENT_STATES', 'June 3202b110'),
     ('SERV_IRUHADEV_JUNE_WAIT_STATE_CHANGE', 'InitWaitStateChange'), ('SERV_IRUHADEV_JUNE_BOUNDING_STATE', 'InitBoundingStates')]
guards = []
for g, why in G:
    guards += ['#ifndef ' + g, '#error %s needs %s ( %s )' % (F, g, why), '#endif ' + g]
ins('insert_after', ['#error SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE needs SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS ( the skill-use helpers )',
                     '#endif SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS'], guards)
open(out, 'w', encoding='ascii', newline='\n').write('\n'.join(spec) + '\n')
print('spec lines', len(spec), 'InitState', len(state_body), 'InitStateCommon', len(common_body))
