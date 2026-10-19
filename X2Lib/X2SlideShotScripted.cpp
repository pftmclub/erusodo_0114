#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
#include "X2SlideShotScripted.h"
#ifdef SERV_IRUHADEV_OFFLINE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_OFFLINE

// Log tag [JUNE_QUEST] STORY ( offline_server.log ): PLAY start / cut / skip / end / message.
//
// June's states here are 0xe ( village ), 0xf ( guild agit, none in March ) and 0x15 ( battle field ): March XS_VILLAGE_MAP / XS_BATTLE_FIELD.
// June's village game is g_pTFieldGame ( 33775870 ), its world +0xa8; the battle field's is g_pX2Game ( 3372c0fc ) -> GetWorld ( vslot 0x90 ).
// June's world +0x05 byte ( saved to +0x55 and zeroed by Start's village branch, zeroed by its battle-field branch, restored by End ) has no
// reader in June outside this class ( scan of every [ x + 0xa8 ] -> byte [ + 5 ] and the 24 mapped CX2World functions ) and no March member:
// not carried.

namespace
{
	void JuneStoryLog( const WCHAR* wszFormat_, ... )
	{
#ifdef SERV_IRUHADEV_OFFLINE
		WCHAR wszBuf[512] = L"";
		va_list vaList;
		va_start( vaList, wszFormat_ );
		StringCchVPrintf( wszBuf, 512, wszFormat_, vaList );
		va_end( vaList );
		CX2OfflineLog::Server( L"%s", wszBuf );
#endif SERV_IRUHADEV_OFFLINE
	}

	/// June 328c5530: sprintf_s( "%I64d" ) of the sign-extended index
	std::string JuneIndexString( int iIndex_ )
	{
		char szBuf[64] = "";
		sprintf_s( szBuf, 64, "%I64d", static_cast<__int64>( iIndex_ ) );
		return std::string( szBuf );
	}

	bool JuneIsStoryState( int iState_ )
	{
		return CX2Main::XS_VILLAGE_MAP == iState_ || CX2Main::XS_BATTLE_FIELD == iState_;
	}

	CX2Cursor* JuneNowStateCursor()
	{
		if( NULL == g_pMain || NULL == g_pMain->GetNowState() )
			return NULL;
		return static_cast< CX2State* >( g_pMain->GetNowState() )->GetCursor();
	}

	bool JuneMP3Ready()
	{
		return NULL != g_pKTDXApp->GetDSManager() && true == g_pKTDXApp->GetDSManager()->GetInit() && NULL != g_pKTDXApp->GetDSManager()->GetMP3Play();
	}
}

/// June 328c34b0
CX2SlideShotScripted::CX2SlideShotScripted()
: m_hBlackBack( INVALID_PARTICLE_SEQUENCE_HANDLE )
, m_bTextShown( false )
, m_bRunning( false )
, m_bShowDialog( false )
, m_bCursorChanged( false )
, m_bCameraMoved( false )
, m_bEnding( false )
, m_bStarting( false )
, m_uiCutIndex( 0 )
, m_uiBGMIndex( 0 )
, m_uiSoundIndex( 0 )
, m_uiSceneIndex( 0 )
, m_uiBackgroundIndex( 0 )
, m_uiSceneDeleteIndex( 0 )
, m_iQuestID( 0 )
, m_fCutTime( 0.f )
, m_fStartFadeTime( 0.f )
, m_fEndFadeTime( 0.f )
, m_fTextEndTime( 0.f )
, m_pDLGTalkBox( NULL )
, m_vSavedEye( 0.f, 0.f, 0.f )
, m_vSavedLookAt( 0.f, 0.f, 0.f )
, m_pStory( NULL )
{
	// 328c35b3..328c3709: no owner stage, fade 0.07 ( 3344b78c ); both black bars hidden, renamed ( the dialog manager's hide mode 5 keeps
	// DLG_SlideShotScripted_TalkBox drawing ), layer 0x1f9, hidden and disabled; the Speech string uses font 1
	m_pDLGTalkBox = new CKTDGUIDialog( NULL, L"DLG_NewSlideShot.lua", 0.07f, 0 );

	CKTDGUIControl* pBlackLayOutDown = m_pDLGTalkBox->GetControl( L"BlackLayOutDown" );
	CKTDGUIControl* pBlackLayOutUp = m_pDLGTalkBox->GetControl( L"BlackLayOutUp" );
	if( NULL != pBlackLayOutDown )
		pBlackLayOutDown->SetShowEnable( false, false );
	if( NULL != pBlackLayOutUp )
		pBlackLayOutUp->SetShowEnable( false, false );

	m_pDLGTalkBox->SetName( L"DLG_SlideShotScripted_TalkBox" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGTalkBox );
	m_pDLGTalkBox->SetLayer( 0x1f9 );
	m_pDLGTalkBox->SetShow( false );
	m_pDLGTalkBox->SetEnable( false );

	CKTDGUIStatic* pStaticSpeech = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Speech" ) );
	if( NULL != pStaticSpeech && NULL != pStaticSpeech->GetString( 0 ) )
		pStaticSpeech->GetString( 0 )->fontIndex = 1;
}

