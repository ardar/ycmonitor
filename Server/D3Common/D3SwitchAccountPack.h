#pragma once

class D3SwitchAccountPack : public ISocketPack
{
public:
	CString m_szAccount;
	CString m_szPassword;

	D3SwitchAccountPack()
	{
	}
	~D3SwitchAccountPack()
	{
	}

	virtual BOOL FromBuffer(BYTE* buf, int len)
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		DWORD dwNameLen = *(DWORD*)(buf+nIndex);
		nIndex+=sizeof(DWORD);
		m_szAccount = (char*)(buf+nIndex);
		nIndex+=dwNameLen;
		DWORD dwPassLen = *(LONGLONG*)(buf+nIndex);
		nIndex+=sizeof(DWORD);
		m_szPassword = (char*)(buf+nIndex);
		nIndex+=dwPassLen;
		return TRUE;
	};

	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen)
	{
		if (maxlen<14)
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)(buf) = D3CS_SWITCHACC_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_szAccount.GetLength()+1;
		bufLen+=4;
		strcpy_s((char*)(buf+bufLen), maxlen-bufLen, m_szAccount.GetBuffer());
		bufLen+=m_szAccount.GetLength()+1;

		*(DWORD*)(buf+bufLen) = m_szPassword.GetLength()+1;
		bufLen+=4;
		strcpy_s((char*)(buf+bufLen), maxlen-bufLen, m_szPassword.GetBuffer());
		bufLen+=m_szPassword.GetLength()+1;
	};
};