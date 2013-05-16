#include "stdafx.h"
#include "SocketPackUtil.h"

void SocketPackUtil::ReadItemProperty( const BYTE* buf, int& nIndex, D3ItemProperty& field )
{
	field.dwDataId = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	field.dwItemDataId = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	int nNameLen = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	strcpy_s(field.szName, sizeof(field.szName), (char*)(buf+nIndex));
	nIndex+=nNameLen;

	int nValueLen = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	strcpy_s(field.szValue, sizeof(field.szValue), (char*)(buf+nIndex));
	nIndex+=nValueLen;

	field.nMinValue = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	field.nMaxValue = *(DWORD*)(buf+nIndex);
	nIndex+=4;
}

BOOL SocketPackUtil::WriteItemProperty( const D3ItemProperty& field, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex < 24+ strlen(field.szName) + strlen(field.szValue) + 2)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = field.dwDataId;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = field.dwItemDataId;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = strlen(field.szName)+1;
	nIndex+=4;
	strcpy_s((char*)(buf+nIndex), nMaxLen-nIndex, field.szName);
	nIndex+= strlen(field.szName)+1;
	*(DWORD*)(buf+nIndex) = strlen(field.szValue)+1;
	nIndex+=4;
	strcpy_s((char*)(buf+nIndex), nMaxLen-nIndex, field.szValue);
	nIndex+= strlen(field.szValue)+1;
	*(DWORD*)(buf+nIndex) = field.nMinValue;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = field.nMaxValue;
	nIndex+=4;
	return TRUE;
}

void SocketPackUtil::ReadItemInfo( const BYTE* buf, int& nIndex, D3ItemInfo& item )
{
	memcpy_s(&item.guid, sizeof(item.guid), buf+nIndex, sizeof(item.guid));
	nIndex+=sizeof(item.guid);
	strcpy_s(item.szName, sizeof(item.szName), (char*)(buf+nIndex));
	nIndex+=sizeof(item.szName);
	strcpy_s(item.szEquipType, sizeof(item.szEquipType), (char*)(buf+nIndex));
	nIndex+=sizeof(item.szEquipType);
	// Low Data
	item.nLowDataLen = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.pLowData = new BYTE[item.nLowDataLen];
	memcpy_s(item.pLowData, item.nLowDataLen, buf+nIndex, item.nLowDataLen);
	nIndex+=item.nLowDataLen;
	// Low Property
	/*item.nLowPropertyLen = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.pLowProperty = new char[item.nLowPropertyLen];
	memcpy_s(item.pLowProperty, item.nLowPropertyLen, buf+nIndex, item.nLowPropertyLen);
	nIndex+=item.nLowPropertyLen;*/

	int nPropertyCount = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	for (int i=0;i<nPropertyCount;i++)
	{
		D3ItemProperty field;
		ReadItemProperty(buf, nIndex, field);
		item.properties.push_back(field);
	}
}

BOOL SocketPackUtil::WriteItemInfo( const D3ItemInfo& info, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex< sizeof(info.guid) + 4 + sizeof(info.szName) + sizeof(info.szEquipType) 
		+ info.nLowDataLen+4 + info.nLowPropertyLen+4) 
	{
		return FALSE;
	}
	memcpy_s(buf+nIndex, sizeof(info.guid), &info.guid, sizeof(info.guid));
	nIndex += sizeof(info.guid);
	memcpy_s(buf+nIndex, sizeof(info.szName), info.szName, sizeof(info.szName));
	nIndex += sizeof(info.szName);
	memcpy_s(buf+nIndex, sizeof(info.szEquipType), info.szEquipType, sizeof(info.szEquipType));
	nIndex += sizeof(info.szEquipType);

	*(DWORD*)(buf+nIndex) = info.nLowDataLen;
	nIndex+=4;
	memcpy_s(buf+nIndex, nMaxLen-nIndex, info.pLowData, info.nLowDataLen);
	nIndex+=info.nLowDataLen;

	/**(DWORD*)(buf+nIndex) = info.nLowPropertyLen;
	nIndex+=4;
	memcpy_s(buf+nIndex, nMaxLen-nIndex, info.pLowProperty, info.nLowPropertyLen);
	nIndex+=info.nLowPropertyLen;*/

	int nPropertyCount = info.properties.size();
	*(DWORD*)(buf+nIndex) = nPropertyCount;
	nIndex+=4;

	std::vector<D3ItemProperty>::const_iterator it;
	for (it = info.properties.begin(); it != info.properties.end();it++)
	{
		const D3ItemProperty& field = *it;
		if(!WriteItemProperty(field, buf, nMaxLen, nIndex))
		{
			return FALSE;
		}
	}
	return TRUE;
}

