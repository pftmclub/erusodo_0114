#include "stdafx.h"
#include "X2OfflineServer.h"

//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-09-02
// Description: Offline mode - phase 5. The skill packets: learning and
//              upgrading, resetting one skill, resetting the whole tree, and
//              assigning skills to the eight equipped slots.
//
//              The rules live in CX2OfflineSkill; this is the packet layer.
//              References are KncWX2Server/GameServer/GSUserGameCommon.cpp
//              (:4466 learn, :4682 reset, :4950 init, :2747 slot) and the
//              client's own X2Lib/X2SkillTree.cpp handlers.
//
//              One decision worth naming, because it is a gameplay change and
//              not an emulation detail: EGS_RESET_SKILL_REQ consumes a skill
//              reset scroll if the character has one and goes ahead without one
//              if it does not. The real server refuses outright
//              (ERR_RESET_SKILL_01). Offline there is currently no way to
//              obtain that scroll - it is a cash-shop item, and the cash shop
//              is phase 7 - so requiring it would make a feature this phase is
//              supposed to deliver impossible to use or test. The log says
//              which of the two happened every time.
//////////////////////////////////////////////////////////////////////////

#ifdef SERV_IRUHADEV_OFFLINE

#include "X2OfflineInventory.h"
#include "X2OfflineSkill.h"
#include "X2OfflineQuest.h"

//////////////////////////////////////////////////////////////////////////

bool CX2OfflineServer::Handler_EGS_GET_SKILL_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_GET_SKILL_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	CX2OfflineDB*		pDB		= CX2OfflineDB::Instance();
	CX2OfflineSkill*	pSkill	= CX2OfflineSkill::Instance();

	KEGS_GET_SKILL_ACK kAck;
	kAck.m_iOK			= NetError::ERR_UNKNOWN;
	kAck.m_iRemainSP	= 0;
	kAck.m_iRemainCSP	= 0;

	// Echoed verbatim, matching GSUserGameCommon.cpp:4534 - the real server never
	// derives this, it just hands the request's page number back. The client's
	// ACK handler (X2SkillTree.cpp:1962) turns it straight into
	// CX2UserSkillTree::SetUsingPage(), which subtracts 1 with no bounds check;
	// leaving this field at its default-constructed 0 underflows m_usUsingPage
	// (a USHORT) to 65535, and every later read or write that indexes off of it -
	// AccessMapSkillAcquired(), GetSkillPoint(), SetSkillLevelAndCSP()'s own page
	// array - either silently no-ops (the skill tree reads as fully unlearned)
	// or walks off a fixed-size array (the crash the field's absence was causing).
	kAck.m_iActiveSkillPageNumber = kReq.m_iActiveSkillPageNumber;
#ifdef SERV_IRUHADEV_JUNE_SKILL_REFORM
	// June skill system S3 (plan rule 4): the client's SetUsingPage subtracts 1 unchecked, so an ACK never carries page 0.
	if( kAck.m_iActiveSkillPageNumber < 1 )
		kAck.m_iActiveSkillPageNumber = 1;
