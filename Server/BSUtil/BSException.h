#pragma once

class _declspec(dllexport) BSException
{
public:
	BSException(LPCTSTR szMessage);
	virtual ~BSException(void);
	virtual LPCTSTR GetErrorMessage();
private:
	DWORD m_dwErrorCode;
	CString m_szMsg;
};

class _declspec(dllexport) ScriptException : public BSException
{
public:
	ScriptException(LPCTSTR szMessage);
	virtual ~ScriptException(void);
};

class _declspec(dllexport) ResetException : public BSException
{
public:
	ResetException(LPCTSTR szMessage);
	virtual ~ResetException(void);
};

class _declspec(dllexport) SocketException : public BSException
{
public:
	SocketException(LPCTSTR szMessage);
	virtual ~SocketException(void);
};

