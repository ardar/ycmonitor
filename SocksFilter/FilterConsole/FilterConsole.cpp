// FilterConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FilterConsole.h"
#include <io.h>

#include "XInstall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;


void  GetCurrPath(OUT TCHAR *sPath) 
{
	TCHAR sFilename[MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];

	GetModuleFileName(NULL, sFilename, _MAX_PATH);

	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	_tcscpy(sPath, sDrive);
	_tcscat(sPath, sDir);

	if(sPath[_tcslen(sPath) - 1] != _T('\\'))
		_tcscat(sPath, _T("\\"));
}  
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		CXInstall	m_Install;
		if (m_Install.IsInstalled())
		{
			m_Install.RemoveProvider();
			return 0;
		}

		TCHAR sProvider[MAX_PATH] = {0};
		GetCurrPath(sProvider);
		if(sProvider[0] == '\0')
		{
			AfxMessageBox(_T("Can't find the application path.\n"));
			return 0;
		}
		_tcscat(sProvider, XFILTER_SERVICE_DLL_NAME);

		if(_taccess(sProvider, 0) == -1)
		{
			CString sz;
			sz.Format(_T("Can't find %s"), sProvider);
			AfxMessageBox(sz);
			return 0;
		}
		CString strProcessPaths = _T("exe");
		
		strProcessPaths.Replace(_T("\r\n"), _T("\n"));
		strProcessPaths.Replace(_T("\n\n"), _T("\n"));
		strProcessPaths.Replace(_T("\n"), _T("|"));
		WritePrivateProfileString(_T("filter"), _T("paths"), strProcessPaths, _T(".\\filter.ini"));

		int nRet = m_Install.InstallProvider(sProvider, strProcessPaths.Trim().GetBuffer());
		
		
		scanf("%c");

		
		m_Install.RemoveProvider();
	}

	return nRetCode;
}