#include "stdafx.h"
#include "PlayerVideo.h"
#include "PlayerMain.h"
#include "Log.h"
#include "PlayerMgr.h"
#include "FFMpegYUVConv.h"
#include "CommonMacro.h"
#include "Utils.h"
CPlayerVideo::CPlayerVideo(CPlayerMain *pMain)
{
	pPlayerMain = pMain;
	if (pPlayerMain)
	{
		ps = pPlayerMain->GetPlayerState();
	}

	hWaitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

}


CPlayerVideo::~CPlayerVideo()
{

}


int CPlayerVideo::prepare_video()
{
	ps->pvideo_stream = ps->pformat_ctx->streams[ps->video_stream_index];
	AVCodecParameters* codecpar = ps->pformat_ctx->streams[ps->video_stream_index]->codecpar;
	ps->pvideo_codec = avcodec_find_decoder(codecpar->codec_id);		//ps->pvideo_codec_ctx->codec_id
	ps->pvideo_codec_ctx = avcodec_alloc_context3(ps->pvideo_codec);//ps->pvideo_stream->codec;
	avcodec_parameters_to_context(ps->pvideo_codec_ctx, codecpar);
	if (ps->pvideo_codec == NULL)
	{
		PrintfDebug("Couldn't find video decoder\n");
		return (-1);
	}
	
	//打开解码器
	if (avcodec_open2(ps->pvideo_codec_ctx, ps->pvideo_codec, NULL) < 0)
	{
		PrintfDebug("Couldn't open video decoder\n");
		return -1;
	}
	

	
	double framerate_temp = (double)(ps->pvideo_stream->r_frame_rate.num) / ps->pvideo_stream->r_frame_rate.den;

	framerate_temp = round(framerate_temp);
	int framenum = framerate_temp;
	ps->delay = 1000 / framenum;

	//ps->pvideo_codec_ctx->flags2 |= AV_CODEC_FLAG2_SHOW_ALL;
	if (ps->pvideo_codec_ctx->width <= 0 || ps->pvideo_codec_ctx->height <= 0)
	{
		PrintfDebug("Couldn't open video decoder\n");
		return -1;
	}

	ps->mVideoWidth = ps->pvideo_codec_ctx->width;
	ps->mVideoHeight = ps->pvideo_codec_ctx->height;

	int width = ps->pvideo_codec_ctx->width;
	int height = ps->pvideo_codec_ctx->height;
	ps->pFFMpegYUVConv = new CFFMpegYUVConv();
	ps->pFFMpegYUVConv->Init(width, height, width, height);

	

	return 0;

}

int main_refresh_fun(void *arg)
{

	CPlayerVideo *pPlayerVideo = (CPlayerVideo*)arg;

	if (pPlayerVideo)
	{
		pPlayerVideo->refresh_fun();
	}

	return 0;
}

int main_decode_show_fun(void *arg)
{
	CPlayerVideo *pPlayerVideo = (CPlayerVideo*)arg;

	if (pPlayerVideo)
	{
		pPlayerVideo->decode_show_fun();
	}

	return 0;
}

int   CPlayerVideo::display_sizechange_1()
{
	if (ps->hSdlHwnd)
	{

		if (ps->prenderer)
		{
			SDL_DestroyRenderer(ps->prenderer);
			ps->prenderer = NULL;
		}
		if (ps->ptexture)
		{
			SDL_DestroyTexture(ps->ptexture);
			ps->ptexture = NULL;
		}

		if (ps->pwindow)
		{
			SDL_DestroyWindow(ps->pwindow);
			ps->pwindow = NULL;
			ShowWindow(ps->hSdlHwnd, 1);
		}


		ps->pwindow = SDL_CreateWindowFrom((void *)(ps->hSdlHwnd));

		if (ps->pwindow == NULL)
		{
			PrintfDebug("Couldn't Create Window\n");
			return -1;
		}
		//新建一个渲染器
		try {
			ps->prenderer = SDL_CreateRenderer(ps->pwindow, -1, 0);
			ps->ptexture = SDL_CreateTexture(ps->prenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, ps->pixel_w, ps->pixel_h);
		}
		catch (...)
		{
			ps->prenderer = NULL;
			ps->ptexture = NULL;
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}

		if (ps->prenderer == NULL || ps->ptexture == NULL)
		{
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}

	}

	return 0;
}

int   CPlayerVideo::display_sizechange_2()
{
	if (ps->hSdlHwnd2)
	{

		if (ps->prenderer2)
		{
			SDL_DestroyRenderer(ps->prenderer2);
			ps->prenderer2 = NULL;
		}
		if (ps->ptexture2)
		{
			SDL_DestroyTexture(ps->ptexture2);
			ps->ptexture2 = NULL;
		}

		if (ps->pwindow2)
		{
			SDL_DestroyWindow(ps->pwindow2);
			ps->pwindow2 = NULL;
			ShowWindow(ps->hSdlHwnd2, 1);
		}


		ps->pwindow2 = SDL_CreateWindowFrom((void *)(ps->hSdlHwnd2));

		if (ps->pwindow2 == NULL)
		{
			PrintfDebug("Couldn't Create Window\n");
			return -1;
		}
		//新建一个渲染器
		try {
			ps->prenderer2 = SDL_CreateRenderer(ps->pwindow2, -1, 0);
			ps->ptexture2 = SDL_CreateTexture(ps->prenderer2, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, ps->pixel_w, ps->pixel_h);
		}
		catch (...)
		{
			ps->prenderer2 = NULL;
			ps->ptexture2 = NULL;
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}

		if (ps->prenderer2 == NULL || ps->ptexture2 == NULL)
		{
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}

	}

	return 0;
}

int CPlayerVideo::play_video()
{
	
	ps->pixel_w = ps->pvideo_codec_ctx->width;
	ps->pixel_h = ps->pvideo_codec_ctx->height;
	ps->window_w = ps->pixel_w;
	ps->window_h = ps->pixel_h;

	ps->pixfmt = AV_PIX_FMT_YUV420P;
	ps->out_frame.format = AV_PIX_FMT_YUV420P;
	ps->out_frame.width = ps->pixel_w;
	ps->out_frame.height = ps->pixel_h;

	//
	ps->video_buf = (uint8_t *)av_malloc(
		avpicture_get_size(ps->pixfmt,
			ps->out_frame.width, ps->out_frame.height)
	);

	//用av_image_fill_arrays代替。
	//根据所给参数和提供的数据设置data指针和linesizes。
	avpicture_fill((AVPicture *)&ps->out_frame, ps->video_buf,
		ps->pixfmt,
		ps->out_frame.width, ps->out_frame.height);

	//使用sws_scale之前要用这个函数进行相关转换操作。
	//分配和返回一个 SwsContext.
	//sws_freeContext(ps->psws_ctx); 需要用这个函数free内存。
	//现在因为只用了一次sws_getContext()所以，这个内存在main释放。
	//因为一直输出格式什么都一样，所以没有放在靠近sws_scale的地方。
	ps->psws_ctx = sws_getContext(ps->pixel_w,
		ps->pixel_h, ps->pvideo_codec_ctx->pix_fmt,
		ps->out_frame.width, ps->out_frame.height,
		ps->pixfmt,
		SWS_BILINEAR, NULL, NULL, NULL);

	ps->sdl_rect.x = 0;
	ps->sdl_rect.y = 0;

	//创建窗口
	//SDL_WINDOW_RESIZABLE: 使窗口可以拉伸
	// ps->pwindow = SDL_CreateWindow("Isshe Video Player!",
	//        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//        ps->window_w, ps->window_h,
	//        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (ps->hSdlHwnd)
	{
		
		ps->pwindow = SDL_CreateWindowFrom((void *)(ps->hSdlHwnd));
		if (ps->pwindow == NULL)
		{
			PrintfDebug("Couldn't Create Window\n");
			return -1;
		}
		//新建一个渲染器
		try {
			ps->prenderer = SDL_CreateRenderer(ps->pwindow, -1, 0);
			ps->ptexture = SDL_CreateTexture(ps->prenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, ps->pixel_w, ps->pixel_h);
		}
		catch (...) 
		{
			ps->prenderer = NULL;
			ps->ptexture= NULL;
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}
		
		if (ps->prenderer == NULL || ps->ptexture == NULL)
		{
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}
	
	}
	

	if (ps->hSdlHwnd2)
	{
		ps->pwindow2 = SDL_CreateWindowFrom((void *)(ps->hSdlHwnd2));
		if (ps->pwindow2 == NULL)
		{
			PrintfDebug("Couldn't Create Window\n");
			return -1;
		}
		//新建一个渲染器
		try {
			ps->prenderer2 = SDL_CreateRenderer(ps->pwindow2, -1, 0);
			ps->ptexture2 = SDL_CreateTexture(ps->prenderer2, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, ps->pixel_w, ps->pixel_h);
		}
		catch (...) {
			ps->prenderer2 = NULL;
			ps->ptexture2 = NULL;
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}
		if (ps->prenderer2 == NULL || ps->ptexture2 == NULL)
		{
			PrintfDebug("Couldn't Create Renderer Texture\n");
			return -1;
		}
	}

	//新建线程运行刷新函数
	SDL_CreateThread(main_refresh_fun, "refresh func", this);
	SDL_CreateThread(main_decode_show_fun, "decdoeshow func", this);

	ps->bRun = true;

	return 0;
}



