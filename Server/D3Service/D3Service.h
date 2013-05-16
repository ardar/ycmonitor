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

	// �������ݰ����ͻ���
	virtual BOOL SendToSession(DWORD dwSessionId, ISocketPack* pPack);
	virtual void CloseSession(DWORD dwSessionId);

	// �����������
	virtual DWORD QueueCmd( IHandlerCmd* pCmd );
	// ����Ҫ�����item����
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

	// ������Ʒ
	BOOL LuaSearchItems(
		LPCTSTR szCharactorFilter, LPCTSTR szPrimaryFilter, LPCTSTR szSecondaryFilter, 
		LPCTSTR szRarityFilter, int nLevelMin, int nLevelMax, double nMaxBuyout, 
		LPCTSTR szUniqueItemTextBox, int nPage, LPCTSTR szAdvanceComboBoxies);
	// ��ȡȫ����Ʒ
	BOOL LuaReceiveAllItems();
	// ������Ʒ����
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

	// ����ѡ��(�������ļ���ȡ)
	CString m_szServiceId;
	ServiceSetting m_setting;

	// ���ݿ�����
	DBSetting m_dbSetting;

	// ���б���
	CString m_szAppDataDir;//���������ļ�����Ŀ¼(ͬWeb�����Ŀ¼)

	static const int m_nStatusUpdateCheckTime = 20;//����¼��Ƿ������State��ʱ����
	static const DWORD s_waitEventTimeout = 30000;
	int m_dwGuessQueuedExecTime;//Ԥ��ִ�ж����е�������ִ��ʱ��

	BSWaitEvent m_waitEvent;
	HANDLE m_hThreadExitEvent;
	HANDLE m_hThreadStartupEvent;

	// ������߳�
	HANDLE m_hCmdThread;
	DWORD m_dwCmdThreadId;
	// ����߳�
	HANDLE m_hMonitorThread;
	DWORD m_dwMonitorThreadId;
	// �ƻ������߳�
	HANDLE m_hScheduleThread;
	DWORD m_dwScheduleThreadId;
	// ���ݱ����߳�
	HANDLE m_hDataThread;
	DWORD m_dwDataThreadId;

	BSLogger m_logger;

	BSMap<DWORD, DWORD, D3ServiceSession, D3ServiceSession&> m_mapSession;
	BSMap<DWORD, DWORD, D3Schedule*, D3Schedule*> m_mapSchedules;
	queue<D3SearchItem*> m_queueSavingItems;
	CMutex m_mutexSavingItems;

	void LoadConfig(LPCTSTR szConfigFileName);

	// ֪ͨ����
	void UISysLog(LPCTSTR szLog);
	void UIErrorLog(DWORD dwDeviceId, DWORD dwErrorCode, LPCTSTR szLog);
	// ��Ϣ
	void notifyMonitor(DWORD dwMsg, DWORD wParam=NULL, DWORD lParam=NULL);
	void executeSchedule(D3Schedule* pSchedule);
	void initLuaFuncs();
	BSLua* m_pBSLua;
};
