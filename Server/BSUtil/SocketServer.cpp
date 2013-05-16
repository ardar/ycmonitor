#include "stdafx.h"
#include "SocketServer.h"


SocketServer::SocketServer( LPCTSTR szServerName)
{
	m_szServerName = szServerName;
	m_dwServerPort = 0;
	m_pNotifier = NULL;

	m_listenSocket = NULL;
	m_hMainWnd = NULL;

	m_hListenThread = NULL;
	m_hMonitorThread = NULL;
	m_hHandleThread = NULL;

	m_dwMonitorThreadId = 0;
	m_dwHandleThreadId = 0;

	m_hThreadStartupEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hAllStartedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_dwSendQueuedId = 0;
}

SocketServer::~SocketServer( void )
{
	if (WaitForSingleObject(m_hThreadExitEvent, 0)!=WAIT_OBJECT_0)
	{
		StopService();
	}
	CloseHandle(m_hThreadExitEvent);
	CloseHandle(m_hThreadStartupEvent);
	CloseHandle(m_hAllStartedEvent);

	if(m_listenSocket) closesocket(m_listenSocket);
	if(m_CompletionPort) CloseHandle(m_CompletionPort);
	if(m_hThreadStartupEvent) CloseHandle(m_hThreadStartupEvent);
	if(m_hMonitorThread) CloseHandle(m_hMonitorThread);
	if(m_hHandleThread) CloseHandle(m_hHandleThread);
	if(m_hListenThread) CloseHandle(m_hListenThread);
	if(m_hIocpWorkerThread) CloseHandle(m_hIocpWorkerThread);

	clearSessions();
	clearSendQueue();
	clearRecvQueue();
}

BOOL SocketServer::InitService(LPCTSTR szIdentifier, LPCTSTR szServerUri, ISocketServerNotify* pCallback)
{
	Log("[%s]初始化", m_szServerName);

	m_szIdentifier = szIdentifier;
	m_pNotifier = pCallback;
	CStringArray strList;
	if(2 == BSHelper::SplitStr(':', szServerUri, strList))
	{
		m_szServerIP = strList.GetAt(0);
		m_dwServerPort = atoi(strList.GetAt(1));
	}
	else
	{
		Log("[%s] 服务器监听Uri不正确 %s", m_szServerName, szServerUri);
		return FALSE;
	}

	// Initialize WinSock version 2.2
	WSADATA wsaData;
	int Ret;
	if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
	{
		// NOTE: Since WinSock failed to load we cannot use 
		// WSAGetLastError to determine the specific error for
		// why it failed. Instead we can rely on the return 
		// status of WSAStartup.
		//throw Exception;
		Log("WSAStartup 错误");
		return FALSE;
	}
}

BOOL SocketServer::StartService()
{
	Log("[%s]正在启动", m_szServerName);
	// IOCP style
	m_CompletionPort = INVALID_HANDLE_VALUE;
	PER_IO_OPERATION_DATA* lpPerIOData = NULL;
	// Create completion port
	m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Create worker thread
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	//for (int i = 0; i < systeminfo.dwNumberOfProcessors; i++)

	Log("[%s]启动服务器: 监听IP: %s, 端口: %d", m_szServerName, m_szServerIP, m_dwServerPort);

	m_listenSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	// Sets Address
	sockaddr_in serverAddr;
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons((USHORT)m_dwServerPort);
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(m_listenSocket, (const SOCKADDR*)&serverAddr, sizeof(serverAddr))==SOCKET_ERROR)
	{
		Log("[%s]绑定端口失败 %d\n", m_szServerName, WSAGetLastError());
		return FALSE;
	}

	if (listen(m_listenSocket, 1024) == SOCKET_ERROR)
	{
		Log("[%s]监听端口%d失败: %d\n", m_szServerName, m_dwServerPort, GetLastError());
		return FALSE;
	}

	// Sets timeouts
	/*int timeout=30000;
	if(setsockopt(m_listenSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout))==SOCKET_ERROR)
	{
	Log("Set socket timeout error %d\n", WSAGetLastError());
	return FALSE;
	}*/

	ResetEvent(m_hThreadExitEvent);

	Log("[%s]开始启动处理线程", m_szServerName);
	ResetEvent(m_hThreadStartupEvent);
	m_hHandleThread = CreateThread(NULL, 0, HandleThread, (LPVOID)this, 0, &m_dwHandleThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("[%s]启动处理线程完成", m_szServerName);

	Log("[%s]开始启动IOCP工作线程", m_szServerName);
	m_hIocpWorkerThread = CreateThread(NULL, 0, IOCPWorkerThread, (LPVOID)this, 0, NULL);
	Log("[%s]启动IOCP工作线程完成", m_szServerName);

	Log("[%s]开始启动监听线程", m_szServerName);
	ResetEvent(m_hThreadStartupEvent);
	m_hListenThread = CreateThread(NULL, 0, ListenThread, (LPVOID)this, 0, NULL);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("[%s]启动监听线程完成", m_szServerName);

	Log("[%s]开始启动监控线程", m_szServerName);
	ResetEvent(m_hThreadStartupEvent);
	m_hMonitorThread = CreateThread(NULL, 0, MonitorThread, (LPVOID)this, 0, &m_dwMonitorThreadId);
	WaitForSingleObject(m_hThreadStartupEvent, INFINITE);
	Log("[%s]启动监控线程完成", m_szServerName);

	SetEvent(m_hAllStartedEvent);

	Log("[%s]启动完成", m_szServerName);

	return TRUE;
}

