#pragma once
#include "..\BSUtil\SocketServer.h"
#include "..\D3Common\UIProxyContract.h"
#include "..\D3Common\IService.h"
#include "..\D3Service\IUIProxy.h"
#include "..\D3Service\SessionManager.h"
#include "BSLua.h"

class _declspec(dllexport) D3Service : public ISocketServerNotify 
{
public:
	static D3Service& Instance();

	// Control
	BOOL InitServer(LPCTSTR szConfigFilename);
	BOOL StartServer();
	void StopServer();
	BOOL IsRunning(){return WaitForSingleObject(m_hThreadExitEvent, 0)!=WAIT_OBJECT_0;};

	// 发送数据包到客户端
	virtual BOOL SendToSession(DWORD dwSessionId, ISocketPack* pPack);
	virtual void CloseSession(DWORD dwSessionId);

	// 加入命令到队列
	virtual DWORD QueueCmd( IHandlerCmd* pCmd );
	// 加入要保存的item数据
	virtual void QueueItemData(D3SearchItem* pItem);

	// IServerCallback Members
	virtual void OnSessionConnected(const PcSessionInfo* pSession);
	virtual void OnSessionDisconnected(const PcSessionInfo* pSession);
	virtual void OnSessionRecvPack(const PcSessionInfo* pSession, BYTE* buf, int len);
	virtual void OnSessionNotifyMsg(
		const PcSessionInfo* pSession, DWORD dwMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnSessionPrintMsg(const PcSessionInfo* pSession, LPCTSTR szMsg);

	// Management Server Part

	// Monitor Part
	void Log(LPCTSTR szFormat, ...);
	void ErrorLog(LPCTSTR szFormat, ...);
	void UIRefreshService();
	void UIRefreshDevices();
	void UIRefreshDevice(LPCTSTR szMac);
	void UIDeleteDevice(LPCTSTR szDeviceMac);

	//Session 
	void CreateSession(DWORD dwSessionId, const PcSessionInfo& pcSession);
	void DeleteSession(DWORD dwSessionId);
	BOOL GetSession(DWORD dwSessionId, D3ServiceSession& session);
	void UpdateSession(D3ServiceSession& session);
	BOOL SetAuththenticated(DWORD dwSessionId, LPCTSTR szUsername);
	BOOL SetSessionOp( DWORD dwSessionId, LPCTSTR szOPName );
	BOOL IsAuthenticated(DWORD dwSessionId);
	void UpdateCurrAccSetting(LPCTSTR szAccount, LPCTSTR szPassword);

	// 搜索物品
	BOOL LuaSearchItems(
		LPCTSTR szCharactorFilter, LPCTSTR szPrimaryFilter, LPCTSTR szSecondaryFilter, 
		LPCTSTR szRarityFilter, int nLevelMin, int nLevelMax, double nMaxBuyout, 
		LPCTSTR szUniqueItemTextBox, int nPage, LPCTSTR szAdvanceComboBoxies);
	// 收取全部物品
	BOOL LuaReceiveAllItems();
	// 跟踪物品出价
	BOOL LuaTraceBidItem(LPCTSTR szItemName, double dMaxPrice, double dAddPrice);
private:
	D3Service(void);
	virtual ~D3Service(void);

	static D3Service s_instance;

	static DWORD WINAPI CmdThread(PVOID pParam);
	static DWORD WINAPI MonitorThread(PVOID pParam);
	static DWORD WINAPI ScheduleThread(PVOID pParam);
	static DWORD WINAPI DataThread(PVOID pParam);

	void monitorProcess();
	void cmdProcess();
	DWORD handleMsg();

	void scheduleProcess();

	void dataProcess();

	// UI Proxy
	IUIProxy* m_pUIProxy;
	SocketServer* m_pServer;
	BSMap<DWORD, DWORD, IHandlerCmd*, IHandlerCmd*> m_mapPackHandler;

	// 配置选项(从配置文件读取)
	CString m_szServiceId;
	ServiceSetting m_setting;

	// 数据库设置
	DBSetting m_dbSetting;

	// 运行变量
	CString m_szAppDataDir;//本地数据文件保存目录(同Web服务根目录)

	static const int m_nStatusUpdateCheckTime = 20;//检查下级是否更新了State的时间间隔
	static const DWORD s_waitEventTimeout = 30000;
	int m_dwGuessQueuedExecTime;//预测执行队列中的命令总执行时间

	BSWaitEvent m_waitEvent;
	HANDLE m_hThreadExitEvent;
	HANDLE m_hThreadStartupEvent;

	// 命令处理线程
	HANDLE m_hCmdThread;
	DWORD m_dwCmdThreadId;
	// 监控线程
	HANDLE m_hMonitorThread;
	DWORD m_dwMonitorThreadId;
	// 计划任务线程
	HANDLE m_hScheduleThread;
	DWORD m_dwScheduleThreadId;
	// 数据保存线程
	HANDLE m_hDataThread;
	DWORD m_dwDataThreadId;

	BSLogger m_logger;

	BSMap<DWORD, DWORD, D3ServiceSession, D3ServiceSession&> m_mapSession;
	BSMap<DWORD, DWORD, D3Schedule*, D3Schedule*> m_mapSchedules;
	queue<D3SearchItem*> m_queueSavingItems;
	CMutex m_mutexSavingItems;

	void LoadConfig(LPCTSTR szConfigFileName);

	// 通知界面
	void UISysLog(LPCTSTR szLog);
	void UIErrorLog(DWORD dwDeviceId, DWORD dwErrorCode, LPCTSTR szLog);
	// 消息
	void notifyMonitor(DWORD dwMsg, DWORD wParam=NULL, DWORD lParam=NULL);
	void executeSchedule(D3Schedule* pSchedule);
	void initLuaFuncs();
	BSLua* m_pBSLua;
};