void SocketPackUtil::ReadDepotItem( const BYTE* buf, int& nIndex, D3DepotItem& item )
{
	item.nItemNum = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.nPackagePos = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.npos = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.npos2 = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.bCanSel = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.nUseSpace = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.pKey = (VOID*)*(DWORD*)(buf+nIndex);
	nIndex+=4;
	ReadItemInfo(buf, nIndex, item.info);
}

BOOL SocketPackUtil::WriteDepotItem( const D3DepotItem& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex<28)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = item.nItemNum;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.nPackagePos;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.npos;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.npos2;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.bCanSel;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.nUseSpace;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = (DWORD)item.pKey;
	nIndex+=4;
	return WriteItemInfo(item.info, buf, nMaxLen, nIndex);
}

void SocketPackUtil::ReadCompletedItem( const BYTE* buf, int& nIndex, D3CompletedItem& item )
{
	item.nIndex = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	ReadItemInfo(buf, nIndex, item.info);
}

BOOL SocketPackUtil::WriteCompletedItem( const D3CompletedItem& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex < 4)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = item.nIndex;
	nIndex+=4;
	return WriteItemInfo(item.info, buf, nMaxLen, nIndex);
}

void SocketPackUtil::ReadSellingItem( const BYTE* buf, int& nIndex, D3SellingItem& item )
{
	item.lEndTime = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.llCurBid = *(DWORD*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.llBuyOut = *(DWORD*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.sellStatus = (D3AuctionSellStatus)*(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.nIndex = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	ReadItemInfo(buf, nIndex, item.info);
}

BOOL SocketPackUtil::WriteSellingItem( const D3SellingItem& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex < 28)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = item.lEndTime;
	nIndex+=4;
	*(LONGLONG*)(buf+nIndex) = item.llCurBid;
	nIndex+=sizeof(LONGLONG);
	*(LONGLONG*)(buf+nIndex) = item.llBuyOut;
	nIndex+=sizeof(LONGLONG);
	*(DWORD*)(buf+nIndex) = item.sellStatus;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.nIndex;
	nIndex+=4;
	return WriteItemInfo(item.info, buf, nMaxLen, nIndex);
}

void SocketPackUtil::ReadBiddingItem( const BYTE* buf, int& nIndex, D3BiddingItem& item )
{
	item.lEndTime = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.llCurBid = *(LONGLONG*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.llBuyOut = *(LONGLONG*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.llMyBid = *(LONGLONG*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.llLowestPrice = *(LONGLONG*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.status = (D3AuctionBidStatus)*(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.nIndex = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	ReadItemInfo(buf, nIndex, item.info);
}

BOOL SocketPackUtil::WriteBiddingItem( D3BiddingItem& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex < 44)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = item.lEndTime;
	nIndex+=4;
	*(LONGLONG*)(buf+nIndex) = item.llCurBid;
	nIndex+=sizeof(LONGLONG);
	*(LONGLONG*)(buf+nIndex) = item.llBuyOut;
	nIndex+=sizeof(LONGLONG);
	*(LONGLONG*)(buf+nIndex) = item.llMyBid;
	nIndex+=sizeof(LONGLONG);
	*(LONGLONG*)(buf+nIndex) = item.llLowestPrice;
	nIndex+=sizeof(LONGLONG);
	*(DWORD*)(buf+nIndex) = item.status;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.nIndex;
	nIndex+=4;
	return WriteItemInfo(item.info, buf, nMaxLen, nIndex);
}

void SocketPackUtil::ReadHistoryItem( const BYTE* buf, int& nIndex, D3HistoryItem& item )
{
	item.nIndex = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	ReadItemInfo(buf, nIndex, item.info);
}

BOOL SocketPackUtil::WriteHistoryItem( const D3HistoryItem& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex < 28)
	{
		return FALSE;
	}
	*(__time32_t*)(buf+nIndex) = item.lTime;
	nIndex+=sizeof(__time32_t);
	*(LONGLONG*)(buf+nIndex) = item.llTradeId;
	nIndex+=sizeof(LONGLONG);
	*(DWORD*)(buf+nIndex) = (DWORD)item.hStatus;
	nIndex+=4;
	*(LONGLONG*)(buf+nIndex) = item.llPrice;
	nIndex+=sizeof(LONGLONG);
	*(DWORD*)(buf+nIndex) = item.nIndex;
	nIndex+=4;
	return WriteItemInfo(item.info, buf, nMaxLen, nIndex);
}

BOOL SocketPackUtil::WriteSearchItem( const D3SearchItem& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex < 36)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = item.dwDataId;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.lEndTime;
	nIndex+=4;
	*(LONGLONG*)(buf+nIndex) = item.llCurBid;
	nIndex+=sizeof(LONGLONG);
	*(LONGLONG*)(buf+nIndex) = item.llBuyOut;
	nIndex+=sizeof(LONGLONG);
	*(LONGLONG*)(buf+nIndex) = item.llLowestPrice;
	nIndex+=sizeof(LONGLONG);
	*(DWORD*)(buf+nIndex) = item.status;
	nIndex+=4;
	return WriteItemInfo(item.info, buf, nMaxLen, nIndex);
}

void SocketPackUtil::ReadSearchItem( const BYTE* buf, int& nIndex, D3SearchItem& item )
{
	item.dwDataId = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.lEndTime = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.llCurBid = *(LONGLONG*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.llBuyOut = *(LONGLONG*)(buf+nIndex);
	nIndex+=sizeof(LONGLONG);
	item.llLowestPrice = *(LONGLONG*)(buf+nIndex); 
	nIndex+=sizeof(LONGLONG);
	item.status = (D3emSearchItemStatus)*(DWORD*)(buf+nIndex);
	nIndex+=4;
	ReadItemInfo(buf, nIndex, item.info);
}

void SocketPackUtil::ReadSchedule( const BYTE* buf, int& nIndex, D3Schedule& item )
{
	item.dwScheduleId = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.repeatType = (D3Schedule::D3RepeatType)*(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.dwRepeatParam = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.operationType = (D3Schedule::D3OpType)*(DWORD*)(buf+nIndex);
	nIndex+=4;

	WORD dwNameLen = *(WORD*)(buf+nIndex);
	nIndex+=2;
	item.szName = (char*)(buf+nIndex);
	nIndex+=dwNameLen;

	WORD dwParamLen = *(WORD*)(buf+nIndex);
	nIndex+=2;
	item.szOperationParam = (char*)(buf+nIndex);
	nIndex+=dwParamLen;

	WORD dwDescLen = *(WORD*)(buf+nIndex);
	nIndex+=2;
	item.szDescription = (char*)(buf+nIndex);
	nIndex+=dwDescLen;

	item.dwBeginTime = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.dwEndTime = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.dwLastExecTime = *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.dwLastError =  *(DWORD*)(buf+nIndex);
	nIndex+=4;
	item.bIsEnabled =  *(DWORD*)(buf+nIndex);
	nIndex+=4;
}

BOOL SocketPackUtil::WriteSchedule( const D3Schedule& item, BYTE* buf, int nMaxLen, int& nIndex )
{
	if (nMaxLen-nIndex<42)
	{
		return FALSE;
	}
	*(DWORD*)(buf+nIndex) = item.dwScheduleId;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.repeatType;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.dwRepeatParam;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.operationType;
	nIndex+=4;

	int nNameLen = item.szName.size();
	*(WORD*)(buf+nIndex) = nNameLen+1;
	nIndex+=2;
	memcpy_s(buf+nIndex, nMaxLen-nIndex, item.szName.c_str(), nNameLen);
	nIndex+=nNameLen;
	*(BYTE*)(buf+nIndex) = 0;
	nIndex+=1;

	int nParamLen = item.szOperationParam.size();
	*(WORD*)(buf+nIndex) = nParamLen+1;
	nIndex+=2;
	memcpy_s(buf+nIndex, nMaxLen-nIndex, item.szOperationParam.c_str(), nParamLen);
	nIndex+=nParamLen;
	*(BYTE*)(buf+nIndex) = 0;
	nIndex+=1;

	int nDescLen = item.szDescription.size();
	*(WORD*)(buf+nIndex) = nDescLen+1;
	nIndex+=2;
	memcpy_s(buf+nIndex, nMaxLen-nIndex, item.szDescription.c_str(), nDescLen);
	nIndex+=nDescLen;
	*(BYTE*)(buf+nIndex) = 0;
	nIndex+=1;

	*(DWORD*)(buf+nIndex) = item.dwBeginTime;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.dwEndTime;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.dwLastExecTime;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.dwLastError;
	nIndex+=4;
	*(DWORD*)(buf+nIndex) = item.bIsEnabled;
	nIndex+=4;
}
