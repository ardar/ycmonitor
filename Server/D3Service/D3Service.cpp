#include "stdafx.h"
#include "D3Service.h"
#include "UIServer.h"
#include <regex>
using namespace std::tr1;

D3Service D3Service::s_instance;

D3Service& D3Service::Instance()
{
	return s_instance;
}

D3Service::D3Service( void )
: m_logger("./d3service.log", 4096, 4096000)
{
	m_dwMonitorThreadId = 0;
	m_hMonitorThread = NULL;
	m_dwScheduleThreadId = 0;
	m_hScheduleThread = NULL;
	m_dwDataThreadId = 0;
	m_hDataThread = NULL;
	m_dwCmdThreadId = 0;
	m_hCmdThread = NULL;

	m_hThreadStartupEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_pUIProxy = NULL;
	m_dwGuessQueuedExecTime = 0;
}

D3Service::~D3Service( void )
{
	StopServer();
	if (WaitForSingleObject(m_hThreadExitEvent, 0)!=WAIT_OBJECT_0)
	{
		StopServer();
	}
	CloseHandle(m_hThreadExitEvent);
	CloseHandle(m_hThreadStartupEvent);

	m_logger.Flush();
}


BOOL D3Service::InitServer(LPCTSTR szConfigFilename)
{
	Log("����D3Service");

	Log("��ȡ����");
	LoadConfig(szConfigFilename);

	//��ʼ��UI����
	m_pUIProxy = (IUIProxy*)new UIServer(this);
	((UIServer*)m_pUIProxy)->InitService("UIProxy", m_setting.szUIServerListenUri, NULL);

	// Pack Handlers
	m_mapPackHandler.SetAt(D3CS_AUTH_REQUEST, new D3AuthCmd());
	

	// ��ʼ��Lua
	initLuaFuncs();

	//��ʼ�����ݿ�
	DataManager::Instance().Initialize(m_dbSetting.szHost, m_dbSetting.dwPort, 
		m_dbSetting.szUser, m_dbSetting.szPass, m_dbSetting.szDBName);

	//Socket������
	m_pServer = new SocketServer(m_szServiceId);
	m_pServer->InitService(m_szServiceId, m_setting.szListenUri, this);

	//���Խ�����
	ItemParser::Instance().LoadConfig(".\\propertyconfig.txt");

	//return TRUE;

	//��ʼ����Ϸ
	GameUser foundUser;
	if (DataManager::Instance().FindNextGameUser(foundUser))
	{
		m_setting.szGameAccount = foundUser.szUsername;
		m_setting.szGamePassword = foundUser.szPassword;
	}
	
	return TRUE;
}

