#pragma once
#include "..\BSUtil\IBSCmd.h"
#include "..\BSUtil\ISocketClient.h"
#include <queue>
using namespace std;

// ����Buffer�ṹ 
struct BSClientPack
{
	// ����ĳ���
	static const int MAX_LEN = 0x40000;
	// ���İ�ͷ����
	static const int HEADER_LEN = 6;
	// ����type
	DWORD dwPackType;
	// ������չ��ͷ����
	DWORD dwExHeaderLen;
	// �����ܳ���
	int nTotalLen;
	// �������յ�����
	int nReceivedLen;
	// ����buffer
	BYTE buffer[MAX_LEN];
};

class SocketClient : public ISocketClient
{
public:
	static const int LOW_PACKTYPE_RANGE = 0xFF;
	SocketClient(LPCTSTR szClientName);
	~SocketClient(void);

	//static PerconClient& Instance();
	virtual BOOL InitClient(LPCTSTR szIdentifier, LPCTSTR szServerUri, ISocketClientNotify* pNotifier);
	// ����
	virtual BOOL StartClient();
	// �Ͽ�
	virtual BOOL StopClient();

	// �Ƿ���������
	virtual BOOL IsConnected();
	// �������ݰ�
	virtual void SendData(BYTE* buf, int len, BOOL bDontQueue=FALSE );

	// ����Monitor�߳���ִ�е�����
	virtual void PostBSCmd(IBSCmd* pCmd);

	BOOL IsRunning();
protected:
	virtual BOOL connectServer();
	virtual void disconnectServer();
	virtual BOOL onRecvPack(BYTE* buf, int len);
	virtual void onConnected();
	virtual void onDisconnected();

	virtual void onSocketError(DWORD dwErrorCode, LPCTSTR szMsg );

	// ������߼�
	virtual int handleCmdProcess();
	// �������̼߳���߼�
	virtual int monitorProcess();

	void Log(LPCTSTR szFormat, ...);

	ISocketClientNotify* m_pNotifier;

	CString m_szIdentifier;
	CString m_szClientName;
	CString m_szServerIP;
	DWORD m_dwServerPort;

private:
	enum WM_MSG
	{
		WM_POST_BSCMD
	};

	static DWORD WINAPI CmdThread(PVOID pParam);
	static DWORD WINAPI MonitorThread(PVOID pParam);
	static DWORD WINAPI WorkerThread(PVOID pParam);
	void WorkerProcess();

	void printMsg(LOG_LEVEL level, LPCTSTR szMsg );

	void splitPack(BSClientPack* pBSClientPack, BYTE* recvBuffer, int nRecvLen);
	void sendQueuedData(BYTE* buf, int len);
	void clearSendQueue();
	void sendHeartBeat();

	SOCKADDR_IN m_serverSockAddr;

	// Send pack Queue
	DWORD m_dwQueuedId;
	struct QueuePackItem
	{
		BYTE* buf;
		int len;
		DWORD packId;
	};
	queue<QueuePackItem> m_queuePacks;
	CMutex m_mutexQueuePacks;

	SOCKET m_socket;
	BSClientPack m_recvPack;
	BOOL m_bThreadContinue;
	HANDLE m_hThreadRunningEvent;
	HANDLE m_hThreadExitEvent;
	HANDLE m_hIsRunning;
	HANDLE m_hConnected;
	// Cmd
	HANDLE m_hCmdThread;
	DWORD m_dwCmdThreadId;
	// Monitor
	HANDLE m_hMonitorThread;
	DWORD m_dwMonitorThreadId;
	// Recv
	HANDLE m_hRecvThread;

	static const int s_CONFIG_HEARTBEAT_TIME = 6000;
	static const int s_CONFIG_HEARTBEAT_TIMEOUT = 60000;
	static const WORD s_PACKTYPE_HEARTBEAT = 0;

	struct SocketClientMonitorInfo
	{
		DWORD dwLastCheckBeatTime;
		DWORD dwLastConnectTime;
		SocketClientMonitorInfo()
		{
			ZeroMemory(this, sizeof(SocketClientMonitorInfo));
		}
	};
	SocketClientMonitorInfo m_monitorInfo;
	struct ConnInfo
	{
		DWORD m_dwSendCount;
		DWORD m_dwRecvCount;
		DWORD m_dwLastSentBeatTime;
		DWORD m_dwLastRecvBeatTime;
		ConnInfo()
		{
			ZeroMemory(this, sizeof(ConnInfo));
		}
	};
	ConnInfo m_connInfo;
};
