#ifdef SERV_IRUHADEV_JUNE_SKILL_TREE_UI
#pragma once

//////////////////////////////////////////////////////////////////////////
// June 2017's skill tree UI, CX2UISkillTreeNew2015 ( RTTI 3373ca34, vt 334c37fc, class block 328d98e0-328ee680 ), under March's
// class name. Included by X2UISkillTreeNew.h when SERV_IRUHADEV_JUNE_SKILL_TREE_UI is defined ( JUNE_SKILL_SYSTEM_PHASES.md S7b ).
// Member comments give June's offset in CX2UISkillTreeNew2015 ( 0x154 bytes ).
//////////////////////////////////////////////////////////////////////////

class CX2EquippedSkillSlotData;
class CX2SkillTreeSlotData;

class CX2UISkillTreeNew
{
public:

	/// June Enum.lua SKILL_TREE_UI_CUSTOM_MSG: March's 29000-29058, then June's 29059-29079 ( UICustomEventProc 328eab00
	/// switches on wParam - 29000, 0..79 ). Names below 29059 are March's, kept so the numbering stays visible.
	enum SKILL_TREE_UI_CUSTOM_MSG
	{
		STUCM_EXIT	= 29000,

		STUCM_BUTTON_SKILL_LEVEL_PLUS,
		STUCM_BUTTON_SKILL_LEVEL_PLUS_MOUSEOVER,
		STUCM_BUTTON_SKILL_LEVEL_PLUS_MOUSEOUT,
		STUCM_BUTTON_SKILL_LEVEL_MINUS,
		STUCM_BUTTON_SKILL_LEVEL_MINUS_MOUSEOVER,
		STUCM_BUTTON_SKILL_LEVEL_MINUS_MOUSEOUT,
		STUCM_BUTTON_SKILL_MASTER_MOUSEOVER,
		STUCM_BUTTON_SKILL_MASTER_MOUSEOUT,
		STUCM_EQUIPSLOT_DROPPED,
		STUCM_EQUIPSLOT_RMOUSEUP,				/// 29010
		STUCM_EQUIPSLOT_MOUSEIN,
		STUCM_EQUIPSLOT_MOUSEOUT,
		STUCM_SKILLTREESLOT_RMOUSEUP,
		STUCM_SKILLTREESLOT_MOUSEIN,
		STUCM_SKILLTREESLOT_MOUSEOUT,
		STUCM_SKILL_RESET_MODE_CHANGE,
		STUCM_SKILL_RESET_MODE_CHANGE_MOUSEOVER,
		STUCM_SKILL_RESET_MODE_CHANGE_MOUSEOUT,
		STUCM_CONTROLLIST_SCROLL,
		STUCM_NAVI_LEFT_CLICKED,				/// 29020
		STUCM_NAVI_DRAGGING,
		STUCM_INGAME_ICON_MOUSE_OVER,
		STUCM_INGAME_ICON_MOUSE_OUT,
		STUCM_FIRST_GUIDE_CLOSE,
		STUCM_FIRST_GUIDE_DONOTSHOW,
		STUCM_CSP_GUIDE_MOUSEIN,
		STUCM_CSP_GUIDE_MOUSEOUT,
		STUCM_SKILLTREESLOTB_GUIDE_MOUSEIN,
		STUCM_SKILLTREESLOTB_GUIDE_MOUSEOUT,
		STUCM_UNSEAL_SKILL_CANCLE,				/// 29030
		STUCM_UNSEAL_SKILL_BUY,
		STUCM_OPEN_COMBO_TREE,
		STUCM_INGAME_ELSWORD_WSS_MOUSE_OVER,
		STUCM_INGAME_ELSWORD_WSS_MOUSE_OUT,
		STUCM_SKILLTREESLOTB_BUY_GUIDE_MOUSEIN,
		STUCM_SKILLTREESLOTB_BUY_GUIDE_MOUSEOUT,
		STUCM_BUTTON_SKILL_LEVEL_MINUS_GUIDE_MOUSEOVER,
		STUCM_BUTTON_SKILL_LEVEL_MINUS_GUIDE_MOUSEOUT,
		STUCM_BUTTON_SKILL_LEVEL_PLUS_RESULT,
		STUCM_BUTTON_SKILL_LEVEL_PLUS_CANCEL,	/// 29040
		STUCM_CHANGE_SKILL_LEVEL_PLUS,
		STUCM_BUTTON_SKILL_LEVEL_UPGRADE,
		STUCM_BUTTON_EXIT,
		STUCM_EXIT_WITH_SKILL_LEVEL_PLUS,
		STUCM_BUTTON_ENABLE_INIT_SKILL,
		STUCM_BUTTON_INIT_SKILL,
		STUCM_INIT_SKILL,
		STUCM_INIT_SKILL_ONE_POINT,
		STUCM_SKILL_PAGE1,
		STUCM_SKILL_PAGE2,						/// 29050
		STUCM_SKILL_PAGE3,
		STUCM_ADD_SKILL_PAGE_ED,
		STUCM_ADD_SKILL_PAGE_CASH,
		STUCM_DECIDE_SKILL_PAGE,
		STUCM_BUY_CONFIRM,
		STUCM_BUY_CANCEL,
		STUCM_DECIDE_WITH_SKILL_LEVEL_PLUS,
		STUCM_CANCEL_DECIDE_WITH_SKILL_LEVEL_PLUS,
		STUCM_MEMORIZE_SKILL_SLOT_MOUSEIN,		/// 29059 June
		STUCM_SKILL_EVENT_PAGE,					/// 29060
		STUCM_MAJOR_DESC_SKILL_MOUSE_OVER,
		STUCM_MAJOR_DESC_SKILL_MOUSE_OUT,
		STUCM_SELECT_SKILL_TAB,
		STUCM_SELECT_COMMAND_TAB,
		STUCM_SELECT_SKILL_LEFT_MEMO,
		STUCM_SELECT_SKILL_RIGHT_MEMO,
		STUCM_SELECT_SKILL_ACTIVE_MEMO,
		STUCM_SELECT_SKILL_DEACTIVE_MEMO,
		STUCM_MAJOR_DESC_COMMAND_MOUSE_OVER,
		STUCM_MAJOR_DESC_COMMAND_MOUSE_OUT,		/// 29070
		STUCM_TOOLTIP_COMMAND_MOUSE_OVER,
		STUCM_TOOLTIP_COMMAND_MOUSE_OUT,
		STUCM_TOOLTIP_COMMAND_MOVIE_MOUSE_OVER,
		STUCM_TOOLTIP_COMMAND_MOVIE_MOUSE_OUT,
		STUCM_JUNE_UNNAMED_29075,				/// absent from June's Enum.lua; UICustomEventProc's default
		STUCM_BUTTON_MEMO_SELECT_MOUSE_OVER,
		STUCM_BUTTON_MEMO_SELECT_MOUSE_OUT,
		STUCM_TRANSFORM_SKILL_SLOT_MOUSEIN,
		STUCM_TRANSFORM_SKILL_SLOT_MOUSEOUT,	/// 29079
	};