BOOL D3Service::StartServer()
{
	Log("����");
	ResetEvent(m_hThreadExitEvent);

	m_dwGuessQueuedExecTime = 0;

	Log("��ʼ���������߳�");
	ResetEvent(m_hThreadStartupEvent);
	m_hDataThread = CreateThread(NULL, 0, DataThread, (LPVOID)this, 0, &m_dwDataThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("���������߳����");

	Log("��ʼ������Ϣ�����߳�");
	ResetEvent(m_hThreadStartupEvent);
	m_hCmdThread = CreateThread(NULL, 0, CmdThread, (LPVOID)this, 0, &m_dwCmdThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("������Ϣ�����߳����");

	Log("��ʼ�����ƻ������߳�");
	ResetEvent(m_hThreadStartupEvent);
	m_hScheduleThread = CreateThread(NULL, 0, ScheduleThread, (LPVOID)this, 0, &m_dwScheduleThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("������Ϣ�ƻ������߳����");

	Log("��ʼ��������߳�");
	ResetEvent(m_hThreadStartupEvent);
	m_hMonitorThread = CreateThread(NULL, 0, MonitorThread, (LPVOID)this, 0, &m_dwMonitorThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("��������߳����");

	if(! ((UIServer*)m_pUIProxy)->StartService())
	{
		ErrorLog("����UIProxy Serverʧ�� %s", m_setting.szUIServerListenUri);
		return FALSE;
	}

	if(!m_pServer->StartService())
	{
		ErrorLog("����D3Serverʧ�� %s", m_setting.szListenUri);
		return FALSE;
	}

	UIRefreshService();
	UIRefreshDevices();

	Log("�������");
	return TRUE;
}

void D3Service::StopServer()
{
	Log("ֹͣ");
	SetEvent(m_hThreadExitEvent);
	if (m_hMonitorThread || m_hScheduleThread)
	{
		WaitForSingleObject(m_hMonitorThread, INFINITE);
		CloseHandle(m_hMonitorThread);
		m_hMonitorThread = NULL;
		WaitForSingleObject(m_hScheduleThread, INFINITE);
		CloseHandle(m_hScheduleThread);
		m_hScheduleThread = NULL;
		WaitForSingleObject(m_hCmdThread, INFINITE);
		CloseHandle(m_hCmdThread);
		m_hCmdThread = NULL;
		WaitForSingleObject(m_hDataThread, INFINITE);
		CloseHandle(m_hDataThread);
		m_hDataThread = NULL;
	}

	Log("MonitorThread��ֹͣ");
	Log("ֹͣServer");
	m_pServer->StopService();
	Log("ֹͣUIServer");
	((UIServer*)m_pUIProxy)->StopService();

	Log("ֹͣ���");
}

void D3Service::LoadConfig(LPCTSTR szConfigFilename)
{
	static const LPCTSTR szServiceSection = "d3service";
	static const LPCTSTR szDBSection = "database";
	BSHelper::GoAppRootPath();

	// database 
	TCHAR szTemp[64];
	if(GetPrivateProfileString(szDBSection, "host", 
		"127.0.0.1", szTemp, MAX_NAME_LENGTH, szConfigFilename))
	{
		m_dbSetting.szHost = szTemp;
	}
	if(GetPrivateProfileString(szDBSection, "user", 
		"root", szTemp, MAX_NAME_LENGTH, szConfigFilename))
	{
		m_dbSetting.szUser = szTemp;
	}
	if(GetPrivateProfileString(szDBSection, "pass", 
		"", szTemp, MAX_NAME_LENGTH, szConfigFilename))
	{
		m_dbSetting.szPass = szTemp;
	}
	if(GetPrivateProfileString(szDBSection, "dbname", 
		"d3", szTemp, MAX_NAME_LENGTH, szConfigFilename))
	{
		m_dbSetting.szDBName = szTemp;
	}
	m_dbSetting.dwPort = GetPrivateProfileInt(szDBSection, "port", 3306, szConfigFilename);

	// local service
	TCHAR szMac[MAX_NAME_LENGTH];
	GetPrivateProfileString(szServiceSection, "serviceid", "", szMac, MAX_NAME_LENGTH, szConfigFilename);
	m_szServiceId = szMac;

	TCHAR szListen[MAX_NAME_LENGTH];
	GetPrivateProfileString(szServiceSection, "listen", 
		"0.0.0.0:8008", szListen, MAX_NAME_LENGTH, szConfigFilename);
	m_setting.szListenUri = szListen;

	TCHAR szAppRoot[MAX_PATH];
	ZeroMemory(szAppRoot, MAX_PATH);
	BSHelper::GetAppRootPath(szAppRoot, MAX_PATH);

	// ui server
	TCHAR szUIListen[MAX_NAME_LENGTH];
	GetPrivateProfileString("monitor_service", "listen", 
		"0.0.0.0:8009", szUIListen, MAX_NAME_LENGTH, szConfigFilename);
	m_setting.szUIServerListenUri = szUIListen;

	// game proxy
	TCHAR szTemp2[MAX_PATH];
	ZeroMemory(szTemp2, sizeof(szTemp2));
	GetPrivateProfileString("game_proxy", "gamepath", 
		"D:\\Games\\Diablo III\\Diablo III.exe", szTemp2, MAX_PATH, szConfigFilename);
	m_setting.szGamePath = szTemp2;
	ZeroMemory(szTemp2, sizeof(szTemp2));
	GetPrivateProfileString("game_proxy", "dllpath", 
		"E:\\Projects\\D3ah\\D3Dll\\Debug\\D3Dll.dll", szTemp2, MAX_PATH, szConfigFilename);
	m_setting.szInjectDllPath = szTemp2;
	ZeroMemory(szTemp2, sizeof(szTemp2));
	GetPrivateProfileString("game_proxy", "account", 
		"", szTemp2, MAX_PATH, szConfigFilename);
	m_setting.szGameAccount = szTemp2;
	ZeroMemory(szTemp2, sizeof(szTemp2));
	GetPrivateProfileString("game_proxy", "password", 
		"", szTemp2, MAX_PATH, szConfigFilename);
	m_setting.szGamePassword = szTemp2;

	Log("��ȡ������[%s]����, ������ַ:%s", m_szServiceId, m_setting.szListenUri);

}

void D3Service::Log(LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start( vl, szFormat );

	TCHAR szResult[0x400] = {0};

	vsprintf_s(szResult, 0x400, szFormat, vl);
	
	TRACE("%s\n", szResult);

	CString szLogText;
	szLogText.Format("%s%s\r\n", "<D3Service>", szResult);
	m_logger.Log(szLogText);

	UISysLog(szResult);

	va_end( vl);
}

void D3Service::ErrorLog(LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start( vl, szFormat );

	TCHAR szResult[0x200] = {0};

	vsprintf_s(szResult, 0x200, szFormat, vl);

	TRACE("Error:%s\n", szResult);

	BSLogger::SysLog(LOG_ERROR, szResult);

	UIErrorLog(0, 0, szResult);

	va_end( vl);
}

DWORD D3Service::CmdThread( PVOID pParam )
{
	// Init the msg queue
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	D3Service* pService = (D3Service*)pParam;

	SetEvent(pService->m_hThreadStartupEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pService->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pService->cmdProcess();
		}
		catch(BSException& e)
		{
			pService->Log("�����������߳��쳣 %s", e.GetErrorMessage());
		}
#ifndef _DEBUG
		catch (...)
		{
			pService->Log("�����������߳�δ֪�쳣");
		}
#endif
	}
	return 0;
}

DWORD D3Service::MonitorThread( PVOID pParam )
{
	// Init the msg queue
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	D3Service* pService = (D3Service*)pParam;

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
		catch(BSException& e)
		{
			pService->Log("���������߳��쳣 %s", e.GetErrorMessage());
		}
#ifndef _DEBUG
		catch (...)
		{
			pService->Log("���������߳�δ֪�쳣");
		}
#endif
	}
	return 0;
}

DWORD D3Service::handleMsg()
{
	MSG msg;
	while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		switch (msg.message)
		{
		case WM_PERCON_MSG::WM_BSCmd:
			{
				IHandlerCmd* pCmd = (IHandlerCmd*)msg.lParam;
				if (pCmd)
				{
					Log("ִ��BSCmd:%s", pCmd->GetName());
					if (GetTickCount() - pCmd->GetQueueTime() > 30000)
					{
						Log("Cmd���r, �G��:%s", pCmd->GetName());
					}
					else
					{
						pCmd->Execute();
						Log("���BSCmd:%s", pCmd->GetName());
					}
					SetSessionOp(pCmd->m_dwSessionId, pCmd->GetName());
					delete pCmd;
					Sleep(1000);
				}
			}
			break;
		default: break;
		}
	}
	return 0;
}


void D3Service::cmdProcess()
{
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

void D3Service::monitorProcess()
{
	while (TRUE)
	{
		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return;
		}

		// TODO: nothing
		

		Sleep(500);
	}
}


DWORD D3Service::ScheduleThread( PVOID pParam )
{
	D3Service* pService = (D3Service*)pParam;

	SetEvent(pService->m_hThreadStartupEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pService->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pService->scheduleProcess();
		}
		catch(BSException& e)
		{
			pService->Log("������ƻ������߳��쳣 %s", e.GetErrorMessage());
			Sleep(5000);
		}
#ifndef _DEBUG
		catch (...)
		{
			pService->Log("������ƻ������߳�δ֪�쳣");
		}
#endif
	}
	return 0;
}

