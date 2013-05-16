#include "stdafx.h"
#include "D3Service.h"

void D3Service::UISysLog(LPCTSTR szLog)
{
	if (m_pUIProxy)
	{
		m_pUIProxy->SysLog(LOG_INFO, 0, szLog);
	}
}

void D3Service::UIErrorLog(DWORD dwDeviceId, DWORD dwErrorCode, LPCTSTR szLog)
{
	if (m_pUIProxy)
	{
		m_pUIProxy->SysLog(LOG_ERROR, dwDeviceId, szLog);
	}
}

void D3Service::UIRefreshService()
{
	if (m_pUIProxy)
	{
		m_pUIProxy->UpdateService(m_setting, m_pServer->IsRunning());
	}
}

void D3Service::UIRefreshDevices()
{
	if (!m_pUIProxy)
	{
		return;
	}
	m_pUIProxy->ClearClients();
	/*for (int i=0;i<m_mapClients.GetCount();i++)
	{
		DeviceNode node;
		if(m_mapClients.GetByIndex(i, node))
		{
			m_pUIProxy->UpdateDevice(node);
		}
	}*/
}

void D3Service::UIDeleteDevice(LPCTSTR szDeviceMac)
{
	if (!m_pUIProxy)
	{
		return;
	}
	/*if (m_mapClients.GetAt(szDeviceMac, node))
	{
		m_pUIProxy->RemoveDevice(node.DeviceId);
	}*/
}

void D3Service::UIRefreshDevice(LPCTSTR szDeviceMac)
{
	if (!m_pUIProxy)
	{
		return;
	}
	/*if (m_mapClients.GetAt(szDeviceMac, node))
	{
		m_pUIProxy->UpdateDevice(node);
	}*/
}