#include "StdAfx.h"
#include "PipeCommunicator.h"

PipeCommunicator::PipeCommunicator(LPCTSTR szPipeName)
:m_hPipe(NULL)
{
}

PipeCommunicator::~PipeCommunicator(void)
{
}

BOOL PipeCommunicator::Connect()
{
	CString szPipe;
	szPipe.Format("\\\\.\\pipe\\%s\\", m_szPipeName);
	m_hPipe = CreateNamedPipe(
		szPipe,
		PIPE_ACCESS_DUPLEX|FILE_FLAG_WRITE_THROUGH,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE| PIPE_WAIT,
		1,2048,2048,1000,NULL);

	if (INVALID_HANDLE_VALUE == m_hPipe) 
	{
		return FALSE;
	}
	return TRUE;
}