#pragma once

typedef void (*Func_OnClientCallback) (DWORD dwMsgId, DWORD dwRequestId, WPARAM wParam, LPARAM lParam);

class _declspec(dllexport) ID3Client
{
public:
	//创建一个ID3Client实例
	static ID3Client* CreateInstance();

	// Client Control
	virtual BOOL InitClient(LPCTSTR szClientId, LPCTSTR szConfigFilename, Func_OnClientCallback pNotifier) = 0;
	virtual BOOL StartClient() = 0;
	virtual void StopClient() = 0;
	virtual BOOL IsConnected() = 0;
	// Facade methods
	//认证客户端账号
	virtual DWORD Authenticate(LPCTSTR szName, LPCTSTR szPass) = 0;
	// 查询金钱
	virtual DWORD GetMoney() = 0;
	// 查询背包
	virtual DWORD GetDepotItems() = 0;
	// 查询拍卖完成物品
	virtual DWORD GetFinishedItems() = 0;
	// 查询已出价物品
	virtual DWORD GetBiddingItems() = 0;
	// 查询已寄售物品
	virtual DWORD GetSellingItems() = 0;
	// 查询历史记录
	virtual DWORD GetHistoryItems() = 0;

	// 搜索物品
	virtual DWORD SearchItems(const D3SearchCondition& condition) = 0;
	// 查询物品数据（在数据库）
	virtual DWORD SearchItemsInLocal(const D3SearchCondition& condition) = 0;

	// 直购
	virtual DWORD DoBuyOut(const D3ItemInfo& itemInfo) = 0;
	// 竞价
	virtual DWORD DoBid(const D3ItemInfo& itemInfo, LONGLONG llPrice) = 0;
	// 拍卖我的物品
	virtual DWORD DoSell(const D3DepotItem& item, LONGLONG nStartPrice, LONGLONG nBuyoutPrice) = 0;
	// 收取买到物品或收到金钱至背包
	virtual DWORD DoReceive(const D3ItemInfo& itemInfo) = 0;

	// 保存物品数据
	virtual DWORD SaveItemData(const D3ItemInfo& record, LONGLONG nBidPrice, LONGLONG nBuyoutPrice) = 0;

	// 获取全部任务计划
	virtual DWORD GetSchedules() = 0;
	// 加入计划任务
	virtual DWORD AddSchedule(const D3Schedule& schedule) = 0;
	// 修改计划任务
	virtual DWORD EditSchedule(const D3Schedule& schedule) = 0;
	// 删除计划任务
	virtual DWORD DelSchedule(const D3Schedule& schedule) = 0;
	// 切换账号
	virtual DWORD SwitchAccount(const D3GameAccount& acc) = 0;

};
