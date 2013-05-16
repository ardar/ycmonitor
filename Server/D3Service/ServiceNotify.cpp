#include "stdafx.h"
#include "D3Service.h"


void D3Service::OnSessionConnected(const PcSessionInfo* pSession)
{
	CreateSession(pSession->SessionId, *pSession);
	Log("[%d][%s]SessionÁ¬½Ó %s", pSession->SessionId, pSession->ClientIP);
}

void D3Service::OnSessionDisconnected(const PcSessionInfo* pSession)
{
	DeleteSession(pSession->SessionId);
	Log("[%d][%s]Session¶Ï¿ª %s", pSession->SessionId, pSession->ClientIP);
}

void D3Service::OnSessionRecvPack(const PcSessionInfo* pSessionInfo, BYTE* buf, int len)
{
	WORD wType = *(DWORD*)(buf);
	buf = buf+2;
	len = len-2;
	IHandlerCmd* pHandler = NULL;
	if(m_mapPackHandler.GetAt(wType, pHandler))
	{
		IHandlerCmd* pCmd = pHandler->CreateInstance();
		pCmd->ReadPack(pSessionInfo->SessionId, buf, len);

		D3ServiceSession session;
		if (GetSession(pSessionInfo->SessionId, session))
		{
			session.dwBeginTime = time(0);
			strcpy_s(session.szLastCmd, sizeof(session.szLastCmd), pCmd->GetName());
		}

		QueueCmd(pCmd);
	}
}

void D3Service::OnSessionNotifyMsg(const PcSessionInfo* pSession, DWORD dwMsg, WPARAM wParam, LPARAM lParam)
{
}

void D3Service::OnSessionPrintMsg(const PcSessionInfo* pSession, LPCTSTR szMsg)
{
	if (pSession)
	{
		Log("[%d][%s]:%s", pSession->SessionId, pSession->ClientIP, szMsg);
	}
	else
	{
		Log("%s", szMsg);
	}
}
