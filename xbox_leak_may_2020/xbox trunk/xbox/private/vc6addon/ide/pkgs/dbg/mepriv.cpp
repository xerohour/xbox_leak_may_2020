 /**** Multiline Edit Control                                           ****
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1991, Microsoft Corp                                     *
 *                                                                         *
 *  Created: September 16, 1991 by Mark A. Brodsky (as CV4.0: me.c)        *
 *                                                                         *
 *  Purpose: Functional replacement of outdated MASM/CW EDIT_FULLMGR       *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// SUSHI uses child docable windows, treat as such
#define	MakeActive(hwnd)	(::SetFocus(hwnd))
#define FIsActiveWindow(hwnd)	((hwnd)==::GetFocus())

typedef LD *        PLD;
typedef IP *        PIP;
typedef PD *        PPD;

#define cchEditMax  (axMax+2)

BYTE fInsertMode = TRUE;

// Mouse compatability
extern DWORD    lParamMouse;
extern WORD     wParamMouse;

// This macro is used to compare to pip's.  This should be
// much faster than !memcmp( pip1, pip2, sizeof( IP ) )!
#define MEFAreIPsEqual(pip1,pip2)    \
    ( (pip1)->oln == (pip2)->oln && (pip1)->ob == (pip2)->ob )

#define cchDelBack  7
#define idTimer	0x1010


/**** ResizeScroll                                                      ****
 *                                                                         *
 *  PURPOSE: After a window has changed size or line numbers, update the   *
 *           update the scroll bar extents and current value               *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: ResizeScroll() {

	// fAlreadyHere is used to ensure that we don't get recursive.  Set it
	// to TRUE while processing and FALSE when we're done.
	static BOOL	fAlreadyHere = FALSE;

	if ( !fAlreadyHere ) {
		UINT	cln = CLinesInBuf();
        WORD    obMac;
		BOOL	fScrollV = FALSE;
		BOOL 	fScrollH = FALSE;
		RRC		rrc;

		fAlreadyHere = TRUE;

		m_olnMacPrev = (DWORD)cln;

		GetEditRrc( FALSE, &rrc );

		if ( WinHeight() >= cln || !gpISrc->GetSrcEnvironParam(ENV_VSCROLL) )
        {
			if (UseWin4Look())
            {
				SCROLLINFO si;
				si.cbSize = sizeof (si);
				si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
				si.nMin = 0;
				si.nMax = 0;
				si.nPage = rrc.ryBottom - rrc.ryTop;
				si.nPos = 0;
				SetScrollInfo(SB_VERT, &si, TRUE);
			}
			else
				SetScrollRange (SB_VERT,0,0,TRUE);
		}
		else
        {
			DWORD	dwT;

			if ( !rrc.ryBottom )
				++rrc.ryBottom;

			dwT = ( cln + rrc.ryBottom ) / rrc.ryBottom;

			if ( dwT > SHRT_MAX )//|| dwT > (DWORD)WinHeight() )
				m_dScrollVal = 0;
			else
				m_dScrollVal = (int) dwT;

			fScrollV = TRUE;

			if (UseWin4Look()) {   
				SCROLLINFO si;
				si.cbSize = sizeof (si);
				si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS ; 
				si.nMin = 0;
				si.nMax = ScrollFromOln(cln - 1);
				si.nPage = rrc.ryBottom - rrc.ryTop;
				si.nPos = ScrollFromOln(m_ipCur.oln);
				SetScrollInfo(SB_VERT, &si, TRUE);

			}
			else
			{
				SetScrollPos( SB_VERT, ScrollFromOln( m_ipCur.oln ), FALSE );
				SetScrollRange( SB_VERT, 0, ScrollFromOln( cln - 1 ), TRUE );
			}
		}

        obMac = ObMaxBuf();

		if ( WinWidth() >= obMac || !gpISrc->GetSrcEnvironParam(ENV_HSCROLL) )
        {
			if (UseWin4Look())
            {
				SCROLLINFO si;
				si.cbSize = sizeof (si);
				si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
				si.nMin = 0;
				si.nMax = 0;
				si.nPage = rrc.ryBottom - rrc.ryTop;
				si.nPos = 0;
				SetScrollInfo(SB_HORZ, &si, TRUE);
			}
			else
				SetScrollRange (SB_HORZ,0,0,TRUE);
		}
		else
        {
			fScrollH = TRUE;
			if (UseWin4Look())
            {
				SCROLLINFO si;
				si.cbSize = sizeof (si);
				si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
				si.nMin = 0;
				si.nMax = obMac - 1;
				si.nPage = rrc.rxRight - rrc.rxLeft;
				si.nPos = m_pdCur.obleft;
				SetScrollInfo(SB_HORZ, &si, TRUE);
			}
			else
            {
				SetScrollPos( SB_HORZ, m_pdCur.obleft, FALSE );
				SetScrollRange (SB_HORZ,0,obMac - 1,TRUE);
			}
		}

		if (fScrollV != m_fScrollV || fScrollH != m_fScrollH)
        {
			m_fScrollV = fScrollV;
			m_fScrollH = fScrollH;
		}

		fAlreadyHere = FALSE;
	}
}

/**** LGetShiftState                                                    ****
 *                                                                         *
 *  PURPOSE: For character input, get the state of the control and shift   *
 *           keys. Set the return value to have KK_ values OR'd in         *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
long CMultiEdit :: LGetShiftState() {
    long    lRet = 0L;

    if ( GetKeyState( VK_SHIFT ) & 0x8000 ) {
        lRet |= MAKELONG( 0, KK_SHIFT );
     }
    if ( GetKeyState( VK_CONTROL ) & 0x8000 ) {
        lRet |= MAKELONG( 0, KK_CONTROL );
    }
    return lRet;
}

/**** FGetSelection - Get start and end positions of selection          ****
 *                                                                         *
 *  PURPOSE: Determine the beginning and end of a selection.               *
 *                                                                         *
 *  INPUTS:                                                                *
 *     oln      Line to get selection range for                            *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *     pobMic   Pointer to the beginning base offset of a selection        *
 *     pobMac   Pointer to the end base offset of a selection              *
 *                                                                         *
 *      Return Value: TRUE if oln is a "selected" line                     *
 *                                                                         *
 *  IMPLEMENTATION: The selection is obMic <= SELECTION <= obMac.          *
 *                                                                         *
 ***************************************************************************/
BOOL CMultiEdit :: FGetSelection(
DWORD   oln,
WORD *  pobMic,
WORD *  pobMac ) {
    BOOL    fRet = TRUE;
    DWORD   olnMin = min( m_ipAnchor.oln, m_ipCur.oln );;
    DWORD   olnMac = max( m_ipAnchor.oln, m_ipCur.oln );;

	// Column selection, special rules!
    if ( m_fColumnSelect ) {
		if ( ( olnMin <= oln && oln <= olnMac ) &&
			m_ipCur.ob != m_ipAnchor.ob
		) {
            *pobMic = min( m_ipAnchor.ob, m_ipCur.ob );
        	*pobMac = max( m_ipAnchor.ob, m_ipCur.ob ) - 1;

			// Adjust obMic and obMac if DBCS enabled so that
			// the selection doesn't cross DBCS characters
			if ( FDbcsEnabled() )
            {
				IP		ipCurSave = m_ipCur;
				int		iAdjustSave = m_iAdjustDbcs;

				m_ipCur.oln = oln;

				// If the left edge is on the second byte
				// this will adjust the selection to INCLUDE
				// it's lead byte.  This way any partial
				// characters in a selection will be included
				// in the whole
				m_ipCur.ob = *pobMic;
				AdjustDbcsCursor( -1 );
				*pobMic = m_ipCur.ob;

				// If the right edge is on a lead byte,
				// put the cursor on the character after
				// and adjust the cursor.  Then subtract
				// the added 1 back off.  This will include
				// the entire DBCS character as part of the
				// selection.  (Same as above)
				m_ipCur.ob = *pobMac + 1;
				AdjustDbcsCursor( 1 );
				*pobMac = m_ipCur.ob - 1;

				// Restore the real ipCur
				m_ipCur = ipCurSave;
				m_iAdjustDbcs = iAdjustSave;
			}
		}
		else
			fRet = FALSE;
    }

    // If the selection is only on 1 line, the selection is
    // just the limits of the anchor and cursor
    else if ( olnMin == olnMac ) {
		if ( oln != olnMin || m_ipAnchor.ob == m_ipCur.ob ) {
			fRet = FALSE;
		}
		else {
            *pobMic = min( m_ipAnchor.ob, m_ipCur.ob );
        	*pobMac = max( m_ipAnchor.ob, m_ipCur.ob ) - 1;
		}
    }

    // Else if line requested is completely inside of selection, just
    // get the whole line
    else if ( olnMin < oln && oln < olnMac ) {
        *pobMic = 0;
        *pobMac = obMax;
    }

    // Otherwise, we are selecting either the beginning portion of
    // the line or the ending portion or nothing
    else {
        PIP pipMin;
        PIP pipMac;

        // Select which ip is the min and which is the mac
        if ( olnMin == m_ipAnchor.oln ) {
            pipMin = &m_ipAnchor;
            pipMac = &m_ipCur;
        }
        else {
            pipMin = &m_ipCur;
            pipMac = &m_ipAnchor;
        }

        // If we're getting the min line of the selection, then
        // this line will have the end selected
        if ( olnMin == oln ) {
            *pobMic = pipMin->ob;
            *pobMac = obMax;
        }

        // If we're getting the mac, then the beginning is selected
        else if ( olnMac == oln && pipMac->ob ) {
            *pobMic = 0;
            *pobMac = pipMac->ob - 1;
        }

        // Not min or mac or in between, return FALSE (no selection)
        else {
            fRet = FALSE;
        }
    }

	// If the return value is TRUE and the line to check the selection
	// for is greater or equal to the number of lines in the buffer,
	// then the line is out of range, return FALSE;
	if ( fRet && oln >= CLinesInBuf() ) {
		fRet = FALSE;
	}

    return fRet;
}

/**** RedisplaySelection - update the displayed selection based on focus ***
 *                                                                         *
 *  PURPOSE: When the focus is gained or lost, redisplay the selection     *
 *           accordingly                                                   *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: RedisplaySelection() {
    DWORD   olnMin = min( m_ipAnchor.oln, m_ipCur.oln );
    DWORD   olnMac = max( m_ipAnchor.oln, m_ipCur.oln );
	WORD	obMin;
	WORD	obMac;

	if ( FGetSelection( olnMin, &obMin, &obMac ) ||
		FGetSelection( olnMac, &obMin, &obMac ) ) {
		RRC	rrc;

		GetEditRrc( TRUE, &rrc );

		// Convert selection range to visible lines
		if ( olnMin > m_pdCur.olntop ) {
			rrc.ryTop = (RY)( olnMin - m_pdCur.olntop );
		}

		if ( olnMac < m_pdCur.olntop + (DWORD)rrc.ryBottom ) {
			rrc.ryBottom = (RY)( olnMac - m_pdCur.olntop + 1 );
		}

		// Wait for a WM_PAINT!
		InvalidateRrc( &rrc );
	}
}

/**** PasteSelection - copy data from scratchpad into buffer            ****
 *                                                                         *
 *  PURPOSE: Paste data from the scratchpad into the buffer at the current *
 *           position.                                                     *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: PasteSelection() {

    // Delete the current selection
    DeleteSelection();

    // Make sure that there's something in the scratchpad and that
    // we're allowed to alter the line!

    if ( ldEMScratch.cb && !FReadOnlyBuf() ) {
        WORD    cch;
        WORD    cchPaste = ldEMScratch.cb;

        cch = CbGetLineIntoCache( m_ipCur.oln );

        // Make sure that we don't overflow the buffer
        if ( (long)( cch + cchPaste ) < (long)m_pldCur->cbMax ) {

            // If we're beyond the end of the line, just concatentate
            // the line.
            if ( m_ipCur.ob >= cch ) {
                _ftcscat( m_pldCur->prgch, ldEMScratch.prgch );
            }

            // Inserting in the line.  Move the original data over and
            // insert the copy data.
            else {
                char *  pszCur = m_pldCur->prgch + m_ipCur.ob;

                memmove( pszCur + cchPaste, pszCur, cch - m_ipCur.ob + 1 );
                memcpy( pszCur, ldEMScratch.prgch, cchPaste );
            }

            // Put the cursor at the end of the insertion and
            // remove any selection

            m_pldCur->cb += cchPaste;
            m_ipCur.ob += cchPaste;
			m_ipAnchor = m_ipCur;
            if ( !FMoveWindowToCursor(FALSE) ) {
                PaintLine( m_ipCur.oln );
                UpdateCursorPos();
            }
        }
        else {
            MessageBeep( 0 );
        }
    }
}

/**** CopySelection - copy a selection into an buffer with info         ****
 *                                                                         *
 *  PURPOSE: Copy selected text into an LD                                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *		lsz		Output buffer, if null, use clipboard                      *
 *		cbMax	Max size of buffer.  If lsz == NULL and cbMax == -1, then  *
 *				return the allocated handle and don't add to clipboard     *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION: Used by WM_COPY and WM_GETTEXT.  WM_COPY pld will be   *
 *                  the scratchpad LD and WM_GETTEXT will be a temporary   *
 *                  LD which will ultimately copy the data into a lsz      *
 *                                                                         *
 ***************************************************************************/
