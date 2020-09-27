///////////////////////////////////////////////////////////////////////////////
//  BARDOCKX.CPP
//      Contains implementations CASBar, the dockable toolbar class.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "shell.h"
#include "bardockx.h"
#include "bardlgs.h"
#include "util.h"
#include "shlmenu.h"
#include "barglob.h"
#include "shell.h"
#include "menuprxy.h"
#include "prxycust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//      CASBar
//              Derived from CCustomBar, provides docking and command functionality
//              to sushi toolbars.
//

#ifdef _DEBUG
extern BOOL bChicagoLookOnNT;
inline BOOL OnChicago() { return theApp.m_bWin4 || bChicagoLookOnNT; }
#else
inline BOOL OnChicago() { return theApp.m_bWin4; }
#endif

UINT DSM_BARSIZECHANGING = RegisterMessage("DevStudioToolbarSizeChanging");

IMPLEMENT_DYNAMIC(CASBar, CCustomBar)

// static data members.

CObArray NEAR CASBar::s_aUsage;
CToolCustomizer* CASBar::s_pCustomizer = NULL;
CASBar *CASBar::s_pMenuBar=NULL;
CWnd *CASBar::s_pOldFocus=NULL;

//      CASBar::CASBar
//      Construction.

CASBar::CASBar()           
{
	m_bDirty = TRUE;        // Assume we want to write to reg DB.
	m_iKeySelect=-1;
	m_bDropPending=FALSE;

	s_aUsage.Add(this);
}

//      CASBar::~CASBar
//              Destructor, removes the toolbar from the usage list.

CASBar::~CASBar()
{
	int nCount = s_aUsage.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		if (s_aUsage[i] == this)
		{
			s_aUsage.RemoveAt(i);
			break;
		}
	}

	ASSERT(i < nCount);
}

//      CASBar::Create
//              Create function used for a new custom toolbar with one, or no command
//              IDs.  (use -1 for nID for empty toolbar).

BOOL CASBar::Create(CDockManager* pManager, UINT nID, LPCRECT lpRect /*=NULL*/,
	LPCSTR lpszTitle /*=NULL*/)
{
    static DOCKINIT BASED_CODE diInit = {
	0,
	PACKAGE_SUSHI,
		PACKET_NIL,
	"",
	INIT_VISIBLE | INIT_CANHORZ | INIT_CANVERT,
	dtToolbar,
	dpNil,
	dpTop,
	{0,0,0,0}};
	
	TOOLBARINIT tbi;
	DOCKINIT di = diInit;
	
	tbi.nIDCount = 1;
	di.nIDWnd = pManager->GetCustomID();
	tbi.nIDWnd = MAKEDOCKID(di.nIDPackage,di.nIDWnd);

	if (lpszTitle != NULL)
	{
		lstrcpy(di.szCaption, lpszTitle);
	}
	else
	{
		CString strFormat;
		VERIFY( strFormat.LoadString( IDS_DEFTBTITLE ) );

		wsprintf(di.szCaption, strFormat, LOWORD(tbi.nIDWnd) - IDTB_CUSTOM_BASE);
	}
	CSize sizeButton=GetButtonSize(orNil);
	
	CRect rect;
	if (lpRect != NULL)
	{
		rect = *lpRect;

		// Check if we should create this in a dock.
		CPoint ptTest((rect.left + rect.right) / 2,
			(rect.top + rect.bottom) / 2);
		CWnd* pWnd = WindowFromPoint(ptTest);
		
		if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CDockWnd)))
		{
			di.dpInit = ((CDockWnd*) pWnd)->m_dp;
			di.fInit &= ~INIT_VISIBLE;
			di.fInit |= INIT_POSITION;
		}
	}       
	else
	{
		CRect rectDummy;
		pManager->GetDeskRects(rect, rectDummy);
		rect.right = rect.left + sizeButton.cx;
		rect.bottom = rect.top + sizeButton.cy;
		rect.OffsetRect(20, 20);        // Semi-random choice.
	}
	
	di.rectFloat.left = rect.left - m_cxLeftBorder - g_mt.cxBorder;
	di.rectFloat.top = rect.top - m_cyTopBorder - g_mt.cyBorder;
	di.rectFloat.right = rect.right + m_cxRightBorder + g_mt.cxBorder;
	di.rectFloat.bottom = rect.bottom + m_cyBottomBorder + g_mt.cyBorder;

	BOOL bReturn = Create(NULL, pManager, WS_BORDER, &tbi, &nID,
		NULL, TRUE, &di);

	// Move it into the dock if it is not a floater.
	if (bReturn && di.dpInit != dpNil)
	{
		// Custom toolbars always need to be written to the reg DB.
		m_bDirty = TRUE;
	
		DkWMoveWindow(tbi.nIDWnd, di.dpInit,
			&di.rectFloat);
	}
		
	return bReturn;
}

//      CASBar::Create
//              This is where the window is really created.  Above creates call this
//              one.

BOOL CASBar::Create(CWnd* pParent, CDockManager* pManager, DWORD dwStyle,
	TOOLBARINIT FAR* lptbi, UINT FAR* lpIDArray, HGLOBAL hglobInit,
	BOOL bRegister /*=FALSE*/, DOCKINIT FAR* lpdi /*=NULL*/)
{
    // create the HWND
    CRect rect;
    rect.SetRectEmpty();    // Size reset in SetButtons.
    
    if (pParent == NULL)
	{
		ASSERT_VALID(pManager);
		pParent = pManager->GetDock(dpHide);
	}

	dwStyle |= (CBRS_TOOLTIPS | CBRS_FLYBY | DWS_SIZABLE);

	if( lptbi &&
		lptbi->nIDWnd==MAKEDOCKID(PACKAGE_SUSHI, IDTB_MENUBAR)) // nasty hack.
	{
		dwStyle|=DWS_MENUBAR;
	}

	// the TOOLBARINIT structure should now always be filled with a MAKEDOCKID result
	// in its nIDWnd field. This assertion will fail if the field didn't include
	// the required package id.
	ASSERT(HIWORD(lptbi->nIDWnd)!=0);

	m_nIDWnd=lptbi->nIDWnd;

    if (!CCustomBar::Create(dwStyle, rect, pParent, LOWORD(lptbi->nIDWnd)))
	{
		delete this;
		return FALSE;
	}

	// if this is the main menu bar, set up the correct toolbar style
	if( lptbi &&
		lptbi->nIDWnd==MAKEDOCKID(PACKAGE_SUSHI, IDTB_MENUBAR)) // nasty hack.
	{
		m_dwStyle |= CTBRS_MAINMENU;
		ASSERT(s_pMenuBar==NULL);
		s_pMenuBar=this;
	}

	SetOwner(pManager->m_pFrame);   // MFC uses owner for command and notify routing.

    // Set buttons before docking for size.
    if (hglobInit != NULL)
    {
		if(!GetProxy()->SetData(hglobInit))
		{
			DestroyWindow();
			return FALSE;
		}
		lptbi->nIDCount = m_nCount;
		lpIDArray = NULL;
    }
    
    if (!DynSetButtons(lpIDArray, lptbi->nIDCount))
    {
		DestroyWindow();
		return FALSE;
    }
    
    if (bRegister)
    {
		lpdi->fInit |= INIT_CANHORZ | INIT_CANVERT;
	
		// Make sure this is a valid rectangle.
		LRESULT lSize = OnGetMoveSize(lpdi->dpInit,
			MAKELPARAM(lpdi->rectFloat.right - lpdi->rectFloat.left,
			lpdi->rectFloat.bottom - lpdi->rectFloat.top));
		lpdi->rectFloat.right = lpdi->rectFloat.left + LOWORD(lSize);
		lpdi->rectFloat.bottom = lpdi->rectFloat.top + HIWORD(lSize);
	
	CDockWorker* pDocker;
		ASSERT_VALID(pManager);
	    if ((pDocker = pManager->RegisterWnd(this, lpdi)) == NULL)
	    {
		DestroyWindow();
		return FALSE;
		}
		
		if (s_pCustomizer != NULL && s_pCustomizer->m_pDialog != NULL &&
			s_pCustomizer->m_pDialog->IsKindOf(RUNTIME_CLASS(CToolbarDialog)))
		{
			((CToolbarDialog*) s_pCustomizer->m_pDialog)->AddToolbar(pDocker);
		}
	}

    return TRUE;
}

//      CASBar::DynSetButtons
//              Completely resets the button information (data, and imagewell) for
//              the bar.  May be passed an ID array, or use existing data in m_pData,
//              usually loaded from disk.

