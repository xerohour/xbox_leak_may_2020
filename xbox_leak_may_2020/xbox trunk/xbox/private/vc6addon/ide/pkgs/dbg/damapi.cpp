#include "stdafx.h"
#pragma hdrstop

IMPLEMENT_DYNCREATE( CDAMView, CMultiEdit )

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

UINT CDAMView::g_cRef = 0;
CFontInfo *CDAMView::g_pFontInfo = NULL;

CDAMView :: CDAMView() {
	// Set up the default parameters.  This should
	// come from some kind of global state eventually!
	memset( &m_dmi, 0, sizeof( DMI ) );

	m_dmi.fAnnotate		= runDebugParams.fDAMAnnotate;
	m_dmi.fDoCodeBytes  = runDebugParams.fDAMCodeBytes;
    m_dmi.fDoSymbols    = runDebugParams.fDAMSymbols;
	m_dmi.fLowerCase	= runDebugParams.fDAMLowerCase;

	m_dmi.addrCSIP = cxfIp.cxt.addr;
	ADDR_IS_LI(m_dmi.addrCall) = TRUE;
	m_dla.pElement = &(pfmtelFromDocType((DOCTYPE)DISASSY_WIN)[FMTEL_TEXT]);
    m_dla.cb = 0xffff;

	m_bAskForFile = !DebuggeeRunning( );  // Don't ask for file name if we do a go.

	memset( &m_dmi.addrSegLim, 0, sizeof( ADDR ) );

	if (g_cRef++ == 0)
	{
		g_pFontInfo = new CFontInfo(*(pfmtcatFromDocType((DOCTYPE)DISASSY_WIN)->pLogFont));
	}
	m_pFontInfo = g_pFontInfo;
    m_dt        = DISASSY_WIN;

    AddThisToCMEList(this);
}

CDAMView :: ~CDAMView() {
	DestroyMap();

	if (--g_cRef == 0)
	{
		delete g_pFontInfo;
		g_pFontInfo = NULL;
	}

    // Set the step mode to SRCSTEPPING.
    TraceInfo.StepMode = SRCSTEPPING;
}

DWORD CDAMView :: CLinesInBuf () {
	DWORD	cln;
	
	if ( !m_dmi.rgdml )
		cln = 0L;
	else
    {
		// If lnEnd is zero, then we need to initialize the map
		if ( !m_dmi.lnEnd )
			UpdateView( &cxfIp );

		cln = m_dmi.lnEnd;
	}

	return cln;
}

/*** DAMCbGetLineBuf *********************************************************
 *
 * Purpose  : to get the edit manager the reqd display line
 *          :
 * Input    : hbuf - the source window of interest
 *          : ln   - line number ( 0 based )
 *          : cbMax- maximum number of characters permitted
 *          : szDst- destination area for the lines characters
 *          :
 * Output   : return - the length of the line's string
 *          : szDst  - the string to place on line ln
 *          :
 * Side     : m_dla - This is set up for the line "ln" so that it
 *  effects :   can be returned by DAMGetLineAttrs
 *          :
 *          : The map may be reset if the edit manager has decided to
 *          :   start displaying lines somewhere off the map.
 *          :
 *          : The map entry associated with ln will be filled if it is
 *          :   not already valid.
 *
 ****************************************************************************/

