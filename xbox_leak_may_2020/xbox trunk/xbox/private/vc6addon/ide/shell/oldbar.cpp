// toolbar.cpp : definition of old backward compatible CToolBar
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#define _AFX_NO_OLE_SUPPORT
#include <afxpriv.h>
#include "barglob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


// globals for fast drawing (shared globals)
#ifndef _MAC
HDC CToolBar::s_hDCGlyphs = NULL;
HDC CToolBar::s_hDCMono = NULL;
#define hDCGlyphs s_hDCGlyphs
#define hDCMono s_hDCMono
#else
#define hDCGlyphs   m_hDCGlyphs
#define hDCMono     m_hDCMono
#endif
static HBRUSH hbrDither = NULL;

/////////////////////////////////////////////////////////////////////////////
// Init / Term

#ifndef _MAC
static HBITMAP AFXAPI CreateDitherBitmap();
#else
static HBITMAP AFXAPI CreateDitherBitmap(BOOL bMonochrome);
#endif

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif



// a special struct that will cleanup automatically
struct _AFX_TOOLBAR_TERM
{
	~_AFX_TOOLBAR_TERM()
	{
#ifndef _MAC
		AfxDeleteObject((HGDIOBJ*)&CToolBar::s_hDCMono);
		AfxDeleteObject((HGDIOBJ*)&CToolBar::s_hDCGlyphs);
#endif
		AfxDeleteObject((HGDIOBJ*)&hbrDither);
	}
};

static const _AFX_TOOLBAR_TERM toolbarTerm;

/////////////////////////////////////////////////////////////////////////////

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

#ifndef _MAC
static HBITMAP AFXAPI CreateDitherBitmap()
#else
static HBITMAP AFXAPI CreateDitherBitmap(BOOL bMonochrome)
#endif
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD      bmiColors[16];
	} bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = ::GetSysColor(COLOR_BTNFACE);
#ifdef _MAC
	// if monochrome or the face color is already white, use black instead to make
	// sure that we draw a dither - otherwise we'll have a white on white bitmap
	if (bMonochrome || clr == RGB(255, 255, 255))
		clr = 0;
#endif
	bmi.bmiColors[0].rgbBlue = GetBValue(clr);
	bmi.bmiColors[0].rgbGreen = GetGValue(clr);
	bmi.bmiColors[0].rgbRed = GetRValue(clr);

	clr = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	bmi.bmiColors[1].rgbBlue = GetBValue(clr);
	bmi.bmiColors[1].rgbGreen = GetGValue(clr);
	bmi.bmiColors[1].rgbRed = GetRValue(clr);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HDC hDC = GetDC(NULL);
	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	ReleaseDC(NULL, hDC);

	return hbm;
}

void CreateCmdBarButtonMask(int iImage,
							CPoint ptOffset,
							BOOL bHilite,
							BOOL bShadow,
							const SIZE& sizeButton,
							const SIZE& sizeImage,
							HDC hdcGlyphs,
							HDC hdcMono)
{
	// initalize whole area with 0's
	PatBlt(hdcMono, 0, 0, sizeButton.cx-2, sizeButton.cy-2, WHITENESS);

	// create mask based on color bitmap
	// convert this to 1's
	SetBkColor(hdcGlyphs, globalData.clrBtnFace);
	BitBlt(hdcMono, ptOffset.x, ptOffset.y, sizeImage.cx, sizeImage.cy,
		hdcGlyphs, iImage * sizeImage.cx, 0, SRCCOPY);

	if (bHilite)
	{
		// convert this to 1's
		SetBkColor(hdcGlyphs, globalData.clrBtnHilite);

		// OR in the new 1's
		BitBlt(hdcMono, ptOffset.x, ptOffset.y, sizeImage.cx, sizeImage.cy,
			hdcGlyphs, iImage * sizeImage.cx, 0, SRCPAINT);

		if (bShadow)
			BitBlt(hdcMono, 1, 1, sizeButton.cx-3, sizeButton.cy-3,
				hdcMono, 0, 0, SRCAND);
	}
}

// create a mono bitmap mask:
void CToolBar::CreateMask(int iImage, CPoint ptOffset,
	 BOOL bHilite, BOOL bHiliteShadow)
{
	// Thunk into our helper function.
	CreateCmdBarButtonMask(iImage, ptOffset, bHilite, bHiliteShadow,
		m_sizeButton, m_sizeImage, hDCGlyphs, hDCMono);
}

// Raster Ops
#define ROP_DSPDxax  0x00E20746L
#define ROP_PSDPxax  0x00B8074AL

