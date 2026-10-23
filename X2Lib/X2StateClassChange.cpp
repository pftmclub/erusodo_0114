#include "StdAfx.h"
#include ".\x2stateclasschange.h"
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE



//
//CX2StateClassChange::CX2StateClassChange(void) :
//m_pUnitViewerUI( NULL ),
//m_pDLGClassChange( NULL )
//{
//
//	// note!! 전직 추가되면 수정
//	switch(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UnitClass)
//	{
//	case CX2Unit::UC_ELSWORD_KNIGHT:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_Knight.lua" );
//		} break;
//	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_Magic_Knight.lua" );
//		} break;
//	case CX2Unit::UC_LIRE_COMBAT_RANGER:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_Combat_Ranger.lua" );
//		} break;
//	case CX2Unit::UC_LIRE_SNIPING_RANGER:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_Sniping_Ranger.lua" );
//		} break;
//	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_High_Magician.lua" );
//		} break;
//	case CX2Unit::UC_ARME_DARK_MAGICIAN:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_Dark_Magician.lua" );
//		} break;
//	case CX2Unit::UC_RAVEN_SOUL_TAKER:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_SwordTaker.lua" );
//		} break;
//	case CX2Unit::UC_RAVEN_OVER_TAKER:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_OverTaker.lua" );
//		} break;
//	case CX2Unit::UC_EVE_EXOTIC_GEAR:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_CodeExotic.lua" );
//		} break;
//	case CX2Unit::UC_EVE_ARCHITECTURE:
//		{
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_CodeArchitecture.lua" );
//		} break;
//	default:
//		{
//			ASSERT( !"ClassChange State error : Unexpected UnitClass" );
//			m_pDLGClassChange = new CKTDGUIDialog( this, L"DLG_Class_Change_Knight.lua" );
//		} break;
//
//	}
//	
//#ifdef _NEXON_KR_
//	if( false == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser )	// 체험 아이디 제한	
//	{
//		CNMCOClientObject::GetInstance().ChangeMyLevel( 
//			( (UINT32)g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UnitClass << 24 ) | (UINT32)g_pData->GetSelectUnitLevel(), kUserFlag_ClassChanged );
//	}
//#endif
//
//
//	if ( m_pDLGClassChange != NULL )
//	{
//		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGClassChange );
//	}
//	
//	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
//	//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
//	m_pUnitViewerUI->SetLightPos( 300, 300, -500 );	// 캐릭터뷰어 라이트 위치 변경
//	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
//
//	m_pUnitViewerUI->SetUnit( g_pData->GetMyUser()->GetSelectUnit() );
//	m_pUnitViewerUI->GetMatrix().Move(  D3DXVECTOR3( -250, -130, 200 ) );
//	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.5f, 1.5f, 1.5f ) );
//	m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
//	m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );
//	m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_READY );
//
//	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
//
//	g_pKTDXApp->GetDGManager()->GetCamera().Point( 0,0,-1300, 0,0,0 );
//	g_pKTDXApp->GetDGManager()->SetProjection( g_pKTDXApp->GetDGManager()->GetNear(),
//		g_pKTDXApp->GetDGManager()->GetFar(), false );
//
//	// 마을을 벗어났으므로 패킷을 날린다.
//	g_pData->GetServerProtocol()->SendID( EGS_LEAVE_FIELD_NOT );
//
//	g_pKTDXApp->SkipFrame();
//
//}
//
//CX2StateClassChange::~CX2StateClassChange(void)
//{
//	SAFE_DELETE_DIALOG( m_pDLGClassChange );
//	SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );
//}
//
//HRESULT CX2StateClassChange::OnFrameMove( double fTime, float fElapsedTime )
//{
//	CX2StateCommonBG::OnFrameMove( fTime, fElapsedTime );
//
//	if ( m_pUnitViewerUI != NULL )
//	{
//		m_pUnitViewerUI->OnFrameMove( fTime, fElapsedTime );
//	}
//
//	return S_OK;
//}
//
//bool CX2StateClassChange::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//	if( CX2StateCommonBG::UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
//	{
//		return true;
//	}
//
//	switch( wParam )
//	{
//	case SCCUCM_STATE_CHANGE_VILLAGE:
//		{
//			Handler_EGS_STATE_CHANGE_FIELD_REQ();
//			return true;
//		}
//		break;
//	}
//
//	return false;
//}
//
//
//bool CX2StateClassChange::LastMsgByESC()
//{
//	OpenLastMsgPopUp( GET_STRING( STR_ID_554 ) );
//	return true;
//}
//
//void CX2StateClassChange::LastMsgPopOk()
//{
//	Handler_EGS_STATE_CHANGE_FIELD_REQ();
//}
//
//

//////////////////////////////////////////////////////////////////////////
CX2ClassChangePopup::CX2ClassChangePopup( void )
: m_bShow( false )
, m_pUnitViewerUI( NULL )
, m_pDLGClassChange( NULL )
{
}

CX2ClassChangePopup::~CX2ClassChangePopup( void )
{
	SAFE_DELETE_DIALOG( m_pDLGClassChange );
	SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );
}

HRESULT CX2ClassChangePopup::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_pUnitViewerUI != NULL )
	{
		m_pUnitViewerUI->OnFrameMove( fTime, fElapsedTime );
	}


	return S_OK;
}



bool CX2ClassChangePopup::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case CCUCM_STATE_CHANGE_VILLAGE:
		{
			SetShow( false );		
			switch ( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_BATTLE_FIELD:
				{
					CX2State* pState = static_cast<CX2State*>(g_pMain->GetNowState());
					if( NULL != pState )
					{
						pState->Handler_EGS_STATE_CHANGE_FIELD_REQ(g_pData->GetBattleFieldManager().GetReturnVillageId());

#ifdef NOT_RENDER_EFFECT_MADE_BY_GAME_UNIT	//김창한
						//필드에서 전직시 아래 조건들을 true로 돌려주지 않아 이펙트가 제대로 나오지 않는 문제 수정.
						if( NULL != g_pX2Game )
						{
							if( NULL != g_pX2Game->GetMajorParticle() )
								g_pX2Game->GetMajorParticle()->SetRenderEffectMadeByGameUnit( true );

							if( NULL != g_pX2Game->GetMinorParticle() )
								g_pX2Game->GetMinorParticle()->SetRenderEffectMadeByGameUnit( true );

							if( NULL != g_pX2Game->GetMajorXMeshPlayer() )
								g_pX2Game->GetMajorXMeshPlayer()->SetRenderEffectMadeByGameUnit( true );

							if( NULL != g_pX2Game->GetMinorXMeshPlayer() )
								g_pX2Game->GetMinorXMeshPlayer()->SetRenderEffectMadeByGameUnit( true );
						}
#endif //NOT_RENDER_EFFECT_MADE_BY_GAME_UNIT
					}
				} break;
			default:
				{			
					CX2StateMenu* pStateMenu = static_cast<CX2StateMenu*>(g_pMain->GetNowState());
					if( NULL != pStateMenu )
					{
						if ( NULL != g_pData->GetCashShop() &&
							true == g_pData->GetCashShop()->GetOpen() )
						{
							pStateMenu->ToggleCashShop();
						}
					}
				} break;
			
			}	
			return true;
		}
		break;
	}


	return false;
}

#ifdef SERV_SHARING_BANK_QUEST_CASH
bool CX2ClassChangePopup::SetShow( bool bShow, int iType )
{
	m_bShow = bShow;

	if( m_pDLGClassChange != NULL )
	{
		m_pDLGClassChange->SetHasUnit( NULL );

		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGClassChange, NULL, false );
		m_pDLGClassChange = NULL;
	}

	if( NULL != m_pUnitViewerUI )
	{
		SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );
	}

	if( true == bShow )
	{
		switch(iType)
		{
		case HPT_SHARE_BANK:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Share_Bank_Tutorial.lua" );
			}
			break;
		default:
			// 없는 타입이네;
			return false;
		}

		if ( m_pDLGClassChange != NULL )
		{
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGClassChange );
		}
	}
	return true;
}
#endif // SERV_SHARING_BANK_QUEST_CASH

