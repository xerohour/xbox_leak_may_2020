/****************************************************************************
 *
 * Mapping routines -
 *
 *      The following are routines that are used to generate the map
 *      used to rapidly display lines in the source window.
 *
 * The map consists of several components of the swinfo structure.
 *
 *  lnMapStart - The display line associated with the top of the map
 *  lnEnd      - The display line representing the current buest
 *                  guess for the end of the virtual file.
 *  ismwInvalid- The beginning of the invalid region of the map
 *  rgdml[]    - An array of map entries
 *
 * There are three fundamental operations involved in maintaining the map.
 *
 *  Anchoring the map - Set the 0th map entry, the map start, and the
 *      lnEnd and invalidate the rest of the map by setting
 *      idmlInvalid == 0.
 *
 *  Shifting the map - Shift the map entries up or down and reset all
 *      of the appropriate map variables appropriately.  We fill in the
 *      beginning of the map when we shift it down since we are pretty
 *      certain that those entries will be used and it obviates the
 *      need of multiple invalid regions.
 *
 *  Filling a map entry - We can fill a map entry from either a previous
 *      entry or a next entry.  This allows the shifting of the map
 *      and filling of the invalid region for display while maintaining
 *      the integrity of the screen in mixed and disassmebly modes.
 *
 *      There are three modes that the mapper can use, ( mixed mode
 *      has two sub- modes ) each of which uses its own algorithm
 *      to derive a map entry from a previous or next entry.  The
 *      precises algorithms are described in the headers of each
 *      of the routines.
 *
 ****************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define lnFudge(ln) ((ln)/2)
#define JavalnFudge(ln) (3*(ln)/4)

/*** FillEntryFromPrev ******************************************************
 *
 * Purpose  : To fill the entry idml from the entry idml - 1 in the mode
 *          :   m_dmi.Mode.
 *          :
 * Input    : idml    - The index of the entry to be filled.
 *          :
 * Output   : None.
 *          :
 * Side     : The entry m_dmi.rgdml [ idml ] is filled based on the
 *  effects :   mode of the window and the entry m_dmi.rgdml [ idml - 1 ]
 *          :
 ****************************************************************************/

void CDAMView :: FillEntryFromPrev ( int idml ) {

    if ( m_dmi.rgdml [ idml - 1 ].met == metNone ) {
        m_dmi.rgdml [ idml ].met = metNone;
    }
    else if ( m_dmi.fAnnotate ) {
		FillMixedFromPrev ( idml );
	}
	else {
		FillAsmFromPrev ( idml );
	}
    m_dmi.fNextAddrSet = FALSE;
}


