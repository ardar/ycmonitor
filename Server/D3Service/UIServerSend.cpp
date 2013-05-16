#include "stdafx.h"
#include "UIServer.h"

void UIServer::sendSysMsg(SocketSession* pSession, LOG_LEVEL level, DWORD dwDeviceId, LPCTSTR szMsg)
{
	int nMsgLen = BSHelper::bsstrlen(szMsg)+1;
	BYTE* buf = new BYTE[0x10+nMsgLen];
	ZeroMemory(buf, 0x10+nMsgLen);
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_SERVICE2UI::WM_MS_SYSMSG;
	nIndex+=2;
	*(DWORD*)(buf+nIndex) = (DWORD)level;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = (DWORD)dwDeviceId;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = nMsgLen;
	nIndex+=4;
	memcpy_s(buf+nIndex, 0x10+nMsgLen-nIndex, szMsg, nMsgLen-1);
	nIndex+=nMsgLen;
	SendData(pSession->SessionId, buf, nIndex);

	delete [] buf;
}
void UIServer::sendServiceInfo(SocketSession* pSession, const Monitor_ServiceInfo& info)
{
	BYTE buf[0x500];
	ZeroMemory(buf, sizeof(buf));
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_SERVICE2UI::WM_MS_SERVICE_UPDATED;
	nIndex+=2;
	memcpy_s(buf+nIndex, sizeof(buf)-nIndex, &info, sizeof(info));
	nIndex+=sizeof(info);
	SendData(pSession->SessionId, buf, nIndex);
}
void UIServer::sendDeviceInfo(SocketSession* pSession, const Monitor_ClientInfo& info)
{
	BYTE buf[0x500];
	ZeroMemory(buf, sizeof(buf));
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_SERVICE2UI::WM_MS_DEVICE_UPDATED;
	nIndex+=2;
	memcpy_s(buf+nIndex, sizeof(buf)-nIndex, &info, sizeof(info));
	nIndex+=sizeof(info);
	SendData(pSession->SessionId, buf, nIndex);
}
void UIServer::sendDeviceDeleted(SocketSession* pSession, DWORD dwDeviceId)
{
	BYTE buf[0x20];
	ZeroMemory(buf, sizeof(buf));
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_SERVICE2UI::WM_MS_DEVICE_REMOVED;
	nIndex+=2;
	*(DWORD*)(buf+nIndex) = dwDeviceId;
	nIndex+=4;
	SendData(pSession->SessionId, buf, nIndex);
}
void UIServer::sendDeviceCleared(SocketSession* pSession)
{
	BYTE buf[0x20];
	ZeroMemory(buf, sizeof(buf));
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_SERVICE2UI::WM_MS_DEVICE_CLEARED;
	nIndex+=2;
	SendData(pSession->SessionId, buf, nIndex);
}