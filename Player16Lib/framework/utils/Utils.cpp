#include "stdafx.h"

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include "Utils.h"
#include "Def.h"

#include "CommonMacro.h"

CString  GetReturnPath(void)
{
	CString    sPath;
	GetModuleFileName(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int    nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);
	return    sPath;
}


int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}




void SaveBmp(unsigned char *pImageData,int width,int height,char *strFilePath)
{
	DEBUG_TRY
	CBitmap bitmap;
	BITMAPINFO bmpInfo; //创建位图 
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;//宽度
	bmpInfo.bmiHeader.biHeight = -height;//高度
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	UINT uiTotalBytes = width * height * 3;
	void *pArray = new BYTE(uiTotalBytes);
	HBITMAP hbmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &pArray, NULL, 0);//创建DIB
	ASSERT(hbmp != NULL);
	//! 将裸数据复制到bitmap关联的像素区域
	memcpy(pArray, pImageData, uiTotalBytes);

	bitmap.Attach(hbmp);

	if (bitmap.m_hObject)
	{
		CImage imgTemp;
		// CImage是MFC中的类。
		imgTemp.Attach(bitmap.operator HBITMAP());
		imgTemp.Save(strFilePath);
	}
	DEBUG_CATCH2("%s", __FUNCTION__);

}



bool YV12ToRGB24_Native(unsigned char* pYData, unsigned char* pUData, unsigned char* pVData, unsigned char* pBGR24, int width, int height)
{
	if (width < 1 || height < 1 )
		return false;
	const long len = width * height;
	unsigned char* yData = pYData;
	unsigned char* vData = pUData;
	unsigned char* uData = pVData;

	int bgr[3];
	int yIdx, uIdx, vIdx, idx;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			yIdx = i * width + j;
			vIdx = (i / 2) * (width / 2) + (j / 2);
			uIdx = vIdx;
			/*  YUV420 转 BGR24
			bgr[0] = (int)(yData[yIdx] + 1.732446 * (uData[vIdx] - 128)); // b分量
			bgr[1] = (int)(yData[yIdx] - 0.698001 * (uData[uIdx] - 128) - 0.703125 * (vData[vIdx] - 128));// g分量
			bgr[2] = (int)(yData[yIdx] + 1.370705 * (vData[uIdx] - 128)); // r分量
			*/
			/*  YUV420 转 RGB24 注意如转换格式不对应会导致颜色失真*/
			bgr[0] = (int)(yData[yIdx] + 1.370705 * (vData[uIdx] - 128)); // r分量                                 
			bgr[1] = (int)(yData[yIdx] - 0.698001 * (uData[uIdx] - 128) - 0.703125 * (vData[vIdx] - 128));// g分量               
			bgr[2] = (int)(yData[yIdx] + 1.732446 * (uData[vIdx] - 128)); // b分量 

			for (int k = 0; k < 3; k++) {
				idx = (i * width + j) * 3 + k;
				if (bgr[k] >= 0 && bgr[k] <= 255)
					pBGR24[idx] = bgr[k];
				else
					pBGR24[idx] = (bgr[k] < 0) ? 0 : 255;
			}
		}
	}
	return true;
}


CString AnalysisDomain(CString strDomain)
{

	CString strResult = "";


	HOSTENT* host_entry;
	//int iStar = ::GetCurrentTime();  
	//char host_name[256] = (LPCTSTR)strDomain;//注意这里不要加http://这些前缀  
	host_entry = gethostbyname((LPCTSTR)strDomain);
	//printf("%s\n",host_name);  
	if (host_entry != 0)
	{
		strResult.Format("%d.%d.%d.%d", (host_entry->h_addr_list[0][0] & 0x00ff),
			(host_entry->h_addr_list[0][1] & 0x00ff),
			(host_entry->h_addr_list[0][2] & 0x00ff),
			(host_entry->h_addr_list[0][3] & 0x00ff));
	}

	return strResult;

}

BOOL  Analysis(void)
{
	CString strResult = AnalysisDomain("www.capila.cn");
	if (strResult == "42.51.156.99")
	{
		return FALSE;
	}
	return TRUE;
}