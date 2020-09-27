//
//	PROGDLG.H
//
//
//	Defines dialog class for giving feedback during a long operation
//	with a "Cancel" button.  Main purpose is to get all yielding code
//	in one place.
//
//
//	Call DoYield function to allow the main window to run.
//
//	Expects the dialog template to have a button with IDCANCEL.
//	Anything else is up to you.  You can give feedback by calling
//	SetDlgItemText or whatever.
//
//
//
// History
// =======
// Date			Who			What
// ----			---			----
// 22-May-93	danw		Created
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __PROGDLG_H__
#define	__PROGDLG_H__

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

class CProgressDialog: public CDialog 
{
public:
	//
	//	Create creates the dialog _and_ disables the main window:
	//
	BOOL CreateProgressDialog	(LPCSTR lpszTemplateName, 
								 CWnd *pParent = NULL);
	BOOL CreateProgressDialog	(UINT id, CWnd *pParent = NULL) 
	{
		return CreateProgressDialog( MAKEINTRESOURCE (id), pParent);
	}
	//
	//	Must call this even if user aborted.  Reenables main window:
	//
	void DestroyProgressDialog  ();
	//
	//	Get the state of the user abort flag.  FALSE if user hit
	//	cancel:
	//
	BOOL GetState () { return ProgressDialogContinue; };
	//
	//	Call DoYield to allow other parts of the app to run.  Returns
	//	FALSE if a WM_QUIT message has been seen by PumpMessage.
	//
	BOOL DoYield ();
private:
	void OnCancel();
	//
	//	The dialog will set this flag to FALSE if the user presses Cancel:
	//
	BOOL ProgressDialogContinue;
};

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __PROGDLG_H__