BOOL CASBar::DynSetButtons(const UINT FAR* lpIDArray, int nIDCount)
{
	int nTiles = 0;
	AFX_TBBUTTON* pTBBNew = NULL;
	TBBUTTON_EXTRA* pExtraNew = NULL;
	HBITMAP hbmNewWell = NULL;
	BOOL bExtraUsed=FALSE; // TRUE if the extra data has been written to.
	GLYPHSTATUS glyphs=cGlyphsUnknown; // as we iterate, remember the glyphs

	ASSERT(nIDCount >= 0);
	if (nIDCount > 0)
	{
		ASSERT(lpIDArray != NULL || m_pData != NULL);

		// If lpIDArray is NULL, then the data comes from the reg DB,
		// otherwise we assume the toolbar is clean.

		if (lpIDArray != NULL)
			m_bDirty = FALSE;
		
		HDC hdcClient = ::GetDC(m_hWnd);
		HDC hdcDest = ::CreateCompatibleDC(hdcClient);
		HDC hdcSrc = ::CreateCompatibleDC(hdcClient);
		
		hbmNewWell = ::CreateCompatibleBitmap(hdcClient,
			nIDCount * m_sizeImage.cx, m_sizeImage.cy);
		::ReleaseDC(m_hWnd, hdcClient);
	
		if (hbmNewWell == NULL || hdcDest == NULL || hdcSrc == NULL)
		{       
			// May RIP.
			::DeleteObject(hbmNewWell);
			::DeleteDC(hdcSrc);
			::DeleteDC(hdcDest);
			return FALSE;
		}
			
		HBITMAP hbmOld = (HBITMAP) ::SelectObject(hdcDest, hbmNewWell);

		CPackage* pPackage;
		
		AFX_TBBUTTON* pTBBNext = pTBBNew =
			(AFX_TBBUTTON*) calloc(nIDCount, sizeof(AFX_TBBUTTON));
		pExtraNew = new TBBUTTON_EXTRA[nIDCount];
		
		int iDest; // index into the destination extra array

		for (int i = 0; i < nIDCount; i++)
		{
			iDest=pTBBNext-pTBBNew;

			if (lpIDArray == NULL)
				*pTBBNext = ((AFX_TBBUTTON*) m_pData)[i];
			else
				pTBBNext->nID = lpIDArray[i];

			if(pTBBNext->nID==-1)
			{
				continue;
			}

			/* despite the fact that 4.x wrote the nStyle information
			out to the registry, the 4.x version of this code just ignored it
			and used the package's version. We use an amalgam of these two 
			strategies. If the button is of a type which cannot be user
			configured, or if the toolbar is not being loaded, then we just use
			the package value. Otherwise, we take notice of the user's optional
			flags. martynl 27Mar96
			*/
			UINT nStyle = theApp.GetCommandBtnInfo(pTBBNext->nID, &pPackage);

			// separator buttons use the package style
			if( (nStyle & (TBBS_SEPARATOR))!=0 ||
				lpIDArray!=NULL)
			{
				// if we're not loading from the registry, or if the button can't be 
				// styled, then use the package versions
				pTBBNext->nStyle = nStyle;
			}
			else
			{
				// if we're loading from the registry, combine the 
				// loaded style and the package style
				UINT nMaskedStyle=pTBBNext->nStyle & (  TBBS_TEXTUAL | TBBS_GLYPH | TBBS_CUSTOMBITMAP | 
														TBBS_CUSTOMTEXT | TBBS_MENU | TBBS_SEPARATOR);

				// allow the registry to control glyph and textual, since it knows what it saved
				UINT nMaskedPackageStyle=nStyle & ~(TBBS_GLYPH|TBBS_TEXTUAL|TBBS_CUSTOMBITMAP|TBBS_CUSTOMTEXT);

				pTBBNext->nStyle = nMaskedPackageStyle | nMaskedStyle;
			}

			// if it's not a separator, and we can't find a package for this command, then it must be invisible
			if((pTBBNext->nStyle & TBBS_SEPARATOR)==0 &&
				pPackage==NULL)
			{
				ASSERT(pTBBNext->nStyle & TBBS_INVISIBLE);
			}

			// window loading means we must create the window
			if (pTBBNext->nStyle & TBBS_HWND)
			{
				HWND hwnd =     pPackage->GetDockableWindow(pTBBNext->nID, NULL);
	
				CWnd* pWnd;
				if (hwnd == NULL || (pWnd = CWnd::FromHandle(hwnd)) == NULL)
					continue;
					
				pWnd->SetParent(this);
				if (lpIDArray == NULL)
				{
					CRect rect;
					pWnd->GetWindowRect(rect);
					::GetDroppedRect(pWnd->m_hWnd, rect);
					
					pWnd->SetWindowPos(NULL, 0, 0,
						pTBBNext->iImage, rect.Height(),
						SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
				}

				if (IsValidCtrlSize(pWnd))
					pWnd->ShowWindow(SW_SHOWNOACTIVATE);
				else
					pWnd->ShowWindow(SW_HIDE);

				pExtraNew[iDest].SetControl(hwnd);
				bExtraUsed=TRUE;
			}

			if (pTBBNext->nStyle & TBBS_SEPARATOR)
			{
				// ditch initial or double separators 
				if (pTBBNext == pTBBNew ||
					((pTBBNext - 1)->nStyle & TBBS_SEPARATOR) != 0)
					continue;

				pTBBNext->iImage = m_cxDefaultGap + 2;
			}
			
			if(pTBBNext->nStyle & TBBS_GLYPH)
			{
				// because we just glyphed a button, we know we are a glyphed bar
				if((pTBBNext->nStyle & TBBS_INVISIBLE)==0)
				{
					glyphs=cGlyphsPresent;
				}

				if (pTBBNext->nStyle & TBBS_CUSTOMBITMAP)
				{
					// we don't want to get the bitmap from the package, but from the
					// saved DIB, which will already be loaded and converted into a DDB In
					// the custom glyph object. At this point we transfer to the well 
					// if appropriate

					// setup internal data
					pTBBNext->iImage = nTiles++;

					// get button bitmap
					HBITMAP hbmCustom=GetButtonExtra(i)->GetGlyph();

					// will always be a bitmap at the moment
					ASSERT(hbmCustom!=NULL);

					BITMAP bmp;
					VERIFY(::GetObject(hbmCustom, sizeof(BITMAP), &bmp));

					if(     bmp.bmHeight==m_sizeImage.cy &&
						bmp.bmWidth==m_sizeImage.cx)
					{
						// transfer to well, lose source
						HBITMAP hbmSrc = (HBITMAP) ::SelectObject(hdcSrc, hbmCustom);

						VERIFY(BitBlt(hdcDest, m_sizeImage.cx * pTBBNext->iImage, 0, m_sizeImage.cx, m_sizeImage.cy, hdcSrc, 0, 0, SRCCOPY));
						
						::SelectObject(hdcSrc, hbmSrc);

						// no longer need it
						::DeleteObject(hbmCustom);
						pExtraNew[iDest].SetGlyph(NULL);
					}
					else
					{
						// scale to well, keep source
						GLOBAL_DATA::ScaleBitmap(hdcDest, m_sizeImage, CPoint(m_sizeImage.cx*pTBBNext->iImage, 0), hbmCustom, CSize(bmp.bmWidth, bmp.bmHeight));
						pExtraNew[iDest].SetGlyph(hbmCustom);
						bExtraUsed=TRUE;
					}
				}
				else
				{
					// if it's invisible, then it does need a space in the well
					if (pTBBNext->nStyle & TBBS_INVISIBLE)
					{
						pTBBNext->iImage = nTiles++;
					}
					else
					{
						if (pTBBNext->nID == TBB_NIL || pPackage == NULL)
						{
							continue; // Because of the extra data, this isn't going to work as we hope/expect. Fix. !!!
						}

						HBITMAP hbmWell;
						int iGlyph;
						VERIFY(theApp.GetCommandBitmap(pTBBNext->nID, &hbmWell, &iGlyph, theApp.m_bLargeToolBars));
						
						if(!BltCommandBitmap(hbmWell, iGlyph, hdcDest, nTiles, hdcSrc))
						{
							continue;
						}
						else
						{
							pTBBNext->iImage = nTiles++;
						}
					}
				}
			}

			
			if (pTBBNext->nStyle & TBBS_TEXTUAL)
			{
				if(lpIDArray!=NULL || (pTBBNext->nStyle & TBBS_CUSTOMTEXT)==0)
				{
					// creating from scratch, so we need to get the label to go with this command
					// get the title from the command table
					LPCSTR pName;
					theCmdCache.GetCommandString(pTBBNext->nID, STRING_MENUTEXT, &pName);

					CString name(pName);
					pExtraNew[iDest].SetLabel(name);
					bExtraUsed=TRUE;
				}
				else
				{
					// labels are already in the extra array
					ASSERT(!GetButtonExtra(i)->GetLabel().IsEmpty());
					pExtraNew[iDest].SetLabel(EditButtonExtra(i)->GetLabel());
					bExtraUsed=TRUE;
				}
			}
			
			pTBBNext++;
		}       // for
		
		::SelectObject(hdcDest, hbmOld);
		::DeleteDC(hdcSrc);
		::DeleteDC(hdcDest);
			
		if (pTBBNext != pTBBNew && ((pTBBNext - 1)->nStyle  & TBBS_SEPARATOR)!=0)
			pTBBNext--;
		
		nIDCount = (int) (pTBBNext - pTBBNew);
	}       // if nIDCount > 0

	// if we're not loading from disk, then we're resetting (otherwise, windows won't have existed
	// correctly in the old bar)
	if(lpIDArray != NULL)
	{
		// windows will have been duplicated, so lose them
		RemoveWindows();
	}
	Empty();

	if (nIDCount == 0)
	{
		if (hbmNewWell != NULL)
			::DeleteObject(hbmNewWell);
		if (pTBBNew != NULL)
			free(pTBBNew);
		if (pExtraNew != NULL)
			delete [] pExtraNew;
	}
	else
	{
		m_pData = pTBBNew;
		if(bExtraUsed)
		{
			m_pButtonExtra = pExtraNew;
		}
		else
		{
			m_pButtonExtra=NULL;
			delete [] pExtraNew;
		}
		m_nCount = nIDCount;
		m_hbmImageWell = hbmNewWell;
		m_nTiles = nTiles;

		// transfer calculated glyph status to bar
		SetGlyphStatus(glyphs);

		ASSERT_VALID(this);

		if (!IsInDialog())
			OnUpdateCmdUI((CFrameWnd*) AfxGetApp()->m_pMainWnd, TRUE);
	}
	
	RecalcSizes();
	
	return TRUE;
}

CToolCustomizer* CASBar::GetCustomizer()
{
	return s_pCustomizer;
}

ORIENTATION CASBar::GetOrient() const
{
	DOCKPOS dp = DkWGetDock(m_nIDWnd);

	CRect rectWindow;
	GetWindowRect(rectWindow);

	switch(dp)
	{
	default:
	case dpEmbedded:
		return orNil;

	case dpTop:
	case dpBottom:
		// empty bars always have their size driven by their dock. Also, one button bars
		// whose size is the same as the width of a single button have the same fudge.
		// This implies a slight unpleasantness - if you empty a converse
		// docked bar in place, when you add to it will have lost its
		// converse-ness, and you will need to use shift+drag again to
		// regain it. Tough luck.
		if( m_nCount==0 ||
			m_nCount==1 && rectWindow.Width() == GetVWidth())
		{
			return orHorz;
		}
	case dpLeft:
	case dpRight:
		if( m_nCount==0 ||
			m_nCount==1 && rectWindow.Width() == GetVWidth())
		{
			return orVert;
		}
		else
		{
			// since SHIFT+DRAG will dock reversed, we need to check the actual size to work out
			if (rectWindow.Width() == GetVWidth())
			{
				return orVert;
			}
			else
			{
				return orHorz;
			}
		}
	}
}

CCustomBar* CASBar::NewBar(UINT nID, const CPoint& pt)
{
	CASBar* pBar = new CASBar;
	CSize sizeButton=CCustomBar::GetDefaultButtonSize(theApp.m_bLargeToolBars);

	CRect rectInit( pt.x-sizeButton.cx/2,
					pt.y-sizeButton.cy/2,
					pt.x+sizeButton.cx/2,
					pt.y+sizeButton.cy/2);
	if (!pBar->Create(((CMainFrame*) AfxGetMainWnd())->m_pManager,
		nID, rectInit))
	{
		// auto delete inside this creation function
		return NULL;
	}
	
	return pBar;
}

void CASBar::MoveBar(const CRect& rect)
{
	DkWMoveWindow(m_nIDWnd, dpCurrent, rect);

	if (GetOrient() == orNil)
	{
		InvalidateRect(NULL);
		UpdateWindow();
	}
}

void CASBar::OnBarActivate()
{
	CPaletteWnd* pPalette = (CPaletteWnd*) GetParent();
	if (pPalette->IsKindOf(RUNTIME_CLASS(CPaletteWnd)))
		pPalette->ActivateNoFocus();
}

HWND CASBar::GetControl(UINT nID)
{
	CPackage* pPackage;

	VERIFY(theApp.GetCommandBtnInfo(nID, &pPackage) & TBBS_HWND);
	if (pPackage == NULL)
	{
		TRACE("CASBar::GetControl(%d); could not get package!\n", nID);
		return NULL;
	}

	return pPackage->GetDockableWindow(nID, NULL);
}

void CASBar::OnButtonSize()
{
	m_bDirty = TRUE;
}

void CASBar::LoseFocus()
{
	AfxGetApp()->m_pMainWnd->SetFocus();
}

void CASBar::DeleteButton(int nIndex, BOOL bMoving /*=FALSE*/)
{
	m_bDirty = TRUE;

	CCustomBar::DeleteButton(nIndex, bMoving);
}

int CASBar::InsertButton(int nIndex, TRANSFER_TBBUTTON& trans,
	BOOL bReplace, GAP gap)
{
	m_bDirty = TRUE;

	return CCustomBar::InsertButton(nIndex, trans, bReplace, gap);
}

BEGIN_MESSAGE_MAP(CASBar, CCustomBar)
	//{{AFX_MSG_MAP(CASBar)
	ON_WM_KILLFOCUS()
	ON_WM_NCPAINT()
    ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SYSCOMMAND()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(DWM_GETDATA, OnGetData)
    ON_REGISTERED_MESSAGE(DWM_GETMOVESIZE, OnGetMoveSize)
    ON_REGISTERED_MESSAGE(DWM_GETTRACKSIZE, OnGetTrackSize)
    ON_REGISTERED_MESSAGE(DMM_NCLBUTTONDOWN, OnDkNcLButtonDown)
    ON_REGISTERED_MESSAGE(DMM_NCLBUTTONDBLCLK, OnDkNcLButtonDown)
    ON_REGISTERED_MESSAGE(DSM_DROPMENU, OnDropMenu)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
//      CASBar message handlers

//      CASBar::PostNcDestroy
//              Dock manager does not delete windows, so we auto-delete.

void CASBar::PostNcDestroy()
{
	if(s_pMenuBar==this)
	{
		s_pMenuBar=NULL;
	}
	delete this;
}

//      CASBar::WindowProc
//              All dockable windows call DkPreHandleMessage.

LRESULT CASBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Starting from Orion #6081, we don't steal the rbuttondown message here
	// this allows us to control it more precisely below, and choose whether we
	// want to show our base class' toolbar customisation popup. martynl 22Mar96
	if (message != DMM_NCLBUTTONDOWN && message != DMM_NCLBUTTONDBLCLK &&
		message != WM_RBUTTONDOWN &&
		message != WM_RBUTTONUP &&
		message != WM_LBUTTONDBLCLK)
	{
		LONG lResult;
	
		if (DkPreHandleMessage(GetSafeHwnd(),
			message, wParam, lParam, &lResult))
			return lResult;
	}

	return CCustomBar::WindowProc(message, wParam, lParam);
}

void CASBar::OnNcPaint()
{
	// Since an ASBar can now have a scroll bar (when there are a large number of buttons
	// present and the bar is contained within the customize toolbars dialogue), we can't
	// omit nc painting when we need the scroll bars. martynl 07Mar96
	if (OnChicago() && 
		(GetStyle() & (WS_VSCROLL | WS_HSCROLL))==0) {
		return;         // Dockwnd paints the border.
	}

	CCustomBar::OnNcPaint();
}

//      CASBar::OnNcHitTest
//      All non-button area is HTCAPTION for dock moving.

UINT CASBar::OnNcHitTest(CPoint pt)
{
	DOCKPOS dp = DkWGetDock(m_nIDWnd);

	CWnd* pParent = GetParent();
	
    ScreenToClient(&pt);
    if (!IsInDialog() && HitTest(pt, FALSE) < 0 && (!IsMDIHost() || HitTestMDI(pt)==cMDINoButton))
	{
		if(dp==dpEmbedded)
		{
			// embedded bars don't drag by their empty areas.
			return HTCLIENT;
		}
		else
		{
			return HTCAPTION;     // nothing hit
		}
	}
    else
    {
		ClientToScreen(&pt);
		return CCustomBar::OnNcHitTest(pt);
    }
}

//      CASBar::OnLButtonDown
//              Handles the customizing calls for button moving, and control sizing.

void CASBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bTemp = (s_pCustomizer == NULL && (::GetKeyState(VK_MENU) & ~1) != 0);

	int nIndex = HitTest(point);
	UINT nStyle=0;

	// cache style of button to be dragged
	if (nIndex != -1 && nIndex < m_nCount)
	{
		nStyle = GetButtonStyle(nIndex);
	}

	if (bTemp)
	{
		s_pCustomizer = new CToolCustomizer(
			((CMainFrame*) AfxGetMainWnd())->m_pManager,
			NULL, s_aUsage, TRUE, FALSE);
	}

	CCustomBar::OnLButtonDown(nFlags, point);

	if (bTemp)
	{
		delete s_pCustomizer;
		s_pCustomizer = NULL;

		// If we dragged a menu, we need to rebuild to sort out the menu bar
		if(nStyle & TBBS_MENU)
		{
			CMainFrame *pFrame=(CMainFrame *)theApp.m_pMainWnd;
			pFrame->RebuildMenus();
		}
	}
}

