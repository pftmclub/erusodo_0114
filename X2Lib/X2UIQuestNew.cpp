#include "StdAfx.h"


#ifdef SERV_EPIC_QUEST
#include ".\X2UIQuestNew.h"
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_QUEST_UI
#ifdef SERV_IRUHADEV_JUNE_QUEST_SCENE
#include "X2GFXEventScene.h"
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE
#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY
#include "X2SlideShotScriptedManager.h"
#endif SERV_IRUHADEV_JUNE_QUEST_STORY

//{{ Iruha : 2026-09-04 // HUD portrait after a quest job change
#ifdef SERV_IRUHADEV_JOBCHANGE_PORTRAIT
#include "X2GageManager.h"
#endif SERV_IRUHADEV_JOBCHANGE_PORTRAIT
//}}

#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
#include "Offline/X2OfflineLog.h"

// June quest system Q2: a quest id with no templet (June's data dropped it, or a save / packet still
// names it) is skipped where it would be dereferenced, and logged once per id.
static void QuestNoTempletLog( int iQuestID, const wchar_t* szWhere )
{
	static std::set< int > s_setLogged;
	if( false == s_setLogged.insert( iQuestID ).second )
		return;

	CX2OfflineLog::Server( L"[JUNE_QUEST] MIG quest %d has no templet - %s skips it (guarded)", iQuestID, szWhere );
}
#endif SERV_IRUHADEV_JUNE_QUEST_DATA

#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
namespace _CONST_UIQUESTNEW_HELPERS_
{
// ---------------------------------------------------------------------------------------------------------------------
// small helpers
// ---------------------------------------------------------------------------------------------------------------------

/// June 32696fa0 + 3267b440: "is this quest id in the NPC friendship manager's map". The manager is Stage 18; an empty manager says no.
static bool IsFriendshipQuest( int /*iQuestID*/ )
{
	return false;
}

/// June checks the game state numbers 0xe..0xf and 0x15 ( village, battle field ). March's X2_STATE order differs ( the arcade states ),
/// so the two states March has are named here.
static bool IsVillageOrBattleFieldState()
{
	if( NULL == g_pMain )
		return false;

	return CX2Main::XS_VILLAGE_MAP == g_pMain->GetNowStateID() || CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID();
}

/// June's unit +0x3c: the selected unit's transcendence step. Stage 6 owns transcendence; every March unit is step 0.
static int SelectUnitClassTran()
{
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return 0;

	return g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iUnitClassTran;
#else SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	return 0;
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
}

static void QuestUILog( const WCHAR* wszFormat, int iA = 0, int iB = 0 )
{
	static int s_iLines = 0;
	if( s_iLines >= 200 )
		return;
	++s_iLines;
	CX2OfflineLog::Server( wszFormat, iA, iB );
}

/// June 323ca880: may the epic quest in this list row be skipped ( Button_Skip_Epic_Quest ).
/// Epic type, every next-quest templet exists, not a time event, and the unit is more than 7 levels above the quest's fair level.
static bool CanSkipEpicQuest( const CX2UIQuestNew::QuestInfo& kInfo, int iQuestID )
{
	CX2QuestManager* pQM = g_pData->GetQuestManager();
	const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( iQuestID );
	if( NULL == pT )
		return false;
	if( CX2UIQuestNew::QTS_EPIC != kInfo.m_eQuestType )
		return false;

	bool bSkip = true;
	if( pT->m_vecAfterQuestID.empty() )
		bSkip = false;
	else
	{
		for( UINT i = 0; i < pT->m_vecAfterQuestID.size(); ++i )
		{
			if( NULL == pQM->GetQuestTemplet( pT->m_vecAfterQuestID[i] ) )
			{
				bSkip = false;
				break;
			}
		}
	}
	if( true == pT->m_bIsTimeEvent )
		bSkip = false;

	if( g_pData->GetSelectUnitLevel() - kInfo.m_iFairLevel > 7 )
		return bSkip;
	return false;
}

static int GetDetailQuestPictureNumber( int iTypeSort, int iRepeatSort )
{
	switch( iTypeSort )
	{
	case CX2UIQuestNew::QTS_EPIC:			return 6;
	case CX2UIQuestNew::QTS_CONTEST:		return 5;
	case CX2UIQuestNew::QTS_EVENT:			return 4;
	case CX2UIQuestNew::QTS_PCBANG:			return 3;
	case CX2UIQuestNew::QTS_CHANGE_JOB:		return 2;
	case CX2UIQuestNew::QTS_SKILL:			return 1;
	case CX2UIQuestNew::QTS_NORMAL:
		switch( iRepeatSort )
		{
		case CX2UIQuestNew::RTS_NORMAL:		return 0;
		case CX2UIQuestNew::RTS_REPEAT:		return 7;
		case CX2UIQuestNew::RTS_DAY:		return 8;
		case CX2UIQuestNew::RTS_WEEK:		return 15;
		case CX2UIQuestNew::RTS_MONTH:		return 19;
		}
		return 0;
	case CX2UIQuestNew::QTS_GUILD:			return 16;
	case CX2UIQuestNew::QTS_STORY:			return 17;
	case CX2UIQuestNew::QTS_TRANSCEND:		return 18;
	}
	return 0;
}

/// June 323d8be0 / 323ce6f0: the three event NPCs whose face is a button texture, not a DLG_UI_Npc_Face01 key.
static bool SetNpcFacePicture( CKTDGUIControl::CPictureData* pPicture, int iNpcID, const wstring& wstrKey )
{
	if( NULL == pPicture )
		return false;

	if( CX2UnitManager::NUI_EVENT_DOLL == iNpcID || CX2UnitManager::NUI_EVENT_BLESS_OF_SOLACE == iNpcID || CX2UnitManager::NUI_EVENT_GUILD_EDAN_ROOM == iNpcID )
		pPicture->SetTex( L"DLG_INT_Button02.TGA", L"NPC_FACE_EVENT" );
	else
		pPicture->SetTex( L"DLG_UI_Npc_Face01.TGA", wstrKey.c_str() );
	return true;
}

/// June's comparator for the available list ( 323eac30 ): story last, the stories by id. It answers true for two non-story rows, so
/// std::sort reorders the other quests ( kept as June has it, JUNE_BUGS ).
struct AvailQuestInfoSort : public std::binary_function< CX2UIQuestNew::QuestInfo, CX2UIQuestNew::QuestInfo, bool >
{
	bool operator()( const CX2UIQuestNew::QuestInfo& kLeft, const CX2UIQuestNew::QuestInfo& kRight ) const
	{
		if( CX2UIQuestNew::QTS_STORY == kLeft.m_eQuestType )
			return CX2UIQuestNew::QTS_STORY == kRight.m_eQuestType && kLeft.m_iID < kRight.m_iID;
		return CX2UIQuestNew::QTS_STORY != kRight.m_eQuestType;
	}
};

static int GetQuestStatePictureNumber( int eState )
{
	switch( eState )
	{
	case CX2UIQuestNew::QSS_COMPLETE_EPIC:
	case CX2UIQuestNew::QSS_COMPLETE:
	case CX2UIQuestNew::QSS_COMPLETE_STORY:	return 9;
	case CX2UIQuestNew::QSS_FAILED:			return 12;
	case CX2UIQuestNew::QSS_ONGOING:		return 10;
	case CX2UIQuestNew::QSS_AVAILABLE:		return 11;
	}
	return 0;
}

}
using namespace _CONST_UIQUESTNEW_HELPERS_;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
CX2UIQuestNew::CX2UIQuestNew( CKTDXStage* pNowStage, const WCHAR* pFileName )
: CX2ItemSlotManager( pNowStage, pFileName ),
m_iQuestPerPage(0),

m_bShow(false),
m_MovedPosition(0,0),						// D3DXVECTOR2
m_DLGPosition(0,0),							// D3DXVECTOR2
m_DLGSize(0,0),								// D3DXVECTOR2
m_pDLGUIQuestReceive(NULL),
m_pDLGUIQuestClear(NULL),
m_pDLGQuestRewardItemInfo(NULL),
m_pDLGMsgBox(NULL),
m_eNPCID(CX2UnitManager::NUI_NONE),
m_bShowQuickQuestDLG(false),
m_bShowClearDLG(false),
m_bUserShowQuickQuestDLG(false),

m_iTopQuestSlotIndex(0),
m_bIsOpenAvailCategory(true),
m_bIsOpenOngoingCategory(true),
m_bIsOpenEventCategory(true),
m_iOpenEpicCategory(-1),
m_eNowQuestUIMode(QUM_EPIC_QUEST),
m_iSelectedQuestID(0),
m_iSelectedQuestSlotIndex(-1),
m_eLastQuestUIMode(QUM_QUEST),
m_bIsTalkingQuestNPC(false),
m_bAvailableQuestNPCHas(false),


m_bCompleteQuestInEpicTab(false),
m_bCompleteQuestInNormalTab(false),
m_bOngoingQuestInEpicTab(false),
m_bOngoingQuestInNormalTab(false),

m_iCompleteEpicCategory(-1),
m_iOngoingEpicCategory(-1),

// m_vecPrevNotAssignedQuestID.clear();
// m_vecNotAssignedQuestID.clear();
// m_vecAvailableQuestID.clear();
// m_vQuestList.clear();
m_iPickedQuestID(0),
//m_mapNewQuest.clear();
m_bAdminQuestComplete(false),
m_bLastRewardWasSelection(false),
m_SumDelta(0),
//m_vecQuestDesc,
m_iQuestDescIndex(0),
m_iQuestDescLinePerPage(1),
m_vQuestDescSize(0,0),
m_byNumOfItemThatICanGet( 0 )	// kimhc // 2009-12-18 // 가질수 있는 선택 보상 아이템 갯수
#ifdef GUIDE_QUICK_QUEST_COMPLETE
, m_iShowQuickQuest(0) // 오현빈 // 2012-05-14 // 보여지는 퀵퀘스트 갯수
#endif //GUIDE_QUICK_QUEST_COMPLETE
#ifdef REFORM_QUEST
, m_hQuestCompleteParticle(INVALID_PARTICLE_SEQUENCE_HANDLE)
#endif //REFORM_QUEST
{
	m_vecPrevNotAssignedQuestID.clear();		// 새로 갱신된 퀘스트 리스트가 있는지 확인하기 위해서
	m_vecNotAssignedQuestID.clear();
	m_vecAvailableQuestID.clear();
	m_vQuestList.clear();
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	InitMembers();
#endif SERV_IRUHADEV_JUNE_QUEST_UI
	m_mapNewQuest.clear();					// 새로 생긴 퀘스트 목록

	//////////////////////////////////////////////////////////////////////////

	RegisterLuaBind();	

	m_pDLGUIQuestList = NULL;

	m_pDLGUIQuestList = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_NEW_Quest_List.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUIQuestList );
	m_pDLGUIQuestList->SetShowEnable(false, false);

	m_pDLGUIQuestList->SetDisableUnderWindow(true);

	m_pTalkBoxManager = CX2TalkBoxManager::CreateTalkBoxManger();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pTalkBoxManager );
	m_pTalkBoxManager->SetOverUI( true );

	WCHAR buffer[256] = {0,};
	for(int i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
	{

		//wsprintf( buffer, L"Button_Category%d", i);
		StringCchPrintf( buffer, 256, L"Button_Category%d", i);
		m_vecUIQuestSlotInfo[i].m_pButtonCategory = (CKTDGUIButton*) m_pDLGUIQuestList->GetControl( buffer );
		if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
			m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetShowEnable(false, false);

		//wsprintf( buffer, L"Static_Category%d", i);
		StringCchPrintf( buffer, 256, L"Static_Category%d", i);
		m_vecUIQuestSlotInfo[i].m_pStaticCategory = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( buffer );
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->SetShowEnable(false, false);

		
		//wsprintf( buffer, L"RadioButton_List%d", i);
		StringCchPrintf( buffer, 256, L"RadioButton_List%d", i);
		m_vecUIQuestSlotInfo[i].m_pButtonQuest = (CKTDGUIRadioButton*) m_pDLGUIQuestList->GetControl( buffer );
		if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonQuest )
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetShowEnable(false, false);

		//wsprintf( buffer, L"Static_QuestList%d", i);
		StringCchPrintf( buffer, 256, L"Static_QuestList%d", i);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( buffer );
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
			m_vecUIQuestSlotInfo[i].m_pStaticQuest->SetShow(false);

		m_vecUIQuestSlotInfo[i].m_bIsCategory = false;
		m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = false;
		m_vecUIQuestSlotInfo[i].m_QuestID = -1;
		m_vecUIQuestSlotInfo[i].m_DungeonID = 0;
	}

	m_pCheckListQuickQuest = static_cast< CKTDGUICheckBox* >( m_pDLGUIQuestList->GetControl( L"CheckBox_QuickQuest" ) );
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	// June 323d0ca0 @323d1638-323d1684, just before the tab hide: the movie rows and the navi steps ( rows of three, rounded up )
	m_iMovieCount = g_pMain->GetClearMovieCount();
	if( 0 == m_iMovieCount % 3 )
		m_iMovieNaviStep = ( m_iMovieCount - 1 ) / 3 + 1;
	else
		m_iMovieNaviStep = m_iMovieCount / 3 + 1;
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	HideMovieTab();		// June 323d0ca0 @323d0cd4 ( window creation ): hide the scenario-movie tab's controls
#endif SERV_IRUHADEV_JUNE_QUEST_UI

	//m_pDLGUIQuestDetail

	m_pDLGUIQuestDetail = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_NEW_Quest_Detail.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUIQuestDetail );
	m_pDLGUIQuestDetail->SetShowEnable(false, false);

	m_pDLGUIQuestDetail->SetDisableUnderWindow(true);


	// Quest Desc 관련 한 라인 길이, 스크립트에서 얻어옴 
	m_iLineWidthQuestClearCondition = m_pDLGUIQuestDetail->GetDummyInt( 0 );
	m_iLineWidthNPCQuestDescription	= m_pDLGUIQuestDetail->GetDummyInt( 1 );

	// Quest Desc 관련 폰트 너비, 스크립트에서 얻어옴 
	CKTDGUIListBox* pListboxClearCondition = (CKTDGUIListBox*) m_pDLGUIQuestDetail->GetControl( L"ListboxClearCondition" );
	m_pFontQuestClearCondition = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pListboxClearCondition->GetString()->fontIndex );


	CKTDGUIListBox* pListboxNPCDescription = (CKTDGUIListBox*) m_pDLGUIQuestDetail->GetControl( L"ListboxNPCDescription" );
	m_pFontNPCQuestDescription = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pListboxNPCDescription->GetString()->fontIndex );


	//m_pDLGUIQuickQuest

	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		m_vecUIQuickQuest[i].m_pDLGUIQuickQuest =  new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_NEW_Quest_Quick.lua" );

		
		m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetShowEnable(false, false);
		m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetDisableUnderWindow(true);

		m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetPos(D3DXVECTOR2(810.f, 143.f + (_CONST_UIQUESTNEW_INFO_::g_fQuickQuestMinimizeSizeY * i)));
			

		m_vecUIQuickQuest[i].m_pButtonMinimize = (CKTDGUIButton*) m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"ButtonMinimize" );
		m_vecUIQuickQuest[i].m_pButtonExpand = (CKTDGUIButton*) m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"ButtonExpand" );
#ifdef REFORM_QUEST
		m_vecUIQuickQuest[i].m_pButtonComplete = static_cast<CKTDGUIButton*>(m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"QuestComplete" ));
		m_vecUIQuickQuest[i].m_pButtonComplete->AddDummyInt(i);
#endif //REFORM_QUEST
		m_vecUIQuickQuest[i].m_pButtonMinimize->AddDummyInt(i);
		m_vecUIQuickQuest[i].m_pButtonExpand->AddDummyInt(i);



		m_vecUIQuickQuest[i].m_pStaticExpandBGNormal = (CKTDGUIStatic*) m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"StaticExpandBGNormal" );
		m_vecUIQuickQuest[i].m_pStaticExpandBGOver = (CKTDGUIStatic*) m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"StaticExpandBGOver" );
		m_vecUIQuickQuest[i].m_pStaticQuestTitle = (CKTDGUIStatic*) m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"StaticQuestTitle" );
		m_vecUIQuickQuest[i].m_pStaticQuestInfo = (CKTDGUIStatic*) m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetControl( L"StaticQuestInfo" );

		m_vecUIQuickQuest[i].m_pButtonMinimize->SetShowEnable(true, true);
		m_vecUIQuickQuest[i].m_pButtonExpand->SetShowEnable(false, false);

		m_vecUIQuickQuest[i].m_pStaticExpandBGNormal->SetShowEnable(false, false);
		m_vecUIQuickQuest[i].m_pStaticExpandBGOver->SetShowEnable(false, false);
		m_vecUIQuickQuest[i].m_pStaticQuestTitle->SetShowEnable(true, true);
		m_vecUIQuickQuest[i].m_pStaticQuestInfo->SetShowEnable(false, false);
	}

	m_pCheckDetailQuickQuest = static_cast< CKTDGUICheckBox* >( m_pDLGUIQuestDetail->GetControl( L"CheckBox_QuickQuest" ) );


#ifdef REFORM_QUEST
	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest;++i)
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_vecUIQuickQuest[i].m_pDLGUIQuickQuest );
	}
#else
	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest;i+=2)
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_vecUIQuickQuest[i].m_pDLGUIQuickQuest );

	}
	for(int i=_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest - 1; i>=0; i-=2)
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_vecUIQuickQuest[i].m_pDLGUIQuickQuest );
	}
#endif //REFORM_QUEST
	m_pFontQuickQuestClearCondition =  g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( m_vecUIQuickQuest[0].m_pStaticQuestInfo->GetString(0)->fontIndex );


}


CX2UIQuestNew::~CX2UIQuestNew(void)
{

	m_vQuestList.clear();

	SAFE_DELETE_DIALOG( m_pDLGUIQuestReceive );	
	SAFE_DELETE_DIALOG( m_pDLGUIQuestClear );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	SAFE_DELETE_DIALOG( m_pDLGQuestRewardItemInfo );

	SAFE_DELETE_KTDGOBJECT( m_pTalkBoxManager );

}

void CX2UIQuestNew::SetLayer( X2_DIALOG_LAYER layer )
{
	SetSlotManagerLayer(layer);
	SetSlotManagerChangeSequence(false);

	if(m_pDLGUIQuestList != NULL) 
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer(m_pDLGUIQuestList, layer);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGUIQuestList, false );
	}
	if(m_pDLGUIQuestDetail != NULL) 
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer(m_pDLGUIQuestDetail, layer);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGUIQuestDetail, false );
	}

}

void CX2UIQuestNew::SetPosition( D3DXVECTOR2 vec )
{
	m_pDLGUIQuestList->SetPos(vec);
	m_pDLGUIQuestDetail->SetPos(vec);

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		D3DXVECTOR2 pos;
		pos = pItemSlot->GetPos();
		
		pos -= m_MovedPosition;
		pos += vec;

		pItemSlot->SetPos(pos);
	}
	m_MovedPosition = vec;
}

void CX2UIQuestNew::FocusQuest( bool bDown )
{
	int iSelectedQuestSlotIndex = m_iSelectedQuestSlotIndex;


	if(bDown)
	{
		iSelectedQuestSlotIndex++;
		if(iSelectedQuestSlotIndex > _CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage - 1)
		{
			return;
		}
		

		if(m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_bIsCategory == true)
		{
			iSelectedQuestSlotIndex++;
			if(iSelectedQuestSlotIndex > _CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage - 1 ||
				m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_bIsCategory == true || 
				m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_QuestID == -1)
			{
				return;
			}
		}
		else if(m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_QuestID == -1)
		{
			return;
		}
		m_iSelectedQuestSlotIndex = iSelectedQuestSlotIndex;
	}
	else
	{
		iSelectedQuestSlotIndex--;
		if(iSelectedQuestSlotIndex < 0)
		{
			return;
		}
		
		if(m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_bIsCategory == true)
		{
			iSelectedQuestSlotIndex--;
			if(iSelectedQuestSlotIndex < 0 ||
				m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_bIsCategory == true || 
				m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_QuestID == -1)
			{
				return;
			}
		}
		else if(m_vecUIQuestSlotInfo[iSelectedQuestSlotIndex].m_QuestID == -1)
		{
			return;
		}
		m_iSelectedQuestSlotIndex = iSelectedQuestSlotIndex;
	}

	m_iSelectedQuestID = m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID;
	m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_pButtonQuest->SetChecked(true);
	
	return;
}


HRESULT CX2UIQuestNew::OnFrameMove( double fTime, float fElapsedTime )
{
	if(!m_bShow)
	{
		return S_OK;
	}


#ifdef REFORM_NOVICE_GUIDE
	SetNoviceGuideStep();
#else
	// 초보자 가이드
	if( g_pData != NULL && g_pTFieldGame != NULL && 
		g_pTFieldGame->GetNoviceGuide() != NULL && g_pTFieldGame->GetNoviceGuide()->GetIsPlayGuide() == true &&
		g_pData->GetLocationManager()->IsDungeonLounge( g_pData->GetLocationManager()->GetCurrentVillageID() ) == false &&
		(g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 || 
		 g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7) )			
	{
		if( m_pDLGUIQuestClear != NULL && m_pDLGUIQuestClear->GetShow() == true )
		{
			// 완료후 보상 창이 열려진 상태
			if( m_eNowQuestUIMode == QUM_QUEST_DETAIL )
			{
				if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 &&
					m_iSelectedQuestID == 11000 && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11000) != true &&
					g_pTFieldGame->GetJoinNpcId() == CX2UnitManager::NUI_HAGERS )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(6);	// 보상창 완료 가이드
				}
				else if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7 &&
					m_iSelectedQuestID == 11010 && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11010) != true )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(6);	// 보상창 완료 가이드
				}
				else
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(0);	// 가이드 제거
				}
			}
		}
		else
		{
			// 일반퀘스트 목록 창이 열려진 상태
			if( m_eNowQuestUIMode == QUM_QUEST || m_eNowQuestUIMode == QUM_QUEST_RECEIVE )
			{
				if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 &&					
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11000) != true &&
					g_pTFieldGame->GetJoinNpcId() == CX2UnitManager::NUI_HAGERS )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(13); // 에픽퀘스트 탭 가이드
				}
				else if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7 &&					
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11010) != true )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(13); // 에픽퀘스트 탭 가이드
				}
				else
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(0);
				}				
			}

			// 에픽퀘스트 목록 창이 열려진 상태
			else if( m_eNowQuestUIMode == QUM_EPIC_QUEST )
			{
				if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 &&					
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11000) != true &&
					g_pTFieldGame->GetJoinNpcId() == CX2UnitManager::NUI_HAGERS )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(3); // 에픽퀘스트 목록 가이드
				}
				else if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7 &&					
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11000) == true && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11010) != true )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(14); // 에픽퀘스트 목록 가이드
				}
				else
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(0);
				}
			}

			// 특정 퀘스트 디테일 창이 열려진 상태
			else if( m_eNowQuestUIMode == QUM_QUEST_DETAIL && ( m_iSelectedQuestID == 11000 || m_iSelectedQuestID == 11010 ) )
			{
				if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 &&
					m_iSelectedQuestID == 11000 && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11000) != true &&
					g_pTFieldGame->GetJoinNpcId() == CX2UnitManager::NUI_HAGERS )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(4);	// 에픽퀘스트 완료 가이드
				}
				else if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7 &&
					m_iSelectedQuestID == 11010 && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11010) != true )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(4);	// 에픽퀘스트 완료 가이드
				}	
				else 
				{
					if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 && 
						g_pTFieldGame->GetJoinNpcId() != CX2UnitManager::NUI_HAGERS)
						g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(0);	// 목록탭 가이드
					else
						g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(13);	// 목록탭 가이드
				}
			}

			// 특정 퀘스트 디테일 창이 열려있지 않은 상태
			else if( m_eNowQuestUIMode == QUM_QUEST_DETAIL && (m_iSelectedQuestID != 11000 && m_iSelectedQuestID != 11010) )
			{
				if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 &&
					m_iSelectedQuestID == 11000 && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11000) != true &&
					g_pTFieldGame->GetJoinNpcId() == CX2UnitManager::NUI_HAGERS )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(13);	// 목록탭 가이드
				}
				else if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7 &&
					m_iSelectedQuestID == 11010 && 
					g_pData->GetQuestManager()->GetUnitCompleteQuest(11010) != true )
				{
					g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(13);	// 목록탭 가이드
				}					
			}
			else
			{
				// 그무엇도 아니라면...
				g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(0);	// 가이드 제거
			}
		}
	}	
#endif //REFORM_NOVICE_GUIDE	
	
	if(false == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDLGUIQuestList ) )	// 앞에 모달 다이얼로그가 없으면
	{
#ifndef REFORM_QUEST //퀘스트 창이 키입력 가져가지 못하도록 수정( 캐릭터 움직임이 불편함 )
		if(g_pMain->GetNowStateID() != CX2Main::XS_DUNGEON_GAME &&
			g_pMain->GetNowStateID() != CX2Main::XS_PVP_GAME)
		{
			if(m_eNowQuestUIMode == QUM_QUEST ||
				m_eNowQuestUIMode == QUM_QUEST_RECEIVE||
				m_eNowQuestUIMode == QUM_EPIC_QUEST)
			{
				if ( GET_KEY_STATE( GA_UP ) == TRUE )
				{
					FocusQuest(false);
				}
				else if ( GET_KEY_STATE( GA_DOWN ) == TRUE )
				{
					FocusQuest(true);
				}
#ifdef SERV_KEY_MAPPING_INT
				else if( GET_KEY_STATE( GA_RETURN ) == TRUE || g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_RETURN ) == TRUE )
#else // SERV_KEY_MAPPING_INT
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_RETURN ) == TRUE )
#endif // SERV_KEY_MAPPING_INT
				{
					if(	m_iSelectedQuestSlotIndex != -1 &&
						m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID != -1)
					{
						SetNowQuestUIMode(QUM_QUEST_DETAIL);
						ResetQuestUI();
					}
				}
			}
		}
#endif //REFORM_QUEST
		if(m_eNowQuestUIMode == QUM_QUEST_DETAIL)
		{
			if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_BACKSPACE ) == TRUE )
			{
				ReturnToQuestListUI();
				ResetQuestUI();
			}
		}
	}

	if( (m_pDLGUIQuestDetail != NULL && m_pDLGUIQuestDetail->GetIsMouseOver() == true) ||
		(m_pDLGQuestRewardItemInfo != NULL && m_pDLGQuestRewardItemInfo->GetIsMouseOver() == true) )
	{
		return CX2ItemSlotManager::OnFrameMove( fTime, fElapsedTime );
	}
	else
	{
		CX2ItemSlotManager::InvalidSelectedItem();
		CX2ItemSlotManager::InvalidSlotDesc();
	}
	return S_OK;
}

bool CX2UIQuestNew::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 앞에 모달 다이얼로그가 있으면 메시지를 처리하지 않는다
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDLGUIQuestList ) )
		return false;

	// 앞에 모달 다이얼로그가 있으면 메시지를 처리하지 않는다
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDLGUIQuestDetail ) )
		return false;

#ifdef FIXED_QUEST_DLG_IS_NOT_CHECK_FLAG_WHEN_NOT_OVER_MOUSE
#ifdef SERV_IRUHADEV_JUNE_QUEST_MSGPROC
	// June 323e5690: after the two modal tests the message switch runs; there is no QuestDetail mouse-over test ( quest-system Q8b fix )
#else SERV_IRUHADEV_JUNE_QUEST_MSGPROC
	if ( false == m_pDLGUIQuestDetail->GetIsMouseOver() )
		return false;
#endif SERV_IRUHADEV_JUNE_QUEST_MSGPROC
#endif // FIXED_QUEST_DLG_IS_NOT_CHECK_FLAG_WHEN_NOT_OVER_MOUSE

	bool bFlag = false;
	switch( uMsg )
	{
		// 마우스 메시지들에 대해
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		//case WM_MOUSEWHEEL:
		{
//			if ( false == m_pDLGUIQuestList->GetIsMouseOver())
// 			{
// 				return false;
// 			}
			bFlag = true;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
// 			if ( false == m_pDLGUIQuestList->GetIsMouseOver())
// 			{
// 				return false;
// 			}
			g_pData->GetUIManager()->SetUILayerIndex(CX2UIManager::UI_MENU_QUEST_NEW, true);
			bFlag = true;			
		}
		break;
	default:
		break;
	}


	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			MouseMove( mousePos );
		}
		break;
	case WM_MOUSEWHEEL:
		{ 
			if(m_eNowQuestUIMode != QUM_QUEST_DETAIL)
			{
				return OnMouseWheel(hWnd, uMsg, wParam,lParam);
			}
		}break;
	}
	return bFlag;
}


bool CX2UIQuestNew::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(wParam)
	{

	case UQNCM_EXIT:
		{
			SetShow(false);
		}
		return true;


#ifdef SERV_IRUHADEV_JUNE_TUTORIAL_MODE
	case UQNCM_CHECK_QUICK_QUEST:
		{
			/// June buff port Phase 14c, CX2UIQuestNew::UICustomEventProc 323e1bf0 @323e1c67-323e1cad: in the tutorial a shown
			/// quick quest stays shown; the Detail dialog's CheckBox_QuickQuest is ticked back and the event is consumed
			if( true == m_bUserShowQuickQuestDLG && true == g_pMain->GetIsPlayingTutorial() )
			{
				if( NULL != m_pDLGUIQuestDetail )
				{
					CKTDGUICheckBox* pCheck = static_cast< CKTDGUICheckBox* >( m_pDLGUIQuestDetail->GetControl( L"CheckBox_QuickQuest" ) );
					if( NULL != pCheck )
						pCheck->SetChecked( true, false );
				}
				return true;
			}
#else SERV_IRUHADEV_JUNE_TUTORIAL_MODE
	case UQNCM_CHECK_QUICK_QUEST:
		{
#endif SERV_IRUHADEV_JUNE_TUTORIAL_MODE
			UserShowQuickQuestDLG(!m_bUserShowQuickQuestDLG);
#ifdef QUEST_GUIDE
			switch ( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
				{
					if( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetNpcIndicator() )
					{
						g_pTFieldGame->GetNpcIndicator()->SetShow(m_bUserShowQuickQuestDLG);
					}
				} break;
			case CX2Main::XS_BATTLE_FIELD:
				{
					if( NULL != g_pX2Game && NULL != static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetNpcIndicator() )
					{
						static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetNpcIndicator()->SetShow(m_bUserShowQuickQuestDLG);
					}
				} break;
			}
#endif //QUEST_GUIDE
		}
		return true;

	case UQNCM_SELECT_QUEST_NORMAL_TAB: //의뢰 퀘스트
		{
			SetNowQuestUIMode(QUM_QUEST);
			m_iTopQuestSlotIndex = 0;
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			m_bMovieTab = false;		// June @323e1dd0: every quest tab closes the Video tab before the reset ( Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			ResetQuestUI();
		}
		return true;
	case UQNCM_SELECT_QUEST_SCENARIO_TAB: //에픽 퀘스트
		{
			SetNowQuestUIMode(QUM_EPIC_QUEST);
			m_iTopQuestSlotIndex = 0;
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			m_bMovieTab = false;		// June @323e1dd0 ( Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			ResetQuestUI();
		}
		return true;
	case UQNCM_QUICK_TO_EXPAND:
		{
#ifndef REFORM_QUEST
			if(g_pMain->GetNowStateID() != CX2Main::XS_DUNGEON_GAME && g_pMain->GetNowStateID() != CX2Main::XS_PVP_GAME)
			{
				MinimizeAllUIQuickQuest();
			}
#endif //REFORM_QUEST

			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iSelectedQuickQuestIndex = pButton->GetDummyInt(0);

			if( iSelectedQuickQuestIndex >= 0 )
				ExpandUIQuickQuest(iSelectedQuickQuestIndex);
		}
		return true;
	case UQNCM_QUICK_TO_MINIMIZE:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iSelectedQuickQuestIndex= pButton->GetDummyInt(0);
	
			if( iSelectedQuickQuestIndex >= 0 )
			{				
				MinimizeUIQuickQuest(iSelectedQuickQuestIndex);
				MouseOutUIQuickQuest(iSelectedQuickQuestIndex);
			}
#ifdef GUIDE_QUICK_QUEST_COMPLETE
			ShowUIDesc( false );
#endif GUIDE_QUICK_QUEST_COMPLETE
		}
		return true;
	case UQNCM_QUICK_EXPAND_MOUSE_OVER:
		{
			//겹치는 UI(던전, 대전도구)가 열려 있을 때 마우스 오버 동작 하지 않도록 설정
			if( NULL != g_pMain && NULL != g_pMain->GetPartyUI() &&
				true == g_pMain->GetPartyUI()->GetShowDungeonMenu() || true == g_pMain->GetPartyUI()->GetShowPVPMenu() )
			{
				return true;
			}
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iSelectedQuickQuestIndex= pButton->GetDummyInt(0);

			if( iSelectedQuickQuestIndex >= 0 )
			{
#ifdef GUIDE_QUICK_QUEST_COMPLETE
				D3DXVECTOR2 pos = pButton->GetPos() + pButton->GetDialog()->GetPos();
				pos.x += pButton->GetWidth() / 2;
				pos.y += pButton->GetHeight() / 2;
				MouseOverUIQuickQuest(iSelectedQuickQuestIndex, pos);
#else
				MouseOverUIQuickQuest(iSelectedQuickQuestIndex);
#endif //GUIDE_QUICK_QUEST_COMPLETE
			}
		}
		return true;	
	case UQNCM_QUICK_EXPAND_MOUSE_OUT:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iSelectedQuickQuestIndex= pButton->GetDummyInt(0);
			if( iSelectedQuickQuestIndex >= 0 )
				MouseOutUIQuickQuest(iSelectedQuickQuestIndex);
#ifdef GUIDE_QUICK_QUEST_COMPLETE
			ShowUIDesc( false );
#endif GUIDE_QUICK_QUEST_COMPLETE
		}
		return true;	
	case UQNCM_QUICK_TO_DETAIL:
		{
			//겹치는 UI(던전, 대전도구)가 열려 있을 때 마우스 동작 하지 않도록 설정
			if( NULL != g_pMain && NULL != g_pMain->GetPartyUI() &&
				true == g_pMain->GetPartyUI()->GetShowDungeonMenu() || true == g_pMain->GetPartyUI()->GetShowPVPMenu() )
			{
				return true;
			}

			//이미 열려 있다면 닫고 return
			if( true == m_bShow && m_eNowQuestUIMode == QUM_QUEST_DETAIL )
			{
				SetShow(false);
				return true;
			}
			
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iSelectedQuickQuestIndex = pButton->GetDummyInt(0);
			m_iSelectedQuestID = m_vecUIQuickQuest[iSelectedQuickQuestIndex].m_QuestID;
			SetNowQuestUIMode(QUM_QUEST_DETAIL);
			if(m_bShow == false)
			{
				m_bShow = true;
				g_pData->GetUIManager()->UIOpened(CX2UIManager::UI_MENU_QUEST_NEW);
			}
			ResetQuestUI();
		}
		return true;
	case UQNCM_BACK_TO_QUEST_LIST: //목록으로
		{
			ReturnToQuestListUI();
			m_iTopQuestSlotIndex = 0;
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			m_bMovieTab = false;		// June 323e2201 -> tail @323e1dd0: Back closes the Video tab before the reset, as the tabs do ( Q8b review )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			ResetQuestUI();
		}
		return true;
	case UQNCM_QUEST_SELECT:
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) lParam;
			pRadioButton->SetChecked(true);
			m_iSelectedQuestSlotIndex= pRadioButton->GetDummyInt(0);

			m_iSelectedQuestID = m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID;
			SetNowQuestUIMode(QUM_QUEST_DETAIL);
			ResetQuestUI();

		}
		return true;
	case UQNCM_QUEST_SELECT_MOUSE_OVER:
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) lParam;
			pRadioButton->SetChecked(true);
			m_iSelectedQuestSlotIndex= pRadioButton->GetDummyInt(0);
			m_iSelectedQuestID = m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID;
		}
		return true;
	case UQNCM_CATEGORY_SELECT:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			m_iSelectedQuestSlotIndex = pButton->GetDummyInt(0);
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
			const int iKeepTop = m_iTopQuestSlotIndex;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
#ifdef MODIFY_ACCEPT_QUEST
			bool bIsChangeReceiveTab = false;
#endif // MODIFY_ACCEPT_QUEST

			//pButton->SetNormalTex(wstrTextureName.c_str(), wstrTextureKey.c_str());
			if(m_eNowQuestUIMode == QUM_QUEST_RECEIVE )
			{
				if(	m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID == 0)
				{
					m_bIsOpenAvailCategory = !m_bIsOpenAvailCategory;
				}
				else
				{
					m_bIsOpenOngoingCategory = !m_bIsOpenOngoingCategory;
				}
				m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_bIsCategoryOpen = !m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_bIsCategoryOpen;
#ifdef MODIFY_ACCEPT_QUEST
				bIsChangeReceiveTab = true;
#endif // MODIFY_ACCEPT_QUEST
			}
			else if(m_eNowQuestUIMode == QUM_QUEST )
			{
				if(	m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID == 1)
				{
					m_bIsOpenOngoingCategory = !m_bIsOpenOngoingCategory;
				}
				m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_bIsCategoryOpen = !m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_bIsCategoryOpen;
			}
			else if(m_eNowQuestUIMode == QUM_EPIC_QUEST)
			{
				if(m_iOpenEpicCategory == m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID)
				{
					m_iOpenEpicCategory = -1;
				}
				else
				{
					m_iOpenEpicCategory = m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID;
				}
			}
			else if( m_eNowQuestUIMode == QUM_EVENT_QUEST )
			{
				if(	m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID == 1)
				{
					m_bIsOpenEventCategory = !m_bIsOpenEventCategory;
				}

				m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_bIsCategoryOpen = m_bIsOpenEventCategory;
			}
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
			else if( m_eNowQuestUIMode == QUM_GUILD_QUEST )
			{
				if(	m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID == 1)
				{
					m_bIsOpenGuildCategory = !m_bIsOpenGuildCategory;
				}
				m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_bIsCategoryOpen = m_bIsOpenGuildCategory;
			}
			else if( m_eNowQuestUIMode == QUM_STORY_QUEST )
			{
				if( m_iOpenStoryCategory == m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID )
					m_iOpenStoryCategory = -1;
				else
					m_iOpenStoryCategory = m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_QuestID;
			}
#endif SERV_IRUHADEV_JUNE_QUEST_UI
#ifdef MODIFY_ACCEPT_QUEST
			// 수행 중 의뢰 카테고리를 열 때
			// 스크롤 초기화되어, 사용이  불펺나 점 개선
			if( false == bIsChangeReceiveTab )
#endif // MODIFY_ACCEPT_QUEST
			{
				m_iTopQuestSlotIndex = 0;
			}
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
			// the request tab keeps its first row when a category opens or closes ( June 323e1bf0 case 0xa03e, mode 1 )
			if( QUM_QUEST_RECEIVE == m_eNowQuestUIMode )
				m_iTopQuestSlotIndex = iKeepTop;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
			ResetQuestUI();


		}
		return true;
	case UQNCM_QUEST_COMPLETE_MOUSE_OVER:
		{
			if(g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ||
				g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME)
			{
				CKTDGUIButton* pControl = (CKTDGUIButton*)lParam;
				D3DXVECTOR2 pos = pControl->GetPos() + pControl->GetDialog()->GetPos();
				pos.x += pControl->GetWidth() / 2.f;
#ifdef HARDCODING_STRING_TO_INDEX
				ShowUIDesc( true, GET_STRING(STR_ID_9635), pos, CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT);
#else
				ShowUIDesc( true, L"던전 게임 중에는 완료 보고 할 수 없습니다.", pos, CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT);
#endif HARDCODING_STRING_TO_INDEX
			}
		}
		return true;
	case UQNCM_TOOLTIP_MOUSE_OUT:
		{
			ShowUIDesc(false);
		}
		return true;
// 	case UQNCM_QUEST_SELECT:
// 		{
// 			CKTDGUIRadioButton* pQuestButton = (CKTDGUIRadioButton*)lParam;
// 			int QuestIndex = m_iTopQuestSlotIndex + pQuestButton->GetDummyInt(0);
// 
// 			if( QuestIndex < (int)m_vecAvailableQuestID.size() )
// 				SelectQuest( m_vecAvailableQuestID[QuestIndex] );			
// 		}
// 		return true;
	case UQNCM_QUEST_ACCEPT_CONFIRM:
		{
			if( m_pDLGMsgBox != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
				m_pDLGMsgBox = NULL;
			}

			if( m_iSelectedQuestID != -1 )
			{
				Handler_EGS_NEW_QUEST_REQ( m_iSelectedQuestID );
			}

			return true;

		}
		return true;
	case UQNCM_QUEST_ACCEPT_CANCLE:
		{
			if( m_pDLGMsgBox != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
				m_pDLGMsgBox = NULL;
			}
		}
		return true;
	case UQNCM_QUEST_ACCEPT:
		{
			if( m_iSelectedQuestID != -1 )
			{
				const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iSelectedQuestID );
				if( NULL == pQuestInst )
				{
					if( m_pDLGMsgBox != NULL )
					{
						g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
						m_pDLGMsgBox = NULL;
					}

					m_pDLGMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250, 300), GET_STRING( STR_ID_891 ), UQNCM_QUEST_ACCEPT_CONFIRM, g_pMain->GetNowState(), UQNCM_QUEST_ACCEPT_CANCLE );										
				}
			}

		}
		return true;
	case UQNCM_QUEST_REMIND:
		{
#ifdef SERV_IRUHADEV_JUNE_QUEST_SCENE
			// June 323e1bf0 case 0xa046 @323e2908..323e2938: the selected quest's scenes again ( 323cb2e0( id, 2 ) ). A story quest then opens the story book
			// ( @323e293d: Q10 ). March's Lua scenes stay in the #else.
			if( m_iSelectedQuestID != -1 )
			{
				const CX2QuestManager::QuestTemplet* pSceneTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
				if( NULL != pSceneTemplet )
					PlayQuestScene( pSceneTemplet->m_iID, 2 );
#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
				// June @323e293d..323e295e: a story quest then replays its slide show ( g_pData +0x288 328c3750; its result is not used )
				if( NULL != pSceneTemplet && CX2QuestManager::QT_STORY == pSceneTemplet->m_eQuestType )
					g_pData->StartStorySlideShot( pSceneTemplet->m_iID );
#endif SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
			}
#else SERV_IRUHADEV_JUNE_QUEST_SCENE
			if( m_iSelectedQuestID != -1 )
			{
				const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
				if( NULL != pQuestTemplet )
				{
					g_pData->GetEventScene()->PlayEventScene(pQuestTemplet->m_wstrStartScene);
					g_pData->GetEventScene()->PlayEventScene(pQuestTemplet->m_wstrEndScene);
				}
			}
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE
		}
		return true;
	case UQNCM_QUEST_COMPLETE:
		{
			OpenQuestRewardWindow( m_iSelectedQuestID, GetThereIsSelectionReward() );
		}
		return true;
	case UQNCM_QUEST_ADMIN_COMPLETE:
		{
			if( CX2User::XUAL_DEV == g_pData->GetMyUser()->GetAuthLevel() )
			{
				m_bAdminQuestComplete = true;
				if( m_iSelectedQuestID != -1 )
				{
					const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
					if( NULL != pQuestTemplet )
					{
						bool bThereIsSelectionReward = false;

						CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
						CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
						for( UINT i = 0; i < pQuestTemplet->m_SelectReward.m_vecSelectItem.size(); i++)
						{
							const CX2QuestManager::ITEM_DATA& itemData = pQuestTemplet->m_SelectReward.m_vecSelectItem[i];
							const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemData.m_iItemID );
							if( pItemTemplet == NULL )
								continue;

							else if( pItemTemplet->GetUnitType() != CX2Unit::UT_NONE )
							{
								if( pItemTemplet->GetUnitType() != eUnitType )
								{
									continue;
								}
							}
#ifdef QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
							if( pItemTemplet->GetUnitClass() != CX2Unit::UC_NONE )
							{
								CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
								if( false == GetX2UnitClassCompatibility(eUnitClass, pItemTemplet->GetUnitClass() ) )
								{
									continue;
								}
							}
#endif QUEST_REWARD_ITEM_CLASS_TYPE_CHECK

							bThereIsSelectionReward = true;
							if( true == bThereIsSelectionReward )
								break;
						}

						//g_pKTDXApp->GetDeviceManager()->PlaySound( L"CompleteQuest.ogg", false, false );
						g_pKTDXApp->GetDeviceManager()->PlaySound( L"QuestReward.ogg", false, false );

						OpenQuestRewardWindow( m_iSelectedQuestID, bThereIsSelectionReward );
					}
				}
				else
				{
					return true;
				}
			}

		}
		return true;
	case UQNCM_QUEST_GIVE_UP:
		{
			if( m_iSelectedQuestID != -1 )
			{
				const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iSelectedQuestID );
				if( NULL != pQuestInst )
				{
					if( m_pDLGMsgBox != NULL )
					{
						g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
						m_pDLGMsgBox = NULL;
					}

					m_pDLGMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_888 ), UQNCM_QUEST_GIVE_UP_CONFIRM, g_pMain->GetNowState(), UQNCM_QUEST_GIVE_UP_CANCLE );
					CKTDGUIButton* pButton_Cancle = (CKTDGUIButton*) m_pDLGMsgBox->GetControl( L"MsgBoxOkAndCancelCancelButton" );
					pButton_Cancle->RequestFocus();


				}
			}

			return true;

		}
		return true;
	case UQNCM_QUEST_GIVE_UP_CONFIRM:
		{
			if( m_pDLGMsgBox != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
				m_pDLGMsgBox = NULL;
			}

			if( m_iSelectedQuestID != -1 )
			{
				const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iSelectedQuestID );
				if( NULL != pQuestInst )
				{	
					return Handler_EGS_GIVE_UP_QUEST_REQ( m_iSelectedQuestID );
				}
			}
			return true;
		}
		return true;
	case UQNCM_QUEST_GIVE_UP_CANCLE:
		{
			if( m_pDLGMsgBox != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
				m_pDLGMsgBox = NULL;
			}
		}
		return true;
	case UQNCM_QUESTCLEAR_OK:
		{
			// 선택보상 선택 여부에서 완료 후 닫기까지..
			map<int, int> mapSelectedItem;
			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );

			if( true == m_bLastRewardWasSelection )
			{
				// 선택된 보상 아이템들의 ID를 서버로 보낸다
				if( m_pDLGUIQuestClear != NULL )
				{

					if( NULL == pQuestTemplet )
					{
						return false;
					}					

					CKTDGUIRadioButton* pRadioButton1 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_1" );
					CKTDGUIRadioButton* pRadioButton2 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_2" );
					CKTDGUIRadioButton* pRadioButton3 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_3" );

					vector<int> veciCheckedItemTID;
					if( true == pRadioButton1->GetChecked() )
					{
						veciCheckedItemTID.push_back( pRadioButton1->GetDummyInt(0) );
					}
					if( true == pRadioButton2->GetChecked() )
					{
						veciCheckedItemTID.push_back( pRadioButton2->GetDummyInt(0) );
					}
					if( true == pRadioButton3->GetChecked() )
					{
						veciCheckedItemTID.push_back( pRadioButton3->GetDummyInt(0) );
					}

					// 선택 보상이 있다고 되어 있는데 선택을 하나도 안 한 경우. User가 선택 취소를 하는 방법은 없으므로 서버에서 0을 보낸 경우이다.
					// 따라서 현재는 완료할 수 없는 퀘스트.
					if( 0 == pQuestTemplet->m_SelectReward.m_iSelectionCount )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_892 ), g_pMain->GetNowState() );
						// 완료할 수 없는 퀘스트. 퀘스트 완료창을 닫아준다.
						CloseQuestRewardWindow();

						return true;
					}

					if( veciCheckedItemTID.size() != pQuestTemplet->m_SelectReward.m_iSelectionCount )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_893 ), g_pMain->GetNowState() );

						CKTDGUIStatic* pStaticSelectionReward = (CKTDGUIStatic*) m_pDLGUIQuestClear->GetControl( L"Static_SelectionRewardSlot" );

						for ( int i = 0; i < m_byNumOfItemThatICanGet; i++ )
						{
							pStaticSelectionReward->GetPicture( i )->SetFlicker( 0.5f, 1.4f, 0.2f );
						}

						return true;
					}


					for( UINT i=0; i<veciCheckedItemTID.size(); i++ )
					{
						// 선택된 아이템의 TID를 맵에 넣어 준다.
						mapSelectedItem[ veciCheckedItemTID[i] ] = veciCheckedItemTID[i];
					}
				}
			}

			if( CX2User::XUAL_DEV == g_pData->GetMyUser()->GetAuthLevel() && true == m_bAdminQuestComplete )
			{
				Handler_EGS_ADMIN_QUEST_COMPLETE_REQ( m_eNPCID, m_iSelectedQuestID, mapSelectedItem );
			}
			else
			{
				Handler_EGS_QUEST_COMPLETE_REQ( m_eNPCID, m_iSelectedQuestID, mapSelectedItem );
			}

			CloseQuestRewardWindow();
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			HideMovieTab();			// June @323e331d / @323e335a: after the complete request ( Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
		}
		return true;
	case UQNCM_SCROOL_UP:
		{
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			// June 323e1bf0 case 0xa02f @323e3364: on the Video tab, three movies up ( not below 1 ) while there are more than 13 rows
			if( true == m_bMovieTab )
			{
				if( m_iMovieCount > 13 )
				{
					m_iMovieStartID -= 3;
					if( m_iMovieStartID <= 1 )
						m_iMovieStartID = 1;
					UpdateMovieList( true );
				}
				return true;
			}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			m_iTopQuestSlotIndex--;
			ResetQuestUI();

		}
		return true;
	case UQNCM_SCROOL_DOWN:
		{
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			// June 323e1bf0 case 0xa030 @323e34a7: on the Video tab, three movies down ( at most the row count ) while there are more than 13 rows
			if( true == m_bMovieTab )
			{
				if( m_iMovieCount > 13 )
				{
					m_iMovieStartID += 3;
					if( m_iMovieStartID >= m_iMovieCount )
						m_iMovieStartID = m_iMovieCount;
					UpdateMovieList( true );
				}
				return true;
			}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			m_iTopQuestSlotIndex++;
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
			// June 323e1bf0 case 0xa030: no scrolling down while 14 rows or fewer are listed
			if( false == m_vecQuestInfoList.empty() && m_vecQuestInfoList.size() <= 14 )
				m_iTopQuestSlotIndex--;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
			ResetQuestUI();

		}
		return true;
	case UQNCM_REWARD_CHECK_ITEM:
		{
		}
		return true;
	case UQNCM_REWARD_UNCHECK_ITEM:
		{
		}
		return true;
	case UQNCM_REWARD_MOUSE_OVER:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*) lParam;
			CKTDGUIDialogType pDialog = (CKTDGUIDialogType)pControl->GetDialog(); 

			int ItemID = pControl->GetDummyInt(0);
			int SocketID = pControl->GetDummyInt(1);
#ifdef QUEST_REWARD_PERIOD
			int Period = pControl->GetDummyInt(2);
#endif QUEST_REWARD_PERIOD

			if( 0 == wcscmp( pControl->GetName(), L"Need_Slot1" ) )
			{
				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;
				D3DXVECTOR2 vPos = pDialog->GetPos() + pButton->GetPos();
				OpenRewardItemInfoPopup( true, vPos, ItemID );
			}
			else if( 0 == wcscmp( pControl->GetName(), L"Reward_Slot_1" ) )
			{
				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;
				D3DXVECTOR2 vPos = pDialog->GetPos() + pButton->GetPos();
#ifdef QUEST_REWARD_PERIOD
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID, Period );
#else
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID );
#endif QUEST_REWARD_PERIOD
			}
			else if( 0 == wcscmp( pControl->GetName(), L"Reward_Slot_2" ) )
			{
				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;
				D3DXVECTOR2 vPos = pDialog->GetPos() + pButton->GetPos();
#ifdef QUEST_REWARD_PERIOD
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID, Period );
#else
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID );
#endif QUEST_REWARD_PERIOD
			}
			else if( 0 == wcscmp( pControl->GetName(), L"Reward_Slot_3" ) )
			{
				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;
				D3DXVECTOR2 vPos = pDialog->GetPos() + pButton->GetPos();
#ifdef QUEST_REWARD_PERIOD
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID, Period );
#else
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID );
#endif QUEST_REWARD_PERIOD
			}

			else if( 0 == wcscmp( pControl->GetName(), L"Selection_Reward_Slot_1" ) )
			{
				CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)lParam;

				D3DXVECTOR2 vPos = pDialog->GetPos() + pRadioButton->GetPos();

#ifdef QUEST_REWARD_PERIOD
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID, Period );
#else
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID );
#endif QUEST_REWARD_PERIOD
			}
			else if( 0 == wcscmp( pControl->GetName(), L"Selection_Reward_Slot_2" ) )
			{
				CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)lParam;


				D3DXVECTOR2 vPos = pDialog->GetPos() + pRadioButton->GetPos();

#ifdef QUEST_REWARD_PERIOD
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID, Period );
#else
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID );
#endif QUEST_REWARD_PERIOD
			}
			else if( 0 == wcscmp( pControl->GetName(), L"Selection_Reward_Slot_3" ) )
			{
				CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)lParam;


				D3DXVECTOR2 vPos = pDialog->GetPos() + pRadioButton->GetPos();

#ifdef QUEST_REWARD_PERIOD
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID, Period );
#else
				OpenRewardItemInfoPopup( true, vPos, ItemID, SocketID );
#endif QUEST_REWARD_PERIOD
			}
		}
		return true;
	case UQNCM_REWARD_MOUSE_OUT:
		{
			OpenRewardItemInfoPopup( false, D3DXVECTOR2(0, 0), -1 );
		}
		return true;
	case UQNCM_NAVI_LCLICKED:
	case UQNCM_NAVI_DRAGGING:		// 둘 다 처리할 건 어차피 같으니까..
		{
			CKTDGUINavigation* pNavi	= (CKTDGUINavigation*)lParam;
			D3DXVECTOR2 vRelativePos = pNavi->GetRelativeWindowPos();
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			// June 323e1bf0 cases 0xa035 / 0xa036 @323e39c0: on the Video tab, while there are more than 12 rows the first movie follows the
			// navi, 1 - 3 * (int)( navi steps * y / -0.68 ), with no clamp; the list is rebuilt without a navi update
			if( true == m_bMovieTab )
			{
				if( m_iMovieCount > 12 )
				{
					int iMovieStartID = 1 - 3 * (int)( (double)m_iMovieNaviStep * (double)vRelativePos.y / -0.68 );
					if( iMovieStartID != m_iMovieStartID )
					{
						m_iMovieStartID = iMovieStartID;
						UpdateMovieList( false );
					}
				}
				return true;
			}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB

			// 0.68 : Navi Size에 따른 값. MAGIC VALUE
			m_iTopQuestSlotIndex = (int)((vRelativePos.y / 0.68f) * (m_iSizeQuestInfoList - _CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage + 1));
			ResetQuestUI(false);

		}
		return true;
#ifdef REFORM_QUEST
	case UQNCM_SELECT_QUEST_EVENT_TAB: //에픽 퀘스트
		{
			SetNowQuestUIMode(QUM_EVENT_QUEST);
			m_iTopQuestSlotIndex = 0;
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			m_bMovieTab = false;		// June @323e1dd0 ( Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
			ResetQuestUI();
		}
		return true;
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	case UQNCM_SELECT_QUEST_GUILD_TAB:
		{
			SelectTab( QUM_GUILD_QUEST );
		}
		return true;
	case UQNCM_SELECT_QUEST_STORY_TAB:
		{
			SelectTab( QUM_STORY_QUEST );
		}
		return true;
	case UQNCM_SKIP_EPIC_QUEST:
		{
			OpenSkipEpicMsgBox();
		}
		return true;
	case UQNCM_SKIP_EPIC_QUEST_OK:
		{
			SkipEpicQuestOk();
		}
		return true;
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	// June 323e1bf0 cases 41038-41042 ( jump table 323e4504 ): the Video tab ( quest-system Q8b )
	case UQNCM_SELECT_SCENARIO_MOVIE_TAB:
		return OpenMovieTab();						// @323e3cb7
	case UQNCM_SELECT_SCENARIO_MOVIE_PLAY:
		{
			PlayMovieFromTab( (CKTDGUIControl*)lParam );	// @323e4055
		}
		return true;
	case UQNCM_SELECT_SCENARIO_MOVIE_AREA_TAB:
		return true;									// 323e1c60: nothing but true
	case UQNCM_SELECT_SCENARIO_MOVIE_BUTTON_OVER:
		{
			MarkMovieButton( (CKTDGUIControl*)lParam, false );	// @323e4117
		}
		return true;
	case UQNCM_SELECT_SCENARIO_MOVIE_BUTTON_OUT:
		{
			MarkMovieButton( (CKTDGUIControl*)lParam, true );	// @323e41d8
		}
		return true;
#else SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	case UQNCM_SELECT_SCENARIO_MOVIE_TAB:
	case UQNCM_SELECT_SCENARIO_MOVIE_PLAY:
	case UQNCM_SELECT_SCENARIO_MOVIE_AREA_TAB:
	case UQNCM_SELECT_SCENARIO_MOVIE_BUTTON_OVER:
	case UQNCM_SELECT_SCENARIO_MOVIE_BUTTON_OUT:
		{
			// the scenario-movie tab ( June 323ca6d0 / 323dfc00 ) is Q8's: nothing to show yet
			CX2OfflineLog::Server( L"[JUNE_QUEST] UI scenario-movie message %d ignored ( Q8 owns the movie tab )", (int)wParam );
		}
		return true;
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
#endif SERV_IRUHADEV_JUNE_QUEST_UI
	case UQNCM_QUEST_INSTANTLY_COMPLETE:
		{			
			if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() ||
				CX2Main::XS_PVP_GAME == g_pMain->GetNowStateID() )
			{
				return true;
			}

			//겹치는 UI(던전, 대전도구)가 열려 있을 때 마우스 동작 하지 않도록 설정
			if( NULL != g_pMain && NULL != g_pMain->GetPartyUI() &&
				true == g_pMain->GetPartyUI()->GetShowDungeonMenu() || true == g_pMain->GetPartyUI()->GetShowPVPMenu() )
			{
				return true;
			}

			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iSelectedQuickQuestIndex = pButton->GetDummyInt(0);
			m_iSelectedQuestID = m_vecUIQuickQuest[iSelectedQuickQuestIndex].m_QuestID;
			OpenQuestRewardWindow( m_iSelectedQuestID, GetThereIsSelectionReward() );
			/* //즉시 완료시에 퀘스트 UI 열기
			SetNowQuestUIMode(QUM_QUEST_DETAIL);
			ResetQuestUI();
			if(m_bShow == false)
			{
				m_bShow = true;
				g_pData->GetUIManager()->UIOpened(CX2UIManager::UI_MENU_QUEST_NEW);
			}
			*/			
		}
		return true;
#endif //REFORM_QUEST
	default:
		break;
	}
	return false;
}

bool CX2UIQuestNew::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	switch( wParam )
	{
	case EGS_TALK_WITH_NPC_ACK:
		{
			return Handler_EGS_TALK_WITH_NPC_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_GIVE_UP_QUEST_ACK:
		{
			return Handler_EGS_GIVE_UP_QUEST_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_QUEST_COMPLETE_ACK:
		{
			return Handler_EGS_QUEST_COMPLETE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_NEW_QUEST_ACK:
		{
			return Handler_EGS_NEW_QUEST_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_NEW_QUEST_NOT:
		{
			return Handler_EGS_NEW_QUEST_NOT( hWnd, uMsg, wParam, lParam );

		} break;
#ifdef SERV_ENTER_FIELD_QUEST_CLEAR
	case EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK:
		{
			return Handler_EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK( hWnd, uMsg, wParam, lParam );
		} break;
#endif //SERV_ENTER_FIELD_QUEST_CLEAR
#ifdef SERV_POINT_COUNT_SYSTEM
	case EGS_QUEST_POINT_COUNT_SYSTEM_NOT:
		{
			Handler_EGS_QUEST_POINT_COUNT_SYSTEM_NOT( hWnd, uMsg, wParam, lParam );
		} break;
#endif //SERV_POINT_COUNT_SYSTEM

	}
	return false;
}

bool CX2UIQuestNew::OnMouseWheel( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	D3DXVECTOR2 mousePos;

	mousePos.x = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos();
	mousePos.y = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos();

	short zDelta = (short)HIWORD(wParam);
	m_SumDelta += zDelta;


	if( MouseOnQuestListArea(mousePos) )
	{
		while (abs(m_SumDelta) >= WHEEL_DELTA)
		{
			if(m_SumDelta>0)
			{
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
				// June 323e45c0 @323e465f: on the Video tab, three movies up ( not below 1 ) while there are more than 12 rows
				if( true == m_bMovieTab )
				{
					if( m_iMovieCount > 12 )
					{
						m_iMovieStartID -= 3;
						if( m_iMovieStartID < 2 )
							m_iMovieStartID = 1;
						UpdateMovieList( true );
					}
					m_SumDelta -= WHEEL_DELTA;
					continue;
				}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
				m_iTopQuestSlotIndex--;
				ResetQuestUI();
				m_SumDelta -= WHEEL_DELTA;
			}
			else
			{
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
				// June 323e45c0 @323e47b0: on the Video tab, three movies down ( at most the row count ) while there are more than 12 rows
				if( true == m_bMovieTab )
				{
					if( m_iMovieCount > 12 )
					{
						m_iMovieStartID += 3;
						if( m_iMovieStartID >= m_iMovieCount )
							m_iMovieStartID = m_iMovieCount;
						UpdateMovieList( true );
					}
					m_SumDelta += WHEEL_DELTA;
					continue;
				}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
				m_iTopQuestSlotIndex++;
				ResetQuestUI();
				m_SumDelta += WHEEL_DELTA;
			}	
		}
		return true;
	}
	return false;
}


bool CX2UIQuestNew::Handler_EGS_TALK_WITH_NPC_REQ( CX2UnitManager::NPC_UNIT_ID questNPCID )
{
	KEGS_TALK_WITH_NPC_REQ kPacket;
	kPacket.m_iNPCID	= (int)questNPCID;

	if(questNPCID == CX2UnitManager::NUI_MOON)
		return true;

	g_pData->GetServerProtocol()->SendPacket( EGS_TALK_WITH_NPC_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_TALK_WITH_NPC_ACK );

	return true;
}



bool CX2UIQuestNew::Handler_EGS_TALK_WITH_NPC_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323e4930: after the talk with an NPC the window goes back to the tab of the selected quest's type
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_TALK_WITH_NPC_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_TALK_WITH_NPC_ACK ) == true && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		if( true == m_bShow )
		{
			ReturnToQuestListUI();

			QUEST_UI_MODE eMode = QUM_QUEST;
			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
			if( NULL != pQuestTemplet )
			{
				switch( pQuestTemplet->m_eQuestType )
				{
				case CX2QuestManager::QT_EVENT:	eMode = QUM_EVENT_QUEST;	break;
				case CX2QuestManager::QT_EPIC:	eMode = QUM_EPIC_QUEST;		break;
				case CX2QuestManager::QT_GUILD:	eMode = QUM_GUILD_QUEST;	break;
				case CX2QuestManager::QT_STORY:	eMode = QUM_STORY_QUEST;	break;
				}
			}
			SetNowQuestUIMode( eMode );
			ResetQuestUI( true, false, true );
		}
		return true;
	}
	return false;
#else
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_TALK_WITH_NPC_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_TALK_WITH_NPC_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{

			if(m_bShow)
			{
				ReturnToQuestListUI();
				SetNowQuestUIMode(QUM_QUEST);
				ResetQuestUI(true, false);
			}
			
			return true;
		}
	}

	return false;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}


bool CX2UIQuestNew::Handler_EGS_ADMIN_QUEST_COMPLETE_REQ( CX2UnitManager::NPC_UNIT_ID questNPCID, int questID, std::map<int,int> mapSelectItemID )
{
	KEGS_QUEST_COMPLETE_REQ kPacket;
	kPacket.m_iTalkNPCID	= (int)questNPCID;
	kPacket.m_iQuestID		= questID;
	kPacket.m_mapSelectItemID	= mapSelectItemID;

#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE
	// June 323d1fc0 @323d206f-323d2183: the quest's clear movie first ( 31d7a240, admin 1 ), then the seen list ( 31d75950 -> 31d78bf0( id, 1 ) ).
	// A movie that starts ( 31d880b0 ) parks this request: it is sent when the movie ends ( 31d883a0 -> 31d822b0 ) and +0x178
	// ( m_bAdminQuestComplete ) is cleared now. One that fails to start is stopped at once, which sends it; the plain send below then
	// sends it again ( JUNE_BUGS ). June reads g_pMain +0x80a without its own null test.
	if( NULL != g_pMain )
	{
		g_pMain->PlayQuestClearMovie( kPacket, true );
		int iMovieID = 0;
		if( true == g_pMain->GetClearMovieIDByQuest( questID, iMovieID ) )
			g_pMain->AddSeenClearMovie( iMovieID, true );
		if( true == g_pMain->IsClearMoviePlaying() && false == g_pMain->StartClearMovie() )
			g_pMain->StopClearMovie();
		if( true == g_pMain->IsClearMoviePlaying() )
		{
			m_bAdminQuestComplete = false;
			return true;
		}
	}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_QUEST_COMPLETE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_QUEST_COMPLETE_ACK );

	m_bAdminQuestComplete = false;

	return true;
}


bool CX2UIQuestNew::Handler_EGS_QUEST_COMPLETE_REQ( CX2UnitManager::NPC_UNIT_ID questNPCID, int questID, std::map<int,int> mapSelectItemID )
{
	KEGS_QUEST_COMPLETE_REQ kPacket;
	kPacket.m_iTalkNPCID	= (int)questNPCID;
	kPacket.m_iQuestID		= questID;
	kPacket.m_mapSelectItemID	= mapSelectItemID;

#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE
	// June 323d24f0 @323d259c-323d26b2: as the admin request above ( 31d7a240 with admin 0, the seen list, start or stop ), with no +0x178
	// write. A started movie parks the request; quest 22336 -> Movie_Elysion_Clear.avi, 22236 -> Movie_Atlas_Clear.avi.
	if( NULL != g_pMain )
	{
		g_pMain->PlayQuestClearMovie( kPacket, false );
		int iMovieID = 0;
		if( true == g_pMain->GetClearMovieIDByQuest( questID, iMovieID ) )
			g_pMain->AddSeenClearMovie( iMovieID, true );
		if( true == g_pMain->IsClearMoviePlaying() && false == g_pMain->StartClearMovie() )
			g_pMain->StopClearMovie();
		if( true == g_pMain->IsClearMoviePlaying() )
			return true;
	}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE
	g_pData->GetServerProtocol()->SendPacket( EGS_QUEST_COMPLETE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_QUEST_COMPLETE_ACK );

	return true;
}

//{{ kimhc // 2010.4.5 // 비밀던전 작업(일일퀘스트 시스템)
#ifdef	SERV_DAILY_QUEST

bool CX2UIQuestNew::Handler_EGS_QUEST_COMPLETE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_QUEST_COMPLETE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	// June 323e57e0: quest 11000 is accepted even when no request is waiting for it
	if( g_pMain->DeleteServerPacket( EGS_QUEST_COMPLETE_ACK ) == true || 11000 == kEvent.m_kCompleteQuestInfo.m_iQuestID )
#else SERV_IRUHADEV_JUNE_QUEST_UI
	if( g_pMain->DeleteServerPacket( EGS_QUEST_COMPLETE_ACK ) == true )
#endif SERV_IRUHADEV_JUNE_QUEST_UI
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetQuestManager()->RemoveUnitQuest( kEvent.m_kCompleteQuestInfo.m_iQuestID, true );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
			g_pData->GetQuestManager()->RefreshVillageNpcRelationDataByQuest( kEvent.m_kCompleteQuestInfo.m_iQuestID );	// June 323e57e0 @323e5903 (Atlas E1c)
#endif SERV_IRUHADEV_ELYSION_VILLAGE

			g_pData->GetMyUser()->GetSelectUnit()->Reset( kEvent.m_kUpdateUnitInfo.m_kUnitInfo );
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_kUpdateUnitInfo.m_vecKInventorySlotInfo );
			if( NULL != g_pData->GetUIManager()->GetUIInventory() )
			{
				g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_kUpdateUnitInfo.m_vecKInventorySlotInfo );
			}

			m_mapNewQuest[ kEvent.m_kCompleteQuestInfo.m_iQuestID ] = false;
			//UpdateQuestList();

			// 반복 퀘스트가 아니라면 들고 있는 퀘스트 ID를 -1로 리셋해주자.
			// 반복 퀘스트이면서 목록에 있는 퀘스트라면 선택된 퀘스트 ID를 리셋하지 않는다 : 엔터 연타로 되게.

#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
			const CX2QuestManager::QuestTemplet* pCompletedQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kEvent.m_kCompleteQuestInfo.m_iQuestID );
			if( NULL == pCompletedQuestTemplet )
				QuestNoTempletLog( kEvent.m_kCompleteQuestInfo.m_iQuestID, L"Handler_EGS_QUEST_COMPLETE_ACK" );

			if( NULL != pCompletedQuestTemplet && CX2QuestManager::QRT_REPEAT == pCompletedQuestTemplet->m_eRepeatType &&
#else SERV_IRUHADEV_JUNE_QUEST_DATA
			if( CX2QuestManager::QRT_REPEAT == g_pData->GetQuestManager()->GetQuestTemplet(kEvent.m_kCompleteQuestInfo.m_iQuestID)->m_eRepeatType &&
#endif SERV_IRUHADEV_JUNE_QUEST_DATA
				find( m_vecAvailableQuestID.begin(), m_vecAvailableQuestID.end(), kEvent.m_kCompleteQuestInfo.m_iQuestID ) != m_vecAvailableQuestID.end() )
			{
				m_iSelectedQuestID = kEvent.m_kCompleteQuestInfo.m_iQuestID;
			}
			else
			{
				m_iSelectedQuestID = -1;
			}

			//UpdateQuestUI();

			// 사운드
			//g_pKTDXApp->GetDeviceManager()->PlaySound( L"QuestNew.ogg", false, false );

			//NotifyQuestListUpdate();
			//UpdateQuestListPage( m_iNowQuestListPage );
			//CloseSubQuestList();
			//CloseQuestDesc();


			ReturnToQuestListUI();
			ResetQuestUI(true, true);

#ifdef SERV_IRUHADEV_JUNE_QUEST_SCENE
			// June 323e57e0 @323e599e..323e59fe: the quest's end scene ( 323cb2e0( id, 1 ) ). The Lua scene of m_wstrEndScene is not played ( JUNE_QUEST_SCENE ).
			PlayQuestScene( kEvent.m_kCompleteQuestInfo.m_iQuestID, 1 );
#else SERV_IRUHADEV_JUNE_QUEST_SCENE
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
			wstring wstrEndSceneID = ( NULL != pCompletedQuestTemplet ) ? pCompletedQuestTemplet->m_wstrEndScene : wstring();
#else SERV_IRUHADEV_JUNE_QUEST_DATA
			wstring wstrEndSceneID = g_pData->GetQuestManager()->GetQuestTemplet(kEvent.m_kCompleteQuestInfo.m_iQuestID)->m_wstrEndScene;
#endif SERV_IRUHADEV_JUNE_QUEST_DATA

			if(wstrEndSceneID != L"")
			{
				g_pData->GetEventScene()->PlayEventScene(wstrEndSceneID);
			}
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE
#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY
			// June 323e57e0 @323e5a03..323e5a6d: a completed QT_STORY quest that has a story ( 326a7d50 ) rebuilds the story book with the
			// Story_New mark on ( 32335f50( 1 ) ). June then starts the story's slide show ( g_pData +0x288 328c3750 ) and, when it started,
			// sets g_pInstanceData +0xabc ( the end message ): Phase Q10b ports the player.
			{
				const CX2QuestManager::QuestTemplet* pStoryTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kEvent.m_kCompleteQuestInfo.m_iQuestID );
				if( NULL != pStoryTemplet && CX2QuestManager::QT_STORY == pStoryTemplet->m_eQuestType &&
					true == CX2SlideShotScriptedManager::GetInstance()->HasStory( pStoryTemplet->m_iID ) )
				{
					g_pData->GetQuestManager()->RebuildStoryBookQuest( true );
#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
					// June @323e5a4a..323e5a6d: the story's slide show; when Start returns true ( started, or one already running ) the end message is due
					if( true == g_pData->StartStorySlideShot( pStoryTemplet->m_iID ) )
						g_pInstanceData->SetStoryEndMessage( true );
#endif SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
				}
			}
#endif SERV_IRUHADEV_JUNE_QUEST_STORY

			// 미니맵 아이콘 업뎃해주고
			UpdateNewQuestNotice();

			// 전직 퀘스트라면 : 스킬트리 업데이트
			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kEvent.m_kCompleteQuestInfo.m_iQuestID );
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
			if( NULL != pQuestTemplet && CX2QuestManager::QT_CHANGE_JOB == pQuestTemplet->m_eQuestType && CX2Unit::UC_NONE != pQuestTemplet->m_Reward.m_eChangeUnitClass )
#else SERV_IRUHADEV_JUNE_QUEST_DATA
			if( CX2QuestManager::QT_CHANGE_JOB == pQuestTemplet->m_eQuestType && CX2Unit::UC_NONE != pQuestTemplet->m_Reward.m_eChangeUnitClass )
#endif SERV_IRUHADEV_JUNE_QUEST_DATA
			{
				// 스킬트리 업데이트. 새로 초기화 시켜 준다.
				g_pData->GetUIManager()->GetUISkillTree()->InitSkillTreeUI();

				//g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_894 ) );
				//g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_CLASS_CHANGE, NULL, false );

				g_pData->GetClassChangePopup().SetShow( true );

				CX2State* pState = (CX2State*) g_pMain->GetNowState();
				pState->ResetUnitViewerInFieldSquare( g_pData->GetMyUser()->GetSelectUnit()->GetUID(), g_pData->GetMyUser()->GetSelectUnit()->GetClass() );
				//{{ Iruha : 2026-09-04 // HUD portrait after a quest job change
#ifdef SERV_IRUHADEV_JOBCHANGE_PORTRAIT
				// ResetUnitViewerInFieldSquare above rebuilds the 3D square unit; the
				// top-left gage portrait is a separate thing and nothing here touched
				// it, so it kept drawing the old class until the gage was rebuilt -
				// which only happens on a state change, i.e. re-selecting the
				// character. This is a gap in the shipped client, not in the server:
				// the GameServer sends only EGS_QUEST_COMPLETE_ACK for a quest-driven
				// class change (UserQuestManager.cpp:5038-5142), and every OTHER
				// class-change path in the client refreshes the portrait itself -
				// EGS_CHANGE_MY_UNIT_INFO_NOT (X2State.cpp:12958), the admin change
				// (:5083), and the jumping character (:13174), whose comment there is
				// literally "refresh the character portrait".
				//
				// GetMyGageData() is the probe, not GetInstance() on its own:
				// CX2GageManager::SetCharacterImage dereferences m_ptrMyGageSet with
				// no null check, unlike almost every neighbour in that same header,
				// and there is no gage at all in some states.
				if( NULL != CX2GageManager::GetInstance() &&
					NULL != CX2GageManager::GetInstance()->GetMyGageData() )
				{
					const CX2Unit::UNIT_CLASS eNewClass =
						g_pData->GetMyUser()->GetSelectUnit()->GetClass();
				
					CX2GageManager::GetInstance()->SetCharacterImage( eNewClass );
#ifdef FIX_CHUNG_GAGE_UI_UPDATE_BUG
					CX2GageManager::GetInstance()->ResetGageUIEtc( eNewClass );
#endif // FIX_CHUNG_GAGE_UI_UPDATE_BUG
				}
#endif SERV_IRUHADEV_JOBCHANGE_PORTRAIT
//}}
			}
			 //도움말 입력
			{
				if( pQuestTemplet->m_iNextVillageID != 0 && NULL != g_pData && NULL != g_pData->GetPlayGuide())
				{	//다음마을 가이드에 대한 ID가 기록되어있는 퀘스트를 완료 했을 시 마을이동 가이드 출력
					g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_COMPLETE_EPIC_QUEST, true);
					g_pData->GetPlayGuide()->SetVillageID(pQuestTemplet->m_iNextVillageID);
				}
			}
#ifdef REFORM_QUEST
			if( NULL != g_pData && NULL != g_pData->GetPlayGuide() &&
				true == g_pData->GetPlayGuide()->GetActiveNoviceGuide() )
			{
				if( CX2PlayGuide::TQI_CHASE_THIEF == kEvent.m_kCompleteQuestInfo.m_iQuestID )
				{
					g_pMain->GetPartyUI()->OpenPartyMenu(true);		
					g_pData->GetPlayGuide()->SetShowDungeonMenu(true);
					g_pData->GetPlayGuide()->SetMouseClickIndex( CX2PlayGuide::NGS_STEP_5 );;
				}
				else if( CX2PlayGuide::TQI_SAVE_EL == kEvent.m_kCompleteQuestInfo.m_iQuestID )
				{
					g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_NOVICE_GUIDE_01, false);
					g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_NOVICE_GUIDE_02, false);					
				}
			}
#endif //REFORM_QUEST
			g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_QUEST_COMPLETE, false);
			return true;
		}
	}

	return false;
}

#else	SERV_DAILY_QUEST

bool CX2UIQuestNew::Handler_EGS_QUEST_COMPLETE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_QUEST_COMPLETE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_QUEST_COMPLETE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetQuestManager()->RemoveUnitQuest( kEvent.m_iQuestID, true );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
			g_pData->GetQuestManager()->RefreshVillageNpcRelationDataByQuest( kEvent.m_iQuestID );	// June 323e57e0 @323e5903 (Atlas E1c)
#endif SERV_IRUHADEV_ELYSION_VILLAGE

			g_pData->GetMyUser()->GetSelectUnit()->Reset( kEvent.m_kUpdateUnitInfo.m_kUnitInfo );
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_kUpdateUnitInfo.m_vecKInventorySlotInfo );
			if( NULL != g_pData->GetUIManager()->GetUIInventory() )
			{
				g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_kUpdateUnitInfo.m_vecKInventorySlotInfo );
			}

			m_mapNewQuest[ kEvent.m_iQuestID ] = false;
			//UpdateQuestList();

			// 반복 퀘스트가 아니라면 들고 있는 퀘스트 ID를 -1로 리셋해주자.
			// 반복 퀘스트이면서 목록에 있는 퀘스트라면 선택된 퀘스트 ID를 리셋하지 않는다 : 엔터 연타로 되게.

			if( true == g_pData->GetQuestManager()->GetQuestTemplet(kEvent.m_iQuestID)->m_bRepeat &&
				find( m_vecAvailableQuestID.begin(), m_vecAvailableQuestID.end(), kEvent.m_iQuestID ) != m_vecAvailableQuestID.end() )
			{
				m_iSelectedQuestID = kEvent.m_iQuestID;
			}
			else
			{
				m_iSelectedQuestID = -1;
			}

			//UpdateQuestUI();


			ReturnToQuestListUI();
			ResetQuestUI(true, true);


			wstring wstrEndSceneID = g_pData->GetQuestManager()->GetQuestTemplet(kEvent.m_iQuestID)->m_wstrEndScene;

			if(wstrEndSceneID != L"")
			{
				g_pData->GetEventScene()->PlayEventScene(wstrEndSceneID);
			}

			//g_pData->GetEventScene()->PlayEventScene(L"10000_01");
			

			// 사운드
			//g_pKTDXApp->GetDeviceManager()->PlaySound( L"QuestNew.ogg", false, false );

			//NotifyQuestListUpdate();
			//UpdateQuestListPage( m_iNowQuestListPage );
			//CloseSubQuestList();
			//CloseQuestDesc();

			// 미니맵 아이콘 업뎃해주고
			UpdateNewQuestNotice();

			// 전직 퀘스트라면 : 스킬트리 업데이트
			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kEvent.m_iQuestID );
			if( CX2QuestManager::QT_CHANGE_JOB == pQuestTemplet->m_eQuestType && CX2Unit::UC_NONE != pQuestTemplet->m_Reward.m_eChangeUnitClass )
			{
				// 스킬트리 업데이트. 새로 초기화 시켜 준다.
				g_pData->GetUIManager()->GetUISkillTree()->InitSkillTreeUI();

				//g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_894 ) );
				//g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_CLASS_CHANGE, NULL, false );

				g_pData->GetClassChangePopup().SetShow( true );

				CX2State* pState = (CX2State*) g_pMain->GetNowState();
				pState->ResetUnitViewerInFieldSquare( g_pData->GetMyUser()->GetSelectUnit()->GetUID(), g_pData->GetMyUser()->GetSelectUnit()->GetClass() );
			}


			return true;
		}
	}

	return false;
}

#endif	SERV_DAILY_QUEST
//}} kimhc // 2010.4.5 // 비밀던전 작업(일일퀘스트 시스템)


bool CX2UIQuestNew::Handler_EGS_GIVE_UP_QUEST_REQ( int questID )
{
	KEGS_GIVE_UP_QUEST_REQ kPacket;
	kPacket.m_iQuestID	= questID;

	g_pData->GetServerProtocol()->SendPacket( EGS_GIVE_UP_QUEST_REQ, kPacket );
	//g_pMain->AddServerPacket( EGS_GIVE_UP_QUEST_ACK );

	return true;
}

#ifdef SERV_GATHER_GIVE_UP_QUEST 
bool CX2UIQuestNew::Handler_EGS_GATHER_GIVE_UP_QUEST_REQ( std::vector< int >& vecGatherQuestID )
{
	KEGS_GATHER_GIVE_UP_QUEST_REQ kPacket;
	kPacket.m_vecGatherQuestID	= vecGatherQuestID;

	g_pData->GetServerProtocol()->SendPacket( EGS_GATHER_GIVE_UP_QUEST_REQ, kPacket );
	//g_pMain->AddServerPacket( EGS_GIVE_UP_QUEST_ACK );

	return true;
}
#endif // SERV_GATHER_GIVE_UP_QUEST

bool CX2UIQuestNew::Handler_EGS_GIVE_UP_QUEST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GIVE_UP_QUEST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	if( 154 == kEvent.m_iOK )		// June 323e4aa0: this OK is dropped before IsValidPacket
		return false;
#endif SERV_IRUHADEV_JUNE_QUEST_UI

#ifdef SERV_SHARING_BANK_QUEST_CASH		// 지헌 : 임시 예외 처리 하자
	if(g_pData->GetMyUser()->IsSharingBankOpen() == true)
	{
		if( kEvent.m_iQuestID == 60720 || kEvent.m_iQuestID == 60710 || kEvent.m_iQuestID == 60700 )
		{
			g_pData->GetQuestManager()->RemoveUnitQuest( kEvent.m_iQuestID, false );
			UpdateNewQuestNotice();
			m_iSelectedQuestID = -1;
			ReturnToQuestListUI();
			ResetQuestUI(true, true);
			g_pMain->KTDGUIOKMsgBox(D3DXVECTOR2( -999, -999), GET_STRING( STR_ID_14842), g_pMain->GetNowState() );
		}
	}
#endif // SERV_SHARING_BANK_QUEST_CASH

	//리미트 레벨에 의한 퀘스트의 경우 REQ요청없이 ACK받기 때문에 제거
// 	if( g_pMain->DeleteServerPacket( EGS_GIVE_UP_QUEST_ACK ) == true )
// 	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetQuestManager()->RemoveUnitQuest( kEvent.m_iQuestID, false );
			
			UpdateNewQuestNotice();

			if(m_bShow)
			{
				m_iSelectedQuestID = -1;
				//UncheckAllQuestList();

				//UpdateQuestList();
				//UpdateQuestUI();
				// NPC 대사도..
				//UpdateNPCQuestDesc();
				ReturnToQuestListUI();
				ResetQuestUI(true, true);


			}
	#ifdef QUEST_GUIDE
			switch ( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
				{
					if( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetNpcIndicator() )
					{
						g_pTFieldGame->GetNpcIndicator()->ResetGuideTargetInfo();
					}
				} break;
			case CX2Main::XS_BATTLE_FIELD:
				{
					if( NULL != g_pX2Game && NULL != static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetNpcIndicator() )
					{
						static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetNpcIndicator()->ResetGuideTargetInfo();
					}
				} break;
			}
	#endif //QUEST_GUIDE

			UpdateQuickQuestDLG();
			CloseQuestRewardWindow();
			return true;
		}
//	}



	return false;
}


bool CX2UIQuestNew::Handler_EGS_NEW_QUEST_REQ( int iQuestID )
{
	KEGS_NEW_QUEST_REQ kPacket;
	kPacket.m_iTalkNPCID	= (int) m_eNPCID;
#ifdef MODIFY_ACCEPT_QUEST // 기본 수락 NPC를 게시판으로 설정
	if( kPacket.m_iTalkNPCID == CX2UnitManager::NUI_NONE )
		kPacket.m_iTalkNPCID = CX2UnitManager::NUI_BILLBOARD;
#endif // MODIFY_ACCEPT_QUEST

#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	// June 323d2b40: no NPC = the board, everywhere ( MODIFY_ACCEPT_QUEST is on in June, off in March )
	if( 0 == kPacket.m_iTalkNPCID )
		kPacket.m_iTalkNPCID = CX2UnitManager::NUI_BILLBOARD;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
	kPacket.m_iQuestID		= iQuestID;

	g_pData->GetServerProtocol()->SendPacket( EGS_NEW_QUEST_REQ, kPacket );
	

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet(iQuestID);
	if(pQuestTemplet != NULL)
	{
		if(pQuestTemplet->m_eQuestType == CX2QuestManager::QT_EPIC)
		{
			return true;
		}
	}

	g_pMain->AddServerPacket( EGS_NEW_QUEST_ACK );

	return true;
}


bool CX2UIQuestNew::Handler_EGS_NEW_QUEST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_NEW_QUEST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_NEW_QUEST_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
		}
		else
		{
			//UncheckAllQuestList();
		}

		// Handler_EGS_NEW_QUEST_NOT에서 처리
		//UpdateQuestList();
		//UpdateQuestUI();

// 		if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_QUEST))
// 		{
// 			g_pData->GetUIManager()->GetUIQuest()->ResetQuestUI();
// 		}

		return true;
	}

	return false;
}

bool CX2UIQuestNew::Handler_EGS_NEW_QUEST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_NEW_QUEST_NOT kEvent;
	DeSerialize( pBuff, &kEvent );
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	// June 323e4cb0 ( NEW_QUEST_NOT ) has no forbidden-quest give-up: June calls IsForbiddenQuest 3232e510 from the June-only
	// push 0x27b 323e05f0 instead ( Q4 ). March's check stays in the #else.
#else SERV_IRUHADEV_JUNE_QUEST_UI

#ifdef SERV_GATHER_GIVE_UP_QUEST
	if( true == g_pData->GetQuestManager()->IsForbiddenQuest( kEvent.m_QuestInst.m_iID ) )
	{
		Handler_EGS_GIVE_UP_QUEST_REQ( kEvent.m_QuestInst.m_iID );	
		return true;
	}
#endif SERV_GATHER_GIVE_UP_QUEST	
#endif SERV_IRUHADEV_JUNE_QUEST_UI

	g_pData->GetQuestManager()->CreateUnitQuest( kEvent.m_QuestInst.m_iID, kEvent.m_QuestInst );
	m_iSelectedQuestID = kEvent.m_QuestInst.m_iID;

	if ( !m_mapNewQuest.empty() )
	{
		map<int, bool >::iterator mItr = m_mapNewQuest.find( m_iSelectedQuestID );
		if ( m_mapNewQuest.end() != mItr )
			mItr->second = false;
	}	

	// 사운드
	g_pKTDXApp->GetDeviceManager()->PlaySound( L"QuestAccept.ogg", false, false );
		
	UpdateNewQuestNotice();

	const CX2QuestManager::QuestTemplet* pQuestTemplet 
		= g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	
#ifdef SERV_IRUHADEV_JUNE_QUEST_SCENE
	if( NULL != pQuestTemplet )
	{
		// June 323e4cb0 @323e4e0b..323e4e39: in state 5 the start scene waits for the village / battle field that follows ( g_pInstanceData +0xaf8 / +0xafc,
		// played by StateChange ), in any other state it plays now ( 323cb2e0( id, 0 ) ). June's state 5 is XS_SERVER_SELECT ( StateChange 31d88bf0 case 5
		// builds CX2StateServerSelect, ctor 32576ff0; Q5b review ), the same slot as March's.
		if( CX2Main::XS_SERVER_SELECT != g_pMain->GetNowStateID() )
			PlayQuestScene( pQuestTemplet->m_iID, 0 );
		else if( NULL != g_pInstanceData )
			g_pInstanceData->SetDeferredQuestScene( pQuestTemplet->m_iID, 0 );

		// @323e4e43..323e4e54: EVERY epic quest sets the GFX scene's +0x70, whether a scene played or not ( the new-epic effect then fires when the next GFX
		// scene ends: JUNE_BUGS ). March set its Lua scene's flag only inside the start-scene branch.
		if( CX2QuestManager::QT_EPIC == pQuestTemplet->m_eQuestType && NULL != g_pData->GetGFXEventScene() )
			g_pData->GetGFXEventScene()->SetNotifyNewEpicQuest( true );
	}
#else SERV_IRUHADEV_JUNE_QUEST_SCENE
	wstring wstrStartSceneID = ( NULL != pQuestTemplet ? pQuestTemplet->m_wstrStartScene : L"" );
	
	if ( !wstrStartSceneID.empty() )
	{
		g_pData->GetEventScene()->PlayEventScene(wstrStartSceneID);

		if ( CX2QuestManager::QT_EPIC == pQuestTemplet->m_eQuestType )
			g_pData->GetEventScene()->NotifyNewEpicQuest();
	}
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE

	if( NULL != pQuestTemplet && CX2QuestManager::QT_EPIC == pQuestTemplet->m_eQuestType )
	{
		
		SetNowQuestUIMode(QUM_QUEST_DETAIL);
		ResetQuestUI(true, true);
	}
	else
	{
		ReturnToQuestListUI();
		ResetQuestUI(true, true);
	}

	//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
#ifdef SERV_QUEST_CLEAR_EXPAND
	g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecUpdatedInventorySlot );
	if(g_pData->GetUIManager()->GetUIInventory() != NULL)
	{
		g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecUpdatedInventorySlot );
	}			
#endif SERV_QUEST_CLEAR_EXPAND
	//}}

#ifdef QUEST_GUIDE
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
		{
			if( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetNpcIndicator() )
			{
				g_pTFieldGame->GetNpcIndicator()->ResetGuideTargetInfo();
			}
		} break;
	case CX2Main::XS_BATTLE_FIELD:
		{
			if( NULL != g_pX2Game && NULL != static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetNpcIndicator() )
			{
				static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetNpcIndicator()->ResetGuideTargetInfo();
			}
		} break;
	}
#endif //QUEST_GUIDE

	//npc대화 퀘스트라면 다시한번 npc talk msg를 보낸다.
	if( m_iSelectedQuestID != -1 )
	{
		if( pQuestTemplet != NULL )
		{
#ifdef REFORM_QUEST
			if( CX2QuestManager::QT_EVENT == pQuestTemplet->m_eQuestType )
			{
				m_setNewEventQuestIDList.insert( kEvent.m_QuestInst.m_iID );
			}
#endif //REFORM_QUEST
			for(UINT i=0; i<pQuestTemplet->m_vecSubQuest.size(); ++i)
			{
				const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet(pQuestTemplet->m_vecSubQuest[i]);
				if( pSubQuestTemplet!=NULL && pSubQuestTemplet->m_eClearType ==  CX2QuestManager::SQT_NPC_TALK )
				{
					Handler_EGS_TALK_WITH_NPC_REQ( m_eNPCID );
					return true;
				}
			}
		}
	}
	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
	{
		if( CX2PlayGuide::TQI_CHASE_THIEF == kEvent.m_QuestInst.m_iID )
		{
			g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_NOVICE_GUIDE_01, true);
		}
	}

	// 	UpdateQuestListPage( m_iNowQuestListPage );
	// 
	// 	CloseSubQuestList();
	// 	CloseQuestDesc();

	// note!! special case
	// 초심자 숲 가기 퀘스트
	// 	int MAGIC_QUEST_ID = 11; 
	// 	if( MAGIC_QUEST_ID == kEvent.m_QuestInst.m_iID )
	// 	{
	// 		CX2StateMenu* pStateMenu = (CX2StateMenu*) g_pMain->GetNowState();
	// 		if( NULL != pStateMenu )
	// 		{
	// 			pStateMenu->CreateMouseClickUI( (CKTDXStage*) this, D3DXVECTOR2(960, 30), 0.5f, 1000, 
	// 				CX2State::MouseClickUI::MCS_FROM_LEFT_BOTTOM ); // EXIT 버튼 
	// 		}
	// 	}
	// 
	// 	MAGIC_QUEST_ID = 3100; 
	// 	if( MAGIC_QUEST_ID == kEvent.m_QuestInst.m_iID )
	// 	{
	// 		std::wstring packetExplain;
	// 		packetExplain += L"퀘스트를 수락하였습니다.\n내 퀘스트창에서 확인 하실 수 있습니다.";
	// 		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), packetExplain.c_str(), (CKTDXStage*)g_pMain->GetNowState() );
	// 	}
#ifdef REFORM_QUEST
	NotificationByQuickQuestInfoChange( kEvent.m_QuestInst.m_iID);
#endif //REFORM_QUEST
	return true;
}
#ifdef SERV_ENTER_FIELD_QUEST_CLEAR
bool CX2UIQuestNew::Handler_EGS_ALL_COMPLETED_QUEST_COMPLETE_REQ( const KEGS_ALL_COMPLETED_QUEST_COMPLETE_REQ& kPacketNot )
{
	g_pData->GetServerProtocol()->SendPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_REQ, kPacketNot );	
	g_pMain->AddServerPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK );

	return true;
}
bool CX2UIQuestNew::Handler_EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	if( g_pMain->DeleteServerPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{			
			for( unsigned int i=0 ; i < kEvent.m_vecQuestCompleteAck.size(); ++i )
			{
				g_pData->GetQuestManager()->RemoveUnitQuest( kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID, true );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
#ifndef SERV_IRUHADEV_JUNE_QUEST_UI
				g_pData->GetQuestManager()->RefreshVillageNpcRelationDataByQuest( kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID );	// June 323e1710 @323e1864 (Atlas E1c)
	// ( June's ALL_COMPLETED 323e5c10 has no relation refresh: 323e1710 is the batch handler, JUNE_QUEST_UI keeps the call out )
#endif SERV_IRUHADEV_JUNE_QUEST_UI
#endif SERV_IRUHADEV_ELYSION_VILLAGE

				g_pData->GetMyUser()->GetSelectUnit()->Reset( kEvent.m_vecQuestCompleteAck[i].m_kUpdateUnitInfo.m_kUnitInfo );
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecQuestCompleteAck[i].m_kUpdateUnitInfo.m_vecKInventorySlotInfo );
				if( NULL != g_pData->GetUIManager()->GetUIInventory() )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecQuestCompleteAck[i].m_kUpdateUnitInfo.m_vecKInventorySlotInfo );
				}

				m_mapNewQuest[ kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID ] = false;
				//UpdateQuestList();

				// 반복 퀘스트가 아니라면 들고 있는 퀘스트 ID를 -1로 리셋해주자.
				// 반복 퀘스트이면서 목록에 있는 퀘스트라면 선택된 퀘스트 ID를 리셋하지 않는다 : 엔터 연타로 되게.

#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
				const CX2QuestManager::QuestTemplet* pCompletedQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID );
				if( NULL == pCompletedQuestTemplet )
					QuestNoTempletLog( kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID, L"Handler_EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK" );

				if( NULL != pCompletedQuestTemplet && CX2QuestManager::QRT_REPEAT == pCompletedQuestTemplet->m_eRepeatType &&
#else SERV_IRUHADEV_JUNE_QUEST_DATA
				if( CX2QuestManager::QRT_REPEAT == g_pData->GetQuestManager()->GetQuestTemplet(kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID)->m_eRepeatType &&
#endif SERV_IRUHADEV_JUNE_QUEST_DATA
					find( m_vecAvailableQuestID.begin(), m_vecAvailableQuestID.end(), kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID ) != m_vecAvailableQuestID.end() )
				{
					m_iSelectedQuestID = kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID;
				}
				else
				{
					m_iSelectedQuestID = -1;
				}

				ReturnToQuestListUI();
				ResetQuestUI(true, true);

#ifdef SERV_IRUHADEV_JUNE_QUEST_SCENE
				// June 323e5c10 @323e5dc7..323e5de6: the end scene of each completed quest ( 323cb2e0( id, 1 ) )
				PlayQuestScene( kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID, 1 );
#else SERV_IRUHADEV_JUNE_QUEST_SCENE
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
				wstring wstrEndSceneID = ( NULL != pCompletedQuestTemplet ) ? pCompletedQuestTemplet->m_wstrEndScene : wstring();
#else SERV_IRUHADEV_JUNE_QUEST_DATA
				wstring wstrEndSceneID = g_pData->GetQuestManager()->GetQuestTemplet(kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID)->m_wstrEndScene;
#endif SERV_IRUHADEV_JUNE_QUEST_DATA

				if(wstrEndSceneID != L"")
				{
					g_pData->GetEventScene()->PlayEventScene(wstrEndSceneID);
				}
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE

				// 미니맵 아이콘 업뎃해주고
				UpdateNewQuestNotice();

				// 전직 퀘스트라면 : 스킬트리 업데이트
				const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kEvent.m_vecQuestCompleteAck[i].m_kCompleteQuestInfo.m_iQuestID );
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
				if( NULL != pQuestTemplet && CX2QuestManager::QT_CHANGE_JOB == pQuestTemplet->m_eQuestType && CX2Unit::UC_NONE != pQuestTemplet->m_Reward.m_eChangeUnitClass )
#else SERV_IRUHADEV_JUNE_QUEST_DATA
				if( CX2QuestManager::QT_CHANGE_JOB == pQuestTemplet->m_eQuestType && CX2Unit::UC_NONE != pQuestTemplet->m_Reward.m_eChangeUnitClass )
#endif SERV_IRUHADEV_JUNE_QUEST_DATA
				{
					// 스킬트리 업데이트. 새로 초기화 시켜 준다.
					g_pData->GetUIManager()->GetUISkillTree()->InitSkillTreeUI();

					g_pData->GetClassChangePopup().SetShow( true );

					CX2State* pState = (CX2State*) g_pMain->GetNowState();
					pState->ResetUnitViewerInFieldSquare( g_pData->GetMyUser()->GetSelectUnit()->GetUID(), g_pData->GetMyUser()->GetSelectUnit()->GetClass() );
					//{{ Iruha : 2026-09-04 // HUD portrait after a quest job change
#ifdef SERV_IRUHADEV_JOBCHANGE_PORTRAIT
					// ResetUnitViewerInFieldSquare above rebuilds the 3D square unit; the
					// top-left gage portrait is a separate thing and nothing here touched
					// it, so it kept drawing the old class until the gage was rebuilt -
					// which only happens on a state change, i.e. re-selecting the
					// character. This is a gap in the shipped client, not in the server:
					// the GameServer sends only EGS_QUEST_COMPLETE_ACK for a quest-driven
					// class change (UserQuestManager.cpp:5038-5142), and every OTHER
					// class-change path in the client refreshes the portrait itself -
					// EGS_CHANGE_MY_UNIT_INFO_NOT (X2State.cpp:12958), the admin change
					// (:5083), and the jumping character (:13174), whose comment there is
					// literally "refresh the character portrait".
					//
					// GetMyGageData() is the probe, not GetInstance() on its own:
					// CX2GageManager::SetCharacterImage dereferences m_ptrMyGageSet with
					// no null check, unlike almost every neighbour in that same header,
					// and there is no gage at all in some states.
					if( NULL != CX2GageManager::GetInstance() &&
						NULL != CX2GageManager::GetInstance()->GetMyGageData() )
					{
						const CX2Unit::UNIT_CLASS eNewClass =
							g_pData->GetMyUser()->GetSelectUnit()->GetClass();
					
						CX2GageManager::GetInstance()->SetCharacterImage( eNewClass );
#ifdef FIX_CHUNG_GAGE_UI_UPDATE_BUG
						CX2GageManager::GetInstance()->ResetGageUIEtc( eNewClass );
#endif // FIX_CHUNG_GAGE_UI_UPDATE_BUG
					}
#endif SERV_IRUHADEV_JOBCHANGE_PORTRAIT
//}}
				}
			}
				return true;
		}
	}

	return false;
}
#endif SERV_ENTER_FIELD_QUEST_CLEAR

void CX2UIQuestNew::SetShow(bool val)
{
	m_bShow = val;
	if(val)	// 켜질 때 처리해야 할 부분
	{
		// 일단 퀘 리스트를 받아 오고
		//UpdateQuestList( m_eNPCID );


		m_iTopQuestSlotIndex = 0;
		// UI의 퀘 리스트를 세팅해 주고
		if(IsTalkingQuestNPC())
			SetNowQuestUIMode(QUM_QUEST);
		else
			SetNowQuestUIMode(QUM_EPIC_QUEST);
		
		ResetQuestUI(true, false, false);
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
		HideMovieTab();		// June 323e5340 @323e53e6: the tab hide after the reset; an open Video tab skipped that reset ( JUNE_BUGS )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB

		g_pData->GetUIManager()->UIOpened(CX2UIManager::UI_MENU_QUEST_NEW);
	}

	else	// 꺼질 때 처리해야 할 부분
	{
		// 아이템 마우스 오버 이미지를 지우자
		m_pDLGSelectedItem->SetShow( false ); 
		InvalidSlotDesc();

		InitRewardItemSlot();

		ShowUIDesc(false);

		m_pDLGUIQuestList->SetShowEnable(false, false);
		m_pDLGUIQuestDetail->SetShowEnable(false, false);

		g_pData->GetUIManager()->UIClosed(CX2UIManager::UI_MENU_QUEST_NEW);

		ResetTabPriorityFactor();

#ifdef REFORM_NOVICE_GUIDE
		if( NULL != g_pData && NULL != g_pData->GetPlayGuide()
			&& true == g_pData->GetPlayGuide()->GetActiveNoviceGuide() )
		{
			switch( g_pData->GetPlayGuide()->GetGuideStep() )
			{
			case CX2PlayGuide::NGS_STEP_1:
			case CX2PlayGuide::NGS_STEP_2:
			case CX2PlayGuide::NGS_STEP_3:
				{
					g_pData->GetPlayGuide()->SetMouseClickIndex( CX2PlayGuide::NGS_STEP_0 );
				}
				break;
			}
		}
#else
		if( g_pTFieldGame != NULL && 
			g_pTFieldGame->GetNoviceGuide() != NULL && 
			g_pTFieldGame->GetNoviceGuide()->GetIsPlayGuide() == true )
			g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(0);
#endif //REFORM_NOVICE_GUIDE

#ifdef MODIFY_ACCEPT_QUEST // 퀘스트 메뉴 New표기 갱신
		switch(g_pMain->GetNowStateID())
		{
		case CX2Main::XS_VILLAGE_MAP:
		case CX2Main::XS_BATTLE_FIELD:
			{
				if( true == g_pMain->IsInheritStateMenu() )
				{
					CX2StateMenu* pStateMenu = static_cast<CX2StateMenu*>(g_pMain->GetNowState());
					if( NULL != pStateMenu &&
						NULL != g_pData && 
						NULL != g_pData->GetQuestManager() )
					{
						vector<int> vecAvailQuestID;
						g_pData->GetQuestManager()->GetAvailableQuest( CX2UnitManager::NUI_BILLBOARD, vecAvailQuestID );
						pStateMenu->SetShowNewQuest( !vecAvailQuestID.empty() );
					}
				}
			} break;
		default:
			break;
		}
#endif // MODIFY_ACCEPT_QUEST

	}

}


void CX2UIQuestNew::SelectQuest( int SelectedQuestID )
{
	m_iPickedQuestID = SelectedQuestID;
	//UpdateQuestUI();

#ifndef REFORM_NOVICE_GUIDE
	// 초보자 가이드	
	bool bHasGuideQuest = false;	
	if( g_pTFieldGame != NULL && g_pTFieldGame->GetNoviceGuide() != NULL && g_pTFieldGame->GetNoviceGuide()->GetIsPlayGuide() == true )
	{
		for(UINT i=0; i<m_vecAvailableQuestID.size(); ++i)
		{
			if(m_vecAvailableQuestID[i] == 11000 || m_vecAvailableQuestID[i] == 11010)
			{
				bHasGuideQuest = true;				
			}
		}

		if( g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP3 &&
			g_pTFieldGame->GetJoinNpcId() == CX2UnitManager::NUI_HAGERS )
		{
			if(bHasGuideQuest == true && (SelectedQuestID == 11000 || SelectedQuestID == 11010))
			{
				g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(4);
			}
			else if(bHasGuideQuest == true && (SelectedQuestID != 11000 && SelectedQuestID != 11010))
			{
				g_pTFieldGame->GetNoviceGuide()->SetMouseClickIndex(3);
			}		
		}		
	}
#endif //REFORM_NOVICE_GUIDE

	//UpdateNPCQuestDesc();

}




std::wstring CX2UIQuestNew::GetSlotItemDescExtra_RBtn( int itemTID, CX2Item* pItem )
{
	std::wstring slotItemDesc = L"";
	return slotItemDesc;
}

wstring CX2UIQuestNew::GetSlotItemDesc()
{
	wstring itemDesc = L"";

	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->GetDialog() == NULL )
		{
			itemDesc = m_pNowOverItemSlot->GetSlotDesc();
		}
		else
		{	
			CX2Item* pkItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( 
				m_pNowOverItemSlot->GetItemUID() );
			if ( pkItem != NULL )
				itemDesc = GetSlotItemDescByUID( m_pNowOverItemSlot->GetItemUID() );
			else
			{
				CX2Item* pItem = NULL;
				map< int, int >::iterator mit;
				mit = m_mapItemIDAndSocketID.find( m_pNowOverItemSlot->GetItemTID() );
				if ( mit != m_mapItemIDAndSocketID.end() )
				{
					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( m_pNowOverItemSlot->GetItemTID() );
					if ( pItemTemplet != NULL )
					{
					    CX2Item::ItemData kItemData;
						kItemData.m_PeriodType = pItemTemplet->GetPeriodType();
						kItemData.m_Endurance = pItemTemplet->GetEndurance();
						int socketID = mit->second; 
						kItemData.m_SocketOption.push_back( socketID );
						kItemData.m_ItemID = m_pNowOverItemSlot->GetItemTID();
#ifdef QUEST_REWARD_PERIOD
						if(m_pNowOverItemSlot->GetPeriod() != 0)
						{
							kItemData.m_Period = m_pNowOverItemSlot->GetPeriod();
						}
#endif QUEST_REWARD_PERIOD
						CX2Item* pItem = new CX2Item( kItemData, NULL );
						itemDesc = GetSlotItemDescByTID( pItem, m_pNowOverItemSlot->GetItemTID() );
						SAFE_DELETE( pItem );
					}
				}
#ifdef QUEST_REWARD_PERIOD	
				else if(m_pNowOverItemSlot->GetPeriod() != 0)
				{
					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( m_pNowOverItemSlot->GetItemTID() );
					if ( pItemTemplet != NULL )
					{
					    CX2Item::ItemData kItemData;
						kItemData.m_PeriodType = pItemTemplet->GetPeriodType();
						kItemData.m_Period = m_pNowOverItemSlot->GetPeriod();
						kItemData.m_Endurance = pItemTemplet->GetEndurance();
						kItemData.m_ItemID = m_pNowOverItemSlot->GetItemTID();
						CX2Item* pItem = new CX2Item( kItemData, NULL );
						itemDesc = GetSlotItemDescByTID( pItem, m_pNowOverItemSlot->GetItemTID() );
						SAFE_DELETE( pItem );
					}
				}
#endif QUEST_REWARD_PERIOD
				else
				{
					itemDesc = GetSlotItemDescByTID( pkItem, m_pNowOverItemSlot->GetItemTID() );
				}

			}
		}

	}

	return itemDesc;	
}

void CX2UIQuestNew::OpenQuestRewardWindow( int iQuestID, bool bEnableSelection )
{
	if(iQuestID == -1) return;

	g_pKTDXApp->GetDeviceManager()->PlaySound( L"QuestReward.ogg", false, false );		

	m_bLastRewardWasSelection = bEnableSelection;

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( iQuestID );
	if( NULL == pQuestTemplet )
		return;

	m_bShowClearDLG = true;

	if( m_pDLGUIQuestClear == NULL )
	{
		m_pDLGUIQuestClear = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_NEW_Quest_Clear.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUIQuestClear );
	}

	// 돈 경험치 SP
	WCHAR wszText[64] = {0,};
	CKTDGUIStatic* pStaticED = (CKTDGUIStatic*) m_pDLGUIQuestClear->GetControl( L"Static_ED" );
	CKTDGUIStatic* pStaticExp = (CKTDGUIStatic*) m_pDLGUIQuestClear->GetControl( L"Static_Exp" );
	CKTDGUIStatic* pStaticSP = (CKTDGUIStatic*) m_pDLGUIQuestClear->GetControl( L"Static_SP" );

	//wsprintf( wszText, L"%d", pQuestTemplet->m_Reward.m_iED );
	StringCchPrintf( wszText, 64, L"%d", pQuestTemplet->m_Reward.m_iED );
	pStaticED->GetString(0)->msg = wszText;
	//wsprintf( wszText, L"%d", pQuestTemplet->m_Reward.m_iEXP );
	StringCchPrintf( wszText, 64, L"%d", pQuestTemplet->m_Reward.m_iEXP );
	pStaticExp->GetString(0)->msg = wszText;
	//wsprintf( wszText, L"%d", pQuestTemplet->m_Reward.m_iSP );
	StringCchPrintf( wszText, 64, L"%d", pQuestTemplet->m_Reward.m_iSP );
	pStaticSP->GetString(0)->msg = wszText;		

	// 일반보상 세팅
	CKTDGUIStatic* pStaticReward = (CKTDGUIStatic*) m_pDLGUIQuestClear->GetControl( L"Static_RewardSlot" );

	for(int i=0; i<_CONST_UIQUESTNEW_INFO_::g_iRewardSlotNum; i++)
	{
		if( i < (int)pQuestTemplet->m_Reward.m_vecItem.size() )
		{
			// 보상슬롯
			const CX2QuestManager::ITEM_DATA& itemData = pQuestTemplet->m_Reward.m_vecItem[i];
			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemData.m_iItemID );
			if(pItemTemplet != NULL)
			{
				pStaticReward->GetPicture(i)->SetTex( 
                    pItemTemplet->GetShopImage()
                    );

				// 보상 버튼에 DummyInt로 ItemID를 달아 주자!
				// 더미인트 0 : ItemID, 1 : SlotID
				WCHAR buff[256] = {0,};
				//wsprintf( buff, L"Reward_Slot_%d", i+1);
				StringCchPrintf( buff, 256, L"Reward_Slot_%d", i+1);
				CKTDGUIButton* pButton = (CKTDGUIButton*) m_pDLGUIQuestClear->GetControl( buff );				
				pButton->AddDummyInt( itemData.m_iItemID );
				pButton->AddDummyInt( itemData.m_iSocketOption1 );
#ifdef QUEST_REWARD_PERIOD
				pButton->AddDummyInt( itemData.m_iPeriod );
#endif QUEST_REWARD_PERIOD
			}

		}
		else
		{
			WCHAR buff[256] = {0};
			//wsprintf( buff, L"Reward_Slot_%d", i+1);
			StringCchPrintf( buff, 256, L"Reward_Slot_%d", i+1);
			CKTDGUIButton* pButton = (CKTDGUIButton*) m_pDLGUIQuestClear->GetControl( buff );
			pButton->SetShowEnable(false, false);
			pButton->AddDummyInt( 0 );
			pButton->AddDummyInt( 0 );
#ifdef QUEST_REWARD_PERIOD
			pButton->AddDummyInt( 0 );
#endif QUEST_REWARD_PERIOD

		}
	}



	// 선택보상 세팅
	CKTDGUIStatic* pStaticSelectionReward = (CKTDGUIStatic*) m_pDLGUIQuestClear->GetControl( L"Static_SelectionRewardSlot" );

	// 	CKTDGUIRadioButton* pRadioButton1 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_1" );
	// 	CKTDGUIRadioButton* pRadioButton2 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_2" );
	// 	CKTDGUIRadioButton* pRadioButton3 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_3" );



	int SlotIndex = 0;
	m_byNumOfItemThatICanGet = 0;

	for(int i=0; SlotIndex<_CONST_UIQUESTNEW_INFO_::g_iRewardSlotNum; i++)
	{
		if( i < (int)pQuestTemplet->m_SelectReward.m_vecSelectItem.size())
		{
			// 선택보상슬롯
			const CX2QuestManager::ITEM_DATA& itemData = pQuestTemplet->m_SelectReward.m_vecSelectItem[i];
			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemData.m_iItemID );
			//{{10.01.14 김정협 아이템 없을 시 문제 처리
			if(pItemTemplet == NULL)
				continue;
			//}}10.01.14 김정협 아이템 없을 시 문제 처리

			// fix!! 일단 선택보상에서는 unit_type만 체크
			if( pItemTemplet->GetUnitType() != CX2Unit::UT_NONE )
			{
				CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
				if( pItemTemplet->GetUnitType() != eUnitType )
				{
					continue;
				}
			}
#ifdef QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
			if( pItemTemplet->GetUnitClass() != CX2Unit::UC_NONE )
			{
				CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
				if( false == GetX2UnitClassCompatibility(eUnitClass, pItemTemplet->GetUnitClass()) )
				{
					continue;
				}
			}
#endif QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
			if(pItemTemplet != NULL)
			{
				// 보상 버튼에 DummyInt로 ItemID를 달아 주자!
				// 더미인트 0 : ItemID, 1 : SlotID
				WCHAR buff[256] = {0,};
				//wsprintf( buff, L"Selection_Reward_Slot_%d", SlotIndex+1);
				StringCchPrintf( buff, 256, L"Selection_Reward_Slot_%d", SlotIndex+1);
				CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( buff );
				pRadioButton->AddDummyInt( itemData.m_iItemID );
				pRadioButton->AddDummyInt( itemData.m_iSocketOption1 );
#ifdef QUEST_REWARD_PERIOD
				pRadioButton->AddDummyInt( itemData.m_iPeriod );
#endif QUEST_REWARD_PERIOD
				pStaticSelectionReward->GetPicture(SlotIndex)->SetTex( 
                    pItemTemplet->GetShopImage() 
                    );

				++m_byNumOfItemThatICanGet;

			}
		}
		else
		{
			WCHAR buff[256] = {0,};
			//wsprintf( buff, L"Selection_Reward_Slot_%d", SlotIndex+1);
			StringCchPrintf( buff, 256, L"Selection_Reward_Slot_%d", SlotIndex+1);
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( buff );
			pRadioButton->SetShowEnable( false, false );
			pRadioButton->AddDummyInt( 0 );
			pRadioButton->AddDummyInt( 0 );
#ifdef QUEST_REWARD_PERIOD
			pRadioButton->AddDummyInt( 0 );
#endif QUEST_REWARD_PERIOD
		}
		SlotIndex++;
	}

	if ( m_byNumOfItemThatICanGet == 1 )	// 보상 받을 수 있는 물품이 한가지인 경우에는 무조건 선택 되도록!!
	{
		// note!! 임시로 첫번째 아이템이 무조건 선택되어 있게
		CKTDGUIRadioButton* pRadioButton1 = (CKTDGUIRadioButton*) m_pDLGUIQuestClear->GetControl( L"Selection_Reward_Slot_1" );
		if(true == pRadioButton1->GetShow())
		{
			pRadioButton1->SetChecked( true );
		}
	}
}

void CX2UIQuestNew::CloseQuestRewardWindow()
{

	m_bShowClearDLG = false;

	if( m_pDLGUIQuestClear != NULL )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGUIQuestClear, NULL, false );
		m_pDLGUIQuestClear = NULL;
	}

	if( m_pDLGMsgBox != NULL )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
		m_pDLGMsgBox = NULL;
	}

	if( m_pDLGQuestRewardItemInfo != NULL )
	{
		OpenRewardItemInfoPopup( false, D3DXVECTOR2(0,0), -1 );
	}
#ifndef REFORM_NOVICE_GUIDE
	// 초보자 가이드	
	if( g_pTFieldGame != NULL && g_pTFieldGame->GetNoviceGuide() != NULL && 
		g_pTFieldGame->GetNoviceGuide()->GetIsPlayGuide() == true &&
		g_pTFieldGame->GetNoviceGuide()->GetNowGuideStep() == CX2NoviceGuide::NGS_STEP7 && 
		m_iSelectedQuestID == 11010 )
	{				
		g_pTFieldGame->GetNoviceGuide()->SetNotifyGuide(CX2NoviceGuide::GE_COMPLETE);
	}
#endif //REFORM_NOVICE_GUIDE

}

#ifdef QUEST_REWARD_PERIOD
void CX2UIQuestNew::OpenRewardItemInfoPopup( bool bOpen, D3DXVECTOR2 vPos, int iItemID, int socketID , int Period )
#else
void CX2UIQuestNew::OpenRewardItemInfoPopup( bool bOpen, D3DXVECTOR2 vPos, int iItemID, int socketID )
#endif QUEST_REWARD_PERIOD
{
	//{{ 허상형 : [2009/9/29] //	퀘스트 종료 후에도 툴팁이 남아있는 버그 수정을 위한 조건식 추가
	if( m_pDLGUIQuestClear == NULL && true == bOpen )
	{
		return;
	}

	if( m_pDLGQuestRewardItemInfo == NULL && true == bOpen )
	{
		m_pDLGQuestRewardItemInfo = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_SlotManager.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGQuestRewardItemInfo );

		CKTDGUIStatic* pStatic = new CKTDGUIStatic();
		pStatic->SetName( L"ItemDesc" );
		m_pDLGQuestRewardItemInfo->AddControl( pStatic );
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		pStatic->AddPicture( pPicture );
		pPicture->SetEdge( false, 2, D3DXCOLOR(1,1,1,1) );
		pPicture->SetTex( L"FadeInOut.dds" );


		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		pStatic->AddString( pString );
		pString->fontIndex = XUF_DODUM_13_SEMIBOLD;
		pString->color = D3DXCOLOR( 1,1,1,1 );
		pString->outlineColor = D3DXCOLOR( 1,1,1,1 );
		pString->msg = L"";
		pString->fontStyle = CKTDGFontManager::FS_NONE;
		pString->sortFlag = DT_LEFT;
		pString->pos = D3DXVECTOR2( 20, 20 );
	}


	if( m_pDLGQuestRewardItemInfo != NULL )
	{
		m_pDLGQuestRewardItemInfo->SetShowEnable( bOpen, bOpen );

		if( true == bOpen )
		{
			int iLayer = XDL_POP_UP;
			if( m_pDLGUIQuestClear != NULL )
			{
				iLayer = m_pDLGUIQuestClear->GetLayer() + 1;
			}
			g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDLGQuestRewardItemInfo, iLayer );
		}
	}
	else
	{
		return;
	}

	if( false == bOpen )
		return;


	CX2Item* pItem = NULL;

	if ( socketID != 0 )
	{
		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( iItemID );
		if ( pItemTemplet != NULL )
		{
			CX2Item::ItemData kItemData;
			kItemData.m_Endurance = pItemTemplet->GetEndurance();
			kItemData.m_ItemID = iItemID;
#ifdef QUEST_REWARD_PERIOD
			if( Period != 0 )
			{
				kItemData.m_Period = Period;
			}
#endif QUEST_REWARD_PERIOD
			kItemData.m_SocketOption.push_back( socketID );
			pItem = new CX2Item( kItemData, NULL );
		}
	}
#ifdef QUEST_REWARD_PERIOD
	else if( Period != 0 )
	{
		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( iItemID );
		if ( pItemTemplet != NULL )
		{
			CX2Item::ItemData kItemData;
			kItemData.m_Endurance = pItemTemplet->GetEndurance();
			kItemData.m_ItemID = iItemID;
			kItemData.m_Period = Period;
			pItem = new CX2Item( kItemData, NULL );
		}
	}
#endif QUEST_REWARD_PERIOD



	wstring wstrItemDesc = GetSlotItemDescByTID( pItem, iItemID, false );
	SAFE_DELETE( pItem );

	CKTDGUIStatic* pStatic_ItemDesc = (CKTDGUIStatic*) m_pDLGQuestRewardItemInfo->GetControl(L"ItemDesc");
	if( NULL == pStatic_ItemDesc )
		return; 

	CKTDGUIControl::UIStringData* pString_ItemDesc = pStatic_ItemDesc->GetString(0);
	if( NULL == pString_ItemDesc )
		return;

#ifdef FIX_TOOLTIP	// 슬롯 형태가 아닌 버튼 형태로 되어 있어서 툴팁 자르기를 따로 수행(X2ItemSlotMananger::DrawSlotDesc)
	bool bItemDescAdd = false;			// 선택된 아이템의 설명 나누기

	const int iWidth = DXUTGetBackBufferSurfaceDesc()->Width;
	const int iHeight = DXUTGetBackBufferSurfaceDesc()->Height;
	const int SIZE_Y = 40;

	CKTDGFontManager::CUKFont* pItemDescFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( SLOT_MANAGER_FONT_INDEX );

	int itemDescWidth = static_cast<int>( iWidth / 3 / g_pKTDXApp->GetResolutionScaleX() );
	int itemDescHeight = 0;
	int itemDescFontHeight = g_pKTDXApp->GetResolutionScaleY() > 1.1 ? 15 : 16;	// XUF_DODUM_13_SEMIBOLD의 크기

	wstring wstrItemDescTemp = wstrItemDesc;

#ifdef CLIENT_GLOBAL_LINEBREAK
	wstrItemDesc = CWordLineHandler::GetStrByLineBreakColorInX2Main(wstrItemDesc.c_str(), itemDescWidth - ITEM_DESC_ADD_SIZE_X, SLOT_MANAGER_FONT_INDEX);
#else //CLIENT_GLOBAL_LINEBREAK
	wstrItemDesc = g_pMain->GetStrByLineBreakColor(wstrItemDesc.c_str(), itemDescWidth - ITEM_DESC_ADD_SIZE_X, SLOT_MANAGER_FONT_INDEX);
#endif //CLIENT_GLOBAL_LINEBREAK

	SplitDescHeight( &wstrItemDesc, &wstrItemDescTemp, itemDescHeight, itemDescFontHeight, SLOT_MANAGER_FONT_INDEX, false );

	CKTDGUIStatic* pItemDesc = (CKTDGUIStatic*)m_pDLGItemDesc->GetControl(L"ItemDesc");
	if( NULL != pItemDesc && 
		NULL != pItemDesc->GetString(0) )
	{
		const int MAGIC_ITEM_TOOL_TIP_MAX_HEIGHT = static_cast<int>( iHeight / g_pKTDXApp->GetResolutionScaleY() );

		if( itemDescHeight > MAGIC_ITEM_TOOL_TIP_MAX_HEIGHT )
		{
			pItemDesc->GetString(0)->fontIndex = SLOT_MANAGER_FONT_INDEX_SMALL;
			pItemDescFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( SLOT_MANAGER_FONT_INDEX_SMALL );
			itemDescFontHeight = g_pKTDXApp->GetResolutionScaleY() > 1.1 ? 13 : 14;	// XUF_DODUM_11_NORMAL의 크기
#ifdef CLIENT_GLOBAL_LINEBREAK
			wstrItemDesc = CWordLineHandler::GetStrByLineBreakColorInX2Main(wstrItemDescTemp.c_str(), itemDescWidth - ITEM_DESC_ADD_SIZE_X, SLOT_MANAGER_FONT_INDEX_SMALL);
#else //CLIENT_GLOBAL_LINEBREAK
			wstrItemDesc = g_pMain->GetStrByLineBreakColor(wstrItemDescTemp.c_str(), itemDescWidth - ITEM_DESC_ADD_SIZE_X, SLOT_MANAGER_FONT_INDEX_SMALL);
#endif //CLIENT_GLOBAL_LINEBREAK
			SplitDescHeight( &wstrItemDesc, &wstrItemDescTemp, itemDescHeight, itemDescFontHeight, SLOT_MANAGER_FONT_INDEX, false );
		}
		else
		{
			pItemDesc->GetString(0)->fontIndex = SLOT_MANAGER_FONT_INDEX;
		}
	}

	int offsetSize = 0;
	int lineNum = 0;
	offsetSize = wstrItemDesc.find( L"\n", offsetSize );
	while( offsetSize != -1 )
	{
		offsetSize++;
		lineNum++;
		offsetSize = wstrItemDesc.find( L"\n", offsetSize );
	}

	itemDescHeight += lineNum;

	//{{ 폰트를 작게 설정했는데도 화면 높이보다 클 경우는 분할
	int itemDescAddHeight = 0;
	if ( itemDescHeight > iHeight / g_pKTDXApp->GetResolutionScaleY() )
	{
		itemDescWidth = static_cast<int>( iWidth / 4 / g_pKTDXApp->GetResolutionScaleX() );

#ifdef CLIENT_GLOBAL_LINEBREAK
		wstrItemDesc = CWordLineHandler::GetStrByLineBreakColorInX2Main(wstrItemDescTemp.c_str(), itemDescWidth - ITEM_DESC_ADD_SIZE_X, SLOT_MANAGER_FONT_INDEX_SMALL);
#else //CLIENT_GLOBAL_LINEBREAK
		wstrItemDesc = g_pMain->GetStrByLineBreakColor(wstrItemDescTemp.c_str(), itemDescWidth - ITEM_DESC_ADD_SIZE_X, SLOT_MANAGER_FONT_INDEX_SMALL);
#endif //CLIENT_GLOBAL_LINEBREAK

		SplitDescHeight( &wstrItemDesc, &wstrItemDescTemp, itemDescHeight, itemDescFontHeight, SLOT_MANAGER_FONT_INDEX, false );

		itemDescAddHeight = static_cast<int>( itemDescHeight - iHeight / g_pKTDXApp->GetResolutionScaleY() + itemDescFontHeight * 2 );
		itemDescHeight = static_cast<int>( iHeight / g_pKTDXApp->GetResolutionScaleY() );
		bItemDescAdd = true;

		SplitDescHeight( &wstrItemDesc, &wstrItemDescTemp, itemDescHeight, itemDescFontHeight, SLOT_MANAGER_FONT_INDEX, true );
	}

	int itemDescPosX = 0;
	int itemDescPosY = 0;


#endif

	m_pDLGQuestRewardItemInfo->GetStatic_LUA( "ItemDesc" )->GetPicture(0)->SetSize( D3DXVECTOR2( (float)itemDescWidth, (float)itemDescHeight) );
	m_pDLGQuestRewardItemInfo->GetStatic_LUA( "ItemDesc" )->GetPicture(0)->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
	m_pDLGQuestRewardItemInfo->GetStatic_LUA( "ItemDesc" )->GetString(0)->msg = wstrItemDesc.c_str();
#ifdef FIX_TOOLTIP
	D3DXVECTOR2 vDialogPos = D3DXVECTOR2( (float)itemDescPosX, (float)itemDescPosY );
#else
	D3DXVECTOR2 vDialogPos = vPos;
	vDialogPos.y -= (float) itemDescHeight;
#endif
	m_pDLGQuestRewardItemInfo->SetPos( vDialogPos );

}

void CX2UIQuestNew::UpdateNavi()
{
	CKTDGUINavigation* pNavi = (CKTDGUINavigation*) m_pDLGUIQuestList->GetControl( L"Navi_QuestList" );
	if( NULL != pNavi )
	{
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
		// June 323c8da0 @323c8dd0 / @323c8e09: on the Video tab the navi shows from 13 rows, at ( start / 3 ) * 0.68 / ( navi steps - 1 )
		if( true == m_bMovieTab )
		{
			if( m_iMovieCount < 13 )
			{
				pNavi->SetShow( false );
				return;
			}
			pNavi->SetShow( true );
			D3DXVECTOR2 vMoviePos( 0.0f, (float)( m_iMovieStartID / 3 ) * 0.68f / (float)( m_iMovieNaviStep - 1 ) );
			pNavi->SetRelativeWindowPos( vMoviePos );
			return;
		}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
		if( m_iSizeQuestInfoList <= _CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage )
		{
			pNavi->SetShow(false);
			return;
		}

		pNavi->SetShow(true);

		// 위치 정해주고~
		D3DXVECTOR2 vPos;
		vPos.x = 0.0f;
		// 0.68 : Navi Size에 따른 값. MAGIC VALUE
		vPos.y = (float)m_iTopQuestSlotIndex / (m_iSizeQuestInfoList - _CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage ) * 0.68f;

		pNavi->SetRelativeWindowPos( vPos );
	}
}


void CX2UIQuestNew::ShowNextButton(bool val)
{
	CKTDGUIButton* pButtonNext = (CKTDGUIButton*) m_pDLGUIQuestReceive->GetControl( L"Button_Next" );
	CKTDGUIButton* pButtonGiveup = (CKTDGUIButton*) m_pDLGUIQuestReceive->GetControl( L"Button_Giveup" );
	CKTDGUIButton* pButtonAccept = (CKTDGUIButton*) m_pDLGUIQuestReceive->GetControl( L"Button_Accept" );
	CKTDGUIButton* pButtonComplete = (CKTDGUIButton*) m_pDLGUIQuestReceive->GetControl( L"Button_Complete" );

	if(val)		// NEXT 버튼 보여라
	{
		if(pButtonNext != NULL)
		{
			pButtonNext->SetShow(true);
			pButtonNext->RequestFocus();
		}

		if(pButtonGiveup != NULL)
			pButtonGiveup->SetShow(false);
		if(pButtonAccept != NULL)
			pButtonAccept->SetShow(false);
		if(pButtonComplete != NULL)
			pButtonComplete->SetShow(false);		
	}
	else
	{
		pButtonNext->SetShow(false);

		const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iPickedQuestID );
		if( NULL != pQuestInst )
		{
			if( true == pQuestInst->IsComplete() )
			{
				// 완료된 퀘스트
				pButtonComplete->SetShow(true);
				pButtonComplete->RequestFocus();
			}
			else
			{
				// 진행중인 퀘스트
				pButtonGiveup->SetShow(true);
				pButtonGiveup->RequestFocus();
			}
		}
		else
		{
			// 안 받은 퀘스트
			pButtonAccept->SetShow(true);
			pButtonAccept->RequestFocus();
		}
	}
}



void CX2UIQuestNew::UpdateNewQuestNotice()
{
	// 일단 미니맵이 있는지부터 검사좀하고
	if( g_pInstanceData == NULL ||
		g_pInstanceData->GetMiniMapUI() == NULL )
	{
		return;
	}

	// 준비작업
	const map<int, int>& mapQuestClear = g_pData->GetQuestManager()->GetMapCompleteQuest(); // 이미 완료, 보상받은 퀘스트들  
	int iQuestNum = 0;
	int TopQuestID = 0;
	SEnum::VILLAGE_MAP_ID VID = SEnum::VMI_RUBEN;

	// 자기가 만날 수 있는 NPC의 리스트를 만들어두자.
	// 09.03.17 변경 : 특정 지역에 있을 때 해당하는 NPC로 기획변경.
	// 일단 여기서 NPC 필터링하면 정상작동할테니 그렇게 만들어두고 나서 나중에 코드를 정리하는 쪽으로 가자..
	std::map<CX2UnitManager::NPC_UNIT_ID, SEnum::VILLAGE_MAP_ID> mapNPCID;
	mapNPCID.clear();
	
	for(int LID = (int)CX2LocationManager::LMI_RUBEN; 
		g_pData->GetLocationManager()->GetLocalMapTemplet((CX2LocationManager::LOCAL_MAP_ID)LID) != NULL; 
		++LID)
	{
		// 내가 갈 수 있는 지역인지 테스트
		CX2LocationManager::LocalMapTemplet* pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet((CX2LocationManager::LOCAL_MAP_ID)LID);
		if( pLocalMapTemplet->m_vecRequireClearDungeonID.size() > 0 )
		{
			if( false == g_pData->GetMyUser()->GetSelectUnit()->IsClearDungeon( pLocalMapTemplet->m_vecRequireClearDungeonID ) ||
				g_pData->GetSelectUnitLevel() < pLocalMapTemplet->m_RequireUnitLevel )
			{
				continue;
			}
#ifdef SERV_IRUHADEV_JUNE_QUEST_GATE
			// June UpdateNewQuestNotice 323d4390 @323d44ca: the map joins the candidate NPC map only when ( +0x70 < 1 or its quest is complete ) too
			if( false == pLocalMapTemplet->IsQuestGateOpen( L"quest notice" ) )
			{
				continue;
			}
#endif SERV_IRUHADEV_JUNE_QUEST_GATE
		}

#if 0
		// 내가 지금 있는 지역과 목표로 하는 마을이 "가까운지" 테스트
		// 내가 속한 지역의 퀘스트만 받는다.
		if( false == IsNearVillage(g_pData->GetLocationManager()->GetCurrentVillageID(), pLocalMapTemplet->m_VillageID) )
		{
			continue;
		}		
#endif
		
        // 해서 해당하는 NPC들의 정보를 저장하자.
		CX2LocationManager::VillageTemplet* pVillageTemplet = g_pData->GetLocationManager()->GetVillageMapTemplet( pLocalMapTemplet->m_VillageID );
		if(pVillageTemplet != NULL)
		{
			for(std::vector<CX2LocationManager::HOUSE_ID>::iterator it = pVillageTemplet->m_HouseList.begin(); it < pVillageTemplet->m_HouseList.end(); it++)
			{
				CX2LocationManager::HouseTemplet* pHouseTemplet = g_pData->GetLocationManager()->GetHouseTemplet(*it);
				if(pHouseTemplet != NULL)
				{
					for(std::vector<CX2UnitManager::NPC_UNIT_ID>::iterator itNPC = pHouseTemplet->m_NPCList.begin(); itNPC < pHouseTemplet->m_NPCList.end(); itNPC++)
					{
						mapNPCID.insert(std::make_pair(*itNPC, pLocalMapTemplet->m_VillageID) );
					}
				}			
			}
		}
	}
	
	


	//전직퀘스트 중에서 한쪽은 안보이게 하자
	const CX2QuestManager::ChangeJobQuest& changeJobQuest = 
		g_pData->GetQuestManager()->GetChangeJobQuest( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );


	// 가지고 있거나 완료한 전직 퀘스트중에서 해당하는 것을 찾는다.
	CX2QuestManager::CHANGE_JOB_TYPE ePickedChangeJobType = CX2QuestManager::CJT_JOB_NONE;
	for( UINT i = CX2QuestManager::CJT_JOB_FIRST_OLD; i < CX2QuestManager::CJT_JOB_END; ++i )
	{
		const std::set<int>& setClassChangeQuestID = changeJobQuest.m_setQuestID[ CX2QuestManager::CHANGE_JOB_TYPE(i) ];
		BOOST_TEST_FOREACH( int, iClassChangeQuestID, setClassChangeQuestID )
		{
			if( NULL != g_pData->GetQuestManager()->GetUnitQuest( iClassChangeQuestID ) ||
				mapQuestClear.end() != mapQuestClear.find( iClassChangeQuestID ) )
			{
				ePickedChangeJobType = static_cast<CX2QuestManager::CHANGE_JOB_TYPE>( i );
				break;
			}
		}
	}

	// 접근 가능한 모든 퀘스트에 대해-_-;
	const map<int, CX2QuestManager::QuestTemplet*> vMapQuest = g_pData->GetQuestManager()->GetMapQuestTemplet();
	map<int, CX2QuestManager::QuestTemplet*>::const_iterator it;
	for( it = vMapQuest.begin(); it != vMapQuest.end(); it++ )
	{
		int QuestID = it->first;
		CX2QuestManager::QuestTemplet* pQuestTemplet = (CX2QuestManager::QuestTemplet*) it->second;
		if(pQuestTemplet == NULL) continue;

#ifdef ADD_SERVER_GROUP
		if( pQuestTemplet->m_iServerGroupID != -1 && pQuestTemplet->m_iServerGroupID != g_pInstanceData->GetServerGroupID() )
			continue;
#endif
		// -1. 에픽 퀘스트는 뺀다.
		if( pQuestTemplet->m_eQuestType == CX2QuestManager::QT_EPIC)
		{
			continue;
		}

		// 0. 이벤트 퀘스트는 뺀다.
		if( pQuestTemplet->m_eQuestType == CX2QuestManager::QT_EVENT
			//{{ kimhc // 2010.2.1 //	PC방 퀘스트
#ifdef	PC_BANG_QUEST
			|| pQuestTemplet->m_eQuestType == CX2QuestManager::QT_PCBANG
#endif	PC_BANG_QUEST
			//}} kimhc // 2010.2.1 //	PC방 퀘스트
			)
			continue;
	
		// 1. 이미 받은 퀘스트면 뺀다.
		if(g_pData->GetQuestManager()->GetUnitQuest(QuestID) != NULL)
			continue;

		// 2. 접근 가능한 퀘스트인지 확인한다.
		if( false == CanIAcceptQuest(QuestID) )
			continue;

		// 3. 이미 클리어한 퀘스트면 뺀다.
		if(mapQuestClear.find(QuestID) != mapQuestClear.end())
			continue;
				
		// 4. 클리어하지 않은 쪽의 전직 퀘스트는 뺀다.

		if ( CX2QuestManager::CJT_JOB_NONE != ePickedChangeJobType )
		{
			bool bFoundJobQuestTobeExcepted = false;
			for ( UINT uiJobType = CX2QuestManager::CJT_JOB_NONE; uiJobType < CX2QuestManager::CJT_JOB_END; ++uiJobType )
			{
				// 완료 했거나 진행 중인 퀘스트 외의 전직 퀘스트는 제외
				if ( ePickedChangeJobType != uiJobType )
				{
					const std::set<int>& setClassChangeQuestID = changeJobQuest.m_setQuestID[ static_cast<CX2QuestManager::CHANGE_JOB_TYPE>( uiJobType ) ];
					if( setClassChangeQuestID.find( QuestID ) != setClassChangeQuestID.end() )
					{
						bFoundJobQuestTobeExcepted = true;
						break;
					}	// if
				}	// if
			}	// for

			if ( true == bFoundJobQuestTobeExcepted )	// 제외 되어야 할 전직 퀘스트 이면
				continue;
		}

		// 5. 레벨이 너무 낮으면 뺀다. (전직 퀘스트 제외)
		if( pQuestTemplet->m_eQuestType == CX2QuestManager::QT_NORMAL || pQuestTemplet->m_eQuestType == CX2QuestManager::QT_SKILL )
		{
			int iLevelGap = g_pData->GetSelectUnitLevel() - pQuestTemplet->m_iFairLevel;
			if( iLevelGap > 5 )
			{
				continue;
			}
		}

		// 6. 해당 NPC가 있는 마을에 갈 수 있는지 검사한다.
		std::map<CX2UnitManager::NPC_UNIT_ID, SEnum::VILLAGE_MAP_ID>::iterator itNPCID = mapNPCID.find(pQuestTemplet->m_eStartNPCID);
		if(itNPCID == mapNPCID.end())
		{
			continue;
		}




				
		
		// 7. 그 수와 최고레벨 퀘스트를 구한다.
		const CX2QuestManager::QuestTemplet* pTopQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( TopQuestID );
		iQuestNum++;
		if(pTopQuestTemplet == NULL || pQuestTemplet->m_iFairLevel >= pTopQuestTemplet->m_iFairLevel )
		{
			TopQuestID = QuestID;
			VID = itNPCID->second;
		}
		
	}

	// 8. 미니맵 쪽으로 쏴준다.	(포인터 유효성은 시작할 때 검사)

}

bool CX2UIQuestNew::CanIAcceptQuest( int iQuestID )
{

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( iQuestID );
	if( NULL == pQuestTemplet )
		return false;


	if( true == g_pData->GetQuestManager()->IsForbiddenQuest( iQuestID ) )
		return false;


	for( int iC = 0; iC < (int)pQuestTemplet->m_Condition.m_vecBeforeQuestID.size(); ++iC )
	{
		if( pQuestTemplet->m_Condition.m_vecBeforeQuestID[iC] != 0 &&
			false == g_pData->GetQuestManager()->GetUnitCompleteQuest( pQuestTemplet->m_Condition.m_vecBeforeQuestID[iC] ) )
		{
			return false;
		}
	}

#ifdef REFORM_QUEST
	const int iSelectUnitLevel = g_pData->GetSelectUnitLevel();
	if( (0 != pQuestTemplet->m_Condition.m_iLevel && iSelectUnitLevel < pQuestTemplet->m_Condition.m_iLevel ) ||
		(-1 != pQuestTemplet->m_Condition.m_iLimitLevel && iSelectUnitLevel > pQuestTemplet->m_Condition.m_iLimitLevel ) )
#else
	if( pQuestTemplet->m_Condition.m_iLevel != 0 &&
		g_pData->GetSelectUnitLevel() < pQuestTemplet->m_Condition.m_iLevel )  
#endif //REFORM_QUEST
	{
		return false;
	}

	if( -1 != pQuestTemplet->m_Condition.m_iConditionItemID && pQuestTemplet->m_Condition.m_iConditionItemNum > 0 )
	{
		int iItemCount = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( pQuestTemplet->m_Condition.m_iConditionItemID );
		if( iItemCount < pQuestTemplet->m_Condition.m_iConditionItemNum )
		{
			return false;
		}
	}



	// 피해야할 퀘스트를 완료한 퀘스트는 받을 수 없다
	if( -1 != pQuestTemplet->m_Condition.m_iNotBeforeQuestID )
	{
		const map<int, int>& mapQuestClear = g_pData->GetQuestManager()->GetMapCompleteQuest();			// 이미 완료, 보상 받은 퀘스트
		if( mapQuestClear.end() != mapQuestClear.find( pQuestTemplet->m_Condition.m_iNotBeforeQuestID ) )
		{
			return false;
		}
	}



	return true;
}

bool CX2UIQuestNew::IsNearVillage( int CurrentVillageID, int TargetVillageID )	// 퀘스트 알리미에서 "PC가 속한 지역"인지를 확인하는 함수.
{
	// 주의 : 새 마을이 추가되면 이 부분을 수정해 줘야 합니다.
	//** 나중에 스크립트로 빼서 초기화할 때 읽어오게 하자!
	if(m_mulmapNearVillageData.empty())
	{
		// 현재 묶인 단위 :
		//  (루벤마을,루벤외곽)
		//	(엘더마을,엘더외곽)
		//	(베스마마을,베스마산맥,페이타)
		//	(알테라마을,알테라섬,페이타)
		// CurrentVillageID랑 TargetVillageID가 서로 묶여있으면 return true
		// 퀘스트 주는 곳은 마을 뿐이니까 마을 :: 던전게이트 pair로 묶어준다

		//  (루벤마을,루벤외곽)
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_RUBEN, (int)SEnum::VMI_DUNGEON_GATE_RUBEN));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_RUBEN, (int)SEnum::VMI_DUNGEON_LOUNGE_RUBEN));
		
		//	(엘더마을,엘더외곽)
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ELDER, (int)SEnum::VMI_DUNGEON_GATE_ELDER));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ELDER, (int)SEnum::VMI_DUNGEON_LOUNGE_ELDER));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ELDER, (int)SEnum::VMI_BATTLE_FIELD_ELDER_REST_00));

		//	(베스마마을,베스마산맥,페이타)
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_BESMA, (int)SEnum::VMI_DUNGEON_GATE_BESMA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_BESMA, (int)SEnum::VMI_PEITA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_BESMA, (int)SEnum::VMI_DUNGEON_LOUNGE_BESMA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_BESMA, (int)SEnum::VMI_DUNGEON_LOUNGE_PEITA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_BESMA, (int)SEnum::VMI_BATTLE_FIELD_BESMA_REST_00));
		//m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_PEITA, (int)SEnum::VMI_PEITA));

		//	(알테라마을,알테라섬,페이타)
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ALTERA, (int)SEnum::VMI_DUNGEON_GATE_ALTERA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ALTERA, (int)SEnum::VMI_PEITA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ALTERA, (int)SEnum::VMI_DUNGEON_LOUNGE_ALTERA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ALTERA, (int)SEnum::VMI_DUNGEON_LOUNGE_PEITA));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_ALTERA, (int)SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00));
		//m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_DUNGEON_GATE_ALTERA, (int)SEnum::VMI_PEITA));

		//  (벨더근교,벨더수도)
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_VELDER, (int)SEnum::VMI_DUNGEON_GATE_VELDER));
		m_mulmapNearVillageData.insert( std::make_pair((int)SEnum::VMI_VELDER, (int)SEnum::VMI_DUNGEON_LOUNGE_VELDER));
	}

	// 두 ID가 같으면 무조건 맞고
	if( CurrentVillageID == TargetVillageID )
	{
		return true;
	}

	// 한 VID로 다른 VID와 매치되는지 검사
	// 위에서 호출할 때 마을 ID가 앞에 오는 경우가 많으니 이 검사를 아래쪽 것보다 먼저 하게 하자
	{
		std::pair<std::multimap<int,int>::iterator, std::multimap<int,int>::iterator> range = m_mulmapNearVillageData.equal_range(TargetVillageID);
		for (std::multimap<int,int>::iterator it = range.first; it != range.second; ++it)
		{
			if(it->second == CurrentVillageID)
				return true;
		}
	}
	
	// 반대로도 검사
	{
		std::pair<std::multimap<int,int>::iterator, std::multimap<int,int>::iterator> range = m_mulmapNearVillageData.equal_range(CurrentVillageID);
		for (std::multimap<int,int>::iterator it = range.first; it != range.second; ++it)
		{
			if(it->second == TargetVillageID)
				return true;
		}
	}
	
	// 같은 묶음이 아니다
	return false;
}


bool CX2UIQuestNew::MouseOnQuestListArea( D3DXVECTOR2 mousePos )
{
	// 퀘스트 목록의 위치와 크기
	D3DXVECTOR2 m_ConvertPos	= g_pKTDXApp->ConvertByResolution( D3DXVECTOR2(15, 75) + m_MovedPosition );
	D3DXVECTOR2 m_ConvertSize	= g_pKTDXApp->ConvertByResolution( D3DXVECTOR2(409, 452) );

	if ( mousePos.x >= m_ConvertPos.x   && mousePos.x <= m_ConvertPos.x + m_ConvertSize.x &&
		mousePos.y >= m_ConvertPos.y && mousePos.y <= m_ConvertPos.y + m_ConvertSize.y )
	{
		return true;
	}

	return false;

}


bool CX2UIQuestNew::TargetQuestisOnScreenList( int TargetQuestID )
{
	for(int i=0; i<m_iQuestPerPage; ++i)
	{
		int NowIndex = m_iTopQuestSlotIndex+i;
		if( NowIndex > (int)m_vecAvailableQuestID.size() )
			return false;

		int QuestID = m_vecAvailableQuestID[NowIndex];
		if(QuestID == TargetQuestID)
			return true;
	}

	return false;
}


void CX2UIQuestNew::InitRewardItemSlot()
{
	for(UINT i=0; i<m_SlotList.size(); i++)
	{
		CX2SlotItem* pSlotItem = (CX2SlotItem*) m_SlotList[i];
		pSlotItem->DestroyItemUI();
		pSlotItem->SetShow(false);
		pSlotItem->SetEnable(false);
	}
	return;
}



void CX2UIQuestNew::UpdateRewardItemSlot()
{
	m_mapItemIDAndSocketID.clear();

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	if( pQuestTemplet != NULL )
	{
		for(int i=0; i<_CONST_UIQUESTNEW_INFO_::g_iRewardSlotNum; i++)
		{
			CX2SlotItem* pSlotItem = (CX2SlotItem*) m_SlotList[i];

			if( i < (int)pQuestTemplet->m_Reward.m_vecItem.size() )
			{
				// 보상슬롯
				const CX2QuestManager::ITEM_DATA& itemData = pQuestTemplet->m_Reward.m_vecItem[i];
                const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemData.m_iItemID );
				if ( itemData.m_iSocketOption1 != 0 )
				{
					m_mapItemIDAndSocketID.insert( std::make_pair( itemData.m_iItemID, itemData.m_iSocketOption1 ) );
				}
				if(pItemTemplet != NULL)
				{
					CX2Item::ItemData kItemData;
					kItemData.m_PeriodType = pItemTemplet->GetPeriodType();
					kItemData.m_SocketOption.push_back(itemData.m_iSocketOption1);
					kItemData.m_ItemID = itemData.m_iItemID;
					kItemData.m_Period = itemData.m_iPeriod;
					kItemData.m_Endurance = pItemTemplet->GetEndurance();
					CX2Item* pItem = new CX2Item( kItemData, NULL);
					if ( pItem != NULL )
					{
						if ( pSlotItem != NULL )
						{
							pSlotItem->CreateItemUI( pItem, itemData.m_iQuantity );
							pSlotItem->SetShow(true);
							pSlotItem->SetEnable(true);
						}
					}
					SAFE_DELETE(pItem);
				}

			}
			else
			{
				// 비우자
				if(pSlotItem != NULL)
				{
					pSlotItem->DestroyItemUI();
					pSlotItem->SetShow(false);
				}
			}
		}

		// 선택보상
		int SlotIndex = 0;
		for(int i=0; SlotIndex < _CONST_UIQUESTNEW_INFO_::g_iRewardSlotNum; i++)
		{
			CX2SlotItem* pSlotItem = (CX2SlotItem*) m_SlotList[SlotIndex+_CONST_UIQUESTNEW_INFO_::g_iRewardSlotNum];

			if( i < (int)pQuestTemplet->m_SelectReward.m_vecSelectItem.size())
			{
				// 선택보상슬롯
				const CX2QuestManager::ITEM_DATA& itemData = pQuestTemplet->m_SelectReward.m_vecSelectItem[i];
				const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemData.m_iItemID );
				//{{10.01.14 김정협 아이템 없을 시 문제 처리
				if(pItemTemplet == NULL)
					continue;
				//}}10.01.14 김정협 아이템 없을 시 문제 처리

				// fix!! 일단 선택보상에서는 unit_type만 체크
				if( pItemTemplet->GetUnitType() != CX2Unit::UT_NONE )
				{
					CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
					if( pItemTemplet->GetUnitType() != eUnitType )
					{
						continue;
					}
				}
#ifdef QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
				if( pItemTemplet->GetUnitClass() != CX2Unit::UC_NONE )
				{
					CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
					if( false == GetX2UnitClassCompatibility(eUnitClass, pItemTemplet->GetUnitClass() ) )
					{
						continue;
					}
				}
#endif QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
				if ( itemData.m_iSocketOption1 != 0 )
				{
					m_mapItemIDAndSocketID.insert( std::make_pair( itemData.m_iItemID, itemData.m_iSocketOption1 ) );
				}
				if(pItemTemplet != NULL)
				{
					CX2Item::ItemData kItemData;
					kItemData.m_PeriodType = pItemTemplet->GetPeriodType();
					kItemData.m_SocketOption.push_back(itemData.m_iSocketOption1);
					kItemData.m_ItemID = itemData.m_iItemID;
					kItemData.m_Period = itemData.m_iPeriod;
					kItemData.m_Endurance = pItemTemplet->GetEndurance();
					CX2Item* pItem = new CX2Item(kItemData, NULL);
					if ( pItem != NULL )
					{
						if ( pSlotItem != NULL )
						{
							pSlotItem->CreateItemUI( pItem, itemData.m_iQuantity );
							pSlotItem->SetShow(true);
							pSlotItem->SetEnable(true);
						}
					}
					SAFE_DELETE(pItem);

				}
			}
			else
			{
				// 비우자
				if(pSlotItem != NULL)
				{
					pSlotItem->DestroyItemUI();
					pSlotItem->SetShow(false);
				}
			}
			SlotIndex++;
		}

	}
	else
	{
		// 다 비워버려 다 미워
		for(UINT i=0; i<m_SlotList.size(); i++)
		{
			CX2SlotItem* pSlotItem = (CX2SlotItem*) m_SlotList[i];
			pSlotItem->DestroyItemUI();
			pSlotItem->SetShow(false);
		}
	}
}


bool CX2UIQuestNew::GetPartyUIQuestIcon( int iDungeonID, int iDungeonDifficulty, int iDungeonMode)
{
	wstringstream wstrm;
	wstring wstrEpicQuest;
	wstring wstrNormalQuest;
	bool bAvailableEpicQuest = false;
	bool bAvailableNormalQuest = false;
	
	const CX2Dungeon::DungeonData* pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID) iDungeonID );

	if( NULL == pDungeonData )
	{
		return false;
	}

	bAvailableEpicQuest = GetPartyUIQuestIcon(m_vecEpicQuestInfo, iDungeonID, iDungeonDifficulty, iDungeonMode);
	bAvailableNormalQuest = GetPartyUIQuestIcon(m_vecOnGoingQuestInfo, iDungeonID, iDungeonDifficulty, iDungeonMode);

	if(bAvailableEpicQuest || bAvailableNormalQuest)
	{
		return true;
	}
	return false;
}



bool CX2UIQuestNew::GetPartyUIQuestIcon( std::vector<QuestInfo>& vecQuestInfo, int iDungeonID, int iDungeonDifficulty, int iDungeonMode)
{
	wstringstream wstrm;
	bool bEmpty = true;

	for(UINT i=0; i< vecQuestInfo.size(); i++)
	{
		QuestInfo kQuestInfo = vecQuestInfo[i];

		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kQuestInfo.m_iID );
		const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( kQuestInfo.m_iID );
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
		if( NULL == pQuestTemplet )
		{
			QuestNoTempletLog( kQuestInfo.m_iID, L"GetPartyUIQuestIcon" );
			continue;
		}
#endif SERV_IRUHADEV_JUNE_QUEST_DATA

		if(pQuestInst == NULL) // oasis907 : 김상윤 [2010.7.11] // 여기서 완료 보고된 에픽퀘스트 걸러짐
		{
			continue;
		}

#ifndef SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
		if(pQuestInst->IsComplete())
		{
			continue;
		}
#endif SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED

		if(kQuestInfo.m_eQuestState == QSS_WAIT_EPIC || 
			kQuestInfo.m_eQuestState == QSS_COMPLETE_EPIC || 
			kQuestInfo.m_eQuestState == QSS_COMPLETE)
		{
			continue;
		}


		for(UINT i= 0; i< pQuestTemplet->m_vecSubQuest.size(); i++)
		{
			const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( pQuestTemplet->m_vecSubQuest[i] );

#ifdef REFORM_QUEST
			vector<int> vecDungeonID;
			GetSubQuestDungeonIDList( pQuestTemplet->m_vecSubQuest[i], vecDungeonID );
			for( vector<int>::iterator it = vecDungeonID.begin(); 
				it != vecDungeonID.end() ; ++it)
			{
				if( *it / 10 == iDungeonID / 10
#ifdef SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
					&& pQuestInst->HasNotCompletedSubQuest( pQuestTemplet->m_vecSubQuest[i] ) == true
	#endif SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
					)
				{
					int iQuestConditionDifficulty = 0;
					int iNowDungeonDifficulty = 0;
					if(pSubQuestTemplet->m_ClearCondition.m_eDungeonMode != CX2Dungeon::DM_INVALID) // 헬모드일 경우
					{
						iQuestConditionDifficulty = pSubQuestTemplet->m_ClearCondition.m_eDungeonMode;
						iNowDungeonDifficulty = iDungeonMode;
					}
					else
					{
						iQuestConditionDifficulty = static_cast<int>( *it );
						iNowDungeonDifficulty = iDungeonID + iDungeonDifficulty;
					}

					///// 임시 코드//// 툴에서 SQT_VISIT_DUNGEON 과 SQT_FIND_NPC 가 UPPER 지정이 안되므로 임시 코드 넣음
					if ( CX2QuestManager::SQT_VISIT_DUNGEON == pSubQuestTemplet->m_eClearType ||
						 CX2QuestManager::SQT_FIND_NPC == pSubQuestTemplet->m_eClearType || 
						 pSubQuestTemplet->m_ClearCondition.m_bUpperDifficulty )
					{
						if(iNowDungeonDifficulty >= iQuestConditionDifficulty)
							return true;
						else
							return false;
					}
					else
					{
						if(iNowDungeonDifficulty == iQuestConditionDifficulty)
							return true;
						else
							return false;
					}
				}
			}
#else
			int SubQuestDungeonID = GetSubQuestDungeonID( pQuestTemplet->m_vecSubQuest[i] );
			if( SubQuestDungeonID / 10 == iDungeonID / 10
	#ifdef SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
				&& pQuestInst->HasNotCompletedSubQuest( pQuestTemplet->m_vecSubQuest[i] ) == true
	#endif SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
				)
			{

				int iQuestConditionDifficulty = 0;
				int iNowDungeonDifficulty = 0;
				if(pSubQuestTemplet->m_ClearCondition.m_eDungeonMode != CX2Dungeon::DM_INVALID) // 헬모드일 경우
				{
					iQuestConditionDifficulty = pSubQuestTemplet->m_ClearCondition.m_eDungeonMode;
					iNowDungeonDifficulty = iDungeonMode;
				}
				else
				{
					iQuestConditionDifficulty = pSubQuestTemplet->m_ClearCondition.m_cDifficulty;
					iNowDungeonDifficulty = iDungeonDifficulty;
				}

				if(true == pSubQuestTemplet->m_ClearCondition.m_bUpperDifficulty)
				{
					if(iNowDungeonDifficulty >= iQuestConditionDifficulty)
						return true;
					else
						return false;
				}
				else
				{
					if(iNowDungeonDifficulty == iQuestConditionDifficulty)
						return true;
					else
						return false;
				}
			}
#endif //REFORM_QUEST
		}

	}
	return false;
}
bool CX2UIQuestNew::GetLocalMapQuestDesc(int iDungeonID, OUT wstring& QuestDesc)
{
	//영웅모집공고에 대한 정보를 퀘스트 정보와 같이 알려주기 위해 변경.

	const CX2Dungeon::DungeonData* pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID) iDungeonID );
	if( NULL == pDungeonData )
		return L"";

	wstring wstrEpicQuest = GetLocalMapQuestDesc(m_vecEpicQuestInfo, iDungeonID);
	wstring wstrNormalQuest = GetLocalMapQuestDesc(m_vecOnGoingQuestInfo, iDungeonID);
	wstring wstrHeroRecruit;
	
	switch(pDungeonData->m_eDungeonType) 
	{ // 영웅 주화 보너스 알림
	case CX2Dungeon::DT_NORMAL:
		{
#ifdef SERV_DUNGEON_OPTION_IN_LUA
			if( pDungeonData->m_bEventDungeon == true)
				break;
#endif SERV_DUNGEON_OPTION_IN_LUA

			if( NULL != g_pData && NULL != g_pData->GetPartyManager() )
			{		
				if( true == g_pData->GetPartyManager()->IsHeroRecruitDungeon(iDungeonID + CX2Dungeon::DL_NORMAL ) )
				{//(보통) : 영웅의 주화 획득 가능
					wstrHeroRecruit += GET_STRING(STR_ID_17438);
				}
				if( true == g_pData->GetPartyManager()->IsHeroRecruitDungeon(iDungeonID + CX2Dungeon::DL_HARD ) )
				{//(어려움) : 영웅의 주화 획득 가능			
					wstrHeroRecruit += GET_STRING(STR_ID_17439);
				}
				if( true == g_pData->GetPartyManager()->IsHeroRecruitDungeon(iDungeonID + CX2Dungeon::DL_EXPERT ) )
				{//(매우 어려움) : 영웅의 주화 획득 가능
					wstrHeroRecruit += GET_STRING(STR_ID_17440);
				}
			}
		}break;
	case CX2Dungeon::DT_SECRET:
		{	
			unsigned int iHellModeIndex = 1;
			if( true == g_pData->GetPartyManager()->IsHeroRecruitDungeon(iDungeonID ) )
			{//(일반) : 영웅의 주화 획득 가능
				wstrHeroRecruit += GET_STRING(STR_ID_17441);
			}
			if( true == g_pData->GetPartyManager()->IsHeroRecruitDungeon(iDungeonID + iHellModeIndex) )
			{//(헬) : 영웅의 주화 획득 가능			
				wstrHeroRecruit += GET_STRING(STR_ID_17442);
			}
		}break;
	default:
		break;
	}

	wstring wstrDungeonName	= L"#CFFFFFF" + pDungeonData->m_DungeonName + L"#CX\n\n" ; 
	QuestDesc = wstrDungeonName;

#ifdef SERV_LIMITED_DUNGEON_PLAY_TIMES
	wstring wstrLimitedPlayTimes;
	if( pDungeonData->m_eDungeonType == CX2Dungeon::DT_NORMAL )
	{
		for( int iDifficultyLevel = CX2Dungeon::DL_NORMAL; iDifficultyLevel <= CX2Dungeon::DL_EXPERT; ++iDifficultyLevel )
		{
			wstring wstrDifficulty;
			switch( iDifficultyLevel )
			{
			case CX2Dungeon::DL_NORMAL:
				{
					wstrDifficulty = wstring( L" (" ) + GET_STRING( STR_ID_385 ) + wstring( L") : " );
				} break;
			case CX2Dungeon::DL_HARD:
				{
					wstrDifficulty = wstring( L" (" ) + GET_STRING( STR_ID_386 ) + wstring( L") : " );
				} break;
			case CX2Dungeon::DL_EXPERT:
				{
					wstrDifficulty = wstring( L" (" ) + GET_STRING( STR_ID_388 ) + wstring( L") : " );
				} break;
			}

			int iDungeonIDWithDifficulty = iDungeonID + iDifficultyLevel;
			wstring wstrDesc = g_pData->GetMyUser()->GetSelectUnit()->GetLocalMapPlayTimesDesc( iDungeonIDWithDifficulty );
			if( wstrDesc != L"" )
			{
				wstrLimitedPlayTimes += wstrDifficulty + wstrDesc;
			}
		}
	}
	else if( pDungeonData->m_eDungeonType == CX2Dungeon::DT_SECRET )
	{
		for( int iDifficultyLevel = 0; iDifficultyLevel <= 1; ++iDifficultyLevel )
		{
			wstring wstrDifficulty;
			switch( iDifficultyLevel )
			{
			case 0:
				{
					wstrDifficulty = wstring( L" (" ) + GET_STRING( STR_ID_938 ) + wstring( L") : " );
				} break;
			case 1:
				{
					wstrDifficulty = wstring( L" (" ) + GET_STRING( STR_ID_4943 ) + wstring( L") : " );
				} break;
			}

			int iDungeonIDWithDifficulty = iDungeonID + iDifficultyLevel;
			wstring wstrDesc = g_pData->GetMyUser()->GetSelectUnit()->GetLocalMapPlayTimesDesc( iDungeonIDWithDifficulty );
			if( wstrDesc != L"" )
			{
				wstrLimitedPlayTimes += wstrDifficulty + wstrDesc;
			}
		}
	}
	
	if( L"" != wstrLimitedPlayTimes )
	{
		QuestDesc += wstrLimitedPlayTimes;
	}
#endif SERV_LIMITED_DUNGEON_PLAY_TIMES

	if( L"" != wstrHeroRecruit )
	{
		QuestDesc += wstrHeroRecruit ;
	}

	bool bShowQuest = true;
	if( L"" == wstrEpicQuest && L"" == wstrNormalQuest )
	{
		if( L"" == wstrHeroRecruit
#ifdef SERV_LIMITED_DUNGEON_PLAY_TIMES
			&& L"" == wstrLimitedPlayTimes
#endif SERV_LIMITED_DUNGEON_PLAY_TIMES
			)
		{
			QuestDesc.clear();
		}
		bShowQuest = false;
		return bShowQuest;
	}

	if( L"" != wstrHeroRecruit )
		wstrHeroRecruit += L"\n";

	//던전이름
	//해당 던전에서 수행가능 퀘스트
	wstring wstrQuestGuide	= L"#CFFFF00" ;
	wstrQuestGuide += GET_STRING(STR_ID_9623);
	wstrQuestGuide += L"#CX\n";

	QuestDesc += wstrQuestGuide + wstrEpicQuest + wstrNormalQuest;

	return bShowQuest;
}



wstring CX2UIQuestNew::GetLocalMapQuestDesc( std::vector<QuestInfo>& vecQuestInfo, int iDungeonID)
{
	wstringstream wstrm;
	bool bEmpty = true;

	int	eLastQuestType = -1;
	int eLastRepeatType = -1;

	for(UINT i=0; i< vecQuestInfo.size(); i++)
	{
		QuestInfo kQuestInfo = vecQuestInfo[i];

		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kQuestInfo.m_iID );
		const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( kQuestInfo.m_iID );
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
		if( NULL == pQuestTemplet )
		{
			QuestNoTempletLog( kQuestInfo.m_iID, L"GetLocalMapQuestDesc" );
			continue;
		}
#endif SERV_IRUHADEV_JUNE_QUEST_DATA

		if(pQuestInst == NULL) // oasis907 : 김상윤 [2010.7.11] // 여기서 완료 보고된 에픽퀘스트 걸러짐
		{
			continue;
		}

#ifndef SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
		if(pQuestInst->IsComplete())
		{
			continue;
		}
#endif SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED

		if(kQuestInfo.m_eQuestState == QSS_WAIT_EPIC || 
			kQuestInfo.m_eQuestState == QSS_COMPLETE_EPIC || 
			kQuestInfo.m_eQuestState == QSS_COMPLETE)
		{
			continue;
		}

		bool bSetLocalMapQuestDesc = false;
		for(UINT i= 0; i< pQuestTemplet->m_vecSubQuest.size(); i++)
		{
#ifdef REFORM_QUEST
			vector<int> vecDungeonID;
			GetSubQuestDungeonIDList( pQuestTemplet->m_vecSubQuest[i], vecDungeonID );
			for( vector<int>::iterator it = vecDungeonID.begin(); 
				it != vecDungeonID.end() ; ++it)
			{
				if( *it / 10 == iDungeonID / 10
	#ifdef SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
					&& pQuestInst->HasNotCompletedSubQuest( pQuestTemplet->m_vecSubQuest[i] ) == true
	#endif SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
					)
				{
					bSetLocalMapQuestDesc = true;
				}
			}
#else
			int SubQuestDungeonID = GetSubQuestDungeonID( pQuestTemplet->m_vecSubQuest[i] );
			if( SubQuestDungeonID / 10 == iDungeonID / 10
	#ifdef SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
				&& pQuestInst->HasNotCompletedSubQuest( pQuestTemplet->m_vecSubQuest[i] ) == true
	#endif SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED
				)
			{
				bSetLocalMapQuestDesc = true;
			}
#endif //REFORM_QUEST
		}
		if(bSetLocalMapQuestDesc == false)
		{
			continue;
		}

		bEmpty = false;
		if(eLastQuestType != pQuestTemplet->m_eQuestType || eLastRepeatType != pQuestTemplet->m_eRepeatType)
		{
			wstring wstrQuestType = L"";
			switch(pQuestTemplet->m_eQuestType)
			{
			case CX2QuestManager::QT_NORMAL:
				{
					switch(pQuestTemplet->m_eRepeatType)
					{
					case CX2QuestManager::QRT_NORMAL:
						//wstrQuestType = L"+일반";	
						wstrQuestType = GET_STRING(STR_ID_9624);
						break;
					case CX2QuestManager::QRT_DAY:
						//wstrQuestType = L"+일일";
						wstrQuestType = GET_STRING(STR_ID_9625);
						break;
					case CX2QuestManager::QRT_REPEAT:
						//wstrQuestType = L"+반복";
						wstrQuestType = GET_STRING(STR_ID_9626);
						break;
					}
				}
				break;
			case CX2QuestManager::QT_EVENT:
				//wstrQuestType = L"+이벤트";
				wstrQuestType = GET_STRING(STR_ID_9627);
				break;
			case CX2QuestManager::QT_CHANGE_JOB:
				//wstrQuestType = L"+전직";
				wstrQuestType = GET_STRING(STR_ID_9628);
				break;
			case CX2QuestManager::QT_SKILL:
				//wstrQuestType = L"+스킬";
				wstrQuestType = GET_STRING(STR_ID_9629);
				break;
			case CX2QuestManager::QT_PCBANG:
				//wstrQuestType = L"+PC방";
				wstrQuestType = GET_STRING(STR_ID_9630);
				break;
			case CX2QuestManager::QT_EPIC:
				//wstrQuestType = L"+에픽";
				wstrQuestType = GET_STRING(STR_ID_9631);
				break;
			case CX2QuestManager::QT_CONTEST:
				//wstrQuestType = L"+경쟁";
				wstrQuestType = GET_STRING(STR_ID_9632);
				break;
			}
			wstrm << L"#CFFFFFF" << wstrQuestType << L"#CX\n";
		}
		wstring wstrTitle = pQuestTemplet->m_wstrTitle;
		wstring wstrTitleColor = g_pData->GetQuestManager()->GetQuestColorString( pQuestTemplet->m_iID, g_pData->GetSelectUnitLevel());

		wstrm << L" " << wstrTitleColor << wstrTitle << L"#CX\n";

		eLastQuestType = pQuestTemplet->m_eQuestType;
		eLastRepeatType = pQuestTemplet->m_eRepeatType;
	}

	if(bEmpty)
	{
		return L"";
	}

	return wstrm.str().c_str();
}



wstring CX2UIQuestNew::CreateStringNPCQuestDescription()
{
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	if(pQuestTemplet == NULL)
		return L"";

	return pQuestTemplet->m_wstrMainText;
}


wstring CX2UIQuestNew::CreateStringQuestClearCondition()
{
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iSelectedQuestID );

	wstringstream wstrm;

	// 퀘스트 완료조건 넣는 부분
	if( pQuestTemplet != NULL )
	{
		// 선택된 퀘가 있으면~
		bool bThereWasNPCTalkSubQuest = false;
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
		// June 323cdfe0: one group holding a "play together" sub-quest lists only the play-together ones
		bool bTogetherOnly = false;
		if( 1 == pQuestTemplet->m_mapSubQuestGroup.size() )
		{
			const std::vector<int>& vecGroup = pQuestTemplet->m_mapSubQuestGroup.begin()->second;
			for( UINT k = 0; k < vecGroup.size(); ++k )
			{
				const CX2QuestManager::SubQuestTemplet* pSub = g_pData->GetQuestManager()->GetSubQuestTemplet( vecGroup[k] );
				if( NULL != pSub && CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR == pSub->m_eClearType )
				{
					bTogetherOnly = true;
					break;
				}
			}
		}
#endif SERV_IRUHADEV_JUNE_QUEST_UI

#ifdef REFORM_QUEST
		// sub-quest
		std::map< int, std::vector<int>>::const_iterator mit = pQuestTemplet->m_mapSubQuestGroup.begin();
		for( ; mit != pQuestTemplet->m_mapSubQuestGroup.end(); ++mit)
		{
			bool bIsSubquestGroupCompelete = true;
			for( UINT i=0; i<mit->second.size(); i++ )
			{
				const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( mit->second[i] );
				if( NULL == pSubQuestTemplet )
					continue;

				const CX2QuestManager::ClearCondition& clearCond = pSubQuestTemplet->m_ClearCondition;
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
				if( true == bTogetherOnly && CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR != pSubQuestTemplet->m_eClearType )
					continue;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
				if( CX2QuestManager::SQT_NPC_TALK == pSubQuestTemplet->m_eClearType )
				{
					bThereWasNPCTalkSubQuest = true;
				}

				wstring wstrSubQuestDesc = L"";
				wstrSubQuestDesc = g_pData->GetQuestManager()->MakeSubQuestString( pSubQuestTemplet->m_bAutomaticDescription, pSubQuestTemplet, pQuestInst );

				if( 0 != wstrSubQuestDesc.length() )
				{
					if( NULL  !=  pQuestInst )
					{
						const CX2QuestManager::SubQuestInst* pSubQuestInst= pQuestInst->GetSubQuest( mit->second[i]);
						if( NULL == pSubQuestInst )
						{							
							ErrorLogMsg( XEM_ERROR79, L"GetSubQuestFail");
						}
						else
						{
							if( true == pSubQuestInst->IsComplete() )
							{
								// 완료된 거
								wstrm << L"#CAA0000" << wstrSubQuestDesc << L"#CX\n";
							}
							else
							{
								// 아~직
								wstrm << L"#C281CC0" << wstrSubQuestDesc << L"#CX\n";;
								bIsSubquestGroupCompelete = false;
							}
						}					
					}
					else
					{
						wstrm << L"#C281CC0" << wstrSubQuestDesc << L"#CX\n";;
					}
				}
			}

			if( false == bIsSubquestGroupCompelete )
			{//해당 그룹의 서브퀘스트가 모두 클리어되지 않았다면 해당 그룹의 내용만 보여주기
				break;			 
			}
		}
#else
		// sub-quest
		for( UINT i=0; i<pQuestTemplet->m_vecSubQuest.size(); i++ )
		{
			const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( pQuestTemplet->m_vecSubQuest[i] );
			if( NULL == pSubQuestTemplet )
				continue;

			const CX2QuestManager::ClearCondition& clearCond = pSubQuestTemplet->m_ClearCondition;
			if( CX2QuestManager::SQT_NPC_TALK == pSubQuestTemplet->m_eClearType )
			{
				bThereWasNPCTalkSubQuest = true;
			}

			wstring wstrSubQuestDesc = L"";
			wstrSubQuestDesc = g_pData->GetQuestManager()->MakeSubQuestString( pSubQuestTemplet->m_bAutomaticDescription, pSubQuestTemplet, pQuestInst );

			if( 0 != wstrSubQuestDesc.length() )
			{
				if( 
#ifndef FIX_TALK_QUEST_COMPLETE_COLOR
					CX2QuestManager::SQT_NPC_TALK != pSubQuestTemplet->m_eClearType &&
#endif
					NULL != pQuestInst && true == pQuestInst->GetSubQuest(pQuestTemplet->m_vecSubQuest[i])->IsComplete() )
				{
					// 완료된 거
					wstrm << L"#CAA0000" << wstrSubQuestDesc << L"#CX\n";
				}
				else
				{
					// 아~직
					wstrm << L"#C281CC0" << wstrSubQuestDesc << L"#CX\n";;
				}
			}
		}
#endif REFORM_QUEST
	}

	return wstrm.str().c_str();
}




void CX2UIQuestNew::ProcessQuestDesc( const wstring& wstrDesc, const int iLineWidth, CKTDGFontManager::CUKFont* pFont )
{
	// 일단 wstrDesc를 만들어내자-_-
	// 클리어 조건 -> 
	enum CHAR_STATE
	{
		Q_NORMAL,
		Q_SHARP,
		Q_SHARP_C,
	};

	m_vQuestDescSize = D3DXVECTOR2( (float) m_pDLGUIQuestDetail->GetDummyInt( 0 ), (float) m_pDLGUIQuestDetail->GetDummyInt( 1 ) );

	const int MAGIC_MAX_WIDTH = (int) (g_pKTDXApp->GetResolutionScaleX() * iLineWidth);

	m_vecQuestDesc.resize(0);

	if( wstrDesc.length() == 0 )
	{
		m_iQuestDescIndex = 0;
		return;
	}

	int iNowCharWidth = 0;		
	int iNowLineWidth = 0;
	int iNowCharIndex = 0;
	wstring wstrLineBuf = L"";
	wstring wstrPageBuf = L"";
	wstring wstrColorCode = L"";

	CHAR_STATE eNowCharState = Q_NORMAL;
	bool bIsFirstCharOfLine = true; 

	int iDescLength = (int) wstrDesc.length();
	while( iNowCharIndex < iDescLength )
	{
		WCHAR wszCharBuf = wstrDesc[iNowCharIndex];
		wstring wstrCharBuf = wstrDesc.substr( iNowCharIndex, 1 ); 

		switch( eNowCharState )
		{
		case Q_NORMAL:
			{
				if( 0 == wstrCharBuf.compare( L"#" ) )
				{
					wstrLineBuf += wstrCharBuf;
					eNowCharState = Q_SHARP;
				}
				else if( 0 == wstrCharBuf.compare( L"\n" ) )
				{
					wstrLineBuf += wstrCharBuf;
					wstrPageBuf += wstrLineBuf;
					wstrLineBuf = L"";
					iNowLineWidth = 0;

					if( 0 != wstrColorCode.length() )
					{
						wstrLineBuf += wstrColorCode;
					}
					if(false == bIsFirstCharOfLine)
					{
						m_vecQuestDesc.push_back( wstrPageBuf );
					}
					bIsFirstCharOfLine = true;
					wstrPageBuf = L"";

				}
				else
				{
					if( 0 != wstrCharBuf.compare( L" " ) || false == bIsFirstCharOfLine )
					{
						if( true == bIsFirstCharOfLine )
							bIsFirstCharOfLine = false;
						wstrLineBuf += wstrCharBuf;					
						iNowCharWidth = pFont->GetWidth( wszCharBuf );
						iNowLineWidth += iNowCharWidth;
					}
				}

			} break;

		case Q_SHARP:
			{
				if( 0 == wstrCharBuf.compare( L"C" ) )
				{
					wstrLineBuf += wstrCharBuf;
					eNowCharState = Q_SHARP_C;
				}
				else if( 0 == wstrCharBuf.compare( L"m" ) )
				{
					wstrLineBuf.erase( wstrLineBuf.end()-1 ); // '#'을 지우자
					wstrPageBuf += wstrLineBuf;
					if(false == bIsFirstCharOfLine)
					{
						m_vecQuestDesc.push_back( wstrPageBuf );
					}
					wstrLineBuf = L"";
					wstrPageBuf = L"";
					bIsFirstCharOfLine = true;
					if( 0 != wstrColorCode.length() )
					{
						wstrLineBuf += wstrColorCode;
					}

					eNowCharState = Q_NORMAL;
				}
				else
				{
					wstrLineBuf += wstrCharBuf;
					iNowCharWidth = pFont->GetWidth( wszCharBuf );
					iNowLineWidth += iNowCharWidth;

					eNowCharState = Q_NORMAL;
				}

			} break;

		case Q_SHARP_C:
			{
				if( 0 == wstrCharBuf.compare( L"X" ) )
				{
					wstrLineBuf += wstrCharBuf;
					wstrColorCode = L"";
					eNowCharState = Q_NORMAL;
				}
				else // RGB 
				{
					wstrColorCode = L"#C";
					wstrColorCode += wstrDesc.substr( iNowCharIndex, 6 );
					wstrLineBuf += wstrDesc.substr( iNowCharIndex, 6 );
					iNowCharIndex += 5;
					eNowCharState = Q_NORMAL;
				}

			} break;
		}

		if( iNowLineWidth >= MAGIC_MAX_WIDTH && Q_NORMAL == eNowCharState )
		{
			wstrLineBuf += L"\n";
			wstrPageBuf += wstrLineBuf;
			wstrLineBuf = L"";
			iNowLineWidth = 0;
			if( 0 != wstrColorCode.length() )
			{
				wstrLineBuf += wstrColorCode;
			}

			if(false == bIsFirstCharOfLine)
			{
				m_vecQuestDesc.push_back( wstrPageBuf );
			}
			bIsFirstCharOfLine = true;
			wstrPageBuf = L"";
		}

		iNowCharIndex++;
	}

	if( wstrLineBuf.length() > 0 )
	{
		wstrPageBuf += wstrLineBuf;
	}

	if( wstrPageBuf.length() > 0 )
	{
		m_vecQuestDesc.push_back( wstrPageBuf );
	}

	m_iQuestDescIndex = 0;

}

void CX2UIQuestNew::ShowUIDesc( bool bShow, wstring wstr /*= L""*/, D3DXVECTOR2 pos /*= D3DXVECTOR2(0,0)*/, CX2TalkBoxManagerImp::TALK_BOX_TYPE TBT /*= CX2TalkBoxManagerImp::TBT_FROM_UP_RIGHT*/, D3DXCOLOR coTextColor /*= D3DXCOLOR(0,0,0,1)*/, D3DXCOLOR coBackColor /*= D3DXCOLOR(1, 0.96f, 0.6f, 1 )*/)
{
	m_pTalkBoxManager->Clear();

	if(bShow)
	{
		CX2TalkBoxManagerImp::TalkBox talkBox;

		talkBox.m_vPos				= pos;
		talkBox.m_GameUnitType		= CX2GameUnit::GUT_USER;
		talkBox.m_fRemainTime		= 2.0f;

		talkBox.m_wstrTalkContent	= wstr.c_str();
		talkBox.m_bTraceUnit		= false;
		talkBox.m_TalkBoxType		= TBT;
		talkBox.m_coTextColor		= coTextColor;
		talkBox.m_BackTexColor		= coBackColor;

		m_pTalkBoxManager->Push( talkBox );

	}
}


void CX2UIQuestNew::UpdateQuestClearCondition()
{

	CKTDGUIListBox *pListboxClearCondition = (CKTDGUIListBox*)m_pDLGUIQuestDetail->GetControl(L"ListboxClearCondition");
	pListboxClearCondition->RemoveAllItems();

	if( m_vecQuestDesc.size() == 0 )
	{
		return;
	}
	

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );

	if(pQuestTemplet == NULL)
		return;

	//if(m_vecUIQuestSlotInfo[m_iSelectedQuestSlotIndex].m_eQuestState == QSS_WAIT_EPIC)
	if(g_pData->GetSelectUnitLevel() < pQuestTemplet->m_iPlayLevel)
	{
		wstring wstrClearCondition = GET_REPLACED_STRING( ( STR_ID_9622, "i", pQuestTemplet->m_iPlayLevel ) );
		
		pListboxClearCondition->AddItem(wstrClearCondition.c_str(), NULL);
	}

	for(UINT i = 0; i<m_vecQuestDesc.size(); i++)
	{
		pListboxClearCondition->AddItem(m_vecQuestDesc[i].c_str(), NULL);
	}
	return;
}

void CX2UIQuestNew::UpdateNPCQuestDescription()
{

	CKTDGUIListBox *pListboxNPCDescription = (CKTDGUIListBox*)m_pDLGUIQuestDetail->GetControl(L"ListboxNPCDescription");
	pListboxNPCDescription->RemoveAllItems();

	if( m_vecQuestDesc.size() == 0 )
	{
		return;
	}

	for(UINT i = 0; i<m_vecQuestDesc.size(); i++)
	{
		pListboxNPCDescription->AddItem(m_vecQuestDesc[i].c_str(), NULL);
	}
	return;
}

wstring CX2UIQuestNew::GetNPCFaceTextureKey(CX2UnitManager::NPC_UNIT_ID NpcID)
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323cc280: the NPC face key in DLG_UI_Npc_Face01. The ids are June's NPC_UNIT_ID values ( Enum.lua ); 10142 is the story
/// quest NPC and 10153 the transcendence one. June dropped the event NPCs March had ( crayonpop, apink, benders ): they fall to Default.
	switch( static_cast<int>( NpcID ) )
	{
	case CX2UnitManager::NUI_ANNE:	return L"ANN";
	case CX2UnitManager::NUI_HAGERS:	return L"HAGUS";
	case CX2UnitManager::NUI_LOW:	return L"ROU";
	case CX2UnitManager::NUI_ADAMS:	return L"ADAMS";
	case CX2UnitManager::NUI_ECHO:	return L"ECO";
	case CX2UnitManager::NUI_HOFMANN:	return L"HOFMAN";
	case CX2UnitManager::NUI_LUICHEL:	return L"RUICHEL";
	case CX2UnitManager::NUI_LENPAD:	return L"RENPAT";
	case CX2UnitManager::NUI_STELLA:	return L"STELRA";
	case CX2UnitManager::NUI_CHACHABUK:	return L"CHACHABUK";
	case CX2UnitManager::NUI_RICHANG:	return L"RICHIANG";
	case CX2UnitManager::NUI_TOMA:	return L"TOMA";
	case CX2UnitManager::NUI_ADEL:	return L"ADEL";
	case CX2UnitManager::NUI_AGATHA:	return L"AGATA";
	case CX2UnitManager::NUI_AIDA:	return L"AIDA";
	case CX2UnitManager::NUI_AMOS:	return L"AMOSE";
	case CX2UnitManager::NUI_MOON:	return L"";				// NUI_MOON: empty, as March
	case CX2UnitManager::NUI_CAMILLA:	return L"KAMILRA";
	case CX2UnitManager::NUI_ARIEL:	return L"ARIEL";
	case CX2UnitManager::NUI_HELLEN:	return L"HELREN";
	case CX2UnitManager::NUI_ALLEGRO:	return L"ALREGRO";
	case CX2UnitManager::NUI_LENTO:	return L"RENTO";
	case CX2UnitManager::NUI_GLAVE:	return L"GLAVE";
	case CX2UnitManager::NUI_LURIEL:	return L"LURIEL";
	case CX2UnitManager::NUI_GRAIL:	return L"GRAIL";
	case CX2UnitManager::NUI_NOEL:	return L"NOEL";
	case CX2UnitManager::NUI_VANESSA:	return L"VANESSA";
	case CX2UnitManager::NUI_PRAUS:	return L"PRAUS";
	case CX2UnitManager::NUI_HANNA:	return L"HANNA";
	case CX2UnitManager::NUI_HORATIO:	return L"HORATIO";
	case CX2UnitManager::NUI_LUCY:	return L"LUCY";
	case CX2UnitManager::NUI_DAISY:	return L"DAISY";
	case CX2UnitManager::NUI_PENENSIO:	return L"PENENSIO";
	case CX2UnitManager::NUI_DENKA:	return L"DENKA";
	case CX2UnitManager::NUI_LORDROS:	return L"LORDROS";
	case CX2UnitManager::NUI_HELLPUTT:	return L"Default";
	case CX2UnitManager::NUI_MU:	return L"MU";
	case CX2UnitManager::NUI_LUTO_VILLAGE:	return L"RUTO";
	case CX2UnitManager::NUI_ROSEANG:	return L"ROSEANG";
	case CX2UnitManager::NUI_EMIRATE:	return L"EMIRATE";
	case CX2UnitManager::NUI_VAPOR:	return L"VAPOR";
	case CX2UnitManager::NUI_DAPPAR:	return L"DAPPAR";
	case CX2UnitManager::NUI_EVENT_ADAMS_UI_SHOP:	return L"EDEL";	// June NUI_EDEL = 10067 ( March's NUI_EDEL is 10072 )
	case CX2UnitManager::NUI_STEEL:	return L"STEEL";
	case CX2UnitManager::NUI_FACEOFF:	return L"FACEOFF";
	case CX2UnitManager::NUI_SYOTA:	return L"SYOTA";
	case CX2UnitManager::NUI_IGNIA:	return L"IGNIA";
	case CX2UnitManager::NUI_YUNO:	return L"YUNO";
	case CX2UnitManager::NUI_STORY_QUEST:	return L"Story";
	case CX2UnitManager::NUI_BERNARD:	return L"VERENADE";
	case CX2UnitManager::NUI_DURANDA:	return L"DYURENDA";
	case CX2UnitManager::NUI_THEODOR:	return L"TEODOR";
	case CX2UnitManager::NUI_HUGO:	return L"HUGO";
	case CX2UnitManager::NUI_TRANSCEND_QUEST:	return L"Trans";
	case CX2UnitManager::NUI_PANE:	return L"PANE";
	}
	return L"Default";
#else SERV_IRUHADEV_JUNE_QUEST_UI
	wstring wstrKey;

	switch( NpcID )
	{
	case CX2UnitManager::NUI_ANNE:		wstrKey = L"ANN"; break;
	case CX2UnitManager::NUI_HAGERS:	wstrKey = L"HAGUS"; break;
	case CX2UnitManager::NUI_LOW:		wstrKey = L"ROU"; break;		
	case CX2UnitManager::NUI_ADAMS:		wstrKey = L"ADAMS"; break;
	case CX2UnitManager::NUI_ECHO:		wstrKey = L"ECO"; break;
	case CX2UnitManager::NUI_HOFMANN:	wstrKey = L"HOFMAN"; break;
	case CX2UnitManager::NUI_LUICHEL:	wstrKey = L"RUICHEL"; break;
	case CX2UnitManager::NUI_LENPAD:	wstrKey = L"RENPAT"; break;
	case CX2UnitManager::NUI_STELLA:	wstrKey = L"STELRA"; break;
	case CX2UnitManager::NUI_CHACHABUK:	wstrKey = L"CHACHABUK"; break;
	case CX2UnitManager::NUI_RICHANG:	wstrKey = L"RICHIANG"; break;
	case CX2UnitManager::NUI_TOMA:		wstrKey = L"TOMA"; break;
	case CX2UnitManager::NUI_ADEL:		wstrKey = L"ADEL"; break;
	case CX2UnitManager::NUI_AGATHA:	wstrKey = L"AGATA"; break;
	case CX2UnitManager::NUI_AIDA:		wstrKey = L"AIDA"; break;
	case CX2UnitManager::NUI_AMOS:		wstrKey = L"AMOSE"; break;
	case CX2UnitManager::NUI_MOON:		/*wstrKey = L"ANN";*/ break;
	case CX2UnitManager::NUI_CAMILLA:	wstrKey = L"KAMILRA"; break;
	case CX2UnitManager::NUI_ARIEL:		wstrKey = L"ARIEL"; break;
	case CX2UnitManager::NUI_HELLEN:	wstrKey = L"HELREN"; break;
	case CX2UnitManager::NUI_ALLEGRO:	wstrKey = L"ALREGRO"; break;
	case CX2UnitManager::NUI_LENTO:		wstrKey = L"RENTO"; break;
	case CX2UnitManager::NUI_GLAVE:		wstrKey = L"GLAVE"; break;
	case CX2UnitManager::NUI_LURIEL:	wstrKey = L"LURIEL"; break;
	case CX2UnitManager::NUI_GRAIL:		wstrKey = L"GRAIL"; break;
	case CX2UnitManager::NUI_NOEL:		wstrKey = L"NOEL"; break;
	case CX2UnitManager::NUI_VANESSA:	wstrKey = L"VANESSA"; break;
	case CX2UnitManager::NUI_PRAUS:		wstrKey = L"PRAUS"; break;
	case CX2UnitManager::NUI_HANNA:		wstrKey = L"HANNA"; break;
	// oasis907 : 김상윤 [2011.1.27] // 하멜 NPC
	case CX2UnitManager::NUI_HORATIO:	wstrKey = L"HORATIO"; break;
	case CX2UnitManager::NUI_LUCY:		wstrKey = L"LUCY"; break;
	case CX2UnitManager::NUI_DAISY:		wstrKey = L"DAISY"; break;
	case CX2UnitManager::NUI_PENENSIO:	wstrKey = L"PENENSIO"; break;		
	case CX2UnitManager::NUI_DENKA:		wstrKey = L"DENKA"; break;
	case CX2UnitManager::NUI_LORDROS:	wstrKey = L"LORDROS"; break;
	case CX2UnitManager::NUI_HELLPUTT:	wstrKey = L"Default"; break;
#ifdef SERV_PSHOP_AGENCY
	case CX2UnitManager::NUI_MU:		wstrKey = L"MU"; break;
#endif
#ifdef NEW_HENIR_TEST
	case CX2UnitManager::NUI_LUTO_VILLAGE:		wstrKey = L"RUTO"; break;
#endif NEW_HENIR_TEST
#ifdef APINK_NPC
	case CX2UnitManager::NUI_EVENT_CRAYONPOP: wstrKey = L"CRAYONPOP "; break;
#endif
#ifdef APINK_ARCHANGEL_NPC
	case CX2UnitManager::NUI_EVENT_APINK_ARCHANGEL: wstrKey = L"NAUN"; break;
#endif //APINK_ARCHANGEL_NPC

#ifdef SERV_APRIL_FOOLS_DAY
	case CX2UnitManager::NUI_EVENT_BENDERS:	wstrKey = L"Andre_Benders"; break;
#endif //APRIL_FOOLS_DAY

#ifdef VILLAGE_SANDER
	case CX2UnitManager::NUI_ROSEANG:	wstrKey = L"ROSEANG";	break;
	case CX2UnitManager::NUI_EMIRATE:	wstrKey = L"EMIRATE";	break;
	case CX2UnitManager::NUI_VAPOR:		wstrKey = L"VAPOR";		break;
	case CX2UnitManager::NUI_DAPPAR:	wstrKey = L"DAPPAR";	break;
#endif VILLAGE_SANDER
	default:							wstrKey = L"Default"; break;
	}
	return wstrKey;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}



void CX2UIQuestNew::UpdateDetailQuestTitle()
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323ce6f0: the title row and the pictures of the detail window ( June's picture numbers, the event NPCs' face )
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iSelectedQuestID );
	if( NULL == pQuestTemplet )
		return;

	CKTDGUIStatic* pStaticQuestTitle = (CKTDGUIStatic*) m_pDLGUIQuestDetail->GetControl( L"StaticQuestTitle" );
	pStaticQuestTitle->SetString( 0, pQuestTemplet->m_wstrTitle.c_str() );

#ifdef ELLIPSE_GLOBAL
	bool bEllipse = false;
	std::wstring wstrTitle = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse( pQuestTemplet->m_wstrTitle.c_str(), 260, pStaticQuestTitle->GetString( 0 )->fontIndex, 1, bEllipse );

	CKTDGUIButton* pButtonQuestTitle = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"QuestTitleSystem" );
	if( NULL != pButtonQuestTitle )
	{
		if( true == bEllipse )
			pButtonQuestTitle->SetGuideDesc( pQuestTemplet->m_wstrTitle.c_str() );
		pButtonQuestTitle->SetShow( bEllipse );
	}
	pStaticQuestTitle->SetString( 0, wstrTitle.c_str() );
#endif ELLIPSE_GLOBAL

	// June 323ce6f0 @323ce89a: the red developer suffix "( id, repeat, time event, level, class )" for auth level 2 and up ( March:
	// XUAL_DEV and up under SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER, without W / M ). Strings 334939bc .. 3345732c.
	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		wstringstream wstrstm;
#ifdef ELLIPSE_GLOBAL
		wstrstm << wstrTitle;
#else ELLIPSE_GLOBAL
		wstrstm << pQuestTemplet->m_wstrTitle;
#endif ELLIPSE_GLOBAL
		wstrstm << L"#CFF0000(" << pQuestTemplet->m_iID << L", ";
		switch( pQuestTemplet->m_eRepeatType )
		{
		case CX2QuestManager::QRT_NORMAL:	wstrstm << L"N";	break;
		case CX2QuestManager::QRT_DAY:		wstrstm << L"D";	break;
		case CX2QuestManager::QRT_REPEAT:	wstrstm << L"R";	break;
		case CX2QuestManager::QRT_WEEK:		wstrstm << L"W";	break;
		case CX2QuestManager::QRT_MONTH:	wstrstm << L"M";	break;
		default:							wstrstm << L"?";	break;
		}
		wstrstm << L", " << ( true == pQuestTemplet->m_bIsTimeEvent ? L"T" : L"F" );
		wstrstm << L", " << pQuestTemplet->m_Condition.m_iLevel << L", ";

		bool bSelected = ( CX2Unit::UT_NONE != pQuestTemplet->m_Condition.m_eUnitType );
		if( false == bSelected )
		{
			std::set< CX2Unit::UNIT_CLASS >::const_iterator sit;
			for( sit = pQuestTemplet->m_Condition.m_setUnitClass.begin(); sit != pQuestTemplet->m_Condition.m_setUnitClass.end(); ++sit )
			{
				if( CX2Unit::UC_NONE != *sit )
				{
					bSelected = true;
					break;
				}
			}
		}
		wstrstm << ( true == bSelected ? L"S" : L"A" ) << L")";
		pStaticQuestTitle->SetString( 0, wstrstm.str().c_str() );
	}

	// pictures 0..12 and 15..19 off ( 13 is the face, 14 the frame )
	for( int iPic = 0; iPic <= 12; ++iPic )
		pStaticQuestTitle->GetPicture( iPic )->SetShow( false );
	for( int iPic = 15; iPic <= 19; ++iPic )
		pStaticQuestTitle->GetPicture( iPic )->SetShow( false );

	const QUEST_TYPE_SORT eQuestTypeSort = ConvertToQuestTypeSort( pQuestTemplet->m_eQuestType );
	const REPEAT_TYPE_SORT eRepeatTypeSort = ConvertToRepeatTypeSort( pQuestTemplet->m_eRepeatType );
	pStaticQuestTitle->GetPicture( GetDetailQuestPictureNumber( eQuestTypeSort, eRepeatTypeSort ) )->SetShow( true );

	QUEST_STATE_SORT eQuestStateSort = QSS_AVAILABLE;
	if( NULL != pQuestInst )
		eQuestStateSort = ( true == pQuestInst->IsComplete() ) ? QSS_COMPLETE : QSS_ONGOING;
	pStaticQuestTitle->GetPicture( GetQuestStatePictureNumber( eQuestStateSort ) )->SetShow( true );

	SetNpcFacePicture( pStaticQuestTitle->GetPicture( 13 ), (int)pQuestTemplet->m_eStartNPCID, GetNPCFaceTextureKey( pQuestTemplet->m_eStartNPCID ) );
#else
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_iSelectedQuestID );

	if(pQuestTemplet == NULL)
		return;

	CKTDGUIStatic* pStaticQuestTitle = (CKTDGUIStatic*) m_pDLGUIQuestDetail->GetControl( L"StaticQuestTitle" );
	pStaticQuestTitle->SetString(0, pQuestTemplet->m_wstrTitle.c_str());

#ifdef ELLIPSE_GLOBAL
	bool bEllipse = false;
	
#ifdef UNIQUENESS_EU_ONLY
	std::wstring wstrTitle = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(pQuestTemplet->m_wstrTitle.c_str(), 280, pStaticQuestTitle->GetString(0)->fontIndex, 1, bEllipse);
#else //UNIQUENESS_EU_ONLY
	std::wstring wstrTitle = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(pQuestTemplet->m_wstrTitle.c_str(), 260, pStaticQuestTitle->GetString(0)->fontIndex, 1, bEllipse);
#endif 	//UNIQUENESS_EU_ONLY	

	if(bEllipse == true)
	{
		CKTDGUIButton* pButtonQuestTitle = (CKTDGUIButton*)m_pDLGUIQuestDetail->GetControl(L"QuestTitleSystem");

		if(pButtonQuestTitle != NULL)
		{
			pButtonQuestTitle->SetGuideDesc(pQuestTemplet->m_wstrTitle.c_str());
			pButtonQuestTitle->SetShow(true);
		}
	}
	else
	{
		CKTDGUIButton* pButtonQuestTitle = (CKTDGUIButton*)m_pDLGUIQuestDetail->GetControl(L"QuestTitleSystem");

		if(pButtonQuestTitle != NULL)
		{
			pButtonQuestTitle->SetShow(false);
		}
	}

	pStaticQuestTitle->SetString(0, wstrTitle.c_str());
#endif ELLIPSE_GLOBAL

#ifdef SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER
	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		wstringstream wstrstm;
#ifdef ELLIPSE_GLOBAL
		wstrstm << wstrTitle;
#else
		wstrstm << pQuestTemplet->m_wstrTitle;
#endif ELLIPSE_GLOBAL
		wstrstm << L"#CFF0000(" << pQuestTemplet->m_iID << L", ";
		switch( pQuestTemplet->m_eRepeatType )
		{
		case CX2QuestManager::QRT_NORMAL:
			wstrstm << L"N";
			break;
		case CX2QuestManager::QRT_DAY:
			wstrstm << L"D";
			break;
		case CX2QuestManager::QRT_REPEAT:
			wstrstm << L"R";
			break;
		default:
			wstrstm << L"?";
			break;
		}
		wstrstm << L", ";

#ifdef SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER_FIX
		wstrstm << ( pQuestTemplet->m_bStartTimeEvent ? L"T" : L"F" );
#else // SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER_FIX
		wstrstm << ( pQuestTemplet->m_bIsTimeEvent ? L"T" : L"F" );
#endif // SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER_FIX
		wstrstm << L", ";

		wstrstm << pQuestTemplet->m_Condition.m_iLevel;
		wstrstm << L", ";

		bool bSelected = false;
		if( pQuestTemplet->m_Condition.m_eUnitType != CX2Unit::UT_NONE )
		{
			bSelected = true;
		}
		else
		{
			std::set< CX2Unit::UNIT_CLASS >::iterator sit;
			for( sit = pQuestTemplet->m_Condition.m_setUnitClass.begin(); sit != pQuestTemplet->m_Condition.m_setUnitClass.end(); ++sit )
			{
				if( *sit != CX2Unit::UC_NONE )
				{
					bSelected = true;
					break;
				}
			}
		}
		wstrstm << ( bSelected ? L"S" : L"A" );
		wstrstm << L")";

		pStaticQuestTitle->SetString(0, wstrstm.str().c_str() );
	}
#endif SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER

	pStaticQuestTitle->GetPicture(0)->SetShow(false);
	pStaticQuestTitle->GetPicture(1)->SetShow(false);
	pStaticQuestTitle->GetPicture(2)->SetShow(false);		
	pStaticQuestTitle->GetPicture(3)->SetShow(false);		
	pStaticQuestTitle->GetPicture(4)->SetShow(false);
	pStaticQuestTitle->GetPicture(5)->SetShow(false);
	pStaticQuestTitle->GetPicture(6)->SetShow(false);
	pStaticQuestTitle->GetPicture(7)->SetShow(false);
	pStaticQuestTitle->GetPicture(8)->SetShow(false);
	pStaticQuestTitle->GetPicture(9)->SetShow(false);
	pStaticQuestTitle->GetPicture(10)->SetShow(false);
	pStaticQuestTitle->GetPicture(11)->SetShow(false);
	pStaticQuestTitle->GetPicture(12)->SetShow(false);

	QUEST_TYPE_SORT eQuestTypeSort = ConvertToQuestTypeSort(pQuestTemplet->m_eQuestType);
	REPEAT_TYPE_SORT eRepeatTypeSort = ConvertToRepeatTypeSort(pQuestTemplet->m_eRepeatType);

	int iQuestTypePictureNumber = GetQuestPictureNumber(eQuestTypeSort, eRepeatTypeSort);
	pStaticQuestTitle->GetPicture(iQuestTypePictureNumber)->SetShow(true);


	QUEST_STATE_SORT eQuestStateSort = QSS_AVAILABLE;
	if(pQuestInst != NULL)
	{
		if( true == pQuestInst->IsComplete() )
		{
			eQuestStateSort = QSS_COMPLETE;
		}
		// FAIL 추가해야함
		else
		{
			eQuestStateSort = QSS_ONGOING;
		}
	}

	int iQuestStatePictureNumber = GetQuestPictureNumber(eQuestStateSort);
	pStaticQuestTitle->GetPicture(iQuestStatePictureNumber)->SetShow(true);

	wstring wstrKey = GetNPCFaceTextureKey(pQuestTemplet->m_eStartNPCID);
	pStaticQuestTitle->GetPicture(13)->SetTex(L"DLG_UI_Npc_Face01.TGA", wstrKey.c_str());
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}



void CX2UIQuestNew::UpdateRewardInfo()
{
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );

	if(pQuestTemplet == NULL)
		return;


	WCHAR wszText[64] = {0,};
	CKTDGUIStatic* pStaticED = (CKTDGUIStatic*) m_pDLGUIQuestDetail->GetControl( L"Static_ED" );
	//wsprintf( wszText, L"%d", pQuestTemplet->m_Reward.m_iED );
	StringCchPrintf( wszText, 64, L"%d", pQuestTemplet->m_Reward.m_iED );
	pStaticED->GetString(0)->msg = wszText;

	CKTDGUIStatic* pStaticExp = (CKTDGUIStatic*) m_pDLGUIQuestDetail->GetControl( L"Static_Exp" );
	//wsprintf( wszText, L"%d", pQuestTemplet->m_Reward.m_iEXP );
	StringCchPrintf( wszText, 64, L"%d", pQuestTemplet->m_Reward.m_iEXP );
	pStaticExp->GetString(0)->msg = wszText;

	CKTDGUIStatic* pStaticSP = (CKTDGUIStatic*) m_pDLGUIQuestDetail->GetControl( L"Static_SP" );
	//wsprintf( wszText, L"%d", pQuestTemplet->m_Reward.m_iSP );
	StringCchPrintf( wszText, 64, L"%d", pQuestTemplet->m_Reward.m_iSP );
	pStaticSP->GetString(0)->msg = wszText;		

	return;
}

void CX2UIQuestNew::UpdateQuestButton()
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323ccbc0: which of the detail window's buttons show for the selected quest. The Giveup / Accept / Complete ... buttons are the
/// same as March's; new are the skip-epic button, the guild rule ( none ), the story states, and a Remind that needs a scene row.
	CKTDGUIButton* pButtonGiveup		= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Giveup" );
	CKTDGUIButton* pButtonAccept		= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Accept" );
	CKTDGUIButton* pButtonComplete		= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Complete" );
	CKTDGUIButton* pButtonRemind		= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Remind" );
	CKTDGUIButton* pButtonOngoing		= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Ongoing" );
	CKTDGUIButton* pButtonWaitOngoing	= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_WaitOngoing" );
	CKTDGUIButton* pButtonAdminComplete	= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Admin_Complete" );
	CKTDGUIButton* pButtonCompleteToolTip = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_CompleteToolTip" );
	m_pButtonSkipEpicQuest				= (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Skip_Epic_Quest" );

	pButtonGiveup->SetShowEnable( false, false );
	pButtonAccept->SetShowEnable( false, false );
	pButtonComplete->SetShowEnable( false, false );
	pButtonRemind->SetShowEnable( false, false );
	pButtonAdminComplete->SetShowEnable( false, false );
	pButtonOngoing->SetShowEnable( false, false );
	pButtonWaitOngoing->SetShowEnable( false, false );
	pButtonCompleteToolTip->SetShowEnable( false, false );
	if( NULL != m_pButtonSkipEpicQuest )
		m_pButtonSkipEpicQuest->SetShowEnable( false, false );

	// the selected quest, from whichever list has it ( the later list wins, as in June )
	QuestInfo kQuestInfo;
	const std::vector<QuestInfo>* apVec[5] = { &m_vecOnGoingQuestInfo, &m_vecEpicQuestInfo, &m_vecOnGoingEventQuestInfo, &m_vecOnGoingGuildQuestInfo, &m_vecOnGoingStoryQuestInfo };
	for( int iList = 0; iList < 5; ++iList )
	{
		for( UINT i = 0; i < apVec[iList]->size(); ++i )
		{
			if( (*apVec[iList])[i].m_iID == m_iSelectedQuestID )
				kQuestInfo = (*apVec[iList])[i];
		}
	}

	bool bGuildQuest = false;
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
	if( NULL != pQuestTemplet )
		bGuildQuest = pQuestTemplet->m_bIsGuild;

	const bool bDev = ( CX2User::XUAL_DEV == g_pData->GetMyUser()->GetAuthLevel() );
	const bool bCanSkip = CanSkipEpicQuest( kQuestInfo, m_iSelectedQuestID );

	if( true == bDev && kQuestInfo.m_eQuestState > QSS_COMPLETE && ( kQuestInfo.m_eQuestState < QSS_COMPLETE_STORY || QSS_CHAIN_WAIT == kQuestInfo.m_eQuestState ) )
	{
		pButtonAdminComplete->SetShowEnable( true, true );
		if( true == bCanSkip && NULL != m_pButtonSkipEpicQuest )
			m_pButtonSkipEpicQuest->SetShowEnable( true, true );
	}

	if( true == bGuildQuest )
		return;

	switch( kQuestInfo.m_eQuestState )
	{
	case QSS_COMPLETE_EPIC:
		{
			// the remind button replays a quest scene: enabled in a village / field when the quest has a Quest_Event_Scene_Info row ( Q5b )
			const std::map<int, CX2QuestManager::QuestEventSceneInfo>& mapScene = g_pData->GetQuestManager()->GetQuestEventSceneInfoMap();
			if( true == IsVillageOrBattleFieldState() && mapScene.end() != mapScene.find( m_iSelectedQuestID ) )
			{
				pButtonRemind->SetShowEnable( true, true );
				pButtonRemind->RequestFocus();
			}
			else
				pButtonRemind->SetShowEnable( true, false );
		}
		break;
	case QSS_COMPLETE:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_PVP_GAME:
				pButtonComplete->SetShowEnable( true, false );
				pButtonCompleteToolTip->SetShowEnable( true, true );
				break;
			default:
				pButtonComplete->SetShowEnable( true, true );
				pButtonComplete->RequestFocus();
				break;
			}
		}
		break;
	case QSS_WAIT_EPIC:
	case QSS_CHAIN_WAIT:
		pButtonWaitOngoing->SetShowEnable( true, false );
		if( true == bCanSkip && NULL != m_pButtonSkipEpicQuest )
			m_pButtonSkipEpicQuest->SetShowEnable( true, true );
		break;
	case QSS_FAILED:
	case QSS_ONGOING:
		{
			if( QTS_EPIC == kQuestInfo.m_eQuestType || QTS_EVENT == kQuestInfo.m_eQuestType || QTS_GUILD == kQuestInfo.m_eQuestType )
				pButtonOngoing->SetShowEnable( true, false );
			else
			{
				pButtonGiveup->SetShowEnable( true, true );
				pButtonGiveup->RequestFocus();
			}
			if( true == bDev )
				pButtonAdminComplete->SetShowEnable( true, true );
			if( true == bCanSkip && NULL != m_pButtonSkipEpicQuest )
				m_pButtonSkipEpicQuest->SetShowEnable( true, true );
		}
		break;
	case QSS_AVAILABLE:
		pButtonAccept->SetShowEnable( true, true );
		pButtonAccept->RequestFocus();
		break;
	case QSS_COMPLETE_STORY:
#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY
		// June 323ccbc0 case 6 ( @323cd3a7..323cd409, quest-system Q10 ): a story quest with a story ( 326ab210 -> 326ab090 ) shows the remind
		// button, enabled and focused in a village / field ( June's states 0xe..0xf / 0x15; 0xf is the guild agit, which March lacks ), shown
		// disabled and still focused elsewhere; no story = hidden. The button replays the story ( UQNCM_QUEST_REMIND, Phase Q10b ).
		if( NULL != CX2SlideShotScriptedManager::GetInstance()->GetStory( m_iSelectedQuestID ) )
		{
			if( true == IsVillageOrBattleFieldState() )
				pButtonRemind->SetShowEnable( true, true );
			else
				pButtonRemind->SetShowEnable( true, false );
			pButtonRemind->RequestFocus();
		}
		else
			pButtonRemind->SetShowEnable( false, false );
#else SERV_IRUHADEV_JUNE_QUEST_STORY
		// June 323ccbc0 case 6 ( @323cd3a7 ): the remind button needs the story-book singleton 326ab210 / 326ab090 ( Q10 ); until then it
		// stays hidden, as June has it for a quest with no book entry
#endif SERV_IRUHADEV_JUNE_QUEST_STORY
		break;
	}
#else
	CKTDGUIButton* pButtonGiveup = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Giveup" );
	CKTDGUIButton* pButtonAccept = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Accept" );
	CKTDGUIButton* pButtonComplete = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Complete" );
	CKTDGUIButton* pButtonRemind = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Remind" );
	CKTDGUIButton* pButtonOngoing = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Ongoing" );
	CKTDGUIButton* pButtonWaitOngoing = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_WaitOngoing" );

	CKTDGUIButton* pButtonAdminComplete = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_Admin_Complete" );

	CKTDGUIButton* pButtonCompleteToolTip = (CKTDGUIButton*) m_pDLGUIQuestDetail->GetControl( L"Button_CompleteToolTip" );



	pButtonGiveup->SetShowEnable(false, false);
	pButtonAccept->SetShowEnable(false, false);
	pButtonComplete->SetShowEnable(false, false);
	pButtonRemind->SetShowEnable(false, false);
	pButtonAdminComplete->SetShowEnable(false, false);
	pButtonOngoing->SetShowEnable(false, false);
	pButtonWaitOngoing->SetShowEnable(false, false);
	pButtonCompleteToolTip->SetShowEnable(false, false);

	QuestInfo kQuestInfo;

	for(UINT i=0; i< m_vecOnGoingQuestInfo.size(); i++)
	{
		if(m_vecOnGoingQuestInfo[i].m_iID == m_iSelectedQuestID)
		{
			kQuestInfo = m_vecOnGoingQuestInfo[i];
		}
	}
	for(UINT i=0; i< m_vecEpicQuestInfo.size(); i++)
	{
		if(m_vecEpicQuestInfo[i].m_iID == m_iSelectedQuestID)
		{
			kQuestInfo = m_vecEpicQuestInfo[i];
		}
	}
	for(UINT i=0; i< m_vecOnGoingEventQuestInfo.size(); i++)
	{
		if(m_vecOnGoingEventQuestInfo[i].m_iID == m_iSelectedQuestID)
		{
			kQuestInfo = m_vecOnGoingEventQuestInfo[i];
		}
	}
	
	switch(kQuestInfo.m_eQuestState)
	{
	case QSS_AVAILABLE:
		pButtonAccept->SetShowEnable(true, true);
		pButtonAccept->RequestFocus();
		break;
	case QSS_WAIT_EPIC:
		pButtonWaitOngoing->SetShowEnable(true, false);
		break;
	case QSS_ONGOING:
	case QSS_FAILED:
		{
			if(kQuestInfo.m_eQuestType == QTS_EPIC || QTS_EVENT == kQuestInfo.m_eQuestType)
			{
				pButtonOngoing->SetShowEnable(true, false);
			}
			else
			{
				pButtonGiveup->SetShowEnable(true, true);
				pButtonGiveup->RequestFocus();
			}
			if( CX2User::XUAL_DEV == g_pData->GetMyUser()->GetAuthLevel() )
			{
				pButtonAdminComplete->SetShowEnable(true, true);
			}
		}
		break;
	case QSS_COMPLETE_EPIC:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_BATTLE_FIELD:
				{
					pButtonRemind->SetShowEnable(true, true);
					pButtonRemind->RequestFocus();
				} break;
			default:
				{
					pButtonRemind->SetShowEnable(true, false);
				} break;
			}
		}
		break;
	case QSS_COMPLETE:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_PVP_GAME:
				{
					pButtonComplete->SetShowEnable(true, false);
					pButtonCompleteToolTip->SetShowEnable(true, true);
				} break;
			default:
				{
					pButtonComplete->SetShowEnable(true, true);
					pButtonComplete->RequestFocus();
				} break;
			}
		}
		break;
	}
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}


CX2UIQuestNew::QUEST_TYPE_SORT CX2UIQuestNew::ConvertToQuestTypeSort( int _eQuestType )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323d5650 / 323d0770 / 323ce6f0 switch on templet +0x8
	switch( _eQuestType )
	{
	case CX2QuestManager::QT_EVENT:							return QTS_EVENT;
	case CX2QuestManager::QT_CHANGE_JOB:					return QTS_CHANGE_JOB;
	case CX2QuestManager::QT_SKILL:							return QTS_SKILL;
	case CX2QuestManager::QT_PCBANG:						return QTS_PCBANG;
	case CX2QuestManager::QT_EPIC:							return QTS_EPIC;
	case CX2QuestManager::QT_GUILD:							return QTS_GUILD;
	case CX2QuestManager::QT_CHANGE_TRANSCENDENTAL_CLASS:	return QTS_TRANSCEND;
	case CX2QuestManager::QT_STORY:							return QTS_STORY;
	default:												return QTS_NORMAL;
	}
#else
	QUEST_TYPE_SORT eQuestTypeSort = QTS_NORMAL;

	switch( _eQuestType )
	{
	case CX2QuestManager::QT_NORMAL:	eQuestTypeSort = QTS_NORMAL; break;
	case CX2QuestManager::QT_SKILL:		eQuestTypeSort = QTS_SKILL; break;
	case CX2QuestManager::QT_CHANGE_JOB:eQuestTypeSort = QTS_CHANGE_JOB; break;
	case CX2QuestManager::QT_PCBANG:	eQuestTypeSort = QTS_PCBANG; break;
	case CX2QuestManager::QT_EVENT:		eQuestTypeSort = QTS_EVENT; break;
	//case CX2QuestManager::QT_CONTEST: eQuestTypeSort = QTS_CONTEST; break;
	case CX2QuestManager::QT_EPIC:		eQuestTypeSort = QTS_EPIC; break;
	}
	return eQuestTypeSort;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}
CX2UIQuestNew::REPEAT_TYPE_SORT CX2UIQuestNew::ConvertToRepeatTypeSort( int _eQuestRepeatType )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June: templet +0x34 ( NORMAL 0, DAY 1, REPEAT 2, WEEK 3, MONTH 4 ). QRT_CHAIN_REPEAT 5 is not mapped ( normal ).
	switch( _eQuestRepeatType )
	{
	case CX2QuestManager::QRT_DAY:		return RTS_DAY;
	case CX2QuestManager::QRT_REPEAT:	return RTS_REPEAT;
	case CX2QuestManager::QRT_WEEK:		return RTS_WEEK;
	case CX2QuestManager::QRT_MONTH:	return RTS_MONTH;
	default:							return RTS_NORMAL;
	}
#else
	REPEAT_TYPE_SORT eRepeatTypeSort = RTS_NORMAL;

	switch( _eQuestRepeatType )
	{
	case CX2QuestManager::QRT_REPEAT:	eRepeatTypeSort = RTS_REPEAT; break;
	case CX2QuestManager::QRT_NORMAL:	eRepeatTypeSort = RTS_NORMAL; break;
	case CX2QuestManager::QRT_DAY:		eRepeatTypeSort = RTS_DAY; break;
	}
	return eRepeatTypeSort;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}



void CX2UIQuestNew::UpdateAvailQuestInfo( CX2UnitManager::NPC_UNIT_ID eNPCID )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323d0770: the quests the NPC can give. Without an NPC the board ( NUI_BILLBOARD ) answers in a village / field, so the request
/// tab lists the available quests anywhere there.
	if( CX2UnitManager::NUI_NONE == eNPCID )
	{
		eNPCID = m_eNPCID;
		if( true == IsVillageOrBattleFieldState() && CX2UnitManager::NUI_NONE == eNPCID )
			eNPCID = CX2UnitManager::NUI_BILLBOARD;
	}

	CX2QuestManager* pQM = g_pData->GetQuestManager();
	vector<int> vecAvailQuestID;

	m_vecAvailQuestInfo.clear();
	pQM->GetAvailableQuest( eNPCID, vecAvailQuestID );

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	for( UINT i = 0; i < vecAvailQuestID.size(); ++i )
	{
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( vecAvailQuestID[i] );
		if( NULL == pT )
			continue;

		const int iClassTran = pT->m_Condition.m_iUnitClassTran;
		if( -1 != iClassTran && SelectUnitClassTran() != iClassTran )
			continue;

#ifdef	PC_BANG_QUEST
		if( true == pT->m_bIsPcBang && false == g_pData->GetMyUser()->GetIsPCRoom() )
			continue;
#endif	PC_BANG_QUEST

		if( true == pT->m_bIsTimeEvent )
			continue;

		// June 323d0770 @323d093f: the five event quests 208020..208060 are gated on the premium-buff manager ( g_pData +0x334 ). They are not
		// in June's FieldQuest, so the gate has nothing to hold here.

		if( false == pT->m_Condition.m_setUnitClass.empty() &&
			pT->m_Condition.m_setUnitClass.end() == pT->m_Condition.m_setUnitClass.find( pUnit->GetClass() ) )
			continue;

		QuestInfo kInfo;
		kInfo.m_iID				= vecAvailQuestID[i];
		kInfo.m_eQuestState		= QSS_AVAILABLE;
		kInfo.m_eQuestType		= ConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= ConvertToRepeatTypeSort( pT->m_eRepeatType );
		kInfo.m_iConditionLevel	= pT->m_Condition.m_iLevel;
		m_vecAvailQuestInfo.push_back( kInfo );
	}

	std::sort( m_vecAvailQuestInfo.begin(), m_vecAvailQuestInfo.end(), QuestInfoSort() );
	std::sort( m_vecAvailQuestInfo.begin(), m_vecAvailQuestInfo.end(), AvailQuestInfoSort() );
#else
	if( CX2UnitManager::NUI_NONE == eNPCID )
	{
#ifdef MODIFY_ACCEPT_QUEST // 기본 수락 NPC를 게시판으로 설정
		switch(g_pMain->GetNowStateID())
		{
		case CX2Main::XS_VILLAGE_MAP:
		case CX2Main::XS_BATTLE_FIELD:
			{
				if( CX2UnitManager::NUI_NONE == m_eNPCID )
					eNPCID = CX2UnitManager::NUI_BILLBOARD;
				else
					eNPCID = m_eNPCID;
			} break;
		default:
			{
				eNPCID = m_eNPCID;
			} break;
		}
#else
		eNPCID = m_eNPCID;
#endif // MODIFY_ACCEPT_QUEST

	}

	vector<int> vecAvailQuestID;

	m_vecAvailQuestInfo.clear();
	g_pData->GetQuestManager()->GetAvailableQuest( eNPCID, vecAvailQuestID );


	for(UINT i=0; i<vecAvailQuestID.size(); i++)
	{
		QuestInfo kQuestInfo;

		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( vecAvailQuestID[i] );


		//{{ oasis907 : 김상윤 [2010.7.7] // 피시방, 일일 퀘스트 리스트 제거 여기서 함.

		//{{ kimhc // 2010.2.1 //	PC방이 아닌곳에서는 받지 않은 PC방퀘스트 목록에서 보이지 않도록
#ifdef	PC_BANG_QUEST
		if ( pQuestTemplet->m_bIsPcBang == true && g_pData->GetMyUser()->GetIsPCRoom() == false)
			continue;
#endif	PC_BANG_QUEST
		//}} kimhc // 2010.2.1 //	PC방이 아닌곳에서는 받지 않은 PC방퀘스트 목록에서 보이지 않도록

		//{{ kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현
#ifdef	SERV_DAY_QUEST
		if ( pQuestTemplet->m_bIsTimeEvent == true )
			continue;
#endif	SERV_DAY_QUEST
		//}}

#ifdef SERV_RECRUIT_EVENT_QUEST_FOR_NEW_USER
		if( CX2QuestManager::IsNewUserOnlyQuest( vecAvailQuestID[i] ) == true &&
			g_pInstanceData->IsRecruit() == false )
			continue;
#endif SERV_RECRUIT_EVENT_QUEST_FOR_NEW_USER

		//}} oasis907 : 김상윤 [2010.7.7] //

		// QUEST_ID
		kQuestInfo.m_iID = vecAvailQuestID[i];

		// QUEST_STATE
		kQuestInfo.m_eQuestState = QSS_AVAILABLE;

		// QUEST_TYPE
		kQuestInfo.m_eQuestType = ConvertToQuestTypeSort(pQuestTemplet->m_eQuestType);		
		
		// REPEAT_TYPE
		kQuestInfo.m_eRepeatType = ConvertToRepeatTypeSort(pQuestTemplet->m_eRepeatType);

		// CONDITION LEVEL
		kQuestInfo.m_iConditionLevel = pQuestTemplet->m_Condition.m_iLevel;

		m_vecAvailQuestInfo.push_back(kQuestInfo);

	}

	std::sort(m_vecAvailQuestInfo.begin(), m_vecAvailQuestInfo.end(), QuestInfoSort());


// 	int iLevelDiff = g_pData->GetSelectUnitLevel() - pQuestTemplet->m_iFairLevel;
// 	if( iLevelDiff > 5 )
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

bool CX2UIQuestNew::GetUnitQuest( std::vector<int>& vecQuestID )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323cf570: the quests the unit is doing, as ids. A guild quest is the guild's, not the unit's. An event quest has to fit the unit's
/// type and class; a change-job quest is listed once.
	vecQuestID.clear();

	CX2QuestManager* pQM = g_pData->GetQuestManager();
	const map<int,CX2QuestManager::QuestInst*> mapUnitQuest = pQM->GetUnitQuestMap();
	if( true == mapUnitQuest.empty() )
		return false;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	map<int,CX2QuestManager::QuestInst*>::const_iterator it;
	for( it = mapUnitQuest.begin(); it != mapUnitQuest.end(); ++it )
	{
		const int iQuestID = it->first;
		const CX2QuestManager::QuestInst* pInst = it->second;
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( iQuestID );
		if( NULL == pInst || NULL == pT )
			continue;

		if( false == pT->m_bIsGuild && pInst->m_OwnorUnitUID != pUnit->GetUID() )
			continue;

		const int iClassTran = pT->m_Condition.m_iUnitClassTran;
		if( -1 != iClassTran && SelectUnitClassTran() != iClassTran && CX2QuestManager::QT_EVENT != pT->m_eQuestType )
			continue;

		if( CX2QuestManager::QT_EVENT == pT->m_eQuestType )
		{
			if( CX2Unit::UT_NONE != pT->m_Condition.m_eUnitType && pUnit->GetType() != pT->m_Condition.m_eUnitType )
				continue;

			if( false == pT->m_Condition.m_setUnitClass.empty() )
			{
				bool bFit = false;
				std::set< CX2Unit::UNIT_CLASS >::const_iterator sit;
				for( sit = pT->m_Condition.m_setUnitClass.begin(); sit != pT->m_Condition.m_setUnitClass.end(); ++sit )
				{
					if( CX2Unit::UC_NONE == *sit || pUnit->GetClass() == *sit )
					{
						bFit = true;
						break;
					}
				}
				if( false == bFit )
					continue;
			}
			vecQuestID.push_back( iQuestID );
		}
		else
		{
			if( CX2QuestManager::QT_CHANGE_JOB == pT->m_eQuestType &&
				vecQuestID.end() != std::find( vecQuestID.begin(), vecQuestID.end(), iQuestID ) )
				continue;
			vecQuestID.push_back( iQuestID );
		}
	}
	return true;
#else

	vecQuestID.clear();

	// 퀘 리스트 받아오고
	const map<int,CX2QuestManager::QuestInst*> mapUnitQuest = g_pData->GetQuestManager()->GetUnitQuestMap();

	if ( mapUnitQuest.empty() == true )
		return false;

	map<int, CX2QuestManager::QuestInst*>::const_iterator i;

	for ( i = mapUnitQuest.begin(); i != mapUnitQuest.end(); i++ )
	{
		int questID = i->first;

		CX2QuestManager::QuestInst* pQuestInst = i->second;
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( questID );

		if ( pQuestInst == NULL || 
			pQuestInst->m_OwnorUnitUID != g_pData->GetMyUser()->GetSelectUnit()->GetUID() ||
			pQuestTemplet == NULL )
			continue;
		
#ifdef SHOW_ONLY_MY_EVENT_QUEST
		switch( pQuestTemplet->m_eQuestType )
		{
		// 이벤트 타입은 유닛타입 검사하여, 자기 유닛의 퀘스트일 경우만 받기
		case CX2QuestManager::QT_EVENT:
			{
				if( CX2Unit::UT_NONE == pQuestTemplet->m_Condition.m_eUnitType ||
					g_pData->GetSelectUnitType() == pQuestTemplet->m_Condition.m_eUnitType )
					vecQuestID.push_back(questID);		
			} break;
		default:
			vecQuestID.push_back(questID);
			break;
		}
#else
		vecQuestID.push_back(questID);
#endif // SHOW_ONLY_MY_EVENT_QUEST

	}
	return true;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

bool  CX2UIQuestNew::GetCompleteEpicQuest( std::vector<int>& vecQuestID )
{
	vecQuestID.clear();

	const map<int,int> mapCompleteQuest = g_pData->GetQuestManager()->GetMapCompleteQuest();
	
	if ( mapCompleteQuest.empty() == true )
		return false;
	
	map<int, int>::const_iterator i;

	for ( i = mapCompleteQuest.begin(); i != mapCompleteQuest.end(); i++ )
	{
		int iQuestID = i->first;

		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( iQuestID );

		if ( pQuestTemplet == NULL )
			continue;

		if(pQuestTemplet->m_eQuestType == CX2QuestManager::QT_EPIC)
		{
			vecQuestID.push_back(iQuestID);
		}
	}
	return true;
}





void CX2UIQuestNew::EpicQuestSort( std::vector<QuestInfo>& vecEpicQuestInfo)
{
	
	std::sort(vecEpicQuestInfo.begin(), vecEpicQuestInfo.end(), EpicQuestInfoSort());

	return;
}


void CX2UIQuestNew::UpdateOngoingQuestInfo()
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323d5650: build every tab's list from the unit's quests. A tab gets its own vector ( epic, event, guild, story ); the rest go to the
/// ongoing list; a story quest in progress is in the story and the ongoing list both.
	CX2QuestManager* pQM = g_pData->GetQuestManager();

	m_vecOnGoingQuestInfo.clear();
	m_vecEpicQuestInfo.clear();
	m_vecQuickQuest.clear();
	m_vecOnGoingEventQuestInfo.clear();
	m_vecOnGoingGuildQuestInfo.clear();
	m_vecOnGoingStoryQuestInfo.clear();

	vector<int> vecOnGoingQuestID;
	vector<int> vecCompleteEpicQuestID;
	vector<int> vecCompleteStoryQuestID;

	GetUnitQuest( vecOnGoingQuestID );
	GetCompleteQuestOfType( vecCompleteEpicQuestID, CX2QuestManager::QT_EPIC );
	GetCompleteQuestOfType( vecCompleteStoryQuestID, CX2QuestManager::QT_STORY );
	AddFirstStoryQuests( vecOnGoingQuestID, vecCompleteStoryQuestID );
	AddNextStoryQuests( vecOnGoingQuestID, vecCompleteStoryQuestID );

	m_vecQuickQuest.reserve( vecOnGoingQuestID.size() );

	for( UINT i = 0; i < vecOnGoingQuestID.size(); ++i )
	{
		const int iQuestID = vecOnGoingQuestID[i];
		if( false == GetIsValidDayTheWeekQuest( iQuestID ) )
			continue;

		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( iQuestID );
		const CX2QuestManager::QuestInst* pInst = pQM->GetUnitQuest( iQuestID );
		if( NULL == pT )
			continue;

		QuestInfo kInfo;
		kInfo.m_iID				= iQuestID;
		kInfo.m_eQuestType		= ConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= ConvertToRepeatTypeSort( pT->m_eRepeatType );
		kInfo.m_iConditionLevel	= pT->m_Condition.m_iLevel;
		kInfo.m_iPlayLevel		= pT->m_iPlayLevel;
		kInfo.m_iFairLevel		= pT->m_iFairLevel;
		kInfo.m_eQuestState		= QSS_AVAILABLE;

		const bool bInstComplete = ( NULL != pInst && true == pInst->IsComplete() );

		if( QTS_EPIC == kInfo.m_eQuestType )
		{
			kInfo.m_iEpicGroupID = pT->m_iEpisodeGroupID;
			if( true == pQM->GetUnitCompleteQuest( iQuestID ) )
				kInfo.m_eQuestState = QSS_COMPLETE_EPIC;
			else if( g_pData->GetSelectUnitLevel() < kInfo.m_iPlayLevel )
				kInfo.m_eQuestState = QSS_WAIT_EPIC;
			else if( NULL != pInst )
				kInfo.m_eQuestState = ( true == bInstComplete ) ? QSS_COMPLETE : QSS_ONGOING;
		}
		else if( QTS_STORY == kInfo.m_eQuestType )
		{
			kInfo.m_iEpicGroupID = pT->m_iEpisodeGroupID;
			if( true == pQM->GetUnitCompleteQuest( iQuestID ) )
				kInfo.m_eQuestState = QSS_COMPLETE_STORY;
			else if( NULL == pInst )
				kInfo.m_eQuestState = QSS_AVAILABLE;
			else
				kInfo.m_eQuestState = ( true == bInstComplete ) ? QSS_COMPLETE : QSS_ONGOING;

			// a story quest that is not in progress waits for its chain ( 32322ad0 )
			if( NULL == pQM->GetUnitQuest( iQuestID ) )
			{
#ifdef SERV_IRUHADEV_JUNE_QUEST_RUNTIME
				const bool bChainStartable = pQM->GetChainQuestStartable( pT->m_iChainID );
#else SERV_IRUHADEV_JUNE_QUEST_RUNTIME
				const bool bChainStartable = false;		// June: a chain with no flag reads false
#endif SERV_IRUHADEV_JUNE_QUEST_RUNTIME
				if( false == bChainStartable )
					kInfo.m_eQuestState = QSS_CHAIN_WAIT;
			}
		}
		else
		{
			if( NULL == pInst )
				kInfo.m_eQuestState = QSS_AVAILABLE;
			else
				kInfo.m_eQuestState = ( true == bInstComplete ) ? QSS_COMPLETE : QSS_ONGOING;
		}

		// the tab the window opens on ( 323d5bd4 / 323d5c43 )
		if( QSS_COMPLETE == kInfo.m_eQuestState )
		{
			if( QTS_EPIC == kInfo.m_eQuestType )		{ m_bCompleteQuestInEpicTab = true;		m_iCompleteEpicCategory = kInfo.m_iEpicGroupID; }
			else if( QTS_STORY == kInfo.m_eQuestType )	{ m_bCompleteQuestInStoryTab = true;	m_iCompleteStoryCategory = kInfo.m_iEpicGroupID; }
			else										m_bCompleteQuestInNormalTab = true;
		}
		else if( QSS_ONGOING == kInfo.m_eQuestState )
		{
			if( QTS_EPIC == kInfo.m_eQuestType )		{ m_bOngoingQuestInEpicTab = true;		m_iOngoingEpicCategory = kInfo.m_iEpicGroupID; }
			else if( QTS_STORY == kInfo.m_eQuestType )	{ m_bOngoingQuestInStoryTab = true;		m_iOngoingStoryCategory = kInfo.m_iEpicGroupID; }
			else										m_bOngoingQuestInNormalTab = true;
		}

		// which list ( 323d5c90 - 323d5d04 )
		bool bStoryTimeEvent = false;
		switch( kInfo.m_eQuestType )
		{
		case QTS_EPIC:		m_vecEpicQuestInfo.push_back( kInfo );				break;
		case QTS_EVENT:		m_vecOnGoingEventQuestInfo.push_back( kInfo );		break;
		case QTS_GUILD:		m_vecOnGoingGuildQuestInfo.push_back( kInfo );		break;
		case QTS_STORY:
			if( true == pT->m_bIsTimeEvent )
				bStoryTimeEvent = true;
			else
			{
				m_vecOnGoingStoryQuestInfo.push_back( kInfo );
				if( QSS_ONGOING == kInfo.m_eQuestState )
					m_vecOnGoingQuestInfo.push_back( kInfo );
			}
			break;
		default:
			if( QUM_GUILD_QUEST != m_eNowQuestUIMode )
				m_vecOnGoingQuestInfo.push_back( kInfo );
			break;
		}

		if( false == bStoryTimeEvent && NULL != g_pData && kInfo.m_iPlayLevel <= g_pData->GetSelectUnitLevel() &&
			QSS_CHAIN_WAIT != kInfo.m_eQuestState && QSS_AVAILABLE != kInfo.m_eQuestState )
			m_vecQuickQuest.push_back( kInfo );
	}

	// the epic groups that have a quest in progress
	m_mapEpicQuestProgressInfo.clear();
	if( NULL != g_pData && NULL != pQM )
	{
		for( UINT i = 0; i < m_vecEpicQuestInfo.size(); ++i )
		{
			const CX2QuestManager::QuestInst* pInst = pQM->GetUnitQuest( m_vecEpicQuestInfo[i].m_iID );
			if( NULL != pInst && false == pInst->IsComplete() )
				m_mapEpicQuestProgressInfo.insert( std::make_pair( m_vecEpicQuestInfo[i].m_iEpicGroupID, true ) );
		}
	}

	// the completed epic quests
	for( UINT i = 0; i < vecCompleteEpicQuestID.size(); ++i )
	{
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( vecCompleteEpicQuestID[i] );
		if( NULL == pT )
			continue;

		QuestInfo kInfo;
		kInfo.m_iID				= vecCompleteEpicQuestID[i];
		kInfo.m_eQuestType		= ConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= ConvertToRepeatTypeSort( pT->m_eRepeatType );
		kInfo.m_iConditionLevel	= pT->m_Condition.m_iLevel;
		kInfo.m_iPlayLevel		= pT->m_iPlayLevel;
		kInfo.m_iFairLevel		= pT->m_iFairLevel;
		kInfo.m_eQuestState		= QSS_COMPLETE_EPIC;
		kInfo.m_iEpicGroupID	= pT->m_iEpisodeGroupID;
		m_vecEpicQuestInfo.push_back( kInfo );
	}

	// the story groups: started ( 1 ) or not ( 0 ), from the quests listed so far
	m_mapStoryQuestProgressInfo.clear();
	for( UINT i = 0; i < m_vecOnGoingStoryQuestInfo.size(); ++i )
	{
		const CX2QuestManager::QuestInst* pInst = pQM->GetUnitQuest( m_vecOnGoingStoryQuestInfo[i].m_iID );
		if( NULL == pInst )
			m_mapStoryQuestProgressInfo.insert( std::make_pair( m_vecOnGoingStoryQuestInfo[i].m_iEpicGroupID, 0 ) );
		else if( false == pInst->IsComplete() )
			m_mapStoryQuestProgressInfo.insert( std::make_pair( m_vecOnGoingStoryQuestInfo[i].m_iEpicGroupID, 1 ) );
	}

	// the completed story quests. A completed quest with no next quest, or the last of its group, ends the group's progress; any other
	// keeps it going.
	const std::map< int, std::set<int> > mapStoryGroup = pQM->GetStoryQuestMap();
	for( UINT i = 0; i < vecCompleteStoryQuestID.size(); ++i )
	{
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( vecCompleteStoryQuestID[i] );
		if( NULL == pT )
			continue;

		QuestInfo kInfo;
		kInfo.m_iID				= vecCompleteStoryQuestID[i];
		kInfo.m_eQuestType		= ConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= ConvertToRepeatTypeSort( pT->m_eRepeatType );
		kInfo.m_iConditionLevel	= pT->m_Condition.m_iLevel;
		kInfo.m_iPlayLevel		= pT->m_iPlayLevel;
		kInfo.m_iFairLevel		= pT->m_iFairLevel;
		kInfo.m_eQuestState		= QSS_COMPLETE_STORY;
		kInfo.m_iEpicGroupID	= pT->m_iEpisodeGroupID;

		if( true == pT->m_vecAfterQuestID.empty() )
			m_mapStoryQuestProgressInfo.erase( kInfo.m_iEpicGroupID );
		else
		{
			std::map< int, std::set<int> >::const_iterator git = mapStoryGroup.find( kInfo.m_iEpicGroupID );
			if( git != mapStoryGroup.end() && false == git->second.empty() )
			{
				if( kInfo.m_iID == *git->second.rbegin() )
					m_mapStoryQuestProgressInfo.erase( kInfo.m_iEpicGroupID );
				else
					m_mapStoryQuestProgressInfo[ kInfo.m_iEpicGroupID ] = 1;
			}
		}
		m_vecOnGoingStoryQuestInfo.push_back( kInfo );
	}

	std::sort( m_vecOnGoingQuestInfo.begin(), m_vecOnGoingQuestInfo.end(), QuestInfoSort() );
	std::sort( m_vecOnGoingEventQuestInfo.begin(), m_vecOnGoingEventQuestInfo.end(), QuestInfoSort() );
	std::sort( m_vecOnGoingGuildQuestInfo.begin(), m_vecOnGoingGuildQuestInfo.end(), QuestInfoSort() );
	std::sort( m_vecQuickQuest.begin(), m_vecQuickQuest.end(), QuickQuestInfoSort() );
	std::sort( m_vecEpicQuestInfo.begin(), m_vecEpicQuestInfo.end(), EpicQuestInfoSort() );
	if( -1 == m_iOpenEpicCategory )
		SetOpenEpicCategory();
	std::sort( m_vecOnGoingStoryQuestInfo.begin(), m_vecOnGoingStoryQuestInfo.end(), EpicQuestInfoSort() );
	if( -1 == m_iOpenStoryCategory )
		SetOpenStoryCategory();
#else

	vector<int> vecOnGoingQuestID;
	vector<int> vecCompleteEpicQuestID;

	m_vecOnGoingQuestInfo.clear();
	m_vecEpicQuestInfo.clear();
	m_vecQuickQuest.clear();
#ifdef REFORM_QUEST
	m_vecOnGoingEventQuestInfo.clear();
#endif //REFORM_QUEST

	GetUnitQuest(vecOnGoingQuestID);
	GetCompleteEpicQuest(vecCompleteEpicQuestID);

	m_vecQuickQuest.reserve( vecOnGoingQuestID.size() );

	for(UINT i=0; i<vecOnGoingQuestID.size(); i++)
	{
#ifdef DAY_OF_THE_WEEK_QUEST
		// 현재 요일에 수행할 수 없는 퀘스트라면 등록하지 않기.
		if( false == GetIsValidDayTheWeekQuest( vecOnGoingQuestID[i] ) )
			continue;
#endif // DAY_OF_THE_WEEK_QUEST

		QuestInfo kQuestInfo;

		const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( vecOnGoingQuestID[i] );
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( vecOnGoingQuestID[i] );

		// QUEST_ID
		kQuestInfo.m_iID = vecOnGoingQuestID[i];
		
		// QUEST_TYPE
		kQuestInfo.m_eQuestType = ConvertToQuestTypeSort(pQuestTemplet->m_eQuestType);
		
		// REPEAT_TYPE
		kQuestInfo.m_eRepeatType = ConvertToRepeatTypeSort(pQuestTemplet->m_eRepeatType);
		
		// CONDITION LEVEL
		kQuestInfo.m_iConditionLevel = pQuestTemplet->m_Condition.m_iLevel;

		// FAIR LEVEL
		kQuestInfo.m_iFairLevel = pQuestTemplet->m_iFairLevel;

		kQuestInfo.m_iAfterQuestID = pQuestTemplet->m_iAfterQuestID;
		kQuestInfo.m_iPlayLevel = pQuestTemplet->m_iPlayLevel;

		if(kQuestInfo.m_eQuestType == QTS_EPIC)
		{
			kQuestInfo.m_iEpicGroupID = pQuestTemplet->m_iEpisodeGroupID;
			if(g_pData->GetQuestManager()->GetUnitCompleteQuest(kQuestInfo.m_iID) == true)
			{
				kQuestInfo.m_eQuestState = QSS_COMPLETE_EPIC;
			}
			else if(g_pData->GetSelectUnitLevel() < kQuestInfo.m_iPlayLevel)
			{
				kQuestInfo.m_eQuestState = QSS_WAIT_EPIC;
			}
			else if(pQuestInst != NULL) // OngoingQuest 중에 QuestInst가 NULL일리는 없음
			{
				if( true == pQuestInst->IsComplete() )
				{
					kQuestInfo.m_eQuestState = QSS_COMPLETE;
				}
				// FAIL 추가해야함
				else
				{
					kQuestInfo.m_eQuestState = QSS_ONGOING;
				}
			}
			// 에픽 퀘스트는 QSS_AVAILABLE이 없음
		}
		else
		{
			if(pQuestInst != NULL) // OngoingQuest 중에 QuestInst가 NULL일리는 없음
			{
				if( true == pQuestInst->IsComplete() )
				{
					kQuestInfo.m_eQuestState = QSS_COMPLETE;
				}
				// FAIL 추가해야함
				else
				{
					kQuestInfo.m_eQuestState = QSS_ONGOING;
				}
			}
			else
			{
				kQuestInfo.m_eQuestState = QSS_AVAILABLE;
			}
		}
		
		// 우선 순위 인자 설정		
		if(kQuestInfo.m_eQuestState == QSS_COMPLETE)
		{
			if(kQuestInfo.m_eQuestType == QTS_EPIC)
			{
				m_bCompleteQuestInEpicTab = true;
				m_iCompleteEpicCategory = kQuestInfo.m_iEpicGroupID;
			}
			else
			{
				m_bCompleteQuestInNormalTab = true;
			}
		}
		else if(kQuestInfo.m_eQuestState == QSS_ONGOING)
		{
			if(kQuestInfo.m_eQuestType == QTS_EPIC)
			{
				m_bOngoingQuestInEpicTab = true;
				m_iOngoingEpicCategory = kQuestInfo.m_iEpicGroupID;
			}
			else
			{
				m_bOngoingQuestInNormalTab = true;
			}
		}	


		if(kQuestInfo.m_eQuestType == QTS_EPIC)
		{
			m_vecEpicQuestInfo.push_back(kQuestInfo);
		}
#ifdef REFORM_QUEST
		else if ( kQuestInfo.m_eQuestType == QTS_EVENT )
		{
			m_vecOnGoingEventQuestInfo.push_back( kQuestInfo );
		}
#endif //REFORM_QUEST
		else
		{
			m_vecOnGoingQuestInfo.push_back(kQuestInfo);
		}

		if( NULL != g_pData && kQuestInfo.m_iPlayLevel <= g_pData->GetSelectUnitLevel() )
			m_vecQuickQuest.push_back(kQuestInfo);
	}

#ifdef REFORM_QUEST
	m_mapEpicQuestProgressInfo.clear();

	if( NULL != g_pData && NULL != g_pData->GetQuestManager() )
	{	//에픽퀘스트의 카테고리 별 완료 여부 기록
		bool bIsProgress = false;
		int iEpicGroupID = -1;
		for(UINT i=0; i< m_vecEpicQuestInfo.size(); i++)
		{
			//그룹아이디가 바뀌면 상태 초기화
			if( iEpicGroupID != m_vecEpicQuestInfo[i].m_iEpicGroupID )
			{
				iEpicGroupID = m_vecEpicQuestInfo[i].m_iEpicGroupID;
				bIsProgress = false;
			}

			const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( m_vecEpicQuestInfo[i].m_iID );
			if( NULL != pQuestInst )
			{//완료되지 않은 퀘스트가 있으면 진행중 상태로 기록
				if( false == pQuestInst->IsComplete() )
				{
					bIsProgress = true;
					m_mapEpicQuestProgressInfo.insert( std::make_pair(iEpicGroupID, bIsProgress));
				}
			}
		}
	}
#endif //REFORM_QUEST

	for(UINT i=0; i<vecCompleteEpicQuestID.size(); i++)
	{
		QuestInfo kQuestInfo;

		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( vecCompleteEpicQuestID[i] );

		// QUEST_ID
		kQuestInfo.m_iID = vecCompleteEpicQuestID[i];

		// QUEST_TYPE
		kQuestInfo.m_eQuestType = ConvertToQuestTypeSort(pQuestTemplet->m_eQuestType);		

		// REPEAT_TYPE
		kQuestInfo.m_eRepeatType = ConvertToRepeatTypeSort(pQuestTemplet->m_eRepeatType);

		// CONDITION LEVEL
		kQuestInfo.m_iConditionLevel = pQuestTemplet->m_Condition.m_iLevel;
		
		kQuestInfo.m_eQuestState = QSS_COMPLETE_EPIC;


		kQuestInfo.m_iEpicGroupID = pQuestTemplet->m_iEpisodeGroupID;
		kQuestInfo.m_iAfterQuestID = pQuestTemplet->m_iAfterQuestID;

		m_vecEpicQuestInfo.push_back(kQuestInfo);
	}


	
	std::sort(m_vecOnGoingQuestInfo.begin(), m_vecOnGoingQuestInfo.end(), QuestInfoSort());
#ifdef REFORM_QUEST
	std::sort(m_vecOnGoingEventQuestInfo.begin(), m_vecOnGoingEventQuestInfo.end(), QuestInfoSort());
#endif //REFORM_QUEST


#ifdef MODIFY_QUEST_UI
	BOOST_FOREACH( QuestInfo kQustInfo, m_vecQuickQuest )
	{
		kQustInfo.m_eQuestState = ConvertToStateTypeSort( kQustInfo.m_eQuestState ) ;
	}
#endif //MODIFY_QUEST_UI
	std::sort(m_vecQuickQuest.begin(), m_vecQuickQuest.end(), QuickQuestInfoSort());
	EpicQuestSort(m_vecEpicQuestInfo);
#ifdef REFORM_QUEST
	if( -1 == m_iOpenEpicCategory )
	{
		SetOpenEpicCategory();
	}
#endif //REFORM_QUEST
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}


int CX2UIQuestNew::GetQuestPictureNumber(QUEST_TYPE_SORT _eQuestTypeSort, REPEAT_TYPE_SORT _eRepeatTypeSort)
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323c86f0: the list row's type picture ( Static_QuestList pictures of DLG_UI_NEW_Quest_List ).
	switch( _eQuestTypeSort )
	{
	case QTS_EPIC:			return 6;
	case QTS_CONTEST:		return 5;
	case QTS_EVENT:			return 4;
	case QTS_PCBANG:		return 3;
	case QTS_CHANGE_JOB:	return 2;
	case QTS_SKILL:			return 1;
	case QTS_NORMAL:
		switch( _eRepeatTypeSort )
		{
		case RTS_NORMAL:	return 0;
		case RTS_REPEAT:	return 7;
		case RTS_DAY:		return 8;
		case RTS_WEEK:		return 15;
		case RTS_MONTH:		return 22;
		}
		return 0;
	case QTS_GUILD:			return 20;
	case QTS_STORY:			return 19;
	case QTS_TRANSCEND:		return 21;
	}
	return 0;
#else
	int iRet = 0;
	switch(_eQuestTypeSort)
	{
	case QTS_NORMAL:
		{
			switch ( _eRepeatTypeSort )
			{
			case RTS_NORMAL:	iRet = 0; break;
			case RTS_REPEAT:	iRet = 7; break;
			case RTS_DAY:		iRet = 8; break;
			}
		}
		break;
	case QTS_SKILL:			iRet = 1; break;
	case QTS_CHANGE_JOB:	iRet = 2; break;
	case QTS_PCBANG:		iRet = 3; break;
	case QTS_EVENT:			iRet = 4; break;
	case QTS_CONTEST:		iRet = 5; break;
	case QTS_EPIC:			iRet = 6; break;

	default: break;
	}

	return iRet;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

int CX2UIQuestNew::GetQuestPictureNumber(QUEST_STATE_SORT _eQuestStateSort)
{
	int iRet = 0;
	switch(_eQuestStateSort)
	{
	case QSS_COMPLETE_EPIC: iRet = 9; break; // 완료 보고된 에픽
	case QSS_COMPLETE:	iRet = 9; break; // 완료
	case QSS_ONGOING:	iRet = 10; break; // 진행중
	case QSS_AVAILABLE: iRet = 11; break; // 나중에 New로 바꾸자
	case QSS_FAILED:	iRet = 12; break; // 진행중
	default: break;
	}

	return iRet;
}

void CX2UIQuestNew::SetUIQuestSlotInfoToEpicCategory( UINT iQuestSlotInfoIndex,  QuestInfo& QuestInfo )
{

	return;
}

void CX2UIQuestNew::SetUIQuestSlotInfoToCategory( UINT iQuestSlotInfoIndex,  QuestInfo& QuestInfo )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323d80d0: a category row. A group row ( epic or story ) shows the group title and, from the two progress maps, a "done" or a
/// "going" mark; the request tab's two rows ( 0 available, 1 in progress ) show a fixed title and the count.
	UINT i = iQuestSlotInfoIndex;
	UIQuestSlotInfo& kSlot = m_vecUIQuestSlotInfo[i];

	wstring wstrTextureName = L"DLG_UI_BUTTON01.tga";
	wstring wstrTextureName2 = L"DLG_UI_Button16.tga";

	wstring wstrTextureOpenKey = L"list_down";
	wstring wstrTextureCloseKey = L"list_normal";
	wstring wstrTextureOverKey = L"list_over";
	wstring wstrTextureOverKey2 = L"list_down_over";

	// the open / closed look of the row's button
	bool bOpen = false;

	if( -1 != QuestInfo.m_iEpicGroupID )
	{
		kSlot.m_QuestID = QuestInfo.m_iEpicGroupID;

		if( NULL != kSlot.m_pButtonCategory )
			kSlot.m_pButtonCategory->SetShowEnable( true, true );
		if( NULL != kSlot.m_pStaticCategory )
			kSlot.m_pStaticCategory->SetShowEnable( true, true );

		wstring wstrGroupTitle = g_pData->GetQuestManager()->GetEpicGroupTitle( QuestInfo.m_iEpicGroupID );
		if( NULL != kSlot.m_pStaticCategory )
		{
			kSlot.m_pStaticCategory->GetString( 0 )->msg = wstrGroupTitle;
			kSlot.m_pStaticCategory->GetString( 1 )->msg = L"";
		}

		kSlot.m_bIsCategory = true;

		bOpen = ( m_iOpenEpicCategory == QuestInfo.m_iEpicGroupID || m_iOpenStoryCategory == QuestInfo.m_iEpicGroupID );

		// the two marks: picture 0 is "done", picture 1 is "going"
		if( NULL != kSlot.m_pStaticCategory )
		{
			bool bFound = false;
			std::map<int, bool>::iterator mit = m_mapEpicQuestProgressInfo.find( QuestInfo.m_iEpicGroupID );
			if( mit != m_mapEpicQuestProgressInfo.end() )
			{
				bFound = true;
				kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( false == mit->second );
				kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( true == mit->second );
			}

			std::map<int, int>::iterator sit = m_mapStoryQuestProgressInfo.find( QuestInfo.m_iEpicGroupID );
			if( sit == m_mapStoryQuestProgressInfo.end() )
			{
				if( false == bFound )
				{
					kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( true );
					kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( false );
				}
			}
			else if( 1 == sit->second )
			{
				kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( false );
				kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( true );
			}
			else if( 2 == sit->second )
			{
				kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( true );
				kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( false );
			}
			else
			{
				kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( false );
				kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( false );
			}
		}
	}
	else
	{
		if( NULL != kSlot.m_pStaticCategory )
		{
			kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( false );
			kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( false );
		}

		if( 0 == QuestInfo.m_iID )
		{
			kSlot.m_QuestID = 0;

			if( NULL != kSlot.m_pButtonCategory )
				kSlot.m_pButtonCategory->SetShowEnable( true, true );
			if( NULL != kSlot.m_pStaticCategory )
			{
				kSlot.m_pStaticCategory->SetShowEnable( true, true );
				kSlot.m_pStaticCategory->GetString( 0 )->msg = GET_STRING( STR_ID_9618 );
				kSlot.m_pStaticCategory->GetString( 1 )->msg = L"";
			}

			kSlot.m_bIsCategory = true;
			bOpen = m_bIsOpenAvailCategory;
		}
		else if( 1 == QuestInfo.m_iID )
		{
			kSlot.m_QuestID = 1;

			if( NULL != kSlot.m_pButtonCategory )
				kSlot.m_pButtonCategory->SetShowEnable( true, true );
			if( NULL != kSlot.m_pStaticCategory )
			{
				kSlot.m_pStaticCategory->SetShowEnable( true, true );
				kSlot.m_pStaticCategory->GetString( 0 )->msg = GET_STRING( STR_ID_9619 );
			}

			WCHAR buff[64] = {0,};
			if( QUM_EVENT_QUEST != m_eNowQuestUIMode && QUM_GUILD_QUEST != m_eNowQuestUIMode )
				StringCchPrintf( buff, 64, L"%d/%d", m_vecOnGoingQuestInfo.size(), MY_QUEST_LIST_MAX_SIZE );

			if( NULL != kSlot.m_pStaticCategory )
				kSlot.m_pStaticCategory->GetString( 1 )->msg = buff;
			kSlot.m_bIsCategory = true;

			bOpen = ( true == m_bIsOpenOngoingCategory && QUM_QUEST == m_eNowQuestUIMode ) ||
					( true == m_bIsOpenEventCategory && QUM_EVENT_QUEST == m_eNowQuestUIMode ) ||
					( true == m_bIsOpenGuildCategory && QUM_GUILD_QUEST == m_eNowQuestUIMode ) ||
					( true == m_bIsOpenOngoingCategory && QUM_QUEST_RECEIVE == m_eNowQuestUIMode );
		}
		else
			return;
	}

	kSlot.m_bIsCategoryOpen = bOpen;
	if( NULL != kSlot.m_pButtonCategory )
	{
		if( true == bOpen )
		{
			kSlot.m_pButtonCategory->SetNormalTex( wstrTextureName.c_str(), wstrTextureOpenKey.c_str() );
			kSlot.m_pButtonCategory->SetOverTex( wstrTextureName2.c_str(), wstrTextureOverKey2.c_str() );
		}
		else
		{
			kSlot.m_pButtonCategory->SetNormalTex( wstrTextureName.c_str(), wstrTextureCloseKey.c_str() );
			kSlot.m_pButtonCategory->SetOverTex( wstrTextureName.c_str(), wstrTextureOverKey.c_str() );
		}
	}
#else
	UINT i = iQuestSlotInfoIndex;

	wstring wstrTextureName = L"DLG_UI_BUTTON01.tga";
	wstring wstrTextureName2 = L"DLG_UI_Button16.tga";

	wstring wstrTextureOpenKey = L"list_down";
	wstring wstrTextureCloseKey = L"list_normal";
	wstring wstrTextureOverKey = L"list_over";
	wstring wstrTextureOverKey2 = L"list_down_over";
	

	if(QuestInfo.m_iEpicGroupID != -1)
	{
		m_vecUIQuestSlotInfo[i].m_QuestID = QuestInfo.m_iEpicGroupID;

		if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
			m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetShowEnable(true, true);
		
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->SetShowEnable(true, true);


		wstring wstrEpicGroupTitle = g_pData->GetQuestManager()->GetEpicGroupTitle(QuestInfo.m_iEpicGroupID);

		wstringstream wstrm;

		// oasis907 : 김상윤 [2010.10.5] // 기획 요청. 에픽 그룹 번호의 자유로운 사용을 위해 수정
		//wstrm << L"EP." << QuestInfo.m_iEpicGroupID << L" " << wstrEpicGroupTitle;
		wstrm << wstrEpicGroupTitle;

		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
		{
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(0)->msg = wstrm.str();
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(1)->msg = L"";
		}

		m_vecUIQuestSlotInfo[i].m_bIsCategory = true;


		if(m_iOpenEpicCategory == QuestInfo.m_iEpicGroupID)
		{
			m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = true;
			if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
			{
				m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetNormalTex(wstrTextureName.c_str(), wstrTextureOpenKey.c_str());
				m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetOverTex(wstrTextureName2.c_str(), wstrTextureOverKey2.c_str());
			}
		}
		else
		{
			m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = false;
			if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
			{
				m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetNormalTex(wstrTextureName.c_str(), wstrTextureCloseKey.c_str());
				m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetOverTex(wstrTextureName.c_str(), wstrTextureOverKey.c_str());
			}
		}
#ifdef REFORM_QUEST
		std::map<int, bool>::iterator mit = m_mapEpicQuestProgressInfo.find( QuestInfo.m_iEpicGroupID );
		if( mit != m_mapEpicQuestProgressInfo.end() )
		{//m_mapEpicQuestProgressInfo에는 진행중인 카테고리일때만 삽입됨.
			if( true == mit->second )
			{
				if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
				{
					m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(0)->SetShow(false); //완료
					m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(1)->SetShow(true);//진행중
				}
			}
			else
			{
				if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
				{
					m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(0)->SetShow(true); //완료
					m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(1)->SetShow(false);//진행중
				}
			}
		}
		else
		{
			if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			{
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(0)->SetShow(true); //완료
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(1)->SetShow(false);//진행중
			}
		}
#endif //REFORM_QUEST
	}
	else
	{
#ifdef REFORM_QUEST
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
		{
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(0)->SetShow(false);
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetPicture(1)->SetShow(false);
		}
#endif //REFORM_QUEST
		if(QuestInfo.m_iID == 0) // 수행 가능한 의뢰
		{
			m_vecUIQuestSlotInfo[i].m_QuestID = 0;
			
			if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
				m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetShowEnable(true, true);
			if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			{
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->SetShowEnable(true, true);
				//m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(0)->msg = L"수행 가능 의뢰";
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(0)->msg = GET_STRING(STR_ID_9618);
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(1)->msg = L"";
			}

			m_vecUIQuestSlotInfo[i].m_bIsCategory = true;
			if(m_bIsOpenAvailCategory)
			{
				m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = true;
				if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
				{
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetNormalTex(wstrTextureName.c_str(), wstrTextureOpenKey.c_str());
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetOverTex(wstrTextureName2.c_str(), wstrTextureOverKey2.c_str());
				}

			}
			else
			{
				m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = false;
				if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
				{
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetNormalTex(wstrTextureName.c_str(), wstrTextureCloseKey.c_str());
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetOverTex(wstrTextureName.c_str(), wstrTextureOverKey.c_str());
				}
			}
		}
		else if(QuestInfo.m_iID == 1) // 수행 중인 의뢰
		{
			m_vecUIQuestSlotInfo[i].m_QuestID = 1;

			if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
				m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetShowEnable(true, true);
			if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			{
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->SetShowEnable(true, true);
				//m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(0)->msg = L"수행 중 의뢰";
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(0)->msg = GET_STRING(STR_ID_9619);
			}

			
			WCHAR buff[64] = {0,};
			//wsprintf( buff, L"%d/%d", m_vecOnGoingQuestInfo.size(), MY_QUEST_LIST_MAX_SIZE );
#ifdef REFORM_QUEST
			if( QUM_EVENT_QUEST != m_eNowQuestUIMode )
#endif  //REFORM_QUEST
			{
				StringCchPrintf( buff, 64, L"%d/%d", m_vecOnGoingQuestInfo.size(), MY_QUEST_LIST_MAX_SIZE );
			}

			if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
				m_vecUIQuestSlotInfo[i].m_pStaticCategory->GetString(1)->msg = buff;
			m_vecUIQuestSlotInfo[i].m_bIsCategory = true;
			
			if( (true == m_bIsOpenOngoingCategory && QUM_QUEST == m_eNowQuestUIMode) ||
				(true == m_bIsOpenEventCategory && QUM_EVENT_QUEST == m_eNowQuestUIMode)
#ifdef MODIFY_ACCEPT_QUEST
				|| (true == m_bIsOpenOngoingCategory && QUM_QUEST_RECEIVE == m_eNowQuestUIMode)				
#endif // MODIFY_ACCEPT_QUEST
				)
			{
				m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = true;
				if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
				{
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetNormalTex(wstrTextureName.c_str(), wstrTextureOpenKey.c_str());
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetOverTex(wstrTextureName2.c_str(), wstrTextureOverKey2.c_str());
				}
			}
			else
			{
				m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = false;
				if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
				{
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetNormalTex(wstrTextureName.c_str(), wstrTextureCloseKey.c_str());
					m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetOverTex(wstrTextureName.c_str(), wstrTextureOverKey.c_str());
				}
			}

		}
	}

	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

void CX2UIQuestNew::UpdateUIQuestSlotInfo( UINT iQuestSlotInfoIndex,  QuestInfo& QuestInfo )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323d8be0: one quest row
	if( false == m_bShow )
		return;

	UINT i = iQuestSlotInfoIndex;
	UIQuestSlotInfo& kSlot = m_vecUIQuestSlotInfo[i];

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( QuestInfo.m_iID );
	if( NULL == pQuestTemplet )
	{
		QuestUILog( L"[JUNE_QUEST] UI row for quest %d has no templet - skipped", QuestInfo.m_iID );
		return;
	}

	kSlot.m_QuestID			= QuestInfo.m_iID;
	kSlot.m_eQuestState		= QuestInfo.m_eQuestState;
	kSlot.m_eQuestType		= QuestInfo.m_eQuestType;
	kSlot.m_eRepeatType		= QuestInfo.m_eRepeatType;
	kSlot.m_iPlayLevel		= QuestInfo.m_iPlayLevel;
	kSlot.m_bIsCategory		= false;
	kSlot.m_bIsCategoryOpen	= false;

	if( NULL != kSlot.m_pStaticQuest )
		kSlot.m_pButtonQuest->SetShowEnable( true, true );

	if( NULL != kSlot.m_pStaticQuest )
	{
		kSlot.m_pStaticQuest->SetShow( true );

#ifdef ELLIPSE_GLOBAL
		bool bEllipse = false;
	#ifdef QUEST_LIST_TITLE_CUT_STRING
		const int MAGIC_STRING_WIDTH = 208;
	#else //QUEST_LIST_TITLE_CUT_STRING
		const int MAGIC_STRING_WIDTH = 230;
	#endif//QUEST_LIST_TITLE_CUT_STRING
		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse( pQuestTemplet->m_wstrTitle.c_str(), MAGIC_STRING_WIDTH, kSlot.m_pStaticQuest->GetString( 0 )->fontIndex, 1, bEllipse );
		if( true == bEllipse )
		{
			kSlot.m_pButtonQuest->SetGuideDesc( pQuestTemplet->m_wstrTitle.c_str() );
			kSlot.m_pButtonQuest->SetGuideDescOffsetPos( D3DXVECTOR2( 150, 50 ) );
		}
		else
			kSlot.m_pButtonQuest->SetGuideDesc( L"" );

		kSlot.m_pStaticQuest->GetString( 0 )->msg = tempName;
#else
		kSlot.m_pStaticQuest->GetString( 0 )->msg = pQuestTemplet->m_wstrTitle;
#endif ELLIPSE_GLOBAL
		kSlot.m_pStaticQuest->GetString( 0 )->color = g_pData->GetQuestManager()->GetQuestColor( QuestInfo.m_iID, g_pData->GetSelectUnitLevel() );
		kSlot.m_pStaticQuest->GetString( 1 )->msg = L"";

		// every picture off: 0..12 ( types and states ) and 15..23 ( week, dev marks, new, story, guild, transcend, month, shade ); 13 is the face
		for( int iPic = 0; iPic <= 12; ++iPic )
			kSlot.m_pStaticQuest->GetPicture( iPic )->SetShow( false );
		for( int iPic = 15; iPic <= 23; ++iPic )
			kSlot.m_pStaticQuest->GetPicture( iPic )->SetShow( false );

		// a completed epic / story quest is shaded and its title greyed
		if( QSS_COMPLETE_EPIC == kSlot.m_eQuestState || QSS_COMPLETE_STORY == kSlot.m_eQuestState )
		{
			kSlot.m_pStaticQuest->GetPicture( 23 )->SetShow( true );
			kSlot.m_pStaticQuest->GetString( 0 )->color = D3DXCOLOR( 0.7058f, 0.7058f, 0.7058f, 1 );
		}

		if( QSS_WAIT_EPIC == kSlot.m_eQuestState )
		{
			WCHAR buff[64] = {0,};
			StringCchPrintf( buff, 64, L"Lv.%d", kSlot.m_iPlayLevel );
			kSlot.m_pStaticQuest->GetString( 1 )->msg = buff;
		}

		if( QSS_CHAIN_WAIT == kSlot.m_eQuestState )
		{
			WCHAR buff[64] = {0,};
			if( g_pData->GetSelectUnitLevel() < kSlot.m_iPlayLevel )
				StringCchPrintf( buff, 64, L"Lv.%d", kSlot.m_iPlayLevel );
			else
				StringCchPrintf( buff, 64, L"%s", GET_STRING( 40047 ) );
			kSlot.m_pStaticQuest->GetString( 1 )->msg = buff;
		}

		kSlot.m_pStaticQuest->GetPicture( GetQuestPictureNumber( QuestInfo.m_eQuestType, QuestInfo.m_eRepeatType ) )->SetShow( true );

		if( false == m_setNewEventQuestIDList.empty() &&
			m_setNewEventQuestIDList.end() != m_setNewEventQuestIDList.find( kSlot.m_QuestID ) )
			kSlot.m_pStaticQuest->GetPicture( 18 )->SetShow( true );

		kSlot.m_pStaticQuest->GetPicture( GetQuestStatePictureNumber( QuestInfo.m_eQuestState ) )->SetShow( true );

		SetNpcFacePicture( kSlot.m_pStaticQuest->GetPicture( 13 ), (int)pQuestTemplet->m_eStartNPCID, GetNPCFaceTextureKey( pQuestTemplet->m_eStartNPCID ) );
	}

	if( NULL != kSlot.m_pStaticQuest )
	{
		if( kSlot.m_QuestID == m_iSelectedQuestID && -1 != m_iSelectedQuestID )
		{
			kSlot.m_pButtonQuest->SetChecked( true );
			m_iSelectedQuestSlotIndex = i;
		}
		else if( true == kSlot.m_pButtonQuest->GetChecked() )
		{
			kSlot.m_pButtonQuest->SetChecked( false );
			m_iSelectedQuestSlotIndex = -1;
		}
	}
#else
	if(false == m_bShow)
		return;

	UINT i = iQuestSlotInfoIndex;

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( QuestInfo.m_iID );
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
	if( NULL == pQuestTemplet )
	{
		QuestNoTempletLog( QuestInfo.m_iID, L"UpdateUIQuestSlotInfo" );
		return;
	}
#endif SERV_IRUHADEV_JUNE_QUEST_DATA


	m_vecUIQuestSlotInfo[i].m_QuestID = QuestInfo.m_iID;
	
	m_vecUIQuestSlotInfo[i].m_eQuestState = QuestInfo.m_eQuestState;
	m_vecUIQuestSlotInfo[i].m_eQuestType = QuestInfo.m_eQuestType;
	m_vecUIQuestSlotInfo[i].m_eRepeatType = QuestInfo.m_eRepeatType;

	m_vecUIQuestSlotInfo[i].m_iPlayLevel = QuestInfo.m_iPlayLevel;

	m_vecUIQuestSlotInfo[i].m_bIsCategory = false;
	m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = false;

	if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
		m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetShowEnable(true, true);

	if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
	{
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->SetShow(true);

#ifdef ELLIPSE_GLOBAL
		bool bEllipse = false;

		#ifdef QUEST_LIST_TITLE_CUT_STRING
		const int MAGIC_STRING_WIDTH = 208;
		#else //QUEST_LIST_TITLE_CUT_STRING
		const int MAGIC_STRING_WIDTH = 230;
		#endif//QUEST_LIST_TITLE_CUT_STRING

		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(pQuestTemplet->m_wstrTitle.c_str(), MAGIC_STRING_WIDTH, m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(0)->fontIndex, 1, bEllipse);

		if(bEllipse == true)
		{
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetGuideDesc(pQuestTemplet->m_wstrTitle.c_str());
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetGuideDescOffsetPos(D3DXVECTOR2(150,50));
		}
		else
		{
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetGuideDesc(L"");
		}

		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(0)->msg = tempName;
#else
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(0)->msg = pQuestTemplet->m_wstrTitle;
#endif ELLIPSE_GLOBAL
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(0)->color = g_pData->GetQuestManager()->GetQuestColor( QuestInfo.m_iID, g_pData->GetSelectUnitLevel() );
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(1)->msg = L"";

		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(0)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(1)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(2)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(3)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(4)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(5)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(6)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(7)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(8)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(9)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(10)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(11)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(12)->SetShow(false);

		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(15)->SetShow(false);

#ifdef SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(16)->SetShow(false);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(17)->SetShow(false);
#endif SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER

#ifdef REFORM_QUEST
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(18)->SetShow(false);
#endif //REFORM_QUEST

		if(m_vecUIQuestSlotInfo[i].m_eQuestState == QSS_COMPLETE_EPIC)
		{
			m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(15)->SetShow(true);
			m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(0)->color = D3DXCOLOR( 0.7058f, 0.7058f, 0.7058f, 1 );
		}

		if(m_vecUIQuestSlotInfo[i].m_eQuestState == QSS_WAIT_EPIC)
		{
			WCHAR buff[64] = {0,};
			//wsprintf( buff, L"Lv.%d", m_vecUIQuestSlotInfo[i].m_iPlayLevel);
			StringCchPrintf( buff, 64, L"Lv.%d", m_vecUIQuestSlotInfo[i].m_iPlayLevel);
			m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetString(1)->msg = buff;
		}

		int iQuestTypePictureNumber = GetQuestPictureNumber(QuestInfo.m_eQuestType, QuestInfo.m_eRepeatType);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(iQuestTypePictureNumber)->SetShow(true);

#ifdef SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER
		if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
		{
			if( QuestInfo.m_eRepeatType == RTS_REPEAT )
			{
				m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture( 16 )->SetShow( true );
			}
			else if( QuestInfo.m_eRepeatType == RTS_DAY )
			{
				m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture( 17 )->SetShow( true );
			}
		}
#endif SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER

#ifdef REFORM_QUEST
		if ( false == m_setNewEventQuestIDList.empty() )
		{
			std::set<int>::iterator sit = m_setNewEventQuestIDList.find(m_vecUIQuestSlotInfo[i].m_QuestID);
			if( sit != m_setNewEventQuestIDList.end() )
			{
				m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(18)->SetShow(true);
			}		
		}
#endif //REFORM_QUEST

		int iQuestStatePictureNumber = GetQuestPictureNumber(QuestInfo.m_eQuestState);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(iQuestStatePictureNumber)->SetShow(true);

		wstring wstrKey = GetNPCFaceTextureKey(pQuestTemplet->m_eStartNPCID);
		m_vecUIQuestSlotInfo[i].m_pStaticQuest->GetPicture(13)->SetTex(L"DLG_UI_Npc_Face01.TGA", wstrKey.c_str());
	}


	if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
	{
		if(m_vecUIQuestSlotInfo[i].m_QuestID == m_iSelectedQuestID && m_iSelectedQuestID != -1 )
		{
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetChecked(true);
			m_iSelectedQuestSlotIndex = i;

		}
		else if(true == m_vecUIQuestSlotInfo[i].m_pButtonQuest->GetChecked())
		{
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetChecked(false);
			m_iSelectedQuestSlotIndex = -1;
		}
	}
	
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}


void CX2UIQuestNew::InitUIQuestSlotInfo()
{
	for(int i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
	{
		if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
			m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetShowEnable(false, false);
		
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->SetShowEnable(false, false);
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetShowEnable(false, false);
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
			m_vecUIQuestSlotInfo[i].m_pStaticQuest->SetShow(false);

		m_vecUIQuestSlotInfo[i].m_bIsCategory = false;
		m_vecUIQuestSlotInfo[i].m_bIsCategoryOpen = false;
		m_vecUIQuestSlotInfo[i].m_QuestID = -1;
		m_vecUIQuestSlotInfo[i].m_DungeonID = 0;

		m_vecUIQuestSlotInfo[i].m_eQuestState = CX2UIQuestNew::QSS_AVAILABLE;
		m_vecUIQuestSlotInfo[i].m_eQuestType = CX2UIQuestNew::QTS_NORMAL;
		m_vecUIQuestSlotInfo[i].m_eRepeatType = CX2UIQuestNew::RTS_NORMAL;
	}
	return;
}

void CX2UIQuestNew::HideUIQuestSlotInfo()
{
	for(int i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
	{
		if( NULL != m_vecUIQuestSlotInfo[i].m_pButtonCategory )
			m_vecUIQuestSlotInfo[i].m_pButtonCategory->SetShowEnable(false, false);
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticCategory )
			m_vecUIQuestSlotInfo[i].m_pStaticCategory->SetShowEnable(false, false);
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
			m_vecUIQuestSlotInfo[i].m_pButtonQuest->SetShowEnable(false, false);
		if( NULL != m_vecUIQuestSlotInfo[i].m_pStaticQuest )
			m_vecUIQuestSlotInfo[i].m_pStaticQuest->SetShow(false);
	}
	return;
}


void CX2UIQuestNew::ResetQuestList(QUEST_UI_MODE eQuestUIMode)
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323d9400
	m_vecQuestInfoList.clear();

	SetNowQuestUIMode( eQuestUIMode );
	switch( m_eNowQuestUIMode )
	{
	case QUM_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back( kOngoingCategory );

			if( true == m_bIsOpenOngoingCategory )
			{
				for( UINT i = 0; i < m_vecOnGoingQuestInfo.size(); ++i )
					m_vecQuestInfoList.push_back( m_vecOnGoingQuestInfo[i] );
			}
			FillQuestSlots();
		}
		break;
	case QUM_QUEST_RECEIVE:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			UpdateAvailQuestInfo();

			QuestInfo kAvailCategory;
			kAvailCategory.m_bIsCategory = true;
			kAvailCategory.m_iID = 0;
			m_vecQuestInfoList.push_back( kAvailCategory );

			if( true == m_bIsOpenAvailCategory )
			{
				// the event and the guild quests have their own tabs
				for( UINT i = 0; i < m_vecAvailQuestInfo.size(); ++i )
				{
					if( QTS_GUILD != m_vecAvailQuestInfo[i].m_eQuestType && QTS_EVENT != m_vecAvailQuestInfo[i].m_eQuestType )
						m_vecQuestInfoList.push_back( m_vecAvailQuestInfo[i] );
				}
			}

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back( kOngoingCategory );

			if( true == m_bIsOpenOngoingCategory )
			{
				for( UINT i = 0; i < m_vecOnGoingQuestInfo.size(); ++i )
					m_vecQuestInfoList.push_back( m_vecOnGoingQuestInfo[i] );
			}
			FillQuestSlots();
		}
		break;
	case QUM_EPIC_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			int iEpicGroupID = -1;
			for( UINT i = 0; i < m_vecEpicQuestInfo.size(); ++i )
			{
				if( m_vecEpicQuestInfo[i].m_iEpicGroupID != iEpicGroupID )
				{
					iEpicGroupID = m_vecEpicQuestInfo[i].m_iEpicGroupID;

					QuestInfo kEpicCategory;
					kEpicCategory.m_bIsCategory = true;
					kEpicCategory.m_iEpicGroupID = iEpicGroupID;
					m_vecQuestInfoList.push_back( kEpicCategory );
				}
				if( m_vecEpicQuestInfo[i].m_iEpicGroupID == m_iOpenEpicCategory )
					m_vecQuestInfoList.push_back( m_vecEpicQuestInfo[i] );
			}
			FillQuestSlots();
		}
		break;
	case QUM_QUEST_DETAIL:
		{
			m_pDLGUIQuestList->SetShowEnable( false, false );
			m_pDLGUIQuestDetail->SetShowEnable( true, true );

			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
			if( NULL == pQuestTemplet )
			{
				ReturnToQuestListUI();
				ResetQuestUI( true, true );
				return;
			}
			m_setNewEventQuestIDList.erase( m_iSelectedQuestID );

			InitRewardItemSlot();
			HideUIQuestSlotInfo();

			UpdateDetailQuestTitle();

			const wstring wstrDesc = CreateStringQuestClearCondition();
#ifdef CLIENT_GLOBAL_LINEBREAK
			D3DXVECTOR2 tmp_vQuestDescSize1 = D3DXVECTOR2( (float) m_pDLGUIQuestDetail->GetDummyInt( 0 ), (float) m_pDLGUIQuestDetail->GetDummyInt( 1 ) );
			m_vecQuestDesc.resize( 0 );
			CWordLineHandler::ProcessQuestDescInUIQuestNew( wstrDesc, m_iLineWidthQuestClearCondition, m_pFontQuestClearCondition, tmp_vQuestDescSize1, m_vecQuestDesc );
#else //CLIENT_GLOBAL_LINEBREAK
			ProcessQuestDesc( wstrDesc, m_iLineWidthQuestClearCondition, m_pFontQuestClearCondition );
#endif //CLIENT_GLOBAL_LINEBREAK
			UpdateQuestClearCondition();

			const wstring wstrNPCDesc = CreateStringNPCQuestDescription();
#ifdef CLIENT_GLOBAL_LINEBREAK
			D3DXVECTOR2 tmp_vQuestDescSize2 = D3DXVECTOR2( (float) m_pDLGUIQuestDetail->GetDummyInt( 0 ), (float) m_pDLGUIQuestDetail->GetDummyInt( 1 ) );
			CWordLineHandler::ProcessQuestDescInUIQuestNew( wstrNPCDesc, m_iLineWidthNPCQuestDescription, m_pFontNPCQuestDescription, tmp_vQuestDescSize2, m_vecQuestDesc );
#else //CLIENT_GLOBAL_LINEBREAK
			ProcessQuestDesc( wstrNPCDesc, m_iLineWidthNPCQuestDescription, m_pFontNPCQuestDescription );
#endif //CLIENT_GLOBAL_LINEBREAK
			UpdateNPCQuestDescription();

			UpdateRewardInfo();
			UpdateRewardItemSlot();
			// June 323cdbc0 here: the guild's reward rows ( GuildCoopQuestRewardTable ). Guild quests are not loaded offline ( Q1 ).
			UpdateQuestButton();
		}
		break;
	case QUM_EVENT_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back( kOngoingCategory );

			if( true == m_bIsOpenEventCategory )
			{
				for( UINT i = 0; i < m_vecOnGoingEventQuestInfo.size(); ++i )
					m_vecQuestInfoList.push_back( m_vecOnGoingEventQuestInfo[i] );
			}
			FillQuestSlots();
		}
		break;
	case QUM_GUILD_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back( kOngoingCategory );

			if( true == m_bIsOpenGuildCategory )
			{
				// the guild quests that can be taken, then the ones in progress
				for( UINT i = 0; i < m_vecAvailQuestInfo.size(); ++i )
				{
					if( QTS_GUILD == m_vecAvailQuestInfo[i].m_eQuestType )
						m_vecQuestInfoList.push_back( m_vecAvailQuestInfo[i] );
				}
				for( UINT i = 0; i < m_vecOnGoingGuildQuestInfo.size(); ++i )
					m_vecQuestInfoList.push_back( m_vecOnGoingGuildQuestInfo[i] );
			}
			FillQuestSlots();
		}
		break;
	case QUM_STORY_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			int iStoryGroupID = -1;
			for( UINT i = 0; i < m_vecOnGoingStoryQuestInfo.size(); ++i )
			{
				if( m_vecOnGoingStoryQuestInfo[i].m_iEpicGroupID != iStoryGroupID )
				{
					iStoryGroupID = m_vecOnGoingStoryQuestInfo[i].m_iEpicGroupID;

					QuestInfo kStoryCategory;
					kStoryCategory.m_bIsCategory = true;
					kStoryCategory.m_iEpicGroupID = iStoryGroupID;
					m_vecQuestInfoList.push_back( kStoryCategory );
				}
				if( m_vecOnGoingStoryQuestInfo[i].m_iEpicGroupID == m_iOpenStoryCategory )
					m_vecQuestInfoList.push_back( m_vecOnGoingStoryQuestInfo[i] );
			}
			FillQuestSlots();
		}
		break;
	}
#else
	m_vecQuestInfoList.clear();

	SetNowQuestUIMode(eQuestUIMode);
	switch(m_eNowQuestUIMode)
	{
	case QUM_QUEST_RECEIVE:
		{	
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable(true, true);
			m_pDLGUIQuestDetail->SetShowEnable(false, false);

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			UpdateAvailQuestInfo();
			//UpdateOngoingQuestInfo();

			QuestInfo kAvailCategory;
			kAvailCategory.m_bIsCategory = true;
			kAvailCategory.m_iID = 0;
			m_vecQuestInfoList.push_back(kAvailCategory);

			if(m_bIsOpenAvailCategory)
			{
				for(UINT i=0; i< m_vecAvailQuestInfo.size(); i++)
				{
#ifdef ACCEPT_QUEST_TAB_FIX
					if( m_vecAvailQuestInfo[i].m_eQuestType != QTS_EVENT )
#endif // ACCEPT_QUEST_TAB_FIX
					m_vecQuestInfoList.push_back(m_vecAvailQuestInfo[i]);
				}
			}

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back(kOngoingCategory);

			if(m_bIsOpenOngoingCategory)
			{
				for(UINT i=0; i< m_vecOnGoingQuestInfo.size(); i++)
				{
					m_vecQuestInfoList.push_back(m_vecOnGoingQuestInfo[i]);
				}
			}
			m_iSizeQuestInfoList = m_vecQuestInfoList.size();

#ifdef MODIFY_ACCEPT_QUEST
			if(m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage)
			{
				m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
			}
			if(m_iTopQuestSlotIndex < 0)
			{
				m_iTopQuestSlotIndex = 0;
			}
#endif // MODIFY_ACCEPT_QUEST
			// 업데이트
			for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
			{
				if(i + m_iTopQuestSlotIndex >= m_vecQuestInfoList.size())
				{
					break;
				}

				if(m_vecQuestInfoList[m_iTopQuestSlotIndex + i].m_bIsCategory == true)
				{
					SetUIQuestSlotInfoToCategory(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
				else
				{
					UpdateUIQuestSlotInfo(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
			}
		}
		break;
	case QUM_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable(true, true);
			m_pDLGUIQuestDetail->SetShowEnable(false, false);

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			//UpdateOngoingQuestInfo();

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back(kOngoingCategory);

			if(m_bIsOpenOngoingCategory)
			{
				for(UINT i=0; i< m_vecOnGoingQuestInfo.size(); i++)
				{
					m_vecQuestInfoList.push_back(m_vecOnGoingQuestInfo[i]);
				}
			}

			m_iSizeQuestInfoList = m_vecQuestInfoList.size();

#ifdef MODIFY_ACCEPT_QUEST
			if(m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage)
			{
				m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
			}
			if(m_iTopQuestSlotIndex < 0)
			{
				m_iTopQuestSlotIndex = 0;
			}
#endif // MODIFY_ACCEPT_QUEST
			// 업데이트
			for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
			{
				if(i + m_iTopQuestSlotIndex >= m_vecQuestInfoList.size())
				{
					break;
				}

				if(m_vecQuestInfoList[m_iTopQuestSlotIndex + i].m_bIsCategory == true)
				{
					SetUIQuestSlotInfoToCategory(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
				else
				{
					UpdateUIQuestSlotInfo(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
			}
		}
		break;
	case QUM_EPIC_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable(true, true);
			m_pDLGUIQuestDetail->SetShowEnable(false, false);

			// current opened ep index
			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			//UpdateOngoingQuestInfo();

			int iEpicGroupID = -1;
			for(UINT i=0; i< m_vecEpicQuestInfo.size(); i++)
			{
				if(m_vecEpicQuestInfo[i].m_iEpicGroupID != iEpicGroupID)
				{
					iEpicGroupID = m_vecEpicQuestInfo[i].m_iEpicGroupID;

					QuestInfo kEpicCategory;
					kEpicCategory.m_bIsCategory = true;
					kEpicCategory.m_iEpicGroupID = iEpicGroupID;
					m_vecQuestInfoList.push_back(kEpicCategory);
				}
				if(m_vecEpicQuestInfo[i].m_iEpicGroupID == m_iOpenEpicCategory)
				{
					m_vecQuestInfoList.push_back(m_vecEpicQuestInfo[i]);
				}
			}

			m_iSizeQuestInfoList = m_vecQuestInfoList.size();

#ifdef MODIFY_ACCEPT_QUEST
			if(m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage)
			{
				m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
			}
			if(m_iTopQuestSlotIndex < 0)
			{
				m_iTopQuestSlotIndex = 0;
			}
#endif // MODIFY_ACCEPT_QUEST
			// 업데이트
			for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
			{
				if(i + m_iTopQuestSlotIndex >= m_vecQuestInfoList.size())
				{
					break;
				}

				if(m_vecQuestInfoList[m_iTopQuestSlotIndex + i].m_bIsCategory == true)
				{
					SetUIQuestSlotInfoToCategory(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
				else
				{
					UpdateUIQuestSlotInfo(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
			}
		}
		break;
	case QUM_QUEST_DETAIL:
		{
			m_pDLGUIQuestList->SetShowEnable(false, false);
			m_pDLGUIQuestDetail->SetShowEnable(true, true);

			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );

			if(pQuestTemplet == NULL)
			{
				ReturnToQuestListUI();
				ResetQuestUI(true, true);
				return;
			}
#ifdef REFORM_QUEST
			m_setNewEventQuestIDList.erase( m_iSelectedQuestID );
#endif //REFORM_QUEST
			InitRewardItemSlot();
			// UIQuestSlotInfo의 정보를 Detail다이얼로그에서 활용하기 때문에
			// InitUIQuestSlotInfo(); 하지 않는다.
			HideUIQuestSlotInfo();

			// 퀘스트 제목
			UpdateDetailQuestTitle();

			// 퀘스트 완료 조건
			const wstring wstrDesc = CreateStringQuestClearCondition();
#ifdef CLIENT_GLOBAL_LINEBREAK
			D3DXVECTOR2 tmp_vQuestDescSize1 = D3DXVECTOR2( (float) m_pDLGUIQuestDetail->GetDummyInt( 0 ), (float) m_pDLGUIQuestDetail->GetDummyInt( 1 ) );
			m_vecQuestDesc.resize(0);
			CWordLineHandler::ProcessQuestDescInUIQuestNew( wstrDesc, m_iLineWidthQuestClearCondition, m_pFontQuestClearCondition, tmp_vQuestDescSize1, m_vecQuestDesc);
#else //CLIENT_GLOBAL_LINEBREAK
			ProcessQuestDesc( wstrDesc, m_iLineWidthQuestClearCondition, m_pFontQuestClearCondition);
#endif //CLIENT_GLOBAL_LINEBREAK

			UpdateQuestClearCondition();

			// 퀘스트 MainText
			const wstring wstrNPCDesc = CreateStringNPCQuestDescription();
#ifdef CLIENT_GLOBAL_LINEBREAK
			D3DXVECTOR2 tmp_vQuestDescSize2 = D3DXVECTOR2( (float) m_pDLGUIQuestDetail->GetDummyInt( 0 ), (float) m_pDLGUIQuestDetail->GetDummyInt( 1 ) );
			CWordLineHandler::ProcessQuestDescInUIQuestNew( wstrNPCDesc, m_iLineWidthNPCQuestDescription, m_pFontNPCQuestDescription, tmp_vQuestDescSize2, m_vecQuestDesc);
#else //CLIENT_GLOBAL_LINEBREAK
			ProcessQuestDesc( wstrNPCDesc, m_iLineWidthNPCQuestDescription, m_pFontNPCQuestDescription);
#endif //CLIENT_GLOBAL_LINEBREAK

			UpdateNPCQuestDescription();

			UpdateRewardInfo();

			UpdateRewardItemSlot();

			UpdateQuestButton();

		}
		break;
#ifdef REFORM_QUEST
	case QUM_EVENT_QUEST:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable(true, true);
			m_pDLGUIQuestDetail->SetShowEnable(false, false);

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back(kOngoingCategory);

			if(m_bIsOpenEventCategory)
			{
				for(UINT i=0; i< m_vecOnGoingEventQuestInfo.size(); i++)
				{
					m_vecQuestInfoList.push_back(m_vecOnGoingEventQuestInfo[i]);
				}
			}

			m_iSizeQuestInfoList = m_vecQuestInfoList.size();

#ifdef MODIFY_ACCEPT_QUEST
			if(m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage)
			{
				m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
			}
			if(m_iTopQuestSlotIndex < 0)
			{
				m_iTopQuestSlotIndex = 0;
			}
#endif // MODIFY_ACCEPT_QUEST
			// 업데이트
			for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
			{
				if(i + m_iTopQuestSlotIndex >= m_vecQuestInfoList.size())
				{
					break;
				}

				if(m_vecQuestInfoList[m_iTopQuestSlotIndex + i].m_bIsCategory == true)
				{
					SetUIQuestSlotInfoToCategory(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
				else
				{
					UpdateUIQuestSlotInfo(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
			}
		}
		break;
#ifdef ACCEPT_QUEST_TAB_FIX
	case QUM_EVENT_QUEST_RECEIVE:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable(true, true);
			m_pDLGUIQuestDetail->SetShowEnable(false, false);

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			UpdateAvailQuestInfo();

			QuestInfo kAvailCategory;
			kAvailCategory.m_bIsCategory = true;
			kAvailCategory.m_iID = 0;
			m_vecQuestInfoList.push_back(kAvailCategory);

			if(m_bIsOpenAvailCategory)
			{
				for(UINT i=0; i< m_vecAvailQuestInfo.size(); i++)
				{
					if( m_vecAvailQuestInfo[i].m_eQuestType == QTS_EVENT )
						m_vecQuestInfoList.push_back(m_vecAvailQuestInfo[i]);
				}
			}

			QuestInfo kOngoingCategory;
			kOngoingCategory.m_bIsCategory = true;
			kOngoingCategory.m_iID = 1;
			m_vecQuestInfoList.push_back(kOngoingCategory);

			if(m_bIsOpenEventCategory)
			{
				for(UINT i=0; i< m_vecOnGoingEventQuestInfo.size(); i++)
				{
					m_vecQuestInfoList.push_back(m_vecOnGoingEventQuestInfo[i]);
				}
			}

			m_iSizeQuestInfoList = m_vecQuestInfoList.size();

#ifdef MODIFY_ACCEPT_QUEST
			if(m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage)
			{
				m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
			}
			if(m_iTopQuestSlotIndex < 0)
			{
				m_iTopQuestSlotIndex = 0;
			}
#endif // MODIFY_ACCEPT_QUEST
			// 업데이트
			for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; i++)
			{
				if(i + m_iTopQuestSlotIndex >= m_vecQuestInfoList.size())
				{
					break;
				}

				if(m_vecQuestInfoList[m_iTopQuestSlotIndex + i].m_bIsCategory == true)
				{
					SetUIQuestSlotInfoToCategory(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
				else
				{
					UpdateUIQuestSlotInfo(i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i]);
				}
			}
		}
		break;
#endif // ACCEPT_QUEST_TAB_FIX
#endif //REFORM_QUEST
	}
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

void CX2UIQuestNew::ResetTabPriorityFactor()
{
	m_bCompleteQuestInEpicTab = false;
	m_bCompleteQuestInNormalTab = false;
	m_bOngoingQuestInEpicTab = false;
	m_bOngoingQuestInNormalTab = false;

	m_iCompleteEpicCategory = -1;
	m_iOngoingEpicCategory = -1;

	return;
}


CX2UIQuestNew::QUEST_UI_MODE CX2UIQuestNew::DetermineUIModeWithTabPriority()
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323c9250: the tab the window opens on. Only the "complete" flags matter; the request tab with the available list open is the default.
	if( m_bCompleteQuestInEpicTab )
	{
		m_iOpenEpicCategory = m_iCompleteEpicCategory;
		return QUM_EPIC_QUEST;
	}
	if( m_bCompleteQuestInStoryTab )
	{
		m_iOpenStoryCategory = m_iCompleteStoryCategory;
		return QUM_STORY_QUEST;
	}
	if( m_bCompleteQuestInNormalTab )
	{
		m_bIsOpenOngoingCategory = true;
		return QUM_QUEST_RECEIVE;
	}

	m_bIsOpenAvailCategory = true;
	return QUM_QUEST_RECEIVE;
#else
	QUEST_UI_MODE eQuestUIMode;
	// oasis907 : 김상윤 [2010.7.12] // 퀘스트창 열었을시 우선 순위에 따라 시나리오 또는 의뢰탭 결정
	if(m_bCompleteQuestInEpicTab)
	{
		eQuestUIMode = QUM_EPIC_QUEST;
		m_iOpenEpicCategory = m_iCompleteEpicCategory;
	}
	else if(m_bCompleteQuestInNormalTab)
	{
		if(IsTalkingQuestNPC() )
		{
			eQuestUIMode = QUM_QUEST_RECEIVE;
		}
		else
		{
			eQuestUIMode = QUM_QUEST;
		}
		m_bIsOpenOngoingCategory = true;
	}
	else if(IsTalkingQuestNPC() )
	{
		eQuestUIMode = QUM_QUEST_RECEIVE;
		m_bIsOpenAvailCategory = true;
	}
	else if(m_bOngoingQuestInNormalTab)
	{
		eQuestUIMode = QUM_QUEST;
		m_bIsOpenOngoingCategory = true;
	}
	else if(m_bOngoingQuestInEpicTab)
	{
		eQuestUIMode = QUM_EPIC_QUEST;
		m_iOpenEpicCategory = m_iOngoingEpicCategory;
	}
	else
	{
		eQuestUIMode = QUM_QUEST;
	}
	return eQuestUIMode;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

void CX2UIQuestNew::ResetQuestUI( bool bUpdateNavi, bool bResetExpand, bool bIgnoreTabPriority )
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323dfa80. The scenario-movie tab ( +0x774 ) skips all of it; Q8 owns that tab.
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	if( true == m_bMovieTab )		// June 323dfab2: an open Video tab skips the whole reset ( quest-system Q8b )
		return;
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	UpdateOngoingQuestInfo();
#ifdef GUIDE_QUEST_MONSTER
	UpdateQuestMonster();
#endif //GUIDE_QUEST_MONSTER
#ifdef QUEST_GUIDE
	if( NULL != g_pData && NULL != g_pData->GetQuestManager() )
		g_pData->GetQuestManager()->SetHasDungeonQuest();
#endif //QUEST_GUIDE

	if( false == bIgnoreTabPriority )
		ResetQuestList( DetermineUIModeWithTabPriority() );
	else
	{
		QUEST_UI_MODE eMode = m_eNowQuestUIMode;
		if( QUM_QUEST == eMode )
			eMode = QUM_QUEST_RECEIVE;		// June: IsTalkingQuestNPC() is always true
		ResetQuestList( eMode );
	}

	if( true == bUpdateNavi )
		UpdateNavi();

	HideMovieTab();		// June 323e1bf0 / 323e5340: every reset ends with the movie-tab hide
	UpdateQuickQuestDLG( bResetExpand, false );

#ifdef GUIDE_QUICK_QUEST_COMPLETE
	ShowUIDesc( false );
#endif //GUIDE_QUICK_QUEST_COMPLETE

	if( false == m_bShow )
	{
		InitRewardItemSlot();
		m_pDLGUIQuestList->SetShowEnable( false, false );
		m_pDLGUIQuestDetail->SetShowEnable( false, false );
	}
#else SERV_IRUHADEV_JUNE_QUEST_UI

	// 
#ifndef MODIFY_ACCEPT_QUEST
	// m_iSizeQuestInfoList 값이 변경되기 전m_iTopQuestSlotIndex 변경했을 때 문제점 때문에 위치 변경.
	// 문제점 : m_iSizeQuestInfoList이 커져도 , m_iTopQuestSlotIndex 값은 커지기 전 값으로 값이 제한 됨.
	if(m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage)
	{
		m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
	}
	if(m_iTopQuestSlotIndex < 0)
	{
		m_iTopQuestSlotIndex = 0;
	}
#endif // MODIFY_ACCEPT_QUEST
	
	UpdateOngoingQuestInfo();
#ifdef GUIDE_QUEST_MONSTER
	UpdateQuestMonster();
#endif //GUIDE_QUEST_MONSTER

#ifdef QUEST_GUIDE
	if( NULL != g_pData && NULL != g_pData->GetQuestManager() )
	{		
		g_pData->GetQuestManager()->SetHasDungeonQuest();
	}
#endif //QUEST_GUIDE
	if(bIgnoreTabPriority)
	{
		if(m_eNowQuestUIMode == QUM_QUEST)
		{
			if(IsTalkingQuestNPC())
				ResetQuestList(QUM_QUEST_RECEIVE);
			else
				ResetQuestList(QUM_QUEST);
		}
#ifdef ACCEPT_QUEST_TAB_FIX
		else if( m_eNowQuestUIMode == QUM_EVENT_QUEST )
		{
			if(IsTalkingQuestNPC())
				ResetQuestList( QUM_EVENT_QUEST_RECEIVE );
			else
				ResetQuestList( QUM_EVENT_QUEST );
		}
#endif // ACCEPT_QUEST_TAB_FIX
		else
		{
			ResetQuestList(m_eNowQuestUIMode);
		}
	}
	else
	{
		QUEST_UI_MODE eQuestUIMode = DetermineUIModeWithTabPriority();
		ResetQuestList(eQuestUIMode);
	}


	if(bUpdateNavi)
	{
		UpdateNavi();
	}

	UpdateQuickQuestDLG(bResetExpand, false);
	
	if(!m_bShow)
	{
		InitRewardItemSlot();
		m_pDLGUIQuestList->SetShowEnable(false, false);
		m_pDLGUIQuestDetail->SetShowEnable(false, false);
	}

#ifdef GUIDE_QUICK_QUEST_COMPLETE
	ShowUIDesc(false);
#endif //GUIDE_QUICK_QUEST_COMPLETE
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}


void CX2UIQuestNew::ExpandUIQuickQuest(int iSelectedUIQuickQuestIndex, bool bMouseOver)
{
	float fExpandSizeY =  m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_fExpandSizeY;
	
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_bIsExpand = true;


	CKTDGUIControl::UIPointData* pNormalPointData = new CKTDGUIControl::UIPointData();
	pNormalPointData->addSize.y = fExpandSizeY + 18.f;
	pNormalPointData->addSize.x = 200.f;
	pNormalPointData->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	CKTDGUIControl::UIPointData* pOverPointData = new CKTDGUIControl::UIPointData();
	pOverPointData->addSize.y = fExpandSizeY + 18.f;
	pOverPointData->addSize.x = 200.f;
	pOverPointData->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	CKTDGUIControl::UIPointData* pDownPointData = new CKTDGUIControl::UIPointData();
	pDownPointData->addSize.y = fExpandSizeY + 18.f;
	pDownPointData->addSize.x = 200.f;
	pDownPointData->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonExpand->SetNormalPoint(pNormalPointData);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonExpand->SetOverPoint(pOverPointData);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonExpand->SetDownPoint(pDownPointData);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonExpand->SetShowEnable(true, true);
	

	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->GetPicture(1)->SetSizeY(fExpandSizeY);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->GetPicture(2)->SetPos(D3DXVECTOR2(0.f, fExpandSizeY + 18.f));
	

	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->GetPicture(1)->SetSizeY(fExpandSizeY);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->GetPicture(2)->SetPos(D3DXVECTOR2(0.f, fExpandSizeY + 18.f));

	if(bMouseOver)
	{
		m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->SetShowEnable(false, false);
		m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->SetShowEnable(true, true);
	}
	else
	{
		m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->SetShowEnable(true, true);
		m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->SetShowEnable(false, false);
	}



	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticQuestTitle->SetShowEnable(true, true);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticQuestInfo->SetShowEnable(true, true);


	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonMinimize->SetShowEnable(false, false);

	for(UINT i=iSelectedUIQuickQuestIndex + 1; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		D3DXVECTOR2 Pos = m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetPos();
		Pos.y = Pos.y + (fExpandSizeY + 4.f);
		m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetPos(Pos);
	}
	return;
}

void CX2UIQuestNew::MinimizeUIQuickQuest(int iSelectedUIQuickQuestIndex, bool bPreserveExpandInfo)
{
	float fExpandSizeY =  m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_fExpandSizeY;


	if(!bPreserveExpandInfo)
	{
		m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_bIsExpand = false;
	}

	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonExpand->SetShowEnable(false, false);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->SetShowEnable(false, false);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->SetShowEnable(false, false);

	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pButtonMinimize->SetShowEnable(true, true);

	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticQuestTitle->SetShowEnable(true, true);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticQuestInfo->SetShowEnable(false, false);

	for(UINT i=iSelectedUIQuickQuestIndex + 1; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		D3DXVECTOR2 Pos = m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetPos();
		Pos.y = Pos.y - (fExpandSizeY + 4.f);
		m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetPos(Pos);
	}
	return;
}

#ifdef GUIDE_QUICK_QUEST_COMPLETE
void CX2UIQuestNew::MouseOverUIQuickQuest(int iSelectedUIQuickQuestIndex, D3DXVECTOR2 vPos)
#else
void CX2UIQuestNew::MouseOverUIQuickQuest(int iSelectedUIQuickQuestIndex)
#endif //GUIDE_QUICK_QUEST_COMPLETE
{
	if( false == m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_bIsExpand )
		return;

	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->SetShowEnable(false, false);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->SetShowEnable(true, true);

#ifdef GUIDE_QUICK_QUEST_COMPLETE
	//보여지는 퀘스트 개수를 초과하는 경우 말풍선 가이드 보여주지 않도록 수정
	if( iSelectedUIQuickQuestIndex < m_iShowQuickQuest )
	{
		if( NULL != g_pData &&
			NULL != g_pData->GetMessenger() &&
			false == g_pData->GetMessenger()->GetOpen() )
		{
			ShowUIDesc( true, GET_STRING(STR_ID_19197), vPos ); 
		}
	}
#endif //GUIDE_QUICK_QUEST_COMPLETE

	return;
}

void CX2UIQuestNew::MouseOutUIQuickQuest(int iSelectedUIQuickQuestIndex)
{
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGNormal->SetShowEnable(true, true);
	m_vecUIQuickQuest[iSelectedUIQuickQuestIndex].m_pStaticExpandBGOver->SetShowEnable(false, false);
	return;
}

bool CX2UIQuestNew::GetOverMouseQuest()
{

	return false;		
}
bool CX2UIQuestNew::GetShowQuickQuestDLG()
{
	if(true == m_bShowQuickQuestDLG && true == m_bUserShowQuickQuestDLG)
	{
		return true;
	}
	return false;
}


#ifdef REFORM_QUEST
bool CX2UIQuestNew::GetSubQuestDungeonIDList( IN int iSubQuestID, OUT vector<int>& vecDungeonID_ )
{
	if( NULL == g_pData )
		return false;

	if( NULL == g_pData->GetQuestManager() )
		return false;

	vecDungeonID_.clear();

	const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( iSubQuestID );

	if( NULL != pSubQuestTemplet )
	{
		switch( pSubQuestTemplet->m_eClearType )
		{
		case CX2QuestManager::SQT_NPC_HUNT:
		case CX2QuestManager::SQT_QUEST_ITEM_COLLECTION:
		case CX2QuestManager::SQT_DUNGEON_TIME:
		case CX2QuestManager::SQT_DUNGEON_RANK:
		case CX2QuestManager::SQT_DUNGEON_DAMAGE:
		case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
#ifdef SERV_INTEGRATION
		case CX2QuestManager::SQT_WITH_DIF_SERV_USER:
#endif SERV_INTEGRATION
		case CX2QuestManager::SQT_VISIT_DUNGEON:
#ifdef SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST
		case CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR:
#endif SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST
			{
				// DID는 normal-hard-expert가 붙어 있고, 30000부터 시작하고, 각 던전마다 10단위로 분류되어 있다.
				// 따라서 그냥 10자리를 자른 값을 넣어 준다.
				// 해당 던전이 존재하는 던전이면 던전 ID로 묶어주고
				int iDungeonID = 0;
				std::set<SEnum::DUNGEON_ID>::iterator sit = pSubQuestTemplet->m_ClearCondition.m_setDungeonID.begin();
				for( ; sit != pSubQuestTemplet->m_ClearCondition.m_setDungeonID.end(); ++sit)
				{
					//iDungeonID = static_cast<int>( *sit / 10 ) * 10;
					vecDungeonID_.push_back(*sit);
				}
			}
			break;
		case CX2QuestManager::SQT_NPC_TALK:
		case CX2QuestManager::SQT_ITEM_COLLECTION:
		case CX2QuestManager::SQT_PVP_PLAY:
		case CX2QuestManager::SQT_PVP_WIN:
		case CX2QuestManager::SQT_PVP_KILL:
#ifdef SERV_NEW_PVP_QUEST
		case CX2QuestManager::SQT_PVP_NPC_HUNT:
#endif SERV_NEW_PVP_QUEST
#ifdef SERV_PVP_NPC_HUNT_QUEST_EXTEND
		case CX2QuestManager::SQT_PVP_HERO_NPC_KILL:
		case CX2QuestManager::SQT_PVP_HERO_NPC_PLAY:
#endif //SERV_PVP_NPC_HUNT_QUEST_EXTEND
		default:
			{
			} break;
		}
	}

	return true;

}
#else
int CX2UIQuestNew::GetSubQuestDungeonID( int iSubQuestID )
{
	const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( iSubQuestID );

	if(pSubQuestTemplet != NULL )
	{
		switch( pSubQuestTemplet->m_eClearType )
		{
		case CX2QuestManager::SQT_NPC_HUNT:
		case CX2QuestManager::SQT_QUEST_ITEM_COLLECTION:
		case CX2QuestManager::SQT_DUNGEON_TIME:
		case CX2QuestManager::SQT_DUNGEON_RANK:
		case CX2QuestManager::SQT_DUNGEON_DAMAGE:
		case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
#ifdef SERV_INTEGRATION
		case CX2QuestManager::SQT_WITH_DIF_SERV_USER:
#endif SERV_INTEGRATION
#ifdef SERV_POINT_COUNT_SYSTEM
		case CX2QuestManager::SQT_POINT_COUNT:
#endif SERV_POINT_COUNT_SYSTEM
			{
				// DID는 normal-hard-expert가 붙어 있고, 30000부터 시작하고, 각 던전마다 10단위로 분류되어 있다.
				// 따라서 그냥 10자리를 자른 값을 넣어 준다.
				// 해당 던전이 존재하는 던전이면 던전 ID로 묶어주고
				if( NULL != g_pData->GetDungeonManager()->GetDungeonData((SEnum::DUNGEON_ID)pSubQuestTemplet->m_ClearCondition.m_eDungeonID) )
				{
					return (int)(pSubQuestTemplet->m_ClearCondition.m_eDungeonID / 10) * 10;
				}
				else
				{
					// 아니면 일반으로 묶는다
					return SEnum::DI_END;
				}				
			}
			break;
		case CX2QuestManager::SQT_NPC_TALK:
		case CX2QuestManager::SQT_ITEM_COLLECTION:
		case CX2QuestManager::SQT_PVP_PLAY:
		case CX2QuestManager::SQT_PVP_WIN:
		case CX2QuestManager::SQT_PVP_KILL:
#ifdef SERV_NEW_PVP_QUEST
		case CX2QuestManager::SQT_PVP_NPC_HUNT:
#endif SERV_NEW_PVP_QUEST
#ifdef SERV_PVP_NPC_HUNT_QUEST_EXTEND
		case CX2QuestManager::SQT_PVP_HERO_NPC_KILL:
		case CX2QuestManager::SQT_PVP_HERO_NPC_PLAY:
#endif //SERV_PVP_NPC_HUNT_QUEST_EXTEND
#ifdef SERV_SKILL_USE_SUBQUEST
		case CX2QuestManager::SQT_SKILL_USE:
#endif SERV_SKILL_USE_SUBQUEST
		default:
			{
				// 없다
				return SEnum::DI_END;
			} break;
		}
	}
	else
	{
		// 아예 해당 서브퀘스트가 없다
		return SEnum::DI_END;
	}

}
#endif //REFORM_QUEST


bool CX2UIQuestNew::IsStateQuickQueskFilter()
{
	bool bRet = false;
	switch(g_pMain->GetNowStateID())
	{
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_PVP_GAME:
		ShowUIDesc(false);
		bRet = true;
		break;
	default:
		bRet = false;
	}
	return bRet;
}

bool CX2UIQuestNew::IsStateSubQuestCanClear( int iSubQuestID, int iNowDungeonID )
{
	bool bAnswer = false;
	const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( iSubQuestID );

	if(pSubQuestTemplet != NULL )
	{
		switch( pSubQuestTemplet->m_eClearType )
		{
		case CX2QuestManager::SQT_NPC_HUNT:
		case CX2QuestManager::SQT_ITEM_COLLECTION:
		case CX2QuestManager::SQT_QUEST_ITEM_COLLECTION:
		case CX2QuestManager::SQT_DUNGEON_TIME:
		case CX2QuestManager::SQT_DUNGEON_RANK:
		case CX2QuestManager::SQT_DUNGEON_DAMAGE:
		case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
		case CX2QuestManager::SQT_FIND_NPC:
#ifdef SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST
		case CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR:
#endif SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST
#ifdef SERV_POINT_COUNT_SYSTEM
		case CX2QuestManager::SQT_POINT_COUNT:
#endif SERV_POINT_COUNT_SYSTEM
			{
#ifdef REFORM_QUEST
				if( false == pSubQuestTemplet->m_ClearCondition.m_setDungeonID.empty() )
				{
					std::set<SEnum::DUNGEON_ID>::iterator sit = pSubQuestTemplet->m_ClearCondition.m_setDungeonID.begin();
					for( ; sit != pSubQuestTemplet->m_ClearCondition.m_setDungeonID.end(); ++sit)
					{
						int SubQuestDungeonID = ( static_cast<int>(*sit) / 10 ) * 10;
						if(SubQuestDungeonID / 10 == iNowDungeonID / 10)
							bAnswer = true;
					}
				}

				if( true == pSubQuestTemplet->m_ClearCondition.m_setDungeonID.empty() &&
					true == pSubQuestTemplet->m_ClearCondition.m_setBattleFieldID.empty() )
				{
					bAnswer = true;
				}
#else
				if(pSubQuestTemplet->m_ClearCondition.m_eDungeonID != SEnum::DI_NONE)
				{
					int SubQuestDungeonID = (int)(pSubQuestTemplet->m_ClearCondition.m_eDungeonID / 10) * 10;
					if(SubQuestDungeonID / 10 == iNowDungeonID / 10)
						bAnswer = true;
					else
						bAnswer = false; // 확실히 아님
				}
				else
				{
					bAnswer = true; // 알수 없으므로
				}
#endif //REFORM_QUEST
			}
			break;
		case CX2QuestManager::SQT_PVP_PLAY:
		case CX2QuestManager::SQT_PVP_WIN:
		case CX2QuestManager::SQT_PVP_KILL:
		case CX2QuestManager::SQT_PVP_NPC_HUNT:
#ifdef SERV_PVP_NPC_HUNT_QUEST_EXTEND
		case CX2QuestManager::SQT_PVP_HERO_NPC_KILL:
		case CX2QuestManager::SQT_PVP_HERO_NPC_PLAY:
#endif //SERV_PVP_NPC_HUNT_QUEST_EXTEND
			{
				if(g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME)
					bAnswer = true;
				else
					bAnswer = false;
			}
			break;
#ifdef SERV_SKILL_USE_SUBQUEST
		case CX2QuestManager::SQT_SKILL_USE:
			{
				bAnswer = true;
			}
			break;
#endif SERV_SKILL_USE_SUBQUEST
		default:
			bAnswer = false;
			break;
		}
	}
	return bAnswer;
}




bool CX2UIQuestNew::SetUIQuickQuestSlotInfo(int iQuestSlotInfoIndex, QuestInfo& QuestInfo )
{

#ifdef DAY_OF_THE_WEEK_QUEST
	if( false == GetIsValidDayTheWeekQuest( QuestInfo.m_iID ) )
		return false;
#endif // DAY_OF_THE_WEEK_QUEST


	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( QuestInfo.m_iID );
	const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( QuestInfo.m_iID );
#ifdef SERV_IRUHADEV_JUNE_QUEST_DATA
	if( NULL == pQuestTemplet )
	{
		QuestNoTempletLog( QuestInfo.m_iID, L"SetUIQuickQuestSlotInfo" );
		return false;
	}
#endif SERV_IRUHADEV_JUNE_QUEST_DATA


	if(iQuestSlotInfoIndex > _CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest - 1)
	{
		return false;
	}

	if(QuestInfo.m_eQuestState == QSS_COMPLETE_EPIC)
	{
		return false;
	}
	if(QuestInfo.m_eQuestState == QSS_WAIT_EPIC)
	{
		return false;
	}


	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_QuestID = QuestInfo.m_iID;

	if(IsStateQuickQueskFilter())
	{		
		bool bSetQuickQuestSlot = false;
		int iCurrentDungeonID = 0;
		if(g_pData->GetDungeonRoom() != NULL)
		{
			iCurrentDungeonID = g_pData->GetDungeonRoom()->GetDungeonID();
		}
		for(UINT i= 0; i< pQuestTemplet->m_vecSubQuest.size(); i++)
		{
			if(IsStateSubQuestCanClear(pQuestTemplet->m_vecSubQuest[i], iCurrentDungeonID))
			{
				bSetQuickQuestSlot = true;
			}
		}

		if(bSetQuickQuestSlot == false)
		{
			return false;
		}
	}
#ifdef REFORM_QUEST
	// sub-quest
	wstring wstr;
	bool bQuestComplete = true;

	std::map< int, std::vector<int>>::const_iterator mit = pQuestTemplet->m_mapSubQuestGroup.begin();
	for( ; mit != pQuestTemplet->m_mapSubQuestGroup.end(); ++mit)
	{
		//해당 서브퀘스트 그룹에 대한 내용만 보여주기 위해 clear()
		wstr.clear();			

		bool bIsSubquestGroupCompelete = true;
		for( UINT i=0; i<mit->second.size() ; i++ )
		{
			const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( mit->second[i] );
			if( NULL == pSubQuestTemplet )
				continue;

			const CX2QuestManager::ClearCondition& clearCond = pSubQuestTemplet->m_ClearCondition;

			//서브퀘스트 내용
			wstring wstrSubQuestDesc = L"";			
			wstrSubQuestDesc = g_pData->GetQuestManager()->MakeSubQuestString( pSubQuestTemplet->m_bAutomaticDescription, pSubQuestTemplet, pQuestInst );

			if( pQuestInst != NULL && pQuestInst->GetSubQuest(mit->second[i]) == NULL )
			{
				wstringstream wstrstm;
				wstrstm << L"SubQuestID: " << mit->second[i];
				ErrorLogMsg( XEM_ERROR125, wstrstm.str().c_str() );
			}

			if( 0 != wstrSubQuestDesc.length() )
			{
				if(
	#ifndef FIX_TALK_QUEST_COMPLETE_COLOR
					CX2QuestManager::SQT_NPC_TALK != pSubQuestTemplet->m_eClearType &&
	#endif
					NULL != pQuestInst && 
					NULL != pQuestInst->GetSubQuest(mit->second[i]) &&
					true == pQuestInst->GetSubQuest(mit->second[i])->IsComplete() )
				{
					wstr += L"#CFBAF5D";
					wstr += wstrSubQuestDesc;
					wstr += L" (";
					wstr += GET_STRING( STR_ID_759 );
					wstr += L")#CX";
				}
				else
				{
					wstr += wstrSubQuestDesc;
					bQuestComplete = false;
					bIsSubquestGroupCompelete = false;
				}
				wstr += L"\n";
			}
		}

		if( false == bIsSubquestGroupCompelete )
		{//해당 서브퀘스트 그룹의 퀘스트가 모두 완료되지 않으면 내용 그만 보여주기
			break;			 
		}
	} 
#else
	// sub-quest
	wstring wstr;
	bool bQuestComplete = true;
	for( UINT i=0; i<pQuestTemplet->m_vecSubQuest.size(); i++ )
	{
		const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( pQuestTemplet->m_vecSubQuest[i] );
		if( NULL == pSubQuestTemplet )
			continue;

		const CX2QuestManager::ClearCondition& clearCond = pSubQuestTemplet->m_ClearCondition;

		wstring wstrSubQuestDesc = L"";
		wstrSubQuestDesc = g_pData->GetQuestManager()->MakeSubQuestString( pSubQuestTemplet->m_bAutomaticDescription, pSubQuestTemplet, pQuestInst );

		if( pQuestInst != NULL && pQuestInst->GetSubQuest(pQuestTemplet->m_vecSubQuest[i]) == NULL )
		{
			wstringstream wstrstm;
			wstrstm << L"SubQuestID: " << pQuestTemplet->m_vecSubQuest[i];
			ErrorLogMsg( XEM_ERROR125, wstrstm.str().c_str() );
		}

		if( 0 != wstrSubQuestDesc.length() )
		{
			if(
#ifndef FIX_TALK_QUEST_COMPLETE_COLOR
				CX2QuestManager::SQT_NPC_TALK != pSubQuestTemplet->m_eClearType &&
#endif
				NULL != pQuestInst && 
				NULL != pQuestInst->GetSubQuest(pQuestTemplet->m_vecSubQuest[i]) &&
				true == pQuestInst->GetSubQuest(pQuestTemplet->m_vecSubQuest[i])->IsComplete() )
			{
				{
					wstr += L"#CFBAF5D";
					wstr += wstrSubQuestDesc;
					wstr += L" (";
					wstr += GET_STRING( STR_ID_759 );
					wstr += L")#CX";
				}
			}
			else
			{
				wstr += wstrSubQuestDesc;
				bQuestComplete = false;
			}
			wstr += L"\n";
		}
	}
#endif //REFORM_QUEST

	if(bQuestComplete)
	{
		m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(1)->color = D3DXCOLOR(0.9f,0.9f,0.17f,1.0f);
		m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(2)->msg = GET_STRING( STR_ID_759 );
	}
	else
	{
		m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(2)->msg = L"";
	}
#ifdef REFORM_QUEST
	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pButtonComplete->SetEnable( (CX2Main::XS_DUNGEON_GAME != g_pMain->GetNowStateID()) );
	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pButtonComplete->SetShow(bQuestComplete);
#endif //REFORM_QUEST


	wstring wstrTitle = L"";
	if(QuestInfo.m_eQuestType == QTS_EPIC)
	{
		//wstrTitle = L"▶[에픽]" + pQuestTemplet->m_wstrTitle;
		wstrTitle = L"▶";
		wstrTitle += pQuestTemplet->m_wstrTitle;
		m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(1)->color = D3DXCOLOR(0.12f,0.91f,1.f,1.0f);
	}
	else
	{
		wstring wstrRandom = GET_STRING(STR_ID_17273);
		wstring::size_type szTemp = pQuestTemplet->m_wstrTitle.find(wstrRandom);
		if( szTemp != wstring::npos )
		{
			wstring wstrTemp = pQuestTemplet->m_wstrTitle;
			wstrTemp.erase(szTemp, wstrRandom.length() );
			wstrTitle = L"▶" + wstrTemp;
			m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(1)->color = D3DXCOLOR(0.68f,0.95f,0.17f,1.0f);
		}
		else
		{
			wstrTitle = L"▶" + pQuestTemplet->m_wstrTitle;
			m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(1)->color = D3DXCOLOR(0.68f,0.95f,0.17f,1.0f);
		}
	}
#if defined(ELLIPSE_GLOBAL) && defined(CLIENT_COUNTRY_EU) 
	bool bEllipse = false;
	wstrTitle = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(wstrTitle.c_str(), 190, m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->GetString(0)->fontIndex, 1, bEllipse);
#else //defined(ELLIPSE_GLOBAL) && defined(CLIENT_COUNTRY_EU)
	//너비 초과하면 ...으로 대체
	int iTitleWidth = 155;
	if( false == bQuestComplete )
		iTitleWidth += 25;
#ifdef CLIENT_GLOBAL_LINEBREAK
	CWordLineHandler::LineBreakInX2Main( wstrTitle, m_pFontQuickQuestClearCondition, (int)((float)iTitleWidth*g_pKTDXApp->GetResolutionScaleX()), L"", true );
#else //#ifdef CLIENT_GLOBAL_LINEBREAK
	CX2Main::LineBreak( wstrTitle, m_pFontQuickQuestClearCondition, (int)((float)iTitleWidth*g_pKTDXApp->GetResolutionScaleX()), L"", true );
#endif //CLIENT_GLOBAL_LINEBREAK
	wstring::size_type szTemp = wstrTitle.find(L"\n");
	if( szTemp != wstring::npos )
	{
		wstring wstrTemp = pQuestTemplet->m_wstrTitle;
		wstrTitle.erase(szTemp);
		wstrTitle = wstrTitle + L"...";
	}
#endif //defined(ELLIPSE_GLOBAL) && defined(CLIENT_COUNTRY_EU)
	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestTitle->SetString(1, wstrTitle.c_str());

#ifdef CLIENT_GLOBAL_LINEBREAK
	//국가에 맞게 수정
	const int MAGIC_TEXT_WIDTH = 200;
	CWordLineHandler::LineBreakInX2Main( wstr, m_pFontQuickQuestClearCondition, (int)((float)MAGIC_TEXT_WIDTH*g_pKTDXApp->GetResolutionScaleX()), L"", true );
#else //#ifdef CLIENT_GLOBAL_LINEBREAK
	const int MAGIC_TEXT_WIDTH = 190;
	CX2Main::LineBreak( wstr, m_pFontQuickQuestClearCondition, (int)((float)MAGIC_TEXT_WIDTH*g_pKTDXApp->GetResolutionScaleX()), L"", true );
#endif //CLIENT_GLOBAL_LINEBREAK

	float DescHeight = ( m_pFontQuickQuestClearCondition->GetPaddedHeight( wstr.c_str() , 2) / g_pKTDXApp->GetResolutionScaleY() );

	DescHeight -= 4.f;

	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_fExpandSizeY = DescHeight;
	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pStaticQuestInfo->SetString(0, wstr.c_str());

	m_vecUIQuickQuest[iQuestSlotInfoIndex].m_pDLGUIQuickQuest->SetShowEnable(true, true);

	return true;
}
void CX2UIQuestNew::MinimizeAllUIQuickQuest()
{
	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		if(m_vecUIQuickQuest[i].m_bIsExpand)
		{
			MinimizeUIQuickQuest(i);
		}
	}
	return;
}

void CX2UIQuestNew::MinimizeAllUIQuickQuestWithExpandInfo()
{
	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		if(m_vecUIQuickQuest[i].m_bIsExpand)
		{
			MinimizeUIQuickQuest(i, true);
		}
	}
	return;
}

void CX2UIQuestNew::ExpandAllUIQuickQuest()
{
	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		if(!m_vecUIQuickQuest[i].m_bIsExpand)
		{
			ExpandUIQuickQuest(i, false);
		}
	}
	return;
}

void CX2UIQuestNew::ExpandAllUIQuickQuestWithExpandInfo()
{
	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
	{
		if(m_vecUIQuickQuest[i].m_bIsExpand)
		{
			ExpandUIQuickQuest(i, false);
		}
	}
	return;
}

void CX2UIQuestNew::UpdateQuickQuestDLG(bool bResetExpand /*= false*/, bool bUpdateOngoingQuest /*= true*/)
{
	if( true == GetShowQuickQuestDLG() )
	{
#ifdef REFORM_QUEST //EXPAND
		MinimizeAllUIQuickQuest();
#else
		if(bResetExpand)
		{
			MinimizeAllUIQuickQuest();
		}
		else
		{
			MinimizeAllUIQuickQuestWithExpandInfo();
		}
#endif //REFORM_QUEST

		//오현빈 // 2012-10-02 // ResetQuestUI에서 호출되었을 경우에는 UpdateOngoingQuestInfo중복 수행 하지 않도록 인자 추가(bUpdateOngoingQuest)
		if( true == bUpdateOngoingQuest )
		{
			UpdateOngoingQuestInfo();
		}
		for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
		{
			m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetShowEnable(false, false);
		}

		int iSlotIndex = 0;
		for( UINT i=0; i<m_vecQuickQuest.size(); ++i)
		{
			if(SetUIQuickQuestSlotInfo(iSlotIndex, m_vecQuickQuest[i]) == true)
			{
				iSlotIndex++;
			}
			if( iSlotIndex >= _CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest )
			{
				break;
			}
		}
#ifdef GUIDE_QUICK_QUEST_COMPLETE
		m_iShowQuickQuest = iSlotIndex; // 오현빈 // 2012-05-14 // 보여지는 퀵퀘스트 갯수
#endif //GUIDE_QUICK_QUEST_COMPLETE

#ifdef REFORM_QUEST //EXPAND
		ExpandAllUIQuickQuest();
#else
		if(bResetExpand && IsStateQuickQueskFilter())
		{
			ExpandAllUIQuickQuest();
		}
		else
		{
			ExpandAllUIQuickQuestWithExpandInfo();
		}
#endif //REFORM_QUEST
	}

	if(false == m_bShowQuickQuestDLG || false == m_bUserShowQuickQuestDLG)
	{
		for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
		{
			m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetShowEnable(false, false);
		}
	}
	g_pMain->GetInformerManager()->GetQuestInformer()->SetNeedCheckInfo( true );

	return;
}

void CX2UIQuestNew::SetShowQuickQuestDLG( bool forOpen )
{
	m_bShowQuickQuestDLG = forOpen;

	if(forOpen == true)
	{
		UpdateQuickQuestDLG();
	}
	else
	{
		for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; i++)
		{
			m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->SetShowEnable(false, false);
		}
		ShowUIDesc(false);
	}
	return;
}

void CX2UIQuestNew::UserShowQuickQuestDLG( bool forOpen )
{
	m_bUserShowQuickQuestDLG = forOpen;

	m_pCheckListQuickQuest->SetChecked(m_bUserShowQuickQuestDLG);
	m_pCheckDetailQuickQuest->SetChecked(m_bUserShowQuickQuestDLG);
	UpdateQuickQuestDLG();
	return;
}

void CX2UIQuestNew::InitUserShowQuickQuestDLG( bool forOpen )
{
	m_bUserShowQuickQuestDLG = forOpen;
	m_pCheckListQuickQuest->SetChecked(m_bUserShowQuickQuestDLG);
	m_pCheckDetailQuickQuest->SetChecked(m_bUserShowQuickQuestDLG);
	return;
}



void CX2UIQuestNew::ReturnToQuestListUI()
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323ca520: back to the tab the list was last on ( scenario / event / guild / story ), anything else is the request tab.
	switch( m_eLastQuestUIMode )
	{
	case QUM_EPIC_QUEST:
	case QUM_EVENT_QUEST:
	case QUM_GUILD_QUEST:
	case QUM_STORY_QUEST:
		SetNowQuestUIMode( m_eLastQuestUIMode );
		break;
	default:
		SetNowQuestUIMode( QUM_QUEST );
		break;
	}
#else
	if( QUM_EPIC_QUEST == m_eLastQuestUIMode)
	{
		SetNowQuestUIMode(QUM_EPIC_QUEST);
	}
	else if( QUM_EVENT_QUEST == m_eLastQuestUIMode)
	{
		SetNowQuestUIMode(QUM_EVENT_QUEST);
	}
#ifdef ACCEPT_QUEST_TAB_FIX
	else if( QUM_EVENT_QUEST_RECEIVE == m_eLastQuestUIMode)
	{
		SetNowQuestUIMode(QUM_EVENT_QUEST);
	}
#endif // ACCEPT_QUEST_TAB_FIX
	else
	{
		SetNowQuestUIMode(QUM_QUEST);
	}
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}

void CX2UIQuestNew::SetTalkingQuestNPC(bool bVal, CX2UnitManager::NPC_UNIT_ID NpcID )
{
	m_bIsTalkingQuestNPC = bVal;
	
	m_eNPCID = NpcID;
	if(m_bIsTalkingQuestNPC && m_eNPCID != CX2UnitManager::NUI_NONE)
	{
		UpdateAvailQuestInfo();
		if(m_vecAvailQuestInfo.size() != 0)
		{
			m_bAvailableQuestNPCHas = true;
		}
		else
		{
			m_bAvailableQuestNPCHas = false;
		}
	}
	else
	{
		m_bAvailableQuestNPCHas = false;
	}
	return;
}


bool CX2UIQuestNew::IsTalkingQuestNPC()
{
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
	return true;		// June ResetQuestUI 323dfa80 / TALK ACK never ask: the request tab always lists the board's quests
#endif SERV_IRUHADEV_JUNE_QUEST_UI
#ifdef MODIFY_ACCEPT_QUEST
	return true;
#endif // MODIFY_ACCEPT_QUEST

	if(m_bIsTalkingQuestNPC && m_bAvailableQuestNPCHas)
	{
		return true;
	}
	return false;
}



void CX2UIQuestNew::SetNowQuestUIMode(QUEST_UI_MODE _eNowQuestUIMode)
{ 
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
/// June 323c9e80: set the mode and check the matching tab radio. Mode 3 ( detail ) has no radio.
	m_eNowQuestUIMode = _eNowQuestUIMode;

	if( NULL == m_pDLGUIQuestList )
		return;

	const WCHAR* wszRadio = NULL;
	switch( _eNowQuestUIMode )
	{
	case QUM_QUEST:
	case QUM_QUEST_RECEIVE:	wszRadio = L"RadioButton_Request";	break;
	case QUM_EPIC_QUEST:	wszRadio = L"RadioButton_Scenario";	break;
	case QUM_EVENT_QUEST:	wszRadio = L"RadioButton_Event";	break;
	case QUM_GUILD_QUEST:	wszRadio = L"RadioButton_Guild";	break;
	case QUM_STORY_QUEST:	wszRadio = L"RaidoButton_Story";	break;		// June's own spelling of the control name
	default:
		return;
	}

	CKTDGUIRadioButton* pRadio = (CKTDGUIRadioButton*) m_pDLGUIQuestList->GetControl( wszRadio );
	if( NULL != pRadio )
		pRadio->SetChecked( true, false, false );
#else
	m_eNowQuestUIMode = _eNowQuestUIMode;

	if( NULL != m_pDLGUIQuestList )
	{
		if(_eNowQuestUIMode == QUM_QUEST || _eNowQuestUIMode == QUM_QUEST_RECEIVE)
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) m_pDLGUIQuestList->GetControl( L"RadioButton_Request" );
			pRadioButton->SetChecked(true);
		}
		else if(_eNowQuestUIMode == QUM_EPIC_QUEST)
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) m_pDLGUIQuestList->GetControl( L"RadioButton_Scenario" );
			pRadioButton->SetChecked(true);
		}
#ifdef REFORM_QUEST
		else if(_eNowQuestUIMode == QUM_EPIC_QUEST)
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*) m_pDLGUIQuestList->GetControl( L"RadioButton_Event" );
			pRadioButton->SetChecked(true);
		}
#endif //REFORM_QUEST
	}
	return;
#endif SERV_IRUHADEV_JUNE_QUEST_UI
}



void CX2UIQuestNew::NewEpicStartEffect()
{


	CKTDGParticleSystem::CParticleEventSequenceHandle m_Seq = 

	g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL, L"UI_NewEpic_StartEffect", 512, 433, 0, 9999, 9999, -1, 1 );

	//g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL, L"UI_NewEpic_StartEffect", vecEffectPos.x, vecEffectPos.y, 0 );
	CKTDGParticleSystem::CParticleEventSequence* pParticle = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_Seq );
	if( pParticle != NULL )
	{
		pParticle->SetOverUI( true );
		//pParticle->SetResolutionConvert(true);
	}
	return;
}

#ifdef REFORM_QUEST

void CX2UIQuestNew::SetOpenEpicCategory()
{
	//적정레벨 에픽퀘스트의 카테고리를 열어주기 위해 적정레벨 퀘스트 ID를 설정
	if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		int iLevel = g_pData->GetMyUser()->GetSelectUnit()->GetPrevLevel();
		for(UINT i=0; i< m_vecEpicQuestInfo.size(); i++)
		{
			if( 0 != m_vecEpicQuestInfo[i].m_iPlayLevel && iLevel >= m_vecEpicQuestInfo[i].m_iPlayLevel )
			{
				m_iOpenEpicCategory	= m_vecEpicQuestInfo[i].m_iEpicGroupID;
				break;
			}			
		}
	}
}
void CX2UIQuestNew::PlayQuestCompleteSound( bool bIsEverySubQuestInQuestComplete_ )
{
	// 퀘스트 내의 모든 서브 퀘스트를 완료 했을 때 사운드 출력
	if( bIsEverySubQuestInQuestComplete_ )
	{
		wstring strQuestCompleteSound ;

		switch( g_pData->GetMyUser()->GetSelectUnit()->GetType() )
		{
		case CX2Unit::UT_ELSWORD:
			strQuestCompleteSound = L"QuestComplete_Elsword.ogg";
			break;
		case CX2Unit::UT_ARME:
			strQuestCompleteSound = L"QuestComplete_Aisha.ogg";
			break;
		case CX2Unit::UT_LIRE:
			strQuestCompleteSound = L"QuestComplete_Lena.ogg";
			break;
		case CX2Unit::UT_RAVEN:
			strQuestCompleteSound = L"QuestComplete_Raven.ogg";
			break;
		case CX2Unit::UT_EVE:
			strQuestCompleteSound = L"QuestComplete_Eve.ogg";
			break;
		case CX2Unit::UT_CHUNG:
			strQuestCompleteSound = L"QuestComplete_Chung.ogg";
			break;
		case CX2Unit::UT_ARA:
			strQuestCompleteSound = L"QuestComplete_Ara.ogg";
			break;
#ifdef NEW_CHARACTER_EL
		case CX2Unit::UT_ELESIS:
			strQuestCompleteSound = L"QuestComplete_Elesis.ogg";
			break;
#endif // NEW_CHARACTER_EL
#ifdef SERV_9TH_NEW_CHARACTER // 김태환 ( 캐릭터 추가용 )
		case CX2Unit::UT_ADD:		/// 변경이 필요 하다.
			strQuestCompleteSound = L"QuestComplete_Add.ogg";
			break;
#endif //SERV_9TH_NEW_CHARACTER

		default:
			ASSERT( !L"Unit Type Is Invalid" );
			break;
		}

		if ( !strQuestCompleteSound.empty() )
			g_pKTDXApp->GetDeviceManager()->PlaySound( strQuestCompleteSound.c_str() , false, false );
	}
	g_pKTDXApp->GetDeviceManager()->PlaySound( L"UI_quickQuest_check.ogg" , false, false );
}
void CX2UIQuestNew::NotificationByQuickQuestInfoChange( const int iQuestID_, bool bIsSubQuestComplete_)
{
	PlayQuestCompleteSound(bIsSubQuestComplete_);

#ifdef GUIDE_FIRST_EPIC_QUEST
	if( NULL != g_pData->GetPlayGuide() )
		g_pData->GetPlayGuide()->SetShowFirstEpicQuestGuide(false);
#endif //GUIDE_FIRST_EPIC_QUEST

#ifdef REFORM_NOVICE_GUIDE
	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() 
		&& true == g_pData->GetPlayGuide()->GetActiveNoviceGuide() )
	{
		g_pData->GetPlayGuide()->ResetNoviceGuide();
	}
#endif //REFORM_NOVICE_GUIDE

	if( false == g_pData->GetUIManager()->GetShowUI() )
		return;

	if( false == GetShowQuickQuestDLG() )
		return;

	for(UINT i=0; i<_CONST_UIQUESTNEW_INFO_::g_iMaxNumOngoingQuest; ++i)
	{
		if( iQuestID_ == m_vecUIQuickQuest[i].m_QuestID )
		{
			D3DXVECTOR2 vPos = m_vecUIQuickQuest[i].m_pDLGUIQuickQuest->GetPos();
			vPos.x = (vPos.x + 96.f) * g_pKTDXApp->GetResolutionScaleX();
			if( true == m_vecUIQuickQuest[i].m_bIsExpand )
			{
				vPos.y = (vPos.y + 30.f ) * g_pKTDXApp->GetResolutionScaleY();
			}
			else
			{
				vPos.y = (vPos.y + 10.f ) * g_pKTDXApp->GetResolutionScaleY();
			}			

			CKTDGParticleSystem::CParticleEventSequence* pParticle = NULL;
			pParticle = g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"UI_quickQuest_complete_P01",
				vPos.x, vPos.y, 0);
			if( NULL != pParticle )
			{
				pParticle->SetOverUI( true );
			}		
			pParticle = g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"UI_quickQuest_complete_P02",
				vPos.x, vPos.y, 0);
			if( NULL != pParticle )
			{
				pParticle->SetOverUI( true );
			}		


			if( false == bIsSubQuestComplete_ && true == m_vecUIQuickQuest[i].m_bIsExpand )
			{
				pParticle = g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"UI_quickQuest_complete_P03",
					vPos.x, vPos.y, 0);

				if( NULL != pParticle )
				{
					pParticle->SetOverUI( true );
					m_hQuestCompleteParticle = pParticle->GetHandle();
				}		
			}
			break;
		}
	}

}
#endif //REFORM_QUEST

#ifdef SERV_IRUHADEV_JUNE_QUEST_UI
#ifndef SERV_IRUHADEV_OFFLINE
#error SERV_IRUHADEV_JUNE_QUEST_UI reads CX2QuestManager::QuestTemplet::m_vecAfterQuestID ( SERV_IRUHADEV_OFFLINE )
#endif SERV_IRUHADEV_OFFLINE
#ifndef SERV_IRUHADEV_JUNE_QUEST_LOAD
#error SERV_IRUHADEV_JUNE_QUEST_UI needs SERV_IRUHADEV_JUNE_QUEST_LOAD ( June's quest enums, Quest_Event_Scene_Info, the story index )
#endif SERV_IRUHADEV_JUNE_QUEST_LOAD



// ---------------------------------------------------------------------------------------------------------------------
// ctor hook, tab modes
// ---------------------------------------------------------------------------------------------------------------------

void CX2UIQuestNew::InitMembers()
{
	m_pButtonSkipEpicQuest		= NULL;
	m_bIsOpenGuildCategory		= true;		// June ctor: +0x720 = 1
	m_iOpenStoryCategory		= -1;		// +0x75c
	m_bCompleteQuestInStoryTab	= false;	// +0x758
	m_bOngoingQuestInStoryTab	= false;	// +0x759
	m_iCompleteStoryCategory	= -1;		// +0x760
	m_iOngoingStoryCategory		= -1;		// +0x764
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	m_iMovieStartID			= 0;		// June ctor 323d121d-323d122f: +0x770 / +0x774 / +0x778 / +0x77c = 0
	m_bMovieTab				= false;
	m_iMovieCount			= 0;
	m_iMovieNaviStep		= 0;
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
}




// ---------------------------------------------------------------------------------------------------------------------
// the lists
// ---------------------------------------------------------------------------------------------------------------------

bool CX2UIQuestNew::GetCompleteQuestOfType( std::vector<int>& vecQuestID, int iQuestType )
{
	vecQuestID.clear();

	CX2QuestManager* pQM = g_pData->GetQuestManager();
	const map<int,int> mapComplete = pQM->GetMapCompleteQuest();
	if( true == mapComplete.empty() )
		return false;

	map<int,int>::const_iterator it;
	for( it = mapComplete.begin(); it != mapComplete.end(); ++it )
	{
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( it->first );
		if( NULL != pT && iQuestType == pT->m_eQuestType )
			vecQuestID.push_back( it->first );
	}
	return true;
}

/// June 323cab40: may this ( story ) quest be started now - not forbidden, every required quest done, level in range, the item held,
/// and the not-before quest not done.
bool CX2UIQuestNew::IsQuestStartable( int iQuestID )
{
	CX2QuestManager* pQM = g_pData->GetQuestManager();
	const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( iQuestID );
	if( NULL == pT )
		return false;
	if( true == pQM->IsForbiddenQuest( iQuestID ) )
		return false;

	for( UINT i = 0; i < pT->m_Condition.m_vecBeforeQuestID.size(); ++i )
	{
		const int iBefore = pT->m_Condition.m_vecBeforeQuestID[i];
		if( 0 != iBefore && false == pQM->GetUnitCompleteQuest( iBefore ) )
			return false;
	}

	const int iLevel = g_pData->GetSelectUnitLevel();
	if( 0 != pT->m_Condition.m_iLevel && pT->m_Condition.m_iLevel > iLevel )
		return false;
	if( -1 != pT->m_Condition.m_iLimitLevel && iLevel > pT->m_Condition.m_iLimitLevel )
		return false;

	if( -1 != pT->m_Condition.m_iConditionItemID && pT->m_Condition.m_iConditionItemNum >= 1 )
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
		if( NULL == pUnit || pUnit->GetInventory().GetNumItemByTID( pT->m_Condition.m_iConditionItemID ) < pT->m_Condition.m_iConditionItemNum )
			return false;
	}

	if( -1 != pT->m_Condition.m_iNotBeforeQuestID && true == pQM->GetUnitCompleteQuest( pT->m_Condition.m_iNotBeforeQuestID ) )
		return false;

	return true;
}

/// June 323d4910: the first quest of every story group is listed when it can be started and the unit is in a village / field.
void CX2UIQuestNew::AddFirstStoryQuests( std::vector<int>& vecOngoing, const std::vector<int>& vecCompleteStory )
{
	CX2QuestManager* pQM = g_pData->GetQuestManager();
	const std::map< int, std::set<int> > mapStory = pQM->GetStoryQuestMap();

	std::map< int, std::set<int> >::const_iterator it;
	for( it = mapStory.begin(); it != mapStory.end(); ++it )
	{
		if( true == it->second.empty() )
			continue;

		const int iFirst = *it->second.begin();
		if( 0 == iFirst )
			continue;
		if( vecCompleteStory.end() != std::find( vecCompleteStory.begin(), vecCompleteStory.end(), iFirst ) )
			continue;
		if( vecOngoing.end() != std::find( vecOngoing.begin(), vecOngoing.end(), iFirst ) )
			continue;

		if( false == IsQuestStartable( iFirst ) )
			continue;

		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( iFirst );
		if( NULL != pT && false == pT->m_bIsTimeEvent && true == IsVillageOrBattleFieldState() )
			vecOngoing.push_back( iFirst );
	}
}

/// June 323d4ae0: after a completed story quest that still has a next quest, the next quests that are not in progress are listed.
/// A story quest that is the last of its group has no next listed. ( June does not drop duplicates here; neither does this. )
void CX2UIQuestNew::AddNextStoryQuests( std::vector<int>& vecOngoing, const std::vector<int>& vecCompleteStory )
{
	CX2QuestManager* pQM = g_pData->GetQuestManager();
	const std::map< int, std::set<int> > mapStory = pQM->GetStoryQuestMap();

	const std::set<int> setDone( vecCompleteStory.begin(), vecCompleteStory.end() );
	std::set<int> setOpen;

	std::set<int>::const_iterator it;
	for( it = setDone.begin(); it != setDone.end(); ++it )
	{
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( *it );
		if( NULL == pT )
			continue;

		for( UINT i = 0; i < pT->m_vecAfterQuestID.size(); ++i )
		{
			if( setDone.end() != setDone.find( pT->m_vecAfterQuestID[i] ) )
				continue;

			std::map< int, std::set<int> >::const_iterator git = mapStory.find( pT->m_iEpisodeGroupID );
			if( git != mapStory.end() && false == git->second.empty() && *it != *git->second.rbegin() )
				setOpen.insert( *it );
		}
	}

	for( it = setOpen.begin(); it != setOpen.end(); ++it )
	{
		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( *it );
		if( NULL == pT || CX2QuestManager::QT_STORY != pT->m_eQuestType )
			continue;

		for( UINT i = 0; i < pT->m_vecAfterQuestID.size(); ++i )
		{
			const int iNext = pT->m_vecAfterQuestID[i];
			if( NULL != pQM->GetUnitQuest( iNext ) )
				continue;

			const CX2QuestManager::QuestTemplet* pNext = pQM->GetQuestTemplet( iNext );
			if( NULL != pNext && CX2QuestManager::QT_STORY == pNext->m_eQuestType && false == pNext->m_bIsTimeEvent )
				vecOngoing.push_back( iNext );
		}
	}
}

/// June 323ca300: the story group that opens first: the first listed quest the unit's level reaches ( fair level, not the epic's play level )
void CX2UIQuestNew::SetOpenStoryCategory()
{
	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	const int iLevel = g_pData->GetMyUser()->GetSelectUnit()->GetPrevLevel();
	for( UINT i = 0; i < m_vecOnGoingStoryQuestInfo.size(); ++i )
	{
		if( 0 != m_vecOnGoingStoryQuestInfo[i].m_iFairLevel && iLevel >= m_vecOnGoingStoryQuestInfo[i].m_iFairLevel )
		{
			m_iOpenStoryCategory = m_vecOnGoingStoryQuestInfo[i].m_iEpicGroupID;
			return;
		}
	}
}


void CX2UIQuestNew::FillQuestSlots()
{
	m_iSizeQuestInfoList = (int)m_vecQuestInfoList.size();

	if( m_iTopQuestSlotIndex > m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage )
		m_iTopQuestSlotIndex = m_iSizeQuestInfoList - (int)_CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage;
	if( m_iTopQuestSlotIndex < 0 )
		m_iTopQuestSlotIndex = 0;

	for( UINT i = 0; i < _CONST_UIQUESTNEW_INFO_::g_iNumQuestSlotPerPage; ++i )
	{
		if( i + m_iTopQuestSlotIndex >= m_vecQuestInfoList.size() )
			break;

		if( true == m_vecQuestInfoList[m_iTopQuestSlotIndex + i].m_bIsCategory )
			SetUIQuestSlotInfoToCategory( i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i] );
		else
			UpdateUIQuestSlotInfo( i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i] );
	}
}


/// June 323ca6d0: hide the scenario-movie tab's controls ( DLG_UI_NEW_Quest_List.lua builds 12 Button_Movie_N / NewTexture_N,
/// g_pStatic_HorizonLine, Village and RadioButton_Area; the tab itself is Q8 ). Without this they draw over every quest tab.
void CX2UIQuestNew::HideMovieTab()
{
	if( NULL == m_pDLGUIQuestList )
		return;
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	m_bMovieTab = false;			// June 323ca716: +0x774 = 0 ( quest-system Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB

	int iMovieCount = m_pDLGUIQuestList->GetDummyInt( 4 );		// Lua AddDummyInt( 12 ), June reads dummy int #4
	for( int i = 1; i <= iMovieCount; ++i )
	{
		WCHAR wszName[256];
		wsprintf( wszName, L"Button_Movie_%d", i );
		CKTDGUIControl* pControl = m_pDLGUIQuestList->GetControl( wszName );
		if( NULL != pControl )
			pControl->SetShowEnable( false, false );

		wsprintf( wszName, L"NewTexture_%d", i );
		pControl = m_pDLGUIQuestList->GetControl( wszName );
		if( NULL != pControl )
			pControl->SetShowEnable( false, false );
	}

	CKTDGUIControl* pControl = m_pDLGUIQuestList->GetControl( L"g_pStatic_HorizonLine" );
	if( NULL != pControl )
		pControl->SetShowEnable( false, false );

	CKTDGUIRadioButton* pArea = (CKTDGUIRadioButton*) m_pDLGUIQuestList->GetControl( L"RadioButton_Area" );
	if( NULL != pArea )
	{
		pArea->SetShowEnable( false, false );
		pArea->SetChecked( true, false, false );		// June 323ca81f: checked in its own group 3, so no tab changes
	}

	pControl = m_pDLGUIQuestList->GetControl( L"Village" );
	if( NULL != pControl )
		pControl->SetShowEnable( false, false );
}

void CX2UIQuestNew::SelectTab( QUEST_UI_MODE eMode )
{
	QuestUILog( L"[JUNE_QUEST] UI tab %d", (int)eMode );
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	m_bMovieTab = false;			// June @323e1dd0: the Guild / Story tabs close the Video tab before the reset ( Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	SetNowQuestUIMode( eMode );
	m_iTopQuestSlotIndex = 0;
	ResetQuestUI();
}

/// June 323e1bf0 case 0xa055: Button_Skip_Epic_Quest asks first
void CX2UIQuestNew::OpenSkipEpicMsgBox()
{
	QuestUILog( L"[JUNE_QUEST] UI skip epic quest %d: ask", m_iSelectedQuestID );

	if( NULL != m_pDLGMsgBox )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
		m_pDLGMsgBox = NULL;
	}
	m_pDLGMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( 43475 ), UQNCM_SKIP_EPIC_QUEST_OK, g_pMain->GetNowState(), -1 );
}

/// June 323e1bf0 case 0xa056: the skip is a plain QUEST_COMPLETE_REQ for the selected quest with no chosen items ( 323d24f0 ).
/// Whether the server completes a quest whose sub-quests are not done is the server's rule ( the offline emulator: Q4 ).
void CX2UIQuestNew::SkipEpicQuestOk()
{
	if( NULL != m_pDLGMsgBox )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
		m_pDLGMsgBox = NULL;
	}

	if( NULL != g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID ) )
	{
		QuestUILog( L"[JUNE_QUEST] UI skip epic quest %d: complete request", m_iSelectedQuestID );
		std::map<int,int> mapNoSelectedItem;
		Handler_EGS_QUEST_COMPLETE_REQ( m_eNPCID, m_iSelectedQuestID, mapNoSelectedItem );
#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
		HideMovieTab();				// June @323e44f8 ( Q8b )
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	}
}

// ---------------------------------------------------------------------------------------------------------------------
// quest scenes ( quest-system Q5b, SERV_IRUHADEV_JUNE_QUEST_SCENE ): June 323cb2e0 / 323cb190 on june-gfx G3's CX2GFXEventScene
// ---------------------------------------------------------------------------------------------------------------------
#ifdef SERV_IRUHADEV_JUNE_QUEST_SCENE
#ifndef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
#error SERV_IRUHADEV_JUNE_QUEST_SCENE needs SERV_IRUHADEV_JUNE_GFX_EVENTSCENE ( CX2GFXEventScene )
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE

#include "X2GFXEventScene.h"

/// [JUNE_QUEST] SCENE lines are few ( one per scene call ), but a quest list refresh must not flood the log: the first 300 are written.
static bool QuestSceneLogOk()
{
	static int s_iLines = 0;
	if( s_iLines >= 300 )
		return false;
	++s_iLines;
	return true;
}

/// June 323cb190( quest, out start, out end, out start ment, out end ment, out start end frame, out end end frame ): the quest's Quest_Event_Scene_Info row
/// ( g_pData +0x498, here the quest manager's map ). The DEFAULT_* values go out first; then the selected unit's row ( the unit TYPE: the first dword of
/// the unit templet, @323cb1b1 ) overrides the start values when its start scene is non-empty and the end values when its end scene is.
/// False when the quest has no row, or no selected unit ( @323cb1a4 ).
static bool GetQuestEventSceneInfo( int iQuestID, std::wstring& wstrStart, std::wstring& wstrEnd, std::vector<int>& vecStartMent,
	std::vector<int>& vecEndMent, int& iStartEndFrame, int& iEndEndFrame )
{
	if( NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	const CX2Unit::UnitTemplet* pUnitTemplet = g_pData->GetMyUser()->GetSelectUnit()->GetUnitTemplet();
	const int iUnitType = ( NULL != pUnitTemplet ) ? static_cast<int>( pUnitTemplet->m_UnitType ) : 0;

	const std::map<int, CX2QuestManager::QuestEventSceneInfo>& mapInfo = g_pData->GetQuestManager()->GetQuestEventSceneInfoMap();
	std::map<int, CX2QuestManager::QuestEventSceneInfo>::const_iterator itInfo = mapInfo.find( iQuestID );
	if( mapInfo.end() == itInfo )
		return false;

	const CX2QuestManager::QuestEventSceneInfo& kInfo = itInfo->second;
	wstrStart		= kInfo.m_wstrDefaultStartScene;
	wstrEnd			= kInfo.m_wstrDefaultEndScene;
	vecStartMent	= kInfo.m_vecDefaultStartSceneMent;
	vecEndMent		= kInfo.m_vecDefaultEndSceneMent;
	iStartEndFrame	= kInfo.m_iDefaultStartSceneEndFrame;
	iEndEndFrame	= kInfo.m_iDefaultEndSceneEndFrame;

	std::map<int, CX2QuestManager::QuestEventSceneChar>::const_iterator itChar = kInfo.m_mapChar.find( iUnitType );
	if( kInfo.m_mapChar.end() != itChar )
	{
		const CX2QuestManager::QuestEventSceneChar& kChar = itChar->second;
		if( false == kChar.m_wstrStartScene.empty() )
		{
			wstrStart		= kChar.m_wstrStartScene;
			vecStartMent	= kChar.m_vecStartSceneMent;
			iStartEndFrame	= kChar.m_iStartSceneEndFrame;
		}
		if( false == kChar.m_wstrEndScene.empty() )
		{
			wstrEnd			= kChar.m_wstrEndScene;
			vecEndMent		= kChar.m_vecEndSceneMent;
			iEndEndFrame	= kChar.m_iEndSceneEndFrame;
		}
	}
	return true;
}

/// June 323cb2e0( quest, mode ): 0 the start scene, 1 the end scene and every additional scene quest's start and end, 2 both ( the Remind button ).
/// Each scene is registered ( its ment ids, 3226ac80 ) and played ( 3226b130( name, end frame, 0 ) ); a scene with a BGM name starts the quest BGM
/// ( 3232df20 ). The quest's Lua scenes ( m_wstrStartScene / m_wstrEndScene ) are not played: June never reads them, so a quest with no row plays nothing.
void CX2UIQuestNew::PlayQuestScene( int iQuestID, int iMode )
{
	// @323cb31f..323cb339: whatever follows, the deferred scene is cleared first
	if( NULL != g_pInstanceData )
		g_pInstanceData->SetDeferredQuestScene( 0, 0 );

	if( NULL == g_pData || NULL == g_pData->GetQuestManager() || NULL == g_pData->GetGFXEventScene() )
		return;

	// @323cb342..323cb34b
	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( iQuestID );
	if( NULL == pQuestTemplet )
		return;

	CX2GFXEventScene* pScene = g_pData->GetGFXEventScene();
	CX2QuestManager* pQuestManager = g_pData->GetQuestManager();

	std::wstring		wstrStart;
	std::wstring		wstrEnd;
	std::vector<int>	vecStartMent;
	std::vector<int>	vecEndMent;
	int					iStartFrame = -1;
	int					iEndFrame = -1;

	if( true == GetQuestEventSceneInfo( iQuestID, wstrStart, wstrEnd, vecStartMent, vecEndMent, iStartFrame, iEndFrame ) )
	{
		if( true == QuestSceneLogOk() )
			CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE quest %d mode %d: row start '%s' frame %d end '%s' frame %d", iQuestID, iMode, wstrStart.c_str(), iStartFrame, wstrEnd.c_str(), iEndFrame );

		if( 0 == iMode || 2 == iMode )		// @323cb3e6..323cb4a2
		{
			if( false == wstrStart.empty() )
			{
				pScene->RegisterMent( wstrStart, vecStartMent );
				pScene->Play( wstrStart, iStartFrame, 0 );
				pQuestManager->PlayQuestSelectBGM( pQuestTemplet->m_wstrSelectBGM );
			}
		}
		if( 1 == iMode || 2 == iMode )		// @323cb4a7..323cb55e
		{
			if( false == wstrEnd.empty() )
			{
				pScene->RegisterMent( wstrEnd, vecEndMent );
				pScene->Play( wstrEnd, iEndFrame, 0 );
				pQuestManager->PlayQuestSelectBGM( pQuestTemplet->m_wstrSelectBGM );
			}
		}
	}
	else
	{
		if( true == QuestSceneLogOk() )
			CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE quest %d mode %d: no Quest_Event_Scene_Info row, no scene plays ( the Lua scenes start '%s' end '%s' are not played: June has no Lua fallback )",
				iQuestID, iMode, pQuestTemplet->m_wstrStartScene.c_str(), pQuestTemplet->m_wstrEndScene.c_str() );
	}

	if( 1 == iMode || 2 == iMode )			// @323cb563..323cb771: the additional scene quests ( 32079c50 copies the vector )
	{
		const std::vector<int> vecAdditional = pQuestTemplet->m_vecAdditionalSceneQuestID;
		for( std::vector<int>::const_iterator it = vecAdditional.begin(); it != vecAdditional.end(); ++it )
		{
			// @323cb5a3..323cb5fd: names and ment vectors start empty for each; the end frames are overwritten by a row
			wstrStart.clear();
			wstrEnd.clear();
			vecStartMent.clear();
			vecEndMent.clear();

			if( true == GetQuestEventSceneInfo( *it, wstrStart, wstrEnd, vecStartMent, vecEndMent, iStartFrame, iEndFrame ) )
			{
				if( true == QuestSceneLogOk() )
					CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE quest %d mode %d: additional quest %d row start '%s' end '%s'", iQuestID, iMode, *it, wstrStart.c_str(), wstrEnd.c_str() );

				// both are registered, then both are played ( an empty name is dropped by Play ), then the BGM
				pScene->RegisterMent( wstrStart, vecStartMent );
				pScene->RegisterMent( wstrEnd, vecEndMent );
				pScene->Play( wstrStart, iStartFrame, 0 );
				pScene->Play( wstrEnd, iEndFrame, 0 );
				pQuestManager->PlayQuestSelectBGM( pQuestTemplet->m_wstrSelectBGM );
			}
		}
	}
}

/// June StateChange 31d88bf0 @31d896bd..31d896fb, at the end of the village and the battle field blocks: the scene NEW_QUEST_NOT deferred in state 5.
/// June skips it while a tutorial guide runs ( 31fbda90 on g_pMain +0x8c4 ): March has no tutorial-guide manager ( Stage 9 ), so it is never skipped.
void CX2UIQuestNew::PlayDeferredQuestScene()
{
	if( NULL == g_pInstanceData || NULL == g_pData || NULL == g_pData->GetUIManager() )
		return;

	const int iQuestID = g_pInstanceData->GetDeferredSceneQuestID();
	const int iMode = g_pInstanceData->GetDeferredSceneMode();
	if( iQuestID <= 0 )
		return;

	CX2UIQuestNew* pUIQuestNew = g_pData->GetUIManager()->GetUIQuestNew();
	if( NULL == pUIQuestNew )
		return;

	if( true == QuestSceneLogOk() )
		CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE deferred scene of quest %d ( mode %d ) plays now", iQuestID, iMode );
	pUIQuestNew->PlayQuestScene( iQuestID, iMode );
}
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE

#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
// quest-system Q8b - the quest window's Video tab ( RadioButton_Movie in DLG_UI_NEW_Quest_List.lua ) on Q8's movie table ( CX2Main ).

/// June 323e1bf0 case 41038 ( @323e3cb7 ): the Video tab. Refused with "Cannot be used here." ( string 4495 at ( 250, 300 ), the case
/// returns false ) in a boss-raid battle field and in the dungeon game. Otherwise the quest rows ( 323c8820 ) and reward slots ( 323c9200 )
/// are cleared, the list starts at movie 1 and the tab is open ( +0x770 = 1, +0x774 = 1, 323dfc00( true ) ).
bool CX2UIQuestNew::OpenMovieTab()
{
	bool bRefuse = false;

	// 32481fa0 ( g_pData +0x124 ) comes first and is true only in June's boss-raid fields 40033 / 40034 / 40039-40044 ( 3266cdc0 ). March has
	// none of those fields, so it is never true here and is left out ( as Skill S7o left it; owner june-dungeons C3z-8 ).
	if( CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() )
	{
		// 32482650: the current field's type ( June battle-field data +0x9c ) == 2, re-expressed as March's boss-raid test as X2DamageEffect and
		// X2EmblemManager do for the same function; March keeps a bool, not June's type, so the two are not proven equal
		bRefuse = g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField();
	}
	else if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
	{
		// June states 0xb ( XS_DUNGEON_GAME ), 0x17 ( XS_PVE_GAME ) and 0x1a ( XS_PHOTOZONE_GAME ); the last two have no March state
		bRefuse = true;
	}

	if( true == bRefuse )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] MOVIE tab refused in state %d ( string 4495 )", (int)g_pMain->GetNowStateID() );
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_4495 ), g_pMain->GetNowState() );
		return false;
	}

	InitUIQuestSlotInfo();		// June 323c8820
	InitRewardItemSlot();		// June 323c9200
	m_iMovieStartID	= 1;
	m_bMovieTab		= true;
	CX2OfflineLog::Server( L"[JUNE_QUEST] MOVIE tab open: %d movie row(s), %d navi step(s)", m_iMovieCount, m_iMovieNaviStep );
	UpdateMovieList( true );
	return true;
}

/// June 323dfc00 ( bool update navi ): the 13 Button_Movie_N show movie ids m_iJuneMovieStartID on ( id 10 is skipped for 11 when code
/// event 2260 or 2261 is on; offline it never is ). The button's dummy int 0 becomes the id. A seen movie gets its row's movie texture
/// ( ScenarioMovie_01 if empty ), "<key>_Overlay" for over / down, no description and clicks; its NewTexture_N shows ( to the front,
/// flickering ) while it is still new. A movie not seen gets Overlay_Lock, no clicks and a description: string 31168 without a movie file,
/// else the lock text ( 323ddb00 ); with no row the button hides. Then g_pStatic_HorizonLine shows one line per row of three and Village one
/// icon + area name per shown movie ( the row's icon texture and m_dungeonName ), the rest hidden and blanked.
void CX2UIQuestNew::UpdateMovieList( bool bUpdateNavi )
{
	if( NULL == m_pDLGUIQuestList || NULL == g_pMain )
		return;

	if( true == bUpdateNavi )
		UpdateNavi();		// June 323c8da0

	const WCHAR* wszDefaultTex = L"DLG_UI_Common_Texture_ScenarioMovie_01.tga";
	int iMovieID = m_iMovieStartID;
	int iSkipped = 0;
	int iSeen = 0, iLocked = 0, iHidden = 0;
	WCHAR wszName[256] = {0,};

	for( int iSlot = 1; iSlot <= 13; ++iSlot )
	{
		wsprintf( wszName, L"NewTexture_%d", iSlot );
		CKTDGUIStatic* pNew = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( wszName );
		if( NULL != pNew && NULL != pNew->GetPicture( 0 ) )
			pNew->SetShowEnable( false, false );

		wsprintf( wszName, L"Button_Movie_%d", iSlot );
		CKTDGUIButton* pButton = (CKTDGUIButton*) m_pDLGUIQuestList->GetControl( wszName );
		if( NULL == pButton )
			continue;		// June @323dfd55: no button, no id step

		wstring wstrTex = wszDefaultTex;
		if( 10 == iMovieID && NULL != g_pInstanceData )
		{
			if( true == g_pInstanceData->IsEnableCode( 2260 ) || true == g_pInstanceData->IsEnableCode( 2261 ) )
			{
				iSkipped	= 1;
				iMovieID	= 11;
			}
		}

		pButton->SetDummyInt( 0, iMovieID );

		if( true == g_pMain->IsSeenClearMovie( iMovieID ) )
		{
			const CX2Main::ClearMovieTemplet* pTemplet = g_pMain->GetClearMovieTemplet( iMovieID );
			wstrTex = ( NULL != pTemplet ) ? pTemplet->m_wstrMovieTextureName : L"";
			if( true == wstrTex.empty() )
				wstrTex = wszDefaultTex;
			wstring wstrKey = ( NULL != pTemplet ) ? pTemplet->m_wstrMovieTextureKeyName : L"";

			pButton->SetNormalTex( wstrTex.c_str(), wstrKey.c_str() );
			wstrKey += L"_Overlay";
			pButton->SetOverTex( wstrTex.c_str(), wstrKey.c_str() );
			pButton->SetDownTex( wstrTex.c_str(), wstrKey.c_str() );
			pButton->SetGuideDesc( L"" );
			pButton->SetEnableClick( true );
			pButton->SetShowEnable( true, true );

			wsprintf( wszName, L"NewTexture_%d", iSlot );
			pNew = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( wszName );
			if( NULL != pNew && NULL != pNew->GetPicture( 0 ) )
			{
				if( true == g_pMain->IsNewClearMovie( iMovieID ) )
				{
					m_pDLGUIQuestList->ChangeSequence( pNew, true );
					pNew->SetShowEnable( true, true );
					pNew->GetPicture( 0 )->SetFlicker( 1.0f, 1.0f, 0.0f, true );		// June 31c1a5b0( 1, 1, 0, true )
				}
				else
				{
					pNew->SetShowEnable( false, false );
				}
			}
			++iSeen;
		}
		else
		{
			pButton->SetNormalTex( wstrTex.c_str(), L"Overlay_Lock" );
			pButton->SetOverTex( wstrTex.c_str(), L"Overlay_Lock" );
			pButton->SetEnableClick( false );
			pButton->SetShowEnable( true, true );

			wsprintf( wszName, L"NewTexture_%d", iSlot );
			pNew = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( wszName );
			if( NULL != pNew )
				pNew->SetShowEnable( false, false );

			switch( g_pMain->GetClearMovieFileState( iMovieID ) )
			{
			case 0:
				pButton->SetGuideDesc( GET_STRING( 31168 ) );
				++iLocked;
				break;
			case 1:
				{
					wstring wstrDesc = GetMovieLockDesc( iMovieID );
					pButton->SetGuideDesc( wstrDesc.c_str() );
					++iLocked;
				}
				break;
			default:
				pButton->SetShowEnable( false, false );
				++iHidden;
				break;
			}
		}
		++iMovieID;
	}

	const int iRows = m_iMovieCount - iSkipped - m_iMovieStartID + 1;

	CKTDGUIStatic* pHorizonLine = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( L"g_pStatic_HorizonLine" );
	if( NULL != pHorizonLine )
	{
		int iLines = iRows / 3;
		if( iRows != iLines * 3 )
			++iLines;

		int i = 0;
		for( ; i < iLines; ++i )
		{
			if( NULL != pHorizonLine->GetPicture( i ) )
				pHorizonLine->GetPicture( i )->SetShow( true );
		}
		for( ; i < pHorizonLine->GetPictureNum(); ++i )
		{
			if( NULL != pHorizonLine->GetPicture( i ) )
				pHorizonLine->GetPicture( i )->SetShow( false );
		}
		pHorizonLine->SetShowEnable( true, true );
	}

	CKTDGUIStatic* pVillage = (CKTDGUIStatic*) m_pDLGUIQuestList->GetControl( L"Village" );
	if( NULL != pVillage )
	{
		int i = 0;
		for( ; i < iRows; ++i )
		{
			if( NULL == pVillage->GetPicture( i ) || NULL == pVillage->GetString( i ) )
				continue;

			pVillage->GetPicture( i )->SetShow( true );

			int iAreaMovieID = 1;
			wsprintf( wszName, L"Button_Movie_%d", i + 1 );
			CKTDGUIControl* pAreaButton = m_pDLGUIQuestList->GetControl( wszName );
			if( NULL != pAreaButton )
				iAreaMovieID = pAreaButton->GetDummyInt( 0 );

			const CX2Main::ClearMovieTemplet* pTemplet = g_pMain->GetClearMovieTemplet( iAreaMovieID );
			if( NULL != pTemplet )
			{
				pVillage->GetPicture( i )->SetTex( pTemplet->m_wstrIconTextureFileName.c_str(), pTemplet->m_wstrIconTextureKeyName.c_str() );
				pVillage->GetString( i )->msg = pTemplet->m_wstrDungeonName;
			}
		}
		for( ; i < pVillage->GetPictureNum(); ++i )
		{
			if( NULL != pVillage->GetPicture( i ) )
				pVillage->GetPicture( i )->SetShow( false );
			if( NULL != pVillage->GetString( i ) )
				pVillage->GetString( i )->msg = L"";
		}
		pVillage->SetShowEnable( true, true );
	}

	CKTDGUIControl* pArea = m_pDLGUIQuestList->GetControl( L"RadioButton_Area" );
	if( NULL != pArea )
		pArea->SetShowEnable( true, true );

	CX2OfflineLog::Server( L"[JUNE_QUEST] MOVIE list from %d: %d seen, %d locked, %d without a row, %d skipped ( code event ), %d area row(s)",
		m_iMovieStartID, iSeen, iLocked, iHidden, iSkipped, iRows );
}

/// June 323ddb00 ( id ): the lock description of a movie not seen yet. With g_pMain, g_pData and the dungeon manager ( g_pData +0x44 ) and
/// a row: a row with dungeons reads the first one's data ( 327da150 ): a Henir dungeon ( type +0x184 == 2 ) -> 36031, Elder's secret
/// dungeon ( 33030 / 33031 ) -> 36397, else 31167 "LL" ( the row's area name, the dungeon's name ). A row without dungeons and with a quest
/// -> 43910 "iL" ( the quest's m_iEpisodeGroupID / 10, its title ). Anything else -> L"".
wstring CX2UIQuestNew::GetMovieLockDesc( int iMovieID )
{
	wstring wstrDesc = L"";
	if( NULL == g_pMain || NULL == g_pData || NULL == g_pData->GetDungeonManager() )
		return wstrDesc;

	const CX2Main::ClearMovieTemplet* pTemplet = g_pMain->GetClearMovieTemplet( iMovieID );
	if( NULL == pTemplet )
		return wstrDesc;

	if( false == pTemplet->m_vecDungeonID.empty() )
	{
		const CX2Dungeon::DungeonData* pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID) pTemplet->m_vecDungeonID[0] );
		if( NULL != pDungeonData )
		{
			if( CX2Dungeon::DT_HENIR == pDungeonData->m_eDungeonType )
			{
				wstrDesc = GET_STRING( 36031 );
			}
			else if( SEnum::DI_ELDER_SECRET_COMMON == pDungeonData->m_DungeonID || SEnum::DI_ELDER_SECRET_HELL == pDungeonData->m_DungeonID )
			{
				wstrDesc = GET_STRING( 36397 );
			}
			else
			{
				wstring wstrAreaName	= pTemplet->m_wstrDungeonName;
				wstring wstrDungeonName	= pDungeonData->m_DungeonName;
				wstrDesc = GET_REPLACED_STRING( ( 31167, "LL", wstrAreaName, wstrDungeonName ) );
			}
		}
	}
	else if( pTemplet->m_iQuestID > 0 && NULL != g_pData->GetQuestManager() )
	{
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( pTemplet->m_iQuestID );
		if( NULL != pQuestTemplet )
		{
			wstring wstrTitle = pQuestTemplet->m_wstrTitle;
			wstrDesc = GET_REPLACED_STRING( ( 43910, "iL", pQuestTemplet->m_iEpisodeGroupID / 10, wstrTitle ) );
		}
	}
	return wstrDesc;
}

/// June 323e1bf0 case 41039 ( @323e4055 ): a seen movie ( the button's dummy int 0 ) loses its new mark ( 31d75bb0 ) and plays ( 31d884f0 );
/// then NewTexture_<that id> hides. June indexes NewTexture by the movie id, not the slot ( JUNE_BUGS ).
void CX2UIQuestNew::PlayMovieFromTab( CKTDGUIControl* pButton )
{
	if( NULL == pButton || NULL == g_pMain || NULL == m_pDLGUIQuestList )
		return;

	const int iMovieID = pButton->GetDummyInt( 0 );
	if( false == g_pMain->IsSeenClearMovie( iMovieID ) )
		return;

	g_pMain->SetNewClearMovie( iMovieID, false );
	g_pMain->PlayClearMovieByID( iMovieID );

	WCHAR wszName[256] = {0,};
	wsprintf( wszName, L"NewTexture_%d", pButton->GetDummyInt( 0 ) );
	CKTDGUIControl* pNew = m_pDLGUIQuestList->GetControl( wszName );
	if( NULL != pNew )
		pNew->SetShowEnable( false, false );
}

/// June 323e1bf0 cases 41041 / 41042 ( @323e4117 / @323e41d8 ): the hovered button's NewTexture ( slot = id + 1 - first id ) goes to the back
/// on mouse-over and to the front on mouse-out ( ChangeSequence 31cd4da0 ).
void CX2UIQuestNew::MarkMovieButton( CKTDGUIControl* pButton, bool bFront )
{
	if( NULL == m_pDLGUIQuestList || NULL == pButton )
		return;

	int iSlot = pButton->GetDummyInt( 0 );
	if( 1 != m_iMovieStartID )
		iSlot += 1 - m_iMovieStartID;

	WCHAR wszName[256] = {0,};
	wsprintf( wszName, L"NewTexture_%d", iSlot );
	CKTDGUIControl* pNew = m_pDLGUIQuestList->GetControl( wszName );
	if( NULL != pNew )
		m_pDLGUIQuestList->ChangeSequence( pNew, bFront );
}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB

#endif SERV_IRUHADEV_JUNE_QUEST_UI
#endif SERV_EPIC_QUEST

#ifdef REFORM_NOVICE_GUIDE
void CX2UIQuestNew::SetNoviceGuideStep()
{
	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() 
		&& true == g_pData->GetPlayGuide()->GetActiveNoviceGuide() )
	{
		if( NULL != m_pDLGUIQuestClear && true == m_pDLGUIQuestClear->GetShow() && QUM_QUEST_DETAIL == m_eNowQuestUIMode )
		{
			switch( g_pData->GetPlayGuide()->GetGuideStep() )
			{
			case CX2PlayGuide::NGS_STEP_0:
			case CX2PlayGuide::NGS_STEP_1:
			case CX2PlayGuide::NGS_STEP_2:
			case CX2PlayGuide::NGS_STEP_3:
				{
					g_pData->GetPlayGuide()->SetMouseClickIndex( CX2PlayGuide::NGS_STEP_4 );
				}
				break;
			}
		}
		else
		{
			switch( m_eNowQuestUIMode )
			{
			case QUM_EPIC_QUEST:
				{
					switch( g_pData->GetPlayGuide()->GetGuideStep() )
					{
					case CX2PlayGuide::NGS_STEP_0:
					case CX2PlayGuide::NGS_STEP_1:
					case CX2PlayGuide::NGS_STEP_3:
						{
							g_pData->GetPlayGuide()->SetMouseClickIndex(CX2PlayGuide::NGS_STEP_2);
						} break;
					case CX2PlayGuide::NGS_STEP_4:
						{
							g_pData->GetPlayGuide()->ResetNoviceGuide();
						} break;
					}
				} break;
			case QUM_QUEST:
			case QUM_EVENT_QUEST:
				{
					if( CX2PlayGuide::NGS_STEP_2 == g_pData->GetPlayGuide()->GetGuideStep() )
					{
						g_pData->GetPlayGuide()->SetMouseClickIndex( CX2PlayGuide::NGS_STEP_1 );
					}
				} break;
			case QUM_QUEST_DETAIL:
				{
					if( CX2PlayGuide::NGS_STEP_2 == g_pData->GetPlayGuide()->GetGuideStep() )
					{
						g_pData->GetPlayGuide()->SetMouseClickIndex( CX2PlayGuide::NGS_STEP_3 );
					}

				} break;
			}
		}
	}
}
#endif //REFORM_NOVICE_GUIDE
#ifdef REFORM_QUEST
/** @function : ClearQuestCompleteParticle
	@breif : 퀵퀘스트 완료 파티클 제거
*/
void CX2UIQuestNew::ClearQuestCompleteParticle()
{
	if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hQuestCompleteParticle )
		return;

	if( NULL != g_pData && NULL != g_pData->GetUIMajorParticle() )
	{
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle(m_hQuestCompleteParticle);
	}	
}
#endif //REFORM_QUEST

#ifdef GUIDE_QUEST_MONSTER
/** @function : UpdateQuestMonster
	@breif	: 퀘스트 몬스터 리스트 작성 후 몬스터 정보 갱신
*/
void CX2UIQuestNew::UpdateQuestMonster()
{
	//x2game이 없을 때는 퀘스트 몬스터를 표현할 필요가 없음..
	if( NULL == g_pX2Game )
		return;

	//퀘스트 정보 갱신 이전 몬스터 정보 초기화
	set<CX2UnitManager::NPC_UNIT_ID>::iterator it = m_setQuestNPCList.begin();
	for( ; it != m_setQuestNPCList.end(); ++it)
	{
		g_pX2Game->SetQuestMonster(*it, false); 
	}

	//퀘스트 몬스터 리스트 작성
	m_setQuestNPCList.clear();
	SetQuestMonsterByQuestInfo(m_vecEpicQuestInfo);
	SetQuestMonsterByQuestInfo(m_vecOnGoingEventQuestInfo);
	SetQuestMonsterByQuestInfo(m_vecOnGoingQuestInfo);
		
	//퀘스트 몬스터 정보를 해당 NPC 객체에 설정
	for( it = m_setQuestNPCList.begin(); it != m_setQuestNPCList.end(); ++it)
	{
		g_pX2Game->SetQuestMonster(*it);
	}
}

/** @function : SetQuestMonsterByQuestInfo
	@breif	: 수행중인 서브퀘스트의 m_setKillNPCID 리스트에 등록된 몬스터ID를 퀘스트 몬스터 리스트에 등록
			  UpdateQuestMonster()에서 호출됨
	@param	: 퀘스트 리스트 (vecQuestInfo_)
*/
void CX2UIQuestNew::SetQuestMonsterByQuestInfo( const vector<QuestInfo>& vecQuestInfo_ )
{	
	if( CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() &&
		CX2Main::XS_DUNGEON_GAME != g_pMain->GetNowStateID() )
		return;

	//퀘스트 목록 순회
	for( vector<QuestInfo>::const_iterator it = vecQuestInfo_.begin(); it != vecQuestInfo_.end(); ++it)
	{				
		if( CX2UIQuestNew::QSS_ONGOING != it->m_eQuestState )
		{ 
			continue;//현재 진행중인 퀘스트가 아니면 다음 목록 순회
		}
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( it->m_iID );
		if( NULL == pQuestTemplet || true == pQuestTemplet->m_vecSubQuest.empty() )
		{ 
			continue;//pQuestTemplet가 NULL이거나, 서브퀘스트 목록이 비어있다면 다음 목록 순회
		}

		for( vector<int>::const_iterator itSubQuest = pQuestTemplet->m_vecSubQuest.begin(); 
			itSubQuest != pQuestTemplet->m_vecSubQuest.end(); ++itSubQuest)
		{					
			const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( *itSubQuest );	
			if( NULL == pSubQuestTemplet || true == pSubQuestTemplet->m_ClearCondition.m_setKillNPCID.empty() )
			{
				continue;//m_setKillNPCID가 지정되어 있지 않다면 다음 목록 순회
			}
			
			//해당 퀘스트가 현재 진행중인 던전과 관계 있는지 확인
			if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )  
			{
				bool bPossibleQuest = false;

				const SEnum::DUNGEON_ID eDungeonID = g_pData->GetDungeonRoom()->GetDungeonID(); //난이도가 포함되지 않은 던전ID
				const CX2Dungeon::DIFFICULTY_LEVEL eCurrentDifficultyLevel =  g_pData->GetDungeonRoom()->GetDifficulty();//난이도

				if( true == pSubQuestTemplet->m_ClearCondition.m_bUpperDifficulty ) //~이상 난이도 체크 true라면
				{
					for( int iDifficultyLevel = CX2Dungeon::DL_NORMAL ;	iDifficultyLevel <= eCurrentDifficultyLevel; ++iDifficultyLevel )
					{//난이도별로 체크
						if( pSubQuestTemplet->m_ClearCondition.m_setDungeonID.end() != 
							pSubQuestTemplet->m_ClearCondition.m_setDungeonID.find( static_cast<SEnum::DUNGEON_ID>(eDungeonID+iDifficultyLevel)) )
						{
							bPossibleQuest = true;
							break;
						}		
					}
				}
				else //~이상 난이도 체크 false 라면
				{
					if( pSubQuestTemplet->m_ClearCondition.m_setDungeonID.end() !=  
						pSubQuestTemplet->m_ClearCondition.m_setDungeonID.find( static_cast<SEnum::DUNGEON_ID>(eDungeonID+eCurrentDifficultyLevel)) )
					{//단일 대상만 체크
						bPossibleQuest = true;
					}
				}
				if( false == bPossibleQuest )
				{
					continue;//현재 위치에서 수행할 수 없다면 다음 목록 순회
				}
			}
			//해당 퀘스트가 현재 진행중인 필드와 관계 있는지 확인
			else if( CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() )  
			{
				bool bPossibleQuest = false;

				const int iFieldID = g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam(); //현재 필드 ID
								
				if( pSubQuestTemplet->m_ClearCondition.m_setBattleFieldID.end() !=
					pSubQuestTemplet->m_ClearCondition.m_setBattleFieldID.find( iFieldID ) )
				{
					bPossibleQuest = true;
				}
				
				if( false == bPossibleQuest )
				{
					continue;//현재 위치에서 수행할 수 없다면 다음 목록 순회
				}
			}

			for( set< CX2UnitManager::NPC_UNIT_ID >::iterator sit = pSubQuestTemplet->m_ClearCondition.m_setKillNPCID.begin(); 
				sit != pSubQuestTemplet->m_ClearCondition.m_setKillNPCID.end(); ++sit )
			{	//퀘스트 몬스터 리스트에 등록
				m_setQuestNPCList.insert(*sit);
			}
		}
	}
}
#endif //GUIDE_QUEST_MONSTER

bool CX2UIQuestNew::GetThereIsSelectionReward() const
{
	if( NULL == g_pData )
		return false;
	
	if( NULL != g_pData->GetQuestManager() )
	{
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID );
		if( NULL != pQuestTemplet )
		{
			if( NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
				CX2Unit::UNIT_TYPE eUnitType = pUnit->GetType();
				CX2Unit::UNIT_CLASS eUnitClass = pUnit->GetClass();
				for( UINT i = 0; i < pQuestTemplet->m_SelectReward.m_vecSelectItem.size(); ++i)
				{
					const CX2QuestManager::ITEM_DATA& itemData = pQuestTemplet->m_SelectReward.m_vecSelectItem[i];

					if( NULL != g_pData->GetItemManager() )
					{
						const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemData.m_iItemID );
						if( NULL == pItemTemplet )
						{
							continue;
						}
						else if( CX2Unit::UT_NONE != pItemTemplet->GetUnitType() &&
							eUnitType != pItemTemplet->GetUnitType() )
						{
							continue;
						}
#ifdef QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
						if( pItemTemplet->GetUnitClass() != CX2Unit::UC_NONE )
						{
							CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();
							if( false == GetX2UnitClassCompatibility(eUnitClass, pItemTemplet->GetUnitClass() ) )
							{
								continue;
							}
						}
#endif QUEST_REWARD_ITEM_CLASS_TYPE_CHECK
					}
					return true;
				}			
			}	
		}
	}
	return false;
}


#ifdef MODIFY_QUEST_UI
CX2UIQuestNew::QUEST_STATE_SORT CX2UIQuestNew::ConvertToStateTypeSort( int eQuestStateType_ )
{
	CX2UIQuestNew::QUEST_STATE_SORT eQuestStateType = QSS_COMPLETE;

	switch( eQuestStateType_ )
	{
	case CX2UIQuestNew::QSS_COMPLETE_EPIC:
	case CX2UIQuestNew::QSS_COMPLETE:
		break;
	default:
		eQuestStateType = QSS_END;
		break;
	}
	return eQuestStateType;
}
#endif //MODIFY_QUEST_UI

#ifdef DAY_OF_THE_WEEK_QUEST
// 요일 퀘스트 유효셩 여부 체크.요일 퀘스트가 아니라면 무조건 true 반환.
bool CX2UIQuestNew::GetIsValidDayTheWeekQuest( int iQuestID_ )
{
	if( NULL == g_pData )
		return false;

	if( NULL == g_pData->GetQuestManager() )
		return false;

	UINT iQuestDayOfWeek = g_pData->GetQuestManager()->GetQuestDayByQuestID( iQuestID_ );
	if( 0 == iQuestDayOfWeek )
		return true;

	CTime ctCurrentTime( g_pData->GetServerCurrentTime() );
	const UINT iCurrentDayOfWeek = ctCurrentTime.GetDayOfWeek();
			
	if( iCurrentDayOfWeek == iQuestDayOfWeek )
		return true;

	return false;
}
#endif //DAY_OF_THE_WEEK_QUEST

#ifdef SERV_POINT_COUNT_SYSTEM
bool CX2UIQuestNew::Handler_EGS_QUEST_POINT_COUNT_SYSTEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_QUEST_POINT_COUNT_SYSTEM_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pData != NULL && g_pData->GetQuestManager() != NULL  )
	{
		g_pData->GetQuestManager()->SetUpdataQuestInstance(kEvent.m_mapQuestInstance);
	}

	return true;
}
#endif //SERV_POINT_COUNT_SYSTEM

#ifdef SERV_IRUHADEV_JUNE_TUTORIAL_MODE
/// June buff port Phase 14c, CX2UIQuestNew::ToggleUserShowQuickQuestDLG 323ddfe0: March's inline body (X2UIQuestNew.h) under
/// June's gate @323de00b-323de031: no toggle in the training game, and in the tutorial a shown quick quest cannot be hidden.
/// June also mirrors the new value into a June-only CX2Main byte (+0x2aa) and runs its refresh twice; not ported.
void CX2UIQuestNew::ToggleUserShowQuickQuestDLG()
{
#ifdef FIX_UI_OFF_TAB_OFF
	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		return;
#endif //FIX_UI_OFF_TAB_OFF
	if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() )
		return;
	if( true == g_pMain->GetIsPlayingTutorial() && true == m_bUserShowQuickQuestDLG )
		return;
	m_bUserShowQuickQuestDLG = !m_bUserShowQuickQuestDLG;
	UpdateQuickQuestDLG();UserShowQuickQuestDLG(m_bUserShowQuickQuestDLG); ShowUIDesc(false);
}
#endif SERV_IRUHADEV_JUNE_TUTORIAL_MODE
