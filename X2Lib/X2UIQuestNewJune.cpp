#include "StdAfx.h"

#ifdef SERV_EPIC_QUEST
#ifdef SERV_IRUHADEV_JUNE_QUEST_UI

// June quest system Phase Q5a - June's quest window ( CX2UIQuestNew, June 2017 323c8da0 .. 323e5fb0 ), ported onto March's class.
//
// Everything here is behind SERV_IRUHADEV_JUNE_QUEST_UI. X2UIQuestNew.cpp keeps March's bodies and sends the flagged
// callers into the June* members below ( one early return per replaced function ); with the flag off this file is empty.
//
// What stays out, and who owns it ( see the Done note of Q5a ):
//   - quest scenes ( 323cb2e0 / Quest_Event_Scene_Info ): Q5b, built on june-gfx G3. March's PlayEventScene calls are untouched.
//   - the scenario-movie tab ( 323ca6d0 / 323dfc00 ) and the clear movie of a complete request ( 31d7a240 ): Q8.
//   - the village-NPC friendship manager ( 32696fa0 / 3267b440, backlog Stage 18 ): no quest is a friendship quest here, which is
//     what June's lookup returns for an empty manager ( JuneIsFriendshipQuest ).
//   - the story-book singleton ( 326ab210 / 326ab090 ) behind the story Remind button: Q10.
//   - the tutorial-guide manager ( 31d70b00 / 31fc5920 ) and the menu "Skill_New" mark refresh ( state vtable +0x104 ): Stage 9.

#ifndef SERV_IRUHADEV_OFFLINE
#error SERV_IRUHADEV_JUNE_QUEST_UI reads CX2QuestManager::QuestTemplet::m_vecAfterQuestID ( SERV_IRUHADEV_OFFLINE )
#endif SERV_IRUHADEV_OFFLINE
#ifndef SERV_IRUHADEV_JUNE_QUEST_LOAD
#error SERV_IRUHADEV_JUNE_QUEST_UI needs SERV_IRUHADEV_JUNE_QUEST_LOAD ( June's quest enums, Quest_Event_Scene_Info, the story index )
#endif SERV_IRUHADEV_JUNE_QUEST_LOAD

#include ".\X2UIQuestNew.h"
#include "Offline/X2OfflineLog.h"

// ---------------------------------------------------------------------------------------------------------------------
// small helpers
// ---------------------------------------------------------------------------------------------------------------------

/// June 32696fa0 + 3267b440: "is this quest id in the NPC friendship manager's map". The manager is Stage 18; an empty manager says no.
static bool JuneIsFriendshipQuest( int /*iQuestID*/ )
{
	return false;
}

/// June checks the game state numbers 0xe..0xf and 0x15 ( village, battle field ). March's X2_STATE order differs ( the arcade states ),
/// so the two states March has are named here.
static bool JuneIsVillageOrBattleFieldState()
{
	if( NULL == g_pMain )
		return false;

	return CX2Main::XS_VILLAGE_MAP == g_pMain->GetNowStateID() || CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID();
}

/// June's unit +0x3c: the selected unit's transcendence step. Stage 6 owns transcendence; every March unit is step 0.
static int JuneSelectUnitClassTran()
{
	return 0;
}

static void JuneQuestUILog( const WCHAR* wszFormat, int iA = 0, int iB = 0 )
{
	static int s_iLines = 0;
	if( s_iLines >= 200 )
		return;
	++s_iLines;
	CX2OfflineLog::Server( wszFormat, iA, iB );
}

/// June 323ca880: may the epic quest in this list row be skipped ( Button_Skip_Epic_Quest ).
/// Epic type, every next-quest templet exists, not a time event, and the unit is more than 7 levels above the quest's fair level.
static bool JuneCanSkipEpicQuest( const CX2UIQuestNew::QuestInfo& kInfo, int iQuestID )
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

// ---------------------------------------------------------------------------------------------------------------------
// ctor hook, tab modes
// ---------------------------------------------------------------------------------------------------------------------

