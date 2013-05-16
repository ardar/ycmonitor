
// SoftKVMDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SoftKVM.h"
#include "SoftKVMDlg.h"
#include "LoginDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	IDS_SERVERSTATE,
	IDS_UPSERVER,
	IDS_LISTEN,
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSoftKVMDlg 对话框




CSoftKVMDlg::CSoftKVMDlg(CWnd* pParent /*=NULL*/)
	: CDialogSampleDlgBase(CSoftKVMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_rcBorders.SetRectEmpty();
	m_bInRepositionControls = FALSE;
	m_bInitDone = FALSE;
	m_nMsgLength = 0;

	m_pService = &D3Service::Instance();

	m_pUIClient = new UIClient();
}

CSoftKVMDlg::~CSoftKVMDlg()
{
	//delete m_pService;
	delete m_pUIClient;
}


void CSoftKVMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogSampleDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MACHINE, m_wndListDevices);
	DDX_Control(pDX, IDC_EDIT_LOG, m_wndMsg);
}

BEGIN_MESSAGE_MAP(CSoftKVMDlg, CDialogSampleDlgBase)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MS_DEVICE_CLEARED,&CSoftKVMDlg::OnDeviceCleared)
	ON_MESSAGE(WM_MS_DEVICE_UPDATED,&CSoftKVMDlg::OnDeviceUpdated)
	ON_MESSAGE(WM_MS_SYSMSG,&CSoftKVMDlg::OnSysMsg)
	ON_MESSAGE(WM_MS_SYSERROR,&CSoftKVMDlg::OnSysErrorMsg)
	ON_MESSAGE(WM_MS_SERVICE_UPDATED, &CSoftKVMDlg::OnServiceInfo)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MACHINE, &CSoftKVMDlg::OnNMDblclkListDevice)
	ON_UPDATE_COMMAND_UI(ID_SYS_STARTSERVER,&CSoftKVMDlg::OnUpdateSysStartServer)
	ON_COMMAND(ID_SYS_STARTSERVER,&CSoftKVMDlg::OnSysStartServer)
	ON_WM_SIZE()
	//ON_MESSAGE(WM_MS_BOTPRINTMSG,&CSoftKVMDlg::OnPrintMsg)
	ON_WM_TIMER()
END_MESSAGE_MAP()
void CSoftKVMDlg::SetRowColor(UINT nIndex,COLORREF crl)
{
	COLORREF crBack;
	crBack=RGB(255-GetRValue(crl),255-GetGValue(crl),255-GetBValue(crl));
	m_wndListDevices.SetRowColor(nIndex,crBack,crl,1);
}
void CSoftKVMDlg::OnUpdateSysStartServer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText("启动服务");
	pCmdUI->SetText("停止服务");
	// TODO: Start and stop the service;
	
}
// CSoftKVMDlg 消息处理程序
void CSoftKVMDlg::RepositionControls()
{
	if (m_bInRepositionControls || !m_bInitDone)
		return;

	CRect rcClientStart;
	CRect rcClientNow;
	GetClientRect(rcClientStart);

	if ((GetStyle() & WS_MINIMIZE) || (rcClientStart.IsRectEmpty()))
		return;

	m_bInRepositionControls = TRUE;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);

	CRect rcBorders(rcClientNow.left - rcClientStart.left, rcClientNow.top - rcClientStart.top,  rcClientStart.right - rcClientNow.right, 
		rcClientStart.bottom - rcClientNow.bottom);

	if (rcBorders != m_rcBorders)
	{

		CPoint ptOffset(rcBorders.left - m_rcBorders.left, rcBorders.top - m_rcBorders.top);
		CSize szOffset(rcBorders.left + rcBorders.right - m_rcBorders.left - m_rcBorders.right,
			rcBorders.top + rcBorders.bottom - m_rcBorders.top - m_rcBorders.bottom);

		CRect rcWindow;
		GetWindowRect(rcWindow);
		rcWindow.BottomRight() += szOffset;

		Offset(ptOffset);
		m_szWindow += szOffset;
		m_szMin += szOffset;

		MoveWindow(rcWindow, TRUE);
	}

	m_rcBorders = rcBorders;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_bInRepositionControls = FALSE;
}
void CSoftKVMDlg::SetStatusBarText(int nIndex,LPCTSTR psz)
{
	ASSERT(nIndex>=0&& nIndex<sizeof(indicators));
	DWORD nID=indicators[nIndex];
	char szTitle[128];
	ZeroMemory(szTitle,128);
	LoadString(GetModuleHandle(NULL),nID,szTitle,127);
	CString sz;
	sz.Format("%s: %s",szTitle, psz );
	m_wndStatusBar.SetPaneText(nIndex, sz);
}
BOOL CSoftKVMDlg::OnInitDialog()
{
	CDialogSampleDlgBase::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, 100);
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_STRETCH, 200);
	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_STRETCH, 200);
	CXTPPaintManager::SetTheme(xtpThemeWhidbey);
	VERIFY(InitCommandBars());
	
	CXTPCommandBars* pCommandBars = GetCommandBars();
	//pCommandBars->SetMenu(_T("主菜单"), IDR_MENU_MAIN);
