#include "stdafx.h"
#include "PlayerAudio.h"
#include "PlayerMain.h"
#include "PlayerMgr.h"
#include "Log.h"
#include "CommonMacro.h"
CPlayerAudio::CPlayerAudio(CPlayerMain *pMain)
{
	pPlayerMain = pMain;
	if (pPlayerMain)
	{
		ps = pPlayerMain->GetPlayerState();
	}
	bPause = false;
}


CPlayerAudio::~CPlayerAudio()
{


}


int CPlayerAudio::prepare_audio()
{
	ps->paudio_stream = ps->pformat_ctx->streams[ps->audio_stream_index];
	ps->paudio_codec_ctx = ps->paudio_stream->codec;
	ps->paudio_codec = avcodec_find_decoder(ps->paudio_codec_ctx->codec_id);
	
	if (ps->paudio_codec == NULL)
	{
		PrintfDebug("Couldn't find audio decoder!!!!!!!\n");
		return -1;
	}
	//初始化AVCondecContext，以及进行一些处理工作。
	if (avcodec_open2(ps->paudio_codec_ctx, ps->paudio_codec, NULL) < 0)
	{
		PrintfDebug("avcodec_open2 audio error!!!!!!!\n");
		return -1;
	}
	m_play_channels = ps->paudio_stream->codec->channels;
	return 0;
}

void  main_audio_callback(void *userdata, uint8_t *stream, int len)
{
	CPlayerAudio *pPlayerAudio = (CPlayerAudio *)userdata;
	if (pPlayerAudio)
		pPlayerAudio->audio_callback(stream, len);
}

void CPlayerAudio::audio_callback(uint8_t *stream, int len)
{
	int 		send_data_size = 0;
	int 		audio_size = 0;

	if (ps->quit == 1)
	{
		return;
	}

	if (!ps->bPlaying)
	{
		return;
	}

	//SDL_memset(stream, 0, len);
	memset(stream, 0, len);

	while (len > 0)
	{
		if (ps->audio_buf_index >= ps->audio_buf_size)
		{
			//数据已经全部发送，再去取
			audio_size = audio_decode_frame();

			if (audio_size < 0)
			{
				//错误则静音一下
				ps->audio_buf_size = 1024;
				memset(ps->audio_buf, 0, ps->audio_buf_size); //
			}
			else
			{
				ps->audio_buf_size = audio_size;       //解码这么多
			}
			//回到缓冲区头部
			ps->audio_buf_index = 0;
		}

		send_data_size = ps->audio_buf_size - ps->audio_buf_index;
		if (len < send_data_size)
		{
			send_data_size = len;
		}

		//第2个参数也有写len的，不过觉得发实际的对一些吧？
		//再看
		if (ps->bEnableAudioPlay)
		{
			SDL_MixAudio(stream,
				(uint8_t *)ps->audio_buf + ps->audio_buf_index,
				send_data_size, SDL_MIX_MAXVOLUME);
		}
		

		len -= send_data_size;
		stream += send_data_size;
		ps->audio_buf_index += send_data_size;
	}
}

int main_play_auido(void *arg)
{

	CPlayerAudio *pPlayerAudio = (CPlayerAudio*)arg;

	if (pPlayerAudio)
	{
		pPlayerAudio->play_audio_real();
	}

	return 0;
}

int CPlayerAudio::play_audio()
{
	SDL_CreateThread(main_play_auido, "play audio func", this);
	return 0;
}

int CPlayerAudio::play_audio_real()
{
	
	SDL_AudioSpec have;
	SDL_AudioSpec      wanted_spec;
	//    SDL_AudioSpec      spec;
	wanted_spec.freq = ps->paudio_codec_ctx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = m_play_channels;		//ps->paudio_codec_ctx->channels		// 
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;     //
	wanted_spec.callback = main_audio_callback;
	wanted_spec.userdata = this; // ps->paudio_codec_ctx;

	CoInitialize(NULL);
							   //打开音频设备
							   //运行callback
	int ret;
	if (PlayerMgr()->IsMainAudioOpened())
	{
		try
		{
			ps->dev = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
			if (ps->dev == 0) {
				PrintfDebug("Failed to open audio: %s", SDL_GetError());
				ps->bAudioOpenOk = false;
				return -1;
			}
			SDL_PauseAudioDevice(ps->dev, 0);
			ps->bAudioOpenOk = true;
			ps->bMainAudio = false;
		}
		catch (...)
		{
			ps->bAudioOpenOk = false;
			return -1;
		}
	}
	else
	{
		try
		{
			ret = SDL_OpenAudio(&wanted_spec, NULL);
			if (ret < 0)
			{
				PrintfDebug("Failed to open audio: %s", SDL_GetError());
				return -1;
			}
			ps->dev = ret;
			SDL_PauseAudio(0);
			ps->bMainAudio = true;
			ps->bAudioOpenOk = true;
		}
		catch (...)
		{
			ps->bAudioOpenOk = false;
			return -1;
		}
		
	}
	

	while (ps->quit == 0)
	{

		::Sleep(100);
	}
	
	return 0;
	
}



