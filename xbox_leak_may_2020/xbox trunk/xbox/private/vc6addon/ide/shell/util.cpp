#include "stdafx.h"
#pragma hdrstop

#include <macimage.h>
#include <ppcimage.h>

#include "shell.h"
#include "util.h"
#include "resource.h"
#include "barglob.h"
#include "image.h"

#include <bldapi.h>

// [multimon] 11 mar 97 - der
#include "mmonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
BOOL bChicagoLookOnNT = FALSE;
inline BOOL OnChicago() { return theApp.m_bWin4 || bChicagoLookOnNT; }
#else
inline BOOL OnChicago() { return theApp.m_bWin4; }
#endif

///////////////////////////////////////////////////////////////////////////////
//	CLockDC
//		Tries to lock its window for drawing.

CLockDC::CLockDC(CWnd* pWnd)
{
	m_hWnd = pWnd->GetSafeHwnd();
	if (m_hWnd == NULL)
		m_hWnd = ::GetDesktopWindow();
	::LockWindowUpdate(m_hWnd);

	if (!Attach(::GetDCEx(m_hWnd, NULL,
			DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE)))
		AfxThrowResourceException();
}

CLockDC::~CLockDC()
{
	ASSERT(m_hDC != NULL);
	::ReleaseDC(m_hWnd, Detach());
	::LockWindowUpdate(NULL);
}

/////////////////////////////////////////////////////////////////////////////
//
// The following code manages a cache of GDI brushes and pens that correspond
// to the system defined colors.  The cache is flushed when the user changes
// any of the system colors using the control panel.  Using GetSysBrush() or
// GetSysPen to get a system colored brush or pen will be more efficient than 
// creating the brushes and pens yourself.
//

#define nSysBrushes 25
#define nOurBrushes 4
static CBrush NEAR g_rgSysBrushes[nSysBrushes];
static CPen NEAR g_rgSysPens[nSysBrushes];

/* ResetSysBrush
 *	Deletes all of the system-colored brushes AND pens.
 */
void ResetSysBrushes()
{
    //NOTE: we don't include our extensions to the "system" brushes, because
    //  often the brush handle is used as hbrBackground for a Window class!

	for (UINT nBrush = 0; nBrush < nSysBrushes - nOurBrushes; nBrush++)
	{
		g_rgSysBrushes[nBrush].DeleteObject();
		g_rgSysPens[nBrush].DeleteObject();
	}
}

COLORREF MyGetSysColor(UINT nSysColor)
{
    if (nSysColor < CMP_COLOR_HILITE)
        return ::GetSysColor(nSysColor);

    static COLORREF rgColors[nOurBrushes] =
    {
        CMP_RGB_HILITE, CMP_RGB_LTGRAY, CMP_RGB_DKGRAY, CMP_RGB_BLACK,
    };

    ASSERT(nSysColor - CMP_COLOR_HILITE >= 0);
    ASSERT(nSysColor - CMP_COLOR_HILITE < nOurBrushes);
    return rgColors[nSysColor - CMP_COLOR_HILITE];
}

CBrush* GetSysBrush(UINT nSysColor)
{
    ASSERT(nSysColor < nSysBrushes);

    if (g_rgSysBrushes[nSysColor].m_hObject == NULL)
    {
        COLORREF cr = MyGetSysColor(nSysColor);
        if (!g_rgSysBrushes[nSysColor].CreateSolidBrush(cr))
        {
            // Woa!  GDI is really low on memory!  Flush our cache and
            // try again...

            ResetSysBrushes();

            if (!g_rgSysBrushes[nSysColor].CreateSolidBrush(cr))
            {
                TRACE("GetSysBrush failed!\n");
#ifdef _COMPOSER
                ((CTheApp*) AfxGetApp())->SetPaintEmergency();
#endif
            }
        }
    }

    return &g_rgSysBrushes[nSysColor];
}

CPen* GetSysPen(UINT nSysColor)
{
	ASSERT( nSysColor < nSysBrushes );

	if( g_rgSysPens[nSysColor].m_hObject == NULL )
	{
		COLORREF cr = MyGetSysColor(nSysColor);
		if( !g_rgSysPens[nSysColor].CreatePen(PS_SOLID, 0, cr) )
		{
			// Woa!  GDI is really low on memory!  Flush our cache and
			// try again...

			ResetSysBrushes();

			if( !g_rgSysPens[nSysColor].CreatePen(PS_SOLID, 0, cr) )
			{
				TRACE("GetSysPen failed!\n");
#ifdef _COMPOSER
			((CTheApp*) AfxGetApp())->SetPaintEmergency();
#endif
			}
		}
	}

	return &g_rgSysPens[nSysColor];
}

///////////////////////////////////////////////////////////////////////////////
//	Drawing helpers

void DrawSplitter(CDC* pdc, CRect rect, BOOL bVert)
{
	COLORREF colorLt = GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF colorDk = GetSysColor(COLOR_BTNSHADOW);

	if (!OnChicago())
	{
		pdc->FrameRect(rect, GetSysBrush(COLOR_WINDOWFRAME));	// Faster with FillSolidRect().

		rect.InflateRect(-1, -1);

		if (bVert)
		{
			pdc->FillSolidRect(rect.right - 1, rect.top, 1, rect.Height(), colorDk);
			pdc->FillSolidRect(rect.left, rect.top, 1, rect.Height(), colorLt);
		}
		else
		{
			pdc->FillSolidRect(rect.left, rect.bottom - 1, rect.Width(), 1, colorDk);
			pdc->FillSolidRect(rect.left, rect.top, rect.Width(), 1, colorLt);
		}
	}
	else
	{
		if (bVert)
		{
			// Cap the ends.
			pdc->FillSolidRect(rect.left, rect.top - 1, rect.Width(), 1, colorDk);
			pdc->FillSolidRect(rect.left, rect.bottom - 1, rect.Width(), 1, colorDk);
			pdc->FillSolidRect(rect.left, rect.top, rect.Width(), 1, colorLt);
			pdc->FillSolidRect(rect.left, rect.bottom, rect.Width(), 1, colorLt);

			// 3d edges.
			rect.InflateRect(0, -1);
			pdc->FillSolidRect(rect.left, rect.top, 1, rect.Height(), colorLt);
			pdc->FillSolidRect(rect.right - 2, rect.top, 1, rect.Height(), colorDk);
			pdc->FillSolidRect(rect.right - 1, rect.top, 1, rect.Height(),
				GetSysColor(COLOR_WINDOWFRAME));
			
			// Fill center.
			rect.InflateRect(-1, 0);
			pdc->FillSolidRect(rect.left, rect.top,	rect.Width() - 1, rect.Height(),
				GetSysColor(COLOR_BTNFACE));
		}
		else
		{
			// Cap the ends.
			pdc->FillSolidRect(rect.left - 1, rect.top, 1, rect.Height(), colorDk);
			pdc->FillSolidRect(rect.right - 1, rect.top, 1, rect.Height(), colorDk);
			pdc->FillSolidRect(rect.left, rect.top, 1, rect.Height(), colorLt);
			pdc->FillSolidRect(rect.right, rect.bottom, 1, rect.Height(), colorLt);

			// 3d edges.
			rect.InflateRect(-1, 0);
			pdc->FillSolidRect(rect.left, rect.top, rect.Width(), 1, colorLt);
			pdc->FillSolidRect(rect.left, rect.bottom - 2, rect.Width(), 1, colorDk);
			pdc->FillSolidRect(rect.left, rect.bottom - 1, rect.Width(), 1,
				GetSysColor(COLOR_WINDOWFRAME));

			// Fill center.
			rect.InflateRect(0, -1);
			pdc->FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height() - 1,
				GetSysColor(COLOR_BTNFACE));
		}
	}
}

