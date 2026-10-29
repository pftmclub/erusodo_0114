#include "StdAfx.h"
#include "X2SkillNoteManager.h"

#ifdef REFORM_SKILL_NOTE_UI


CX2SkillNoteManager* CX2SkillNoteManager::m_pManager = NULL;
/*static */CX2SkillNoteManager* CX2SkillNoteManager::GetInstance()
{
	if( NULL == m_pManager)
		m_pManager = new CX2SkillNoteManager;

	return m_pManager;
}

/*static*/ void CX2SkillNoteManager::ResetSkillNoteManager()
{
	DestroyInstance();
}

/*static*/ void CX2SkillNoteManager::DestroyInstance()
{
	SAFE_DELETE( m_pManager );
}

CX2SkillNoteManager::CX2SkillNoteManager():
m_eUnitClass(CX2Unit::UC_NONE)
{
}

CX2SkillNoteManager::~CX2SkillNoteManager()
{
}
void CX2SkillNoteManager::AddSkillNoteTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	CX2Unit::UNIT_CLASS eUnitClass;
	LUA_GET_VALUE_ENUM( luaManager, "UNIT_CLASS", eUnitClass, CX2Unit::UNIT_CLASS, CX2Unit::UC_NONE );

	ASSERT( CX2Unit::UC_NONE != eUnitClass );
	if( eUnitClass == CX2Unit::UC_NONE )
		return;

	vector<pair<int, int>> vecMemoList;
	if( luaManager.BeginTable( "MEMO_LIST" ) == true )
	{
		int iTableIndex = 1;
		while( true == luaManager.BeginTable( iTableIndex++  ) )
		{
			int iMemoItemID = -1;
			int iSkillID = -1;
			luaManager.GetValue( 1, iMemoItemID );
			luaManager.GetValue( 2, iSkillID );

			if( -1 != iMemoItemID &&
				-1 != iSkillID )
			{
				vecMemoList.push_back( std::make_pair(iMemoItemID,iSkillID));
			}
			else
			{
				ASSERT(!"SkillNoteTemplet Setting Error");
			}
			luaManager.EndTable();
		}
		luaManager.EndTable();
	}

	if( false == vecMemoList.empty() )
		m_mapSkillNoteTemplet.insert( std::make_pair( eUnitClass, vecMemoList ));
	return;
}
bool CX2SkillNoteManager::OpenScriptFile()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, false );

	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pSkillNoteManager", this );
	if( false == g_pKTDXApp->LoadLuaTinker( L"SkillNoteTemplet.lua" ) )
	{
		ErrorLogMsg( XEM_ERROR147, L"SkillNoteTemplet.lua" );
		return false;
	}

	return true;
}
/** @function : ResetMemoList
	@brief : 선택된 클래스에 맞게 메모 리스트 갱신
			1. 메모 리스트 UI 갱신
			2. 현재 클래스가 사용 가능한 메모를 멤버 변수에 등록
			3. 정렬 기준에 맞게 메모 리스트 정렬
*/
void CX2SkillNoteManager::ResetMemoList()
{
	if( CX2Unit::UC_NONE == m_eUnitClass )
		return;

	if( NULL != g_pData &&
		NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillNote() )
		g_pData->GetUIManager()->GetUISkillNote()->ResetMemoListDLG();

	m_vecUsableMemoListIfIhave.clear();


	// #1 현재 클래스가 참조해야 할 전체 클래스 얻기
	vector<CX2Unit::UNIT_CLASS> vecUnitClass;
	vecUnitClass.push_back( m_eUnitClass );
	// 하위 클래스 얻기
	g_pData->GetUnitManager()->GetLowRankUnitClass(vecUnitClass, m_eUnitClass );

	/*노전~2차 구분
		vecUnitClass.size() 3 : 2차전직이 먼저.
		vecUnitClass.size() 2 : 1차전직이 먼저.
		vecUnitClass.size() 1 : 0차전직이 먼저.*/		
	char cClassLine = static_cast<char>(vecUnitClass.size()-1);

	// #2 m_mapMemoList에, 현재 클래스가 사용 할 수 있는 메모 추가
	BOOST_FOREACH( CX2Unit::UNIT_CLASS eUnitClass, vecUnitClass )
	{
		map< CX2Unit::UNIT_CLASS, vector<pair<int, int>> >::const_iterator mit =  m_mapSkillNoteTemplet.find(eUnitClass);
		if( mit != m_mapSkillNoteTemplet.end() )
		{
			typedef pair<int, int> MemoList;
			BOOST_FOREACH( const MemoList& pairMemoList,  mit->second )
			{
				MemoData sMemoData;
				sMemoData.iMemoItemID	= pairMemoList.first;
				sMemoData.iSkillID		= pairMemoList.second;
				sMemoData.cClassLine	= cClassLine;
				sMemoData.iMemoItemNum	= -1;

				m_vecUsableMemoListIfIhave.push_back(sMemoData);
			}
		}
		--cClassLine;
	}

	// #3 소지여부 및 소지 했을 경우 전직 순 대로 정렬
	std::sort(m_vecUsableMemoListIfIhave.begin(), m_vecUsableMemoListIfIhave.end(), MemoListSort());


	// #4 소지 하지 않은 메모를 전직 순으로 재 정렬
	{
		vector<MemoData>::iterator it = m_vecUsableMemoListIfIhave.begin();
		for( ; it != m_vecUsableMemoListIfIhave.end(); ++ it )
		{
			if( 0 >= it->iMemoItemNum )
				break;
		}

		std::sort(it, m_vecUsableMemoListIfIhave.end(), MemoListClassSort());
	}
}
/** @function : UpdateMemoPossession
	@brief : 소지 여부 갱신
*/
void CX2SkillNoteManager::UpdateMemoPossession()
{
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2Inventory& pInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
		BOOST_FOREACH( MemoData& sMemoData , m_vecUsableMemoListIfIhave)
		{
			const int iItemNum = pInventory.GetNumItemByTID( sMemoData.iMemoItemID);
			sMemoData.iMemoItemNum = iItemNum;
		}
	}
}
/** @function : UpdateMemoPossession
	@brief : 메모 리스트 소지여부에 따라 정렬.
			 (소지여부 갱신 후 정렬 해야 한다.)
*/
void CX2SkillNoteManager::UpdateSortMemoList()
{
	std::sort(m_vecUsableMemoListIfIhave.begin(), m_vecUsableMemoListIfIhave.end(), MemoListSort());

	// #4 소지 하지 않은 메모를 전직 순으로 재 정렬
	{
		vector<MemoData>::iterator it = m_vecUsableMemoListIfIhave.begin();
		for( ; it != m_vecUsableMemoListIfIhave.end(); ++ it )
		{
			if( 0 >= it->iMemoItemNum )
				break;
		}

		std::sort(it, m_vecUsableMemoListIfIhave.end(), MemoListClassSort());
	}
}
/** @function : GetSkillIDByMemoItemID
	@brief : 메모 아이템 ID에 해당하는 스킬ID 얻기
			 bIsMySkill_이 false일 경우 순회 시간이 오래 걸린다.
*/
int CX2SkillNoteManager::GetSkillIDByMemoItemID( int iMemoItemID_ , bool bIsMySkill_ /*= true */) const
{
	if( true == bIsMySkill_ )
	{
		BOOST_FOREACH( const MemoData& sMemoData , m_vecUsableMemoListIfIhave)
		{
			if( sMemoData.iMemoItemID == iMemoItemID_ )
				return sMemoData.iSkillID;
		}
	}
	else
	{
		map< CX2Unit::UNIT_CLASS, vector<pair<int, int>> >::const_iterator it = m_mapSkillNoteTemplet.begin();
		for( ; it != m_mapSkillNoteTemplet.end(); ++it )
		{
			typedef pair<int, int> MemoIDAndSkillID;

			BOOST_FOREACH( const MemoIDAndSkillID& sMemoIDAndSkillID, it->second)
			{
				if( sMemoIDAndSkillID.first == iMemoItemID_ )
					return sMemoIDAndSkillID.second;
			}
			
		}
	}
	return 0;
}
#endif // REFORM_SKILL_NOTE_UI

