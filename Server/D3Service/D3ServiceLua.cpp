#include "stdafx.h"
#include "D3Service.h"
#include "LuaBind.h"

void D3Service::initLuaFuncs()
{
	m_pBSLua = new BSLua();
	m_pBSLua->Initialize("");
	//RegLuaMemFunc(m_pBSLua->GetLuaState(), "搜索物品", *this, &D3Service::LuaSearchItems);
	//RegLuaMemFunc(m_pBSLua->GetLuaState(), "收取完成物品", *this, &D3Service::LuaReceiveAllItems);
	//RegLuaMemFunc(m_pBSLua->GetLuaState(), "跟踪出价", *this, &D3Service::LuaTraceBidItem);
}
