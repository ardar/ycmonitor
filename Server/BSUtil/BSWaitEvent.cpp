#include "StdAfx.h"
#include "BSUtil.h"

BSWaitEvent::BSWaitEvent(void)
: m_dwWaitEventId(0), m_dwWaitResult(0), m_dwResultWParam(0), m_dwResultLParam(0)
{
	m_hWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

BSWaitEvent::~BSWaitEvent(void)
{
	CloseHandle(m_hWaitEvent);
}

void BSWaitEvent::StartEvent( DWORD dwEventId )
{
	BSAutoLock lock(&m_mutexWaitEvent);
	ResetEvent(m_hWaitEvent);
	m_dwWaitEventId = dwEventId;
	m_dwWaitResult = 0;
	m_dwResultWParam = 0;
	m_dwResultLParam = 0;
}

void BSWaitEvent::CancelEvent()
{
	BSAutoLock lock(&m_mutexWaitEvent);
	m_dwWaitResult = RESULT_CANCELED;
	m_dwResultWParam = 0;
	m_dwResultLParam = 0;
	SetEvent(m_hWaitEvent);
}

BOOL BSWaitEvent::WaitEvent( DWORD dwEventId, int nTimeout )
{
	if ( m_dwWaitEventId == dwEventId)
	{
		return WaitForSingleObject(m_hWaitEvent, nTimeout) == WAIT_OBJECT_0;
	}
	else
	{
		return FALSE;
	}
}

void BSWaitEvent::EndEvent( DWORD dwEventId )
{
	BSAutoLock lock(&m_mutexWaitEvent);
	ResetEvent(m_hWaitEvent);
	m_dwWaitEventId = 0;
	m_dwWaitResult = 0;
	m_dwResultWParam = 0;
	m_dwResultLParam = 0;
}

void BSWaitEvent::OnEvent( DWORD dwEventId, DWORD dwResult /*= 0*/, DWORD wParam /*= 0*/, DWORD lParam /*= 0*/ )
{
	BSAutoLock lock(&m_mutexWaitEvent);
	if( m_dwWaitEventId == dwEventId)
	{
		m_dwWaitResult = dwResult;
		m_dwResultWParam = wParam;
		m_dwResultLParam = lParam;
		SetEvent(m_hWaitEvent);
	}
}

DWORD BSWaitEvent::GetResult(DWORD* pWParam/*=NULL*/, DWORD* pLParam/*=NULL*/)
{
	BSAutoLock lock(&m_mutexWaitEvent);
	if (pWParam)
	{
		*pWParam = m_dwResultWParam;
	}
	if (pLParam)
	{
		*pWParam = m_dwResultLParam;
	}
	return m_dwWaitResult;
}

int BSWaitEvent::WaitResult( DWORD dwEventId, int nTimeout, DWORD* pWParam/*=NULL*/, DWORD* pLParam/*=NULL*/ )
{
	if ( m_dwWaitEventId == dwEventId)
	{
		if (WaitEvent(dwEventId, nTimeout))
		{
			return GetResult(pWParam, pLParam);
		}
		else
		{
			return RESULT_TIMEOUT;
		}
	}
	else
	{
		return RESULT_CANCELED;
	}
}