int CPlayerVideo::refresh_fun()
{

	//SDL_Event event;
	while (ps->quit == 0)
	{
		//event.type = ISSHE_REFRESH_EVENT;
		//SDL_PushEvent(&event);
		

		SetEvent(hWaitHandle);

		::Sleep(ps->delay);
	}
	return 0;
}

int CPlayerVideo::decode_show_fun()
{
	while (ps->quit == 0)
	{
		WaitForSingleObject(hWaitHandle, 200);
		decode_and_show();
	}
	DEBUG_TRY
	if (ps->hSdlHwnd)
	{
		if (ps->prenderer)
		{
			SDL_DestroyRenderer(ps->prenderer);
			ps->prenderer = NULL;
		}
		if (ps->ptexture)
		{
			SDL_DestroyTexture(ps->ptexture);
			ps->ptexture = NULL;
		}
		if (ps->pwindow)
		{
			SDL_DestroyWindow(ps->pwindow);
			ps->pwindow = NULL;
			ShowWindow(ps->hSdlHwnd, 1);
		}
		
		ps->hSdlHwnd = 0;
	}
	

	if (ps->hSdlHwnd2)
	{
		if (ps->prenderer2)
		{
			SDL_DestroyRenderer(ps->prenderer2);
			ps->prenderer2 = NULL;
		}
		if (ps->ptexture2)
		{
			SDL_DestroyTexture(ps->ptexture2);
			ps->ptexture2 = NULL;
		}
		if (ps->pwindow2)
		{
			SDL_DestroyWindow(ps->pwindow2);
			ps->pwindow2 = NULL;
			ShowWindow(ps->hSdlHwnd2, 1);
		}
		
		ps->hSdlHwnd2 = 0;
	}
	return 0;
	DEBUG_CATCH2("%s", __FUNCTION__);

	ps->hSdlHwnd = 0;
	ps->hSdlHwnd2 = 0;
	return -1;
}



int CPlayerVideo::do_player_loop()
{
	if (ps->mReqPlayerCmd == PLAYER_CMD_NULL)
		return -1;

	switch (ps->mReqPlayerCmd)
	{
	case PLAYER_CMD_ATTACH_WIN:
	{
		if (ps->hSdlHwnd && (!ps->pwindow))
		{
			ps->pwindow = SDL_CreateWindowFrom((void *)(ps->hSdlHwnd));
			if (ps->pwindow == NULL)
			{
				fprintf(ERR_STREAM, "Couldn't Create Window\n");
				return -1;
			}
			//新建一个渲染器
			try {
				ps->prenderer = SDL_CreateRenderer(ps->pwindow, -1, 0);
				ps->ptexture = SDL_CreateTexture(ps->prenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, ps->pixel_w, ps->pixel_h);
			}
			catch (...) {
				ps->prenderer = NULL;
				ps->ptexture = NULL;
				PrintfDebug("Couldn't Create Renderer Texture\n");
				return -1;
			}
		}

	}
	break;
	case PLAYER_CMD_ATTACH_WIN2:
	{
		if (ps->hSdlHwnd2 && (!ps->pwindow2))
		{
			ps->pwindow2 = SDL_CreateWindowFrom((void *)(ps->hSdlHwnd2));
			if (ps->pwindow2 == NULL)
			{
				fprintf(ERR_STREAM, "Couldn't Create Window\n");
				return -1;
			}
			//新建一个渲染器
			try {
				ps->prenderer2 = SDL_CreateRenderer(ps->pwindow2, -1, 0);
				ps->ptexture2 = SDL_CreateTexture(ps->prenderer2, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, ps->pixel_w, ps->pixel_h);
			}
			catch (...) {
				ps->prenderer2 = NULL;
				ps->ptexture2 = NULL;
				PrintfDebug("Couldn't Create Renderer Texture\n");
				return -1;
			}
		}

	}
	break;
	case PLAYER_CMD_RELEASE_WIN:
	{
		try {
			if (ps->hSdlHwnd)
			{
				if (ps->prenderer)
				{
					SDL_DestroyRenderer(ps->prenderer);
					ps->prenderer = NULL;
				}
				if (ps->ptexture)
				{
					SDL_DestroyTexture(ps->ptexture);
					ps->ptexture = NULL;
				}
				if (ps->pwindow)
				{
					SDL_DestroyWindow(ps->pwindow);
					ps->pwindow = NULL;
					ShowWindow(ps->hSdlHwnd, 1);
				}
				ps->hSdlHwnd = 0;
			}
		}
		catch (...) {
			ps->hSdlHwnd = 0;
		}
	}
	break;

	case PLAYER_CMD_RELEASE_WIN2:
	{
		try {

			if (ps->hSdlHwnd2)
			{
				if (ps->prenderer2)
				{
					SDL_DestroyRenderer(ps->prenderer2);
					ps->prenderer2 = NULL;
				}
				if (ps->ptexture2)
				{
					SDL_DestroyTexture(ps->ptexture2);
					ps->ptexture2 = NULL;
				}
				if (ps->pwindow2)
				{
					SDL_DestroyWindow(ps->pwindow2);

					ps->pwindow2 = NULL;
					ShowWindow(ps->hSdlHwnd2, 1);
				}
				ps->hSdlHwnd2 = 0;
			}
		}
		catch (...) {
			ps->hSdlHwnd2 = 0;
		}
	}
	break;

	case PLAYER_CMD_CAPTURE_PIC:
	{
		if (ps->pFFMpegYUVConv)
		{
			int width = ps->pvideo_codec_ctx->width;
			int height = ps->pvideo_codec_ctx->height;

			unsigned char *pDataOut = NULL;
			pDataOut = new unsigned char[1920*1080 * 3];
			ps->pFFMpegYUVConv->ConverFmt420PToRgb24(&ps->out_frame, pDataOut);
			SaveBmp(pDataOut, width, height, ps->capturePath);
			SAFE_DELETE(pDataOut);
		}

	}
	break;

	}

	ps->mReqPlayerCmd = PLAYER_CMD_NULL;

	return 0;

}


