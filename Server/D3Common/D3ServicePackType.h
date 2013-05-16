#pragma once
#pragma pack (push, 1)

// 客户端向服务器发送的报文type
enum D3PACKTYPE 
{
	LOW_HEARTBEAT = 0,
	D3CS_AUTH_REQUEST,
	D3CS_QUERYITEM_REQUEST,
	D3CS_BUYITEM_REQUEST,

	D3SC_BASE = 0x100,
	D3SC_AUTH_RESULT,
	D3SC_QUERY_RESULT,
};

#pragma pack (pop)

