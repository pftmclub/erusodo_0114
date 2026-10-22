#ifdef SERV_IRUHADEV_JUNE_GFX_MANAGER
#pragma once

// June GFx runtime, Phase G2 ( SERV_IRUHADEV_JUNE_GFX_MANAGER ): June's GFx manager ( RTTI CKTDGGGFXManager, 0x120 B, g_pKTDXApp +0x1c8;
// ctor 31c45cc0, dtor 31c45970 ), map in june2017_re/g0_gfx_map.tsv.
// Plain header: no Scaleform type appears here, so KTDXApp.cpp, DXUT.cpp and X2Lib keep March's own include paths. Every Scaleform-typed
// member ( the Loader at +8, the HAL +0x3c, Renderer2D +0x40, the CKTDGGFXUI movie wrapper +0x98, the font configs +0xf8, the sound
// renderer +0x110 ) lives in CKTDGGFXManagerImpl, KTDGGFXManager.cpp ( per-file Scaleform include dirs in KTDXLIB_2010.vcxproj ).

class CKTDXStage;
class CKTDGGFXManagerImpl;

class CKTDGGFXManager
{
public:
	explicit CKTDGGFXManager( HWND hWnd );		// 31c45cc0 ( app +4 = hWnd -> +0xb8 )
	~CKTDGGFXManager();							// 31c45970

	// 31c456e0, from 31be4510 ( CKTDXApp::JuneOpenPerturbationMap ) in OnCreateDevice: loader states, the movie wrapper, main.gfx
	void		Init( IDirect3DDevice9* pDevice );

	void		OnFrameMove();												// 31c430e0: dt from timeGetTime ( +0x7c / +0xd4 )
	void		OnFrameRender();											// 31c43b20: advance, state block Capture, Renderer2D Begin / Display / End, Apply
	void		OnLostDevice();												// 31c43440
	void		OnResetDevice( IDirect3DDevice9* pDevice );					// 31c43e60
	void		OnResize( const D3DPRESENT_PARAMETERS& kPresentParams );	// 31c430c0 ( 14 dwords into +0x44 )

