///////////////////////////////////////////////////////////////////////////////
//  BDRBAR.CPP
//      Contains implementations for CBorderBar.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "shell.h"
#include "barbdr.h"
#include "barglob.h"
#include "util.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//	CBorderBar
//		Class for wrapping a single window with a border area when docked
//		so that it sort of looks like a toolbar for one window.  The border,
//		however, disappears when the window is floating.
//

#ifdef _DEBUG
extern BOOL bChicagoLookOnNT;
inline BOOL OnChicago() { return theApp.m_bWin4 || bChicagoLookOnNT; }
#else
inline BOOL OnChicago() { return theApp.m_bWin4; }
#endif

// Buttons larger than this value will be drawn with different drawing code.
const int cLargeCaptionButtonThreshold=8;

//	CBorderBar::CBorderBar
//		Constructor.

CBorderBar::CBorderBar()
{
	m_cyTopBorder = m_cyBottomBorder = g_mt.cyToolBorder;
	m_cxLeftBorder = m_cxRightBorder = g_mt.cxToolBorder;

	m_bCaption = FALSE;
	m_bHorz = FALSE;
	m_bBtnClose = FALSE;
	m_bBtnMax = FALSE;

	m_nBtnCapture = BUTTON_NIL;
	m_bBtnPushed = FALSE;
}

CBorderBar::~CBorderBar()
{
}

//	CBorderBar::Create
//		Creates the window, puts the child in the window, and registers
//		with the docks when necessary.

BOOL CBorderBar::Create(CWnd* pParent, CDockManager* pManager, DWORD dwStyle,
	HWND hwndChild, UINT nID, BOOL bRegister /*=FALSE*/, DOCKINIT FAR* lpdi /*=NULL*/)
{
    // create the HWND
    CRect rect;
    rect.SetRectEmpty();	// Size later.
    
    if (pParent == NULL)
    	pParent = pManager->GetDock(dpHide);
    
	m_nID =	nID;
	ASSERT(LOWORD(m_nID) == ::GetDlgCtrlID(hwndChild));
    if (pParent == NULL || !CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS,
            						::LoadCursor(NULL, IDC_ARROW),
            						(HBRUSH) (COLOR_BTNFACE + 1)),
            	"",
    			dwStyle | WS_CHILD | WS_BORDER | WS_CLIPCHILDREN,
    			rect,
    			pParent,
    			LOWORD(m_nID)))
	{
        return FALSE;
	}

    ::SetParent(hwndChild, GetSafeHwnd());
    ::ShowWindow(hwndChild, SW_SHOWNOACTIVATE);
    
	if (dwStyle & DWS_STRETCHY)
	{
		m_bBtnClose = m_bBtnMax = TRUE;
	}
	else
	{
		m_bBtnClose=TRUE;
		m_bBtnMax=FALSE;
	}

	EnableToolTips();

    if (bRegister)
    {
		if (pManager->RegisterWnd(this, lpdi) == NULL)
		{
	        DestroyWindow();
	        return FALSE;
		}

		// Only after registering do we have the correct dock position for
		// layout calcultation.

		RecalcLayout();
	}

    return TRUE;
}

