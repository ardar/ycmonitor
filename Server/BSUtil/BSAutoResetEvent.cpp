
#include "stdafx.h"
#include "BSUtil.h"

BSAutoResetEvent::BSAutoResetEvent(HANDLE hEvent)
{
	m_hEvent = hEvent;
	SetEvent(m_hEvent);
}

BSAutoResetEvent::~BSAutoResetEvent()
{
	ResetEvent(m_hEvent);
}