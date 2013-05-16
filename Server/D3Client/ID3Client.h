#pragma once

typedef void (*Func_OnClientCallback) (DWORD dwMsgId, DWORD dwRequestId, WPARAM wParam, LPARAM lParam);

class _declspec(dllexport) ID3Client
{
public:
	//����һ��ID3Clientʵ��
	static ID3Client* CreateInstance();

	// Client Control
	virtual BOOL InitClient(LPCTSTR szClientId, LPCTSTR szConfigFilename, Func_OnClientCallback pNotifier) = 0;
	virtual BOOL StartClient() = 0;
	virtual void StopClient() = 0;
	virtual BOOL IsConnected() = 0;
	// Facade methods
	//��֤�ͻ����˺�
	virtual DWORD Authenticate(LPCTSTR szName, LPCTSTR szPass) = 0;
	// ��ѯ��Ǯ
	virtual DWORD GetMoney() = 0;
	// ��ѯ����
	virtual DWORD GetDepotItems() = 0;
	// ��ѯ���������Ʒ
	virtual DWORD GetFinishedItems() = 0;
	// ��ѯ�ѳ�����Ʒ
	virtual DWORD GetBiddingItems() = 0;
	// ��ѯ�Ѽ�����Ʒ
	virtual DWORD GetSellingItems() = 0;
	// ��ѯ��ʷ��¼
	virtual DWORD GetHistoryItems() = 0;

	// ������Ʒ
	virtual DWORD SearchItems(const D3SearchCondition& condition) = 0;
	// ��ѯ��Ʒ���ݣ������ݿ⣩
	virtual DWORD SearchItemsInLocal(const D3SearchCondition& condition) = 0;

	// ֱ��
	virtual DWORD DoBuyOut(const D3ItemInfo& itemInfo) = 0;
	// ����
	virtual DWORD DoBid(const D3ItemInfo& itemInfo, LONGLONG llPrice) = 0;
	// �����ҵ���Ʒ
	virtual DWORD DoSell(const D3DepotItem& item, LONGLONG nStartPrice, LONGLONG nBuyoutPrice) = 0;
	// ��ȡ����Ʒ���յ���Ǯ������
	virtual DWORD DoReceive(const D3ItemInfo& itemInfo) = 0;

	// ������Ʒ����
	virtual DWORD SaveItemData(const D3ItemInfo& record, LONGLONG nBidPrice, LONGLONG nBuyoutPrice) = 0;

	// ��ȡȫ������ƻ�
	virtual DWORD GetSchedules() = 0;
	// ����ƻ�����
	virtual DWORD AddSchedule(const D3Schedule& schedule) = 0;
	// �޸ļƻ�����
	virtual DWORD EditSchedule(const D3Schedule& schedule) = 0;
	// ɾ���ƻ�����
	virtual DWORD DelSchedule(const D3Schedule& schedule) = 0;
	// �л��˺�
	virtual DWORD SwitchAccount(const D3GameAccount& acc) = 0;

};
