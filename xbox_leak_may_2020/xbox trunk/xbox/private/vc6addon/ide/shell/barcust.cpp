///////////////////////////////////////////////////////////////////////////////
//  BARCUST.CPP
//      Contains implementation of the CCustomBar class
///////////////////////////////////////////////////////////////////////////////

/* 
This class implements all of the customisation and command bar functionality for command bars. You should use it for all
bars which are not to participate in the docking windows. Docking is handled by a subclass - CASBar.

It inherits from the CToolBar class, which implements a manually drawn command bar style toolbar, with no diversity of
button types (ordinary only). 

When most of the CCustomBar class was implemented, we had made a decision to avoid modifications to oldbar.cpp, to ease
reintegration with MFC's version of this class at some later date. This explains much of the design rationale for CCustomBar.

When we decided to do the command bar look for toolbars, it became clear that we would not be able to maintain this rule for
the drawing code, and substantial modifications were made to the oldbar.cpp code. 

Important members added by this class

m_sizeMaxLayout is the maximum width/height of the bar, in horizontal and vertical postures
m_sizeFixedLayout is the minimum height/width of the bar in horizontal and vertical postures, except for vertical docking

A max width, min height bar (e.g. docked at top) is m_sizeMaxLayout.cx, and m_sizeFixedLayout.cy;
A min width, max height bar (e.g. floating thin) is m_sizeMaxLayout.cy, and m_sizeFixedLayout.cx;

Vertical bars must are always very thin, and have their window controls drawn in a special way.

m_pButtonExtra is allocated on demand. See btnextra.h for a description

CCustomBarProxy m_Proxy;

Proxies are described more fully in the header file barprxy.h

MDI Child System Menu Implementation
------------------------------------

When we are MDI hosting we allow an extra menu to be pulled from the main menu bar. That menu is given the s_nDroppedItem
value equal to m_nCount. It is a constructed command bar menu.
Actually, I'm fairly unhappy with the way I chose to implement this - I think that a set of special buttons
might have been simpler, though there would still have been a number of special cases.

Use of FIND_SLOW_CMDUI
----------------------

This will help when searching for slow cmdui update handlers in the product. The code tracks the longest cmdui handler, and emits a 
trace message for any handler that is within 90% of the current highest known value. There are two separate defines, one for menus,
in cbmenu.cpp, and one for toolbars, in barcust.cpp. 

The main problem with this code is that once a very slow cmdui handler is hit, other handlers may not trace any more because the maximum 
has become too high. Things like the file menu MRU handlers (which are very slow) can cause this problem.

*/

#include "stdafx.h"

#include "shell.h"
#include "barcust.h"
#include "bardlgs.h"
#include "barglob.h"
#include "ipchost.h"
#include "shlmenu.h"
#include "custbtn.h"
#include "shell.h"
#include "menuprxy.h"
#include "prxycust.h"

IMPLEMENT_DYNAMIC(CCustomBar, CToolBar);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define AUTOUPDATE_BUTTONS

// Define this to one to trace on Cmdui handlers that are in the slowest 10% of the range
// See above for an explanation of this define
#ifdef _DEBUG
#define FIND_SLOW_CMDUI 0
#else
#define FIND_SLOW_CMDUI 0
#endif

// descriptions of these messages are in barcust.h
UINT DSM_BARSELECTCHANGING = RegisterMessage("DevStudioToolbarSelectionChanging");
extern UINT DSM_DROPMENU = RegisterMessage("DevStudioDropPendingMenu");

int CCustomBar::s_nDroppedItem=-1;
CCustomBar* CCustomBar::s_pDroppedBar=NULL;
CBMenuPopup *CCustomBar::s_pSystemMenu=NULL;
CPoint CCustomBar::s_ptLastMove(0,0);

// TRUE if, during a cmdui update, an enable happened in a bar (the menu bar) that needs a recalclayout after cmdui. This happens
// when things have lost/got the invisible bit.
BOOL CCustomBar::s_bRecalcLayoutPending=FALSE;

///////////////////////////////////////////////////////////////////////////////
//      CCustomBar
//              Derived from MFC CToolBar, this enhancement allows functionality such
//              as toolbar sizing, standard for HWNDs in toolbars, and customizing.
//              providing a base class for both CASBar and the toolbars in the VRES
//              toolbar editors.
//

// static data members.

HCURSOR CCustomBar::s_hcurVSize = NULL;

//      CCustomBar::CCustomBar
//      Construction.
CCustomBar::CCustomBar()           
{
	// Attach the proxy.
	m_Proxy.AttachBar(this);

	if (s_hcurVSize == NULL)
	VERIFY(s_hcurVSize = AfxGetApp()->LoadCursor(IDCUR_VSIZE));

	SetDefaultSizes(theApp.m_bLargeToolBars);

	m_cyDefaultGap = m_cxDefaultGap / 2;    // MFC sets m_cxDefaultGap.
	m_cyTopBorder = m_cyBottomBorder = 1; //g_mt.cyToolBorder;
	m_cxLeftBorder = m_cxRightBorder = 1; //g_mt.cxToolBorder;
	m_glyphStatus=cGlyphsAbsent;
	CSize sizeButton=GetButtonSize(orNil);
	m_sizeFixedLayout.cx = GetLeftBorder(orNil) + GetRightBorder(orNil) +
		sizeButton.cx + g_mt.cxBorder * 2;
	m_sizeFixedLayout.cy = GetTopBorder(orNil) + GetBottomBorder(orNil) +
		sizeButton.cy + g_mt.cyBorder * 2;
		
	m_pButtonExtra = NULL;
	m_nTiles = 0;
	m_hIconMDI=NULL;
	m_nMDITrack=cMDINotDrawn;
}

//      CCustomBar::~CCustomBar
//              Destructor.

CCustomBar::~CCustomBar()
{
	RemoveExtra();
	Empty();

	// bar can't be destroyed while system menu is down, so destroy it
	if(s_pSystemMenu)
	{
		delete s_pSystemMenu;
		s_pSystemMenu=NULL;
	}
	if(m_hIconMDI)
	{
		::DestroyIcon(m_hIconMDI);
		m_hIconMDI=NULL;
	}
}

BOOL CCustomBar::Create(DWORD dwStyle, const CRect& rect,
	CWnd* pParent, UINT nID)
{
	// Use RecalcLayout to update size after setting buttons and bitmap.
	//
	CRect rectInit(rect.top, rect.left,
		rect.top, rect.left);
    
	dwStyle |= (WS_CHILD | DWS_SIZABLE);
	m_dwStyle = dwStyle & ~CBRS_GRIPPER; // No MFC Gripper bar, we're doing our own.

    if (pParent == NULL ||
	!CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
			  ::LoadCursor(NULL, IDC_ARROW),
			  (HBRUSH) (COLOR_BTNFACE + 1)),
		"",
			dwStyle,
			rect,
			pParent,
			nID))
	{
	return FALSE;
	}
	
	return TRUE;
}

// Like SetSize, but without any redraw action.
void CCustomBar::SetDefaultSizes(BOOL bLarge)
{
	if(!bLarge)
	{
		CToolBar::SetDefaultSizes();
	}
	else
	{
		// UISG standard large sizes
		m_sizeButton=GetDefaultButtonSize(TRUE);
		m_sizeImage=GetDefaultBitmapSize(TRUE);
		m_cyTopBorder = m_cyBottomBorder = 4;

		// adjust sizes when running on Win4
		if (globalData.bWin4)
		{
			m_sizeButton.cx = 39;
			m_cySharedBorder = m_cxSharedBorder = 0;
			m_cxDefaultGap = 8;
		}
		else
		{
			m_cxDefaultGap = 6;
			m_cySharedBorder = m_cxSharedBorder = 1;
		}
	}
}

CSize CCustomBar::GetDefaultBitmapSize(BOOL bLarge)
{
	if(!bLarge)
	{
		return CToolBar::GetDefaultBitmapSize();
	}
	else
	{
		return globalData.csLargeButtonGlyph;
	}
}

CSize CCustomBar::GetDefaultButtonSize(BOOL bLarge)
{
	if(!bLarge)
	{
		return CToolBar::GetDefaultButtonSize();
	}
	else
	{
		return globalData.csLargeButton;
	}
}

BOOL CCustomBar::LoadBitmap(LPCSTR lpszResourceName, BOOL bUpdateTiles /*=FALSE*/)
{
	BOOL bRet = CToolBar::LoadBitmap(lpszResourceName);
	if (bUpdateTiles)
		m_nTiles = CountTiles();

	return bRet;
}

void CCustomBar::SetBitmap(HBITMAP hbmImageWell, BOOL bUpdateTiles /*=FALSE*/)
{
	if (m_hbmImageWell != NULL)
		::DeleteObject(m_hbmImageWell);     // get rid of old one

	m_hbmImageWell = hbmImageWell;
	if (bUpdateTiles)
		m_nTiles = CountTiles();
}

BOOL CCustomBar::SetButtons(const UINT FAR* lpIDArray, int nIDCount)
{
	if (lpIDArray != NULL)  
		RemoveExtra();
	Empty();        // Note: Destroys the image well.

	if (!CToolBar::SetButtons(lpIDArray, nIDCount))
		return FALSE;

	SetGlyphStatus(cGlyphsUnknown);

	return TRUE;
}

int CCustomBar::CountTiles()
{
	BITMAP bmStruct;
	if (m_hbmImageWell == NULL ||
		::GetObject(m_hbmImageWell, sizeof(BITMAP), &bmStruct) == 0)
		return 0;

	return bmStruct.bmWidth / m_sizeImage.cx;
}

ORIENTATION CCustomBar::GetOrient() const
{
	return orHorz;
}

HWND CCustomBar::GetControl(UINT nID)
{
	return NULL;
}

CCustomBar* CCustomBar::NewBar(UINT nID, const CPoint& pt)
{
	return NULL;
}

void CCustomBar::MoveBar(const CRect& rect)
{
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);    
}

void CCustomBar::OnSelChange(int nIndex)
{
}

void CCustomBar::OnDeleteSelection()
{
}

void CCustomBar::OnBarActivate()
{
}

void CCustomBar::OnButtonSize()
{
}

BOOL CCustomBar::IsStaticItem(int nIndex)
{
	return FALSE;
}

void CCustomBar::RemoveControl(HWND hwndRemove)
{
	::DestroyWindow(hwndRemove);
}

//      CCustomBar::GetHSize
//              Gets the horizontal dock snap size (all buttons in one row).

CSize CCustomBar::GetHSize(ORIENTATION or)
{
	CSize size(m_sizeMaxLayout.cx, m_sizeFixedLayout.cy);

	return size;
}

//      CCustomBar::GetVSize
//              Gets the vertical dock snap size (all buttons in one column).
//              Returns null size if can't put all buttons in one column, because
//              of an oversized HWND.

CSize CCustomBar::GetVSize()
{
    CSize size;

	size = Fit(GetVWidth(), orVert);
    
    return size;
}

//      CCustomBar::GetVWidth
//              Gets the vertical dock snap width (all buttons in one column).

int CCustomBar::GetVWidth() const
{
	CSize sizeButton=GetButtonSize(orVert);

	return GetLeftBorder(orVert) + GetRightBorder(orVert) + sizeButton.cx + g_mt.cxBorder * 2;
}

//      CCustomBar::GetSize
//              Given a size and a HitTest code, returns the best fit size restricted
//              on either width or height, depending on the HitTest code.

CSize CCustomBar::GetSize(UINT nHitTest, CSize size)
{
	int nLeft, nRight, nMid;
	
	nLeft = m_sizeFixedLayout.cx;

	ORIENTATION or=GetOrient();

	// can't have floating windows larger than screen - windows enforces this, so we have to
	BOOL bSizeLimit=(or==orNil); // true if size limit should be enforced

	CSize sizeLimit(0,0);

	if(bSizeLimit)
	{
		CWnd *pDesktop=GetDesktopWindow();
		ASSERT(pDesktop!=NULL);
		CRect rectDesktop;
		pDesktop->GetWindowRect(rectDesktop);
		sizeLimit=rectDesktop.Size();

		ASSERT(sizeLimit.cx!=0 && sizeLimit.cy!=0);
	}
	
	switch (nHitTest)
	{
	case HTTOP:
	case HTBOTTOM:
		nRight = m_sizeMaxLayout.cx;
		break;
		
	case HTLEFT:
	case HTRIGHT:
		nRight = max(size.cx, m_sizeFixedLayout.cx);
		size.cy = Fit(size.cx, orNil).cy;       // use the fit height
		break;
		
	default:
		ASSERT(FALSE);
		break;
	}

	// Binary search for the pixel farthest left (smallest width) that
	// keeps the height less than or equal to size.cy.
	//
	while (nLeft < nRight)
	{
		nMid = (nLeft + nRight) / 2;
			
		if (Fit(nMid, orNil).cy > size.cy)
			nLeft = nMid + 1;
		else if (nMid > nLeft && Fit(nMid - 1, orNil).cy <= size.cy)
			nRight = nMid - 1;
		else
			nLeft = nRight = nMid;
	}
		
	if(bSizeLimit)
	{
		ASSERT(sizeLimit.cx!=0 && sizeLimit.cy!=0);
		if(nRight > sizeLimit.cx)
		{
			// if we exceed the screen width, then we can no longer treat this bar as a single line one, and must
			// instead use the same method as for HTRIGHT
			if(nHitTest==HTTOP || nHitTest==HTBOTTOM)
			{
				nRight = max(size.cx, m_sizeFixedLayout.cx);
			}
			else
			{
				nRight=Fit(sizeLimit.cx,orNil).cx;
			}
		}
	}
	
	size.cx = nRight;
	size.cy = Fit(size.cx, orNil).cy;
	
	return size;
}

//      CCustomBar::RecalcSizes
//              Recalculates the various layout sizes.  Used when the button layout
//              changes.

void CCustomBar::RecalcSizes()
{
	// minimum acceptable width, used for calculating maximum height
	int xMinWidth;

	// Start with defaults.
	CSize sizeButton=GetButtonSize(orNil);

	m_sizeFixedLayout.cx = GetLeftBorder(orNil) + GetRightBorder(orNil) +
		sizeButton.cx + g_mt.cxBorder * 2;
	m_sizeFixedLayout.cy = GetTopBorder(orNil) + GetBottomBorder(orNil) +
		sizeButton.cy + g_mt.cyBorder * 2;
		

	if (m_nCount == 0)
	{
		m_sizeMaxLayout = m_sizeFixedLayout;
		return;
	}

	xMinWidth=m_sizeFixedLayout.cx;

	m_sizeMaxLayout.cx = GetLeftBorder(orHorz) + g_mt.cxBorder;

	if(IsMDIHost())
	{
		// caption button width
		m_sizeFixedLayout.cx=max(m_sizeFixedLayout.cx, GetLeftBorder(orNil) + GetRightBorder(orNil) +
				2*globalData.csCaptionButton.cx + g_mt.cxBorder * 2);
		// small icon width
		m_sizeFixedLayout.cx=max(m_sizeFixedLayout.cx, GetLeftBorder(orNil) + GetRightBorder(orNil) +
				globalData.csSmallIcon.cx + g_mt.cxBorder * 2);

		// altogether
		m_sizeMaxLayout.cx+=GetLeftMDIArea(orNil)+GetRightMDIArea(orNil);
	}
		
	BOOL bInGroup = FALSE;
	CRect rectTmp;
    AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;

    for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
    {
		if(pTBB->nStyle & TBBS_INVISIBLE)
		{
			continue;
		}

		if (pTBB->nStyle & TBBS_SEPARATOR)
		{
			bInGroup = FALSE;
			m_sizeMaxLayout.cx += m_cxDefaultGap;
		}
		else if (pTBB->nStyle & TBBS_HWND)
		{
			::GetWindowRect(GetButtonExtra(iButton)->GetControl(), rectTmp);

			m_sizeMaxLayout.cx += rectTmp.Width() - m_cxSharedBorder;
			m_sizeFixedLayout.cx = max(m_sizeFixedLayout.cx,
				GetLeftBorder(orNil) + GetRightBorder(orNil) +
				rectTmp.Width() + g_mt.cxBorder * 2);
		}
		else
		{
			// buttons can have text, glyph, or both
			CSize sizeContent;

			// start by working out the size of the content of this button
			if (pTBB->nStyle & TBBS_TEXTUAL) {
				// measure string accounts for the glyph
				MeasureString(iButton, sizeContent, orHorz);
			} else {
				sizeContent=m_sizeImage;
			}

			// calculate extra width added by internal blank area around text
			int internalBorder=sizeButton.cx-m_sizeImage.cx - m_cxSharedBorder;

			// calculate potential width
			m_sizeMaxLayout.cx+=sizeContent.cx + internalBorder;

			// calculate minimum width
			m_sizeFixedLayout.cx = max(     m_sizeFixedLayout.cx, GetLeftBorder(orNil) + GetRightBorder(orNil) + 
										sizeContent.cx + internalBorder + g_mt.cxBorder * 2);
		}       
		
		if (!bInGroup)
		{
			// no overlap for first button in a group
			bInGroup = TRUE;
			m_sizeMaxLayout.cx += m_cxSharedBorder;
		}        
	}       // for

	m_sizeMaxLayout.cx += GetRightBorder(orNil) + g_mt.cxBorder;    // no overlap on last button

	// martynl: We must preinit maxLayout.cy here because Fit relies on it being set to something
	// sensible.
	m_sizeMaxLayout.cy = GetTopBorder(orVert) +  g_mt.cyBorder + sizeButton.cy;
	m_sizeMaxLayout.cy = Fit(xMinWidth, orVert).cy;
}

