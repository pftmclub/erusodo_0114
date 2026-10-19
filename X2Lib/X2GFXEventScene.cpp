#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
#include "X2GFXEventScene.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

// Log tag [JUNE_GFX] SCENE ( ErrorLog.txt )
static void JuneGfxSceneLog( const char* szFormat, ... )
{
	char szBuf[ 1024 ] = { 0, };
	va_list args;
	va_start( args, szFormat );
	_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
	va_end( args );
	KLOG( "ErrorLog.txt" ) << "[JUNE_GFX] SCENE " << szBuf << fileout;
}

static std::string JuneGfxSceneToUTF8( const std::wstring& wstr )
{
	if ( wstr.empty() == true )
		return std::string();
	int iLen = WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL );
	std::string str( iLen > 0 ? iLen - 1 : 0, '\0' );
	if ( iLen > 1 )
		WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), -1, &str[0], iLen, NULL, NULL );
	return str;
}

static std::wstring JuneGfxSceneFromUTF8( const std::string& str )
{
	if ( str.empty() == true )
		return std::wstring();
	int iLen = MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, NULL, 0 );
	std::wstring wstr( iLen > 0 ? iLen - 1 : 0, L'\0' );
	if ( iLen > 1 )
		MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, &wstr[0], iLen );
	return wstr;
}

// 31ee3350 ( string attribute, empty when missing ) and 31edf910 ( int attribute through atoi, left at its default when missing )
static std::string JuneGfxSceneGetProp( xmlNode* pNode, const char* szName )
{
	std::string str;
	xmlChar* pValue = xmlGetProp( pNode, (const xmlChar*)szName );
	if ( pValue != NULL )
	{
		str = (const char*)pValue;
		xmlFree( pValue );
	}
	return str;
}

static bool JuneGfxSceneGetIntProp( xmlNode* pNode, const char* szName, int& iValue )
{
	xmlChar* pValue = xmlGetProp( pNode, (const xmlChar*)szName );
	if ( pValue == NULL )
		return false;
	iValue = atoi( (const char*)pValue );
	xmlFree( pValue );
	return true;
}

static bool JuneGfxSceneIsElement( xmlNode* pNode, const char* szName )
{
	return pNode != NULL && pNode->type == XML_ELEMENT_NODE && strcmp( (const char*)pNode->name, szName ) == 0;
}

// June 3226aed0: the scene is made by CX2Data ( 31edc620 ) once the root movie exists ( the bridge finds nothing otherwise )
CX2GFXEventScene::CX2GFXEventScene()
	: m_bPlaying( false )
	, m_bNotifyNewEpicQuest( false )
	, m_bSkipHeld( false )
	, m_fSkipTime( 0.f )
	, m_bSkipRequested( false )
	, m_pSound( NULL )
	, m_pBridge( NULL )
	, m_hBlackLayOut( INVALID_PARTICLE_SEQUENCE_HANDLE )
{
	if ( g_pKTDXApp != NULL )
		m_pBridge = new CKTDGGFXEventSceneBridge( g_pKTDXApp->GetJuneGFXManager(), this );
}

