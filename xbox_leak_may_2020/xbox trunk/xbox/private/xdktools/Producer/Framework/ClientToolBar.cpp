// ClientToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ClientToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VERSION_WIN4    MAKELONG(0, 4)
#define VERSION_IE3     MAKELONG(70, 4)
#define VERSION_IE4     MAKELONG(71, 4)
#define VERSION_IE401   MAKELONG(72, 4)

struct AFX_DLLVERSIONINFO
{
		DWORD cbSize;
		DWORD dwMajorVersion;                   // Major version
		DWORD dwMinorVersion;                   // Minor version
		DWORD dwBuildNumber;                    // Build number
		DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
};

typedef HRESULT (CALLBACK* AFX_DLLGETVERSIONPROC)(AFX_DLLVERSIONINFO *);

int _afxComCtlVersion = -1;

DWORD AFXAPI _AfxGetComCtlVersion()
{
	// return cached version if already determined...
	if (_afxComCtlVersion != -1)
		return _afxComCtlVersion;

	// otherwise determine comctl32.dll version via DllGetVersion
	HINSTANCE hInst = ::GetModuleHandleA("COMCTL32.DLL");
	ASSERT(hInst != NULL);
	AFX_DLLGETVERSIONPROC pfn;
	pfn = (AFX_DLLGETVERSIONPROC)GetProcAddress(hInst, "DllGetVersion");
	DWORD dwVersion = VERSION_WIN4;
	if (pfn != NULL)
	{
		AFX_DLLVERSIONINFO dvi;
		memset(&dvi, 0, sizeof(dvi));
		dvi.cbSize = sizeof(dvi);
		HRESULT hr = (*pfn)(&dvi);
		if (SUCCEEDED(hr))
		{
			ASSERT(dvi.dwMajorVersion <= 0xFFFF);
			ASSERT(dvi.dwMinorVersion <= 0xFFFF);
			dwVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
		}
	}
	_afxComCtlVersion = dwVersion;
	return dwVersion;
}

int _afxDropDownWidth = -1;

int AFXAPI _AfxGetDropDownWidth()
{
	// return cached version if already determined...
	if (_afxDropDownWidth != -1)
		return _afxDropDownWidth;

	// otherwise calculate it...
	HDC hDC = GetDC(NULL);
	ASSERT(hDC != NULL);
	HFONT hFont = NULL, hFontOld = NULL;
	if ((hFont = CreateFont(GetSystemMetrics(SM_CYMENUCHECK), 0, 0, 0,
		FW_NORMAL, 0, 0, 0, SYMBOL_CHARSET, 0, 0, 0, 0, _T("Marlett"))) != NULL)
		hFontOld = (HFONT)SelectObject(hDC, hFont);
	VERIFY(GetCharWidth(hDC, '6', '6', &_afxDropDownWidth));
	if (hFont != NULL)
	{
		hFont = (HFONT)SelectObject(hDC, hFontOld);
		DeleteObject(hFont);
	}
	ReleaseDC(NULL, hDC);
	ASSERT(_afxDropDownWidth != -1);
	return _afxDropDownWidth;
}

/////////////////////////////////////////////////////////////////////////////
// CClientToolBar

CClientToolBar::CClientToolBar( IDMUSProdToolBar* pIToolBar )
{
	ASSERT( pIToolBar != NULL );

	m_pIToolBar = pIToolBar;
	m_pIToolBar->AddRef();

	m_sizeDefaultButton = m_sizeButton;

	m_hInstance = AfxGetInstanceHandle();	// default instance

	_AfxGetComCtlVersion();

	_AfxGetDropDownWidth();
}

