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
		return "�ļ�δ�ҵ�!";

	case ERROR_FILE_HAS_PLAYING:

		return "������Ƶ�ļ��ڹ㲥!";

	case ERROR_AUDIO_HAS_PLAYING:
		return "������Ƶ�ļ��ڹ㲥!";

	case ERROR_AUDIO_PLAY_INIT_FAIL:
		return "��Ƶ���ų�ʼ��ʧ��!";

	case ERROR_MIC_DEVICE_NONE:
		return "��δ���������Ƶ��������!";

	case ERROR_MIC_RECODERING:
		return "����¼��������!";

	case ERROR_VIDEO_AUDIO_FMT_NO_SUPPORT:

		return "����Ƶ��ʽ��֧�֣���Ƶ֧��mp3 aac ��Ƶ֧��h264 mpeg4";

	case ERROR_FILE_OPEN_ERROR:

		return "���ļ�����!";

	case ERROR_FIND_STREAM_ERROR:

		return "����Stream����!";


	case ERROR_UNKNOWN:
		return "δ֪����!";

	default:

		return "error unknow";
	}

}