CX2GFXEventScene::~CX2GFXEventScene()
{
	SAFE_DELETE( m_pBridge );
	if ( m_pSound != NULL && g_pKTDXApp != NULL && g_pKTDXApp->GetDeviceManager() != NULL )
	{
		g_pKTDXApp->GetDeviceManager()->JuneCloseDeviceByPointer( m_pSound );
		m_pSound = NULL;
	}
	m_pSound = NULL;
	DestroyBlackLayOut();
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// data
// ---------------------------------------------------------------------------------------------------------------------------------------

// 31ee7000: <Village><VillageList Name="..."/>...</Village>
bool CX2GFXEventScene::LoadVillageNames( std::vector< std::string >& vecNames )
{
	vecNames.clear();
	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( std::wstring( L"VillageName.xml" ) );
	if ( Info == NULL || Info->pRealData == NULL || Info->size <= 0 )
		return false;
	xmlDoc* pDoc = xmlReadMemory( Info->pRealData, Info->size, "VillageName.xml", NULL, 0 );
	if ( pDoc == NULL )
		return false;
	xmlNode* pRoot = xmlDocGetRootElement( pDoc );
	if ( JuneGfxSceneIsElement( pRoot, "Village" ) == true )
	{
		for ( xmlNode* pNode = pRoot->children; pNode != NULL; pNode = pNode->next )
		{
			if ( JuneGfxSceneIsElement( pNode, "VillageList" ) == true )
				vecNames.push_back( JuneGfxSceneGetProp( pNode, "Name" ) );
		}
	}
	xmlFreeDoc( pDoc );
	return true;
}

// 31eecc40 ( one region file, reads xPos / yPos / wordBalloonPosX / wordBalloonPosY too ) and 31eed7a0 ( GFXEventSceneInfo.xml: does not ).
// Rows are inserted, never replaced ( the 31ef6400 / 31ef6eb0 pair ): the first file to name a questID keeps it.
int CX2GFXEventScene::LoadSceneXml( const WCHAR* wszFile, bool bRegion )
{
	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_PTR Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( std::wstring( wszFile ) );
	if ( Info == NULL || Info->pRealData == NULL || Info->size <= 0 )
		return -1;
	xmlDoc* pDoc = xmlReadMemory( Info->pRealData, Info->size, "eventScene.xml", NULL, 0 );
	if ( pDoc == NULL )
		return -1;

	int iAdded = 0;
	xmlNode* pRoot = xmlDocGetRootElement( pDoc );
	if ( JuneGfxSceneIsElement( pRoot, "EventScene" ) == true )
	{
		for ( xmlNode* pData = pRoot->children; pData != NULL; pData = pData->next )
		{
			if ( JuneGfxSceneIsElement( pData, "EventSceneData" ) == false )
				continue;

			SceneInfo kInfo;
			const std::string strQuestID = JuneGfxSceneGetProp( pData, "questID" );
			kInfo.m_strSwfFileName = JuneGfxSceneGetProp( pData, "swfFileName" );

			for ( xmlNode* pList = pData->children; pList != NULL; pList = pList->next )
			{
				if ( JuneGfxSceneIsElement( pList, "EventSceneDataList" ) == false )
					continue;

				KJuneGfxSceneFrame kFrame;
				JuneGfxSceneGetIntProp( pList, "frame", kFrame.m_iFrame );
				kFrame.m_strSoundKey			= JuneGfxSceneGetProp( pList, "soundKey" );
				kFrame.m_strCharacterNameKey	= JuneGfxSceneGetProp( pList, "characterNameKey" );
				kFrame.m_strTextKey				= JuneGfxSceneGetProp( pList, "textKey" );
				kFrame.m_strWordBalloonName		= JuneGfxSceneGetProp( pList, "wordBalloonName" );
				if ( bRegion == true )
				{
					JuneGfxSceneGetIntProp( pList, "wordBalloonPosX", kFrame.m_iWordBalloonPosX );
					JuneGfxSceneGetIntProp( pList, "wordBalloonPosY", kFrame.m_iWordBalloonPosY );
				}

				for ( xmlNode* pSymbol = pList->children; pSymbol != NULL; pSymbol = pSymbol->next )
				{
					if ( JuneGfxSceneIsElement( pSymbol, "SymbolData" ) == false )
						continue;
					KJuneGfxSceneSymbol kSymbol;
					kSymbol.m_strSymbolName = JuneGfxSceneGetProp( pSymbol, "symbolName" );
					JuneGfxSceneGetIntProp( pSymbol, "delayTime", kSymbol.m_iDelayTime );
					if ( bRegion == true )
					{
						JuneGfxSceneGetIntProp( pSymbol, "xPos", kSymbol.m_iPosX );
						JuneGfxSceneGetIntProp( pSymbol, "yPos", kSymbol.m_iPosY );
					}
					kFrame.m_vecSymbol.push_back( kSymbol );
				}
				kInfo.m_vecFrame.push_back( kFrame );
			}

			if ( m_mapSceneInfo.insert( std::make_pair( JuneGfxSceneFromUTF8( strQuestID ), kInfo ) ).second == true )
				++iAdded;
		}
	}
	xmlFreeDoc( pDoc );
	return iAdded;
}

// 31eed660 ( VillageName.xml, then <NAME>.xml for each ) and 31eed7a0 ( GFXEventSceneInfo.xml ), from the startup loader ( 3258c470 @3259085e )
void CX2GFXEventScene::LoadSceneInfo()
{
	m_mapSceneInfo.clear();

	std::vector< std::string > vecVillage;
	const bool bVillageList = LoadVillageNames( vecVillage );
	int iRegionRows = 0;
	int iRegionFiles = 0;
	for ( size_t i = 0; i < vecVillage.size(); ++i )
	{
		const std::wstring wstrFile = JuneGfxSceneFromUTF8( vecVillage[ i ] ) + L".xml";
		const int iAdded = LoadSceneXml( wstrFile.c_str(), true );
		if ( iAdded >= 0 )
		{
			iRegionRows += iAdded;
			++iRegionFiles;
		}
		else
			JuneGfxSceneLog( "region file %s not loaded ( kom data036 )", JuneGfxSceneToUTF8( wstrFile ).c_str() );
	}
	const int iGfxRows = LoadSceneXml( L"GFXEventSceneInfo.xml", false );

	JuneGfxSceneLog( "LOAD VillageName.xml %s, %d region files %d rows, GFXEventSceneInfo.xml %d rows, %d scenes in all",
		bVillageList ? "ok" : "MISSING", iRegionFiles, iRegionRows, iGfxRows, (int)m_mapSceneInfo.size() );
	if ( bVillageList == false || iGfxRows < 0 )
		JuneGfxSceneLog( "LOAD data is missing: stage June's data036 XMLs ( GFXEventSceneInfo.xml, the region files, VillageName.xml ) and pack data036" );
}

// 31ee3160: found when the map has the key ( the lower-bound node compares equal )
bool CX2GFXEventScene::HasSceneInfo( const std::wstring& wstrName ) const
{
	return m_mapSceneInfo.find( wstrName ) != m_mapSceneInfo.end();
}

// 31ee3250
bool CX2GFXEventScene::GetSwfFileName( const std::wstring& wstrName, std::string& strSwfFileName ) const
{
	std::map< std::wstring, SceneInfo >::const_iterator it = m_mapSceneInfo.find( wstrName );
	if ( it == m_mapSceneInfo.end() )
		return false;
	strSwfFileName = it->second.m_strSwfFileName;
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// play
// ---------------------------------------------------------------------------------------------------------------------------------------

// 3226a010: this scene's flag, or the Lua scene's ( 325ae1e0 = the byte at +0x40, March's m_bIsPlayingEventScene )
bool CX2GFXEventScene::IsPlaying() const
{
	if ( m_bPlaying == true )
		return true;
	return g_pData != NULL && g_pData->GetEventScene() != NULL && g_pData->GetEventScene()->GetIsPlayingEventScene() == true;
}

// 3226a130: the scene that ended last is consumed by the question
bool CX2GFXEventScene::IsRecentlyPlayed( const std::wstring& wstrName )
{
	if ( m_wstrRecent.compare( wstrName ) == 0 )
	{
		m_wstrRecent.clear();
		return true;
	}
	return false;		// June's else-branch asks the Lua scene ( 325aece0 ): its string has no writer ( JUNE_BUGS )
}

CKTDGParticleSystem* CX2GFXEventScene::GetBlackLayOutParticleSystem()
{
	// 322ceaa0: g_pData +0x84, the particle system whose script holds "EventScene_BlackLayOut" ( GameMajorParticle_INT.txt in June's data036 )
	return ( g_pData != NULL ) ? g_pData->GetGameMajorParticle() : NULL;
}

void CX2GFXEventScene::CreateBlackLayOut()
{
	m_hBlackLayOut = INVALID_PARTICLE_SEQUENCE_HANDLE;
	CKTDGParticleSystem* pParticle = GetBlackLayOutParticleSystem();
	if ( pParticle == NULL )
	{
		JuneGfxSceneLog( "Play: no game major particle system here, no black layout" );
		return;
	}
	// Play @3226b5c5..3226b611 -> 31f33bb0 -> 31f318d0 -> CreateSequence 31cf90f0: ( no object, name, (0,0,0), -1, -1, -1, -1, -1.0f, true, 0.0f, false )
	m_hBlackLayOut = pParticle->CreateSequenceHandle( NULL, L"EventScene_BlackLayOut", D3DXVECTOR3( 0.f, 0.f, 0.f ), -1.f, -1.f, -1, -1, -1.f, true, 0.f, false );
	if ( m_hBlackLayOut == INVALID_PARTICLE_SEQUENCE_HANDLE )
		JuneGfxSceneLog( "Play: particle EventScene_BlackLayOut not found ( June's GameMajorParticle_INT.txt is not the one mounted ): no black layout" );
}

void CX2GFXEventScene::DestroyBlackLayOut()
{
	if ( m_hBlackLayOut == INVALID_PARTICLE_SEQUENCE_HANDLE )
		return;
	CKTDGParticleSystem* pParticle = GetBlackLayOutParticleSystem();
	if ( pParticle != NULL )
		pParticle->DestroyInstanceHandle( m_hBlackLayOut );
	m_hBlackLayOut = INVALID_PARTICLE_SEQUENCE_HANDLE;
}

// 32269f20( bShow ): the dialogs ( 31d05af0( bShow, 0 ) ), then for bShow the quest list, and the mini-map / world map follow the hidden state.
// June's SetShow takes a hide mode; march's is the bool alone until G5 ports the mode mechanism ( this scene only ever uses mode 0 ).
void CX2GFXEventScene::SetSceneUIShow( bool bShow )
{
	if ( g_pKTDXApp == NULL || g_pKTDXApp->GetDGManager() == NULL || g_pKTDXApp->GetDGManager()->GetDialogManager() == NULL )
		return;
	CKTDGUIDialogManager* pDialogManager = g_pKTDXApp->GetDGManager()->GetDialogManager();
	pDialogManager->SetShow( bShow );

	if ( bShow == true && g_pData != NULL && g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIQuestNew() != NULL
		&& g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		g_pData->GetUIManager()->GetUIQuestNew()->ResetQuestUI( true, false, true );		// 323dfa80( 1, 0, 1 )
	}

#ifdef DIALOG_SHOW_TOGGLE
	const bool bHide = pDialogManager->GetHideDialog();		// 31d03920
#else DIALOG_SHOW_TOGGLE
	const bool bHide = false;
#endif DIALOG_SHOW_TOGGLE
	if ( g_pInstanceData != NULL && g_pInstanceData->GetMiniMapUI() != NULL )
	{
		CX2MiniMapUI* pMiniMapUI = g_pInstanceData->GetMiniMapUI();
		pMiniMapUI->SetShowMiniMap( pMiniMapUI->GetCurrentMiniMapType(), bHide == false );		// 32264170
		CX2WorldMapUI* pWorldMapUI = pMiniMapUI->GetWorldMapUI();
		if ( pWorldMapUI != NULL )
		{
			pWorldMapUI->OpenWorldMap( false );			// 322630a0( 0 )
			pWorldMapUI->OpenFieldMap( false, -1 );		// 322633c0( 0, -1 )
		}
	}
}

// 3226a050: the previous voice is closed, the new one opened ( 10 buffers, 3D, no distance limit ) and played without loop or 3D
void CX2GFXEventScene::PlaySound( const std::wstring& wstrKey )
{
	if ( g_pKTDXApp == NULL || g_pKTDXApp->GetDeviceManager() == NULL )
		return;
	if ( m_pSound != NULL )
	{
		g_pKTDXApp->GetDeviceManager()->JuneCloseDeviceByPointer( m_pSound );
		m_pSound = NULL;
	}
	m_pSound = g_pKTDXApp->GetDeviceManager()->OpenSound( wstrKey, 10, true, -1.f );
	if ( m_pSound != NULL )
		m_pSound->Play( false, false );
}

// 3226b130
void CX2GFXEventScene::Play( const std::wstring& wstrName, int iFrame /*= -1*/, int iArg /*= 0*/ )
{
	if ( wstrName.size() <= 1 )		// @3226b17d: an empty or one-character name is dropped
		return;

	if ( m_bPlaying == true )
	{
		// @3226b198: asked for while one plays: queued once ( a name already in the queue is not queued again )
		for ( std::deque< QueueEntry >::const_iterator it = m_dequeQueue.begin(); it != m_dequeQueue.end(); ++it )
		{
			if ( it->m_wstrName.compare( wstrName ) == 0 )
				return;
		}
		QueueEntry kEntry;
		kEntry.m_wstrName	= wstrName;
		kEntry.m_iFrame		= iFrame;
		m_dequeQueue.push_back( kEntry );
		return;
	}

	m_wstrName = wstrName;		// @3226b2ea

	WCHAR wszPath[ 260 ] = { 0, };
	std::string strSwf;
	if ( GetSwfFileName( wstrName, strSwf ) == true )		// 31ee3160 then 31ee3250
		StringCchPrintf( wszPath, 260, L"%s.swf", JuneGfxSceneFromUTF8( strSwf ).c_str() );
	else
		StringCchPrintf( wszPath, 260, L"%s.swf", wstrName.c_str() );

	// GetPullPath ( 31c4e7f0 ): the first data directory ( every folder of the game dir ) where the file opens, written back as the full path
	if ( g_pKTDXApp == NULL || g_pKTDXApp->GetDeviceManager() == NULL
		|| g_pKTDXApp->GetDeviceManager()->GetPullPath( wszPath ) == NULL || GetFileAttributesW( wszPath ) == INVALID_FILE_ATTRIBUTES )
	{
		JuneGfxSceneLog( "Play %s: scene file %s.swf not found in any data folder ( eventscene\\ )", JuneGfxSceneToUTF8( wstrName ).c_str(),
			( GetSwfFileName( wstrName, strSwf ) == true ) ? strSwf.c_str() : JuneGfxSceneToUTF8( wstrName ).c_str() );
		return;
	}

	CKTDGGFXManager* pManager = g_pKTDXApp->GetJuneGFXManager();
	if ( pManager == NULL || pManager->IsMovieLoaded() == false )
	{
		// June has no such test ( its movie always loads ); without one the scene would hide the UI and wait for a movie that is not there
		JuneGfxSceneLog( "Play %s: no root movie ( main.gfx did not load ): not played", JuneGfxSceneToUTF8( wstrName ).c_str() );
		return;
	}

	WCHAR wszBack[ 260 ] = L"Tutorial_Back.swf";
	if ( g_pKTDXApp->GetDeviceManager()->GetPullPath( wszBack ) != NULL && GetFileAttributesW( wszBack ) != INVALID_FILE_ATTRIBUTES )
		pManager->loadUserWidget( 0x2712, wszBack, 0, 0 );
	if ( iFrame == -1 )
		pManager->loadUserWidget( 0x2711, wszPath, 0, 0 );
	else
		pManager->loadEventSceneEndFrame( 0x2711, wszPath, iFrame );
	m_bPlaying = true;

	// @3226b530..3226b56b: June draws its software cursor in event-scene mode ( CKTDXCursor, R4f ) and turns off the CX2Cursor idle hide
	// ( 32868250 ); March has neither, its CX2Cursor draws the same DLG_Eventscene_Cursor.dds in XCS_EVENT_SCENE
	if ( g_pMain != NULL && g_pMain->GetNowState() != NULL )
	{
		CX2State* pState = static_cast< CX2State* >( g_pMain->GetNowState() );
		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_EVENT_SCENE )
			pCursor->ChangeCursorState( CX2Cursor::XCS_EVENT_SCENE );
	}

	pManager->SetEventScenePlaying( true );		// +0x115: the keys stop, the mouse is wheel-only
	m_fSkipTime		= 0.f;
	m_bSkipHeld		= false;
	SetSceneUIShow( false );
	CreateBlackLayOut();
	pManager->unloadWidget( 3 );

	JuneGfxSceneLog( "Play %s ( %s ): frame %d", JuneGfxSceneToUTF8( wstrName ).c_str(), JuneGfxSceneToUTF8( std::wstring( wszPath ) ).c_str(), iFrame );
}

// 3226b6c0
void CX2GFXEventScene::End( bool bForce )
{
	// the cursor back to normal ( 32868790( 0 ) )
	if ( g_pMain != NULL && g_pMain->GetNowState() != NULL )
	{
		CX2State* pState = static_cast< CX2State* >( g_pMain->GetNowState() );
		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL )
			pCursor->ChangeCursorState( CX2Cursor::XCS_NORMAL );
	}
	// not ported here: the quest manager's pending-scene reset ( 3232e000, quest manager +0x148 ) is the quest-system plan's; the software cursor
	// reset ( 31ce50d0 ) is R4f's

	CKTDGGFXManager* pManager = ( g_pKTDXApp != NULL ) ? g_pKTDXApp->GetJuneGFXManager() : NULL;
	if ( pManager != NULL )
	{
		pManager->SetEventScenePlaying( false );
		pManager->unloadUserWidget( 0x2712 );
		pManager->unloadUserWidget( 0x2711 );
	}
	m_bPlaying = false;
	SetSceneUIShow( true );
	DestroyBlackLayOut();

	// 3226ade0: the ment ids of the scene that ended go
	m_mapMent.erase( m_wstrName );

	if ( bForce == true )
	{
		// @3226b8e4..3226badb: the queue is dropped ( 327464a0 ) and the +0xc0 map cleared ( 3226ccb0 ). The popup block before it ( the tutorial skip
		// warning for Quest_11005 *Start, gated on IsEnableCode( 0x93a ) and quest 0x2afd, REQ 0x6c5 ) needs the server event code 0x93a, which
		// offline never gets, and packets March does not have: not ported ( see the Done note )
		m_dequeQueue.clear();
		m_mapMent.clear();
	}
	m_bSkipRequested = false;		// @3226bb95: read and cleared by that popup block only; cleared here so a request cannot outlive its scene

	if ( m_dequeQueue.empty() == false )
	{
		// @3226bd26: the next queued scene starts at once ( 32746440 pops it ), and End returns: no recent-name / sound tail
		const QueueEntry kNext = m_dequeQueue.front();
		m_dequeQueue.pop_front();
		Play( kNext.m_wstrName, kNext.m_iFrame, 0 );
		return;
	}

	// tail @3226be2a. Not ported: 31d87420 ( June's dungeon 41470 start hook ), 31d70b30 ( the tutorial guide manager, Stage 9 ), the Aisha-only
	// game value at @3226bf38 ( CX2Game +0xb38 = 2.8f when my unit type is 2 ) and 32396dc0 / 3239d810 ( CX2UIProfessionManager, Stage 9 )
	if ( m_bNotifyNewEpicQuest == true )
	{
		m_bNotifyNewEpicQuest = false;
		if ( g_pData != NULL && g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIQuestNew() != NULL )
			g_pData->GetUIManager()->GetUIQuestNew()->NewEpicStartEffect();		// 323ca640
	}

	m_wstrRecent = m_wstrName;			// @3226bf79: +0x94 = +0x78
	m_wstrName.clear();					// @3226bf8c

	if ( m_pSound != NULL )
		m_pSound->Stop();				// 31d416c0
	if ( m_pSound != NULL && g_pKTDXApp != NULL && g_pKTDXApp->GetDeviceManager() != NULL )
	{
		g_pKTDXApp->GetDeviceManager()->JuneCloseDeviceByPointer( m_pSound );
		m_pSound = NULL;
	}
	JuneGfxSceneLog( "End ( %s )", bForce ? "forced" : "by the scene" );
}