	void		MainLoopFilter( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	// 31c43ab0, from the PeekMessage loop
	bool		MsgProcMouse( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );		// 31c44040, first in CKTDXApp::MsgProc
	bool		MsgProcKey( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );		// 31c43890, after the stage chain

	// +0x84: the stage the movie's "MSG" / IME / Enter / arrow forwards go to ( CKTDXApp 31be2fd0 / 31be3000 )
	void		SetMessageStage( CKTDXStage* pStage );
	CKTDXStage*	GetMessageStage() const;

	// The 14 ActionScript wrappers ( thiscall on the manager, Invoke 31c3f150 on the root Value the movie registers )
	void		openUI( int iUIID );																	// 31c44530
	bool		isOpenUI( int iUIID );																	// 31c44710
	bool		closeToTopZorder();																		// 31c43bb0
	void		moveToTopZorder( int iUIID );															// 31c44860
	void		unloadWidget( int iUIID );																// 31c445d0
	void		unloadUserWidget( int iUIID );															// 31c44670
	void		loadUserWidget( int iUIID, const WCHAR* wszPath, int iArg1, int iArg2 );				// 31c45af0
	void		loadEventSceneEndFrame( int iUIID, const WCHAR* wszPath, int iFrame );				// 31c457e0
	void		clearFocus();																			// 31c43150
	void		changeCharacter( int iUnitClass );														// 31c44940
	void		showOkPopup( std::wstring wstrMessage );												// 31c44a20 ( June passes the wstring by value )
	void		ShowUI( int iUIID );																	// 31c44ae0
	void		HideUI( int iUIID );																	// 31c44bc0
	bool		isVisibleI( int iUIID );																// 31c44ca0

	bool		IsMovieLoaded() const;					// +0x98 != 0 ( the 14 external readers' test )
	bool		GetShow() const;						// +0x108: 1 = GFx shown ( render + the mouse swallow need it ); ctor 1
	void		SetShow( bool bShow );
	bool		GetEventScenePlaying() const;			// +0x115: an event scene plays -> keys ignored, mouse wheel only ( G3 writes it )
	void		SetEventScenePlaying( bool bPlaying );
	bool		GetLastLButtonDownHit() const;			// +0x114: GFx took the last WM_LBUTTONDOWN ( read by Stage 9's cash shop )
	void		SetMouseInputEnabled( bool bEnabled );	// +0x11c: ctor 0, 1 from the CX2StateServerSelect ctor ( 32576ff0 @32577e9c )
	bool		GetMouseInputEnabled() const;

	// June's app +0x28c: the id of the GFx IME text field that has focus. Its only writer in June is the Stage 9 screen callback
	// getSelectedIMETextFieldObjectID ( 32664210, table 3373b830 ); 31c435b0 / 31c43480 read 1 / 3 / 4. Kept here, 0 until Stage 9 sets it.
	void		SetIMETextFieldID( int iID );
	int			GetIMETextFieldID() const;

	// "[JUNE_GFX] MGR ..." lines, written at the first frame ( a line logged while the koms mount never reaches ErrorLog.txt )
	void		FirstFrameLog();

private:
#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
	friend class CKTDGGFXEventSceneBridge;		// reaches the root movie's data-binding adapter ( KTDGGFXManager.cpp )
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
	friend class CKTDGGFXSimpleWidgetPlayer;	// reaches the root movie's data-binding adapter ( KTDGGFXManager.cpp )
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
	friend class CKTDGGFXResultBridge;			// reaches the root movie's data-binding adapter ( KTDGGFXManager.cpp )
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
	CKTDGGFXManagerImpl*	m_pImpl;
};

#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
// ---------------------------------------------------------------------------------------------------------------------------------------
// June GFx runtime, Phase G3 ( SERV_IRUHADEV_JUNE_GFX_EVENTSCENE ): the Scaleform side of June's CX2GFXEventScene ( ctor 3226aed0 ).
// Plain types only, so X2Lib's CX2GFXEventScene ( X2GFXEventScene.{h,cpp} ) keeps March's own include paths.
// ---------------------------------------------------------------------------------------------------------------------------------------
struct KJuneGfxSceneSymbol		// one SymbolData row ( 0x28 B in June: name 0x1c, x, y, delay )
{
	std::string		m_strSymbolName;
	int				m_iPosX;
	int				m_iPosY;
	int				m_iDelayTime;
	KJuneGfxSceneSymbol() : m_iPosX( 0 ), m_iPosY( 0 ), m_iDelayTime( 0 ) {}
};

struct KJuneGfxSceneFrame		// one EventSceneDataList row ( 0x8c B in June ); strings are the XML's UTF-8
{
	int				m_iFrame;
	std::string		m_strSoundKey;
	std::string		m_strCharacterNameKey;
	std::string		m_strTextKey;
	std::string		m_strWordBalloonName;
	int				m_iWordBalloonPosX;
	int				m_iWordBalloonPosY;
	std::vector< KJuneGfxSceneSymbol >	m_vecSymbol;
	KJuneGfxSceneFrame() : m_iFrame( 0 ), m_iWordBalloonPosX( 0 ), m_iWordBalloonPosY( 0 ) {}
};

// What the movie's four ActionScript callbacks ( table 33531f58 ) ask of the scene
class IJuneGfxEventSceneHost
{
public:
	virtual ~IJuneGfxEventSceneHost() {}
	virtual void	OnSceneAsEnd() = 0;										// endEventScene 32e597e0: End( false )
	virtual void	OnSceneAsPlaySound( const std::wstring& wstrKey ) = 0;	// playEventSceneSound 32e59d40 / the sound of playedEventSceneAndGetMent: PlaySound 3226a050
	virtual int		OnSceneAsGetMentID( int iFrame ) = 0;					// playedEventSceneAndGetMent 32e59e00: row 3226a2b0
	virtual bool	OnSceneAsGetFrames( std::vector< KJuneGfxSceneFrame >& vecFrames ) = 0;	// getEventSceneData 32e59f10: 32e5aa50 + 32e5b030
};

// Made by CX2GFXEventScene's ctor ( 32e59c50 ), deleted by its dtor ( 32e5a9c0 / 32e59820 ): registers the scene's listener on the root movie's
// adapter and the "EventScene.nextScene" ( trigger ) and "EventScene.bExactFit" ( bool ) data bindings, removes them again.
class CKTDGGFXEventSceneBridge
{
public:
	CKTDGGFXEventSceneBridge( CKTDGGFXManager* pManager, IJuneGfxEventSceneHost* pHost );
	~CKTDGGFXEventSceneBridge();

