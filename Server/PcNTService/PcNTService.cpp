#include "stdafx.h"
#include "stdio.h"
#include "tchar.h"
#include "PcNTService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// ServiceTest.cpp : Defines the entry point for the application.
//


TCHAR szServiceName[0x100] = _T("PerconServer");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;

static D3Service* service = NULL;

int __stdcall APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	return 0;
	Init();

	dwThreadID = ::GetCurrentThreadId();

    SERVICE_TABLE_ENTRY st[] =
    {
        { szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

	CString szCmdLine = lpCmdLine;
	CStringArray strList;
	int nSepCount = BSHelper::SplitStr(' ', szCmdLine, strList, TRUE);

	LogEvent("SplitStr:%d", nSepCount);
	if (nSepCount>0)
	{
		if (nSepCount>1)
		{
			strcpy_s(szServiceName, 0x100, strList.GetAt(1));
		}
		if (_stricmp(strList.GetAt(0), "--install") == 0)
		{
			Install();
		}
		else if (_stricmp(strList.GetAt(0), "--uninstall") == 0)
		{
			Uninstall();
		}
		else if (_stricmp(strList.GetAt(0), "--standalone") == 0)
		{
			service = new D3Service();
			service->InitServer(".\\service.ini");
			service->StartServer();
			TCHAR sz[0x100];

			Sleep(100000000);
			
			service->StopServer();
		}
		else
		{
			printf("parameters invalid. %s\n", szCmdLine);
		}
	}
	else
	{
		LogEvent("StartServiceCtrlDispatcher:%s", szServiceName);

		if (!::StartServiceCtrlDispatcher(st))
		{
			LogEvent(_T("Register Service Main Function Error!"));
		}
	}

	//LogEvent(_T("Register Service Main Function"));
	return 0;
}
//*********************************************************
//Functiopn:			Init
//Description:			��ʼ��
//Calls:				main
//Called By:				
//Table Accessed:				
//Table Updated:				
//Input:				
//Output:				
//Return:				
//Others:				
//History:				
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void Init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOPPED;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
	//LogEvent("Init PerconServer service:%s", szServiceName);
}

//*********************************************************
//Functiopn:			ServiceMain
//Description:			��������������������п��ƶԷ�����Ƶ�ע��
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI ServiceMain()
{
	LogEvent("ServiceMain:%s", szServiceName);
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//ע��������
    hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(hServiceStatus, &status);

    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);
	LogEvent("Starting Percon service:%s", szServiceName);

	BSHelper::GoAppRootPath();
	service = new D3Service();
	service->InitServer(".\\service.ini");
	service->StartServer();

	//ģ���������У�10���Զ��˳���Ӧ��ʱ����Ҫ������ڴ˼���
	int i = 0;
	while (service->IsRunning())
	{
		Sleep(500);
		//LogEvent("service running:%s", szServiceName);
		i++;
	}
	//

    status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &status);
	LogEvent("Percon service stopped:%s", szServiceName);
}

//*********************************************************
//Functiopn:			ServiceStrl
//Description:			�������������������ʵ�ֶԷ���Ŀ��ƣ�
//						���ڷ����������ֹͣ����������ʱ���������д˴�����
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:				dwOpcode�����Ʒ����״̬
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
	case SERVICE_CONTROL_STOP:
		LogEvent("stopping PerconServer service:%s", szServiceName);
		service->StopServer();
		status.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hServiceStatus, &status);
		LogEvent("after stopping PerconServer service:%s", szServiceName);
        PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
        break;
	case SERVICE_CONTROL_PAUSE:
		LogEvent("Pausing PerconServer service:%d", (DWORD)&service);
        break;
	case SERVICE_CONTROL_CONTINUE:
		LogEvent("Continuing PerconServer service:%d", (DWORD)&service);
        break;
	case SERVICE_CONTROL_INTERROGATE:
		LogEvent("Interrogate PerconServer service:%d", (DWORD)&service);
        break;
	case SERVICE_CONTROL_SHUTDOWN:
		LogEvent("shutdown PerconServer service:%s", szServiceName);
		service->StopServer();
		status.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hServiceStatus, &status);
		LogEvent("after shutdown PerconServer service:%s", szServiceName);
		PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
        break;
    default:
        LogEvent(_T("Bad service request"));
    }
}
//*********************************************************
//Functiopn:			IsInstalled
//Description:			�жϷ����Ƿ��Ѿ�����װ
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL IsInstalled()
{
    BOOL bResult = FALSE;

	//�򿪷�����ƹ�����
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
		//�򿪷���
        SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

//*********************************************************
//Functiopn:			Install
//Description:			��װ������
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL Install()
{
    if (IsInstalled())
        return TRUE;

	//�򿪷�����ƹ�����
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	//��������
    SC_HANDLE hService = ::CreateService(
        hSCM, szServiceName, szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't create service"), szServiceName, MB_OK);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

//*********************************************************
//Functiopn:			Uninstall
//Description:			ɾ��������
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't open service"), szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//ɾ������
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    LogEvent(_T("Service could not be deleted"));
    return FALSE;
}

//*********************************************************
//Functiopn:			LogEvent
//Description:			��¼�����¼�
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf_s(chMsg, sizeof(chMsg), pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;
	
	hEventSource = RegisterEventSource(NULL, szServiceName);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}


