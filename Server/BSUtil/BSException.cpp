#include "stdafx.h"
#include "BSException.h"

BSException::BSException(LPCTSTR szMessage)
:m_dwErrorCode(0)
{
	 m_szMsg = szMessage;
	//TRACE("BSException Constructor %d: %s\n", this,  szMessage);
}

BSException::~BSException(void)
{
	//TRACE("BSException Destroyed %d: %s\n", this, m_szMsg.c_str());
}

LPCTSTR BSException::GetErrorMessage()
{
	return m_szMsg.GetBuffer();
}