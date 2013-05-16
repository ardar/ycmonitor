#pragma once

class D3GetScheduleCmd : public IHandlerCmd
{
public:
	D3GetSchedulePack* m_pPack;

	D3GetScheduleCmd();
	virtual ~D3GetScheduleCmd();
	virtual CString GetName() { return CString("≤È‘É”ãÑù»ŒÑ’"); };

	virtual IHandlerCmd* CreateInstance() 
	{
		return new D3GetScheduleCmd();
	}

	virtual void ReadPack(DWORD dwSessionId, BYTE* buf, int len);

	virtual void Execute();

	virtual void Cancel( DWORD dwErrorCode );
};