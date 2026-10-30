#include "StdAfx.h"
#include ".\x2uicharinfo.h"
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_DLG
#include "Offline/X2OfflineLog.h"
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_STATS
#include <boost/math/special_functions/erf.hpp>
#endif SERV_IRUHADEV_JUNE_CHARINFO_STATS
#endif SERV_IRUHADEV_JUNE_CHARINFO_DLG



#ifdef MONSTER_TROPHY
	#include ".\X2NPCUnitViewerUI.h"
#endif MONSTER_TROPHY


CX2UICharInfo::CX2UICharInfo( CKTDXStage* pNowStage, const WCHAR* pFileName )
: CX2UIDragable( pNowStage, NULL ),
//: CX2ItemSlotManager( pNowStage, pFileName )
m_pDLGCharInfo(NULL),
//m_pStat(NULL),
//m_pStatEqip(NULL),
m_pUnitViewerUI(NULL),
m_DLGPosition(0,0),					// D3DXVECTOR2
m_DLGSize(0,0),						// D3DXVECTOR2
m_fRemainRotY(0.0f),
m_fMouseSensitivity(0.01f),
m_bRotateReturn(true),
m_UnitClickPos(117,93),				// D3DXVECTOR2
m_UnitClickSize(214,253),			// D3DXVECTOR2
m_bUnitClicked(false),
m_bShow(false),
m_MovedPosition(0,0)				// D3DXVECTOR2
#ifdef MONSTER_TROPHY

, m_pDLGMonsterTrophy( NULL )
, m_pNPCViewerUI( NULL )
//, m_eNPCID( CX2UnitManager::NUI_NONE )
, m_eNPCID( CX2UnitManager::NUI_BENDERS_B )

#endif MONSTER_TROPHY
#ifdef VIEW_SPECIAL_OPTIONDATA
, m_bShowPVPOption(false)
, m_bShowDungeonOption (true)
#endif //VIEW_SPECIAL_OPTIONDATA
#ifdef SERV_LOCAL_RANKING_SYSTEM
, m_pDlgPassword( NULL )
#endif //SERV_LOCAL_RANKING_SYSTEM
#ifdef MODIFY_CHARACTER_STAT_INFO
, m_fAddEvadePercent(0.f)
#endif // MODIFY_CHARACTER_STAT_INFO
//{{ robobeg : 2013-11-04
, m_Stat()
, m_StatEqip()
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
, m_bSwitchingView( false )
, m_bUnitClickedForSwitching( false )
#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW
#ifdef  PET_AURA_SKILL
, m_StatPet()
#endif  PET_AURA_SKILL
//}} robobeg : 2013-11-04
{

	//////////////////////////////////////////////////////////////////////////

	RegisterLuaBind();	

	m_pDLGCharInfo = new CKTDGUIDialog( g_pMain->GetNowState(), pFileName );
	m_pDLGCharInfo->SetShowEnable(false, false);
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCharInfo );	
	m_pDLGCharInfo->SetDisableUnderWindow(true);

	//m_pDLGCharInfo->SetFront(true);

//	m_pStat				= new CX2Stat;
//	m_pStatEqip			= new CX2Stat;
//#ifdef PET_AURA_SKILL
//	m_pStatPet			= new CX2Stat;
//#endif

	// 슬롯을 끄자

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		pItemSlot->LostItemUI();
		pItemSlot->SetShow( false );
		pItemSlot->SetEnable( false );
	}

	D3DXVECTOR3 tmp;
	tmp = m_pDLGCharInfo->GetDummyPos(0);
	m_DLGPosition.x = tmp.x;
	m_DLGPosition.y = tmp.y;
	tmp = m_pDLGCharInfo->GetDummyPos(1);
	m_DLGSize.x = tmp.x;
	m_DLGSize.y = tmp.y;
}

CX2UICharInfo::~CX2UICharInfo(void)
{
	SAFE_DELETE_DIALOG( m_pDLGCharInfo );	
//	SAFE_DELETE( m_pStat );
//	SAFE_DELETE( m_pStatEqip );
//#ifdef PET_AURA_SKILL
//	SAFE_DELETE( m_pStatPet );
//#endif

	SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );


#ifdef MONSTER_TROPHY
	SAFE_DELETE_DIALOG( m_pDLGMonsterTrophy );	
	SAFE_DELETE_KTDGOBJECT( m_pNPCViewerUI );
#endif MONSTER_TROPHY

#ifdef SERV_LOCAL_RANKING_SYSTEM
	SAFE_DELETE_DIALOG( m_pDlgPassword );
#endif //SERV_LOCAL_RANKING_SYSTEM
}

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
namespace _CONST_CHARINFO_C4_
{
	/// June 32918410 ( local_1c5 before 3234f4d0 ): a fashion or accessory templet goes to the side its minor-unit flag names; stand-in: the viewer-form accessory case reads form 0 ( Lu/Ciel not ported )
	bool IsMinorUnitItem( const CX2Item::ItemTemplet* pItemTemplet_ )
	{
		if ( pItemTemplet_->GetItemType() == CX2Item::IT_ACCESSORY || pItemTemplet_->GetFashion() == true )
			return pItemTemplet_->GetIsForMinorUnit();

		return false;
	}
}
using namespace _CONST_CHARINFO_C4_;
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
HRESULT CX2UICharInfo::OnFrameMove( double fTime, float fElapsedTime )
{

	KTDXPROFILE();


//#ifdef TITLE_SYSTEM
	if( g_pData->GetTitleManager() != NULL && g_pData->GetTitleManager()->GetShow() == true )
	{
		g_pData->GetTitleManager()->OnFrameMove( fTime, fElapsedTime );
	}
//#endif


// ** 유닛뷰어 조명 테스트
// 		KLuaManager LuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
// 		g_pKTDXApp->LoadAndDoMemory( &LuaManager, L"unproject.lua" );
// 		float x;
// 		float y;
// 		float z;
// 	
// 		if( LuaManager.BeginTable( "TEMP" ) == true )
// 		{
// 			LUA_GET_VALUE( LuaManager, 1, x, 250.0f );
// 			LUA_GET_VALUE( LuaManager, 2, y, 370.0f );
// 			LUA_GET_VALUE( LuaManager, 3, z, 100.0f );
// 		}
// 		m_pUnitViewerUI->SetLightPos(x, y, z);

	UnitHandling( fTime, fElapsedTime );

	if(m_pDLGCharInfo != NULL && m_pDLGCharInfo->GetIsMouseOver() == true)
	{
		return CX2ItemSlotManager::OnFrameMove( fTime, fElapsedTime );
	}
	else
	{
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
		/// June 32914380 returns here without invalidating the selected item and the slot description
	}

	return S_OK;
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
		CX2ItemSlotManager::InvalidSelectedItem();
		CX2ItemSlotManager::InvalidSlotDesc();
	}

	return S_OK;
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW

}




void CX2UICharInfo::UnitHandling( double fTime, float fElapsedTime )
{
	if( m_pUnitViewerUI != NULL )
	{
		float fRotSpeed = 13.0f;


		if ( m_bUnitClicked == true )
		{
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
			/// June 32912d90: a drag that moved the mouse is not a click on the unit
			if ( g_pKTDXApp->GetDIManager()->GetMouse()->GetXDelta() != 0 )
				m_bUnitClickedForSwitching = true;
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
			D3DXVECTOR3 RotateY = D3DXVECTOR3(0, -((float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXDelta() * m_fMouseSensitivity), 0);
			m_pUnitViewerUI->GetMatrix().RotateRel(RotateY);
			m_bRotateReturn = false;

			fRotSpeed = 13.0f;
		}
		else
		{
			D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();



			m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
			int addSpeed = 20;

			if ( m_bRotateReturn == false )
			{
				fRotSpeed -= (fElapsedTime * addSpeed);

				if ( fRotSpeed <= 0 )
				{
					fRotSpeed = fElapsedTime;
				}

				if (m_fRemainRotY >= 0.0f &&  m_fRemainRotY < 3.14f )
				{

					if ( m_fRemainRotY <= 0.0f )
					{
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						m_fRemainRotY = 0.0f;
						m_bRotateReturn = true;
					}
					else
					{
						m_fRemainRotY -= (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY <= 0.0f )
						{
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
							m_fRemainRotY = 0.0f;
							m_bRotateReturn = true;
						}
					}
				}
				else if ( m_fRemainRotY > 3.14f && m_fRemainRotY <= 6.18f  )
				{
					if ( m_fRemainRotY >= 6.18f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY += (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY >= 6.18f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
				else if ( m_fRemainRotY >= -3.14f && m_fRemainRotY <= 0.0f )
				{
					if ( m_fRemainRotY >= 0.0f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY += (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY >= 0.0f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
				else if (  m_fRemainRotY >= -6.28f && m_fRemainRotY <= -3.14f )
				{
					if ( m_fRemainRotY <= -6.28f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY -= (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY <= -6.28f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}

				}
			}
		}

		m_pUnitViewerUI->OnFrameMove( fTime, fElapsedTime );

		if ( m_pUnitViewerUI->GetXSkinAnim()->GetState() == CKTDGXSkinAnim::XAP_ONE_WAIT )
		{
			m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_WAIT );
		}

	}





#ifdef MONSTER_TROPHY

	if( m_pNPCViewerUI != NULL )
	{
		m_pNPCViewerUI->GetMatrix().RotateRel( 0.f, fElapsedTime * 0.6f, 0.f );
		m_pNPCViewerUI->OnFrameMove( fTime, fElapsedTime );
	}
		

#endif MONSTER_TROPHY
}



bool CX2UICharInfo::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 앞에 모달 다이얼로그가 있으면 메시지를 처리하지 않는다
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDLGCharInfo ) )
		return false;

	bool bFlag = false;
	switch( uMsg )
	{
		// 마우스 메시지들에 대해
	case WM_MOUSEMOVE:
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	case WM_MBUTTONUP:
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	//case WM_MOUSEWHEEL:
		{
			if ( false == m_pDLGCharInfo->GetIsMouseOver())
			{
				return false;
			}
			bFlag = true;
		}
		break;
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	/// June 3291d390: the mouse-up bookkeeping runs before the mouse-over test
	case WM_LBUTTONUP:
		{
			if ( m_pUnitViewerUI != NULL && m_bUnitClicked == true )
			{
				D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();
				int degree = (int)D3DXToDegree( rot.y );
				m_fRemainRotY = D3DXToRadian( degree % 360 );
			}

			if ( m_bUnitClicked == true && m_bUnitClickedForSwitching == false )
			{
				/// stand-in: June switches the Lu/Ciel form here ( 32916e10 / 32916e40 / 32912c50 ), Stage 8
				if ( m_pUnitViewerUI != NULL && m_pUnitViewerUI->GetUnit() != NULL && m_pUnitViewerUI->GetUnit()->GetType() == CX2Unit::UT_LUCIEL )
					CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB unit click switching -> Stage 8" );
			}

			m_bUnitClickedForSwitching = false;
			m_bUnitClicked = false;

			if ( false == m_pDLGCharInfo->GetIsMouseOver())
			{
				return false;
			}
			bFlag = true;
		}
		break;
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			if ( false == m_pDLGCharInfo->GetIsMouseOver())
			{
				return false;
			}
			g_pData->GetUIManager()->SetUILayerIndex(CX2UIManager::UI_MENU_CHARINFO, true);
			bFlag = true;			
		}
		break;
	default:
		break;
	}

	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
			/// the first switch already ran the rotation bookkeeping ( June 3291d390 )
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
			if ( m_pUnitViewerUI != NULL && m_bUnitClicked == true )
			{
				D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();
				int degree = (int)D3DXToDegree( rot.y );
				m_fRemainRotY = D3DXToRadian( degree % 360 );
			}

			m_bUnitClicked = false;
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW


			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );

			MouseUp( mousePos );


		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );


			if ( MouseDown( mousePos) == true )
				return true;

			if ( m_bShow == true && IsInUnitClickReact( mousePos ) == true )
			{
				m_bUnitClicked = true ;
				return true;
			}
		}
		break;

	case WM_MOUSEMOVE:
		{

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			MouseMove( mousePos );
		}
		break;

	case WM_RBUTTONDOWN:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );

			SetNowDragItemReturn();

			MouseRButtonUp( mousePos );

		}
		break;

	case WM_RBUTTONUP:
		{
			SetNowDragItemReturn();
		}

	}

	return bFlag;
}



bool CX2UICharInfo::MouseDown( D3DXVECTOR2 mousePos )
{	

	CX2SlotItem* pSlot = (CX2SlotItem*)GetSlotInMousePos( mousePos );

	if ( pSlot == NULL )
		return false;

	if ( pSlot->IsResetItemUI() == false )
	{
		return false;
	}

	CX2State* pState = (CX2State*)g_pMain->GetNowState();
	if(pState != NULL)
	{
		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_NORMAL )
		{
			return false;
		}
	}

	*m_pSlotBeforeDragging = pSlot;
	//g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( (*m_pSlotBeforeDragging)->GetDialog(), true );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), GetDraggingItemLayer() );
	(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,0.7f) );

	*m_DraggingItemUID = pSlot->GetItemUID();
	
	if ( (*m_pSlotBeforeDragging)->GetSlotType() == CX2Slot::ST_EQUIPPED )
	{		

		g_pData->GetMyUser()->GetSelectUnit()->RemoveEqip( *m_DraggingItemUID );
		if( NULL != m_pUnitViewerUI )
		{
			m_pUnitViewerUI->RemoveEqip( *m_DraggingItemUID );
		}

		ResetStat();
//		ResetRoomStateToInventory( *m_DraggingItemUID );
		if(NULL != g_pData->GetUIManager())
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN, true);
		}
		g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTabByUid( *m_DraggingItemUID );

	}

	g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );

	return true;
}


bool CX2UICharInfo::MouseUp( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pSlot = (CX2SlotItem*)GetSlotInMousePos( mousePos );
	if( pSlot == NULL)
		return false;

	// 헐
	CX2State* pState = (CX2State*)g_pMain->GetNowState();	// DownCast니까 안심
	if(pState != NULL)
	{

		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_NORMAL )
		{
			UidType ItemUID = pSlot->GetItemUID();
			CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( ItemUID );

			if(pItem == NULL )
				return false;

			switch(pCursor->GetCurorState())
			{
			case CX2Cursor::XCS_SELL:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_784 ), g_pMain->GetNowState() );
				}
				return true;
			case CX2Cursor::XCS_REPAIR:
				{
					if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP))
						g_pData->GetUIManager()->GetUIShop()->RepairItem( pSlot->GetItemUID() );
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_785 ), g_pMain->GetNowState() );

				}
				return true;
			case CX2Cursor::XCS_DECOMPOSITION:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_786 ), g_pMain->GetNowState() );
				}
				return true;
			case CX2Cursor::XCS_ENCHANT:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_787 ), g_pMain->GetNowState() );
				}
				return true;
			case CX2Cursor::XCS_SOCKET:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_788 ), g_pMain->GetNowState(), -1, 3.0f );
				}
				return true;

			case CX2Cursor::XCS_ATTRIB_ENCHANT:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_789 ), g_pMain->GetNowState() );
				}
				return true;

			case CX2Cursor::XCS_DELETE_ITEM:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4426 ), g_pMain->GetNowState() );
				}
				return true;

			//{{ kimhc // 2009-08-27 // 아이템 봉인
#ifdef	SEAL_ITEM
			case CX2Cursor::XCS_SEAL_ITEM:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4425 ), g_pMain->GetNowState() );
				}
				return true;
#endif	SEAL_ITEM
			//}} kimhc // 2009-08-27 // 아이템 봉인

				//case CX2Cursor::XCS_ITEM_EXCHANGE:
			default:
				break;
			}
			return true;
		}
	}


	if ( *m_pSlotBeforeDragging != NULL )
	{

		CX2SlotItem* pSlotItem = (CX2SlotItem*) *m_pSlotBeforeDragging;
		if( CX2Inventory::ST_E_SKILL == pSlotItem->GetSortType() ) 
		{
			if ( *m_pSlotBeforeDragging != NULL && (*m_pSlotBeforeDragging)->GetDialog() != NULL )
			{
				g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), (*m_pSlotBeforeDragging)->GetNormalLayer() );
				(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,1) );

				(*m_pSlotBeforeDragging)->ResetPos();
			}

			*m_pSlotBeforeDragging = NULL;
			*m_DraggingItemUID = -1;

			return true;
		}

		// 해당 다이얼로그 슬롯인지 찾는 함수가 필요하겠군

		//{{ kimhc // 실시간 엘소드 중 던전내에서 유저가 죽었거나, wait, mpCharge 상태가 아닐때 장비 교체 막기
#ifdef	REAL_TIME_ELSWORD

		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_BATTLE_FIELD:
			{
				if ( g_pX2Game == NULL || g_pX2Game->GetMyUnit() == NULL )
					return false;

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
				/// June 32917080 has no IsTransformed test
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
				if ( g_pX2Game->GetMyUnit()->IsTransformed() )
					return false;
#endif
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW

				if ( g_pX2Game->GetMyUnit()->GetNowHp() <= 0 )
				{
					g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2691 ),g_pMain->GetNowState() );

					return false;
				}

				// 게임내에서 바쁜 상태인 경우 장비교체 불가능
				if ( NULL != g_pX2Game && g_pX2Game->CheckAndWarningBusyStateNow() )
					return false;

			} break;

		default:
			break;
		}

		OnDropAnyItem( mousePos );
		SetNowDragItemReturn();
#endif	REAL_TIME_ELSWORD
		//}} kimhc // 실시간 엘소드 중 던전내에서 유저가 죽었거나, wait, mpCharge 상태가 아닐때 장비 교체 막기

		

		return true;
	}


// 	if ( *m_pSlotBeforeDragging != NULL && (*m_pSlotBeforeDragging)->GetDialog() != NULL )
// 	{
// 		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), GetItemLayer() );
// 		(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,1) );
// 	}

	//*m_pSlotBeforeDragging = NULL;
	//*m_DraggingItemUID = -1;

	return false;
}


bool CX2UICharInfo::MouseRButtonUp( D3DXVECTOR2 mousePos )
{
	if(GetSlotInMousePos( mousePos ) == NULL) return false;
	//*m_pSlotBeforeDragging = NULL;	
	//*m_DraggingItemUID = -1;

	return OnRClickedItem( mousePos );
}


bool CX2UICharInfo::OnRClickedItem( D3DXVECTOR2 mousePos )
{
	// 장비 아이템일 경우 처리 : 장착 해제
	CX2SlotItem* pItemSlot = NULL;
	pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );
	if ( pItemSlot == NULL )
		return false;

	if ( pItemSlot->GetSlotType() != CX2SlotManager::CX2Slot::ST_EQUIPPED || pItemSlot->IsResetItemUI() == false )
		return false;

	const CX2Item::ItemTemplet* pkItemTemplet = g_pData->GetItemManager()->GetItemTemplet( pItemSlot->GetItemTID() );
	if ( pkItemTemplet == NULL )
		return false;

	//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG
	if ( g_pMain->GetNowState() != NULL)
	{
		CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
		if ( pX2State->CheckIsReady() == true )
		{
			pX2State->ShowOkAndCancelMsgForUnReadyByEquip();
			return true;
		}
	}

	if ( g_pInstanceData->GetSendGameStartReqPacket() == true )
		return true;
#endif	MODIFY_INFINITE_SKILL_BUG
	//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정

	//{{ kimhc // 실시간 엘소드 중 던전내에서 유저가 죽었거나, wait, mpCharge 상태가 아닐때 장비 교체 막기
#ifdef	REAL_TIME_ELSWORD
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_BATTLE_FIELD:
		{
			if ( g_pX2Game == NULL || g_pX2Game->GetMyUnit() == NULL )
				return false;

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
			/// June 32917fd0 has no IsTransformed test
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
			if( g_pX2Game->GetMyUnit()->IsTransformed() )
				return false;
#endif
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW

			if ( g_pX2Game->GetMyUnit()->GetNowHp() <= 0 )
			{
				g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2691 ),g_pMain->GetNowState() );
				return false;
			}

			// 게임내에서 바쁜 상태인 경우 장비교체 불가능
			if ( NULL != g_pX2Game && g_pX2Game->CheckAndWarningBusyStateNow() )
				return false;

		} break;

	default:
		break;
	}
#endif	REAL_TIME_ELSWORD
	//}} kimhc // 실시간 엘소드 중 던전내에서 유저가 죽었거나, wait, mpCharge 상태가 아닐때 장비 교체 막기

	// TODO:
	// 인벤토리 열고 장비창으로 바꿔주고 (정렬도 됨)
	// 인벤토리 슬롯 변경 리퀘스트를 보낸다
	if(NULL != g_pData->GetUIManager())
	{
		g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN, true);
	}
	g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTabByUid( pItemSlot->GetItemUID() );
	CX2Inventory::SORT_TYPE NowInventorySortType = g_pData->GetUIManager()->GetUIInventory()->GetSortType();

	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
	{
		for ( int i = 0; i < kInventory.GetItemMaxNum( NowInventorySortType ); i++ )
		{
			// 빈자리 찾아서
			CX2Item* pItem = kInventory.GetItem( NowInventorySortType, i );
			if ( pItem == NULL )
			{
				Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( pItemSlot->GetSortType(), pItemSlot->GetSlotID(),
					NowInventorySortType, i );

				// 이전 슬롯은 지워주고
				pItemSlot->DestroyItemUI();
				InvalidSlotDesc();
				return true;
			}
		}
	}

	return false;
}

void CX2UICharInfo::SetNowDragItemReturn()
{
	if ( *m_pSlotBeforeDragging != NULL )
	{
		// 혹시 장비면
		if ( (*m_pSlotBeforeDragging)->GetSlotType() == CX2SlotManager::CX2Slot::ST_EQUIPPED )
		{
			//{{ 2008.11.13 김태완 : UI 예외처리
			UidType DraggingItemUID = ((CX2SlotItem*)(*m_pSlotBeforeDragging))->GetItemUID();
			//}}
			if ( g_pData->GetMyUser()->GetSelectUnit()->AddEqip( DraggingItemUID ) == true 
				&& g_pData->GetUIManager()->GetUICharInfo() != NULL )
			{
				g_pData->GetUIManager()->GetUICharInfo()->EquipChanged();						
			}
		}
		(*m_pSlotBeforeDragging)->ResetPos();
		if ( (*m_pSlotBeforeDragging)->GetDialog() != NULL )
		{
			(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,1) );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), (*m_pSlotBeforeDragging)->GetNormalLayer() );
		}

	}
	*m_pSlotBeforeDragging = NULL;

}



