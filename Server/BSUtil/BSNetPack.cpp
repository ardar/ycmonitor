#include "stdafx.h"
#include "BSUtil.h"

BSNetPack::BSNetPack(UINT maxBufLen, UINT initBufLen, UINT headerLen)
{
	HeaderLen = headerLen;
	PackLen = 0;
	ReceivedLen = 0;
	MaxBufferLen = maxBufLen;
	BufferLen = initBufLen;
	Buffer = new BYTE[BufferLen];
	ZeroMemory(Buffer, BufferLen);
};
BSNetPack::~BSNetPack()
{
	if (Buffer)
	{
		delete[] Buffer;
	}
};
void BSNetPack::ClearBuffer()
{
	BSAutoLock lock(&m_mutexBuffer);
	PackLen = 0;
	ReceivedLen = 0;
	ZeroMemory(Buffer, BufferLen);
};
BOOL BSNetPack::SaveToBuffer(BYTE* buf, UINT nReadLen)
{
	BSAutoLock lock(&m_mutexBuffer);
	if (ReceivedLen+nReadLen>BufferLen)
	{
		if(!ExpendBuffer(ReceivedLen+nReadLen))
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
	memcpy(Buffer+ReceivedLen, buf, nReadLen);
	ReceivedLen+=nReadLen;
	return TRUE;
};
UINT BSNetPack::ParsePackLen()
{
	BSAutoLock lock(&m_mutexBuffer);
	if (ReceivedLen>=HeaderLen)
	{
		PackLen = *(DWORD*)(Buffer);
	}
	else
	{
		PackLen = 0;
	}
	return PackLen;
};
BOOL BSNetPack::IsHeaderFinished()
{
	BSAutoLock lock(&m_mutexBuffer);
	return PackLen>0 || ReceivedLen>=HeaderLen;
};
BOOL BSNetPack::IsPackFinished()
{
	BSAutoLock lock(&m_mutexBuffer);
	return PackLen>0&&ReceivedLen==PackLen;
};
UINT BSNetPack::GetHeaderLen()
{
	BSAutoLock lock(&m_mutexBuffer);
	return HeaderLen;
};
UINT BSNetPack::GetPackLen()
{
	BSAutoLock lock(&m_mutexBuffer);
	return PackLen;
};
BYTE* BSNetPack::GetPackBuffer()
{
	BSAutoLock lock(&m_mutexBuffer);
	if (IsPackFinished())
	{
		return Buffer;
	}
	else
	{
		return NULL;
	}
};

BOOL BSNetPack::SplitPack(BYTE* recvBuffer, UINT nRecvLen, UINT& nSavedLen)
{
	BSAutoLock lock(&m_mutexBuffer);
	//TRACE("IntoSplitPack:%X len:%X\n:", *recvBuffer, nRecvLen);
	// ����RecvBuffer
	for (UINT i=0;i<nRecvLen;)
	{
		// δȡ�õ�ǰ������ʱ
		if (!IsHeaderFinished())
		{
			// �����ͷ����
			UINT nRealHeaderLen = GetHeaderLen();
			// ��δ�չ���ͷ����ʱ
			if (ReceivedLen <  nRealHeaderLen)
			{
				int tempcopylen = nRealHeaderLen - ReceivedLen;
				if (nRecvLen-i<tempcopylen)
				{
					tempcopylen=nRecvLen-i;
					SaveToBuffer((recvBuffer+i), tempcopylen);
					i+=tempcopylen;
					continue;
				}
				SaveToBuffer((recvBuffer+i), tempcopylen);
				i+=tempcopylen;
			}				
			// ��ȡ�����ȣ�������ͷ�Ͱ����ݣ�
			ParsePackLen();
			//TRACE("TotalLen:%d\n", nLen);
		}
		if ( GetPackLen() < GetHeaderLen())
		{
			// pack len Error Handling 
			ASSERT(FALSE);
			TRACE("ErrorPackLen:%d%X, len:%d\n", *(BYTE*)(Buffer), GetPackLen());
			BSLogger::SysLog(LOG_ERROR, "���ĳ��Ȳ���ȷ %u", GetPackLen());
			ClearBuffer();
			return FALSE;
			break;
		}

		// ���ηְ������ڵ�ǰrecv����buffer����Զ�ȡ�ĳ���
		int nPackThisRead = (nRecvLen-i) < (PackLen-ReceivedLen) ? (nRecvLen-i) : (PackLen-ReceivedLen);

		//TRACE("PackThisRead:%d\n", nPackThisRead);

		if(PackLen>MaxBufferLen || ReceivedLen+nPackThisRead>MaxBufferLen)
		{
			// pack len too big, drop it.
			ASSERT(FALSE);
			i += nPackThisRead;
			TRACE("DropTooBigPack:%X, len:%d\n", *(BYTE*)(Buffer), PackLen);
			BSLogger::SysLog(LOG_ERROR, "���ĳ��ȹ��� %u", PackLen);
			ClearBuffer();
			return FALSE;
			break;
		}

		SaveToBuffer(recvBuffer+i, nPackThisRead);
		i += nPackThisRead;
		nSavedLen = i;

		// �յ�һ����������
		if (IsPackFinished())
		{
			return TRUE;
		}
	}
	return TRUE;
};

BOOL BSNetPack::ExpendBuffer(UINT nNewLen)
{
	BSAutoLock lock(&m_mutexBuffer);
	if (nNewLen>MaxBufferLen)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	else if (nNewLen<=BufferLen)
	{
		ASSERT(FALSE);
		return TRUE;
	}
	BYTE* newBuf = new BYTE[nNewLen];
	if (newBuf)
	{
		if(PackLen>0 && ReceivedLen>0)
		{
			memcpy_s(newBuf, nNewLen, Buffer, ReceivedLen);
		}
		delete [] Buffer;
		BufferLen = nNewLen;
		Buffer = newBuf;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL BSNetPack::Lock()
{
	return m_mutexBuffer.Lock();
}
BOOL BSNetPack::Unlock()
{
	return m_mutexBuffer.Unlock();
}
