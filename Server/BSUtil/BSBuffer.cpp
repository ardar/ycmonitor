#include "StdAfx.h"
#include "BSBuffer.h"

BOOL BSBuffer::SetMaxLength( UINT nMaxLen )
{
	if(m_bBind)return FALSE;
	if(m_pbuf==NULL)
	{
		m_pbuf=malloc(nMaxLen);
		if(m_pbuf==NULL)
		{
			m_nMaxLen=0;
			m_nUseLen=0;
			return FALSE;
		}
		m_nMaxLen=nMaxLen;
	}else
	{
		if (m_nMaxLen>=nMaxLen)
		{
			if(m_nUseLen>nMaxLen)
			{
				m_nUseLen=nMaxLen;
			}
			
		}
		else
		{
			void *tempbuf=malloc(m_nUseLen);
			memcpy(tempbuf,m_pbuf,m_nUseLen);
			this->m_pbuf=realloc(this->m_pbuf,nMaxLen);
			if (this->m_pbuf==NULL)
			{
				free(tempbuf);
				m_nMaxLen=0;
				m_nUseLen=0;
				return FALSE;
			}
			memcpy(m_pbuf,tempbuf,m_nUseLen);
			free(tempbuf);
			m_nMaxLen=nMaxLen;
		}	
	}
	return TRUE;
}

BSBuffer::BSBuffer(BYTE *buf,UINT nByteCount)
{
	m_nUseLen=0;
	m_pbuf=buf;
	m_nMaxLen=nByteCount;
	m_bBind=TRUE;
// 	m_pbuf=NULL;
// 	m_nMaxLen=0;
// 	m_nUseLen=0;
// 	m_bBind=FALSE;
// 	if (buf!=NULL)
// 	{
// 		m_pbuf=malloc(nByteCount);
// 		if(m_pbuf==NULL)
// 		{
// 			m_pbuf=NULL;
// 			return;
// 		}
// 		memcpy(m_pbuf,buf,nByteCount);
// 		m_nMaxLen=m_nUseLen=nByteCount;
// 	}

}
BSBuffer::BSBuffer( VOID )
{
	m_pbuf=NULL;
	m_nUseLen=0;
	m_bBind=FALSE;
	m_nMaxLen=0;
}
BSBuffer::BSBuffer( UINT nMaxLen )
{
	m_pbuf=malloc(nMaxLen);
	if(m_pbuf!=NULL)m_nMaxLen=nMaxLen;
	m_nUseLen=0;
	m_bBind=FALSE;
	
}

BSBuffer::~BSBuffer(void)
{
	if(!m_bBind && m_pbuf!=NULL) free(m_pbuf);
	m_pbuf = NULL;
	m_nUseLen=0;
	m_nMaxLen=0;
}
VOID * BSBuffer::GetBuffer()
{
	return m_pbuf;
}

UINT BSBuffer::GetUseLength()
{
	return m_nUseLen;
}
UINT BSBuffer::GetMaxLength()
{
	return m_nMaxLen;
}
VOID * BSBuffer::GetBufferPos( UINT nPos )
{
	if (nPos>m_nMaxLen)return NULL;
	return (BYTE *)m_pbuf+nPos;
}
BSBuffer::BSBuffer( BSBuffer& src)
{
	m_pbuf = NULL;
	m_nMaxLen = 0;
	m_nUseLen = 0;
	m_pbuf=malloc(src.GetMaxLength());
	if (m_pbuf==NULL)return;
	memcpy(m_pbuf,src.GetBuffer(),src.GetUseLength());
	m_nMaxLen=src.GetMaxLength();
	m_nUseLen=src.GetUseLength();
}
BSBuffer& BSBuffer::operator=( BSBuffer& src)
{
	if (m_bBind)
	{
		return *this;
	}
	this->m_pbuf=realloc(this->m_pbuf,src.GetMaxLength());
	if (this->m_pbuf==NULL)return *this;
	memcpy(this->m_pbuf,src.GetBuffer(),src.GetUseLength());
	this->m_nMaxLen=src.GetMaxLength();
	this->m_nUseLen=src.GetUseLength();
	return *this;
}
BSBuffer& BSBuffer::operator+=( BSBuffer& src)
{
	*this=*this+src;
	return *this;
}
BSBuffer& BSBuffer::operator+( BSBuffer& src)
{
	if (m_bBind)
	{
		return *this;
	}
	VOID *tempbuf=malloc(this->GetUseLength());
	memcpy(tempbuf,this->GetBuffer(),this->GetUseLength());
	UINT templen=this->GetUseLength();
	UINT OldMaxlen=this->GetMaxLength();
	
	this->m_pbuf=realloc(this->m_pbuf,src.GetMaxLength()+this->GetMaxLength());
	if (this->m_pbuf==NULL)
	{
		free(tempbuf);
		return *this;
	}

	memcpy(this->m_pbuf,tempbuf,templen);
	memcpy(((BYTE *)this->m_pbuf)+templen,src.GetBuffer(),src.GetUseLength());
	this->m_nMaxLen=OldMaxlen+src.GetMaxLength();
	this->m_nUseLen=templen+src.GetUseLength();
	free(tempbuf);
	return *this;
}
BYTE BSBuffer::operator[](UINT nIndex)
{
	if(nIndex>m_nMaxLen)return 0;
	return ((BYTE *)this->m_pbuf)[nIndex];
}

WORD BSBuffer::GetWord(UINT nPos)
{
	VOID *p=GetBufferPos(nPos);
	if(p==NULL)return 0;
	if(nPos+2>m_nUseLen)return 0;
	return *(WORD *)p;
}
DWORD BSBuffer::GetDword(UINT nPos)
{
	VOID *p=GetBufferPos(nPos);
	if(p==NULL)return 0;
	if(nPos+4>m_nUseLen)return 0;
	return *(DWORD *)p;
}
INT64 BSBuffer::GetInt64(UINT nPos)
{
	VOID *p=GetBufferPos(nPos);
	if(p==NULL)return 0;
	if(nPos+8>m_nUseLen)return 0;
	return *(INT64 *)p;
}
float BSBuffer::GetFloat(UINT nPos)
{
	VOID *p=GetBufferPos(nPos);
	if(p==NULL)return 0;
	if(nPos+4>m_nUseLen)return 0;
	return *(float *)p;
}
double BSBuffer::GetDouble(UINT nPos)
{
	VOID *p=GetBufferPos(nPos);
	if(p==NULL)return 0;
	if(nPos+8>m_nUseLen)return 0;
	return *(double *)p;
}

void BSBuffer::Clean()
{
	m_nUseLen=0;
}

BOOL BSBuffer::SetUseLength( UINT nUseLen )
{
	if (nUseLen<=m_nMaxLen)
	{
		m_nUseLen=nUseLen;
		return TRUE;
	}
	return FALSE;
}

