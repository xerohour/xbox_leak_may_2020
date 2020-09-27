//////////////////////////////////////////////////////////////////////////
//
//	MW.C - Memory Window Handler
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

//// CchFormatData
//
// Purpose: Format real data into display format.  Broken out from
// MWFormatLine so the compiler can optimize.
//
// Input:
//	 	imwd		Format to use.
//      fShowRaw    If RAW data is being displayed (affects MBCS ASCII)
//	 	rgb			Data to format.
//
// Output:
//		sz			Where to put the formatted data
//		pchAscii	If BYTE display, here's where to put the ascii data
//      pfmbcs      pointer to BOOL indicating if ASCII char processed
//                  is MBCS char.
//
// 	Returns number of characters added to sz.
//
// Exceptions:
//
// Notes:
//      !!! If you dereference rgb as anything other than a 1-byte quantity
//      !!! be sure to use UNALIGNED so as not to fault on MIPS
//
///////////////////////////////////////////////////////////////////////////
int CMemoryView :: CchFormatData(USHORT imwd, BOOL fShowRaw, BYTE *rgb, char *sz, char *pchAscii, BOOL *pfmbcs)
{
	int	cch = 0;

	switch( imwd ) {
		case MW_ASCII:
			cch = 1;

			// If not processing second mbcs byte, handle
			// normally
			if ( !*pfmbcs )
            {
				// If any of the conditions are FALSE, then
				// we want to evaluate the char as an MBCS
				// character.  Else it must be a single byte
				// char
				if (fShowRaw || !FDbcsEnabled() || !_ismbblead(*(UCHAR *)rgb) )
                {
					if ( isprint( *rgb ) || _ismbbkana( *rgb ) ) {
						*sz = *(char *)rgb;
					}
					else {
						*sz = '.';
					}
				}
				// MB char (possibly.  Validate the second byte
				else {
					// Second byte is valid trail byte, set
					// flag and copy 2 bytes
                    if ( _ismbbtrail( *(UCHAR *)( rgb + 1 ) ) ) {
                        *pfmbcs = TRUE;
                        *sz = *rgb;
                        *( sz + 1 ) = *( rgb + 1 );
                    }

					// Invalid trail byte.  Treat as non-printable
					// single byte char
                    else {
                        *sz = '.';

                    }
				}
			}
			else {
				*pfmbcs = FALSE;
			}
			break;

#ifdef _WIN32
        case MW_WCHAR: {
			wchar_t	wc;
			char	rgchMB[ 5 ];

			ASSERT( sizeof( rgchMB ) >= MB_CUR_MAX );
				
			FlipBytes(rgb, 2);
			wc = *(UNALIGNED wchar_t *)rgb;

			*sz = '\0';
			if ( iswprint( wc ) && wctomb( rgchMB, wc ) != -1 ) {
				sprintf( sz, "%-2lc", wc );
				ASSERT( _tcslen( sz ) == 2 );
				cch = 2;
			}
			else {
				_tcscpy( sz, ". " );
				cch = 2;
			}
			break;
		}
#endif	// _WIN32

		case MW_BYTE:
			*sz++ = mpbHex[ *rgb >> 4 & 0x0f ];
			*sz = mpbHex[ *rgb & 0x0f ];
			cch = 2;

            CchFormatData(
                MW_ASCII,
                FALSE,
                rgb,
                pchAscii,
                (char *)NULL,
                pfmbcs
            );
			break;

		case MW_INT:
			FlipBytes(rgb, 2);
			cch = sprintf( sz, "%6d", *(UNALIGNED short *)rgb );
			break;

		case MW_INT_HEX:
			FlipBytes(rgb, 2);
			cch = sprintf( sz, "%04X", *(UNALIGNED USHORT *)rgb );
			break;

		case MW_INT_UNSIGNED:
			FlipBytes(rgb, 2);
			cch = sprintf( sz, "%5u", *(UNALIGNED USHORT *)rgb );
			break;

		case MW_LONG:
			FlipBytes(rgb, 4);
			cch = sprintf( sz, "%11ld", *(UNALIGNED long *)rgb );
			break;

		case MW_LONG_HEX:
			FlipBytes(rgb, 4);
			cch = sprintf( sz, "%08lX", *(UNALIGNED long *)rgb );
			break;

		case MW_LONG_UNSIGNED:
			FlipBytes(rgb, 4);
			cch = sprintf( sz, "%10lu", *(UNALIGNED ULONG *)rgb );
			break;

		case MW_REAL:
			FlipBytes(rgb, 4);
			cch = sprintf( sz, "%+.5E", *(UNALIGNED float *)rgb );
			break;

		case MW_REAL_LONG:
			FlipBytes(rgb, 8);
			cch = sprintf( sz, "%+.14lE", *(UNALIGNED double *)rgb );
			break;

#ifndef _WIN32
		case MW_REAL_TEN:
			sz += ( cch = sprintf( sz, "%+.19LE", *(long double *)rgb ) );
			if ( *( sz - 5 ) == 'E' ) {
				memmove( ( sz - 2 ), ( sz - 3 ), 3 );
				*( sz - 3 ) = '0';
				++sz;
				++cch;
			}
			break;
#else	// !_WIN32
#if _INTEGRAL_MAX_BITS >= 64
		case MW_INT64:
			FlipBytes(rgb, 8);
			cch = sprintf( sz, "%20I64d", *(UNALIGNED __int64 *)rgb );
			break;

		case MW_INT64_HEX:
			FlipBytes(rgb, 8);
			cch = sprintf( sz, "%016I64X", *(UNALIGNED __int64 *)rgb );
			break;

		case MW_INT64_UNSIGNED:
			FlipBytes(rgb, 8);
			cch = sprintf( sz, "%20I64u", *(UNALIGNED __int64 *)rgb );
			break;
#endif	// _INTEGRAL_MAX_BITS >= 64
#endif	// !_WIN32
    }
	return cch;
}

