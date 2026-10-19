#pragma once

class CKTDGUIDialogManager : public CKTDXStage
{
	public:
		CKTDGUIDialogManager(void);
		~CKTDGUIDialogManager(void);

#ifdef DIALOG_SHOW_TOGGLE
		bool	GetShow() { return m_bShow; }
		void	ToggleShow() { SetShow(!m_bShow);}
#endif
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
		/// June GFx runtime G5: June's SetShow( bShow, mode ) ( 31d05af0 ) takes the hide mode; the one-argument form is mode 0, as every March / Lua call site passes it.
		void	SetShow( bool bShow ) { JuneSetShow( bShow, 0 ); }
#else SERV_IRUHADEV_JUNE_GFX_GATES
		void	SetShow( bool bShow ) { m_bShow = bShow; }
#endif SERV_IRUHADEV_JUNE_GFX_GATES

		void	RegisterLuaBind();

        bool	AddDlg( CKTDGUIDialogType pDialog );
		//bool	AddDlg( CKTDGUIDialogHandle hDialog_ );
//{{ robobeg : 2008-01-07
        bool    UnmanageDlg( CKTDGUIDialogType pDialog );
//}} robobeg : 2008-01-07

        bool	DeleteDlg( CKTDGUIDialogType pDialog );
		//{{ seojt // 2009-8-18, 18:00
		//bool	DeleteDlg( CKTDGUIDialogHandle hDialog_ );
		//}} seojt // 2009-8-18, 18:00
//{{ robobeg : 2008-01-07
		//bool	DeleteDlgByName( const WCHAR* pName );
//}} robobeg : 2008-01-07
		void	ClearAllDlg();
		bool	ClearAllOKAndCancelMsgDlg();
		
//{{ robobeg : 2009-01-07
		//std::vector<CKTDGUIDialogType>& GetDialogList() { return m_vecDialog; }
//}} robobeg : 2009-01-07

		bool	CheckDlg( CKTDGUIDialogType pDialog );
		bool	CheckModalDlg();
		bool	CheckFrontModalDlg( CKTDGUIDialogType pPivotDialog ); //해당 다이얼로그 앞에 모달 다이얼로그가 있는지 확인
		CKTDGUIDialogType GetFirstFrontModalDlg();		//현제 맨앞에서 모달 역할을 하는 다이얼로그 가져오기
		bool	CheckMostFrontDlg( CKTDGUIDialogType pDialog ); //이 다이얼로그가 맨 앞에 보여지는 다이얼로그인지 확인		

		void	CountModalDialog( CKTDGUIDialog* pDialog = NULL );


		bool	ChangeLayer( CKTDGUIDialogType pDialog, int layerToChange );
        bool    ChangeFront( CKTDGUIDialogType pDialog, bool bFront ); ///< front, rear 변경
        bool	ChangeSequence( CKTDGUIDialogType pDialog, bool bTop ); //속해있는 레이어에서 가장 앞으로 옮길것이냐 가장 뒤로 옮길것이냐?
		//{{ seojt // 2009-8-18, 18:12, qff
		//bool	ChangeSequence( CKTDGUIDialogHandle hDialog_, bool bTop ); //속해있는 레이어에서 가장 앞으로 옮길것이냐 가장 뒤로 옮길것이냐?
		//}} seojt // 2009-8-18, 18:12
		bool	ChangeSequence( CKTDGUIDialogType pDialog, CKTDGUIDialogType pPivotDialog, bool bOver, bool bChangeLayer = false );

		CKTDGUIDialog* GetDialog_LUA( const char* pName ); 
		CKTDGUIDialogType GetDialog( const WCHAR* pName );

#ifdef PRIORITY_EVERY_UI
		bool ExistDialogAtMousePos();
#endif PRIORITY_EVERY_UI


