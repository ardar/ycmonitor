#pragma once
#include "ISocketPack.h"

class D3ActionResultPack :
	public ISocketPack
{
public:
	enum ActionType
	{
		ActionDoBid,
		ActionDoBuyout,
		ActionDoSell,
		ActionDoReceive,
		ActionSaveItemData,
		ActionAddSchedule,
		ActionEditSchedule,
		ActionDelSchedule,
		ActionSwitchAccount,
	};
	ActionType m_action;
	DWORD m_dwResult;
	DWORD m_dwResultParam;
	CString m_szResultParam;
	CArray<D3BiddingItem*> m_biddingItemList;

	D3ActionResultPack() 
	{
		m_dwResult = 0;
		m_dwResultParam = 0;
	};
	virtual ~D3ActionResultPack(void){};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_action = (ActionType)*(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_dwResult = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_dwResultParam = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		DWORD dwParamLen = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_szResultParam = (char*)(buf+nIndex);
		nIndex+=dwParamLen;
		int nBiddingItemCount = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		for(int i=0;i<nBiddingItemCount;i++)
		{
			D3BiddingItem* item = new D3BiddingItem();
			SocketPackUtil::ReadBiddingItem(buf, nIndex, *item);
			m_biddingItemList.Add(item);
		}
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		if (maxlen<26+m_szResultParam.GetLength()+1)
		{
			return FALSE;
		}
		ZeroMemory(buf, maxlen);
		bufLen = 0;
		*(WORD*)buf = D3PACKTYPE::D3SC_ACTION_RESULT;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_action;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_dwResult;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_dwResultParam;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_szResultParam.GetLength()+1;
		bufLen+=4;
		strcpy_s((char*)(buf+bufLen), maxlen-bufLen, m_szResultParam.GetBuffer());
		bufLen+=m_szResultParam.GetLength()+1;
		*(DWORD*)(buf+bufLen) = m_biddingItemList.GetCount();
		bufLen+=4;
		for(int i=0;i<m_biddingItemList.GetCount();i++)
		{
			D3BiddingItem* item = m_biddingItemList.GetAt(i);
			BOOL bRet = SocketPackUtil::WriteBiddingItem(*item, buf, maxlen, bufLen);
			if (!bRet)
			{
				return FALSE;
			}
		}
		return TRUE;
	};

};
