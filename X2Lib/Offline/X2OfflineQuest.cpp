#include "stdafx.h"
#include "X2OfflineQuest.h"

//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-09-03
// Description: Offline mode - quest state (phase 6). See the header.
//
//              Reference: KncWX2Server/GameServer/UserQuestManager.cpp, the
//              SERV_REFORM_QUEST arm of every handler in it - that flag is
//              defined (ServerDefine.h:2827), so the other arm is dead code.
//              The templet accessors are the client's own CX2QuestManager,
//              whose data comes out of the same Lua the server's
//              CXSLQuestManager reads.
//////////////////////////////////////////////////////////////////////////

#ifdef SERV_IRUHADEV_OFFLINE

#include "X2OfflineLog.h"
#include "X2OfflineInventory.h"

CX2OfflineQuest* CX2OfflineQuest::ms_pInstance = NULL;

CX2OfflineQuest::CX2OfflineQuest()
: m_nUnitUID( 0 )
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
, m_nAccountKey( 0 )
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
{
}

CX2OfflineQuest::~CX2OfflineQuest()
{
}

CX2OfflineQuest* CX2OfflineQuest::Instance()
{
	if( NULL == ms_pInstance )
		ms_pInstance = new CX2OfflineQuest;

	return ms_pInstance;
}

void CX2OfflineQuest::Release()
{
	if( NULL == ms_pInstance )
		return;

	delete ms_pInstance;
	ms_pInstance = NULL;
}

//////////////////////////////////////////////////////////////////////////
// templets

/*static*/ const CX2QuestManager::QuestTemplet* CX2OfflineQuest::Templet( int iQuestID )
{
	if( NULL == g_pData || NULL == g_pData->GetQuestManager() )
		return NULL;

	return g_pData->GetQuestManager()->GetQuestTemplet( iQuestID );
}

/*static*/ const CX2QuestManager::SubQuestTemplet* CX2OfflineQuest::SubTemplet( int iSubQuestID )
{
	if( NULL == g_pData || NULL == g_pData->GetQuestManager() )
		return NULL;

	return g_pData->GetQuestManager()->GetSubQuestTemplet( iSubQuestID );
}