// 	pCommandBars->GetCommandBarsOptions()->ShowKeyboardCues(xtpKeyboardCuesShowWindowsDefault);
// 	pCommandBars->GetToolTipContext()->SetStyle(xtpToolTipOffice);
	
//	pCommandBars->GetShortcutManager()->SetAccelerators(IDR_ACCELERATOR);

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	SetResize(IDC_LIST_MACHINE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	//SetResize(IDC_LIST_ERRLIST, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT_LOG, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	m_wndListDevices.InsertColumn(0, _T("设备名/Id"), LVCFMT_LEFT, 1);
	m_wndListDevices.InsertColumn(1, _T("账户"), LVCFMT_LEFT, 2);
	m_wndListDevices.InsertColumn(2, _T("IP地址"), LVCFMT_LEFT, 2);
	m_wndListDevices.InsertColumn(3, _T("端口"), LVCFMT_LEFT, 2);
	m_wndListDevices.InsertColumn(4, _T("状态"), LVCFMT_LEFT, 2);
	m_wndListDevices.InsertColumn(5, _T("最后操作"), LVCFMT_LEFT, 2);
	m_wndListDevices.InsertColumn(6, _T("操作时间"), LVCFMT_LEFT, 2);

	HWND hWndHeader = m_wndListDevices.GetDlgItem(0)->GetSafeHwnd();
	m_wndListDevicesHeader.SubclassWindow(hWndHeader);

	// add bitmap images.
	//m_wndListMachineHeader.SetBitmap(0, IDB_COLUMN_0, FALSE, RGB(0,255,0));
	//m_wndListMachineHeader.SetBitmap(1, IDB_COLUMN_1, FALSE, RGB(0,255,0));
	//m_wndListMachineHeader.SetBitmap(2, IDB_COLUMN_2, FALSE, RGB(0,255,0));

	// enable auto sizing.
	m_wndListDevicesHeader.EnableAutoSize(TRUE);
	m_wndListDevicesHeader.ResizeColumnsToFit();

	m_wndListDevices.ModifyExtendedStyle(0, LVS_EX_FULLROWSELECT|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);

	m_wndListDevices.EnableUserRowColor(TRUE);
	m_wndListDevicesHeader.SetTheme(new CXTHeaderCtrlThemeExplorer());
	
	//if(m_BotListDlg.Create(IDD_DIALOG_BOTS,this))
	//{
	//	m_BotListDlg.Hide();
	//}
	CRect rcClientNow;
	m_bInitDone = TRUE;
	RepositionControls();
	SetStatusBarText(0);
	SetStatusBarText(1);
	SetStatusBarText(2);
	SetWindowText("未连接");
	
	ShowWindow(SW_SHOW);
	CenterWindow();
// 	for (int i=0;i<DetectTypeCount;i++)
// 	{
// 		m_wndListMachine.InsertItem(i,"");
// 		m_wndListMachine.SetItemText(i,1,"颜色测试");
// 		CString sz;
// 		sz.Format("12.12.22.%d",i);
// 		m_wndListMachine.SetItemText(i,2,sz);
// 		SetRowColor(i,m_BotListDlg.m_DetectConfig[i].crl);
// 		m_wndListMachine.SetItemData(i,i+1);
// 	}

	SetTimer(0,5000,0);


	m_pService->InitServer(".\\service.ini");
	m_pService->StartServer();

	m_pUIClient->SetMainWnd( m_hWnd );
	/*LoginDialog loginDlg(m_pUIClient, this);
	loginDlg.DoModal();
	m_szUIServer = loginDlg.m_szUIServer;*/

	TCHAR szServerUri[0x200];
	GetPrivateProfileString("monitor_client", 
		"server", "127.0.0.1:8009", szServerUri, sizeof(szServerUri), ".\\monitor.ini");
	m_szUIServer = szServerUri;
	m_pUIClient->InitClient("UIClient", m_szUIServer, NULL);
	m_pUIClient->StartClient();

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSoftKVMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogSampleDlgBase::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSoftKVMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogSampleDlgBase::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSoftKVMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

long CSoftKVMDlg::OnServiceInfo( WPARAM wParam,LPARAM lParam )
{
	Monitor_ServiceInfo* pInfo = (Monitor_ServiceInfo*)lParam;
	if (!pInfo)
	{
		CString sz;
		sz.Format("未连接监控目标");
		SetWindowText(sz);
	}
	else
	{
		ASSERT(pInfo);

		
		CString sz;
		sz.Format("监控:%s", m_szUIServer);
		SetStatusBarText(0, sz);

		CString szClientConn = pInfo->bIsClientConnected ? "已连接" : "未连接";
		sz.Format("%s(%s)", pInfo->szUpServerUri, szClientConn);
		SetStatusBarText(1, sz);

		CString szServerConn = pInfo->bIsServerRunning ? "正在监听" : "未监听";
		sz.Format("%s(%s)", pInfo->szListenUri, szServerConn);
		SetStatusBarText(2, sz);

		sz.Format("正在监控(%s) Listen：%s 當前用戶名:%s", 
			m_szUIServer, pInfo->szListenUri, pInfo->szCurrGameAccount);
		SetWindowText(sz);

		delete pInfo;
		pInfo = NULL;
	}
	return 0;
}

void CSoftKVMDlg::DrawErrState(UINT nIndex)
{
	DWORD MachineID=m_wndListDevices.GetItemData(nIndex);
	//MonitorService::Instance().RequestErrorState(MachineID, 0);
	/*for (int i=1;i<DetectTypeCount;i++)
	{
		if(m_BotListDlg.m_DetectConfig[i].bDetect)
		{
			if(MonitorService::Instance().GetMachineErrState((MONITOR_ERR_FIELD)i,MachineID,m_BotListDlg.m_DetectConfig[i].dwTimeOutLimit))
			{
				SetRowColor(nIndex,m_BotListDlg.m_DetectConfig[i].crl);
				return;
			}
		}
	}
	SetRowColor(nIndex);*/
}

long CSoftKVMDlg::OnMachineErrState( WPARAM wParam,LPARAM lParam )
{
	DWORD dwMachineId = (DWORD)wParam;
	/*MONITOR_ERR_FIELD ErrField = (MONITOR_ERR_FIELD)lParam;*/
	int nIndex = FindDevice(dwMachineId);

	if (nIndex>=0)
	{
		/*if (ErrField>0)
		{
			SetRowColor(nIndex,m_BotListDlg.m_DetectConfig[ErrField].crl);
			if (m_BotListDlg.m_DetectConfig[ErrField].bPlayMusic)m_BotListDlg.PlayErr();
		}
		else
		{
			SetRowColor(nIndex);
		}*/
	}
	return 0;
}
void CSoftKVMDlg::SortColumn(int iCol, BOOL bAsc)
{
	m_bAscending = bAsc;
	m_nSortedCol = iCol;



	CXTSortClass csc(&m_wndListDevices,m_nSortedCol);
	csc.Sort(m_bAscending, xtSortString);
}
BOOL CSoftKVMDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDNotify = (HD_NOTIFY*)lParam;

	if (pHDNotify->hdr.code == HDN_ITEMCLICKA ||
		pHDNotify->hdr.code == HDN_ITEMCLICKW)
	{
		if (pHDNotify->iItem == m_nSortedCol)
			SortColumn(pHDNotify->iItem, !m_bAscending);
		else
			SortColumn(pHDNotify->iItem,m_wndListDevicesHeader.GetAscending());

	}

	return CXTResizeDialog::OnNotify(wParam, lParam, pResult);
}
long CSoftKVMDlg::OnBotErrState( WPARAM wParam,LPARAM lParam )
{
	//m_BotListDlg.SendMessage(WM_MS_BOTERRSTATE,wParam,lParam);
	return 0;
}

