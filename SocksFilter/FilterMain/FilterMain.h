
// FilterMain.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFilterMainApp:
// �йش����ʵ�֣������ FilterMain.cpp
//

class CFilterMainApp : public CWinAppEx
{
public:
	CFilterMainApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFilterMainApp theApp;