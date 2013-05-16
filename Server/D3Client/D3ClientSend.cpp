#include "stdafx.h"
#include "D3ClientEvent.h"

DWORD D3Client::Authenticate(LPCTSTR szUser, LPCTSTR szPass)
{
	BOOL bSuccess = FALSE;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_Auth);
	D3AuthPack pack;
	pack.m_szUser = szUser;
	pack.m_szPass = szPass;
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}
	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_Auth, 30000))
	{
		if (m_waitEvent.GetResult()==0)
		{
			bSuccess = pack.m_dwRequestId;
		}
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_Auth);
	return 0;
}

DWORD D3Client::GetMoney()
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetMoney);
	D3EmptyPack pack(D3CS_GETMONEY_REQUEST);
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetMoney, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetMoney);
	return dwResult;
}

DWORD D3Client::GetDepotItems()
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetDepotItems);
	D3EmptyPack pack(D3CS_GETDEPOT_REQUEST);
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}
	
	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetDepotItems, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetDepotItems);
	return dwResult;
}

DWORD D3Client::GetFinishedItems()
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetFinishedItems);
	D3EmptyPack pack(D3CS_GETFINISHED_REQUEST);
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetFinishedItems, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetFinishedItems);
	return dwResult;
}

DWORD D3Client::GetSellingItems()
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetSellingItems);
	D3EmptyPack pack(D3CS_GETSELLING_REQUEST);
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetSellingItems, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetSellingItems);
	return dwResult;
}

DWORD D3Client::GetBiddingItems()
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetBiddingItems);
	D3EmptyPack pack(D3CS_GETBIDDING_REQUEST);
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetBiddingItems, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetBiddingItems);
	return dwResult;
}

DWORD D3Client::GetHistoryItems()
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetHistoryItems);
	D3EmptyPack pack(D3CS_GETHISTORY_REQUEST);
	pack.m_dwRequestId = createRequestId();
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetHistoryItems, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetHistoryItems);
	return dwResult;
}

DWORD D3Client::SearchItems(const D3SearchCondition& condition)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_SearchItems);
	D3SearchItemsPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_bSearchInLocal = FALSE;
	memcpy_s(&pack.m_condition, sizeof(pack.m_condition), &condition, sizeof(condition));
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_SearchItems, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_SearchItems);
	return dwResult;
}

DWORD D3Client::SearchItemsInLocal(const D3SearchCondition& condition)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_SearchItemsInLocal);
	D3SearchItemsPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_bSearchInLocal = TRUE;
	memcpy_s(&pack.m_condition, sizeof(pack.m_condition), &condition, sizeof(condition));
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_SearchItemsInLocal, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_SearchItemsInLocal);
	return dwResult;
}

DWORD D3Client::DoBid(const D3ItemInfo& item, LONGLONG nBidPrice)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_DoBid);
	D3DoBidBuyOutPack pack(FALSE);
	pack.m_bIsBuyOut = FALSE;
	pack.m_dwRequestId = createRequestId();
	pack.m_llPrice = nBidPrice;
	pack.m_guid = item.guid;
	pack.m_nItemDataLen = item.nLowDataLen;
	pack.m_pItemData = new BYTE[item.nLowDataLen];
	memcpy_s(pack.m_pItemData, pack.m_nItemDataLen, item.pLowData, item.nLowDataLen);
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_DoBid, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_DoBid);
	return dwResult;
}

DWORD D3Client::DoBuyOut(const D3ItemInfo& item)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_DoBuyOut);
	D3DoBidBuyOutPack pack(TRUE);
	pack.m_bIsBuyOut = TRUE;
	pack.m_dwRequestId = createRequestId();
	pack.m_guid = item.guid;
	pack.m_llPrice = 0;
	pack.m_nItemDataLen = item.nLowDataLen;
	pack.m_pItemData = new BYTE[item.nLowDataLen];
	memcpy_s(pack.m_pItemData, pack.m_nItemDataLen, item.pLowData, item.nLowDataLen);
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_DoBuyOut, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_DoBuyOut);
	return dwResult;
}

DWORD D3Client::DoSell(const D3DepotItem& item, LONGLONG nStartPrice,LONGLONG nBuyoutPrice)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_DoSell);
	D3DoSellPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_pItemKey = item.pKey;
	pack.m_llStartPrice = nStartPrice;
	pack.m_llBuyoutPrice = nBuyoutPrice;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_DoSell, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_DoSell);
	return dwResult;
}

DWORD D3Client::DoReceive(const D3ItemInfo& item)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_DoReceive);
	D3DoReceivePack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_itemGUID = item.guid;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_DoReceive, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_DoReceive);
	return dwResult;
}

DWORD D3Client::SaveItemData(const D3ItemInfo& item, LONGLONG llBidPrice,LONGLONG llBuyoutPrice)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_SaveItem);
	D3SaveItemPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_record = item;
	pack.m_llBidPrice = llBidPrice;
	pack.m_llBuyoutPrice = llBuyoutPrice;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_SaveItem, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_SaveItem);
	return dwResult;
}

DWORD D3Client::GetSchedules( )
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_GetSchedules);
	D3GetSchedulePack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_wPackType = D3CS_GETSCHEDULES_REQUEST;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_GetSchedules, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_GetSchedules);
	return dwResult;
}

DWORD D3Client::AddSchedule(const D3Schedule& schedule)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_AddSchedule);
	D3ScheduleOPPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_record = schedule;
	pack.m_op = D3ScheduleOPPack::OPAdd;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_AddSchedule, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_AddSchedule);
	return dwResult;
}

DWORD D3Client::EditSchedule(const D3Schedule& schedule)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_EditSchedule);
	D3ScheduleOPPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_record = schedule;
	pack.m_op = D3ScheduleOPPack::OPEdit;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_EditSchedule, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_EditSchedule);
	return dwResult;
}

DWORD D3Client::DelSchedule(const D3Schedule& schedule)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_DelSchedule);
	D3ScheduleOPPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_record = schedule;
	pack.m_op = D3ScheduleOPPack::OPDelete;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_DelSchedule, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_DelSchedule);
	return dwResult;
}

DWORD D3Client::SwitchAccount(const D3GameAccount& acc)
{
	DWORD dwResult = ERR_Timeout;
	m_waitEvent.StartEvent(D3ClientEvent::D3Event_SwitchAccount);
	D3SwitchAccountPack pack;
	pack.m_dwRequestId = createRequestId();
	pack.m_szAccount = acc.szAccount;
	pack.m_szPassword = acc.szPassword;
	SendPack(&pack);
	if (!m_bIsWaitCallResult)
	{
		return pack.m_dwRequestId;
	}

	if (m_waitEvent.WaitEvent(D3ClientEvent::D3Event_SwitchAccount, 30000))
	{
		dwResult = m_waitEvent.GetResult();
	}
	m_waitEvent.EndEvent(D3ClientEvent::D3Event_SwitchAccount);
	return dwResult;
}
