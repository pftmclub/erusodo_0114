#include "stdafx.h"
#include "X2OfflineDungeonRank.h"

#ifdef SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK

#include "X2OfflineLog.h"

CX2OfflineDungeonRank* CX2OfflineDungeonRank::ms_pInstance = NULL;

namespace
{
	const wchar_t* const SCRIPT_NAME = L"ResultData_new.lua";

	/// The functions Result_DUNGEON calls ( ResultProcess.cpp:1954 / :1969 / :1984 / :1999 / :2014 ) and the one they share.
	const char* const FUNC_NAMES[] =
	{
		"WEIGHT_BY_DUNGEONID",
		"DUNGEON_COMBO_RESULT",
		"DUNGEON_TECHNICAL_RESULT",
		"DUNGEON_DAMAGE_RESULT",
		"DUNGEON_TIME_RESULT",
		"DUNGEON_TOTAL_RANK_RESULT",
	};

	const wchar_t* RankName( int iRank )
	{
		static const wchar_t* const NAMES[] = { L"NONE", L"F", L"E", L"D", L"C", L"B", L"A", L"S", L"SS" };
		return ( iRank >= 0 && iRank <= (int)CX2DungeonRoom::RT_SS ) ? NAMES[ iRank ] : L"?";
	}
}

//////////////////////////////////////////////////////////////////////////

CX2OfflineRankUnit::CX2OfflineRankUnit( const KDungeonPlayResultInfo& kPlay, int iBaseHP )
: m_kPlay( kPlay )
, m_iBaseHP( iBaseHP )
, m_fPercent( 0.0f )
, m_nRank( (int)CX2DungeonRoom::RT_F )
, m_bSet( false )
{
}

CX2OfflineRankMonsters::CX2OfflineRankMonsters( const int aiDie[ DT_END ] )
{
	for( int i = 0; i < DT_END; ++i )
		m_aiDie[ i ] = aiDie[ i ];
}

/*static*/ CX2OfflineRankMonsters::DIE_TYPE CX2OfflineRankMonsters::DieTypeOfGrade( char cMonsterGrade )
{
	if( (char)CX2GUNPC::MG_MIDDLE_BOSS_NPC == cMonsterGrade )
		return DT_MIDDLE_BOSS;
	if( (char)CX2GUNPC::MG_BOSS_NPC == cMonsterGrade )
		return DT_BOSS;
	return DT_NORMAL;
}

//////////////////////////////////////////////////////////////////////////

CX2OfflineDungeonRank::CX2OfflineDungeonRank()
: m_bLoadAttempted( false )
, m_bLoaded( false )
, m_iAddCalls( 0 )
{
}

CX2OfflineDungeonRank::~CX2OfflineDungeonRank()
{
}

CX2OfflineDungeonRank* CX2OfflineDungeonRank::Instance()
{
	if( NULL == ms_pInstance )
		ms_pInstance = new CX2OfflineDungeonRank;

	return ms_pInstance;
}

void CX2OfflineDungeonRank::Release()
{
	if( NULL == ms_pInstance )
		return;

	delete ms_pInstance;
	ms_pInstance = NULL;
}

void CX2OfflineDungeonRank::AddDungeonPartyBonus_LUA( float fTwo, float fThree, float fFour )
{
	++m_iAddCalls;
	CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ResultProcess:AddDungeonPartyBonus( %.2f, %.2f, %.2f ) - party EXP only, unused by a party of one",
		fTwo, fThree, fFour );
}

void CX2OfflineDungeonRank::AddDungeonPartyDifficultyBonus_LUA( float fNormal, float fHard, float fExpert )
{
	++m_iAddCalls;
	CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ResultProcess:AddDungeonPartyDifficultyBonus( %.2f, %.2f, %.2f ) - party EXP only, unused by a party of one",
		fNormal, fHard, fExpert );
}

void CX2OfflineDungeonRank::AddDungeonModePartyBonus_LUA( float fNormal, float fHenir, float fSecret )
{
	++m_iAddCalls;
	CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ResultProcess:AddDungeonModePartyBonus( %.2f, %.2f, %.2f ) - party EXP only, unused by a party of one",
		fNormal, fHenir, fSecret );
}

//////////////////////////////////////////////////////////////////////////
// Loading

bool CX2OfflineDungeonRank::HasFunctions( lua_State* pLuaState ) const
{
	for( int i = 0; i < (int)ARRAY_SIZE( FUNC_NAMES ); ++i )
	{
		lua_getglobal( pLuaState, FUNC_NAMES[ i ] );
		const bool bFunc = ( 0 != lua_isfunction( pLuaState, -1 ) );
		lua_pop( pLuaState, 1 );

		if( false == bFunc )
			return false;
	}

	return true;
}

