#include "stdafx.h"
#include ".\x2unit.h"
#if defined( SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP ) && defined( SERV_IRUHADEV_OFFLINE )
#include "Offline/X2OfflineLog.h"
#endif // SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP && SERV_IRUHADEV_OFFLINE

CX2Unit::CX2Unit( const KUnitInfo& unitInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, unitInfo );	
    m_UnitData.Init( this, unitInfo );
	Init();
}

CX2Unit::CX2Unit( const KRoomUserInfo& RoomUserInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, RoomUserInfo );
    m_UnitData.Init( this, RoomUserInfo );
	Init();
#ifdef SERV_PET_SYSTEM
	if( RoomUserInfo.m_vecPet.size() > 0 )
		SetFullPetInfo( RoomUserInfo.m_vecPet[0] );
#endif
#ifdef SERV_GATE_OF_DARKNESS_SUPPORT_EVENT
	m_iGateOfDarknessSupportEventTime = RoomUserInfo.m_iGateOfDarknessSupportEventTime;
#endif SERV_GATE_OF_DARKNESS_SUPPORT_EVENT
}

CX2Unit::CX2Unit( const KSquareUserInfo& pKSquareUserInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, pKSquareUserInfo );
    m_UnitData.Init( this, pKSquareUserInfo );
	Init();
}


CX2Unit::CX2Unit( const KFieldUserInfo& pKFieldUserInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, pKFieldUserInfo );
    m_UnitData.Init( this, pKFieldUserInfo );
	Init();
#ifdef SERV_PET_SYSTEM
	if( pKFieldUserInfo.m_vecPet.size() > 0 )
		SetSimplePetInfo( pKFieldUserInfo.m_vecPet[0] );
#endif
}


void CX2Unit::Init()
{
	m_pUnitTemplet				= g_pData->GetUnitManager()->GetUnitTemplet( m_UnitData.m_UnitClass );
    ASSERT( m_pUnitTemplet != NULL );

	m_bIsLevelUp				= false;	
	m_PrevLevel					= m_UnitData.m_Level;

	m_PrevEXP					= m_UnitData.m_EXP;
	m_PrevNowBaseLevelEXP		= m_UnitData.m_NowBaseLevelEXP;
	m_PrevNextBaseLevelEXP		= m_UnitData.m_NextBaseLevelEXP;

	m_ResurrectionStoneNum		= 0;

	//{{ 2007. 10. 5  최육사  
	m_bMyInfoInDungoenRoom		= false;
	//}}

	memset( m_ViewEqipPosition,		0, sizeof(bool) * EP_END );
	memset( m_FashionEqipPosition,	0, sizeof(bool) * EP_END );
	memset( m_NormalEqipPosition,	0, sizeof(bool) * EP_END );

	ResetEqip();

	//{{ kimhc // 2009-12-04 // 크리스마스 이벤트
#ifdef	CHRISTMAS_TREE
	m_mapEventData.clear();
#else	CHRISTMAS_TREE
	m_iMarketingEventID						= -1;
	m_fMarketingEventTime					= 0.f;
#endif	CHRISTMAS_TREE
	//}} kimhc // 2009-12-04 // 크리스마스 이벤트
	m_bReceivedGuestUserReward				= true;
	m_bGuestUserReachedLevelLimit			= false;
	m_bGuestUserMovingToCharacterSelect		= false;


	//{{ 허상형 : [2009/10/14] //	자동 결제 부활석
#ifdef AUTO_PAYMENT
	m_AutoResStoneNum = 0;
#endif //	AUTO_PAYMENT
	//}} 허상형 : [2009/10/14] //	자동 결제 부활석

#ifdef SERV_SKILL_NOTE
	m_iMaxSKillNotePage = 0;
#endif

#ifdef SERV_PET_SYSTEM
	m_SummonPetUid = 0;
#endif

#ifdef SERV_NEW_UNIT_TRADE_LIMIT
	m_bTradeBlockUnit = false;
#endif SERV_NEW_UNIT_TRADE_LIMIT

#ifdef	SERV_EXPAND_QUICK_SLOT
//{{ Iruha : 2026-08-27 // All 6 consumable quick slots open by default
#ifdef SERV_IRUHADEV_QUICK_SLOT_FULL_FREE
	m_bExpandQuickSlot = true;
#else
	m_bExpandQuickSlot = false;
#endif SERV_IRUHADEV_QUICK_SLOT_FULL_FREE
//}}
#endif  SERV_EXPAND_QUICK_SLOT

	SetSumOfItemLevel( 0 );

#ifdef SERV_NEW_DEFENCE_DUNGEON
	m_iRecentEnterDungeonID = 0;
#endif // SERV_NEW_DEFENCE_DUNGEON

#ifdef REFORM_ENTRY_POINT	 	// 13-11-11, 진입 구조 개편, kimjh
	m_eServerGroupID = SEnum::SGI_INVALID;
#endif // REFORM_ENTRY_POINT	// 13-11-11, 진입 구조 개편, kimjh

#ifdef SERV_GATE_OF_DARKNESS_SUPPORT_EVENT
	m_iGateOfDarknessSupportEventTime = 0;
#endif SERV_GATE_OF_DARKNESS_SUPPORT_EVENT
#ifdef SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET
	m_kSocketAggregate.Recreate();		/// June 32363ca0 @32363cb9: the old aggregate's vslot 0( 1 ), then new 0x468 ( 32091080 )
#endif SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET
}

CX2Unit::~CX2Unit(void)
{
	//SAFE_DELETE( m_pUnitData );

	m_ViewEqipItemUIDList.clear();
	m_FashionEqipItemUIDList.clear();
	m_NormalEqipItemUIDList.clear();

	//{{ kimhc // 2009-12-04 // 크리스마스 이벤트
#ifdef	CHRISTMAS_TREE
	m_mapEventData.clear();
#endif	CHRISTMAS_TREE

#ifdef SERV_PET_SYSTEM
	m_vecPetInfo.clear();
#endif

#ifdef RIDING_SYSTEM
	m_vecRidingPetInfo.clear();
#endif //RIDING_SYSTEM
}





void CX2Unit::Reset( const KUnitInfo& unitInfo, bool bForceUpdate /* = false */ )
{
#ifdef SERV_NEW_YEAR_EVENT_2014
	bool bRefresh2014UI = false;

	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		GetUID() == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		if( static_cast< int >( unitInfo.m_ucLevel ) != static_cast< int >( m_UnitData.m_Level ) ||
			static_cast< CX2Unit::UNIT_CLASS >( unitInfo.m_cUnitClass ) != m_UnitData.m_UnitClass )
		{
			bRefresh2014UI = true;
		}
	}
#endif SERV_NEW_YEAR_EVENT_2014

#ifdef SERV_EVENT_CHECK_POWER
	bool bRefreshCheckPower = false;

	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		GetUID() == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		if( static_cast< CX2Unit::UNIT_CLASS >( unitInfo.m_cUnitClass ) != m_UnitData.m_UnitClass )
		{
			bRefreshCheckPower = true;
		}
	}
#endif SERV_EVENT_CHECK_POWER

	// XS_PVP_GAME = 9
	if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME ) 
	{
		if ( (int)m_UnitData.m_Level < (int)unitInfo.m_ucLevel )
		{
			SetIsLevelUp( true );
		}
	}
	

	//SAFE_DELETE( m_pUnitData );

	m_ViewEqipItemUIDList.resize(0);
	m_FashionEqipItemUIDList.resize(0);
	m_NormalEqipItemUIDList.resize(0);
	

	//m_pUnitData		= new CX2Unit::UnitData( this, unitInfo );
	m_PrevLevel					= m_UnitData.m_Level;

	m_PrevEXP					= m_UnitData.m_EXP;
	m_PrevNowBaseLevelEXP		= m_UnitData.m_NowBaseLevelEXP;
	m_PrevNextBaseLevelEXP		= m_UnitData.m_NextBaseLevelEXP;

	m_UnitData.SetKUnitInfo( unitInfo );
	m_pUnitTemplet	= g_pData->GetUnitManager()->GetUnitTemplet( m_UnitData.m_UnitClass );

#ifdef SERV_LIMITED_DUNGEON_PLAY_TIMES
	m_mapDungeonPlay.clear();
	m_mapDungeonPlay.insert( unitInfo.m_mapDungeonPlay.begin(), unitInfo.m_mapDungeonPlay.end() );
#endif SERV_LIMITED_DUNGEON_PLAY_TIMES

	
	if ( bForceUpdate == true )
	{
		// Dungeon Clear Data
		m_mapDungeonClear.clear();
		m_mapDungeonClear.insert( unitInfo.m_mapDungeonClear.begin(), unitInfo.m_mapDungeonClear.end() );
	}
	else
	{
		std::map< int, KDungeonClearInfo >::const_iterator mit;

		for ( mit = unitInfo.m_mapDungeonClear.begin(); mit != unitInfo.m_mapDungeonClear.end(); mit++ )
		{
			std::map< int, KDungeonClearInfo >::const_iterator iter2;
			iter2 = m_mapDungeonClear.find( mit->first );
			if( iter2 == m_mapDungeonClear.end() )
			{
				m_mapDungeonClear.insert( std::make_pair( mit->first, mit->second ) );
			}
		}
	}

#ifdef SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	// June 32362730 @323629d7-32362a5d: after the dungeon-clear merge, every cleared dungeon with a movie row adds that movie to
	// g_pMain's seen list as watched ( 31d758c0, then 31d78bf0( id, false ) ). Any unit's Reset does it ( quest-system Q8b, JUNE_BUGS ).
	{
		std::map< int, KDungeonClearInfo >::const_iterator mitMovie;
		for( mitMovie = m_mapDungeonClear.begin(); mitMovie != m_mapDungeonClear.end(); ++mitMovie )
		{
			int iMovieID = 0;
			if( true == g_pMain->GetClearMovieIDByDungeon( mitMovie->first, iMovieID ) )
				g_pMain->AddSeenClearMovie( iMovieID, false );
		}
	}
#endif SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB
	m_mapTCClear.clear();
	m_mapTCClear = unitInfo.m_mapTCClear;

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
	if( NULL != m_pUnitTemplet )
	{
		g_pData->GetItemStatCalculator().SetUnitData( m_pUnitTemplet->m_UnitType, m_pUnitTemplet->m_UnitClass);
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
		g_pData->GetItemStatCalculator().SetUnitClassTran( m_UnitData.m_iUnitClassTran );		// June 32362730 @32362b2a passes +0x3c
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	}
#endif //SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef REFORM_SKILL_NOTE_UI
	if( m_pUnitTemplet->m_UnitClass != CX2SkillNoteManager::GetInstance()->GetUnitClass() )
	{
		CX2SkillNoteManager::GetInstance()->SetUnitClass( m_pUnitTemplet->m_UnitClass );
		CX2SkillNoteManager::GetInstance()->ResetMemoList();
	}
#endif // REFORM_SKILL_NOTE_UI

	ResetEqip();

#ifdef SERV_NEW_YEAR_EVENT_2014
	if( bRefresh2014UI )
		g_pMain->GetMemoryHolder()->UpdateNewYear2014Event();
#endif SERV_NEW_YEAR_EVENT_2014

#ifdef SERV_EVENT_CHECK_POWER
	if( bRefreshCheckPower )
		g_pMain->GetMemoryHolder()->UpdateCheckPowerEvent();
#endif SERV_EVENT_CHECK_POWER
}



void CX2Unit::ResetInventory( std::map< int, int >& mapInventorySlotSize, std::map< UidType, KInventoryItemInfo >& mapItem )
{
	//ASSERT( m_pUnitData != NULL );
    m_UnitData.m_Inventory.Reset( mapInventorySlotSize, mapItem ); 
}

void CX2Unit::ResetEqip()
{
	memset( m_ViewEqipPosition,		0, sizeof(bool) * EP_END );
	memset( m_FashionEqipPosition,	0, sizeof(bool) * EP_END );
	memset( m_NormalEqipPosition,	0, sizeof(bool) * EP_END );
	
	m_NormalEqipItemUIDList.resize(0);
	m_ViewEqipItemUIDList.resize(0);
	m_FashionEqipItemUIDList.resize(0);


	for( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		CX2Item* pCX2Item = GetInventory().GetItem( m_UnitData.m_NowEqipItemUIDList[i] );
//		ASSERT( pCX2Item != NULL );
		if( pCX2Item == NULL )
			continue;

		if ( IsPossibleAddEqip( pCX2Item ) == false )
			continue;

		//아이템 추가
		if( pCX2Item->GetItemTemplet()->GetFashion() == true )
		{
			m_FashionEqipItemUIDList.push_back( pCX2Item->GetUID() );
		}
		else
		{
			m_NormalEqipItemUIDList.push_back( pCX2Item->GetUID() );
		}

		//장착위치 업데이트
		UpdateEqipPosition( pCX2Item, true );

	}

	//{{ dmlee 2008.05.12 
	//ViewList를 업데이트한다
	UpdateViewEqip();
	UpdateViewEqipPosition();
	//}} dmlee 2008.05.12
}



void CX2Unit::ResetUnitClass( CX2Unit::UNIT_CLASS eUnitClass )
{
	if( m_UnitData.m_UnitClass == eUnitClass )
		return;

	m_UnitData.m_UnitClass = eUnitClass;
	m_UnitData.m_UserSkillTree.SetUnitClass( eUnitClass );
#ifdef GUILD_SKILL	
	//{{ [11/11/2009 : oasis907 ]
	// 임시
	//m_UnitData.m_GuildClass = 0; // Not Now
	m_UnitData.m_UserSkillTree.SetGuildClass( 0 );
	//}}  [11/11/2009 : oasis907 ]
#endif GUILD_SKILL
	m_pUnitTemplet = g_pData->GetUnitManager()->GetUnitTemplet( eUnitClass );

	
}


bool CX2Unit::AddEqip( UidType itemUID )
{
	CX2Item* pCX2Item = GetInventory().GetItem( itemUID );
	return AddEqip( pCX2Item );
}

bool CX2Unit::AddEqip( CX2Item* pItem )
{
	//자기 인벤토리에 있는 아이템인지 확인해봐야 하지 않을까??

	if( pItem == NULL )
		return false;
/*
	if ( pItem->GetItemTemplet()->GetEqipType() == CX2Item::ET_NONE )
		return false;

	//능력치가 되는지 확인한다
	if( EqipAbility( pItem ) == false )
		return false;

	//장착된 장비의 위치 정보를 업데이트한다
	//UpdateFashionEqipPosition();
	//UpdateNormalEqipPosition();

	//충돌되지 않는지 확인한다
	if( EqipConflict( pItem ) == true )
		return false;
*/

	if ( IsPossibleAddEqip( pItem ) == false )
		return false;

	//아이템 추가
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		m_FashionEqipItemUIDList.push_back( pItem->GetUID() );
	}
	else
	{
		m_NormalEqipItemUIDList.push_back( pItem->GetUID() );
	}

	//장착 리스트 업데이트
	m_UnitData.m_NowEqipItemUIDList.push_back( pItem->GetUID() );

	//장착위치 업데이트
	UpdateEqipPosition( pItem, true );

	//ViewList를 업데이트한다
	UpdateViewEqip();
	UpdateViewEqipPosition();
	
	return true;
}

bool CX2Unit::RemoveEqip( UidType itemUID )
{
	CX2Item* pCX2Item = GetInventory().GetItem( itemUID );
	return RemoveEqip( pCX2Item );
}

bool CX2Unit::RemoveEqip( CX2Item* pItem )
{
	if( pItem == NULL )
		return false;

	if( pItem->GetItemTemplet() == NULL )
		return false;

	if ( pItem->GetItemTemplet()->GetUseType() == CX2Item::UT_NONE )
		return false;

	//아이템 제거
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		for ( int i = 0; i < (int)m_FashionEqipItemUIDList.size(); i++ )
		{
			UidType uidType = m_FashionEqipItemUIDList[i];
			if ( uidType == pItem->GetUID() )
			{
				m_FashionEqipItemUIDList.erase( m_FashionEqipItemUIDList.begin() + i );
				break;
			}
		}
	}
	else
	{
		for ( int i = 0; i < (int)m_NormalEqipItemUIDList.size(); i++ )
		{
			UidType uidType = m_NormalEqipItemUIDList[i];
			if ( uidType == pItem->GetUID() )
			{
				m_NormalEqipItemUIDList.erase( m_NormalEqipItemUIDList.begin() + i );
				break;
			}
		}
	}

	//장착 리스트 업데이트
	for ( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType uidType = m_UnitData.m_NowEqipItemUIDList[i];
		if ( uidType == pItem->GetUID() )
		{
			m_UnitData.m_NowEqipItemUIDList.erase( m_UnitData.m_NowEqipItemUIDList.begin() + i );
			break;
		}
	}

	//장착위치 업데이트
	UpdateEqipPosition( pItem, false );

	//ViewList를 업데이트한다
	UpdateViewEqip();
	UpdateViewEqipPosition();

	return true;
}

bool CX2Unit::IsPossibleAddEqip( UidType itemUID )
{
	CX2Item* pCX2Item = GetInventory().GetItem( itemUID );
	return IsPossibleAddEqip( pCX2Item );
}
bool CX2Unit::IsPossibleAddEqip( CX2Item* pItem )
{
	if( pItem == NULL )
		return false;

	if( pItem->GetItemTemplet() == NULL )
		return false;

	if ( pItem->GetItemTemplet()->GetUseType() == CX2Item::UT_NONE )
		return false;

	//능력치가 되는지 확인한다
	if( EqipAbility( pItem ) == false )
		return false;

	//충돌되지 않는지 확인한다
	if( EqipConflict( pItem ) == true )
		return false;


	return true;
}

bool CX2Unit::CheckNowEquipItem( UidType itemUID )
{
	for ( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType uidType = m_UnitData.m_NowEqipItemUIDList[i];

		if ( uidType == itemUID )
		{
			return true;
		}
	}

	return false;
}

bool CX2Unit::EqipAbility( int itemID )
{
	CX2Item x2Item( itemID );
	return EqipAbility( &x2Item );
}


bool CX2Unit::EqipAbility( CX2Item* pItem )
{
	if ( pItem == NULL )
		return false;
	
	const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
	if ( pItemTemplet == NULL )
		return false;

	if( false == CX2Unit::CanEquipAsParts(
        pItemTemplet->GetItemID(),
        this, m_UnitData.m_Level ) )
		return false;


	// 내 유닛의 경우 item data가 유효하지만, 내 유닛이 아닌 경우에 itemdata가 유효하지 않을 수 있다.
	// 따라서 내 유닛이 아닌 경우에 내구도 검사 안함
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		GetUID() == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		// 내구도 검사
		const CX2Item::ItemData& kItemData = pItem->GetItemData();
		if( CX2Item::PT_ENDURANCE == kItemData.m_PeriodType )
		{
			if( kItemData.m_Endurance <= 0 )
				return false;
		}
#ifdef ITEM_RECOVERY_TEST
		// 강화 레벨 검사 (사용불능인가?)
		if( true == pItem->IsDisabled() )
		{
			return false;
		}
#endif //ITEM_RECOVERY_TEST
		
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		if( false == pItem->GetIsEvaluation() )
		{// 미감정 상태라면 착용 할 수 없도록 하기
			return false;
		}
#endif // NEW_ITEM_SYSTEM_2013
	}

	return true;
}

bool CX2Unit::AddClearDungeon( int dungeonID )
{
	std::map< int, KDungeonClearInfo >::iterator mit;
	mit = m_mapDungeonClear.find( dungeonID );
	if ( mit == m_mapDungeonClear.end() )
	{
		KDungeonClearInfo kDungeonClearInfo;
		
		m_mapDungeonClear.insert( std::make_pair( (int)dungeonID, kDungeonClearInfo ) );

		return true;
	}

	return false;
}

bool CX2Unit::UpdateViewEqipPosition()
{
	memset( m_ViewEqipPosition,		0, sizeof(bool) * EP_END );
	for( int i =0; i < (int)m_ViewEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_ViewEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			return false;

		m_ViewEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}

	return true;
}

bool CX2Unit::UpdateFashionEqipPosition()
{
	memset( m_FashionEqipPosition,	0, sizeof(bool) * EP_END );
	for( int i =0; i < (int)m_FashionEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_FashionEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			return false;

		m_FashionEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}

	return true;
}

bool CX2Unit::UpdateNormalEqipPosition()
{
	memset( m_NormalEqipPosition,	0, sizeof(bool) * EP_END );
	for( int i =0; i < (int)m_NormalEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_NormalEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			return false;

		m_NormalEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}

	return true;
}

void CX2Unit::UpdateEqipPosition( CX2Item* pItem, bool bAdd )
{
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		m_FashionEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] = bAdd;
	}
	else
	{
		m_NormalEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] = bAdd;
	}
}

bool CX2Unit::EqipConflict( CX2Item* pItem )
{
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{		
		if( m_FashionEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] == true )
		{
			//충돌!
			return true;
		}
	}
	else
	{
		if( m_NormalEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] == true )
		{
			//충돌!
			return true;
		}
	}
	return false;
}

bool CX2Unit::UpdateViewEqip()
{
	m_ViewEqipItemUIDList.resize(0);
	SetSumOfItemLevel( 0 );	// 초기화

	for( int i =0; i < (int)m_NormalEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_NormalEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			continue;

		AddItemLevel( pEqipItem );

		if( m_FashionEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] == true )
		{
			//충돌!
			continue;
		}

			

		m_ViewEqipItemUIDList.push_back( pEqipItem->GetUID() );
	}

	SetSumOfItemLevel( GetSumOfItemLevel() / 8 );

	for( int i =0; i < (int)m_FashionEqipItemUIDList.size(); i++ )
	{
		m_ViewEqipItemUIDList.push_back( m_FashionEqipItemUIDList[i] );
	}
	return true;
}

//{{ kimhc // 2011.1.14 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
// 

/** @function : UpdateEnchantStatFromPassiveSkill
	@brief : 패시브 스킬에의한 속성 스탯 업데이트
*/
void CX2Unit::UpdateEnchantStatFromPassiveSkill()
{
	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	int iSkillLevel = 0;
	m_EnchantStatFromPassiveSkill.Init();

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A );
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0)
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A );
		if( pSkillTemplet != NULL )
		{
			m_EnchantStatFromPassiveSkill.m_fDefBlaze	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefWater	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WATER_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefNature	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_NATURE_DEFENCE_ABS, iSkillLevel );
		}
#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A, iSkillLevel );
		if( pSkillTemplet != NULL )
		{
			m_EnchantStatFromPassiveSkill.m_fDefBlaze	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS );
			m_EnchantStatFromPassiveSkill.m_fDefWater	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WATER_DEFENCE_ABS );
			m_EnchantStatFromPassiveSkill.m_fDefNature	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_NATURE_DEFENCE_ABS );
		}
#endif // UPGRADE_SKILL_SYSTEM_2013
	}

#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B );
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0)
	{
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B );
		if( pSkillTemplet != NULL )
		{
			m_EnchantStatFromPassiveSkill.m_fDefWind	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WIND_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefLight	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_LIGHT_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefDark	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DARK_DEFENCE_ABS, iSkillLevel );
		}
#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B, iSkillLevel );
		if( pSkillTemplet != NULL )
		{
			m_EnchantStatFromPassiveSkill.m_fDefWind	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WIND_DEFENCE_ABS );
			m_EnchantStatFromPassiveSkill.m_fDefLight	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_LIGHT_DEFENCE_ABS );
			m_EnchantStatFromPassiveSkill.m_fDefDark	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DARK_DEFENCE_ABS );
		}
#endif // UPGRADE_SKILL_SYSTEM_2013
	}
#ifdef NEW_CHARACTER_EL
	// 불의 신체
	{
		const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SPK_BODY_OF_FIRE;
		const int iSkillLevel = refUserSkillTree.GetSkillLevel( eSkillID );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
			if( NULL != pSkillTemplet )
			{
				m_EnchantStatFromPassiveSkill.m_fDefBlaze	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS, iSkillLevel );
			}
		}
	}
#endif //NEW_CHARACTER_EL
}
#endif	CHUNG_FIRST_CLASS_CHANGE
//}} kimhc // 2011.1.14 // 청 1차 전직

//{{ kimhc // 실시간 엘소드 중 실시간 장비 및 아바타 교체
#ifdef REAL_TIME_ELSWORD
bool CX2Unit::IsNormalEquipInThePosition( EQIP_POSITION equipPosition ) const
{
	return m_NormalEqipPosition[equipPosition];
}

bool CX2Unit::IsFashionEquipInThePosition( EQIP_POSITION equipPosition ) const
{
	return m_FashionEqipPosition[equipPosition];
}

bool CX2Unit::IsViewEquipInThePosition( EQIP_POSITION equipPosition ) const
{
	return m_ViewEqipPosition[equipPosition];
}

