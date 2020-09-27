#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
void CDAMView :: GotoLine()
{
	CString str;
	str.LoadString(IDS_GOTOADDR_WHAT);

	DoGoToDialog(IDD_GOTOADDR, str);
}


#if 0  // old code
// Edit.Goto... (Address)
class CDAMGotoDlg : public CGotoDlg {
	public:
		CDAMGotoDlg( void );

	protected:
		virtual BOOL FCanDeactivate( void );
};

// Real hard code!
void CDAMView :: GotoLine() {
	CDAMGotoDlg		dgd;
	BOOL			fLookAround = TRUE;
	char			sz[ axMax ];

	*sz = '\0';
	if ( IsFindComboActive() ) {
		GetFocus()->SendMessage(
			WM_GETTEXT,
			sizeof( sz ),
			(LPARAM)(LPSTR)sz
		);
	}
	else if ( GetCurTextView() != NULL ) {
		ICH	ichLeft;

		GetCurTextView()->GetCurrentText(
			&fLookAround,
			(LPSTR)sz,
			sizeof( sz ),
			&ichLeft,
			NULL
		);
	}

	dgd.m_strGoto = sz;
	dgd.m_strEditTitle.LoadString( IDS_GOTOEXPR );
	dgd.DoModal();
}

CDAMGotoDlg :: CDAMGotoDlg()
	: CGotoDlg( IDD_GOTOADDR, (CWnd *)NULL )
{
}

// On OK button press, check the expression to see if we can set the
// DAM window there.  If not, beep and bail.  Otherwise adjust the DAM
// window ( open if necessary ) to the specified address and exit the
// dialog
BOOL CDAMGotoDlg :: FCanDeactivate() {
	ADDR	addr = {0};
	ADDR	addrFixedUp = {0};
	char	sz[ axMax ];
	BOOL	fExit = FALSE;
	int		radixInputSave;
	char *	psz = sz;

	extern BOOL FAddrFromSz( char *, ADDR * );

	// To solve the problem that the dialog may be initialized
	// with text from the disassembly window, change the default
	// input radix to hex.  When this code exits, the radix HAD
	// BETTER BE RESTORED!!!!
	radixInputSave = radixInput;
	radixInput = 16;

	_fstrcpy( sz, m_strGoto );

	// Skip leading zeroes.  We don't want to force this to octal
	// and this will clearly be a bug if the user actually wanted
	// to enter octal!
	if ( *sz == '0' ) {

		while( *psz && *psz == '0' ) {
			++psz;
		}

		// The zero is required for specifying hex or dex (ie 0n10 or 0x1fa)
		if ( psz > sz &&
			( !*psz || *psz == 'n' || *psz == 'N' || *psz == 'x' || *psz == 'X' )
		) {
			--psz;
		}
 	}

	// Try to get an addr from the expression
	if ( FAddrFromSz( psz, &addr ) &&
		 ( addrFixedUp=addr, SYFixupAddr(&addrFixedUp) )
	) {
		CXF		cxf = {0};

		*SHpADDRFrompCXT( SHpCXTFrompCXF( &cxf ) ) = addr;

		// This wll make sure that the window is either
		// open or has the input focus
		if ( !pViewDisassy ) {
			OpenDebugWindow( DISASSY_WIN );
		}
		else {
			theApp.SetActiveView( pViewDisassy->GetSafeHwnd() );
		}

		// Window is open, set to new address
		if( pViewDisassy ) {
			pViewDisassy->SendMessage(
				WU_UPDATEDEBUG,
				wPCXF,
				(DWORD)&cxf
			);

			// Invalidate the window so we get a repaint after
			// the dialog gets destroyed
			pViewDisassy->InvalidateRect(
				(LPRECT)NULL,
				FALSE
			);

		}
		fExit = TRUE;
	}
	else {
		CVMessage( ERRORMSG, BADADDR, MSGSTRING, (char FAR *)sz );
		DebuggerMessage( Error, 0, (LPSTR)sz, SHOW_IMMEDIATE );
	}

	// Make sure that the radix gets restored!!!
	radixInput = radixInputSave;

	return fExit;
}
#endif // old code
