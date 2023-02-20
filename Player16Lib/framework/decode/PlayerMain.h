#pragma once

#include "player.h"
#include "PlayerAudio.h"
#include "PlayerVideo.h"

class CPlayerMain
{
public:
	CPlayerMain(int id);
	virtual ~CPlayerMain();



public:


	int play_main(char *url, HWND hSDLWnd, HWND hSDLWnd2, int bOpenAudi);
	
	int play_re_main();
	int play_stop();
	

	int play_stop_audio();

	int play_attch_win(HWND hSDLWnd);
	int play_attch_win2(HWND hSDLWnd);

	int play_release_win();
	int play_release_win2();

	void set_displaysize_changed_1();
	void set_displaysize_changed_2();

	int play_capture_pic(char *path);


	bool is_play_command_busy();

	int enable_audioplay(bool bPlay);

	PlayerState * GetPlayerState(void) { return ps; }
	int GetId(void) { return mId; }
	int decode_thread();
	bool IsRuning(void);

protected:
	void player_state_init();
	int prepare_common();
	int free_audio_packet_queue();
	int free_video_packet_queue();
	
private:
	PlayerState *ps;
	CPlayerVideo *pPlayerVideo;
	CPlayerAudio *pPlayerAudio;
	int mId;
};