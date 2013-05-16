
// FilterMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FilterMain.h"
#include "FilterMainDlg.h"
#include <io.h>
#include "XInstall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFilterMainDlg 对话框




CFilterMainDlg::CFilterMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwExpireTime = 1336406571;
}

void CFilterMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_txtPaths);
	DDX_Control(pDX, ID_START, m_btnStart);
	DDX_Control(pDX, ID_STOP, m_btnStop);
	DDX_Control(pDX, IDC_EDIT3, m_editServer);
	DDX_Control(pDX, IDC_EDIT4, m_editPort);
	DDX_Control(pDX, IDC_EDIT5, m_editUser);
	DDX_Control(pDX, IDC_EDIT6, m_editPass);
	m_dwCurrTime = time(0);
}

BEGIN_MESSAGE_MAP(CFilterMainDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CFilterMainDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_START, &CFilterMainDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_STOP, &CFilterMainDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_BUTTON1, &CFilterMainDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFilterMainDlg 消息处理程序

BOOL CFilterMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	TCHAR szPaths[0x2000];
	GetPrivateProfileString("filter", "paths", "", szPaths, sizeof(szPaths), ".\\filter.ini");
	CString strPaths = szPaths;
	strPaths.Replace("|", "\r\n");
	m_txtPaths.SetWindowText(strPaths);
	TCHAR szServer[0x100],szUser[0x100], szPass[0x100];
	TCHAR szServerPort[0x100];
	GetPrivateProfileString("socks5", "server", "127.0.0.1", szServer, sizeof(szServer), ".\\filter.ini");
	GetPrivateProfileString("socks5", "port", "1080", szServerPort, sizeof(szServerPort), ".\\filter.ini");
	GetPrivateProfileString("socks5", "user", "", szUser, sizeof(szUser), ".\\filter.ini");
	GetPrivateProfileString("socks5", "pass", "", szPass, sizeof(szPass), ".\\filter.ini");
	m_editServer.SetWindowText(szServer);
	m_editUser.SetWindowText(szUser);
	m_editPass.SetWindowText(szPass);
	m_editPort.SetWindowText(szServerPort);

	m_btnStop.EnableWindow(FALSE);

	SetWindowText("SocksFilter");


	if (time(0)>m_dwExpireTime)
	{
		memcpy(this+0x33, &this->m_editPass, 0x12);
		exit(0);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFilterMainDlg::OnPaint()
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

		if (m_dwCurrTime>m_dwExpireTime)
		{
			memcpy(this+0x123, &this->m_editPass, 0x100);
		}
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFilterMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFilterMainDlg::GetCurrPath(OUT TCHAR *sPath) 
{
	TCHAR sFilename[MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];

	GetModuleFileName(NULL, sFilename, _MAX_PATH);

	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	_tcscpy(sPath, sDrive);
	_tcscat(sPath, sDir);

	if(sPath[_tcslen(sPath) - 1] != _T('\\'))
		_tcscat(sPath, _T("\\"));
}  
void CFilterMainDlg::OnOK()
{
	return;
}

void CFilterMainDlg::OnBnClickedCancel()
{
	CXInstall	m_Install;
	m_Install.RemoveProvider();
	OnCancel();
}

void CFilterMainDlg::OnBnClickedStart()
{
	CXInstall	m_Install;
	if (m_Install.IsInstalled())
	{
		m_Install.RemoveProvider();
	}
	TCHAR sProvider[MAX_PATH] = {0};
	GetCurrPath(sProvider);
	if(sProvider[0] == '\0')
	{
		AfxMessageBox("Can't find the application path.\n");
		return;
	}
	_tcscat(sProvider, XFILTER_SERVICE_DLL_NAME);

	if(_taccess(sProvider, 0) == -1)
	{
		CString sz;
		sz.Format("Can't find %s", sProvider);
		AfxMessageBox(sz);
		return;
	}
	CString strProcessPaths;
	CString szServer, szPort, szUser, szPass;
	m_txtPaths.GetWindowText(strProcessPaths);
	m_editServer.GetWindowText(szServer);
	m_editPort.GetWindowText(szPort);
	m_editUser.GetWindowText(szUser);
	m_editPass.GetWindowText(szPass);
	strProcessPaths.Replace("\r\n", "\n");
	strProcessPaths.Replace("\n\n", "\n");
	strProcessPaths.Replace("\n", "|");
	WritePrivateProfileString("filter", "paths", strProcessPaths, ".\\filter.ini");
	WritePrivateProfileString("socks5", "server", szServer, ".\\filter.ini");
	WritePrivateProfileString("socks5", "port", szPort, ".\\filter.ini");
	WritePrivateProfileString("socks5", "user", szUser, ".\\filter.ini");
	WritePrivateProfileString("socks5", "pass", szPass, ".\\filter.ini");

	m_Install.InstallProvider(sProvider, strProcessPaths.Trim().GetBuffer());
	m_btnStart.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);


	SetWindowText("SocksFilter 正在运行...");
}

void CFilterMainDlg::OnBnClickedStop()
{
	CXInstall	m_Install;
	m_Install.RemoveProvider();
	m_btnStart.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);
	SetWindowText("SocksFilter");
}

void CFilterMainDlg::OnBnClickedButton1()
{
	CFileDialog filed(TRUE);
	if (filed.DoModal()==IDOK)
	{
		CString szFilePath = filed.GetPathName();
		UpdateData(FALSE);
		CString szOriPath;
		m_txtPaths.GetWindowText(szOriPath);
		szOriPath = szOriPath.Trim();
		szOriPath.AppendFormat("\r\n%s", szFilePath.Trim());
		m_txtPaths.SetWindowText(szOriPath);
	}
}