BOOL DrawButtonGlyph(CDC* pDC,
					 int x, int y,
					 CPoint ptOffset,
					 int iImage,
					 UINT nStyle,
					 const SIZE& sizeButton,
					 const SIZE& sizeImage,
					 HDC hdcGlyphs,
					 HDC hdcMono)
{
	// This function paints the button glyph.  It assumes
	// the border and background have already been handeled.

	if ((nStyle & TBBS_PRESSED) || !(nStyle & TBBS_DISABLED))
	{
		// normal image version
		BitBlt(pDC->m_hDC, x + ptOffset.x, y + ptOffset.y,
			sizeImage.cx, sizeImage.cy,
			hdcGlyphs, iImage * sizeImage.cx, 0, SRCCOPY);
		
		if (nStyle & TBBS_PRESSED)
			return TRUE;        // nothing more to do (rest of style is ignored)
	}
	
	if (nStyle & (TBBS_DISABLED | TBBS_INDETERMINATE))
	{
		// disabled or indeterminate version
		CreateCmdBarButtonMask(iImage, ptOffset, TRUE, FALSE,
			sizeButton, sizeImage, hdcGlyphs, hdcMono);
		
		pDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
		pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1
		
		if (nStyle & TBBS_DISABLED)
		{
			// disabled - draw the hilighted shadow
			HGDIOBJ hbrOld = pDC->SelectObject(globalData.hbrBtnHilite);
			if (hbrOld != NULL)
			{
				// draw hilight color where we have 0's in the mask
				BitBlt(pDC->m_hDC, x + 1, y + 1,
					sizeButton.cx - 2, sizeButton.cy - 2,
					hdcMono, 0, 0, ROP_PSDPxax);
				pDC->SelectObject(hbrOld);
			}
		}
		
		//BLOCK: always draw the shadow
		{
			HGDIOBJ hbrOld = pDC->SelectObject(globalData.hbrBtnShadow);
			if (hbrOld != NULL)
			{
				// draw the shadow color where we have 0's in the mask
				BitBlt(pDC->m_hDC, x, y,
					sizeButton.cx - 2, sizeButton.cy - 2,
					hdcMono, 0, 0, ROP_PSDPxax);
				pDC->SelectObject(hbrOld);
			}
		}
	}
	
	// if it is checked do the dither brush avoiding the glyph; extruded checked items paint with a grey bkgnd
	if ((nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE))!=0 &&
		(nStyle & (TBBS_EXTRUDED))==0)
	{
		HGDIOBJ hbrOld = pDC->SelectObject(hbrDither);
		if (hbrOld != NULL)
		{
			ptOffset.x -= globalData.cxBorder2;
			ptOffset.y -= globalData.cyBorder2;
			CreateCmdBarButtonMask(iImage, ptOffset, ~(nStyle & TBBS_INDETERMINATE),
				nStyle & TBBS_DISABLED,
				sizeButton, sizeImage, hdcGlyphs, hdcMono);
			
			pDC->SetTextColor(0L);              // 0 -> 0
			pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1 -> 1
			
			ASSERT(globalData.cxBorder2 == globalData.cyBorder2);
			int delta = (nStyle & TBBS_INDETERMINATE) ?
				globalData.bWin4 ? globalData.cxBorder2*2 : 3 : globalData.cxBorder2*2;
			
			// only draw the dither brush where the mask is 1's
			BitBlt(pDC->m_hDC,
				x + globalData.cxBorder2, y + globalData.cyBorder2, sizeButton.cx-delta, sizeButton.cy-delta,
				hdcMono, 0, 0, ROP_DSPDxax);
			pDC->SelectObject(hbrOld);
		}
	}
	
	return TRUE;
}

// if bOutlineOnly is TRUE (header default is FALSE), then the button outline will be drawn without
// the content bitmap. This can be used for special kinds of buttons which wish to exploit the inbuilt
// framing code. They should also pass the pointer to a CRect, which will be filled with the correct bounds
// for the custom drawing to be placed martynl 21Mar96
BOOL CToolBar::DrawButton(CDC* pDC, int x, int y, int iImage, UINT nStyle, BOOL bOutlineOnly, CRect *pDrawBounds)
{
	ASSERT_VALID(pDC);

	int dx = m_sizeButton.cx;
	int dy = m_sizeButton.cy;

	// interior grey
	pDC->FillSolidRect(x, y, dx, dy, globalData.clrBtnFace);

	// determine offset of bitmap (centered within button)
	CPoint ptOffset;
	ptOffset.x = (dx - m_sizeImage.cx - 1) / 2;
	ptOffset.y = (dy - m_sizeImage.cy) / 2;

	if (nStyle & (TBBS_PRESSED | TBBS_CHECKED))
	{
		// pressed in or checked
		pDC->Draw3dRect(x, y, dx, dy,
			globalData.clrBtnShadow, globalData.clrBtnHilite);

		// for any depressed button, add one to the offsets.
		ptOffset.x += 1;
		ptOffset.y += 1;
	}
	else if (nStyle & TBBS_EXTRUDED)
	{
		// regular button look
		pDC->Draw3dRect(x, y, dx, dy,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
	}

	if (!bOutlineOnly)
	{
		return DrawButtonGlyph(pDC, x, y, ptOffset, iImage,
			nStyle, m_sizeButton, m_sizeImage, hDCGlyphs, hDCMono);
	}
	else if (pDrawBounds != NULL)
	{
		*pDrawBounds=CRect(     x + ptOffset.x,                                  y + ptOffset.y, 
							x + ptOffset.x + m_sizeImage.cx, y + ptOffset.y + m_sizeImage.cy);
	}

	return TRUE;
}

#ifdef _MAC
BOOL CToolBar::DrawMonoButton(CDC* pDC, int x, int y, int dx, int dy,
	int iImage, UINT nStyle)
{
	// interior is black if pressed, white if not
	if (nStyle & (TBBS_PRESSED | TBBS_CHECKED))
	{
		pDC->FillSolidRect(x, y, dx, dy, RGB(0, 0, 0));
		pDC->SetBkColor(RGB(255, 255, 255));    // bkcolor was set by PatB
	}
	else
	{
		pDC->FillSolidRect(x, y, dx, dy, RGB(0xFF, 0xFF, 0xFF));
	}

	CPoint ptOffset;
	ptOffset.x = (dx - m_sizeImage.cx - 1) / 2;
	ptOffset.y = (dy - m_sizeImage.cy) / 2;

	if ((nStyle & TBBS_PRESSED) || !(nStyle & TBBS_DISABLED))
	{
		// normal image version
		BitBlt(pDC->m_hDC, x + ptOffset.x, y + ptOffset.y, m_sizeImage.cx,
			m_sizeImage.cy, hDCGlyphs, iImage * m_sizeImage.cx, 0,
			(nStyle & (TBBS_PRESSED | TBBS_CHECKED)) ? NOTSRCCOPY : SRCCOPY);

		if (nStyle & (TBBS_PRESSED | TBBS_CHECKED))
			return TRUE;        // nothing more to do (rest of style is ignored)
	}

	if (nStyle & TBBS_DISABLED)
	{
		BitBlt(pDC->m_hDC, x + ptOffset.x, y + ptOffset.y, m_sizeImage.cx,
			m_sizeImage.cy, hDCGlyphs, iImage * m_sizeImage.cx, 0, SRCCOPY);

		int ropOld = pDC->SetROP2(R2_MASKNOTPEN);
		RECT rect;
		SetRect(&rect, 0, 0, m_sizeImage.cx, m_sizeImage.cy);
		OffsetRect(&rect, x + ptOffset.x, y + ptOffset.y);
		AfxFillRect(pDC->m_hDC, &rect, hbrDither);
		pDC->SetROP2(ropOld);

		return TRUE;
	}

	// if it is checked do the dither brush avoiding the glyph
	if (nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE))
	{
		HGDIOBJ hbrOld = pDC->SelectObject(hbrDither);
		if (hbrOld != NULL)
		{
			CreateMask(iImage, ptOffset, ~(nStyle & TBBS_INDETERMINATE),
					nStyle & TBBS_DISABLED);

			pDC->SetTextColor(0L);              // 0 -> 0
			pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1 -> 1

			int delta = (nStyle & TBBS_INDETERMINATE) ? 3 : 1;

			// only draw the dither brush where the mask is 1's
			CRect rect(0, 0, dx, dy);
			::InvertRect(hDCMono, &rect);

			BitBlt(pDC->m_hDC, x, y, dx, dy, hDCMono, 0, 0, ROP_PSDPxax);
			pDC->SelectObject(hbrOld);
		}
	}

	return TRUE;
}
#endif