//// FormatLine
//
// Purpose: Format a given line for the specified window
//
// Input:
//		iLn		Relative line number to format
//		sz		Buffer for formatted line (assumes sizeof( sz ) == axMax )
//
// Output:
//		sz			is filled with the line data for the requested line.
//		m_mwi.rgf	contains the information indictating debuggee
//					data change since the last debuggee execution.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: FormatLine(
DWORD	iln,
char *	sz ) {
	ADDR		addrT;
	BYTE *		pbT;
	const MWD * pmwd;
	USHORT		cItemsLeft;
	USHORT		cb;
	USHORT		imwd;
	USHORT		cbNeeded;
	USHORT		cbRequest;
	char		szAscii[ cchBufMax ];
	USHORT		ichCur;
	USHORT		cchPadT;
	BOOL FAR *	lpf;
	BYTE		rgb[ cbDataMax + 1 ];
	short		dSeg;
	BOOL 		fmbcs = FALSE;

	pmwd = m_mwi.pmwd;
	imwd = pmwd - rgmwd;
	cbNeeded = pmwd->cbType;

	lpf = m_mwi.rgf;

	ichCur = 1;
	// We require an additional space between the byte data and the
	// ASCII data, so ...
	*szAscii = ' ';
	cchPadT = ( imwd == MW_BYTE ? 1 : cchPad );

	cItemsLeft = m_mwi.cFmtVals;

	// Get address for line
	MWAddrFromIln( &m_mwi, iln, &addrT );

	// Format address
	// Don't forget 16:32, 0:32???  have expression evaluator for
	// OSDEB format addresses
	dSeg = (short)GetAddrSeg( addrT ) - (short)GetAddrSeg( m_mwi.addr );

	dSeg += (short)GetAddrSeg( m_mwi.addr );

	SetAddrSeg( &addrT, (SEGMENT)dSeg );

	if ( ADDR_IS_FLAT( addrT ) ) {
		ASSERT( ADDR_IS_OFF32( addrT ) );
		sz += sprintf( sz, "%.8lX  ", GetAddrOff( addrT ) );
	}
	else {
		sz += sprintf( sz, "%.4X:%.*X  ", GetAddrSeg( addrT ),
			ADDR_IS_OFF32( addrT ) ? 8 : 4,
			GetAddrOff( addrT ) );
	}

	cbRequest = cbNeeded;

	// For DBCS and ASCII or BYTE mode, we will need to read an
	// extra byte so we can display DBCS caracters.
	if ( FDbcsEnabled() && ( imwd == MW_BYTE || imwd == MW_ASCII ) ) {
		++cbRequest;
	}

	while( cItemsLeft-- ) {
		// Get the data from the debuggee

		// If we are on the last item, do not read any extra bytes
		// for DBCS.  We want there to be an invalid trail byte so
		// it gets displayed as a '.'.  So just reset the number
		// of bytes to read to the actual number for this display type
		if ( cItemsLeft == 0 ) {
			cbRequest = cbNeeded;
		}

		// Set the byte after sizeof data to zero.  If MBCS and the last
		// byte is a lead byte, zero is an invalid trailing byte
		rgb[ cbNeeded ] = 0;

		if (cbNeeded > CbGetDebuggeeBytes( &addrT, cbRequest, rgb )) {

			// We were not able to get enough bytes to display
			// the entire value, so print out "?"s for bogus
			// values.
			if ( m_mwi.fShowRaw && imwd != MW_BYTE ) {
				cb = cbNeeded;
				while( cb-- ) {
					*sz++ = ' ';
					*sz++ = '?';
					*sz++ = '?';
					*lpf++ = TRUE;
				}
				ASSERT( cchPad == 2 );
				*sz++ = ' ';
				*sz++ = ' ';
			}
			memset( sz, '?', pmwd->cchFormatted );
			sz += pmwd->cchFormatted;
			*lpf++ = TRUE;
			szAscii[ ichCur++ ] = '?';
		}
		else {
			BOOL	fmbcsPrev;

			if ( m_mwi.fShowRaw && imwd != MW_BYTE ) {
            	ADDR    addrRaw = addrT;
				BOOL	fNoChange = TRUE;

				pbT = rgb;
				cb = cbNeeded;
				while ( cb-- ) {
					fNoChange &=
						( *lpf++ = FCmpUserMemory(
							&addrRaw,
							sizeof(BYTE),
							pbT )
						);

					*sz++ = ' ';
					*sz++ = mpbHex[ *pbT >> 4 & 0x0f ];
					*sz++ = mpbHex[ *pbT & 0x0f ];
					++pbT;
					// Update address packet.
					// If we're on the last byte of a
					// segment, update segment in
					// addr packet.
					if ( GetAddrOff( addrRaw ) ==
						(UOFFSET)(-1) ) {
						SetAddrSeg( &addrRaw,
							GetAddrSeg( addrRaw ) + 1 );
					}
					SetAddrOff( &addrRaw,
						GetAddrOff( addrRaw ) + 1 );
				}
				ASSERT( cchPad == 2 );
				*sz++ = ' ';
				*sz++ = ' ';

				*lpf++ = fNoChange;
			}
			else {
				*lpf++ = FCmpUserMemory( &addrT, cbNeeded,
					rgb );
			}

			// Format the real data

			fmbcsPrev = fmbcs;

			sz += CchFormatData(
                imwd,
                m_mwi.fShowRaw,
                rgb,
                sz,
                &szAscii[ ichCur++ ],
                &fmbcs
            );

			// If the following conditions are met, we have just formatted a
			// dbcs character.  Go back and make sure that the differences
			// for the two bytes are the same
			if ( !fmbcs && fmbcsPrev && imwd == MW_ASCII && !m_mwi.fShowRaw ) {
				*( lpf - 2 ) = *( lpf - 1 ) = ( *( lpf - 2 ) && *( lpf - 1 ) );
			}
		}

		// Put padding between the data.
		if ( !IsCharFormat( imwd ) || m_mwi.fShowRaw ) {
			memset( sz, ' ', cchPadT );
			sz += cchPadT;
		}

		// Update address packet
		if ( GetAddrOff( addrT ) >= (UOFFSET)-(long)cbNeeded ) {
			SetAddrSeg( &addrT, GetAddrSeg( addrT ) + 1 );
		}
		SetAddrOff( &addrT, GetAddrOff( addrT ) + cbNeeded );
	}

	*sz = chNull;

	// If we're in BYTE mode, append the ASCII data to the end of the
	// line.
	if ( imwd == MW_BYTE ) {
		szAscii[ ichCur ] = chNull;
		_ftcscat( sz, szAscii );

		if ( FDbcsEnabled() ) {
			BOOL FAR *	lpfCur = m_mwi.rgf;
			BOOL FAR *	lpfMax = lpf;
			char *		pch = szAscii + 1;

	        // Walk through the MBCS string and keep track of duplicates
	        // Create the ASCII diffs by walking the previous diffs and
	        // checking character length
	        while( lpfCur < lpfMax ) {
				size_t	cch = _tclen( pch );

				pch += cch;
	            if ( cch == 1 ) {
	                *lpf++ = *lpfCur++;
	            }
	            else {
	                BOOL    fT;

	                ASSERT( cch == 2 );

	                //
	                // The optimizer screws up if the following is done,
	                // which is why there's a temporary fT
	                // *lpf++ = *lpf++ = *lpfCur && *( lpfCur + 1 )
	                //
	                fT = *lpfCur && *( lpfCur + 1 );
	                *lpf++ = fT;
	                *lpf++ = fT;
	                lpfCur += 2;
	            }
	        }
		}
		else {
			// Duplicate the byte value changes.  Simple 1-1 correlation.
			_fmemcpy(
				lpf,
				m_mwi.rgf,
				(size_t)( sizeof( BOOL ) * ( lpf - m_mwi.rgf ) )
			);
		}
	}
	// Create the line attribute information in the m_rgla buffer.
	BuildRgla();
}