/*static*/ void CX2OfflineQuest::LogTempletCensus()
{
	if( NULL == g_pData || NULL == g_pData->GetQuestManager() )
	{
		CX2OfflineLog::Server( L"QUEST    CENSUS there is no quest manager at all" );
		return;
	}

	const std::map< int, CX2QuestManager::QuestTemplet* >& mapAll =
		g_pData->GetQuestManager()->GetMapQuestTemplet();

	int iEpic		= 0;
	int iVisible	= 0;		///< survives GetQuestTemplet's server-group filter
	int iVillage	= 0;		///< epic, and names at least one opening village
	int iDungeon	= 0;
	int iField		= 0;

	std::map< int, CX2QuestManager::QuestTemplet* >::const_iterator mit;
	for( mit = mapAll.begin(); mit != mapAll.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = mit->second;
		if( NULL == pTemplet )
			continue;

		// The same lookup every other caller makes, including the client's own
		// NPC quest lists. A count lower than the map size means the filter is
		// throwing quests away.
		if( NULL != Templet( mit->first ) )
			++iVisible;

		if( CX2QuestManager::QT_EPIC != pTemplet->m_eQuestType )
			continue;

		++iEpic;

		if( false == pTemplet->m_Condition.m_setEnableVillage.empty() )		++iVillage;
		if( false == pTemplet->m_Condition.m_setEnableDungeon.empty() )		++iDungeon;
		if( false == pTemplet->m_Condition.m_setEnableBattleField.empty() )	++iField;
	}

	CX2OfflineLog::Server(
		L"QUEST    CENSUS %u templet(s) loaded, %d visible through the server-group filter"
		L" (client group=%d); %d epic, of which %d open in a village, %d in a dungeon, %d in a field",
		(unsigned int)mapAll.size(), iVisible,
		( NULL != g_pInstanceData ) ? g_pInstanceData->GetServerGroupID() : -999,
		iEpic, iVillage, iDungeon, iField );

	//////////////////////////////////////////////////////////////////////////
	// The early story chain, quest by quest.
	//
	// These four IDs are not guessed - the client names them itself, in
	// CX2PlayGuide::TUTORIAL_QUEST_ID (X2PlayGuide.h:5): visit Ruben, chase the
	// thief, save the El, visit the elf ruins. They are the quests a new
	// character is supposed to be handed in its first few minutes, so printing
	// exactly how each one opens turns "no quest appeared" into a question with
	// an answer.
	//
	// This is read out of the client's own loaded templets on purpose.
	// ScriptData/QuestTable.xls is in the tree and would be easier, but it is
	// the stale snapshot CLAUDE.md warns about - deriving behaviour from a
	// spreadsheet that disagrees with what shipped is exactly the mistake that
	// rule was written about.
	{
		static const int aiChain[] =
		{
			(int)CX2PlayGuide::TQI_VISIT_RUBEN,			///< 11000
			(int)CX2PlayGuide::TQI_CHASE_THIEF,			///< 11005
			(int)CX2PlayGuide::TQI_SAVE_EL,				///< 11010
			(int)CX2PlayGuide::TQI_VISIT_RUIN_OF_ELF,	///< 11030
		};

		for( int i = 0; i < (int)( sizeof( aiChain ) / sizeof( aiChain[0] ) ); ++i )
		{
			std::map< int, CX2QuestManager::QuestTemplet* >::const_iterator mitC =
				mapAll.find( aiChain[i] );

			if( mitC == mapAll.end() || NULL == mitC->second )
			{
				CX2OfflineLog::Server( L"QUEST    CHAIN %d : NOT IN THE SCRIPT AT ALL",
					aiChain[i] );
				continue;
			}

			const CX2QuestManager::QuestTemplet* pT = mitC->second;

			std::wstring wstrBefore;
			for( size_t b = 0; b < pT->m_Condition.m_vecBeforeQuestID.size(); ++b )
			{
				wchar_t szNum[16];
				_snwprintf( szNum, 16, L"%s%d",
					( 0 == b ) ? L"" : L",", pT->m_Condition.m_vecBeforeQuestID[b] );
				szNum[15] = L'\0';
				wstrBefore += szNum;
			}

			std::wstring wstrVillage;
			std::set<int>::const_iterator sit;
			for( sit = pT->m_Condition.m_setEnableVillage.begin();
				 sit != pT->m_Condition.m_setEnableVillage.end(); ++sit )
			{
				wchar_t szNum[16];
				_snwprintf( szNum, 16, L"%s%d",
					wstrVillage.empty() ? L"" : L",", *sit );
				szNum[15] = L'\0';
				wstrVillage += szNum;
			}

			// m_setEnableDungeon is a set<int>, unlike ClearCondition's
			// m_setDungeonID which is a set<DUNGEON_ID>. Easy to mix up.
			std::wstring wstrDungeon;
			for( sit = pT->m_Condition.m_setEnableDungeon.begin();
				 sit != pT->m_Condition.m_setEnableDungeon.end(); ++sit )
			{
				wchar_t szNum[16];
				_snwprintf( szNum, 16, L"%s%d",
					wstrDungeon.empty() ? L"" : L",", *sit );
				szNum[15] = L'\0';
				wstrDungeon += szNum;
			}

			// The successor list, which is what actually advances the story.
			std::wstring wstrAfter;
			for( size_t a = 0; a < pT->m_vecAfterQuestID.size(); ++a )
			{
				wchar_t szNum[16];
				_snwprintf( szNum, 16, L"%s%d",
					( 0 == a ) ? L"" : L",", pT->m_vecAfterQuestID[a] );
				szNum[15] = L'\0';
				wstrAfter += szNum;
			}

			CX2OfflineLog::Server(
				L"QUEST    CHAIN %d '%s': type=%d level=%d limit=%d playLv=%d"
				L" before=[%s] after=[%s] village=[%s] dungeon=[%s] subQuests=%u"
				L" startScene='%s' visibleThroughFilter=%d",
				pT->m_iID, pT->m_wstrTitle.c_str(), (int)pT->m_eQuestType,
				pT->m_Condition.m_iLevel, pT->m_Condition.m_iLimitLevel, pT->m_iPlayLevel,
				wstrBefore.empty()  ? L"-" : wstrBefore.c_str(),
				wstrAfter.empty()   ? L"-" : wstrAfter.c_str(),
				wstrVillage.empty() ? L"-" : wstrVillage.c_str(),
				wstrDungeon.empty() ? L"-" : wstrDungeon.c_str(),
				(unsigned int)pT->m_vecSubQuest.size(),
				pT->m_wstrStartScene.empty() ? L"-" : pT->m_wstrStartScene.c_str(),
				( NULL != Templet( pT->m_iID ) ) ? 1 : 0 );

			// And what each step actually asks for, since a quest that opens but
			// cannot advance is the other half of the same complaint.
			for( size_t sq = 0; sq < pT->m_vecSubQuest.size(); ++sq )
			{
				const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pT->m_vecSubQuest[sq] );

				if( NULL == pSub )
				{
					CX2OfflineLog::Server( L"QUEST    CHAIN   sub %d : NO TEMPLET",
						pT->m_vecSubQuest[sq] );
					continue;
				}

				CX2OfflineLog::Server(
					L"QUEST    CHAIN   sub %d : clearType=%d talkNPC=%d killNum=%d"
					L" item=%d x%d dungeon=%s",
					pSub->m_iID, (int)pSub->m_eClearType,
					(int)pSub->m_ClearCondition.m_eTalkNPCID,
					pSub->m_ClearCondition.m_iKillNum,
					pSub->m_ClearCondition.m_iCollectionItemID,
					pSub->m_ClearCondition.m_iCollectionItemNum,
					DungeonReqString( pSub ).c_str() );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Which sub-quest clear types the shipped script actually uses, and which
	// of those nothing offline can ever tick.
	//
	// This exists because "the quest will not complete" is unfalsifiable
	// otherwise. A type that appears 200 times and has no driver is a bug worth
	// a day; a type that appears zero times is not worth a line of code, and
	// there is no way to tell which is which by reading the enum. The counts
	// come from the client's own loaded templets, so they describe what this
	// install will really run into.
	//
	// Counted by walking the quests rather than the sub-quest table, which
	// needs no accessor added to the client and measures the more useful thing:
	// steps a player can actually be handed. A sub-quest no quest references
	// can never run, and a sub-quest two quests share is two chances to hit it.
	{
		std::map< int, int > mapByType;			///< clear type -> steps reachable

		// And how many of those steps say "at this difficulty or harder". The
		// client's parser used to drop m_bUpperDifficulty for three clear types
		// (22, 26, 27), which made every such step Normal-only and was
		// invisible from the outside. A zero here for 26 or 27 means the read
		// is missing again - the packed script sets the flag on 88 of its 121
		// VISIT_DUNGEON and 39 of its 47 FIND_NPC sub-quests.
		std::map< int, int > mapUpperByType;

		for( mit = mapAll.begin(); mit != mapAll.end(); ++mit )
		{
			// Only quests that survive the server-group filter, since the ones
			// that do not are invisible to the client too.
			const CX2QuestManager::QuestTemplet* pT = Templet( mit->first );
			if( NULL == pT )
				continue;

			for( size_t sq = 0; sq < pT->m_vecSubQuest.size(); ++sq )
			{
				const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pT->m_vecSubQuest[sq] );
				if( NULL != pSub )
				{
					++mapByType[ (int)pSub->m_eClearType ];

					if( true == pSub->m_ClearCondition.m_bUpperDifficulty )
						++mapUpperByType[ (int)pSub->m_eClearType ];
				}
			}
		}

		std::map< int, int >::const_iterator mitT;
		for( mitT = mapByType.begin(); mitT != mapByType.end(); ++mitT )
		{
			const wchar_t* pszWhy = IsClearTypeUndriven( mitT->first );

			CX2OfflineLog::Server( L"QUEST    CENSUS   clearType=%2d : %4d sub-quest(s),"
				L" %4d of them upperDiff=1%s%s",
				mitT->first, mitT->second, mapUpperByType[ mitT->first ],
				( NULL != pszWhy ) ? L"  ** NOT DRIVEN OFFLINE: " : L"",
				( NULL != pszWhy ) ? pszWhy : L"" );
		}
	}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// Quest Q4's done gate: every clear type 0..SQT_END-1 is either in the driven list below ( each has a driver in this file )
	// or named by IsClearTypeUndriven. A type in neither, or in both, is printed; the line must read "0 unclassified, 0 both".
	{
		static const int aiDriven[] =
		{
			CX2QuestManager::SQT_NPC_TALK,			CX2QuestManager::SQT_NPC_HUNT,				CX2QuestManager::SQT_ITEM_COLLECTION,
			CX2QuestManager::SQT_QUEST_ITEM_COLLECTION,	CX2QuestManager::SQT_DUNGEON_TIME,		CX2QuestManager::SQT_DUNGEON_RANK,
			CX2QuestManager::SQT_DUNGEON_DAMAGE,	CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT,	CX2QuestManager::SQT_ITEM_ENCHANT,
			CX2QuestManager::SQT_ITEM_SOCKET,		CX2QuestManager::SQT_USE_SKILL_POINT,		CX2QuestManager::SQT_ITEM_USE,
			CX2QuestManager::SQT_SKILL_USE,			CX2QuestManager::SQT_VISIT_VILLAGE,			CX2QuestManager::SQT_VISIT_FIELD,
			CX2QuestManager::SQT_VISIT_DUNGEON,		CX2QuestManager::SQT_FIND_NPC,				CX2QuestManager::SQT_CHAR_LEVEL_UP,
			CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR,	CX2QuestManager::SQT_LEARN_NEW_SKILL,
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
			CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR,
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
#ifdef SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
			CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR,
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
		};
		const int iDrivenNum = (int)( sizeof( aiDriven ) / sizeof( aiDriven[0] ) );

		int				iDriven		= 0;
		int				iUndriven	= 0;
		std::wstring	wstrNone;
		std::wstring	wstrBoth;

		for( int eType = 0; eType < (int)CX2QuestManager::SQT_END; ++eType )
		{
			bool bDriven = false;
			for( int d = 0; d < iDrivenNum; ++d )
			{
				if( aiDriven[d] == eType )
					bDriven = true;
			}

			const bool bUndriven = ( NULL != IsClearTypeUndriven( eType ) );

			wchar_t szNum[16];
			_snwprintf( szNum, 16, L" %d", eType );
			szNum[15] = L'\0';

			if( true == bDriven && true == bUndriven )
				wstrBoth += szNum;
			else if( true == bDriven )
				++iDriven;
			else if( true == bUndriven )
				++iUndriven;
			else
				wstrNone += szNum;
		}

		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF SQT coverage 0-%d: %d driven, %d undriven, unclassified [%s], both [%s]",
			(int)CX2QuestManager::SQT_END - 1, iDriven, iUndriven,
			wstrNone.empty() ? L"none" : wstrNone.c_str(), wstrBoth.empty() ? L"none" : wstrBoth.c_str() );
	}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	if( 0 == mapAll.size() )
	{
		CX2OfflineLog::Server(
			L"QUEST    CENSUS the quest script is not loaded at all - no quest can ever appear" );
	}
	else if( iVisible < (int)mapAll.size() )
	{
		CX2OfflineLog::Server(
			L"QUEST    CENSUS %u templet(s) are hidden by ADD_SERVER_GROUP: their m_iServerGroupID"
			L" is neither -1 nor the client's group. The client's own NPC quest lists lose them too.",
			(unsigned int)mapAll.size() - iVisible );

		// Which groups the script actually uses, so the fix is a fact rather
		// than a guess: whichever value most of the quests carry is the one the
		// client has to believe it is on.
		std::map< int, int > mapByGroup;

		for( mit = mapAll.begin(); mit != mapAll.end(); ++mit )
		{
			if( NULL != mit->second )
				++mapByGroup[ mit->second->m_iServerGroupID ];
		}

		std::map< int, int >::const_iterator mitG;
		for( mitG = mapByGroup.begin(); mitG != mapByGroup.end(); ++mitG )
		{
			CX2OfflineLog::Server( L"QUEST    CENSUS   m_iServerGroupID=%d : %d quest(s)",
				mitG->first, mitG->second );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CX2OfflineQuest::Clear()
{
	m_nUnitUID = 0;
	m_mapQuesting.clear();
	m_mapComplete.clear();
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	m_nAccountKey = 0;
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
}

bool CX2OfflineQuest::Load( UidType nUnitUID )
{
	if( 0 == nUnitUID )
		return false;

	if( nUnitUID == m_nUnitUID )
		return true;

	Clear();
	m_nUnitUID = nUnitUID;

	// Once per process, and before anything reads a templet.
	static bool s_bCensusDone = false;
	if( false == s_bCensusDone )
	{
		s_bCensusDone = true;
		LogTempletCensus();
	}

	CX2OfflineDB* pDB = CX2OfflineDB::Instance();

	std::vector< KOfflineQuestRow > vecRow;
	pDB->LoadQuests( nUnitUID, vecRow );

	for( size_t i = 0; i < vecRow.size(); ++i )
	{
		const KOfflineQuestRow& kRow = vecRow[i];

		const CX2QuestManager::QuestTemplet* pTemplet = Templet( kRow.m_iQuestID );
		if( NULL == pTemplet )
		{
			// The templet is gone but the row is not. Dropping the row would
			// destroy a save; keeping it in memory without a templet would put
			// an unusable quest in front of the player. Say so and skip it -
			// the row stays on disk, so a build that has the templet again
			// picks the quest back up.
			CX2OfflineLog::Server(
				L"QUEST    WARNING quest %d is in the save but has no templet in this"
				L" build - left on disk, not loaded", kRow.m_iQuestID );
			continue;
		}

		// Rebuilt in TEMPLET order, not in row order: the wire format is
		// positional. Anything the save has no row for starts at zero, which is
		// also what happens when a templet gains a sub-quest between builds.
		KQuestInstance kInst;
		MakeInstance( pTemplet, nUnitUID, kInst );

		for( size_t s = 0; s < kInst.m_vecSubQuestInstance.size(); ++s )
		{
			for( size_t r = 0; r < kRow.m_vecSub.size(); ++r )
			{
				if( kRow.m_vecSub[r].m_iSubID != kInst.m_vecSubQuestInstance[s].m_iID )
					continue;

				kInst.m_vecSubQuestInstance[s].m_ucClearData =
					(UCHAR)kRow.m_vecSub[r].m_iClearData;
				kInst.m_vecSubQuestInstance[s].m_bIsSuccess = kRow.m_vecSub[r].m_bSuccess;
				break;
			}
		}

		m_mapQuesting[ kRow.m_iQuestID ] = kInst;
	}

	std::vector< KOfflineCompleteQuestRow > vecDone;
	pDB->LoadCompleteQuests( nUnitUID, vecDone );

	for( size_t i = 0; i < vecDone.size(); ++i )
	{
		KCompleteQuestInfo kInfo;
		kInfo.m_iQuestID		= vecDone[i].m_iQuestID;
		kInfo.m_iCompleteCount	= vecDone[i].m_iCount;
		kInfo.m_tCompleteDate	= vecDone[i].m_tDate;

		m_mapComplete[ kInfo.m_iQuestID ] = kInfo;
	}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// Quest Q4. The account's quests ( KUserAccountQuestManager::SetAccountQuest, UserAccountQuestManager.cpp:92-192 ): kept per
	// account and folded into this character's two lists, the instance owned by the character ( GSAccountDBThread.cpp:~867 ).
	{
		KOfflineUnitRow kUnit;
		if( true == pDB->LoadUnit( nUnitUID, kUnit ) && 0 < kUnit.m_nUserUID )
			m_nAccountKey = -kUnit.m_nUserUID;

		int iAccountQuesting	= 0;
		int iAccountComplete	= 0;

		if( 0 != m_nAccountKey )
		{
			std::vector< KOfflineQuestRow > vecAccRow;
			pDB->LoadQuests( m_nAccountKey, vecAccRow );

			for( size_t i = 0; i < vecAccRow.size(); ++i )
			{
				const CX2QuestManager::QuestTemplet* pTemplet = Templet( vecAccRow[i].m_iQuestID );
				if( NULL == pTemplet || false == pTemplet->m_bIsAccount )
				{
					CX2OfflineLog::Server( L"[JUNE_QUEST] OFF account row %d has no account templet - left on disk, not loaded",
						vecAccRow[i].m_iQuestID );
					continue;
				}

				KQuestInstance kInst;
				MakeInstance( pTemplet, nUnitUID, kInst );

				for( size_t s = 0; s < kInst.m_vecSubQuestInstance.size(); ++s )
				{
					for( size_t r = 0; r < vecAccRow[i].m_vecSub.size(); ++r )
					{
						if( vecAccRow[i].m_vecSub[r].m_iSubID != kInst.m_vecSubQuestInstance[s].m_iID )
							continue;

						kInst.m_vecSubQuestInstance[s].m_ucClearData	= (UCHAR)vecAccRow[i].m_vecSub[r].m_iClearData;
						kInst.m_vecSubQuestInstance[s].m_bIsSuccess		= vecAccRow[i].m_vecSub[r].m_bSuccess;
						break;
					}
				}

				m_mapQuesting[ vecAccRow[i].m_iQuestID ] = kInst;
				++iAccountQuesting;
			}

			std::vector< KOfflineCompleteQuestRow > vecAccDone;
			pDB->LoadCompleteQuests( m_nAccountKey, vecAccDone );

			for( size_t i = 0; i < vecAccDone.size(); ++i )
			{
				KCompleteQuestInfo kInfo;
				kInfo.m_iQuestID		= vecAccDone[i].m_iQuestID;
				kInfo.m_iCompleteCount	= vecAccDone[i].m_iCount;
				kInfo.m_tCompleteDate	= vecAccDone[i].m_tDate;

				m_mapComplete[ kInfo.m_iQuestID ] = kInfo;
				++iAccountComplete;
			}
		}

		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF account key %I64d: %d account quest(s) in progress, %d completed",
			(__int64)m_nAccountKey, iAccountQuesting, iAccountComplete );
	}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	CX2OfflineLog::Server( L"QUEST    loaded for unitUID=%I64d: %u in progress, %u completed",
		(__int64)nUnitUID, (unsigned int)m_mapQuesting.size(),
		(unsigned int)m_mapComplete.size() );

	return true;
}

/*static*/ void CX2OfflineQuest::MakeInstance( const CX2QuestManager::QuestTemplet* pTemplet,
											   UidType nUnitUID, OUT KQuestInstance& kOut )
{
	kOut.m_iID			= pTemplet->m_iID;
	kOut.m_OwnorUnitUID	= nUnitUID;
	kOut.m_vecSubQuestInstance.clear();

	for( size_t i = 0; i < pTemplet->m_vecSubQuest.size(); ++i )
	{
		KSubQuestInstance kSub;
		kSub.m_iID			= pTemplet->m_vecSubQuest[i];
		kSub.m_ucClearData	= 0;
		kSub.m_bIsSuccess	= false;

		kOut.m_vecSubQuestInstance.push_back( kSub );
	}
}

bool CX2OfflineQuest::Save( int iQuestID ) const
{
	std::map< int, KQuestInstance >::const_iterator mit = m_mapQuesting.find( iQuestID );
	if( mit == m_mapQuesting.end() )
		return false;

	KOfflineQuestRow kRow;
	kRow.m_iQuestID = iQuestID;

	for( size_t i = 0; i < mit->second.m_vecSubQuestInstance.size(); ++i )
	{
		KOfflineSubQuestRow kSub;
		kSub.m_iSubID		= mit->second.m_vecSubQuestInstance[i].m_iID;
		kSub.m_iClearData	= (int)mit->second.m_vecSubQuestInstance[i].m_ucClearData;
		kSub.m_bSuccess		= mit->second.m_vecSubQuestInstance[i].m_bIsSuccess;

		kRow.m_vecSub.push_back( kSub );
	}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	return CX2OfflineDB::Instance()->SaveQuest( SaveKey( iQuestID ), kRow );
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	return CX2OfflineDB::Instance()->SaveQuest( m_nUnitUID, kRow );
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
UidType CX2OfflineQuest::SaveKey( int iQuestID ) const
{
	const CX2QuestManager::QuestTemplet* pTemplet = Templet( iQuestID );

	if( NULL != pTemplet && true == pTemplet->m_bIsAccount && 0 != m_nAccountKey )
		return m_nAccountKey;

	return m_nUnitUID;
}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

void CX2OfflineQuest::MarkChanged( const KQuestInstance& kInst,
								   OUT std::vector< KQuestInstance >& vecChanged )
{
	for( size_t i = 0; i < vecChanged.size(); ++i )
	{
		if( vecChanged[i].m_iID == kInst.m_iID )
		{
			vecChanged[i] = kInst;		///< keep the latest state, not the first
			return;
		}
	}

	vecChanged.push_back( kInst );
}

//////////////////////////////////////////////////////////////////////////
// reads

void CX2OfflineQuest::GetQuestInstances( OUT std::vector< KQuestInstance >& vecOut ) const
{
	vecOut.clear();

	std::map< int, KQuestInstance >::const_iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
		vecOut.push_back( mit->second );
}

void CX2OfflineQuest::GetCompleteQuests( OUT std::vector< KCompleteQuestInfo >& vecOut ) const
{
	vecOut.clear();

	std::map< int, KCompleteQuestInfo >::const_iterator mit;
	for( mit = m_mapComplete.begin(); mit != m_mapComplete.end(); ++mit )
		vecOut.push_back( mit->second );
}

bool CX2OfflineQuest::IsQuesting( int iQuestID ) const
{
	return m_mapQuesting.end() != m_mapQuesting.find( iQuestID );
}

int CX2OfflineQuest::GetCompleteCount( int iQuestID ) const
{
	std::map< int, KCompleteQuestInfo >::const_iterator mit = m_mapComplete.find( iQuestID );
	if( mit == m_mapComplete.end() )
		return 0;

	return mit->second.m_iCompleteCount;
}

//////////////////////////////////////////////////////////////////////////
// shared rules

/*static*/ bool CX2OfflineQuest::IsQuestAdvanceable( const CX2QuestManager::QuestTemplet* pTemplet,
													 int iLevel )
{
	if( NULL == pTemplet )
		return false;

	if( true == pTemplet->m_vecSubQuest.empty() )
		return false;

	// An epic quest below its play level does not advance at all. This is the
	// server's own guard and it matters offline for the same reason: the epic
	// chain hands out several quests at once and only the one the character has
	// grown into should be moving.
	if( CX2QuestManager::QT_EPIC == pTemplet->m_eQuestType &&
		pTemplet->m_iPlayLevel > iLevel )
	{
		return false;
	}

	return true;
}

/*static*/ const wchar_t* CX2OfflineQuest::IsClearTypeUndriven( int eClearType )
{
	switch( (CX2QuestManager::SUB_QUEST_TYPE)eClearType )
	{
	// Nothing offline drives PvP, because there is no PvP offline: no second
	// player, no PvP channel, no arranged match.
	case CX2QuestManager::SQT_PVP_PLAY:
	case CX2QuestManager::SQT_PVP_WIN:
	case CX2QuestManager::SQT_PVP_KILL:
	case CX2QuestManager::SQT_PVP_NPC_HUNT:
	case CX2QuestManager::SQT_PVP_PLAY_ARRANGE:
	case CX2QuestManager::SQT_PVP_TAG_COUNT:
	case CX2QuestManager::SQT_PVP_HERO_NPC_KILL:
	case CX2QuestManager::SQT_PVP_HERO_NPC_PLAY:
		return L"no PvP offline";

	case CX2QuestManager::SQT_WITH_DIF_SERV_USER:
		return L"needs a party member from another server group";

	// These four have no driver in the REAL server either. XSLQuestManager
	// parses them - SQT_FEED_PET and SQT_USER_DIE with empty case bodies and a
	// pair of TODO comments (XSLQuestManager.cpp:1206 and :1213) - and then
	// nothing anywhere calls anything that would move them. Worth saying out
	// loud, because "implement the missing driver" is the wrong instinct here:
	// there is nothing to port.
	case CX2QuestManager::SQT_FEED_PET:
	case CX2QuestManager::SQT_USER_DIE:
	case CX2QuestManager::SQT_HYPER_MODE_USE:
		return L"the shipped GameServer never ticks this type either";

	case CX2QuestManager::SQT_ITEM_ATTRIB:
		return L"the item attribute system is compiled out of this build";

	// SQT_ITEM_ENCHANT and SQT_ITEM_SOCKET both used to be listed here, on the
	// grounds that both features were refused outright. Neither is any more:
	// phase 23 packed EnchantTable.lua and phase 24 SocketItemTable.lua, and
	// OnEnchantItem / OnSocketItem drive the two types. Nothing to return.

	case CX2QuestManager::SQT_ITEM_RESOLVE:
		return L"item disassembly is not implemented offline (no REQ handled)";

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// Quest Q4: SQT_SUITABLE_LEVEL_DUNGEON_CLEAR is driven. The room server's answer is reproducible after all: it is the
	// dungeon row's own level band ( CenterServer Room.cpp:1593-1627, min - 2 <= level <= max + 2 at loading 100 ), which the
	// client has ( CX2Dungeon::DungeonData m_MinLevel / m_MaxLevel ). See IsSuitableLevel and OnDungeonClear.
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// "Was this cleared at a level appropriate to the dungeon?" is not a
	// question this side can answer. It arrives at the GameServer already
	// decided, as m_mapSuitableLevelInfo on the room server's end-of-game
	// packet - one bool per party member, computed where the party is known.
	// Nothing in the client sends or receives it (checked: X2Lib never names
	// the field), so there is no honest local substitute; "am I inside the
	// dungeon's own level band" is a different rule that would agree with the
	// server sometimes and drift silently the rest of the time.
	case CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR:
		return L"suitable-level is decided by the room server, not derivable here";
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	// The live server's own check for this one is unreachable code: it looks
	// the quest up in mapOngoingQuest and then dereferences the iterator on the
	// `== end()` branch (UserQuestManager.cpp:2234). So the type either faults
	// or never completes on live either, and there is no working behaviour to
	// copy. Everything around it - the dungeon exclusions, the equipped-item
	// lookup - is implementable; the rule itself is not knowable.
	case CX2QuestManager::SQT_ITEM_EQUIP_DUNGEON_CLEAR:
		return L"the shipped server's own check dereferences end(); no correct behaviour to copy";

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// Quest Q4: every type 0-48 is either driven or named here ( the census proves it, LogTempletCensus ). June counts and
	// parents are from june2017_re/quest_plan ( q_june_quest_types.tsv; the Q4 Done note ).
	case CX2QuestManager::SQT_NONE:
		return L"not a clear type";

	case CX2QuestManager::SQT_POINT_COUNT:
		return L"the shipped GameServer compiles its driver out ( SERV_POINT_COUNT_SYSTEM off ) and no June sub-quest uses it";

	case CX2QuestManager::SQT_PVE_WIN:
		return L"no June quest uses it ( its 15 sub-quests are orphans ) and its dungeon 41130 has no DungeonData row";

	case CX2QuestManager::SQT_GUILD_JOIN:
	case CX2QuestManager::SQT_GUILD_HONOR_POINT_COLLECTION:
		return L"guild quests are parse-and-log offline ( decision 2026-10-17 )";

	case CX2QuestManager::SQT_PLAYER_WITH_DUNGEON_CLEAR:
	case CX2QuestManager::SQT_PGT_1VS1_CHALLENGE_MODE:
	case CX2QuestManager::SQT_PGT_WINNER:
	case CX2QuestManager::SQT_IMMEDIATELY_CLEAR:
		return L"no June sub-quest uses it and June's clear-condition parser ( 32323ad0 ) has no case for it";

	case CX2QuestManager::SQT_ITEM_EXCHANGE:
		return L"no June quest uses it ( 3 orphan sub-quests ) and the item workshop is not implemented offline";

#ifndef SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
	case CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR:
		return L"a June-server-only rule ( its one sub-quest 35365, quest 35363, counts runs that clear the quest's other steps together ) with no March server code";
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER

	case CX2QuestManager::SQT_PVP_RANK:
		return L"no PvP offline";

	case CX2QuestManager::SQT_PLAY_TOGETHER_WITH_TITLE:
		return L"party-only ( a pair with a titled senior ) and no June quest uses it ( 2 orphan sub-quests )";

#ifndef SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
	case CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR:
		return L"a June-server-only rule; driven only by the stand-in SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN";
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	default:
		break;
	}

	return NULL;
}

/*static*/ std::wstring CX2OfflineQuest::DungeonReqString(
	const CX2QuestManager::SubQuestTemplet* pSub )
{
	if( NULL == pSub )
		return L"<no templet>";

	if( true == pSub->m_ClearCondition.m_setDungeonID.empty() )
		return L"<no dungeon named>";

	std::wstring wstrOut;

	std::set< SEnum::DUNGEON_ID >::const_iterator sit;
	for( sit = pSub->m_ClearCondition.m_setDungeonID.begin();
		 sit != pSub->m_ClearCondition.m_setDungeonID.end(); ++sit )
	{
		wchar_t szNum[32];
		_snwprintf( szNum, 32, L"%s%d", wstrOut.empty() ? L"" : L",", (int)(*sit) );
		szNum[31] = L'\0';
		wstrOut += szNum;
	}

	wstrOut += ( true == pSub->m_ClearCondition.m_bUpperDifficulty )
			 ? L" (upperDiff=1, any difficulty at or above the digit)"
			 : L" (upperDiff=0, exact match only)";

	return wstrOut;
}

/*static*/ bool CX2OfflineQuest::IsExistDungeonInSub( const CX2QuestManager::SubQuestTemplet* pSub,
													  int iDungeonID )
{
	if( NULL == pSub )
		return false;

	if( false == pSub->m_ClearCondition.m_bUpperDifficulty )
	{
		return pSub->m_ClearCondition.m_setDungeonID.end() !=
			   pSub->m_ClearCondition.m_setDungeonID.find(
					(SEnum::DUNGEON_ID)iDungeonID );
	}

	// "this dungeon at this difficulty or harder". The script packs the two
	// into one number - dungeon * 10 + difficulty - so the comparison has to be
	// made on the halves. CXSLQuestManager::IsExistDungeonInSubQuest:2147.
	const int iUserDungeon	= iDungeonID / 10;
	const int iUserDiff		= iDungeonID % 10;

	std::set< SEnum::DUNGEON_ID >::const_iterator sit;
	for( sit = pSub->m_ClearCondition.m_setDungeonID.begin();
		 sit != pSub->m_ClearCondition.m_setDungeonID.end(); ++sit )
	{
		if( iUserDungeon == ( (int)(*sit) / 10 ) && ( (int)(*sit) % 10 ) <= iUserDiff )
			return true;
	}

	return false;
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
/*static*/ bool CX2OfflineQuest::HasDungeonFilter( const CX2QuestManager::SubQuestTemplet* pSub )
{
	if( NULL == pSub )
		return false;

	// CXSLQuestManager::CheckValidDungeonID ( XSLQuestManager.cpp:2099-2107 ): any entry > 0.
	std::set< SEnum::DUNGEON_ID >::const_iterator sit;
	for( sit = pSub->m_ClearCondition.m_setDungeonID.begin();
		 sit != pSub->m_ClearCondition.m_setDungeonID.end(); ++sit )
	{
		if( (int)(*sit) > 0 )
			return true;
	}

	return false;
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
/*static*/ bool CX2OfflineQuest::IsExceptionDungeon( const CX2QuestManager::SubQuestTemplet* pSub, int iDungeonPlusDifficulty )
{
	if( NULL == pSub )
		return false;

	return pSub->m_ClearCondition.m_setExceptionDungeonID.end() !=
		   pSub->m_ClearCondition.m_setExceptionDungeonID.find( (SEnum::DUNGEON_ID)iDungeonPlusDifficulty );
}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN

/*static*/ bool CX2OfflineQuest::IsQuestTutorialDungeon( int iDungeonID )
{
	// Q4 review: CXSLDungeon::IsTutorialDungeon ( XSLDungeon.cpp:820-843 ), all 15 ids. The client shares DungeonEnum.h with the
	// server, so every one is a live enumerator here.
	switch( iDungeonID )
	{
	case SEnum::DI_TUTORIAL_ELSWORD:
	case SEnum::DI_TUTORIAL_ARME:
	case SEnum::DI_TUTORIAL_LIRE:
	case SEnum::DI_TUTORIAL_RAVEN:
	case SEnum::DI_TUTORIAL_EVE:
	case SEnum::DI_TUTORIAL_CHUNG:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_ELSWORD:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_AISHA:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_LENA:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_RAVEN:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_EVE:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_CHUNG:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_ARA:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_EL:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_ADD:
		return true;

	default:
		break;
	}

	return false;
}

/*static*/ bool CX2OfflineQuest::IsQuestCountedDungeon( int iDungeonID )
{
	// UserQuestManager.cpp:2377-2392: !IsTutorialDungeon && !IsHenirDungeon && !IsTCDungeon && !IsRubenDungeon ( the Valentine /
	// Halloween terms are compiled out for US ). Q4 review: transcribed from XSLDungeon.cpp itself; the first version copied
	// Handlers_Skill.cpp's shorter list and missed 13 tutorials, DI_RUBEN_SECRET_HELL and the TC range above RAVEN_0 + 6.
	if( true == IsQuestTutorialDungeon( iDungeonID ) )
		return false;

	switch( iDungeonID )
	{
	case SEnum::DI_ELDER_HENIR_SPACE:			///< IsHenirDungeon ( XSLDungeon.cpp:1152-1161 )

	case SEnum::DI_EL_FOREST_WEST_NORMAL:		///< IsRubenDungeon ( XSLDungeon.cpp:856-878 )
	case SEnum::DI_EL_FOREST_NORTH_NORMAL:
	case SEnum::DI_EL_FOREST_GATE_NORMAL:
	case SEnum::DI_EL_FOREST_HELL_NORMAL:
	case SEnum::DI_EVENT_KIDDAY_RUBEN:
	case SEnum::DI_MONSTER_TEST_NORMAL:
	case SEnum::DI_RUBEN_SECRET_COMMON:
	case SEnum::DI_RUBEN_SECRET_HELL:
	case SEnum::DI_RUBEN_EL_TREE_NORMAL:
	case SEnum::DI_RUBEN_RUIN_OF_ELF_NORMAL:
	case SEnum::DI_RUBEN_SWAMP_NORMAL:
		return false;

	default:
		break;
	}

	// CXSLDungeon::IsTCDungeon ( XSLDungeon.cpp:845-854 ): DI_TRAINING_FREE <= id < DI_END. That takes in every event dungeon
	// above 39000 too, and DI_RANOX_SECRET_COMMON / _HELL ( 41270 / 41271 ). DI_END is 41530 here ( Elysion port ) and in June's
	// DungeonEnum.lua; the March server's 41311 names the same live ids.
#ifdef SERV_IRUHADEV_JUNE_QUEST_STANDIN_RANOX_SECRET
	// Q4 review stand-in ( user 2026-10-19 ): June's Ranox secret dungeon is a real dungeon, not training camp. 41270 is June's
	// DI_RANOX_SECRET_COMMON ( DungeonEnum.lua:36; no March enumerator ), the hell id - 1.
	if( (int)SEnum::DI_RANOX_SECRET_HELL == iDungeonID ||
		(int)SEnum::DI_RANOX_SECRET_HELL - 1 == iDungeonID )
	{
		return true;
	}
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_RANOX_SECRET
	if( iDungeonID >= (int)SEnum::DI_TRAINING_FREE &&
		iDungeonID <  (int)SEnum::DI_END )
	{
		return false;
	}

	return true;
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
/*static*/ bool CX2OfflineQuest::IsTogetherRunMet( const CX2QuestManager::QuestTemplet* pTemplet, int iSelfSubID, int iDungeonID,
												   char cDifficulty, char cRank, int iPlayTime, int iDamage, bool bSuitableLevel,
												   bool bHaveExp )
{
	if( NULL == pTemplet )
		return false;

	// Each other dungeon step is judged on this run by the same filter and rule OnDungeonClear applies to it, its done state ignored.
	int iSteps = 0;

	for( size_t i = 0; i < pTemplet->m_vecSubQuest.size(); ++i )
	{
		if( iSelfSubID == pTemplet->m_vecSubQuest[i] )
			continue;

		const CX2QuestManager::SubQuestTemplet* pOther = SubTemplet( pTemplet->m_vecSubQuest[i] );
		if( NULL == pOther )
			continue;

		bool bMet = false;

		switch( pOther->m_eClearType )
		{
		case CX2QuestManager::SQT_DUNGEON_TIME:
			bMet = ( pOther->m_ClearCondition.m_iDungeonClearTime >= iPlayTime );
			break;

		case CX2QuestManager::SQT_DUNGEON_RANK:
			bMet = ( (char)pOther->m_ClearCondition.m_eDungeonRank <= cRank );
			break;

		case CX2QuestManager::SQT_DUNGEON_DAMAGE:
			bMet = ( pOther->m_ClearCondition.m_iDungeonDamage >= iDamage );
			break;

		case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
			if( CX2QuestManager::QT_EVENT == pTemplet->m_eQuestType || CX2QuestManager::QT_PCBANG == pTemplet->m_eQuestType )
				bMet = ( true == IsQuestCountedDungeon( iDungeonID ) && ( true == HasDungeonFilter( pOther ) || true == bHaveExp ) );
			else
				bMet = ( (int)SEnum::DI_EL_FOREST_GATE_NORMAL != iDungeonID && false == IsQuestTutorialDungeon( iDungeonID ) );
			break;

		case CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR:
			bMet = ( true == IsQuestCountedDungeon( iDungeonID ) && true == bSuitableLevel );
			break;

		case CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR:
			bMet = bHaveExp;
			break;

		default:
			continue;		///< not a dungeon step ( talk, hunt, collect ... ): not part of "together"
		}

		++iSteps;

		if( true == HasDungeonFilter( pOther ) && false == IsExistDungeonInSub( pOther, iDungeonID + (int)cDifficulty ) )
			bMet = false;

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
		if( true == IsExceptionDungeon( pOther, iDungeonID + (int)cDifficulty ) )		// Q4b: the same exclusion, judged on the other step
			bMet = false;
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN

		if( false == bMet )
		{
			CX2OfflineLog::Server( L"[JUNE_QUEST] OFF together %d: step %d ( type %d ) not met this run ( dungeon %d rank %d suitable %d )",
				iSelfSubID, pTemplet->m_vecSubQuest[i], (int)pOther->m_eClearType, iDungeonID, (int)cRank, bSuitableLevel ? 1 : 0 );
			return false;
		}
	}

	if( 0 == iSteps )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF together %d: quest %d has no other dungeon step - not counted", iSelfSubID, pTemplet->m_iID );
		return false;
	}

	return true;
}
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER

/*static*/ bool CX2OfflineQuest::IsSuitableLevel( int iDungeonIDAndDif, int iUnitLevel )
{
	if( NULL == g_pData || NULL == g_pData->GetDungeonManager() )
		return false;

	const CX2Dungeon::DungeonData* pData =
		g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID)iDungeonIDAndDif );

	if( NULL == pData )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF suitable level: dungeon %d has no DungeonData row -> 0", iDungeonIDAndDif );
		return false;
	}

	// Room.cpp:1597-1605: the slack is a hard-coded 2 on both sides ( the server ignores June's m_SuitableLevelFixValue ).
	const int	iMin	= pData->m_MinLevel - 2;
	const int	iMax	= pData->m_MaxLevel + 2;
	const bool	bOK		= ( iMin <= iUnitLevel && iUnitLevel <= iMax );

	CX2OfflineLog::Server( L"[JUNE_QUEST] OFF suitable level: dungeon %d unit level %d band %d..%d -> %d",
		iDungeonIDAndDif, iUnitLevel, iMin, iMax, bOK ? 1 : 0 );

	return bOK;
}

/// Local midnight of tDay's date plus iAddDays days, or of the 1st of the next month when bNextMonth.
static __time64_t JuneQuestLocalMidnight( __time64_t tDay, int iAddDays, bool bNextMonth )
{
	struct tm kTm;
	if( 0 != ::_localtime64_s( &kTm, &tDay ) )
		return 0;

	if( true == bNextMonth )
	{
		kTm.tm_mon	+= 1;		///< _mktime64 rolls December into January of the next year
		kTm.tm_mday	= 1;
	}
	else
	{
		kTm.tm_mday	+= iAddDays;
	}

	kTm.tm_hour		= 0;
	kTm.tm_min		= 0;
	kTm.tm_sec		= 0;
	kTm.tm_isdst	= -1;

	return ::_mktime64( &kTm );
}

int CX2OfflineQuest::CheckJuneRepeat( const CX2QuestManager::QuestTemplet* pTemplet, bool bQuiet ) const
{
	if( NULL == pTemplet )
		return NetError::NET_OK;

	const __time64_t tNow = ::_time64( NULL );

	if( CX2QuestManager::QRT_WEEK == pTemplet->m_eRepeatType ||
		CX2QuestManager::QRT_MONTH == pTemplet->m_eRepeatType )
	{
		std::map< int, KCompleteQuestInfo >::const_iterator mit = m_mapComplete.find( pTemplet->m_iID );
		if( mit != m_mapComplete.end() )
		{
			const __time64_t tDone = (__time64_t)mit->second.m_tCompleteDate;
			if( 0 == tDone )
				return NetError::ERR_QUEST_04;		///< as QRT_DAY treats an undated completion

			__time64_t tExpiry = 0;

			if( CX2QuestManager::QRT_WEEK == pTemplet->m_eRepeatType )
			{
				// June 32322050: the next Wednesday 00:00 after the completion's date. CTime::GetDayOfWeek is 1 ( Sunday ) .. 7;
				// Wed..Sat add 11 - dow, Sun..Tue add 4 - dow, so a Wednesday completion waits a full week. Its "same Wednesday,
				// hour < 0" branch can never be taken ( Ghidra marks it unreachable ).
				struct tm kDone;
				if( 0 != ::_localtime64_s( &kDone, &tDone ) )
					return NetError::ERR_QUEST_04;

				const int iDow	= kDone.tm_wday + 1;
				const int iDays	= ( iDow > 3 ) ? ( 11 - iDow ) : ( 4 - iDow );

				tExpiry = JuneQuestLocalMidnight( tDone, iDays, false );
			}
			else
			{
				// June 32321d10: the 1st of the next month, 00:00 ( its "the 1st, hour < 0" branch is dead the same way ).
				tExpiry = JuneQuestLocalMidnight( tDone, 0, true );
			}

			if( tExpiry > tNow )
			{
				if( false == bQuiet )
				{
					CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d refused: %s quest resets in %I64d s",
						pTemplet->m_iID, ( CX2QuestManager::QRT_WEEK == pTemplet->m_eRepeatType ) ? L"weekly" : L"monthly",
						(__int64)( tExpiry - tNow ) );
				}

				return NetError::ERR_QUEST_04;
			}
		}
	}

	// June 3232caf0: a quest of a chain ( m_iChainID, ChainQuestInfo.lua ) waits until local midnight of ( the chain's latest
	// completion + AllowFewDays days ). Its TestMinute variant runs only under code event 0x10, which nothing offline enables.
	if( 0 != pTemplet->m_iChainID && NULL != g_pData && NULL != g_pData->GetQuestManager() )
	{
		const std::map< int, CX2QuestManager::ChainQuestData >& mapChain = g_pData->GetQuestManager()->GetChainQuestMap();

		std::map< int, CX2QuestManager::ChainQuestData >::const_iterator mitChain = mapChain.find( pTemplet->m_iChainID );
		if( mitChain != mapChain.end() && 0 < mitChain->second.m_iAllowFewDays )
		{
			__int64	tLast		= 0;
			int		iLastQuest	= 0;

			std::set< int >::const_iterator sit;
			for( sit = mitChain->second.m_setQuestID.begin(); sit != mitChain->second.m_setQuestID.end(); ++sit )
			{
				std::map< int, KCompleteQuestInfo >::const_iterator mitDone = m_mapComplete.find( *sit );
				if( mitDone != m_mapComplete.end() && mitDone->second.m_tCompleteDate > tLast )
				{
					tLast		= mitDone->second.m_tCompleteDate;
					iLastQuest	= *sit;
				}
			}

			if( 0 < tLast )
			{
				const __time64_t tAvail = JuneQuestLocalMidnight( (__time64_t)tLast, mitChain->second.m_iAllowFewDays, false );

				if( tAvail > tNow )
				{
					if( false == bQuiet )
					{
						CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d refused: chain %d ( last quest %d ) opens in %I64d s",
							pTemplet->m_iID, pTemplet->m_iChainID, iLastQuest, (__int64)( tAvail - tNow ) );
					}

					return NetError::ERR_QUEST_04;
				}
			}
		}
	}

	return NetError::NET_OK;
}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