#ifdef SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
/// June 32486e60 ( called by 324886b0 ): the class-change dialog by unit class; class - 10 above 0x7e, or below 10, takes the Knight dialog
static const WCHAR* JuneGetClassChangeDialogName( const int iUnitClass_ )
{
	switch( iUnitClass_ )
	{
	case 10:	return L"DLG_Class_Change_Knight.lua";
	case 11:	return L"DLG_Class_Change_Magic_Knight.lua";
	case 12:	return L"DLG_Class_Change_Combat_Ranger.lua";
	case 13:	return L"DLG_Class_Change_Sniping_Ranger.lua";
	case 14:	return L"DLG_Class_Change_High_Magician.lua";
	case 15:	return L"DLG_Class_Change_Dark_Magician.lua";
	case 16:	return L"DLG_Class_Change_SwordTaker.lua";
	case 17:	return L"DLG_Class_Change_OverTaker.lua";
	case 18:	return L"DLG_Class_Change_CodeExotic.lua";
	case 19:	return L"DLG_Class_Change_CodeArchitecture.lua";
	case 20:	return L"DLG_Class_Change_Chung_FG.lua";
	case 21:	return L"DLG_Class_Change_Chung_SG.lua";
	case 22:	return L"DLG_Class_Change_SheathKnight.lua";
	case 23:	return L"DLG_Class_Change_Battle_Magician.lua";
	case 24:	return L"DLG_Class_Change_TrappingRanger.lua";
	case 25:	return L"DLG_Class_Change_WeaponTaker.lua";
	case 26:	return L"DLG_Class_Change_Electra.lua";
	case 27:	return L"DLG_Class_Change_Shelling_Guardian.lua";
	case 28:	return L"DLG_Class_Change_Ara_LH.lua";
	case 29:	return L"DLG_Class_Change_Elesis_SK.lua";
	case 30:	return L"DLG_Class_Change_Elesis_PK.lua";
	case 31:	return L"DLG_Class_Change_Ara_LD.lua";
	case 32:	return L"DLG_Class_Change_Add_PT.lua";
	case 33:	return L"DLG_Class_Change_Add_AT.lua";
	case 34:	return L"DLG_Class_Change_Ara_LS.lua";
	case 35:	return L"DLG_Class_Change_Elesis_DK.lua";
	case 36:	return L"DLG_Class_Change_Add_TT.lua";
	case 37:	return L"DLG_Class_Change_Luciel_LC.lua";
	case 38:	return L"DLG_Class_Change_Luciel_CG.lua";
	case 39:	return L"DLG_Class_Change_Luciel_DB.lua";
	case 40:	return L"DLG_Class_Change_Rose_BD.lua";
	case 41:	return L"DLG_Class_Change_Rose_HB.lua";
	case 42:	return L"DLG_Class_Change_Rose_MH.lua";
	case 43:	return L"DLG_Class_Change_Rose_VK.lua";
	case 44:	return L"DLG_Class_Change_Ain_LE.lua";
	case 45:	return L"DLG_Class_Change_Ain_LA.lua";
	case 46:	return L"DLG_Class_Change_Ain_LW.lua";
	case 100:	return L"DLG_Class_Change_Lord_Knight.lua";
	case 101:	return L"DLG_Class_Change_Rune_Slayer.lua";
	case 102:	return L"DLG_Class_Change_Rena_WS.lua";
	case 103:	return L"DLG_Class_Change_Rena_GA.lua";
	case 104:	return L"DLG_Class_Change_Aisha_EM.lua";
	case 105:	return L"DLG_Class_Change_Aisha_VP.lua";
	case 106:	return L"DLG_Class_Change_Raven_BM.lua";
	case 107:	return L"DLG_Class_Change_Raven_RF.lua";
	case 108:	return L"DLG_Class_Change_Eve_CN.lua";
	case 109:	return L"DLG_Class_Change_Eve_CE.lua";
	case 110:	return L"DLG_Class_Change_Chung_IP.lua";
	case 111:	return L"DLG_Class_Change_Chung_DC.lua";
	case 112:	return L"DLG_Class_Change_Elsword_IS.lua";
	case 113:	return L"DLG_Class_Change_Aisha_DW.lua";
	case 114:	return L"DLG_Class_Change_Night_Watcher.lua";
	case 115:	return L"DLG_Class_Change_VeteranCommander.lua";
	case 116:	return L"DLG_Class_Change_Battle_Seraph.lua";
	case 117:	return L"DLG_Class_Change_Chung_TT.lua";
	case 118:	return L"DLG_Class_Change_Ara_SD.lua";
	case 119:	return L"DLG_Class_Change_Ara_YR.lua";
	case 120:	return L"DLG_Class_Change_Elesis_GM.lua";
	case 121:	return L"DLG_Class_Change_Elesis_BH.lua";
	case 122:	return L"DLG_Class_Change_Add_LP.lua";
	case 123:	return L"DLG_Class_Change_Add_MM.lua";
	case 124:	return L"DLG_Class_Change_Ara_AS.lua";
	case 125:	return L"DLG_Class_Change_Elesis_CA.lua";
	case 126:	return L"DLG_Class_Change_Add_DE.lua";
	case 127:	return L"DLG_Class_Change_Luciel_LD.lua";
	case 128:	return L"DLG_Class_Change_Luciel_CN.lua";
	case 129:	return L"DLG_Class_Change_Luciel_DN.lua";
	case 130:	return L"DLG_Class_Change_Rose_CR.lua";
	case 131:	return L"DLG_Class_Change_Rose_ST.lua";
	case 132:	return L"DLG_Class_Change_Rose_OM.lua";
	case 133:	return L"DLG_Class_Change_Rose_FY.lua";
	case 134:	return L"DLG_Class_Change_Ain_AT.lua";
	case 135:	return L"DLG_Class_Change_Ain_EE.lua";
	case 136:	return L"DLG_Class_Change_Ain_AP.lua";
	default:	break;
	}
	return L"DLG_Class_Change_Knight.lua";
}

