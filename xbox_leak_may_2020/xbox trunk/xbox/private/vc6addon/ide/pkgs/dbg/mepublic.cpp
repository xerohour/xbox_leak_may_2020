#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define MAXEDITMGRBUF		256	/* max size of edit mgr buffer */

WORD    wParamMouse;
DWORD   lParamMouse;

char		editbuf[MAXEDITMGRBUF];	/* editmgr line buf */
char		scrchbuf[MAXEDITMGRBUF];/* editmgr scratch buf */
LD ldEMScratch = {		/* editmgr scrach data area */
    0, 0, MAXEDITMGRBUF-1, scrchbuf
};
LD ldEMMultiEdit = {	/* editmgr default scrap */
	0, 0, MAXEDITMGRBUF-1, editbuf
};

IMPLEMENT_DYNCREATE(CMultiEdit, CDockablePartView)

BEGIN_MESSAGE_MAP(CMultiEdit, CDockablePartView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   	ON_COMMAND(IDM_GOTO_LINE, OnGotoLine)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo) 
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo) 
   	ON_UPDATE_COMMAND_UI(IDM_GOTO_LINE, OnUpdateGotoLine)
END_MESSAGE_MAP()

CMultiEdit :: CMultiEdit()
{
	m_fUseMultiEdit = FALSE;	// Will be reset when WU_INITDEBUG is received
	m_pDragInfo     = NULL;
	InitEditMgr();
}

BOOL CMultiEdit :: PreTranslateMessage( MSG * pMsg )
{
	BOOL	fRet = FALSE;

	switch (pMsg->message)
    {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:

		if (CDataTip::GetTip()) CDataTip::GetTip()->DestroyTip();
	}

	if ( pMsg->message == WM_KEYDOWN ) {
		switch ( pMsg->wParam ) {

			// Add keys to this table to bypass global accelerator table!

		/*
			case VK_ESCAPE:
				// If a window needs to get the ESCAPE key, then it should
				// override this PreTranslateMessage and check for a VK_ESCAPE
				// without the CONTROL or SHIFT keys down.  When this condition
				// is met, 'this' should do any checks for "dirtyness" of a
				// buffer and process the event immediately!  Otherwise,
				// the dockmanager will get the key event and do move the
				// active window from the dockable window to the active MDI!
				//
				// MarkBro 11/18/93
				//
		*/

			case VK_TAB:
				// MDI gets Ctrl+Tab.
				if ((GetKeyState(VK_CONTROL) & ~1) != 0) {
					break;
				}
				// fall through

			// Fall through.
			case VK_BACK:
			case VK_DELETE:
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
			case VK_END:
			case VK_HOME:
			case VK_NEXT:
			case VK_PRIOR:
			case VK_RETURN:
				TranslateMessage( pMsg );
				DispatchMessage( pMsg );
				fRet = TRUE;
				break;

			case VK_F8:
				if (
					(GetKeyState(VK_CONTROL) & ~1) != 0 &&
					(GetKeyState(VK_SHIFT) & ~1) != 0
				) {
					m_fColumnSelect = TRUE;
					fRet = TRUE;
				}
				break;
				
			default:
				break;
		}
	}

	return( fRet || CPartView::PreTranslateMessage(pMsg) );
}

