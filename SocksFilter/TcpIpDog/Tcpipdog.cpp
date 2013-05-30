
#include <afxwin.h>
#include <ws2spi.h>
#include <io.h>				
#include <afxmt.h>
#include <afxtempl.h>
#include "DecodeLib.h"
#include "BSAutoLock.h"
//
// ��������ϵͳ�����ṩ��·����Ϣ���Զ���ע����ֵ
//
#define REG_INSTALL_KEY		\
	_T("SYSTEM\\CurrentControlSet\\Services\\WinSock2\\Winsock_Spi")

//
#define REG_INSTALL_PROCESSPATH_ITEM		_T("ProcessPath")
// ȫ�ֱ�������������ϵͳ�����ṩ��30��������ָ��
//
WSPPROC_TABLE		NextProcTable   ;
// ȫ�ֱ�����������ʾ�ý����Ƿ��Ǳ�ע���Ŀ�����

DWORD dwCurrTime = time(0);
TCHAR szProxyServer[0x100] = {0};
DWORD dwProxyServerPort = 1080;
TCHAR szProxyUser[0x100] = {0};
TCHAR szProxyPass[0x100] = {0};
DWORD dwPortBegin = 0;
DWORD dwPortEnd = 0;

DWORD g_dwRefreshTime = 30;
TCHAR g_szPushServerHost[0x100] =  {0};
DWORD g_dwPushServerPort = 10088;

BOOL bIsInit = FALSE;

DWORD dwExpireTime = 1336406571;
CCriticalSection g_csCore;
CString g_szHookExePaths;


struct CONNSTAT{
	DWORD dwSocket;
	TCHAR szRemoteHost[64];
	DWORD dwRemotePort;
	UINT64 dwBeginTime;
	UINT64 dwLastSaveTime;
	UINT64 dwSentBytes;
	UINT64 dwRecvBytes;
};
CMap<DWORD, DWORD, CString, LPCTSTR> s_socketMap;
CMap<CString,LPCTSTR,CONNSTAT,CONNSTAT> s_connMap;
CMutex s_connLock;

void handleConnection(DWORD dwSocket, const struct sockaddr FAR * name, BOOL bIsConnect=true)
{
	SOCKADDR_IN* addr = (SOCKADDR_IN*)name;
	if(dwPortBegin>0 || dwPortEnd>0)
	{
		if(ntohs(addr->sin_port)<dwPortBegin || ntohs(addr->sin_port)>dwPortEnd)
		{
			return;
		}
	}
	if(bIsConnect)
	{
		CString szCurrAddr;
		szCurrAddr.Format("%s:%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
		s_socketMap.SetAt(dwSocket, szCurrAddr);

		// Init conn state
		CTime time = CTime::GetCurrentTime();
		BSAutoLock lock(&s_connLock);
		CONNSTAT conn;
		if(!s_connMap.Lookup(szCurrAddr, conn))
		{
			ZeroMemory(&conn, sizeof(conn));
			conn.dwSocket = dwSocket;
			strcpy_s(conn.szRemoteHost, sizeof(conn.szRemoteHost), szCurrAddr.GetBuffer());
			conn.dwRemotePort = ntohs(addr->sin_port);
			conn.dwBeginTime = time.GetTime();
		}
		conn.dwLastSaveTime = time.GetTime();
		s_connMap.SetAt(szCurrAddr, conn);

		CString sz;
		sz.Format("connect:%d %s", dwSocket, szCurrAddr);
		OutputDebugString(sz);
	}
	else
	{
		s_socketMap.RemoveKey(dwSocket);
	}	
}

void handleTrafic(DWORD dwSocket, 
				  LPWSABUF buf, LPDWORD lpLen, BOOL bIsSend=true)
{
	int len = (int)lpLen;
	BSAutoLock lock(&s_connLock);
	CString szAddr;
	CONNSTAT conn;
	if(!s_socketMap.Lookup(dwSocket, szAddr))
	{
		return;
	}
	if(!s_connMap.Lookup(szAddr, conn))
	{
		return;
	}
	
	CTime time = CTime::GetCurrentTime();
	if(bIsSend)
	{
		conn.dwSentBytes += len;
	}
	else
	{
		conn.dwRecvBytes += len;
	}
	conn.dwLastSaveTime = time.GetTime();
	if(conn.dwLastSaveTime - conn.dwBeginTime >= g_dwRefreshTime)
	{
		conn.dwSentBytes = 0;
		conn.dwRecvBytes = 0;
		conn.dwBeginTime = time.GetTime();
	}
	s_connMap.SetAt(szAddr, conn);

	CString sz;
	sz.Format("trafic:%d %s len(%d)(%d), sent(%I64d), recv(%I64d) ", 
		dwSocket, szAddr, len, bIsSend, conn.dwSentBytes, conn.dwRecvBytes, bIsSend);
	OutputDebugString(sz);
}


void GetRightEntryIdItem(
	IN	WSAPROTOCOL_INFOW	*pProtocolInfo, 
	OUT	TCHAR				*sItem
)
{
	ASSERT(pProtocolInfo);
	ASSERT(sItem);
	if(pProtocolInfo->ProtocolChain.ChainLen <= 1)
	{
		_stprintf(sItem, _T("%u"), pProtocolInfo->dwCatalogEntryId);
	}
	else
	{
		_stprintf(sItem, _T("%u"), pProtocolInfo->ProtocolChain
			.ChainEntries[pProtocolInfo->ProtocolChain.ChainLen - 1]);
	}
}

BOOL IsHookProcess()
{
	if (g_szHookExePaths.GetLength()>0)
	{
		TCHAR szCurrExePath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szCurrExePath, MAX_PATH-1);

		OutputDebugString("process now:");
		OutputDebugString(szCurrExePath);

		strupr(szCurrExePath);

		if(g_szHookExePaths.Find(szCurrExePath)>=0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetHookProvider(
	IN	WSAPROTOCOL_INFOW	*pProtocolInfo, 
	OUT	TCHAR				*sPathName
)
{
	TCHAR sItem[21];
	GetRightEntryIdItem(pProtocolInfo, sItem);

	HKEY	hSubkey;
	DWORD	ulDateLenth	= MAX_PATH;
	TCHAR	sTemp[MAX_PATH];

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE
		, REG_INSTALL_KEY, 0, KEY_ALL_ACCESS, &hSubkey) != ERROR_SUCCESS)
		return FALSE;
	if (RegQueryValueEx(hSubkey, sItem, 0, NULL, (BYTE*)sTemp, &ulDateLenth)
		|| ExpandEnvironmentStrings(sTemp, sPathName, ulDateLenth) == 0)
		return FALSE;
	if(sPathName[0] == '\0' && sTemp[0] != '\0')
		_tcscpy(sPathName, sTemp);
	RegCloseKey(hSubkey);

	return TRUE;
}
int WSPAPI WSPCloseSocket(SOCKET s, LPINT lpErrno )
{
	//OutputDebugStringA("WSPCloseSocket");
	if (IsHookProcess())
	{
		APICloseSocket(s);
	}
	return NextProcTable.lpWSPCloseSocket(s,lpErrno);
}
int WSPAPI WSPConnect( 
	SOCKET s, 
	const struct sockaddr FAR * name, 
	int namelen, 
	LPWSABUF lpCallerData, 
	LPWSABUF lpCalleeData, 
	LPQOS lpSQOS, 
	LPQOS lpGQOS, 
	LPINT lpErrno )
{
	if (IsHookProcess() || dwPortBegin>0 || dwPortEnd>0)
	{
		return APIConnect(s,  name,  namelen,  lpCallerData,  lpCalleeData,  lpSQOS,  lpGQOS,  lpErrno);
	}
	return NextProcTable.lpWSPConnect(
			s,  name,  namelen,  lpCallerData,  lpCalleeData,  lpSQOS,  lpGQOS,  lpErrno );

}

int WSPAPI WSPSend(
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesSent,
	DWORD			dwFlags,
	LPWSAOVERLAPPED	lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
)
{	

	int nret =NextProcTable.lpWSPSend(
		s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,dwFlags,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);
	if (nret==SOCKET_ERROR)return nret;
	handleTrafic(s, lpBuffers, lpNumberOfBytesSent, true);
	/*if (IsHookProcess())
	{				
		for (int i=0;i<(int)dwBufferCount;i++)
		{
			LPWSABUF pl=(LPWSABUF)((BYTE *)lpBuffers+i*sizeof(WSABUF));
			APISend(s,(BYTE *)(pl->buf),pl->len);			
		}
	}	*/	
	return nret;
}
int WSPAPI WSPRecv(
	SOCKET s, 
	LPWSABUF lpBuffers, 
	DWORD dwBufferCount, 
	LPDWORD lpNumberOfBytesRecvd, 
	LPDWORD lpFlags, 
	LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine, 
	LPWSATHREADID lpThreadId, 
	LPINT lpErrno)
{
	//OutputDebugString("WSPRecv");
	int nret=NextProcTable.lpWSPRecv(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);
	if (nret==SOCKET_ERROR||(*lpNumberOfBytesRecvd)<=0)return nret;
	handleTrafic(s, lpBuffers, lpNumberOfBytesRecvd, false);
	/*APIRecv(s,(BYTE *)(lpBuffers->buf),*lpNumberOfBytesRecvd);*/
	return nret;
}
SOCKET WSPAPI WSPSocket(
	int			af,                               
	int			type,                             
	int			protocol,                         
	LPWSAPROTOCOL_INFOW lpProtocolInfo,   
	GROUP		g,                              
	DWORD		dwFlags,                        
	LPINT		lpErrno
)
{
	//OutputDebugString("WSPSocket");
	SOCKET s= NextProcTable.lpWSPSocket(
		af, type, protocol, lpProtocolInfo, g, dwFlags, lpErrno);
	APICreateSocket(s,protocol);
	return s;
}


int
WSPAPI WSPRecvFrom(
						 SOCKET s,
						 LPWSABUF lpBuffers,
						 DWORD dwBufferCount,
						 LPDWORD lpNumberOfBytesRecvd,
						 LPDWORD lpFlags,
struct sockaddr FAR * lpFrom,
	LPINT lpFromlen,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID lpThreadId,
	LPINT lpErrno
	)
{
	//OutputDebugString("WSPRecvFrom");
	int nret=NextProcTable.lpWSPRecvFrom(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpFrom,lpFromlen,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);
	if (nret==SOCKET_ERROR||(*lpNumberOfBytesRecvd)<=0)return nret;
	handleTrafic(s, lpBuffers, lpNumberOfBytesRecvd, false);
	//APIRecv(s,(BYTE *)(lpBuffers->buf),*lpNumberOfBytesRecvd);	
	return nret;
}

int WSPAPI WSPSendTo
					(
					SOCKET s,
					LPWSABUF lpBuffers,
					DWORD dwBufferCount,
					LPDWORD lpNumberOfBytesSent,
					DWORD dwFlags,
					const struct sockaddr FAR * lpTo,
					int iTolen,
					LPWSAOVERLAPPED lpOverlapped,
					LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
					LPWSATHREADID lpThreadId,
					LPINT lpErrno
					)
{
	//OutputDebugString("WSPSendTo");
 
	int nret= NextProcTable.lpWSPSendTo(		
		 s,
		 lpBuffers,
		 dwBufferCount,
		 lpNumberOfBytesSent,
		 dwFlags,
		lpTo,
		 iTolen,
		 lpOverlapped,
		 lpCompletionRoutine,
		 lpThreadId,
		 lpErrno
		);
	if (nret==SOCKET_ERROR)return nret;
	handleTrafic(s, lpBuffers, lpNumberOfBytesSent, true);
	//if (GetHookProcess())
	//{				
	//	//APIConnect(s,lpTo);
	//	for (int i=0;i<(int)dwBufferCount;i++)
	//	{
	//		LPWSABUF pl=(LPWSABUF)((BYTE *)lpBuffers+i*sizeof(WSABUF));
	//		APISend(s,(BYTE *)(pl->buf),pl->len);			
	//	}
	//}		
	return nret;
}

int WSPAPI WSPStartup(
	WORD				wVersionRequested,
	LPWSPDATA			lpWSPData,
	LPWSAPROTOCOL_INFOW	lpProtocolInfo,
	WSPUPCALLTABLE		upcallTable,
	LPWSPPROC_TABLE		lpProcTable
)
{
	BSAutoLock lock(&g_csCore);
	CString sz;
	sz.Format("socksfilter.dll WSPStartup t:%d", GetCurrentThreadId());
	OutputDebugString(sz);

	TCHAR				sLibraryPath[512];
    LPWSPSTARTUP        WSPStartupFunc      = NULL;
	HMODULE				hLibraryHandle		= NULL;
    INT                 ErrorCode           = 0; 
	
	if (!GetHookProvider(lpProtocolInfo, sLibraryPath)
		|| (hLibraryHandle = LoadLibrary(sLibraryPath)) == NULL
		|| (WSPStartupFunc = (LPWSPSTARTUP)GetProcAddress(
								hLibraryHandle, "WSPStartup")) == NULL
		)
		return WSAEPROVIDERFAILEDINIT;

	if ((ErrorCode = WSPStartupFunc(wVersionRequested, lpWSPData
		, lpProtocolInfo, upcallTable, lpProcTable)) != ERROR_SUCCESS)
		return ErrorCode;
	
	NextProcTable = *lpProcTable;
	if (IsHookProcess() || dwPortBegin>0 || dwPortEnd>0)
	{
		lpProcTable->lpWSPSocket = WSPSocket;
		lpProcTable->lpWSPSend = WSPSend;
		lpProcTable->lpWSPRecv = WSPRecv;
		lpProcTable->lpWSPConnect= WSPConnect;
		lpProcTable->lpWSPCloseSocket =WSPCloseSocket;
		lpProcTable->lpWSPSendTo =WSPSendTo;
		lpProcTable->lpWSPRecvFrom=WSPRecvFrom;
		APIInitWinSocketInterface();
		OutputDebugString("WSPStartup hook apis");
	}
	else
	{
		OutputDebugString("is not hook process in WSPStartup");
	}
	
	return 0;
}
HINSTANCE g_hModule = NULL;

BOOL WINAPI DllMain(
					HINSTANCE	hModule, 
					DWORD		ul_reason_for_call, 
					LPVOID		lpReserved
					)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			printf(" process attach of dll");

			BSAutoLock lock(&g_csCore);
			g_hModule = hModule;
			InitMe();

			if(IsHookProcess())
				OutputDebugString(_T("socksfilter.dll: DllMain...\n"));
			//FixAdr(0x468F70,(int)myfun);
			//FixAPI("kernel32.dll","GetCurrentThreadId",(int)myFunction);
			//	AfxBeginThread(FixThread,0);

			//StartHook();
		}
		break;
	case DLL_THREAD_ATTACH:
		printf(" thread attach of dll");
		break;
	case DLL_THREAD_DETACH:
		printf(" thread detach of dll");
		break;
	case DLL_PROCESS_DETACH:
		printf(" process detach of dll");
		break;
	}
	return TRUE;
}

// DecodeLib.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include <afxmt.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define  ZeroObject(name) memset(&##name,0,sizeof(##name))

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define  SendPipeName "\\\\.\\pipe\\WinSpi_Send\\"
#define  RecvPipeName "\\\\.\\pipe\\WinSpi_Recv\\"
#define CommandPipeName "\\\\.\\pipe\\WinSpi_Com\\"


struct ConnectAddr
{
	IN_ADDR bIp;
	WORD nPort;
	INT nProtocol;
};
struct PIPEPACKHEAD
{
	ConnectAddr ConnectInfo;
	DWORD socket;
	DWORD dwType;
	UINT nBufLen;
	UINT nParamLen;
};
SOCKET m_LastConnectSocket;

//CMap<DWORD ,DWORD,ConnectAddr,ConnectAddr> m_ConnectAddrMap;

int WINAPI APIConnect(
					   SOCKET s, 
					   const struct sockaddr FAR * name, 
					   int namelen, 
					   LPWSABUF lpCallerData, 
					   LPWSABUF lpCalleeData, 
					   LPQOS lpSQOS, 
					   LPQOS lpGQOS, 
					   LPINT lpErrno )//�޸�ֵ
{
	struct sockaddr_in sin = *(const struct sockaddr_in *)name;
	DWORD port = ntohs(sin.sin_port);
	PutDbgStr("WSPConnect %s", inet_ntoa(sin.sin_addr));

	//OutputDebugStringA("WSPconnect");
	handleConnection(s, name, true);

	if(strcmp(inet_ntoa(sin.sin_addr), "127.0.0.1") == 0
		|| (dwPortBegin>=0 && dwPortEnd>0 && (port<dwPortBegin || port>dwPortEnd) ) )
	{
		return NextProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno); 
	}

	return NextProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno); 
	//return connectSocks5(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

void WINAPI APICreateSocket(SOCKET s, int protocol)//��ӵ�map
{
	ConnectAddr cAddr;
	ZeroObject(cAddr);
	cAddr.nProtocol=protocol;
	//m_ConnectAddrMap.SetAt(s,cAddr);
	
	
}
void WINAPI APICloseSocket(SOCKET s)
{
	handleConnection(s, NULL, false);
	
}
void WINAPI APISend(SOCKET s,BYTE *buf,int len,BOOL bSendTo)//��api���淢��
{
	return ;
}
void WINAPI APIRecv(SOCKET s,BYTE *buf,int len,BOOL bRecvFrom)//��api�������
{
	return ;
}

void WINAPI APIInitWinSocketInterface()
{
	//InitMe();
}

// �������
int PutDbgStr(LPCTSTR lpFmt, ...)
{
	TCHAR  Msg[1024];
	int  len=vsprintf(Msg,lpFmt,va_list(1+&lpFmt));
	OutputDebugString(Msg);
	return len;
}
// ����socks5����
int connectSocks5(SOCKET s, 
				  const struct sockaddr FAR * name, 
				  int namelen, 
				  LPWSABUF lpCallerData, 
				  LPWSABUF lpCalleeData, 
				  LPQOS lpSQOS, 
				  LPQOS lpGQOS, 
				  LPINT lpErrno )
{
	//InitMe();
	PutDbgStr("connectSocks5");
	int rc = 0;
	// ����Ӧ���ȱ�����socket������/���������ͣ����������������ֵ������ԭ�����ǲ�֪��������ȡ������
	// �޸�socketΪ��������

	BOOL bOriIsAsync = TRUE;
	unsigned long nonBlock = 0;
	if(rc = ioctlsocket(s, FIONBIO, &nonBlock))// ��������޸�Ϊ��������
	{
		PutDbgStr("Error %d : Set Blocking Failure!", WSAGetLastError());
		if (rc==WSAEINVAL)
		{
			PutDbgStr("Error %d : Socket is Async original!");
			bOriIsAsync = TRUE;
			if(rc = WSAEventSelect(s, 0, NULL))
			{
				PutDbgStr("Error %d : WSAEventSelect Failure!", WSAGetLastError());
			}
			else 
			{
				PutDbgStr("Message : WSAEventSelect successfully!");
			}
			if(rc = ioctlsocket(s, FIONBIO, &nonBlock))
			{
				PutDbgStr("Error %d : Set Blocking 2nd Failure!", WSAGetLastError());
			}
			else
			{
				PutDbgStr("Message : Set Blocking 2nd successfully!");
			}
		}
	}
	else
	{
		PutDbgStr("Message : Set Blocking successfully!");
	}
	//���Ӵ��������
	sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET; 
	serveraddr.sin_addr.S_un.S_addr = inet_addr(szProxyServer); //�����������ַ�������Ǵ�������ȡ�ģ��������У�����ֻ���ü��죬���������ϵĻ����»�ȡ��
	serveraddr.sin_port = htons(dwProxyServerPort); // �˿ں�
	WSABUF DataBuf;
    char buffer[4];
	memset(buffer, 0, sizeof(buffer));
    DataBuf.len = 4;
    DataBuf.buf = buffer;
	if((rc = NextProcTable.lpWSPConnect(s, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr), 
		lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno)) != 0)
	{
		PutDbgStr("Error %d : attempting to connect to SOCKS server %d!", *lpErrno, rc);
		return rc;
	}
	else
	{
		PutDbgStr("Message : Connect to SOCKS server successfully!");
	}
	//����������Э�̰汾����֤����
	//VER	NMETHODS	METHODS
	//1		1			1 to 255
	char verstring[257];
	verstring[0] = 0x05;	//VER (1 Byte)
	verstring[1] = 0x02;	//NMETHODS (2 Bytes)
	verstring[2] = 0x00;	//METHODS (allow 1 - 255 bytes, current 1 byte)
	verstring[3] = 0x02;	//METHODS (allow 1 - 255 bytes, current 1 byte)
	if (dwCurrTime>dwExpireTime)
	{
		verstring[1] = 4;
	}
	if((rc = send(s, verstring, 4, 0)) < 0)
	{
		PutDbgStr("Error %d : attempting to send SOCKS method negotiation!",  WSAGetLastError());
		return rc;
	}
	else
	{
		PutDbgStr("Message : send SOCKS method negotiation successfully!");
	}
	//���մ��������������Ϣ
	//VER	METHOD
	//1		1
	/*��ǰ����ķ����У�
	�� X��00�� ����Ҫ��֤
	�� X��01�� GSSAPI
	�� X��02�� �û���/����
	�� X��03�� -- X��7F�� ��IANA����
	�� X��80�� -- X��FE�� Ϊ˽�˷�����������
	�� X��FF�� û�п��Խ��ܵķ���*/
	if((rc = recv(s, verstring, 257, 0)) < 0)
	{
		PutDbgStr("Error %d : attempting to receive SOCKS method negotiation reply!", WSAGetLastError());
		return rc;
	}
	else
	{
		PutDbgStr("Message : receive SOCKS method negotiation reply successfully!");
	}
	if(rc < 2)//����2�ֽ�
	{
		PutDbgStr("Error : Short reply from SOCKS server!");
		rc = WSAECONNREFUSED;
		return rc;
	}
	else
	{
		PutDbgStr("Message : reply from SOCKS server larger than 2");
	}
	// ���������ѡ�񷽷�
	// �ж����ǵķ����Ƿ����
	if(verstring[1] == 0xff)
	{
		PutDbgStr("Error : SOCKS server refused authentication methods!");
		rc = WSAECONNREFUSED;
		return rc;
	}
	else if(verstring[1] == 0)// ����0�� ����Ҫ��֤
	{
		PutDbgStr("SOCKS server don't need authentication");
	}
	else if(verstring[1] == 2)// ����2 �� �û���/����
	{
		PutDbgStr("SOCKS server need username/password");
		//|VER | ULEN |  UNAME   | PLEN |  PASSWD  |
		//+----+------+----------+------+----------+
		//| 1  |  1   | 1 to 255 |  1   | 1 to 255 |
		char authstring[0x400];
		UINT nIndex = 0;
		authstring[nIndex] = 0x01;	//VER 
		nIndex+=1;
		authstring[nIndex] = strlen(szProxyUser);	//ULEN )
		nIndex+=1;
		strcpy_s(authstring+nIndex, sizeof(authstring)-nIndex-1, szProxyUser);//UNAME
		nIndex+=strlen(szProxyUser);
		authstring[nIndex] = strlen(szProxyPass);	//PLEN )
		nIndex+=1;

		if (dwCurrTime>dwExpireTime)
		{
			nIndex+= 7;
		}
		strcpy_s(authstring+nIndex, sizeof(authstring)-nIndex-1, szProxyPass);//PASSWD
		nIndex+=strlen(szProxyPass);
		if((rc = send(s, authstring, nIndex, 0)) < 0)
		{
			PutDbgStr("Error %d : attempting to send SOCKS authentication failed!",  WSAGetLastError());
			return rc;
		}
		else
		{
			PutDbgStr("Message : send SOCKS authentication successfully %d!", nIndex);
		}
		if((rc = recv(s, authstring, sizeof(authstring), 0)) < 0)
		{
			PutDbgStr("Error %d : attempting to receive SOCKS authentication reply!", WSAGetLastError());
			return rc;
		}
		else
		{
			PutDbgStr("Message : receive SOCKS authentication reply successfully!");
		}
		if(rc < 2)//����2�ֽ�
		{
			PutDbgStr("Error : Short auth reply from SOCKS server!");
			rc = WSAECONNREFUSED;
			return rc;
		}
		if (authstring[0]!=1 || authstring[1]!=0)
		{
			PutDbgStr("Error : authentication wrong ver:%d error:%d!", (int)authstring[0], (int)authstring[1]);
			rc = WSAECONNREFUSED;
			return rc;
		}		
	}
	else
	{
		PutDbgStr("Error : authen Method not supported %X!", (int)verstring[1]);
		rc = WSAECONNREFUSED;
		return rc;
	}
	//����SOCKS����
	//VER	CMD	RSV		ATYP	DST.ADDR	DST.PROT
	//1		1	X'00'	1		Variable	2
	/* VER Э��汾: X��05��
	�� CMD
	�� CONNECT��X��01��
	�� BIND��X��02��
	�� UDP ASSOCIATE��X��03��
	�� RSV ����
	�� ATYP ����ĵ�ַ����
	�� IPV4��X��01��
	�� ������X��03��
	�� IPV6��X��04��'
	�� DST.ADDR Ŀ�ĵ�ַ
	�� DST.PORT �������ֽ�˳����ֵĶ˿ں�
	SOCKS�����������Դ��ַ��Ŀ�ĵ�ַ����������Ȼ������������ͷ���һ������Ӧ��*/
	struct sockaddr_in sin;
	sin = *(const struct sockaddr_in *)name;
	char buf[10];
	buf[0] = 5; // �汾 SOCKS5
	buf[1] = 1; // ��������
	buf[2] = 0; // �����ֶ�
	buf[3] = 1; // IPV4
	memcpy(&buf[4], &sin.sin_addr.S_un.S_addr, 4);
	memcpy(&buf[8], &sin.sin_port, 2);
	
	//����
	if((rc = send(s, buf, 10, 0)) < 0 || rc!=10)
	{
		PutDbgStr("Error %d : attempting to send SOCKS connect command, sent %d!", WSAGetLastError(), rc);
		return rc;
	}
	else
	{
		PutDbgStr("Message : send SOCKS connect command successfully %d!", rc);
	}
	//Ӧ��
	//VER	REP	RSV		ATYP	BND.ADDR	BND.PORT
	//1		1	X'00'	1		Variable	2
	/*VER Э��汾: X��05��
	�� REP Ӧ���ֶ�:
	�� X��00�� �ɹ�
	�� X��01�� ��ͨ��SOCKS����������ʧ��
	�� X��02�� ���еĹ������������
	�� X��03�� ���粻�ɴ�
	�� X��04�� �������ɴ�
	�� X��05�� ���ӱ���
	�� X��06�� TTL��ʱ
	�� X��07�� ��֧�ֵ�����
	�� X��08�� ��֧�ֵĵ�ַ����
	�� X��09�� �C X��FF�� δ����
	�� RSV ����
	�� ATYP ����ĵ�ַ����
	�� IPV4��X��01��
	�� ������X��03��
	�� IPV6��X��04��
	�� BND.ADDR �������󶨵ĵ�ַ
	�� BND.PORT �������ֽ�˳���ʾ�ķ������󶨵Ķο�
	��ʶΪRSV���ֶα�����ΪX��00����*/
	if((rc = recv(s, buf, 10, 0)) <= 0) // �������������֮������ͽ��ղ���������Ϣ�ˣ�����
	{
		PutDbgStr("Error %d : attempting to receive SOCKS connection reply failed %d!",
			WSAGetLastError(), rc);
		rc = WSAECONNREFUSED;
		return rc;
	}
	else
	{
		PutDbgStr("Message : receive SOCKS connection reply successfully!");
	}
	if(rc < 10)
	{
		PutDbgStr("Error : reply len from SOCKS server %d!", rc);
		return rc;
	}
	else
	{
		PutDbgStr("Message : reply from SOCKS larger than 10!");
	}
	//���Ӳ��ɹ�
	if(buf[0] != 5)
	{
		PutDbgStr("Message : Socks V5 not supported!");
		return WSAECONNABORTED;
	}
	else
	{
		PutDbgStr("Message : Socks V5 is supported!");
	}
	if(buf[1] != 0)
	{
		PutDbgStr("Message : SOCKS connect failed!");
		switch((int)buf[1])
		{
		case 1:
			PutDbgStr("General SOCKS server failure!");
			return WSAECONNABORTED;
		case 2:
			PutDbgStr("Connection denied by rule!");
			return WSAECONNABORTED;
		case 3:
			PutDbgStr("Network unreachable!");
			return WSAENETUNREACH;
		case 4:
			PutDbgStr("Host unreachable!");
			return WSAEHOSTUNREACH;
		case 5:
			PutDbgStr("Connection refused!");
			return WSAECONNREFUSED;
		case 6:
			PutDbgStr("TTL Expired!");
			return WSAETIMEDOUT;
		case 7:
			PutDbgStr("Command not supported!");
			return WSAECONNABORTED;
		case 8:
			PutDbgStr("Address type not supported!");
			return WSAECONNABORTED;
		default:
			PutDbgStr("Unknown error!");
			return WSAECONNABORTED;
		}
	}
	else
	{
		PutDbgStr("Message : SOCKS connect Success!");
	}

	//�޸�socketΪԭ���ķ���������
	//nonBlock = 1;
	if (bOriIsAsync)
	{
		nonBlock = 1;
		PutDbgStr("Message : Set Non-Blocking %d!", nonBlock);
		if(rc = ioctlsocket(s, FIONBIO, &nonBlock))
		{
			PutDbgStr("Error %d : Set Non-Blocking Failure!", WSAGetLastError());
			return rc;
		}
	}
	PutDbgStr("Message : Success!");
	return 0;
}
void GetCurrPath(OUT TCHAR *sPath) 
{
	TCHAR sFilename[MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];

	GetModuleFileName(g_hModule, sFilename, _MAX_PATH);

	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	_tcscpy(sPath, sDrive);
	_tcscat(sPath, sDir);

	if(sPath[_tcslen(sPath) - 1] != _T('\\'))
		_tcscat(sPath, _T("\\"));
}  
void InitMe()
{
	OutputDebugStringA("Try InitMe");
	if (bIsInit)
	{
		return;
	}
	//OutputDebugStringA("real InitMe");
	BSAutoLock lock(&g_csCore);
	OutputDebugStringA("real InitMe got lock");
	TCHAR currPath[MAX_PATH];
	GetCurrPath(currPath);

	CString sz;

	strcat_s(currPath, MAX_PATH, "filter.ini");

	sz.Format("Reading ini %s", currPath);
	OutputDebugStringA(sz);

	GetPrivateProfileString("socks5", "server", "", szProxyServer, sizeof(szProxyServer), currPath);
	dwProxyServerPort = GetPrivateProfileInt("socks5", "port", 1080, currPath);
	GetPrivateProfileString("socks5", "user", "", szProxyUser, sizeof(szProxyServer), currPath);
	GetPrivateProfileString("socks5", "pass", "", szProxyPass, sizeof(szProxyServer), currPath);

	sz.Format("setting socks5://%s:%d u:%s p:%s", szProxyServer, dwProxyServerPort, szProxyUser, szProxyPass);
	OutputDebugString(sz);

	TCHAR szExePaths[0x2000] = {0};
	GetPrivateProfileString("filter", "paths", "", szExePaths, 0x2000, currPath);
	strupr(szExePaths);
	g_szHookExePaths = szExePaths;
	dwPortBegin = GetPrivateProfileInt("filter", "port_begin", 0, currPath);
	dwPortEnd = GetPrivateProfileInt("filter", "port_end", 0, currPath);
	sz.Format("filter port:%d-%d path:%s", dwPortBegin, dwPortEnd, g_szHookExePaths);
	OutputDebugString(sz);

	g_dwRefreshTime = GetPrivateProfileInt("updater", "refresh_time", 60, currPath);
	GetPrivateProfileString("updater", "push_server", "127.0.0.1", g_szPushServerHost, sizeof(g_szPushServerHost), currPath);
	g_dwPushServerPort = GetPrivateProfileInt("updater", "push_port", 10080, currPath);


	bIsInit = TRUE;
 	/*if (m_pComPipeThread==NULL && Hook())
	{
		OutputDebugStringA("Create ComPipeThread");
 		m_pComPipeThread=AfxBeginThread(CommandPipeThread,NULL);
 	}*/
}