#include "StdAfx.h"

#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY
#include "X2SlideShotScriptedManager.h"
#ifdef SERV_IRUHADEV_OFFLINE
#include "Offline/X2OfflineLog.h"
#endif SERV_IRUHADEV_OFFLINE

// Log tag [JUNE_QUEST] STORY ( offline_server.log; a missing file also goes to ErrorLog.txt )

CX2SlideShotScriptedManager* CX2SlideShotScriptedManager::ms_pInstance = NULL;

/// June 326ab210: the getter makes the singleton on first use ( every reader calls it, so a reader before the load gets an empty one )
CX2SlideShotScriptedManager* CX2SlideShotScriptedManager::GetInstance()
{
	if( NULL == ms_pInstance )
		ms_pInstance = new CX2SlideShotScriptedManager();
	return ms_pInstance;
}

/// June 326ab290
void CX2SlideShotScriptedManager::DestroyInstance()
{
	SAFE_DELETE( ms_pInstance );
}

/// June 326ab190: an empty story map and an empty media list
CX2SlideShotScriptedManager::CX2SlideShotScriptedManager()
	: m_iAddStoryCalls( 0 )
	, m_iAddMediaInfoCalls( 0 )
{
}

/// June 326ab0d0
CX2SlideShotScriptedManager::~CX2SlideShotScriptedManager()
{
	m_vecMediaInfo.clear();
	m_mapStory.clear();
}

/// June 326a8d40: MediaInfoScript.lua ( 326a8560 ) and, only when it loaded, SlideShotScript.lua ( 326a7d80 ). Each loader publishes
/// g_pSlideShotManager = this first ( @326a874f ) and reads the file out of the kom ( KOM_GetMemberFile + XOR + DoMemory ); a miss logs
/// error 0x48 / 0x49 / 0x2712 ( 31bd0f50 ). The quest loader ignores the result.
bool CX2SlideShotScriptedManager::Load()
{
	if( false == LoadScript( L"MediaInfoScript.lua" ) )
		return false;
	return LoadScript( L"SlideShotScript.lua" );
}

bool CX2SlideShotScriptedManager::LoadScript( const WCHAR* wszFileName_ )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "g_pSlideShotManager", this );
	if( false == g_pKTDXApp->LoadLuaTinker( wszFileName_ ) )
	{
		WCHAR wszBuf[ 256 ] = { 0, };
		StringCchPrintf( wszBuf, 256, L"[JUNE_QUEST] STORY LOAD %s did not load: no story book data. Stage June's file into data036.kom ( quest-system Q10 ).", wszFileName_ );
		ErrorLogMsg( XEM_ERROR142, wszBuf );
#ifdef SERV_IRUHADEV_OFFLINE
		CX2OfflineLog::Server( L"%s", wszBuf );
#endif SERV_IRUHADEV_OFFLINE
		return false;
	}
	return true;
}

/// June 326a8e10 ( binding 31d95080 @31e446e1 ): one book row. Read order Title, IsCategory, MediaType, UnitType; a category row stops there
/// ( @326a920a ), an episode group goes on to MediaGroup and the MediaFile array ( i = 1.., up to the first entry that is not a table ).
/// Every row is appended ( 326b0660 ), in file order. A MediaFile whose QuestID is already in the row is dropped ( 326ae600: the first wins ).
bool CX2SlideShotScriptedManager::AddMediaInfo_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	++m_iAddMediaInfoCalls;

	MediaInfo kInfo;
	LUA_GET_VALUE( luaManager, "Title",			kInfo.m_iTitleStringID,	-1 );		// @326a8eaf
	LUA_GET_VALUE( luaManager, "IsCategory",	kInfo.m_bIsCategory,	false );	// @326a8f64
	LUA_GET_VALUE( luaManager, "MediaType",		kInfo.m_iMediaType,		0 );		// @326a9065
	LUA_GET_VALUE( luaManager, "UnitType",		kInfo.m_iUnitType,		0 );		// @326a9158

	if( false == kInfo.m_bIsCategory )
	{
		LUA_GET_VALUE( luaManager, "MediaGroup",	kInfo.m_iMediaGroup,	0 );	// @326a924a

		if( true == luaManager.BeginTable( "MediaFile" ) )							// @326a9352
		{
			int iIndex = 1;
			while( true == luaManager.BeginTable( iIndex ) )
			{
				int iQuestID = 0;
				MediaFile kFile;
				LUA_GET_VALUE( luaManager, "QuestID",	iQuestID,				0 );	// @326a9442
				LUA_GET_VALUE( luaManager, "Title",		kFile.m_iTitleStringID,	-1 );	// @326a9505
				LUA_GET_VALUE( luaManager, "Tga",		kFile.m_wstrTga,		L"" );	// @326a95ab
				LUA_GET_VALUE( luaManager, "Tet",		kFile.m_wstrTet,		L"" );	// @326a9628
				kInfo.m_mapMediaFile.insert( std::make_pair( iQuestID, kFile ) );
				luaManager.EndTable();
				++iIndex;
			}
			luaManager.EndTable();
		}
	}

	m_vecMediaInfo.push_back( kInfo );		// @326a9812
	return true;
}

