#pragma once

#include "stdafx.h"
extern "C" {
#include "..\3rdParty\Lua\src\lua.h"
#include "..\3rdParty\Lua\src\lualib.h"
#include "..\3rdParty\Lua\src\lauxlib.h"
}


//////////////////////////////////////////////////////////////////////////
// CBSLua
class BSLua;

/*

调用真正的C函数，现已int func()作为特例。



参数 func 函数指针，指向参数为返回值为int类型的函数

L lua变量

index lua栈中索引



对于其他的类型，可用模板实现

如对于一个参数的函数，实现如下

template <typname RT, typename P1>

int callFunc(RT (*func)(P1), lua_State *L, int index)

{

//Get 通过index索引得到在lua栈中的值并转换成P1类型

//Push 把函数的返回值压入堆栈

RT ret = func(Get(Type<P1>(), L, index + 0));

Push(L, ret);

return 1;

}

*/
template <typename _Tp>
struct param_traits
{
	static _Tp get_param( lua_State *L, int index )
	{
		return static_cast<_Tp>( lua_tonumber( L, index ) );
	}
	static void push_param( lua_State *L, lua_Number val)
	{
		return lua_pushnumber( L, val );
	}
}; 

template <>
struct param_traits<CString>
{
	static const char *get_param( lua_State *L, int index )
	{
		return lua_tostring(L, index);
	}
	static void push_param( lua_State *L, CString sz)
	{
		return lua_pushstring( L, sz.GetBuffer() );
	}

};

template <>
struct param_traits<const char*>
{
	static const char *get_param( lua_State *L, int index )
	{
		return lua_tostring( L, index );
	}
	static void push_param( lua_State *L, const char * sz)
	{
		return lua_pushstring( L, sz );
	}

};
template <>
struct param_traits<double>
{
	static double get_param( lua_State *L, int index )
	{
		return lua_tonumber( L, index );
	}
	static void push_param( lua_State *L, lua_Number sz)
	{
		return lua_pushnumber( L, sz );
	}

};

template <>
struct param_traits<float>
{
	static float get_param( lua_State *L, int index )
	{
		return lua_tonumber( L, index );
	}
	static void push_param( lua_State *L, lua_Number sz)
	{
		return lua_pushnumber( L, sz );
	}

};
template <>
struct param_traits<UINT64>
{
	static UINT64 get_param( lua_State *L, int index )
	{
		return lua_tonumber( L, index );
	}
	static void push_param( lua_State *L, UINT64 sz)
	{
		return lua_pushnumber( L, sz );
	}
};
template <>
struct param_traits<INT64>
{
	static INT64 get_param( lua_State *L, int index )
	{
		return lua_tonumber( L, index );
	}
	static void push_param( lua_State *L, INT64 sz)
	{
		return lua_pushnumber( L, sz );
	}
};
// template <>
// struct param_traits<BOOL>
// {
// 	static BOOL get_param( lua_State *L, int index )
// 	{
// 		return lua_toboolean( L, index );
// 	}
// 	static void push_param( lua_State *L,BOOL bbool)
// 	{
// 		return lua_pushboolean( L, bbool );
// 	}
// 
// };
// Support char * parameter
//template <>
//struct param_traits<char*>
//{
//	static const char *get_param( lua_State *L, int index )
//	{
//		return lua_tostring( L, index );
//	}
//	static void push_param( lua_State *L, char * sz)
//	{
//		return lua_pushstring( L, sz );
//	}
//
//};

// support 1 or 0 return value
template <typename _Tp>
struct return_number_traits
{
	enum
	{
		count = 1
	};
};
template <>
struct return_number_traits<void>
{
	enum
	{
		count = 0
	};
};


template <typename RT>
void Lua_Push(lua_State *L, RT rt)
{
	param_traits<RT>::push_param( L, rt);

}
template <typename RT>
int callFunc(RT (*func)(), lua_State *L, int index)
{
	RT ret = func();
	Lua_Push(L,ret);
	//return 1;
	return return_number_traits<RT>::count; 
}

template <typename RT, typename P1>
int callFunc(RT (*func)(P1), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}