#ifdef SERV_IRUHADEV_JUNE_SKILL_NOTE
#ifdef SERV_IRUHADEV_OFFLINE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_OFFLINE

CX2SkillNoteManager* CX2SkillNoteManager::m_pManager = NULL;

/// June2017 324550a0: made on first use (0x54 bytes)
/*static*/ CX2SkillNoteManager* CX2SkillNoteManager::GetInstance()
{
	if( NULL == m_pManager )
		m_pManager = new CX2SkillNoteManager;
	return m_pManager;
}

/*static*/ void CX2SkillNoteManager::ResetSkillNoteManager()
{
	DestroyInstance();
}

/// June2017 32455240
/*static*/ void CX2SkillNoteManager::DestroyInstance()
{
	SAFE_DELETE( m_pManager );
}

CX2SkillNoteManager::CX2SkillNoteManager() :
m_iOldTempletCalls( 0 ),
m_iNewTempletCalls( 0 ),
m_iMatchCalls( 0 ),
m_iRemovableCalls( 0 ),
m_iOwnRows( 0 ),
m_iInheritedRows( 0 ),
m_iZeroMemoRows( 0 ),
m_iZeroSkillRows( 0 ),
m_iMissingParents( 0 ),
m_iDuplicateClasses( 0 )
{
}

CX2SkillNoteManager::~CX2SkillNoteManager()
{
}

