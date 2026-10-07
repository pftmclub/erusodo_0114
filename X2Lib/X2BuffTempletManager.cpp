/** @file : X2BuffTempletManager.h
    @breif : 버프 템플릿을 관리하는 매니저 클래스
*/

#pragma once

#include "StdAfx.h"
#include ".\X2BuffTempletManager.h"
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/// June buff port 4b-ii: as in the five buff runtime files. Here it matters for GetBuffFactorPtrFromBuffFactorList
/// ("uiBuffFactorId Invalid"), which item and skill templet loading call outside any capture; the other sites run
/// inside the Phase 3 captures, where RuntimeError hands the text to the capture exactly as DisplayErrorMessage did.
#undef DISPLAY_ERROR
#define DISPLAY_ERROR( wszError_ ) CX2JuneBuffCapture::RuntimeError( wszError_ );
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

CX2BuffTempletManager* CX2BuffTempletManager::pInstance = NULL;

#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
namespace
{
	/// June 326c3f30 / 326c32a0: ids from 10000 up belong to the _INT branch; _INT id N sits at [N-10001]
	const UINT JUNE_BUFF_FACTOR_ID_INT_BASE = 10000;

	/// [JUNE_BUFF] lines also come from loader threads (GetBuffFactorPtr), and KDbgStm is one global stream
	struct KJuneBuffLogLock
	{
		CRITICAL_SECTION m_cs;
		KJuneBuffLogLock() { InitializeCriticalSection( &m_cs ); }
		~KJuneBuffLogLock() { DeleteCriticalSection( &m_cs ); }
	};
	KJuneBuffLogLock	g_kJuneBuffLogLock;
	std::set<UINT>		g_setJuneBuffReportedID;

	/// GetBuffFactorPtr runs during play; report each bad id once, not on every lookup
	bool JuneBuffFirstReport( const UINT uiBuffFactorID_ )
	{
		EnterCriticalSection( &g_kJuneBuffLogLock.m_cs );
		const bool bFirst = g_setJuneBuffReportedID.insert( uiBuffFactorID_ ).second;
		LeaveCriticalSection( &g_kJuneBuffLogLock.m_cs );
		return bFirst;
	}
}

#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
namespace
{
	/// CX2JuneBuffRuntimeScope: the buff whose runtime code runs on this thread (0 = none named)
	__declspec(thread) int					g_iJuneBuffRuntimeBTI = 0;
	/// RuntimeError reports each (BTI, text) once, so a failure that repeats every frame cannot flood ErrorLog.txt
	std::set< std::pair<int, string> >		g_setJuneBuffRuntimeReported;

	bool JuneBuffFirstRuntimeReport( const int iBuffTempletID_, const char* szError_ )
	{
		EnterCriticalSection( &g_kJuneBuffLogLock.m_cs );
		const bool bFirst = g_setJuneBuffRuntimeReported.insert( std::make_pair( iBuffTempletID_, string( szError_ ) ) ).second;
		LeaveCriticalSection( &g_kJuneBuffLogLock.m_cs );
		return bFirst;
	}
}

CX2JuneBuffRuntimeScope::CX2JuneBuffRuntimeScope( const int iBuffTempletID_ )
	: m_iPrevBuffTempletID( g_iJuneBuffRuntimeBTI )
{
	g_iJuneBuffRuntimeBTI = iBuffTempletID_;
}

CX2JuneBuffRuntimeScope::~CX2JuneBuffRuntimeScope()
{
	g_iJuneBuffRuntimeBTI = m_iPrevBuffTempletID;
}