#endif SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
const WCHAR* CX2ClassChangePopup::JuneGetTranscendGuideName( int iUnitType_ )
{
	switch( iUnitType_ )
	{
	case 1:		return L"DLG_Transcendental_Change_Guide_Elsowrd.lua";		// June's file name is spelled so
	case 2:		return L"DLG_Transcendental_Change_Guide_Aisha.lua";
	case 3:		return L"DLG_Transcendental_Change_Guide_Lena.lua";
	case 4:		return L"DLG_Transcendental_Change_Guide_Raven.lua";
	case 5:		return L"DLG_Transcendental_Change_Guide_Eve.lua";
	case 6:		return L"DLG_Transcendental_Change_Guide_Chung.lua";
	case 7:		return L"DLG_Transcendental_Change_Guide_Ara.lua";
	case 8:		return L"DLG_Transcendental_Change_Guide_Elesis.lua";
	case 9:		return L"DLG_Transcendental_Change_Guide_Add.lua";
	case 10:	return L"DLG_Transcendental_Change_Guide_Luciel.lua";
	case 11:	return L"DLG_Transcendental_Change_Guide_Rose.lua";
	case 12:	return L"DLG_Transcendental_Change_Guide_Ain.lua";
	default:	break;
	}
	return NULL;
}

bool CX2ClassChangePopup::JuneIsTranscendGuideName( const WCHAR* pwszFileName_ )
{
	if( NULL == pwszFileName_ )
		return false;

	for( int iUnitType = 1; iUnitType <= 12; ++iUnitType )
	{
		if( 0 == wcscmp( pwszFileName_, JuneGetTranscendGuideName( iUnitType ) ) )
			return true;
	}
	return false;
}

void CX2ClassChangePopup::JuneInitTranscendGuide( CKTDGUIDialogType pDialog_, int iUnitClass_, bool bClassPopup_ )
{
	if( NULL == pDialog_ )
		return;

	CKTDGUIStatic* pChange = pDialog_->GetStatic_LUA( "Static_Transcendental_Change" );
	if( NULL == pChange || NULL == pChange->GetPicture( 0 ) || NULL == pChange->GetPicture( 1 ) || NULL == pChange->GetPicture( 2 ) )
		return;

	pChange->GetPicture( 0 )->SetShow( false );
	pChange->GetPicture( 1 )->SetShow( false );
	pChange->GetPicture( 2 )->SetShow( false );
	if( NULL != pChange->GetPicture( 3 ) )
		pChange->GetPicture( 3 )->SetShow( false );

	// the picture of the class: even first-of-a-pair 0, odd 1, the third of a trio 2 ( June jump tables in 32486790 / 32710710 )
	int iPicture = -1;
	switch( iUnitClass_ )
	{
	case 100: case 102: case 104: case 106: case 108: case 110: case 118: case 120: case 122: case 127: case 130: case 134:
		iPicture = 0; break;
	case 101: case 103: case 105: case 107: case 109: case 111: case 119: case 121: case 123: case 128: case 131: case 135:
		iPicture = 1; break;
	case 112: case 113: case 114: case 115: case 116: case 117: case 124: case 125: case 126: case 129: case 132: case 136:
		iPicture = 2; break;
	case 133:
		iPicture = ( NULL != pChange->GetPicture( 3 ) ) ? 3 : -1; break;
	default:
		break;
	}
	if( iPicture >= 0 )
		pChange->GetPicture( iPicture )->SetShow( true );

	CKTDGUIStatic* pPopup = pDialog_->GetStatic_LUA( "Static_Transcendental_Popup" );
	if( NULL != pPopup && NULL != pPopup->GetPicture( 0 ) && NULL != pPopup->GetPicture( 1 ) )
	{
		pPopup->GetPicture( 0 )->SetShow( false == bClassPopup_ );
		pPopup->GetPicture( 1 )->SetShow( true == bClassPopup_ );
	}

	// June's popup keeps the image static and 1.5 in two members nothing reads yet ( Static_Transcendental_Image )
	if( true == bClassPopup_ )
	{
		CKTDGUIButton* pAccept = pDialog_->GetButton_LUA( "g_pButton_Accept_Quest1" );
		CKTDGUIButton* pBuy = pDialog_->GetButton_LUA( "g_pButton_Buy_CashItem1" );
		CKTDGUIButton* pClose = pDialog_->GetButton_LUA( "ButtonClass_Change_X" );
		CKTDGUIStatic* pString = pDialog_->GetStatic_LUA( "g_pStatic_Button_String" );
		if( NULL != pAccept )	pAccept->SetShow( false );
		if( NULL != pBuy )		pBuy->SetShow( false );
		if( NULL != pClose )	pClose->SetShow( true );
		if( NULL != pString )	pString->SetShow( false );
	}
}