//      CASBar::OnRButtonDown
//              Chooses between a base class popup (for customisation) and a popup from the dock manager
void CASBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	LONG ignored;   // we can safely ignore the lresult return, because there's no significance
					// to button down return values

	const MSG *pMsg=GetCurrentMessage();
	if(!DkPreHandleMessage(GetSafeHwnd(), WM_RBUTTONDOWN, pMsg->wParam, pMsg->lParam, &ignored))
	{
		CCustomBar::OnRButtonDown(nFlags, point);
	}
}

//      CASBar::OnRButtonUp
//              Chooses between a base class popup (for customisation) and a popup from the dock manager
void CASBar::OnRButtonUp(UINT nFlags, CPoint point)
{
	CToolCustomizer *pCustomizer=GetCustomizer();

	if(pCustomizer)
	{
		int nIndex = HitTest(point);

		if (nIndex != -1)
		{
			if (this != pCustomizer->m_pSelectBar->GetRealObject()
				|| nIndex != pCustomizer->m_nSelectIndex)
			{
				pCustomizer->SetSelection(GetProxy(), nIndex);
			}
		}

		CPoint scrPlace=point;
		ClientToScreen(&scrPlace);

		if(pCustomizer->DisplayButtonMenu(nFlags, scrPlace))
		{
			// customizer has done it already
			return;
		}
	}

	LONG ignored;   // we can safely ignore the lresult return, because there's no significance
					// to button down return values

	const MSG *pMsg=GetCurrentMessage();
	if(!DkPreHandleMessage(GetSafeHwnd(), WM_RBUTTONUP, pMsg->wParam, pMsg->lParam, &ignored))
	{
		CCustomBar::OnRButtonUp(nFlags, point);
	}
}

//      CASBar::OnLButtonDblClk
//              On a button, translate to LButtonDown.

void CASBar::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	if(IsMDIHost() && HitTestMDI(pt)==cMDIIcon)
	{
		// double click to close window

		ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

		CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

		BOOL bMaximized;
		CWnd *pActive=pMainWnd->MDIGetActive(&bMaximized);

		pActive->SendMessage(WM_SYSCOMMAND, SC_CLOSE, MAKELONG(pt.x, pt.y));
	}
	else
	{
	    SendMessage(WM_LBUTTONDOWN, nFlags, MAKELPARAM(pt.x, pt.y));
	}
}

int CASBar::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest,
	UINT message)
{
	// Because of a what appears to be a SetWindowPos bug
	// when we try to ActivateNoFocus() in CPaletteWnd, we  need to hide
	// the tool tip here.

	//DestroyToolTip(TRUE, TRUE);
	CancelToolTips();

	// test if active window is a browser popup window
	CWnd * pWndActiveView = GetActiveWindow();
	if (pWndActiveView && pWndActiveView->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd)))
	{
		return MA_NOACTIVATE;
	}

	CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
	if (pDocker != NULL && pDocker->GetDock() == dpNil)
	{
		ASSERT(pDocker->m_pWnd != NULL);

		if (theApp.m_bDeactByOtherApp)
			pDocker->m_pWnd->SetFocus();

		return MA_NOACTIVATE;
	}
		
	// Note: CCustomBar::OnMouseActivate seems to be changing the focus
	// for us away from the toolbar customize dialog when clicking in the
	// embedded toolbar...skipping it and going over its head to CWnd
	//return CCustomBar::OnMouseActivate(pDesktopWnd, nHitTest, message);
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

// causes the bar's buttons to become large or small
BOOL CASBar::SetButtonSize(BOOL bLarge)
{
	// remember current orientation for later
	ORIENTATION or=GetOrient();

	UINT nConstraint=GetExpansionConstraint(CRect(0,0,0,0), &or, TRUE); 

	// remember current button size for later scaling
	CSize oldSizeButton=m_sizeButton;
	CSize oldSizeImage=m_sizeImage;
	// to be conservative, assume the button has borders on all sides;
	oldSizeButton.cx+=m_cxSharedBorder;
	oldSizeButton.cy+=m_cySharedBorder;
	
	// set size values
	SetDefaultSizes(bLarge);

	// This process is complicated by the presence of custom bitmaps. We only store
	// custom bitmaps separately if they are of the wrong size for the current bar
	// Otherwise, we store the current custom bitmap inside the image well. To ensure
	// custom bitmaps are preserved here, we need to perform a three-step process:

	// 1. Copy any custom bitmaps which were not derived from another size into the
	// extra data.
	// 2. Refresh bitmaps. This will copy across or scale all custom bitmaps
	// 3. Delete any custom bitmaps which are the same size as the current bar
	// martynl 07May96

	// build a new image well for the new size
	HBITMAP hbmNewWell = NULL;
	
	HDC hdcClient = ::GetDC(m_hWnd);

	if(hdcClient==NULL) {
		return FALSE;
	}
	
	hbmNewWell = ::CreateCompatibleBitmap(hdcClient,
		m_nCount * m_sizeImage.cx, m_sizeImage.cy);
	::ReleaseDC(m_hWnd, hdcClient);

	if (hbmNewWell == NULL) {       
		return FALSE;
	}

	// Duplicate all custom bitmaps stored within the well into the extra data
	AFX_TBBUTTON *pTBB=(AFX_TBBUTTON *)m_pData;
	for(int iButton=0; iButton<m_nCount; ++iButton, ++pTBB)
	{
		if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
		{
			// we need to copy the bitmap out either if there isn't a custom bitmap already
			if(     !HasButtonExtra() ||
				GetButtonExtra(iButton)->GetGlyph()==NULL)
			{
				EditButtonExtra(iButton)->SetGlyph(GLOBAL_DATA::DuplicateBitmap(this,
								m_hbmImageWell, oldSizeImage, CPoint(oldSizeImage.cx * pTBB->iImage, 0)));
			}
		}
	}

	// delete the old well
	if(m_hbmImageWell)
	{
		::DeleteObject(m_hbmImageWell);
	}
	
	m_hbmImageWell = hbmNewWell;

	// get bitmaps from packages into the new well, also copy across custom bitmaps
	RefreshButtonBitmaps();

	// lose custom bitmaps that are same size as well
	pTBB=(AFX_TBBUTTON *)m_pData;
	for(iButton=0; iButton<m_nCount; ++iButton, ++pTBB)
	{
		if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
		{
			if(     HasButtonExtra() &&
				GetButtonExtra(iButton)->GetGlyph()!=NULL)
			{
				BITMAP bmp;
				VERIFY(::GetObject(GetButtonExtra(iButton)->GetGlyph(), sizeof(BITMAP), &bmp));

				if(     bmp.bmHeight==m_sizeImage.cy &&
					bmp.bmWidth==m_sizeImage.cx)
				{
					::DeleteObject(GetButtonExtra(iButton)->GetGlyph());
					EditButtonExtra(iButton)->SetGlyph(NULL);
				}
			}
		}

		// flush the label sizes cache coz the font size just changed
		if((pTBB->nStyle & TBBS_TEXTUAL)!=0)
		{
			EditButtonExtra(iButton)->SetLabelSize(CSize(0,0));
		}
	}

	RecalcLayout(nConstraint, &or);

	// If it's embedded, we need to size it ourself, since the parent will normally control the embedded bar size
	CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();
	CDockWorker *pDocker=pFrame->m_pManager->WorkerFromWindow(this);

	if(pDocker &&
		pDocker->GetDock()==dpEmbedded)
	{
		// size the bar
		CSize sizeNew = GetHSize(or);

		SetWindowPos(NULL, 0,0, sizeNew.cx, sizeNew.cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		
		// now tell the parent to sort itself out
		CWnd *pWndParent=GetParent();
		if(pWndParent &&
			!pWndParent->IsIconic())
		{
			// size the parent to its current size
			CRect rectCurrent;
			pWndParent->GetWindowRect(rectCurrent);
			CSize sizeCurrent=rectCurrent.Size();
			UINT nSize = pWndParent->IsZoomed() ? SIZE_MAXIMIZED : SIZE_RESTORED;

			pWndParent->SendMessage(WM_SIZE, nSize, MAKELPARAM(sizeCurrent.cx, sizeCurrent.cy));
		}
	}

	return TRUE; // success
}

void CASBar::OnSysColorChange()
{
	HBITMAP hbmSave = m_hbmImageWell;
	m_hbmImageWell = NULL;  // So CToolbar won't destroy the image well.

	CCustomBar::OnSysColorChange();

	m_hbmImageWell = hbmSave;

	RefreshButtonBitmaps();
}

void CASBar::OnSysCommand(UINT nID, LONG lParam)
{
	if(nID == SC_KEYMENU)
		GetParent()->SendMessage(WM_SYSCOMMAND, nID, lParam);
	else
		CCustomBar::OnSysCommand(nID, lParam);
}

LRESULT CASBar::OnGetData(WPARAM, LPARAM)
{
	return (LRESULT)(LPVOID) (m_bDirty ? GetProxy()->GetData() : NULL);
}

//      CASBar::OnGetTrackSize
//              For dock worker implementation of snap sizing.

LRESULT CASBar::OnGetTrackSize(UINT wParam, LPARAM lParam)
{
	CSize size((short) LOWORD(lParam), (short) HIWORD(lParam));
	
	size = GetSize(wParam, size);
	
	return MAKELRESULT((WORD)(short) size.cx, (WORD)(short) size.cy);
}

//      CASBar::OnGetMoveSize
//              Returns the various sizes for dock positions of the toolbar.

LRESULT CASBar::OnGetMoveSize(UINT wParam, LPARAM lParam)
{
	CSize size((short) LOWORD(lParam), (short) HIWORD(lParam));
	
	DOCKPOS dp = (DOCKPOS) wParam;
	
	switch (dp)
	{
	case dpTop:
	case dpBottom:
	case dpHorzInit:
	case dpEmbedded:
		size = GetHSize(orHorz);
		break;

	case dpLeft:
	case dpRight:
	case dpVertInit:
		size = GetVSize();
		break;
		
	case dpNil:
		if(s_pMenuBar==this)
		{
			// floating menu bars shouldn't grow to multiple lines unless they have to.
			size=GetSize(HTBOTTOM, size);
		}
		else
		{
			size = GetSize(HTRIGHT, size);
		}
		break;
	}
		
	return MAKELRESULT((WORD)(short) size.cx, (WORD)(short) size.cy);
}

//      CASBar::OnDkNcLButtonDown
//              Send HTCLIENTs to the bar.  These may come from HWND controls that
//              may need to be sized.  Otherwise just forward as usual.

LRESULT CASBar::OnDkNcLButtonDown(UINT wParam, LPARAM lParam)
{
	HGLOBAL hglob = (HGLOBAL) lParam;
    MOUSESTRUCT FAR* lpmst = (MOUSESTRUCT FAR*) GlobalLock(hglob);
    MOUSESTRUCT mst = *lpmst;
    GlobalUnlock(hglob);
    
	if (mst.nHitTest == HTCLIENT &&
		(s_pCustomizer != NULL || (::GetKeyState(VK_MENU) & ~1) != 0))
	{
		ScreenToClient(&mst.pt);
		SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(mst.pt.x, mst.pt.y));
		return TRUE;
	}
	else if (mst.nHitTest == HTCAPTION && IsInDialog())
	{
		// If we are in a dialog, then block HTCAPTIONs from children
		// or they will be movable.
		//
		return TRUE;
	}

	LRESULT lResult;
	const MSG* pMsg = GetCurrentMessage();
		
	if (DkPreHandleMessage(GetSafeHwnd(),
		pMsg->message, wParam, lParam, &lResult))
		return lResult;
		
	return 0L;
}

