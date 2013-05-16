#pragma once

class D3SaveItemPack : public ISocketPack
{
public:
	D3ItemInfo m_record;
	LONGLONG m_llBidPrice;
	LONGLONG m_llBuyoutPrice;

	D3SaveItemPack()
	{
		m_llBidPrice = m_llBuyoutPrice = 0;
	}
	~D3SaveItemPack()
	{
	}

	virtual BOOL FromBuffer(BYTE* buf, int len)
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_llBidPrice = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(LONGLONG);
		m_llBuyoutPrice = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(LONGLONG);
		SocketPackUtil::ReadItemInfo(buf, nIndex, m_record);
		return TRUE;
	};

	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen)
	{
		if (maxlen<22)
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)(buf) = D3CS_SAVEITEMDATA_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(LONGLONG*)(buf+bufLen) = m_llBidPrice;
		bufLen+=sizeof(LONGLONG);
		*(LONGLONG*)(buf+bufLen) = m_llBuyoutPrice;
		bufLen+=sizeof(LONGLONG);
		return SocketPackUtil::WriteItemInfo(m_record, buf, maxlen, bufLen);
	};
};