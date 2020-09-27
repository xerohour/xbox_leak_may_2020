// AwusbIODlg.cpp : implementation file
//

#include "stdafx.h"
#include "AwusbIO.h"
#include "AwusbIODlg.h"

#include "AwusbAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CAwusbIODlg dialog

CAwusbIODlg::CAwusbIODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAwusbIODlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAwusbIODlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAwusbIODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwusbIODlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAwusbIODlg, CDialog)
	//{{AFX_MSG_MAP(CAwusbIODlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_DIR_0, OnDir0)
	ON_BN_CLICKED(IDC_DIR_1, OnDir1)
	ON_BN_CLICKED(IDC_DIR_2, OnDir2)
	ON_BN_CLICKED(IDC_DIR_3, OnDir3)
	ON_BN_CLICKED(IDC_DIR_4, OnDir4)
	ON_BN_CLICKED(IDC_DIR_5, OnDir5)
	ON_BN_CLICKED(IDC_DIR_6, OnDir6)
	ON_BN_CLICKED(IDC_DIR_7, OnDir7)
	ON_BN_CLICKED(IDC_DIR_8, OnDir8)
	ON_BN_CLICKED(IDC_DIR_9, OnDir9)
	ON_BN_CLICKED(IDC_DIR_10, OnDir10)
	ON_BN_CLICKED(IDC_DIR_11, OnDir11)
	ON_BN_CLICKED(IDC_DIR_12, OnDir12)
	ON_BN_CLICKED(IDC_DIR_13, OnDir13)
	ON_BN_CLICKED(IDC_DIR_14, OnDir14)
	ON_BN_CLICKED(IDC_DIR_15, OnDir15)
	ON_BN_CLICKED(IDC_VALUE_0, OnValue0)
	ON_BN_CLICKED(IDC_VALUE_1, OnValue1)
	ON_BN_CLICKED(IDC_VALUE_2, OnValue2)
	ON_BN_CLICKED(IDC_VALUE_3, OnValue3)
	ON_BN_CLICKED(IDC_VALUE_4, OnValue4)
	ON_BN_CLICKED(IDC_VALUE_5, OnValue5)
	ON_BN_CLICKED(IDC_VALUE_6, OnValue6)
	ON_BN_CLICKED(IDC_VALUE_7, OnValue7)
	ON_BN_CLICKED(IDC_VALUE_8, OnValue8)
	ON_BN_CLICKED(IDC_VALUE_9, OnValue9)
	ON_BN_CLICKED(IDC_VALUE_10, OnValue10)
	ON_BN_CLICKED(IDC_VALUE_11, OnValue11)
	ON_BN_CLICKED(IDC_VALUE_12, OnValue12)
	ON_BN_CLICKED(IDC_VALUE_13, OnValue13)
	ON_BN_CLICKED(IDC_VALUE_14, OnValue14)
	ON_BN_CLICKED(IDC_VALUE_15, OnValue15)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwusbIODlg message handlers

BOOL CAwusbIODlg::OnInitDialog()
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
	

	// Load DLL library
	LoadLibrary (AWUSB_DLL_NAME);

	// Initialize global variables
	Dir = 0;
	Data = 0;

	// Set default device number
	SetDlgItemText (IDC_DEV_NUM, "0");	// default to 0


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAwusbIODlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAwusbIODlg::OnPaint() 
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
HCURSOR CAwusbIODlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CAwusbIODlg::OnOpen() 
{
	DWORD	devnum;
	DWORD	ret;
	
	// Open USB device
	devnum = (DWORD) GetDlgItemInt (IDC_DEV_NUM, NULL, FALSE);
	ret = AwusbOpen (devnum);
	if (ret != AWUSB_OK)	// if error
	{
		Error ("%s", AwusbErrorMessage (ret));
		return;
	}
	else
		Debug ("Device #%ld : Open successfully.", devnum);

	// set the board to match with the current GUI
	ret = AwusbEnablePort ((BYTE*)(&Dir), 2);
	if (ret != AWUSB_OK)
		Error ("%s", AwusbErrorMessage (ret));

	ret = AwusbOutPort ((BYTE*)(&Data), 2);
	if (ret != AWUSB_OK)
		Error ("%s", AwusbErrorMessage (ret));

	// update GUI
	Update();
}


void CAwusbIODlg::OnClose() 
{
	DWORD	ret;

	// Close device
	ret = AwusbClose ();
	if (ret != AWUSB_OK)
		Error ("%s", AwusbErrorMessage (ret));
	else
		Debug ("Device Closed");
}