void CBorderBar::RecalcLayout()
{
    CRect rect;
    CWnd* pChild;
	DOCKPOS dp;
    
    GetClientRect(rect);
    if ((pChild = GetTopWindow()) != NULL && (dp = DkWGetDock(m_nID)) != dpHide)
    {
    	if (dp == dpNil || dp == dpMDI)
		{
			m_bCaption = FALSE;
			if (!OnChicago())
        		rect.InflateRect(g_mt.cxBorder, g_mt.cyBorder);
		}
        else
        {
			m_bCaption = TRUE;
			if (dp == dpLeft || dp == dpRight)
			{
				m_bHorz = FALSE;
				m_cxLeftBorder = g_mt.cxToolBorder;
				m_cyTopBorder = g_mt.cyToolCaption + g_mt.cyToolCapBorder * 2;
			}
			else
			{
				m_bHorz = TRUE;
				m_cxLeftBorder = g_mt.cxToolCaption + g_mt.cxToolCapBorder * 2;
				m_cyTopBorder = g_mt.cyToolBorder;

				// If toolbar is visible, it provides the top border.
				// But, if it is vertically docked we need space for the
				// caption.

				CWnd* pBar = GetDescendantWindow(AFX_IDW_TOOLBAR);
				if (pBar != NULL && (pBar->GetStyle() & WS_VISIBLE) != 0)
					m_cyTopBorder -= g_mt.cyToolBorder;
			}

        	rect.left += m_cxLeftBorder;
        	rect.top += m_cyTopBorder;
        	rect.right -= m_cxRightBorder;
        	rect.bottom -= m_cyBottomBorder;
        }
		
		CRect rectCurrent;
		pChild->GetWindowRect(rectCurrent);
		ScreenToClient(rectCurrent);
        
		if (rect != rectCurrent)
		{
	        pChild->SetWindowPos(NULL, rect.left, rect.top,
	            rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		}
    }
}

IMPLEMENT_DYNAMIC(CBorderBar, CWnd)

BEGIN_MESSAGE_MAP(CBorderBar, CWnd)
	//{{AFX_MSG_MAP(CBorderBar)
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETMINMAXINFO, OnGetMinMaxInfo)
	ON_REGISTERED_MESSAGE(DWM_GETMOVESIZE, OnGetMoveSize)
    ON_REGISTERED_MESSAGE(DWM_GETTRACKSIZE, OnGetTrackSize)
	ON_REGISTERED_MESSAGE(DWM_SHOWWINDOW, OnRouteChildMessage)
	ON_REGISTERED_MESSAGE(DWM_GETDATA, OnRouteChildMessage)
	ON_REGISTERED_MESSAGE(DWM_ENABLECHILD, OnEnableChild)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0x0000, 0xffff, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0x0000, 0xffff, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBorderBar message handlers

// Override to allow hwnd buttons to nominate an id for their tooltip, depending
// on their context
int CBorderBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);

	int nHit=InCaptionButtonRect(point);

	if(nHit!=BUTTON_NIL)
	{
		CRect btn;
		int nTip=0;
		GetCaptionButtonRect(nHit, btn);
		pTI->rect=btn;
		switch(nHit)
		{
			case BUTTON_CLOSE:
				nTip=IDS_DOCK_HIDE;
				break;

			case BUTTON_MAX:
				{
					MAX_STATE stateMax = DkWGetMaxState(m_nID);
					if (stateMax == stateMaximize ||
						stateMax == stateDisabled)
					{
						nTip=IDS_DOCK_EXPAND;
					}
					else
					{
						nTip=IDS_DOCK_CONTRACT;
					}
				}
				break;

			default: 
				ASSERT(FALSE);
		}
		pTI->uId = nHit;
		pTI->hwnd = m_hWnd;
		CString strTip;
		VERIFY(strTip.LoadString(nTip));
		// will be freed in tip code
		pTI->lpszText = _strdup(strTip);
	}
	else
	{
		nHit=-1;
	}


	return nHit;
}

//	CBorderBar::WindowProc
//		All dockable windows call DkPreHandle message.

LRESULT CBorderBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG lResult;

	if (DkPreHandleMessage(GetSafeHwnd(),
		message, wParam, lParam, &lResult))
		return lResult;

	return CWnd::WindowProc(message, wParam, lParam);
}

//	CBorderBar::OnCommand
//		Routes all commands to the window's owner.

BOOL CBorderBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return !!GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
}

//	CBorderBar::PostNcDestroy
//		Self deletion.

void CBorderBar::PostNcDestroy()
{
	delete this;
}

//	CBorderBar;:OnRouteChildMessage
//		For routing DWM_* from the dock manager to the child window.

LRESULT CBorderBar::OnRouteChildMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(GetTopWindow() != NULL);
	return	GetTopWindow()->SendMessage(GetCurrentMessage()->message,
		wParam, lParam);
}

