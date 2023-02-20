#include "stdafx.h"
#include "PlayerMain.h"
#include "CommonMacro.h"
#include "wrap_base.h"
#include "PlayerMgr.h"
#include "Log.h"
#include "FFMpegYUVConv.h"
CPlayerMain::CPlayerMain(int id)
{
	mId = id;
	ps = NULL;
	pPlayerVideo = NULL;
	pPlayerAudio = NULL;
}

CPlayerMain::~CPlayerMain()
{

}

int decode_thread_main(void *arg)
{
	CPlayerMain *pPlayerMain = (CPlayerMain *)arg;
	if (!pPlayerMain)
	{
		return -1;
	}

	pPlayerMain->decode_thread();

	return 0;
}


int CPlayerMain::play_re_main()
{
	if (!ps)
		return -1;
	if(!ps->quit)
		return -1;
	char buf[1024] = { 0 };
	strcpy(buf, ps->filename);
	HWND	hSdlHwnd;
	HWND	hSdlHwnd2;
	hSdlHwnd = ps->hBackSdlHwnd;
	hSdlHwnd2 = ps->hBackSdlHwnd2;
	bool bEnableAudioPlay = ps->bEnableAudioPlay;
	return play_main(buf, hSdlHwnd, ps->hSdlHwnd2, bEnableAudioPlay);
}

int CPlayerMain::play_main(char *url, HWND hSDLWnd, HWND hSDLWnd2, int bOpenAudi)
{
	
	SDL_Event      event;
	if (ps)
	{
		if (ps->bRun)
		{
			//GlobalError()->SetCurError(ERROR_FILE_HAS_PLAYING);
			return -1;
		}

		if (ps->bCloseing)
		{
			return -1;
		}
		if (ps->bPlayReadying)
		{
			return -1;
		}

		av_free(ps);

	}

	ps = NULL;

	
	if (pPlayerAudio)
	{
		SAFE_DELETE(pPlayerAudio);
	}

	if (pPlayerVideo)
	{
		SAFE_DELETE(pPlayerVideo);
	}


	ps = (PlayerState *)av_malloc(sizeof(PlayerState));
	if (ps == NULL)
	{
		PrintfDebug("malloc ps error\n");
		return -1;
	}

	player_state_init();
	ps->play_id = mId;

	if (bOpenAudi>0)
		ps->bEnableAudioPlay = true;
	else
		ps->bEnableAudioPlay = false;

	pPlayerAudio = new CPlayerAudio(this);

	
	pPlayerVideo = new CPlayerVideo(this);


	memset(ps->filename, 0, 1024);
	strcpy(ps->filename, url);

	ps->hSdlHwnd = hSDLWnd;
	ps->hSdlHwnd2 = hSDLWnd2;
	ps->bPlaying = true;
	ps->bPlayReadying = true;
	if (prepare_common() != 0)
	{
		PrintfDebug("prepare common error\n");
		ps->bPlayReadying = false;
		return -1;
	}

	av_dump_format(ps->pformat_ctx, 0, ps->filename, false);
	//至少有一种流，读流，解码。
	
	if (ps->audio_stream_index != -1)
	{
		if (packet_queue_init(&ps->audio_packet_queue) < 0)
		{
			ps->bPlayReadying = false;
			return -1;
		}
		if (pPlayerAudio->prepare_audio() >=0)
		{
			pPlayerAudio->play_audio();
		}
		
	}

	if (ps->video_stream_index != -1)
	{
		if (packet_queue_init(&ps->video_packet_queue) < 0)
		{
			play_stop_audio();
			avformat_close_input(&ps->pformat_ctx);
			ps->bPlayReadying = false;
			return -1;
		}
		if (pPlayerVideo->prepare_video()<0)
		{
			play_stop_audio();
			avformat_close_input(&ps->pformat_ctx);
			ps->bPlayReadying = false;
			return -1;
		}
		if (pPlayerVideo->play_video() < 0)
		{
			play_stop_audio();
			avcodec_close(ps->pvideo_codec_ctx);
			avformat_close_input(&ps->pformat_ctx);
			ps->bPlayReadying = false;
			return -1;
		}
	}

	ps->bPlayReadying = false;
	SDL_CreateThread(decode_thread_main, "decode_thread", this);

	::Sleep(100);
	/*while (1)
	{
		if (ps->quit == 1)
		{
			break;
		}

		
		SDL_Delay(200);
	}*/

	return 0;
	
}

