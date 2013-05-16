#pragma once

class _declspec(dllexport) UserManager
{
public:
	UserManager(void);
	virtual ~UserManager(void);

	BOOL GetUser(LPCTSTR szUserName, ServiceUser& user);
	BOOL SaveUser(ServiceUser& user);
};
