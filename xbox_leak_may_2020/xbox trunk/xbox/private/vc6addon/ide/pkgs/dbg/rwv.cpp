/*** RWV.C - Register Window Handler - Varialbe Width
*
*   Copyright (C)1993, Microsoft Corporation
*
*   Purpose: To display ad update the register window
*
*   Revision History:
*        Written by: Mark A. Brodsky
*
*   Notes:
*
*************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// simple class to switch numeric locales temporarily
#include "locale.h"
class EnglishLocale
{
public:
	EnglishLocale()
	{
		setlocale( LC_NUMERIC, "C" );
	}

	~EnglishLocale()
	{
		setlocale( LC_NUMERIC, "" );
	}
};

void CCpuView :: InitializeMembers() {
	m_hlliRuiUndo = (HLLI)NULL;
	m_hlliRuiRedo = (HLLI)NULL;
	m_hlliRwi = (HLLI)NULL;
	m_hrwiEdit = hrwiNull;
	m_fShowCpu = runDebugParams.fCPURegs;
	m_fShowFlags = runDebugParams.fCPUFlags;
	m_fShowEA = runDebugParams.fCPUEA;
	m_fShowFPU = runDebugParams.fCPUFloat;
	m_obMax = 0;
	m_obMacPrev = 0;
	m_wEM = 0xffff;
	m_wModel = 0xffff;
	m_cEA = 0;
	m_ihrwiEA = (DWORD)-1;

#if defined (_X86_)
	m_cchLongDouble = 0;
#endif	// _X85_

	m_f386Mode = 'a';	// A bogus junk value
}

#ifdef _DEBUG
#undef new
#endif

IMPLEMENT_DYNCREATE( CCpuView, CMultiEdit )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT CCpuView::g_cRef = 0;
CFontInfo *CCpuView::g_pFontInfo = NULL;

CCpuView :: CCpuView() {
	InitializeMembers();

	if (g_cRef++ == 0)
	{
		g_pFontInfo = new CFontInfo(*(pfmtcatFromDocType((DOCTYPE)CPU_WIN)->pLogFont));
	}

	m_pFontInfo = g_pFontInfo;
    m_dt        = CPU_WIN;

    AddThisToCMEList(this);
}

CCpuView :: ~CCpuView() {
	DestroyInfo();

	if (--g_cRef == 0)
	{
		delete g_pFontInfo;
		g_pFontInfo = NULL;
	}
}

/* spacing for equals sign */
const char	szEqual[] = " = ";
#define cchEqual (sizeof(szEqual)/sizeof(szEqual[0])-1)

size_t CCpuView :: CchFloatFromRwi( LPRWI lprwi ) {
	ASSERT( lprwi->rui.rwf == rwfFloat );

    if ( lprwi->cbit == 32 ) {
        return 13;
    }
    else  if ( lprwi->cbit == 64 ) {
        return 22;
    } else {
#if defined (_X86_)
	if ( !m_cchLongDouble ) {
		FLOAT10	f10 = {0};
		char	sz[ axMax ];

		// Calculate the display width of a FLOAT10.  This doesn't need
		// to be done here, but this will reduce the number of times
		// this gets set

		SzFromLd( sz, sizeof( sz ), f10 );
		m_cchLongDouble = _ftcslen( sz );
	}
	return m_cchLongDouble;
#else // _X86_
	return 0;
#endif	// _X86_
	}
}

BOOL CCpuView :: FIsVisible( HRWI hrwi ) {
	BOOL	fRet;
	LPRWI	lprwi;

	ASSERT( hrwi );

	lprwi = (LPRWI)LLLpvFromHlle( hrwi );

	if(lprwi->rui.ut.rt & rtInvisible) {
		fRet = FALSE;
	} else switch( lprwi->rui.rwf ) {
		case rwfEffAddr:
		case rwfEffAddrOnly:
		case rwfEffData:
			fRet = m_fShowEA;
			break;

		case rwfFloat:
		case rwfHexReg:
			if ( lprwi->rui.ut.rt & rtFPU ) {
				fRet = m_fShowFPU;
			}
			else {
				fRet = m_fShowCpu;
			}
			break;

		case rwfFlag:
			fRet = m_fShowFlags;
			break;

		default:
			fRet = FALSE;
			break;
	}

	UnlockHlle( hrwi );

	return fRet;
}


void CCpuView :: UpdateDocInfo() {
	// Undo list isn't empty
	if ( m_hlliRuiUndo && LLChlleInLl( m_hlliRuiUndo ) ) {

		// If redo list isn't empty, playCount = 1 enables
		// both, == 0 disables REDO
		if ( m_hlliRuiRedo && LLChlleInLl(m_hlliRuiRedo) )
        {
			m_mePlayCount = 1;
		}
		else
        {
			m_mePlayCount = 0;
		}
		m_undoBuffer = ubUndo;
		m_undoState = usNormal;
	}

	// No undo, just redo, playcount < = disables UNDO, enables REDO
	else if ( m_hlliRuiRedo && LLChlleInLl( m_hlliRuiRedo ) )
    {
		m_mePlayCount = -1;
		m_undoBuffer = ubUndo;
		m_undoState = usNormal;
	}

	// Default, nothing to undo or redo
	else
    {
		m_mePlayCount = -1;
		m_undoState = usSuspendedError;
	}
}

void CCpuView :: SaveForUndo( LPRUI	lpruiSet ) {

	// Make sure that there's an undo list available
    if ( !m_hlliRuiUndo ) {
        m_hlliRuiUndo = LLHlliInit( sizeof( RUI ), llfNull, NULL, NULL );
    }

    // If we're allowed to undo this change and there's a list
    // add change to list...

    if ( m_hlliRuiUndo ) {
        HRUI    hrui;

        if ( hrui = LLHlleCreate( m_hlliRuiUndo ) ) {
            LPRUI   lprui = (LPRUI)LLLpvFromHlle( hrui );

            *lprui = *lpruiSet;

			// The cached value may already be changed,
			// so get the real value from OSDEBUG
			switch( lprui->rwf ) {
				case rwfFloat:
				case rwfHexReg:
					OSDReadRegister(
						hpidCurr,
						htidCurr,
						lprui->hReg,
						lprui->ur.rgb
					);
					break;

				case rwfFlag:
					OSDReadFlag(
						hpidCurr,
						htidCurr,
						lprui->iFlag,
						&lprui->ur.ul
					);
					break;

				case rwfEffData:
				case rwfEffAddr:
					DHGetDebuggeeBytes(
						lprui->addrEA,
						lprui->cbEA,
						&lprui->ur.rgb
					);
					FlipBytes(
						(BYTE *)&lprui->ur.rgb,
						lprui->cbEA
					);
					break;

			}

            UnlockHlle( hrui );
            LLAddHlleToLl( m_hlliRuiUndo, hrui );

			// New edits destroy the Redo list
    		if ( m_hlliRuiRedo ) {
		        LLChlleDestroyLl( m_hlliRuiRedo );
		        m_hlliRuiRedo = (HLLI)NULL;
			}

			UpdateDocInfo();
        }
    }
}

void CCpuView :: WriteReg(
LPRUI		lprui,
LPUR		lpur) {
	ADDR		addrPCCur;
	ADDR		addrPCNew;
	WORD		UpdateFlags = UPDATE_ALLDBGWIN;

	// Save PC before register change
	SYGetAddr( hpidCurr, htidCurr, adrPC, (LPADDR)&addrPCCur ) ;

	// Here we have to get to the child directly to make the change
    switch( lprui->rwf ) {
    	case rwfEffAddr:
	    case rwfEffData: {
			BYTE	rgb[ cbRegMax ];

			ASSERT( lprui->cbEA <= cbRegMax );
			memcpy( rgb, lpur->rgb, lprui->cbEA );
			FlipBytes( rgb, lprui->cbEA );
			DHPutDebuggeeBytes(
				lprui->addrEA,
				lprui->cbEA,
				(char FAR *)rgb
			);
            break;
		}

		case rwfFloat:
			OSDWriteRegister(
				hpidCurr,
				htidCurr,
				lprui->hReg,
				lpur->rgb
			);
	        break;

	    case rwfHexReg:
			OSDWriteRegister(
				hpidCurr,
				htidCurr,
				lprui->hReg,
				&lpur->ul
			);
	        break;

        case rwfFlag:
        	OSDWriteFlag(
    	        hpidCurr,
	            htidCurr,
                lprui->iFlag,
        	    (VOID FAR *)&lpur->ul
    	    );
	        break;
    }

	// Get new PC
	SYGetAddr( hpidCurr, htidCurr, adrPC, &addrPCNew );
	if ( !FAddrsEq( addrPCCur, addrPCNew ) ) {
		// Need to update source window
		UpdateFlags |= UPDATE_SOURCE;

		// Make sure that the cxfIp is also updated
		set_addrs();
	}

	// Update all of the windows since the edit may have altered data
	UpdateDebuggerState( UpdateFlags );
}

void CCpuView :: FlushEdit( BOOL fThrowOut ) {
	if ( m_hrwiEdit ) {
		if ( fThrowOut ) {
			LPRWI	lprwi = (LPRWI)LLLpvFromHlle( m_hrwiEdit );

			if ( lprwi->rui.rwf == rwfEffAddr ||
				lprwi->rui.rwf == rwfEffData
			) {
				DHGetDebuggeeBytes(
					lprwi->rui.addrEA,
					lprwi->rui.cbEA,
					lprwi->rui.ur.rgb
				);

				FlipBytes( (BYTE *)lprwi->rui.ur.rgb, lprwi->rui.cbEA );
			}
			else {
	            OSDReadRegister(
					hpidCurr,
					htidCurr,
					lprwi->rui.hReg,
					lprwi->rui.ur.rgb
				);
			}

			if ( FIsVisible( m_hrwiEdit ) ) {
				DrawLine( lprwi->oln );
			}

			UnlockHlle( m_hrwiEdit );
			m_hrwiEdit = hrwiNull;
		}
		else {
			RUI	rui = ((LPRWI)LLLpvFromHlle( m_hrwiEdit ))->rui;

			UnlockHlle( m_hrwiEdit );
			m_hrwiEdit = hrwiNull;

			SaveForUndo( &rui );

			WriteReg( &rui, &rui.ur );
		}
	}
}


/**** NextField                                                         ****
 *                                                                         *
 *  PURPOSE: Move the cursor to the next field in the window               *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: NextField() {

    DWORD   oln = OlnGetCurPos();
    WORD    ob = ObGetCurPos();
    HRWI    hrwi = hrwiNull;
    BOOL    fFound = FALSE;
    BOOL    fForce = FALSE;

    while( !fFound && ( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) ) {
		if ( FIsVisible( hrwi ) ) {
	        LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

    	    // If we've gone beyond the end of the data which is on the line
        	// of the cursor, change the current position so that the current
	        // item will be selected.
    	    if ( oln < lprwi->oln ) {
        	    oln = lprwi->oln;
            	fForce = TRUE;
	        }

    	    // Don't allow cursor to be set on EffAddrOnly field since there is
        	// no editable data
	        if ( lprwi->rui.rwf != rwfEffAddrOnly &&
    	        oln == lprwi->oln &&
	            ( fForce || ob < lprwi->obValMin ) ) {

    	        SetCursorAt( lprwi->obValMin, lprwi->oln );
        	    fFound = TRUE;
	        }

    	    UnlockHlle( hrwi );
		}
    }

    // If the next item hasn't been found, then we've reached the bottom.
    // Just get the first item in the list and put the cursor there
    if ( !fFound ) {
        ASSERT( !hrwi );

        while( !fFound && ( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) ) {
			if ( FIsVisible( hrwi ) ) {
	        	LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

		        SetCursorAt( lprwi->obValMin, lprwi->oln );
    		    UnlockHlle( hrwi );
				fFound = TRUE;
			}
		}
    }
}


/**** PrevField                                                         ****
 *                                                                         *
 *  PURPOSE: Move the cursor to the previous field in the window           *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: PrevField() {
    DWORD   oln = OlnGetCurPos();
    WORD    ob = ObGetCurPos();
    HRWI    hrwi = LLHlleGetLast( m_hlliRwi );
    BOOL    fFound = FALSE;

    // When the list was created, we added a prev pointer so this would
    // be much easier!

    do {
        LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

		if ( FIsVisible( hrwi ) ) {

			// Set to the previous line if no prev entry on current
			// line.  Set ob to end of line so we can find the
			// last entry on the new line
    	    if ( oln > lprwi->oln ) {
        	    oln = lprwi->oln;
            	ob = (WORD)-1;
	        }

    	    // Don't allow cursor to move to the EffAddrOnly field since there
        	// is no editable data there.
	        if ( lprwi->rui.rwf != rwfEffAddrOnly &&
    	        oln == lprwi->oln &&
	            ob >= lprwi->obValMac ) {

    	        SetCursorAt( lprwi->obValMin, lprwi->oln );
        	    fFound = TRUE;
			}
		}
       	UnlockHlle( hrwi );
        hrwi = lprwi->hrwiPrev;

    } while( !fFound && hrwi );

    // If not found, we're trying to go past the top, just get the last item
    // (always valid) and put the cursor there
    if ( !fFound ) {
		HRWI	hrwiLast = hrwiNull;
        ASSERT( !hrwi );

		while( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) {
			if ( FIsVisible( hrwi ) ) {
				hrwiLast = hrwi;
			}
		}

		if ( hrwiLast ) {
	        LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwiLast );

        	SetCursorAt( lprwi->obValMin, lprwi->oln );
	        UnlockHlle( hrwiLast );
		}
    }
}


/**** SetReg                                                            ****
 *                                                                         *
 *  PURPOSE: Change/set a register, flag, or PCode stack item.             *
 *                                                                         *
 *  INPUTS:                                                                *
 *      lpur    Union containing long/rgb[10] to change data to            *
 *      fUndo   Flag indicating if SetReg is being called from UNDO.       *
 *              If in undo, then don't save change.                        *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: SetReg(
LPUR		lpur,
HRUI        hruiSet,
BOOL        fUndo ) {
    LPRUI   lpruiSet = (LPRUI)LLLpvFromHlle( hruiSet );

    // Unlock the ruiSet since update destroys the hrwi if it's an EA thing!
    UnlockHlle( hruiSet );

	// If we need to save the change so we can undo....
	// rwfHexReg will be saved un FlushEdit
	if ( !fUndo ) {
	    // Here we have to get to the child directly to make the change
		memcpy( lpruiSet->ur.rgb, lpur->rgb, cbRegMax );
		m_hrwiEdit = (HRWI)hruiSet;

		// When not undo, the HRUISET is really an HRWI!
		DrawLine( ((LPRWI)lpruiSet)->oln );
	}
 	else {
		WriteReg( lpruiSet, lpur );
	}
}


/**** UndoValue                                                         ****
 *                                                                         *
 *  PURPOSE: Undo the user's last change and position cursor.              *
 *                                                                         *
 *  INPUTS:                                                                *
 *      fRedo   TRUE = redo, FALSE = undo                                  *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: UndoValue(
BOOL	fRedo ) {
	HLLI	hlliFrom;
	HLLI *	phlliTo;

	if ( fRedo ) {
		hlliFrom = m_hlliRuiRedo;
		phlliTo = &m_hlliRuiUndo;
	}
	else {
		hlliFrom = m_hlliRuiUndo;
		phlliTo = &m_hlliRuiRedo;
	}

    // Make sure that there's something to undo!
    if ( hlliFrom ) {
        HRUI    hrui = LLHlleGetLast( hlliFrom );

        if ( hrui ) {
            LPRUI   lprui = (LPRUI)LLLpvFromHlle( hrui );
            BOOL    fFound = FALSE;
            HRWI    hrwi = hrwiNull;
            WORD    ob;
            DWORD   oln;
			UR		ur;
			BOOL	fVisible;

            ASSERT( m_hlliRwi );

            // Also, make sure that we CAN undo.  If the register
            // set is changed (ie options.native...), we won't allow
            // undos from the old register set.

            while( !fFound && ( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) ) {
                LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

                if ( lprwi->rui.hReg == lprui->hReg &&
                    lprwi->rui.rwf == lprui->rwf &&
                    lprwi->rui.iFlag == lprui->iFlag ) {

					// If the undo item is an EA item, make sure that
					// we have a match
					if ( lprui->rwf != rwfEffAddr &&
						lprui->rwf != rwfEffData ||
						lprwi->cbit &&
						FAddrsEq( lprwi->rui.addrEA, lprui->addrEA )
					) {
	                    fFound = TRUE;
	                    ob = lprwi->obValMin;
	                    oln = lprwi->oln;
						ur = lprwi->rui.ur;
						fVisible = FIsVisible( hrwi );
					}
                }
                UnlockHlle( hrwi );
            }

            if ( fFound ) {
	            SetReg( &lprui->ur, hrui, TRUE );
				if ( fVisible ) {
                	SetCursorAt( ob, oln );
				}

				// Save value which was just overridden so the reverse
				// operation has the correct data
				lprui->ur = ur;
            }
            UnlockHlle( hrui );

			if ( !*phlliTo ) {
				*phlliTo = LLHlliInit( sizeof( RUI ), llfNull, NULL, NULL );
			}

			if ( *phlliTo ) {
				// Just move the node fro, one list to another
				LLFRemoveHlleFromLl( hlliFrom, hrui );
				LLAddHlleToLl( *phlliTo, hrui );
			}
			else {
	            // Remove the undone value from the list.
            	LLFDeleteHlleFromLl( hlliFrom, hrui );
			}
        }
        else {
            MessageBeep( 0 );
        }
    }
    else {
        MessageBeep( 0 );
    }

	UpdateDocInfo();
}


/**** RwfFieldType                                                      ****
 *                                                                         *
 *  PURPOSE: Determine the field type which the cursor is on top of        *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *      *phrwi  sets phrwi to the handle for the register struct           *
 *                                                                         *
 *      Return Value: Type of field                                        *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
RWF CCpuView :: RwfFieldType(
HRWI *  phrwi ) {
    RWF     rwf = rwfNone;

	if ( m_hlliRwi ) {
    	DWORD   oln = OlnGetCurPos();
	    WORD    ob = ObGetCurPos();
	    HRWI    hrwi = hrwiNull;
    	BOOL    fFound = FALSE;

	    // Search for the rwi which is at the current location and if cursor
    	// is inside of its editable data area, return its rwf

	    while( !fFound && ( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) ) {
    	    LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

        	if ( oln == lprwi->oln ) {
				if ( lprwi->obValMin <= ob && ob < lprwi->obValMac ) {
	        	    fFound = TRUE;
    	        	*phrwi = hrwi;
	    	        rwf = lprwi->rui.rwf;
				}

				// Text part of a value
				else if ( lprwi->obSzMin <= ob && ob < lprwi->obValMin ) {
	        	    fFound = TRUE;
    	        	*phrwi = hrwi;
	    	        rwf = rwfText;
				}
    	    }
        	UnlockHlle( hrwi );
	    }
	}

    return rwf;
}


/**** ToggleFlag                                                        ****
 *                                                                         *
 *  PURPOSE: Flip the flag that the cursor is sitting on.                  *
 *                                                                         *
 *  INPUTS:                                                                *
 *      hrwi    handle of register(flag) to flip                           *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: ToggleFlag(
HRWI    hrwi ) {
    LPRWI   lprwi;
	UR		ur = {0};

    ASSERT( hrwi );
    lprwi = (LPRWI)LLLpvFromHlle( hrwi );
    ASSERT( lprwi->rui.rwf == rwfFlag );

    ur.ul = !lprwi->rui.ur.ul;

    UnlockHlle( hrwi );

    SetReg( &ur, (HRUI)hrwi, FALSE );
}

/**** CbGetLineBuf                                                      ****
 *                                                                         *
 *  PURPOSE: Get a specific display line and return to edit manager        *
 *                                                                         *
 *  INPUTS:                                                                *
 *      oln     Line number requested                                      *
 *      cbMax   Size of destination buffer                                 *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *      szBuf   Data space to put display line                             *
 *                                                                         *
 *      Return Value: _ftcslen( szBuf )                                    *
 *                                                                         *
 *  IMPLEMENTATION: Format appropriate data and create m_rgla which will   *
 *                  be returned to edit manager in GetLineAttrs()          *
 *                                                                         *
 ***************************************************************************/