/// June 324886b0 with bTranscendGuide = 1: the class-change dialog and the unit viewer go, the guide of the unit's type comes up
void CX2ClassChangePopup::SetShowTranscendGuide( bool bShow )
{
	m_bShow = bShow;

	if( m_pDLGClassChange != NULL )
	{
		m_pDLGClassChange->SetHasUnit( NULL );

		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGClassChange, NULL, false );
		m_pDLGClassChange = NULL;
	}

	if( NULL != m_pUnitViewerUI )
		SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );

	if( false == bShow )
		return;

	if( NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	const WCHAR* pwszName = JuneGetTranscendGuideName( static_cast<int>( pUnit->GetType() ) );
	if( NULL == pwszName )
	{
#ifdef SERV_IRUHADEV_OFFLINE
		CX2OfflineLog::Server( L"[JUNE_TRANSCEND] CHANGE guide: unit type %d has no guide dialog", static_cast<int>( pUnit->GetType() ) );
#endif SERV_IRUHADEV_OFFLINE
		return;
	}

	m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), pwszName, 0.07f, 0 );
	if( m_pDLGClassChange != NULL )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGClassChange );
		JuneInitTranscendGuide( m_pDLGClassChange, static_cast<int>( pUnit->GetClass() ), true );
#ifdef SERV_IRUHADEV_OFFLINE
		CX2OfflineLog::Server( L"[JUNE_TRANSCEND] CHANGE guide popup %s for class %d", pwszName, static_cast<int>( pUnit->GetClass() ) );
#endif SERV_IRUHADEV_OFFLINE
	}
}
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
void CX2ClassChangePopup::SetShow( bool bShow )
{
	m_bShow = bShow;


	if( m_pDLGClassChange != NULL )
	{
		m_pDLGClassChange->SetHasUnit( NULL );

		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGClassChange, NULL, false );
		m_pDLGClassChange = NULL;
	}

	if( NULL != m_pUnitViewerUI )
	{
		SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );
	}

	if( true == bShow )
	{

#ifdef _NEXON_KR_
		if( false == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser )	// 체험 아이디 제한	
		{
			CNMCOClientObject::GetInstance().ChangeMyLevel( 
				( (UINT32)g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UnitClass << 24 ) | (UINT32)g_pData->GetSelectUnitLevel(), kUserFlag_ClassChanged );
		}
#endif

		// note!! 전직 추가되면 수정
#ifdef SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
		/// June 324886b0 mode 0 -> 32486e60: June names March's file for every March class, adds UC_ADD_PSYCHIC_TRACER ( 32 ) and the June-only classes
		m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(),
			JuneGetClassChangeDialogName( static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UnitClass ) ) );
#else SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
		switch(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UnitClass)
		{
		case CX2Unit::UC_ELSWORD_KNIGHT:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Knight.lua" );
			} break;
		case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Magic_Knight.lua" );
			} break;
#ifdef ELSWORD_SHEATH_KNIGHT
		case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_SheathKnight.lua" );
			} break;