/*static*/ int CX2JuneBuffRuntimeScope::GetBuffTempletID()
{
	return g_iJuneBuffRuntimeBTI;
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
CX2JuneBuffCapture::CX2JuneBuffCapture()
	: m_bOwner( NULL == g_pIruhaDisplayErrorSink )
{
	if ( m_bOwner )
		g_pIruhaDisplayErrorSink = this;
}

/*virtual*/ CX2JuneBuffCapture::~CX2JuneBuffCapture()
{
	if ( m_bOwner )
		g_pIruhaDisplayErrorSink = NULL;
}

/*virtual*/ void CX2JuneBuffCapture::OnDisplayError( const char* szError_ )
{
	m_vecError.push_back( NULL != szError_ ? szError_ : "" );
}

void CX2JuneBuffCapture::Report( const bool bParsed_, const char* szEntry_, const char* szOutcome_ ) const
{
	if ( false == m_bOwner || ( bParsed_ && m_vecError.empty() ) )
		return;

	/// the reason is the first "unsupported <kind> <n>" a switch's default branch raised, else the first text
	string strAll, strReason;
	BOOST_FOREACH( const string& strError, m_vecError )
	{
		if ( false == strAll.empty() )
			strAll += "; ";
		strAll += strError;
		if ( strReason.empty() && 0 == strError.compare( 0, 12, "unsupported " ) )
			strReason = strError.substr( 12 );
	}
	if ( strReason.empty() )
		strReason = m_vecError.empty() ? string( "(no error text)" ) : m_vecError.front();

	if ( bParsed_ )
		Line( "warning in %s: %s", szEntry_, strAll.c_str() );
	else
		Line( "unsupported %s in %s -> %s: %s", strReason.c_str(), szEntry_, szOutcome_, strAll.empty() ? "(no error text)" : strAll.c_str() );
}

/*static*/ void CX2JuneBuffCapture::Line( const char* szFormat_, ... )
{
	char szLine[1024] = { 0, };
	va_list vaList;
	va_start( vaList, szFormat_ );
	_vsnprintf_s( szLine, _countof( szLine ), _TRUNCATE, szFormat_, vaList );
	va_end( vaList );

	EnterCriticalSection( &g_kJuneBuffLogLock.m_cs );
	KLOG( "ErrorLog.txt" ) << "[JUNE_BUFF] " << szLine << fileout;
	LeaveCriticalSection( &g_kJuneBuffLogLock.m_cs );
}

/*static*/ bool CX2JuneBuffCapture::UnsupportedType( const WCHAR* wszKind_, const UINT uiType_ )
{
	WCHAR wszError[64] = { 0, };
	swprintf_s( wszError, _countof( wszError ), L"unsupported %s %u", wszKind_, uiType_ );
	return DisplayErrorMessage( wszError );		/// captured during a parse; a MessageBox otherwise, as before
}
#ifdef SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY

/*static*/ bool CX2JuneBuffCapture::RuntimeError( const WCHAR* wszError_ )
{
	if ( NULL != g_pIruhaDisplayErrorSink )
		return DisplayErrorMessage( wszError_ );	/// inside a capture: collected and reported by its owner, as before

	char szError[512] = { 0, };
	WideCharToMultiByte( CP_ACP, 0, NULL != wszError_ ? wszError_ : L"", -1, szError, sizeof( szError ) - 1, NULL, NULL );
	const int iBuffTempletID = CX2JuneBuffRuntimeScope::GetBuffTempletID();
	if ( JuneBuffFirstRuntimeReport( iBuffTempletID, szError ) )
	{
		StateLog( wszError_ );						/// what DisplayErrorMessage writes, but once: no popup throttles a per-frame error now
		Line( "runtime error, BTI %d: %s (June raises no popup here; written once per buff and text)", iBuffTempletID, szError );
	}
	return false;
}

/*static*/ bool CX2JuneBuffCapture::RuntimeError( const char* szError_ )
{
	if ( NULL != g_pIruhaDisplayErrorSink )
		return DisplayErrorMessage( szError_ );	/// inside a capture: collected and reported by its owner, as before

	const int iBuffTempletID = CX2JuneBuffRuntimeScope::GetBuffTempletID();
	if ( JuneBuffFirstRuntimeReport( iBuffTempletID, NULL != szError_ ? szError_ : "" ) )
	{
		StateLog( szError_ );						/// once, as the wide overload
		Line( "runtime error, BTI %d: %s (June raises no popup here; written once per buff and text)", iBuffTempletID, NULL != szError_ ? szError_ : "" );
	}
	return false;
}
#endif SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER

/** @function : OpenScriptFile
	@brief : 버프템플릿 루아스크립트 파싱
	@param : 루아파일명
*/
void CX2BuffTempletManager::OpenScriptFileForTemplet()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "g_pBuffTempletManager", this );



	const wchar_t* wszScriptFileName = L"BattleFieldBuffTemplet.lua";

    if ( g_pKTDXApp->LoadLuaTinker( wszScriptFileName ) == false )
    {
		ASSERT( !"DoMemory doesn't work!" );
		ErrorLogMsg( XEM_ERROR2, wszScriptFileName );
    }
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	LoadINTScriptFile( L"BattleFieldBuffTemplet_INT.lua", "326c0020" );
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
}