CX2Item* CX2Unit::CreateBasicEquip( CX2Unit::EQIP_POSITION equipPosition )
{
	CX2Item::ItemData kBasicEquipData;

	switch ( equipPosition )
	{
	case CX2Unit::EP_DEFENCE_HAIR:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicHairItemID;
		break;
	case CX2Unit::EP_DEFENCE_FACE:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicFaceItemID;
		break;
	case CX2Unit::EP_DEFENCE_BODY:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicBodyItemID;
		break;
	case CX2Unit::EP_DEFENCE_LEG:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicLegItemID;
		break;
	case CX2Unit::EP_DEFENCE_HAND:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicHandItemID;
		break;
	case CX2Unit::EP_DEFENCE_FOOT:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicFootItemID;
		break;
	case CX2Unit::EP_WEAPON_HAND:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicWeaponItemID;
		break;
	}
	if ( kBasicEquipData.m_ItemID == 0 )		// 위의 상의에 없는 악세사리인 경우
	{
		return NULL;
	}
	kBasicEquipData.m_ItemUID		= 0;
	kBasicEquipData.m_Endurance	= 1;

	return new CX2Item( kBasicEquipData, this );
}

#endif REAL_TIME_ELSWORD
//}} kimhc // 실시간 엘소드 중 실시간 장비 및 아바타 교체

//{{ kimhc // 2010.12.20 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
/** @function : IsBasicEquip
	@brief : 아무것도 장작하지 않은 기본 장비 인지를 조사함
	@param : const int iItemID_ (기본장비인지 알고 싶어하는 아이템의 ID)
	@return : bool (기본장비면 true)
*/
bool CX2Unit::IsBasicEquip( const int iItemID_ )
{
	const CX2Unit::UnitTemplet *pUnitTemplet = GetUnitTemplet();

	if ( iItemID_ == pUnitTemplet->m_BasicWeaponItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicHairItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicFaceItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicBodyItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicLegItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicHandItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicFootItemID )
		return true;
	else
		return false;	
}
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.20 //  2010-12-23 New Character CHUNG


// 클리어한 던전중에 제일 레벨이 높은 던전 ID를 return 
int CX2Unit::GetLastClearDungeonID()
{
	if( NULL == g_pData->GetDungeonManager() )
		return -1;

	int iRequireLevel = 0;
	int iDungeonID = -1;

	const int iMyLevel = GetUnitData().m_Level;

	std::map< int, KDungeonClearInfo >::iterator it;
	for( it = m_mapDungeonClear.begin(); it != m_mapDungeonClear.end(); it++ )
	{
		const CX2Dungeon::DungeonData* pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID) it->first );
		if( NULL == pDungeonData )
			continue;
		
		// 난이도는 보통만 
		if( CX2Dungeon::DL_NORMAL != pDungeonData->m_eDifficulty )
			continue;

		if( pDungeonData->m_DungeonID == SEnum::DI_BATTLE_SHIP_VELDER ||
			pDungeonData->m_DungeonID == SEnum::DI_BATTLE_SHIP_HAMEL )
			continue;

		const int iAdequateLevelToPlay = pDungeonData->m_MinLevel - 2;

		// 캐릭터의 레벨이 적정플레이레벨 보다 같거나 높고, 클리어한 던전들 중에 제일 높으면
		if ( iMyLevel >= iAdequateLevelToPlay && iAdequateLevelToPlay > iRequireLevel )
		{
			iDungeonID		= pDungeonData->m_DungeonID;
			iRequireLevel	= pDungeonData->m_MinLevel;
		}
	}

	return iDungeonID;
}

bool CX2Unit::IsClearDungeon( int iDungeonID )
{
	std::map< int, KDungeonClearInfo >::iterator iD = m_mapDungeonClear.find( iDungeonID );
	if ( iD != m_mapDungeonClear.end() )
		return true;

	return false;
}

bool CX2Unit::IsClearDungeon( vector<int> &vecDungeonId )
{
	if(vecDungeonId.size() <= 0)
		return true;

	for(UINT i=0; i<vecDungeonId.size(); ++i)
	{
		std::map< int, KDungeonClearInfo >::iterator iD = m_mapDungeonClear.find( vecDungeonId[i] );
		if ( iD == m_mapDungeonClear.end() )
		{
			return false;
		}
	}	

	return true;
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_MAX_MP
/// June 3234b9d0 ( CX2Unit::ResetMaxMp( float, int ) ): SA_MAX_MP_ABS ( 0x15 ) of 19 passives, each read as
/// GetSkillLevel( id, true, 0, true ) ( 32890620( id, 1, 0, 1 ) on +0x494 ), GetSkillTemplet( id ) ( 32206450( id, 1 ) ) and
/// GetSkillAbilityValue( SA_MAX_MP_ABS, level, false ) ( 32206e50( 0x15, level, 0, 0.0 ) ), summed in June's order with the equipment
/// term ( 32869f80, inventory +0x2a4 ) after the 7th skill. June skips that term in XS_PVE_GAME / XS_PVE_RESULT ( 31d717d0: state
/// 23 / 24 ); March has no PVE state, so it is always summed. 17 of the 19 ids are June-only RNSI_* names ( June Enum.lua );
/// March's 2402 / 5402 / 5410 / 7402 are not in June's skill data any more.
float CX2Unit::ResetMaxMp( const float fMaxMp_, const int iEquipGroup_ )
{
	static const int s_aMaxMpSkillBeforeEquip[] =
	{
		22401,  /// RNSI_P_AEM_UNLIMIT_MANA
		25405,  /// RNSI_P_EAT_QUEENS_POTENTIAL
		25414,  /// RNSI_P_ECB_ADVANCED_EL_ENERGY_REACTOR
		27400,  /// RNSI_P_ALH_FILLED_POWER
		2411,   /// SI_P_AVP_MAGIC_AMPLIFICATION
		2415,   /// SI_P_ADW_SPIRIT_ACCELERATION
		27410,  /// RNSI_P_ALS_FILLED_POWER
	};
	static const int s_aMaxMpSkillAfterEquip[] =
	{
		30402,  /// RNSI_P_LLC_COMMUNION_EXPAND
		29406,  /// RNSI_P_AAT_HIDDEN_MEMORY
		29410,  /// RNSI_P_ATT_NASOD_ARMOR_MODE
		31415,  /// RNSI_P_ROM_MECA_GENERATOR
		23401,  /// RNSI_P_RWS_FRIENDSHIP_OF_NATURE
		23406,  /// RNSI_P_RGA_COMMUNE_OF_NATURE
		23408,  /// RNSI_P_RTR_HUNTERS_ABILITY
		30412,  /// RNSI_P_LDB_COMMUNION_ABSORB
		22408,  /// RNSI_P_ABM_FITNESS
		32401,  /// RNSI_P_ALE_SEQUENCE_TRACE
		32408,  /// RNSI_P_AEE_INNER_GROW
		32414,  /// RNSI_P_AAP_AWAKENING
	};

	float fResultMaxMp = fMaxMp_;

	for ( int i = 0; i < static_cast<int>( ARRAY_SIZE( s_aMaxMpSkillBeforeEquip ) ); ++i )
		fResultMaxMp += GetMaxMpSkillValue( s_aMaxMpSkillBeforeEquip[i] );

	/// 31d717d0 ( June PVE state ) is never true in March
	fResultMaxMp += GetInventory().GetAddMaxMPValue( iEquipGroup_ );

	for ( int i = 0; i < static_cast<int>( ARRAY_SIZE( s_aMaxMpSkillAfterEquip ) ); ++i )
		fResultMaxMp += GetMaxMpSkillValue( s_aMaxMpSkillAfterEquip[i] );

	return fResultMaxMp;
}

/// One block of June 3234b9d0: 0 unless the skill is learned ( base level > 0 ) and has a templet.
float CX2Unit::GetMaxMpSkillValue( const int iSkillID_ ) const
{
	const CX2SkillTree::SKILL_ID eSkillID = static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ );
	const int iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID, true, 0, true );
	if ( iSkillLevel <= 0 )
		return 0.f;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
	if ( NULL == pSkillTemplet )
		return 0.f;

	return pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel, false );
}
#else SERV_IRUHADEV_JUNE_SKILL_MAX_MP
float CX2Unit::ResetMaxMp( const float fMaxMp_ )
{
	float fResultMaxMp = fMaxMp_;
	
	// 끝없는 마력
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	int iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AHM_UNLIMIT_MANA, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AHM_UNLIMIT_MANA );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}
#else //UPGRADE_SKILL_SYSTEM_2013
	int iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_UNLIMIT_MANA );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_UNLIMIT_MANA, iSkillLevel );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS );
			fResultMaxMp += maxMPIncrement;
		}
	}
#endif //UPGRADE_SKILL_SYSTEM_2013
	
	// 여왕의 잠재력 적용해주기(MAX MP 추가)
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL );
#endif // UPGRADE_SKILL_SYSTEM_2013

	if( iSkillLevel > 0 )
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL, iSkillLevel );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS );
			fResultMaxMp += maxMPIncrement;
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}

#ifdef EVE_ELECTRA
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR, true );
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR );
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0 )
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS,iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR, iSkillLevel );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS );
			fResultMaxMp += maxMPIncrement;
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}
#endif EVE_ELECTRA

#ifdef ARA_CHANGE_CLASS_FIRST
#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_FILLED_POWER, true ); // 아라 1차 전직 소선 - 충만한 힘
#else // UPGRADE_SKILL_SYSTEM_2013
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_FILLED_POWER ); // 아라 1차 전직 소선 - 충만한 힘
#endif // UPGRADE_SKILL_SYSTEM_2013
	
	if( iSkillLevel > 0 )
	{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ALH_FILLED_POWER );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	#else // UPGRADE_SKILL_SYSTEM_2013
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ALH_FILLED_POWER, iSkillLevel );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS );
			fResultMaxMp += maxMPIncrement;
		}
	#endif // UPGRADE_SKILL_SYSTEM_2013
	}
#endif //ARA_CHANGE_CLASS_FIRST

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
	// 마력 증폭
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}
#endif //UPGRADE_SKILL_SYSTEM_2013

#ifdef BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편
	// 기력 가속 패시브 적용 시, 최대 MP 값 증가 추가
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADW_SPIRIT_ACCELERATION, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ADW_SPIRIT_ACCELERATION );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}

#endif // BALANCE_PATCH_20131107					// 김종훈 / 13-10-16, 2013년 후반기 밸런스 개편

	//{{ kimhc // 2009-10-19 // 최대 MP 증가 값 추가
	{
		const float maxMPIncrement = GetInventory().GetAddMaxMPValue();
		fResultMaxMp += maxMPIncrement;
	}
	//}} kimhc // 2009-10-19 // 최대 MP 증가 값 추가


	return fResultMaxMp;
}
#endif SERV_IRUHADEV_JUNE_SKILL_MAX_MP

#ifdef SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET
/// June skill system Phase S7k: CX2Unit's own socket aggregate. June CX2Unit +0xe1c is a CX2SocketItem::SocketData ( new 0x468, ctor
/// 32091080, vtable 33478a98; unit type 10, Lu/Ciel, gets 0x488 32091730: no March class ), made by CX2Unit::Init 32363ca0 and filled
/// from this unit's data by vtable 3348ffe4 slot 1 ( 32365d30 = JuneInitSocketData ) through the unit-side SumSocketOption 32364790.
/// +0xe20 ( summon list ) and +0xe30 ( the title's options ) are two more CX2Unit vectors only that fill writes; they live here too.
struct CX2Unit::SocketAggregate
{
	struct SummonNpc					/// the 12-byte entry 32364790 @32364c3d..32364c6c pushes ( 3210aae0 )
	{
		int		m_iNpcId;				/// socket +0x240
		float	m_fSummonRate;			/// socket +0x244
		float	m_fSummonNpcCoolTime;	/// socket +0x248
	};

	CX2SocketItem::SocketData	m_kSocketData;			/// +0xe1c
	std::vector<SummonNpc>		m_vecSummonNpcSocket;	/// +0xe20: June never clears it ( Init, the fill and the sum leave it ): JUNE_BUGS
	std::vector<int>			m_vecTitleSocketOption;	/// +0xe30: the title's options, copied in before they are summed ( 32853ff0 )
};

CX2Unit::SocketAggregateHolder::SocketAggregateHolder()
	: m_p( NULL )
{
}

CX2Unit::SocketAggregateHolder::SocketAggregateHolder( const SocketAggregateHolder& rhs_ )
	: m_p( NULL == rhs_.m_p ? NULL : new SocketAggregate( *rhs_.m_p ) )
{
}

CX2Unit::SocketAggregateHolder& CX2Unit::SocketAggregateHolder::operator=( const SocketAggregateHolder& rhs_ )
{
	if ( this != &rhs_ )
	{
		SAFE_DELETE( m_p );
		if ( NULL != rhs_.m_p )
			m_p = new SocketAggregate( *rhs_.m_p );
	}
	return *this;
}

CX2Unit::SocketAggregateHolder::~SocketAggregateHolder()
{
	SAFE_DELETE( m_p );
}

/// June 32363ca0 @32363cb9..32363d53: the old SocketData is deleted ( vslot 0( 1 ) ) and a new one made; the unit vectors +0xe20 /
/// +0xe30 are not touched ( Init runs from the ctors, where they are empty ).
void CX2Unit::SocketAggregateHolder::Recreate()
{
	if ( NULL == m_p )
		m_p = new SocketAggregate;
	else
		m_p->m_kSocketData = CX2SocketItem::SocketData();
}

/** @function : JuneSumSocketOption
	@brief : June 32364790 ( CX2Unit, ret 0xc; args vector, float, int ): the unit-side SumSocketOption, into the aggregate +0xe1c.
		Per id: templet 32715320 ( socket item g_pData +0x280 ); a PVP_ONLY ( +0x191 ) or DUNGEON_ONLY ( +0x190 ) socket is skipped whole
		( the user side 3204f190 gates them by game type instead ); the unit type is 31e4c370 ( the +0xaf0 templet's, 0 without one ).
		The float is the item's magic attack for equipment ( 32365d30 @323660b3: [esp+0x30] = 328842f0's +0x20 ), 1.0 otherwise; June
		reads it only in the +0x324 block, which March does not have ( see there ). Not in the unit version ( user-side only ): the
		enchant attack / resist, the skill-slot pass 31fe14f0, user vslot 485, InitSocketEffectEx, the old EDT copy, the monster damage /
		evade maps, the immune entry, socket 2000145 and the 4119 / 4120 summon gate.
*/
void CX2Unit::SumSocketOption( const std::vector<int>& vecSocketOption_, const float fAtkMagic_, const int iSocketLevel_ )
{
	/// March safety: June calls 32715320 on g_pData +0x280 unchecked, and its Init always made the aggregate
	if ( NULL == m_kSocketAggregate.m_p || NULL == g_pData || NULL == g_pData->GetSocketItem() )
		return;

	SocketAggregate& kSocketAggregate = *m_kSocketAggregate.m_p;
	CX2SocketItem::SocketData& kAggregate = kSocketAggregate.m_kSocketData;

	for ( UINT i = 0; i < vecSocketOption_.size(); ++i )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( vecSocketOption_[i] );
		if ( NULL == pSocketData || true == pSocketData->m_bPVPOnly || true == pSocketData->m_bDungeonOnly )
			continue;
		const CX2SocketItem::SocketData& kSocketData = *pSocketData;

		const CX2Unit::UNIT_TYPE eUnitType = ( NULL != GetUnitTemplet() ) ? GetType() : CX2Unit::UT_NONE;

		/// @32364844..3236498a: rank-linked ( +0x294 ) -> a scaled copy at the PvP table's rate for this unit's rank ( 31edfe70( +0x114 ) );
		/// PVE-rank-linked ( +0x58 ) -> the PVE table ( 31edfeb0( 32070600( 1 ), bForMP ); March has no PVE rank: key 0, as the user side );
		/// else the socket itself. Each is ConvertAndAdd'ed ( 32080670 ) with this level and this unit's type.
		if ( kSocketData.m_bIsRankLinkedStat || kSocketData.m_bIsPVERankLinkedStat )
		{
			float fRankRate = 0.f;
			if ( true == kSocketData.m_bIsRankLinkedStat )
			{
				fRankRate = g_pData->GetPvpReferData( static_cast<int>( static_cast<char>( m_UnitData.m_cRank ) ) );
			}
			else
			{
				const bool bForMP = ( 0.f < kSocketData.m_fMPIncByAttack && kSocketData.m_fMPIncByAttack == kSocketData.m_fMPIncByAttacked );
				fRankRate = g_pData->GetPveReferData( 0, bForMP );
			}

			CX2SocketItem::SocketData kScaledSocketData;
			kScaledSocketData.SetRankLinkedScaled( kSocketData, fRankRate );
#ifdef SERV_9TH_NEW_CHARACTER
			kAggregate.ConvertAndAdd( kScaledSocketData, iSocketLevel_, eUnitType );
#else SERV_9TH_NEW_CHARACTER
			kAggregate.ConvertAndAdd( kScaledSocketData, iSocketLevel_ );
#endif SERV_9TH_NEW_CHARACTER
		}
		else
		{
#ifdef SERV_9TH_NEW_CHARACTER
			kAggregate.ConvertAndAdd( kSocketData, iSocketLevel_, eUnitType );
#else SERV_9TH_NEW_CHARACTER
			kAggregate.ConvertAndAdd( kSocketData, iSocketLevel_ );
#endif SERV_9TH_NEW_CHARACTER
		}

		/// @3236498a: the templet's skill power map ( +0x48 ) is added key by key ( 3207a5e0 = operator[] on the aggregate's +0x48 )
		for ( std::map<int, float>::const_iterator mit = kSocketData.m_mapSkillAdditionalPowerRate.begin();
			mit != kSocketData.m_mapSkillAdditionalPowerRate.end(); ++mit )
		{
			kAggregate.m_mapSkillAdditionalPowerRate[ mit->first ] += mit->second;
		}

		/// @323649f7: the skill level-up ids ( +0x38 ) go to the end ( 3207d660 )
		if ( false == kSocketData.m_vecSkillLevelUpID.empty() )
		{
			kAggregate.m_vecSkillLevelUpID.insert( kAggregate.m_vecSkillLevelUpID.end(), kSocketData.m_vecSkillLevelUpID.begin(), kSocketData.m_vecSkillLevelUpID.end() );
		}
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		/// @32364a1e: each level-up group ( +0x28 ) by this unit's class ( +0x38 ), every result pushed ( 32719be0, 3207b4c0 )
		if ( false == kSocketData.m_vecSkillLevelUpGroupID.empty() )
		{
			BOOST_FOREACH( int iGroupID, kSocketData.m_vecSkillLevelUpGroupID )
			{
#ifdef UPGRADE_SKILL_SYSTEM_2013
				vector<SkillLevelUpIDAndNum> vecSkillLevelUPIDAndNumList;
				g_pData->GetSocketItem()->GetSkillIDByUnicClassAndGropID( vecSkillLevelUPIDAndNumList, m_UnitData.m_UnitClass, iGroupID );
				BOOST_FOREACH( const SkillLevelUpIDAndNum& kSkillLevelUPIDAndNum, vecSkillLevelUPIDAndNumList )
				{
					kAggregate.m_vecSkillLevelUpID.push_back( kSkillLevelUPIDAndNum );
				}
#else UPGRADE_SKILL_SYSTEM_2013
				CX2SkillTree::SKILL_ID eSkillID = g_pData->GetSocketItem()->GetSkillIDByUnicClassAndGropID( m_UnitData.m_UnitClass, iGroupID );
				if ( CX2SkillTree::SI_NONE != eSkillID )
					kAggregate.m_vecSkillLevelUpID.push_back( eSkillID );
#endif UPGRADE_SKILL_SYSTEM_2013
			}
		}
#endif SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET_EDT
		/// @32364af3..32364bb2: the templet's +0x324 is March's single extra damage ( m_SocketExtraDamage: the EXTRA_DAMAGE table's single
		/// types, loader 32726850 @3272926f case 0 ). When its type is set it is copied into the aggregate's ( +0x32c, 3208c500 ), and the
		/// aggregate's damage per second ( +0x344 ) becomes the templet's ( +0x334 ) x fAtkMagic_. The unit side has no 0.0 branch: the user
		/// side 3204f190 @3204f61f takes the unit's magic attack when fAtkMagic is 0, as March's CX2GUUser::SumSocketOption does.
		if ( CX2DamageManager::EDT_NONE != kSocketData.m_SocketExtraDamage.m_ExtraDamageType )
		{
			kAggregate.m_SocketExtraDamage.Copy( kSocketData.m_SocketExtraDamage );
			kAggregate.m_SocketExtraDamage.m_DamagePerSec = fAtkMagic_ * kSocketData.m_SocketExtraDamage.m_DamagePerSec;
		}
#else SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET_EDT
		/// @32364af3..32364bb2 not ported: when the templet's +0x324 is set, its 0x1c-byte block ( an int, two bytes, a refcounted pointer,
		/// two floats ) is copied into the aggregate's +0x32c ( 3208c500 ) and its second float x fAtkMagic_ into the aggregate's +0x344.
		/// March's SocketData has no such field and its loader no such key ( the user side 3204f190 @3204f5fc has the same block ).
#endif SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET_EDT

		/// @32364bb2 / @32364beb: the multiple extra damage ( +0x374 ) and the second list ( +0x384 ) are inserted at the front ( 3208f840 )
		if ( false == kSocketData.m_MultipleSocketExtraDamage.empty() )
		{
			kAggregate.m_MultipleSocketExtraDamage.insert( kAggregate.m_MultipleSocketExtraDamage.begin(),
				kSocketData.m_MultipleSocketExtraDamage.begin(), kSocketData.m_MultipleSocketExtraDamage.end() );
		}
		if ( false == kSocketData.m_vecSecondSocketExtraDamage.empty() )
		{
			kAggregate.m_vecSecondSocketExtraDamage.insert( kAggregate.m_vecSecondSocketExtraDamage.begin(),
				kSocketData.m_vecSecondSocketExtraDamage.begin(), kSocketData.m_vecSecondSocketExtraDamage.end() );
		}

		/// @32364c24: a summoning socket ( id > 0, rate > 0 ) goes to this unit's list +0xe20 ( no 4119 / 4120 gate here )
		if ( 0 < kSocketData.m_iSummonNpcId && 0.f < kSocketData.m_fSummonNpcRate )
		{
			SocketAggregate::SummonNpc kSummonNpc;
			kSummonNpc.m_iNpcId				= kSocketData.m_iSummonNpcId;
			kSummonNpc.m_fSummonRate		= kSocketData.m_fSummonNpcRate;
			kSummonNpc.m_fSummonNpcCoolTime	= kSocketData.m_fSummonNpcCoolTime;
			kSocketAggregate.m_vecSummonNpcSocket.push_back( kSummonNpc );
		}

#ifdef HAMEL_SECRET_DUNGEON
		/// @32364c71: a hyper-mode socket ( +0x27c != -1 ) adds the user side's 24-byte entry to the aggregate's +0x280 ( 3208fa40 ), with
		/// socket ID -1 ( 0xffffffff @32364cab ) where the user side writes the socket's own
		if ( CX2SocketItem::HMET_NONE != kSocketData.m_eHyperModeEffectiveType )
		{
			CX2SocketItem::POSSIBLE_USE_PLACE ePossibleUsePlace = CX2SocketItem::PUP_ALL;
			if ( true == kSocketData.m_bDungeonOnly )
				ePossibleUsePlace = CX2SocketItem::PUP_DUNGEON_ONLY;
			else if ( true == kSocketData.m_bPVPOnly )
				ePossibleUsePlace = CX2SocketItem::PUP_PVP_ONLY;

			CX2SocketItem::InfoByUseHyperMode sInfoByUseHyperMode;
			sInfoByUseHyperMode.m_ePossibleUsePlace			= ePossibleUsePlace;
			sInfoByUseHyperMode.m_eHyperModeEffectiveType	= kSocketData.m_eHyperModeEffectiveType;
			sInfoByUseHyperMode.m_fEffectiveValue			= kSocketData.m_fEffectiveValue;
#ifdef ADJUST_SECRET_ITEM_OPTION
			sInfoByUseHyperMode.m_fCoolTime					= kSocketData.m_fIntervalTime;
#endif ADJUST_SECRET_ITEM_OPTION
			if ( NULL != kSocketData.m_ptrBuffFactorCustomUse )
				sInfoByUseHyperMode.m_pBuffFactorPtr = kSocketData.m_ptrBuffFactorCustomUse;
			sInfoByUseHyperMode.m_fIntervalTime				= kSocketData.m_fIntervalTime;	/// +0x1f8
			sInfoByUseHyperMode.m_iSocketID				= -1;
			kAggregate.m_vecInfoByUseHyperMode.push_back( sInfoByUseHyperMode );
		}
#endif HAMEL_SECRET_DUNGEON

		/// @32364d40..32365256: the cool-time cuts go to this unit's slot map +0xe78 ( 32355d70's insert-or-add, inlined ): active / 1st /
		/// 2nd / 3rd SA to slots 1-4, then the all-skill cut to slots 1-4 ( the user side also gives it slot 6 )
		AddSocketCoolTimeCut( 1, kSocketData.m_fDecreaseActiveSkillCooltime );
		AddSocketCoolTimeCut( 2, kSocketData.m_fDecrease1stSA_SkillCooltime );
		AddSocketCoolTimeCut( 3, kSocketData.m_fDecrease2ndSA_SkillCooltime );
		AddSocketCoolTimeCut( 4, kSocketData.m_fDecrease3rdSA_SkillCooltime );
		AddSocketCoolTimeCut( 1, kSocketData.m_fAllSkillCooltimeDecreaseRate );
		AddSocketCoolTimeCut( 2, kSocketData.m_fAllSkillCooltimeDecreaseRate );
		AddSocketCoolTimeCut( 3, kSocketData.m_fAllSkillCooltimeDecreaseRate );
		AddSocketCoolTimeCut( 4, kSocketData.m_fAllSkillCooltimeDecreaseRate );
	}
}

