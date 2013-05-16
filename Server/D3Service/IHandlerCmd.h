#pragma once
// 
class _declspec(dllexport) IHandlerCmd : public IBSCmd
{
public:
	DWORD m_dwSessionId;
	DWORD m_dwQueueTime;

	virtual ~IHandlerCmd(){/*TRACE("IHandlerCmd::Desctry\n");*/};
	virtual CString GetName() { return CString("IHandlerCmd"); };
	virtual IHandlerCmd* CreateInstance() = 0;
	virtual void ReadPack(DWORD dwSessionId, BYTE* buf, int len) = 0;

	virtual DWORD GetQueueTime() { return m_dwQueueTime; };
	virtual DWORD GuessExecTime() { return 500; };
	virtual void Execute() = 0;
	virtual void Cancel(DWORD dwErrorCode) = 0;
};