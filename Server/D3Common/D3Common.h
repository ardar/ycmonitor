#pragma once
#pragma warning(disable:4482)
#include <vector>
//#define BSCATCH_ALL_EXCEPTION

#pragma pack(push, 1)

const int MAX_NAME_LENGTH = 64;

// 错误代码
enum D3ErrorCode
{
	ERR_Timeout=-1,
	ERR_Success,
	ERR_Unknown,//未知錯誤
	ERR_DBConnFailed,//數據庫鏈接失敗
	ERR_DBQueryFailed,//數據庫查詢失敗
	ERR_SocketConnFailed,//鏈接失敗

	ERR_UserInvalid,//用戶不存在
	ERR_UserPasswordInvalid,//密碼錯誤
	ERR_UserDisabled,//用戶被禁用

	ERR_CallApiFailed,//调用DllApi失败
	ERR_CallApiTimeout,//调用API超时
	ERR_CallGameError,//调用出现游戏内错误消息
	ERR_MoneyNotEnough,//金钱不足
	ERR_GameDisconnected,//遊戲未登陸在線
	ERR_AuctionClosed,//拍賣行已關閉

	ERR_ScriptError,//腳本執行錯誤
	ERR_TooManyQueuedCmd,//正在等待的队列过多, 请稍后重新提交.
};

// 物品属性字段
typedef struct _tagD3ItemProperty
{
	DWORD dwDataId;//字段数据库Id
	DWORD dwItemDataId;//物品数据库ID
	char szName[64];//字段名
	char szValue[64];//当前值
	int nMinValue;//最小值
	int nMaxValue;//最大值
	_tagD3ItemProperty()
	{
		ZeroMemory(this, sizeof(_tagD3ItemProperty));
	};
	~_tagD3ItemProperty()
	{
		//TRACE("D3ItemProperty destructor\n");
	}
	_tagD3ItemProperty(const _tagD3ItemProperty & src)
	{
		memcpy_s(this, sizeof(_tagD3ItemProperty), &src, sizeof(src));
	};
	void operator=(const _tagD3ItemProperty& src)
	{
		memcpy_s(this, sizeof(_tagD3ItemProperty), &src, sizeof(src));
	};
} D3ItemProperty, *PD3ItemProperty;

//物品属性数据
typedef struct _tagD3ItemInfo
{
	GUID guid;
	//数据长度
	int nLowDataLen;
	//物品数据（发包用）
	BYTE* pLowData;
	//物品属性长度
	int nLowPropertyLen;
	//物品属性字符串
	char* pLowProperty;
	//名字
	char szName[64];
	//武器类型/防具位置
	char szEquipType[64];
	//物品字段数据
	std::vector<D3ItemProperty> properties;

	_tagD3ItemInfo()
	{
		nLowDataLen = 0;
		pLowData = NULL;
		nLowPropertyLen = 0;
		pLowProperty = NULL;
		ZeroMemory(szName, sizeof(szName));
		ZeroMemory(szEquipType, sizeof(szEquipType));
	};
	~_tagD3ItemInfo()
	{
		//TRACE("D3ItemInfo destructor\n");
		if (nLowDataLen && pLowData)
		{
			delete [] pLowData;
			nLowDataLen = 0;
		}
		if (nLowPropertyLen && pLowProperty)
		{
			delete [] pLowProperty;
			nLowPropertyLen = 0;
		}
		//TRACE("D3ItemInfo destructor End\n");
	};
	_tagD3ItemInfo(const _tagD3ItemInfo & src)
	{
		*this = src;
	};
	void operator=(const _tagD3ItemInfo& src)
	{
		guid = src.guid;
		if (src.pLowData && src.nLowDataLen>0)
		{
			nLowDataLen = src.nLowDataLen;
			pLowData = new BYTE[src.nLowDataLen];
			memcpy_s(pLowData, nLowDataLen, src.pLowData, src.nLowDataLen);
		}
		if (src.pLowProperty && src.nLowPropertyLen>0)
		{
			nLowPropertyLen = src.nLowPropertyLen;
			pLowProperty = new char[src.nLowPropertyLen];
			memcpy_s(pLowProperty, nLowPropertyLen, src.pLowProperty, src.nLowPropertyLen);
		}
		memcpy_s(szName, sizeof(szName), src.szName, sizeof(src.szName));
		memcpy_s(szEquipType, sizeof(szEquipType), src.szEquipType, sizeof(src.szEquipType));

		std::vector<D3ItemProperty>::const_iterator it;
        for(it = src.properties.begin(); it != src.properties.end();it++)
		{
			properties.push_back(*it);
		}
	};
	BOOL GetProperty(LPCTSTR szFieldName, D3ItemProperty& field)
	{
		std::vector<D3ItemProperty>::const_iterator it;
		for(it = properties.begin(); it != properties.end();it++)
		{
			const D3ItemProperty& prop = *it;
			if (szFieldName && strcmp(prop.szName, szFieldName)==0)
			{
				field = prop;
				return TRUE;
			}
		}
		return FALSE;
	};
} D3ItemInfo, *PD3ItemInfo;