void CDAMView :: FillMixedFromPrev ( int idml ) {

    PDML pdmlPrev = &m_dmi.rgdml [ idml - 1 ];
    PDML pdml     = &m_dmi.rgdml [ idml ];

    if ( pdmlPrev->met == metSource ) {
        ADDR  addr = { 0 };
        SHOFF cb   = 0;

		// If the previous line doesn't have any address associated
		// with it, then it's probably (assumed) to be a contributing
		// source line for the specified address.  If it succeeds, then
		// it's the last line of the "source line" and we should now
		// include the disassembly which goes with it
        if ( SLFLineToAddr (
            pdmlPrev->mfl.hsf,
            pdmlPrev->mfl.wLine,
            &addr,
            &cb,
            NULL
        ) ) {
			if (pDebugCurr && pDebugCurr->IsJava ())
			{
				if (GetAddrSeg (addr) != GetAddrSeg (pdmlPrev->addr))
				{
					pdml->met  = metSource;
					pdml->mfl.hsf = pdmlPrev->mfl.hsf;
					pdml->mfl.wLine = pdmlPrev->mfl.wLine + 1;
				}
				else
				{
					pdml->met = metDisasm;
				}
			}
			else
			{
				pdml->met  = metDisasm;
			}
		}
        else {
            pdml->met  = metSource;
            pdml->mfl.hsf = pdmlPrev->mfl.hsf;
            pdml->mfl.wLine = pdmlPrev->mfl.wLine + 1;

			if (pDebugCurr && pDebugCurr->IsJava ())
			{
				LPMDF lpmdf = DAMLpmdfFromHSF (pdmlPrev->mfl.hsf);

				if (lpmdf != NULL)
				{
					ASSERT (pdml->mfl.hsf == lpmdf->hsf);

					if (pdml->mfl.wLine >= lpmdf->clnBuf)
					{
						m_dmi.lnEnd = m_dmi.lnMapStart + idml + 1;
					}
				}
			}
        }
		pdml->addr = pdmlPrev->addr;
    }
    else {
        ADDR  	addr = pdmlPrev->addr;
        CXT   	cxt  = { 0 };
        WORD  	ln   = 0;
        SHOFF 	db   = 0;
		HSF		hsf = (HSF)NULL;
		BOOL	fSource;
		PCXT	pcxt;

		if ( pdmlPrev->met != metFileName ) {
        	SDI   	sdi;

	        sdi.dop    = dopNone;
   		    sdi.addr   = addr;
			
	       	SYUnassemble ( hpidCurr, htidCurr, &sdi );
    	    addr = sdi.addr;
		}

		fSource = SLLineFromAddr ( &addr, &ln, NULL, &db );
		if ( SHSetCxt( &addr, &cxt ) ) {
			hsf = SLHsfFromPcxt( &cxt );
		}

		// If we're mixed mode, then get the context of the previous
		// line to see if we've changed HSFs
		if ( m_dmi.fAnnotate &&
			pdmlPrev->met != metFileName &&
			( !( pcxt = SHSetCxt( &pdmlPrev->addr, &cxt ) ) && hsf ) ||
			( pcxt && SLHsfFromPcxt( &cxt ) != hsf )
		) {
			pdml->met = metFileName;
			pdml->mfl.hsf = hsf;
			pdml->addr = addr;

			// If we've crossed a file boundary, it's possible
			// that the assembly didn't match up, so realign the
			// disassembly to the current line
			if ( fSource && db != 0 ) {
				SetAddrOff( &pdml->addr, GetAddrOff( pdml->addr ) - (UOFFSET)db );
			}
		}

		// If it's a label, then there's no source matching it.  Just set the
		// next to metDisasm
		else if ( pdmlPrev->met == metLabel ) {
			*pdml = *pdmlPrev;
			pdml->met = metDisasm;
		}
		else {
			char	rgch[ cbSymMax ];

			// Always use the address
			pdml->addr = addr;

			// Now we have the address of the next instruction (addr).
			// If there's source line information about it and we are
			// EXACTLY at the beginning, scan back in the source file
			// to include everything which may have contributed to the
			// disassembly (as in, stop when we've found another
			// contributing line or the beginning of the file
    	    if ( fSource &&
    	    	db == 0 &&
    	    	hsf &&
				DAMFLoadHSFFile( hsf )
    	    ) {
        	    WORD	rgln[2];

	            rgln[ 0 ] = ln - 1;

	            do {
    	            ADDR  addrT = { 0 };
        	        SHOFF cbT = 0;
				
                	ln = rgln[ 0 ];
	                if ( SLFLineToAddr (
    	                hsf,
        	            ln,
            	        &addrT,
                	    &cbT,
	                    rgln
    	            ) ) {
        	            break;
            	    }
	            } while ( ln > 0 );

	            pdml->met = metSource;
    	        pdml->mfl.hsf = hsf;
            	pdml->mfl.wLine = ln + 1;
    	    }
	        else if ( m_dmi.fDoSymbols && SHAddrToLabel ( &addr, rgch ) ) {
	            pdml->met = metLabel;
			}
			else {
				if (pDebugCurr && pDebugCurr->IsJava ())
				{
					if (ln != 0 && db >= GetAddrOff (m_dmi.addrSegLim))
					{
						pdml->met = metSource;
						pdml->mfl.hsf = hsf;
						pdml->mfl.wLine = (WORD) (ln + db - GetAddrOff (m_dmi.addrSegLim)) + 1;
					}
					else
					{
						pdml->met = metDisasm;
					}
				}
				else
				{
					pdml->met = metDisasm;
				}
	        }
		}
    }
}