HANDLE	CMultiEdit :: CopySelection (
char FAR *	lszOut,
size_t		cbMax ) {
	BOOL	fRetHandle = (BOOL)( cbMax == (size_t)-1L );
	HANDLE	hRet = (HANDLE)NULL;

    if ( !MEFAreIPsEqual( &m_ipAnchor, &m_ipCur ) ) {
		DWORD	olnMin;
		DWORD	olnMac;
		DWORD	olnBufMac;
        HANDLE  hMem;
		size_t	cbUsed;

		olnMin = min( m_ipAnchor.oln, m_ipCur.oln );

		// The max line may be out of range during
		// a drag operation

		// Get the number of lines in the buf.  If non-zero
		// then we want the index to the last line, not the count
		if ( olnBufMac = CLinesInBuf() ) {
			--olnBufMac;
		}

		olnMac = min(max( m_ipAnchor.oln, m_ipCur.oln ),olnBufMac);

        hMem = RealGlobalAlloc(GHND, ( olnMac - olnMin + 2 ) * cchEditMax);

        if ( hMem ) {
        	DWORD	oln = olnMin;
            LSZ     lsz = (LSZ)RealGlobalLock( hMem );
            LSZ     lszBegin = lsz;

            do {
	    		char	sz[ cchEditMax ];
    	    	WORD	obMic;
        		WORD	obMac;
				size_t	cchLine;

    			CbGetCachedLine( oln, sizeof( sz ) - 1, sz );
				cchLine = _tcslen( sz );

    	    	if ( FGetSelection( oln, &obMic, &obMac ) ) {
					// must be ">=", not ">", so that if this line has
					// just a cr/lf, we copy that to the clipboard
					if ( cchLine >= obMic ) {
	    				char *	psz;
	    				WORD	cch;

	                    if ( obMac == obMax || cchLine < obMac ||
	                    	( m_fColumnSelect && olnMin != olnMac )
	                    ) {
		                    if ( obMac == obMax || cchLine < obMac ) {
	                        	obMac = cchLine;
							}
							else {
								++obMac;
							}
	                		*( sz + obMac++ ) = '\r';
	                    	*( sz + obMac ) = '\n';
	    			    }
				    	sz[ obMac + 1 ] = '\0';

	    				psz = sz + obMic;
	                    cch = obMac + 1 - obMic;
	                    memcpy( lsz, psz, cch + 1 );
			    		lsz += cch;
					}
                }

            } while( ++oln <= olnMac );

            RealGlobalUnlock( hMem );

            // Free up space not really needed.  Should NEVER fail since
            // we are SHRINKING the Global object
			cbUsed = (size_t)( lsz - lszBegin + 1 );
            hMem = RealGlobalReAlloc( hMem, cbUsed, (UINT)NULL );

            ASSERT( hMem );

            ASSERT( ( olnMac - olnMin + 2 ) * cchEditMax >=
                (DWORD)cbUsed );

			if ( lszOut ) {
				LSZ	lszData = (LSZ)RealGlobalLock( hMem );

				_ftcsncpy( lszOut, lszData, cbMax );
				if ( cbUsed < cbMax ) {
					cbMax = cbUsed;
				}
				*( lszOut + cbMax - 1 ) = '\0';
				RealGlobalUnlock( hMem );
				RealGlobalFree( hMem );
			}
			else if ( fRetHandle ) {
				hRet = hMem;
			}
			else {
				GetDesktopWindow()->OpenClipboard();
	            EmptyClipboard();
    	        SetClipboardData( CF_TEXT, hMem );
				if (m_fColumnSelect)
					SetClipboardData(g_cfColSelect, HandleFromString(_T("C")));
			    CloseClipboard();
			}
        }
	}

	// No selection, copy the current line into the clipboard only
	// if lsz is NULL
	else if ( !lszOut && !fRetHandle ) {
		char	sz[ cchEditMax ];
		WORD	cch;
		HANDLE	hMem;

		CbGetCachedLine( m_ipCur.oln, sizeof( sz ) - 1, sz );
		if ( ( cch = _ftcslen( sz ) ) &&
			( hMem = RealGlobalAlloc( GHND, cch + 3 ) ) ) {

			LSZ	lsz = (LSZ)RealGlobalLock( hMem );

			_ftcscpy( lsz, sz );
            _ftcscat( lsz, "\r\n" );

			RealGlobalUnlock( hMem );

			GetDesktopWindow()->OpenClipboard();
			EmptyClipboard();
			SetClipboardData( CF_TEXT, hMem );
			CloseClipboard();
		}
	}

	return fRetHandle ? hRet : (HANDLE)NULL;
}

/**** DeleteSelection - delete a selection                              ****
 *                                                                         *
 *  PURPOSE: Delete a selection from the buffer.                           *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: DeleteSelection() {

	if ( !FReadOnlyBuf() ) {
	    // Make sure that there's a selection to delete!
	    if ( m_fSelection ) {
	        IP      ip = m_ipCur;
	        WORD    obMic;
	        WORD    obMac;
	        WORD    cch;

	        cch = CbGetLineIntoCache( m_ipAnchor.oln );
	        FGetSelection( m_ipAnchor.oln, &obMic, &obMac );

	        // Make sure that we're deleting REAL text
	        if ( cch > obMic ) {
	            if ( cch < obMac ) {
	                obMac = cch - 1;
	            }

	            // Make sure that we're allowed to delete here.
	            // We only check the beginning of the deletion.  To
	            // do so, we set the cursor to the beginning of the
	            // selection (the buffer owner may look at the ipCur),
	            // and then call the FReadOnlyBuf()

	            m_ipCur.oln = m_ipAnchor.oln;
	            m_ipCur.ob = obMic;

                memmove( m_pldCur->prgch + obMic, m_pldCur->prgch + obMac + 1,
                    cch - obMac + 1 );
                m_pldCur->cb -= obMac - obMic + 1;
	        }

	        // What ever happened, remove the selection and update the
	        // cursor/line as appropriate
	        m_ipAnchor = m_ipCur;

	        if ( !FMoveWindowToCursor( FALSE ) ) {
	            // Don't paint line since the wndproc will force the update
	            // Just update the cursor.
	            UpdateCursorPos();
	        }
	    }
	}
	else {
		MessageBeep( 0 );
	}
}

/**** LcbGetText - Get selected text into a buffer                      ****
 *                                                                         *
 *  PURPOSE: Copy the selected text into a buffer.                         *
 *                                                                         *
 *  INPUTS:                                                                *
 *    cchMax    Maximum size of destination buffer.                        *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *    lsz       far pointer to buffer for selection text.                  *
 *                                                                         *
 *      Return Value: Number of characters put into lsz.                   *
 *                                                                         *
 *  IMPLEMENTATION: Create a temporary LD and get the selection into it.   *
 *                  Then copy string (if there's one) accordingly.         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
long CMultiEdit :: LcbGetText(
WORD        cchMax,
char FAR *  lsz ) {
	ASSERT( lsz );

	// Make sure that the previous selection (or junk text)
	// is destroyed so we don't return a bogus success
	*lsz = '\0';
    CopySelection( lsz, (size_t)cchMax );

    return (long)_ftcslen( lsz );
}

/**** LcbGetWord - Get the current word                                 ****
 *                                                                         *
 *  PURPOSE: Get the word which cursor is on and (optional) coordinates    *
 *           for the selection.                                            *
 *                                                                         *
 *  INPUTS:                                                                *
 *    cchMax  Maximum number of characters which can be put in lsz         *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *    lsz     far pointer to write "word" to                               *
 *    pobMic  pointer to beginning base offset of selection                *
 *    pobMac  pointer to end base offset of selection                      *
 *                                                                         *
 *      Return Value: Number of characters in lsz.                         *
 *                                                                         *
 *  IMPLEMENTATION: If pobMic (or pobMac) is non-zero, then we want the    *
 *                  word we're actually sitting on.  Otherwise, we want    *
 *                  either the word we're sitting on OR the next word on   *
 *                  the line.                                              *
 *                                                                         *
 ***************************************************************************/
long CMultiEdit :: LcbGetWord(
UINT		cchMax,
char FAR *  lsz,
WORD *      pobMic,
WORD *      pobMac ) {
    char    sz[ cchEditMax];
    char *  szPlus1 = &sz[1];
    WORD    cch;
    WORD    obMic = 0;
    WORD    obMac = 0;
    char *  pch;

    cch = CbGetCachedLine( m_ipCur.oln, sizeof( sz ) - 1, szPlus1 );

    // To get a word:
    //   1) There must be something on the line.
    //   2) The cursor must be inside the line.
    //   3) If we want the current word only and on a non-white-space
    //      character.
    //   4) pobMic and pobMac are zero then try the next word.

    if ( cch && m_ipCur.ob < cch && ( !pobMic || !pobMac ||
        !_istspace((unsigned char) *( szPlus1 + m_ipCur.ob ) ) ) ) {

        // This lets us go backwards w/o checking a counter.
        *sz = '\0';
        pch = szPlus1 + m_ipCur.ob;

        // Skip over white spaces.
        if ( _istspace((unsigned char) *pch ) ) {
            while( *pch && _istspace( (unsigned char)*pch ) ) {
                ++pch;
            }
        }

        // Not on a white space, so back up to beginning of the current word
        else if ( FIsWordChar( *pch ) ) {
            while( *( pch - 1 ) && FIsWordChar( *( pch - 1 ) ) ) {
                --pch;
            }
        }

        // If we're on at the end of the string, mark the beginning of
        // the word
        if ( *pch ) {
            obMic = (WORD)( pch - szPlus1 );
        }

        // Now find the end of the word.
        while( *pch && FIsWordChar( *pch ) ) {
            ++pch;
        }

        // mark the end
        obMac = pch - szPlus1;

        // Put a zero after the word so we can do a _ftcsncpy with
        // a max value of the lsz buffer max.
        *pch = '\0';
        _ftcsncpy( lsz, szPlus1 + obMic, cchMax );

        // If the buffer's too small, put a zero at its end and
        // adjust obMic and obMac so that the return value is cchMax
		if ( (UINT) (obMac - obMic) >= cchMax ) {
            *( lsz + cchMax - 1 ) = '\0';
            obMic = 0;
            obMac = cchMax;
        }
    }

    // Don't update the data if we've go no where to put it
    if ( pobMic ) {
        *pobMic = obMic;
    }
    if ( pobMac ) {
        *pobMac = obMac;
    }

    return (long)( obMac - obMic );
}

/**** GetWordAtPostion - Given a line number and column no, get the     ****
 *                       get the word at that position                     *
 *                                                                         *
 ***************************************************************************/

 // FUTURE : This function is very similar to LcbGetWord, except for the weird meaning
 // of the pobMic and pobMac being passed in as NULL to LcbGetWord, which I don't understand
 // We should just make LcbGetWowrd call GetWordAtPosition [ sanjays ].

 BOOL CMultiEdit::GetWordAtPosition(
 WORD ob,
 DWORD oln,
 LPSTR lsz,
 UINT cchMax,
 WORD *pobMic,
 WORD *pobMac ) {
	char sz[cchEditMax];
	char *szPlus1 = &sz[1];
	WORD cch;
	WORD obMic = 0;
	WORD obMac = 0;
	char *pch;

	cch = CbGetCachedLine(oln, sizeof(sz) - 1, szPlus1);

	if (cch && !_istspace((unsigned char)* (szPlus1 + ob ))) {
		
		// This lets us go backwards w/o checking a counter.
		*sz = '\0';
		pch = szPlus1 + ob;
		
		if ( FIsWordChar ( *pch ) ) {
			while ( *(pch - 1) && FIsWordChar( *( pch - 1 ) ) ) {
				--pch ;
			}
		}

        // If we're on at the end of the string, mark the beginning of
        // the word
        if ( *pch ) {
            obMic = (WORD)( pch - szPlus1 );
        }

        // Now find the end of the word.
        while( *pch && FIsWordChar( *pch ) ) {
            ++pch;
        }

        // mark the end
        obMac = pch - szPlus1;

        // Put a zero after the word so we can do a _ftcsncpy with
        // a max value of the lsz buffer max.
        *pch = '\0';
        _ftcsncpy( lsz, szPlus1 + obMic, cchMax );

        // If the buffer's too small, put a zero at its end and
        // adjust obMic and obMac so that the return value is cchMax
		if ( (UINT) (obMac - obMic) >= cchMax ) {
            *( lsz + cchMax - 1 ) = '\0';
            obMic = 0;
            obMac = cchMax;
        }
    }

    // Don't update the data if we've go no where to put it
    if ( pobMic ) {
        *pobMic = obMic;
    }
    if ( pobMac ) {
        *pobMac = obMac;
    }

    return (( obMac - obMic ) != 0);
}


 	
/**** UpdateAnchor - Update the anchor to new cursor position           ****
 *                                                                         *
 *  PURPOSE: This function is just to shrink code.  This will be done      *
 *           a lot.  We don't want to update if the shift key was down.    *
 *                                                                         *
 *  INPUTS:                                                                *
 *    lParam   long arg for WM_CHAR and mouse messages.  Contains shift    *
 *             key state.                                                  *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: UpdateAnchor(
long   lParam ) {

    if ( !( HIWORD( lParam ) & KK_SHIFT ) ) {
	    m_ipAnchor = m_ipCur;
		m_fColumnSelect = FALSE;
    }

	// Update the selection status
	m_fSelection = !MEFAreIPsEqual( &m_ipAnchor, &m_ipCur );

    // If we haven't initialized m_ipAnchorOld and m_ipCurOld do it
    // and don't bother trying to repaing non-existant selections.  Otherwise,
    // we need to repaint if there was an old selection or there is a new selection

    if ( m_ipAnchorOld.ob != 0xffff &&
    	( !MEFAreIPsEqual( &m_ipAnchorOld, &m_ipCurOld ) ||
    	m_fSelection )
    ) {

    	DWORD	olnSelMin;
	    DWORD	olnSelMac;
    	RRC		rrc;
		RY		ryBottomOld;

		// If the anchor didn't change, then only need to repaint
		// the lines from the old cursor to the new cursor
		olnSelMin = min( m_ipCur.oln, m_ipCurOld.oln );
		olnSelMac = max( m_ipCur.oln, m_ipCurOld.oln );

		// If the anchor changed, then we need to include the old
		// anchor range in the repaint range
		if ( !MEFAreIPsEqual( &m_ipAnchor, &m_ipAnchorOld ) ||
			( m_fColumnSelect && m_ipCur.ob != m_ipCurOld.ob )
		) {
			olnSelMin = min( min( olnSelMin, m_ipAnchor.oln ), m_ipAnchorOld.oln );
			olnSelMac = max( max( olnSelMac, m_ipAnchor.oln ), m_ipAnchorOld.oln );
		}

    	GetEditRrc( TRUE, &rrc );
		ryBottomOld = rrc.ryBottom;

		// Convert the line numbers to window rrc coordinates IF
		// they are still visible w/i the window
	    if ( olnSelMin > m_pdCur.olntop &&
			olnSelMin < m_pdCur.olntop + ryBottomOld
		) {
		   	rrc.ryTop = (RY)( olnSelMin - m_pdCur.olntop );
    	}

		if ( m_pdCur.olntop <= olnSelMac &&
			olnSelMac < m_pdCur.olntop + ryBottomOld + 1
		) {
			rrc.ryBottom = (RY)( olnSelMac - m_pdCur.olntop + 1 );
			if ( rrc.ryBottom > ryBottomOld ) {
	    		rrc.ryBottom = ryBottomOld;
		    }
		}

		// Finally, repaint the old garbage
    	Paint( &rrc, NULL );
    }
    m_ipAnchorOld = m_ipAnchor;
    m_ipCurOld = m_ipCur;
}

/**** CbGetCachedLine - Get a line from callback with cache override    ****
 *                                                                         *
 *  PURPOSE: Get a specific line from the buffer.  If the line requested   *
 *           line is cached, return the cached version and by-pass the     *
 *           callback to the owner of the buffer.                          *
 *                                                                         *
 *  INPUTS:                                                                *
 *     oln      Line number to get                                         *
 *     cchMax   Maximum number of characters in the destination buffer,    *
 *				not including the trailing \0							   *			
 *                                                                         *
 *  OUTPUTS:                                                               *
 *     sz       Pointer to buffer for string.                              *
 *                                                                         *
 *      Return Value: Number of characters in the returned(filled) buffer. *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
WORD CMultiEdit :: CbGetCachedLine(
DWORD   oln,
WORD    cchMax,
char *  sz ) {
    WORD    cch = 0;

#ifdef DEBUG
	sz[cchMax] = '\0';  // Make sure the buffer is big enough.
#endif

    // Only get cached line if the buffer is cached and the requested line
    // is the cached one.
    if ( m_pldCur->flags && oln == m_pdCur.oln ) {
        _ftcscpy( sz, m_pldCur->prgch );
        cch = m_pldCur->cb;
    }
    else if ( oln < CLinesInBuf() ) {
		PDLA	pdla;

        cch = CbGetLineBuf( oln, cchMax, sz, pdla );
    }
	else {
		*sz = '\0';
		cch = 0;
	}

    return cch;
}

/**** CbGetLineIntoCache - Get a line and put into cache for edit       ****
 *                                                                         *
 *  PURPOSE: Copy the requested line into the cache                        *
 *                                                                         *
 *  INPUTS:                                                                *
 *     oln  Line number to put into cache.                                 *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: Number of characters in cached line                  *
 *                                                                         *
 *  IMPLEMENTATION: If there's a cached line already, return the _ftcslen. *
 *                  If not, cache the line.                                *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
WORD CMultiEdit :: CbGetLineIntoCache(
DWORD    oln ) {
    WORD    cch = 0;

    if ( !m_pldCur->flags ) {
		PDLA	pdla;

        cch = CbGetLineBuf( oln, m_pldCur->cbMax, m_pldCur->prgch, pdla );
        m_pldCur->flags = TRUE;
        m_pldCur->cb = cch;
        m_pdCur.oln = oln;
    }
    else {
        cch = m_pldCur->cb;
    }

    return cch;
}

/**** FlushCache - Flush the cached/edited line                         ****
 *                                                                         *
 *  PURPOSE: Flush the cache.  If there is one.  Notify the owner of the   *
 *           the buffer that its contents have changed and clear out the   *
 *           cache info.                                                   *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: FlushCache() {
    if ( m_pldCur->flags && m_pdCur.oln != olnMax ) {
        DWORD    olnCache;

        FReplaceLineBuf( m_pdCur.oln, m_pldCur->cb,
           m_pldCur->prgch );

        m_pldCur->flags = FALSE;
        m_pldCur->cb = 0;
        *m_pldCur->prgch = '\0';
        olnCache = m_pdCur.oln;
        m_pdCur.oln = olnMax;

        // Repaint the line (the owner may have different attributes!
        PaintLine( olnCache );
    }
}

/**** GetEditRrc - Get the bounding rrc for the current window          ****
 *                                                                         *
 *  PURPOSE: Return the bounding rectanngle for the current edit window.   *
 *                                                                         *
 *  INPUTS:                                                                *
 *     fAll TRUE = Get EditRrc for entire window, FALSE = Get area where   *
 *          cursor positions are valid                                     *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *     prrc  -  rectangle is filled with bounding RRC for window           *
 *                                                                         *
 *      Return Value: None                                                 *
 *                                                                         *
 *  IMPLEMENTATION: There is a CLOSE match between this and GetClientRrc() *
 *                  but we completely ignore the WS_ flags since we KNOW   *
 *                  that the window is NOT obscurred by a scrollbar.       *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void
CMultiEdit::GetEditRrc(
	BOOL    fAll,
	PRRC    prrc
	)
{
    RECT    rect;

    GetClientRect( &rect );

	rect.right -= m_cxSelectMargin;

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

	int tmAveCharWidth = m_pFontInfo->m_tm.tmAveCharWidth;
	int tmHeight = m_pFontInfo->m_tm.tmHeight;

    prrc->rxLeft = 0;
	prrc->rxRight = (RX)( ( rect.right - rect.left ) / tmAveCharWidth ) + 1;
    prrc->ryTop = 0;
    prrc->ryBottom = (RY)( ( rect.bottom - rect.top ) / tmHeight ) + 1;

    if ( !fAll ) {
		if ( prrc->rxRight * tmAveCharWidth > rect.right ) {
			--prrc->rxRight;
		}
    	if ( prrc->ryBottom * tmHeight > rect.bottom ) {
        	--prrc->ryBottom;
		}
    }
}


/**** DrawAttrText - Draw a single line of text with attributes         ****
 *                                                                         *
 *  PURPOSE: Draw a specific line of the window with passed in attributes. *
 *                                                                         *
 *  INPUTS:                                                                *
 *    pdc      Device context for drawing                                  *
 *    fActive  Does the window have the focus?                             *
 *    oln      Line number being painted (for selection)                   *
 *    sz       String to display                                           *
 *    cch      Strlen of sz                                                *
 *    cchSzMax sizeof(sz)                                                  *
 *    pdla     pointer to first element of LINE_ATTRIBUTE information      *
 *    ry       Relative "y" position to display text at.                   *
 *    rxMax    Number of chars which are visible per line.  This is done   *
 *             by caller to avoid overhead of GetEditRrc for each painted  *
 *             line.                                                       *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: Node.                                                *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: DrawAttrText(
CDC *	pdc,
BOOL	fActive,
DWORD	oln,
char *  sz,
WORD    cch,
WORD	cchSzMax,
PDLA    pdla,
RY		ry,
WORD    rxMax ) {
	RX	    	rx;
	RX	    	rxMin;
	WORD    	cchAttr;
	char *  	szEnd;
	WORD    	cchLine;
	WORD    	obSelMic;
	WORD    	obSelMac;
	BOOL    	fSel;
	char *  	szBegin = sz;
	int			rxPaint;
	CBrush		brush;
	LOGBRUSH	lgbr;
	RECT		rectClient;
	int			tmHeight = m_pFontInfo->m_tm.tmHeight;
	int			tmAveCharWidth = m_pFontInfo->m_tm.tmAveCharWidth;

	GetClientRect( &rectClient );
	rectClient.left += m_cxSelectMargin;

	rx = 0;
	rxMin = (RX)m_pdCur.obleft;

	lgbr.lbStyle = BS_SOLID;
	FMT_ELEMENT *rgfmtel = pfmtelFromDocType(m_dt);

	lgbr.lbColor = rgfmtel[FMTEL_TEXT].rgbBackground;
	lgbr.lbHatch = 0L;
	brush.CreateBrushIndirect( &lgbr );

	// Clear out the margin
	if ( m_cxSelectMargin ) {
		RECT	rect;

		rect.left =  0;
		rect.right = m_cxSelectMargin;
		rect.top = ry * tmHeight;
		rect.bottom = rect.top + tmHeight;

		pdc->FillRect( &rect, &brush );
	}

	// Clear out the trailing portion of the line which is out of the
	// line limit length
	if ( ( cchLine = rxMax + rxMin ) > cchSzMax ) {
		RECT	rect;

		rect.left =  ( rxMax - rxMin ) * tmAveCharWidth;
		rect.right = rxMax * tmAveCharWidth;
		rect.top = ry * tmHeight;
		rect.bottom = rect.top + tmHeight;

		pdc->FillRect( &rect, &brush );

		cchLine = cchSzMax;
	}

	brush.DeleteObject();

	// Pad string with spaces up.  This is done so the end of the
	// line gets "filled" with spaces

	if ( cch < cchLine ) {
		memset( sz + cch, ' ', cchLine - cch + 1 );
	}

	// Zero terminate the string at the last visible character for the line
	sz[ cchLine ] = '\0';

	// Skip over attributes which are clipped by the left border et al
	// and set the cchAttr to be the amount left over (still visible)
	cchAttr = pdla->cb;

	rxPaint = -(int)m_pdCur.obleft;

	// Reset the base (left margin) to the beginning
	// since we will let windows clip
	rxMin = 0;
	szEnd = sz + cchLine;
	sz += rxMin;

	// Get the selection range( don't paint selections if we're not
	// active
	fSel = fActive && FGetSelection( oln, &obSelMic, &obSelMac );

	// The obSelMac is the end point (inclusive).  If it is not
	// obMax, increment it so it is just beyond the endpoint
	if ( obSelMac != obMax ) {
		++obSelMac;
	}

	while( sz < szEnd ) {
		WORD cchT = cchAttr;

		if ( cchT == obMax ) {
			cchT = _ftcslen( sz );
		}

		if ( fSel ) {
			WORD    obAttrMic = sz - szBegin;
			WORD    obAttrMac = sz - szBegin;

			// If the cch is obMax set it to the real end of the string
			if ( cchAttr == obMax ) {
				obAttrMac += cchT;
			}

			// Otherwise, set it to the end of the portion with the
			// attribute
			else {
				obAttrMac += cchAttr;
			}

			// Selection not part of this portion, just display it
			if ( obAttrMic > obSelMac || obAttrMac < obSelMic ) {
				pdc->SetTextColor(pdla->pElement->rgbText);
				pdc->SetBkColor(pdla->pElement->rgbBackground);

				pdc->ExtTextOut(
					rxPaint * tmAveCharWidth + m_cxSelectMargin,
					ry * tmHeight,
					ETO_CLIPPED,
					&rectClient,
					sz,
					cchT,
					NULL
				);
			}
			else {
				WORD    db;
				int     rxT = rxPaint;
				char *  szT = sz;

				// Possible regular display before selection
				if ( obAttrMic < obSelMic ) {
					db = min( obAttrMac, obSelMic ) - obAttrMic;

					pdc->SetTextColor(pdla->pElement->rgbText);
					pdc->SetBkColor(pdla->pElement->rgbBackground);

					pdc->ExtTextOut(
						rxT * tmAveCharWidth + m_cxSelectMargin,
						ry * tmHeight,
						ETO_CLIPPED,
						&rectClient,
						szT,
						db,
						NULL
					);

					szT += db;
					rxT += db;
					obAttrMic += db;
				}

				// Selection
				if ( obSelMic < obAttrMac ) {
					db = min( obSelMac, obAttrMac ) - obAttrMic;
					pdc->SetTextColor(rgfmtel[FMTEL_SELC].rgbText);
					pdc->SetBkColor(rgfmtel[FMTEL_SELC].rgbBackground);

					pdc->ExtTextOut(
						rxT * tmAveCharWidth + m_cxSelectMargin,
						ry * tmHeight,
						ETO_CLIPPED,
						&rectClient,
						szT,
						db,
						NULL
					);

					szT += db;
					rxT += db;
					obAttrMic += db;
				}

				// Ending portion (regular)
				if ( obSelMac < obAttrMac ) {
					db = obAttrMac - obAttrMic;

					pdc->SetTextColor(pdla->pElement->rgbText);
					pdc->SetBkColor(pdla->pElement->rgbBackground);

					pdc->ExtTextOut(
						rxT * tmAveCharWidth + m_cxSelectMargin,
						ry * tmHeight,
						ETO_CLIPPED,
						&rectClient,
						szT,
						db,
						NULL
					);
				}
			}
		}
		else {
			// No selection on this line, just regular output
			pdc->SetTextColor(pdla->pElement->rgbText);
			pdc->SetBkColor(pdla->pElement->rgbBackground);

			pdc->ExtTextOut(
				rxPaint * tmAveCharWidth + m_cxSelectMargin,
				ry * tmHeight,
				ETO_CLIPPED,
				&rectClient,
				sz,
				cchT,
				NULL
			);
		}

		// If there's more, update position, string, and attribute info
		if ( cchAttr <= cch ) {
			rxPaint += (int)cchT;
			if ( cchAttr == obMax ) {
				sz = szEnd;
			}
			else {
				sz += cchAttr;
			}
			++pdla;
			cchAttr = pdla->cb;
		}

		// Othewise, get out
		else {
			sz = szEnd;
		}
    }
}

/****UpdateCursorPos - Move the cursor to the current location          ****
 *                                                                         *
 *  PURPOSE: Move the cursor to the current location, provided to make     *
 *           code SMALLER!                                                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: UpdateCursorPos() {
    if ( !( m_Style & ES_NOREDRAW ) && FIsActiveWindow( m_hWnd ) ) {
		POINT	pt;

		pt.x =
			(int)( m_ipCur.ob - m_pdCur.obleft ) * m_pFontInfo->m_tm.tmAveCharWidth + m_cxSelectMargin;

		pt.y =
            (int)( m_ipCur.oln - m_pdCur.olntop ) * m_pFontInfo->m_tm.tmHeight;

        SetCaretPos( pt );

		imeMoveConvertWin( m_hWnd, pt.x, pt.y );
    }
}

/**** FMoveWindowToCursor - Scroll Window if necessary                  ****
 *                                                                         *
 *  PURPOSE: To ensure that cursor is contained within a window.  If the   *
 *           cursor has been moved outside the visible rrc of the window,  *
 *           position the WINDOW so that the cursor is at the closest      *
 *           border to where is was scrolled and that the text is moved    *
 *           appropriately.  Otherwise, just position the cursor.          *
 *                                                                         *
 *  INPUTS:                                                                *
 *      fKeepAnchor If TRUE pass KK_SHIFT to scroll routines to keep the   *
 *                  anchor from being reset                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: TRUE if the window was scrolled/repainted, else FALSE*
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
BOOL CMultiEdit :: FMoveWindowToCursor(
BOOL    fKeepAnchor ) {
    RRC     rrc;
    BOOL    fUpdate = TRUE;
    BOOL    fScrolled = FALSE;
    long    lParam = ( fKeepAnchor ? MAKELONG( 0, KK_SHIFT ) : 0L );

    GetEditRrc( FALSE, &rrc );

	// If the bottom line is zero, then the window is displaying part
	// of 1 line.  To ensure that we don't scroll (erroneously), set
	// the window height to 1
	if ( !rrc.ryBottom ) {
		++rrc.ryBottom;
	}

    // Scrolled off of the LEFT border
    if ( m_ipCur.ob < m_pdCur.obleft ) {
        m_pdCur.obleft = m_ipCur.ob;
    }

    // Cursor off of RIGHT
    else if ( m_ipCur.ob > m_pdCur.obleft + (WORD)rrc.rxRight - 1 ) {
        m_pdCur.obleft = 1 + m_ipCur.ob - (WORD)rrc.rxRight;
    }

    // Cursor scrolled 1 line UP, so be smart and do a real scroll
    else if ( m_ipCur.oln + 1 == m_pdCur.olntop ) {
        VScroll( SB_LINEUP, lParam );
        fUpdate = FALSE;
        fScrolled = TRUE;
    }

    // Cursor way off (>1 line) TOP, reposition
    else if ( m_ipCur.oln < m_pdCur.olntop ) {
        m_pdCur.olntop = m_ipCur.oln;
    }

    // Cursor scrolled 1 line DOWN, be smart...
    else if ( m_ipCur.oln == m_pdCur.olntop + (WORD)rrc.ryBottom ) {
        VScroll( SB_LINEDOWN, lParam );
        fUpdate = FALSE;
        fScrolled = TRUE;
    }

    // Cursor way off(>1 line) DOWN, reposition
    else if ( m_ipCur.oln > m_pdCur.olntop + (WORD)rrc.ryBottom - 1 ) {
        m_pdCur.olntop = 1 + m_ipCur.oln - (WORD)rrc.ryBottom;
    }

    // Cursor somewhere on screen, don't repaint anything!
    else {
        fUpdate = FALSE;
    }

    // If fUpdate then we must do a repaint of the screen
    if ( fUpdate ) {
        Paint( (PRRC)0, NULL );
    }

    // Always reposition the cursor (inexpensive, so why not!)
    UpdateCursorPos();

    return fUpdate || fScrolled;
}

/**** ValidateCursor - Move cursor into client area if outside          ****
 *                                                                         *
 *  PURPOSE: Make sure that cursor has not "moved" off of rrc when we      *
 *           we paint.  This may have happened by resizing the window.     *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION: Pretty lame.                                           *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: ValidateCursor() {
    RRC     rrc;
    DWORD   olntop = m_pdCur.olntop;
    WORD    obLeft = (WORD)m_pdCur.obleft;
    BOOL    fUpdate = FALSE;

    GetEditRrc( FALSE, &rrc );

    if ( m_ipCur.oln > olntop + (DWORD)rrc.ryBottom ) {
        m_ipCur.oln = olntop;
        fUpdate = TRUE;
    }

    if ( (WORD)m_ipCur.ob > obLeft + (WORD)rrc.rxRight ) {
        m_ipCur.ob = obLeft;
        fUpdate = TRUE;
    }

    if ( fUpdate ) {
		// Throw out the selection
		UpdateAnchor( 0L );

        UpdateCursorPos();
    }
}

/**** Paint - Draw a window                                             ****
 *                                                                         *
 *  PURPOSE: Paint the whole or a part of the current window.              *
 *                                                                         *
 *  INPUTS:                                                                *
 *      prrc   If non-null, specifies which part of the window to repaint. *
 *             Else, paint the whole window.                               *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: Number of lines in the buffer (CLinesInBuf())        *
 *                                                                         *
 *  IMPLEMENTATION: Fairly straight foward                                 *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
DWORD CMultiEdit :: Paint(
PRRC    prrc,
CDC *   pdc ) {
    DWORD   cLineMax;
    DWORD   dyWin;
    WORD    dxWin;
    DWORD   iln;
    char *   sz;
    DLA     dla;
    PDLA    pdla;
    RY	    ry;
    RY	    ryTop;
    RRC     rrc;
    DWORD   olnMac = CLinesInBuf();
    BOOL    fCached = m_pldCur->flags;
    WORD    cch;
    DWORD   olnAnchor = m_ipAnchor.oln;
    BOOL	fActive = FIsActiveWindow( m_hWnd );
    BOOL    fReleaseDC = FALSE;
	BOOL	fShowSel;
	WORD	cbsz;
	FMT_ELEMENT *rgfmtel = pfmtelFromDocType(m_dt);

	sz = (char *)alloca( cbsz = max( cchEditMax + 2, WinWidth() + 2 ) );

	fShowSel = fActive;

    if ( !( m_Style & ES_NOREDRAW ) ) {
        if ( !pdc ) {
            pdc = GetDC();
            fReleaseDC = TRUE;
        }

		pdc->SelectObject( m_pFontInfo->m_hFont );

        if ( fActive ) {
            HideCaret();
        }

        GetEditRrc( TRUE, &rrc );

        ++rrc.ryBottom;

        ++rrc.rxRight;

		  // For DBCS builds, we want to paint an extra
		  // character to ensure that a clipped DBCS character
		  // gets the non-clipped portion correct
        dxWin = (WORD)rrc.rxRight + 1;

        iln = m_pdCur.olntop;

        if ( prrc ) {
            ry = prrc->ryTop;
            dyWin = prrc->ryBottom - ry;
            iln += prrc->ryTop;
        }
        else {
            ry = 0;
            dyWin = (WORD)rrc.ryBottom;
        }
        ryTop = ry;

        cLineMax = min( olnMac, iln + dyWin );

        for( ; iln < cLineMax; ++iln, ++ry ) {
			RECT	rect;

			// If the display line is cached, create
			// an attribute for it
		    if ( fCached && iln == m_pdCur.oln ) {
                _ftcscpy( sz, m_pldCur->prgch );
                cch = m_pldCur->cb;
			  	dla.pElement = &(rgfmtel[FMTEL_TEXT]);
                dla.cb = obMax;
                pdla = &dla;
            }
            else {
                *sz = '\0';
                CbGetLineBuf( iln, cbsz, sz, pdla );
                cch = _ftcslen( sz );
            }
            DrawAttrText( pdc, fShowSel, iln, sz, cch, cbsz, pdla, ry, dxWin );

			rect.left = 0;
			rect.right = rect.left + m_cxSelectMargin;
			rect.top = ry * m_pFontInfo->m_tm.tmHeight;
			rect.bottom = rect.top + m_pFontInfo->m_tm.tmHeight;
			DrawLineGraphics( pdc, rect, iln );
		}

        // We may need to fill in the bottom of the window since the
        // text does not fill the window up.
        rrc.ryTop = (RY)ry;
        rrc.ryBottom = ryTop + (RY)dyWin ;
        if ( rrc.ryBottom - rrc.ryTop ) {
              RECT      rect;
              LOGBRUSH  lgbr;
			  CBrush	brush;

			  lgbr.lbStyle = BS_SOLID;
			  lgbr.lbColor = rgfmtel[FMTEL_TEXT].rgbBackground;
			  lgbr.lbHatch = 0L;

              brush.CreateBrushIndirect( &lgbr );

              rect.left = 0;
              rect.right = rrc.rxRight * m_pFontInfo->m_tm.tmAveCharWidth + m_cxSelectMargin;
              rect.top = rrc.ryTop * m_pFontInfo->m_tm.tmHeight;
              rect.bottom = ( rrc.ryBottom + 1 ) * m_pFontInfo->m_tm.tmHeight;
              pdc->FillRect( &rect, &brush );

              brush.DeleteObject();
        }


        ValidateCursor();

        if ( fActive ) {
            ShowCaret();
        }

        if ( fReleaseDC ) {
            ReleaseDC( pdc );
        }
    }
    return olnMac;
}

/**** VScroll - Perform VERTICAL scrolling                              ****
 *                                                                         *
 *  PURPOSE: Perform VERTICAL window scrolling                             *
 *                                                                         *
 *  INPUTS:                                                                *
 *     wScrollType  Type of scroll to perform                              *
 *     lParam       Scroll specific information (ala CW spec)              *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: VScroll(
UINT   wScrollType,
long   lParam ) {
    WORD    dyWin;
    WORD    dxWin;
    RRC     rrc;
    DWORD   olnMac = CLinesInBuf();
	RECT	r;

	GetClientRect ( &r );
    GetEditRrc( FALSE, &rrc );
    dyWin = max( (WORD)rrc.ryBottom, 1 );
    dxWin = (WORD)rrc.rxRight;

    switch( wScrollType ) {
        case SB_LINEUP:
            if ( m_pdCur.olntop ) {
				BOOL	fUpdateAnchor = FALSE;

                --m_pdCur.olntop;

                // If the cursor is now below the bottom of the
                // screen, move it to the bottom-most visible line
                if ( m_pdCur.olntop + (DWORD)dyWin == m_ipCur.oln ) {
                    --m_ipCur.oln;
					fUpdateAnchor = TRUE;
                }

				ScrollWindow( 0, m_pFontInfo->m_tm.tmHeight, NULL, NULL );
				AdjustDbcsCursor( m_iAdjustDbcs );
                UpdateCursorPos();
				if ( fUpdateAnchor ) {
					UpdateAnchor( 0L );
				}
                UpdateWindow();
            }
            break;

        case SB_LINEDOWN:

            // Make sure that we keep the buffer full by not
            // scrolling the last line from the bottom (unless
            // the window is larger in height than the file!
            if ( m_pdCur.olntop + 1L < CLinesInBuf() ) {
				BOOL	fUpdateAnchor = FALSE;

                ++m_pdCur.olntop;
                if ( m_pdCur.olntop - 1 == m_ipCur.oln ) {
                    ++m_ipCur.oln;
					fUpdateAnchor = TRUE;
                }
				ScrollWindow( 0, -m_pFontInfo->m_tm.tmHeight, NULL, NULL );
				AdjustDbcsCursor( m_iAdjustDbcs );
                UpdateCursorPos();
				if ( fUpdateAnchor ) {
					UpdateAnchor( 0L );
				}
                UpdateWindow();
            }
            break;

        case SB_PAGEUP: {
            DWORD   olntopNew;

            if ( m_pdCur.olntop <= (DWORD)dyWin ) {
                olntopNew = 0;
                m_ipCur.oln -= m_pdCur.olntop;
            }
            else {
				--dyWin;
                olntopNew = m_pdCur.olntop - (DWORD)dyWin;
                m_ipCur.oln -= (DWORD)dyWin;
            }
            m_pdCur.olntop = olntopNew;
			AdjustDbcsCursor( m_iAdjustDbcs );
            UpdateAnchor( lParam );
			if ( !FMoveWindowToCursor( TRUE ) ) {
            	Paint( (PRRC)0, NULL );
			}
            break;
        }

        case SB_PAGEDOWN: {
            DWORD   olntopNew;
            BOOL    fUpdate = FALSE;

            // We want to keep the cursor in the same
            // position within the window after the pagedown.
            if ( m_pdCur.olntop + (DWORD)dyWin < olnMac ) {
                olntopNew = m_pdCur.olntop + (DWORD)dyWin - 1;
				m_ipCur.oln += (DWORD)dyWin - 1;
				if ( m_ipCur.oln >= olnMac ) {
					m_ipCur.oln = olnMac - 1;
				}
                fUpdate = TRUE;
            }

			// If the page down causes us to go beyond the
			// end of file, position the cursor on the last
			// line of the file (WinWord 2.0 style).
			else if ( m_ipCur.oln != olnMac - 1 ) {
				olntopNew = m_pdCur.olntop;
				m_ipCur.oln = olnMac - 1;
				fUpdate = TRUE;
			}

            // Don't repaint if they pagedown'ed when already at the bottom
            if ( fUpdate ) {
                m_pdCur.olntop = olntopNew;
				AdjustDbcsCursor( m_iAdjustDbcs );
            	UpdateAnchor( lParam );
				if ( !FMoveWindowToCursor( TRUE ) ) {
    	        	Paint( (PRRC)0, NULL );
				}
            }
            break;
        }

        case SB_THUMBPOSITION:
			if (
				lParam <= (long)ScrollFromOln ( olnMac  ) &&
				lParam != (long)ScrollFromOln ( m_pdCur.olntop )
			) {
				m_ipCur.oln =
					OlnFromScrollPos ( (short)LOWORD( lParam ) ) +
					( m_ipCur.oln - m_pdCur.olntop );
                m_pdCur.olntop = OlnFromScrollPos ( (short)LOWORD( lParam ) );

				AdjustDbcsCursor( m_iAdjustDbcs );
				UpdateAnchor( 0L );
				if ( !FMoveWindowToCursor( FALSE ) ) {
	            	Paint( (PRRC)0, NULL );
				}
			}
            break;

        case SB_TOP:
            m_ipCur.oln = 0;
            m_pdCur.olntop = 0;
			AdjustDbcsCursor( m_iAdjustDbcs );
            UpdateAnchor( lParam );
			if ( !FMoveWindowToCursor( TRUE ) ) {
            	Paint( (PRRC)0, NULL );
			}
            break;

        case SB_BOTTOM: {
            WORD    dyCursor = (WORD)( m_ipCur.oln - m_pdCur.olntop );

            // We want to put the last line of the buffer at the
            // bottom of the window

            // If the whole file fits in the window, set the
            // top of the window to be the top of the file.
            if ( olnMac < (DWORD)dyWin ) {
               dyWin = LOWORD( olnMac );
            }
            m_pdCur.olntop = olnMac - (DWORD)dyWin;
            m_ipCur.oln = olnMac - 1;
			AdjustDbcsCursor( m_iAdjustDbcs );
            UpdateAnchor( lParam );
			if ( !FMoveWindowToCursor( TRUE ) ) {
            	Paint( (PRRC)0, NULL );
			}
            break;
        }
    }
}

/**** HScroll - Perform HORIZONTAL scrolling                            ****
 *                                                                         *
 *  PURPOSE: Perform HORIZONTAL window scrolling                           *
 *                                                                         *
 *  INPUTS:                                                                *
 *     wScrollType  Type of scroll to perform                              *
 *     lParam       Scroll specific information (ala CW spec)              *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: HScroll(
UINT   wScrollType,
long   lParam ) {
    WORD    dxWin;
    RRC     rrc;
	DWORD	olnMac = CLinesInBuf();
	char	sz[ cchEditMax ];
	WORD	cch;

    GetEditRrc( TRUE, &rrc );
    dxWin = (WORD)rrc.rxRight;

	CbGetCachedLine ( m_ipCur.oln, sizeof ( sz ) - 1, sz );
	cch = ObMaxBuf();

    switch( wScrollType ) {
        case SB_LINEUP:
            if ( m_pdCur.obleft ) {
                --m_pdCur.obleft;
                if ( m_pdCur.obleft + dxWin == m_ipCur.ob ) {
                    --m_ipCur.ob;
					AdjustDbcsCursor( -1 );
                }
                UpdateCursorPos();
                UpdateAnchor( lParam );
                Paint( (PRRC)0, NULL );
            }
            break;

        case SB_LINEDOWN:
			if ( m_pdCur.obleft + dxWin <= cch ) {
                ++m_pdCur.obleft;
                if ( m_pdCur.obleft - 1 == m_ipCur.ob ) {
                    ++m_ipCur.ob;
					AdjustDbcsCursor( 1 );
                }
                UpdateCursorPos();
                UpdateAnchor( lParam );
                Paint( (PRRC)0, NULL );
            }
            break;

        case SB_PAGEUP: {
            WORD    obleftNew;

            if ( m_pdCur.obleft <= dxWin ) {
                obleftNew = 0;
                m_ipCur.ob -= m_pdCur.obleft;
            }
            else {
                obleftNew = m_pdCur.obleft - dxWin;
                m_ipCur.ob -= dxWin;
            }
            m_pdCur.obleft = obleftNew;
			AdjustDbcsCursor( -1 );
            UpdateAnchor( lParam );
			if ( !FMoveWindowToCursor( TRUE ) ) {
				Paint( (PRRC)0, NULL );
			}
            break;
        }

        case SB_PAGEDOWN: {
            WORD    obleftNew;

            if ( m_pdCur.obleft + ( dxWin << 1 ) < cch ) {
                obleftNew = m_pdCur.obleft + dxWin;
                m_ipCur.ob += dxWin;
            }
            else if ( m_pdCur.obleft + dxWin < cch ) {
                obleftNew = cch - dxWin + 1;
                m_ipCur.ob -= m_pdCur.obleft - obleftNew;
            }
			else {
				// If the display is empty, put the cursor at the top(left edge)
				if ( cch <= 1 ) {
					cch = 1;
				}
	            obleftNew = m_ipCur.ob = cch - 1;
			}

			m_pdCur.obleft = obleftNew;
			AdjustDbcsCursor( 1 );
			UpdateAnchor( lParam );
			if ( !FMoveWindowToCursor( TRUE ) ) {
				Paint( (PRRC)0, NULL );
			}
            break;
        }

        case SB_THUMBPOSITION:
            if ( LOWORD( lParam ) <= cch - 1 ) {
	            m_pdCur.obleft = m_ipCur.ob = LOWORD( lParam );
				AdjustDbcsCursor( m_iAdjustDbcs );
				UpdateAnchor( lParam );
				if ( !FMoveWindowToCursor( TRUE ) ) {
					Paint( (PRRC)0, NULL );
				}
            }
            break;

        case SB_TOP:
            m_ipCur.ob = 0;
            m_pdCur.obleft = 0;
            UpdateAnchor( lParam );
            Paint( (PRRC)0, NULL );
            UpdateCursorPos();
            break;

        case SB_BOTTOM: {
            char    sz[ cchEditMax ];
            WORD    cch;

            cch = CbGetCachedLine( m_ipCur.oln, sizeof( sz ) - 1, sz );
            m_ipCur.ob = cch;
            UpdateAnchor( lParam );
            FMoveWindowToCursor( FALSE );
            break;
        }
    }
}

/**** DeleteChar - Delete the character either before/after cursor      ****
 *                                                                         *
 *  PURPOSE: Delete the character at the current position.                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *     fDeleteCur   if TRUE then DELETE key (delete under cursor),         *
 *                  else, then BACKSPACE key (delete key before cursor)    *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: DeleteChar(
BOOL    fDeleteCur ) {

    // The buffer (OR where cursor sits) must NOT be read-only.  Ask
    // the owner if we're allowed to edit

    if ( !FReadOnlyBuf() ) {
        WORD    cch;
        char *  pch;
        char *  psz = m_pldCur->prgch;

        // If BACKSPACE, then
        //    1) Make sure that we're not attempting to delete before ob == 0
        //    2) Set the cursor back 1 position.  This will be where the
        //       cursor should be AFTER the either delete key has occurred
        if ( !fDeleteCur ) {
            if ( !m_ipCur.ob ) {
                MessageBeep( 0 );
                return;
            }
            --m_ipCur.ob;
        }

        cch = CbGetLineIntoCache( m_ipCur.oln );

        // Reset the selection
		m_ipAnchor = m_ipCur;

        if ( m_ipCur.ob < cch ) {
            pch = psz + m_ipCur.ob;
            memmove( pch, pch + 1, cch - m_ipCur.ob + 1 );
            --m_pldCur->cb;

            if ( m_pdCur.obleft && m_ipCur.ob == m_pdCur.obleft ) {
                if ( m_pdCur.obleft < cchDelBack ) {
                    m_pdCur.obleft = 0;
                }
                else {
                    m_pdCur.obleft -= cchDelBack;
                }
                Paint( (PRRC)0, NULL );
            }
            else {
                if ( !FMoveWindowToCursor( FALSE ) ) {
                    PaintLine( m_ipCur.oln );
                }
            }
        }
        else {
            // They we're deleting (BACKSPACE) beyond the true
            // end of the line, so just move the cursor
            if ( !fDeleteCur ) {
               FMoveWindowToCursor( FALSE );
            }
        }
    }
    else {
        MessageBeep( 0 );
    }
}

/**** InsertChar - Insert a character into the current line/buffer      ****
 *                                                                         *
 *  PURPOSE: Insert a character at the current buffer position.  If        *
 *           insertion point is beyond end of line, pad with spaces.       *
 *                                                                         *
 *  INPUTS:                                                                *
 *     wParam   WM_CHAR specific value.  Usually ASCII value.  This        *
 *              routine will only be called if !isprint( wParam ).         *
 *     lParam   WM_CHAR specific value.  Not used (yet?)                   *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: InsertChar(
UINT   wParam,
long   lParam ) {

    // We can't insert if the cursor is sitting over a read-only
    // area in the file/buffer, so make sure that we're on read-write.
    if ( !FReadOnlyBuf() ) {
        WORD    cch;
        WORD    obCur = m_ipCur.ob;
        char *  psz = m_pldCur->prgch;
        char *  pchInsert = psz + obCur;
        BOOL    fInsertChar = (BOOL)fInsertMode;

        cch = CbGetLineIntoCache( m_ipCur.oln );

        // If the line is already axMax chars, there's no room
        // for more insertions, so Beep at em
        if ( cch + 1 < (WORD)axMax || !fInsertChar ) {

            // If the cursor's beyond the end of the line, pad
            // with spaces before inserting the character.
            if ( cch < obCur ) {
                WORD    cchT = obCur - cch;

                memset( _ftcschr( psz, _T('\0') ), ' ', cchT );
                *( pchInsert + 1 ) = '\0';
                m_pldCur->cb += cchT + 1;
            }
            // Regular insert, move existing text over first
            else if ( fInsertChar || cch == obCur ) {
                memmove( pchInsert + 1, pchInsert, cch + 1 - obCur );
                ++m_pldCur->cb;
            }

            if ( m_ipCur.ob < axMax ) {
                ++m_ipCur.ob;
            }

            // Insert the character into the buffer
            *pchInsert = LOBYTE( wParam );

            // Reset the selection
			m_ipAnchor = m_ipCur;

            // Update the cursor position
            if ( !FMoveWindowToCursor( FALSE ) ) {
                PaintLine( m_ipCur.oln );
            }
        }
        else {
            MessageBeep( 0 );
        }
    }
    else {
        MessageBeep( 0 );
    }
}

/**** PreviousWord - Scroll back to the previous "word"                 ****
 *                                                                         *
 *  PURPOSE: Move the cursor to the beginning of the previous word that    *
 *           that cursor is sitting on.                                    *
 *                                                                         *
 *  INPUTS:                                                                *
 *      lParam   WM_CHAR lParam message value (contains shift key state)   *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: PreviousWord(
long   lParam ) {
    register WORD    cch;
    register CHAR *  pch;
    CHAR             szBuf[ cchEditMax ];
    CHAR *           sz;
    BOOL             fFound = FALSE;
    WORD             obCur = m_ipCur.ob;
    DWORD            olnCur = m_ipCur.oln;
    BOOL             fFirstPass = TRUE;

    // The buffer will be 1 larger than the max size so we
    // can go backwards through the buffer and check the current
    // position - 1 w/o GP faulting
    *szBuf = ' ';
    sz = &szBuf[ 1 ];

    do {
        cch = CbGetCachedLine( m_ipCur.oln, sizeof( szBuf ) - 1, sz );

        cch = min( cch, obCur );
        pch = _tcsdec( szBuf, sz + cch );

        if ( fFirstPass && cch && _istspace((UCHAR)*( pch ) ) ) {
            pch = _tcsdec( szBuf, pch );
            cch -= _tclen( pch );
        }

        // Skip the white spaces
        while( cch && _istspace((UCHAR)*pch ) ) {
            pch = _tcsdec( szBuf, pch );
            cch -= _tclen( pch );
        }

        // Find the beginning of this word
        if ( FIsWordChar( *pch ) && cch ) {
			CHAR *	pchPrev = _tcsdec( szBuf, pch );

            while( cch && FIsWordChar( *pchPrev ) ) {
	            pch = pchPrev;
	            cch -= _tclen( pch );
				pchPrev = _tcsdec( szBuf, pch );
            }
        }

        // If cch is non-zero, then we've found a spot.  Otherwise,
        // try the previous line in the file for search
        if ( !cch ) {
            --m_ipCur.oln;
            pch = sz;
            obCur = (WORD)axMax;
        }
        else {
            m_ipCur.ob = (WORD)( pch - sz );
            fFound = TRUE;
        }

        fFirstPass = FALSE;

        // If the current line == olnMax, we've either found the new
        // position (fFound==TRUE) or we've backed up beyond the beginning
        // of the file.  Either way, get out
    } while ( !fFound && m_ipCur.oln != olnMax );


    // If found, move the cursor (or window) to the correct position
    if ( fFound ) {
		AdjustDbcsCursor( m_iAdjustDbcs );
        UpdateAnchor( lParam );
        if ( !FMoveWindowToCursor( FALSE ) ) {
            PaintLine( m_ipCur.oln );
        }
    }

    // Otherwise, restore the current line (we may have trashed it above)
    // and beep at them.
    else {
        m_ipCur.oln = olnCur;
        MessageBeep( 0 );
    }
}

/**** NextWord - Scroll over to the next "word"                         ****
 *                                                                         *
 *  PURPOSE: Move the cursor to the beginning of the next word in the file *
 *                                                                         *
 *  INPUTS:                                                                *
 *      lParam   WM_CHAR lParam message value (contains shift key state)   *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: NextWord(
long   lParam ) {
    register WORD    cch;
    register TCHAR * pch;
    TCHAR            sz[ cchEditMax ];
    DWORD            olnMac = CLinesInBuf();
    BOOL             fFound = FALSE;
    BOOL             fFirstPass = TRUE;
    DWORD            oln = m_ipCur.oln;

    // Check to make sure that we're in range before looking.  We
    // may be beyond the last line in the file.
    while ( !fFound && m_ipCur.oln < olnMac ) {

        cch = CbGetCachedLine( m_ipCur.oln, sizeof( sz ) - 1, sz );

		// On the first pass, we want to begin on the next character
		if ( fFirstPass && m_ipCur.ob < cch ) {
	    	pch = _tcsinc( sz + m_ipCur.ob );
		}

        if ( fFirstPass && m_ipCur.ob < cch || !fFirstPass && cch ) {

            cch -= pch - sz;

            // First pass only, skip over current word
            if ( fFirstPass ) {
                if ( FIsWordChar( *( _tcsdec( sz, pch ) ) ) ) {
					// Does NOT need to be DBCS here since FIsWordChar
					// only accepts single byte characters!
                    while( FIsWordChar( *pch ) ) {
                        ++pch;
                        --cch;
                    }
                }
            }

            // Skip the white spaces
            while( cch && _istspace( (_TUCHAR)*pch ) ) {
				size_t	cb;
				
				cb = _tclen( pch );
				cch -= cb;
				pch += cb;
            }
        }
        else {
            cch = 0;
        }

        fFirstPass = FALSE;

        // If cch is non-null then we found what we're looking for.
        // Otherwise, start searching the next line for something.
        if ( !cch ) {
            ++m_ipCur.oln;
            pch = sz;
        }
        else {
            m_ipCur.ob = (WORD)( pch - sz );
            fFound = TRUE;
        }
    }

    // If we've found something, set the text and the cursor
    // accordingly.  Otherwise, reset the current line and
    // Beep at them to let them know that we failed
    if ( fFound ) {
		AdjustDbcsCursor( m_iAdjustDbcs );
        UpdateAnchor( lParam );
        if ( !FMoveWindowToCursor( FALSE ) ) {
            PaintLine( m_ipCur.oln );
        }
    }
    else {
        m_ipCur.oln = oln;
        MessageBeep( 0 );
    }
}

void CMultiEdit :: DoEnterKey() {
#ifdef FULL_
    WORD    cch = CbGetLineIntoCache( m_ipCur.oln );
    char *  psz;

    psz = m_pldCur->prgch;
    if ( m_ipCur.ob >= cch ) {
        // Point to zero terminator
        psz += cch;
    }
    else {
        psz += m_ipCur.ob;
    }

    if ( !FReadOnlyBuf() &&
        InsertLineBuf( m_ipCur.oln, _ftcslen( psz ), psz ) ) {
        *psz = '\0';
        m_pldCur->cb = _ftcslen( m_pldCur->prgch );
        FlushCache();
        ++m_ipCur.oln;
        m_ipCur.ob = 0;
		m_ipAnchor = m_ipCur;
        if ( !FMoveWindowToCursor( FALSE ) ) {
            Paint( (PRRC)0, NULL );
            UpdateCursorPos();
        }
    }
    else {
        FlushCache();
    }
#endif	// FULL_
}

/**** DoChar - responses for character (WM_CHAR) input to window        ****
 *                                                                         *
 *  PURPOSE: Main processing for character (keyboard) input.               *
 *                                                                         *
 *  INPUTS:                                                                *
 *    wParam  WM_CHAR specific data.  Usually ASCII character.             *
 *    lParam  WM_CHAR specific data.  Usually contains ctrl/shift/alt      *
 *            states along with untranslated data.  See CW Messages API.   *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: DoChar(
UINT   wParam,
long   lParam ) {

    if ( lParam & 0x02000000L ) {
		char	sz[ cchEditMax ];

		CbGetCachedLine( m_ipCur.oln, sizeof ( sz ) - 1, sz );

        switch( wParam ) {
            case VK_HOME:

                // Ctrl-home: Move to top of file
                // Home: Move to beginning of current line

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    m_pdCur.obleft = m_ipCur.ob = 0;
                    VScroll( SB_TOP, lParam );
                }
                else {
                    HScroll( SB_TOP, lParam );
                }
                break;

            case VK_END:

                // Ctrl-End: Move to bottom of file
                // End: Move to end of current line

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    VScroll( SB_BOTTOM, lParam );
                }
                HScroll( SB_BOTTOM, lParam );
                break;

            case VK_PRIOR:

                // Ctrl-PageUp: Scroll LEFT 1 page
                // PageUp: Scroll UP 1 page

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    HScroll( SB_PAGEUP, lParam );
                }
                else {
                    VScroll( SB_PAGEUP, lParam );
                }
                break;

            case VK_NEXT:

                // Ctrl-PageDn: Scroll RIGHT 1 page
                // PageDn: Scroll DOWN 1 page

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    HScroll( SB_PAGEDOWN, lParam );
                }
                else {
                    VScroll( SB_PAGEDOWN, lParam );
                }
                break;

            case VK_UP:

                // Ctrl-Up: Scroll 1 line UP
                // Up: Move cursor 1 line UP.  Scroll if necessary.

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    VScroll( SB_LINEUP, lParam );
                }
                else {
                    if ( m_ipCur.oln ) {
                        --m_ipCur.oln;
						AdjustDbcsCursor( m_iAdjustDbcs );
                        UpdateAnchor( lParam );
                        FMoveWindowToCursor( HIWORD( lParam ) & KK_SHIFT );
                    }
                }
                break;

            case VK_DOWN:

                // Ctrl-Down: Scroll 1 line DOWN
                // Down: Move cursor 1 line DOWN.  Scroll if necessary.

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    VScroll( SB_LINEDOWN, lParam );
                }
                else {
                    if ( m_ipCur.oln + 1 < CLinesInBuf() ) {
                        ++m_ipCur.oln;
						AdjustDbcsCursor( m_iAdjustDbcs );
                        UpdateAnchor( lParam );
                        FMoveWindowToCursor( HIWORD( lParam ) & KK_SHIFT );
                    }
                }
                break;

            case VK_LEFT:

                // Ctrl-Left: Move cursor to beginning of previous word
                // Left: Move cursor 1 char left.

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    PreviousWord( lParam );
                }
                else {
                    if ( m_ipCur.ob ) {
                        --m_ipCur.ob;
						AdjustDbcsCursor( -1 );
                        UpdateAnchor( lParam );
						FMoveWindowToCursor( HIWORD( lParam ) & KK_SHIFT );
                    }
                }
                break;

            case VK_RIGHT:

                // Ctrl-Right: Move cursor to beginning of next word
                // Right: Move cursor 1 char right.

                if ( HIWORD( lParam ) & KK_CONTROL ) {
                    NextWord( lParam );
                }
                else {
                    if ( m_ipCur.ob < ObMaxBuf() ) {
                        ++m_ipCur.ob;
						AdjustDbcsCursor( 1 );
                        UpdateAnchor( lParam );
						FMoveWindowToCursor( HIWORD( lParam ) & KK_SHIFT );
                    }
                }
                break;

            case VK_DELETE:
                // Delete the character at the cursor position (DELETE)
                // or delete the character to the LEFT of the cursor
                // position (BACKSPACE)

                DeleteChar( TRUE );
                break;

            default:
                break;
        }
    }
    else {
        // If the character is printable, do something with it
        // (insert into buffer).  Otherwise, we don't do anything
        // special for non-printable stuff so...
        switch( wParam ) {
            case VK_BACK:
                DeleteChar( FALSE );
                break;

            case VK_CANCEL:
                if ( m_pldCur->flags ) {
                   // Throw away the cached line and redraw the
                   // original
                   m_pldCur->flags = FALSE;
                   m_pldCur->cb = 0;
                   *m_pldCur->prgch = '\0';
                   PaintLine( m_pdCur.oln );
                   m_pdCur.oln = 0xffff;
                }
                break;

            case VK_ESCAPE:
				m_ipAnchor = m_ipCur;

                // Clean out cache and throw away changes!
                if ( m_pldCur->flags && m_pdCur.oln != 0xffff ) {
                    DWORD   olnCache = m_pdCur.oln;

                    m_pldCur->flags = FALSE;
                    m_pldCur->cb = 0;
                    m_pdCur.oln = olnMax;
                    *m_pldCur->prgch = '\0';
                    PaintLine( olnCache );
                }
                break;

            case VK_RETURN:
                // ENTER key pressed. Notify buffer owner that a line has
                // (may have) changed.
                DoEnterKey();
                break;

            default:
                if ( _istprint( wParam ) && !FReadOnlyBuf() ) {
                    // Always remove the selection
                    DeleteSelection();
					InsertChar( wParam, lParam );
                }
				else {
					MessageBeep( 0 );
				}
                break;
        }
    }
}

/**** PaintLine - Paint 1 line only                                     ****
 *                                                                         *
 *  PURPOSE: Paint a specific line.  It is assumed that it is already      *
 *           visible and that we're just updating it.  Functionally,       *
 *           this is just to shorten code since we do this all over the    *
 *           place.                                                        *
 *                                                                         *
 *  INPUTS:                                                                *
 *     oln   Line number to paint                                          *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: PaintLine(
DWORD    oln ) {
    RRC rrc;

    // The top line should be the relative window coordinate, so
    // compute it bades upon the top line in the window and the
    // offset line which we want to display.
    // The bounding RRC must have ryBottom - ryTop != 0.  If it's
    // zero, then no painting will be done.

    rrc.ryTop = (RY)( oln - m_pdCur.olntop );
    rrc.ryBottom = rrc.ryTop + (RY)1;
    Paint( &rrc, NULL );
}

void CMultiEdit::ArmDataTip()
{
	CPoint pt;
	GetCursorPos(&pt);

	if (abs(CDataTip::GetX() - pt.x) <= 2 && abs(CDataTip::GetY() - pt.y) <= 2)
		   return;

	if (GetKeyState(VK_CONTROL) < 0)
		SetTimer(DATATIP_TIMER, 100, NULL);
	else
		SetTimer(DATATIP_TIMER, 600, NULL);

	CDataTip::SetX(pt.x);
	CDataTip::SetY(pt.y);

	if (CDataTip::GetTip()) CDataTip::GetTip()->DestroyTip();
}

/**** DoMouse - Perform responsed to mouse messages for window          ****
 *                                                                         *
 *  PURPOSE: Perform mouse responses to window messages.  We will mostly   *
 *           set pipCur and do scrolling in here.  Since most CV windows   *
 *           do something special with double clicks, we will ignore it.   *
 *           normally, we would select the current word.                   *
 *                                                                         *
 *  INPUTS:                                                                *
 *     wMsg     Window message (limited to MOUSE messages in this case)    *
 *     wParam   Message specific WORD data.  See CW Message API for details*
 *     lParam   Message specific long data.  Contains SHIFT key state.     *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: DoMouse(
UINT   wMsg,
UINT   wParam,
long   lParam ) {
    static long  lParamSav;
    static DWORD olnDragMac = 0;
    static BOOL  fTimer = FALSE;

    WORD    obMouse;
    DWORD   olnMouse;
	WORD	obMacBuf;
    int     xMouse;
    int     yMouse;

	PosFromPoint( xMouse, yMouse, CPoint( (DWORD)lParam ) );
    obMouse = ( xMouse - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth + m_pdCur.obleft;
    olnMouse = (DWORD)( yMouse / m_pFontInfo->m_tm.tmHeight ) + m_pdCur.olntop;

	obMacBuf = ObMaxBuf();
	if ( obMouse > obMacBuf ) {
		obMouse = obMacBuf;
	}

    switch( wMsg ) {

        // LBtnDown: Set the cursor where the mouse is.
        // Later, we will want to make sure that the cursor
        // is being placed w/i the buffers boundaries and if it's
        // not, put it at the end of the buffer.  Also, we will
        // want to set the mouse capture.
        case WM_LBUTTONDOWN: {
			BOOL	fAltKey = (BOOL)!!( GetKeyState( VK_MENU ) & ~1 );

            SetCapture();
            m_ipCur.ob = obMouse;
            if ( olnDragMac = CLinesInBuf() ) {
                --olnDragMac;
            }

            m_ipCur.oln = min( olnMouse, olnDragMac );
			AdjustDbcsCursor( -1 );

			// If the shift key is down and the alt key is
			// down, then the selection is retained, but the
			// mode is set to column select.  UpdateAnchor
			// will redisplay the selection
			if ( ( wParam & MK_SHIFT ) && fAltKey ) {
				m_fColumnSelect = TRUE;
			}
 			UpdateAnchor( MAKELONG( 0, wParam & MK_SHIFT ? KK_SHIFT : 0 ) );

			// Shift key or not, the alt key is down set to column select
			// if the shift key is down, the code above will redisplay the
			// selection to column select, if not, this will set us in
			// column select mode
			if ( fAltKey ) {
				m_fColumnSelect = TRUE;
			}

			FMoveWindowToCursor( TRUE );
           break;
		}

        // Mouse move mouse will be down, do selection stuff (repaint)
        // after resetting pipCur...  We *MAY* want to do scrolling
        // if we're out of the client region.
        case WM_TIMER:
            lParam = lParamSav;
			PosFromPoint( xMouse, yMouse, CPoint( (DWORD)lParam ) );
            obMouse = ( xMouse - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth + m_pdCur.obleft;
			if ( obMouse > obMacBuf ) {
				obMouse = obMacBuf;
			}
            olnMouse = (DWORD)( yMouse / m_pFontInfo->m_tm.tmHeight ) + m_pdCur.olntop;
            // fall through

        case WM_MOUSEMOVE:
            if ( GetCapture() == this ) {
                BOOL    fNonClient = FALSE;
                RECT    rect;
                RRC     rrc;
				WPARAM	wParamSend = SB_TOP;

                GetClientRect( &rect );
                GetEditRrc( FALSE, &rrc );

				// Horizontal scrolling (no problems)
                if ( xMouse < rect.left ) {
                    fNonClient = TRUE;
                    if ( m_ipCur.ob ) {
                        --m_ipCur.ob;
                    }
                }
                else if ( xMouse >= rect.right ) {
                    fNonClient = TRUE;
					if ( m_ipCur.ob < ObMaxBuf() ) {
                        ++m_ipCur.ob;
                    }
                }
                else {
                    m_ipCur.ob = min( obMouse, ObMaxBuf() );
                }

				// Vertical scrolling, may cause remapping in Disasm/Mixed
				// window.  Need to send WM_VSCROLL messages so the window
				// can readjust itself
                if ( yMouse < rect.top ) {
                    fNonClient = TRUE;
                    if ( m_ipCur.oln ) {
						if ( m_ipCur.oln == m_pdCur.olntop ) {
							wParamSend = SB_LINEUP;
						}
						else {
							--m_ipCur.oln;
						}
                    }
                }

				// The yMouse is out of range if it's past
				// the last FULL line in the window!
                else if ( yMouse >= rrc.ryBottom * m_pFontInfo->m_tm.tmHeight ) {
                    fNonClient = TRUE;
                    if ( m_ipCur.oln < olnDragMac + 1 ) {
						if ( m_ipCur.oln == m_pdCur.olntop + rrc.ryBottom - 1 ) {
							wParamSend = SB_LINEDOWN;
						}
						else {
                        	++m_ipCur.oln;
						}
                    }
                }
                else {
		            m_ipCur.oln = olnMouse;
                }

				if ( m_ipCur.oln > olnDragMac ) {
					m_ipCur.oln = olnDragMac + 1;
				}

                if ( fNonClient ) {
					if ( wParamSend != SB_TOP ) {
						SendMessage( WM_VSCROLL, wParamSend, 0L );
					}
                    if ( !fTimer ) {
                        SetTimer( idTimer, 2, NULL );
                        fTimer = TRUE;
                    }
					lParamSav = lParam;
                }
                else if ( fTimer ) {
                    KillTimer( idTimer );
                    fTimer = FALSE;
                }

				AdjustDbcsCursor( -1 );
                FMoveWindowToCursor( TRUE );
                UpdateAnchor( MAKELONG( 0, KK_SHIFT ) );
			}
			else if ( wMsg == WM_MOUSEMOVE ) {
				CWnd * pActiveWnd = GetActiveWindow();

				if (!(wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON | MK_SHIFT))
					&& (pActiveWnd != NULL) && SupportsDataTips())
				{
					ArmDataTip();
				}
			}
				
            break;

        // Release the capture here and nothing else.
        case WM_LBUTTONUP:
            if ( GetCapture() == this ) {
                ReleaseCapture();
                fTimer = FALSE;
                KillTimer( idTimer );
            }
            break;

        // Don't use wParam and lParam since the BUTTON up/down messages
        // have already positioned the cursor (and possibly the anchor)
        case WM_LBUTTONDBLCLK: {
			SelectWordCur();
            break;
        }
    }
}

/**** SelectWordCur - selects word at cursor location. Assumes cursor and
 * possibly anchor have already been positioned. Used by WM_LBUTTONDBLCLK	*
 ***************************************************************************/
