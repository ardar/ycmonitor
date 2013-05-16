#pragma once

class IService
{
public:
	virtual void QueueCmd(IBSCmd* pCmd) = 0;
	virtual BOOL SendToSession(DWORD dwSessionId, ISocketPack* pPack) = 0;
	virtual void CloseSession(DWORD dwSessionId) = 0;
};