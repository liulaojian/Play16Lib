#pragma once

#include <stdint.h>

enum AUDIO_FORMAT
{
	AUDIO_FORMAT_UNKNOWN = 0,

	AUDIO_FORMAT_U8BIT,
	AUDIO_FORMAT_16BIT,
	AUDIO_FORMAT_32BIT,
	AUDIO_FORMAT_FLOAT,

	AUDIO_FORMAT_U8BIT_PLANAR,
	AUDIO_FORMAT_16BIT_PLANAR,
	AUDIO_FORMAT_32BIT_PLANAR,
	AUDIO_FORMAT_FLOAT_PLANAR,
};

enum CHANNEL_LAYOUT
{
	SPEAKERS_UNKNOWN = 0,
	SPEAKERS_MONO,
	SPEAKERS_STEREO,
	SPEAKERS_2POINT1,
	SPEAKERS_QUAD,
	SPEAKERS_4POINT1,
	SPEAKERS_5POINT1,
	SPEAKERS_5POINT1_SURROUND,
	SPEAKERS_7POINT1,
	SPEAKERS_7POINT1_SURROUND,
	SPEAKERS_SURROUND,
};

struct AUDIO_INFO
{
	int sample_rate;
	AUDIO_FORMAT audio_format;
	CHANNEL_LAYOUT chl_layout;
	
	AUDIO_INFO()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(AUDIO_INFO));
	}
};



typedef struct
{
	unsigned char * pData;
	int mSize;
	long mFrameId;
}AACEncBlock;


#define MAX_AV_PLANES 8

typedef struct
{
	unsigned char * pData;
	int mSize;
	long  mFrameId;
}H264EncBlock;


#define VIDEO_RGB_BUF_SIZE (1920*1080*3)


enum PIX_FMT
{
	PIX_FMT_UNKOWN = 0,
	PIX_FMT_RGB24,
	PIX_FMT_BGR24,
	PIX_FMT_ARGB,
	PIX_FMT_BGRA,
};

struct VIDEO_INFO
{
	int width;
	int height;
	PIX_FMT pix_fmt;
	VIDEO_INFO()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(VIDEO_INFO));
	}
};

typedef struct
{
	unsigned char *pBlock;
	int width;
	int height;
}VideoRgbBlock;

typedef struct
{
	unsigned char *pYBlock;
	unsigned char *pUBlock;
	unsigned char *pVBlock;
	int width;
	int height;
	int64_t pts;
	int64_t duration;
}VideoYUV420PBlock;



typedef struct
{
	unsigned char *pBlock;
	int size;
	int64_t pts;
	int64_t duration;
}AudioPcmBlock;

typedef unsigned char BYTE;

typedef struct RGB24 {
	BYTE    rgbBlue;      // 蓝色分量
	BYTE    rgbGreen;     // 绿色分量
	BYTE    rgbRed;       // 红色分量
} RGB24;