#pragma once

#include "FFMpegCore.h"
#include "Lock.h"


class CFFMpegYUVConv
{
public:
	CFFMpegYUVConv();
	virtual ~CFFMpegYUVConv();

public:
	static CFFMpegYUVConv* GetInstance();

	int Init(int srcWidth, int srcHeight, int dstWidth, int dstHeight);

	int ConverFmt420PToRgb24(AVFrame *pYUVFrame, unsigned char *pDataOut);

	int Close();

	


private:
	static CFFMpegYUVConv* s_pIntance;

	int src_w;
	int src_h;
	AVPixelFormat src_pixfmt;
	int src_bpp ;

	int dst_w;
	int dst_h;

	AVPixelFormat dst_pixfmt;

	int dst_bpp;
	
	uint8_t *src_data[4];
	int src_linesize[4];

	uint8_t *dst_data[4];
	int dst_linesize[4];

	int rescale_method;
	struct SwsContext *img_convert_ctx;
	
	bool bInited;
};

#define FFMpegYUVConv()	CFFMpegYUVConv::GetInstance()