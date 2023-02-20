#include "stdafx.h"

#include "FFMpegYUVConv.h"
#include "CommonMacro.h"

CFFMpegYUVConv* CFFMpegYUVConv::s_pIntance = NULL;

CFFMpegYUVConv::CFFMpegYUVConv()
{
	bInited = false;

	src_w = 0;
	src_h = 0;
	
	dst_w = 0;
	dst_h = 0;

}

CFFMpegYUVConv::~CFFMpegYUVConv()
{

}

CFFMpegYUVConv* CFFMpegYUVConv::GetInstance()
{
	if (s_pIntance == NULL)
	{
		s_pIntance = new CFFMpegYUVConv();
		CHECKF(s_pIntance);
	}

	return s_pIntance;
}

int CFFMpegYUVConv::Init(int srcWidth,int srcHeight,int dstWidth, int dstHeight)
{
	if (bInited)
		return -1;
	bInited = false;
	src_w = srcWidth;//pYUVFrame->width;
	src_h = srcHeight;	// pYUVFrame->height;
	src_pixfmt = AV_PIX_FMT_YUV420P;

	src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));

	dst_w = dstWidth;
	dst_h = dstHeight;

	dst_pixfmt = AV_PIX_FMT_RGB24;
	dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));

	rescale_method = SWS_BICUBIC;
	

	//int frame_idx = 0;
	int ret = 0;
	
	ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);

	if (ret < 0) {
		printf("Could not allocate source image\n");
		return -1;
	}
	
	ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
	if (ret < 0) {
		printf("Could not allocate destination image\n");
		return -1;
	}

	//Init Method 1
	img_convert_ctx = sws_alloc_context();
	//Show AVOption
	//av_opt_show2(img_convert_ctx, stdout, AV_OPT_FLAG_VIDEO_PARAM, 0);
	//Set Value
	av_opt_set_int(img_convert_ctx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
	av_opt_set_int(img_convert_ctx, "srcw", src_w, 0);
	av_opt_set_int(img_convert_ctx, "srch", src_h, 0);
	av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
	//'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
	av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
	av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
	av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
	av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
	av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
	sws_init_context(img_convert_ctx, NULL, NULL);

	bInited = true;

	return 0;

}


int CFFMpegYUVConv::ConverFmt420PToRgb24(AVFrame *pYUVFrame, unsigned char *pDataOut)
{
	memcpy(src_data[0], pYUVFrame->data[0], src_w*src_h);                    //Y
	memcpy(src_data[1], pYUVFrame->data[2], src_w*src_h / 4);      //U
	memcpy(src_data[2], pYUVFrame->data[1], src_w*src_h / 4);  //V

	sws_scale(img_convert_ctx, src_data, src_linesize, 0, src_h, dst_data, dst_linesize);

	memcpy(pDataOut, dst_data[0], dst_w*dst_h * 3);
	return 0;
}


int CFFMpegYUVConv::Close()
{
	if (!bInited)
		return -1;
	DEBUG_TRY
	if (img_convert_ctx)
	{
		sws_freeContext(img_convert_ctx);
	}

	if (src_data)
	{
		av_freep(&src_data[0]);
	}
	av_freep(&src_data);
	bInited = false;
	return 0;
	DEBUG_CATCH2("%s", __FUNCTION__);
	return -1;
}