bool CX2UICharInfo::OnDropAnyItem( D3DXVECTOR2 mousePos )
{
	//{{ 2008.11.13 김태완 : UI 예외처리
	if(!(*m_pSlotBeforeDragging)->IsItem()) return false;
	//*m_DraggingItemUID = ((CX2SlotItem*)(*m_pSlotBeforeDragging))->GetItemUID();
	//}}

	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
	CX2Item* pItem = kInventory.GetItem( *m_DraggingItemUID );

	CX2SlotItem* pItemSlot = NULL;
	if(g_pData->GetUIManager()->GetUIInventory() == NULL)
	{
		ASSERT( !"UIInventory NULL" );			// 최초 생성 후 항상 살아있어야 함
		g_pData->GetUIManager()->CreateUIInventory();
	}

#ifdef SERV_SOCKET_NEW
	if (g_pData->GetUIManager()->GetUIInventory() != NULL &&
		g_pData->GetUIManager()->GetUIInventory()->GetUISocketItem() != NULL &&
		g_pData->GetUIManager()->GetUIInventory()->GetUISocketItem()->GetShow() == true)
	{
		if(g_pData->GetUIManager()->GetUIInventory()->GetUISocketItem()->GetSocketItemUID() == *m_DraggingItemUID)
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5113 ), g_pMain->GetNowState() );
			SetNowDragItemReturn();
			return false;
		}
	}
#endif SERV_SOCKET_NEW

	//{{ kimhc // 2009-09-09 // 봉인된 아이템 장착 불가
#ifdef	SEAL_ITEM
	if ( pItem == NULL )
		 return false;

	if ( pItem->GetItemData().m_bIsSealed == true )
		return false;

#endif	SEAL_ITEM
	//}} kimhc // 2009-09-09 // 봉인된 아이템 장착 불가

	//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
#ifdef	MODIFY_INFINITE_SKILL_BUG
	if ( g_pMain->GetNowState() != NULL)
	{
		CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
		if ( pX2State->CheckIsReady() == true )
		{
			pX2State->ShowOkAndCancelMsgForUnReadyByEquip();
			return true;
		}
	}

	if ( g_pInstanceData->GetSendGameStartReqPacket() == true )
		return true;
#endif	MODIFY_INFINITE_SKILL_BUG
	//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정

	g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTabByUid(*m_DraggingItemUID);

	if ( kInventory.GetItem( *m_DraggingItemUID ) != NULL )
	{
		CX2Item::ITEM_TYPE itemType = kInventory.GetItem( *m_DraggingItemUID )->GetItemTemplet()->GetItemType();
		if (  itemType == CX2Item::IT_WEAPON || itemType == CX2Item::IT_DEFENCE || itemType == CX2Item::IT_ACCESSORY ||
			itemType == CX2Item::IT_SKILL )
		{
			CX2Unit::EQIP_POSITION equipPosition = kInventory.GetItem( *m_DraggingItemUID )->GetItemTemplet()->GetEqipPosition();
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
			const bool bMinorUnitItem = IsMinorUnitItem( kInventory.GetItem( *m_DraggingItemUID )->GetItemTemplet() );
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW

			for ( int i = 0; i < (int)m_SlotList.size(); i++ )
			{
				CX2SlotItem* pTempItemSlot = (CX2SlotItem*)GetSlot(i);
				//{{ 태완 : 패션 악세사리 장착 예외처리
				if ( pTempItemSlot->GetSlotType() == CX2Slot::ST_EQUIPPED && pTempItemSlot->GetEquipPos() == equipPosition && 
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
					( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pTempItemSlot->GetFashion() == kInventory.GetItem( *m_DraggingItemUID )->GetItemTemplet()->GetFashion() ) &&
					pTempItemSlot->GetIsForMinorUnit() == bMinorUnitItem )
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
					( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pTempItemSlot->GetFashion() == kInventory.GetItem( *m_DraggingItemUID )->GetItemTemplet()->GetFashion() ) )
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
				{
					pItemSlot = pTempItemSlot;
					break;
				}
			}
		}
	}


	if ( pItemSlot == NULL )
		pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );

	//	CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );
	if ( pItemSlot == NULL || pItemSlot->GetEnable() == false )
		return false;

	//{{ 태완 : 패션 악세사리 장착 예외처리
	if ( pItemSlot->GetEquipPos() == pItem->GetItemTemplet()->GetEqipPosition() &&
		(pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItem->GetItemTemplet()->GetFashion() ) )
	{
		if ( pItemSlot == *m_pSlotBeforeDragging )
		{
// 			if ( g_pData->GetMyUser()->GetSelectUnit()->AddEqip( *m_DraggingItemUID ) == true )
// 				m_pUnitViewerUI->AddEqip( *m_DraggingItemUID );

			g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );
			SetNowDragItemReturn();
//			ResetStat();
			return true;
		}
		else
		{
#ifdef ITEM_RECOVERY_TEST
			if( true == pItem->IsDisabled() )
			{
				return false;
			}
#endif //ITEM_RECOVERY_TEST
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05 
			else if( false == pItem->GetIsEvaluation() )
			{
				return false;
			}			
#endif //SERV_NEW_ITEM_SYSTEM_2013_05

			if ( pItem->GetItemTemplet()->GetPeriodType() == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
			{
				return false;
			}
			else
			{
				Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( ((CX2SlotItem*)*m_pSlotBeforeDragging), pItemSlot );

				CX2SlotItem* pkSlotBeforeDragging = (CX2SlotItem*) *m_pSlotBeforeDragging;
				pkSlotBeforeDragging->DestroyItemUI();
			}
		}

		return true;
	}	

	SetNowDragItemReturn();
	return false;
}






bool CX2UICharInfo::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
//#ifdef TITLE_SYSTEM
	if( g_pData->GetTitleManager() != NULL && g_pData->GetTitleManager()->GetShow() == true )
	{
		if( g_pData->GetTitleManager()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
			return true;
	}
//#endif

	switch(wParam)
	{
	case CIM_EXIT:
		{
			SetShow(false);
			return true;
		}
		break;
	case CIM_TITLE:
		{
			if(g_pData->GetTitleManager() != NULL)
			{
				g_pData->GetTitleManager()->OpenTitle();
					//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
#ifdef PRINT_INGAMEINFO_TO_EXCEL
				if(g_pMain->IsInGameInfoToExcel())
				{
					g_pData->GetTitleManager()->PrintTitleInfo_ToExcel();
				}
#endif PRINT_INGAMEINFO_TO_EXCEL
				//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
				return true;
			}
		}
		break;
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
	/// June 3291db60 does not handle PROFILE / PASSWORD_OK / PASSWORD_CANCLE
#else SERV_IRUHADEV_JUNE_CHARINFO_SHOW
#ifdef SERV_LOCAL_RANKING_SYSTEM

	case CIM_PASSWORD_OK:
		{
			CKTDGUIEditBox* pEditBox = (CKTDGUIEditBox*) m_pDlgPassword->GetControl( L"EditBoxMessage" );
			if(pEditBox != NULL)
			{
				wstring password = pEditBox->GetText();

				if( 0 == StrCmp( L"", password.c_str() ) )
				{
					return true;
				}
				else
				{
					if(m_pDlgPassword != NULL)
					{
						g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDlgPassword, NULL, false );
						m_pDlgPassword = NULL;
					}

					if( NULL != g_pData->GetProfileManager() )
					{
						g_pData->GetProfileManager()->Handler_EGS_LOCAL_RANKING_USER_INFO_READ_REQ( password );
					}
				}
			}
			return true;
		}
		break;
			
	case CIM_PASSWORD_CANCLE:
		{
			if(m_pDlgPassword != NULL)
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDlgPassword, NULL, false );
				m_pDlgPassword = NULL;
			}
			return true;
		}
		break;

	case CIM_PROFILE:
		{
			if( NULL != g_pData->GetProfileManager() )
			{
				if( true == g_pData->GetProfileManager()->GetIsProfileSave() )
					g_pData->GetProfileManager()->Handler_EGS_LOCAL_RANKING_USER_INFO_READ_REQ( L"" );
				else
				{
#ifdef SERV_NAVER_CHANNELING
					if ( NULL != g_pData && NULL != g_pData->GetMyUser() )
					{
						switch ( g_pData->GetMyUser()->GetUserData().m_uChannelCode )
						{
						case KNexonAccountInfo::CE_NEXON_ACCOUNT:
							OpenProfilePasswordDlg();
							break;

						default:
							g_pData->GetProfileManager()->Handler_EGS_LOCAL_RANKING_USER_INFO_READ_REQ( L"" );
							break;
						}
					}
#else // SERV_NAVER_CHANNELING
					OpenProfilePasswordDlg();
#endif // SERV_NAVER_CHANNELING
	
				}
			}
			return true;
		}
		break;

#endif //SERV_LOCAL_RANKING_SYSTEM

#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW
#ifdef VIEW_SPECIAL_OPTIONDATA
	case CIM_SPECIAL_OPTIONDATA_TOGGLE:
		{
			CKTDGUIRadioButton* pRadionButtn = (CKTDGUIRadioButton*)lParam;
			if( NULL == pRadionButtn )
				return true;

			switch ( pRadionButtn->GetDummyInt(0) )
			{
			case 0: //던전 스탯
				{
					m_bShowPVPOption = false;
					m_bShowDungeonOption = true;
				} break;
			case 1: //대전 스탯
				{
					m_bShowPVPOption = true;
					m_bShowDungeonOption = false;
				} break;
			}

			ResetStat();
			return true;
		}
		break;
#endif VIEW_SPECIAL_OPTIONDATA
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_DLG
	/// the feature behind each button is not ported: June's window is drawn, the click only logs
	case CIM_CUSTOMIZE:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_CUSTOMIZE -> Stage 12" );
		return true;
	case CIM_RESONANCE:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_RESONANCE -> Stage 30" );
		return true;
	case UASCM_ACHIEVEMENT_OPEN:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB UASCM_ACHIEVEMENT_OPEN -> Stage 31" );
		return true;
	case CIM_SWITCHING:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_SWITCHING -> Stage 8" );
		return true;
	case CIM_SWITCHING_ONLY_THIS_DIALOG:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_SWITCHING_ONLY_THIS_DIALOG -> Stage 8" );
		return true;
	case CIM_WEAPON_CHANGE_REVOLVER:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_WEAPON_CHANGE_REVOLVER -> Stage 8" );
		return true;
	case CIM_WEAPON_CHANGE_AUTOGUN:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_WEAPON_CHANGE_AUTOGUN -> Stage 8" );
		return true;
	case CIM_WEAPON_CHANGE_MUSKET:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_WEAPON_CHANGE_MUSKET -> Stage 8" );
		return true;
	case CIM_WEAPON_CHANGE_HANDCANNON:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_WEAPON_CHANGE_HANDCANNON -> Stage 8" );
		return true;
	case CIM_LOCK_WEAPON_CHANGE_REVOLVER:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_LOCK_WEAPON_CHANGE_REVOLVER -> Stage 8" );
		return true;
	case CIM_LOCK_WEAPON_CHANGE_AUTOGUN:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_LOCK_WEAPON_CHANGE_AUTOGUN -> Stage 8" );
		return true;
	case CIM_LOCK_WEAPON_CHANGE_MUSKET:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_LOCK_WEAPON_CHANGE_MUSKET -> Stage 8" );
		return true;
	case CIM_LOCK_WEAPON_CHANGE_HANDCANNON:
		CX2OfflineLog::Server( L"[JUNE_CHARINFO] STUB CIM_LOCK_WEAPON_CHANGE_HANDCANNON -> Stage 8" );
		return true;
#endif SERV_IRUHADEV_JUNE_CHARINFO_DLG
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	/// June 3291db60 answers false for both; Light_On is the button shown while the background is dark, so LIGHT_ON clears the dark flag
	case CIM_LIGHT_ON:
		if ( g_pData->GetUIManager() != NULL )
			g_pData->GetUIManager()->SetCharWindowDarkBackground( false );
		return false;
	case CIM_LIGHT_OFF:
		if ( g_pData->GetUIManager() != NULL )
			g_pData->GetUIManager()->SetCharWindowDarkBackground( true );
		return false;
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	}
	return false;
}

bool CX2UICharInfo::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}

void CX2UICharInfo::SetShow(bool val)
{
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
	// June 3291df90
	m_bShow = val;

	if ( val == true )
	{
		CX2Unit* pSelectUnit = g_pData->GetMyUser()->GetSelectUnit();
		const CX2Unit::UnitData* pUnitData = &pSelectUnit->GetUnitData();
		WCHAR buff[256] = {0};

		CKTDGUIStatic* pStaticUserInfo = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"Static_Nickname" );
		if ( pStaticUserInfo != NULL && pStaticUserInfo->GetString(0) != NULL )
			pStaticUserInfo->GetString(0)->msg = pSelectUnit->GetNickName();

		CKTDGUIStatic* pStaticGuildName = static_cast< CKTDGUIStatic* >( m_pDLGCharInfo->GetControl( L"Static_GuildName" ) );
		if ( pStaticGuildName != NULL )
			pStaticGuildName->SetString( 0, g_pData->GetGuildManager()->ConvertDisplayGuildName( pUnitData->m_wstrGuildName ).c_str() );

		CKTDGUIStatic* pStaticPvpWin = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_PVP_Win" );
		if ( pStaticPvpWin != NULL && pStaticPvpWin->GetString(0) != NULL )
		{
			StringCchPrintf( buff, 256, GET_REPLACED_STRING( ( STR_ID_669, "i", ( int )pUnitData->m_Win ) ) );
			pStaticPvpWin->GetString(0)->msg = buff;
		}

		CKTDGUIStatic* pStaticPvpGrade = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"PVP_Grade" );
		if ( pStaticPvpGrade != NULL && g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem() != NULL )
		{
			pStaticPvpGrade->GetPicture(0)->SetTex( g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem()->m_TextureName.c_str(),
				g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem()->m_TextureKey.c_str() );
		}

		/// stand-in: UnitData has no PvE record ( June +0xa00 count, +0xa0c grade, +0xa0d wins ): 0 wins, grade hidden as June does under 5 records
		const int iPveWin = 0;
		CKTDGUIStatic* pStaticPveWin = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_PVE_Win" );
		if ( pStaticPveWin != NULL && pStaticPveWin->GetString(0) != NULL )
		{
			StringCchPrintf( buff, 256, GET_REPLACED_STRING( ( STR_ID_669, "i", iPveWin ) ) );
			pStaticPveWin->GetString(0)->msg = buff;
		}

		CKTDGUIStatic* pStaticPveGrade = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"PVE_Grade" );
		if ( pStaticPveGrade != NULL && pStaticPveGrade->GetPicture(0) != NULL )
			pStaticPveGrade->SetShow( false );

		CKTDGUIStatic* pStaticCharIcon = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"Static_CharIcon" );
		wstring fileName;
		wstring pieceName;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE
		if ( true == CX2Data::GetCharacterImageName( fileName, pieceName, pUnitData->m_UnitClass, CX2Data::CIT_20by20, 1 == pUnitData->m_iUnitClassTran ) )
#else SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE
		if ( true == CX2Data::GetCharacterImageName( fileName, pieceName, pUnitData->m_UnitClass, CX2Data::CIT_20by20 ) )
#endif SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE
		{
			pStaticCharIcon->GetPicture(0)->SetTex( fileName.c_str(), pieceName.c_str() );
		}

		/// June's 0x17 ( XS_PVE_GAME ) has no March state
		if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
		{
			m_bShowPVPOption = false;
			m_bShowDungeonOption = true;
		}

		if ( m_bShowDungeonOption == true )
		{
			CKTDGUIRadioButton* pRadioDungeonStat = static_cast<CKTDGUIRadioButton*>( m_pDLGCharInfo->GetControl( L"DungeonStat" ) );
			if ( pRadioDungeonStat != NULL )
				pRadioDungeonStat->SetChecked( true );
		}
		if ( m_bShowPVPOption == true )
		{
			CKTDGUIRadioButton* pRadioPvpStat = static_cast<CKTDGUIRadioButton*>( m_pDLGCharInfo->GetControl( L"PvpStat" ) );
			if ( pRadioPvpStat != NULL )
				pRadioPvpStat->SetChecked( true );
		}

		if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() > CX2User::XUAL_SPECIAL_USER )
		{
			CKTDGUIControl* pRadioEquipRelease = m_pDLGCharInfo->GetControl( L"EquipRelease" );
			if ( pRadioEquipRelease != NULL )
			{
				pRadioEquipRelease->SetEnable( true );
				pRadioEquipRelease->SetShow( true );
			}
		}

		/// stand-in: Lu/Ciel not ported, the form byte ( unit +0xe40 ) is 0
		SetSwitchingView( false );

		ResetStat();
		ResetNowEquipUI();
		ResetEDnCashnVP();
		UpdateLevel();
		ResetResurrectionStoneUI();

		/// June 329148d0 is March's ResetUnitViewerUI at form 0; 31faa150 ( Lu/Ciel minor-unit items ) is Stage 8
		ResetUnitViewerUI();
		m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVVMT_DOWN_LANDING );
		m_pUnitViewerUI->SetShowObject(true);
		m_pUnitViewerUI->SetHideSelectUnitWeapon();

		g_pData->GetUIManager()->UIOpened( CX2UIManager::UI_MENU_CHARINFO );
		if ( NULL != g_pData->GetPlayGuide() )
			g_pData->GetPlayGuide()->SetPlayGuide( CX2PlayGuide::PGT_OBTAIN_TITLE, false );

		CKTDGUIControl* pButtonResonance = m_pDLGCharInfo->GetControl( L"g_pButton_Resonance" );
		CKTDGUIControl* pButtonResonanceGuide = m_pDLGCharInfo->GetControl( L"g_pButton_Resonance_Guide" );
		CKTDGUIControl* pButtonResonanceGuideOnGame = m_pDLGCharInfo->GetControl( L"g_pButton_Resonance_Guide_OnGame" );
		if ( pButtonResonance != NULL && pButtonResonanceGuide != NULL && pButtonResonanceGuideOnGame != NULL )
		{
			/// stand-in: the unit's resonance flag ( +0xaa9 ) is not in March, June's absent value 0; 0xf ( XS_GUILD_AGIT_GAME ) has no March state
			bool bResonanceOpen = false;
			if ( bResonanceOpen == false )
			{
				pButtonResonance->SetShowEnable( true, false );
				pButtonResonanceGuide->SetShowEnable( true, true );
				pButtonResonanceGuideOnGame->SetShowEnable( false, false );
			}
			else if ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
			{
				pButtonResonance->SetShowEnable( true, true );
				pButtonResonanceGuide->SetShowEnable( false, false );
				pButtonResonanceGuideOnGame->SetShowEnable( false, false );
			}
			else
			{
				pButtonResonance->SetShowEnable( true, false );
				pButtonResonanceGuide->SetShowEnable( false, false );
				pButtonResonanceGuideOnGame->SetShowEnable( true, true );
			}
		}

		CKTDGUIControl* pButtonSwitching = m_pDLGCharInfo->GetControl( L"CharacterSwitching" );
		CKTDGUIControl* pButtonSwitchingInThisDialog = m_pDLGCharInfo->GetControl( L"CharacterSwitchingInThisDialog" );
		if ( pButtonSwitching != NULL && pButtonSwitchingInThisDialog != NULL )
		{
			pButtonSwitching->SetShowEnable( false, false );
			pButtonSwitchingInThisDialog->SetShowEnable( false, false );

			if ( pSelectUnit->GetType() == CX2Unit::UT_LUCIEL )
			{
				if ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
					pButtonSwitching->SetShowEnable( true, true );
				else
					pButtonSwitchingInThisDialog->SetShowEnable( true, true );
			}
		}

		m_bUnitClickedForSwitching = false;

		/// Rose's weapon change / lock UIs ( 32913f40 / 329141b0 ) belong to Stage 8
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
		/// June ends SetShow with the light ( 329142a0 ) of the flag CX2UIManager keeps
		SetLight( g_pData->GetUIManager()->GetCharWindowDarkBackground() );
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
	}
	else
	{
		if ( m_pUnitViewerUI != NULL )
			m_pUnitViewerUI->SetShowObject( false );
		m_pDLGSelectedItem->SetShow( false );
		InvalidSlotDesc();
		g_pData->GetUIManager()->UIClosed( CX2UIManager::UI_MENU_CHARINFO );
		/// stand-in: June also hides the account achievement window ( g_pData +0x5c8 ) here, not ported ( Stage 31 )
	}

	if ( m_bShow == true )
	{
		SetSwitchingView( false );
	}
	else
	{
		for ( int i = 0; i < (int)m_SlotList.size(); i++ )
		{
			CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
			pItemSlot->LostItemUI();
			pItemSlot->SetShow( false );
			pItemSlot->SetEnable( false );
		}
	}

	m_pDLGCharInfo->SetShowEnable( m_bShow, m_bShow );

	CX2UIInventory* pUIInventory = g_pData->GetUIManager()->GetUIInventory();
	if ( pUIInventory != NULL && pUIInventory->GetShow() == true )
		pUIInventory->SetInventorySort( pUIInventory->GetSortType(), pUIInventory->GetNowInvenSortTypePageNum() );
#else SERV_IRUHADEV_JUNE_CHARINFO_SHOW
	m_bShow = val;
	//SetEnable(m_bShow);
	//SetShow(m_bShow);


	if(val)	// 켜질 때 처리해야 할 부분
	{
		CX2Unit* pSelectUnit = g_pData->GetMyUser()->GetSelectUnit();

		WCHAR buff[256] = {0};
		const CX2Unit::UnitData* pUnitData = &pSelectUnit->GetUnitData();

		// 이름
		CKTDGUIStatic* pStaticUserInfo	= (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"Static_Nickname" );
		pStaticUserInfo->GetString(0)->msg = pSelectUnit->GetNickName();

		//{{ kimhc // 2009-11-04 // 길드 명 추가
#ifdef	GUILD_MANAGEMENT
		// 길드명
		CKTDGUIStatic* pStaticGuildName = static_cast< CKTDGUIStatic* >( m_pDLGCharInfo->GetControl( L"Static_GuildName" ) );
		if ( pStaticGuildName != NULL )
			pStaticGuildName->SetString( 0, g_pData->GetGuildManager()->ConvertDisplayGuildName( pSelectUnit->GetUnitData().m_wstrGuildName ).c_str() );
#endif	GUILD_MANAGEMENT
		//}} kimhc // 2009-11-04 // 길드 명 추가

#ifdef SERV_LOCAL_RANKING_SYSTEM
		CKTDGUIStatic* pStatic_Profile = static_cast< CKTDGUIStatic* >( m_pDLGCharInfo->GetControl( L"g_pStatic_Profile" ) );
		CKTDGUIButton* pButton_Profile = static_cast< CKTDGUIButton* >( m_pDLGCharInfo->GetControl( L"Profile" ) );
		if( NULL != pStatic_Profile && NULL != pButton_Profile &&  NULL != g_pData->GetProfileManager() )
		{
			g_pData->GetProfileManager()->SetDlgCharInfo( m_pDLGCharInfo );
			if( true == g_pData->GetProfileManager()->GetIsProfileSave() )
			{
				pStatic_Profile->SetString( 0, GET_STRING( STR_ID_23729 ) );
				pButton_Profile->SetGuideDesc( L"" );
			}
			else
			{
				pStatic_Profile->SetString( 0, GET_STRING( STR_ID_23933 ) );
				pButton_Profile->SetGuideDesc( GET_STRING( STR_ID_23934 ) );
			}	
		}
#endif //SERV_LOCAL_RANKING_SYSTEM

		// 전적
		CKTDGUIStatic* pStaticPVP_Room_Unit_WinNum = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_PVP_Win" );
		if( NULL != pStaticPVP_Room_Unit_WinNum &&
			NULL != pStaticPVP_Room_Unit_WinNum->GetString(0) )
		{	

// #ifdef TODAY_RECORD_TEST
// 
// 			StringCchPrintfW( buff, ARRAY_SIZE(buff), L"%d승 %d패\n%d kill, %d assist,\n %d dead,\n %d win,\n %d lose,\n %d draw,\n %d 연승,\n %d연패",
// 				(int)pUnitData->m_Win, (int)pUnitData->m_Lose,
// 				pUnitData->m_DailyAchievement.m_iKillCount,
// 				pUnitData->m_DailyAchievement.m_iAssistCount,
// 				pUnitData->m_DailyAchievement.m_iDieCount,
// 				pUnitData->m_DailyAchievement.m_iWinCount,
// 				pUnitData->m_DailyAchievement.m_iLoseCount,
// 				pUnitData->m_DailyAchievement.m_iDrawCount,
// 				pUnitData->m_DailyAchievement.m_iMaxConsecutivePVPWinCount,
// 				pUnitData->m_DailyAchievement.m_iMaxConsecutivePVPLoseCount );
// 
// 
// #else TODAY_RECORD_TEST

#ifdef SERV_PVP_NEW_SYSTEM
			StringCchPrintf( buff, 256, GET_REPLACED_STRING( ( STR_ID_669, "i", ( int )pUnitData->m_Win ) ) );
#else
			//wsprintf( buff, GET_REPLACED_STRING( ( STR_ID_790, "ii", ( int )pUnitData->m_Win, ( int )pUnitData->m_Lose ) ) );
			StringCchPrintf( buff, 256, GET_REPLACED_STRING( ( STR_ID_790, "ii", ( int )pUnitData->m_Win, ( int )pUnitData->m_Lose ) ) );
#endif

//#endif TODAY_RECORD_TEST

			pStaticPVP_Room_Unit_WinNum->GetString(0)->msg = buff;
		}
		
#ifdef SERV_PVP_NEW_SYSTEM
		CKTDGUIStatic* pStaticPVP_RankPoint = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_PVP_SCORE" );
		if( pStaticPVP_RankPoint != NULL )
		{
			pStaticPVP_RankPoint->GetString(0)->msg = g_pMain->GetEDString( (int)pUnitData->m_iRPoint );
		}
#endif

#ifdef PVP_SEASON2
		CKTDGUIStatic* pStaticPVP_Total = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_PVP_Total" );
		if( pStaticPVP_Total != NULL )
		{
			StringCchPrintf( buff, 256, GET_REPLACED_STRING( ( STR_ID_669, "i", ( int )pUnitData->m_Win + pUnitData->m_iPastSeasonWin ) ) );
			pStaticPVP_Total->GetString(0)->msg = buff;
		}
#endif

		// PVP랭크
		CKTDGUIStatic* pStaticPVP_Room_Unit_Title = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"PVP_Grade" );
		if ( pStaticPVP_Room_Unit_Title != NULL && g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem() != NULL )
		{
			//pStaticPVP_Room_Unit_Title->GetString(0)->msg = g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem()->m_EmblemName;
			pStaticPVP_Room_Unit_Title->GetPicture(0)->SetTex( g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem()->m_TextureName.c_str(),
				g_pMain->GetPVPEmblem()->GetMyNowUnitPVPEmblem()->m_TextureKey.c_str() );
		}

		// 캐릭터 아이콘
		CKTDGUIStatic* pStaticCharIcon = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"Static_CharIcon" );
		wstring fileName;
		wstring pieceName;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE
		if( true == CX2Data::GetCharacterImageName( fileName, pieceName, pUnitData->m_UnitClass, CX2Data::CIT_20by20, 1 == pUnitData->m_iUnitClassTran ) )
#else SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE
		if( true == CX2Data::GetCharacterImageName( fileName, pieceName, pUnitData->m_UnitClass, CX2Data::CIT_20by20 ) )
#endif SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE
		{
			pStaticCharIcon->GetPicture(0)->SetTex( fileName.c_str(), pieceName.c_str() );
		}
		else
		{
			ASSERT( !"NO" );
		}




#ifdef VIEW_SPECIAL_OPTIONDATA
		if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID())
		{
			m_bShowDungeonOption = true;
			m_bShowPVPOption = false;
		}
		if( true == m_bShowDungeonOption )
		{
			CKTDGUIRadioButton* pStatic = static_cast<CKTDGUIRadioButton*>(m_pDLGCharInfo->GetControl(L"DungeonStat"));
			if( NULL != pStatic)
			{
				pStatic->SetChecked(true);
			}
		}
		if( true == m_bShowPVPOption )
		{
			CKTDGUIRadioButton* pStatic = static_cast<CKTDGUIRadioButton*>(m_pDLGCharInfo->GetControl(L"PvpStat"));
			if( NULL != pStatic)
			{
				pStatic->SetChecked(true);
			}
		}
#endif //VIEW_SPECIAL_OPTIONDATA

		ResetStat();
		ResetNowEquipUI();
		ResetEDnCashnVP();
		UpdateLevel();
		ResetResurrectionStoneUI();

		ResetUnitViewerUI();
		m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVVMT_DOWN_LANDING ); 
		m_pUnitViewerUI->SetShowObject(true);
		m_pUnitViewerUI->SetHideSelectUnitWeapon();

		g_pData->GetUIManager()->UIOpened(CX2UIManager::UI_MENU_CHARINFO);
		 //도움말 입력
		if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
		{
			//캐릭터 창 열었을 때 칭호 가이드 끄기
			g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_OBTAIN_TITLE, false);
		}
	}
	else	// 꺼질 때 처리해야 할 부분
	{
		if(m_pUnitViewerUI != NULL) m_pUnitViewerUI->SetShowObject(false);
		// 아이템 마우스 오버 이미지를 지우자
		m_pDLGSelectedItem->SetShow( false ); 
		InvalidSlotDesc();
		g_pData->GetUIManager()->UIClosed(CX2UIManager::UI_MENU_CHARINFO);

#ifdef SERV_LOCAL_RANKING_SYSTEM
		if(m_pDlgPassword != NULL)
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDlgPassword, NULL, false );
			m_pDlgPassword = NULL;
		}
#endif //SERV_LOCAL_RANKING_SYSTEM

	}

	// 슬롯을 켜고 끄자
	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		if(m_bShow == false) pItemSlot->LostItemUI();
		pItemSlot->SetShow( m_bShow );
		pItemSlot->SetEnable( m_bShow );
	}

	m_pDLGCharInfo->SetShowEnable(m_bShow, m_bShow);



#ifdef MONSTER_TROPHY

	OpenDLGNPCViewerUI( val );
	if( true == val )
	{
		m_eNPCID = (CX2UnitManager::NPC_UNIT_ID) (rand() % (int) (CX2UnitManager::NUI_NPC_END-1) + 1);

		UpdateDLGNPCViewerUI();
	}

#endif MONSTER_TROPHY
#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW
}


void CX2UICharInfo::SetPosition(D3DXVECTOR2 vec)
{
	// 슬롯 다이얼로그 유닛뷰어
	m_pDLGCharInfo->SetPos(vec);

	m_pUnitViewerUI->SetFixed( true );
	D3DXVECTOR3 UnitviewerPos = m_pDLGCharInfo->GetDummyPos(2);
	m_pUnitViewerUI->SetPositionOnScr( UnitviewerPos.x+vec.x , UnitviewerPos.y+vec.y, UnitviewerPos.z, 1.15f );		// 유닛뷰어 위치는 스크립트에 있음

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		D3DXVECTOR2 pos;
		pos = pItemSlot->GetPos();

		// 복귀시킨 후 이동시킨다.
		pos -= m_MovedPosition;
		pos += vec;

		pItemSlot->SetPos(pos);
	}
	m_MovedPosition = vec;
}

void CX2UICharInfo::SetLayer(X2_DIALOG_LAYER layer)
{
	SetSlotManagerLayer(layer);
	SetSlotManagerChangeSequence(false);
	if(m_pDLGCharInfo!=NULL) 
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer(m_pDLGCharInfo, layer);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGCharInfo, false );
	}

	// 유닛뷰어는 다이얼로그 그릴 때 함께 그려진다.

}

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
namespace _CONST_CHARINFO_C2_
{
	/// June 32e22ce0: stand-in, a minor-unit slot is told by IS_FOR_MINOR_UNIT ( June's NESI ids for them clash with March's )
	bool IsEquippedSlotShown( CX2SlotItem* pItemSlot_, bool bSwitchingView_ )
	{
		if ( pItemSlot_->GetIsForMinorUnit() == true )
			return bSwitchingView_;

		switch ( pItemSlot_->GetSlotID() )
		{
		case CX2Unit::NESI_DEFENCE_BODY:
		case CX2Unit::NESI_DEFENCE_LEG:
		case CX2Unit::NESI_DEFENCE_HAND:
		case CX2Unit::NESI_DEFENCE_FOOT:
		case CX2Unit::NESI_WEAPON_HAND:
			return true;
		}

		return bSwitchingView_ == false;
	}
}
using namespace _CONST_CHARINFO_C2_;

void CX2UICharInfo::SetSwitchingView( bool bSwitchingView_ )
{
	/// June 32912c50; its viewer flag ( viewer +0xd80, 31f93a10 ) is the Lu/Ciel two-body view, Stage 8
	m_bSwitchingView = bSwitchingView_;

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(i);
		if ( pItemSlot == NULL || pItemSlot->GetSlotType() != CX2Slot::ST_EQUIPPED )
			continue;

		const bool bShow = IsEquippedSlotShown( pItemSlot, bSwitchingView_ );
		pItemSlot->SetShow( bShow );
		pItemSlot->SetEnable( bShow );
	}
}
#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
void CX2UICharInfo::SetLight( const bool bDarkBackground_ )
{
	/// June 329142a0: pictures 0-2 of the light set follow the flag, 3-5 its opposite, 6 is left alone
	if ( m_pDLGCharInfo == NULL )
		return;

	CKTDGUIStatic* pStaticLightSet = static_cast<CKTDGUIStatic*>( m_pDLGCharInfo->GetControl( L"CharWindowCharLightSet" ) );
	if ( pStaticLightSet != NULL && pStaticLightSet->GetPictureNum() == 7 )
	{
		for ( int i = 0; i < 7; i++ )
		{
			CKTDGUIControl::CPictureData* pPicture = pStaticLightSet->GetPicture( i );
			if ( pPicture != NULL && i != 6 )
				pPicture->SetShow( i < 3 ? bDarkBackground_ : bDarkBackground_ == false );
		}
	}

	CKTDGUIControl* pButtonLightOn = m_pDLGCharInfo->GetControl( L"Light_On" );
	CKTDGUIControl* pButtonLightOff = m_pDLGCharInfo->GetControl( L"Light_Off" );
	if ( pButtonLightOn != NULL && pButtonLightOff != NULL )
	{
		pButtonLightOn->SetShowEnable( bDarkBackground_, bDarkBackground_ );
		pButtonLightOff->SetShowEnable( bDarkBackground_ == false, bDarkBackground_ == false );
	}

	CX2OfflineLog::Server( L"[JUNE_CHARINFO] VIEW light dark %d statics %d buttons %d", bDarkBackground_ ? 1 : 0, pStaticLightSet != NULL ? 1 : 0, ( pButtonLightOn != NULL && pButtonLightOff != NULL ) ? 1 : 0 );
}
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
void CX2UICharInfo::SetTitle()
{	
//#ifdef TITLE_SYSTEM
	if(m_pDLGCharInfo != NULL && m_pDLGCharInfo->GetShow() == true)
	{
		bool bIsEmptyTitle = true;
		CKTDGUIStatic* pStaticUserInfo = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_Title" );		
		if(pStaticUserInfo != NULL)
		{
			// 칭호 명칭
#ifdef ELLIPSE_GLOBAL
			bool bEllipse = false;
			pStaticUserInfo->GetString(0)->msg = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(g_pData->GetTitleManager()->GetTitleName().c_str(), 100, pStaticUserInfo->GetString(0)->fontIndex, 1, bEllipse);
#else
			pStaticUserInfo->GetString(0)->msg = g_pData->GetTitleManager()->GetTitleName();        
#endif ELLIPSE_GLOBAL

			// 칭호 이미지		
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
			CKTDGUIStatic* pStaticTitleImg = pStaticUserInfo;	/// June 32915100: the picture sits in g_pStaticInfo_Title
#else SERV_IRUHADEV_JUNE_CHARINFO_SHOW
			CKTDGUIStatic* pStaticTitleImg = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo" );
#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW
			if(pStaticTitleImg != NULL)
			{
				CKTDGUIControl::CPictureData* pTitlePicture = pStaticTitleImg->GetPictureIndex(1);

				if(pTitlePicture != NULL)
				{
					wstring imgName = g_pData->GetTitleManager()->GetTitleImgName();
					pTitlePicture->SetTex(imgName.c_str());
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
					/// June 32915100 has no title score row
#else SERV_IRUHADEV_JUNE_CHARINFO_SHOW
#ifdef TITLE_SHOWING_PVP_RESULT
					CKTDGUIStatic* pStaticTitleScore = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_TitleScore" );
					if( pStaticTitleScore != NULL )
					{
						if( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
						{
							if( g_pData->GetMyUser()->GetSelectUnit()->GetTitleId() == _CONST_TITLE_SHOWING_PVP_RESULT_::iTitleID )
							{
								char szCount[10];
								::itoa( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAccountPVPLoseCount, szCount, 10 );
								pStaticTitleScore->GetString(0)->msg = KncUtil::toWideString(szCount).c_str();
								pStaticTitleScore->SetShow( true );
							}
							else
								pStaticTitleScore->SetShow( false );
						}
					}
#endif //TITLE_SHOWING_PVP_RESULT
#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW

#ifdef ADD_TITLE_DESCRIPTION
					// 타이틀 디스크립션 추가
					if( NULL != g_pData && 
						NULL != g_pData->GetMyUser() &&
						NULL != g_pData->GetMyUser()->GetSelectUnit() )
					{
						CKTDGUIButton* pTitleButton = static_cast<CKTDGUIButton*>(m_pDLGCharInfo->GetControl( L"TitleButton" ));
						if( NULL != pTitleButton )
						{
							const CX2TitleManager::TitleInfo* pTitleInfo =
								g_pData->GetTitleManager()->GetTitleInfo( g_pData->GetMyUser()->GetSelectUnit()->GetTitleId() );
								
							if( NULL != pTitleInfo )
							{
								pTitleButton->SetShowEnable(true, true);

								wstring wstrDesc = L"#C00FFFF";
								wstrDesc += pTitleInfo->m_wstrTitleName;
								wstrDesc += L"\n";
								wstrDesc += pTitleInfo->m_wstrDescription;
								wstrDesc += g_pData->GetTitleManager()->GetTitleAbilityDesc(  pTitleInfo->m_iTitleID );

								pTitleButton->SetGuideDesc( wstrDesc.c_str() );
								bIsEmptyTitle = false;
							}
						}
					}
#endif // ADD_TITLE_DESCRIPTION
				}
			}			
		}

		if( true == bIsEmptyTitle )
		{
#ifdef ADD_TITLE_DESCRIPTION
			// 타이틀 디스크립션 숨기기
			{
				CKTDGUIButton* pTitleButton = static_cast<CKTDGUIButton*>(m_pDLGCharInfo->GetControl( L"TitleButton" ));
				if( NULL != pTitleButton )
				{
					pTitleButton->SetShowEnable( false, false );
				}
			}
#endif // ADD_TITLE_DESCRIPTION

		}
	}    
//#endif
}

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_STATS
namespace _CONST_CHARINFO_C3_
{
	// June 322a8b60: the basic defence of a level ( its only caller is GetDamageDecrement )
	float GetBasicDefenceByLevel( const int iLevel_ )
	{
		return static_cast<float>( static_cast<double>( iLevel_ ) * static_cast<double>( 39.35f ) + static_cast<double>( 258.65f ) );
	}

	// June 3291f0c0..3291f270: a CalculateFinal*Coefficient curve of SocketOptionForm.lua ( sum, level )
	float GetFinalCoefficient( const char* pszCurve_, const float fSumValue_, const int iUserLevel_ )
	{
		return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), pszCurve_, fSumValue_, iUserLevel_ );
	}

	// June 3291f2a0 / 3291f4b0: the unit type picks CalculateFinal<Class>Specialization{A,B}Coefficient, any other type gives 0
	float GetFinalSpecializationCoefficient( const CX2Unit::UNIT_TYPE eUnitType_, const bool bA_, const float fSumValue_, const int iUserLevel_ )
	{
		const char* pszCurve = NULL;
		switch ( eUnitType_ )
		{
		case CX2Unit::UT_ELSWORD:	pszCurve = bA_ ? "CalculateFinalElswordSpecializationACoefficient" : "CalculateFinalElswordSpecializationBCoefficient";	break;
		case CX2Unit::UT_ARME:		pszCurve = bA_ ? "CalculateFinalArmeSpecializationACoefficient" : "CalculateFinalArmeSpecializationBCoefficient";			break;
		case CX2Unit::UT_LIRE:		pszCurve = bA_ ? "CalculateFinalLireSpecializationACoefficient" : "CalculateFinalLireSpecializationBCoefficient";			break;
		case CX2Unit::UT_RAVEN:		pszCurve = bA_ ? "CalculateFinalRavenSpecializationACoefficient" : "CalculateFinalRavenSpecializationBCoefficient";		break;
		case CX2Unit::UT_EVE:		pszCurve = bA_ ? "CalculateFinalEveSpecializationACoefficient" : "CalculateFinalEveSpecializationBCoefficient";			break;
		case CX2Unit::UT_CHUNG:		pszCurve = bA_ ? "CalculateFinalChungSpecializationACoefficient" : "CalculateFinalChungSpecializationBCoefficient";		break;
		case CX2Unit::UT_ARA:		pszCurve = bA_ ? "CalculateFinalAraSpecializationACoefficient" : "CalculateFinalAraSpecializationBCoefficient";			break;
		case CX2Unit::UT_ELESIS:	pszCurve = bA_ ? "CalculateFinalElesisSpecializationACoefficient" : "CalculateFinalElesisSpecializationBCoefficient";		break;
		case CX2Unit::UT_ADD:		pszCurve = bA_ ? "CalculateFinalAddSpecializationACoefficient" : "CalculateFinalAddSpecializationBCoefficient";			break;
		case CX2Unit::UT_LUCIEL:	pszCurve = bA_ ? "CalculateFinalLucielSpecializationACoefficient" : "CalculateFinalLucielSpecializationBCoefficient";		break;
		case CX2Unit::UT_ROSE:		pszCurve = bA_ ? "CalculateFinalRoseSpecializationACoefficient" : "CalculateFinalRoseSpecializationBCoefficient";			break;
		case CX2Unit::UT_AIN:		pszCurve = bA_ ? "CalculateFinalAinSpecializationACoefficient" : "CalculateFinalAinSpecializationBCoefficient";			break;
		default:					return 0.f;
		}
		return GetFinalCoefficient( pszCurve, fSumValue_, iUserLevel_ );
	}

	// June 32919430: every option guide ends with its curve's efficiency stage ( string 39999 )
	wstring GetCoefficientGuide( const float fCoefficient_ )
	{
		return SiCKTDXStringTable()->GetReplacedString( 39999, "i", static_cast<int>( fCoefficient_ * 100.0 + 0.1f ) );
	}

	// June 32919430: picture 0 ( awakening ) or, for Add, picture 1 ( DP ) of a charge / time static; returns the guide string id
	int ShowHyperModePicture( CKTDGUIStatic* pStatic_, const bool bAdd_, const int iStringID_, const int iAddStringID_ )
	{
		CKTDGUIControl::CPictureData* pPictureHyperMode = pStatic_->GetPicture( 0 );
		CKTDGUIControl::CPictureData* pPictureDP = pStatic_->GetPicture( 1 );
		if ( NULL == pPictureHyperMode || NULL == pPictureDP )
			return iStringID_;

		pPictureHyperMode->SetShow( false == bAdd_ );
		pPictureDP->SetShow( bAdd_ );
		return bAdd_ ? iAddStringID_ : iStringID_;
	}

	// June 32913460: a passive's relative rate as June adds it ( rate - 1.0 ( 334355b0 ) + 5e-8 ( 3346ed68 ) )
	float GetPassiveIncreaseRate( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const CX2SkillTree::SKILL_ABILITY_TYPE eType_, const int iSkillLevel_ )
	{
		return static_cast<float>( static_cast<double>( pSkillTemplet_->GetSkillAbilityValue( eType_, iSkillLevel_, false ) ) - 1.0 + 5e-08 );
	}
}
using namespace _CONST_CHARINFO_C3_;

void CX2UICharInfo::ResetStat( const bool bUseViewerForm_ /*= false*/ )
{
	// June 3291cfb0
	UNREFERENCED_PARAMETER( bUseViewerForm_ );
	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();
	const bool bMinorUnitForm = false;		// stand-in: Lu/Ciel not ported ( my unit +0xe40, or the viewer's +0xd80 when bUseViewerForm_ )

	CX2SocketItem::SocketData socketData = GetMyFormSocketData( bMinorUnitForm );

	m_Stat.InitStat();
	m_Stat.AddStat( pMyUnit->GetUnitStat(), true );
	m_Stat.AddStat( GetShownEquipStat( pMyUnit, bMinorUnitForm ) );
	m_Stat.AddStat( pMyUnit->GetSkillStat() );
	m_Stat.AddStat( pMyUnit->GetGuildSkillStat() );
	m_Stat.AddStat( g_pData->GetTitleManager()->GetSocketStat() );
	m_Stat.AddStat( socketData.m_Stat );

	UpdateBasicStatFromSpecialSkill();
	UpdateBasicStatUI( socketData );
	UpdateOptionStatFromSpecialSkill( socketData );
	UpdateOptionStatUI( socketData );
	UpdateResistanceStatUI( socketData );
	UpdateRecordGuideUI();

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
	const CX2Stat::EnchantStat& kEnchantStat = pMyUnit->GetEnchantStat( false );
	CX2OfflineLog::Server( L"[JUNE_CHARINFO] STATS ResetStat level %d type %d form %d viewer %d pvp %d dungeon %d resist %d %d %d %d %d %d",
		GetMyLevel(), static_cast<int>( pMyUnit->GetType() ), bMinorUnitForm ? 1 : 0, bUseViewerForm_ ? 1 : 0, m_bShowPVPOption ? 1 : 0, m_bShowDungeonOption ? 1 : 0,
		static_cast<int>( kEnchantStat.m_fDefBlaze ), static_cast<int>( kEnchantStat.m_fDefWater ), static_cast<int>( kEnchantStat.m_fDefNature ),
		static_cast<int>( kEnchantStat.m_fDefWind ), static_cast<int>( kEnchantStat.m_fDefLight ), static_cast<int>( kEnchantStat.m_fDefDark ) );
#endif SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
}

CX2SocketItem::SocketData CX2UICharInfo::GetMyFormSocketData( const bool bMinorUnitForm_ )
{
	// June 3291cee0: the form's equip group, with the HP increase of the shared group ( 2 )
	const CX2SocketItem::SocketData kSharedSocketData = GetMyEquippedSocketData( 2 );
	CX2SocketItem::SocketData kSocketData = GetMyEquippedSocketData( bMinorUnitForm_ ? 1 : 0 );
	kSocketData.m_Stat.m_ExtraStat.m_fIncreaseHPRate = kSharedSocketData.m_Stat.m_ExtraStat.m_fIncreaseHPRate;
	return kSocketData;
}

CX2Stat::Stat CX2UICharInfo::GetShownEquipStat( CX2Unit* pUnit_, const bool bMinorUnitForm_ ) const
{
	// June 32913df0: the form's equipment stat for the PvP / dungeon radio, with the HP of both forms ( group 2 )
	CX2Stat::Stat kEquipStat;
	if ( NULL != pUnit_ )
	{
		const CX2Stat::Stat kSharedEquipStat = pUnit_->GetEqipStat( m_bShowPVPOption, 2 );
		kEquipStat = pUnit_->GetEqipStat( m_bShowPVPOption, bMinorUnitForm_ ? 1 : 0 );
		kEquipStat.m_fBaseHP = static_cast<float>( kSharedEquipStat.m_fBaseHP );
	}
	return kEquipStat;
}

/*inline*/
int CX2UICharInfo::GetMyLevel() const
{
	return g_pData->GetSelectUnitLevel();
}

/*inline*/
float CX2UICharInfo::GetDamageDecrement( const float fTotalDefValue_ ) const
{
	// June 3291eb50
	float fDamageDecrement = fTotalDefValue_ / ( fTotalDefValue_ + GetBasicDefenceByLevel( g_pData->GetSelectUnitLevel() ) );

	if( 0.0f > fDamageDecrement )
		fDamageDecrement = 0.f;
	else if ( 99.99f < fDamageDecrement )
		fDamageDecrement = 99.99f;

	return fDamageDecrement;
}

void CX2UICharInfo::UpdateBasicStatUI( const CX2SocketItem::SocketData& socketData_ )
{
	// June 32915ab0
	WCHAR wszBuffer[MAX_PATH] = {0, };
	const CX2Stat::Stat& kStat = m_Stat.GetStat();
	const CX2Stat::Stat::ExtraStat& kExtraStat = socketData_.m_Stat.m_ExtraStat;
	const int iMyLevel = GetMyLevel();

	const float fBaseHP = kStat.m_fBaseHP;
	const float fMaxHpIncrement = static_cast<float>( fBaseHP * 0.5 );
	const int iHP = ( kExtraStat.m_fIncreaseHPRate <= fMaxHpIncrement ) ? static_cast<int>( kExtraStat.m_fIncreaseHPRate + fBaseHP ) : static_cast<int>( fMaxHpIncrement + fBaseHP );
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5d", iHP );
	SetStatInfoStatic( L"g_pStaticinfo_hp", wszBuffer );

	const float fAtkPhysic = kStat.m_fAtkPhysic;
	const int iAtkPhysic = static_cast<int>( fAtkPhysic * kExtraStat.m_fIncreaseAtkPhysicRate + fAtkPhysic );
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5d", iAtkPhysic );
	SetStatInfoStatic( L"g_pStaticinfo_p_attack", wszBuffer );

	const float fAtkMagic = kStat.m_fAtkMagic;
	const int iAtkMagic = static_cast<int>( fAtkMagic * kExtraStat.m_fIncreaseAtkMagicRate + fAtkMagic );
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5d", iAtkMagic );
	SetStatInfoStatic( L"g_pStaticinfo_m_attack", wszBuffer );

	const float fAtkPhysicSpread = 0.f;		// stand-in: June Stat [9] ( +0x108 ) has no March member ( Stage 33 )
	const int iMinAtkPhysic = static_cast<int>( ( fAtkPhysic - fAtkPhysicSpread ) * ( kExtraStat.m_fIncreaseAtkPhysicRate + 1.0 ) );
	const int iMaxAtkPhysic = static_cast<int>( ( fAtkPhysicSpread + fAtkPhysic ) * ( kExtraStat.m_fIncreaseAtkPhysicRate + 1.0 ) );
	const float fPhysicMaximizeRate = CX2SocketItem::GetFinalDamageMaximizeRate( socketData_.m_fDamageMaximizeRate, iMyLevel );
	const int iMaximizedAtkPhysic = iMinAtkPhysic + static_cast<int>( fPhysicMaximizeRate * ( iMaxAtkPhysic - iMinAtkPhysic ) );
	SetStatInfoGuideDesc( L"Button_PhysicAttack", GET_REPLACED_STRING( ( 39431, "iiii", iMinAtkPhysic, iMaxAtkPhysic, iMaximizedAtkPhysic, iMaxAtkPhysic ) ) );

	const float fAtkMagicSpread = 0.f;		// stand-in: June Stat [10] ( +0x10c ) has no March member ( Stage 33 )
	const int iMinAtkMagic = static_cast<int>( ( fAtkMagic - fAtkMagicSpread ) * ( kExtraStat.m_fIncreaseAtkMagicRate + 1.0 ) );
	const int iMaxAtkMagic = static_cast<int>( ( fAtkMagicSpread + fAtkMagic ) * ( kExtraStat.m_fIncreaseAtkMagicRate + 1.0 ) );
	const float fMagicMaximizeRate = CX2SocketItem::GetFinalDamageMaximizeRate( socketData_.m_fDamageMaximizeRate, iMyLevel );
	const int iMaximizedAtkMagic = iMinAtkMagic + static_cast<int>( fMagicMaximizeRate * ( iMaxAtkMagic - iMinAtkMagic ) );
	SetStatInfoGuideDesc( L"Button_MagicAttack", GET_REPLACED_STRING( ( 39432, "iiii", iMinAtkMagic, iMaxAtkMagic, iMaximizedAtkMagic, iMaxAtkMagic ) ) );

	const float fDefPhysic = kStat.m_fDefPhysic;
	const float fTotalDefPhysic = kExtraStat.m_fIncreaseDefPhysicRate * fDefPhysic + fDefPhysic;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5d", static_cast<int>( fTotalDefPhysic ) );
	SetStatInfoStatic( L"g_pStaticinfo_p_defence", wszBuffer );
	SetStatInfoGuideDesc( L"Button_PhysicDefence", GET_REPLACED_STRING( ( STR_ID_13570, "f", GetDamageDecrement( fTotalDefPhysic ) * 100.0 ) ) );

	const float fDefMagic = kStat.m_fDefMagic;
	const float fTotalDefMagic = kExtraStat.m_fIncreaseDefMagicRate * fDefMagic + fDefMagic;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5d", static_cast<int>( fTotalDefMagic ) );
	SetStatInfoStatic( L"g_pStaticinfo_m_defence", wszBuffer );
	SetStatInfoGuideDesc( L"Button_MagicDefence", GET_REPLACED_STRING( ( STR_ID_13571, "f", GetDamageDecrement( fTotalDefMagic ) * 100.0 ) ) );

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
	CX2OfflineLog::Server( L"[JUNE_CHARINFO] STATS basic hp %d (base %.1f inc %.3f) patk %d matk %d prange %d-%d max %d mrange %d-%d max %d pdef %d mdef %d",
		iHP, fBaseHP, kExtraStat.m_fIncreaseHPRate, iAtkPhysic, iAtkMagic, iMinAtkPhysic, iMaxAtkPhysic, iMaximizedAtkPhysic,
		iMinAtkMagic, iMaxAtkMagic, iMaximizedAtkMagic, static_cast<int>( fTotalDefPhysic ), static_cast<int>( fTotalDefMagic ) );
#endif SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
}

void CX2UICharInfo::UpdateRecordGuideUI()
{
	// June 32915620
	if ( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	const CX2Unit::UnitData& kUnitData = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
	const int iTotalPvpWin = kUnitData.m_Win + kUnitData.m_iPastSeasonWin;	// June unit +0x12c + +0x120
	SetStatInfoGuideDesc( L"ButtonPvpOpacity", GET_REPLACED_STRING( ( 42074, "i", iTotalPvpWin ) ) );

	const int iTotalPveWin = 0;		// stand-in: UnitData lacks June's PvE record ( unit +0xa0d )
	SetStatInfoGuideDesc( L"ButtonPveOpacity", GET_REPLACED_STRING( ( 32067, "i", iTotalPveWin ) ) );

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
	CX2OfflineLog::Server( L"[JUNE_CHARINFO] STATS record pvp %d pve %d", iTotalPvpWin, iTotalPveWin );
#endif SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
}
#else SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::ResetStat()
{
	//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
	// 월드 버프 수치 표시는 보류!!!
	CX2SocketItem::SocketData socketData = GetMyEquippedSocketData();
	const CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();
	
	m_Stat.InitStat();
	// 장비, 셋트, 펫, 칭호의 소켓 옵션은 위의 GetMyEquippedSocketData()에서 socketData의 옵션으로 더 해졌을 것이기 때문에 여기서 처리 하지 않음
	m_Stat.AddStat( pMyUnit->GetUnitStat(), true );	// true를 안해도 될 것 같지만... 확인해 보지 않았음
	// 장비의 기본 Stat을 추가함 (부가효과, 소켓효과 X)
	m_Stat.AddStat( pMyUnit->GetEqipStat() );	
	// 스킬 스탯은 소켓 옵션이 아니므로 여기서 처리 ( GetSkillStat() 내부에서 ExtraStat을 더하지 않으므로 여기서도 해줄 필요 없음 )
	m_Stat.AddStat( pMyUnit->GetSkillStat() );	// Stat을 기본적으로 올려주는 스킬만 이곳에서 처리 중이다.
	// 길드 스킬 스탯은 소켓 옵션이 아니므로 여기서 처리 ( GetGuildSkillStat() 내부에서 ExtraStat을 더하지 않으므로 여기서도 해줄 필요 없음 )
	m_Stat.AddStat( pMyUnit->GetGuildSkillStat() );	// Stat을 기본적으로 올려주는 스킬만 이곳에서 처리 중이다.

	// 칭호 스탯 추가
	m_Stat.AddStat( g_pData->GetTitleManager()->GetSocketStat() );

	// SocketData의 기본 스탯 을 더해준다.
	m_Stat.AddStat( socketData.m_Stat );

	// 스킬에 의한 스탯 (2011.07.20 현재는 HP와 관련된 것만 있음)
	UpdateBasicStatFromSpecialSkill();

	// HP, 물공, 마공, 물방, 마방 UI 갱신
	UpdateBasicStatUI( socketData );

	// 스킬에 의한 OptionStat
	UpdateOptionStatFromSpecialSkill( socketData );
	
	// 크리티컬, 추가데미지, 명중, 각성충전속도, 각성지속시간, 회피, 데미지감소, 동작속도, 이동력, 점프력
	UpdateOptionStatUI( socketData );

	// 불, 물, 자연, 바람, 빛, 어둠 속성 저항 UI 갱신
	#ifdef VIEW_SPECIAL_OPTIONDATA
	UpdateResistanceStatUI(socketData);
	#else
	UpdateResistanceStatUI();
	#endif //VIEW_SPECIAL_OPTIONDATA

	//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
}

	//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업

/*inline*/
int CX2UICharInfo::GetMyLevel() const
{
	return g_pData->GetSelectUnitLevel();
}

/*inline*/
float CX2UICharInfo::GetDamageDecrement( const float fTotalDefValue_ ) const
{
	// 기본적인 스탯만 가지고 계산했을 때의 레벨별 방어력 (기존에 이런 공식으로 되어있었음...)
	const int iBasicDefPerLevel = 348 + 62 * GetMyLevel();

	float fDamageDecrement = fTotalDefValue_ / iBasicDefPerLevel;

	if( 0.0f > fDamageDecrement )
		fDamageDecrement = 0.f;
	else if ( 99.99f < fDamageDecrement )
		fDamageDecrement = 99.99f;
	
	return fDamageDecrement;
}

/** @function : UpdateBasicStatUI
	@brief : HP, 물공, 마공, 물방, 마방 UI 갱신
	@param : 갱신된 정보가 저장된 SocketData
*/
void CX2UICharInfo::UpdateBasicStatUI( const CX2SocketItem::SocketData& socketData_ )		
{
	WCHAR wszBuffer[MAX_PATH] = {0, };

	// HP
	// 옵션 수치화 작업 이후에는 유저 정보창에 BaseHP와 추가 HP를 분리해서 표시 하지 않음

	const float fMaxHpIncrement = m_Stat.GetStat().m_fBaseHP * 0.5f;

	if ( fMaxHpIncrement < socketData_.m_Stat.m_ExtraStat.m_fIncreaseHPRate )
		StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", m_Stat.GetStat().m_fBaseHP + fMaxHpIncrement );
	else
		StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", m_Stat.GetStat().m_fBaseHP + socketData_.m_Stat.m_ExtraStat.m_fIncreaseHPRate );

	SetStatInfoStatic( L"g_pStaticinfo_hp", wszBuffer );

	const CX2Stat::Stat& kStat = m_Stat.GetStat();
	const CX2Stat::Stat::ExtraStat& extraStat = socketData_.m_Stat.m_ExtraStat;

	// 물공
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", kStat.m_fAtkPhysic + kStat.m_fAtkPhysic * extraStat.m_fIncreaseAtkPhysicRate );
	SetStatInfoStatic( L"g_pStaticinfo_p_attack", wszBuffer );

	// 마공
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", kStat.m_fAtkMagic + kStat.m_fAtkMagic * extraStat.m_fIncreaseAtkMagicRate );
	SetStatInfoStatic( L"g_pStaticinfo_m_attack", wszBuffer );
	
	// 물방
	const float fTotalDefPhysic = kStat.m_fDefPhysic + kStat.m_fDefPhysic * extraStat.m_fIncreaseDefPhysicRate;
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", fTotalDefPhysic );
	SetStatInfoStatic( L"g_pStaticinfo_p_defence", wszBuffer );
	// 물방 툴팁
	SetStatInfoGuideDesc( L"Button_PhysicDefence", GET_REPLACED_STRING( ( STR_ID_13570, "f", GetDamageDecrement( fTotalDefPhysic ) * 100 ) ) );

	// 마방
	const float fTotalDefMagic = kStat.m_fDefMagic + kStat.m_fDefMagic * extraStat.m_fIncreaseDefMagicRate;
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", kStat.m_fDefMagic + kStat.m_fDefMagic * extraStat.m_fIncreaseDefMagicRate );
	SetStatInfoStatic( L"g_pStaticinfo_m_defence", wszBuffer );
	// 마방 툴팁
	SetStatInfoGuideDesc( L"Button_MagicDefence", GET_REPLACED_STRING( ( STR_ID_13571, "f", GetDamageDecrement( fTotalDefMagic ) * 100 ) ) );
}

#endif SERV_IRUHADEV_JUNE_CHARINFO_STATS
/** @function : UpdateOptionStatUI
	@brief : 크리티컬, 추가데미지, 명중, 각성충전속도, 각성지속시간, 회피, 데미지감소, 동작속도, 이동력, 점프력
	@param : 갱신된 정보가 저장된 SocketData
*/
#ifndef MODIFY_CHARACTER_STAT_INFO
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::UpdateOptionStatUI( const CX2SocketItem::SocketData& socketData_ )
{
	// June 32919430
	const int iMyLevel = GetMyLevel();
	WCHAR wszBuffer[MAX_PATH] = {0, };

	if ( NULL == m_pDLGCharInfo || NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		NULL == m_pDLGCharInfo->GetControl( L"g_pStaticInfo2" ) )
		return;

	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();
	const CX2Unit::UNIT_TYPE eUnitType = ( NULL != pMyUnit->GetUnitTemplet() ) ? pMyUnit->GetType() : CX2Unit::UT_NONE;
	wstring wstrGuide;

	// Critical
	const float fPercentCritical = socketData_.m_fPercentCritical;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", ( 0.f > fPercentCritical ) ? 0.f : fPercentCritical );
	SetStatInfoStatic( L"Static_Critical", wszBuffer );
	float fFinalCritical = static_cast<float>( CX2SocketItem::GetFinalCriticalPercent( fPercentCritical, iMyLevel ) * 100.0 );
	if ( 0.f > fFinalCritical )
		fFinalCritical = 0.f;
	wstrGuide = GET_REPLACED_STRING( ( STR_ID_12785, "f", fFinalCritical ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalCriticalCoefficient", fPercentCritical, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Critical", wstrGuide.c_str() );

	// Add: the extra damage range for physical, then magical attack
	const float fAdditionalAttack = socketData_.m_fAdditionalAttackValue;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fAdditionalAttack );
	SetStatInfoStatic( L"Static_Add", wszBuffer );
	{
		const float fFinalAdditionalAttack = CX2SocketItem::GetFinalAdditionalAttackValue( fAdditionalAttack, iMyLevel );
		const double dMinLevelRate = pow( 1.0, 0.65 );
		const double dMaxLevelRate = pow( 50.0, 0.65 );
		const double dAtkPhysic = static_cast<float>( m_Stat.GetStat().m_fAtkPhysic ) * 1.5;
		const double dAtkMagic = static_cast<float>( m_Stat.GetStat().m_fAtkMagic ) * 1.5;
		wstrGuide = GET_STRING( STR_ID_30051 );
		wstrGuide += GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( dAtkPhysic * dMinLevelRate * fFinalAdditionalAttack ),
			static_cast<int>( dMaxLevelRate * dAtkPhysic * fFinalAdditionalAttack ) ) );
		wstrGuide += L"\n";
		wstrGuide += GET_STRING( STR_ID_30052 );
		wstrGuide += GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( dAtkMagic * dMinLevelRate * fFinalAdditionalAttack ),
			static_cast<int>( dMaxLevelRate * dAtkMagic * fFinalAdditionalAttack ) ) );
		wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalAdditionalAttackCoefficient", fAdditionalAttack, iMyLevel ) );
		SetStatInfoGuideDesc( L"Button_Add", wstrGuide.c_str() );
	}

	// Charge ( Add: DP charge )
	const float fHyperModeChargeRate = socketData_.m_fHyperModeChargeRate;
	CKTDGUIStatic* pStaticCharge = static_cast<CKTDGUIStatic*>( m_pDLGCharInfo->GetControl( L"Static_Charge" ) );
	if ( NULL == pStaticCharge )
		return;
	const int iChargeStringID = ShowHyperModePicture( pStaticCharge, CX2Unit::UT_ADD == eUnitType, STR_ID_13556, STR_ID_29871 );
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fHyperModeChargeRate );
	SetStatInfoStatic( L"Static_Charge", wszBuffer );
	const float fFinalCharge = static_cast<float>( CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( fHyperModeChargeRate, iMyLevel ) * 100.0 );
	wstrGuide = GET_REPLACED_STRING( ( iChargeStringID, "f", fFinalCharge ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalHyperGageChargeSpeedCoefficient", fHyperModeChargeRate, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Charge", wstrGuide.c_str() );

	// Time ( Add: DP use )
	const float fHyperModeTime = socketData_.m_fHyperModeTime;
	CKTDGUIStatic* pStaticTime = static_cast<CKTDGUIStatic*>( m_pDLGCharInfo->GetControl( L"Static_Time" ) );
	if ( NULL == pStaticTime )
		return;
	const int iTimeStringID = ShowHyperModePicture( pStaticTime, CX2Unit::UT_ADD == eUnitType, STR_ID_13557, STR_ID_29872 );
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%3.0f%%", fHyperModeTime );
	SetStatInfoStatic( L"Static_Time", wszBuffer );
	SetStatInfoGuideDesc( L"Button_Time", GET_REPLACED_STRING( ( iTimeStringID, "f", fHyperModeTime ) ) );

	// Decline
	const float fAdditionalDefence = socketData_.m_fAdditionalDefenceValue;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fAdditionalDefence );
	SetStatInfoStatic( L"Static_Decline", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( STR_ID_12788, "f", CX2SocketItem::GetFinalAdditionalDefencePercent( fAdditionalDefence, iMyLevel ) * 100.0 ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalAdditionalDefenceCoefficient", fAdditionalDefence, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Decline", wstrGuide.c_str() );

	// Moving ( animation speed )
	const float fAnimationSpeedRate = socketData_.m_fAnimationSpeedRate;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fAnimationSpeedRate );
	SetStatInfoStatic( L"Static_Moving", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( STR_ID_12787, "f", CX2SocketItem::GetFinalAnimationSpeedPercent( fAnimationSpeedRate, iMyLevel ) * 100.0 ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalAnimationSpeedCoefficient", fAnimationSpeedRate, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Moving", wstrGuide.c_str() );

	// Speed ( move speed )
	const float fMoveSpeed = socketData_.m_fMoveSpeed;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fMoveSpeed );
	SetStatInfoStatic( L"Static_Speed", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( STR_ID_12790, "f", CX2SocketItem::GetFinalMoveSpeedPercent( fMoveSpeed, iMyLevel ) * 100.0 ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalMoveSpeedCoefficient", fMoveSpeed, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Speed", wstrGuide.c_str() );

	// Jump
	const float fJumpSpeed = socketData_.m_fJumpSpeed;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fJumpSpeed );
	SetStatInfoStatic( L"Static_Jump", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( STR_ID_12791, "f", CX2SocketItem::GetFinalJumpSpeedPercent( fJumpSpeed, iMyLevel ) * 100.0 ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalJumpSpeedCoefficient", fJumpSpeed, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Jump", wstrGuide.c_str() );

	// Maximize: the rate, then the chance to roll the maximum ( 1 - erfc( 2 ( rate - 1 ) / sqrt( 2 ) ) / 2 )
	const float fDamageMaximizeRate = socketData_.m_fDamageMaximizeRate;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fDamageMaximizeRate );
	SetStatInfoStatic( L"Static_Maximize", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( 36703, "f", CX2SocketItem::GetFinalDamageMaximizeRate( fDamageMaximizeRate, iMyLevel ) * 100.0 ) );
	{
		// June also calls the final rate and CalculateIntermediateDamageMaximizeRate here and drops both results
		const float fFinalMaximizeRate = CX2SocketItem::GetFinalDamageMaximizeRate( fDamageMaximizeRate, g_pData->GetSelectUnitLevel() );
		const float fNormalized = ( ( fFinalMaximizeRate - 1.f ) + ( fFinalMaximizeRate - 1.f ) ) / static_cast<float>( sqrt( 2.0 ) );
		const float fMaximizeChance = static_cast<float>( 1.0 - boost::math::erfc( fNormalized ) * 0.5 );
		wstrGuide += L"\n";
		wstrGuide += GET_REPLACED_STRING( ( 39433, "f", fMaximizeChance * 100.0 ) );
	}
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalDamageMaximizeCoefficient", fDamageMaximizeRate, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_Maximize", wstrGuide.c_str() );

	// MP gain by attack
	const float fMPIncByAttack = socketData_.m_fMPIncByAttack;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fMPIncByAttack );
	SetStatInfoStatic( L"Static_MpIncByAttack", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( 36701, "f", CX2SocketItem::GetFinalMpIncPercentByAttack( fMPIncByAttack, iMyLevel ) * 100.0 ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalMpIncCoefficientByAttack", fMPIncByAttack, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_MpIncByAttack", wstrGuide.c_str() );

	// MP gain when attacked
	const float fMPIncByAttacked = socketData_.m_fMPIncByAttacked;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fMPIncByAttacked );
	SetStatInfoStatic( L"Static_MpIncByAttacked", wszBuffer );
	wstrGuide = GET_REPLACED_STRING( ( 36702, "f", CX2SocketItem::GetFinalMpIncPercentByAttacked( fMPIncByAttacked, iMyLevel ) * 100.0 ) );
	wstrGuide += GetCoefficientGuide( GetFinalCoefficient( "CalculateFinalMpIncCoefficientByAttacked", fMPIncByAttacked, iMyLevel ) );
	SetStatInfoGuideDesc( L"Button_MpIncByAttacked", wstrGuide.c_str() );

	// Specialization A
	const float fSpecializationA = socketData_.m_fCharacterSpecializationA;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fSpecializationA );
	SetStatInfoStatic( L"Static_SpecializationA", wszBuffer );
	{
		const float fFinalRate = CX2SocketItem::SpecializationFinal( eUnitType, true )( fSpecializationA, iMyLevel );
		const double dPercent = fFinalRate * 100.0;
		switch ( eUnitType )
		{
		case CX2Unit::UT_ELSWORD:	wstrGuide = GET_REPLACED_STRING( ( 36704, "f", dPercent ) );	break;
		case CX2Unit::UT_ARME:		wstrGuide = GET_REPLACED_STRING( ( 36706, "f", dPercent ) );	break;
		case CX2Unit::UT_LIRE:		wstrGuide = GET_REPLACED_STRING( ( 36708, "f", dPercent ) );	break;
		case CX2Unit::UT_RAVEN:		wstrGuide = GET_REPLACED_STRING( ( 36710, "f", dPercent ) );	break;
		case CX2Unit::UT_EVE:		wstrGuide = GET_REPLACED_STRING( ( 36712, "f", dPercent ) );	break;
		case CX2Unit::UT_CHUNG:		wstrGuide = GET_REPLACED_STRING( ( 36714, "f", dPercent ) );	break;
		case CX2Unit::UT_ARA:
			wstrGuide = GET_REPLACED_STRING( ( 36716, "fffff", dPercent * 0.5, dPercent * static_cast<double>( 0.71f ), dPercent,
				dPercent * static_cast<double>( 1.41f ), dPercent + dPercent ) );
			break;
		case CX2Unit::UT_ELESIS:	wstrGuide = GET_REPLACED_STRING( ( 36718, "f", dPercent ) );	break;
		case CX2Unit::UT_ADD:		wstrGuide = GET_REPLACED_STRING( ( 36720, "f", dPercent ) );	break;
		case CX2Unit::UT_LUCIEL:	wstrGuide = GET_REPLACED_STRING( ( 36722, "f", fFinalRate * 10.0 ) );	break;
		case CX2Unit::UT_ROSE:		wstrGuide = GET_REPLACED_STRING( ( 38430, "f", dPercent * static_cast<double>( 0.6f ) ) );	break;
		case CX2Unit::UT_AIN:		wstrGuide = GET_REPLACED_STRING( ( 43546, "f", dPercent ) );	break;
		default:					wstrGuide = L"";	break;
		}
		wstrGuide += GetCoefficientGuide( GetFinalSpecializationCoefficient( eUnitType, true, fSpecializationA, iMyLevel ) );
		SetStatInfoGuideDesc( L"Button_SpecializationA", wstrGuide.c_str() );
	}

	// Specialization B
	const float fSpecializationB = socketData_.m_fCharacterSpecializationB;
	StringCchPrintfW( wszBuffer, MAX_PATH, L"%5.0f", fSpecializationB );
	SetStatInfoStatic( L"Static_SpecializationB", wszBuffer );
	{
		const float fFinalRate = CX2SocketItem::SpecializationFinal( eUnitType, false )( fSpecializationB, iMyLevel );
		const double dPercent = fFinalRate * 100.0;
		switch ( eUnitType )
		{
		case CX2Unit::UT_ELSWORD:	wstrGuide = GET_REPLACED_STRING( ( 36705, "f", dPercent ) );	break;
		case CX2Unit::UT_ARME:		wstrGuide = GET_REPLACED_STRING( ( 36707, "f", dPercent ) );	break;
		case CX2Unit::UT_LIRE:
			wstrGuide = GET_REPLACED_STRING( ( 36709, "fff", dPercent / 3.0, dPercent / 3.0 + dPercent / 3.0, dPercent ) );
			break;
		case CX2Unit::UT_RAVEN:		wstrGuide = GET_REPLACED_STRING( ( 36711, "f", dPercent ) );	break;
		case CX2Unit::UT_EVE:		wstrGuide = GET_REPLACED_STRING( ( 36713, "f", dPercent ) );	break;
		case CX2Unit::UT_CHUNG:		wstrGuide = GET_REPLACED_STRING( ( 36715, "f", dPercent ) );	break;
		case CX2Unit::UT_ARA:		wstrGuide = GET_REPLACED_STRING( ( 36717, "f", fFinalRate * 20.0 ) );	break;
		case CX2Unit::UT_ELESIS:	wstrGuide = GET_REPLACED_STRING( ( 36719, "f", dPercent ) );	break;
		case CX2Unit::UT_ADD:		wstrGuide = GET_REPLACED_STRING( ( 36721, "f", fFinalRate * 50.0 ) );	break;
		case CX2Unit::UT_LUCIEL:	wstrGuide = GET_REPLACED_STRING( ( 36723, "f", dPercent ) );	break;
		case CX2Unit::UT_ROSE:		wstrGuide = GET_REPLACED_STRING( ( 38431, "f", dPercent ) );	break;
		case CX2Unit::UT_AIN:		wstrGuide = GET_REPLACED_STRING( ( 43547, "f", dPercent ) );	break;
		default:					wstrGuide = L"";	break;
		}
		wstrGuide += GetCoefficientGuide( GetFinalSpecializationCoefficient( eUnitType, false, fSpecializationB, iMyLevel ) );
		SetStatInfoGuideDesc( L"Button_SpecializationB", wstrGuide.c_str() );
	}

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
	CX2OfflineLog::Server( L"[JUNE_CHARINFO] STATS option crit %.0f add %.0f charge %.0f time %.0f decline %.0f moving %.0f speed %.0f jump %.0f maximize %.0f mpatk %.0f mpatked %.0f specA %.0f specB %.0f",
		fPercentCritical, fAdditionalAttack, fHyperModeChargeRate, fHyperModeTime, fAdditionalDefence, fAnimationSpeedRate, fMoveSpeed, fJumpSpeed,
		fDamageMaximizeRate, fMPIncByAttack, fMPIncByAttacked, fSpecializationA, fSpecializationB );
