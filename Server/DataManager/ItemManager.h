#pragma once

class _declspec(dllexport) ItemManager
{
public:
	ItemManager(void);
	virtual ~ItemManager(void);

	BOOL GetItem(DWORD dwItemid, D3Item& item);
	BOOL QueryItems(const D3SearchCondition& condition, D3SearchResult& result);
	BOOL SaveItem(const D3Item& item);
};