/*static*/ bool CX2OfflineQuest::IsSubComplete( const CX2QuestManager::SubQuestTemplet* pSub,
												const KSubQuestInstance& kInst )
{
	if( NULL == pSub )
		return false;

	// The collection types carry no counter. The bag is the state, exactly as
	// CX2QuestManager::SubQuestInst::IsComplete decides it on the client.
	if( CX2QuestManager::SQT_ITEM_COLLECTION == pSub->m_eClearType ||
		CX2QuestManager::SQT_QUEST_ITEM_COLLECTION == pSub->m_eClearType )
	{
		const int iHave = CX2OfflineInventory::Instance()->CountItemByID(
			pSub->m_ClearCondition.m_iCollectionItemID, true );

		return ( iHave >= pSub->m_ClearCondition.m_iCollectionItemNum );
	}

	return kInst.m_bIsSuccess;
}

bool CX2OfflineQuest::CheckBeforeGroup( const CX2QuestManager::QuestTemplet* pTemplet,
										int iSubQuestID ) const
{
	if( NULL == pTemplet )
		return false;

	// A quest with no grouping is a flat checklist: every sub-quest is always
	// live. The real server returns false here instead - it logs "could not
	// find the group" and refuses - which is safe for it because every shipped
	// quest is grouped. Offline the templets come from the same script, so the
	// case should not arise; treating it as "no staging" rather than as "never
	// advances" means a quest that somehow lacks groups is playable instead of
	// permanently stuck, and the log line says which one it was.
	if( true == pTemplet->m_mapSubQuestGroup.empty() )
		return true;

	int iGroupID = 0;

	std::map< int, std::vector<int> >::const_iterator mitGroup;
	for( mitGroup = pTemplet->m_mapSubQuestGroup.begin();
		 mitGroup != pTemplet->m_mapSubQuestGroup.end() && 0 == iGroupID; ++mitGroup )
	{
		for( size_t i = 0; i < mitGroup->second.size(); ++i )
		{
			if( mitGroup->second[i] == iSubQuestID )
			{
				iGroupID = mitGroup->first;
				break;
			}
		}
	}

	if( 0 == iGroupID )
	{
		CX2OfflineLog::Server(
			L"QUEST    WARNING quest %d has groups but sub-quest %d is in none of them",
			pTemplet->m_iID, iSubQuestID );
		return true;
	}

	std::map< int, KQuestInstance >::const_iterator mit = m_mapQuesting.find( pTemplet->m_iID );
	if( mit == m_mapQuesting.end() )
		return false;

	const KQuestInstance& kInst = mit->second;

	for( mitGroup = pTemplet->m_mapSubQuestGroup.begin();
		 mitGroup != pTemplet->m_mapSubQuestGroup.end(); ++mitGroup )
	{
		if( mitGroup->first >= iGroupID )
			continue;

		for( size_t i = 0; i < mitGroup->second.size(); ++i )
		{
			const int iEarlierSub = mitGroup->second[i];

			for( size_t s = 0; s < kInst.m_vecSubQuestInstance.size(); ++s )
			{
				if( kInst.m_vecSubQuestInstance[s].m_iID != iEarlierSub )
					continue;

				// IsSubComplete rather than the raw flag, so a collection step
				// counts as done the moment the items are in the bag - which is
				// the special case the server spells out at
				// UserQuestManager.cpp:7330.
				if( false == IsSubComplete( SubTemplet( iEarlierSub ),
											kInst.m_vecSubQuestInstance[s] ) )
				{
					return false;
				}

				break;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// accept

int CX2OfflineQuest::Accept( int iQuestID, int iTalkNPCID, const KOfflineUnitRow& kRow,
							 bool bAfterQuest, OUT KQuestInstance& kOut,
							 bool bQuiet /*= false*/ )
{
	const CX2QuestManager::QuestTemplet* pTemplet = Templet( iQuestID );
	if( NULL == pTemplet )
	{
		if( false == bQuiet )
			CX2OfflineLog::Server( L"QUEST    accept %d refused: no such quest templet", iQuestID );

		return NetError::ERR_QUEST_03;
	}

	// PC-bang-only quests. There is no PC bang offline and there is no way to
	// become one, so this is a refusal rather than a relaxation - taking the
	// quest would leave it permanently in the list with no way to finish it.
	if( true == pTemplet->m_bIsPcBang )
	{
		if( false == bQuiet )
			CX2OfflineLog::Server( L"QUEST    accept %d refused: PC-bang only", iQuestID );

		return NetError::ERR_QUEST_16;
	}

	// A timed event quest is normally checked against the live event schedule
	// (KUserQuestManager::IsExistEventQuest, fed by the GameServer's event
	// table). There is no schedule offline and no table to build one from, so
	// the check is skipped rather than faked - the alternative is refusing
	// every event quest forever, which is a bigger divergence than allowing
	// one. Logged so it is visible in the packet trail.
	if( true == pTemplet->m_bIsTimeEvent && false == bQuiet )
	{
		CX2OfflineLog::Server(
			L"QUEST    accept %d is a timed event quest - offline has no event schedule,"
			L" allowing it", iQuestID );
	}

	if( CX2QuestManager::QT_EPIC != pTemplet->m_eQuestType &&
		CX2QuestManager::QT_EVENT != pTemplet->m_eQuestType )
	{
		int iNormal = 0;

		std::map< int, KQuestInstance >::const_iterator mit;
		for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
		{
			const CX2QuestManager::QuestTemplet* pOther = Templet( mit->first );
			if( NULL == pOther )
				continue;

			if( CX2QuestManager::QT_EPIC != pOther->m_eQuestType &&
				CX2QuestManager::QT_EVENT != pOther->m_eQuestType )
			{
				++iNormal;
			}
		}

		if( iNormal >= (int)MAX_ING_QUEST_NUM )
			return NetError::ERR_QUEST_28;
	}

	if( true == IsQuesting( iQuestID ) )
		return NetError::ERR_QUEST_01;

	// One job-advancement quest at a time.
	if( CX2QuestManager::QT_CHANGE_JOB == pTemplet->m_eQuestType )
	{
		std::map< int, KQuestInstance >::const_iterator mit;
		for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
		{
			const CX2QuestManager::QuestTemplet* pOther = Templet( mit->first );

			if( NULL != pOther && CX2QuestManager::QT_CHANGE_JOB == pOther->m_eQuestType )
			{
				if( false == bQuiet )
				{
					CX2OfflineLog::Server(
						L"QUEST    accept %d refused: change-job quest %d is already in progress",
						iQuestID, mit->first );
				}

				return NetError::ERR_QUEST_17;
			}
		}
	}

	// Repeatability.
	if( CX2QuestManager::QRT_NORMAL == pTemplet->m_eRepeatType )
	{
		if( GetCompleteCount( iQuestID ) > 0 )
			return NetError::ERR_QUEST_04;
	}
	else if( CX2QuestManager::QRT_DAY == pTemplet->m_eRepeatType )
	{
		std::map< int, KCompleteQuestInfo >::const_iterator mit = m_mapComplete.find( iQuestID );
		if( mit != m_mapComplete.end() )
		{
			// The daily boundary is 6am, not midnight: a quest finished before
			// 6am is available again at 6am the same day, one finished after is
			// available at 6am tomorrow. UserQuestManager.cpp:527.
			const __int64 tDone = mit->second.m_tCompleteDate;

			if( 0 == tDone )
				return NetError::ERR_QUEST_04;

			struct tm kDone;
			const __time64_t tDoneT = (__time64_t)tDone;

			if( 0 != ::_localtime64_s( &kDone, &tDoneT ) )
				return NetError::ERR_QUEST_04;

			struct tm kBoundary = kDone;
			kBoundary.tm_hour	= (int)DAILY_REPEAT_HOUR;
			kBoundary.tm_min	= 0;
			kBoundary.tm_sec	= 0;

			__time64_t tBoundary = ::_mktime64( &kBoundary );

			if( kDone.tm_hour >= (int)DAILY_REPEAT_HOUR )
				tBoundary += 24 * 60 * 60;

			if( tBoundary > ::_time64( NULL ) )
				return NetError::ERR_QUEST_04;
		}
	}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// Quest Q4: June's weekly / monthly expiry and the chain-day rule. The March server has neither; June's client applies
	// them in SetUnitQuest / IsChainQuestAvailable, so the emulator refuses the accept the client would not offer.
	{
		const int iJune = CheckJuneRepeat( pTemplet, bQuiet );
		if( NetError::NET_OK != iJune )
			return iJune;
	}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	// Character conditions. A quest names either a unit type or a set of
	// classes, never both - the client asserts on that when it loads the
	// templet (X2QuestManager.cpp:158).
	if( CX2Unit::UT_NONE != pTemplet->m_Condition.m_eUnitType )
	{
		CX2Unit::UNIT_TYPE eType = CX2Unit::UT_NONE;

		if( NULL != g_pData && NULL != g_pData->GetUnitManager() )
		{
			const CX2Unit::UnitTemplet* pUnitTemplet =
				g_pData->GetUnitManager()->GetUnitTemplet( (CX2Unit::UNIT_CLASS)kRow.m_iUnitClass );

			if( NULL != pUnitTemplet )
				eType = pUnitTemplet->m_UnitType;
		}

		if( pTemplet->m_Condition.m_eUnitType != eType )
			return NetError::ERR_QUEST_05;
	}

	if( false == pTemplet->m_Condition.m_setUnitClass.empty() )
	{
		if( pTemplet->m_Condition.m_setUnitClass.end() ==
			pTemplet->m_Condition.m_setUnitClass.find( (CX2Unit::UNIT_CLASS)kRow.m_iUnitClass ) )
		{
			return NetError::ERR_QUEST_05;
		}
	}

	for( size_t i = 0; i < pTemplet->m_Condition.m_vecBeforeQuestID.size(); ++i )
	{
		if( 0 == GetCompleteCount( pTemplet->m_Condition.m_vecBeforeQuestID[i] ) )
		{
			if( false == bQuiet )
			{
				CX2OfflineLog::Server( L"QUEST    accept %d refused: quest %d is not completed yet",
					iQuestID, pTemplet->m_Condition.m_vecBeforeQuestID[i] );
			}

			return NetError::ERR_QUEST_06;
		}
	}

	// Only a quest the player clicked on an NPC is checked against that NPC.
	if( false == bAfterQuest )
	{
		if( CX2QuestManager::QT_EPIC != pTemplet->m_eQuestType &&
			(int)pTemplet->m_eStartNPCID != iTalkNPCID &&
			(int)CX2UnitManager::NUI_BILLBOARD != iTalkNPCID )
		{
			if( false == bQuiet )
			{
				CX2OfflineLog::Server( L"QUEST    accept %d refused: NPC %d does not offer it (start NPC is %d)",
					iQuestID, iTalkNPCID, (int)pTemplet->m_eStartNPCID );
			}

			return NetError::ERR_QUEST_07;
		}
	}

	if( pTemplet->m_Condition.m_iLevel > kRow.m_iLevel )
		return NetError::ERR_QUEST_12;

	if( -1 != pTemplet->m_Condition.m_iLimitLevel &&
		pTemplet->m_Condition.m_iLimitLevel < kRow.m_iLevel )
	{
		return NetError::ERR_QUEST_27;
	}

	// The daily random-quest rotation (m_iRandomGroupID) is picked by the
	// GameServer from a table it seeds at midnight. There is no such table
	// offline, so the check is skipped and the quest is allowed - see the
	// timed-event note above; the same reasoning applies.
	if( pTemplet->m_iRandomGroupID > 0 && false == bQuiet )
	{
		CX2OfflineLog::Server(
			L"QUEST    accept %d is in random group %d - offline has no daily rotation,"
			L" allowing it", iQuestID, pTemplet->m_iRandomGroupID );
	}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	// Q4b. The last refusal: the accept-time items ( UserQuestManager.cpp:694 runs it right before the DB write ). Nothing is
	// inserted unless the quest is then taken.
	std::vector< KInventoryItemInfo > vecAcceptSlots;
	{
		const int iAccept = PayAcceptItems( pTemplet, vecAcceptSlots );
		if( NetError::NET_OK != iAccept )
			return iAccept;
	}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	MakeInstance( pTemplet, m_nUnitUID, kOut );
	m_mapQuesting[ iQuestID ] = kOut;

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	if( false == vecAcceptSlots.empty() )
		m_mapAcceptSlots[ iQuestID ] = vecAcceptSlots;
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	Save( iQuestID );

	CX2OfflineLog::Server( L"QUEST    accepted %d '%s' (%u sub-quest(s), from NPC %d)",
		iQuestID, pTemplet->m_wstrTitle.c_str(),
		(unsigned int)pTemplet->m_vecSubQuest.size(), iTalkNPCID );

	return NetError::NET_OK;
}

//////////////////////////////////////////////////////////////////////////
// auto-open - how the story chain is actually handed out

void CX2OfflineQuest::CheckAutoOpen( AUTO_OPEN_PLACE ePlace, int iPlaceID,
									 const KOfflineUnitRow& kRow,
									 OUT std::vector< KQuestInstance >& vecOpened )
{
	if( NULL == g_pData || NULL == g_pData->GetQuestManager() )
		return;

	// The server keeps two prebuilt lists - every QT_EPIC templet, and the
	// subset of those with a non-empty before-quest list
	// (CXSLQuestManager::m_vecEpicQuestTemplet / m_vecExistBeforeEpicQuestTemplet,
	// XSLQuestManager.cpp:336-404). Neither exists on the client, but both are
	// just filters over the templet map, so they are applied inline here.
	const std::map< int, CX2QuestManager::QuestTemplet* >& mapAll =
		g_pData->GetQuestManager()->GetMapQuestTemplet();

	int						iCandidates = 0;	///< passed the place test
	std::map< int, int >	mapRefused;			///< NetError -> how many

	std::map< int, CX2QuestManager::QuestTemplet* >::const_iterator mit;
	for( mit = mapAll.begin(); mit != mapAll.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = mit->second;

		if( NULL == pTemplet )
			continue;

		// The place sweeps walk the server's epic-only lists; the after-quest
		// sweep does not - CheckEpicQuest_NewQuestByAfterQuest opens whatever
		// the finished quest names, and with bForce (which a completion always
		// passes) it does not filter on type at all
		// (UserQuestManager.cpp:6136-6152).
		if( AOP_AFTER_QUEST != ePlace &&
			CX2QuestManager::QT_EPIC != pTemplet->m_eQuestType )
		{
			continue;
		}

		if( true == IsQuesting( pTemplet->m_iID ) )
			continue;

		if( 0 < GetCompleteCount( pTemplet->m_iID ) )
			continue;

		bool bPlaceOK = false;

		switch( ePlace )
		{
		case AOP_BEFORE_QUEST:
			{
				// "Every quest this one waits on is finished." A quest with no
				// before-list is deliberately NOT opened by this path - it is
				// the first link of a chain and belongs to a place instead
				// (UserQuestManager.cpp:6068 sets bIsSuccessEpic = false for an
				// empty list). Accept() checks the same prerequisites again and
				// is the authority; this is only the filter that decides which
				// quests are worth offering it.
				if( true == pTemplet->m_Condition.m_vecBeforeQuestID.empty() )
					break;

				bPlaceOK = true;

				for( size_t i = 0; i < pTemplet->m_Condition.m_vecBeforeQuestID.size(); ++i )
				{
					const int iBefore = pTemplet->m_Condition.m_vecBeforeQuestID[i];

					if( 0 != iBefore && 0 == GetCompleteCount( iBefore ) )
					{
						bPlaceOK = false;
						break;
					}
				}
			}
			break;

		case AOP_VILLAGE:
			{
				// An empty set means "this quest does not open in a village",
				// not "any village" - the server `continue`s on the empty case
				// (UserQuestManager.cpp:6008).
				bPlaceOK = ( false == pTemplet->m_Condition.m_setEnableVillage.empty() &&
							 pTemplet->m_Condition.m_setEnableVillage.end() !=
								pTemplet->m_Condition.m_setEnableVillage.find( iPlaceID ) );
			}
			break;

		case AOP_DUNGEON:
			{
				if( iPlaceID <= 0 )
					break;

				// The dungeon and battlefield paths additionally refuse a quest
				// above the character's play level, and one with no sub-quests;
				// the village path does neither. That asymmetry is the real
				// server's (compare :7375-7382 with :5978) and is left alone: a
				// village quest opened below its play level sits in the list
				// until the character grows into it, which is what a story chain
				// looks like from the player's side.
				if( pTemplet->m_iPlayLevel > kRow.m_iLevel )
					break;

				if( true == pTemplet->m_vecSubQuest.empty() )
					break;

				bPlaceOK = ( false == pTemplet->m_Condition.m_setEnableDungeon.empty() &&
							 pTemplet->m_Condition.m_setEnableDungeon.end() !=
								pTemplet->m_Condition.m_setEnableDungeon.find( iPlaceID ) );
			}
			break;

		case AOP_BATTLE_FIELD:
			{
				if( iPlaceID <= 0 )
					break;

				if( pTemplet->m_iPlayLevel > kRow.m_iLevel )
					break;

				if( true == pTemplet->m_vecSubQuest.empty() )
					break;

				bPlaceOK = ( false == pTemplet->m_Condition.m_setEnableBattleField.empty() &&
							 pTemplet->m_Condition.m_setEnableBattleField.end() !=
								pTemplet->m_Condition.m_setEnableBattleField.find( iPlaceID ) );
			}
			break;

		case AOP_AFTER_QUEST:
			{
				// Named in the just-finished quest's own after-list. The list
				// belongs to iPlaceID, not to the candidate, so this is the one
				// case where the test reads a *different* templet.
				const CX2QuestManager::QuestTemplet* pDone = Templet( iPlaceID );
				if( NULL == pDone )
					break;

				for( size_t a = 0; a < pDone->m_vecAfterQuestID.size(); ++a )
				{
					if( pDone->m_vecAfterQuestID[a] == pTemplet->m_iID )
					{
						bPlaceOK = true;
						break;
					}
				}
			}
			break;

		default:
			break;
		}

		if( false == bPlaceOK )
			continue;

		++iCandidates;

		KQuestInstance kInst;

		// Quiet: this offers every epic quest that names this place and expects
		// most of them to be refused on level or on a prerequisite. The one that
		// takes logs itself; the refusals are counted and summarised below.
		const int iResult = Accept( pTemplet->m_iID, (int)CX2UnitManager::NUI_NONE,
									kRow, true, kInst, true );

		if( NetError::NET_OK != iResult )
		{
			++mapRefused[ iResult ];
			continue;
		}

		vecOpened.push_back( kInst );
	}

	//////////////////////////////////////////////////////////////////////////
	// The summary is written whenever anything was even considered, not only
	// when something opened.
	//
	// "No quest appeared" is the failure this phase actually shipped with, and
	// silence cannot distinguish its three causes: the sweep did not run, it ran
	// and nothing named this place, or it ran and everything it found was
	// refused. One line per sweep that had candidates makes all three legible,
	// and a sweep with no candidates at all is the common case and stays quiet.
	if( 0 == iCandidates )
		return;

	static const wchar_t* const PLACE[] =
		{ L"a finished prerequisite", L"village", L"dungeon", L"field",
		  L"the after-quest list of" };

	const wchar_t* szPlace =
		( ePlace >= AOP_BEFORE_QUEST && ePlace <= AOP_AFTER_QUEST ) ? PLACE[ ePlace ] : L"?";

	CX2OfflineLog::Server( L"QUEST    auto-open by %s %d: %d candidate(s), %u opened",
		szPlace, iPlaceID, iCandidates, (unsigned int)vecOpened.size() );

	// Why the rest did not take. ERR_QUEST_12 (level too low) is the ordinary
	// answer and means the chain is simply ahead of the character; anything else
	// is worth looking at.
	std::map< int, int >::const_iterator mitR;
	for( mitR = mapRefused.begin(); mitR != mapRefused.end(); ++mitR )
	{
		CX2OfflineLog::Server( L"QUEST    auto-open   %d refused with error %d%s",
			mitR->second, mitR->first,
			( NetError::ERR_QUEST_12 == mitR->first ) ? L" (level too low - normal)" : L"" );
	}
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
void CX2OfflineQuest::CheckAccountOpen( const KOfflineUnitRow& kRow, OUT std::vector< KQuestInstance >& vecOpened )
{
	if( NULL == g_pData || NULL == g_pData->GetQuestManager() || 0 == m_nAccountKey )
		return;

	const std::map< int, CX2QuestManager::QuestTemplet* >& mapAll = g_pData->GetQuestManager()->GetMapQuestTemplet();

	int						iCandidates = 0;
	std::map< int, int >	mapRefused;			///< NetError -> how many

	std::map< int, CX2QuestManager::QuestTemplet* >::const_iterator mit;
	for( mit = mapAll.begin(); mit != mapAll.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( NULL == pTemplet || false == pTemplet->m_bIsAccount )
			continue;

		// UserAccountQuestManager.cpp:375-386: not running and not in the completed map.
		if( true == IsQuesting( pTemplet->m_iID ) || 0 < GetCompleteCount( pTemplet->m_iID ) )
			continue;

		++iCandidates;

		// Handler_EGS_NEW_QUEST_REQ_Account with NUI_NONE: no start-NPC check, the accept rules otherwise apply.
		KQuestInstance kInst;
		const int iResult = Accept( pTemplet->m_iID, (int)CX2UnitManager::NUI_NONE, kRow, true, kInst, true );

		if( NetError::NET_OK != iResult )
		{
			++mapRefused[ iResult ];
			continue;
		}

		vecOpened.push_back( kInst );
	}

	CX2OfflineLog::Server( L"[JUNE_QUEST] OFF account auto-open: %d candidate(s), %u opened",
		iCandidates, (unsigned int)vecOpened.size() );

	std::map< int, int >::const_iterator mitR;
	for( mitR = mapRefused.begin(); mitR != mapRefused.end(); ++mitR )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF account auto-open   %d refused with error %d%s",
			mitR->second, mitR->first,
			( NetError::ERR_QUEST_12 == mitR->first ) ? L" (level too low - normal)" : L"" );
	}
}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

int CX2OfflineQuest::CountWaitingOn( int iQuestID ) const
{
	if( NULL == g_pData || NULL == g_pData->GetQuestManager() )
		return 0;

	const std::map< int, CX2QuestManager::QuestTemplet* >& mapAll =
		g_pData->GetQuestManager()->GetMapQuestTemplet();

	int iCount = 0;

	std::map< int, CX2QuestManager::QuestTemplet* >::const_iterator mit;
	for( mit = mapAll.begin(); mit != mapAll.end(); ++mit )
	{
		if( NULL == mit->second )
			continue;

		for( size_t i = 0; i < mit->second->m_Condition.m_vecBeforeQuestID.size(); ++i )
		{
			if( mit->second->m_Condition.m_vecBeforeQuestID[i] == iQuestID )
			{
				++iCount;
				break;
			}
		}
	}

	return iCount;
}

//////////////////////////////////////////////////////////////////////////
// complete

int CX2OfflineQuest::CheckCompletable( int iQuestID, const KOfflineUnitRow& kRow ) const
{
	std::map< int, KQuestInstance >::const_iterator mit = m_mapQuesting.find( iQuestID );
	if( mit == m_mapQuesting.end() )
		return NetError::ERR_QUEST_03;

	const CX2QuestManager::QuestTemplet* pTemplet = Templet( iQuestID );
	if( NULL == pTemplet )
		return NetError::ERR_QUEST_08;

	if( true == pTemplet->m_bIsPcBang )
		return NetError::ERR_QUEST_20;

	// The real server has a bare `goto error_proc` here, which leaves the error
	// at NET_OK and sends the client an "OK" that hands out nothing - a hang
	// rather than a message. ERR_QUEST_12 is the level-too-low code the accept
	// path uses, and it puts a readable reason on screen instead.
	if( CX2QuestManager::QT_EPIC == pTemplet->m_eQuestType &&
		pTemplet->m_Condition.m_iLevel > kRow.m_iLevel )
	{
		return NetError::ERR_QUEST_12;
	}

	const KQuestInstance& kInst = mit->second;

	for( size_t i = 0; i < kInst.m_vecSubQuestInstance.size(); ++i )
	{
		const CX2QuestManager::SubQuestTemplet* pSub =
			SubTemplet( kInst.m_vecSubQuestInstance[i].m_iID );

		if( NULL == pSub )
			return NetError::ERR_QUEST_08;

		if( false == IsSubComplete( pSub, kInst.m_vecSubQuestInstance[i] ) )
		{
			CX2OfflineLog::Server(
				L"QUEST    complete %d refused: sub-quest %d (type %d) is not done",
				iQuestID, kInst.m_vecSubQuestInstance[i].m_iID, (int)pSub->m_eClearType );
			return NetError::ERR_QUEST_09;
		}
	}

	return NetError::NET_OK;
}

bool CX2OfflineQuest::Finish( int iQuestID, OUT KCompleteQuestInfo& kOut )
{
	std::map< int, KQuestInstance >::iterator mit = m_mapQuesting.find( iQuestID );
	if( mit == m_mapQuesting.end() )
		return false;

	m_mapQuesting.erase( mit );
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	CX2OfflineDB::Instance()->DeleteQuest( SaveKey( iQuestID ), iQuestID );
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	CX2OfflineDB::Instance()->DeleteQuest( m_nUnitUID, iQuestID );
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	std::map< int, KCompleteQuestInfo >::iterator mitC = m_mapComplete.find( iQuestID );

	if( mitC == m_mapComplete.end() )
	{
		KCompleteQuestInfo kInfo;
		kInfo.m_iQuestID		= iQuestID;
		kInfo.m_iCompleteCount	= 1;
		kInfo.m_tCompleteDate	= (__int64)::_time64( NULL );

		m_mapComplete[ iQuestID ] = kInfo;
	}
	else
	{
		++( mitC->second.m_iCompleteCount );
		mitC->second.m_tCompleteDate = (__int64)::_time64( NULL );
	}

	kOut = m_mapComplete[ iQuestID ];

	KOfflineCompleteQuestRow kRow;
	kRow.m_iQuestID	= kOut.m_iQuestID;
	kRow.m_iCount	= kOut.m_iCompleteCount;
	kRow.m_tDate	= kOut.m_tCompleteDate;

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	CX2OfflineDB::Instance()->SaveCompleteQuest( SaveKey( iQuestID ), kRow );
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	CX2OfflineDB::Instance()->SaveCompleteQuest( m_nUnitUID, kRow );
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	return true;
}

bool CX2OfflineQuest::GiveUp( int iQuestID )
{
	std::map< int, KQuestInstance >::iterator mit = m_mapQuesting.find( iQuestID );
	if( mit == m_mapQuesting.end() )
		return false;

	m_mapQuesting.erase( mit );
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	CX2OfflineDB::Instance()->DeleteQuest( SaveKey( iQuestID ), iQuestID );
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
	CX2OfflineDB::Instance()->DeleteQuest( m_nUnitUID, iQuestID );
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

	CX2OfflineLog::Server( L"QUEST    gave up %d", iQuestID );
	return true;
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
//////////////////////////////////////////////////////////////////////////
// Q4b. accept-time reward items ( June's m_vecAcceptRewardItem, March's server m_mapAcceptRewardItem )

int CX2OfflineQuest::PayAcceptItems( const CX2QuestManager::QuestTemplet* pTemplet, OUT std::vector< KInventoryItemInfo >& vecSlots )
{
	vecSlots.clear();

	if( NULL == pTemplet || true == pTemplet->m_mapAcceptRewardItem.empty() )
		return NetError::NET_OK;

	CX2OfflineInventory* pInven = CX2OfflineInventory::Instance();

	if( false == pInven->IsLoaded() )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d refused: the bag is not loaded, cannot pay its %u accept item(s)",
			pTemplet->m_iID, (unsigned int)pTemplet->m_mapAcceptRewardItem.size() );
		return NetError::ERR_QUEST_25;
	}

	std::map< int, int >::const_iterator mit;

	// UserQuestManager.cpp:698-710: an accept item the character already holds, bag or bank, refuses the quest ( ERR_QUEST_24 ).
	// Q4b review: IsExistInventoryItemWithBank is KInventory::IsExist( id, false ) ( Inventory.cpp:674 ), every row of any
	// category. CountItemByID skips ST_BANK, and the emulator's private bank holds rows, so every row is scanned instead.
	std::map< UidType, KInventoryItemInfo > mapHeld;
	pInven->GetAllItems( mapHeld );

	std::set< int > setHeldID;
	{
		std::map< UidType, KInventoryItemInfo >::const_iterator hit;
		for( hit = mapHeld.begin(); hit != mapHeld.end(); ++hit )
			setHeldID.insert( hit->second.m_kItemInfo.m_iItemID );
	}

	for( mit = pTemplet->m_mapAcceptRewardItem.begin(); mit != pTemplet->m_mapAcceptRewardItem.end(); ++mit )
	{
		if( setHeldID.end() != setHeldID.find( mit->first ) )
		{
			CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d refused: already holds accept item %d (ERR_QUEST_24)",
				pTemplet->m_iID, mit->first );
			return NetError::ERR_QUEST_24;
		}
	}

	// :713-722 IsEnoughSpaceExist. HasRoomFor answers per item ( optimistic when two items would share the last free slot ); an
	// item the client has no templet for is skipped below as the server skips it ( :729-736 ), so it is not asked about here.
	for( mit = pTemplet->m_mapAcceptRewardItem.begin(); mit != pTemplet->m_mapAcceptRewardItem.end(); ++mit )
	{
		if( NULL == CX2OfflineInventory::Templet( mit->first ) )
			continue;

		if( false == pInven->HasRoomFor( mit->first, mit->second ) )
		{
			CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d refused: no room for accept item %d x %d (ERR_QUEST_25)",
				pTemplet->m_iID, mit->first, mit->second );
			return NetError::ERR_QUEST_25;
		}
	}

	// :724-760 PrepareInsert. A fresh vector per item, appended: InsertItem clears the one it is handed.
	for( mit = pTemplet->m_mapAcceptRewardItem.begin(); mit != pTemplet->m_mapAcceptRewardItem.end(); ++mit )
	{
		if( NULL == CX2OfflineInventory::Templet( mit->first ) )
		{
			CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d: accept item %d has no templet, skipped (as the server does)",
				pTemplet->m_iID, mit->first );
			continue;
		}

		std::vector< KInventoryItemInfo > vecChanged;
		int iInserted = 0;

		pInven->InsertItem( mit->first, mit->second, 0, vecChanged, iInserted );

		for( size_t i = 0; i < vecChanged.size(); ++i )
			vecSlots.push_back( vecChanged[i] );

		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF accept %d paid accept item %d: %d of %d fitted",
			pTemplet->m_iID, mit->first, iInserted, mit->second );
	}

	return NetError::NET_OK;
}