/// June2017 32452270: g_pSkillNoteManager = this, then SkillNoteTemplet.lua (the shipped DoMemory path, as March's loader). Logs the
/// "[JUNE_SKILL] NOTE" counts after the script ran.
bool CX2SkillNoteManager::OpenScriptFile()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "g_pSkillNoteManager", this );
	if( false == g_pKTDXApp->LoadLuaTinker( L"SkillNoteTemplet.lua" ) )
	{
		ErrorLogMsg( XEM_ERROR147, L"SkillNoteTemplet.lua" );
#ifdef SERV_IRUHADEV_OFFLINE
		CX2OfflineLog::Server( L"[JUNE_SKILL] NOTE SkillNoteTemplet.lua did not load (XEM_ERROR147): no memo rows; check data036 carries June's file and Enum.lua's MEMO_ID" );
#endif SERV_IRUHADEV_OFFLINE
		return false;
	}

#ifdef SERV_IRUHADEV_OFFLINE
	int iClassRows = 0;
	for( std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.begin(); it != m_mapMemoTemplet.end(); ++it )
		iClassRows += static_cast<int>( it->second.size() );
	CX2OfflineLog::Server( L"[JUNE_SKILL] NOTE AddNewSkillNoteTemplet=%d AddMemoIDMatchItemID=%d AddRemovableMemoItemID=%d AddSkillNoteTemplet=%d",
		m_iNewTempletCalls, m_iMatchCalls, m_iRemovableCalls, m_iOldTempletCalls );
	CX2OfflineLog::Server( L"[JUNE_SKILL] NOTE classes=%d rows=%d ( own=%d inherited=%d ) match_pairs=%d removable_items=%d | memo0_dropped=%d skill0=%d parent_missing=%d class_dup=%d",
		static_cast<int>( m_mapMemoTemplet.size() ), iClassRows, m_iOwnRows, m_iInheritedRows,
		static_cast<int>( m_mmapMemoMatchItemID.size() ), static_cast<int>( m_setRemovableMemoItemID.size() ),
		m_iZeroMemoRows, m_iZeroSkillRows, m_iMissingParents, m_iDuplicateClasses );
#endif SERV_IRUHADEV_OFFLINE
	return true;
}

/// June2017 32453160 = March's AddSkillNoteTemplet_LUA: UNIT_CLASS (0 -> nothing), MEMO_LIST rows { memo item id, skill id } (-1 when
/// absent; a row needs both), the class inserted when it has rows (never replaces)
void CX2SkillNoteManager::AddSkillNoteTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	++m_iOldTempletCalls;

	int iUnitClass = 0;
	if( false == luaManager.GetValue( "UNIT_CLASS", iUnitClass ) || 0 == iUnitClass )
		return;

	std::vector< std::pair<int, int> > vecMemoList;
	if( true == luaManager.BeginTable( "MEMO_LIST" ) )
	{
		int iTableIndex = 1;
		while( true == luaManager.BeginTable( iTableIndex++ ) )
		{
			int iMemoItemID = -1;
			int iSkillID = -1;
			luaManager.GetValue( 1, iMemoItemID );
			luaManager.GetValue( 2, iSkillID );
			if( -1 != iMemoItemID && -1 != iSkillID )
				vecMemoList.push_back( std::make_pair( iMemoItemID, iSkillID ) );
			luaManager.EndTable();
		}
		luaManager.EndTable();
	}

	if( false == vecMemoList.empty() )
		m_mapSkillNoteTemplet.insert( std::make_pair( iUnitClass, vecMemoList ) );
}

namespace
{
	/// June2017 31eca1d0: UNIT_CLASS -> UNIT_TYPE, a switch (0 for any other class)
	int UnitTypeOfClass( int iUnitClass_ )
	{
		switch( iUnitClass_ )
		{
		case 1:   case 10:  case 11:  case 22:  case 100: case 101: case 112:	return 1;
		case 2:   case 14:  case 15:  case 23:  case 104: case 105: case 113:	return 2;
		case 3:   case 12:  case 13:  case 24:  case 102: case 103: case 114:	return 3;
		case 4:   case 16:  case 17:  case 25:  case 106: case 107: case 115:	return 4;
		case 5:   case 18:  case 19:  case 26:  case 108: case 109: case 116:	return 5;
		case 6:   case 20:  case 21:  case 27:  case 110: case 111: case 117:	return 6;
		case 7:   case 28:  case 31:  case 34:  case 118: case 119: case 124:	return 7;
		case 8:   case 29:  case 30:  case 35:  case 120: case 121: case 125:	return 8;
		case 9:   case 32:  case 33:  case 36:  case 122: case 123: case 126:	return 9;
		case 37:  case 38:  case 39:  case 127: case 128: case 129: case 200:	return 10;
		case 40:  case 41:  case 42:  case 43:  case 130: case 131: case 132: case 133: case 201:	return 11;
		case 44:  case 45:  case 46:  case 134: case 135: case 136: case 202:	return 12;
		default:	return 0;
		}
	}

	/// June2017 32368880: UNIT_TYPE -> the type's base class (0 for any other type)
	int BaseClassOfType( int iUnitType_ )
	{
		if( 1 <= iUnitType_ && iUnitType_ <= 9 )
			return iUnitType_;
		switch( iUnitType_ )
		{
		case 10:	return 200;
		case 11:	return 201;
		case 12:	return 202;
		default:	return 0;
		}
	}

