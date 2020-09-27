//-----------------------------------------------------------------------------
// File: autorunDlg.cpp
//
// Desc: Implemention file
//
// Hist: 10.27.00 Emily Wang Created For XBOX
//       
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "autorun.h"
#include "autorunDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EXTRA_CXBORDER   10
#define EXTRA_CYBORDER   10

/////////////////////////////////////////////////////////////////////////////
// CAutorunDlg dialog

CAutorunDlg::CAutorunDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutorunDlg::IDD, pParent),
    m_nIndexHitLink(-1),
    m_hOldCursor(NULL)
{
	//{{AFX_DATA_INIT(CAutorunDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

void CAutorunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutorunDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutorunDlg, CDialog)
	//{{AFX_MSG_MAP(CAutorunDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutorunDlg message handlers

//-----------------------------------------------------------------------------
// Name: OnInitDialog()
// Desc: AutorunDialog initialization.
//-----------------------------------------------------------------------------
BOOL CAutorunDlg::OnInitDialog()
{
	
	CClientDC dc(this);
    RECT      rcBounding;
    SIZE      size;

	CDialog::OnInitDialog();

	PSAutorunInfo psInfo = ((CAutorunApp*)AfxGetApp())->GetInfo();
	ASSERT(psInfo != NULL);

    // Load icon 
    m_hIcon = (HICON)LoadImage(NULL, psInfo->szIconPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (!m_hIcon)
    {
        // If fail, we use the default icon in the resource
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog

	SetIcon(m_hIcon, FALSE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		// Set small icon

	// Load cursor 
	m_hCursor = (HCURSOR)LoadImage(NULL, psInfo->szCursorPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
   
	// Set dialog text
	SetWindowText(psInfo->pszTitle);

	// Create font
    LOGFONT logFont;
	
	//Sets buffers to a specified character
    memset(&logFont, 0, sizeof(logFont));
    strcpy(logFont.lfFaceName, psInfo->szFontName);
    logFont.lfCharSet = DEFAULT_CHARSET;
    logFont.lfHeight = -MulDiv(psInfo->nFontSize, GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);
    logFont.lfWeight = 700; // Bold
    m_font.CreateFontIndirect(&logFont);
	
    // Init bounding rect
    rcBounding.left   = rcBounding.top   = INT_MAX;
    rcBounding.bottom = rcBounding.right = INT_MIN;

	for (INT i = 0; i < psInfo->cnLinks; i++)
	{
		size = CalculateTextSize(&dc, psInfo->psLinks[i].pszName);

        psInfo->psLinks[i].rcRect.right  = psInfo->psLinks[i].rcRect.left + size.cx;
        psInfo->psLinks[i].rcRect.bottom = psInfo->psLinks[i].rcRect.top  + size.cy;
        
        // Get the maximun rectangle boundary.
        if (rcBounding.left > psInfo->psLinks[i].rcRect.left)
            rcBounding.left = psInfo->psLinks[i].rcRect.left;

        if (rcBounding.top > psInfo->psLinks[i].rcRect.top)
            rcBounding.top = psInfo->psLinks[i].rcRect.top;

        if (rcBounding.right < psInfo->psLinks[i].rcRect.right)
            rcBounding.right = psInfo->psLinks[i].rcRect.right;

        if (rcBounding.bottom < psInfo->psLinks[i].rcRect.bottom)
            rcBounding.bottom = psInfo->psLinks[i].rcRect.bottom;
	}

    //Set bmp file
	m_cImage.InitData(psInfo->szImagePath);

    // Calculate the size of window
    size = m_cImage.GetSize();
    if (!size.cx && !size.cy && psInfo->cnLinks)
    {
        size.cx = rcBounding.right  + EXTRA_CXBORDER;
        size.cy = rcBounding.bottom + EXTRA_CYBORDER;
    }

    size.cx += 2 * GetSystemMetrics(SM_CXBORDER);
    size.cy += 2 * GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYCAPTION);

    // Change the size of the window
    SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//-----------------------------------------------------------------------------
// Name: OnPaint()
// Desc: Draw the icon for minimize button to dialog.  
//-----------------------------------------------------------------------------

void CAutorunDlg::OnPaint() 
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
	    PSAutorunInfo psInfo = ((CAutorunApp*)AfxGetApp())->GetInfo();
	    ASSERT(psInfo != NULL);

		CPaintDC dc(this); // device context for painting

        m_cImage.DrawImage(&dc, 0, 0, FALSE);

        CFont* pOldFont = dc.SelectObject(&m_font);

        for (INT i = 0; i < psInfo->cnLinks; i++)
        {
            COLORREF crText = m_nIndexHitLink == i ? psInfo->crHighLight : psInfo->crFont;

            DrawLink(&dc, psInfo->psLinks[i].pszName, crText, psInfo->psLinks[i].rcRect);
        }

	    dc.SelectObject(pOldFont);

		CDialog::OnPaint();
	}
}

//-----------------------------------------------------------------------------
// Name: OnQueryDrayIcon()
// Desc: The system calls to obtain the cursor to display while the user drags
//		 the minimized window.
//-----------------------------------------------------------------------------
HCURSOR CAutorunDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//-----------------------------------------------------------------------------
// Name: OnLButtonDown()
// Desc: Performs the commands to display others when left button down.
//-----------------------------------------------------------------------------
void CAutorunDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
}

//-----------------------------------------------------------------------------
// Name: OnLButtonUP()
// Desc: Performs the commands to display others when left button up.
//-----------------------------------------------------------------------------
void CAutorunDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
    PSAutorunInfo psInfo = ((CAutorunApp*)AfxGetApp())->GetInfo();

    OnOK();
}