CClientToolBar::~CClientToolBar()
{
	if( m_pIToolBar )
	{
		m_pIToolBar->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CClientToolBar overrides

/////////////////////////////////////////////////////////////////////////////
// CClientToolBar::OnUpdateCmdUI

void CClientToolBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CToolBar::OnUpdateCmdUI( pTarget, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CClientToolBar::OnToolHitTest

int CClientToolBar::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
    ASSERT_VALID( this );
    ASSERT( ::IsWindow(m_hWnd) );

    // Check child windows first by calling CControlBar
	CRect rect;

    int nControlID = CControlBar::OnToolHitTest( point, pTI );
    if( nControlID != -1 )
	{
		// Cursor over control in toolbar
		CWnd* pWnd = GetDlgItem( nControlID );
		if( pWnd )
		{
			CWnd* pWndParent = pWnd->GetParent();
			pWnd->GetClientRect( &rect );
			pWnd->ClientToScreen( &rect );
			pWndParent->ScreenToClient( &rect );
			if( pTI != NULL && pTI->cbSize >= 40 )	// 40 = sizeof(AfxOldTOOLINFO)
			{
				pTI->uFlags |= TTF_IDISHWND; 
				pTI->uId = (UINT)pWnd->m_hWnd;
			}
		}
		else
		{
			nControlID = -1;
		}
    }
	else
	{
		// Now hit test against CToolBar buttons
		CClientToolBar* pBar = (CClientToolBar*)this;
		int nButtons = (int)pBar->DefWindowProc( TB_BUTTONCOUNT, 0, 0 );

		for( int i = 0; i < nButtons; i++ )
		{
			TBBUTTON button;

			if( pBar->DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rect)
			&&  rect.PtInRect(point)
			&&  pBar->DefWindowProc(TB_GETBUTTON, i, (LPARAM)&button)
			&& !(button.fsStyle & TBSTYLE_SEP) )
			{
				nControlID = GetItemID( i );
				break;
			}
        }
	}

	if( nControlID > 0 )
	{
		if( pTI != NULL && pTI->cbSize >= 40 )	// 40 = sizeof(AfxOldTOOLINFO)
        {
			TCHAR szFullText[256];
			CString strTipText;

            pTI->hwnd = m_hWnd;
            pTI->rect = rect;
            if( !(pTI->uFlags & TTF_IDISHWND) )
			{
				pTI->uId = nControlID;
			}
			pTI->uFlags |= TTF_ALWAYSTIP;
			pTI->hinst = NULL;
			::LoadString( m_hInstance, nControlID, szFullText, sizeof(szFullText) / sizeof(TCHAR) );
			AfxExtractSubString( strTipText, szFullText, 1, '\n' );
		    _tcsncpy( szFullText, strTipText, sizeof(szFullText) / sizeof(TCHAR ));
			pTI->lpszText = _tcsdup( szFullText );
        }

		return nControlID;
	}
	else
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );

		CString strText;
		
		strText.LoadString( AFX_IDS_IDLEMESSAGE );
		pMainFrame->m_wndStatusBar.SetWindowText( strText );
	}

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CClientToolBar::SetStatusText

BOOL CClientToolBar::SetStatusText( int nControlID )
{
	if( CToolBar::SetStatusText(nControlID) == TRUE )
	{
		if( nControlID != -1 )
		{
			TCHAR szFullText[256];
			CString strHelpText;

			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			ASSERT( pMainFrame != NULL );

			::LoadString( m_hInstance, nControlID, szFullText, sizeof(szFullText) / sizeof(TCHAR) );
			AfxExtractSubString( strHelpText, szFullText, 0, '\n' );
			pMainFrame->m_wndStatusBar.SetWindowText( strHelpText );
		}

		return TRUE;
	}

	return FALSE;
}


BEGIN_MESSAGE_MAP(CClientToolBar, CToolBar)
	//{{AFX_MSG_MAP(CClientToolBar)
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CClientToolBar message handlers

CSize CClientToolBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	// Hack to make the Output status toolbar resize work
	// BEGIN CHANGED MFC CODE
	if( nLength == -1 && dwMode == 0x06 )
	{
		dwMode |= LM_COMMIT;
	}
	// END CHANGED MFC CODE

	if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
	{
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
	}
	return CalcLayout(dwMode, nLength);
}

CSize CClientToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout(dwMode);
}

#define CX_OVERLAP  0