	/// June2017 31eca340: UNIT_CLASS -> its job line 1-4 (0 for a base class or any other class)
	int LineOfClass( int iUnitClass_ )
	{
		switch( iUnitClass_ )
		{
		case 10:  case 12:  case 14:  case 16:  case 18:  case 20:  case 28:  case 29:  case 32:  case 37:  case 41:  case 44:
		case 100: case 102: case 104: case 106: case 108: case 110: case 118: case 120: case 122: case 127: case 131: case 134:
			return 1;
		case 11:  case 13:  case 15:  case 17:  case 19:  case 21:  case 30:  case 31:  case 33:  case 38:  case 40:  case 45:
		case 101: case 103: case 105: case 107: case 109: case 111: case 119: case 121: case 123: case 128: case 130: case 135:
			return 2;
		case 22:  case 23:  case 24:  case 25:  case 26:  case 27:  case 34:  case 35:  case 36:  case 39:  case 43:  case 46:
		case 112: case 113: case 114: case 115: case 116: case 117: case 124: case 125: case 126: case 129: case 133: case 136:
			return 3;
		case 42:  case 132:
			return 4;
		default:
			return 0;
		}
	}

	/// June2017 3234b390: the class at [ type ][ tier ][ line ] of the table at 337a1660 (BSS, filled by the static initializer
	/// @3234afb5-3234b389; read out of its stores). Tier 0 base, 1 first job, 2 second job; entries the initializer never writes are 0.
	int ClassAt( int iUnitType_, int iTier_, int iLine_ )
	{
		static const int s_aiClassTable[13][4][5] =
		{
			{ { 0, 0,   0,   0,   0   }, { 0, 0,   0,   0,   0   }, { 0, 0,   0,   0,   0   }, { 0 } },
			{ { 0, 1,   1,   1,   0   }, { 0, 10,  11,  22,  0   }, { 0, 100, 101, 112, 0   }, { 0 } },
			{ { 0, 2,   2,   2,   0   }, { 0, 14,  15,  23,  0   }, { 0, 104, 105, 113, 0   }, { 0 } },
			{ { 0, 3,   3,   3,   0   }, { 0, 12,  13,  24,  0   }, { 0, 102, 103, 114, 0   }, { 0 } },
			{ { 0, 4,   4,   4,   0   }, { 0, 16,  17,  25,  0   }, { 0, 106, 107, 115, 0   }, { 0 } },
			{ { 0, 5,   5,   5,   0   }, { 0, 18,  19,  26,  0   }, { 0, 108, 109, 116, 0   }, { 0 } },
			{ { 0, 6,   6,   6,   0   }, { 0, 20,  21,  27,  0   }, { 0, 110, 111, 117, 0   }, { 0 } },
			{ { 0, 7,   7,   7,   0   }, { 0, 28,  31,  34,  0   }, { 0, 118, 119, 124, 0   }, { 0 } },
			{ { 0, 8,   8,   8,   0   }, { 0, 29,  30,  35,  0   }, { 0, 120, 121, 125, 0   }, { 0 } },
			{ { 0, 9,   9,   9,   0   }, { 0, 32,  33,  36,  0   }, { 0, 122, 123, 126, 0   }, { 0 } },
			{ { 0, 200, 200, 0,   0   }, { 0, 37,  38,  39,  0   }, { 0, 127, 128, 129, 0   }, { 0 } },
			{ { 0, 201, 201, 201, 201 }, { 0, 41,  40,  43,  42  }, { 0, 131, 130, 133, 132 }, { 0 } },
			{ { 0, 202, 202, 202, 202 }, { 0, 44,  45,  46,  0   }, { 0, 134, 135, 136, 0   }, { 0 } },
		};
		if( iUnitType_ < 0 || 13 <= iUnitType_ || iTier_ < 0 || 4 <= iTier_ || iLine_ < 0 || 5 <= iLine_ )
			return 0;
		return s_aiClassTable[ iUnitType_ ][ iTier_ ][ iLine_ ];
	}

	/// June2017 32207210 (g_pData skill tree present): an [Enhanced] id reads as the base it replaces
	int MemoBaseSkillID( int iSkillID_ )
	{
		if( NULL == g_pData || NULL == g_pData->GetSkillTree() )
			return iSkillID_;
		return static_cast<int>( g_pData->GetSkillTree()->GetUpgradeBaseSkillID( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) ) );
	}
}

