#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_RANDOM_MISSION
#include "X2RandomMission.h"
#include "WordWrap.h"
#ifdef SERV_IRUHADEV_OFFLINE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_OFFLINE

// Log tag [JUNE_QUEST] RM ( offline_server.log ). Map: june2017_re/quest_plan/rm0_map.tsv; every function names its June address.

namespace
{
	/// June's string ids for the mission UI ( General.ess )
	const int RM_STR_WITHIN_SECONDS		= 30700;	/// 0x77ec
	const int RM_STR_WITHIN_MINUTES		= 30721;	/// 0x7801
	const int RM_STR_WITHIN_MIN_SEC		= 30724;	/// 0x7804
	const int RM_STR_REWARD_EXP			= 30705;	/// 0x77f1
	const int RM_STR_REWARD_BUFF		= 30706;	/// 0x77f2
	const int RM_STR_REWARD_TIP			= 30745;	/// 0x7819
	const int RM_STR_MISSION_SUCCESS	= 30746;	/// 0x781a
	const int RM_STR_SKILL_ACTIVE		= 2673;		/// 0xa71
	const int RM_STR_SKILL_SPECIAL		= 2672;		/// 0xa70
	const int RM_STR_SKILL_HYPER		= 26134;	/// 0x6616

	const int RM_TEXT_WIDTH				= 135;		/// 0x87, the width every June line break uses
	const int RM_TEXT_FONT				= 0;
	const int RM_TEXT_LINES				= 2;
	const float RM_POPUP_FADE_START		= 3.0f;		/// 33436700
	const float RM_POPUP_TIMER			= 5.0f;		/// 33443804
	const float RM_POPUP_FADE_TIME		= 0.5f;		/// 3343777c
	const float RM_POPUP_FADE_IN_END	= 2.5f;		/// 334b0460 ( a double )
	const float RM_POPUP_FADE_OUT_START	= 0.5f;		/// 3342ebc8 ( a double )
	const float RM_DIALOG_Z				= 0.07f;	/// 3344b78c
	const float RM_WAIT_COMPLETE_ACK	= 10.0f;	/// 3344a51c

	void DialogFade( CKTDGUIDialogHandle& hDlg_, float fAlpha_ )
	{
		/// 31cd35a0( x, y, 1, 1, 1, alpha, 0.5, direct = 1, 0, 0, 1, 1 ): the dialog's own position, the colour ( 1, 1, 1, alpha )
		/// over 0.5 s, direct, no auto delete, no ping-pong, one repeat, fade in/out
		hDlg_->Move( hDlg_->GetPos(), D3DXCOLOR( 1.f, 1.f, 1.f, fAlpha_ ), RM_POPUP_FADE_TIME, true, false, false, 1
#ifdef MOVE_FADE_IN_OUT
			, true
#endif MOVE_FADE_IN_OUT
			);
	}
}

//////////////////////////////////////////////////////////////////////////
// CX2RandomMission
//////////////////////////////////////////////////////////////////////////

/// 32679bf0 ( the ctor ) + 326798b0 ( SetMission's field copy ): id +4, type +0xc, goal +0x14, seconds +0x28 / +0x2c, description
/// +0x18, and the two reward words. The packet's +0x3c lands on +0x1c, its +0x38 on +0x20, the counter starts at 0.
CX2RandomMission::CX2RandomMission( const KEGS_JUNE_RANDOM_MISSION_NOT& kNot_ )
	: m_iMissionID( kNot_.m_iMissionID )
	, m_iResult( MR_NONE )
	, m_iEventType( kNot_.m_iEventType )
	, m_iCounter( 0 )
	, m_iGoal( kNot_.m_iGoal )
	, m_iDescStrID( kNot_.m_iDescStrID )
	, m_iRewardBuffID( kNot_.m_iRewardBuffID )
	, m_iRewardExpRate( kNot_.m_iRewardExpRate )
	, m_fTimeAccum( -1.f )
	, m_iRemainSec( kNot_.m_iSeconds )
	, m_iTotalSec( kNot_.m_iSeconds )
	, m_fFadeTimer( -1.f )
{
}

/// 32679c50
CX2RandomMission::~CX2RandomMission()
{
	if( m_hDlgSmall != NULL )
	{
		SAFE_DELETE_DIALOG( m_hDlgSmall );
	}
	if( m_hDlgPopup != NULL )
	{
		SAFE_DELETE_DIALOG( m_hDlgPopup );
	}
}

