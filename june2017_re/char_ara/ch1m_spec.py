"""Write the bytepatch spec that puts Ara's June registrars into X2GUAra.cpp under _LIVE (char-ara CH1m).

    python june2017_re/char_ara/ch1m_spec.py <gen dir> <spec out>   # run from Trunk; <gen dir> = ch1m_gen.py's output

Rena's ch1m_spec.py with Ara's names. The InitState wrapper (32bcf4a0: cut-in index, START / WIN / LOSE, wait-state-change, tail) is
written here; InitStateCommon (32bca580) and InitStateByUnitClass (32bcced0) come from ch1m_gen.py. Each function gets `#ifdef _LIVE`
June `#else` March `#endif` around its whole body. Also: CX2GUUser::GetBerserkModeTableName (31fdd800, _CORE, X2GUUser.h / .cpp) and the
registration guards of the _LIVE block (Always.h).
"""
import os, sys
F = 'SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE'
CORE = 'SERV_IRUHADEV_JUNE_CHAR_ARA_CORE'
gd, out = sys.argv[1], sys.argv[2]


def inc(name):
    return open(os.path.join(gd, name), 'rb').read().decode('ascii').replace('\r\n', '\n').rstrip('\n').split('\n')


C = 'CX2Unit::'
CLASS = '\tconst CX2Unit::UNIT_CLASS eClass = ( NULL != GetUnit() ) ? GetUnit()->GetUnitData().m_UnitClass : CX2Unit::UC_NONE;'
init_state_head = [
    '#ifdef ' + F,
    '\t// char-ara CH1m: June 32bcf4a0 ( vslot 0x7a4 )',
    CLASS,
    '',
    '\tswitch( eClass )	// June byte table 32bcfa1c: Little Specter and every other class keep the index',
    '\t{',
    '\tcase %sUC_NONE:' % C,
    '\tcase %sUC_ARA_MARTIAL_ARTIST:' % C,
    '\tcase %sUC_ARA_LITTLE_HSIEN:' % C,
    '\tcase %sUC_ARA_LITTLE_DEVIL:' % C,
    '\t\tm_iSkillCutInSetIndex = 0;',
    '\t\tbreak;',
    '\tcase %sUC_ARA_SAKRA_DEVANAM:' % C,
    '\t\tm_iSkillCutInSetIndex = 1;',
    '\t\tbreak;',
    '\tcase %sUC_ARA_YAMA_RAJA:' % C,
    '\t\tm_iSkillCutInSetIndex = 2;',
    '\t\tbreak;',
    '\tcase %sUC_ARA_ASURA:' % C,
    '\t\tm_iSkillCutInSetIndex = 3;',
    '\t\tbreak;',
    '\tdefault:',
    '\t\tbreak;',
    '\t}',
    '',
    '\tInitStateID();				// June inlines it ( +0x4088 = 0xd = USI_WAIT )',
    '\tInitStateCommon();			// June 32bca580',
    '\tInitStateByUnitClass();		// June 32bcced0 ( June runs InitEventStates first; their ids do not meet )',
    '',
    '\tstd::string strStartTableNameUTF8;',
    '\tstd::string strWinTableNameUTF8;',
    '\tstd::string strLoseTableNameUTF8;',
    '\tInitStateCommonRandom( strStartTableNameUTF8, strWinTableNameUTF8, strLoseTableNameUTF8 );	// June 3202b110 then 31fe22d0',
    '',
    '\tUserUnitStateData stateData;',
    '',
    '\tstateData.Init();',
    '\tstateData.stateID			= USI_START;',
    '\tm_LuaManager.MakeTableReference( strStartTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartStateStart );',
    '\tstateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartEventProcess );',
    '\tstateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\tstateData.Init();',
    '\tstateData.stateID			= USI_WIN;',
    '\tm_LuaManager.MakeTableReference( strWinTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart		= SET_CB_FUNC( CX2GUUser, WinStateStart );',
    '\tstateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RandomLoseEventProcess );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\tstateData.Init();',
    '\tstateData.stateID			= USI_LOSE;',
    '\tm_LuaManager.MakeTableReference( strLoseTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart		= SET_CB_FUNC( CX2GUUser, LoseStateStart );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\tInitWaitStateChange( "ASI_WAIT_STATE_CHANGE" );	// June @32bcf90d ( id 0x82, checklist 7h )',
    '',
    '\tInitPassiveSkillState();					// June vslot 0x690',
    '\tInitEquippedActiveSkillState( false );		// June vslot 0x684',
    '\tCX2GUUser::InitState();						// June 31fd8f00',
    '#else ' + F,
]
common_head = [
    '#ifdef ' + F,
    '\t// char-ara CH1m: June 32bca580 ( the InitState body, no class test )',
    '\tUserUnitStateData stateData;',
    '\tconst std::string strHyperModeTable = GetHyperModeTableName( false );	// June 31fdd490( ..., 0 )',
    '\tconst std::string strBerserkModeTable = GetBerserkModeTableName();	// June 31fdd800( ..., 0 )',
    '',
] + inc('InitStateCommon.inc') + ['#else ' + F]
by_class_head = [
    '#ifdef ' + F,
    '\t// char-ara CH1m: June 32bcced0 ( the per-class part )',
    '\tUserUnitStateData stateData;',
    CLASS,
    '',
] + inc('InitStateByUnitClass.inc') + ['#else ' + F]
END = ['#endif ' + F]

