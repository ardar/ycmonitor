#pragma once
#include "ISocketPack.h"
#include "SocketPackUtil.h"

class D3GetBiddingResultPack :
	public ISocketPack
{
public:
	D3ErrorCode m_errorCode;
	CArray<D3BiddingItem*> m_itemList;

	D3GetBiddingResultPack() 
	{
		m_errorCode = D3ErrorCode::ERR_Success;
	};
	virtual ~D3GetBiddingResultPack(void)
	{
		for (int i=0;i<m_itemList.GetCount();i++)
		{
			delete m_itemList.GetAt(i);
		}
		m_itemList.RemoveAll();
	};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_errorCode = (D3ErrorCode)(*(DWORD*)(buf+nIndex));
		nIndex += 4;
		int nItemCount = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		for (int i=0;i<nItemCount;i++)
		{
			D3BiddingItem* item = new D3BiddingItem();
			SocketPackUtil::ReadBiddingItem(buf, nIndex, *item);
			m_itemList.Add(item);
		}
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		if (maxlen<14)
		{
			return FALSE;
		}
		ZeroMemory(buf, maxlen);
		bufLen = 0;
		*(WORD*)buf = D3PACKTYPE::D3SC_GETBIDDING_RESULT;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_errorCode;
		bufLen+=4;
		int nItemCount = m_itemList.GetCount();
		*(DWORD*)(buf+bufLen) = nItemCount;
		bufLen+=4;
		for (int i=0;i<nItemCount;i++)
		{
			D3BiddingItem* item = m_itemList.GetAt(i);
			SocketPackUtil::WriteBiddingItem(*item, buf, maxlen, bufLen);
		}
		return TRUE;
	};

private:

};
