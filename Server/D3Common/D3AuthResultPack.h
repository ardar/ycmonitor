#pragma once
#include "ISocketPack.h"

class D3AuthResultPack :
	public ISocketPack
{
public:
	DWORD m_dwResult;

	D3AuthResultPack() 
	{
		m_dwResult = 0;
	};
	virtual ~D3AuthResultPack(void){};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		UINT nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_dwResult = *(DWORD*)(buf+nIndex);
		nIndex+=4;
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
		*(WORD*)buf = D3PACKTYPE::D3SC_AUTH_RESULT;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_dwResult;
		bufLen+=4;
		return TRUE;
	};

};
