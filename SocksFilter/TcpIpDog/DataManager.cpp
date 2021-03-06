
#include "DataManager.h"
#include "winsock.h" 
#include "mysql.h" 

DataManager DataManager::s_instance;


DataManager& DataManager::Instance()
{
	return s_instance;
}

DataManager::DataManager(void)
{
}

DataManager::~DataManager(void)
{
}

BOOL DataManager::Initialize(LPCTSTR szServer, DWORD dwPort, LPCTSTR szUser, LPCTSTR szPass, LPCTSTR szDbName)
{
	m_szDbServer = szServer;
	m_dwDbPort = dwPort;
	m_szDbUser = szUser;
	m_szDbPass = szPass;
	m_szDbName = szDbName;
	CString sz;
	sz.Format("tcp://%s:%d", m_szDbServer, m_dwDbPort);
	m_szHostStr = sz.GetBuffer();

	try 
	{
		MYSQL* pMysql = (MYSQL*)GetConnection();

		return TRUE;  
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

void* DataManager::GetConnection()
{
	BSAutoLock lock(m_mapConnections.GetMutex());
	void* p = NULL;
	MYSQL* pMysql = NULL;
	if(!m_mapConnections.GetAt(GetCurrentThreadId(), p))
	{
		pMysql = new MYSQL;
		mysql_init(pMysql);

		mysql_options(pMysql, MYSQL_SET_CHARSET_NAME, "gbk");//utf8

		my_bool my_true= TRUE;
		mysql_options(pMysql, MYSQL_OPT_RECONNECT, &my_true);

		if(!mysql_real_connect(pMysql, m_szDbServer, m_szDbUser, m_szDbPass, m_szDbName, m_dwDbPort,NULL,0))
		{
			AfxMessageBox(_T("数据库连接失败!"));
			return FALSE;
		}
		mysql_query(pMysql, "SET NAMES 'GBK'");
		m_mapConnections.SetAt(GetCurrentThreadId(), pMysql);
	}
	else
	{
		pMysql = (MYSQL*)p;
		if(mysql_ping(pMysql)!=0)
		{
			D3Service::Instance().Log("Mysql Ping %s", mysql_error(pMysql));
		}
	}
	return pMysql;
}

void DataManager::CleanUp()
{
	BSAutoLock lock(m_mapConnections.GetMutex());
	for (int i=0;i<m_mapConnections.GetCount();i++)
	{
		void* p = NULL;
		if(m_mapConnections.GetByIndex(i, p))
		{
			mysql_close((MYSQL*)p);
		}
	}
}

CString DataManager::stripSlashes(LPCTSTR sz)
{
	CString szInput = sz;
	szInput.Replace("'", "\\'");
	return szInput;
}

void DataManager::throwError(void* m_mysql, LPCTSTR szFunc)
{
	CString szErr;
	szErr.Format("MysqlError Func:%s : %u: %s\n", szFunc, mysql_errno((MYSQL*)m_mysql), mysql_error((MYSQL*)m_mysql));
	//throw BSException(szErr);
}

BOOL DataManager::DeleteItem( DWORD dwItemId )
{
	BOOL bSuccess = FALSE;
	/*MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	szSql.Format("delete item where ItemId = '%d' limit 1", dwItemId);
	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "DeleteItem::query");
	}
	szSql.Format("delete itemproperty where ItemId = '%d'", dwItemId);
	ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "DeleteItem::query");
	}*/
	bSuccess = TRUE;
	return bSuccess;
}
//
//BOOL DataManager::SaveSchedule( D3Schedule& schedule )
//{
//	BOOL bSuccess = FALSE;
//	/*MYSQL* pMysql = (MYSQL*)GetConnection();
//
//	CString szSql;
//	if (schedule.dwScheduleId==0 || schedule.dwScheduleId==-1)
//	{
//		szSql.Format("insert into schedule set ScheduleName='%s',BeginTime='%d',EndTime='%d', \
//					 RepeatType='%d', RepeatParam='%d', IsEnabled='%d', LastExecTime='%d',\
//					 RetriedTimes='%d', LastError='%d', Operation='%d', OperationParam='%s',\
//					 Description='%s'",
//					 stripSlashes(schedule.szName.c_str()), schedule.dwBeginTime, schedule.dwEndTime,
//					 schedule.repeatType, schedule.dwRepeatParam, schedule.bIsEnabled, schedule.dwLastExecTime,
//					 0, schedule.dwLastError, schedule.operationType, stripSlashes(schedule.szOperationParam.c_str()),
//					 stripSlashes(schedule.szDescription.c_str()));
//		int ret = mysql_query(pMysql, szSql);
//		if (ret!=0)
//		{
//			throwError(pMysql, "SaveSchedule::query");
//		}
//		schedule.dwScheduleId = mysql_insert_id(pMysql);
//	}
//	else
//	{
//		szSql.Format("update schedule set ScheduleName='%s',BeginTime='%d',EndTime='%d', \
//					 RepeatType='%d', RepeatParam='%d', IsEnabled='%d', LastExecTime='%d',\
//					 RetriedTimes='%d', LastError='%d', Operation='%d', OperationParam='%s',\
//					 Description='%s'\
//					 where ScheduleId = '%d'",
//					 stripSlashes(schedule.szName.c_str()), schedule.dwBeginTime, schedule.dwEndTime,
//					 schedule.repeatType, schedule.dwRepeatParam, schedule.bIsEnabled, schedule.dwLastExecTime,
//					 0, schedule.dwLastError, schedule.operationType, stripSlashes(schedule.szOperationParam.c_str()),
//					 stripSlashes(schedule.szDescription.c_str()),
//					 schedule.dwScheduleId);
//		int ret = mysql_query(pMysql, szSql);
//		if (ret!=0)
//		{
//			throwError(pMysql, "SaveSchedule::query");
//		}
//	}*/
//	bSuccess = TRUE;
//	return bSuccess;
//}

BOOL DataManager::DeleteSchedule( DWORD dwScheduleId )
{
	BOOL bSuccess = FALSE;
	/*MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	szSql.Format("delete from schedule where scheduleid = '%d' limit 1", dwScheduleId);
	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "DeleteSchedule::query");
	}*/
	bSuccess = TRUE;
	return bSuccess;
}
//
//BOOL DataManager::GetSchedules( CArray<D3Schedule*>& result )
//{
//	BOOL bSuccess = FALSE;
//	//CString szSql = "select * from schedule order by scheduleid desc";
//
//	//MYSQL* pMysql = (MYSQL*)GetConnection();
//
//	//int ret = mysql_query(pMysql, szSql);
//	//if (ret!=0)
//	//{
//	//	throwError(pMysql, "GetSchedules::query");
//	//}
//	//MYSQL_RES *mysql_res = mysql_store_result(pMysql);
//	//if (mysql_res==NULL)
//	//{
//	//	throwError(pMysql, "GetSchedules::store result");
//	//}
//	////ASSERT(mysql_num_fields(mysql_res)==10);
//	//int num_rows = mysql_num_rows(mysql_res);
//	//if (num_rows>0)
//	//{
//	//	MYSQL_ROW row;
//	//	while(row = mysql_fetch_row(mysql_res))
//	//	{
//	//		D3Schedule* pSchedule = new D3Schedule();
//	//		DataMapping::MapSchedule(row, *pSchedule);
//	//		result.Add(pSchedule);
//	//	}
//	//}
//
//	//mysql_free_result(mysql_res);
//
//	bSuccess = TRUE;
//	return bSuccess;
//}
