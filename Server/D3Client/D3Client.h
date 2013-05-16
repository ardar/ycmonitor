#pragma once

class _declspec(dllexport) D3Client : public ISocketClientNotify, public ID3Client
{
public:
	enum D3ClientMsg
	{
		D3ClientCmd,
		D3ClientDisconnect,
	};

	D3Client(void);
	virtual ~D3Client(void);

	// Control
	virtual BOOL InitClient(LPCTSTR szClientId, LPCTSTR szConfigFilename, Func_OnClientCallback pCallback);
	virtual BOOL StartClient();
	virtual void StopClient();
	virtual BOOL IsConnected();

	// Facade Member
	//认证客户端账号
	virtual DWORD Authenticate(LPCTSTR szName, LPCTSTR szPass);
	// 查询金钱
	virtual DWORD GetMoney();
	// 查询背包
	virtual DWORD GetDepotItems();
	// 查询拍卖完成物品
	virtual DWORD GetFinishedItems();
	// 查询已出价物品
	virtual DWORD GetBiddingItems();
	// 查询已寄售物品
	virtual DWORD GetSellingItems();
	// 查询历史记录
	virtual DWORD GetHistoryItems();

	// 搜索物品
	virtual DWORD SearchItems(const D3SearchCondition& condition);
	// 查询物品数据（在数据库）
	virtual DWORD SearchItemsInLocal(const D3SearchCondition& condition);

	// 直购
	virtual DWORD DoBuyOut(const D3ItemInfo& itemInfo);
	// 竞价
	virtual DWORD DoBid(const D3ItemInfo& itemInfo, LONGLONG llPrice);
	// 拍卖我的物品
	virtual DWORD DoSell(const D3DepotItem& itemInfo, LONGLONG nStartPrice,LONGLONG nBuyoutPrice);
	// 收取买到物品或收到金钱至背包
	virtual DWORD DoReceive(const D3ItemInfo& itemInfo);

	// 保存物品数据
	virtual DWORD SaveItemData(const D3ItemInfo& record, LONGLONG nBidPrice, LONGLONG nBuyoutPrice);

	// 获取全部任务计划
	virtual DWORD GetSchedules();
	// 加入计划任务
	virtual DWORD AddSchedule(const D3Schedule& schedule);
	// 修改计划任务
	virtual DWORD EditSchedule(const D3Schedule& schedule);
	// 删除计划任务
	virtual DWORD DelSchedule(const D3Schedule& schedule);
	// 切换账号
	virtual DWORD SwitchAccount(const D3GameAccount& acc);

	// IClientNotify Members
	virtual void OnClientConnected();
	virtual void OnClientDisconnected(DWORD dwErrorCode, LPCTSTR szErrorMsg);
	virtual void OnClientRecvPack(BYTE* buf, int len);
	virtual void OnClientNotifyMsg(DWORD dwMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnClientPrintMsg(LPCTSTR szMsg);

	void SendPack(ISocketPack* pCmd);
private:
	// Monitor Part
	void SetMainWnd(HWND hMainWnd);
	void Log(LPCTSTR szFormat, ...);
	void ErrorLog(LPCTSTR szFormat, ...);

	static DWORD WINAPI MonitorThread(PVOID pParam);

	void monitorProcess();
	DWORD handleMsg();
	DWORD createRequestId();

	ISocketClient* m_pClient;

	CString m_szClientId;
	// 是否等待调用返回
	BOOL m_bIsWaitCallResult;
	// 是否已认证
	BOOL m_bIsAuthenticated;
	// 上级服务器地址
	CString m_szUpServerUri;
	CString m_szAppRootDir;

	DWORD m_waitEventTimeout;
	BSWaitEvent m_waitEvent;

	HANDLE m_hMonitorThread;
	DWORD m_dwMonitorThreadId;

	HANDLE m_hThreadExitEvent;
	HANDLE m_hThreadStartupEvent;
	HWND m_hMainWnd;

	CMutex m_mutexRequestId;
	DWORD m_dwCurrRequestId;

	vector<D3SearchItem*> m_searchItemsCache;
	DWORD m_searchItemCacheRequestId;

	Func_OnClientCallback m_pOnClientCallback;

	typedef void (D3Client::*RecvFunc)(BYTE* buf, int len);

	BSMap<DWORD, DWORD, RecvFunc, RecvFunc> m_mapRecvFunc;

	void LoadConfig(LPCTSTR szConfigFile);

	// 通知界面
	void UISysLog(LPCTSTR szLog);
	void UIErrorLog(DWORD dwErrorCode, LPCTSTR szLog);
	void UIPostMsg(DWORD dwMsg, DWORD wParam=NULL, DWORD lParam=NULL);

	void notifyMonitor(DWORD dwMsg, DWORD wParam=NULL, DWORD lParam=NULL);

	// 收报处理函数
	void onRecvAuthResult(BYTE* buf, int len);
	void onRecvGetMoneyResult( BYTE* buf, int len );
	void onRecvDepotItems( BYTE* buf, int len );
	void onRecvSearchItems( BYTE* buf, int len );
	void onRecvFinishedItems( BYTE* buf, int len );
	void onRecvBiddingItems( BYTE* buf, int len );
	void onRecvSellingItems( BYTE* buf, int len );
	void onRecvActionResult( BYTE* buf, int len );
	void onRecvGetSchedules( BYTE* buf, int len );
	void onRecvScheduleUpdated( BYTE* buf, int len );
};
