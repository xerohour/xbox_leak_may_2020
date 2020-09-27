#ifndef DIALOG_H
#define DIALOG_H
/*************************************************************************************************
Dialog.h			: Dialog headers.. There are no current versions that take anything not from
                        a string table.  We might want to change that if we end up formatting strings
                        
Creation Date		: 1/9/2002 6:04:23 PM
Library				: .lib
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
//*************************************************************************************************/
#include "MsgDialog.h"


// This version of the dialog has just an OK button, and an error message
HRESULT RunOKDlg( long ErrorID );

// Set textID, not an error
HRESULT RunOKDlg( const TCHAR* pMessageID, const TCHAR* pOKStrID = _T("OK") );


HRESULT RunYesNoDlg( const TCHAR* pMessageID, const TCHAR* pYESStrID = _T("YES"), 
                   const TCHAR* pNoStrID = _T("NO"), bool bUseBigButtons = 0 );
HRESULT RunProgressDlg( const TCHAR* pMessageID, ProgressFunction pFunction, const TCHAR* pCancelButtonStrID = _T("CANCEL") );
HRESULT RunWaitCursor( const TCHAR* pMessageID, WaitFunction pFunction, const TCHAR* pCancelButtonStrID = _T("CANCEL") );



//*************************************************************************************************/
#endif  // end of file ( Dialog.h )
