#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
#include "X2UIDungeonResultAdapter.h"
#include "X2StateDungeonGame.h"
#include <set>
#include <algorithm>

// Log tag [JUNE_RESULT] WIDGET ( ErrorLog.txt )
void ResultWidgetLog( const char* szFormat, ... )
{
	char szBuf[ 1024 ] = { 0, };
	va_list args;
	va_start( args, szFormat );
	_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
	va_end( args );
	KLOG( "ErrorLog.txt" ) << "[JUNE_RESULT] WIDGET " << szBuf << fileout;
}

// 32eea2c0 and the others read the current state as g_pMain +0x90 and dynamic_cast it to the dungeon state
static CX2StateDungeonGame* ResultNowState()
{
	if ( g_pMain == NULL )
		return NULL;
	return dynamic_cast< CX2StateDungeonGame* >( g_pMain->GetNowState() );
}

#ifdef SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
// 32eea2c0 / 32eea2e0 / 32eea300 take g_pMain +0x90 at call time and use it as a CX2State, no dungeon cast and no owner; 32eea320 / 32eea370 cast
static void ResultDispatch( int iID )
{
	CX2State* pState = ( g_pMain != NULL ) ? static_cast< CX2State* >( g_pMain->GetNowState() ) : NULL;
	switch ( iID )
	{
	case 0:
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "notifyCloseResultUI" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		if ( pState != NULL )
			pState->OpenExitBox( true );		// 31f1dd70
		break;
	case 1:
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "notfyRetryButton" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		if ( pState != NULL )
			pState->ResultRetry();				// vslot 0xe0
		break;
	case 2:
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "notifyInitialize" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		if ( pState != NULL )
			pState->SetResultUIMode( 2, 0.f );	// 31ef7be0
		break;
	case 3:
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "notfyitemSlotArrowLeft" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		if ( CX2StateDungeonGame* pDungeon = ResultNowState() )
			pDungeon->ResultItemPagePrev();		// 324d1270
		break;
	case 4:
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "notfyitemSlotArrowRight" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		if ( CX2StateDungeonGame* pDungeon = ResultNowState() )
			pDungeon->ResultItemPageNext();		// 324d1210
		break;
	}
}
#endif SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL

// ---------------------------------------------------------------------------------------------------------------------------------------
// CX2UIDungeonResultAdapter
// ---------------------------------------------------------------------------------------------------------------------------------------
CX2UIDungeonResultAdapter::CX2UIDungeonResultAdapter( CKTDGGFXManager* pManager )
	: m_pBridge( NULL )
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
	CKTDGGFXResultBridge::SetFunctionDispatcher( &ResultDispatch );
#endif SERV_IRUHADEV_JUNE_RESULT_NOW_STATE_CALL
	if ( pManager != NULL )
		m_pBridge = new CKTDGGFXResultBridge( pManager, this );
}

CX2UIDungeonResultAdapter::~CX2UIDungeonResultAdapter()
{
	SAFE_DELETE( m_pBridge );
}

bool CX2UIDungeonResultAdapter::IsBound() const
{
	return m_pBridge != NULL && m_pBridge->IsBound() == true;
}

void CX2UIDungeonResultAdapter::UpdateDungeonInfo( const KGfxResultInfo& kInfo )
{
	if ( m_pBridge != NULL )
		m_pBridge->UpdateDungeonInfo( kInfo );
}

void CX2UIDungeonResultAdapter::SetItem( int iIndex, int iItemID, int iNum, int iUIDLow, int iUIDHigh )
{
	if ( m_pBridge == NULL )
		return;

	KGfxResultItem kItem;
	kItem.m_iItemID		= iItemID;
	kItem.m_iNum		= iNum;
	kItem.m_iUIDLow		= iUIDLow;
	kItem.m_iUIDHigh	= iUIDHigh;
	if ( iItemID > 0 && g_pData != NULL && g_pData->GetItemManager() != NULL )		// 32eea6a0: 325bb740, then 328672d0 ( GetShopImage )
	{
		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( iItemID );
		if ( pItemTemplet != NULL )
			kItem.m_wstrIconPath = pItemTemplet->GetShopImage();
	}
	m_pBridge->SetItem( iIndex, kItem );
}

void CX2UIDungeonResultAdapter::ShowRetryButton( bool bShow )
{
	if ( m_pBridge != NULL )
		m_pBridge->ShowRetryButton( bShow );
}

void CX2UIDungeonResultAdapter::EnableRetryButton( bool bEnable )
{
	if ( m_pBridge != NULL )
		m_pBridge->EnableRetryButton( bEnable );
}

#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
void CX2UIDungeonResultAdapter::NotifyTabKey()
{
	if ( m_pBridge != NULL )
		m_pBridge->NotifyTabKey();
}
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW

void CX2UIDungeonResultAdapter::OnResultAsClose()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultWidgetLog( "notifyCloseResultUI" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState != NULL )
		pState->ResultOnClose();
}

void CX2UIDungeonResultAdapter::OnResultAsRetry()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultWidgetLog( "notfyRetryButton" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState != NULL )
		pState->ResultOnRetry();
}

void CX2UIDungeonResultAdapter::OnResultAsInitialize()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultWidgetLog( "notifyInitialize" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState != NULL )
		pState->ResultOnInitialize();
}

void CX2UIDungeonResultAdapter::OnResultAsArrowLeft()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultWidgetLog( "notfyitemSlotArrowLeft" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState != NULL )
		pState->ResultItemPagePrev();		// 324d1270
}

void CX2UIDungeonResultAdapter::OnResultAsArrowRight()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultWidgetLog( "notfyitemSlotArrowRight" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState != NULL )
		pState->ResultItemPageNext();		// 324d1210
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// CX2StateDungeonGame: the adapter owner ( June dungeon / training state vtables 3349f91c / 3349e02c, slots 0xb0-0xf0 identical )
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	const int JUNE_RESULT_UI_ID			= 8;		// GFx UI 8 = dungeon_result.gfx ( openUI 31c44530 )
	const int JUNE_RESULT_ITEM_SLOTS	= 12;

	// 324ddca0: the order of one category's map ( a unique-key std::map: an entry neither side of which is less is dropped )
	struct KResultEntryLess
	{
		bool operator()( const KGfxResultEntry& kA, const KGfxResultEntry& kB ) const
		{
			if ( kB.m_iCategory == 4 )
			{
				if ( kB.m_fDetailValue <= kA.m_fDetailValue )
				{
					if ( kB.m_fDetailValue < kA.m_fDetailValue )
						return false;
				}
				else
					return true;
			}
			else
			{
				if ( kA.m_fDetailValue <= kB.m_fDetailValue )
				{
					if ( kA.m_fDetailValue < kB.m_fDetailValue )
						return false;
				}
				else
					return true;
			}
			return kB.m_wstrName.compare( kA.m_wstrName ) < 0;		// @324ddce6: B.compare( A ) < 0
		}
	};

	float ResultClamp01( float fValue )		// 324d6ccb..: the two rates before 32eeae30 clamps them again
	{
		if ( fValue <= 1.0f )
		{
			if ( fValue < 0.0f )
				fValue = 0.0f;
		}
		else
			fValue = 1.0f;
		return fValue;
	}
}