/** @function : JuneSumSetItemSocketOption
	@brief : June vtable 3348ffe4 slot 2 323652b0( mode, type-10 aggregate ): the set options. Set ID / parts of group 0 ( 32350cb0( 0 ) ),
		per set 325bb840 ( item manager g_pData +0x34: options, max level ); with options and mode 0 or 2, summed at rate 1.0 and the max
		level, or this unit's level when that is 0. The second group ( 32350cb0( 1 ), modes 1 / 2 ) runs only with the type-10 pointer:
		never in March. 32350cb0 is March's GetSetIDNPartsNum plus three item tests: usable ( 328825c0 ), class ( 31ed6500 ) and
		328823d0 ( an appearance item's linked templet, item +0x80 / +0x84: no March field ). Equipped March items already pass the
		class test; the usable test is the endurance check March has ( REAL_TIME_ELSWORD ), the rest is backlog Stage 16.
*/
void CX2Unit::SumSetItemSocketOption( const int iMode_ )
{
	if ( NULL == g_pData || NULL == g_pData->GetItemManager() )
		return;

	map<int,int> mapSetIDNParts;
	GetSetIDNPartsNum( mapSetIDNParts );

	for ( map<int,int>::const_iterator mItr = mapSetIDNParts.begin(); mItr != mapSetIDNParts.end(); ++mItr )
	{
		vector<int> vecOptions;
		const int iMaxLevel = g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, vecOptions );
		if ( true == vecOptions.empty() )
			continue;

		if ( 0 == iMode_ || 2 == iMode_ )
			SumSocketOption( vecOptions, 1.f, ( 0 < iMaxLevel ) ? iMaxLevel : static_cast<int>( m_UnitData.m_Level ) );
	}
}

/** @function : JuneInitSocketData
	@brief : June vtable 3348ffe4 slot 1 32365d30( mode ): refill the aggregate from this unit's data. Mode 0 / 1 / 2 and the group
		filter 3286a4b0 act only with the type-10 aggregate ( Lu/Ciel, @32365d63 ); without it every source below is summed.
		Order: ReInit, clear +0xe78, equipment, pet aura, riding pet, ( customize ), title, ( party ), set items, premium buffs,
		LimitMaximum( 0 ), ResetIncreaseSkillLevelBySocket.
*/
void CX2Unit::InitSocketData( const int iMode_ )
{
	if ( NULL == m_kSocketAggregate.m_p || NULL == g_pData )
		return;

	SocketAggregate& kSocketAggregate = *m_kSocketAggregate.m_p;

	kSocketAggregate.m_kSocketData = CX2SocketItem::SocketData();	/// aggregate vslot 2 = ReInit 320922a0
	ClearSocketCoolTimeCut();								/// 31d06e90 on +0xe78 @32365d95

	/// @32365d9a: the equipped list ( +0x284 ); an item 3286a230( uid ) with a templet that is usable ( 328825c0 ) and, when it names a
	/// class ( templet +0xd3 ), fits this unit's ( 31ed6500( 31e4c380(), class ) )
	for ( UINT i = 0; i < m_UnitData.m_NowEqipItemUIDList.size(); ++i )
	{
		CX2Item* pItem = GetInventory().GetItem( m_UnitData.m_NowEqipItemUIDList[i] );
		if ( NULL == pItem || NULL == pItem->GetItemTemplet() )
			continue;

		/// 328825c0: June's usable test ( an expired item, a broken one, the item-manager list 0x4d ); March has the endurance part
#ifdef REAL_TIME_ELSWORD
		if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
			continue;
#endif REAL_TIME_ELSWORD

		if ( CX2Unit::UC_NONE != pItem->GetItemTemplet()->GetUnitClass() &&
			false == GetX2UnitClassCompatibility( m_UnitData.m_UnitClass, pItem->GetItemTemplet()->GetUnitClass() ) )
			continue;

		/// 328842f0 @32365e45: the item's stat; its magic attack ( +0x20 ) is the float every sum of this item gets
		const float fAtkMagic = pItem->GetStat().m_fAtkMagic;

		/// @32365e61..32365ec1: the random sockets ( item +0x6c ), then the templet's ( 32867f80 ), in one list
		std::vector<int> vecSocketOptions( pItem->GetItemData().m_vecRandomSocket.begin(), pItem->GetItemData().m_vecRandomSocket.end() );
		std::vector<int> vecNormalSocketOption;
		pItem->GetItemTemplet()->GetSocketOptions( vecNormalSocketOption );
		vecSocketOptions.insert( vecSocketOptions.end(), vecNormalSocketOption.begin(), vecNormalSocketOption.end() );

		/// @32365ec1: the templet's use level ( +0xd1 ) when it has one, else this unit's level ( +0xb4 )
		const int iSocketLevel = ( 0 < pItem->GetItemTemplet()->GetUseLevel() ) ? pItem->GetItemTemplet()->GetUseLevel() : static_cast<int>( m_UnitData.m_Level );

		SumSocketOption( vecSocketOptions, fAtkMagic, iSocketLevel );						/// @323660c3
		SumSocketOption( pItem->GetItemData().m_SocketOption, fAtkMagic, iSocketLevel );	/// @323660d7 ( item +0x5c )
		/// @323660ee / @32366105 ( item +0x8c / +0x9c ) and @323661ad ( item +0xac when its set is complete: 32883300 / 3286a6b0 ): June-only
		/// item socket lists; March's ItemData has neither field, so they are not ported
	}

#ifdef PET_AURA_SKILL
	/// @3236627c: this unit's pet ( +0xd6c, first entry ) with the pet manager ( g_pData +0x328 )
	KPetInfo* pPetInfo = GetPetInfo();
	if ( NULL != g_pData->GetPetManager() && NULL != pPetInfo )
	{
#ifdef PET_DROP_ITEM_PICKUP
		/// @323662c4..32366432: in the dungeon / training / battle field states ( June 0xb / 0x12 / 0x15, the same constants as the user side
		/// 3206e337, whose March form is CX2GUUser::InitSocketData's switch ) and when this is my game unit, the pick-up flag is reset and
		/// then set from the pet ( +0x88 ). The aura below has no state gate here.
		if ( NULL != g_pMain && ( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() || CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() ||
			CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() ) &&
			NULL != g_pX2Game && NULL != g_pX2Game->GetDropItemManager() && NULL != g_pX2Game->GetMyUnit() &&
			GetUID() == g_pX2Game->GetMyUnit()->GetUnitUID() )
		{
			g_pX2Game->GetDropItemManager()->SetSummonItemPickupPet( false );
			g_pX2Game->GetDropItemManager()->SetSummonItemPickupPet( pPetInfo->m_bAutoLooting );
		}
#endif PET_DROP_ITEM_PICKUP

#ifdef SERV_PETID_DATA_TYPE_CHANGE
		CX2PetManager::PetTemplet* pPetTemplet = g_pData->GetPetManager()->GetPetTemplet( static_cast<CX2PetManager::PET_UNIT_ID>( pPetInfo->m_iPetID ) );
#else SERV_PETID_DATA_TYPE_CHANGE
		CX2PetManager::PetTemplet* pPetTemplet = g_pData->GetPetManager()->GetPetTemplet( static_cast<CX2PetManager::PET_UNIT_ID>( pPetInfo->m_cPetID ) );
#endif SERV_PETID_DATA_TYPE_CHANGE
		/// @32366442..32366475: satiety / 4800 ( 334b5048 ) >= 0.4 ( 334b504c ), then m_vecPetStatus[ evolution ] steps of aura options
		/// ( templet +0xfc, 16 bytes each ), each at rate 1.0 and this unit's level
		if ( NULL != pPetTemplet &&
			static_cast<float>( pPetInfo->m_sSatiety ) / CX2PetManager::MAX_OF_SATIETY >= CX2PetManager::SATIETY_RATIO_TO_BE_ABLE_TO_APPLY_AURA &&
			static_cast<UINT>( pPetInfo->m_cEvolutionStep ) < pPetTemplet->m_vecPetStatus.size() )	/// March safety: June indexes unchecked
		{
			const int iStep = pPetTemplet->m_vecPetStatus[ pPetInfo->m_cEvolutionStep ];
			for ( int i = 0; i < iStep && i < static_cast<int>( ARRAY_SIZE( pPetTemplet->m_vecAuraSkillOption ) ); ++i )	/// March safety
				SumSocketOption( pPetTemplet->m_vecAuraSkillOption[ i ], 1.f, static_cast<int>( m_UnitData.m_Level ) );
		}
	}
#endif PET_AURA_SKILL

#ifdef RIDING_SYSTEM
	/// @323664c8: riding ( 321f8a40 = GetRidingOn ), the manager's current riding pet ( 321f5b10 on +0x40 ) -> its socket list ( +0xec ),
	/// at rate 1.0 and this unit's level. March keeps that list on the riding pet's templet, where CX2GUUser::InitSocketData reads it.
	if ( NULL != CX2RidingPetManager::GetInstance() && true == CX2RidingPetManager::GetInstance()->GetRidingOn() )
	{
		CX2RidingPet* pRidingPet = CX2RidingPetManager::GetInstance()->GetMyRidingPet();
		if ( NULL != pRidingPet )
		{
			CX2RidingPetManager::RidingPetTemplet* pRidingPetTemplet =
				CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>( pRidingPet->GetInfo().m_Id ) );
			if ( NULL != pRidingPetTemplet )
				SumSocketOption( pRidingPetTemplet->m_vecSocketOption, 1.f, static_cast<int>( m_UnitData.m_Level ) );
		}
	}
#endif RIDING_SYSTEM

	/// @32366528..323666dc not ported: with the item manager, the first entry's +4 of every node of the customize map +0xa44 ( June's
	/// applied customize, Effect C9d / C9g ) is summed; March has no such system.

	/// @32366720: the title ( +0x9a0 ) in the title manager ( g_pData +0x28c ): its options ( +0xa0 ) copied to +0xe30, then summed
	if ( NULL != g_pData->GetTitleManager() && NULL != g_pData->GetTitleManager()->GetTitleInfo( m_UnitData.m_iTitleId ) )
	{
		kSocketAggregate.m_vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( m_UnitData.m_iTitleId )->m_vecSocketOption;
		SumSocketOption( kSocketAggregate.m_vecTitleSocketOption, 1.f, static_cast<int>( m_UnitData.m_Level ) );
	}

#ifdef SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET
	/// @323667bd..32366c26: the room members' shared sockets ( 31ed40c0 with this unit's UID +0x28 / +0x2c, then the shared pass ), each
	/// at its source's rate and level
	{
		std::vector<SocketOptionSource> vecShared;
		GetPartyShareSocketOptions( GetUID(), vecShared );
		for ( UINT i = 0; i < vecShared.size(); ++i )
			SumSocketOption( vecShared[i].m_vecSocketOption, vecShared[i].m_fAtkMagic, vecShared[i].m_iSocketLevel );
	}
#else SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET
	/// @323667bd..32366c26 not ported: party members' shared sockets ( 31ed40c0 through g_pX2Game, ids 2000139-2000141 / 2000144 /
	/// 2000145 by 31d70d30, deduplicated ); the user side 3206df3e has the same pass and March has neither.
#endif SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET

	SumSetItemSocketOption( iMode_ );					/// this vslot 2 @32366c3f

#ifdef BUFF_TEMPLET_SYSTEM
	/// @32366c4d: the premium buffs ( 326c67f0 on +0x9a4 ), at rate 1.0 and this unit's level
	if ( NULL != g_pData->GetPremiumBuffManager() )
	{
		vector<int> vecBuffSocketOptions;
		g_pData->GetPremiumBuffManager()->GetSocketOptions( m_UnitData.GetPremiumBuffList(), vecBuffSocketOptions );
		if ( false == vecBuffSocketOptions.empty() )
			SumSocketOption( vecBuffSocketOptions, 1.f, static_cast<int>( m_UnitData.m_Level ) );
	}
#endif BUFF_TEMPLET_SYSTEM

	kSocketAggregate.m_kSocketData.LimitMaximum( false );		/// 320804f0( 0 ) @32366ca9

#ifdef UPGRADE_SKILL_SYSTEM_2013
	/// 3235a080( 31d717e0() ) @32366cbc: June's extra argument ( my select unit's +0xe40 ) has no March parameter
	ResetIncreaseSkillLevelBySocket();
#endif UPGRADE_SKILL_SYSTEM_2013
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET
/** @function : JuneGetPartySocketOptionSources
	@brief : June 31ed40c0 ( no this, ret 0x10; args the unit UID, &mine, &others ). Needs the room ( g_pX2Room 33775868 ) and the game
		( 31aad500( 3372c0fc ) ). Every room slot that is not empty or closed ( 31ed23a0 on the room: slot state +0xc != 1 / 2, UID +0x18;
		no NPC or unit test ) is looked up as a user ( game vslot 0xa0 ) and its unit ( +0x867c ). Each source of that unit goes to mine
		when the unit's UID ( +0x28 ) is iUnitUID_, else to others ( @31ed4223 ):
		- per set ( 32350b80: set ID -> equipped parts ) with options ( 325bb840 ): rate 1.0, the set's max level, or the unit's level
		  ( +0xb4 ) when that is 0;
		- per equipped item ( +0x284, 3286a230 ) with a templet, not a broken endurance item ( +0xc == 1 and +0x10 <= 0 ) and, when the
		  templet names a class ( +0xd3 ), of a fitting class ( 31e4c380 / 31ed6500 ): the random sockets ( +0x6c ) + the templet's
		  ( 32867f80 ), then the item's own ( +0x5c ), both at the item's magic attack ( 328842f0 +0x20 ) and the templet's use level
		  ( +0xd1 ) or the unit's. Unlike the unit fill: no full usable test ( 328825c0 ), and no item +0x8c / +0x9c / +0xac.
		32350b80 is March's GetSetIDNPartsNum plus June's usable / class / 328823d0 tests and the group 3286a3e0( 3234d8d0() ), as the
		S7k set sum reads it ( JuneSumSetItemSocketOption ).
*/
void CX2Unit::GetPartySocketOptionSources( const UidType iUnitUID_, std::vector<SocketOptionSource>& vecMine_,
	std::vector<SocketOptionSource>& vecOthers_ )
{
	if ( NULL == g_pX2Room || NULL == g_pX2Game )
		return;

	/// 31ed23a0: the UIDs of the slots that are neither SS_EMPTY ( 1 ) nor SS_CLOSE ( 2 )
	std::vector<UidType> vecMemberUID;
	for ( int iSlot = 0; iSlot < g_pX2Room->GetSlotNum(); ++iSlot )
	{
		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( iSlot );
		if ( NULL != pSlotData && CX2Room::SS_EMPTY != pSlotData->GetSlotState() && CX2Room::SS_CLOSE != pSlotData->GetSlotState() )
			vecMemberUID.push_back( pSlotData->m_UnitUID );
	}

	for ( UINT iMember = 0; iMember < vecMemberUID.size(); ++iMember )
	{
		CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( vecMemberUID[ iMember ] );
		if ( NULL == pUser || NULL == pUser->GetUnit() )
			continue;

		CX2Unit* pUnit = pUser->GetUnit();
		std::vector<SocketOptionSource>& vecTarget = ( iUnitUID_ == pUnit->GetUID() ) ? vecMine_ : vecOthers_;
		const int iUnitLevel = static_cast<int>( pUnit->GetUnitData().m_Level );

		/// @31ed421e..31ed4377: the sets
		if ( NULL != g_pData && NULL != g_pData->GetItemManager() )
		{
			map<int,int> mapSetIDNParts;
			pUnit->GetSetIDNPartsNum( mapSetIDNParts );
			for ( map<int,int>::const_iterator mItr = mapSetIDNParts.begin(); mItr != mapSetIDNParts.end(); ++mItr )
			{
				SocketOptionSource kSource;
				const int iMaxLevel = g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, kSource.m_vecSocketOption );
				if ( true == kSource.m_vecSocketOption.empty() )
					continue;
				kSource.m_fAtkMagic		= 1.f;
				kSource.m_iSocketLevel	= ( 0 < iMaxLevel ) ? iMaxLevel : iUnitLevel;
				vecTarget.push_back( kSource );
			}
		}

		/// @31ed43a6..31ed45d3: the equipped items
		for ( UINT i = 0; i < pUnit->GetUnitData().m_NowEqipItemUIDList.size(); ++i )
		{
			CX2Item* pItem = pUnit->GetInventory().GetItem( pUnit->GetUnitData().m_NowEqipItemUIDList[i] );
			if ( NULL == pItem || NULL == pItem->GetItemTemplet() )
				continue;

#ifdef REAL_TIME_ELSWORD
			if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
				continue;
#endif REAL_TIME_ELSWORD

			if ( CX2Unit::UC_NONE != pItem->GetItemTemplet()->GetUnitClass() &&
				false == GetX2UnitClassCompatibility( pUnit->GetUnitData().m_UnitClass, pItem->GetItemTemplet()->GetUnitClass() ) )
				continue;

			SocketOptionSource kSource;
			kSource.m_fAtkMagic		= pItem->GetStat().m_fAtkMagic;
			kSource.m_iSocketLevel	= ( 0 < pItem->GetItemTemplet()->GetUseLevel() ) ? pItem->GetItemTemplet()->GetUseLevel() : iUnitLevel;

			/// @31ed446c / @31ed44ad: the random sockets, then the templet's, in one list
			kSource.m_vecSocketOption.assign( pItem->GetItemData().m_vecRandomSocket.begin(), pItem->GetItemData().m_vecRandomSocket.end() );
			std::vector<int> vecNormalSocketOption;
			pItem->GetItemTemplet()->GetSocketOptions( vecNormalSocketOption );
			kSource.m_vecSocketOption.insert( kSource.m_vecSocketOption.end(), vecNormalSocketOption.begin(), vecNormalSocketOption.end() );
			vecTarget.push_back( kSource );

			/// @31ed451e: the item's own sockets ( +0x5c ), same rate and level
			kSource.m_vecSocketOption = pItem->GetItemData().m_SocketOption;
			vecTarget.push_back( kSource );
		}
	}
}

/** @function : JuneIsPartyShareSocketID
	@brief : June 31d70d30 ( a CX2Main method that ignores this; both sums call it on g_pMain 3377585c ): clears the out byte, then true
		for the "Shared Item Effect" sockets 2000139-2000141 / 2000144 / 2000145 ( 0x1e850b..0x1e850d, 0x1e8510, 0x1e8511 ).
*/
bool CX2Unit::IsPartyShareSocketID( const int iSocketID_, bool& bSkipDedup_ )
{
	bSkipDedup_ = false;
	switch ( iSocketID_ )
	{
	case 2000139:
	case 2000140:
	case 2000141:
	case 2000144:
	case 2000145:
		return true;
	}
	return false;
}

/** @function : JuneGetPartyShareSocketOptions
	@brief : the pass both June sums inline after 31ed40c0 ( the unit fill 32365d30 @323667e9..32366c26 and the user's InitSocketData
		3206d770 @3206df43..3206e326, the same code ). Every non-zero id of my own sources is collected. Then, per source of the others,
		its non-zero ids that 31d70d30 admits ( with g_pMain ) and that are not among mine are kept in order, and a source that keeps any
		is returned with its rate and level. Ids are not deduplicated across members ( JUNE_BUGS ): two members with the same shared
		socket both count. The no-dedup branch ( out byte set ) is dead in June, since 31d70d30 always clears the byte.
*/
void CX2Unit::GetPartyShareSocketOptions( const UidType iUnitUID_, std::vector<SocketOptionSource>& vecShared_ )
{
	std::vector<SocketOptionSource> vecMine;
	std::vector<SocketOptionSource> vecOthers;
	GetPartySocketOptionSources( iUnitUID_, vecMine, vecOthers );

	std::vector<int> vecMineID;
	for ( UINT iSource = 0; iSource < vecMine.size(); ++iSource )
	{
		for ( UINT i = 0; i < vecMine[ iSource ].m_vecSocketOption.size(); ++i )
		{
			if ( 0 != vecMine[ iSource ].m_vecSocketOption[i] )
				vecMineID.push_back( vecMine[ iSource ].m_vecSocketOption[i] );
		}
	}

	for ( UINT iSource = 0; iSource < vecOthers.size(); ++iSource )
	{
		SocketOptionSource kShared;
		kShared.m_fAtkMagic		= vecOthers[ iSource ].m_fAtkMagic;
		kShared.m_iSocketLevel	= vecOthers[ iSource ].m_iSocketLevel;

		for ( UINT i = 0; i < vecOthers[ iSource ].m_vecSocketOption.size(); ++i )
		{
			const int iSocketID = vecOthers[ iSource ].m_vecSocketOption[i];
			if ( 0 == iSocketID || NULL == g_pMain )
				continue;

			bool bSkipDedup = false;
			if ( false == IsPartyShareSocketID( iSocketID, bSkipDedup ) )
				continue;
			if ( false == bSkipDedup && vecMineID.end() != std::find( vecMineID.begin(), vecMineID.end(), iSocketID ) )
				continue;

			kShared.m_vecSocketOption.push_back( iSocketID );
		}

		if ( false == kShared.m_vecSocketOption.empty() )
			vecShared_.push_back( kShared );
	}
}
#endif SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET
#endif SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET

#ifdef SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP
/** @function : JuneResetHudMaxHpMp
	@brief : June 3235a160 ( CX2Unit, __fastcall ): my HUD's max MP and max HP from this unit's data ( the "x/300" MP text and the guide
		marks, CX2MyGageUI ). Guard: CX2GageManager::GetInstance ( 324943c0 ), g_pData +0x68 ( my user ) +0x90 ( select unit ). Then:
		vslot 1( 2 ), keep the aggregate's stat[0] ( m_fIncreaseHPRate, the HP cap ), vslot 1( +0xe40 == 1 ) ( June's second-character
		byte, Lu/Ciel: 0 in March ).
		MP = 3234b9d0( 300 ( 3344f348 ), 0 ) x ( 1 + vslot 0xc ( m_fIncreaseMPRate ) + 32358970( 5 ) x 0.01 ) + vslot 0x10 ( m_iAddMPValue )
		-> 3248a800( uid, mp, 0 ); unit type 10 repeats it for its second gage ( no March class ).
		HP: a CX2Stat::Stat ( 32e89900 ) sums 32358b00 ( all 13 floats ), then floats [6..12] of 3234e210( 0, +0xe40 == 1 ), 32890230,
		328902f0( +0x988 ), 326f8080 ( title ) and the aggregate's own stat; only [6], the base HP, is read:
		HP = sum + min( sum x 0.5 ( 3342ebc8 ), cap ) -> 3248d3a0( uid, hp ).
*/
void CX2Unit::ResetHudMaxHpMp()
{
	CX2GageManager* pGageManager = CX2GageManager::GetInstance();
	if ( NULL == pGageManager || NULL == g_pData || NULL == g_pData->GetMyUser() || NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;
	if ( NULL == m_kSocketAggregate.m_p )	/// March safety: June's Init always made it
		return;

	const CX2SocketItem::SocketData& kAggregate = m_kSocketAggregate.m_p->m_kSocketData;

	InitSocketData( 2 );
	const float fHpCap = kAggregate.m_Stat.m_ExtraStat.m_fIncreaseHPRate;
	InitSocketData( 0 );

	const float fResetMaxMp = ResetMaxMp( 300.f, 0 );
	const float fAbilityRate = 0.f;		/// 32358970( 5 ) x 0.01: June's per-unit ability map, 0 in March ( Buff Phase 16, S7j )
	const float fMpRate = kAggregate.m_Stat.m_ExtraStat.m_fIncreaseMPRate + fAbilityRate;
	const float fMaxMp = fResetMaxMp + fResetMaxMp * fMpRate + static_cast<float>( kAggregate.m_iAddMPValue );
	pGageManager->SetMaxMp( GetUID(), fMaxMp );

	float fSumBaseHP = 0.f;
	fSumBaseHP = GetUnitStat().m_fBaseHP + fSumBaseHP;
	fSumBaseHP = GetEqipStat( false, 0 ).m_fBaseHP + fSumBaseHP;
	fSumBaseHP = GetSkillStat().m_fBaseHP + fSumBaseHP;
#ifdef GUILD_SKILL
	fSumBaseHP = GetGuildSkillStat().m_fBaseHP + fSumBaseHP;
#endif GUILD_SKILL
	const float fTitleBaseHP = ( NULL != g_pData->GetTitleManager() ) ? g_pData->GetTitleManager()->GetSocketStat().m_fBaseHP : 0.f;
	/// S7k review: June adds the title into the sum and stores it ( @3235a5a9..3235a5b0 ), then the aggregate ( @3235a624..3235a62f ),
	/// each rounded to float; Ghidra's decompile prints it as aggregate + title + sum
	fSumBaseHP = fTitleBaseHP + fSumBaseHP;
	fSumBaseHP = kAggregate.m_Stat.m_fBaseHP + fSumBaseHP;

	float fHpBonus = 0.5f * fSumBaseHP;
	if ( fHpCap < fHpBonus )
		fHpBonus = fHpCap;
	const float fMaxHp = fHpBonus + fSumBaseHP;
	pGageManager->SetMaxHp( GetUID(), fMaxHp );

#ifdef SERV_IRUHADEV_OFFLINE
	CX2OfflineLog::Server( L"[JUNE_SKILL] HUD uid %I64d: max MP %.1f ( ResetMaxMp %.1f, socket rate %.3f, add %d ), max HP %.1f ( base %.1f, socket cap %.1f )",
		GetUID(), fMaxMp, fResetMaxMp, fMpRate, kAggregate.m_iAddMPValue, fMaxHp, fSumBaseHP, fHpCap );
#endif SERV_IRUHADEV_OFFLINE
}
#endif SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP

CX2Stat::Stat CX2Unit::GetUnitStat() const
{
#ifdef SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS
	/// June 32358b00 ( skill Phase S7k ): the unit stat ( +0x150, 13 protected floats ), then, with the skill tree ( g_pData +0x50 ), four
	/// passives read as GetSkillLevel( id, true, 0, true ) ( 32890620( id, 1, 0, 1 ) ), GetSkillTemplet( id ) ( 32206450( id, 1 ) ) and
	/// GetSkillAbilityValue( type, level, false ) ( 32206e50( type, level, 0, 0.0 ) ). The added terms use the base value ( the protected
	/// copy ) and June's ( rate - 1.0 ( 334355b0 ) + 5e-8 ( 3346ed68 ) ), where March's CalculateIncreasingRate adds 5e-5.
	CX2Stat::Stat retval = m_UnitData.m_Stat;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	if ( NULL != pSkillTree )
	{
		const CX2UserSkillTree& refUserSkillTree = m_UnitData.m_UserSkillTree;

		/// 21403 RNSI_P_ELK_STRONG_BONE ( June Enum.lua ): base HP x SA_MAX_HP_REL
		const CX2SkillTree::SKILL_ID eStrongBone = static_cast<CX2SkillTree::SKILL_ID>( 21403 );
		int iSkillLevel = refUserSkillTree.GetSkillLevel( eStrongBone, true, 0, true );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eStrongBone );
			if ( NULL != pSkillTemplet )
				retval.m_fBaseHP = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel, false ) * retval.m_fBaseHP;
		}

		/// 26403 ( June's Guard Mastery, icon SI_P_CFG_GUARD_MASTERY ): base HP x ( SA_MAX_HP_REL - 1 )
		const CX2SkillTree::SKILL_ID eGuardMastery = static_cast<CX2SkillTree::SKILL_ID>( 26403 );
		iSkillLevel = refUserSkillTree.GetSkillLevel( eGuardMastery, true, 0, true );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eGuardMastery );
			if ( NULL != pSkillTemplet )
			{
				const float fRate = static_cast<float>( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel, false ) ) - 1.0 + 5e-08 );
				retval.m_fBaseHP = fRate * m_UnitData.m_Stat.m_fBaseHP + retval.m_fBaseHP;
			}
		}

		/// 7407 SI_P_ASD_SELF_PROTECTION_FORTITUDE: physical / magic defence ( SA 9 / 10 ) and base HP, each x ( rate - 1 ) of its base
		const CX2SkillTree::SKILL_ID eFortitude = static_cast<CX2SkillTree::SKILL_ID>( 7407 );
		iSkillLevel = refUserSkillTree.GetSkillLevel( eFortitude, true, 0, true );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eFortitude );
			if ( NULL != pSkillTemplet )
			{
				float fRate = static_cast<float>( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_PHYSIC_REL, iSkillLevel, false ) ) - 1.0 + 5e-08 );
				retval.m_fDefPhysic = fRate * m_UnitData.m_Stat.m_fDefPhysic + retval.m_fDefPhysic;
				fRate = static_cast<float>( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_MAGIC_REL, iSkillLevel, false ) ) - 1.0 + 5e-08 );
				retval.m_fDefMagic = fRate * m_UnitData.m_Stat.m_fDefMagic + retval.m_fDefMagic;
				fRate = static_cast<float>( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel, false ) ) - 1.0 + 5e-08 );
				retval.m_fBaseHP = fRate * m_UnitData.m_Stat.m_fBaseHP + retval.m_fBaseHP;
			}
		}

		/// 106 RNSI_P_COMMON_HYPER_AURA ( June's level-80 common passive ): base HP x ( SA_MAX_HP_REL - 1 )
		const CX2SkillTree::SKILL_ID eHyperAura = static_cast<CX2SkillTree::SKILL_ID>( 106 );
		iSkillLevel = refUserSkillTree.GetSkillLevel( eHyperAura, true, 0, true );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eHyperAura );
			if ( NULL != pSkillTemplet )
			{
				const float fRate = static_cast<float>( static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel, false ) ) - 1.0 + 5e-08 );
				retval.m_fBaseHP = fRate * m_UnitData.m_Stat.m_fBaseHP + retval.m_fBaseHP;
			}
		}
	}
	/// @32358db0: + 32358970( 5 ) x base HP x 0.01, June's per-unit ability map: 0 in March ( Buff Phase 16 )
	return retval;
#else SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS
	CX2Stat::Stat retval = m_UnitData.m_Stat;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	if( pSkillTree != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = m_UnitData.m_UserSkillTree;
		
		int iSkillLevel = 0;
		
#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ES_POWERFUL_VITAL, true );

		if( iSkillLevel > 0)
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ES_POWERFUL_VITAL );

			if( pSkillTemplet != NULL )
			{
				retval.m_fBaseHP *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel );
			}
		}
#else //UPGRADE_SKILL_SYSTEM_2013
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_POWERFUL_VITAL );

		if( iSkillLevel > 0)
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_POWERFUL_VITAL, iSkillLevel );

			if( pSkillTemplet != NULL )
			{
				retval.m_fBaseHP *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL );
			}
		}
#endif //UPGRADE_SKILL_SYSTEM_2013

		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ELK_STRONG_BONE, true );
		if( iSkillLevel > 0)
		{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ELK_STRONG_BONE );

			if( pSkillTemplet != NULL )
			{
				retval.m_fBaseHP *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel );
			}
	#else // UPGRADE_SKILL_SYSTEM_2013
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ELK_STRONG_BONE, iSkillLevel );

			if( pSkillTemplet != NULL )
			{
				retval.m_fBaseHP *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL );
			}
	#endif // UPGRADE_SKILL_SYSTEM_2013
		}

		//{{ kimhc // 2011.1.14 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_GUARD_MASTERY, true );
	#else // UPGRADE_SKILL_SYSTEM_2013
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_GUARD_MASTERY );
	#endif // UPGRADE_SKILL_SYSTEM_2013
		
		if ( 0 < iSkillLevel )
		{
	#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환 - 스킬 시스템 변경
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_GUARD_MASTERY );
			if ( NULL != pSkillTemplet )
			{
				retval.m_fBaseHP += m_UnitData.m_Stat.m_fBaseHP * 
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel ) );
			} // if
	#else // UPGRADE_SKILL_SYSTEM_2013
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_GUARD_MASTERY, iSkillLevel );
			if ( NULL != pSkillTemplet )
			{
				retval.m_fBaseHP += m_UnitData.m_Stat.m_fBaseHP * CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL ) );
			} // if
	#endif // UPGRADE_SKILL_SYSTEM_2013

		} // if
#endif	CHUNG_FIRST_CLASS_CHANGE
		//}} kimhc // 2011.1.14 // 청 1차 전직

#ifdef UPGRADE_SKILL_SYSTEM_2013 //JHKang
		#pragma region SI_P_ASD_SELF_PROTECTION_FORTITUDE
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ASD_SELF_PROTECTION_FORTITUDE, true );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ASD_SELF_PROTECTION_FORTITUDE  );

			if( NULL != pSkillTemplet )
			{
				retval.m_fDefPhysic += m_UnitData.m_Stat.m_fDefPhysic * 
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_PHYSIC_REL, iSkillLevel ) );
				retval.m_fDefMagic += m_UnitData.m_Stat.m_fDefMagic * 
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_MAGIC_REL, iSkillLevel ) );
				retval.m_fBaseHP += m_UnitData.m_Stat.m_fBaseHP * 
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel ) );
			}
		}
		#pragma endregion 호신강기
#endif //UPGRADE_SKILL_SYSTEM_2013
	}
	return retval;
#endif SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS

}

#ifdef SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS
/** @function : GetEqipStat
	@brief : June 3234e210( out, bWithPvpEquip, group ) ( skill Phase S7k ): the fashion list ( +0xca0 ) and the normal list ( +0xcb0 ),
		each item 3286a230( uid ) with a templet and in the group ( group 2, item group 2 or item group == group: 3286a4b0 =
		CX2Inventory::JuneGetEquipGroup ); a normal item must also be usable ( 328825c0: March's endurance check ) and, when it is a
		PvP-equip item ( templet flags bit 7 ), counts only with bWithPvpEquip ( 32368910( stat, 0 ), the same floats ). Each adds
		floats [6..12] of its stat ( 328842f0 ): base HP, attack, defence, never the ExtraStat rates and without AddStat's LimitMaximum.
*/
CX2Stat::Stat CX2Unit::GetEqipStat( const bool bWithPvpEquip_, const int iEquipGroup_ ) const
{
	CX2Stat::Stat retStat;
	for( int i = 0; i < GetFashionEqipNum(); i++ )
	{
		CX2Item* pItem = GetInventory().GetItem( GetFashionEqipUID(i) );
		if ( NULL == pItem || NULL == pItem->GetItemTemplet() )
			continue;

		const int iGroup = GetInventory().GetEquipGroup( pItem->GetUID() );
		if ( 2 != iEquipGroup_ && 2 != iGroup && iGroup != iEquipGroup_ )
			continue;

		const CX2Stat::Stat& kItemStat = pItem->GetStat();
		retStat.m_fBaseHP		= kItemStat.m_fBaseHP + retStat.m_fBaseHP;
		retStat.m_fAtkPhysic	= kItemStat.m_fAtkPhysic + retStat.m_fAtkPhysic;
		retStat.m_fAtkMagic		= kItemStat.m_fAtkMagic + retStat.m_fAtkMagic;
		retStat.m_fDefPhysic	= kItemStat.m_fDefPhysic + retStat.m_fDefPhysic;
		retStat.m_fDefMagic		= kItemStat.m_fDefMagic + retStat.m_fDefMagic;
	}

	for( int i = 0; i < GetNormalEqipNum(); i++ )
	{
		CX2Item* pItem = GetInventory().GetItem( GetNormalEqipUID(i) );
		if ( NULL == pItem || NULL == pItem->GetItemTemplet() )
			continue;

		/// 328825c0: June's usable test ( expiry, endurance, the item-manager list 0x4d ); March has the endurance part, the rest is Stage 16
#ifdef REAL_TIME_ELSWORD
		if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
			continue;
#endif REAL_TIME_ELSWORD

		const int iGroup = GetInventory().GetEquipGroup( pItem->GetUID() );
		if ( 2 != iEquipGroup_ && 2 != iGroup && iGroup != iEquipGroup_ )
			continue;

		if ( true == pItem->GetItemTemplet()->GetPvpEquip() && false == bWithPvpEquip_ )
			continue;

		const CX2Stat::Stat& kItemStat = pItem->GetStat();
		retStat.m_fBaseHP		= kItemStat.m_fBaseHP + retStat.m_fBaseHP;
		retStat.m_fAtkPhysic	= kItemStat.m_fAtkPhysic + retStat.m_fAtkPhysic;
		retStat.m_fAtkMagic		= kItemStat.m_fAtkMagic + retStat.m_fAtkMagic;
		retStat.m_fDefPhysic	= kItemStat.m_fDefPhysic + retStat.m_fDefPhysic;
		retStat.m_fDefMagic		= kItemStat.m_fDefMagic + retStat.m_fDefMagic;
	}

	return retStat;
}
#else SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS
CX2Stat::Stat CX2Unit::GetEqipStat() const
{
	CX2Stat::Stat retStat;
	for( int i = 0; i < GetFashionEqipNum(); i++ )
	{
		CX2Item* pItem = GetInventory().GetItem( GetFashionEqipUID(i) );		

		if ( pItem != NULL 
            && pItem->GetItemTemplet() != NULL 
            )
		{
			//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
			// 소켓 옵션은 가져오지 않도록 수정
			retStat.AddStat( pItem->GetStat() );
			//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		}	
	}

	for( int i = 0; i < GetNormalEqipNum(); i++ )
	{
		CX2Item* pItem = GetInventory().GetItem( GetNormalEqipUID(i) );

		if ( pItem != NULL 
            && pItem->GetItemTemplet() != NULL 
            )
		{
			//{{ kimhc // 실시간 엘소드 중 실시간 내구도 소모
#ifdef REAL_TIME_ELSWORD
			if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
				continue;
#endif REAL_TIME_ELSWORD
			//}} kimhc // 실시간 엘소드 중 실시간 내구도 소모
			
			//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
			// 소켓 옵션은 가져오지 않도록 수정
			retStat.AddStat( pItem->GetStat() );
			//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업			
		}		
	}

	return retStat;
}
#endif SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS



// 속성 enchant에 의한 stat, 즉 속성 방어 수치를 계산해준다
// @bUpdate: true인 경우 stat을 다시 계산해주고, false인 경우 이전에 계산해뒀던 값을 바로 return


// UI출력용 스탯 계산 할 때만 사용하는 매개변수
// @bShowDungeonOptionUI_ : 던전용 스탯 보여줄지 여부
// @bShowPvpOptionUI : 대전용 스탯 보여줄지 여부
const CX2Stat::EnchantStat& CX2Unit::GetEnchantStat( bool bUpdate /*= true*/, bool bShowDungeonOptionUI_ /*= false*/, bool bShowPvpOptionUI_ /*= false */)
{
	if( false == bUpdate )
		return m_EnchantStat;

	m_EnchantStat.Init();

	for( UINT i=0; i< m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType eqipUID = m_UnitData.m_NowEqipItemUIDList[i];
		CX2Item* pItem = m_UnitData.m_Inventory.GetItem( eqipUID );
		if( NULL == pItem )
			continue;

		const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
		if( NULL == pItemTemplet )
			continue;


		if( pItemTemplet->GetItemType() == CX2Item::IT_DEFENCE ) 
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE currExtraDamageType = g_pData->GetEnchantItem()->GetExtraDamageType( pItem->GetItemData().m_EnchantedAttribute );

			switch( currExtraDamageType )
			{

			case CX2DamageManager::EDT_ENCHANT_BLAZE:						m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN:						m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_POISON:						m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE; 		break;
			case CX2DamageManager::EDT_ENCHANT_PIERCING:					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_SHOCK:						m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_SNATCH:						m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE; 			break;

			case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:				m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:				m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:				m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:				m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:				m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:				m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
				

			case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:				
				{
					m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:					
				{
					m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:				
				{
					m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE; 
				} break;

			case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:				
				{
					m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:				
				{
					m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:				
				{
					m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:	
				{
					m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:	
				{
					m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:				
				{
					m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE; 
					m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE; 
				} break;
			}
		}


		// 아이템 고유의 소켓 옵션에 의한
        unsigned uiNumSocketOption = pItemTemplet->GetNumSocketOption();
		for( UINT j=0; j<uiNumSocketOption; j++ )
		{
            int socketOptionID = pItemTemplet->GetSocketOption(j);
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );

			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( socketOptionID );
		}


		// 아이템에 유저가 박은 소켓에 의한 
		for( UINT j=0; j < pItem->GetItemData().m_SocketOption.size(); j++ )
		{
			int socketOptionID = pItem->GetItemData().m_SocketOption[j];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );

			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( socketOptionID );
		}

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		// 감정을 통해 얻는 랜덤 소켓 옵션
		BOOST_FOREACH( int iSocketID, pItem->GetItemData().m_vecRandomSocket )
		{
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );

			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( iSocketID );
		}
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

	} // for(i)


//#ifdef TITLE_SYSTEM   
	if( NULL != g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() ) )
	{
		const vector<int>& vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() )->m_vecSocketOption;

		for(UINT i=0; i<vecTitleSocketOption.size(); ++i)
		{
			int socketOptionID = vecTitleSocketOption[i];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( socketOptionID );
		}
	}
//#endif TITLE_SYSTEM

#ifdef PET_AURA_SKILL
	KPetInfo *pPetInfo = GetPetInfo();
	if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetMyPet() != NULL && pPetInfo != NULL )
	{			
		// 포만도가 40% 이상일 경우만 기운스킬 적용되도록 한다.
		if( (float)pPetInfo->m_sSatiety / CX2PetManager::MAX_OF_SATIETY >= CX2PetManager::SATIETY_RATIO_TO_BE_ABLE_TO_APPLY_AURA )
		{
#ifdef SERV_PETID_DATA_TYPE_CHANGE //2013.07.02
			CX2PetManager::PetTemplet *pPetTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)pPetInfo->m_iPetID );
#else //SERV_PETID_DATA_TYPE_CHANGE
			CX2PetManager::PetTemplet *pPetTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)pPetInfo->m_cPetID );
#endif //SERV_PETID_DATA_TYPE_CHANGE
			int iStep = pPetTemplet->m_vecPetStatus[pPetInfo->m_cEvolutionStep];
			if( pPetTemplet != NULL && iStep > 0 )
			{			
				for(int i=0; i < iStep; ++i)
				{
					const std::vector<int>& vecAuraSkillOptionTemp = pPetTemplet->m_vecAuraSkillOption[ i ];
					for( UINT j=0; j<vecAuraSkillOptionTemp.size(); ++j)
					{
						AddEchantStatFromSocket( vecAuraSkillOptionTemp[j] );
					}
				}
			}
		}
	}	
#endif

	// 세트 아이템에 의해 속성방어증가
	map<int,int> mapSetIDNParts;
	map<int,int>::iterator it;

	GetSetIDNPartsNum( mapSetIDNParts );
	vector< int > vecSetItemOptions;

	for( it = mapSetIDNParts.begin(); it != mapSetIDNParts.end(); it ++ )
	{
		g_pData->GetItemManager()->GetSetItemOptions( it->first, it->second, vecSetItemOptions );
	}

	for ( int i = 0; i < (int)vecSetItemOptions.size(); i++ )
	{
		AddEchantStatFromSocket( vecSetItemOptions[i] );
	}


//{{ kimhc // 2011.1.14 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	UpdateEnchantStatFromPassiveSkill();

	m_EnchantStat += GetEnchantStatFromPassiveSkill();
#endif	CHUNG_FIRST_CLASS_CHANGE
//}} kimhc // 2011.1.14 // 청 1차 전직

	//{{ kimhc // 2011.4.24 // 월드버프 2단계, 드래곤의 숨결
#ifdef	SERV_INSERT_GLOBAL_SERVER
	AddEnchantStatFromDragonBreath( m_EnchantStat );	
#endif	SERV_INSERT_GLOBAL_SERVER
	//}} kimhc // 2011.4.24 // 월드버프 2단계, 드래곤의 숨결

	if( m_EnchantStat.m_fDefBlaze > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefBlaze = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefWater > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefWater = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefNature > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefNature = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefWind > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefWind = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefLight > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefLight = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefDark > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefDark = CX2EnchantItem::EAR_MAX_VALUE;

	return m_EnchantStat;
}


void CX2Unit::AddEchantStatFromSocket( int iSocketOptionID )
{
	const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketOptionID );

	if( NULL == pSocketData )
		return;



	// 속성 저항
	for( int k= CX2EnchantItem::ET_NONE+1; k<CX2EnchantItem::ET_END; k++ )
	{
		switch( k )
		{
		case CX2EnchantItem::ET_BLAZE:
			m_EnchantStat.m_fDefBlaze += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_WATER:
			m_EnchantStat.m_fDefWater += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_NATURE:
			m_EnchantStat.m_fDefNature += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_WIND:
			m_EnchantStat.m_fDefWind += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_LIGHT:
			m_EnchantStat.m_fDefLight += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_DARK:
			m_EnchantStat.m_fDefDark += (float) pSocketData->m_aiResistEnchant[ k ];
			break;
		}
	}

}

//{{ kimhc // 2011.4.24 // 월드버프 2단계, 드래곤의 숨결
#ifdef	SERV_INSERT_GLOBAL_SERVER
void CX2Unit::AddEnchantStatFromDragonBreath( OUT CX2Stat::EnchantStat& enchantStat_ )
{
	if ( NULL != g_pData->GetWorldMissionManager() )
	{
		const CX2Stat::EnchantStat* pEnchantStatDragonBreath = NULL;
		
		// 2단계 월드버프가 적용 중이어서 증가될 EnchantStat을 얻어오면
		if ( true == g_pData->GetWorldMissionManager()->GetDragonBreathEnchantStatIncreased( pEnchantStatDragonBreath ) )
			enchantStat_ += *pEnchantStatDragonBreath;
	}	
}
#endif	SERV_INSERT_GLOBAL_SERVER
//}} kimhc // 2011.4.24 // 월드버프 2단계, 드래곤의 숨결


float CX2Unit::GetRepairDiscountRate()
{
	float fDiscountRate = 0.f;


	BOOST_TEST_FOREACH( const UidType&, eqipUID, m_UnitData.m_NowEqipItemUIDList )
	{
		CX2Item* pItem = m_UnitData.m_Inventory.GetItem( eqipUID );
		if( NULL == pItem )
			continue;

		const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
		if( NULL == pItemTemplet )
			continue;

		// 아이템 고유의 소켓 옵션에 의한
        unsigned uNumSockOption = pItemTemplet->GetNumSocketOption();
		for( UINT j=0; j<uNumSockOption; j++ )
		{
            int socketOptionID = pItemTemplet->GetSocketOption(j);
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}


		// 아이템에 유저가 박은 소켓에 의한 
		for( UINT j=0; j < pItem->GetItemData().m_SocketOption.size(); j++ )
		{
			int socketOptionID = pItem->GetItemData().m_SocketOption[j];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
		// 감정을 통해 얻는 랜덤 소켓 옵션
		BOOST_FOREACH( int iSocketID, pItem->GetItemData().m_vecRandomSocket )
		{
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}
#endif // SERV_NEW_ITEM_SYSTEM_2013_05
	} 



	// 칭호
	if( NULL != g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() ) )
	{
		const vector<int>& vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() )->m_vecSocketOption;

		for(UINT i=0; i<vecTitleSocketOption.size(); ++i)
		{
			int socketOptionID = vecTitleSocketOption[i];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}
	}
	



	// 세트 아이템에 의한
	map<int,int> mapSetIDNParts;
	map<int,int>::iterator it;

	GetSetIDNPartsNum( mapSetIDNParts );
	vector< int > vecSetItemOptions;

	for( it = mapSetIDNParts.begin(); it != mapSetIDNParts.end(); it ++ )
	{
		g_pData->GetItemManager()->GetSetItemOptions( it->first, it->second, vecSetItemOptions );
	}

	for ( int i = 0; i < (int)vecSetItemOptions.size(); i++ )
	{
		int socketOptionID = vecSetItemOptions[i];
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
		if( NULL == pSocketData )
			continue;

		if( 0.f == pSocketData->m_fRepairPriceSale )
			continue;

		fDiscountRate += pSocketData->m_fRepairPriceSale;
	}

#ifdef SERV_NEW_DEFENCE_DUNGEON // 적용날짜: 2013-04-18

#ifdef SERV_IRUHADEV_JUNE_BUFF_REPAIR_DISCOUNT
	/// June 32363f00 @32364246: each world-buff record's factor, by the factor's templet ID (+0x28)
	BOOST_FOREACH( const WorldBuffRecordPair& kRecord, m_vecWorldBuffRecord )
	{
		if ( NULL == CX2BuffTempletManager::GetInstance() )
			continue;

		CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( static_cast<UINT>( kRecord.first ) );
		if ( NULL == ptrBuffFactor )
			continue;

		switch ( static_cast<int>( ptrBuffFactor->GetBuffTempletID() ) )	/// jump table 32364480 covers 202..207
		{
		case BTI_BUFF_2013_DEFENSE_BUFF:		/// 202
		case 10038:								/// BTI_SURPRISE_SALE_EVENT
			fDiscountRate += 0.5f;
			break;
		case BTI_BUFF_2013_DEFENSE_DEBUFF:		/// 203
			fDiscountRate -= 1.5f;
			break;
		case BTI_BUFF_2013_DEFENSE_BUFF_EVENT:	/// 204
			fDiscountRate += 0.7f;
			break;
		case 207:								/// BTI_FESTIVAL_EVENT_BUFF
			fDiscountRate += 0.1f;
			break;
		case 10027:								/// BTI_FESTIVAL_EVENT_BUFF_INT
			fDiscountRate += 0.2f;
			break;
		case 876:								/// BTI_SUMMER_VACATION_2016_KR_VER2_BUFF
			/// June @32364315 SETS the rate to 1.0 when key 17 of June's event map (global 33775864 +0xa18,
			/// also read by 32950d60) is on. March has no such map, so the key reads as absent and the
			/// case does nothing here (JUNE_BUGS #40).
			break;
		}
	}

	/// June @323643c1: no MAX_REPAIR_PRICE_DISCOUNT clamp; below 0 the rate is negated, otherwise 1 - rate
	if ( 0.f > fDiscountRate )
		fDiscountRate *= -1.f;
	else
		fDiscountRate = 1.f - fDiscountRate;