#endif SERV_IRUHADEV_JUNE_CHARINFO_DEBUG
}
#else SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::UpdateOptionStatUI( const CX2SocketItem::SocketData& socketData_ )
{
	const int iMyLevel = GetMyLevel();
	WCHAR wszBuffer[MAX_PATH] = {0, };

#ifdef SERV_9TH_NEW_CHARACTER // 김태환
	if( NULL == m_pDLGCharInfo ||
		NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>( m_pDLGCharInfo->GetControl( L"g_pStaticInfo2" ) );

	if ( NULL == pStatic )
		return;

	CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
#endif //SERV_9TH_NEW_CHARACTER
	
	// 크리티컬
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fPercentCritical );
	SetStatInfoStatic( L"Static_Critical", wszBuffer );

	// 크리티컬 툴팁
	if ( 40 < CX2UICharInfo::GetIntermediateCriticalValue( socketData_.m_fPercentCritical, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12785, "f", CX2SocketItem::GetFinalCriticalPercent( socketData_.m_fPercentCritical, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Critical", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Critical", GET_REPLACED_STRING( ( STR_ID_12785, "f", CX2SocketItem::GetFinalCriticalPercent( socketData_.m_fPercentCritical, iMyLevel ) * 100 ) ) );
	
	// 추가데미지
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAdditionalAttackValue );
	SetStatInfoStatic( L"Static_Add", wszBuffer );

#ifdef ADJUST_BUFF_CALCULATE
	// 추가데미지 툴팁 (1배율~50배율)
	if ( 40 < CX2UICharInfo::GetIntermediateAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) )
	{
		const float fFinalAdditionalAttackValue = CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel );
		float fStat = m_Stat.GetStat().m_fAtkPhysic;
		wstring wstrTooTip = GET_STRING(STR_ID_30051);
		wstrTooTip += GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( fStat * 1.5 * pow( 1, 0.65 ) * fFinalAdditionalAttackValue ), static_cast<int>( fStat * 1.5 * pow( 50, 0.65 ) * fFinalAdditionalAttackValue ) ) );
		wstrTooTip += L"\n";
		fStat = m_Stat.GetStat().m_fAtkMagic;
		wstrTooTip += GET_STRING(STR_ID_30052);
		wstrTooTip += GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( fStat * 1.5 * pow( 1, 0.65 ) * fFinalAdditionalAttackValue ), static_cast<int>( fStat * 1.5 * pow( 50, 0.65 ) * fFinalAdditionalAttackValue ) ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Add", wstrTooTip.c_str() );
	}
	else
	{
		const float fFinalAdditionalAttackValue = CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel );
		float fStat = m_Stat.GetStat().m_fAtkPhysic;
		wstring wstrTooTip = GET_STRING(STR_ID_30051);
		wstrTooTip += GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( fStat * 1.5 * pow( 1, 0.65 ) * fFinalAdditionalAttackValue ), static_cast<int>( fStat * 1.5 * pow( 50, 0.65 ) * fFinalAdditionalAttackValue ) ) );
		wstrTooTip += L"\n";
		fStat = m_Stat.GetStat().m_fAtkMagic;
		wstrTooTip += GET_STRING(STR_ID_30052);
		wstrTooTip += GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( fStat * 1.5 * pow( 1, 0.65 ) * fFinalAdditionalAttackValue ), static_cast<int>( fStat * 1.5 * pow( 50, 0.65 ) * fFinalAdditionalAttackValue ) ) );

		SetStatInfoGuideDesc( L"Button_Add", wstrTooTip.c_str() );
	}
#else //ADJUST_BUFF_CALCULATE
	// 추가데미지 툴팁 (1배율~50배율)
	if ( 40 < CX2UICharInfo::GetIntermediateAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( 1.5 * pow( 1, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ), static_cast<int>( 1.5 * pow( 50, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ) ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Add", wstrTooTip.c_str() );
	}
	else
	{
		SetStatInfoGuideDesc( L"Button_Add", 
			GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( 1.5 * pow( 1, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ), static_cast<int>( 1.5 * pow( 50, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ) ) ) );
	}
#endif //ADJUST_BUFF_CALCULATE

	// 명중
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAntiEvadePercent );
	SetStatInfoStatic( L"Static_Accuracy", wszBuffer );
	
	// 명중 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateAnitEvadeValue( socketData_.m_fAntiEvadePercent, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12786, "f", CX2SocketItem::GetFinalAnitEvadePercent( socketData_.m_fAntiEvadePercent, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Accuracy", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Accuracy", GET_REPLACED_STRING( ( STR_ID_12786, "f", CX2SocketItem::GetFinalAnitEvadePercent( socketData_.m_fAntiEvadePercent, iMyLevel ) * 100 ) ) );
	
	// 각성 충전 속도
#ifdef SERV_9TH_NEW_CHARACTER // 김태환 //JHKang
	UINT uStrID = STR_ID_13556;
	float fHyperModeChargeRate = socketData_.m_fHyperModeChargeRate;

	CKTDGUIControl::CPictureData* pChargeRate	= pStatic->GetPicture( 3 );
	CKTDGUIControl::CPictureData* pDPcharge		= pStatic->GetPicture( 10 );

	if ( NULL != pChargeRate && NULL != pDPcharge )
	{
		if ( CX2Unit::UT_ADD == eUnitType )
		{
			pChargeRate->SetShow( false );
			pDPcharge->SetShow(true );
			uStrID = STR_ID_29871;
		}
		else
		{
			pChargeRate->SetShow( true );
			pDPcharge->SetShow( false );
		}
	}

	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", fHyperModeChargeRate );
	SetStatInfoStatic( L"Static_Charge", wszBuffer );

	// 최대 표기 값 제한.
	//const float fLimitValue = CX2Unit::UT_ADD == eUnitType ? 25.f: 50.f;
	// 최대 표기 값 애드전용 예외 제거
	const float fLimitValue = 50.f;		

	// 최대 표기값 제한 한 결과 값
	const float fResultHyperModeChargeIncreaseRate = 
		min( fLimitValue, CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( fHyperModeChargeRate, iMyLevel ) * 100 ) ;
	// 각성 충전 속도 툴팁
	if ( 23 < CX2UICharInfo::GetIntermediateHyperGageChargeSpeedValue( fHyperModeChargeRate, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( uStrID, "f", fResultHyperModeChargeIncreaseRate ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Charge", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Charge", GET_REPLACED_STRING( ( uStrID, "f", fResultHyperModeChargeIncreaseRate ) ) );
#else // SERV_9TH_NEW_CHARACTER
	// 각성 충전 속도
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fHyperModeChargeRate );
	SetStatInfoStatic( L"Static_Charge", wszBuffer );
	
	// 각성 충전 속도 툴팁
	if ( 23 < CX2UICharInfo::GetIntermediateHyperGageChargeSpeedValue( socketData_.m_fHyperModeChargeRate, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_13556, "f", CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( socketData_.m_fHyperModeChargeRate, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Charge", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Charge", GET_REPLACED_STRING( ( STR_ID_13556, "f", CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( socketData_.m_fHyperModeChargeRate, iMyLevel ) * 100 ) ) );
#endif // SERV_9TH_NEW_CHARACTER


	// 각성 지속 시간
#ifdef SERV_9TH_NEW_CHARACTER // 김태환 //JHKang
	uStrID = STR_ID_13557;
	float fHyperModeTime = socketData_.m_fHyperModeTime;

	CKTDGUIControl::CPictureData* pHyperModeTime = pStatic->GetPicture( 4 );
	CKTDGUIControl::CPictureData* pDPConsume	 = pStatic->GetPicture( 11 );

	if ( NULL != pHyperModeTime && NULL != pDPConsume )
	{
		if ( CX2Unit::UT_ADD == eUnitType )
		{
			pHyperModeTime->SetShow( false );
			pDPConsume->SetShow( true );
			uStrID = STR_ID_29872;
		}
		else
		{
			pHyperModeTime->SetShow( true );
			pDPConsume->SetShow( false );
		}
	}

	StringCchPrintf( wszBuffer, MAX_PATH, L"%3.0f%%", fHyperModeTime );
	SetStatInfoStatic( L"Static_Time", wszBuffer );
	// 각성 지속 시간 툴팁
	SetStatInfoGuideDesc( L"Button_Time", GET_REPLACED_STRING( ( uStrID, "f", fHyperModeTime ) ) );
#else // SERV_9TH_NEW_CHARACTER
	// 각성 지속 시간
	StringCchPrintf( wszBuffer, MAX_PATH, L"%3.0f%%", socketData_.m_fHyperModeTime );
	SetStatInfoStatic( L"Static_Time", wszBuffer );
	// 각성 지속 시간 툴팁
	SetStatInfoGuideDesc( L"Button_Time", GET_REPLACED_STRING( ( STR_ID_13557, "f", socketData_.m_fHyperModeTime ) ) );
#endif // SERV_9TH_NEW_CHARACTER


	// 회피
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fEvadePercent );
	SetStatInfoStatic( L"Static_Evade", wszBuffer );
	
	// 회피 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateEvadeValue( socketData_.m_fEvadePercent, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12789, "f", CX2SocketItem::GetFinalEvadePercent( socketData_.m_fEvadePercent, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Evade", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Evade", GET_REPLACED_STRING( ( STR_ID_12789, "f", CX2SocketItem::GetFinalEvadePercent( socketData_.m_fEvadePercent, iMyLevel ) * 100 ) ) );

	// 데미지 감소
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAdditionalDefenceValue );
	SetStatInfoStatic( L"Static_Decline", wszBuffer );
	
	// 데미지 감소 툴팁
	if ( 23 < CX2UICharInfo::GetIntermediateAdditionalDefenceValue( socketData_.m_fAdditionalDefenceValue, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12788, "f", CX2SocketItem::GetFinalAdditionalDefencePercent( socketData_.m_fAdditionalDefenceValue, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Decline", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Decline", GET_REPLACED_STRING( ( STR_ID_12788, "f", CX2SocketItem::GetFinalAdditionalDefencePercent( socketData_.m_fAdditionalDefenceValue, iMyLevel ) * 100 ) ) );
	
	// 속도
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAnimationSpeedRate );
	SetStatInfoStatic( L"Static_Moving", wszBuffer );
	
	// 속도 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateAnimationSpeedValue( socketData_.m_fAnimationSpeedRate, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12787, "f", CX2SocketItem::GetFinalAnimationSpeedPercent( socketData_.m_fAnimationSpeedRate, iMyLevel ) * 100.0f ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Moving", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Moving", GET_REPLACED_STRING( ( STR_ID_12787, "f", CX2SocketItem::GetFinalAnimationSpeedPercent( socketData_.m_fAnimationSpeedRate, iMyLevel ) * 100.0f ) ) );
	
	// 이동 속도
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fMoveSpeed );
	SetStatInfoStatic( L"Static_Speed", wszBuffer );
	
	// 이동 속도 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateMoveSpeedValue( socketData_.m_fMoveSpeed, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12790, "f", CX2SocketItem::GetFinalMoveSpeedPercent( socketData_.m_fMoveSpeed, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Speed", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Speed", GET_REPLACED_STRING( ( STR_ID_12790, "f", CX2SocketItem::GetFinalMoveSpeedPercent( socketData_.m_fMoveSpeed, iMyLevel ) * 100 ) ) );
	
	// 점프력
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fJumpSpeed );
	SetStatInfoStatic( L"Static_Jump", wszBuffer );
	
	// 점프력 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateJumpSpeedValue( socketData_.m_fJumpSpeed, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12791, "f", CX2SocketItem::GetFinalJumpSpeedPercent( socketData_.m_fJumpSpeed, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Jump", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Jump", GET_REPLACED_STRING( ( STR_ID_12791, "f", CX2SocketItem::GetFinalJumpSpeedPercent( socketData_.m_fJumpSpeed, iMyLevel ) * 100 ) ) );
}
#endif SERV_IRUHADEV_JUNE_CHARINFO_STATS
#else
void CX2UICharInfo::UpdateOptionStatUI( const CX2SocketItem::SocketData& socketData_ )
{
	const int iMyLevel = GetMyLevel();
	WCHAR wszBuffer[MAX_PATH] = {0, };

	// 크리티컬
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fPercentCritical );
	SetStatInfoStatic( L"Static_Critical", wszBuffer );

	// 크리티컬 툴팁
	if ( 40 < CX2UICharInfo::GetIntermediateCriticalValue( socketData_.m_fPercentCritical, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12785, "f", CX2SocketItem::GetFinalCriticalPercent( socketData_.m_fPercentCritical, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Critical", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Critical", GET_REPLACED_STRING( ( STR_ID_12785, "f", CX2SocketItem::GetFinalCriticalPercent( socketData_.m_fPercentCritical, iMyLevel ) * 100 ) ) );

	// 추가데미지
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAdditionalAttackValue );
	SetStatInfoStatic( L"Static_Add", wszBuffer );

	// 추가데미지 툴팁 (1배율~50배율)
	if ( 40 < CX2UICharInfo::GetIntermediateAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( 1.5 * pow( 1, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ), static_cast<int>( 1.5 * pow( 50, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ) ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Add", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Add", 
		GET_REPLACED_STRING( ( STR_ID_13555, "ii", static_cast<int>( 1.5 * pow( 1, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ), static_cast<int>( 1.5 * pow( 50, 0.65 ) * CX2SocketItem::GetFinalAdditionalAttackValue( socketData_.m_fAdditionalAttackValue, iMyLevel ) ) ) ) );

	// 명중
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAntiEvadePercent );
	SetStatInfoStatic( L"Static_Accuracy", wszBuffer );

	// 명중 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateAnitEvadeValue( socketData_.m_fAntiEvadePercent, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12786, "f", CX2SocketItem::GetFinalAnitEvadePercent( socketData_.m_fAntiEvadePercent, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Accuracy", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Accuracy", GET_REPLACED_STRING( ( STR_ID_12786, "f", CX2SocketItem::GetFinalAnitEvadePercent( socketData_.m_fAntiEvadePercent, iMyLevel ) * 100 ) ) );

	// 각성 충전 속도
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fHyperModeChargeRate );
	SetStatInfoStatic( L"Static_Charge", wszBuffer );

	// 각성 충전 속도 툴팁
	if ( 23 < CX2UICharInfo::GetIntermediateHyperGageChargeSpeedValue( socketData_.m_fHyperModeChargeRate, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_13556, "f", CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( socketData_.m_fHyperModeChargeRate, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Charge", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Charge", GET_REPLACED_STRING( ( STR_ID_13556, "f", CX2SocketItem::GetFinalHyperGageChargeSpeedPercent( socketData_.m_fHyperModeChargeRate, iMyLevel ) * 100 ) ) );

	// 각성 지속 시간
	StringCchPrintf( wszBuffer, MAX_PATH, L"%3.0f%%", socketData_.m_fHyperModeTime );
	SetStatInfoStatic( L"Static_Time", wszBuffer );
	// 각성 지속 시간 툴팁
	SetStatInfoGuideDesc( L"Button_Time", GET_REPLACED_STRING( ( STR_ID_13557, "f", socketData_.m_fHyperModeTime ) ) );

	// 회피
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fEvadePercent );
	SetStatInfoStatic( L"Static_Evade", wszBuffer );
	
	// 회피 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateEvadeValue( socketData_.m_fEvadePercent, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_26150, "ff", CX2SocketItem::GetFinalEvadePercent( socketData_.m_fEvadePercent, iMyLevel ) * 100 , m_fAddEvadePercent ));
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Evade", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Evade", GET_REPLACED_STRING( ( STR_ID_26150, "ff", CX2SocketItem::GetFinalEvadePercent( socketData_.m_fEvadePercent, iMyLevel ) * 100 , m_fAddEvadePercent ) ));

	// 데미지 감소
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAdditionalDefenceValue );
	SetStatInfoStatic( L"Static_Decline", wszBuffer );

	// 데미지 감소 툴팁
	if ( 23 < CX2UICharInfo::GetIntermediateAdditionalDefenceValue( socketData_.m_fAdditionalDefenceValue, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12788, "f", CX2SocketItem::GetFinalAdditionalDefencePercent( socketData_.m_fAdditionalDefenceValue, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Decline", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Decline", GET_REPLACED_STRING( ( STR_ID_12788, "f", CX2SocketItem::GetFinalAdditionalDefencePercent( socketData_.m_fAdditionalDefenceValue, iMyLevel ) * 100 ) ) );

	// 속도
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fAnimationSpeedRate );
	SetStatInfoStatic( L"Static_Moving", wszBuffer );

	// 속도 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateAnimationSpeedValue( socketData_.m_fAnimationSpeedRate, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12787, "f", CX2SocketItem::GetFinalAnimationSpeedPercent( socketData_.m_fAnimationSpeedRate, iMyLevel ) * 100.0f ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Moving", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Moving", GET_REPLACED_STRING( ( STR_ID_12787, "f", CX2SocketItem::GetFinalAnimationSpeedPercent( socketData_.m_fAnimationSpeedRate, iMyLevel ) * 100.0f ) ) );

	// 이동 속도
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fMoveSpeed );
	SetStatInfoStatic( L"Static_Speed", wszBuffer );

	// 이동 속도 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateMoveSpeedValue( socketData_.m_fMoveSpeed, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12790, "f", CX2SocketItem::GetFinalMoveSpeedPercent( socketData_.m_fMoveSpeed, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Speed", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Speed", GET_REPLACED_STRING( ( STR_ID_12790, "f", CX2SocketItem::GetFinalMoveSpeedPercent( socketData_.m_fMoveSpeed, iMyLevel ) * 100 ) ) );

	// 점프력
	StringCchPrintf( wszBuffer, MAX_PATH, L"%5.0f", socketData_.m_fJumpSpeed );
	SetStatInfoStatic( L"Static_Jump", wszBuffer );

	// 점프력 툴팁
	if ( 20 < CX2UICharInfo::GetIntermediateJumpSpeedValue( socketData_.m_fJumpSpeed, iMyLevel ) )
	{
		wstring wstrTooTip = GET_REPLACED_STRING( ( STR_ID_12791, "f", CX2SocketItem::GetFinalJumpSpeedPercent( socketData_.m_fJumpSpeed, iMyLevel ) * 100 ) );
		wstrTooTip += GET_STRING( STR_ID_13645 );
		SetStatInfoGuideDesc( L"Button_Jump", wstrTooTip.c_str() );
	}
	else
		SetStatInfoGuideDesc( L"Button_Jump", GET_REPLACED_STRING( ( STR_ID_12791, "f", CX2SocketItem::GetFinalJumpSpeedPercent( socketData_.m_fJumpSpeed, iMyLevel ) * 100 ) ) );
}
#endif // MODIFY_CHARACTER_STAT_INFO

