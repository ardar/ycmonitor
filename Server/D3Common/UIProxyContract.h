#pragma once
#pragma pack(push, 1)

enum WM_UI2SERVICE
{
	WM_NONE=0,
	WM_REFRESH_SERVICE,
	WM_REFRESH_DEVICES,
};

enum WM_SERVICE2UI
{
	// Service to UI
	WM_MS_DEVICE_CLEARED = WM_USER+22, //WPARAM=NULL,LPARAM=NULL
	WM_MS_DEVICE_UPDATED,//WPARAM=DeviceId, LPARAM = Monitor_DeviceInfo*
	WM_MS_DEVICE_REMOVED,//WPARAM=DeviceId, LPARAM = NULL
	WM_MS_SESSION_MSG,//WPARAM=DeviceId,LPARAM=CString * 
	WM_MS_SYSMSG,//WPARAM=LEVEL,LPARAM=CString * 
	WM_MS_SYSERROR,//WPARAM=LEVEL,LPARAM=MonitorServiceError * 
	WM_MS_SERVICE_UPDATED,//WPARAM=NULL, LPARAM=Monitor_ServiceInfo*
};

struct Monitor_ServiceInfo
{
	BOOL bIsServerRunning;
	BOOL bIsClientConnected;
	TCHAR szListenUri[MAX_NAME_LENGTH];
	TCHAR szUpServerUri[MAX_NAME_LENGTH];
	TCHAR szCurrGameAccount[MAX_NAME_LENGTH];
	
	Monitor_ServiceInfo()
	{
		ZeroMemory(this, sizeof(Monitor_ServiceInfo));
	}
};
struct Monitor_ClientInfo
{
	DWORD dwSessionId;
	TCHAR szClientIP[MAX_NAME_LENGTH];
	DWORD dwClientPort;
	TCHAR szServiceUser[MAX_NAME_LENGTH];
	BOOL bIsAuthenticated;
	DWORD dwLastOpTime;
	TCHAR szLastOperation[MAX_NAME_LENGTH];
	Monitor_ClientInfo()
	{
		ZeroMemory(this, sizeof(Monitor_ClientInfo));
	}
};

// Service启动设置
struct ServiceSetting
{
	// Cmd server
	CString szListenUri;
	// UI server
	CString szUIServerListenUri;

	// GameProxy
	CString szGamePath;
	CString szInjectDllPath;
	CString szGameAccount;
	CString szGamePassword;

	// Acc
	D3GameAccount AccountSetting;
};

//数据库设置
struct DBSetting
{
	CString szHost;
	CString szUser;
	CString szPass;
	CString szDBName;
	DWORD dwPort;
};

//遊戲帳戶
struct GameUser
{
	DWORD dwId;
	TCHAR szUsername[MAX_NAME_LENGTH];
	TCHAR szPassword[MAX_NAME_LENGTH];
	DWORD dwLastLogin;
	BOOL bIsEnabled;
	GameUser()
	{
		ZeroMemory(this, sizeof(GameUser));
	};
};

//系统用户
struct ServiceUser
{
	DWORD dwId;
	CString szUser;
	CString szPass;
	int nGroupId;
	BOOL bIsEnabled;
};

//D3服务器会话
struct D3ServiceSession
{
public:
	DWORD dwSessionid;
	TCHAR szClientIP[MAX_NAME_LENGTH];
	DWORD dwClientPort;
	TCHAR szUsername[MAX_NAME_LENGTH];
	DWORD dwBeginTime;
	DWORD dwLastRecvTime;
	DWORD dwLastSentTime;
	BOOL bIsAuthenticated;
	TCHAR szLastCmd[MAX_NAME_LENGTH];
	D3ServiceSession()
	{
		ZeroMemory(this, sizeof(D3ServiceSession));
	};
};

struct ServiceStatusInfo
{
public:

};

enum WM_PERCON_MSG
{
	WM_BSCmd = WM_USER + 0x1,// IBSCmd* 消息
};
#pragma pack(pop)