/// June 326a9890 ( binding 31d95080 @31e44599 ): one story. Quest_ID ( default -1 ), then Cut_Scene_List.Cut_Scene0, Cut_Scene1, ... up to the
/// first missing index ( "%s%d", @326a9a0e ). A story with no Cut_Scene_List is still stored, with no cut scene. Each list is sorted by its time
/// ( 326a8d70 ). The story goes in by unique insert ( 326b0ea0 ): a second AddStory for the same quest is dropped ( SlideShotScript.lua has 43430
/// twice, identical ). June reads no BlackLayoutMove key ( every shipped cut scene carries one ).
bool CX2SlideShotScriptedManager::AddStory_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
#ifndef X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING
	TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );
#endif  X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING

	++m_iAddStoryCalls;

	int iQuestID = -1;
	LUA_GET_VALUE( luaManager, "Quest_ID", iQuestID, -1 );		// @326a99ae

	CutSceneList vecCutScene;
	if( true == luaManager.BeginTable( "Cut_Scene_List" ) )	// @326a99de
	{
		int iCut = 0;
		while( true == luaManager.BeginTable( "Cut_Scene", iCut ) )		// 31c7b920( "%s%d" ) + 32fe9940
		{
			CutScene kCut;		// 326af720
			LUA_GET_VALUE( luaManager, "Scene_Duration", kCut.m_fSceneDuration, 7.0f );		// @326a9aa2

			if( true == luaManager.BeginTable( "Scene_BGM_List" ) )		// @326a9bbf
			{
				int iIndex = 1;
				while( true == luaManager.BeginTable( iIndex ) )
				{
					TimedName kRow;
					LUA_GET_VALUE( luaManager, "BGM_Name", kRow.m_wstrName,	L"" );		// @326a9c50
					LUA_GET_VALUE( luaManager, "BGM_Time", kRow.m_fTime,	0.0f );		// @326a9d31
					kCut.m_vecBGM.push_back( kRow );
					luaManager.EndTable();
					++iIndex;
				}
				luaManager.EndTable();
			}
			if( true == luaManager.BeginTable( "Background_List" ) )		// @326a9e50
			{
				int iIndex = 1;
				while( true == luaManager.BeginTable( iIndex ) )
				{
					TimedName kRow;
					LUA_GET_VALUE( luaManager, "BG_Name",		kRow.m_wstrName,	L"" );		// @326a9eef
					LUA_GET_VALUE( luaManager, "Start_Time",	kRow.m_fTime,		0.0f );		// @326a9fcb
					kCut.m_vecBackground.push_back( kRow );
					luaManager.EndTable();
					++iIndex;
				}
				luaManager.EndTable();
			}
			if( true == luaManager.BeginTable( "Sound_List" ) )		// @326aa0f1
			{
				int iIndex = 1;
				while( true == luaManager.BeginTable( iIndex ) )
				{
					TimedSound kRow;
					LUA_GET_VALUE( luaManager, "Sound_Name",	kRow.m_wstrName,	L"" );		// @326aa19a
					LUA_GET_VALUE( luaManager, "Start_Time",	kRow.m_fTime,		0.0f );		// @326aa2a6
					LUA_GET_VALUE( luaManager, "Stop_All",		kRow.m_bStopAll,	false );	// @326aa3a2
					kCut.m_vecSound.push_back( kRow );
					luaManager.EndTable();
					++iIndex;
				}
				luaManager.EndTable();
			}
			if( true == luaManager.BeginTable( "Scene_List" ) )		// @326aa782
			{
				int iIndex = 1;
				while( true == luaManager.BeginTable( iIndex ) )
				{
					TimedName kRow;
					LUA_GET_VALUE( luaManager, "Scene_Name",	kRow.m_wstrName,	L"" );		// @326aa54d
					LUA_GET_VALUE( luaManager, "Start_Time",	kRow.m_fTime,		0.0f );		// @326aa65c
					kCut.m_vecScene.push_back( kRow );
					luaManager.EndTable();
					++iIndex;
				}
				luaManager.EndTable();
			}
			if( true == luaManager.BeginTable( "Scene_Text" ) )		// one table, not an array
			{
				LUA_GET_VALUE( luaManager, "Name_Left",		kCut.m_bNameLeft,			false );	// @326aa821
				LUA_GET_VALUE( luaManager, "Speaker",		kCut.m_iSpeakerStringID,	-1 );		// @326aa90c
				LUA_GET_VALUE( luaManager, "Text",			kCut.m_iTextStringID,		-1 );		// @326aa9bd
				LUA_GET_VALUE( luaManager, "Spread_Rate",	kCut.m_fSpreadRate,			0.0f );		// @326aaa62
				LUA_GET_VALUE( luaManager, "Color",			kCut.m_wstrColor,			L"" );		// @326aab66
				LUA_GET_VALUE( luaManager, "Start_Time",	kCut.m_fTextStartTime,		0.0f );		// @326aac22
				luaManager.EndTable();
			}
			if( true == luaManager.BeginTable( "Scene_Delete" ) )
			{
				int iIndex = 1;
				while( true == luaManager.BeginTable( iIndex ) )
				{
					TimedKey kRow;
					// @326aadff: June reads a narrow string ( 32feb110 ). The client's KLuaManager has no std::string reader, so the key is read
					// wide and narrowed ( the shipped keys are ASCII: "3_0", "4_0", "5_0" )
					std::wstring wstrKey;
					LUA_GET_VALUE( luaManager, "Key",	wstrKey,		L"" );
					kRow.m_strKey.assign( wstrKey.begin(), wstrKey.end() );
					LUA_GET_VALUE( luaManager, "Time",	kRow.m_fTime,	0.0f );		// @326aae7c
					kCut.m_vecSceneDelete.push_back( kRow );
					luaManager.EndTable();
					++iIndex;
				}
				luaManager.EndTable();
			}

			// 326a8d70: every list ascending by its time ( Scene_Delete 326af240, Background / BGM / Scene 326af330, Sound 326af420 )
			std::sort( kCut.m_vecSceneDelete.begin(),	kCut.m_vecSceneDelete.end() );
			std::sort( kCut.m_vecBackground.begin(),	kCut.m_vecBackground.end() );
			std::sort( kCut.m_vecBGM.begin(),			kCut.m_vecBGM.end() );
			std::sort( kCut.m_vecScene.begin(),			kCut.m_vecScene.end() );
			std::sort( kCut.m_vecSound.begin(),			kCut.m_vecSound.end() );

			vecCutScene.push_back( kCut );		// 326b0320
			luaManager.EndTable();
			++iCut;
		}
		luaManager.EndTable();
	}

	m_mapStory.insert( std::make_pair( iQuestID, vecCutScene ) );		// 326b0ea0: unique insert
	return true;
}