// 324c8b80 ( vslot 0xb8 ): June does not free a previous adapter here; the callers go through JuneResultRecreateAdapter
void CX2StateDungeonGame::ResultCreateAdapter()
{
	CKTDGGFXManager* pManager = ( g_pKTDXApp != NULL ) ? g_pKTDXApp->GetGFXManager() : NULL;
	if ( pManager != NULL && pManager->IsMovieLoaded() == false )
		pManager = NULL;
	m_pJuneResultAdapter = new CX2UIDungeonResultAdapter( pManager );
}

// 324c8320 ( vslot 0xbc )
void CX2StateDungeonGame::ResultDeleteAdapter()
{
	if ( m_pJuneResultAdapter != NULL )
	{
		delete m_pJuneResultAdapter;
		m_pJuneResultAdapter = NULL;
	}
}

// 324c8c20 ( vslot 0xc4 )
void CX2StateDungeonGame::ResultCloseUI()
{
	if ( g_pKTDXApp != NULL && g_pKTDXApp->GetGFXManager() != NULL )
		g_pKTDXApp->GetGFXManager()->unloadWidget( JUNE_RESULT_UI_ID );		// 31c445d0
}

// 324c8350 ( vslot 0xc8 )
void CX2StateDungeonGame::ResultRecreateAdapter()
{
	ResultDeleteAdapter();
	ResultCreateAdapter();
}

// 324c8370 ( vslot 0xcc )
void CX2StateDungeonGame::ResultCloseAndDelete()
{
	ResultCloseUI();
	ResultDeleteAdapter();
}

// 324c83f0 ( vslot 0xe4 ): t > 0 is a cool-down ( stored, the tick 324c83a0 is not ported ), else the button state
void CX2StateDungeonGame::ResultEnableRetry( bool bEnable, float fCoolTime )
{
	if ( fCoolTime > 0.0f )
	{
		m_fJuneResultRetryCool = fCoolTime;
		return;
	}
	m_bJuneResultRetryEnabled = bEnable;
	if ( m_pJuneResultAdapter != NULL )
		m_pJuneResultAdapter->EnableRetryButton( bEnable );
}

// 324d1120: June's 31edf8f0( dungeon data, 0xe ) has no March home ( read false, as 3202d3c0 ); +0x7a9 is Stage 22's post-clear NPC flag ( never set offline )
bool CX2StateDungeonGame::ResultRetryAllowed()
{
	if ( g_pX2Game != NULL && g_pX2Game->GetGameType() != CX2Game::GT_DUNGEON )
		return false;
	if ( ResultNpcEventActive() == true )
		return false;

	// 31edf7d0( my unit UID ): the party manager, and when I have a party, only its leader
	if ( g_pData == NULL || g_pData->GetPartyManager() == NULL )
		return false;
	UidType iMyUnitUID = -1;
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
		iMyUnitUID = g_pData->GetMyUser()->GetSelectUnit()->GetUID();
	if ( g_pData->GetPartyManager()->DoIHaveParty() == true )
	{
		const CX2PartyManager::PartyMemberData* pMember = g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( iMyUnitUID );		// 32430e90
		if ( pMember == NULL || pMember->m_bPartyLeader == false )
			return false;
	}
	return true;
}

// 324cf8b0: the 12 item slots of the current page ( game vslot 0x220 ), the rest ( 0, 0, -1, -1 )
void CX2StateDungeonGame::ResultFillItems()
{
	if ( m_pJuneResultAdapter == NULL )
		return;

	std::vector< CX2Item::ItemData > vecItem;
	if ( m_pDungeonGame == NULL || m_pDungeonGame->GetObtainedItems( m_iJuneResultItemPage * JUNE_RESULT_ITEM_SLOTS, JUNE_RESULT_ITEM_SLOTS, vecItem ) == false )
		vecItem.clear();

	int iSlot = 0;
	for ( ; iSlot < (int)vecItem.size() && iSlot < JUNE_RESULT_ITEM_SLOTS; ++iSlot )
	{
		const CX2Item::ItemData& kItem = vecItem[ iSlot ];
		m_pJuneResultAdapter->SetItem( iSlot, kItem.m_ItemID, kItem.m_Quantity, (int)( kItem.m_ItemUID & 0xffffffff ), (int)( kItem.m_ItemUID >> 32 ) );
	}
	for ( ; iSlot < JUNE_RESULT_ITEM_SLOTS; ++iSlot )
		m_pJuneResultAdapter->SetItem( iSlot, 0, 0, -1, -1 );
}

// 324d1210 ( right ): one page on when game vslot 0x224( page * 12 + 13 ) says there is one
void CX2StateDungeonGame::ResultItemPageNext()
{
	if ( m_pDungeonGame != NULL && m_pDungeonGame->HasObtainedItemPage( m_iJuneResultItemPage * JUNE_RESULT_ITEM_SLOTS + 13 ) == true )
	{
		++m_iJuneResultItemPage;
		ResultFillItems();
	}
}

// 324d1270 ( left )
void CX2StateDungeonGame::ResultItemPagePrev()
{
	if ( m_iJuneResultItemPage != 0 )
	{
		--m_iJuneResultItemPage;
		ResultFillItems();
	}
}

