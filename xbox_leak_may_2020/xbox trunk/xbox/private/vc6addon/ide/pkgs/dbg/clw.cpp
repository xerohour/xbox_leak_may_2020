#include "stdafx.h"
#pragma hdrstop

#include "dbgenc.h"

// CL.C internal prototypes
extern char * PASCAL CLGetProcName(int, char *, int,BOOL );
extern char * PASCAL CLGetProcFromHfme ( HFME, char *, int, BOOL );
extern void PASCAL CLClearCachedStrings( void );
extern BOOL CLDispCallInfo( int iCall );


extern int iCallsCxf ;
extern CXF cxfCalls ;

IMPLEMENT_DYNCREATE( CCallsView, CMultiEdit )

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT CCallsView::g_cRef = 0;
CFontInfo *CCallsView::g_pFontInfo = NULL;

CCallsView :: CCallsView()
{
	m_dla.pElement = &(pfmtelFromDocType((DOCTYPE)CALLS_WIN)[FMTEL_TEXT]);
	m_dla.cb = 0xffff;

	m_fShowBlank = DebuggeeRunning();
	m_radixLast = radix;

	if (g_cRef++ == 0)
		g_pFontInfo = new CFontInfo(*(pfmtcatFromDocType((DOCTYPE)CALLS_WIN)->pLogFont));

	m_pFontInfo = g_pFontInfo;
    m_dt        = CALLS_WIN;

    AddThisToCMEList(this);
}

CCallsView :: ~CCallsView() {
	if (--g_cRef == 0)
	{
		delete g_pFontInfo;
		g_pFontInfo = NULL;
	}
}


BOOL CCallsView :: DispCallInfo( int iCall ) {

	if ( m_fShowBlank ) {
		return FALSE;
	}

	BOOL fRet = CLDispCallInfo(iCall);

	ADDR *paddr = (LPADDR)(SHpADDRFrompCXT(SHpCXTFrompCXF(&cxfCalls)));
	// Notify user in case of stale code due to an ENC operation
	if (ENCReplacesAddr(paddr)) {
		gpISrc->StatusText(IDS_Enc_SourceChanged,STATUS_ERRORTEXT,TRUE);
		MessageBeep( 0 );
	}

	return fRet;
}

/*** DumpHfme
*
*   Purpose: Display a specific frame in the call stack
*
*   Input:
*       hfme    Specific frame to dump
*       ifme    index in the list for display purposes.
*
*   Output:
*       szBuf   Must be axMax chars if not null
*
*   Returns:
*
*   Exceptions:
*
*   Notes:
*
*************************************************************************/
void CCallsView :: DumpHfme( HFME hfme, int ifme, WORD cbMax, char * szBuf )
{
    LPFME   lpfme;

    if ( !hfme )
        return;

    lpfme = (LPFME)LLLpvFromHlle( hfme );

	// If the line is cached, get it
	if ( lpfme->lsz )
    {
		// Copy to destination buffer
	    _ftcsncpy( szBuf, lpfme->lsz, cbMax );
	    szBuf[ cbMax - 1 ] = '\0';
	}

	// Not cached, create the line and cache it
	else
    {
	    ADDR    addrT;
	    HMOD    hmod;
	    WORD    wLine;
	    SHOFF   cbLine;
	    SHOFF   cbDeltaB = 0;
	    char	szDump[ axMax * 2 ];	// Need room for text + offset/address XXXX
	    char    sz[ axMax ];

	    addrT = lpfme->addrCSIP;
	    hmod  = 0;

		// Try to get a source line from the address.  wLine == 0 is not a valid
		// source line (1 based)
	    if ( ! SLLineFromAddr ( &addrT, &wLine, &cbLine, &cbDeltaB ) && cbDeltaB ) {
	        wLine = 0;
	    }

		// Fill in the proc, types, and params (where applicable)
	    CLGetProcFromHfme( hfme, sz, sizeof( sz ), (BOOL)runDebugParams.fCallParams );

		// If there's a line, display "foo.c line x [+ y byte[s]]"
	    if ( wLine ) {
	        if ( cbDeltaB == 0 ) {
	            sprintf ( szDump, "%s line %d", sz, wLine );
	        }
	        else {
	            sprintf(
	                szDump,
	                "%s line %d + %d byte%c",
	                sz,
	                wLine,
	                (int) cbDeltaB,
	                cbDeltaB == 1 ? '\0' : 's'
	            );
	        }
	    }
		// No line number, but we've got a symbol (public or otherwise)
	    else if ( lpfme->clt != cltNone ) {
            ADDR    addrT1 = lpfme->addrProc;

	        if ( ADDR_IS_LI ( addrT ) ) {
	            SYFixupAddr ( &addrT );
	        }

            if ( ADDR_IS_LI( addrT1 ) ) {
                SYFixupAddr( &addrT1 );
            }

			// Non-zero offset from symbol, display symbol + difference
            if ( GetAddrSeg( addrT ) == GetAddrSeg( addrT1 ) &&
                GetAddrOff( addrT ) != GetAddrOff( addrT1 ) ) {

                ULONG   ul = GetAddrOff( addrT ) - GetAddrOff( addrT1 );

                sprintf(
					szDump,
                    "%s + %ld %s",
					sz,
                    ul,
                    ul == 1L ? "byte" : "bytes"
                );
            }

			// Zero offset, display symbol name and actual address (flat-32 bit)
			else if ( ADDR_IS_FLAT ( addrT ) ) {
				ASSERT( ADDR_IS_OFF32( addrT ) );
                sprintf (
                    szDump,
                    "%s address 0x%08lx",
                    sz,
                    GetAddrOff ( addrT )
                );
            }

			// Zero offset, display 16:16 address or 16:32 (Mac)
            else {
                sprintf (
                    szDump,
                    "%s address %04x:%0*lx",
                    sz,
                    GetAddrSeg ( addrT ),
					ADDR_IS_OFF32( addrT ) ? 8 : 4,
                    GetAddrOff ( addrT )
                );
            }
		}
		// No line number or symbol, just an address.  Already formatted.
		else {
            _ftcscpy( szDump, sz );
        }

		// Copy to destination buffer
	    _ftcsncpy( szBuf, szDump, cbMax );
	    szBuf[ cbMax - 1 ] = '\0';

		// Attempt to cache the result
		if ( lpfme->lsz = (LSZ)_fmalloc( _tcslen( szDump ) + 1 ) ) {
			_ftcscpy( lpfme->lsz, szDump );
		}
	}

    UnlockHlle( hfme );
}

void CCallsView :: DrawLineGraphics( CDC * pdc, RECT & rect, DWORD oln )
{
	DrawMarginGlyphs( pdc->m_hDC, rect, m_WidgetFlags );
}

DWORD CCallsView :: CLinesInBuf()
{
	DWORD	olnMac = (DWORD)CLChfmeOnStack();;
	
	if ( m_fShowBlank || ( !olnMac && DebuggeeRunning() ) )
		olnMac = 1;

	return olnMac;
}

#define CHECK_DISABLED_BREAKPOINTS TRUE

WORD CCallsView::CbGetLineBuf(DWORD ilnGet, WORD cbMax, char *szBuf, PDLA &pdla)
{
	HFME	hfme = CLHfmeGetNth((int)ilnGet);
	FMT_ELEMENT *rgfmtel = pfmtelFromDocType((DOCTYPE)CALLS_WIN);

    UINT uiWidgetFlag;
	m_WidgetFlags = 0;

	if (!m_fShowBlank && pDebugCurr && pDebugCurr->IsJava () && DebuggeeRunning ())
	{
		m_fShowBlank = TRUE;
	}

	if ( hfme && !m_fShowBlank ) {
		LPFME	lpfme = (LPFME)LLLpvFromHlle( hfme );

		// Format the line for the textual portion of the display
	    DumpHfme( hfme, (int)ilnGet, cbMax, szBuf );

		// Set the display attributes.
		m_dla.pElement = &(rgfmtel[FMTEL_TEXT]);

		// Breakpoint?
		if ( BPNbrAtAddr(&lpfme->addrCSIP,1,CHECK_DISABLED_BREAKPOINTS,&uiWidgetFlag ) != -1 )
        {
			m_dla.pElement = gpISrc->GetSourceColor(CLR_BREAKPT);
			m_WidgetFlags |= uiWidgetFlag;
		}

		// PC line?
		if ( !ilnGet ) {
			m_dla.pElement = gpISrc->GetSourceColor(CLR_CURRENTLINE);
			m_WidgetFlags |= CURRENT_LINE;
		}

		// Call frame line
		else if ( ilnGet == (DWORD)iCallsCxf ) {
	        m_dla.pElement = &(rgfmtel[FMTEL_CSHI]);
			m_WidgetFlags |= CALLER_LINE;
		}

		// Either widgets OR text hiliting, not both
		if ( m_cxSelectMargin ) {
			m_dla.pElement = &(rgfmtel[FMTEL_TEXT]);
		}
		else {
			m_WidgetFlags = 0;
		}

		// Show superceded code after ENC using appropriate color
		// (use FMTEL_CSSU instead of FMTEL_TEXT -- vc98 #26105)
		if ( ENCReplacesAddr(&lpfme->addrCSIP) && 
			m_dla.pElement == &(rgfmtel[FMTEL_TEXT]) ) {
			m_dla.pElement = &(rgfmtel[FMTEL_CSSU]);
		}

	}
	else {
		m_dla.pElement = &(rgfmtel[FMTEL_TEXT]);
		*szBuf = '\0';
		if ( DebuggeeRunning() )
        {
			::LoadString( hInst, DBG_No_Callers_Avail, szBuf, cbMax );

			m_fShowBlank = TRUE;
		}
	}

	pdla = &m_dla;

    return _ftcslen( szBuf );
}

LRESULT CCallsView :: LTextProc(
UINT	wMsg,
WPARAM	wParam,
LPARAM	lParam ) {
	LRESULT	lRet = 0L;
	BOOL	fUseEM = TRUE;	// Use the EditMgr?

    switch( wMsg ) {
        case WU_INITDEBUGWIN:
            lRet = InitEditMgr();
			fUseEM = FALSE;
            break;

		case WU_GETCURSORADDR: {
			// Get the address at the cursor.  This is to support
			// "Goto cursor", "Toggle breakpoint at cursor", ...
			HFME	hfme = CLHfmeGetNth( (int)OlnGetCurPos() );

			// If the hfme is non-null, then we've got a frame, get
			// the return address and return success!
			if ( hfme )
            {
				LPFME		lpfme = (LPFME)LLLpvFromHlle( hfme );

				((PGCAINFO)lParam)->addr = lpfme->addrCSIP;
				UnlockHlle( hfme );
				lRet = (LRESULT)gcafAddress;
			}

			fUseEM = FALSE;
			break;
		}

		case WU_UPDATEDEBUG:
        case WM_PAINT:
			if ( m_radixLast != radix ) {
				CLClearCachedStrings();
				m_radixLast = radix;
			}
			// Always make sure that the CL manager (cl.c) has
			// a cached walkback stack.  Will be free'd up when
			// the child is executed and when memory/register
			// writes occur.  Don't update the callstack in
			// the event that the debuggee is running when
			// we get a paint message.  The htidCurr may have
			// died since the UI was last updated
			if ( wMsg == WU_UPDATEDEBUG || !DebuggeeRunning() ) {
            	CLGetWalkbackStack( hpidCurr, htidCurr, (UINT)NULL );
				m_fShowBlank = FALSE;
			}

			// If the old cursor position is no longer valid, reset
			// the displayed line to the cs:eip (first line in the buffer)
            if ( (DWORD)CLChfmeOnStack() < OlnGetCurPos() ) {
                SetWinTopLine( 0L );
            }

			CMultiEdit::LTextProc (WU_UPDATESCROLLBARS, 0, 0);
            break;

        case WM_LBUTTONDBLCLK:
            wParam = VK_RETURN;
            // fall through to WM_CHAR

        case WM_CHAR:
            if ( wParam == VK_RETURN ) {
				// Don't do anything if the debuggee is running!
				if ( DebuggeeRunning() ) {
					MessageBeep( 0 );
				}
				else {
					DispCallInfo( (int)OlnGetCurPos() );
				}
                fUseEM = FALSE;
			}
            break;

		case WM_COMMAND:
			fUseEM = FALSE;

			// This forces the cache to be blown away!
			CLFreeWalkbackStack( FALSE );

			// Update the window
			InvalidateRect( NULL, FALSE );
			break;
    }

    // Some messages may have been already processed, so don't
    // pass on if eaten already
    if ( fUseEM ) {
        lRet = CMultiEdit::LTextProc( wMsg, wParam, lParam );
    }

    return lRet;
}

BEGIN_POPUP_MENU(CallStack)
	MENU_ITEM(IDM_GOTO_CODE)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_RUN_TOGGLEBREAK)
	MENU_ITEM(IDM_RUN_ENABLEBREAK)
	MENU_ITEM(IDM_RUN_TOCURSOR)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_DBGSHOW_STACKVALUES)
	MENU_ITEM(IDM_DBGSHOW_STACKTYPES)
	MENU_ITEM(IDM_DEBUG_RADIX)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_TOGGLE_MDI)
	MENU_ITEM(ID_WINDOW_POPUP_HIDE)
END_POPUP_MENU()

void CCallsView::ShowContextPopupMenu(CPoint pt)
{
    #ifndef NO_CMDBARS
        TRACE2("CBShow: Callstack (%s, %d)\r\n", __FILE__, __LINE__);
        ::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(CallStack), pt);
    #else
    	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(CallStack), pt);
    #endif
}