void CX2UIQuestNew::JuneInitMembers()
{
	m_pButtonSkipEpicQuest		= NULL;
	m_bIsOpenGuildCategory		= true;		// June ctor: +0x720 = 1
	m_iOpenStoryCategory		= -1;		// +0x75c
	m_bCompleteQuestInStoryTab	= false;	// +0x758
	m_bOngoingQuestInStoryTab	= false;	// +0x759
	m_iCompleteStoryCategory	= -1;		// +0x760
	m_iOngoingStoryCategory		= -1;		// +0x764
}

/// June 323c9e80: set the mode and check the matching tab radio. Mode 3 ( detail ) has no radio.
void CX2UIQuestNew::JuneSetNowQuestUIMode( QUEST_UI_MODE eMode )
{
	m_eNowQuestUIMode = eMode;

	if( NULL == m_pDLGUIQuestList )
		return;

	const WCHAR* wszRadio = NULL;
	switch( eMode )
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
}

/// June 323ca520: back to the tab the list was last on ( scenario / event / guild / story ), anything else is the request tab.
void CX2UIQuestNew::JuneReturnToQuestListUI()
{
	switch( m_eLastQuestUIMode )
	{
	case QUM_EPIC_QUEST:
	case QUM_EVENT_QUEST:
	case QUM_GUILD_QUEST:
	case QUM_STORY_QUEST:
		JuneSetNowQuestUIMode( m_eLastQuestUIMode );
		break;
	default:
		JuneSetNowQuestUIMode( QUM_QUEST );
		break;
	}
}

/// June 323c9250: the tab the window opens on. Only the "complete" flags matter; the request tab with the available list open is the default.
CX2UIQuestNew::QUEST_UI_MODE CX2UIQuestNew::JuneDetermineUIModeWithTabPriority()
{
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
}

// ---------------------------------------------------------------------------------------------------------------------
// type conversion and picture numbers
// ---------------------------------------------------------------------------------------------------------------------

/// June 323d5650 / 323d0770 / 323ce6f0 switch on templet +0x8
CX2UIQuestNew::QUEST_TYPE_SORT CX2UIQuestNew::JuneConvertToQuestTypeSort( int iQuestType )
{
	switch( iQuestType )
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
}

/// June: templet +0x34 ( NORMAL 0, DAY 1, REPEAT 2, WEEK 3, MONTH 4 ). QRT_CHAIN_REPEAT 5 is not mapped ( normal ).
CX2UIQuestNew::REPEAT_TYPE_SORT CX2UIQuestNew::JuneConvertToRepeatTypeSort( int iQuestRepeatType )
{
	switch( iQuestRepeatType )
	{
	case CX2QuestManager::QRT_DAY:		return RTS_DAY;
	case CX2QuestManager::QRT_REPEAT:	return RTS_REPEAT;
	case CX2QuestManager::QRT_WEEK:		return RTS_WEEK;
	case CX2QuestManager::QRT_MONTH:	return RTS_MONTH;
	default:							return RTS_NORMAL;
	}
}

