#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

void CDAMView :: ShiftTopLine (DWORD ln) {
	DWORD	dln = OlnGetCurPos() - OlnGetTop();
    //
    // since we are dorking around with the map
    // relative positioning means almost nothing unless
    // we base it on our current "best guess"
    // This is why I calculate the dln as a result of the old top line
    // and the old cursor position.  and since we may be changing the
    // top line drastically i have to use it as the relative postion.
    //
    SetWinTopLineCursor( ln, ObGetCurPos(), ln + dln );
}

/*** CDAMView :: SegmentLength **********************************************
 *
 * Purpose  : to calculate the length of the segment containing addr
 *          :
 * Input    : addr - The address contained in the segment of interest
 *          :        Note that this must be an unfixed-up address
 *          :
 * Output   : return - The length of the segment containing addr
 *          :
 * Notes    : This function is potentially called for every line draw
 *          :   so it must be FAST, but it calls osdebug which can
 *          :   potentially invoke a remote debug monitor to answer this
 *          :   question, so we cache the answer and return the cached
 *          :   value.
 *
 ****************************************************************************/
UOFFSET CDAMView :: SegmentLength ( ADDR addr ) {
    UOFFSET offBase;
    UOFFSET	offLim;
	ADDR	addrPC;
	DWORD	dwResult;
	UOFFSET	offPrev = GetAddrOff( m_dmi.addrSegLim );

    SYFixupAddr( &addr );

	// Only ask OSDebug for the limits if the address doesn't
	// match the cached value
	if ( GetAddrSeg ( addr ) != GetAddrSeg( m_dmi.addrSegLim ) ||
		emiAddr( addr ) != emiAddr( m_dmi.addrSegLim )
	) {
	    if ( lpprcCurr->stp == stpRunning && !(pDebugCurr->MtrcAsync() & asyncMem) ) {
			offLim = 0xFFFFFFFF;
		} else {
			OSDGetObjectLength ( hpidCurr, htidCurr, &addr, &offBase, &offLim );
		}

		// Set the offset to the limit so OSDCompareAddr has an address to
		// compare.
		SetAddrOff( &addr, offLim );

		// Copy ALL of the address info.  addr should be a fixed up address
		m_dmi.addrSegLim = addr;
	}

	// Retrieving cached information, initialize locals to determine if the PC
	// is out of range
	else {
		addr = m_dmi.addrSegLim;
		offLim = GetAddrOff( addr );
	}

	// Get the PC value to see if it is outside the limits
	SYGetAddr( hpidCurr, htidCurr, adrPC, (LPADDR)&addrPC );
	SYFixupAddr( &addrPC );
	
	// Both addrs should be fixed up so OSDCompAddr can succeed
	ASSERT( !ADDR_IS_LI( addr ) );
	ASSERT( !ADDR_IS_LI( addrPC ) );

	// PC is beyond the range, let the display show ????
	OSDCompareAddrs( hpidCurr, &addrPC, &addr, &dwResult );

	// OSDCompareAddrs (fCmpLT is also an error condition, so
	// look for fCmpGT with args above reversed to isolate the
	// error) -iin
	if ( dwResult == fCmpGT ) {
		// Copy all of the address information
		m_dmi.addrSegLim = addrPC;
		offLim = GetAddrOff( addrPC );
	}

	// If the offset limit changed, make sure that the scroll bar(s)
	// are updated to reflect a new length.  Post the message to ensure
	// the the map is completely updated before the scrollbars.
	if ( offPrev != offLim ) {
		PostMessage( WU_UPDATESCROLLBARS, (WPARAM)0, (LPARAM)0 );
	}

    return offLim;
}

UOFFSET CDAMView :: GetFixedOff ( ADDR addr ) {

    SYFixupAddr ( &addr );

    return addr.addr.off;
}

