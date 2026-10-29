#include "stdafx.h"
#include ".\x2worldmanager.h"
#if defined(SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG) && defined(SERV_IRUHADEV_OFFLINE)
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG


#ifdef KHELP_SYSTEM_TEST // 2008-10-24
IMPLEMENT_HELP_SYSTEM( CX2WorldManager )
{
    WCHAR   wbuffer[1024];
    int     ix = 16;
    int     iy = 16;
    StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer), L"%S", __FUNCTION__ );

    if ( g_pMain && g_pData )
    {
	    g_pMain->GetFontForDebuf()->OutTextXY( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1), 
		    CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
        g_pData->GetWorldManager()->Debug_RenderHelp( ix, iy );
    }//if
}//IMPLEMENT_HELP_SYSTEM( CX2WorldManager )
#endif // KHELP_SYSTEM_TEST // 2008-10-24


///-----------------------------------------------------------------------------
//
// constructor.
CX2WorldManager::CX2WorldManager()
{
	::InitializeCriticalSection( &m_csWorldManager );
    m_pX2WorldLatestRef         = NULL;
}//CX2WorldManager::CX2WorldManager()


// destructor.
CX2WorldManager::~CX2WorldManager(void)
{
	::EnterCriticalSection( &m_csWorldManager );

	for( int i = 0; i < (int)m_WorldDataList.size(); i++ )
	{
		CX2World::WorldData* pWorldData = m_WorldDataList[i];
		SAFE_DELETE( pWorldData );
	}
	m_WorldDataList.clear();

	::LeaveCriticalSection( &m_csWorldManager );

	::DeleteCriticalSection( &m_csWorldManager );
}//CX2WorldManager::~CX2WorldManager()

#ifdef X2TOOL
CX2World* CX2WorldManager::CreateWorld( wstring wstrWorldName, KLuabinder* pKLuabinder )
{
	CX2World::WorldData worldData;
	worldData.dataFileName = wstrWorldName;
	CX2World* pWorld = new CX2World( &worldData );
	pWorld->OpenScriptFile( wstrWorldName.c_str() );

	return pWorld;
}
#endif

