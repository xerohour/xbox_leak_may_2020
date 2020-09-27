/////////////////////////////////////////////////////////////////////////////
// About Box and Splash Screen
//

#include "stdafx.h"
#include <dos.h>
#include <direct.h>
#include "about.h"
#include "main.h"
#include "version.h"
#include "resource.h"
// #include "vinfoapi.h" --- BUILD LAB UNCOMMENT!
#include "dlgbase.h"
#include "util.h"
// #include "intnl.h" --- BUILD LAB UNCOMMENT!
#include "tchar.h"

#include "shell.h"	// willf: 11/21/96 MyPlaySound() moved to util.cpp

//[multimon] 7 mar 97 - der
#include "mmonitor.h"

#define HIGHLIGHT 0

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// NOTE: This should NOT be localized.
// It will be overwritten by information from a PID resource written by SETUP
PRODUCT_INFO AFX_EXT_DATADEF productInfo =
{
#ifdef _DEBUG
	szVerUser,
	"Team VC++",
#else
	"This is an unauthorized copy",
	"of Microsoft Developer",
#endif
	"xxxxxxxxxxxxxxxxxxxxxxx",
};

BOOL InitProductId( HMODULE hMod )
{
	CProductInfo pid;
	pid.Create(hMod);
	if (pid.IsValid())
		pid.CopyInfo(productInfo.szSerialNumber, productInfo.szLicenseName, productInfo.szLicenseCompany);
	return pid.IsValid();
}

// CWnd for splash screen
static class CSplashScreen* pSplash = NULL;

/////////////////////////////////////////////////////////////////////////////
// Generic Bitmap Drawing Window Class
//

class CBitmapWnd : public CWnd
{
public:
	CBitmapWnd();
	~CBitmapWnd();
	BOOL Create(LPCSTR lpszBitmapName, DWORD dwStyle,
		const RECT& rect = CFrameWnd::rectDefault,
		CWnd* pParentWnd = NULL, UINT nID = 0);

	BOOL Create(UINT nBitmapID, DWORD dwStyle,
		const RECT& rect = CFrameWnd::rectDefault,
		CWnd* pParentWnd = NULL, UINT nID = 0)
			{ return Create(MAKEINTRESOURCE(nBitmapID), dwStyle,
				rect, pParentWnd, nID); }

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);

	virtual void OnDraw(CDC* pDC);
	
	LPSTR m_lpDIB;
	CPalette* m_pPalette;

	DECLARE_MESSAGE_MAP()
};

CBitmapWnd::CBitmapWnd()
{
	m_lpDIB = NULL;
	m_pPalette = NULL;
}

CBitmapWnd::~CBitmapWnd()
{
	if (m_pPalette != NULL)
	{
		delete m_pPalette;
		m_pPalette = NULL;
		ModifyStyle(WS_POPUP, 0);
		SetOwner(NULL);
		SetActiveWindow();
	}
	if (m_hWnd != NULL)
		DestroyWindow();	// Avoid warning in CWnd destructor.
}

BEGIN_MESSAGE_MAP(CBitmapWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
END_MESSAGE_MAP()


#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof (BITMAPINFOHEADER))

WORD DIBNumColors(LPSTR lpbi)
{
    WORD wBitCount;

    // If this is a Windows style DIB, the number of colors in the
    // color table can be less than the number of bits per pixel
    // allows for (i.e. lpbi->biClrUsed can be set to some value).
    // If this is the case, return the appropriate value.

    if (IS_WIN30_DIB(lpbi))
    {
        DWORD dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;

        if (dwClrUsed != 0)
            return (WORD)dwClrUsed;
    }


    // Calculate the number of colors in the color table based on
    //  the number of bits per pixel for the DIB.

    if (IS_WIN30_DIB(lpbi))
        wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
    else
        wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

    switch (wBitCount)
    {
    case 1:
        return 2;

    case 4:
        return 16;

    case 8:
        return 256;

    default:
        return 0;
    }
}

WORD PaletteSize(LPSTR lpbi)
{
    return DIBNumColors(lpbi) *
        (IS_WIN30_DIB(lpbi) ? sizeof(RGBQUAD) : sizeof(RGBTRIPLE));
}

LPSTR FindDIBBits(LPSTR lpbi)
{
    return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

DWORD DIBWidth(LPSTR lpDIB)
{
    LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)lpDIB;
    LPBITMAPCOREHEADER lpbmc = (LPBITMAPCOREHEADER)lpDIB;

    if (lpbmi->biSize == sizeof (BITMAPINFOHEADER))
        return lpbmi->biWidth;
    else
        return (DWORD)lpbmc->bcWidth;
}


DWORD DIBHeight(LPSTR lpDIB)
{
    LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)lpDIB;
    LPBITMAPCOREHEADER lpbmc = (LPBITMAPCOREHEADER)lpDIB;

    if (lpbmi->biSize == sizeof (BITMAPINFOHEADER))
        return lpbmi->biHeight;
    else
        return (DWORD)lpbmc->bcHeight;
}

CPalette* CreateDIBPalette(LPSTR lpbi)
{
    LPLOGPALETTE lpPal;
    HGLOBAL hLogPal;
    CPalette* pPal = NULL;
    int i;
	WORD wNumColors;
    LPBITMAPINFO lpbmi;
    LPBITMAPCOREINFO lpbmc;
    BOOL bWinStyleDIB;

    ASSERT(lpbi != NULL);

    lpbmi = (LPBITMAPINFO)lpbi;
    lpbmc = (LPBITMAPCOREINFO)lpbi;
    wNumColors = DIBNumColors(lpbi);
    bWinStyleDIB = IS_WIN30_DIB(lpbi);

    if (wNumColors == 0)
        return NULL;

    hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) +
        sizeof (PALETTEENTRY) * wNumColors);

    if (hLogPal == NULL)
        return NULL;

    lpPal = (LPLOGPALETTE)GlobalLock(hLogPal);

    lpPal->palVersion = 0x300;
    lpPal->palNumEntries = wNumColors;

    for (i = 0; i < wNumColors; i += 1)
    {
        if (bWinStyleDIB)
        {
            lpPal->palPalEntry[i].peRed  = lpbmi->bmiColors[i].rgbRed;
            lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
            lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
            lpPal->palPalEntry[i].peFlags = 0;
        }
        else
        {
            lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
            lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
            lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
            lpPal->palPalEntry[i].peFlags = 0;
        }
    }

    pPal = new CPalette;
    pPal->CreatePalette(lpPal);

    GlobalUnlock(hLogPal);
    GlobalFree(hLogPal);
	
    return pPal;
}

LPSTR LoadDIBResource(LPCSTR lpszBitmapName)
{
	HINSTANCE hDibInst = AfxFindResourceHandle(lpszBitmapName, RT_BITMAP);
	if (hDibInst == NULL)
		return NULL;
	HRSRC hDibRsrc = FindResource(hDibInst, lpszBitmapName, RT_BITMAP);
	if (hDibRsrc == NULL)
		return NULL;
	return (LPSTR)LockResource(LoadResource(hDibInst, hDibRsrc));
}

LPSTR LoadDIBResource(UINT nBitmapID)
{
	return LoadDIBResource(MAKEINTRESOURCE(nBitmapID)); 
}

BOOL CBitmapWnd::Create(LPCSTR lpszBitmapName, DWORD dwStyle,
	const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	int dcRaster = 0;

	{
		CClientDC dc(NULL);
		dcRaster = dc.GetDeviceCaps(RASTERCAPS);
	}

	BOOL bUsePalette = (dcRaster & RC_PALETTE) != 0;

	m_lpDIB = LoadDIBResource(lpszBitmapName);
	if (m_lpDIB == NULL)
		return FALSE;
	
	if (bUsePalette)
	{
		m_pPalette = CreateDIBPalette(m_lpDIB);
		if (m_pPalette == NULL)
			return FALSE;
	}

	int x = rect.left;
	int y = rect.top;
	int cxWidth = rect.right - rect.left;
	int cyHeight = rect.bottom - rect.top;
	if (cxWidth == CW_USEDEFAULT)
	{
		x = 0;
		y = 0;
		cxWidth = DIBWidth(m_lpDIB);
		cyHeight = DIBHeight(m_lpDIB);
	}

	if (rect.left == CW_USEDEFAULT)
	{
		// [multimon] 7 mar 97 - der - this is the splash screen
		//x += GetSystemMetrics(SM_CXSCREEN) / 2 - cxWidth / 2;
		//y += GetSystemMetrics(SM_CYSCREEN) / 2 - cyHeight / 2;		

		// Center on the current monitor.
		CRect rect(x,y,x+cxWidth, y+cyHeight) ;
		CenterRectToMonitor(pParentWnd->GetSafeHwnd(), &rect, TRUE) ;
		x = rect.left ;
		y = rect.top;
	}

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, dwStyle, x, y, cxWidth, cyHeight, pParentWnd->GetSafeHwnd(), NULL);
}

void CBitmapWnd::OnPaletteChanged(CWnd* pFocusWnd)
{
	if (pFocusWnd == this)
		return;
	
	OnQueryNewPalette();
}

BOOL CBitmapWnd::OnQueryNewPalette()
{
	if (m_pPalette == NULL)
		return FALSE;
	
	// BLOCK
	{
		CClientDC dc(this);
		CPalette* pOldPalette = dc.SelectPalette(m_pPalette,
			GetCurrentMessage()->message == WM_PALETTECHANGED);
		UINT nChanged = dc.RealizePalette();
		dc.SelectPalette(pOldPalette, TRUE);

		if (nChanged == 0)
			return FALSE;
	}
	
	Invalidate(FALSE);
	
	return TRUE;
}

void CBitmapWnd::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CBitmapWnd::OnDraw(CDC* pDC)
{
	CPalette* pOldPalette = NULL;
	
	if (m_pPalette != NULL)
	{
		pOldPalette = pDC->SelectPalette(m_pPalette, FALSE);
		pDC->RealizePalette();
	}
	
	SetDIBitsToDevice(pDC->m_hDC, 0, 0, DIBWidth(m_lpDIB), DIBHeight(m_lpDIB),
		0, 0, 0, DIBHeight(m_lpDIB), FindDIBBits(m_lpDIB), 
		(LPBITMAPINFO)m_lpDIB, DIB_RGB_COLORS);
	
	if (m_pPalette != NULL)
		pDC->SelectPalette(pOldPalette, TRUE);
}

/////////////////////////////////////////////////////////////////////////////

class CSplashScreen : public CBitmapWnd
{
public:
	CSplashScreen();

protected:
	void OnDraw(CDC* pDC);
#if HIGHLIGHT
	void AddOn(int nImageIndex);
	LPSTR m_lpHighlightDIB;

	DWORD m_grfAddOnImages;
#endif
};

#if HIGHLIGHT
static RECT rgrectProducts [] =
{
	{ 22, 26, 157, 151 },	// Visual C++
	{ 177, 22, 322, 138 },	// FORTRAN PowerStation
	{ 329, 31, 474, 141 },	// Visual Test
	{ 481, 23, 626, 133 },	// Developer Library
};
#endif

CSplashScreen::CSplashScreen()
{
#if HIGHLIGHT
	CClientDC dc(NULL);
	m_lpHighlightDIB = LoadDIBResource((dc.GetDeviceCaps(PLANES) * dc.GetDeviceCaps(BITSPIXEL)) < 8 ? 
		IDB_SPLASH16_GR : IDB_SPLASH_GR);

	m_grfAddOnImages = 0;
#endif
}

#if HIGHLIGHT
void CSplashScreen::AddOn(int nImageIndex)
{
	ASSERT(nImageIndex >= 0 && nImageIndex < 4);
	m_grfAddOnImages |= 1 << nImageIndex;
	InvalidateRect(&rgrectProducts[nImageIndex], FALSE);
	UpdateWindow();
}
#endif

void CSplashScreen::OnDraw(CDC* pDC)
{
	CBitmapWnd::OnDraw(pDC);

	CFont* pOldFont = pDC->SelectObject(GetStdFont(font_Bold));
	pDC->SetBkMode(TRANSPARENT);

	// LOCALIZATION NOTE: This rectangle should be set to the area of the
	// splash screen bitmap that contains the licensing information lines.
	// This information is displayed in the form:
	//
	//     Licensed to:
	//
	//     <name of licensee>
	//     <name of company>
	//
	CString str; str.LoadString(IDS_LICENSEDTO);
	CRect rect;
	rect.SetRect(87, 214, 400, 274);	// Bitmap Specific!


	char chM = 'M';
	int cyEmm = pDC->GetTextExtent(&chM, 1).cy;

	// NOTE: If this assertion fails, the rect is not big enough to
	// hold the licensee text and should be adjusted (which may require
	// adjusting SPLASH.BMP as well)...
	//ASSERT(rect.Height() >= cyEmm * 4);

	int Y = rect.bottom - 3 * cyEmm;

	pDC->ExtTextOut(rect.left, Y, 0, NULL, // [removed] ETO_CLIPPED, rect,
		str, str.GetLength(), NULL);
	Y += cyEmm;


	LPTSTR szUser, szOrg;
#ifdef _DEBUG
	szUser = szVerUser;
	szOrg = "Team VC++";
#else
	szUser = productInfo.szLicenseName;
	szOrg = productInfo.szLicenseCompany;
#endif
	pDC->SelectObject(GetStdFont(font_Normal));
	pDC->ExtTextOut(rect.left, Y, 0, NULL, // [removed] ETO_CLIPPED, rect,
		szUser, lstrlen(szUser), NULL);
	Y += cyEmm;

	pDC->ExtTextOut(rect.left, Y, 0, NULL, // [removed] ETO_CLIPPED, rect,
		szOrg, lstrlen(szOrg), NULL);

	pDC->SelectObject(pOldFont);
	
#if HIGHLIGHT
	if (m_lpHighlightDIB == NULL)
		return;
#endif
	
	CPalette* pOldPalette = NULL;
	
	if (m_pPalette != NULL)
	{
		pOldPalette = pDC->SelectPalette(m_pPalette, FALSE);
		pDC->RealizePalette();
	}

#if HIGHLIGHT
// [mikearn]  This behavior is going away; comment out for now
//	for (int nImageIndex = 0; nImageIndex < 4; nImageIndex += 1)
//	{
//		if ((m_grfAddOnImages & (1 << nImageIndex)) != 0)
//		{
//			rect = rgrectProducts[nImageIndex];
//			SetDIBitsToDevice(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(),
//				rect.left, DIBHeight(m_lpHighlightDIB) - rect.bottom, 0, DIBHeight(m_lpHighlightDIB), FindDIBBits(m_lpHighlightDIB), 
//				(LPBITMAPINFO)m_lpHighlightDIB, DIB_RGB_COLORS);
//		}
//	}
#endif

	if (m_pPalette != NULL)
		pDC->SelectPalette(pOldPalette, TRUE);
}

void ShowSplashScreen(BOOL bShow)
{
	if (bShow)
	{
		int dcPlanes = 0, dcPix = 0;
		{
			CClientDC dc(NULL);
			dcPlanes = dc.GetDeviceCaps(PLANES);
			dcPix = dc.GetDeviceCaps(BITSPIXEL);
		}
		BOOL bUseLowColor = (dcPlanes * dcPix) < 8;
		BOOL fEntreprise = FALSE;

		ASSERT(pSplash == NULL);
		pSplash = new CSplashScreen;


#if 0	// 23Feb98 FabriceD We now have seperate builds for Std/Pro/Ent
		LPCTSTR lpszExtensionDir = theApp.GetExeString(DefaultExtensionDir);
		if (lstrlen(lpszExtensionDir) > 0)
		{
			TCHAR szIDEDir [_MAX_PATH];
			
			// Get the directory of the IDE executable
			{
				TCHAR szPath [_MAX_PATH];
				TCHAR szDir [_MAX_DIR];
				GetModuleFileName(NULL, szPath, _MAX_PATH);
				_tsplitpath(szPath, szIDEDir, szDir, NULL, NULL);
				_tcscat(szIDEDir, szDir);
			}
			
			_tcscat(szIDEDir, lpszExtensionDir);
#ifndef _DEBUG
			_tcscat(szIDEDir, _TEXT("\\DEVENT.PKG"));
#else
			_tcscat(szIDEDir, _TEXT("\\DEVENTD.PKG"));
#endif
			if(_access(szIDEDir, 00) != -1)
				fEntreprise = TRUE;
		}
#endif
		int IDSplash;
		IDSplash = bUseLowColor ? IDB_VC16 : IDB_VC256;

		if (!pSplash->Create(IDSplash,
			WS_POPUP | WS_VISIBLE,
			CFrameWnd::rectDefault, AfxGetApp()->m_pMainWnd))
		{
			delete pSplash;
			pSplash = NULL;
			return;
		}

		pSplash->UpdateWindow();
	}
	else
	{
		if (pSplash != NULL)
		{
			// Speed 4634 [paulde] 
			// use temp to protect against reentrancy/double destruction
			CSplashScreen *pwnd = pSplash;
			pSplash = NULL;
			pwnd->ShowWindow(SW_HIDE);
			AfxGetApp()->m_pMainWnd->UpdateWindow();
			delete pwnd;
		}
	}
}