void CX2BuffTempletManager::OpenScriptFileForFactor()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "g_pBuffTempletManager", this );

	const wchar_t* wszScriptFileName = L"BattleFieldBuffFactor.lua";

    if ( g_pKTDXApp->LoadLuaTinker( wszScriptFileName ) == false )
    {
		ASSERT( !"DoMemory doesn't work!" );
		ErrorLogMsg( XEM_ERROR2, wszScriptFileName );
    }
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	LoadINTScriptFile( L"BattleFieldBuffFactor_INT.lua", "326c13a0" );
	LogLoadSummary();
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
}
//{{ Iruha : 2026-10-05 // June buff port Phase 10 (JUNE_BUFF_PORT_PHASES.md)
#ifdef SERV_IRUHADEV_JUNE_BUFF_GROUP

/** @function : OpenScriptFileForBuffGroup
	@brief : June 326c26f0, called by the start-up right after the factor loader (32599e00). It binds
			 g_pBuffTempletManager, runs UnitApplyBuffGroup.lua and logs a failure (June: error 0x48/0x49,
			 then 0x2712). June then registers the group vector with its anti-tamper checksum (31c59710,
			 326c0000); that is not ported, as in Phase 3's loaders. Without the file June has no groups
			 (326c31c0 lets every buff through) and an empty officially-debuff set.
*/
void CX2BuffTempletManager::OpenScriptFileForBuffGroup()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "g_pBuffTempletManager", this );

	const wchar_t* wszScriptFileName = L"UnitApplyBuffGroup.lua";
	const HRESULT hr = g_pKTDXApp->LoadAndDoMemory_ErrorCode( g_pKTDXApp->GetLuaBinder(), wszScriptFileName );
	if ( FAILED( hr ) )
	{
		ErrorLogMsg( XEM_ERROR2, wszScriptFileName );
		if ( HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) == hr )
			CX2JuneBuffCapture::Line( "UnitApplyBuffGroup.lua not found: no buff groups and an empty officially-debuff list this run. Pack march2014/update/data036/UnitApplyBuffGroup.lua into data036.kom (June 326c26f0)" );
		else
			CX2JuneBuffCapture::Line( "UnitApplyBuffGroup.lua did not run (hr 0x%08x): no buff groups and an empty officially-debuff list this run (June 326c26f0)", static_cast<UINT>( hr ) );
	}

	CX2JuneBuffCapture::Line( "UnitApplyBuffGroup.lua: %u buff group(s), %u officially debuff(s) (June 326c26f0)",
		static_cast<UINT>( m_vecBuffGroup.size() ), static_cast<UINT>( m_setOfficiallyDebuff.size() ) );
}

/** @function : AddBuffGroup_LUA
	@brief : June 326c3450. BUFF_GROUP_ID and BUFF_GROUP_APPLY_DEFAULT_VALUE are ints, 0 when absent;
			 BUFF_TEMPLET_LIST holds { BTI, apply } rows read by index until a row is missing (BTI 0 and
			 apply true when a value is absent, as June presets them). map::insert keeps a BTI's first row.
			 The group is pushed even when it repeats an id (no dedupe in June). June never closes
			 BUFF_TEMPLET_LIST, and neither KLuaManager destructor pops it (June 32fe9660 and LuaCommon's
			 DetachLuaState only release refs). The table goes when this call returns to Lua (the void functor
			 returns 0 results), so leaving it open is harmless; transcribed as June has it.
*/
void CX2BuffTempletManager::AddBuffGroup_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	BuffGroup kBuffGroup;
	if ( false == luaManager.GetValue( "BUFF_GROUP_ID", kBuffGroup.m_iBuffGroupID ) )
		kBuffGroup.m_iBuffGroupID = 0;
	if ( false == luaManager.GetValue( "BUFF_GROUP_APPLY_DEFAULT_VALUE", kBuffGroup.m_iApplyDefaultValue ) )
		kBuffGroup.m_iApplyDefaultValue = 0;

	if ( luaManager.BeginTable( "BUFF_TEMPLET_LIST" ) )
	{
		int iIndex = 1;
		while ( luaManager.BeginTable( iIndex ) )
		{
			++iIndex;
			UINT uiBuffTempletID = 0;
			bool bApply = true;
			luaManager.GetValue( 1, uiBuffTempletID );
			luaManager.GetValue( 2, bApply );
			kBuffGroup.m_mapBuffTemplet.insert( std::make_pair( static_cast<BUFF_TEMPLET_ID>( uiBuffTempletID ), bApply ) );
			luaManager.EndTable();
		}
	}

	m_vecBuffGroup.push_back( kBuffGroup );
}

