#include "stdafx.h"
#include "BSLua.h"

extern "C" {
#include "..\3rdParty\Lua\src\lua.h"
#include "..\3rdParty\Lua\src\lualib.h"
#include "..\3rdParty\Lua\src\lauxlib.h"
}

BSLua::BSLua()
: m_L(0)
{
	ZeroMemory(&m_hookItem1, sizeof(m_hookItem1));
	ZeroMemory(&m_hookItem2, sizeof(m_hookItem2));
}

BSLua::~BSLua()
{
	CleanUp();
}

lua_State* BSLua::GetLuaState()
{
	return m_L;
}

void BSLua::Initialize(LPCTSTR szInitScript)
{
	if (!m_L)
	{
		/* initialize LUA */
		m_L = lua_open();

		/* load LUA base libraries */
		luaL_openlibs(m_L);
	}

	lua_pushlightuserdata(m_L, this);
	lua_setglobal(m_L, "g_lua");

	//lua_sethook(m_L, LuaHook, LUA_MASKLINE|LUA_MASKCALL, 0);//
	lua_setcallback(m_L, BSLua::LuaCallBack, this, 0);

	BSHelper::GoAppRootPath();

	// 初始化脚本
	m_strInitScript = szInitScript;

	int scriptResult = luaL_dofile(m_L, szInitScript);
	if (scriptResult != 0)
	{
		const char* errMsg = lua_tostring(m_L, -1);
		lua_settop(m_L, 0);
		BSLogger::SysLog(LOG_ERROR, "执行全局初始化脚本失败: %s", errMsg);
		//throw BSScriptException(szErrorMessage);
	}
}

void BSLua::RegisterHook(BSLuaHook hook, PVOID pParam)
{
	//lua_sethook(m_L, LuaHook, LUA_MASKLINE|LUA_MASKCALL, 0);//
	BSAutoLock lock(&m_mutexHookList);
	BSLuaHookItem item;
	item.hookFunction = hook;
	item.pHookParam = pParam;
	BSLogger::SysLog(LOG_VERBOSE, "Add hookList Item %X param:%X\n", hook, pParam);
	//m_hookList.AddTail(item);
	if(m_hookItem1.hookFunction == NULL)
	{
		m_hookItem1 = item;
	}
	else
	{
		m_hookItem2 = item;
	}
}

void BSLua::UnregisterHook(BSLuaHook hook, PVOID pParam)
{
	BSAutoLock lock(&m_mutexHookList);
	BSLuaHookItem item;
	item.hookFunction = hook;
	item.pHookParam = pParam;
	if(m_hookItem1.hookFunction == hook && m_hookItem1.pHookParam==pParam)
	{
		m_hookItem1.hookFunction = NULL;
		m_hookItem1.pHookParam = NULL;
	}
	else if(m_hookItem2.hookFunction == hook && m_hookItem2.pHookParam==pParam)
	{
		m_hookItem2.hookFunction = NULL;
		m_hookItem2.pHookParam = NULL;
	}
	/*POSITION pos = m_hookList.Find(item);
	if(pos)
	{
		BSLogger::SysLog(LOG_VERBOSE, "Remove hookList Item %d\n", pos);
		m_hookList.RemoveAt(pos);
	}
	else
	{
		BSLogger::SysLog(LOG_VERBOSE, "Done found hookList item %X param:%X\n", hook, pParam);
	}*/
}

int BSLua::LuaCallBack (void *wParam,void *lParam,int nLine)
{
	BSLua* pLua = (BSLua*)wParam;
	if (pLua)
	{
		if(pLua->m_hookItem1.hookFunction)
		{
			pLua->m_hookItem1.hookFunction(pLua->m_hookItem1.pHookParam);
		}
		if(pLua->m_hookItem2.hookFunction)
		{
			pLua->m_hookItem2.hookFunction(pLua->m_hookItem2.pHookParam);
		}
		/*POSITION pos = pLua->m_hookList.GetHeadPosition();
		while(pos)
		{
		BSLuaHookItem item = pLua->m_hookList.GetNext(pos);
		if(item.hookFunction)
		{
		item.hookFunction(item.pHookParam);
		}
		}*/
	}
	return 0;
 }

void BSLua::LuaHook(lua_State* L, lua_Debug* ar)
{
	lua_getglobal(L, "g_lua");
	BSLua* pLua = (BSLua*)lua_touserdata(L, -1);
	if (pLua)
	{
		if(pLua->m_hookItem1.hookFunction)
		{
			pLua->m_hookItem1.hookFunction(pLua->m_hookItem1.pHookParam);
		}
		if(pLua->m_hookItem2.hookFunction)
		{
			pLua->m_hookItem2.hookFunction(pLua->m_hookItem2.pHookParam);
		}
		/*POSITION pos = pLua->m_hookList.GetHeadPosition();
		while(pos)
		{
			BSLuaHookItem item = pLua->m_hookList.GetNext(pos);
			if(item.hookFunction)
			{
				item.hookFunction(item.pHookParam);
			}
		}*/
	}
}

void BSLua::CleanUp()
{
	if(m_L)
	{
		/* cleanup LUA */
		lua_close(m_L);
		m_L = 0;
	}
}