/// June 323c86f0: the list row's type picture ( Static_QuestList pictures of DLG_UI_NEW_Quest_List ).
int CX2UIQuestNew::JuneGetQuestPictureNumber( QUEST_TYPE_SORT eTypeSort, REPEAT_TYPE_SORT eRepeatSort )
{
	switch( eTypeSort )
	{
	case QTS_EPIC:			return 6;
	case QTS_CONTEST:		return 5;
	case QTS_EVENT:			return 4;
	case QTS_PCBANG:		return 3;
	case QTS_CHANGE_JOB:	return 2;
	case QTS_SKILL:			return 1;
	case QTS_NORMAL:
		switch( eRepeatSort )
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
}

/// June 323c8620: the detail title's type picture ( StaticQuestTitle pictures of DLG_UI_NEW_Quest_Detail ). Differs from the list's in the
/// month ( 19 ) and the guild / story / transcend pictures ( 16 / 17 / 18 ).
static int JuneGetDetailQuestPictureNumber( int iTypeSort, int iRepeatSort )
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

/// June 323cc280: the NPC face key in DLG_UI_Npc_Face01. The ids are June's NPC_UNIT_ID values ( Enum.lua ); 10142 is the story
/// quest NPC and 10153 the transcendence one. June dropped the event NPCs March had ( crayonpop, apink, benders ): they fall to Default.
wstring CX2UIQuestNew::JuneGetNPCFaceTextureKey( int iNpcID )
{
	switch( iNpcID )
	{
	case 10000:	return L"ANN";
	case 10001:	return L"HAGUS";
	case 10002:	return L"ROU";
	case 10003:	return L"ADAMS";
	case 10004:	return L"ECO";
	case 10005:	return L"HOFMAN";
	case 10006:	return L"RUICHEL";
	case 10007:	return L"RENPAT";
	case 10008:	return L"STELRA";
	case 10009:	return L"CHACHABUK";
	case 10010:	return L"RICHIANG";
	case 10011:	return L"TOMA";
	case 10012:	return L"ADEL";
	case 10013:	return L"AGATA";
	case 10014:	return L"AIDA";
	case 10015:	return L"AMOSE";
	case 10016:	return L"";				// NUI_MOON: empty, as March
	case 10017:	return L"KAMILRA";
	case 10018:	return L"ARIEL";
	case 10019:	return L"HELREN";
	case 10020:	return L"ALREGRO";
	case 10021:	return L"RENTO";
	case 10026:	return L"GLAVE";
	case 10027:	return L"LURIEL";
	case 10028:	return L"GRAIL";
	case 10029:	return L"NOEL";
	case 10030:	return L"VANESSA";
	case 10031:	return L"PRAUS";
	case 10032:	return L"HANNA";
	case 10037:	return L"HORATIO";
	case 10038:	return L"LUCY";
	case 10039:	return L"DAISY";
	case 10040:	return L"PENENSIO";
	case 10041:	return L"DENKA";
	case 10042:	return L"LORDROS";
	case 10043:	return L"Default";
	case 10045:	return L"MU";
	case 10048:	return L"RUTO";
	case 10055:	return L"ROSEANG";
	case 10056:	return L"EMIRATE";
	case 10057:	return L"VAPOR";
	case 10058:	return L"DAPPAR";
	case 10067:	return L"EDEL";
	case 10068:	return L"STEEL";
	case 10069:	return L"FACEOFF";
	case 10070:	return L"SYOTA";
	case 10071:	return L"IGNIA";
	case 10078:	return L"YUNO";
	case 10142:	return L"Story";
	case 10143:	return L"VERENADE";
	case 10144:	return L"DYURENDA";
	case 10145:	return L"TEODOR";
	case 10146:	return L"HUGO";
	case 10153:	return L"Trans";
	case 10218:	return L"PANE";
	}
	return L"Default";
}

/// June 323d8be0 / 323ce6f0: the three event NPCs whose face is a button texture, not a DLG_UI_Npc_Face01 key.
static bool JuneSetNpcFacePicture( CKTDGUIControl::CPictureData* pPicture, int iNpcID, const wstring& wstrKey )
{
	if( NULL == pPicture )
		return false;

	if( 4309 == iNpcID || 4346 == iNpcID || 4254 == iNpcID )
		pPicture->SetTex( L"DLG_INT_Button02.TGA", L"NPC_FACE_EVENT" );
	else
		pPicture->SetTex( L"DLG_UI_Npc_Face01.TGA", wstrKey.c_str() );
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// the lists
// ---------------------------------------------------------------------------------------------------------------------

/// June 323cf570: the quests the unit is doing, as ids. A guild quest is the guild's, not the unit's. An event quest has to fit the unit's
/// type and class; a change-job quest is listed once.
bool CX2UIQuestNew::JuneGetUnitQuest( std::vector<int>& vecQuestID )
{
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
		if( -1 != iClassTran && JuneSelectUnitClassTran() != iClassTran && CX2QuestManager::QT_EVENT != pT->m_eQuestType )
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
}

/// June 323cf250 ( epic, 5 ) / 323cf3e0 ( story, 10 ): the completed quests of one type
bool CX2UIQuestNew::JuneGetCompleteQuestOfType( std::vector<int>& vecQuestID, int iQuestType )
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
bool CX2UIQuestNew::JuneIsQuestStartable( int iQuestID )
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
void CX2UIQuestNew::JuneAddFirstStoryQuests( std::vector<int>& vecOngoing, const std::vector<int>& vecCompleteStory )
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

		if( false == JuneIsQuestStartable( iFirst ) )
			continue;

		const CX2QuestManager::QuestTemplet* pT = pQM->GetQuestTemplet( iFirst );
		if( NULL != pT && false == pT->m_bIsTimeEvent && true == JuneIsVillageOrBattleFieldState() )
			vecOngoing.push_back( iFirst );
	}
}

