// Player16Lib.h : Player16Lib DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPlayer16LibApp
// �йش���ʵ�ֵ���Ϣ������� Player16Lib.cpp
//

class CPlayer16LibApp : public CWinApp
{
public:
	CPlayer16LibApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
