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
	//��֤�ͻ����˺�
	virtual DWORD Authenticate(LPCTSTR szName, LPCTSTR szPass);
	// ��ѯ��Ǯ
	virtual DWORD GetMoney();
	// ��ѯ����
	virtual DWORD GetDepotItems();
	// ��ѯ���������Ʒ
	virtual DWORD GetFinishedItems();
	// ��ѯ�ѳ�����Ʒ
	virtual DWORD GetBiddingItems();
	// ��ѯ�Ѽ�����Ʒ
	virtual DWORD GetSellingItems();
	// ��ѯ��ʷ��¼
	virtual DWORD GetHistoryItems();

	// ������Ʒ
	virtual DWORD SearchItems(const D3SearchCondition& condition);
	// ��ѯ��Ʒ���ݣ������ݿ⣩
	virtual DWORD SearchItemsInLocal(const D3SearchCondition& condition);

	// ֱ��
	virtual DWORD DoBuyOut(const D3ItemInfo& itemInfo);
	// ����
	virtual DWORD DoBid(const D3ItemInfo& itemInfo, LONGLONG llPrice);
	// �����ҵ���Ʒ
	virtual DWORD DoSell(const D3DepotItem& itemInfo, LONGLONG nStartPrice,LONGLONG nBuyoutPrice);
	// ��ȡ����Ʒ���յ���Ǯ������
	virtual DWORD DoReceive(const D3ItemInfo& itemInfo);

	// ������Ʒ����
	virtual DWORD SaveItemData(const D3ItemInfo& record, LONGLONG nBidPrice, LONGLONG nBuyoutPrice);

	// ��ȡȫ������ƻ�
	virtual DWORD GetSchedules();
	// ����ƻ�����
	virtual DWORD AddSchedule(const D3Schedule& schedule);
	// �޸ļƻ�����
	virtual DWORD EditSchedule(const D3Schedule& schedule);
	// ɾ���ƻ�����
	virtual DWORD DelSchedule(const D3Schedule& schedule);
	// �л��˺�
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
	// �Ƿ�ȴ����÷���
	BOOL m_bIsWaitCallResult;
	// �Ƿ�����֤
	BOOL m_bIsAuthenticated;
	// �ϼ���������ַ
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

	// ֪ͨ����
	void UISysLog(LPCTSTR szLog);
	void UIErrorLog(DWORD dwErrorCode, LPCTSTR szLog);
	void UIPostMsg(DWORD dwMsg, DWORD wParam=NULL, DWORD lParam=NULL);

	void notifyMonitor(DWORD dwMsg, DWORD wParam=NULL, DWORD lParam=NULL);

	// �ձ�������
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