bool CX2OfflineDungeonRank::EnsureLoaded()
{
	if( true == m_bLoadAttempted )
		return m_bLoaded;

	// One attempt per process, as the stat table: a second try would not find the file any more readily.
	m_bLoadAttempted = true;

	if( NULL == g_pKTDXApp ||
		NULL == g_pKTDXApp->GetLuaBinder() ||
		NULL == g_pKTDXApp->GetDeviceManager() ||
		NULL == g_pKTDXApp->GetDeviceManager()->GetMassFileManager() )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ERROR engine not ready - dungeon ranks stay F." );
		return false;
	}

	lua_State* pLuaState = g_pKTDXApp->GetLuaBinder()->GetLuaState();
	if( NULL == pLuaState )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ERROR no lua state - dungeon ranks stay F." );
		return false;
	}

	// KResultProcess::RegToLua (ResultProcess.cpp:54-101): the global ResultProcess the chunk calls at its top, and the
	// two argument classes with the server's Lua method names. Class names are this port's own; the chunk never names them.
	lua_tinker::class_add< CX2OfflineDungeonRank >( pLuaState, "CX2OfflineDungeonRank" );
	lua_tinker::class_def< CX2OfflineDungeonRank >( pLuaState, "AddDungeonPartyBonus",				&CX2OfflineDungeonRank::AddDungeonPartyBonus_LUA );
	lua_tinker::class_def< CX2OfflineDungeonRank >( pLuaState, "AddDungeonPartyDifficultyBonus",	&CX2OfflineDungeonRank::AddDungeonPartyDifficultyBonus_LUA );
	lua_tinker::class_def< CX2OfflineDungeonRank >( pLuaState, "AddDungeonModePartyBonus",			&CX2OfflineDungeonRank::AddDungeonModePartyBonus_LUA );
	lua_tinker::decl( pLuaState, "ResultProcess", this );

	lua_tinker::class_add< CX2OfflineRankUnit >( pLuaState, "CX2OfflineRankUnit" );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetComboScore",			&CX2OfflineRankUnit::GetComboScore_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetTechnicalScore",		&CX2OfflineRankUnit::GetTechnicalScore_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetBaseHP",				&CX2OfflineRankUnit::GetBaseHP_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetTotalGivenDamamge",		&CX2OfflineRankUnit::GetTotalGivenDamamge_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetTotalAttackedDamage",	&CX2OfflineRankUnit::GetTotalAttackedDamage_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetTotalRecoveryHP",		&CX2OfflineRankUnit::GetTotalRecoveryHP_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "GetUsingKindOfSkill",		&CX2OfflineRankUnit::GetUsingKindOfSkill_LUA );
	lua_tinker::class_def< CX2OfflineRankUnit >( pLuaState, "SetResultData",			&CX2OfflineRankUnit::SetResultData_LUA );

	lua_tinker::class_add< CX2OfflineRankMonsters >( pLuaState, "CX2OfflineRankMonsters" );
	lua_tinker::class_def< CX2OfflineRankMonsters >( pLuaState, "GetNormalNpcDieCount",		&CX2OfflineRankMonsters::GetNormalNpcDieCount_LUA );
	lua_tinker::class_def< CX2OfflineRankMonsters >( pLuaState, "GetLowEliteNpcDieCount",	&CX2OfflineRankMonsters::GetLowEliteNpcDieCount_LUA );
	lua_tinker::class_def< CX2OfflineRankMonsters >( pLuaState, "GetHighEliteNpcDieCount",	&CX2OfflineRankMonsters::GetHighEliteNpcDieCount_LUA );
	lua_tinker::class_def< CX2OfflineRankMonsters >( pLuaState, "GetMiddleBossDieCount",	&CX2OfflineRankMonsters::GetMiddleBossDieCount_LUA );
	lua_tinker::class_def< CX2OfflineRankMonsters >( pLuaState, "GetBossDieCount",			&CX2OfflineRankMonsters::GetBossDieCount_LUA );

	// Archive first, as X2OfflineStatTable: test the payload, never the struct ( see the note there ).
	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER kInfo;
#ifdef SERV_IRUHADEV_OFFLINE_PROBE_ENCRYPT_FIX
	kInfo = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( SCRIPT_NAME, true, true );
#else
	kInfo = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( SCRIPT_NAME );
