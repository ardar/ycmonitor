#include "StdAfx.h"
#include "SessionManager.h"

SessionManager::SessionManager(void)
{
}

SessionManager::~SessionManager(void)
{
}

BOOL SessionManager::GetSession( DWORD dwSessionId, D3ServiceSession& session)
{
	return m_mapSession.GetAt(dwSessionId, session);
}

void SessionManager::CreateSession( DWORD dwSessionId, const PcSessionInfo& pcSession )
{
	D3ServiceSession session;
	strcpy_s(session.szClientIP, sizeof(session.szClientIP), pcSession.ClientIP);
	session.dwClientPort = pcSession.ClientPort;
	session.dwSessionid = pcSession.SessionId;
	session.dwBeginTime = time(0);
	session.bIsAuthenticated = FALSE;
	m_mapSession.SetAt(dwSessionId, session);
}

void SessionManager::DeleteSession( DWORD dwSessionId )
{
	m_mapSession.RemoveKey(dwSessionId);
}

BOOL SessionManager::SetAuththenticated( DWORD dwSessionId, LPCTSTR szUsername )
{
	D3ServiceSession session;
	if(m_mapSession.GetAt(dwSessionId, session))
	{
		session.bIsAuthenticated = TRUE;
		strcpy_s(session.szUsername, sizeof(session.szUsername), szUsername);
		return TRUE;
	}
	return FALSE;
}

BOOL SessionManager::IsAuthenticated( DWORD dwSessionId )
{
	D3ServiceSession session;
	if(m_mapSession.GetAt(dwSessionId, session))
	{
		return session.bIsAuthenticated;
	}
	return FALSE;
}
