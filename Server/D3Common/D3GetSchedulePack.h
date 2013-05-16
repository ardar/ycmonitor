#pragma once

#include "..\D3Common\D3EmptyPack.h"

class D3GetSchedulePack : public D3EmptyPack
{
public:
	D3GetSchedulePack()
		: D3EmptyPack(D3CS_GETSCHEDULES_REQUEST)
	{
	};
};