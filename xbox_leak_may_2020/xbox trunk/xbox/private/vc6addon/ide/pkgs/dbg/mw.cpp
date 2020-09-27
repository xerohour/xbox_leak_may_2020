/////////////////////////////////////////////////////////////////////////
//
//	MW.C - Memory Window Handler
//
//	Copyright (C)1990, Microsoft Corporation
//
//	Purpose: To display and update the memory windows.
//
//  Revision History:
//       Written by: Mark A. Brodsky
//
// 	Notes:
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

IMPLEMENT_DYNCREATE( CMemoryView, CMultiEdit )

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT        CMemoryView::g_cRef         = 0;
CFontInfo   *CMemoryView::g_pFontInfo   = NULL;

CMemoryView :: CMemoryView()
{
	memset(&m_mwi, 0, sizeof(MWI));

	m_hlliCur       = (HLLI)NULL;
	m_rgbCacheCur   = (BYTE FAR *)NULL;
	m_hlliPrev      = (HLLI)NULL;
	m_rgbCachePrev  = (BYTE FAR *)NULL;

	if (g_cRef++ == 0)
		g_pFontInfo = new CFontInfo(*(pfmtcatFromDocType((DOCTYPE)MEMORY_WIN)->pLogFont));

    m_pFontInfo = g_pFontInfo;
    m_dt        = MEMORY_WIN;

    AddThisToCMEList(this);
}

CMemoryView :: ~CMemoryView()
{
	UncacheAll();

	DestroyUndoList();

	if ( m_mwi.szExpr )
    {
		runDebugParams.strExpr = m_mwi.szExpr;
		free(m_mwi.szExpr);
		m_mwi.szExpr = NULL;
	}

	if ( --g_cRef == 0 )
	{
		delete g_pFontInfo;

		g_pFontInfo = NULL;
	}

	if ( m_mwi.rgwLineInfo )
	{
		_ffree(m_mwi.rgwLineInfo);

		m_mwi.rgwLineInfo = NULL;
	}

	if ( m_mwi.rgf )
	{
		_ffree(m_mwi.rgf);
		m_mwi.rgf = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//
//	      PUBLIC API's - Interfaces to the Memory Window
//
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//// MWSaveSettings
//
// Purpose: Save information into global runDebugParams for global access
//
// Input:
//	sz	Address expression to save into global runDebugParams.pszExpr
//
// Output:
//	none.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void PASCAL MWSaveSettings( char * sz )
{
	runDebugParams.strExpr = sz;
}

//// MWScanType
//
// Purpose: Circularly loop through display types resulting from <Sh+F3>
// accelerator.
//
// Input:
//	iDir	Direction for change ( 1 == foward, -1 == backwards )
//
// Output:
//	Active memory window changes display format.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void PASCAL MWScanType(short	iDir )
{
	if ( pViewMemory )
    {
		USHORT			imwd;
		USHORT			imwdMax;
		CMemoryView *	pmv;
	
		pmv = (CMemoryView *)pViewMemory;
		ASSERT(pmv->IsKindOf(RUNTIME_CLASS(CMemoryView)));

#if _INTEGRAL_MAX_BITS >= 64
		imwdMax = MW_cDisplayTypesExt;
#else
		imwdMax = MW_cDisplayTypes;
#endif

		if ( !(imwd = pmv->m_mwi.pmwd - rgmwd) && iDir == -1 )
			imwd = imwdMax - 1;
		else
			imwd = ( imwd + iDir ) % imwdMax;

		pmv->SetAttributes(imwd, NULL, MWNINCH, MWNINCH, MWNINCH);

		runDebugParams.iMWFormat = imwd;
	}
	else
		MessageBeep( 0 );
}

//// UpdateWindows
//
// Purpose: Update the memory windows. This should be called when
// the debuggee returns from execution.
//
// Input: None.
//
// Output: Window upating.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: UpdateView()
{
	// If the window's open, re-evaluate the expression if live, and
	// repaint the window.

	if ( m_mwi.fLiveExpr == MW_LIVE )
		SetAttributes(MWNINCH, NULL, MWNINCH, MWNINCH, MWNINCH);
	else
		UpdateExprFromMwiAddr();

    UncacheData();

	InvalidateRect( NULL, FALSE );

	CMultiEdit::LTextProc(WU_UPDATESCROLLBARS, (WPARAM)0, (LPARAM)0);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//
//                     Memory Text Window Procedure
//
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//// LTextProc
//
// Purpose: Main window procedure for the memory windows.
//
// Input:
//	wMsg		Message for window.
//	wParam		Message specific information.
//	lParam		Message specific information.
//
// Output:
//	Varies with message.  Usually, 0 == OK, non-zero == failure/error
//
// Exceptions:
//	WM_CLOSE: Free up any allocated data associated with the window
//
//	WM_CHAR: Special case the following:
//		TAB: Skip to the next field of any type.
//		SHIFT-TAB: Skip to the previous field of any type.
//		Ctrl-E: If cursor is on valid field, bring up a dialog to
//		     edit field.
//		BSP: Fake backspace, by shifting field data 1 char right
//                   up to insertion point-1.
//		Other chars: If printable, make edit and redisplay line.
//		     Otherwise, pass data on to the edit manager for normal
//                   interpretation.
//
//	WM_CLEAR: Delete key has been pressed. Fake delete key, by
//	     shifting field data 1 char right up to the insertion point.
//
//	WM_CREATE: Allocate required space and set default display conditions.
//
//	WM_SETFOCUS: Make the window receiving the focus the active memory
//	     window.
//
//	WM_SIZE and WM_ZOOM: Window size is being changed.  Recalculate
//	     the display information for the window and redisplay if
//	     necessary.
//
//	WM_UNDO: Undo the last edit command.
//
// Notes: For the most part, let the EditMgr take care of everything.
//
///////////////////////////////////////////////////////////////////////////
LRESULT CMemoryView :: LTextProc(UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRet    = 0L;
    BOOL    fUseEM  = TRUE; // Use the EditMgr?
	PMWI    pmwi    = NULL;

	errno = CV_NOERROR;

	switch( wMsg )
    {
		case WM_CHAR:
            switch( wParam )
            {
		    	case VK_TAB:
                {
                    WORD    cRep = LOWORD( lParam );

                    ASSERT( cRep );
	    			fUseEM = FALSE;
                    while( cRep-- )
						NextField ((short)( HIWORD( lParam ) & KK_SHIFT ? MW_REV : MW_FWD ), TRUE);

                    break;
                }

                case (WORD)chBsp:
                {
                    WORD    cRep = LOWORD(lParam);

                    ASSERT( cRep );
       				fUseEM = FALSE;

                    while( cRep-- )
	        			DoDelChar(TRUE);

                    break;
                }

                default:
                    if ( _istprint( wParam ) )
				       	fUseEM = !FDoChar(wParam, lParam);

                    break;
            }
			break;

		case WM_KEYDOWN:
			if ( wParam == VK_DELETE )
            {
				DoDelChar(FALSE);
				fUseEM = FALSE;
			}

			break;

		case WU_CLEARDEBUG:
			UncacheAll();
			DestroyUndoList();
			MWSaveSettings(m_mwi.szExpr);
			break;

		case WU_UPDATEDEBUG:
			UpdateView();
			break;

		case WU_FREEZEDATA:
			SaveDebuggeeData();
			DestroyUndoList();
			break;

		case WU_UPDATESCROLLBARS:
			SetAttributes(MWNINCH, NULL, MWNINCH, MWNINCH, MWNINCH);
			break;

		case WU_INITDEBUGWIN:
			lRet = 1L;
			InitEditMgr();
			errno = CV_NOERROR;

			// Reset this to uninitialized so we
			// pick up the configuration from
			// the rundebugparams later!
			m_mwi.fNoUpdate = FALSE;
			m_mwi.fRetry = FALSE;
			break;

        case WM_SYSCOMMAND:
            switch ( wParam )
            {
                case SC_MINIMIZE:
        			SetAttributes(MWNINCH, NULL, MWNINCH, MWNINCH, MWNINCH);
        			break;

		        case SC_SIZE:
		        case SC_MAXIMIZE:
    		    	// window size being altered, adjust redisplay
	    		    // buffer size so we can indicate correct data
    		    	// changes.
    	    		if ( m_mwi.fNoUpdate )
	    	    		SetAttributes(MWNINCH, NULL, MWNINCH, MWNINCH, MWNINCH);
            }
			break;

		case WM_SIZE:
			// window size being altered, adjust redisplay
			// buffer size so we can indicate correct data
			// changes.
			if ( m_mwi.fNoUpdate )
				SetAttributes(MWNINCH, NULL, MWNINCH, MWNINCH, MWNINCH);
			break;

		case WM_SETFOCUS:
			MakeWindowActive();
			break;

		case WM_UNDO:
			fUseEM = FALSE;
			UndoValue((BOOL) wParam);
			break;

		case WM_COMMAND:
			// lParam (if non-null) is a POINT FAR * for drag and
			// drop.  The new address should be placed on this line
			if ( lParam )
            {
				DWORD	 oln;

				oln = (DWORD)(((POINT FAR *)lParam)->y / m_pFontInfo->m_tm.tmHeight) + OlnGetTop();

				SetCursorAt(ObGetCurPos(), oln);
			}

            SetAttributes(runDebugParams.iMWFormat,
                          (LPSTR)(LPCSTR)runDebugParams.strExpr,
                          runDebugParams.fMWLive,
                          runDebugParams.fMWShowRaw,
                          (USHORT)runDebugParams.iMWItemsPerLine);

			wMsg = WU_UPDATESCROLLBARS;
			ChangeDebuggeeData();
			break;

		default:
			break;
	}

	if ( fUseEM )
		lRet = CMultiEdit::LTextProc(wMsg, wParam, lParam);

	return( lRet );
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//
//          Edit Manager Initialization and Callback Procedures
//
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//// CbGetLineBuf
//
// Purpose: Get a requested display line and pass back to the EditMgr.
//
// Input:
//	iLn	index to the requested line number.
//	cbMax	maximum size of returned string(s).
//	rgch	Place to put data.
//
// Output: Number of bytes stuffed into rgch.
//
// Exceptions:  If this routine is called by functions outside
// of the EditMgr the caller MUST SET pwndEditCur to
// the window
//
// Notes:  To get around a previous problem, the global variable
// pwndEditCur can be used to determine which window request is
// servicing.  The EditMgr maintains this value.
//
///////////////////////////////////////////////////////////////////////////
WORD CMemoryView :: CbGetLineBuf(DWORD iln, WORD cbMax, char *rgch, PDLA &pdla )
{
	// MWFormatLine is generic which uses window information.
	FormatLine(iln, rgch);

	pdla = m_rgdla;

	return( _ftcslen(rgch) );
}

//// CLinesInBuf
//
// Purpose: To inform the EditMgr how many lines are in this buffer.
//
// Input:
//
// Output: The number of lines which we can display.
//
// Exceptions:
//
// Notes:  We will be using the pwndEditCur to delve into the data
// structures for the specified window.
//
///////////////////////////////////////////////////////////////////////////
DWORD CMemoryView :: CLinesInBuf()
{
	DWORD	dwRet;

	// Try setting the memory window to the expression from runDebugParams
	// a) The first time we get in.
	// b) if we failed the first time and the debuggee hasn't reached
	//    the entry point restart. [sanjays]
	if ( !m_mwi.fNoUpdate || (m_mwi.fRetry && lpprcCurr && lpprcCurr->IsBeyondEntryPoint()) )
    {
		MWI		mwi = {0};
		char 	rgch[20];
		ADDR 	addr = {0};
		char *	psz = 0;
		BOOL	fFailed = FALSE;

		m_mwi.fNoUpdate = TRUE;
		m_mwi.fRetry  = FALSE;

		// The user may have tried to entered something
		// which we can't parse/bind/eval, so if that
		// happens, restore the address to OSDebug default
		// data address
		if (!runDebugParams.strExpr.IsEmpty())
			mwi.szExpr = (LPSTR)(LPCTSTR)runDebugParams.strExpr; // hack
		if ( mwi.szExpr && !MWSetAddrFromExpr(&mwi, MW_NO_AUTO_TYPE) )
        {
			fFailed = TRUE;
            // If we haven't reached the entry point yet, we should retry when 
            // all the modules are loaded. 
			if ( !lpprcCurr || !lpprcCurr->IsBeyondEntryPoint() )
				m_mwi.fRetry = TRUE;
		}

		if ( fFailed || runDebugParams.strExpr.IsEmpty() )
        {
			if ( !OSDGetAddr(hpidCurr, htidCurr, adrData, &addr) )
            {
				if ( ADDR_IS_FLAT(addr) )
                {
					ASSERT(ADDR_IS_OFF32(addr));

					sprintf(rgch, "0x%08lX", GetAddrOff ( addr ));
				}
				else
                {
					sprintf(rgch, "0x%04X:0x%0*lX",
                            GetAddrSeg(addr),
    						ADDR_IS_OFF32(addr) ? 8 : 4,
    						GetAddrOff( addr ) );
				}
			}
			else
				_ftcscpy(rgch, szDefaultExpr);

			psz = rgch;
		}
		// we either set the default (psz) or user's was ok (rundebugParams.strExpr)
		psz = psz ? psz : mwi.szExpr;
		if ( !SetAttributes(runDebugParams.iMWFormat,
            				psz, 
            				runDebugParams.fMWLive,
            				runDebugParams.fMWShowRaw,
            				(USHORT)runDebugParams.iMWItemsPerLine) )
			return 0L;

		SaveDebuggeeData();
		MakeWindowActive();
    }

	dwRet = m_mwi.cLines;

	return( dwRet );
}

WORD CMemoryView :: ObMaxBuf()
{
	return( m_mwi.cwLineInfo );
}

BOOL CMemoryView::InsertFromDataObject(COleDataObject * data, const CPoint & point)
{
	UINT cf;
	HGLOBAL hData = NULL;
	LPSTR pszExpr;

	if (data->IsDataAvailable(g_cfTMText))
	{
		cf = g_cfTMText;
	}
	else if (data->IsDataAvailable(CF_TEXT))
	{
		cf = CF_TEXT;
	}
	else
	{
		return FALSE;
	}

	hData = data->GetGlobalData(cf);
	if (NULL == hData)
		return FALSE;

	if (RealGlobalSize(hData))
	{
		// Get a pointer to the data.  Leave if we can't do it.
		VERIFY((pszExpr = (LPSTR)RealGlobalLock(hData)));
		if (!pszExpr)
			return FALSE;
		CPoint pt(point);
		MWDropNewAddress(pszExpr, strlen(pszExpr), &pt);
		RealGlobalUnlock(hData);
		return TRUE;
	}
	return FALSE;
}

void CMemoryView::OnDragEnterFeedback(DWORD grfKeyState, CPoint point)
{
//	CreateGrayCaret(CURSOR_WIDTH, m_pFontInfo->m_tm.tmHeight);
	OnDragOverFeedback(grfKeyState,point);
	ShowCaret();
}

void CMemoryView::OnDragOverFeedback(DWORD grfKeyState, CPoint point)
{
	DWORD oln;
	oln = (DWORD)((point.y / m_pFontInfo->m_tm.tmHeight) + OlnGetTop());
	//SetCursorAt(ObGetCurPos(), oln);

	CRect rc;
	GetClientRect(&rc);
	// make sure line is completely in window so SetCursorAt doesn't scroll
	if ((oln + 1 - OlnGetTop())*m_pFontInfo->m_tm.tmHeight < rc.bottom)
	{
//		SetCursorAt(ObGetLeft(), oln);
		EMSC	emsc;
		emsc.x1 = ObGetLeft();
		emsc.x2 = ObGetLeft() + 8;
		emsc.y = oln;
		CMultiEdit::LTextProc( EM_SELCHARS, 0, (LPARAM)&emsc );
	}
}

void CMemoryView::OnDragLeaveFeedback()
{
//	CreateSolidCaret(CURSOR_WIDTH, m_pFontInfo->m_tm.tmHeight);
}

	
BOOL CMemoryView::Create(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID, CCreateContext *pContext )
{
	CMemoryParent   *pMemoryParent = new CMemoryParent;
	DWORD           styleP         = dwStyle & ~(WS_HSCROLL | WS_VSCROLL | WS_BORDER);

	if ( !pMemoryParent->Create(NULL, NULL, styleP, rect, pParentWnd, nID, NULL) )
		return FALSE;

	if ( !CMultiEdit::Create(lpszClassName, lpszWindowName,
                       dwStyle | WS_VISIBLE | WS_CLIPSIBLINGS,
                       rect, pMemoryParent, nID, pContext) )
	{
		return FALSE;
	}

	pMemoryParent->m_pView = this;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//	ShowContextPopupMenu

BEGIN_POPUP_MENU(Memory)
	MENU_ITEM(IDM_DBGSHOW_MEMBYTE)
	MENU_ITEM(IDM_DBGSHOW_MEMSHORT)
	MENU_ITEM(IDM_DBGSHOW_MEMLONG)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_DBGSHOW_MEMORY_BAR)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_TOGGLE_MDI)
	MENU_ITEM(ID_WINDOW_POPUP_HIDE)
END_POPUP_MENU()

void CMemoryView::ShowContextPopupMenu(CPoint pt)
{
    #ifndef NO_CMDBARS
        TRACE2("CBShow: CMemoryView (%s, %d)\r\n", __FILE__, __LINE__);
        ::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(Memory), pt);
    #else
    	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(Memory), pt);
    #endif
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//
//	      PUBLIC UTILITY API's - Interfaces to the Memory Window
//
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

/*
 * LINE_NUMBER_ADDRESS
 *
 *  Parses line number and address expressions.  Error if module is not
 *      loaded.  In DOS5, non-breakpoint addresses have to be loaded.
 */

int PASCAL line_number_address(PADDR paddr, ushort BPSegType, char *s)
{
    PBP pbp;
	BREAKPOINTNODE bpn;
	EESTATUS eest;
	TCHAR sz[cbBpCmdMax];

	// BPParseAddress requires the bp string to be in the canonical form.
	// So we have to do the Parse - MakeCV400BPCmd bit.
	memset( &bpn, 0, sizeof(BREAKPOINTNODE));

	if ( !ParseCV400Location(s, &bpn) && !ParseQC25Location (s, &bpn) )
		return FALSE ;

	// Create a CV BP descriptor string from our descriptor
	*sz = '\0';
	MakeCV400BPCmd(&bpn, sz, sizeof(sz));

    memset( &pbp, 0, sizeof(PBP) );

    pbp.lszCmd		= (char FAR *)sz;
    pbp.BPSegType	= BPSegType;
    pbp.pCXF		= PcxfLocal();

    if ( eest = BPParseAddress( &pbp, TRUE ) )
    {
		errno = eest;
		return FALSE;
    }

    if ( !errno  && ( pbp.BPSegType == EECODE ) && !SHFIsAddrNonVirtual(&pbp.Addr))
    {
		errno = MODLOADERROR;
		return FALSE;
    }

    *paddr = pbp.Addr;

	return(TRUE);
}

// Drag and drop support.  Call this function when
// a string is available for dropping to set the
// memory window's top line to
void MWDropNewAddress(LPSTR lszExpr, DWORD cbData, POINT FAR * lppt)
{

	ASSERT( pViewMemory );

	// Only support this when debugging
	if ( DebuggeeAlive() )
    {
		TCHAR	sz[ axMax ];
		TCHAR *	ptch = sz;
		MWI		mwi = {0};
		
		// Copy the string into local memory and only
		// replace an eol or end of buffer with a zero
		// terminator since FAddrToSz only allows for
		// a single-line zero terminated string
		memset(sz, 0, sizeof(sz));
		_ftcsncpy(sz, lszExpr, (size_t)min((DWORD)sizeof(sz) - 1, cbData));

		while( *ptch && *ptch != '\n' && *ptch != '\r' )
			ptch = _tcsinc( ptch );

		*ptch = '\0';

		mwi.szExpr = sz;

	    if ( MWSetAddrFromExpr( &mwi, runDebugParams.iMWAutoTypes ) ) {
			// Update the global address
			MWSaveSettings( sz );

			// Update the memory window
			pViewMemory->SendMessage(WM_COMMAND, 0, (LPARAM)lppt);
		}
		else
			MessageBeep( 0 );
	}
}