// REVIEW: This should be in CCustomBar, I think.
LRESULT CASBar::OnDropMenu(UINT wParam, LPARAM lParam)
{
	if(s_pSystemMenu!=NULL && wParam!=m_nCount)
	{
		delete s_pSystemMenu;
		s_pSystemMenu=NULL;
	}
	// redraw anything that needs to be done
	return ExpandItem(wParam, TRUE, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// Keyboard interface
afx_msg void CASBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!KeyDown(nChar, nRepCnt, nFlags))
	{
		CCustomBar::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

afx_msg void CASBar::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!KeyDown(nChar, nRepCnt, nFlags))
	{
		CCustomBar::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
}

BOOL CASBar::KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bProcessed=FALSE;

	// hmm. One variable?
	if(s_pDroppedBar!=NULL)
	{
		m_iKeySelect=s_nDroppedItem;
	}

	// vertical bars treat keyboard differently
	ORIENTATION or=GetOrient();
	BOOL bVertBar=(or==orVert);

	BOOL bMoveSelection=FALSE;      // true if a selection move is decoded from this keystroke
	int iNewSelection=0;            // button index of the new selection
	BOOL bActivateNewSelection=FALSE; // TRUE if the keyboard indicates that the new selection
								// should be pressed

	if(bVertBar)
	{
		switch(nChar)
		{
			case VK_UP:
				// up means previous
				nChar=VK_LEFT;
				break;
			case VK_DOWN:
				// down means next
				nChar=VK_RIGHT;
				break;
			case VK_RIGHT:
			case VK_LEFT:
				// right means dropout
				nChar=VK_RETURN;
				break;

		}
	}

	if(m_iKeySelect!=-1)
	{
		// down means enter on a menu button
		if( (m_iKeySelect==m_nCount ||
			(GetButtonStyle(m_iKeySelect) & TBBS_MENU)) &&
			!bVertBar &&
			(nChar==VK_DOWN ||
			 nChar==VK_UP))
		{
			nChar=VK_RETURN;
		}
	}

	CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

	BOOL bMaximized;
	CWnd *pMDIChild=pMainWnd->MDIGetActive(&bMaximized);
	
	int maxIndex=m_nCount-1;
	if((GetBarStyle() & CTBRS_MAINMENU)!=0 &&
		pMDIChild!=NULL &&
		s_pCustomizer==NULL) // can't customize system menu.
	{
		maxIndex+=1;
	}

	// we now assume a horizontal one
	// find next selection
	switch(LOBYTE(nChar))
	{
		case VK_LEFT:
			if(m_iKeySelect!=-1)
			{
				// previous button
				bMoveSelection=TRUE;
				int iInitialSelection=iNewSelection=m_iKeySelect;
				do
				{
					iNewSelection-=1;
					if(iNewSelection<0)
					{
						iNewSelection=maxIndex;
					}
				}
				while(iNewSelection!=iInitialSelection && !CanFocus(iNewSelection));
				if(iNewSelection!=iInitialSelection)
				{
					bMoveSelection=TRUE;
				}
				bProcessed=TRUE;
			}
			break;
		
		case VK_TAB:
			if(m_iKeySelect!=-1)
			{
				if(GetKeyState(VK_CONTROL))
				{
					int nOffset=1;
					if( GetKeyState(VK_SHIFT))
					{
						// previous bar
						nOffset=-1;
					}

					// find us within the list of all toolbars
					int iBar=0;
					int nSize=s_aUsage.GetSize();

					while(iBar < nSize)
					{
						if(s_aUsage[iBar]==this)
						{
							break;
						}
						++iBar;
					}

					if(iBar>=nSize)
					{
						// not found ourselves, which is very odd
						ASSERT(FALSE);
					}

					// move to next/previous bar
					iBar+=nOffset;

					if(iBar<0)
					{
						iBar=nSize-1;
					}

					if(iBar>=nSize)
					{
						iBar=0;
					}

					CASBar *pNewBar=(CASBar *)s_aUsage[iBar];

					ASSERT(pNewBar!=NULL);

					// cause it to take focus
					LoseKeyFocus();
					pNewBar->GainKeyFocus(0);
					bProcessed=TRUE;
					break;
				}
			}
			// deliberate drop thru to right
		case VK_RIGHT:
			if(m_iKeySelect!=-1)
			{
				// next button
				int iInitialSelection=iNewSelection=m_iKeySelect;
				bMoveSelection=TRUE;
				do
				{
					iNewSelection+=1;
					if(iNewSelection>maxIndex)
					{
						iNewSelection=0;
					}
				}
				while(iInitialSelection!=iNewSelection && !CanFocus(iNewSelection));
				if(iNewSelection!=iInitialSelection)
				{
					bMoveSelection=TRUE;
				}
				bProcessed=TRUE;
			}
			break;

			// Note: Horizontal toolbars have all rows the same height
			// This code would not work for a vertical toolbars, but vertical toolbars
			// have their VK_UP mapped to VK_LEFT, so never get here
		case VK_UP:
			// previous row, roughly same position
		case VK_DOWN:
			if(m_iKeySelect!=-1 && m_iKeySelect!=maxIndex)
			{
				// next row, roughly same position
				ASSERT(!bVertBar);

				// if this is a one line bar, then this has no meaning
				CRect rectWindow;
				GetWindowRect(rectWindow);
				if (rectWindow.Height() == GetHSize(orHorz).cy)
					break;
				
				// iterate till we are on the right line, then till the centre point of the
				// button passes the centre point of this button. Because GetItemRect is very
				// inefficient, we use EnumItemRects directly, and only move forward thru
				// the buttons. Otherwise, this routine would be much easier.

				CRect rectCurrent;
				GetItemRect(m_iKeySelect, rectCurrent, or);
				CPoint ptCurrentCenter=rectCurrent.CenterPoint();
				CRect rectTest;
				int iButton=0;
				BOOL bWrapped=FALSE;
				BOOL bRowFound=FALSE;
				CSize sizeButton=GetButtonSize(or);
				int nRowHeight=sizeButton.cy+m_cyDefaultGap;
				int yCenterTarget=0;
				CRect rectClient;
				GetClientRect(rectClient);

				// find centre of target line
				if(nChar==VK_UP)
				{
					yCenterTarget=ptCurrentCenter.y-nRowHeight;
					if(yCenterTarget<0)
					{
						// wrap to bottom of toolbar
						yCenterTarget=rectClient.bottom-nRowHeight/2;
					}
				}
				else
				{
					yCenterTarget=ptCurrentCenter.y+nRowHeight;
					if(yCenterTarget>rectClient.bottom)
					{
						// wrap to bottom of toolbar
						yCenterTarget=nRowHeight/2;
					}
				}

				// find start of correct line
				do
				{
					EnumItemRects(iButton, rectTest, bWrapped, or);

					// find out distance between centre points
					int yDiff=rectTest.Height()-yCenterTarget;

					// get the absolute value for comparison purposes
					int ayDiff=yDiff;
					if(ayDiff<0)
					{
						ayDiff=-ayDiff;
					}

					// because the centring of the items may not be precise (windows, for example)
					// we need to do an imprecise comparison here
					
					// if it's near 0, they must be one row apart
					if(ayDiff>-3 && ayDiff <3)
					{
						bRowFound=TRUE;
					}
					else
					{
						++iButton;
					}
				}
				while(iButton<m_nCount && !bRowFound);

				if(!bRowFound)
				{
					// there really should always be a row
					ASSERT(FALSE);
				}
				else
				{
					// now walk until the horz centre point of the button passes the horz
					// centre point of our current button

					int xCenterTarget=ptCurrentCenter.x;

					CRect rectOld=rectTest;

					while(iButton < m_nCount &&
						  rectTest.top<rectOld.bottom &&   // give up if we wrap to next line
						  rectTest.CenterPoint().x < xCenterTarget)
					{
						++iButton;
						rectOld=rectTest;
						EnumItemRects(iButton, rectTest, bWrapped, or);
					}
					
					if(iButton==m_nCount)
					{
						bMoveSelection=TRUE;
						// we walked into the last button, so move to that one
						iNewSelection=m_nCount-1; // assumes last cannot be separator, which
												  // is enforced elsewhere
						ASSERT((GetButtonStyle(iNewSelection) & TBBS_SEPARATOR)!=0);
					}
					else
					{
						// at this point, we've found two possibilities
						// rectOld is at iButton -1, 
						// rectTest is at iButton
						// Which is nearer the center of our button?

						bMoveSelection=TRUE;
						int oldDiff=rectOld.CenterPoint().x - xCenterTarget;
						int newDiff=rectTest.CenterPoint().x - xCenterTarget;
						
						if(oldDiff<0)
						{
							oldDiff=-oldDiff;
						}
						if(newDiff<0)
						{
							newDiff=-newDiff;
						}

						if(oldDiff<newDiff)
						{
							iNewSelection=iButton-1;
						}
						else
						{
							iNewSelection=iButton;
						}

						ASSERT(iNewSelection>0);
						ASSERT(iNewSelection<m_nCount);
					}
				}
				bProcessed=TRUE;
			}
			break;

		case VK_MENU:
			if(nRepCnt>1)
			{
				break;
			}
			if(m_iKeySelect==-1)
			{
				GainKeyFocus(0);
				bMoveSelection=TRUE;
				iNewSelection=0;
				bProcessed=TRUE;
				break;
			}
			else
			{
				// drop thru to escape
			}
		case VK_ESCAPE:
			// cancel keyboard mode
			LoseKeyFocus();
			bMoveSelection=TRUE;
			iNewSelection=-1;
			bProcessed=TRUE;
			break;

		case VK_RETURN:
			if(m_iKeySelect!=-1)
			{
				int iKeySelect=m_iKeySelect;
				LoseKeyFocus();
				
				BOOL bMenu=TRUE;
				BOOL bEnabled=TRUE;
				if(iKeySelect<m_nCount)
				{
					AFX_TBBUTTON *pTBB=_GetButtonPtr(iKeySelect);
					if((pTBB->nStyle & TBBS_MENU)==0)
					{
						bMenu=FALSE;
					}
					if((pTBB->nStyle & TBBS_DISABLED))
					{
						bEnabled=FALSE;
					}
				}
				if(bEnabled)
				{
					if(bMenu)
					{
						ExpandItem(iKeySelect, TRUE, TRUE);
					}
					else
					{
						OnButtonPressed(iKeySelect);
					}
				}
				bProcessed=TRUE;
			}
			break;

		case VK_SUBTRACT:
			if(Menu::IsShowingCmdBars())
			{
				if(	s_pDroppedBar!=s_pMenuBar ||
					s_nDroppedItem!=m_nCount)
				{
					ASSERT(theApp.m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

					CMDIFrameWnd *pMainWnd=(CMDIFrameWnd *)(theApp.m_pMainWnd);

					BOOL bMaximized;
					CWnd *pActive=pMainWnd->MDIGetActive(&bMaximized);
					if(	pActive &&
						s_pCustomizer==NULL)
					{
						s_pMenuBar->ExpandItem(s_pMenuBar->m_nCount, TRUE, TRUE);
					}
					bProcessed=TRUE;
				}
			}
			break;

		default:
			// this is the case where we need to scan for accelerators - nasty, because we
			// have to scan for all that menu related crap, and we have to be sure that it'll 
			// work right on J systems, etc.

			// davidga tells me (martynl) that accelerators can only ever be sbcs, even in an
			// mbcs environment, which makes things somewhat simpler

			// we scan the menu for one full iteration, taking note of the number of times
			// we meet this accelerator, the first time we meet it, and the first time
			// we meet it when iButton is greater than the current iButton. If there is 
			// exactly one instance of this accelerator, then we drop it down. If there is
			// more than one, then we move to the next one after the current position,
			// wrapping around if appropriate.

			// martynl 24May96

			{
				if( s_pDroppedBar!=NULL &&
					s_nDroppedItem!=-1)
				{
					// something is pulled down, pass this key onto that
					CBMenuPopup *pMenu=s_pDroppedBar->GetItemMenu(s_nDroppedItem);
					if(pMenu)
					{
						pMenu->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
						bProcessed=TRUE;
						break;
					}
				}

				if(nChar<'0' || nChar>'Z')
				{
					break;
				}

				int iFirstFind=-1;  // first one (nearest start of bar)
				int iNextFind=-1;   // first one after current item (can be same as iFirstFind)
				int nFinds=0;       // total number of finds

				// this loop will terminate early once iNextFind is found - any button
				// after that would never get selected this tiem anyway
				for(int iButton=0; iButton<m_nCount && (iNextFind==-1 || nFinds<2); ++iButton)
				{
					UINT nStyle=GetButtonStyle(iButton);
					if((nStyle & TBBS_TEXTUAL)!=0 &&
						(nStyle & TBBS_INVISIBLE)==0 )
					{

						TCHAR chAccel=0;
						const CString Label = GetButtonExtra(iButton)->GetLabel();
						ASSERT(!Label.IsEmpty());

						CString strLabel = Label;

						chAccel=GLOBAL_DATA::ExtractAccelerator(Label);
				
						if(chAccel!=0)
						{
							// it's an accelerator, now what's the key code
							if(tolower(chAccel)==tolower(LOBYTE(nChar)))
							{
								// same key - this is one
								++nFinds;

								if(iFirstFind==-1)
								{
									iFirstFind=iButton;
								}

								if(iNextFind==-1 &&
									iButton> m_iKeySelect)
								{
									iNextFind=iButton;
								}
							}
						}
					}
				}
				// now know how many matches
				if(nFinds>0)
				{
					ASSERT(iFirstFind!=-1);
					// some matches, some action
					if(nFinds==1)
					{
						if(iFirstFind!=m_iKeySelect || !(GetButtonStyle(iFirstFind) & TBBS_MENU))
						{
							bMoveSelection=TRUE;
							iNewSelection=iFirstFind;
							bActivateNewSelection=TRUE;
						}
						else
						{
							int iKeySelect=m_iKeySelect;
							LoseKeyFocus();
							ExpandItem(iKeySelect, TRUE, TRUE);
						}
					}
					else
					{
						// if there's one just after us, go for that
						if(iNextFind!=-1)
						{
							bMoveSelection=TRUE;
							iNewSelection=iNextFind;
						}
						else
						{
							// otherwise, wrap around. 

							// Because we found more than one valid possibility, and because
							// iNextFind is -1, there must
							// be one other selection before iNextFind.
							ASSERT(iFirstFind!=m_iKeySelect);

							bMoveSelection=TRUE;
							iNewSelection=iFirstFind;
						}
					}
				}
				bProcessed=TRUE;
			}
			break;

	}

	// act on next selection
	if(bMoveSelection)
	{
		if(m_iKeySelect!=-1)
		{
			int nKeySelect=m_iKeySelect;

			if(nKeySelect!=m_nCount)
			{
				// undraw current selection
				UINT nOldStyle=GetButtonStyle(nKeySelect);

				// if it's a menu, it could be dropped or extruded
				if(nOldStyle & TBBS_MENU)
				{
					if(nOldStyle & TBBS_PRESSED)
					{
						ExpandItem(nKeySelect,FALSE);
						m_bDropPending=TRUE;
						nOldStyle &=~TBBS_PRESSED;
					}
					else
					{
						m_bDropPending=FALSE;
						nOldStyle &=~TBBS_EXTRUDED;
					}
				}
				else
				{
					nOldStyle&=~TBBS_EXTRUDED;
				}

				SetButtonStyle(nKeySelect, nOldStyle);
				InvalidateButton(nKeySelect);
			}
			else
			{
				if(s_pSystemMenu!=NULL)
				{
					ExpandItem(nKeySelect,FALSE);
					m_bDropPending=TRUE;
				}
				else
				{
					m_bDropPending=FALSE;
				}
			}
		}

		m_iKeySelect=iNewSelection;

		if(m_iKeySelect>=0)
		{
			// default style used for system menu
			UINT nNewStyle=TBBS_MENU;
			if(m_iKeySelect!=m_nCount)
			{
				nNewStyle = GetButtonStyle(m_iKeySelect);
			}
			int iKeySelect=m_iKeySelect;

			if(nNewStyle & TBBS_MENU)
			{
				if(m_bDropPending || bActivateNewSelection)
				{
					LoseKeyFocus();
					PostMessage(DSM_DROPMENU, iKeySelect,TRUE);
				}
				else
				{
					nNewStyle |=TBBS_EXTRUDED;
				}
			}
			else
			{
				if(bActivateNewSelection)
				{
					nNewStyle |=TBBS_PRESSED;
				}
				else
				{
					nNewStyle |=TBBS_EXTRUDED;
				}
			}
			

			if(iKeySelect!=m_nCount)
			{
				SetButtonStyle(iKeySelect,  nNewStyle);
				InvalidateButton(iKeySelect);
			}

			if(nNewStyle & TBBS_PRESSED)
			{
				LoseKeyFocus();
				if(nNewStyle & TBBS_MENU)
				{
					ExpandItem(iKeySelect, TRUE, TRUE);
				}
				else
				{
					// command button on menu bar
					if(iKeySelect<m_nCount && iKeySelect>=0)
					{
						// press the button
						OnButtonPressed(iKeySelect);

						// If the command was Exit, we'd better
						// do nothing and return
						if(!theApp.m_bQuitting)
						{
							// draw it unpressed after command is done
							nNewStyle &= ~ TBBS_PRESSED;
							SetButtonStyle(iKeySelect,  nNewStyle);
							InvalidateButton(iKeySelect);
						}
					}
				}
			}
		}
		else
		{
			LoseKeyFocus();
		}
	}
	return bProcessed;
}

afx_msg void CASBar::OnKillFocus(CWnd *pNew)
{
	if(m_iKeySelect!=-1)
	{
		if(m_iKeySelect!=m_nCount)
		{
			// undraw current selection
			UINT nOldStyle=GetButtonStyle(m_iKeySelect);

			// if it's a menu, it could be dropped or extruded
			if(nOldStyle & TBBS_MENU)
			{
				if(nOldStyle & TBBS_PRESSED)
				{
					ExpandItem(m_iKeySelect,FALSE);
					m_bDropPending=TRUE;
					nOldStyle &=~TBBS_PRESSED;
				}
				else
				{
					m_bDropPending=FALSE;
					nOldStyle &=~TBBS_EXTRUDED;
				}
			}
			else
			{
				nOldStyle&=~TBBS_EXTRUDED;
			}

			SetButtonStyle(m_iKeySelect, nOldStyle);
			InvalidateButton(m_iKeySelect);
		}
		m_iKeySelect=-1;
	}

	// ensable ime
	imeEnableIME( NULL, TRUE );
	
	CCustomBar::OnKillFocus(pNew);
}

BOOL CASBar::OnButtonPressed(int iButton)
{
	LoseKeyFocus();
	return CCustomBar::OnButtonPressed(iButton);
}

void CASBar::LoseKeyFocus(void)
{

	// lose the visible focus
	if(m_iKeySelect!=-1 && m_iKeySelect!=m_nCount)
	{
		SetButtonStyle(m_iKeySelect, GetButtonStyle(m_iKeySelect) & ~ TBBS_EXTRUDED);
		InvalidateButton(m_iKeySelect);
	}

	m_iKeySelect=-1;
	m_bDropPending=FALSE;

	// donate the focus to the main MDI child
	if(GetFocus()==this && s_pOldFocus && ::IsWindow(s_pOldFocus->GetSafeHwnd()))
	{
		s_pOldFocus->SetFocus();
	}
	s_pOldFocus=NULL;
}

void CASBar::GainKeyFocus(int iFocus)
{
	// lose parent extrusions
	OnCancelMode();

	s_pOldFocus=GetFocus();
	// take the focus and draw it
	m_iKeySelect=iFocus;
	m_bDropPending=FALSE;
	if(m_iKeySelect!=m_nCount)
	{
		SetButtonStyle(m_iKeySelect, GetButtonStyle(m_iKeySelect) | TBBS_EXTRUDED);
		InvalidateButton(m_iKeySelect);
	}
	::SetFocus(this->GetSafeHwnd());

	// disable ime
	imeEnableIME( NULL, FALSE );
}

// menu calls this when it is completed, to stop the bar tracking
void CASBar::MenuDone()
{
	LoseKeyFocus();
	CCustomBar::MenuDone();
}

// Removes all references to command id from all bars
void CASBar::RemoveAllCommandReferences(UINT id)
{
	// we need to recheck the size of s_ausage every time, because RemoveCommandReferences can result
	// in the deletion of the bar.
	for (int i = 0; i < s_aUsage.GetSize();)
	{
		CASBar *pBar=(CASBar *)(s_aUsage[i]);

		if(!pBar->RemoveCommandReferences(id))
		{
			// only increment if the bar wasn't deleted
			++i;
		}

	}
}

// Removes reference to command id from this bar
BOOL CASBar::RemoveCommandReferences(UINT id)
{
	AFX_TBBUTTON *pTBB=(AFX_TBBUTTON *)m_pData;

	BOOL bDeleted=FALSE;

	for(int iButton=0; iButton<m_nCount;)
	{
		if(pTBB->nID==id)
		{
			DeleteButton(iButton);
			bDeleted=TRUE;

			// in This case we can be sure that the next button is at pTBB, or that iButton will now be >=m_nCount
		}
		else
		{
			// don't increment if we deleted, because things will have moved up
			++iButton;
			++pTBB;
		}
	}

	if(bDeleted)
	{
		if(m_nCount==0)
		{
			// delete bar
			UINT nIDWnd = (UINT) GetDlgCtrlID();
			CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();
			pFrame->m_pManager->RemoveWnd(MAKEDOCKID(PACKAGE_SUSHI, nIDWnd));
			return TRUE;
		}
		else
		{
			RecalcLayout();
		}
	}
	return FALSE;
}

CSize CASBar::GetHSize(ORIENTATION or)
{
	return CCustomBar::GetHSize(or);
}

CSize CASBar::GetSize(UINT nHitTest, CSize size)
{
	return CCustomBar::GetSize(nHitTest, size);
}

// Returns TRUE if the item is expecting to regain the focus to the bar
int CASBar::ExpandItem(int nIndex, BOOL bExpand, BOOL bKeyboard)
{
	int nDroppedItem=CCustomBar::ExpandItem(nIndex, bExpand, bKeyboard);
	
	if(bExpand && nDroppedItem!=-1)
	{
		if(nDroppedItem<m_nCount)
		{
			AFX_TBBUTTON* pTBB = _GetButtonPtr(nDroppedItem);
			pTBB->nStyle &= ~(TBBS_PRESSED);
			pTBB->nStyle |= (TBBS_EXTRUDED);
			InvalidateButton(nDroppedItem);
			GainKeyFocus(nDroppedItem);
		}
		else
		{
			GainKeyFocus(nDroppedItem);
		}
	}
	return nDroppedItem;
}

void CASBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// if there's a keyboard selection, and menus are not dropped then moving the mouse should move that
	if(m_iKeySelect!=-1 && s_pDroppedBar==NULL && GetCustomizer()==NULL)
	{
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

		if(iCurrentHit>=0)
		{
			AFX_TBBUTTON* pTBB=NULL;

			if(m_iKeySelect!=m_nCount)
			{
				pTBB=_GetButtonPtr(m_iKeySelect);
			}

			//deextrude it
			if(pTBB)
			{
				pTBB->nStyle &= ~(TBBS_EXTRUDED);
				InvalidateButton(m_iKeySelect);
			}

			m_iKeySelect=iCurrentHit;

			if(m_iKeySelect!=m_nCount)
			{
				pTBB =_GetButtonPtr(m_iKeySelect);
			}
			else
			{
				pTBB=NULL;
			}

			if(pTBB)
			{
				//deextrude it
				pTBB->nStyle |= TBBS_EXTRUDED;
				InvalidateButton(m_iKeySelect);
			}

			m_iKeySelect=iCurrentHit;
		}
	}
	else
	{
		CCustomBar::OnMouseMove(nFlags, point);
	}
}

