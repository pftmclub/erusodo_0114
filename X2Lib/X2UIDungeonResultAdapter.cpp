#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
#include "X2UIDungeonResultAdapter.h"
#include "X2StateDungeonGame.h"
#include <set>
#include <algorithm>

// Log tag [JUNE_RESULT] WIDGET ( ErrorLog.txt )
void JuneResultWidgetLog( const char* szFormat, ... )
{
	char szBuf[ 1024 ] = { 0, };
	va_list args;
	va_start( args, szFormat );
	_vsnprintf_s( szBuf, sizeof( szBuf ), _TRUNCATE, szFormat, args );
	va_end( args );
	KLOG( "ErrorLog.txt" ) << "[JUNE_RESULT] WIDGET " << szBuf << fileout;
}

// 32eea2c0 and the others read the current state as g_pMain +0x90 and dynamic_cast it to the dungeon state
static CX2StateDungeonGame* JuneResultNowState()
{
	if ( g_pMain == NULL )
		return NULL;
	return dynamic_cast< CX2StateDungeonGame* >( g_pMain->GetNowState() );
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// CX2UIDungeonResultAdapter
// ---------------------------------------------------------------------------------------------------------------------------------------
CX2UIDungeonResultAdapter::CX2UIDungeonResultAdapter( CKTDGGFXManager* pManager )
	: m_pBridge( NULL )
{
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

void CX2UIDungeonResultAdapter::UpdateDungeonInfo( const KJuneGfxResultInfo& kInfo )
{
	if ( m_pBridge != NULL )
		m_pBridge->UpdateDungeonInfo( kInfo );
}

void CX2UIDungeonResultAdapter::SetItem( int iIndex, int iItemID, int iNum, int iUIDLow, int iUIDHigh )
{
	if ( m_pBridge == NULL )
		return;

	KJuneGfxResultItem kItem;
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

void CX2UIDungeonResultAdapter::OnResultAsClose()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultWidgetLog( "notifyCloseResultUI" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState != NULL )
		pState->JuneResultOnClose();
}

void CX2UIDungeonResultAdapter::OnResultAsRetry()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultWidgetLog( "notfyRetryButton" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState != NULL )
		pState->JuneResultOnRetry();
}

void CX2UIDungeonResultAdapter::OnResultAsInitialize()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultWidgetLog( "notifyInitialize" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState != NULL )
		pState->JuneResultOnInitialize();
}

void CX2UIDungeonResultAdapter::OnResultAsArrowLeft()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultWidgetLog( "notfyitemSlotArrowLeft" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState != NULL )
		pState->JuneResultItemPagePrev();		// 324d1270
}

void CX2UIDungeonResultAdapter::OnResultAsArrowRight()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultWidgetLog( "notfyitemSlotArrowRight" );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState != NULL )
		pState->JuneResultItemPageNext();		// 324d1210
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// CX2StateDungeonGame: the adapter owner ( June dungeon / training state vtables 3349f91c / 3349e02c, slots 0xb0-0xf0 identical )
// ---------------------------------------------------------------------------------------------------------------------------------------
namespace
{
	const int JUNE_RESULT_UI_ID			= 8;		// GFx UI 8 = dungeon_result.gfx ( openUI 31c44530 )
	const int JUNE_RESULT_ITEM_SLOTS	= 12;

	// 324ddca0: the order of one category's map ( a unique-key std::map: an entry neither side of which is less is dropped )
	struct KJuneResultEntryLess
	{
		bool operator()( const KJuneGfxResultEntry& kA, const KJuneGfxResultEntry& kB ) const
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

