#pragma once
#include "mysql.h" 

class DataMapping
{
public:
	static void MapGameUser(MYSQL_ROW row, GameUser& user)
	{
		user.dwId = BSHelper::Str2Int(row[0]);
		strcpy_s(user.szUsername, sizeof(user.szUsername), row[1]);
		strcpy_s(user.szPassword, sizeof(user.szPassword), row[2]);
		user.bIsEnabled = BSHelper::Str2Int(row[3]);
		user.dwLastLogin = BSHelper::Str2Int(row[4]);
	};
	static void MapServiceUser(MYSQL_ROW row, ServiceUser& user)
	{
		user.dwId = BSHelper::Str2Int(row[0]);
		user.szUser = row[1];
		user.szPass = row[2];
		user.nGroupId = BSHelper::Str2Int(row[3]);
		user.bIsEnabled = BSHelper::Str2Int(row[4]);
	};
	static void MapItem(MYSQL_ROW row, D3SearchItem& item)
	{
		item.dwDataId = BSHelper::Str2Int(row[0]);
		item.info.guid = ItemParser::Instance().Str2GUID(row[1]);
		strcpy_s(item.info.szName, sizeof(item.info.szName), row[2]);
		item.llCurBid =BSHelper::Str2Int(row[4]);
		item.llBuyOut =BSHelper::Str2Int(row[5]);
		item.llLowestPrice =BSHelper::Str2Int(row[6]);
		item.status = (D3emSearchItemStatus) BSHelper::Str2Int(row[7]);
		strcpy_s(item.info.szEquipType, sizeof(item.info.szEquipType), row[8]);
	};
	static void MapItemProperty(MYSQL_ROW row, D3ItemProperty& item)
	{
		item.dwDataId = BSHelper::Str2Int(row[0]);
		item.dwItemDataId = BSHelper::Str2Int(row[1]);
		strcpy_s(item.szName, sizeof(item.szName), row[2]);
		strcpy_s(item.szValue, sizeof(item.szValue), row[3]);
		item.nMinValue = BSHelper::Str2Int(row[4]);
		item.nMaxValue = BSHelper::Str2Int(row[5]);
	};
	static void MapSchedule(MYSQL_ROW row, D3Schedule& schedule)
	{
		schedule.dwScheduleId = BSHelper::Str2Int(row[0]);
		schedule.szName = row[1] ? row[1] : "";
		schedule.dwBeginTime  = BSHelper::Str2Int(row[2]);
		schedule.dwEndTime  = BSHelper::Str2Int(row[3]);
		schedule.repeatType = (D3Schedule::D3RepeatType)BSHelper::Str2Int(row[4]);
		schedule.dwRepeatParam = BSHelper::Str2Int(row[5]);
		schedule.bIsEnabled = BSHelper::Str2Int(row[6]);
		schedule.dwLastExecTime = BSHelper::Str2Int(row[7]);
		schedule.dwLastError = BSHelper::Str2Int(row[9]);
		schedule.operationType = (D3Schedule::D3OpType)BSHelper::Str2Int(row[10]);
		schedule.szOperationParam = row[11] ? row[11] : "";
		schedule.szDescription = row[12] ? row[12] : "";
	};
};
