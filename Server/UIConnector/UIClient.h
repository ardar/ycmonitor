#pragma once
#include "../BSUtil/BSUtil.h"
#include "../BSUtil/SocketClient.h"
#include "../D3Common/D3Common.h"

class _declspec(dllexport) UIClient : public SocketClient
{
public:
	UIClient(void);
	virtual ~UIClient(void);

	void RefreshService();
	void RefreshDevices();
	
	void SetMainWnd(HWND hMainWnd);

protected:
	// virtual Members
	virtual BOOL onRecvPack(BYTE* buf, int len);
	virtual void onConnected();
	virtual void onDisconnected();
private:
	CString m_szServiceUIUri;
	HWND m_hMainWnd;

	void onRecvServiceUpdated(BYTE* buf, int len);
	void onRecvDeviceUpdated(BYTE* buf, int len);
	void onRecvDeviceRemoved(BYTE* buf, int len);
	void onRecvDevicesCleared(BYTE* buf, int len);
	void onRecvSysLog(BYTE* buf, int len);

	void sendRefreshService();
	void sendRefreshDevices();
};