/// 32677020: "Within [N seconds] " when there is a time limit, then the description with ( counter, goal ), the three skill types
/// with their type name first.
std::wstring CX2RandomMission::BuildText() const
{
	std::wstringstream wstm;

	if( m_iTotalSec > 0 )
	{
		const int iMin = m_iTotalSec / 60;
		const int iSec = m_iTotalSec % 60;
		if( iMin < 1 )
		{
			if( iSec >= 1 )
				wstm << GET_REPLACED_STRING( ( RM_STR_WITHIN_SECONDS, "i", iSec ) ) << L" ";
		}
		else if( iSec < 1 )
			wstm << GET_REPLACED_STRING( ( RM_STR_WITHIN_MINUTES, "i", iMin ) ) << L" ";
		else
			wstm << GET_REPLACED_STRING( ( RM_STR_WITHIN_MIN_SEC, "ii", iMin, iSec ) ) << L" ";
	}

	switch( m_iEventType )
	{
	case RME_ACTIVE_SKILL:
		wstm << GET_REPLACED_STRING( ( m_iDescStrID, "Sii", GET_STRING( RM_STR_SKILL_ACTIVE ), m_iCounter, m_iGoal ) );
		break;
	case RME_SPECIAL_SKILL:
		wstm << GET_REPLACED_STRING( ( m_iDescStrID, "Sii", GET_STRING( RM_STR_SKILL_SPECIAL ), m_iCounter, m_iGoal ) );
		break;
	case RME_HYPER_SKILL:
		wstm << GET_REPLACED_STRING( ( m_iDescStrID, "Sii", GET_STRING( RM_STR_SKILL_HYPER ), m_iCounter, m_iGoal ) );
		break;
	default:
		wstm << GET_REPLACED_STRING( ( m_iDescStrID, "ii", m_iCounter, m_iGoal ) );
		break;
	}

	return wstm.str();
}

/// 326768b0 ( RM1 review ): 32677020's text, every type then broken at the panel's width by 31d79390( text, 0x87, 0 ) @32676b16,
/// the helper 326784b0 also uses. The popup's line 0 ( 32678910 @32679111, 326791c0 @326796da ).
std::wstring CX2RandomMission::BuildPopupText() const
{
	const std::wstring wstrText = BuildText();
	return CWordLineHandler::GetStrByLineBreakInX2Main( wstrText.c_str(), RM_TEXT_WIDTH, RM_TEXT_FONT );
}

/// 32677740: the same text with the red colour code #C600000 swapped for #CED3131, then one newline ( the tooltip's first part )
std::wstring CX2RandomMission::BuildTooltipPrefix() const
{
	std::wstring wstrText = BuildText();
	const std::wstring wstrOld( L"#C600000" );
	const std::wstring wstrNew( L"#CED3131" );
	size_t iPos = 0;
	while( ( iPos = wstrText.find( wstrOld, iPos ) ) != std::wstring::npos )
	{
		wstrText.replace( iPos, wstrOld.size(), wstrNew );
		iPos += wstrNew.size();
	}
	wstrText += L"\n";
	return wstrText;
}

/// the reward button's tooltip: "@1 final EXP [x@2%]\n[@3] buff" ( 30745 ) with ( the prefix, the EXP rate, the buff's name ).
/// June builds it only when the reward buff has a templet ( 326c3170 on +0x1c ); returns an empty string when not.
std::wstring CX2RandomMission::BuildRewardTip( const WCHAR* wszPrefix_ ) const
{
	CX2BuffTempletPtr ptrBuff = CX2BuffTempletManager::GetInstance()->GetBuffTempletPtr( static_cast<BUFF_TEMPLET_ID>( m_iRewardBuffID ) );
	if( NULL == ptrBuff )
		return std::wstring();

	return std::wstring( GET_REPLACED_STRING( ( RM_STR_REWARD_TIP, "SiS", wszPrefix_, m_iRewardExpRate, GET_STRING( ptrBuff->GetBuffName() ) ) ) );
}