int CPlayerMain::enable_audioplay(bool bPlay)
{
	if (!ps)
		return -1;
	ps->bEnableAudioPlay = bPlay;
	return 0;
}


int CPlayerMain::play_stop_audio()
{
	if (ps->audio_stream_index >= 0)
	{
		if (ps->bMainAudio)
		{
			if (ps->bAudioOpenOk)
				SDL_CloseAudio();
			ps->bMainAudio = false;
		}
		else
		{
			if (ps->bAudioOpenOk)
				SDL_CloseAudioDevice(ps->dev);
		}


	}
	return 0;
}



int CPlayerMain::play_stop()
{
	if (!ps->bRun)
		return -1;
	ps->bCloseing = true;
	ps->quit = 1;
	ps->bPlaying = false;
	if (ps->audio_stream_index >= 0)
	{
		if (ps->bMainAudio)
		{
			if (ps->bAudioOpenOk)
				SDL_CloseAudio();
			ps->bMainAudio = false;
		}
		else
		{
			if (ps->bAudioOpenOk)
				SDL_CloseAudioDevice(ps->dev);
		}


	}


	// = false;
	int nums = 0;
	while (ps->bRun)
	{
		::Sleep(100);
		nums++;
		if (nums > 10)
			break;
	}


	if (ps->pFFMpegYUVConv)
		ps->pFFMpegYUVConv->Close();

	av_free(ps->video_buf);
	//av_frame_unref(&ps->out_frame);
	free_video_packet_queue();
	free_audio_packet_queue();


	avcodec_close(ps->pvideo_codec_ctx);
	avcodec_close(ps->paudio_codec_ctx);
	avformat_close_input(&ps->pformat_ctx);
	if (ps->video_stream_index != -1)
	{
		sws_freeContext(ps->psws_ctx);
	}

	ps->bCloseing = false;
	//if (PlayerMgr()->GetRuningNums() == 0)
	//SDL_Quit();
	return 0;
	
}


static int lockmgr(void **mtx, enum AVLockOp op)
{
	switch (op) {
	case AV_LOCK_CREATE:
		*mtx = SDL_CreateMutex();
		if (!*mtx) {
			av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
			return 1;
		}
		return 0;
	case AV_LOCK_OBTAIN:
		return !!SDL_LockMutex((SDL_mutex *)*mtx);
	case AV_LOCK_RELEASE:
		return !!SDL_UnlockMutex((SDL_mutex *)*mtx);
	case AV_LOCK_DESTROY:
		SDL_DestroyMutex((SDL_mutex *)*mtx);
		return 0;
	}
	return 1;
}

static int decode_interrupt_cb(void *ctx)
{
	PlayerState *ps = (PlayerState *)ctx;
	if (ps->bPlaying)
		return 0;
	return 1;
}

int CPlayerMain::prepare_common()
{
	avcodec_register_all();
	av_register_all();
	avformat_network_init();
	av_lockmgr_register(lockmgr);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		PrintfDebug("init SDL error: %s\n", SDL_GetError());
		return -1;
	}

	AVDictionary* d = NULL;           // "create" an empty dictionary
	av_dict_set(&d, "stimeout", "3000000", 0); // 3 sec
											   //av_dict_set(&d, "allowed_media_types", "video", 0); // video only
											   //av_dict_set(&d, "rtsp_transport", "tcp", 0); // prefer tcp
	av_dict_set(&d, "buffer_size", "20971520", 0); // 20 MB

	ps->pformat_ctx = avformat_alloc_context();
	ps->pformat_ctx->interrupt_callback.callback = decode_interrupt_cb;
	ps->pformat_ctx->interrupt_callback.opaque = ps;

	//打开文件
	//pformat_ctx会有所指向，不用分配内存
	if (avformat_open_input(&ps->pformat_ctx, ps->filename, NULL, &d) != 0)
	{
		PrintfDebug("open input file error\n");
		return -1;
	}

	AVDictionary* pOptions = NULL;
	ps->pformat_ctx->probesize = 100 * 1024;
	ps->pformat_ctx->max_analyze_duration = 5 * AV_TIME_BASE;

	if (avformat_find_stream_info(ps->pformat_ctx, NULL) < 0)
	{
		PrintfDebug("Couldn't find stream info\n");
		return -1;
	}

	ps->video_stream_index = -1;
	ps->audio_stream_index = -1;
	if (find_stream_index(ps->pformat_ctx,
		&ps->video_stream_index,
		&ps->audio_stream_index) == -2)
	{
		PrintfDebug("Couldn't find any stream index\n");
		return -1;
	}

	return 0;
}


