#pragma once

class DataManager
{
public:
	static DataManager& Instance();

	BOOL Initialize(LPCTSTR szServer, DWORD dwPort, LPCTSTR szUser, LPCTSTR szPass, LPCTSTR szDbName);
	void CleanUp();

	BOOL GetServiceUser(LPCTSTR szUser, ServiceUser& user);
	BOOL SearchItems(const D3SearchCondition& condition, CArray<D3SearchItem*>& result);
	BOOL SaveItem( D3SearchItem& record, BOOL bIsCustomRecord=FALSE );
	BOOL DeleteItem(DWORD dwItemId);
	BOOL GetSchedules(CArray<D3Schedule*>& result);
	BOOL SaveSchedule(D3Schedule& schedule);
	BOOL DeleteSchedule(DWORD dwScheduleId);
	BOOL GetGameUsers(CArray<GameUser>& result);
	BOOL GetGameUser( LPCTSTR szUser, GameUser& user );
	BOOL SaveGameUser(GameUser& user);
	BOOL FindNextGameUser(GameUser& foundUser);
private:
	DataManager(void);
	virtual ~DataManager(void);
	static DataManager s_instance; 
	CString m_szHostStr;
	CString m_szDbServer;
	DWORD m_dwDbPort;
	CString m_szDbUser;
	CString m_szDbPass;
	CString m_szDbName;

	void throwError(void* m_mysql, LPCTSTR szFunc);
	CString stripSlashes(LPCTSTR sz);
	CString generateSearchSql(const D3SearchCondition& condition);
	void* GetConnection();
	BSMap<DWORD, DWORD, void*, void*> m_mapConnections;
};
