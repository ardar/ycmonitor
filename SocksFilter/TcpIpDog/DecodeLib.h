// DecodeLib.h : DecodeLib DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif


int WINAPI APIConnect(
					   SOCKET s, 
					   const struct sockaddr FAR * name, 
					   int namelen, 
					   LPWSABUF lpCallerData, 
					   LPWSABUF lpCalleeData, 
					   LPQOS lpSQOS, 
					   LPQOS lpGQOS, 
					   LPINT lpErrno );
void WINAPI APICreateSocket(SOCKET s,int	protocol);//添加到map
void WINAPI APICloseSocket(SOCKET s);
void WINAPI APISend(SOCKET s,BYTE *buf,int len,BOOL bSendTo=FALSE);//由api层面发送
void WINAPI APIRecv(SOCKET s,BYTE *buf,int len,BOOL bRecvFrom=FALSE);//由api层面接收
void WINAPI APIInitWinSocketInterface();

void InitMe();//初始化我，修改游戏代码以及，连接命令管道

int PutDbgStr(LPCTSTR lpFmt, ...);
int connectSocks5(SOCKET s, const struct sockaddr FAR * name, int namelen, 
	LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno );
