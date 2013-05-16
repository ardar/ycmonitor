#pragma once

class D3DoSellPack : public ISocketPack
{
public:
	VOID* m_pItemKey;
	LONGLONG m_llStartPrice;
	LONGLONG m_llBuyoutPrice;
	D3DoSellPack()
	{
		m_pItemKey = NULL;
		m_llStartPrice = m_llBuyoutPrice = 0;
	}
	~D3DoSellPack()
	{
	}

	virtual BOOL FromBuffer(BYTE* buf, int len)
	{
		UINT nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_pItemKey = (VOID*)(*(DWORD*)(buf+nIndex));
		nIndex+=sizeof(DWORD);
		m_llStartPrice = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(LONGLONG);
		m_llBuyoutPrice = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(LONGLONG);
		return TRUE;
	};

	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen)
	{
		if (maxlen<26)
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)(buf) = D3CS_DOSELL_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = (DWORD)m_pItemKey;
		bufLen+=4;
		*(LONGLONG*)(buf+bufLen) = m_llStartPrice;
		bufLen+=sizeof(LONGLONG);
		*(LONGLONG*)(buf+bufLen) = m_llBuyoutPrice;
		bufLen+=sizeof(LONGLONG);
		return TRUE;
	};
};