// HackAttackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HackAttack.h"
#include "HackAttackDlg.h"
#include "AttackConstants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern PATTACK_FUNCTION g_IPAttackFunctions[];
extern PATTACK_FUNCTION g_TCPAttackFunctions[];
extern PATTACK_FUNCTION g_UDPAttackFunctions[];
extern PATTACK_FUNCTION g_ESPAttackFunctions[];

#define MAX_MAC_LEN 12

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHackAttackDlg dialog

CHackAttackDlg::CHackAttackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHackAttackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHackAttackDlg)
	m_fTCP = FALSE;
	m_fIP = FALSE;
	m_fUDP = FALSE;
	m_stringResults = _T("");
	m_stringMacAddress = _T("");
	m_fESP = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHackAttackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHackAttackDlg)
	DDX_Control(pDX, IDC_ESP, m_checkESP);
	DDX_Control(pDX, IDC_CHECKUDP, m_checkUDP);
	DDX_Control(pDX, IDC_CHECKTCP, m_checkTCP);
	DDX_Control(pDX, IDC_CHECKIP, m_checkIP);
	DDX_Control(pDX, IDC_START, m_buttonStart);
	DDX_Check(pDX, IDC_CHECKTCP, m_fTCP);
	DDX_Check(pDX, IDC_CHECKIP, m_fIP);
	DDX_Check(pDX, IDC_CHECKUDP, m_fUDP);
	DDX_Text(pDX, IDC_RESULTS, m_stringResults);
	DDX_Text(pDX, IDC_MACADDRESS, m_stringMacAddress);
	DDV_MaxChars(pDX, m_stringMacAddress, 12);
	DDX_Check(pDX, IDC_ESP, m_fESP);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHackAttackDlg, CDialog)
	//{{AFX_MSG_MAP(CHackAttackDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHackAttackDlg message handlers

BOOL CHackAttackDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	
	m_pInterfaceList = NULL;
	m_pInterfaceList = new CInterfaceList;
	if(!m_pInterfaceList)
	{
		MessageBox("Couldn't create interface list!", "SPARTA Error", MB_ICONERROR | MB_OK);
		return TRUE;
	}

	m_pInterface = NULL;
	m_pInterface = new CInterface(m_pInterfaceList->GetNameAt(0));
	if(!m_pInterface)
	{
		MessageBox("Couldn't create interface!", "SPARTA Error", MB_ICONERROR | MB_OK);
		return TRUE;
	}

	m_fAttackRunning = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHackAttackDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHackAttackDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHackAttackDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHackAttackDlg::OnClose() 
{
	m_pInterfaceList ? delete m_pInterfaceList : 0;
	m_pInterface ? delete m_pInterface : 0;
	
	m_pInterfaceList = NULL;
	m_pInterface = NULL;

	CDialog::OnClose();
}

void CHackAttackDlg::OnStart() 
{
	UpdateData(TRUE);

	if(!m_fAttackRunning)
	{
		m_buttonStart.EnableWindow(FALSE);
		m_checkTCP.EnableWindow(FALSE);
		m_checkIP.EnableWindow(FALSE);
		m_checkUDP.EnableWindow(FALSE);
		m_checkESP.EnableWindow(FALSE);

		CreateThread(NULL, 0, SpartaThread, this, 0, NULL);

		m_fAttackRunning = TRUE;
	}	
}


DWORD WINAPI SpartaThread(LPVOID lpParameter)
{
	CHackAttackDlg *pMainDialog = (CHackAttackDlg *) lpParameter;
	CHAR szMacAddress[MAX_MAC_LEN + 1];
	DWORD dwAttackCounter = 0;
	DWORD nMacAddressLen = 0;

	szMacAddress[0] = 0;
	
#ifdef UNICODE
	LPWSTR szTempMacAddress = NULL;
	
	nMacAddressLen = wcslen(szTempMacAddress = pMainDialog->m_stringMacAddress.GetBuffer(0));
	if(nMacAddressLen == MAX_MAC_LEN)
		wcstombs(szMacAddress, szTempMacAddress, MAX_MAC_LEN);
	else
		goto Exit;
#else
	LPSTR szTempMacAddress = NULL;
	
	nMacAddressLen = strlen(szTempMacAddress = pMainDialog->m_stringMacAddress.GetBuffer(0));
	if(nMacAddressLen == MAX_MAC_LEN)
		strncpy(szMacAddress, szTempMacAddress, MAX_MAC_LEN);
	else
		goto Exit;
#endif

	szMacAddress[MAX_MAC_LEN] = 0;

	if(pMainDialog->m_fTCP)
	{
		for(dwAttackCounter = 0; dwAttackCounter < TCPATTACK_MAXTEST; ++dwAttackCounter)
		{
			g_TCPAttackFunctions[dwAttackCounter](pMainDialog->m_pInterface, szMacAddress);
		}
	}
	
	if(pMainDialog->m_fUDP)
	{
		for(dwAttackCounter = 0; dwAttackCounter < UDPATTACK_MAXTEST; ++dwAttackCounter)
		{
			g_UDPAttackFunctions[dwAttackCounter](pMainDialog->m_pInterface, szMacAddress);
		}
	}
	
	if(pMainDialog->m_fIP)
	{
		for(dwAttackCounter = 0; dwAttackCounter < IPATTACK_MAXTEST; ++dwAttackCounter)
		{
			g_IPAttackFunctions[dwAttackCounter](pMainDialog->m_pInterface, szMacAddress);
		}
	}

	if(pMainDialog->m_fESP)
	{
		for(dwAttackCounter = 0; dwAttackCounter < ESPATTACK_MAXTEST; ++dwAttackCounter)
		{
			g_ESPAttackFunctions[dwAttackCounter](pMainDialog->m_pInterface, szMacAddress);
		}
	}

Exit:

	pMainDialog->m_fAttackRunning = FALSE;

	pMainDialog->m_buttonStart.EnableWindow(TRUE);
	pMainDialog->m_checkTCP.EnableWindow(TRUE);
	pMainDialog->m_checkIP.EnableWindow(TRUE);
	pMainDialog->m_checkUDP.EnableWindow(TRUE);
	pMainDialog->m_checkESP.EnableWindow(TRUE);

	return 0;
}