#endif SERV_IRUHADEV_JUNE_SKILL_REFORM

	KOfflineUnitRow kRow;
	if( false == pDB->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) )
		return Reply( kSes, EGS_GET_SKILL_ACK, kAck );

	kAck.m_iRemainSP = kRow.m_iSP;

	pSkill->Load( kRow.m_nUnitUID );

	// The request is the client's desired end state, skill by skill. It comes
	// back echoed with the before-levels filled in, which is what the client's
	// ACK handler writes into its own tree.
	kAck.m_mapSkillList = kReq.m_mapSkillList;

	int iSPLeft = kRow.m_iSP;

	kAck.m_iOK = pSkill->LearnSkills( kRow.m_iLevel, kRow.m_iUnitClass,
									  kAck.m_mapSkillList, kRow.m_iSP, iSPLeft );

	if( NetError::NET_OK != kAck.m_iOK )
	{
		kAck.m_mapSkillList.clear();
		return Reply( kSes, EGS_GET_SKILL_ACK, kAck );
	}

	pDB->SaveSkillPoint( kRow.m_nUnitUID, iSPLeft, 0 );

	kAck.m_iRemainSP	= iSPLeft;
	kAck.m_iRemainCSP	= 0;

	Reply( kSes, EGS_GET_SKILL_ACK, kAck );

	//////////////////////////////////////////////////////////////////////////
	// Phase 6, round 6: the quest steps that ask the player to open the skill
	// tree and spend a point.
	//
	// This was the whole of the reported bug. Learning a skill worked, the SP
	// came off, the tree persisted - and the quest step never moved, because
	// nothing told the quest engine. On live it is ticked from
	// DBE_INSERT_SKILL_ACK, the *database reply* handler
	// (GSUserGameCommon.cpp:4639), which is a long way from anything
	// quest-shaped and easy to miss when porting the packet rather than the
	// feature.
	//
	// Two separate types come off one request, counted differently:
	//
	//  * USE_SKILL_POINT counts POINTS. One skill taken from level 1 to 3 is
	//    two points, and the server calls its handler once per level in a loop.
	//  * LEARN_NEW_SKILL counts SKILLS, and only ones that were not known
	//    before. The client works that list out itself and sends it in the
	//    request as m_vecNowLearnSkill (X2SkillTree.cpp:3879), so it is used
	//    verbatim rather than recomputed here - same as the server does
	//    (GSUserGameCommon.cpp:4569).
	{
		std::map< int, KGetSkillInfo >::const_iterator mitS;
		for( mitS = kAck.m_mapSkillList.begin(); mitS != kAck.m_mapSkillList.end(); ++mitS )
		{
			const int iGained = mitS->second.m_iSkillLevel - mitS->second.m_iBeforeSkillLevel;

			if( iGained > 0 )
				QuestOnUseSkillPoint( kSes, mitS->first, iGained );
		}

		QuestOnLearnNewSkill( kSes, kReq.m_vecNowLearnSkill );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

/// The dungeons a skill-use quest step is allowed to count in.
///
/// The server excludes four categories - tutorial, Henir, Ruben and the
/// training camp - via CXSLDungeon predicates its own handler calls
/// (GSUserGameCommon.cpp:8846). Transcribed here against the CLIENT's DI_*
/// enum, which is the right target and is not the same list: most of the
/// entries CXSLDungeon names are commented out of X2Lib/X2Dungeon.h, so those
/// dungeons cannot be entered from this build at all and need no case.
///
/// Every name below was checked to be a live enumerator, not merely present in
/// the file - the first attempt at this used grep and matched six commented-out
/// lines, which the compiler then rejected one by one.
static bool IsSkillUseCountedDungeon( int iDungeonID )
{
	switch( iDungeonID )
	{
	// CXSLDungeon::IsTutorialDungeon. Only these two of its fourteen entries
	// survive in this client. The room's own m_bTutorial flag is checked by the
	// caller as well and is the more reliable of the two.
	case SEnum::DI_TUTORIAL_ELSWORD:
	case SEnum::DI_BATTLE_FIELD_TUTORIAL_ELSWORD:

	// CXSLDungeon::IsHenirDungeon. The other five Henir spaces are commented
	// out here exactly as they are in the client's own IsHenirDungeon.
	case SEnum::DI_ELDER_HENIR_SPACE:

	// CXSLDungeon::IsRubenDungeon, less DI_RUBEN_SECRET_HELL, which this client
	// has no enumerator for. The two entries the server itself keeps commented
	// out are left out for the same reason it leaves them out.
	case SEnum::DI_EL_FOREST_WEST_NORMAL:
	case SEnum::DI_EL_FOREST_NORTH_NORMAL:
	case SEnum::DI_EL_FOREST_GATE_NORMAL:
	case SEnum::DI_EL_FOREST_HELL_NORMAL:
	case SEnum::DI_EVENT_KIDDAY_RUBEN:
	case SEnum::DI_MONSTER_TEST_NORMAL:
	case SEnum::DI_RUBEN_SECRET_COMMON:
	case SEnum::DI_RUBEN_EL_TREE_NORMAL:
	case SEnum::DI_RUBEN_RUIN_OF_ELF_NORMAL:
	case SEnum::DI_RUBEN_SWAMP_NORMAL:
		return false;

	default:
		break;
	}

	// CXSLDungeon::IsTCDungeon - the training camp, one contiguous range from
	// DI_TRAINING_FREE (39000) to DI_TRAINING_RAVEN_6. This client stops its
	// enum at DI_TRAINING_RAVEN_0 (39400), so the upper bound is spelled as
	// that plus six rather than as a bare 39406: the server's enum runs
	// RAVEN_0..RAVEN_6 with no explicit values (XSLDungeon.h), and writing the
	// arithmetic out keeps the derivation visible.
	if( iDungeonID >= (int)SEnum::DI_TRAINING_FREE &&
		iDungeonID <= (int)SEnum::DI_TRAINING_RAVEN_0 + 6 )
	{
		return false;
	}

	return true;
}

bool CX2OfflineServer::Handler_EGS_SKILL_USE_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_SKILL_USE_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	// No ACK exists for this one and the client arms no wait on it - it is a
	// fire-and-forget notification that only SQT_SKILL_USE listens to. Handled
	// rather than dropped so the log stops calling it an unknown packet, which
	// matters more than it sounds: it arrives on every skill cast, so an
	// unhandled line for it drowns out everything else in a dungeon run.
	if( -1 == kReq.m_iSkillID )
		return true;

	// Dungeons only, and not the ones the feature is switched off in.
	if( false == m_kRoom.m_bActive || 0 == m_kRoom.m_kInfo.m_iDungeonID )
		return true;

	if( true == m_kRoom.m_bTutorial )
		return true;

	if( false == IsSkillUseCountedDungeon( m_kRoom.m_kInfo.m_iDungeonID ) )
		return true;

	KOfflineUnitRow kRow;
	if( false == LoadQuestState( kSes, kRow ) )
		return true;

	std::vector< KQuestInstance > vecChanged;
	CX2OfflineQuest::Instance()->OnUseSkill( kReq.m_iSkillID, kRow, vecChanged );
	PushQuestUpdate( kSes, vecChanged );

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CX2OfflineServer::Handler_EGS_RESET_SKILL_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_RESET_SKILL_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	CX2OfflineDB*			pDB		= CX2OfflineDB::Instance();
	CX2OfflineSkill*		pSkill	= CX2OfflineSkill::Instance();
	CX2OfflineInventory*	pInven	= CX2OfflineInventory::Instance();

	KEGS_RESET_SKILL_ACK kAck;
	kAck.m_iOK			= NetError::ERR_RESET_SKILL_00;
	kAck.m_iDelSkillID	= kReq.m_iSkillID;
	kAck.m_iSPoint		= 0;
	kAck.m_iCSPoint		= 0;

	// See the same field in Handler_EGS_GET_SKILL_REQ - echoed, never derived.
	kAck.m_iActiveSkillPageNumber = kReq.m_iActiveSkillPageNumber;
#ifdef SERV_IRUHADEV_JUNE_SKILL_REFORM
	// June skill system S3 (plan rule 4): the client's SetUsingPage subtracts 1 unchecked, so an ACK never carries page 0.
	if( kAck.m_iActiveSkillPageNumber < 1 )
		kAck.m_iActiveSkillPageNumber = 1;
#endif SERV_IRUHADEV_JUNE_SKILL_REFORM

	KOfflineUnitRow kRow;
	if( false == pDB->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) )
		return Reply( kSes, EGS_RESET_SKILL_ACK, kAck );

	kAck.m_iSPoint = kRow.m_iSP;

	pSkill->Load( kRow.m_nUnitUID );

	int iRefund = 0;

	kAck.m_iOK = pSkill->ResetSkill( kReq.m_iSkillID, kRow.m_iUnitClass, iRefund );

	if( NetError::NET_OK != kAck.m_iOK )
		return Reply( kSes, EGS_RESET_SKILL_ACK, kAck );

	// The scroll, if there is one. See the file header for why its absence does
	// not stop the reset.
	const UidType nScrollUID = FindItemByID( CX2OfflineSkill::RI_RESET_ONE_SKILL );

	if( 0 != nScrollUID )
	{
		KInventoryItemInfo kSlotInfo;
		if( true == pInven->ConsumeOne( nScrollUID, kSlotInfo ) )
			kAck.m_vecInventorySlotInfo.push_back( kSlotInfo );
	}
	else
	{
		CX2OfflineLog::Server( L"SKILL    no skill reset scroll (item %d) in the bag - the reset is"
			L" allowed anyway offline", (int)CX2OfflineSkill::RI_RESET_ONE_SKILL );
	}

	kRow.m_iSP += iRefund;

	pDB->SaveSkillPoint( kRow.m_nUnitUID, kRow.m_iSP, 0 );

	kAck.m_iSPoint	= kRow.m_iSP;
	kAck.m_iCSPoint	= 0;

	return Reply( kSes, EGS_RESET_SKILL_ACK, kAck );
}