WORD CCpuView :: CbGetLineBuf(
DWORD   oln,
WORD    cbMax,
char *  szBuf,
PDLA &	pdla
) {
	*szBuf = '\0';

	if ( m_hlliRwi ) {
		char *		pszFmt;
		char *		pszFmtNibble;
		HRWI        hrwi = hrwiNull;
		char *      psz = szBuf;
		int			cReg = (int)LLChlleInLl( m_hlliRwi );
		BOOL		fDone = FALSE;
	 	FMT_ELEMENT *rgfmtel = pfmtelFromDocType((DOCTYPE)CPU_WIN);
		FMT_ELEMENT *fmtelValue = &(rgfmtel[FMTEL_TEXT]);
		FMT_ELEMENT *fmtelHilight = &(rgfmtel[FMTEL_RGHI]);

		pdla = m_rgdla;
		pszFmt = "%02X";
		pszFmtNibble = "%01X";

		// Loop through the rwi's in the window and pick off
		// what should be on this line
		while( !fDone && ( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) ) {
			LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

			// If the item is hidden (not selected), don't bother
			if ( FIsVisible( hrwi ) ) {
				// Include the item if it belongs on this line
				// and won't cause a buffer overflow
				if ( oln == lprwi->oln && lprwi->obValMac < cbMax ) {
					BOOL    fChanged = FALSE;
					RWF		rwf = lprwi->rui.rwf;

					// Check for differences for everything except
					// EA addr string only or EAs with invalid addresses
					if (
						rwf == rwfEffAddr && lprwi->cbit ||
						rwf == rwfEffData && lprwi->cbit ||
						rwf != rwfEffAddr &&
						rwf != rwfEffAddrOnly &&
						rwf != rwfEffData
					) {
						fChanged = memcmp(
							&lprwi->rui.ur,
							&lprwi->urDiff,
							lprwi->cbit / 8
						);
					}

					// Flags treated specially (no spaces between flagname
					// and value)
					if ( rwf == rwfFlag ) {
						WORD	cch = sprintf( psz, " %Fs=", lprwi->lsz );

						psz += cch;
						pdla->cb = cch;
						pdla->pElement = fmtelValue;

						psz += sprintf( psz, pszFmtNibble, lprwi->rui.ur.ul );
						if ( fChanged ) {
							++pdla;
							pdla->cb = 1;
							pdla->pElement = fmtelHilight;
						}
						else {
							++pdla->cb;
						}
						++pdla;
					}

					// Floating point value.  Display as +x.yyyyy+eeee
					else if ( rwf == rwfFloat ) {
						char *	pszBefore = psz;
						size_t	cch;
						size_t	cchFloat;

						psz += sprintf(
							psz,
							" %Fs%s",
							lprwi->lsz,
							szEqual
						);	
						pdla->cb = _ftcslen( pszBefore );
						pdla->pElement = fmtelValue;

						cchFloat = CchFloatFromRwi( lprwi );

                        if (lprwi->cbit == 32) {
                            sprintf(
	                            psz,
	                            "%+.5E",
	                            *( UNALIGNED float * ) lprwi->rui.ur.rgb
                            );
                        } else if (lprwi->cbit == 64) {
                            sprintf(
	                            psz,
	                            "%+.14LE",
	                            *( UNALIGNED double * ) lprwi->rui.ur.rgb
                            );
                        } else if (lprwi->cbit == 80) {
							SzFromLd(
								psz,
								cchFloat + 1,
								*(FLOAT10 *)lprwi->rui.ur.rgb
							);
                        } else {
                            ASSERT(FALSE);
                        }

						cch = strlen( psz );
                        if (cch < cchFloat) {
                            memset( psz + cch, ' ', cchFloat - cch );
                        }
						psz += cchFloat;
						if ( fChanged ) {
							++pdla;
							pdla->pElement = fmtelHilight;
							pdla->cb = cchFloat;
						}
						else {
							pdla->cb += cchFloat;
						}

						++pdla;
					}

					// Hexadecimal value.  Handle all cases of power of 2 bytes
					// up to cbRegMax
					else {
						WORD        cb = lprwi->cbit / 8;
						BYTE FAR *  lpb = (BYTE FAR *)&lprwi->rui.ur.rgb + cb - 1;

						// EffData does not have the string on the same line,
						// so, don't include it
						if ( rwf != rwfEffData ) {
							int	cch;

							cch = sprintf(
								psz,
								" %Fs",
								lprwi->lsz
							);

							psz += cch;
							pdla->cb = cch;
						}
						else {
							*psz++ = ' ';
							pdla->cb = 1;
						}
						pdla->pElement = fmtelValue;

						// EffAddrOnly and EffData do
						// not have both text and value,
						// so no need to include the '='
						// seperator
						if ( rwf != rwfEffAddrOnly &&
							rwf != rwfEffData
						) {

							_ftcscat( psz, szEqual );
							psz += cchEqual;
							pdla->cb += cchEqual;
						}
						++pdla;

						// effAddrOnly has no value, just text
						if ( lprwi->rui.rwf != rwfEffAddrOnly ) {
							WORD	cbT = cb << 1;

							// This case will cover everything
							// except EAs with invalid addresses
							if ( cb ) {
								while( cb-- ) {
								    psz += sprintf(
								        psz,
								        pszFmt,
								        *lpb--
								    );
								}
							}
							else {
								// EA with invalid address.  Fill
								// with corresponsing number of '?'
								// to coinside with the number of
								// hex digits to display the value
								// if there was one
								cbT = lprwi->rui.cbEA << 1;
								memset( psz, '?', cbT );
								psz += cbT;
							}

							if ( fChanged ) {
								pdla->pElement = fmtelHilight;
								pdla->cb = cbT;
							}
							else {
								--pdla;
								pdla->cb += cbT;
							}
							++pdla;
						}
					}
				}
				
				// If the node's line number is greater than the
				// requested line, then no need to go on since
				// the nodes are increasing in line number
				else if ( lprwi->oln > oln ) {
					fDone = TRUE;
				}
			}
			pdla->pElement = fmtelValue;
			pdla->cb = 0xffff;

			*psz = '\0';
			UnlockHlle( hrwi );
		}
	}

	pdla = m_rgdla;
	return _ftcslen( szBuf );
}