LRESULT CBorderBar::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
	ASSERT(GetTopWindow() != NULL);
	GetTopWindow()->SendMessage(WM_GETMINMAXINFO, wParam, lParam);
	
	if (OnChicago())
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

		lpMMI->ptMinTrackSize.x += g_mt.cxBorder * 2;
		lpMMI->ptMinTrackSize.y += g_mt.cyBorder * 2;
		lpMMI->ptMaxTrackSize.x += g_mt.cxBorder * 2;
		lpMMI->ptMaxTrackSize.y += g_mt.cyBorder * 2;
	}

	return 0L;
}

LRESULT CBorderBar::OnGetTrackSize(WPARAM wParam, LPARAM lParam)
{
	return OnSizeMessage(DWM_GETTRACKSIZE, dpNil, wParam, lParam);
}

//	CBorderBar::OnGetMoveSize
//		Docked window size grows by the size of the border, so that child
//		window size will not change to show the border when docked.

LRESULT CBorderBar::OnGetMoveSize(WPARAM wParam, LPARAM lParam)
{
	return OnSizeMessage(DWM_GETMOVESIZE, (DOCKPOS) wParam, wParam, lParam);
}

LRESULT CBorderBar::OnSizeMessage(UINT message, DOCKPOS dp,
	WPARAM wParam, LPARAM lParam)
{
	CSize size((short) LOWORD(lParam), (short) HIWORD(lParam));

	if (dp == dpHorzInit)
	{
		size.cx -= g_mt.cxToolCaption + g_mt.cxToolCapBorder * 2 +
			m_cxRightBorder + g_mt.cxBorder * 2;
		size.cy -= g_mt.cyToolBorder + m_cyBottomBorder + g_mt.cyBorder * 2;
	}
	else if(dp == dpVertInit)
	{
		size.cx -= g_mt.cxToolBorder + m_cxRightBorder + g_mt.cxBorder * 2;
		size.cy -= g_mt.cyToolCaption + g_mt.cxToolCapBorder * 2 +
			m_cyBottomBorder + g_mt.cyBorder * 2;
	}
	else if (OnChicago())
	{
		size.cx -= g_mt.cxBorder * 2;
		size.cy -= g_mt.cyBorder * 2;
	}
	
	LRESULT lResult = GetTopWindow()->SendMessage(message,
		wParam, MAKELPARAM((WORD)(short) size.cx, (WORD)(short) size.cy));
		
	if (lResult)
	{
		size.cx = (short) LOWORD(lResult);
		size.cy = (short) HIWORD(lResult);
	}
	
	switch (dp)
	{
	case dpTop:
	case dpBottom:
	case dpHorzInit:
		//BLOCK:
		{
			size.cx += g_mt.cxToolCaption + g_mt.cxToolCapBorder * 2 +
				m_cxRightBorder + g_mt.cxBorder * 2;
			size.cy += g_mt.cyToolBorder + m_cyBottomBorder + g_mt.cyBorder * 2;

			CWnd* pBar = GetDescendantWindow(AFX_IDW_TOOLBAR);
			if (pBar != NULL && (pBar->GetStyle() & WS_VISIBLE) != 0)
				size.cy -= g_mt.cyToolBorder;
		}
		break;

	case dpLeft:
	case dpRight:
	case dpVertInit:
		size.cx += g_mt.cxToolBorder + m_cxRightBorder + g_mt.cxBorder * 2;
		size.cy += g_mt.cyToolCaption + g_mt.cxToolCapBorder * 2 +
			m_cyBottomBorder + g_mt.cyBorder * 2;
		break;

	case dpNil:
		if (OnChicago())
		{
			size.cx += g_mt.cxBorder * 2;
			size.cy += g_mt.cyBorder * 2;
		}
		break;
	}
	
	return MAKELRESULT((WORD)(short) size.cx, (WORD)(short) size.cy);
}

//	CBorderBar::OnEnableChild
//		Calls enable window on the child window.  Used to disable input
//		during customize mode.

LRESULT CBorderBar::OnEnableChild(UINT wParam, LPARAM lParam)
{
	ASSERT(GetTopWindow() != NULL);
	GetTopWindow()->EnableWindow(wParam);
	
	return 0L;
}