/// 326784b0: the popup's second line - "Additional [N%] Clear Dungeon EXP!" and the buff line, each broken at the panel's width
std::wstring CX2RandomMission::BuildRewardText() const
{
	std::wstringstream wstm;

	if( m_iRewardExpRate > 0 )
	{
		const std::wstring wstrExp = GET_REPLACED_STRING( ( RM_STR_REWARD_EXP, "i", m_iRewardExpRate ) );
		wstm << CWordLineHandler::GetStrByLineBreakInX2Main( wstrExp.c_str(), RM_TEXT_WIDTH, RM_TEXT_FONT ) << L"\n";
	}

	if( m_iRewardBuffID > 0 )
	{
		CX2BuffTempletPtr ptrBuff = CX2BuffTempletManager::GetInstance()->GetBuffTempletPtr( static_cast<BUFF_TEMPLET_ID>( m_iRewardBuffID ) );
		if( NULL != ptrBuff )
		{
			const std::wstring wstrBuff = GET_REPLACED_STRING( ( RM_STR_REWARD_BUFF, "S", GET_STRING( ptrBuff->GetBuffName() ) ) );
			wstm << CWordLineHandler::GetStrByLineBreakInX2Main( wstrBuff.c_str(), RM_TEXT_WIDTH, RM_TEXT_FONT );
		}
	}

	return wstm.str();
}

/// the block 32678910 ( create ), 32677f90 ( counter ) and 326791c0 ( result ) share: StaticInfo line 0 = the text broken to two
/// lines at 135, and the reward button's tooltip. An ellipsis in the text puts the whole text ahead of the reward line.
/// RM1 review: the three differ ( see SMALL_TEXT_WRITER ). The result's line 0 is broken only for its ellipsis flag
/// ( @3267930c, the string is dropped ); the counter writes the tooltip only with an ellipsis ( @3267818a, button +0x1d0
/// @3267842b ), so after a success ( AddEvent 32679820 runs the result, then the counter ) an ellipsed text ends with
/// "Mission Success" again and a short one keeps the result's empty string.
void CX2RandomMission::RefreshSmallText( SMALL_TEXT_WRITER eWriter_ )
{
	if( m_hDlgSmall == NULL )
		return;

	CKTDGUIStatic* pStaticInfo = static_cast<CKTDGUIStatic*>( m_hDlgSmall->GetControl( L"StaticInfo" ) );
	bool bEllipse = false;
	if( NULL != pStaticInfo || STW_RESULT == eWriter_ )
	{
		const std::wstring wstrText = BuildText();
		const std::wstring wstrBroken = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse( wstrText.c_str(), RM_TEXT_WIDTH, RM_TEXT_FONT, RM_TEXT_LINES, bEllipse );
		if( NULL != pStaticInfo && STW_RESULT != eWriter_ )
			pStaticInfo->SetString( 0, wstrBroken.c_str() );
	}

	if( STW_COUNTER == eWriter_ && false == bEllipse )
		return;

	CKTDGUIControl* pReward = m_hDlgSmall->GetControl( L"RewardButton" );
	if( NULL != pReward )
	{
		/// result ( 326791c0 ) passes an empty first argument ( 3344f1f4 ) where create / counter pass "Mission Success"
		const std::wstring wstrPrefix = ( STW_RESULT == eWriter_ ) ? std::wstring() : std::wstring( GET_STRING( RM_STR_MISSION_SUCCESS ) );
		std::wstring wstrTip = BuildRewardTip( wstrPrefix.c_str() );
		if( false == wstrTip.empty() )
		{
			if( true == bEllipse )
				wstrTip = BuildTooltipPrefix() + wstrTip;
			pReward->SetGuideDesc( wstrTip.c_str() );
		}
	}
}

/// 32678910: both dialogs are created hidden ( SetShow( false ) ), added to the dialog manager, and filled; the popup's fade timer
/// starts at 3.0
void CX2RandomMission::CreateUI()
{
	m_hDlgSmall = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Random_Mission.lua", RM_DIALOG_Z );
	if( m_hDlgSmall != NULL )
	{
		m_hDlgSmall->SetShow( false );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_hDlgSmall );
		RefreshSmallText( STW_CREATE );
	}

	m_hDlgPopup = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Random_Mission_PopUp.lua", RM_DIALOG_Z );
	if( m_hDlgPopup != NULL )
	{
		m_hDlgPopup->SetShow( false );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_hDlgPopup );
		m_fFadeTimer = RM_POPUP_FADE_START;

		CKTDGUIStatic* pStaticInfo = static_cast<CKTDGUIStatic*>( m_hDlgPopup->GetControl( L"StaticInfo" ) );
		if( NULL != pStaticInfo )
		{
			pStaticInfo->SetString( 0, BuildPopupText().c_str() );
			pStaticInfo->SetString( 1, BuildRewardText().c_str() );
		}
	}
}

/// 326791c0: the mission ended. Failed ( 2 ): the small panel's background only. Success ( 1 ): the small panel's background and
/// tooltip, then the popup ( background, shown, fade 3.0, its two lines ), and the time limit stops ( +0x28 = -1 ).
void CX2RandomMission::UpdateUI()
{
	if( m_hDlgSmall == NULL )
		return;

	CKTDGUIStatic* pBG = static_cast<CKTDGUIStatic*>( m_hDlgSmall->GetControl( L"StaticBG" ) );
	if( NULL == pBG || NULL == pBG->GetPicture( 0 ) )
		return;

	if( MR_SUCCESS != m_iResult )
	{
		if( MR_FAILED == m_iResult )
			pBG->GetPicture( 0 )->SetTex( L"DLG_UI_Common_Texture83_NEW.TGA", L"RANDOM_MISSION_UI_02" );
		return;
	}

	pBG->GetPicture( 0 )->SetTex( L"DLG_UI_Common_Texture83_NEW.TGA", L"RANDOM_MISSION_UI_03" );
	RefreshSmallText( STW_RESULT );

	if( m_hDlgPopup != NULL )
	{
		CKTDGUIStatic* pPopupBG = static_cast<CKTDGUIStatic*>( m_hDlgPopup->GetControl( L"StaticBG" ) );
		if( NULL != pPopupBG && NULL != pPopupBG->GetPicture( 0 ) )
		{
			pPopupBG->GetPicture( 0 )->SetTex( L"DLG_UI_Common_Texture84_NEW.TGA", L"RANDOM_MISSION_BIG_UI_02" );
			m_hDlgPopup->SetShow( true );
			m_fFadeTimer = RM_POPUP_FADE_START;
		}

		CKTDGUIStatic* pStaticInfo = static_cast<CKTDGUIStatic*>( m_hDlgPopup->GetControl( L"StaticInfo" ) );
		if( NULL != pStaticInfo )
		{
			pStaticInfo->SetString( 0, BuildPopupText().c_str() );
			pStaticInfo->SetString( 1, BuildRewardText().c_str() );
		}
	}

	m_iRemainSec = -1;
}

/// 32677f90
void CX2RandomMission::UpdateCounterText()
{
	RefreshSmallText( STW_COUNTER );
}

/// 32676660: "mm : ss" of the seconds left, in the panel's red line 2, once a second
void CX2RandomMission::UpdateTimeText()
{
	if( m_iRemainSec < 0 || m_hDlgSmall == NULL )
		return;

	CKTDGUIStatic* pStaticInfo = static_cast<CKTDGUIStatic*>( m_hDlgSmall->GetControl( L"StaticInfo" ) );
	if( NULL == pStaticInfo )
		return;

	const int iMin = m_iRemainSec / 60;
	const int iSec = m_iRemainSec % 60;
	std::wstringstream wstm;
	wstm << ( iMin < 10 ? L"0" : L"" ) << iMin << L" : " << ( iSec < 10 ? L"0" : L"" ) << iSec;
	pStaticInfo->SetString( 2, wstm.str().c_str() );
}

/// 326761c0
void CX2RandomMission::ShowSmall( bool bShow_ )
{
	if( m_hDlgSmall != NULL )
		m_hDlgSmall->SetShow( bShow_ );
}

/// 32676200
void CX2RandomMission::ShowPopup( bool bShow_ )
{
	if( m_hDlgPopup != NULL )
		m_hDlgPopup->SetShow( bShow_ );
}

/// 32679a50 @32679ac4..32679b3d: both dialogs are shown and set to ( 1, 1, 1, 0 ) at once for the fade-in. June does that with
/// 31cd35a0( pos, ( 1, 1, 1, 0 ), time 0 ); March's Move ignores a zero time, so the colour is set directly.
void CX2RandomMission::AppearBoth()
{
	ShowSmall( true );
	ShowPopup( true );
	if( m_hDlgSmall != NULL )
		m_hDlgSmall->SetColor( D3DXCOLOR( 1.f, 1.f, 1.f, 0.f ) );
	if( m_hDlgPopup != NULL )
		m_hDlgPopup->SetColor( D3DXCOLOR( 1.f, 1.f, 1.f, 0.f ) );
}

