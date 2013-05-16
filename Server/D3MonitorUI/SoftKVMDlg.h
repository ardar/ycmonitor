
// SoftKVMDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#define CDialogSampleDlgBase CXTPDialogBase<CXTResizeDialog>
// CSoftKVMDlg �Ի���

class CSoftKVMDlg : public CDialogSampleDlgBase
{
// ����
public:

	CSoftKVMDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSoftKVMDlg();

// �Ի�������
	enum { IDD = IDD_SOFTKVM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ʵ��
protected:
	CString m_szUIServer;
	UIClient* m_pUIClient;
	Monitor_ServiceInfo m_serviceInfo;
	D3Service* m_pService;
	HICON m_hIcon;
 	void RepositionControls();
 	CRect m_rcBorders;
 	BOOL m_bInRepositionControls, m_bInitDone;
	CEdit m_wndMsg;
	UINT m_nMsgLength;
	int             m_nSortedCol;
	BOOL            m_bAscending;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnNMDblclkListDevice(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg	void OnUpdateSysStartServer(CCmdUI* pCmdUI);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LONG OnDeviceCleared(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnDeviceUpdated(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnSysMsg( WPARAM wParam,LPARAM lParam );
	afx_msg LONG OnSysErrorMsg( WPARAM wParam,LPARAM lParam );
	afx_msg long OnMachineErrState( WPARAM wParam,LPARAM lParam );
	afx_msg long OnBotErrState( WPARAM wParam,LPARAM lParam );
	afx_msg long OnServiceInfo( WPARAM wParam,LPARAM lParam );
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	
public:
	CXTListCtrl m_wndListDevices;
	CXTHeaderCtrl m_wndListDevicesHeader;
	CXTPStatusBar m_wndStatusBar;
	int FindDevice(DWORD dwDeviceId);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetStatusBarText(int nIndex,LPCTSTR psz="");
	void OnSysStartServer();
	void SetRowColor(UINT nIndex,COLORREF crl=RGB(255,255,255));
	long OnPrintMsg( WPARAM wParam,LPARAM lParam );
	void DrawErrState(UINT nIndex);
	void SortColumn(int iCol, BOOL bAsc);
};