CX2World* CX2WorldManager::CreateWorld( const CX2World::WORLD_ID worldID, KLuabinder* pKLuabinder /*= NULL*/, const bool bBackgroundLoad_ /*= false*/ )
{
	CX2World::WorldData* pWorldData = NULL;

	{
		CSLock locker( m_csWorldManager );

		for( UINT i = 0; i < m_WorldDataList.size(); i++ )
		{
			if( worldID == m_WorldDataList[i]->worldID )
			{
	#ifdef FIX_3D_EFFECT_NOT_WORKING_FIRST_SECONDS
				g_pKTDXApp->GetDSManager()->SetListenerData( D3DXVECTOR3( 99999.f, 99999.f, 99999.f ) );
	#endif //FIX_3D_EFFECT_NOT_WORKING_FIRST_SECONDS
				pWorldData = m_WorldDataList[i];
				break;
			}
		}//for
	}

	// WorldData가 없으면
#ifdef SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG
	// June effect C3x: a world ID with no WorldData.lua row used to return NULL silently, and
	// CX2DungeonGame::StageLoading then read m_pWorld->GetLineMap() through it ( crash, no log ).
	if ( NULL == pWorldData )
	{
#ifdef SERV_IRUHADEV_OFFLINE
		CX2OfflineLog::Server( L"JUNE_FX C3x world %d: no WorldData row ( data040 WorldData.lua ), CreateWorld returns NULL", (int) worldID );
#endif SERV_IRUHADEV_OFFLINE
		return NULL;
	}
#else
	if ( NULL == pWorldData )
		return NULL;
#endif SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG
	else // WorldData가 있으면
	{
		CX2World* pCX2World = new CX2World( pWorldData, bBackgroundLoad_ );
		ASSERT( pCX2World );
#ifdef SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
		// June effect C3x-1: June's CreateWorld ( 32803d10 @32803e63-32803f13; same pick for the main world of 32803f60
		// @328040eb-328041ad ) opens dataFileName_CodeEvent instead of dataFileName when g_pInstanceData is set, code event 46
		// is on ( map +0xa18 node byte == 1, i.e. IsEnableCode; March's ScriptID_Code.h names 46 CEI_TEAR_OF_ELWOMAN ) and the
		// name is not empty. Offline nothing sends EGS_UPDATE_CODE_EVENT_NOT, so the plain name stays.
		const WCHAR* pWorldScript = pWorldData->dataFileName.c_str();
#ifdef SERV_CODE_EVENT
		if ( NULL != g_pInstanceData && true == g_pInstanceData->IsEnableCode( 46 ) && false == pWorldData->dataFileName_CodeEvent.empty() )
			pWorldScript = pWorldData->dataFileName_CodeEvent.c_str();
#endif SERV_CODE_EVENT
#ifdef SERV_IRUHADEV_OFFLINE
		if ( pWorldScript != pWorldData->dataFileName.c_str() || pWorldData->dataFileName != pWorldData->dataFileName_Original )
			CX2OfflineLog::Server( L"JUNE_FX C3x-1 world %d %s: variant script %s ( plain %s )",
				(int) worldID, pWorldData->worldName.c_str(), pWorldScript, pWorldData->dataFileName_Original.c_str() );
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_WORLD_FESTIVAL

		m_pX2WorldLatestRef = pCX2World; // 가장 최근에 생성한 월드 객체에 대한 참고 포인터를 저장해 둔다.

#ifdef SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG
		// June effect C3x: name the world whose script failed, or that never set a line map
		// ( m_pLineMap stays NULL, which StageLoading dereferences ).
#ifdef SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
		const bool bWorldScriptOpened = pCX2World->OpenScriptFile( pWorldScript, pKLuabinder );
#else
		const bool bWorldScriptOpened = pCX2World->OpenScriptFile( pWorldData->dataFileName.c_str(), pKLuabinder );
#endif SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
#ifdef SERV_IRUHADEV_OFFLINE
		if ( false == bWorldScriptOpened )
			CX2OfflineLog::Server( L"JUNE_FX C3x world %d %s: script %s failed to load ( XEM_ERROR71 / lua error in ErrorLog.txt )",
				(int) worldID, pWorldData->worldName.c_str(), pWorldData->dataFileName.c_str() );
		else if ( NULL == pCX2World->GetLineMap() )
			CX2OfflineLog::Server( L"JUNE_FX C3x world %d %s: script %s loaded but set no line map",
				(int) worldID, pWorldData->worldName.c_str(), pWorldData->dataFileName.c_str() );
#endif SERV_IRUHADEV_OFFLINE
#else
#ifdef SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
		pCX2World->OpenScriptFile( pWorldScript, pKLuabinder );
#else
		pCX2World->OpenScriptFile( pWorldData->dataFileName.c_str(), pKLuabinder );
#endif SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
#endif SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG

#ifdef X2OPTIMIZE_GAMEOPTION_BUGFIX
		pCX2World->SetMapDetail( g_pMain->GetGameOption().GetOptionList().m_MapDetail );
#endif//X2OPTIMIZE_GAMEOPTION_BUGFIX

		return pCX2World;
	}
}//CX2WorldManager::CreateWorld()

