#include "stdafx.h"
#include "BSException.h"

ResetException::ResetException(LPCTSTR szMessage)
: BSException(szMessage)
{
}

ResetException::~ResetException(void)
{
}
