#pragma once

class ISocketPack
{
public:
	DWORD m_dwSessionId;
	DWORD m_dwRequestId;
	virtual BOOL FromBuffer(BYTE* buf, int len) = 0;
	virtual BOOL ToBuffer(BYTE* buf, int maxlen, int& bufLen) = 0;
};