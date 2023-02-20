#pragma once

CString  GetReturnPath(void);
int gettimeofday(struct timeval *tp, void *tzp);
CString AnalysisDomain(CString strDomain);
BOOL  Analysis(void);
void SaveBmp(unsigned char *pImageData, int width, int height, char *strFilePath);
bool YV12ToRGB24_Native(unsigned char* pYData, unsigned char* pUData, unsigned char* pVData, unsigned char* pBGR24, int width, int height);