#endif ELSWORD_SHEATH_KNIGHT
		case CX2Unit::UC_LIRE_COMBAT_RANGER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Combat_Ranger.lua" );
			} break;
		case CX2Unit::UC_LIRE_SNIPING_RANGER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Sniping_Ranger.lua" );
			} break;
#ifdef SERV_TRAPPING_RANGER_TEST
		case CX2Unit::UC_LIRE_TRAPPING_RANGER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_TrappingRanger.lua" );
			} break;
#endif SERV_TRAPPING_RANGER_TEST
		case CX2Unit::UC_ARME_HIGH_MAGICIAN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_High_Magician.lua" );
			} break;
		case CX2Unit::UC_ARME_DARK_MAGICIAN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Dark_Magician.lua" );
			} break;
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
		case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Battle_Magician.lua" );
			} break;
#endif
		case CX2Unit::UC_RAVEN_SOUL_TAKER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_SwordTaker.lua" );
			} break;
		case CX2Unit::UC_RAVEN_OVER_TAKER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_OverTaker.lua" );
			} break;
#ifdef RAVEN_WEAPON_TAKER
		case CX2Unit::UC_RAVEN_WEAPON_TAKER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_WeaponTaker.lua" );
			} break;
#endif RAVEN_WEAPON_TAKER
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_CodeExotic.lua" );
			} break;
		case CX2Unit::UC_EVE_ARCHITECTURE:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_CodeArchitecture.lua" );
			} break;
#ifdef EVE_ELECTRA
		case CX2Unit::UC_EVE_ELECTRA:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Electra.lua" );
			} break;
#endif EVE_ELECTRA

		case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Lord_Knight.lua" );
			} break;
		case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Rune_Slayer.lua" );
			} break;
#ifdef SERV_ELSWORD_INFINITY_SWORD
		case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Elsword_IS.lua" );
			} break;
#endif

		case CX2Unit::UC_LIRE_WIND_SNEAKER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Rena_WS.lua" );
			} break;
		case CX2Unit::UC_LIRE_GRAND_ARCHER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Rena_GA.lua" );
			} break;
#ifdef SERV_RENA_NIGHT_WATCHER
		case CX2Unit::UC_LIRE_NIGHT_WATCHER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Night_Watcher.lua" );
			} break;
#endif



		case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Aisha_EM.lua" );
			} break;
		case CX2Unit::UC_ARME_VOID_PRINCESS:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Aisha_VP.lua" );
			} break;
#ifdef SERV_ARME_DIMENSION_WITCH
		case CX2Unit::UC_ARME_DIMENSION_WITCH:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Aisha_DW.lua" );
			} break;
#endif

			// 전직 팝업창에 캐릭터만 보이고 UI가 나오지 않는다면 UTF-8 포맷이 맞는지 확인을 먼저 해봅니다.

#ifdef RAVEN_SECOND_CLASS_CHANGE
		case CX2Unit::UC_RAVEN_BLADE_MASTER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Raven_BM.lua" );
			} break;
		case CX2Unit::UC_RAVEN_RECKLESS_FIST:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Raven_RF.lua" );
			} break;
#endif
#ifdef SERV_RAVEN_VETERAN_COMMANDER
		case CX2Unit::UC_RAVEN_VETERAN_COMMANDER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_VeteranCommander.lua" );
			} break;
#endif

		//{{ kimhc // 2010.1.22 //
#ifdef EVE_SECOND_CLASS_CHANGE
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Eve_CN.lua" );
			} break;
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Eve_CE.lua" );
			} break;
#endif EVE_SECOND_CLASS_CHANGE
		//}} kimhc // 2010.1.22 //
#ifdef SERV_EVE_BATTLE_SERAPH
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Battle_Seraph.lua" );
			} break;
#endif

		//{{ kimhc // 2011.1.3 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE //chung임시
	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Chung_FG.lua" );
			} break;
		case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Chung_SG.lua" );
			} break;
#endif	CHUNG_FIRST_CLASS_CHANGE
//}} kimhc // 2011.1.3 // 청 1차 전직

#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Shelling_Guardian.lua" );
			} break;
#endif

			//{{ JHKang / 강정훈 / 2011.6.13 / 청 2차 전직
#ifdef CHUNG_SECOND_CLASS_CHANGE
		case CX2Unit::UC_CHUNG_IRON_PALADIN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Chung_IP.lua" );
			} break;
		case CX2Unit::UC_CHUNG_DEADLY_CHASER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Chung_DC.lua" );
			} break;
#endif
				//}}
