#pragma once
class BSBuffer
{
public:
	BSBuffer(BYTE *buf,UINT nByteCount);
	BSBuffer(VOID);
	BSBuffer( UINT nMaxLen );
	BSBuffer( BSBuffer& src);
	BOOL SetMaxLength(UINT nMaxLen);
	BOOL SetUseLength(UINT nUseLen);
	void Clean();
	VOID *GetBufferPos(UINT nPos);
	VOID * GetBuffer();
	UINT GetUseLength();
	UINT GetMaxLength();

	WORD GetWord(UINT nPos);
	DWORD GetDword(UINT nPos);
	INT64 GetInt64(UINT nPos);
	float GetFloat(UINT nPos);
	double GetDouble(UINT nPos);
	BOOL AddEmpty(UINT len)
	{
		int nlen=len;
		if(m_nMaxLen-nlen<m_nUseLen)
		{
			if (!SetMaxLength(m_nMaxLen+nlen*2))return FALSE;
		}
		memset((BYTE *)m_pbuf+m_nUseLen,0,len);
		m_nUseLen+=nlen;
		return TRUE;
	}
	template<typename RT>
	BOOL Add(RT rt)
	{
		int nlen=sizeof(RT);
		if(m_nMaxLen-nlen<m_nUseLen)
		{
			if (!SetMaxLength(m_nMaxLen+nlen*2))return FALSE;
		}
		*(RT *)((BYTE *)m_pbuf+m_nUseLen)=rt;
		m_nUseLen+=nlen;
		return TRUE;
	}
	BOOL Add(BYTE *buf,int len)
	{
		int nlen=len;
		if(m_nMaxLen-nlen<m_nUseLen)
		{
			if (!SetMaxLength(m_nMaxLen+nlen*2))return FALSE;
		}
		memcpy((BYTE *)m_pbuf+m_nUseLen,buf,len);
		m_nUseLen+=nlen;
		return TRUE;
	}
	~BSBuffer(void);
	BSBuffer& operator=(BSBuffer& src);
	BSBuffer& operator+(BSBuffer& src);
	BSBuffer& operator+=(BSBuffer& src);
	BYTE operator[](UINT nIndex);


private:
	BOOL m_bBind;
	VOID *m_pbuf;
	UINT m_nUseLen;
	UINT m_nMaxLen;
};


