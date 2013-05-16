#include "stdafx.h"
#include "SocketClient.h"


SocketClient::SocketClient(LPCTSTR szClientName)
:m_pNotifier(NULL)
{
	m_szClientName = szClientName;

	ZeroMemory(&m_connInfo, sizeof(m_connInfo));
	m_socket = NULL;
	m_dwServerPort = 0;
	m_dwMonitorThreadId = 0;
	m_dwCmdThreadId = 0;

	m_hMonitorThread = NULL;
	m_hCmdThread = NULL;
	m_hRecvThread = NULL;

	m_hIsRunning = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hConnected = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThreadRunningEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

SocketClient::~SocketClient(void)
{
	if (WaitForSingleObject(m_hThreadExitEvent, 0)!=WAIT_OBJECT_0)
	{
		StopClient();
	}

	CloseHandle(m_hIsRunning);
	CloseHandle(m_hConnected);
	CloseHandle(m_hThreadRunningEvent);
	CloseHandle(m_hThreadExitEvent);
}

BOOL SocketClient::InitClient(LPCTSTR szIdentifier, LPCTSTR szServerUri, ISocketClientNotify* pNotifier)
{
	Log("[%s]初始化", m_szClientName);
	m_szIdentifier = szIdentifier;
	m_pNotifier = pNotifier;
	CString strServerUri = szServerUri;
	strServerUri = strServerUri.MakeLower();
	strServerUri.Replace("socket://", "");
	CStringArray strList;
	if(2 == BSHelper::SplitStr(':', strServerUri, strList))
	{
		m_szServerIP = strList.GetAt(0);
		m_dwServerPort = atoi(strList.GetAt(1));
	}
	else
	{
		Log("[%s] 服务器Uri不正确 %s", m_szClientName, szServerUri);
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
		BSLogger::SysLog(LOG_ERROR, "[%s]WSAStartup 错误", m_szClientName);
		return FALSE;
	}
}

BOOL SocketClient::StartClient()
{
	Log("[%s]启动", m_szClientName);
	BSAutoLock lock(&m_mutexQueuePacks);
	clearSendQueue();

	ResetEvent(m_hThreadExitEvent);

	Log("[%s]开始启动命令处理线程", m_szClientName);
	ResetEvent(m_hThreadRunningEvent);
	m_hCmdThread = CreateThread(NULL, 0, CmdThread, (LPVOID)this, 0, &m_dwCmdThreadId);
	WaitForSingleObject(m_hThreadRunningEvent, INFINITE);
	Log("[%s]命令处理线程已启动", m_szClientName);

	ResetEvent(m_hThreadRunningEvent);
	Log("[%s]开始启动Monitor线程", m_szClientName);
	m_hMonitorThread = CreateThread(NULL, NULL, MonitorThread, this, NULL, &m_dwMonitorThreadId);
	WaitForSingleObject(m_hThreadRunningEvent, INFINITE);
	Log("[%s]Monitor线程已启动 ID:%u", m_szClientName, m_dwMonitorThreadId);

	ResetEvent(m_hThreadRunningEvent);
	Log("[%s]开始启动收报线程", m_szClientName);
	m_hRecvThread = CreateThread(NULL, NULL, WorkerThread, this, NULL, NULL);
	WaitForSingleObject(m_hThreadRunningEvent, INFINITE);
	Log("[%s]收报线程已启动", m_szClientName);

	SetEvent(m_hIsRunning);
	return TRUE;
}

BOOL SocketClient::StopClient()
{
	Log( "[%s]停止", m_szClientName);
	
	ResetEvent(m_hIsRunning);
	SetEvent(m_hThreadExitEvent);

	// wait for every thread to exit;
	WaitForSingleObject(m_hRecvThread, INFINITE);
	WaitForSingleObject(m_hMonitorThread, INFINITE);
	WaitForSingleObject(m_hCmdThread, INFINITE);

	CloseHandle(m_hRecvThread);
	CloseHandle(m_hMonitorThread);
	CloseHandle(m_hCmdThread);

	m_hRecvThread = NULL;
	m_hMonitorThread = NULL;
	m_hCmdThread = NULL;

	BSAutoLock lock(&m_mutexQueuePacks);
	clearSendQueue();
	return TRUE;
}

void SocketClient::clearSendQueue()
{
	BSAutoLock lock(&m_mutexQueuePacks);
	while (m_queuePacks.size()>0)
	{
		QueuePackItem& item = m_queuePacks.front();
		delete [] item.buf;
		m_queuePacks.pop();
	}
	m_dwQueuedId = 0;
}

BOOL SocketClient::connectServer()
{
	if (m_socket!=NULL)
	{
		disconnectServer();
	}
	Log("[%s]开始连接到服务器", m_szClientName);

	ZeroMemory(&m_recvPack, sizeof(m_recvPack));

	m_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	ZeroMemory(&m_serverSockAddr, sizeof(m_serverSockAddr));
	m_serverSockAddr.sin_family=AF_INET;
	m_serverSockAddr.sin_port=htons((USHORT)m_dwServerPort);
	m_serverSockAddr.sin_addr.s_addr=inet_addr(m_szServerIP);

	//int timeout=15000;
	//if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout))==SOCKET_ERROR)
	//{
	//	BSLogger::SysLog(LOG_ERROR, "初始化socket连接失败");
	//	return FALSE;
	//}
	/*int timeout=60000;
	if(setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout))==SOCKET_ERROR)
	{
	BSLogger::SysLog(LOG_ERROR, "初始化认证Monitor连接设置失败 %d", WSAGetLastError());
	return FALSE;
	}*/

	m_monitorInfo.dwLastConnectTime = GetTickCount();
	ZeroMemory(&m_connInfo, sizeof(m_connInfo));

	if(connect(m_socket, (const struct sockaddr FAR*)&m_serverSockAddr, sizeof(m_serverSockAddr)) == SOCKET_ERROR)
	{
		int nErrorCode = WSAGetLastError();
		CString strMessage;
		strMessage.Format("[%s]连接 %s:%d 错误: %d", m_szClientName, m_szServerIP, m_dwServerPort, nErrorCode);
		closesocket(m_socket);
		m_socket=NULL;
		Log(strMessage);
		return FALSE;
	}

	onConnected();

	Log("[%s]SOCKET 已连接: %d", m_szClientName,  m_socket);
	return TRUE;
}

void SocketClient::disconnectServer()
{
	if (IsConnected())
	{
		Log("断开PerconService连接");
		closesocket(m_socket);
		m_socket = NULL;
		onDisconnected();
	}
}

BOOL SocketClient::IsConnected()
{
	return WaitForSingleObject(m_hConnected, 0)==WAIT_OBJECT_0;
}

BOOL SocketClient::IsRunning()
{
	return WaitForSingleObject(m_hIsRunning, 0)==WAIT_OBJECT_0;
}

void SocketClient::onConnected()
{
	m_connInfo.m_dwLastRecvBeatTime = GetTickCount();
	m_connInfo.m_dwLastSentBeatTime = GetTickCount();
	SetEvent(m_hConnected);
	if (m_pNotifier)
	{
		m_pNotifier->OnClientConnected();
	}
}

void SocketClient::onDisconnected()
{
	ResetEvent(m_hConnected);
	if (m_pNotifier)
	{
		m_pNotifier->OnClientDisconnected(-1, "连接已断开");
	}
}

void SocketClient::onSocketError(DWORD dwErrorCode, LPCTSTR szMsg)
{
	CString szErrorMsg;
	if (dwErrorCode==10004)
	{
		szErrorMsg.Format("[%s]连接已断开(%d) %s", m_szClientName, dwErrorCode, szErrorMsg);
	}
	else
	{
		szErrorMsg.Format("[%s]网络错误(%d) %s", m_szClientName, dwErrorCode, szErrorMsg);
	}
	BSLogger::SysLog(LOG_ERROR, szErrorMsg);
	//throw SocketException(szErrorMsg);
}

void SocketClient::SendData( BYTE* buf, int len, BOOL bDontQueue/*=FALSE*/ )
{
	BSAutoLock lock(&m_mutexQueuePacks);
	if(!IsRunning())
	{
		CString sz;
		sz.Format("[%s]离线状态不能发送数据包", m_szClientName);
		Log(sz);
		//throw BSException(sz);
		return;
	}
	if(bDontQueue)
	{
		sendQueuedData(buf, len);
	}
	else
	{
		QueuePackItem item;
		item.buf = new BYTE[len];
		memcpy_s(item.buf, len, buf, len);
		item.len = len;
		item.packId = ++m_dwQueuedId;
		m_queuePacks.push(item);
	}
}

void SocketClient::sendQueuedData(BYTE* buf, int len)
{
	BSAutoLock lock(&m_mutexQueuePacks);

	BYTE* sendbuf = new BYTE[len+4];
	*(DWORD*)(sendbuf) = len+4;
	memcpy_s(sendbuf+4, len, buf, len);
	int sendlen = len+4;

	DWORD dwBeginTime = GetTickCount();
	int nTotalSent = 0;
	while(TRUE)
	{
		int nSent2 = send(m_socket, (const char*)(sendbuf+nTotalSent), (sendlen-nTotalSent), 0);
		if (nSent2 == SOCKET_ERROR)
		{
			DWORD dwErrorCode = WSAGetLastError();
			onSocketError(dwErrorCode, "发送数据包错误");
			break;
		}
		nTotalSent += nSent2;
		if (nTotalSent>=sendlen)
		{
			break;
		}
		if (GetTickCount()-dwBeginTime>30000)
		{
			DWORD dwErrorCode = -1;
			onSocketError(dwErrorCode, "发送数据包超时");
			break;
		}
	}
	delete [] sendbuf;
}

void SocketClient::PostBSCmd( IBSCmd* pCmd )
{
	PostThreadMessage(m_dwCmdThreadId, WM_POST_BSCMD, (WPARAM)this, (LPARAM)pCmd);
}


DWORD SocketClient::CmdThread( PVOID pParam )
{
	// Init the msg queue
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	SocketClient* pClient = (SocketClient*)pParam;

	SetEvent(pClient->m_hThreadRunningEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pClient->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pClient->handleCmdProcess();
			Sleep(100);
		}
		catch (BSException& e)
		{
			pClient->Log("[%s]命令线程异常: %s", pClient->m_szClientName, e.GetErrorMessage());
		}
		catch (...)
		{
			pClient->Log("[%s]命令线程未知异常", pClient->m_szClientName);
		}
	}
	return 0;
}

