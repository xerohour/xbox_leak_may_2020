//////////////////////////////////////////////////////////////////////////
//
//	MWUNDO.C - Memory Window Handler
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

//// UndoValue
//
// Purpose: Undo the last changed value in the active memory window.
//
// Input: none.
//
// Output: none.
//
// Exceptions:
//
// Notes: Forces cached data to be uncached.
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: UndoValue(
BOOL	fRedo
) {
	HLLE	hlle;
	HLLE    hlleNew;
	LPMUB	lpmub;
	LPMUB	lpmubNew;
	DWORD	iln;
	DWORD	ilnTop;
	DWORD	ilnBottom;
	WORD	ich;
    DWORD	dln;
	RRC		rrc;
	ADDR	addrT;
	UOFFSET	uo;
	USHORT	ucls;

	if ( !m_mwi.hlliUndo ) {
		m_mwi.hlliUndo = LLHlliInit( sizeof( MUB ), llfNull, NULL, NULL  );
	}
	if ( !m_mwi.hlliRedo ) {
		m_mwi.hlliRedo = LLHlliInit( sizeof( MUB ), llfNull, NULL, NULL  );
	}

	if ( hlle = LLHlleGetLast( (fRedo ? m_mwi.hlliRedo : m_mwi.hlliUndo )) ) {
		lpmub = (LPMUB)LLLpvFromHlle( hlle );

		// Set up for moving from undo/redo

		hlleNew 	= LLHlleCreate( fRedo ? m_mwi.hlliUndo : m_mwi.hlliRedo );
		lpmubNew  	= (LPMUB)LLLpvFromHlle( hlleNew );

		lpmubNew->ucls = lpmub->ucls;
		lpmubNew->pmwd = lpmub->pmwd;

		lpmubNew->addr = lpmub->addr;
		lpmubNew->cb   = lpmub->cb;

		DHGetDebuggeeBytes( lpmub->addr, lpmub->cb, (char far *)lpmubNew->rgb );

		// Save cursor/window position info.
		ilnTop = OlnGetTop();
		dln = OlnGetCurPos() - ilnTop;
		iln = MWIlnFromAddr( &m_mwi, &lpmub->addr );
		ich = ObGetCurPos();
		GetClientRrc( &rrc );
		ilnBottom = (DWORD)( ilnTop + rrc.ryBottom - 1 );

		// Update the debuggees data
		DHPutDebuggeeBytes( lpmub->addr, lpmub->cb, lpmub->rgb );
		UpdateWorld( &lpmub->addr, (USHORT)lpmub->cb );

		// update screen (if necessary)
        if ( GetAddrSeg( lpmub->addr ) ==
             GetAddrSeg( m_mwi.addr ) ) {
			
			BOOL	fFound = FALSE;
			
			UncacheData();

			// position cursor
			MWAddrFromIln( &m_mwi, iln, &addrT );

			// If type is same as when changed, try to find exact
			// match on address and class
			if ( lpmub->pmwd == m_mwi.pmwd ) {
				// Start at the beginning of the line and work foward
				SetCurObPos( 0 );

				// Set to the correct line
				SetCurOlnPos( iln );

				do {
					// Move to the next field and get the info
					NextField( MW_FWD, FALSE );
				    CchGetCellInfo( FALSE, &uo, &ucls, &ich );

					fFound = (
						uo + GetAddrOff( addrT ) == GetAddrOff( lpmub->addr ) &&
						ucls == lpmub->ucls
					);
				} while( !fFound && iln == OlnGetCurPos() );
			}

			if ( !fFound ) {
				WORD	ichPrev;
				UOFFSET	cb = (UOFFSET)lpmub->cb;

				// Start at the beginning of the line and work foward
				SetCurObPos( ich = 0 );

				// Set to the correct line
				SetCurOlnPos( iln );

				do {
					// Move to the next field and get the info
					NextField( MW_FWD, FALSE );
					ichPrev = ich;
				    CchGetCellInfo( FALSE, &uo, &ucls, &ich );
					
					// If address matches exactly then done
					if ( uo + GetAddrOff( addrT ) == GetAddrOff( lpmub->addr ) ) {
						fFound = TRUE;
					}

					// Else if the current beginning address is bigger than the
					// current value, put the cursor at the previous field
					else if ( uo + GetAddrOff( addrT ) > GetAddrOff( lpmub->addr ) ) {
						fFound = TRUE;
						ich = ichPrev;
					}
				} while( !fFound && iln == OlnGetCurPos() );

				// Wrapped lines, set cursor to last value on line
				if ( !fFound ) {
					ich = ichPrev;
				}
			}

			if ( iln < ilnTop || iln > ilnBottom ) {
			    if ( iln < dln ) {
			        dln = iln;
			    }
				SetWinTopLineCursor( iln - dln, ich, iln );
			}
			else {
				DWORD	ilnMac;

				addrT = lpmub->addr;
				SetAddrOff( &addrT, GetAddrOff( addrT ) + (UOFFSET)lpmub->cb - 1 );
				ilnMac = MWIlnFromAddr( &m_mwi, &addrT );

				// Redraw all lines which may have been updated
				do {
					DrawLine( ilnMac-- );
				} while( ilnMac >= iln );
				
			    SetCursorAt( ich, iln );
			}
        }

		UnlockHlle ( hlleNew );		
		UnlockHlle ( hlle );

		if ( !fRedo ) {
			// Add the undone item to the redo list
				
			LLAddHlleToLl( m_mwi.hlliRedo, hlleNew );

			// Remove the undo'ed item from the list of changes
			LLFDeleteHlleFromLl( m_mwi.hlliUndo, hlle );

			m_undoBuffer = ubRedo;

			if ( !LLHlleFindNext  ( m_mwi.hlliUndo, 0 ) )
            {
				m_mePlayCount = -2;

				gpISrc->StatusText(STA_End_Of_Undo, STATUS_INFOTEXT, FALSE);
			}
			else
            {
				m_mePlayCount = 1;
				gpISrc->StatusText (STA_Undo, STATUS_INFOTEXT, TRUE);
			}
		}	
		else {
			// Add the redone item to the unredo list
				
			LLAddHlleToLl( m_mwi.hlliUndo, hlleNew );

			// Remove the redone item from the list of changes
			LLFDeleteHlleFromLl( m_mwi.hlliRedo, hlle );

			if ( !LLHlleFindNext  ( m_mwi.hlliRedo, 0 ) )
            {
				m_undoBuffer = ubUndo;
				m_mePlayCount = 0;
				gpISrc->StatusText(STA_End_Of_Redo, STATUS_INFOTEXT, FALSE);
			}
			else
            {
				m_mePlayCount = 1;
				gpISrc->StatusText (STA_Redo, STATUS_INFOTEXT, TRUE);
			}
		}
	}
}

//// DestroyUndoList
//
// Purpose: Destroy the list data associated with a window.  This will be
// called before the debuggee steps or gos, so they can't undo from a
// previous step.
//
// Input: none.
//
// Output: none.
//
// Exceptions:
//
// Notes:
//
///////////////////////////////////////////////////////////////////////////
void CMemoryView :: DestroyUndoList() {
	HLLI	hlli;

	if ( pViewMemory )
    {
		m_undoState = usNormal;
		m_mePlayCount = -1;
	}

	if ( hlli = m_mwi.hlliUndo )
    {
		LLChlleDestroyLl( hlli );
		m_mwi.hlliUndo = (HLLI)NULL;
	}

	if ( hlli = m_mwi.hlliRedo )
    {
		LLChlleDestroyLl( hlli );
		m_mwi.hlliRedo = (HLLI)NULL;
	}
}