BOOL CToolBar::PrepareDrawButton(DrawState& ds)
{
	ASSERT(m_hbmImageWell != NULL);
	ASSERT(m_sizeButton.cx > 2 && m_sizeButton.cy > 2);

	// We need to kick-start the bitmap selection process.
	ds.hbmOldGlyphs = (HBITMAP)SelectObject(hDCGlyphs, m_hbmImageWell);
	ds.hbmMono = CreateBitmap(m_sizeButton.cx-2, m_sizeButton.cy-2,
					1, 1, NULL);
	ds.hbmMonoOld = (HBITMAP)SelectObject(hDCMono, ds.hbmMono);
	if (ds.hbmOldGlyphs == NULL || ds.hbmMono == NULL || ds.hbmMonoOld == NULL)
	{
		TRACE0("Error: can't draw toolbar.\n");
		AfxDeleteObject((HGDIOBJ*)&ds.hbmMono);
		return FALSE;
	}
	return TRUE;
}

void CToolBar::EndDrawButton(DrawState& ds)
{
	SelectObject(hDCMono, ds.hbmMonoOld);
	AfxDeleteObject((HGDIOBJ*)&ds.hbmMono);
	SelectObject(hDCGlyphs, ds.hbmOldGlyphs);
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar creation etc

#ifdef _DEBUG
AFX_TBBUTTON* CToolBar::_GetButtonPtr(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(m_pData != NULL);
	return ((AFX_TBBUTTON*)m_pData) + nIndex;
}
#endif

#if 0
// Just use AfxLoadSysColorBitmap
/*
	DIBs use RGBQUAD format:
		0xbb 0xgg 0xrr 0x00

	Reasonably efficient code to convert a COLORREF into an
	RGBQUAD is byte-order-dependent, so we need different
	code depending on the byte order we're targeting.
*/
#ifndef _MAC
#define RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))
#else
#define RGB_TO_RGBQUAD(r,g,b)   (RGB(r,g,b) << 8)
#define CLR_TO_RGBQUAD(clr)     (clr << 8)
#endif

#ifndef _MAC
HBITMAP AFXAPI LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc)
#else
HBITMAP AFXAPI LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc,
	HDC hDCGlyphs, BOOL bMonochrome)
