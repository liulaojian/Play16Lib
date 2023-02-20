#pragma once

extern "C" __declspec(dllexport) int Play16_Begin(int mId,char *Url, HWND hSdlWnd, HWND hSdlWnd2, bool bOpenAudio);

extern "C" __declspec(dllexport) int Play16_Stop(int mId);

extern "C" __declspec(dllexport) int Play16_AudioPlayEnable(int mId, bool bPlayAudio);

extern "C" __declspec(dllexport) int Play16_AttchWin(int mId, HWND hSdlWnd);

extern "C" __declspec(dllexport) int Play16_AttchWin2(int mId, HWND hSdlWnd);

extern "C" __declspec(dllexport) int Play16_ReleaseWin(int mId);

extern "C" __declspec(dllexport) int Play16_ReleaseWin2(int mId);

extern "C" __declspec(dllexport) bool Play16_IsCommandBusy(int mId);

extern "C" __declspec(dllexport) int Play16_CapturePic(int mId,char *path);

extern "C" __declspec(dllexport) int Play16_SetDisplaySizeChange_1(int mId, HWND mHwnd);


extern "C" __declspec(dllexport) int Play16_SetDisplaySizeChange_2(int mId, HWND mHwnd);