/**** ObMaxBuf                                                          ****
 *                                                                         *
 *  PURPOSE: Get the max number of characters in the buffer                *
 *                                                                         *
 *  INPUTS:                                                                *
 *      hBuf    Window Id of parent window                                 *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: Number of characters (max) for the register window   *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
WORD CCpuView :: ObMaxBuf() {
    return m_obMax;
}


/**** CLinesInBuf                                                       ****
 *                                                                         *
 *  PURPOSE: Get the number of lines in the buffer                         *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: Number of lines in the register window               *
 *                                                                         *
 *  IMPLEMENTATION: The last item in the rwi list will have a cooridnate   *
 *  oln for the last line in the window.  Add 1 to it so it becomes a count*
 *  instead of an index                                                    *
 *                                                                         *
 ***************************************************************************/
DWORD CCpuView :: CLinesInBuf() {
    DWORD   dwRet = 0;

    if ( m_hlliRwi ) {
		HRWI	hrwi = hrwiNull;

		while( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) {
			if ( FIsVisible( hrwi ) ) {
		        LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

    		    dwRet = lprwi->oln + 1;

        		UnlockHlle( hrwi );
			}
		}
    }
    return dwRet;
}

BOOL CCpuView :: FDoKeyDown(
WPARAM	wParam,
LPARAM	lParam ) {
	BOOL	fUseEM = TRUE;

	switch( wParam ) {
		case VK_TAB:
			if ( m_hlliRwi ) {
				FlushEdit( FALSE );
    		    if ( HIWORD( lParam ) & KK_SHIFT ) {
				    PrevField();
	        	}
	    	    else {
    				NextField();
				}
	        }
			break;

		case VK_BACK:
		case VK_DELETE:
			MessageBeep( 0 );
			fUseEM = FALSE;
			break;

		case VK_LEFT:
		case VK_RIGHT:
			if ( m_hrwiEdit ) {
				HRWI	hrwi;
				WORD	ob = ObGetCurPos();
				WORD	obPrev = ob;

				// Set cursor in appropriate direction
				// since RwfFieldType uses current
				// cursor position to determine what
				// hrwi we really want.  Avoid sign
				// changes from incr and decr
				if ( wParam == VK_RIGHT ) {
					if ( ob + 1 < (WORD)-1 ) {
						++ob;
					}
				}
				else {
					if ( ob > 0 ) {
						--ob;
					}
				}

				// This does NOT update the UI
				SetCurObPos( ob );

				RwfFieldType( &hrwi );

				// Reset the cursor pos to the
				// original
				SetCurObPos( obPrev );

				if ( hrwi != m_hrwiEdit ) {
					FlushEdit( FALSE );
				}
			}
			break;

		case VK_ESCAPE:
			FlushEdit( TRUE );
			fUseEM = FALSE;
			break;

		case VK_UP:
		case VK_DOWN:
		case VK_END:
		case VK_HOME:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_RETURN:
			FlushEdit( FALSE );
			break;
	}

	return fUseEM;
}

