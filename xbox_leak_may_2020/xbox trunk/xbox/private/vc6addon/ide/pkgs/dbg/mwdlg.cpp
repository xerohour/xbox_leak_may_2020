#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// Edit.Goto... (Address)

void CMemoryView :: GotoLine()
{
	CString str;
	str.LoadString(IDS_GOTOADDR_WHAT);

	DoGoToDialog(IDD_GOTOADDR, str);
}

#if 0 // old code
class CMWGotoDlg : public CGotoDlg {
	public:
		CMWGotoDlg( void );

	protected:
		virtual BOOL FCanDeactivate( void );
};

void CMemoryView :: GotoLine() {
	CMWGotoDlg		mdg;
	BOOL			fLookAround = TRUE;
	char			sz[ axMax ];

	*sz = '\0';

	// If the ribbon has the focus, get the text from it
	// (this probably can't happen!)
	if ( IsFindComboActive() ) {
		GetFocus()->SendMessage(
			WM_GETTEXT,
			sizeof( sz ),
			(LPARAM)(LPSTR)sz
		);
	}

	// Memory window is up, get the current address!
	else {
		PMWI	pmwi = &((CMemoryView *)pViewMemory)->m_mwi;

		strcpy( sz, pmwi->szExpr );
	}

	// Set up the dialog object and begin....
	mdg.m_strGoto = sz;
	mdg.m_strEditTitle.LoadString( IDS_GOTOEXPR );
	mdg.DoModal();
}

CMWGotoDlg :: CMWGotoDlg()
	: CGotoDlg( IDD_GOTOADDR, (CWnd *)NULL )
{
}

BOOL CMWGotoDlg :: FCanDeactivate() {
	char	sz[ axMax ];
	MWI		mwi = {0};
	BOOL	fExit = FALSE;

	_ftcsncpy( sz, (LPCTSTR)m_strGoto, sizeof( sz ) );
    sz[ sizeof( sz ) - 1 ] = '\0';

	mwi.szExpr = sz;

    if ( MWSetAddrFromExpr( &mwi, runDebugParams.iMWAutoTypes ) ) {
		// If we get here the window MUST be open already
		ASSERT( pViewMemory );

		// Update the global address
		MWSaveSettings( sz );

		// Update the memory window
		pViewMemory->SendMessage( WM_COMMAND, 0, 0L );

		fExit = TRUE;
	}
	else {
		CVMessage( ERRORMSG, BADADDR, MSGSTRING, (char FAR *)sz );
		DebuggerMessage( Error, 0, (LPSTR)sz, SHOW_IMMEDIATE );
	}

	return fExit;
}

#endif // old code