/** @function : AddOfficiallyBuffList_LUA
	@brief : June 326bfd80. Clears the set first (3260ea70), so the last call wins; then every
			 OFFICIALLY_DEBUFF_TEMPLET_LIST value up to the first missing one goes in (0 included).
*/
void CX2BuffTempletManager::AddOfficiallyBuffList_LUA()
{
	m_setOfficiallyDebuff.clear();

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	if ( luaManager.BeginTable( "OFFICIALLY_DEBUFF_TEMPLET_LIST" ) )
	{
		int iIndex = 1;
		UINT uiBuffTempletID = 0;
		while ( luaManager.GetValue( iIndex, uiBuffTempletID ) )
		{
			++iIndex;
			m_setOfficiallyDebuff.insert( static_cast<BUFF_TEMPLET_ID>( uiBuffTempletID ) );
		}
		luaManager.EndTable();
	}
}

/** @function : IsApplyBuffByGroup
	@brief : June 326c31c0, called by CX2GUNPC::CanApplyBuffToGameUnit (327bdb60). The first group with this id
			 decides; no group with it lets the buff through. A BTI the group's list names gets its own
			 apply flag; any other BTI follows the group's default: NONE/ACTIVE yes, ACTIVE_JUST_BUFF only
			 a buff, ACTIVE_JUST_DEBUFF only a debuff (templet +0x64 = IS_DEBUFF), DEACTIVE and the rest no.
			 June reads the templet without a null check (JUNE_BUGS.md); a BTI with no templet is refused here
			 with a log line instead of a crash.
*/
bool CX2BuffTempletManager::IsApplyBuffByGroup( const BUFF_TEMPLET_ID eBuffTempletID_, const int iBuffGroupID_ ) const
{
	vector<BuffGroup>::const_iterator vItr = m_vecBuffGroup.begin();
	for ( ; m_vecBuffGroup.end() != vItr; ++vItr )
	{
		if ( iBuffGroupID_ == vItr->m_iBuffGroupID )
			break;
	}
	if ( m_vecBuffGroup.end() == vItr )
		return true;

	map<BUFF_TEMPLET_ID, bool>::const_iterator mItr = vItr->m_mapBuffTemplet.find( eBuffTempletID_ );
	if ( vItr->m_mapBuffTemplet.end() != mItr )
		return mItr->second;

	CX2BuffTempletPtr ptrBuffTemplet = GetBuffTempletPtr( eBuffTempletID_ );
	if ( NULL == ptrBuffTemplet )
	{
		static std::set<int> s_setReported;
		if ( s_setReported.insert( static_cast<int>( eBuffTempletID_ ) ).second )
			CX2JuneBuffCapture::Line( "buff group %d: BTI %d has no templet, refused (June 326c31c0 would read a null templet)",
				iBuffGroupID_, static_cast<int>( eBuffTempletID_ ) );
		return false;
	}

	const bool bDebuff = ptrBuffTemplet->IsDeBuff();
	switch ( vItr->m_iApplyDefaultValue )
	{
	case BGADV_NONE:
	case BGADV_ACTIVE:
		return true;

	case BGADV_ACTIVE_JUST_BUFF:
		return !bDebuff;

	case BGADV_ACTIVE_JUST_DEBUFF:
		return bDebuff;

	case BGADV_DEACTIVE:
	default:
		return false;
	}
}

/** @function : IsOfficiallyDebuff
	@brief : June 326bfd50 (set find != end). Readers: EraseDebuffAndAddMP (3204d440), Ara's
			 GetReduceDebuffDurationRate (32bd32f0), and 3204e6a0, which has no callers in June.
*/
bool CX2BuffTempletManager::IsOfficiallyDebuff( const BUFF_TEMPLET_ID eBuffTempletID_ ) const
{
	return m_setOfficiallyDebuff.end() != m_setOfficiallyDebuff.find( eBuffTempletID_ );
}
#endif SERV_IRUHADEV_JUNE_BUFF_GROUP
//}}

/** @function : AddBuffTemplet_LUA
	@brief : 버프템플릿 인스턴스 생성, 스크립트 파싱 및 map에 저장
*/
void CX2BuffTempletManager::AddBuffTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	/// June 326c3990: create, ParsingScript, insert keyed by the templet id; the same shape as March.
	/// What changes is the reporting: the entry's parse errors become one [JUNE_BUFF] line.
	CX2JuneBuffCapture kCapture;
	CX2BuffTempletPtr	ptrBuffTemplet = CX2BuffTemplet::CreateBuffTempletPtr();
	if ( NULL == ptrBuffTemplet )
	{
		++m_uiTempletFailed;
		CX2JuneBuffCapture::Line( "new BuffTemplet didn't work" );	/// silent in June
		return;
	}

	const bool bParsed = ptrBuffTemplet->ParsingScript( luaManager );
	char szEntry[64] = { 0, };
	sprintf_s( szEntry, _countof( szEntry ), "BTI %d", static_cast<int>( ptrBuffTemplet->GetBuffTempletID() ) );
	if ( bParsed )
	{
		if ( false == m_mapBuffTempletPtr.insert( make_pair( ptrBuffTemplet->GetBuffTempletID(), ptrBuffTemplet ) ).second )
		{
			++m_uiTempletDuplicate;
			CX2JuneBuffCapture::Line( "duplicate %s: the first definition is kept (map insert)", szEntry );
		}
	}
	else
		++m_uiTempletFailed;

	kCapture.Report( bParsed, szEntry, "templet dropped" );