		virtual	HRESULT OnFrameMove( double fTime, float fElapsedTime );
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
		// June 31d053f0 ( vslot 0x30 ): ( bFront, pass ); pass 0 is March's frame, pass 1 the post-effect pass ( R4b )
		virtual	HRESULT OnFrameRender( bool bFront = false, int iRenderPass_ = 0 );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
		virtual	HRESULT OnFrameRender( bool bFront = false );
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
#ifdef SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
		// June 31d05700 ( vslot 0x38 ): only dialogs whose layer is in [ iMinLayer_, iMaxLayer_ ] ( CKTDGManager::FrontUIRender )
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
		// June's order is ( bFront, pass, iMinLayer_, iMaxLayer_ ); the pass goes last here so the three-argument calls keep their meaning
		virtual	HRESULT OnFrameRender( bool bFront, int iMinLayer_, int iMaxLayer_, int iRenderPass_ = 0 );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
		virtual	HRESULT OnFrameRender( bool bFront, int iMinLayer_, int iMaxLayer_ );
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
		// June +0x110: only dialogs flagged +0x1e0 are drawn ( 31d0556a / 31d0587b ); FrontUIRender then draws no band. Written only by
		// June-only game code ( 31f1e108, 3223dfea, 3243d15f, ... ), not ported: false here
		bool	GetJuneFlaggedDialogsOnly() const { return m_bJuneFlaggedDialogsOnly; }
		void	SetJuneFlaggedDialogsOnly( bool bOnly_ ) { m_bJuneFlaggedDialogsOnly = bOnly_; }
#endif SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS

		virtual HRESULT OnFrameRenderTopModal();

		virtual	bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual	HRESULT OnResetDevice();
		virtual	HRESULT OnLostDevice();

#ifndef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
		void	SetFont( int uiFontID, CKTDGFontManager::CKTDGFont* pFont );
		CKTDGFontManager::CKTDGFont* GetFont( int fontID ){ return m_mapFontList[fontID]; }
#endif

		void	SetUKFont( int uiUKFontID, CKTDGFontManager::CUKFont* pFont );
		CKTDGFontManager::CUKFont* GetUKFont( int fontID ){ return m_mapUKFontList[fontID]; }

#ifdef  KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
		void    MapFontToUKFont( int fontID, int uiUKFontID )
		{
			if ( m_mapUKFontList.find( uiUKFontID ) != m_mapUKFontList.end() )
			{
				m_mapFontToUKFont[ fontID ] = uiUKFontID;
			}//if
		}

		CKTDGFontManager::CUKFont* GetMappedUKFont( int fontID )
		{
			std::map< int, int >::const_iterator iter = m_mapFontToUKFont.find( fontID );
			if ( iter == m_mapFontToUKFont.end() )
				return  NULL;
			return  GetUKFont( iter->second );
		}
#endif

//{{ robobeg : 2009-01-07
		//CKTDGUIDialogType GetDialogByKeyID( int iKeyID );
//}} robobeg : 2009-01-07
		
		bool IsDraggingDialog();

		CKTDGUIDialogType GetCloseDialogByESC();

		void SetStage( CKTDXStage* pStage );

		bool GetFocusInEditBox();	// imeedit or editbox에 포커스가 있는지 검사한다.

#ifdef DIALOG_SHOW_TOGGLE
		bool GetHideDialog();
		void SetCanHide(bool bVal) { m_bCanHide = bVal; }
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
		/// June GFx runtime G5 ( SERV_IRUHADEV_JUNE_GFX_GATES ): June's dialog hide modes. 0 = shown, 1 = hide all ( what March has ), 2-4 = the result-UI
		/// modes, 5 = the presentation mode; 2-5 are entered only by Stage 9 code ( June 31f14e40, 324c95b0, 324d8600, 3243be8b, 324ac353, 328c3750 ).
		/// Each mode keeps a name list ( preset, StartUp loader 32597410 ) of the dialogs that still run and draw while the UI is hidden.
		void	JuneSetShow( bool bShow, int iHideMode );										// 31d05af0
		void	JuneToggleHide();																// 31f02f50: the DELETE-key toggle
		int		GetJuneHideMode() const { return m_iJuneHideMode; }							// +0xc8
		int		GetJuneHidePrevMode() const { return m_iJuneHidePrevMode; }					// +0xcc
		static bool	IsJuneRestoreMode( int iMode ) { return 2 == iMode || 3 == iMode || 4 == iMode; }	// 31d03940
		bool	IsJuneHideModeKeepingHidden() const { return IsJuneRestoreMode( m_iJuneHideMode ); }	// 31d03a10: ESC does not unhide in modes 2-4
#endif SERV_IRUHADEV_JUNE_GFX_GATES
#endif

private:

