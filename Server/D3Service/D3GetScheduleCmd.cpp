#include "StdAfx.h"
#include "D3GetScheduleCmd.h"

D3GetScheduleCmd::D3GetScheduleCmd(void)
{
	m_dwSessionId = 0;
	m_pPack = NULL;
}

D3GetScheduleCmd::~D3GetScheduleCmd(void)
{
	if (m_pPack)
	{
		delete m_pPack;
	}
}

void D3GetScheduleCmd::ReadPack(DWORD dwSessionId, BYTE* buf, int len)
{
	if (!m_pPack)
	{
		m_pPack = new D3GetSchedulePack();
	}
	m_dwSessionId = dwSessionId;
	m_pPack->FromBuffer(buf, len);
};

void D3GetScheduleCmd::Execute()
{
	D3GetScheduleResultPack* pRetCmd = new D3GetScheduleResultPack();
	pRetCmd->m_dwRequestId = m_pPack->m_dwRequestId;

	if(DataManager::Instance().GetSchedules(pRetCmd->m_itemList))
	{
		pRetCmd->m_errorCode = ERR_Success;
	}
	else
	{
		pRetCmd->m_errorCode = ERR_DBQueryFailed;
	}
	D3Service::Instance().SendToSession(m_dwSessionId, pRetCmd);

	delete pRetCmd;
}

void D3GetScheduleCmd::Cancel( DWORD dwErrorCode )
{
	D3GetScheduleResultPack retCmd;
	retCmd.m_dwRequestId = m_pPack->m_dwRequestId;
	retCmd.m_errorCode = (D3ErrorCode)dwErrorCode;
	D3Service::Instance().SendToSession(m_dwSessionId, &retCmd);
}