#else // SERV_IRUHADEV_JUNE_BUFF_REPAIR_DISCOUNT
#ifdef SET_WORLD_BUFF_AT_RESURRECTION // 김태환		서버에서 준 버프 정보를 버프 펙터 아이디로 저장하였기 때문에, 수정
	BOOST_FOREACH( BUFF_FACTOR_ID eBuffFactorID, m_vecWorldBuffFactorID )
	{
		if ( NULL != CX2BuffTempletManager::GetInstance() )
		{
			CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( eBuffFactorID );

			if( NULL != ptrBuffFactor )
			{
				switch ( ptrBuffFactor->GetBuffTempletID() )
				{	
				case BTI_BUFF_2013_DEFENSE_BUFF_EVENT:	/// 이벤트용 2013 어둠의 문 참여자 버프
					{
						fDiscountRate += 0.7f;
					} break;
				case BTI_BUFF_2013_DEFENSE_BUFF:		/// 2013 어둠의 문 참여자 진짜 버프
					{
						fDiscountRate += 0.5f;
					} break;
				case BTI_BUFF_2013_DEFENSE_DEBUFF:		/// 2013 어둠의 문 월드 디버프
					{
						fDiscountRate -= 1.5f;
					} break;
				}
			}
		}
	}
#else // SET_WORLD_BUFF_AT_RESURRECTION
	BOOST_FOREACH( CX2BuffFactorPtr pBuffFactorPtr, m_vecWorldBuffFactorPtr )
	{
		if( NULL != pBuffFactorPtr )
		{
			switch ( pBuffFactorPtr->GetBuffTempletID() )
			{	
			case BTI_BUFF_2013_DEFENSE_BUFF_EVENT:	/// 이벤트용 2013 어둠의 문 참여자 버프
				{
					fDiscountRate += 0.7f;
				} break;
			case BTI_BUFF_2013_DEFENSE_BUFF:		/// 2013 어둠의 문 참여자 진짜 버프
				{
					fDiscountRate += 0.5f;
				} break;
			case BTI_BUFF_2013_DEFENSE_DEBUFF:		/// 2013 어둠의 문 월드 디버프
				{
					fDiscountRate -= 1.5f;
				} break;
			}
			
		}
	}
#endif // SET_WORLD_BUFF_AT_RESURRECTION

	if( fDiscountRate > CX2SocketItem::SocketData::MAX_REPAIR_PRICE_DISCOUNT )
		fDiscountRate = CX2SocketItem::SocketData::MAX_REPAIR_PRICE_DISCOUNT;

	if ( 0.f <= fDiscountRate )		/// 수리비 감소시
		fDiscountRate = 1.f - fDiscountRate;
	else							/// 수리비 증가시
		fDiscountRate *= -1.f ;
#endif SERV_IRUHADEV_JUNE_BUFF_REPAIR_DISCOUNT

#else // SERV_NEW_DEFENCE_DUNGEON

		if( fDiscountRate > CX2SocketItem::SocketData::MAX_REPAIR_PRICE_DISCOUNT )
		fDiscountRate = CX2SocketItem::SocketData::MAX_REPAIR_PRICE_DISCOUNT;

	//{{ kimhc // 2011-04-24 // 월드버프 1단계 주민의 감사
#ifdef	SERV_INSERT_GLOBAL_SERVER
	//if ( NULL != g_pData->GetWorldMissionManager() )
	//{
	//	// 주민의 감사가 적용 됐으면 (1단계)
	//	if ( 0 < g_pData->GetWorldMissionManager()->GetWorldBuffStepNum() )
	//		fDiscountRate += 0.5f;
	//}

	/// 월드버프 1단계 주민의 감사
	BOOST_FOREACH( CX2BuffFactorPtr pBuffFactorPtr, m_vecWorldBuffFactorPtr )
	{
		if( NULL != pBuffFactorPtr && BTI_BUFF_THANKS_OF_RESIDENTS == pBuffFactorPtr->GetBuffTempletID() )
			fDiscountRate += 0.5f;
	}

	// 최대가 넘은면 최대치로
	if ( 1.0f < fDiscountRate )
		fDiscountRate = 1.0f;

#endif	SERV_INSERT_GLOBAL_SERVER
	//}} kimhc // 2011-04-24 // 월드버프 1단계 주민의 감사

#endif // SERV_NEW_DEFENCE_DUNGEON

	return fDiscountRate;
}

#ifdef UNIT_EMOTION
const char* CX2Unit::GetEmotionName(EMOTION_TYPE eEmotionType)
{
	// 이모션 id에 해당되는 스테이트이름을 반환한다.
	switch(eEmotionType)
	{
	case ET_NONE:
		return "";
	case ET_SITREADY:
		return "EMOTION_SITREADY";
	case ET_SITWAIT:
		return "EMOTION_SITWAIT";
	case ET_STANDUP:
		return "EMOTION_STANDUP";
	case ET_ANGRY:
	case ET_HELLO:
	case ET_NO:
	case ET_SAD:
	case ET_SMILE:
	case ET_EMOTION_AVATAR1:
	case ET_EMOTION_AVATAR2:
	case ET_EMOTION_AVATAR3:
	case ET_EMOTION_AVATAR4:
	case ET_EMOTION_AVATAR5:
#ifdef CRAYONPOP_SECOND_EMOTION
	case ET_EMOTION_AVATAR6:
#endif // CRAYONPOP_SECOND_EMOTION

#ifdef CRAYONPOP_EMOTION_WITH_MUSIC		// 크래용 팝 한벌 아바타 이모션, 사운드가 출력됨
	case ET_EMOTION_AVATAR7:
#endif // CRAYONPOP_EMOTION_WITH_MUSIC	// 크래용 팝 한벌 아바타 이모션, 사운드가 출력됨

		return "EMOTION_STATE";
	default:
		return "";
	}	

	return "";	
}

wstring CX2Unit::GetEmotionAniNameById(EMOTION_TYPE eEmotionType)
{
	// 이모션 id에 해당되는 모션이름을 반환한다.
	switch(eEmotionType)
	{
	case ET_NONE:
		return L"";
	case ET_SITREADY:
		return L"Emotion_SitReady";
	case ET_SITWAIT:
		return L"EMOTION_SITWAIT";
	case ET_STANDUP:
		return L"Emotion_StandUp";
	case ET_ANGRY:
		return L"Emotion_Angry";
	case ET_HELLO:
		return L"Emotion_Hello";
	case ET_NO:
		return L"Emotion_No";
	case ET_SAD:
		return L"Emotion_Sad";
	case ET_SMILE:
		return L"Emotion_Smile";
	case ET_EMOTION_AVATAR1:
		return L"Emotion_BIGBANG";
	case ET_EMOTION_AVATAR2:
		return L"Emotion_DeepBow";
	case ET_EMOTION_AVATAR3:
		return L"Emotion_APINK";
	case ET_EMOTION_AVATAR4:
		return L"Emotion_APINK_LOVE";
	case ET_EMOTION_AVATAR5:
		return L"Emotion_CRAYONPOP";
#ifdef CRAYONPOP_SECOND_EMOTION
	case ET_EMOTION_AVATAR6:
		return L"Emotion_BbaBbaBba";
#endif // CRAYONPOP_SECOND_EMOTION
	default:
		return L"";
	}	

	return L"";	
}
#endif

#ifdef SERV_SKILL_NOTE
void CX2Unit::SetSkillNote( std::map<char, int> &mapSkillNote, char iMaxNotePage )
{
	m_mapSkillNote.clear();
	m_mapSkillNote = mapSkillNote;

	m_iMaxSKillNotePage= iMaxNotePage;
	

}

int CX2Unit::GetSkillMemoIdBySlot( char slotId )
{
	map<char, int>::iterator mit = m_mapSkillNote.find(slotId);
   
	if( mit != m_mapSkillNote.end() )
	{
		return mit->second;
	}

	return -1;
}

void CX2Unit::SetSkillNotePage(char iPage, int iMemo)
{
	map<char, int>::iterator mit = m_mapSkillNote.find(iPage);

	if( mit != m_mapSkillNote.end() )
	{
		mit->second = iMemo;
	}
	else
	{
		m_mapSkillNote.insert( std::make_pair(iPage, iMemo) );
	}
}

#endif



CX2Stat::Stat CX2Unit::GetSkillStat() const
{
	return m_UnitData.m_UserSkillTree.GetSkillStat();
}

//{{ kimhc // 2009-11-17 //길드 스킬과 관련된 스탯
#ifdef	GUILD_SKILL

CX2Stat::Stat CX2Unit::GetGuildSkillStat() const
{
	// oasis907 : 김상윤 [2009.12.8] // 길드 등급
	BYTE byMemberShipGrade = m_UnitData.m_byMemberShipGrade; 
	return m_UnitData.m_UserSkillTree.GetGuildSkillStat(byMemberShipGrade);
}

#endif	GUILD_SKILL
//}} kimhc // 2009-11-17 //길드 스킬과 관련된 스탯



void CX2Unit::SetBlackList( vector<KChatBlackListUnit>& blackList )
{

		m_UnitData.SetBlackList( blackList );
}

bool CX2Unit::AddBlackList( KChatBlackListUnit blackList )
{

		return m_UnitData.AddBlackList( blackList );

}

bool CX2Unit::RemoveBlackList( UidType unitUID )
{
		return m_UnitData.RemoveBlackList( unitUID );

}


const vector<KChatBlackListUnit>& CX2Unit::GetBlackList() const
{
	return m_UnitData.GetBlackList();
}

UidType CX2Unit::GetBlackListUnitUID( const WCHAR* wszNickName )
{
	return m_UnitData.GetBlackListUnitUID( wszNickName );
}

void CX2Unit::GetSetIDNPartsNum( map<int,int>& mapSetIDNPartsNum )
{
	const CX2Inventory& kInventory = GetUnitData().m_Inventory;
	{
		for ( int i = 0;  i < kInventory.GetItemMaxNum( CX2Inventory::ST_E_EQUIP ); i++ )
		{
			CX2Item* pItem = kInventory.GetItem( CX2Inventory::ST_E_EQUIP, i );
			if ( pItem != NULL && 
                pItem->GetItemTemplet() != NULL 
                )
			{
				//{{ kimhc // 실시간 엘소드 중 실시간 내구도 감소
#ifdef REAL_TIME_ELSWORD
				if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
					continue;
#endif REAL_TIME_ELSWORD
				//}} kimhc // 실시간 엘소드 중 실시간 내구도 감소

                int setID = pItem->GetItemTemplet()->GetSetID();
				if ( setID != 0 )
				{
					map< int, int >::iterator mit = mapSetIDNPartsNum.find( setID );
					if ( mit != mapSetIDNPartsNum.end() )
					{
						mit->second += 1;
					}
					else
					{
						mapSetIDNPartsNum.insert( std::make_pair( setID, 1 ) );
					}
				}
			}
		}
	}
	
}




// 내 캐릭터가 사용할 수 있는 아이템인가? 
/*static*/ bool CX2Unit::CanUse( int itemID, CX2Unit* pUnit, int iUnitLevel /*= -1*/ )
{
	ASSERT( NULL != g_pData->GetItemManager() );

	if( NULL == pUnit )
		return false;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	if( NULL == pItemTemplet )
		return false;

	if( -1 != iUnitLevel )	
	{
		if( pUnit->GetUnitData().m_Level < pItemTemplet->GetUseLevel() )
			return false;
	}

	switch( pItemTemplet->GetUseCondition() )
	{
	case CX2Item::UC_NONE:
		{
			return false;
		} break;

	case CX2Item::UC_ANYONE:
		{
			return true;
		} break;

	case CX2Item::UC_ONE_UNIT:
		{
			return ( pItemTemplet->GetUnitType() == pUnit->GetType() );
		} break;

	default:
	case CX2Item::UC_ONE_CLASS:
		{
			return GetX2UnitClassCompatibility( pUnit->GetClass(), pItemTemplet->GetUnitClass() );
		} break;
	}

	return false;
}


// 내가 장비류로 몸에 장착할 수 있는 종류의 아이템인가?
// @iUnitLevel이 -1이면 레벨 조건은 검사하지 않는다
/*static*/ bool CX2Unit::CanEquipAsParts( int itemID, CX2Unit* pUnit, int iUnitLevel /*= -1*/ )
{
	ASSERT( NULL != g_pData->GetItemManager() );

	if( NULL == pUnit )
		return false;


	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	if( NULL == pItemTemplet )
		return false;

	if( CX2Item::UT_NONE == pItemTemplet->GetUseType() ) // 장비류로 몸에 걸치는 아이템이 아닌지
		return false;
	
	if( -1 != iUnitLevel )	
	{
		if( pUnit->GetUnitData().m_Level < pItemTemplet->GetUseLevel() ) // 레벨 제한에 걸린다면
			return false;
	}

	// 장비템이 맞다면 장착 조건에 맞는지
	switch( pItemTemplet->GetUseCondition() )
	{
	default:
	case CX2Item::UC_NONE:
		{
			return false;
		} break;

	case CX2Item::UC_ANYONE:
		{
			return true;
		} break;

	case CX2Item::UC_ONE_UNIT:
		{
			return pItemTemplet->GetUnitType() == pUnit->GetType();
		} break;

	case CX2Item::UC_ONE_CLASS:
		{
			return GetX2UnitClassCompatibility( pUnit->GetClass(), pItemTemplet->GetUnitClass() );
		} break;
	}

	return false;
}



// 1차 전직이면 1을 return, 2차 전직이면 2를 return, 
int CX2Unit::GetClassLevel()
{
	switch( m_UnitData.m_UnitClass )
	{
	case UC_ELSWORD_SWORDMAN:		
	case UC_ARME_VIOLET_MAGE:		
	case UC_LIRE_ELVEN_RANGER:		
	case UC_RAVEN_FIGHTER:			
	case UC_EVE_NASOD:
//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
	case UC_CHUNG_IRON_CANNON:
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG
#ifdef ARA_CHARACTER_BASE
	case UC_ARA_MARTIAL_ARTIST:
#endif
	case UC_ELESIS_KNIGHT:

#ifdef SERV_9TH_NEW_CHARACTER
	case UC_ADD_NASOD_RULER:
#endif // SERV_9TH_NEW_CHARACTER

		{
			return 0;
		} break;

	case UC_ELSWORD_KNIGHT:			
	case UC_ELSWORD_MAGIC_KNIGHT:
#ifdef ELSWORD_SHEATH_KNIGHT
	case UC_ELSWORD_SHEATH_KNIGHT:
#endif ELSWORD_SHEATH_KNIGHT
	case UC_LIRE_COMBAT_RANGER:		
	case UC_LIRE_SNIPING_RANGER:	
#ifdef	SERV_TRAPPING_RANGER_TEST
	case UC_LIRE_TRAPPING_RANGER:	
#endif	SERV_TRAPPING_RANGER_TEST
	case UC_ARME_HIGH_MAGICIAN:		
	case UC_ARME_DARK_MAGICIAN:		
	case UC_RAVEN_SOUL_TAKER:		
	case UC_RAVEN_OVER_TAKER:		
	case UC_EVE_EXOTIC_GEAR:		
	case UC_EVE_ARCHITECTURE:		
	//{{ kimhc // 2011.1.3 // 청 1차 전직
#ifdef	CHUNG_FIRST_CLASS_CHANGE
	case UC_CHUNG_FURY_GUARDIAN:
	case UC_CHUNG_SHOOTING_GUARDIAN:
#endif	CHUNG_FIRST_CLASS_CHANGE
	//}} kimhc // 2011.1.3 // 청 1차 전직
#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
	case UC_ARME_BATTLE_MAGICIAN:
#endif
#ifdef RAVEN_WEAPON_TAKER
	case UC_RAVEN_WEAPON_TAKER:
#endif RAVEN_WEAPON_TAKER
#ifdef EVE_ELECTRA
	case UC_EVE_ELECTRA:
#endif EVE_ELECTRA
#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
	case UC_CHUNG_SHELLING_GUARDIAN:
#endif
#ifdef ARA_CHANGE_CLASS_FIRST
	case UC_ARA_LITTLE_HSIEN:
#endif //ARA_CHANGE_CLASS_FIRST
	case UC_ELESIS_SABER_KNIGHT:
	case UC_ELESIS_PYRO_KNIGHT:
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
	case UC_ARA_LITTLE_DEVIL:
#endif // SERV_ARA_CHANGE_CLASS_SECOND
#ifdef SERV_9TH_NEW_CHARACTER
	case UC_ADD_PSYCHIC_TRACER:
#endif // SERV_9TH_NEW_CHARACTER
		{
			return 1;
		} break;

	case UC_ELSWORD_LORD_KNIGHT:
	case UC_ELSWORD_RUNE_SLAYER:
	case UC_LIRE_WIND_SNEAKER:
	case UC_LIRE_GRAND_ARCHER:
	case UC_ARME_ELEMENTAL_MASTER:
	case UC_ARME_VOID_PRINCESS:
#ifdef RAVEN_SECOND_CLASS_CHANGE
	case UC_RAVEN_BLADE_MASTER:
	case UC_RAVEN_RECKLESS_FIST:
#endif

	//{{ kimhc // 2010.1.22 // 이브 2차 전직
#ifdef EVE_SECOND_CLASS_CHANGE
	case UC_EVE_CODE_NEMESIS:
	case UC_EVE_CODE_EMPRESS:
#endif EVE_SECOND_CLASS_CHANGE
	//}} kimhc // 2010.1.22 // 이브 2차 전직

		//{{ JHKang / 강정훈 / 2011.6.13 / 청 2차 전직
#ifdef CHUNG_SECOND_CLASS_CHANGE
	case UC_CHUNG_IRON_PALADIN:
	case UC_CHUNG_DEADLY_CHASER:
#endif
		//}}
#ifdef SERV_ELSWORD_INFINITY_SWORD
	case UC_ELSWORD_INFINITY_SWORD:
#endif
#ifdef SERV_ARME_DIMENSION_WITCH
	case UC_ARME_DIMENSION_WITCH:
#endif
#ifdef SERV_RENA_NIGHT_WATCHER
	case UC_LIRE_NIGHT_WATCHER:
#endif
#ifdef SERV_RAVEN_VETERAN_COMMANDER
	case UC_RAVEN_VETERAN_COMMANDER:
#endif
#ifdef SERV_EVE_BATTLE_SERAPH
	case UC_EVE_BATTLE_SERAPH:
#endif
#ifdef SERV_CHUNG_TACTICAL_TROOPER
	case UC_CHUNG_TACTICAL_TROOPER:
#endif
#ifdef ARA_CHANGE_CLASS_FIRST
	case UC_ARA_SAKRA_DEVANAM:
#endif //ARA_CHANGE_CLASS_FIRST
#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
	case UC_ARA_YAMA_RAJA:
#endif // SERV_ARA_CHANGE_CLASS_SECOND
#ifdef SERV_ELESIS_SECOND_CLASS_CHANGE	  // 김종훈, 엘리시스 1-2 그랜드 마스터, 2-2 블레이징 하트
	case UC_ELESIS_GRAND_MASTER:
	case UC_ELESIS_BLAZING_HEART:
#endif // SERV_ELESIS_SECOND_CLASS_CHANGE // 김종훈, 엘리시스 1-2 그랜드 마스터, 2-2 블레이징 하트
#ifdef SERV_ADD_LUNATIC_PSYKER // 김태환
	case UC_ADD_LUNATIC_PSYKER:
#endif //SERV_ADD_LUNATIC_PSYKER

		{
			return 2;
		} break;
	}


	return -1;
}

//{{ kimhc // 2010-01-04 // 기존의 EGS_SELECT_UNIT_ACK에 인벤정보가 포함되어 오던 것 분할
#ifdef	PC_BANG_WORK
void	CX2Unit::ResetInventorySize( const std::map<int, int>& mapInventorySize )
{
	m_UnitData.m_Inventory.Clear();
	m_UnitData.m_Inventory.ResetSize( mapInventorySize );
}

void	CX2Unit::ResetInventoryItems( const std::map< UidType, KInventoryItemInfo >& mapItem )
{
	m_UnitData.m_Inventory.ResetItems( mapItem );

}
#endif	PC_BANG_WORK
//}} kimhc // 2010-01-04 // 기존의 EGS_SELECT_UNIT_ACK에 인벤정보가 포함되어 오던 것 분할


#ifdef SERV_PET_SYSTEM
void CX2Unit::SetSimplePetInfo( const KFieldPetInfo& kFieldPetInfo_ )
{
	KPetInfo kPetInfo;
	kPetInfo.m_iPetUID = kFieldPetInfo_.m_iPetUID;
#ifdef SERV_PETID_DATA_TYPE_CHANGE //2013.07.02
	kPetInfo.m_iPetID = kFieldPetInfo_.m_iPetID;
#else //SERV_PETID_DATA_TYPE_CHANGE
	kPetInfo.m_cPetID = kFieldPetInfo_.m_cPetID;
#endif //SERV_PETID_DATA_TYPE_CHANGE
	kPetInfo.m_wstrPetName = kFieldPetInfo_.m_wstrPetName;
	kPetInfo.m_cEvolutionStep = kFieldPetInfo_.m_cEvolutionStep;
	kPetInfo.m_iIntimacy = kFieldPetInfo_.m_iIntimacy;
#ifdef PET_DROP_ITEM_PICKUP
	
#endif //PET_DROP_ITEM_PICKUP

	m_vecPetInfo.clear();
	m_vecPetInfo.push_back(kPetInfo);
}

void CX2Unit::SetFullPetInfo( const KPetInfo& kPetInfo_ )
{
	m_vecPetInfo.clear();
	m_vecPetInfo.push_back( kPetInfo_ );
}

KPetInfo *CX2Unit::GetPetInfo()
{
	if( m_vecPetInfo.size() > 0 )
	{		
		return &m_vecPetInfo[0];
	}

	return NULL;
}

#endif
#ifdef PET_DROP_ITEM_PICKUP
void CX2Unit::SetPetAutoLooting( UidType uiPetUID, bool bAutoLooting)
{
	if( m_vecPetInfo.size() == 0 )
		return;

	for(UINT i=0; i<m_vecPetInfo.size(); ++i)
	{
		KPetInfo &petInfo = m_vecPetInfo[i];
		if( petInfo.m_iPetUID == uiPetUID )
		{
			petInfo.m_bAutoLooting = bAutoLooting;
			return;
		}
	}
}
#endif //PET_DROP_ITEM_PICKUP

bool CX2Unit::CheckNowEquipItemByItemId( UidType itemID )
{
	for ( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType itemUid = m_UnitData.m_NowEqipItemUIDList[i];
		
		CX2Item *pItem = GetInventory().GetItem( itemUid );

		if( pItem != NULL && 
            pItem->GetItemTemplet()->GetItemID() == itemID 
            )
		{
			return true;
		}
	}
	return false;
}
bool CX2Unit::GetAvatarEmotion(wstring &wstrEmotionName, CX2Unit::EMOTION_TYPE &eEmotionID)
{	
	switch( GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;	
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_ARME:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_ARME_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_LIRE:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_LIRE_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_RAVEN:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_RAVEN_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_EVE:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Eve_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Eve_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_CHUNG:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Chung_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Chung_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;

#ifdef ADD_ARA_EMOTION		/// 아라 아바타 이모션 추가
	case CX2Unit::UT_ARA:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARA2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Ara_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}

			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARA5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Ara_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARA6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_ARA_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
#endif ADD_ARA_EMOTION


#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
	case CX2Unit::UT_ELESIS:
		{
	#ifdef ADD_KOREAN_CLOTHES_EMOTION // 김태환
			{
				/// 한복 아바타
				for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2 ); ++i )
				{
					if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2[i] ) == 6 )
					{
						wstrEmotionName = L"Motion_EL_Emotion_DeepBow.X";
						eEmotionID = ET_EMOTION_AVATAR2;
						return true;
					}
				}
			}
			break;
	#endif // ADD_KOREAN_CLOTHES_EMOTION

			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_EL_Emotion_BbaBbaBba.x";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;	
				}
			}
		}
		break;
#endif // CRAYONPOP_SECOND_EMOTION

#ifdef ADD_KOREAN_CLOTHES_EMOTION // 김태환
	case CX2Unit::UT_ADD:
		{
			/// 한복 아바타
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ADD2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ADD2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Add_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
		}
		break;
#endif // ADD_KOREAN_CLOTHES_EMOTION

	default:
		break;
	}

	return false;
}

bool CX2Unit::GetAvatarEmotionID(CX2Unit::EMOTION_TYPE &eEmotionID)
{	
	switch( GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;	
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;	
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_ARME:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARME6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_LIRE:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_LIRE6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_RAVEN:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_EVE:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_EVE6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
	case CX2Unit::UT_CHUNG:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;

#ifdef ADD_ARA_EMOTION		/// 아라 아바타 이모션 추가
	case CX2Unit::UT_ARA:
		{
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARA2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARA5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ARA6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
#endif // CRAYONPOP_SECOND_EMOTION
		}
		break;
#endif ADD_ARA_EMOTION

#ifdef CRAYONPOP_SECOND_EMOTION // 김태환
	case CX2Unit::UT_ELESIS:
		{
	#ifdef ADD_KOREAN_CLOTHES_EMOTION // 김태환
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;	
				}
			}
	#endif // ADD_KOREAN_CLOTHES_EMOTION

			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;	
				}
			}
		}
		break;
#endif // CRAYONPOP_SECOND_EMOTION

#ifdef ADD_KOREAN_CLOTHES_EMOTION // 김태환
	case CX2Unit::UT_ADD:
		{
			/// 한복 아바타
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_ET_EMOTION_AVATAR_ADD2 ); ++i )
			{				
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ADD2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;	
				}
			}
		}
		break;
#endif // ADD_KOREAN_CLOTHES_EMOTION

	default:
		break;
	}

	
	return false;
}


void CX2Unit::AddItemLevel( IN const CX2Item* pEquipItem_ )
{
    const CX2Item::ItemTemplet* pItemTemplet = pEquipItem_->GetItemTemplet();

	switch ( pItemTemplet->GetItemType() )
	{
	case CX2Item::IT_WEAPON:
		{
			SetSumOfItemLevel( GetSumOfItemLevel() 
				+ CX2Item::WEAPON_LEVEL_WEIGHT * ( pItemTemplet->GetItemLevel() + pEquipItem_->GetIEchantedItemLevel() ) );
		} break;

	case CX2Item::IT_DEFENCE:
		{
			SetSumOfItemLevel( GetSumOfItemLevel() + ( pItemTemplet->GetItemLevel() + pEquipItem_->GetIEchantedItemLevel() ) );
		} break;

	default:
		break;
	}	
}

void CX2Unit::UpdateItemLevel()
{
	SetSumOfItemLevel( 0 );	// 초기화

	for ( UINT i =0; i < m_NormalEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_NormalEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			continue;

		AddItemLevel( pEqipItem );		
	}
}

int CX2Unit::GetAverageItemlevel() const
{
	return m_iSumOfItemLevel / CX2Item::AVERAGE_ITEM_LEVEL_DIVISOR;
}



//////////////////////////////////////////////////////////////////////////
// CX2Unit::UnitData

void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KUnitInfo& data )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKUnitInfo( data );
}

void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KRoomUserInfo& data )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKRoomUserInfo( data );
}

void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KSquareUserInfo& pKSquareUserInfo )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKSquareUserInfo( pKSquareUserInfo );

}


void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KFieldUserInfo& pKFieldUserInfo )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKFieldUserInfo( pKFieldUserInfo );
	
}


CX2Unit::UnitData::~UnitData()
{
	//SAFE_DELETE( m_pInventory );
}

void CX2Unit::UnitData::Init()
{
	m_UnitUID				= 0;
	m_UserUID				= 0;
	m_UnitClass				= UC_NONE;	
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	m_iUnitClassTran		= 0;
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT

	m_Port					= 0;

	m_ED					= 0;	
	m_Level					= 0;
	m_EXP					= 0;

#ifdef SERV_4TH_ANNIVERSARY_EVENT
	m_iAccountPVPLoseCount	= 0;
#endif //SERV_4TH_ANNIVERSARY_EVENT

#ifdef SERV_PVP_NEW_SYSTEM
	m_iRating				= 0;
	m_iRPoint				= 0;
	m_iAPoint				= 0;
#else
	m_PVPEmblem				= CX2PVPEmblem::PE_RECRUIT;
	m_VSPoint				= 0;
	m_VSPointMax			= 0;
#endif

#ifdef PVP_SEASON2
	m_cRank					= 0;
	m_iPastSeasonWin		= 0;
	m_fKFactor				= 0.f;
#endif

	m_Age					= 0;
	m_bMan					= true;

	m_Win					= 0;
	m_Lose					= 0;
	m_Seceder				= 0;

	m_NowBaseLevelEXP		= 0;
	m_NextBaseLevelEXP		= 0;

	//	m_InventorySize			= 0;


	//m_RemainStatPoint		= 0;
	m_iSPoint				= 0;
	//m_pInventory			= NULL;


	m_nStraightVictories	= 0;
	m_nMapID				= 0;

	m_bIsGameBang			= false;
	m_fAddExpRate			= 0.f;
	
	m_UserSkillTree.Reset( true, false, false, false );

#ifdef GUILD_SKILL	
	//{{ oasis907 : 김상윤 //// 2009-11-17 //// 
	m_UserSkillTree.Reset( true, true, false, false, false );	
	//}} oasis907 : 김상윤 //// 2009-11-17 //// 
#endif GUILD_SKILL
	m_bIsParty				= false;

	m_iSpirit				= 0;
	m_iSpiritMax			= 0;

#ifdef SERV_CHINA_SPIRIT_EVENT
	m_arrChinaSpirit[0]		= 0;
	m_arrChinaSpirit[1]		= 0;
	m_arrChinaSpirit[2]		= 0;
	m_arrChinaSpirit[3]		= 0;
	m_arrChinaSpirit[4]		= 0;
	m_arrChinaSpirit[5]		= 0;
#endif SERV_CHINA_SPIRIT_EVENT

//#ifdef TITLE_SYSTEM
    m_iTitleId              = 0;
//#endif

	//{{ 허상형 : [2009/9/25] //	길드 초기화
#ifdef GUILD_MANAGEMENT
	m_wstrGuildName = L"";
	m_byMemberShipGrade	= 0;
#endif	//	GUILD_MANAGEMENT
	//}} 허상형 : [2009/9/25] //	길드 초기화

	//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
#ifdef SERV_UNIT_WAIT_DELETE
	m_wstrLastDate			= L"";			
	m_bDeleted				= false;
	m_trDelAbleDate			= 0LL;			
	m_trRestoreAbleDate		= 0LL;		
	m_bDeletedMotionPlay	= false;// 삭제 모션 재생 중인지 //2012.03.07 lygan_조성욱 // 캐릭터 삭제 모션 제어용
#endif SERV_UNIT_WAIT_DELETE
	//}}

#ifdef SERV_INVISIBLE_GM
	m_bInvisible			= false;
#endif SERV_INVISIBLE_GM

#ifdef SERV_RELATIONSHIP_SYSTEM
	m_cWeddingStatus = 0;
	m_iLoverUnitUID = 0;
#endif //SERV_RELATIONSHIP_SYSTEM
#ifdef FIELD_BOSS_RAID
	m_bIgnoreLastTouch = false;
#endif // FIELD_BOSS_RAID

#ifdef SERV_GROW_UP_SOCKET
	m_mapGrowUpPoint.clear();
#endif
#ifdef SERV_GROW_UP_TITLE
	m_iOldLevel = 0;
#endif
#ifdef SERV_RELATIONSHIP_EVENT_INT
	m_bCouple = false;
#endif SERV_RELATIONSHIP_EVENT_INT
#ifdef SERV_ELESIS_UPDATE_EVENT
	m_iNoteViewCount = 0;
	m_bReserveShow = false;
#endif SERV_ELESIS_UPDATE_EVENT

#ifdef SERV_NEW_YEAR_EVENT_2014
	m_ucOldYearMissionRewardedLevel = 0;
	m_iNewYearMissionStepID = -1;
#endif SERV_NEW_YEAR_EVENT_2014

#ifdef SERV_EVENT_CHECK_POWER
	m_ucCheckPowerCount = 0;
	m_iCheckPowerTime = CTime( 2000, 1, 1, 0, 0, 0 ).GetTime();
	m_bCheckPowerShowPopUp = false;
	m_ucCheckPowerScore = 0;
#endif SERV_EVENT_CHECK_POWER

#ifdef SERV_EVENT_VALENTINE_DUNGEON_GIVE_ITEM
	m_iValentineItemCount = -1;
#endif SERV_EVENT_VALENTINE_DUNGEON_GIVE_ITEM
}



#if defined( SERV_IRUHADEV_JUNE_TRANSCEND_UNIT ) && defined( SERV_IRUHADEV_OFFLINE )
/// one line per unit, source and value ( 0 select, 1 room, 2 square, 3 field ), not one per refresh
static void LogTranscendUnit( const int iSource_, const UidType iUnitUID_, const int iClass_, const int iTran_ )
{
	static const WCHAR* s_wszSource[] = { L"select", L"room", L"square", L"field" };
	static std::map< __int64, int > s_mapLogged;

	const __int64 iKey = static_cast<__int64>( iUnitUID_ ) * 4 + iSource_;
	std::map< __int64, int >::iterator mit = s_mapLogged.find( iKey );
	if( mit != s_mapLogged.end() && mit->second == iTran_ )
		return;

	s_mapLogged[ iKey ] = iTran_;
	CX2OfflineLog::Server( L"[JUNE_TRANSCEND] UNIT %I64d class %d transcendence %d ( %s )", static_cast<__int64>( iUnitUID_ ), iClass_, iTran_, s_wszSource[ iSource_ ] );
}
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
void CX2Unit::UnitData::SetKUnitInfo( const KUnitInfo& data )
{
	m_UnitUID				= data.m_nUnitUID;
	m_UserUID				= data.m_iOwnerUserUID;
	m_UnitClass				= (UNIT_CLASS)data.m_cUnitClass;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	m_iUnitClassTran		= data.m_iUnitClassTran;
#ifdef SERV_IRUHADEV_OFFLINE
	LogTranscendUnit( 0, m_UnitUID, static_cast<int>( m_UnitClass ), m_iUnitClassTran );
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT

#ifdef SERV_4TH_ANNIVERSARY_EVENT
	m_iAccountPVPLoseCount = data.m_iAccountPVPLoseCount;
#endif //SERV_4TH_ANNIVERSARY_EVENT
	
#ifndef NEW_MESSENGER
	m_iNMKSerialNum			= data.m_uiKNMSerialNum;
#endif

	m_IP					= data.m_wstrIP;
	m_Port					= (int)data.m_usPort;

	m_NickName				= data.m_wstrNickName;

	m_ED					= data.m_iED;	
	m_Level					= data.m_ucLevel;
	m_EXP					= data.m_iEXP;	

#ifdef SERV_PVP_NEW_SYSTEM
	m_iRating				= data.m_iRating;
	m_iRPoint				= data.m_iRPoint;
	m_iAPoint				= data.m_iAPoint;
#else
	m_PVPEmblem				= (CX2PVPEmblem::PVP_EMBLEM)data.m_iPVPEmblem;
	m_VSPoint				= data.m_iVSPoint;
	m_VSPointMax			= data.m_iVSPointMax;
#endif

#ifdef PVP_SEASON2
	m_cRank					= data.m_cRank;
	m_iPastSeasonWin		= data.m_iPastSeasonWin;
	m_fKFactor				= data.m_fKFactor;
#endif

	m_Win					= data.m_iWin;
	m_Lose					= data.m_iLose;
	//m_Seceder				= data.m_iSeceder;


	m_NowBaseLevelEXP		= data.m_nNowBaseLevelEXP;
	m_NextBaseLevelEXP		= data.m_nNextBaseLevelEXP;

//	m_InventorySize			= data.m_usInventorySize;
	//m_RemainStatPoint		= data.m_ucRemainStatPoint;
	m_iSPoint				= data.m_iSPoint;


#ifdef NEW_SKILL_TREE
	m_iCSPoint				= data.m_iCSPoint;		// 가지고 있는 캐시 스킬 포인트
	//{{ 09.05.28 태완 : CSP 잘못 날아오는 데 대한 임시 예외처리.
	if(m_iCSPoint < 0 || m_iCSPoint > 500 )
	{
		ASSERT( !"Wrong CSP Point from server" );
		m_iCSPoint = 0;
	}
	//}}
#ifdef GUILD_SKILL
	// oasis907 : 김상윤 [2009.11.27] // 길드 스킬 포인트 관련, 이부분들 X2GuildManager의 EGS_GUILD_INFO_NOT에서 처리
	/*
	//{{ oasis907 : 김상윤 //// 2009-11-14 //// 임시
	m_iGuildSPoint			= 20;
	m_iGuildCSPoint			= 0;

	//}} oasis907 : 김상윤 //// 2009-11-14 //// 
	*/
#endif GUILD_SKILL

	m_wstrCSPointEndDate	= data.m_wstrCSPointEndDate;
#endif NEW_SKILL_TREE



	m_nStraightVictories	= data.m_nStraightVictories;
	m_nMapID				= data.m_kLastPos.m_iMapID;
#ifdef REMEMBER_LOGOUT_POSITION_TEST
	m_ucLastTouchLineIndex	= data.m_kLastPos.m_ucLastTouchLineIndex;
	m_usLastPosValue		= data.m_kLastPos.m_usLastPosValue;
#endif REMEMBER_LOGOUT_POSITION_TEST
#ifdef FIELD_BOSS_RAID
	m_bIgnoreLastTouch		= data.m_kLastPos.m_bIgnoreLastTouch;			// LastTouch를 사용하지 않고 m_iMapID에 해당하는 기본 위치를 사용하도록 알린다.
#endif // FIELD_BOSS_RAID
	m_bIsGameBang			= data.m_bIsGameBang;



	m_UserSkillTree.SetGuildClass( 0 );
	m_UserSkillTree.SetUnitClass( m_UnitClass );
#ifdef SERV_IRUHADEV_JUNE_SKILL_SLOT_10
	m_UserSkillTree.SetSlotStateFromVector( data.m_UnitSkillData.m_vecSkillSlotState );
#else SERV_IRUHADEV_JUNE_SKILL_SLOT_10
	m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) data.m_UnitSkillData.m_cSkillSlotBExpirationState );
	m_UserSkillTree.SetSkillSlotBEndDateString( data.m_UnitSkillData.m_wstrSkillSlotBEndDate );
#endif SERV_IRUHADEV_JUNE_SKILL_SLOT_10

	// 내 유닛이 아니라면 장착스킬과 패시브 스킬 정보로 부터 스킬트리를 만들어 낸다
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != data.m_nUnitUID )
		{
			m_UserSkillTree.SetEquippedSkill( data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecPassiveSkill, data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructGuildSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecGuildPassiveSkill );
		}
	}

	for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
	{
        m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
	}


	m_NowEqipItemUIDList.resize(0);


	std::map< int, KInventoryItemInfo >::const_iterator iM;
	for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
	{
		KInventoryItemInfo kInventorySlotInfo = (*iM).second;
		m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
	}

	for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
	{
		KInventoryItemInfo kInventorySlotInfo = (*iM).second;
		if ( kInventorySlotInfo.m_iItemUID > 0 )
		{
			m_NowEqipItemUIDList.push_back( kInventorySlotInfo.m_iItemUID );
			m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
            {
			    CX2Item::ItemData kItemData( kInventorySlotInfo );
			    m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID, kItemData );
            }
		}
	}

#ifdef SERV_SKILL_NOTE
	// 기술의 노트관련 등록된 메모설정
	m_UserSkillTree.SetEqipSkillMemo( data.m_UnitSkillData.m_vecSkillNote );	
#endif
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	/// June 3235eb20 @3235f9e0: the unit's active memos ( KUnitSkillData +0x88 +0x20, KJuneSkillExtraData's map, skill -> memo; KUnitInfo
	/// +0x1ce ) merge into the tree ( 32892410, the pairs not active yet ), for every unit, after the equipped items
	{
		const std::map< int, int >& mapActiveMemo = data.m_UnitSkillData.m_kSkillExtraData.m_mapIntInt;
		m_UserSkillTree.MergeActiveMemo( std::multimap< int, int >( mapActiveMemo.begin(), mapActiveMemo.end() ) );
	}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET


	m_bIsParty				= data.m_bIsParty;

	//{{ 2007. 10. 5  최육사  근성도 추가
	m_iSpirit				= data.m_iSpirit;
	m_iSpiritMax			= data.m_iSpiritMax;
	//}}

#ifdef SERV_CHINA_SPIRIT_EVENT
	m_arrChinaSpirit[0]		= data.m_arrChinaSpirit[0];
	m_arrChinaSpirit[1]		= data.m_arrChinaSpirit[1];
	m_arrChinaSpirit[2]		= data.m_arrChinaSpirit[2];
	m_arrChinaSpirit[3]		= data.m_arrChinaSpirit[3];
	m_arrChinaSpirit[4]		= data.m_arrChinaSpirit[4];
	m_arrChinaSpirit[5]		= data.m_arrChinaSpirit[5];
#endif SERV_CHINA_SPIRIT_EVENT

	m_Stat.SetKStat( data.m_kStat );
	m_GameStat.SetKStat( data.m_kGameStat );


//#ifdef TITLE_SYSTEM
#ifdef SERV_TITLE_DATA_SIZE
	m_iTitleId              = data.m_iTitleID;
#else
    m_iTitleId              = data.m_sTitleID;
#endif
//#endif

	//{{ 허상형 : [2009/9/25] //	길드 이름 추가
#ifdef GUILD_MANAGEMENT
	m_wstrGuildName			= data.m_kUserGuildInfo.m_wstrGuildName;
	m_byMemberShipGrade		= data.m_kUserGuildInfo.m_ucMemberShipGrade;
	m_iGuildHonorPoint		= data.m_kUserGuildInfo.m_iHonorPoint;
#endif	//	GUILD_MANAGEMENT
	//}} 허상형 : [2009/9/25] //	길드 이름 추가

	//SetPremiumBuffInfo( data.m_vecBuffInfo );		/// 구 버프 시스템

#ifdef BUFF_TEMPLET_SYSTEM
	//Room 퇴장시 능력치 초기화
	//SetPremiumBuffInfo( data.m_vecBuffInfo );
	ErasePremiumBuffInfo( m_vecPremiumBuffList );
	m_vecPremiumBuffList.resize(0);
#endif BUFF_TEMPLET_SYSTEM

	//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
#ifdef SERV_UNIT_WAIT_DELETE
	m_wstrLastDate			= data.m_wstrLastDate;			// 최종 Logout 타임
	m_bDeleted				= data.m_bDeleted;				// 삭제 대기 상태 구분
	m_trDelAbleDate		= data.m_trDelAbleDate;		// 삭제 가능 일시
	m_trRestoreAbleDate	= data.m_trRestoreAbleDate;	// 복구 가능 일시
#endif SERV_UNIT_WAIT_DELETE
	//}}

#ifdef SERV_ADD_WARP_BUTTON
	m_trWarpVipEndDate = data.m_trWarpVipEndData;	// 코보 VIP 끝나는 시간
#endif // SERV_ADD_WARP_BUTTON
	
#ifdef SERV_GROW_UP_SOCKET
#ifdef SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_TAG_POINT, m_iTagPoint, data.m_nUnitUID );
#endif SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_QUEST_CLEAR_COUNT, data.m_iEventQuestClearCount, data.m_nUnitUID );
	SetGrowUpPoint( GUT_EXCHANGE_COUNT, data.m_iExchangeCount, data.m_nUnitUID );
#endif//SERV_GROW_UP_SOCKET

#ifdef SERV_NEW_YEAR_EVENT_2014
	m_ucOldYearMissionRewardedLevel = data.m_ucOldYearMissionRewardedLevel;
	m_iNewYearMissionStepID = data.m_iNewYearMissionStepID;
#endif SERV_NEW_YEAR_EVENT_2014

#ifdef SERV_EVENT_CHECK_POWER
	m_ucCheckPowerCount = data.m_ucCheckPowerCount;
	m_iCheckPowerTime = data.m_iCheckPowerTime;
	m_bCheckPowerShowPopUp = data.m_bCheckPowerShowPopUp;
	m_ucCheckPowerScore = data.m_ucCheckPowerScore;
#endif SERV_EVENT_CHECK_POWER
}

void CX2Unit::UnitData::SetKRoomUserInfo( const KRoomUserInfo& data )
{
	m_UnitUID				= data.m_nUnitUID;
	m_UserUID				= data.m_iOwnerUserUID;

	m_bMan					= data.m_bMale;
	m_Age					= data.m_ucAge;

	m_UnitClass				= (UNIT_CLASS)data.m_cUnitClass;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	m_iUnitClassTran		= data.m_iUnitClassTran;
#ifdef SERV_IRUHADEV_OFFLINE
	LogTranscendUnit( 1, m_UnitUID, static_cast<int>( m_UnitClass ), m_iUnitClassTran );
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT

#ifdef SERV_4TH_ANNIVERSARY_EVENT
	m_iAccountPVPLoseCount = data.m_iAccountPVPLoseCount;
#endif //SERV_4TH_ANNIVERSARY_EVENT

	// 랭크정보
#ifdef SERV_PVP_NEW_SYSTEM
#ifdef PVP_SEASON2
	m_cRank					= data.m_cRank;
#else
	m_iRating				= data.m_iRating;
#endif
#else
	m_PVPEmblem				= (CX2PVPEmblem::PVP_EMBLEM)data.m_iPVPEmblem;
#endif

#ifndef NEW_MESSENGER
	m_iNMKSerialNum			= data.m_uiKNMSerialNum;
#endif NEW_MESSENGER

	m_IP					= data.m_wstrIP;
	m_Port					= (int)data.m_usPort;
	//{{ 2013. 1. 9	박세훈	Merge 공인IP 연결 실패시 내부IP로 시도( 박진웅 )
//#ifdef SERV_KTDX_RETRY_USING_INTERNAL_IP
	m_InternalIP			= data.m_wstrInternalIP;
	m_InternalPort			= data.m_usInternalPort;
//#endif SERV_KTDX_RETRY_USING_INTERNAL_IP
	//}}

	m_NickName				= data.m_wstrNickName;

	m_Level					= data.m_ucLevel;

	m_bIsGameBang			= data.m_bIsGameBang;
	//{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
#ifdef SERV_INTEGRATION
	m_fAddExpRate			= data.GetBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE );
#else
	m_fAddExpRate			= data.m_fAddPremiumExpRate;
#endif SERV_INTEGRATION
	//}}	

	m_UserSkillTree.SetGuildClass( 0 );
	m_UserSkillTree.SetUnitClass( m_UnitClass );
#ifdef SERV_IRUHADEV_JUNE_SKILL_SLOT_10
	m_UserSkillTree.SetSlotStateFromVector( data.m_UnitSkillData.m_vecSkillSlotState );
#else SERV_IRUHADEV_JUNE_SKILL_SLOT_10
	m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) data.m_UnitSkillData.m_cSkillSlotBExpirationState );
	m_UserSkillTree.SetSkillSlotBEndDateString( data.m_UnitSkillData.m_wstrSkillSlotBEndDate );
#endif SERV_IRUHADEV_JUNE_SKILL_SLOT_10


	// 내 유닛이 아니라면 장착스킬과 패시브 스킬 정보로 부터 스킬트리를 만들어 낸다
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != data.m_nUnitUID )
		{
			m_UserSkillTree.SetEquippedSkill( data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecPassiveSkill, data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructGuildSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecGuildPassiveSkill );

			for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
			{
                m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
			}

			m_NowEqipItemUIDList.resize(0);
			{
				std::map< int, KInventoryItemInfo >::const_iterator iM;
				for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
				{
					KInventoryItemInfo kInventorySlotInfo = (*iM).second;
					m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
				}

				for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
				{
					KInventoryItemInfo kInventorySlotInfo = (*iM).second;
					if ( kInventorySlotInfo.m_iItemUID > 0 )
					{
						m_NowEqipItemUIDList.push_back( kInventorySlotInfo.m_iItemUID );
						m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
                        {
                            CX2Item::ItemData kItemData( kInventorySlotInfo );
						    m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID, kItemData );
                        }
					}
				}
			}
		}
	}
	
	

#ifdef SERV_SKILL_NOTE
	// 기술의 노트관련 등록된 메모설정	
	m_UserSkillTree.SetEqipSkillMemo( data.m_UnitSkillData.m_vecSkillNote );
#endif
#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
	/// June 32360090 @323608a4: the same merge from the room user's skill data ( KRoomUserInfo +0x1a1 )
	{
		const std::map< int, int >& mapActiveMemo = data.m_UnitSkillData.m_kSkillExtraData.m_mapIntInt;
		m_UserSkillTree.MergeActiveMemo( std::multimap< int, int >( mapActiveMemo.begin(), mapActiveMemo.end() ) );
	}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

#ifndef SERV_DELETE_ROOM_USER_INFO_DATA
	//{{ 2007. 10. 5  최육사  근성도 추가
	m_iSpirit				= data.m_iSpirit;
	m_iSpiritMax			= data.m_iSpiritMax;
	//}}
#endif //SERV_DELETE_ROOM_USER_INFO_DATA

	m_GameStat.SetKStat( data.m_kGameStat );

//#ifdef TITLE_SYSTEM
#ifdef SERV_TITLE_DATA_SIZE
    m_iTitleId              = data.m_iTitleID;