/// June 328c3290: the particles of the map go, the background ( +0x30 ) does not; nothing is restored
CX2SlideShotScripted::~CX2SlideShotScripted()
{
	DestroyAllParticles();
	m_bShowDialog = false;
	m_pStory = NULL;
	SAFE_DELETE_DIALOG( m_pDLGTalkBox );
}

/// June 328c3750
bool CX2SlideShotScripted::Start( int iQuestID_ )
{
	const int iState = g_pMain->GetNowStateID();
	if( false == JuneIsStoryState( iState ) )
		return false;

	if( true == m_bRunning )
		return true;		// @328c37ae: already running counts as started ( JUNE_BUGS 360 )

	m_pStory = CX2SlideShotScriptedManager::GetInstance()->GetStory( iQuestID_ );
	if( NULL == m_pStory )
		return false;

	// @328c37cb..328c383d: the hard-coded black background, over the UI, layer 100 ( June's 14th CreateSequence argument )
	CKTDGParticleSystem::CParticleEventSequence* pBlackBack = g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"PresentProlBlackBack", 0.f, 0.f, 0.f,
		-1.f, -1.f, -1, -1, 1.0f, false, 0.f, false );
	if( NULL != pBlackBack )
	{
		pBlackBack->SetLayer( 100 );
		pBlackBack->SetOverUI( true );
		m_hBlackBack = pBlackBack->GetHandle();
	}
	m_bCameraMoved = false;

	// @328c3843..328c3940: the village hides its world objects and stops the world's effect sounds; the battle field only stops the sounds
	if( NULL != g_pTFieldGame && CX2Main::XS_VILLAGE_MAP == iState )
	{
		CX2World* pWorld = g_pTFieldGame->GetWorld();
		if( NULL != pWorld )
		{
			pWorld->SetShowObject( false );
			pWorld->StopAllEffectBGM();
		}
	}
	else if( NULL != g_pX2Game && CX2Main::XS_BATTLE_FIELD == iState && NULL != g_pX2Game->GetWorld() )
	{
		g_pX2Game->GetWorld()->StopAllEffectBGM();
	}

	m_fCutTime = 0.f;
	m_fTextEndTime = 0.f;
	m_bEnding = false;
	m_bStarting = true;
	m_uiCutIndex = 0;
	m_uiBackgroundIndex = 0;
	m_uiSoundIndex = 0;
	m_uiBGMIndex = 0;
	m_uiSceneIndex = 0;
	m_bRunning = true;
	m_bShowDialog = true;
	m_uiSceneDeleteIndex = 0;
	m_iQuestID = iQuestID_;

	if( NULL != m_pDLGTalkBox )
	{
		CKTDGUIStatic* pStaticEnter = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Static_Enter" ) );
		if( NULL != pStaticEnter )
		{
			for( int i = 0; NULL != pStaticEnter->GetPicture( i ); ++i )
				pStaticEnter->GetPicture( i )->bShow = false;
		}
	}

	// @328c39c0..328c3a0e: the BGM playing now is kept for End, then stopped
	if( true == JuneMP3Ready() )
	{
		m_wstrSavedBGM = g_pKTDXApp->GetDSManager()->GetMP3Play()->GetPlayingFileName();
		g_pKTDXApp->GetDSManager()->GetMP3Play()->Stop();
	}

	// @328c3a18: every dialog hides except the hide-mode-5 list; @328c3a2c: the event-scene cursor ( June 0xf, as the old slide show 325c6791 )
	g_pKTDXApp->GetDGManager()->GetDialogManager()->JuneSetShow( false, 5 );
	CX2Cursor* pCursor = JuneNowStateCursor();
	if( NULL != pCursor )
		pCursor->ChangeCursorState( CX2Cursor::XCS_EVENT_SCENE );
	m_bCursorChanged = true;

	if( NULL != m_pDLGTalkBox )
	{
		m_pDLGTalkBox->SetShow( m_bShowDialog );
		m_pDLGTalkBox->SetEnable( m_bShowDialog );
	}

	JuneStoryLog( L"[JUNE_QUEST] STORY PLAY start: quest %d, %d cut scene(s), state %d, saved BGM '%s'", iQuestID_, static_cast<int>( m_pStory->size() ),
		iState, m_wstrSavedBGM.c_str() );
	return true;
}