void D3Service::scheduleProcess()
{
	while (TRUE)
	{
		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return;
		}
		//��ȡSchedules
		CArray<D3Schedule*> scheduleList;
		if(DataManager::Instance().GetSchedules(scheduleList))
		{
			Log("����ȡ���ƻ����� %d ��", scheduleList.GetCount());
		}

		for (int i=0;i<scheduleList.GetCount();i++)
		{
			D3Schedule* pSchedule = scheduleList.GetAt(i);
			if (pSchedule)
			{
				if (pSchedule->szName.size()==0)
				{
					continue;
				}
				//Log("���Schedule:%s", pSchedule->szName.c_str());

				BOOL bToExecute = FALSE;
				DWORD dwGapTime = 0;
				DWORD dwRepeatUnit = 0;
				switch(pSchedule->repeatType)
				{
				case D3Schedule::D3RepeatType::RepeatBySeconds:
					{
						CTime timeNow = CTime::GetCurrentTime();
						
						DWORD dwToRunTime = pSchedule->dwLastExecTime > 0  
							? (pSchedule->dwLastExecTime + pSchedule->dwRepeatParam) 
							: (pSchedule->dwBeginTime + pSchedule->dwRepeatParam);
						if (timeNow.GetTime() >= pSchedule->dwBeginTime 
							&& timeNow.GetTime() >= dwToRunTime)
						{
							bToExecute = TRUE;
							Log("����ִ�� nexttime:%d, Now:%d", dwToRunTime, (DWORD)time(0));
						}
					}
					break;
				case D3Schedule::D3RepeatType::RepeatByMinute:
					{
						CTime timeNow = CTime::GetCurrentTime();
						int nMonth = timeNow.GetMonth();
						int nYear = timeNow.GetYear();
						int nDay = timeNow.GetDay();
						int nHour = timeNow.GetHour();
						int nMin = timeNow.GetMinute();
						CTime dateNow(nYear, nMonth, nDay, nHour, nMin, 0);
						DWORD dwToRunTime = dateNow.GetTime() + pSchedule->dwRepeatParam%60;
						if (dwToRunTime >= timeNow.GetTime()
							&& dwToRunTime>pSchedule->dwLastExecTime)
						{
							bToExecute = TRUE;
							Log("����ִ�� nexttime:%d, Now:%d", dwToRunTime, (DWORD)time(0));
						}
					}
					break;
				case D3Schedule::D3RepeatType::RepeatByHour:
					{
						CTime timeNow = CTime::GetCurrentTime();
						int nMonth = timeNow.GetMonth();
						int nYear = timeNow.GetYear();
						int nDay = timeNow.GetDay();
						int nHour = timeNow.GetHour();
						CTime dateNow(nYear, nMonth, nDay, nHour, 0, 0);
						DWORD dwToRunTime = dateNow.GetTime() + pSchedule->dwRepeatParam%3600;
						if (dwToRunTime >= timeNow.GetTime()
							&& dwToRunTime>pSchedule->dwLastExecTime)
						{
							bToExecute = TRUE;
							Log("��Сʱnexttime:%d, Now:%d", dwToRunTime, (DWORD)time(0));
						}
					}
					break;
				case D3Schedule::D3RepeatType::RepeatByDay:
					{
						CTime timeNow = CTime::GetCurrentTime();
						int nMonth = timeNow.GetMonth();
						int nYear = timeNow.GetYear();
						int nDay = timeNow.GetDay();
						CTime dateNow(nYear, nMonth, nDay, 0, 0, 0);
						DWORD dwToRunTime = dateNow.GetTime() + pSchedule->dwRepeatParam%86400;
						if (dwToRunTime >= timeNow.GetTime()
							&& dwToRunTime>pSchedule->dwLastExecTime)
						{
							bToExecute = TRUE;
							Log("���� nexttime:%d, Now:%d", dwToRunTime, (DWORD)time(0));
						}
					}
					break;
				case D3Schedule::D3RepeatType::RepeatByWeek://����Ϊ���ܳ���Ӧִ��ʱ֮����
					{
						CTime timeNow = CTime::GetCurrentTime();
						int nMonth = timeNow.GetMonth();
						int nYear = timeNow.GetYear();
						int nDayOfWeek = timeNow.GetDayOfWeek();
						int nDay = timeNow.GetDay();
						CTime dateNow(nYear, nMonth, nDay, 0, 0, 0);
						DWORD dwWeekBeginTime = dateNow.GetTime() - nDay*86400;
						DWORD dwToRunTime = dwWeekBeginTime + pSchedule->dwRepeatParam;
						if (dwToRunTime >= timeNow.GetTime()
							&& dwToRunTime>pSchedule->dwLastExecTime)
						{
							bToExecute = TRUE;
							Log("���� nexttime:%d, Now:%d", dwToRunTime, (DWORD)time(0));
						}
					}
					break;
				case D3Schedule::D3RepeatType::RepeatByMonth://����Ϊ���³���Ӧִ��ʱ֮����
					{
						CTime timeNow = CTime::GetCurrentTime();
						int nMonth = timeNow.GetMonth();
						int nYear = timeNow.GetYear();
						CTime monthBeginTime(nYear, nMonth, 1, 0, 0, 0);
						DWORD dwToRunTime = monthBeginTime.GetTime() + pSchedule->dwRepeatParam;
						if (dwToRunTime >= timeNow.GetTime()
							&& dwToRunTime>pSchedule->dwLastExecTime)
						{
							bToExecute = TRUE;
							Log("���� nexttime:%d, Now:%d", dwToRunTime, (DWORD)time(0));
						}
					}
					break;
				case D3Schedule::D3RepeatType::RepeatOnce://����Ϊtime(0)ʱ���
					if (time(0)>=pSchedule->dwRepeatParam 
						&& pSchedule->dwLastExecTime<pSchedule->dwRepeatParam)
					{
						bToExecute = TRUE;
						Log("������һ�� nexttime:%d, Now:%d", pSchedule->dwRepeatParam, (DWORD)time(0));
					}
					break;
				default:
					Log("Not Supported repeat type %d", pSchedule->repeatType);
					break;
				}
				if (bToExecute)
				{
					executeSchedule(pSchedule);
				}
			}
		}
		for (int i=0;i<scheduleList.GetCount();i++)
		{
			delete scheduleList.GetAt(i);
		}
		Sleep(10000);
	}
}

