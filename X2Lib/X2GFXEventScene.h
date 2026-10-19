#ifdef SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
#pragma once

// June GFx runtime, Phase G3 ( SERV_IRUHADEV_JUNE_GFX_EVENTSCENE ): June's CX2GFXEventScene ( RTTI .?AVCX2GFXEventScene@@, 0xd8 B, ctor 3226aed0,
// vtable 33486ee0, g_pData +0x274 ). It plays the Scaleform event scenes ( eventscene\X.swf ) beside March's Lua CX2EventScene ( June keeps both ).
// Map: june2017_re/g0_gfx_map.tsv. The Scaleform half ( the movie's callbacks and the two data bindings ) is CKTDGGFXEventSceneBridge in
// KTDXLIB/KTDGGFXManager.cpp; this class is its host.
//
//   data   : VillageName.xml -> <NAME>.xml x 11 ( 31eed660 ) and GFXEventSceneInfo.xml ( 31eed7a0 ) into one map keyed by questID
//            ( June: CX2Data +0x4a8, here the scene's own member ): { swfFileName, frames [ { frame, soundKey, characterNameKey, textKey,
//            wordBalloonName, wordBalloonPos, symbols [ { symbolName, xPos, yPos, delayTime } ] } ] }
//   Play   : 3226b130   End : 3226b6c0   IsPlaying : 3226a010   frame move : 3226c120   MsgProc : 32269e00

#include "KTDGGFXManager.h"

class CX2GFXEventScene : public IJuneGfxEventSceneHost
{
public:
	CX2GFXEventScene();					// 3226aed0
	virtual ~CX2GFXEventScene();		// 3226ab10

	// ---- data ( CX2Data +0x4a8 map ) ----------------------------------------------------------------------------------------------
	void	LoadSceneInfo();									// 31eed660 then 31eed7a0
	bool	HasSceneInfo( const std::wstring& wstrName ) const;		// 31ee3160
	bool	GetSwfFileName( const std::wstring& wstrName, std::string& strSwfFileName ) const;		// 31ee3250
	int		GetSceneInfoCount() const { return (int)m_mapSceneInfo.size(); }

	// ---- play ----------------------------------------------------------------------------------------------------------------------
	void	Play( const std::wstring& wstrName, int iFrame = -1, int iArg = 0 );		// 3226b130 ( June's third argument is never read )
	void	End( bool bForce );															// 3226b6c0: bForce = the caller's bool ( the queue is dropped )
	bool	IsPlaying() const;				// 3226a010: this scene, or March's Lua scene ( June's 325ae1e0 )
	bool	IsPlayingGFX() const { return m_bPlaying; }		// vslot 1 ( 32269e40 )
	bool	IsRecentlyPlayed( const std::wstring& wstrName );		// 3226a130: the GFX half ( the Lua scene's 325aece0 has no writer in June, see JUNE_BUGS )
	void	ClearRecentlyPlayed() { m_wstrRecent.clear(); }			// 3226a260
	void	SetSkipRequested() { m_bSkipRequested = true; }			// 32269e60 ( June's tutorial-skip ACK handler 31efccf0 )
	void	SetNotifyNewEpicQuest( bool bNotify ) { m_bNotifyNewEpicQuest = bNotify; }		// +0x70: written by the quest UI ( 323e4cb0 )

	void	OnFrameMove( double fTime, float fElapsedTime );		// vslot 2 ( 3226c120 )
	bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );		// vslot 3 ( 32269e00 )

	// ---- IJuneGfxEventSceneHost ( the movie's AS callbacks ) --------------------------------------------------------------------
	virtual void	OnSceneAsEnd();
	virtual void	OnSceneAsPlaySound( const std::wstring& wstrKey );
	virtual int		OnSceneAsGetMentID( int iFrame );
	virtual bool	OnSceneAsGetFrames( std::vector< KJuneGfxSceneFrame >& vecFrames );

private:
	struct SceneInfo
	{
		std::string							m_strSwfFileName;
		std::vector< KJuneGfxSceneFrame >	m_vecFrame;
	};

	struct QueueEntry		// June: a 0x20 B node, wstring + int
	{
		std::wstring	m_wstrName;
		int				m_iFrame;
	};

	bool	LoadVillageNames( std::vector< std::string >& vecNames );		// 31ee7000
	int		LoadSceneXml( const WCHAR* wszFile, bool bRegion );				// 31eecc40 ( region ) / 31eed7a0 ( GFX ): rows added
	void	PlaySound( const std::wstring& wstrKey );						// 3226a050
	void	SetSceneUIShow( bool bShow );									// 32269f20
	void	CreateBlackLayOut();											// Play @3226b5c5..3226b611
	void	DestroyBlackLayOut();											// End @3226b7b3..3226b7da ( 31cefbc0 )
	int		GetMentID( int iFrame );										// 3226a2b0
	CKTDGParticleSystem* GetBlackLayOutParticleSystem();

	std::map< std::wstring, SceneInfo >	m_mapSceneInfo;		// CX2Data +0x4a8

	bool				m_bPlaying;				// +4
	std::wstring		m_wstrName;				// +0x78: the scene being played
	std::wstring		m_wstrRecent;			// +0x94: the scene that ended last ( IsRecentlyPlayed consumes it )
	std::deque< QueueEntry >	m_dequeQueue;	// +0x2c: scenes asked for while one plays
	bool				m_bNotifyNewEpicQuest;	// +0x70
	bool				m_bSkipHeld;			// +0xb8: ESC / right button released: the skip timer runs
	float				m_fSkipTime;			// +0xbc
	bool				m_bSkipRequested;		// +0xd4
	CKTDXDeviceSound*	m_pSound;				// +0xb0
	CKTDGGFXEventSceneBridge*	m_pBridge;		// +0xb4 ( June: the adapter, 32e59c50 )
	CKTDGParticleSystem::CParticleEventSequenceHandle	m_hBlackLayOut;		// +0xd0
	std::map< std::wstring, std::vector< int > >	m_mapMent;		// +0xc0: name -> GFX_ID per frame ( no writer in June: always empty )
};

// Lua bindings, June's CX2Main::RegisterLuabind 31d95080 ( @31de31c0 / @31de342a )
void JuneGfxPlayEventScene_LUA( const char* szName );					// 327ccad0: Play( name, -1, 0 )
bool JuneGfxIsRecentlyPlayedEventScene_LUA( const char* szName );		// 327ccb20
#endif SERV_IRUHADEV_JUNE_GFX_EVENTSCENE