//////////////////////////////////////////////////////////////////////////

bool CX2OfflineServer::Handler_EGS_INIT_SKILL_TREE_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_INIT_SKILL_TREE_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	CX2OfflineDB*			pDB		= CX2OfflineDB::Instance();
	CX2OfflineSkill*		pSkill	= CX2OfflineSkill::Instance();
	CX2OfflineInventory*	pInven	= CX2OfflineInventory::Instance();

	KEGS_INIT_SKILL_TREE_ACK kAck;
	kAck.m_iOK		= NetError::ERR_ITEM_04;
	kAck.m_iSPoint	= 0;
	kAck.m_iCSPoint	= 0;

	// See the same field in Handler_EGS_GET_SKILL_REQ - echoed, never derived.
	kAck.m_iActiveSkillPageNumber = kReq.m_iActiveSkillPageNumber;
#ifdef SERV_IRUHADEV_JUNE_SKILL_REFORM
	// June skill system S3 (plan rule 4): the client's SetUsingPage subtracts 1 unchecked, so an ACK never carries page 0.
	if( kAck.m_iActiveSkillPageNumber < 1 )
		kAck.m_iActiveSkillPageNumber = 1;
#endif SERV_IRUHADEV_JUNE_SKILL_REFORM

	KOfflineUnitRow kRow;
	if( false == pDB->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) )
		return Reply( kSes, EGS_INIT_SKILL_TREE_ACK, kAck );

	kAck.m_iSPoint = kRow.m_iSP;

	pSkill->Load( kRow.m_nUnitUID );

	// Unlike the single-skill reset, this request *names* the item it is being
	// paid for: the client only sends it when the player uses a whole-tree
	// reset scroll out of the inventory. So the item is required here, exactly
	// as GSUserGameCommon.cpp:4980 requires it - there is nothing to relax.
	KOfflineItemRow kItem;
	if( false == pInven->GetItemRow( kReq.m_iItemUID, kItem ) )
	{
		CX2OfflineLog::Server( L"SKILL    refused a full tree reset - itemUID=%I64d is not in the"
			L" inventory", (__int64)kReq.m_iItemUID );

		return Reply( kSes, EGS_INIT_SKILL_TREE_ACK, kAck );
	}

	int iSPTotal = 0;

	kAck.m_iOK = pSkill->InitSkillTree( kRow.m_iLevel, kRow.m_iUnitClass, iSPTotal );

	if( NetError::NET_OK != kAck.m_iOK )
		return Reply( kSes, EGS_INIT_SKILL_TREE_ACK, kAck );

	KInventoryItemInfo kSlotInfo;
	if( true == pInven->ConsumeOne( kReq.m_iItemUID, kSlotInfo ) )
		kAck.m_vecInventorySlotInfo.push_back( kSlotInfo );

	pDB->SaveSkillPoint( kRow.m_nUnitUID, iSPTotal, 0 );

	kAck.m_iSPoint	= iSPTotal;
	kAck.m_iCSPoint	= 0;

