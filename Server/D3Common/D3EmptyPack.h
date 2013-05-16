#pragma once
#include "..\D3Common\ISocketPack.h"

class D3EmptyPack :
	public ISocketPack
{
public:
	WORD m_wPackType;
	D3EmptyPack(WORD wPackType)
	{
		m_wPackType = wPackType;
	};
	virtual ~D3EmptyPack(void){};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		m_dwRequestId = *(DWORD*)(buf);
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		int nTotalLen = sizeof(WORD)+4;
		if (maxlen<nTotalLen)
		{
			return FALSE;
		}
		ZeroMemory(buf, maxlen);
		*(WORD*)buf = m_wPackType;
		bufLen = 2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		return TRUE;
	};
};