/// June2017 32454840. UNIT_CLASS (0 -> nothing). A first job (class 10-99) starts with its base class's rows, a second job (100-199)
/// with its first job's rows and then its base's, each inserted at the front (32455e30 at begin), so a second job holds its base's
/// rows twice (JUNE_BUGS). Then MEMO_LIST: { MEMO_ID, SKILL_ID, required level, up to five values }; memo, skill read 0 and the level -1
/// when absent, each value -1.0 (334380f0); a row with memo 0 is dropped. The class goes in with map insert (32457140: never replaces).
void CX2SkillNoteManager::AddNewSkillNoteTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	++m_iNewTempletCalls;

	int iUnitClass = 0;
	if( false == luaManager.GetValue( "UNIT_CLASS", iUnitClass ) || 0 == iUnitClass )
		return;

	std::vector<SSkillMemoTemplet> vecMemo;
	const int iTier = ( 9 < iUnitClass && iUnitClass < 200 ) ? ( ( 99 < iUnitClass ) ? 2 : 1 ) : 0;
	const int iUnitType = UnitTypeOfClass( iUnitClass );
	if( 2 == iTier )
	{
		/// @324549b9-324549f9: the first job on the same line
		const int iFirstJob = ClassAt( iUnitType, 1, LineOfClass( iUnitClass ) );
		std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.find( iFirstJob );
		/// June inserts from the found node without testing for end(); an absent parent would read the map head. Guarded here
		/// (June's own data always defines a parent first: none is missing), counted in the load line.
		if( m_mapMemoTemplet.end() != it )
			vecMemo.insert( vecMemo.begin(), it->second.begin(), it->second.end() );
		else
			++m_iMissingParents;
	}
	if( 1 <= iTier )
	{
		/// @32454a0b-32454a40: the base class of the type
		const int iBaseClass = BaseClassOfType( iUnitType );
		std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.find( iBaseClass );
		if( m_mapMemoTemplet.end() != it )
			vecMemo.insert( vecMemo.begin(), it->second.begin(), it->second.end() );
		else
			++m_iMissingParents;
	}
	m_iInheritedRows += static_cast<int>( vecMemo.size() );

	if( true == luaManager.BeginTable( "MEMO_LIST" ) )
	{
		int iTableIndex = 1;
		while( true == luaManager.BeginTable( iTableIndex++ ) )
		{
			SSkillMemoTemplet kMemo;
			int iValue = 0;
			kMemo.m_iType = ( true == luaManager.GetValue( 1, iValue ) ) ? iValue : 0;
			iValue = 0;
			kMemo.m_iSkillID = ( true == luaManager.GetValue( 2, iValue ) ) ? iValue : 0;
			iValue = -1;
			kMemo.m_iRequireLevel = ( true == luaManager.GetValue( 3, iValue ) ) ? iValue : -1;
			for( int i = 0; i < 5; ++i )
			{
				float fValue = -1.f;
				if( false == luaManager.GetValue( i + 4, fValue ) )
					fValue = -1.f;
				kMemo.m_vecValue.push_back( fValue );
			}

			if( 0 != kMemo.m_iType )
			{
				if( 0 == kMemo.m_iSkillID )
					++m_iZeroSkillRows;
				vecMemo.push_back( kMemo );
				++m_iOwnRows;
			}
			else
			{
				++m_iZeroMemoRows;
			}
			luaManager.EndTable();
		}
		luaManager.EndTable();
	}

	if( false == vecMemo.empty() )
	{
		if( false == m_mapMemoTemplet.insert( std::make_pair( iUnitClass, vecMemo ) ).second )
			++m_iDuplicateClasses;
	}
}

/// June2017 32452120: multimap insert ( MEMO_ID, item id ) at +0x14
void CX2SkillNoteManager::AddMemoIDMatchItemID_LUA( int iMemoID_, int iItemID_ )
{
	++m_iMatchCalls;
	m_mmapMemoMatchItemID.insert( std::make_pair( iMemoID_, iItemID_ ) );
}

/// June2017 32450f20: set insert at +0x24
void CX2SkillNoteManager::AddRemovableMemoItemID_LUA( int iItemID_ )
{
	++m_iRemovableCalls;
	m_setRemovableMemoItemID.insert( iItemID_ );
}

/// June2017 324511a0: the class has memo rows
bool CX2SkillNoteManager::HasMemoClass( int iUnitClass_ ) const
{
	return m_mapMemoTemplet.end() != m_mapMemoTemplet.find( iUnitClass_ );
}

/// June2017 324510e0: the class's first row with the base skill (32207210) and the memo
const SSkillMemoTemplet* CX2SkillNoteManager::GetMemo( int iSkillID_, int iMemoID_, int iUnitClass_ ) const
{
	const int iBaseSkillID = MemoBaseSkillID( iSkillID_ );
	std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.find( iUnitClass_ );
	if( m_mapMemoTemplet.end() == it )
		return NULL;
	for( std::vector<SSkillMemoTemplet>::const_iterator itMemo = it->second.begin(); itMemo != it->second.end(); ++itMemo )
	{
		if( itMemo->m_iSkillID == iBaseSkillID && itMemo->m_iType == iMemoID_ )
			return &( *itMemo );
	}
	return NULL;
}

/// June2017 32451140: the class's iIndex_-th row (from 0) with the base skill (32207210)
const SSkillMemoTemplet* CX2SkillNoteManager::GetMemoByIndex( int iSkillID_, int iIndex_, int iUnitClass_ ) const
{
	const int iBaseSkillID = MemoBaseSkillID( iSkillID_ );
	std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.find( iUnitClass_ );
	if( m_mapMemoTemplet.end() == it )
		return NULL;
	int iFound = 0;
	for( std::vector<SSkillMemoTemplet>::const_iterator itMemo = it->second.begin(); itMemo != it->second.end(); ++itMemo )
	{
		if( itMemo->m_iSkillID == iBaseSkillID )
		{
			if( iFound == iIndex_ )
				return &( *itMemo );
			++iFound;
		}
	}
	return NULL;
}

