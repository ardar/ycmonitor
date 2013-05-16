#include "stdafx.h"
#include "UIServer.h"

UIServer::UIServer(D3Service* pService)
:SocketServer("UIServer.Socket"), m_pService(pService)
{
	
}

void UIServer::UpdateService( ServiceSetting& setting,  BOOL bServerListening)
{
	BSAutoLock lock(m_mapSession.GetMutex());
	int nCount = m_mapSession.GetCount();
	if (nCount==0)
	{
		return;
	}
	Monitor_ServiceInfo serviceInfo;
	convertServiceInfo(setting, bServerListening, &serviceInfo);
	for (int i=0;i<nCount;i++)
	{
		SocketSession* pSession;
		if(m_mapSession.GetByIndex(i, pSession))
		{
			sendServiceInfo(pSession, serviceInfo);
		}
	}
}

void UIServer::UpdateClient( const D3ServiceSession& session )
{
	BSAutoLock lock(m_mapSession.GetMutex());
	int nCount = m_mapSession.GetCount();
	if (nCount==0)
	{
		return;
	}
	Monitor_ClientInfo info;
	convertClientInfo(session, &info);
	for (int i=0;i<nCount;i++)
	{
		SocketSession* pSession;
		if(m_mapSession.GetByIndex(i, pSession))
		{
			sendDeviceInfo(pSession, info);
		}
	}
}

void UIServer::RemoveClient( DWORD dwDeviceId )
{
	BSAutoLock lock(m_mapSession.GetMutex());
	int nCount = m_mapSession.GetCount();
	for (int i=0;i<nCount;i++)
	{
		SocketSession* pSession;
		if(m_mapSession.GetByIndex(i, pSession))
		{
			sendDeviceDeleted(pSession, dwDeviceId);
		}
	}
}

void UIServer::ClearClients()
{
	BSAutoLock lock(m_mapSession.GetMutex());
	int nCount = m_mapSession.GetCount();
	if (nCount==0)
	{
		return;
	}
	for (int i=0;i<nCount;i++)
	{
		SocketSession* pSession;
		if(m_mapSession.GetByIndex(i, pSession))
		{
			sendDeviceCleared(pSession);
		}
	}
}

void UIServer::SysLog( LOG_LEVEL level, DWORD dwDeviceId, LPCTSTR szLog )
{
	BSAutoLock lock(m_mapSession.GetMutex());
	int nCount = m_mapSession.GetCount();
	if (nCount==0)
	{
		return;
	}
	for (int i=0;i<nCount;i++)
	{
		SocketSession* pSession;
		if(m_mapSession.GetByIndex(i, pSession))
		{
			sendSysMsg(pSession, level, dwDeviceId, szLog);
		}
	}
}

void UIServer::convertServiceInfo( ServiceSetting& setting, BOOL bServerListening, Monitor_ServiceInfo* pInfo )
{
	pInfo->bIsServerRunning = bServerListening;
	strcpy_s(pInfo->szListenUri, sizeof(pInfo->szListenUri), setting.szListenUri.GetBuffer());
	strcpy_s(pInfo->szCurrGameAccount, sizeof(pInfo->szCurrGameAccount), setting.szGameAccount.GetBuffer());
}

void UIServer::convertClientInfo( const D3ServiceSession& session, Monitor_ClientInfo* pInfo )
{
	pInfo->dwSessionId = session.dwSessionid;
	strcpy_s(pInfo->szClientIP, sizeof(pInfo->szClientIP), session.szClientIP);
	pInfo->dwClientPort = session.dwClientPort;
	pInfo->dwClientPort = session.dwClientPort;
	strcpy_s(pInfo->szServiceUser, sizeof(pInfo->szServiceUser), session.szUsername);
	strcpy_s(pInfo->szLastOperation, sizeof(pInfo->szLastOperation), session.szLastCmd);
	pInfo->dwLastOpTime = session.dwLastRecvTime;
	pInfo->bIsAuthenticated = session.bIsAuthenticated;
}

BOOL UIServer::onRecvPack( SocketSession* pSession, BYTE* buf, int len )
{
	if(SocketServer::onRecvPack(pSession, buf, len))
	{
		return TRUE;
	}
	WORD wType = *(WORD*)(buf);
	BYTE* databuf = buf+2;
	int datalen = len-2;
	switch (wType)
	{
	case WM_UI2SERVICE::WM_REFRESH_SERVICE:
		onRecvRefreshService(pSession, databuf, datalen);
		return TRUE;
		break;
	case WM_UI2SERVICE::WM_REFRESH_DEVICES:
		onRecvRefreshDevices(pSession, databuf, datalen);
		return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}