int  CPlayerAudio::audio_decode_frame()
{
	uint8_t 			*audio_buf = ps->audio_buf;
	AVPacket           packet;
	AVFrame            *pframe;
	AVSampleFormat     dst_sample_fmt;
	uint64_t           dst_channel_layout;
	uint64_t           dst_nb_samples;
	int                convert_len;
	SwrContext 		*swr_ctx = NULL;
	int                data_size;
	int 				ret = 0;


	
	if (ps->quit == 1)
		return -1;

	pframe = av_frame_alloc();

	if (packet_queue_get(&ps->audio_packet_queue, &packet, 1) < 0)
	{
		PrintfDebug("Get queue packet error\n");
		return -1;
	}
	//获取音频当前时间
	if (packet.pts != AV_NOPTS_VALUE)
	{
		ps->audio_clock = packet.pts * av_q2d(ps->paudio_stream->time_base);
	}

	ret = avcodec_send_packet(ps->paudio_codec_ctx, &packet);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
	{
		PrintfDebug("send decode packet error\n");
		return -1;
	}

	ret = avcodec_receive_frame(ps->paudio_codec_ctx, pframe);
	if (ret < 0 && ret != AVERROR_EOF)
	{
		PrintfDebug("receive decode frame error\n");
		return -1;
	}

	if (packet.pts != AV_NOPTS_VALUE)
	{
		ps->audio_clock = packet.pts
			*av_q2d(ps->paudio_stream->time_base);
	//	PrintfDebug("ps->audio_clock = %lf\n", ps->audio_clock);
	}

	//设置通道数和channel_layout
	if (pframe->channels > 0 && pframe->channel_layout == 0)
	{
		pframe->channel_layout = av_get_default_channel_layout(pframe->channels);
	}
	else if (pframe->channels == 0 && pframe->channel_layout > 0)
	{
		pframe->channels = av_get_channel_layout_nb_channels(pframe->channel_layout);
	}
	av_packet_unref(&packet);
	av_init_packet(&packet);

	//以下设置转换参数并转换,设置参数应该只设置一次就可以了，再实验
	dst_sample_fmt = AV_SAMPLE_FMT_S16;
	dst_channel_layout = av_get_default_channel_layout(m_play_channels);	 //  	  pframe->channels
	//设置转换参数
	swr_ctx = swr_alloc_set_opts(NULL, dst_channel_layout, dst_sample_fmt,
		pframe->sample_rate, pframe->channel_layout,
		(AVSampleFormat)pframe->format, pframe->sample_rate, 0, NULL);
	if (swr_ctx == NULL || swr_init(swr_ctx) < 0)
	{
		PrintfDebug("swr set open or swr init error\n");
		return -1;
	}

	//计算转换后的sample个数 a * b / c， 使用这个函数是为了不overflow
	//AVRounding 舍入
	//也可以dst_nb_samples = 192000代替。
	dst_nb_samples = av_rescale_rnd(
		swr_get_delay(swr_ctx, pframe->sample_rate) + pframe->nb_samples,
		pframe->sample_rate, pframe->sample_rate, AVRounding(1));
	
	//前面几个都是为了这个函数的参数。
	convert_len = swr_convert(swr_ctx, &audio_buf, dst_nb_samples,
		(const uint8_t **)pframe->data, pframe->nb_samples);

	data_size = convert_len * m_play_channels * av_get_bytes_per_sample(dst_sample_fmt);		//   pframe->channels

	av_frame_free(&pframe);
	swr_free(&swr_ctx);            //

	return data_size;
	
}