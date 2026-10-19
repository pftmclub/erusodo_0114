#pragma once

//////////////////////////////////////////////////////////////////////////
// Author: Iruha
// Date: 2026-10-19
// Description: June quest system Phase RM1 - the random dungeon mission (SERV_IRUHADEV_JUNE_RANDOM_MISSION).
//              June's CX2RandomMissionManager ( singleton 337a66b4, GetInstance 32676150 ) and CX2RandomMission ( ctor 32679bf0,
//              vtable 334b045c = the destructor only ). A mission is set only by the server push 0x492 ( here
//              EGS_JUNE_RANDOM_MISSION_NOT ), counted by 12 event types raised from March's dungeon code, and judged at the end
//              of the dungeon. Map: june2017_re/quest_plan/rm0_map.tsv. Log [JUNE_QUEST] RM.
//////////////////////////////////////////////////////////////////////////

#ifdef SERV_IRUHADEV_JUNE_RANDOM_MISSION

struct KEGS_JUNE_RANDOM_MISSION_NOT;
class CX2GUNPC;
class CX2GUUser;

class CX2RandomMission
{
public:
	/// June's event types ( the second field of the 0x492 push ), as RM0 named them
	enum EVENT_TYPE
	{
		RME_NONE				= 0,
		RME_COMBO				= 1,	/// AddCombo
		RME_BOSS_DAMAGE			= 2,	/// % of the boss's max HP
		RME_HITTED				= 3,	/// hits taken, the count must stay at or under the goal
		RME_GATE_KILL			= 4,	/// kills of a NCT_THING_GATE npc ( "break the door" )
		RME_SURVIVE				= 5,	/// no raise site ( judged at the dungeon's end )
		RME_SUB_STAGE			= 6,	/// ClearSubStage
		RME_QUICK_SLOT			= 7,
		RME_RESURRECT			= 8,
		RME_MP_SPENT			= 9,
		RME_ACTIVE_SKILL		= 10,	/// skill type ST_ACTIVE
		RME_SPECIAL_SKILL		= 11,	/// ST_SPECIAL_ACTIVE and its grades 1-3
		RME_HYPER_SKILL			= 12,	/// ST_HYPER_ACTIVE_SKILL
	};

	/// the mission's own result ( +8 )
	enum MISSION_RESULT
	{
		MR_NONE		= 0,
		MR_SUCCESS	= 1,
		MR_FAILED	= 2,
	};

	CX2RandomMission( const KEGS_JUNE_RANDOM_MISSION_NOT& kNot_ );
	~CX2RandomMission();		/// 32679c50: deletes both dialogs

	int		AddEvent( int iValue_ );			/// 32679820
	int		OnDungeonEnd();						/// 326797a0
	void	FadeTick( float fElapsedTime_ );	/// 326762d0

	void	CreateUI();							/// 32678910
	void	UpdateUI();							/// 326791c0 ( the result )
	void	UpdateCounterText();				/// 32677f90
	void	UpdateTimeText();					/// 32676660
	void	ShowSmall( bool bShow_ );			/// 326761c0
	void	ShowPopup( bool bShow_ );			/// 32676200
	void	HideBoth()							{ ShowSmall( false ); ShowPopup( false ); }
	void	AppearBoth();						/// 32679a50: both dialogs shown and made transparent for the fade-in
	void	SendComplete();						/// 32676870

	int		GetEventType() const				{ return m_iEventType; }
	int		GetMissionID() const				{ return m_iMissionID; }
	int		GetResult() const					{ return m_iResult; }
	int		GetCounter() const					{ return m_iCounter; }
	int		GetGoal() const						{ return m_iGoal; }

private:
	std::wstring	BuildText() const;			/// 32677020 / 326768b0: the time limit and the description
	std::wstring	BuildTooltipPrefix() const;	/// 32677740: the same text with the red colour code swapped, plus a newline
	std::wstring	BuildRewardText() const;	/// 326784b0: the popup's reward lines
	std::wstring	BuildRewardTip( const WCHAR* wszPrefix_ ) const;	/// the 30745 line ( the reward button's tooltip )
	void			RefreshSmallText();			/// the StaticInfo line 0 and the reward button's tooltip, shared by 32678910 / 326791c0 / 32677f90

