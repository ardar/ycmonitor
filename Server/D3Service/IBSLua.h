#pragma once

struct lua_State;

class _declspec(dllexport) IBSLua
{
public: 
	typedef void (*BSLuaHook) (PVOID pParam);

	virtual ~IBSLua(){TRACE("IBSLua::Desctry\n");};

	virtual lua_State* GetLuaState() = 0;
	virtual void Initialize(LPCTSTR szInitScript) = 0;
	virtual void CleanUp() = 0;
	virtual void Reset() = 0;
	virtual void DoFile(LPCTSTR szScriptFile) = 0;
	virtual void DoString(LPCTSTR szScript) = 0;
	virtual void SetError(DWORD dwErrCode, LPCTSTR szErrMsg) = 0;
	virtual void RegisterHook(BSLuaHook hook, PVOID pParam) = 0;
	virtual void SetGlobal(LPCTSTR szKey, INT64 nValue) = 0;
	virtual void SetGlobal(LPCTSTR szKey, double fValue) = 0;
	virtual void SetGlobal(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual BOOL GetGlobal(LPCTSTR szName, INT64& nValue) = 0;
	virtual BOOL GetGlobal(LPCTSTR szName, double& fValue) = 0;
	virtual BOOL GetGlobal(LPCTSTR szName, CString& szValue) = 0;
};