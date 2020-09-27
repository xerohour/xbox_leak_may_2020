// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "barglob.h"
#include "resource.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// SPI value taken from NT source (currently undocced), with Manager approval.
#define SPI_GETMENUSHOWDELAY      106
#define REGISTRY_MENUSHOWDELAY_KEY "Control Panel\\Desktop"
#define REGISTRY_MENUSHOWDELAY_VALUE "MenuShowDelay"

/////////////////////////////////////////////////////////////////////////////
// Cached system metrics, etc

GLOBAL_DATA globalData;

// Initialization code
GLOBAL_DATA::GLOBAL_DATA()
{
	// Cache various target platform version information
	DWORD dwVersion = ::GetVersion();
	nWinVer = (LOBYTE(dwVersion) << 8) + HIBYTE(dwVersion);
	bWin32s = (dwVersion & 0x80000000) != 0;
	bWin4 = (BYTE)dwVersion >= 4;
	bNotWin4 = 1 - bWin4;   // for convenience
#ifndef _MAC
	bSmCaption = bWin4;
#else
	bSmCaption = TRUE;
#endif
	bWin31 = bWin32s && !bWin4; // Windows 95 reports Win32s

	// Border attributes
	hbrLtGray = ::CreateSolidBrush(RGB(192, 192, 192));
	hbrDkGray = ::CreateSolidBrush(RGB(128, 128, 128));
	ASSERT(hbrLtGray != NULL);
	ASSERT(hbrDkGray != NULL);

	// Cached system values (updated in CWnd::OnSysColorChange)
	hbrBtnFace = NULL;
	hbrBtnShadow = NULL;
	hbrBtnHilite = NULL;
	hbrWindowFrame = NULL;
	hpenBtnShadow = NULL;
	hpenBtnHilite = NULL;
	hpenBtnText = NULL;
	UpdateSysColors();

	// cxBorder2 and cyBorder are 2x borders for Win4
	cxBorder2 = bWin4 ? CX_BORDER*2 : CX_BORDER;
	cyBorder2 = bWin4 ? CY_BORDER*2 : CY_BORDER;

	// allocated on demand
	hStatusFont = NULL;
	hToolTipsFont = NULL;
	hButtonVerticalFont = NULL;
	hButtonVerticalFontLarge = NULL;
	pFontMenuText=NULL;
	pFontMenuBold=NULL;
	pFontMenuVertical=NULL;
	nMenuTextHeight=0;
	bDeleteMenuText=FALSE;
	bDeleteMenuBold=FALSE;
	bDeleteMenuVertical=FALSE;

	hbmMenuArrow=NULL;
	hbmMenuArrowDisabled=NULL;
	hbmMenuArrowVertical=NULL;
	hbmMenuArrowVerticalDisabled=NULL;

	// information about toolbar sizes
	csSmallButtonGlyph.cx=16;
	csSmallButtonGlyph.cy=16;
	csLargeButtonGlyph.cx=32;
	csLargeButtonGlyph.cy=32;
	csSmallButton.cx=24;
	csSmallButton.cy=22;
	csLargeButton.cx=40;
	csLargeButton.cy=38;

	// determined empirically. No known systemmetric returns these, but on NT40, at least, they don't change even when
	// the system font size is ridculously large (e.g. 40)
	cxPreCloseGap=2;
	cyPreCloseGap=2;
	cxPostCloseGap=1;
	cyPostCloseGap=1;
	cxPreMinimizeGap=1;
	cyPreMinimizeGap=1;

	// Cached system metrics (updated in CWnd::OnWinIniChange)
	UpdateSysMetrics();
}

// Termination code
GLOBAL_DATA::~GLOBAL_DATA()
{
	FlushMenuFonts();

	// cleanup standard brushes
	AfxDeleteObject((HGDIOBJ*)&hbrLtGray);
	AfxDeleteObject((HGDIOBJ*)&hbrDkGray);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnFace);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hbrWindowFrame);

	// cleanup standard pens
	AfxDeleteObject((HGDIOBJ*)&hpenBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnText);

	// clean up objects we don't actually create
	AfxDeleteObject((HGDIOBJ*)&hStatusFont);
	AfxDeleteObject((HGDIOBJ*)&hToolTipsFont);
	AfxDeleteObject((HGDIOBJ*)&hButtonVerticalFont);
	AfxDeleteObject((HGDIOBJ*)&hButtonVerticalFontLarge);

	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrow);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrowDisabled);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrowVertical);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrowVerticalDisabled);
}

void GLOBAL_DATA::UpdateSysColors()
{
	ASSERT(COLOR_3DHIGHLIGHT == COLOR_BTNHIGHLIGHT);
	ASSERT(COLOR_3DSHADOW == COLOR_BTNSHADOW);

	clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clr3dDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);
	clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrGrayText = ::GetSysColor(COLOR_GRAYTEXT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);
	clrMenu = ::GetSysColor(COLOR_MENU);

	AfxDeleteObject((HGDIOBJ*)&hbrBtnFace);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hbrWindowFrame);

	hbrBtnFace = ::CreateSolidBrush(clrBtnFace);
	ASSERT(hbrBtnFace != NULL);
	hbrBtnShadow = ::CreateSolidBrush(clrBtnShadow);
	ASSERT(hbrBtnShadow != NULL);
	hbrBtnHilite = ::CreateSolidBrush(clrBtnHilite);
	ASSERT(hbrBtnHilite != NULL);
	hbrWindowFrame = ::CreateSolidBrush(clrWindowFrame);
	ASSERT(hbrWindowFrame != NULL);

	AfxDeleteObject((HGDIOBJ*)&hpenBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnText);

	hpenBtnShadow = ::CreatePen(PS_SOLID, 0, clrBtnShadow);
	ASSERT(hpenBtnShadow != NULL);
	hpenBtnHilite = ::CreatePen(PS_SOLID, 0, clrBtnHilite);
	ASSERT(hpenBtnHilite != NULL);
	hpenBtnText = ::CreatePen(PS_SOLID, 0, clrBtnText);
	ASSERT(hpenBtnText != NULL);

	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrow);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrowDisabled);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrowVertical);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuArrowVerticalDisabled);
	hbmMenuArrow=NULL;
	hbmMenuArrowDisabled=NULL;
	hbmMenuArrowVertical=NULL;
	hbmMenuArrowVerticalDisabled=NULL;
}