// Given an address, see if it is part of a source line.  If
// so, ensure that the line is in sync with the beginning of
// the source line.  This is only to be used when aligning
// addresses BACKWARDS.  paddrCur will be >= paddrPrev (fixed address).
// paddrCur is READONLY.
void CDAMView :: AlignAddrWithSource( PADDR paddrCur, PADDR paddrPrev ) {
	WORD	ln;
	SHOFF	db;
	SDI		sdi;
	DWORD	dwResult;
	ADDR	addrCur = *paddrCur;
	
	// If the current address doesn't start at the beginning of an source line,
	// align the addr with the beginning of the source statement.  Address
	// will be aligned below (see SetAddrOff(...))
	if ( SLLineFromAddr( paddrCur, &ln, NULL, &db ) && db != 0 ) {
		*paddrPrev = *paddrCur;
	}

	// Otherwise, align with a source address passed in
	else {
		if ( !SLLineFromAddr( paddrPrev, &ln, NULL, &db ) ) {
			// No source at this address.  Just begin at this address
			// w/o adjusting the offset.
			db = 0;
		}
	}

	// Need fixed up addr to used as boundary condition
	SYFixupAddr( &addrCur );

	// Set offset to be aligned with the current source line
	SetAddrOff( paddrPrev, GetAddrOff( *paddrPrev ) - db );

	// Initialize the SDI for unassembling
	sdi.dop = dopNone;
	sdi.addr = *paddrPrev;

	do {
		ADDR	addrPrev;

		// Copy entire address in case the emi changes.  This is
		// the previous assembly address.  When we exit this loop
		// it will be the last instruction which began at < addrCur
		*paddrPrev = sdi.addr;

		// Unassemble next instruction
		SYUnassemble( hpidCurr, htidCurr, &sdi );

		// Copy to temp to fixup for compare
		addrPrev = sdi.addr;
		SYFixupAddr( &addrPrev );
		OSDCompareAddrs( hpidCurr, &addrPrev, &addrCur, &dwResult );

	// Exit once we've unassembled beyond or at the end point
	} while( dwResult == fCmpLT );
}

void CDAMView :: DrawLineGraphics( CDC * pdc, RECT & rect, DWORD oln )
{
	DrawMarginGlyphs( pdc->m_hDC, rect, m_WidgetFlags );
}

/*** CDAMView :: SetAttributes **********************************************
 *
 * Purpose  : Set the static structure denoting current line attributes
 *          :   according to the presence of CS:IP and any breakpoints
 *          :   between the specified addresses.
 *          :
 * Input    : hwnd  - the window to move
 *          : paddr - the address to move to
 *          :
 * Output   : none
 *
 ****************************************************************************/

// check for disabled breakpoints
#define CHECK_DISABLED TRUE

void CDAMView :: SetAttributes ( PDML pdml ) {
	m_WidgetFlags = 0;
	FMT_ELEMENT *rgfmtel = pfmtelFromDocType((DOCTYPE)DISASSY_WIN);
	// FMT_ELEMENT *rgfmtelSource = pfmtelFromDocType((DOCTYPE)DOC_WIN);

	switch( pdml->met ) {
		case metSource:
        	m_dla.pElement = gpISrc->GetSourceColor(CLR_TEXT);
			break;

		case metFileName:
        	m_dla.pElement = gpISrc->GetSourceColor(CLR_COMMENT);
			break;

		case metDisasm:
			m_dla.pElement = &(rgfmtel[FMTEL_DASM]);

			// Breakpoint address?
            UINT uiWidgetFlag;
			if ( BPNbrAtAddr(
                    &pdml->addr,
                    1,
                    CHECK_DISABLED,
                    &uiWidgetFlag ) != -1
            ) {
				m_WidgetFlags |= uiWidgetFlag;
	        	m_dla.pElement = gpISrc->GetSourceColor(CLR_BREAKPT);
		    }

			// Current line?
		    if (FAddrsEq(m_dmi.addrCSIP, pdml->addr)) {
				m_WidgetFlags |= CURRENT_LINE;
	        	m_dla.pElement = gpISrc->GetSourceColor(CLR_CURRENTLINE);
			}
			// Viewed line from callstack setting?
			else if (FAddrsEq(m_dmi.addrCall, pdml->addr)) {
				m_WidgetFlags |= CALLER_LINE;
				m_dla.pElement = &(pfmtelFromDocType((DOCTYPE)CALLS_WIN)[FMTEL_CSHI]);
			}

			// Either widgets OR text hiliting, not both
			if ( m_cxSelectMargin )
				m_dla.pElement = &(rgfmtel[FMTEL_DASM]);
			else
				m_WidgetFlags = 0;

			break;

		default:
			m_dla.pElement = &(rgfmtel[FMTEL_TEXT]);
			break;
	}
}


