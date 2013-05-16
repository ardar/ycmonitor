#include "stdafx.h"
#include "UIClient.h"

void UIClient::onRecvSysLog(BYTE* buf, int len)
{
	if (m_hMainWnd)
	{
		if (len<12)
		{
			ASSERT(FALSE);
			return;
		}
		UINT nIndex = 0;
		LOG_LEVEL level = (LOG_LEVEL)*(DWORD*)(buf+nIndex);
		nIndex+=4;
		DWORD dwDeviceId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		DWORD dwLen = *(DWORD*)(buf+nIndex);
		nIndex+=4;

		CString* pStrLog = new CString((char*)(buf+nIndex));
		PostMessage(m_hMainWnd, WM_SERVICE2UI::WM_MS_SYSMSG, (WPARAM)dwDeviceId, (LPARAM)pStrLog);
	}
}

void UIClient::onRecvServiceUpdated(BYTE* buf, int len)
{
	if (m_hMainWnd)
	{
		int structlen = sizeof(Monitor_ServiceInfo);
		if (len<structlen)
		{
			ASSERT(FALSE);
			//return;
		}
		Monitor_ServiceInfo* pInfo = new Monitor_ServiceInfo();
		memcpy_s(pInfo, sizeof(Monitor_ServiceInfo), buf, sizeof(Monitor_ServiceInfo));
		PostMessage(m_hMainWnd, WM_SERVICE2UI::WM_MS_SERVICE_UPDATED, (WPARAM)0, (LPARAM)pInfo);
	}
}

void UIClient::onRecvDeviceUpdated(BYTE* buf, int len)
{
	if (m_hMainWnd)
	{
		if (len<sizeof(Monitor_ClientInfo))
		{
			ASSERT(FALSE);
			return;
		}
		Monitor_ClientInfo* pInfo = new Monitor_ClientInfo();
		memcpy_s(pInfo, sizeof(Monitor_ClientInfo), buf, sizeof(Monitor_ClientInfo));
		PostMessage(m_hMainWnd, WM_SERVICE2UI::WM_MS_DEVICE_UPDATED, (WPARAM)pInfo->dwSessionId, (LPARAM)pInfo);
	}
}

void UIClient::onRecvDeviceRemoved(BYTE* buf, int len)
{
	if (m_hMainWnd)
	{
		if (len<sizeof(DWORD))
		{
			ASSERT(FALSE);
			return;
		}
		DWORD dwDeviceId = *(DWORD*)(buf);
		PostMessage(m_hMainWnd, WM_SERVICE2UI::WM_MS_DEVICE_UPDATED, (WPARAM)dwDeviceId, (LPARAM)0);
	}
}

void UIClient::onRecvDevicesCleared(BYTE* buf, int len)
{
	if (m_hMainWnd)
	{
		PostMessage(m_hMainWnd, WM_SERVICE2UI::WM_MS_DEVICE_CLEARED, (WPARAM)0, (LPARAM)0);
	}
}