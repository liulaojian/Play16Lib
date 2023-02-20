#include "stdafx.h"
#include "GlobalError.h"
#include "CommonMacro.h"

CGlobalError* CGlobalError::s_pIntance = NULL;
CGlobalError::CGlobalError()
{
	curError = ERROR_NULL;
	
}

CGlobalError::~CGlobalError()
{

}


CGlobalError* CGlobalError::GetInstance()
{
	if (s_pIntance == NULL)
	{
		s_pIntance = new CGlobalError();
		CHECKF(s_pIntance);
	}

	return s_pIntance;
}

void CGlobalError::SetCurError(GLOBAL_ERROR mCurErr)
{
	char *error = GetErrorString(mCurErr);
	memset(cur_error, 0, 512);
	strcpy(cur_error, error);
	curError = mCurErr;
	
}

char * CGlobalError::GetErrorString(GLOBAL_ERROR mErr)
{
	switch (mErr)
	{
	case ERROR_NULL:
		return "no error";

	case ERROR_FILE_NO_FOUND:
		return "文件未找到!";

	case ERROR_FILE_HAS_PLAYING:

		return "已有视频文件在广播!";

	case ERROR_AUDIO_HAS_PLAYING:
		return "已有音频文件在广播!";

	case ERROR_AUDIO_PLAY_INIT_FAIL:
		return "音频播放初始化失败!";

	case ERROR_MIC_DEVICE_NONE:
		return "麦未插入或者音频驱动问题!";

	case ERROR_MIC_RECODERING:
		return "已有录音在运行!";

	case ERROR_VIDEO_AUDIO_FMT_NO_SUPPORT:

		return "音视频格式不支持，音频支持mp3 aac 视频支持h264 mpeg4";

	case ERROR_FILE_OPEN_ERROR:

		return "打开文件错误!";

	case ERROR_FIND_STREAM_ERROR:

		return "查找Stream错误!";


	case ERROR_UNKNOWN:
		return "未知错误!";

	default:

		return "error unknow";
	}

}