#ifndef MODIFY_CHARACTER_STAT_INFO
/** @function : UpdateOptionStatFromSpecialSkill
	@brief : 스킬에 의한 추가 스탯 적용
			(크리티컬, 추가데미지, 명중, 각성충전속도, 각성지속시간, 회피, 데미지감소, 동작속도, 이동력, 점프력)
	@param : 변경하기 전 정보가 저장된 SocketData
*/
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::UpdateOptionStatFromSpecialSkill( CX2SocketItem::SocketData& socketData_ )
{
	// June 32912a90
	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	const int iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_ECN_ENERGY_OF_CONCENTRATION, true, 0, true );
	if ( 0 < iSkillLevel && NULL != g_pData->GetSkillTree() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::RNSI_P_ECN_ENERGY_OF_CONCENTRATION );
		if ( NULL != pSkillTemplet )
		{
			const float fMixAtk = static_cast<float>( m_Stat.GetStat().m_fAtkMagic ) + static_cast<float>( m_Stat.GetStat().m_fAtkPhysic );
			socketData_.m_fPercentCritical = static_cast<float>( fMixAtk * 0.5 *
				pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ATK_BASE, iSkillLevel, false ) + socketData_.m_fPercentCritical );
		}
	}
}
#else SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::UpdateOptionStatFromSpecialSkill( CX2SocketItem::SocketData& socketData_ )
{

	const CX2UserSkillTree& UserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	int iSkillLevel = 0;
#ifdef BALANCE_CODE_NEMESIS_20121213
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION, true ); //코드 네메시스 - 집중의 기운
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION ); //코드 네메시스 - 집중의 기운
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0 )
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION );

		if( NULL != pSkillTemplet )
		{
			const float fCriticalRateValueAtkBase = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ATK_BASE, iSkillLevel );
			const float fMixAtk = (m_Stat.GetStat().m_fAtkMagic + m_Stat.GetStat().m_fAtkPhysic) / 2;

			float fPassiveCriticalValue = fMixAtk * fCriticalRateValueAtkBase;
			socketData_.m_fPercentCritical += fPassiveCriticalValue;
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION, iSkillLevel );

		if( NULL != pSkillTemplet )
		{
			const float fCriticalRateValueAtkBase = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ATK_BASE );
			const float fMixAtk = (m_Stat.GetStat().m_fAtkMagic + m_Stat.GetStat().m_fAtkPhysic) / 2;

			float fPassiveCriticalValue = fMixAtk * fCriticalRateValueAtkBase;
			socketData_.m_fPercentCritical += fPassiveCriticalValue;
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	} 
#endif //BALANCE_CODE_NEMESIS_20121213

#ifndef FIX_SKILL_BALANCE_AISHA_LENA //JHKang
#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
	iSkillLevel = UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LCR_SOFTBODY, true ); // 컴뱃 레인져 - 유연한 몸놀림
	CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
	if( iSkillLevel > 0 && ( CX2Unit::UC_LIRE_COMBAT_RANGER == eUnitClass || CX2Unit::UC_LIRE_WIND_SNEAKER == eUnitClass ) )
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LCR_SOFTBODY );

		if( NULL != pSkillTemplet )
		{
			const int iSkillLevel = UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LCR_SOFTBODY, true );

			const float fAddAvoidancePercent = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
			socketData_.m_fEvadePercent += fAddAvoidancePercent;
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LCR_SOFTBODY, iSkillLevel );

		if( NULL != pSkillTemplet )
		{
			const float fAddAvoidancePercent = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE );
			socketData_.m_fEvadePercent += fAddAvoidancePercent;
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}
#endif //UPGRADE_SKILL_SYSTEM_2013
#endif //FIX_SKILL_BALANCE_AISHA_LENA
}
#endif SERV_IRUHADEV_JUNE_CHARINFO_STATS

#else
void CX2UICharInfo::UpdateOptionStatFromSpecialSkill( CX2SocketItem::SocketData& socketData_ )
{
	if( NULL == g_pData || 
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	const CX2UserSkillTree& UserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;

	m_fAddEvadePercent = 0.f;

	UpdateOptionStatFromSpecialSkill_LENA( socketData_, UserSkillTree );
	UpdateOptionStatFromSpecialSkill_EVE( socketData_, UserSkillTree );
}

void CX2UICharInfo::UpdateOptionStatFromSpecialSkill_LENA(OUT CX2SocketItem::SocketData& socketData_, const CX2UserSkillTree& UserSkillTree_ )
{
	CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_NONE;

	 // 컴뱃 레인져 - 유연한 몸놀림
#ifndef FIX_SKILL_BALANCE_AISHA_LENA //JHKang
	eSkillID = CX2SkillTree::SI_P_LCR_SOFTBODY;
	{
		int iSkillLevel = UserSkillTree_.GetSkillLevel( eSkillID, true );
		{// 1.xx 형태의 배율 
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
			if( NULL != pSkillTemplet )
			{
				const float fAddAvoidancePercent = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
				m_fAddEvadePercent += ( fAddAvoidancePercent - 1.f ) * 100;
			}
		}
	}
#endif //FIX_SKILL_BALANCE_AISHA_LENA
}

void CX2UICharInfo::UpdateOptionStatFromSpecialSkill_EVE(OUT CX2SocketItem::SocketData& socketData_, const CX2UserSkillTree& UserSkillTree_)
{
	CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_NONE;
#ifdef BALANCE_CODE_NEMESIS_20121213
	eSkillID = CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION;
	{
		int iSkillLevel = UserSkillTree_.GetSkillLevel( eSkillID, true ); //코드 네메시스 - 집중의 기운

		if( iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
			if( NULL != pSkillTemplet )
			{
				const float fCriticalRateValueAtkBase = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ATK_BASE, iSkillLevel );
				const float fMixAtk = (m_Stat.GetStat().m_fAtkMagic + m_Stat.GetStat().m_fAtkPhysic) / 2;
				float fPassiveCriticalValue = fMixAtk * fCriticalRateValueAtkBase;
				socketData_.m_fPercentCritical += fPassiveCriticalValue;
			}
		} 
	}
#endif //BALANCE_CODE_NEMESIS_20121213
}
#endif // MODIFY_CHARACTER_STAT_INFO

/** @function : UpdateResistanceStatUI
	@brief : 불, 물, 자연, 바람, 빛, 어둠 속성 저항 UI 갱신
*/
#ifdef VIEW_SPECIAL_OPTIONDATA
void CX2UICharInfo::UpdateResistanceStatUI(const CX2SocketItem::SocketData& socketData_)
{
	// 속성저항
	const int iMaxResistance = 500;
	WCHAR wszBuffer[MAX_PATH] = {0, };	
	const CX2Stat::EnchantStat& enchantStat = g_pData->GetMyUser()->GetSelectUnit()->GetEnchantStat( true, m_bShowDungeonOption, m_bShowPVPOption );//캐릭터창 UI의 던전, 대전용 옵션 영향 받도록 수정
	
	// 불
	int iDefResistance = static_cast<int>( enchantStat.m_fDefBlaze ) ;
	StringCchPrintf( wszBuffer, MAX_PATH, L"  %d", iDefResistance );
	SetStatInfoStatic( L"g_pStaticInfo_fire", wszBuffer );
	SetStatInfoGuideDesc( L"opacity1", GET_REPLACED_STRING( ( STR_ID_13558, "ii", iDefResistance, iMaxResistance ) ) );

	// 물
	iDefResistance = static_cast<int>( enchantStat.m_fDefWater ) ;
	StringCchPrintf( wszBuffer, MAX_PATH, L"  %d", iDefResistance );
	SetStatInfoStatic( L"g_pStaticInfo_water", wszBuffer );
	SetStatInfoGuideDesc( L"opacity2", GET_REPLACED_STRING( ( STR_ID_13559, "ii", iDefResistance, iMaxResistance ) ) );

	// 자연
	iDefResistance = static_cast<int>( enchantStat.m_fDefNature ) ;
	StringCchPrintf( wszBuffer, MAX_PATH, L"  %d", iDefResistance );
	SetStatInfoStatic( L"g_pStaticInfo_nature", wszBuffer );
	SetStatInfoGuideDesc( L"opacity3", GET_REPLACED_STRING( ( STR_ID_13560, "ii", iDefResistance, iMaxResistance ) ) );

	// 바람
	iDefResistance = static_cast<int>( enchantStat.m_fDefWind ) ;
	StringCchPrintf( wszBuffer, MAX_PATH, L"  %d", iDefResistance );
	SetStatInfoStatic( L"g_pStaticInfo_wind", wszBuffer );
	SetStatInfoGuideDesc( L"opacity4", GET_REPLACED_STRING( ( STR_ID_13561, "ii", iDefResistance, iMaxResistance ) ) );
	
	// 빛
	iDefResistance = static_cast<int>( enchantStat.m_fDefLight ) ;
	StringCchPrintf( wszBuffer, MAX_PATH, L"  %d", iDefResistance );
	SetStatInfoStatic( L"g_pStaticInfo_light", wszBuffer );
	SetStatInfoGuideDesc( L"opacity5", GET_REPLACED_STRING( ( STR_ID_13562, "ii", iDefResistance, iMaxResistance ) ) );

	// 어둠
	iDefResistance = static_cast<int>( enchantStat.m_fDefDark ) ;
	StringCchPrintf( wszBuffer, MAX_PATH, L"  %d", iDefResistance );
	SetStatInfoStatic( L"g_pStaticInfo_dark", wszBuffer );
	SetStatInfoGuideDesc( L"opacity6", GET_REPLACED_STRING( ( STR_ID_13563, "ii", iDefResistance, iMaxResistance ) ) );
}
#else
void CX2UICharInfo::UpdateResistanceStatUI()
{
	// 속성저항
	const int iMaxResistance = 500;
	WCHAR wszBuffer[MAX_PATH] = {0, };
	const CX2Stat::EnchantStat& enchantStat = g_pData->GetMyUser()->GetSelectUnit()->GetEnchantStat( true );
	
	// 불
	StringCchPrintf( wszBuffer, MAX_PATH, L"%4.0f", enchantStat.m_fDefBlaze );
	SetStatInfoStatic( L"g_pStaticInfo_fire", wszBuffer );
	SetStatInfoGuideDesc( L"opacity1", GET_REPLACED_STRING( ( STR_ID_13558, "ii", static_cast<int>( enchantStat.m_fDefBlaze ), iMaxResistance ) ) );

	// 물
	StringCchPrintf( wszBuffer, MAX_PATH, L"%4.0f", enchantStat.m_fDefWater );
	SetStatInfoStatic( L"g_pStaticInfo_water", wszBuffer );
	SetStatInfoGuideDesc( L"opacity2", GET_REPLACED_STRING( ( STR_ID_13559, "ii", static_cast<int>( enchantStat.m_fDefWater ), iMaxResistance ) ) );

	// 자연
	StringCchPrintf( wszBuffer, MAX_PATH, L"%4.0f", enchantStat.m_fDefNature );
	SetStatInfoStatic( L"g_pStaticInfo_nature", wszBuffer );
	SetStatInfoGuideDesc( L"opacity3", GET_REPLACED_STRING( ( STR_ID_13560, "ii", static_cast<int>( enchantStat.m_fDefNature ), iMaxResistance ) ) );

	// 바람
	StringCchPrintf( wszBuffer, MAX_PATH, L"%4.0f", enchantStat.m_fDefWind );
	SetStatInfoStatic( L"g_pStaticInfo_wind", wszBuffer );
	SetStatInfoGuideDesc( L"opacity4", GET_REPLACED_STRING( ( STR_ID_13561, "ii", static_cast<int>( enchantStat.m_fDefWind ), iMaxResistance ) ) );

	// 빛
	StringCchPrintf( wszBuffer, MAX_PATH, L"%4.0f", enchantStat.m_fDefLight );
	SetStatInfoStatic( L"g_pStaticInfo_light", wszBuffer );
	SetStatInfoGuideDesc( L"opacity5", GET_REPLACED_STRING( ( STR_ID_13562, "ii", static_cast<int>( enchantStat.m_fDefLight ), iMaxResistance ) ) );

	// 어둠
	StringCchPrintf( wszBuffer, MAX_PATH, L"%4.0f", enchantStat.m_fDefDark );
	SetStatInfoStatic( L"g_pStaticInfo_dark", wszBuffer );
	SetStatInfoGuideDesc( L"opacity6", GET_REPLACED_STRING( ( STR_ID_13563, "ii", static_cast<int>( enchantStat.m_fDefDark ), iMaxResistance ) ) );
}
#endif //VIEW_SPECIAL_OPTIONDATA

/** @function : UpdateBasicStatFromSpecialSkill
	@brief : 스킬에서 올려주는 능력치를 갱신
*/
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::UpdateBasicStatFromSpecialSkill()
{
	// June 32913460; its 32358970( 1 ) / ( 2 ) attack and defence terms read June's per-unit ability map: 0 in March
	CX2Stat::Stat& kStat = m_Stat.AccessStat();
	float fBaseAtkPhysic = kStat.m_fAtkPhysic;
	float fBaseAtkMagic = kStat.m_fAtkMagic;
	const float fBaseDefPhysic = kStat.m_fDefPhysic;
	const float fBaseDefMagic = kStat.m_fDefMagic;

	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	if ( NULL == pSkillTree )
		return;
	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	const CX2SkillTree::SkillTemplet* pSkillTemplet = NULL;

	int iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_CIP_IRON_WILL, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_CIP_IRON_WILL ) ) )
		kStat.m_fAtkPhysic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_ATK_PHYSIC_REL, iSkillLevel ) * fBaseAtkPhysic + kStat.m_fAtkPhysic;

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION ) ) )
		kStat.m_fAtkMagic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) * fBaseAtkMagic + kStat.m_fAtkMagic;

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_EMK_IF_PHYSICUP_MAGIC_GET, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_EMK_IF_PHYSICUP_MAGIC_GET ) ) )
	{
		const CX2Stat::Stat kSkillStat = pSkillTemplet->GetSkillStatValue( iSkillLevel );
		kStat.m_fAtkPhysic = static_cast<float>( kSkillStat.m_fAtkPhysic ) + kStat.m_fAtkPhysic;
		kStat.m_fAtkMagic = static_cast<float>( kSkillStat.m_fAtkMagic ) + kStat.m_fAtkMagic;
		fBaseAtkPhysic += kSkillStat.m_fAtkPhysic;
		fBaseAtkMagic += kSkillStat.m_fAtkMagic;
	}

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_ERS_RUNE_MASTERY, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_ERS_RUNE_MASTERY ) ) )
		kStat.m_fAtkMagic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) * fBaseAtkMagic + kStat.m_fAtkMagic;

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_ERS_MAGIC_RESISTANCE, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_ERS_MAGIC_RESISTANCE ) ) )
		kStat.m_fDefMagic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_DEF_MAGIC_REL, iSkillLevel ) * fBaseDefMagic + kStat.m_fDefMagic;

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_COMMON_HYPER_AURA, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_COMMON_HYPER_AURA ) ) )
	{
		kStat.m_fAtkPhysic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_ATK_PHYSIC_REL, iSkillLevel ) * fBaseAtkPhysic + kStat.m_fAtkPhysic;
		kStat.m_fAtkMagic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) * fBaseAtkMagic + kStat.m_fAtkMagic;
		kStat.m_fDefPhysic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_DEF_PHYSIC_REL, iSkillLevel ) * fBaseDefPhysic + kStat.m_fDefPhysic;
		kStat.m_fDefMagic = GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_DEF_MAGIC_REL, iSkillLevel ) * fBaseDefMagic + kStat.m_fDefMagic;
	}

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_RF_PHYSIC_MAGIC_ATTACK, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_RF_PHYSIC_MAGIC_ATTACK ) ) )
	{
		const CX2Stat::Stat kSkillStat = pSkillTemplet->GetSkillStatValue( iSkillLevel );
		kStat.m_fAtkPhysic = static_cast<float>( kSkillStat.m_fAtkPhysic ) + kStat.m_fAtkPhysic;
		kStat.m_fAtkMagic = static_cast<float>( kSkillStat.m_fAtkMagic ) + kStat.m_fAtkMagic;
		fBaseAtkMagic += kSkillStat.m_fAtkMagic;		// June updates only the magic base here
	}

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_RRF_SPIRITUALIZED_FURY, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_RRF_SPIRITUALIZED_FURY ) ) )
		kStat.m_fAtkMagic = fBaseAtkMagic * GetPassiveIncreaseRate( pSkillTemplet, CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) + kStat.m_fAtkMagic;

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_ABM_FITNESS, true, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_ABM_FITNESS ) ) )
	{
		const CX2Stat::Stat kSkillStat = pSkillTemplet->GetSkillStatValue( iSkillLevel );
		kStat.m_fAtkPhysic = static_cast<float>( kSkillStat.m_fAtkPhysic ) + kStat.m_fAtkPhysic;
		kStat.m_fAtkMagic = static_cast<float>( kSkillStat.m_fAtkMagic ) + kStat.m_fAtkMagic;
	}

	// June reads this one with the base-level flag off and scales the running totals by ( rate - 1.0 ), no 5e-8
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::RNSI_P_RSR_OPEN_MIND, false, 0, true );
	if ( 0 < iSkillLevel && NULL != ( pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::RNSI_P_RSR_OPEN_MIND ) ) )
	{
		const float fAtkPhysic = kStat.m_fAtkPhysic;
		kStat.m_fAtkPhysic = static_cast<float>( fAtkPhysic * ( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_REL, iSkillLevel, false ) ) - 1.0 ) + fAtkPhysic );
		const float fAtkMagic = kStat.m_fAtkMagic;
		kStat.m_fAtkMagic = static_cast<float>( fAtkMagic * ( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel, false ) ) - 1.0 ) + fAtkMagic );
	}
}
#else SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::UpdateBasicStatFromSpecialSkill()
{
#ifdef	NEW_SKILL_TREE
	// 일부 스킬에 대해서 : 최종 스탯 수치에서 마지막으로 보정해준다
	CX2Stat TotalStat;
	TotalStat.InitStat();

	TotalStat.AddStat( m_Stat.GetStat(), true );
	
	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	int iSkillLevel = 0;

#ifndef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_POWERFUL_SOUL );
	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_POWERFUL_SOUL, iSkillLevel );
		if( pSkillTemplet != NULL )
		{
#ifdef BALANCE_RUNE_SLAYER_20130214
			// 물공 %만큼 마공증가
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkPhysic * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_TO_ATK_MAGIC );
#else
			// 마공 %만큼 물공증가
			m_Stat.AccessStat().m_fAtkPhysic += TotalStat.GetStat().m_fAtkMagic * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_TO_ATK_PHYSIC );
#endif //BALANCE_RUNE_SLAYER_20130214
		}
	}
#endif //UPGRADE_SKILL_SYSTEM_2013

#ifndef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_PHYSIC_HARMONY );
	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_PHYSIC_HARMONY, iSkillLevel );
		if( pSkillTemplet != NULL )
		{
			// 물공 %만큼 마공증가
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkPhysic * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_TO_ATK_MAGIC );
		}
	}

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_FIGHTER_SOUL );
	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_FIGHTER_SOUL, iSkillLevel );
		if( pSkillTemplet != NULL )
		{
			// 물공 %만큼 마공증가
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkPhysic * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_TO_ATK_MAGIC );
		}
	}
#ifdef SERV_RENA_NIGHT_WATCHER
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_RNW_GUARDIAN_OF_ELF );
	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_RNW_GUARDIAN_OF_ELF, iSkillLevel );
		if( pSkillTemplet != NULL )
		{
			// 물공 %만큼 마공증가
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkPhysic * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_TO_ATK_MAGIC );
		}
	}
#endif SERV_RENA_NIGHT_WATCHER
#endif //UPGRADE_SKILL_SYSTEM_2013

#ifdef NEW_SKILL_2010_11
	// oasis907 : 김상윤 [2010.12.3] // 룬 슬레이어 - 마법저항 수련
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ERS_MAGIC_RESISTANCE, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ERS_MAGIC_RESISTANCE );
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0)
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ERS_MAGIC_RESISTANCE );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fDefMagic += TotalStat.GetStat().m_fDefMagic * 
				CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_MAGIC_REL, iSkillLevel ) );
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ERS_MAGIC_RESISTANCE, iSkillLevel );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fDefMagic += TotalStat.GetStat().m_fDefMagic * CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_MAGIC_REL ) );
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}

	//{{ kimhc // 2010-12-13 // 승화된 분노
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_RRF_SPIRITUALIZED_FURY, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_RRF_SPIRITUALIZED_FURY );
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0)
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_RRF_SPIRITUALIZED_FURY );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkMagic * 
				CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) );
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_RRF_SPIRITUALIZED_FURY, iSkillLevel );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkMagic * CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_REL ) );
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}
	//}} kimhc // 2010-12-13 // 승화된 분노
#endif NEW_SKILL_2010_11

#ifdef CHUNG_SECOND_CLASS_CHANGE
	// oasis907 : 김상윤 [2011.6.28] 청 2차 전직 - 아이언 팔라딘 - 굳건한 의지
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CIP_IRON_WILL, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CIP_IRON_WILL );
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0)
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CIP_IRON_WILL );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fAtkPhysic += TotalStat.GetStat().m_fAtkPhysic * 
				CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_REL, iSkillLevel  ) );
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CIP_IRON_WILL, iSkillLevel );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fAtkPhysic += TotalStat.GetStat().m_fAtkPhysic * CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_REL  ) );
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}
#endif CHUNG_SECOND_CLASS_CHANGE

#endif NEW_SKILL_TREE

#ifdef BALANCE_BLADE_MASTER_20130117		/// 승리의 자신감 공격력 증가
#ifndef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_CONFIDENCE_VICTORY );

	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_CONFIDENCE_VICTORY, iSkillLevel );

		if( pSkillTemplet != NULL )
		{
			m_Stat.AccessStat().m_fAtkPhysic += TotalStat.GetStat().m_fAtkPhysic * CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_PHYSIC_REL  ) );
		}
	}
#endif //UPGRADE_SKILL_SYSTEM_2013
#endif BALANCE_BLADE_MASTER_20130117

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	// 마력 증폭
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION );

		if( NULL != pSkillTemplet )
		{
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkMagic * CalculateIncreasingRate(
				pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) );
		}
	}
#endif //UPGRADE_SKILL_SYSTEM_2013

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	// 룬 마스터리
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ERS_RUNE_MASTERY, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ERS_RUNE_MASTERY );

		if( NULL != pSkillTemplet )
		{
			m_Stat.AccessStat().m_fAtkMagic += TotalStat.GetStat().m_fAtkMagic * CalculateIncreasingRate(
				pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) );
		}
	}
#endif //UPGRADE_SKILL_SYSTEM_2013
}

#endif SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::SetStatInfoStatic( const WCHAR* wszStaticName_, const WCHAR* wszMsg_ )
{
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDLGCharInfo->GetControl( wszStaticName_ );

	if ( pStatic )
	{
		pStatic->GetString(0)->msg = wszMsg_;
	}
	else
	{
		ASSERT( !"UICharInfo Static does not exsist" );
	}
}

void CX2UICharInfo::SetStatInfoGuideDesc( const WCHAR* wszButtonName_, const WCHAR* wszGuideMsg_ )
{
	CKTDGUIButton* pButton = (CKTDGUIButton*) m_pDLGCharInfo->GetControl( wszButtonName_ );

	if ( pButton )
		pButton->SetGuideDesc( wszGuideMsg_ );
	else
	{
		ASSERT( !"UICharInfo Button does not exsist" );
	}
}

#ifdef SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::SumSocketOption( const vector<int>& vecOptions_, OUT CX2SocketItem::SocketData& sumSocketData_, const int iSocketLevel_ )
{
	// June 3291b4b0
	const CX2SocketItem* pSocketItem = g_pData->GetSocketItem();
	if ( NULL == pSocketItem )
		return;

	for ( UINT i = 0; i < vecOptions_.size(); ++i )
	{
		const CX2SocketItem::SocketData* pSocketData = pSocketItem->GetSocketData( vecOptions_[i] );
		if ( NULL == pSocketData || 0 != pSocketData->m_iDungeonOnlyType )
			continue;

		// off the Dungeon radio a dungeon-only socket counts only in a dungeon, battle field, PvE or type 5 game ( no June GAME_TYPE name )
		if ( false == m_bShowDungeonOption && true == pSocketData->m_bDungeonOnly )
		{
			if ( NULL == g_pX2Game )
				continue;
			const int iGameType = g_pX2Game->GetGameType();
			if ( CX2Game::GT_DUNGEON != iGameType && CX2Game::GT_BATTLE_FIELD != iGameType && CX2Game::GT_PVE != iGameType && 5 != iGameType )
				continue;
		}

		// off the PvP radio a PvP-only socket counts only in a PvP game ( June compares the game type with 1 here, not 322cc1a0 )
		if ( false == m_bShowPVPOption && true == pSocketData->m_bPVPOnly )
		{
			if ( NULL == g_pX2Game || CX2Game::GT_PVP != g_pX2Game->GetGameType() )
				continue;
		}

		if ( true == m_bShowPVPOption && true == pSocketData->m_bPVPOnly && true == pSocketData->m_bIsLevelLinkedStat )
		{
			sumSocketData_.LevelLinkedStatAdd( *pSocketData, iSocketLevel_ + pSocketData->m_iAddLevel );
			continue;
		}

		if ( NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
			continue;

		CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();
		const CX2Unit::UNIT_TYPE eUnitType = ( NULL != pMyUnit->GetUnitTemplet() ) ? pMyUnit->GetType() : CX2Unit::UT_NONE;

		if ( true == pSocketData->m_bIsRankLinkedStat )
		{
			CX2SocketItem::SocketData kScaledSocketData;
			kScaledSocketData.SetRankLinkedScaled( *pSocketData, g_pData->GetPvpReferData( static_cast<int>( static_cast<char>( pMyUnit->GetUnitData().m_cRank ) ) ) );
			sumSocketData_.ConvertAndAdd( kScaledSocketData, iSocketLevel_, eUnitType );
		}
		else if ( true == pSocketData->m_bIsPVERankLinkedStat )
		{
			const bool bForMP = ( 0.f < pSocketData->m_fMPIncByAttack && pSocketData->m_fMPIncByAttacked == pSocketData->m_fMPIncByAttack );
			const int iPveGrade = 0;		// stand-in: UnitData lacks June's PvE record ( unit +0xa00 >= 5 ? +0xa0c : 0 )
			CX2SocketItem::SocketData kScaledSocketData;
			kScaledSocketData.SetRankLinkedScaled( *pSocketData, g_pData->GetPveReferData( iPveGrade, bForMP ) );
			sumSocketData_.ConvertAndAdd( kScaledSocketData, iSocketLevel_, eUnitType );
		}
		else
		{
			sumSocketData_.ConvertAndAdd( *pSocketData, iSocketLevel_, eUnitType );
		}
	}
}

CX2SocketItem::SocketData CX2UICharInfo::GetMyEquippedSocketData( const int iEquipGroup_ /*= 2*/ )
{
	// June 3291ba40
	CX2SocketItem::SocketData SocketDataRetVal;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	if ( NULL == pUnit )
		return SocketDataRetVal;
	const int iUnitLevel = static_cast<int>( pUnit->GetUnitData().m_Level );

	for ( UINT i = 0; i < pUnit->GetUnitData().m_NowEqipItemUIDList.size(); ++i )
	{
		CX2Item* pItem = pUnit->GetInventory().GetItem( pUnit->GetUnitData().m_NowEqipItemUIDList[i] );
		if ( NULL == pItem || NULL == pItem->GetItemTemplet() )
			continue;

		// a fashion item or an accessory counts only in its own equip group ( 3286a4b0; 2 = both forms )
		if ( ( true == pItem->GetItemTemplet()->GetFashion() || CX2Item::IT_ACCESSORY == pItem->GetItemTemplet()->GetItemType() ) && 2 != iEquipGroup_ )
		{
			const int iGroup = pUnit->GetInventory().GetEquipGroup( pItem->GetUID() );
			if ( 2 != iGroup && iEquipGroup_ != iGroup )
				continue;
		}

		// 328825c0 ( usable ): March has its endurance part; 328823d0 ( appearance templet ) has no March field ( Stage 16 )
#ifdef REAL_TIME_ELSWORD
		if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
			continue;
#endif REAL_TIME_ELSWORD

		if ( CX2Unit::UC_NONE != pItem->GetItemTemplet()->GetUnitClass() &&
			false == GetX2UnitClassCompatibility( pUnit->GetUnitData().m_UnitClass, pItem->GetItemTemplet()->GetUnitClass() ) )
			continue;

		const int iItemSocketLevel = ( 0 < pItem->GetItemTemplet()->GetUseLevel() ) ? pItem->GetItemTemplet()->GetUseLevel() : iUnitLevel;

		std::vector<int> vecSocketOption( pItem->GetItemData().m_vecRandomSocket.begin(), pItem->GetItemData().m_vecRandomSocket.end() );
		std::vector<int> vecNormalSocketOption;
		pItem->GetItemTemplet()->GetSocketOptions( vecNormalSocketOption );
		vecSocketOption.insert( vecSocketOption.end(), vecNormalSocketOption.begin(), vecNormalSocketOption.end() );

		SumSocketOption( vecSocketOption, SocketDataRetVal, iItemSocketLevel );
		SumSocketOption( pItem->GetItemData().m_SocketOption, SocketDataRetVal, iItemSocketLevel );
		// June also sums item +0x8c / +0x9c: June-only socket lists March's ItemData lacks ( Stage 16 )
	}

	// 32350b80: the sets, each at its max level or the unit's
	if ( NULL != g_pData->GetItemManager() )
	{
		map<int,int> mapSetIDNParts;
		pUnit->GetSetIDNPartsNum( mapSetIDNParts );
		for ( map<int,int>::const_iterator mItr = mapSetIDNParts.begin(); mItr != mapSetIDNParts.end(); ++mItr )
		{
			vector<int> vecOptions;
			const int iMaxLevel = g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, vecOptions );
			if ( false == vecOptions.empty() )
				SumSocketOption( vecOptions, SocketDataRetVal, ( 0 < iMaxLevel ) ? iMaxLevel : iUnitLevel );
		}
	}
	// June @3291bf2d: group 2 of a unit type 10 also sums the other form's sets: stand-in: Lu/Ciel not ported

#ifdef RIDING_SYSTEM
	if ( NULL != CX2RidingPetManager::GetInstance() )
	{
		CX2RidingPetPtr pRidingPet = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( pUnit->GetUID() );
		if ( NULL != pRidingPet )
		{
			CX2RidingPetManager::RidingPetTemplet* pRidingPetTemplet =
				CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>( pRidingPet->GetInfo().m_Id ) );
			if ( NULL != pRidingPetTemplet )
				SumSocketOption( pRidingPetTemplet->m_vecSocketOption, SocketDataRetVal, iUnitLevel );
		}
	}
#endif RIDING_SYSTEM

	if ( NULL != g_pData->GetTitleManager() && NULL != g_pData->GetTitleManager()->GetTitleInfo( pUnit->GetTitleId() ) )
		SumSocketOption( g_pData->GetTitleManager()->GetTitleInfo( pUnit->GetTitleId() )->m_vecSocketOption, SocketDataRetVal, iUnitLevel );

#ifdef SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET
	if ( NULL != g_pX2Room && NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
	{
		std::vector<CX2Unit::SocketOptionSource> vecShared;
		CX2Unit::GetPartyShareSocketOptions( g_pX2Game->GetMyUnit()->GetUnitUID(), vecShared );
		for ( UINT i = 0; i < vecShared.size(); ++i )
			SumSocketOption( vecShared[i].m_vecSocketOption, SocketDataRetVal, vecShared[i].m_iSocketLevel );
	}
#endif SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET

#ifdef PET_AURA_SKILL
	if( g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetMyPet() != NULL )
	{
		CX2PET *pMyPet = g_pData->GetPetManager()->GetMyPet();
		if( pMyPet->GetPetInfo().m_Satiety / 4800.f >= 0.4f )
		{
			CX2PetManager::PetTemplet *pTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)pMyPet->GetPetInfo().m_PetId );
			const int iStep = pTemplet->m_vecPetStatus[ pMyPet->GetPetInfo().m_Evolution_Step ];
			for( int j = 0; j < iStep; ++j )
				SumSocketOption( pTemplet->m_vecAuraSkillOption[j], SocketDataRetVal, iUnitLevel );
		}
	}
#endif PET_AURA_SKILL

#ifdef BUFF_TEMPLET_SYSTEM
	vector<int> vecBuffSocketOptions;
	g_pData->GetPremiumBuffManager()->GetSocketOptions( vecBuffSocketOptions );
	if ( false == vecBuffSocketOptions.empty() )
		SumSocketOption( vecBuffSocketOptions, SocketDataRetVal, iUnitLevel );
#endif BUFF_TEMPLET_SYSTEM

	// June @3291c88f: the applied customize ( unit +0xa48 ) is summed too: March has no such system ( Stage 12 )

	SocketDataRetVal.LimitMaximum( true );		// June inlines the hyper-time / super-armor caps ( 40 / 0.5 / 10 )
	return SocketDataRetVal;
}
#else SERV_IRUHADEV_JUNE_CHARINFO_STATS
/** @function : SumSocketOption
	@brief : vector로 전달된 optionID에 대한 소켓 옵션을 얻어와서 sumSocketData_에 Sum을 수행함
	@param : const vector<int>& vecOptions_(소켓옵션ID들), OUT CX2SocketItem::SocketData& sumSocketData_(Sum을 수행할 socketData), const int iSocketLevel_(소켓효과적용레벨)
*/
void CX2UICharInfo::SumSocketOption( const vector<int>& vecOptions_, OUT CX2SocketItem::SocketData& sumSocketData_, const int iSocketLevel_ )
	//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
{
	const CX2SocketItem* pSocketItem = g_pData->GetSocketItem();
#ifdef SERV_GROW_UP_SOCKET
	vector<int> vecTempSocketOption;
	g_pData->GetSocketItem()->GetGrowUpSocketData( vecOptions_, vecTempSocketOption );
	for each( const int iOptions in vecTempSocketOption )
#else //SERV_GROW_UP_SOCKET
 	BOOST_TEST_FOREACH( const int, iOptions, vecOptions_ )
#endif //SERV_GROW_UP_SOCKET
 	{
		const CX2SocketItem::SocketData* pSocketData = pSocketItem->GetSocketData( iOptions );

		if( NULL == pSocketData )
			continue;

#ifdef VIEW_SPECIAL_OPTIONDATA
		if( ( false == m_bShowDungeonOption && pSocketData->m_bDungeonOnly == true )&& 
#else
		if( pSocketData->m_bDungeonOnly == true && 
#endif //VIEW_SPECIAL_OPTIONDATA
			( g_pX2Game == NULL || ( NULL != g_pX2Game && ( g_pX2Game->GetGameType() != CX2Game::GT_DUNGEON && g_pX2Game->GetGameType() != CX2Game::GT_BATTLE_FIELD ) ) ) )
		{
			continue;
		}

#ifdef VIEW_SPECIAL_OPTIONDATA		
		if( (false == m_bShowPVPOption && pSocketData->m_bPVPOnly  == true )&& 
#else
		if(	pSocketData->m_bPVPOnly  == true && 
#endif //VIEW_SPECIAL_OPTIONDATA
			( g_pX2Game == NULL || (NULL != g_pX2Game && g_pX2Game->GetGameType() != CX2Game::GT_PVP)) )
		{
			continue;
		}

		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
	#ifdef VIEW_SPECIAL_OPTIONDATA
		//오현빈//2012-10-13// 기본 스탯의 옵션수치화가 하드코딩 되어 있기 때문에 툴팁도 하드코딩
		if( true == m_bShowPVPOption && true == pSocketData->m_bPVPOnly && 
			true == pSocketData->m_bIsLevelLinkedStat )
		{ 
			sumSocketData_.LevelLinkedStatAdd( *pSocketData, iSocketLevel_ + pSocketData->m_iAddLevel );
		}
		else
	#endif //VIEW_SPECIAL_OPTIONDATA
#ifdef LEVEL_LINKED_STAT_DUNGEON
		if( false == pSocketData->m_bPVPOnly && 
			true == pSocketData->m_bIsLevelLinkedStat )
		{ 
			sumSocketData_.LevelLinkedStatAdd( *pSocketData, iSocketLevel_ + pSocketData->m_iAddLevel );
		}
		else
#endif // LEVEL_LINKED_STAT_DUNGEON
		{
	#ifdef SERV_9TH_NEW_CHARACTER // 김태환
			if ( NULL != g_pData &&
				 NULL != g_pData->GetMyUser() &&
				 NULL != g_pData->GetMyUser()->GetSelectUnit()  )
			{
				CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();

				sumSocketData_.ConvertAndAdd( *pSocketData, iSocketLevel_, eUnitType );
			}
	#else //SERV_9TH_NEW_CHARACTER
			sumSocketData_.ConvertAndAdd( *pSocketData, iSocketLevel_ );
	#endif //SERV_9TH_NEW_CHARACTER
			
		}
		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업	
	}			
}

CX2SocketItem::SocketData CX2UICharInfo::GetMyEquippedSocketData()
{
	CX2SocketItem::SocketData SocketDataRetVal;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	if( NULL == pUnit )
	{
		return SocketDataRetVal; 
	}

#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
    std::vector<int>    vecSocketOption;
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

	for( int i = 0; i < (int)pUnit->GetUnitData().m_NowEqipItemUIDList.size(); i++ )
	{
		UidType eqipUID = pUnit->GetUnitData().m_NowEqipItemUIDList[i];
		CX2Item* pItem = pUnit->GetInventory().GetItem( eqipUID );

		if( NULL == pItem ||  
			NULL == pItem->GetItemTemplet() 
            )
		{
			continue;
		}

		//{{ kimhc // 실시간 엘소드 중 실시간 내구도 감소
#ifdef REAL_TIME_ELSWORD
		if ( ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 ) )
			continue;
#endif REAL_TIME_ELSWORD
		//}} kimhc // 실시간 엘소드 중 실시간 내구도 감소

		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		// 착용레벨
		int iItemSocketLevel = 0;

		if ( 0 < pItem->GetItemTemplet()->GetUseLevel() )
			iItemSocketLevel = pItem->GetItemTemplet()->GetUseLevel();
		else
			iItemSocketLevel = pUnit->GetUnitData().m_Level;

		// 아이템 고유의 소켓 옵션
#ifdef	X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
	#ifdef SERV_NEW_ITEM_SYSTEM_2013_05 // 오현빈
			// 랜덤 소켓 옵션 담기
			if( NULL != pItem )
			{
				vecSocketOption.assign( pItem->GetItemData().m_vecRandomSocket.begin(), 
					pItem->GetItemData().m_vecRandomSocket.end() );
			}
			// 일반 소켓 옵션 담기
			std::vector<int> vecNormalSocketOption;
			pItem->GetItemTemplet()->GetSocketOptions( vecNormalSocketOption );
#ifdef SERV_EVENT_VALENTINE_RING_IS_DUNGEON
			//여기서 수치가 계산 되는지 확인해 보장 
			std::vector<int>::iterator ivecIter;
			for(ivecIter = vecNormalSocketOption.begin(); ivecIter != vecNormalSocketOption.end();)
			{
				if( (g_pMain!= NULL && g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP) || ( g_pX2Game != NULL && CX2Game::GT_DUNGEON != g_pX2Game->GetGameType() ) 
					|| ( g_pData != NULL && g_pData->GetDungeonRoom() != NULL && g_pData->GetDungeonRoom()->GetDungeonID() != SEnum::DI_EVENT_VALENTINE_DUNGEON_INT ))
				{
					if( *ivecIter == 1090005 || *ivecIter == 1090006 || *ivecIter == 1090007 || *ivecIter == 1090008
						|| *ivecIter == 1090009 || *ivecIter == 1090010 || *ivecIter == 1090011 || *ivecIter == 1090012 )
					{
						ivecIter = vecNormalSocketOption.erase(ivecIter++);
					}
					else
					{
						++ivecIter;
					}
				}
				else
				{
					++ivecIter;
				}
			}		
#endif SERV_EVENT_VALENTINE_RING_IS_DUNGEON
			vecSocketOption.insert( vecSocketOption.end(), vecNormalSocketOption.begin(), vecNormalSocketOption.end() );
	#else
			pItem->GetItemTemplet()->GetSocketOptions( vecSocketOption );
	#endif // SERV_NEW_ITEM_SYSTEM_2013_05
        SumSocketOption( vecSocketOption, SocketDataRetVal, iItemSocketLevel );
#else   //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING
		SumSocketOption( pItem->GetItemTemplet()->m_vecSocketOption, SocketDataRetVal, iItemSocketLevel );
#endif  //X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING

		// 아이템에 유저가 박은 소켓에 의한 옵션
		SumSocketOption( pItem->GetItemData().m_SocketOption, SocketDataRetVal, iItemSocketLevel );

		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업	
	}
	
	//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
	// 세트 아이템

	// 세트 아이디와 파츠 갯수를 얻는다
	map<int,int> mapSetIDNParts;
	pUnit->GetSetIDNPartsNum( mapSetIDNParts );

	if ( false == mapSetIDNParts.empty() )
	{
		map< int, int >::const_iterator mItr = mapSetIDNParts.begin();
		vector<int> vecOptions;

		while ( mItr != mapSetIDNParts.end() )
		{
			// SetID 별로 Option들과 SetItem의 최대 적용 레벨을 얻어옴
			const int iMaxLevel = g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, vecOptions );

			// Option이 있는 경우
			if ( false == vecOptions.empty() )
			{
				// SetItem에 적용 레벨이 있으면 적용레벨을 소켓의 레벨로 사용
				if ( 0 < iMaxLevel )
					SumSocketOption( vecOptions, SocketDataRetVal, iMaxLevel );
				// 없으면 유저의 레벨을 소켓의 레벨로 사용
				else
					SumSocketOption( vecOptions, SocketDataRetVal, pUnit->GetUnitData().m_Level );

				// 옵션들을 GUUser가 가지고 있는 m_vecSetItemOptions에 넣어줌
				vecOptions.resize(0);
			}
			++mItr;
		}		
	}
	//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

//#ifdef TITLE_SYSTEM   
	if(g_pData->GetTitleManager()->GetTitleInfo(pUnit->GetTitleId()) != NULL)
	{
		const vector< int >& vecTitleOptions = g_pData->GetTitleManager()->GetTitleInfo(pUnit->GetTitleId())->m_vecSocketOption;
		
		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		// 칭호 옵션조은 Socket레벨로 유저의 레벨을 전달
		SumSocketOption( vecTitleOptions, SocketDataRetVal, pUnit->GetUnitData().m_Level );
		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업		
	} // if

//#endif

#ifdef PET_AURA_SKILL
	if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetMyPet() != NULL )
	{
		CX2PET *pMyPet = g_pData->GetPetManager()->GetMyPet();
		if( pMyPet != NULL && pMyPet->GetPetInfo().m_Satiety / 4800.f >= 0.4f )
		{
			CX2PetManager::PetTemplet *pTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)pMyPet->GetPetInfo().m_PetId );
			int iStep = pTemplet->m_vecPetStatus[ pMyPet->GetPetInfo().m_Evolution_Step ];

			if( iStep > 0 )
			{
				for( int j=0; j<iStep; ++j)
				{		
					//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
					// 펫 기운 스킬은 Socket레벨로 유저의 레벨을 전달
					SumSocketOption( pTemplet->m_vecAuraSkillOption[j], SocketDataRetVal, pUnit->GetUnitData().m_Level );
					//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업	
					
				}
			}
		}		
	}
