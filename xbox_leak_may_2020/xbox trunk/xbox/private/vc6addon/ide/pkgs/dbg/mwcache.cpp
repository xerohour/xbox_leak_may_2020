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

//// MWCacheDebuggeeData
//
// Purpose: To read in all required debuggee data for repainting a window
// with 1 read, to avoid numerous calls to DHGetDebuggeeBytes.
//
// Input:
//
// Output: None.
//
// Exceptions:
//
// Notes: Uses private locals above
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: CacheDebuggeeData(HLLI *phlli, LPB *plpb )
{
	RRC		rrc;
	WORD	cb;
	WORD	cPage;
	ADDR	addrT;

	if ( !DebuggeeAlive() )
		return;

	// Destroy the old cache before creating a new one
	DestroyCache(phlli, plpb);

	// Begin cache at top of visible window
	MWAddrFromIln( &m_mwi, OlnGetTop(), &addrT );

	// Fiqure out number bytes needed
	GetClientRrc( &rrc );

    // Allow for 1 additional row for partially painted row
	cb = m_mwi.pmwd->cbType * ( rrc.ryBottom + 2 );
    if ( m_mwi.cFmtVals ) {
    	cb *= m_mwi.cFmtVals;
    }
    else {
        cb *= (WORD)rrc.rxRight;
    }

    if ( !cb ) {
        ++cb;
    }

    cPage = ( cb + cbPageMax - 1 ) / cbPageMax;

	if ( ( *plpb = (unsigned char *)_fmalloc( cbPageMax * cPage ) ) &&
        ( *phlli = LLHlliInit( sizeof( MWC ), llfNull, NULL, NULL ) ) ) {
		HLLE	hlleLast = hlleNull;
		LPMWC	lpmwc;
		LPB		lpb = *plpb;
		UOFFSET	uo;
		WORD	cbReq = 0;
        HLLI    hlli = *phlli;

       	// Try to read partial first page
		uo = GetAddrOff( addrT ) & ~( cbPageMax - 1 );
		if ( uo != GetAddrOff( addrT ) ) {
			HLLE	hlle = hlleLast = LLHlleCreate( hlli );

			if ( hlle ) {
				lpmwc = (LPMWC)LLLpvFromHlle( hlle );

				LLAddHlleToLl( hlli, hlle );

				lpmwc->addrBegin = addrT;
				lpmwc->cbReq =
					min( (WORD)( cbPageMax - ( GetAddrOff( addrT ) - uo ) ), cb );
                lpmwc->lpb = lpb;

                lpmwc->cbRead =
                    DHGetDebuggeeBytes( addrT, lpmwc->cbReq, lpb );

				// Don't unlock hlle since it will be unlocked
				// later on.  We will use the lpmwc on the next page
				// to try to have as few entries in this list as possible

				lpb += lpmwc->cbRead;

				// Comparison below checks for overflow in
				// unsigned values.  If there's an overflow,
				// then need to adjust the segment
				if ( uo + cbPageMax < uo ) {
					SetAddrSeg( &addrT, GetAddrSeg( addrT ) + 1 );
				}
				SetAddrOff( &addrT, uo + cbPageMax );

				// If cPage starts out at 1, then we are reading <= 1 pages and
				// we've crossed a page boundary.  If so, don't change cPage,
				// so the loop below doesn't exit before doing anything.
				if ( cPage != 1 || lpmwc->cbReq >= cb ) {
					--cPage;
				}
				cbReq = lpmwc->cbReq;
			}
		}

		// Read remaining full pages and possible partial last page
		while( cPage-- ) {
			MWC	mwc;

			mwc.addrBegin = addrT;
			// If were not on the last page, read an entire page
			if ( cPage ) {
				mwc.cbReq = cbPageMax;
			}
			// Otherwise, read only what's necessary.  The actual
			// number of bytes is the total requested less the
			// total that we've tried to furnish
			else {
				mwc.cbReq = cb - cbReq;
			}
			mwc.lpb = lpb;
			mwc.cbRead = DHGetDebuggeeBytes( addrT, mwc.cbReq, lpb );

			// If the last node was a complete read and the segments
			// match for the next read, then we can extend the previous
			// cache item to include this one
			if ( hlleLast &&
				lpmwc->cbReq == lpmwc->cbRead &&
				GetAddrSeg( lpmwc->addrBegin ) == GetAddrSeg( addrT ) ) {

				lpmwc->cbReq += mwc.cbReq;
				lpmwc->cbRead += mwc.cbRead;
			}
			else {
				HLLE	hlle = LLHlleCreate( hlli );

				if ( hlle ) {
					// Unlock the old last node
					if ( hlleLast ) {
						UnlockHlle( hlleLast );
					}

					// Have the new mwc be the last
					lpmwc = (LPMWC)LLLpvFromHlle( hlle );
					hlleLast = hlle;

					// Add to the list (can't fail) and
					// initialize the mwc's data
					LLAddHlleToLl( hlli, hlle );
					*lpmwc = mwc;

					// Don't unlock the hlleLast since it will
					// be unlocked when we get out of the
					// while loop or we make it back here
				}
				else {
					// Set condition to get out of the while loop
					cPage = 0;
				}
			}

			// Update for the next pass
			cbReq += mwc.cbReq;
			lpb += mwc.cbRead;

			// See if the UNSIGNED offset will overflow to the
			// next segment.  If so, adjust the segment
			if ( GetAddrOff( addrT ) + lpmwc->cbReq < GetAddrOff( addrT ) ) {
				SetAddrSeg( &addrT, GetAddrSeg( addrT ) + 1 );
			}

			// Always page aligned
			SetAddrOff( &addrT, GetAddrOff( addrT ) + cbPageMax );
        }

		if ( hlleLast ) {
			UnlockHlle( hlleLast );
		}
    }
}

