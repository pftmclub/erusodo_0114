"""Write the bytepatch spec that puts Raven's June registrars into X2GURaven.cpp under _LIVE (char-raven CH1m).

    python june2017_re/char_raven/ch1m_spec.py <gen dir> <spec out>   # run from Trunk; <gen dir> = ch1m_gen.py's output

InitState (32c66fe0) head, START / WIN / LOSE and tail are written here; the class sections, InitStateCommon (32c5d660) and the
three line registrars come from ch1m_gen.py. Each function gets `#ifdef _LIVE` June `#else` March `#endif` around its whole body.
"""
import os, sys
F = 'SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE'
gd, out = sys.argv[1], sys.argv[2]


def inc(name):
    return open(os.path.join(gd, name), 'rb').read().decode('ascii').replace('\r\n', '\n').rstrip('\n').split('\n')


C = 'CX2Unit::'
head = [
    '#ifdef ' + F,
    '// June 32c66fe0 ( vslot 0x7a4 )',
    '\tconst CX2Unit::UNIT_CLASS eClass = ( NULL != GetUnit() ) ? GetUnit()->GetUnitData().m_UnitClass : CX2Unit::UC_NONE;',
    '',
    '\tswitch( eClass )',
    '\t{',
    '\tcase %sUC_RAVEN_FIGHTER:' % C,
    '\tcase %sUC_RAVEN_SOUL_TAKER:' % C,
    '\tcase %sUC_RAVEN_OVER_TAKER:' % C,
    '\tcase %sUC_RAVEN_WEAPON_TAKER:' % C,
    '\t\tm_iSkillCutInSetIndex = 0;',
    '\t\tbreak;',
    '\tcase %sUC_RAVEN_BLADE_MASTER:' % C,
    '\t\tm_iSkillCutInSetIndex = 1;',
    '\t\tbreak;',
    '\tcase %sUC_RAVEN_RECKLESS_FIST:' % C,
    '\t\tm_iSkillCutInSetIndex = 2;',
    '\t\tbreak;',
    '\tcase %sUC_RAVEN_VETERAN_COMMANDER:' % C,
    '\t\tm_iSkillCutInSetIndex = 3;',
    '\t\tbreak;',
    '\tdefault:',
    '\t\tbreak;',
    '\t}',
    '',
    '\tswitch( eClass )',
    '\t{',
    '\tcase %sUC_RAVEN_SOUL_TAKER:' % C,
    '\t\tm_iSkillCutInSetSubIndex = 1;',
    '\t\tbreak;',
    '\tcase %sUC_RAVEN_OVER_TAKER:' % C,
    '\t\tm_iSkillCutInSetSubIndex = 2;',
    '\t\tbreak;',
    '\tdefault:',
    '\t\tm_iSkillCutInSetSubIndex = 0;',
    '\t\tbreak;',
    '\t}',
    '',
    '\tInitStateID();				/// June @32c6709f-32c670a5 inlines it',
    '\tInitStateCommon();			/// June 32c5d660',
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
    '\tstateData.Init();',
    '\tstateData.stateID\t\t\t= USI_LOSE;',
    '\tm_LuaManager.MakeTableReference( strLoseTableNameUTF8.c_str(), stateData.stateID );',
    '\tstateData.StateStart\t\t= SET_CB_FUNC( CX2GUUser, LoseStateStart );',
    '\tif( 0 == strLoseTableNameUTF8.compare( "RSI_RWT_LOSE2" ) )		/// June @32c673b1',
    '\t\tstateData.OnFrameMove\t= SET_CB_FUNC( CX2GURaven, RSI_RWT_LOSE2_FrameMove );',
    '\tm_StateList[stateData.stateID] = stateData;',
    '',
    '\t/// June @32c67494 ( id 0x82, checklist 7h )',
    '\tInitWaitStateChange( "RSI_WAIT_STATE_CHANGE" );',
    '',
    '\t/// June 32c66fe0 @315: the line registrars',
    '\tswitch( eClass )',
    '\t{',
    '\tcase %sUC_RAVEN_SOUL_TAKER:' % C,
    '\tcase %sUC_RAVEN_BLADE_MASTER:' % C,
    '\t\tInitStateSwordTaker();		/// June 32c511c0',
    '\t\tbreak;',
    '\tcase %sUC_RAVEN_OVER_TAKER:' % C,
    '\tcase %sUC_RAVEN_RECKLESS_FIST:' % C,
    '\t\tInitStateOverTaker();		/// June 32c51b70',
    '\t\tbreak;',
    '\tcase %sUC_RAVEN_WEAPON_TAKER:' % C,
    '\tcase %sUC_RAVEN_VETERAN_COMMANDER:' % C,
    '\t\tInitStateWeaponTaker();		/// June 32c63670',
    '\t\tbreak;',
    '\tdefault:',
    '\t\tbreak;',
    '\t}',
    '',
]
cls = inc('InitStateClass.inc')
while cls and cls[-1] == '':
    cls.pop()
tail = [
    '',
    '\tInitPassiveSkillState();					/// June vslot 0x690',
    '\tInitEquippedActiveSkillState( false );		/// June vslot 0x684',
    '\tCX2GUUser::InitState();						/// June 31fd8f00 @32c6839f',
    '#else ' + F,
]
state_body = head + cls + tail
common_body = ['#ifdef ' + F, '// June 32c5d660', '\tconst std::string strHyperModeTable = GetHyperModeTableName( false );	/// June 31fdd490, called at the USI_HYPER_MODE registration',
               '\tUserUnitStateData stateData;', ''] + inc('InitStateCommon.inc') + ['#else ' + F]


def line_body(fn):
    return ['#ifdef ' + F, '// June ' + fn, '\tUserUnitStateData stateData;', ''] + inc('InitStateLine_%s.inc' % fn) + ['#else ' + F]


END = ['#endif ' + F]
spec = []


def ins(op, anchor, text):
    spec.extend(['@@ ' + op, '@@ anchor'] + anchor + ['@@ text'] + text + ['@@ end'])


spec.append('@@ file X2Lib/X2GURaven.cpp')
ins('insert_after', ['void CX2GURaven::InitState()', '{'], state_body)
ins('insert_before', ['}', '', 'void CX2GURaven::InitStateID()'], END)
ins('insert_after', ['void CX2GURaven::InitStateCommon()', '{'], common_body)
ins('insert_before', ['}', '', 'void CX2GURaven::InitStateSwordTaker()'], END)
ins('insert_after', ['void CX2GURaven::InitStateSwordTaker()', '{'], line_body('32c511c0'))
ins('insert_before', ['}', '', 'void CX2GURaven::InitStateOverTaker()'], END)
ins('insert_after', ['void CX2GURaven::InitStateOverTaker()', '{'], line_body('32c51b70'))
ins('insert_before', ['}', '', '', '#ifdef RAVEN_WEAPON_TAKER', 'void CX2GURaven::InitStateWeaponTaker()'], END)
ins('insert_after', ['void CX2GURaven::InitStateWeaponTaker()', '{'], line_body('32c63670'))
ins('insert_before', ['}', '#endif RAVEN_WEAPON_TAKER', '', '/*virtual*/ void CX2GURaven::InitPassiveSkillState()'], END)
open(out, 'w', encoding='ascii', newline='\n').write('\n'.join(spec) + '\n')
print('spec lines', len(spec), 'InitState', len(state_body), 'InitStateCommon', len(common_body))