/// June2017 32451250 (filter struct { skill, level, class, memo }): every row of the class matching each non-zero filter; the skill
/// goes through 32207210 first, and an id 32207210 leaves at 0 matches any skill
void CX2SkillNoteManager::GetMemoList( int iSkillID_, int iRequireLevel_, int iUnitClass_, int iMemoID_,
	OUT std::vector<const SSkillMemoTemplet*>& vecMemo_ ) const
{
	const int iBaseSkillID = MemoBaseSkillID( iSkillID_ );
	std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.find( iUnitClass_ );
	if( m_mapMemoTemplet.end() == it )
		return;
	for( std::vector<SSkillMemoTemplet>::const_iterator itMemo = it->second.begin(); itMemo != it->second.end(); ++itMemo )
	{
		if( ( 0 == iRequireLevel_ || iRequireLevel_ == itMemo->m_iRequireLevel ) &&
			( 0 == iBaseSkillID || iBaseSkillID == itMemo->m_iSkillID ) &&
			( 0 == iMemoID_ || iMemoID_ == itMemo->m_iType ) )
			vecMemo_.push_back( &( *itMemo ) );
	}
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
/// June2017 32452150 ( skill, memo, class; ret 0xc ): the class's rows ( 32076c90 ), one with the memo ( row +0 ) and the base skill
/// ( 32207210, row +4 ) -> true. The row's value vector is copied per row and dropped ( 31e6ecd0 ); nothing is returned but the bool.
bool CX2SkillNoteManager::HasMemo( int iSkillID_, int iMemoID_, int iUnitClass_ ) const
{
	return NULL != GetMemo( iSkillID_, iMemoID_, iUnitClass_ );
}
#else SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
/// June2017 32452150: a copy of the values of the class's first row with the memo and the base skill (32207210); empty when none
std::vector<float> CX2SkillNoteManager::GetMemoValue( int iSkillID_, int iMemoID_, int iUnitClass_ ) const
{
	const SSkillMemoTemplet* pMemo = GetMemo( iSkillID_, iMemoID_, iUnitClass_ );
	if( NULL == pMemo )
		return std::vector<float>();
	return pMemo->m_vecValue;
}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET

/// June2017 32451ff0: the skill of the class's first MI_TRANSCENDENCE_1 / _2 row (19, 20) whose value 0 or value 1, truncated
/// (32fe25f0), is iValue_ (a value past the end reads 0); 0 when none. No base mapping.
int CX2SkillNoteManager::GetSkillIDByTranscendenceValue( int iValue_, int iUnitClass_ ) const
{
	std::map< int, std::vector<SSkillMemoTemplet> >::const_iterator it = m_mapMemoTemplet.find( iUnitClass_ );
	if( m_mapMemoTemplet.end() == it )
		return 0;
	for( std::vector<SSkillMemoTemplet>::const_iterator itMemo = it->second.begin(); itMemo != it->second.end(); ++itMemo )
	{
		if( 19 != itMemo->m_iType && 20 != itMemo->m_iType )
			continue;
		const int iValueCount = static_cast<int>( itMemo->m_vecValue.size() );
		const float fValue0 = ( 1 <= iValueCount ) ? itMemo->m_vecValue[0] : 0.f;
		const float fValue1 = ( 2 <= iValueCount ) ? itMemo->m_vecValue[1] : 0.f;
		if( iValue_ == static_cast<int>( fValue0 ) || iValue_ == static_cast<int>( fValue1 ) )
			return itMemo->m_iSkillID;
	}
	return 0;
}

/// June2017 32450f50: equal_range( memo ) on the +0x14 multimap, item ids appended
void CX2SkillNoteManager::GetMemoMatchItemIDList( int iMemoID_, OUT std::vector<int>& vecItemID_ ) const
{
	typedef std::multimap<int, int>::const_iterator MatchIt;
	std::pair<MatchIt, MatchIt> range = m_mmapMemoMatchItemID.equal_range( iMemoID_ );
	for( MatchIt it = range.first; it != range.second; ++it )
		vecItemID_.push_back( it->second );
}

/// June2017 32451210: every removable-memo item id, appended in set order
void CX2SkillNoteManager::GetRemovableMemoItemIDList( OUT std::vector<int>& vecItemID_ ) const
{
	for( std::set<int>::const_iterator it = m_setRemovableMemoItemID.begin(); it != m_setRemovableMemoItemID.end(); ++it )
		vecItemID_.push_back( *it );
}

/// June2017 32455150 (static; makes the manager if needed): 0 when the class has the row and the unit's level is below its level,
/// else 2 when MY selected unit's tree (g_pData +0x68 +0x90, +0x494) has the memo active for the base skill (32890f90), else 1
/*static*/ int CX2SkillNoteManager::GetMemoState( int iSkillID_, int iMemoID_, int iUnitLevel_, int iUnitClass_ )
{
	const int iBaseSkillID = MemoBaseSkillID( iSkillID_ );
	const SSkillMemoTemplet* pMemo = GetInstance()->GetMemo( iBaseSkillID, iMemoID_, iUnitClass_ );
	if( NULL != pMemo && iUnitLevel_ < pMemo->m_iRequireLevel )
		return 0;

	if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.IsMemoActive( iBaseSkillID, iMemoID_ ) )
		return 2;

	return 1;
}

