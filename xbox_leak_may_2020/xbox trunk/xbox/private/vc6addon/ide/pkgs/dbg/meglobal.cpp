 /**** GWIN.C - Windows CodeView Mapping layer for CW services          ****
 *                                                                         *
 *  Copyright <C> 1992, Microsoft Corp                                     *
 *                                                                         *
 *  Created: March 29, 1992 by Mark A. Brodsky                             *
 *                                                                         *
 *  Purpose: To provide API equivalents of CW in the Windows world         *
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

CMultiEdit *CMultiEdit::m_pList[NUM_ME_WINDOWS] = {NULL, NULL, NULL, NULL, NULL, NULL};

// ClearMultiEdit is called when the debuggee has died or has been terminated.
// This function will call all MultiEdit controlled windows with the
// WU_CLEARDEBUG notification so the window can be cleared.
void ClearMultiEdit ( void )
{
    for (INT i = 0; i < NUM_ME_WINDOWS; i++)
    {
        if ( CMultiEdit::m_pList[i] )
            CMultiEdit::m_pList[i]->SendMessage( WU_CLEARDEBUG, 0, 0L );
	}
}

void AddThisToCMEList(CMultiEdit *pme)
{
    // Add the CME to the list using the doctype as the index.
    ASSERT(!CMultiEdit::m_pList[pme->m_dt]);

    CMultiEdit::m_pList[pme->m_dt] = pme;
}

void RemoveThisFromCMEList(CMultiEdit *pme)
{
    ASSERT(CMultiEdit::m_pList[pme->m_dt] == pme);

    CMultiEdit::m_pList[pme->m_dt] = NULL;
}

void UpdateMultiEditScroll (CMultiEdit *pCME)
{
	pCME->SendMessage(WU_UPDATESCROLLBARS, 0, 0L);
}

void UpdateAllMultiEditScroll()
{
    for (INT i = 0; i < NUM_ME_WINDOWS; i++)
    {
        if ( CMultiEdit::m_pList[i] )
            CMultiEdit::m_pList[i]->SendMessage( WU_UPDATESCROLLBARS, 0, 0L );
	}
}

// Following code is VERY similar to SaveDocument found in docfile.c!+
BOOL SaveMultiEditDocument( CMultiEdit *pme, const CPath &pathSave)
{
    HCURSOR hSaveCursor;
    FILE *	pfile;

    // Put up the hourglass.  EARLY EXIT POINT IS NOW 'error'.
	hSaveCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	if ( pfile = fopen( pathSave.GetFullPath(), "w" ) )
    {
		DWORD			lnStart;
		DWORD			lnEnd;

		// ASSERT( pme->IsKindOf( RUNTIME_CLASS( CMultiEdit ) ) );

		// Memory and disassembly windows will only dump currently visible data
		if ( pme->m_dt == MEMORY_WIN || pme->m_dt == DISASSY_WIN )
        {
			lnStart = pme->OlnGetTop();
			lnEnd = lnStart + pme->WinHeight() + 1;
		}
		else
        {
			lnStart = 0;
			lnEnd = pme->CLinesInBuf();
		}

		while( lnStart < lnEnd )
        {
			char	sz[ axMax + 2 ];
			PDLA	pdla;

			pme->CbGetLineBuf(lnStart++, sizeof( sz ) - 1, sz, pdla);

			fprintf( pfile, "%s\n", sz );
		}

		fclose( pfile );
	}

	SetCursor (hSaveCursor);

	return( pfile != NULL );
}