CSize CClientToolBar::CalcSize(TBBUTTON* pData, int nCount)
{
	ASSERT(pData != NULL && nCount > 0);

	CPoint cur(0,0);
	CSize sizeResult(0,0);

	DWORD dwExtendedStyle = DefWindowProc(TB_GETEXTENDEDSTYLE, 0, 0);

	for (int i = 0; i < nCount; i++)
	{
		//WINBUG: The IE4 version of COMCTL32.DLL calculates the separation
		//  on a TBSTYLE_WRAP button as 100% of the value in iBitmap compared
		//  to the other versions which calculate it at 2/3 of that value.
		//  This is actually a bug which should be fixed in IE 4.01, so we
		//  only do the 100% calculation specifically for IE4.
		int cySep = pData[i].iBitmap;
		ASSERT(_afxComCtlVersion != -1);
		if (!(GetStyle() & TBSTYLE_FLAT) && _afxComCtlVersion != VERSION_IE4)
			cySep = cySep * 2 / 3;

		if (pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		int cx = m_sizeButton.cx;
		if (pData[i].fsStyle & TBSTYLE_SEP)
		{
			// a separator represents either a height or width
			if (pData[i].fsState & TBSTATE_WRAP)
				sizeResult.cy = max(cur.y + m_sizeButton.cy + cySep, sizeResult.cy);
			else
			{
				sizeResult.cx = max(cur.x + pData[i].iBitmap, sizeResult.cx);
				// BEGIN CHANGED MFC CODE
				// a separator that has a command ID is not
				// a separator, but a custom control.
				// So, ensure it also updates the current height
				if( pData[i].idCommand != 0 )
				{
					sizeResult.cy = max(cur.y + m_sizeButton.cy, sizeResult.cy);
				}
				// END CHANGED MFC CODE
			}
		}
		else
		{
			// check for dropdown style, but only if the buttons are being drawn
			if ((pData[i].fsStyle & TBSTYLE_DROPDOWN) &&
				(dwExtendedStyle & TBSTYLE_EX_DRAWDDARROWS))
			{
				// add size of drop down
				ASSERT(_afxDropDownWidth != -1);
				cx += _afxDropDownWidth;
			}
			/*
			// BEGIN CHANGED MFC CODE
			else
			{
				TBBUTTONINFO tbButtonInfo;
				tbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
				tbButtonInfo.dwMask = TBIF_SIZE;
				if( DefWindowProc( TB_GETBUTTONINFO, (WPARAM)(INT)pData[i].idCommand, (LPARAM)(LPTBBUTTONINFO)&tbButtonInfo ) != -1 )
				{
					cx = max( cx, tbButtonInfo.cx );
				}
			}
			// END CHANGED MFC CODE
			*/
			sizeResult.cx = max(cur.x + cx, sizeResult.cx);
			sizeResult.cy = max(cur.y + m_sizeButton.cy, sizeResult.cy);
		}

		if (pData[i].fsStyle & TBSTYLE_SEP)
			cur.x += pData[i].iBitmap;
		else
			cur.x += cx - CX_OVERLAP;

		if (pData[i].fsState & TBSTATE_WRAP)
		{
			cur.x = 0;
			cur.y += m_sizeButton.cy;
			if (pData[i].fsStyle & TBSTYLE_SEP)
				cur.y += cySep;
		}
	}
	return sizeResult;
}

int CClientToolBar::WrapToolBar(TBBUTTON* pData, int nCount, int nWidth)
{
	ASSERT(pData != NULL && nCount > 0);

	int nResult = 0;
	int x = 0;
	for (int i = 0; i < nCount; i++)
	{
		pData[i].fsState &= ~TBSTATE_WRAP;

		if (pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		int dx, dxNext;
		if (pData[i].fsStyle & TBSTYLE_SEP)
		{
			dx = pData[i].iBitmap;
			dxNext = dx;
		}
		else
		{
			dx = m_sizeButton.cx;
			dxNext = dx - CX_OVERLAP;
		}

		if (x + dx > nWidth)
		{
			BOOL bFound = FALSE;
			for (int j = i; j >= 0  &&  !(pData[j].fsState & TBSTATE_WRAP); j--)
			{
				// Find last separator that isn't hidden
				// a separator that has a command ID is not
				// a separator, but a custom control.
				if ((pData[j].fsStyle & TBSTYLE_SEP) &&
					(pData[j].idCommand == 0) &&
					!(pData[j].fsState & TBSTATE_HIDDEN))
				{
					bFound = TRUE; i = j; x = 0;
					pData[j].fsState |= TBSTATE_WRAP;
					nResult++;
					break;
				}
			}
			if (!bFound)
			{
				for (int j = i - 1; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
				{
					// Never wrap anything that is hidden,
					// or any custom controls
					if ((pData[j].fsState & TBSTATE_HIDDEN) ||
						((pData[j].fsStyle & TBSTYLE_SEP) &&
						(pData[j].idCommand != 0)))
						continue;

					bFound = TRUE; i = j; x = 0;
					pData[j].fsState |= TBSTATE_WRAP;
					nResult++;
					break;
				}
				if (!bFound)
					x += dxNext;
			}
		}
		else
			x += dxNext;
	}
	return nResult + 1;
}

void  CClientToolBar::SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert)
{
	ASSERT(pData != NULL && nCount > 0);

	if (!bVert)
	{
		int nMin, nMax, nTarget, nCurrent, nMid;

		// Wrap ToolBar as specified
		nMax = nLength;
		nTarget = WrapToolBar(pData, nCount, nMax);

		// Wrap ToolBar vertically
		nMin = 0;
		nCurrent = WrapToolBar(pData, nCount, nMin);

		if (nCurrent != nTarget)
		{
			while (nMin < nMax)
			{
				nMid = (nMin + nMax) / 2;
				nCurrent = WrapToolBar(pData, nCount, nMid);

				if (nCurrent == nTarget)
					nMax = nMid;
				else
				{
					if (nMin == nMid)
					{
						WrapToolBar(pData, nCount, nMax);
						break;
					}
					nMin = nMid;
				}
			}
		}
		CSize size = CalcSize(pData, nCount);
		WrapToolBar(pData, nCount, size.cx);
	}
	else
	{
		CSize sizeMax, sizeMin, sizeMid;

		// Wrap ToolBar vertically
		WrapToolBar(pData, nCount, 0);
		sizeMin = CalcSize(pData, nCount);

		// Wrap ToolBar horizontally
		WrapToolBar(pData, nCount, 32767);
		sizeMax = CalcSize(pData, nCount);

		while (sizeMin.cx < sizeMax.cx)
		{
			sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
			WrapToolBar(pData, nCount, sizeMid.cx);
			sizeMid = CalcSize(pData, nCount);

			if (nLength < sizeMid.cy)
			{
				if (sizeMin == sizeMid)
				{
					WrapToolBar(pData, nCount, sizeMax.cx);
					return;
				}
				sizeMin = sizeMid;
			}
			else if (nLength > sizeMid.cy)
			{
				if (sizeMax == sizeMid)
				{
					WrapToolBar(pData, nCount, sizeMin.cx);
					return;
				}
				sizeMax = sizeMid;
			}
			else
				return;
		}
	}
}

struct _AFX_CONTROLPOS
{
	int nIndex, nID;
	CRect rectOldPos;
};

CSize CClientToolBar::CalcLayout(DWORD dwMode, int nLength)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	if (dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);

	int nCount;
	TBBUTTON* pData = NULL;
	CSize sizeResult(0,0);

	//BLOCK: Load Buttons
	{
		nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
		if (nCount != 0)
		{
			int i;
			pData = new TBBUTTON[nCount];
			for (i = 0; i < nCount; i++)
				_GetButton(i, &pData[i]);
		}
	}

	if (nCount > 0)
	{
		if (!(m_dwStyle & CBRS_SIZE_FIXED))
		{
			BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

			if (bDynamic && (dwMode & LM_MRUWIDTH))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else if (bDynamic && (dwMode & LM_HORZDOCK))
				SizeToolBar(pData, nCount, 32767);
			else if (bDynamic && (dwMode & LM_VERTDOCK))
				SizeToolBar(pData, nCount, 0);
			else if (bDynamic && (nLength != -1))
			{
				CRect rect; rect.SetRectEmpty();
				CalcInsideRect(rect, (dwMode & LM_HORZ));
				BOOL bVert = (dwMode & LM_LENGTHY);
				int nLen = nLength + (bVert ? rect.Height() : rect.Width());

				SizeToolBar(pData, nCount, nLen, bVert);
			}
			else if (bDynamic && (m_dwStyle & CBRS_FLOATING))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else
				SizeToolBar(pData, nCount, (dwMode & LM_HORZ) ? 32767 : 0);
		}

		sizeResult = CalcSize(pData, nCount);

		if (dwMode & LM_COMMIT)
		{
			_AFX_CONTROLPOS* pControl = NULL;
			int nControlCount = 0;
			BOOL bIsDelayed = m_bDelayedButtonLayout;
			m_bDelayedButtonLayout = FALSE;

			for (int i = 0; i < nCount; i++)
				if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
					nControlCount++;

			if (nControlCount > 0)
			{
				pControl = new _AFX_CONTROLPOS[nControlCount];
				nControlCount = 0;

				for(int i = 0; i < nCount; i++)
				{
					if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
					{
						pControl[nControlCount].nIndex = i;
						pControl[nControlCount].nID = pData[i].idCommand;

						CRect rect;
						GetItemRect(i, &rect);
						ClientToScreen(&rect);
						pControl[nControlCount].rectOldPos = rect;

						nControlCount++;
					}
				}
			}

			if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
				m_nMRUWidth = sizeResult.cx;
			for (i = 0; i < nCount; i++)
				_SetButton(i, &pData[i]);

			if (nControlCount > 0)
			{
				for (int i = 0; i < nControlCount; i++)
				{
					CWnd* pWnd = GetDlgItem(pControl[i].nID);
					if (pWnd != NULL)
					{
						CRect rect;
						pWnd->GetWindowRect(&rect);
						CPoint pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
						GetItemRect(pControl[i].nIndex, &rect);
						pt = rect.TopLeft() + pt;
						pWnd->SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
					}
				}
				delete[] pControl;
			}
			m_bDelayedButtonLayout = bIsDelayed;
		}
		delete[] pData;
	}

	//BLOCK: Adjust Margins
	{
		CRect rect; rect.SetRectEmpty();
		CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
		sizeResult.cx -= rect.Width();

		CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);
	}
	return sizeResult;
}

void CClientToolBar::_GetButton(int nIndex, TBBUTTON* pButton)
{
	/* BEGIN CHANGED MFC CODE
	CToolBar* pBar = (CToolBar*)this;
	VERIFY(pBar->DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)pButton));
	*/
	VERIFY(DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)pButton));
	// END CHANGED MFC CODE
	pButton->fsState ^= TBSTATE_ENABLED;
}

void CClientToolBar::_SetButton(int nIndex, TBBUTTON* pButton)
{
	// get original button state
	TBBUTTON button;
	VERIFY(DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)&button));

	// prepare for old/new button comparsion
	button.bReserved[0] = 0;
	button.bReserved[1] = 0;
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
	pButton->bReserved[0] = 0;
	pButton->bReserved[1] = 0;

	// nothing to do if they are the same
	if (memcmp(pButton, &button, sizeof(TBBUTTON)) != 0)
	{
		// don't redraw everything while setting the button
		DWORD dwStyle = GetStyle();
		ModifyStyle(WS_VISIBLE, 0);
		VERIFY(DefWindowProc(TB_DELETEBUTTON, nIndex, 0));

		// BEGIN CHANGED MFC CODE
		/* This causes drawing bugs for the edit controls in the toolbars
		// Force a recalc of the toolbar's layout to work around a comctl bug
		int iTextRows;
		iTextRows = (int)::SendMessage(m_hWnd, TB_GETTEXTROWS, 0, 0);
		::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
		::SendMessage(m_hWnd, TB_SETMAXTEXTROWS, iTextRows+1, 0);
		::SendMessage(m_hWnd, TB_SETMAXTEXTROWS, iTextRows, 0);
		::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
		*/
		// END CHANGED MFC CODE

		VERIFY(DefWindowProc(TB_INSERTBUTTON, nIndex, (LPARAM)pButton));
		ModifyStyle(0, dwStyle & WS_VISIBLE);

		// invalidate appropriate parts
		if (((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
			((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
		{
			// changing a separator
			Invalidate();
		}
		else
		{
			// invalidate just the button
			CRect rect;
			if (DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)&rect))
				InvalidateRect(rect);
		}
	}
}

BOOL CClientToolBar::OnEraseBkgnd(CDC* pDC) 
{
	if (::IsWindow(m_hWnd)
	&&	m_sizeButton != m_sizeDefaultButton )
	{
		// set the sizes via TB_SETBITMAPSIZE and TB_SETBUTTONSIZE
		VERIFY(::SendMessage(m_hWnd, TB_SETBUTTONSIZE, 0, MAKELONG(m_sizeButton.cx, m_sizeButton.cy)));

		// Uncommenting this causes an infinite loop
		//Invalidate();   // just to be nice if called when toolbar is visible
	}
	
	return CToolBar::OnEraseBkgnd(pDC);
}

void CClientToolBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CToolBar::OnWindowPosChanged(lpwndpos);

	if( m_pIToolBar
	&&	(lpwndpos->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)) )
	{
		IDMUSProdToolBar8 *pToolBar8;
		if( SUCCEEDED( m_pIToolBar->QueryInterface( IID_IDMUSProdToolBar8, (void **)&pToolBar8 ) ) )
		{
			if( lpwndpos->flags & SWP_SHOWWINDOW )
			{
				pToolBar8->ShowToolBar( TRUE );
			}
			else // if ( lpwndpos->flags & SWP_HIDEWINDOW )
			{
				pToolBar8->ShowToolBar( FALSE );
			}
			pToolBar8->Release();
		}
	}
}