// 3226c120
void CX2GFXEventScene::OnFrameMove( double /*fTime*/, float fElapsedTime )
{
	if ( m_bPlaying == false )
		return;

	// the keys the scene swallows: the four arrows, F1, F8 and the fast attack ( 31c48870 x 5, 31d24fb0 x 2 )
	GET_KEY_STATE( GA_UP );
	GET_KEY_STATE( GA_DOWN );
	GET_KEY_STATE( GA_LEFT );
	GET_KEY_STATE( GA_RIGHT );
	g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F1 );
#ifndef NOT_APPLY_F8
	g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F8 );
#endif
	GET_KEY_STATE( GA_ATTACK_FAST );

	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_RETURN ) == TRUE )
	{
		if ( m_pBridge != NULL )
			m_pBridge->FireNextScene();		// 32e59800
	}
	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_ESCAPE ) == TRUE )
		m_bSkipHeld = true;

	if ( m_bSkipHeld == true )
	{
		m_fSkipTime += fElapsedTime;
		if ( m_fSkipTime > 0.6f )		// _DAT_3344930c
		{
			End( true );
			m_fSkipTime = 0.f;
			m_bSkipHeld = false;
		}
	}
}

// 32269e00: a click advances the scene, the right button is eaten and its release starts the skip timer
bool CX2GFXEventScene::MsgProc( HWND /*hWnd*/, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	if ( m_bPlaying == false )
		return false;
	switch ( uMsg )
	{
	case WM_LBUTTONUP:
		if ( m_pBridge != NULL )
			m_pBridge->FireNextScene();
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		m_bSkipHeld = true;
		return false;
	}
	return false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// IJuneGfxEventSceneHost
// ---------------------------------------------------------------------------------------------------------------------------------------
void CX2GFXEventScene::OnSceneAsEnd()
{
	End( false );
}

void CX2GFXEventScene::OnSceneAsPlaySound( const std::wstring& wstrKey )
{
	PlaySound( wstrKey );
}

// 3226a2b0: the ment table of the scene being played, by frame ( 1-based ); a miss is 0
int CX2GFXEventScene::GetMentID( int iFrame )
{
	std::map< std::wstring, std::vector< int > >::const_iterator it = m_mapMent.find( m_wstrName );
	if ( it == m_mapMent.end() )
		return 0;
	const int iIndex = iFrame - 1;
	if ( iIndex < 0 || iIndex >= (int)it->second.size() )
		return 0;
	return it->second[ iIndex ];
}

int CX2GFXEventScene::OnSceneAsGetMentID( int iFrame )
{
	return GetMentID( iFrame );
}

// 32e5aa50 ( the name at +0x78 ) + 32e5b030 ( the map row, a copy )
bool CX2GFXEventScene::OnSceneAsGetFrames( std::vector< KJuneGfxSceneFrame >& vecFrames )
{
	std::map< std::wstring, SceneInfo >::const_iterator it = m_mapSceneInfo.find( m_wstrName );
	if ( it == m_mapSceneInfo.end() )
	{
		JuneGfxSceneLog( "getEventSceneData: no row for %s", JuneGfxSceneToUTF8( m_wstrName ).c_str() );
		return false;
	}
	vecFrames = it->second.m_vecFrame;
	JuneGfxSceneLog( "getEventSceneData %s: %d frames", JuneGfxSceneToUTF8( m_wstrName ).c_str(), (int)vecFrames.size() );
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// Lua bindings
// ---------------------------------------------------------------------------------------------------------------------------------------
void JuneGfxPlayEventScene_LUA( const char* szName )		// 327ccad0
{
	if ( g_pData == NULL || g_pData->GetGFXEventScene() == NULL || szName == NULL )
		return;
	std::wstring wstrName;
	ConvertUtf8ToWCHAR( wstrName, szName );
	g_pData->GetGFXEventScene()->Play( wstrName, -1, 0 );
}

bool JuneGfxIsRecentlyPlayedEventScene_LUA( const char* szName )		// 327ccb20
{
	if ( g_pData == NULL || g_pData->GetGFXEventScene() == NULL || szName == NULL )
		return false;
	std::wstring wstrName;
	ConvertUtf8ToWCHAR( wstrName, szName );
	return g_pData->GetGFXEventScene()->IsRecentlyPlayed( wstrName );
}
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE

#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
// ---------------------------------------------------------------------------------------------------------------------------------------
// Phase G4: CX2Main's CX2SimpleWidgetPlayer ( g_pMain +0xd7c ) and the user skill cut-in widgets. Log tag [JUNE_GFX] CUTIN ( ErrorLog.txt ).
// ---------------------------------------------------------------------------------------------------------------------------------------
static void JuneGfxCutInLog( const char* szFormat, ... )
{
	char szBuf[ 1024 ] = { 0, };
	va_list args;
	va_start( args, szFormat );
	_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
	va_end( args );
	KLOG( "ErrorLog.txt" ) << "[JUNE_GFX] CUTIN " << szBuf << fileout;
}

// 31d70c60: made once; June passes the root movie's adapter ( g_pKTDXApp +0x1c8 -> +0x98 vslot 1 ) or 0 when there is no movie, and then
// 31fc75e0 returns 0 for good ( this +4 == 0 ), as CKTDGGFXSimpleWidgetPlayer does without an adapter
void CX2Main::JuneMakeSimpleWidgetPlayer()
{
	if ( NULL != m_pJuneSimpleWidgetPlayer || NULL == g_pKTDXApp )
		return;
	m_pJuneSimpleWidgetPlayer = new CKTDGGFXSimpleWidgetPlayer( g_pKTDXApp->GetJuneGFXManager() );
}

// 31d76b40( uiId, name, side, b ): make the player, then, when it exists: UI 0xc first plays the frame ( side, 1 ) and a random talk balloon
// CUT_IN_TALK_<R_|L_><rand() % 3 + 1> ( 0, 0x3c ) ( "L_" for a non-zero side ), then the cut-in itself ( side, b )
void CX2Main::JunePlayCutInWidget( int iUIID_, const std::string& strName_, bool bLeft_, int iParam_ )
{
	JuneMakeSimpleWidgetPlayer();
	if ( NULL == m_pJuneSimpleWidgetPlayer )
		return;

	const int iSide = ( true == bLeft_ ) ? 1 : 0;
	bool bPlayed = true;
	if ( 0xc == iUIID_ )
	{
		bPlayed = m_pJuneSimpleWidgetPlayer->PlaySimpleWidget( 0xc, "Common_CutIn_Frame.gfx", iSide, 1 ) && bPlayed;

		char szTalk[ 64 ] = { 0, };
		sprintf_s( szTalk, sizeof( szTalk ), "CUT_IN_TALK_%s%d.gfx", ( true == bLeft_ ) ? "L_" : "R_", rand() % 3 + 1 );
		bPlayed = m_pJuneSimpleWidgetPlayer->PlaySimpleWidget( 0xc, szTalk, 0, 0x3c ) && bPlayed;
	}
	bPlayed = m_pJuneSimpleWidgetPlayer->PlaySimpleWidget( iUIID_, strName_.c_str(), iSide, iParam_ ) && bPlayed;

	JuneGfxCutInLog( "play ui=0x%x %s side=%s%s", iUIID_, strName_.c_str(), ( true == bLeft_ ) ? "L" : "R",
		( true == bPlayed ) ? "" : " ( no widget binding: the root movie is not up )" );
}
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