void BSLua::DoFile(LPCTSTR szScriptFile)
{
	TRACE("Dofile:%s\n", szScriptFile);
	BSHelper::GoAppRootPath();
	DWORD dwOriStack = *(DWORD*)(((BYTE*)m_L)+0x34);
	int nResult = luaL_dofile(m_L, szScriptFile);
	DWORD dwDebugStack = *(DWORD*)(((BYTE*)m_L)+0x34);
	//ASSERT(dwDebugStack==0);
	ASSERT(dwOriStack==dwDebugStack);
	if (nResult!=0)
	{
		const char* errMsg = lua_tostring(m_L, -1);
		lua_settop(m_L, 0);
		CString szErrMsg;
		//ZeroMemory((BYTE*)szErrMsg, 0x200);
		if (errMsg)
		{
			szErrMsg = errMsg; //strncpy_s(szErrMsg, sizeof(szErrMsg), errMsg, 16);
			if (szErrMsg.Find("<ResetException>")>=0)
			{
				throw ResetException(szErrMsg.GetBuffer());
			}
			TRACE("Luareturn err:%s\n",errMsg);
			szErrMsg.Format("执行脚本 %s 失败: %s", szScriptFile, errMsg);
		}
		else
		{
			szErrMsg.Format("执行脚本 %s 失败: 未知错误", szScriptFile);
		}
		szErrMsg.Replace("%","％");
		BSLogger::SysLog(LOG_ERROR, szErrMsg);
		throw ScriptException(szErrMsg);
	}
}

void BSLua::DoString(LPCTSTR szScript)
{
	int nResult = luaL_dostring(m_L, szScript);
	if (nResult != 0)
	{
		const char* errMsg = lua_tostring(m_L, -1);
		lua_settop(m_L, 0);
		CString szErrMsg;
		//ZeroMemory((BYTE*)szErrMsg, 0x200);
		if (errMsg)
		{
			szErrMsg = errMsg; //strncpy_s(szErrMsg, sizeof(szErrMsg), errMsg, 16);
			if (szErrMsg.Find("<ResetException>")>=0)
			{
				throw ResetException(szErrMsg.GetBuffer());
			}
			TRACE("Luareturn err:%s\n",errMsg);
			szErrMsg.Format("执行脚本 %s 失败: %s", szScript, errMsg);
		}
		else
		{
			szErrMsg.Format("执行脚本 %s 失败: 未知错误", szScript);
		}
		BSLogger::SysLog(LOG_ERROR, "%s", szErrMsg);
		throw ScriptException(szErrMsg);
	}
}

void BSLua::Reset()
{
	TRACE("ResetLua\n");
	lua_settop(m_L, 0);
	CleanUp();
	Initialize(m_strInitScript.GetBuffer());
}

void BSLua::SetError(DWORD dwErrCode, LPCTSTR szErrMsg)
{
	StackDump();
	luaL_error(m_L, szErrMsg);
}

void BSLua::SetGlobal(LPCTSTR szName, INT64 nValue)
{
	lua_pushnumber(m_L, (lua_Number)nValue);
	lua_setglobal(m_L, szName);
}

void BSLua::SetGlobal(LPCTSTR szName, double fValue)
{
	lua_pushnumber(m_L, fValue);
	lua_setglobal(m_L, szName);
}

void BSLua::SetGlobal(LPCTSTR szName, LPCTSTR szValue)
{
	lua_pushstring(m_L, szValue);
	lua_setglobal(m_L, szName);
}

BOOL BSLua::GetGlobal(LPCTSTR szName, INT64& nValue)
{
	lua_getglobal(m_L, szName);
	if(lua_gettop(m_L)==1)
	{
		nValue = (INT64)lua_tonumber(m_L, -1);
		return TRUE;
	}
	return FALSE;
}

BOOL BSLua::GetGlobal(LPCTSTR szName, double& fValue)
{
	lua_getglobal(m_L, szName);
	if(lua_gettop(m_L)==1)
	{
		fValue = lua_tonumber(m_L, -1);
		return TRUE;
	}
	return FALSE;
}

BOOL BSLua::GetGlobal(LPCTSTR szName, CString& szValue)
{
	if(lua_gettop(m_L)==1)
	{
		lua_getglobal(m_L, szName);
		szValue = lua_tostring(m_L, -1);
		return TRUE;
	}
	return FALSE;
}

void BSLua::StackDump () 
{
	int i;
	int top = lua_gettop(m_L);
	for (i = 1; i <= top; i++) {
		int t = lua_type(m_L, i);
		switch (t) {

		case LUA_TSTRING:
			printf("'%s'", lua_tostring(m_L, i));
			TRACE("'%s'", lua_tostring(m_L, i));
			break;

		case LUA_TBOOLEAN:
			printf(lua_toboolean(m_L, i) ? "true":"false");
			TRACE(lua_toboolean(m_L, i) ? "true":"false");
			break;

		case LUA_TNUMBER:
			printf("%g", lua_tonumber(m_L, i));
			TRACE("%g", lua_tonumber(m_L, i));
			break;

		default:
			printf("%s", lua_typename(m_L, t));
			TRACE("%s", lua_typename(m_L, t));
			break;

		}
		printf(" ");
		TRACE(" ");
	}
	printf("\n");
	TRACE("\n");
}