// Warning: GetTextExtent returns the width of the text in cx, even for a vertical font. 
// martynl 06May96
// Its funny how I wanted GetTextExtent to work the way its does, but someone else didn't....
// :-) der
// That just means you have a twisted (or should I say rotated?) mind... :) martynl

// We are now caching the length of the string in a button extra field. If the cached values are negative, then they are
// for a vertical bar. We always update the cache. This caching avoids dcs, fonts, ampersand stripping. Should be a big
// performance win. The cost is a dword per bar item, once window, textual or customglyph buttons are added to the bar.
// martynl 25Jun96
void CCustomBar::MeasureString(int iButton, CSize &stringSize, ORIENTATION or)
{
    AFX_TBBUTTON* pTBB = _GetButtonPtr(iButton);

	ASSERT(pTBB->nStyle & TBBS_TEXTUAL);

	// if there are text buttons, then the bar must have extra info
	ASSERT(HasButtonExtra());

	CSize sizeLabel=GetButtonExtra(iButton)->GetLabelSize();

	if( (sizeLabel.cx<0 && or!=orVert) ||
		(sizeLabel.cx>0 && or==orVert))
	{
		sizeLabel=CSize(0,0);
	}
	if(sizeLabel.cx==0)
	{
		// determine size of string if drawn in that font
		CDC *pDC=GetDC();               // dev context of the toolbar

		// select the button font
		CFont *pOldFont=pDC->SelectObject(GetTextFont(or));

		sizeLabel=globalData.GetTextExtentNoAmpersands(pDC, GetButtonExtra(iButton)->GetLabel());

		// clean up context
		pDC->SelectObject(pOldFont);
		VERIFY(ReleaseDC(pDC));

		if(or!=orVert)
		{
			EditButtonExtra(iButton)->SetLabelSize(sizeLabel);
		}
		else
		{
			EditButtonExtra(iButton)->SetLabelSize(CSize(-sizeLabel.cx, -sizeLabel.cy));
		}
	}
	else
	{
		if(or==orVert)
		{
			sizeLabel.cx=-sizeLabel.cx;
			sizeLabel.cy=-sizeLabel.cy;
		}
	}

	if(or==orVert)
	{
		stringSize.cx=sizeLabel.cy;
		stringSize.cy=sizeLabel.cx;

		stringSize.cy+=cExtraTextBorder;
	}
	else
	{
		stringSize=sizeLabel;

		stringSize.cx+=cExtraTextBorder;
	}

	// if it's a menu not on the main menu bar, it gets a dropdown (or dropright) arrow
	if( (pTBB->nStyle & TBBS_MENU)!=0 &&
		(GetBarStyle() & CTBRS_MAINMENU)==0)
	{
		HBITMAP hbmArrow=GetMenuArrow(or);

		// figure the size of the bitmap
		BITMAP bmp;
		::GetObject(hbmArrow, sizeof(bmp), &bmp);

		ASSERT(bmp.bmWidth!=0);
		ASSERT(bmp.bmHeight!=0);

		if(or==orVert)
		{
			stringSize.cy+=bmp.bmHeight;
			stringSize.cx=max(stringSize.cx, bmp.bmWidth);
		}
		else
		{
			stringSize.cx+=bmp.bmWidth;
			stringSize.cy=max(stringSize.cy, bmp.bmHeight);
		}
	}

	if(pTBB->nStyle & TBBS_GLYPH)
	{
		if(or==orVert)
		{
			stringSize.cy+=m_sizeImage.cy;
			stringSize.cx=max(stringSize.cx, m_sizeImage.cx);
		}
		else
		{
			stringSize.cx+=m_sizeImage.cx;
			stringSize.cy=max(stringSize.cy, m_sizeImage.cy);
		}
	}
}

//      CCustomBar::RecalcLayout
//              Given a HitTest code (specifies whether to ristrict resizing by height,
//              or width), resizes the toolbar to accomadate button layout changes.

void CCustomBar::RecalcLayout(UINT nHTSize /*=HTRIGHT*/, ORIENTATION *pOr)
{
	// removed by martynl becuase the code (appears to) work fine in a dialog,
	// and is needed for the customize button dialog. martynl 27Mar96
	// ASSERT(!IsInDialog());
	
	ORIENTATION or;
	if(pOr)
	{
		or=*pOr;	
	}
	else
	{
		or=GetOrient();
	}
	
	CSize size;
	CRect rect;
	GetWindowRect(rect);

	RecalcSizes();
	
	CSize sizeWindow=rect.Size();

	switch (or)
	{
	case orHorz:
		size = GetHSize(or);
		break;

	case orVert:
		size = GetVSize();
		break;
	
	case orNil:
		size = GetSize(nHTSize, sizeWindow);
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	
	BOOL bSameSize=FALSE;

	// If it's a menu bar, the dock manager will be adjusting it if it's docked, so we only compare the other dimension
	if((m_dwStyle & CTBRS_MAINMENU)!=0 &&
		or!=orNil)
	{
		if(or!=orHorz)
		{
			bSameSize=(size.cx==sizeWindow.cx);
		}
		else
		{
			// If this is a docked menu, then the size we calculated above isn't going to be right for the case where
			// the bar has just wrapped to two lines, since Get(H/V)Size assume no wrapping. So we work out whether a wrap will
			// happen. The actual wrapping is done in the dock manager when it gets the MoveBar below.
			CSize sizeFit=Fit(sizeWindow.cx, orHorz);

			bSameSize=(	sizeFit.cy==sizeWindow.cy+1 ||
						sizeFit.cy==sizeWindow.cy); // Nasty hack. Why the plus one? Somewhere, sometimes, a -1 is applied to window sizes when docked
		}
	}
	else
	{
		bSameSize=(size == sizeWindow);
	}

	if (bSameSize)
	{
		// Send a size message to ensure correct placement of Hwnd controls.
		SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(size.cx, size.cy));
		InvalidateRect(NULL);
		UpdateWindow();
	}
	else
	{
		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;
		MoveBar(rect);
	}
}

//      CCustomBar::ShiftImage
//              Given and image index, a number of tiles to shift, and an array of
//              new IDs (if the number of tiles is positive), Shifts the tiles of
//              the current image well at the given index, and inserts new tiles
//              if necessary.

BOOL CCustomBar::ShiftImage(int nImage, int nTiles,
	TRANSFER_TBBUTTON* pTrans /*=NULL*/)
{
	ASSERT(nTiles == 1 || nTiles == -1);

	HDC hdcClient = ::GetDC(m_hWnd);
	HDC hdcDest = ::CreateCompatibleDC(hdcClient);
	HDC hdcSrc = ::CreateCompatibleDC(hdcClient);
		
	HBITMAP hbmNewWell = ::CreateCompatibleBitmap(hdcClient,
		(m_nTiles + nTiles) * m_sizeImage.cx, m_sizeImage.cy);
	::ReleaseDC(m_hWnd, hdcClient);
	
	if (hbmNewWell == NULL || hdcDest == NULL || hdcSrc == NULL)
	{       
		// May RIP.
		::DeleteObject(hbmNewWell);
		::DeleteDC(hdcSrc);
		::DeleteDC(hdcDest);
		return FALSE;
	}
		
	HBITMAP hbmOldDest = (HBITMAP) ::SelectObject(hdcDest, hbmNewWell);
	HBITMAP hbmOldSrc = (HBITMAP) ::SelectObject(hdcDest, hbmNewWell);      // Get old bitmap.
	if (m_hbmImageWell != NULL)
		hbmOldSrc = (HBITMAP) ::SelectObject(hdcSrc, m_hbmImageWell);
	
	// Blt the image well up to the given index into the new bitmap.
	CRect rect(0, 0, nImage * m_sizeImage.cx, m_sizeImage.cy);
		
	if (!rect.IsRectEmpty())
	{
		::BitBlt(hdcDest, rect.left, rect.top, rect.Width(), rect.Height(),
			hdcSrc, rect.left, rect.top, SRCCOPY);
	}
	
	// Blt the rest of the image well to its new location (overlaps first
	// Blt if nTiles is negative).
	rect.left = rect.right;
	if (nTiles > 0)
		rect.left += nTiles * m_sizeImage.cx;
		
	rect.right = (m_nTiles + nTiles) * m_sizeImage.cx;
		
	if (!rect.IsRectEmpty())
	{
		::BitBlt(hdcDest, rect.left, rect.top, rect.Width(), rect.Height(),
			hdcSrc, rect.left - nTiles * m_sizeImage.cx, rect.top, SRCCOPY);
	}

	if (nTiles > 0 && pTrans!=NULL)
	{
		// load the appropriate bitmap, if required
		pTrans->LoadCommandBitmap(theApp.m_bLargeToolBars);

		// Blt new command glyph.

		// This code used to assert that pTrans must be non-NULL. Now, however,
		// shiftimage is used by MakeBitmap to return a toolbar button to
		// its native state. In this case, no pTrans is used.

		// If old glyph is too small, fill new location with background.
		if (m_sizeImage.cx > pTrans->sizeImage.cx ||
			m_sizeImage.cy > pTrans->sizeImage.cy)
		{
			CBrush* pbr = GetSysBrush(CMP_COLOR_LTGRAY);
			HBRUSH hbrOld = (HBRUSH) ::SelectObject(hdcDest, pbr->m_hObject);
			::PatBlt(hdcDest, m_sizeImage.cx * nImage, 0,
				m_sizeImage.cx, m_sizeImage.cy, PATCOPY);
			::SelectObject(hdcDest, hbrOld);
		}

		int xShiftNew = max(0, (m_sizeImage.cx - pTrans->sizeImage.cx) / 2);
		int yShiftNew = max(0, (m_sizeImage.cy - pTrans->sizeImage.cy) / 2);
		int xShiftCur = max(0, (pTrans->sizeImage.cx - m_sizeImage.cx) / 2);
		int yShiftCur = max(0, (pTrans->sizeImage.cy - m_sizeImage.cy) / 2);
		int cxBlt = min(pTrans->sizeImage.cx, m_sizeImage.cx);
		int cyBlt = min(pTrans->sizeImage.cy, m_sizeImage.cy);

		::SelectObject(hdcSrc, pTrans->hbmImageWell);
		::BitBlt(hdcDest,
			(m_sizeImage.cx * nImage) + xShiftNew, yShiftNew, cxBlt, cyBlt,
			hdcSrc, (pTrans->sizeImage.cx * pTrans->iImage) + xShiftCur, yShiftCur,
			SRCCOPY);
	}

	::SelectObject(hdcDest, hbmOldDest);
	::SelectObject(hdcSrc, hbmOldSrc);
	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	if (m_hbmImageWell != NULL)
		::DeleteObject(m_hbmImageWell);
	m_hbmImageWell = hbmNewWell;
	m_nTiles += nTiles;
	return TRUE;
}

//      CCustomBar::DeleteButton
//              Deletes the button at the given index from the toolbar.  Also
//              automitically removes redundant separators left by delete.  If we are permanently removing a 
//              textual button then its label string can be removed from the list of
//              labels

void CCustomBar::DeleteButton(int nIndex, BOOL bMoving /*=FALSE*/)
{
	ASSERT(0 <= nIndex && nIndex < m_nCount);
	ASSERT(m_nCount > 0);
	ASSERT_VALID(this);

	if( s_pDroppedBar==this)
	{
		CancelMenu();
	}

	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*) m_pData;
	TBBUTTON_EXTRA* pExtra = m_pButtonExtra;
	BOOL bButton = ((pTBB[nIndex].nStyle & TBBS_SEPARATOR) == 0);

	// Deal with the HWND.
	if (!bMoving && (pTBB[nIndex].nStyle & TBBS_HWND) != 0)
	{
		HWND hwndFocus = ::GetFocus();
		HWND hwndControl = GetButtonExtra(nIndex)->GetControl();
		if (hwndControl == hwndFocus || ::IsChild(hwndControl, hwndFocus))
			LoseFocus();

		RemoveControl(hwndControl);
	}

	if (m_nCount - 1 == 0)
	{
		Empty();
	}
	else
	{
		if(pTBB[nIndex].nStyle & TBBS_TEXTUAL) 
		{
			RemoveTextLabel(nIndex);
		}

		if(pTBB[nIndex].nStyle & TBBS_CUSTOMBITMAP)
		{
			// remove the mapping and delete the associated object
			if(HasButtonExtra())
			{
				HBITMAP hbmCustom=GetButtonExtra(nIndex)->GetGlyph();
				::DeleteObject(hbmCustom);
			}
		}
	
		AFX_TBBUTTON* pTBBNext = pTBB;
		TBBUTTON_EXTRA* pExtraNext = pExtra;

		BOOL bImageShift=FALSE;

		if(pTBB[nIndex].nStyle & TBBS_GLYPH)
		{
			if (!ShiftImage(pTBB[nIndex].iImage, -1))
				return;

			// because we just deleted a glyphed button, we no longer know if we have any left
			SetGlyphStatus(cGlyphsUnknown);

			bImageShift=TRUE;
		}
		
		int nNewCount = 0;
		int iNext=0; // the destination index (of tbbnext)
		for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++, pExtra++)
		{
			ASSERT(pTBB != NULL);
			if (iButton == nIndex)
				continue;
				
			// If it is a separator for the first or last position, or
			// following another separator, skip it.
			//
			if (!((pTBB->nStyle & TBBS_SEPARATOR) != 0 &&
				(pTBBNext == m_pData || (((pTBBNext - 1)->nStyle & TBBS_SEPARATOR) != 0))))
			{
				// move button itself
				*pTBBNext = *pTBB;

				// only copy button extra if it's valid
				if(HasButtonExtra())
				{
					*pExtraNext = *pExtra;
					pExtraNext++;
				}

				// if we deleted a glyph from the well before this button, and this
				// button has a glyph, then adjust its index
				if (bImageShift &&
					iButton > nIndex &&
					(pTBBNext->nStyle & TBBS_GLYPH) != 0
					)
				{
					pTBBNext->iImage--;
				}
				
				pTBBNext++;
				nNewCount++; iNext++;
			}
		}
		m_nCount=nNewCount;
		
		// No ending separator.
		ASSERT(pTBBNext != m_pData);
		if (((pTBBNext - 1)->nStyle & TBBS_SEPARATOR) != 0)
			m_nCount--;
		
		ASSERT(m_nCount > 0);
	}
	ASSERT_VALID(this);
}

void CCustomBar::RemoveTextLabel(int nIndex)
{
	ASSERT_VALID(this);

	// should always already be extra data, otherwise how could we delete a label
	ASSERT(GetButtonExtra(nIndex)!=NULL);

	EditButtonExtra(nIndex)->SetLabel("");
	
	// remove the flag
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*) m_pData;
	pTBB[nIndex].nStyle &= ~(TBBS_TEXTUAL|TBBS_CUSTOMTEXT);
}

// CCustomBar::AppendButton
//		Adds a button to the end of the bar with a specified command id

int CCustomBar::AppendButton(UINT nId)
{
	ASSERT(nId!=0);

	TRANSFER_TBBUTTON trans(nId);

	int nIndex=InsertButton(m_nCount, trans, FALSE, gapNil);

	Invalidate();

	return nIndex;
}

//      CCustomBar::InsertButton
//              Inserts a button into the bar at the given index.  Also deals with
//              any separators that may need to be added.
//              Can also insert just a separator, when TRANSFER_TBBUTTON is appropriate.
//				Returns the index where the button was inserted