#else SERV_IRUHADEV_JUNE_BUFF_LOADER
	
	CX2BuffTempletPtr	ptrBuffTemplet = CX2BuffTemplet::CreateBuffTempletPtr();

	if ( NULL != ptrBuffTemplet )
	{
		if ( ptrBuffTemplet->ParsingScript( luaManager ) )
			m_mapBuffTempletPtr.insert( make_pair( ptrBuffTemplet->GetBuffTempletID(), ptrBuffTemplet ) );
	}
	else
	{
		DISPLAY_ERROR( L"new BuffTemplet didn't work" );
	}
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
}

/** @function : AddBuffFactor_LUA
	@brief : 특정 버프가 유지되는 동안 생성되는 버프 팩터를 위해서 만드는 버프 팩터
*/
void CX2BuffTempletManager::AddBuffFactor_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	/// June 326c4050. An unreadable BUFF_FACTOR_ID stays 0 and fails the ordinal check (no early return).
	UINT uiNum = 0;
	if ( false == luaManager.GetValue( "BUFF_FACTOR_ID", uiNum ) )
		uiNum = 0;

	if ( AddBuffFactorINT( luaManager, uiNum ) )	/// June 326c3f30 claims every id from 10000 up
		return;

	if ( m_vecBuffFactorPtr.size() == uiNum - 1 )	/// the ordinal rule, kept by June
		PushBuffFactor( luaManager, uiNum, m_vecBuffFactorPtr, m_uiFactorPlaceholder );
	else
	{
		++m_uiFactorOrdinalReject;
		CX2JuneBuffCapture::Line( "ordinal reject BFI %u: %u factor slot(s) filled, so the next id must be %u; dropped, as in June 326c4050 and March",
			uiNum, static_cast<UINT>( m_vecBuffFactorPtr.size() ), static_cast<UINT>( m_vecBuffFactorPtr.size() + 1 ) );
	}
#else SERV_IRUHADEV_JUNE_BUFF_LOADER

	UINT uiNum = 0;		/// BUFF_FACTOR_ID 를 order로 사용함
	LUA_GET_VALUE_RETURN( luaManager, "BUFF_FACTOR_ID", uiNum, 0, DISPLAY_ERROR( L"BUFF_FACTOR_ID doesn't exist" ) );
	
	if ( m_vecBuffFactorPtr.size() == uiNum - 1 )
	{
		if ( luaManager.BeginTable( "BUFF_FACTOR" ) )
		{
			CX2BuffFactorPtr ptrBuffFactor = CX2BuffFactor::CreateBuffFactorPtr();
			if ( NULL != ptrBuffFactor )
			{
				if ( ptrBuffFactor->ParsingBuffFactor( luaManager ) )
					m_vecBuffFactorPtr.push_back( ptrBuffFactor );
			}
			else
			{
				DISPLAY_ERROR( L"new BUFF_FACTOR didn't work" );
			}

			luaManager.EndTable();
		}
		else
		{
			DISPLAY_ERROR( L"BUFF_FACTOR table doesn't exist" );
		}
	}
	else
	{
		DISPLAY_ERROR( L"BUFF_FACTOR_ID invalid" );
	}
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
}

/** @function : GetBuffTempletPtr
	@brief : 버프템플릿아이디와 매칭되는 버프템플릿을 얻어옴
	@param : 버프템플릿아이디(eBuffTempletID)
	@return : 버프아이디와 매칭되는 템플릿(CX2BuffTempletPtr)
*/
CX2BuffTempletPtr CX2BuffTempletManager::GetBuffTempletPtr( const BUFF_TEMPLET_ID eBuffTempletID_ ) const
{
	if ( !m_mapBuffTempletPtr.empty() )
	{
		map<BUFF_TEMPLET_ID, CX2BuffTempletPtr>::const_iterator mItr = m_mapBuffTempletPtr.find( eBuffTempletID_ );

		if ( m_mapBuffTempletPtr.end() != mItr )
			return mItr->second;
	}

	return CX2BuffTempletPtr();
}