template <typename RT, typename P1,typename P2>
int callFunc(RT (*func)(P1,P2), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename RT, typename P1,typename P2,typename P3>
int callFunc(RT (*func)(P1,P2,P3), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 )
		,param_traits<P3>::get_param( L, -3 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}

template <typename RT, typename P1,typename P2,typename P3,typename P4>
int callFunc(RT (*func)(P1,P2,P3,P4), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 )
		,param_traits<P3>::get_param( L, -3 ),param_traits<P4>::get_param( L, -4 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename RT, typename P1,typename P2,typename P3,typename P4,typename P5>
int callFunc(RT (*func)(P1,P2,P3,P4,P5), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 )
		,param_traits<P3>::get_param( L, -3 ),param_traits<P4>::get_param( L, -4 )
		,param_traits<P5>::get_param( L, -5 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
int callFunc(RT (*func)(P1,P2,P3,P4,P5,P6), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 )
		,param_traits<P3>::get_param( L, -3 ),param_traits<P4>::get_param( L, -4 )
		,param_traits<P5>::get_param( L, -5 ),param_traits<P5>::get_param( L, -6 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
int callFunc(RT (*func)(P1,P2,P3,P4,P5,P6,P7), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 )
		,param_traits<P3>::get_param( L, -3 ),param_traits<P4>::get_param( L, -4 )
		,param_traits<P5>::get_param( L, -5 ),param_traits<P5>::get_param( L, -6 )
		,param_traits<P5>::get_param( L, -7 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8>
int callFunc(RT (*func)(P1,P2,P3,P4,P5,P6,P7,P8), lua_State *L, int index)
{
	RT ret = func(param_traits<P1>::get_param( L, -1 ),param_traits<P2>::get_param( L, -2 )
		,param_traits<P3>::get_param( L, -3 ),param_traits<P4>::get_param( L, -4 )
		,param_traits<P5>::get_param( L, -5 ),param_traits<P5>::get_param( L, -6 )
		,param_traits<P5>::get_param( L, -7 ),param_traits<P5>::get_param( L, -8 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
//函数指针相关数据会存到UpValue的第一个值中，此处从upvalue中取出

//unsigned char* getFirstUpValue(lua_State *L)
//
//{
//
//	unsigned char* buffer = (unsigned char*)lua_touserdata(L, lua_upvalueindex(1));
//
//	return buffer;
//
//}



/*

实现callFunc的lua调用形式封装

*/

template <typename Func>

int directCallFunc(lua_State *L)

{

	//得到函数指针

	unsigned char* buffer = getFirstUpValue(L);

	//转换成相应的函数调用

	return callFunc(*(Func*)(buffer), L, 1);

}



/*

将directCallFunc注册进lua

*/

template <typename Func>

void lua_pushdirectclosure(lua_State *L, Func func, unsigned int nupvalues)

{

	//创建userdata并把func指针的值拷贝进去

	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));

	memcpy(buffer, &func, sizeof(func));



	lua_pushcclosure(L, directCallFunc<Func>, nupvalues + 1);

}







/*

实现对class里面memeber function的调用

参数cla 要调用的类的实例

Cla::*func 类的函数指针

*/

template <typename Cla,typename RT>
int callMemFunc(Cla &cla, RT (Cla::*func)(), lua_State *L, int index)
{
	RT ret = (cla.*func)();
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1), lua_State *L, int index)
{
	//RT ret = (cla.*func)(param_traits<P1>::get_param( L, -1 ));
	RT ret = (cla.*func)(param_traits<P1>::get_param( L, 1 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2), lua_State *L, int index)
{
	RT ret = (cla.*func)(param_traits<P1>::get_param( L, 1 ),param_traits<P2>::get_param( L, 2 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3), lua_State *L, int index)
{
	RT ret = (cla.*func)(param_traits<P1>::get_param( L, 1 ),param_traits<P2>::get_param( L, 2 )
		,param_traits<P3>::get_param( L, 3 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4), lua_State *L, int index)
{
	RT ret = (cla.*func)(param_traits<P1>::get_param( L, 1 ),param_traits<P2>::get_param( L, 2 )
		,param_traits<P3>::get_param( L, 3 ),param_traits<P4>::get_param( L, 4 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5), lua_State *L, int index)
{
	RT ret = (cla.*func)(param_traits<P1>::get_param( L, 1 ),param_traits<P2>::get_param( L, 2 )
		,param_traits<P3>::get_param( L, 3 ),param_traits<P4>::get_param( L, 4 )
		,param_traits<P5>::get_param( L, 5 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 )
		,param_traits<P2>::get_param( L, 2 )
		,param_traits<P2>::get_param( L, 3 )
		,param_traits<P3>::get_param( L, 4 )
		,param_traits<P4>::get_param( L, 5 )
		,param_traits<P5>::get_param( L, 6 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 )
		,param_traits<P2>::get_param( L, 2 )
		,param_traits<P3>::get_param( L, 3 )
		,param_traits<P4>::get_param( L, 4 )
		,param_traits<P5>::get_param( L, 5 )
		,param_traits<P6>::get_param( L, 6 )
		,param_traits<P7>::get_param( L, 7 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10, typename P11>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ),
		param_traits<P11>::get_param( L, 11 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10, typename P11, typename P12>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ),
		param_traits<P11>::get_param( L, 11 ),
		param_traits<P12>::get_param( L, 12 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10, typename P11, typename P12, typename P13>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ),
		param_traits<P11>::get_param( L, 11 ),
		param_traits<P12>::get_param( L, 12 ),
		param_traits<P13>::get_param( L, 13 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10, typename P11, typename P12, typename P13, typename P14>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ),
		param_traits<P11>::get_param( L, 11 ),
		param_traits<P12>::get_param( L, 12 ),
		param_traits<P13>::get_param( L, 13 ),
		param_traits<P14>::get_param( L, 14 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ),
		param_traits<P11>::get_param( L, 11 ),
		param_traits<P12>::get_param( L, 12 ),
		param_traits<P13>::get_param( L, 13 ),
		param_traits<P14>::get_param( L, 14 ),
		param_traits<P15>::get_param( L, 15 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}
template <typename Cla,typename RT, typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,
typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16>
int callMemFunc(Cla &cla, RT (Cla::*func)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16), lua_State *L, int index)
{
	RT ret = (cla.*func)(
		param_traits<P1>::get_param( L, 1 ),
		param_traits<P2>::get_param( L, 2 ),
		param_traits<P3>::get_param( L, 3 ),
		param_traits<P4>::get_param( L, 4 ),
		param_traits<P5>::get_param( L, 5 ),
		param_traits<P6>::get_param( L, 6 ),
		param_traits<P7>::get_param( L, 7 ),
		param_traits<P8>::get_param( L, 8 ),
		param_traits<P9>::get_param( L, 9 ),
		param_traits<P10>::get_param( L, 10 ),
		param_traits<P11>::get_param( L, 11 ),
		param_traits<P12>::get_param( L, 12 ),
		param_traits<P13>::get_param( L, 13 ),
		param_traits<P14>::get_param( L, 14 ),
		param_traits<P15>::get_param( L, 15 ),
		param_traits<P16>::get_param( L, 16 ));
	Lua_Push(L, ret);
	return return_number_traits<RT>::count; 
}

/*
将directCallMemFunc注册进lua
*/
template <typename Cla, typename Func>
void lua_pushdirectmemclosure(BSLua* pBSLua, lua_State *L, Cla &cla, Func func, unsigned int nupvalues)
{
	//创建userdata并把cla和func指针的值拷贝进去
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(PVOID) + sizeof(PVOID) + sizeof(func));

	PVOID pClass = &cla;

	memcpy(buffer, &pClass, sizeof(pClass));// Cla*
	memcpy(buffer+  sizeof(PVOID) , &pBSLua, sizeof(PVOID));// BSLua*

	memcpy(buffer +  sizeof(PVOID)+ sizeof(PVOID), &func, sizeof(func));// Func*

	lua_pushcclosure(L, LuaCallMemFunc<Cla, Func>, nupvalues + 1);
	/*
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(Cla) + sizeof(func));
	memcpy(buffer, &cla, sizeof(Cla));
	memcpy(buffer + sizeof(Cla), &func, sizeof(func));
	lua_pushcclosure(L, LuaCallMemFunc<Cla, Func>, nupvalues + 1);*/
}

#define lua_directregistry_function(L, func) lua_pushstring(L, #func);lua_pushdirectclosure(L, func, 0);lua_settable(L, LUA_GLOBALSINDEX);
//#define lua_reg_memfunction(pBSLua, L, name, cla, func) lua_pushstring(L, name);lua_pushdirectmemclosure(pBSLua, L, cla, func, 0);lua_settable(L, LUA_GLOBALSINDEX);


// Lua注册函数
template <typename Cla, typename Func>
void RegLuaMemFunc(lua_State* L, LPCTSTR szName, Cla &cla, Func func)
{
	TRACE("Register Lua func: %s  func:%X\n", szName, &func);

	lua_pushstring(L, szName);

	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(PVOID) + sizeof(func));

	PVOID pClass = &cla;

	memcpy(buffer  , &pClass, sizeof(pClass));// Cla*

	memcpy(buffer + sizeof(PVOID), &func, sizeof(func));// Func*

	int nupvalues = 0;
	lua_pushcclosure(L, LuaCallMemFunc<Cla, Func>, nupvalues + 1);

	lua_settable(L, LUA_GLOBALSINDEX);
}

template <typename Cla, typename Func>
static int LuaCallMemFunc(lua_State *L)
{
	//得到函数指针

	unsigned char* buffer = (unsigned char*)lua_touserdata(L, lua_upvalueindex(1));
	// 
	// 	//转换成相应的函数调用
	PVOID pClass = *(PVOID*)(buffer);
	Func* pFunc = (Func*)(buffer + sizeof(PVOID));

	int nResult = callMemFunc(*((Cla*)pClass), *pFunc, L, 1);

	return nResult;
}