void GLOBAL_DATA::UpdateSysMetrics()
{
	// Device metrics for screen
	HDC hDCScreen = GetDC(NULL);
	ASSERT(hDCScreen != NULL);
	cxPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSX);
	cyPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSY);
	ReleaseDC(NULL, hDCScreen);

	csSmallIcon.cx=GetSystemMetrics(SM_CXSMICON);
	csSmallIcon.cy=GetSystemMetrics(SM_CYSMICON);
	csCaptionButton.cx=GetSystemMetrics(SM_CXMENUSIZE);
	csCaptionButton.cy=GetSystemMetrics(SM_CYMENUSIZE);

	// Those nice people over in office fortunately worked this out for me:
		// MSDN or Win95 is lying to me. The icon sizes are *not* SM_CXMENUSIZE and SM_CYMENUSIZE.
		// The values used below are the empirical result of painstaking pixel counting in
		// different Win95 schemes. (fix 14286)
	// Who am I to argue with the Office source code? Anyway, painstaking pixel counting sounds like
	// the sort of thing you want someone else to do... :)

	csCaptionButton.cx-=2;
	csCaptionButton.cy-=4;

	if(!SystemParametersInfo(SPI_GETMENUSHOWDELAY, 0, &nMenuShowDelay, FALSE))
	{
		nMenuShowDelay=400;

		// Under Win95 build 950 the SPI_ value is not supported apparently, so we use the registry directly. Mostly
		// ok, unless you've recently changed the double-click speed. 
		CString strKey = REGISTRY_MENUSHOWDELAY_KEY;

		HKEY hKey;
		if( RegOpenKeyEx(HKEY_CURRENT_USER, (char*)(const char*)strKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS )
		{
			unsigned char abBuf[512];
			DWORD dwcb = 511;
			DWORD dwType = REG_SZ;
			CString strValue=REGISTRY_MENUSHOWDELAY_VALUE;
			if( (RegQueryValueEx(hKey, strValue, NULL, &dwType, abBuf, &dwcb) == ERROR_SUCCESS) && (dwcb > 0) )
			{
				int val=atoi((char *)abBuf);
				if(val>=0)
				{
					nMenuShowDelay=val;
				}
			}
			RegCloseKey(hKey);
		}
	}

	FlushMenuFonts();

	// get details of double clicks
	tmDoubleClick=GetDoubleClickTime();
	cxDoubleClick=GetSystemMetrics(SM_CXDOUBLECLK)/2;
	cyDoubleClick=GetSystemMetrics(SM_CYDOUBLECLK)/2;
}

//
// MenuTextOut
// One stop shopping for drawing text on menus and toolbars.
//
//
void GLOBAL_DATA::MenuTextOut(CDC* pDC, 
				 const CRect* pRect,    // Bounding rectangle
				 const CString& text, 
				 UINT eFlags /*= e_LeftAlign*/)
{
	ASSERT( pRect != NULL) ;
	ASSERT( !pRect->IsRectEmpty() && !pRect->IsRectNull()) ;

	if (text.IsEmpty())
	{
		return ;
	}

	// If disabled use the shadow drawing function.
	if (eFlags & e_mtoDisabled)
	{
		DisabledTextOut(pDC, pRect, text, eFlags);
	}
	else
	{
		// If not disabled call the underlining function directly.
		UnderlineTextOut(pDC, pRect, text, eFlags);
	}
}

