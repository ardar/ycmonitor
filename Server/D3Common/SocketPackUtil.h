#pragma once

class SocketPackUtil
{
public:
	static void ReadItemProperty(const BYTE* buf, int& nIndex, D3ItemProperty& field);
	static BOOL WriteItemProperty(const D3ItemProperty& field, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadItemInfo(const BYTE* buf, int& nIndex, D3ItemInfo& item);
	static BOOL WriteItemInfo(const D3ItemInfo& info, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadDepotItem(const BYTE* buf, int& nIndex, D3DepotItem& item);
	static BOOL WriteDepotItem(const D3DepotItem& item, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadCompletedItem(const BYTE* buf, int& nIndex, D3CompletedItem& item);
	static BOOL WriteCompletedItem(const D3CompletedItem& item, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadSellingItem(const BYTE* buf, int& nIndex, D3SellingItem& item);
	static BOOL WriteSellingItem(const D3SellingItem& item, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadBiddingItem(const BYTE* buf, int& nIndex, D3BiddingItem& item);
	static BOOL WriteBiddingItem(D3BiddingItem& item, BYTE* buf, int nMaxLen, int& nIndex);
	static BOOL WriteSearchItem(const D3SearchItem& item, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadSearchItem(const BYTE* buf, int& nIndex, D3SearchItem& item);
	static void ReadSchedule(const BYTE* buf, int& nIndex, D3Schedule& item);
	static BOOL WriteSchedule(const D3Schedule& item, BYTE* buf, int nMaxLen, int& nIndex);
	static void ReadHistoryItem( const BYTE* buf, int& nIndex, D3HistoryItem& item );
	static BOOL WriteHistoryItem( const D3HistoryItem& item, BYTE* buf, int nMaxLen, int& nIndex );
};