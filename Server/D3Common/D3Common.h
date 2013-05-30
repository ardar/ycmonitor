#pragma once
#pragma warning(disable:4482)
#include <vector>
//#define BSCATCH_ALL_EXCEPTION

#pragma pack(push, 1)

const int MAX_NAME_LENGTH = 64;

// �������
enum D3ErrorCode
{
	ERR_Timeout=-1,
	ERR_Success,
	ERR_Unknown,//δ֪�e�`
	ERR_DBConnFailed,//������朽�ʧ��
	ERR_DBQueryFailed,//�������ԃʧ��
	ERR_SocketConnFailed,//朽�ʧ��

	ERR_UserInvalid,//�Ñ�������
	ERR_UserPasswordInvalid,//�ܴa�e�`
	ERR_UserDisabled,//�Ñ�������

	ERR_CallApiFailed,//����DllApiʧ��
	ERR_CallApiTimeout,//����API��ʱ
	ERR_CallGameError,//���ó�����Ϸ�ڴ�����Ϣ
	ERR_MoneyNotEnough,//��Ǯ����
	ERR_GameDisconnected,//�[��δ����ھ�
	ERR_AuctionClosed,//���u�����P�]

	ERR_ScriptError,//�_�������e�`
	ERR_TooManyQueuedCmd,//���ڵȴ��Ķ��й���, ���Ժ������ύ.
};

