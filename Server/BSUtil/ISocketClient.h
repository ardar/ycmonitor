#pragma once

class ISocketClientNotify
{
public:
	virtual void OnClientConnected() = 0;
	virtual void OnClientDisconnected(DWORD dwErrorCode, LPCTSTR szErrorMsg) = 0;
	virtual void OnClientRecvPack(BYTE* buf, int len) = 0;
	virtual void OnClientNotifyMsg(DWORD dwMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnClientPrintMsg(LPCTSTR szMsg) = 0;
};

class ISocketClient
{
public:
	virtual BOOL InitClient(LPCTSTR szIdentifier, LPCTSTR szServerUri, ISocketClientNotify* pCallback) = 0;
	virtual BOOL StartClient() = 0;
	virtual BOOL StopClient() = 0;
	virtual BOOL IsConnected() = 0;
	virtual void SendData(BYTE* buf, int len, BOOL bSendImediatly)= 0;
};