void CPlayerMain::player_state_init()
{
	ps->pformat_ctx = NULL;
	ps->quit = 0;

	ps->audio_stream_index = -1;
	ps->paudio_stream = NULL;

	ps->paudio_codec_ctx = NULL;
	ps->paudio_codec = NULL;
	ps->audio_buf_size = 0;
	ps->audio_buf_index = 0;
	//视频
	ps->video_stream_index = -1;
	ps->pvideo_stream = NULL;
	ps->pvideo_codec_ctx = NULL;
	ps->pvideo_codec = NULL;
	ps->video_buf = NULL;
	ps->video_buf_size = 0;
	ps->video_buf_index = 0;
	ps->psws_ctx = NULL;

	ps->audio_packet_queue.nb_packets = 0;
	ps->audio_packet_queue.size = 0;
	ps->video_packet_queue.nb_packets = 0;
	ps->video_packet_queue.size = 0;

	ps->pixel_w = 0;
	ps->pixel_h = 0;
	ps->window_w = 0;
	ps->window_h = 0;

	ps->pwindow = NULL;
	ps->prenderer = NULL;
	ps->ptexture = NULL;

	ps->pwindow2 = NULL;
	ps->prenderer2 = NULL;
	ps->ptexture2 = NULL;


	ps->pixfmt = AV_PIX_FMT_YUV420P;
	ps->dev = 0;

	ps->audio_clock = 0.0;
	ps->video_clock = 0.0;
	ps->pre_frame_pts = 0.0;		//前一帧显示时间
									//   ps->cur_frame_pkt_pts			= 0.0; 		//当前帧在packet中标记的pts
	ps->pre_cur_frame_delay = 40e-3; 	//当前帧和前一帧的延时，前面两个相减的结果
	ps->cur_frame_pts = 0.0;		//packet.pts
	ps->delay = 40;
	ps->hSdlHwnd = 0;
	ps->hSdlHwnd2 = 0;
	ps->bRun = false;
	ps->bCloseing = false;
	ps->bPlayReadying = false;
	ps->bPlaying = false;
	ps->mReqPlayerCmd = 0;

	ps->bMainAudio = false;
	ps->bEnableAudioPlay = false;
	ps->pFFMpegYUVConv = NULL;
	memset(ps->capturePath, 0, 512);
	ps->bAudioOpenOk = false;
	ps->mVideoWidth=0;
	ps->mVideoHeight=0;
	ps->mVideoSizeChangeNums = 0;
	ps->bVideoResHasChange = false;

	ps->hBackSdlHwnd = 0;
	ps->hBackSdlHwnd2 = 0;
	ps->bDisplaySizeChanged1 = false;
	ps->bDisplaySizeChanged2 = false;


}

bool  CPlayerMain::IsRuning(void)
{
	if (!ps)
		return false;

	return ps->bRun;
}

int CPlayerMain::play_attch_win(HWND hSDLWnd)
{
	if (!ps)
		return -1;

	if (ps->hSdlHwnd)
	{
		printf("win alread attched!");
		return -1;
	}

	ps->hSdlHwnd = hSDLWnd;

	ps->mReqPlayerCmd = PLAYER_CMD_ATTACH_WIN;
	return 0;

}
int CPlayerMain::play_attch_win2(HWND hSDLWnd)
{
	if (!ps)
		return -1;

	if (ps->hSdlHwnd2)
	{
		printf("win alread attched!");
		return -1;
	}

	ps->hSdlHwnd2 = hSDLWnd;

	ps->mReqPlayerCmd = PLAYER_CMD_ATTACH_WIN2;
	return 0;

}

int CPlayerMain::play_release_win()
{
	if (!ps)
		return -1;
	if (!ps->hSdlHwnd)
	{

		printf("win alread release!");
		return -1;
	}
	ps->mReqPlayerCmd = PLAYER_CMD_RELEASE_WIN;
	return 0;
}

int  CPlayerMain::play_release_win2()
{
	if (!ps)
		return -1;
	if (!ps->hSdlHwnd2)
	{
		printf("win alread release!");
		return -1;
	}
	ps->mReqPlayerCmd = PLAYER_CMD_RELEASE_WIN2;
	return 0;

}

bool  CPlayerMain::is_play_command_busy()
{
	if (!ps)
		return true;
	if (ps->mReqPlayerCmd == PLAYER_CMD_NULL)
		return false;

	return true;
}

void CPlayerMain::set_displaysize_changed_1()
{
	if (!ps)
		return;
	ps->bDisplaySizeChanged1 = true;
}

void CPlayerMain::set_displaysize_changed_2()
{
	if (!ps)
		return;
	ps->bDisplaySizeChanged2 = true;
}

int CPlayerMain::play_capture_pic(char *path)
{
	if (!ps)
		return -1;
	if (!ps->bRun)
		return -1;
	ps->mReqPlayerCmd = PLAYER_CMD_CAPTURE_PIC;
	memset(ps->capturePath, 0, 512);
	strcpy(ps->capturePath, path);
	return 0;
}


int thread_re_play_main(void *arg)
{
	CPlayerMain *pPlayerMain = (CPlayerMain *)arg;
	if (!pPlayerMain)
	{
		return -1;
	}
	pPlayerMain->play_stop();
	::_sleep(300);
	pPlayerMain->play_re_main();

	return 0;
}

int CPlayerMain::decode_thread()
{

	AVPacket    *packet = av_packet_alloc();

	while (1)
	{
		if (ps->quit == 1)
		{
			break;
		}

		//如果队列数据过多，就等待以下
		if (ps->audio_packet_queue.nb_packets >= MAX_AUDIO_QUEUE_SIZE || ps->video_packet_queue.nb_packets >= MAX_VIDEO_QUEUE_SIZE)
		{
			
			
			PrintfDebug("too many auido video packet\n");
			::Sleep(10);
			//SDL_Delay(10);
			continue;
		}

		if (ps->bVideoResHasChange)
		{
			ps->bVideoResHasChange = false;
			ps->hBackSdlHwnd = ps->hSdlHwnd;
			ps->hBackSdlHwnd2 = ps->hSdlHwnd2;

			SDL_CreateThread(thread_re_play_main, "play_main", this);
			
		}
		
		if (av_read_frame(ps->pformat_ctx, packet) < 0)
		{
			if (ps->pformat_ctx->pb)
			{
				if ((ps->pformat_ctx->pb->error) == 0)
				{
					PrintfDebug("over delay\n");
					//SDL_Delay(200); //不是出错，可能是读完了
					::Sleep(200);
									//                  ps->quit = 1;
					play_stop();
					break;//continue;
				}
				else
				{
					PrintfDebug("error delay\n");
					continue; 		//出错了，继续读，这里
				}
			}
			else
			{
				PrintfDebug("error delay\n");
				continue; 		//出错了，继续读，这里
			}

		}

		//读取到数据了
		if (packet->stream_index == ps->video_stream_index)
		{

			packet_queue_put(&ps->video_packet_queue, packet);

		}

		if (packet->stream_index == ps->audio_stream_index)
		{
			if(ps->bAudioOpenOk)
				packet_queue_put(&ps->audio_packet_queue, packet);

		}

	}

	//av_packet_free(&packet);
	av_packet_unref(packet);
	ps->bRun = false;
	
	return 0;
}


int CPlayerMain::free_video_packet_queue()
{
	AVPacket packet;
	int ret = 0;
	int num = 0;
	if (ps->video_packet_queue.nb_packets <= 0)
		return -1;

	while (num <MAX_VIDEO_QUEUE_SIZE)		//MAX_VIDEO_QUEUE_SIZE  ps->video_packet_queue.nb_packets
	{
		ret = packet_queue_get(&ps->video_packet_queue, &packet, 0);
		if (ret <= 0)
			return 0;
		if (ps->video_packet_queue.size <= 0)
			return 0;
		av_packet_unref(&packet);
		//av_free_packet(&packet);
		num++;
	}




	return 0;

}

int CPlayerMain::free_audio_packet_queue()
{
	AVPacket packet;
	int ret = 0;
	int num = 0;
	if (ps->audio_packet_queue.nb_packets <= 0)
		return -1;
	while (num < MAX_AUDIO_QUEUE_SIZE)	//MAX_AUDIO_QUEUE_SIZE		//ps->audio_packet_queue.nb_packets
	{
		ret = packet_queue_get(&ps->audio_packet_queue, &packet, 0);
		if (ret <= 0)
			return 0;
		if (ps->audio_packet_queue.size <= 0)
			return 0;
		av_packet_unref(&packet);
		//av_free_packet(&packet);
		num++;
	}

	return 0;

}
