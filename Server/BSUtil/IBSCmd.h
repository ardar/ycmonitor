#pragma once
// 
class _declspec(dllexport) IBSCmd
{
public:
	virtual ~IBSCmd(){/*TRACE("IBSCommand::Desctry\n");*/};
	virtual CString GetName() { return CString("IBSCmd"); };
	virtual void Execute() = 0;
};