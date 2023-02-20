#pragma once


#include "player.h"

class CPlayerMain;

class CPlayerAudio
{
public:
	CPlayerAudio(CPlayerMain *pMain);
	virtual ~CPlayerAudio();

	int prepare_audio();

	int play_audio();

	int play_audio_real();
	
	void  audio_callback(uint8_t *stream, int len);

	int audio_decode_frame();

	int play_pause_audio(bool pause);
private:
	PlayerState *ps;
	CPlayerMain *pPlayerMain;
	int  m_play_channels;
	bool bPause;
};