// 324d60e0 ( vslot 0xc0 ): the result info -> updateDungeonInfo, then the game's item sort ( vslot 0x228 ) and the item fill.
// Not ported: the dialog-manager block at its start ( 31d05fe0 / 31d05ec0, two flag-walks over the dialog stack at g_pKTDXApp +0x1fc +0x38c ) -> Stage 9.
void CX2StateDungeonGame::ResultFill()
{
	CX2DungeonGame::DungeonResultInfo* pResultInfo = ( g_pData != NULL ) ? g_pData->GetDungeonResultInfo() : NULL;
	if ( pResultInfo == NULL || g_pX2Game == NULL || m_pJuneResultAdapter == NULL )		// June: vslot 0xf0( 0 ) ( the result confirm, R3 )
	{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "fill: %s", pResultInfo == NULL ? "no result info" : ( g_pX2Game == NULL ? "no game" : "no adapter" ) );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
		ResultSendConfirm( false );
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW
		return;
	}

	// the six category lists ( 324b3930 x 6 ), filled per unit below and pushed in order ( 324b54b0 x 6 )
	std::set< KGfxResultEntry, KResultEntryLess >	setCategory[ 6 ];
	const CX2DungeonGame::DungeonResultUnitInfo*			pMyResult	= NULL;
	int														iPlayerCount = 0;

	CX2GUUser* pMyGameUnit = g_pX2Game->GetMyUnit();
	const UidType iMyUnitUID = ( pMyGameUnit != NULL && pMyGameUnit->GetUnit() != NULL ) ? pMyGameUnit->GetUnit()->GetUID() : -1;

	for ( size_t i = 0; i < pResultInfo->m_DungeonResultUnitInfoList.size(); ++i )
	{
		const CX2DungeonGame::DungeonResultUnitInfo* pUnitResult = pResultInfo->m_DungeonResultUnitInfoList[ i ];
		if ( pUnitResult == NULL || pMyGameUnit == NULL )		// 31ed1ae0 ( my game unit ) null
			continue;

		std::wstring	wstrName;
		int				iUnitType = 0;
		bool			bIsMyUnit = false;
		if ( pUnitResult->m_UnitUID == iMyUnitUID )
		{
			if ( pMyGameUnit->GetUnit() != NULL )
			{
				wstrName	= pMyGameUnit->GetUnit()->GetNickName();
				iUnitType	= (int)pMyGameUnit->GetUnit()->GetType();
				bIsMyUnit	= true;
				pMyResult	= pUnitResult;
				// the bonus string lists ( 327cacf0( 2 / 1, 1..12 ) over the unit's June-only map +0xb4 ) stay empty: absent value 0
			}
		}
		else
		{
			CX2GUUser* pOtherGameUnit = g_pX2Game->GetUserUnitByUID( pUnitResult->m_UnitUID );
			if ( pOtherGameUnit != NULL && pOtherGameUnit->GetUnit() != NULL )
			{
				wstrName	= pOtherGameUnit->GetUnit()->GetNickName();
				iUnitType	= (int)pOtherGameUnit->GetUnit()->GetType();
			}
		}

		// categories 0..5: TotalScore, [0x26], ComboScore, TechnicalScore, [0x25], [0x24] / [0x23] * 100 ( the June-only ones read 0 )
		const float afDetail[ 6 ] =
		{
			(float)pUnitResult->m_nTotalScore, 0.0f, (float)pUnitResult->m_nComboScore, (float)pUnitResult->m_nTechnicalScore, 0.0f, 0.0f,
		};
		for ( int iCategory = 0; iCategory < 6; ++iCategory )
		{
			KGfxResultEntry kEntry;
			kEntry.m_bIsMyUnit		= bIsMyUnit;
			kEntry.m_wstrName		= wstrName;
			kEntry.m_iCategory		= iCategory;
			kEntry.m_fDetailValue	= afDetail[ iCategory ];
			kEntry.m_iUnitType		= iUnitType;
			setCategory[ iCategory ].insert( kEntry );
		}
		++iPlayerCount;
	}

	if ( pMyResult == NULL || g_pData->GetMyUser() == NULL || g_pData->GetMyUser()->GetSelectUnit() == NULL )		// 324d6ccb: the update needs my own row
	{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		ResultWidgetLog( "fill: no row for my unit ( %d units )", iPlayerCount );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		return;
	}

	const CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();
	const CX2Unit::UnitData& kUnitData = pMyUnit->GetUnitData();

	// the exp bar: ( EXP - base ) / range, range = the next level's need ( the ExpTable row, 32741030 )
	const int iLevel = kUnitData.m_Level;
	const float fBase = (float)(int)kUnitData.m_NowBaseLevelEXP;
	const float fRange = ( g_pData->GetEXPTable() != NULL ) ? (float)g_pData->GetEXPTable()->GetEXPData( iLevel + 1 ).m_nNeedExp : 0.0f;
	float fExpRate = ResultClamp01( ( (float)(int)kUnitData.m_EXP - fBase ) / fRange );
	const float fOldExp = (float)pMyResult->m_nOldEXP;
	float fOldExpRate = ResultClamp01( ( fOldExp - fBase ) / fRange );

	// June caps the gain at the level ED limit ( 328b1880, GameSysVal ) less the start ED; March has no limit table, so June's no-table path
	const long long llED = pMyResult->m_nED;

	KGfxResultInfo kInfo;
	kInfo.m_iTotalRank		= (int)pMyResult->m_cTotalRank;
	kInfo.m_iSec			= pResultInfo->m_Sec;
	kInfo.m_iBestSec		= 0;										// resultInfo +0x2c: June-only, no writer
	kInfo.m_iED				= (int)llED;
	kInfo.m_iHitRank		= 0;										// unit [0x20]: June-only
	kInfo.m_iComboRank		= (int)pMyResult->m_cComboRank;
	kInfo.m_iTechnicalRank	= (int)pMyResult->m_cTechnicalRank;
	kInfo.m_iBeAttackedRank	= (int)pMyResult->m_cDamagedRank;
	kInfo.m_iTotalEXP		= (int)(float)const_cast< CX2DungeonGame::DungeonResultUnitInfo* >( pMyResult )->GetTotalExp();	// 327c9390, through a float as June ( JUNE_BUGS 407 )
	kInfo.m_fExpRate		= fExpRate;
	kInfo.m_fOldExpRate		= fOldExpRate;
	kInfo.m_bLevelUp		= ( fOldExp < fBase );
	kInfo.m_iLevel			= iLevel;
	kInfo.m_bShowExitButton	= ( ResultNpcEventActive() == false );
	kInfo.m_iResonanceLevel	= 0;										// 3247b460: June-only system
	kInfo.m_iUnitType		= (int)pMyUnit->GetType();
	kInfo.m_iPlayerCount	= iPlayerCount;
	kInfo.m_iRewardType		= 0;										// unit [0x29]: June-only
	kInfo.m_bReTryButtonShow = ResultRetryAllowed();
	kInfo.m_vecCategory.resize( 6 );
	for ( int iCategory = 0; iCategory < 6; ++iCategory )
		kInfo.m_vecCategory[ iCategory ].assign( setCategory[ iCategory ].begin(), setCategory[ iCategory ].end() );

	m_pJuneResultAdapter->UpdateDungeonInfo( kInfo );
	if ( m_pDungeonGame != NULL )
		m_pDungeonGame->SortObtainedItems();		// game vslot 0x228
	ResultFillItems();

#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultWidgetLog( "fill: %d players, my rank %d, ED %d, EXP %d ( rate %.3f, old %.3f ), level %d%s, retry %d, obtained items %d",
		iPlayerCount, kInfo.m_iTotalRank, kInfo.m_iED, kInfo.m_iTotalEXP, kInfo.m_fExpRate, kInfo.m_fOldExpRate, kInfo.m_iLevel, kInfo.m_bLevelUp ? " ( level up )" : "",
		kInfo.m_bReTryButtonShow ? 1 : 0, m_pDungeonGame != NULL ? m_pDungeonGame->GetObtainedItemCount() : 0 );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
}