#endif SERV_IRUHADEV_OFFLINE_PROBE_ENCRYPT_FIX

	if( NULL == kInfo->pRealData || kInfo->size <= 0 )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ERROR '%s' not found in any .kom or on disk - every dungeon rank stays F"
			L" ( no rank EXP bonus, rank sub-quests do not tick ).", SCRIPT_NAME );
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK XOR-encrypt KncWX2Server/ServerResource/US/ResultData_new.lua and pack it into data036.kom." );
		return false;
	}

	// Encrypted first ( the shipped form ), then plaintext; a chunk that fails to load has run nothing.
	bool bRan = g_pKTDXApp->LoadAndDoMemory( g_pKTDXApp->GetLuaBinder(), SCRIPT_NAME );
	if( false == bRan || false == HasFunctions( pLuaState ) )
	{
		m_iAddCalls = 0;
		bRan = g_pKTDXApp->LoadAndDoMemory( g_pKTDXApp->GetLuaBinder(), SCRIPT_NAME, false );

		if( true == bRan && true == HasFunctions( pLuaState ) )
		{
			CX2OfflineLog::Server( L"[JUNE_QUEST] RANK NOTE '%s' is NOT encrypted - loaded as plaintext. Fine for testing;"
				L" encrypt it to match every other packed script.", SCRIPT_NAME );
		}
	}

	if( false == bRan )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ERROR '%s' failed to run, encrypted or plaintext - every dungeon rank stays F.",
			SCRIPT_NAME );
		return false;
	}

	if( false == HasFunctions( pLuaState ) )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ERROR '%s' ran but does not define DUNGEON_*_RESULT / WEIGHT_BY_DUNGEONID"
			L" - every dungeon rank stays F.", SCRIPT_NAME );
		return false;
	}

	m_bLoaded = true;
	CX2OfflineLog::Server( L"[JUNE_QUEST] RANK '%s' loaded: %d function(s), %d ResultProcess call(s)",
		SCRIPT_NAME, (int)ARRAY_SIZE( FUNC_NAMES ), m_iAddCalls );
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Ranking

int CX2OfflineDungeonRank::CallAxis( lua_State* pLuaState, AXIS eAxis, CX2OfflineRankUnit& kUnit,
									 CX2OfflineRankMonsters& kMonsters, int iDungeonIDAndDif, int iPlaySec,
									 int iEndNumMember, int iPartyTotalGivenDamage, OUT float& fPercent )
{
	// The client's lua_tinker::call pushes nil on its error paths ( LuaCommon/lua_tinker.h:1177-1190 ), so the stack is
	// balanced already; the pad and settop are a belt only.
	const int iTop = lua_gettop( pLuaState );
	lua_pushnil( pLuaState );

	kUnit.m_bSet		= false;
	kUnit.m_fPercent	= 0.0f;
	kUnit.m_nRank		= (int)CX2DungeonRoom::RT_F;

	const char* pszFunc = "";
	switch( eAxis )
	{
	case AXIS_COMBO:		// ResultProcess.cpp:1954
		pszFunc = "DUNGEON_COMBO_RESULT";
		lua_tinker::call< double >( pLuaState, pszFunc, &kUnit, iDungeonIDAndDif, &kMonsters, iEndNumMember, iPartyTotalGivenDamage );
		break;

	case AXIS_TECHNICAL:	// :1969
		pszFunc = "DUNGEON_TECHNICAL_RESULT";
		lua_tinker::call< double >( pLuaState, pszFunc, &kUnit, iDungeonIDAndDif, &kMonsters, iEndNumMember, iPartyTotalGivenDamage );
		break;

	case AXIS_DAMAGED:		// :1984
		pszFunc = "DUNGEON_DAMAGE_RESULT";
		lua_tinker::call< double >( pLuaState, pszFunc, &kUnit, iDungeonIDAndDif, &kMonsters );
		break;

	case AXIS_TIME:			// :1999
		pszFunc = "DUNGEON_TIME_RESULT";
		lua_tinker::call< int >( pLuaState, pszFunc, &kUnit, iDungeonIDAndDif, &kMonsters, iPlaySec );
		break;
	}

	lua_settop( pLuaState, iTop );

	if( false == kUnit.m_bSet )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] RANK ERROR %S did not reach SetResultData ( a Lua error; see ErrorLog ) - this axis is F",
			pszFunc );
		fPercent = 0.0f;
		return (int)CX2DungeonRoom::RT_F;
	}

	// GetPercent / GetRank ( :1960-1961 and the three after ).
	fPercent = kUnit.m_fPercent;
	return kUnit.m_nRank;
}