	bool	IsBound() const;			// the root movie's adapter was there ( June's adapter +8 != 0 )
	void	FireNextScene();			// 32e59800: notify "EventScene.nextScene"

private:
	class CImpl;
	CImpl*	m_pImpl;
};
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE

#ifdef SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN
// ---------------------------------------------------------------------------------------------------------------------------------------
// June GFx runtime, Phase G4 ( SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN ): June's CX2SimpleWidgetPlayer ( RTTI .?AVCX2SimpleWidgetPlayerAdapter@@, 8 B:
// vtable 33471da4, +4 = the root movie's adapter; ctor 31fc74e0, dtor 31fc72e0 / 31fc7650 ), owned by CX2Main ( +0xd7c ). Plain types only.
// ---------------------------------------------------------------------------------------------------------------------------------------
class CKTDGGFXSimpleWidgetPlayer
{
public:
	explicit CKTDGGFXSimpleWidgetPlayer( CKTDGGFXManager* pManager );	// 31fc74e0: listener "simpleWidgetPlayer" + the bindings playSimpleWidget / clearSimpleWidgetPlayer
	~CKTDGGFXSimpleWidgetPlayer();										// 31fc72e0: the same three taken off again

	bool	IsBound() const;											// this +4 != 0: there was a root movie adapter
	// 31fc75e0: false without an adapter or binding; else sets the binding's widgetID / fileName / parameter1 / parameter2 and notifies it
	bool	PlaySimpleWidget( int iUIID, const char* szFileName, int iParam1, int iParam2 );

private:
	class CImpl;
	CImpl*	m_pImpl;
};
#endif SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN

#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
// ---------------------------------------------------------------------------------------------------------------------------------------
// June dungeon result, Phase R1 ( SERV_IRUHADEV_JUNE_RESULT_WIDGET ): the Scaleform half of June's CX2UIDungeonResultAdapter ( RTTI .?AVCX2UIDungeonResultAdapter@@,
// vtable 3353a060, ctor 32eea7e0, dtor 32eea090 ). It is a listener "UIDungeonResult" on the root movie's adapter and owns the data bindings
// GameResult.updateDungeonInfo ( DW_DUNGEON_RESULT_VALUE, 0x88 B ), GameResult.getItem0..11 ( 0x40 B ), GameResult.onTabKeyProcess, GameResult.showRetryButton and
// GameResult.enableRetryButton. Plain types only, so X2Lib keeps March's own include paths.
// ---------------------------------------------------------------------------------------------------------------------------------------
struct KJuneGfxResultEntry		// June's 0x28 B map entry: one player in one of the six categories ( node +0xc )
{
	bool			m_bIsMyUnit;		// +0
	std::wstring	m_wstrName;			// +4
	int				m_iCategory;		// +0x20
	float			m_fDetailValue;		// +0x24
	int				m_iUnitType;		// +0x28
	KJuneGfxResultEntry() : m_bIsMyUnit( false ), m_iCategory( 0 ), m_fDetailValue( 0.f ), m_iUnitType( 0 ) {}
};

struct KJuneGfxResultInfo		// 324e20d0: the struct 32eeae30 copies into DW_DUNGEON_RESULT_VALUE
{
	int				m_iTotalRank;		// [0]
	int				m_iSec;				// [1]
	int				m_iBestSec;			// [2]
	int				m_iED;				// [3]
	int				m_iHitRank;			// [4]
	int				m_iComboRank;		// [5]
	int				m_iTechnicalRank;	// [6]
	int				m_iBeAttackedRank;	// [7]
	int				m_iTotalEXP;		// [8]
	float			m_fExpRate;			// [9]
	float			m_fOldExpRate;		// [10]
	bool			m_bLevelUp;			// [11]
	int				m_iLevel;			// [12]
	std::vector< std::vector< KJuneGfxResultEntry > >	m_vecCategory;	// [0xe]: six lists, each in June's map order
	bool			m_bShowExitButton;	// [0x12]
	int				m_iResonanceLevel;	// [0x13]
	int				m_iUnitType;		// [0x14]
	int				m_iPlayerCount;		// [0x15]
	int				m_iRewardType;		// [0x16]
	std::vector< std::wstring >	m_vecExpBonusString;	// [0x17]
	std::vector< std::wstring >	m_vecEdBonusString;		// [0x1b]
	bool			m_bReTryButtonShow;	// [0x1f]
	KJuneGfxResultInfo()
		: m_iTotalRank( 0 ), m_iSec( 0 ), m_iBestSec( 0 ), m_iED( 0 ), m_iHitRank( 0 ), m_iComboRank( 0 ), m_iTechnicalRank( 0 ), m_iBeAttackedRank( 0 )
		, m_iTotalEXP( 0 ), m_fExpRate( 0.f ), m_fOldExpRate( 0.f ), m_bLevelUp( false ), m_iLevel( 0 ), m_bShowExitButton( false ), m_iResonanceLevel( 0 )
		, m_iUnitType( 0 ), m_iPlayerCount( 0 ), m_iRewardType( 0 ), m_bReTryButtonShow( false )
	{
	}
};

struct KJuneGfxResultItem		// the 0x40 B GameResult.getItemN value ( 3353a080 ): itemID +8, icon path +0x10, count +0x34, the item UID pair +0x38 / +0x3c
{
	int				m_iItemID;
	std::wstring	m_wstrIconPath;
	int				m_iNum;
	int				m_iUIDLow;
	int				m_iUIDHigh;
	KJuneGfxResultItem() : m_iItemID( 0 ), m_iNum( 1 ), m_iUIDLow( -1 ), m_iUIDHigh( -1 ) {}
};

// What the five callbacks of the table 33539ea8 ask of the owner ( the dungeon state )
class IJuneGfxResultHost
{
public:
	virtual ~IJuneGfxResultHost() {}
	virtual void	OnResultAsClose() = 0;			// notifyCloseResultUI 32eea2c0
	virtual void	OnResultAsRetry() = 0;			// notfyRetryButton 32eea2e0
	virtual void	OnResultAsInitialize() = 0;		// notifyInitialize 32eea300
	virtual void	OnResultAsArrowLeft() = 0;		// notfyitemSlotArrowLeft 32eea320
	virtual void	OnResultAsArrowRight() = 0;		// notfyitemSlotArrowRight 32eea370
};

class CKTDGGFXResultBridge
{
public:
	CKTDGGFXResultBridge( CKTDGGFXManager* pManager, IJuneGfxResultHost* pHost );	// 32eea7e0: listener + 16 bindings ( none without the root movie )
	~CKTDGGFXResultBridge();														// 32eea090: the listener and the 16 bindings taken off again