BOOL SocketServer::StopService()
{
	Log("[%s]正在停止服务", m_szServerName);
	SetEvent(m_hThreadExitEvent);
	ResetEvent(m_hAllStartedEvent);

	postIocpOpStatus(0, IOCP_EXIT_POSTED);

	closesocket(m_listenSocket);
	m_listenSocket = NULL;
	m_hListenThread = NULL;

	WaitForSingleObject(m_hMonitorThread, INFINITE);
	WaitForSingleObject(m_hListenThread, INFINITE);
	WaitForSingleObject(m_hIocpWorkerThread, INFINITE);
	WaitForSingleObject(m_hHandleThread, INFINITE);

	CloseHandle(m_hMonitorThread);
	CloseHandle(m_hListenThread);
	CloseHandle(m_hIocpWorkerThread);
	CloseHandle(m_hHandleThread);

	m_hMonitorThread = NULL;
	m_hListenThread = NULL;
	m_hIocpWorkerThread = NULL;
	m_hHandleThread = NULL;

	CloseHandle(m_CompletionPort);
	m_CompletionPort = NULL;

	clearSessions();
	clearSendQueue();
	clearRecvQueue();

	Log("[%s]已停止服务", m_szServerName);
	return TRUE;
}

BOOL SocketServer::IsRunning()
{
	return WaitForSingleObject(m_hAllStartedEvent, 0)==WAIT_OBJECT_0;
}

void SocketServer::Log(LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start( vl, szFormat );

	TCHAR szResult[0x200] = {0};

	vsprintf_s(szResult, 0x200, szFormat, vl);

	if(m_pNotifier)
	{
		m_pNotifier->OnSessionPrintMsg(NULL, szResult);
	}
	else
	{
		BSLogger::SysLog(LOG_INFO, szResult);
	}

	va_end( vl);
}

DWORD SocketServer::MonitorThread( PVOID pParam )
{
	SocketServer* pService = (SocketServer*)pParam;

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
		catch (exception* e)
		{
			pService->Log("[%s]监控线程未知异常", pService->m_szServerName, e->what());
		}
#ifdef BSCATCH_ALL_EXCEPTION
		catch (...)
		{
			pService->Log("[%s]监控线程未知异常", pService->m_szServerName);
		}
#endif
	}
	return 0;
}

DWORD SocketServer::HandleThread( PVOID pParam )
{
	// Init the msg queue
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	SocketServer* pService = (SocketServer*)pParam;

	SetEvent(pService->m_hThreadStartupEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pService->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pService->handlePackage();
			Sleep(100);
		}
		catch (...)
		{
			pService->Log("[%s]命令线程未知异常", pService->m_szServerName);
		}
	}
	return 0;
}

void SocketServer::PostBSCmd( IBSCmd* pCmd )
{
	PostThreadMessage(m_dwHandleThreadId, WM_POST_BSCMD, (WPARAM)this, (LPARAM)pCmd);
}