berserk_decl = [
    '#ifdef ' + CORE,
    '\t\tstd::string GetBerserkModeTableName() const;	// char-ara CH1m: June 31fdd800 ( RET 0x8: its second argument is never read )',
    '#endif ' + CORE,
]
berserk_def = [
    '#ifdef ' + CORE,
    '// char-ara CH1m: June 31fdd800, Ara\'s berserk table by unit type, then by class for a transcended unit',
    'std::string CX2GUUser::GetBerserkModeTableName() const',
    '{',
    '\tstd::string strName;',
    '\tCX2Unit* pUnit = GetUnit();',
    '\tif( NULL != pUnit && NULL != pUnit->GetUnitTemplet() && CX2Unit::UT_ARA == pUnit->GetUnitTemplet()->m_UnitType )',
    '\t\tstrName = "ASI_BERSERK_MODE";',
    '',
    '\tif( NULL != pUnit && true == IsUnitTranscended( pUnit ) )',
    '\t{',
    '\t\tswitch( pUnit->GetUnitData().m_UnitClass )',
    '\t\t{',
    '\t\tcase CX2Unit::UC_ARA_SAKRA_DEVANAM:	strName = "ASI_ASD_BERSERK_MODE";	break;',
    '\t\tcase CX2Unit::UC_ARA_YAMA_RAJA:		strName = "ASI_AYR_BERSERK_MODE";	break;',
    '\t\tcase CX2Unit::UC_ARA_ASURA:			strName = "ASI_AA_BERSERK_MODE";	break;',
    '\t\tdefault:	break;',
    '\t\t}',
    '\t}',
    '\treturn strName;',
    '}',
    '',
    '#endif ' + CORE,
]
guards = [
    '',
    '//////////////////////////////////////////////////////////////////////////',
    '// Author: Iruha',
    '// Date: 2026-11-11',
    '// Description: June character port ( Ara ) Phase CH1m - guards only ( no new flag ): the registration calls the shared registrars ( GetHyperModeTableName, StartStateStart, InitEventStates, RandomLoseEventProcess, InitWaitStateChange, InitBoundingStates ) and GetBerserkModeTableName reads IsUnitTranscended',
    '#if defined( SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE ) && ( !defined( SERV_IRUHADEV_JUNE_CHAR_ARME_CORE ) || !defined( SERV_IRUHADEV_JUNE_CHAR_EVE_CORE ) || !defined( HYPER_MODE_FIX ) || !defined( SERV_IRUHADEV_JUNE_COMMON_RANDOM_STATE ) || !defined( SERV_IRUHADEV_JUNE_EVENT_STATES ) || !defined( SERV_IRUHADEV_JUNE_WAIT_STATE_CHANGE ) || !defined( SERV_IRUHADEV_JUNE_BOUNDING_STATE ) )',
    '#error SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE needs SERV_IRUHADEV_JUNE_CHAR_ARME_CORE, SERV_IRUHADEV_JUNE_CHAR_EVE_CORE, HYPER_MODE_FIX, SERV_IRUHADEV_JUNE_COMMON_RANDOM_STATE, SERV_IRUHADEV_JUNE_EVENT_STATES, SERV_IRUHADEV_JUNE_WAIT_STATE_CHANGE and SERV_IRUHADEV_JUNE_BOUNDING_STATE',
    '#endif SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE',
    '#if defined( SERV_IRUHADEV_JUNE_CHAR_ARA_CORE ) && !defined( SERV_IRUHADEV_JUNE_CHAR_ARME_CORE )',
    '#error SERV_IRUHADEV_JUNE_CHAR_ARA_CORE needs SERV_IRUHADEV_JUNE_CHAR_ARME_CORE ( IsUnitTranscended, GetBerserkModeTableName sits in its block )',
    '#endif SERV_IRUHADEV_JUNE_CHAR_ARA_CORE',
    '//////////////////////////////////////////////////////////////////////////',
]


def edit(op, anchor, text):
    return ['@@ ' + op, '@@ anchor'] + anchor + ['@@ text'] + text + ['@@ end']


S = ['@@ file X2Lib/X2GUAra.cpp']
S += edit('insert_after', ['/*virtual*/ void CX2GUAra::InitState()', '{'], init_state_head)
S += edit('insert_after', ['\tInitPassiveSkillState();', '\tInitEquippedActiveSkillState();', '', '\tCX2GUUser::InitState();'], END)
S += edit('insert_after', ['void CX2GUAra::InitStateCommon()', '{'], common_head)
S += edit('insert_before', ['}', '#pragma endregion', '', '#pragma region InitStateByUnitClass'], END)
S += edit('insert_after', ['void CX2GUAra::InitStateByUnitClass()', '{'], by_class_head)
S += edit('insert_before', ['}', '#pragma endregion', '', '#pragma region InitPassiveSkillState'], END)
S += ['@@ file X2Lib/X2GUUser.h']
S += edit('insert_after', ['\t\tvoid StartStateStart();\t\t\t/// June 31ffbd70'], berserk_decl)
S += ['@@ file X2Lib/X2GUUser.cpp']
S += edit('insert_before', ['// June 31fc7f50', 'void CX2GUUser::DashJumpStartFuture()'], berserk_def)
S += ['@@ file KTDXLIB/Always.h']
S += edit('insert_after', ['#error SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE needs SERV_IRUHADEV_JUNE_USER_SHOWN_SKILL_COOL_TIME and SERV_IRUHADEV_JUNE_USER_NUMERICAL_BY_NAME',
                           '#endif SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE', '//////////////////////////////////////////////////////////////////////////'], guards)
open(out, 'w', encoding='ascii', newline='\n').write('\n'.join(S) + '\n')
print('spec lines', len(S))