void CX2OfflineQuest::TakeAcceptSlots( int iQuestID, OUT std::vector< KInventoryItemInfo >& vecOut )
{
	vecOut.clear();

	std::map< int, std::vector< KInventoryItemInfo > >::iterator mit = m_mapAcceptSlots.find( iQuestID );
	if( mit == m_mapAcceptSlots.end() )
		return;

	vecOut.swap( mit->second );
	m_mapAcceptSlots.erase( mit );
}

void CX2OfflineQuest::RemoveAcceptItems( int iQuestID, OUT std::vector< KInventoryItemInfo >& vecOut )
{
	vecOut.clear();

	const CX2QuestManager::QuestTemplet* pTemplet = Templet( iQuestID );
	if( NULL == pTemplet || true == pTemplet->m_mapAcceptRewardItem.empty() )
		return;

	CX2OfflineInventory* pInven = CX2OfflineInventory::Instance();
	if( false == pInven->IsLoaded() )
		return;

	// DeleteItemAll: every copy of the item, worn ones too. A snapshot of the uids first, DeleteItem edits the inventory.
	std::map< UidType, KInventoryItemInfo > mapAll;
	pInven->GetAllItems( mapAll );

	std::vector< UidType > vecUID;
	std::map< UidType, KInventoryItemInfo >::const_iterator ait;
	for( ait = mapAll.begin(); ait != mapAll.end(); ++ait )
	{
		if( pTemplet->m_mapAcceptRewardItem.end() != pTemplet->m_mapAcceptRewardItem.find( ait->second.m_kItemInfo.m_iItemID ) )
			vecUID.push_back( ait->first );
	}

	for( size_t i = 0; i < vecUID.size(); ++i )
	{
		KInventoryItemInfo kOut;
		if( true == pInven->DeleteItem( vecUID[i], 0, kOut ) )
			vecOut.push_back( kOut );
	}

	if( false == vecUID.empty() )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] OFF give up %d: removed %u row(s) of its accept items",
			iQuestID, (unsigned int)vecOut.size() );
	}
}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