/**** FDoChar                                                           ****
 *                                                                         *
 *  PURPOSE: Process character input for the register window.  This        *
 *           function will be called when a WM_CHAR is received by the     *
 *           Register Window WndProc.                                      *
 *                                                                         *
 *  INPUTS:                                                                *
 *      wParam  ASCII value of WM_CHAR (see CW docs)                       *
 *      lParam  Untranslated character info along with ctrl/shift/alt state*
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: TRUE if the edit manager is to get the message.      *
 *                    If FALSE is returned, the character input has been   *
 *                    'eaten' and should NOT be passed on the the edit mgr.*
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
BOOL CCpuView :: FDoChar(
WPARAM	wParam,
LPARAM	lParam ) {
	BOOL fUseEM = TRUE;
	char szSel[ 2 ];
	EnglishLocale SetTheLocaleToC;//This sets the functions locale to "C"

	// [dolphin#5001 12/30/93 mikemo]
	if ( wParam == TAB || wParam == CR ) {
		fUseEM = FALSE;
	}
	else if ( DebuggeeRunning() ||
		GetEditSelection( sizeof( szSel ), szSel ) ||
		!_istprint( wParam )
	) {
		MessageBeep( 0 );
		fUseEM = FALSE;
	}
	else {
		RWF     rwf;
		HRWI    hrwi;
		char *	szBuf = (char *)alloca( m_obMax + 1 );

		ASSERT( szBuf );

		fUseEM = FALSE;

		// Normal input here!
		switch( rwf = RwfFieldType( &hrwi ) ) {
			case rwfText:
			case rwfNone:
				fUseEM = FALSE;
				MessageBeep( 0 );
				break;

			case rwfFlag:
				if ( wParam == (WORD)(unsigned char)'0' ||
					wParam == (WORD)(unsigned char)'1'
				) {

					UR	ur = {0};
#ifdef _DEBUG
					LPRWI	lprwi = (LPRWI)LLLpvFromHlle( hrwi );

					ASSERT( lprwi->cbit == 1 );
					UnlockHlle( hrwi );
#endif	// _DEBUG

					ur.ul = (ULONG)( wParam == (WORD)(unsigned char)'1' );
					SetReg( &ur, (HRUI)hrwi, FALSE );

					CMultiEdit::LTextProc( WM_KEYDOWN, VK_RIGHT, 0L );
				}
				else if ( wParam == VK_SPACE ) {
					ToggleFlag( hrwi );
					FlushEdit( FALSE );
					fUseEM = FALSE;
				}
				else {
					MessageBeep( 0 );
				}
				break;

			case rwfEffAddr:
			case rwfEffData: {
				LPRWI	lprwi = (LPRWI)LLLpvFromHlle( hrwi );
				WORD	cbEA = lprwi->cbit * 8;

				UnlockHlle( hrwi );

				if ( cbEA == 0 ) {
					MessageBeep( 0 );
					break;
				}
			}
				// fall through

			case rwfHexReg: {
				LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

				if ( _istxdigit( wParam ) ) {
					WORD    ob = ObGetCurPos();
					WORD    obIns = ob;
					WORD    obMac;
					char *  pch;
					UR      ur = { 0 };
					char    rgch[ 3 ];
					BYTE *  pb = ur.rgb + lprwi->cbit / 8 - 1;
					PDLA	pdla;

					CbGetLineBuf( OlnGetCurPos(), m_obMax + 1, szBuf, pdla );

					*( szBuf + ob ) = LOBYTE( wParam );

					obMac = lprwi->obValMac;
					obIns = lprwi->obValMin;
					pch = szBuf + obIns;
					rgch[ 2 ] = '\0';

					while( obIns < obMac ) {
						WORD    wT;

						_ftcsncpy( rgch, pch, 2 );
						sscanf( rgch, "%hX", &wT );
						*pb = LOBYTE( wT );
						--pb;
						pch += 2;
						obIns += 2;
					}

					// Unlock here since SetReg will destroy
					// hrwi if it's an EA thing!
					UnlockHlle( hrwi );
					SetReg( &ur, (HRUI)hrwi, FALSE );
					CMultiEdit::LTextProc( WM_KEYDOWN, VK_RIGHT, 0L );
				}
				else {
					UnlockHlle( hrwi );
					MessageBeep( 0 );
				}
				break;
			}

			case rwfFloat: {
				LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );
				BOOL    fOK = TRUE;
				WORD    ob = ObGetCurPos();
				char    ch;
				PDLA	pdla;

				CbGetLineBuf( OlnGetCurPos(), m_obMax + 1, szBuf, pdla );

				ch = *( szBuf + ob );

				if ( ch == '+' || ch == '-' ) {
					fOK =
						(BOOL)( (char)wParam == '+' || (char)wParam == '-' );
				}
				else if ( ch == 'E' || ch == 'e' ) {
					fOK =
						(BOOL)( (char)wParam == 'E' || (char)wParam == 'e' );
				}
				else if ( ch == '.' ) {
					fOK = (BOOL)( (char)wParam == '.' );
				}
				else if ( !_istdigit( wParam ) ) {
					fOK = FALSE;
				}

				if ( fOK ) {
					WORD    obIns;
					WORD    obMac;
					char *  pch;
					UR      ur = { 0 };

					*( szBuf + ob ) = LOBYTE( wParam );

                    obMac = lprwi->obValMac;
                    obIns = lprwi->obValMin;
                    pch = szBuf + obIns;
                    *( szBuf + obMac ) = '\0';

                    if (lprwi->cbit == 64) {
						sscanf( pch, "%le", (UNALIGNED double *)ur.rgb );
					}
					else if ( lprwi->cbit == 32 ) {
						sscanf( pch, "%e", (UNALIGNED float *)ur.rgb );
					}
					else if ( lprwi->cbit == 80 ) {
						char *	pchRet;
						*(FLOAT10 *)ur.rgb = LdFromSz( pch, &pchRet );
					} 
					else {
						ASSERT (FALSE);
					}

					// Unlock here since SetReg will destroy
					// hrwi if it's an EA thing!
					UnlockHlle( hrwi );

					SetReg( &ur, (HRUI)hrwi, FALSE );

					CMultiEdit::LTextProc( WM_KEYDOWN, VK_RIGHT, 0L );
				}
				else {
					MessageBeep( 0 );
				}
				break;
			}
		}
	}

	return fUseEM;
}


/**** Freeze                                                            ****
 *                                                                         *
 *  PURPOSE: Save the current state of the register window for showing     *
 *           differences between execution of the child.                   *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: Freeze() {

	FlushEdit( FALSE );
    if ( m_hlliRwi ) {
        WORD    creg = (WORD)LLChlleInLl( m_hlliRwi );
        HRWI    hrwi = hrwiNull;

        // Copy all of the old values
        while( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) {
	        LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

	        lprwi->urDiff = lprwi->rui.ur;
    	    UnlockHlle( hrwi );
		}
    }

    // We don't allow undo's across execs, so empty the list
    if ( m_hlliRuiUndo ) {
        LLChlleDestroyLl( m_hlliRuiUndo );
        m_hlliRuiUndo = (HLLI)NULL;
	}

	if ( m_hlliRuiRedo ) {
        LLChlleDestroyLl( m_hlliRuiRedo );
        m_hlliRuiRedo = (HLLI)NULL;
    }

	UpdateDocInfo();
}


/**** RecalcCoords                                                      ****
 *                                                                         *
 *  PURPOSE: Recalculate rwi cooridnates for variable width display        *
 *                                                                         *
 *  INPUTS:                                                                *
 *      fRealign    Process this message during WM_SIZE, allow items to    *
 *                  move.  Otherwise, just adding an ea into a blank line  *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *  NOTE: This implementation is directly tied to CbGetLineBuf.  Any       *
 *        changes here should be echoed there also.                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: RecalcCoords(
BOOL fRealign  ) {

    ASSERT( m_hWnd );

	m_obMax = 0;

    // Must have a list or rwis
    if ( m_hlliRwi ) {
        HRWI    hrwi = hrwiNull;
        WORD    obMac;
		DWORD	iReg = 0;
   	    WORD    ob = 1;
   		DWORD   oln = 0L;
		DWORD	dwGrpCur = (DWORD)rwfNone;

		if ( fRealign || !m_obMacPrev ) {
	        // Ensure that the data in the window has a valid width
        	obMac = max( min( WinWidth(), (WORD)axMax ), 1 );
			m_obMacPrev = obMac;
		}
		else {
			obMac = m_obMacPrev;
		}

        while( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) {
			if ( FIsVisible( hrwi ) ) {
			    LPRWI   lprwi;
				WORD    cch;
    	        WORD    cchDisp;
				BOOL	fInsertExtraLine = FALSE;

	            lprwi = (LPRWI)LLLpvFromHlle( hrwi );

    	        cch = _ftcslen( lprwi->lsz );

                // Room for the '='
				if ( lprwi->rui.rwf == rwfFlag ) {
					++cch;
				}
				else {
   	            	cch += cchEqual;
				}

				if ( lprwi->rui.rwf == rwfFloat ) {
					cchDisp = CchFloatFromRwi( lprwi );
				}
				else if ( lprwi->cbit ) {
    	            // The display bytes are number of nibbles.
   	    	        cchDisp = max( lprwi->cbit / 4, 1 );
				}
				// EA on invalid or unevaluateable address
				else {
					cchDisp = lprwi->rui.cbEA * 2;  // number of '?' to display
				}

				// We previously displayed the EA address and data on
				// different lines.  Merge them back together so we
				// can reset
				if ( lprwi->rui.rwf == rwfEffAddrOnly ) {
					HRWI	hrwiNext = LLHlleFindNext( m_hlliRwi, hrwi );
					LPRWI	lprwiNext;

					ASSERT( hrwiNext );
					lprwiNext = (LPRWI)LLLpvFromHlle( hrwiNext );
					*lprwi = *lprwiNext;
					lprwi->rui.rwf = rwfEffAddr;
					UnlockHlle( hrwiNext );
					LLFDeleteHlleFromLl( m_hlliRwi, hrwiNext );
					if ( lprwi->cbit ) {
	   	    	        cchDisp = lprwi->cbit / 4;
					}
					else {
						cchDisp = lprwi->rui.cbEA * 2;
					}
				}

				if ( dwGrpCur != (DWORD)rwfNone &&
					dwGrpCur != lprwi->dwGrp
				) {
					if ( ob == 1 ) {
						++oln;
					}
					else {
						oln += 2;
					}
					ob = 1;
				}
				dwGrpCur = lprwi->dwGrp;

        	    // If the item is an EffAddr (addr and data) AND it doesn't
	            // fit on the current line any longer, create a EffData and
    	        // make this the EffAddrOnly to have a two line display of the EA
        	    if ( lprwi->rui.rwf == rwfEffAddr ) {

					// Only allow EA splitting when there is only 1 EA.  Otherwise,
					// the display will get real ugly!
					if ( m_cEA == 1 && obMac && ob + cch + cchDisp > obMac ) {
        	            HRWI    hrwiNew = LLHlleCreate( m_hlliRwi );

            	        if ( hrwiNew ) {
                	        LPRWI   lprwiNew = (LPRWI)LLLpvFromHlle( hrwiNew );

                    	    *lprwiNew = *lprwi;
                        	lprwiNew->rui.rwf = rwfEffData;

	                        lprwi->rui.rwf = rwfEffAddrOnly;
	                        if ( ob ) {
        	                    ob = 1;
            	            }
                    	    lprwi->oln = oln++;
                	        lprwi->obSzMin = ob;
                        	lprwi->obValMac = lprwi->obValMin = ob + cch;
	                        lprwi->cbit = 0;
    	                    lprwiNew->hrwiPrev = hrwi;
        	                ob = 1;
            	            cch = 0;

							m_obMax = max( m_obMax, lprwi->obValMac );

                    	    UnlockHlle( hrwi );
                        	LLInsertHlleInLl( m_hlliRwi, hrwiNew, m_ihrwiEA + 1 );
	                        hrwi = hrwiNew;
    	                    lprwi = lprwiNew;
        	            }
            	    }
					else if ( ( fIn386Mode && obMac < 32 ) ||
						( !fIn386Mode && obMac < 16 )
					) {
						fInsertExtraLine = TRUE;
					}
            	}

	            // See if the item will fit on the current line.
    	        // If not, reset to the beginning of the next line
                // v-vadimp or it has to be on a new line
                if ( (ob > 1 && ob + cch + cchDisp > obMac) || (lprwi->rui.ut.rt & rtNewLine)) { 
            	    ob = 1;
                	++oln;
	            }

    	        // Beginning of the display string (ie "EAX")
        	    lprwi->obSzMin = ob;

               	// Nothing special here, just set the values min to the
                // beginning of the display string plus its length
   	            lprwi->obValMin = ob + cch;

       	        // The end is the beginning plus the data width
           	    lprwi->obValMac = lprwi->obValMin + cchDisp;

    	        lprwi->oln = oln;

				m_obMax = max( m_obMax, lprwi->obValMac );

				if ( fInsertExtraLine ) {
					ob = 1;
					++oln;
				}
				else {
        	    	// Place a space at the end.
	   	    	    ob = lprwi->obValMac + 1;
				}

            	UnlockHlle( hrwi );
			}
			++iReg;
        }
    }
}


/**** RwiUpdateEA                                                       ****
 *                                                                         *
 *  PURPOSE: Update the rwi list to include EA data if there's an EA       *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: RwiUpdateEA() {
    SDI	sdi;
	int	cEAPrev = m_cEA;

	SYGetAddr( hpidCurr, htidCurr, adrPC, &sdi.addr );
	sdi.dop = (DOP)( dopEA + dopHexUpper + dopUpper );
	SYUnassemble( hpidCurr, htidCurr, &sdi );

	m_cEA = 0;
	AddEANode( m_ihrwiEA + m_cEA, sdi.ichEA0, sdi.lpch, &sdi.addrEA0, sdi.cbEA0 );
	AddEANode( m_ihrwiEA + m_cEA, sdi.ichEA1, sdi.lpch, &sdi.addrEA1, sdi.cbEA1 );
	AddEANode( m_ihrwiEA + m_cEA, sdi.ichEA2, sdi.lpch, &sdi.addrEA2, sdi.cbEA2 );

	// All of the code below is to support displaying differences in the EA
	// across execution or editing
	if ( cEAPrev ) {
		int		ihrwi;
		HRWI	hrwi;
		HRWI	hrwiPrev;

		// Walk the list to find the previous entry
		ihrwi = m_ihrwiEA + 1;
		hrwi = hrwiNull;

		// Locate the beginning of the new EA nodes
		while( ihrwi-- ) {
			hrwiPrev = hrwi;
			hrwi = LLHlleFindNext( m_hlliRwi, hrwi );
		}

		// There is only updating if there are new EA nodes
		if ( m_cEA ) {
			HRWI	hrwiNew = hrwi;
			HRWI	hrwiOldBegin;
			int		iEA = m_cEA;

			// Skip over the new records
			while( iEA-- ) {
				hrwiOldBegin = hrwi;
				hrwi = LLHlleFindNext( m_hlliRwi, hrwi );
			}

			// Now try to copy "old" data to new records
			iEA = m_cEA;
			while( iEA-- && cEAPrev-- ) {
				LPRWI	lprwiNew = (LPRWI)LLLpvFromHlle( hrwiNew );
				LPRWI	lprwiOld = (LPRWI)LLLpvFromHlle( hrwi );

				if (lprwiOld == NULL || lprwiNew == NULL)
				{
					ASSERT(FALSE);
					break;
				}

				// If rwfEffAddr only, skip to next node to get to
				// the data node
				if ( lprwiOld->rui.rwf == rwfEffAddrOnly ) {
					UnlockHlle( hrwi );
					hrwi = LLHlleFindNext( m_hlliRwi, hrwi );
					lprwiOld = (LPRWI)LLLpvFromHlle( hrwi );
				}

				// This node must be a data node for the EA
				ASSERT(
					lprwiOld->rui.rwf == rwfEffAddr ||
					lprwiOld->rui.rwf == rwfEffData
				);

				// If both the old and new are non-errors and the addresses
				// match, then copy the diff data
				if ( lprwiOld->cbit && lprwiNew->cbit &&
					FAddrsEq( lprwiOld->rui.addrEA, lprwiNew->rui.addrEA )
				) {
					lprwiNew->urDiff = lprwiOld->urDiff;
				}

				// Advance to the next nodes
				UnlockHlle( hrwi );
				UnlockHlle( hrwiNew );
				hrwi = LLHlleFindNext( m_hlliRwi, hrwi );
				hrwiNew = LLHlleFindNext( m_hlliRwi, hrwiNew );
			}

			// Set up to remove
			hrwiPrev = hrwiOldBegin;
			hrwi = LLHlleFindNext( m_hlliRwi, hrwiPrev );
		}

		// Go through the list of old EA nodes and remove them from the
		// list.  All "important" data has been copied out of them.  The
		// EA nodes are consecutive, so when a non-EA node has been found
		// stop.
		while( hrwi ) {
			LPRWI	lprwi = (LPRWI)LLLpvFromHlle( hrwi );

			UnlockHlle( hrwi );

			if ( lprwi->rui.rwf == rwfEffAddr ||
				lprwi->rui.rwf == rwfEffAddrOnly ||
				lprwi->rui.rwf == rwfEffData
			) {
				LLFDeleteHlleFromLl( m_hlliRwi, hrwi );
				hrwi = LLHlleFindNext( m_hlliRwi, hrwiPrev );
			}
			else {
				// Reset the "previous" pointer since it
				// points to a deleted node
				lprwi = (LPRWI)LLLpvFromHlle( hrwi );
				lprwi->hrwiPrev = hrwiPrev;
				UnlockHlle( hrwi );

				hrwi = (HRWI)NULL;
			}
		}
	}
}


/**** AddEANode                                                         ****
 *                                                                         *
 *  PURPOSE: Add the EA data to the list of registers                      *
 *                                                                         *
 *  INPUTS:                                                                *
 *      ihrwiInsert index into rwi list to place node                      *
 *      ichEA   index into lpch string for nth EA.  -1 == no EA            *
 *      lpch    string containing one or more zero terminated EAs          *
 *      lpaddr  pointer to address for specific EA                         *
 *      cbEA    number of bytes for specific EA                            *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: AddEANode(
DWORD	ihrwiInsert,
int		ichEA,
LPCH	lpch,
LPADDR	lpaddr,
int		cbEA
) {
	if ( ichEA != -1 && cbEA != 0) {
		LSZ		lsz;
		LPCH	lpchEq;

		lpchEq = _ftcschr( lpch + ichEA, _T('=') );
		ASSERT( lpchEq );
		*lpchEq = '\0';

		if ( lsz = (LSZ)malloc( _ftcslen( lpch + ichEA ) + 1 ) ) {
	        HRWI    hrwi = LLHlleCreate( m_hlliRwi );
			HRWI	hrwiT;
			DWORD	ihrwi;

	        // Create a new EA item with rwf == rwfEffAddr.  This may
	        // be broken up in RecalcCoords if the window is too narrow

	        if ( hrwi ) {
	            LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );
				USHORT	cbRead;

				// Increment the number of EAs (for RecalcCoords)
				++m_cEA;

	            lprwi->lsz = lsz;
				_ftcscpy( lsz, lpch + ichEA );

	            lprwi->cbit = cbEA * 8;

	            ASSERT( cbEA <= cbRegMax );

	            // Save the EA data
	            cbRead = DHGetDebuggeeBytes(
	            	*lpaddr,
	            	cbEA,
	            	lprwi->rui.ur.rgb
	            );

				FlipBytes( (BYTE *)lprwi->rui.ur.rgb, cbEA );

				// Walk the list to find the previous entry
				ihrwi = ihrwiInsert;
				hrwiT = hrwiNull;
				while( ihrwi-- ) {
					hrwiT = LLHlleFindNext( m_hlliRwi, hrwiT );
				}

	            lprwi->hrwiPrev = hrwiT;
	            lprwi->rui.rwf = rwfEffAddr;
				lprwi->rui.ut.rt = rtInteger;
				lprwi->rui.addrEA = *lpaddr;
				lprwi->dwGrp = (DWORD)rwfEffAddr;

				if ( cbEA != cbRead ) {
					lprwi->cbit = 0;
				}
				cbEA <<= 1;

		        // cbEA from above is actually cchEA so...
		        // Note that hex 4567 is 4 characters, but is two bytes.
		        // Get it?!
				lprwi->rui.cbEA = cbEA >> 1;

				lprwi->urDiff = lprwi->rui.ur;

	            UnlockHlle( hrwi );

				LLInsertHlleInLl( m_hlliRwi, hrwi, ihrwiInsert );
				if ( hrwiT = LLHlleFindNext( m_hlliRwi, hrwi ) ) {
					lprwi = (LPRWI)LLLpvFromHlle( hrwiT );

					lprwi->hrwiPrev = hrwi;
					UnlockHlle( hrwiT );
				}
	        }
	        else {
	            errno = ENOMEM;
				free( lsz );
	        }
		}
		else {
			errno = ENOMEM;
		}
	}
}


/**** RwvKillRwiNode                                                    ****
 *                                                                         *
 *  PURPOSE: Callback from linklist manager to free up any allocated data  *
 *  in a RWI node.                                                         *
 *                                                                         *
 *  INPUTS:                                                                *
 *      lpvRwiNode   void pointer to RWI node.                             *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION: Only nodes with allocated memory are effective address *
 *      nodes which contain the EA string.                                 *
 *                                                                         *
 ***************************************************************************/