#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
/** @function : CreateJuneAdditionalWorlds
	@brief : Atlas E7, the second half of June's 32803f60 ( callers 327cd1b0 dungeon stage load, 32d52fa0 / 32d54ae0 battle field ).
			 After the main world ( = CreateWorld above ), one more CX2World per WorldData dataFileName_Additional name
			 ( +0x58, @32804248-328042a6 ): new CX2World( same WorldData, bBackgroundLoad ), OpenScriptFile( name, pKLuabinder ),
			 the map detail ( 327fbeb0 with g_pMain +0x1a8 ), then hidden and silent: SetShowObject( false ) 327fba60,
			 PlayEffectBGM( false ) 327fb2a0, Play3DEffectBGM( false ) 327fb320, push_back into the caller's vector.
			 Not the latest-world reference ( June writes +0x2c for the main world only ).
*/
void CX2WorldManager::CreateAdditionalWorlds( const CX2World::WORLD_ID worldID, std::vector<CX2World*>& vecAdditionalWorld_, KLuabinder* pKLuabinder /*= NULL*/, const bool bBackgroundLoad_ /*= false*/ )
{
	CX2World::WorldData* pWorldData = NULL;
	{
		CSLock locker( m_csWorldManager );
		for( UINT i = 0; i < m_WorldDataList.size(); i++ )
		{
			if( worldID == m_WorldDataList[i]->worldID )
			{
				pWorldData = m_WorldDataList[i];
				break;
			}
		}
	}
	if ( NULL == pWorldData )
		return;

	for ( UINT i = 0; i < pWorldData->dataFileName_Additional.size(); i++ )
	{
		CX2World* pAdditionalWorld = new CX2World( pWorldData, bBackgroundLoad_ );
		const bool bOk = pAdditionalWorld->OpenScriptFile( pWorldData->dataFileName_Additional[i].c_str(), pKLuabinder );
#ifdef X2OPTIMIZE_GAMEOPTION_BUGFIX
		pAdditionalWorld->SetMapDetail( g_pMain->GetGameOption().GetOptionList().m_MapDetail );
#endif//X2OPTIMIZE_GAMEOPTION_BUGFIX
		pAdditionalWorld->SetShowObject( false );
		pAdditionalWorld->PlayEffectBGM( false );
		pAdditionalWorld->Play3DEffectBGM( false );
		vecAdditionalWorld_.push_back( pAdditionalWorld );
#if defined( SERV_IRUHADEV_ELYSION_DEBUG ) && defined( SERV_IRUHADEV_OFFLINE )
		CX2OfflineLog::Server( L"[ELYDBG] additional world %d: world %d %s script %s %s, slot %d ( hidden )",
			(int) i, (int) worldID, pWorldData->worldName.c_str(), pWorldData->dataFileName_Additional[i].c_str(),
			bOk ? L"loaded" : L"FAILED", (int) vecAdditionalWorld_.size() - 1 );
#endif // SERV_IRUHADEV_ELYSION_DEBUG && SERV_IRUHADEV_OFFLINE
	}
}
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B

bool CX2WorldManager::OpenScriptFile( const WCHAR* pFileName )
{
    ASSERT( pFileName );
    CSLock locker( m_csWorldManager );

	//바인더에 등록
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pWorldManager", this );

    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR72, pFileName );

		return false;
    }

	return true;
}


//{{ seojt // 2008-10-22, 10:33
void CX2WorldManager::OnPreDestroyWorld(CX2World* pWorld_)
{
    ASSERT( pWorld_ );
    /** 어떤 월드 객체가 파괴되면, 가장 최근에 생성한 월드 객체에 대한 참고 포인터를 무효화시킨다.
        CX2WorldManager가 객체를 생성하는 일은 담당하지만, 객체를 파괴하는 일은 맡고 있지
        않으므로, 이 부분을 수정해야 할 것이다. - jintaeks on 2008-10-22, 10:30
    
        @수정 안:
        
            step 1) CX2WorldManager는 자신이 생성한 모든 게임 월드 객체를 리스트에 유지한다.

            step 2) CX2World가 직접 delete되지 않는 다는 것을 보장하기 위해 CX2World의 파괴자
                    ~CX2World()를 private로 선언하고, CX2WorldManager에 CX2World 객체를 파괴하는
                    헬퍼 함수를 구현한다. CX2WorldManager는 CX2World의 private 파괴자에 접근할 수
                    있어야 하므로, CX2World에서 friend로 선언되어야 할 것이다.

            step 3) 각 게임 스테이트는 CX2World를 직접 파괴하지 않고 - 현재 직접 SAFE_DELETE()를 호출한다 -
                    CX2WorldManager에게 CX2World::WORLD_ID에 해당하는 월드를 파괴할 것을 요청한다.

            요약) CX2WorldManager가 객체를 생성하는 factory의 역할을 하므로, 객체의 파괴와 관리도 담당시켜서
                    완전한 factory가 되도록 해야 할 것이다.

        - jintaeks on 2008-10-22, 10:26 */

    m_pX2WorldLatestRef = NULL;

    // 현재 월드에서 요청한 리소스 정보를 모두 clear한다.
    ASSERT( g_pKTDXApp && g_pKTDXApp->GetDeviceManager() );
}//CX2WorldManager::OnPreDestroyWorld()
//}} seojt // 2008-10-22, 10:33



const CX2World::WorldData* CX2WorldManager::GetWorldData( CX2World::WORLD_ID eWorldID )
{
	for( UINT i=0; i<m_WorldDataList.size(); i++ )
	{
		CX2World::WorldData* pWorldData = m_WorldDataList[i];
		if( pWorldData->worldID == eWorldID )
		{
			return pWorldData;
		}
	}

	return NULL;
}