void CDAMView :: FillAsmFromPrev ( int idml ) {

    PDML pdmlPrev = &m_dmi.rgdml [ idml - 1 ];
    PDML pdml     = &m_dmi.rgdml [ idml ];
    ADDR addr     = pdmlPrev->addr;

    if ( GetFixedOff ( addr ) >= SegmentLength ( addr ) ) {
        pdml->met = metNone;
        m_dmi.lnEnd = m_dmi.lnMapStart + idml;
        m_dmi.idmlInvalid = idml;
    }
    else if ( m_dmi.fDoSymbols && pdmlPrev->met == metLabel ) {
        pdml->met = metDisasm;
    }
    else {
        SDI  sdi;
        char rgch [ cbSymMax ];

        if ( m_dmi.fNextAddrSet ) {
            addr = pdml->addr;
        }
        else {
            sdi.dop    = dopNone;
            sdi.addr   = addr;

            SYUnassemble ( hpidCurr, htidCurr, &sdi );
            addr = sdi.addr;
        }

        if (
            m_dmi.fDoSymbols &&
            pdmlPrev->met == metDisasm &&
            SHAddrToLabel ( &addr, rgch )
        ) {
            pdml->met = metLabel;
        }
        else {
            pdml->met = metDisasm;
        }
    }

    pdml->addr = addr;
}

int	CDAMView :: FillAsmFromNext( BOOL fAnnotate, PDML pdmlNext, PDML pdml ) {
    int     ddmlRet  = -1;
	ADDR  	addr = pdmlNext->addr;

	int ib = SYGetPrevInst ( hpidCurr, htidCurr, &addr );

    if ( ib <= 0 ) {
    	pdml->met  = metDisasm;
        pdml->addr = addr;
    }
	else {
    	SetAddrOff (
        	&pdmlNext->addr,
            GetAddrOff ( pdmlNext->addr ) - ib
        );
	    pdml->met  = metDisasm;
		pdml->addr = addr;
	}

	// If the new address is part of a source line
	// realign with it's source line.  This assumes
	// that the backwards disassembly makes alignment
	// errors.  (Which it does)
	AlignAddrWithSource( &pdmlNext->addr, &pdml->addr );

	// If we're in mixed mode, see if we've crossed an HSF
	// boundary.  If so, overwrite the DML.
	if ( fAnnotate && pdmlNext->met != metFileName ) {
		HSF	hsfNext = (HSF)NULL;
		HSF hsf = (HSF)NULL;
		CXT cxt = {0};

		if ( SHSetCxt( &pdmlNext->addr, &cxt ) ) {
			hsfNext = SLHsfFromPcxt( &cxt );
		}

		if ( SHSetCxt( &pdml->addr, &cxt ) ) {
			hsf = SLHsfFromPcxt( &cxt );
		}

		if ( hsf != hsfNext ) {
			pdml->addr = pdmlNext->addr;
			pdml->met = metFileName;
			pdml->mfl.hsf = hsfNext;
			ddmlRet = -1;
		}
	}

	return ddmlRet;
}