void CBorderBar::GetCaptionButtonRect(int nType, CRect& rectButton) const
{
	CRect rectClient;
	GetClientRect(rectClient);

	CPoint ptTopLeft;

	// Horizontal windows have a vertical caption.
	if (m_bHorz)
	{
		ptTopLeft.x = rectClient.left +
			((g_mt.cxToolCaption + g_mt.cxToolCapBorder * 2) - g_mt.cxToolCapButton) / 2;
		ptTopLeft.y = g_mt.cyToolBorder +
			(g_mt.cyToolCapButton + g_mt.cyToolCapGap) * nType;
	}
	else
	{
		ptTopLeft.x = rectClient.right - g_mt.cxToolBorder - g_mt.cxToolCapButton -
			(g_mt.cxToolCapButton + g_mt.cxToolCapGap) * nType;
		ptTopLeft.y = rectClient.top +
			((g_mt.cyToolCaption + g_mt.cyToolCapBorder * 2) - g_mt.cyToolCapButton) / 2;
	}

	rectButton.SetRect(ptTopLeft.x, ptTopLeft.y,
		ptTopLeft.x + g_mt.cxToolCapButton, ptTopLeft.y + g_mt.cyToolCapButton);
}

int CBorderBar::InCaptionButtonRect(CPoint point) const
{
	if (m_bBtnClose)
	{
		CRect rect;
		GetCaptionButtonRect(BUTTON_CLOSE, rect);
		if (rect.PtInRect(point))
			return BUTTON_CLOSE;
	}
	
	if (m_bBtnMax)
	{
		CRect rect;
		GetCaptionButtonRect(BUTTON_MAX, rect);
		if (rect.PtInRect(point))
			return BUTTON_MAX;
	}

	return BUTTON_NIL;
}

