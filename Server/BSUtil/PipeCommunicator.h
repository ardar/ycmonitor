#pragma once

class PipeCommunicator
{
public:
	PipeCommunicator(LPCTSTR szPipeName);
	virtual ~PipeCommunicator(void);
	BOOL Connect();
	void Disconnect();
	BOOL IsConnected();
	void SendMsg(DWORD dwMsgId, BYTE* buf, int len);
private:
	CString m_szPipeName;
	HANDLE m_hPipe;
};