/// June 328c41f0
void CX2SlideShotScripted::OnFrameMove( float fElapsedTime_ )
{
	if( false == m_bRunning )
		return;

	m_fCutTime += fElapsedTime_;
	const int iState = g_pMain->GetNowStateID();

	// the end fade: End after 0.1 s ( 334395e8 )
	if( true == m_bEnding && false == m_bStarting )
	{
		m_fEndFadeTime += fElapsedTime_;
		if( m_fEndFadeTime > 0.1 )
			End();
		return;
	}

	// the start fade: after 0.1 s the camera is kept and moved far away ( village: camera process off )
	if( true == m_bStarting && false == m_bEnding )
	{
		m_fStartFadeTime += fElapsedTime_;
		if( m_fStartFadeTime > 0.1 )
		{
			if( false == m_bCameraMoved )
			{
				CKTDGCamera& kCamera = g_pKTDXApp->GetDGManager()->GetCamera();
				m_vSavedEye = kCamera.GetEye();
				m_vSavedLookAt = kCamera.GetLookAt();

				if( NULL != g_pTFieldGame && CX2Main::XS_VILLAGE_MAP == iState )
				{
					if( NULL != g_pTFieldGame->GetWorld() )
					{
						// @328c436c..328c43c6: eye ( 0, -5000, -1300 ), at ( 0, -5000, 0 ) ( 3349d870 / 33443950 )
						kCamera.PointAndUpdateViewTransform( 0.f, -5000.f, -1300.f, 0.f, -5000.f, 0.f, 0.f, 1.f, 0.f, 0.f );
						g_pTFieldGame->SetEnableCameraProcess( false );
						m_bCameraMoved = true;
						m_bStarting = false;
						return;
					}
				}
				else if( NULL != g_pX2Game && CX2Main::XS_BATTLE_FIELD == iState && NULL != g_pX2Game->GetWorld() )
				{
					kCamera.PointAndUpdateViewTransform( 0.f, -5000.f, -1300.f, 0.f, -5000.f, 0.f, 0.f, 1.f, 0.f, 0.f );
				}
				m_bCameraMoved = true;
			}
			m_bStarting = false;
		}
		return;
	}

	if( NULL == m_pStory )
		return;

	CKTDGUIStatic* pStaticEnter = NULL;
	CKTDGUIControl::CPictureData* pEnterPicture = NULL;
	CKTDGUIStatic* pStaticSpeech = NULL;
	if( NULL != m_pDLGTalkBox )
	{
		pStaticEnter = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Static_Enter" ) );
		if( NULL != pStaticEnter )
			pEnterPicture = pStaticEnter->GetPicture( 0 );
		pStaticSpeech = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Speech" ) );
	}

	// past the last cut, or ESC / right click: the end fade ( 328c544b )
	if( m_uiCutIndex >= m_pStory->size() ||
		TRUE == g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_ESCAPE ) ||
		TRUE == g_pKTDXApp->GetDIManager()->GetMouse()->GetButtonState( MOUSE_RBUTTON ) )
	{
		if( m_uiCutIndex < m_pStory->size() )
			JuneStoryLog( L"[JUNE_QUEST] STORY PLAY skip: quest %d at cut %d", m_iQuestID, static_cast<int>( m_uiCutIndex ) );
		m_bEnding = true;
		return;
	}

	// Enter / left click with the text up: finish the spread, else the next cut once Static_Enter shows ( no picture: after 1.5 s, 33471a1c )
	if( true == m_bTextShown &&
		( TRUE == g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_RETURN ) ||
		  TRUE == g_pKTDXApp->GetDIManager()->GetMouse()->GetButtonState( MOUSE_LBUTTON ) ) )
	{
		if( true == IsTextSpreading() )
		{
			FinishTextSpread();
		}
		else if( NULL == pEnterPicture )
		{
			if( 1.5f <= m_fCutTime )
			{
				NextCutScene();
				return;
			}
		}
		else if( true == pEnterPicture->bShow )
		{
			NextCutScene();
			return;
		}
	}

	if( false == m_bRunning || NULL == m_pStory )
		return;

	const CutScene& kCut = GetCut( m_uiCutIndex );
	const float fTime = m_fCutTime;

	// Scene_Delete: every key that is due ( a loop, the lists below take one row a frame )
	while( m_uiSceneDeleteIndex < kCut.m_vecSceneDelete.size() && fTime >= kCut.m_vecSceneDelete[ m_uiSceneDeleteIndex ].m_fTime )
	{
		DeleteParticle( kCut.m_vecSceneDelete[ m_uiSceneDeleteIndex ].m_strKey );
		++m_uiSceneDeleteIndex;
	}

	// Background_List: key "<cut>_<row>BG", layer 100
	if( m_uiBackgroundIndex < kCut.m_vecBackground.size() && kCut.m_vecBackground[ m_uiBackgroundIndex ].m_fTime <= fTime )
	{
		const std::string strKey = JuneIndexString( static_cast<int>( m_uiCutIndex ) ) + "_" + JuneIndexString( static_cast<int>( m_uiBackgroundIndex ) ) + "BG";
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,
			kCut.m_vecBackground[ m_uiBackgroundIndex ].m_wstrName.c_str(), 0.f, 0.f, 0.f, -1.f, -1.f, -1, -1, 1.0f, false, 0.f, false );
		if( NULL != pSeq )
		{
			pSeq->SetLayer( 100 );
			pSeq->SetOverUI( true );
			AddParticle( strKey, pSeq );
		}
		++m_uiBackgroundIndex;
	}

	// Scene_BGM_List
	if( m_uiBGMIndex < kCut.m_vecBGM.size() && kCut.m_vecBGM[ m_uiBGMIndex ].m_fTime <= fTime )
	{
		PlayBGM( kCut.m_vecBGM[ m_uiBGMIndex ].m_wstrName.c_str() );
		++m_uiBGMIndex;
	}

	// Sound_List ( Stop_All is not read here )
	if( m_uiSoundIndex < kCut.m_vecSound.size() && kCut.m_vecSound[ m_uiSoundIndex ].m_fTime <= fTime )
	{
		PlaySound( kCut.m_vecSound[ m_uiSoundIndex ].m_wstrName.c_str() );
		++m_uiSoundIndex;
	}

	// Scene_List: key "<cut>_<row>", next-frame start, the templet's layer
	if( m_uiSceneIndex < kCut.m_vecScene.size() && kCut.m_vecScene[ m_uiSceneIndex ].m_fTime <= fTime )
	{
		const std::string strKey = JuneIndexString( static_cast<int>( m_uiCutIndex ) ) + "_" + JuneIndexString( static_cast<int>( m_uiSceneIndex ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,
			kCut.m_vecScene[ m_uiSceneIndex ].m_wstrName.c_str(), 0.f, 0.f, 0.f, -1.f, -1.f, -1, -1, 1.0f, false, 0.f, true );
		if( NULL != pSeq )
		{
			pSeq->SetOverUI( true );
			AddParticle( strKey, pSeq );
		}
		++m_uiSceneIndex;
	}

	// Scene_Text: once its start time is due; spread gap = Spread_Rate / the text's length
	if( false == m_bTextShown )
	{
		std::wstring wstrName = GET_STRING( kCut.m_iSpeakerStringID );
		std::wstring wstrText = GET_STRING( kCut.m_iTextStringID );
		if( -1 == kCut.m_iTextStringID )
		{
			m_bTextShown = true;
		}
		else
		{
			if( -1 == kCut.m_iSpeakerStringID )
				wstrName = L"";

			if( kCut.m_fTextStartTime <= fTime )
			{
				ClearText();
				const float fSpreadTime = kCut.m_fSpreadRate / static_cast<float>( wstrText.length() );
				AddText( kCut.m_bNameLeft, wstrName.c_str(), wstrText.c_str(), kCut.m_wstrColor, true, fSpreadTime );
				m_bTextShown = true;
				if( NULL != m_pDLGTalkBox )
				{
					m_pDLGTalkBox->SetShow( true );
					m_pDLGTalkBox->SetEnable( true );
				}
			}
		}
	}

	// the time the text finished spreading
	if( NULL != pStaticSpeech && 0.f == m_fTextEndTime && NULL != pStaticSpeech->GetString( 0 ) && false == pStaticSpeech->GetString( 0 )->bSpread )
		m_fTextEndTime = m_fCutTime;

	// the next cut at max( Scene_Duration, text end + 8.0 s ) ( 33439cb0 ), Static_Enter 1.0 s after the text ( 334355b0 ); not while the
	// duration runs and a background is still due
	const float fDuration = GetCut( m_uiCutIndex ).m_fSceneDuration;
	if( fDuration > m_fCutTime && m_uiBackgroundIndex < GetCut( m_uiCutIndex ).m_vecBackground.size() )
		return;
	if( NULL == pStaticSpeech || NULL == pStaticSpeech->GetString( 0 ) || true == pStaticSpeech->GetString( 0 )->bSpread || 0.f == m_fTextEndTime )
		return;

	float fAdvanceTime = static_cast<float>( m_fTextEndTime + 8.0 );
	if( fDuration >= fAdvanceTime )
		fAdvanceTime = fDuration;
	if( fAdvanceTime <= m_fCutTime )
	{
		NextCutScene();
		return;
	}

	if( m_fCutTime - m_fTextEndTime >= 1.0 && NULL != pStaticEnter && NULL != pEnterPicture && false == pEnterPicture->bShow )
		pEnterPicture->bShow = true;
}