/// June 323d4ae0: after a completed story quest that still has a next quest, the next quests that are not in progress are listed.
/// A story quest that is the last of its group has no next listed. ( June does not drop duplicates here; neither does this. )
void CX2UIQuestNew::JuneAddNextStoryQuests( std::vector<int>& vecOngoing, const std::vector<int>& vecCompleteStory )
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
void CX2UIQuestNew::JuneSetOpenStoryCategory()
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

/// June's comparator for the available list ( 323eac30 ): story last, the stories by id. It answers true for two non-story rows, so
/// std::sort reorders the other quests ( kept as June has it, JUNE_BUGS ).
struct JuneAvailQuestInfoSort : public std::binary_function< CX2UIQuestNew::QuestInfo, CX2UIQuestNew::QuestInfo, bool >
{
	bool operator()( const CX2UIQuestNew::QuestInfo& kLeft, const CX2UIQuestNew::QuestInfo& kRight ) const
	{
		if( CX2UIQuestNew::QTS_STORY == kLeft.m_eQuestType )
			return CX2UIQuestNew::QTS_STORY == kRight.m_eQuestType && kLeft.m_iID < kRight.m_iID;
		return CX2UIQuestNew::QTS_STORY != kRight.m_eQuestType;
	}
};

/// June 323d0770: the quests the NPC can give. Without an NPC the board ( NUI_BILLBOARD ) answers in a village / field, so the request
/// tab lists the available quests anywhere there.
void CX2UIQuestNew::JuneUpdateAvailQuestInfo( CX2UnitManager::NPC_UNIT_ID eNPCID )
{
	if( CX2UnitManager::NUI_NONE == eNPCID )
	{
		eNPCID = m_eNPCID;
		if( true == JuneIsVillageOrBattleFieldState() && CX2UnitManager::NUI_NONE == eNPCID )
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
		if( -1 != iClassTran && JuneSelectUnitClassTran() != iClassTran )
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
		kInfo.m_eQuestType		= JuneConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= JuneConvertToRepeatTypeSort( pT->m_eRepeatType );
		kInfo.m_iConditionLevel	= pT->m_Condition.m_iLevel;
		m_vecAvailQuestInfo.push_back( kInfo );
	}

	std::sort( m_vecAvailQuestInfo.begin(), m_vecAvailQuestInfo.end(), QuestInfoSort() );
	std::sort( m_vecAvailQuestInfo.begin(), m_vecAvailQuestInfo.end(), JuneAvailQuestInfoSort() );
}

/// June 323d5650: build every tab's list from the unit's quests. A tab gets its own vector ( epic, event, guild, story ); the rest go to the
/// ongoing list; a story quest in progress is in the story and the ongoing list both.
void CX2UIQuestNew::JuneUpdateOngoingQuestInfo()
{
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

	JuneGetUnitQuest( vecOnGoingQuestID );
	JuneGetCompleteQuestOfType( vecCompleteEpicQuestID, CX2QuestManager::QT_EPIC );
	JuneGetCompleteQuestOfType( vecCompleteStoryQuestID, CX2QuestManager::QT_STORY );
	JuneAddFirstStoryQuests( vecOnGoingQuestID, vecCompleteStoryQuestID );
	JuneAddNextStoryQuests( vecOnGoingQuestID, vecCompleteStoryQuestID );

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
		kInfo.m_eQuestType		= JuneConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= JuneConvertToRepeatTypeSort( pT->m_eRepeatType );
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
		kInfo.m_eQuestType		= JuneConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= JuneConvertToRepeatTypeSort( pT->m_eRepeatType );
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
		kInfo.m_eQuestType		= JuneConvertToQuestTypeSort( pT->m_eQuestType );
		kInfo.m_eRepeatType		= JuneConvertToRepeatTypeSort( pT->m_eRepeatType );
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
		JuneSetOpenStoryCategory();
}

// ---------------------------------------------------------------------------------------------------------------------
// the list window
// ---------------------------------------------------------------------------------------------------------------------