void CBorderBar::DrawCaptionButton(int nType, CDC* pdc, BOOL bPushed)
{
	CRect rectButton;
	GetCaptionButtonRect(nType, rectButton);
	if (!::RectVisible(pdc->m_hDC, rectButton))
		return;

	// Draw the button itself
	COLORREF colorLt = globalData.clrBtnHilite;
	COLORREF colorDk = globalData.clr3dDkShadow;
	if (bPushed)
		pdc->Draw3dRect(rectButton, colorDk, colorLt);
	else
		pdc->Draw3dRect(rectButton, colorLt, colorDk);

	rectButton.InflateRect(-1, -1);
	colorLt = globalData.clrBtnFace;
	colorDk = globalData.clrBtnShadow;
	if (bPushed)
		pdc->Draw3dRect(rectButton, colorDk, colorLt);
	else
		pdc->Draw3dRect(rectButton, colorLt, colorDk);

	rectButton.InflateRect(-1, -1);
	pdc->FillSolidRect(rectButton, colorLt);

	if (bPushed)
		rectButton.OffsetRect(1, 1);

	// Draw the glyph on the button
	BOOL bLarge=FALSE;
	if(g_mt.cxToolCapButton>cLargeCaptionButtonThreshold)
	{
		bLarge=TRUE;
	}

	CPen penDk(PS_SOLID, 1, globalData.clr3dDkShadow);
	CPen* ppenOld = pdc->SelectObject(&penDk);

	if (nType == BUTTON_CLOSE)
	{
		if(bLarge)
		{
			CPoint ptCenter=rectButton.CenterPoint();
			// center isn't the real center, since we're offset.
			ptCenter.x-=1;
			ptCenter.y-=1;

#if 0
			pdc->MoveTo(rectButton.left+1, rectButton.top);
			pdc->LineTo(ptCenter.x+1, ptCenter.y);
			pdc->MoveTo(rectButton.right-3, rectButton.top);
			pdc->LineTo(ptCenter.x-1, ptCenter.y);
			pdc->MoveTo(rectButton.left+1, rectButton.bottom-2);
			pdc->LineTo(ptCenter.x+1, ptCenter.y);
			pdc->MoveTo(rectButton.right-3, rectButton.bottom-2);
			pdc->LineTo(ptCenter.x-1, ptCenter.y);
#endif
			pdc->MoveTo(rectButton.left+1, rectButton.top+1);
			pdc->LineTo(rectButton.right - 2, rectButton.bottom - 2);
			pdc->MoveTo(rectButton.left+2, rectButton.top+1);
			pdc->LineTo(rectButton.right - 1, rectButton.bottom - 2);
			pdc->MoveTo(rectButton.right - 3, rectButton.top+1);
			pdc->LineTo(rectButton.left , rectButton.bottom - 2);
			pdc->MoveTo(rectButton.right - 2, rectButton.top+1);
			pdc->LineTo(rectButton.left +1, rectButton.bottom - 2);
		}
		else
		{
			pdc->MoveTo(rectButton.left, rectButton.top);
			pdc->LineTo(rectButton.right - 1, rectButton.bottom - 1);
			pdc->MoveTo(rectButton.right - 2, rectButton.top);
			pdc->LineTo(rectButton.left - 1, rectButton.bottom - 1);
		}
	}
	else if (nType == BUTTON_MAX)
	{
		MAX_STATE stateMax = DkWGetMaxState(m_nID);

		CPen penGray(PS_SOLID, 1, globalData.clrBtnShadow);
		if (stateMax == stateDisabled)
			pdc->SelectObject(&penGray);

		if (m_bHorz)
		{
			if (stateMax == stateRestore)
			{
				pdc->MoveTo(rectButton.left, rectButton.top);
				pdc->LineTo(rectButton.left, rectButton.bottom - 1);
				pdc->MoveTo(rectButton.left + 1, rectButton.top + 1);
				pdc->LineTo(rectButton.left + 1, rectButton.bottom - 2);
				if(bLarge)
				{
					pdc->MoveTo(rectButton.left + 2, rectButton.top + 2);
					pdc->LineTo(rectButton.left + 2, rectButton.bottom - 3);
					pdc->MoveTo(rectButton.left + 3, rectButton.top + 3);
					pdc->LineTo(rectButton.left + 3, rectButton.bottom - 4);
				}
				else
				{
					pdc->SelectObject(&penGray);
					pdc->MoveTo(rectButton.left + 2, rectButton.top + 1);
					pdc->LineTo(rectButton.left - 1, rectButton.top - 2);
					pdc->MoveTo(rectButton.left + 2, rectButton.bottom - 3);
					pdc->LineTo(rectButton.left - 1, rectButton.bottom);
				}
			}
			else
			{
				pdc->MoveTo(rectButton.right - 2, rectButton.top);
				pdc->LineTo(rectButton.right - 2, rectButton.bottom - 1);
				pdc->MoveTo(rectButton.right - 3, rectButton.top + 1);
				pdc->LineTo(rectButton.right - 3, rectButton.bottom - 2);
				
				if(bLarge)
				{
					pdc->MoveTo(rectButton.right - 4, rectButton.top+2);
					pdc->LineTo(rectButton.right - 4, rectButton.bottom - 3);
					pdc->MoveTo(rectButton.right - 5, rectButton.top + 3);
					pdc->LineTo(rectButton.right - 5, rectButton.bottom - 4);
				}
				else
				{
					pdc->SelectObject(&penGray);
					pdc->MoveTo(rectButton.right - 4, rectButton.top + 1);
					pdc->LineTo(rectButton.right - 1, rectButton.top - 2);
					pdc->MoveTo(rectButton.right - 4, rectButton.bottom - 3);
					pdc->LineTo(rectButton.right - 1, rectButton.bottom);
				}
			}
		}
		else
		{
			if (stateMax == stateRestore)
			{
				pdc->MoveTo(rectButton.left, rectButton.top);
				pdc->LineTo(rectButton.right - 1, rectButton.top);
				pdc->MoveTo(rectButton.left + 1, rectButton.top + 1);
				pdc->LineTo(rectButton.right - 2, rectButton.top + 1);

				if(bLarge)
				{
					pdc->MoveTo(rectButton.left + 2, rectButton.top + 2);
					pdc->LineTo(rectButton.right - 3, rectButton.top + 2);
					pdc->MoveTo(rectButton.left + 3, rectButton.top + 3);
					pdc->LineTo(rectButton.right - 4, rectButton.top + 3);
				}
				else
				{
					pdc->SelectObject(&penGray);
					pdc->MoveTo(rectButton.left + 1, rectButton.top + 2);
					pdc->LineTo(rectButton.left - 2, rectButton.top - 1);
					pdc->MoveTo(rectButton.right - 3, rectButton.top + 2);
					pdc->LineTo(rectButton.right, rectButton.top - 1);
				}
			}
			else
			{
				pdc->MoveTo(rectButton.left, rectButton.bottom - 2);
				pdc->LineTo(rectButton.right - 1, rectButton.bottom - 2);
				pdc->MoveTo(rectButton.left + 1, rectButton.bottom - 3);
				pdc->LineTo(rectButton.right - 2, rectButton.bottom - 3);
				
				if(bLarge)
				{
					pdc->MoveTo(rectButton.left + 2 , rectButton.bottom - 4);
					pdc->LineTo(rectButton.right - 3, rectButton.bottom - 4);
					pdc->MoveTo(rectButton.left + 3, rectButton.bottom - 5);
					pdc->LineTo(rectButton.right - 4, rectButton.bottom - 5);
				}
				else
				{
					pdc->SelectObject(&penGray);
					pdc->MoveTo(rectButton.left + 1, rectButton.bottom - 4);
					pdc->LineTo(rectButton.left - 2, rectButton.bottom - 1);
					pdc->MoveTo(rectButton.right - 3, rectButton.bottom - 4);
					pdc->LineTo(rectButton.right, rectButton.bottom - 1);
				}
			}
		}
		// [fabriced] This call is necessary because there is a CPen going out of scope.
		// we need to deselect the object first, so that it can be deleted.
		pdc->SelectObject(ppenOld);
	}

	pdc->SelectObject(ppenOld);
}

