#pragma once

class D3SearchItemsResultPack :
	public ISocketPack
{
public:
	DWORD m_errorCode;
	BOOL m_bSearchInLocal;
	CArray<D3SearchItem*> m_itemList;
	BOOL m_bFinished;//是否已发送完所有数据

	D3SearchItemsResultPack()
	{
		m_errorCode = m_bSearchInLocal = m_bFinished = 0;
	}

	virtual ~D3SearchItemsResultPack(void)
	{
		for (int i=0;i<m_itemList.GetCount();i++)
		{
			delete m_itemList.GetAt(i);
		}
		m_itemList.RemoveAll();
	};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_errorCode = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_bSearchInLocal = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_bFinished = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		int nItemCount = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		for(int i=0;i<nItemCount;i++)
		{
			D3SearchItem* item = new D3SearchItem();
			SocketPackUtil::ReadSearchItem(buf, nIndex, *item);
			m_itemList.Add(item);
		}
		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		if (maxlen-bufLen<22)
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)buf = D3PACKTYPE::D3SC_SEARCHITEMS_RESULT;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_errorCode;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_bSearchInLocal;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_bFinished;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_itemList.GetCount();
		bufLen+=4;
		for(int i=0;i<m_itemList.GetCount();i++)
		{
			D3SearchItem* item = m_itemList.GetAt(i);
			BOOL bRet = SocketPackUtil::WriteSearchItem(*item, buf, maxlen, bufLen);
			if (!bRet)
			{
				return FALSE;
			}
		}
		return TRUE;
	};

};
