#pragma once




class CX2LocalMapUI : public CKTDXDeviceHolder
{
public: 
	enum LOCAL_MAP_UI_CUSTOM_MSG
	{
		LMUCM_DUNGEON_CLICKED = 18000,
		LMUCM_DUNGEON_GATE_SELECTED,
		LMUCM_CLOSE_WINDOW,
		LMUCM_DUNGEON_MOUSE_OVER,
		LMUCM_DUNGEON_MOUSE_OUT,
#ifdef SERV_IRUHADEV_ELYSION_PORT
	//{{ Iruha : 2026-10-17 // Atlas/Elysion port Phase A3 -- LMUCM 18006-18008, June values (the Elysion page scroll + guide window); explicit, never positional
		LMUCM_LOCAL_MAP_MOVE_UP = 18006,
		LMUCM_LOCAL_MAP_MOVE_DOWN = 18007,
		LMUCM_CLOSE_GUIDE_WINDOW = 18008,
	//}}
#endif SERV_IRUHADEV_ELYSION_PORT
#ifdef SERV_IRUHADEV_JUNE_LOCAL_MAP_UI
		// June2017 UI parity (JUNE_UI_PARITY_PHASES.md Phase 1). Values are June's own
		// (Phase 0 findings). 18006-18008 (LMUCM_LOCAL_MAP_MOVE_UP/DOWN,
		// LMUCM_CLOSE_GUIDE_WINDOW) are deliberately omitted: Elysion-only, no March user.
		LMUCM_DUNGEON_MODE_SELECTED = 18005,
		LMUCM_LOCAL_MAP_ARROW_LEFT = 18009,
		LMUCM_LOCAL_MAP_ARROW_RIGHT = 18010,
#endif SERV_IRUHADEV_JUNE_LOCAL_MAP_UI
	};

public:
	CX2LocalMapUI(void);
	virtual ~CX2LocalMapUI(void);

	virtual void OnFrameMove( double fTime, float fElapsedTime );
	virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool GetShowLocalMap() { return m_bShowLocalMap; }
	CX2LocationManager::LOCAL_MAP_ID GetPickedLocalMapID() const { return m_ePickedLocalMapID; }
	void ResetLocalMapID(){m_ePickedLocalMapID = CX2LocationManager::LMI_INVALID;}

private:
	CKTDXStage*			m_pStage;

	CX2LocationManager::LOCAL_MAP_ID							m_ePickedLocalMapID;
	map< CX2LocationManager::LOCAL_MAP_ID, CKTDGUIDialogType >		m_mapLocalDLG;

	bool m_bShowLocalMap;

#ifdef SERV_EPIC_QUEST
	CKTDGUIDialogType		m_pDLGQuestDesc;
	map< int, wstring >		m_mapLocalMapQuestDesc;
#endif SERV_EPIC_QUEST

	CKTDGUIDialogType		m_pDlgHeroRecruit;
#ifdef SERV_IRUHADEV_JUNE_LOCAL_MAP_UI
	// June2017 CX2LocalMapUI members (JUNE_UI_PARITY_PHASES.md Phase 2).
	bool	m_bHeroMode;		// June +0x44, set by LMUCM_DUNGEON_MODE_SELECTED (Mode Button, Phase 4)
	bool	m_bFirstPage;	// June +0x60, tab-strip page; only the ctor and the two arrow messages write it
	bool	m_bLoggedNoArrows;	// once-per-session log when the dialog lua has no page arrows
#endif SERV_IRUHADEV_JUNE_LOCAL_MAP_UI
public:
	void SetStage( CKTDXStage* pStage );

	void OpenLocalMapDLG( bool bOpen, CX2LocationManager::LOCAL_MAP_ID eLocalMapID = CX2LocationManager::LMI_INVALID );
	void UpdateLocalMapDLG();
	void ClearLocalMapDLGs();
	CKTDGUIDialogType GetLocalMapDLG( CX2LocationManager::LOCAL_MAP_ID eLocalMapID );
	CKTDGUIDialogType CreateLocalMapDLG( CX2LocationManager::LOCAL_MAP_ID eLocalMapID );

	void UpdateHeroRecruitButton();

	void UpdateDungeonButtons( CX2LocationManager::LOCAL_MAP_ID eLocalMapID );
#ifdef SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS
	// June2017 (FUN_32939aa0, JUNE_UI_PARITY_PHASES.md Phase 3): a secret dungeon's button
	// and its SecretDungeonDesc lock button, by whether the dungeon is open.
	void SecretDungeonDesc( const CX2Dungeon::DungeonData* pDungeonData_, CKTDGUIControl* pControl_ );
#endif SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS

	void CreateDungeonClearStars( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID );
	void UpdateDungeonClearStars( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID, bool bShow );

	void DropDownDungeonButton( CKTDGUIControl* pControl );
	void CreateNewDungeonParticle( CKTDGUIControl* pControl, bool bEventDungeon = false );	
#ifdef SERV_EPIC_QUEST
	//{{ oasis907 : 김상윤 [2010.7.1] // 에픽 퀘스트
	void CreateLocalQuestPicture( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID );
	void DrawLocalMapQuestDesc(D3DXVECTOR2 DrawPos, int iDungeonID);
	void UdateLocalMapQuestDesc( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID, bool bShow );
	//}}
#endif SERV_EPIC_QUEST

	//{{ kimhc // 2011-04-27 // 디펜스 던전 시간에 따라, 던전 아이콘 비/활성화 여부 결정
#ifdef SERV_INSERT_GLOBAL_SERVER
	bool UpdateDefenceDungeonIcon( CKTDGUIControl* pControlDefenceDungeon_, const bool bActiveDefenceDungeon_ );
#endif SERV_INSERT_GLOBAL_SERVER
	//}} kimhc // 2011-04-27 // 디펜스 던전 시간에 따라, 던전 아이콘 비/활성화 여부 결정
#ifdef NEW_HENIR_TEST
	void GetHenirDungeonIcnoDesc( OUT wstringstream& wstrStreamDungeonIconDesc_ );
#endif NEW_HENIR_TEST

#ifdef SERV_EVENT_TEAR_OF_ELWOMAN
	// 김석근 [12.08.27] 엘의 여인의 눈물
	void CreateLocalTearOfELWoman( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID );
	void UpdateLocalTearOfELWoman( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID, bool bShow );
#endif SERV_EVENT_TEAR_OF_ELWOMAN
#ifdef SERV_EVENT_VALENTINE_DUNGEON_INT
	void GetEventValentineDungeonIconDesc( OUT wstringstream& wstrStreamDungeonIconDesc );
#endif SERV_EVENT_VALENTINE_DUNGEON_INT

#ifdef DUNGEON_SELECT_ALL_CLOSE //던전선택화면이 활성화 되면 다른 UI들을 닫는다.
	void CloseOtherUI();
#endif //DUNGEON_SELECT_ALL_CLOSE
};
