// DMCoreTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DMCoreTest.h"
#include "DMCoreTestDlg.h"
#include "MXF.h"
#include "BasicMXF.h"
#include "AllocatorMXF.h"
#include "SplitterMXF.h"
#include "SequencerMXF.h"

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
// CDMCoreTestDlg dialog

CDMCoreTestDlg::CDMCoreTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDMCoreTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDMCoreTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDMCoreTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDMCoreTestDlg)
	DDX_Control(pDX, IDC_COMBO_MXF_TYPE, m_MXF_COMBO);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDMCoreTestDlg, CDialog)
	//{{AFX_MSG_MAP(CDMCoreTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, OnButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_UNPACK, OnButtonUnpack)
	ON_BN_CLICKED(IDC_BUTTON_PACK, OnButtonPack)
	ON_BN_CLICKED(IDC_BUTTON1, OnSequence)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDMCoreTestDlg message handlers

BOOL CDMCoreTestDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDMCoreTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDMCoreTestDlg::OnPaint() 
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
HCURSOR CDMCoreTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDMCoreTestDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

/*
typedef struct 
{
    ULONG cbEvent;
    ULONG ulChannelGroup;
    ULONGLONG ullPresTime100Ns;
    SINGLE_LIST_ENTRY sleEventList;
    union
    {
        PBYTE pbData;
        BYTE abData[sizeof(PBYTE)];
    } uActionData;
} DMUS_KERNEL_EVENT, *PDMUS_KERNEL_EVENT;
*/

void CDMCoreTestDlg::OnButtonConnect() 
{
    CBasicMXF     *aSourceMXF,*aSource2MXF,*aSinkMXF;
    CSplitterMXF  *aSplitterMXF;
    CAllocatorMXF *allocatorMXF;
    PDMUS_KERNEL_EVENT  pDMKEvt;
    short count;

    for (ULONG count2 = 0;count2 < 10000; count2++)
    {
        allocatorMXF    = new CAllocatorMXF;
        aSourceMXF      = new CBasicMXF(allocatorMXF);
        aSource2MXF     = new CBasicMXF(allocatorMXF);
        aSinkMXF        = new CBasicMXF(allocatorMXF);

        aSplitterMXF    = new CSplitterMXF(allocatorMXF);

        aSourceMXF->ConnectOutput(aSplitterMXF);    //  tell src where to put
        aSplitterMXF->ConnectOutput(aSource2MXF);
        aSplitterMXF->ConnectOutput(aSinkMXF);
        aSource2MXF->ConnectOutput(aSinkMXF);   //  tell src2 where to put
        aSinkMXF->ConnectOutput(allocatorMXF);  //  tell snk where to put

        allocatorMXF->GetMessage(&pDMKEvt);
        pDMKEvt->cbEvent = 1;
        pDMKEvt->uData.abData[0] = (BYTE)count;

        aSourceMXF->PutMessage(pDMKEvt);

//      src
//       |
//     split
//       |  \
//       |   src2
//       |  /
//      sink
//       |
//  (allocator)

        aSplitterMXF->DisconnectOutput(aSource2MXF);
        aSplitterMXF->DisconnectOutput(aSinkMXF);
        aSplitterMXF->ConnectOutput(aSourceMXF);

        aSourceMXF->DisconnectOutput(aSplitterMXF);
        aSourceMXF->ConnectOutput(aSinkMXF);

//     split
//    /  |  \
// src   |   src2
//    \  |  /
//      sink
//       |
//  (allocator)

        allocatorMXF->GetMessage(&pDMKEvt);
        pDMKEvt->cbEvent = 4;
        pDMKEvt->pNextEvt = NULL;
        *(PULONG(pDMKEvt->uData.abData)) = 0xbeefbabe;

        aSplitterMXF->PutMessage(pDMKEvt);

    
        delete aSourceMXF;
        delete aSource2MXF;
        delete aSinkMXF;
        delete aSplitterMXF;
        delete allocatorMXF;
    }
}

void CDMCoreTestDlg::OnButtonProcess() 
{
    CBasicMXF     *aSourceMXF,*aSource2MXF,*aSinkMXF;
    CSplitterMXF  *aSplitterMXF;
    CAllocatorMXF *allocatorMXF;
    short count;
#define kNumMessages    3
    PDMUS_KERNEL_EVENT  pDMKEvt[kNumMessages];
    
    for (ULONG count2 = 0;count2 < 5000; count2++)
    {
        allocatorMXF    = new CAllocatorMXF;
        aSourceMXF      = new CBasicMXF(allocatorMXF);
        aSource2MXF     = new CBasicMXF(allocatorMXF);
        aSinkMXF        = new CBasicMXF(allocatorMXF);

        aSplitterMXF    = new CSplitterMXF(allocatorMXF);

        aSourceMXF->ConnectOutput(aSplitterMXF);    //  tell src where to put
        aSplitterMXF->ConnectOutput(aSource2MXF);
        aSplitterMXF->ConnectOutput(aSinkMXF);
        aSource2MXF->ConnectOutput(aSinkMXF);   //  tell src2 where to put
        aSinkMXF->ConnectOutput(allocatorMXF);  //  tell snk where to put

        allocatorMXF->GetMessage(&(pDMKEvt[0]));
        pDMKEvt[0]->uData.pbData = (unsigned char *)malloc(13);
        for (count = 0;count < 13;count++)
        {
            pDMKEvt[0]->cbEvent = 13;
            pDMKEvt[0]->uData.pbData[count] = (unsigned char)count;
        }
        for (count = 1; count < kNumMessages; count++)
        {
            allocatorMXF->GetMessage(&(pDMKEvt[count]));
            pDMKEvt[count]->cbEvent = 1;
            pDMKEvt[count]->uData.abData[0] = (BYTE)count;
        }
        pDMKEvt[1]->pNextEvt = pDMKEvt[0];

        aSourceMXF->PutMessage(pDMKEvt[1]);
        aSource2MXF->PutMessage(pDMKEvt[2]);

//      src
//       |
//     split
//       |  \
//       |   src2
//       |  /
//      sink
//       |
//  (allocator)

        aSplitterMXF->DisconnectOutput(aSource2MXF);
        aSplitterMXF->DisconnectOutput(aSinkMXF);
        aSplitterMXF->ConnectOutput(aSourceMXF);

        aSourceMXF->DisconnectOutput(aSplitterMXF);
        aSourceMXF->ConnectOutput(aSinkMXF);

//     split
//    /  |  \
// src   |   src2
//    \  |  /
//      sink
//       |
//  (allocator)

        allocatorMXF->GetMessage(&(pDMKEvt[0]));
        allocatorMXF->GetMessage(&(pDMKEvt[1]));

        pDMKEvt[0]->cbEvent = 1;
        pDMKEvt[1]->cbEvent = 4;

        pDMKEvt[0]->ulChannelGroup = 0x01234567;
        pDMKEvt[1]->ulChannelGroup = 0x89abcdef;
    
        pDMKEvt[0]->ullPresTime100Ns = (0xfedcba9876543210);
        pDMKEvt[1]->ullPresTime100Ns = (0x0123456789abcdef);
    
        pDMKEvt[0]->pNextEvt = NULL;
        pDMKEvt[1]->pNextEvt = pDMKEvt[0];
    
        pDMKEvt[0]->uData.abData[0] = 0xa5;
        *(PULONG(pDMKEvt[1]->uData.abData)) = 0xbeefbabe;

        aSplitterMXF->PutMessage(pDMKEvt[1]);

        delete aSourceMXF;
        delete aSource2MXF;
        delete aSinkMXF;
        delete aSplitterMXF;
        delete allocatorMXF;
    }
}

extern void TestUnpacker();
void CDMCoreTestDlg::OnButtonUnpack() 
{
	TestUnpacker();	
}

extern void TestPacker();
void CDMCoreTestDlg::OnButtonPack() 
{
	TestPacker();	
}

//extern void TestSequencer();
void CDMCoreTestDlg::OnSequence() 
{
//    TestSequencer();	
    CSequencerMXF *aSequencerMXF;
    CAllocatorMXF *allocatorMXF;

    short count;
#define kNumSeqMessages    5
    PDMUS_KERNEL_EVENT  pDMKEvt[kNumSeqMessages];
    
    allocatorMXF  = new CAllocatorMXF;
    aSequencerMXF = new CSequencerMXF(allocatorMXF);
    //  don't ConnectOutput -- Sequencer can feed to allocator

    short timeArray[5] = {1,0,0,3,2};

    for (count = 0; count < kNumSeqMessages; count++)
    {
        allocatorMXF->GetMessage(&(pDMKEvt[count]));
        pDMKEvt[count]->ullPresTime100Ns = (ULONGLONG)timeArray[count];
        aSequencerMXF->PutMessage(pDMKEvt[count]);
    }
    (void) aSequencerMXF->DisconnectOutput(allocatorMXF);

    delete aSequencerMXF;
    delete allocatorMXF;
}