	float JuneResultClamp01( float fValue )		// 324d6ccb..: the two rates before 32eeae30 clamps them again
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
void CX2StateDungeonGame::JuneResultCreateAdapter()
{
	CKTDGGFXManager* pManager = ( g_pKTDXApp != NULL ) ? g_pKTDXApp->GetJuneGFXManager() : NULL;
	if ( pManager != NULL && pManager->IsMovieLoaded() == false )
		pManager = NULL;
	m_pJuneResultAdapter = new CX2UIDungeonResultAdapter( pManager );
}

// 324c8320 ( vslot 0xbc )
void CX2StateDungeonGame::JuneResultDeleteAdapter()
{
	if ( m_pJuneResultAdapter != NULL )
	{
		delete m_pJuneResultAdapter;
		m_pJuneResultAdapter = NULL;
	}
}

// 324c8c20 ( vslot 0xc4 )
void CX2StateDungeonGame::JuneResultCloseUI()
{
	if ( g_pKTDXApp != NULL && g_pKTDXApp->GetJuneGFXManager() != NULL )
		g_pKTDXApp->GetJuneGFXManager()->unloadWidget( JUNE_RESULT_UI_ID );		// 31c445d0
}

// 324c8350 ( vslot 0xc8 )
void CX2StateDungeonGame::JuneResultRecreateAdapter()
{
	JuneResultDeleteAdapter();
	JuneResultCreateAdapter();
}

// 324c8370 ( vslot 0xcc )
void CX2StateDungeonGame::JuneResultCloseAndDelete()
{
	JuneResultCloseUI();
	JuneResultDeleteAdapter();
}

// 324c83f0 ( vslot 0xe4 ): t > 0 is a cool-down ( stored, the tick 324c83a0 is not ported ), else the button state
void CX2StateDungeonGame::JuneResultEnableRetry( bool bEnable, float fCoolTime )
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
bool CX2StateDungeonGame::JuneResultRetryAllowed()
{
	if ( g_pX2Game != NULL && g_pX2Game->GetGameType() != CX2Game::GT_DUNGEON )
		return false;
	if ( JuneResultNpcEventActive() == true )
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
void CX2StateDungeonGame::JuneResultFillItems()
{
	if ( m_pJuneResultAdapter == NULL )
		return;

	std::vector< CX2Item::ItemData > vecItem;
	if ( m_pDungeonGame == NULL || m_pDungeonGame->JuneGetObtainedItems( m_iJuneResultItemPage * JUNE_RESULT_ITEM_SLOTS, JUNE_RESULT_ITEM_SLOTS, vecItem ) == false )
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
void CX2StateDungeonGame::JuneResultItemPageNext()
{
	if ( m_pDungeonGame != NULL && m_pDungeonGame->JuneHasObtainedItemPage( m_iJuneResultItemPage * JUNE_RESULT_ITEM_SLOTS + 13 ) == true )
	{
		++m_iJuneResultItemPage;
		JuneResultFillItems();
	}
}

// 324d1270 ( left )
void CX2StateDungeonGame::JuneResultItemPagePrev()
{
	if ( m_iJuneResultItemPage != 0 )
	{
		--m_iJuneResultItemPage;
		JuneResultFillItems();
	}
}

// 324d60e0 ( vslot 0xc0 ): the result info -> updateDungeonInfo, then the game's item sort ( vslot 0x228 ) and the item fill.
// Not ported: the dialog-manager block at its start ( 31d05fe0 / 31d05ec0, two flag-walks over the dialog stack at g_pKTDXApp +0x1fc +0x38c ) -> Stage 9.
void CX2StateDungeonGame::JuneResultFill()
{
	CX2DungeonGame::DungeonResultInfo* pResultInfo = ( g_pData != NULL ) ? g_pData->GetDungeonResultInfo() : NULL;
	if ( pResultInfo == NULL || g_pX2Game == NULL || m_pJuneResultAdapter == NULL )		// June: vslot 0xf0( 0 ) ( the result confirm, R3 )
	{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
		JuneResultWidgetLog( "fill: %s", pResultInfo == NULL ? "no result info" : ( g_pX2Game == NULL ? "no game" : "no adapter" ) );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		return;
	}

	// the six category lists ( 324b3930 x 6 ), filled per unit below and pushed in order ( 324b54b0 x 6 )
	std::set< KJuneGfxResultEntry, KJuneResultEntryLess >	setCategory[ 6 ];
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
			KJuneGfxResultEntry kEntry;
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
		JuneResultWidgetLog( "fill: no row for my unit ( %d units )", iPlayerCount );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
		return;
	}

	const CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();
	const CX2Unit::UnitData& kUnitData = pMyUnit->GetUnitData();

	// the exp bar: ( EXP - base ) / range, range = the next level's need ( the ExpTable row, 32741030 )
	const int iLevel = kUnitData.m_Level;
	const float fBase = (float)(int)kUnitData.m_NowBaseLevelEXP;
	const float fRange = ( g_pData->GetEXPTable() != NULL ) ? (float)g_pData->GetEXPTable()->GetEXPData( iLevel + 1 ).m_nNeedExp : 0.0f;
	float fExpRate = JuneResultClamp01( ( (float)(int)kUnitData.m_EXP - fBase ) / fRange );
	const float fOldExp = (float)pMyResult->m_nOldEXP;
	float fOldExpRate = JuneResultClamp01( ( fOldExp - fBase ) / fRange );

	// the ED: the gain, held to what the unit now has above the dungeon-start ED ( state +0x7e8, June-only: 0 here )
	long long llED = pMyResult->m_nED;
	{
		const long long llNowED = (int)kUnitData.m_ED;
		if ( llNowED < llED + m_llJuneResultStartED )
			llED = llNowED - m_llJuneResultStartED;
	}

	KJuneGfxResultInfo kInfo;
	kInfo.m_iTotalRank		= (int)pMyResult->m_cTotalRank;
	kInfo.m_iSec			= pResultInfo->m_Sec;
	kInfo.m_iBestSec		= 0;										// resultInfo +0x2c: June-only, no writer
	kInfo.m_iED				= (int)llED;
	kInfo.m_iHitRank		= 0;										// unit [0x20]: June-only
	kInfo.m_iComboRank		= (int)pMyResult->m_cComboRank;
	kInfo.m_iTechnicalRank	= (int)pMyResult->m_cTechnicalRank;
	kInfo.m_iBeAttackedRank	= (int)pMyResult->m_cDamagedRank;
	kInfo.m_iTotalEXP		= const_cast< CX2DungeonGame::DungeonResultUnitInfo* >( pMyResult )->GetTotalExp();	// 327c9390
	kInfo.m_fExpRate		= fExpRate;
	kInfo.m_fOldExpRate		= fOldExpRate;
	kInfo.m_bLevelUp		= ( fOldExp < fBase );
	kInfo.m_iLevel			= iLevel;
	kInfo.m_bShowExitButton	= ( JuneResultNpcEventActive() == false );
	kInfo.m_iResonanceLevel	= 0;										// 3247b460: June-only system
	kInfo.m_iUnitType		= (int)pMyUnit->GetType();
	kInfo.m_iPlayerCount	= iPlayerCount;
	kInfo.m_iRewardType		= 0;										// unit [0x29]: June-only
	kInfo.m_bReTryButtonShow = JuneResultRetryAllowed();
	kInfo.m_vecCategory.resize( 6 );
	for ( int iCategory = 0; iCategory < 6; ++iCategory )
		kInfo.m_vecCategory[ iCategory ].assign( setCategory[ iCategory ].begin(), setCategory[ iCategory ].end() );

	m_pJuneResultAdapter->UpdateDungeonInfo( kInfo );
	if ( m_pDungeonGame != NULL )
		m_pDungeonGame->JuneSortObtainedItems();		// game vslot 0x228
	JuneResultFillItems();

#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultWidgetLog( "fill: %d players, my rank %d, ED %d, EXP %d ( rate %.3f, old %.3f ), level %d%s, retry %d, obtained items %d",
		iPlayerCount, kInfo.m_iTotalRank, kInfo.m_iED, kInfo.m_iTotalEXP, kInfo.m_fExpRate, kInfo.m_fOldExpRate, kInfo.m_iLevel, kInfo.m_bLevelUp ? " ( level up )" : "",
		kInfo.m_bReTryButtonShow ? 1 : 0, m_pDungeonGame != NULL ? m_pDungeonGame->JuneGetObtainedItemCount() : 0 );
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
}

bool CX2StateDungeonGame::JuneResultIsBound() const
{
	return m_pJuneResultAdapter != NULL && m_pJuneResultAdapter->IsBound() == true;
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// CX2DungeonGame: the obtained-item list ( June +0xdf4 / +0xdf8, 0xc0 B records = CX2Item::ItemData: UID +0, id +8, period type +0xc, quantity +0x14 )
// ---------------------------------------------------------------------------------------------------------------------------------------
// 327cedf0 ( vslot 0x21c ): a negative quantity is dropped; a quantity item ( period type 2 ) adds to the record with its id, any other item
// is dropped when its UID is already listed; else it is appended
void CX2DungeonGame::JuneAddObtainedItem( const CX2Item::ItemData& kItemData )
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
bool CX2DungeonGame::JuneGetObtainedItems( unsigned uStart, unsigned uCount, std::vector< CX2Item::ItemData >& vecOut ) const
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
bool CX2DungeonGame::JuneHasObtainedItemPage( unsigned uCount ) const
{
	return m_vecJuneObtainedItem.size() >= uCount;
}

// 327cef20 ( vslot 0x228 ): the item manager's std::sort over the list, by the item grade ( templet nibble +4 >> 0x14 ); an item without a templet never sorts
// before another ( 327d7300 is the insertion sort June's std::sort uses up to 32 items, which is stable )
namespace
{
	struct KJuneObtainedItemLess
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

void CX2DungeonGame::JuneSortObtainedItems()
{
	if ( m_vecJuneObtainedItem.empty() == true || g_pData == NULL || g_pData->GetItemManager() == NULL )
		return;
	std::stable_sort( m_vecJuneObtainedItem.begin(), m_vecJuneObtainedItem.end(), KJuneObtainedItemLess() );
}

// notifyCloseResultUI 32eea2c0: June opens the exit box ( 31f1dd70( 1 ) ), R3. In the dev console the widget just closes.
void CX2StateDungeonGame::JuneResultOnClose()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultCloseUI();
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
}

// notfyRetryButton 32eea2e0: June calls state vslot 0xe0 ( the retry request ), R2 / R3
void CX2StateDungeonGame::JuneResultOnRetry()
{
}

// notifyInitialize 32eea300: June sets the result-UI mode 2 ( 31ef7be0( 2, 0.0 ) ), whose tick runs the fill ( R3 ). In the dev console the fill runs here.
void CX2StateDungeonGame::JuneResultOnInitialize()
{
#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
	JuneResultFill();
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
}

#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
void JuneResultOpen_LUA()
{
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState == NULL )
	{
		JuneResultWidgetLog( "open: the current state is not a dungeon state" );
		return;
	}
	CKTDGGFXManager* pManager = ( g_pKTDXApp != NULL ) ? g_pKTDXApp->GetJuneGFXManager() : NULL;
	if ( pManager == NULL || pManager->IsMovieLoaded() == false )
	{
		JuneResultWidgetLog( "open: no GFx manager or main.gfx did not load" );
		return;
	}

	pState->JuneResultRecreateAdapter();
	const bool bWasOpen = pManager->isOpenUI( JUNE_RESULT_UI_ID );
	if ( bWasOpen == false )
		pManager->openUI( JUNE_RESULT_UI_ID );
	JuneResultWidgetLog( "open: adapter %s, UI 8 %s", pState->JuneResultIsBound() ? "bound" : "not bound", bWasOpen ? "was open ( the fill runs now )" : "opened ( the fill runs at notifyInitialize )" );
	if ( bWasOpen == true )
		pState->JuneResultFill();
}

void JuneResultClose_LUA()
{
	CX2StateDungeonGame* pState = JuneResultNowState();
	if ( pState == NULL )
		return;
	pState->JuneResultCloseAndDelete();
	JuneResultWidgetLog( "close: UI 8 closed, adapter deleted" );
}
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