/** @function : GetCombination
	@brief : 해당 요소가 어떠한 종류의 요소들로 조합되어 있는지를 알아내는 함수
	@param : 파싱 중인 스크립트의 루아매니저(luaManager_), 조합들을 얻어오는 벡터(vecType_)
	@return : 파싱 성공시 true, 실패시 false
*/
/*static*/ bool CX2BuffTempletManager::GetCombination( KLuaManager& luaManager_, OUT vector<UINT>& vecType_ )
{
	if ( luaManager_.BeginTable( "COMBINATION" ) )
	{
		int		iIndex = 1;
		UINT	uiType = 0;

		while ( luaManager_.GetValue( iIndex, uiType ) )
		{
			vecType_.push_back( uiType );

			++iIndex;
		}

		luaManager_.EndTable();

		if ( vecType_.empty() )
		{
			return DISPLAY_ERROR( L"COMBINATION elements don't exist" );	/// 벡터가 비어있으면 컴비네이션이 지정되어 있지 않은 것이므로 파싱 실패
		}
		else
			return true;
	}
	else
		return DISPLAY_ERROR( L"COMBINATION doesn't exist" );	/// 컴비네이션이 지정 되어 있지 않으므로 파싱 실패
}

CX2BuffFactorPtr CX2BuffTempletManager::GetBuffFactorPtr( const UINT uiBuffFactorNumber_ ) const
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
	/// June 326c32a0: ids up to 10000 index the main vector, 10001 and up the _INT one. Id 0 means "none"
	/// and is silent. June formats a bad id's message and discards it (no popup); here it is logged once.
	if ( uiBuffFactorNumber_ <= JUNE_BUFF_FACTOR_ID_INT_BASE )
	{
		if ( 0 == uiBuffFactorNumber_ )
			return CX2BuffFactorPtr();

		if ( uiBuffFactorNumber_ <= m_vecBuffFactorPtr.size() )
		{
			const CX2BuffFactorPtr& ptrBuffFactor = m_vecBuffFactorPtr[uiBuffFactorNumber_ - 1];
			if ( NULL == ptrBuffFactor && JuneBuffFirstReport( uiBuffFactorNumber_ ) )
				CX2JuneBuffCapture::Line( "BFI %u requested: it is a placeholder (it failed to parse at load), so nothing is applied", uiBuffFactorNumber_ );
			return ptrBuffFactor;
		}

		if ( JuneBuffFirstReport( uiBuffFactorNumber_ ) )
			CX2JuneBuffCapture::Line( "BFI %u requested but not loaded (%u factor slot(s)), so nothing is applied", uiBuffFactorNumber_, static_cast<UINT>( m_vecBuffFactorPtr.size() ) );
		return CX2BuffFactorPtr();
	}

	const UINT uiIndexINT = uiBuffFactorNumber_ - JUNE_BUFF_FACTOR_ID_INT_BASE;	/// 1-based, never 0 here
	if ( uiIndexINT <= m_vecBuffFactorPtrINT.size() )
	{
		const CX2BuffFactorPtr& ptrBuffFactor = m_vecBuffFactorPtrINT[uiIndexINT - 1];
		if ( NULL == ptrBuffFactor && JuneBuffFirstReport( uiBuffFactorNumber_ ) )
			CX2JuneBuffCapture::Line( "_INT BFI %u requested: it is a placeholder (it failed to parse at load), so nothing is applied", uiBuffFactorNumber_ );
		return ptrBuffFactor;
	}

	if ( JuneBuffFirstReport( uiBuffFactorNumber_ ) )
		CX2JuneBuffCapture::Line( "_INT BFI %u requested but not loaded (%u _INT factor slot(s)), so nothing is applied", uiBuffFactorNumber_, static_cast<UINT>( m_vecBuffFactorPtrINT.size() ) );
	return CX2BuffFactorPtr();
#else SERV_IRUHADEV_JUNE_BUFF_LOADER
	if ( uiBuffFactorNumber_ != 0 && uiBuffFactorNumber_ <= m_vecBuffFactorPtr.size() )	/// 작다로 비교 하지 않은 이유는 인덱스 0번이 uiBuffFactorNumber 1로 시작하기 때문
	{
		return m_vecBuffFactorPtr[uiBuffFactorNumber_ - 1];
	}
	else
	{
		DISPLAY_ERROR( L"uiBuffFactorNumber_ invaild" );
		return CX2BuffFactorPtr();
	}
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
}