DWORD D3Service::DataThread( PVOID pParam )
{
	D3Service* pService = (D3Service*)pParam;

	SetEvent(pService->m_hThreadStartupEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pService->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pService->dataProcess();
		}
		catch(BSException& e)
		{
			pService->Log("�����߳��쳣 %s", e.GetErrorMessage());
		}
#ifndef _DEBUG
		catch (...)
		{
			pService->Log("�����߳�δ֪�쳣");
		}
#endif
	}
	return 0;
}
void D3Service::dataProcess()
{
	while (TRUE)
	{
		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return;
		}
		vector<D3SearchItem*> penddingList;
		if (m_queueSavingItems.size()>0)
		{
			BSAutoLock lock(&m_mutexSavingItems);
			while (m_queueSavingItems.size()>0)
			{
				D3SearchItem* pItem = m_queueSavingItems.front();
				m_queueSavingItems.pop();
				if (pItem)
				{
					penddingList.push_back(pItem);
				}
			}
		}
		if (penddingList.size()>0)
		{
			vector<D3SearchItem*>::iterator it;
			for (it = penddingList.begin(); it!=penddingList.end(); it++)
			{
				D3SearchItem* pItem = *it;
				DataManager::Instance().SaveItem(*pItem);
				Log("Saving queued item: %d %s", pItem->dwDataId, pItem->info.szName);
				delete pItem;
			}
			penddingList.clear();
		}
		Sleep(500);
	}
}

void D3Service::executeSchedule(D3Schedule* pSchedule)
{
	D3Service::Log("ִ�мƻ�����:%s ID:%d", pSchedule->szName.c_str(), pSchedule->dwScheduleId);
	try
	{
		switch(pSchedule->operationType)
		{
		case D3Schedule::D3OpType::ScheduleScript:
			{
				Log("ִ�мƻ�����ű�:%s", pSchedule->szOperationParam.c_str());
				m_pBSLua->DoString(pSchedule->szOperationParam.c_str());
				pSchedule->dwLastError = 0;
				pSchedule->dwLastExecTime = time(0);
				DataManager::Instance().SaveSchedule(*pSchedule);
			}
			break;
		default:
			break;
		}
	}
	catch (ScriptException& e)
	{
		pSchedule->dwLastError = ERR_ScriptError;
		pSchedule->dwLastExecTime = time(0);
		DataManager::Instance().SaveSchedule(*pSchedule);
		BSLogger::SysLog(LOG_ERROR, "����Ӌ���΄��_���e�` %s", e.GetErrorMessage());
	}
	catch (BSException& ex)
	{
		pSchedule->dwLastError = ERR_Unknown;
		pSchedule->dwLastExecTime = time(0);
		DataManager::Instance().SaveSchedule(*pSchedule);
		BSLogger::SysLog(LOG_ERROR, "����Ӌ���΄�δ֪�e�` %s", ex.GetErrorMessage());
	}
	D3ScheduleUpdatePack pack;
	pack.m_dwRequestId = pack.m_dwSessionId = 0;
	pack.m_errorCode = ERR_Success;
	pack.m_schedule = *pSchedule;
	SendToSession(-1, &pack);
}

DWORD D3Service::QueueCmd( IHandlerCmd* pCmd )
{
	if (m_dwCmdThreadId)
	{
		if (m_dwGuessQueuedExecTime>30000)
		{
			pCmd->Cancel(D3ErrorCode::ERR_TooManyQueuedCmd);
			delete pCmd;
			return D3ErrorCode::ERR_Success;
		}
		m_dwGuessQueuedExecTime += pCmd->GuessExecTime();
		pCmd->m_dwQueueTime = GetTickCount();
		PostThreadMessage(m_dwCmdThreadId, WM_PERCON_MSG::WM_BSCmd, NULL, (LPARAM)pCmd);
	}
	else
	{
		delete pCmd;
	}
	return D3ErrorCode::ERR_Success;
}

