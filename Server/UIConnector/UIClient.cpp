#include "StdAfx.h"
#include "UIClient.h"

UIClient::UIClient(void)
: SocketClient("UIClient"), m_hMainWnd(NULL)
{
}

UIClient::~UIClient(void)
{
}


void UIClient::SetMainWnd( HWND hMainWnd )
{
	ASSERT(hMainWnd);
	m_hMainWnd = hMainWnd;
}

BOOL UIClient::onRecvPack( BYTE* buf, int len )
{
	if(SocketClient::onRecvPack(buf, len))
	{
		return TRUE;
	}
	WORD wType = *(WORD*)(buf);
	BYTE* databuf = buf+2;
	int datalen = len-2;
	switch (wType)
	{
	case WM_SERVICE2UI::WM_MS_SYSMSG:
		onRecvSysLog(databuf, datalen);
		return TRUE;
		break;
	case WM_SERVICE2UI::WM_MS_SERVICE_UPDATED:
		onRecvServiceUpdated(databuf, datalen);
		return TRUE;
		break;
	case WM_SERVICE2UI::WM_MS_DEVICE_CLEARED:
		onRecvDevicesCleared(databuf, datalen);
		return TRUE;
		break;
	case WM_SERVICE2UI::WM_MS_DEVICE_UPDATED:
		onRecvDeviceUpdated(databuf, datalen);
		return TRUE;
		break;
	case WM_SERVICE2UI::WM_MS_DEVICE_REMOVED:
		onRecvDeviceRemoved(databuf, datalen);
		return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}


void UIClient::RefreshService()
{
	sendRefreshService();
}

void UIClient::RefreshDevices()
{
	sendRefreshDevices();
}

void UIClient::onConnected()
{
	SocketClient::onConnected();
	RefreshService();
	RefreshDevices();
}

void UIClient::onDisconnected()
{
	PostMessage(m_hMainWnd, WM_SERVICE2UI::WM_MS_SERVICE_UPDATED, (WPARAM)0, (LPARAM)NULL);
	onRecvDevicesCleared(NULL, 0);
	SocketClient::onDisconnected();
}