//
// DisabledTextOut is to be called from MenuTextOut.
//
void GLOBAL_DATA::DisabledTextOut(      CDC *pDC, 
									const CRect* pRect,
									const CString &text,
									UINT eFlags)
{
	BOOL bPainted=FALSE;

	if(UseWin4Look())
	{
		// only attempt embossed look if button face and 3d colour are same,
		// and the button face colour is different from the highlight colour; 
		// otherwise, revert to Win3x style dithering
		if( clrBtnFace != clrBtnHilite  &&
			clrMenu == clrBtnFace)
		{
			int iOldBkMode = pDC->SetBkMode(TRANSPARENT) ;
			COLORREF oldText=pDC->SetTextColor(globalData.clrBtnHilite);

			// draw the text
			CRect rectShadow(pRect) ;
			rectShadow.OffsetRect(1,1) ;
			UnderlineTextOut(pDC, &rectShadow, text, eFlags);
			pDC->SetTextColor(globalData.clrBtnShadow);
			UnderlineTextOut(pDC, pRect, text, eFlags);

			// clean up
			pDC->SetBkMode(iOldBkMode) ;
			pDC->SetTextColor(oldText);

			bPainted=TRUE;
		}
	}

	if(!bPainted)
	{
		// according to the docs for graystring, we can just draw the text ourselves
		// unless either the gray colour is dithered, or the gray colour is black

		if( clrGrayText != pDC->GetNearestColor(clrGrayText) || 
			clrGrayText == RGB(0,0,0))
		{
			//TODO: GrayString doesn't do underlining.
			//paint grey
			bPainted=pDC->GrayString(NULL, NULL, (LPARAM)((LPCTSTR)text),
									 text.GetLength(), pRect->left, pRect->top, 
									 pRect->Width(),pRect->Height());
		} 

		if(!bPainted)
		{
			// get here if graystring was inappropriate, or if it reported an error
			COLORREF oldText=pDC->SetTextColor(globalData.clrBtnText);
			UnderlineTextOut(pDC, pRect, text, eFlags);
			pDC->SetTextColor(oldText);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------_---
  6 jun 96 stolen from MSO.

  Hey, there's a comment, must mean I stole this from Mso which I did. I 
  changed it to use TCHARs and CString instead of Mso's wide strings that
  keep a zero in the first byte.

  Instead of returning a pointer, it returns an index into the CString. This
  index is -1 if an & is not found.

  MSO Comment below:

  MsoPwchStripAmpersandsWtz

	Strip odd '&' chars from the string 'wtz' in place, and return a pointer 
	to the character after the last odd '&' (the character to underline 
	as per Windows DrawText rules), or NULL if none.  As per DrawText,
	consecutive pairs of '&'s will leave a real '&' character, and all
	odd '&'s are removed even though only the last may be underlined.
------------------------------------------------------------------- DAVEPA -*/

int GLOBAL_DATA::StripAmpersands(TCHAR* pstart, int len)
{
	int indexUnderline = -1 ;
	for (TCHAR *pch = pstart ; *pch ; pch++)
	{
		if (*pch == '&')
		{
			memcpy(pch, pch + 1, (len - (pch - pstart)) * sizeof(TCHAR));
			len--;
			if (*pch && *pch != '&')
			{
				indexUnderline = pch - pstart ;
			}
		}
	}
	return indexUnderline;  
}
/////////////////////////////////////////////////////////////////////////////

void GLOBAL_DATA::UnderlineTextOut(CDC* pdc, 
								   const CRect* pRect,
								   LPCTSTR str, 
								   UINT eFlags) 
{
	BOOL bVertical = (eFlags & e_mtoVertical);
	int alignVertical = (bVertical) ? TA_BOTTOM : TA_TOP ;
	UINT oldAlign=pdc->SetTextAlign(TA_LEFT | alignVertical);
	
	// Make a copy of the string, because we are going to change it in place.
	int len = _tcslen(str) ;
	ASSERT( len > 0) ;

	TCHAR* szStripped = new TCHAR[len+1];
	_tcscpy(szStripped, str) ;

	// Find the & charater and strip it. 
	int indexUnderline = StripAmpersands(szStripped, len) ;
	if (indexUnderline >=0)
	{
		// Underline was found so string length changed.
		len = _tcslen(szStripped) ;
	}

	// Get the size.
	CSize sizeText = pdc->GetTextExtent(szStripped, len) ;

	// New rect for adjusting the size in.
	CRect rectAligned(pRect) ;

	// Center the text in its normal Y direction.
	if (bVertical)
	{
		rectAligned.left += (pRect->Width() - sizeText.cy) / 2 ;
		rectAligned.right -= (pRect->Width() - sizeText.cy) / 2 ;
	}
	else
	{
		rectAligned.top += (pRect->Height() - sizeText.cy) / 2 ;
	}

	// Center the text horizontally.
	if (eFlags & e_mtoCenterAlign)
	{
		if (bVertical)
		{
			rectAligned.top += (pRect->Height() - sizeText.cx) / 2 ;
			rectAligned.bottom -= (pRect->Height() - sizeText.cx) / 2 ;
		}
		else
		{
			rectAligned.left += (pRect->Width() - sizeText.cx) / 2 ;
		}
	}
	else if (eFlags & e_mtoRightAlign)
	{
		if (bVertical)
		{
			rectAligned.top = rectAligned.bottom - sizeText.cx ;
		}
		else
		{
			rectAligned.left = rectAligned.right - sizeText.cx ;
		}

	}
	
	// Print the text.
	pdc->ExtTextOut(rectAligned.left, rectAligned.top, 0, &rectAligned, szStripped, _tcslen(szStripped),NULL) ;

	if (indexUnderline != -1)
	{
		// draw underline by hand
		// all I'm doing here is drawing a line one pixel thick two pixels below (or
		// to the left of, depending on orientation) the font baseline. Instead of
		// calling LineTo which requires a pen, I blast the line out with ExtTextOut.
		// (three consecutive ExtTextOuts don't work, because Windows won't draw the
		// underline under a single character properly)

		// Get the metrics for the text.
		TEXTMETRIC tm ;
		pdc->GetTextMetrics(&tm) ;

		// Get the starting point for the underline
		CSize sizeBeforeUnderline = pdc->GetTextExtent(szStripped, indexUnderline);
		if (indexUnderline != 0)        // only subtract overhang if underline is not first character (fix 45383)
		{
			sizeBeforeUnderline.cx -= tm.tmOverhang;
		}

		// Get width of the underling
		CSize sizeUnderline = pdc->GetTextExtent(&szStripped[indexUnderline], 1 ) ;
		sizeUnderline.cx -= tm.tmOverhang;

		CRect rc;
		if (bVertical)
		{
			rc.left = rectAligned.right - tm.tmAscent - 2;
			rc.top  = rectAligned.top + sizeBeforeUnderline.cx;

			rc.right = rc.left + 1;                         // +1 so that the underlineline has thickness 1
			rc.bottom = rc.top  + sizeUnderline.cx;
		}
		else
		{
			rc.left = rectAligned.left + sizeBeforeUnderline.cx;
			rc.top = rectAligned.top + tm.tmAscent+1;

			rc.right = rc.left + sizeUnderline.cx;
			rc.bottom = rc.top + 1;                         // +1 so that the underlineline has thickness 1
		}

		// Draw the underline.
		COLORREF crText = pdc->GetTextColor() ;
		int iOldBkMode = pdc->SetBkMode(OPAQUE);
		COLORREF crBkSav = pdc->SetBkColor(crText);
		pdc->ExtTextOut(0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		pdc->SetBkColor(crBkSav);
		pdc->SetBkMode(iOldBkMode) ;

	}

	delete [] szStripped ;

	pdc->SetTextAlign(oldAlign);

};

//
//      GetTextExtentNoAmpersands
//
//      Gets the width of a line of text, stripping out the ampersands.
//
CSize GLOBAL_DATA::GetTextExtentNoAmpersands(CDC* pdc, const CString& str ) const
{
	ASSERT(!str.IsEmpty()) ;

	// Get the length of the string to measure
	int len = str.GetLength() ;

	// Make a copy of this string.
	TCHAR* szStripped = new TCHAR[len+1];
	_tcscpy(szStripped, (LPCTSTR)str) ;

	// Find the & charater and strip it. 
	int indexUnderline = StripAmpersands(szStripped, len) ;

	if (indexUnderline >=0)
	{
		// Underline was found so string length changed.
		len = _tcslen(szStripped) ;
	}

	// Get the size.
	CSize size = pdc->GetTextExtent(szStripped, len) ;

	// Cleanup
	delete [] szStripped ;

	return size ;
}

/////////////////////////////////////////////////////////////////////////////

// Accessors for on-demand loaded items
HFONT GLOBAL_DATA::GetButtonFont(BOOL bLarge)
{
	if(bLarge)
	{
		return static_cast<HFONT>(GetStdFont(font_Large)->m_hObject);
	}
	else
	{
		return static_cast<HFONT>(GetStdFont(font_Normal)->m_hObject);
	}
}

// Accessors for on-demand loaded items
// Vertical fonts are troublesome, as there appears to be no guarantee that a user will
// actually have any TT font on their machine at a given time.
HFONT GLOBAL_DATA::GetButtonVerticalFont(BOOL bLarge)
{
	HFONT *phFont;
	if(bLarge)
	{
		phFont=&hButtonVerticalFontLarge;
	}
	else
	{
		phFont=&hButtonVerticalFont;
	}

	// auto cache flush if size has changed
	if(*phFont==NULL)
	{
		HFONT hHoriz=GetButtonFont(bLarge);
		
		LOGFONT lfHoriz;

		VERIFY(::GetObject(hHoriz,sizeof(LOGFONT), &lfHoriz));

		lfHoriz.lfEscapement=-900;
		lfHoriz.lfOrientation=-900;
		lfHoriz.lfOutPrecision=OUT_TT_ONLY_PRECIS;
		lfHoriz.lfPitchAndFamily=FF_SWISS | VARIABLE_PITCH;
		
		*phFont=CreateFontIndirect(&lfHoriz);

		BOOL bValid=TRUE;

		if(!*phFont)
		{
			bValid=FALSE;
		}
		else
		{
			LOGFONT lfVert;

			VERIFY(::GetObject(*phFont,sizeof(LOGFONT), &lfVert));

			// must be vertical
			if(     lfVert.lfEscapement!=-900 ||
				lfVert.lfOrientation!=-900)
			{
				bValid=FALSE;
				::DeleteObject(*phFont);
			}
		}

		if(!bValid)
		{
			// try again, for any font family
			lfHoriz.lfPitchAndFamily=FF_SWISS | VARIABLE_PITCH;
			lfHoriz.lfFaceName[0]='\0';

			*phFont=CreateFontIndirect(&lfHoriz);
		}
	}

	ASSERT(*phFont!=NULL);

	return *phFont;
}

HBITMAP GLOBAL_DATA::LoadSysColorBitmap(UINT id)
{
	HINSTANCE hInstance = AfxFindResourceHandle(MAKEINTRESOURCE(id), RT_BITMAP);
	ASSERT( hInstance != NULL );
	HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(id), RT_BITMAP);

	HBITMAP hbmLoad = AfxLoadSysColorBitmap(hInstance, hRsrc);

	ASSERT(hbmLoad!=NULL);

	return hbmLoad;
}

HBITMAP GLOBAL_DATA::GetMenuArrow()
{
	if(hbmMenuArrow==NULL)
	{
		hbmMenuArrow=LoadSysColorBitmap(IDB_MENUBTN_ARROW);
	}
	return hbmMenuArrow;
}

HBITMAP GLOBAL_DATA::GetMenuArrowDisabled()
{
	if(hbmMenuArrowDisabled==NULL)
	{
		hbmMenuArrowDisabled=LoadSysColorBitmap(IDB_MENUBTN_ARROWD);
	}
	return hbmMenuArrowDisabled;
}

HBITMAP GLOBAL_DATA::GetMenuArrowVertical()
{
	if(hbmMenuArrowVertical==NULL)
	{
		hbmMenuArrowVertical=LoadSysColorBitmap(IDB_MENUBTN_ARROWV);
	}
	return hbmMenuArrowVertical;
}

HBITMAP GLOBAL_DATA::GetMenuArrowVerticalDisabled()
{
	if(hbmMenuArrowVerticalDisabled==NULL)
	{
		hbmMenuArrowVerticalDisabled=LoadSysColorBitmap(IDB_MENUBTN_ARROWVD);
	}
	return hbmMenuArrowVerticalDisabled;
}

// duplicates the portion of the selected bitmap beginning at the specified point and with area 
// specified by size 
HBITMAP GLOBAL_DATA::DuplicateBitmap(CWnd *pWnd, HBITMAP hbmBitmap, CSize size, CPoint offset)
{
	HBITMAP hbmDuplicate;

	// get dcs for src and dest
	HDC hdcClient = ::GetDC(pWnd->GetSafeHwnd());
	HDC hdcDest = ::CreateCompatibleDC(hdcClient);
	HDC hdcSrc = ::CreateCompatibleDC(hdcClient);

	// Get details of source bitmap
	BITMAP bmpSrcData;
	::GetObject(hbmBitmap, sizeof(BITMAP), &bmpSrcData);

	if(size.cx==0 && size.cy==0)
	{
		size.cx=bmpSrcData.bmWidth;
		size.cy=bmpSrcData.bmHeight;
	}
		
	// create dest
	hbmDuplicate=CreateBitmap(size.cx, size.cy, bmpSrcData.bmPlanes, bmpSrcData.bmBitsPixel, NULL);
	::ReleaseDC(pWnd->GetSafeHwnd(), hdcClient);
	
	if (hbmDuplicate==NULL || hdcDest == NULL || hdcSrc == NULL)
	{       
		// May RIP.
		::DeleteObject(hbmDuplicate);
		::DeleteDC(hdcSrc);
		::DeleteDC(hdcDest);
		return NULL;
	}
			
	// setup dcs
	HBITMAP hbmOldDest = (HBITMAP) ::SelectObject(hdcDest, hbmDuplicate);
	HBITMAP hbmOldSrc = (HBITMAP) ::SelectObject(hdcSrc, hbmBitmap);
	
	::BitBlt(hdcDest, 0, 0, size.cx, size.cy, hdcSrc, offset.x, offset.y,  SRCCOPY);

	// clean up
	::SelectObject(hdcDest, hbmOldDest);
	::SelectObject(hdcSrc, hbmOldSrc);
	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	return hbmDuplicate;
}

BOOL GLOBAL_DATA::ScaleBitmap(CWnd *pWnd, HBITMAP dest, CSize destSize, CPoint destOffset, const  HBITMAP src, CSize srcSize)
{
	HDC hdcClient = ::GetDC(pWnd->GetSafeHwnd());
	HDC hdcDest = ::CreateCompatibleDC(hdcClient);
		
	// create dest
	::ReleaseDC(pWnd->GetSafeHwnd(), hdcClient);
	
	if (hdcDest == NULL)
	{       
		// May RIP.
		::DeleteDC(hdcDest);
		return FALSE;
	}
		
	// setup dcs
	HBITMAP hbmOldDest = (HBITMAP) ::SelectObject(hdcDest, dest);

	BOOL rv=ScaleBitmap(hdcDest, destSize, destOffset, src, srcSize);

	::SelectObject(hdcDest, hbmOldDest);
	::DeleteDC(hdcDest);

	return rv;
}

BOOL GLOBAL_DATA::ScaleBitmap(HDC hdcDest, CSize destSize, CPoint destOffset, const  HBITMAP src, CSize srcSize)
{
	// get dc for src
	HDC hdcSrc = ::CreateCompatibleDC(hdcDest);

	if (hdcSrc == NULL)
	{       
		// May RIP.
		::DeleteDC(hdcSrc);
		return FALSE;
	}
		
	// setup dcs
	HBITMAP hbmOldSrc = (HBITMAP) ::SelectObject(hdcSrc, src);
	
	// copy and stretch
	BOOL rv=::StretchBlt(hdcDest, destOffset.x, destOffset.y, destSize.cx, destSize.cy, hdcSrc, 0, 0, srcSize.cx, srcSize.cy, SRCCOPY);
	ASSERT(rv);
	
	// clean up
	::SelectObject(hdcSrc, hbmOldSrc);
	::DeleteDC(hdcSrc);

	return rv;
}


// DIB functions stolen from the DIBLOOK sample

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

static WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
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

static WORD WINAPI PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

static LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}

void GLOBAL_DATA::PasteGlyph(CWnd *pWnd, HBITMAP hbmWell, int iImage, CSize sizeImage)
{
	BOOL bPasteFailed=FALSE;
	HDC hdcClient = NULL;
	HDC hdcDest = NULL;
	HGLOBAL hDib=NULL;
	HBITMAP hbmOldDest=NULL;
	HWND hWnd=pWnd->GetSafeHwnd();

	{ 
		// BLOCK containing gotos
		// start by filling in background, in case things go wrong
		hdcClient = ::GetDC(hWnd);
		hdcDest = ::CreateCompatibleDC(hdcClient);


		if (hdcDest == NULL)
		{       
			TRACE("Cannot get dc\n");
			bPasteFailed=TRUE;
			goto lPasteFailed;
		}

		hbmOldDest = (HBITMAP) ::SelectObject(hdcDest, hbmWell);

		// fill in backgroyund
		CBrush* pbr = GetSysBrush(CMP_COLOR_LTGRAY);
		HBRUSH hbrOld = (HBRUSH) ::SelectObject(hdcDest, pbr->m_hObject);
		::PatBlt(hdcDest, sizeImage.cx * iImage, 0,     sizeImage.cx, sizeImage.cy, PATCOPY);
		::SelectObject(hdcDest, hbrOld);

		// copy from current clipboard
		if (!pWnd->OpenClipboard())
		{
			// reporting error with Beep following convention in resource editor
			TRACE("Cannot open clipboard\n");
			bPasteFailed=TRUE;
			goto lPasteFailed;
		}
		
		// Enumerate the cliboard contents to determine what is available.
		// If a CF_BITMAP is seen, stop looking. If CF_DIB is seen, set a 
		// flag and proceed. 
		
		BOOL bBitmapAvailable = FALSE;
		BOOL bDibAvailable = FALSE;

		UINT nClipFmt = 0;
		do
		{
			nClipFmt = EnumClipboardFormats(nClipFmt);
			
			switch (nClipFmt)
			{
			case CF_BITMAP:
				bBitmapAvailable = TRUE;
				break;
				
			case CF_DIB:
				bDibAvailable = TRUE;
				break;
			}
		}
		while (!bBitmapAvailable && nClipFmt != 0);

		if(!bBitmapAvailable && !bDibAvailable)
		{
			// reporting error with Beep following convention in resource editor
			TRACE("Cannot find appropriate format\n");
			bPasteFailed=TRUE;
			goto lPasteFailed;
		}

		CSize sizePaste;
		CBitmap *pClipBitmap=NULL;
		LPBITMAPINFO lpDib=NULL;

		if(bBitmapAvailable)
		{
			pClipBitmap = CBitmap::FromHandle((HBITMAP)GetClipboardData(CF_BITMAP));

			// Clipboard occasionally lies, when app's use defered rendering,
			// but don't actually produce the goods on demand.

			if (pClipBitmap == NULL)
			{
				TRACE("Cannot access bitmap format\n");
				bPasteFailed=TRUE;
				goto lPasteFailed;
			}

			BITMAP bm;
			pClipBitmap->GetObject(sizeof(BITMAP), &bm);
			sizePaste.cx = bm.bmWidth;
			sizePaste.cy = bm.bmHeight;
		}
		else
		{
			ASSERT(bDibAvailable);

			hDib = (HGLOBAL)GetClipboardData(CF_DIB);
			if (hDib == NULL)
			{
				TRACE("Cannot access dib format\n");
				bPasteFailed=TRUE;
				goto lPasteFailed;
			}

			lpDib = (LPBITMAPINFO)GlobalLock(hDib);
			if (lpDib->bmiHeader.biSize != sizeof (BITMAPINFOHEADER))
			{
				// Can't deal with OS/2 style DIBs...
				GlobalUnlock(hDib);
				TRACE("Cannot use OS2 dib format\n");
				bPasteFailed=TRUE;
				goto lPasteFailed;
			}

			sizePaste.cx = lpDib->bmiHeader.biWidth;
			sizePaste.cy = lpDib->bmiHeader.biHeight;
		}

		sizePaste.cx=min(sizeImage.cx, sizePaste.cx);
		sizePaste.cy=min(sizeImage.cy, sizePaste.cy);

		if(bBitmapAvailable)
		{
			HDC hdcSrc = ::CreateCompatibleDC(hdcClient);

			if (hdcSrc == NULL)
			{       
				TRACE("Cannot get dc\n");
				bPasteFailed=TRUE;
				goto lPasteFailed;
			}
		
			HBITMAP hbmOldSrc = (HBITMAP) ::SelectObject(hdcSrc, (HBITMAP)(*pClipBitmap));

			BitBlt(hdcDest, iImage*sizeImage.cx, 0, sizePaste.cx, sizePaste.cy, hdcSrc, 0, 0, SRCCOPY);

			::SelectObject(hdcSrc, hbmOldSrc);
			::DeleteDC(hdcSrc);
		}
		else
		{
			LPSTR    lpDIBBits=::FindDIBBits((LPSTR) lpDib);
			SetDIBitsToDevice(hdcDest,iImage*sizeImage.cx,0,sizePaste.cx, sizePaste.cy, 0,0, 0, sizePaste.cy, lpDIBBits, lpDib, DIB_RGB_COLORS);
		}
	}

lPasteFailed:
	if(hdcClient)
	{
		::ReleaseDC(hWnd, hdcClient);
	}

	if(hdcDest!=NULL)
	{
		::SelectObject(hdcDest, hbmOldDest);
		::DeleteDC(hdcDest);
	}

	if (hDib != NULL)
		GlobalUnlock(hDib);
	
	CloseClipboard();

	if(bPasteFailed)
	{
		MessageBeep(0);
		// returning here is not a disaster. The well has been filled with a grey
		// background.
		return;
	}
}

// Code stolen and adapted from MFC - had to, they don't make it public.
#define RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

struct TBCOLORMAP
{
	// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
	DWORD rgbqFrom;
	int iSysColorTo;
};

// This table is copied from MFC
static const TBCOLORMAP sysColorMap[] =
{
	// mapping from color in DIB to system color
	{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
	{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark grey
	{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright grey
	{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
};
const int nMaps = 4;

#define WIDTHBYTES(i)   ((i+31)/32*4)      /* ULONG aligned ! */

// This is the palette for all of our toolbars. We use this palette for all custom bitmaps too,
// meaning that a bitmap pasted from one of our own bars cannot be rendered incorrectly
PALETTEENTRY stdToolbarPalette[]=
{
	{ 0x00,0x00,0x00,0x0 },
	{ 0x00,0x00,0x80,0x0 },
	{ 0x00,0x80,0x00,0x0 },
	{ 0x00,0x80,0x80,0x0 },
	{ 0x80,0x00,0x00,0x0 },
	{ 0x80,0x00,0x80,0x0 },
	{ 0x80,0x80,0x00,0x0 },
	{ 0x80,0x80,0x80,0x0 },
	{ 0xC0,0xC0,0xC0,0x0 },
	{ 0x00,0x00,0xF0,0x0 },
	{ 0x00,0xF0,0x00,0x0 },
	{ 0x00,0xF0,0xF0,0x0 },
	{ 0xF0,0x00,0x00,0x0 },
	{ 0xF0,0x00,0xF0,0x0 },
	{ 0xF0,0xF0,0x00,0x0 },
	{ 0xF0,0xF0,0xF0,0x0 }
};

// Assorted DIB functions stolen from the CROPDIB sample
/* DibNumColors(pv)
 *
 * Returns the number of palette entries in the palette. The <pv> parameter
 * can point to a BITMAPINFOHEADER or BITMAPCOREHEADER structure.
 */
WORD GLOBAL_DATA::DibNumColors(
    VOID FAR * pv)          // Pointer to the bitmap info header structure
{
    LPBITMAPINFOHEADER lpbi= ((LPBITMAPINFOHEADER)pv);

    int nBitCount;

    /* With a BITMAPINFOHEADER structure, the number of palette entries
     * is in biClrUsed; otherwise, the count depends on the number of bits
     * per pixel.
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
    {
	if(lpbi->biClrUsed != 0)
	    return (WORD)lpbi->biClrUsed;

	nBitCount = lpbi->biBitCount;
    }

    switch (nBitCount)
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

/* PaletteSize(pv)
 *
 * Returns the size of the palette in bytes. The <pv> parameter can point
 * to a BITMAPINFOHEADER or BITMAPCOREHEADER structure.
 */
WORD GLOBAL_DATA::PaletteSize(
    VOID FAR * pv)          // Pointer to the bitmap info header structure
{
    LPBITMAPINFOHEADER lpbi ((LPBITMAPINFOHEADER)pv);

    WORD    NumColors;

    NumColors = DibNumColors(lpbi);

	return NumColors * sizeof(RGBQUAD);
}

// This code has to manually convert a DDB to a DIB for the above palette

HANDLE GLOBAL_DATA::SaveSysColorBitmap(HBITMAP hbmSrc)
{
	ASSERT(hbmSrc!=NULL);

	if (hbmSrc != NULL)
	{
		// LOGPALETTE includes one palette entry in size
		LOGPALETTE *pLogPal=(LOGPALETTE *)calloc(1,sizeof(LOGPALETTE)+sizeof(stdToolbarPalette)-sizeof(PALETTEENTRY));

		ASSERT(pLogPal!=NULL);

		pLogPal->palVersion=0x300;
		pLogPal->palNumEntries=sizeof(stdToolbarPalette)/sizeof(PALETTEENTRY);
		memcpy(pLogPal->palPalEntry, stdToolbarPalette, sizeof(stdToolbarPalette));

		HPALETTE hPal=::CreatePalette(pLogPal);

		HANDLE hDIB=CreateLogicalDib(hbmSrc, 4, hPal);

		::DeleteObject(hPal);

		free(pLogPal);

		return hDIB;
	}

	return NULL;
}

HBITMAP GLOBAL_DATA::LoadSysColorBitmap(LPBITMAPINFOHEADER lpBitmap)
{
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
				pColorTable[iColor] =
						CLR_TO_RGBQUAD(::GetSysColor(sysColorMap[i].iSysColorTo));
				break;
			}
		}
	}

	// now load it into one of our screen-type bitmaps, using the adjusted colour table.
	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);

		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);

	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);

	return hbm;
}

// Code stolen and modified from an CROPDIB example

/*  CreateLogicalDib(hbm, biBits, hPal)
 *
 *  Given a DDB and a HPALETTE create a "logical" DIB. If the HPALETTE is
 *  NULL, it uses the system palette.
 *
 *  A "logical" DIB is a DIB where the DIB color table *exactly* matches
 *  the passed logical palette.  There will be no system colors in the
 *  color table, and a pixel value of <n> in the DIB corresponds to logical
 *  palette index <n>.
 *
 *  Why create a "logical" DIB? When the DIB is written to a disk file and
 *  then reloaded, the logical palette created from the DIB color table 
 *  exactly matches the one used originaly to create the bitmap. It also
 *  prevents GDI from doing nearest color matching on PC_RESERVED palettes.
 *
 *  To create the logical DIB, we call GetDIBits() with the DIB_PAL_COLORS
 *  option. We then convert the palette indices returned in the color table
 *  to logical RGB values.  The entire logical palette passed to <hpal> 
 *  is always copied to the DIB color table.
 *
 *  The DIB color table will have exactly the same number of entries as
 *  the logical palette.  Normally GetDIBits() sets the biClrUsed field to
 *  the maximum colors supported by the device, regardless of the number of
 *  colors in the logical palette. If the logical palette contains more 
 *  than 256 colors, the function truncates the color table at 256 entries.
 */

HANDLE GLOBAL_DATA::CreateLogicalDib(
    HBITMAP hbm,        // DDB to copy
    WORD biBits,        // New DIB: bit count: 8, 4, or 0
    HPALETTE hpal)      // New DIB: palette
{
    LPBITMAPINFOHEADER  lpbiDDB;      // Temporary pointer to DDB BITMAPINFO
    WORD FAR *          lpDDBClrTbl;  // Pointer to DIB color table

    HANDLE              hLDib;
    LPBITMAPINFOHEADER  lpLDib;       // Pointer to logical DIB header
    BYTE FAR *          lpLDibBits;   // Pointer to logical DIB bits
    RGBQUAD FAR *       lpLDibRGB;    // Pointer to logical DIB color table
    WORD                nLDibColors;  // How many colors in logical DIB
    DWORD               dwLDibLen;    // Size of logical DIB

    HDC                 hdc;          // Temp stuff, working variables
    BITMAP              bm;
    BITMAPINFOHEADER    bi;
    PALETTEENTRY        peT;
    DWORD               dw;
    int                 n;
    HPALETTE            hpalT;

	ASSERT(hpal!=NULL);
    if (hbm == NULL)
	return NULL;

    VERIFY(GetObject(hpal,sizeof(nLDibColors),(LPSTR)&nLDibColors));
    VERIFY(GetObject(hbm,sizeof(bm),(LPSTR)&bm));

    /* Truncate palette entries at 256 if the logical palette has more
     * than 256 entries.
     */
    if (nLDibColors > 256)
	nLDibColors = 256;

    /* If bit count is zero, fill in bit count based on number of colors
     * in palette.
     */
    if (biBits == 0)
	biBits = nLDibColors > 16 ? 8 : 4;

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = BI_RGB;
    bi.biSizeImage          = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * 
				bm.bmHeight;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = nLDibColors;
    bi.biClrImportant       = 0;

    dwLDibLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;

    hLDib = GlobalAlloc(GMEM_FIXED,dwLDibLen);
				    
    if (!hLDib)
	return NULL;

    lpbiDDB = (LPBITMAPINFOHEADER)GlobalAlloc(GMEM_FIXED,bi.biSize + 256 * sizeof(RGBQUAD));

    if (!lpbiDDB)
    {
	GlobalFree(hLDib);
	return NULL;
    }

    hdc = ::GetDC(NULL);
    hpalT = SelectPalette(hdc,hpal,FALSE);
    RealizePalette(hdc);

    lpLDib = (LPBITMAPINFOHEADER)GlobalLock(hLDib);

    *lpbiDDB  = bi;
    *lpLDib = bi;

    /* Get the DIB bits. With DIB_PAL_COLORS flag, the color table is
     * filled with logical palette indices.
     */
    lpLDibBits = (BYTE *)lpLDib + (WORD)lpLDib->biSize + PaletteSize(lpLDib);

    GetDIBits(hdc,                    // Device context
	      hbm,                    // Bitmap we're copying
	      0,                      // Starting scan line
	      (WORD)bi.biHeight,      // Number of scan lines to copy
	      lpLDibBits,           // Receives bitmap bits in DIB format
	      (LPBITMAPINFO)lpbiDDB,  // Receives DDB color table
	      DIB_PAL_COLORS);        // Usage--copy indices into the 
				      // currently realized logical palette

    /* Convert the DIB bits from indices into the color table (which
     * contains indices into the logical palette) to direct indices
     * into the logical palette.
     *
     * lpDDBClrTbl   Points to the DIB color table, which is a WORD array of
     *               logical palette indices.
     *
     * lpLDibBits    Points to the DIB bits. Each DIB pixel is a index into
     *               the DIB color table.
     */
    lpDDBClrTbl = (WORD FAR *)((LPSTR)lpbiDDB + (WORD)lpbiDDB->biSize);

    if (biBits == 8)
    {
	for (dw = 0; dw < bi.biSizeImage; dw++, ((BYTE *)lpLDibBits)++)
	    *lpLDibBits = (BYTE)lpDDBClrTbl[*lpLDibBits];
    }
    else // biBits == 4
    {
	for (dw = 0; dw < bi.biSizeImage; dw++, ((BYTE *)lpLDibBits)++)
	    *lpLDibBits = (BYTE)(lpDDBClrTbl[*lpLDibBits & 0x0F] |
			     (lpDDBClrTbl[(*lpLDibBits >> 4) & 0x0F] << 4));
    }

    /* Now copy the RGBs in the logical palette to the DIB color table.
     */
    lpLDibRGB = (RGBQUAD FAR *)((LPSTR)lpLDib + (WORD)lpLDib->biSize);

    for (n=0; n<nLDibColors; n++, lpLDibRGB++)
    {
	GetPaletteEntries(hpal,n,1,&peT);

	lpLDibRGB->rgbRed      = peT.peRed;
	lpLDibRGB->rgbGreen    = peT.peGreen;
	lpLDibRGB->rgbBlue     = peT.peBlue;
	lpLDibRGB->rgbReserved = (BYTE)0;
    }

    GlobalUnlock(hLDib);
    GlobalFree(lpbiDDB);

    SelectPalette(hdc,hpalT,FALSE);
    ::ReleaseDC(NULL,hdc);

    return hLDib;
}

TCHAR GLOBAL_DATA::ExtractAccelerator(LPCTSTR item)
{
	CString strLabel(item);
	BOOL bFinished=FALSE;
	TCHAR chAccel=0;
	while(!bFinished)
	{
		int iAmpersand;
		if ((iAmpersand = strLabel.Find('&')) != -1)
		{
			ASSERT(strLabel.GetLength() > iAmpersand+1);    // & can't be last char
			if (strLabel[iAmpersand + 1] == '&')
			{
				strLabel = strLabel.Right(strLabel.GetLength() - (iAmpersand + 2));
			}
			else
			{
				chAccel = strLabel[iAmpersand + 1];
				bFinished=TRUE;
			}
		}
		else
		{
			bFinished=TRUE;
		}
	}
	return chAccel;
}                               


// This code is shared between the popup menu and the custom button dialog, so it's been put here
		
// Because buttons can have compulsory, optional or forbidden states for each of text and image,
// there are a complex set of options. This table summarises:

/*
				IF      IF      IF      IO      IO      IO      IC      IC      IC
				TF      TO      TC      TF      TO      TC      TF      TO      TC


I               D       D       D       D       E       D       D       E       D
T               D       D       D       D       E       E       D       D       D
IT              D       D       D       D       E       E       D       E       D

IL              D       D       D       E       C       C       E       E       E
TE              D       E       E       D       C       E       D       C       E

DG              N       Y               Y               Y               Y               Y               Y               Y               Y

Key:

Rows describe the availability of items within this dialog:
I: Image Only Radio button
T: Text ONly Radio button
IT: Image and Text Radio Button
IL: The image list (m_pButtonSource)
TL: The text entry field
DG: Should the dialog be visible at all

Columns describe the values of m_aoCanText and m_aoCanImage
IF: m_aoCanImage==cAppearanceForbidden
IO: m_aoCanImage==cAppearanceOptional
IC: m_aoCanImage==cAppearanceCompulsory
TF: m_aoCanText==cAppearanceForbidden
TO: m_aoCanText==cAppearanceOptional
TC: m_aoCanText==cAppearanceCompulsory

Table entries are as follows:
D: Control is disabled
E: Control is enabled
C: Control is enabled conditionally, depending on current state of I, T and IT.
Y: Dialog is available
N: Dialog is unavailable

Could use a karnaugh map, I guess, but I think I'll do it manually.

When the radio buttons are disabled, their current state will still correctly reflect the only valid state of the dialog

martynl 24Jun96

*/

// local helper function
static inline BOOL EverythingDisabled(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage)
{
	// no radio buttons if
	return aoText==cAppearanceForbidden || aoImage==cAppearanceForbidden || 
						(aoText==cAppearanceCompulsory && aoImage==cAppearanceCompulsory);
}


BOOL GLOBAL_DATA::CanChooseImageOnly(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage)
{
	return !(EverythingDisabled(aoText, aoImage) || aoText==cAppearanceCompulsory);
}

BOOL GLOBAL_DATA::CanChooseTextOnly(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage)
{
	return !(EverythingDisabled(aoText, aoImage) || aoImage==cAppearanceCompulsory);
}

BOOL GLOBAL_DATA::CanChooseImageText(APPEARANCEOPTION aoText, APPEARANCEOPTION aoImage)
{
	return !EverythingDisabled(aoText, aoImage);
}

// gets the font that would be used in a menu at this time
CFont *GLOBAL_DATA::GetMenuFont(BOOL bDefault)
{
	CFont **ppFont=NULL;
	BOOL *pbDelete;

	if(!bDefault)
	{
		ppFont=&pFontMenuText;
		pbDelete=&bDeleteMenuText;
	}
	else
	{
		ppFont=&pFontMenuBold;
		pbDelete=&bDeleteMenuBold;
	}

	ASSERT(ppFont!=NULL);

	// if the font's already loaded, use it
	if(*ppFont)
	{
		return *ppFont;
	}
	else
	{
		// load the font
		NONCLIENTMETRICS ncm;
		ncm.cbSize=sizeof(ncm);

		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, FALSE);

		if(bDefault)
		{
			// make it a bold font. 700 is the defined weight for bold
			if(ncm.lfMenuFont.lfWeight < 700)
			{
				ncm.lfMenuFont.lfWeight=700;
			}
			else
			{
				// it's already bold or heaver, so make it as heavy as possible
				ncm.lfMenuFont.lfWeight=1000;
			}
		}

		// so now create the font
		*ppFont=new CFont;

		if(!((*ppFont)->CreateFontIndirect(&ncm.lfMenuFont)))
		{
			delete *ppFont;
			// creation failed. Fall back on our default internals. 
			*ppFont=GetStdFont(bDefault ? font_Bold : font_Normal);
			*pbDelete=FALSE;
		}
		else
		{
			*pbDelete=TRUE;
		}

		return *ppFont;
	}
}

