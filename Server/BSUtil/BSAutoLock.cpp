
#include "stdafx.h"
#include "BSUtil.h"

BSAutoLock::BSAutoLock(CMutex* pMutex)
: m_lock(pMutex)
{
	if (!m_lock.IsLocked())
	{
		m_lock.Lock();
	}
	else
	{
		TRACE("Warning @ BSAutoLock::BSAutoLock() the mutex is already locked.\n");
	}
}

BSAutoLock::~BSAutoLock()
{
	m_lock.Unlock();
}