int CSoftKVMDlg::FindDevice(DWORD dwMachineKey)
{
	int nCount=m_wndListDevices.GetItemCount();
	for (int i=0;i<nCount;i++)
	{
		Monitor_ClientInfo* pOldInfo = (Monitor_ClientInfo*)m_wndListDevices.GetItemData(i);
		if (pOldInfo && pOldInfo->dwSessionId==dwMachineKey)
		{
			return i;
		}
	}
	return -1;
}

LONG CSoftKVMDlg::OnDeviceCleared( WPARAM wParam,LPARAM lParam )
{
	int nCount=m_wndListDevices.GetItemCount();
	for (int i=0;i<nCount;i++)
	{
		Monitor_ClientInfo* pOldInfo = (Monitor_ClientInfo*)m_wndListDevices.GetItemData(i);
		m_wndListDevices.SetItemData(i, NULL);
		delete pOldInfo;
	}
	m_wndListDevices.DeleteAllItems();
	return 0;
}

LONG CSoftKVMDlg::OnDeviceUpdated( WPARAM wParam,LPARAM lParam )
{
	DWORD dwDeviceId = (DWORD)wParam;
	int Index=FindDevice(dwDeviceId);
	Monitor_ClientInfo *info=(Monitor_ClientInfo *)lParam;

	if(Index!=-1)
	{
		if(info==NULL)
		{
			/*if(dwDeviceId==m_BotListDlg.m_dwCurrentDeviceKey)
			m_BotListDlg.Hide();*/
			Monitor_ClientInfo* pOldInfo = (Monitor_ClientInfo*)m_wndListDevices.GetItemData(Index);
			m_wndListDevices.DeleteItem(Index);
			delete pOldInfo;
		}
		else
		{
			CString sz;
			sz.Format("%d",info->dwSessionId);
			m_wndListDevices.SetItemText(Index,0,sz);

			sz.Format("%s",info->szServiceUser);
			m_wndListDevices.SetItemText(Index,1,sz);

			sz.Format("%s",info->szClientIP);
			m_wndListDevices.SetItemText(Index,2,sz);

			sz.Format("%d",info->dwClientPort);
			m_wndListDevices.SetItemText(Index,3,sz);

			sz.Format("%s",info->bIsAuthenticated ? "已认证":"未认证");
			m_wndListDevices.SetItemText(Index,4,sz);

			sz.Format("%s",info->szLastOperation);
			m_wndListDevices.SetItemText(Index,5,sz);

			CTime theTime(info->dwLastOpTime);
			sz.Format("%d時%d分d秒",theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
			m_wndListDevices.SetItemText(Index,6,sz);

			Monitor_ClientInfo* pOldInfo = (Monitor_ClientInfo*) m_wndListDevices.GetItemData(Index);
			if (pOldInfo && pOldInfo!=info)
			{
				delete pOldInfo;
			}
			m_wndListDevices.SetItemData(Index, (DWORD_PTR)info);
			info = NULL;
			//获取是否出错，然后换颜色
			//DrawErrState(Index);
		}
	}else
	{
		if(info==NULL)return 0;

		CString titile;
		titile.Format("%d",m_wndListDevices.GetItemCount()+1);
		Index=m_wndListDevices.InsertItem(m_wndListDevices.GetItemCount(),titile);

		CString sz;
		sz.Format("%d",info->dwSessionId);
		m_wndListDevices.SetItemText(Index,0,sz);

		sz.Format("%s",info->szServiceUser);
		m_wndListDevices.SetItemText(Index,1,sz);

		sz.Format("%s",info->szClientIP);
		m_wndListDevices.SetItemText(Index,2,sz);

		sz.Format("%d",info->dwClientPort);
		m_wndListDevices.SetItemText(Index,3,sz);

		sz.Format("%s",info->bIsAuthenticated ? "已认证":"未认证");
		m_wndListDevices.SetItemText(Index,4,sz);

		sz.Format("%s",info->szLastOperation);
		m_wndListDevices.SetItemText(Index,5,sz);

		CTime theTime(info->dwLastOpTime);
		sz.Format("%d時%d分d秒",theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		m_wndListDevices.SetItemText(Index,6,sz);

		Monitor_ClientInfo* pOldInfo = (Monitor_ClientInfo*) m_wndListDevices.GetItemData(Index);
		if (pOldInfo && pOldInfo!=info)
		{
			delete pOldInfo;
		}
		m_wndListDevices.SetItemData(Index, (DWORD_PTR)info);
		info = NULL;
		//获取是否出错，然后换颜色

	}
	if (info!=NULL)
	{
		delete info;
	}

	return 0;
}

long CSoftKVMDlg::OnPrintMsg( WPARAM wParam,LPARAM lParam )
{
	/*if((DWORD )wParam==m_BotListDlg.m_dwFocusBotId)
	{
		m_BotListDlg.SendMessage(WM_MS_BOTPRINTMSG,wParam,lParam);
	}
	else
	{
		CString* pStr = (CString*)lParam;
		delete pStr;
	}*/
	return 0;
}

LONG CSoftKVMDlg::OnSysMsg( WPARAM wParam,LPARAM lParam )
{
	DWORD dwLevel = (DWORD)(wParam);
	CString* pStr = (CString*)(lParam);
	ASSERT(pStr);

	TCHAR szPrintMsg[0x400];
	CTime time = CTime::GetCurrentTime();
	sprintf_s(szPrintMsg, 0x400, "[%s]:%s\r\n", time.Format("%H:%M:%S").GetBuffer(), pStr->GetBuffer());

	m_wndMsg.SetSel(m_nMsgLength, m_nMsgLength, FALSE);
	m_wndMsg.ReplaceSel(szPrintMsg);
	m_nMsgLength += BSHelper::bsstrlen(szPrintMsg);
	m_wndMsg.SetSel(m_nMsgLength, m_nMsgLength, FALSE);

	// Remove the old msg from top.
	if(m_nMsgLength>0x8000)
	{
		m_wndMsg.SetSel(0, m_nMsgLength-0x4000);
		m_wndMsg.ReplaceSel("");
		m_nMsgLength = 0x4000;
	}

	m_wndMsg.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	delete pStr;
	pStr = NULL;
	return 0;
}

LONG CSoftKVMDlg::OnSysErrorMsg( WPARAM wParam,LPARAM lParam )
{
	//DWORD dwLevel = (DWORD)(wParam);
	//MonitorServiceError* pError = (MonitorServiceError*)(lParam);
	//ASSERT(pError);

	//TRACE("SysError:%s\n",pError->strErrorMessage.GetBuffer());

	//TCHAR szPrintMsg[0x500];
	//CTime time = CTime::GetCurrentTime();
	//sprintf_s(szPrintMsg, 0x500, "[%s][Error]:%s\r\n", time.Format("%H:%M:%S").GetBuffer(), pError->strErrorMessage.GetBuffer());

	//m_wndMsg.SetSel(m_nMsgLength, m_nMsgLength, TRUE);
	//m_wndMsg.ReplaceSel(szPrintMsg);
	//m_nMsgLength += BSHelper::bsstrlen(szPrintMsg);
	//m_wndMsg.SetSel(m_nMsgLength, m_nMsgLength, FALSE);

	//// Remove the old msg from top.
	//if(m_nMsgLength>0x3000)
	//{
	//	m_wndMsg.SetSel(0, m_nMsgLength-0x500);
	//	m_wndMsg.ReplaceSel("");
	//	m_nMsgLength = 0x500;
	//}

	//m_wndMsg.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//delete pError;
	//pError = NULL;
	return 0;
}
void CSoftKVMDlg::OnNMDblclkListDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int Index=m_wndListDevices.GetSelectionMark();
	if(Index<0||Index>=m_wndListDevices.GetItemCount())return;
	Monitor_ClientInfo * pInfo = (Monitor_ClientInfo *)m_wndListDevices.GetItemData(Index);
	
	
	*pResult = 0;
}

void CSoftKVMDlg::OnSize(UINT nType, int cx, int cy)
{
	CXTPDialogBase<CXTResizeDialog>::OnSize(nType, cx, cy);
	RepositionControls();
	// TODO: 在此处添加消息处理程序代码
}

void CSoftKVMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_pUIClient->RefreshService();
	//m_pUIClient->RefreshDevices();
 	/*for (int i=0;i<m_wndListDevices.GetItemCount();i++)
 	{
 		DrawErrState(i);
 	}*/

	CXTPDialogBase<CXTResizeDialog>::OnTimer(nIDEvent);
}

void CSoftKVMDlg::OnSysStartServer()
{
	// TODO: Start the service;
}