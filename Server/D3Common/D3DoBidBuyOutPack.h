#pragma once

class D3DoBidBuyOutPack : public ISocketPack
{
public:
	BOOL m_bIsBuyOut;// Is BuyOut or Bid
	GUID m_guid;
	BYTE* m_pItemData;
	int m_nItemDataLen;
	LONGLONG m_llPrice;
	D3DoBidBuyOutPack(BOOL bIsBuyOut=FALSE)
	{
		m_bIsBuyOut = bIsBuyOut;
		m_pItemData = NULL;
		m_nItemDataLen = m_llPrice = 0;
	}
	~D3DoBidBuyOutPack()
	{
		if (m_pItemData)
		{
			delete [] m_pItemData;
			m_pItemData = NULL;
			m_nItemDataLen = 0;
		}
	}

	virtual BOOL FromBuffer(BYTE* buf, int len)
	{
		UINT nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_bIsBuyOut = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_llPrice = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(LONGLONG);
		m_guid = *(GUID*)(buf+nIndex);
		nIndex+=sizeof(GUID);
		m_nItemDataLen = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_pItemData = new BYTE[m_nItemDataLen];
		memcpy_s(m_pItemData, m_nItemDataLen, buf+nIndex, m_nItemDataLen);
		nIndex+=m_nItemDataLen;
		return TRUE;
	};

	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen)
	{
		if (maxlen<18+m_nItemDataLen + sizeof(GUID))
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)(buf) = D3CS_DOBIDBUYOUT_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_bIsBuyOut;
		bufLen+=4;
		*(LONGLONG*)(buf+bufLen) = m_llPrice;
		bufLen+=sizeof(LONGLONG);
		*(GUID*)(buf+bufLen) = m_guid;
		bufLen += sizeof(GUID);
		*(int*)(buf+bufLen) = m_nItemDataLen;
		bufLen+=4;
		memcpy_s(buf+bufLen, maxlen-bufLen, m_pItemData, m_nItemDataLen);
		bufLen+=m_nItemDataLen;
		return TRUE;
	};
};