int CCustomBar::InsertButton(int nIndex, TRANSFER_TBBUTTON& trans,
	BOOL bReplace, GAP gap)
{
	ASSERT(0 <= nIndex && nIndex <= m_nCount);
	ASSERT_VALID(this);

	if( s_pDroppedBar==this)
	{
		CancelMenu();
	}

	// load the appropriate bitmap, if required
	trans.LoadCommandBitmap(theApp.m_bLargeToolBars);

	int iButton=0;
	int iDestButton=0;
	int iInsertedAt=0;
	BOOL bHwnd = ((trans.nStyle & TBBS_HWND) != 0);
	TBBUTTON_EXTRA newExtra;
	newExtra.Create();
	BOOL bShiftImage=FALSE; // true if the image was shifted
	
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*) m_pData;
	AFX_TBBUTTON* pTBBNext;
	AFX_TBBUTTON* pTBBNew = pTBBNext =
		(AFX_TBBUTTON*) calloc(m_nCount + 2, sizeof(AFX_TBBUTTON));
	TBBUTTON_EXTRA* pExtra = m_pButtonExtra;
	TBBUTTON_EXTRA* pExtraNext=NULL;
	TBBUTTON_EXTRA* pExtraNew=NULL;

	BOOL bNeedExtra=FALSE; // true if a block of extra information is needed in the destination bar

	if(     pExtra ||
		trans.extra.IsUsed() ||
		bHwnd)
	{
		bNeedExtra=TRUE;
		pExtraNew = pExtraNext = new TBBUTTON_EXTRA[m_nCount + 2];
	}
		
	int nImageIndex=0;
	
	if (pTBBNew == NULL || (bNeedExtra && pExtraNew == NULL))
		goto cleanup;
	
	// Deal with the HWND.
	if (bHwnd)
	{
		ASSERT(bNeedExtra);

		newExtra.SetControl(trans.extra.GetControl());

		if (newExtra.GetControl() == NULL)
			newExtra.SetControl(GetControl(trans.nID));
	
		if (newExtra.GetControl() == NULL)
			goto cleanup;
					
		CWnd* pWnd = CWnd::FromHandle(newExtra.GetControl());
		pWnd->SetParent(this);
		if (IsValidCtrlSize(pWnd))
			pWnd->ShowWindow(SW_SHOWNOACTIVATE);
		else
			pWnd->ShowWindow(SW_HIDE);
	}

	// deal with the label
	if(!trans.extra.GetLabel().IsEmpty())
	{
		newExtra.SetLabel(trans.extra.GetLabel());
	}
	
	// deal with the custom bitmap
	if(trans.extra.GetGlyph()!=NULL)
	{
		newExtra.SetGlyph(GLOBAL_DATA::DuplicateBitmap(this, trans.extra.GetGlyph()));
	}

	// New Image.
	nImageIndex = nIndex; 
	while (nImageIndex < m_nCount &&
		(pTBB[nImageIndex].nStyle & TBBS_GLYPH) == 0)
	{
		nImageIndex++;
	}

	// At this point, nImageIndex is the index of the first button following the selection which has a glyph

	if(trans.nStyle & TBBS_GLYPH)
	{
		if (nImageIndex == m_nCount)
			nImageIndex = m_nTiles;
		else if (m_nCount > 0)
			nImageIndex = pTBB[nImageIndex].iImage;
		
		if (!ShiftImage(nImageIndex, 1, &trans))
			goto cleanup;
		else
			bShiftImage=TRUE;

		// because we just added a glyphed button, we know we are a glyphed bar
		SetGlyphStatus(cGlyphsPresent);
	}

	// Update the data.
	for (iButton = 0; iButton <= m_nCount; iButton++, pTBBNext++, iDestButton++)
	{
		if (iButton != nIndex)
		{
			*pTBBNext = *pTBB++;
			if(bNeedExtra && pExtra)
			{
				*pExtraNext = *pExtra++;
			}


			if (bShiftImage && 
				iButton > nIndex &&
				(pTBBNext->nStyle & TBBS_GLYPH) != 0)
			{
				pTBBNext->iImage++;
			}
		}
		else
		{
			if (gap == gapBefore && nIndex > 0 &&
				((pTBBNext - 1)->nStyle & TBBS_SEPARATOR) == 0)
			{
				pTBBNext->nID = ID_SEPARATOR;
				pTBBNext->nStyle = TBBS_SEPARATOR;
				pTBBNext->iImage = m_cxDefaultGap + 2;
				if(bNeedExtra)
				{
					pExtraNext->Create();
					pExtraNext++;
				}
				pTBBNext++;
				iDestButton++;
			}
				
			iInsertedAt=iDestButton;
			pTBBNext->nID = trans.nID;
			// remove extruded style in here, in case along the way the button picked it up.
			pTBBNext->nStyle = (trans.nStyle) & (~TBBS_EXTRUDED);
			pTBBNext->iImage = nImageIndex;
			if(bNeedExtra)
			{
				*pExtraNext=newExtra;
			}
			
			if (bReplace)
			{
				pTBB++; iButton++;
				if(bNeedExtra && pExtra)
				{
					pExtra++;
				}
			}
			
			if (gap == gapAfter && nIndex < m_nCount &&
				(pTBB->nStyle & TBBS_SEPARATOR) == 0)
			{
				pTBBNext++;     
				iDestButton++;
				pTBBNext->nID = ID_SEPARATOR;
				pTBBNext->nStyle = TBBS_SEPARATOR;
				if(bNeedExtra)
				{
					pExtraNext++;
					pExtraNext->Create();
				}
				pTBBNext->iImage = m_cxDefaultGap + 2;
			}
		}
		if(bNeedExtra)
		{
			pExtraNext++;
		}
	}

	m_nCount=(pTBBNext - pTBBNew);

	if (m_pData != NULL)
		free(m_pData);
	m_pData = pTBBNew;

	if (HasButtonExtra())
		delete [] m_pButtonExtra;
	m_pButtonExtra = pExtraNew;

	ASSERT_VALID(this);

	return iInsertedAt;

// If we failed cleanup, may be somewhat complex, so we handle it here,
// using a goto (only way to get to this control path). 

cleanup:
	if (pTBBNew != NULL)
		free(pTBBNew);

	if (pExtraNew != NULL)
		free((void*) pExtraNew);
		
	newExtra.Destroy();

	// allow callers to detect failure
	return -1;
}

//      CCustomBar::Empty
//              Deletes data, and image well, and sets relavant counts to 0.

void CCustomBar::Empty()
{
	if (m_pData != NULL)
	{
		m_nCount = 0;
		free(m_pData);
		m_pData = NULL;
	}

	if (HasButtonExtra())
	{
		delete [] m_pButtonExtra;
		m_pButtonExtra = NULL;
	}

	if (m_hbmImageWell != NULL)
	{
		m_nTiles = 0;
		::DeleteObject(m_hbmImageWell);
		m_hbmImageWell = NULL;
	}

	// because we just cleared the bar, we know we are an unglyphed bar
	SetGlyphStatus(cGlyphsAbsent);
}

//      CCustomBar::RemoveWindows
//              Removes all windows referenced in m_pData.
void CCustomBar::RemoveWindows()
{
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		if (pTBB->nStyle & TBBS_HWND)
		{
			ASSERT(GetButtonExtra(iButton)!=NULL);
			HWND hwndControl=EditButtonExtra(iButton)->GetControl();
			EditButtonExtra(iButton)->SetControl(NULL);
			RemoveControl(hwndControl);
			pTBB->nStyle &=~(TBBS_HWND|TBBS_SIZABLE);
		}
	}
}


//      CCustomBar::RemoveExtra
//              Removes all extra information for the bar
void CCustomBar::RemoveExtra()
{
	RemoveWindows();

	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		if(pTBB->nStyle & TBBS_TEXTUAL)
		{
			ASSERT(GetButtonExtra(iButton)!=NULL);
			EditButtonExtra(iButton)->SetLabel("");
			pTBB->nStyle &=~(TBBS_TEXTUAL|TBBS_CUSTOMTEXT);
		}
		if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
		{
			// there doesn't have to be a glyph, so we must be careful
			if(HasButtonExtra())
			{
				if(GetButtonExtra(iButton)->GetGlyph())
				{
					HBITMAP hbmCustom=EditButtonExtra(iButton)->GetGlyph();
					EditButtonExtra(iButton)->SetGlyph(NULL);
					::DeleteObject(hbmCustom);
					pTBB->nStyle &=~TBBS_CUSTOMBITMAP;
				}
			}
		}
	}
}

//      CCustomBar::IsGap
//              Determines whether a separator is located at the given index.

BOOL CCustomBar::IsGap(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		return FALSE;
	else
		return ((((AFX_TBBUTTON*)m_pData) + nIndex)->nStyle & TBBS_SEPARATOR) != 0;
}

BOOL CCustomBar::IsValidCtrlSize(CWnd* pWnd)
{
	CRect rectClient, rectCtrl;
	GetClientRect(rectClient);
	pWnd->GetWindowRect(rectCtrl);

	return (rectCtrl.Width() <=
		rectClient.Width() - GetLeftBorder(orNil) - GetRightBorder(orNil));
}

// This decides the HT code for a bar expansion at a given bar location
UINT CCustomBar::GetExpansionConstraint(const CRect &rect, ORIENTATION *pOrBar, BOOL bUseOrient/*=FALSE*/)
{
	CRect rectClient, rectWindow;
	GetClientRect(rectClient);
	GetWindowRect(rectWindow);

	ORIENTATION orBar;
	if(bUseOrient)
	{
		ASSERT(pOrBar);
		orBar=*pOrBar;
	}
	else
	{
		orBar= GetOrient();
	}
		
	// Treat undocked but dock sized windows as if they were docked,
	if (orBar == orNil)
	{
		if (rectWindow.Height() == GetHSize(orHorz).cy)
			orBar = orHorz; // Force single row of buttons.
		else if (rectWindow.Width() == GetVWidth())
			orBar = orVert; // Force single column of buttons.
	}

	UINT nHTSize = HTRIGHT; // Sizing width restricted.
	if (orBar == orHorz || (orBar == orNil &&
		(rect.left <= rectClient.left || rect.right >= rectClient.right)))
	{
		nHTSize = HTBOTTOM;     // Sizing height restricted.
	}

	if(pOrBar)
	{
		*pOrBar=orBar;
	}

	return nHTSize;
}

//      CCustomBar::GetItemRect
//              Override of CToolbar function to support sizable toolbars.

void CCustomBar::GetItemRect(int nIndex, LPRECT lpRect, ORIENTATION or) const
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT)));

	CRect rect;
	BOOL bWrapped = FALSE;
	
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton <= nIndex; iButton++, pTBB++)
	{
		EnumItemRects(iButton, rect, bWrapped, or);
	}
	
	::CopyRect(lpRect, rect);
}

//      CCustomBar::DoPaint
//              Override of CToolbar function to support sizable toolbars.
//              Also draws selection for toolbar customizing.

void CCustomBar::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CControlBar::DoPaint(pDC);      // draw border
	DrawGrabber(pDC);
	BOOL bTextInitialised=FALSE;    // used to ensure that time is not wasted initialising the
									// device context for font drawing unless actually required

	// these are only valid when bTextInitialised is true. And then they contain the values
	// to restore at the end of the routine.
	CFont *pOldFont=NULL;                   
	UINT oldAlign=0;
	
	if (m_nCount == 0)
		return;

	DrawState ds;
	if (m_nTiles > 0 && !PrepareDrawButton(ds))
		return;     // something went wrong

	CRect rect;
	CWnd* pWndChild = GetTopWindow();
	CToolCustomizer* pCustomizer = GetCustomizer();

	ORIENTATION or=GetOrient();

	CSize sizeButton=GetButtonSize(or);

	if (pCustomizer != NULL && pCustomizer->m_pSelectBar->GetRealObject() == this)
	{
		// If we have the selection, update window controls so that we
		// won't paint over the selection.  We paint in the same order
		// as windows.  This is expensive and ugly, so we only do it
		// if we really need to.

		BOOL bUpdateAllControls = FALSE;
		CRect rectSel, rectWnd;

		GetItemRect(pCustomizer->m_nSelectIndex, rectSel, or);
		ClientToScreen(rectSel);

		while (pWndChild != NULL)
		{
			pWndChild->GetWindowRect(rectWnd);
			if (pWndChild->IsWindowVisible() &&
				rect.IntersectRect(rectWnd, rectSel))
			{
				bUpdateAllControls = TRUE;
				break;
			}
			pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
		}

		if (bUpdateAllControls)
		{
			while (pWndChild != NULL)
			{
				pWndChild->GetWindowRect(rectWnd);
				if (pWndChild->IsWindowVisible())
					pWndChild->UpdateWindow();
				pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
			}
		}
	}

	BOOL bWrapped = FALSE;
	
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		EnumItemRects(iButton, rect, bWrapped, or);
		
		if(pTBB->nStyle & TBBS_INVISIBLE)
		{
			continue;
		}

		if (::RectVisible(pDC->m_hDC, &rect))
		{
			ASSERT(pTBB != NULL);
			if ((pTBB->nStyle & (TBBS_SEPARATOR|TBBS_TEXTUAL)) == 0 &&
				!IsVisibleControl(iButton))
			{
				DrawButton(pDC, rect.left, rect.top,
					pTBB->iImage, pTBB->nStyle
#ifdef AUTOUPDATE_BUTTONS
					);
#else
					& ~TBBS_DISABLED);
#endif
			}

			if (pTBB->nStyle & TBBS_SEPARATOR)
			{
				int cSep = (pTBB->iImage - 1) / 2;
				if (or == orVert)
				{
					pDC->FillSolidRect(rect.left, rect.top + cSep,
						sizeButton.cx, 1, globalData.clrBtnShadow);
					pDC->FillSolidRect(rect.left, rect.top + cSep + 1,
						sizeButton.cx, 1, globalData.clrBtnHilite);
				}
				else // orNil has same painting style as orHorz
				{
					pDC->FillSolidRect(rect.left + cSep,     rect.top,
						1, sizeButton.cy, globalData.clrBtnShadow);
					pDC->FillSolidRect(rect.left + cSep + 1, rect.top,
						1, sizeButton.cy, globalData.clrBtnHilite);
				}
				// REVIEW: if sep at end of line, paint horz instead.
			}

			if(pTBB->nStyle & TBBS_TEXTUAL)
			{
				if(!bTextInitialised)
				{
					pOldFont=pDC->SelectObject(GetTextFont(GetOrient()));
					bTextInitialised=TRUE;
				}

				// set the 'button size to the width of this button. This will cause the
				// DrawButton routine to draw correctly. Must restore it afterwards
				CSize oldButtonSize=m_sizeButton;
				CSize oldImageSize=m_sizeImage;

				// calculate 'fake' image and button widths that correspond to this button.
				m_sizeImage.cx=rect.Width()-(oldButtonSize.cx-oldImageSize.cx);
				m_sizeButton.cx=rect.Width();
				m_sizeImage.cy=rect.Height()-(oldButtonSize.cy-oldImageSize.cy);
				m_sizeButton.cy=rect.Height();

				// draw the frame, and get coordinates for the text
				CRect drawBounds;
				DrawButton(pDC, rect.left, rect.top, pTBB->iImage, pTBB->nStyle, TRUE, &drawBounds);

				// restore button and image sizes
				m_sizeButton=oldButtonSize;
				m_sizeImage=oldImageSize;

				// determine colours
				COLORREF oldBack=pDC->SetBkColor(globalData.clrBtnFace);

				// if it's got a drop arrow, then place the text within the right/bottom portion of the button
				int nArrowWidth=0; 
				int nArrowHeight=0;
				HBITMAP hbmArrow=NULL;
				
				if(     (pTBB->nStyle & TBBS_MENU)!=0 &&
					(GetBarStyle() & CTBRS_MAINMENU)==0)
				{
					if(pTBB->nStyle & TBBS_DISABLED)
					{
						hbmArrow=GetDisabledMenuArrow(or);
					}
					else
					{
						hbmArrow=GetMenuArrow(or);
					}

					// figure the size of the bitmap
					BITMAP bmp;
					::GetObject(hbmArrow, sizeof(bmp), &bmp);

					nArrowWidth=bmp.bmWidth;
					nArrowHeight=bmp.bmHeight;
				}
				int nGlyphWidth=0;
				int nGlyphHeight=0;
				if( pTBB->nStyle & TBBS_GLYPH)
				{
					nGlyphWidth=m_sizeImage.cx;
					nGlyphHeight=m_sizeImage.cy;
				}

				UINT oldAlign;

				CSize stringHeight;
				MeasureString(iButton, stringHeight, or); 

				UINT MenuTextFlags = GLOBAL_DATA::e_mtoCenterAlign ;
				if(or==orVert)
				{
					// Text is vertical.
					MenuTextFlags |= GLOBAL_DATA::e_mtoVertical ;

					// Adjust the boundary.
					drawBounds.top+=nGlyphHeight;
					drawBounds.bottom-=nArrowHeight;
					int dx=(drawBounds.Width()-stringHeight.cx)/2;
					drawBounds.left+=dx;
					drawBounds.right-=dx;
				}
				else
				{
					// Adjust the boundary
					drawBounds.left+=nGlyphWidth;
					drawBounds.right-=nArrowWidth;
					int dy=(drawBounds.Height()-stringHeight.cy)/2;
					drawBounds.top+=dy;
					drawBounds.bottom-=dy;
				}

				COLORREF oldText ;
				if(pTBB->nStyle & TBBS_DISABLED) 
				{
					// Text is disabled.
					MenuTextFlags |= GLOBAL_DATA::e_mtoDisabled ;
				}
				else
				{
					// Text color is not set by MenuTextOut.
					oldText=pDC->SetTextColor(globalData.clrBtnText);
				}

				globalData.MenuTextOut(pDC, &drawBounds, 
								GetButtonExtra(iButton)->GetLabel(), 
								MenuTextFlags);

				// Cleanup...
				if((pTBB->nStyle & TBBS_DISABLED)==0) 
				{
					pDC->SetTextColor(oldText);
				}

				if(nArrowWidth>0)
				{
					// need to draw dropdown menu button
					CDC dcArrow;
					dcArrow.CreateCompatibleDC( pDC );
					ASSERT(hbmArrow!=NULL);
					CBitmap *pOldBitmap = dcArrow.SelectObject(CBitmap::FromHandle(hbmArrow));

					// vertically centre arrow, and put it to the right of the text
					CPoint drawArrow;
					
					if(or==orVert)
					{
						drawArrow=CPoint(drawBounds.left+(drawBounds.Width()-nArrowWidth)/2, 
									 drawBounds.bottom);
					}
					else
					{
						drawArrow=CPoint(drawBounds.right, 
									 drawBounds.top+(drawBounds.Height()-nArrowHeight)/2);
					}

					// draw it
					pDC->BitBlt(drawArrow.x, drawArrow.y, nArrowWidth,nArrowHeight, &dcArrow, 0, 0, SRCCOPY);

					dcArrow.SelectObject( pOldBitmap );
				}

				if(nGlyphWidth>0)
				{
					CPoint drawGlyph;

					if(or==orVert)
					{
						drawGlyph=CPoint(drawBounds.left+(drawBounds.Width()-nGlyphWidth)/2, 
									 drawBounds.top-m_sizeImage.cy);
					}
					else
					{
						drawGlyph=CPoint(drawBounds.left-m_sizeImage.cx, 
									 drawBounds.top+(drawBounds.Height()-nGlyphHeight)/2);
					}

					DrawButtonGlyph(pDC, drawGlyph.x, drawGlyph.y, CPoint(0,0), pTBB->iImage, pTBB->nStyle, sizeButton, m_sizeImage, s_hDCGlyphs, s_hDCMono);
				}

				pDC->SetBkColor(oldBack);
			}

			if (pCustomizer != NULL &&
				pCustomizer->m_pSelectBar->GetRealObject() == this &&
				pCustomizer->m_nSelectIndex == iButton)
			{
				COLORREF crBk = pDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
				COLORREF crFG = pDC->SetTextColor(RGB(0, 0, 0));
		
				pCustomizer->DrawSelection(pDC);

				pDC->SetBkColor(crBk);
				pDC->SetTextColor(crFG);
			}
		}
	}
	
	// Remember whether we drew these controls
	if(IsMDIHost())
	{
		DrawMDIControls(pDC);
		if(m_nMDITrack==cMDINotDrawn)
		{
			m_nMDITrack=cMDINoButton;
		}
	}
	else
	{
		m_nMDITrack=cMDINotDrawn;
	}

	if (m_nTiles > 0)
		EndDrawButton(ds);

	// clean up context
	if(bTextInitialised)
	{
		pDC->SelectObject(pOldFont);
		pDC->SetTextAlign(oldAlign);
	}
}

