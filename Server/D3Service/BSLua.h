#pragma once
#include <afxtempl.h>
#include "IBSLua.h"

struct lua_State;
struct lua_Debug;


class _declspec(dllexport) BSLua : public IBSLua
{
public: 

	BSLua();
	virtual ~BSLua();

	// 实现 IBSVM 接口
	virtual lua_State* GetLuaState();
	virtual void Initialize(LPCTSTR szInitScript);
	virtual void CleanUp();
	virtual void Reset();
	virtual void DoFile(LPCTSTR szScriptFile);
	virtual void DoString(LPCTSTR szScript);
	virtual void SetError(DWORD dwErrCode, LPCTSTR szErrMsg);
	virtual void RegisterHook(BSLuaHook hook, PVOID pParam);
	virtual void UnregisterHook(BSLuaHook hook, PVOID pParam);
	virtual void SetGlobal(LPCTSTR szName, INT64 nValue);
	virtual void SetGlobal(LPCTSTR szName, double fValue);
	virtual void SetGlobal(LPCTSTR szName, LPCTSTR szValue);
	virtual BOOL GetGlobal(LPCTSTR szName, INT64& nValue);
	virtual BOOL GetGlobal(LPCTSTR szName, double& fValue);
	virtual BOOL GetGlobal(LPCTSTR szName, CString& szValue);

private:
	struct BSLuaHookItem
	{
		IBSLua::BSLuaHook hookFunction;
		PVOID pHookParam;
	};
	lua_State* m_L;
	CMutex m_mutexHookList;
	CList<BSLuaHookItem, const BSLuaHookItem> m_hookList;
	BSLuaHookItem m_hookItem1;
	BSLuaHookItem m_hookItem2;
	CString m_strInitScript;
	void StackDump();
	static void LuaHook(lua_State* L, lua_Debug* ar);
	static int LuaCallBack (void *wParam,void *lParam,int nLine);
};