#endif
{
	struct COLORMAP
	{
		// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
		DWORD rgbqFrom;
		int iSysColorTo;
	};
	static const COLORMAP sysColorMap[] =
	{
		// mapping from color in DIB to system color
		{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
		{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark grey
		{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright grey
		{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
	};
	const int nMaps = 4;

	HGLOBAL hglb;
	if ((hglb = ::LoadResource(hInst, hRsrc)) == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 16;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		for (int i = 0; i < nMaps; i++)
		{
			if (pColorTable[iColor] == sysColorMap[i].rgbqFrom)
			{
#ifdef _MAC
				if (bMonochrome)
				{
					// all colors except text become white
					if (sysColorMap[i].iSysColorTo != COLOR_BTNTEXT)
						pColorTable[iColor] = RGB_TO_RGBQUAD(255, 255, 255);
				}
				else
#endif
				pColorTable[iColor] =
					CLR_TO_RGBQUAD(::GetSysColor(sysColorMap[i].iSysColorTo));
				break;
			}
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);
	::ReleaseDC(NULL, hDCScreen);

	if (hbm != NULL)
	{
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);

#ifdef _MAC
		// We don't change this bitmap any more, so get rid of the big,
		// wasteful Macintosh port
		::SetBitmapReadOnly(hbm, BRO_READONLY);
#endif
	}

	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}
#endif

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

CToolBar::CToolBar()
{
	m_hbmImageWell = NULL;
	m_hInstImageWell = NULL;
	m_hRsrcImageWell = NULL;
	m_iButtonCapture = -1;      // nothing captured

	SetDefaultSizes();

#ifdef _MAC
	m_hDCGlyphs = CreateCompatibleDC(NULL);
	m_hDCMono = ::CreateCompatibleDC(NULL);
	if (m_hDCGlyphs == NULL || m_hDCMono == NULL)
		AfxThrowResourceException();
#endif

	// initialize the toolbar drawing engine
	static BOOL bInitialized;
	if (!bInitialized)
	{
#ifndef _MAC
		hDCGlyphs = CreateCompatibleDC(NULL);

		// Mono DC and Bitmap for disabled image
		hDCMono = ::CreateCompatibleDC(NULL);
#endif

#ifndef _MAC
		HBITMAP hbmGray = ::CreateDitherBitmap();
#else
		HBITMAP hbmGray = ::CreateDitherBitmap(m_bMonochrome);
#endif
		if (hbmGray != NULL)
		{
			ASSERT(hbrDither == NULL);
			hbrDither = ::CreatePatternBrush(hbmGray);
			AfxDeleteObject((HGDIOBJ*)&hbmGray);
		}

#ifndef _MAC
		if (hDCGlyphs == NULL || hDCMono == NULL || hbrDither == NULL)
			AfxThrowResourceException();
#else
		if (hbrDither == NULL)
			AfxThrowResourceException();
#endif
		bInitialized = TRUE;
	}
}

CToolBar::~CToolBar()
{
#ifdef _MAC
	ASSERT(m_hDCGlyphs != NULL);
	VERIFY(::DeleteDC(m_hDCGlyphs));

	ASSERT(m_hDCMono != NULL);
	VERIFY(::DeleteDC(m_hDCMono));
#endif

	AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);

	// MFC bug fix.  Make sure pLastHit is not this.
	if (m_hWnd != NULL)
		SetStatusText(-1);
	m_dwStyle = 0;
}

BOOL CToolBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	if (pParentWnd != NULL)
		ASSERT_VALID(pParentWnd);   // must have a parent

	// save the style
	m_dwStyle = dwStyle;
	if (nID == AFX_IDW_TOOLBAR)
		m_dwStyle |= CBRS_HIDE_INPLACE;

	// create the HWND
	CRect rect;
	rect.SetRectEmpty();
	LPCTSTR szWndClass = AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_BTNFACE + 1), NULL);
	if (!CWnd::Create(szWndClass, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	// Note: Parent must resize itself for control bar to be resized

	return TRUE;
}

void CToolBar::SetSizes(SIZE sizeButton, SIZE sizeImage)
{
	ASSERT_VALID(this);
	ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);
	ASSERT(sizeImage.cx > 0 && sizeImage.cy > 0);

	// button must be big enough to hold image + 3 pixels on each side
	ASSERT(sizeButton.cx >= sizeImage.cx + 6);
	ASSERT(sizeButton.cy >= sizeImage.cy + 6);

	m_sizeButton = sizeButton;
	m_sizeImage = sizeImage;

	// set height
	Invalidate();   // just to be nice if called when toolbar is visible
}

void CToolBar::SetDefaultSizes()
{
	// UISG standard sizes
	m_sizeButton = GetDefaultButtonSize();
	m_sizeImage = GetDefaultBitmapSize();
	m_cyTopBorder = m_cyBottomBorder = 3;   

	// adjust sizes
	m_sizeButton.cx = 23;
	m_cySharedBorder = m_cxSharedBorder = 0;
	m_cxDefaultGap = 8;
}

CSize CToolBar::GetDefaultBitmapSize()
{
	return globalData.csSmallButtonGlyph;
}

CSize CToolBar::GetDefaultButtonSize()
{
	return globalData.csSmallButton;
}

void CToolBar::SetHeight(int cyHeight)
{
	ASSERT_VALID(this);

	int nHeight = cyHeight;
	if (m_dwStyle & CBRS_BORDER_TOP)
		cyHeight -= globalData.cyBorder2;
	if (m_dwStyle & CBRS_BORDER_BOTTOM)
		cyHeight -= globalData.cyBorder2;
	m_cyBottomBorder = (cyHeight - m_sizeButton.cy) / 2;
	// if there is an extra pixel, m_cyTopBorder will get it
	m_cyTopBorder = cyHeight - m_sizeButton.cy - m_cyBottomBorder;
	if (m_cyTopBorder < 0)
	{
		TRACE1("Warning: CToolBar::SetHeight(%d) is smaller than button.\n",
			nHeight);
		m_cyBottomBorder += m_cyTopBorder;
		m_cyTopBorder = 0;  // will clip at bottom
	}
	// bottom border will be ignored (truncate as needed)
	Invalidate();   // just to be nice if called when toolbar is visible
}

BOOL CToolBar::LoadBitmap(UINT nIDBitmap)
{
	return LoadBitmap(MAKEINTRESOURCE(nIDBitmap));
}

BOOL CToolBar::LoadBitmap(LPCTSTR lpszResourceName)
{
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);     // get rid of old one

	m_hInstImageWell = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	if ((m_hRsrcImageWell = ::FindResource(m_hInstImageWell,
		lpszResourceName, RT_BITMAP)) == NULL)
		return FALSE;

#ifndef _MAC
	m_hbmImageWell = AfxLoadSysColorBitmap(m_hInstImageWell, m_hRsrcImageWell);
#else
	m_hbmImageWell = AfxLoadSysColorBitmap(m_hInstImageWell, m_hRsrcImageWell,
		m_hDCGlyphs, m_bMonochrome);
#endif
	return (m_hbmImageWell != NULL);
}