/// June 328c3210
void CX2SlideShotScripted::NextCutScene()
{
	++m_uiCutIndex;
	m_fCutTime = 0.f;
	m_fTextEndTime = 0.f;
	m_uiSceneIndex = 0;
	m_uiBackgroundIndex = 0;
	m_uiSoundIndex = 0;
	m_uiBGMIndex = 0;
	m_uiSceneDeleteIndex = 0;
	m_bTextShown = false;

	if( NULL != m_pDLGTalkBox )
	{
		CKTDGUIStatic* pStaticEnter = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Static_Enter" ) );
		if( NULL != pStaticEnter )
		{
			for( int i = 0; NULL != pStaticEnter->GetPicture( i ); ++i )
				pStaticEnter->GetPicture( i )->bShow = false;
		}
	}
	StopSounds();

	JuneStoryLog( L"[JUNE_QUEST] STORY PLAY cut %d / %d ( quest %d )", static_cast<int>( m_uiCutIndex ), NULL != m_pStory ? static_cast<int>( m_pStory->size() ) : 0, m_iQuestID );
}

/// June 328c3ae0
void CX2SlideShotScripted::End()
{
	if( false == m_bRunning )
		return;

	m_bTextShown = false;
	m_bRunning = false;
	m_fCutTime = 0.f;
	m_uiCutIndex = 0;
	m_fEndFadeTime = 0.f;
	m_bShowDialog = false;
	m_fStartFadeTime = 0.f;
	m_bEnding = false;

	// @328c3b59..328c3c01: the camera kept by the start fade ( even when the fade never ran: then it is the ctor's / last run's ), the projection
	CKTDGCamera& kCamera = g_pKTDXApp->GetDGManager()->GetCamera();
	kCamera.Move( m_vSavedEye.x, m_vSavedEye.y, m_vSavedEye.z );
	kCamera.LookAt( m_vSavedLookAt.x, m_vSavedLookAt.y, m_vSavedLookAt.z );
	g_pKTDXApp->GetDGManager()->SetProjection( g_pKTDXApp->GetDGManager()->GetNear(), g_pKTDXApp->GetDGManager()->GetFar(), true, 1.f );

	g_pKTDXApp->GetDGManager()->GetDialogManager()->JuneSetShow( true, 0 );
	DestroyAllParticles();
	if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hBlackBack )
	{
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hBlackBack );
		m_hBlackBack = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
	ClearText();

	// @328c3c4b..328c3d3c: the world shows and its effect sounds play again ( both on the battle field too ); the village camera runs again
	const int iState = g_pMain->GetNowStateID();
	if( CX2Main::XS_VILLAGE_MAP == iState )
	{
		if( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetWorld() )
		{
			g_pTFieldGame->GetWorld()->SetShowObject( true );
			g_pTFieldGame->GetWorld()->PlayAllEffectBGM();
			g_pTFieldGame->SetEnableCameraProcess( true );
		}
	}
	else if( CX2Main::XS_BATTLE_FIELD == iState )
	{
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetWorld() )
		{
			g_pX2Game->GetWorld()->SetShowObject( true );
			g_pX2Game->GetWorld()->PlayAllEffectBGM();
		}
	}

	StopSounds();
	if( true == m_bCursorChanged )
	{
		CX2Cursor* pCursor = JuneNowStateCursor();
		if( NULL != pCursor )
			pCursor->ChangeCursorState( CX2Cursor::XCS_NORMAL );
		m_bCursorChanged = false;
	}

	if( NULL != m_pDLGTalkBox )
	{
		m_pDLGTalkBox->SetShow( m_bShowDialog );
		m_pDLGTalkBox->SetEnable( m_bShowDialog );
	}
	m_pStory = NULL;

	// @328c3dce..328c3df9: the BGM kept by Start ( an empty name when none played: March's Play finds no file and returns )
	if( true == JuneMP3Ready() )
		g_pKTDXApp->GetDSManager()->GetMP3Play()->Play( m_wstrSavedBGM.c_str(), true );

	JuneStoryLog( L"[JUNE_QUEST] STORY PLAY end: quest %d, end message %d", m_iQuestID, true == g_pInstanceData->GetJuneStoryEndMessage() ? 1 : 0 );

	// @328c3dfe..328c41b5: after a completion ( g_pInstanceData +0xabc ): "<the story's MediaFile Title> Story has been added to Media Gallery."
	// ( STR 40048 "@2 Story has been added to @1." with "SL" = STR 40147, the title ), OK box at ( 250, 300 ) ( 334577b8 / 3344f348 ); then cleared
	if( true == g_pInstanceData->GetJuneStoryEndMessage() )
	{
		CX2SlideShotScriptedManager* pManager = CX2SlideShotScriptedManager::GetInstance();
		if( NULL != pManager )
		{
			std::wstring wstrTitle = L"";
			const std::vector< CX2SlideShotScriptedManager::MediaInfo >& vecMediaInfo = pManager->GetMediaInfoList();
			for( UINT i = 0; i < vecMediaInfo.size(); ++i )
			{
				std::map< int, CX2SlideShotScriptedManager::MediaFile >::const_iterator mit = vecMediaInfo[i].m_mapMediaFile.find( m_iQuestID );
				if( mit != vecMediaInfo[i].m_mapMediaFile.end() )
				{
					wstrTitle = GET_STRING( mit->second.m_iTitleStringID );
					break;
				}
			}
			const std::wstring wstrGallery = GET_STRING( 40147 );
			const std::wstring wstrMessage = GET_REPLACED_STRING( ( 40048, "SL", wstrGallery.c_str(), wstrTitle ) );
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250.f, 300.f ), wstrMessage.c_str(), g_pMain->GetNowState() );
			JuneStoryLog( L"[JUNE_QUEST] STORY PLAY message: %s", wstrMessage.c_str() );
		}
		g_pInstanceData->SetJuneStoryEndMessage( false );
	}
}