//// MWSetAddrFromExpr
//
// Purpose: Reevaluate the expression from the specified window.
//
// Input:
//	pmwi		Pointer to MWI data containing addr packet for evaluation.
// iAutoType	Auto type setting.
//					-1 = Auto type enabled, use HEX, update mwi
//					 0 = Auto type disabled, don't update mwi
//					 1 = Auto type enabled use, [un]signed, update mwi
//
// Output:
//	Returns FALSE for failure or TRUE for success.  If successful,
//	the addr packet in pmwi is updated.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
USHORT PASCAL MWSetAddrFromExpr( PMWI pmwi, int iAutoType ) {
	USHORT		usRet = FALSE;
	ADDR		addrT = {0};
	char *		psz;
	char *		pszExpr;
	int			radixInputSave = radixInput;
	BOOL		fSourceExpr = FALSE;

    ASSERT( pmwi );

	errno = CV_NOERROR;

    if ( !pmwi->szExpr ) {
        return ( usRet );
    }

	// Force the expression to hex if the address started with a zero
	pszExpr = psz = pmwi->szExpr;
	if ( *psz == '0' ) {
		while( *psz && *psz == '0' ) {
			++psz;
		}

		// The zero is required for specifying hex or dex (ie 0n10 or 0x1fa)
		if ( psz > pszExpr &&
			( !*psz || *psz == 'n' || *psz == 'N' || *psz == 'x' || *psz == 'X' )
		) {
			--psz;
		}
	}
	else {
		TCHAR FAR *	lptch = (TCHAR FAR *)psz;

		if ( _ftcschr( lptch, _T('{') ) ) {
			// Character past context
			lptch = _ftcschr( lptch, _T('}') ) + 1;

			if((int)lptch == 1)
				// _ftcschr didn't find a '}' and returned 0.
				return FALSE;
		}

		// Skip white space
		while( _istspace( (_TUCHAR)*lptch ) ) {
			lptch = _ftcsinc( lptch );
		}

		// Source line must have a period or an 'at sign' as the
		// 1st character outside of a context operator
		if ( *lptch == '.' || *lptch == '@' ) {
			fSourceExpr = TRUE;
		}
	}
	

	// Default to hex
	radixInput = 16;

    // Line number or simple address expression
    if ( fSourceExpr && line_number_address( &addrT, EEDATA | EECODE, psz ) ) {
		usRet = TRUE;
    }
    // Complex expression
    else if ( errno != NOCODE ) {
	    HTM			htm;
    	PCXF		pcxf = PcxfLocal();
    	PCXT		pcxt = SHpCXTFrompCXF( pcxf );

    	if ( ( CVParseBind (
    		    &htm, pcxt, radixInput, fCaseSensitive, TRUE, psz ) == EENOERROR
    		) &&
		     ( EEvaluateTM ( &htm, SHpFrameFrompCXF( pcxf ),
                            EEHORIZONTAL ) == EENOERROR ) ) {

#ifdef AUTO_DEREF
			EEPDTYP	eepdtyp = EEIsExpandable( &htm );

			// If the expression is a pointer, then display
			// what it points to, not the address of the address
			if ( eepdtyp == EETYPEPTR || eepdtyp == EEPOINTER ) {
				EEHSTR	eehstr;

				// The EE has the address of the pointer internally, but
				// there is no API to access it.  So, just get the string
				// value from the EE and reparse it as a constant address
				// to build up the addr packet
				if (!EEGetValueFromTM( &htm, radixInput, (PEEFORMAT)"p", &eehstr )) {
					LPSTR		lpstr = (LPSTR)BMLock( eehstr );
					TCHAR FAR *	lptch = (TCHAR FAR *)lpstr;

					// strip off ANYTHING past and address field
					while( *lptch && !_istspace( *lptch ) ) {
						lptch = _ftcsinc( lptch );
					}
					*lptch = '\0';
					if ( line_number_address( &addrT, EEDATA | EECODE, lpstr ) ) {
						usRet = TRUE;
					}

					BMUnlock( eehstr );
					EEFreeStr( eehstr );
				}
	 		}
#endif	// AUTO_DEREF

			// Get the address of the TM if it wasn't an lvalue
			if ( !usRet ) {
	            USHORT	usSegType = EEANYSEG;

	            if ( !BPADDRFromTM ( &htm, &usSegType, &addrT ) ) {
				    usRet = TRUE;
    			}
			}
	    	EEFreeTM( &htm );
    	}
    }

	// Have a parsed address in addrT, adjust so the number of lines
	// is correct
    if ( usRet ) {
		// We only deal with fixed up addresses.  So fix it up and
		// save in the mwi
		SYFixupAddr( &addrT );
		pmwi->addr = addrT;

        if ( MWIlnFromAddr( pmwi, &addrT ) >= pmwi->cLines ) {
            ++pmwi->cLines;
        }
        else {
            MWAddrFromIln( pmwi, pmwi->cLines - 1, &addrT );

            if ( ADDR_IS_FLAT( addrT ) && !GetAddrOff( addrT ) ||
                !LOWORD( GetAddrOff( addrT ) ) ) {

                --pmwi->cLines;
            }
        }
    }

	// Restore the radix
	radixInput = radixInputSave;

	return usRet;
}

