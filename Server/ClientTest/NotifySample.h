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
//	// 收到认证结果
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
//	// 连接断开
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
//	// 收到配置信息
//	virtual void OnConfig(LPCTSTR szMac, LPCTSTR szConfig)
//	{
//		printf("OnAuthenticated %s\n", szConfig);
//	};
//
//	// 收到更新软件配置
//	virtual void OnUpdate(LPCTSTR szMac)
//	{
//		printf("OnUpdate\n");
//	};
//	// 收到播放列表
//	virtual void OnSchedule(LPCTSTR szMac)
//	{
//		printf("OnSchedule\n");
//	};
//	// 收到插播列表
//	virtual void OnEmSchedule(LPCTSTR szMac)
//	{
//		printf("OnEmSchedule\n");
//	};
//	// 收到执行命令
//	virtual void OnCommand(PerconCommand command, BYTE* buf, int buflen)
//	{
//		printf("OnCommand %d\n", command);
//	};
//	// 收到扩展消息
//	virtual void OnExMsg(LPCTSTR szMsg, BYTE* buf, int buflen)
//	{
//		printf("OnExMsg %s\n", szMsg);
//	};
//};