/// June 323d9400, the tail every tab shares: clamp the first row, then fill the 13 rows from it.
void CX2UIQuestNew::JuneFillQuestSlots()
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
			JuneSetUIQuestSlotInfoToCategory( i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i] );
		else
			JuneUpdateUIQuestSlotInfo( i, m_vecQuestInfoList[m_iTopQuestSlotIndex + i] );
	}
}

/// June 323dfa80. The scenario-movie tab ( +0x774 ) skips all of it; Q8 owns that tab.
void CX2UIQuestNew::JuneResetQuestUI( bool bUpdateNavi, bool bResetExpand, bool bIgnoreTabPriority )
{
	JuneUpdateOngoingQuestInfo();
#ifdef GUIDE_QUEST_MONSTER
	UpdateQuestMonster();
#endif //GUIDE_QUEST_MONSTER
#ifdef QUEST_GUIDE
	if( NULL != g_pData && NULL != g_pData->GetQuestManager() )
		g_pData->GetQuestManager()->SetHasDungeonQuest();
#endif //QUEST_GUIDE

	if( false == bIgnoreTabPriority )
		JuneResetQuestList( JuneDetermineUIModeWithTabPriority() );
	else
	{
		QUEST_UI_MODE eMode = m_eNowQuestUIMode;
		if( QUM_QUEST == eMode )
			eMode = QUM_QUEST_RECEIVE;		// June: IsTalkingQuestNPC() is always true
		JuneResetQuestList( eMode );
	}

	if( true == bUpdateNavi )
		UpdateNavi();

	JuneHideMovieTab();		// June 323e1bf0 / 323e5340: every reset ends with the movie-tab hide
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
}

/// June 323ca6d0: hide the scenario-movie tab's controls ( DLG_UI_NEW_Quest_List.lua builds 12 Button_Movie_N / NewTexture_N,
/// g_pStatic_HorizonLine, Village and RadioButton_Area; the tab itself is Q8 ). Without this they draw over every quest tab.
void CX2UIQuestNew::JuneHideMovieTab()
{
	if( NULL == m_pDLGUIQuestList )
		return;

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
/// June 323d9400
void CX2UIQuestNew::JuneResetQuestList( QUEST_UI_MODE eQuestUIMode )
{
	m_vecQuestInfoList.clear();

	JuneSetNowQuestUIMode( eQuestUIMode );
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
			JuneFillQuestSlots();
		}
		break;
	case QUM_QUEST_RECEIVE:
		{
			m_eLastQuestUIMode = m_eNowQuestUIMode;
			m_pDLGUIQuestList->SetShowEnable( true, true );
			m_pDLGUIQuestDetail->SetShowEnable( false, false );

			InitUIQuestSlotInfo();
			InitRewardItemSlot();

			JuneUpdateAvailQuestInfo();

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
			JuneFillQuestSlots();
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
			JuneFillQuestSlots();
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
			JuneFillQuestSlots();
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
			JuneFillQuestSlots();
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
			JuneFillQuestSlots();
		}
		break;
	}
}