BOOL CToolBar::SetButtons(const UINT* lpIDArray, int nIDCount)
{
	ASSERT_VALID(this);
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(UINT) * nIDCount, FALSE));

	// first allocate array for panes and copy initial data
	if (!AllocElements(nIDCount, sizeof(AFX_TBBUTTON)))
		return FALSE;
	ASSERT(nIDCount == m_nCount);

	if (lpIDArray != NULL)
	{
		int iImage = 0;
		// go through them adding buttons
		AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
		for (int i = 0; i < nIDCount; i++, pTBB++)
		{
			ASSERT(pTBB != NULL);
			if ((pTBB->nID = *lpIDArray++) == 0)
			{
				// separator
				pTBB->nStyle = TBBS_SEPARATOR;
				// width of separator includes 2 pixel overlap
				pTBB->iImage = m_cxDefaultGap + m_cxSharedBorder * 2;
			}
			else
			{
				// a command button with image
				pTBB->nStyle = TBBS_BUTTON;
				pTBB->iImage = iImage++;
			}
		}
	}
	return TRUE;
}

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBar attribute access

int CToolBar::CommandToIndex(UINT nIDFind) const
{
	ASSERT_VALID(this);

	AFX_TBBUTTON* pTBB = _GetButtonPtr(0);
	for (int i = 0; i < m_nCount; i++, pTBB++)
		if (pTBB->nID == nIDFind)
			return i;
	return -1;
}

UINT CToolBar::GetItemID(int nIndex) const
{
	ASSERT_VALID(this);

	return _GetButtonPtr(nIndex)->nID;
}

ORIENTATION CToolBar::GetOrient() const
{
	// backwards compatibility. Overriden differently in subclasses
	if(m_dwStyle & CBRS_ORIENT_HORZ)
	{
		return orHorz;
	}
	else
	{
		return orVert;
	}
}

void CToolBar::GetItemRect(int nIndex, LPRECT lpRect, ORIENTATION or) const
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT)));

	BOOL bHorz = GetOrient()!=orVert;
	CRect rect;
	rect.SetRectEmpty();        // only need top and left
	CalcInsideRect(rect, bHorz);
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton < nIndex; iButton++, pTBB++)
	{
		ASSERT(pTBB != NULL);
		// skip this button or separator
		if (bHorz)
		{
			rect.left += (pTBB->nStyle & TBBS_SEPARATOR) ?
						pTBB->iImage : m_sizeButton.cx;
			rect.left -= m_cxSharedBorder;    // go back for overlap
		}
		else
		{
			rect.top += (pTBB->nStyle & TBBS_SEPARATOR) ?
						pTBB->iImage : m_sizeButton.cy;
			rect.top -= m_cySharedBorder;    // go back for overlap
		}
	}
	ASSERT(iButton == nIndex);
	ASSERT(pTBB == _GetButtonPtr(nIndex));

	// button or image width
	if (bHorz)
	{
		int cx = (pTBB->nStyle & TBBS_SEPARATOR) ? pTBB->iImage : m_sizeButton.cx;
		lpRect->right = (lpRect->left = rect.left) + cx;
		lpRect->bottom = (lpRect->top = rect.top) + m_sizeButton.cy;
	}
	else
	{
		int cy = (pTBB->nStyle & TBBS_SEPARATOR) ? pTBB->iImage : m_sizeButton.cy;
		lpRect->bottom = (lpRect->top = rect.top) + cy;
		lpRect->right = (lpRect->left = rect.left) + m_sizeButton.cx;
	}
}

UINT CToolBar::GetButtonStyle(int nIndex) const
{
	return _GetButtonPtr(nIndex)->nStyle;
}

void CToolBar::SetButtonStyle(int nIndex, UINT nStyle)
{
	AFX_TBBUTTON* pTBB = _GetButtonPtr(nIndex);
	UINT nOldStyle = pTBB->nStyle;
	if (nOldStyle != nStyle)
	{
		// update the style and invalidate
		pTBB->nStyle = nStyle;

		// invalidate the button only if both styles not "pressed"
		if (!(nOldStyle & nStyle & TBBS_PRESSED))
			InvalidateButton(nIndex);
	}
}

CSize CToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);

	CSize size = CControlBar::CalcFixedLayout(bStretch, bHorz);

	CRect rect;
	rect.SetRectEmpty();        // only need top and left
	CalcInsideRect(rect, bHorz);
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	int nButtonDist = 0;

	if (!bStretch)
	{
		for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
		{
			ASSERT(pTBB != NULL);
			// skip this button or separator
			nButtonDist += (pTBB->nStyle & TBBS_SEPARATOR) ?
				pTBB->iImage : (bHorz ? m_sizeButton.cx : m_sizeButton.cy);
			// go back one for overlap
			nButtonDist -= bHorz ? m_cxSharedBorder : m_cySharedBorder;
		}
		if (bHorz)
			size.cx = nButtonDist - rect.Width() + m_cxSharedBorder;
		else
			size.cy = nButtonDist - rect.Height() + m_cySharedBorder;
	}

	if (bHorz)
		size.cy = m_sizeButton.cy - rect.Height(); // rect.Height() < 0
	else
		size.cx = m_sizeButton.cx - rect.Width(); // rect.Width() < 0

	return size;
}

void CToolBar::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
{
	ASSERT_VALID(this);

	AFX_TBBUTTON* pTBB = _GetButtonPtr(nIndex);
	nID = pTBB->nID;
	nStyle = pTBB->nStyle;
	iImage = pTBB->iImage;
}

void CToolBar::SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage)
{
	ASSERT_VALID(this);

	AFX_TBBUTTON* pTBB = _GetButtonPtr(nIndex);
	pTBB->nID = nID;
	pTBB->iImage = iImage;
	pTBB->nStyle = nStyle;
	InvalidateButton(nIndex);
}

