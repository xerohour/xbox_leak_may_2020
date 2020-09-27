//////////////////////////////////////////////////////////////////////////
//
//	MWUTIL.C - Memory Window Handler
//
//	Copyright (C)1990, Microsoft Corporation
//
//	Purpose: To display and update the memory windows.
//
// 	Notes:
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//// SetAttributes
//
// Purpose: Change display attributes for specified memory window
//
// Input:
//	iMwType	display type (index into rgmwd table).  If MWNINCH, then
//			do not change display type.
//	szExpr		New expression for window.  If NULL, use old expr.
//	usOpts		Make expression live if TRUE.  If MWNINCH, don't
//			alter setting.
//	wRaw		Show raw data along with the formatted values if TRUE.
//			If FALSE, only show the formatted data.  If MWNINCH,
//			don't alter setting.
// cFixedFmt	If not MWNINCH, set the number of args (!= 0).
//
// Output: Window may be redisplayed if any formats or the display address
// has changed.
//
// Exceptions:
//
// Notes: There are some diferences between overlapped and non-overlapped
// behaviors.  The overlapped version has the scroll bars as children of
// the border window.  The non-overlapped version has the scroll bars as
// children of the TEXT window.  To force repaints, of the border (with
// the display information), we send a WM_PAINT message to the parent.
// We don't want to call DrawWindow( PwndParent() ) becuase this would
// cause a duplicate painting of the memory window.  The
// MESetWinTopLineCursor() call forces a repaint of the text window only.
//
// To correctly paint the window, the non-overlapped model needs to send
// WM_PAINT messages to the scroll bars, since just painting the border
// MAY cause the scroll bar(s) to get over painted and temporarily disappear.
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: SetAttributes(
WORD	iMwType,
char *	szExpr,
USHORT	usOpts,
WORD	wRaw,
USHORT	cFixedFmt ) {
	ADDR	addrT;
	DWORD	dln;
	DWORD	iln;
	WORD	ich;
	BOOL	fUpdate;	// Do we need to repaint?

	if ( emiAddr ( m_mwi.addr ) == 0 ) {
		// clear the segment part of the addr, so if only the
		// bottom two bytes are filled in, the segment will still be valid
		GetAddrSeg(m_mwi.addr) = 0;

		OSDGetAddr ( hpidCurr, htidCurr, adrData, &m_mwi.addr );
	}

	// Nothing has changed, so don't require any forced updating.
	fUpdate = FALSE;

	// Get the current cursor position within the file and the
	// vertical offset cursor positon within the visible window.
	iln = OlnGetCurPos();
	dln = iln - OlnGetTop();
	ich = ObGetCurPos();

	// Get the address of the line where the cursor is. If we're going
	// to change everything but the address, we will make sure that
	// the cursor is on the same line and same horizontal offset
	// position within the line after we've udpated.
	MWAddrFromIln( &m_mwi, iln, &addrT );

	// Change display type - Force a new cursor position and display
	// data information.
	if ( iMwType != MWNINCH && m_mwi.pmwd != &rgmwd[ iMwType ] ) {
		m_mwi.pmwd = &rgmwd[ iMwType ];
		fUpdate = TRUE;
	}

	if ( wRaw != MWNINCH && m_mwi.fShowRaw != (BOOL)wRaw ) {
		m_mwi.fShowRaw = (BOOL)wRaw;
		fUpdate = TRUE;
	}

	if ( cFixedFmt != MWNINCH && ( m_mwi.cFixedFmt != cFixedFmt ||
		m_mwi.cFmtVals != cFixedFmt ) ) {
		m_mwi.cFixedFmt = cFixedFmt;
		fUpdate = TRUE;
	}
	// Rebuild any display information necessary.  We may not be forcing
	// an update, but, if the screen size has changed, we may need to
	// redisplay if the number of formatted values per line has changed.
	fUpdate |= FUpdateInfo( fUpdate );

	// If something has been altered to set fUpdate, then we need to
	// set the addr packet to where we left off.
	if ( fUpdate ) {
		if ( GetAddrSeg( m_mwi.addr ) != GetAddrSeg( addrT ) ) {
			ADDR	addrT1;

			MWAddrFromIln( &m_mwi, 1, &addrT1 );
			m_mwi.addr = addrT1;
		}
		else {
			m_mwi.addr = addrT;
		}
        UncacheData();
	}

	// Change expression.  If there's an old one, free it's memory.
	// Here, only the address is getting updated, so don't force a
	// display data info. update.
	if ( szExpr ) {
		char *	psz;

		psz = (char *)NULL;
		if ( m_mwi.szExpr ) {
			psz = m_mwi.szExpr;
		}
		if ( !( m_mwi.szExpr = _ftcsdup( szExpr ) ) ) {
			OOMError();
			return FALSE;;
		}
		fUpdate = TRUE;

		// Re-evaluate the expression.
		if ( MWSetAddrFromExpr( &m_mwi, MW_NO_AUTO_TYPE ) ) {
			// if fRetry is TRUE we are going to try again
			// with the string.
			if ( psz) {
				free( psz );
                psz = NULL;
			}
		}
		else {
			free( m_mwi.szExpr );
            m_mwi.szExpr = psz;
            // return FALSE;
		}

		// Change the addr packet for the line to leave the cursor on.
		addrT = m_mwi.addr;
	}

	// Just update the address if the expression is live
	if ( !szExpr && m_mwi.fLiveExpr ) {
		MWSetAddrFromExpr( &m_mwi, MW_NO_AUTO_TYPE );
		addrT = m_mwi.addr;

        // Foce the update of the cursor position
        fUpdate = TRUE;
	}

	// Set expression to be living or static.  Doesn't require a
	// redisplay, only save value for reevaluating expression between
	// debugge trace, step, and go commands.
	if ( usOpts != MWNINCH ) {
		m_mwi.fLiveExpr = usOpts;
	}

	if ( fUpdate ) {
		char	sz[ axMax ];
        WORD	ichMax;
		WORD	obLeft;
		WORD	dx = min( WinWidth(), (WORD)axMax );

		iln = MWIlnFromAddr( &m_mwi, &addrT );

		// If iln < dln, there's not enough space to display in
		// the offset position, so display the iln(th) line at
		// the iln(th) vertical position.
		if ( iln < dln ) {
			dln = 0L;
		}
		// If the cursor is now positioned beyond the end of the
		// line, set it to the end of the line
		FormatLine( OlnGetTop(), sz );
		ichMax = _ftcslen( sz );
        if ( ich > ichMax ) {
			ich = ichMax;
		}
		if ( ich >= dx ) {
			obLeft = ich - dx + 1;
		}
		else {
			obLeft = 0;
		}

		SetWinTopLineCursor( iln - dln, obLeft, iln );
		SetCursorAt( ich, iln );
		ChangeTitle();
	}

	InvalidateRect( NULL, FALSE );

	return TRUE;
}

//// MWMemWSet
//
// Purpose: Same as memset, but WORD sized instead of BYTE
//
// Input:
//  lpw     Buffer to set
//  wVal    Value to set buffer to
//  cw      Number of WORDS to set
//
// Output: none
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void PASCAL MWMemWSet(
WORD FAR *  lpw,
WORD        wVal,
size_t      cw ) {
    while( cw-- ) {
        *lpw++ = wVal;
    }
}

//// FUpdateInfo
//
// Purpose: Update the information about the window.  This runction should
// only be called by MWSetAttributes().
//
// Input:
//		fForceChange	Force data to be updated.  This should be set,
//						if the display information has changed.  This
//						will only be false if the address expression
//						and/or the dead/live status has changed and
//						nothing else.
//
// Output: Returns TRUE if display data has changed, else FALSE.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: FUpdateInfo( BOOL fForceChange ) {
	WORD		rgwBuf[ cchBufMax ];
	const MWD * pmwd;
	USHORT		imwd;
	USHORT		cchAddr;
	USHORT		cchSpace;
	USHORT		cchData;
	USHORT		cchExtra;
	WORD		wRaw;
	WORD		wFmt;
	USHORT		cCell;
	USHORT		cCellRaw;
	WORD *		pwEnd;
	USHORT		cFmtLast;
	WORD *		pwLineP1;
    USHORT      us;
	DWORD		dw;
	DWORD		dwT;
    DWORD       lcbMax;
	WORD		dx;
	register WORD *	pw;

	pmwd = m_mwi.pmwd;

	imwd = pmwd - rgmwd;

    // Set cchAddr
    m_mwi.cchAddr = ADDR_IS_OFF32( m_mwi.addr ) ? cchAddr32 : cchAddr16;
	// Overhead (address and space between address and 1st data item
	cchAddr = m_mwi.cchAddr + cchPad;

	// Field width (normal)
	cchData = pmwd->cchFormatted;

	// cchExtra is extra space overhead for BYTE displays.  No other
	// formats use it.  When calculating the number of items which
	// fit on a line, the formula throws out the final trailing pad,
	// but for BYTE displays, the pad is consumed in the two spaces
	// between the last byte and the first ASCII field
	cchExtra = 0;
	
	// cchSpace is the padding between data items.
	//		ASCII or WCHAR w/o RAW data will have zero
	//		BYTE will have one
	//		all others will have cchPad
	//
	// The code to do this will be done below

	// Byte -- only 1 space between data items
	if ( imwd == MW_BYTE ) {
		// For BYTE display, also add in ASCII data for double data display.
		// This could be done below, but this simplifies the source code
		// and we don't need to have an Assertion that cchPad == 2
		++cchData;

		// Also, there is an extra space between the BYTE and the ASCII
		// data, so change the overhead
		++cchAddr;

		cchExtra = cchSpace = 1;
	}

	// Non-character (ASCII or wchar_t), normal spacing OR
	// character data with RAW
	else if ( !IsCharFormat( imwd ) || m_mwi.fShowRaw ) {
		cchSpace = cchPad;
	}
	else {
		cchSpace = 0;
	}

	// Now add in space for RAW data
	if ( m_mwi.fShowRaw && imwd != MW_BYTE ) {
		cchData += pmwd->cbType * cchRaw + cchPad;
	}

	// Make sure that xRight is within range
	dx = min( (WORD)axMax, WinWidth() );

	// Fixed format, just make sure that the output line doesn't exceed max
	if ( m_mwi.cFixedFmt ) {

		// The max number of items per line is the total number of
		// character less the line overhead (address) less the padding
		// between data items (padding is put after the data, so there
		// is 1 extra pad at the end of the info) divided by the number
		// of charcters to display and item plus the space between items.
		// cchExtra is described above
		us = min(
			m_mwi.cFixedFmt,
			((USHORT)axMax-cchAddr-cchSpace+cchExtra) / (cchData+cchSpace),
		);
	}
	else if ( (USHORT)dx > cchData ) {
        // Same formula as above, except look at width of window instead of
		// edit manager line max
		us = max(
			((USHORT)dx-cchAddr-cchSpace+cchExtra) / (cchData+cchSpace),
			1
		);
	}
	// Variable format where window is more narrow than 1 item.  Force to 1.
    else {
        us = 1;
    }

	// Save the old count to compare if we've changed
	cFmtLast = m_mwi.cFmtVals;

	lcbMax = ( ADDR_IS_OFF32( m_mwi.addr ) ? 0xffffffffL : 0x10000L );
	// Make sure that we do boof EditMgr with too many lines
	dw = (DWORD)( pmwd->cbType * us );
	while( ( lcbMax / dw ) > 0x7FFFFFFFL ) {
		dw += pmwd->cbType;
		++us;
	}
	dwT = lcbMax / dw;
    if ( dwT * dw != lcbMax ) {
		++dwT;
	}

	m_mwi.cLines = dwT;

	m_mwi.cFmtVals = us;

    if ( MWIlnFromAddr( &m_mwi, &m_mwi.addr ) >= dwT ) {
        ++m_mwi.cLines;
    }
    else {
        ADDR    addrT;

        MWAddrFromIln( &m_mwi, dwT - 1, &addrT );

        if ( ADDR_IS_FLAT( addrT ) && !GetAddrOff( addrT ) ||
            !LOWORD( GetAddrOff( addrT ) ) ) {

            --m_mwi.cLines;
        }
    }
	fForceChange |= ( cFmtLast != us );

	// If we're forced to update the information or the # of values/line
	// have changed, update the display information.
	if ( fForceChange ) {
		if ( m_mwi.rgwLineInfo ) {
			_ffree( m_mwi.rgwLineInfo );
            m_mwi.cwLineInfo = 0;
		}
		if ( m_mwi.rgf ) {
			_ffree ( m_mwi.rgf );
		}
		//
		// Build rgwLineInfo
		// The format line data is done as follows:
		//   1)  The address area and all places where there are
		//       fixed spaces are, the value "wNoType" is placed.
		//   2)  Any place on the line where a BYTE value appears, a
		//       wFmtLoMin is placed.
		//   3)  All remaining positions where non-byte values
		//       appear, will be filled with wFmtHiMin.
		//   4)  The line data is preceeded and terminated by a
		//       wNull.
		//
		// This formatting of data allows for FAST tabbing to
		// next field upon character input.
		wFmt = imwd == MW_BYTE ? wFmtLoMin : wFmtHiMin;
		wRaw = wFmt ^ wFmtMask;

		pw = rgwBuf;
		*pw++ = wNull;
		pwLineP1 = pw;
		MWMemWSet( pw, wNoType, m_mwi.cchAddr + cchPad );
		pw += m_mwi.cchAddr + cchPad;

		for( cCell = m_mwi.cFmtVals; cCell; --cCell ) {

			// Add format info for raw data.
			if ( m_mwi.fShowRaw && imwd != MW_BYTE ) {
				for( cCellRaw = pmwd->cbType; cCellRaw;
					--cCellRaw ) {

					*pw++ = wNoType;
					MWMemWSet( pw, wRaw++, cchRaw - 1 );
					pw += cchRaw - 1;
				}
				ASSERT( cchPad == 2 );
				*pw++ = wNoType;
				*pw++ = wNoType;
			}

			// Append the information for the actual formatted
			// data here
			MWMemWSet( pw, wFmt++, pmwd->cchFormatted );
			pw += pmwd->cchFormatted;
			pwEnd = pw;
			if ( cchSpace ) {
				MWMemWSet( pw, wNoType, cchSpace );
				pw += cchSpace;
			}
		}
		if ( imwd == MW_BYTE ) {
			// Add real pad.  There's already 1 space, add another.
			ASSERT( cchPad == 2 );
			*pw++ = wNoType;

			// BYTE display, add the ASCII values to the end
			// of the line
			for( cCell = m_mwi.cFmtVals; cCell; --cCell ) {
				*pw++ = wRaw++;
			}
		}
		// Remove trailing spaces
		while( *--pw == wNoType );
		*++pw = wNull;

		// Allocate space for the final product and save away.
		if ( !( m_mwi.rgwLineInfo =
			(WORD FAR *)_fmalloc( sizeof( WORD ) * ( pw - rgwBuf + 2 ) ) ) ) {

			OOMError();
			return FALSE;
		}
		else if ( !( m_mwi.rgf =
			(BOOL FAR *)_fmalloc( sizeof( BOOL ) *
				( ( wRaw & ~wFmtMask ) - wFmtLoMin +
				  ( wFmt & ~wFmtMask ) - wFmtLoMin ) ) ) ) {

			OOMError();
			return FALSE;
		}
		else {
			_fmemcpy(
                m_mwi.rgwLineInfo,
                rgwBuf,
				sizeof( WORD ) * ( pw - rgwBuf + 2 )
            );

            m_mwi.cwLineInfo = (WORD)( pw - rgwBuf - 1 );
		}
	}
	return fForceChange;
}
