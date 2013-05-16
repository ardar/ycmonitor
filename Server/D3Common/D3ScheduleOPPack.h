#pragma once

class D3ScheduleOPPack : public ISocketPack
{
public:
	enum ScheduleOP
	{
		OPNone,
		OPAdd,
		OPEdit,
		OPDelete,
	};
	D3Schedule m_record;
	ScheduleOP m_op;

	D3ScheduleOPPack()
	{
		m_op = OPNone;
	}
	~D3ScheduleOPPack()
	{
	}

	virtual BOOL FromBuffer(BYTE* buf, int len)
	{
		int nIndex = 0;
		m_dwRequestId = *(DWORD*)(buf+nIndex);
		nIndex+=4;
		m_op = (ScheduleOP)*(DWORD*)(buf+nIndex);
		nIndex+=4;
		SocketPackUtil::ReadSchedule(buf, nIndex, m_record);
		return TRUE;
	};

	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen)
	{
		if (maxlen<22)
		{
			return FALSE;
		}
		bufLen = 0;
		*(WORD*)(buf) = D3CS_SCHEDULEOP_REQUEST;
		bufLen+=2;
		*(DWORD*)(buf+bufLen) = m_dwRequestId;
		bufLen+=4;
		*(DWORD*)(buf+bufLen) = (DWORD)m_op;
		bufLen+=sizeof(DWORD);
		return SocketPackUtil::WriteSchedule(m_record, buf, maxlen, bufLen);
	};
};