
// FilterMainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FilterMain.h"
#include "FilterMainDlg.h"
#include <io.h>
#include "XInstall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFilterMainDlg �Ի���




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


// CFilterMainDlg ��Ϣ�������

BOOL CFilterMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFilterMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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


	SetWindowText("SocksFilter ��������...");
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
