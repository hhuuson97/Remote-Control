
// RemoteControlDlg.h : header file
//

#pragma once
#include "ServerManager.h"
#include "afxwin.h"

// CRemoteControlDlg dialog
class CRemoteControlDlg : public CDialog
{
// Construction
public:
	CRemoteControlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	SOCKET sServer;
	SOCKET sClient;
	bool onBind = false;
	sockaddr_in servAdd;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_Thread_handle;
	CWinThread *cTh;
	ServerManager *m_pServer;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	static UINT __cdecl StaticThreadFunc(LPVOID pParam);
	UINT ThreadFunc();
	afx_msg void OnEnChangeEdit1();
	void AddInfo(CString str);
	void AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText);
	CString log;
	CEdit logBox;
};