bool CX2WorldManager::AddWorldData_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
    TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	CX2World::WorldData* pWorldData = new CX2World::WorldData;
	LUA_GET_VALUE_RETURN_ENUM(	luaManager, "worldID",			pWorldData->worldID,		CX2World::WORLD_ID,	CX2World::WI_NONE,	SAFE_DELETE(pWorldData); return false; );
	LUA_GET_VALUE_RETURN(		luaManager, "worldName",		pWorldData->worldName,		L"",										SAFE_DELETE(pWorldData); return false; );
	LUA_GET_VALUE_RETURN(		luaManager, "dataFileName",	pWorldData->dataFileName,	L"",										SAFE_DELETE(pWorldData); return false; );
#ifdef SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
	// June effect C3x-1: June's AddWorldData_LUA ( 32805770 ). After dataFileName it keeps a copy of the plain name ( +0x3c,
	// @32805aeb ), then reads "dataFileName_Festival" only while the local clock is before 2016-03-16 04:00 and, when that is
	// not empty, swaps it in as dataFileName ( @32805af0-32805dc1 ). June builds three CTimes ( 2016-03-23 00:00,
	// 2016-04-19 23:59, 2016-03-16 04:00 ) and compares only the last ( JUNE_BUGS 198 ). Then "dataFileName_CodeEvent" ( +0x68, @32805f69 ).
	// June also reads a "dataFileName_Additional" table here ( +0x58 vector, @32805f81-32805fee ): routed to ELYSION_PORT_PHASES.md.
	pWorldData->dataFileName_Original = pWorldData->dataFileName;
	{
		struct tm tmFestivalCutoff = { 0, 0, 4, 16, 2, 116, 0, 0, -1 };	// sec, min, hour, mday, mon ( March ), year - 1900, -, -, isdst -1 ( @32805b98-32805bc2 )
		const __time64_t tNow = _time64( NULL );
		const __time64_t tFestivalCutoff = _mktime64( &tmFestivalCutoff );
		if ( tNow < tFestivalCutoff )
		{
			wstring wstrFestival;
			LUA_GET_VALUE( luaManager, "dataFileName_Festival", wstrFestival, L"" );
			if ( false == wstrFestival.empty() )
				pWorldData->dataFileName = wstrFestival;
		}
	}
	LUA_GET_VALUE( luaManager, "dataFileName_CodeEvent", pWorldData->dataFileName_CodeEvent, L"" );
#endif SERV_IRUHADEV_JUNE_WORLD_FESTIVAL
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
	// Atlas E7: June 32805770 @32805f81-32805fee, after dataFileName_CodeEvent: the "dataFileName_Additional" table, values 1..n
	// until the first missing one ( 32feafd0 ), each pushed into +0x58 ( 31aad250 ).
	if( true == luaManager.BeginTable( "dataFileName_Additional" ) )
	{
		int iAdditionalIndex = 1;
		wstring wstrAdditional;
		while( true == luaManager.GetValue( iAdditionalIndex, wstrAdditional ) )
		{
			pWorldData->dataFileName_Additional.push_back( wstrAdditional );
			iAdditionalIndex++;
		}
		luaManager.EndTable();
	}
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B

	m_WorldDataList.push_back( pWorldData );
	return true;
}


//{{ seojt // 2008-10-21, 15:14
bool CX2WorldManager::SetPreprocessingData_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
    TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	WORLD_PREPROCESSING_INFO    preInfo;

    // qfff
    preInfo.m_eInfoType = X2WM_PPIT_UNKNOWN;
	LUA_GET_VALUE( luaManager, "meshName",             preInfo.m_wstrName,         L"" );
	//LUA_GET_VALUE( luaManager, "centerPos",		    preInfo.m_vCenter,          D3DXVECTOR3(0,0,0) );
	LUA_GET_VALUE( luaManager, "boundingSphereRadius",	preInfo.m_fBSphereRadiua,  0.f );
#ifdef  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
    LUA_GET_USER_DEFINED_TYPE_VALUE( luaManager, "centerPos", preInfo.m_vCenter, D3DXVECTOR3(0,0,0) );
