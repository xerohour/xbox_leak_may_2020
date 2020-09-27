#include "stdafx.h"

// Helper: return True if pActiveView is a valid CTextView pointer
BOOL FValidPTextView(CPartView * pActiveView)
{
	return 	(pActiveView != NULL && pActiveView->IsKindOf(RUNTIME_CLASS(CMultiEdit)));
}


// Helper: return True if dialog's current active view is a valid CTextView pointer
BOOL FValidCurTextView(CGoToDialog * pDlg)
{
	return 	(pDlg != NULL && pDlg->IsKindOf(RUNTIME_CLASS(CGoToDialog)) &&
		FValidPTextView(pDlg->m_pActiveView));
}

// Helper: returns dialogs current active view if it is a CTextView, otherwise NULL
CMultiEdit *PCurTextView(CGoToDialog * pDlg)
{
	if (FValidCurTextView(pDlg))
		return 	(CMultiEdit *)(pDlg->m_pActiveView);
	else
		return NULL;
}

void CDebugPackage::AddGoToItems(CGoToDialog *pDlg)
{
	// called by CGoToDIalog. That dialog will free up any created objects.
	CGoToAddress *pgtAddress = new CGoToAddress(pDlg);
	pDlg->AddItem(pgtAddress);
}

	// ***Initialization. These functions are used to initialize the Go To
	// dialog. They will be called when the dialog is invoked but may
	// actually be called at any time.


	// REQUIRED: Return the string to show in the Go To What listbox

static CString strGotoaddrWhat;	// static so we can return

CGoToAddress::CGoToAddress(CGoToDialog * pDlg) : CGoToItem(pDlg)
{
    return;
}

const TCHAR * CGoToAddress::GetItemString()
{
	if (strGotoaddrWhat.IsEmpty())
    {
		strGotoaddrWhat.LoadString(IDS_GOTOADDR_WHAT);
		ASSERT(!strGotoaddrWhat.IsEmpty());
    }

	return strGotoaddrWhat;
}


	// ***Callbacks. These functions are called as the user manipulates the Go To
	// dialog or active editor view

	// Called when the item is selected in the Go To What listbox

void CGoToAddress::OnSelectWhat()
{
	// need to set button style, control style, prompt and helper text
	// since this uses an edit control, not a list box, we don;t have
	// list issues. We do need to initialize the edit

	// Called when the item is selected in the Go To What listbox,
	// and at CGoToDialog startup and reactivation.
	// CGoToDialog will call CanGoViewActive after returning from this
	// call, so let it handle situational enabling/disabling

	// Note that this function needs to be able to tolerate a NULL m_pDlg->m_pActiveView
	// or one of the incorrect type. It should set what it can and leave other controls empty.

	ASSERT(m_pDlg != NULL);

	ADDR addr = {0}; // just so we can init m_addr
	m_addr = addr;

	m_pDlg->SetControlStyle(CGoToDialog::csEdit);

	CString str;
	str.LoadString(IDS_GOTOADDR_PROMPT);
	m_pDlg->SetPrompt(str);

	str.LoadString(IDS_GOTOADDR_HELP);
	m_pDlg->SetHelperText(str);
	m_pDlg->SetButtonStyle(CGoToDialog::bsGoTo);


	if (!FValidCurTextView(m_pDlg) || !CanGoViewActive((CPartView *)m_pDlg->m_pActiveView))
		m_pDlg->Select("");
	else
	{
		ASSERT (FValidCurTextView(m_pDlg));
		CMultiEdit   *pCurTextDoc = PCurTextView(m_pDlg);

		ASSERT(pCurTextDoc != NULL);
		
		BOOL fDAMWindow = pCurTextDoc->m_dt == DISASSY_WIN;

		BOOL			fLookAround = TRUE;
		char			sz[ axMax ];
		*sz = '\0';

		if ( IsFindComboActive() )
			CWnd::GetFocus()->GetWindowText((LPTSTR)sz, sizeof(sz));
		else
		{
			if ( fDAMWindow )
			{
    			ICH	ichLeft;

    			PCurTextView(m_pDlg)->GetCurrentText(&fLookAround,(LPSTR)sz,sizeof( sz ),&ichLeft,NULL);
			}
			else
			{
				ASSERT (pCurTextDoc->m_dt == MEMORY_WIN);

				PMWI	pmwi = &((CMemoryView *)pViewMemory)->m_mwi;
				strcpy( sz, pmwi->szExpr );
			}
		}

    	m_pDlg->Select(sz);	
	}
}

	// Called when the active editor changes, and after OnSelectWhat is called.
	// A FALSE return indicates that the item should not be enabled in the dialog,
	// TRUE indicates that the item may be active.
	// check all situations that may require deactivation here (e.g., deactivate if
	// editor specific and in wrong editor, or if going to an empty list, e.g., of bookmarks)

BOOL CGoToAddress::CanGoViewActive(CPartView * pActiveView)
{
	CMultiEdit  *pCurTextDoc;
    DOCTYPE     dt;

	if (!FValidPTextView(pActiveView))
		return FALSE;

	pCurTextDoc = (CMultiEdit *)pActiveView;
    dt          = ((CMultiEdit *)pCurTextDoc)->m_dt;

    return( (dt == DISASSY_WIN || dt == MEMORY_WIN ) && DebuggeeAlive() );
}


	// Called when the selection control changes (either the user changed
	// the text in the edit control or they selected something from the
	// drop list). For csEdit style selection controls, the fSelInList
	// and dwItemData parameters should be ignored (FALSE/-1 will always
	// be passed here).
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	// generally used for simple test like handling empty strings

