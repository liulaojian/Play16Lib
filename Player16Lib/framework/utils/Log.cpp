#include "stdafx.h"
#include "Log.h"
#include "Utils.h"


CStdioFile fileDebug;
BOOL bFileDebugOpen = FALSE;


void LogFileOpen(void)
{
	CString strPath;
	strPath = GetReturnPath();
	strPath += "\\DebugLog.txt";
	fileDebug.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
	bFileDebugOpen = TRUE;
}


void LogFileClose(void)
{
	bFileDebugOpen = FALSE;
	fileDebug.Close();
}


void PrintfDebug(const char *format, ...)
{
#if 0
	va_list ap;
	va_start(ap, format);
	PrintfInfo(format, ap);
	va_end(ap);
#endif
}

void PrintfInfo(const char *format,va_list argp)
{
	char buf[1024];
	vsnprintf(buf, 1024, format, argp);
	//if (bFileDebugOpen)
	//	fileDebug.WriteString((LPCTSTR)buf);
	printf("%s", buf);
}