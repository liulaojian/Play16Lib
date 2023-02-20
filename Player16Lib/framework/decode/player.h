
#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/audio_fifo.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include <libavutil/time.h>
#include <SDL2/SDL.h>


#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

#include "packet_queue.h"

#define MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE   1024    //

#define FILE_NAME              "rtsp://192.168.1.168:8554/1"	// "rtsp://192.168.0.132:8554/1"	//"naxienian.mp4"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100
#define OUT_STREAM              stdout
#define WINDOW_W                640
#define WINDOW_H                320
#define ISSHE_REFRESH_EVENT 			(SDL_USEREVENT + 1)
#define BREAK_EVENT   			(SDL_USEREVENT + 2)
#define MAX_AUDIO_QUEUE_SIZE 	512
#define MAX_VIDEO_QUEUE_SIZE 	256


enum PlayerCmd
{
	PLAYER_CMD_NULL = 0,
	PLAYER_CMD_ATTACH_WIN,
	PLAYER_CMD_ATTACH_WIN2,
	PLAYER_CMD_RELEASE_WIN,
	PLAYER_CMD_RELEASE_WIN2,
	PLAYER_CMD_CAPTURE_PIC,
};

class CFFMpegYUVConv;
//这是一个统筹的结构
typedef struct PlayerState
{
     //公共
     AVFormatContext    *pformat_ctx;
     char               filename[1024];
     int                quit;
/*
     SDL_Thread         *audio_decode_tid;
     SDL_Thread         *audio_tid;
     SDL_Thread         *video_decode_tid;
     SDL_Thread         *video_tid;
*/
     //音频
     int                audio_stream_index;
     AVStream           *paudio_stream;
     AVCodecContext     *paudio_codec_ctx;
     AVCodec            *paudio_codec;
     PacketQueue        audio_packet_queue;
     uint8_t            audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
     unsigned int       audio_buf_size;
     unsigned int       audio_buf_index;

     //视频
     int                video_stream_index;
     AVStream           *pvideo_stream;
     AVCodecContext     *pvideo_codec_ctx;
     AVCodec            *pvideo_codec;
     PacketQueue        video_packet_queue;
     uint8_t            *video_buf;
     unsigned int       video_buf_size;
     unsigned int       video_buf_index;
     struct SwsContext  *psws_ctx;

     int                pixel_w;
     int                pixel_h;
     int                window_w;
     int                window_h;

     SDL_Window         *pwindow;
     SDL_Renderer       *prenderer;
     SDL_Texture        *ptexture;

	 SDL_Window         *pwindow2;
	 SDL_Renderer       *prenderer2;
	 SDL_Texture        *ptexture2;

     SDL_Rect           sdl_rect;
     AVPixelFormat      pixfmt;
//     AVFrame            frame;
     AVFrame            out_frame;
	 SDL_AudioDeviceID  dev;

     //同步相关
     double             audio_clock;
     double             video_clock;
     double 			pre_frame_pts; 			//前一帧显示时间
     double 			cur_frame_pts; 			//packet.pts
     double 			pre_cur_frame_delay; 	//当前帧和前一帧的延时，前面两个相减的结果
     uint32_t			delay;
     double             frame_timer;
	 HWND				hSdlHwnd;
	 HWND				hSdlHwnd2;
	 int                mReqPlayerCmd;
	 int                play_id;
	 bool				bRun;
	 bool				bPlaying;
	 bool				bMainAudio;
	 bool               bEnableAudioPlay;
	 CFFMpegYUVConv *pFFMpegYUVConv;
	 char				capturePath[512];
	 bool               bAudioOpenOk;
	 bool               bCloseing;
	 bool               bPlayReadying;

	 int                mVideoWidth;
	 int                mVideoHeight;
	 int				mVideoSizeChangeNums;

	 bool               bVideoResHasChange;

	 HWND				hBackSdlHwnd;
	 HWND				hBackSdlHwnd2;
	 bool               bDisplaySizeChanged1;
	 bool               bDisplaySizeChanged2;

}PlayerState;


void player_state_init(PlayerState *ps);

int decode_thread(void *arg);

int prepare_common(PlayerState *ps);

#ifdef __cplusplus
}
#endif




