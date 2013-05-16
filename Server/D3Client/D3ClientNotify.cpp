#include "stdafx.h"
#include "D3ClientEvent.h"

void D3Client::OnClientConnected()
{
	Log("已连接到上级服务器");
	if(m_pOnClientCallback)
	{
		m_pOnClientCallback(D3Event_Connected,0,0,0);
	}
}

void D3Client::OnClientDisconnected( DWORD dwErrorCode, LPCTSTR szErrorMsg )
{
	Log("到上级服务器连接已断开");
	if(m_pOnClientCallback)
	{
		m_pOnClientCallback(D3Event_Disconnected,0,0,0);
	}
}

void D3Client::OnClientNotifyMsg(DWORD dwMsg, WPARAM wParam, LPARAM lParam)
{
}

void D3Client::OnClientRecvPack( BYTE* buf, int len )
{
	WORD wType = *(WORD*)(buf);
	buf = buf+2;
	len = len-2;
	switch (wType)
	{
	case D3SC_AUTH_RESULT: onRecvAuthResult(buf, len); break;
	case D3SC_ACTION_RESULT: onRecvActionResult(buf, len); break;
	case D3SC_GETMONEY_RESULT: onRecvGetMoneyResult(buf, len); break;
	case D3SC_GETDEPOT_RESULT: onRecvDepotItems(buf, len); break;
	case D3SC_GETFINISHED_RESULT: onRecvFinishedItems(buf, len); break;
	case D3SC_GETSELLING_RESULT: onRecvSellingItems(buf, len); break;
	case D3SC_GETBIDDING_RESULT: onRecvBiddingItems(buf, len); break;
	case D3SC_SEARCHITEMS_RESULT: onRecvSearchItems(buf, len); break;
	case D3SC_GETSCHEDULE_RESULT: onRecvGetSchedules(buf, len); break;
	case D3SC_SCHEDULE_UPDATED: onRecvScheduleUpdated(buf, len); break;
	default: break;
	}
}

void D3Client::OnClientPrintMsg( LPCTSTR szMsg )
{
	Log(szMsg);
}