/// 32676870: the complete request ( 0x493, the mission id ) and the 10 s wait for its ACK ( 0x494 )
void CX2RandomMission::SendComplete()
{
	if( NULL == g_pData || NULL == g_pData->GetServerProtocol() )
		return;

	KEGS_JUNE_RANDOM_MISSION_COMPLETE_REQ kPacket;
	kPacket.m_iMissionID = m_iMissionID;
	g_pData->GetServerProtocol()->SendPacket( EGS_JUNE_RANDOM_MISSION_COMPLETE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_JUNE_RANDOM_MISSION_COMPLETE_ACK, RM_WAIT_COMPLETE_ACK );
#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_QUEST] RM complete request sent: mission %d", m_iMissionID );
#endif SERV_IRUHADEV_OFFLINE
}

/// 32679820. June bug kept ( JUNE_BUGS ): type 3 stores the value, and only a value over the goal ends it ( failed ).
int CX2RandomMission::AddEvent( int iValue_ )
{
	bool bEnded = false;
	switch( m_iEventType )
	{
	case RME_HITTED:
		m_iCounter = iValue_;
		if( iValue_ > m_iGoal )
		{
			m_iResult = MR_FAILED;
			bEnded = true;
		}
		break;
	case RME_GATE_KILL:
	case RME_QUICK_SLOT:
	case RME_RESURRECT:
	case RME_MP_SPENT:
	case RME_ACTIVE_SKILL:
	case RME_SPECIAL_SKILL:
	case RME_HYPER_SKILL:
		m_iCounter += iValue_;
		iValue_ = m_iCounter;
		if( m_iGoal <= iValue_ )
		{
			m_iResult = MR_SUCCESS;
			bEnded = true;
		}
		break;
	default:
		m_iCounter = iValue_;
		if( m_iGoal <= iValue_ )
		{
			m_iResult = MR_SUCCESS;
			bEnded = true;
		}
		break;
	}

	if( true == bEnded )
	{
		if( MR_SUCCESS == m_iResult )
			SendComplete();
		UpdateUI();
	}
	UpdateCounterText();

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_QUEST] RM event: mission %d type %d counter %d / goal %d result %d", m_iMissionID, m_iEventType, m_iCounter, m_iGoal, m_iResult );
#endif SERV_IRUHADEV_OFFLINE
	return m_iResult;
}

/// 326797a0: the dungeon ended. Types 3 ( hits ) and 5 ( survive ) succeed, anything else still running fails.
int CX2RandomMission::OnDungeonEnd()
{
	if( RME_HITTED != m_iEventType && RME_SURVIVE != m_iEventType )
	{
		m_iResult = MR_FAILED;
		UpdateUI();
	}
	else
	{
		m_iResult = MR_SUCCESS;
		SendComplete();
		UpdateUI();
	}
#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_QUEST] RM dungeon end: mission %d type %d counter %d / goal %d result %d", m_iMissionID, m_iEventType, m_iCounter, m_iGoal, m_iResult );
#endif SERV_IRUHADEV_OFFLINE
	return m_iResult;
}

/// 326762d0: the popup's life. Only while the fade timer is >= 0 and both dialogs are shown.
void CX2RandomMission::FadeTick( float fElapsedTime_ )
{
	if( 0.f <= m_fFadeTimer
		&& m_hDlgSmall != NULL && true == m_hDlgSmall->GetShow()
		&& m_hDlgPopup != NULL && true == m_hDlgPopup->GetShow() )
	{
		if( m_fFadeTimer > RM_POPUP_FADE_IN_END && m_hDlgSmall != NULL )
			DialogFade( m_hDlgSmall, 1.f );

		if( m_fFadeTimer <= RM_POPUP_FADE_IN_END )
		{
			if( m_fFadeTimer < RM_POPUP_FADE_OUT_START && m_hDlgPopup != NULL )
			{
				DialogFade( m_hDlgPopup, 0.f );
				m_fFadeTimer -= fElapsedTime_;
				return;
			}
		}
		else if( m_hDlgPopup != NULL )
		{
			DialogFade( m_hDlgPopup, 1.f );
			m_fFadeTimer -= fElapsedTime_;
			return;
		}
		m_fFadeTimer -= fElapsedTime_;
	}
}

//////////////////////////////////////////////////////////////////////////
// CX2RandomMissionManager
//////////////////////////////////////////////////////////////////////////

CX2RandomMissionManager* CX2RandomMissionManager::ms_pInstance = NULL;

/// 32676150: the bytes at +0 are 0x100 ( UI shown false, toggle true ), the popup timer -1.0, state 0, no mission
CX2RandomMissionManager::CX2RandomMissionManager()
	: m_bShowUI( false )
	, m_bToggleUI( true )
	, m_fPopupTimer( -1.f )
	, m_iState( MS_NONE )
	, m_pMission( NULL )
{
}

CX2RandomMissionManager* CX2RandomMissionManager::GetInstance()
{
	if( NULL == ms_pInstance )
		ms_pInstance = new CX2RandomMissionManager();
	return ms_pInstance;
}

/// 32676590
void CX2RandomMissionManager::DestroyInstance()
{
	if( NULL != ms_pInstance )
	{
		SAFE_DELETE( ms_pInstance->m_pMission );
		SAFE_DELETE( ms_pInstance );
	}
}

/// 32676070
void CX2RandomMissionManager::Reset()
{
#ifdef SERV_IRUHADEV_OFFLINE
	if( NULL != m_pMission || MS_NONE != m_iState )
		CX2OfflineLog::Server( L"[JUNE_QUEST] RM reset: state %d, mission %d", m_iState, NULL != m_pMission ? m_pMission->GetMissionID() : -1 );
#endif SERV_IRUHADEV_OFFLINE
	SAFE_DELETE( m_pMission );
	m_iState = MS_NONE;
	m_fPopupTimer = -1.f;
}

/// 326760c0
void CX2RandomMissionManager::StartPopupTimer()
{
	m_fPopupTimer = RM_POPUP_TIMER;
}

/// 326765d0
bool CX2RandomMissionManager::IsRunningEvent( int iEventType_ ) const
{
	if( MS_RUNNING != m_iState || NULL == m_pMission )
		return false;
	return m_pMission->GetEventType() == iEventType_;
}

/// 326765f0: with a mission, past the received state and the popup timer over: both dialogs follow the two UI bytes
void CX2RandomMissionManager::RefreshUI()
{
	if( NULL != m_pMission && m_iState > MS_RECEIVED && m_fPopupTimer <= 0.f )
	{
		if( true == m_bShowUI )
		{
			m_pMission->ShowSmall( m_bToggleUI );
			m_pMission->ShowPopup( m_bToggleUI );
		}
		else
			m_pMission->HideBoth();
	}
}

/// 32679970
void CX2RandomMissionManager::AddEvent( int iValue_ )
{
	if( NULL != m_pMission )
	{
		const int iResult = m_pMission->AddEvent( iValue_ );
		if( CX2RandomMission::MR_SUCCESS == iResult )
			m_iState = MS_SUCCESS;
		else if( CX2RandomMission::MR_FAILED == iResult )
			m_iState = MS_FAILED;
	}
}

/// 32679a20
void CX2RandomMissionManager::OnDungeonEnd()
{
	if( NULL != m_pMission )
	{
		const int iResult = m_pMission->OnDungeonEnd();
		if( CX2RandomMission::MR_SUCCESS == iResult )
			m_iState = MS_SUCCESS;
		else if( CX2RandomMission::MR_FAILED == iResult )
			m_iState = MS_FAILED;
	}
}

/// 32679a50
void CX2RandomMissionManager::OnFrameMove( float fElapsedTime_ )
{
	if( 0.f < m_fPopupTimer )
	{
		m_fPopupTimer -= fElapsedTime_;
		if( m_fPopupTimer <= 0.f )
		{
			if( NULL != m_pMission )
			{
				m_bShowUI = true;
				m_bToggleUI = true;
				m_pMission->AppearBoth();
			}
			m_fPopupTimer = -1.f;
		}
	}

	if( NULL != m_pMission )
	{
		/// 326799a0 ( this = the manager: the time limit of its mission ), then the popup's fade 326762d0.
		/// June quirk kept ( JUNE_BUGS ): the accumulator starts at -1.0, so the first second takes two, and the countdown keeps
		/// running after a success until it reaches 0, when it turns the mission to failed.
		CX2RandomMission* pMission = m_pMission;
		pMission->m_fTimeAccum += fElapsedTime_;
		if( 1.f <= pMission->m_fTimeAccum && 0 < pMission->m_iRemainSec )
		{
			--pMission->m_iRemainSec;
			pMission->m_fTimeAccum = 0.f;
			const int iRemain = pMission->m_iRemainSec;
			if( iRemain < 1 )
			{
				pMission->m_iResult = CX2RandomMission::MR_FAILED;
				pMission->UpdateUI();
			}
			pMission->UpdateTimeText();
			if( iRemain < 1 )
			{
				m_iState = MS_FAILED;
#ifdef SERV_IRUHADEV_OFFLINE
				CX2OfflineLog::Server( L"[JUNE_QUEST] RM time limit over: mission %d failed", pMission->GetMissionID() );
#endif SERV_IRUHADEV_OFFLINE
			}
		}
		if( NULL != m_pMission )
			m_pMission->FadeTick( fElapsedTime_ );
	}
}

