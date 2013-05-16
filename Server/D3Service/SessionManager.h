#pragma once


class _declspec(dllexport) SessionManager
{
public:
	SessionManager(void);
	virtual ~SessionManager(void);

	BOOL GetSession(DWORD dwSessionId, D3ServiceSession& session);
	void CreateSession(DWORD dwSessionId, const PcSessionInfo& pcSession);
	void DeleteSession(DWORD dwSessionId);
	BOOL SetAuththenticated(DWORD dwSessionId, LPCTSTR szUsername);
	BOOL IsAuthenticated(DWORD dwSessionId);
	
private:
	BSMap<DWORD, DWORD, D3ServiceSession, D3ServiceSession&> m_mapSession;
};