/// June 328c2740
void CX2SlideShotScripted::ClearText()
{
	if( NULL == m_pDLGTalkBox )
		return;

	CKTDGUIStatic* pStaticName = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Name" ) );
	CKTDGUIStatic* pStaticSpeech = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Speech" ) );
	if( NULL != pStaticName && NULL != pStaticName->GetString( 0 ) )
		pStaticName->GetString( 0 )->msg = L"";
	if( NULL != pStaticSpeech && NULL != pStaticSpeech->GetString( 0 ) )
		pStaticSpeech->GetString( 0 )->msg = L"";
}

/// June 328c27d0: March's CX2SlideShot::AddText ( CLIENT_GLOBAL_LINEBREAK ) with June's width ( resolution scale x * 675.0, 334c21e0 ), the text's
/// own line breaks removed first, no -1 check on the wrap, and the name / speech set only when the Name static exists
void CX2SlideShotScripted::AddText( bool bNameLeft_, const WCHAR* wszName_, const WCHAR* wszMsg_, std::wstring wstrColor_, bool bSpread_, float fSpreadTime_ )
{
	if( NULL == m_pDLGTalkBox )
		return;

	const int iLineWidth = static_cast<int>( static_cast<float>( g_pKTDXApp->GetResolutionScaleX() * 675.0 ) );
	std::wstring wstrSpeech = wszMsg_;

	CKTDGUIStatic* pStaticSpeech = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Speech" ) );
	if( NULL == pStaticSpeech || NULL == pStaticSpeech->GetString( 0 ) )
		return;

	std::wstring::size_type uiOut = 0;
	for( std::wstring::size_type uiIn = 0; uiIn < wstrSpeech.length(); ++uiIn )
	{
		if( L'\n' != wstrSpeech[uiIn] && L'\r' != wstrSpeech[uiIn] )
			wstrSpeech[uiOut++] = wstrSpeech[uiIn];
	}
	wstrSpeech.resize( uiOut );

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStaticSpeech->GetString( 0 )->fontIndex );
	if( NULL == pFont )
		return;

	CWordLineHandler::LineBreakInX2Main( wstrSpeech, pFont, iLineWidth, wstrColor_, true, false );

	CKTDGUIStatic* pStaticName = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Name" ) );
	if( NULL == pStaticName || NULL == pStaticName->GetString( 0 ) )
		return;

	pStaticName->GetString( 0 )->msg = wszName_;
	pStaticSpeech->GetString( 0 )->msg = wstrSpeech;
	if( true == bSpread_ )
	{
		pStaticSpeech->GetString( 0 )->bSpread = true;
		pStaticSpeech->GetString( 0 )->fNowSpreadTime = 0.f;
		pStaticSpeech->GetString( 0 )->fSpreadTimeGap = fSpreadTime_;
		pStaticSpeech->GetString( 0 )->iSpreadCount = 0;
	}

	CKTDGUIStatic* pStaticNameTab = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"NameTab" ) );
	if( NULL != pStaticNameTab )
	{
		const D3DXVECTOR3 vPos = m_pDLGTalkBox->GetDummyPos( true == bNameLeft_ ? 0 : 1 );
		pStaticNameTab->SetOffsetPos( D3DXVECTOR2( vPos.x, vPos.y ) );
		pStaticName->SetOffsetPos( D3DXVECTOR2( vPos.x, vPos.y ) );
	}
}

