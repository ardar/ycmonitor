#pragma once

class _declspec(dllexport) IUIProxy
{
public:
	virtual ~IUIProxy(){};
	virtual void UpdateService( ServiceSetting& setting, BOOL bServerListening) = 0;
	virtual void UpdateClient(const D3ServiceSession& session) = 0;
	virtual void RemoveClient(DWORD dwDeviceId) = 0;
	virtual void ClearClients() = 0;
	virtual void SysLog(LOG_LEVEL level, DWORD dwDeviceId, LPCTSTR szLog) = 0;

};
