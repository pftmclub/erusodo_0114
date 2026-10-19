#ifdef SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
#pragma once

// June quest system, Phase Q10b ( SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER ): June's story slide-show player, the object at CX2Data +0x288
// ( 0xc8 B, no vtable, no RTTI; named here after its dialog, DLG_SlideShotScripted_TalkBox ). It plays a completed story quest's cut scenes
// from Q10's data ( CX2SlideShotScriptedManager::GetStory ) with KTDGUI + UI particles, no GFx.
//
//   reset 31edee40 ( the quest loader 3258c470 @32590890, after the story data ), ctor 328c34b0, dtor 328c3290
//   Start 328c3750 ( the complete ACK 323e5a5e, the Remind button 323e295e, the story book 32d74148 = Q10c )
//   OnFrameMove 328c41f0 ( CX2Data's frame move 31ee9f50 @31eea122 ), NextCutScene 328c3210, End 328c3ae0
//   text: AddText 328c27d0, ClearText 328c2740, IsTextSpreading 328c26b0, FinishTextSpread 328c2660
//   sound: PlayBGM 328c2700, PlaySound 328c3400, StopSounds 328c2e90; particles: DeleteParticle 328c2d20, DestroyAllParticles 328c2df0
// Map: june2017_re/quest_plan/q10_story_map.tsv ( "manager+player" ), q10b_player_map.tsv.
//
// June fields not carried ( no reader in June ): +0x20 vector, +0x56, +0x70, +0x7c..+0x84, +0x8c ( last frame's time ); +0x55 is the saved
// June CX2World +0x05 byte ( see Start ), which March has no member for.

#include "X2SlideShotScriptedManager.h"

class CX2SlideShotScripted
{
public:
	CX2SlideShotScripted();				// 328c34b0
	~CX2SlideShotScripted();			// 328c3290

	bool		Start( int iQuestID_ );		// 328c3750: false outside a village / battle field or with no story; true when started or already running
	void		OnFrameMove( float fElapsedTime_ );	// 328c41f0
	void		End();						// 328c3ae0: only while running
	bool		IsRunning() const { return m_bRunning; }	// +0x51 ( read by the forced-End callers and June's gates )

private:
	typedef CX2SlideShotScriptedManager::CutScene		CutScene;
	typedef CX2SlideShotScriptedManager::CutSceneList	CutSceneList;

	void		NextCutScene();				// 328c3210
	const CutScene&	GetCut( UINT uiIndex_ ) const { return m_pStory->at( uiIndex_ ); }	// 328c5480 ( bounds-checked, throws )

	void		ClearText();				// 328c2740
	void		AddText( bool bNameLeft_, const WCHAR* wszName_, const WCHAR* wszMsg_, std::wstring wstrColor_, bool bSpread_, float fSpreadTime_ );	// 328c27d0
	bool		IsTextSpreading();			// 328c26b0
	void		FinishTextSpread();			// 328c2660

	void		PlayBGM( const WCHAR* wszName_ );	// 328c2700
	void		PlaySound( const WCHAR* wszName_ );	// 328c3400
	void		StopSounds();				// 328c2e90

	void		AddParticle( const std::string& strKey_, CKTDGParticleSystem::CParticleEventSequence* pSeq_ );	// 328c5800 / 328c5660 + 327c5f70 ( first wins )
	void		DeleteParticle( const std::string& strKey_ );	// 328c2d20
	void		DestroyAllParticles();		// 328c2df0

	std::map< std::string, CKTDGParticleSystem::CParticleEventSequenceHandle >	m_mapParticle;	// +0x04: "<cut>_<row>" ( Scene_List ), "<cut>_<row>BG" ( Background_List )
	std::vector< std::wstring >	m_vecPlayingSound;			// +0x10: Sound_List names played this cut, stopped by NextCutScene
	CKTDGParticleSystem::CParticleEventSequenceHandle	m_hBlackBack;	// +0x30: the PresentProlBlackBack background
	std::wstring				m_wstrSavedBGM;				// +0x34: the BGM playing at Start, played again at End

	bool						m_bTextShown;				// +0x50: this cut's text is up ( or the cut has none )
	bool						m_bRunning;					// +0x51
	bool						m_bShowDialog;				// +0x52: the talk box's show / enable value
	bool						m_bCursorChanged;			// +0x57
	bool						m_bCameraMoved;				// +0x58: the start fade moved the camera away
	bool						m_bEnding;					// +0x59: the end fade ( after the last cut or a skip )
	bool						m_bStarting;				// +0x5a: the start fade

	UINT						m_uiCutIndex;				// +0x5c
	UINT						m_uiBGMIndex;				// +0x60
	UINT						m_uiSoundIndex;				// +0x64
	UINT						m_uiSceneIndex;				// +0x68
	UINT						m_uiBackgroundIndex;		// +0x6c
	UINT						m_uiSceneDeleteIndex;		// +0x78
	int							m_iQuestID;					// +0x88

	float						m_fCutTime;					// +0x90
	float						m_fStartFadeTime;			// +0x94
	float						m_fEndFadeTime;				// +0x98
	float						m_fTextEndTime;				// +0x9c: when the text finished spreading, 0 = not yet

	CKTDGUIDialogType			m_pDLGTalkBox;				// +0xa4 / +0xa8: DLG_NewSlideShot.lua, named DLG_SlideShotScripted_TalkBox
	D3DXVECTOR3					m_vSavedEye;				// +0xac
	D3DXVECTOR3					m_vSavedLookAt;				// +0xb8
	const CutSceneList*			m_pStory;					// +0xc4
};

#endif SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER
