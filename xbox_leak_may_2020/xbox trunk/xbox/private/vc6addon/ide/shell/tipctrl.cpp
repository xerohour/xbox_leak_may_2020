// TipCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "tipctrl.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipCtrl

CTipCtrl::CTipCtrl()
: m_rectText(0,0,0,0)
{
	m_margin = MulDiv(3,GetSystemMetrics(SM_CXFRAME),2);
	VERIFY(m_sDidya.LoadString(IDS_DIDYA));
	VERIFY(m_bitmap.LoadBitmap(IDB_TIPOFTHEDAY));
}

CTipCtrl::~CTipCtrl()
{
}

BEGIN_MESSAGE_MAP(CTipCtrl, CWnd)
	//{{AFX_MSG_MAP(CTipCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTipCtrl message handlers
void CTipCtrl::CalcTextRect()
{
	if (m_rectText.IsRectNull())
	{
		GetClientRect(m_rectText);
		m_rectText.InflateRect(-2*m_margin,-2*m_margin);
		BITMAP bitmap;
		m_bitmap.GetObject(sizeof (BITMAP), &bitmap);
		m_rectText.top += m_margin + 43;
		m_rectText.left += bitmap.bmWidth + 3*m_margin;
	}
}

int CharSet()
{
	switch (theApp.GetCP())
	{
	case OEM_JAPAN:
		return SHIFTJIS_CHARSET;
	case OEM_PRC:
		return GB2312_CHARSET;
	case OEM_TAIWAN:
		return CHINESEBIG5_CHARSET;
	case OEM_KOR_WANGSUNG:
	case OEM_KOR_JOHAB:
		return DEFAULT_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

LPCTSTR SerifFace()
{
	switch (theApp.GetCP())
	{
	case OEM_JAPAN:
		return _TEXT("‚l‚r ‚o–¾’©"); // MS Proportional (roman?)
	case OEM_PRC:
		return 0;
	case OEM_TAIWAN:
		return 0;
	case OEM_KOR_WANGSUNG:
	case OEM_KOR_JOHAB:
		return 0;
	default:
		return _TEXT("Times New Roman");
	}
}

LPCTSTR SansFace()
{
	switch (theApp.GetCP())
	{
	case OEM_JAPAN:
		return _TEXT("‚l‚r ‚oƒSƒVƒbƒN"); // "MS Proportional Gothic"
	case OEM_PRC:
	case OEM_TAIWAN:
	case OEM_KOR_WANGSUNG:
	case OEM_KOR_JOHAB:
		return 0;
	default:
		return _TEXT("Arial");
	}
}

void CTipCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
	CBrush brWindow(::GetSysColor(COLOR_WINDOW));
	CRect rectFrame;
	GetClientRect(rectFrame);
	dc.Draw3dRect(&rectFrame, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));
	rectFrame.InflateRect(-1,-1);

	CalcTextRect();
	COLORREF coPanel = ::GetSysColor(COLOR_APPWORKSPACE);
	if (coPanel == ::GetSysColor(COLOR_WINDOW))
		coPanel = ::GetSysColor(COLOR_ACTIVECAPTION);
	if (coPanel == ::GetSysColor(COLOR_WINDOW))
		coPanel = RGB(128,128,255);
	CBrush brPanel(coPanel);
	CBrush * pbrushOld = dc.SelectObject(&brPanel);
	dc.PatBlt( m_rectText.left - m_margin, m_rectText.top - 2*m_margin,
		m_rectText.Width() + 3*m_margin, 1, PATCOPY );

	BITMAP bitmap;
	m_bitmap.GetObject(sizeof (BITMAP), &bitmap);
	{
		CRect r(rectFrame.left, rectFrame.top,
				rectFrame.left + bitmap.bmWidth + 4*m_margin, rectFrame.bottom
			);
		dc.FillRect(r,&brPanel);

		int x = rectFrame.left + 2*m_margin;
		int y = rectFrame.top + 2*m_margin;
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap* pOldBitmap = memDC.SelectObject(&m_bitmap);
		dc.BitBlt(x, y, bitmap.bmWidth, bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBitmap);
		dc.ExtFloodFill( x, y, dc.GetPixel(x,y), FLOODFILLSURFACE );
	}

	dc.SelectObject(&brWindow);

	CRect rectDidya(
		rectFrame.left + 5*m_margin + bitmap.bmWidth,
		rectFrame.top  + 3*m_margin,
		rectFrame.left + 5*m_margin + bitmap.bmWidth + 100,
		rectFrame.top  + 4*m_margin + 36
		);

	CFont * pfontOld;
	CFont Font;
	int nFontHeight = MulDiv(14, dc.GetDeviceCaps(LOGPIXELSY), 72);
	Font.CreateFont(-nFontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		(BYTE)CharSet(), OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH | FF_ROMAN, SerifFace());
	pfontOld = dc.SelectObject(&Font);

	dc.DrawText( m_sDidya, -1, rectDidya, DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE );
	dc.SelectObject(pfontOld);

	Font.DeleteObject();
	nFontHeight = MulDiv(10, dc.GetDeviceCaps(LOGPIXELSY), 72);
	Font.CreateFont(-nFontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		(BYTE)CharSet(), OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS | TMPF_TRUETYPE, SansFace());
	pfontOld = dc.SelectObject(&Font);

	dc.DrawText( m_sTip, -1, m_rectText, DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK );
	dc.SelectObject(pfontOld);

	dc.SelectObject(pbrushOld);
}

void CTipCtrl::SetTip( const CString & sTip )
{
	m_sTip = sTip;
	CalcTextRect();
	InvalidateRect(m_rectText,TRUE);
}


BOOL CTipCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CBrush brush(::GetSysColor(COLOR_WINDOW));
	CRect r;
	GetClientRect(r);
	pDC->FillRect(r, &brush);
	return TRUE;
}


