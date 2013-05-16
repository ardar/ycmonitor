#pragma once
#include "D3Service.h"

class _declspec(dllexport) UIServer  : public SocketServer , public IUIProxy
{
public:
	UIServer(D3Service* pService);
	virtual void UpdateService( ServiceSetting& setting, BOOL bServerListening);
	virtual void UpdateClient( const D3ServiceSession& session );
	virtual void RemoveClient(DWORD dwDeviceId);
	virtual void ClearClients();
	virtual void SysLog(LOG_LEVEL level, DWORD dwDeviceId, LPCTSTR szLog);
protected:
	// virtual Members
	virtual BOOL onRecvPack(SocketSession* pSession, BYTE* buf, int len);
private:
	D3Service* m_pService;

	void convertServiceInfo( ServiceSetting& setting, BOOL bServerListening, Monitor_ServiceInfo* pInfo );
	void convertClientInfo( const D3ServiceSession& session, Monitor_ClientInfo* pInfo );
	// recv pack
	void onRecvRefreshService(SocketSession* pSession, BYTE* buf, int len);
	void onRecvRefreshDevices(SocketSession* pSession, BYTE* buf, int len);
	// send pack
	void sendServiceInfo(SocketSession* pSession, const Monitor_ServiceInfo& info);
	void sendDeviceInfo(SocketSession* pSession, const Monitor_ClientInfo& info);
	void sendDeviceDeleted(SocketSession* pSession, DWORD dwDeviceId);
	void sendDeviceCleared(SocketSession* pSession);
	void sendSysMsg(SocketSession* pSession, LOG_LEVEL level, DWORD dwDeviceId, LPCTSTR szMsg);
};