int SocketClient::handleCmdProcess()
{
	if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
	{
		return 1;
	}
	// Only handle one msg each time
	MSG msg;
	while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (msg.message==WM_POST_BSCMD)
		{
			IBSCmd* pCmd = (IBSCmd*)(msg.lParam);
			if (pCmd)
			{
				Log("执行命令:%s", pCmd->GetName());
				pCmd->Execute();
			}
			delete pCmd;
		}
	}
	return 0;
}

DWORD SocketClient::MonitorThread( PVOID pParam )
{
	SocketClient* pClient = (SocketClient*)pParam;

	SetEvent(pClient->m_hThreadRunningEvent);

	while(TRUE)
	{
		try
		{
			if (WaitForSingleObject(pClient->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pClient->monitorProcess();
			Sleep(100);
		}
		catch(SocketException& e)
		{
			pClient->Log("[%s]监控线程网络错误: %s", pClient->m_szClientName, e.GetErrorMessage());
			Sleep(50);
		}
		catch (BSException& e)
		{
			pClient->Log("[%s]监控线程异常: %s", pClient->m_szClientName, e.GetErrorMessage());
			Sleep(100);
		}
		catch (...)
		{
			pClient->Log("[%s]监控线程未知错误", pClient->m_szClientName);
			Sleep(100);
		}
	}
	pClient->m_dwMonitorThreadId = 0;
	return 0;
}

int SocketClient::monitorProcess()
{
	if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
	{
		return -1;
	}

	if (IsRunning() && !IsConnected() 
		&& GetTickCount()-m_monitorInfo.dwLastConnectTime > 15000)
	{
		Log("[%s]未连接到服务器，尝试重新连接...", m_szClientName);
		connectServer();
		return -1;
	}

	if (IsConnected() && GetTickCount()-m_monitorInfo.dwLastCheckBeatTime>=s_CONFIG_HEARTBEAT_TIME/2)
	{
		if (GetTickCount()-m_connInfo.m_dwLastRecvBeatTime > s_CONFIG_HEARTBEAT_TIMEOUT)
		{
			Log("[%s]收心跳包超时, 连接已断开.", m_szClientName);
			onSocketError(-1, "收心跳包超时, 连接已断开");
			disconnectServer();
			return -1;
		}
		if(GetTickCount()-m_connInfo.m_dwLastSentBeatTime>= s_CONFIG_HEARTBEAT_TIME)
		{
			sendHeartBeat();
		}
		m_monitorInfo.dwLastCheckBeatTime = GetTickCount();
	}

	// 发送Queued数据包
	if (IsConnected())
	{

		while(IsConnected() && m_queuePacks.size()>0)
		{
			BSAutoLock sendLock(&m_mutexQueuePacks);
			if(IsConnected() && m_queuePacks.size()>0)
			{
				QueuePackItem& item = m_queuePacks.front();
				//Log("[%s]SendQueued %d len:%d", m_szClientName, item.packId, item.len);
				sendQueuedData(item.buf, item.len);
				m_queuePacks.pop();
				delete [] item.buf;
			}
		}
	}
	return 0;
}

DWORD SocketClient::WorkerThread( PVOID pParam )
{
	SocketClient* pClient = (SocketClient*)pParam;
	SetEvent(pClient->m_hThreadRunningEvent);

	while (TRUE)
	{
		try
		{
			if (WaitForSingleObject(pClient->m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				break;
			}
			pClient->WorkerProcess();
		}
		catch (SocketException& e)
		{
			BSLogger::SysLog(LOG_ERROR, "[%s] 收报线程网络异常 %s", pClient->m_szClientName, e.GetErrorMessage());
			Sleep(100);
		}
		catch (...)
		{
			BSLogger::SysLog(LOG_ERROR, "[%s] 收报线程未知异常", pClient->m_szClientName);
			AfxMessageBox("BSAuthClient 收报线程未知异常");
			Sleep(100);
		}
	}
	BSLogger::SysLog(LOG_INFO, "[%s] 收报线程退出", pClient->m_szClientName);
	return 0;
}

void SocketClient::WorkerProcess()
{
	int nErrorCode = 0;
	// recv 缓冲区长度
	static const DWORD RECV_BUFFER_SIZE = 0x5000;
	// 本次 recv 返回buffer
	BYTE recvBuffer[RECV_BUFFER_SIZE];
	// 本次 recv 返回buffer长度 
	int nRecvLen = 0;
	// 本次分包报文在当前recv到的buffer里可以读取的长度
	int nPackThisRead = 0;
	// Decode buffer
	char szOutBuf[RECV_BUFFER_SIZE] = {0};

	while (TRUE)
	{
		// 等待Socket建立和连接初始化
		HANDLE wHandles[] = { m_hConnected, m_hThreadExitEvent };
		int nWaitRet = WaitForMultipleObjects(2, wHandles, FALSE, INFINITE);

		// 检查线程是否需要退出
		if (nWaitRet==1)
		{
			return;
		}

		ZeroMemory(recvBuffer, RECV_BUFFER_SIZE);

		nRecvLen = recv(m_socket, (char*)recvBuffer, RECV_BUFFER_SIZE, 0);

		//TRACE("Recv result : %d\n", nRecvLen);

		if(WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
		{
			return;
		}

		// 错误处理
		if (nRecvLen==SOCKET_ERROR)
		{
			if (WaitForSingleObject(m_hThreadExitEvent, 0)==WAIT_OBJECT_0)
			{
				return;
			}
			nErrorCode = WSAGetLastError();
			TRACE("SocketRecvError:%d\n", nErrorCode);
			if(nErrorCode!=10004 && nErrorCode!=WSAEWOULDBLOCK)
			{
				onSocketError(nErrorCode, "收报错误");
				disconnectServer();
				Sleep(1000);
				break;
			}
			else
			{
				Sleep(100);
				continue;
			}
		}
		else if(nRecvLen==0)
		{
			Sleep(50);
			continue;
		}

		//Log("[SocketClient]recv %X len:%d", (DWORD)(*(WORD*)(recvBuffer)), nRecvLen);
		splitPack(&m_recvPack, recvBuffer, nRecvLen);
	}
}


void SocketClient::splitPack(BSClientPack* pGamePack, BYTE* recvBuffer, int nRecvLen)
{
	//TRACE("IntoSplitPack:%X len:%X\n:", *recvBuffer, nRecvLen);
	// 遍历RecvBuffer
	for (int i=0;i<nRecvLen;)
	{
		// 未取得当前包长度时
		if (pGamePack->nTotalLen<=0)
		{
			// 计算包头长度
			UINT nRealHeaderLen = BSClientPack::HEADER_LEN+pGamePack->dwExHeaderLen;
			// 当未收够包头长度时
			if (pGamePack->nReceivedLen <  nRealHeaderLen)
			{
				int tempcopylen = nRealHeaderLen - pGamePack->nReceivedLen;
				if (nRecvLen-i<tempcopylen)
				{
					tempcopylen=nRecvLen-i;
					memcpy(pGamePack->buffer+pGamePack->nReceivedLen, (recvBuffer+i), tempcopylen);
					i+=tempcopylen;
					pGamePack->nReceivedLen+=tempcopylen;
					continue;
				}
				//ZeroMemory(packBuffer, MAX_GAMEPACK_LEN);
				memcpy(pGamePack->buffer+pGamePack->nReceivedLen, (recvBuffer+i), tempcopylen);
				i+=tempcopylen;
				pGamePack->nReceivedLen+=tempcopylen;
			}
			int nLen = *(DWORD*)(pGamePack->buffer);
			//TRACE("GetPackLen:%X, len:%d\n", *(pGamePack->buffer), nLen);

			// 获取包长度（包括包头和包内容）
			pGamePack->nTotalLen = nLen;
			//TRACE("TotalLen:%d\n", nLen);
		}
		if ( pGamePack->nTotalLen<BSClientPack::HEADER_LEN+pGamePack->dwExHeaderLen)
		{
			//TODO: pack len Error Handling 
			ASSERT(FALSE);
			TRACE("ErrorPackLen:%d%X, len:%d\n", *(BYTE*)(pGamePack->buffer), pGamePack->nTotalLen);
			ZeroMemory(pGamePack, sizeof(*pGamePack));
			onSocketError(0, "分包错误");
			disconnectServer();
			break;
		}

		// 本次分包报文在当前recv到的buffer里可以读取的长度
		int nPackThisRead = (nRecvLen-i) < (pGamePack->nTotalLen-pGamePack->nReceivedLen) ? (nRecvLen-i) : (pGamePack->nTotalLen-pGamePack->nReceivedLen);

		//TRACE("PackThisRead:%d\n", nPackThisRead);

		if(pGamePack->nTotalLen>BSClientPack::MAX_LEN || pGamePack->nReceivedLen+nPackThisRead>BSClientPack::MAX_LEN)
		{
			// TODO: pack len too big, drop it.
			ASSERT(FALSE);
			pGamePack->nReceivedLen += nPackThisRead;
			i += nPackThisRead;
			TRACE("DropTooBigPack:%X, len:%d\n", *(BYTE*)(pGamePack->buffer), pGamePack->nTotalLen);
			onSocketError(0, "分包长度过大");
			disconnectServer();
			break;
		}

		memcpy(pGamePack->buffer+pGamePack->nReceivedLen, (recvBuffer+i), nPackThisRead);
		pGamePack->nReceivedLen += nPackThisRead;
		i += nPackThisRead;

		// 收到一个完整报文
		if (pGamePack->nReceivedLen == pGamePack->nTotalLen)
		{
			if(!onRecvPack(pGamePack->buffer+4, pGamePack->nTotalLen-4))
			{
				if (m_pNotifier)
				{
					m_pNotifier->OnClientRecvPack(pGamePack->buffer+4, pGamePack->nTotalLen-4);
				}
			}
			ZeroMemory(pGamePack, sizeof(BSClientPack));
		}
	}
}

BOOL SocketClient::onRecvPack( BYTE* buf, int len )
{
	WORD wType = *(WORD*)(buf);

	//Log("[SocketClient]onRecvPack %d len:%d", (DWORD)wType, len);

	if (wType==s_PACKTYPE_HEARTBEAT)
	{
		m_connInfo.m_dwLastRecvBeatTime = GetTickCount();
		return TRUE;
	}
	return FALSE;
}

void SocketClient::printMsg(LOG_LEVEL level, LPCTSTR szMsg )
{
	BSLogger::SysLog(level, szMsg);
}

void SocketClient::sendHeartBeat()
{
	BYTE buf[0x10];
	ZeroMemory(buf, sizeof(buf));
	*(WORD*)(buf) = s_PACKTYPE_HEARTBEAT;
	SendData(buf, 2);
	m_connInfo.m_dwLastSentBeatTime = GetTickCount();
}


void SocketClient::Log(LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start( vl, szFormat );

	TCHAR szResult[0x200] = {0};

	vsprintf_s(szResult, 0x200, szFormat, vl);

	if(m_pNotifier)
	{
		m_pNotifier->OnClientPrintMsg(szResult);
	}
	else
	{
		BSLogger::SysLog(LOG_INFO, szResult);
	}

	va_end( vl);
}
