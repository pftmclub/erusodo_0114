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
	CKTDGGFXManagerImpl*	m_pImpl;
};

#ifdef SERV_IRUHADEV_JUNE_GFX_DEBUG
// Dev-console hooks ( C++ Lua bindings, CKTDXApp::RegisterLuabind ): no .lua is involved.
void JuneGfxOpenUI_LUA( int iUIID );
void JuneGfxLoadWidget_LUA( int iUIID, const char* szName );	// GetPullPath( name ), then loadUserWidget( id, path, 0, 0 ) as Play 3226b130 does
void JuneGfxUnloadWidget_LUA( int iUIID );						// unloadUserWidget( id )
void JuneGfxShow_LUA( bool bShow );								// +0x108
#endif SERV_IRUHADEV_JUNE_GFX_DEBUG
#endif SERV_IRUHADEV_JUNE_GFX_MANAGER