void CToolBar::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

#ifdef _MAC
#ifdef _DEBUG
	// turn off validation to speed up button drawing
	int wdSav = WlmDebug(WD_NOVALIDATE | WD_ASSERT);
#endif
#endif

	CControlBar::DoPaint(pDC);              // draw border

	// if no toolbar loaded, don't draw any buttons
	if (m_hbmImageWell == NULL)
		return;

	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;
	CRect rect;
	GetClientRect(rect);
	CalcInsideRect(rect, bHorz);

	// force the full size of the button
	if (bHorz)
		rect.bottom = rect.top + m_sizeButton.cy;
	else
		rect.right = rect.left + m_sizeButton.cx;

	DrawState ds;
	if (!PrepareDrawButton(ds))
		return;     // something went wrong

	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		ASSERT(pTBB != NULL);
		if (pTBB->nStyle & TBBS_SEPARATOR)
		{
			// separator
			if (bHorz)
				rect.right = rect.left + pTBB->iImage;
			else
				rect.bottom = rect.top + pTBB->iImage;
		}
		else
		{
			if (bHorz)
				rect.right = rect.left + m_sizeButton.cx;
			else
				rect.bottom = rect.top + m_sizeButton.cy;
		}
		if (!globalData.bWin32s || pDC->RectVisible(&rect))
		{
			DrawButton(pDC, rect.left, rect.top,
				pTBB->iImage, pTBB->nStyle);
		}

		// adjust for overlap
		if (bHorz)
			rect.left = rect.right - m_cxSharedBorder;
		else
			rect.top = rect.bottom - m_cySharedBorder;
	}
	EndDrawButton(ds);

#ifdef _MAC
#ifdef _DEBUG
	WlmDebug(wdSav);
#endif
#endif
}

void CToolBar::InvalidateButton(int nIndex)
{
	ASSERT_VALID(this);

	CRect rect;
	GetItemRect(nIndex, &rect, GetOrient());
	InvalidateRect(rect, FALSE);    // don't erase background
}

int CToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);

	// check child windows first by calling CControlBar
	int nHit = CControlBar::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	// now hit test against CToolBar buttons
	nHit = HitTest(point);
	if (nHit != -1)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(nHit);
		if (pTI != NULL)
		{
			GetItemRect(nHit, &pTI->rect, GetOrient());
			pTI->uId = pTBB->nID;
			pTI->hwnd = m_hWnd;
			pTI->lpszText = LPSTR_TEXTCALLBACK;
		}
		nHit = pTBB->nID;
	}
	return nHit;
}

int CToolBar::HitTest(CPoint point, BOOL bCheckWindow) const // in window relative coords
{
	if (m_pData == NULL)
		return -1;      // no buttons

	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;
	CRect rect;
	rect.SetRectEmpty();        // only need top and left
	CalcInsideRect(rect, bHorz);
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	ASSERT(pTBB != NULL);
	if (bHorz)
	{
		if (point.y < rect.top || point.y >= rect.top + m_sizeButton.cy)
			return -1;      // no Y hit
		for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
		{
			if (point.x < rect.left)
				break;      // missed it
			rect.left += (pTBB->nStyle & TBBS_SEPARATOR) ?
							pTBB->iImage : m_sizeButton.cx;
			if (point.x < rect.left && !(pTBB->nStyle & TBBS_SEPARATOR))
				return iButton;     // hit !
			rect.left -= m_cxSharedBorder;    // go back for overlap
		}
	}
	else
	{
		if (point.x < rect.left || point.x >= rect.left + m_sizeButton.cx)
			return -1;      // no X hit
		for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
		{
			if (point.y < rect.top)
				break;      // missed it
			rect.top += (pTBB->nStyle & TBBS_SEPARATOR) ?
							pTBB->iImage : m_sizeButton.cy;
			if (point.y < rect.top && !(pTBB->nStyle & TBBS_SEPARATOR))
				return iButton;     // hit !
			rect.top -= m_cySharedBorder;    // go back for overlap
		}
	}

	return -1;      // nothing hit
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar message handlers

BEGIN_MESSAGE_MAP(CToolBar, CControlBar)
	//{{AFX_MSG_MAP(CToolBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	int iButtonCapture = HitTest(point);
	if (iButtonCapture < 0) // nothing hit
	{
		CControlBar::OnLButtonDown(nFlags, point);
		return;
	}

	// only do this for old style bars (martynl, raid 10755)
	if(!this->IsKindOf(RUNTIME_CLASS(CCustomBar)))
	{
		// DevStudio 96 Raid BUG # 5851
		// Clicking one of the tool bar buttons does not activate the 
		// parent window. Therefore, we activate it ourselves.
		if ((m_iButtonCapture < -1) &&  // Capture set by MouseMove.
			(GetCapture() == this) )        // Capture is set.
		{
			// Activate Parent.
			GetParent()->SetActiveWindow() ;
		}
	}
	m_iButtonCapture = iButtonCapture ;

	AFX_TBBUTTON* pTBB = _GetButtonPtr(m_iButtonCapture);
	ASSERT(!(pTBB->nStyle & TBBS_SEPARATOR));

	// update the button before checking for disabled status
	UpdateButton(m_iButtonCapture);
	if (pTBB->nStyle & TBBS_DISABLED)
	{
		m_iButtonCapture = -1;
		return;     // don't press it
	}

	pTBB->nStyle |= TBBS_PRESSED;
	InvalidateButton(m_iButtonCapture);
	UpdateWindow(); // immediate feedback
	SetCapture();
	GetOwner()->SendMessage(WM_SETMESSAGESTRING, (WPARAM)pTBB->nID);
}