WORD CDAMView :: CbGetLineBuf ( DWORD ln, WORD cbMax, char *szDst, PDLA & pdla ) {
	char *	pch;
	char *	szSp;
    int		idml = 0;
	PDML	pdml;

	pdla = &m_dla;

    if ( m_dmi.fDontDraw  ) {
		m_dla.pElement = &(pfmtelFromDocType((DOCTYPE)DISASSY_WIN)[FMTEL_TEXT]);
        *szDst = '\0';
        return 0;
    }

	// We will get here when the user has shrunk the window, scrolled
	// to the bottom of the map and resized much bigger.  What will
	// happen is that we will be off of the map.  This will not happen
	// very often so don't worry about alignment.  To corrected this
	// reset the map such that the top line of the window is at the
	// beginning of the map.
	if ( ln >= m_dmi.lnMapStart + (DWORD)m_dmi.cdml ) {
		int	idmlTop = (int)( OlnGetTop() - m_dmi.lnMapStart );

		// Move the map so we can get this line in
		memmove(
			&m_dmi.rgdml[ 0 ],
			&m_dmi.rgdml[ idmlTop ],
			( m_dmi.cdml - idmlTop ) * sizeof( DML )
		);
		m_dmi.idmlInvalid -= idmlTop;
		m_dmi.lnMapStart = OlnGetTop();
	}

	if (
        (ln >= m_dmi.lnMapStart + m_dmi.idmlInvalid ) &&
        (ln < m_dmi.lnMapStart + m_dmi.cdml )
    ) {
        //
        // If the line is in the invalid region we must build entry
        //

        for (
            idml = m_dmi.idmlInvalid;
            idml <= (int) ( ln - m_dmi.lnMapStart );
            idml++
        ) {
            FillEntryFromPrev ( idml );

			if (pDebugCurr && pDebugCurr->IsJava () &&
				m_dmi.lnMapStart + idml > m_dmi.lnEnd)
			{
				break;
			}
		}

        m_dmi.idmlInvalid = idml;
    }

    idml = (int) (ln - m_dmi.lnMapStart);
	ASSERT( idml >= 0 );

	pdml = &m_dmi.rgdml[ idml ];

    if ( idml < m_dmi.cdml ) {
        //
        //  At this point we are guaranteed a valid map entry
        //

        switch ( pdml->met ) {

            case metNone:
                *szDst = '\0';
                m_dmi.lnEnd = ln;
                break;

			case metFileName: {
				HSF		hsf = pdml->mfl.hsf;
				char	sz[ axMax ];
				size_t	cch;
				
				cch = min(
					max( (size_t)WinWidth() + 1, 80 ),
					sizeof( sz ) - 1
				);

				if ( hsf ) {
					char FAR *	lpst = SLNameFromHsf( hsf );
					char		cchT;

					ASSERT( lpst );
					cchT = (size_t)*(unsigned char *)lpst;
					_ftcsncpy( sz, lpst + 1, cch );
					sz[ cchT ] = '\0';
				}
				else {
					::LoadString( hInst, IDS_NO_SOURCE, sz, sizeof(sz) );
				}
				_ftcscat( sz, " " );

				memset( szDst, '-', cch );
				szDst[ cch ] = '\0';

				// Just needs to be bigger than 5, but it comes
				// from the edit manager, so it's probably axMax (255)
				ASSERT( cbMax > 10 );
				cch = min( _ftcslen( sz ), (size_t)cbMax - 4 );

				_ftcsncpy( szDst + 4, sz, cch );

				// Pad spaces around the file name just to look a bit better
				szDst[ 3 ] = ' ';
				szDst[ 4 + cch ] = ' ';
				break;
			}

			case metSource: {
				int cbSprintf;

                memset ( szDst, ' ', 6 );
				cbSprintf = sprintf (
                    szDst,
                    "%u:",
                    pdml->mfl.wLine
				);

				*( szDst + cbSprintf ) = ' ';

				if ( !DAMFGetLine (
                    pdml->mfl.hsf,
                    (long) pdml->mfl.wLine,
                    cbMax - 6,
                    szDst + 6,
                    //gpISrc->GetTabSpacing()) // m_cbTab )
					4)
                ) {
					if ( !LoadString(
						hInst,
						IDS_SOURCE_DIFFERS,
						szDst + 6,
						cbMax - 6 )
					) {
						*( szDst + 6 ) = '\0';
					}
                }
                break;
			}

            case metDisasm: {

                    SDI  sdi;
                    ADDR addr = pdml->addr;

                    sdi.dop = (DOP)(
                        dopOpcode |
                        dopOperands |
						dopAddr |
                        ( !m_dmi.fLowerCase * dopUpper ) |
                        ( m_dmi.fDoCodeBytes * dopRaw ) |
                        ( m_dmi.fDoSymbols * dopSym ) |
                        ( ADDR_IS_FLAT(addr) * dopFlatAddr )
                    );
                    sdi.addr = addr;

                    SYUnassemble ( hpidCurr, htidCurr, &sdi );
                    addr = sdi.addr;

                    // Set the next address if it is not already valid
                    //  to save some work for FillEntryFromNext

                    if ( idml == (int) m_dmi.idmlInvalid - 1 &&
                        idml < m_dmi.cdml - 1
                    ) {
                        m_dmi.rgdml [ idml + 1 ].addr = addr;
                        m_dmi.fNextAddrSet = TRUE;
                    }

                    SYFormatSdi ( &sdi, szDst, TRUE );
                }
                break;

            case metLabel: {
                    char sz[ cbSymMax ];

                    SHAddrToLabel ( &pdml->addr , sz );
                    sprintf ( szDst, "%s:", sz );
                }
                break;

            default:
                ASSERT ( FALSE );
                break;
        }

		SetAttributes ( pdml );
    }

	// Strip trailing blanks.  This code is written in such a way as
	// to be efficient on MBCS as well as SBCS (avoid calling _ftcsdec)
	for (szSp = NULL, pch = szDst; *pch; pch = _ftcsinc(pch)) {
		if (*pch == ' ') {
			if (szSp == NULL)
				szSp = pch;
		}
		else {
			szSp = NULL;
		}
	}
	if (szSp)
		*szSp = '\0';

    return _ftcslen ( szDst );
}

