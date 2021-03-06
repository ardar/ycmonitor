#include "StdAfx.h"
#include "DataManager.h"
#include "DataMapping.h"
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
	throw BSException(szErr);
}

BOOL DataManager::GetGameUser( LPCTSTR szUser, GameUser& user )
{
	BOOL bFound = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	szSql.Format("select * from gameacc where username = '%s'", szUser);
	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "GetGameAccount::query");
	}
	MYSQL_RES *mysql_res = mysql_store_result(pMysql);
	if (mysql_res==NULL)
	{
		throwError(pMysql, "GetGameAccount::store result");
	}
	ASSERT(mysql_num_fields(mysql_res)==5);
	int num_rows = mysql_num_rows(mysql_res);
	if (num_rows>0)
	{
		MYSQL_ROW row = mysql_fetch_row(mysql_res);
		if (row)
		{
			DataMapping::MapGameUser(row, user);
		}
	}

	mysql_free_result(mysql_res);

	bFound = TRUE;
	return bFound;
}

BOOL DataManager::GetServiceUser( LPCTSTR szUser, ServiceUser& user )
{
	BOOL bFound = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	szSql.Format("select * from user where username = '%s'", szUser);
	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "GetServiceUser::query");
	}
	MYSQL_RES *mysql_res = mysql_store_result(pMysql);
	if (mysql_res==NULL)
	{
		throwError(pMysql, "GetServiceUser::store result");
	}
	ASSERT(mysql_num_fields(mysql_res)==5);
	int num_rows = mysql_num_rows(mysql_res);
	if (num_rows>0)
	{
		MYSQL_ROW row = mysql_fetch_row(mysql_res);
		if (row)
		{
			DataMapping::MapServiceUser(row, user);
			bFound = TRUE;
		}
	}

	mysql_free_result(mysql_res);

	return bFound;
}

BOOL DataManager::SaveItem( D3SearchItem& record, BOOL bIsCustomRecord/*=FALSE*/)
{
	BOOL bSuccess = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();
	GUID emptyGuid;
	ZeroMemory(&emptyGuid, sizeof(emptyGuid));
	if (memcmp(&record.info.guid, &emptyGuid, sizeof(GUID))==0)
	{
		::CoInitialize(NULL);
		HRESULT hRes = ::CoCreateGuid(&record.info.guid);
		ASSERT(hRes==S_OK);
		::CoUninitialize();
	}
	CString szGUID = ItemParser::Instance().GUID2Str(record.info.guid);
	CString szSql;
	if (record.dwDataId==0)
	{
		szSql.Format("select * from item where Guid = '%s'", szGUID);
		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveItem::query");
		}
		MYSQL_RES *mysql_res = mysql_store_result(pMysql);
		if (mysql_res==NULL)
		{
			throwError(pMysql, "SaveItem::store result");
		}
		if ( mysql_num_rows(mysql_res)>0) 
		{
			MYSQL_ROW row = mysql_fetch_row(mysql_res);
			if (row)
			{
				record.dwDataId = BSHelper::Str2Int(row[0]);
			}
		}
		mysql_free_result(mysql_res);
	}
	record.status = bIsCustomRecord ? D3emSearchItemStatus::D3CUSTOMRECORD : record.status;

	CString szItemName;
	szItemName = stripSlashes(record.info.szName);
	CString szPropertyText = record.info.pLowProperty ? stripSlashes(record.info.pLowProperty) : "";
	if (record.dwDataId==0)
	{
		szSql.Format("insert into item set CurBid='%I64d',BuyOut='%I64d',LowestPrice='%I64d',\
					 ItemName='%s', EquipType='%s',\
					 Guid='%s', Status='%d', RecordTime='%d', PropertyText='%s'", 
					 record.llCurBid, record.llBuyOut, record.llLowestPrice, 
					 szItemName.GetBuffer(),record.info.szEquipType,
					 szGUID.GetBuffer(), record.status, (DWORD)time(0), szPropertyText.GetBuffer());
	}
	else
	{
		szSql.Format("update item set CurBid='%I64d',BuyOut='%I64d',LowestPrice='%I64d',\
					 ItemName='%s', EquipType='%s',\
					 Guid='%s', Status='%d', RecordTime='%d', PropertyText='%s'\
					 where ItemId = '%d'", 
					 record.llCurBid, record.llBuyOut, record.llLowestPrice, 
					 szItemName.GetBuffer(),record.info.szEquipType,
					 szGUID.GetBuffer(), record.status, (DWORD)time(0), szPropertyText.GetBuffer(),
					 record.dwDataId);
	}
	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "SaveItem:: update/insert");
	}
	if (record.dwDataId>0)
	{
		szSql.Format("delete from itemproperty where itemid='%d'", record.dwDataId);
		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveItem::delete properties");
		}
	}
	else
	{
		record.dwDataId = mysql_insert_id(pMysql);
	}
	// Save the properties
	vector<D3ItemProperty>::iterator it;
	for (it = record.info.properties.begin();it!=record.info.properties.end();it++)
	{
		D3ItemProperty& field = *it;
		field.dwItemDataId = record.dwDataId;

		szSql.Format("insert into itemproperty set ItemId='%d', Name='%s', Value='%s', MinValue='%d', MaxValue='%d'", 
			field.dwItemDataId, field.szName, field.szValue, field.nMinValue, field.nMaxValue);

		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveItem::insert/update properties");
		}
		field.dwDataId = mysql_insert_id(pMysql);
	}


	bSuccess = TRUE;
	return bSuccess;
}

