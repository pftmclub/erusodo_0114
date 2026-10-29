#include "StdAfx.h"
#include ".\x2locationmanager.h"
#include <algorithm>
#ifdef SERV_IRUHADEV_JUNE_QUEST_GATE
#include <set>
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_JUNE_QUEST_GATE
#ifdef SERV_IRUHADEV_RANOX_SCENE_DEBUG
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_RANOX_SCENE_DEBUG

CX2LocationManager::CX2LocationManager(void) :
m_eCurrentVillage( SEnum::VMI_INVALID ),
m_eOldVillage( SEnum::VMI_INVALID ),
m_vOldPos( 0, 0, 0 ),
m_vCurrentPos( 0, 0, 0 ),
m_iLineIndex( 0 ),
m_vLastPos( 0, 0, 0 ),
m_iLastPosLineIndex( 0 ),
m_ComeBackInfoFromDungeon(),
m_bIsRight(true)
{	
	OpenScript();
}

#ifdef SERV_IRUHADEV_JUNE_QUEST_GATE
//{{ Iruha : 2026-10-19 // June quest Q6 -- the test every reader of the templet's +0x70 makes ( 32322260 = CX2QuestManager::GetUnitCompleteQuest ):
//   June tests "+0x70 > 0 ( < 1 or == 0 at the warp lists ) and the quest is not complete" -> closed. Every id is positive, so the
//   readers share this one test. Each ( site, map, state ) is logged once so a per-frame reader does not flood the log.
bool CX2LocationManager::LocalMapTemplet::IsQuestGateOpen( const WCHAR* pwszSite_ ) const
{
	if( m_iRequireCompletedQuestID < 1 )
		return true;

	CX2QuestManager* pQuestManager = ( NULL != g_pData ) ? g_pData->GetQuestManager() : NULL;
	const bool bOpen = ( NULL != pQuestManager && true == pQuestManager->GetUnitCompleteQuest( m_iRequireCompletedQuestID ) );

	static std::set<std::wstring> s_setLogged;
	WCHAR wszKey[128] = {0};
	StringCchPrintf( wszKey, 128, L"%s|%d|%d", pwszSite_, (int) m_LocalMapID, bOpen ? 1 : 0 );
	if( true == s_setLogged.insert( wszKey ).second )
	{
		CX2OfflineLog::Server( L"[JUNE_QUEST] GATE %s: local map %d is %s ( quest %d %s )", pwszSite_, (int) m_LocalMapID,
			bOpen ? L"open" : L"closed", m_iRequireCompletedQuestID, bOpen ? L"complete" : L"not complete" );
	}

	return bOpen;
}
//}}
#endif SERV_IRUHADEV_JUNE_QUEST_GATE
CX2LocationManager::~CX2LocationManager(void)
{
	//메모리 해제 시켜줘야지?

	map< CX2LocationManager::WORLD_MAP_ID,	CX2LocationManager::WorldMapTemplet* >::iterator iWorldMapTemplet;
	for ( iWorldMapTemplet = m_mapWorldMapTemplet.begin(); iWorldMapTemplet != m_mapWorldMapTemplet.end(); iWorldMapTemplet++ )
	{
		CX2LocationManager::WorldMapTemplet* pWorldMapTemplet = (CX2LocationManager::WorldMapTemplet*)iWorldMapTemplet->second;
		SAFE_DELETE( pWorldMapTemplet );
	}
	m_mapWorldMapTemplet.clear();

	map< CX2LocationManager::LOCAL_MAP_ID,	CX2LocationManager::LocalMapTemplet* >::iterator iLocalMapTemplet;
	for ( iLocalMapTemplet = m_mapLocalMapTemplet.begin(); iLocalMapTemplet != m_mapLocalMapTemplet.end(); iLocalMapTemplet++ )
	{
		CX2LocationManager::LocalMapTemplet* pLocalMapTemplet = (CX2LocationManager::LocalMapTemplet*)iLocalMapTemplet->second;
		SAFE_DELETE( pLocalMapTemplet );
	}
	m_mapLocalMapTemplet.clear();

	map< SEnum::VILLAGE_MAP_ID,	CX2LocationManager::VillageTemplet* >::iterator iVillageTemplet;
	for ( iVillageTemplet = m_mapVillageTemplet.begin(); iVillageTemplet != m_mapVillageTemplet.end(); iVillageTemplet++ )
	{
		CX2LocationManager::VillageTemplet* pVillageTemplet = (CX2LocationManager::VillageTemplet*)iVillageTemplet->second;
		SAFE_DELETE( pVillageTemplet );
	}
	m_mapVillageTemplet.clear();

	map< CX2LocationManager::HOUSE_ID,	CX2LocationManager::HouseTemplet* >::iterator iHouseTemplet;
	for ( iHouseTemplet = m_mapHouseTemplet.begin(); iHouseTemplet != m_mapHouseTemplet.end(); iHouseTemplet++ )
	{
		CX2LocationManager::HouseTemplet* pHouseTemplet = (CX2LocationManager::HouseTemplet*)iHouseTemplet->second;
		SAFE_DELETE( pHouseTemplet );
	}
	m_mapHouseTemplet.clear();

	map< int, VillageStartPos* >::iterator iVillageMapStartPos;
	for ( iVillageMapStartPos = m_mapVillageMap.begin(); iVillageMapStartPos != m_mapVillageMap.end(); iVillageMapStartPos++ )
	{
		CX2LocationManager::VillageStartPos* pVillageStartPos = (CX2LocationManager::VillageStartPos*)iVillageMapStartPos->second;
		SAFE_DELETE( pVillageStartPos );
	}
	m_mapVillageMap.clear();

#ifdef QUEST_GUIDE
/*	m_mapPortalMoveInfo.clear();*/
#endif //QUEST_GUIDE
}


CX2LocationManager::WorldMapTemplet*	CX2LocationManager::GetWorldMapTemplet( WORLD_MAP_ID worldMapID )
{
	CX2LocationManager::WorldMapTemplet* pWorldMapTemplet = NULL;

	std::map<CX2LocationManager::WORLD_MAP_ID, CX2LocationManager::WorldMapTemplet*>::iterator mit;
	mit = m_mapWorldMapTemplet.find( worldMapID );

	if( mit != m_mapWorldMapTemplet.end() )
	{
		pWorldMapTemplet = mit->second;
	}

	return pWorldMapTemplet;
}

CX2LocationManager::LocalMapTemplet*	CX2LocationManager::GetLocalMapTemplet( LOCAL_MAP_ID localMapID )
{
	CX2LocationManager::LocalMapTemplet* pLocalMapTemplet = NULL;

	std::map<CX2LocationManager::LOCAL_MAP_ID, CX2LocationManager::LocalMapTemplet*>::iterator mit;
	mit = m_mapLocalMapTemplet.find( localMapID );

	if( mit != m_mapLocalMapTemplet.end() )
	{
		pLocalMapTemplet = mit->second;
	}

	return pLocalMapTemplet;
}

CX2LocationManager::VillageTemplet*		CX2LocationManager::GetVillageMapTemplet( SEnum::VILLAGE_MAP_ID villageMapTemplet )
{
	CX2LocationManager::VillageTemplet* pVillageTemplet = NULL;

	std::map<SEnum::VILLAGE_MAP_ID, CX2LocationManager::VillageTemplet*>::iterator mit;
	mit = m_mapVillageTemplet.find( villageMapTemplet );

	if( mit != m_mapVillageTemplet.end() )
	{
		pVillageTemplet = mit->second;
	}

	return pVillageTemplet;
}

CX2LocationManager::HouseTemplet*		CX2LocationManager::GetHouseTemplet( HOUSE_ID houseID )
{
	CX2LocationManager::HouseTemplet* pHouseTemplet = NULL;

	std::map<CX2LocationManager::HOUSE_ID, CX2LocationManager::HouseTemplet*>::iterator mit;
	mit = m_mapHouseTemplet.find( houseID );

	if( mit != m_mapHouseTemplet.end() )
	{
		pHouseTemplet = mit->second;
	}

	return pHouseTemplet;
}

void CX2LocationManager::OpenScript()
{
//{{ robobeg : 2008-10-28
	//KLuaManager kLuamanager;
    KLuaManager kLuamanager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//}} robobeg : 2008-10-28

//{{ robobeg : 2008-10-28
	//g_pKTDXApp->LoadAndDoMemory( &kLuamanager, "Enum.lua"			);
	//g_pKTDXApp->LoadAndDoMemory( &kLuamanager, "DLG_Map_Enum.lua"	);
//}} robobeg : 2008-10-28
	g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"DLG_Map_World.lua"	);
	g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"DLG_Map_Local.lua"	);
	g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"DLG_Map_Village.lua");
	g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"DLG_Map_House.lua"	);

	//World
	for ( int worldID = CX2LocationManager::WMI_INVALID + 1; worldID < CX2LocationManager::WMI_END; worldID++ )
	{
		wstring worldDialogFileName;

		if( kLuamanager.BeginTable( "World" ) == true )
		{
			kLuamanager.GetValue( worldID, worldDialogFileName );

			kLuamanager.EndTable();
		}
		else
		{
			MessageBox( g_pKTDXApp->GetHWND(), L"CX2LocationManager World OpenScript Error", L"Error", MB_OK );
			return;
		}
		g_pKTDXApp->LoadAndDoMemory( &kLuamanager, worldDialogFileName.c_str() );
		vector< LOCAL_MAP_ID > vecLocalMapID;

		if( kLuamanager.BeginTable( "LocalIDList" ) == true )
		{
			int tableIndex = 1;
			int localID = -1;
			while( kLuamanager.GetValue( tableIndex, localID ) == true )
			{
				vecLocalMapID.push_back( (LOCAL_MAP_ID)localID );
				tableIndex++;
			}

			kLuamanager.EndTable();
		}

		CX2LocationManager::WorldMapTemplet* pWorldMapTemplet = new CX2LocationManager::WorldMapTemplet();
		pWorldMapTemplet->m_WorldID = (CX2LocationManager::WORLD_MAP_ID)worldID;
		pWorldMapTemplet->m_LocalMapIDList = vecLocalMapID;
		pWorldMapTemplet->m_ScriptFileName = worldDialogFileName;

		m_mapWorldMapTemplet.insert( std::make_pair( (CX2LocationManager::WORLD_MAP_ID)worldID, pWorldMapTemplet ) );

		LocalParsing( kLuamanager, vecLocalMapID );

	}
}

void CX2LocationManager::LocalParsing( KLuaManager& kLuamanager, const vector< LOCAL_MAP_ID >& vecLocalMapID )
{
	//Local
	int villageID = -1;
	int villageDGID = -1;
	int villageDungeonLoungeID = -1;
	int iBattleFieldRestID = -1;

	for ( int i = 0; i < (int)vecLocalMapID.size(); i++ )
	{
		wstring localDialogFileName;
		int localMapID = vecLocalMapID[i];

	
		vector< SEnum::DUNGEON_ID > vecDungeonID;
		vector< SEnum::VILLAGE_MAP_ID > vecBattleFieldID;

		if( kLuamanager.BeginTable( "Local" ) == true )
		{
			kLuamanager.GetValue( localMapID, localDialogFileName );

			kLuamanager.EndTable();
		}
		else
		{
			MessageBox( g_pKTDXApp->GetHWND(), L"CX2LocationManager Local OpenScript Error", L"Error", MB_OK );
			return;
		}

		g_pKTDXApp->LoadAndDoMemory( &kLuamanager, localDialogFileName.c_str()	 );

		
		villageID	= -1;
		villageDGID = -1;
		villageDungeonLoungeID = -1;
		if( kLuamanager.BeginTable( "VillageData" ) == true )
		{
			LUA_GET_VALUE( kLuamanager, "ID", villageID, -1 );
			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "VillageDGData" ) == true )
		{
			LUA_GET_VALUE( kLuamanager, "ID", villageDGID, -1 );
			kLuamanager.EndTable();
		}


		if( kLuamanager.BeginTable( "VillageDungeonLoungeData" ) == true )
		{
			LUA_GET_VALUE( kLuamanager, "ID", villageDungeonLoungeID, -1 );
			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "BattleFieldRestData" ) == true )
		{
			LUA_GET_VALUE( kLuamanager, "ID", iBattleFieldRestID, -1 );
			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "DungeonIDList" ) == true )
		{
			int tableIndex = 1;
			int dungeonID = -1;
			while( kLuamanager.GetValue( tableIndex, dungeonID ) == true )
			{
				vecDungeonID.push_back( (SEnum::DUNGEON_ID)dungeonID );
				tableIndex++;
			}

			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "BattleFieldIDList" ) == true )
		{
			int tableIndex = 1;
			int BattleFiedlID = -1;
			while( kLuamanager.GetValue( tableIndex, BattleFiedlID ) == true )
			{
				vecBattleFieldID.push_back( (SEnum::VILLAGE_MAP_ID)BattleFiedlID );
				tableIndex++;
			}

			kLuamanager.EndTable();
		}

		CX2LocationManager::LocalMapTemplet* pLocalMapTemplet 
			= new CX2LocationManager::LocalMapTemplet( static_cast<const LOCAL_MAP_ID>( localMapID ),
			static_cast<const SEnum::VILLAGE_MAP_ID>( villageID ), static_cast<const SEnum::VILLAGE_MAP_ID>( villageDGID ), 
			static_cast<const SEnum::VILLAGE_MAP_ID>( villageDungeonLoungeID ), static_cast<const SEnum::VILLAGE_MAP_ID>( iBattleFieldRestID ),
			vecDungeonID, vecBattleFieldID, localDialogFileName, true );
		
		if( kLuamanager.BeginTable( "EnterCondition" ) == true )
		{			
			//LUA_GET_VALUE_ENUM( kLuamanager, "RequireClearDungeonID", pLocalMapTemplet->m_RequireClearDungeonID, SEnum::DUNGEON_ID, SEnum::DI_NONE );
			if( kLuamanager.BeginTable( "RequireClearDungeonID" ) == true )
			{
				int tableIndex = 1;
				int dungeonID = -1;
				while( kLuamanager.GetValue( tableIndex, dungeonID ) == true )
				{					
					if( dungeonID != 0 )
					{
						pLocalMapTemplet->m_vecRequireClearDungeonID.push_back( (int)dungeonID );
					}
					
					tableIndex++;
				}

				kLuamanager.EndTable();
			}


			
			int requireUnitLevel = 1;
			LUA_GET_VALUE( kLuamanager, "RequireUnitLevel", pLocalMapTemplet->m_RequireUnitLevel, 1 );
#ifdef SERV_IRUHADEV_JUNE_QUEST_GATE
			// June 32501bb0 @32502b6a: RequireCompletedQuestID -> templet +0x70, 0 when absent. Only June's Elysion sets it ( 22218 ).
			LUA_GET_VALUE( kLuamanager, "RequireCompletedQuestID", pLocalMapTemplet->m_iRequireCompletedQuestID, 0 );
			if( 0 < pLocalMapTemplet->m_iRequireCompletedQuestID )
			{
				CX2OfflineLog::Server( L"[JUNE_QUEST] GATE local map %d requires completed quest %d ( RequireCompletedQuestID )",
					localMapID, pLocalMapTemplet->m_iRequireCompletedQuestID );
			}
#endif SERV_IRUHADEV_JUNE_QUEST_GATE

			kLuamanager.EndTable(); // EnterCondition
		}

#ifdef SERV_IRUHADEV_RANOX_SCENE_DEBUG
		CX2OfflineLog::Server( L"[RANOXDBG] LocalParsing localMapID=%d villageID=%d villageDGID=%d villageDungeonLoungeID=%d iBattleFieldRestID=%d", localMapID, villageID, villageDGID, villageDungeonLoungeID, iBattleFieldRestID );
#endif SERV_IRUHADEV_RANOX_SCENE_DEBUG
		m_mapLocalMapTemplet.insert( std::make_pair( (CX2LocationManager::LOCAL_MAP_ID)localMapID, pLocalMapTemplet ) );

		if( -1 != villageID )
			VillageParsing( kLuamanager, villageID );

		if( -1 != villageDGID )
			VillageParsing( kLuamanager, villageDGID );

		if( -1 != villageDungeonLoungeID )
			VillageParsing( kLuamanager, villageDungeonLoungeID );

		if ( -1 != iBattleFieldRestID )
			VillageParsing( kLuamanager, iBattleFieldRestID );
	}

}

void CX2LocationManager::VillageParsing( KLuaManager& kLuamanager, const int villageID )
{
	//빌리지
	float fCameraDist = 1200.f;
	float fLookatHeight = 200.f;
	float fEyeHeight = 300.f;
	vector< HOUSE_ID > vecHouseID;

	if ( kLuamanager.BeginTable( "Village" ) == true )
	{
		if( kLuamanager.BeginTable( villageID ) == true )
		{	
			
			if( kLuamanager.BeginTable( "HouseIDList" ) == true )
			{
				int tableIndex = 1;
				int houseID = -1;
				while( kLuamanager.GetValue( tableIndex, houseID ) == true )
				{
					vecHouseID.push_back( (HOUSE_ID)houseID );
					tableIndex++;
				}

				kLuamanager.EndTable();
			}
					
			int worldId;
			LUA_GET_VALUE_ENUM( kLuamanager, "WORLD_ID", worldId, CX2World::WORLD_ID, CX2World::WI_NONE );
			m_mapWorldMap.insert(std::make_pair((int)villageID, worldId));			

			
			if( kLuamanager.BeginTable( "CAMERA" ) == true )
			{
				LUA_GET_VALUE( kLuamanager, 1, fCameraDist, 1.0f );
				LUA_GET_VALUE( kLuamanager, 2, fLookatHeight, 1.0f );
				LUA_GET_VALUE( kLuamanager, 3, fEyeHeight, 1.0f );
				kLuamanager.EndTable();
			}			

#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
			//{{ Iruha : 2026-10-17 // Atlas/Elysion E1d -- June VillageParsing 324ff4b0 reads two village keys March does not, after CAMERA
			//   and before START_POS: CameraModifyDistanceByPortal (miss 3000 @324ffa18, March's literal in the portal camera test) and the
			//   SPA_INFO table (@324ffa22: rows of Type, miss -1, and a LineIdx list; a row is kept even without LineIdx).
			float fCameraModifyDistanceByPortal = 3000.f;
			LUA_GET_VALUE( kLuamanager, "CameraModifyDistanceByPortal", fCameraModifyDistanceByPortal, 3000.f );

			std::vector<CX2LocationManager::SpaInfo> vecSpaInfo;
			if( kLuamanager.BeginTable( "SPA_INFO" ) == true )
			{
				int iSpaIndex = 1;
				while( kLuamanager.BeginTable( iSpaIndex ) == true )
				{
					CX2LocationManager::SpaInfo kSpaInfo;
					LUA_GET_VALUE( kLuamanager, "Type", kSpaInfo.m_iType, -1 );

					if( kLuamanager.BeginTable( "LineIdx" ) == true )
					{
						int iLineTableIndex = 1;
						int iLineIdx = -1;
						while( kLuamanager.GetValue( iLineTableIndex, iLineIdx ) == true )
						{
							kSpaInfo.m_vecLineIdx.push_back( iLineIdx );
							++iLineTableIndex;
						}
						kLuamanager.EndTable();	// LineIdx
					}

					vecSpaInfo.push_back( kSpaInfo );
					kLuamanager.EndTable();	// iSpaIndex
					++iSpaIndex;
				}
				kLuamanager.EndTable();	// SPA_INFO
			}
#ifdef SERV_IRUHADEV_ELYSION_DEBUG
			int iInternalPlaceRows = 0;
#endif SERV_IRUHADEV_ELYSION_DEBUG
			//}}
#endif SERV_IRUHADEV_ELYSION_VILLAGE
			if( kLuamanager.BeginTable( "START_POS" ) == true )
			{
				int index = 1;
				int startPosId = 0;
#ifdef QUEST_GUIDE //테스트
/*				vector<MapInfo> vecMapInfo;*/
#endif //QUEST_GUIDE
				while( kLuamanager.BeginTable( index ) == true )
				{
					VillageStartPos *pStartPosInfo = new CX2LocationManager::VillageStartPos();

					// 현재 포지션의 id를 설정한다.
					LUA_GET_VALUE( kLuamanager, "StartPosId", startPosId , 0 );

					// 현재 포지션을 가지는 마을번호를 설정한다.
					pStartPosInfo->m_VillageID = (SEnum::VILLAGE_MAP_ID)villageID;

					// 현재 포지션의 3d 위치를 설정한다.
					LUA_GET_VALUE( kLuamanager, "StartPosX", pStartPosInfo->m_StartPos.x , 0 );
					LUA_GET_VALUE( kLuamanager, "StartPosY", pStartPosInfo->m_StartPos.y , 0 );
					LUA_GET_VALUE( kLuamanager, "StartPosZ", pStartPosInfo->m_StartPos.z , 0 );
					
					LUA_GET_VALUE( kLuamanager, "IsMarket",	pStartPosInfo->m_bIsMarket , false );
					LUA_GET_VALUE( kLuamanager, "IsSummon",	pStartPosInfo->m_bIsSummon , false );
					LUA_GET_VALUE( kLuamanager, "IsWarp",		pStartPosInfo->m_bIsWarp , false );
					LUA_GET_VALUE( kLuamanager, "IsRight",		pStartPosInfo->m_bIsRight, true );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
					// June @3250046d-32500758, in June's order after IsRight
					LUA_GET_VALUE( kLuamanager, "IsGuildAgitEnter",	pStartPosInfo->m_bIsGuildAgitEnter, false );
					LUA_GET_VALUE( kLuamanager, "IsSpaEnter",		pStartPosInfo->m_bIsSpaEnter, false );
					LUA_GET_VALUE( kLuamanager, "IsSpaOut",			pStartPosInfo->m_bIsSpaOut, false );
#endif SERV_IRUHADEV_ELYSION_VILLAGE

					LUA_GET_VALUE( kLuamanager, "BattleFieldId",	pStartPosInfo->m_uiBattleFieldId , false );
					LUA_GET_VALUE( kLuamanager, "BattleFieldStartLineIndex",		pStartPosInfo->m_usBattleFieldStartLineIndex , false );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
					// June @32500ac4 / @32500be0, after June's OtherPlaceID / OtherPlaceStartLineIndex (= the two keys above under March's
					// names, Atlas A5's choice for this file)
					LUA_GET_VALUE( kLuamanager, "WaitingTime",				pStartPosInfo->m_fWaitingTime, 3.f );
					LUA_GET_VALUE( kLuamanager, "InternalPlaceStartPosID",	pStartPosInfo->m_usInternalPlaceStartPosID, 0 );
#ifdef SERV_IRUHADEV_ELYSION_DEBUG
					if( 0 != pStartPosInfo->m_usInternalPlaceStartPosID )
						++iInternalPlaceRows;
#endif SERV_IRUHADEV_ELYSION_DEBUG
#endif SERV_IRUHADEV_ELYSION_VILLAGE
					
					// 현재 포지션과 연결된 포지션을 설정한다.
// 					if( kLuamanager.BeginTable( "LinkPos" ) == true )
// 					{
// 						int LinkIndex	= 1; 
// 						int linkPos		= -1;
// 						while( kLuamanager.GetValue( LinkIndex, linkPos ) == true )
// 						{
// 							if( linkPos >= 0 )
// 								pStartPosInfo->m_vecLinkStartPos.push_back(linkPos);
// 
// 							++LinkIndex;
// 						}
// 
// 						kLuamanager.EndTable();						
// 					}
#ifdef QUEST_GUIDE //테스트
/*
					if( false != pStartPosInfo->m_uiBattleFieldId )
					{
						vecMapInfo.push_back( MapInfo(pStartPosInfo->m_uiBattleFieldId, startPosId) );
						}*/
#endif //QUEST_GUIDE


					kLuamanager.EndTable();
					index++;

#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
					// June @32500d94-32500dbb: when the StartPosId is already in the map the new row is freed (March leaked it)
					if( false == m_mapVillageMap.insert( std::make_pair( startPosId, pStartPosInfo ) ).second )
						SAFE_DELETE( pStartPosInfo );
#else SERV_IRUHADEV_ELYSION_VILLAGE
					m_mapVillageMap.insert( std::make_pair( startPosId, pStartPosInfo ) );							
#endif SERV_IRUHADEV_ELYSION_VILLAGE
				}

#ifdef QUEST_GUIDE
/*
				if( false == vecMapInfo.empty() )
				{
					m_mapPortalMoveInfo.insert( std::make_pair(static_cast<SEnum::VILLAGE_MAP_ID>(villageID), vecMapInfo) );
				}*/

#endif //QUEST_GUIDE

				kLuamanager.EndTable();
			}




			// 던전 라운지에서 유저 유닛의 시작 위치를 Parsing 
			std::map<CX2World::WORLD_ID, DungeonLoungeUserStartPos > mapDungeonLoungeUserStartPos;
			if( kLuamanager.BeginTable( "DUNGEON_LOUNGE_START_POS" ) == true )
			{
				int index = 1;
				while( kLuamanager.BeginTable( index ) == true )
				{
					CX2World::WORLD_ID eWorldID;
					LUA_GET_VALUE_ENUM( kLuamanager, "START_POS_WORLD_ID", eWorldID, CX2World::WORLD_ID, CX2World::WI_NONE );
					
					if( true == kLuamanager.BeginTable( "USER_START_POS" ) )
					{
						DungeonLoungeUserStartPos dungeonLoungeUserStartPos;

						int iStartPosIndex = 1;
						while( kLuamanager.BeginTable( iStartPosIndex ) == true )
						{
							D3DXVECTOR3 vUserPos;
							
							LUA_GET_VALUE( kLuamanager, "StartPosX", vUserPos.x , 0 );
							LUA_GET_VALUE( kLuamanager, "StartPosY", vUserPos.y , 0 );
							LUA_GET_VALUE( kLuamanager, "StartPosZ", vUserPos.z , 0 );

							dungeonLoungeUserStartPos.m_vecStartPos.push_back( vUserPos );

							kLuamanager.EndTable(); // iStartPosIndex 
							iStartPosIndex++;
						}

						if( false == dungeonLoungeUserStartPos.m_vecStartPos.empty() )
						{
							mapDungeonLoungeUserStartPos[ eWorldID ] = dungeonLoungeUserStartPos;
						}

						kLuamanager.EndTable(); // USER_START_POS
					}

					kLuamanager.EndTable(); // index
					index++;
				}

				kLuamanager.EndTable(); // DUNGEON_LOUNGE_START_POS
			}

			wstring frameMoveFuncName;
			LUA_GET_VALUE( kLuamanager, "FrameMoveFunc", frameMoveFuncName, L"" );

			CX2LocationManager::VillageTemplet* pVillageTemplet = new CX2LocationManager::VillageTemplet();
			pVillageTemplet->m_VillageID = (SEnum::VILLAGE_MAP_ID)villageID;
			pVillageTemplet->m_HouseList = vecHouseID;
			pVillageTemplet->m_FrameMoveFuncName = frameMoveFuncName;

			pVillageTemplet->m_fCameraDistance	= fCameraDist;
			pVillageTemplet->m_fLookAtHeight	= fLookatHeight;
			pVillageTemplet->m_fEyeHeight		= fEyeHeight;

			pVillageTemplet->m_mapDungeonLoungeUserStartPos = mapDungeonLoungeUserStartPos;
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
			pVillageTemplet->m_fCameraModifyDistanceByPortal	= fCameraModifyDistanceByPortal;	// June templet +0x58
			pVillageTemplet->m_vecSpaInfo						= vecSpaInfo;						// June templet +0x70 (@325019a6)
#ifdef SERV_IRUHADEV_ELYSION_DEBUG
			if( 0 < iInternalPlaceRows || false == vecSpaInfo.empty() || 3000.f != fCameraModifyDistanceByPortal )
			{
				WCHAR buff[256] = {0};
				StringCchPrintf( buff, 256, L"[ELYDBG] VillageParsing June keys: village %d internalPlaceRows %d spaInfo %d cameraByPortal %.0f",
					villageID, iInternalPlaceRows, (int)vecSpaInfo.size(), fCameraModifyDistanceByPortal );
				ErrorLogMsg( XEM_ERROR142, buff );
			}
#endif SERV_IRUHADEV_ELYSION_DEBUG
#endif SERV_IRUHADEV_ELYSION_VILLAGE

            int iIndex;
			LUA_GET_VALUE( kLuamanager, "NAME", iIndex,	STR_ID_EMPTY );
            pVillageTemplet->m_Name = GET_STRING( iIndex );

			LUA_GET_VALUE( kLuamanager, "IS_DUNGEON_RESULT_FIELD", pVillageTemplet->m_bIsDungeonResultField, false );






#ifdef HEAVY_LOG_TEST
			map< SEnum::VILLAGE_MAP_ID, VillageTemplet* >::iterator it_temp =
				m_mapVillageTemplet.find( (SEnum::VILLAGE_MAP_ID)villageID );
			ASSERT( it_temp == m_mapVillageTemplet.end() );
#endif HEAVY_LOG_TEST
#ifdef SERV_IRUHADEV_RANOX_SCENE_DEBUG
			CX2OfflineLog::Server( L"[RANOXDBG] VillageParsing insert villageID=%d worldId=%d", villageID, worldId );
#endif SERV_IRUHADEV_RANOX_SCENE_DEBUG


			m_mapVillageTemplet.insert( std::make_pair( (SEnum::VILLAGE_MAP_ID)villageID, pVillageTemplet ) );

			kLuamanager.EndTable();
		}
		else
		{
#ifdef SERV_IRUHADEV_RANOX_SCENE_DEBUG
			CX2OfflineLog::Server( L"[RANOXDBG] VillageParsing BeginTable(villageID) FAILED for villageID=%d", villageID );
#endif SERV_IRUHADEV_RANOX_SCENE_DEBUG
#ifdef SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION
			// Atlas A7 stand-in (user, 2026-10-17): June's DLG_Map_Local_Elysion names the Elysion village (20009)
			// before atlas E2 gives it a Village[] row. Skip that village with a log line instead of the popup, and
			// close the "Village" table first: March's return left it open, so every later village and rest area
			// (1008 included) failed to parse too. Idle once every named village has its row.
			{
				WCHAR buff[256] = {0};
				StringCchPrintf( buff, 256, L"[ELYDBG] VillageParsing: no Village[%d] row in DLG_Map_Village.lua, village skipped (a local map names it; Elysion 20009 = atlas E2)", villageID );
				ErrorLogMsg( XEM_ERROR142, buff );
			}
			kLuamanager.EndTable();	// "Village"
			return;
#else SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION
			MessageBox( g_pKTDXApp->GetHWND(), L"CX2LocationManager Village OpenScript Error", L"Error", MB_OK );
			return;
#endif SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION
		}

		kLuamanager.EndTable();
	}

	HouseParsing( kLuamanager, vecHouseID );

}