//// MWChangeTitle
//
// Purpose: Change the title of the specified window.  The status window is
// NOT updated.
//
// Input:
//
// Output: The information in the cvWin struct is updated in ChangeTitle()
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: ChangeTitle() {
	CMemoryParent *	pMemoryParent = (CMemoryParent *)GetParent();

	ASSERT(pMemoryParent->IsKindOf(RUNTIME_CLASS(CMemoryParent)));
	pMemoryParent->m_toolbar.m_edit.SetWindowText( (LPCSTR)m_mwi.szExpr );
	pMemoryParent->m_toolbar.m_edit.SetSel( 0, -1, TRUE );
}

//// NextField
//
// Purpose: Move the cursor in the desired direction to the next field of
// any type (address, raw, or formatted).
//
// Input:
//		sDirection	One of MW_FWD or MW_REV.
//		fUpdate		Update the screen (TRUE), else update the editmgr info
//
// Output:  Screen is updated to have cursor on new position and visible.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: NextField(
short	sDirection,
BOOL	fUpdate ) {
	const MWD * pmwd;
	WORD		ich;
	DWORD		iln;
	DWORD		ilnMac;
    WORD FAR *  lpw;
    WORD FAR *  lpwMin;
    WORD FAR *  lpwMac;
    WORD        wT;

	pmwd = m_mwi.pmwd;

    lpw = lpwMin = m_mwi.rgwLineInfo + 1;
    lpwMac = lpw + m_mwi.cwLineInfo - 1;

	ich = ObGetCurPos();
	iln = OlnGetCurPos();

    if ( FDbcsEnabled() && sDirection == MW_FWD ) {
    	char		sz[ axMax ];

	    FormatLine( iln, sz );
	    if( _ismbblead( *( sz + ich ) ) ) {
        	ich++;
	    }
    }

    lpw += ich;

	if ( lpw <= lpwMac ) {
		wT = *lpw;

		// Locate different field in specified direction
        while( *lpw && *lpw == wT ) {
			lpw += sDirection;
		}

		// Skip over blanks (if any)
        while( *lpw && *lpw == wNoType ) {
			lpw += sDirection;
		}
	}
	else {
	    lpw = lpwMac;
		if ( sDirection == MW_FWD ) {
			++lpw;
		}
	}

	// If we're on top of a valid field, look for the first occurance of
	// the character and that's where the cursor belongs.
    if ( lpwMin <= lpw && lpw <= lpwMac ) {
        wT = *lpw;
        while( *( lpw - 1 ) == wT ) {
            --lpw;
        }
        ich = lpw - lpwMin;
	}
	// If we're at the beginning or the buffer and the cursor was beyond
	// the address portion and we were going backwards, put the cursor
	// on the beginning of the address field.
	else if ( sDirection == MW_REV && lpw == lpwMin - 1 && ich >= m_mwi.cchAddr ) {
		ich = 0;
	}
	// Otherwise, we're either past the end of the line or at the
	// beginning...
	else {
		iln += (DWORD)sDirection;
		ilnMac = m_mwi.cLines - 1;

		// Make sure that the new line number is valid
		if ( iln >= ilnMac ) {
			iln = ilnMac;
		}
		// If FWD put cursor on address field, otherwise,
		// put cursor on first occurence of last field.
		if ( sDirection == MW_FWD ) {
			ich = 0;
		}
		else {
			// Find the end of the string and get the
			// preceeding char.
            lpw = lpwMac;
            wT = *lpw;
            while( *( lpw - 1 ) == wT ) {
                --lpw;
            }
            ich = lpw - lpwMin;
		}
	}

    if ( FDbcsEnabled() && sDirection == MW_REV ) {
		char	sz[ axMax ];
        WORD    ichT = 0;

        FormatLine( iln, sz );
        while( ichT < ich ) {
            ichT += _tclen( &sz[ ichT ] );
        }
        if ( ich != ichT ) {
            --ich;
        }
    }

	if ( fUpdate ) {
		SetCursorAt( ich, iln );
	}
	else {
		SetCurObPos( ich );
		SetCurOlnPos( iln );
	}
}

//// NextLikeField
//
// Purpose: Locate next field with same class (Either RAW or FMT)
//
// Input:
//
// Output: Cursor is moved to new field of same type in foward direction.
//
// Exceptions:
//
// Notes: This function EXPECTS that the cursor is already on a
// RAW or FMT field
//
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: NextLikeField( char * sz ) {
	const MWD * pmwd;
	WORD		ich;
	DWORD		iln;
    WORD        wT;
    WORD FAR *  lpw;
	DWORD		ilnMac;

	pmwd = m_mwi.pmwd;
	lpw = m_mwi.rgwLineInfo + 1;

	ich = ObGetCurPos();
	iln = OlnGetCurPos();

	if( FDbcsEnabled() && _ismbblead( *( sz + ich ) ) ) {
		// If the last character is a DBCS character, we
		// may move off the map, so check to see if the
		// new map entry is the EOM before incrementing
		// ich
		if ( *( lpw + ich + 1 ) ) {
	    	ich++;
		}
	}
	
	lpw += ich;

    // wT Should be the next field found unless we're at the EOL
    wT = *lpw + 1;

    // Skip along until we've found the new field or hit EOL
    while( *++lpw && *lpw != wT );

    // if !*lpw, then we need to find the first item of same type
    // on the next line.  Preserve the mask bit and the beginning
    // index is 1
    if ( !*lpw ) {
        wT = wT & wFmtMask | 1;
        lpw = m_mwi.rgwLineInfo;
        while( *++lpw != wT );

		// We have to get the last valid line number.
		ilnMac = m_mwi.cLines - 1;

        // Validate the new line number
        if ( iln == ilnMac ) {
            iln = 0L;
        }
        else {
            ++iln;
        }
	}
	ich = lpw - m_mwi.rgwLineInfo - 1;
	SetCursorAt( ich, iln );
}