/*** CDAMView :: SetCsip ****************************************************
 *
 * Purpose  : To display the csip.
 *          :
 * Input    : hwnd  - the window of interest
 *          : paddr - the new csip.
 *          : bForce - We want to reset the map even if it is currently visible.
 *          :
 * Output   : return - Always returns TRUE.
 *          :
 * Side     : Set the m_dmi.addrCSIP to *paddr.
 *  effects :
 *          : If the new csip is not currently visible, reset the map
 *          :   to the csip.  Then draw the entire window.
 *          :
 *          : Otherwise, just redraw the old csip line if visible, and
 *          :   the new csip line, which is visible by definition at
 *          :   this point.
 *
 ****************************************************************************/

BOOL CDAMView :: SetCsip( PADDR paddr, BOOL bForce) {

	BOOL	fVisible = FALSE;
	WORD	idmlOld = m_dmi.cdml;
	WORD	idmlNew = m_dmi.cdml;
	ADDR	addr    = m_dmi.addrCSIP;
	WORD 	wLine;
	PDML	pdml;
	PDML	pdmlMax;

	pdml = &m_dmi.rgdml[ OlnGetTop() - m_dmi.lnMapStart ];
	pdmlMax = min( pdml + (WORD)WinHeight(), &m_dmi.rgdml[ m_dmi.idmlInvalid ] );
	ASSERT( pdml == pdmlMax || pdmlMax - m_dmi.rgdml <= m_dmi.idmlInvalid );

    m_dmi.addrCSIP = *paddr;
    if ( m_dmi.idmlInvalid == 0 ) {
        goto draw;
    }

	while( pdml < pdmlMax ) {
		ADDR addrStart = { 0 };
		ADDR addrT     = { 0 };

		if (
			pdml->met == metDisasm &&
			RangeFromPdml ( pdml, &addrStart, &addrT )
		) {
			if (
				GetAddrSeg ( *paddr ) == GetAddrSeg ( addrStart ) &&
				GetAddrOff ( *paddr ) == GetAddrOff ( addrStart ) &&
				emiAddr ( *paddr ) == emiAddr (addrStart)
			) {

				idmlNew = (int)( pdml - m_dmi.rgdml );
				fVisible = TRUE;

				if ( idmlOld < m_dmi.cdml ) {
					break;
				}
			}

			if (
				GetAddrSeg ( addr ) == GetAddrSeg ( addrStart ) &&
				GetAddrOff ( addr ) == GetAddrOff ( addrStart ) &&
				emiAddr ( addr ) == emiAddr (addrStart)
			) {
				idmlOld = (int)( pdml - m_dmi.rgdml );

				if ( fVisible ) {
					break;
				}
			}
		}
		++pdml;
	}

	addr = *paddr;
	if ( SLLineFromAddr ( &addr, &wLine, NULL, NULL )) {
		m_dmi.lnLastCSIP = wLine - 1;
	}

draw:

    if ( !fVisible || bForce ) {
        SetMapFromAddr ( paddr, TRUE );
		InvalidateRect( (LPRECT)NULL, FALSE );
    }
    else if ( m_dmi.fNewMap ) {
        DrawWindow ();
    }
    else if ( idmlOld != idmlNew )  {

        if ( idmlOld < m_dmi.cdml ) {
            DrawLine ( m_dmi.lnMapStart + idmlOld );
        }

        DrawLine ( m_dmi.lnMapStart + idmlNew );
    }

    //
    // Set the cursor to the CSIP marker
    //
    if ( fVisible ) {
        //
        // it is visible so make sure we redraw with hightlight.
        //
        DrawLine ( m_dmi.lnMapStart + idmlNew );
        SetCursorAt( 0, m_dmi.lnMapStart + idmlNew );
    }

    // This is no longer needed!
    m_dmi.fNewMap = FALSE;
    return TRUE;

}


/*** CDAMView :: SetAddress *************************************************
 *
 * Purpose  : To move the hwnd window to the address *paddr
 *          :
 * Input    : hwnd  - the window to move
 *          : paddr - the address to move to
 *          :
 * Output   : return - Always returns TRUE
 *          :
 * Side     : This will cause a remap to occur.  The top item of the map
 *  effects :   will correspond to *paddr.
 *
 ****************************************************************************/

long CDAMView :: SetAddress( PADDR paddr ) {
    SetMapFromAddr ( paddr, FALSE );
	SetCursorAt( 0, OlnGetCurPos() );
	InvalidateRect( (LPRECT)NULL, TRUE );

    return TRUE;
}

/*** GotoTopOrEnd
*
* Purpose: Support for control-end
*
* Input:
*
* Output:
*  Returns
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
void CDAMView :: GotoTopOrEnd ( BOOL fEnd ) {
	PDML		pdml = &m_dmi.rgdml[ OlnGetCurPos() - m_dmi.lnMapStart ];
	ADDR    	addr = pdml->addr;
	UOFFSET 	uOff = 0;

	ASSERT( pdml->met != metNone );

	if ( fEnd ) {
		// requires UNFIXED address!
		uOff = SegmentLength( addr );
	}
	SYFixupAddr( &addr );

    SetAddrOff ( &addr, uOff );
    SYUnFixupAddr ( &addr );
    SetMapFromAddr ( &addr, FALSE );
}

/*** CDAMView :: GotoEnd
*
* Purpose: Support for control-end
*
* Input:
*
* Output:
*  Returns
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
void CDAMView :: GotoEnd () {
    GotoTopOrEnd ( TRUE );
}

/*** CDAMView :: GotoTop
*
* Purpose: Support for control-home
*
* Input:
*
* Output:
*  Returns
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
void CDAMView :: GotoTop () {

    GotoTopOrEnd ( FALSE );
}

/*** CDAMView :: ResetMap
*
* Purpose: Reset the map
*
* Input:
*
* Output:
*  Returns
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
void CDAMView :: ResetMap ( DWORD ln ) {
	ADDR	addr;

	// We are off of the map, should only happend when someone
	// does a start/end or SB_THUMBPOS, reset the map by address.
	// We lie and set it to 2 * the current addresses offset.  Why?
	// Beats me.  That's what CV did, so why ask why?
	addr = m_dmi.rgdml[ 0 ].addr;
	SYFixupAddr( &addr );
	if (pDebugCurr && pDebugCurr->IsJava ())
	{
		ADDR *paddr = &m_dmi.rgdml [0].addr;
		CXT cxt;
		HSF hsf;

		// attempt to derive its file
		if (m_dmi.fAnnotate && SHSetCxt (paddr, &cxt) &&
			(hsf = SLHsfFromPcxt (&cxt)))
		{
			WORD wLine = 0;
			SLLineFromAddr (paddr, &wLine, NULL, NULL);

			if (ln > wLine)
			{
				ln -= wLine;
			}
			else
			{
				ln = 0;
			}
		}

		// un-fudge the address
		SetAddrOff( &addr, (ln * 4) / 3);
	}
	else
	{
	    SetAddrOff( &addr, ln << 1 );
	}
	SYUnFixupAddr( &addr );
	SetMapFromAddr( &addr, FALSE );
}

// Set the cursor position to the specified address
// 	1) If visible
//	2) met==metDisasm
BOOL CDAMView :: FSetCursorAtAddr( PADDR paddr ) {
	BOOL	fSet = FALSE;

	// Make sure that the map is valid
	if ( m_dmi.lnMapStart ) {
		int		idmlMin;
		int		idmlMac;
		PDML	pdml;

		// index to the dml at the top of the visible window
		idmlMin = OlnGetTop() - m_dmi.lnMapStart;

		// index to the last+1 valid dml in the visible window
		idmlMac = min( idmlMin + (int)WinHeight(), m_dmi.idmlInvalid );
		pdml = &m_dmi.rgdml[ idmlMin ];

		// Loop until we find what we're looking for or run out off
		// valid dmls
		while( !fSet && idmlMin < idmlMac ) {

			// Match condition is addresses are same and display
			// item is code (not label, file, or source)
			if ( pdml->met == metDisasm &&
				GetAddrSeg( *paddr ) == GetAddrSeg( pdml->addr ) &&
				GetAddrOff( *paddr ) == GetAddrOff( pdml->addr ) &&
				emiAddr( *paddr ) == emiAddr( pdml->addr )
			) {
				// Found a match, set the cursor and get out!
				fSet = TRUE;
				SetCursorAt(
					0,
					m_dmi.lnMapStart + idmlMin
				);
			}
			++pdml;
			++idmlMin;
		}
	}
	return fSet;
}

// Drag and drop support.  Call this function when
// a string is available for dropping to set the
// DAM window's top line to
void DAMDropNewAddress( LPSTR lszExpr, DWORD cbData ) {

	extern BOOL FAddrFromSz( char *, ADDR * );

		// Only support this when debugging
	if ( DebuggeeAlive() ) {
		ADDR	addr = {0};
		TCHAR	sz[ axMax ];
		TCHAR *	ptch = sz;
		int		radixInputSave;

		// To solve the problem that the dialog may be initialized
		// with text from the disassembly window, change the default
		// input radix to hex.  When this code exits, the radix HAD
		// BETTER BE RESTORED!!!!
		radixInputSave = radixInput;
		radixInput = 16;

		// Copy the string into local memory and only
		// replace an eol or end of buffer with a zero
		// terminator since FAddrToSz only allows for
		// a single-line zero terminated string
		memset( sz, 0, sizeof( sz ) );
		_ftcsncpy(
			sz,
			lszExpr,
			(size_t)min( (DWORD)sizeof( sz ) - 1, cbData )
		);
		while( *ptch && *ptch != '\n' && *ptch != '\r' ) {
			ptch = _tcsinc( ptch );
		}
		*ptch = '\0';

		// Skip leading zeroes.  We don't want to force this to octal
		// and this will clearly be a bug if the user actually wanted
		// to enter octal!
		ptch = sz;
		if ( *sz == '0' ) {

			while( *ptch && *ptch == '0' ) {
				++ptch;
			}

			// The zero is required for specifying hex or dex (ie 0n10 or 0x1fa)
			if ( ptch > sz &&
				( !*ptch || *ptch == 'n' || *ptch == 'N' || *ptch == 'x' || *ptch == 'X' )
			) {
				--ptch;
			}
	 	}

		// If the address parses ok, fill in a cxt
		// packet and update the window
		if ( FAddrFromSz( ptch, &addr ) ) {
			CXF	cxf = {0};

			*SHpADDRFrompCXT( SHpCXTFrompCXF( &cxf ) ) = addr;

			((CDAMView *)pViewDisassy)->UpdateView( &cxf );
		}
		else {
			MessageBeep( 0 );
		}

		// Make sure that the radix gets restored!!!
		radixInput = radixInputSave;
	}
}

void CDAMView :: UpdateView( PCXF pcxf ) {

	ResizeMapToWindow();

	if ( m_dmi.cdml && DebuggeeAlive() ) {
		PADDR	paddr;
		ADDR	addrPC;

		SYGetAddr( hpidCurr, htidCurr, adrPC, (LPADDR)&addrPC ) ;

		if ( pcxf ) {
			ADDR	addrFixed;

			paddr = SHpADDRFrompCXT( SHpCXTFrompCXF( pcxf ) );

			// If a particular address cannot be fixed up (e.g. it's
			// for a DLL which has not yet been loaded), then you're
			// not allowed to call UpdateView with that address
			addrFixed = *paddr;
			if (!SYFixupAddr(&addrFixed)) {
				paddr = NULL;
				pcxf = NULL;
			}
		}
		else {
			if ( !FAddrsEq( addrPC, m_dmi.addrCSIP ) ) {
				// The PC changed and no cxt specified, reset the
				// window to the PC location anyway
				pcxf = &cxfIp;
				paddr = SHpADDRFrompCXT( SHpCXTFrompCXF( pcxf ) );
			}
			else {
				// No context passed in.  Assume that code data has changed
				// and force the map to be recalculated and displayed
				DWORD	lnTop = OlnGetTop();
				int		idml = (int)( lnTop - m_dmi.lnMapStart );

				ASSERT( m_dmi.rgdml[ idml ].met != metNone );

				// Align the top of the map with the top of the window (visible)
				if ( idml ) {
					m_dmi.rgdml[ 0 ] = m_dmi.rgdml[ idml ];
					m_dmi.lnMapStart = lnTop;
				}
				m_dmi.idmlInvalid = 1;

				paddr = NULL;
				InvalidateRect( (LPRECT)NULL, FALSE );
			}
		}

		// Update the caller hilite address
		// FUTURE: should this be done when pcxf is NULL???
		m_dmi.addrCall =
			*SHpADDRFrompCXT( SHpCXTFrompCXF( PcxfLocal() ) );

		BOOL bForce = FALSE;

		if ( !DebuggeeRunning() && !m_bAskForFile )
		{
			m_bAskForFile = TRUE;
			bForce = TRUE;
		}

		if ( paddr ) {
			// Update to the current PC
			if ( FAddrsEq( addrPC, *paddr ) ) {
				SetCsip ( paddr, bForce );

				// Just make sure that the cursor is placed on the PC address as
				// we trace/step
				FSetCursorAtAddr( paddr );
			}
			else {
				// Address specified -- make sure that the offset is valid for
				// the selector
				if ( GetAddrOff( *paddr ) <= SegmentLength( *paddr ) ) {
					if ( !FSetCursorAtAddr( paddr ) ) {
						SetAddress ( paddr );
					}
				}

				// Invalid address specified, should probably bring up a message box
				// but for now, just beep!
				else {
					MessageBeep( 0 );
				}
			}
		}
	}
}

void CDAMView :: DestroyMap() {

	DAMUnloadAll();

	m_dmi.lnMapStart = 0L;
	m_dmi.lnEnd = 0L;
	m_dmi.idmlInvalid = 0;
	m_dmi.lnLastCSIP = 0L;
	memset( &m_dmi.addrCSIP, 0, sizeof( ADDR ) );
	if ( m_dmi.rgdml ) {
		_ffree( m_dmi.rgdml );
		m_dmi.cdml = 0;
		m_dmi.rgdml = (PDML)NULL;
	}
}

GCAF CDAMView :: GcafGetCursorAddr( PGCAINFO	pgcai ) {
	GCAF	gcaf = gcafNone;

	if ( m_dmi.cdml ) {
		DWORD	olnCur = OlnGetCurPos();
		PDML	pdml = &m_dmi.rgdml[ olnCur - m_dmi.lnMapStart ];

		if ( pdml->met == metDisasm || pdml->met == metSource ) {
			pgcai->addr = pdml->addr;
			gcaf = gcafAddress;
		}
	}
	return gcaf;
}

// The macro is used below to make the map
// smaller for debugging to debug map update
// errors.  W/o this, the map is too big to
// effectively/efficiently debug
#ifdef DEBUGDAM
// Height + 2.  This allows for a partial
// (clipped) line and a possible 1 scroll
#define	MapSizeFromHeight(cln)	((cln)+2)
#else	// DEBUGDAM
// Height * 5.  Big map so user can have
// a reasonable selection (and scroll)
#define	MapSizeFromHeight(cln)	((cln)*10)
#endif	// DEBUGDAM

void	CDAMView :: ResizeMapToWindow() {
	int		cdmlNeed = MapSizeFromHeight( (int)WinHeight() );
	PDML	pdmlOld = m_dmi.rgdml;

	// Not yet initialized, make sure that we don't
	// allocate too small a map.
	if ( !m_dmi.cdml ) {
		cdmlNeed = max( cdmlNeed, cdmlMin );
	}

	// Always grow the map, never shrink.  This way
	// the map never needs to get dorked with.  All
	// indicies and pointers don't need to change
	if ( cdmlNeed > m_dmi.cdml ) {
		PDML	pdml;

		// Realloc behaves as malloc if the item to
		// be realloc'd is null!
		pdml = (PDML)_frealloc(
			m_dmi.rgdml,
			(size_t)cdmlNeed * sizeof( DML )
		);

		// pdml will be zero only when the realloc
		// couldn't happen, in which case we will
		// use what was there before.  This will
		// really ugly up the display, but things
		// will still mostly work (YUK!)
		if ( pdml ) {
			m_dmi.rgdml = pdml;
			m_dmi.cdml = cdmlNeed;

			// This is the first time here, initialize
			// the map at the the current view frame
			// (either the PC or a callstack)
			if ( !pdmlOld && IsWindowVisible() ) {
				UpdateView( PcxfLocal() );
			}
		}
	}
}

void	CDAMView :: ChangeOpts() {
	// Make sure that there's something to change
	if ( m_dmi.rgdml ) {
		BOOL	fPaint = FALSE;
		BOOL	fReset = FALSE;

		// If the codebytes or case changed, just repainting
		// will cause the correct updates
		if ( runDebugParams.fDAMCodeBytes != m_dmi.fDoCodeBytes ||
			runDebugParams.fDAMLowerCase != m_dmi.fLowerCase ) {

			fPaint = TRUE;
		}

		// If symbols changed, need to reset since the visible
		// data may have been showing publics (areas w/o source)
		if ( runDebugParams.fDAMSymbols != m_dmi.fDoSymbols ) {
			fReset = TRUE;
		}

		// Annotations changed, always reset the map
		if ( runDebugParams.fDAMAnnotate != m_dmi.fAnnotate ) {
			fReset = TRUE;
		}

		m_dmi.fAnnotate = runDebugParams.fDAMAnnotate;
		m_dmi.fDoCodeBytes = runDebugParams.fDAMCodeBytes;
		m_dmi.fLowerCase = runDebugParams.fDAMLowerCase;
		m_dmi.fDoSymbols = runDebugParams.fDAMSymbols;

		if ( fReset ) {
			// When we reset, grab the top visible line of the window
			// to use as the new base address
			int		idmlTop = (int)( OlnGetTop() - m_dmi.lnMapStart );
			ADDR	addr = m_dmi.rgdml[ idmlTop ].addr;

			// After resetting the map, move the cursor to the same relative
			// position within the window
			WORD	dob = ObGetCurPos() - ObGetLeft();
			DWORD	doln = OlnGetCurPos() - OlnGetTop();

	        SetMapFromAddr ( &addr, TRUE );

			SetCursorAt(
				ObGetLeft() + dob,
				OlnGetTop() + doln
			);
		}

		if ( fPaint || fReset ) {
			InvalidateRect( NULL, FALSE );
		}
	}
}
