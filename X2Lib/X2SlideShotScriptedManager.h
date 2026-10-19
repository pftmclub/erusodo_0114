#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY
#pragma once

// June quest system, Phase Q10 ( SERV_IRUHADEV_JUNE_QUEST_STORY ): June's CX2SlideShotScriptedManager ( lua_tinker ptr2user RTTI 3373ba28, Lua global
// g_pSlideShotManager ), the data of the story book. A singleton of 0x20 B: getter 326ab210 ( global 337a6c64, made on first use ), ctor 326ab190,
// destroy 326ab290 ( clear 326ab0d0 ). The quest loader 3258c470 destroys it, makes it and loads it ( 326a8d40 ) after the quest scene data
// ( @32590879..32590885 ). Map: june2017_re/quest_plan/q10_story_map.tsv.
//
//   +0x04  map< quest id, vector< CutScene > >   AddStory 326a9890     ( SlideShotScript.lua, 326a7d80 ): the story a completed story quest replays
//   +0x10  vector< MediaInfo >                     AddMediaInfo 326a8e10 ( MediaInfoScript.lua, 326a8560 ): the book's rows, in file order
//
// Readers: the quest-window Remind button ( 323ccbc0 case 6 ), the story-book rebuild ( CX2QuestManager 32335f50 ), the complete ACK ( 323e57e0 ),
// the slide-show player ( 328c3750 / 328c3ae0, Phase Q10b ) and the book UI ( CX2UIStoryBook, Phase Q10c ).

class CX2SlideShotScriptedManager
{
public:
	/// 0x20 B rows of Background_List / Scene_BGM_List / Scene_List: { time, name }. The name is a particle sequence ( BG_Name, Scene_Name ) or a sound file.
	struct TimedName
	{
		float			m_fTime;		// +0x00  Start_Time / BGM_Time, default 0
		std::wstring	m_wstrName;		// +0x04  BG_Name / BGM_Name / Scene_Name

		TimedName() : m_fTime( 0.0f ) {}
		bool operator<( const TimedName& rhs_ ) const { return m_fTime < rhs_.m_fTime; }	// 326ac130
	};
	/// 0x24 B rows of Sound_List ( read by June, used by no shipped story )
	struct TimedSound
	{
		float			m_fTime;		// +0x00  Start_Time
		std::wstring	m_wstrName;		// +0x04  Sound_Name
		bool			m_bStopAll;		// +0x20  Stop_All

		TimedSound() : m_fTime( 0.0f ), m_bStopAll( false ) {}
		bool operator<( const TimedSound& rhs_ ) const { return m_fTime < rhs_.m_fTime; }	// 326ac530
	};
	/// 0x20 B rows of Scene_Delete: { time, key }. The key names a Scene_List particle by its runtime key "<cut>_<row>" ( narrow string )
	struct TimedKey
	{
		float			m_fTime;		// +0x00  Time
		std::string		m_strKey;		// +0x04  Key

		TimedKey() : m_fTime( 0.0f ) {}
		bool operator<( const TimedKey& rhs_ ) const { return m_fTime < rhs_.m_fTime; }
	};
	/// June 0x88 B, ctor 326af720: one Cut_SceneN table
	struct CutScene
	{
		float						m_fSceneDuration;	// +0x00  Scene_Duration, default 7.0 ( 334729f0 )
		float						m_fTextStartTime;	// +0x04  Scene_Text.Start_Time
		float						m_fUnused08;		// +0x08  copied from a zero local, never read
		float						m_fSpreadRate;		// +0x0c  Scene_Text.Spread_Rate
		bool						m_bNameLeft;		// +0x10  Scene_Text.Name_Left
		int							m_iSpeakerStringID;	// +0x14  Scene_Text.Speaker, -1 = no name
		int							m_iTextStringID;	// +0x18  Scene_Text.Text, -1 = no text
		std::wstring				m_wstrColor;		// +0x1c  Scene_Text.Color
		std::vector< TimedName >	m_vecBackground;	// +0x38  Background_List
		std::vector< TimedSound >	m_vecSound;			// +0x48  Sound_List
		std::vector< TimedName >	m_vecBGM;			// +0x58  Scene_BGM_List
		std::vector< TimedKey >		m_vecSceneDelete;	// +0x68  Scene_Delete
		std::vector< TimedName >	m_vecScene;			// +0x78  Scene_List

		CutScene() : m_fSceneDuration( 0.0f ), m_fTextStartTime( 0.0f ), m_fUnused08( 0.0f ), m_fSpreadRate( 0.0f ), m_bNameLeft( false ),
			m_iSpeakerStringID( -1 ), m_iTextStringID( -1 ) {}
	};
	/// one MediaFile entry ( map node 0x50 B )
	struct MediaFile
	{
		std::wstring	m_wstrTga;			// node +0x10  Tga
		std::wstring	m_wstrTet;			// node +0x2c  Tet ( the texture piece )
		int				m_iTitleStringID;	// node +0x48  Title, default -1

		MediaFile() : m_iTitleStringID( -1 ) {}
	};
	/// June 0x24 B, ctor 326af8b0: one AddMediaInfo row ( a category header or an episode group )
	struct MediaInfo
	{
		bool						m_bIsCategory;		// +0x00  IsCategory
		int							m_iMediaType;		// +0x04  MediaType ( MEDIA_TYPE )
		int							m_iUnitType;		// +0x08  UnitType, 0 = every unit
		int							m_iMediaGroup;		// +0x0c  MediaGroup = the quests' m_iEpisodeGroupID
		int							m_iTitleStringID;	// +0x10  Title, default -1
		std::map< int, MediaFile >	m_mapMediaFile;		// +0x14  MediaFile by QuestID ( the first of a duplicate id wins )

		MediaInfo() : m_bIsCategory( false ), m_iMediaType( 0 ), m_iUnitType( 0 ), m_iMediaGroup( 0 ), m_iTitleStringID( -1 ) {}
	};

	typedef std::vector< CutScene >				CutSceneList;
	typedef std::map< int, CutSceneList >		StoryMap;

public:
	static CX2SlideShotScriptedManager*	GetInstance();		// 326ab210: makes it on first use
	static void							DestroyInstance();	// 326ab290

	bool						Load();						// 326a8d40: MediaInfoScript.lua ( 326a8560 ), then SlideShotScript.lua ( 326a7d80 ) only if the first loaded

	bool						AddStory_LUA();				// 326a9890
	bool						AddMediaInfo_LUA();			// 326a8e10

	const CutSceneList*			GetStory( int iQuestID_ ) const;		// 326ab090: NULL when the quest has no story
	bool						HasStory( int iQuestID_ ) const;		// 326a7d50
	const std::vector< MediaInfo >&	GetMediaInfoList() const { return m_vecMediaInfo; }		// 326a7cd0

	void						LogLoadCensus() const;

private:
	CX2SlideShotScriptedManager();			// 326ab190
	~CX2SlideShotScriptedManager();			// 326ab0d0

	bool						LoadScript( const WCHAR* wszFileName_ );

	static CX2SlideShotScriptedManager*	ms_pInstance;	// June 337a6c64

	StoryMap					m_mapStory;			// +0x04
	std::vector< MediaInfo >	m_vecMediaInfo;		// +0x10

	int							m_iAddStoryCalls;		// census only ( not June's )
	int							m_iAddMediaInfoCalls;	// census only ( not June's )
};

#endif SERV_IRUHADEV_JUNE_QUEST_STORY