/// June 328c26b0
bool CX2SlideShotScripted::IsTextSpreading()
{
	if( NULL == m_pDLGTalkBox )
		return false;
	CKTDGUIStatic* pStaticSpeech = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Speech" ) );
	if( NULL == pStaticSpeech || NULL == pStaticSpeech->GetString( 0 ) )
		return false;
	return pStaticSpeech->GetString( 0 )->bSpread;
}

/// June 328c2660
void CX2SlideShotScripted::FinishTextSpread()
{
	if( NULL == m_pDLGTalkBox )
		return;
	CKTDGUIStatic* pStaticSpeech = static_cast< CKTDGUIStatic* >( m_pDLGTalkBox->GetControl( L"Speech" ) );
	if( NULL != pStaticSpeech && NULL != pStaticSpeech->GetString( 0 ) )
		pStaticSpeech->GetString( 0 )->bSpread = false;
}

/// June 328c2700: stop, then play looped
void CX2SlideShotScripted::PlayBGM( const WCHAR* wszName_ )
{
	if( false == JuneMP3Ready() )
		return;
	g_pKTDXApp->GetDSManager()->GetMP3Play()->Stop();
	g_pKTDXApp->GetDSManager()->GetMP3Play()->Play( wszName_, true );
	JuneStoryLog( L"[JUNE_QUEST] STORY PLAY BGM '%s'", wszName_ );
}