/// June 326ab090: the story's cut scenes, or NULL when the quest has none
const CX2SlideShotScriptedManager::CutSceneList* CX2SlideShotScriptedManager::GetStory( int iQuestID_ ) const
{
	StoryMap::const_iterator mit = m_mapStory.find( iQuestID_ );
	if( m_mapStory.end() == mit )
		return NULL;
	return &mit->second;
}

/// June 326a7d50
bool CX2SlideShotScriptedManager::HasStory( int iQuestID_ ) const
{
	return m_mapStory.end() != m_mapStory.find( iQuestID_ );
}

/// one line after the load ( not June's ): offline_server.log [JUNE_QUEST] STORY LOAD
void CX2SlideShotScriptedManager::LogLoadCensus() const
{
#ifdef SERV_IRUHADEV_OFFLINE
	int iCategory = 0;
	int iGroup = 0;
	int iFile = 0;
	for( UINT i = 0; i < m_vecMediaInfo.size(); ++i )
	{
		if( true == m_vecMediaInfo[i].m_bIsCategory )
			++iCategory;
		else
			++iGroup;
		iFile += static_cast<int>( m_vecMediaInfo[i].m_mapMediaFile.size() );
	}
	int iCut = 0;
	int iFileWithStory = 0;
	for( StoryMap::const_iterator mit = m_mapStory.begin(); mit != m_mapStory.end(); ++mit )
		iCut += static_cast<int>( mit->second.size() );
	for( UINT i = 0; i < m_vecMediaInfo.size(); ++i )
	{
		for( std::map< int, MediaFile >::const_iterator fit = m_vecMediaInfo[i].m_mapMediaFile.begin(); fit != m_vecMediaInfo[i].m_mapMediaFile.end(); ++fit )
		{
			if( true == HasStory( fit->first ) )
				++iFileWithStory;
		}
	}
	CX2OfflineLog::Server( L"[JUNE_QUEST] STORY LOAD media rows %d ( AddMediaInfo calls %d: %d categories, %d groups, %d files, %d with a story ) stories %d ( AddStory calls %d ) cut scenes %d",
		static_cast<int>( m_vecMediaInfo.size() ), m_iAddMediaInfoCalls, iCategory, iGroup, iFile, iFileWithStory,
		static_cast<int>( m_mapStory.size() ), m_iAddStoryCalls, iCut );
#endif SERV_IRUHADEV_OFFLINE
}

#endif SERV_IRUHADEV_JUNE_QUEST_STORY