//// DestroyCache
//
// Purpose: Free up memory associated with specified cache
//
// Input:
//      phlli   Pointer to list containing cache info
//      plpb    Pointer to buffer for cache data
//
// Output: None.
//
// Exceptions:
//
// Notes: Destroy the list and zero out handle (if not null).  Same
//        for data buffer
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: DestroyCache(
HLLI *  phlli,
LPB *   plpb ) {

    // Destroy the diff info
    if ( *phlli ) {
        LLChlleDestroyLl( *phlli );
        *phlli = (HLLI)NULL;
    }

	// Destroy the data cache
    if ( *plpb ) {
        _ffree( *plpb );
        *plpb = (LPB)NULL;
    }
}

//// UncacheData
//
// Purpose: Free up memory associated with cached debuggee data.
//
// Input: None.
//
// Output: None.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: UncacheData() {
    DestroyCache( &m_hlliCur, &m_rgbCacheCur );
}

//// UncacheAll
//
// Purpose: Free up memory associated with display caches
//
// Input: None.
//
// Output: None.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: UncacheAll() {
    // Destroy the display cache
    DestroyCache( &m_hlliCur, &m_rgbCacheCur );

    // Destroy the diff info
    DestroyCache( &m_hlliPrev, &m_rgbCachePrev );
}