//搜索物品状态
enum D3emSearchItemStatus
{
	//未被竞标
	D3SEARCHNONE,
	//已被竞标
	D3SEARCHBIDED,
	//被其他人直购
	D3SEARCHOTHERBUYOUT,
	//自己直购
	D3SEARCHMYBUYOUT,
	//用户自主记录数据
	D3CUSTOMRECORD,
};

//拍卖物品记录
typedef struct _tagD3SearchItem
{
	//数据库ID
	DWORD dwDataId;
	//当前物品状态
	D3emSearchItemStatus status;
	//拍卖结束时间
	__time32_t lEndTime;
	//当前出价
	LONGLONG llCurBid;
	//直购价
	LONGLONG llBuyOut;
	//最低出价
	LONGLONG llLowestPrice;
	// 物品属性数据
	D3ItemInfo info;
	_tagD3SearchItem()
	{
		dwDataId = 0;
		status = D3emSearchItemStatus::D3SEARCHNONE;
		lEndTime =llCurBid =llBuyOut = llLowestPrice = 0;
	};
	_tagD3SearchItem(const _tagD3SearchItem & src)
	{
		*this = src;
	};
	~_tagD3SearchItem()
	{
		//TRACE("D3SearchItem destructor\n");
	};
	void operator=(const _tagD3SearchItem& src)
	{
		dwDataId = src.dwDataId;
		status = src.status;
		lEndTime = src.lEndTime;
		llCurBid = src.llCurBid;
		llBuyOut = src.llBuyOut;
		llLowestPrice = src.llLowestPrice;
		info = src.info;
	};
} D3SearchItem, *PD3SearchItem;

typedef struct _tagD3SearchCondition 
{
	//搜索多少页数据
	int nPage;
	//主角色过滤
	char szCharacterFilter[MAX_NAME_LENGTH];
	//主武器类型过滤
	char szPrimaryFilter[MAX_NAME_LENGTH];
	//次武器类型过滤
	char szSecondaryFilter[MAX_NAME_LENGTH];
	//最低等级
	int nLevelMin;
	//最高等级
	int nLevelMax;
	//物品品质过滤
	char szRarityFilter[MAX_NAME_LENGTH];
	//偏好属性 选项
	char szAdvancedComboBox[6][MAX_NAME_LENGTH];
	//偏好属性 数值
	int nAdvancedTextBox[6];
	//最高直购价
	int nBuyoutTextBox;
	//传奇或套装名字
	char szUniqueItemTextBox[MAX_NAME_LENGTH];
	_tagD3SearchCondition()
	{
		ZeroMemory(this, sizeof(D3SearchCondition));
	};
} D3SearchCondition, *PD3SearchCondition;


//背包物品数据
typedef struct _tagD3DepotItem
{
	//物品数量
	int nItemNum;
	//物品位置所在包裹
	int nPackagePos;
	//物品在包裹中的位置
	int npos;
	//?
	int npos2;
	//能否被选中
	BOOL bCanSel;
	void* pKey;
	//占用空间大小
	int nUseSpace;
	//物品数据
	D3ItemInfo info;
}D3DepotItem,*PD3DepotItem;