	/// June 328de040 builds 11 dwords per tree skill ( map value at node +0x10, copied by 328edb50 )
	struct SkillSlotUI
	{
		CKTDGUISlot*	m_pSlot;					/// +0x00 g_pSlot_SkillTree_%d
		CKTDGUIStatic*	m_pStaticSkillTypeBG;		/// +0x04 g_pStatic_SkillTypeBackGround_%d ( June-only )
		CKTDGUIStatic*	m_pStaticSkillBG;			/// +0x08 g_pStatic_SkillBackGround_%d
		CKTDGUIStatic*	m_pStaticSkillLock;			/// +0x0c g_pStaticSkill_Lock_%d
		bool			m_bSelectSkillBlind;		/// +0x10 read by the slot's UpdateUI ( 328dcd50 @+0x20 of the node ); June never sets it
		int				m_iReserved14;				/// +0x14 zeroed by 328de040, not read in S7b's functions ( S7c: memo popup )
		CKTDGUIButton*	m_pButtonLeftMemoDesc;		/// +0x18 g_pButton_LeftMemoDesc_%d ( S7c )
		CKTDGUIStatic*	m_pStaticLeftMemoState;		/// +0x1c g_pStatic_LeftMemoState_%d ( S7c )
		int				m_iReserved20;				/// +0x20 zeroed by 328de040, not read in S7b's functions ( S7c )
		CKTDGUIButton*	m_pButtonRightMemoDesc;		/// +0x24 g_pButton_RightMemoDesc_%d ( S7c )
		CKTDGUIStatic*	m_pStaticRightMemoState;	/// +0x28 g_pStatic_RightMemoState_%d ( S7c )

		SkillSlotUI()
		: m_pSlot( NULL ), m_pStaticSkillTypeBG( NULL ), m_pStaticSkillBG( NULL ), m_pStaticSkillLock( NULL ), m_bSelectSkillBlind( false )
		, m_iReserved14( 0 ), m_pButtonLeftMemoDesc( NULL ), m_pStaticLeftMemoState( NULL ), m_iReserved20( 0 )
		, m_pButtonRightMemoDesc( NULL ), m_pStaticRightMemoState( NULL )
		{}
	};