static BOOL bLastAltDown=FALSE;			// true if the very last input message was an alt-down message

BOOL CASBar::StealMenuMessage(MSG *pMsg)
{
	// Orion 96 Bug # 13133: Pressing alt in a tabbed dialog when not in a control resulted
	// in the main menu getting activated.
	// Don't steal messages if the main frame is not active
	if (!AfxGetMainWnd()->IsWindowEnabled())
	{
		return FALSE ;
	}

	// don't steal keyboard messages during customization
	if(s_pCustomizer!=NULL)
	{
		bLastAltDown=FALSE;

		return FALSE;
	}

	// We only pass the alt message to the menu bar if it happens ALTdown/ALTup with no other input messages between
	if(Menu::IsShowingCmdBars())
	{
		if(pMsg->message==WM_SYSKEYDOWN &&
			pMsg->wParam==VK_MENU &&
			LOWORD(pMsg->lParam)<=1 &&
			(pMsg->lParam & 0x40000000)==0) // don't set it true if this is an autorepeat. Bit 30 is set if the key was down before the message was send, implying a resend
		{
			bLastAltDown=TRUE;
		}
		else
		{
			// Orion 14688 - The WM_SYSKEYUP is translated to a WM_KEYUP by the time it gets here.
			if((pMsg->message==WM_SYSKEYUP || pMsg->message==WM_KEYUP) &&
				pMsg->wParam==VK_MENU &&
				Menu::IsShowingCmdBars() &&
				bLastAltDown)
			{
				bLastAltDown=FALSE;
				// send alt thru to custom bar
				if(s_pMenuBar->KeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)))
				{
					return TRUE;
				}
			}

			if(	bLastAltDown &&
				((pMsg->message>=WM_KEYFIRST && pMsg->message<=WM_KEYLAST) ||
				(pMsg->message>=WM_MOUSEFIRST && pMsg->message<=WM_MOUSELAST)) )
			{
				//Orion 96 Bug 14688: Releasing capture sends fake mousemoves. If the mouse
				// move is fake, ignore it.
				if (pMsg->message == WM_MOUSEMOVE) 
				{
					if (pMsg->wParam == 0x0000) // (0,0) 
					{
						return FALSE ;
					}					
				}
				
				if( pMsg->message!=WM_SYSKEYDOWN ||
					pMsg->wParam!=VK_MENU)
				{
					// don't set it false if this is an autorepeat
					bLastAltDown=FALSE;
				}
			}
		}
	}

	if((pMsg->message==WM_SYSKEYDOWN) &&
		((	pMsg->wParam>='0' &&
			pMsg->wParam<='Z' ) ||
			pMsg->wParam==VK_SUBTRACT) &&
		Menu::IsShowingCmdBars() &&
		s_pMenuBar)
	{
		// send thru to custom bar
		if(s_pMenuBar->KeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)))
		{
			return TRUE;
		}
	}

	if((pMsg->message==WM_SYSKEYDOWN ||
		pMsg->message==WM_KEYDOWN) &&
		Menu::IsShowingCmdBars() &&
		s_pDroppedBar!=NULL &&
		s_nDroppedItem!=-1)
	{
		// send thru to custom menu
		CBMenuPopup *pMenu=s_pDroppedBar->GetItemMenu(CASBar::s_nDroppedItem);
		if(pMenu)
		{
			pMenu->KeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			return TRUE;
		}
	}

	return FALSE;
}