bool CX2StateDungeonGame::ResultIsBound() const
{
	return m_pJuneResultAdapter != NULL && m_pJuneResultAdapter->IsBound() == true;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// CX2DungeonGame: the obtained-item list ( June +0xdf4 / +0xdf8, 0xc0 B records = CX2Item::ItemData: UID +0, id +8, period type +0xc, quantity +0x14 )
// ---------------------------------------------------------------------------------------------------------------------------------------
// 327cedf0 ( vslot 0x21c ): a negative quantity is dropped; a quantity item ( period type 2 ) adds to the record with its id, any other item
// is dropped when its UID is already listed; else it is appended
void CX2DungeonGame::AddObtainedItem( const CX2Item::ItemData& kItemData )
{
	if ( kItemData.m_Quantity < 0 )
		return;

	bool bFound = false;
	for ( size_t i = 0; i < m_vecJuneObtainedItem.size(); ++i )
	{
		CX2Item::ItemData& kListed = m_vecJuneObtainedItem[ i ];
		if ( kItemData.m_PeriodType == CX2Item::PT_QUANTITY )
		{
			if ( kListed.m_ItemID == kItemData.m_ItemID )
			{
				kListed.m_Quantity += kItemData.m_Quantity;
				bFound = true;
			}
		}
		else if ( kListed.m_ItemUID == kItemData.m_ItemUID )
			bFound = true;
	}
	if ( bFound == false )
		m_vecJuneObtainedItem.push_back( kItemData );
}

// 327cee60 ( vslot 0x220 ): [ start, start + count ) of the list into vecOut ( cleared first ); false for an empty list or a start past the end
bool CX2DungeonGame::GetObtainedItems( unsigned uStart, unsigned uCount, std::vector< CX2Item::ItemData >& vecOut ) const
{
	if ( m_vecJuneObtainedItem.empty() == true )
		return false;
	if ( uStart >= m_vecJuneObtainedItem.size() )
		return false;

	vecOut.clear();
	unsigned uEnd = (unsigned)m_vecJuneObtainedItem.size();
	if ( uCount + uStart < uEnd )
		uEnd = uCount + uStart;
	for ( unsigned i = uStart; i < uEnd; ++i )
		vecOut.push_back( m_vecJuneObtainedItem[ i ] );
	return true;
}

// 327ca2f0 ( vslot 0x224 )
bool CX2DungeonGame::HasObtainedItemPage( unsigned uCount ) const
{
	return m_vecJuneObtainedItem.size() >= uCount;
}

// 327cef20 ( vslot 0x228 ): June's std::sort ( 327d7e30 ) by item grade; unstable above 32 items, a templet-less item never sorts first ( JUNE_BUGS 405 )
namespace
{
	struct KObtainedItemLess
	{
		bool operator()( const CX2Item::ItemData& kA, const CX2Item::ItemData& kB ) const
		{
			const CX2Item::ItemTemplet* pA = g_pData->GetItemManager()->GetItemTemplet( kA.m_ItemID );
			const CX2Item::ItemTemplet* pB = g_pData->GetItemManager()->GetItemTemplet( kB.m_ItemID );
			if ( pA == NULL || pB == NULL )
				return false;
			return (int)pA->GetItemGrade() < (int)pB->GetItemGrade();
		}
	};
}

void CX2DungeonGame::SortObtainedItems()
{
	if ( m_vecJuneObtainedItem.empty() == true || g_pData == NULL || g_pData->GetItemManager() == NULL )
		return;
	std::sort( m_vecJuneObtainedItem.begin(), m_vecJuneObtainedItem.end(), KObtainedItemLess() );
}

// notifyCloseResultUI 32eea2c0: June opens the exit box ( 31f1dd70( 1 ) ), R3. In the dev console the widget just closes.
void CX2StateDungeonGame::ResultOnClose()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
	OpenExitBox( true );
#else SERV_IRUHADEV_JUNE_RESULT_FLOW
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultCloseUI();
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW
}

// notfyRetryButton 32eea2e0: June calls state vslot 0xe0 ( the retry request ), R2 / R3
void CX2StateDungeonGame::ResultOnRetry()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
	ResultRetry();
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW
}

// notifyInitialize 32eea300: June sets the result-UI mode 2 ( 31ef7be0( 2, 0.0 ) ), whose tick runs the fill ( R3 ). In the dev console the fill runs here.
void CX2StateDungeonGame::ResultOnInitialize()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
	SetResultUIMode( 2, 0.f );
#else SERV_IRUHADEV_JUNE_RESULT_FLOW
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	ResultFill();
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW
}

#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
// ---------------------------------------------------------------------------------------------------------------------------------------
// R3: the in-state result flow ( CX2State's result-UI mode and exit box, the dungeon state's mode machine and vslots 0xd0-0xf0 )
// ---------------------------------------------------------------------------------------------------------------------------------------
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
// Log tag [JUNE_RESULT] FLOW ( ErrorLog.txt )
static void ResultFlowLog( const char* szFormat, ... )
{
	char szBuf[ 1024 ] = { 0, };
	va_list args;
	va_start( args, szFormat );
	_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
	va_end( args );
	KLOG( "ErrorLog.txt" ) << "[JUNE_RESULT] FLOW " << szBuf << fileout;
}
#define JUNE_RESULT_FLOW_LOG( args ) ResultFlowLog args
#else SERV_IRUHADEV_JUNE_RESULT_DEBUG
#define JUNE_RESULT_FLOW_LOG( args )
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG

static bool ResultDlgAlive( CKTDGUIDialogType pDialog )
{
	return pDialog != NULL && g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( pDialog ) == true;
}

// 31edf7d0: true without a party manager in a party, or as its leader; false without a party manager
static bool ResultIsSoloOrLeader()
{
	if ( g_pData == NULL || g_pData->GetPartyManager() == NULL )
		return false;
	if ( g_pData->GetPartyManager()->DoIHaveParty() == false )
		return true;
	if ( g_pData->GetMyUser() == NULL || g_pData->GetMyUser()->GetSelectUnit() == NULL )
		return false;
	const CX2PartyManager::PartyMemberData* pMember = g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( g_pData->GetMyUser()->GetSelectUnit()->GetUID() );	// 32430e90
	return pMember != NULL && pMember->m_bPartyLeader == true;
}

// 31ee3a40: two June-only id ranges are named as dungeon 120000
static std::wstring ResultDungeonName( int iDungeonID, int iDifficulty, int iMode )
{
	if ( g_pData->GetDungeonManager() == NULL )
		return L"";
	if ( 100000 <= iDungeonID && iDungeonID <= 100532 )
	{
		iDifficulty = 0;
		iDungeonID = 120000;
	}
	else if ( 110000 < iDungeonID && iDungeonID <= 110310 )
	{
		iDifficulty = 1;
		iDungeonID = 120000;
	}
	return g_pData->GetDungeonManager()->MakeDungeonNameString( (SEnum::DUNGEON_ID)iDungeonID, (CX2Dungeon::DIFFICULTY_LEVEL)iDifficulty, (CX2Dungeon::DUNGEON_MODE)iMode );
}