//      CCustomBar::HitTest
//              Override of CToolbar function to support sizable toolbars.

int CCustomBar::HitTest(CPoint point, BOOL bCheckWindow) const // in window relative coords
{
	if(bCheckWindow)
	{
		// always return -1 if the point isn't inside our window
		CPoint ptScreen=point;
		ClientToScreen(&ptScreen);
		CWnd *pWndHit=CWnd::WindowFromPoint(ptScreen);
		if(pWndHit->GetSafeHwnd()!=this->GetSafeHwnd() && !IsChild(pWndHit))
		{
			return -1;
		}
	}

	CRect rect;
	BOOL bWrapped = FALSE;
	
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	ORIENTATION or=GetOrient();
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		EnumItemRects(iButton, rect, bWrapped, or);
		
		if (point.x < rect.left && point.y < rect.top)
			return -1;      // we have passed the point.
			
		ASSERT(pTBB != NULL);
		if ((pTBB->nStyle & TBBS_SEPARATOR) == 0)
		{
			if (rect.PtInRect(point))
				return iButton; // Hit!
		}
	}
	return -1;      // nothing hit
}

//      CCustomBar::SizeTest
//              Variant on HitTest, to determine whether the given point lies within
//              an active sizing region.  We must be in customize mode, with the
//              sizable control selected for this to return positive.

int CCustomBar::SizeTest(CPoint point)
{
	CToolCustomizer* pCustomizer = GetCustomizer();
	if (pCustomizer != NULL && pCustomizer->m_pSelectBar->GetRealObject() == this)
	{
		ORIENTATION or = GetOrient();
		int nIndex = HitTest(point);
		
		if (or != orVert &&     // No sizing in vert docks.
			!pCustomizer->m_pSelectBar->IsInDialog() &&
			pCustomizer->m_nSelectIndex == nIndex &&
			(GetButtonStyle(nIndex) & TBBS_SIZABLE) != 0)
		{
			ASSERT(GetButtonStyle(nIndex) & TBBS_HWND);
			if (!::IsWindowVisible(GetButtonExtra(nIndex)->GetControl()))
				return -1;

			CRect rect;
			GetItemRect(nIndex, rect, or);
			rect.left = rect.right - g_mt.cxFrame;
			
			if (rect.PtInRect(point))
				return nIndex;
		}
	}
	
	return -1;
}

//      CCustomBar::IndexFromPoint
//              Returns the insertion index for a button based on the specified point
//              If no intersection is found, then we return either the first or last
//              button in the row that contains the rect's center.  If the mid point
//              is within a separator, then we return the separator index.

// pButtonRect contains the rect of the button with the index that we actually matched.

// These debugging aids help one trace problems with index from point, since one often needs to see the results of several
// calls to spot the inconsistency.
#if 0
#define IFP_TRACE(msg) TRACE0(msg "\n\r") 
#define IFP_TRACE_PT(msg, pt) TRACE0(msg); TRACE2("x: %d, y: %d\n\r", pt.x, pt.y)
#define IFP_TRACE_IRECT(msg, index, rect) TRACE0(msg); TRACE1("n: %d,", index); TRACE2("l: %d, t: %d, ", rect.left, rect.top); TRACE2("b: %d, r: %d\n\r", rect.right, rect.bottom);
#else
#define IFP_TRACE(msg)
#define IFP_TRACE_PT(msg, pt)
#define IFP_TRACE_IRECT(msg, index, rect)
#endif

int CCustomBar::IndexFromPoint(const CPoint& pt, CRect *pButtonRect)
{
	int nyRowFirst;
	int nRowFirst = -2, nRowLast = -2;
	int nIndex=-2;
	CRect rectOld, rectTest;
	CRect rectFirst, rectRowFirst, rectRowLast, rectMatch;
	BOOL bWrapped = FALSE;
	int iButtonOld=-1;
	BOOL bFoundVisible=FALSE;

	IFP_TRACE_PT("Starting IndexFromPoint", pt);
	
	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	ORIENTATION or=GetOrient();
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		EnumItemRects(iButton, rectTest, bWrapped, or);
		
		IFP_TRACE_IRECT("Button ", iButton, rectTest);

		if((pTBB->nStyle & TBBS_INVISIBLE)!=0)
		{
			// skip all invisibles
			continue;
		}

		if (!bFoundVisible)
		{
			rectFirst = rectOld = rectTest;
			IFP_TRACE_IRECT("Initial ", iButton, rectFirst);
			bFoundVisible=TRUE;
		}

		if(pt.y<rectTest.top && pt.x<rectTest.left)
		{
			break;
		}       
		
		ASSERT(pTBB != NULL);

		if (rectTest.bottom > pt.y)
		{
			if (nRowFirst == -2)
			{
				nRowFirst = iButtonOld;
				nyRowFirst = rectTest.bottom;
				rectRowFirst=rectOld;
				IFP_TRACE_IRECT("RowFirst ", nRowFirst, rectRowFirst);
			}
			else if (nRowLast == -2 && rectTest.top > nyRowFirst)
			{
				rectRowLast=rectTest;
				nRowLast = iButton;
				IFP_TRACE_IRECT("RowFirst ", nRowLast, rectRowLast);
			}
		}
		
		if (rectTest.PtInRect(pt))
		{
			CPoint ptCenter=rectTest.CenterPoint();
			IFP_TRACE_PT("IndexFromPoint Center ", ptCenter);

			BOOL bNext=FALSE;

			if(or==orVert)
			{
				if(pt.y>ptCenter.y)
				{
					bNext=TRUE;
				}
			}
			else
			{
				if(pt.x>ptCenter.x)
				{
					bNext=TRUE;
				}
			}

			if(bNext)
			{
				nIndex=iButton;
				rectMatch=rectTest;
			}
			else
			{
				nIndex=iButtonOld;
				rectMatch=rectOld;
			}
			IFP_TRACE_IRECT("Match", nIndex, rectMatch);
			break;
		}

		// only track the previous visible button
		if((pTBB->nStyle & TBBS_INVISIBLE)==0)
		{
			rectOld = rectTest;
			iButtonOld=iButton;
		}
	}
	if (nIndex!=-2)
	{
		if(pButtonRect)
		{
			*pButtonRect=rectMatch;
		}
		IFP_TRACE("Return match\n\r");
		return nIndex;
	}

	// hold final rect in rectMatch
	rectMatch=rectTest;

	// no intersection, we must be on the fringes somewhere
	GetClientRect(rectTest);
	if ((or!=orVert && pt.y < rectFirst.top) ||
		(or==orVert && pt.x < rectFirst.left))
	{
		if(pButtonRect)
		{
			*pButtonRect=rectFirst;
		}
		IFP_TRACE("Return first");
		return -1;
	}

	if ((or!=orVert && pt.y > rectTest.bottom) ||
		(or==orVert && pt.x > rectTest.right))
	{
		if(pButtonRect)
		{
			*pButtonRect=rectMatch;
		}
		IFP_TRACE("Return match");
		return m_nCount-1;
	}

	if ((or!=orVert && pt.x < rectTest.left) ||
		(or==orVert && pt.y < rectTest.top))
	{
		if(pButtonRect)
		{
			*pButtonRect=rectRowFirst;
		}
		IFP_TRACE("Return rowfirst");
		return nRowFirst;
	}

	if(nRowLast!=-2)
	{
		if(pButtonRect)
		{
			*pButtonRect=rectRowLast;
		}
		IFP_TRACE("Return rowlast");
		return nRowLast;
	}
	else
	{
		if(pButtonRect)
		{
			*pButtonRect=rectMatch;
		}
		IFP_TRACE("Return match/2");
		return m_nCount-1;
	}
}

//      CCustomBar::Fit
//              Returns the best size for the toolbar, given an approximate width.

CSize CCustomBar::Fit(int nWidth, ORIENTATION or)
{
	if (m_nCount == 0)
		return m_sizeFixedLayout;
		
	CSize size(0, 0);
	CRect rect;
	CRect rectClient(g_mt.cxBorder, g_mt.cyBorder, nWidth - g_mt.cxBorder,
		m_sizeMaxLayout.cy - g_mt.cyBorder);
	BOOL bWrapped = FALSE;
	
	for (int iButton = 0; iButton < m_nCount; iButton++)
	{
		EnumItemRects(iButton, rect, bWrapped, or, rectClient);
		if (!IsGap(iButton))
		{
			size.cx = max(size.cx, rect.right);
			size.cy = max(size.cy, rect.bottom);
		}

		// on the last button, we must account for the mdi controls
		if(iButton==m_nCount-1)
		{
			int nMDIRight=rect.right+GetRightMDIArea(or);
			if(nMDIRight > rectClient.right)
			{
				// System buttons caused a wrap, so pretend that the last button is 1 row higher than it really is.
				CSize sizeButton=GetButtonSize(or);
				size.cy+=sizeButton.cy;
			}
			else
			{
				size.cx=max(size.cx, nMDIRight);
			}
		}
	}

	size.cx += GetRightBorder(or) + g_mt.cxBorder;
	size.cy += GetBottomBorder(or) + g_mt.cyBorder;

	return size;
}

//      CCustomBar::EnumItemRects
//              Because of the way toolbars are sized, getting the rectangle of a
//              given item is a fairly complex task.  This function allows item rect
//              enumeration, within a given toolbar rect (lpRect), by incrementing the
//              item index, and passing in the last rect, and wrapped state bool and a
//              bar orientation

void CCustomBar::EnumItemRects(int nItem, CRect& rect, BOOL& bWrapped, ORIENTATION or,
	LPCRECT lpRect /*=NULL*/) const
{
	ASSERT(nItem < m_nCount);

	CSize sizeButton=GetButtonSize(or);

	// true when we've just been forced to the next line by a vertical text button
	BOOL bVerticalWrapped=FALSE;

	AFX_TBBUTTON* pTBB = ((AFX_TBBUTTON*)m_pData) + nItem;

	// If no rect is given, use the window's client rect.
	CRect rectClient;

	if (lpRect != NULL) {
		rectClient = *lpRect;
	} else {
		GetClientRect(rectClient);

		int yScroll=GetScrollPos(SB_VERT);
		int xScroll=GetScrollPos(SB_HORZ);
		rectClient.OffsetRect(-xScroll,-yScroll);
	}

	rectClient.left += GetLeftBorder(or);
	rectClient.right -= m_cxRightBorder;

	rectClient.top += GetTopBorder(or);

	// determine if this is the very first item on the bar, or the first visible one
	BOOL bFirst=TRUE;
	int nPrev=nItem-1;

	// After the loop, this will point to the previous visible toolbar button before the current one.
	AFX_TBBUTTON* pTBBPrev = ((AFX_TBBUTTON*)m_pData) + nItem-1;
	while(nPrev>=0)
	{
		if((pTBBPrev->nStyle & TBBS_INVISIBLE)==0)
		{
			bFirst=FALSE;
			break;
		}
		--pTBBPrev;
		--nPrev;
	}
	
	if (!bFirst)
	{
		if(pTBB->nStyle & TBBS_INVISIBLE)
		{
			if(or!=orVert)
			{
				rect.left=rect.right;
			}
			else
			{
				rect.top=rect.bottom;
			}
			return;
		}

		// if the last item was a tall vertical item, we must go underneath, so we have wrapped
		if(	rect.Height()>sizeButton.cy ||
			(rect.Height()==0 && or==orVert))
		{
			// possibly a vertical button, but check
			if(pTBBPrev->nStyle & TBBS_TEXTUAL)
			{
				rect.top = rect.bottom;
				rect.left -= (sizeButton.cx - rect.Width()) / 2;      // Re-align to col.

				bVerticalWrapped=TRUE;
			}
		}

		if(!bVerticalWrapped)
		{
			if(or!=orVert)
			{
				rect.left = rect.right;

				// text buttons are always top aligned, so this is not necessary. We only need it for window controls
				if((pTBBPrev->nStyle & TBBS_TEXTUAL)==0)
				{
					rect.top -= (sizeButton.cy - rect.Height()) / 2;      // Re-align to row.
				}
			}
			else
			{
				rect.top=rect.bottom;
			}
		}
	}
	else
	{
		rect.left = rectClient.left+GetLeftMDIArea(or);
		rect.top = rectClient.top+GetTopMDIArea(or);

		// invisible buttons exist theoretically with zero size but the right height

		if(pTBB->nStyle & TBBS_INVISIBLE)
		{
			if(or!=orVert)
			{
				rect.right=rect.left;
				rect.bottom=rect.top + sizeButton.cy;
			}
			else
			{
				rect.bottom=rect.top;
				rect.right=rect.left+sizeButton.cx;
			}
			return;
		}
	}

	if (pTBB->nStyle & TBBS_SEPARATOR)
	{
		if(!bVerticalWrapped)
		{
			if(or!=orVert)
			{
				rect.right = rect.left + m_cxDefaultGap;
				rect.bottom = rect.top + sizeButton.cy;
			}
			else
			{
				rect.bottom= rect.top+ m_cxDefaultGap; // Not a typo. Reversed bar, so we use x.
				rect.right= rect.left+ sizeButton.cx;
			}
		}
	}
	else
	{
		if (pTBB->nStyle & TBBS_TEXTUAL) 
		{
			/* function is const, and must remain that way as it is called from 
			const framework functions. But to get a DC we need a non-cost window
			pointer. Nasty, but I can't see any other way to measure this text. 
			I think it's safe to say that rom-ability is not an issue here.
			martynl 08Apr96
			*/
			CCustomBar *pVariableThis=const_cast<CCustomBar*>(this);

			// determine size of string if drawn in that font
			CSize stringSize;
			pVariableThis->MeasureString(nItem, stringSize, or);

			// the size from which the text height is derived
			CSize sizeTextHeight; 
			sizeTextHeight=sizeButton;

			if(or==orVert)
			{
				rect.right=rect.left+sizeTextHeight.cx;
				rect.bottom = rect.top + stringSize.cy + (sizeButton.cy - m_sizeImage.cy);
			}
			else
			{
				rect.right = rect.left + stringSize.cx + (sizeButton.cx - m_sizeImage.cx);
				rect.bottom = rect.top + sizeTextHeight.cy; 
			}
		}
		else if ((pTBB->nStyle & TBBS_HWND) == 0)
		{
			rect.right = rect.left + sizeButton.cx;
			rect.bottom = rect.top + sizeButton.cy;
		}
		else 
		{
			CRect rectTmp;
			
			::GetWindowRect(GetButtonExtra(nItem)->GetControl(), rectTmp);
			if (or==orVert)
			{
				rect.right = rect.left + sizeButton.cx;
				rect.bottom = rect.top + sizeButton.cy;
			}
			else
			{
				rect.top += (sizeButton.cy - rectTmp.Height()) / 2;
				rect.right = rect.left + rectTmp.Width();
				rect.bottom = rect.top + rectTmp.Height();
			}
		}
	}

	if (nItem == 0)
		return;
	BOOL bInGroup = (((pTBB - 1)->nStyle & TBBS_SEPARATOR) == 0);

	if(!bVerticalWrapped && or!=orVert)
	{
		if (bInGroup && rect.left != rectClient.left)
		{
			rect.left -= m_cxSharedBorder; rect.right -= m_cxSharedBorder;  // overlap
		
			if (rect.right > rectClient.right)
			{
				bWrapped = TRUE;
				rect.OffsetRect(rectClient.left - rect.left,
					sizeButton.cy - m_cySharedBorder);    // overlap
			}
				
		}
		else
		{
			int nGroupEnd = rect.right;     // no overlap on last button.
			
			// Check the extent of this group.
			while (++nItem < m_nCount && ((++pTBB)->nStyle & TBBS_SEPARATOR) == 0)
			{
				if (pTBB->nStyle & TBBS_INVISIBLE)
				{
					continue;
				}

				if ((pTBB->nStyle & (TBBS_TEXTUAL|TBBS_HWND)) == 0)
				{
					nGroupEnd += sizeButton.cx - m_cxSharedBorder;  // overlap
				}
				else
				{
					if(pTBB->nStyle & TBBS_TEXTUAL)
					{
						CSize stringSize;

						// see comment above about this cast
						CCustomBar *pVariableThis=const_cast<CCustomBar*>(this);

						pVariableThis->MeasureString(nItem, stringSize, orHorz);

						nGroupEnd+= sizeButton.cx-m_sizeImage.cx+stringSize.cx;
					}
					else
					{
						CRect rectTmp;
						::GetWindowRect(GetButtonExtra(nItem)->GetControl(), rectTmp);
						
						if (rectTmp.Width() > rectClient.Width())
							nGroupEnd += sizeButton.cx - m_cxSharedBorder;  // overlap
						else
							nGroupEnd += rectTmp.Width() - m_cxSharedBorder;  // overlap
					}
				}
			}
				
			if (nGroupEnd > rectClient.right || bWrapped)
			{
				bWrapped = FALSE;
				rect.OffsetRect(rectClient.left - rect.left,
					sizeButton.cy + m_cyDefaultGap);
			}
		}
	}
}

