#include "stdafx.h"

void D3Client::UISysLog(LPCTSTR szLog)
{
	return;
	CString* pStr = new CString(szLog);
	TRACE("SysLog: %s\n", szLog);
	if(m_hMainWnd)
	{
		//PostMessage(m_hMainWnd, WM_MS_SYSMSG, (WPARAM)0, (LPARAM)pStr);
	}
}

void D3Client::UIErrorLog(DWORD dwErrorCode, LPCTSTR szLog)
{
	return;
	CString* pStr = new CString(szLog);
	TRACE("ErrorLog: %s\n", szLog);
	pStr->Format("Err:%d %s", dwErrorCode, szLog);
	if(m_hMainWnd)
	{
		//PostMessage(m_hMainWnd, WM_MS_SYSMSG, (WPARAM)0, (LPARAM)pStr);
	}
}

void D3Client::UIPostMsg(DWORD dwMsg, DWORD wParam/*=NULL*/, DWORD lParam/*=NULL*/)
{
	if(m_hMainWnd)
	{
		PostMessage(m_hMainWnd, dwMsg, (WPARAM)wParam, (LPARAM)lParam);
	}
}