DWORD SocketServer::handlePackage()
{
	if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
	{
		return 0;
	}
	// Only handle one msg each time
	QueuePackItem item;
	while (m_queueRecvPacks.size()>0)
	{
		if(TRUE)
		{
			BSAutoLock lock(&m_mutexQueueRecvPacks);
			if (m_queueRecvPacks.size()>0)
			{
				item = m_queueRecvPacks.front();
				m_queueRecvPacks.pop();
			}
		}
		if (item.buf && item.len)
		{
			SocketSession* pSession = NULL;
			if (m_mapSession.GetAt(item.SessionId, pSession))
			{
				if(!onRecvPack(pSession, item.buf, item.len))
				{
					if (m_pNotifier && pSession)
					{
						m_pNotifier->OnSessionRecvPack(
							&pSession->Info, item.buf, item.len);
					}
				}
			}
			delete [] item.buf;
			item.buf = NULL;
			item.len = 0;
		}
	}
	return 0;
}

void SocketServer::monitorProcess()
{
	DWORD dwLastCheckTime = 0;
	while (TRUE)
	{
		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return;
		}

		// Check heartbeat and send heart beat
		if (IsRunning() && GetTickCount()-dwLastCheckTime>=s_CONFIG_HEARTBEAT_TIME/2)
		{
			BSAutoLock sessionLock(m_mapSession.GetMutex());
			for (int i=m_mapSession.GetCount()-1; i>=0 ;i--)
			{
				SocketSession* pSession = NULL;
				if (m_mapSession.GetByIndex(i, pSession))
				{
					if (GetTickCount()- pSession->LastRecvBeatTime > s_CONFIG_HEARTBEAT_TIMEOUT)
					{
						Log("[SocketServer.minotor][%d][%s]Session收心跳超时 Last:%d Now:%d", 
							pSession->Info.SessionId, pSession->Info.ClientIP, 
							pSession->LastRecvBeatTime, GetTickCount());
						CloseSession(pSession->SessionId);
						continue;
					}
					if (GetTickCount()- pSession->LastSentBeatTime >= s_CONFIG_HEARTBEAT_TIME)
					{
						sendHeartBeat(pSession);
					}
				}
			}
			dwLastCheckTime = GetTickCount();
		}
		// 发送Queued数据包
		if (IsRunning())
		{
			BSAutoLock sessionLock(m_mapSession.GetMutex());
			BSAutoLock sendLock(&m_mutexQueueSendPacks);
			while(m_queueSendPacks.size()>0)
			{
				QueuePackItem& item = m_queueSendPacks.front();
				SocketSession* pSession = NULL;
				if (m_mapSession.GetAt(item.SessionId, pSession))
				{
					//BSLogger::SysLog(LOG_INFO, "SendQueued %d len:%d", item.packId, item.len);
					sendQueuedData(pSession, item.buf, item.len);
				}
				else
				{
					BSLogger::SysLog(LOG_INFO, "DropQueued %d len:%d", item.packId, item.len);
				}
				m_queueSendPacks.pop();
				delete [] item.buf;
			}
		}

		Sleep(50);
	}
}

