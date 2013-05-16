//#pragma once
//
//class NotifySample : public IPerconNotify
//{
//private:
//	IPerconClient* client;
//public:
//	NotifySample()
//	{
//		client = IPerconClient::CreateInstance();
//	}
//	// �յ���֤���
//	virtual void OnConnStateChanged(BOOL bIsOnlined, DWORD dwErrorCode=0, LPCTSTR szErrorMsg=NULL)
//	{
//		if (szErrorMsg)
//		{
//			printf("OnConnStateChanged IsOnlined:%d Error:%d %s\n", bIsOnlined, dwErrorCode, szErrorMsg);
//		}
//		else
//		{
//			printf("OnConnStateChanged IsOnlined:%d Error:%d\n", bIsOnlined, dwErrorCode);
//		}
//		if (bIsOnlined)
//		{
//			client->GetUpdate();
//			client->GetConfig();
//			client->GetSchedule();
//		}
//	};
//	// ���ӶϿ�
//	virtual void OnDisconnected(DWORD dwErrorCode, LPCTSTR szErrorMsg)
//	{
//		if (szErrorMsg)
//		{
//			printf("OnDisconnected %d %s\n", dwErrorCode, szErrorMsg);
//		}
//		else
//		{
//			printf("OnDisconnected %d\n", dwErrorCode);
//		}
//	};
//
//	// �յ�������Ϣ
//	virtual void OnConfig(LPCTSTR szMac, LPCTSTR szConfig)
//	{
//		printf("OnAuthenticated %s\n", szConfig);
//	};
//
//	// �յ������������
//	virtual void OnUpdate(LPCTSTR szMac)
//	{
//		printf("OnUpdate\n");
//	};
//	// �յ������б�
//	virtual void OnSchedule(LPCTSTR szMac)
//	{
//		printf("OnSchedule\n");
//	};
//	// �յ��岥�б�
//	virtual void OnEmSchedule(LPCTSTR szMac)
//	{
//		printf("OnEmSchedule\n");
//	};
//	// �յ�ִ������
//	virtual void OnCommand(PerconCommand command, BYTE* buf, int buflen)
//	{
//		printf("OnCommand %d\n", command);
//	};
//	// �յ���չ��Ϣ
//	virtual void OnExMsg(LPCTSTR szMsg, BYTE* buf, int buflen)
//	{
//		printf("OnExMsg %s\n", szMsg);
//	};
//};