	/// June's tree position key ( map at +0xe4: tier, then index in tier )
	struct SkillPositionKey
	{
		int m_iRow;
		int m_iCol;

		SkillPositionKey() : m_iRow( 0 ), m_iCol( 0 ) {}
		SkillPositionKey( int iRow_, int iCol_ ) : m_iRow( iRow_ ), m_iCol( iCol_ ) {}
		bool operator < ( const SkillPositionKey& rhs_ ) const
		{
			if ( m_iRow != rhs_.m_iRow )
				return m_iRow < rhs_.m_iRow;
			return m_iCol < rhs_.m_iCol;
		}
	};

public:
	CX2UISkillTreeNew( CKTDXStage* pNowStage );
	virtual ~CX2UISkillTreeNew( void );

	HRESULT OnFrameMove( double fTime, float fElapsedTime );
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnMouseWheel( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool GetShow() { return m_bShow; }
	void SetShow( bool val );
	void SetLayer( X2_DIALOG_LAYER layer );
	D3DXVECTOR2 GetDLGSize() { return m_DLGSize; }
	void SetPosition( D3DXVECTOR2 vec );

	void InitSkillTreeUI();
	void CreateSkillTree( CX2Unit::UNIT_CLASS eUnitClass, bool bTranscended_ );	/// 328eaa50( class, unit +0x3c )
	void UpdateSkillTree();
	void UpdateSkillTree( CX2SkillTree::SKILL_ID eSkillID );

	/// June 328e7b70 ( no SP argument ). The March signature stays for the 30 callers: bSP is ignored.
	void UpdateUI( bool bEquipped, bool bSP = true, bool bSkillTree = false, bool bSkillPage_ = false );
	void BuySkillPageDialog( const int iED_ );
	void UpdateSkillPageUI();

	void CreateSlotUIPreset( SkillSlotUI& UISet, int eSkillID, int iCol );
	const map<CX2SkillTree::SKILL_ID, SkillSlotUI>& GetMapSkillSlotUISet() { return m_mapSkillSlotUISet; }
	bool GetRemoveMode() { return m_bSkillRemoveMode; }
	CKTDGUIDialogType GetDLGUISkillTree() { return m_pDLGUISkillTree; }

	/// equip bar ( S6 SERV_IRUHADEV_JUNE_SKILL_SLOT_UI, carried verbatim )
	CX2EquippedSkillSlotData* GetEquippedSlotData( int index, bool bSlotB = false );
	CKTDGUISlot* GetEquippedSlot( int index, bool bSlotB = false );
	void InitEquippedSkillSlotData();
	void UpdateSkillEquipedSlotUI();
	void UpdateJuneSkillSlotLockUI();
	void SetOffsetPosJuneTCSlotUI( const bool bTypeA_ );
	bool JuneAutoEquipNewSkills( const std::vector<int>& vecSkillID_ );
	void UpdateSkillCoolTime( float fElapsedTime );
	bool GetSkillSlotSwap() const { return m_bSkillSlotSwap; }
	void SwapSkillSlot( bool bVal, bool bForce = false );
	void SetOffsetPosSkillSLotStatic( const D3DXVECTOR2& vPos_, const D3DXVECTOR2& vAddPos_ = D3DXVECTOR2( 0, 0 ) );
	void SetOffsetStatic( const WCHAR* strControl_, const D3DXVECTOR2& vPos_ );
	CKTDGUIDialogType GetDLGGameSkillSLot() { return m_pDLGGameSkillSlot; }
	bool IsExpandSkillSlot() { return m_bIsExpandSkillSlot; }
	bool GetShowEquipSkillGuide() { return m_bShowEquipSkillGuide; }
	void UpdateSkillSlotString();
	void SetSkillUIType( bool bIsTypeA_ );
	bool GetIsSkillUiTypeA() const { return m_bIsSkillUITypeA; }
	void SwapSkillSlotTypeB( bool bVal_, bool bForce_ = false );
	void SwapSkillSlotTypeA( bool bVal_, bool bForce_ = false );
	void SetOffsetPosEtcSkillSlotUI();

	/// skill acquisition popup ( DLG_Skill_Acquisition.lua, +0xcc )
	void OpenDLGSkillUnsealed( bool bOpen );
	void UpdateDLGSkillUnsealed( CX2SkillTree::SKILL_ID eSkillID );

	/// tooltips
	void ShowSkillDesc( bool bShow, CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_NONE, D3DXVECTOR2 pos = D3DXVECTOR2( 0, 0 ),
		D3DXVECTOR2 size = D3DXVECTOR2( 0, 0 ), bool bEquipped = false, int iDescMode_ = 1 );
	void GetSkillEffectiveDesc( OUT wstring& wstrDesc, IN int iSkillLevel, IN const CX2SkillTree::SkillTemplet* pSkillTemplet, IN int iDescMode_ ) const;	/// 328e5e60: no increase-level argument
	void GetSkillTitleDesc( OUT wstring& wstrDesc, IN const CX2SkillTree::SkillTemplet* pSkillTemplet, IN int iSkillLevel, IN int iDescMode_ ) const;
	void GetSkillSubDesc( OUT wstring& wstrDesc, IN const CX2SkillTree::SkillTemplet* pSkillTemplet, IN int iSkillLevel, IN int iDescMode_, IN int iLineCount_ ) const;
	void GetSkillUnSealedDesc( OUT wstring& wstrDesc, IN const CX2SkillTree::SKILL_ID eSkillID ) const;
	void ShowUIDesc( bool bShow, wstring wstr = L"", D3DXVECTOR2 pos = D3DXVECTOR2( 0, 0 ), CX2TalkBoxManagerImp::TALK_BOX_TYPE TBT = CX2TalkBoxManagerImp::TBT_FROM_UP_LEFT,
		D3DXCOLOR coTextColor = D3DXCOLOR( 0, 0, 0, 1 ), D3DXCOLOR coBackColor = D3DXCOLOR( 1, 0.96f, 0.6f, 1 ) );

	bool ProcessSkillTreeSlotRMouseUp( LPARAM lParam );
	bool ProcessEquipSlotDropped( LPARAM lParam );
	bool ProcessEquipSlotRMouseUp( LPARAM lParam );

	/// June's new-skill marks ( +0x150 )
	void MarkJuneNewSkills( const std::vector<KSkillData>& vecSkillData_ );	/// 328da2e0
	void RecountJuneNewSkills();												/// 328da100
	void DecreaseJuneNewSkillCount();											/// 328d98e0
	int  GetJuneNewSkillCount() const { return m_iNewSkillCount; }

	/// tabs ( 328d9fb0 / 328e89f0 ); the command list itself is S7e's
	void SelectJuneTab( const bool bSkillTab_ );
	void UpdateJuneTabUI();

	/// June 322074e0 ( CX2SkillTree ): the two ids are equal, or eTarget_'s upgrade map / m_iDeleteSkillIDByUpgrade gives eSkill_
	static bool JuneIsSameOrUpgradeSkill( const int iSkill_, const int iTarget_ );
	/// June reads CX2Unit +0x3c ( transcendence ); March has no transcendence ( Stage 6 ), so the TC rules see "not transcended"
	static bool JuneIsMyUnitTranscended() { return false; }

	/// March API the June class does not have. Kept so the callers compile; each returns what June's absence means.
	bool GetNowLearnSkill() const { return false; }			/// June has no learn mode: skills are never blocked by it
	bool GetNowInitSkill() const { return false; }
	const int GetSkillLevelInSkillWimdow( const CX2SkillTree::SKILL_ID eSkillID ) const;	/// June reads GetSkillLevel ( 32890620 ) directly
	void UpdateSPInfo() {}									/// June has no SP display

private:
	void CreateSkillTreeSlots( CX2Unit::UNIT_CLASS eUnitClass );		/// 328de040
	void CreateSkillTreeLevelRows();									/// 328dc680
	void CreateSkillTreePortrait( CX2Unit::UNIT_CLASS eUnitClass, bool bTranscended_ );		/// 328e8e90
	bool JuneSkillPageCustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	/// March's page cases until S7d

private:
	int									m_iSelectedTab;				/// +0x04 0 skill tab, 1 command tab
	vector<CKTDGUIControl*>				m_vecSkillTabControl;		/// +0x08 shown with the skill tab ( 328e89f0 )
	CKTDGUIDialogType					m_pDLGCommandDesc;			/// +0x30 DLG_UI_Command_Desc_Video.lua ( S7e opens it; SelectJuneTab tests it )

	bool								m_bShow;					/// +0xa8
	bool								m_bSkillRemoveMode;			/// +0xa9 29016 toggles it
	D3DXVECTOR2							m_DLGSize;					/// +0xac

	CKTDGUIDialogType					m_pDLGUISkillTree;			/// +0xb4 DLG_UI_Skill_Tree_New2015.lua
	CKTDGUIDialogType					m_pDLGSkillToolTip;			/// +0xbc DLG_UI_Skill_Desc_Video.lua
	CKTDGUIDialogType					m_pDLGSkillUnsealInform;	/// +0xc4 nothing in the class opens it; 29030 and SetShow(false) close it
	CKTDGUIDialogType					m_pDLGSkillUnsealed;		/// +0xcc DLG_Skill_Acquisition.lua

	map<SkillPositionKey, CX2SkillTree::SKILL_ID>	m_mapSkillPosition;	/// +0xe4
	const CX2UserSkillTree*				m_pUserSkillTree;			/// +0xf4
	SHORT								m_SumDelta;					/// +0xf8
	CX2TalkBoxManager*					m_pTalkBoxManager;			/// +0x104
	bool								m_bIsExpandSkillSlot;		/// +0x109 B slots 5-8 open ( S6's UpdateJuneSkillSlotLockUI, 328dfab0 )
	CKTDGUIDialogType					m_pDLGGameSkillSlot;		/// +0x10c DLG_GAME_SKILL_SLOT_NEW_10slot.lua
	bool								m_bShowEquipSkillGuide;		/// +0x114
	bool								m_bSkillSlotSwap;			/// +0x115
	bool								m_bIsSkillUITypeA;			/// +0x116
	int									m_iMaxSkillRowNum;			/// +0x118

	bool								m_bReplacedEsCounter;		/// +0x120 read by GetSkillSubDesc's default label; nothing in June's class writes it
	CKTDGUIDialogType					m_pDLGBuyConfirm;			/// +0x124
	int									m_iConfirmED;				/// +0x12c

	map<CX2SkillTree::SKILL_ID, SkillSlotUI>	m_mapSkillSlotUISet;	/// +0x13c
	CKTDGUIButton*						m_pHoveredMemoButton;		/// +0x14c the memo button under the mouse ( S7c sets it: 29076 / 29077 )
	int									m_iNewSkillCount;			/// +0x150
};

/////////////////////////////////////////////////////////////////////////
// slot data type 1 ( vt 334c2e94 )
/////////////////////////////////////////////////////////////////////////
class CX2SkillTreeSlotData : public CKTDGUISlotData
{
public:
	enum SKILL_TREE_SLOT_ADD_PICTURE_TYPE
	{
		STSAPT_DISABLE = 0,
		STSAPT_EQUIPPED,
		STSAPT_SEALED,
		STSAPT_LEVEL,
		STSAPT_CANLEARN,
		STSAPT_SELECT,
		STSAPT_NEW,			/// June 328dbcd0 case 6: the new-skill glow
		STSAPT_END,
	};