//// CchGetCellInfo
//
// Purpose: Get the cell information for the cell which the cursor is in.
//
// Input:
//      fBkSpc      TRUE if a back space is being processed
//
// Output:
//		Returns the width of the cell.
//		*puo		Offset address WITHIN THE LINE, NOT ABSOLUTE!!
//					(indeterminate if class is MW_NONE)
//		*puClass	One of MW_NONE, MW_ADDR, MW_RAW, MW_FMT, or
//					MW_ASCIIDAT indictating what the cursor is sitting on.
//		*pichStart	Index to character which is starting position for
//					cell. (indeterminate if class is MW_NONE)
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
USHORT CMemoryView :: CchGetCellInfo(
BOOL        fBkSpc,
UOFFSET *	puo,
USHORT *	puClass,
WORD *		pichStart ) {
	const MWD * pmwd;
	USHORT		ich;
	USHORT		cch;
    WORD        wT;
    WORD FAR *  lpw;

	pmwd = m_mwi.pmwd;
    lpw = m_mwi.rgwLineInfo + 1;

	ich = ObGetCurPos();
    if ( ich && fBkSpc ) {
        --ich;
    }
	*pichStart = 0;

	// Address field, real quick
	if ( ich < m_mwi.cchAddr ) {
		*puo = (UOFFSET)0;
		*puClass = MW_ADDR;
		*pichStart = 0;
		cch = m_mwi.cchAddr;
	}

	// Past the end or on a space (filler)
	else if ( ich >= m_mwi.cwLineInfo ||
        ( wT = *( lpw + ich ) ) == wNoType ) {

		*puClass = MW_NONE;
		cch = 0;
	}

	// We're sitting on type of a BYTE formatted field
	// (either MW_BYTE or on a RAW data.  Either way, tell 'em RAW)
	else if ( !( wT & wFmtMask ) ) {
		*puo = (UOFFSET)( wT - wFmtLoMin );
		*puClass = MW_RAW;
		*pichStart = MWNINCH;
		cch = cchRaw - 1;
	}

	// Last case, it's formatted or ascii
	else {
		*puo = (UOFFSET)( pmwd->cbType * ( wT - wFmtHiMin ) );

		// If we're here and its BYTE mode, then we're on top of the
		// ascii portion of the line.
		if ( pmwd == &rgmwd[ MW_BYTE ] ) {
			*puClass = MW_ASCIIDAT;
			cch = 1;
		}

		// Otherwise, it's really a formatted field
		else {
			*puClass = MW_FMT;
			cch = pmwd->cchFormatted;
		}
		*pichStart = MWNINCH;
	}

	// If RAW, FMT, or ASCII, fiqure out the length of the field.
	if ( *pichStart == MWNINCH ) {
		if ( ich < cch ) {
			ich = cch;
		}
		lpw = m_mwi.rgwLineInfo;
        while( *lpw != wT ) {
            ++lpw;
        }

		*pichStart = (WORD)( lpw - m_mwi.rgwLineInfo - 1 );
	}
	return cch;
}

//// FDoChar
//
// Purpose: Handle single character input to memory windows.
//
// Input:
//	wParam	word value of WM_CHAR message
//	lParam	dword value of WM_CHAR message
//
// Output: Depending on character we may:
//		1) Change the address expression,
//		2) Change a memory value, or
//		3) Beep() for an invalid input!
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: FDoChar(
UINT	wParam,
DWORD	lParam ) {
	BOOL	fRet;
	const MWD *pmwd;
	USHORT	imwd;
	DWORD	iln;
	WORD	ich;
	UOFFSET	uo;
	USHORT	ucls;
	USHORT	cch;
	WORD	ichStart;
	char	szBuf[ cchBufMax ];
	char	sz[ cchBufMax ];
	char	rgch[ 2 ];
	ADDR	addrT;
	char	ch;
	size_t	cchInput;
	
	rgch[ 1 ] = (UCHAR)LOBYTE( wParam );
	rgch[ 0 ] = (UCHAR)HIBYTE( wParam );
	cchInput = _tclen( rgch );

	// If the debuggee is running or there's a selection,
	// don't allow typing
	if ( GetEditSelection( 2, sz ) ) {
		MessageBeep( 0 );
		return TRUE;
	}

	fRet = FALSE;
	pmwd = m_mwi.pmwd;
	imwd = pmwd - rgmwd;

	iln = OlnGetCurPos();
	ich = ObGetCurPos();

	MWAddrFromIln( &m_mwi, iln, &addrT );

	cch = CchGetCellInfo( FALSE, &uo, &ucls, &ichStart );

	// Only allow DB chars if in ASCIIDAT or ASCII w/o RAW
	if ( cchInput > 1 &&
		ucls != MW_ASCIIDAT &&
		( ucls != MW_FMT || imwd != MW_ASCII || m_mwi.fShowRaw )
	) {
		MessageBeep( 0 );
		return fRet;
	}

	FormatLine( iln, sz );

	// If it's not printable, then we can't use it!
	if ( _istprint( wParam ) ) {
		BOOL	fUpdateExtraLine = FALSE;

		fRet = TRUE;
		ch = (char)wParam;
		switch( ucls ) {
			// Not on an editable field, BEEP at them!
			case MW_NONE:
				MessageBeep( 0 );
				break;

			// On an editable field,  replace the
			// character where the cursor is sitting with
			// the on the user just typed in.  After that,
			// adjust the cursor.
			case MW_ADDR:
				if (
					cchInput > 1 ||
					sz[ ich ] == ':' ||
					!_ftcschr( mpbHex, (_TUCHAR)ch )
				) {
					MessageBeep( 0 );
					break;
				}
				else {
					// Convert to upper-case to title string will be
					// consistent.
					ch = (char)_totupper( (_TUCHAR)ch );
				}
			case MW_ASCIIDAT:
			case MW_RAW:
			case MW_FMT: {
				char *	pch;


				// Spaces are only allowed for ASCII OR leading chars of
				// int's and longs
				if ( cchInput > 1 || ch != ' ' || ucls == MW_ASCIIDAT ||
					( ucls == MW_FMT &&
						( imwd == MW_ASCII ||
#ifdef _WIN32
							imwd == MW_WCHAR ||
#endif	// _WIN32
							imwd == MW_INT ||
							imwd == MW_LONG
						)
					)
				) {
#ifdef _WIN32
					// If we're editing in a wchar field, convert from
					// mbcs to wchar before passing on
					if ( ucls == MW_FMT && imwd == MW_WCHAR ) {
						wchar_t	wc;
						char	rgchLow[ sizeof( wchar_t ) ];
						
						*(WORD *)rgchLow = LOWORD( wParam );

						// Since this comes from WM_CHAR, should ALWAYS
						// be valid!
						VERIFY( mbtowc( &wc, rgchLow, 2 ) > 0 );
						memcpy( szBuf, &wc, cchInput = sizeof( wc ) );
						szBuf[ cchInput ] = '\0';
						ich += cchInput;
					}
					else
#endif	// _WIN32
					{
						if ( cchInput > 1 ) {
							// if the input clips the current line,
							// accept the change, just update the extra line
							if ( ich + 1 == (WORD)strlen( sz ) ) {
								fUpdateExtraLine = TRUE;
							}

							sz[ ich++ ] = (char)( wParam >> 8 );
							++cch;
						}

						sz[ ich++ ] = ch;

						_ftcsncpy( szBuf, sz + ichStart, cch );
						szBuf[ cch ] = chNull;

						// Zero fill the field in case they've typed
						// in a field which is left justified.
						if ( ucls == MW_ADDR ) {
							if ( sz[ ich ] == ':' ) {
								++ich;
							}
							memmove( szBuf + 2, szBuf, _ftcslen( szBuf ) + 1 );
							szBuf[ 0 ] = '0';
							szBuf[ 1 ] = 'x';
							if(!ADDR_IS_FLAT( addrT )) {
								pch = _ftcschr( szBuf, _T(':') ) + 1;
								memmove( pch + 2, pch, _ftcslen( pch ) + 1 );
								*pch++ = '0';
								*pch = 'x';
							}
						}
						else {
							pch = szBuf + ich - ichStart;
							while ( *pch ) {
								if ( *pch == ' ' ) {
									*pch = '0';
								}
								++pch;
							}
						}
					}

	                // Make sure that we can show a diff on the visible data
	                ChangeDebuggeeData();

					// Make the change.  If we fail, beep at em.
					if ( FChangeMemory( FALSE, cchInput, szBuf ) ) {
						// See if we need to move to the
						// next field, or...
						DrawLine( iln = OlnGetCurPos() );

						// If fUpdateExtraLine,, then a DBCS char was input
						// at the end of the line and we need to advance
						// an extra time onto the next line
						if ( fUpdateExtraLine ) {
							NextLikeField( sz );
							DrawLine( ++iln );
							FormatLine( iln, sz );

							// Move to the next field (off of the trail
							// byte) from the previous edit
							NextLikeField( sz );
						}
						else {
							if ( ich >= ichStart + cch ) {
								if ( ucls == MW_ADDR ) {
									NextField( MW_FWD, TRUE );
								}
								else {
									NextLikeField( sz );
								}
							}
							// Just go to the new cursor position
							else {
								iln = OlnGetCurPos();
								SetCursorAt( ich, iln );
							}
						}
					}
				}
				else {
					MessageBeep( 0 );
				}
				break;
			}
		}
	}
	return fRet;
}

