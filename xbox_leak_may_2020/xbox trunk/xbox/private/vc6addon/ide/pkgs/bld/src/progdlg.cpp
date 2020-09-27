//
//	PROGDLG.CPP
//
//
// History
// =======
// Date			Who			What
// ----			---			----
// 22-May-93	danw		Created
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);

///////////////////////////////////////////////////////////////////////////////
//
// ----------------------------- CProgressDialog ------------------------------
//
///////////////////////////////////////////////////////////////////////////////
BOOL CProgressDialog::CreateProgressDialog	(
							LPCSTR lpszTemplateName, 
							CWnd *pParent /*= NULL*/)
{
//
//	Create the dialog and disable the main window:
//
	PreModalWindow();
	// allow OLE servers to disable themselves, usually called in CDialog::PreModal
	AfxGetApp()->EnableModeless(FALSE);

	ProgressDialogContinue = TRUE;
	if (!CDialog::Create (lpszTemplateName,
		FromHandle(_SushiGetSafeOwner(pParent))))
	{
		return FALSE;
	}
	AfxGetApp()->m_pMainWnd->EnableWindow(FALSE);
	EnableWindow (TRUE);
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
void CProgressDialog::DestroyProgressDialog  ()
{
	AfxGetApp()->m_pMainWnd->EnableWindow (TRUE);
	EnableWindow (FALSE);
	DestroyWindow();
	AfxGetApp()->EnableModeless(TRUE);
	PostModalWindow();
}
///////////////////////////////////////////////////////////////////////////////
void CProgressDialog::OnCancel()
{
//
//	Set the flag to false.  User still has to call DestroyProgressDialog.
//
	ProgressDialogContinue = FALSE;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProgressDialog::DoYield () 
{
//
//	Yield function calls PumpMessage, which will can IsDialog message.  This
//	needs to be changed if we have our own message pump:
//
	MSG msg;
	//
	//	FUTURE: Special yielding magic goes here:
	//
	while ( PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		if (!AfxGetApp()->PumpMessage ()) return FALSE;

	}
	return TRUE;
}