BOOL DataManager::DeleteItem( DWORD dwItemId )
{
	BOOL bSuccess = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();

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
	}
	bSuccess = TRUE;
	return bSuccess;
}

BOOL DataManager::SaveSchedule( D3Schedule& schedule )
{
	BOOL bSuccess = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	if (schedule.dwScheduleId==0 || schedule.dwScheduleId==-1)
	{
		szSql.Format("insert into schedule set ScheduleName='%s',BeginTime='%d',EndTime='%d', \
					 RepeatType='%d', RepeatParam='%d', IsEnabled='%d', LastExecTime='%d',\
					 RetriedTimes='%d', LastError='%d', Operation='%d', OperationParam='%s',\
					 Description='%s'",
					 stripSlashes(schedule.szName.c_str()), schedule.dwBeginTime, schedule.dwEndTime,
					 schedule.repeatType, schedule.dwRepeatParam, schedule.bIsEnabled, schedule.dwLastExecTime,
					 0, schedule.dwLastError, schedule.operationType, stripSlashes(schedule.szOperationParam.c_str()),
					 stripSlashes(schedule.szDescription.c_str()));
		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveSchedule::query");
		}
		schedule.dwScheduleId = mysql_insert_id(pMysql);
	}
	else
	{
		szSql.Format("update schedule set ScheduleName='%s',BeginTime='%d',EndTime='%d', \
					 RepeatType='%d', RepeatParam='%d', IsEnabled='%d', LastExecTime='%d',\
					 RetriedTimes='%d', LastError='%d', Operation='%d', OperationParam='%s',\
					 Description='%s'\
					 where ScheduleId = '%d'",
					 stripSlashes(schedule.szName.c_str()), schedule.dwBeginTime, schedule.dwEndTime,
					 schedule.repeatType, schedule.dwRepeatParam, schedule.bIsEnabled, schedule.dwLastExecTime,
					 0, schedule.dwLastError, schedule.operationType, stripSlashes(schedule.szOperationParam.c_str()),
					 stripSlashes(schedule.szDescription.c_str()),
					 schedule.dwScheduleId);
		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveSchedule::query");
		}
	}
	bSuccess = TRUE;
	return bSuccess;
}

BOOL DataManager::DeleteSchedule( DWORD dwScheduleId )
{
	BOOL bSuccess = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	szSql.Format("delete from schedule where scheduleid = '%d' limit 1", dwScheduleId);
	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "DeleteSchedule::query");
	}
	bSuccess = TRUE;
	return bSuccess;
}

BOOL DataManager::GetSchedules( CArray<D3Schedule*>& result )
{
	BOOL bSuccess = FALSE;
	CString szSql = "select * from schedule order by scheduleid desc";

	MYSQL* pMysql = (MYSQL*)GetConnection();

	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "GetSchedules::query");
	}
	MYSQL_RES *mysql_res = mysql_store_result(pMysql);
	if (mysql_res==NULL)
	{
		throwError(pMysql, "GetSchedules::store result");
	}
	//ASSERT(mysql_num_fields(mysql_res)==10);
	int num_rows = mysql_num_rows(mysql_res);
	if (num_rows>0)
	{
		MYSQL_ROW row;
		while(row = mysql_fetch_row(mysql_res))
		{
			D3Schedule* pSchedule = new D3Schedule();
			DataMapping::MapSchedule(row, *pSchedule);
			result.Add(pSchedule);
		}
	}

	mysql_free_result(mysql_res);

	bSuccess = TRUE;
	return bSuccess;
}