void D3Service::CloseSession(DWORD dwSessionId)
{
	m_pServer->CloseSession(dwSessionId);
	DeleteSession(dwSessionId);
}

BOOL D3Service::SendToSession( DWORD dwSessionId, ISocketPack* pPack )
{
	if (dwSessionId==0)
	{
		// SessionId==0 do not reply to any client.
		return TRUE;
	}
	else if (dwSessionId==-1)
	{
		// Send to all clients
		for (int i=0;i<m_mapSession.GetCount();i++)
		{
			DWORD dwSesId = 0;
			if (m_mapSession.GetKeyByIndex(i, dwSesId))
			{
				SendToSession(dwSesId, pPack);
			}
		}
	}
	else
	{
		BYTE buf[0x10000];
		ZeroMemory(buf, sizeof(buf));
		int nBuflen = 0;
		if(pPack->ToBuffer(buf, sizeof(buf), nBuflen))
		{
			m_pServer->SendData(dwSessionId, buf, nBuflen);
		}
		else if(nBuflen>0)
		{
			nBuflen = 0;
			BYTE* pbuf = new BYTE[0x10000];
			ZeroMemory(pbuf, 0x10000);
			if(pPack->ToBuffer(pbuf, 0x10000, nBuflen))
			{
				m_pServer->SendData(dwSessionId, pbuf, nBuflen);
			}
			else
			{
				delete [] pbuf;
				throw SocketException("Pack too big");
			}
			delete [] pbuf;
		}
	}
	return TRUE;
}

BOOL D3Service::GetSession( DWORD dwSessionId, D3ServiceSession& session)
{
	return m_mapSession.GetAt(dwSessionId, session);
}

void D3Service::UpdateSession(D3ServiceSession& session)
{
	m_mapSession.SetAt(session.dwSessionid, session);
}

void D3Service::CreateSession( DWORD dwSessionId, const PcSessionInfo& pcSession )
{
	D3ServiceSession session;
	strcpy_s(session.szClientIP, sizeof(session.szClientIP), pcSession.ClientIP);
	session.dwClientPort = pcSession.ClientPort;
	session.dwSessionid = pcSession.SessionId;
	session.dwBeginTime = time(0);
	session.bIsAuthenticated = FALSE;
	m_mapSession.SetAt(dwSessionId, session);
	m_pUIProxy->UpdateClient(session);
}

void D3Service::DeleteSession( DWORD dwSessionId )
{
	m_mapSession.RemoveKey(dwSessionId);
	m_pUIProxy->RemoveClient(dwSessionId);
}

BOOL D3Service::SetSessionOp( DWORD dwSessionId, LPCTSTR szOPName )
{
	D3ServiceSession session;
	if(m_mapSession.GetAt(dwSessionId, session))
	{
		session.dwLastRecvTime = time(0);
		strcpy_s(session.szLastCmd, sizeof(session.szLastCmd), szOPName);
		m_pUIProxy->UpdateClient(session);
		return TRUE;
	}
	return FALSE;
}

BOOL D3Service::SetAuththenticated( DWORD dwSessionId, LPCTSTR szUsername )
{
	D3ServiceSession session;
	if(m_mapSession.GetAt(dwSessionId, session))
	{
		session.bIsAuthenticated = TRUE;
		strcpy_s(session.szUsername, sizeof(session.szUsername), szUsername);
		m_pUIProxy->UpdateClient(session);
		return TRUE;
	}
	return FALSE;
}

BOOL D3Service::IsAuthenticated( DWORD dwSessionId )
{
	D3ServiceSession session;
	if(m_mapSession.GetAt(dwSessionId, session))
	{
		return session.bIsAuthenticated;
	}
	return FALSE;
}

void D3Service::QueueItemData( D3SearchItem* pItem )
{
	BSAutoLock lock(&m_mutexSavingItems);
	m_queueSavingItems.push(pItem);
}

void D3Service::UpdateCurrAccSetting( LPCTSTR szAccount, LPCTSTR szPassword )
{
	m_setting.szGameAccount = szAccount;
	m_setting.szGamePassword = szPassword;
	UIRefreshService();
}