void CX2BuffTempletManager::GetBuffFactorPtrFromBuffFactorList( IN KLuaManager& luaManager_, OUT vector<CX2BuffFactorPtr>& vecBuffFactorPtr_, OUT vector<UINT>& vecBuffFactorID_)
{
	if ( luaManager_.BeginTable( "BUFF_FACTOR" ) )
	{
		int iIndex = 1;
		UINT uiBuffFactorId = 0;
		while( luaManager_.GetValue( iIndex, uiBuffFactorId ) == true )
		{
			if ( 0 != uiBuffFactorId )
			{
				CX2BuffFactorPtr ptrBuffFactor = GetBuffFactorPtr( uiBuffFactorId );
				if ( NULL != ptrBuffFactor )
				{
					vecBuffFactorPtr_.push_back( ptrBuffFactor );
					vecBuffFactorID_.push_back( uiBuffFactorId );
				}
			}
			else
				DISPLAY_ERROR( L"uiBuffFactorId Invalid" );

			++iIndex;
		}

		luaManager_.EndTable();
	}
}



#ifdef SERV_IRUHADEV_JUNE_BUFF_LOADER
/** @function : AddBuffFactorINT
	@brief : June 326c3f30. Ids from 10000 up go to the second vector, under their own ordinal rule
		(size == id-10001). June returns "handled" for every such id even when that check fails, so a
		bad _INT id is dropped there silently; here the drop is logged.
	@return : true if the id belongs to the _INT range (whatever happened to it)
*/
bool CX2BuffTempletManager::AddBuffFactorINT( KLuaManager& luaManager_, const UINT uiBuffFactorID_ )
{
	if ( uiBuffFactorID_ < JUNE_BUFF_FACTOR_ID_INT_BASE )
		return false;

	if ( m_vecBuffFactorPtrINT.size() == uiBuffFactorID_ - ( JUNE_BUFF_FACTOR_ID_INT_BASE + 1 ) )
		PushBuffFactor( luaManager_, uiBuffFactorID_, m_vecBuffFactorPtrINT, m_uiFactorPlaceholderINT );
	else
	{
		++m_uiFactorOrdinalRejectINT;
		CX2JuneBuffCapture::Line( "ordinal reject _INT BFI %u: %u _INT factor slot(s) filled, so the next _INT id must be %u; dropped, as in June 326c3f30",
			uiBuffFactorID_, static_cast<UINT>( m_vecBuffFactorPtrINT.size() ), static_cast<UINT>( m_vecBuffFactorPtrINT.size() + JUNE_BUFF_FACTOR_ID_INT_BASE + 1 ) );
	}

	return true;
}

/** @function : PushBuffFactor
	@brief : Parse one factor entry that passed its ordinal check, the way June 326c4050 and 326c3f30
		both do (BeginTable BUFF_FACTOR, ParsingBuffFactor( lua, true )), and append it to vecBuffFactorPtr_.
		Deviation from June, by plan: an entry that fails to parse is appended as a NULL placeholder.
		June and March drop it, after which every later id fails the ordinal check; the placeholder
		keeps them aligned, and GetBuffFactorPtr returns NULL for it, so it is never applied.
*/
void CX2BuffTempletManager::PushBuffFactor( KLuaManager& luaManager_, const UINT uiBuffFactorID_, IN OUT vector<CX2BuffFactorPtr>& vecBuffFactorPtr_, IN OUT UINT& uiPlaceholder_ )
{
	CX2JuneBuffCapture kCapture;
	CX2BuffFactorPtr ptrBuffFactor;
	bool bParsed = false;
	if ( luaManager_.BeginTable( "BUFF_FACTOR" ) )
	{
		ptrBuffFactor = CX2BuffFactor::CreateBuffFactorPtr();
		if ( NULL != ptrBuffFactor )
			bParsed = ptrBuffFactor->ParsingBuffFactor( luaManager_ );
		else
			DisplayErrorMessage( L"new BUFF_FACTOR didn't work" );

		luaManager_.EndTable();
	}
	else
		DisplayErrorMessage( L"BUFF_FACTOR table doesn't exist" );

	char szEntry[64] = { 0, };
	if ( NULL != ptrBuffFactor )
		sprintf_s( szEntry, _countof( szEntry ), "BFI %u (BTI %d)", uiBuffFactorID_, static_cast<int>( ptrBuffFactor->GetBuffTempletID() ) );
	else
		sprintf_s( szEntry, _countof( szEntry ), "BFI %u", uiBuffFactorID_ );

	if ( bParsed )
		vecBuffFactorPtr_.push_back( ptrBuffFactor );
	else
	{
		vecBuffFactorPtr_.push_back( CX2BuffFactorPtr() );
		++uiPlaceholder_;
	}

	kCapture.Report( bParsed, szEntry, "placeholder" );
}