// This retrieves a pointer to the appropriate font object
CFont *CCustomBar::GetTextFont(ORIENTATION or)
{
	ASSERT_VALID(this);

	if(or==orVert)
	{
		if((m_dwStyle & CTBRS_MAINMENU)!=0)
		{
			// menu bars use the menu font
			return globalData.GetMenuVerticalFont();
		}
		else
		{
			return CFont::FromHandle(globalData.GetButtonVerticalFont(theApp.m_bLargeToolBars));
		}
	}
	else
	{
		if((m_dwStyle & CTBRS_MAINMENU)!=0)
		{
			// menu bars use the menu font
			return globalData.GetMenuFont(FALSE);
		}
		else
		{
			return CFont::FromHandle(globalData.GetButtonFont(theApp.m_bLargeToolBars));
		}
	}
}

HBITMAP CCustomBar::GetMenuArrow(ORIENTATION or)
{
	ASSERT_VALID(this);

	if(or==orVert)
	{
		return globalData.GetMenuArrowVertical();
	}
	else
	{
		return globalData.GetMenuArrow();
	}
}

HBITMAP CCustomBar::GetDisabledMenuArrow(ORIENTATION or)
{
	ASSERT_VALID(this);

	if(or==orVert)
	{
		return globalData.GetMenuArrowVerticalDisabled();
	}
	else
	{
		return globalData.GetMenuArrowDisabled();
	}
}

// default version gets the menu text
void CCustomBar::AddText(int nIndex)
{
	AFX_TBBUTTON *pTBB=_GetButtonPtr(nIndex);

	LPCSTR pText;

	theCmdCache.GetCommandString(pTBB->nID, STRING_MENUTEXT, &pText);
	if (!pText || *pText=='\0')
	{
		theCmdCache.GetCommandString(pTBB->nID, STRING_COMMAND, &pText);
	}

	CString text(pText);
	AddText(nIndex, text, FALSE);
}

// makes the button textual, and gives it the specified label
void CCustomBar::AddText(int nIndex, const CString &text, BOOL bCustom/*=TRUE*/)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[nIndex]);
	
	ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);

	ASSERT(!text.IsEmpty());

	if(text.IsEmpty())
	{
		return;
	}

	EditButtonExtra(nIndex)->SetLabel(text);

	// modify toolbar button
	pTBB->nStyle |= TBBS_TEXTUAL;
	if(bCustom)
	{
		pTBB->nStyle |= TBBS_CUSTOMTEXT;
	}
	else
	{
		pTBB->nStyle &= ~TBBS_CUSTOMTEXT;
	}

	ASSERT_VALID(this);
}


//      CCustomBar::BltCommandBitmap
//              Used for constructing toolbar image wells.  Gets the command bitmap
//              from the command table, and blts it to the image well DC.
BOOL CCustomBar::BltCommandBitmap(HBITMAP hbmSrc, int nGlyph,
	HDC hdcDest, int nDest, HDC hdcSrc)
{
	if (nGlyph == -1)
		return FALSE;

	hbmSrc = (HBITMAP) ::SelectObject(hdcSrc, hbmSrc);

	BitBlt(hdcDest, m_sizeImage.cx * nDest, 0, m_sizeImage.cx, m_sizeImage.cy,
		hdcSrc, m_sizeImage.cx * nGlyph, 0, SRCCOPY);
	
	::SelectObject(hdcSrc, hbmSrc);
	
	return TRUE;
}

void CCustomBar::ButtonModified(int nIndex, BOOL bSized)
{
	if(bSized)
	{
		CRect rectNew;
		GetItemRect(nIndex, &rectNew, GetOrient());

		// redraw bars
		RecalcLayout(GetExpansionConstraint(rectNew));
	}
	OnButtonSize(); // REVIEW:: !!! extra bardockx func with better name
}

// remove the textual status of a button, which should already have a glyph
void CCustomBar::RemoveText(int nIndex)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[nIndex]);
	
	// can't be a separator
	ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);

	ASSERT((pTBB->nStyle & (TBBS_TEXTUAL | TBBS_GLYPH))!=0);

	CToolCustomizer* pCustomizer = GetCustomizer();

	int iImage=0;

	AFX_TBBUTTON* pTBBFix=((AFX_TBBUTTON *)m_pData);

	// remove the label
	RemoveTextLabel(nIndex);
}

void CCustomBar::RemoveBitmap(int nIndex)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[nIndex]);
	
	// can't be a separator
	ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);

	ASSERT((pTBB->nStyle & (TBBS_TEXTUAL | TBBS_GLYPH))==(TBBS_TEXTUAL | TBBS_GLYPH));

	if((pTBB->nStyle & TBBS_GLYPH)==0)
	{
		return;
	}

	// because we just deglyphed a button, we no longer know whether we are a glyphed bar
	SetGlyphStatus(cGlyphsUnknown);

	CToolCustomizer* pCustomizer = GetCustomizer();

	// do a full conversion to text
	// delete bitmap from well
	if(!ShiftImage(pTBB->iImage, -1))
	{
		return;
	}

	// delete any custom glyph
	if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
	{
		HBITMAP hbmCustom=EditButtonExtra(nIndex)->GetGlyph();
		if(hbmCustom)
		{
			::DeleteObject(hbmCustom);
			EditButtonExtra(nIndex)->SetGlyph(NULL);
		}
	}

	AFX_TBBUTTON* pTBBFix=((AFX_TBBUTTON *)m_pData)+nIndex+1;

	// adjust the iImage value for all the following buttons
	for (int iButton = nIndex+1; iButton < m_nCount; iButton++, pTBBFix++)
	{
		// If it is an image button, shift the image index by one
		if ((pTBBFix->nStyle & (TBBS_GLYPH)))
		{
			pTBBFix->iImage--;
		}
	}

	pTBB->nStyle &=~(TBBS_CUSTOMBITMAP | TBBS_GLYPH);
}

// makes a button use the a bitmap:
// There are three possible entry situations, dictated by bsSource
void CCustomBar::AddBitmap(int nIndex, BITMAPSOURCE bsSource, HBITMAP hbmCustom)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[nIndex]);
	
	ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);

	CToolCustomizer* pCustomizer = GetCustomizer();

	if((pTBB->nStyle & TBBS_GLYPH)==0)
	{
		// if we can't find any glyphs before here, then we'll go in first
		int iImage=0;

		AFX_TBBUTTON* pTBBFix=((AFX_TBBUTTON *)m_pData)+nIndex-1;

		// deduce the correct iImage value for this button
		for (int iButton = nIndex-1; iButton >= 0; iButton--, pTBBFix--)
		{
			// if this button has a glyph, it can tell us the correct image index
			if(pTBBFix->nStyle & TBBS_GLYPH)
			{
				iImage=pTBBFix->iImage+1;
				break;
			}
		}

		// enlarge the image well
		if (!ShiftImage(iImage, 1))
		{
			return; 
		}

		// set up image to point to new space in well
		pTBB->iImage=iImage;

		pTBB->nStyle |= TBBS_GLYPH;

		pTBBFix=((AFX_TBBUTTON *)m_pData)+m_nCount-1;

		// adjust the iImage value for all the following buttons
		for (iButton = m_nCount-1; iButton > nIndex; iButton--, pTBBFix--)
		{
			// If it is an image button, shift the image index by one
			if (pTBBFix->nStyle & TBBS_GLYPH)
			{
				pTBBFix->iImage++;
			}
		}

		// because we just glyphed a button, we know we are a glyphed bar
		SetGlyphStatus(cGlyphsPresent);
	}

	// remove custom bitmap if going
	if((pTBB->nStyle & TBBS_CUSTOMBITMAP)!=0)
	{
		if(HasButtonExtra())
		{
			HBITMAP hbmOldCustom=GetButtonExtra(nIndex)->GetGlyph();
			if(hbmOldCustom)
			{
				::DeleteObject(hbmOldCustom);
				EditButtonExtra(nIndex)->SetGlyph(NULL);
			}
		}

		pTBB->nStyle &= ~TBBS_CUSTOMBITMAP;
	}

	if(bsSource!=cBitmapSourcePackage)
	{
		// result will be a custom bitmap
		if(bsSource==cBitmapSourceClipboard)
		{
			// attempts to paste in a custom glyph from the clipboard
			GLOBAL_DATA::PasteGlyph(this, m_hbmImageWell, pTBB->iImage, m_sizeImage);
		}
		else
		{
			// copy the custom bitmap to the image well
			HDC hdcClient = ::GetDC(m_hWnd);
			HDC hdcDest = ::CreateCompatibleDC(hdcClient);
			HDC hdcSrc = ::CreateCompatibleDC(hdcClient);
			::ReleaseDC(m_hWnd, hdcClient);
			HBITMAP hbmOld = (HBITMAP) ::SelectObject(hdcDest, m_hbmImageWell);
			HBITMAP hbmOldSrc = (HBITMAP) ::SelectObject(hdcSrc, hbmCustom);

			BitBlt(hdcDest, pTBB->iImage*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, hdcSrc, 0, 0, SRCCOPY);

			::SelectObject(hdcDest, hbmOld);
			::SelectObject(hdcSrc, hbmOldSrc);

			::DeleteDC(hdcSrc);
			::DeleteDC(hdcDest);
		}

		pTBB->nStyle |= TBBS_CUSTOMBITMAP;
	}
	else
	{
		// copy the new bitmap to the image well
		HDC hdcClient = ::GetDC(m_hWnd);
		HDC hdcDest = ::CreateCompatibleDC(hdcClient);
		HDC hdcSrc = ::CreateCompatibleDC(hdcClient);
		::ReleaseDC(m_hWnd, hdcClient);
		HBITMAP hbmOld = (HBITMAP) ::SelectObject(hdcDest, m_hbmImageWell);

		RefreshButtonBitmap(hdcDest, hdcSrc, nIndex);

		::SelectObject(hdcDest, hbmOld);

		::DeleteDC(hdcSrc);
		::DeleteDC(hdcDest);
	}

	// allow subclasses to take any action relevant to the change of button
	OnButtonSize();

	CRect rectNew;
	GetItemRect(nIndex, &rectNew, GetOrient());

	// redraw bars
	RecalcLayout(GetExpansionConstraint(rectNew));

	ASSERT_VALID(this);
}

void CCustomBar::InsertSeparator(int nIndex)
{
	ASSERT(nIndex!=0);
	InsertButton(nIndex,*CToolCustomizer::CreateSeparatorTransfer(), FALSE, gapNil);
	CRect rectNew;
	GetItemRect(nIndex, &rectNew, GetOrient());

	RecalcLayout(GetExpansionConstraint(rectNew));
}

void CCustomBar::RemoveSeparator(int nIndex)
{
	ASSERT(nIndex!=0);

	DeleteButton(nIndex-1);
	OnDeleteSelection();
	RecalcLayout();
}

// re-get all button bitmaps, either from their packages or from their custom bitmap
void CCustomBar::RefreshButtonBitmaps()
{
	if (m_nCount == 0)
		return;

	ASSERT(m_pData != NULL && m_hbmImageWell != NULL);
		
	HDC hdcClient = ::GetDC(m_hWnd);
	HDC hdcDest = ::CreateCompatibleDC(hdcClient);
	HDC hdcSrc = ::CreateCompatibleDC(hdcClient);
	::ReleaseDC(m_hWnd, hdcClient);

	if (hdcDest == NULL || hdcSrc == NULL)
	{       
		// May RIP.
		::DeleteDC(hdcSrc);
		::DeleteDC(hdcDest);
		return;
	}

	HBITMAP hbmOld = (HBITMAP) ::SelectObject(hdcDest, m_hbmImageWell);

	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
	{
		if (pTBB->nStyle & TBBS_GLYPH)
		{
			RefreshButtonBitmap(hdcDest, hdcSrc, iButton);
		}
	}

	::SelectObject(hdcDest, hbmOld);
	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);
}

// re-get just one bitmap
void CCustomBar::RefreshButtonBitmap(HDC hdcDest, HDC hdcSrc, int nIndex)
{
	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[nIndex]);
		
	if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
	{
		// copy or scale as appropriate
		if(     HasButtonExtra() &&
			GetButtonExtra(nIndex)->GetGlyph()!=NULL)
		{
			BITMAP bmp;
			VERIFY(::GetObject(GetButtonExtra(nIndex)->GetGlyph(), sizeof(BITMAP), &bmp));

			if(     bmp.bmHeight==m_sizeImage.cy &&
				bmp.bmWidth==m_sizeImage.cx)
			{
				BltCommandBitmap(GetButtonExtra(nIndex)->GetGlyph(), 0, hdcDest, pTBB->iImage, hdcSrc);
			}
			else
			{
				GLOBAL_DATA::ScaleBitmap(hdcDest, m_sizeImage, CPoint(m_sizeImage.cx*pTBB->iImage, 0), GetButtonExtra(nIndex)->GetGlyph(), CSize(bmp.bmWidth, bmp.bmHeight));
			}
		}
	}
	else
	{
		CPackage* pPackage;

		VERIFY((theApp.GetCommandBtnInfo(pTBB->nID, &pPackage) & (TBBS_SEPARATOR|TBBS_INVISIBLE)) == 0);

		int iGlyph;
		HBITMAP hbmWell;
		VERIFY(theApp.GetCommandBitmap(pTBB->nID, &hbmWell, &iGlyph, theApp.m_bLargeToolBars));

		BltCommandBitmap(hbmWell, iGlyph, hdcDest, pTBB->iImage, hdcSrc);
	}
}

// TRUE if a paste button bitmap operation would be legal on the currently selected button
BOOL CCustomBar::CanPasteImage()
{
	BOOL bEnable=FALSE;

	if (OpenClipboard())
	{
		UINT nClipFmt = 0;
		do
		{
			nClipFmt = EnumClipboardFormats(nClipFmt);
			
			switch (nClipFmt)
			{
			case CF_BITMAP:
			case CF_DIB:
				bEnable = TRUE;
				break;
			}
		}
		while (!bEnable && nClipFmt != 0);

		CloseClipboard();
	}

	CToolCustomizer* pCustomizer = GetCustomizer();

	// neither of these should ever be false, but for safety we check for them in the 
	// if clause as well
	ASSERT(pCustomizer!=NULL);
	ASSERT(pCustomizer->m_pSelectBar->GetRealObject()==this);

	// validate selection index
	ASSERT(pCustomizer->m_nSelectIndex>=0);
	ASSERT(pCustomizer->m_nSelectIndex<m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[pCustomizer->m_nSelectIndex]);

	if((pCustomizer!=NULL &&
		pCustomizer->m_pSelectBar!=NULL &&
		pCustomizer->m_pSelectBar->IsInDialog()) ||
	   (pTBB->nStyle & (TBBS_SEPARATOR))!=0 ||
	    IsVisibleControl(pCustomizer->m_nSelectIndex))
	{
		return FALSE;
	}
	else
	{
		return bEnable;
	}
}

