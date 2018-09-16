
// RemoteControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteControl.h"
#include "RemoteControlDlg.h"
#include "afxdialogex.h"
#include "windows.h"

#include "KeyBoardControl.h"
#include "MouseControl.h"
#include "ScreenShot.h"
#include "ProcessManager.h"
#include "ServerManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteControlDlg dialog



CRemoteControlDlg::CRemoteControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_REMOTECONTROL_DIALOG, pParent)
	, log(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, log);
	DDX_Control(pDX, IDC_EDIT1, logBox);
}

BEGIN_MESSAGE_MAP(CRemoteControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteControlDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRemoteControlDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, &CRemoteControlDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CRemoteControlDlg message handlers

BOOL CRemoteControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoteControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRemoteControlDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	cTh = AfxBeginThread(StaticThreadFunc, this);

	//cTh->m_bAutoDelete = FALSE;
	m_Thread_handle = cTh->m_hThread;
}

UINT __cdecl CRemoteControlDlg::StaticThreadFunc(LPVOID pParam)
{
	CRemoteControlDlg *pYourClass = reinterpret_cast<CRemoteControlDlg*>(pParam);
	UINT retCode = pYourClass->ThreadFunc();

	return retCode;
}

UINT CRemoteControlDlg::ThreadFunc()
{
	// Do your thing, this thread now has access to all the classes member variables
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);

	int iPort = 3000;
	if (iPort == 0)
	{
		return -1;
	}
	m_pServer = new ServerManager(this);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
	m_pServer->StartListening(iPort);

	return 0;
}

void CRemoteControlDlg::AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText)
{
	// get the initial text length
	int nLength = edit.GetWindowTextLength();
	// put the selection at the end of text
	edit.SetSel(nLength, nLength);
	// replace the selection
	edit.ReplaceSel(pszText);
}

void CRemoteControlDlg::AddInfo(CString str) {
	AppendTextToEditCtrl(logBox, str);
}

void CRemoteControlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
	delete m_pServer;
	WaitForSingleObject(m_Thread_handle, INFINITE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
}

void CRemoteControlDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
