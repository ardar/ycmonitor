#include "stdafx.h"
#include "D3ClientEvent.h"


void D3Client::onRecvAuthResult( BYTE* buf, int len )
{
	D3AuthResultPack pack;
	if(pack.FromBuffer(buf, len))
	{
		if (pack.m_dwResult==0)
		{
			m_bIsAuthenticated = TRUE;
		}
	}
	if (m_pOnClientCallback)
	{
		m_pOnClientCallback(D3ClientEvent::D3Event_Auth, pack.m_dwRequestId, pack.m_dwResult, 0);
	}
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_Auth, pack.m_dwResult);
}

void D3Client::onRecvActionResult( BYTE* buf, int len )
{
	D3ActionResultPack pack;
	if(pack.FromBuffer(buf, len))
	{
		switch (pack.m_action)
		{
		case D3ActionResultPack::ActionDoBuyout:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_DoBuyOut, pack.m_dwRequestId, pack.m_dwResult, 0);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_DoBuyOut, pack.m_dwResult);
			break;
		case D3ActionResultPack::ActionDoBid:
			if (m_pOnClientCallback)
			{
				vector<D3BiddingItem*> vec;
				for (int i=0;i<pack.m_biddingItemList.GetCount();i++)
				{
					vec.push_back(pack.m_biddingItemList.GetAt(i));
				}
				pack.m_biddingItemList.RemoveAll();
				m_pOnClientCallback(D3ClientEvent::D3Event_DoBid, 
					pack.m_dwRequestId, pack.m_dwResult, (LPARAM)&vec);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_DoBid, pack.m_dwResult);
			break;
		case D3ActionResultPack::ActionDoReceive:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_DoReceive, 
					pack.m_dwRequestId, pack.m_dwResult, 0);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_DoReceive, pack.m_dwResult);
			break;
		case D3ActionResultPack::ActionDoSell:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_DoSell, 
					pack.m_dwRequestId, pack.m_dwResult, 0);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_DoReceive, pack.m_dwResult);
			break;
		case D3ActionResultPack::ActionSaveItemData:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_SaveItem, 
					pack.m_dwRequestId, pack.m_dwResult, pack.m_dwResultParam);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_SaveItem, pack.m_dwResult, 0, pack.m_dwResultParam);
			break;
		case D3ActionResultPack::ActionAddSchedule:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_AddSchedule, 
					pack.m_dwRequestId, pack.m_dwResult, pack.m_dwResultParam);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_AddSchedule, pack.m_dwResult, 0, pack.m_dwResultParam);
			break;
		case D3ActionResultPack::ActionEditSchedule:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_EditSchedule, 
					pack.m_dwRequestId,  pack.m_dwResult, 0);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_EditSchedule, pack.m_dwResult);
			break;
		case D3ActionResultPack::ActionDelSchedule:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_DelSchedule, 
					pack.m_dwRequestId, pack.m_dwResult, 0);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_DelSchedule, pack.m_dwResult);
			break;
		case D3ActionResultPack::ActionSwitchAccount:
			if (m_pOnClientCallback)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_SwitchAccount, 
					pack.m_dwRequestId, pack.m_dwResult, (LPARAM)&pack.m_szResultParam);
			}
			m_waitEvent.OnEvent(D3ClientEvent::D3Event_SwitchAccount, pack.m_dwResult);
			break;

		}
	}
}

void D3Client::onRecvGetMoneyResult( BYTE* buf, int len )
{
	DWORD dwResult = ERR_Unknown;
	D3GetMoneyResultPack pack;
	if(pack.FromBuffer(buf, len))
	{
		dwResult = ERR_Success;
	}
	if (m_pOnClientCallback)
	{
		m_pOnClientCallback(D3ClientEvent::D3Event_GetMoney, pack.m_dwRequestId, dwResult, (DWORD)&pack.m_llMoney);
	}
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_GetMoney, dwResult, pack.m_llMoney);
}

void D3Client::onRecvDepotItems( BYTE* buf, int len )
{
	D3GetDepotResultPack* pack = new D3GetDepotResultPack();
	if (pack->FromBuffer(buf, len) && pack->m_errorCode==ERR_Success)
	{
		if (m_pOnClientCallback)
		{
			vector<D3DepotItem*> vec;
			for (int i=0;i<pack->m_itemList.GetCount();i++)
			{
				vec.push_back(pack->m_itemList.GetAt(i));
			}
			pack->m_itemList.RemoveAll();
			m_pOnClientCallback(D3ClientEvent::D3Event_GetDepotItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&vec);
		}
	}
	else
	{
		if (m_pOnClientCallback)
		{
			m_pOnClientCallback(D3ClientEvent::D3Event_GetDepotItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
		}
	}
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_GetDepotItems, pack->m_errorCode);
	delete pack;
}

void D3Client::onRecvFinishedItems( BYTE* buf, int len )
{
	D3GetFinishedResultPack* pack = new D3GetFinishedResultPack();
	if (pack->FromBuffer(buf, len) && pack->m_errorCode==ERR_Success)
	{
		if (m_pOnClientCallback)
		{
			vector<D3CompletedItem*> vec;
			for (int i=0;i<pack->m_itemList.GetCount();i++)
			{
				vec.push_back(pack->m_itemList.GetAt(i));
			}
			pack->m_itemList.RemoveAll();
			m_pOnClientCallback(D3ClientEvent::D3Event_GetFinishedItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&vec);
		}
	}
	else
	{
		if (m_pOnClientCallback)
		{
			m_pOnClientCallback(D3ClientEvent::D3Event_GetFinishedItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
		}
	}
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_GetFinishedItems, pack->m_errorCode);
	delete pack;
}

