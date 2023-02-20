#pragma once

#include "player.h"


class CPlayerMain;

class CPlayerVideo
{
public:
	CPlayerVideo(CPlayerMain *pMain);
	virtual ~CPlayerVideo();


public:
	int prepare_video();

	int play_video();

	int decode_and_show();


	int refresh_fun();

	int decode_show_fun();


	double get_audio_clock();

	double get_frame_pts(AVFrame *pframe);

	double get_delay();

protected:

	int do_player_loop();
	int display_sizechange_1();
	int display_sizechange_2();
private:
	PlayerState *ps;
	CPlayerMain *pPlayerMain;

	HANDLE hWaitHandle;
};