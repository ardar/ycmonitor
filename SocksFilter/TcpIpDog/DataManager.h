#pragma once

class DataManager
{
public:
	static DataManager& Instance();

	bool Initialize(LPCTSTR szServer, DWORD dwPort, LPCTSTR szUser, LPCTSTR szPass, LPCTSTR szDbName);
	void CleanUp();

	BOOL DeleteItem(DWORD dwItemId);
	BOOL DeleteSchedule(DWORD dwScheduleId);
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
	void* GetConnection();
	//BSMap<DWORD, DWORD, void*, void*> m_mapConnections;
};