/// June 323d80d0: a category row. A group row ( epic or story ) shows the group title and, from the two progress maps, a "done" or a
/// "going" mark; the request tab's two rows ( 0 available, 1 in progress ) show a fixed title and the count.
void CX2UIQuestNew::JuneSetUIQuestSlotInfoToCategory( UINT iQuestSlotInfoIndex, QuestInfo& kQuestInfo )
{
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

	if( -1 != kQuestInfo.m_iEpicGroupID )
	{
		kSlot.m_QuestID = kQuestInfo.m_iEpicGroupID;

		if( NULL != kSlot.m_pButtonCategory )
			kSlot.m_pButtonCategory->SetShowEnable( true, true );
		if( NULL != kSlot.m_pStaticCategory )
			kSlot.m_pStaticCategory->SetShowEnable( true, true );

		wstring wstrGroupTitle = g_pData->GetQuestManager()->GetEpicGroupTitle( kQuestInfo.m_iEpicGroupID );
		if( NULL != kSlot.m_pStaticCategory )
		{
			kSlot.m_pStaticCategory->GetString( 0 )->msg = wstrGroupTitle;
			kSlot.m_pStaticCategory->GetString( 1 )->msg = L"";
		}

		kSlot.m_bIsCategory = true;

		bOpen = ( m_iOpenEpicCategory == kQuestInfo.m_iEpicGroupID || m_iOpenStoryCategory == kQuestInfo.m_iEpicGroupID );

		// the two marks: picture 0 is "done", picture 1 is "going"
		if( NULL != kSlot.m_pStaticCategory )
		{
			bool bFound = false;
			std::map<int, bool>::iterator mit = m_mapEpicQuestProgressInfo.find( kQuestInfo.m_iEpicGroupID );
			if( mit != m_mapEpicQuestProgressInfo.end() )
			{
				bFound = true;
				kSlot.m_pStaticCategory->GetPicture( 0 )->SetShow( false == mit->second );
				kSlot.m_pStaticCategory->GetPicture( 1 )->SetShow( true == mit->second );
			}

			std::map<int, int>::iterator sit = m_mapStoryQuestProgressInfo.find( kQuestInfo.m_iEpicGroupID );
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

		if( 0 == kQuestInfo.m_iID )
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
		else if( 1 == kQuestInfo.m_iID )
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
}

/// June 323d8be0 state -> picture: complete ( 0 / 1 / 6 ) 9, failed 12, going 10, available 11; waiting states show picture 0
static int JuneGetQuestStatePictureNumber( int eState )
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

/// June 323d8be0: one quest row
void CX2UIQuestNew::JuneUpdateUIQuestSlotInfo( UINT iQuestSlotInfoIndex, QuestInfo& kQuestInfo )
{
	if( false == m_bShow )
		return;

	UINT i = iQuestSlotInfoIndex;
	UIQuestSlotInfo& kSlot = m_vecUIQuestSlotInfo[i];

	const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( kQuestInfo.m_iID );
	if( NULL == pQuestTemplet )
	{
		JuneQuestUILog( L"[JUNE_QUEST] UI row for quest %d has no templet - skipped", kQuestInfo.m_iID );
		return;
	}

	kSlot.m_QuestID			= kQuestInfo.m_iID;
	kSlot.m_eQuestState		= kQuestInfo.m_eQuestState;
	kSlot.m_eQuestType		= kQuestInfo.m_eQuestType;
	kSlot.m_eRepeatType		= kQuestInfo.m_eRepeatType;
	kSlot.m_iPlayLevel		= kQuestInfo.m_iPlayLevel;
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
		kSlot.m_pStaticQuest->GetString( 0 )->color = g_pData->GetQuestManager()->GetQuestColor( kQuestInfo.m_iID, g_pData->GetSelectUnitLevel() );
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

		kSlot.m_pStaticQuest->GetPicture( JuneGetQuestPictureNumber( kQuestInfo.m_eQuestType, kQuestInfo.m_eRepeatType ) )->SetShow( true );

		if( false == m_setNewEventQuestIDList.empty() &&
			m_setNewEventQuestIDList.end() != m_setNewEventQuestIDList.find( kSlot.m_QuestID ) )
			kSlot.m_pStaticQuest->GetPicture( 18 )->SetShow( true );

		kSlot.m_pStaticQuest->GetPicture( JuneGetQuestStatePictureNumber( kQuestInfo.m_eQuestState ) )->SetShow( true );

		JuneSetNpcFacePicture( kSlot.m_pStaticQuest->GetPicture( 13 ), (int)pQuestTemplet->m_eStartNPCID, JuneGetNPCFaceTextureKey( (int)pQuestTemplet->m_eStartNPCID ) );
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
}

/// June 323ccbc0: which of the detail window's buttons show for the selected quest. The Giveup / Accept / Complete ... buttons are the
/// same as March's; new are the skip-epic button, the guild rule ( none ), the story states, and a Remind that needs a scene row.
void CX2UIQuestNew::JuneUpdateQuestButton()
{
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
	const bool bCanSkip = JuneCanSkipEpicQuest( kQuestInfo, m_iSelectedQuestID );

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
			if( true == JuneIsVillageOrBattleFieldState() && mapScene.end() != mapScene.find( m_iSelectedQuestID ) )
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
		// June 323ccbc0 case 6 ( @323cd3a7 ): the remind button needs the story-book singleton 326ab210 / 326ab090 ( Q10 ); until then it
		// stays hidden, as June has it for a quest with no book entry
		break;
	}
}

// ---------------------------------------------------------------------------------------------------------------------
// the detail window
// ---------------------------------------------------------------------------------------------------------------------

/// June 323ce6f0: the title row and the pictures of the detail window ( June's picture numbers, the event NPCs' face )
void CX2UIQuestNew::JuneUpdateDetailQuestTitle()
{
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

	const QUEST_TYPE_SORT eQuestTypeSort = JuneConvertToQuestTypeSort( pQuestTemplet->m_eQuestType );
	const REPEAT_TYPE_SORT eRepeatTypeSort = JuneConvertToRepeatTypeSort( pQuestTemplet->m_eRepeatType );
	pStaticQuestTitle->GetPicture( JuneGetDetailQuestPictureNumber( eQuestTypeSort, eRepeatTypeSort ) )->SetShow( true );

	QUEST_STATE_SORT eQuestStateSort = QSS_AVAILABLE;
	if( NULL != pQuestInst )
		eQuestStateSort = ( true == pQuestInst->IsComplete() ) ? QSS_COMPLETE : QSS_ONGOING;
	pStaticQuestTitle->GetPicture( JuneGetQuestStatePictureNumber( eQuestStateSort ) )->SetShow( true );

	JuneSetNpcFacePicture( pStaticQuestTitle->GetPicture( 13 ), (int)pQuestTemplet->m_eStartNPCID, JuneGetNPCFaceTextureKey( (int)pQuestTemplet->m_eStartNPCID ) );
}

// ---------------------------------------------------------------------------------------------------------------------
// server events and window messages
// ---------------------------------------------------------------------------------------------------------------------

/// June 323e4930: after the talk with an NPC the window goes back to the tab of the selected quest's type
bool CX2UIQuestNew::JuneHandler_EGS_TALK_WITH_NPC_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_TALK_WITH_NPC_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_TALK_WITH_NPC_ACK ) == true && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		if( true == m_bShow )
		{
			JuneReturnToQuestListUI();

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
			JuneSetNowQuestUIMode( eMode );
			ResetQuestUI( true, false, true );
		}
		return true;
	}
	return false;
}