//-----------------------------------------------------------------------------
// Name: OnMouseMove()
// Desc: Performs change the text color between mouse on and off of the text.
//-----------------------------------------------------------------------------
void CAutorunDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	PSAutorunInfo psInfo = ((CAutorunApp*)AfxGetApp())->GetInfo();
    static BOOL   bPrevOver = FALSE;

	for (INT i = 0; i < psInfo->cnLinks; i++)
	{
		if (PtInRect(&psInfo->psLinks[i].rcRect, point))
		{
			break;
		}
	}

	if (i >= psInfo->cnLinks)
	{
		if (m_nIndexHitLink >= 0)
		{
			InvalidateRect(&psInfo->psLinks[m_nIndexHitLink].rcRect, TRUE);
		}
		m_nIndexHitLink = -1;
	}
	else 
	{
		if (m_nIndexHitLink < 0)
		{
			InvalidateRect(&psInfo->psLinks[i].rcRect, TRUE);
        }
        
        if (m_nIndexHitLink >= 0 && i != m_nIndexHitLink)
        {
			InvalidateRect(&psInfo->psLinks[m_nIndexHitLink].rcRect, TRUE);
        }
		m_nIndexHitLink = i;
	}

    if (m_nIndexHitLink >= 0 && m_hCursor)
    {
        m_hOldCursor = SetCursor(m_hCursor);
    } 
    else if (m_hOldCursor)
    {
        SetCursor(m_hOldCursor);
        m_hOldCursor = NULL;
    }
}

//-----------------------------------------------------------------------------
// Name: CalculateTextSize()
// Desc: Performs a calculation on each of the strings to determine
//       the text sizes.
//-----------------------------------------------------------------------------
SIZE CAutorunDlg::CalculateTextSize(CDC* pDC, LPCTSTR pszString)
{
	SIZE   size;
	CFont* pOldFont = pDC->SelectObject(&m_font);

	size = pDC->GetTextExtent(pszString, _tcslen(pszString));

	pDC->SelectObject(pOldFont);

	return size;
}

//-----------------------------------------------------------------------------
// Name: DrawLink()
// Desc: Draw the text on the image with transparent background color.
//-----------------------------------------------------------------------------
void CAutorunDlg::DrawLink(CDC* pDC, LPCTSTR pszText, COLORREF crText, RECT rcRect)
{

    INT nPrevBkMode = pDC->SetBkMode(TRANSPARENT);
    COLORREF crPrevColor = pDC->SetTextColor(crText);

    pDC->TextOut(rcRect.left, rcRect.top, pszText);

    pDC->SetTextColor(crPrevColor);
    pDC->SetBkMode(nPrevBkMode);
}


//-----------------------------------------------------------------------------
// Name: OnKeyDown()
// Desc: Function for Keyboard access
//-----------------------------------------------------------------------------
void CAutorunDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    PSAutorunInfo psInfo = ((CAutorunApp*)AfxGetApp())->GetInfo();
    INT           nPrevHit = m_nIndexHitLink;

    if (!psInfo->cnLinks)
    {
        return;
    }

    switch(nChar)
    {
    case VK_RETURN:
        OnOK();
        break;

    case VK_PRIOR:
    case VK_DOWN:
        m_nIndexHitLink++;
        if (m_nIndexHitLink < 0 || m_nIndexHitLink >= psInfo->cnLinks)
        {
            m_nIndexHitLink = 0;
        }
        break;

    case VK_NEXT:
    case VK_UP:
        m_nIndexHitLink--;
        if (m_nIndexHitLink < 0)
        {
            m_nIndexHitLink = psInfo->cnLinks - 1;
        }
        break;
    }

    if (nPrevHit != m_nIndexHitLink)
    {
        if (nPrevHit >= 0)
        {
			InvalidateRect(&psInfo->psLinks[nPrevHit].rcRect, TRUE);
        }
        if (m_nIndexHitLink >= 0)
        {
            InvalidateRect(&psInfo->psLinks[m_nIndexHitLink].rcRect, TRUE);
        }
    }
}


//-----------------------------------------------------------------------------
// Name: OnGetDlgCode()
// Desc: Release the key that locked by dlg
//-----------------------------------------------------------------------------
UINT CAutorunDlg::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}


//-----------------------------------------------------------------------------
// Name: OnOK()
// Desc: Function for Keyboard access
//-----------------------------------------------------------------------------
void CAutorunDlg::OnOK(void)
{
    PSAutorunInfo psInfo = ((CAutorunApp*)AfxGetApp())->GetInfo();

    if (m_nIndexHitLink >= 0)
    {
        // Do link actions
        if (!psInfo->psLinks[m_nIndexHitLink].pszCommand ||! psInfo->psLinks[m_nIndexHitLink].pszCommand[0])
        {
            EndDialog(IDOK);
        }
        else
        {
	        PROCESS_INFORMATION piCmd;
	        STARTUPINFO         si;

		    memset(&si, 0, sizeof(STARTUPINFO));
		    si.cb = sizeof(STARTUPINFO);

            CreateProcess(	NULL,			// pointer to name of executable module 
			      psInfo->psLinks[m_nIndexHitLink].pszCommand,		// pointer to command line string
			      NULL,				// pointer to process security attributes
			      NULL,				// pointer to thread security attributes
			      FALSE,				// handle inheritance flag 
			      DETACHED_PROCESS,	// creation flags 
			      NULL,				// pointer to new environment block
			      psInfo->szPath,	// pointer to current directory name
			      &si,				// pointer to STARTUPINFO
			      &piCmd);		
        }

        // Restore the highlight link
		InvalidateRect(&psInfo->psLinks[m_nIndexHitLink].rcRect, TRUE);
        m_nIndexHitLink = -1;
    }
}