static CX2Main::TimedMessagePopUp::TimedPopupUserData ResultMyPopupData()
{
	CX2Main::TimedMessagePopUp::TimedPopupUserData kUserData;
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
		kUserData.iMyUID = g_pData->GetMyUser()->GetSelectUnit()->GetUID();		// 31edf090
	return kUserData;
}

// 31f2d850: the minimap dialog's "mini" control
void CX2MiniMapUI::SetShowMiniControl( bool bShow )
{
	if ( m_pDLGFrameWindow == NULL )
		return;
	CKTDGUIControl* pControl = m_pDLGFrameWindow->GetControl( L"mini" );
	if ( pControl != NULL )
		pControl->SetShow( bShow );
}

// 31f2d8b0
bool CX2MiniMapUI::GetShowMiniControl()
{
	if ( m_pDLGFrameWindow == NULL )
		return false;
	CKTDGUIControl* pControl = m_pDLGFrameWindow->GetControl( L"mini" );
	return pControl != NULL && pControl->GetShow() == true;
}

// 31ef7be0: +0x214 is not reset; mode 3 hides the minimap, mode 0 shows it back
void CX2State::SetResultUIMode( int iMode, float fTime )
{
	m_iJuneResultMode = iMode;
	CX2MiniMapUI* pMiniMap = ( g_pInstanceData != NULL ) ? g_pInstanceData->GetMiniMapUI() : NULL;
	if ( iMode == 3 )
	{
		if ( pMiniMap != NULL )
			pMiniMap->SetShowMiniControl( false );
	}
	else if ( iMode == 0 && pMiniMap != NULL && pMiniMap->GetShowMiniControl() == false )
		pMiniMap->SetShowMiniControl( true );

	if ( 0.f < fTime )
		m_fJuneResultModeLimit = fTime;
	JUNE_RESULT_FLOW_LOG(( "mode %d ( time %.2f, limit %.1f )", iMode, m_fJuneResultModeTime, m_fJuneResultModeLimit ));
}

// 31ef7390 ( vslot 0xd4 base )
void CX2State::ResultModeFrameMove( float fElapsedTime )
{
	m_fJuneResultModeTime += fElapsedTime;
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	// R3b: what the hide path sees while the exit box is up, once a second
	static float s_fJuneExitBoxLogTime = 0.f;
	s_fJuneExitBoxLogTime += fElapsedTime;
	if ( ResultDlgAlive( m_pDLGJuneExitBox ) == true && 1.f <= s_fJuneExitBoxLogTime )
	{
		s_fJuneExitBoxLogTime = 0.f;
		CKTDGUIDialogManager* pDialogManager = g_pKTDXApp->GetDGManager()->GetDialogManager();
		CKTDGUIDialogType pByName = pDialogManager->GetDialog( m_pDLGJuneExitBox->GetName() );
		char szName[ 128 ] = { 0, };
		WideCharToMultiByte( CP_ACP, 0, m_pDLGJuneExitBox->GetName(), -1, szName, sizeof( szName ) - 1, NULL, NULL );
		JUNE_RESULT_FLOW_LOG(( "exit box check: name %s, shown %d, found by name %d, hide mode %d, manager shown %d, hidden %d, flagged-only %d, layer %d, front %d",
			szName, m_pDLGJuneExitBox->GetShow() ? 1 : 0, ( pByName == m_pDLGJuneExitBox ) ? 1 : 0, pDialogManager->GetHideMode(),
			pDialogManager->GetShow() ? 1 : 0, pDialogManager->GetHideDialog() ? 1 : 0, pDialogManager->GetFlaggedDialogsOnly() ? 1 : 0,
			m_pDLGJuneExitBox->GetLayer(), m_pDLGJuneExitBox->GetFront() ? 1 : 0 ));
	}
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
}

// 31ef7c60
bool CX2State::IsExitBoxShown()
{
	return ResultDlgAlive( m_pDLGJuneExitBox ) == true && m_pDLGJuneExitBox->GetShow() == true;
}

// 31f1dd70: a party gets DLG_UI_ExitPartyMsgBox.lua, a solo player an OK / Cancel box; false sends the box's close event
void CX2State::OpenExitBox( bool bShow )
{
	if ( g_pMain == NULL || g_pMain->IsThereTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST ) == true )
		return;

	CKTDGUIDialogManager* pDialogManager = g_pKTDXApp->GetDGManager()->GetDialogManager();
	if ( bShow == false )
	{
		if ( ResultDlgAlive( m_pDLGJuneExitBox ) == true )
			g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, m_pDLGJuneExitBox->GetCloseCustomUIEventID(), NULL, true );
		return;
	}

	if ( pDialogManager->IsHideModeKeepingHidden() == false )		// 31d03a10
		return;
	CX2State* pNowState = static_cast< CX2State* >( g_pMain->GetNowState() );
	if ( pNowState == NULL || pNowState->ResultNpcEventActive() == true )
		return;

	const bool bParty = ( g_pData->GetPartyManager() == NULL ) ? true : g_pData->GetPartyManager()->DoIHaveParty();
	if ( ResultDlgAlive( m_pDLGJuneExitBox ) == false )
	{
		if ( bParty == false )
		{
			m_pDLGJuneExitBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( 39778 ), SUCM_QUIT_BUTTON_IN_RESULT_STATE_MSG_BOX,
				pNowState, SUCM_CANCEL_BUTTON_IN_RESULT_STATE_MSG_BOX );
			if ( ResultDlgAlive( m_pDLGJuneExitBox ) == true )
				m_pDLGJuneExitBox->SetCloseCustomUIEventID( SUCM_CANCEL_BUTTON_IN_RESULT_STATE_MSG_BOX );
		}
		else
		{
			m_pDLGJuneExitBox = new CKTDGUIDialog( pNowState, L"DLG_UI_ExitPartyMsgBox.lua", 0.07f );
			pDialogManager->AddDlg( m_pDLGJuneExitBox );		// 31d04390; the message box is already in the manager
		}
		pDialogManager->SetFlaggedDialogsOnly( true );		// @31f1e108
	}

	if ( ResultDlgAlive( m_pDLGJuneExitBox ) == true )
	{
		m_pDLGJuneExitBox->SetShow( bShow );
		m_pDLGJuneExitBox->SetEnable( bShow );
		SetExitBoxText( bParty );
		ResultEnableRetry( false, -1.f );
	}
	JUNE_RESULT_FLOW_LOG(( "exit box: %s, %s", bParty ? "party" : "solo", ResultDlgAlive( m_pDLGJuneExitBox ) ? "shown" : "not created" ));
}