/// June 323e1bf0 cases 0xa053 / 0xa054 ( and the other tab cases ): switch the tab, back to the first row
void CX2UIQuestNew::JuneSelectTab( QUEST_UI_MODE eMode )
{
	JuneQuestUILog( L"[JUNE_QUEST] UI tab %d", (int)eMode );
	JuneSetNowQuestUIMode( eMode );
	m_iTopQuestSlotIndex = 0;
	ResetQuestUI();
}

/// June 323e1bf0 case 0xa055: Button_Skip_Epic_Quest asks first
void CX2UIQuestNew::JuneOpenSkipEpicMsgBox()
{
	JuneQuestUILog( L"[JUNE_QUEST] UI skip epic quest %d: ask", m_iSelectedQuestID );

	if( NULL != m_pDLGMsgBox )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
		m_pDLGMsgBox = NULL;
	}
	m_pDLGMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( 43475 ), UQNCM_SKIP_EPIC_QUEST_OK, g_pMain->GetNowState(), -1 );
}

/// June 323e1bf0 case 0xa056: the skip is a plain QUEST_COMPLETE_REQ for the selected quest with no chosen items ( 323d24f0 ).
/// Whether the server completes a quest whose sub-quests are not done is the server's rule ( the offline emulator: Q4 ).
void CX2UIQuestNew::JuneSkipEpicQuestOk()
{
	if( NULL != m_pDLGMsgBox )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMsgBox, NULL, false );
		m_pDLGMsgBox = NULL;
	}

	if( NULL != g_pData->GetQuestManager()->GetQuestTemplet( m_iSelectedQuestID ) )
	{
		JuneQuestUILog( L"[JUNE_QUEST] UI skip epic quest %d: complete request", m_iSelectedQuestID );
		std::map<int,int> mapNoSelectedItem;
		Handler_EGS_QUEST_COMPLETE_REQ( m_eNPCID, m_iSelectedQuestID, mapNoSelectedItem );
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
static bool JuneQuestSceneLogOk()
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
static bool JuneGetQuestEventSceneInfo( int iQuestID, std::wstring& wstrStart, std::wstring& wstrEnd, std::vector<int>& vecStartMent,
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
void CX2UIQuestNew::JunePlayQuestScene( int iQuestID, int iMode )
{
	// @323cb31f..323cb339: whatever follows, the deferred scene is cleared first
	if( NULL != g_pInstanceData )
		g_pInstanceData->SetJuneDeferredQuestScene( 0, 0 );

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

	if( true == JuneGetQuestEventSceneInfo( iQuestID, wstrStart, wstrEnd, vecStartMent, vecEndMent, iStartFrame, iEndFrame ) )
	{
		if( true == JuneQuestSceneLogOk() )
			CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE quest %d mode %d: row start '%s' frame %d end '%s' frame %d", iQuestID, iMode, wstrStart.c_str(), iStartFrame, wstrEnd.c_str(), iEndFrame );

		if( 0 == iMode || 2 == iMode )		// @323cb3e6..323cb4a2
		{
			if( false == wstrStart.empty() )
			{
				pScene->RegisterMent( wstrStart, vecStartMent );
				pScene->Play( wstrStart, iStartFrame, 0 );
				pQuestManager->JunePlayQuestSelectBGM( pQuestTemplet->m_wstrSelectBGM );
			}
		}
		if( 1 == iMode || 2 == iMode )		// @323cb4a7..323cb55e
		{
			if( false == wstrEnd.empty() )
			{
				pScene->RegisterMent( wstrEnd, vecEndMent );
				pScene->Play( wstrEnd, iEndFrame, 0 );
				pQuestManager->JunePlayQuestSelectBGM( pQuestTemplet->m_wstrSelectBGM );
			}
		}
	}
	else
	{
		if( true == JuneQuestSceneLogOk() )
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

			if( true == JuneGetQuestEventSceneInfo( *it, wstrStart, wstrEnd, vecStartMent, vecEndMent, iStartFrame, iEndFrame ) )
			{
				if( true == JuneQuestSceneLogOk() )
					CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE quest %d mode %d: additional quest %d row start '%s' end '%s'", iQuestID, iMode, *it, wstrStart.c_str(), wstrEnd.c_str() );

				// both are registered, then both are played ( an empty name is dropped by Play ), then the BGM
				pScene->RegisterMent( wstrStart, vecStartMent );
				pScene->RegisterMent( wstrEnd, vecEndMent );
				pScene->Play( wstrStart, iStartFrame, 0 );
				pScene->Play( wstrEnd, iEndFrame, 0 );
				pQuestManager->JunePlayQuestSelectBGM( pQuestTemplet->m_wstrSelectBGM );
			}
		}
	}
}

/// June StateChange 31d88bf0 @31d896bd..31d896fb, at the end of the village and the battle field blocks: the scene NEW_QUEST_NOT deferred in state 5.
/// June skips it while a tutorial guide runs ( 31fbda90 on g_pMain +0x8c4 ): March has no tutorial-guide manager ( Stage 9 ), so it is never skipped.
void CX2UIQuestNew::JunePlayDeferredQuestScene()
{
	if( NULL == g_pInstanceData || NULL == g_pData || NULL == g_pData->GetUIManager() )
		return;

	const int iQuestID = g_pInstanceData->GetJuneDeferredSceneQuestID();
	const int iMode = g_pInstanceData->GetJuneDeferredSceneMode();
	if( iQuestID <= 0 )
		return;

	CX2UIQuestNew* pUIQuestNew = g_pData->GetUIManager()->GetUIQuestNew();
	if( NULL == pUIQuestNew )
		return;

	if( true == JuneQuestSceneLogOk() )
		CX2OfflineLog::Server( L"[JUNE_QUEST] SCENE deferred scene of quest %d ( mode %d ) plays now", iQuestID, iMode );
	pUIQuestNew->JunePlayQuestScene( iQuestID, iMode );
}
#endif SERV_IRUHADEV_JUNE_QUEST_SCENE

#endif SERV_IRUHADEV_JUNE_QUEST_UI
#endif SERV_EPIC_QUEST