//////////////////////////////////////////////////////////////////////////
// events

void CX2OfflineQuest::OnTalkNPC( int iNPCID, int iVillageMapID, const KOfflineUnitRow& kRow,
								 OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_NPC_TALK != pSub->m_eClearType )
				continue;

			if( false == pSub->m_ClearCondition.m_setVillageMapID.empty() )
			{
				if( pSub->m_ClearCondition.m_setVillageMapID.end() ==
					pSub->m_ClearCondition.m_setVillageMapID.find( iVillageMapID ) )
				{
					continue;
				}
			}

			if( (int)pSub->m_ClearCondition.m_eTalkNPCID != iNPCID )
				continue;

			if( 0 != kInst.m_vecSubQuestInstance[i].m_ucClearData )
				continue;

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server( L"QUEST    %d sub %d NPC_TALK done (NPC %d)",
				mit->first, pTemplet->m_vecSubQuest[i], iNPCID );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnNpcDie( int iDungeonID, char cDifficulty, int iBattleFieldID, int iNpcID,
								const KOfflineUnitRow& kRow,
								OUT std::vector< KQuestInstance >& vecChanged )
{
	const bool bBattleField = ( 0 != iBattleFieldID );

	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_NPC_HUNT != pSub->m_eClearType )
				continue;

			if( pSub->m_ClearCondition.m_setKillNPCID.end() ==
				pSub->m_ClearCondition.m_setKillNPCID.find(
					(CX2UnitManager::NPC_UNIT_ID)iNpcID ) )
			{
				continue;
			}

			// The place filters, and only when the sub-quest sets one - an empty
			// set means "anywhere".
			if( true == bBattleField )
			{
				if( false == pSub->m_ClearCondition.m_setBattleFieldID.empty() &&
					pSub->m_ClearCondition.m_setBattleFieldID.end() ==
						pSub->m_ClearCondition.m_setBattleFieldID.find( iBattleFieldID ) )
				{
					continue;
				}
			}
			else
			{
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				if( true == HasDungeonFilter( pSub ) &&
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				if( false == pSub->m_ClearCondition.m_setDungeonID.empty() &&
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
					false == IsExistDungeonInSub( pSub, iDungeonID + (int)cDifficulty ) )
				{
					continue;
				}
			}

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			if( pSub->m_ClearCondition.m_iKillNum <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				continue;
			}

			++kInst.m_vecSubQuestInstance[i].m_ucClearData;

			if( pSub->m_ClearCondition.m_iKillNum <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
			}

			CX2OfflineLog::Server( L"QUEST    %d sub %d NPC_HUNT %d/%d (NPC %d)",
				mit->first, pTemplet->m_vecSubQuest[i],
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				pSub->m_ClearCondition.m_iKillNum, iNpcID );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
void CX2OfflineQuest::OnDungeonClear( int iDungeonID, char cDifficulty, char cRank, int iPlayTime,
									  int iDamage, bool bSuitableLevel, bool bHaveExp, const KOfflineUnitRow& kRow,
									  OUT std::vector< KQuestInstance >& vecChanged )
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
void CX2OfflineQuest::OnDungeonClear( int iDungeonID, char cDifficulty, char cRank, int iPlayTime,
									  int iDamage, const KOfflineUnitRow& kRow,
									  OUT std::vector< KQuestInstance >& vecChanged )
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			// The dungeon filter first, for every type - a sub-quest that names
			// dungeons only counts the ones it names.
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			if( true == HasDungeonFilter( pSub ) &&
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			if( false == pSub->m_ClearCondition.m_setDungeonID.empty() &&
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				false == IsExistDungeonInSub( pSub, iDungeonID + (int)cDifficulty ) )
			{
				continue;
			}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
			// Q4b ( user 2026-10-19 "follow June's" ): m_eExceptionDungeonID, a server-only key of 6 sub-quests ( types 8 / 32 ). A
			// listed dungeon never counts. Entries are dungeon id + difficulty, the same packed number the filter above compares.
			if( true == IsExceptionDungeon( pSub, iDungeonID + (int)cDifficulty ) )
			{
				CX2OfflineLog::Server( L"[JUNE_QUEST] OFF %d sub %d not counted: dungeon %d+%d is in its m_eExceptionDungeonID",
					mit->first, iSubQuestID, iDungeonID, (int)cDifficulty );
				continue;
			}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN

			bool bHit = false;

			switch( pSub->m_eClearType )
			{
			case CX2QuestManager::SQT_DUNGEON_TIME:
				bHit = ( pSub->m_ClearCondition.m_iDungeonClearTime >= iPlayTime );
				break;

			case CX2QuestManager::SQT_DUNGEON_RANK:
				bHit = ( (char)pSub->m_ClearCondition.m_eDungeonRank <= cRank );
				break;

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
				// UserQuestManager.cpp:2076-2097. An event / PC-bang quest counts outside tutorial, Henir, training camp and
				// Ruben, and a step that names no dungeon also needs the run to have paid EXP ( ExistHaveExpInDungeon ); any
				// other quest counts anywhere but the El Forest Gate and the tutorials.
				if( CX2QuestManager::QT_EVENT == pTemplet->m_eQuestType ||
					CX2QuestManager::QT_PCBANG == pTemplet->m_eQuestType )
				{
					bHit = ( true == IsQuestCountedDungeon( iDungeonID ) &&
							 ( true == HasDungeonFilter( pSub ) || true == bHaveExp ) );
				}
				else
				{
					// :2231-2232, IsTutorialDungeon's whole list ( Q4 review ).
					bHit = ( (int)SEnum::DI_EL_FOREST_GATE_NORMAL != iDungeonID &&
							 false == IsQuestTutorialDungeon( iDungeonID ) );
				}
				break;

			case CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR:
				// UserQuestManager.cpp:2375-2420: a counted dungeon, cleared by a unit inside the dungeon's level band.
				bHit = ( true == IsQuestCountedDungeon( iDungeonID ) && true == bSuitableLevel );
				break;

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
			case CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR:
				// Stand-in ( June's server only; user 2026-10-19 "follow June's" ): a clear counts when the run paid EXP.
				bHit = bHaveExp;
				break;
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN
#ifdef SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
			case CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR:
				// Q4 review stand-in ( June's server only; user 2026-10-19 ): the run also meets the quest's other dungeon steps.
				bHit = IsTogetherRunMet( pTemplet, iSubQuestID, iDungeonID, cDifficulty, cRank, iPlayTime, iDamage,
										 bSuitableLevel, bHaveExp );
				break;
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
				bHit = true;
				break;
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

			// A damage target, met by the total this character dealt over the
			// run. The comparison really is this way round on the server
			// (UserQuestManager.cpp:1919): m_iDungeonDamage is a CEILING the
			// run must stay under, not a floor - these are "clear it without
			// hitting anything harder than" steps.
			case CX2QuestManager::SQT_DUNGEON_DAMAGE:
				bHit = ( pSub->m_ClearCondition.m_iDungeonDamage >= iDamage );
				break;

			default:
				break;
			}

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			if( false == bHit &&
				( CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR == pSub->m_eClearType ||
				  CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR == pSub->m_eClearType ) )
			{
				CX2OfflineLog::Server( L"[JUNE_QUEST] OFF %d sub %d type %d not counted: dungeon %d counted=%d suitable=%d haveExp=%d",
					mit->first, iSubQuestID, (int)pSub->m_eClearType, iDungeonID,
					IsQuestCountedDungeon( iDungeonID ) ? 1 : 0, bSuitableLevel ? 1 : 0, bHaveExp ? 1 : 0 );
			}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE

			if( false == bHit )
				continue;

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			// The counted types keep a running total; everything else is a
			// one-shot flag. UserQuestManager.cpp:2268.
			if( CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT == pSub->m_eClearType ||
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				// UserQuestManager.cpp:2519-2539 counts suitable level the same way; June's 48 is case 8's body ( 32323ad0 ).
				CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR == pSub->m_eClearType ||
				CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR == pSub->m_eClearType ||
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
#ifdef SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
				// June's client shows 44 with case 8's cur / need body ( 32337db0 case 0x2c ): a running total of m_iDungeonClearCount.
				CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR == pSub->m_eClearType ||
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
				CX2QuestManager::SQT_DUNGEON_RANK == pSub->m_eClearType )
			{
				if( pSub->m_ClearCondition.m_iDungeonClearCount >
					(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
				{
					++kInst.m_vecSubQuestInstance[i].m_ucClearData;
				}

				if( pSub->m_ClearCondition.m_iDungeonClearCount <=
					(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
				{
					kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
				}
			}
			else
			{
				if( 0 != kInst.m_vecSubQuestInstance[i].m_ucClearData )
					continue;

				kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;
			}

			CX2OfflineLog::Server( L"QUEST    %d sub %d dungeon clear type %d -> %d, done=%d",
				mit->first, iSubQuestID, (int)pSub->m_eClearType,
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );

#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			if( CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR == pSub->m_eClearType ||
				CX2QuestManager::SQT_HAS_EXP_DUNGEON_CLEAR == pSub->m_eClearType )
			{
				CX2OfflineLog::Server( L"[JUNE_QUEST] OFF %d sub %d type %d counted %d / %d, done=%d",
					mit->first, iSubQuestID, (int)pSub->m_eClearType,
					(int)kInst.m_vecSubQuestInstance[i].m_ucClearData, pSub->m_ClearCondition.m_iDungeonClearCount,
					kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );
			}
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
#ifdef SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER
			if( CX2QuestManager::SQT_TOGETHER_SUBQUEST_CLEAR == pSub->m_eClearType )
			{
				CX2OfflineLog::Server( L"[JUNE_QUEST] OFF together %d ( quest %d ) counted %d / %d, done=%d",
					iSubQuestID, mit->first,
					(int)kInst.m_vecSubQuestInstance[i].m_ucClearData, pSub->m_ClearCondition.m_iDungeonClearCount,
					kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );
			}
#endif SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// The three "you went somewhere" types are near-identical loops kept apart on
// purpose: each has a different place test on a different member of
// ClearCondition, and folding them into one body behind a flag made the one
// thing worth reading - which set is consulted - the least visible line in it.

void CX2OfflineQuest::OnVisitVillage( int iMapID, const KOfflineUnitRow& kRow,
									  OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_VISIT_VILLAGE != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( pSub->m_ClearCondition.m_setVillageMapID.end() ==
				pSub->m_ClearCondition.m_setVillageMapID.find( iMapID ) )
			{
				continue;
			}

			if( 0 != kInst.m_vecSubQuestInstance[i].m_ucClearData )
				continue;

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server( L"QUEST    %d sub %d VISIT_VILLAGE done (map %d)",
				mit->first, pTemplet->m_vecSubQuest[i], iMapID );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnVisitField( int iBattleFieldID, const KOfflineUnitRow& kRow,
									OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_VISIT_FIELD != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( pSub->m_ClearCondition.m_setBattleFieldID.end() ==
				pSub->m_ClearCondition.m_setBattleFieldID.find( iBattleFieldID ) )
			{
				continue;
			}

			if( 0 != kInst.m_vecSubQuestInstance[i].m_ucClearData )
				continue;

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server( L"QUEST    %d sub %d VISIT_FIELD done (field %d)",
				mit->first, pTemplet->m_vecSubQuest[i], iBattleFieldID );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnVisitDungeon( int iDungeonID, char cDifficulty, const KOfflineUnitRow& kRow,
									  OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_VISIT_DUNGEON != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( false == IsExistDungeonInSub( pSub, iDungeonID + (int)cDifficulty ) )
			{
				// The rejection used to be a bare continue, which made a
				// step that never ticks indistinguishable from a step that
				// was never looked at. Both halves of the comparison, so the
				// next report of this class is one play-test rather than three.
				CX2OfflineLog::Server(
					L"QUEST    VISIT quest %d sub %d wants dungeon %s, got %d - no match",
					mit->first, pSub->m_iID, DungeonReqString( pSub ).c_str(),
					iDungeonID + (int)cDifficulty );
				continue;
			}

			if( 0 != kInst.m_vecSubQuestInstance[i].m_ucClearData )
				continue;

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server( L"QUEST    %d sub %d VISIT_DUNGEON done (dungeon %d+%d)",
				mit->first, pTemplet->m_vecSubQuest[i], iDungeonID, (int)cDifficulty );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnFindNPC( const std::vector< int >& vecNpcID, bool bDungeon,
								 int iDungeonID, char cDifficulty, int iBattleFieldID,
								 const KOfflineUnitRow& kRow,
								 OUT std::vector< KQuestInstance >& vecChanged )
{
	if( true == vecNpcID.empty() )
		return;

	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_FIND_NPC != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( true == bDungeon )
			{
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				if( true == HasDungeonFilter( pSub ) &&
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				if( false == pSub->m_ClearCondition.m_setDungeonID.empty() &&
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
					false == IsExistDungeonInSub( pSub, iDungeonID + (int)cDifficulty ) )
				{
					CX2OfflineLog::Server(
						L"QUEST    FINDNPC quest %d sub %d wants dungeon %s, got %d - no match",
						mit->first, pSub->m_iID, DungeonReqString( pSub ).c_str(),
						iDungeonID + (int)cDifficulty );
					continue;
				}
			}
			else
			{
				if( false == pSub->m_ClearCondition.m_setBattleFieldID.empty() &&
					pSub->m_ClearCondition.m_setBattleFieldID.end() ==
						pSub->m_ClearCondition.m_setBattleFieldID.find( iBattleFieldID ) )
				{
					continue;
				}
			}

			bool bFound = false;
			for( size_t n = 0; n < vecNpcID.size(); ++n )
			{
				if( vecNpcID[n] == (int)pSub->m_ClearCondition.m_iFindNPCID )
				{
					bFound = true;
					break;
				}
			}

			if( false == bFound )
				continue;

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server( L"QUEST    %d sub %d FIND_NPC done (NPC %d)",
				mit->first, pTemplet->m_vecSubQuest[i],
				(int)pSub->m_ClearCondition.m_iFindNPCID );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnLevelUp( const KOfflineUnitRow& kRow,
								 OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_CHAR_LEVEL_UP != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( kRow.m_iLevel < pSub->m_ClearCondition.m_iCharLevel )
				continue;

			if( false == CheckBeforeGroup( pTemplet, pTemplet->m_vecSubQuest[i] ) )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server( L"QUEST    %d sub %d CHAR_LEVEL_UP done (level %d >= %d)",
				mit->first, pTemplet->m_vecSubQuest[i], kRow.m_iLevel,
				pSub->m_ClearCondition.m_iCharLevel );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// The "you used a feature" half.
//
// Every one of these is a game action that happens somewhere other than a
// dungeon room: spending a skill point, learning a skill, firing one, drinking
// a potion, handing an item to the blacksmith. The real server ticks them from
// the packet handler for that feature, not from anything quest-shaped, which is
// why they were the group left unimplemented and why all of them sat silently
// stuck rather than visibly broken.
//
// They share a shape - walk the quests, walk the sub-quests, match the type,
// apply the filter, bump the counter, flag success at the target - and they are
// still written out one at a time, because the *filter* is the entire content
// of each and folding them together would hide the only part worth reading.

void CX2OfflineQuest::OnUseSkillPoint( int iSkillID, const KOfflineUnitRow& kRow,
									   OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_USE_SKILL_POINT != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			// m_iSkillID 0 means "any skill", which is what a "spend a skill
			// point" tutorial step uses. KUserQuestManager::
			// Handler_OnUseSkillPoint, UserQuestManager.cpp:3510.
			if( 0 != pSub->m_ClearCondition.m_iSkillID &&
				iSkillID != pSub->m_ClearCondition.m_iSkillID )
			{
				continue;
			}

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			if( pSub->m_ClearCondition.m_iSpUseCount >
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				++kInst.m_vecSubQuestInstance[i].m_ucClearData;
			}

			if( pSub->m_ClearCondition.m_iSpUseCount <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
			}

			CX2OfflineLog::Server(
				L"QUEST    %d sub %d USE_SKILL_POINT skill %d -> %d/%d, done=%d",
				mit->first, iSubQuestID, iSkillID,
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				pSub->m_ClearCondition.m_iSpUseCount,
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnLearnNewSkill( const std::vector< int >& vecSkillID,
									   const KOfflineUnitRow& kRow,
									   OUT std::vector< KQuestInstance >& vecChanged )
{
	if( true == vecSkillID.empty() )
		return;

	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_LEARN_NEW_SKILL != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			// Unlike USE_SKILL_POINT, an empty skill set here means the
			// sub-quest can never complete rather than "any skill" - the server
			// bails on it explicitly (Handler_OnLearnNewSkill:7717).
			if( true == pSub->m_ClearCondition.m_setSkillID.empty() )
				continue;

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			bool bAny = false;

			for( size_t s = 0; s < vecSkillID.size(); ++s )
			{
				if( pSub->m_ClearCondition.m_setSkillID.end() ==
					pSub->m_ClearCondition.m_setSkillID.find( vecSkillID[s] ) )
				{
					continue;
				}

				bAny = true;

				++kInst.m_vecSubQuestInstance[i].m_ucClearData;

				if( pSub->m_ClearCondition.m_iLearnNewSkillCount <=
					(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
				{
					kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
					break;
				}
			}

			if( false == bAny )
				continue;

			CX2OfflineLog::Server(
				L"QUEST    %d sub %d LEARN_NEW_SKILL -> %d/%d, done=%d",
				mit->first, iSubQuestID,
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				pSub->m_ClearCondition.m_iLearnNewSkillCount,
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnUseSkill( int iSkillID, const KOfflineUnitRow& kRow,
								  OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_SKILL_USE != pSub->m_eClearType )
				continue;

			// An empty skill set here DOES mean "any skill" - the server tests
			// empty-or-found in a single condition
			// (Handler_EGS_SKILL_USE_REQ:7595). That is the opposite of
			// LEARN_NEW_SKILL above, in the same file, a hundred lines apart,
			// so neither one is safe to infer from the other.
			if( false == pSub->m_ClearCondition.m_setSkillID.empty() &&
				pSub->m_ClearCondition.m_setSkillID.end() ==
					pSub->m_ClearCondition.m_setSkillID.find( iSkillID ) )
			{
				continue;
			}

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			// The server's handler applies no dungeon or field filter of its
			// own; the caller has already refused the dungeons the feature is
			// switched off in.
			if( pSub->m_ClearCondition.m_iUseSkillCount <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				continue;
			}

			++kInst.m_vecSubQuestInstance[i].m_ucClearData;

			if( pSub->m_ClearCondition.m_iUseSkillCount <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
			}

			CX2OfflineLog::Server(
				L"QUEST    %d sub %d SKILL_USE skill %d -> %d/%d, done=%d",
				mit->first, iSubQuestID, iSkillID,
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				pSub->m_ClearCondition.m_iUseSkillCount,
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnUseItem( int iItemID, int iDungeonID, char cDifficulty,
								 int iBattleFieldID, const KOfflineUnitRow& kRow,
								 OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			if( CX2QuestManager::SQT_ITEM_USE != pSub->m_eClearType )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			// The dungeon filter wins over the field filter, and they are
			// checked in that order with an else - a sub-quest that names
			// dungeons is never also field-filtered.
			// KUserQuestManager::CountUseItem, UserQuestManager.cpp:6496.
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			if( true == HasDungeonFilter( pSub ) )
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			if( false == pSub->m_ClearCondition.m_setDungeonID.empty() )
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
			{
				if( false == IsExistDungeonInSub( pSub, iDungeonID + (int)cDifficulty ) )
				{
					CX2OfflineLog::Server(
						L"QUEST    USEITEM quest %d sub %d wants dungeon %s, got %d - no match",
						mit->first, pSub->m_iID, DungeonReqString( pSub ).c_str(),
						iDungeonID + (int)cDifficulty );
					continue;
				}
			}
			else if( false == pSub->m_ClearCondition.m_setBattleFieldID.empty() )
			{
				if( pSub->m_ClearCondition.m_setBattleFieldID.end() ==
					pSub->m_ClearCondition.m_setBattleFieldID.find( iBattleFieldID ) )
				{
					continue;
				}
			}

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			bool bNamed = false;

			for( size_t u = 0; u < pSub->m_ClearCondition.m_vecUseItemID.size(); ++u )
			{
				if( pSub->m_ClearCondition.m_vecUseItemID[u] != iItemID )
					continue;

				bNamed = true;

				if( pSub->m_ClearCondition.m_iUseItemNum >
					(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
				{
					++kInst.m_vecSubQuestInstance[i].m_ucClearData;
				}
			}

			if( false == bNamed )
				continue;

			if( pSub->m_ClearCondition.m_iUseItemNum <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
			}

			CX2OfflineLog::Server(
				L"QUEST    %d sub %d ITEM_USE item %d -> %d/%d, done=%d",
				mit->first, iSubQuestID, iItemID,
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				pSub->m_ClearCondition.m_iUseItemNum,
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnEnchantItem( int iItemID, int iEnchantLevel, const KOfflineUnitRow& kRow,
									 OUT std::vector< KQuestInstance >& vecChanged )
{
	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( CX2QuestManager::SQT_ITEM_ENCHANT != pSub->m_eClearType )
				continue;

			// The target item. The server calls this field m_iItemID; the
			// client parses the same Lua key (m_iCollectionItemID) into the
			// field of that name, so this is a rename, not a different rule -
			// X2QuestManager.cpp:2292 is the read, and :3832 is the client's
			// own "0 means any item" in the quest-description text.
			//
			// Absent is treated as "any item", which is what the title steps
			// this unblocks want: they ask for a level, not for a particular
			// weapon. Both 0 and -1 count as absent - the parser writes 0 for
			// this clear type and ClearCondition's constructor leaves -1 -
			// because a step that named no item must not become unreachable on
			// the strength of which of the two got there first.
			const int iWantedItemID = pSub->m_ClearCondition.m_iCollectionItemID;

			if( iWantedItemID > 0 && iWantedItemID != iItemID )
				continue;

			// Exact, not at-least. KUserQuestManager::Handler_OnEnchantItem
			// (UserQuestManager.cpp:2943) compares with !=, so a player who
			// goes straight past the level the step wants has to come back
			// down to it - which is a thing enhancement can actually do.
			if( pSub->m_ClearCondition.m_iEnchantLevel != iEnchantLevel )
				continue;

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			// One-shot: this type has no counter, only a flag.
			if( 0 != kInst.m_vecSubQuestInstance[i].m_ucClearData )
				continue;

			kInst.m_vecSubQuestInstance[i].m_ucClearData	= 1;
			kInst.m_vecSubQuestInstance[i].m_bIsSuccess		= true;

			CX2OfflineLog::Server(
				L"QUEST    %d sub %d ITEM_ENCHANT item %d reached +%d, done=1",
				mit->first, iSubQuestID, iItemID, iEnchantLevel );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

void CX2OfflineQuest::OnSocketItem( int iItemID, int iSocketUseCount, const KOfflineUnitRow& kRow,
									OUT std::vector< KQuestInstance >& vecChanged )
{
	if( iSocketUseCount <= 0 )
		return;

	std::map< int, KQuestInstance >::iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		KQuestInstance& kInst = mit->second;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size() &&
							i < kInst.m_vecSubQuestInstance.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			if( true == kInst.m_vecSubQuestInstance[i].m_bIsSuccess )
				continue;

			if( CX2QuestManager::SQT_ITEM_SOCKET != pSub->m_eClearType )
				continue;

			// Same rename as the enchant hook: the server's m_iItemID is the
			// client's m_iCollectionItemID, parsed from the same Lua key
			// (X2QuestManager.cpp:2299), and absent means "any item".
			const int iWantedItemID = pSub->m_ClearCondition.m_iCollectionItemID;

			if( iWantedItemID > 0 && iWantedItemID != iItemID )
				continue;

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			// Add this request's slots, then clamp to what the step asks for -
			// the counter is a UCHAR and a big multi-slot request would
			// otherwise be able to run past the target and wrap.
			if( pSub->m_ClearCondition.m_iSocketCount >
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				const int iNew = (int)kInst.m_vecSubQuestInstance[i].m_ucClearData + iSocketUseCount;

				kInst.m_vecSubQuestInstance[i].m_ucClearData =
					(unsigned char)( ( iNew > pSub->m_ClearCondition.m_iSocketCount )
									 ? pSub->m_ClearCondition.m_iSocketCount : iNew );
			}

			if( pSub->m_ClearCondition.m_iSocketCount <=
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData )
			{
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess = true;
			}

			CX2OfflineLog::Server(
				L"QUEST    %d sub %d ITEM_SOCKET item %d -> %d/%d, done=%d",
				mit->first, iSubQuestID, iItemID,
				(int)kInst.m_vecSubQuestInstance[i].m_ucClearData,
				pSub->m_ClearCondition.m_iSocketCount,
				kInst.m_vecSubQuestInstance[i].m_bIsSuccess ? 1 : 0 );

			Save( mit->first );
			MarkChanged( kInst, vecChanged );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CX2OfflineQuest::GetQuestItemDrops( int iDungeonID, char cDifficulty, int iBattleFieldID,
										 int iNpcID, const KOfflineUnitRow& kRow,
										 OUT std::vector< int >& vecItemID )
{
	vecItemID.clear();

	const bool bBattleField = ( 0 != iBattleFieldID );

	CX2OfflineInventory* pInven = CX2OfflineInventory::Instance();

	std::map< int, KQuestInstance >::const_iterator mit;
	for( mit = m_mapQuesting.begin(); mit != m_mapQuesting.end(); ++mit )
	{
		const CX2QuestManager::QuestTemplet* pTemplet = Templet( mit->first );
		if( false == IsQuestAdvanceable( pTemplet, kRow.m_iLevel ) )
			continue;

		for( size_t i = 0; i < pTemplet->m_vecSubQuest.size(); ++i )
		{
			const int iSubQuestID = pTemplet->m_vecSubQuest[i];

			const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( iSubQuestID );
			if( NULL == pSub )
				continue;

			// Only the quest-item type drops. SQT_ITEM_COLLECTION asks for an
			// ordinary item the player is meant to find elsewhere.
			if( CX2QuestManager::SQT_QUEST_ITEM_COLLECTION != pSub->m_eClearType )
				continue;

			if( pSub->m_ClearCondition.m_iCollectionItemID <= 0 )
				continue;

			if( false == pSub->m_ClearCondition.m_setKillNPCID.empty() &&
				pSub->m_ClearCondition.m_setKillNPCID.end() ==
					pSub->m_ClearCondition.m_setKillNPCID.find(
						(CX2UnitManager::NPC_UNIT_ID)iNpcID ) )
			{
				continue;
			}

			if( true == bBattleField )
			{
				if( false == pSub->m_ClearCondition.m_setBattleFieldID.empty() &&
					pSub->m_ClearCondition.m_setBattleFieldID.end() ==
						pSub->m_ClearCondition.m_setBattleFieldID.find( iBattleFieldID ) )
				{
					continue;
				}
			}
			else
			{
#ifdef SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				if( true == HasDungeonFilter( pSub ) &&
#else SERV_IRUHADEV_JUNE_QUEST_OFFLINE
				if( false == pSub->m_ClearCondition.m_setDungeonID.empty() &&
#endif SERV_IRUHADEV_JUNE_QUEST_OFFLINE
					false == IsExistDungeonInSub( pSub, iDungeonID + (int)cDifficulty ) )
				{
					continue;
				}
			}

			if( false == CheckBeforeGroup( pTemplet, iSubQuestID ) )
				continue;

			// Enough already? The bag is the counter for this type, so the
			// stopping condition is the same one that marks the step done.
			const int iHave = pInven->CountItemByID(
				pSub->m_ClearCondition.m_iCollectionItemID, true );

			if( iHave >= pSub->m_ClearCondition.m_iCollectionItemNum )
				continue;

			// KRoomUser::GetQuestDropItemInDungeon:1056 - a 1..10000 roll scaled
			// to hundredths of a percent, against the sub-quest's own rate. The
			// four live multipliers (comeback user, Gaia server, two events) are
			// all 1.0 here, the same simplification phase 4 and 5 made for EXP,
			// ED and the ordinary drop table.
			float fRoll = (float)( ::rand() % 10000 ) + 1.0f;
			fRoll /= 100.0f;

			//{{ Iruha : 2026-09-06 // offline QoL: 3x quest collection item rate
			// Unlike the ordinary drop table this IS a true per-item roll, so here
			// the rate really is a multiplier rather than a repeated draw - and the
			// clamp is meaningful, because a single roll cannot exceed certainty.
			// Overshooting the requirement is impossible: the iHave check above has
			// already skipped this sub-quest once the bag holds enough.
			float fRate = pSub->m_ClearCondition.m_fQuestItemDropRate;

#ifdef SERV_IRUHADEV_OFFLINE_DROP_BOOST
			fRate *= SERV_IRUHADEV_OFFLINE_QUEST_ITEM_RATE;

			if( fRate > 100.0f )
				fRate = 100.0f;
#endif SERV_IRUHADEV_OFFLINE_DROP_BOOST
			//}} Iruha : 2026-09-06

			if( fRoll > fRate )
				continue;

			vecItemID.push_back( pSub->m_ClearCondition.m_iCollectionItemID );

			CX2OfflineLog::Server( L"QUEST    %d sub %d quest item %d drops (%d/%d held, rate %.2f)",
				mit->first, iSubQuestID, pSub->m_ClearCondition.m_iCollectionItemID,
				iHave, pSub->m_ClearCondition.m_iCollectionItemNum,
				fRate );
		}
	}
}

/*static*/ void CX2OfflineQuest::GetConditionItems( int iQuestID, OUT std::map< int, int >& mapOut )
{
	mapOut.clear();

	const CX2QuestManager::QuestTemplet* pTemplet = Templet( iQuestID );
	if( NULL == pTemplet )
		return;

	for( size_t i = 0; i < pTemplet->m_vecSubQuest.size(); ++i )
	{
		const CX2QuestManager::SubQuestTemplet* pSub = SubTemplet( pTemplet->m_vecSubQuest[i] );
		if( NULL == pSub )
			continue;

		if( CX2QuestManager::SQT_ITEM_COLLECTION != pSub->m_eClearType &&
			CX2QuestManager::SQT_QUEST_ITEM_COLLECTION != pSub->m_eClearType )
		{
			continue;
		}

		if( pSub->m_ClearCondition.m_iCollectionItemID <= 0 )
			continue;

		mapOut[ pSub->m_ClearCondition.m_iCollectionItemID ] =
			pSub->m_ClearCondition.m_iCollectionItemNum;
	}
}

/*static*/ int CX2OfflineQuest::GetChangeUnitClass( int iQuestID )
{
	const CX2QuestManager::QuestTemplet* pTemplet = Templet( iQuestID );
	if( NULL == pTemplet )
		return (int)CX2Unit::UC_NONE;

	if( CX2QuestManager::QT_CHANGE_JOB != pTemplet->m_eQuestType )
		return (int)CX2Unit::UC_NONE;

	return (int)pTemplet->m_Reward.m_eChangeUnitClass;
}

#endif SERV_IRUHADEV_OFFLINE