// 31f17e60: the text by state; June's PvE-state row ( 0x17 ) belongs to Stage 9
void CX2State::SetExitBoxText( bool bParty )
{
	if ( ResultDlgAlive( m_pDLGJuneExitBox ) == false )
		return;
	CKTDGUIStatic* pStatic = static_cast< CKTDGUIStatic* >( m_pDLGJuneExitBox->GetControl( L"StaticMsgBoxBG" ) );
	if ( pStatic == NULL )
		return;

	int iStringID = 0;
	if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
	{
		iStringID = 39778;
		if ( bParty == false )
			iStringID = 43515;
		else if ( g_pData->GetDungeonRoom() != NULL && 38300 <= (int)g_pData->GetDungeonRoom()->GetDungeonID() && (int)g_pData->GetDungeonRoom()->GetDungeonID() <= 38302 )		// 328b26a0
			iStringID = 43487;
	}
	else
		iStringID = ( bParty == true ) ? 39779 : 43516;
	pStatic->SetString( 0, GET_STRING( iStringID ) );
}

// 31f17c20 ( vslot 0xd8 ): June dereferences the dialog without a check
void CX2State::UpdatePartyResultDLG( bool bDone, bool bShowTime, int* piSec )
{
	if ( ResultDlgAlive( m_pDLGJunePartyResult ) == false || piSec == NULL )
		return;

	CKTDGUIControl* pState = m_pDLGJunePartyResult->GetControl( L"Party_Member_Result_State" );
	if ( pState != NULL )
		pState->SetShow( bDone == true && m_bJuneResultConfirmSent == true && m_bJuneResultQuit == false );

	CKTDGUIStatic* pTime = static_cast< CKTDGUIStatic* >( m_pDLGJunePartyResult->GetControl( L"Party_Member_Waiting_Time" ) );
	if ( pTime != NULL )
	{
		WCHAR wszText[ 256 ] = L"";
		StringCchPrintfW( wszText, ARRAY_SIZE( wszText ), L"%2d:%2d %s", *piSec / 60, *piSec % 60, GET_STRING( 32047 ) );
		if ( pTime->GetString( 0 ) != NULL )
			pTime->GetString( 0 )->msg = wszText;
		pTime->SetShow( bShowTime );
	}
}

// 31f08590 ( vslot 0xdc ): June does not free an earlier dialog here
void CX2State::CreatePartyResultDLG()
{
	m_pDLGJunePartyResult = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Party_Member_Result_State.lua", 0.07f );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGJunePartyResult );
	int iSec = 0;
	UpdatePartyResultDLG( false, false, &iSec );
}

// 324c8390 ( vslot 0xd0 ): no caller found in June
void CX2StateDungeonGame::ResultSetModeFour()
{
	SetResultUIMode( 4, 0.f );
}

// 324cf3d0 ( vslot 0xd4 ), shared with the training state
void CX2StateDungeonGame::ResultModeFrameMove( float fElapsedTime )
{
	CX2State::ResultModeFrameMove( fElapsedTime );

	switch ( m_iJuneResultMode )
	{
	case 1:
		{
			if ( (double)m_fJuneResultModeTime < 0.1 && ResultNpcEventActive() == false )
				return;

			if ( g_pMain->GetIsPlayingTutorial() == true )
			{
				if ( m_pDungeonGame != NULL && m_pDungeonGame->GetCanUseEscFlag() == false )
					m_pDungeonGame->SetCanUseEscFlag( true );
				SetResultUIMode( 0, 0.f );
				ResultSendConfirm( false );
				return;
			}

			CKTDGGFXManager* pManager = g_pKTDXApp->GetGFXManager();
			if ( pManager == NULL || pManager->IsMovieLoaded() == false )
			{
				// degrade: no GFx widget, June's no-result-info path ( vslot 0xf0 )
				KLOG( "ErrorLog.txt" ) << "[JUNE_RESULT] FLOW no GFx manager or main.gfx: result confirmed without the widget" << fileout;
				ResultSendConfirm( false );
				return;
			}
			if ( pManager->isOpenUI( JUNE_RESULT_UI_ID ) == true )
				return;
			if ( g_pData->GetDungeonResultInfo() == NULL )
			{
				ResultSendConfirm( false );
				return;
			}
			if ( m_bJuneResultUIOpened == false )
			{
				ResultRecreateAdapter();
				pManager->openUI( JUNE_RESULT_UI_ID );
				m_bJuneResultUIOpened = true;
				JUNE_RESULT_FLOW_LOG(( "mode 1: UI 8 opened" ));
			}
			if ( g_pKTDXApp->GetDGManager()->GetDialogManager() != NULL )
				g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( false, 2 );		// @324cf508
			if ( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIQuickSlot() != NULL )
				g_pData->GetUIManager()->GetUIQuickSlot()->SetShow( true );						// quick slot vslot 0x20( 1 )
		} break;

	case 2:
		{
			ResultFill();
			CreatePartyResultDLG();
			SetResultUIMode( 3, 0.f );
		} break;

	case 3:
		{
			if ( m_pDungeonGame != NULL && m_pDungeonGame->GetCanUseEscFlag() == false )
				m_pDungeonGame->SetCanUseEscFlag( true );

			int iSec = (int)( m_fJuneResultModeLimit - m_fJuneResultModeTime );
			if ( iSec < 0 )
				iSec = 0;
			UpdatePartyResultDLG( false, true, &iSec );

			if ( m_fJuneResultModeLimit <= m_fJuneResultModeTime )
			{
				SetResultUIMode( 4, 0.f );
				return;
			}
			if ( m_fJuneResultModeTime < 0.f || m_bJuneResultSpeedChecked == true )
				return;

			// June's speed-clear hack report ( @324cf63d-324cf762 )
			const CX2Dungeon::DungeonData* pDungeonData = NULL;
			if ( g_pData->GetDungeonManager() != NULL && g_pData->GetPartyManager() != NULL )
			{
				const CX2PartyManager::PartyData* pPartyData = g_pData->GetPartyManager()->GetMyPartyData();
				pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID)( pPartyData->m_iDungeonID + pPartyData->m_iDungeonDifficulty ) );		// 327da150
			}
			int iLevelLimit = 0;
			if ( pDungeonData != NULL )
			{
				iLevelLimit = pDungeonData->m_MinLevel;
				if ( pDungeonData->m_MaxLevel > 0 )
					iLevelLimit = pDungeonData->m_MaxLevel;
			}

			const CX2DungeonGame::DungeonResultInfo* pResultInfo = g_pData->GetDungeonResultInfo();
			CX2DungeonRoom* pRoom = g_pData->GetDungeonRoom();
			CX2Room::SlotData* pMySlot = ( pRoom != NULL ) ? pRoom->GetMySlot() : NULL;
			if ( pResultInfo != NULL && pMySlot != NULL && pMySlot->m_pUnit != NULL && g_pData->GetMyUser() != NULL )
			{
				const int iMyLevel		= (int)pMySlot->m_pUnit->GetUnitData().m_Level;
				const int iDungeonID	= (int)pRoom->GetDungeonID();
				if ( pResultInfo->m_bWin == true && pResultInfo->m_Sec <= 120 && pResultInfo->m_DungeonResultUnitInfoList.size() == 1
					&& iMyLevel <= iLevelLimit + 20 && g_pKTDXApp->GetFindHacking() == false
					&& g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_OPERATOR && iDungeonID != 38100 && 30133 <= iDungeonID && iDungeonID <= 38999 )
				{
					if ( g_pData->GetServerProtocol() != NULL )
					{
						g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );		// June 0x1f2
						g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
					}
					// g_pMain 31d75660( 0x79, L"", 0, 1 ) is a no-op for 0x79
					JUNE_RESULT_FLOW_LOG(( "speed-clear report: %d s, level %d, limit %d, dungeon %d", pResultInfo->m_Sec, iMyLevel, iLevelLimit, iDungeonID ));
				}
			}
			m_bJuneResultSpeedChecked = true;
		} break;

	case 4:
		{
			int iSec = (int)( m_fJuneResultModeLimit - m_fJuneResultModeTime );
			if ( iSec < 0 )
				iSec = 0;
			UpdatePartyResultDLG( true, true, &iSec );
			CKTDGGFXManager* pManager = g_pKTDXApp->GetGFXManager();
			if ( pManager != NULL && pManager->isOpenUI( JUNE_RESULT_UI_ID ) == true )
				ResultCloseAndDelete();
			ResultSendConfirm( false );
		} break;

	default:
		break;
	}
}