//// LpbFromCache
//
// Purpose: Given a cache and an address, return pointer to data
//  where the cached user memory is.
//
// Input:
//      hlli    	Cache list for data info
//      paddr   	address to get data from cache for
//      cb      	number of bytes at paddr to ensure read for
//		pcbAvail	Number of bytes available for partial reads
//
// Output:
//      returns
//          (LPB)0xffffffffL:  Memory would be cached if there was
//                             data there.
//          (LPB)NULL       :  Memory would not be contained in cache
//          else            :  Valid pointer to cached data
//
// Exceptions:
//
// Notes:
//      This function will return (LPB)NULL if there is no cache
//
///////////////////////////////////////////////////////////////////////////
LPB CMemoryView :: LpbFromCache(
HLLI    	hlli,
PADDR   	paddr,
size_t  	cb,
size_t * 	pcbAvail ) {
    LPB 	lpbRet = 0L;
	size_t	cbRead = 0;
	size_t	cbRequest = cb;
	BOOL    fFound = FALSE;

    // Make sure that there's a list
    if ( hlli ) {
		BOOL	fPartial = FALSE;
        HLLE    hlle = (HLLE)NULL;
        UOFFSET uoReq = GetAddrOff( *paddr );

        // Loop through all entries in cache until exhausted or
        // found
        while( !fFound && ( hlle = LLHlleFindNext( hlli, hlle ) ) ) {
            LPMWC   lpmwc = (LPMWC)LLLpvFromHlle( hlle );

            // First see if segments match up
            if ( GetAddrSeg( *paddr ) == GetAddrSeg( lpmwc->addrBegin ) ) {
                UOFFSET uoSav = GetAddrOff( lpmwc->addrBegin );

                // Segments match up.  See if data is contained within
                // the cached request.
                if ( uoSav <= uoReq && uoReq <= uoSav + lpmwc->cbReq - 1 ) {
					UOFFSET	uoLast = uoReq + (UOFFSET)cb - 1L;

                    // Determine if there is REAL data to satisfy read
                    // request.  If so, return a pointer to the data,
                    // else return 0xffffffff
                    if ( lpmwc->cbRead ) {

						// Entire request can be furnished by this cache
						if ( uoLast <= uoSav + lpmwc->cbRead - 1 ) {
							
							// If !fPartial, then save starting buffer
							// address.  Otherwise, assume that we
							// are attempting to finish a cache read
							// which crosses 2 consecutive caches
							if ( !fPartial ) {
	                        	lpbRet = lpmwc->lpb + uoReq - uoSav;
							}
							// Everything has been read in
							cb = 0;

	                    	fFound = TRUE;
	                    }

						// Cache is not able to fill request, but can
						// fill first part of request and next cache
						// may be able to complete request.  Only do
						// partial reads if pcbAvail != NULL
    	                else if ( lpmwc->cbRead == lpmwc->cbReq && pcbAvail ) {
	                        lpbRet = lpmwc->lpb + uoReq - uoSav;

							// Adjust remaining bytes to read and
							// "beginning" offset
							cb = (size_t)( uoLast - ( uoSav + lpmwc->cbRead - 1 ) );
							uoLast += (UOFFSET)cb;
							fPartial = TRUE;
						}

						// Memory doesn't exist for request to be satisifed
						else {
							fFound = TRUE;
            	        }
					}

					// Memory doesn't exist for request to be satisifed
					else {
						fFound = TRUE;
					}
                }
				else if ( fPartial ) {
					fFound = TRUE;
				}
            }

            UnlockHlle( hlle );
        }
    }

	// If pcbAvail != NULL, then fill in the number of bytes read in.
	// cbRequest is actual requested, cb is remaining bytes to read.
	if ( pcbAvail ) {
		*pcbAvail = cbRequest - cb;

		// If cb == cbRequest, then nothing was read in, but the
		// memory would have been cached if it existed.  But it
		// didn't, so return the number of bytes read plus set the
		// return buffer to indicate that a cached read failed
		if ( cb == cbRequest && fFound ) {
			lpbRet = (LPB)0xffffffffL;
		}
	}

	// Otherwise, if there is anything left to read and at some point
	// we found a buffer with a partial data cached (readable or not),
	// set to a failed read request
	else if ( cb && fFound  ) {
		lpbRet = (LPB)0xffffffffL;
	}

    return lpbRet;
}

//// FCmpUserMemory
//
// Purpose: Compare values with thosed save from the last MWSaveDebuggeData
// call.
//
// Input:
//		paddr	Address of debugged data we're to compare.
//		cbCmp	Number of BYTES to compare.
//		rgbCmp	Data to compare against the debuggee.
//
// Output:
//		Return TRUE if:
//			1) We don't have the debuggee data to compare against, or
//			2) The data are equivalent.
//		Return FALSE if:
//			The data is different.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: FCmpUserMemory(
PADDR	paddr,
USHORT	cbCmp,
BYTE *	rgbCmp ) {
	BOOL	fRet = TRUE;
    LPB     lpb1;

    // Locate display data in cache
    lpb1 = LpbFromCache( m_hlliCur, paddr, cbCmp );

    // If the data's there, continue
    if ( lpb1 && lpb1 != (LPB)0xffffffffL ) {

        // Locate the compare data in the other cache
        LPB lpb2 = LpbFromCache( m_hlliPrev, paddr, cbCmp );

        // If the data's there, then do the compare
        if ( lpb2 && lpb2 != (LPB)0xffffffffL ) {
            switch( cbCmp ) {
                case 1:
                    fRet = *lpb1 == *lpb2;
                    break;

                case 2:
                    fRet = *(UNALIGNED short FAR *)lpb1 == *(UNALIGNED short FAR *)lpb2;
                    break;

                case 4:
                    fRet = *(UNALIGNED long FAR *)lpb1 == *(UNALIGNED long FAR *)lpb2;
                    break;

                case 8: {
                    UNALIGNED long FAR *  lpl1 = (UNALIGNED long FAR *)lpb1;
                    UNALIGNED long FAR *  lpl2 = (UNALIGNED long FAR *)lpb2;

                    fRet = *lpl1 == *lpl2 && *(lpl1 + 1) == *(lpl2 + 1);
					break;
				}

                default:
                    fRet = !_fmemcmp( lpb1, lpb2, cbCmp );
                    break;
            }
        }
    }

	return fRet;
}

//// CbGetDebuggeeBytes
//
// Purpose: Get debuggee data from cached memory to avoid the hit of
// DHGetDebuggeeBytes() calls.
//
// Input:
// 	paddr		Debuggee address to get bytes for.
// 	cbNeeded	# of bytes requested
//
// Output:
// 	lpb		far pointer to buffer to stuff debuggee data.
//
// Return value: Returns the number of bytes actually gotten.
//
// Exceptions:
//
// Notes: If the requested data is not-cached, or there is no cache,
// we will call DHGetDebuggeeBytes() for the request.
//
///////////////////////////////////////////////////////////////////////////
USHORT CMemoryView :: CbGetDebuggeeBytes(
PADDR		paddr,
size_t	    cbNeeded,
BYTE far * 	lpb ) {
	USHORT	cbRet;
    LPB     lpbCache;
	size_t	cbOriginal;

	cbRet = 0;
	cbOriginal = cbNeeded;

	// Let the world know somebody wants nothing.
	ASSERT( cbNeeded );

    // Try to get the data from the cache as it is now
    lpbCache = LpbFromCache( m_hlliCur, paddr, cbNeeded, &cbNeeded );

    // If the read fails, reset the cache and try again
    if ( !lpbCache ) {
        CacheDebuggeeData( &m_hlliCur, &m_rgbCacheCur );
        lpbCache = LpbFromCache( m_hlliCur, paddr, cbOriginal, &cbNeeded );
    }

    // If the read failed from the cache, just go to OSDebug for the read
    if ( !lpbCache ) {
		cbRet = DHGetDebuggeeBytes( *paddr, cbOriginal, lpb );
    }

    // Otherwise, if the lpbCache is NOT -1, then copy the data.  If it
    // is -1, then OSDebug wouldn't be able to satisfy request, so fail

    else if ( lpbCache != (LPB)0xffffffffL ) {
		_fmemcpy( lpb, lpbCache, cbNeeded );
        cbRet = cbNeeded;
    }

    return cbRet;
}