void D3Client::onRecvBiddingItems( BYTE* buf, int len )
{
	D3GetBiddingResultPack* pack = new D3GetBiddingResultPack();
	if (pack->FromBuffer(buf, len) && pack->m_errorCode==ERR_Success)
	{
		if (m_pOnClientCallback)
		{
			vector<D3BiddingItem*> vec;
			for (int i=0;i<pack->m_itemList.GetCount();i++)
			{
				vec.push_back(pack->m_itemList.GetAt(i));
			}
			pack->m_itemList.RemoveAll();
			m_pOnClientCallback(D3ClientEvent::D3Event_GetBiddingItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&vec);
		}
	}
	else
	{
		if (m_pOnClientCallback)
		{
			m_pOnClientCallback(D3ClientEvent::D3Event_GetBiddingItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
		}
	}
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_GetBiddingItems, pack->m_errorCode);
	delete pack;
}

void D3Client::onRecvSellingItems( BYTE* buf, int len )
{
	D3GetSellingResultPack* pack = new D3GetSellingResultPack();
	if (pack->FromBuffer(buf, len) && pack->m_errorCode==ERR_Success)
	{
		if (m_pOnClientCallback)
		{
			vector<D3SellingItem*> vec;
			for (int i=0;i<pack->m_itemList.GetCount();i++)
			{
				vec.push_back(pack->m_itemList.GetAt(i));
			}
			pack->m_itemList.RemoveAll();
			m_pOnClientCallback(D3ClientEvent::D3Event_GetSellingItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&vec);
		}
	}
	else
	{
		if (m_pOnClientCallback)
		{
			m_pOnClientCallback(D3ClientEvent::D3Event_GetSellingItems, pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
		}
	}
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_GetSellingItems, pack->m_errorCode);
	delete pack;
}

void D3Client::onRecvSearchItems( BYTE* buf, int len )
{
	D3SearchItemsResultPack* pack = new D3SearchItemsResultPack();
	if (pack->FromBuffer(buf, len) && pack->m_errorCode==ERR_Success)
	{
		if (m_pOnClientCallback)
		{
			if (pack->m_dwRequestId!=m_searchItemCacheRequestId)
			{
				m_searchItemsCache.clear();
			}
			m_searchItemCacheRequestId = pack->m_dwRequestId;
			for (int i=0;i<pack->m_itemList.GetCount();i++)
			{
				m_searchItemsCache.push_back(pack->m_itemList.GetAt(i));
			}
			pack->m_itemList.RemoveAll();
			if (pack->m_bFinished)
			{
				if (pack->m_bSearchInLocal)
				{
					m_pOnClientCallback(D3ClientEvent::D3Event_SearchItemsInLocal, 
						pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&m_searchItemsCache);
				}
				else
				{
					m_pOnClientCallback(D3ClientEvent::D3Event_SearchItems, 
						pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&m_searchItemsCache);
				}
				TRACE("Finished pack record count:%d\n", (int)m_searchItemsCache.size());
			}
			else
			{
				TRACE("UnFinished pack record count:%d\n", (int)m_searchItemsCache.size());
			}
		}
	}
	else
	{
		if (m_pOnClientCallback)
		{
			if (pack->m_bSearchInLocal)
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_SearchItemsInLocal, 
					pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
			}
			else
			{
				m_pOnClientCallback(D3ClientEvent::D3Event_SearchItems, 
					pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
			}
			
		}
	}
	delete pack;
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_SearchItems, pack->m_errorCode);
}
void D3Client::onRecvGetSchedules( BYTE* buf, int len )
{
	D3GetScheduleResultPack* pack = new D3GetScheduleResultPack();
	if (pack->FromBuffer(buf, len) && pack->m_errorCode==ERR_Success)
	{
		if (m_pOnClientCallback)
		{
			vector<D3Schedule*> vec;
			for (int i=0;i<pack->m_itemList.GetCount();i++)
			{
				vec.push_back(pack->m_itemList.GetAt(i));
			}
			pack->m_itemList.RemoveAll();
			m_pOnClientCallback(D3ClientEvent::D3Event_GetSchedules, 
						pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&vec);
				TRACE("Finished pack record count:%d\n", (int)vec.size());
		}
	}
	else
	{
		if (m_pOnClientCallback)
		{
			m_pOnClientCallback(D3ClientEvent::D3Event_GetSchedules, 
				pack->m_dwRequestId, pack->m_errorCode, (LPARAM)NULL);
		}
	}
	delete pack;
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_SearchItems, pack->m_errorCode);
}
void D3Client::onRecvScheduleUpdated( BYTE* buf, int len )
{
	D3ScheduleUpdatePack* pack = new D3ScheduleUpdatePack();
	if (pack->FromBuffer(buf, len))
	{
		if (m_pOnClientCallback)
		{
			m_pOnClientCallback(D3ClientEvent::D3Event_ScheduleUpdated, 
				pack->m_dwRequestId, pack->m_errorCode, (LPARAM)&(pack->m_schedule));
			TRACE("onRecvScheduleUpdated:%d\n", (int)pack->m_schedule.dwScheduleId);
		}
	}
	delete pack;
	m_waitEvent.OnEvent(D3ClientEvent::D3Event_SearchItems, pack->m_errorCode);
}