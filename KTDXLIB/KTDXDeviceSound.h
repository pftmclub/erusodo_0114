#pragma once

#include "fmod/inc/fmod.h"
#include "fmod/inc/fmod_errors.h"
#ifdef	X2OPTIMIZE_SOUND_BACKGROUND_LOAD
#include "KGCMassFileBufferManager.h"
#endif	X2OPTIMIZE_SOUND_BACKGROUND_LOAD


class CKTDXDeviceSound : public CKTDXDevice
{
public:
	CKTDXDeviceSound( FMOD_SYSTEM* pSystem, wstring fileName, bool bUse3D = true );



	void Play( bool loop = false, bool b3DSound = true );

#ifdef CHECK_SOUND_LOADING_TIME
	void CheckPlay( bool& bCreateSound, bool loop = false, bool b3DSound = true );
#endif // CHECK_SOUND_LOADING_TIME

	void Wait();
	void Stop();

	bool IsPlaying();
#ifdef SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE
	// June: the manager reads +0x60 = m_pChannel to match the ended channel (31cdcbb0)
	FMOD_CHANNEL* GetChannel() const { return m_pChannel; }
#endif SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE
#ifdef SERV_IRUHADEV_JUNE_DAMAGEEFFECT_SOUND
	// June CEffect::OnFrameMove_SoundData 31f6eca0 @31f6ede4 reads +0x5c = m_pSound for its DELETE_WHEN_EFFECT_END pair
	FMOD_SOUND* GetSound() const { return m_pSound; }
	// June 31d41710 ( no symbol; its only caller is that sound play @31f6ee0e ). June keeps March's store-only
	// Set3DPosition too ( 31d41070 ); this one also applies the position to the playing channel.
	void JuneSet3DPositionAndUpdate( const D3DXVECTOR3& pos );
#endif SERV_IRUHADEV_JUNE_DAMAGEEFFECT_SOUND
	void Set3DPosition( const D3DXVECTOR3& pos );
	void SetMax3DDistance( float fMaxDistance );
	void SetVolume( float fVolume );

protected:

	virtual ~CKTDXDeviceSound(void);

	bool Update3DPosition();
	virtual HRESULT _Load( bool bSkipStateCheck = false
#ifdef	X2OPTIMIZE_SOUND_BACKGROUND_LOAD
				, bool bBackgroundQueueing = false
#endif	X2OPTIMIZE_SOUND_BACKGROUND_LOAD		
		);

#ifdef CHECK_SOUND_LOADING_TIME
	 virtual HRESULT _CheckLoad( bool& bCreateSound,
		bool bSkipStateCheck = false
#ifdef	X2OPTIMIZE_SOUND_BACKGROUND_LOAD
		, bool bBackgroundQueueing = false
#endif	X2OPTIMIZE_SOUND_BACKGROUND_LOAD		
		);
#endif // CHECK_SOUND_LOADING_TIME

	virtual HRESULT _UnLoad();
#ifdef	X2OPTIMIZE_SOUND_BACKGROUND_LOAD
	void			_CheckLoadFMODSound();
#endif	X2OPTIMIZE_SOUND_BACKGROUND_LOAD

#ifdef CHECK_SOUND_LOADING_TIME
	void			_CheckCheckLoadFMODSound( bool& bCreateSound );
#endif // CHECK_SOUND_LOADING_TIME

protected:


	FMOD_VECTOR 		m_vFmodPos;

	FMOD_SYSTEM*		m_pSystem;
	FMOD_SOUND*			m_pSound;
	FMOD_CHANNEL*		m_pChannel;

	bool				m_b3DSound;
    float               m_fMaxDistance;

#ifdef	X2OPTIMIZE_SOUND_BACKGROUND_LOAD
	KGCMassFileBufferPtr m_spMemoryBuffer;	
#endif	X2OPTIMIZE_SOUND_BACKGROUND_LOAD

};