void CX2LocationManager::HouseParsing( KLuaManager& kLuamanager, const vector< HOUSE_ID >& vecHouseID )
{
	//하우스
	for ( int j = 0; j < (int)vecHouseID.size(); j++ )
	{
		int houseID = vecHouseID[j];

		map< HOUSE_ID, HouseTemplet* >::iterator it = m_mapHouseTemplet.find( (CX2LocationManager::HOUSE_ID) houseID );
		if( it != m_mapHouseTemplet.end() )
			continue;

		if ( kLuamanager.BeginTable( "House" ) == true )
		{

			if( kLuamanager.BeginTable( houseID ) == true )
			{
                int iStringIndex;

				int houseType;
				LUA_GET_VALUE( kLuamanager, "Type", houseType, -1 );

				wstring houseName;
				LUA_GET_VALUE( kLuamanager, "Name", iStringIndex, STR_ID_EMPTY );
                houseName = GET_STRING( iStringIndex );

				wstring npcTextureName;
				LUA_GET_VALUE( kLuamanager, "NPCTextureName", npcTextureName, L"" );

				wstring npcTextureKey;
				LUA_GET_VALUE( kLuamanager, "NPCTextureKey", npcTextureKey, L"" );


				float npcTexturePosX = 0.0f;
				float npcTexturePosY = 0.0f;
			
				LUA_GET_VALUE( kLuamanager, "NPCTexturePosX", npcTexturePosX, -999.0f );
				LUA_GET_VALUE( kLuamanager, "NPCTexturePosY", npcTexturePosY, -999.0f );

				float questInfoPosX = 0.0f;
				float questInfoPosY = 0.0f;

				LUA_GET_VALUE( kLuamanager, "QuestInfoPosX", questInfoPosX, 0.0f );
				LUA_GET_VALUE( kLuamanager, "QuestInfoPosY", questInfoPosY, 0.0f );

				float eventQuestInfoPosX = 0.0f;
				float eventQuestInfoPosY = 0.0f;

				LUA_GET_VALUE( kLuamanager, "EventQuestInfoPosX", eventQuestInfoPosX, 0.0f );
				LUA_GET_VALUE( kLuamanager, "EventQuestInfoPosY", eventQuestInfoPosY, 0.0f );


				wstring			titleTextureName;
				wstring			titleTextureKey;

				LUA_GET_VALUE( kLuamanager, "TitleTextureName", titleTextureName, L"" );
				LUA_GET_VALUE( kLuamanager, "TitleTextureKey", titleTextureKey, L"" );

				wstring			npcMeshName;
				LUA_GET_VALUE( kLuamanager, "NPCMeshName", npcMeshName, L"" );
				wstring			npcAniTex;
				LUA_GET_VALUE( kLuamanager, "NPCAniTex", npcAniTex, L"" );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				// Atlas E1a, June 324f47c2 .. 324f4d6e: second mesh and the mesh render switches
				wstring wstrNPCMeshName2nd, wstrNPCAniTex2nd;
				LUA_GET_VALUE( kLuamanager, "NPCMeshName2nd", wstrNPCMeshName2nd, L"" );
				LUA_GET_VALUE( kLuamanager, "NPCAniTex2nd", wstrNPCAniTex2nd, L"" );
				bool bAlphaBlend1stMesh = false, bAlphaBlend2ndMesh = false, bNoEdge2ndMesh = false;
				LUA_GET_VALUE( kLuamanager, "AlphaBlend1stMesh", bAlphaBlend1stMesh, false );
				LUA_GET_VALUE( kLuamanager, "AlphaBlend2ndMesh", bAlphaBlend2ndMesh, false );
				LUA_GET_VALUE( kLuamanager, "NoEdge2ndMesh", bNoEdge2ndMesh, false );
#endif SERV_IRUHADEV_ELYSION_VILLAGE
															
				vector<wstring> vecNpcJoinTalk;
				if( kLuamanager.BeginTable( "NPCJoinTalk" ) == true )
				{
					int tableIndex = 1;
					wstring npcTalk = L"";
					while( kLuamanager.GetValue( tableIndex, npcTalk ) == true )
					{
						vecNpcJoinTalk.push_back( npcTalk );
						tableIndex++;
					}

					kLuamanager.EndTable();
				}

				vector<wstring> vecNpcByeTalk;
				if( kLuamanager.BeginTable( "NPCByeTalk" ) == true )
				{
					int tableIndex = 1;
					wstring npcTalk = L"";
					while( kLuamanager.GetValue( tableIndex, npcTalk ) == true )
					{
						vecNpcByeTalk.push_back( npcTalk );
						tableIndex++;
					}

					kLuamanager.EndTable();
				}
				

				D3DXVECTOR3 npcMeshPos, npcMeshRot, npcMeshScale;
				if( kLuamanager.BeginTable( "NPCMeshPos" ) == true )
				{
					LUA_GET_VALUE( kLuamanager, 1, npcMeshPos.x, 0.0f );
					LUA_GET_VALUE( kLuamanager, 2, npcMeshPos.y, 0.0f );
					LUA_GET_VALUE( kLuamanager, 3, npcMeshPos.z, 0.0f );
					kLuamanager.EndTable();
				}
				if( kLuamanager.BeginTable( "NPCMeshRot" ) == true )
				{
					LUA_GET_VALUE( kLuamanager, 1, npcMeshRot.x, 0.0f );
					LUA_GET_VALUE( kLuamanager, 2, npcMeshRot.y, 0.0f );
					LUA_GET_VALUE( kLuamanager, 3, npcMeshRot.z, 0.0f );
					kLuamanager.EndTable();
				}				
				if( kLuamanager.BeginTable( "NPCMeshScale" ) == true )
				{
					LUA_GET_VALUE( kLuamanager, 1, npcMeshScale.x, 1.0f );
					LUA_GET_VALUE( kLuamanager, 2, npcMeshScale.y, 1.0f );
					LUA_GET_VALUE( kLuamanager, 3, npcMeshScale.z, 1.0f );
					kLuamanager.EndTable();
				}
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				// Atlas E1a, June 324f517b .. 324f53e7
				float fJoinAnimSpeed = 1.f, fWaitAnimSpeed = 1.f, fByeAnimSpeed = 1.f;
				LUA_GET_VALUE( kLuamanager, "JoinAnimSpeed", fJoinAnimSpeed, 1.f );
				LUA_GET_VALUE( kLuamanager, "WaitAnimSpeed", fWaitAnimSpeed, 1.f );
				LUA_GET_VALUE( kLuamanager, "ByeAnimSpeed", fByeAnimSpeed, 1.f );
#endif SERV_IRUHADEV_ELYSION_VILLAGE
				
				int joinArea;
				LUA_GET_VALUE( kLuamanager, "JoinArea", joinArea, 500 );

				//{{ kimhc // 2009-12-04 // 크리스마스 이벤트 나무
#ifdef	CHRISTMAS_TREE
				// Zbutton 입력을 사용할 것인가?
				bool	bCanTalkNpc = true;
				LUA_GET_VALUE( kLuamanager, "bCanTalkNpc",	bCanTalkNpc,	true );
#endif	CHRISTMAS_TREE
				//}} kimhc // 2009-12-04 // 크리스마스 이벤트 나무

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				bool	bExchangeNewItem = false;
				LUA_GET_VALUE( kLuamanager, "bExchangeNewItem",	bExchangeNewItem,	false );
#endif // SERV_NEW_ITEM_SYSTEM_2013_05
				bool bPvpNpc, bMakingNpc, bTraining, bFreeTraining, bEnchant, bAttribute, bRepair, bSell;
#ifdef GUILD_BOARD // oasis907 : 김상윤 [2010.2.24] // 
				bool bGuild			= false;
#endif GUILD_BOARD
				bool bRanking		= false;
				//{{ kimhc // 2009-08-06 // 캐릭터별 은행
#ifdef PRIVATE_BANK
				bool bPrivateBank	= false;
#endif PRIVATE_BANK
				//}} kimhc // 2009-08-06 // 캐릭터별 은행
#ifdef WEB_POINT_EVENT
				bool bWebEvent		= false;
#endif WEB_POINT_EVENT
#ifdef DEF_TRADE_BOARD
				bool bPersonalShop	= false;
#endif DEF_TRADE_BOARD
				//{{ 2011.05.04   임규수 아바타 합성 시스템
#ifdef SERV_SYNTHESIS_AVATAR
				bool bSynthesis		= false;
#endif SERV_SYNTHESIS_AVATAR
				//}}
				//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
#ifdef	ITEM_EXCHANGE_SHOP
				bool bExchange	= false;
#endif	ITEM_EXCHANGE_SHOP
				//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
#ifdef NPC_EVENT_BUTTON
				bool bEvent1 = false;
				bool bEvent2 = false;
				bool bEvent3 = false;
				bool bEvent4 = false;
#endif NPC_EVENT_BUTTON

				LUA_GET_VALUE( kLuamanager, "PVP_NPC",		bPvpNpc, false );
				LUA_GET_VALUE( kLuamanager, "MAKING_NPC",	bMakingNpc, false );
				LUA_GET_VALUE( kLuamanager, "TRAINING",	bTraining, false );
				LUA_GET_VALUE( kLuamanager, "FREE_TRAINING",bFreeTraining, false );
				LUA_GET_VALUE( kLuamanager, "ENCHANT",		bEnchant, false );
				LUA_GET_VALUE( kLuamanager, "ATTRIBUTE",	bAttribute, false );
				LUA_GET_VALUE( kLuamanager, "REPAIR_NPC",	bRepair, false );
				LUA_GET_VALUE( kLuamanager, "SELL_NPC",	bSell, false );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				bool bOldEnchant = false, bOldAdvancedEnchant = false;	// Atlas E1a, June 324f5ffd / 324f61b0
				LUA_GET_VALUE( kLuamanager, "OLD_ENCHANT", bOldEnchant, false );
				LUA_GET_VALUE( kLuamanager, "OLD_ADVANCED_ENCHANT", bOldAdvancedEnchant, false );
#endif SERV_IRUHADEV_ELYSION_VILLAGE

#ifdef GUILD_BOARD
				LUA_GET_VALUE( kLuamanager, "GUILD",		bGuild, false ); // oasis907 : 김상윤 [2009.11.24] // 길드 게시판
#endif GUILD_BOARD
				LUA_GET_VALUE( kLuamanager, "RANKING",		bRanking, false );
				//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
#ifdef	ITEM_EXCHANGE_SHOP
				LUA_GET_VALUE( kLuamanager, "EXCHANGE",	bExchange, false );
#endif	ITEM_EXCHANGE_SHOP
				//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

				//{{ kimhc // 2009-08-03 // 캐릭터별 은행
#ifdef PRIVATE_BANK
				LUA_GET_VALUE( kLuamanager, "PRIVATE_BANK", bPrivateBank, false );
#endif PRIVATE_BANK
				//}} kimhc // 2009-08-03 // 캐릭터별 은행

#ifdef WEB_POINT_EVENT // oasis907 : 김상윤 [2010.2.24] // 
				LUA_GET_VALUE( kLuamanager, "WEB_EVENT", bWebEvent, false );				
#endif WEB_POINT_EVENT
#ifdef DEF_TRADE_BOARD // oasis907 : 김상윤 [2010.3.12] // 
				LUA_GET_VALUE( kLuamanager, "PERSONAL_SHOP", bPersonalShop, false );				
#endif DEF_TRADE_BOARD
#ifdef SERV_PSHOP_AGENCY
				bool bTraderRegister = false;
				bool bTraderReceive = false;
				LUA_GET_VALUE( kLuamanager, "AGENCY_TRADER_REGISTER",	bTraderRegister, false );
				LUA_GET_VALUE( kLuamanager, "AGENCY_TRADER_RECEIVE",	bTraderReceive, false );
#endif
				//{{ 2011.05.04   임규수 아바타 합성 시스템
#ifdef SERV_SYNTHESIS_AVATAR
				LUA_GET_VALUE( kLuamanager, "SYNTHESIS", bSynthesis, false );				
#endif SERV_SYNTHESIS_AVATAR
				//}}
#ifdef NPC_EVENT_BUTTON
				LUA_GET_VALUE( kLuamanager, "EVENT1", bEvent1, false );
				LUA_GET_VALUE( kLuamanager, "EVENT2", bEvent2, false );
				LUA_GET_VALUE( kLuamanager, "EVENT3", bEvent3, false );
				LUA_GET_VALUE( kLuamanager, "EVENT4", bEvent4, false );
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				bool bPhotoZone = false, bFragmentExchange = false, bRockScissorsPaperNpc = false;	// Atlas E1a, June 324f6dac .. 324f70f4
				LUA_GET_VALUE( kLuamanager, "PHOTOZONE", bPhotoZone, false );
				LUA_GET_VALUE( kLuamanager, "FRAGMENT_EXCHANGE", bFragmentExchange, false );
				LUA_GET_VALUE( kLuamanager, "ROCKSCISSORSPAPER_NPC", bRockScissorsPaperNpc, false );
#endif SERV_IRUHADEV_ELYSION_VILLAGE
#endif NPC_EVENT_BUTTON
				// 이벤트NPC 설정
				bool bEventNpc;
				LUA_GET_VALUE( kLuamanager, "EVENT_NPC",	bEventNpc, false );
				float fStartYear, fStartMonth, fStartDay, fStartHour, fWaitMin, fActiveMin;
				fStartYear = fStartMonth = fStartDay = fStartHour = fWaitMin = fActiveMin = 0.f;
				if( kLuamanager.BeginTable( "EVENT_NPC_TIME" ) == true )
				{					
					LUA_GET_VALUE( kLuamanager, 1, fStartYear, 0.f );
					LUA_GET_VALUE( kLuamanager, 2, fStartMonth, 0.f );
					LUA_GET_VALUE( kLuamanager, 3, fStartDay, 0.f );
					LUA_GET_VALUE( kLuamanager, 4, fStartHour, 0.f );
					LUA_GET_VALUE( kLuamanager, 5, fActiveMin, 0.f );
					LUA_GET_VALUE( kLuamanager, 6, fWaitMin, 0.f );
					
					kLuamanager.EndTable();					
				}

				// 공용 NPC 위치 지정
				vector<CommonNpcPos> vecNpcPos;
				if( kLuamanager.BeginTable( "COMMON_NPC_LOCATION" ) == true )
				{
					int index = 1;
					while( kLuamanager.BeginTable( index ) == true )
					{
						CommonNpcPos npcPos;

						LUA_GET_VALUE_ENUM( kLuamanager, 1, npcPos.m_eWorldID, CX2World::WORLD_ID, CX2World::WI_NONE );
						LUA_GET_VALUE( kLuamanager, 2, npcPos.m_vNpcPos.x, 0.f );
						LUA_GET_VALUE( kLuamanager, 3, npcPos.m_vNpcPos.y, 0.f );
						LUA_GET_VALUE( kLuamanager, 4, npcPos.m_vNpcPos.z, 0.f );	
						LUA_GET_VALUE( kLuamanager, 5, npcPos.m_vNpcRot.x, 0.f );
						LUA_GET_VALUE( kLuamanager, 6, npcPos.m_vNpcRot.y, 0.f );
						LUA_GET_VALUE( kLuamanager, 7, npcPos.m_vNpcRot.z, 0.f );
#ifdef LOCAL_NPC_JOIN_AREA
						LUA_GET_VALUE( kLuamanager, 8, npcPos.m_fLocalJoinArea, 0.f );	
#endif LOCAL_NPC_JOIN_AREA						

						vecNpcPos.push_back(npcPos);
						kLuamanager.EndTable();
						++index;
					}					
					kLuamanager.EndTable();					
				}

				vector< CX2UnitManager::NPC_UNIT_ID > vecNPCID;		
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				// Atlas E1a, June 324f7493: CUSTOM_NPC_LOACION (June's spelling), entries laid out as COMMON_NPC_LOCATION's
				vector<CommonNpcPos> vecCustomNpcPos;
				if( kLuamanager.BeginTable( "CUSTOM_NPC_LOACION" ) == true )
				{
					int index = 1;
					while( kLuamanager.BeginTable( index ) == true )
					{
						CommonNpcPos npcPos;
						LUA_GET_VALUE_ENUM( kLuamanager, 1, npcPos.m_eWorldID, CX2World::WORLD_ID, CX2World::WI_NONE );
						LUA_GET_VALUE( kLuamanager, 2, npcPos.m_vNpcPos.x, 0.f );
						LUA_GET_VALUE( kLuamanager, 3, npcPos.m_vNpcPos.y, 0.f );
						LUA_GET_VALUE( kLuamanager, 4, npcPos.m_vNpcPos.z, 0.f );
						LUA_GET_VALUE( kLuamanager, 5, npcPos.m_vNpcRot.x, 0.f );
						LUA_GET_VALUE( kLuamanager, 6, npcPos.m_vNpcRot.y, 0.f );
						LUA_GET_VALUE( kLuamanager, 7, npcPos.m_vNpcRot.z, 0.f );
#ifdef LOCAL_NPC_JOIN_AREA
						LUA_GET_VALUE( kLuamanager, 8, npcPos.m_fLocalJoinArea, 0.f );
#endif LOCAL_NPC_JOIN_AREA
						vecCustomNpcPos.push_back( npcPos );
						kLuamanager.EndTable();
						++index;
					}
					kLuamanager.EndTable();
				}
#endif SERV_IRUHADEV_ELYSION_VILLAGE
				if( kLuamanager.BeginTable( "NPC_ID_LIST" ) == true )
				{
					int tableIndex = 1;
					int NPCID = -1;
					while( kLuamanager.GetValue( tableIndex, NPCID ) == true )
					{
						vecNPCID.push_back( (CX2UnitManager::NPC_UNIT_ID)NPCID );
						tableIndex++;
					}

					kLuamanager.EndTable();
				}

				//{{ kimhc // 2009-12-08 // 판매하는 아이템 별로 카테고리가 자동으로 정해지도록 수정
#ifndef	ADD_HOUSE_ID_TO_MANUFACTURE

		// 2008.12.9 김태완 : CX2Shop -> CX2UIShop 변환작업
		#ifdef NEW_VILLAGE_UI
						vector< CX2UIShop::BUY_ITEM_CATEGORY > vecShopCategoryType;
		#else
						vector< CX2Shop::BUY_ITEM_CATEGORY > vecShopCategoryType;
		#endif

						if( kLuamanager.BeginTable( "SHOP_CATEGORY_TYPE_LIST" ) == true )
						{
							int tableIndex = 1;
							int shopCateTypeID = -1;
							while( kLuamanager.GetValue( tableIndex, shopCateTypeID ) == true )
							{
		#ifdef NEW_VILLAGE_UI
								vecShopCategoryType.push_back( (CX2UIShop::BUY_ITEM_CATEGORY)shopCateTypeID );
		#else
								vecShopCategoryType.push_back( (CX2Shop::BUY_ITEM_CATEGORY)shopCateTypeID );
		#endif
								tableIndex++;
							}

							kLuamanager.EndTable();
						}

#endif	ADD_HOUSE_ID_TO_MANUFACTURE
					//}} kimhc // 2009-12-08 // 판매하는 아이템 별로 카테고리가 자동으로 정해지도록 수정

				wstring waitTalk, MakeTalk, shopTalk, questTalk, pvpTalk, byeTalk, exchangeTalk, rankingTalk;

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				wstring wstrExchangeNewItem;
#endif //SERV_NEW_ITEM_SYSTEM_2013_05
				
				//{{ kimhc // 2009-08-06 // 캐릭터별 은행
#ifdef PRIVATE_BANK
				wstring privateBankTalk;
#endif PRIVATE_BANK
				//}} kimhc // 2009-08-06 // 캐릭터별 은행
				
#ifdef GUILD_BOARD
				//{{ oasis907 : 김상윤 [2010.2.2] // 길드 게시판
				wstring guildADTalk;
				//}}
#endif GUILD_BOARD
	
#ifdef DEF_TRADE_BOARD
				//{{ oasis907 : 김상윤 [2010.3.31] // 
				wstring personalShopTalk;
				//}}
#endif DEF_TRADE_BOARD

				//{{ 2011.05.04   임규수 아바타 합성 시스템
#ifdef SERV_SYNTHESIS_AVATAR
				wstring SynthesisTalk;
#endif SERV_SYNTHESIS_AVATAR
				//}}
				LUA_GET_VALUE( kLuamanager, "Wait_Talk",	iStringIndex,	STR_ID_EMPTY );
                waitTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Make_Talk",	iStringIndex,	STR_ID_EMPTY );
                MakeTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Shop_Talk",	iStringIndex,	STR_ID_EMPTY );
                shopTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Pvp_Talk",	iStringIndex,	STR_ID_EMPTY );
                pvpTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Quest_Talk",	iStringIndex,	STR_ID_EMPTY );
                questTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Bye_Talk",	iStringIndex,	STR_ID_EMPTY );
                byeTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Exchange_Talk",	iStringIndex,	STR_ID_EMPTY );
				exchangeTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Ranking_Talk",	iStringIndex,	STR_ID_EMPTY );
				rankingTalk = GET_STRING( iStringIndex );
				//{{ kimhc // 2009-08-03 // 캐릭터별 은행
#ifdef PRIVATE_BANK
				LUA_GET_VALUE( kLuamanager, "PrivateBank_Talk", iStringIndex,	STR_ID_EMPTY );
				privateBankTalk		= GET_STRING( iStringIndex );
#endif PRIVATE_BANK
				//}} kimhc // 2009-08-03 // 캐릭터별 은행
				
#ifdef GUILD_BOARD
				//{{ oasis907 : 김상윤 [2010.2.2] // 길드 게시판
				LUA_GET_VALUE( kLuamanager, "GuildAD_Talk",	iStringIndex,	STR_ID_EMPTY );
				guildADTalk = GET_STRING( iStringIndex );
				//}}
#endif GUILD_BOARD

#ifdef DEF_TRADE_BOARD
				//{{ oasis907 : 김상윤 [2010.2.2] // 길드 게시판
				LUA_GET_VALUE( kLuamanager, "PersonalShop_Talk",	iStringIndex,	STR_ID_EMPTY );
				personalShopTalk = GET_STRING( iStringIndex );
				//}}
#endif DEF_TRADE_BOARD

#ifdef SERV_PSHOP_AGENCY
				wstring wstrRegisterTalk, wstrReceiveTalk;
				LUA_GET_VALUE( kLuamanager, "Register_Talk", iStringIndex, STR_ID_EMPTY );
				wstrRegisterTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Receive_Talk", iStringIndex, STR_ID_EMPTY );
				wstrReceiveTalk = GET_STRING( iStringIndex );
#endif
				//{{ 2011.05.04   임규수 아바타 합성 시스템
#ifdef SERV_SYNTHESIS_AVATAR
				LUA_GET_VALUE( kLuamanager, "Synthesis_Talk",	iStringIndex,	STR_ID_EMPTY );
				SynthesisTalk = GET_STRING( iStringIndex );
#endif SERV_SYNTHESIS_AVATAR
				//}}
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				LUA_GET_VALUE( kLuamanager, "Exchange_New_Item_Talk",	iStringIndex,	STR_ID_EMPTY );
				wstrExchangeNewItem = GET_STRING( iStringIndex );				
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				// Atlas E1a, June 324f980f .. 324fbb9d: the three June-only talks, two render values, the NPC_TALK_LIST<n> tables
				// and NPC_COMMON_TALK (each string id -1 -> L"")
				wstring wstrUpgradeTalk, wstrServiceTalk, wstrTradeTalk;
				LUA_GET_VALUE( kLuamanager, "Upgrade_Talk", iStringIndex, STR_ID_EMPTY );
				wstrUpgradeTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Service_Talk", iStringIndex, STR_ID_EMPTY );
				wstrServiceTalk = GET_STRING( iStringIndex );
				LUA_GET_VALUE( kLuamanager, "Trade_Talk", iStringIndex, STR_ID_EMPTY );
				wstrTradeTalk = GET_STRING( iStringIndex );
				float fOutlineWidthMesh = 1.2f;
				LUA_GET_VALUE( kLuamanager, "OutlineWidthMesh", fOutlineWidthMesh, 1.2f );
				bool bApplyAlphaTest = false;
				LUA_GET_VALUE( kLuamanager, "bApplyAlphaTest", bApplyAlphaTest, false );

				std::map< int, CX2LocationManager::HouseTemplet::NpcTalkList > mapNpcTalkList;
				for ( int iTalkList = 0; kLuamanager.BeginTable( "NPC_TALK_LIST", iTalkList ) == true; ++iTalkList )
				{
					CX2LocationManager::HouseTemplet::NpcTalkList kTalkList;
					if( kLuamanager.BeginTable( "Main_Talk" ) == true )
					{
						int iMainTalkIndex = 1;
						int iMainTalkID = STR_ID_EMPTY;
						while( kLuamanager.GetValue( iMainTalkIndex, iMainTalkID ) == true )
						{
							kTalkList.m_vecMainTalk.push_back( GET_STRING( iMainTalkID ) );
							++iMainTalkIndex;
						}
						kLuamanager.EndTable();
					}
					LUA_GET_VALUE( kLuamanager, "Receive_Like_Gift_Talk", iStringIndex, STR_ID_EMPTY );
					kTalkList.m_strReceiveLikeGiftTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "Receive_DisLike_Gift_Talk", iStringIndex, STR_ID_EMPTY );
					kTalkList.m_strReceiveDisLikeGiftTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "Receive_Normal_Gift_Talk", iStringIndex, STR_ID_EMPTY );
					kTalkList.m_strReceiveNormalGiftTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "Receive_Cash_Gift_Talk", iStringIndex, STR_ID_EMPTY );
					kTalkList.m_strReceiveCashGiftTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "Level_Up_Talk", iStringIndex, STR_ID_EMPTY );
					kTalkList.m_strLevelUpTalk = GET_STRING( iStringIndex );
					kLuamanager.EndTable();	// NPC_TALK_LIST<n>
					mapNpcTalkList.insert( std::make_pair( iTalkList, kTalkList ) );
				}

				wstring wstrAfterTalk, wstrEveryDayRewardTalk, wstrRejectGiftTalk, wstrGiftUITalk;
				if( kLuamanager.BeginTable( "NPC_COMMON_TALK" ) == true )
				{
					LUA_GET_VALUE( kLuamanager, "Every_Day_Reward_Talk", iStringIndex, STR_ID_EMPTY );
					wstrEveryDayRewardTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "After_Talk", iStringIndex, STR_ID_EMPTY );
					wstrAfterTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "Reject_Gift_Talk", iStringIndex, STR_ID_EMPTY );
					wstrRejectGiftTalk = GET_STRING( iStringIndex );
					LUA_GET_VALUE( kLuamanager, "Gift_UI_Talk", iStringIndex, STR_ID_EMPTY );
					wstrGiftUITalk = GET_STRING( iStringIndex );
					kLuamanager.EndTable();
				}
#endif SERV_IRUHADEV_ELYSION_VILLAGE
				CX2LocationManager::HouseTemplet* pHouseTemplet = new CX2LocationManager::HouseTemplet();
				pHouseTemplet->m_ID = (HOUSE_ID)houseID;
				pHouseTemplet->m_Type = (HOUSE_TYPE)houseType;
				pHouseTemplet->m_HouseName = houseName;

				pHouseTemplet->m_NPCTextureName = npcTextureName;
				pHouseTemplet->m_NPCTextureKey = npcTextureKey;
				pHouseTemplet->m_NPCTexturePos.x = npcTexturePosX;
				pHouseTemplet->m_NPCTexturePos.y = npcTexturePosY;

				pHouseTemplet->m_QuestInfoPos.x = questInfoPosX;
				pHouseTemplet->m_QuestInfoPos.y = questInfoPosY;

				pHouseTemplet->m_EventQuestInfoPos.x = eventQuestInfoPosX;
				pHouseTemplet->m_EventQuestInfoPos.y = eventQuestInfoPosY;		

				pHouseTemplet->m_TitleTextureName	= titleTextureName;
				pHouseTemplet->m_TitleTextureKey	= titleTextureKey;

				pHouseTemplet->m_NPCList = vecNPCID;

				//{{ kimhc // 2009-12-08 // 판매하는 아이템 별로 카테고리가 자동으로 정해지도록 수정
#ifndef	ADD_HOUSE_ID_TO_MANUFACTURE
				pHouseTemplet->m_ShopCategoryTypeList = vecShopCategoryType;
#endif	ADD_HOUSE_ID_TO_MANUFACTURE
				//}} kimhc // 2009-12-08 // 판매하는 아이템 별로 카테고리가 자동으로 정해지도록 수정

				pHouseTemplet->m_NPCMeshName	= npcMeshName;
				pHouseTemplet->m_NPCAniTex		= npcAniTex;

				pHouseTemplet->m_vecNPCJoinTalkSound = vecNpcJoinTalk;
				pHouseTemplet->m_vecNPCByeTalkSound	 = vecNpcByeTalk;
				
				pHouseTemplet->m_NPCMeshPos		= npcMeshPos;
				pHouseTemplet->m_NPCMeshRot		= npcMeshRot;
				pHouseTemplet->m_NPCMeshScal	= npcMeshScale;

				pHouseTemplet->m_NPCJoinArea	= joinArea;
				pHouseTemplet->m_bPvpNpc		= bPvpNpc;
				pHouseTemplet->m_bMakingNpc		= bMakingNpc;
				pHouseTemplet->m_bTraining		= bTraining;
				pHouseTemplet->m_bFreeTraining	= bFreeTraining;
				pHouseTemplet->m_bEnchant		= bEnchant;					
				pHouseTemplet->m_bAttribute		= bAttribute;
				pHouseTemplet->m_bRepair		= bRepair;
				pHouseTemplet->m_bSell			= bSell;
#ifdef GUILD_BOARD
				pHouseTemplet->m_bGuild			= bGuild;
#endif GUILD_BOARD
				pHouseTemplet->m_bRanking		= bRanking;

				//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
#ifdef	ITEM_EXCHANGE_SHOP
				pHouseTemplet->m_bExchange		= bExchange;
#endif	ITEM_EXCHANGE_SHOP
				//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

				//{{ kimhc // 2009-08-03 // 캐릭터별 은행
#ifdef PRIVATE_BANK
				pHouseTemplet->m_bPrivateBank	= bPrivateBank;
#endif PRIVATE_BANK
				//}} kimhc // 2009-08-03 // 캐릭터별 은행

#ifdef WEB_POINT_EVENT // oasis907 : 김상윤 [2010.2.24] // 
				pHouseTemplet->m_bWebEvent	= bWebEvent;
#endif WEB_POINT_EVENT
#ifdef DEF_TRADE_BOARD
				pHouseTemplet->m_bPersonalShop = bPersonalShop;
#endif DEF_TRADE_BOARD
#ifdef SERV_PSHOP_AGENCY
				pHouseTemplet->m_bAgencyTraderRegister	= bTraderRegister;
				pHouseTemplet->m_bAgencyTraderReceive	= bTraderReceive;

				pHouseTemplet->m_strRegisterTalk = wstrRegisterTalk;
				pHouseTemplet->m_strReceiveTalk = wstrReceiveTalk;
#endif
				//{{ 2011.05.04   임규수 아바타 합성 시스템
#ifdef SERV_SYNTHESIS_AVATAR
				pHouseTemplet->m_bSynthesis	= bSynthesis;
#endif SERV_SYNTHESIS_AVATAR
				//}}
				//{{ kimhc // 2009-12-04 // 크리스마스 이벤트 나무
#ifdef	CHRISTMAS_TREE
				// Zbutton 입력을 사용할 것인가?
				pHouseTemplet->m_bCanTalkNpc = bCanTalkNpc;				
#endif	CHRISTMAS_TREE
				//}} kimhc // 2009-12-04 // 크리스마스 이벤트 나무
#ifdef NPC_EVENT_BUTTON
				pHouseTemplet->m_bEvent1 = bEvent1;
				pHouseTemplet->m_bEvent2 = bEvent2;
				pHouseTemplet->m_bEvent3 = bEvent3;
				pHouseTemplet->m_bEvent4 = bEvent4;
#endif NPC_EVENT_BUTTON

#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				pHouseTemplet->m_bExchangeNewItem = bExchangeNewItem;
#endif // SERV_NEW_ITEM_SYSTEM_2013_05

				// 이벤트 npc 설정
				pHouseTemplet->m_bEventNpc		= bEventNpc;
				pHouseTemplet->m_fStartYear		= fStartYear;
				pHouseTemplet->m_fStartMonth	= fStartMonth;
				pHouseTemplet->m_fStartDay		= fStartDay;
				pHouseTemplet->m_fStartHour		= fStartHour;
				pHouseTemplet->m_fWaitMin		= fWaitMin;
				pHouseTemplet->m_fActiveMin		= fActiveMin;

				pHouseTemplet->m_strWaitTalk	= waitTalk;
				pHouseTemplet->m_strMakeTalk	= MakeTalk;				
				pHouseTemplet->m_strShopTalk	= shopTalk;
				pHouseTemplet->m_strPvpTalk		= pvpTalk;
				pHouseTemplet->m_strQuestTalk	= questTalk;
				pHouseTemplet->m_strByeTalk		= byeTalk;
				pHouseTemplet->m_strRankingTalk	= rankingTalk;
				pHouseTemplet->m_strExchangeTalk = exchangeTalk;
#ifdef SERV_NEW_ITEM_SYSTEM_2013_05
				pHouseTemplet->m_strExchangeNewItemTalk = wstrExchangeNewItem;				
#endif //SERV_NEW_ITEM_SYSTEM_2013_05

				//{{ kimhc // 2009-08-03 // 캐릭터별 은행
#ifdef PRIVATE_BANK
				pHouseTemplet->m_strPrivateBank		= privateBankTalk;
#endif PRIVATE_BANK
				//}} kimhc // 2009-08-03 // 캐릭터별 은행

#ifdef GUILD_BOARD
				//{{ oasis907 : 김상윤 [2010.2.2] // 
				pHouseTemplet->m_strGuildADTalk		= guildADTalk;
				//}}
#endif GUILD_BOARD
#ifdef DEF_TRADE_BOARD
				pHouseTemplet->m_strPersonalShopTalk	= personalShopTalk;
#endif DEF_TRADE_BOARD
				//{{ 2011.05.04   임규수 아바타 합성 시스템
#ifdef SERV_SYNTHESIS_AVATAR
				pHouseTemplet->m_strSynthesisTalk	= SynthesisTalk;
#endif SERV_SYNTHESIS_AVATAR
				//}}

				pHouseTemplet->m_vecCommonPos = vecNpcPos;

#ifdef TALK_BOX_VILLAGE_NPC
				pHouseTemplet->m_vecNPCTalkBox.clear();

				if( kLuamanager.BeginTable( "TALK_BOX" ) == true )
				{
					int index = 1;
					while( kLuamanager.BeginTable( index ) == true )
					{
						int iStringIndex;
						LUA_GET_VALUE( kLuamanager, "MESSAGE",	iStringIndex,	STR_ID_EMPTY );
						pHouseTemplet->m_vecNPCTalkBox.push_back( GET_STRING( iStringIndex ) );

						kLuamanager.EndTable();
						index++;
					}
					kLuamanager.EndTable();
				}
#endif

#ifdef ADD_PLAY_MUSIC_WHEN_VILLAGE_NPC_NEAR // 마을 NPC 에 일정 거리 이상 가까워지면 n초 간격으로 사운드를 출력하는 기능 추가
				if( kLuamanager.BeginTable( "NPCNearSound" ) == true )
				{
					int index = 1;
					while( kLuamanager.BeginTable( index ) == true )
					{

						LUA_GET_VALUE( kLuamanager, 1,	pHouseTemplet->m_wstrNearSoundFileName,		L"" );
						LUA_GET_VALUE( kLuamanager, 2,	pHouseTemplet->m_fPlayNearSoundCoolTime,	-1.f );
						LUA_GET_VALUE( kLuamanager, 3,	pHouseTemplet->m_fPlayNearSoundDistance,	-1.f );
						kLuamanager.EndTable();
						++index;	
					}
					kLuamanager.EndTable();					
				}
#endif // ADD_PLAY_MUSIC_WHEN_VILLAGE_NPC_CLOSE // 마을 NPC 에 일정 거리 이상 가까워지면 n초 간격으로 사운드를 출력하는 기능 추가


#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
				// Atlas E1a: the June-only values read above (June's copy section 324fbbcc .. 324fc122) ...
				pHouseTemplet->m_wstrNPCMeshName2nd		= wstrNPCMeshName2nd;
				pHouseTemplet->m_wstrNPCAniTex2nd		= wstrNPCAniTex2nd;
				pHouseTemplet->m_bAlphaBlend1stMesh		= bAlphaBlend1stMesh;
				pHouseTemplet->m_bAlphaBlend2ndMesh		= bAlphaBlend2ndMesh;
				pHouseTemplet->m_bNoEdge2ndMesh			= bNoEdge2ndMesh;
				pHouseTemplet->m_wstrUIFileName			= L"";		// June @324fbd1a
				pHouseTemplet->m_fJoinAnimSpeed			= fJoinAnimSpeed;
				pHouseTemplet->m_fWaitAnimSpeed			= fWaitAnimSpeed;
				pHouseTemplet->m_fByeAnimSpeed			= fByeAnimSpeed;
				pHouseTemplet->m_fOutlineWidthMesh		= fOutlineWidthMesh;
				pHouseTemplet->m_bApplyAlphaTest		= bApplyAlphaTest;
				pHouseTemplet->m_bOldEnchant			= bOldEnchant;
				pHouseTemplet->m_bOldAdvancedEnchant	= bOldAdvancedEnchant;
				pHouseTemplet->m_bPhotoZone				= bPhotoZone;
				pHouseTemplet->m_bFragmentExchange		= bFragmentExchange;
				pHouseTemplet->m_bRockScissorsPaperNpc	= bRockScissorsPaperNpc;
				pHouseTemplet->m_vecCustomNpcPos		= vecCustomNpcPos;
				pHouseTemplet->m_strUpgradeTalk			= wstrUpgradeTalk;
				pHouseTemplet->m_strServiceTalk			= wstrServiceTalk;
				pHouseTemplet->m_strTradeTalk			= wstrTradeTalk;
				pHouseTemplet->m_mapNpcTalkList			= mapNpcTalkList;
				pHouseTemplet->m_strAfterTalk			= wstrAfterTalk;
				pHouseTemplet->m_strEveryDayRewardTalk	= wstrEveryDayRewardTalk;
				pHouseTemplet->m_strRejectGiftTalk		= wstrRejectGiftTalk;
				pHouseTemplet->m_strGiftUITalk			= wstrGiftUITalk;

				// ... and the keys June reads straight into the templet after NPCNearSound (324fc642 .. 324fecbd)
				LUA_GET_VALUE( kLuamanager, "HENIR_EQUIP_UPGRADE",	pHouseTemplet->m_bHenirEquipUpgrade,	false );
				LUA_GET_VALUE( kLuamanager, "GUILD_AGIT_SERVICE",	pHouseTemplet->m_bGuildAgitService,		false );
				LUA_GET_VALUE( kLuamanager, "GUILD_ENTER",			pHouseTemplet->m_bGuildEnter,			false );
				LUA_GET_VALUE( kLuamanager, "EXTRACTION",			pHouseTemplet->m_bExtraction,			false );
				LUA_GET_VALUE( kLuamanager, "TRADE",				pHouseTemplet->m_bTrade,				false );
				LUA_GET_VALUE( kLuamanager, "TalkLoopAniNpc",		pHouseTemplet->m_bTalkLoopAniNpc,		false );
				LUA_GET_VALUE( kLuamanager, "WAIT01_MOTION_NAME",	pHouseTemplet->m_wstrWait01MotionName,		L"Wait01" );
				LUA_GET_VALUE( kLuamanager, "WAIT02_MOTION_NAME",	pHouseTemplet->m_wstrWait02MotionName,		L"Wait02" );
				LUA_GET_VALUE( kLuamanager, "WAIT03_MOTION_NAME",	pHouseTemplet->m_wstrWait03MotionName,		L"Wait03" );
				LUA_GET_VALUE( kLuamanager, "TALKLOOP_MOTION_NAME",	pHouseTemplet->m_wstrTalkLoopMotionName,	L"Talkloop" );
				LUA_GET_VALUE( kLuamanager, "TALK_MOTION_NAME",		pHouseTemplet->m_wstrTalkMotionName,		L"Talk" );
				LUA_GET_VALUE( kLuamanager, "TALKWAIT_MOTION_NAME",	pHouseTemplet->m_wstrTalkWaitMotionName,	L"TalkWait" );
				LUA_GET_VALUE( kLuamanager, "BYE_MOTION_NAME",		pHouseTemplet->m_wstrByeMotionName,			L"Bye" );
				LUA_GET_VALUE( kLuamanager, "NORMAL_MOTION_NAME",	pHouseTemplet->m_wstrNormalMotionName,		L"Normal" );

				pHouseTemplet->m_vecTalkChangeCondition.clear();
				if( kLuamanager.BeginTable( "Talk_Change_Condition" ) == true )
				{
					int index = 1;
					while( kLuamanager.BeginTable( index ) == true )
					{
						CX2LocationManager::HouseTemplet::TalkChangeCondition kCondition;
						LUA_GET_VALUE( kLuamanager, "TYPE",		kCondition.m_iType,		1 );
						LUA_GET_VALUE( kLuamanager, "STATE",	kCondition.m_iState,	0 );
						if( 0 == kCondition.m_iType )	// June @324fdaa9: any other TYPE (a missing one included) drops the row
						{
							LUA_GET_VALUE( kLuamanager, "QUEST_ID",		kCondition.m_iQuestID,		0 );
							LUA_GET_VALUE( kLuamanager, "SUB_QUEST_ID",	kCondition.m_iSubQuestID,	0 );
							LUA_GET_VALUE( kLuamanager, "MESSAGE",		iStringIndex,				STR_ID_EMPTY );
							kCondition.m_wstrMessage = GET_STRING( iStringIndex );
							LUA_GET_VALUE( kLuamanager, "MOTION",		kCondition.m_wstrMotion,	L"" );
							pHouseTemplet->m_vecTalkChangeCondition.push_back( kCondition );
						}
						kLuamanager.EndTable();
						++index;
					}
					kLuamanager.EndTable();
				}

				LUA_GET_VALUE( kLuamanager, "AlwaysShow",		pHouseTemplet->m_bAlwaysShow,		false );
				LUA_GET_VALUE( kLuamanager, "bOnlyClickable",	pHouseTemplet->m_bOnlyClickable,	false );

				pHouseTemplet->m_vecCustomMotionName.clear();	// June @324fe32a assigns the (maybe empty) list either way
				if( kLuamanager.BeginTable( "CUSTOM_MOTION_NAME" ) == true )
				{
					int index = 1;
					wstring wstrMotionName = L"";
					while( kLuamanager.GetValue( index, wstrMotionName ) == true )
					{
						CX2LocationManager::HouseTemplet::CustomMotion kMotion;
						kMotion.m_wstrMotionName = wstrMotionName;
						kMotion.m_bPlayed = false;
						pHouseTemplet->m_vecCustomMotionName.push_back( kMotion );
						++index;
					}
					kLuamanager.EndTable();
				}

				LUA_GET_VALUE( kLuamanager, "bMovingNPC", pHouseTemplet->m_bMovingNPC, false );

				// June 324fe434: the block is built with its defaults first and copied on both paths, so a house without the
				// table gets UseRenderOption off, Src 5 / Dest 6, colour 1
				{
					SVillageNpcRenderOption kRenderOption;
					if( kLuamanager.BeginTable( "RenderOption" ) == true )
					{
						LUA_GET_VALUE( kLuamanager, "UseRenderOption",	kRenderOption.m_bUseRenderOption,	false );
						LUA_GET_VALUE( kLuamanager, "UseAlphaBlend",	kRenderOption.m_bUseAlphaBlend,		false );
						LUA_GET_VALUE( kLuamanager, "MultiTex",			kRenderOption.m_wstrMultiTex,		L"" );
						if( kLuamanager.BeginTable( "MultiTexSpeed" ) == true )
						{
							LUA_GET_VALUE( kLuamanager, 1, kRenderOption.m_vMultiTexSpeed.x, 0.f );
							LUA_GET_VALUE( kLuamanager, 2, kRenderOption.m_vMultiTexSpeed.y, 0.f );
							LUA_GET_VALUE( kLuamanager, 3, kRenderOption.m_vMultiTexSpeed.z, 0.f );
							kLuamanager.EndTable();
						}
						LUA_GET_VALUE( kLuamanager, "SrcBlend",		kRenderOption.m_iSrcBlend,	5 );
						LUA_GET_VALUE( kLuamanager, "DestBlend",	kRenderOption.m_iDestBlend,	6 );
						if( false == kLuamanager.GetUserDefinedTypeValue( "Color", kRenderOption.m_Color ) )
							kRenderOption.m_Color = D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
						kLuamanager.EndTable();
					}
					pHouseTemplet->m_RenderOption = kRenderOption;
				}

				LUA_GET_VALUE( kLuamanager, "EFFECT_SET_NAME", pHouseTemplet->m_wstrEffectSetName, L"" );