#else
	m_iTitleId              = data.m_sTitleID;
#endif
//#endif

	//{{ 허상형 : [2009/9/25] //	길드 이름 추가
#ifdef GUILD_MANAGEMENT
	m_wstrGuildName	= data.m_wstrGuildName;
	m_byMemberShipGrade = data.m_ucMemberShipGrade;
#endif	//	GUILD_MANAGEMENT
	//}} 허상형 : [2009/9/25] //	길드 이름 추가

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
#ifdef SERV_RELATIONSHIP_SYSTEM
	m_cWeddingStatus = data.m_cWeddingStatus;
	m_iLoverUnitUID = data.m_iLoverUnitUID;
#endif //SERV_RELATIONSHIP_SYSTEM
	//}

#ifdef SERV_GROW_UP_SOCKET
#ifdef SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_TAG_POINT, m_iTagPoint, data.m_nUnitUID );
#endif SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_QUEST_CLEAR_COUNT, data.m_iEventQuestClearCount, data.m_nUnitUID );
	SetGrowUpPoint( GUT_EXCHANGE_COUNT, data.m_iExchangeCount, data.m_nUnitUID );
#endif//SERV_GROW_UP_SOCKET

#ifdef SERV_GATE_OF_DARKNESS_SUPPORT_EVENT
	m_iGateOfDarknessSupportEventTime = data.m_iGateOfDarknessSupportEventTime;
#endif SERV_GATE_OF_DARKNESS_SUPPORT_EVENT

#ifdef SERV_RELATIONSHIP_EVENT_INT
	m_bCouple = data.m_bCouple;
	m_iRelationTargetUserUid = data.m_iRelationTargetUserUid;
	m_wstrRelationTargetUserNickname = data.m_wstrRelationTargetUserNickname;
#endif SERV_RELATIONSHIP_EVENT_INT
}

void CX2Unit::UnitData::SetKSquareUserInfo( const KSquareUserInfo& pKSquareUserInfo )
{
	m_UnitUID				= pKSquareUserInfo.m_iUnitUID;
	m_UnitClass				= (UNIT_CLASS)pKSquareUserInfo.m_cUnitClass;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	m_iUnitClassTran		= pKSquareUserInfo.m_iUnitClassTran;
#ifdef SERV_IRUHADEV_OFFLINE
	LogTranscendUnit( 2, m_UnitUID, static_cast<int>( m_UnitClass ), m_iUnitClassTran );
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT

	m_IP					= pKSquareUserInfo.m_wstrIP;
	m_Port					= pKSquareUserInfo.m_usPort;

	m_NickName				= pKSquareUserInfo.m_wstrNickName;
	m_Level					= pKSquareUserInfo.m_ucLevel;

	m_bMan					= false; //pKSquareUserInfo.m_bMale;
	m_Age					= 0; //pKSquareUserInfo.m_ucAge;

#ifdef SERV_4TH_ANNIVERSARY_EVENT
	m_iAccountPVPLoseCount = pKSquareUserInfo.m_iAccountPVPLoseCount;
#endif //SERV_4TH_ANNIVERSARY_EVENT

	// 랭크정보
#ifdef SERV_PVP_NEW_SYSTEM
#ifdef PVP_SEASON2
	m_cRank					= pKSquareUserInfo.m_cRank;
#else
	m_iRating				= pKSquareUserInfo.m_iRating;
#endif
#else
	m_PVPEmblem				= g_pMain->GetPVPEmblem()->PvpEmblemEnumToPvpEmblem( (CX2PVPEmblem::PVP_EMBLEM_ENUM)pKSquareUserInfo.m_cPVPEmblem );
#endif

	m_bIsParty				= pKSquareUserInfo.m_bIsParty;

	for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
	{
        m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
	}

	m_NowEqipItemUIDList.resize(0);


	{
		for( int i = 0; i < (int)pKSquareUserInfo.m_vecEquippedItem.size(); i++ )
		{
			KInventoryItemSimpleInfo kInventoryItemSimpleInfo = pKSquareUserInfo.m_vecEquippedItem[i];

			if ( kInventoryItemSimpleInfo.m_iItemUID > 0 )
			{
				m_NowEqipItemUIDList.push_back( kInventoryItemSimpleInfo.m_iItemUID );
				CX2Item::ItemData kItemData;
				kItemData.m_ItemUID			= kInventoryItemSimpleInfo.m_iItemUID;
				kItemData.m_ItemID				= kInventoryItemSimpleInfo.m_iItemID;
				kItemData.m_EnchantLevel		= kInventoryItemSimpleInfo.m_EnchantLevel;

				kItemData.m_EnchantedAttribute.m_aEnchantedType[0]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant0;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[1]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant1;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[2]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant2;

				m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID );
				m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID, kItemData );
			}
		}
	}

//#ifdef TITLE_SYSTEM
#ifdef SERV_TITLE_DATA_SIZE
	m_iTitleId              = pKSquareUserInfo.m_iTitleID;
#else
    m_iTitleId              = pKSquareUserInfo.m_sTitleID;
#endif
//#endif	

	//{{ 허상형 : [2009/9/25] //	길드 이름 추가
#ifdef GUILD_MANAGEMENT
	m_wstrGuildName	= pKSquareUserInfo.m_wstrGuildName;
#endif	//	GUILD_MANAGEMENT
	//}} 허상형 : [2009/9/25] //	길드 이름 추가

#ifdef SERV_GROW_UP_SOCKET
#ifdef SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_TAG_POINT, m_iTagPoint, pKSquareUserInfo.m_iUnitUID);
#endif SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_QUEST_CLEAR_COUNT, pKSquareUserInfo.m_iEventQuestClearCount, pKSquareUserInfo.m_iUnitUID );
	SetGrowUpPoint( GUT_EXCHANGE_COUNT, pKSquareUserInfo.m_iExchangeCount, pKSquareUserInfo.m_iUnitUID );
#endif //SERV_GROW_UP_SOCKET
}


void CX2Unit::UnitData::SetKFieldUserInfo( const KFieldUserInfo& pKFieldUserInfo )
{
	m_UnitUID				= pKFieldUserInfo.m_iUnitUID;
	m_UnitClass				= (UNIT_CLASS)pKFieldUserInfo.m_cUnitClass;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	m_iUnitClassTran		= pKFieldUserInfo.m_iUnitClassTran;
#ifdef SERV_IRUHADEV_OFFLINE
	LogTranscendUnit( 3, m_UnitUID, static_cast<int>( m_UnitClass ), m_iUnitClassTran );
#endif SERV_IRUHADEV_OFFLINE
#endif SERV_IRUHADEV_JUNE_TRANSCEND_UNIT
	
	m_NickName				= pKFieldUserInfo.m_wstrNickName;
	m_Level					= pKFieldUserInfo.m_ucLevel;

#ifdef SERV_4TH_ANNIVERSARY_EVENT
	m_iAccountPVPLoseCount = pKFieldUserInfo.m_iAccountPVPLoseCount;
#endif //SERV_4TH_ANNIVERSARY_EVENT

	// 랭크정보
#ifdef SERV_PVP_NEW_SYSTEM
#ifdef PVP_SEASON2
	m_cRank					= pKFieldUserInfo.m_cRank;
#else
	m_iRating				= pKFieldUserInfo.m_iRating;
#endif
#else
	m_PVPEmblem				= g_pMain->GetPVPEmblem()->PvpEmblemEnumToPvpEmblem( (CX2PVPEmblem::PVP_EMBLEM_ENUM)pKFieldUserInfo.m_cPVPEmblem );
#endif

	// 필요없는 정보
	m_IP					= L"";
	m_Port					= 0;
	m_bMan					= false;
	m_Age					= 0;
	m_bIsParty				= false;


	for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
	{
        m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
	}

	m_NowEqipItemUIDList.resize(0);


	{
		for( int i = 0; i < (int)pKFieldUserInfo.m_vecEquippedItem.size(); i++ )
		{
			KInventoryItemSimpleInfo kInventoryItemSimpleInfo = pKFieldUserInfo.m_vecEquippedItem[i];

			if ( kInventoryItemSimpleInfo.m_iItemUID > 0 )
			{
				m_NowEqipItemUIDList.push_back( kInventoryItemSimpleInfo.m_iItemUID );
				CX2Item::ItemData kItemData;
				kItemData.m_ItemUID			= kInventoryItemSimpleInfo.m_iItemUID;
				kItemData.m_ItemID				= kInventoryItemSimpleInfo.m_iItemID;
				kItemData.m_EnchantLevel		= kInventoryItemSimpleInfo.m_EnchantLevel;


				kItemData.m_EnchantedAttribute.m_aEnchantedType[0]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant0;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[1]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant1;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[2]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant2;


				m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID );
				m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID, kItemData );
			}
		}
	}

//#ifdef TITLE_SYSTEM
#ifdef SERV_TITLE_DATA_SIZE
	m_iTitleId              = pKFieldUserInfo.m_iTitleID;
#else
	m_iTitleId              = pKFieldUserInfo.m_sTitleID;
#endif
//#endif

	//{{ 허상형 : [2009/9/25] //	길드 이름 추가
#ifdef GUILD_MANAGEMENT
	m_wstrGuildName	= pKFieldUserInfo.m_wstrGuildName;
#endif	//	GUILD_MANAGEMENT
	//}} 허상형 : [2009/9/25] //	길드 이름 추가

#ifdef SERV_INVISIBLE_GM
	m_bInvisible = pKFieldUserInfo.m_bInvisible;
#endif SERV_INVISIBLE_GM

#ifdef SERV_GROW_UP_SOCKET
#ifdef SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_TAG_POINT, m_iTagPoint, pKFieldUserInfo.m_iUnitUID );
#endif SERV_PVP_TAG_MODE_RECORD
	SetGrowUpPoint( GUT_QUEST_CLEAR_COUNT, pKFieldUserInfo.m_iEventQuestClearCount, pKFieldUserInfo.m_iUnitUID );
	SetGrowUpPoint( GUT_EXCHANGE_COUNT, pKFieldUserInfo.m_iExchangeCount, pKFieldUserInfo.m_iUnitUID );
#endif //SERV_GROW_UP_SOCKET
}


void CX2Unit::UnitData::Verify() const
{
	if( m_ED.Verify()							== false
#ifdef SERV_PVP_NEW_SYSTEM
		|| m_iRating.Verify()					== false
		|| m_iRPoint.Verify()					== false
		|| m_iAPoint.Verify()					== false
#else
		|| m_VSPoint.Verify()					== false
		|| m_VSPointMax.Verify()				== false
#endif
		|| m_Level.Verify()						== false
		|| m_EXP.Verify()						== false 
		|| m_NowBaseLevelEXP.Verify()			== false 
		|| m_NextBaseLevelEXP.Verify()			== false
		|| m_iSPoint.Verify()					== false 
#ifdef NEW_SKILL_TREE
		|| m_iCSPoint.Verify()					== false
#endif NEW_SKILL_TREE
		|| m_nStraightVictories.Verify()		== false 
		|| m_nMapID.Verify()					== false )
	{
		g_pKTDXApp->SetFindHacking( true );
	}

	m_Stat.Verify();
	m_GameStat.Verify();
}

void CX2Unit::UnitData::SetBlackList( vector<KChatBlackListUnit>& blackList )
{
	m_BlackList = blackList;
}

bool CX2Unit::UnitData::AddBlackList( KChatBlackListUnit blackList )
{
	for( int i = 0; i < (int)m_BlackList.size(); i++ )
	{
		KChatBlackListUnit kBlackListUnit = m_BlackList[i];
		if( kBlackListUnit.m_iUnitUID == blackList.m_iUnitUID )
			return false;
	}
	m_BlackList.push_back( blackList );
	return true;
}

bool CX2Unit::UnitData::RemoveBlackList( UidType unitUID )
{
	for( int i = 0; i < (int)m_BlackList.size(); i++ )
	{
		KChatBlackListUnit kBlackListUnit = m_BlackList[i];
		if( kBlackListUnit.m_iUnitUID == unitUID )
		{
			m_BlackList.erase( m_BlackList.begin() + i );
			return true;
		}
	}
	return false;
}

UidType CX2Unit::UnitData::GetBlackListUnitUID( const WCHAR* wszNickName ) const
{
	UidType unitUID = -1;
	for( UINT i=0; i<m_BlackList.size(); i++ )
	{
		if( 0 == m_BlackList[i].m_wstrNickName.compare( wszNickName ) )
		{
			unitUID = m_BlackList[i].m_iUnitUID;
			break;
		}
	}

	return unitUID;
}

#ifdef BUFF_TEMPLET_SYSTEM	
void CX2Unit::UnitData::SetPremiumBuffInfo( const vector<KBuffInfo>& vecBuffInfo_ )
{
	/*BOOST_FOREACH( const KBuffInfo& kBuffInfo, vecBuffInfo_ )		/// 구 버프 시스템
	{
		m_vecPremiumBuffList.push_back( kBuffInfo.m_iBuffID );
	}*/
}

void CX2Unit::UnitData::ErasePremiumBuffInfo( const vector<int>& vecBuffInfo_ )
{
	/*BOOST_FOREACH( const int& iBuffID, vecBuffInfo_ )				/// 구 버프 시스템
	{
		std::vector< int >::iterator vit = m_vecPremiumBuffList.begin();

		while( m_vecPremiumBuffList.end() != vit )
		{
			if( iBuffID  == *vit )
			{
				vit = m_vecPremiumBuffList.erase( vit );
			}
			else
			{
				++vit;
			}
		}
	}*/
}

#ifdef PVP_SEASON2
void CX2Unit::UnitData::ClearPremiumBuffInfo()
{
	m_vecPremiumBuffList.clear();
}
#endif
#endif BUFF_TEMPLET_SYSTEM 


/** @function : IsShowDungeonOnly
	@brief : 해당 옵션이 현재 등록되어야 하는 옵션인지 여부 확인
	@param : bDungeonOnlyItem_( pSocketData의 m_bDungeonOnly인자)
			 bShowDungeonOptionUI_ ( UI용 인자, 
									true 라면 현재 위치가 던전이 아니더라도 true)
*/
bool CX2Unit::IsShowDungeonOnly( bool bDungeonOnlyItem_, bool bShowDungeonOptionUI_ /*= false */)
{
	//UI용 옵션, 던전용 스탯을 보도록 설정했다면 무조건 보여주기
	if( true == bShowDungeonOptionUI_ )
		return true;

	//던전온리가 아니라면 무조건 true
	if( false == bDungeonOnlyItem_ ) 
		return true;

	//던전에서만 보여줘야 한다면, 지금이 던전 또는 필드인지 확인
	if( NULL == g_pX2Game )
		return true;

	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_DUNGEON:
	case CX2Game::GT_BATTLE_FIELD:
		return true;
		break;
	default:
		//던전온리 옵션인데, 던전 또는 필드가 아니라면 안보여줌
		return false;
		break;
	}
}
/** @function : IsShowPvpOnly
	@brief : 해당 옵션이 현재 등록되어야 하는 옵션인지 여부 확인
	@param : bDungeonOnlyItem_( pSocketData의 m_bPVPOnly인자)
			bShowPvpOptionUI_ ( UI용 인자, 
									true 라면 현재 위치가 던전이 아니더라도 true)
*/
bool CX2Unit::IsShowPvpOnly( bool bPvpOnlyItem_, bool bShowPvpOptionUI_ /*= false*/ )
{
	//UI용 옵션, 대전용 스탯을 보도록 설정했다면 무조건 보여주기
	if( true == bShowPvpOptionUI_ )
		return true;

	//대전온리가 아니라면 무조건 true
	if( false == bPvpOnlyItem_ ) 
		return true;

	//대전에서만 보여줘야 한다면, 지금이 대전인지 확인
	if( NULL == g_pX2Game )
		return false;

	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_PVP:
		return true;
		break;
	default:
		//대전온리 옵션인데, 대전이 아니라면 안보여줌
		return false;
		break;
	}
}

#ifdef RIDING_SYSTEM
/** @function 	: 
	@brief 		: 
*/
void CX2Unit::SetFullRidingPetInfo( const KRidingPetInfo& pRidingPetInfo_ )
{
	m_vecRidingPetInfo.clear();
	m_vecRidingPetInfo.push_back( pRidingPetInfo_ );
}

/** @function 	: 
	@brief 		: 
*/
KRidingPetInfo* CX2Unit::GetRidingPetInfo()
{
	if( m_vecRidingPetInfo.size() > 0 )
	{		
		return &m_vecRidingPetInfo[0];
	}

	return NULL;
}

#endif //RIDING_SYSTEM


#ifdef ADDED_RELATIONSHIP_SYSTEM
bool CX2Unit::IsManCharacter ()
{
	UNIT_CLASS ucMyClass = GetUnitData().m_UnitClass;

	if ( 
		ucMyClass == UC_ELSWORD_SWORDMAN ||
		ucMyClass == UC_RAVEN_FIGHTER ||
		ucMyClass == UC_CHUNG_IRON_CANNON ||
		ucMyClass == UC_ELSWORD_KNIGHT ||
		ucMyClass == UC_ELSWORD_MAGIC_KNIGHT ||
		ucMyClass == UC_RAVEN_OVER_TAKER ||
		ucMyClass == UC_RAVEN_SOUL_TAKER ||
		ucMyClass == UC_CHUNG_FURY_GUARDIAN ||
		ucMyClass == UC_CHUNG_SHOOTING_GUARDIAN ||
		ucMyClass == UC_ELSWORD_SHEATH_KNIGHT ||
		ucMyClass == UC_RAVEN_WEAPON_TAKER ||
		ucMyClass == UC_CHUNG_SHELLING_GUARDIAN ||
		ucMyClass == UC_ELSWORD_LORD_KNIGHT ||
		ucMyClass == UC_ELSWORD_RUNE_SLAYER ||
		ucMyClass == UC_RAVEN_BLADE_MASTER ||
		ucMyClass == UC_RAVEN_RECKLESS_FIST ||
		ucMyClass == UC_CHUNG_IRON_PALADIN ||
		ucMyClass == UC_CHUNG_DEADLY_CHASER ||
		ucMyClass == UC_ELSWORD_INFINITY_SWORD ||
		ucMyClass == UC_RAVEN_VETERAN_COMMANDER ||
		ucMyClass == UC_CHUNG_TACTICAL_TROOPER
		)
		return true;		// 남자 캐릭터면 참

	return false;			// 여자 캐릭터면 거짓
}
#endif // ADDED_RELATIONSHIP_SYSTEM


#ifdef UPGRADE_SKILL_SYSTEM_2013 // 김태환

/** @function	: ResetIncreaseSkillLevelByBuff
	@brief		: 버프로 인한 스킬 레벨 증가 효과 재설정 함수
*/
void CX2Unit::ResetIncreaseSkillLevelByBuff()
{
#ifdef SKILL_PAGE_SYSTEM // 해외팀 추가
	/// 스킬 페이지에 대한 스킬 목록이 없다면, 구문 중지
	if ( false == m_UnitData.m_UserSkillTree.IsEnableSkillAcquiredPage() )
		return;
#else
	const int iIncreaseSkillLevelByBuff = GetIncreaseSkillLevelByBuff();	/// 버프로 인한 스킬 레벨 증가 효과값
#endif SKILL_PAGE_SYSTEM

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>& mapSkillAcquired = m_UnitData.m_UserSkillTree.AccessMapSkillAcquired();

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>::iterator mit = mapSkillAcquired.begin();

	/// 버프는 스킬별로 레벨을 올리는 효과가 없기 때문에, 증가값을 스킬 모두에게 설정
	for ( mit; mit != mapSkillAcquired.end(); ++mit )
	{
		const int iIncreaseSkillLevelByBuff = GetIncreaseSkillLevelByBuff();	/// 배운 스킬의 스킬 레벨 증가값 반환

		mit->second.SetIncreaseSkillLevelByBuff( iIncreaseSkillLevelByBuff );
	}

	if ( NULL != g_pData && 
		NULL != g_pData->GetUIManager() && 
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, false, false );	/// 장비 스킬 업데이트
	}
}

/** @function	: GetIncreaseSkillLevelByBuff
	@brief		: 버프로 인한 스킬 레벨 증가 효과값 반환 함수
	@return		: 증가값
*/
int CX2Unit::GetIncreaseSkillLevelByBuff()
{
#ifdef SERV_IRUHADEV_JUNE_BUFF_PREMIUM
	/// June 323644a0: 0 in June's PvE game (IsPve 322cc1c0: game type 4, absent here); the game
	/// unit's own buffs when it exists (@323645e0, then straight to the return at @323645e4);
	/// otherwise the world-buff records. That is March's split (Phase 11 review: 11 read the
	/// record walk as unconditional; the assembly returns before it).
	if ( NULL != g_pX2Game && 4 == g_pX2Game->GetGameType() )
		return 0;

	int iIncreaseSkillLevelByBuff = 0;

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
		return g_pX2Game->GetMyUnit()->GetAddSkillLevelByBuff();

	BOOST_FOREACH( const WorldBuffRecordPair& kRecord, g_pData->GetMyUser()->GetSelectUnit()->GetWorldBuffRecord() )
	{
		CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( static_cast<UINT>( kRecord.first ) );

		if ( NULL != ptrBuffFactor )
		{
			const KBuffBehaviorFactor* pBehaviorFactor = NULL;

			ptrBuffFactor->GetBehaviorFactor( BBT_ADD_SKILL_LEVEL, &pBehaviorFactor );

			if ( NULL != pBehaviorFactor )
				iIncreaseSkillLevelByBuff += static_cast<int>( pBehaviorFactor->m_vecValues[0] );
		}
	}

	return iIncreaseSkillLevelByBuff;
#else // SERV_IRUHADEV_JUNE_BUFF_PREMIUM
	int iIncreaseSkillLevelByBuff = 0;

	/// 유닛 객체가 있다면, 유닛에게 적용된 버프의 효과 적용
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
	{
		iIncreaseSkillLevelByBuff += g_pX2Game->GetMyUnit()->GetAddSkillLevelByBuff();
	}
	/// 유닛 객체가 없다면, 월드 버프에서 검색 ( 마을 등에서는 월드 버프만 유지되어 있다.( 효과 없이 아이콘만 표시 ) )
	else
	{
#ifdef SET_WORLD_BUFF_AT_RESURRECTION // 김태환		서버에서 준 버프 정보를 버프 펙터 아이디로 저장하였기 때문에, 수정
		/// 월드 버프 저장 컨테이너
		std::vector<BUFF_FACTOR_ID> vecWorldBuffID = g_pData->GetMyUser()->GetSelectUnit()->GetWorldBuffFactorID();

		BOOST_FOREACH( BUFF_FACTOR_ID eAddSkillLevelByBuffID, vecWorldBuffID )
		{
			CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( eAddSkillLevelByBuffID );

			if ( NULL != ptrBuffFactor )
			{
				const KBuffBehaviorFactor* pBehaviorFactor = NULL;

				ptrBuffFactor->GetBehaviorFactor( BBT_ADD_SKILL_LEVEL, &pBehaviorFactor );		/// 스킬 레벨 증가 효과 검사

				if ( NULL != pBehaviorFactor )
					iIncreaseSkillLevelByBuff += static_cast<int>( pBehaviorFactor->m_vecValues[0] );	/// 설정된 레벨치 적용
			}
		}
#else // SET_WORLD_BUFF_AT_RESURRECTION
		/// 월드 버프 저장 컨테이너
		std::vector<CX2BuffFactorPtr> vecWorldBuff = g_pData->GetMyUser()->GetSelectUnit()->GetWorldBuffFactorPtr();

		BOOST_FOREACH( CX2BuffFactorPtr addSkillLevelByBuffPtr, vecWorldBuff )
		{
			if ( NULL != addSkillLevelByBuffPtr )
			{
				const KBuffBehaviorFactor* pBehaviorFactor = NULL;

				addSkillLevelByBuffPtr->GetBehaviorFactor( BBT_ADD_SKILL_LEVEL, &pBehaviorFactor );		/// 스킬 레벨 증가 효과 검사

				if ( NULL != pBehaviorFactor )
					iIncreaseSkillLevelByBuff += static_cast<int>( pBehaviorFactor->m_vecValues[0] );	/// 설정된 레벨치 적용
			}
		}
#endif // SET_WORLD_BUFF_AT_RESURRECTION
	}

	return iIncreaseSkillLevelByBuff;
#endif SERV_IRUHADEV_JUNE_BUFF_PREMIUM
}

/** @function	: ResetIncreaseSkillLevelBySocket
	@brief		: 소켓으로 인한 스킬 레벨 증가 효과 재설정 함수
*/
void CX2Unit::ResetIncreaseSkillLevelBySocket()
{
#ifdef SKILL_PAGE_SYSTEM // 해외팀 추가
	/// 스킬 페이지에 대한 스킬 목록이 없다면, 구문 중지
	if ( false == m_UnitData.m_UserSkillTree.IsEnableSkillAcquiredPage() )
		return;
#endif SKILL_PAGE_SYSTEM

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>& mapSkillAcquired = m_UnitData.m_UserSkillTree.AccessMapSkillAcquired();

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>::iterator mit = mapSkillAcquired.begin();

	for( ; mit != mapSkillAcquired.end(); ++mit )
	{
		const CX2SkillTree::SKILL_ID eSkillID = mit->first;		/// 스킬 아이디

		const int iIncreaseSkillLevel = GetIncreaseSkillLevelBySocket( eSkillID );		///	소켓으로 인한 추가 레벨값 설정

		mit->second.SetIncreaseSkillLevelBySocket( iIncreaseSkillLevel );	/// 스킬 레벨 정보
	}

	if ( NULL != g_pData->GetUIManager() && 
		 NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, false, false );	/// 장비 스킬 업데이트
	}
}

