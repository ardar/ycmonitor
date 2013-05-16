#include "stdafx.h"
#include "BSException.h"

ScriptException::ScriptException(LPCTSTR szMessage)
: BSException(szMessage)
{
}

ScriptException::~ScriptException(void)
{
}