int CPlayerVideo::decode_and_show()
{


	AVPacket packet;
	AVFrame *pframe = av_frame_alloc();
	double pts = 0.0;
	int ret = 0;


	// ...
	if (ps->quit == 1)
		return 0;

	if (ps->video_packet_queue.nb_packets == 0)
	{
		//PrintfDebug("nb_packets=0\n");
		return 0;
	}

	//从packet队列取一个packet出来解码
	ret = packet_queue_get(&ps->video_packet_queue, &packet, 1);
	if (ret < 0)
	{
		PrintfDebug("Get video packet error\n");
		return -1;     //
	}

	
	ret = avcodec_send_packet(ps->pvideo_codec_ctx, &packet);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
	{
		PrintfDebug("send video packet error\n");
		return -1;     //
	}

	if ( (ps->pvideo_codec_ctx->width != ps->mVideoWidth)  && (ps->pvideo_codec_ctx->height != ps->mVideoHeight))
	{
		ps->mVideoSizeChangeNums++;
		if (ps->mVideoSizeChangeNums > 10)
		{
			ps->bVideoResHasChange = true;
			ps->mVideoSizeChangeNums = -200;
		}
		return -1;
	}
	else if ((ps->pvideo_codec_ctx->width == ps->mVideoWidth) && (ps->pvideo_codec_ctx->height == ps->mVideoHeight))
	{
		ps->mVideoSizeChangeNums = 0;
	}

	ret = avcodec_receive_frame(ps->pvideo_codec_ctx, pframe);
	if (ret < 0 && ret != AVERROR_EOF)
	{
		PrintfDebug("receive video frame error\n");
		return -1;
	}

	
	
	av_packet_unref(&packet);
	av_init_packet(&packet);

	//下面三句实现音视频同步，还有一句在audio部分。
	//获取pts
	pts = get_frame_pts(pframe);

	//ps中用cur_frame_pts是为了减少get_delay()的参数
	ps->cur_frame_pts = pts; //*(double *)pframe.opaque;
	ps->delay = get_delay() * 1000 + 0.5;

	//PrintfDebug("video frame pts = %lf\n", pts);
	//PrintfDebug("ps->delay = %d\n", ps->delay);

	//这个函数看了手册也不大理解
	sws_scale(ps->psws_ctx, (uint8_t const *const *)pframe->data,
		pframe->linesize, 0, ps->pixel_h,
		ps->out_frame.data, ps->out_frame.linesize);

	ps->sdl_rect.w = ps->window_w;
	ps->sdl_rect.h = ps->window_h;

	if (ps->bDisplaySizeChanged1)
	{
		ps->bDisplaySizeChanged1 = false;
		display_sizechange_1();
	}

	if (ps->bDisplaySizeChanged2)
	{
		ps->bDisplaySizeChanged2 = false;
		display_sizechange_2();
	}


	if (ps->hSdlHwnd && ps->ptexture)
	{
		try {
			SDL_UpdateTexture(ps->ptexture, NULL, ps->out_frame.data[0], ps->out_frame.linesize[0]);
			SDL_RenderClear(ps->prenderer);
			SDL_RenderCopy(ps->prenderer, ps->ptexture, NULL, NULL);
			SDL_RenderPresent(ps->prenderer);
		}
		catch (...) {
			
		}
	}
	
	if (ps->hSdlHwnd2 && ps->ptexture2)
	{
		try {
			SDL_UpdateTexture(ps->ptexture2, NULL, ps->out_frame.data[0], ps->out_frame.linesize[0]);
			SDL_RenderClear(ps->prenderer2);
			SDL_RenderCopy(ps->prenderer2, ps->ptexture2, NULL, NULL);
			SDL_RenderPresent(ps->prenderer2);
		}
		catch (...) {

		}
	}

	do_player_loop();

	av_frame_free(&pframe);
	//av_frame_unref(pframe);
	
	return 0;
}