/// 326798b0: state received, a new mission from the push ( the old one goes ), its dialogs made
void CX2RandomMissionManager::SetMission( const KEGS_JUNE_RANDOM_MISSION_NOT& kNot_ )
{
	m_iState = MS_RECEIVED;
	SAFE_DELETE( m_pMission );
	m_pMission = new CX2RandomMission( kNot_ );
	if( NULL != m_pMission )
	{
		m_pMission->CreateUI();
#ifdef SERV_IRUHADEV_OFFLINE
		CX2OfflineLog::Server( L"[JUNE_QUEST] RM mission received: id %d type %d goal %d seconds %d desc STR %d exp %d%% buff %d",
			kNot_.m_iMissionID, kNot_.m_iEventType, kNot_.m_iGoal, kNot_.m_iSeconds, kNot_.m_iDescStrID, kNot_.m_iRewardExpRate, kNot_.m_iRewardBuffID );
#endif SERV_IRUHADEV_OFFLINE
	}
}

/// 31fcd3d0: a skill was used. The skill's type ( 32206450( id, 1 ) +4 ) picks the event: 6 -> 10, 7 / 14 / 15 / 16 -> 11, 13 -> 12.
void CX2RandomMissionManager::OnSkillUse( int iSkillID_ )
{
	if( MS_RUNNING != m_iState || 0 == iSkillID_ || NULL == g_pData || NULL == g_pData->GetSkillTree() )
		return;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );	/// June passes ( id, 1 ); March's UPGRADE_SKILL_SYSTEM_2013 overload takes the id only
	if( NULL == pSkillTemplet )
		return;

	int iEventType = 0;
	switch( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_ACTIVE:
		iEventType = CX2RandomMission::RME_ACTIVE_SKILL;
		break;
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
	case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_1:
	case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_2:
	case CX2SkillTree::ST_SPECIAL_ACTIVE_GRADE_3:
		iEventType = CX2RandomMission::RME_SPECIAL_SKILL;
		break;
	case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
		iEventType = CX2RandomMission::RME_HYPER_SKILL;
		break;
	}

	if( true == IsRunningEvent( iEventType ) )
		AddEvent( 1 );
}

/// 327d1820 @327d1b8a: a stage past the first starts a received mission ( state 2 and the 5 s popup timer ) and clears a failed one
void CX2RandomMissionManager::OnStageStart( int iNowStageIndex_ )
{
	if( iNowStageIndex_ > 0 )
	{
		if( MS_RECEIVED == m_iState )
		{
			m_iState = MS_RUNNING;
			StartPopupTimer();
#ifdef SERV_IRUHADEV_OFFLINE
			CX2OfflineLog::Server( L"[JUNE_QUEST] RM started at stage %d", iNowStageIndex_ );
#endif SERV_IRUHADEV_OFFLINE
		}
		else if( MS_FAILED == m_iState )
		{
			DestroyInstance();
		}
	}
}


