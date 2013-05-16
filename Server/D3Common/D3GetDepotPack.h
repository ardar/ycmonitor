#pragma once
#include "..\D3Common\D3EmptyPack.h"

class D3GetDepotPack : public D3EmptyPack
{
public:
	D3GetDepotPack()
		: D3EmptyPack(D3CS_GETDEPOT_REQUEST)
	{
	};
};