#ifdef SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	// June skill system S7: June's INIT ACK (3220ea50) empties the client's acquired map (Reset +
	// SetDefaultSkill, and June has no default skills), so every skill the level earns goes back out
	// after it.
	const bool bAckSent = Reply( kSes, EGS_INIT_SKILL_TREE_ACK, kAck );
	PushJuneSkillGrant( kSes, kRow.m_nUnitUID, kRow.m_iUnitClass, kRow.m_iLevel, true, L"tree reset" );
	return bAckSent;
#else SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
	return Reply( kSes, EGS_INIT_SKILL_TREE_ACK, kAck );
#endif SERV_IRUHADEV_JUNE_SKILL_ACQUIRE
}

//////////////////////////////////////////////////////////////////////////

bool CX2OfflineServer::Handler_EGS_CHANGE_SKILL_SLOT_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_CHANGE_SKILL_SLOT_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	CX2OfflineSkill* pSkill = CX2OfflineSkill::Instance();

	KEGS_CHANGE_SKILL_SLOT_ACK kAck;
	kAck.m_iOK			= NetError::ERR_SKILL_09;
	kAck.m_iSlotID		= kReq.m_iSlotID;
	kAck.m_iSkillID		= kReq.m_iSkillID;
	kAck.m_iSlotID2		= -1;
	kAck.m_iSkillID2	= 0;

	if( 0 == kSes.m_nSelectedUnitUID )
		return Reply( kSes, EGS_CHANGE_SKILL_SLOT_ACK, kAck );

	pSkill->Load( kSes.m_nSelectedUnitUID );

	int iSlotID2	= -1;
	int iSkillID2	= 0;

	kAck.m_iOK = pSkill->ChangeSkillSlot( kReq.m_iSlotID, kReq.m_iSkillID, iSlotID2, iSkillID2 );

	if( NetError::NET_OK != kAck.m_iOK )
		return Reply( kSes, EGS_CHANGE_SKILL_SLOT_ACK, kAck );

	// The server overwrites slot 1 from the request and reports slot 2 from the
	// derived pair, which is the shape the client's ACK handler reads: it calls
	// SetSkillSlotInfo for slot 1 unconditionally and for slot 2 only when
	// m_iSlotID2 is not -1 (X2SkillTree.cpp:1669-1677).
	kAck.m_iSlotID		= kReq.m_iSlotID;
	kAck.m_iSkillID		= kReq.m_iSkillID;
	kAck.m_iSlotID2		= iSlotID2;
	kAck.m_iSkillID2	= iSkillID2;

	// No EGS_CHANGE_SKILL_SLOT_NOT. Its client handler only does anything in
	// XS_PVP_ROOM and only for another player's slot data
	// (X2SkillTree.cpp:1732-1743), so offline it has nothing to update.

	return Reply( kSes, EGS_CHANGE_SKILL_SLOT_ACK, kAck );
}

