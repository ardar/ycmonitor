#pragma once

class D3AuthCmd : public IHandlerCmd
{
public:
	D3AuthPack* m_pPack;

	D3AuthCmd()
	{
		m_dwSessionId = 0;
		m_pPack = NULL;
	}
	virtual ~D3AuthCmd()
	{
		if (m_pPack)
		{
			delete m_pPack;
		}
	}
	virtual CString GetName() { return CString("™àÏÞÕJ×C"); };

	virtual IHandlerCmd* CreateInstance() 
	{
		return new D3AuthCmd();
	}

	virtual void ReadPack(DWORD dwSessionId, BYTE* buf, int len)
	{
		if (!m_pPack)
		{
			m_pPack = new D3AuthPack();
		}
		m_dwSessionId = dwSessionId;
		m_pPack->FromBuffer(buf, len);
	};

	virtual void Cancel(DWORD dwErrorCode)
	{
		D3AuthResultPack retCmd;
		retCmd.m_dwRequestId = m_pPack->m_dwRequestId;
		retCmd.m_dwResult = dwErrorCode;
		D3Service::Instance().SendToSession(m_dwSessionId, &retCmd);
	}

	virtual void Execute()
	{
		D3AuthResultPack retCmd;
		retCmd.m_dwRequestId = m_pPack->m_dwRequestId;
		ServiceUser user;
		if(DataManager::Instance().GetServiceUser(m_pPack->m_szUser, user))
		{
			if (!user.bIsEnabled)
			{
				retCmd.m_dwResult = ERR_UserDisabled;
			}
			else if(user.szPass.Compare(m_pPack->m_szPass)!=0)
			{
				retCmd.m_dwResult = ERR_UserPasswordInvalid;
			}
			else
			{
				retCmd.m_dwResult = ERR_Success;
				D3Service::Instance().SetAuththenticated(m_dwSessionId, user.szUser);
			}
		}
		else
		{
			retCmd.m_dwResult = ERR_DBQueryFailed;
		}
		D3Service::Instance().SendToSession(m_dwSessionId, &retCmd);
	}
};