// Return TRUE if the editmanager should be called!
BOOL CDAMView :: FDoScrollChar(
WPARAM		wParam,
LPARAM *	plParam ) {

    BOOL    fRet = TRUE;
    WORD    cRep = LOWORD( *plParam );  // Save old repeat count!
	int		dyWin = max( (int)WinHeight(), 1 );

	// No rgdml, bail!
	if ( !m_dmi.cdml ) {
		return FALSE;
	}
	
    // Slow things down and don't allow multiple repeat counts!
    *(DWORD *)plParam = (DWORD)MAKELONG( 1, HIWORD( *(DWORD *)plParam ) );

    switch ( wParam ) {

        case VK_UP:

            if ( HIWORD( *plParam ) & KK_CONTROL ) {
				WORD	dob = ObGetCurPos() - ObGetLeft();
				DWORD	doln = OlnGetCurPos() - OlnGetTop();

                if ( !( fRet = !PreDraw ( -1, FALSE ) ) ) {
					doln = min( doln + 1, (DWORD)dyWin - 1 );
					
					SetCursorAt(
						ObGetLeft() + dob,
						OlnGetTop() + doln
					);
				}
            }
            else {
                long    olnCur = (long)OlnGetCurPos();
                long    olnTop = (long)OlnGetTop();

                if ( olnCur - (long)cRep > olnTop ) {
                    *(DWORD *)plParam = (DWORD)MAKELONG( cRep, HIWORD( *plParam ) );
					// Do nothing (fRet = TRUE)
                }
                else if ( olnCur <= olnTop ) {
                    fRet = !PreDraw ( -1, TRUE );
                }
            }
            break;

        case VK_DOWN: {
        	if ( HIWORD( *plParam ) & KK_CONTROL ) {
				WORD	dob = ObGetCurPos() - ObGetLeft();
				DWORD	doln = OlnGetCurPos() - OlnGetTop();

                if ( !( fRet = !PreDraw ( 1, FALSE ) ) ) {
					doln = min( doln + 1, (DWORD)dyWin - 1 );
					
					SetCursorAt(
						ObGetLeft() + dob,
						OlnGetTop() + doln
					);
				}
			}
			else {
    	        long    olnCur = (long)OlnGetCurPos();
		        long	olnTop = (long)OlnGetTop();
		        long    olnMac = olnTop + (long)dyWin - 1L;

		        if ( olnCur + (long)cRep < olnMac ) {
	    	        *(DWORD *)plParam = (DWORD)MAKELONG( cRep, HIWORD( *plParam ) );
	        	}
	            else if ( olnCur >= olnMac ) {
		            fRet = !PreDraw ( 1, TRUE );
				}
    	    }
            break;
        }

        case VK_PRIOR:
            // if it is control - page down then let it
            // go to be handled as a page right --
            // horizontal scroll
            if (!(HIWORD( *plParam ) & KK_CONTROL) ) {
                if ( OlnGetTop () == 0 ) {
                    fRet = FALSE;
                }

                fRet = !PreDraw (
                    -(int)( dyWin - 1 ),
                    TRUE
                );
            }

            break;

        case VK_NEXT:

            // if it is control - page down then let it
            // go to be handled as a page left --
            // horizontal scroll
            if (!(HIWORD( *plParam ) & KK_CONTROL) ) {
                fRet = !PreDraw (
                    dyWin,
                    TRUE
                );
            }

            break;

        case VK_HOME : {

            if ( HIWORD( *plParam ) & KK_CONTROL ) {
                GotoTop ();

                // Now set the cursor at the beginning of the line.
                // Let the key go through as a normal home
                // so the edit manager does the horizontal scroll.

                *(DWORD *)plParam ^= (DWORD)MAKELONG( 0, KK_CONTROL );
            }
            break;
        }

        case VK_END: {

            if ( HIWORD( *plParam ) & KK_CONTROL ) {
                GotoEnd();

                // Now set the cursor at the end of the line.
                // Let the key go through as a normal end
                // so the edit manager does the horizontal scroll.

                *(DWORD *)plParam ^= (DWORD)MAKELONG( 0, KK_CONTROL );
            }
            break;
        }

        case VK_LEFT: {
            //
            // get previous word may ask for the previous line
            // therefore we must trap this and predraw the
            // source window to make sure that requests for
            // lines can be satisfied.
            //

            if ( HIWORD( *plParam ) & KK_CONTROL ) {
				DWORD	 lnTop = OlnGetTop();

				// If the current line is the top and we are
				// at the beginning of the line and not at
				// the top of the window and not at the top
				// of the map, shift the map 1 line and continue
				// the prev word operation.  If any of those
				// conditions fail to be met, then the edit
				// manager will get it correct.

				if ( OlnGetCurPos() == lnTop &&
					ObGetCurPos() == 0 &&
					lnTop != (DWORD)0 &&
					lnTop == m_dmi.lnMapStart
				) {
					PreDraw( -1, FALSE );
					SetCursorAt(
						ObGetCurPos(),
						OlnGetTop() + 1
					);
				}
			}
			else {
            	*(DWORD *)plParam =
					(DWORD)MAKELONG( cRep, HIWORD( *plParam ) );
			}
            break;
        }

        case VK_RIGHT: {

            if ( HIWORD( *plParam ) & KK_CONTROL ) {
				DWORD	oln = OlnGetCurPos();

                if ( oln + 1 >= m_dmi.lnMapStart + m_dmi.cdml &&
                     oln < m_dmi.lnEnd
                ) {
                    PreDraw ( 1, FALSE );
                }
            }
            else {
                *(DWORD *)plParam = (DWORD)MAKELONG( cRep, HIWORD( *plParam ) );
            }
            break;
        }
    }
    return fRet;
}

// Something was dropped onto the Dam window,
// try to update the addresses
BOOL CDAMView::InsertFromDataObject(COleDataObject * data, const CPoint & point)
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
		DAMDropNewAddress(pszExpr, strlen(pszExpr));
		RealGlobalUnlock(hData);
		return TRUE;
	}
	return FALSE;

}

// Return TRUE if the editmanager should be called!
BOOL CDAMView :: FDoVScroll(WPARAM wParam, LPARAM lParam )
{
	BOOL	fUseEM;

	// No rgdml, bail!
	if ( !m_dmi.cdml )
		wParam = WM_NULL;

	switch ( LOWORD( wParam ) ) {
            case SB_LINEUP:
				lParam = MAKELONG( 1, KK_CONTROL );
				fUseEM = FDoScrollChar( VK_UP, &lParam );
                break;

            case SB_LINEDOWN:
				lParam = MAKELONG( 1, KK_CONTROL );
				fUseEM = FDoScrollChar( VK_DOWN, &lParam );
                break;

            case SB_PAGEUP:
                if ( OlnGetTop () == 0 ) {
					fUseEM = FALSE;
                }
				else {
	                fUseEM = !PreDraw (
        	            -(int)( max( (int)WinHeight(), 1 ) ) - 1,
            	        FALSE
	                );
				}
                break;

            case SB_PAGEDOWN:
                fUseEM = !PreDraw (
                    max( WinHeight (), 1 ),
                    FALSE
                );
				break;

            case SB_THUMBPOSITION:
				ASSERT( sizeof( WPARAM ) == sizeof( unsigned long ) );

                ResetMap(
                	OlnFromScrollPos( (short)HIWORD( wParam ) )
                );

                fUseEM = FALSE;
                break;

		default:
			fUseEM = FALSE;
			break;
	}

	return fUseEM;
}

LRESULT CDAMView :: LTextProc(
UINT	wMsg,
WPARAM	wParam,
LPARAM	lParam ) {
	LRESULT	lRet = (DWORD)0;
	BOOL	fCallEM = TRUE;

	// Most of these events need to handle the case where the
	// m_dmi.rgdml (m_dmi.cdml == 0) is NULL.  This is an OOM
	// situation and in most cases, need to fail gracefully!

	switch( wMsg ) {
		case WU_INITDEBUGWIN:
			fCallEM = FALSE;
			lRet = InitEditMgr();
			break;

		case WU_UPDATEDEBUG:
			fCallEM = FALSE;
			UpdateView( (PCXF)lParam );
			break;

		case WU_CLEARDEBUG:
			fCallEM = FALSE;
			RetrySearchFile();
			DestroyMap();
			break;

		case WU_GETCURSORADDR:
			fCallEM = FALSE;
			lRet = (LRESULT)GcafGetCursorAddr( (PGCAINFO)lParam );
			break;

		case WU_UPDATESCROLLBARS:
		case WM_SIZE:
			ResizeMapToWindow();
			break;

		case WM_VSCROLL:
			fCallEM = FDoVScroll( wParam, lParam );
			break;

		case WM_KEYDOWN:
			fCallEM = FDoScrollChar( wParam, &lParam );
			break;

		case WM_COMMAND:
			fCallEM = FALSE;
			ChangeOpts();
			break;

		case WU_UPDATEENC:
			fCallEM = FALSE;
			//RetrySearchFile();
			DestroyMap();
			UpdateView( (PCXF)lParam );
			break;

		default:
			break;
	}

	if ( fCallEM )
		lRet = CMultiEdit::LTextProc( wMsg, wParam, lParam );

	return lRet;
}

BEGIN_POPUP_MENU(Disassembly)
	MENU_ITEM(IDM_GOTO_SOURCE)
	MENU_ITEM(IDM_DEBUG_SHOWIP)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_RUN_TOGGLEBREAK)
	MENU_ITEM(IDM_RUN_ENABLEBREAK)
	MENU_ITEM(IDM_RUN_TOCURSOR)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_RUN_SETNEXTSTMT)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_DBGSHOW_SOURCE)
	MENU_ITEM(IDM_DBGSHOW_CODEBYTES)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_TOGGLE_MDI)
	MENU_ITEM(ID_WINDOW_POPUP_HIDE)
END_POPUP_MENU()


void CDAMView::ShowContextPopupMenu(CPoint pt)
{
    #ifndef NO_CMDBARS
        TRACE2("CBShow: Disassembly (%s, %d)\r\n", __FILE__, __LINE__);
        ::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(Disassembly), pt);
    #else
	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(Disassembly), pt);
    #endif
}