void CToolBar::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	if (m_iButtonCapture >= 0)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(m_iButtonCapture);
		ASSERT(!(pTBB->nStyle & TBBS_SEPARATOR));

		UINT nNewStyle = (pTBB->nStyle & ~TBBS_PRESSED);
		int iButtonCapture = m_iButtonCapture;
		if (GetCapture() != this)
		{
			m_iButtonCapture = -1; // lost capture
			nNewStyle &= ~TBBS_EXTRUDED;

			GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
		}
		else
		{
			// should be pressed if still hitting the captured button
			if (HitTest(point) == m_iButtonCapture)
				nNewStyle |= TBBS_PRESSED;
		}
		SetButtonStyle(iButtonCapture, nNewStyle);
		UpdateWindow(); // immediate feedback
	}
	else // just "float"
	{
		int iOldBtnCap = m_iButtonCapture;

		// are we active?
		int iButton=HitTest(point);
		BOOL bActive= iButton>=0 && ShouldExtrude(iButton);
		if (bActive)
			m_iButtonCapture = - iButton - 2;
		else
			m_iButtonCapture = -1;

		if(m_iButtonCapture<-1)
		{
			AFX_TBBUTTON* pTBB = _GetButtonPtr(-m_iButtonCapture - 2);

			// don't float on pressed button
			if(pTBB->nStyle & TBBS_PRESSED)
			{
				m_iButtonCapture=-1;
			}
		}

		// Update button styles
		BOOL bDirty = FALSE;
		if (iOldBtnCap != m_iButtonCapture)
		{
			if (iOldBtnCap < -1)
			{
				AFX_TBBUTTON* pTBB = _GetButtonPtr(-iOldBtnCap - 2);
				pTBB->nStyle &= ~TBBS_EXTRUDED;
				InvalidateButton(-iOldBtnCap - 2);
				bDirty = TRUE;
			}

			if (m_iButtonCapture < -1)
			{
				AFX_TBBUTTON* pTBB = _GetButtonPtr(-m_iButtonCapture - 2);

				if ((pTBB->nStyle & TBBS_DISABLED) == 0)
				{
					pTBB->nStyle |= TBBS_EXTRUDED;
					InvalidateButton(-m_iButtonCapture - 2);
					bDirty = TRUE;
				}
			}
		}

		if (iOldBtnCap == -1 && m_iButtonCapture < -1)
			SetCapture();
		else if (iOldBtnCap < -1 && m_iButtonCapture == -1 && GetCapture()==this)
			ReleaseCapture();
	}
}

BOOL CToolBar::ShouldExtrude(int iButton)
{
	return  ((GetButtonStyle(iButton) & TBBS_DISABLED)==0) && theApp.ShouldExtrude();
}

void CToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_iButtonCapture < 0)
	{
		CControlBar::OnLButtonUp(nFlags, point);
		return;     // not captured
	}

	AFX_TBBUTTON* pTBB = _GetButtonPtr(m_iButtonCapture);
	ASSERT(!(pTBB->nStyle & TBBS_SEPARATOR));
	BOOL bPressed=FALSE;

	UINT nNewStyle = pTBB->nStyle & ~TBBS_EXTRUDED;
	if (GetCapture() == this)
	{
		// we did not lose the capture
		ReleaseCapture();
		if (HitTest(point) == m_iButtonCapture)
		{
			// give button a chance to update
			UpdateButton(m_iButtonCapture);

			// then check for disabled state
			if (!(pTBB->nStyle & TBBS_DISABLED))
			{
				bPressed=TRUE;

				if (pTBB->nStyle & TBBS_CHECKBOX)
				{
					// auto check: three state => down
					if (nNewStyle & TBBS_INDETERMINATE)
						nNewStyle &= ~TBBS_INDETERMINATE;

					nNewStyle ^= TBBS_CHECKED;
				}
			}
		}
	}

	GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

	int iButtonCapture = m_iButtonCapture;
	m_iButtonCapture = -1;
	if(!bPressed ||
		OnButtonPressed(iButtonCapture)) // only gets called if the button wasn't disabled
	{
		nNewStyle &= ~ TBBS_PRESSED;
	}

	if(!theApp.m_bQuitting)
	{
		SetButtonStyle(iButtonCapture, nNewStyle);
		UpdateButton(iButtonCapture);

		if(bPressed)
		{
			// deferred feedback, to allow dialog to go away
		}
		else
		{
			UpdateWindow(); // immediate feedback
		}
	}
}

BOOL CToolBar::OnButtonPressed(int iButton)
{
	AFX_TBBUTTON* pTBB = _GetButtonPtr(iButton);
	ASSERT(!(pTBB->nStyle & (TBBS_SEPARATOR|TBBS_DISABLED)));

	if (pTBB->nID != 0)
		GetOwner()->SendMessage(WM_COMMAND, pTBB->nID);    // send command

	return TRUE;
}

void CToolBar::OnCancelMode()
{
	CControlBar::OnCancelMode();

	if (m_iButtonCapture >= 0)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(m_iButtonCapture);
		ASSERT(!(pTBB->nStyle & TBBS_SEPARATOR));
		UINT nNewStyle = (pTBB->nStyle & ~TBBS_PRESSED);
		if (GetCapture() == this)
			ReleaseCapture();
		SetButtonStyle(m_iButtonCapture, nNewStyle);
		m_iButtonCapture = -1;
		UpdateWindow();
	}
	else if (m_iButtonCapture < -1)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(-m_iButtonCapture - 2);
		ASSERT(!(pTBB->nStyle & TBBS_SEPARATOR));
		UINT nNewStyle = (pTBB->nStyle & ~TBBS_EXTRUDED);
		if (GetCapture() == this)
			ReleaseCapture();
		SetButtonStyle(-m_iButtonCapture - 2, nNewStyle);
		m_iButtonCapture = -1;
		UpdateWindow();
	}
}