// 324d8600 ( vslot 0xe0 ): a party asks the server at once, a solo player gets the ready box first
void CX2StateDungeonGame::ResultRetry()
{
	if ( m_bJuneResultRetryEnabled == false )
		return;

#ifdef SERV_IRUHADEV_JUNE_RESULT_RETRY_DIRECT
	// User decision 2026-10-22: solo and auto-party send the REQ at once, no ready box
	ResultSendRetryReq();
#else SERV_IRUHADEV_JUNE_RESULT_RETRY_DIRECT
	if ( g_pData->GetPartyManager() != NULL && g_pData->GetPartyManager()->DoIHaveParty() == true )
	{
		ResultSendRetryReq();
		return;
	}

	CX2Main::TimedMessagePopUp::TimedPopupUserData kUserData = ResultMyPopupData();
	int iDungeonID = 0, iDifficulty = 0, iMode = 0;
	if ( g_pData->GetPartyManager() != NULL )
	{
		const CX2PartyManager::PartyData* pPartyData = g_pData->GetPartyManager()->GetMyPartyData();
		iDungeonID	= pPartyData->m_iDungeonID;
		iDifficulty	= pPartyData->m_iDungeonDifficulty;
#ifdef HENIR_TEST
		iMode		= pPartyData->m_iDungeonMode;
#endif HENIR_TEST
	}
	const std::wstring wstrDungeonName = ResultDungeonName( iDungeonID, iDifficulty, iMode );
	// June's extra ( 230, 200 ) argument to 31d94a20 has no March parameter
	g_pMain->AddTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST, CX2Main::TimedMessagePopUp::MBT_OK_CANCEL,
		kUserData, 10.f, GET_REPLACED_STRING( ( STR_ID_21304, "Li", wstrDungeonName, 1 ) ), (CKTDXStage*)g_pMain->GetNowState(),
		DGUCM_JUNE_RESULT_RETRY_OK, DGUCM_JUNE_RESULT_RETRY_CANCEL, DGUCM_JUNE_RESULT_RETRY_CANCEL );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( false, 2 );		// @324d880f
	ResultEnableRetry( false, -1.f );
	JUNE_RESULT_FLOW_LOG(( "retry: solo ready box ( dungeon %d, difficulty %d, mode %d )", iDungeonID, iDifficulty, iMode ));
#endif SERV_IRUHADEV_JUNE_RESULT_RETRY_DIRECT
}

// 324c8c40 ( vslot 0x294 ): Tab while the dialogs are hidden, only in mode 3
void CX2StateDungeonGame::ResultTabKey()
{
	if ( m_iJuneResultMode == 3 && m_pJuneResultAdapter != NULL )
	{
		m_pJuneResultAdapter->NotifyTabKey();
		JUNE_RESULT_FLOW_LOG(( "Tab: onTabKeyProcess notified" ));
	}
}

// 324d11e0 ( vslot 0xe8 ): its only June caller is the party manager ( Stage 28 )
void CX2StateDungeonGame::ResultRefreshRetryButton()
{
	if ( m_iJuneResultMode == 3 && m_pJuneResultAdapter != NULL )
		m_pJuneResultAdapter->ShowRetryButton( ResultRetryAllowed() );
}

// 324c9990 ( vslot 0xec ): ESC with the ready box up is its Cancel
void CX2StateDungeonGame::ResultEscWithRetryPopup()
{
	if ( g_pMain->IsThereTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST ) == true )
		g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, DGUCM_JUNE_RESULT_RETRY_CANCEL, NULL, true );
}

// 324d84c0 ( vslot 0xf0 ): once; a pending ready box is declined first ( 32434250( 0 ) )
bool CX2StateDungeonGame::ResultSendConfirm( bool bQuit )
{
	if ( m_bJuneResultConfirmSent == true )
		return false;
	m_bJuneResultConfirmSent	= true;
	m_bJuneResultQuit			= bQuit;

	if ( g_pMain->IsThereTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST ) == true )
	{
		g_pMain->RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST, ResultMyPopupData() );
		g_pData->GetPartyManager()->Send_EGS_PARTY_GAME_START_OPINION_CHECK_REPLY_NOT( false );
	}
	g_pData->GetServerProtocol()->SendID( EGS_RESULT_SUCCESS_REQ );
	g_pMain->AddServerPacket( EGS_RESULT_SUCCESS_ACK, 0.f );
	JUNE_RESULT_FLOW_LOG(( "confirm sent ( quit %d )", bQuit ? 1 : 0 ));
	return true;
}

// 324c9530
bool CX2StateDungeonGame::ResultSendRetryReq()
{
	if ( ResultIsSoloOrLeader() == false )
		return false;
	if ( g_pMain->IsWaitingServerPacket( EGS_DUNGEON_RETRY_ACK ) == true )		// 31d70e00
		return false;
	g_pData->GetServerProtocol()->SendID( EGS_DUNGEON_RETRY_REQ );
	g_pMain->AddServerPacket( EGS_DUNGEON_RETRY_ACK, 0.f );
	ResultEnableRetry( false, -1.f );
	JUNE_RESULT_FLOW_LOG(( "retry REQ sent" ));
	return true;
}

