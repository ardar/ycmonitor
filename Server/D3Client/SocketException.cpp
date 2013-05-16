#include "stdafx.h"
#include "BSException.h"

SocketException::SocketException(LPCTSTR szMessage)
: BSException(szMessage)
{
}

SocketException::~SocketException(void)
{
}