double CPlayerVideo::get_audio_clock()
{
	long long bytes_per_sec = 0;
	double cur_audio_clock = 0.0;
	double cur_buf_pos = ps->audio_buf_index;

	
	bytes_per_sec = ps->paudio_stream->codec->sample_rate
		* ps->paudio_codec_ctx->channels * 2;

	cur_audio_clock = ps->audio_clock +
		cur_buf_pos / (double)bytes_per_sec;

	return cur_audio_clock;
}

double CPlayerVideo::get_frame_pts(AVFrame *pframe)
{
	double pts = 0.0;
	double frame_delay = 0.0;

	pts = av_frame_get_best_effort_timestamp(pframe);
	if (pts == AV_NOPTS_VALUE) 		//???
	{
		pts = 0;
	}

	pts *= av_q2d(ps->pvideo_stream->time_base);

	if (pts != 0)
	{
		ps->video_clock = pts; 		//video_clock貌似没有什么实际用处
	}
	else
	{
		pts = ps->video_clock;
	}

	//更新video_clock, 这里不理解
	//这里用的是AVCodecContext的time_base
	//extra_delay = repeat_pict / (2*fps), 这个公式是在ffmpeg官网手册看的
	frame_delay = av_q2d(ps->pvideo_stream->codec->time_base);
	frame_delay += pframe->repeat_pict / (frame_delay * 2);
	ps->video_clock += frame_delay;

	return pts;
}

double CPlayerVideo::get_delay()
{
	double 		ret_delay = 0.0;
	double 		frame_delay = 0.0;
	double 		cur_audio_clock = 0.0;
	double 		compare = 0.0;
	double  	threshold = 0.0;

	if (ps->audio_stream_index < 0 || (!ps->bAudioOpenOk))
	{
		ret_delay = 0.020;
		return ret_delay;
	}



	//这里的delay是秒为单位， 化为毫秒：*1000
	frame_delay = ps->cur_frame_pts - ps->pre_frame_pts;
	if (frame_delay <= 0 || frame_delay >= 1.0)
	{
		frame_delay = ps->pre_cur_frame_delay;
	}
	//两帧之间的延时
	ps->pre_cur_frame_delay = frame_delay;
	ps->pre_frame_pts = ps->cur_frame_pts;


//	if (ps->audio_stream_index < 0)
//		return frame_delay;

	
	cur_audio_clock = get_audio_clock();

	//compare < 0 说明慢了， > 0说明快了
	compare = ps->cur_frame_pts - cur_audio_clock;

	//设置一个阀值, 是一个正数
	//这里设阀值为两帧之间的延迟，
	threshold = frame_delay;
	//SYNC_THRESHOLD ? frame_delay : SYNC_THRESHOLD;


	if (compare <= -threshold) 		//慢， 加快速度
	{
		ret_delay = frame_delay / 2;
	}
	else if (compare >= threshold) 	//快了，就在上一帧延时的基础上加长延时
	{
		ret_delay = frame_delay * 2;
	}
	else
	{
		//
		ret_delay = frame_delay;//frame_delay;
	}
	

	/*	//这部分可能用了更准确之类的，但是这次没用，也没测试过
	ps->frame_timer += ret_delay/1000; 	//考虑要不要,注意这里是秒单位
	int64_t cur_time = av_gettime()/1000000; 	//av_gettime()返回微秒
	double real_delay = ps->frame_timer - cur_time;
	if (real_delay <= 0.010)
	{
	read_delay = 0.010;
	}
	ret_delay = actual_delay * 1000 + 0.5;
	*/
	return ret_delay;
}