#ifdef SERV_IRUHADEV_ELYSION_DEBUG
				if( false == pHouseTemplet->m_wstrNPCMeshName2nd.empty() || false == pHouseTemplet->m_wstrEffectSetName.empty()
					|| false == pHouseTemplet->m_vecCustomNpcPos.empty() || false == pHouseTemplet->m_mapNpcTalkList.empty()
					|| false == pHouseTemplet->m_vecCustomMotionName.empty() || true == pHouseTemplet->m_bOnlyClickable
					|| true == pHouseTemplet->m_bAlwaysShow || true == pHouseTemplet->m_bFragmentExchange || true == pHouseTemplet->m_bTalkLoopAniNpc )
				{
					WCHAR buff[512] = {0};
					StringCchPrintf( buff, 512, L"[ELYDBG] HouseParsing June keys: house %d mesh2 '%s' effectset '%s' customPos %d talkLists %d customMotions %d onlyClickable %d alwaysShow %d fragment %d talkLoop %d",
						houseID, pHouseTemplet->m_wstrNPCMeshName2nd.c_str(), pHouseTemplet->m_wstrEffectSetName.c_str(),
						static_cast<int>( pHouseTemplet->m_vecCustomNpcPos.size() ), static_cast<int>( pHouseTemplet->m_mapNpcTalkList.size() ),
						static_cast<int>( pHouseTemplet->m_vecCustomMotionName.size() ), (int)pHouseTemplet->m_bOnlyClickable,
						(int)pHouseTemplet->m_bAlwaysShow, (int)pHouseTemplet->m_bFragmentExchange, (int)pHouseTemplet->m_bTalkLoopAniNpc );
					ErrorLogMsg( XEM_ERROR142, buff );
				}
#endif SERV_IRUHADEV_ELYSION_DEBUG
#endif SERV_IRUHADEV_ELYSION_VILLAGE
				m_mapHouseTemplet.insert( std::make_pair( (CX2LocationManager::HOUSE_ID)houseID, pHouseTemplet ) );

				kLuamanager.EndTable();
			}
			else
			{
				MessageBox( g_pKTDXApp->GetHWND(), L"CX2LocationManager House OpenScript Error", L"Error", MB_OK );
				return;
			}

			kLuamanager.EndTable();
		}


	}
}

#ifdef SERV_IRUHADEV_RANOX_SCENE_DEBUG
void CX2LocationManager::DebugProbeRanox()
{
	// Iruha : 2026-09-20 : Ranox port debug -- mirrors LocalParsing/VillageParsing
	// step by step, in a fresh isolated KLuaManager (bInNewEnv_=true, same as
	// OpenScript()'s own usage), so it never touches the live Village/Local
	// globals. Called on demand from the "ranoxprobe" chat command (X2ChatBox.cpp)
	// since OpenScript() itself runs before the offline log is open. See
	// LANOX_TROUBLESHOOTING.md.
	CX2OfflineLog::Server( L"[RANOXDBG] probe start" );

	KLuaManager kLuamanager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	bool bLoadedLocal = g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"DLG_Map_Local_Ranox.lua" );
	CX2OfflineLog::Server( L"[RANOXDBG] probe LoadAndDoMemory(DLG_Map_Local_Ranox.lua)=%d", (int)bLoadedLocal );

	int villageID = -1;
	if( kLuamanager.BeginTable( "VillageData" ) == true )
	{
		LUA_GET_VALUE( kLuamanager, "ID", villageID, -1 );
		kLuamanager.EndTable();
	}
	CX2OfflineLog::Server( L"[RANOXDBG] probe VillageData.ID=%d", villageID );

	bool bLoadedVillage = g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"DLG_Map_Village.lua" );
	CX2OfflineLog::Server( L"[RANOXDBG] probe LoadAndDoMemory(DLG_Map_Village.lua)=%d", (int)bLoadedVillage );

	bool bBeginVillage = kLuamanager.BeginTable( "Village" );
	CX2OfflineLog::Server( L"[RANOXDBG] probe BeginTable(Village)=%d", (int)bBeginVillage );

	if( bBeginVillage )
	{
		bool bBeginID = kLuamanager.BeginTable( villageID );
		CX2OfflineLog::Server( L"[RANOXDBG] probe BeginTable(villageID=%d)=%d", villageID, (int)bBeginID );

		if( bBeginID )
		{
			int worldId = -1;
			LUA_GET_VALUE_ENUM( kLuamanager, "WORLD_ID", worldId, CX2World::WORLD_ID, CX2World::WI_NONE );
			CX2OfflineLog::Server( L"[RANOXDBG] probe WORLD_ID=%d", worldId );
			kLuamanager.EndTable();
		}
		kLuamanager.EndTable();
	}

	CX2OfflineLog::Server( L"[RANOXDBG] probe end" );
}
#endif SERV_IRUHADEV_RANOX_SCENE_DEBUG

SEnum::VILLAGE_MAP_ID CX2LocationManager::GetDungeonLoungeIDByDungeonID(  const SEnum::DUNGEON_ID eDungeonId_ )
{
	map< LOCAL_MAP_ID,	LocalMapTemplet* >::const_iterator cmItr = m_mapLocalMapTemplet.begin();

	while ( m_mapLocalMapTemplet.end() != cmItr )
	{
		BOOST_FOREACH( const SEnum::DUNGEON_ID eVal, cmItr->second->m_DungeonList )
		{
			// 던전아이디가 같은게 있으면
			if ( eVal == eDungeonId_ )
			{
				// 라운지 아이디를 리턴
				return cmItr->second->m_DungeonLoungeID;
			}
		}
		++cmItr;
	}

	return SEnum::VMI_INVALID;
}

SEnum::VILLAGE_MAP_ID CX2LocationManager::GetVillageID( LOCAL_MAP_ID eLocalMapID )
{
	LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( eLocalMapID );
	if( NULL != pLocalMapTemplet )
	{
		return pLocalMapTemplet->m_VillageID;
	}
	else
	{
		return SEnum::VMI_INVALID;
	}
}


SEnum::VILLAGE_MAP_ID CX2LocationManager::GetDungeonGateID( LOCAL_MAP_ID eLocalMapID )
{
	LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( eLocalMapID );
	if( NULL != pLocalMapTemplet )
	{
		return pLocalMapTemplet->m_DungeonGateID;
	}
	else
	{
		return SEnum::VMI_INVALID;
	}
}

SEnum::VILLAGE_MAP_ID CX2LocationManager::GetDungeonLoungeID( LOCAL_MAP_ID eLocalMapID )
{
	LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( eLocalMapID );
	if( NULL != pLocalMapTemplet )
	{
		return pLocalMapTemplet->m_DungeonLoungeID;
	}
	else
	{
		return SEnum::VMI_INVALID;
	}
}

CX2LocationManager::LOCAL_MAP_ID CX2LocationManager::GetLocalMapID( SEnum::DUNGEON_ID eDungeonID, WORLD_MAP_ID eWorldID /*= WMI_INVALID*/ )
{
	WorldMapTemplet* pWorldMapTemplet = GetWorldMapTemplet( eWorldID );
	if( NULL == pWorldMapTemplet )
	{
		std::map<WORLD_MAP_ID, WorldMapTemplet*>::iterator it;
		for( it=m_mapWorldMapTemplet.begin(); it!=m_mapWorldMapTemplet.end(); it++ )
		{
			pWorldMapTemplet = it->second;

			for( UINT i=0; i< pWorldMapTemplet->m_LocalMapIDList.size(); i++ )
			{
				LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( pWorldMapTemplet->m_LocalMapIDList[i] );

				for( UINT j=0; j< pLocalMapTemplet->m_DungeonList.size(); j++ )
				{
					SEnum::DUNGEON_ID eTempDungeonID = pLocalMapTemplet->m_DungeonList[j];
					if( eTempDungeonID == eDungeonID )
					{
						return pLocalMapTemplet->m_LocalMapID;
					}
				}
			}
		}

		return LMI_INVALID;
	}
	else
	{
		for( UINT i=0; i< pWorldMapTemplet->m_LocalMapIDList.size(); i++ )
		{
			LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( pWorldMapTemplet->m_LocalMapIDList[i] );

			for( UINT j=0; j< pLocalMapTemplet->m_DungeonList.size(); j++ )
			{
				if( pLocalMapTemplet->m_DungeonList[j] == eDungeonID )
				{
					return pLocalMapTemplet->m_LocalMapID;
				}
			}
		}

		return LMI_INVALID;
	}
}

CX2LocationManager::LOCAL_MAP_ID CX2LocationManager::GetLocalMapID( SEnum::VILLAGE_MAP_ID eVillageMapID, WORLD_MAP_ID eWorldID /*= WMI_INVALID*/ )
{
	WorldMapTemplet* pWorldMapTemplet = GetWorldMapTemplet( eWorldID );
	if( NULL == pWorldMapTemplet )
	{
		std::map<WORLD_MAP_ID, WorldMapTemplet*>::iterator it;
		for( it=m_mapWorldMapTemplet.begin(); it!=m_mapWorldMapTemplet.end(); it++ )
		{
			pWorldMapTemplet = it->second;
			for( UINT i=0; i< pWorldMapTemplet->m_LocalMapIDList.size(); i++ )
			{
				LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( pWorldMapTemplet->m_LocalMapIDList[i] );

				if( pLocalMapTemplet->m_VillageID == eVillageMapID ||
					pLocalMapTemplet->m_DungeonGateID == eVillageMapID ||
					pLocalMapTemplet->m_DungeonLoungeID == eVillageMapID 
					|| pLocalMapTemplet->m_eBattleFieldRestID == eVillageMapID
					|| pLocalMapTemplet->m_vecFieldList.end() != std::find( pLocalMapTemplet->m_vecFieldList.begin(), pLocalMapTemplet->m_vecFieldList.end(), eVillageMapID )
					)
				{
					return pLocalMapTemplet->m_LocalMapID;
				}
			}
		}
		return LMI_INVALID;
	}
	else
	{
		for( UINT i=0; i< pWorldMapTemplet->m_LocalMapIDList.size(); i++ )
		{
			LocalMapTemplet* pLocalMapTemplet = GetLocalMapTemplet( pWorldMapTemplet->m_LocalMapIDList[i] );

			if( pLocalMapTemplet->m_VillageID == eVillageMapID ||
				pLocalMapTemplet->m_DungeonGateID == eVillageMapID ||
				pLocalMapTemplet->m_DungeonLoungeID == eVillageMapID )
			{
				return pLocalMapTemplet->m_LocalMapID;
			}			
		}
		return LMI_INVALID;
	}
}

const CX2LocationManager::VillageStartPos* CX2LocationManager::GetVillageStartPos(int startPos) const
{
	const CX2LocationManager::VillageStartPos* pVillageStartPos = NULL;

	std::map<int, CX2LocationManager::VillageStartPos*>::const_iterator mit = m_mapVillageMap.find( startPos );

	if( mit != m_mapVillageMap.end() )
	{
		pVillageStartPos = mit->second;
	}

	return pVillageStartPos;
}

SEnum::VILLAGE_MAP_ID CX2LocationManager::GetMapId(int startPos) const
{
	const CX2LocationManager::VillageStartPos* pVillageStartPos = GetVillageStartPos(startPos);
	
#ifdef QUEST_GUIDE
	//배틀필드ID 연결되어 있으면 필드와 연결된 startpos로 간주
	if(pVillageStartPos != NULL)
	{
		if( 0 != pVillageStartPos->m_uiBattleFieldId )
		{
			return static_cast<SEnum::VILLAGE_MAP_ID>(pVillageStartPos->m_uiBattleFieldId);
		}
		else
		{
			return pVillageStartPos->m_VillageID;
		}
	}
#else
	return pVillageStartPos->m_VillageID;
#endif //QUEST_GUIDE

	return SEnum::VMI_INVALID;
}
D3DXVECTOR3 CX2LocationManager::GetStartPosLoc(int startPos) const
{
	const CX2LocationManager::VillageStartPos* pVillageStartPos = GetVillageStartPos(startPos);

	if(pVillageStartPos != NULL)
		return pVillageStartPos->m_StartPos;

	return D3DXVECTOR3(0.f, 0.f, 0.f);
}

void CX2LocationManager::GetVecLinkedPos(std::vector<int>& vStart)
{
	CX2LocationManager::VillageStartPos* pVillageStartPos = NULL;
	
	std::map<int, CX2LocationManager::VillageStartPos*>::iterator mit;

	for(mit = m_mapVillageMap.begin(); mit != m_mapVillageMap.end(); ++mit)
	{
		pVillageStartPos = mit->second;
#ifdef SERV_IRUHADEV_ELYSION_VILLAGE
		//{{ Iruha : 2026-10-17 // Atlas/Elysion E1d -- June GetVecLinkedPos 324f2ff0: also no guild base entrance (+0x1a), spa entrance
		//   (+0x22) or spa exit (+0x23) row (June lists those apart: 324f2c20 / 324f2ca0 / 324f2d30, read by June's guild base and spa only),
		//   and with code event 2319 on, no portal to 40037 (a June boss-raid field, no March enum).
		if( pVillageStartPos->m_VillageID == m_eCurrentVillage &&
			false == pVillageStartPos->m_bIsMarket &&
			false == pVillageStartPos->m_bIsGuildAgitEnter &&
			false == pVillageStartPos->m_bIsSpaEnter &&
			false == pVillageStartPos->m_bIsSpaOut &&
			false == pVillageStartPos->m_bIsSummon &&
			false == pVillageStartPos->m_bIsWarp &&
			( NULL == g_pInstanceData || true != g_pInstanceData->IsEnableCode( 2319 ) || 40037 != pVillageStartPos->m_uiBattleFieldId ) )
			vStart.push_back(mit->first);
		//}}
#else SERV_IRUHADEV_ELYSION_VILLAGE
		if( pVillageStartPos->m_VillageID == m_eCurrentVillage && 
			/*pVillageStartPos->m_vecLinkStartPos.size() > 0 &&			*/
			pVillageStartPos->m_bIsMarket == false &&
			pVillageStartPos->m_bIsSummon == false &&
			pVillageStartPos->m_bIsWarp	== false )
			vStart.push_back(mit->first);
#endif SERV_IRUHADEV_ELYSION_VILLAGE
	}	
}

void CX2LocationManager::GetVecMarketPos(std::vector<int>& vStart)
{
	CX2LocationManager::VillageStartPos* pVillageStartPos = NULL;

	std::map<int, CX2LocationManager::VillageStartPos*>::iterator mit;

	for(mit = m_mapVillageMap.begin(); mit != m_mapVillageMap.end(); ++mit)
	{
		pVillageStartPos = mit->second;
		if(pVillageStartPos->m_VillageID == m_eCurrentVillage && pVillageStartPos->m_bIsMarket == true)
			vStart.push_back(mit->first);
	}	
}

// int	CX2LocationManager::GetStartPosLink(int startPos) const
// {
// 	const CX2LocationManager::VillageStartPos* pVillageStartPos  = GetVillageStartPos(startPos);
// 
// 	if(pVillageStartPos != NULL)
// 	{		
// 		if(pVillageStartPos->m_vecLinkStartPos.size() == 0)
// 			return 0;
// 
// 		if(pVillageStartPos->m_vecLinkStartPos.size() == 1)
// 		{
// 			return 	pVillageStartPos->m_vecLinkStartPos[0];
// 		}
// 
// 		int randInx = rand() % pVillageStartPos->m_vecLinkStartPos.size();
// 			return pVillageStartPos->m_vecLinkStartPos[randInx];
// 	}
// 
// 	return 0;
// }

bool CX2LocationManager::GetIsRight( int iStartPosID_)
{
	bool bIsRight = true;
	std::map<int, CX2LocationManager::VillageStartPos*>::iterator mit;
	for(mit = m_mapVillageMap.begin(); mit != m_mapVillageMap.end(); ++mit)
	{
		if (mit->first == iStartPosID_)
		{
			bIsRight = mit->second->m_bIsRight;
			break;
		}
	}

	return bIsRight;
}

D3DXVECTOR3 CX2LocationManager::GetWarpPos(int iVillageId )
{
	CX2LocationManager::VillageStartPos* pVillageStartPos = NULL;

	std::vector<int> vecLoginPos;
	std::map<int, CX2LocationManager::VillageStartPos*>::iterator mit;

	for(mit = m_mapVillageMap.begin(); mit != m_mapVillageMap.end(); ++mit)
	{
		pVillageStartPos = mit->second;
		if(pVillageStartPos->m_VillageID == iVillageId && pVillageStartPos->m_bIsWarp == true)
		{
			return pVillageStartPos->m_StartPos;
		}
	}

	return D3DXVECTOR3(0.f, 0.f, 0.f);
}

UINT CX2LocationManager::GetBattleFieldId( const int iStartPosNumber_ ) const
{
	 const CX2LocationManager::VillageStartPos* pVillageStartPos = GetVillageStartPos( iStartPosNumber_ );

	 if(pVillageStartPos != NULL)
		 return pVillageStartPos->m_uiBattleFieldId;
	else	// 찾지 못했으면
		return 0;
}

int CX2LocationManager::GetLoginPos(int mapId)
{
	CX2LocationManager::VillageStartPos* pVillageStartPos = NULL;

	std::vector<int> vecLoginPos;
	std::map<int, CX2LocationManager::VillageStartPos*>::iterator mit;

	for(mit = m_mapVillageMap.begin(); mit != m_mapVillageMap.end(); ++mit)
	{
		pVillageStartPos = mit->second;
		if(pVillageStartPos->m_VillageID == mapId && /*pVillageStartPos->m_vecLinkStartPos.size() <= 0 &&*/ pVillageStartPos->m_bIsSummon == true)
			vecLoginPos.push_back(mit->first);
	}

	if(vecLoginPos.size() == 0)
		return 0;
    
	int randInx = rand() % vecLoginPos.size();
	return vecLoginPos[randInx];
}

D3DXVECTOR3 CX2LocationManager::GetNearLoginPos(D3DXVECTOR3 vPos)
{
	CX2LocationManager::VillageStartPos* pVillageStartPos = NULL;

	std::vector<int> vecLoginPos;
	std::map<int, CX2LocationManager::VillageStartPos*>::iterator mit;
	D3DXVECTOR3 vRet;
	float fDist = 99999.f;

	for(mit = m_mapVillageMap.begin(); mit != m_mapVillageMap.end(); ++mit)
	{
		pVillageStartPos = mit->second;
		if(pVillageStartPos->m_VillageID == m_eCurrentVillage && /*pVillageStartPos->m_vecLinkStartPos.size() <= 0 &&*/ pVillageStartPos->m_bIsSummon == true)
		{
			D3DXVECTOR3 vStartPos = GetStartPosLoc(mit->first);

			if(fDist >= GetDistance(vPos, vStartPos))
			{
				fDist = GetDistance(vPos, vStartPos);
				vRet = vStartPos;			
			}
		}
	}

	return vRet;
}


void CX2LocationManager::SetVillage(SEnum::VILLAGE_MAP_ID val, const D3DXVECTOR3& pos, int iLineIndex /*=0*/ )
{
	m_eOldVillage = m_eCurrentVillage;
	m_vOldPos = m_vCurrentPos;

	m_eCurrentVillage = val;
	m_vCurrentPos = pos;
	m_iLineIndex = iLineIndex;
}
void CX2LocationManager::GetVillage(SEnum::VILLAGE_MAP_ID &VillageId, D3DXVECTOR3 &vPos, int* pLineIndex /*= NULL */ )
{
	VillageId = m_eCurrentVillage;
	vPos = m_vCurrentPos;	
	if( NULL != pLineIndex )
	{
		*pLineIndex = m_iLineIndex;
	}
}
void CX2LocationManager::RestoreVillage()
{
	m_eCurrentVillage = m_eOldVillage;
	m_vCurrentPos = m_vOldPos;
}
int CX2LocationManager::GetWorldID(SEnum::VILLAGE_MAP_ID val)
{
	int worldId = SEnum::VMI_INVALID;

	std::map<int, int>::iterator mit;
	mit = m_mapWorldMap.find( val );

	if( mit != m_mapWorldMap.end() )
	{
		worldId = mit->second;
	}

	return worldId;
} 

bool CX2LocationManager::IsValidVillage( int iVillageId )
{
	if(iVillageId >= SEnum::VMI_RUBEN && iVillageId <= SEnum::VMI_ALTERA)
		return true;

	return false;
}

bool CX2LocationManager::IsValidWarpField( int iVillageId )
{
	switch(iVillageId)
	{
	case SEnum::VMI_BATTLE_FIELD_ELDER_REST_00:
	case SEnum::VMI_BATTLE_FIELD_BESMA_REST_00:
	case SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00:
	case SEnum::VMI_BATTLE_FIELD_VELDER_REST_00:
	case SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00:
	case SEnum::VMI_BATTLE_FIELD_SANDER_REST_00:	/// 윈드스톤 유적지

	case SEnum::VMI_RUBEN:
	case SEnum::VMI_ELDER:
	case SEnum::VMI_BESMA:
	case SEnum::VMI_ALTERA:
	case SEnum::VMI_PEITA:
	case SEnum::VMI_VELDER:
#ifdef ADD_HAMEL_VILLAGE
	case SEnum::VMI_HAMEL:
#endif
	case SEnum::VMI_SANDER:
#ifdef SERV_IRUHADEV_RANOX_PORT
	case SEnum::VMI_RANOX:
	case SEnum::VMI_BATTLE_FIELD_RANOX_REST_00:
#endif SERV_IRUHADEV_RANOX_PORT
#ifdef SERV_IRUHADEV_ELYSION_PORT
	//{{ Iruha : 2026-10-17 // Atlas A5: June 2017 IsValidWarpField (unpacked_x2.exe 0x324f1e70) is a range test:
	//   1000-1008 except 1003, 1500-1501, 20000-20009, 25000. Every March case above is in it; added here are the
	//   ids June has and March lacked. Omitted (no March enum): 1500 VMI_SPA_RUBEN, 25000 VMI_GUILD_AGIT.
	case SEnum::VMI_BATTLE_FIELD_ELYSION_REST_00:
	case SEnum::VMI_ELYSION:
#ifdef SERV_IRUHADEV_RANOX_PORT
	case SEnum::VMI_SPA_RANOX:
#endif SERV_IRUHADEV_RANOX_PORT
	//}}
#endif SERV_IRUHADEV_ELYSION_PORT

		return true;
	default:
		return false;
	}

	return false;
}