// ��Ʒ�����ֶ�
typedef struct _tagD3ItemProperty
{
	DWORD dwDataId;//�ֶ����ݿ�Id
	DWORD dwItemDataId;//��Ʒ���ݿ�ID
	char szName[64];//�ֶ���
	char szValue[64];//��ǰֵ
	int nMinValue;//��Сֵ
	int nMaxValue;//���ֵ
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

//��Ʒ��������
typedef struct _tagD3ItemInfo
{
	GUID guid;
	//���ݳ���
	int nLowDataLen;
	//��Ʒ���ݣ������ã�
	BYTE* pLowData;
	//��Ʒ���Գ���
	int nLowPropertyLen;
	//��Ʒ�����ַ���
	char* pLowProperty;
	//����
	char szName[64];
	//��������/����λ��
	char szEquipType[64];
	//��Ʒ�ֶ�����
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


//������Ʒ״̬
enum D3emSearchItemStatus
{
	//δ������
	D3SEARCHNONE,
	//�ѱ�����
	D3SEARCHBIDED,
	//��������ֱ��
	D3SEARCHOTHERBUYOUT,
	//�Լ�ֱ��
	D3SEARCHMYBUYOUT,
	//�û�������¼����
	D3CUSTOMRECORD,
};

//������Ʒ��¼
typedef struct _tagD3SearchItem
{
	//���ݿ�ID
	DWORD dwDataId;
	//��ǰ��Ʒ״̬
	D3emSearchItemStatus status;
	//��������ʱ��
	__time32_t lEndTime;
	//��ǰ����
	LONGLONG llCurBid;
	//ֱ����
	LONGLONG llBuyOut;
	//��ͳ���
	LONGLONG llLowestPrice;
	// ��Ʒ��������
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
	//��������ҳ����
	int nPage;
	//����ɫ����
	char szCharacterFilter[MAX_NAME_LENGTH];
	//���������͹���
	char szPrimaryFilter[MAX_NAME_LENGTH];
	//���������͹���
	char szSecondaryFilter[MAX_NAME_LENGTH];
	//��͵ȼ�
	int nLevelMin;
	//��ߵȼ�
	int nLevelMax;
	//��ƷƷ�ʹ���
	char szRarityFilter[MAX_NAME_LENGTH];
	//ƫ������ ѡ��
	char szAdvancedComboBox[6][MAX_NAME_LENGTH];
	//ƫ������ ��ֵ
	int nAdvancedTextBox[6];
	//���ֱ����
	int nBuyoutTextBox;
	//�������װ����
	char szUniqueItemTextBox[MAX_NAME_LENGTH];
	_tagD3SearchCondition()
	{
		ZeroMemory(this, sizeof(D3SearchCondition));
	};
} D3SearchCondition, *PD3SearchCondition;


//������Ʒ����
typedef struct _tagD3DepotItem
{
	//��Ʒ����
	int nItemNum;
	//��Ʒλ�����ڰ���
	int nPackagePos;
	//��Ʒ�ڰ����е�λ��
	int npos;
	//?
	int npos2;
	//�ܷ�ѡ��
	BOOL bCanSel;
	void* pKey;
	//ռ�ÿռ��С
	int nUseSpace;
	//��Ʒ����
	D3ItemInfo info;
}D3DepotItem,*PD3DepotItem;

//������Ʒ״̬
enum D3AuctionSellStatus
{
	D3ASELLNONE,
	D3ASELLCANCEL,
	D3ASELLEXPIRED
};

//������Ʒ��Ϣ
typedef struct _tagD3SellingItem
{
	//��������ʱ��
	__time32_t lEndTime;
	//��ǰ����
	LONGLONG llCurBid;
	//ֱ����
	LONGLONG llBuyOut;
	//����״̬
	D3AuctionSellStatus sellStatus;
	int nIndex;
	//��Ʒ����
	D3ItemInfo info;
}D3SellingItem,*PD3SellingItem;

//������Ʒ��Ϣ
enum D3AuctionBidStatus
{
	D3BIDNONE,
	//�Ø�
	D3BIDWIN,
	//�A��
	D3BIDWON,
	//δ�Ø�
	D3BIDLOST
};
//������Ʒ��Ϣ
typedef struct _tagD3BiddingItem
{
	//��������ʱ��
	__time32_t lEndTime;
	//��ǰ����
	LONGLONG llCurBid;
	//ֱ����
	LONGLONG llBuyOut;
	//�ҵĳ���
	LONGLONG llMyBid;
	//��ͳ���
	LONGLONG llLowestPrice;
	//״̬
	D3AuctionBidStatus status;
	int nIndex;
	//��Ʒ����
	D3ItemInfo info;
}D3BiddingItem,*PD3BiddingItem;

//���ҳ����Ʒ��Ϣ
typedef struct _tagD3CompletedItem
{
	int nIndex;
	//��Ʒ����
	D3ItemInfo info;
}D3CompletedItem,*PD3CompletedItem;

//����״̬
enum D3historyStatus
{
	D3HisNONE,
	//δ�u��
	D3HisNotSold,
	//���r�����^
	D3HisOutbid,
	//��ȡ��
	D3HisCanceled,
	//ʧ��
	D3HisFailed,
	//�۳�
	D3HisSold,
	//���۳�
	D3HisCommoditySold,
	//ֱُ
	D3HisPaidBuyout,
	//���r
	D3HisPaidBid,
	//̎����
	D3HisPending,
	//ُ��
	D3HisCommodityPurchased,
	//��ُ�I
	D3HisPurchased
};
//��ʷ��¼
typedef struct _tagD3HistoryItem
{
	//����
	__time32_t lTime;
	//���׺�
	LONGLONG llTradeId;
	//״̬
	D3historyStatus hStatus;
	//���
	LONGLONG llPrice;
	//Index
	int nIndex;
	//��Ʒ����
	D3ItemInfo info;
}D3HistoryItem,*PD3HistoryItem;

// ��Ϸ�˺�����
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
//�ƻ�����
struct D3Schedule
{
	// �ƻ������ظ�ִ�з�ʽ
	enum D3RepeatType
	{
		RepeatOnce,//������һ��
		RepeatBySeconds,//ÿ������������һ��
		RepeatByMinute,//û��������һ��, ָ��ÿ���ӵڼ�������
		RepeatByHour,//ÿСʱ����һ��,ָ��ÿСʱ�ĵڼ���������
		RepeatByDay,
		RepeatByWeek,
		RepeatByMonth,
	};
	// �ƻ���������
	enum D3OpType
	{
		ScheduleNone,
		ScheduleScript,//ִ�нű�
		ScheduleExec,//ִ�н���
	};
	DWORD dwScheduleId;
	//�ظ�����
	D3RepeatType repeatType;
	DWORD dwRepeatParam;
	// �ƻ�����
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