/// 327b1f30 @327b4f87..327b5063 ( twice: the user attacker, and a summon owned by my unit ): event 2. The damaged npc has a boss gage,
/// the now sub-stage is a boss stage ( +0x2c ), and my unit adds this hit to a running total ( user +0x8850 ); the value is that total
/// as a whole percent of the boss's max HP.
void CX2RandomMissionManager::OnBossDamage( CX2GUNPC* pNpc_, CX2GUUser* pMyUnit_, float fFinalDamage_ )
{
	if( false == IsRunningEvent( CX2RandomMission::RME_BOSS_DAMAGE ) )
		return;
	if( NULL == pNpc_ || NULL == pMyUnit_ || false == pNpc_->GetHasBossGage() )
		return;
	if( NULL == g_pX2Game || CX2Game::GT_DUNGEON != g_pX2Game->GetGameType() )
		return;

	CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>( g_pX2Game );
	CX2Dungeon* pDungeon = pDungeonGame->GetDungeon();
	if( NULL == pDungeon || NULL == pDungeon->GetNowStage() || NULL == pDungeon->GetNowStage()->GetNowSubStage() )
		return;
	if( false == pDungeon->GetNowStage()->GetNowSubStage()->GetIsBossStage() )
		return;

	const float fTotal = fFinalDamage_ + pMyUnit_->GetJuneMissionBossDamage();
	pMyUnit_->SetJuneMissionBossDamage( fTotal );

	float fMaxHp = pNpc_->GetMaxHp();
	if( fMaxHp <= 0.f )
		fMaxHp = 1.f;
	AddEvent( static_cast<int>( fTotal / fMaxHp * 100.0 ) );
}

/// 322b33d0 @322b3492..322b34b7 ( CX2GageData slot 3 ): a requested MP change below zero is MP spent; event 9 counts the whole
/// number. March's UpNowMp has no tracking flag or skip flag, so every negative change of my own gage counts.
void CX2RandomMissionManager::OnMpSpent( float fSpent_ )
{
	if( true == IsRunningEvent( CX2RandomMission::RME_MP_SPENT ) )
		AddEvent( static_cast<int>( fSpent_ ) );
}

void CX2GageData::JuneRandomMissionMpSpent( float fChangeOfNowMp_ )
{
	if( fChangeOfNowMp_ < 0.f && NULL != CX2GageManager::GetInstance() && this == CX2GageManager::GetInstance()->GetMyGageData() )
		CX2RandomMissionManager::GetInstance()->OnMpSpent( -fChangeOfNowMp_ );
}

/// 31efe2a0 ( dispatch 31f2ab80 case 0x492 ): read the push, make the manager, set the mission
bool CX2State::Handler_EGS_JUNE_RANDOM_MISSION_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_JUNE_RANDOM_MISSION_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	CX2RandomMissionManager::GetInstance()->SetMission( kEvent );
	return true;
}

/// 324c94d0 ( CX2StateDungeonGame, dispatcher 324dbc20 ): the complete request's ACK. Clears the wait entry, checks the OK word.
bool CX2StateDungeonGame::Handler_EGS_JUNE_RANDOM_MISSION_COMPLETE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_JUNE_RANDOM_MISSION_COMPLETE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_JUNE_RANDOM_MISSION_COMPLETE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
			return true;
	}
	return false;
}

/// 322f0db0 / 322f0e00 / 322f0e30: the dev-console cheats ( GameEditCommandList Slash_rm / Slash_rmexp / Slash_rmbuff ). Plain sends.
void CX2Game::SendRandomMissionCheat( int iA_, int iB_, int iC_ )
{
	KEGS_JUNE_RANDOM_MISSION_CHEAT_REQ kPacket;
	kPacket.m_iA = iA_;
	kPacket.m_iB = iB_;
	kPacket.m_iC = iC_;
	g_pData->GetServerProtocol()->SendPacket( EGS_JUNE_RANDOM_MISSION_CHEAT_REQ, kPacket );
}

void CX2Game::SendRandomMissionRewardEXPCheat( int iValue_ )
{
	KEGS_JUNE_RANDOM_MISSION_REWARD_EXP_CHEAT_REQ kPacket;
	kPacket.m_iValue = iValue_;
	g_pData->GetServerProtocol()->SendPacket( EGS_JUNE_RANDOM_MISSION_REWARD_EXP_CHEAT_REQ, kPacket );
}

void CX2Game::SendRandomMissionRewardBuffCheat( int iValue_ )
{
	KEGS_JUNE_RANDOM_MISSION_REWARD_BUFF_CHEAT_REQ kPacket;
	kPacket.m_iValue = iValue_;
	g_pData->GetServerProtocol()->SendPacket( EGS_JUNE_RANDOM_MISSION_REWARD_BUFF_CHEAT_REQ, kPacket );
}

#endif SERV_IRUHADEV_JUNE_RANDOM_MISSION
