#ifdef SERV_IRUHADEV_JUNE_RESULT_WIDGET
#pragma once

// June dungeon result, Phase R1 ( SERV_IRUHADEV_JUNE_RESULT_WIDGET ): June's CX2UIDungeonResultAdapter ( RTTI .?AVCX2UIDungeonResultAdapter@@, vtable 3353a060,
// ctor 32eea7e0, dtor 32eea090 / 32eeafd0 ) and its callback handler ( .?AVCX2UIDungeonResultAdapterCallbackHandler@@, function object vtable 3353a074 ).
// Map: june2017_re/result_r0_map.tsv. The Scaleform half ( the listener, the 16 data bindings, DW_DUNGEON_RESULT_VALUE ) is CKTDGGFXResultBridge in
// KTDXLIB/KTDGGFXManager.cpp; this class is its host. Owner: the dungeon state ( CX2StateDungeonGame, June +0x7c4 ).

#include "KTDGGFXManager.h"

class CX2UIDungeonResultAdapter : public IGfxResultHost
{
public:
	explicit CX2UIDungeonResultAdapter( CKTDGGFXManager* pManager );		// 32eea7e0 ( June passes the movie: the manager's +0x98, or 0 without a manager )
	virtual ~CX2UIDungeonResultAdapter();									// 32eea090

	bool	IsBound() const;													// June's adapter +4 != 0

	void	UpdateDungeonInfo( const KGfxResultInfo& kInfo );			// 32eeae30
	void	SetItem( int iIndex, int iItemID, int iNum, int iUIDLow, int iUIDHigh );	// 32eea6a0 ( the name "GameResult.getItem<N>" is the index here )
	void	ShowRetryButton( bool bShow );									// 32eea1c0
	void	EnableRetryButton( bool bEnable );								// 32eea1f0
#ifdef SERV_IRUHADEV_JUNE_RESULT_FLOW
	void	NotifyTabKey();													// 32eea070: "GameResult.onTabKeyProcess"
#endif SERV_IRUHADEV_JUNE_RESULT_FLOW

	// ---- the callback handler ( table 33539ea8 ) -----------------------------------------------------------------------------------
	virtual void	OnResultAsClose();			// notifyCloseResultUI 32eea2c0
	virtual void	OnResultAsRetry();			// notfyRetryButton 32eea2e0
	virtual void	OnResultAsInitialize();		// notifyInitialize 32eea300
	virtual void	OnResultAsArrowLeft();		// notfyitemSlotArrowLeft 32eea320
	virtual void	OnResultAsArrowRight();		// notfyitemSlotArrowRight 32eea370

private:
	CKTDGGFXResultBridge*	m_pBridge;
};

// Log tag [JUNE_RESULT] WIDGET ( ErrorLog.txt )
void ResultWidgetLog( const char* szFormat, ... );

#ifdef SERV_IRUHADEV_JUNE_RESULT_DEBUG
// Dev console ( C++ Lua bindings, X2Main::RegisterLuabind ): no .lua is involved.
void ResultOpen_LUA();		// recreates the adapter, openUI( 8 ), and fills from g_pData's result info once the movie says notifyInitialize
void ResultClose_LUA();		// closeUI( 8 ) and deletes the adapter
#endif SERV_IRUHADEV_JUNE_RESULT_DEBUG
#endif SERV_IRUHADEV_JUNE_RESULT_WIDGET