/// June 328c3400: 2D, not looped ( June's 4th argument 1 = auto-release ); kept for NextCutScene's stop when it played
void CX2SlideShotScripted::PlaySound( const WCHAR* wszName_ )
{
	if( NULL == g_pKTDXApp || NULL == g_pKTDXApp->GetDeviceManager() )
		return;
	if( NULL != g_pKTDXApp->GetDeviceManager()->PlaySound( wszName_, false, false ) )
		m_vecPlayingSound.push_back( std::wstring( wszName_ ) );
}

/// June 328c2e90
void CX2SlideShotScripted::StopSounds()
{
	if( true == m_vecPlayingSound.empty() || NULL == g_pKTDXApp || NULL == g_pKTDXApp->GetDeviceManager() )
		return;
	for( UINT i = 0; i < m_vecPlayingSound.size(); ++i )
		g_pKTDXApp->GetDeviceManager()->StopSound( m_vecPlayingSound[i].c_str() );
	m_vecPlayingSound.clear();
}

/// June 328c5800 / 328c5660 -> 327c5f70: std::map insert, a key already there keeps its particle ( the new one is not tracked )
void CX2SlideShotScripted::AddParticle( const std::string& strKey_, CKTDGParticleSystem::CParticleEventSequence* pSeq_ )
{
	if( NULL == pSeq_ )
		return;
	m_mapParticle.insert( std::make_pair( strKey_, pSeq_->GetHandle() ) );
}

/// June 328c2d20
void CX2SlideShotScripted::DeleteParticle( const std::string& strKey_ )
{
	std::map< std::string, CKTDGParticleSystem::CParticleEventSequenceHandle >::iterator mit = m_mapParticle.find( strKey_ );
	if( mit == m_mapParticle.end() )
		return;
	g_pData->GetUIMajorParticle()->DestroyInstanceHandle( mit->second );
	m_mapParticle.erase( mit );
}

/// June 328c2df0
void CX2SlideShotScripted::DestroyAllParticles()
{
	std::map< std::string, CKTDGParticleSystem::CParticleEventSequenceHandle >::iterator mit;
	for( mit = m_mapParticle.begin(); mit != m_mapParticle.end(); ++mit )
	{
		CKTDGParticleSystem::CParticleEventSequenceHandle hSeq = mit->second;
		if( NULL != g_pData && NULL != g_pData->GetUIMajorParticle() )
			g_pData->GetUIMajorParticle()->DestroyInstanceHandle( hSeq );
	}
	m_mapParticle.clear();
}

#endif SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
