#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "afxcmn.h"


// LoginDialog 对话框

class LoginDialog : public CDialog
{
	DECLARE_DYNAMIC(LoginDialog)

public:
	LoginDialog(UIClient* pClient, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~LoginDialog();

// 对话框数据
	enum { IDD = IDD_LOGINDIALOG };

	CString m_szUIServer;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	UIClient* m_pUIClient;
	CComboBox m_cmbServiceUrls;
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	CButton m_btnOK;
	CButton m_btnCancel;
	CProgressCtrl m_progressBar;
	afx_msg void OnBnClickedCancel();
};