#else   X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
    preInfo.m_vCenter = lua_tinker::get<D3DXVECTOR3>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "centerPos" );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING


    ///-----------------------------------------------------------------------------
    // 현재 생성된 CX2World*를 얻는다.
    // 이름에 해당하는 게임 객체를 찾아서 바운딩 구 정보를 설정한다.
    //
    if ( m_pX2WorldLatestRef )
    {
        CX2WorldObject* pWorldObject = m_pX2WorldLatestRef->FindObjectByName( preInfo.m_wstrName );
        if ( pWorldObject )
        {
            pWorldObject->SetBoundingRadius( preInfo.m_fBSphereRadiua );
            pWorldObject->SetCenter( preInfo.m_vCenter );
        }//if
    }//if

	return true;
}//CX2WorldManager::SetPreprocessingData_LUA()
//}} seojt // 2008-10-21, 15:14


CX2World::WORLD_ID CX2WorldManager::GetWorldIDUsingLineMapFileName( const WCHAR* pLineMapFileName )
{
	CSLock locker( m_csWorldManager );

	for( UINT i=0; i<m_WorldDataList.size(); i++ )
	{
		CX2World::WorldData* pWorldData = m_WorldDataList[i];
		CX2World* pWorld = new CX2World( pWorldData );

		lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pWorld", pWorld );

		KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
		Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( pWorldData->dataFileName );
		if( Info == NULL )
			continue;

		wstring wstrWorldScript;
		ConvertCharToWCHAR( wstrWorldScript, Info->pRealData, Info->size );

		int iIndex		= wstrWorldScript.find( L"g_pWorld:SetLineMap" );
		int iStartIndex = iIndex;
		iStartIndex		= wstrWorldScript.find( L"\"", iStartIndex );
		int iEndIndex	= iStartIndex+1;
		iEndIndex		= wstrWorldScript.find( L"\"", iEndIndex );

		wstring wstrLineMapFileName = wstrWorldScript.substr( iStartIndex+1, iEndIndex-iStartIndex-1 );
		MakeUpperCase( wstrLineMapFileName );
		wstring wstrInputLineMapFileName = pLineMapFileName;
		MakeUpperCase( wstrInputLineMapFileName );
		if( 0 == wstrLineMapFileName.compare( wstrInputLineMapFileName.c_str() ) )
		{
			SAFE_DELETE( pWorld );
			return pWorldData->worldID;
		}
		SAFE_DELETE( pWorld );
	}

	return CX2World::WI_NONE;
}

// #ifdef BACKGROUND_LOADING_TEST // 2008-10-18
// CX2World* CX2WorldManager::ThreadRequest_CreateWorld(CX2World::WORLD_ID worldID_, KLuabinder* pKLuabinder_ /*=NULL*/)
// {
//     /** EnableThreadLoading( true )의 의미:
// 
//         EnableThreadLoading( true )로 설정되면, CreateWorld()내부에서 읽어 들인 lua 스크립트의
//         메시 로드는 실제 로드가 아니라, 쓰레드 큐잉한다.
//         - jintaeks on 2008-10-24, 10:38 */
// 
//     ASSERT( g_pKTDXApp );
//     g_pKTDXApp->GetDeviceManager()->EnableThreadLoading( true );
//     CX2World*   pWorld = CreateWorld( worldID_, pKLuabinder_ );
//     ASSERT( pWorld );
//     g_pKTDXApp->GetDeviceManager()->EnableThreadLoading( false );
// 
//     return  pWorld;
// }//CX2WorldManager::ThreadRequest_CreateWorld()
// #endif // BACKGROUND_LOADING_TEST // 2008-10-18


void CX2WorldManager::Debug_RenderHelp( int ix_, int iy_ )
{
    int     ix = ix_ + 16;
    int     iy = iy_ + 16;
    WCHAR   wbuffer[1024];

    if ( m_pX2WorldLatestRef )
    {
        const CX2World::WorldData*  pWorldData = m_pX2WorldLatestRef->GetWorldData();
        if ( pWorldData )
        {
            StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer),
                L"WorldID = %i\n"
                L"WorldName = %s\n"
                L"DataFileName = %s\n",
                pWorldData->worldID,
                pWorldData->worldName.c_str(),
                pWorldData->dataFileName.c_str() );
	        g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1), 
		        CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
            iy += 16 * 3;
        }//if
    }//if
}//CX2WorldManager::Debug_RenderHelp()