	bool	IsBound() const;															// June's adapter +4 != 0
	void	UpdateDungeonInfo( const KJuneGfxResultInfo& kInfo );						// 32eeae30
	void	SetItem( int iIndex, const KJuneGfxResultItem& kItem );					// 32eea6a0 ( the icon path is the caller's: 328672d0 on the item templet )
	void	ShowRetryButton( bool bShow );											// 32eea1c0
	void	EnableRetryButton( bool bEnable );										// 32eea1f0
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
	void	NotifyTabKey();															// 32eea070
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW

private:
	class CImpl;
	CImpl*	m_pImpl;
};
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET

#ifdef SERV_IRUHADEV_JUNE_GFX_DEBUG
// Dev-console hooks ( C++ Lua bindings, CKTDXApp::RegisterLuabind ): no .lua is involved.
void JuneGfxOpenUI_LUA( int iUIID );
void JuneGfxLoadWidget_LUA( int iUIID, const char* szName );	// GetPullPath( name ), then loadUserWidget( id, path, 0, 0 ) as Play 3226b130 does
void JuneGfxUnloadWidget_LUA( int iUIID );						// unloadUserWidget( id )
void JuneGfxShow_LUA( bool bShow );								// +0x108
#endif SERV_IRUHADEV_JUNE_GFX_DEBUG
#endif SERV_IRUHADEV_JUNE_GFX_MANAGER