void CMultiEdit :: SelectWordCur()
{
    char    sz[ cchEditMax ];
    WORD    obMic;
    WORD    obMac;

    if ( LcbGetWord( sizeof( sz ), sz, &obMic, &obMac ) )
	 {
        m_ipCur.ob = obMac;
        m_ipAnchor.ob = obMic;
        if ( !FMoveWindowToCursor( FALSE ) )
		{
            UpdateCursorPos();
        }
		UpdateAnchor( MAKELONG( 0, KK_SHIFT ) );
    }

}



/**** FMouseHitSel - determine if the mouse is over a selection         ****
 *                                                                         *
 *  PURPOSE: Used for drag and drop.  If the lparam coordinates of the     *
 *           mouse intersect with a selection in the ACTIVE window,        *
 *           return true.  It is the responsibility of the caller to change*
 *           cursors or initiate drag and drop.                            *
 *                                                                         *
 *  INPUTS:                                                                *
 *     fSetCursor                                                          *
 *              If return value is true and fSetCursor is non-zero, set    *
 *              cursor at specified position                               *
 *                                                                         *
 *     lParam   Mouse message lParam (client coordinates of mouse event).  *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: TRUE if mouse intersects with a selection            *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
BOOL CMultiEdit :: FMouseHitSel( BOOL fSetCursor, LPARAM lParam ) {
	BOOL	fRet = FALSE;

	if ( FIsActiveWindow( m_hWnd ) ) {
		WORD	obSelMic;
		WORD	obSelMac;
		WORD	obMouse;
		DWORD	olnMouse;
		WORD	obMacBuf = ObMaxBuf() - 1;
		int		xMouse;
		int		yMouse;

		PosFromPoint( xMouse, yMouse, CPoint( (DWORD)lParam ) );
	    obMouse = ( xMouse - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth + m_pdCur.obleft;
		olnMouse = (DWORD)( yMouse / m_pFontInfo->m_tm.tmHeight ) + m_pdCur.olntop;
		if ( obMouse > obMacBuf ) {
			obMouse = obMacBuf;
		}

		if ( FGetSelection( olnMouse, &obSelMic, &obSelMac ) ) {
			if ( obSelMic <= obMouse && obMouse <= obSelMac ) {
				fRet = TRUE;

				// Since the cursor is in a selection, the position
				// must be valid for the buffer and must already be
				// visible.  This will set the cursor position to
				// the specified location, reset the selection (to NULL)
				// and move the cursor
				if ( fSetCursor ) {
					m_ipCur.ob = obMouse;
					m_ipCur.oln = olnMouse;
					UpdateAnchor( 0L );
			        UpdateCursorPos();
				}
			}
		}
	}
	return fRet;
}

/**** AdjustDbcsCursor - update cursor to be on lead byte               ****
 *                                                                         *
 *  PURPOSE: Update ipCur to be on lead byte of dbcs char                  *
 *                                                                         *
 *  INPUTS:                                                                *
 *      iAdjust	Direction to adjust (must be either 1 or -1)               *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *      Possibly updates ipCursor and iDbcsAdjust.  iDbcsAdjust will have  *
 *      the sign flipped if the cursor was adjusted.                       *
 *                                                                         *
 ***************************************************************************/
void CMultiEdit :: AdjustDbcsCursor( int iAdjust ) {
	if ( FDbcsEnabled() )
    {
		char	sz[ axMax ];
		size_t	cch;

	    cch = CbGetCachedLine( m_ipCur.oln, sizeof( sz ) - 1, sz );

		// If the cursor is beyond the end of the line, no need to adjust
		if ( m_ipCur.ob < cch ) {
			char *	pch = sz;
			char *	pchCursor = &sz[ m_ipCur.ob ];

			// Scan the string until we have found the cursor
			// location
			while( pch < pchCursor ) {
				pch += _tclen( pch );
			}

			// If the cursor location doesn't match the current
			// location, then we need to adjust the ipCur.ob
			if ( pch != pchCursor ) {
				m_ipCur.ob += iAdjust;
				m_iAdjustDbcs = iAdjust * -1;
			}
		}
	}
}
/**** CMultiEdit::LTextProc - Main implementation                       ****
 *                                                                         *
 *  PURPOSE: Entry point for all multiline edit control.  WndProc to       *
 *           respond to CW/window owner messages.                          *
 *                                                                         *
 *                                                                         *
 *  INPUTS:                                                                *
 *     wMsg    Message to respond to or act upon.                          *
 *     wParam  Message specific WORD value.  See CW Message API.           *
 *     lParam  Message specific long value.                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: 1L if message consumed or responded to, else 0L.     *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
LRESULT CMultiEdit :: LTextProc(UINT wMsg, WPARAM wParam, LPARAM lParam )
{
    long    	lRet = 0L;
    DWORD   	oln;
    WORD    	ob;
    BYTE    	fSelection;
    DWORD   	olnAnchor;
	BOOL		fCallDef = TRUE;

    if ( wMsg == WM_DESTROY )
        return(this->WindowProc(wMsg, wParam, lParam) );

	// Save the old current position to determin if we need
	// to update Scrollbars
	oln = m_pdCur.olntop;
	ob = m_pdCur.obleft;
    fSelection = m_fSelection;
    olnAnchor = m_ipAnchor.oln;

    switch( wMsg ) {
        case WM_SETFOCUS:
            ::CreateCaret( m_hWnd, NULL, CURSOR_WIDTH, m_pFontInfo->m_tm.tmHeight );	// m_tmAveCharWidth
            if ( !( m_Style & ES_NOREDRAW ) ) {
                ShowCaret();
            }
            UpdateCursorPos();

			// If there's a selection, we will recheck to see if the status
			// changed during this message.  By setting it to FALSE, we will
			// detect that the selection status has toggled and hide the caret
			// if there's a selection.  If there's no selection, the status
			// will not have changed and nothing will be done with the caret
			fSelection = FALSE;
            break;

        case WM_KILLFOCUS:
            FlushCache();
            if ( ::HideCaret( m_hWnd ) ) {
            	DestroyCaret();
			}
            break;

        case WM_FLUSHFOCUS:
            FlushCache();
            PaintLine( m_ipCur.oln );
            fCallDef = FALSE;
            break;

        case WM_SETREDRAW:
            fCallDef = FALSE;
            if ( wParam ) {
                m_Style &= ~(BYTE)ES_NOREDRAW;
                --ob;
                --oln;
                if ( !FMoveWindowToCursor( FALSE ) ) {
                    Paint( (PRRC)0, NULL );
                }
            }
            else {
                m_Style |= (BYTE)ES_NOREDRAW;
            }
            if ( wParam ) {
                ShowCaret();
            }
            else {
                HideCaret();
            }

			// Reset fSelection so that we don't re-show/hide the caret below
			fSelection = (BYTE)!MEFAreIPsEqual( &m_ipAnchor, &m_ipCur );
            break;

        case WM_PAINTLINE:
            // Paint the line specified in lParam.  If the line
            // is not visible, readjust the screen so it is.
            m_ipCur.oln = lParam;
            if ( !wParam ) {
				m_ipAnchor = m_ipCur;
            }
            if ( !FMoveWindowToCursor( FALSE ) ) {
                PaintLine( m_ipCur.oln );
            }
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
			RRC			rrc;
			RRC *		prrc = (PRRC)LOWORD( lParam );

            BeginPaint( &ps );
			if ( !prrc &&
				( ps.rcPaint.left != ps.rcPaint.right ||
				ps.rcPaint.top != ps.rcPaint.bottom ) ) {
					int tmAveCharWidth = m_pFontInfo->m_tm.tmAveCharWidth;
					int tmHeight = m_pFontInfo->m_tm.tmHeight;
					rrc.rxLeft = ps.rcPaint.left / tmAveCharWidth;
					rrc.rxRight = ps.rcPaint.right / tmAveCharWidth + 1;
					rrc.ryTop = ps.rcPaint.top / tmHeight;
					rrc.ryBottom = ps.rcPaint.bottom / tmHeight + 1;
					prrc = &rrc;
			}
            Paint( prrc, CDC::FromHandle( ps.hdc ) );
            ValidateCursor();
            EndPaint( &ps );
	        fCallDef = FALSE;

			lRet = TRUE;	// ??? [dwg] ???
            // Change oln and ob to force scroll bar updateing
            ++oln;
            ++ob;
            break;
        }

        case WM_CLEAR:
            if ( fSelection ) {
                DeleteSelection();
            }
            else {
                DoChar( VK_DELETE, 0x02000000L | LGetShiftState() );
            }
            fCallDef = FALSE;
            break;

        case WM_KEYDOWN:
            fCallDef = FALSE;
            switch( wParam ) {
                case VK_DELETE:
                    if ( fSelection ) {
                        DeleteSelection();
                    }
                    else {
                        DoChar( VK_DELETE, lParam | 0x02000000L );
                    }
                    break;

                case VK_INSERT: {
                    if ( HIWORD( lParam ) & KK_SHIFT ) {
                        PasteSelection();
                    }
                    else if ( HIWORD( lParam ) & KK_CONTROL ) {
                        CopySelection( (char FAR *)NULL, 0 );
                    }
                    break;
                }

                case VK_HOME:
                case VK_END:
                case VK_PRIOR:
                case VK_NEXT:
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:
                    DoChar( wParam, lParam | 0x02000000L );
                    break;

#if 0
				// the shell already does this on your behalf if you
				// return a nonempty string from package virtual GetHelpWord
				// this is redundant and causes a double lookup
				// ORION bug 13977 - craigc
				//
				case VK_F1: {
					CString strHelp;

					if ( GetHelpWord( strHelp ) ) {
						theApp.HelpOnKeyword( strHelp );
					}
					break;
				}
#endif

                default:
                    fCallDef = TRUE;
                    break;
            }
            break;

        case WM_CHAR:
           	DoChar( wParam, lParam );
            fCallDef = FALSE;
            break;

        case WM_CUT:
            CopySelection( (char FAR *)NULL, 0 );
            DeleteSelection();
            fCallDef = FALSE;
            break;

        case WM_COPY:
            lRet = (long)(LRESULT)CopySelection( (char FAR *)NULL, (size_t)lParam );
            fCallDef = FALSE;
            break;

        case WM_PASTE:
            PasteSelection();
            fCallDef = FALSE;
            break;

        case WM_HSCROLL:
			HScroll( LOWORD ( wParam ), (long)HIWORD( wParam ) );
            fCallDef = FALSE;
            break;

        case WM_VSCROLL:
			VScroll( LOWORD ( wParam ), (long)HIWORD( wParam ) );
            fCallDef = FALSE;
            break;

        case EM_SELCHARS: {
            // Adjust the text and the cursor.
            // wParam == Line to put the cursor on.
            // lParam is as follows:
            //  For selection:
            //     x1 == start (anchor)
            //     x2 == end (cursor)
            //  For display:
            //     x2 == position for cursor on line
            //     x1 == distance from HIWORD for
            //           left margin.
            EMSC *  pemsc = (EMSC *) lParam;

            // Position cursor
			m_ipAnchor.oln = m_ipCur.oln = pemsc->y;

			// Put anchor into ip so the position may be
			// adjusted for DBCS, then put into anchor
            m_ipCur.ob = pemsc->x1;
			AdjustDbcsCursor( m_iAdjustDbcs );
            m_ipAnchor.ob = m_ipCur.ob;

            m_ipCur.ob = pemsc->x2;
			AdjustDbcsCursor( m_iAdjustDbcs );

			UpdateAnchor( MAKELONG( 0, KK_SHIFT ) );
			if ( !FMoveWindowToCursor( FALSE ) ) {
            	PaintLine( m_ipCur.oln );
            }
            fCallDef = FALSE;
            break;
        }

        case WM_TIMER:
			// If not the edit manager's timer, ignore.
			if ( wParam == idTimer ) {
	            wParam = 0;
    	        lParam = 0L;

            	if ( FIsActiveWindow( m_hWnd ) ) {
                	DoMouse( wMsg, wParamMouse, lParamMouse );
	            }
			}
			else if ( wParam == DATATIP_TIMER )
			{
				// This code is plagiarized from data tip handling in the
				// CTextView class.
				KillTimer(DATATIP_TIMER);

				if (CDataTip::GetTip())
                    CDataTip::GetTip()->DestroyTip();

				if (GetKeyState(VK_LBUTTON) < 0 || GetKeyState(VK_RBUTTON) < 0 ||
					GetKeyState(VK_MBUTTON) < 0)
						break;

				if (!DebuggeeAlive() || DebuggeeRunning())
					break;

				CPoint ptScreen;
				GetCursorPos(&ptScreen);
				CMultiEdit* pPointEdit = (CMultiEdit*)WindowFromPoint(ptScreen);

				if (pPointEdit != this)
					break;
				
				CPoint pt = ptScreen;
				ScreenToClient(&pt);

				WORD    obMouse;
				DWORD   olnMouse;
				WORD	obMacBuf;
				int     xMouse;
				int     yMouse;

				PosFromPoint( xMouse, yMouse, pt);
				obMouse = ( xMouse - m_cxSelectMargin ) / m_pFontInfo->m_tm.tmAveCharWidth + m_pdCur.obleft;
				olnMouse = (DWORD)( yMouse / m_pFontInfo->m_tm.tmHeight ) + m_pdCur.olntop;

				obMacBuf = ObMaxBuf();
				if ( obMouse > obMacBuf ) {
					obMouse = obMacBuf;
				}

				WORD obMic, obMac;
				char szExpr[cchEditMax * 2 + 4 ];
				BOOL fOk = TRUE;
				
				// If we have a selection and the cursor is in the selection use the
				// currently selected text. Otherwise just get the word at the current
				// cursor position.
				if ( FGetSelection(olnMouse, &obMic, &obMac) &&
					 FMouseHitSel(FALSE, MAKELPARAM(LOWORD(pt.x), LOWORD(pt.y) ))
				  )
				{
					CopySelection(szExpr, cchEditMax);
					RemoveSideEffectsExpr(szExpr);
				}
				else {
					fOk = GetWordAtPosition(obMouse, olnMouse, szExpr, cchEditMax, &obMic, &obMac);
				}	
						
				char buf[cchEditMax];
				buf[0] = 0;

				if (!fOk || !FEvalExpr(szExpr, buf, sizeof(buf)))
					break;

				// exclude uninteresting tips
				if (0 == buf[0] || 0 == _tcsicmp(szExpr, buf))
					break;

				strcat(szExpr, " = ");
				strcat(szExpr, buf);

				CWnd *pWnd = new CDataTip;

				if (!pWnd->CreateEx(0, AfxRegisterWndClass(CS_SAVEBITS|CS_HREDRAW,
						::LoadCursor(NULL, IDC_ARROW), NULL, NULL),
					&afxChNil, WS_POPUP|WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL))
				{
					delete pWnd;
					break;
				}

				pWnd->SetWindowText(szExpr);

				CPoint point(ptScreen);
				int yAdjust = 10;
				point.x -= 10;
				point.y += yAdjust; 	// Initial postion for the data tip.

				CRect rect;
				pWnd->GetWindowRect(rect);

				// Make sure that the datatip doesn't get clipped by the screen.
				int xScreenRight = ::GetSystemMetrics(SM_CXSCREEN);
				int yScreenBottom = ::GetSystemMetrics(SM_CYSCREEN);

				if ( point.x + rect.Width() > xScreenRight )
					point.x = xScreenRight - rect.Width();	
				if ( point.y + rect.Height()  > yScreenBottom )
					point.y -= yAdjust + yAdjust/2 + rect.Height() ;

				// show it and update it
				pWnd->SetWindowPos(NULL, point.x, point.y, 0, 0,
					SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
				pWnd->UpdateWindow();
			}


	        break;

		case EM_PTINSELECTION: {
			POINT *	ppt = (POINT *)lParam;

			lRet = (long)FMouseHitSel(
				(BOOL)wParam,
				(DWORD)MAKELPARAM( LOWORD( ppt->x ), LOWORD( ppt->y ) )
			);

			fCallDef = FALSE;
			break;
		}

        case WM_MOUSEMOVE:
			if ( GetCapture() == NULL && FMouseHitSel( FALSE, lParamMouse ) )
				SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
			else
            {
				HCURSOR 		hCursor;

				if ( (int)LOWORD( lParamMouse ) < m_cxSelectMargin ) {
					hCursor = AfxGetApp()->LoadCursor( IDC_RIGHT_ARROW );
				}
				else if (m_pFontInfo->m_tm.tmHeight <= 8) {
					hCursor = ::LoadCursor( hInst, MAKEINTRESOURCE( IDC_SMALL_I ) );
				}
				else {
					hCursor = ::LoadCursor( NULL, IDC_IBEAM );
				}

				SetCursor( hCursor );

			}
        	// fall through

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
			if (wMsg == WM_LBUTTONDOWN &&
				!( GetKeyState( VK_SHIFT ) & 0x8000 ) &&
				FMouseHitSel( FALSE, lParamMouse )
			) {
				POINTS	pts = MAKEPOINTS( lParamMouse );
				POINT	pt;

				pt.x = (LONG)pts.x;
				pt.y = (LONG)pts.y;
				DoDrag(pt);

				return (LRESULT)lRet;
			}
			// fall through

        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
//        case WM_RBUTTONUP:
  //      case WM_RBUTTONDBLCLK:
            if ( FIsActiveWindow( m_hWnd ) ) {
                DoMouse( wMsg, wParamMouse, lParamMouse );
            }
			else if ( wMsg == WM_LBUTTONDOWN ) {
				MakeActive( m_hWnd );
			}
            break;

        case WM_UNDO:
            DoChar( VK_CANCEL, 0L );
            fCallDef = FALSE;
            break;

        case EM_GETTEXT:
            lRet = LcbGetText( (WORD)wParam, (char far *)lParam );
            fCallDef = FALSE;
            break;

        case EM_GETWORD: {
			// CV used the last to pWords to get the boundaries for the
			// selected word.  Also, if the values were NULL, the search
			// criteria would look for the next word if the cursor was
			// on a white-space.  We don't want this behavior for SUSHI
			// so, pass in dummy args to ensure that the returned word
			// is where the cursor is not near.  This bogus value is
			// being passed in so the LcbGetWord code doesn't have to
			// change in the event we want that functionality back.
			WORD	wDummy;

            lRet = LcbGetWord( wParam, (char far *)lParam, &wDummy, &wDummy );
            fCallDef = FALSE;
            break;
		}

        case WM_SIZE:
            FMoveWindowToCursor( FALSE );

			// Intentional fall-through

		case WU_UPDATESCROLLBARS:
            ResizeScroll();
            --ob;
            --oln;
			UpdateCursorPos();
            break;

        case EM_MOVECURSOR:
            // Just update the cursor.  The window's wndproc has reset
            // the m_ipCur and pipAnchor.
			AdjustDbcsCursor( m_iAdjustDbcs );
            if ( !FMoveWindowToCursor( TRUE ) ) {
            	Paint( (PRRC)0, NULL );
			}
            --ob;
            --oln;
            break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
        case WM_COMMAND:
        case WM_INITMENU:
        case WM_INITMENUPOPUP:
        case WM_MENUSELECT:
        case WM_MENUCHAR:
            return GetParent()->SendMessage( wMsg, wParam, lParam );

        default:
            if ( wMsg == WM_VIEWACTIVATE )
                RedisplaySelection();

			fCallDef = TRUE;
    }

    // If we're not on the same line which we started out on, then
    // flush the cache.
    if ( m_pdCur.oln != m_ipCur.oln ) {
        FlushCache();
    }

    // Update the selection:  We can be smarter about this!
    m_fSelection = (BYTE)!MEFAreIPsEqual( &m_ipAnchor, &m_ipCur );
	if ( m_fSelection != fSelection ) {
		if ( fSelection ) {
			ShowCaret();
		}
		else {
			HideCaret();
		}
	}

    // Update the scrollbars

    // This code can be cleaned up if we restrict the pipCur
    // to always be contained within the file.  As in, don't
    // allow m_ipCur.oln to be > olnMac.

    if ( !( m_Style & ES_NOREDRAW ) ) {
    	DWORD	olnMac = CLinesInBuf();
		RRC	rrc;
		short	dy;
		short	dx;
		int 	cMax;
		int 	cMin;
		int 	iCur;
        BOOL    fVS;
        BOOL    fHS;

    	GetEditRrc( TRUE, &rrc );
	    if ( !( dy = rrc.ryBottom ) ) {
            ++dy;
        }
        if ( !( dx = rrc.rxRight ) ) {
            ++dx;
        }

		if ( olnMac != m_olnMacPrev ) {
			ResizeScroll();
		}

		if ( ( fVS = m_fScrollV ) && oln != m_ipCur.oln ) {
			if ( olnMac ) {
                GetScrollRange( SB_VERT, &cMin, &cMax );

				if ( cMax && (int)ScrollFromOln( m_ipCur.oln ) > cMax ) {
					m_ipCur.oln = olnMac - 1;
					UpdateCursorPos();
				}

				iCur = m_pdCur.olntop;

                if ( GetScrollPos( SB_VERT ) != ScrollFromOln(iCur) ||
					wMsg == WU_UPDATESCROLLBARS
            	) {
               		SetScrollPos( SB_VERT, ScrollFromOln(iCur), TRUE );
                }
            }
			else {
				SetScrollPos( SB_VERT, ScrollFromOln(olnMac), TRUE );
            }
        }

		if ( ( fHS = m_fScrollH ) && ob != m_pdCur.obleft ) {

			GetScrollRange( SB_HORZ, &cMin, &cMax );

			if ( m_pdCur.obleft > axMax ) {
				m_pdCur.obleft = m_ipCur.ob = axMax;
                UpdateCursorPos();
            }

			iCur = m_pdCur.obleft;
            if ( GetScrollPos( SB_HORZ ) != iCur ||
				wMsg == WU_UPDATESCROLLBARS
			) {
                SetScrollPos( SB_HORZ, iCur, TRUE );
            }
        }
    }

	if ( fCallDef ) {
		return DefWindowProc( wMsg, wParam, lParam );
	}

    return (LRESULT)lRet;
}

BOOL CMultiEdit::GetHelpWord(CString& strHelp)
{
    BOOL    lookAround = TRUE;
    char    sz[255 ];

    if ( GetCurrentText(&lookAround, (PSTR)sz, 255, NULL, NULL) &&
		lookAround &&
         !FWhitespaceOnly(sz, _tcslen(sz)) )
    {
        strHelp = sz;
        return( TRUE );
    }

    return( FALSE );
}

BOOL CMultiEdit::FWhitespaceOnly(PCSTR pCurLine, _CB cbCurLine)
{
    for (IB ib = 0; ib < cbCurLine; ib += _tclen(pCurLine + ib))
    {
        if ( !whitespace(*(unsigned char *)(pCurLine + ib)))
            return( FALSE );
    }

    return( TRUE );
}