LRESULT CMultiEdit :: WindowProc(UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	BOOL 	fCallEditProc = TRUE;
	BOOL	fCallBase = FALSE;
	LRESULT	lResult = FALSE;

	// Filter window docking messages
	if (!IsPopupMessage(wMsg) &&
		DkPreHandleMessage(m_hWnd, wMsg, wParam, lParam, &lResult))
	{
		return lResult;
	}

	switch( wMsg ) {
        case WM_SETFOCUS:
            OnViewActivate( this, TRUE );
            break;

		case WM_DESTROY:
        {
			m_fUseMultiEdit = FALSE;

        	switch( m_dt )
            {
        		case CPU_WIN:
        			pViewCpu = NULL;
        			break;
        		case PHINWATCH_WIN:
        			// REVIEW : If we get a WM_VIEWACTIVATE when the
        			// view is being destroyed this is not neccessary.
        			pViewNewWatch = NULL;
        			break;
        		case PHINLOCALS_WIN:
        			pViewNewLocals = NULL;
        			break;
        		case DISASSY_WIN:
        			pViewDisassy = NULL;
        			break;
        		case MEMORY_WIN:
        			pViewMemory = NULL;
        			break;
        		case CALLS_WIN:
        			pViewCalls = NULL;
        			break;
        		default:
        			ASSERT(FALSE);
        			break;
        	}

            break;
        }

		case WU_CLEARDEBUG:
			// Make sure that the LTextProc gets this message
			LTextProc( wMsg, wParam, lParam );
			m_fUseMultiEdit = FALSE;

		case WU_UPDATEDEBUG:
			// When these messages occur, make sure that the window gets
			// completely updated and not just the update region.
			InvalidateRect( (LPRECT)NULL, (BOOL)( wMsg == WU_UPDATEDEBUG ) );
			break;

		case WU_INITDEBUGWIN:
			m_fUseMultiEdit = TRUE;
			LTextProc( wMsg, wParam, lParam );
			SetScrollRange ( SB_HORZ, 0, 0, TRUE );
//SetScrollRange ( SB_VERT, 0, 0, TRUE );

			fCallEditProc = FALSE;
            break;

        case WM_CHAR:
        case WM_KEYDOWN: {
			// Simulate COW key events by adding the shift and control
			// key states in the message parameters.

            WORD    wl = LOWORD( lParam );
            WORD    wh = HIWORD( lParam ) & 0x3fff;

            if ( GetKeyState( VK_SHIFT ) & 0x8000 ) {
                wh |= KK_SHIFT;
            }
            if ( GetKeyState( VK_CONTROL ) & 0x8000 ) {
                wh |= KK_CONTROL;
            }
            lParam = MAKELONG( wl, wh );

			if ( wMsg == WM_CHAR ) {
				// If we haven't received a lead byte,
				// accept one and set the status
				if ( m_fAcceptDbcs ) {
					if ( _ismbblead( LOWORD( wParam ) ) ) {
						m_fAcceptDbcs = FALSE;
						m_bDbcsLead = LOBYTE( LOWORD( wParam ) );

						// Just eat the message since we will
						// pass it on when the second byte is
						// received
						fCallEditProc = fCallBase = FALSE;
					}
				}

				// We have already saved a lead byte and this MUST be
				// the trail byte.  Mask off the high byte of the loword
				// and put in the lead byte.  When the textproc receives
				// the wParam, it will be a complete DBCS character.
				else {
        		    wl = ( LOWORD( wParam ) & 0xff ) | ( m_bDbcsLead << 8 );
    	        	wh = HIWORD( wParam );
	            	wParam = MAKELONG( wl, wh );

					// re-enable accepting of DBCS lead bytes
					m_fAcceptDbcs = TRUE;
				}
			}
            break;
        }

		case WM_FONTCHANGE:
			// We want this message to get through to the TextView
			// wndproc AND then pass it on to the individual window
			break;

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
            {
				// Mouse messages need to be translated into character coords
				// for the individual wndprocs.  This way it's easy to identify
				// what the textual location of the cursor is.  wParamMouse and
				// lParamMouse are used in the MultiEdit control since it needs
				// the real Windows mouse coordinates.

	            RX    obMouse;
	            RY    olnMouse;

	            obMouse = (SHORT)( ( LOWORD( lParam ) - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth );
	            olnMouse = (SHORT)( HIWORD( lParam ) / m_pFontInfo->m_tm.tmHeight );

	            wParamMouse = wParam;
	            lParamMouse = lParam;

				lParam = MAKELONG( obMouse, olnMouse );
			}
            break;

        case WM_RBUTTONDOWN:
        	//BLOCK:
        	{
	        	if (!FMouseHitSel(FALSE, lParam))
	        	{
	        		// Hacky way of resetting the cursor.
	        		SendMessage(WM_LBUTTONDOWN, 0, lParam);
	        		SendMessage(WM_LBUTTONUP, 0, lParam);
					UpdateWindow();	// So we don't blow save bits.
	        	}
			}
        	break;

        case WM_RBUTTONUP:
			{
        		CPoint point(lParam);
				ClientToScreen(&point);
				ShowContextPopupMenu(point);
			}
			break;

		case WM_COMMAND:
			if ( wParam ) {
				// All command events should have received
				// a WM_UNDO... message, so ignore the
				// WM_COMMAND stuff!
				fCallEditProc = FALSE;
			}
			break;

        default:
	    	// Copied from qcqp.c ChildWndProc.  Without this, the
	    	// set font and other window commands won't work
			if ( wMsg == WM_VIEWACTIVATE )
				OnViewActivate( this, (BOOL)wParam );

            break;
    }

	if ( fCallBase || !m_fUseMultiEdit ) {
		switch( wMsg )
		{
		case WM_CHAR:
			MessageBeep( 0 );
			// FALL THROUGH //

		case WM_SETFOCUS:
			break;

    	default:
			lResult = CWnd::WindowProc(wMsg, wParam, lParam);
		}
	}
	else if ( fCallEditProc ) {
		lResult = LTextProc( wMsg, wParam, lParam );

		// In order for the debug windows to keep up with the
		// potential rapid fire F10/F8s, force the window to
		// receive it's updates immediately rather than waiting
		// for the input queue to be empty and then having windows
		// send the WM_PAINT on the invalid region
		if ( wMsg == WU_UPDATEDEBUG ) {
			UpdateWindow();
		}
	}

	return lResult;
}

/****************************************************************************

	FUNCTION:   GetCurrentText

	PURPOSE:	
		What is does:
				If there's a selection, return the first line of the selected
				text.  If there's no selection, get the word at the current
				cursor location.

		What it's supposed to do:
			    Retrieve in view the word at cursor, or the first line
				of text if selection is active. If not NULL, beginOffset
				and endOffset return number of chars relative to cursor
				position. If a selection is active and is over several
				lines and cursor in selection is not above a word and
				lookAround is TRUE, the word at left is returned and
				'lookAround' is TRUE, otherwise lookAround is FALSE.

	RETURNS:    FALSE if there is a problem or nothing to get

****************************************************************************/
BOOL CMultiEdit :: GetCurrentText(
	BOOL *lookAround,
	LPSTR pText,
	_CB maxSize,
	ICH * pxLeft,
	ICH * pxRight)
{
	BOOL	fRet = FALSE;

	if ( SendMessage( EM_GETTEXT, (WPARAM)maxSize, (LPARAM)pText ) ) {
		char FAR *	lpch = _ftcschr( pText, _T('\r') );

		if ( lpch ) {
			*lpch = '\0';
		}
		*lookAround = fRet = TRUE;
	}
	else {
		fRet = (BOOL)SendMessage( EM_GETWORD, (WPARAM)maxSize, (LPARAM)pText );
		*lookAround = FALSE;
	}

	// The browser looks at xLeft to NOT be -1 if we've succeeded!
	if ( fRet && pxLeft ) {
		*pxLeft = 0;
	}

	return fRet;
}

void CMultiEdit :: OnFindNext( BOOL * pfSuccess ) {
	MessageBeep(0);
	if ( pfSuccess ) {
		*pfSuccess = FALSE;
	}
}

#ifdef DRAG_DROP
BOOL CMultiEdit::GetDragSourceText(CDragObject& dragObject, HANDLE& hData, long& cchData)
{
	ASSERT(dragObject.InTextView());
	ASSERT(this == dragObject.m_pViewDragFrom);

	if ( hData = (HANDLE)SendMessage( WM_COPY, (WPARAM)0, (LPARAM)-1L ) )
	{
		TCHAR FAR *	lptch;
		
		if ( lptch = (TCHAR FAR *)RealGlobalLock( hData ) )
		{
			cchData = _tcslen( lptch );
			RealGlobalUnlock( hData );
		}
	}
	return (hData != NULL);
}

VOID CMultiEdit::AssimilateDroppedData(CDragObject&, LPSTR, long, POINT)
{
	// If the view supports a drop this is a required override.
	ASSERT(FALSE);
}
#endif // DRAG_DROP

void CMultiEdit :: PosFromPoint( int & X, int & Y, CPoint pt ) {
	if ( pt.x < m_cxSelectMargin ) {
		pt.x = m_cxSelectMargin;
	}

	int tmAveCharWidth = m_pFontInfo->m_tm.tmAveCharWidth;
	X = ( ( pt.x - m_cxSelectMargin + tmAveCharWidth / 2 - 1 ) / tmAveCharWidth )
		* tmAveCharWidth + m_cxSelectMargin;

	int tmHeight = m_pFontInfo->m_tm.tmHeight;
	Y = (pt.y/tmHeight) * tmHeight;
}

#ifdef DRAG_DROP
void CMultiEdit :: SetCaretPosition(RTXVPT txvpt, CTextClip::ScrollActions ScrollAction)
{
	POINT	pt;

	pt.x = txvpt.Ich();
	pt.y = txvpt.Iline();

    SetCaretPos( pt );
}
#endif // DRAG_DROP

BOOL CMultiEdit :: InitEditMgr()
{

	//
	// Set up edit field and associated structures.
	//

	memset( &m_pdCur, 0, sizeof( m_pdCur ) );
	memset( &m_ipCur, 0, sizeof( m_ipCur ) );
	memset( &m_ipAnchor, 0, sizeof( m_ipAnchor ) );

	m_dScrollVal        = 0;
	m_olnMacPrev        = 0;
	m_fScrollV          = TRUE;
	m_fScrollH          = FALSE;
	m_pdCur.oln         = 0xffffffff;
	m_pldCur            = &ldEMMultiEdit;
	m_Style             = ES_MULTILINE;
	m_fSelection        = FALSE;
	m_iAdjustDbcs       = 1;
	m_ipCurOld.oln      = 0xffffffff;
	m_ipCurOld.ob       = 0xffff;
	m_ipAnchorOld.oln   = 0xffffffff;
	m_ipAnchorOld.ob    = 0xffff;
	m_fAcceptDbcs       = TRUE;
	m_fColumnSelect     = FALSE;
	m_pDragInfo         = NULL;

	// undo-redo related.
	m_mePlayCount = REC_CANNOTUNDO;
	m_undoState = usNormal;

	if ( FCanUseMarginSelect() && gpISrc->GetSrcEnvironParam(ENV_ENABLE_MARGIN_GLYPHS) )
		m_cxSelectMargin = gpISrc->GetSrcEnvironParam(ENV_CX_DEFAULT_SELECT_MARGIN);
	else
		m_cxSelectMargin = 0;

	// No longer do allocations, always return true!
	return( TRUE );
}

void CMultiEdit :: DrawWindow() {
	InvalidateRect( NULL, FALSE );
	UpdateWindow();
}

/* MULTIEDIT DEFAULT VIRTUAL FUNCTIONS */
BOOL CMultiEdit :: FReplaceLineBuf( DWORD iln, WORD cb, char * psz ) {
	// Default is no replacing allowed
	return FALSE;
}

BOOL CMultiEdit :: FReadOnlyBuf() {
	// Default is READ_ONLY
	return TRUE;
}

BOOL CMultiEdit :: FIsWordChar( TCHAR ch ) {
    return( ch && ( _istalnum((unsigned char)ch) || _ftcschr(rglan[ESilan()].lszIsIdChar,(_TUCHAR)ch) ) );
}

WORD CMultiEdit :: ObMaxBuf() {
	// By default, MultiEdit docs are axMax chars wide
	return (WORD)axMax;
}

void CMultiEdit :: GetClientRrc( PRRC prrc ) {
	RECT	rect;

	GetClientRect( &rect );

	// The right and bottom may be less than the
	// left and top if the scroll bars are wider than
	// the window is wide/high.  If so, adjust so the
	// values are zero not negative
	if ( rect.right < rect.left ) {
		rect.right = rect.left;
	}
	if ( rect.bottom < rect.top ) {
		rect.bottom = rect.top;
	}

	// Compute RRC of client area.  Only get complete
	// characters (partially clipped, ignore)

    prrc->rxLeft = 0;
    prrc->rxRight = (RX)( ( rect.right - rect.left - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth );

    prrc->ryTop = 0;
    prrc->ryBottom = (RY)( ( rect.bottom - rect.top ) / m_pFontInfo->m_tm.tmHeight );
}

void CMultiEdit :: InvalidateRrc( PRRC prrc ) {
    RRC     rrc;
    RECT    rect;

    GetClientRrc( &rrc );

    if ( !memcmp( &rrc, prrc, sizeof( RRC ) ) ) {
        GetClientRect( &rect );
    }
    else {
		int tmAveCharWidth = m_pFontInfo->m_tm.tmAveCharWidth;
		int tmHeight = m_pFontInfo->m_tm.tmHeight;

        rect.left = prrc->rxLeft * tmAveCharWidth;
        rect.top = prrc->ryTop * tmHeight;
        rect.right = prrc->rxRight * tmAveCharWidth + m_cxSelectMargin;
        rect.bottom = prrc->ryBottom * tmHeight;
    }
    InvalidateRect( &rect, FALSE );
}

void CMultiEdit :: DrawLine( DWORD oln ) {
	SendMessage( WM_PAINTLINE, 0, oln );
}

DWORD CMultiEdit :: WinHeight( void ) {
	RECT	rect;

	GetClientRect( &rect );

	// if the window is small and the scroll bars take up
	// the remaining client area, it is possible that the
	// client area will be a negative value.  In that case
	// reset the rect so that the returned value is zero
	if ( rect.bottom < rect.top ) {
		rect.bottom = rect.top;
	}

	return (DWORD)( ( rect.bottom - rect.top ) / m_pFontInfo->m_tm.tmHeight );
}

WORD CMultiEdit :: WinWidth( void ) {
	RECT	rect;

	GetClientRect( &rect );

	// if the window is small and the scroll bars take up
	// the remaining client area, it is possible that the
	// client area will be a negative value.  In that case
	// reset the rect so that the returned value is zero
	if ( rect.right < rect.left ) {
		rect.right = rect.left;
	}

	if ( rect.right - rect.left < m_cxSelectMargin ) {
		rect.right = rect.left + m_cxSelectMargin;
	}

	return (WORD)( ( rect.right - rect.left - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth );
}

WORD CMultiEdit :: ObGetCurPos( void ) {
	return m_ipCur.ob;
}

void CMultiEdit :: SetCurObPos( WORD obNew ) {
	m_ipCur.ob = obNew;
}

DWORD CMultiEdit :: OlnGetCurPos( void ) {
	return m_ipCur.oln;
}

void CMultiEdit :: SetCurOlnPos( DWORD olnNew ) {
	m_ipCur.oln = olnNew;
}

DWORD CMultiEdit :: OlnGetTop( void ) {
	return m_pdCur.olntop;
}

WORD CMultiEdit :: ObGetLeft( void ) {
	return m_pdCur.obleft;
}

void CMultiEdit :: SetCursorAt( WORD ob, DWORD oln ) {
	EMSC	emsc;

	emsc.x1 = emsc.x2 = ob;
	emsc.y = oln;
	CMultiEdit::LTextProc( EM_SELCHARS, 0, (LPARAM)&emsc );
}

void CMultiEdit :: SetWinTopLineCursor(
	DWORD	olnTop,
	WORD	obCursor,
	DWORD	olnCursor
) {

	m_pdCur.olntop = olnTop;
	m_pdCur.oln = (DWORD)-1;
	InvalidateRect( NULL, FALSE );
	SetCursorAt( obCursor, olnCursor );
	UpdateWindow();
}

void CMultiEdit :: SetWinTopLine( DWORD oln ) {
	SetWinTopLineCursor( oln, m_ipCur.ob, oln );
}

void CMultiEdit :: SetWinUpperLeft( WORD ob, DWORD oln ) {
	SetWinTopLineCursor( oln, ob, oln );
}

DWORD CMultiEdit :: GetEditSelection( WORD cbMax, char FAR * lsz ) {
	return (DWORD)SendMessage( EM_GETTEXT, (WPARAM)cbMax, (LPARAM)lsz );
}

DWORD CMultiEdit :: GetEditWord( WORD cbMax, char FAR * lsz ) {
	return (DWORD)SendMessage( EM_GETWORD, (WPARAM)cbMax, (LPARAM)lsz );
}

WORD CMultiEdit :: FInsertMode() {
    return (WORD)fInsertMode;
}

DWORD CMultiEdit :: OlnFromScrollPos( int iScroll ) {
	DWORD	olnMac;
	DWORD	oln;

	if ( m_dScrollVal )
	{
		// 1-1
		oln = (DWORD)( iScroll );
	}
	// else, if the buffer isn't empty, scale
	else if (olnMac = CLinesInBuf())
	{
		oln = (DWORD)( (double)iScroll / (double)SHRT_MAX * (double)olnMac );
	}
	// else buffer's empty, return zero
	else
	{
		oln = 0L;
	}

	return oln;
}

/**** ScrollFromOln                                                     ****
 *                                                                         *
 *  PURPOSE: Given a line number convert to a mapped scroll index          *
 *                                                                         *
 *  INPUTS:                                                                *
 *       oln     Line number to convert                                    *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: index into scroll bar range or zero for failure      *
 *                                                                         *
 *  IMPLEMENTATION: This is a private function, but is in this file to     *
 *  keep it with OlnFromScrollPos                                          *
 *                                                                         *
 ***************************************************************************/
int CMultiEdit :: ScrollFromOln( DWORD	oln ) {
	int	iScroll;
	DWORD	olnMac;

	// If there's a multiplier, use it
	if ( m_dScrollVal )
	{
		iScroll = (int)(oln);
	}

	// else, if the buffer isn't empty calculate a mapped value
	else if ( olnMac = CLinesInBuf() )
	{
		iScroll = (int)( (double)oln / (double)olnMac * (double)SHRT_MAX );
	}

	// else buffer's empty, return zero
	else
	{
		iScroll = 0;
	}

	return iScroll;
}


/**** FlipBytes                                                         ****
 *                                                                         *
 *  PURPOSE: Reverse byte order (toggle big/little endian)                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *       lpb     pointer to byte stream to be reversed                     *
 *       cb      number of bytes in buffer                                 *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *       lpb     byte stream reversed                                      *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: FlipBytes( BYTE FAR * lpb, int cb ) {
	if( pDebugCurr && pDebugCurr->MtrcEndian() == endBig ) {
		BYTE	bT;

		while( cb > 1 ) {
			--cb;
			bT = *lpb;
			*lpb = *( lpb + cb );
			*( lpb + cb ) = bT;

			++lpb;
			--cb;
		}
	}
}

BOOL CMultiEdit::Create(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// m_nID = nID;
	
	DWORD dwStyleEx = 0;

	if (UseWin4Look() && IsKindOf(RUNTIME_CLASS(CDockablePartView)))
	{
		dwStyleEx |= WS_EX_CLIENTEDGE;
		dwStyle &= ~WS_BORDER;
	}

//	if (pContext != NULL)
//		g_pTCreateDoc = (CTextDoc *)(pContext->m_pCurrentDoc);

	if ( !CView::CreateEx(dwStyleEx, AfxRegisterWndClass(CS_DBLCLKS, theApp.LoadCursor(IDC_IBEAM)),
		lpszWindowName,	dwStyle | WS_VISIBLE,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID, pContext) )
    {                                   
		return FALSE;
	}

	m_DropTarget.Register(this);

	return TRUE;
}

void CMultiEdit::OnDraw( CDC *)
{
    // REVIEW: Some paint messages sneak through after we get the WU_CLEARDEBUG
    // We don't need to paint anything since
    // 1) The window is going away.
    // 2) we are going to restart and repaint on WU_INITDEBUGWIN. 
    //  so I am relaxing this assert from FALSE to !m_fUseMultiEdit.
    // However we need to figure out when this is happening and 
    // fix CMultiEdit::WindowProc appropriately.

    ASSERT(!m_fUseMultiEdit);
}

void CMultiEdit::DrawMarginGlyphs(HDC hDC, RECT& rcMargin, UINT uStatus)
{
	FMT_ELEMENT *rgfmtel;

	if ( gpISrc->GetSrcEnvironParam(ENV_ENABLE_MARGIN_GLYPHS) )
	{

		int cxMargin    = rcMargin.right - rcMargin.left;
		int cyMargin    = rcMargin.bottom - rcMargin.top;
		int nMarginSize = min(cyMargin, cxMargin);
		int nGlyphSize  = (nMarginSize - 3) | 1; // at least 2 less and odd

#ifdef CODECOVERAGE
		if (uStatus & COVERAGE_LINE)
		{
			HBRUSH hBrush = CreateSolidBrush(BACKCOLOR(Cols_CodeCoverage));
			HPEN hPen = CreatePen(PS_SOLID, 1, FORECOLOR(Cols_CodeCoverage));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

			// Define a circle with center halfway down right side of our
			// glyph space.
			int Radius = cxMargin - 1;
			int xCenter = rcMargin.right - 1;
			int yCenter = rcMargin.top + cyMargin / 2;
			int left = xCenter - Radius;
			int top  = yCenter - Radius;
			int right= xCenter + Radius;
			int bottom = yCenter + Radius;
			int xRadial1 = rcMargin.left;
			int yRadial1 = rcMargin.top;
			int xRadial2 = rcMargin.left;
			int yRadial2 = rcMargin.bottom;

			Pie (hDC, left, top, right, bottom, xRadial1, yRadial1, xRadial2, yRadial2);

			SelectObject(hDC, hOldBrush);
			SelectObject(hDC, hOldPen);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
#endif

		if ( uStatus & (BRKPOINT_LINE | DISABLED_BRKPOINT_LINE | MIXED_BRKPOINT_LINE) )
        {
			HBRUSH  hBrush;
			HPEN    hPen;
			HBRUSH  hOldBrush;
			HPEN    hOldPen;

            if ( uStatus & MIXED_BRKPOINT_LINE )
            {
                rgfmtel = gpISrc->GetSourceColor(CLR_BREAKPT);

                hBrush = CreateSolidBrush(GetSysColor(COLOR_GRAYTEXT));
                hPen   = CreatePen(PS_SOLID, 1, rgfmtel->rgbBackground);
            }
            else if ( uStatus & DISABLED_BRKPOINT_LINE )
            {
                rgfmtel = gpISrc->GetSourceColor(CLR_TEXT);
                hBrush  = CreateSolidBrush(rgfmtel->rgbBackground);

                rgfmtel = gpISrc->GetSourceColor(CLR_BREAKPT);
                hPen    = CreatePen(PS_SOLID, 1, rgfmtel->rgbBackground);
            }
            else if ( uStatus & BRKPOINT_LINE )
            {
                rgfmtel = gpISrc->GetSourceColor(CLR_BREAKPT);
                hBrush  = CreateSolidBrush(rgfmtel->rgbBackground);
                hPen    = CreatePen(PS_SOLID, 1, rgfmtel->rgbText);
            }

            hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
            hOldPen   = (HPEN)SelectObject(hDC, hPen);

			RECT rect;

			rect.left   = rcMargin.left + 2;
			rect.top    = rcMargin.top + (cyMargin - nGlyphSize) / 2;
			rect.right  = rect.left + nGlyphSize;
			rect.bottom = rect.top + nGlyphSize;
			Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);

			SelectObject(hDC, hOldBrush);
			SelectObject(hDC, hOldPen);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}

		if (uStatus & CALLER_LINE)
		{
			FMT_ELEMENT *rgfmtelCall = pfmtelFromDocType(CALLS_WIN);

			HBRUSH  hBrush    = CreateSolidBrush( rgfmtelCall[FMTEL_CSHI].rgbBackground );
			HPEN    hPen      = CreatePen(PS_SOLID, 1,  rgfmtelCall[FMTEL_CSHI].rgbText );
			HBRUSH  hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			HPEN    hOldPen   = (HPEN)SelectObject(hDC, hPen);
			POINT poly [3];
			RECT rect;

			rect.left   = rcMargin.right - (nMarginSize + 2);
			rect.top    = rcMargin.top + (cyMargin - nGlyphSize) / 2;
			rect.right  = rect.left + nGlyphSize;
			rect.bottom = rect.top + nGlyphSize;

			poly[0].x = rect.left;
			poly[0].y = rect.top;
			poly[1].x = rect.right - 1;
			poly[1].y = rect.top + nGlyphSize / 2;
			poly[2].x = poly[0].x;
			poly[2].y = rect.top + nGlyphSize;
			Polygon(hDC, &poly[0], 3);

			SelectObject(hDC, hOldBrush);
			SelectObject(hDC, hOldPen);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}

		if (uStatus & CURRENT_LINE)
		{
            rgfmtel = gpISrc->GetSourceColor(CLR_CURRENTLINE);

			HBRUSH  hBrush    = CreateSolidBrush(rgfmtel->rgbBackground);
			HPEN    hPen      = CreatePen(PS_SOLID, 1, rgfmtel->rgbText);
			HBRUSH  hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			HPEN    hOldPen   = (HPEN)SelectObject(hDC, hPen);
			RECT    rect;

			rect.left   = rcMargin.right - (nMarginSize + 2);
			rect.top    = rcMargin.top + (cyMargin - nGlyphSize) / 2;
			rect.right  = rect.left + nGlyphSize;
			rect.bottom = rect.top + nGlyphSize;

			POINT poly [7];

			poly[0].x = rect.left;
			poly[0].y = rect.top + nGlyphSize / 3;
			poly[1].x = rect.left + nGlyphSize / 2;
			poly[1].y = poly[0].y;
			poly[2].x = poly[1].x;
			poly[2].y = rect.top;
			poly[3].x = rect.right - 1;
			poly[3].y = rect.top + nGlyphSize / 2;
			poly[4].x = poly[2].x;
			poly[4].y = rect.bottom - 1;
			poly[5].x = poly[4].x;
			poly[5].y = rect.bottom - 1 - nGlyphSize / 3;
			poly[6].x = poly[0].x;
			poly[6].y = poly[5].y;
			Polygon(hDC, &poly[0], 7);

			SelectObject(hDC, hOldBrush);
			SelectObject(hDC, hOldPen);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
	}
}

void CMultiEdit::ShowContextPopupMenu(CPoint pt)
{
    return;
}

void CMultiEdit::OnEditCopy(void)
{
    SendMessage(WM_COPY, 0, 0L );
}


void CMultiEdit::OnUpdateGotoLine(CCmdUI * pCmdUI)
{
	// bobz - changed so always available due to infoviewer stuff

#if 0
    BOOL bEnable = FALSE;
    if ( IsKindOf(RUNTIME_CLASS(CMemoryView)) || IsKindOf(RUNTIME_CLASS(CDAMView)) )
        bEnable = TRUE;
#endif

    pCmdUI->Enable(TRUE);
}

void CMultiEdit::OnGotoLine( )
{
    ASSERT(IsKindOf(RUNTIME_CLASS(CMemoryView)) || IsKindOf(RUNTIME_CLASS(CDAMView)) );
    GotoLine( ); 
}

BOOL CMultiEdit::FCanUndo( )
{
	return IsEditable() &&
			m_mePlayCount >= 0 && 
			m_undoState == usNormal;
}

BOOL CMultiEdit::FCanRedo( )
{
	return IsEditable() &&
			m_mePlayCount != 0 && 
			m_mePlayCount != REC_CANNOTUNDO &&
			m_undoState == usNormal;

}

void CMultiEdit::OnEditUndo( )
{
	int cReps = max(1, theApp.GetCmdArg());

	for (int i = 0; i < cReps && FCanUndo() ; i++ )
		PostMessage(WM_UNDO, 0, 0L);

	theApp.ClearRepCount();
}

void CMultiEdit::OnEditRedo( )
{
	int cReps = max(1, theApp.GetCmdArg());

	for (int i = 0; i < cReps && FCanRedo() ; i++ )
		PostMessage(WM_UNDO, TRUE, 0L);

	theApp.ClearRepCount();
}

void CMultiEdit::OnUpdateEditUndo(CCmdUI * pCmdUI)
{
	BOOL bEnable = TRUE;
	
	pCmdUI->Enable(FCanUndo());
}

void CMultiEdit::OnUpdateEditRedo(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(FCanRedo());
}