#endif

#ifdef BUFF_TEMPLET_SYSTEM
	vector<int> vecBuffSocketOptions;
	g_pData->GetPremiumBuffManager()->GetSocketOptions( vecBuffSocketOptions );

	if ( !vecBuffSocketOptions.empty() )
		SumSocketOption( vecBuffSocketOptions, SocketDataRetVal, pUnit->GetUnitData().m_Level );
#endif BUFF_TEMPLET_SYSTEM

	SocketDataRetVal.LimitMaximum( true );

	return SocketDataRetVal;
}

#endif SERV_IRUHADEV_JUNE_CHARINFO_STATS
void CX2UICharInfo::ResetNowEquipUI()
{
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_SHOW
	// June 329146d0
	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(i);
		if ( pItemSlot != NULL && pItemSlot->GetSlotType() == CX2Slot::ST_EQUIPPED )
			pItemSlot->DestroyItemUI();
	}

	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Unit::UnitData* pMyUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		for ( int i = 0; i < (int)pMyUnitData->m_NowEqipItemUIDList.size(); i++ )
		{
			CX2Item* pItem = kInventory.GetItem( pMyUnitData->m_NowEqipItemUIDList[i] );
			if ( pItem == NULL )
				continue;

			/// stand-in: 3286a350 tests June's minor-unit equip positions, Lu/Ciel not ported; 3234fe90 ( ring in the second ring slot ) needs a slot map March lacks
			const bool bMinorUnitItem = false;

			for ( int j = 0; j < (int)m_SlotList.size(); j++ )
			{
				CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(j);
				if ( pItemSlot->GetEquipPos() == pItem->GetItemTemplet()->GetEqipPosition() &&
					( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItem->GetItemTemplet()->GetFashion() ) &&
					pItemSlot->GetIsForMinorUnit() == bMinorUnitItem )
				{
					if ( pItemSlot->IsCreatedItemUI() == true )
						pItemSlot->ResetItemUI( pItem );
					else
						pItemSlot->CreateItemUI( pItem );

					break;
				}
			}
		}
	}

	UpdateItemLevel();
#else SERV_IRUHADEV_JUNE_CHARINFO_SHOW


	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(i);
		if ( pItemSlot != NULL && pItemSlot->GetSlotType() == CX2Slot::ST_EQUIPPED )
		{
			pItemSlot->DestroyItemUI();
		}

	}

    if ( g_pData->GetMyUser() != NULL
        && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
	    const CX2Unit::UnitData* pMyUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
	    const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		for ( int i = 0; i < (int)pMyUnitData->m_NowEqipItemUIDList.size(); i++ )
		{
			UidType uidType = pMyUnitData->m_NowEqipItemUIDList[i];
			CX2Item* pItem = kInventory.GetItem( uidType );
			if ( pItem == NULL )
				continue;

			for ( int j = 0; j < (int)m_SlotList.size(); j++ )
			{
				CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(j);
				//{{ 09.06.02 태완 : 악세사리 류의 경우에는 fashion 조건 체크를 하지 않도록 예외처리 :
				// 악세사리이면서 패션템인 아이템이 장착되지 않는 것 때문에
				if( pItemSlot->GetEquipPos() == pItem->GetItemTemplet()->GetEqipPosition() && 
					(pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItem->GetItemTemplet()->GetFashion() )  )
				{
					if ( pItemSlot->IsCreatedItemUI() == true )
						pItemSlot->ResetItemUI( pItem );
					else
						pItemSlot->CreateItemUI( pItem );

					break;
				}
			}
		}
	}

	//g_pData->GetUIManager()->SetUILayerIndex(CX2UIManager::UI_MENU_CHARINFO, true);

	// 내구도검사
	if(g_pInstanceData != NULL && g_pInstanceData->GetMiniMapUI() != NULL)
	{
		g_pInstanceData->GetMiniMapUI()->UpdateEnduranceNotice();
	}
	UpdateItemLevel();
#endif SERV_IRUHADEV_JUNE_CHARINFO_SHOW
}


void CX2UICharInfo::ResetEDnCashnVP()
{
	WCHAR buff[256] = {0};
	CKTDGUIStatic* pStaticUnitStat;

#ifdef SERV_SUPPORT_SEVERAL_CASH_TYPES
#ifndef CLIENT_COUNTRY_TW
	pStaticUnitStat = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_cash" );
	pStaticUnitStat->GetString(0)->msg = g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalTotalCash() );
#endif CLIENT_COUNTRY_TW
#else //SERV_SUPPORT_SEVERAL_CASH_TYPES
	pStaticUnitStat = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_cash" );
	pStaticUnitStat->GetString(0)->msg = g_pMain->GetEDString( g_pData->GetMyUser()->GetCash() );
#endif //SERV_SUPPORT_SEVERAL_CASH_TYPES

	//{{ kimhc // 2011-07-18 // 옵션 수치화 작업
	//}} kimhc // 2011-07-18 // 옵션 수치화 작업
}

void CX2UICharInfo::UpdateLevel()
{

	CKTDGUIStatic* pStaticUserInfo = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_Lv" );
	if( NULL == pStaticUserInfo )
		return;

	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();

	WCHAR wszText[32] = {0,};
	//wsprintf( wszText, L"%d", (int)pUnitData->m_Level );
	StringCchPrintf( wszText, 32, L"%d", (int)pUnitData->m_Level );
	pStaticUserInfo->GetString(0)->msg = wszText;



}
void CX2UICharInfo::ResetResurrectionStoneUI()
{
	CKTDGUIStatic* pStaticResurrectionNum = (CKTDGUIStatic*)m_pDLGCharInfo->GetControl( L"g_pStaticInfo_resurrection" );
	if ( pStaticResurrectionNum != NULL && pStaticResurrectionNum->GetString(0) != NULL )
	{
		//{{ 허상형 : [2009/10/14] //	자동결제 부활석 표시 부분
#ifdef AUTO_PAYMENT	//	자동결제 부활석 추가
		
		WCHAR wszAutoResStoneNum[10] = {0,};
		if( g_pData->GetMyUser()->GetSelectUnit()->GetAutoResStoneNum() > 0 )
		{
//			wsprintfW( wszAutoResStoneNum, L"(+%d)", g_pData->GetMyUser()->GetSelectUnit()->GetAutoResStoneNum() );
			StringCchPrintf( wszAutoResStoneNum, 10, L"(+%d)", g_pData->GetMyUser()->GetSelectUnit()->GetAutoResStoneNum() );
		}

		pStaticResurrectionNum->GetString(0)->msg = g_pMain->GetEDString( g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum() ) + wszAutoResStoneNum;	

#else	//	기존 형태
		pStaticResurrectionNum->GetString(0)->msg = g_pMain->GetEDString( g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum() );	
#endif	//	AUTO_PAYMENT
		//}} 허상형 : [2009/10/14] //	자동결제 부활석 표시 부분
	}
}

void CX2UICharInfo::EquipChanged()
{
	if(false == m_bShow) 
		return;

	ResetNowEquipUI();
	m_pUnitViewerUI->UpdateEqip(true);
	ResetStat();

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	if ( NULL != g_pData &&
		 NULL != g_pData->GetMyUser() &&
		 NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		/// 스킬 추가 레벨 효과 갱신
		g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelBySocket();
	}
#endif // UPGRADE_SKILL_SYSTEM_2013
}


void CX2UICharInfo::DeleteUnitViewerUI()
{
	SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );
}

void CX2UICharInfo::ResetUnitViewerUI()
{

	if ( m_pUnitViewerUI == NULL )
	{
		//m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
		m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();

		//g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

		//m_pUnitViewerUI->SetOverUI(true);

		m_pUnitViewerUI->SetShowObject( m_bShow );

		//중복 설정 방지를 위해 제거
		m_pDLGCharInfo->SetHasUnit( (CKTDGObject*)m_pUnitViewerUI );

		m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
	}



	if( m_pUnitViewerUI == NULL )
		return; 

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	if(pUnit != NULL)
	{
		if( m_pUnitViewerUI->GetUnit() == NULL ||
			pUnit->GetUID() != m_pUnitViewerUI->GetUnit()->GetUID() )
		{
#ifdef SEPARATION_MOTION
			m_pUnitViewerUI->SetUnit( pUnit, CX2UnitViewerUI::UVS_CHARINFO ); 
#else
			m_pUnitViewerUI->SetUnit( pUnit );
#endif
		}
		else
		{
			m_pUnitViewerUI->UpdateEqip( true );
		}

		// 위치정보 : SetPosition에도 있음. 주의!!		
		m_pUnitViewerUI->SetFixed( true );
		D3DXVECTOR3 UnitviewerPos = m_pDLGCharInfo->GetDummyPos(2);
		m_pUnitViewerUI->SetPositionOnScr( UnitviewerPos.x + m_MovedPosition.x, UnitviewerPos.y + m_MovedPosition.y, UnitviewerPos.z, 1.15f );		// 유닛뷰어 위치는 스크립트에 있음		

		//m_pUnitViewerUI->GetMatrix().Move(D3DXVECTOR3(0, 0, 0));
		//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
		D3DXVECTOR3 UnitviewerLightPos = m_pDLGCharInfo->GetDummyPos(3);
		m_pUnitViewerUI->SetLightPos( UnitviewerLightPos );
		m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
		m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );
	}


// 	CX2UnitViewerUI::DeleteKTDGObject( m_pUnitViewerUI );
// 
// 	m_fMouseSensitivity = 0.01f;
// 	m_fRemainRotY = 0.0f;
// 	m_bRotateReturn = true;
// 
// 	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
// 
// 	m_bUnitClicked = false;
// 
// 	//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
// 	m_pUnitViewerUI->SetLightPos( 1000, 1000, 200 );
// 	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
// 	

// 	m_pUnitViewerUI->SetUnit( g_pData->GetMyUser()->GetSelectUnit() );
// 	m_pUnitViewerUI->GetMatrix().Move(  D3DXVECTOR3( -275, -105, 100 ) );
// 	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.35f, 1.35f, 1.35f ) );
// 	m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
// 	m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );
// 	
// 	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
}


bool CX2UICharInfo::IsInUnitClickReact( D3DXVECTOR2 mousePos )
{
	D3DXVECTOR2 m_ConvertPos	= g_pKTDXApp->ConvertByResolution( m_UnitClickPos + m_MovedPosition );
	D3DXVECTOR2 m_ConvertSize	= g_pKTDXApp->ConvertByResolution( m_UnitClickSize );

	if ( mousePos.x >= m_ConvertPos.x   && mousePos.x <= m_ConvertPos.x + m_ConvertSize.x &&
		mousePos.y >= m_ConvertPos.y && mousePos.y <= m_ConvertPos.y + m_ConvertSize.y )
	{
		return true;
	}

	return false;
}



void CX2UICharInfo::DrawSlotMouseOverImage()
{
	if ( GetCheckOperationCondition() == false )
	{
		m_pDLGSelectedItem->SetShow( false );
		return;
	}

	bool bCheck = false;

	if ( *m_pSlotBeforeDragging != NULL )
	{
		if ( IsInEquippedItemSlotReact( m_NowMousePos ) == true && (*m_pSlotBeforeDragging)->IsItem() )
		{
			//{{ 2008.11.13 김태완 : UI 예외처리
			//*m_DraggingItemUID = ((CX2SlotItem*)(*m_pSlotBeforeDragging))->GetItemUID();
			//}}
			const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

			
			CX2Item *pItem = kInventory.GetItem( *m_DraggingItemUID );
			if( pItem == NULL )
				return;

			const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
			if( pItemTemplet == NULL )
				return;

			CX2Item::ITEM_TYPE itemType = pItemTemplet->GetItemType();
			if (  itemType == CX2Item::IT_WEAPON || itemType == CX2Item::IT_DEFENCE || itemType == CX2Item::IT_ACCESSORY )
			{
				CX2Unit::EQIP_POSITION equipPosition = pItemTemplet->GetEqipPosition();
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
				const bool bMinorUnitItem = false;		// stand-in: June 3286a350 tests whether the dragged item sits in a minor-unit equip slot ( Lu/Ciel not ported )
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW

				for ( int i = 0; i < (int)m_SlotList.size(); i++ )
				{
					CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(i);
					if ( pItemSlot->GetSlotType() == CX2Slot::ST_EQUIPPED && pItemSlot->GetEquipPos() == equipPosition && 
#ifdef SERV_IRUHADEV_JUNE_CHARINFO_VIEW
						( itemType == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItemTemplet->GetFashion() ) &&
						pItemSlot->GetIsForMinorUnit() == bMinorUnitItem )
#else SERV_IRUHADEV_JUNE_CHARINFO_VIEW
						( itemType == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItemTemplet->GetFashion() ) )
#endif SERV_IRUHADEV_JUNE_CHARINFO_VIEW
					{
						m_pDLGSelectedItem->SetPos( pItemSlot->GetPos() );
						m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( pItemSlot->GetSize() );

						bCheck = true;
					}
				}
			}
			else
			{
				if ( m_pNowOverItemSlot != NULL )
				{
					m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
					m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

					bCheck = true;
				}
			}			
		}
		else
		{
			if ( m_pNowOverItemSlot != NULL )
			{
				m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
				m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

				bCheck = true;
			}
		}
	}
	else
	{
		if ( m_pNowOverItemSlot != NULL )
		{
			m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
			m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

			bCheck = true;
		}
	}

	if ( bCheck == true )
	{
		m_pDLGSelectedItem->SetShow( true );
	}
	else
	{
		m_pDLGSelectedItem->SetShow( false );
	}

}

wstring CX2UICharInfo::GetSlotItemDesc()
{

	wstring itemDesc=  L"";

	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->IsResetItemUI() == false )
		{
			itemDesc = m_pNowOverItemSlot->GetSlotDesc();
		}
		else
		{	
			itemDesc = GetSlotItemDescByUID( m_pNowOverItemSlot->GetItemUID() );
		}
	}

	return itemDesc;

}


bool CX2UICharInfo::Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( CX2SlotItem* pFromCX2SlotItem, CX2SlotItem* pToCX2SlotItem )
{
	if ( pFromCX2SlotItem == NULL || pToCX2SlotItem == NULL )
		return false;

	if ( pFromCX2SlotItem->GetSortType() == pToCX2SlotItem->GetSortType() && pFromCX2SlotItem->GetSlotID() == pToCX2SlotItem->GetSlotID() )
		return false;

	if(g_pData->GetUIManager()->GetUIInventory() == NULL)
	{
		ASSERT( !"UIInventory NULL" );			// 최초 생성 후 항상 살아있어야 함
		g_pData->GetUIManager()->CreateUIInventory();
	}
	int NowInvenSortTypePageNum = g_pData->GetUIManager()->GetUIInventory()->GetNowInvenSortTypePageNum();

	KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ;

	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType	= pFromCX2SlotItem->GetSortType();

#ifndef SERV_REFORM_INVENTORY_TEST
	if ( pFromCX2SlotItem->GetSlotType() == CX2Slot::ST_INVENTORY )
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iFromSlotID	= pFromCX2SlotItem->GetSlotID() + (( NowInvenSortTypePageNum - 1 ) * MY_INVENTORY_SHOW_MAX_SLOT_NUM);
	}
	else
#endif SERV_REFORM_INVENTORY_TEST
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iFromSlotID	= pFromCX2SlotItem->GetSlotID();
	}


	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType	= pToCX2SlotItem->GetSortType();

#ifndef SERV_REFORM_INVENTORY_TEST
	if ( pToCX2SlotItem->GetSlotType() == CX2Slot::ST_INVENTORY )
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iToSlotID		= pToCX2SlotItem->GetSlotID() + (( NowInvenSortTypePageNum - 1 ) * MY_INVENTORY_SHOW_MAX_SLOT_NUM);
	}
	else
#endif SERV_REFORM_INVENTORY_TEST
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iToSlotID		= pToCX2SlotItem->GetSlotID();
	}

#ifdef SERV_SHARING_BANK_TEST
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = -1;
	if( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIPrivateBank() != NULL )
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = g_pData->GetUIManager()->GetUIPrivateBank()->GetShareUnitUID();
	}
#endif //SERV_SHARING_BANK_TEST

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK );

	return true;
}


bool CX2UICharInfo::Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( CX2Inventory::SORT_TYPE fromSortType, int fromSlotID, CX2Inventory::SORT_TYPE toSortType, int toSlotID )
{
	if ( fromSortType == toSortType && fromSlotID == toSlotID )
		return false;

	KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ;

	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType	= fromSortType;
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iFromSlotID	= fromSlotID;
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType	= toSortType;
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iToSlotID		= toSlotID;

#ifdef SERV_SHARING_BANK_TEST
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = -1;
	if( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIPrivateBank() != NULL )
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = g_pData->GetUIManager()->GetUIPrivateBank()->GetShareUnitUID();
	}
#endif //SERV_SHARING_BANK_TEST

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK );

	return true;
}


bool CX2UICharInfo::GetOverMouseCharInfo()
{
	if( GetShow() == true && m_pDLGCharInfo != NULL && m_pDLGCharInfo->GetIsMouseOver() == true )
		return true;
	return false;		
}





#ifdef MONSTER_TROPHY

	void CX2UICharInfo::OpenDLGNPCViewerUI( bool bOpen )
	{
		if( true == bOpen )
		{
			if ( m_pDLGMonsterTrophy == NULL )
			{
				m_pDLGMonsterTrophy = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Monster_Trophy_Test.lua" );
				g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMonsterTrophy );	
			}

			if ( m_pDLGMonsterTrophy != NULL )
			{
				m_pDLGMonsterTrophy->SetShowEnable( true, true );
			}
		}
		else
		{
			if ( m_pDLGMonsterTrophy != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMonsterTrophy, NULL, false );
			m_pDLGMonsterTrophy = NULL;
		}
	}

	void CX2UICharInfo::UpdateDLGNPCViewerUI()
	{
		const CX2UnitManager::NPCUnitTemplet* pNPCTemplet = g_pData->GetUnitManager()->GetNPCUnitTemplet( m_eNPCID );
		if( NULL == pNPCTemplet )
		{
			if( NULL != m_pNPCViewerUI )
			{
				m_pNPCViewerUI->SetShowObject( false );
			}
			return; 
		}


		if( NULL == m_pNPCViewerUI )
		{
			m_pNPCViewerUI = CX2NPCUnitViewerUI::Create();
		}

		m_pNPCViewerUI->ResetNPC( m_eNPCID );
		m_pNPCViewerUI->SetShowObject( true );

		m_pNPCViewerUI->PlayAnim( NULL, CKTDGXSkinAnim::XAP_LOOP, true );	// 0 번째 index animation을 play

		if( m_pDLGMonsterTrophy != NULL )
		{
			m_pDLGMonsterTrophy->SetHasUnit( (CKTDGObject*) m_pNPCViewerUI );

			D3DXVECTOR3 vScale = m_pDLGMonsterTrophy->GetDummyPos(1);
			m_pNPCViewerUI->SetScale( vScale );

			D3DXVECTOR3 vPos = m_pDLGMonsterTrophy->GetDummyPos(0);
			vPos.x += m_pDLGMonsterTrophy->GetPos().x;
			vPos.y += m_pDLGMonsterTrophy->GetPos().y;
			m_pNPCViewerUI->SetPosition( vPos );
		}
	}


#endif MONSTER_TROPHY
void CX2UICharInfo::UpdateItemLevel()
{
	// 아이템 평균 레벨
	if( NULL != g_pData && NULL != g_pData->GetMyUser() )
	{
		CX2Unit* pSelectUnit = g_pData->GetMyUser()->GetSelectUnit();
		if( NULL != pSelectUnit )
		{
			pSelectUnit->UpdateItemLevel();
			CKTDGUIStatic* pStaticAverageItemLevel = static_cast< CKTDGUIStatic* >( m_pDLGCharInfo->GetControl( L"Static_AverageItemLevel" ) );
			if ( pStaticAverageItemLevel != NULL )
			{
				pStaticAverageItemLevel->SetString( 0, GET_REPLACED_STRING( (STR_ID_19227, "i", pSelectUnit->GetAverageItemlevel() )));
			}
		}
	}
}

#ifdef SERV_LOCAL_RANKING_SYSTEM
void CX2UICharInfo::OpenProfilePasswordDlg()
{
	if(m_pDlgPassword != NULL)
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDlgPassword, NULL, false );
		m_pDlgPassword = NULL;
	}

	m_pDlgPassword = g_pMain->KTDGUIOkAndCancelEditBox_Hide( D3DXVECTOR2( 300, 300), GET_STRING( STR_ID_12537 ), CIM_PASSWORD_OK, g_pMain->GetNowState(),16,false, CIM_PASSWORD_CANCLE );
	if( NULL == m_pDlgPassword )
		return;

	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDlgPassword->GetControl( L"OkAndCancelEditBox" ));
	if( NULL != pStatic )
	{
		CKTDGUIControl::CPictureData* pPicture = pStatic->GetPicture( 4 );
		if( NULL != pPicture )
		{
			pPicture->SetSizeX( 250.f );
		}
		pPicture = pStatic->GetPicture( 5 );
		if( NULL != pPicture )
		{
			pPicture->SetPos( D3DXVECTOR2( 280,50) );
		}
		pPicture = pStatic->GetPicture( 0 );
		if( NULL != pPicture )
		{
			pPicture->SetSizeX( 310.0f );
		}
		pPicture = pStatic->GetPicture( 1 );
		if( NULL != pPicture )
		{
			pPicture->SetSizeX( 310.0f );
		}
		pPicture = pStatic->GetPicture( 2 );
		if( NULL != pPicture )
		{
			pPicture->SetSizeX( 310.0f );
		}
		CKTDGUIButton* pButton = static_cast<CKTDGUIButton*>(m_pDlgPassword->GetControl( L"Button_Check" ) );
		if( NULL != pButton )
		{
			pButton->SetOffsetPos(D3DXVECTOR2(1,6));
		}
		pButton = static_cast<CKTDGUIButton*>(m_pDlgPassword->GetControl( L"Cancel" ) );
		if( NULL != pButton )
		{
			pButton->SetOffsetPos(D3DXVECTOR2(4,6));
		}
	}
}
#endif // SERV_LOCAL_RANKING_SYSTEM
