#pragma once

class D3TracePriceCmd : public IHandlerCmd
{
public:
	LONGLONG m_llMaxPrice;
	LONGLONG m_llAddPrice;
	CString m_szItemName;

	D3TracePriceCmd(void);
	virtual ~D3TracePriceCmd(void);
	virtual CString GetName() { return CString("¸úÛ™³öƒr"); };

	virtual IHandlerCmd* CreateInstance() 
	{
		return new D3TracePriceCmd();
	}
	virtual void ReadPack(DWORD dwSessionId, BYTE* buf, int len);

	virtual void Execute();

	virtual void Cancel( DWORD dwErrorCode );
};
