
// SoftKVM.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSoftKVMApp:
// �йش����ʵ�֣������ SoftKVM.cpp
//

class CSoftKVMApp : public CWinAppEx
{
public:
	CSoftKVMApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSoftKVMApp theApp;