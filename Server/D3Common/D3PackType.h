#pragma once
#pragma pack (push, 1)

// �ͻ�������������͵ı���type
enum D3PACKTYPE 
{
	LOW_HEARTBEAT = 0,
	D3CS_AUTH_REQUEST,
	D3CS_GETMONEY_REQUEST,
	D3CS_GETDEPOT_REQUEST,
	D3CS_GETFINISHED_REQUEST,
	D3CS_GETSELLING_REQUEST,
	D3CS_GETBIDDING_REQUEST,
	D3CS_GETHISTORY_REQUEST,
	D3CS_SEARCHITEMS_REQUEST,
	D3CS_SEARCHITEMSLOCAL_REQUEST,
	D3CS_DOBIDBUYOUT_REQUEST,
	D3CS_DOBID_REQUEST,
	D3CS_DOSELL_REQUEST,
	D3CS_DORECEIVE_REQUEST,
	D3CS_SAVEITEMDATA_REQUEST,
	D3CS_SWITCHACC_REQUEST,
	D3CS_GETSCHEDULES_REQUEST,
	D3CS_SCHEDULEOP_REQUEST,
	D3CS_ADDSCHEDULE_REQUEST,
	D3CS_EDITSCHEDULE_REQUEST,
	D3CS_DELSCHEDULE_REQUEST,

	D3SC_BASE = 0x100,
	D3SC_AUTH_RESULT,
	D3SC_GETDEPOT_RESULT,
	D3SC_SEARCHITEMS_RESULT,
	D3SC_SEARCHITEMSLOCAL_RESULT,
	D3SC_GETMONEY_RESULT,
	D3SC_GETFINISHED_RESULT,
	D3SC_GETSELLING_RESULT,
	D3SC_GETBIDDING_RESULT,
	D3SC_GETHISTORY_RESULT,
	D3SC_ACTION_RESULT,
	D3SC_DOBUYOUT_RESULT,
	D3SC_DOBID_RESULT,
	D3SC_DOSELL_RESULT,
	D3SC_DORECEIVE_RESULT,
	D3SC_SWITCHACC_RESULT,
	D3SC_GETSCHEDULE_RESULT,
	D3SC_ADDSCHEDULE_RESULT,
	D3SC_EDITSCHEDULE_RESULT,
	D3SC_DELSCHEDULE_RESULT,
	D3SC_SCHEDULE_UPDATED,
};

#pragma pack (pop)