// 324db6b0: replaces March's handler; the first-clear movie ( 31d7a150 / 31d758c0 / 31d78bf0 ) is Stage 26's
bool CX2StateDungeonGame::ResultEndHandler( KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT& kPacket )
{
	if ( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIQuickSlot() != NULL )
		g_pData->GetUIManager()->GetUIQuickSlot()->SetEnable( false );			// 3294c460( 0, 0 )

	if ( m_pDungeonGame != NULL )
		m_pDungeonGame->Handler_EGS_END_GAME_DUNGEON_RESULT_DATA_NOT( kPacket );	// 327c99e0

	bool bDieMyUnit = true;
	const UidType iMyUID = g_pData->GetMyUser()->GetSelectUnit()->GetUID();
	for ( int i = 0; i < (int)kPacket.m_vecDungeonUnitInfo.size(); ++i )
	{
		if ( kPacket.m_vecDungeonUnitInfo[ i ].m_UnitUID == iMyUID )
		{
			bDieMyUnit = kPacket.m_vecDungeonUnitInfo[ i ].m_bIsDie;
			break;
		}
	}

	if ( kPacket.m_bIsWin == true && bDieMyUnit == false && g_pX2Game != NULL && g_pX2Game->GetMyUnit() != NULL )
	{
		const int iRealDungeonID = kPacket.m_iDungeonID + (int)kPacket.m_cDifficulty;
		CX2Unit* pMyUnit = g_pX2Game->GetMyUnit()->GetUnit();
		if ( pMyUnit != NULL && pMyUnit->IsClearDungeon( iRealDungeonID ) == false )		// 3234f870
			pMyUnit->AddClearDungeon( iRealDungeonID );									// 32356a30
	}

	if ( g_pData->GetDungeonResultInfo() == NULL )
	{
		if ( g_pMain->GetNowState() != NULL )
			static_cast< CX2State* >( g_pMain->GetNowState() )->ReturnToPlaceWhereBeforeDungeonStart();
	}
	else
		SetResultUIMode( 1, 30.f );		// @324db8d7
	// DI_EVENT_HIGHSCHOOL ( 41450 ) score effects read a June-only packet map: routed to the event-dungeon owner
	JUNE_RESULT_FLOW_LOG(( "end: win %d, my unit dead %d, dungeon %d, units %d", kPacket.m_bIsWin ? 1 : 0, bDieMyUnit ? 1 : 0, kPacket.m_iDungeonID, (int)kPacket.m_vecDungeonUnitInfo.size() ));
	return true;
}

// 324d4cd0
bool CX2StateDungeonGame::Handler_EGS_RESULT_SUCCESS_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESULT_SUCCESS_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_RESULT_SUCCESS_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( m_bJuneResultAckDone == false )
			{
				if ( m_bJuneResultQuit == true )
				{
					if ( m_pDungeonGame != NULL )
						m_pDungeonGame->Handler_EGS_LEAVE_ROOM_REQ( NetError::NOT_LEAVE_ROOM_REASON_00 );		// game vslot 0x12c( 0x72 )
					if ( g_pData->GetPartyManager() != NULL && g_pData->GetPartyManager()->DoIHaveParty() == true && m_bJuneResultLeaveOnAck == false )
						g_pData->GetPartyManager()->Handler_EGS_LEAVE_PARTY_REQ( NetError::NOT_LEAVE_ROOM_REASON_00 );	// 324334d0( 0x72 )
					m_bJuneResultAckDone = true;
				}
				else if ( m_bJuneResultLeaveOnAck == true && g_pMain->GetNowState() != NULL )
				{
					static_cast< CX2State* >( g_pMain->GetNowState() )->ReturnToPlaceWhereBeforeDungeonStart();
					m_bJuneResultAckDone = true;
				}
			}
			JUNE_RESULT_FLOW_LOG(( "confirm ACK: quit %d, leave on ACK %d, done %d", m_bJuneResultQuit ? 1 : 0, m_bJuneResultLeaveOnAck ? 1 : 0, m_bJuneResultAckDone ? 1 : 0 ));
			return true;
		}
		if ( kEvent.m_iOK == NetError::NOT_LEAVE_ROOM_REASON_14 && g_pData->GetDungeonRoom() != NULL )
			g_pData->GetDungeonRoom()->SetNotEnoughSpirit( true );
	}
	return false;
}

// 324dbb30
bool CX2StateDungeonGame::Handler_EGS_RESULT_ALL_UNIT_SUCCESS_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESULT_ALL_UNIT_SUCCESS_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->GetIsPlayingTutorial() == true )
	{
		g_pMain->SetIsPlayingTutorial( false );		// 31d716c0( 0 )
		g_pMain->SetIsExitingTutorial( true );		// +0x16c
	}
	JUNE_RESULT_FLOW_LOG(( "all-unit NOT: %s", m_bJuneResultAckDone ? "already left" : "leaving" ));
	if ( m_bJuneResultAckDone == false && g_pMain->GetNowState() != NULL )
		static_cast< CX2State* >( g_pMain->GetNowState() )->ReturnToPlaceWhereBeforeDungeonStart();
	return true;
}

// 324c95b0: success does nothing here, the server starts the dungeon again
bool CX2StateDungeonGame::Handler_EGS_DUNGEON_RETRY_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_DUNGEON_RETRY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_DUNGEON_RETRY_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == false )
			ResultEnableRetry( true, -1.f );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( false, 2 );		// @324c966e
	}
	JUNE_RESULT_FLOW_LOG(( "retry ACK: %d", kEvent.m_iOK ));
	return true;
}
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW

#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
void ResultOpen_LUA()
{
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState == NULL )
	{
		ResultWidgetLog( "open: the current state is not a dungeon state" );
		return;
	}
	CKTDGGFXManager* pManager = ( g_pKTDXApp != NULL ) ? g_pKTDXApp->GetGFXManager() : NULL;
	if ( pManager == NULL || pManager->IsMovieLoaded() == false )
	{
		ResultWidgetLog( "open: no GFx manager or main.gfx did not load" );
		return;
	}

	pState->ResultRecreateAdapter();
	const bool bWasOpen = pManager->isOpenUI( JUNE_RESULT_UI_ID );
	if ( bWasOpen == false )
		pManager->openUI( JUNE_RESULT_UI_ID );
	ResultWidgetLog( "open: adapter %s, UI 8 %s", pState->ResultIsBound() ? "bound" : "not bound", bWasOpen ? "was open ( the fill runs now )" : "opened ( the fill runs at notifyInitialize )" );
	if ( bWasOpen == true )
		pState->ResultFill();
}

void ResultClose_LUA()
{
	CX2StateDungeonGame* pState = ResultNowState();
	if ( pState == NULL )
		return;
	pState->ResultCloseAndDelete();
	ResultWidgetLog( "close: UI 8 closed, adapter deleted" );
}
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