        void    _InsertIntoDialogListTop( CKTDGUIDialogType pkInDialog_ );
        void    _InsertIntoDialogListBottom( CKTDGUIDialogType pkInDialog_ );
        bool    _RemoveFromDialogList( CKTDGUIDialogType pkInDialog_ );

		inline void	IncreaseDialogListChangeCount()
		{
			m_iDialogListChangeCount += 1;
			if( m_iDialogListChangeCount > INT_MAX - 10 )
			{
				m_iDialogListChangeCount = 1;
			}
		}

private:

		bool										m_bShow;

//{{ robobeg : 2009-01-07
		CKTDGUIDialogMap							m_mapDialog;			// dialog name으로 dialog handle을 검색할 때 사용
		CKTDGUIDialogSet							m_setDialog;			// dialog handle로 dialog가 있는지 없는지 검사할 때 사용
        CKTDGUIDialogList                           m_listDialog;			// dialog를 순차적으로 traverse할 때 사용
        std::vector<CKTDGUIDialogList::iterator>    m_vecIterator;
        std::vector<CKTDGUIDialogList::iterator>    m_vecReverseIterator;
//}} robobeg : 2009-01-07


#ifndef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
		map< int, CKTDGFontManager::CKTDGFont* >	m_mapFontList;
#endif  KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT

		map< int, CKTDGFontManager::CUKFont* >		m_mapUKFontList;

#ifdef KTDGDEVICEFONT_SIMULATE_DIRECTX_FONT
		std::map< int, int >                        m_mapFontToUKFont;
#endif


		int m_iModalDialogCount;		// 현재 dialog list에 포함 되어있는 modal dialog 개수
		int m_iDialogListChangeCount;	// dialog가 dialog list에 추가될때, list가 변경된 회수를 저장. dialog list에 추가된 이후에 dialog list가 변동이 있었는지 확인하기 위해 사용.
		std::map< std::wstring, int > m_mapDialogListChangeCountWhenLastGetDialog; // < name, dialog_list_change_count> // 마지막으로 GetDialog(name) 함수를 호출했을때 dialog list change count

#ifdef DIALOG_SHOW_TOGGLE
		bool m_bCanHide;
#ifdef SERV_IRUHADEV_JUNE_GFX_GATES
		int										m_iJuneHideMode;			// +0xc8: the current hide mode ( ctor: 0 )
		int										m_iJuneHidePrevMode;		// +0xcc: the mode before the last SetShow
		std::set< std::wstring >				m_setJuneHideNames;			// +0xd0 ( head +0xd4 ): the current mode's name list ( June: a std::set of narrow names )
		std::map< int, std::set< std::wstring > >	m_mapJuneHidePreset;	// +0x100: mode -> name list, built by _BuildJuneHidePresets ( June: set of 0x40 B presets )
		void	_BuildJuneHidePresets();										// June's StartUp loader 32597410 @3259b25e-3259bafb
#endif SERV_IRUHADEV_JUNE_GFX_GATES
#endif
#ifdef SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
		bool m_bJuneFlaggedDialogsOnly;
#ifdef SERV_IRUHADEV_JUNE_RENDER_PASS1
		HRESULT _OnFrameRenderJune( bool bFront, bool bLayerBand_, int iMinLayer_, int iMaxLayer_, int iRenderPass_ );
#else SERV_IRUHADEV_JUNE_RENDER_PASS1
		HRESULT _OnFrameRenderJune( bool bFront, bool bLayerBand_, int iMinLayer_, int iMaxLayer_ );
#endif SERV_IRUHADEV_JUNE_RENDER_PASS1
#endif SERV_IRUHADEV_JUNE_UI_DIALOG_BANDS
};