int  CDAMView :: FillEntryFromNext ( int idml ) {
    int     ddmlRet  = -1;
    PDML    pdmlNext = &m_dmi.rgdml [ idml + 1 ];
    PDML    pdml     = &m_dmi.rgdml [ idml ];

	switch( pdmlNext->met ) {
		// If next item is source try to add previous items
		// which don't contribute code.  Probably multi-line
		// statements
		case metSource: {
	        ADDR  addr = { 0 };
    	    SHOFF cb   = 0;

			if (pDebugCurr && pDebugCurr->IsJava ())
			{
				if (pdmlNext->mfl.wLine > 1)
				{
					if (SLFLineToAddr (pdmlNext->mfl.hsf, pdmlNext->mfl.wLine - 1, &addr, &cb, NULL) &&
						(GetAddrSeg (pdmlNext->addr) == GetAddrSeg (addr)))
					{
						ddmlRet = FillAsmFromNext( m_dmi.fAnnotate, pdmlNext, pdml );
					}
					else
					{
						*pdml = *pdmlNext;
						pdml->mfl.wLine -= 1;
					}
				}
				else
				{
					*pdml = *pdmlNext;
					pdml->met = metFileName;
				}
			}
			else
			{
				if (
            		pdmlNext->mfl.wLine > 1 &&
        			!SLFLineToAddr (
    					pdmlNext->mfl.hsf,
						pdmlNext->mfl.wLine - 1,
            			&addr,
        				&cb,
    					NULL
				) ) {
        			*pdml = *pdmlNext;
    				pdml->mfl.wLine -= 1;
				}
        		else {
					ddmlRet = FillAsmFromNext( m_dmi.fAnnotate, pdmlNext, pdml );
				}
			}
			break;
    	}

		// metNone is an error, propagage!
	    case metNone:
			pdml->met = metNone;
			break;

		// Regular case where we are attempting to back up in assembly code
		case metDisasm:
		case metLabel: {
		case metFileName:
			ADDR	addr = pdmlNext->addr;
			char	rgch[ cbSymMax ];
			CXT		cxt;
			HSF		hsf;
			WORD	wLine;
			SHOFF	db;
			BOOL	fLabel = FALSE;

			if ( m_dmi.fDoSymbols ) {
				fLabel = SHAddrToLabel( &addr, rgch );
			}

			// Show address labels if:
			//		1) Not in mixed mode
			//		2) Symbols option turned on (fLabel)
			//		3) Backing up from disassembly (not source or a label)
			if ( !m_dmi.fAnnotate &&
				pdmlNext->met == metDisasm &&
				fLabel
			) {

				pdml->met = metLabel;
				pdml->addr = addr;
			}

			// Try to find a source line
			else if ( pdmlNext->met != metFileName &&
				m_dmi.fAnnotate &&
				SHSetCxt( &addr, &cxt ) &&
				( hsf = SLHsfFromPcxt( &cxt ) ) &&
				SLLineFromAddr( &addr, &wLine, NULL, &db ) &&
				db == 0 &&
				DAMFLoadHSFFile( hsf )
			) {

				pdml->met = metSource;
				pdml->addr = pdmlNext->addr;
				pdml->mfl.hsf = hsf;
				pdml->mfl.wLine = wLine;
			}

			// No source, don't care if annotating, use a label
			// if we've got one (fSymbols already checked)
			else if ( pdmlNext->met == metDisasm && fLabel ) {
				pdml->met = metLabel;
				pdml->addr = addr;
			}
			else
			if ((pdmlNext->met == metFileName || pdmlNext->met == metLabel) &&
				pDebugCurr && pDebugCurr->IsJava ())
			{
				ddmlRet = 0;
			}
			// All else failed, just get the previous address.
			else {
				ddmlRet = FillAsmFromNext( m_dmi.fAnnotate, pdmlNext, pdml );
			}
			break;
		}

		default:
			ASSERT( FALSE );
			break;
	}

    return ddmlRet;
}