/** @function	: GetIncreaseSkillLevelBySocket
	@brief		: 소켓으로 인한 스킬 레벨 증가 효과값 반환 함수
	@param		: 스킬 아이디
	@return		: 증가값
*/
int CX2Unit::GetIncreaseSkillLevelBySocket( CX2SkillTree::SKILL_ID eSkillID )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetSocketItem() ||
		NULL == g_pData->GetTitleManager() )
		return 0;

	int iIncreaseSkillLevelBySocket = 0;
	/// 소켓에 레벨 증가 관련 효과가 있는지 검사
		
	BOOST_TEST_FOREACH( const UidType&, eqipUID, m_UnitData.m_NowEqipItemUIDList )		/// 장비중인 아이템을 모두 검사
	{
		CX2Item* pItem = m_UnitData.m_Inventory.GetItem( eqipUID );		/// 아이템 UID를 통해 아이템 객체 반환
		if( NULL == pItem )
			continue;

		const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();	/// 아이템 템플릿 반환
		if( NULL == pItemTemplet )
			continue;

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
#ifdef UPGRADE_SKILL_SYSTEM_2013
		/// 해당 아이템의 소켓 옵션 검사
		unsigned uNumSockOption = pItemTemplet->GetNumSocketOption();
		for( UINT j=0; j<uNumSockOption; j++ )
		{
			int socketOptionID = pItemTemplet->GetSocketOption(j);
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			/// 스킬 레벨 증가 효과로 인한 스킬 레벨 증가치 연산
			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}

		/// 아이템에 유저가 박은 소켓에 의한 소켓 옵션 검사
		for( UINT j=0; j < pItem->GetItemData().m_SocketOption.size(); j++ )
		{
			int socketOptionID = pItem->GetItemData().m_SocketOption[j];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}
#endif // UPGRADE_SKILL_SYSTEM_2013

		/// 감정을 통해 얻는 랜덤 소켓 옵션 검사
		BOOST_FOREACH( int iSocketID, pItem->GetItemData().m_vecRandomSocket )
		{
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
			if( NULL == pSocketData )
				continue;

			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}

	}

	/// 칭호에 레벨 증가 관련 효과가 있는지 검사
	const int iTitleID = m_UnitData.m_iTitleId;		/// 칭호 아이디

	if( NULL != g_pData->GetTitleManager()->GetTitleInfo( iTitleID ) )
	{
		const vector<int>& vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( iTitleID )->m_vecSocketOption;

		BOOST_FOREACH( int socketOptionID, vecTitleSocketOption )
		{
			const CX2SocketItem::SocketData*	pSocketData		= g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}
	}

	/// {{ 세트 효과에 의한 세트 옵션 검사
	map<int,int> mapSetIDNParts;

	g_pData->GetMyUser()->GetSelectUnit()->GetSetIDNPartsNum( mapSetIDNParts );

	if ( false == mapSetIDNParts.empty() )
	{
		map< int, int >::const_iterator mItr = mapSetIDNParts.begin();

		while ( mItr != mapSetIDNParts.end() )
		{	
			vector<int> vecOptions;

			// SetID 별로 Option들과 SetItem의 최대 적용 레벨을 얻어옴
			g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, vecOptions );

			// Option이 없으면, 검사 안해
			if ( false == vecOptions.empty() )
			{
				/// 옵션중 스킬 레벨 등가 효과가 있으면, 적용
				BOOST_FOREACH( int iSocketID, vecOptions )
				{
					const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
					if( NULL != pSocketData )
						g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
				}
			}

			++mItr;		/// iterator 증가
		}	
#endif // SERV_NEW_ITEM_SYSTEM_2013_05	
	}

	return iIncreaseSkillLevelBySocket;
}

#endif // UPGRADE_SKILL_SYSTEM_2013

#ifdef SERV_GROW_UP_SOCKET
void CX2Unit::UnitData::SetGrowUpPoint( GROW_UP_TYPE eType_, int const iPoint_, UidType nUnitUID )
{
	if( GUT_NONE == eType_ )
	{
		dbg::cerr << L"성장 포인트 타입이 없다" << dbg::endl;
	}
	else
	{
		std::map<GROW_UP_TYPE, int>::iterator mit = m_mapGrowUpPoint.find( eType_ );
		if( mit != m_mapGrowUpPoint.end() )
		{
			mit->second = iPoint_;
		}
		else
		{
			m_mapGrowUpPoint.insert( std::make_pair( eType_, iPoint_ ) );
		}

		// 단계가 변경되면  성장 아이템들 갱신하자.
		if( _CheckGrowUpNow(eType_) == true )
		{
			// 1. 타이틀 파티클
			// 타이틀은 UpdateEquippedEmblem 에서 메 프레임마다 갱신하고 있다.
			// 정보창의 타이틀은 string에 이미지 이름이 착용할때 저장해둔다. 이걸 변경하자
			if( NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() &&
				nUnitUID == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
				g_pData->GetTitleManager()->AttachTitle( m_iTitleId );
			// 2. 소켓
			// 장착 아이템 갱신해줘야 한다.
		}
	}
}

void CX2Unit::UnitData::IncreaseGrowUpPoint( GROW_UP_TYPE eType_, int const iPoint_, UidType iUnitUID_ )
{
	if( GUT_NONE == eType_ )
	{
		dbg::cerr << L"성장 포인트 타입이 없다" << dbg::endl;
	}
	else
	{
		std::map<GROW_UP_TYPE, int>::iterator mit = m_mapGrowUpPoint.find( eType_ );
		if( mit != m_mapGrowUpPoint.end() )
		{
			mit->second += iPoint_;

			// 단계가 변경되면  성장 아이템들 갱신하자.
			if( _CheckGrowUpNow(eType_) == true )
			{
				// 1. 타이틀 파티클
				// 타이틀은 UpdateEquippedEmblem 에서 메 프레임마다 갱신하고 있다.
				// 정보창의 타이틀은 string에 이미지 이름이 착용할때 저장해둔다. 이걸 변경하자
				if( NULL != g_pData->GetMyUser() &&
					NULL != g_pData->GetMyUser()->GetSelectUnit() &&
					iUnitUID_ == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
					g_pData->GetTitleManager()->AttachTitle( m_iTitleId );

			}
		}
		else
		{
			m_mapGrowUpPoint.insert(std::make_pair(eType_, iPoint_));
		}
	}
}

int CX2Unit::UnitData::GetGrowUpPoint( GROW_UP_TYPE eType_ )
{
	if( eType_ == GUT_NONE )
		return 0;

	std::map<GROW_UP_TYPE, int>::iterator mit = m_mapGrowUpPoint.find(eType_);
	if( mit != m_mapGrowUpPoint.end() )
	{
		return mit->second;
	}
	else
	{
		return 0;
	}
}

int CX2Unit::UnitData::GetGrowUpLevel( GROW_UP_TYPE eType_ )
{
	if( eType_ == GUT_NONE )
		return 0;

	std::map<GROW_UP_TYPE, int>::iterator mit = m_mapGrowUpPoint.find(eType_);
	if( mit != m_mapGrowUpPoint.end() )
	{
		switch( mit->first )
		{
		case GUT_TAG_POINT:
			{
				if( mit->second <= 9 )
					return 1;
				else if( mit->second <= 99 )
					return 2;
				else if( mit->second <= 499 )
					return 3;
				else if( mit->second <= 1999 )
					return 4;
				else
					return 5;
			}
			break;
		case GUT_QUEST_CLEAR_COUNT:
			{
				return mit->second + 1; // 횟수가 0 이면 1lv 소켓 옵션을 가진다.
			}
			break;
		case GUT_EXCHANGE_COUNT:
			{
				// 최대 레벨은 15. 이런식으로 만들면 안 됨. 나중에 제대로 수정해야 할 듯 by 박진웅
#ifdef SERV_2013_SILVER_WEEK_TITLE
				return min( mit->second + 1, 12 );
#else //SERV_2013_SILVER_WEEK_TITLE
				return min( mit->second + 1, 15 );
#endif //SERV_2013_SILVER_WEEK_TITLE
			}
			break;

		default:
			return 0;
			break;
		}
	}
	else
	{
		return 0;
	}
}

int CX2Unit::UnitData::GetGrowUpLevelBySocket( const int iType_ )
{
	GROW_UP_TYPE eGrowUpType = GUT_NONE;
	if( iType_ == static_cast<int>( CX2SocketItem::SDT_PVP_TAG_POINT ) )
	{
		eGrowUpType = GUT_TAG_POINT;
	}
	else if( iType_ == static_cast<int>( CX2SocketItem::SDT_QUEST_CLEAR_COUNT ) )
	{
		eGrowUpType = GUT_QUEST_CLEAR_COUNT;
	}
	else if( iType_ == static_cast<int>( CX2SocketItem::SDT_ITEM_EXCHANGE_COUNT ) )
	{
		eGrowUpType = GUT_EXCHANGE_COUNT;
	}
	else
		return 0;

	return GetGrowUpLevel( eGrowUpType );
}

int CX2Unit::UnitData::GetGrowUpLevelByTitle( const int iTitleID )
{
	const CX2TitleManager::TitleInfo* pTitleInfo = g_pData->GetTitleManager()->GetTitleInfo( iTitleID );

	if( NULL == pTitleInfo )
		return 0;

	int iLevel = 0;
	for each( const int iSocketID in pTitleInfo->m_vecSocketOption )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
		if( pSocketData == NULL ||
			g_pData->GetMyUser() == NULL ||
			g_pData->GetMyUser()->GetSelectUnit() == NULL )
			continue;

		// 설마 그럴리는 없겠지만 한 타이틀 안에 여러 종류의 레벨업 소켓을 넣었다면 그 중 더 높은 소켓의 레벨이 사용됨 by 박진웅
		iLevel = max( iLevel, g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().GetGrowUpLevelBySocket( pSocketData->m_Type ) );
	}

	return iLevel;
}

bool CX2Unit::UnitData::_CheckGrowUpNow( GROW_UP_TYPE eType_ )
{
	if( m_iOldLevel == 0)
	{
		m_iOldLevel = 1;
		return false;
	}
	else
	{
		int iNowLevel = GetGrowUpLevel( eType_ );
		if( m_iOldLevel < iNowLevel )
		{
			m_iOldLevel = iNowLevel;
			return true;
		}

		return false;
	}
}
#endif //SERV_GROW_UP_SOCKET

#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
/** @function : ApplySkillCoolTimeRateByBuff
	@brief : June buff port Phase 6b. June 3235a710 (a CX2Unit member: +0x494 the user skill tree, +0x28 the unit UID), called right after a cooldown is
		set from 16 places (CX2GUUser::ChangeEquippedSkillState 31fd8ff0, CX2User's 3264ad40, each class's skill-use function, vtable +0x800, and two
		class helpers). It reads the cooldown left back (32891460 -> 3288ff80, the same A[i]-then-B[i] search as GetSkillSlot; -1.0 when the skill is in
		no slot) and, when it is above 0: applies BBT 100 through the unit's CX2GUUser in the current game (32358770: g_pX2Game vtable +0xa0 by this UID,
		then 3203a420); multiplies by June's two cool-time rates; and writes the result (32893880) unless both rates are 1.0 and 100 did not apply.
		The two rates are not in March: 32358480 (a June skill-upgrade cool-time modifier) and max( 3234fbd0( skill ), 0.8 ) (the socket cool-time
		factor, which Phase 16 adds here). Without them the rate is 1.0, as June's would be with no upgrade and no socket option, so the write happens
		only when 100 applied. June passes the skill through 328905b0 first (the memo/base ID); March has no such mapping and uses the plain ID.
*/
void CX2Unit::ApplySkillCoolTimeRateByBuff( const CX2SkillTree::SKILL_ID eSkillID_ )
{
	CX2UserSkillTree& userSkillTree = AccessUnitData().m_UserSkillTree;
	const CX2UserSkillTree::SkillSlotData* pSkillSlot = userSkillTree.GetSkillSlot( eSkillID_ );
	float fCoolTime = ( NULL != pSkillSlot ? static_cast<float>( pSkillSlot->m_fCoolTimeLeft ) : -1.0f );
	if ( fCoolTime <= 0.0f )
		return;

	bool bAppliedByBuff = false;
	if ( NULL != g_pX2Game )
	{
		CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( GetUID() );
		if ( NULL != pUser )
			bAppliedByBuff = pUser->ApplySkillTypeCoolTimeRate( eSkillID_, fCoolTime );
	}

#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
	/// June buff port 16-step3b2 item 6: June @3235a71d..3235a74b. The socket factor is max( 3234fbd0( skill ), 0.8 ); the
	/// skill-upgrade modifier 32358480 is still not in March (1.0).
	const float fSocketFactor = GetSocketCoolTimeFactor( eSkillID_ );
	const float fCoolTimeRate = 1.0f * ( ( fSocketFactor < 0.8f ) ? 0.8f : fSocketFactor );
#else SERV_IRUHADEV_JUNE_SOCKET_PARITY
	const float fCoolTimeRate = 1.0f;	/// June: 32358480( skill ) * max( 3234fbd0( skill ), 0.8 ); neither is in March (see above)
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
	if ( 1.0f == fCoolTimeRate )
	{
		if ( true != bAppliedByBuff )
			return;
	}
	else
		fCoolTime = fCoolTimeRate * fCoolTime;

#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	static bool s_bSeenHost = false;
	if ( !s_bSeenHost )
	{
		s_bSeenHost = true;
		CX2BuffCapture::Line( "first host BBT 100 cool time: skill %d cool time left %g -> %g",
			static_cast<int>( eSkillID_ ), static_cast<float>( pSkillSlot->m_fCoolTimeLeft ), fCoolTime );
	}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	userSkillTree.SetSkillCoolTimeLeft( eSkillID_, fCoolTime );
}
#endif SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL
#ifdef SERV_IRUHADEV_JUNE_SOCKET_PARITY
/** @function : AddJuneSocketCoolTimeCut
	@brief : June 32355d70: map[ slot ] += cut, inserting the slot when it is missing (16-step3b2 item 6)
*/
void CX2Unit::AddSocketCoolTimeCut( const int iSlot_, const float fCut_ )
{
	std::map<int, float>::iterator mit = m_mapSocketCoolTimeCut.find( iSlot_ );
	if ( m_mapSocketCoolTimeCut.end() != mit )
		mit->second += fCut_;
	else
		m_mapSocketCoolTimeCut.insert( std::make_pair( iSlot_, fCut_ ) );
}

/** @function : GetJuneSocketCoolTimeFactor
	@brief : June 3234fbd0: the skill's slot (June's categories, JuneSkillCategory), then max( 1 - map[ slot ], 0 ), or 1.0
		when the skill has no templet or the slot is not in the map. June's SA grades 14/15/16 do not exist in March, so
		every SA is slot 2 and the 2nd / 3rd SA cuts (slots 3 / 4) never match (16-step3b2 item 6).
*/
float CX2Unit::GetSocketCoolTimeFactor( const CX2SkillTree::SKILL_ID eSkillID_ ) const
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID_ );
	if ( NULL == pSkillTemplet )
		return 1.0f;

	std::map<int, float>::const_iterator mit = m_mapSocketCoolTimeCut.find( SkillCategory( *pSkillTemplet ) );
	if ( m_mapSocketCoolTimeCut.end() == mit )
		return 1.0f;

	const float fFactor = 1.0f - mit->second;
#ifdef SERV_IRUHADEV_JUNE_BUFF_DEBUG
	if ( 1.0f != fFactor )
	{
		static bool s_bSeenFactor = false;
		if ( !s_bSeenFactor )
		{
			s_bSeenFactor = true;
			CX2BuffCapture::Line( "first socket cool-time factor: skill %d slot %d cut %g -> factor %g", static_cast<int>( eSkillID_ ), mit->first, mit->second, ( fFactor < 0.0f ) ? 0.0f : fFactor );
		}
	}
#endif SERV_IRUHADEV_JUNE_BUFF_DEBUG
	return ( fFactor < 0.0f ) ? 0.0f : fFactor;
}
#endif SERV_IRUHADEV_JUNE_SOCKET_PARITY
#ifdef SERV_LIMITED_DUNGEON_PLAY_TIMES
wstring CX2Unit::GetLocalMapPlayTimesDesc( int iDungeonID )
{
	std::map< int, KDungeonPlayInfo >::iterator mitDungeonPlay = m_mapDungeonPlay.find( iDungeonID );

	// 플레이한 횟수 획득
	int iUserPlayTimes = 0;
	int iUserClearTimes = 0;
	if( mitDungeonPlay != m_mapDungeonPlay.end() )
	{
		iUserPlayTimes = mitDungeonPlay->second.m_iPlayTimes;
		iUserClearTimes = mitDungeonPlay->second.m_iClearTimes;
	}

	// 플레이 가능한 횟수 획득
	int iLimitedPlayTimes = 0;
	int iLimitedClearTimes = 0;
	bool bLimitedPlayTimes = g_pData->GetDungeonManager()->GetLimitedPlayTimes( iDungeonID, iLimitedPlayTimes );
	bool bLimitedClearTimes = g_pData->GetDungeonManager()->GetLimitedClearTimes( iDungeonID, iLimitedClearTimes );

	// 스트링 만들기
	wstring wstrLimitedPlayTimes;
	if( bLimitedPlayTimes && bLimitedClearTimes )
	{
		wstrLimitedPlayTimes += GET_REPLACED_STRING( ( STR_ID_28236, "ii", iLimitedPlayTimes - iUserPlayTimes, bLimitedClearTimes - iUserClearTimes ) ) + std::wstring( L"\n" );
	}
	else if( bLimitedPlayTimes )
	{
		wstrLimitedPlayTimes += GET_REPLACED_STRING( ( STR_ID_28237, "i", iLimitedPlayTimes - iUserPlayTimes ) ) + std::wstring( L"\n" );
	}
	else if( bLimitedClearTimes )
	{
		wstrLimitedPlayTimes += GET_REPLACED_STRING( ( STR_ID_28238, "i", iLimitedClearTimes - iUserClearTimes ) ) + std::wstring( L"\n" );
	}

	return wstrLimitedPlayTimes;
}
#endif SERV_LIMITED_DUNGEON_PLAY_TIMES

#ifdef SERV_BALANCE_FINALITY_SKILL_EVENT
bool CX2Unit::IsInfinityElEssence() const
{
	if( NULL != GetInventory().GetItemByTID( INFINITY_EL_ESSENCE_ITEM_EVENT, false, false ) )
	{
		if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
			&& NULL != g_pData->GetPartyManager()
			&& NULL != g_pData->GetPartyManager()->GetMyPartyData()) 
		{
			if( true == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
				|| true == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
				return false;
		}

		return true;
	}

	return false;
}

float CX2Unit::GetSkillCoolTimeDecreaseRate(CX2SkillTree::SKILL_ID eSkillID, CX2SkillTree::SKILL_TYPE eSkillType) const
{
	float fDecreaseRate = 1.0f;

	if( CX2SkillTree::ST_HYPER_ACTIVE_SKILL == eSkillType )
	{
		if( NULL != GetInventory().GetItemByTID( FINALITY_SKILL_COOL_TIME_LEFT_ITEM_EVENT, false, false ) )
		{
			if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
				&& NULL != g_pData->GetPartyManager()
				&& NULL != g_pData->GetPartyManager()->GetMyPartyData()) 
			{
				if( false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
					&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
					fDecreaseRate = 0.7f;
			}
			else
				fDecreaseRate = 0.7f;
		}

		return fDecreaseRate;
	}

	switch(eSkillID)
	{
	case 202:
	case 204:
	case 1000:
	case 1002:
	case 1004:
	case 1007:
	case 1015:
	case 1200:
	case 1209:
	case 1210:
	case 1217:
	case 1222:
	case 1224:
	case 1402:
	case 1406:
	case 1408:
	case 2000:
	case 2007:
	case 2013:
	case 2014:
	case 2016:
	case 2017:
	case 2020:
	case 2021:
	case 2201:
	case 2203:
	case 2206:
	case 2212:
	case 2214:
	case 2217:
	case 2218:
	case 2219:
	case 2223:
	case 2415:
	case 3000:
	case 3001:
	case 3003:
	case 3005:
	case 3008:
	case 3011:
	case 3016:
	case 3017:
	case 3018:
	case 3201:
	case 3206:
	case 3207:
	case 3210:
	case 3218:
	case 3219:
	case 3220:
	case 3222:
	case 3224:
	case 3226:
	case 3227:
	case 3229:
	case 3400:
	case 3404:
	case 3410:
	case 3412:
	case 3413:
	case 4000:
	case 4002:
	case 4005:
	case 4009:
	case 4010:
	case 4013:
	case 4016:
	case 4017:
	case 4019:
	case 4021:
	case 4022:
	case 4200:
	case 4203:
	case 4213:
	case 4214:
	case 4215:
	case 4216:
	case 4218:
	case 4411:
	case 4412:
	case 4414:
	case 4417:
	case 5005:
	case 5008:
	case 5009:
	case 5011:
	case 5013:
	case 5015:
	case 5017:
	case 5024:
	case 5200:
	case 5202:
	case 5206:
	case 5214:
	case 5217:
	case 5407:
	case 6000:
	case 6001:
	case 6002:
	case 6006:
	case 6009:
	case 6010:
	case 6016:
	case 6020:
	case 6021:
	case 6200:
	case 6201:
	case 6206:
	case 6210:
	case 6213:
	case 6214:
	case 6218:
	case 6219:
	case 6222:
	case 6224:
	case 6225:
	case 6410:
	case 6413:
	case 7001:
	case 7006:
	case 7008:
	case 7010:
	case 7011:
	case 7012:
	case 7014:
	case 7200:
	case 7202:
	case 7203:
	case 7204:
	case 7206:
	case 7207:
	case 7212:
	case 7214:
	case 7216:
	case 7219:
	case 7408:
	case 7410:
	case 8003:
	case 8004:
	case 8006:
	case 8011:
	case 8016:
	case 8019:
	case 8202:
	case 8204:
	case 8205:
	case 8206:
	case 8209:
	case 8210:
	case 8213:
	case 8217:
	case 8218:
	case 8401:
	case 8402:
	case 8403:
	case 8404:
	case 8405:
	case 8406:
	case 8407:
	case 8408:
	case 8409:
	case 8600:
	case 8601:
		{
			if( NULL != GetInventory().GetItemByTID( BALANCE_SKILL_COOL_TIME_LEFT_ITEM_EVENT, false, false ) )
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
					&& NULL != g_pData->GetPartyManager()
					&& NULL != g_pData->GetPartyManager()->GetMyPartyData()) 
				{
					if( false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
						&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
						fDecreaseRate = 0.7f;
				}
				//else
				//	fDecreaseRate = 0.7f;
			}

			return fDecreaseRate;
		}
		break;
	}

	return fDecreaseRate;
}

float CX2Unit::GetSkillMpDecreaseRate(CX2SkillTree::SKILL_ID eSkillID, CX2SkillTree::SKILL_TYPE eSkillType) const
{
#ifdef SERV_IRUHADEV_JUNE_SKILL_MP_DECREASE_RATE
	// June 3234d840: the item's cut needs a dungeon game; March's 0.8 outside a dungeon is gone
	float fDecreaseRate = 1.0f;

	if( CX2SkillTree::ST_HYPER_ACTIVE_SKILL == eSkillType
		&& NULL != GetInventory().GetItemByTID( FINALITY_SKILL_COOL_ACTIVATE_ITEM_EVENT, false, false )
		&& g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
		&& NULL != g_pData->GetPartyManager()
		&& NULL != g_pData->GetPartyManager()->GetMyPartyData()
		&& false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) )
		&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ) )
		fDecreaseRate = 0.8f;

	return fDecreaseRate;
#else SERV_IRUHADEV_JUNE_SKILL_MP_DECREASE_RATE
	float fDecreaseRate = 1.0f;

	if( CX2SkillTree::ST_HYPER_ACTIVE_SKILL == eSkillType )
	{
		if( NULL != GetInventory().GetItemByTID( FINALITY_SKILL_COOL_ACTIVATE_ITEM_EVENT, false, false ) )
		{
			if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
				&& NULL != g_pData->GetPartyManager()
				&& NULL != g_pData->GetPartyManager()->GetMyPartyData()) 
			{
				if( false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
					&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
					fDecreaseRate = 0.8f;
			}
			else
				fDecreaseRate = 0.8f;
		}
	}

	return fDecreaseRate;
#endif SERV_IRUHADEV_JUNE_SKILL_MP_DECREASE_RATE
}
#endif //SERV_BALANCE_FINALITY_SKILL_EVENT