void CBorderBar::OnPaint()
{
	CPaintDC dc(this);

	if (m_bCaption)
	{
		CRect rect;
		GetClientRect(rect);

		// Horizontal windows have a vertical caption.
		if (m_bHorz)
		{
			rect.InflateRect(-g_mt.cxToolCapBorder, -g_mt.cyToolBorder);
			rect.right = rect.left + g_mt.cxToolCaption;
		}
		else
		{
			rect.InflateRect(-g_mt.cxToolBorder, -g_mt.cyToolCapBorder);
			rect.bottom = rect.top + g_mt.cyToolCaption;
		}

		CRect rcBtn;
		if (m_bBtnMax)
			GetCaptionButtonRect(BUTTON_MAX, rcBtn);
		else if (m_bBtnClose)
			GetCaptionButtonRect(BUTTON_CLOSE, rcBtn);
		if (!rcBtn.IsRectNull())
		{
			// Subtract the button rect from the bar rect.
			if (m_bHorz)
				rect.top = rcBtn.bottom + 3;
			else
				rect.right = rcBtn.left - 3;
		}

		COLORREF colorLt = ::GetSysColor(COLOR_BTNHILIGHT);
		COLORREF colorDk = ::GetSysColor(COLOR_BTNSHADOW);
		dc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(),
			::GetSysColor(COLOR_BTNFACE));
		if (m_bHorz)
		{
			dc.Draw3dRect(rect.left, rect.top, 3, rect.Height(),
				colorLt, colorDk);
			dc.Draw3dRect(rect.left + 4, rect.top, 3, rect.Height(),
				colorLt, colorDk);
		}
		else
		{
			dc.Draw3dRect(rect.left, rect.top, rect.Width(), 3,
				colorLt, colorDk);
			dc.Draw3dRect(rect.left, rect.top + 4, rect.Width(), 3,
				colorLt, colorDk);
		}

		if (m_bBtnClose)
			DrawCaptionButton(BUTTON_CLOSE, &dc, FALSE);
		if (m_bBtnMax)
			DrawCaptionButton(BUTTON_MAX, &dc, FALSE);
	}
}