void FAR PASCAL RwvKillRwiNode( LPV lpvRwiNode ) {
	LPRWI	lprwi = (LPRWI)lpvRwiNode;

	if ( lprwi->rui.rwf == rwfEffAddr || lprwi->rui.rwf == rwfEffAddrOnly ) {
		ASSERT( lprwi->lsz );
		free( lprwi->lsz );
		lprwi->lsz = (LSZ)NULL;
	}
}


/**** UpdateRwi                                                         ****
 *                                                                         *
 *  PURPOSE: Reset the rwi list since we are either creating a new         *
 *  register set or changing EM/NMs.  This will get all of the info from   *
 *  OSDebug                                                                *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: UpdateRwi() {

    if ( m_hlliRwi ) {
        LLChlleDestroyLl( m_hlliRwi );
        m_hlliRwi = (HLLI)NULL;
    }

    m_hlliRwi = LLHlliInit(
    	sizeof( RWI ),
    	llfNull,
    	RwvKillRwiNode,
    	NULL
    );

    if ( m_hlliRwi ) {
        RD      rd;
        RT      rt = rtRegular;
        FD      fd;
        USHORT  ird = 0;
        HPID    hpid = hpidCurr;
		HTID	htid = htidCurr;
        HRWI    hrwiPrev = hrwiNull;

#if !(defined (_MIPS_))
        if ( fIn386Mode ) {
            rt = rtExtended;
        }
#else
        rt = rtRegular | rtExtended;
#endif
		// Regular CPU registers
        while( xosdNone == OSDGetRegDesc( hpid, htid, ird++, &rd ) ) {
            if ( rd.rt & rtCPU && rd.rt & rt ) {
                HRWI	hrwi = LLHlleCreate( m_hlliRwi );

                if ( hrwi ) {
                    LPRWI    lprwi = (LPRWI)LLLpvFromHlle( hrwi );

                    lprwi->lsz = rd.lszName;
                    lprwi->cbit = rd.dwcbits;
                    lprwi->rui.hReg = rd.dwId;
	  				lprwi->dwGrp = rd.dwGrp;

					OSDReadRegister(
						hpid,
						htid,
						lprwi->rui.hReg,
						lprwi->rui.ur.rgb
					);


					lprwi->rui.ut.rt = rd.rt;
                    lprwi->rui.rwf = rwfHexReg;
                    lprwi->hrwiPrev = hrwiPrev;
                    hrwiPrev = hrwi;
                    UnlockHlle( hrwi );
                    LLAddHlleToLl( m_hlliRwi, hrwi );
                }
                else {
                    errno = ENOMEM;
                    break;
                }
            }
        }

		// Regular CPU flags
        ird = 0;
        while( xosdNone == OSDGetFlagDesc( hpid, htid, ird, &fd ) ) {
            if ( !( fd.ft & ftInvisible ) ) {
                HRWI	hrwi = LLHlleCreate( m_hlliRwi );

                if ( hrwi ) {
                    LPRWI    lprwi = (LPRWI)LLLpvFromHlle( hrwi );

                    lprwi->lsz = fd.lszName;
                    lprwi->cbit = fd.dwcbits;
                    lprwi->rui.hReg = fd.dwId;
		    		lprwi->rui.ut.ft = fd.ft;
				    lprwi->dwGrp = fd.dwGrp;

                    OSDReadFlag(
                        hpid,
                        htid,
                        ird,
                        (VOID FAR *)&lprwi->rui.ur.ul
                    );

                    lprwi->rui.iFlag = ird;
                    lprwi->rui.rwf = rwfFlag;
                    lprwi->hrwiPrev = hrwiPrev;
                    hrwiPrev = hrwi;
                    UnlockHlle( hrwi );
                    LLAddHlleToLl( m_hlliRwi, hrwi );
                }
                else {
                    errno = ENOMEM;
                    break;
                }
            }
            ++ird;
        }

        // After all the CPU regs and flags are loaded, add in the EA stuff
		m_ihrwiEA = LLChlleInLl( m_hlliRwi );
        RwiUpdateEA();

		// Regular FPU registers
		ird = 0;
        while( xosdNone == OSDGetRegDesc( hpid, htid, ird++, &rd ) ) {
            if ( rd.rt & rtFPU && rd.rt & rt ) {
                HRWI	hrwi = LLHlleCreate( m_hlliRwi );

                if ( hrwi ) {
                    LPRWI    lprwi = (LPRWI)LLLpvFromHlle( hrwi );

                    lprwi->lsz = rd.lszName;
                    lprwi->cbit = rd.dwcbits;
                    lprwi->rui.hReg = rd.dwId;
					lprwi->dwGrp = rd.dwGrp;

					OSDReadRegister(
						hpid,
						htid,
						lprwi->rui.hReg,
						lprwi->rui.ur.rgb
					);

					lprwi->rui.ut.rt = rd.rt;
					if ( rd.rt & rtFloat ) {
						lprwi->rui.rwf = rwfFloat;
					}
					else {
                    	lprwi->rui.rwf = rwfHexReg;
					}
                    lprwi->hrwiPrev = hrwiPrev;
                    hrwiPrev = hrwi;
                    UnlockHlle( hrwi );
                    LLAddHlleToLl( m_hlliRwi, hrwi );
                }
                else {
                    errno = ENOMEM;
                    break;
                }
            }
        }

        // Now, reset the display cooridnates
        RecalcCoords( FALSE );
    }
}


/**** UpdateRegs                                                        ****
 *                                                                         *
 *  PURPOSE: Update all of the DATA in the rwi list.  Usually done after   *
 *  execution                                                              *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: UpdateRegs() {

    HRWI    hrwi = hrwiNull;
    HPID    hpid = hpidCurr;
    HTID    htid = htidCurr;
    HRWI    hrwiPrev;
	
    while( hrwi = LLHlleFindNext( m_hlliRwi, hrwi ) ) {
        LPRWI   lprwi = (LPRWI)LLLpvFromHlle( hrwi );

        // Unlock here since we may delete the node below!
        UnlockHlle( hrwi );

        switch( lprwi->rui.rwf ) {

            case rwfFlag:
                OSDReadFlag(
                    hpid,
                    htid,
                    lprwi->rui.iFlag,
                    (VOID FAR *)&lprwi->rui.ur.ul
                );
                hrwiPrev = hrwi;
                break;

            case rwfHexReg:
			case rwfFloat:
				OSDReadRegister(
					hpid,
					htid,
					lprwi->rui.hReg,
					lprwi->rui.ur.rgb
				);
                hrwiPrev = hrwi;
				break;

            // EA addr and/or data.  Delete the nodes from the list
            // since the call to RwiUpdateEA will add them back
            // if necessary

/*            case rwfEffAddr:
            case rwfEffAddrOnly:
            case rwfEffData:
                LLFDeleteHlleFromLl( m_hlliRwi, hrwi );
                hrwi = hrwiPrev;
                break;
*/
        }
    }

    // Get new EA info
    RwiUpdateEA();

    // Update the display cooridnates (necessary if EA info changed)
    RecalcCoords( FALSE );
}


