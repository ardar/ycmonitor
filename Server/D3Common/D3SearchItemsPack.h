#pragma once

class D3SearchItemsPack :
	public ISocketPack
{
public:
	BOOL m_bSearchInLocal;
	D3SearchCondition m_condition;

	D3SearchItemsPack()
	{
		m_bSearchInLocal = FALSE;
	};
	virtual ~D3SearchItemsPack(void){};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		if (len< sizeof(m_condition)+8)
		{
			return FALSE;
		}
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_bSearchInLocal = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		memcpy_s(&m_condition, sizeof(m_condition), buf+nIndex, sizeof(m_condition));
		nIndex+=sizeof(m_condition);
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		*(WORD*)(buf) = D3CS_SEARCHITEMS_REQUEST;
		bufLen = 2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_bSearchInLocal;
		bufLen+=4;
		if (maxlen< sizeof(m_condition) + 10)
		{
			return FALSE;
		}
		memcpy_s(buf+10, maxlen-10, &m_condition, sizeof(m_condition));
		bufLen += sizeof(m_condition);
		return TRUE;
	};


};