	int				m_iMissionID;				/// +4
	int				m_iResult;					/// +8
	int				m_iEventType;				/// +0xc
	int				m_iCounter;					/// +0x10
	int				m_iGoal;					/// +0x14
	int				m_iDescStrID;				/// +0x18
	int				m_iRewardBuffID;			/// +0x1c ( read from the UI text, not proven )
	int				m_iRewardExpRate;			/// +0x20 ( read from the UI text, not proven )
	float			m_fTimeAccum;				/// +0x24 ( -1.0 at construction )
	int				m_iRemainSec;				/// +0x28
	int				m_iTotalSec;				/// +0x2c
	float			m_fFadeTimer;				/// +0x30 ( -1.0 at construction )
	CKTDGUIDialogHandle	m_hDlgSmall;			/// +0x34 DLG_UI_Random_Mission.lua
	CKTDGUIDialogHandle	m_hDlgPopup;			/// +0x3c DLG_UI_Random_Mission_PopUp.lua

	friend class CX2RandomMissionManager;
};

class CX2RandomMissionManager
{
public:
	/// manager state ( +8 )
	enum MANAGER_STATE
	{
		MS_NONE		= 0,
		MS_RECEIVED	= 1,
		MS_RUNNING	= 2,
		MS_SUCCESS	= 3,
		MS_FAILED	= 4,
	};

	static CX2RandomMissionManager*	GetInstance();		/// 32676150: creates on first use
	static void						DestroyInstance();	/// 32676590

	void	Reset();									/// 32676070
	void	SetState( int iState_ )						{ m_iState = iState_; }
	int		GetState() const							{ return m_iState; }
	void	StartPopupTimer();							/// 326760c0
	bool	IsRunningEvent( int iEventType_ ) const;	/// 326765d0
	void	RefreshUI();								/// 326765f0
	void	AddEvent( int iValue_ );					/// 32679970
	void	OnDungeonEnd();								/// 32679a20
	void	OnFrameMove( float fElapsedTime_ );			/// 32679a50
	void	SetMission( const KEGS_JUNE_RANDOM_MISSION_NOT& kNot_ );	/// 326798b0
	void	OnSkillUse( int iSkillID_ );				/// 31fcd3d0
	void	OnStageStart( int iNowStageIndex_ );		/// 327d1820 @327d1b8a
	void	OnBossDamage( CX2GUNPC* pNpc_, CX2GUUser* pMyUnit_, float fFinalDamage_ );	/// 327b1f30 @327b4f87 / @327b5063: event 2
	void	OnMpSpent( float fSpent_ );					/// 322b33d0 @322b3492: event 9
	bool	IsActiveState() const						{ return m_iState >= MS_RECEIVED && m_iState <= MS_FAILED; }	/// the sites that test 'state - 1 <= 3'
	void	SetShowUIByte( bool bShow_ )				{ m_bShowUI = bShow_; }
	void	SetToggleByte( bool bShow_ )				{ m_bToggleUI = bShow_; }

	/// the sites that read the mission ( rm0_map.tsv )
	CX2RandomMission*	GetMission() const				{ return m_pMission; }

private:
	CX2RandomMissionManager();

	bool				m_bShowUI;					/// +0, false at construction ( June stores the word 0x100 )
	bool				m_bToggleUI;				/// +1, true at construction
	float				m_fPopupTimer;				/// +4 ( -1.0 )
	int					m_iState;					/// +8
	CX2RandomMission*	m_pMission;					/// +0xc

	static CX2RandomMissionManager*	ms_pInstance;	/// 337a66b4
};

#endif SERV_IRUHADEV_JUNE_RANDOM_MISSION