	CX2SkillTreeSlotData();
	virtual ~CX2SkillTreeSlotData();
	virtual void UpdateUI( CKTDGUISlot* pSlot );
	void ShowSlotPicture( bool bShow, CKTDGUISlot* pSlot, SKILL_TREE_SLOT_ADD_PICTURE_TYPE eType, int val = -1 );

	CX2SkillTree::SKILL_ID	m_eSkillID;		/// +0x0c
	bool					m_bNewSkill;	/// +0x10 June-only: set by 328da2e0, cleared on mouse-in ( 29014 )
};

/////////////////////////////////////////////////////////////////////////
// slot data type 2 ( vt 334c2ea0 )
/////////////////////////////////////////////////////////////////////////
class CX2EquippedSkillSlotData : public CKTDGUISlotData
{
public:
	enum SKILL_EQUIPPED_SLOT_ADD_PICTURE_TYPE
	{
		SESAPT_DISABLE = 0,
		SESAPT_LEVEL,
		SESAPT_MEMORIZE,	/// June 328e0460 case 2: Aisha's memorize sign ( character material, not built here )
		SESAPT_END,
	};

	CX2EquippedSkillSlotData();
	CX2EquippedSkillSlotData( int index, bool bSlotb );
	virtual ~CX2EquippedSkillSlotData();

	int GetSlotID() const;
	virtual void UpdateUI( CKTDGUISlot* pSlot );
	void ShowSlotPicture( bool bShow, CKTDGUISlot* pSlot, SKILL_EQUIPPED_SLOT_ADD_PICTURE_TYPE eType, int val = -1, bool bIsSkillLevelUpByItem = false );

	CX2SkillTree::SKILL_ID	m_eSkillID;		/// +0x0c
	int						m_iIndex;		/// +0x10
	bool					m_bSlotb;		/// +0x14

private:
	void SetLockedSlotUI( CKTDGUISlot* pSlot, const bool bLocked_ );	/// 328e1830
};
#endif SERV_IRUHADEV_JUNE_SKILL_TREE_UI
