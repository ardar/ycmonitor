// DecodeLib.h : DecodeLib DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
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
void WINAPI APICreateSocket(SOCKET s,int	protocol);//��ӵ�map
void WINAPI APICloseSocket(SOCKET s);
void WINAPI APISend(SOCKET s,BYTE *buf,int len,BOOL bSendTo=FALSE);//��api���淢��
void WINAPI APIRecv(SOCKET s,BYTE *buf,int len,BOOL bRecvFrom=FALSE);//��api�������
void WINAPI APIInitWinSocketInterface();

void InitMe();//��ʼ���ң��޸���Ϸ�����Լ�����������ܵ�

int PutDbgStr(LPCTSTR lpFmt, ...);
int connectSocks5(SOCKET s, const struct sockaddr FAR * name, int namelen, 
	LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno );
