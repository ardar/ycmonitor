#pragma once
#include "ISocketPack.h"
#include "SocketPackUtil.h"

class D3ScheduleUpdatePack :
	public ISocketPack
{
public:
	D3ErrorCode m_errorCode;
	D3Schedule m_schedule;

	D3ScheduleUpdatePack() 
	{
		m_errorCode = D3ErrorCode::ERR_Success;
	};
	virtual ~D3ScheduleUpdatePack(void)
	{
	};

	virtual BOOL FromBuffer( BYTE* buf, int len )
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_errorCode = (D3ErrorCode)(*(DWORD*)(buf+nIndex));
		nIndex += 4;

		SocketPackUtil::ReadSchedule(buf, nIndex, m_schedule);

		return TRUE;
	};

	virtual BOOL ToBuffer( BYTE* buf, int maxlen, int& bufLen )
	{
		if (maxlen<14)
		{
			return FALSE;
		}
		ZeroMemory(buf, maxlen);
		bufLen = 0;
		*(WORD*)buf = D3PACKTYPE::D3SC_SCHEDULE_UPDATED;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = m_errorCode;
		bufLen+=4;
		return SocketPackUtil::WriteSchedule(m_schedule, buf, maxlen, bufLen);
	};

private:

};