//////////////////////////////////////////////////////////////////////////

UidType CX2OfflineServer::FindItemByID( int iItemID )
{
	CX2OfflineInventory* pInven = CX2OfflineInventory::Instance();

	std::map< UidType, KInventoryItemInfo > mapItem;
	pInven->GetAllItems( mapItem );

	std::map< UidType, KInventoryItemInfo >::const_iterator mit;
	for( mit = mapItem.begin(); mit != mapItem.end(); ++mit )
	{
		if( mit->second.m_kItemInfo.m_iItemID != iItemID )
			continue;

		// Not one that is being worn - a consumable never is, but the guard
		// costs nothing and stops a future caller from eating equipment.
		if( CX2Inventory::ST_E_EQUIP == (int)mit->second.m_cSlotCategory )
			continue;

		return mit->first;
	}

	return 0;
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
//////////////////////////////////////////////////////////////////////////
// June skill system S7c: the skill memos ( traits ), REQ 0x554 / 0x556.
//
// There is no June server, so both handlers are a labelled stand-in built from June's client: they repeat the checks the client
// makes before it sends ( 3235b070 for switching on, 3235b750 / 3235be30 for switching off ), keep one memo per skill ( the unit
// info carries a map, skill -> memo ), spend the removal item the client named, and log every decision. A refusal answers
// ERR_UNKNOWN, which the client shows through IsValidPacket. The skill in both REQs is already the base ( 32207210 ).
//////////////////////////////////////////////////////////////////////////
bool CX2OfflineServer::Handler_EGS_JUNE_ACTIVE_SKILL_MEMO_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_JUNE_ACTIVE_SKILL_MEMO_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	KEGS_JUNE_ACTIVE_SKILL_MEMO_ACK kAck;
	kAck.m_iOK		= NetError::ERR_UNKNOWN;
	kAck.m_iSkillID	= kReq.m_iSkillID;
	kAck.m_iMemoID	= kReq.m_iMemoID;

	KOfflineUnitRow kRow;
	if( false == CX2OfflineDB::Instance()->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO emu activate refused: no selected unit" );
		return Reply( kSes, EGS_JUNE_ACTIVE_SKILL_MEMO_ACK, kAck );
	}

	CX2OfflineSkill* pSkill = CX2OfflineSkill::Instance();
	pSkill->Load( kRow.m_nUnitUID );

	const SJuneSkillMemoTemplet* pMemo = CX2SkillNoteManager::GetInstance()->GetJuneMemo( kReq.m_iSkillID, kReq.m_iMemoID, kRow.m_iUnitClass );
	int iUpgradedSkillID = -1;
	if( NULL != g_pData && NULL != g_pData->GetSkillTree() )
		iUpgradedSkillID = g_pData->GetSkillTree()->GetUpgradedSkillID( kReq.m_iSkillID );
	const bool bLearned = ( 0 < pSkill->GetSkillLevel( kReq.m_iSkillID ) ) || ( 0 < iUpgradedSkillID && 0 < pSkill->GetSkillLevel( iUpgradedSkillID ) );

	const wchar_t* szRefusal = NULL;
	if( NULL == pMemo )
		szRefusal = L"the class has no such memo row";
	else if( kRow.m_iLevel < pMemo->m_iRequireLevel )
		szRefusal = L"below the row's level";
	else if( 0 != pSkill->GetJuneActiveMemo( kReq.m_iSkillID ) )
		szRefusal = L"the skill already has a memo";
	else if( false == bLearned )
		szRefusal = L"the skill is not learned";
	else if( false == pSkill->SetJuneActiveMemo( kReq.m_iSkillID, kReq.m_iMemoID ) )
		szRefusal = L"save failed";

	if( NULL != szRefusal )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO emu activate refused unit=%I64d class=%d L%d skill=%d memo=%d ( %s; active %d )",
			(__int64)kRow.m_nUnitUID, kRow.m_iUnitClass, kRow.m_iLevel, kReq.m_iSkillID, kReq.m_iMemoID, szRefusal,
			pSkill->GetJuneActiveMemo( kReq.m_iSkillID ) );
		return Reply( kSes, EGS_JUNE_ACTIVE_SKILL_MEMO_ACK, kAck );
	}

	kAck.m_iOK = NetError::NET_OK;
	CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO emu activated unit=%I64d skill=%d memo=%d ( %u active memo(s), stand-in )",
		(__int64)kRow.m_nUnitUID, kReq.m_iSkillID, kReq.m_iMemoID, (unsigned int)pSkill->GetJuneActiveMemos().size() );
	return Reply( kSes, EGS_JUNE_ACTIVE_SKILL_MEMO_ACK, kAck );
}

