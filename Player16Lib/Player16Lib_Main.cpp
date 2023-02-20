#include "stdafx.h"

#include "Player16Lib_Main.h"

#include "PlayerMain.h"
#include "PlayerMgr.h"

#include   <io.h>
#include   <fcntl.h>
static bool bAllocConsole = false;
void   OpenConsole()
{
	AllocConsole();
	HANDLE   handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int   hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE   *   hf = _fdopen(hCrt, "w");
	*stdout = *hf;
	bAllocConsole = true;
}

extern "C" __declspec(dllexport) int Play16_Begin(int mId, char *Url, HWND hSdlWnd, HWND hSdlWnd2, bool bOpenAudio)
{
	CPlayerMain *pPlayerMain;

	//OpenConsole();

	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);

	if (pPlayerMain)
	{
		if (pPlayerMain->IsRuning())
			return -1;
	}
	else
	{
		pPlayerMain = PlayerMgr()->NewPlayerMain(mId);
	}
	return pPlayerMain->play_main(Url, hSdlWnd, hSdlWnd2,bOpenAudio);
	
}



extern "C" __declspec(dllexport) int Play16_Stop(int mId)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->play_stop();
	}

	return -1;
}



extern "C" __declspec(dllexport) int Play16_AudioPlayEnable(int mId,bool bPlayAudio)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->enable_audioplay(bPlayAudio);
	}

	return -1;

}


extern "C" __declspec(dllexport) int Play16_AttchWin(int mId, HWND hSdlWnd)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->play_attch_win(hSdlWnd);
	}

	return -1;
}

extern "C" __declspec(dllexport) int Play16_AttchWin2(int mId, HWND hSdlWnd)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->play_attch_win2(hSdlWnd);
	}

	return -1;
}

extern "C" __declspec(dllexport) int Play16_ReleaseWin(int mId)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->play_release_win();
	}

	return -1;
}

extern "C" __declspec(dllexport) int Play16_ReleaseWin2(int mId)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->play_release_win2();
	}

	return -1;
}



extern "C" __declspec(dllexport) bool Play16_IsCommandBusy(int mId)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->is_play_command_busy();
	}

	return false;

}



extern "C" __declspec(dllexport) int Play16_CapturePic(int mId, char *path)
{

	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		return pPlayerMain->play_capture_pic(path);
	}

	return -1;

}



extern "C" __declspec(dllexport) int Play16_SetDisplaySizeChange_1(int mId, HWND mHwnd)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		pPlayerMain->set_displaysize_changed_1();
	}

	return -1;
}


extern "C" __declspec(dllexport) int Play16_SetDisplaySizeChange_2(int mId, HWND mHwnd)
{
	CPlayerMain *pPlayerMain;
	pPlayerMain = PlayerMgr()->GetPlayerMainById(mId);
	if (pPlayerMain)
	{
		pPlayerMain->set_displaysize_changed_2();
	}

	return -1;
}