DWORD SocketServer::ListenThread( PVOID pParam )
{
	SocketServer* pService = (SocketServer*)pParam;

	SetEvent(pService->m_hThreadStartupEvent);

	SOCKADDR_IN clientAddr;
	int nClientAddrLen = sizeof(clientAddr);

	while(TRUE)
	{
		if(WaitForSingleObject(pService->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return 0;
		}

		// Accept a connection
		SOCKET clientSocket = accept(pService->m_listenSocket, (SOCKADDR*)(&clientAddr), &nClientAddrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			pService->Log("[%s]Accept socket error %d\n", pService->m_szServerName, WSAGetLastError());
			Sleep(1000);
			continue;
		}

		pService->Log("Accepted client: %d, %s:%d\n", 
			(DWORD)clientSocket, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		DWORD dwSessionId = (DWORD)clientSocket;

		SocketSession* pSession = pService->createSession(dwSessionId, clientSocket, clientAddr);

		/// IOCP
		// Associate the newly arrived client socket with completion port
		HANDLE hHandleRet = CreateIoCompletionPort(
			(HANDLE)clientSocket, pService->m_CompletionPort, (DWORD)dwSessionId, 0);
		ASSERT(hHandleRet);
		if (!hHandleRet)
		{
			pService->Log("绑定socket到Iocp失败: %d, %s:%d\n", 
				(DWORD)clientSocket, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
			pService->deleteSession(dwSessionId);
			continue;
		}

		pService->onSessionCreated(pSession);

		PER_IO_OPERATION_DATA*lpPerIOData = new PER_IO_OPERATION_DATA();
		lpPerIOData->Buffer.len = 1024;
		lpPerIOData->Buffer.buf = new CHAR[1024];
		lpPerIOData->OperationType = IOCP_RECV_POSTED;
		lpPerIOData->NumberOfBytesProcessed = 0;
		int nRet = 0;
			
		// Launch another asynchronous operation for sClient
		ZeroMemory(&(lpPerIOData->overlap), sizeof(lpPerIOData->overlap));
		ZeroMemory(lpPerIOData->Buffer.buf, lpPerIOData->Buffer.len);
		lpPerIOData->Flags = 0;
		lpPerIOData->NumberOfBytesProcessed = 0;
		lpPerIOData->OperationType = IOCP_RECV_POSTED;
		nRet = WSARecv(pSession->ClientSocket,
			&lpPerIOData->Buffer,
			1,
			&lpPerIOData->NumberOfBytesProcessed,
			&lpPerIOData->Flags,
			&lpPerIOData->overlap,
			NULL);
		if (nRet==0)
		{
			//TRACE("WSARecv Success Imediatly\n");
		}
		else if (nRet==SOCKET_ERROR)
		{
			DWORD dwErrorCode = WSAGetLastError();
			if (dwErrorCode==WSA_IO_PENDING )
			{
				//TRACE("WSARecv IO Pendding\n");
			}
			else
			{
				pService->printSocketError(pSession, dwErrorCode, "WSARecv 失败");
			}
		}

		Sleep(50);
	}
	return 0;
}

void SocketServer::iocpWorkerProcess()
{
	DWORD dwBytesTransferred = 0;
	PER_IO_OPERATION_DATA* lpPerIOData = NULL;

	while (TRUE)
	{
		DWORD dwSessionId = 0;
		BOOL dwIORet = GetQueuedCompletionStatus(
			m_CompletionPort,
			&dwBytesTransferred,
			(PULONG_PTR)(&dwSessionId),
			(LPOVERLAPPED *)&lpPerIOData,
			INFINITE);

		if (lpPerIOData->OperationType==IOCP_EXIT_POSTED)
		{
			Log("[%d] 服务器执行清理", dwSessionId);
			delete lpPerIOData;
			break;
		}

		SocketSession* pSession = NULL;
		if (!m_mapSession.GetAt(dwSessionId, pSession))
		{
			if (lpPerIOData)
			{
				Log("[%d]SessionInvalid in IOCP Type:%d Bytes:%d", 
					dwSessionId, lpPerIOData->OperationType, dwBytesTransferred);
			}
			else
			{
				Log("[%d]SessionInvalid in IOCP", dwSessionId);
			}
			delete lpPerIOData;
			continue;
		}

		if (!dwIORet)
		{
			DWORD dwErrorCode = WSAGetLastError();//ERROR_OPERATION_ABORTED 服务器主动关闭
			if (dwErrorCode==ERROR_OPERATION_ABORTED)
			{
				printSocketError(pSession, dwErrorCode, "服务器主动关闭 In GetQueuedCompletionStatus");
			}
			else
			{
				printSocketError(pSession, dwErrorCode, "Ret Error In GetQueuedCompletionStatus");
			}
			deleteSession(dwSessionId);
			delete lpPerIOData;
			continue;
		}
		if (lpPerIOData->OperationType==IOCP_DISCONNECT)
		{
			Log("[%d] 执行删除Session", dwSessionId);
			deleteSession(dwSessionId);
		}
		else if (lpPerIOData->OperationType == IOCP_RECV_POSTED)
		{
			if (dwBytesTransferred == 0)
			{
				// Connection was closed by client
				Log("[%d] 客户端已断开连接 (InRecv)", dwSessionId);
				// ASSERT(FALSE);
				delete lpPerIOData;
				deleteSession(dwSessionId);
			}
			else
			{
				//Log("[%d] Recved %d(InRecv)", dwSessionId, dwBytesTransferred);
				lpPerIOData->NumberOfBytesProcessed = dwBytesTransferred;
				int nRet = 0;
				processBuffer(pSession, (BYTE*)lpPerIOData->Buffer.buf, lpPerIOData->NumberOfBytesProcessed);
				// Launch another asynchronous operation for sClient
				ZeroMemory(&(lpPerIOData->overlap), sizeof(lpPerIOData->overlap));
				ZeroMemory(lpPerIOData->Buffer.buf, lpPerIOData->Buffer.len);
				lpPerIOData->Flags = 0;
				lpPerIOData->NumberOfBytesProcessed = 0;
				lpPerIOData->OperationType = IOCP_RECV_POSTED;
				nRet = WSARecv(pSession->ClientSocket,
					&lpPerIOData->Buffer,
					1,
					&lpPerIOData->NumberOfBytesProcessed,
					&lpPerIOData->Flags,
					&lpPerIOData->overlap,
					NULL);
				if (nRet==0)
				{
					//TRACE("WSARecv Success Imediatly\n");
					//continue;
				}
				else if (nRet==SOCKET_ERROR)
				{
					DWORD dwErrorCode = WSAGetLastError();
					if (dwErrorCode==WSA_IO_PENDING )
					{
						//TRACE("WSARecv IO Pendding\n");
					}
					else
					{
						printSocketError(pSession, dwErrorCode, "WSARecv 失败");
					}
				}
			}
		}
		else if (lpPerIOData->OperationType == IOCP_SEND_POSTED)
		{
			if (dwBytesTransferred == 0)
			{
				// Connection was closed by client
				Log("客户端已关闭连接 %d (InSend)", dwSessionId);
				delete lpPerIOData;
				deleteSession(dwSessionId);
			}
			else
			{
				// Send Success nothing to do 
				//Log("[%d] : %d bytes sent", dwSessionId, dwBytesTransferred);
				delete lpPerIOData;
			}  
		}
	}
}

DWORD SocketServer::IOCPWorkerThread( LPVOID pParam )
{
	SocketServer* pService = (SocketServer*)pParam;
	if(!pService)
	{
		pService->Log("IOCP线程参数不正确 %X", (DWORD)pParam);
		return 0;
	}
	pService->Log("[%s] IOCP线程启动", pService->m_szServerName);
	try
	{
		pService->iocpWorkerProcess();
		Sleep(100);
	}
	catch(CException* e)
	{
		TCHAR szMsg[0x200];
		ZeroMemory(szMsg, sizeof(szMsg));
		e->GetErrorMessage(szMsg, sizeof(szMsg));
		pService->Log("[%s] IOCP线程错误 %s", pService->m_szServerName, szMsg);
	}
	catch(...)
	{
		pService->Log("[%s] IOCP线程未知错误", pService->m_szServerName);
	}
	pService->Log("[%s] IOCP线程退出", pService->m_szServerName);
	return 0;
}

void SocketServer::processBuffer(SocketSession* pSession, BYTE* recvBuffer, int nRecvLen)
{
	BSNetPack* pPack = &(pSession->Pack);
	// 遍历RecvBuffer
	while (nRecvLen>0)
	{
		UINT nSaved = 0;
		if (pPack->SplitPack(recvBuffer, nRecvLen, nSaved))
		{
			if(pPack->IsPackFinished())
			{
				// 收到一个完整报文, 加入队列
				QueuePackItem queueItem;
				queueItem.buf = new BYTE[pPack->GetPackLen()-4];
				memcpy_s(queueItem.buf, pPack->GetPackLen()-4, pPack->GetPackBuffer()+4, pPack->GetPackLen()-4);
				queueItem.len = pPack->GetPackLen()-4;
				queueItem.SessionId = pSession->SessionId;
				queueItem.packId = 0;
				BSAutoLock lock(&m_mutexQueueRecvPacks);
				m_queueRecvPacks.push(queueItem);
				
				pPack->ClearBuffer();
				recvBuffer+= nSaved;
				nRecvLen-=nSaved;
			}
			else
			{
				break;
			}
		}
		else
		{
			Log("[%s]解包错误..", m_szServerName);
			printSocketError(pSession, -1, "解包错误");
			break;
		}
	}
}

void SocketServer::printSocketError( SocketSession* pSession, DWORD dwErrorCode, LPCTSTR szMsg )
{
	if (szMsg)
	{
		Log("[%d][%s]发生网络错误 %d %s", 
			pSession->SessionId, pSession->Info.ClientIP, dwErrorCode, szMsg);
	}
	else
	{
		Log("[%d][%s]发生网络错误 %d", 
			pSession->SessionId, pSession->Info.ClientIP, dwErrorCode);
	}
}

void SocketServer::SendData( DWORD dwSessionId, BYTE* buf, int len, BOOL bDontQueue/*=FALSE*/ )
{
	BSAutoLock lock1(m_mapSession.GetMutex());
	BSAutoLock lock2(&m_mutexQueueSendPacks);
	SocketSession* pSession = NULL;
	if (m_mapSession.GetAt(dwSessionId, pSession))
	{
		if (bDontQueue)
		{
			sendQueuedData(pSession, buf, len);
		}
		else
		{
			/*if(!is())
			{
			CString sz;
			sz.Format("[%s]离线状态不能发送数据包");
			throw BSException(sz);
			return;
			}*/
			QueuePackItem item;
			item.SessionId = dwSessionId;
			item.buf = new BYTE[len];
			memcpy_s(item.buf, len, buf, len);
			item.len = len;
			item.packId = ++m_dwSendQueuedId;
			m_queueSendPacks.push(item);
		}
	}
}

void SocketServer::clearSessions()
{
	BSAutoLock lock(m_mapSession.GetMutex());
	for (int i=m_mapSession.GetCount()-1;i>=0;i--)
	{
		SocketSession* pSession = NULL;
		if(m_mapSession.GetByIndex(i, pSession))
		{
			deleteSession(pSession->SessionId);
		}
	}
	m_mapSession.RemoveAll();
}

void SocketServer::clearSendQueue()
{
	BSAutoLock lock(&m_mutexQueueSendPacks);
	while (m_queueSendPacks.size()>0)
	{
		QueuePackItem& item = m_queueSendPacks.front();
		delete [] item.buf;
		m_queueSendPacks.pop();
	}
	m_dwSendQueuedId = 0;
}

void SocketServer::clearRecvQueue()
{
	BSAutoLock lock(&m_mutexQueueRecvPacks);
	while (m_queueRecvPacks.size()>0)
	{
		QueuePackItem& item = m_queueRecvPacks.front();
		delete [] item.buf;
		m_queueRecvPacks.pop();
	}
}

void SocketServer::CloseSession( DWORD dwSessionId )
{
	postIocpOpStatus(dwSessionId, IOCP_DISCONNECT);
}

void SocketServer::postIocpOpStatus(DWORD dwSessionId, IOCP_OPERATION operationType)
{
	if (dwSessionId)
	{
		BSAutoLock lock(m_mapSession.GetMutex());
		SocketSession* pSession = NULL;
		if (m_mapSession.GetAt(dwSessionId, pSession))
		{
			PER_IO_OPERATION_DATA* lpPerIOData = new PER_IO_OPERATION_DATA();
			ZeroMemory(lpPerIOData, sizeof(PER_IO_OPERATION_DATA));
			lpPerIOData->OperationType = operationType;
			Log("[%d][%s]sendIocpOpStatus(%d)", 
				pSession->Info.SessionId, pSession->Info.ClientIP, operationType);
			PostQueuedCompletionStatus(m_CompletionPort, 0, (ULONG_PTR)dwSessionId, &lpPerIOData->overlap);
		}
	}
	else
	{
		PER_IO_OPERATION_DATA* lpPerIOData = new PER_IO_OPERATION_DATA();
		ZeroMemory(lpPerIOData, sizeof(PER_IO_OPERATION_DATA));
		lpPerIOData->OperationType = operationType;
		Log("[All]sendIocpOpStatus(%d)", operationType);
		PostQueuedCompletionStatus(m_CompletionPort, 0, (ULONG_PTR)dwSessionId, &lpPerIOData->overlap);
	}
}

void SocketServer::sendQueuedData( SocketSession* pSession, const BYTE* buf, int len )
{
	BSAutoLock lock(&m_mutexQueueSendPacks);
	// Launch another asynchronous operation for sClient
	PER_IO_OPERATION_DATA*lpPerIOData = new PER_IO_OPERATION_DATA();
	DWORD dwTotalLen = len+4;
	lpPerIOData->Buffer.len = dwTotalLen;
	lpPerIOData->Buffer.buf = new CHAR[dwTotalLen];
	lpPerIOData->OperationType = IOCP_SEND_POSTED;

	// Add the pack len to the top of the pack
	memcpy(lpPerIOData->Buffer.buf, &dwTotalLen, 4);
	memcpy_s(lpPerIOData->Buffer.buf+4, lpPerIOData->Buffer.len-4, buf, len);

	int nRet = WSASend(pSession->ClientSocket,
		&lpPerIOData->Buffer,
		1,
		&lpPerIOData->NumberOfBytesProcessed,
		NULL,
		&lpPerIOData->overlap,
		NULL);
	if (nRet==0)
	{
		//TRACE("WSASend Success\n");
	}
	else if (nRet==SOCKET_ERROR)
	{
		DWORD dwErrorCode = WSAGetLastError();
		if (dwErrorCode==WSA_IO_PENDING )
		{
			//TRACE("Send IO Pendding\n");
		}
		else
		{
			printSocketError(pSession, dwErrorCode, "发包错误");
		}
	}
}

void SocketServer::onSessionCreated( SocketSession* pSession)
{
	if (pSession)
	{
		if (m_pNotifier)
		{
			m_pNotifier->OnSessionConnected(&pSession->Info);
		}
	}
}

void SocketServer::onSessionDeleted( SocketSession* pSession)
{
	if (m_pNotifier && pSession)
	{
		m_pNotifier->OnSessionDisconnected(&pSession->Info);
	}
}

BOOL SocketServer::onRecvPack( SocketSession* pSession, BYTE* buf, int len )
{
	WORD wType = *(WORD*)(buf);
	//Log("[SocketServer]onRecvPack %X len:%d", wType, len);
	if (wType==s_PACKTYPE_HEARTBEAT)
	{
		pSession->LastRecvBeatTime = GetTickCount();
		return TRUE;
	}
	return FALSE;
}

SocketSession* SocketServer::createSession( 
	DWORD dwSessionId, SOCKET clientSocket, SOCKADDR_IN& clientAddr )
{
	BSAutoLock lock(m_mapSession.GetMutex());
	SocketSession* pSession = new SocketSession(dwSessionId, clientSocket);
	m_mapSession.SetAt(dwSessionId, pSession);	
	memcpy_s(&(pSession->ClientSockAddr), sizeof(pSession->ClientSockAddr), 
		&clientAddr, sizeof(pSession->ClientSockAddr));
	strcpy_s(pSession->Info.ClientIP, 64, inet_ntoa(clientAddr.sin_addr));
	pSession->Info.ClientPort = clientAddr.sin_port;
	pSession->LastRecvBeatTime = GetTickCount();
	pSession->LastSentBeatTime = GetTickCount();
	return pSession;
}

void SocketServer::deleteSession(DWORD dwSessionId)
{
	BSAutoLock lock(m_mapSession.GetMutex());
	SocketSession* pSession = NULL;
	if (m_mapSession.GetAt(dwSessionId, pSession))
	{
		closesocket(pSession->ClientSocket);
		onSessionDeleted(pSession);
		delete pSession;
		pSession = NULL;
		m_mapSession.RemoveKey(dwSessionId);
	}
}

void SocketServer::sendHeartBeat( SocketSession* pSession )
{
	BYTE buf[0x10];
	ZeroMemory(buf, sizeof(buf));
	*(WORD*)(buf) = s_PACKTYPE_HEARTBEAT;
	pSession->LastSentBeatTime = GetTickCount();
	sendQueuedData(pSession, buf, 2);
}