//// FChangeMemory
//
// Purpose: Change the address expresssion from editing or change some
// debuggee data.
//
// Input:
//  fBkSpc  	Is a backspace char being processed
//  cchInput	Number of characters changed.  Only non-1 for DBCS
//				on ASCII input
//	szIn		String to parse for input as update information
//
// Output: Return TRUE if OK, else FALSE.  Window will be updated to reflect
// change.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: FChangeMemory(
BOOL        fBkSpc,
int			cchInput,
char *	    szIn ) {
	const MWD *pmwd;
	USHORT	imwd;
	USHORT	cch;
	UOFFSET	uo;
	USHORT	ucls;
	WORD	iStart;
	BOOL	fRet;
	DWORD	iln;
	ADDR	addrT;

	pmwd = m_mwi.pmwd;
	imwd = pmwd - rgmwd;
	iln = OlnGetCurPos();
	fRet = FALSE;
	cch = CchGetCellInfo( fBkSpc, &uo, &ucls, &iStart );

	switch( ucls ) {
		case MW_ADDR: {
			// Update the address expression.  Editing this
			// field FORCES non-LIVE expression evaluation.
			if ( SetAttributes( MWNINCH, szIn, MWNINCH,
				MWNINCH, MWNINCH ) ) {

				// Do not reset live expressions
				// m_mwi.fLiveExpr = FALSE;

				MWSaveSettings( szIn );

                // Update the differences cache if necessary
                ChangeDebuggeeData();
				fRet = TRUE;
			}
			break;
		}
		case MW_ASCIIDAT:
		case MW_RAW:
		case MW_FMT: {
			BYTE	rgb[ cbDataMax ];
			UINT	cbIn;
			char	szFmt[ 10 ];	// scanf format

			// IF RAW or ASCII, get correct input format
			if ( ucls == MW_RAW ) {
				imwd = MW_BYTE;
			}
			else if ( ucls == MW_ASCIIDAT ) {
				imwd = MW_ASCII;
			}
			pmwd = &rgmwd[ imwd ];
			_ftcscpy( (char far *)szFmt, (char far *)pmwd->bszFmtIn );
			_ftcscat( szFmt, "%n" );

			if ( cchInput > 1 && imwd == MW_ASCII
#ifdef _WIN32
				|| imwd == MW_WCHAR
#endif	// _WIN32
			) {
				ASSERT( cchInput == 2 );
				memcpy( rgb, szIn, cchInput );
				cbIn = cchInput;
			}
			else {
				char *	pch;
				
				pch = szIn + _ftcslen( szIn );
				*pch = ' ';
				sscanf( szIn, szFmt, rgb, &cbIn );
				*pch = chNull;
			}

            if ( !*szIn ) {
                rgb[0] = *szIn;
            }
			if (
#ifdef _WIN32
				imwd != MW_WCHAR &&
#endif	// _WIN32
				cbIn != _ftcslen( szIn ) &&
                !( IsCharFormat( imwd ) && !*szIn ) ) {
				MessageBeep( 0 );
			}
			else {
				MWAddrFromIln( &m_mwi, iln, &addrT );
				if ( uo &&  GetAddrOff( addrT ) >= 	(UOFFSET)0 - uo ) {
					SetAddrSeg( &addrT, GetAddrSeg( addrT ) + 1 );
				}
				SetAddrOff( &addrT, GetAddrOff( addrT ) + uo );

				if ( !m_mwi.hlliUndo ) {
					m_mwi.hlliUndo =
						LLHlliInit( sizeof( MUB ), llfNull, NULL, NULL	);
				}

				if ( !m_mwi.hlliRedo ) {
					m_mwi.hlliRedo =
						LLHlliInit( sizeof( MUB ), llfNull, NULL, NULL	);
				}

				// Set the number of bytes being input to the number
				// of bytes for the type, unless the input was a
				// dbcs char, in which case 2 bytes will be updated.
				// In any event, the cchInput must be 1 or we have
				// already set cbIn to be 2!
				ASSERT( cchInput == 1 || cbIn == 2 );

				if ( cchInput == 1 ) {
					cbIn = pmwd->cbType;
				}
				
				FlipBytes(rgb, pmwd->cbType);
				if ( FPutDebuggeeBytes( ucls, &addrT, cbIn, rgb,
					m_mwi.hlliUndo, m_mwi.hlliRedo ) ) {

		    		// We need to update other windows
			    	UpdateWorld( &addrT, cbIn );

				    fRet = TRUE;
                }

                // Write failed, probably read_only
                else {
                    MessageBeep( 0 );
                }
			}
			break;
		}
	}
	return fRet;
}

//// MWAddrFromIln
//
// Purpose: Given window information and a line number, compte the
// starting address for the line.
//
// Input:
//		pmwi	Window information
//		iln		Line number to obtain address for.
//
// Output:
//		paddr	addr packet updated.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void PASCAL
MWAddrFromIln(
PMWI	pmwi,
DWORD	iln,
PADDR	paddr ) {
	USHORT	cbLine;
	DWORD	loffset;

	ASSERT( pmwi );

	if ( !pmwi->pmwd ) {
	    pmwi->pmwd = &rgmwd[ 0 ];
	}

	// # of bytes per displayed line
	cbLine = pmwi->cFmtVals * pmwi->pmwd->cbType;

	// If none, then no address!
	if ( !cbLine ) {
		_fmemset( paddr, 0, sizeof( ADDR ) );
	}
	else {
		// Initialize the addr packet
		*paddr = pmwi->addr;

		// compute the number of bytes left over once
		// the address expression is paged back to beginning
		// of window
		loffset = (DWORD)cbLine -
			(DWORD)( GetAddrOff( pmwi->addr ) % cbLine );

		// if the offset == cbLine, then there really ain't an offset
		if ( loffset == (DWORD)cbLine ) {
			loffset = 0L;
		}

		// On line zero, we may be showing some bytes in the
		// previous segment, so adjust segment #
		if ( iln == 0L && loffset ) {
			SetAddrSeg( paddr, GetAddrSeg( pmwi->addr ) - 1 );
		}
		else {
			SetAddrSeg( paddr, GetAddrSeg( pmwi->addr ) );
		}

		// The offset address is a simple calculation
		SetAddrOff( paddr,
			(UOFFSET)( iln * (DWORD)cbLine - loffset ) );

		emiAddr ( *paddr ) = 0;
		SYSetEmi ( hpidCurr, htidCurr, paddr );
	}
    ADDR_IS_FLAT(*paddr) = ADDR_IS_FLAT(pmwi->addr);
    ADDR_IS_OFF32(*paddr) = ADDR_IS_OFF32(pmwi->addr);
}

//// MWIlnFromAddr
//
// Purpose: Determine which line the addr byte appears on.
//
// Input:
//		pmwi	window information.
//		paddr	Address to find line number for.
//
// Output: Returns the line number for the specified address or,
// zero if the address is not in the window!
//
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
DWORD PASCAL
MWIlnFromAddr(
PMWI	pmwi,
PADDR	paddr ) {
	USHORT	cbLine;
	UOFFSET	uo;
	DWORD	iln;

	ASSERT( pmwi );

	if ( !pmwi->pmwd ) {
	    pmwi->pmwd = &rgmwd[ 0 ];
	}

    // If we didn't get an adress, get the expression address
	if ( !paddr ) {
    	paddr = &pmwi->addr;
	}

    cbLine = pmwi->cFmtVals * pmwi->pmwd->cbType;

    // If the # of bytes / line is zero, we have to be at line 0.
	if ( !cbLine ) {
    	iln = 0L;
	}
    else {
		// Byte offset/line
	    uo = (UOFFSET)(cbLine) -
		    (UOFFSET)( GetAddrOff( pmwi->addr ) % cbLine );

    	if ( uo == (UOFFSET)cbLine ) {
			uo = (UOFFSET)0;
	    }

    	// If segments aren't equal, force to beginning
		if ( GetAddrSeg( pmwi->addr ) !=
            GetAddrSeg( *paddr ) ) {

	    	iln = 0L;
    	}
		else {
			UOFFSET	uoMod;

			iln  = GetAddrOff( *paddr ) / cbLine;
			uoMod = GetAddrOff( *paddr ) - iln * cbLine;
			if ( (USHORT)( uoMod + uo ) >= cbLine ) {
				++iln;
			}
    	}
	}
	return iln;
}

//// UpdateExprFromMwiAddr
//
// Purpose: Update the expression from the address specified in the mwi.
//
// Input: None.
//
// Output: m_mwi.szExpr may be updated.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: UpdateExprFromMwiAddr() {

	// Save new expression if not-live.  And prepare new title
	// when we come back from debuggee execution.
	if ( m_mwi.fNoUpdate && m_mwi.fLiveExpr == MW_DEAD &&
		!ADDR_IS_LI( m_mwi.addr ) ) {

		char	sz[ cchAddrMax + 4 ];
		MWI		mwi;
		MWI		mwiT;

		mwiT = mwi = m_mwi;

		// Need to get string expression from expression evaluator
		if ( ADDR_IS_FLAT( m_mwi.addr ) ) {
			ASSERT( ADDR_IS_OFF32( m_mwi.addr ) );
			sprintf( sz, "0x%.8lX", GetAddrOff( m_mwi.addr ) );
		}
		else {
			sprintf( sz, "0x%.4X:0x%.*X", GetAddrSeg( m_mwi.addr ),
				ADDR_IS_OFF32 ( m_mwi.addr ) ? 8 : 4,
				GetAddrOff( m_mwi.addr ) );
		}
		mwi.szExpr = sz;

		if ( !MWSetAddrFromExpr( &mwiT, MW_NO_AUTO_TYPE ) ) {
			mwiT.addr = m_mwi.addr;
		}

		if ( MWSetAddrFromExpr( &mwi, MW_NO_AUTO_TYPE ) &&
			!FAddrsEq( mwi.addr, mwiT.addr )
		) {

			if ( m_mwi.szExpr ) {
				free( m_mwi.szExpr );
			}
			if ( !( m_mwi.szExpr = _ftcsdup( sz ) ) ) {
				OOMError();
				return;
			}
			ChangeTitle();
		}
	}
}

//// LpwFindNotEq
//
// Purpose: Locate the first occurance of a WORD not equal to *lpw
//
// Input:
//	lpw Buffer to look in.
//
// Output: Returns a pointer to the location where non-match
// was found.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
WORD FAR * CMemoryView :: LpwFindNotEq( WORD FAR *	lpw ) {
	WORD    wT = *lpw;

	while ( *++lpw == wT );

	return lpw;
}

//// BuildRgla
//
// Purpose: From the rgwLineInfo and rgf information update the
// line attribute information for the last line formatted.  This
// API should ONLY be called from MWFormatLine()!
//
// Input:
//
// Output: Updates m_rgla
//
// Exceptions:
//
// Notes: THIS NEEDS TO BE REAL FAST!!!!
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: BuildRgla() {
	WORD FAR *			lpw;
	USHORT				cla;
	WORD FAR *			rgwLineInfo;
	WORD FAR *			lpwLast;
	register PDLA		pdlaPrev;
	register PDLA		pdla;
	register BOOL FAR *	lpf;
	FMT_ELEMENT *rgfmtel = pfmtelFromDocType((DOCTYPE)MEMORY_WIN);
	FMT_ELEMENT *fmtelMemory = &(rgfmtel[FMTEL_TEXT]);
	FMT_ELEMENT *fmtelHilight = &(rgfmtel[FMTEL_MMHI]);

	// Get initial values
	rgwLineInfo = m_mwi.rgwLineInfo + 1;
	lpf = m_mwi.rgf;
	pdla = m_rgdla;

	// Set the address field value (it's never hilited!)
	lpw = LpwFindNotEq( rgwLineInfo );
	pdla->cb = lpw - rgwLineInfo;
	pdla->pElement = fmtelMemory;

	// Set the maximum number of values remaining.  We've already
	// filled 1 and if we go over, we'll use another. Array is
	// zero indexed, so subtract another 1. == -3.
	cla = MW_LAMAX - 3;

    pdlaPrev = pdla++;
	while( *lpw && cla-- ) {

		// Determine length of this field
		lpwLast = lpw;
		lpw = LpwFindNotEq( lpwLast );
		pdla->cb = lpw - lpwLast;

		// Determine the attribute for the field
		pdla->pElement = *( lpwLast ) == wNoType || *lpf++ ? fmtelMemory : fmtelHilight;

        if ( pdlaPrev->pElement == pdla->pElement ) {
            pdlaPrev->cb += pdla->cb;
        }
        else {
            pdlaPrev = pdla++;
        }
	}
	// Beyond the end of the line, or out of range, just display
	// non-hilited.
    if ( pdlaPrev->pElement == fmtelMemory) {
        pdla = pdlaPrev;
    }
    else {
    	pdla->pElement = fmtelMemory;
    }
    pdla->cb = 0xffff;
}

//// MakeWindowActive
//
// Purpose: Update the active memory window information and the
// status bar.
//
// Input:
//
// Output:
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: MakeWindowActive() {
}

//// OOMError
//
// Purpose: Let user know that an OOM error has occured and terminate
// the window which received the error.
//
// Input:
//
// Output:
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: OOMError() {
	// CVMessage ( ERRORMSG, ENOMEM, MSGBOX );
	ASSERT ( FALSE );
}

//// DoDelChar
//
// Purpose: Handle either BSP or DEL key.
//
// Input:
//	fBkSpc	Is this a BackSpce key hit?
//
// Output:
//	Updated window or Beep
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: DoDelChar( BOOL	fBkSpc ) {
	USHORT	cch;
	UOFFSET	uo;
	USHORT	ucls;
	WORD	ichStart;
	DWORD	iln;
	WORD	ich;
	char	sz[ cchBufMax + 2 ];
	USHORT	imwd;
	char *	psz;
	int		cbMove;
	int		cbOff;
	char *	pch;

	// If the debuggee is running or there's a selection,
	// don't allow typing
	if ( GetEditSelection( 2, sz ) ) {
		MessageBeep( 0 );
		return;
	}

	// Get the information about the current line
	cch = CchGetCellInfo( fBkSpc, &uo, &ucls, &ichStart );
	iln = OlnGetCurPos();
	ich = ObGetCurPos();

	imwd = m_mwi.pmwd - rgmwd;

	// Don't allow editing of ASCII data, padding, and don't
	// allow back-spacing on the first char of a cell.  The
	// Delete would be a space anyway.
	if ( ucls == MW_ASCIIDAT || ucls == MW_NONE ||
		( fBkSpc && ich == ichStart ) ||
		( IsCharFormat( imwd ) && ucls == MW_FMT ) ) {

		MessageBeep( 0 );
	}
	else {
		FormatLine( iln, sz + 2 );

		// If they're DEL-ing, delete this char.
		// Otherwise, delete the previous char.
		cbOff = ( fBkSpc ? 1 : 0 );
		psz = sz + 2 + ichStart;
		cbMove = ich - ichStart - cbOff;

		// If we're not at the beginning of the field
		// then we have to shift the other data right.
		if ( cbMove ) {
			memmove( psz + 1, psz, cbMove );
		}

		// Replace the leading char with a space.
		// The sscanf will take care of the rest.
        if ( ucls == MW_ADDR ) {
            sz[ 0 ] = '0';
            sz[ 1 ] = 'x';
    		*psz = '0';
            psz -= 2;
            cch += 2;
        }
        else {
    		*psz = ' ';
        }

		// If we've deleted the rightmost char, then
		// we have to pop in a zero to ensure a zero
		// value.
		pch = psz + cch - 1;
		if ( *pch == ' ' ) {
		 	*pch = '0';
		}

		// Null terminate the cell and change the user's
		// memory.
		*( psz + cch ) = chNull;
		FChangeMemory( fBkSpc, 1, psz );

		// Redraw the changes.
        if ( ucls != MW_ADDR ) {
    		DrawLine( iln );
        }
	}
}

//// FIsAddrInRange
//
// Purpose: Determin if one address is contained within a pair of
// addresses.
//
// Input:
//	paddrLook	Address to see if in range.
//	paddrMin	Low address for range.
//	paddrMac	High address for range.
//
// Output: Returns TRUE if paddrMin <= paddrLook <= paddrMac
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: FIsAddrInRange(
PADDR	paddrLook,
PADDR	paddrMin,
PADDR	paddrMac ) {
    BOOL    fRet = FALSE;
	DWORD	result;

	VERIFY(OSDCompareAddrs(hpidCurr, paddrMin, paddrLook, &result) == xosdNone);
	if ((long)result <= 0) {
		VERIFY(OSDCompareAddrs(hpidCurr, paddrLook, paddrMac, &result) == xosdNone);
		fRet = ((long)result <= 0);
	}	
	return fRet;
}