#ifdef SERV_CHUNG_TACTICAL_TROOPER
		case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Chung_TT.lua" );
			} break;
#endif
#ifdef ARA_CHANGE_CLASS_FIRST
		case CX2Unit::UC_ARA_LITTLE_HSIEN:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Ara_LH.lua" );
			} break;
		case CX2Unit::UC_ARA_SAKRA_DEVANAM:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Ara_SD.lua" );
			} break;
#endif //ARA_CHANGE_CLASS_FIRST
#ifdef NEW_CHARACTER_EL
		case CX2Unit::UC_ELESIS_SABER_KNIGHT:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Elesis_SK.lua" );
			} break;
		case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Elesis_PK.lua" );
			} break;
#endif // NEW_CHARACTER_EL

#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		case CX2Unit::UC_ARA_LITTLE_DEVIL:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Ara_LD.lua" );
			} break;
		case CX2Unit::UC_ARA_YAMA_RAJA:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Ara_YR.lua" );
			} break;
#endif // SERV_ARA_CHANGE_CLASS_SECOND

#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE //김창한
		case CX2Unit::UC_ELESIS_GRAND_MASTER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Elesis_GM.lua" );
			} break;
		case CX2Unit::UC_ELESIS_BLAZING_HEART:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Elesis_BH.lua" );
			} break;
#endif //SERV_ELESIS_SECOND_CLASS_CHANGE

#ifdef SERV_9TH_NEW_CHARACTER // 김태환 ( 캐릭터 추가용 )
		case CX2Unit::UC_ADD_PSYCHIC_TRACER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Add_PT.lua" );
			} break;
#endif //SERV_9TH_NEW_CHARACTER

#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
		case CX2Unit::UC_ADD_LUNATIC_PSYKER:
			{
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Add_LP.lua" );
			} break;
#endif //SERV_ADD_LUNATIC_PSYKER

		default:
			{
				ASSERT( !"ClassChange State error : Unexpected UnitClass" );
				m_pDLGClassChange = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Class_Change_Knight.lua" );
			} break;
		}
#endif SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP

		if ( m_pDLGClassChange != NULL )
		{
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGClassChange );
		}


		m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
		//m_pUnitViewerUI->SetLightPos( 300, 300, -500 );	// 캐릭터뷰어 라이트 위치 변경
		m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// 캐릭터뷰어 라이트 위치 변경
		//g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
#ifdef FIX_CLASS_CHANGE_MESH
		m_pUnitViewerUI->SetClassChange( true );
#endif

#ifdef SEPARATION_MOTION
		m_pUnitViewerUI->SetUnit( g_pData->GetMyUser()->GetSelectUnit(), CX2UnitViewerUI::UVS_CLASSCHANGE ); 
#else
		m_pUnitViewerUI->SetUnit( g_pData->GetMyUser()->GetSelectUnit() );
#endif
		//m_pUnitViewerUI->GetMatrix().Move(  D3DXVECTOR3( -250, -130, 200 ) );

		
		m_pDLGClassChange->SetHasUnit( (CKTDGObject*)m_pUnitViewerUI );
		
		m_pUnitViewerUI->SetFixed( true );
		D3DXVECTOR3 vUnitPos = m_pDLGClassChange->GetDummyPos(0);
		m_pUnitViewerUI->SetPositionOnScr( vUnitPos.x, vUnitPos.y, vUnitPos.z, 1.5f );

#ifdef SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
		{
			/// June @32488b01: 1.4 when my unit is UC_CHUNG_TACTICAL_TROOPER ( 117 ), else 1.5, through the viewer's scale ( 31f969a0, which also
			/// scales June's duo skin anim ); June then writes 1.5 to viewer +0xd84, a June-only viewer member March's viewer does not have
			const float fJuneScale = ( 117 == static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UnitClass ) ) ? 1.4f : 1.5f;
			m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( fJuneScale, fJuneScale, fJuneScale ) );
		}
#else SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
		m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.5f, 1.5f, 1.5f ) );
#endif SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
		m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
		//m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );
			

		m_pUnitViewerUI->SetShowObject( true, true );
		m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(), g_pKTDXApp->GetElapsedTime() );
		m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_READY );


		//g_pKTDXApp->SkipFrame();

#ifdef ADD_PLAY_SOUND //김창한
		if( NULL != g_pData->GetMyUser() &&
			NULL != g_pData->GetMyUser()->GetSelectUnit() )
			PlaySoundChangeClass(g_pData->GetMyUser()->GetSelectUnit()->GetType() );