#define CX_GAP (4)

HBITMAP CreateButtonBitmap(CWnd* pWnd, UINT nStyle,	LPCRECT lpRect,
	CBitmap* pbmGlyph /*=NULL*/, LPCTSTR lpszText /*=NULL*/, BOOL bAddArrow /*=FALSE*/)
{
	CWindowDC dcWnd(pWnd);

	CDC dc;
	if (!dc.CreateCompatibleDC(&dcWnd))
		return NULL;

	CRect rect(0, 0, 24, 22);
	if (theApp.m_bWin4)
		rect.right = 23;

	if (lpRect != NULL)
		rect = *lpRect;

	rect.OffsetRect(-rect.left, -rect.top);

	CBitmap bm;
	if (!bm.CreateCompatibleBitmap(&dcWnd, rect.Width(), rect.Height()))
		return NULL;

	CBitmap* pbmOld = dc.SelectObject(&bm);

#define BPR(clr)	dc.FillSolidRect(rect.left, rect.top,\
						rect.Width(), rect.Height(), GetSysColor(clr))

	if (!theApp.m_bWin4)
	{
		BPR(COLOR_WINDOWFRAME);

		::SetPixelV(dc.m_hDC, rect.left, rect.top, GetSysColor(COLOR_BTNFACE));
		::SetPixelV(dc.m_hDC, rect.left, rect.bottom - 1, GetSysColor(COLOR_BTNFACE));
		::SetPixelV(dc.m_hDC, rect.right - 1, rect.top, GetSysColor(COLOR_BTNFACE));
		::SetPixelV(dc.m_hDC, rect.right - 1, rect.bottom - 1, GetSysColor(COLOR_BTNFACE));

		rect.InflateRect(-1, -1);
	}

	if (nStyle & (TBBS_PRESSED | TBBS_CHECKED))
	{
		if (theApp.m_bWin4)
		{
			dc.Draw3dRect(rect, GetSysColor(COLOR_WINDOWFRAME),
				GetSysColor(COLOR_BTNHIGHLIGHT));
			rect.InflateRect(-1, -1);
			dc.Draw3dRect(rect, GetSysColor(COLOR_BTNSHADOW),
				GetSysColor(COLOR_BTNFACE));
			rect.InflateRect(-1, -1);
		}
		else
		{
			BPR(COLOR_BTNSHADOW);
			rect.left += 1; rect.top += 1;
		}

		if (nStyle & TBBS_PRESSED)
		{
			BPR(COLOR_BTNFACE);
		}
		else
		{
			CBrush* pbrOld = dc.SelectObject(CDC::GetHalftoneBrush());
			dc.SetTextColor(GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
			dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
			dc.SelectObject(pbrOld);
		}

		if (!theApp.m_bWin4)
			rect.InflateRect(-1, -1);	// for glyph positioning.
		else
		{
			rect.left++; rect.top++;
			rect.right++; rect.bottom++;
		}
	}
	else
	{
		if (theApp.m_bWin4)
		{
			dc.Draw3dRect(rect, GetSysColor(COLOR_BTNHIGHLIGHT),
				GetSysColor(COLOR_WINDOWFRAME));
			rect.InflateRect(-1, -1);
			dc.Draw3dRect(rect, GetSysColor(COLOR_BTNFACE),
				GetSysColor(COLOR_BTNSHADOW));
			rect.InflateRect(-1, -1);
		}
		else
		{
			BPR(COLOR_BTNSHADOW);
			rect.right--; rect.bottom--;
			BPR(COLOR_BTNHIGHLIGHT);
			rect.left++; rect.top++;
			BPR(COLOR_BTNSHADOW);
			rect.right--; rect.bottom--;
		}
		BPR(COLOR_BTNFACE);
	}

	if (lpszText != NULL)
	{
		int nLen = lstrlen(lpszText);

		dc.SetBkMode(TRANSPARENT);

		CFont* pFontOld = dc.SelectObject(GetStdFont(font_Normal));
		if (bAddArrow)
		{
			// get the ID of the arrow
			UINT idArrow;
			if (nLen <= 0) {
				idArrow = (nStyle & TBBS_DISABLED) ? IDB_MENUBTN_NOTEXT_ARROWD : IDB_MENUBTN_NOTEXT_ARROW;
			}
			else {
				idArrow = (nStyle & TBBS_DISABLED) ? IDB_MENUBTN_ARROWD : IDB_MENUBTN_ARROW;
			}
			// load the bitmap
			HINSTANCE hInstance = AfxFindResourceHandle(MAKEINTRESOURCE(idArrow), RT_BITMAP);
			ASSERT( hInstance != NULL );
			HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(idArrow), RT_BITMAP);
			HBITMAP hBitmap = AfxLoadSysColorBitmap(hInstance, hRsrc);
			CBitmap bmpArrow;
			bmpArrow.Attach(hBitmap);

			// figure the size of the bitmap
			BITMAP bmp;
			bmpArrow.GetObject(sizeof(bmp), &bmp);
			int xBitmap = bmp.bmWidth;
			int yBitmap = bmp.bmHeight;

			int cx = dc.GetTextExtent(lpszText, nLen).cx;
			cx += xBitmap;
			if (nLen > 0)
				cx += CX_GAP;

			rect.left += (rect.Width() - cx) / 2;

			// olympus 15814
			// text on disabled menu buttons not visible when bkgrnd color
			// matches gray text color
			// draw text on the button
			if (nStyle & TBBS_DISABLED) {
				// draw disabled text
				if (theApp.m_bWin4) {
					// on Win95, disabled text is drawn with a highlight under
					// and a shadow over
					dc.SetTextColor(GetSysColor(COLOR_BTNHIGHLIGHT));
					rect.OffsetRect(1, 1);
					dc.DrawText(lpszText, nLen, rect, DT_SINGLELINE | DT_VCENTER);
					dc.SetTextColor(GetSysColor(COLOR_BTNSHADOW));
					rect.OffsetRect(-1, -1);
					dc.DrawText(lpszText, nLen, rect, DT_SINGLELINE | DT_VCENTER);
				}
				else {
					DWORD dwGrayText = GetSysColor(COLOR_GRAYTEXT);
					if (dwGrayText == GetSysColor(COLOR_BTNFACE)) {
						// choose RGB(128, 128, 128) unless the button
						// face is RGB(128, 128, 128)
						dwGrayText = RGB(128, 128, 128);
						if (dwGrayText == GetSysColor(COLOR_BTNFACE)) {
							dwGrayText = RGB(192, 192, 192);
						}
					}
					dc.SetTextColor(dwGrayText);
					dc.DrawText(lpszText, nLen, rect, DT_SINGLELINE | DT_VCENTER);
				}
			}
			else {
				// draw normal text
				dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
				dc.DrawText(lpszText, nLen, rect, DT_SINGLELINE | DT_VCENTER);
			}

			// draw the arrow

			// create a DC and select the bitmap
			CDC dc2;
			dc2.CreateCompatibleDC( &dc );
			CBitmap *pOldBitmap = dc2.SelectObject(&bmpArrow);

			// draw the arrow
			int x = rect.left + cx - xBitmap;
			int y = rect.top + rect.Height() / 2 - yBitmap / 2;
			// move the arrow up one pixel if we're on Win95 and there's no text
			if (nLen <= 0 && theApp.m_bWin4)
				y--;
			dc.BitBlt(x, y, xBitmap, yBitmap, &dc2, 0, 0, SRCCOPY);

			dc2.SelectObject( pOldBitmap );

			// FUTURE: Random focus style value
			if (nStyle & 0x80)
			{
				CRect rcFocus(rect);
				rcFocus.left -= 2;
				rcFocus.right = rcFocus.left + cx + 4;
				//rcFocus.top -= 1;

				dc.DrawFocusRect(rcFocus);
			}
		}
		else
		{
			dc.DrawText(lpszText, -1, rect,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		dc.SelectObject(pFontOld);
	}
	else if (pbmGlyph != NULL)
	{
		CDC dcMask, dcMem;
		if (!dcMask.CreateCompatibleDC(&dcWnd) || !dcMem.CreateCompatibleDC(&dcWnd))
			goto leave_function;

		BITMAP bms;
		pbmGlyph->GetObject(sizeof (BITMAP), &bms);

		// Restrict blt to rect given.
		bms.bmWidth = min(bms.bmWidth, rect.Width());
		bms.bmHeight = min(bms.bmHeight, rect.Height());

		CBitmap bmMask;
		if (!bmMask.CreateBitmap(bms.bmWidth, bms.bmHeight, 1, 1, NULL))
			goto leave_function;

		CBitmap* pbmOldMask = dcMask.SelectObject(&bmMask);
		CBitmap* pbmOldMem = dcMem.SelectObject(pbmGlyph);

		// Calculate the mask (based on light gray).
		COLORREF oldBkColor = dcMem.SetBkColor(RGB(192, 192, 192));
		dcMask.BitBlt(0, 0, bms.bmWidth, bms.bmHeight, 
			&dcMem, 0, 0, NOTSRCCOPY);
		dcMem.SetBkColor(oldBkColor);

		// Paint the glyph.
		dc.SetBkColor(RGB(255, 255, 255));
		dc.SetTextColor(RGB(0, 0, 0));

		dc.BitBlt(rect.left + (rect.Width() - bms.bmWidth) / 2,
			rect.top + (rect.Height() - bms.bmHeight) / 2,
			bms.bmWidth, bms.bmHeight, &dcMem, 0, 0, DSx);

		dc.BitBlt(rect.left + (rect.Width() - bms.bmWidth) / 2,
			rect.top + (rect.Height() - bms.bmHeight) / 2,
			bms.bmWidth, bms.bmHeight, &dcMask, 0, 0, DSna);

		dc.BitBlt(rect.left + (rect.Width() - bms.bmWidth) / 2,
			rect.top + (rect.Height() - bms.bmHeight) / 2,
			bms.bmWidth, bms.bmHeight, &dcMem, 0, 0, DSx);

		dcMask.SelectObject(pbmOldMask);
		dcMem.SelectObject(pbmOldMem);
	}

leave_function:

	dc.SelectObject(pbmOld);

	return (HBITMAP) bm.Detach();
}

// Draws a bitmap without glyph suitable for use in a bar
HBITMAP CreateBarBitmap(CWnd* pWnd, UINT nStyle, LPCRECT lpRect, CSize sizeButton, CSize sizeImage)
{
	CWindowDC dcWnd(pWnd);

	CDC dc;
	if (!dc.CreateCompatibleDC(&dcWnd))
		return NULL;

	ASSERT(lpRect!=NULL);

	CRect rect = *lpRect;

	rect.OffsetRect(-rect.left, -rect.top);

	CBitmap bm;
	if (!bm.CreateCompatibleBitmap(&dcWnd, rect.Width(), rect.Height()))
		return NULL;

	CBitmap* pbmOld = dc.SelectObject(&bm);

	int dx = sizeButton.cx;
	int dy = sizeButton.cy;

	// interior grey
	dc.FillSolidRect(0, 0, dx, dy, globalData.clrBtnFace);

	// determine offset of bitmap (centered within button)
	CPoint ptOffset;
	ptOffset.x = (dx - sizeImage.cx - 1) / 2;
	ptOffset.y = (dy - sizeImage.cy) / 2;

	if (nStyle & (TBBS_PRESSED | TBBS_CHECKED))
	{
		// pressed in or checked
		dc.Draw3dRect(0, 0, dx, dy,
			globalData.clrBtnShadow, globalData.clrBtnHilite);

		// for any depressed button, add one to the offsets.
		ptOffset.x += 1;
		ptOffset.y += 1;
	}
	else if (nStyle & TBBS_EXTRUDED)
	{
		// regular button look
		dc.Draw3dRect(0, 0, dx, dy,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
	}

	dc.SelectObject(pbmOld);

	return (HBITMAP) bm.Detach();
}



///////////////////////////////////////////////////////////////////////////////
//
// ForceRectOnScreen
// [multimon] 11 mar 97 - der : This function was updated to handle multiple monitors.
//
void ForceRectOnScreen(CRect& rect)
{
	// Get the boundary rectangle of the screen which contains most of the past in
	// rectangles.
	CRect screenRect ;
	// Get monitor rect.
	multiMonitorRectFromRect(rect, &screenRect, c_fWork) ;

	// Check left and right boundaries.
	if (rect.left < screenRect.left)
	{
		rect.right += screenRect.left - rect.left;
		rect.left = screenRect.left ;
	}
	else if (rect.right > screenRect.right)
	{
		rect.left -= rect.right - screenRect.right;
		rect.right = screenRect.right;
	}

	// Check top and bottom boundaries.
	if (rect.top < screenRect.top)
	{
		rect.bottom += screenRect.top - rect.top;
		rect.top = screenRect.top ;
	}
	else if (rect.bottom > screenRect.bottom)
	{
		rect.top -= rect.bottom - screenRect.bottom;
		rect.bottom = screenRect.bottom;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// ForceRectBelowTop 
// [multimon] 11 mar 97 - der : This function was updated to handle multiple monitors.
//
void ForceRectBelowTop(CRect& rect)
{
	// Get monitor rect.
	RECT rectMonitor ;
	multiMonitorRectFromRect(rect, &rectMonitor, c_fWork) ;

	// Move below top of screen.
	if (rect.top < rectMonitor.top)
	{
		rect.bottom += rectMonitor.top - rect.top;
		rect.top = rectMonitor.top;
	}
}

SCREEN_METRIC g_mt;

void InitMetrics()
{
	extern void InitPaletteBorders();

	InitPaletteBorders();

    g_mt.cxBorder = GetSystemMetrics(SM_CXBORDER);
    g_mt.cyBorder = GetSystemMetrics(SM_CYBORDER);
    g_mt.cxFrame = GetSystemMetrics(SM_CXFRAME);
    g_mt.cyFrame = GetSystemMetrics(SM_CYFRAME);
    g_mt.cxToolBorder = 6;
    g_mt.cyToolBorder = 3;

	// use small caption buttons if screen is small
	int cyScreen= ::GetSystemMetrics(SM_CYSCREEN);
	int cxScreen= ::GetSystemMetrics(SM_CXSCREEN);

	g_mt.cxToolCaption = 6;
	g_mt.cyToolCaption = 6;
	// If we're in less than 800x600, use smaller caption buttons. Why am I using test values of less than 800x600? Sometimes
	// some versions of Windows want to return just less than the actual value for these values; perhaps because some utility
	// stole some screen area.
	if(	cyScreen<550 ||
		cxScreen<750)
	{
		g_mt.cxToolCapBorder = 2;
		g_mt.cyToolCapBorder = 2;
		g_mt.cxToolCapGap = 2;
		g_mt.cyToolCapGap = 2;
		g_mt.cxToolCapButton = 8;
		g_mt.cyToolCapButton = 8;
	}
	else
	{
		g_mt.cxToolCapBorder = 6;
		g_mt.cyToolCapBorder = 6;
		g_mt.cxToolCapGap = 2;
		g_mt.cyToolCapGap = 2;
		g_mt.cxToolCapButton = 12;
		g_mt.cyToolCapButton = 12;
	}
    g_mt.nSplitter = 4;
}

BOOL PollForMouseChange(CPoint& pt, BOOL bYield, BOOL& bCancel, BOOL bAllowPaint /*=FALSE */)
{
	MSG msg;
	BOOL bNewMessage;
	
    if (bNewMessage = ::PeekMessage(&msg, ::GetCapture(), 0, 0,
        (bYield ? PM_REMOVE : PM_NOYIELD | PM_REMOVE)))
	{
		if (msg.message == WM_LBUTTONUP)
		{
	        return TRUE;
	    }
	    else if (msg.message == WM_RBUTTONDOWN || ::GetCapture() == NULL ||
	            (GetAsyncKeyState(VK_ESCAPE) & 0x8000))
	    {
			HWND wnd=::GetCapture();
	        // Cancel the drag.
	        bCancel = TRUE;
	        return TRUE;
	    }
	    else if (msg.message != WM_MOUSEMOVE)
	    {
	        return FALSE;
	    }
		else	// NewMouse with MOUSEMOVE message.            
	    {
			pt = CPoint(msg.lParam);
	        ASSERT(::GetCapture() != NULL);
	    	::ClientToScreen(::GetCapture(), &pt);
		}
	}
	else if(bAllowPaint)
	{
		::RedrawWindow(AfxGetMainWnd()->GetSafeHwnd(),NULL, NULL, RDW_UPDATENOW|RDW_ALLCHILDREN);
	}
	
	return FALSE;
}

void FlushKeys()
{
	MSG msg;

    while (::PeekMessage(&msg, NULL,
    	WM_KEYFIRST, WM_KEYLAST, PM_NOYIELD | PM_REMOVE))
		;
}

/*** GETEXETYPE
 *
 * PURPOSE: Given an open handle to file determine it's executable type if any.
 *
 * INPUT:
 *      hFile - Handle to an open file
 *
 * OUTPUT:
 *      EXT   - EXecutable Type in return value
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *
 *      This function will save the file pointer and restore it on exit.
 *      A return type of EXE_NONE indicates either an error occured while
 *      seeking/reading or the file wasn't of any recognized EXE type.
 *
 *		If the EXE type is EXE_NT, then *pwSubsystem will indicate
 *		the subsystem type (IMAGE_SUBSYSTEM_...).  If the EXE type is
 *		NOT EXE_NT, the value *pwSubsystem on return is meaningless.
 *
 *		You can pass NULL for pwSubsystem, in which case no value will
 *		be returned through that pointer.
 *
 ****************************************************************************/
#define NE_UNKNOWN	0x0
#define NE_OS2		0x1
#define NE_WINDOWS	0x2
#define NE_DOS		0x3

EXT GetExeType
(
	LPCTSTR	lpstrEXEFilename,
	WORD *	pwSubsystem
)
{
    IMAGE_DOS_HEADER        doshdr;
    IMAGE_OS2_HEADER        os2hdr;
    IMAGE_NT_HEADERS        nthdr;
	HANDLE					hFile;
	BOOL					fGotNE = FALSE;
	DWORD					dwBytesRead;

	/*
	** open up our EXE
	*/
	hFile = CreateFile(
				lpstrEXEFilename,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 
				NULL
			);
	if (hFile == INVALID_HANDLE_VALUE) {
		return EXE_FAIL;
	}

    /*
    ** Try to read a an MZ header
    */
    ReadFile(hFile, &doshdr, sizeof (IMAGE_DOS_HEADER),	&dwBytesRead, NULL);

	if (dwBytesRead != sizeof (IMAGE_DOS_HEADER))
    {
		CloseHandle(hFile);
        return EXE_NONE;
    }

    /*
    ** If it was an MZ header and if the address of the relocation table
    ** isn't 0x0040 then it is a DOS non segmented executable.
    */
    if (doshdr.e_magic == IMAGE_DOS_SIGNATURE)
    {
    	if (doshdr.e_lfarlc != 0x0040)
    	{
			CloseHandle(hFile);
			return EXE_DOS;
		}
		else
			fGotNE = TRUE;
    }

    /*
	** Seek now to either the beginning of the file if there was no MZ stub or
	** to the new header specified in the MZ header.
    */
    if (SetFilePointer(hFile, fGotNE ? doshdr.e_lfanew : 0L, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
        return EXE_NONE;
	}

    ReadFile(hFile, &os2hdr, sizeof (IMAGE_OS2_HEADER), &dwBytesRead, NULL);
	if (dwBytesRead != sizeof (IMAGE_OS2_HEADER))
	{
		CloseHandle(hFile);
        return EXE_NONE;
    }

    if (os2hdr.ne_magic == IMAGE_OS2_SIGNATURE)
    {
		EXT	ext;

		switch(os2hdr.ne_exetyp)
		{
			case NE_OS2:
				ext = EXE_OS2_NE;
				break;

			case NE_WINDOWS:
				ext = EXE_WIN;
				break;

			case NE_DOS:
				ext = EXE_DOS;
				break;

			default:
				ext = EXE_NONE;
				break;
		}

		CloseHandle(hFile);
        return ext;
    }
    else if (os2hdr.ne_magic == IMAGE_OS2_SIGNATURE_LE)
    {
		EXT ext;

		ext = (os2hdr.ne_exetyp == NE_UNKNOWN) ? EXE_NONE : EXE_OS2_LE;

		CloseHandle(hFile);
        return EXE_OS2_LE;
    }


    /*
    ** Re-seek to the header, read it as an NT header and check for PE exe.
    */

    if (SetFilePointer(hFile, doshdr.e_lfanew, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
        return EXE_NONE;
    }

    ReadFile(hFile, &nthdr, sizeof (IMAGE_NT_HEADERS), &dwBytesRead, NULL);
	if (dwBytesRead != sizeof (IMAGE_NT_HEADERS))
    {
		CloseHandle(hFile);
        return EXE_NONE;
    }

    if (nthdr.Signature == IMAGE_NT_SIGNATURE)
    {
		// Return the subsystem as well.
		//
		// [CUDA 3557: 4/21/93 mattg]
		if (pwSubsystem != NULL)
		{
			if (nthdr.FileHeader.SizeOfOptionalHeader < IMAGE_SIZEOF_NT_OPTIONAL_HEADER)
				*pwSubsystem = IMAGE_SUBSYSTEM_UNKNOWN;
			else
				*pwSubsystem = nthdr.OptionalHeader.Subsystem;
		}
		CloseHandle(hFile);
        return EXE_NT;
    }


    /*
    ** Well, It isn't an Executable type that we know about....
    */

	CloseHandle(hFile);

    return EXE_NONE;

} /* GetExeType */

/*** GETEXEPLATFORM
 *
 * PURPOSE: Given the name of an executable file, return the associated 
 *			platform ID (win32x86, macppc, mac68k, etc)
 *
 * INPUT:
 *      szExeName - executable file whose platform ID will be returned
 *
 * RETURN:
 *      The platform ID of strExeName
 ****************************************************************************/
UINT GetExePlatform(LPCTSTR szExeName)
{
	HANDLE hfile;
	DWORD dwBytes;
	IMAGE_DOS_HEADER doshdr;
	IMAGE_FILE_HEADER PEHeader;
	DWORD dwMagic;

	if ((hfile = CreateFile(szExeName, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
		return unknown_platform;

	ReadFile(hfile, (LPVOID) &doshdr, sizeof(IMAGE_DOS_HEADER), &dwBytes, NULL);
	if (dwBytes != sizeof(IMAGE_DOS_HEADER))
	{
		CloseHandle(hfile);
		return unknown_platform;
	}

	SetFilePointer(hfile, doshdr.e_lfanew, NULL, FILE_BEGIN);
	ReadFile(hfile, (LPVOID) &dwMagic, sizeof(dwMagic), &dwBytes, NULL);
	if (dwBytes != sizeof(dwMagic) || dwMagic != IMAGE_NT_SIGNATURE)
	{
		CloseHandle(hfile);
		return unknown_platform;
	}

	ReadFile(hfile, (LPVOID) &PEHeader, sizeof(IMAGE_FILE_HEADER), &dwBytes, NULL);
	if (dwBytes != sizeof(IMAGE_FILE_HEADER))
	{
		CloseHandle(hfile);
		return unknown_platform;
	}

	CloseHandle(hfile);

	switch (PEHeader.Machine)
	{
		case IMAGE_FILE_MACHINE_I386:
			return win32x86;

		case IMAGE_FILE_MACHINE_M68K:
			return mac68k;

		case IMAGE_FILE_MACHINE_R3000:
		case IMAGE_FILE_MACHINE_R4000:
			return win32mips;
			
		case IMAGE_FILE_MACHINE_ALPHA:
			return win32alpha;

		case IMAGE_FILE_MACHINE_MPPC_601:
			return macppc;
			
		case IMAGE_FILE_MACHINE_UNKNOWN:
		default:
			return unknown_platform;
			break;
	}
}

BOOL IsFileGUIExe(LPSTR szFileName)
{
	BOOL		fRet;
	IMAGE_INFO	ImageInfo;
	
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szExt[_MAX_DIR];
	_splitpath((const TCHAR *)szFileName, szDrive, szDir, NULL, szExt);

	if (!*szDrive && !*szDir)
	{
		TCHAR szFullPath[_MAX_PATH], *psz;
		// check along the path for it.
		if (*szExt && SearchPath(NULL, szFileName, szExt, _MAX_PATH, szFullPath, &psz))
		{
			szFileName = szFullPath;
		}
	}

	fRet = FALSE;
	ImageInfo.Size = sizeof (ImageInfo);

	if (GetImageInfo (szFileName, &ImageInfo)) {

		if (ImageInfo.ImageType == IMAGE_TYPE_WIN32 &&
			ImageInfo.u.Win32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI) {

			fRet = TRUE;
		}
	}

	return fRet;
}


BOOL
IsExecutableFile(
	const TCHAR*	pszFileName
	)
/*++

Routine Description:

	This function will return TRUE if the file named by pszFileName is an NT
	style executable file (or DLL).  If specifically does not check to see
	if the file is compatible with the running installation of NT.

	Note: for our purposes a CrashDump file is considered an executable.  This
	used to accept Java .class files as executables -- it does so no more.

--*/
{
	BOOL		ret = FALSE;
	IMAGE_INFO	ImageInfo = {0};

	ImageInfo.Size = sizeof (ImageInfo);

	if (GetImageInfo (pszFileName, &ImageInfo)) {

		if (IS_WIN32_SUBSYSTEM_IMAGE (ImageInfo) ||
			ImageInfo.ImageType == IMAGE_TYPE_CRASHDUMP) {

			ret = TRUE;
		}
	}

	if(!ret) {
		/* Check for XBE file -- must have XBE extension and have XE
		 * signature */
		const TCHAR *pch;
		for(pch = pszFileName; *pch; ++pch);
		if(pch - pszFileName >= 4 && !_strnicmp(pch-4, ".xbe", 4)) {
			HANDLE hImage = CreateFile (pszFileName, GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
				NULL);
			if(INVALID_HANDLE_VALUE != hImage) {
				DWORD dwSig;
				DWORD cbRead;
				if(ReadFile(hImage, &dwSig, 4, &cbRead, NULL) && cbRead ==
						4 && dwSig == 0x48454258)
					ret = TRUE;
				CloseHandle(hImage);
			}
		}
	}

	return ret;
}




//////////////////////////////////////////////////////////////////////////////
// PlayDevSound
//  Plays the sound, if any, found in the registry event named by idsEvent.
//  Under Win95, this is assumed to be in a the subkey MSDev.

// Registry names are not localized?
#define WIN95_SOUND_LABELS			"AppEvents\\EventLabels"
#define WIN95_SOUND_APPS			"AppEvents\\Schemes\\Apps"
#define WIN95_SOUND_IDE				"MSDev"
#define WIN95_SOUND_IDE2			"Microsoft Developer"
#define WIN95_SOUND_CURRENT			".Current"
#define WINNT_SOUND_PREFIX			"MSDev: "
#define WINNT_SOUNDS				"Control Panel\\Sounds"

BOOL PlayDevSound(UINT idsEvent, DWORD dwFlags)
{
	if( idsEvent == 0 )
		return FALSE;
	CString str;
	str.LoadString(idsEvent);
	return PlayDevSound(str, dwFlags);
}

BOOL PlayDevSound(const char* szEvent, DWORD dwFlags)
{
	if( (szEvent == NULL) || (*szEvent == '\0') )
		return FALSE;

	if( theApp.m_bWin4 )
	{
		// Under Win95, you can have subkeys of sounds.  However, the names in 
		// those sub-keys are not accessable using sndPlaySound, so we get the
		// value from the sub-key ourselves, and call it by its filename.
		CString strKey = WIN95_SOUND_APPS;
		strKey += '\\';
		strKey += WIN95_SOUND_IDE;
		strKey += '\\';
		strKey += szEvent;
		strKey += '\\';
		strKey += WIN95_SOUND_CURRENT;

		BOOL bRet = FALSE;
		HKEY hKey;
		if( RegOpenKeyEx(HKEY_CURRENT_USER, (char*)(const char*)strKey, 0, 
			KEY_READ, &hKey) == ERROR_SUCCESS )
		{
			unsigned char abBuf[512];
			DWORD dwcb = 511;
			DWORD dwType = REG_SZ;
			if( (RegQueryValueEx(hKey, NULL, NULL, &dwType, abBuf, &dwcb)
				== ERROR_SUCCESS) && (dwcb > 0) )
			{
				bRet = ShsndPlaySound((char*)abBuf, dwFlags);
			}
			RegCloseKey(hKey);
		}
		return bRet;
	}
	else
	{
		// the NT 3.5 version of this API has a bug causing it to return TRUE 
		// *always* based on the existence of a sound driver (+card), so we'll
		// make a pre-check to see if the registry is set up with these sounds.
		BOOL fPreCheckOk = TRUE;
		if (!theApp.m_bWin4) 
		{
			// form our key
			HKEY hkey;
			fPreCheckOk = (RegOpenKeyEx(HKEY_CURRENT_USER, (LPCTSTR)"Control Panel\\Sounds", 0, KEY_READ, &hkey) == ERROR_SUCCESS);
			if (fPreCheckOk)
			{
				DWORD dwType, dwSize;
				fPreCheckOk = (RegQueryValueEx(hkey, (LPTSTR)szEvent, NULL, &dwType, (LPBYTE)NULL, &dwSize) == ERROR_SUCCESS);
				RegCloseKey(hkey);
			}
		}

		return fPreCheckOk && ShsndPlaySound(szEvent, dwFlags);
	}
}

static HMODULE h_winmm = NULL;
typedef BOOL (WINAPI *FPPLAYSOUND)(LPCTSTR,HMODULE,DWORD);
static FPPLAYSOUND ThePlaySound = NULL;

BOOL MyPlaySound(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound)
{
	if (h_winmm == NULL)
	{
		h_winmm = ::LoadLibrary("winmm.dll");
		if (h_winmm == NULL)
			return FALSE;
	}

	if (ThePlaySound == NULL)
	{
		ThePlaySound = (FPPLAYSOUND)::GetProcAddress(h_winmm, "PlaySoundA");
		if (ThePlaySound == NULL)
			return FALSE;
	}

	return ThePlaySound(pszSound, hmod, fdwSound);
}


typedef BOOL (WINAPI *FPSNDPLAYSOUND)(LPCTSTR,UINT);
static FPSNDPLAYSOUND TheSndPlaySound = NULL;

BOOL ShsndPlaySound(LPCSTR lpszSound, UINT fuSound)
{
	if (h_winmm == NULL)
	{
		h_winmm = ::LoadLibrary("winmm.dll");
		if (h_winmm == NULL)
			return FALSE;
	}

	if (TheSndPlaySound == NULL)
	{
		TheSndPlaySound = (FPSNDPLAYSOUND)::GetProcAddress(h_winmm, "sndPlaySoundA");
		if (TheSndPlaySound == NULL)
			return FALSE;
	}

	return TheSndPlaySound(lpszSound, fuSound);
}

//////////////////////////////////////////////////////////////////////////////
// RegisterDevSoundEvent
//	Adds a location in the registry of either Win95 or WinNT, that will show
//	up in the Control Panel's Sound utility.
//		idsEventName:		string ID of symbol used in registry
//		idsFriendlyName:	friendlier name that is displayed in the Sound util

BOOL RegisterDevSoundEvent(UINT idsEventName, UINT idsFriendlyName)
{
	CString strEvent, strFriendly;
	VERIFY( strEvent.LoadString(idsEventName) );
	VERIFY( strFriendly.LoadString(idsFriendlyName) );
	return RegisterDevSoundEvent(strEvent, strFriendly);
}

BOOL RegisterDevSoundEvent(const char* szEvent, const char* szFriendly)
{
	if( theApp.m_bWin4 )
	{
		// Open existing Labels keys
		HKEY hKeyLabels;
		long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, WIN95_SOUND_LABELS, 0, KEY_ALL_ACCESS/*NULL*/, &hKeyLabels) ;
		if ( lResult != ERROR_SUCCESS )
		{			
			return FALSE;
		}

		// Create a subkey under the Labels with the name of this event
		HKEY hKeyEvent;
		lResult = RegOpenKeyEx(hKeyLabels, szEvent, 0, KEY_ALL_ACCESS/*NULL*/, &hKeyEvent) ;
		if( lResult != ERROR_SUCCESS )
		{
			DWORD dw;
			lResult = RegCreateKeyEx(hKeyLabels, szEvent, 0, NULL, 0, KEY_ALL_ACCESS/*NULL*/, NULL, &hKeyEvent, &dw) ;
			if (lResult	!= ERROR_SUCCESS )
			{
				RegCloseKey(hKeyLabels);
				return FALSE;
			}
			VERIFY(RegSetValueEx(hKeyEvent, NULL, 0, REG_SZ, 
					(const unsigned char*)szFriendly, strlen(szFriendly) + 1) == ERROR_SUCCESS) ;
		}
		else
		{
			RegCloseKey(hKeyEvent);
		}
		RegCloseKey(hKeyLabels);

		// Open existing Apps keys
		HKEY hKeyApps;
		lResult = RegOpenKeyEx(HKEY_CURRENT_USER, WIN95_SOUND_APPS, 0, KEY_ALL_ACCESS/*NULL*/, &hKeyApps) ;
		if(lResult != ERROR_SUCCESS )
		{
			return FALSE;
		}

		// Create a subkey under the Apps for Developer
		HKEY hKeyDev;
		lResult = RegOpenKeyEx(hKeyApps, WIN95_SOUND_IDE, 0, KEY_ALL_ACCESS/*NULL*/, &hKeyDev) ;
		if( lResult != ERROR_SUCCESS )
		{
			DWORD dw;
			lResult = RegCreateKeyEx(hKeyApps, WIN95_SOUND_IDE, 0, NULL, 0, KEY_ALL_ACCESS/*NULL*/, NULL, &hKeyDev, &dw) ;
			if( lResult != ERROR_SUCCESS )
			{
				RegCloseKey(hKeyApps);
				return FALSE;
			}
			const char szIdeName[] = WIN95_SOUND_IDE2 ;
			lResult = RegSetValueEx(hKeyDev, NULL, 0, REG_SZ, 
									(const unsigned char*)szIdeName, strlen(szIdeName) + 1) ;
			ASSERT(lResult == ERROR_SUCCESS) ;
		}

		// Create a subkey under Developer for this Event
		lResult = RegOpenKeyEx(hKeyDev, szEvent, 0, KEY_ALL_ACCESS/*NULL*/, &hKeyEvent) ;
		if (lResult	!= ERROR_SUCCESS )
		{
			DWORD dw;
			lResult = RegCreateKeyEx(hKeyDev, szEvent, 0, NULL, 0, KEY_ALL_ACCESS/*NULL*/, NULL, &hKeyEvent, &dw) ;
			if ( lResult != ERROR_SUCCESS )
			{
				RegCloseKey(hKeyApps);
				RegCloseKey(hKeyDev);
				return FALSE;
			}
			lResult = RegSetValueEx(hKeyEvent, NULL, 0, REG_SZ, 
					(const unsigned char*)szFriendly, strlen(szFriendly) + 1);
			ASSERT( lResult == ERROR_SUCCESS) ;
			
		}
		else
		{
			RegCloseKey(hKeyEvent);
		}

		RegCloseKey(hKeyDev);
		RegCloseKey(hKeyApps);
	}
	else		// NT
	{
		HKEY hKeySounds;
		if( RegOpenKeyEx(HKEY_CURRENT_USER, WINNT_SOUNDS, 0,
				KEY_ALL_ACCESS, &hKeySounds) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		// Create a value under the Sounds with the name of this event
		DWORD dwType = REG_SZ;
		BYTE acData[256];
		DWORD dwcb = 256;
		if( RegQueryValueEx(hKeySounds, (char*)(const char*)szEvent, NULL, 
			&dwType, acData, &dwcb) != ERROR_SUCCESS )
		{
			CString strValue = ",";	// nothing before the comma means no sound
			strValue += WINNT_SOUND_PREFIX;		// add prefix to our name, to group and flag them
			strValue += szFriendly;
			if( RegSetValueEx(hKeySounds, szEvent, NULL, REG_SZ, 
				(const BYTE*)(const char*)strValue, (DWORD)strValue.GetLength() + 1)
				!= ERROR_SUCCESS )
			{
				RegCloseKey(hKeySounds);
				return FALSE;
			}
		}
		RegCloseKey(hKeySounds);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL InEmergencyState()
{
	return FALSE;
}

#ifdef _DEBUG
BOOL GetLogUndoFlag()
{
	return FALSE;
}
#endif

////////////////////////////////////////////////////////////////////////////
// Global utility functions
// True if the CCmdui is for any kind of context menu
BOOL IsContextMenu(CCmdUI* pCmdUI)
{

	if (pCmdUI->m_pMenu != NULL && pCmdUI->m_pMenu == pCmdUI->m_pParentMenu)
	{
		// this is a menu which is its own parent, which means
		// that it's a context menu.
		//
		return TRUE;
	}
	else if (pCmdUI->m_pOther != NULL)
	{
		// If this is a command bar popup but not a drop down,
		// then treat it as a context menu.
		//
		if (CBMenuPopup::IsCBMenuCmdUI(pCmdUI))
		{
			CBMenuPopup* pCBMenu = (CBMenuPopup*)pCmdUI->m_pOther;
	
			if (!pCBMenu->IsDropDown() && !pCBMenu->IsSubPopup())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// TRUE if this is the ccmdui for a menu
BOOL IsMenu(CCmdUI* pCmdUI)
{
	return	(pCmdUI->m_pParentMenu != NULL || pCmdUI->m_pMenu != NULL ||
			(pCmdUI->m_pOther != NULL && pCmdUI->m_pOther->IsKindOf(RUNTIME_CLASS(CBMenuPopup))));

}

void UpdateMenuCommand(CCmdUI *pCmdUI)
{
    // menus are enabled if they have any visible items
    CBMenuPopup *pMenu=theCmdCache.GetMenu(pCmdUI->m_nID);
    if(     pMenu &&
            pMenu->m_nVisibleItems>0)
    {
            pCmdUI->Enable(TRUE);
    }
    else
    {
            pCmdUI->Enable(FALSE);
    }
}

// convert the windows ansi code page to the iso code page
// used by the htmx and htm packages for the code page
// meta data html tag
BOOL AnsiCodePageToIsoCodePage(/*[in]*/ LPCTSTR szAnsi, /*[out]*/ CString *pstrIso)
{
	ASSERT(NULL != szAnsi && NULL != pstrIso);
	if(NULL == szAnsi || NULL == pstrIso)
		return FALSE;

	BOOL bReturn = TRUE;
	LPTSTR szUnused;

	// Convert to a number for comparison
	DWORD dwLocale = _tcstoul(szAnsi, (LPTSTR*)&szUnused, 10);
	switch(dwLocale)
	{
		case 1252:	// windows latin 1
			*pstrIso = _T("iso-8859-1");
			break;
		case 1250:	// windows latin 2 (eastern europe)
			*pstrIso = _T("windows-1250");
			break;
		case 1251:	// cyrillic (slavic)
			*pstrIso = _T("windows-1251");
			break;
		case 1253:	// greek
			*pstrIso = _T("windows-1253");
			break;
		case 1254:	// latin 5 (turkish) 
			*pstrIso = _T("windows-1254");
			break;
		case 1255:	// hebrew
			*pstrIso = _T("iso-8859-8");
			break;
		case 1256:	// arabic
			*pstrIso = _T("iso-8859-6");
			break;
		case 1257:	// latin4 or baltic
			*pstrIso = _T("windows-1257");
			break;
//		case 874:	// thai
//			break;
		case 932:	// japanese (shift jis)
			*pstrIso = _T("x-sjis");
			break;
		case 936:	// simplified chinese
			*pstrIso = _T("gb_2312-80");
			break;
		case 949:	// korean
			*pstrIso = _T("ks_c_5601-1987");
			break;
		case 950:	// traditional chinese
			*pstrIso = _T("x-x-big5");
			break;
		case 1200:
			*pstrIso = _T("unicode-1-1-utf-8");
//			*pstrIso = _T("unicode-2-0-utf-8");
			break;
		default:
			// unknown code page
			*pstrIso = _T("unknown");
			bReturn = FALSE;
			break;
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetFirstChildWindowWithID
//
//	This is a breath-first search for the particular ID.
//  It was created by modifying CWnd::GetDescendantWindow.
//
//

CPartView* GetFirstChildWindowWithID(HWND hWnd, int nID, BOOL bOnlyPerm)
{
	// GetDlgItem recursive (return first found)

	// use GetDlgItem since it is a fast USER function
	CPartView* pWndChild;
	HWND hWndChild = ::GetDlgItem(hWnd, nID) ;
	if (hWndChild != NULL)
	{
		if (bOnlyPerm)
		{
			// Return permanent handles only.
			pWndChild = (CPartView*)CWnd::FromHandlePermanent(hWndChild);
		}
		else
		{
			// Return temporary or permanent handles.
			pWndChild = (CPartView*)CWnd::FromHandle(hWndChild);
		}

		// Only return the window if it is a CPartView.
		if (pWndChild != NULL && pWndChild->IsKindOf(RUNTIME_CLASS(CPartView)))
		{
			return pWndChild;
		}
	}

	// walk each child
	for (hWndChild = ::GetTopWindow(hWnd); 
		hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		pWndChild = GetFirstChildWindowWithID(hWndChild, nID, bOnlyPerm);
		if (pWndChild != NULL)
		{
			return pWndChild;
		}
	}
	return NULL;    // not found
}

#if 0
CWnd* CheckNextLevel(HWND* hWnds, int count, int nID, int iLevel, BOOL bOnlyPerm) ;

CWnd* GetFirstChildWindowWithID(HWND hWnd, int nID, BOOL bOnlyPerm)
{
	CWnd* pWndChild = CheckNextLevel(&hWnd, 1, nID, 1, bOnlyPerm) ;
	return pWndChild ;
}

//
//
//
CWnd* CheckNextLevel(HWND* hWnds, int count, int nID, int iLevel, BOOL bOnlyPerm) 
{
	//TRACE("---> DESCENDING TO LEVEL %d.\r\n", iLevel) ;

	ASSERT(count>0) ;
	ASSERT(iLevel>0) ;

	HWND hWndChild = NULL ;
	CWnd* pWndChild = NULL ;

	// Check each window for the ID.
	for (int i = 0 ; i < count ; i++)
	{
		hWndChild = ::GetDlgItem(hWnds[i], nID) ;
		if ( hWndChild != NULL )
		{
			if (!bOnlyPerm)
			{
				pWndChild = CWnd::FromHandle(hWndChild);
			}
			else
			{
				pWndChild = CWnd::FromHandlePermanent(hWndChild);
			}

			// Only return the window if it is a CPartView.
			if (pWndChild != NULL && pWndChild->IsKindOf(RUNTIME_CLASS(CPartView)))
			{
				//* ---Start Cut
				//if (iLevel != 1)
					//TRACE("----> FOUND AT LEVEL %d.\r\n", iLevel) ;
				// *---End Cut
				return pWndChild;
			}
		}
	}

	// Build an array containing the children for each of these windows.
	const int cMaxIndex = 512 ;
	HWND hWndsNextLevel[cMaxIndex] ;
	int index = 0 ;

	for (i = 0 ; i < count && index < cMaxIndex ; i++)
	{
		for (	hWndChild = ::GetTopWindow(hWnds[i]); 
				hWndChild != NULL && index < cMaxIndex; 
				hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
			hWndsNextLevel[index++] = hWndChild ;
		}
	}

	ASSERT(index < cMaxIndex) ;

	if (index >0)
	{
		// Check the next level		
		return CheckNextLevel(hWndsNextLevel, index, nID, iLevel+1, bOnlyPerm) ;
	}
	else
	{
		// Done
		//TRACE0("----> WINDOW NOT FOUND!\r\n") ;
		return NULL ;
	}
}

#endif