BOOL DataManager::SearchItems( const D3SearchCondition& condition, CArray<D3SearchItem*>& result )
{
	CString szSql = generateSearchSql(condition);

	MYSQL* pMysql = (MYSQL*)GetConnection();

	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "SearchItems::query");
	}
	MYSQL_RES *mysql_res = mysql_store_result(pMysql);
	if (mysql_res==NULL)
	{
		throwError(pMysql, "SearchItems::store result");
	}
	//ASSERT(mysql_num_fields(mysql_res)==9);
	CString szIdStr;
	BSMap<DWORD, DWORD, D3SearchItem*, D3SearchItem*> mapItems;
	int num_rows = mysql_num_rows(mysql_res);
	if (num_rows>0)
	{
		MYSQL_ROW row;
		while(row = mysql_fetch_row(mysql_res))
		{
			D3SearchItem* pItem = new D3SearchItem();
			DataMapping::MapItem(row, *pItem);
			mapItems.SetAt(pItem->dwDataId, pItem);
			if (szIdStr.GetLength()>0)
			{
				szIdStr.AppendFormat(",%d", pItem->dwDataId);
			}
			else
			{
				szIdStr.AppendFormat("%d", pItem->dwDataId);
			}
		}
	}

	mysql_free_result(mysql_res);

	// Query item properties of these items.
	if(szIdStr.GetLength()>0)
	{
		CString szSubSql;
		szSubSql.Format("select * from itemproperty where itemid in (%s) order by itemid, linenumber", szIdStr);
		int ret = mysql_query(pMysql, szSubSql);
		if (ret!=0)
		{
			throwError(pMysql, "SearchItems::query properties");
		}
		MYSQL_RES *mysql_res = mysql_store_result(pMysql);
		if (mysql_res==NULL)
		{
			throwError(pMysql, "SearchItems::store result properties");
		}
		//ASSERT(mysql_num_fields(mysql_res)==6);
		int num_rows = mysql_num_rows(mysql_res);
		if (num_rows>0)
		{
			MYSQL_ROW row;
			while(row = mysql_fetch_row(mysql_res))
			{
				D3ItemProperty field;
				DataMapping::MapItemProperty(row, field);
				D3SearchItem* pItem = NULL;
				if (mapItems.GetAt(field.dwItemDataId, pItem))
				{
					pItem->info.properties.push_back(field);
				}
			}
		}

		mysql_free_result(mysql_res);
	}
	for (int i=0;i<mapItems.GetCount();i++)
	{
		D3SearchItem* pItem = NULL;
		if(mapItems.GetByIndex(i, pItem))
		{
			result.Add(pItem);
		}
	}
	return TRUE;
}

CString DataManager::generateSearchSql(const D3SearchCondition& condition)
{
	static const int nPerPage = 11;
	static const int nDefaultRetPages = 2;
	CString szFilter, szLimit, szJoin;
	CString szSql;
	int nFetchCount = (condition.nPage<=0) ? nDefaultRetPages*nPerPage : condition.nPage*nPerPage;
	szLimit.Format(" limit 0, %d", nFetchCount);
	if (strlen(condition.szUniqueItemTextBox)>0)
	{
		szFilter.AppendFormat(" and i.ItemName like '\%%s\%'", stripSlashes(condition.szUniqueItemTextBox));
	}
	if (condition.szCharacterFilter[0])
	{
		szJoin.AppendFormat(" left join itemproperty ip1 on (ip1.ItemId=i.ItemId and ip1.Name='%s')",
			Field_Job);
		szFilter.AppendFormat(" and (ip1.Value = '%s' or ip1.value is null)", stripSlashes(condition.szCharacterFilter));
	}
	if (condition.nLevelMin>0)
	{
		szJoin.AppendFormat(" left join itemproperty ip2 on (ip2.ItemId=i.ItemId and ip2.Name='%s')",
			Field_Level);
		szFilter.AppendFormat(" and CAST( ip2.Value AS SIGNED) >= '%d'", condition.nLevelMin);
	}
	if (condition.nLevelMax>0)
	{
		szJoin.AppendFormat(" left join itemproperty ip3 on (ip3.ItemId=i.ItemId and ip3.Name='%s')",
			Field_Level);
		szFilter.AppendFormat(" and CAST( ip3.Value AS SIGNED) <= '%d'", condition.nLevelMax);
	}
	if (condition.szPrimaryFilter[0])
	{
		CString szSec = condition.szPrimaryFilter;
		if (szSec.GetLength()<=4 || szSec.Left(4).Compare("全部")!=0)
		{
			szJoin.AppendFormat(" left join itemproperty ip4 on (ip4.ItemId=i.ItemId and ip4.Name='%s')",
				Field_PrimaryType);
			szFilter.AppendFormat(" and ip4.Value = '%s'", stripSlashes(condition.szPrimaryFilter));
		}
	}
	if (condition.szSecondaryFilter[0])
	{
		CString szSec = condition.szSecondaryFilter;
		if (szSec.GetLength()<=4 || szSec.Left(4).Compare("所有")!=0)
		{
			szJoin.AppendFormat(" left join itemproperty ip5 on (ip5.ItemId=i.ItemId and ip5.Name='%s')",
				Field_SecondaryType);
			szFilter.AppendFormat(" and ip5.Value = '%s'", stripSlashes(condition.szSecondaryFilter));
		}
	}
	if (condition.nBuyoutTextBox>0)
	{
		szFilter.AppendFormat(" and i.Buyout>0 and i.Buyout <= '%d'", condition.nBuyoutTextBox);
	}
	for (int i=0;i<6;i++)
	{
		if (condition.szAdvancedComboBox[i][0] && condition.nAdvancedTextBox[i])
		{
			CString szTblName;
			szTblName.Format("ipp%d", i);
			CString strFilterName = condition.szAdvancedComboBox[i];
			if (strFilterName.Find("平均")==0)
			{
				strFilterName.Replace("平均", "");
				szJoin.AppendFormat(" left join itemproperty %s on (%s.ItemId=i.ItemId and %s.Name = '%s')",
					szTblName, szTblName, szTblName, strFilterName);
				szFilter.AppendFormat(" and (%s.MinValue+%s.MaxValue)/2 >= '%d'", 
					szTblName, szTblName, condition.nAdvancedTextBox[i]);
			}
			else
			{
				szJoin.AppendFormat(" left join itemproperty %s on (%s.ItemId=i.ItemId and %s.Name = '%s')",
					szTblName, szTblName, szTblName, condition.szAdvancedComboBox[i]);
				szFilter.AppendFormat(" and (CAST( %s.Value AS SIGNED) >= %d or CAST( %s.MinValue AS SIGNED)>=%d )", 
					szTblName, condition.nAdvancedTextBox[i], szTblName, condition.nAdvancedTextBox[i]);
			}
		}
	}
	szSql.Format("select i.* from item i %s where 1 %s \
				 order by recordtime desc, ItemId desc %s", szJoin, szFilter, szLimit);
	TRACE("SearchSql: %s\n", szSql);
	return szSql;
}