int CDAMView :: ShiftMap ( int ddml) {

    //sert ( ddml != 0 );
    ASSERT ( ddml < m_dmi.cdml - 1 && ddml > -( m_dmi.cdml - 1) );

    m_dmi.fNextAddrSet = FALSE;

    //
    // Invalidate the cache
    //
    m_dmi.fNewMap        = TRUE;

    if ( ddml < 0 ) {
        int idml;

        // Shift the map down by dwsm entries

        memmove (
            &m_dmi.rgdml [ -ddml ],
            &m_dmi.rgdml [ 0 ],
            ( m_dmi.cdml - abs ( ddml ) ) * sizeof ( DML )
        );

        // Fill in the new map entries - we can do this here since we
        //  know that they will be used - it also prevents requiring
        //  two invalid regions.

        for ( idml = -ddml; idml > 0; ) {
            int ddmlT = FillEntryFromNext ( idml - 1 );

            if ( ddmlT >= 0 ) {

                // We reached the top of the "document"

                memmove (
                    &m_dmi.rgdml [ 0 ],
                    &m_dmi.rgdml [ idml ],
                    ( m_dmi.cdml - idml ) * sizeof ( DML )
                );

				if (pDebugCurr && pDebugCurr->IsJava ())
				{
					// adjust the invalid region
					if (m_dmi.idmlInvalid <= idml)
					{
						m_dmi.idmlInvalid = 1;
					}
					else
					{
						m_dmi.idmlInvalid -= idml;
					}
				}
				else
				{
					//
					// if there was an invalid region at
					// the end of the map it may be larger
					// due to the movement of the map up since
					// we hit the top of the map.
					// For now I will assume that it is safe to
					// make it the size of the real amount shifted
					// or the calculated amount, whichever is smaller
					// just to make sure we don't have garbage
					// at the end for small files.
					//
					if ( !( m_dmi.idmlInvalid = min (
						m_dmi.idmlInvalid + (ddml + idml),
						-(ddml+idml) ) )
					) {

						m_dmi.idmlInvalid++;
					}
				}
                m_dmi.lnMapStart = 0;
                return idml;
            }

            idml += ddmlT;
        }

        // If there was an invalid region at the end of the map, it has
        //  shrunk by ddml entries

        m_dmi.idmlInvalid = min ( m_dmi.idmlInvalid - ddml, m_dmi.cdml );
    }
    else {

        // If we are about to shift all of the valid entries off the
        //  map, calculate another entry so that we continue to
        //  have a valid anchor point.

        if ( ddml == (int) m_dmi.idmlInvalid ) {
            FillEntryFromPrev ( ddml );
            m_dmi.idmlInvalid += 1;
        }

        // Shift the map up by ddml entries

        memmove (
            &m_dmi.rgdml [ 0 ],
            &m_dmi.rgdml [ ddml ],
            ( m_dmi.cdml - abs ( ddml ) ) * sizeof ( DML )
        );

        // We create ( or enlarge ) an invalid region at the end of the map

        ASSERT ( ddml < m_dmi.idmlInvalid );

        m_dmi.idmlInvalid = m_dmi.idmlInvalid - ddml;
    }

    m_dmi.lnMapStart = m_dmi.lnMapStart + ddml;
	
    return 0;
}


/*** DAMSetMap
*
* Purpose:  Given an address set up the source window map.  Setup includes
*           map region invalidation as well as initial filling in the
*           initial entry into the map in cases where the entire map
*           is invalid.
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
BOOL CDAMView :: SetMap (
HSF			hsf,
WORD    	wLine,
PADDR   	paddr,
BOOL    	fAdjustLine,
WORD    	dln
) {
    BOOL	fReturn = FALSE;
    BOOL    fMidLine = FALSE;
	CXT		cxt = { 0 };

    ASSERT ( paddr ); // Must have either a address  (file NOT supported!!!!!)

    // Turn off updates
    SendMessage ( WM_SETREDRAW, 0, 0L);

    m_dmi.fNextAddrSet = FALSE;

    //
    // Invalidate the cache
    //
    m_dmi.fNewMap = TRUE;

	// If we are annotating, try to put the source line
	// of the code at the top of the window
	if ( m_dmi.fAnnotate ) {
		SHOFF	db;

		// attempt to derive its file
		if (
			SHSetCxt ( paddr, &cxt ) &&
			( hsf = SLHsfFromPcxt ( &cxt ) ) &&
			SLLineFromAddr ( paddr, &wLine, NULL, &db ) &&
			db == (SHOFF)0 &&
			DAMFLoadHSFFile( hsf )
			) {

			m_dmi.rgdml[ 0 ].met = metSource;
			m_dmi.rgdml[ 0 ].addr = *paddr;
			m_dmi.rgdml[ 0 ].mfl.hsf  = hsf;
			m_dmi.rgdml[ 0 ].mfl.wLine = wLine;
	
			fReturn = TRUE;
		}
	}

	// If fReturn, then we are displaying plain code at the
	// top.  Note: If there's a label, use it!
	if ( !fReturn ) {
		ADDR addr = *paddr;
		char rgch [ cbSymMax ];

		if (
			m_dmi.fDoSymbols &&
			SHAddrToLabel ( &addr, rgch )
			) {
			m_dmi.rgdml [ 0 ].met = metLabel;
		}
		else {
			m_dmi.rgdml[ 0 ].met = metDisasm;
		}

		m_dmi.rgdml [ 0 ].addr = addr;

		fAdjustLine = FALSE;
		fReturn = TRUE;
	}

	m_dmi.idmlInvalid = 1;

	if (pDebugCurr && pDebugCurr->IsJava ())
	{
		LPMDF lpmdf = DAMLpmdfFromHSF( hsf );
		INT   cTotalLines;
		INT   cPrevLines;
		SHOFF	db = 0;

		if (lpmdf)
		{
			cTotalLines = lpmdf->clnBuf;
		}
		else
		{
			cTotalLines = 0;
		}

		paddr = &m_dmi.rgdml [0].addr;
		ULONG cbSeg = SegmentLength (*paddr);
		SYUnFixupAddr (paddr);

		if (GetAddrOff (*paddr) >= cbSeg)
		{
			SetAddrOff (paddr, cbSeg - 1);
		}

		wLine = 0;

		// attempt to derive its file
		if (m_dmi.fAnnotate && SHSetCxt (paddr, &cxt) &&
			(hsf = SLHsfFromPcxt (&cxt)))
		{
			SLLineFromAddr (paddr, &wLine, NULL, &db);
			cPrevLines = wLine;
		}
		else
		{
			cPrevLines = 0;
		}

#if 0
		m_dmi.lnMapStart = JavalnFudge (GetFixedOff (m_dmi.rgdml [0].addr)) + cPrevLines;
		m_dmi.lnEnd = JavalnFudge (SegmentLength (m_dmi.rgdml [0].addr)) + 1 + cTotalLines;
#else
		m_dmi.lnMapStart = cPrevLines + JavalnFudge (GetAddrOff (*paddr));
		m_dmi.lnEnd = cTotalLines + JavalnFudge (cbSeg) + 1;
#endif

		if (m_dmi.lnMapStart >= m_dmi.lnEnd)
		{
			m_dmi.lnMapStart = m_dmi.lnEnd - 1;
		}
	}
	else
	{
		m_dmi.lnMapStart = lnFudge ( GetFixedOff ( m_dmi.rgdml[ 0 ].addr ) );
		m_dmi.lnEnd = lnFudge ( SegmentLength ( m_dmi.rgdml[ 0 ].addr ) ) + 1;
	}
	
	//
	// Let CW Know that the current file position is the line we
	// just set the map to.
	//
	SetWinTopLine ( m_dmi.lnMapStart );
	if ( fAdjustLine ) {
		DWORD	dlnT = (DWORD)dln;

		if ( !dlnT ) {
			DWORD	dlnWin = WinHeight();

			// If the window is zero height, set dlnWin
			// to 3 so the dlnWin/3 below yields 1, which
			// then gets subtracted out on the SetCursorAt
			// call
			if ( dlnWin < 3 ) {
				dlnWin = 3;
			}
			dlnT = m_dmi.lnMapStart + dlnWin / 3;
		}
		SetCursorAt( 0, dlnT - 1 );
	}

    // allow updates again.
    SendMessage( WM_SETREDRAW, 1, 0L);
    return fReturn;
}

/*** DAMSetMapFromAddr
*
* Purpose:  Given an address setup the source window map.  Setup includes
*           map region invalidation as well as initial filling in the
*           initial entry into the map in cases where the entire map
*           is invalid.
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
BOOL CDAMView :: SetMapFromAddr (
PADDR paddr,
BOOL fAdjustLine
) {

    CXT     cxt = {0};
    HSF     hsf;
    WORD    wLn = 0;
    WORD    dln = 0;

    if (
        SHSetCxt ( paddr, &cxt ) &&
        ( hsf = SLHsfFromPcxt ( &cxt ) ) &&
        SLLineFromAddr ( paddr, &wLn, NULL, NULL )
    ) {

        if ( DAMFLoadHSFFile( hsf ) ) {

            // For MIXED mode, don't adjust the cursor, since it may
            // put the desired address off of the window!
            if ( fAdjustLine && !m_dmi.fAnnotate ) {
                WORD    wLnAdjust = (WORD)WinHeight() / 3;

                if ( wLn > wLnAdjust ) {
                    wLn -= wLnAdjust;
                }
                else {
                    dln = wLn;
                    wLn = 1;
                }
            }
        }
        else {
            wLn = 0;
        }
    }

    return SetMap ( (HSF)NULL, wLn, paddr, fAdjustLine, dln );
}

MET CDAMView :: RangeFromPdml ( DML *pdml, PADDR paddrStart, PADDR paddrEnd ) {
    MET   met = pdml->met;
    SHOFF cbLn;

    switch ( pdml->met ) {

		// This will only return metSource if the dml actually
		// is the line in the CV info which generated the code.
		// (The last line of the source file statement)
        case metSource: {
			ADDR	addr;

            if (
                pdml->mfl.hsf != NULL &&
                SLFLineToAddr (
                    pdml->mfl.hsf,
                    pdml->mfl.wLine,
                    &addr,
                    &cbLn,
                    NULL
                )
            ) {
				*paddrEnd = *paddrStart = pdml->addr;
                SetAddrOff ( paddrEnd , GetAddrOff ( *paddrStart ) + cbLn );
            }
            else {
                met = metNone;
            }
            break;
		}

        case metDisasm:
        case metLabel:
		case metFileName:
            *paddrStart = *paddrEnd = pdml->addr;
            SetAddrOff ( paddrEnd, GetAddrOff ( *paddrStart ) );
            break;

        default:
            break;
    }

    return met;
}


BOOL CDAMView :: PreDraw ( int dln, BOOL fCurSen ) {
    int     dlnHeight = WinHeight() + 1;
    DWORD   lnWinTop = OlnGetTop();
	int		dShift = 0;

	// Adjust the dln so that we don't go beyond a boundary
    if ( dln < 0 ) {
        if ( lnWinTop ) {
            dln = - ( (int) min ( (DWORD) abs(dln), lnWinTop) );
        }
        else {
			// This forces a return of FALSE.  Don't allow
			// scrolling beyond beginning of window
            dln = 0;
        }
    }
    else if ( dln > 0 ) {
        dln = (int) min ( (DWORD) dln, (lnWinTop + dlnHeight));

		// Make sure that we don't go off of the end
		dln = (int)min( (DWORD)dln, ( m_dmi.lnEnd - lnWinTop ) );
    }

	// Going backwards?  Check map for new top line
    if ( dln < 0 ) {
		int		idmlTop = (int)( lnWinTop - m_dmi.lnMapStart );

		// Can the new top line fit in the map as is?
		if ( idmlTop + dln < 0 ) {

			// No, adjust the map so that the new top line is now the top
			dShift = idmlTop + dln;
		}
    }

	// Going fowards?  Check map and see if new bottom fits
    else if ( dln > 0 ) {
		DWORD	lnBottom = lnWinTop + (DWORD)dlnHeight;

		// If we move down dln lines, can the last line fit in the rgdml?
		if ( (int)( lnBottom - m_dmi.lnMapStart ) + dln >= m_dmi.cdml ) {

			// Adjust the map so that the bottom line is now the top
			dShift = (int)( lnWinTop - m_dmi.lnMapStart ) + dln - 1;

			// if we're at the top of the map and scrolling 1 line, the
			// dShift value will be zero.  Since we know that we're scrolling
			// 1 line, force the shift
			if ( !dShift ) {
				++dShift;
			}
        }
    }

	// If dShift is non-zero, then the map needs to be adjusted
	if ( dShift ) {
		PDML	pdml;

		// Don't show updates while updating the map
		SendMessage( WM_SETREDRAW, 0, 0L );

		ShiftMap( dShift );

		// Adjust the "lnEnd"
		ASSERT( m_dmi.idmlInvalid );
		pdml = &m_dmi.rgdml[ m_dmi.idmlInvalid - 1 ];

		// If the new map start is beyond the "end" of the map, adjust the
		// starting map line number so that the last valid line is really
		// the last line!
		if (pDebugCurr == NULL || !pDebugCurr->IsJava ())
		{
			m_dmi.lnEnd = lnFudge ( SegmentLength ( m_dmi.rgdml[ 0 ].addr ) ) + 1;
		}
		if ( m_dmi.lnEnd <= m_dmi.lnMapStart ) {
			m_dmi.lnMapStart = m_dmi.lnEnd - m_dmi.idmlInvalid;
		}

		// Adjust the window to match the new map
        ShiftTopLine ( m_dmi.lnMapStart );

    	// Allow updates again.
	    SendMessage ( WM_SETREDRAW, 1, 0L);
	}

    return (BOOL)( dShift != 0 );
}
