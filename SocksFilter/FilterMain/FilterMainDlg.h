
// FilterMainDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CFilterMainDlg 对话框
class CFilterMainDlg : public CDialog
{
// 构造
public:
	CFilterMainDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILTERMAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	DWORD m_dwExpireTime;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_txtPaths;
	DWORD m_dwCurrTime;
	afx_msg void OnBnClickedCancel();
	void GetCurrPath(OUT TCHAR *sPath);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	CButton m_btnStart;
	CButton m_btnStop;
	CEdit m_editServer;
	CEdit m_editPort;
	CEdit m_editUser;
	CEdit m_editPass;
	afx_msg void OnBnClickedButton1();
};