void CToolBar::OnSysColorChange()
{
#ifdef _MAC
	CControlBar::OnSysColorChange();

	ASSERT(hDCGlyphs != NULL);
	VERIFY(::DeleteDC(hDCGlyphs));
	hDCGlyphs = ::CreateCompatibleDC(NULL);

	ASSERT(hDCMono != NULL);
	VERIFY(::DeleteDC(hDCMono));
	hDCMono = ::CreateCompatibleDC(NULL);
#endif

	// re-initialize global dither brush
#ifndef _MAC
	HBITMAP hbmGray = ::CreateDitherBitmap();
#else
	HBITMAP hbmGray = ::CreateDitherBitmap(m_bMonochrome);
#endif
	if (hbmGray != NULL)
	{
		HBRUSH hbrNew = ::CreatePatternBrush(hbmGray);
		if (hbrNew != NULL)
		{
			AfxDeleteObject((HGDIOBJ*)&hbrDither);      // free old one
			hbrDither = hbrNew;
		}
		::DeleteObject(hbmGray);
	}

	// re-color bitmap for toolbar
	if (m_hbmImageWell != NULL)
	{
		HBITMAP hbmNew;
#ifndef _MAC
		hbmNew = AfxLoadSysColorBitmap(m_hInstImageWell, m_hRsrcImageWell);
#else
		hbmNew = AfxLoadSysColorBitmap(m_hInstImageWell, m_hRsrcImageWell,
			m_hDCGlyphs, m_bMonochrome);
#endif
		if (hbmNew != NULL)
		{
			::DeleteObject(m_hbmImageWell);     // free old one
			m_hbmImageWell = hbmNew;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar idle update through CToolCmdUI class

void CToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_DISABLED;
	if (!bOn)
		nNewStyle |= TBBS_DISABLED;
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
}

void CToolCmdUI::SetCheck(int nCheck)
{
	ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) &
				~(TBBS_CHECKED | TBBS_INDETERMINATE);
	if (nCheck == 1)
		nNewStyle |= TBBS_CHECKED;
	else if (nCheck == 2)
		nNewStyle |= TBBS_INDETERMINATE;
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
}

void CToolCmdUI::SetText(LPCTSTR lpszNew)
{
	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	if(lpszNew)
	{
		pToolBar->UpdateText(m_nIndex,lpszNew);
	}
}

void CToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CToolCmdUI state;
	state.m_pOther = this;

	state.m_nIndexMax = (UINT)m_nCount;
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(state.m_nIndex);
		state.m_nID = pTBB->nID;

		// ignore separators
		if (!(pTBB->nStyle & TBBS_SEPARATOR))
			state.DoUpdate(pTarget, bDisableIfNoHndler);
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void CToolBar::UpdateButton(int nIndex)
{
	// determine target of command update
	CFrameWnd* pTarget = (CFrameWnd*)GetOwner();

	if (pTarget == NULL || !pTarget->IsFrameWnd())
	{
		// NOTE: bobz - to allow toolbars in CGridControlWnd, add this check. This is pretty
		// special case code to CGridControlWnd toolbars, so generalize or virtualize
		// this function is more general support is needed
		if (pTarget->IsKindOf(RUNTIME_CLASS(CGridControlWnd)))
		{
			return;
		}
		else
			pTarget = GetParentFrame();
	}
	// send the update notification
	if (pTarget != NULL)
	{
		CToolCmdUI state;
		state.m_pOther = this;
		state.m_nIndex = nIndex;
		state.m_nIndexMax = (UINT)m_nCount;
		AFX_TBBUTTON* pTBB = _GetButtonPtr(nIndex);
		state.m_nID = pTBB->nID;
		state.DoUpdate(pTarget, pTarget->m_bAutoMenuEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CToolBar diagnostics

#ifdef _DEBUG
void CToolBar::AssertValid() const
{
	CWnd::AssertValid();
	ASSERT(m_nCount == 0 || m_pData != NULL);
	ASSERT(m_hbmImageWell == NULL ||
		(globalData.bWin32s || ::GetObjectType(m_hbmImageWell) == OBJ_BITMAP));

	if (m_hbmImageWell != NULL)
	{
		ASSERT(m_hRsrcImageWell != NULL);
		ASSERT(m_hInstImageWell != NULL);
	}
}

void CToolBar::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);

	dc << "m_hbmImageWell = " << (UINT)m_hbmImageWell;
	dc << "\nm_hInstImageWell = " << (UINT)m_hInstImageWell;
	dc << "\nm_hRsrcImageWell = " << (UINT)m_hRsrcImageWell;
	dc << "\nm_iButtonCapture = " << m_iButtonCapture;
	dc << "\nm_sizeButton = " << m_sizeButton;
	dc << "\nm_sizeImage = " << m_sizeImage;

	if (dc.GetDepth() > 0)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			AFX_TBBUTTON* pTBB = _GetButtonPtr(i);
			dc << "\ntoolbar button[" << i << "] = {";
			dc << "\n\tnID = " << pTBB->nID;
			dc << "\n\tnStyle = " << pTBB->nStyle;
			if (pTBB->nStyle & TBBS_SEPARATOR)
				dc << "\n\tiImage (separator width) = " << pTBB->iImage;
			else
				dc <<"\n\tiImage (bitmap image index) = " << pTBB->iImage;
			dc << "\n}";
		}
	}

	dc << "\n";
}
#endif

#undef new
#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC(CToolBar, CControlBar)