/**** UpdateDisplay                                                     ****
 *                                                                         *
 *  PURPOSE: Update the register display.  This is called when the kernel  *
 *           comes back from executing the child or a value is changed     *
 *           via the command window or something like that                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *      fForce  Force a repaint of the register window.  If FALSE, no paint*
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: UpdateDisplay(
BOOL    fForce ) {

	if ( DebuggeeAlive() ) {
		BOOL update = FALSE;
		ADDR pc = {0};
		WORD model = m_wModel;
		LPVOID pSym;
		CV_uoff32_t off;

		// If we have P-code present we need to look for model changes as well. 
		if ( lpprcCurr->fPcodePresent && 
       			(SYGetAddr( hpidCurr, htidCurr, adrPC, &pc )==xosdNone )
			)
		{
			SHModelFromAddr( &pc, &model, (LPB)&pSym, &off );
		}

    	if ( !m_hlliRwi ||
        	m_f386Mode != fIn386Mode ||
        	m_wEM != lpprcCurr->wEM  || 
			model != m_wModel )
		{	
			update = TRUE;
		}

		if (update) {
    	    m_f386Mode = fIn386Mode;
	        m_wEM = lpprcCurr->wEM;
			m_wModel = model;
			m_cEA = 0; // No previous EA values. 
        	UpdateRwi();
    	}
	    else {
        	UpdateRegs();
    	}
	}

   	// Only paint the window if we're fForce'd to
   	if ( fForce ) {
   	    DrawWindow();
    }
}


/**** InitWndMgr                                                        ****
 *                                                                         *
 *  PURPOSE: Initialize the edit manager.                                  *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value: non-zero for success.                                *
 *                                                                         *
 *  IMPLEMENTATION: Copy the data structure from _based memory.  It's      *
 *                  faster and smaller than initializing fields.           *
 *                                                                         *
 ***************************************************************************/
