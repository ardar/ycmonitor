#pragma once
#include "ISocketPack.h"

class D3AuthPack :
	public ISocketPack
{
public:
	CString m_szUser;
	CString m_szPass;

	virtual ~D3AuthPack(void){};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		UINT nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		WORD wNameLen = *(WORD*)(buf+nIndex);
		nIndex+=2;
		m_szUser = (TCHAR*)(buf+nIndex);
		nIndex+= wNameLen;
		WORD wPassLen = *(WORD*)(buf+nIndex);
		nIndex+=2;
		m_szPass = (TCHAR*)(buf+nIndex);
		nIndex+=wPassLen;
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		int nTotalLen = 4+ sizeof(WORD)*2 + m_szUser.GetLength()+1 + m_szPass.GetLength()+1;
		if (maxlen<nTotalLen)
		{
			return FALSE;
		}
		ZeroMemory(buf, maxlen);
		*(WORD*)buf = D3PACKTYPE::D3CS_AUTH_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(WORD*)(buf+bufLen) = m_szUser.GetLength()+1;
		bufLen+=2;
		strcpy_s((char*)(buf+bufLen), maxlen-bufLen, m_szUser.GetBuffer());
		bufLen+= m_szUser.GetLength()+1;
		*(WORD*)(buf+bufLen) = m_szPass.GetLength()+1;
		bufLen+=2;
		strcpy_s((char*)(buf+bufLen), maxlen-bufLen, m_szPass.GetBuffer());
		bufLen+= m_szPass.GetLength()+1;
		return TRUE;
	};


};
