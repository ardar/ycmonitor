// UIConnector.h : UIConnector DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUIConnectorApp
// �йش���ʵ�ֵ���Ϣ������� UIConnector.cpp
//

class CUIConnectorApp : public CWinApp
{
public:
	CUIConnectorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