CWnd *PwndSplashScreen(void)
{
	return pSplash;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

void Draw3dRect(CDC* pDC, const CRect& rect, BOOL bIn)
{
	pDC->Draw3dRect(rect, GetSysColor(bIn ? COLOR_3DSHADOW : COLOR_3DHILIGHT),
		GetSysColor(bIn ? COLOR_3DHILIGHT : COLOR_3DSHADOW));
}



CPtrList CAddOn::c_addons;

CAddOn::CAddOn()
{
	// Add our object to the global list
	c_addons.AddTail(this);
}

CAddOn::~CAddOn()
{
	// Remove our object from the global list
	POSITION pos = c_addons.Find(this);
	if (pos != NULL)
	{
		c_addons.RemoveAt(pos);
	}
}

CAddOn::CAddOn(int nSplashImageIndex)
{
	c_addons.AddTail(this);

#if HIGHLIGHT
	if (pSplash != NULL)
		pSplash->AddOn(nSplashImageIndex);
#endif
}

const TCHAR* CAddOn::GetAboutString()
{
	return NULL;
}

HICON CAddOn::GetIcon()
{
	return NULL;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
#ifndef _SHIP
static BOOL GetModuleVersion (LPTSTR szModName, CString& strVer)
{
	// GetFileVersionInfoSize() and GetFileVersionInfo() deal with the version 
	// information buffer size in *characters* (as an abstract concept, in the 
	// sense that UNICODE characters are two bytes each).

	DWORD	dwHandle;
	DWORD	dwVerInfoSize	= GetFileVersionInfoSize(szModName, &dwHandle);
	BOOL	bRet			= FALSE;

	// Check if the module has version info
	if (dwVerInfoSize != 0)
	{
		// Note that while the string block's FileVersion is a string, it could
		// also be retreived as 4 WORDs from the VS_FIXEDFILEINFO block.
		LPVOID	pvVerInfo	= new TCHAR[dwVerInfoSize];
		LPTSTR	szSubBlock	= "\\StringFileInfo\\040904B0\\FileVersion";

		// Get the version info
		if (::GetFileVersionInfo(szModName, dwHandle, dwVerInfoSize, pvVerInfo))
		{
			// Get the file version
			UINT	nValueSize;
			LPVOID	pvValue;
			if (::VerQueryValue(pvVerInfo, szSubBlock, &pvValue, &nValueSize))
			{
				bRet = TRUE;
				strVer.Format("%s", pvValue);
			}
		}

		// Free our buffer
		delete[] pvVerInfo;
	}

	return bRet;
}
#endif	// ifndef _SHIP

class CAboutDlg : public C3dDialog
{
	enum AboutDlg_const
	{
		cPadding	= 3,
		cxMargin	= 8,
		cyMargin	= 8
	};

public:
	CAboutDlg ();
	virtual BOOL	OnInitDialog ();

	void			Draw3dRect (CDC& dc, const CRect& rctSrc);
	CSize			DrawAddInfo (CDC& dc);
	void			PaintAddInfo (CDC& dc);

	CString			m_strTitle;
	CString			m_strCopyright;
	CString			m_strLicenseInfoCaption;
	CString			m_strSerialNumberPrefix;
	CString			m_strSerialNumber;
	CString			m_strAddInfoCaption;
	CString			m_strWarning;
	CString			m_strVer;

	CFont			m_fntTitle;
	CFont			m_fntWarning;

	CBitmap			m_bmpDevStudio;
	CBitmap			m_bmpAddInfo;

	CRect			m_rctLicenseInfo;
	CRect			m_rctAddInfo;
	CRect			m_rctWarning;
	CRect			m_rctVer;

	CPoint			m_ptMouse;
	CPoint			m_ptTitle;
	CPoint			m_ptCopyright;
	CPoint			m_ptLicenseInfoCaption;
	CPoint			m_ptLicenseName;
	CPoint			m_ptLicenseCompany;
	CPoint			m_ptSerialNumberPrefix;
	CPoint			m_ptSerialNumber;
	CPoint			m_ptAddInfoCaption;
	CPoint			m_ptWarningLine;
	
	HINSTANCE		m_hInst;

	int				m_cxWarningLine;
	CPoint			m_ptScroll;
	CSize			m_sizeAddInfoImage;

	afx_msg void	OnPaint ();
	afx_msg LRESULT OnCommandHelp (WPARAM, LPARAM);
	afx_msg void	OnLButtonDblClk (UINT nFlags, CPoint point);
	afx_msg void	OnTimer (UINT nEventID);
	afx_msg void	OnLButtonDown (UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove (UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp (UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
CAboutDlg::CAboutDlg() 
	: C3dDialog(IDD_ABOUTBOX),
	m_hInst(NULL),
	m_ptScroll(0, 0),
	m_sizeAddInfoImage(0, 0)
{
	//TODO: Add any additional construction here
}

BEGIN_MESSAGE_MAP(CAboutDlg, C3dDialog)
	ON_WM_PAINT()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
BOOL CAboutDlg::OnInitDialog ()
{
	/* 
		RAID BUG - DevStudio 96 #5632 
		The Additional Information box was not wide enough to show copyright strings
		for some of the packages. This dialog consists of only a single button. The 
		rest of the dialog is manually painted onto the background. Everything is 
		sized based on the title IDS_ABOUTTITLE. Therefore, I add a padding string
		to the title to get to make it larger and to also center it above the 
		rectangle. See Lines marked TITLE PADDING. The padding string is currently
		5 blanks and is found in IDS_ABOUTTITLEPADDING. NOTE that this dialog is 
		getting large and on my system is about 550x450.
	*/
	CString strPadding;
	VERIFY(strPadding.LoadString(IDS_ABOUTTITLEPADDING));	

	CClientDC	dc(this);

	// REVIEW(ChauV): Check standards for font usage in Dolphin J
	// [olympus 14563 - chauv] check for Japanese codepage and use appropriate font
	int			yLogPixels		= dc.GetDeviceCaps(LOGPIXELSY);
	if (_getmbcp() != 932)
	{
		m_fntTitle.CreateFont(-MulDiv(10, yLogPixels, 72),
			0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS Sans Serif");

		m_fntWarning.CreateFont(-MulDiv(7, yLogPixels, 72),
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Small Fonts");
	}
	else
	{
		m_fntTitle.CreateFont(-MulDiv(10, yLogPixels, 72),
			0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "‚l‚r ‚oƒSƒVƒbƒN");

		m_fntWarning.CreateFont(-MulDiv(9, yLogPixels, 72),
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "‚l‚r ‚oƒSƒVƒbƒN");
	}
	
	// Get the bitmap
	BITMAP		bitmap;
	HINSTANCE	hInstance		= ::AfxGetResourceHandle();
	HRSRC		hResource		= ::FindResource(hInstance, MAKEINTRESOURCE(IDB_ABOUT), RT_BITMAP);
	ASSERT(hResource != NULL);
	m_bmpDevStudio.Attach(::AfxLoadSysColorBitmap(hInstance, hResource));
	m_bmpDevStudio.GetBitmap(&bitmap);

	// Get the dialog title string
	CFont*		pfntOld			= dc.SelectObject(&m_fntTitle);
	VERIFY(m_strTitle.LoadString(IDS_ABOUTTITLE));	
	m_strTitle = strPadding + m_strTitle + strPadding ; // TITLE PADDING
	CSize		sizeTitle		= dc.GetTextExtent(m_strTitle);
	m_ptTitle.x					= cxMargin * 3 + bitmap.bmWidth;
	m_ptTitle.y					= cyMargin;
	
	// Get the copyright string
	dc.SelectObject(::GetStdFont(font_Bold));
	VERIFY(m_strCopyright.LoadString(IDS_COPYRIGHT));
	m_strCopyright = strPadding + m_strCopyright + strPadding ; // TITLE PADDING
	CSize		sizeCopyright	= dc.GetTextExtent(m_strCopyright);
	m_ptCopyright.x				= m_ptTitle.x;
	m_ptCopyright.y				= m_ptTitle.y + sizeTitle.cy + cPadding;

	// Set the location for the LicenseInfo caption
	VERIFY(m_strLicenseInfoCaption.LoadString(IDS_PRODUCTLICENSEDTO));
	m_ptLicenseInfoCaption.x	= m_ptTitle.x;
	m_ptLicenseInfoCaption.y	= m_ptCopyright.y + sizeCopyright.cy + cyMargin;
	// Set the LicenseInfo rect
	CSize		size			= dc.GetTextExtent(m_strLicenseInfoCaption);
	m_rctLicenseInfo.top		= m_ptLicenseInfoCaption.y + size.cy + cPadding;
	m_rctLicenseInfo.left		= m_ptTitle.x;
	m_rctLicenseInfo.right		= m_rctLicenseInfo.left + max(sizeTitle.cx, sizeCopyright.cx);
	// License name
	m_ptLicenseName.x			= m_rctLicenseInfo.left + cPadding;
	m_ptLicenseName.y			= m_rctLicenseInfo.top + cPadding;
	dc.SelectObject(::GetStdFont(font_Normal));
	// License company
	size						= dc.GetTextExtent(productInfo.szLicenseName, _tcslen(productInfo.szLicenseName)); // DevStudio 96 Raid #6051. -1 is invalid.
	m_ptLicenseCompany.x		= m_ptLicenseName.x;
	m_ptLicenseCompany.y		= m_ptLicenseName.y + size.cy;
	size						= dc.GetTextExtent(productInfo.szLicenseCompany, _tcslen(productInfo.szLicenseCompany)); // DevStudio 96 Raid #6051. -1 is invalid.
	// Serial number prefix
	VERIFY(m_strSerialNumberPrefix.LoadString(IDS_SERIALNUMBER));
	m_ptSerialNumberPrefix.x	= m_ptLicenseCompany.x;
	m_ptSerialNumberPrefix.y	= m_ptLicenseCompany.y + size.cy + cPadding;
	// Serial number
	m_strSerialNumber			= productInfo.szSerialNumber;
	dc.SelectObject(::GetStdFont(font_Bold));
	size						= dc.GetTextExtent(m_strSerialNumberPrefix);
	m_ptSerialNumber.x			= m_ptSerialNumberPrefix.x + size.cx + cPadding;
	m_ptSerialNumber.y			= m_ptSerialNumberPrefix.y;
	m_rctLicenseInfo.bottom		= m_ptSerialNumber.y + size.cy + cPadding;

	// Set the AddInfo caption
	VERIFY(m_strAddInfoCaption.LoadString(IDS_ADDITIONS));
	m_ptAddInfoCaption.x		= m_ptLicenseInfoCaption.x;
	m_ptAddInfoCaption.y		= m_rctLicenseInfo.bottom + cyMargin + cPadding;

	// Set the AddInfo rect
	m_rctAddInfo.top			= m_ptAddInfoCaption.y + sizeCopyright.cy + cPadding;
	m_rctAddInfo.left			= m_rctLicenseInfo.left;
	m_rctAddInfo.right			= m_rctLicenseInfo.right;
	m_rctAddInfo.bottom			= m_rctAddInfo.top + (::GetSystemMetrics(SM_CYICON) + 2) * 3;

	// Move the OK button to the upper right corner of the dlg
	CWnd*		pwndOK			= GetDlgItem(IDOK);
	ASSERT_VALID(pwndOK);
	CRect		rect;
	pwndOK->GetWindowRect(rect);
	rect.OffsetRect(m_rctLicenseInfo.right + cxMargin * 2 - rect.left, cyMargin - rect.top);
	pwndOK->MoveWindow(rect);

	// Construct the warning text
	CString		str;
	VERIFY(m_strWarning.LoadString(IDS_WARNING1));
	VERIFY(str.LoadString(IDS_WARNING2));
	m_strWarning += str;
	VERIFY(str.LoadString(IDS_WARNING3));
	m_strWarning += str;
	VERIFY(str.LoadString(IDS_WARNING4));
	m_strWarning += str;
	VERIFY(str.LoadString(IDS_WARNING5));
	m_strWarning += str;
	VERIFY(str.LoadString(IDS_WARNING6));
	m_strWarning += str;
	VERIFY(str.LoadString(IDS_WARNING7));
	m_strWarning += str;

	// Set the position and length of the warning line
	m_ptWarningLine.x			= cxMargin;
	m_ptWarningLine.y			= m_rctAddInfo.bottom + cyMargin * 2;
	m_cxWarningLine				= rect.right - cxMargin;

	// Set the initial warning rect
	m_rctWarning.top			= m_ptWarningLine.y + 2 + cyMargin;
	m_rctWarning.left			= cxMargin;
	m_rctWarning.right			= rect.right;
	m_rctWarning.bottom			= 1000;		// Arbitrary large number

	// Calculate the actual warning rect
	dc.SelectObject(&m_fntWarning);
	dc.DrawText(m_strWarning, m_rctWarning,	DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);

	// Set the size and position of the dlg client rect
	// Adjust based on client rect and center the window relative to the screen
	rect.SetRect(0,	0, rect.right + cxMargin, m_rctWarning.bottom + cyMargin);

	// [multimon] 7 mar 97 - der
	//rect.OffsetRect(::GetSystemMetrics(SM_CXSCREEN) / 2 - rect.Width() / 2,
	//	::GetSystemMetrics(SM_CYSCREEN) / 2 - rect.Height() / 2);
	CenterRectToMonitor(GetSafeHwnd(), &rect, TRUE) ;

	::AdjustWindowRectEx(rect, GetStyle(), FALSE, GetExStyle());
	MoveWindow(rect);
		
	MyPlaySound("Media\\The Microsoft Sound.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

#if defined(_WIN32) && !defined(_SHIP)
	// Get the minor version number from one of the packages
	TCHAR		szName[_MAX_PATH];
	POSITION	pos				= theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage*	pPackage	= (CPackage *) theApp.m_packages.GetNext(pos);
		::GetModuleFileName(pPackage->HInstance(), szName, _MAX_PATH);

		if (GetModuleVersion(szName, m_strVer))
		{
#ifdef _DEBUG
			m_strVer += "\n" szVerUser;
#endif
			m_rctVer.left		= cxMargin;
			m_rctVer.top		= cyMargin + bitmap.bmHeight + cPadding;
			m_rctVer.bottom		= m_rctVer.top + cPadding;	// + cyStdChar
			m_rctVer.right		= m_rctVer.left + bitmap.bmWidth;
			dc.DrawText(m_strVer, &m_rctVer, DT_CALCRECT | DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);
			break;
		}
	}
#endif //_WIN32 && !_SHIP
	
	// Restore our device context objects
	dc.SelectObject(pfntOld);

	return TRUE;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
LRESULT CAboutDlg::OnCommandHelp (WPARAM, LPARAM)
{
	// The About dialog does not have any help.
	return TRUE;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::OnTimer (UINT nEventID)
{
	// Scroll the image
	m_ptScroll.y += 2;

	// Wrap around if necessary
	if (m_ptScroll.y >= m_sizeAddInfoImage.cy)
	{
		m_ptScroll.y = 0;
	}

	// Paint the scrolling image
	CClientDC dc(this);
	PaintAddInfo(dc);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::OnLButtonDown (UINT nFlags, CPoint point)
{
	// Let the user drag-scroll if the whole image is not already 
	// visible and the user clicks in the AddInfo rect
	if (m_rctAddInfo.Height() < m_sizeAddInfoImage.cy 
		&& m_rctAddInfo.PtInRect(point))
	{
		KillTimer(0);
		SetCapture();
		ASSERT(GetCapture() == this);
		m_ptMouse = point;
	}
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::OnMouseMove (UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		// User is manually scrolling image with mouse
		m_ptScroll.y += m_ptMouse.y - point.y;
		m_ptScroll.x += m_ptMouse.x - point.x;
		m_ptMouse = point;

		// Constrain the vertical scrolling to the height of the image
		int cy = m_sizeAddInfoImage.cy - m_rctAddInfo.Height();
		if (cy <= 0 || m_ptScroll.y <= 0)
		{
			m_ptScroll.y = 0;
		}
		else if (m_ptScroll.y > cy)
		{
			m_ptScroll.y = cy;
		}

		// Constrain the horizontal scrolling to the width of the image
		int cx = m_sizeAddInfoImage.cx - m_rctAddInfo.Width();
		if (cx <= 0 || m_ptScroll.x <= 0)
		{
			m_ptScroll.x = 0;
		}
		else if (m_ptScroll.x > cx)
		{
			m_ptScroll.x = cx;
		}
		
		// Paint the image
		CClientDC dc(this);
		PaintAddInfo(dc);
	}
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::OnLButtonUp (UINT nFlags, CPoint point)
{
	// Release the capture and resume normal scrolling
	if (GetCapture() == this)
	{
		ReleaseCapture();
		SetTimer(0, 100, NULL);
	}
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::OnLButtonDblClk (UINT nFlags, CPoint pt)
{
	if (GetKeyState(VK_CONTROL) >= 0)
		return;

	if (m_hInst == NULL)
		m_hInst = ::LoadLibrary( "devcrd.dll" );

	if (m_hInst == NULL)
	{
		TCHAR szDrives[255];
		if (GetLogicalDriveStrings(254, szDrives) > 254)
			return;

		LPCTSTR pDrive = szDrives;
		while (*pDrive != _T('\0'))
		{
			if (GetDriveType(pDrive) == DRIVE_CDROM)
			{
				CString strLib;
				strLib = pDrive;
				strLib += "common\\msdev98\\bin\\devcrd.dll";
				m_hInst = ::LoadLibrary(strLib);
				if (m_hInst != NULL)
					break;
			}
			pDrive = pDrive + lstrlen(pDrive) + 1;
		}
	}

	if (m_hInst != NULL)
	{
		BOOL (*lpProc)(CWnd*);
		lpProc = (BOOL (*)(CWnd*)) ::GetProcAddress(m_hInst, "StartCredits");
		if (lpProc == NULL)
		{
			::FreeLibrary(m_hInst);
			m_hInst = NULL;
			return;
		}

		(*lpProc)(this);
		::FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//-------------------------------------------------------------------------------
// DrawAddInfo:	Renders the whole AddInfo image used to scroll in the 
//		AddInfo	rect.
//-------------------------------------------------------------------------------
CSize CAboutDlg::DrawAddInfo (CDC& dc)
{
	// Set modes and colors
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
	dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));

	// Constants
	const int	cPadding		= 1;
	const int	cxIcon			= ::GetSystemMetrics(SM_CXICON) + cPadding * 2;
	const int	cyIcon			= ::GetSystemMetrics(SM_CYICON) + cPadding * 2;

	// Total size of our AddInfo image; make sure the image is 
	//	at least as wide as the clipping rectangle so that our
	//	separating lines go all the way across
	CSize	sizeTotal(m_rctAddInfo.Width(), cPadding);

	// Paint the background
	dc.FillSolidRect(0, 0, m_sizeAddInfoImage.cx, m_sizeAddInfoImage.cy, ::GetSysColor(COLOR_BTNFACE));

	// Get the extent of standard character	
	dc.SelectObject(::GetStdFont(font_Normal));
	const TCHAR	chStd			= _T('M');
	int		cyStdChar			= dc.GetTextExtent(&chStd, 1).cy;

	// Loop through AddOns
	POSITION pos = CAddOn::GetHeadPosition();
	while (pos != NULL)
	{
		// Get the AddOn object
		CAddOn *pAddOn	= CAddOn::GetNext(pos);
		if (pAddOn == NULL)
		{
			ASSERT(FALSE);
			continue;
		}

// [mikearn] This feature was cut
//		// Get and draw the icon
//		HICON		hAddIcon	= pAddOn->GetIcon();
//		if (hAddIcon != NULL)
//		{
//			dc.DrawIcon(cPadding, sizeTotal.cy + cPadding, hAddIcon);
//		}

		// Get the About string
		const TCHAR* szAbout	= pAddOn->GetAboutString();
		if (szAbout == NULL)
		{
			ASSERT(FALSE);
			continue;
		}

		// Parse and draw the About string
		BOOL		bBold		= FALSE;
		int			cxLine		= cxIcon;
		int			cyLine		= cPadding;

		while (szAbout[0] != '\0')
		{
			const TCHAR* pch = szAbout; 
			while (*pch != '\0' && *pch != '\n' && *pch != '\b')
			{
				pch = _tcsinc(pch);
			}

			int	cch = (int)(pch - szAbout);
			if (cch > 0)
			{
				dc.SelectObject((bBold) ? ::GetStdFont(font_Bold) : ::GetStdFont(font_Normal));
				dc.TextOut(cxLine, sizeTotal.cy + cyLine, szAbout, cch);
				cxLine += dc.GetTextExtent(szAbout, cch).cx;
				if (cxLine > sizeTotal.cx)
				{
					sizeTotal.cx = cxLine;
				}
			}

			switch (*pch)
			{
			case '\n':
				pch = _tcsinc(pch);
				// FALL THROUGH

			case '\0':
				cyLine += cyStdChar;
				cxLine = cxIcon;
				break;

			case '\b':
				bBold = !bBold;
				pch = _tcsinc(pch);
				break;
			}

			szAbout = pch;
		}

		// Increment the total height value
		sizeTotal.cy += max(cyLine, cyIcon);

		// Draw the separating line
		if (pos != NULL || sizeTotal.cy > m_rctAddInfo.Height())
		{
			CBrush*		pbrOld = dc.SelectObject(::GetSysBrush(COLOR_3DSHADOW));
			int			nWidth = sizeTotal.cx - cPadding * 2;
			dc.PatBlt(cPadding, sizeTotal.cy++, nWidth, 1, PATCOPY);
			dc.SelectObject(::GetSysBrush(COLOR_3DHIGHLIGHT));
			dc.PatBlt(cPadding, sizeTotal.cy++, nWidth, 1, PATCOPY);
			dc.SelectObject(pbrOld);
		}
	}

	// Return the total size of our AddInfo image
	return sizeTotal;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::PaintAddInfo (CDC& dc)
{
	CDC		dcAddInfo;

	// Create compatible dc and use that
	if (dcAddInfo.CreateCompatibleDC(&dc))
	{
		// Select bitmap
		dcAddInfo.SelectObject(&m_bmpAddInfo);

		// Get the height of the image
		int cyTopHalf = m_sizeAddInfoImage.cy - m_ptScroll.y;
		if (cyTopHalf > m_rctAddInfo.Height())
		{
			cyTopHalf = m_rctAddInfo.Height();
		}

		int cxLeftHalf = m_sizeAddInfoImage.cx - m_ptScroll.x;
		if (cxLeftHalf > m_rctAddInfo.Width())
		{
			cxLeftHalf = m_rctAddInfo.Width();
		}
		
		// Paint the image
		dc.BitBlt(m_rctAddInfo.left, m_rctAddInfo.top, cxLeftHalf,
			cyTopHalf, &dcAddInfo, m_ptScroll.x, m_ptScroll.y, SRCCOPY);

		// If the image is wrapping, paint the beginning
		if (cyTopHalf < m_sizeAddInfoImage.cy)
		{
			dc.BitBlt(m_rctAddInfo.left, m_rctAddInfo.top + cyTopHalf,
				cxLeftHalf, m_rctAddInfo.Height() - cyTopHalf, &dcAddInfo,
				m_ptScroll.x, 0, SRCCOPY);
		}
	}
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::OnPaint ()
{
	// Set the background mode and text color
	CPaintDC	dc(this);
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));

	// Draw the DevStudio logo bitmap
	{
		BITMAP		bitmap;
		VERIFY(m_bmpDevStudio.GetBitmap(&bitmap));

		CDC			dcMem;
		dcMem.CreateCompatibleDC(&dc);

		CBitmap*	pbmpOld = dcMem.SelectObject(&m_bmpDevStudio);
		dc.BitBlt(cxMargin, cyMargin, bitmap.bmWidth, bitmap.bmHeight,
			&dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pbmpOld);
	}

	// Draw the title string
	CFont*		pfntOld = dc.SelectObject(&m_fntTitle);
	dc.TextOut(m_ptTitle.x, m_ptTitle.y, m_strTitle);

	// Draw the copyright string
	dc.SelectObject(::GetStdFont(font_Bold));
	dc.TextOut(m_ptCopyright.x, m_ptCopyright.y, m_strCopyright);

	// Draw the license caption string
	dc.TextOut(m_ptLicenseInfoCaption.x, m_ptLicenseInfoCaption.y, m_strLicenseInfoCaption);

	// Draw the license info strings
	// Name
	dc.SelectObject(::GetStdFont(font_Normal));
	dc.TextOut(m_ptLicenseName.x, m_ptLicenseName.y, productInfo.szLicenseName,
		_tcslen(productInfo.szLicenseName));
	// Company
	dc.TextOut(m_ptLicenseCompany.x, m_ptLicenseCompany.y, productInfo.szLicenseCompany,
		_tcslen(productInfo.szLicenseCompany));

	// Serial number prefix
	dc.SelectObject(::GetStdFont(font_Bold));
	dc.TextOut(m_ptSerialNumberPrefix.x, m_ptSerialNumberPrefix.y, m_strSerialNumberPrefix);
	// Serial number itself
	dc.SelectObject(::GetStdFont(font_Normal));
	dc.TextOut(m_ptSerialNumber.x, m_ptSerialNumber.y, m_strSerialNumber);

	// Draw the license rect
	Draw3dRect(dc, m_rctLicenseInfo);

	// Draw AddInfo caption
	dc.SelectObject(::GetStdFont(font_Bold));
	dc.TextOut(m_ptAddInfoCaption.x, m_ptAddInfoCaption.y, m_strAddInfoCaption);

	// Draw AddInfo rect
	Draw3dRect(dc, m_rctAddInfo);

	// Get the AddInfo image
	if (m_bmpAddInfo.m_hObject == NULL)
	{
		CDC			dcAddInfo;
		dcAddInfo.CreateCompatibleDC(&dc);
		m_sizeAddInfoImage = DrawAddInfo(dcAddInfo);
		m_bmpAddInfo.CreateCompatibleBitmap(&dc, m_sizeAddInfoImage.cx,
			m_sizeAddInfoImage.cy);

		CBitmap*	pbmpOld = dcAddInfo.SelectObject(&m_bmpAddInfo);
		DrawAddInfo(dcAddInfo);
		dcAddInfo.SelectObject(pbmpOld);

		// Set the timer to scroll if the image is larger than the rect
		if (m_sizeAddInfoImage.cy > m_rctAddInfo.Height())
		{
			SetTimer(0, 100, NULL);
		}
	}

	// Paint the AddInfo image
	PaintAddInfo(dc);

	// Draw the line above the warning text
	CBrush*		pbrOld	= dc.SelectObject(::GetSysBrush(COLOR_3DSHADOW));
	dc.PatBlt(m_ptWarningLine.x, m_ptWarningLine.y, m_cxWarningLine, 1, PATCOPY);
	dc.SelectObject(::GetSysBrush(COLOR_3DHIGHLIGHT));
	dc.PatBlt(m_ptWarningLine.x, m_ptWarningLine.y + 1, m_cxWarningLine, 1, PATCOPY);

	// Draw the warning text
	dc.SelectObject(&m_fntWarning);
	dc.DrawText(m_strWarning, m_rctWarning, DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);

#if defined(_WIN32) && !defined(_SHIP)
	// Draw the VerUser stuff
	dc.DrawText(m_strVer, &m_rctVer, DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);
#endif

	// Restore device context objects
	dc.SelectObject(pfntOld);
	dc.SelectObject(pbrOld);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void CAboutDlg::Draw3dRect (CDC& dc, const CRect& rctSrc)
{
	CRect	rect(rctSrc);
	rect.InflateRect(1, 1);
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DLIGHT));
	rect.InflateRect(1, 1);
	dc.Draw3dRect(rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DHILIGHT));
}

//-------------------------------------------------------------------------------
// App command to run the dialog
//-------------------------------------------------------------------------------
void CTheApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	if (aboutDlg.m_hInst != NULL)
		::FreeLibrary(aboutDlg.m_hInst);
}
