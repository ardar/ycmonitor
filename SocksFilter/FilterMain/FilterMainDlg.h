
// FilterMainDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CFilterMainDlg �Ի���
class CFilterMainDlg : public CDialog
{
// ����
public:
	CFilterMainDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILTERMAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	DWORD m_dwExpireTime;

	// ���ɵ���Ϣӳ�亯��
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
