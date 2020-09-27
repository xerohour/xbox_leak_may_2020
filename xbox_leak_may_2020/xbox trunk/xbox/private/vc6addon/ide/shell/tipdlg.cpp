// tipdlg.cpp : implementation file
//

#include "stdafx.h"

#include "shell.h"
#include "resource.h"
#include "tipdlg.h"
#include "totd.h"
#include "tipmgr.h"

#include "shell.hid" // Help IDs for tip control

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
static const char szGeneralKey[] = "General";
static const char szTotD[] = "Tip of the Day";

void CVshellPackage::OnHelpTip()
{
	TipOfTheDay(FALSE);
}

// Tip of the Day
BOOL TipOfTheDay( BOOL fStartup )
{
	if (fStartup)
	{
		// Don't show tip if we're run from automation (invisibly)
		//  or the user doesn't want tips
		if (theApp.m_bRunInvisibly || !GetRegInt(szGeneralKey, szTotD, 1)) 
			return FALSE;
	}
	CTipFileManager TipData;
	if (!TipData.isTipAvailable())
	{
		if (!fStartup)
			::MessageBeep(0);
		return FALSE;
	}
//ShowDlg:
	CTipDlg Tips(TipData);
	Tips.DoModal();
/*
	switch (Tips.DoModal())
	{
	case IDC_TIP_MORE:
		{
			CMoreTipsDlg2 MoreTips;
			MoreTips.DoModal();
		}
		break;

	case IDC_TIP_ADD:
		{
			CAddTipDlg AddTip;
			AddTip.DoModal();
		}
		goto ShowDlg;
		break;

	default:
		break;
	}
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void Draw3dFrame(CDC* pDC, const CRect& rect, BOOL bIn)
{
	CBrush brush;
	COLORREF crShadow, crHighlight;
	crShadow = ::GetSysColor(COLOR_BTNSHADOW);
	crHighlight = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	brush.CreateSolidBrush(bIn ? crShadow : crHighlight);
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(rect.left-1,  rect.top-1,    rect.Width()+3, 1, PATCOPY);
	pDC->PatBlt(rect.left,    rect.bottom,   rect.Width(),   1, PATCOPY);
	pDC->PatBlt(rect.left-1,  rect.top,      1,  rect.Height()+1, PATCOPY);
	pDC->PatBlt(rect.right,   rect.top+1,    1,  rect.Height(),   PATCOPY);

	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();

	brush.CreateSolidBrush(bIn ? crHighlight : crShadow);
	pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(rect.left,   rect.top,      rect.Width()+1, 1, PATCOPY);
	pDC->PatBlt(rect.left-1, rect.bottom+1, rect.Width()+2, 1, PATCOPY);
	pDC->PatBlt(rect.left,   rect.top,    1,  rect.Height(),   PATCOPY);
	pDC->PatBlt(rect.right+1,rect.top,    1,  rect.Height()+2, PATCOPY);

	pDC->SelectObject(pOldBrush);
}


/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog


CTipDlg::CTipDlg(CTipManager & ATipManager, CWnd* pParent /*=NULL*/ )
	: C3dDialog(CTipDlg::IDD, pParent), m_Tip(ATipManager)
{
	//{{AFX_DATA_INIT(CTipDlg)
	m_bShowTipsAtStartup = !!GetRegInt(szGeneralKey, szTotD, 1);
	//}}AFX_DATA_INIT
}


void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipDlg)
	DDX_Control(pDX, IDC_TIP_NEXT, m_BNext);
	DDX_Check(pDX, IDC_TIP_SHOW, m_bShowTipsAtStartup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTipDlg, C3dDialog)
	//{{AFX_MSG_MAP(CTipDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TIP_NEXT, OnTipNext)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTipDlg message handlers

