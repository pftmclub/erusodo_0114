#pragma once

//class CX2StateClassChange : public CX2StateCommonBG
//{
//	public:
//
//		enum STATE_CLASS_CHANGE_UI_CUSTOM_MSG
//		{
//			SCCUCM_STATE_CHANGE_VILLAGE = 0,
//		};
//
//	public:
//
//		CX2StateClassChange(void);
//		virtual ~CX2StateClassChange(void);
//
//		HRESULT OnFrameMove( double fTime, float fElapsedTime );
//
//	protected:
//		bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//		bool LastMsgByESC();
//		void LastMsgPopOk();
//
//	private:
//		CX2UnitViewerUI*	m_pUnitViewerUI;
//		CKTDGUIDialogType	m_pDLGClassChange;
//};



class CX2ClassChangePopup
{
public:
	enum CLASS_CHANGE_UI_CUSTOM_MSG
	{
		CCUCM_STATE_CHANGE_VILLAGE = 35200,
	};

#ifdef SERV_SHARING_BANK_QUEST_CASH
	enum HELP_POPUP_TYPE
	{
		HPT_SHARE_BANK = 1,
	};
#endif // SERV_SHARING_BANK_QUEST_CASH

public:
	CX2ClassChangePopup(void);
	virtual ~CX2ClassChangePopup(void);

	HRESULT OnFrameMove( double fTime, float fElapsedTime );

	bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetShow( bool bShow );
#ifdef SERV_SHARING_BANK_QUEST_CASH
	bool SetShow( bool bShow, int iType );
#endif // SERV_SHARING_BANK_QUEST_CASH
	bool GetShow() { return m_bShow; }
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	/// June 324886b0( bShow, 1 ): the transcendence guide mode ( a separate name: March's SetShow( bool, int ) takes HPT_SHARE_BANK = 1 )
	void SetShowTranscendGuide( bool bShow );
	/// June 324886b0's switch on the unit type ( 1 .. 12 ); NULL outside it
	static const WCHAR* JuneGetTranscendGuideName( int iUnitType_ );
	/// June 32711490: the dialog file is one of the twelve guides
	static bool JuneIsTranscendGuideName( const WCHAR* pwszFileName_ );
	/// June 32486790 ( bClassPopup_ ) / 32710710 ( level-up guide ): shows the class's picture and the popup statics of a guide dialog
	static void JuneInitTranscendGuide( CKTDGUIDialogType pDialog_, int iUnitClass_, bool bClassPopup_, CKTDGUIStatic*& pImage_, float& fImageTime_ );
	/// June 324869d0 / 327108b0: the guide image's alpha over 1.5 s
	static void JuneFadeTranscendImage( CKTDGUIStatic* pImage_, float& fImageTime_, float fElapsedTime_ );
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE

private:
	bool m_bShow;
	CX2UnitViewerUI*	m_pUnitViewerUI;
	CKTDGUIDialogType	m_pDLGClassChange;
#ifdef SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE
	float				m_fJuneTranscendImageTime;	// June popup +4
	CKTDGUIStatic*		m_pJuneTranscendImage;		// June popup +8
#endif SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE

#ifdef ADD_PLAY_SOUND //±Ë√¢«—
	void PlaySoundChangeClass( CX2Unit::UNIT_TYPE eUnitType );
#endif //ADD_PLAY_SOUND
};
