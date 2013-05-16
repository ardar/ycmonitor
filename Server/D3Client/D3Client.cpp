#include "stdafx.h"

#define MONITOR_INI_FILE ".\\serverconfig.ini"

ID3Client* ID3Client::CreateInstance()
{
	return (ID3Client*)new D3Client();
}

D3Client::D3Client( void )
{
	m_hMainWnd = NULL;

	m_dwMonitorThreadId = 0;
	m_hMonitorThread = NULL;

	m_hThreadStartupEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_pClient = NULL;
	m_bIsWaitCallResult = FALSE;
	m_bIsAuthenticated = FALSE;
	m_pOnClientCallback = NULL;
	m_searchItemCacheRequestId = 0;
	m_dwCurrRequestId = 0;
}

D3Client::~D3Client( void )
{
	if (WaitForSingleObject(m_hThreadExitEvent, 0)!=WAIT_OBJECT_0)
	{
		StopClient();
	}

	CloseHandle(m_hThreadStartupEvent);
	CloseHandle(m_hThreadExitEvent);

	delete m_pClient;
}

BOOL D3Client::InitClient(LPCTSTR szClientId, LPCTSTR szConfigFilename, Func_OnClientCallback pCallback )
{
	Log("[D3Client]初始化");
	m_szClientId = szClientId;
	m_pOnClientCallback = pCallback;

	Log("[D3Client]读取配置");
	LoadConfig(szConfigFilename);

	CString szSocketServerAddr = m_szUpServerUri;
	if(szSocketServerAddr.Find("socket://", 0)==0)
	{
		szSocketServerAddr.Replace("socket://", "");
	}
	else
	{
		Log("[PcClient]服务器地址设置不正确 %s", szSocketServerAddr);
	}

	m_pClient = (SocketClient*)new SocketClient("D3Client");
	m_pClient->InitClient(m_szClientId, szSocketServerAddr, this);

	// 初始化收报函数表
	m_mapRecvFunc.SetAt(D3PACKTYPE::D3SC_AUTH_RESULT, &D3Client::onRecvAuthResult);

	return TRUE;
}

BOOL D3Client::StartClient()
{
	Log("[D3Client]启动");

	// Start the client
	if(!m_pClient->StartClient())
	{
		ErrorLog("[D3Client]启动D3Client失败 %s", m_szUpServerUri);
		return FALSE;
	}

	Log("[D3Client]开始启动消息处理线程");
	ResetEvent(m_hThreadExitEvent);
	ResetEvent(m_hThreadStartupEvent);
	m_hMonitorThread = CreateThread(NULL, 0, MonitorThread, (LPVOID)this, 0, &m_dwMonitorThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("[D3Client]启动消息处理线程完成");

	Log("[D3Client]启动完成");

	return TRUE;
}

void D3Client::StopClient()
{
	Log("[D3Client]停止");
	SetEvent(m_hThreadExitEvent);
	WaitForSingleObject(m_hMonitorThread, INFINITE);
	CloseHandle(m_hMonitorThread);
	m_hMonitorThread = NULL;
	m_dwMonitorThreadId = 0;

	m_pClient->StopClient();

	Log("[D3Client]停止完成");
}

BOOL D3Client::IsConnected()
{
	return m_pClient->IsConnected();
}

void D3Client::LoadConfig(LPCTSTR szConfigFile)
{
	BSHelper::GoAppRootPath();
	LPCTSTR szSection = "server";

	if (m_szClientId.GetLength()==0)
	{
		TCHAR szMac[MAX_NAME_LENGTH];
		GetPrivateProfileString(szSection, "user", "", szMac, MAX_NAME_LENGTH, szConfigFile);
		m_szClientId = szMac;
	}

	if (m_szUpServerUri.GetLength()==0)
	{
		TCHAR szServerIP[MAX_NAME_LENGTH];
		GetPrivateProfileString(szSection, "server", 
			"socket://127.0.0.1:8008", szServerIP, MAX_NAME_LENGTH, szConfigFile);

		m_szUpServerUri.Format("%s", szServerIP);
	}

	TCHAR szAppRoot[MAX_PATH];
	ZeroMemory(szAppRoot, MAX_PATH);
	BSHelper::GetAppRootPath(szAppRoot, MAX_PATH);
	m_szAppRootDir.Format("%s", szAppRoot);

	Log("[D3Client]读取配置完成, Mac地址:%s 上级地址:%s", m_szClientId, m_szUpServerUri);
}

void D3Client::SetMainWnd( HWND hMainWnd )
{
	ASSERT(hMainWnd);
	m_hMainWnd = hMainWnd;
}

void D3Client::Log(LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start( vl, szFormat );

	TCHAR szResult[0x200] = {0};

	vsprintf_s(szResult, 0x200, szFormat, vl);

	TRACE("%s\n", szResult);

	BSLogger::SysLog(LOG_INFO, "%s", szResult);

	UISysLog(szResult);

	va_end( vl);
}

void D3Client::ErrorLog(LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start( vl, szFormat );

	TCHAR szResult[0x200] = {0};

	vsprintf_s(szResult, 0x200, szFormat, vl);

	TRACE("Error:%s\n", szResult);

	BSLogger::SysLog(LOG_ERROR, szResult);

	UIErrorLog(0, szResult);

	va_end( vl);
}

DWORD D3Client::MonitorThread( PVOID pParam )
{
	// Init the msg queue
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	D3Client* pService = (D3Client*)pParam;

	SetEvent(pService->m_hThreadStartupEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pService->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pService->monitorProcess();
		}
		catch (...)
		{
			pService->Log("监控器监控线程未知异常");
		}
	}
	return 0;
}

DWORD D3Client::handleMsg()
{
	MSG msg;
	while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return 0;
		}
	}
	return 0;
}

void D3Client::monitorProcess()
{
	DWORD dwLastCheckTime = 0;
	while (TRUE)
	{
		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return;
		}

		handleMsg();

		Sleep(50);
	}
}

void D3Client::notifyMonitor(DWORD dwMsg, DWORD wParam, DWORD lParam)
{
	PostThreadMessage(m_dwMonitorThreadId, dwMsg, wParam, lParam);
}

void D3Client::SendPack( ISocketPack* pCmd )
{
	BYTE buf[0x1000];
	int nLen = 0;
	if(pCmd->ToBuffer(buf, sizeof(buf), nLen))
	{
		m_pClient->SendData(buf, nLen, FALSE);
	}
	else
	{
		ASSERT(FALSE);
	}
}


DWORD D3Client::createRequestId()
{
	BSAutoLock lock(&m_mutexRequestId);
	return ++m_dwCurrRequestId;
}
