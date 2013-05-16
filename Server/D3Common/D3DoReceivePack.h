#pragma once

class D3DoReceivePack : public ISocketPack
{
public:
	GUID m_itemGUID;
	D3DoReceivePack()
	{
		ZeroMemory(&m_itemGUID, sizeof(m_itemGUID));
	}
	~D3DoReceivePack()
	{
	}

	virtual BOOL FromBuffer(BYTE* buf, int len)
	{
		UINT nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_itemGUID = *(GUID*)(buf+nIndex);
		nIndex+=sizeof(GUID);
		return TRUE;
	};

	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen)
	{
		if (maxlen<6+sizeof(GUID))
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)(buf) = D3CS_DORECEIVE_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(GUID*)(buf+bufLen) = (GUID)m_itemGUID;
		bufLen+=sizeof(GUID);

		return TRUE;
	};
};