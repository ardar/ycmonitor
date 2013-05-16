#pragma once
#include <afxmt.h>
#include <afxtempl.h>

template < class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class _declspec(dllexport) BSMap
{
public:
	BSMap(){}
	virtual ~BSMap(){}
	void InitSize(DWORD dwCount){if(dwCount>0)m_map.InitHashTable(dwCount, true);}
	int GetCount() {Lock();int ret=m_map.GetCount();UnLock();return ret;}
	BOOL IsEmpty() {Lock();BOOL ret=m_map.IsEmpty();UnLock();return ret;}
	BOOL GetAt(ARG_KEY key, VALUE& rValue) {Lock();BOOL ret=m_map.Lookup(key,rValue);UnLock();return ret;}
	const VALUE *PGetAt(ARG_KEY key) const;
	void SetAt(ARG_KEY key, ARG_VALUE newValue)
	{
		Lock();
		VALUE tv;
		if(!m_map.Lookup(key, tv))		
		{
			m_index.Add(key);
		}
		m_map.SetAt(key,newValue);
		UnLock();
	}
	BOOL RemoveKey(ARG_KEY key)
	{
		Lock();
		BOOL ret=m_map.RemoveKey(key);
		for (int i=0;i<m_index.GetCount();i++)
		{
			ARG_KEY dkey=m_index.GetAt(i);
			if (dkey==key)
			{
				m_index.RemoveAt(i);
				break;
			}
		}
		UnLock();
		return ret;
	}
	void RemoveAll(){Lock();m_index.RemoveAll();m_map.RemoveAll();UnLock();}	
	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}

	POSITION StartPosition() {return m_map.GetStartPosition();}
	void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const
	{
		m_map.GetNextAssoc(rNextPosition,rKey,rValue);
	}

	BOOL GetKeyByIndex(UINT nIndex, ARG_KEY& key)
	{
		BOOL bSucess = FALSE;
		Lock();
		if((INT)nIndex>=m_index.GetCount())
		{
			UnLock();
			return FALSE;
		}
		key=m_index.GetAt(nIndex);
		UnLock();
		return TRUE;
	}
	BOOL GetByIndex(UINT nIndex, VALUE& value)
	{
		Lock();
		if((INT)nIndex>=m_index.GetCount())
		{
			UnLock();
			return FALSE;
		}
		ARG_KEY key=m_index.GetAt(nIndex);
		if (m_map.Lookup(key,value))
		{
			UnLock();
			return TRUE;
		}
		UnLock();
		return FALSE;
	}
	VALUE* PGetByIndex(UINT nIndex)
	{
		Lock();
		CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* pPair = NULL;
		if((INT)nIndex>=m_index.GetCount())
		{
			UnLock();
			return FALSE;
		}
		ARG_KEY key=m_index.GetAt(nIndex);
		pPair = m_map.PLookup(key);
		UnLock();
		if (pPair != NULL)
		{
			return &pPair->value;
		}
		return NULL;
	}

	VALUE* PGetAt(ARG_KEY key)
	{
		Lock();
		CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* pPair = m_map.PLookup(key);
		UnLock();
		if (pPair != NULL)
		{
			return &pPair->value;
		}
		return NULL;
	}
	CMutex* GetMutex(){return &m_Lock;};
private:
	CMutex m_Lock;
	CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> m_map;
	CArray<KEY, ARG_KEY> m_index;
};