// true if a reset button image operation is legal on the current selection
BOOL CCustomBar::CanResetImage()
{
	CToolCustomizer* pCustomizer = GetCustomizer();

	// neither of these should ever be false, but for safety we check for them in the 
	// if clause as well
	ASSERT(pCustomizer!=NULL);
	ASSERT(pCustomizer->m_pSelectBar->GetRealObject()==this);

	// validate selection index
	ASSERT(pCustomizer->m_nSelectIndex>=0);
	ASSERT(pCustomizer->m_nSelectIndex<m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[pCustomizer->m_nSelectIndex]);

	// can't reset if there's no glyph
	BOOL bHasGlyph=theApp.HasCommandBitmap(pTBB->nID);

	if((pCustomizer!=NULL &&
		pCustomizer->m_pSelectBar!=NULL &&
		pCustomizer->m_pSelectBar->IsInDialog()) ||
		IsVisibleControl(pCustomizer->m_nSelectIndex) ||
	   (pTBB->nStyle & (TBBS_CUSTOMBITMAP))==0 ||
	   (pTBB->nStyle & (TBBS_MENU))!=0 ||
	   !bHasGlyph)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

// true if the user is allowed to choose an image for this button
BOOL CCustomBar::CanChooseAppearance()
{
	CToolCustomizer* pCustomizer = GetCustomizer();

	// neither of these should ever be false, but for safety we check for them in the 
	// if clause as well
	ASSERT(pCustomizer!=NULL);
	ASSERT(pCustomizer->m_pSelectBar->GetRealObject()==this);

	// validate selection index
	ASSERT(pCustomizer->m_nSelectIndex>=0);
	ASSERT(pCustomizer->m_nSelectIndex<m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[pCustomizer->m_nSelectIndex]);

	if((pCustomizer!=NULL &&
		pCustomizer->m_pSelectBar!=NULL &&
		pCustomizer->m_pSelectBar->IsInDialog()) ||
		IsVisibleControl(pCustomizer->m_nSelectIndex))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CCustomBar::PasteButtonImage()
{
	CToolCustomizer* pCustomizer = GetCustomizer();

	ASSERT(pCustomizer->m_pSelectBar->GetRealObject()==this);

	// validate selection index
	ASSERT(pCustomizer->m_nSelectIndex>=0);
	ASSERT(pCustomizer->m_nSelectIndex<m_nCount);

	// make a custom bitmap for this based on the clipboard
	AddBitmap(pCustomizer->m_nSelectIndex,cBitmapSourceClipboard);
	ButtonModified(pCustomizer->m_nSelectIndex, TRUE);
}

void CCustomBar::ResetButtonImage()
{
	CToolCustomizer* pCustomizer = GetCustomizer();

	ASSERT(pCustomizer->m_pSelectBar->GetRealObject()==this);

	// validate selection index
	ASSERT(pCustomizer->m_nSelectIndex>=0);
	ASSERT(pCustomizer->m_nSelectIndex<m_nCount);

	AFX_TBBUTTON* pTBB = &(((AFX_TBBUTTON*)m_pData)[pCustomizer->m_nSelectIndex]);

	ASSERT((pTBB->nStyle & (TBBS_HWND|TBBS_SEPARATOR))==0);

	BOOL bModified=FALSE;

	if(pTBB->nStyle & (TBBS_CUSTOMBITMAP))
	{
		if(theApp.HasCommandBitmap(pTBB->nID))
		{
			AddBitmap(pCustomizer->m_nSelectIndex);
			bModified=TRUE;
		}
	}
	if(pTBB->nStyle & TBBS_CUSTOMTEXT)
	{
		AddText(pCustomizer->m_nSelectIndex);
		bModified=TRUE;
	}

	if(bModified)
	{
		ButtonModified(pCustomizer->m_nSelectIndex, TRUE);
	}

}

BEGIN_MESSAGE_MAP(CCustomBar, CToolBar)
	//{{AFX_MSG_MAP(CCustomBar)
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
//      CCustomBar message handlers

#define CToolCmdUI COldToolCmdUI

class CCustomToolCmdUI : public CToolCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void CCustomToolCmdUI::Enable(BOOL bOn)
{
	CToolCmdUI::Enable(bOn);

	CToolBar* pToolBar = (CToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nStyle=pToolBar->GetButtonStyle(m_nIndex);

	// Buttons on the menu bar are hidden when disabled
	if(	(nStyle & TBBS_MENU)!=0 &&
		(pToolBar->GetBarStyle() & CTBRS_MAINMENU)!=0)
	{
		if(bOn)
		{
			if(nStyle & TBBS_INVISIBLE)
			{
				// make it visible
				nStyle&=~TBBS_INVISIBLE;
				CCustomBar::s_bRecalcLayoutPending=TRUE;
			}
		}
		else
		{
			if((nStyle & TBBS_INVISIBLE)==0)
			{
				// make it invisible
				nStyle|=TBBS_INVISIBLE;
				CCustomBar::s_bRecalcLayoutPending=TRUE;
			}
		}
	}

	// disabled buttons can't stay extruded, and the bar must lose the capture if it had it for this button
	if(!bOn)
	{
		if(pToolBar->GetCapture()==pToolBar &&
			(pToolBar->GetCaptureIndex()==m_nIndex ||
			 pToolBar->GetCaptureIndex()==- m_nIndex - 2))
		{
			pToolBar->CancelMode();
		}

		nStyle &= ~TBBS_EXTRUDED;
	}
	
	ASSERT(!(nStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nStyle);
}

void CCustomToolCmdUI::SetCheck(int nCheck)
{
	CToolCmdUI::SetCheck(nCheck);
}

void CCustomToolCmdUI::SetText(LPCTSTR lpszNew)
{
	CToolCmdUI::SetText(lpszNew);
}

//      CCustomBar::OnUpdateCmdUI
//              Override CToolbar function, so we can enable the buttons during
//              customize mode without enabling menus/accelerators.

void CCustomBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CCustomToolCmdUI state;
	state.m_pOther = this;
	CCmdUI stateWnd;
	CWnd wndTemp;       // very temporary window just for CmdUI update
	CToolCustomizer* pCustomizer = GetCustomizer();
	s_bRecalcLayoutPending=FALSE;

#if FIND_SLOW_CMDUI
	static UINT nMaxCmdUI=1;
#endif

	state.m_nIndexMax = (UINT)m_nCount;
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(state.m_nIndex);

		if (pTBB->nStyle & TBBS_HWND)
		{
			wndTemp.m_hWnd = GetButtonExtra(state.m_nIndex)->GetControl(); // quick and dirty attach
			if ((pCustomizer == NULL || pCustomizer->m_bTemp) &&
				(pTBB->nStyle & TBBS_ENABLEWND) == 0)
			{
				
				stateWnd.m_nID = (UINT)(WORD) ::GetDlgCtrlID(wndTemp.m_hWnd);
				stateWnd.m_pOther = &wndTemp;
				stateWnd.DoUpdate(pTarget, bDisableIfNoHndler &&
					(wndTemp.SendMessage(WM_GETDLGCODE) & DLGC_BUTTON) != 0);
				// only buttons get automagically disabled
			}
			else
			{
				wndTemp.EnableWindow(TRUE);
			}
		}
		
		// Ignore pressed buttons or separators
		// Let CToolCmdUI handle disabling and repaints for Hwnds too.
		//
		if (!(pTBB->nStyle & (TBBS_PRESSED|TBBS_SEPARATOR)))
		{
			if (pCustomizer == NULL || pCustomizer->m_bTemp || (pTBB->nStyle & TBBS_MENU))
			{
				state.m_nID = pTBB->nID;
				
#if FIND_SLOW_CMDUI
				int nTick=::GetTickCount();
#endif
				state.DoUpdate(pTarget, bDisableIfNoHndler);

#if FIND_SLOW_CMDUI
				int nNewTick=::GetTickCount();

				UINT nDiffTick=nNewTick-nTick;

				// Trace if it's in the slowest 10%
				if((double) nDiffTick > (double) nMaxCmdUI * 0.9)
				{
					TRACE2("CCustomBar::OnUpdateCmdUI: Cmd %d took %d ticks.\n\r", state.m_nID, nDiffTick);
				}

				if(nDiffTick>nMaxCmdUI)
				{
					nMaxCmdUI=nDiffTick;
				}
#endif
			}
			else
			{
				UINT nNewStyle =
					GetButtonStyle(state.m_nIndex) & ~TBBS_DISABLED;
				SetButtonStyle(state.m_nIndex, nNewStyle);
			}
		}
	}

	// If invisibility was changed, we need to recalc layout
	if(s_bRecalcLayoutPending && IsWindowVisible())
	{
		ASSERT((GetBarStyle() & CTBRS_MAINMENU)!=0);
		RecalcLayout(GetExpansionConstraint(CRect(0,0,0,0), NULL));
		Invalidate();
	}

	wndTemp.m_hWnd = NULL;      // quick and dirty detach
}

//      CCustomBar::OnSetCursor
//              Override to show the size cursor in customize mode.

BOOL CCustomBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CToolCustomizer* pCustomizer = GetCustomizer();
	if (pCustomizer != NULL)
	{
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		
		if (SizeTest(pt) != -1)
			::SetCursor(s_hcurVSize);
		else
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		
		return TRUE;
	}
	
	return CToolBar::OnSetCursor(pWnd, nHitTest, message);
}

//      CCustomBar::OnLButtonDown
//              Handles the customizing calls for button moving, and control sizing.

void CCustomBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nIndex;
	CToolCustomizer* pCustomizer = GetCustomizer();

	if (pCustomizer == NULL)
	{
		// Update the whole toolbar.  This is over-kill, but easy to code.
		CToolBar::OnIdleUpdateCmdUI((WPARAM) FALSE, 0L);

		if(IsMDIHost())
		{
			if(CheckMDIButtons(nFlags, point))
			{
				return;
			}
		}

		nIndex = HitTest(point);

#ifndef AUTOUPDATE_BUTTONS
		if (nIndex != -1 &&
			(nStyle & TBBS_DISABLED) != 0)
		{
			MessageBeep(-1);
			return;
		}
#endif
		
		if(nIndex!=-1)
		{
			AFX_TBBUTTON* pTBB = _GetButtonPtr(nIndex);

			if((pTBB->nStyle & (TBBS_MENU|TBBS_DISABLED))==TBBS_MENU)
			{
				// draw it pressed
				pTBB->nStyle |= TBBS_PRESSED;
				InvalidateButton(nIndex);
				UpdateWindow(); // immediate feedback

				// act on the pressing
				OnButtonPressed(nIndex);

				// draw it unpressed
				pTBB->nStyle &= ~TBBS_PRESSED;
				InvalidateButton(nIndex);
			}
			else
			{
				CToolBar::OnLButtonDown(nFlags, point);
			}
		}
		else
		{
			CToolBar::OnLButtonDown(nFlags, point);
		}
	}
	else
	{
		nIndex = SizeTest(point);

		if (nIndex != -1)
		{
			ClientToScreen(&point);
			pCustomizer->DoButtonSize(point, GetProxy(), nIndex);
		}
		else if ((nIndex = HitTest(point)) != -1)
		{
			// if it's a menu button, then drop it, unless it's already dropped
			AFX_TBBUTTON *pTBB=_GetButtonPtr(nIndex);
			
			ClientToScreen(&point);
			OnCancelMode();

			BOOL bDeletePending;

			BOOL bExpandPending=FALSE;

			// close menu before dragging starts
			if(pTBB->nStyle & TBBS_MENU)
			{
				if(theCmdCache.GetMenu(pTBB->nID)->m_hWnd)
				{
					ExpandItem(nIndex, FALSE);
				}
				else
				{
					if(     s_pDroppedBar!=NULL &&
						s_nDroppedItem!=-1)
					{
						s_pDroppedBar->ExpandItem(s_nDroppedItem, FALSE);
					}
					bExpandPending=TRUE;
				}
			}
					
			if(pCustomizer->DoButtonMove(point, GetProxy(), nIndex, &bDeletePending))
			{
				// something was moved - do no more
				if(bDeletePending)
				{
					// delete ourselves tidily - after this, we're in the member function of a deleted object, so must be
					// careful to do no more
					UINT nIDWnd = (UINT) GetDlgCtrlID();
					pCustomizer->m_pManager->RemoveWnd(MAKEDOCKID(PACKAGE_SUSHI, nIDWnd));
					return;
				}
			}
			else
			{
				// no move happened
				if(pTBB->nStyle & TBBS_MENU)
				{
					if(bExpandPending)
					{
						ExpandItem(nIndex, TRUE);
					}
				}
			}
		}
	}
}

//      CCustomBar::OnLButtonUp
//              Handles the customizing calls for button moving, and control sizing.

void CCustomBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// handle MDI tracking separately
	if(m_nMDITrack>=cMDIDragging)
	{
		ASSERT(IsMDIHost());

		if(m_nMDITrack!=cMDIDragPending)
		{
			ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

			CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

			BOOL bMaximized;
			CMDIChildWnd *pActive=pMainWnd->MDIGetActive(&bMaximized);

			switch(m_nMDITrack)
			{
				case cMDIRestore:
					pActive->MDIRestore();
					break;
				case cMDIMinimize:
					pActive->CloseWindow();
					break;
				case cMDIClose:
					pActive->SendMessage(WM_CLOSE,0,0);
					break;
			}

			int nHit=m_nMDITrack;
			
			m_nMDITrack=cMDINoButton;

			if(nHit!=cMDIDragPending)
			{
				CRect buttons[cMDIRectCount];

				GetMDIButtonRects(buttons);

				InvalidateRect(buttons[nHit]);
				UpdateWindow();
			}
		}
		else
		{
			m_nMDITrack=cMDINoButton;
		}
		if(GetCapture()==this)
		{
			ReleaseCapture();
		}
	}
	else
	{
		CToolBar::OnLButtonUp(nFlags, point);
	}
}

//      CCustomBar::OnLButtonDblClk
//              Close icon in menu bars.

void CCustomBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// handle MDI tracking separately
	if(IsMDIHost() && HitTestMDI(point)==cMDIIcon)
	{
		// double click to close window

		ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

		CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

		BOOL bMaximized;
		CWnd *pActive=pMainWnd->MDIGetActive(&bMaximized);

		pActive->SendMessage(WM_SYSCOMMAND, SC_CLOSE, MAKELONG(-1, -1));
	}
	else
	{
		CToolBar::OnLButtonDblClk(nFlags, point);
	}
}

BOOL CCustomBar::OnButtonPressed(int iButton)
{
	AFX_TBBUTTON* pTBB = _GetButtonPtr(iButton);

	// if it's an enabled menu, then pull
	if(     (pTBB->nStyle & TBBS_MENU)!=0 &&
		(pTBB->nStyle & TBBS_DISABLED)==0)
	{
		CMainFrame *pFrame=(CMainFrame *)theApp.m_pMainWnd;

		if (theApp.m_bMenuDirty)
		{
			pFrame->RebuildMenus();
		}

		ExpandItem(iButton,TRUE);

		return TRUE;
	}
	else
	{
		return CToolBar::OnButtonPressed(iButton);
	}
}

void CCustomBar::UpdateButton(int nIndex)
{
	CToolBar::UpdateButton(nIndex);
}

//      CCustomBar::OnRButtonDown
//              Handles the potential popup menu for customising toolbar buttons.

void CCustomBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	CToolBar::OnRButtonDown(nFlags, point);
}

//      CCustomBar::OnGetMinMaxInfo
//              For dock sizing.

void CCustomBar::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	CToolBar::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = m_sizeFixedLayout.cx;
	lpMMI->ptMinTrackSize.y = m_sizeFixedLayout.cy;
	lpMMI->ptMaxTrackSize.x = m_sizeMaxLayout.cx;
	lpMMI->ptMaxTrackSize.y = m_sizeMaxLayout.cy;
}

void CCustomBar::OnSize(UINT nType, int cx, int cy)
{
    if (nType == SIZE_RESTORED)
    {
		CRect rect, rectWnd;
		CWnd* pWnd;

		// Count the number of controls int the toolbar.
		int nControls = 0;
		AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
		for (int iButton = 0; iButton < m_nCount; iButton++, pTBB++)
		{
			ASSERT(pTBB != NULL);
			if ((pTBB->nStyle & TBBS_HWND) == 0)
				continue;

			pWnd = CWnd::FromHandle(GetButtonExtra(iButton)->GetControl());
			ASSERT(pWnd != NULL);

			if (!IsValidCtrlSize(pWnd))
			{
				CWnd* pWndFocus = GetFocus();
				if (pWnd == pWndFocus || pWnd->IsChild(pWndFocus))
					LoseFocus();

				pWnd->ShowWindow(SW_HIDE);
			}
			else
			{
				pWnd->ShowWindow(SW_SHOWNOACTIVATE);
				nControls++;
			}
		}

		if (nControls == 0)
			return;
		
	    HDWP hdwp = ::BeginDeferWindowPos(nControls);
		pTBB = (AFX_TBBUTTON*)m_pData;
		for (iButton = 0; iButton < m_nCount; iButton++, pTBB++)
		{
			ASSERT(pTBB != NULL);
			if ((pTBB->nStyle & TBBS_HWND) == 0)
				continue;

			pWnd = CWnd::FromHandle(GetButtonExtra(iButton)->GetControl());
			ASSERT(pWnd != NULL);

			pWnd->GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			GetItemRect(iButton, rect, GetOrient());
		
			CRect rectDrop = rect;
			::GetDroppedRect(pWnd->m_hWnd, rectDrop);

			// z-order so updates in DoPaint don't cause trash when controls
			// overlap.
	    hdwp = ::DeferWindowPos(hdwp, pWnd->GetSafeHwnd(), HWND_BOTTOM,
		rect.left, rect.top, rect.Width(), rectDrop.Height(),
		SWP_NOACTIVATE);
		}

		// Fix for selection hiding and 3d bug.  Make sure everything stays
		// invalid without flashing.
		if ((GetStyle() & WS_VISIBLE) == 0)
			::EndDeferWindowPos(hdwp);
		else
		{
			ShowWindow(SW_HIDE);
			::EndDeferWindowPos(hdwp);
			ShowWindow(SW_SHOWNOACTIVATE);
		}
	}
}

//      CCustomBar::OnSizeParent
//              Not sure we still get called with this one, but we certainly don't
//              want the standard MFC behavior here.

LRESULT CCustomBar::OnSizeParent(UINT, LPARAM)
{
    return 0;
}

//      CCustomBar::OnIdleUpdateCmdUI
//              Speed up captures.

LRESULT CCustomBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
#ifdef AUTOUPDATE_BUTTONS
	// ignore if mouse capture on someone else, and not on one of our children, or we're hidden
	CWnd *pWndCapture=GetCapture();
    IMsoComponentHost * pCompHost = theApp.GetStdCompMgr()->GetIMsoComponentHost();
	if (!pCompHost->FContinueIdle() ||
		!IsWindowVisible() || 
		(pWndCapture != NULL && pWndCapture != this && !IsChild(pWndCapture)))
#endif
		return 0L;
		
	return CToolBar::OnIdleUpdateCmdUI(wParam, lParam);
}

//      CCustomBar::OnDestroy
//              Remove our control windows.

void CCustomBar::OnDestroy()
{
	RemoveWindows();
	
	CToolBar::OnDestroy();
}

// Override to allow hwnd buttons to nominate an id for their tooltip, depending
// on their context
int CCustomBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);

	// check id using standard toolbar code
	int nHit = CToolBar::OnToolHitTest(point, pTI);
	if (nHit != -1)
	{
		int iButton = HitTest(point);
		if (iButton != -1)
		{
			AFX_TBBUTTON* pTBB = _GetButtonPtr(iButton);

			BOOL bWindowVisible=FALSE;

			// menu buttons don't get tooltips
			if(pTBB->nStyle & TBBS_MENU)
			{
				nHit=-1;
			}
		}
	}
	return nHit;
}

void CCustomBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// handle MDI tracking separately
	if(m_nMDITrack>=cMDIDragging)
	{
		ASSERT(IsMDIHost());
		if(GetCapture()!=this)
		{
			SetCapture();
		}
		//
		int nHit=HitTestMDI(point);

		// don't act on these two, but remember we are dragging
		if(nHit==cMDIIcon ||
			nHit==cMDINoButton)
		{
			nHit=cMDIDragPending;
		}

		if(nHit!=m_nMDITrack)
		{
			CRect buttons[cMDIRectCount];

			GetMDIButtonRects(buttons);

			if(m_nMDITrack!=cMDIDragPending)
			{
				InvalidateRect(buttons[m_nMDITrack]);
			}

			m_nMDITrack=nHit;

			if(m_nMDITrack!=cMDIDragPending)
			{
				InvalidateRect(buttons[m_nMDITrack]);
			}
			UpdateWindow();
		}
	}

	// adjust dropped menu if appropriate
	if( s_pDroppedBar==this && 
		GetCustomizer()==NULL)
	{
		AFX_TBBUTTON* pTBB =NULL;
		if(s_nDroppedItem<m_nCount)
		{
			pTBB=_GetButtonPtr(s_nDroppedItem);
		}
		int iCurrentHit=HitTest(point);
		if(iCurrentHit==-1)
		{
			// If no bar button was hit, maybe we floated over the mdi system icon.
			int nMDIHit=HitTestMDI(point);
			if(nMDIHit==cMDIIcon)
			{
				iCurrentHit=m_nCount;
			}
		}

		// if dropped item is positive, we're mid push, and need to transfer the push allegiance
		if(s_nDroppedItem>=0)
		{
			if(iCurrentHit!=-1 && iCurrentHit!=s_nDroppedItem)
			{
				// will point at button, unless we are over mdi icon
				AFX_TBBUTTON* pTBBNew = NULL;

				if(iCurrentHit<m_nCount)
				{
					pTBBNew=_GetButtonPtr(iCurrentHit);
				}

				BOOL bPost=FALSE;

				if(pTBB)
				{
					if((pTBB->nStyle & TBBS_DISABLED)==0)
					{
						// what was the old thing?
						if(pTBB->nStyle & TBBS_MENU)
						{
							ExpandItem(s_nDroppedItem,FALSE);
							bPost=TRUE;
						}
						else
						{
							//deextrude it
							pTBB->nStyle &= ~(TBBS_EXTRUDED);
							InvalidateButton(s_nDroppedItem);
						}
					}
				}
				else
				{
					ASSERT(s_nDroppedItem==m_nCount);
					// if the menu is open, close it.
					if(s_pSystemMenu)
					{
						ExpandItem(s_nDroppedItem,FALSE);
						bPost=TRUE;
					}
				}

				if(iCurrentHit!=-1)
				{
					if(pTBBNew)
					{
						if((pTBBNew->nStyle & TBBS_DISABLED)==0)
						{
							// we hit something new; what is it?
							if(pTBBNew->nStyle & TBBS_MENU)
							{
								if(bPost)
								{
									// we have to post here so that the new menu's modal loop isn't started inside the
									// old menu's message handler.
									PostMessage(DSM_DROPMENU, iCurrentHit, FALSE);
								}
								else
								{
									ExpandItem(iCurrentHit, TRUE);
								}
							}
							else
							{
								pTBBNew->nStyle |= TBBS_EXTRUDED;
								InvalidateButton(iCurrentHit);
							}
						}
					}
					else
					{
						// system menu
						ASSERT(iCurrentHit==m_nCount);

						if(bPost)
						{
							// we have to post here so that the new menu's modal loop isn't started inside the
							// old menu's message handler.
							PostMessage(DSM_DROPMENU, iCurrentHit, FALSE);
						}
						else
						{
							ExpandItem(iCurrentHit, TRUE);
						}
					}
					if(m_iButtonCapture==s_nDroppedItem &&
						m_iButtonCapture!=-1)
					{
						// we're in a push operation, so adjust the capture too
						m_iButtonCapture=iCurrentHit;
					}
				}

				if(!bPost)
				{
					s_nDroppedItem=iCurrentHit; // note that this allows m_iDroppedItem to get set to
												// a nonmenu during a drag. Hmmm.
				}
			}
		}
	}

	// call base class to do usual stuff.
	CToolBar::OnMouseMove(nFlags, point);
}

// don't extrude if we're a hwnd button
BOOL CCustomBar::ShouldExtrude(int iButton)
{
	return !IsVisibleControl(iButton) && CToolBar::ShouldExtrude(iButton);
}

TBBUTTON_EXTRA *CCustomBar::EditButtonExtra(int iButton)
{ 
	if(!HasButtonExtra())
	{
		m_pButtonExtra= new TBBUTTON_EXTRA[m_nCount];
	}
	ASSERT(HasButtonExtra());
	
	return &(m_pButtonExtra[iButton]);
};

// called when the toolbar scroll bar is hit
// used only in customisation mode
void CCustomBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// should be from our own bar
	ASSERT(pScrollBar==NULL);

	int offset=0;                                                   // ammount to offset the bar by
	int current=GetScrollPos(SB_VERT);              // position before the move
	int minScroll; 
	int maxScroll;                                                  // range of the bar
	CRect rectTools;                                                // tools pane size (used as 'page' size)

	GetWindowRect(rectTools);
	GetScrollRange(SB_VERT,&minScroll, &maxScroll);

	ASSERT(minScroll==0);

	CSize sizeButton=GetButtonSize(GetOrient());

	switch(nSBCode) {
	case SB_BOTTOM:
		offset=maxScroll-current;
		break;

	case SB_LINEDOWN:
		offset= sizeButton.cy+m_cySharedBorder;
		break;

	case SB_LINEUP:
		offset= -(sizeButton.cy+m_cySharedBorder);
		break;

	case SB_PAGEDOWN:
		offset=rectTools.Size().cy;
		break;

	case SB_PAGEUP:
		offset=-rectTools.Size().cy;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		offset=nPos-current;
		break;

	case SB_TOP: 
		offset=-current;
		break;
	}

	// check for going outside range
	if(current+offset<0) {
		offset=-current;
	}
	if(current+offset>maxScroll) {
		offset=maxScroll-current;
	}

	// scroll the bar, and the toolbar
	SetScrollPos(SB_VERT,current+offset);
	ScrollWindow(0,-offset);
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CCustomBar diagnostics

#ifdef _DEBUG
void CCustomBar::AssertValid() const
{
	// deliberate skipping of base class, because we changed the assumptions... 
	CWnd::AssertValid();

	ASSERT(m_nCount == 0 || m_pData != NULL);

	AFX_TBBUTTON* pTBB = (AFX_TBBUTTON*)m_pData;
	for(int iButton=0; iButton <m_nCount; ++iButton, ++pTBB)
	{
		// invisible button could be anything.
		if(pTBB->nStyle & TBBS_INVISIBLE)
		{
			continue;
		}

		if((pTBB->nStyle & TBBS_SEPARATOR)==0)
		{
			ASSERT((pTBB->nStyle & TBBS_GLYPH)!=0 || (pTBB->nStyle & TBBS_TEXTUAL)!=0);
			ASSERT(pTBB->nID!=0);
		}

		if(pTBB->nStyle & TBBS_TEXTUAL)
		{
			ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);

			ASSERT(!GetButtonExtra(iButton)->GetLabel().IsEmpty());
		}

		if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
		{
			ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);
		}

		if(pTBB->nStyle & TBBS_CUSTOMTEXT)
		{
			ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);
			ASSERT((pTBB->nStyle & (TBBS_TEXTUAL))!=0);
		}

		if(pTBB->nStyle & TBBS_HWND)
		{
			ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);
		}

		if(pTBB->nStyle & TBBS_SEPARATOR)
		{
			ASSERT((pTBB->nStyle & (TBBS_CUSTOMBITMAP|TBBS_CUSTOMTEXT|TBBS_SIZABLE|TBBS_TEXTUAL|TBBS_HWND|TBBS_MENU))==0);
		}

		if(pTBB->nStyle & TBBS_SIZABLE)
		{
			ASSERT(pTBB->nStyle & TBBS_HWND);
			ASSERT((pTBB->nStyle & (TBBS_SEPARATOR))==0);
		}

		if(pTBB->nStyle & TBBS_GLYPH)
		{
			ASSERT(pTBB->iImage>=0);
			ASSERT(pTBB->iImage<m_nTiles);
		}
	}
}
#endif

void GetDroppedRect(HWND hwnd, LPRECT lpRect)
{
	CString strClass;
	::GetClassName(hwnd, strClass.GetBuffer(16), 16);
	strClass.ReleaseBuffer();

	if (strClass.CompareNoCase("ComboBox") == 0)
	{
		::SendMessage(hwnd, CB_GETDROPPEDCONTROLRECT, 0,
			(LPARAM) (LPVOID) lpRect);
	}
}

