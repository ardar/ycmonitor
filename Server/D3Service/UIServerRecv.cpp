#include "stdafx.h"
#include "UIServer.h"

void UIServer::onRecvRefreshService(SocketSession* pSession, BYTE* buf, int len)
{
	m_pService->UIRefreshService();
	
}

void UIServer::onRecvRefreshDevices( SocketSession* pSession, BYTE* buf, int len )
{
	m_pService->UIRefreshDevices();
}

