#pragma once

class D3BuyoutCmd : public IHandlerCmd
{
public:
	D3DoBidBuyOutPack* m_pPack;

	D3BuyoutCmd()
	{
		m_dwSessionId = 0;
		m_pPack = NULL;
	}
	virtual ~D3BuyoutCmd()
	{
		if (m_pPack)
		{
			delete m_pPack;
		}
	}

	virtual CString GetName() { return "D3BuyoutCmd";};

	virtual IHandlerCmd* CreateInstance() 
	{
		return new D3BuyoutCmd();
	}

	virtual void ReadPack(DWORD dwSessionId, BYTE* buf, int len)
	{
		if (!m_pPack)
		{
			m_pPack = new D3DoBidBuyOutPack();
		}
		m_dwSessionId = dwSessionId;
		m_pPack->FromBuffer(buf, len);
	};

	virtual void Execute()
	{
		D3DoBidBuyOutPack* pRetCmd = new D3DoBidBuyOutPack();
		pRetCmd->m_dwRequestId = m_pPack->m_dwRequestId;

		

		D3Service::Instance().SendToSession(m_dwSessionId, pRetCmd);
	}
};