#ifdef SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
/// June2017 32453420 ( out, memo ): the memo's name, GET_STRING by MEMO_ID ( with GET_STRING's "[S%1%]%2%" debug branch, 31c31060 +0x40 ).
/// 1-18 -> 35593-35610 ( 0x8b09.. ), 19 / 20 ( the two transcendence memos ) -> 35611, 22 -> 36206; 0, 21 and anything else -> L"".
/*static*/ std::wstring CX2SkillNoteManager::GetMemoName( int iMemoID_ )
{
	if( 1 <= iMemoID_ && iMemoID_ <= 18 )
	{
		const int iStringID = 35592 + iMemoID_;	/// GET_STRING pastes its argument into a boost::format '%' chain
		return GET_STRING( iStringID );
	}
	switch( iMemoID_ )
	{
	case 19:
	case 20:
		return GET_STRING( 35611 );		/// 0x8b1b
	case 22:
		return GET_STRING( 36206 );		/// 0x8d6e
	default:
		break;
	}
	return L"";
}

namespace
{
	/// the row's value[ i ], 0 past the end ( June reads it this way in every case of 324513e0, and in 32071cd0 )
	float MemoValueAt( const SSkillMemoTemplet& kMemo_, const int iIndex_ )
	{
		return ( 0 <= iIndex_ && iIndex_ < static_cast<int>( kMemo_.m_vecValue.size() ) ) ? kMemo_.m_vecValue[ iIndex_ ] : 0.f;
	}

	/// 324513e0 cases 19 / 20: L"Change the skill (@1)" with the named skill's name ( templet +0x14 ), L"" when it has no templet
	std::wstring MemoChangeSkillDesc( const int iSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );
		if( NULL == pTemplet )
			return L"";
		return GET_REPLACED_STRING( ( 35592, "L", pTemplet->m_wstrName ) );		/// 0x8b08
	}
}

/// June2017 324513e0 ( out, skill, memo, unit class; ret 0x10 ): memo 0 or no row ( 324510e0 ) -> L"". Otherwise by the row's MEMO_ID,
/// GET_REPLACED_STRING of the row's values ( v[i], 0 past the end ), most of them x 100.0 ( double 3342f360 ). The x87 is settled
/// opcode by opcode ( dc c9 = fmul st(1),st(0); de e9 = fsubp st(1),st(0) ), so @1 is always v0's figure and @2 v1's.
std::wstring CX2SkillNoteManager::GetMemoEffectDesc( int iSkillID_, int iMemoID_, int iUnitClass_ ) const
{
	if( 0 == iMemoID_ )
		return L"";
	const SSkillMemoTemplet* pMemo = GetMemo( iSkillID_, iMemoID_, iUnitClass_ );
	if( NULL == pMemo )
		return L"";

	const double v0 = static_cast<double>( MemoValueAt( *pMemo, 0 ) );
	const double v1 = static_cast<double>( MemoValueAt( *pMemo, 1 ) );
	const double v2 = static_cast<double>( MemoValueAt( *pMemo, 2 ) );

	switch( pMemo->m_iType )
	{
	case 1:		return GET_REPLACED_STRING( ( 35574, "f", v0 * 100.0 ) );					/// 0x8af6
	case 2:		return GET_REPLACED_STRING( ( 35575, "f", v0 * 100.0 ) );					/// 0x8af7
	case 3:		return GET_REPLACED_STRING( ( 35576, "f", v0 * 100.0 ) );					/// 0x8af8
	case 4:		return GET_REPLACED_STRING( ( 35577, "ff", v0 * 100.0, v1 * 100.0 ) );		/// 0x8af9
	case 5:		return GET_REPLACED_STRING( ( 35578, "ff", v0 * 100.0, v1 * 100.0 ) );		/// 0x8afa
	case 6:		return GET_REPLACED_STRING( ( 35579, "f", v1 * 100.0 ) );					/// 0x8afb: value 1, not 0 ( @324516fa )
	case 7:		return GET_REPLACED_STRING( ( 35580, "ff", v0 * 100.0, v1 * 100.0 ) );		/// 0x8afc
	case 8:		return GET_REPLACED_STRING( ( 43199, "fff", v0 * 100.0, v1 * 100.0, v2 * 100.0 ) );	/// 0xa8bf ( not 0x8afd )
	case 9:		return GET_REPLACED_STRING( ( 35582, "f", v0 * 100.0 ) );					/// 0x8afe
	case 10:	return GET_REPLACED_STRING( ( 35583, "ff", v0 * 100.0, v1 * 100.0 ) );		/// 0x8aff
	case 11:	return GET_REPLACED_STRING( ( 35584, "ff", v0 * 100.0, v1 * 100.0 ) );		/// 0x8b00
	case 12:	return GET_REPLACED_STRING( ( 43304, "f", v0 * 100.0 ) );					/// 0xa928 ( not 0x8b01 )
	case 13:	return GET_REPLACED_STRING( ( 35586, "f", v0 ) );							/// 0x8b02: as is
	case 14:	return GET_REPLACED_STRING( ( 35587, "f", v0 * 100.0 ) );					/// 0x8b03
	case 15:	return GET_REPLACED_STRING( ( 35588, "f", v0 * 100.0 ) );					/// 0x8b04
	case 16:	return GET_REPLACED_STRING( ( 35589, "f", v0 * 100.0 - 100.0 ) );			/// 0x8b05 ( @32451c07 fmul, fsubp )
	case 17:	return GET_REPLACED_STRING( ( 35590, "f", v0 * 100.0 ) );					/// 0x8b06
	case 18:	return GET_REPLACED_STRING( ( 35591, "f", -v0 ) );							/// 0x8b07 ( fchs )
	case 19:
		{
			/// @32451d21: value[ 1 ] when the skill's own templet is an upgraded one ( 32206d00 ), else value[ 0 ], truncated ( 32071cd0, 32fe25f0 )
			if( NULL == g_pData || NULL == g_pData->GetSkillTree() )
				return L"";
			const CX2SkillTree::SkillTemplet* pOwnTemplet = g_pData->GetSkillTree()->GetSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );
			const int iIndex = ( NULL != pOwnTemplet && 0 != pOwnTemplet->m_iDeleteSkillIDByUpgrade ) ? 1 : 0;
			return MemoChangeSkillDesc( static_cast<int>( MemoValueAt( *pMemo, iIndex ) ) );
		}
	case 20:
		{
			/// @32451ddd: value[ 0 ], truncated
			if( NULL == g_pData || NULL == g_pData->GetSkillTree() )
				return L"";
			return MemoChangeSkillDesc( static_cast<int>( MemoValueAt( *pMemo, 0 ) ) );
		}
	case 22:	return GET_REPLACED_STRING( ( 36207, "f", v0 * 100.0 ) );					/// 0x8d6f
	default:	break;		/// 21 and the rest: the jump table's default ( 32451f0d ), L""
	}
	return L"";
}

/// June2017 32453060 ( skill, memo ): unless the ACK is awaited ( 31d70e00 ), REQ 0x554 { 32207210( skill ), memo }, then wait for 0x555.
void CX2SkillNoteManager::SendActiveMemoReq( int iSkillID_, int iMemoID_ )
{
	if( NULL == g_pMain || NULL == g_pData || NULL == g_pData->GetServerProtocol() )
		return;
	if( true == g_pMain->IsWaitingServerPacket( EGS_ACTIVE_SKILL_MEMO_ACK ) )
		return;

	KEGS_ACTIVE_SKILL_MEMO_REQ kPacket;
	kPacket.m_iSkillID	= MemoBaseSkillID( iSkillID_ );
	kPacket.m_iMemoID	= iMemoID_;
	g_pData->GetServerProtocol()->SendPacket( EGS_ACTIVE_SKILL_MEMO_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ACTIVE_SKILL_MEMO_ACK );

	CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO REQ activate skill=%d memo=%d", kPacket.m_iSkillID, kPacket.m_iMemoID );
}

/// June2017 324530d0 ( skill, memo, item UID ): unless the ACK is awaited, REQ 0x556 with the three as given ( no base mapping ), then
/// wait for 0x557.
void CX2SkillNoteManager::SendDeactiveMemoReq( int iSkillID_, int iMemoID_, UidType iItemUID_ )
{
	if( NULL == g_pMain || NULL == g_pData || NULL == g_pData->GetServerProtocol() )
		return;
	if( true == g_pMain->IsWaitingServerPacket( EGS_DEACTIVE_SKILL_MEMO_ACK ) )
		return;

	KEGS_DEACTIVE_SKILL_MEMO_REQ kPacket;
	kPacket.m_iSkillID	= iSkillID_;
	kPacket.m_iMemoID	= iMemoID_;
	kPacket.m_iItemUID	= iItemUID_;
	g_pData->GetServerProtocol()->SendPacket( EGS_DEACTIVE_SKILL_MEMO_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DEACTIVE_SKILL_MEMO_ACK );

	CX2OfflineLog::Server( L"[JUNE_SKILL] MEMO REQ deactivate skill=%d memo=%d itemUID=%I64d", iSkillID_, iMemoID_, (__int64)iItemUID_ );
}
#endif SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET
#endif SERV_IRUHADEV_JUNE_SKILL_NOTE
