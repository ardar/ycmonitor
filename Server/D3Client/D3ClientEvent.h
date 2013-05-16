enum D3ClientEvent
{
	D3Event_None,
	D3Event_Connected,
	D3Event_Disconnected,

	D3Event_Auth,//WPARAM: DWORD dwResult,
	D3Event_GetMoney,//WPARAM: DWORD dwResult, LPARAM: LONGLONG* pllMoney
	D3Event_GetDepotItems,//WPARAM:DWORD dwResult, LPARAM: vector<D3DepotItem*>* itemList
	D3Event_GetFinishedItems,//WPARAM:DWORD dwResult, LPARAM: vector<D3CompletedItem*>* itemList
	D3Event_GetSellingItems,//WPARAM:DWORD dwResult, LPARAM: vector<D3SellingItem*>* itemList
	D3Event_GetBiddingItems,//WPARAM:DWORD dwResult, LPARAM: vector<D3BiddingItem*>* itemList
	D3Event_GetHistoryItems,//WPARAM:DWORD dwResult, LPARAM: vector<D3HistoryItem*>* itemList
	D3Event_SearchItems,//WPARAM: DWORD dwReuslt, LPARAM:vector<D3SearchItem*>* itemList
	D3Event_SearchItemsInLocal,//WPARAM: DWORD dwReuslt, LPARAM:vector<D3SearchItem*>* itemList
	D3Event_DoBid,//WPARAM: DWORD dwReuslt, LPARAM: vector<D3BiddingItem*>* itemList
	D3Event_DoBuyOut,//WPARAM: DWORD dwReuslt
	D3Event_DoSell,//WPARAM: DWORD dwReuslt
	D3Event_DoReceive,//WPARAM: DWORD dwReuslt
	D3Event_SaveItem,//WPARAM: DWORD dwReuslt, LPARAM: DWORD dwSavedItemId
	D3Event_GetSchedules,//WPARAM: DWORD dwResult, LPARAM, vector<D3Schedule*>* scheduleList
	D3Event_AddSchedule,//WPARAM: DWORD dwResult; LPARAM:DWORD inserted scheduleId
	D3Event_EditSchedule,//WPARAM: DWORD dwResult;
	D3Event_DelSchedule,//WPARAM: DWORD dwResult;
	D3Event_ScheduleUpdated,//WPARAM: DWORD dwResult; LPARAM: D3Schedule* pSchedule
	D3Event_SwitchAccount,//WPARAM: DWORD dwResult; LPARAM: char* szNewUsername
};