#pragma once
#include "..\BSUtil\IBSCmd.h"
#include <queue>
using namespace std;

class SocketSession
{
public:
	DWORD SessionId;
	PcSessionInfo Info;
	SOCKET ClientSocket;
	SOCKADDR_IN ClientSockAddr;
	BSNetPack Pack;

	DWORD SendCount;
	DWORD RecvCount;
	DWORD ConnectTime;
	DWORD LastSentBeatTime;
	DWORD LastRecvBeatTime;

	SocketSession(DWORD dwSessionId, SOCKET socket)
		: Pack(0x40000, 0x1000, 4)
	{
		ZeroMemory(&ClientSockAddr, sizeof(ClientSockAddr));
		ZeroMemory(&Info, sizeof(Info));
		SessionId = dwSessionId;
		Info.SessionId = SessionId;
		ClientSocket = socket;
		SendCount = RecvCount = ConnectTime = LastSentBeatTime = LastRecvBeatTime = 0;
	}
	~SocketSession()
	{
	}
};



class _declspec(dllexport) SocketServer : public ISocketServer
{
public:

	SocketServer(LPCTSTR szServerName);
	virtual ~SocketServer(void);
	virtual BOOL InitService(LPCTSTR szIdentifier, LPCTSTR szServerUri, ISocketServerNotify* pCallback);
	virtual BOOL StartService();
	virtual BOOL StopService();
	virtual BOOL IsRunning();
	virtual void SendData( DWORD dwSessionId, BYTE* buf, int len, BOOL bDontQueue=FALSE );
	virtual void CloseSession(DWORD dwSessionId);
	// 发送Monitor线程中执行的命令
	virtual void PostBSCmd(IBSCmd* pCmd);

	void Log(LPCTSTR szFormat, ...);
protected:
	virtual DWORD handlePackage();
	virtual SocketSession* createSession( DWORD dwSessionId, SOCKET clientSocket, SOCKADDR_IN& clientAddr );
	virtual void deleteSession(DWORD dwSessionid);

	virtual void printSocketError(SocketSession* pSession, DWORD dwErrorCode, LPCTSTR szMsg=NULL);
	virtual void onSessionCreated(SocketSession* pSession);
	virtual void onSessionDeleted( SocketSession* pSession);
	virtual BOOL onRecvPack(SocketSession* pSession, BYTE* buf, int len);

	ISocketServerNotify* m_pNotifier;

	CString m_szIdentifier;
	CString m_szServerName;
	CString m_szServerIP;
	DWORD m_dwServerPort;
	BSMap<DWORD, DWORD, SocketSession*, SocketSession*> m_mapSession;

private:
	enum WM_MSG
	{
		WM_POST_BSCMD
	};

	enum IOCP_OPERATION
	{
		IOCP_EXIT_POSTED,
		IOCP_DISCONNECT,
		IOCP_RECV_POSTED,
		IOCP_SEND_POSTED
	};
	struct PER_IO_OPERATION_DATA
	{
		WSAOVERLAPPED overlap;
		WSABUF Buffer;
		DWORD BufferLen;
		DWORD NumberOfBytesProcessed;
		DWORD Flags;
		SocketServer::IOCP_OPERATION OperationType;
		PER_IO_OPERATION_DATA()
		{
			ZeroMemory(this, sizeof(*this));
		}
		~PER_IO_OPERATION_DATA()
		{
			if (Buffer.buf && Buffer.len>0)
			{
				delete [] Buffer.buf;
			}
		}
	};

	static DWORD WINAPI IOCPWorkerThread(LPVOID CompletionPortID);
	static DWORD WINAPI ListenThread(PVOID pParam);
	static DWORD WINAPI MonitorThread(PVOID pParam);
	static DWORD WINAPI HandleThread(PVOID pParam);

	void iocpWorkerProcess();
	void monitorProcess();

	// IOCP
	HANDLE m_CompletionPort;
	SOCKET m_listenSocket;

	HANDLE m_hThreadExitEvent;
	HANDLE m_hThreadStartupEvent;
	HANDLE m_hAllStartedEvent;
	// Listen accept 
	HANDLE m_hListenThread;
	// Worker thread for receiving event from IOCP
	HANDLE m_hIocpWorkerThread;
	// Monitor send queue and heart beat
	HANDLE m_hMonitorThread;
	DWORD m_dwMonitorThreadId;
	// Cmd execute
	HANDLE m_hHandleThread;
	DWORD m_dwHandleThreadId;
	// UI
	HWND m_hMainWnd;

	static const int s_CONFIG_HEARTBEAT_TIME = 6000;
	static const int s_CONFIG_HEARTBEAT_TIMEOUT = 60000;
	static const WORD s_PACKTYPE_HEARTBEAT = 0;

	// Send pack Queue
	DWORD m_dwSendQueuedId;
	struct QueuePackItem
	{
		DWORD SessionId;
		BYTE* buf;
		int len;
		DWORD packId;
		QueuePackItem()
		{
			ZeroMemory(this, sizeof(QueuePackItem));
		};
	};
	queue<QueuePackItem> m_queueSendPacks;
	CMutex m_mutexQueueSendPacks;
	queue<QueuePackItem> m_queueRecvPacks;
	CMutex m_mutexQueueRecvPacks;

	void processBuffer(SocketSession* pSession, BYTE* recvBuffer, int nRecvLen);
	void sendQueuedData(SocketSession* pSession, const BYTE* buf, int len);
	void clearSendQueue();
	void clearRecvQueue();
	void clearSessions();
	void postIocpOpStatus(DWORD dwSessionId, IOCP_OPERATION operationType);

	// 发包函数
	void sendHeartBeat( SocketSession* pSession );
};
