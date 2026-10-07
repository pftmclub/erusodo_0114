#pragma once

#ifdef SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
// June effect completion C9c-2b: the entry types are defined in X2BaseUnitViewerUI.cpp
struct JuneViewerFxEntry;
struct JuneViewerFxSound;
struct JuneViewerFxPlayingSound;
#endif SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
class CX2BaseUnitViewerUI : public CKTDGObject
{
public:
#ifdef SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
	// June2017 CX2BaseUnitViewerUI (vtable 3347113c): ctor 31fae120, dtor 31fae1a0
	CX2BaseUnitViewerUI(void);
	virtual ~CX2BaseUnitViewerUI(void);

	virtual void			NotifyShowObjectChanged();										// 32957760
	virtual HRESULT			OnFrameMove( double fTime, float fElapsedTime );				// 329585f0
	virtual RENDER_HINT		OnFrameRender_Prepare();										// 32957260
	virtual void			OnFrameRender_Draw();											// 32957330

	void	JuneViewerFxPlay( CKTDGXSkinAnimPtr pXSkinAnim_, const std::wstring& wstrSetName_, int iGroup_ );	// 32959640
	void	JuneViewerFxStop( int iGroup_ );												// 329594c0
	void	JuneViewerFxStopSounds( int iGroup_ );											// 32959250
#else SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
	CX2BaseUnitViewerUI(void) {};
	virtual ~CX2BaseUnitViewerUI(void) {};
#endif SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER

#ifdef SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
protected:
	std::vector<JuneViewerFxEntry*>			m_vecJuneFxMesh;			// +0x194 mesh entries (0x40 bytes)
	std::vector<JuneViewerFxEntry*>			m_vecJuneFxParticle;		// +0x1a4 particle entries
	std::vector<JuneViewerFxSound*>			m_vecJuneFxPendingSound;	// +0x1b4 pending sounds (0x30 bytes)
	std::vector<JuneViewerFxPlayingSound*>	m_vecJuneFxPlayingSound;	// +0x1c4 playing sounds (8 bytes)
	bool									m_bJuneFxShow;				// +0x190, set by Play, never cleared
	bool									m_bJuneFxLandDirty;			// +0x1d4, set when the viewer moves
	int										m_iJuneFxTriggerCode1;		// +0x1d8, written by the viewer's frame move (m_ePlayedEmotion)
	int										m_iJuneFxTriggerCode2;		// +0x1dc, motion type 7 -> 2, 8 -> 1, else 0 (always 0 in March)

private:
	void			JuneViewerFxCreatePass();																	// 32957980
	void			JuneViewerFxSoundPass();																	// 329582b0
	D3DXVECTOR3		JuneViewerFxLandPosition( bool bUseLand_, CKTDGXSkinAnim* pXSkinAnim_ );					// 32957170
#endif SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER
};