// gets the font that would be used in a menu at this time
CFont *GLOBAL_DATA::GetMenuVerticalFont()
{
	// if the font's already loaded, use it
	if(pFontMenuVertical)
	{
		return pFontMenuVertical;
	}
	else
	{
		// load the font
		NONCLIENTMETRICS ncm;
		ncm.cbSize=sizeof(ncm);

		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, FALSE);

		ncm.lfMenuFont.lfEscapement=-900;
		ncm.lfMenuFont.lfOrientation=-900;
		ncm.lfMenuFont.lfOutPrecision=OUT_TT_ONLY_PRECIS;

		// so now create the font
		pFontMenuVertical=new CFont;

		if(!((pFontMenuVertical)->CreateFontIndirect(&ncm.lfMenuFont)))
		{
			delete pFontMenuVertical;
			// creation failed. Fall back on our default internals. 
			pFontMenuVertical=CFont::FromHandle(GetButtonVerticalFont(GetLargeMenuButtons()));
			bDeleteMenuVertical=FALSE;
		}
		else
		{
			LOGFONT lfVert;

			VERIFY(pFontMenuVertical->GetObject(sizeof(LOGFONT), &lfVert));

			// must be vertical
			if( lfVert.lfEscapement!=-900 ||
				lfVert.lfOrientation!=-900)
			{
				delete pFontMenuVertical;
				// creation failed. Fall back on our default internals. 
				pFontMenuVertical=CFont::FromHandle(GetButtonVerticalFont(GetLargeMenuButtons()));
				bDeleteMenuVertical=FALSE;
			}
			else
			{
				bDeleteMenuVertical=TRUE;
			}
		}

		return pFontMenuVertical;
	}
}