bool CX2OfflineDungeonRank::Compute( int iDungeonIDAndDif, int iPlaySec, const KDungeonPlayResultInfo& kPlay, int iBaseHP,
									 const int aiDie[ CX2OfflineRankMonsters::DT_END ],
									 OUT KDungeonUnitResultInfo& kUnit, OUT float& fRankBonusRate )
{
	fRankBonusRate = 0.0f;

	if( false == EnsureLoaded() )
		return false;

	lua_State* pLuaState = g_pKTDXApp->GetLuaBinder()->GetLuaState();

	CX2OfflineRankUnit		kRankUnit( kPlay, iBaseHP );
	CX2OfflineRankMonsters	kMonsters( aiDie );

	// Result_DUNGEON :1397 iEndNumMember = GetNumMember(), and :1465-1477 iPartyTotalGivenDamage is every member's
	// GetTotalGivenDamamge summed. Offline the party is the player alone.
	const int iEndNumMember				= 1;
	const int iPartyTotalGivenDamage	= kPlay.m_iTotalGivenDamamge;

	float fComboBonus = 0.0f, fTechBonus = 0.0f, fDamageBonus = 0.0f, fTimeBonus = 0.0f;

	kUnit.m_cComboRank		= (char)CallAxis( pLuaState, AXIS_COMBO,     kRankUnit, kMonsters, iDungeonIDAndDif, iPlaySec, iEndNumMember, iPartyTotalGivenDamage, fComboBonus );
	kUnit.m_cTechnicalRank	= (char)CallAxis( pLuaState, AXIS_TECHNICAL, kRankUnit, kMonsters, iDungeonIDAndDif, iPlaySec, iEndNumMember, iPartyTotalGivenDamage, fTechBonus );
	kUnit.m_cDamagedRank	= (char)CallAxis( pLuaState, AXIS_DAMAGED,   kRankUnit, kMonsters, iDungeonIDAndDif, iPlaySec, iEndNumMember, iPartyTotalGivenDamage, fDamageBonus );
	kUnit.m_cTimeRank		= (char)CallAxis( pLuaState, AXIS_TIME,      kRankUnit, kMonsters, iDungeonIDAndDif, iPlaySec, iEndNumMember, iPartyTotalGivenDamage, fTimeBonus );

	// :2014-2023 - the total from the Lua, then RT_NONE or below -> RT_F and above RT_SS -> RT_SS.
	{
		const int iTop = lua_gettop( pLuaState );
		lua_pushnil( pLuaState );
		const int iTotal = lua_tinker::call< int >( pLuaState, "DUNGEON_TOTAL_RANK_RESULT",
			(int)kUnit.m_cComboRank, (int)kUnit.m_cTechnicalRank, (int)kUnit.m_cDamagedRank, (int)kUnit.m_cTimeRank, &kMonsters );
		lua_settop( pLuaState, iTop );

		kUnit.m_cTotalRank = (char)iTotal;
		if( (char)CX2DungeonRoom::RT_NONE >= kUnit.m_cTotalRank )
			kUnit.m_cTotalRank = (char)CX2DungeonRoom::RT_F;
		else if( (char)CX2DungeonRoom::RT_SS < kUnit.m_cTotalRank )
			kUnit.m_cTotalRank = (char)CX2DungeonRoom::RT_SS;
	}

	// :2046-2048
	fRankBonusRate = fComboBonus + fTechBonus + fDamageBonus + fTimeBonus;
	if( fRankBonusRate > 0.15f )
		fRankBonusRate = 0.15f;

	CX2OfflineLog::Server( L"[JUNE_QUEST] RANK in: dungeon %d sec %d combo score %d tech score %d skills %d given %d attacked %d"
		L" recovered %d base hp %d kills normal %d low elite %d high elite %d middle boss %d boss %d",
		iDungeonIDAndDif, iPlaySec, kPlay.m_ComboScore, kPlay.m_TechScore, kPlay.m_iUsingKindOfSkill,
		kPlay.m_iTotalGivenDamamge, kPlay.m_iTotalAttackedDamage, kPlay.m_iTotalRecoveryHP, iBaseHP,
		aiDie[ CX2OfflineRankMonsters::DT_NORMAL ], aiDie[ CX2OfflineRankMonsters::DT_LOW_ELITE ],
		aiDie[ CX2OfflineRankMonsters::DT_HIGH_ELITE ], aiDie[ CX2OfflineRankMonsters::DT_MIDDLE_BOSS ],
		aiDie[ CX2OfflineRankMonsters::DT_BOSS ] );

	CX2OfflineLog::Server( L"[JUNE_QUEST] RANK combo %s tech %s damaged %s time %s total %s ( bonus %.2f + %.2f + %.2f + %.2f -> %.2f )",
		RankName( kUnit.m_cComboRank ), RankName( kUnit.m_cTechnicalRank ), RankName( kUnit.m_cDamagedRank ),
		RankName( kUnit.m_cTimeRank ), RankName( kUnit.m_cTotalRank ),
		fComboBonus, fTechBonus, fDamageBonus, fTimeBonus, fRankBonusRate );

	return true;
}

#endif SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK
