#pragma once
#include "ISocketPack.h"

class D3GetMoneyResultPack :
	public ISocketPack
{
public:
	DWORD m_dwResult;
	LONGLONG m_llMoney;

	D3GetMoneyResultPack() 
	{
		m_llMoney = 0;
	};
	virtual ~D3GetMoneyResultPack(void){};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		UINT nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_llMoney = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(LONGLONG);
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		if (maxlen<10)
		{
			return FALSE;
		}
		ZeroMemory(buf, maxlen);
		bufLen = 0;
		*(WORD*)buf = D3PACKTYPE::D3SC_GETMONEY_RESULT;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(LONGLONG*)(buf+bufLen) = m_llMoney;
		bufLen+=sizeof(LONGLONG);
		return TRUE;
	};

};