void CGoToAddress::OnSelectChange(	const TCHAR * szSelect,
								BOOL /* fSelInList */, // ignored since csEdit
								DWORD dwItemData)
{
	ASSERT(m_pDlg != NULL);
	m_pDlg->EnableButton (mskButtonGoToNext, (*szSelect != '\0'));
}

	// Override this to indicate whether or not a go to operation can be
	// attempted. Done when an action button is hit.
	// For goNext and goPrevious operations, the szSelect
	// and dwItemData items are not used.
	// For goGoTo operations, they describe the string and optional
	// user-data selected in the Go To dialog.
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	// m_pDlg and idDlgItemError are used to return focus to in case of error

BOOL CGoToAddress::CanGoTo(int idDlgItemError,
						GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL /* fSelInList */,
						DWORD /* dwItemData */)
{
	BOOL fRet = FALSE;

	if (!FValidCurTextView(m_pDlg))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CMultiEdit  *pCurTextDoc = PCurTextView(m_pDlg);
	ASSERT(pCurTextDoc != NULL);

	BOOL fDAMWindow = pCurTextDoc->m_dt == DISASSY_WIN;

	ADDR addr = {0}; // just so we can init m_addr
	m_addr = addr;

	if ( fDAMWindow )
	{
		ADDR	addrFixedUp = {0};
		int		radixInputSave;
		TCHAR	sz[ axMax ];
        TCHAR   *psz = sz;

		extern BOOL FAddrFromSz( char *, ADDR * );

		// To solve the problem that the dialog may be initialized
		// with text from the disassembly window, change the default
		// input radix to hex.  When this code exits, the radix HAD
		// BETTER BE RESTORED!!!!
		radixInputSave = radixInput;
		radixInput = 16;

		_fstrcpy( sz, szSelect );

		// Skip leading zeroes.  We don't want to force this to octal
		// and this will clearly be a bug if the user actually wanted
		// to enter octal!
		if ( *sz == '0' )
        {
			while( *psz && *psz == '0' )
				++psz;

			// The zero is required for specifying hex or dex (ie 0n10 or 0x1fa)
			if ( psz > sz && ( !*psz || *psz == 'n' || *psz == 'N' || *psz == 'x' || *psz == 'X' ) )
				--psz;
 		}

		// Try to get an addr from the expression
		
		fRet = FAddrFromSz (psz, &m_addr);

		if (fRet) {
			addrFixedUp = m_addr;
			SYFixupAddr (&addrFixedUp);
		}

		if (!fRet) {
			CVMessage (ERRORMSG, BADADDR, MSGSTRING, (char FAR *)sz);
			DebuggerMessage (Error, 0, (LPSTR)sz, SHOW_IMMEDIATE);
		}

		// Make sure that the radix gets restored.
		radixInput = radixInputSave;
	}
	else
	{
		char	sz[ axMax ];
		MWI		mwi = {0};

		ASSERT (pCurTextDoc->m_dt == MEMORY_WIN);

		_ftcsncpy( sz, (LPCTSTR)szSelect, sizeof( sz ) );
    	sz[ sizeof( sz ) - 1 ] = '\0';

		mwi.szExpr = sz;

    	fRet = (MWSetAddrFromExpr( &mwi, runDebugParams.iMWAutoTypes));

		if ( fRet )
		{
			// actually do the work here because it is sticky saving part to OnGoTo
			ASSERT(pViewMemory);

			// Update the global address
			MWSaveSettings(sz);
			CMemoryView * pView = (CMemoryView *)pViewMemory;

			// Update the memory window. Note we will see no actula selection, but the edit text
			// will be updated to reflect the new address
			pViewMemory->SendMessage( WM_COMMAND, 0, 0L );

		}
		else
		{
			CVMessage(ERRORMSG, BADADDR, MSGSTRING, (char FAR *)sz);
			DebuggerMessage( Error, 0, (LPSTR)sz, SHOW_IMMEDIATE );
		}
	}

	return fRet;

}

	// Override this to handle a go to operation. For goNext and goPrevious
	// operations, the szSelect and iSelList, and dwItemData items are not used. For goGoTo
	// operations, they describe the string and optional listbox user-data
	// selected in the Go To dialog, and the list selection, if any.
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

void CGoToAddress::OnGoTo(GOTO_TYPE go, const TCHAR * szSelect, BOOL /* fSelInList */,DWORD /* dwItemData */)
{
	ASSERT (FValidCurTextView(m_pDlg));

	CMultiEdit  *pCurTextDoc = PCurTextView(m_pDlg);

	ASSERT(pCurTextDoc != NULL);
	ASSERT (szSelect != NULL);
	ASSERT(go == goGoTo || *szSelect == '\0'); // next/prev should have no text

	BOOL fDAMWindow = ((CMultiEdit *)pCurTextDoc)->m_dt == DISASSY_WIN;

	if ( fDAMWindow )
	{
		CXF		cxf = {0};

		*SHpADDRFrompCXT( SHpCXTFrompCXF( &cxf ) ) = m_addr;

		// This wll make sure that the window is either
		// open or has the input focus
		if ( !pViewDisassy )
			OpenDebugWindow( DISASSY_WIN );
		else
			theApp.SetActiveView( pViewDisassy->GetSafeHwnd() );

		// Window is open, set to new address
		if( pViewDisassy )
		{
			CDAMView *pView = (CDAMView *)pViewDisassy;
			pViewDisassy->SendMessage(WU_UPDATEDEBUG,wPCXF,(DWORD)&cxf);
            pView->SelectWordCur();  // select word at cursor
		}
	}
	else
	{
	   	ASSERT (pCurTextDoc->m_dt == MEMORY_WIN);
		// all work was done in the cangoto code
	}
}