// Take key messages for the toolbar navigation very early on.
BOOL CASBar::EarlyStealMenuMessage(MSG *pMsg)
{
	// Orion 96 Bug # 13133: Pressing alt in a tabbed dialog when not in a control resulted
	// in the main menu getting activated.
	// Don't steal messages if the main frame is not active
	if (!AfxGetMainWnd()->IsWindowEnabled())
	{
		return FALSE ;
	}

	// This is the same as the alt-down code in Steal, but we're only interested in clearing the last alt flag
	// We only pass the alt message to the menu bar if it happens ALTdown/ALTup with no other input messages between
	if(Menu::IsShowingCmdBars())
	{
		if(pMsg->message==WM_SYSKEYDOWN &&
			pMsg->wParam==VK_MENU &&
			LOWORD(pMsg->lParam)<=1 &&
			(pMsg->lParam & 0x40000000)==0) // don't set it true if this is an autorepeat
		{
			// do nothing
		}
		else
		{
			if((pMsg->message==WM_SYSKEYUP || pMsg->message==WM_KEYUP) &&
				pMsg->wParam==VK_MENU &&
				Menu::IsShowingCmdBars() &&
				bLastAltDown)
			{
				// in the steal code, this will process the alt. Here, it must just be ignored.
			}
			else
			{
				if(	bLastAltDown &&
					((pMsg->message>=WM_KEYFIRST && pMsg->message<=WM_KEYLAST) ||
					(pMsg->message>=WM_MOUSEFIRST && pMsg->message<=WM_MOUSELAST)) )
				{
					//Orion 96 Bug 14688: Releasing capture sends fake mousemoves. If the mouse
					// move is fake, ignore it.
					if (pMsg->message == WM_MOUSEMOVE) 
					{
						if (pMsg->wParam == 0x0000) // (0,0) 
						{
							return FALSE ;
						}					
					}
					
					if( pMsg->message!=WM_SYSKEYDOWN ||
						pMsg->wParam!=VK_MENU)
					{
						// don't set it false if this is an autorepeat
						bLastAltDown=FALSE;
					}
				}
			}
		}
	}
	else
	{
		if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP)
		{
			CASBar::ForceReleaseCapture();
		}
	}

	if((pMsg->message==WM_SYSKEYDOWN ||
		pMsg->message==WM_KEYDOWN) &&
		pMsg->wParam!=VK_MENU &&
		Menu::IsShowingCmdBars() &&
		s_pMenuBar &&
		s_pMenuBar->m_iKeySelect>=0 &&
		s_pDroppedBar==NULL)
	{
		// send thru to custom bar
		if(s_pMenuBar->KeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)))
		{
			return TRUE;
		}
	}
	return FALSE;
}

