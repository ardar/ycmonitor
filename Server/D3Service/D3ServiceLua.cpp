#include "stdafx.h"
#include "D3Service.h"
#include "LuaBind.h"

void D3Service::initLuaFuncs()
{
	m_pBSLua = new BSLua();
	m_pBSLua->Initialize("");
	//RegLuaMemFunc(m_pBSLua->GetLuaState(), "������Ʒ", *this, &D3Service::LuaSearchItems);
	//RegLuaMemFunc(m_pBSLua->GetLuaState(), "��ȡ�����Ʒ", *this, &D3Service::LuaReceiveAllItems);
	//RegLuaMemFunc(m_pBSLua->GetLuaState(), "���ٳ���", *this, &D3Service::LuaTraceBidItem);
}
