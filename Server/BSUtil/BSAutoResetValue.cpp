
#include "stdafx.h"
#include "BSUtil.h"

BSAutoResetValue::BSAutoResetValue(VOID* pValue, DWORD dwResetValue)
: m_pValue(pValue), m_dwResetValue(dwResetValue)
{
}

BSAutoResetValue::~BSAutoResetValue()
{
	if (m_pValue && AfxIsValidAddress(m_pValue, 4, TRUE))
	{
		*(DWORD*)(m_pValue) = m_dwResetValue;
	}
}