bool CX2OfflineServer::Handler_EGS_JUNE_DEACTIVE_SKILL_MEMO_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_JUNE_DEACTIVE_SKILL_MEMO_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	KEGS_JUNE_DEACTIVE_SKILL_MEMO_ACK kAck;
	kAck.m_iOK		= NetError::ERR_UNKNOWN;
	kAck.m_iSkillID	= kReq.m_iSkillID;
	kAck.m_iMemoID	= kReq.m_iMemoID;

	KOfflineUnitRow kRow;
	if( false == CX2OfflineDB::Instance()->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO emu deactivate refused: no selected unit" );
		return Reply( kSes, EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK, kAck );
	}

	CX2OfflineSkill*		pSkill	= CX2OfflineSkill::Instance();
	CX2OfflineInventory*	pInven	= CX2OfflineInventory::Instance();
	pSkill->Load( kRow.m_nUnitUID );

	std::vector< int > vecRemovableItemID;
	CX2SkillNoteManager::GetInstance()->GetJuneRemovableMemoItemIDList( vecRemovableItemID );

	// every check first, then the item is spent, then the memo row goes ( the order Handler_EGS_REG_SKILL_NOTE_MEMO_REQ keeps )
	KOfflineItemRow kItemRow;
	const bool bInBag = pInven->GetItemRow( kReq.m_iItemUID, kItemRow );
	const wchar_t* szRefusal = NULL;
	if( 0 == kReq.m_iMemoID || kReq.m_iMemoID != pSkill->GetJuneActiveMemo( kReq.m_iSkillID ) )
		szRefusal = L"the memo is not active on the skill";
	else if( false == bInBag )
		szRefusal = L"the removal item is not in the bag";
	else if( vecRemovableItemID.end() == std::find( vecRemovableItemID.begin(), vecRemovableItemID.end(), kItemRow.m_iItemID ) )
		szRefusal = L"the item is not a memo removal item";

	KInventoryItemInfo kSlotInfo;
	if( NULL == szRefusal && false == pInven->ConsumeOne( kReq.m_iItemUID, kSlotInfo ) )
		szRefusal = L"the item could not be spent";
	if( NULL == szRefusal && false == pSkill->ClearJuneActiveMemo( kReq.m_iSkillID ) )
		szRefusal = L"save failed";

	if( NULL != szRefusal )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO emu deactivate refused unit=%I64d skill=%d memo=%d itemUID=%I64d ( %s; active %d )",
			(__int64)kRow.m_nUnitUID, kReq.m_iSkillID, kReq.m_iMemoID, (__int64)kReq.m_iItemUID, szRefusal,
			pSkill->GetJuneActiveMemo( kReq.m_iSkillID ) );
		return Reply( kSes, EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK, kAck );
	}

	kAck.m_iOK = NetError::NET_OK;
	kAck.m_vecInventorySlotInfo.push_back( kSlotInfo );
	CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO emu deactivated unit=%I64d skill=%d memo=%d, spent item %d ( uid %I64d ), stand-in",
		(__int64)kRow.m_nUnitUID, kReq.m_iSkillID, kReq.m_iMemoID, kItemRow.m_iItemID, (__int64)kReq.m_iItemUID );
	return Reply( kSes, EGS_JUNE_DEACTIVE_SKILL_MEMO_ACK, kAck );
}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

#ifdef SERV_IRUHADEV_JUNE_SKILL_PAGE
//////////////////////////////////////////////////////////////////////////
// June skill system S7d: the skill pages, a LABELLED STAND-IN ( no June server; the shape is March's GSUserGameCommon.cpp page
// handlers, the data June's: a page is a bar, the skill list and the memos are shared, and the decide ACK carries the bar in
// KJuneSkillExtraData, which June's handler 3220f2b0 reads ).

