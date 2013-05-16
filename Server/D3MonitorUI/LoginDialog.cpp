// LoginDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "LoginDialog.h"


// LoginDialog 对话框

IMPLEMENT_DYNAMIC(LoginDialog, CDialog)

LoginDialog::LoginDialog(UIClient* pClient, CWnd* pParent /*=NULL*/)
	: CDialog(LoginDialog::IDD, pParent)
{
	m_pUIClient = pClient;
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_SERVICEADDR, m_cmbServiceUrls);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressBar);
}


BEGIN_MESSAGE_MAP(LoginDialog, CDialog)
	ON_BN_CLICKED(IDOK, &LoginDialog::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &LoginDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// LoginDialog 消息处理程序

void LoginDialog::OnBnClickedOk()
{
	m_btnOK.EnableWindow(FALSE);

	m_cmbServiceUrls.GetWindowText(m_szUIServer);
	if (m_szUIServer.GetLength()==0 || m_szUIServer.Find(':')<=0)
	{
		MessageBox("输入地址不正确, 请重新输入");
		return;
	}
	if(m_pUIClient->IsRunning())
	{
		m_pUIClient->StopClient();
	}

	m_pUIClient->InitClient("UIClient", m_szUIServer, NULL);
	m_pUIClient->StartClient();

	m_progressBar.SetRange(0, 100);
	//m_progressBar.SetMarquee(TRUE, 100);
	SetTimer(1,500,0);

	//OnOK();
}
void LoginDialog::OnBnClickedCancel()
{
	KillTimer(1);

	if(m_pUIClient->IsRunning())
	{
		m_pUIClient->StopClient();
		m_progressBar.SetStep(0);
		m_btnOK.EnableWindow(TRUE);
	}
	else
	{
		OnCancel();
	}
}
void LoginDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==1)
	{
		m_progressBar.StepIt();
		if(m_pUIClient->IsConnected())
		{
			KillTimer(1);
			OnOK();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL LoginDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szServerUri[0x200];
	GetPrivateProfileString("monitor_client", 
		"server", "127.0.0.1:8009", szServerUri, sizeof(szServerUri), ".\\monitor.ini");
	m_szUIServer = szServerUri;
	m_cmbServiceUrls.AddString(m_szUIServer);

	TCHAR szServerUriHistory[0x500];
	GetPrivateProfileString("monitor_client", 
		"server_history", "", szServerUri, sizeof(szServerUri), ".\\monitor.ini");

	CStringArray strList;
	if (BSHelper::SplitStr('|', szServerUriHistory, strList, TRUE))
	{
		for (int i=0;i<strList.GetCount();i++)
		{
			if (m_szUIServer.Compare(strList.GetAt(i))!=0)
			{
				m_cmbServiceUrls.AddString(strList.GetAt(i));
			}
		}
	}
	m_cmbServiceUrls.SelectString(0, m_szUIServer);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