// Deletes any cached fonts
void GLOBAL_DATA::FlushMenuFonts(void)
{
	nMenuTextHeight=0;
	if(pFontMenuText!=NULL)
	{
		// don't delete if we fell back onto a standard font due to resource limitations
		if(bDeleteMenuText)
		{
			delete pFontMenuText;
		}
		pFontMenuText=NULL;
	}

	if(pFontMenuBold!=NULL)
	{
		// don't delete if we fell back onto a standard font due to resource limitations
		if(bDeleteMenuBold)
		{
			delete pFontMenuBold;
		}
		pFontMenuBold=NULL;
	}
	if(pFontMenuVertical!=NULL)
	{
		// don't delete if we fell back onto a standard font due to resource limitations
		if(bDeleteMenuVertical)
		{
			delete pFontMenuVertical;
		}
		pFontMenuVertical=NULL;
	}
}

int GLOBAL_DATA::GetMenuTextHeight(void)
{
	if(nMenuTextHeight==0)
	{
		// get the font that will be used to draw menu items
		CFont *pFont=GetMenuFont(FALSE);

		// get the size of large glyphs
		CSize sizeLargeBitmap=CCustomBar::GetDefaultBitmapSize(TRUE);
		
		TEXTMETRIC tm;

		// determine maximum size of the font
		CDC dc;               
		dc.CreateCompatibleDC(NULL);

		// select the menu font
		CFont *pOldFont=dc.SelectObject(pFont);

		dc.GetTextMetrics(&tm);

		nMenuTextHeight=tm.tmHeight;

		// large buttons if the text is larger than a large button
		bLargeMenuButtons=tm.tmHeight>=sizeLargeBitmap.cy;

		dc.SelectObject(pOldFont);
	}

	return nMenuTextHeight;
}

int GLOBAL_DATA::GetMenuVerticalHeight(void)
{
	if(nMenuVerticalHeight==0)
	{
		// get the font that will be used to draw menu items
		CFont *pFont=GetMenuVerticalFont();

		TEXTMETRIC tm;

		// determine maximum size of the font
		CDC dc;               
		dc.CreateCompatibleDC(NULL);

		// select the menu font
		CFont *pOldFont=dc.SelectObject(pFont);

		dc.GetTextMetrics(&tm);

		nMenuVerticalHeight=tm.tmHeight;

		dc.SelectObject(pOldFont);
	}

	return nMenuVerticalHeight;
}

BOOL GLOBAL_DATA::GetLargeMenuButtons(void)
{
	GetMenuTextHeight();

	return bLargeMenuButtons;
}
