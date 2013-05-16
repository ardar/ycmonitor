#include "stdafx.h"
#include "UIClient.h"

void UIClient::sendRefreshService()
{
	BYTE buf[0x10];
	ZeroMemory(buf, sizeof(buf));
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_UI2SERVICE::WM_REFRESH_SERVICE;
	nIndex+=2;
	SendData(buf, nIndex);
}
void UIClient::sendRefreshDevices()
{
	BYTE buf[0x10];
	ZeroMemory(buf, sizeof(buf));
	UINT nIndex = 0;
	*(WORD*)(buf+nIndex) = WM_UI2SERVICE::WM_REFRESH_DEVICES;
	nIndex+=2;
	SendData(buf, nIndex);
}