void CAwusbIODlg::Update() 
{
	int	IdcDir[] = { IDC_DIR_0, IDC_DIR_1, IDC_DIR_2, IDC_DIR_3, 
					 IDC_DIR_4, IDC_DIR_5, IDC_DIR_6, IDC_DIR_7, 
					 IDC_DIR_8, IDC_DIR_9, IDC_DIR_10, IDC_DIR_11, 
					 IDC_DIR_12, IDC_DIR_13, IDC_DIR_14, IDC_DIR_15, };
	int	IdcVal[] = { IDC_VALUE_0, IDC_VALUE_1, IDC_VALUE_2, IDC_VALUE_3, 
					 IDC_VALUE_4, IDC_VALUE_5, IDC_VALUE_6, IDC_VALUE_7, 
					 IDC_VALUE_8, IDC_VALUE_9, IDC_VALUE_10, IDC_VALUE_11, 
					 IDC_VALUE_12, IDC_VALUE_13, IDC_VALUE_14, IDC_VALUE_15, };
	int		i;
	DWORD	ret;

	// get current I/O port state
	ret = AwusbInPort ((BYTE*)(&Data), 2);
	if (ret != AWUSB_OK)
		Error ("%s", AwusbErrorMessage (ret));

	// update GUI
	for (i=0; i<16; i++)		// change button label according to the state
	{
		(Dir & (1<<i))  ? SetDlgItemText(IdcDir[i], "Out") : SetDlgItemText(IdcDir[i], "In");
		(Data & (1<<i)) ? SetDlgItemText(IdcVal[i], "High") : SetDlgItemText(IdcVal[i], "Low");
	}
}


void CAwusbIODlg::DoDir(int num) 
{
	// this will be called with the I/O number when user click on one of Directions buttons 

	DWORD	ret;

	if (Dir & (1<<num))			// invert the num'th bit of Dir register
		Dir &= (~(1<<num));
	else 
		Dir |= (1<<num);

	// Enable port
	ret = AwusbEnablePort ((BYTE*)(&Dir), 2);
	if (ret != AWUSB_OK)
		Error ("%s", AwusbErrorMessage (ret));

	Update();
}

void CAwusbIODlg::DoValue(int num) 
{
	// this will be called with the I/O number when user click on one of Value buttons 

	DWORD	ret;
	
	if (Data & (1<<num))			// invert the num'th bit of Data register
		Data &= (~(1<<num));
	else 
		Data |= (1<<num);

	// Output port
	ret = AwusbOutPort ((BYTE*)(&Data), 2);
	if (ret != AWUSB_OK)
		Error ("%s", AwusbErrorMessage (ret));
	
	Update();
}


void CAwusbIODlg::Debug (char *fmt, ...)
{
	char 	buf[1024];
	va_list	alist;

	va_start (alist, fmt);
	vsprintf(buf, fmt, alist);
	((CEdit*)GetDlgItem (IDC_DEBUG))->SetWindowText (buf);	// set test in the Debug message edit box
	va_end (alist);
}


void CAwusbIODlg::Error (char *fmt, ...)
{
	char 	buf[1024];
	va_list	alist;

	va_start (alist, fmt);
	vsprintf(buf, fmt, alist);
	Debug ("ERROR : %s", buf);	// call Debug() with "Error : " to indicate an error
	va_end (alist);
}


void CAwusbIODlg::OnDir0() 
{
	DoDir(0);	
}

void CAwusbIODlg::OnDir1() 
{
	DoDir(1);	
}

void CAwusbIODlg::OnDir2() 
{
	DoDir(2);	
}

void CAwusbIODlg::OnDir3() 
{
	DoDir(3);	
}

void CAwusbIODlg::OnDir4() 
{
	DoDir(4);	
}

void CAwusbIODlg::OnDir5() 
{
	DoDir(5);	
}

void CAwusbIODlg::OnDir6() 
{
	DoDir(6);	
}

void CAwusbIODlg::OnDir7() 
{
	DoDir(7);	
}

void CAwusbIODlg::OnDir8() 
{
	DoDir(8);	
}

void CAwusbIODlg::OnDir9() 
{
	DoDir(9);	
}

void CAwusbIODlg::OnDir10() 
{
	DoDir(10);	
}

void CAwusbIODlg::OnDir11() 
{
	DoDir(11);	
}

void CAwusbIODlg::OnDir12() 
{
	DoDir(12);	
}

void CAwusbIODlg::OnDir13() 
{
	DoDir(13);	
}

void CAwusbIODlg::OnDir14() 
{
	DoDir(14);	
}

void CAwusbIODlg::OnDir15() 
{
	DoDir(15);	
}



void CAwusbIODlg::OnValue0() 
{
	DoValue(0);
}

void CAwusbIODlg::OnValue1() 
{
	DoValue(1);	
}

void CAwusbIODlg::OnValue2() 
{
	DoValue(2);	
}

void CAwusbIODlg::OnValue3() 
{
	DoValue(3);	
}

void CAwusbIODlg::OnValue4() 
{
	DoValue(4);	
}

void CAwusbIODlg::OnValue5() 
{
	DoValue(5);	
}

void CAwusbIODlg::OnValue6() 
{
	DoValue(6);	
}

void CAwusbIODlg::OnValue7() 
{
	DoValue(7);	
}

void CAwusbIODlg::OnValue8() 
{
	DoValue(8);	
}

void CAwusbIODlg::OnValue9() 
{
	DoValue(9);	
}

void CAwusbIODlg::OnValue10() 
{
	DoValue(10);	
}

void CAwusbIODlg::OnValue11() 
{
	DoValue(11);	
}

void CAwusbIODlg::OnValue12() 
{
	DoValue(12);	
}

void CAwusbIODlg::OnValue13() 
{
	DoValue(13);	
}

void CAwusbIODlg::OnValue14() 
{
	DoValue(14);	
}

void CAwusbIODlg::OnValue15() 
{
	DoValue(15);	
}

