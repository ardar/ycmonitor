#pragma once

struct PcSessionInfo
{
	DWORD SessionId;
	TCHAR ClientIP[64];
	DWORD ClientPort;
	PcSessionInfo()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

class ISocketServerNotify
{
public:
	virtual void OnSessionConnected(const PcSessionInfo* pSession) = 0;
	virtual void OnSessionDisconnected(const PcSessionInfo* pSession) = 0;
	virtual void OnSessionNotifyMsg(
		const PcSessionInfo* pSession, DWORD dwMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnSessionPrintMsg(const PcSessionInfo* pSession, LPCTSTR szMsg) = 0;
	virtual void OnSessionRecvPack(const PcSessionInfo* pSession, BYTE* buf, int len) = 0;
};


class ISocketServer
{
public:
	virtual BOOL InitService(LPCTSTR szIdentifier, LPCTSTR szServerUri, ISocketServerNotify* pCallback) = 0;
	virtual BOOL StartService() = 0;
	virtual BOOL StopService() = 0;
	virtual BOOL IsRunning() = 0;
	virtual void SendData(DWORD dwSessionId, BYTE* buf, int len, BOOL bSendImediatly)= 0;
	virtual void CloseSession(DWORD dwSessionId) = 0;
};