#endif //ADD_PLAY_SOUND
	}

}

#ifdef ADD_PLAY_SOUND //김창한
void CX2ClassChangePopup::PlaySoundChangeClass( CX2Unit::UNIT_TYPE eUnitType )
{
#ifdef SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
	/// June 32486c10: <prefix 32486ae0> + "Voice_ClassUpFinish0" + "%d.ogg" for unit types 1-12 when the device manager exists; types 1, 6-10
	/// and 12 take rand() % 100 & 1 ( + 1 ), the others 1; PlaySound( name, false, false, 1 ) ( 31ce0e10 ) when the name is not empty
	if( NULL == g_pKTDXApp->GetDeviceManager() )
		return;

	const int iJuneType = static_cast<int>( eUnitType );
	if( static_cast<unsigned int>( iJuneType - 1 ) >= 12 )
		return;

	static const WCHAR* s_apwszJunePrefix[12] = { L"Elsword", L"Aisha", L"Rena", L"Raven", L"Eve", L"Chung", L"Ara", L"Elesis", L"Add", L"Luciel", L"Rose", L"Ain" };
	int iJuneNumber = 1;
	switch( iJuneType )
	{
	case 1: case 6: case 7: case 8: case 9: case 10: case 12:
		iJuneNumber = ( ( rand() % 100 ) & 1 ) + 1;
		break;
	default:
		break;
	}

	WCHAR wszJuneNumber[6];
	StringCchPrintfW( wszJuneNumber, ARRAY_SIZE( wszJuneNumber ), L"%d.ogg", iJuneNumber );
	const std::wstring wstrJuneSoundName = std::wstring( s_apwszJunePrefix[ iJuneType - 1 ] ) + L"Voice_ClassUpFinish0" + wszJuneNumber;
	if( false == wstrJuneSoundName.empty() )
		g_pKTDXApp->GetDeviceManager()->PlaySound( wstrJuneSoundName.c_str(), false, false, true );
#else SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
	if( g_pKTDXApp->GetDeviceManager() == NULL )
		return;

	if( eUnitType <= CX2Unit::UT_NONE || eUnitType >= CX2Unit::UT_END )
		return;

	wstring wstrSoundName = L"";
	int iRandom = -1;

	switch( eUnitType )
	{
	case CX2Unit::UT_ELSWORD: // 2
		{
			wstrSoundName = L"ElswordVoice_ClassUpFinish0";
			iRandom = RandomInt() % 2 + 1;
		} break;

	case CX2Unit::UT_ARME: // 1
		{
			wstrSoundName = L"AishaVoice_ClassUpFinish01.ogg";
		} break;

	case CX2Unit::UT_LIRE: // 1
		{
			wstrSoundName = L"LenaVoice_ClassUpFinish01.ogg";
		} break;

	case CX2Unit::UT_RAVEN: // 1
		{
			wstrSoundName = L"RavenVoice_ClassUpFinish01.ogg";
		} break;

	case CX2Unit::UT_EVE: // 1
		{
			wstrSoundName = L"EveVoice_ClassUpFinish01.ogg";
		} break;

	case CX2Unit::UT_CHUNG: // 2
		{
			wstrSoundName = L"ChungVoice_ClassUpFinish0";
			iRandom = RandomInt() % 2 + 1;
		} break;

	case CX2Unit::UT_ARA: // 2
		{
			wstrSoundName = L"AraVoice_ClassUpFinish0";
			iRandom = RandomInt() % 2 + 1;
		} break;

	case CX2Unit::UT_ELESIS: // 2
		{
			wstrSoundName = L"ElesisVoice_ClassUpFinish0";
			iRandom = RandomInt() % 2 + 1;
		} break;

	case CX2Unit::UT_ADD: // 2
		{
			wstrSoundName =L"AddVoice_ClassUpFinish0";
			iRandom = RandomInt() % 2 + 1;
		} break;

	default:
		break;
	}

	if( iRandom > -1 )
	{
		WCHAR buff[6];
		StringCchPrintf( buff, ARRAY_SIZE(buff), L"%d.ogg ", iRandom );
		wstrSoundName += std::wstring(buff);
	}

	if( wstrSoundName.compare(L"") != 0 )
		g_pKTDXApp->GetDeviceManager()->PlaySound(wstrSoundName.c_str(), false, false );
#endif SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP
}
#endif //ADD_PLAY_SOUND