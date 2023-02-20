#pragma once

enum GLOBAL_ERROR
{
	ERROR_NULL=0,
	ERROR_FILE_NO_FOUND,
	ERROR_FILE_HAS_PLAYING,
	ERROR_AUDIO_HAS_PLAYING,
	ERROR_AUDIO_PLAY_INIT_FAIL,
	ERROR_MIC_DEVICE_NONE,
	ERROR_MIC_RECODERING,
	ERROR_VIDEO_AUDIO_FMT_NO_SUPPORT,
	ERROR_FILE_OPEN_ERROR,
	ERROR_FIND_STREAM_ERROR,
	ERROR_UNKNOWN,
};

class CGlobalError
{
public:
	CGlobalError();
	virtual ~CGlobalError();

	static CGlobalError* GetInstance();

public:
	GLOBAL_ERROR GetCurError(void) { return curError; }
	char * GetCurErrorString(void) { return cur_error; }

	void SetCurError(GLOBAL_ERROR mCurErr);

	char *GetErrorString(GLOBAL_ERROR mErr);

	
private:
	static CGlobalError* s_pIntance;
	GLOBAL_ERROR curError;
	char cur_error[512];
};


#define GlobalError()	CGlobalError::GetInstance()