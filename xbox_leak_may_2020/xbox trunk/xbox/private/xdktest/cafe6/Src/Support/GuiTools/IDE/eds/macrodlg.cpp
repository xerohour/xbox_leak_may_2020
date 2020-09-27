///////////////////////////////////////////////////////////////////////////////
//  UCUSTDLG.CPP
//
//  Created by :            Date :
//      IvanL	             9/10/96
//                                
//  Description :
//      Implementation of the UIMacrosDlg class    
//

#include "stdafx.h"
#include "macrodlg.h"
#include "w32repl.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"




#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: HWND UIMacrosDlg::Display() 
// Description: Bring up the Tools.Macro dialog
// Return: The HWND of the Customize dialog.
// END_HELP_COMMENT
HWND UIMacrosDlg::Display() 
{       
	AttachActive();           // Check if is already up
	if (!IsValid())
	{
		UIWB.DoCommand(IDM_MACROS, DC_MNEMONIC);
		AttachActive();
	}
	return WGetActWnd(0); 
}


// BEGIN_HELP_COMMENT
// Function: HWND UIMacrosDlg::SelectMacroFile(CString FileName) 
// Description: Selects a macro file from the Files dropdown.
// Return: int .
// Param: FileName 
// END_HELP_COMMENT
int UIMacrosDlg::SelectMacroFile(CString MacroFile) 
{       
   MST.WComboItemClk(GetLabel(AUTO1_IDC_MACROFILE),MacroFile) ;
   return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIMacrosDlg::SelectMacro(CString MacroName)
// Description: Selects a macro to run from the macros drop down.
// Return: int 
// Param: MacroName 
// END_HELP_COMMENT
int UIMacrosDlg::SelectMacro(CString MacroName) 
{       
   MST.WComboItemClk(GetLabel(AUTO1_IDC_MACROS),MacroName);
   return ERROR_SUCCESS ;

}

// BEGIN_HELP_COMMENT
// Function: HWND UIMacrosDlg::RunMacro() 
// Description: Runs the selected macro
// Return: int .
// END_HELP_COMMENT
int UIMacrosDlg::RunMacro(void) 
{       
   MST.WButtonClick(GetLabel(AUTO1_ID_RUN));
   return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIMacrosDlg::Close() 
// Description: Closes down the macros dialog.
// Return: int .
// END_HELP_COMMENT
HWND UIMacrosDlg::Close(void) 
{       
   MST.WButtonClick(GetLabel(IDCANCEL));
   return ERROR_SUCCESS ;
}