BOOL DataManager::FindNextGameUser(GameUser& foundUser)
{
	BOOL bFound = FALSE;
	CArray<GameUser> users;
	if(DataManager::Instance().GetGameUsers(users))
	{
		foundUser.dwId = 0;
		for (int i=0;i<users.GetCount();i++)
		{
			GameUser thisUser = users.GetAt(i);
			if (foundUser.dwId==0 || foundUser.dwLastLogin>thisUser.dwLastLogin)
			{
				foundUser = thisUser;
				bFound = TRUE;
			}
		}
	}
	return bFound;
}

BOOL DataManager::GetGameUsers( CArray<GameUser>& result )
{
	BOOL bSuccess = FALSE;
	CString szSql = "select * from gameacc where isenabled=1 order by lastlogin asc";

	MYSQL* pMysql = (MYSQL*)GetConnection();

	int ret = mysql_query(pMysql, szSql);
	if (ret!=0)
	{
		throwError(pMysql, "GetGameUsers::query");
	}
	MYSQL_RES *mysql_res = mysql_store_result(pMysql);
	if (mysql_res==NULL)
	{
		throwError(pMysql, "GetGameUsers::store result");
	}
	//ASSERT(mysql_num_fields(mysql_res)==10);
	int num_rows = mysql_num_rows(mysql_res);
	if (num_rows>0)
	{
		MYSQL_ROW row;
		while(row = mysql_fetch_row(mysql_res))
		{
			GameUser user;
			DataMapping::MapGameUser(row, user);
			result.Add(user);
		}
	}

	mysql_free_result(mysql_res);

	bSuccess = TRUE;
	return bSuccess;
}

BOOL DataManager::SaveGameUser( GameUser& schedule )
{
	BOOL bSuccess = FALSE;
	MYSQL* pMysql = (MYSQL*)GetConnection();

	CString szSql;
	if (schedule.dwId==0 || schedule.dwId==-1)
	{
		szSql.Format("insert into gameacc set username='%s',password='%s',isenabled='%d', \
					 lastlogin='%d'",
					 stripSlashes(schedule.szUsername), stripSlashes(schedule.szPassword), schedule.bIsEnabled,
					 schedule.dwLastLogin);
		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveGameUser::query");
		}
		schedule.dwId = mysql_insert_id(pMysql);
	}
	else
	{
		szSql.Format("update schedule set username='%s',password='%s',isenabled='%d', \
					 lastlogin='%d'\
					 where accid= '%d'",
					 stripSlashes(schedule.szUsername), stripSlashes(schedule.szPassword), schedule.bIsEnabled,
					 schedule.dwLastLogin,
					 schedule.dwId);
		int ret = mysql_query(pMysql, szSql);
		if (ret!=0)
		{
			throwError(pMysql, "SaveGameUser::query");
		}
	}
	bSuccess = TRUE;
	return bSuccess;
}