/// The ED a page costs: KUserSkillTree::ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE ( UserSkillTree.h:344 ), the real server's figure.
static const int JUNE_ED_PER_SKILL_PAGE = 50000000;

/// GSUserGameCommon.cpp EGS_GET_NEXT_SKILL_PAGE_ED_REQ: with 1 or 2 pages open the next one costs the constant, else ERR_SKILL_PAGE_02.
/// June's dialog has no ED button, so nothing in June's UI sends this; it is answered for completeness.
bool CX2OfflineServer::Handler_EGS_GET_NEXT_SKILL_PAGE_ED_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_GET_NEXT_SKILL_PAGE_ED_ACK kAck;
	kAck.m_iOK	= NetError::ERR_SKILL_PAGE_02;
	kAck.m_iED	= 0;

	CX2OfflineSkill* pSkill = CX2OfflineSkill::Instance();
	if( 0 == kSes.m_nSelectedUnitUID || false == pSkill->Load( kSes.m_nSelectedUnitUID ) )
		return Reply( kSes, EGS_GET_NEXT_SKILL_PAGE_ED_ACK, kAck );

	int iPageCount = 1, iActivePage = 1;
	pSkill->GetJuneSkillPageInfo( iPageCount, iActivePage );
	if( iPageCount < CX2OfflineSkill::JUNE_MAX_SKILL_PAGE )
	{
		kAck.m_iOK	= NetError::NET_OK;
		kAck.m_iED	= JUNE_ED_PER_SKILL_PAGE;
	}

	CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE next-page ED: %d page(s) open -> %d ED ( OK %d )", iPageCount, kAck.m_iED, kAck.m_iOK );
	return Reply( kSes, EGS_GET_NEXT_SKILL_PAGE_ED_ACK, kAck );
}

/// GSUserGameCommon.cpp EGS_EXPAND_SKILL_PAGE_REQ ( pay with ED ): below the cap and with the ED, the ED goes and a page is added.
bool CX2OfflineServer::Handler_EGS_EXPAND_SKILL_PAGE_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	CX2OfflineDB*		pDB		= CX2OfflineDB::Instance();
	CX2OfflineSkill*	pSkill	= CX2OfflineSkill::Instance();

	KEGS_EXPAND_SKILL_PAGE_ACK kAck;
	kAck.m_iOK			= NetError::ERR_SKILL_PAGE_02;
	kAck.m_bPayWithED	= true;

	KOfflineUnitRow kRow;
	if( false == pDB->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) || false == pSkill->Load( kRow.m_nUnitUID ) )
		return Reply( kSes, EGS_EXPAND_SKILL_PAGE_ACK, kAck );

	kAck.m_iED = kRow.m_iED;

	int iPageCount = 1, iActivePage = 1;
	pSkill->GetJuneSkillPageInfo( iPageCount, iActivePage );
	if( iPageCount >= CX2OfflineSkill::JUNE_MAX_SKILL_PAGE )
		return Reply( kSes, EGS_EXPAND_SKILL_PAGE_ACK, kAck );

	if( kRow.m_iED < JUNE_ED_PER_SKILL_PAGE )
	{
		CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE expand by ED refused: %d ED, a page costs %d", kRow.m_iED, JUNE_ED_PER_SKILL_PAGE );
		kAck.m_iOK = NetError::ERR_EXPAND_INVENTORY_ED_01;
		return Reply( kSes, EGS_EXPAND_SKILL_PAGE_ACK, kAck );
	}

	kAck.m_iOK = pSkill->ExpandJuneSkillPage( 1, iPageCount );
	if( NetError::NET_OK != kAck.m_iOK )
		return Reply( kSes, EGS_EXPAND_SKILL_PAGE_ACK, kAck );

	kRow.m_iED -= JUNE_ED_PER_SKILL_PAGE;
	pDB->SaveProgress( kRow.m_nUnitUID, kRow.m_iLevel, kRow.m_iEXP, kRow.m_iED );

	kAck.m_iED								= kRow.m_iED;
	kAck.m_iTheNumberOfSkillPagesAvailable	= iPageCount;
	kAck.m_iSPointAvailable					= kRow.m_iSP;
	kAck.m_iCSPointAvailable				= 0;

	CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE expanded by ED: %d page(s), %d ED left", iPageCount, kRow.m_iED );
	return Reply( kSes, EGS_EXPAND_SKILL_PAGE_ACK, kAck );
}

/// GSUserGameCommon.cpp EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ: page 1 .. the open count. The stand-in swaps the bars, answers with the
/// page's bar in KJuneSkillExtraData ( and in m_kUserSkillPageData, which June does not read ), then re-sends the shared skill list
/// ( EGS_JUNE_ACQUIRE_SKILL_NOT ): June's grant handler only adds to the page in use, which is now the new one.
bool CX2OfflineServer::Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ( KOfflineSession& kSes, const KEvent& kEvent )
{
	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ kReq;
	if( false == ReadReq( kEvent, kReq ) )
		return false;

	CX2OfflineDB*		pDB		= CX2OfflineDB::Instance();
	CX2OfflineSkill*	pSkill	= CX2OfflineSkill::Instance();

	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK kAck;
	kAck.m_iOK								= NetError::ERR_SKILL_PAGE_03;
	kAck.m_iSkillPagesNumberDecidedToUse	= 1;	///< never 0: SetUsingPage subtracts 1 ( MARCH_2014_MIGRATION.md:1068-1100 )

	KOfflineUnitRow kRow;
	if( false == pDB->LoadUnit( kSes.m_nSelectedUnitUID, kRow ) || false == pSkill->Load( kRow.m_nUnitUID ) )
		return Reply( kSes, EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, kAck );

	kAck.m_iOK = pSkill->SwitchJuneSkillPage( kReq.m_iSkillPagesNumberDecidedToUse );
	if( NetError::NET_OK != kAck.m_iOK )
		return Reply( kSes, EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, kAck );

	kAck.m_iSkillPagesNumberDecidedToUse	= kReq.m_iSkillPagesNumberDecidedToUse;
	kAck.m_iSPointAvailable					= kRow.m_iSP;
	kAck.m_iCSPointAvailable				= 0;

	{
		KUnitSkillData kUnitSkill;
		pSkill->FillUnitSkillData( kUnitSkill );
		for( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT; ++i )
		{
			kAck.m_kUserSkillPageData.m_aEquippedSkill[i]		= kUnitSkill.m_aEquippedSkill[i];
			kAck.m_kUserSkillPageData.m_aEquippedSkillSlotB[i]	= kUnitSkill.m_aEquippedSkillSlotB[i];
		}
		pSkill->GetAcquiredSkills( kAck.m_kUserSkillPageData.m_vecUserSkillData );
		kAck.m_kUserSkillPageData.m_usSkillPoint		= (USHORT)kRow.m_iSP;
		kAck.m_kUserSkillPageData.m_usCashSkillPoint	= 0;

		kAck.m_kJuneSkillExtraData.m_mapIntInt = kUnitSkill.m_kJuneSkillExtraData.m_mapIntInt;
		pSkill->GetJuneEquippedSkillVectors( kAck.m_kJuneSkillExtraData.m_vecSkillDataA, kAck.m_kJuneSkillExtraData.m_vecSkillDataB );
	}

	CX2OfflineLog::Server( L"[JUNE_SKILL] PAGE decide: page %d in use, A %d/%d/%d/%d B %d/%d/%d/%d, %d memo(s)",
		kAck.m_iSkillPagesNumberDecidedToUse,
		(int)kAck.m_kJuneSkillExtraData.m_vecSkillDataA[0].m_iSkillID, (int)kAck.m_kJuneSkillExtraData.m_vecSkillDataA[1].m_iSkillID,
		(int)kAck.m_kJuneSkillExtraData.m_vecSkillDataA[2].m_iSkillID, (int)kAck.m_kJuneSkillExtraData.m_vecSkillDataA[3].m_iSkillID,
		(int)kAck.m_kJuneSkillExtraData.m_vecSkillDataB[0].m_iSkillID, (int)kAck.m_kJuneSkillExtraData.m_vecSkillDataB[1].m_iSkillID,
		(int)kAck.m_kJuneSkillExtraData.m_vecSkillDataB[2].m_iSkillID, (int)kAck.m_kJuneSkillExtraData.m_vecSkillDataB[3].m_iSkillID,
		(int)kAck.m_kJuneSkillExtraData.m_mapIntInt.size() );

	const bool bAckSent = Reply( kSes, EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, kAck );
	PushJuneSkillGrant( kSes, kRow.m_nUnitUID, kRow.m_iUnitClass, kRow.m_iLevel, true, L"page switch" );
	return bAckSent;
}
#endif SERV_IRUHADEV_JUNE_SKILL_PAGE

#endif SERV_IRUHADEV_OFFLINE