void CBorderBar::OnNcPaint()
{
	if (OnChicago())
		return;		// Dockwnd paints the border.

	CWnd::OnNcPaint();
}

//	CBorderBar::OnNcHitTest
//		Checks for HTCAPTION for dock managed moving.  May have to route to
//		the child to get its HTCAPTION, if it is disabled.

UINT CBorderBar::OnNcHitTest(CPoint point)
{
	ASSERT(GetTopWindow() != NULL);
	
	CRect rect;
	GetTopWindow()->GetWindowRect(rect);
	
	if (rect.PtInRect(point))
	{
		// In case the child window is disabled.
		UINT nHitTest = (UINT) GetTopWindow()->SendMessage(WM_NCHITTEST,
			0, MAKELPARAM(point.x, point.y));
		
		if (nHitTest != HTCAPTION)
			return HTCLIENT;
	}

	ScreenToClient(&point);
	if (InCaptionButtonRect(point) != BUTTON_NIL)
		return HTCLIENT;

	return HTCAPTION;
}

//	CBorderBar::OnLButtonDown
//		Child window must be disabled, and not returning HTCAPTION, so
//		we beep to let the user now that the normal functionality is disabled.
//		Also handle button clicks for caption buttons.

void CBorderBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nButton = InCaptionButtonRect(point);
	if (nButton == BUTTON_NIL)
	{
		MessageBeep(UINT(-1));	// Only happens when child is disabled.
		return;
	}
	else if (nButton == BUTTON_MAX && DkWGetMaxState(m_nID) == stateDisabled)
	{
		return;
	}

	SetCapture();
	m_nBtnCapture = nButton;
	m_bBtnPushed = TRUE;

	CClientDC dc(this);
	DrawCaptionButton(m_nBtnCapture, &dc, TRUE);
}

void CBorderBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nBtnCapture == BUTTON_NIL)
		return;

	BOOL bClose = FALSE;
	if (InCaptionButtonRect(point) == m_nBtnCapture)
	{
		CRect rectButton;
		GetCaptionButtonRect(m_nBtnCapture, rectButton);
		InvalidateRect(rectButton);

		if (m_nBtnCapture == BUTTON_CLOSE)
			bClose = TRUE;
		else if (m_nBtnCapture == BUTTON_MAX)
			DkWMaximizeRestore(m_nID);
	}

	ReleaseCapture();
	m_nBtnCapture = BUTTON_NIL;
	m_bBtnPushed = FALSE;

	// Do this last, since it may destroy the window.
	if (bClose)
		DkWShowWindow(m_nID, FALSE);
}

void CBorderBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nBtnCapture == BUTTON_NIL)
		return;

	BOOL bChange = FALSE;

	if (GetCapture() != this)
	{
		bChange = m_bBtnPushed;

		m_nBtnCapture = BUTTON_NIL;
		m_bBtnPushed = FALSE;
	}
	else if (InCaptionButtonRect(point) == m_nBtnCapture)
	{
		bChange = !m_bBtnPushed;
		m_bBtnPushed = TRUE;
	}
	else
	{
		bChange = m_bBtnPushed;
		m_bBtnPushed = FALSE;
	}

	if (bChange)
	{
		CClientDC dc(this);
		DrawCaptionButton(m_nBtnCapture, &dc, m_bBtnPushed);
	}
}

//	CBorderBar::OnSize
//		Size the child to stay with this window, and compensate for any
//		border we may have to show because of the dock.

void CBorderBar::OnSize(UINT nType, int cx, int cy)
{
    if (nType == SIZE_RESTORED)
		RecalcLayout();
     
	CWnd::OnSize(nType, cx, cy);
}

//	CBorderBar::OnSetFocus
//		Pass focus to the child.

void CBorderBar::OnSetFocus(CWnd* pOldWnd)
{
	CWnd* pChild = GetTopWindow();
	if (pChild != NULL)
		pChild->SetFocus();
}

BOOL CBorderBar::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;

	return CTheApp::HandleToolTipText(nID, pNMHDR, pResult);
}