/** @function : LoadINTScriptFile
	@brief : June loads BattleFieldBuff{Templet,Factor}_INT.lua right after the main file, whether or
		not the main load worked (326c0020 / 326c13a0). March's data036 has no _INT files until Phase 13.
*/
void CX2BuffTempletManager::LoadINTScriptFile( const wchar_t* wszScriptFileName_, const char* szJuneLoader_ )
{
	const HRESULT hr = g_pKTDXApp->LoadAndDoMemory_ErrorCode( g_pKTDXApp->GetLuaBinder(), wszScriptFileName_ );
	if ( SUCCEEDED( hr ) )
		return;

	string strScriptFileName;
	ConvertWCHARToChar( strScriptFileName, wszScriptFileName_ );
	if ( HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) == hr )
		CX2JuneBuffCapture::Line( "%s not found, so no _INT buffs this run (expected until Phase 13 packs June's _INT files; June %s loads it after the main file)",
			strScriptFileName.c_str(), szJuneLoader_ );
	else
		CX2JuneBuffCapture::Line( "%s did not run (hr 0x%08x), so its _INT buffs are missing this run", strScriptFileName.c_str(), static_cast<UINT>( hr ) );
}

/** @function : LogLoadSummary
	@brief : One line after the factor files: what loaded, what became a placeholder, what was rejected.
*/
void CX2BuffTempletManager::LogLoadSummary() const
{
	CX2JuneBuffCapture::Line( "load summary: templets %u loaded, %u failed, %u duplicate; factors %u loaded + %u placeholder(s); _INT factors %u loaded + %u placeholder(s); ordinal rejects %u (+ %u _INT)",
		static_cast<UINT>( m_mapBuffTempletPtr.size() ), m_uiTempletFailed, m_uiTempletDuplicate,
		static_cast<UINT>( m_vecBuffFactorPtr.size() ) - m_uiFactorPlaceholder, m_uiFactorPlaceholder,
		static_cast<UINT>( m_vecBuffFactorPtrINT.size() ) - m_uiFactorPlaceholderINT, m_uiFactorPlaceholderINT,
		m_uiFactorOrdinalReject, m_uiFactorOrdinalRejectINT );
#ifdef SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
	/// June buff port Phase 4a: templet keys that are parsed and stored but whose consumer is not ported, counted once
	UINT uiStoreBuff = 0, uiInstanceKillDebuff = 0, uiNotSaveGageManager = 0;
	for ( map<BUFF_TEMPLET_ID, CX2BuffTempletPtr>::const_iterator mItr = m_mapBuffTempletPtr.begin(); m_mapBuffTempletPtr.end() != mItr; ++mItr )
	{
		if ( NULL == mItr->second )
			continue;
		if ( mItr->second->IsStoreBuff() )
			++uiStoreBuff;
		if ( mItr->second->IsInstanceKillDebuff() )
			++uiInstanceKillDebuff;
		if ( mItr->second->IsNotSaveGageManager() )
			++uiNotSaveGageManager;
	}
#ifdef SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF
	/// Phase 11b ported IS_STORE_BUFF's consumer (June's keep-store clear 3229fb10) and Phase 13 turned it on
	CX2JuneBuffCapture::Line( "IS_STORE_BUFF on %u templet(s): kept over a game change (June keep-store clear 3229fb10; world buffs among them return through the world-buff records)", uiStoreBuff );
	if ( 0 < uiInstanceKillDebuff + uiNotSaveGageManager )
		CX2JuneBuffCapture::Line( "unsupported (stored, consumer not ported): IS_INSTANCE_KILL_DEBUFF on %u (June DamageReact via 31fe4150), NOT_SAVE_GAGEMANAGER on %u (no June reader found)",
			uiInstanceKillDebuff, uiNotSaveGageManager );
#else // SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF
	if ( 0 < uiStoreBuff + uiInstanceKillDebuff + uiNotSaveGageManager )
		CX2JuneBuffCapture::Line( "unsupported (stored, consumer not ported): IS_STORE_BUFF on %u templet(s) (June keep-store clear 3229fb10), IS_INSTANCE_KILL_DEBUFF on %u (June DamageReact via 31fe4150), NOT_SAVE_GAGEMANAGER on %u (no June reader found)",
			uiStoreBuff, uiInstanceKillDebuff, uiNotSaveGageManager );
#endif SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF
#endif SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY
}
#endif SERV_IRUHADEV_JUNE_BUFF_LOADER