// Reset to default buttons
void CASBar::Reset(void)
{
	CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();
	CDockWorker *pDocker=pFrame->m_pManager->WorkerFromWindow(this);

	CPackage* pGrp = theApp.GetPackage(pDocker->m_nIDPackage);
	if (pGrp == NULL)
		return;
	
	HGLOBAL hglob = pGrp->GetToolbarData(LOWORD(pDocker->m_nIDWnd));

	if (hglob != NULL)
	{
		if(CASBar::s_pDroppedBar==this)
		{
			CASBar::CancelMenu();
		}

		// first we need to remove the selection
		if(CASBar::s_pCustomizer &&
			CASBar::s_pCustomizer->m_pSelectBar &&
			CASBar::s_pCustomizer->m_pSelectBar->GetWindow()==this)
		{
			CASBar::s_pCustomizer->SetSelection(NULL,0);
		}

		TOOLBARINIT FAR* lptbi = (TOOLBARINIT*) ::GlobalLock(hglob);
		UINT FAR* lpIDArray = (UINT FAR*) (lptbi + 1);

		DynSetButtons(lpIDArray, lptbi->nIDCount);

		if(LOWORD(pDocker->m_nIDWnd)==IDTB_MENUBAR)
		{
			CMainFrame *pFrame=(CMainFrame *)theApp.m_pMainWnd;

			pFrame->RebuildMenus();
		}

		RecalcLayout();
		
		::GlobalUnlock(hglob);
		::GlobalFree(hglob);
	}
}

void CASBar::ForceReleaseCapture(void)
{
	CWnd* pWndWithCapture = GetCapture() ;
	if (pWndWithCapture && pWndWithCapture->IsKindOf(RUNTIME_CLASS(CASBar)))
	{
		// Turn capture off.
		((CASBar *)pWndWithCapture)->m_iButtonCapture = -1;
		ReleaseCapture();
	}
}
