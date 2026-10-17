#pragma once


#include "fmod/inc/fmod.h"
#include "fmod/inc/fmod_errors.h"





class CKTDSMP3Play
{
public:
	~CKTDSMP3Play(void);
	CKTDSMP3Play( FMOD_SYSTEM* pSystem );

	void Play( const WCHAR* pFileName, bool loop = true );
	void Stop();
	void Clear();

	void SetVolume( int volume );
	void SetPaused( bool bPause );

#ifdef DUNGEONSTAGE_CONTINUOUS_BGM	
	wstring GetPlayingFileName() { return m_FileName; }
#endif DUNGEONSTAGE_CONTINUOUS_BGM
#ifdef SERV_IRUHADEV_ELYSION_SCRIPT_B
	// Atlas E7: June CX2World::StopBGM ( 327fdb80 @327fdc0b ) empties the player's file name ( +0, 31f54910 with L"" ) before Stop.
	void ClearPlayingFileName() { m_FileName = L""; }
#endif SERV_IRUHADEV_ELYSION_SCRIPT_B

private:
	wstring			m_FileName;
	bool			m_bLoop;
	int				m_Volume;
	bool			m_bPaused;

	FMOD_SYSTEM*	m_pSystem;
	FMOD_SOUND*		m_pSound;
	FMOD_CHANNEL*	m_pChannel;
};
