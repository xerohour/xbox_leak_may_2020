// aboutdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "aboutdlg.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

// About Box : Product information
// NOTE: Do not change PRODINFO structure without consulting w/Setup owner;
// Setup directly modifies 'licenceName' and 'licenceCompany' and expects them
// to be of fixed length (Cuda 2414: a-jonj, 2-26-93)
struct PRODINFO
{
	CHAR licenseName[30];
	CHAR licenseCompany[30];
	CHAR serialNumber[20];
};

//
// About Box : Product information
// Setup searches for this information in the exe and overwrites
// it with the real user's name and company.
//
struct PRODINFO productInfo =
{
#ifdef _DEBUG
	/* szVerUser,*/
	"Microsoft Team C++",
#else
	"This is an unauthorized copy",	// Registered user name
	"of Microsoft Spy++",			// Company name
#endif
	"33148253781"					// Serial # (not displayed)
};


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
	CHAR szTmp[80];

	CDialog::OnInitDialog();

	//
	// Display the version string
	//
#ifndef SHIP
	wsprintf(szTmp, "%d.%2.2d.%4.4d", rmj, rmm, rup);
#else	// SHIP
	wsprintf(szTmp, "%d.%2.2d", rmj, rmm);
#endif	// SHIP
	SetDlgItemText(IDC_ABOUT_VERSION, szTmp);

	//Display license information
	SetDlgItemText(IDC_ABOUT_LICENSENAME, productInfo.licenseName);
	SetDlgItemText(IDC_ABOUT_LICENSECOMPANY, productInfo.licenseCompany);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAboutDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// We should only be getting WM_DRAWITEM messages from the
	// icon control (an ownerdrawn button)!
	ASSERT(nIDCtl == IDC_ABOUT_ICON);

	CBitmap bmLarge;
	bmLarge.LoadBitmap(IDB_LARGEABOUT);

	// dolphin 14787 [patbr]
	// on Daytona-J bitmap does not cover control area, so paint control background first...
	HBRUSH hbrGray = CreateSolidBrush(RGB(192, 192, 192));
	::FillRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem), hbrGray);
	DeleteObject(hbrGray);

	// Draw the bitmap...
	BITMAP bitmap;
	bmLarge.GetObject(sizeof (BITMAP), &bitmap);
	{
		CDC memDC;
		memDC.CreateCompatibleDC((CDC *)CDC::FromHandle(lpDrawItemStruct->hDC));
		CBitmap* pOldBitmap = memDC.SelectObject(&bmLarge);
		((CDC *)CDC::FromHandle(lpDrawItemStruct->hDC))->BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBitmap);
	}
}

void CAboutDlg::OnOK()
{
	if ((GetKeyState(VK_CONTROL) >= 0) && (GetKeyState(VK_SHIFT) >= 0))
		CDialog::OnOK();
	else
	{
		if (GetDlgItem(IDC_ABOUT_MSDEV)->IsWindowVisible())
			GetDlgItem(IDC_ABOUT_MSDEV)->ShowWindow(FALSE);
		else
			GetDlgItem(IDC_ABOUT_MSDEV)->ShowWindow(TRUE);
	}
}