BOOL CTipDlg::OnInitDialog() 
{
	C3dDialog::OnInitDialog();
	int xmargin, ymargin;
//	margin = MulDiv(5,GetSystemMetrics(SM_CXFRAME),2);

	CRect r;
	HWND hwnd;

 	// set m_rectFrame from pos of OK button 
	VERIFY(hwnd = ::GetDlgItem(m_hWnd,IDOK));
	::GetWindowRect(hwnd, r);
	::ScreenToClient(m_hWnd,&r.BottomRight());
	// inset m_rectFrame from dialog
	GetClientRect(&m_rectFrame);
	xmargin = m_rectFrame.right- r.right;
	ymargin = m_rectFrame.bottom - r.bottom;
	m_rectFrame.InflateRect(-xmargin, -ymargin);
	VERIFY(hwnd = ::GetDlgItem(m_hWnd,IDOK));
	::GetWindowRect(hwnd, r);
	::ScreenToClient(m_hWnd,&r.TopLeft());
	m_rectFrame.bottom = r.top - ymargin;

	m_TipCtrl.Create((LPCTSTR)0, (LPCTSTR)0, WS_CHILD | /*WS_BORDER |*/ WS_VISIBLE, 
		m_rectFrame, this, IDC_TIP_CTRL/*1*/ );

	// Set Context Help ID for this control.
	m_TipCtrl.SetWindowContextHelpId(HIDC_TIP_CTRL) ;

	if (m_Tip.Status() == Exhausted)
	{
		m_Tip.Reset();
		m_sTip.LoadString(IDS_LAST_TIP);
	}
	else
		m_Tip.Tip(m_sTip);
	m_TipCtrl.SetTip(m_sTip);
	return TRUE;
}

LRESULT CTipDlg::OnCommandHelp(WPARAM,LPARAM) { return TRUE; }

void CTipDlg::OnPaint() 
{
	CPaintDC dc(this);
/*
	// Draw 3d line above Close (OK) button
	int margin = MulDiv(5,GetSystemMetrics(SM_CXFRAME),2);
	int L, R, Y;

	CRect r;
	HWND hwnd;
	VERIFY(hwnd = ::GetDlgItem(m_hWnd,IDOK));
	::GetWindowRect(hwnd, r);
	::ScreenToClient(m_hWnd,&r.TopLeft());
	::ScreenToClient(m_hWnd,&r.BottomRight());

	L = r.left - margin/2;
	R = r.right + margin/2;
	Y = r.top - margin;

	CBrush brush;
	COLORREF crShadow, crHighlight;
	crShadow = ::GetSysColor(COLOR_BTNSHADOW);
	crHighlight = ::GetSysColor(COLOR_BTNHIGHLIGHT);

	brush.CreateSolidBrush(crHighlight);
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.PatBlt(L, Y, R-L, 1, PATCOPY);

	dc.SelectObject(pOldBrush);
	brush.DeleteObject();
	brush.CreateSolidBrush(crShadow);
	dc.SelectObject(&brush);
	dc.PatBlt(L, Y-1, R-L, 1, PATCOPY);

	dc.SelectObject(pOldBrush);
*/
}

void CTipDlg::OnTipNext() 
{
	switch (m_Tip.Next())
	{
	case Ok:
		m_Tip.Tip(m_sTip);
		m_TipCtrl.SetTip(m_sTip);
		break;

	case Exhausted:
		m_Tip.Reset();
		m_sTip.LoadString(IDS_LAST_TIP);
		m_TipCtrl.SetTip(m_sTip);
		break;

	case Invalid:
	default:
		::MessageBeep(0);
	}
}

/*
void CTipDlg::OnTipAdd() 
{
	EndDialog( IDC_TIP_ADD );
}

void CTipDlg::OnTipMore() 
{
	EndDialog( IDC_TIP_MORE );
}
*/

void CTipDlg::OnDestroy() 
{
	WriteRegInt( szGeneralKey, szTotD, m_bShowTipsAtStartup );
	C3dDialog::OnDestroy();
}