// TRUE if this bar is currently required to host the MDI gadgets
BOOL CCustomBar::IsMDIHost(void) const
{
	if(m_dwStyle & CTBRS_MAINMENU)
	{
		ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

		CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

		BOOL bMaximized;
		pMainWnd->MDIGetActive(&bMaximized);
		if(bMaximized)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void CCustomBar::UpdateMDIIcon()
{
	if(m_hIconMDI)
	{
		::DestroyIcon(m_hIconMDI);
		m_hIconMDI=NULL;
	}
	if(IsMDIHost())
	{
		CRect buttons[cMDIRectCount];

		GetMDIButtonRects(buttons);
		
		InvalidateRect(buttons[cMDIIcon]);
	}
}

int CCustomBar::GetLeftBorder(ORIENTATION or) const
{
	if (or == orHorz)
	{
		return cGrabberWidth;
	}
	else
	{
		return 1;
	}
}

int CCustomBar::GetRightBorder(ORIENTATION or) const
{
	return 1;
}

int CCustomBar::GetTopBorder(ORIENTATION or) const
{
	if (or == orVert)
	{
		return cGrabberHeight;
	}
	else
	{
		return 3;
	}
}

int CCustomBar::GetBottomBorder(ORIENTATION or) const
{
	return 3;
}

int CCustomBar::GetLeftMDIArea(ORIENTATION or) const
{
	if (or != orVert && IsMDIHost())
	{
		return globalData.csSmallIcon.cx;
	}
	else
	{
		return 0;
	}
}

int CCustomBar::GetRightMDIArea(ORIENTATION or) const
{
	if(or!=orVert && IsMDIHost())
	{
		// 3 buttons, plus a space between 2 of them and the close button
		return 3*globalData.csCaptionButton.cx+globalData.cxPreCloseGap+globalData.cxPostCloseGap+globalData.cxPreMinimizeGap;
	}
	else
	{
		return 0;
	}
}

int CCustomBar::GetTopMDIArea(ORIENTATION or) const
{
	if (or == orVert && IsMDIHost())
	{
		return globalData.csSmallIcon.cy;
	}
	else
	{
		return 0;
	}
}

int CCustomBar::GetBottomMDIArea(ORIENTATION or) const
{
	if(or==orVert && IsMDIHost())
	{
		// 3 buttons, plus a space between 2 of them and the close button
		return 3*globalData.csCaptionButton.cy+globalData.cyPreCloseGap+globalData.cyPostCloseGap+globalData.cyPreMinimizeGap;
	}
	else
	{
		return 0;
	}
}

void CCustomBar::DrawGrabber(CDC* pDC)
{
	CRect rc;
	ORIENTATION or = GetOrient();
	GetClientRect(rc);

	if (or == orHorz)
	{
		pDC->FillSolidRect(rc.left, rc.top, 8, rc.Height(), globalData.clrBtnFace);
		pDC->Draw3dRect(rc.left + 1, rc.top + 1, 3, rc.Height() - 2,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
		pDC->Draw3dRect(rc.left + 5, rc.top + 1, 3, rc.Height() - 2,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
	}
	else if (or == orVert)
	{
		pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 8, globalData.clrBtnFace);
		pDC->Draw3dRect(rc.left + 1, rc.top + 1, rc.Width() - 2, 3,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
		pDC->Draw3dRect(rc.left + 1, rc.top + 5, rc.Width() - 2, 3,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
	}
}

// Stolen and adapted from Office 97 (tbsys.cpp: FDrawMDISystemIcon)
/*----------------------------------------------------------------------------
	TBS::FDrawMDISystemIcon

	Does its best to draw the document icon in the given hdc and rectangle.
------------------------------------------------------------------- JBELT --*/
BOOL CCustomBar::DrawMDIDocumentIcon(CDC* pDC, const CRect &rectIcon)
{
	// get the MDI child window
	// if didn't get the icon yet, get it now
	// I don't believe this icon can change on the fly so I'm caching it forever
	if (!m_hIconMDI)
	{
		HICON hicon = NULL;

		// try getting it from the document window
		ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

		CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

		BOOL bMaximized;
		CWnd *pWnd=pMainWnd->MDIGetActive(&bMaximized);

		if (pWnd->GetSafeHwnd()!=NULL)
		{
			if ((hicon = (HICON) pWnd->SendMessage(WM_GETICON, FALSE, 0)) != NULL)
				goto LGotIcon;
			if ((hicon = (HICON) GetClassLong(pWnd->GetSafeHwnd(), GCL_HICONSM)) != NULL)
				goto LGotIcon;
		}

		// last resort: get it from the app itself (this will be the app icon)
		if (pMainWnd)
		{
			if ((hicon = (HICON) pMainWnd->SendMessage(WM_GETICON, FALSE, 0)) != NULL)
				goto LGotIcon;
			if ((hicon = (HICON) GetClassLong(pWnd->GetSafeHwnd(), GCL_HICONSM)) != NULL)
				goto LGotIcon;
		}

		return FALSE;
LGotIcon:
		/* Got the Icon, lets make a copy of it */
		ASSERT(hicon != NULL);
		if ((m_hIconMDI = CopyIcon(hicon)) == NULL)
			return FALSE;
	}

	// draw it
	ASSERT(m_hIconMDI);

	::DrawIconEx(pDC->GetSafeHdc(), rectIcon.left, rectIcon.top, m_hIconMDI, globalData.csSmallIcon.cx, globalData.csSmallIcon.cy, 0, globalData.hbrBtnFace,0);
	return TRUE;
}

void CCustomBar::DrawMDIControls(CDC* pDC)
{
	ASSERT(IsMDIHost());

	ORIENTATION or=GetOrient();

	CRect rects[cMDIRectCount];

	GetMDIButtonRects(rects);

	DrawMDIDocumentIcon(pDC, rects[cMDIIcon]);
	pDC->DrawFrameControl(rects[cMDIClose], DFC_CAPTION, DFCS_CAPTIONCLOSE | (m_nMDITrack==cMDIClose ? DFCS_PUSHED : 0));
	pDC->DrawFrameControl(rects[cMDIMinimize], DFC_CAPTION, DFCS_CAPTIONMIN | (m_nMDITrack==cMDIMinimize ? DFCS_PUSHED : 0));
	pDC->DrawFrameControl(rects[cMDIRestore], DFC_CAPTION, DFCS_CAPTIONRESTORE | (m_nMDITrack==cMDIRestore ? DFCS_PUSHED : 0));
}

// tests and acts on 
BOOL CCustomBar::CheckMDIButtons(UINT nFlags, CPoint point)
{
	MDIBUTTON nHit=HitTestMDI(point);

	ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

	CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

	BOOL bMaximized;
	CWnd *pActive=pMainWnd->MDIGetActive(&bMaximized);

	switch(nHit)
	{
		default:
			ASSERT(FALSE);
			// drop thru
		case cMDINoButton:
			return FALSE;

		case cMDIIcon:
			if(pActive)
			{
				// prime for potential double click
				CPoint ptScreen=point;
				ClientToScreen(&ptScreen);
				CBMenuPopup::SetSystemClick(GetMessageTime(), ptScreen);
				pActive->SendMessage(WM_SYSCOMMAND, SC_MOUSEMENU, MAKELONG(ptScreen.x, ptScreen.y));
			}
			break;

		case cMDIMinimize:
		case cMDIRestore:
		case cMDIClose:
			{
				// start mdi tracking
				m_nMDITrack=nHit;
				CRect buttons[cMDIRectCount];

				GetMDIButtonRects(buttons);
				
				InvalidateRect(buttons[nHit]);
				UpdateWindow();

				SetCapture();
			}
			break;
	}

	return TRUE;
}

// gets the rects of the mdi buttons
void CCustomBar::GetMDIButtonRects(CRect rects[cMDIRectCount])
{
	ORIENTATION or=GetOrient();

	CSize sizeButton=GetButtonSize(or);

	int rowHeight=m_cyDefaultGap+sizeButton.cy;
	int colWidth=m_cxDefaultGap+sizeButton.cx;

	// draw the doc icons at the right edge of the left border, or the bottom edge of the top border
	if(or!=orVert)
	{
		rects[cMDIIcon].left=GetLeftBorder(or);
		rects[cMDIIcon].right=rects[cMDIIcon].left+globalData.csSmallIcon.cx;
		rects[cMDIIcon].top=(rowHeight-globalData.csSmallIcon.cy)/2;
		rects[cMDIIcon].bottom=rects[cMDIIcon].top+globalData.csSmallIcon.cy;
	}
	else
	{
		rects[cMDIIcon].top=GetTopBorder(or);
		rects[cMDIIcon].bottom=rects[cMDIIcon].top+globalData.csSmallIcon.cy;
		rects[cMDIIcon].left=(colWidth-globalData.csSmallIcon.cx)/2;
		rects[cMDIIcon].right=rects[cMDIIcon].left+globalData.csSmallIcon.cx;
	}

	// now the three control icons
	// start with the close one at the right, and move leftwards, wrapping upwards if necessary.
	// Never split up the two maximize/restore buttons
	CRect rectClient;

	GetClientRect(rectClient);

	if(or!=orVert)
	{
		rects[cMDIClose].bottom=rectClient.bottom-(rowHeight-globalData.csCaptionButton.cy)/2;
		rects[cMDIClose].top=rects[cMDIClose].bottom- globalData.csCaptionButton.cy;

		rects[cMDIClose].right=rectClient.right-globalData.cxPostCloseGap;
		rects[cMDIClose].left=rects[cMDIClose].right-globalData.csCaptionButton.cx;
	}
	else
	{
		rects[cMDIClose].right=rectClient.right-(colWidth-globalData.csCaptionButton.cx)/2;
		rects[cMDIClose].left=rects[cMDIClose].right - globalData.csCaptionButton.cx;

		rects[cMDIClose].bottom=rectClient.bottom-globalData.cyPostCloseGap;
		rects[cMDIClose].top=rects[cMDIClose].bottom-globalData.csCaptionButton.cy;
	}

	// the two buttons are always minimize and restore, because we are always maximized. If we're not
	// maximized, then we won't be drawing the buttons on the bar.

	rects[cMDIMinimize]=rects[cMDIClose];

	if(or!=orVert)
	{
		rects[cMDIMinimize].OffsetRect(-2*(globalData.csCaptionButton.cx)-globalData.cxPreCloseGap,0);

		if(rects[cMDIMinimize].left<globalData.cxPreMinimizeGap)
		{       
			// wrap to previous line
			rects[cMDIMinimize].top-=(sizeButton.cy + m_cyDefaultGap);
			rects[cMDIMinimize].bottom-=(sizeButton.cy + m_cyDefaultGap);
			rects[cMDIMinimize].right=rectClient.right-globalData.cxPostCloseGap;
			rects[cMDIMinimize].left=rects[cMDIMinimize].right-2*globalData.csCaptionButton.cx;
		}
	}
	else
	{
		rects[cMDIMinimize].OffsetRect(0,-2*(globalData.csCaptionButton.cy)-globalData.cyPreCloseGap);

		// for the moment, vbars don't wrap, so no need to watch wrap here
	}

	rects[cMDIRestore]=rects[cMDIMinimize];
	if(or!=orVert)
	{
		rects[cMDIRestore].OffsetRect(globalData.csCaptionButton.cx,0);
	}
	else
	{
		rects[cMDIRestore].OffsetRect(0,globalData.csCaptionButton.cy);
	}
}

// checks if an MDI button was hit
CCustomBar::MDIBUTTON CCustomBar::HitTestMDI(CPoint point)
{
	CRect buttons[cMDIRectCount];

	GetMDIButtonRects(buttons);

	for(MDIBUTTON iButton=cMDIRectFirst; iButton<=cMDIRectLast; iButton=(MDIBUTTON)(((int)iButton)+1))
	{
		if(buttons[iButton].PtInRect(point))
		{
			return iButton;
		}
	}

	return cMDINoButton;
}

// cancels this menu, or any menu that contains it
void CCustomBar::HideInvalidDestination(UINT nId)
{
	if(s_pDroppedBar)
	{
		ASSERT(s_nDroppedItem!=-1);
		s_pDroppedBar->HideInvalidBarDestination(nId);
	}
}

// implements specific menu cancelling for bars
void CCustomBar::HideInvalidBarDestination(UINT nId)
{
	ASSERT(s_nDroppedItem>=0);

	if(s_nDroppedItem<m_nCount)
	{
		AFX_TBBUTTON *pTBB=_GetButtonPtr(s_nDroppedItem);

		if(pTBB->nID==nId)
		{
			CancelMenu();
		}
		else
		{
			CBMenuPopup *pMenuTest=theCmdCache.GetMenu(nId);
			if(pMenuTest->ContainsMenu(pTBB->nID))
			{
				CancelMenu();
			}
			else
			{
				CBMenuPopup *pMenu=GetItemMenu(s_nDroppedItem);

				ASSERT(pMenu);
				pMenu->HideInvalidMenuDestination(nId);
			}
		}
	}
}

// cancels any open menu
void CCustomBar::CancelMenu(void)
{
	if(s_pDroppedBar)
	{
		ASSERT(s_nDroppedItem!=-1);
		s_pDroppedBar->ExpandItem(s_nDroppedItem, FALSE);
	}
}

// Expand now copes with expanding m_nIndex, which gets us the mdi child menu
int CCustomBar::ExpandItem(int nIndex, BOOL bExpand, BOOL bKeyboard)
{
	// pull up whatever was down
	if(bExpand && s_pDroppedBar!=NULL)
	{
		// can't recurse badly, because bExpand will be false.
		s_pDroppedBar->ExpandItem(s_nDroppedItem, FALSE);

		if(!theApp.IsCustomizing())
		{
			// POST ourselves a message causing the real expansion. May not be necessary, but helps protect against
			// bad recursion bugs.
			PostMessage(DSM_DROPMENU, nIndex, bKeyboard);

			return 0;
		}
	}

	// must be a valid button index or m_nCount
	ASSERT(nIndex>=0);
	ASSERT(nIndex< m_nCount || nIndex==m_nCount);

	AFX_TBBUTTON* pTBB=NULL;
	CBMenuPopup* pPopup=NULL;
	CWnd *pMDIChild=NULL;
	BOOL bMaximized=FALSE;

	// The window which the command will be sent to; used to allow mdi menu to route messages to mdi child
	CWnd *pPostTarget=NULL;

	if(nIndex==m_nCount && (GetBarStyle() & CTBRS_MAINMENU)!=0)
	{
		if(bExpand)
		{
			if(s_pSystemMenu!=NULL)
			{
				// menu is already up, so quit.
				return m_nCount;
			}

			// duplicate the system menu

			// Find active child
			CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

			pMDIChild=pMainWnd->MDIGetActive(&bMaximized);
			
			if(pMDIChild)
			{
				pPostTarget=pMDIChild;

				CMenu *pSourceMenu=pMDIChild->GetSystemMenu(FALSE);

				s_pSystemMenu=new CBMenuPopup;

				// copy menu items 
				int nSourceItems=pSourceMenu->GetMenuItemCount();
				for(int iSource=0;iSource<nSourceItems;++iSource)
				{
					UINT nID=pSourceMenu->GetMenuItemID(iSource);

					// don't expect submenus on the main menu
					ASSERT(nID!=-1);

					if(nID==0)
					{
						s_pSystemMenu->InsertMenu(-1, MF_BYPOSITION | MF_SEPARATOR, 0);
					}
					else
					{
						CString menuName;
						pSourceMenu->GetMenuString(iSource, menuName, MF_BYPOSITION);

						UINT nFlags=pSourceMenu->GetMenuState(iSource, MF_BYPOSITION);
						s_pSystemMenu->InsertMenu(-1, MF_BYPOSITION|nFlags, nID, menuName);
					}
				}

				// make sure we see cancellation notices
				s_pSystemMenu->SetPassBackCancel(TRUE);
			}
			else
			{
				nIndex=0;
			}
		}
		pPopup=s_pSystemMenu;
	}
	
	BOOL bReturnCmd=FALSE;

	if(nIndex<m_nCount)
	{
		pTBB= _GetButtonPtr(nIndex);
		ASSERT(pTBB->nStyle & TBBS_MENU);
		pPopup = theCmdCache.GetMenu(pTBB->nID);
		ASSERT(pPopup != NULL);
	}
	else
	{
		// mdi always gets the command back, so it can be sent to the MDI child.
		if(pPostTarget)
		{
			bReturnCmd=TRUE;
		}
	}

	CSize offset;

	// hide it or show it?
	if (bExpand)
	{
		if (pPopup->GetSafeHwnd() != NULL)
		{
			return -1;
		}

		RECT rectAvoid=CRect(0,0,0,0);

		int xPos=0;
		int yPos=0;
		if(pTBB || bMaximized)
		{
			if(pTBB)
			{
				// draw it pressed
				pTBB->nStyle |= TBBS_PRESSED;
				InvalidateButton(nIndex);

				// drop the menu.
				GetItemRect(nIndex, &rectAvoid, GetOrient());
				ClientToScreen(&rectAvoid);
			}
			else
			{
				CRect buttons[cMDIRectCount];

				GetMDIButtonRects(buttons);

				ClientToScreen(buttons[cMDIIcon]);

				rectAvoid=buttons[cMDIIcon];
			}
			ORIENTATION or=GetOrient();
			xPos = (or == orVert) ? rectAvoid.right+1 : rectAvoid.left-1;
			yPos = (or == orVert) ? rectAvoid.top-1 : rectAvoid.bottom+1;
		}
		else
		{
			ASSERT(pMDIChild);

			if(pMDIChild->IsIconic())
			{
				CRect rcWindow;
				pMDIChild->GetWindowRect(&rcWindow);

				// initially try it below the mdi lump
				yPos=rcWindow.bottom;
				xPos=rcWindow.left;

				// but always avoid the mdi lump
				rectAvoid=rcWindow;

			}
			else
			{
				CRect rcChild;
				pMDIChild->GetClientRect(&rcChild);
				pMDIChild->ClientToScreen(&rcChild);
				CRect rcWindow;
				pMDIChild->GetWindowRect(&rcWindow);

				// initially try it at the top left of the client
				yPos=rcChild.top;
				xPos=rcChild.left;

				// but always avoid the title bar
				rectAvoid.top=rcWindow.top;
				rectAvoid.left=rcWindow.left;
				rectAvoid.right=rcWindow.right;
				rectAvoid.bottom=rcChild.top;
			}
		}

		if(GetCapture()==this)
		{
			ReleaseCapture();
		}

		ASSERT(nIndex!=-1);
		s_nDroppedItem=nIndex;
		s_pDroppedBar=this;
		BOOL bBarFocus=FALSE;
		UINT nCmd=pPopup->TrackDropDownMenu(      xPos, yPos, theApp.m_pMainWnd, NULL, rectAvoid,
									this,   // The menu will forward mouse and keyboard messages to this window.
									bKeyboard,  // When navagating by keyboard, the first menu item is selected.
									&bBarFocus,
									bReturnCmd
									);

		if(bReturnCmd && pPostTarget && nCmd!=0)
		{
			pPostTarget->PostMessage(WM_SYSCOMMAND, nCmd);
		}

		if(s_pSystemMenu)
		{
			delete s_pSystemMenu;
			s_pSystemMenu=NULL;
		}

		return bBarFocus ? nIndex : -1;
	}
	else
	{
		if (pPopup->GetSafeHwnd() != NULL)
			pPopup->Done(0);

		s_nDroppedItem=-1;
		s_pDroppedBar=NULL;

		if(pTBB)
		{
			// unpress the button.
			pTBB->nStyle &= ~TBBS_PRESSED;
			InvalidateButton(nIndex);
		}

		return -1;
	}
}

// menu calls this when it is completed, to stop the bar tracking
void CCustomBar::MenuDone()
{
	if(s_pDroppedBar==this && s_nDroppedItem!=-1 && s_nDroppedItem!=m_nCount)
	{
		AFX_TBBUTTON* pTBB = _GetButtonPtr(s_nDroppedItem);
		pTBB->nStyle &= ~(TBBS_PRESSED|TBBS_EXTRUDED);
		InvalidateButton(s_nDroppedItem);
	}
	m_iButtonCapture=s_nDroppedItem=-1;
	s_pDroppedBar=NULL;
}


// true if the item can receive the focus in key mode 
BOOL CCustomBar::CanFocus(int iButton)
{
	if(iButton == m_nCount &&
		GetBarStyle() & CTBRS_MAINMENU)
	{
		return TRUE;
	}

	if(IsVisibleControl(iButton))
	{
		return FALSE;
	}
	else
	{
		return (GetButtonStyle(iButton) & (TBBS_SEPARATOR|TBBS_INVISIBLE|TBBS_DISABLED))==0;
	}
};


BOOL CCustomBar::IsVisibleControl(int iButton) const
{ 
	if((GetButtonStyle(iButton) & TBBS_HWND)==0)
	{
		return FALSE;
	}
	else
	{
		return ::IsWindowVisible(GetButtonExtra(iButton)->GetControl());
	}
};

void CCustomBar::AdjustMenuBar()
{ 
	ASSERT(m_dwStyle & CTBRS_MAINMENU);

	ORIENTATION or=GetOrient();
	if(or!=orNil)
	{
		RecalcLayout();
	}
}

CBMenuPopup *CCustomBar::GetItemMenu(int iButton)
{
	// must be a valid button index or m_nCount
	ASSERT(iButton>=0);
	ASSERT(iButton< m_nCount || iButton==m_nCount);

	if(iButton==m_nCount)
	{
		return s_pSystemMenu;
	}
	else
	{
		return theCmdCache.GetMenu(GetItemID(iButton));
	}
}

afx_msg void CCustomBar::OnKillFocus(CWnd *pNew)
{
	if(s_pDroppedBar!=NULL)
	{
		MenuDone();
	}
	CToolBar::OnKillFocus(pNew);
}

// FindButton:
//	Checks if a given command id is present somewhere in the bar.
BOOL CCustomBar::FindButton(UINT nId) const
{
	AFX_TBBUTTON *pTBB=_GetButtonPtr(0);

	for(int i=0;i<m_nCount;++i, ++pTBB)
	{
		if((pTBB->nStyle & (TBBS_INVISIBLE|TBBS_SEPARATOR))==0)
		{
			if(pTBB->nID==nId)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

// EnsureMDIControls
//  Ensures that the state of MDI controls on the bar maps to the current window state
void CCustomBar::EnsureMDIControls(void)
{
	if(IsMDIHost())
	{
		if(m_nMDITrack==cMDINotDrawn)
		{
			RecalcLayout(GetOrient()==orVert ? HTRIGHT : HTBOTTOM);
			Invalidate();
		}
	}
	else
	{
		if(m_nMDITrack!=cMDINotDrawn)
		{
			RecalcLayout();
			Invalidate();
		}
	}
}

// finds the next visible item after the one specified
int CCustomBar::FindNextVisibleItem(int nIndex, BOOL bForward)
{
	int nNextVisible=-1;
	int iItem=nIndex;
	int nOffset=1;

	// determine direction
	if(!bForward)
	{
		nOffset=-1;
	}

	iItem+=nOffset;

	// iterate over bar
	while(	iItem>=0 &&
			iItem<m_nCount &&		// iterate all items
			nNextVisible==-1)		// until we find the answer
	{
		AFX_TBBUTTON *pTBB=_GetButtonPtr(iItem);

		if(	(pTBB->nStyle & TBBS_INVISIBLE)==0 &&				// visible item
			(pTBB->nStyle & TBBS_SEPARATOR)==0 )				// that's not a separator
		{
			nNextVisible=iItem;
		}
		else
		{
			iItem+=nOffset;
		}
	}

	// check if we overran
	if(	iItem>=m_nCount ||
		iItem<0)
	{
		iItem=-1;
	}

	return iItem;
}

// TRUE if the bar has glyphs, or is non-menu
BOOL CCustomBar::HasGlyphs(void) const 
{
	if((m_dwStyle & CTBRS_MAINMENU)==0)
	{
		// non-menu bars always have glyphs, effectively. Such is life.
		return TRUE;
	}
	else
	{
		if(m_glyphStatus==cGlyphsUnknown)
		{
			// calculate if required
			AFX_TBBUTTON *pTBB=_GetButtonPtr(0);

			m_glyphStatus=cGlyphsAbsent;

			for(int iButton=0; iButton< m_nCount; ++iButton, ++pTBB)
			{
				// separators and invisible buttons have no influence
				if(pTBB->nStyle & (TBBS_SEPARATOR | TBBS_INVISIBLE))
				{
					continue;
				}
				if((pTBB->nStyle & TBBS_GLYPH)!=0 ||		// has a glyph
					IsVisibleControl(iButton))				// or is a control which is visible (controls can rely on having as much space as a normal bar)
				{
					m_glyphStatus=cGlyphsPresent;
					break;
				}
			}
		}	

		ASSERT(m_glyphStatus!=cGlyphsUnknown);

		return (m_glyphStatus==cGlyphsPresent);
	}
}

// get the size of buttons in the bar
CSize CCustomBar::GetButtonSize(ORIENTATION or) const 
{
	if(HasGlyphs())
	{
		return m_sizeButton;
	}
	else
	{
		CSize sizeDiff=m_sizeButton-m_sizeImage;
		if(or==orVert)
		{
			return CSize(globalData.GetMenuVerticalHeight()+sizeDiff.cx, m_sizeButton.cy);
		}
		else
		{
			return CSize(m_sizeButton.cx, globalData.GetMenuTextHeight()+sizeDiff.cy);
		}
	}
}

// update the text of a button
void CCustomBar::UpdateText(int nIndex, LPCTSTR newText)
{
	AFX_TBBUTTON *pTBB=_GetButtonPtr(nIndex);
	if(pTBB->nStyle & TBBS_TEXTUAL)
	{
		// If the text has a tab in it, we're updating the control too.
		CString strNewText=newText;
		int tabIndex;
		tabIndex=strNewText.Find('\t');
		if(tabIndex!=-1)
		{
			strNewText=strNewText.Left(tabIndex);
		}
			
		if(strNewText!=GetButtonExtra(nIndex)->GetLabel())
		{
			AddText(nIndex, strNewText, FALSE);
			if(	GetSafeHwnd() &&
				::IsWindowVisible(GetSafeHwnd()))
			{
				RecalcLayout();	
			}
		}
	}
}