//拍卖物品状态
enum D3AuctionSellStatus
{
	D3ASELLNONE,
	D3ASELLCANCEL,
	D3ASELLEXPIRED
};

//拍卖物品信息
typedef struct _tagD3SellingItem
{
	//拍卖结束时间
	__time32_t lEndTime;
	//当前出价
	LONGLONG llCurBid;
	//直购价
	LONGLONG llBuyOut;
	//拍卖状态
	D3AuctionSellStatus sellStatus;
	int nIndex;
	//物品数据
	D3ItemInfo info;
}D3SellingItem,*PD3SellingItem;

//竞拍物品信息
enum D3AuctionBidStatus
{
	D3BIDNONE,
	//得標
	D3BIDWIN,
	//贏得
	D3BIDWON,
	//未得標
	D3BIDLOST
};
//竞拍物品信息
typedef struct _tagD3BiddingItem
{
	//拍卖结束时间
	__time32_t lEndTime;
	//当前出价
	LONGLONG llCurBid;
	//直购价
	LONGLONG llBuyOut;
	//我的出价
	LONGLONG llMyBid;
	//最低出价
	LONGLONG llLowestPrice;
	//状态
	D3AuctionBidStatus status;
	int nIndex;
	//物品数据
	D3ItemInfo info;
}D3BiddingItem,*PD3BiddingItem;

//完成页面物品信息
typedef struct _tagD3CompletedItem
{
	int nIndex;
	//物品数据
	D3ItemInfo info;
}D3CompletedItem,*PD3CompletedItem;

//交易状态
enum D3historyStatus
{
	D3HisNONE,
	//未賣出
	D3HisNotSold,
	//出價被超過
	D3HisOutbid,
	//已取消
	D3HisCanceled,
	//失敗
	D3HisFailed,
	//售出
	D3HisSold,
	//已售出
	D3HisCommoditySold,
	//直購
	D3HisPaidBuyout,
	//出價
	D3HisPaidBid,
	//處理中
	D3HisPending,
	//購得
	D3HisCommodityPurchased,
	//已購買
	D3HisPurchased
};
//历史记录
typedef struct _tagD3HistoryItem
{
	//日期
	__time32_t lTime;
	//交易号
	LONGLONG llTradeId;
	//状态
	D3historyStatus hStatus;
	//金额
	LONGLONG llPrice;
	//Index
	int nIndex;
	//物品数据
	D3ItemInfo info;
}D3HistoryItem,*PD3HistoryItem;

// 游戏账号设置
struct D3GameAccount
{
	char szAccount[MAX_NAME_LENGTH];
	char szPassword[MAX_NAME_LENGTH];
	INT64 nMoney;
	D3GameAccount()
	{
		ZeroMemory(this, sizeof(D3GameAccount));
	};
};
//计划任务
struct D3Schedule
{
	// 计划任务重复执行方式
	enum D3RepeatType
	{
		RepeatOnce,//仅运行一次
		RepeatBySeconds,//每隔多少秒运行一次
		RepeatByMinute,//没分钟运行一次, 指定每分钟第几秒运行
		RepeatByHour,//每小时运行一次,指定每小时的第几分钟运行
		RepeatByDay,
		RepeatByWeek,
		RepeatByMonth,
	};
	// 计划任务类型
	enum D3OpType
	{
		ScheduleNone,
		ScheduleScript,//执行脚本
		ScheduleExec,//执行进程
	};
	DWORD dwScheduleId;
	//重复类型
	D3RepeatType repeatType;
	DWORD dwRepeatParam;
	// 计划操作
	D3OpType operationType;
	std::string szName;
	std::string szDescription;
	std::string szOperationParam;
	DWORD dwBeginTime;
	DWORD dwEndTime;
	DWORD dwLastExecTime;
	DWORD dwLastError;
	BOOL bIsEnabled;
	D3Schedule()
	{
		dwScheduleId = 0;
		dwBeginTime = 0;
		dwEndTime = 0;
		dwLastError = dwLastExecTime = 0;
		bIsEnabled = FALSE;
		operationType = ScheduleNone;
		repeatType = RepeatOnce;
		szName = szDescription = szOperationParam;
	}
};



#pragma pack(pop)