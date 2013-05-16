// ClientTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ClientTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MONITOR_INI_FILE ".\\D3Client.ini"

// 唯一的应用程序对象

CWinApp theApp;

ID3Client* pClient = ID3Client::CreateInstance();

using namespace std;

static int nImageNum = 0;
vector<D3SearchItem*>* searchList = new vector<D3SearchItem*>();
vector<D3SearchItem*>* localsearchList = new vector<D3SearchItem*>();
vector<D3DepotItem*>* depotList = new vector<D3DepotItem*>();
D3Schedule schedule;

void  OnClientMsg(DWORD msgId, DWORD dwRequestId, WPARAM wParam, LPARAM lParam)
{
	switch(msgId)
	{
	case D3Event_Connected:
		printf("connected\n");
		break;
	case D3Event_Disconnected:
		printf("connected\n");
		break;
	case D3Event_GetMoney:
		{
			INT64 nMoney = 0;
			if(lParam)
				nMoney = *(INT64*)lParam;
			printf("getmoney  %I64d\n", nMoney);
		}
		break;
	case D3Event_Auth:
		printf("auth result %d\n", wParam);
		break;
	case D3Event_SearchItems:
		if (wParam==0 && lParam)
		{
			searchList->clear();
			vector<D3SearchItem*>* itemList = (vector<D3SearchItem*>*)lParam;
			vector<D3SearchItem*>::const_iterator it;
			for (it=itemList->begin();it!=itemList->end();it++)
			{
				D3SearchItem* pItem = *it;
				printf("item: %d, %I64d, %I64d, %I64d, %s %d\n",
					pItem->dwDataId, pItem->llBuyOut, pItem->llCurBid, pItem->llLowestPrice,
					pItem->info.szName, pItem->status);
				searchList->push_back(pItem);
			}
		}
		printf("search result %d\n", wParam);
		break;
	case D3Event_SearchItemsInLocal:
		if (wParam==0 && lParam)
		{
			localsearchList->clear();
			vector<D3SearchItem*>* itemList = (vector<D3SearchItem*>*)lParam;
			vector<D3SearchItem*>::const_iterator it;
			for (it=itemList->begin();it!=itemList->end();it++)
			{
				D3SearchItem* pItem = *it;
				printf("item: %d, %I64d, %I64d, %I64d, %s %d\n",
					pItem->dwDataId, pItem->llBuyOut, pItem->llCurBid, pItem->llLowestPrice,
					pItem->info.szName, pItem->status);
				localsearchList->push_back(pItem);
			}
		}
		printf("search local result %d\n", wParam);
		break;
	case D3Event_GetDepotItems:
		if (wParam==0 && lParam)
		{
			depotList->clear();
			vector<D3DepotItem*>* itemList = (vector<D3DepotItem*>*)lParam;
			vector<D3DepotItem*>::const_iterator it;
			for (it=itemList->begin();it!=itemList->end();it++)
			{
				D3DepotItem* pItem = *it;
				if(pItem->npos==15)
				{
					printf("item: pos:%d, num:%d, ppos:%d, cansel:%d, %s \n",
						pItem->npos, pItem->nItemNum, pItem->nPackagePos, pItem->bCanSel,
						pItem->info.szName);
					depotList->push_back(pItem);
				}
			}
		}
		printf("getdepot result %d\n", wParam);
		break;
	case D3Event_GetSellingItems:
		if (wParam==0 && lParam)
		{
			vector<D3SellingItem*>* itemList = (vector<D3SellingItem*>*)lParam;
			vector<D3SellingItem*>::const_iterator it;
			for (it=itemList->begin();it!=itemList->end();it++)
			{
				D3SellingItem* pItem = *it;
				printf("item: llCurBid:%I64d, llBuyOut:%I64d, lEndTime:%d, status:%d, %s \n",
					pItem->llCurBid, pItem->llBuyOut, pItem->lEndTime, pItem->sellStatus,
					pItem->info.szName);
			}
		}
		printf("getselling result %d\n", wParam);
		break;
	case D3Event_GetFinishedItems:
		if (wParam==0 && lParam)
		{
			vector<D3CompletedItem*>* itemList = (vector<D3CompletedItem*>*)lParam;
			vector<D3CompletedItem*>::const_iterator it;
			for (it=itemList->begin();it!=itemList->end();it++)
			{
				D3CompletedItem* pItem = *it;
				printf("item: index:%d, %s \n",
					pItem->nIndex, 
					pItem->info.szName);
			}
			it = itemList->begin();
			D3CompletedItem* pItem = *it;
			pClient->DoReceive(pItem->info);
		}
		printf("getfinished result %d\n", wParam);
		break;
	case D3Event_DoBid:
		printf("D3Event_DoBid result %d %d\n", wParam, lParam);
		break;
	case D3Event_DoBuyOut:
		printf("D3Event_DoBuyOut result %d %d\n", wParam, lParam);
		break;
	case D3Event_DoReceive:
		printf("D3Event_DoReceive result %d %d\n", wParam, lParam);
		break;
	case D3Event_DoSell:
		printf("D3Event_DoSell result %d %d\n", wParam, lParam);
		break;
	case D3Event_SaveItem:
		printf("saveitem result %d %d\n", wParam, lParam);
		break;
	case D3Event_GetSchedules:
		if (wParam==0 && lParam)
		{
			vector<D3Schedule*>* itemList = (vector<D3Schedule*>*)lParam;
			vector<D3Schedule*>::const_iterator it;
			for (it=itemList->begin();it!=itemList->end();it++)
			{
				D3Schedule* pItem = *it;
				printf("schedule: %d, %s : %s\n",
					pItem->dwScheduleId, 
					pItem->szName, pItem->szOperationParam);
			}
		}
		printf("getschedule result %d %d\n", wParam, lParam);
		break;
	case D3Event_AddSchedule:
		printf("addschedule result %d %d\n", wParam, lParam);
		schedule.dwScheduleId = lParam;
		break;
	case D3Event_EditSchedule:
		printf("editschedule result %d %d\n", wParam, lParam);
		break;
	case D3Event_DelSchedule:
		printf("delschedule result %d %d\n", wParam, lParam);
		break;
	case D3Event_SwitchAccount:
		printf("switchacc:result:%d %d, %d" );
			break;
	}
	printf("OnClientMsg:%d req:%d w:%d l:%d\n", msgId, dwRequestId, wParam, lParam);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}
	BSHelper::GoAppRootPath();
	TCHAR szMac[MAX_NAME_LENGTH];
	GetPrivateProfileString("D3Client", "mac", "", szMac, MAX_NAME_LENGTH, MONITOR_INI_FILE);
	
	pClient->InitClient(szMac, MONITOR_INI_FILE, OnClientMsg);
	pClient->StartClient();

	while(!pClient->IsConnected())
	{
		Sleep(500);
	}
	printf("connected\n");
	DWORD dwResult = pClient->Authenticate("ad","123");

	printf("authresult %d\n", dwResult);
	pClient->GetMoney();
	TCHAR sz[100];
	BOOL bRunning = TRUE;
	while(bRunning)
	{
		scanf("%c",sz);
		switch(sz[0])
		{
		case 'i':
			{
				if (searchList)
				{
					vector<D3SearchItem*>::iterator it;
					for(it = searchList->begin();it!=searchList->end();it++)
					{
						D3SearchItem* pItem = *it;
						if (pItem->status==D3emSearchItemStatus::D3SEARCHBIDED)
						{
							//pClient->DoBuyOut(pItem->info);
							INT64 llPrice = pItem->llCurBid>pItem->llLowestPrice ? pItem->llCurBid : pItem->llLowestPrice;
							llPrice += 1000;
							llPrice = llPrice > pItem->llBuyOut ? pItem->llBuyOut : llPrice;
							pClient->DoBid(pItem->info, llPrice);
							break;
						}
					}
				}
			}
			break;
		case 'o':
			{
				vector<D3SearchItem*>::iterator it;
				for(it = searchList->begin();it!=searchList->end();it++)
				{
					D3SearchItem* pItem = *it;
					if (pItem->status==D3emSearchItemStatus::D3SEARCHBIDED)
					{
						pClient->DoBuyOut(pItem->info);
						break;
					}
				}
			}
			break;
		case 'a':
			{
				vector<D3SearchItem*>::iterator it;
				for(it = searchList->begin();it!=searchList->end();it++)
				{
					D3SearchItem* pItem = *it;
					if (pItem->status==D3emSearchItemStatus::D3SEARCHBIDED)
					{
						pClient->SaveItemData(pItem->info, pItem->llCurBid, pItem->llBuyOut);
						break;
					}
				}
			}
			break;
		case 'e':
			{
				vector<D3DepotItem*>::iterator it;
				for(it = depotList->begin();it!=depotList->end();it++)
				{
					D3DepotItem* pItem = *it;
					if (pItem->bCanSel)
					{
						pClient->DoSell(*pItem, 20000, 200000);
						break;
					}
				}
			}
			break;
		case 'r':
		case 'l':
			{
				D3SearchCondition con;
				con.nPage = 1;
				con.nBuyoutTextBox = 10000;
				strcpy(con.szCharacterFilter, "巫醫");
				strcpy(con.szPrimaryFilter, "單手");
				strcpy(con.szSecondaryFilter, "所有單手類型物品");
				strcpy(con.szRarityFilter, "全部");
				for(int i=0;i<6;i++)
				{
					strcpy(con.szAdvancedComboBox[i],"無");
				}
				con.nBuyoutTextBox = 2000;
				if (sz[0]=='r')
				{
					pClient->SearchItems(con);
				}
				else
				{
					pClient->SearchItemsInLocal(con);
				}
			};
			break;
		case 'c':
			pClient->GetFinishedItems();
			break;
		case 'd':
			pClient->GetDepotItems();
			break;
		case 's':
			pClient->GetSellingItems();
			break;
		case 'b':
			pClient->GetBiddingItems();
			break;
		case 'm':
			pClient->GetMoney();
			break;
		case 'h':
			{
				schedule.bIsEnabled = TRUE;
				schedule.dwBeginTime = time(0);
				schedule.dwEndTime = 0;
				schedule.dwRepeatParam = time(0)%3600 + 10;
				schedule.repeatType = D3Schedule::RepeatByHour;
				schedule.operationType = D3Schedule::ScheduleScript;
				schedule.szOperationParam = "收取完成物品()";
				schedule.szName = "测试收取全部物品计划";
				DWORD dwRequestId = pClient->AddSchedule(schedule);
				break;
			}
		case 'k':
			{
		        schedule.bIsEnabled = TRUE;
				schedule.dwBeginTime = time(0);
				schedule.dwEndTime = time(0)+86400;
				schedule.dwRepeatParam = 30;
				schedule.repeatType = D3Schedule::RepeatByMinute;
				schedule.operationType = D3Schedule::ScheduleScript;
				schedule.szOperationParam = "搜索物品(\
						\"巫醫\",\"單手\",\"所有單手類型物品\",\"全部\")";
				schedule.szName = "搜索物品";
				DWORD dwRequestId = pClient->EditSchedule(schedule);
			}
		case 'j':
			{
				DWORD dwRequestId = pClient->GetSchedules();
			}
			break;
		case 'w':
			{
				D3GameAccount acc;
				strcpy_s(acc.szAccount, sizeof(acc.szAccount), "aaa");      
				strcpy_s(acc.szAccount, sizeof(acc.szAccount), "bbb");     
				pClient->SwitchAccount(acc);
			}
			break;
		case 'x':
			bRunning = FALSE;
			break;
		}
		Sleep(1000);
	};
	return nRetCode;
}