int CCpuView :: InitWndMgr() {
	return InitEditMgr();
}


/**** DestroyInfo                                                       ****
 *                                                                         *
 *  PURPOSE: Destroy all internal data structures associated with register *
 *           display                                                       *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
void CCpuView :: DestroyInfo() {
	m_hrwiEdit = hrwiNull;

    if ( m_hlliRuiUndo ) {
        LLChlleDestroyLl( m_hlliRuiUndo );
        m_hlliRuiUndo = (HLLI)NULL;
    }

	if ( m_hlliRuiRedo ) {
        LLChlleDestroyLl( m_hlliRuiRedo );
        m_hlliRuiRedo = (HLLI)NULL;
    }

    if ( m_hlliRwi ) {
        LLChlleDestroyLl( m_hlliRwi );
        m_hlliRwi = (HLLI)NULL;
    }

	// Reset the EA information to initial state
	m_cEA = 0;
	m_ihrwiEA = (DWORD)-1;

	// Reset edit state
	m_hrwiEdit = hrwiNull;

	// When the window is destroyed and the
	// destructor of CCpuView is called, don't
	// update the Doc undo info
	if ( pViewCpu ) {
		UpdateDocInfo();
	}
}

BOOL CCpuView :: PreTranslateMessage( MSG *	pMsg ) {
	BOOL	fRet = FALSE;

	// If we get an unshift'd/unctrl'd ESCAPE
	// key, throw out the current edit if there
	// is one.  Else, pass the key on to the next
	// handler

	if ( pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_ESCAPE &&
		!( GetKeyState( VK_SHIFT ) & ~1 ) &&
		!( GetKeyState( VK_CONTROL ) & ~1 ) &&
		m_hrwiEdit
	) {
		// Translate the message to something that
		// will get by the dockmanager!
		pMsg->message = WU_FLUSHEDITS;
		pMsg->wParam = TRUE;
	}

	return CMultiEdit::PreTranslateMessage( pMsg );
}


/**** LTextProc                                                         ****
 *                                                                         *
 *  PURPOSE: Main entry point for display/update of register window        *
 *                                                                         *
 *  INPUTS: standard WndProc                                               *
 *                                                                         *
 *  OUTPUTS: standard WndProc                                              *
 *                                                                         *
 *      Return Value:                                                      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 ***************************************************************************/
LRESULT CCpuView :: LTextProc(
UINT    wMsg,
WPARAM  wParam,
LPARAM  lParam ) {
	LRESULT	lRet = 0L;
	BOOL	fUseEM = TRUE;	// Use the EditMgr?

    switch( wMsg ) {
        case WU_INITDEBUGWIN:
			fUseEM = FALSE;
			InitializeMembers();
            lRet = InitWndMgr();
            UpdateDisplay( FALSE );
            Freeze();
            break;

        case WU_CLEARDEBUG:
			fUseEM = FALSE;
            DestroyInfo();
            break;

		case WM_KEYDOWN:
			fUseEM = FDoKeyDown( wParam, lParam );
			break;

        case WM_CHAR:
            fUseEM = FDoChar( wParam, lParam );
            break;

		case WM_KILLFOCUS:
		case WM_VSCROLL:
		case WM_HSCROLL:
		case WM_LBUTTONDOWN:
			FlushEdit( FALSE );
			break;

        case WM_LBUTTONDBLCLK: {
			HRWI    hrwi;
			RWF		rwf = RwfFieldType( &hrwi );

			// If we're on text, see if it's a flag
			// text item.  If so, allow the toggle
			if ( rwf == rwfText ) {
				LPRWI	lprwi = (LPRWI)LLLpvFromHlle( hrwi );

				rwf = lprwi->rui.rwf;
				UnlockHlle( hrwi );
			}

			if ( rwf == rwfFlag ) {
				if ( DebuggeeRunning() ) {
					MessageBeep( 0 );
				}
				else {
					ToggleFlag( hrwi );
					FlushEdit( FALSE );
				}
				fUseEM = FALSE;
			}
            break;
		}

		case WM_UNDO:
			fUseEM = FALSE;
			UndoValue( (BOOL) wParam );
			break;

		case WU_UPDATESCROLLBARS:
		case WM_SIZE:
			// Make sure that the cursor is reset so
			// we don't end up with a selection or
			// have the cursor beyond the end of the
			// window
			SetCursorAt( 0, 0 );
			RecalcCoords( (BOOL)( wMsg == WM_SIZE ) );
			InvalidateRect(NULL, FALSE);
			break;

        case WM_CLEAR:
            break;

		case WU_UPDATEDEBUG:
			fUseEM = FALSE;
	        UpdateDisplay( TRUE );
			break;

		case WU_FREEZEDATA:
			fUseEM = FALSE;
			Freeze();
			break;

		case WU_FLUSHEDITS:
			fUseEM = FALSE;
			FlushEdit( !!wParam );
			break;

		case WM_COMMAND:
 			fUseEM = FALSE;

			// Set the cursor to 0,0 before resetting the
			// display parameters.  This will cause a repaint
			// and since the recalc hasn't been done, we may
			// overflow the buffer
			SetCursorAt( 0, 0 );

			m_fShowFlags = runDebugParams.fCPUFlags;
			m_fShowCpu = runDebugParams.fCPURegs;
			m_fShowEA = runDebugParams.fCPUEA;
			m_fShowFPU = runDebugParams.fCPUFloat;

			RecalcCoords( FALSE );
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

BEGIN_POPUP_MENU(Registers)
	MENU_ITEM(IDM_DBGSHOW_FLOAT)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_TOGGLE_MDI)
	MENU_ITEM(ID_WINDOW_POPUP_HIDE)
END_POPUP_MENU()

void CCpuView::ShowContextPopupMenu(CPoint pt)
{
    #ifndef NO_CMDBARS
        TRACE2("Registers (%s, %d)\r\n", __FILE__, __LINE__);
        ::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(Registers), pt);
    #else
    	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(Registers), pt);
    #endif
}