//// FPutDebuggeeBytes
//
// Purpose: To change debuggee data, but keep old data for undo
//
// Input:
//		ucls	Input class (MW_RAW, MW_ASCIIDAT, MW_FMT)
//	 	paddr	Address of changed data
//	 	cb		# of bytes to change
//	 	rgb		Data to update to
//	 	hlli	Linked list information associated with window
//
// Output: Returns TRUE if write succeeded
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
BOOL CMemoryView :: FPutDebuggeeBytes(
USHORT	ucls,
PADDR	paddr,
size_t	cb,
LPB 	rgb,
HLLI	hlliUndo,
HLLI	hlliRedo
) {
	HLLE	hlle;
	LPMUB	lpmub;
	BOOL	fUpdate;
	size_t	cbWritten;
	LPB		lpb;

	// fUpdate indicates if we need to update the list of undoable
	// data.
	fUpdate = TRUE;

	// See if the address and size of the last change matches the current.
	// If so, don't add new item to the list.  Assume that they're changing
	// another digit of the same item.

	if ( hlle = LLHlleGetLast( hlliUndo ) ) {
		lpmub = (LPMUB)LLLpvFromHlle( hlle );
		if ( GetAddrSeg( lpmub->addr ) == GetAddrSeg( *paddr ) &&
			GetAddrOff( lpmub->addr ) == GetAddrOff( *paddr ) &&
			lpmub->cb == cb ) {
			fUpdate = FALSE;
		}
		UnlockHlle( hlle );
	}

	// If the value is not at the end of the list, save the old version
	// of the data before writing the new
	if ( fUpdate ) {
		if ( hlle = LLHlleCreate( hlliUndo ) ) {
			size_t	cbRead;

			lpmub = (LPMUB)LLLpvFromHlle( hlle );
			lpmub->ucls = ucls;
			lpmub->pmwd = m_mwi.pmwd;
			lpmub->addr = *paddr;
			
			// Try to get old data from cache
			if ( lpb = LpbFromCache( m_hlliCur, paddr, cb, &cbRead ) ) {
				if ( lpb != (LPB)0xffffffffL ) {
					_fmemcpy( lpmub->rgb, lpb, cbRead );
				}
			}
			// If not in the cache, go to OSDebug
			else {
				cbRead = DHGetDebuggeeBytes( *paddr, cb, (char far *)lpmub->rgb );
			}

			lpmub->cb = cbRead;

			UnlockHlle( hlle );
			LLAddHlleToLl( hlliUndo, hlle );
			m_undoBuffer = ubUndo;
		}
	}

	// Thow away redo buffer
	m_mePlayCount = 0;

	while( LLFDeleteHlleIndexed( hlliRedo, 0L ) );

	cbWritten = DHPutDebuggeeBytes( *paddr, cb, rgb );

	// Only update the display cache if the WRITE succeeded.  We
	// may have read-only data and we don't want to update the
	// display if we couldn't change the user's memory
	if ( cbWritten ) {
		// Update the cached display data so the cache doesn't
		// need to be invalidated and reloaded.  If it's not
		// already in the cache, we don't have to worry since
		// it will be cached later or read from OSDebug
		if ( ( lpb = LpbFromCache( m_hlliCur, paddr, cbWritten ) ) &&
			lpb != (LPB)0xffffffffL ) {

			_fmemcpy( lpb, rgb, cbWritten );
		}
	}

	return cbWritten == cb;
}

//// SaveDebuggeeData
//
// Purpose: Retrieve the debuggee data for compares to show changes
// between debuggee execution.
//
// Input: None.
//
// Output: None.
//
// Exceptions:
//
// Notes: Save debuggee data.  This data is used for showing diffs
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: SaveDebuggeeData() {
     // Load the cache with private args
	CacheDebuggeeData( &m_hlliPrev, &m_rgbCachePrev );
}

//// ChangeDebuggeeData
//
// Purpose: Visible portion of MW display changed, adjust diff's cache
//          so that user will see diffs for direct edits
//
// Input: None.
//
// Output: None.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: ChangeDebuggeeData() {
	ADDR	addrMin;
	ADDR	addrMac;
	USHORT	cb;
	RRC		rrc;

	// Compute #of bytes displayed in window
	GetClientRrc( &rrc );
	cb = m_mwi.pmwd->cbType * m_mwi.cFmtVals * ( rrc.ryBottom + 1 );

	MWAddrFromIln( &m_mwi, OlnGetTop(), &addrMin );
	addrMac = addrMin;
	SetAddrOff( &addrMac, GetAddrOff( addrMin ) + (UOFFSET)cb - 1 );

    // If either end of the visible data is not cached, reset
    // the cache.  Otherwise, leave it alone.
    if ( !LpbFromCache( m_hlliPrev, &addrMin, 1 ) ||
        !LpbFromCache( m_hlliPrev, &addrMac, 1 ) ) {

        SaveDebuggeeData();
    }
}

//// MWUpdateWorld
//
// Purpose: A memory value has changed, update all other windows
//
// Input:
//	paddr	Address to be updated.
//	cb	Number of bytes being updated.
//
// Output:
//	Windows are updated.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView::UpdateWorld(PADDR paddr, USHORT cb) 
{
	Unreferenced ( paddr );
	Unreferenced ( cb );

	UpdateDebuggerState( UPDATE_ALLDBGWIN & ~UPDATE_MEMORY );
}
