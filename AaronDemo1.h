
// AaronDemo1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAaronDemo1App: 
// �йش����ʵ�֣������ AaronDemo1.cpp
//

class CAaronDemo1App : public CWinApp
{
public:
	CAaronDemo1App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAaronDemo1App theApp;