///////////////////////////////////////////////////////////////////////////////
//  UIQW.CPP
//
//  Created by :            Date :
//      MichMa              1/14/94
//
//  Description :
//      Implementation of the UIQuickWatch class
//

#include "stdafx.h"
#include "uiqw.h"
#include "uieewnd.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vcpp32.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

UIEEWindow UIEE;

// BEGIN_HELP_COMMENT
// Function: BOOL UIQuickWatch::Activate(void)
// Description: Open the QuickWatch dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIQuickWatch::Activate(void)
{
	 UIWB.DoCommand(IDM_RUN_QUICKWATCH, DC_MNEMONIC);
	if (WaitAttachActive(5000)) {
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString UIQuickWatch::GetCurrentValue(void) 
// Description: Returns the value of the current expression in the QuickWatch dialog.
// Return: A CString that contains the current value.
// END_HELP_COMMENT
CString UIQuickWatch::GetCurrentValue(void) 
{
	//MST.DoKeys("%V");	 // TODO: Hack should be replaced with GetLabel or something better
	CString strCurrentValue = "%";
	MST.DoKeys(strCurrentValue += ExtractHotKey(GetLabel(VCPP32_ID_QUICKW_TXT_MEMBERS)));
 	
	return UIEE.GetValue();
}		 

// BEGIN_HELP_COMMENT
// Function: CString UIQuickWatch::GetExpression(void) 
// Description: Returns the current expression
// Return: A CString that contains the current expression.
// END_HELP_COMMENT
CString UIQuickWatch::GetExpression(void) 
	{
	if(!(IsActive() && MST.WComboEnabled(GetLabel(VCPP32_ID_QUICKW_EXPRESSION)))) return CString("");
	CString value;
	MST.WComboText(GetLabel(VCPP32_ID_QUICKW_EXPRESSION), value);
	return value;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIQuickWatch::SetExpression(LPCSTR exp)
// Description: Sets a new expression for the Quickwatch dialog. NOTE: does not do a "recalc."
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: exp A pointer to a string that contains the expression to enter in the QuickWatch dialog.
// END_HELP_COMMENT
BOOL UIQuickWatch::SetExpression(LPCSTR exp)
	{
	if(!(IsActive() && MST.WComboEnabled(GetLabel(VCPP32_ID_QUICKW_EXPRESSION)))) return FALSE;
	MST.WComboSetText(GetLabel(VCPP32_ID_QUICKW_EXPRESSION), exp);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIQuickWatch::SetNewValue(LPCSTR value) 
// Description: Modifies the value for the current expression.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: value A pointer to a string that contains the new value to enter in the QuickWatch dialog.
// END_HELP_COMMENT
BOOL UIQuickWatch::SetNewValue(LPCSTR value) 
	{
//	MST.DoKeys("%V");	 // Hack should be replaced with GetLabel or something better
	CString strCurrentValue = "%";
	MST.DoKeys(strCurrentValue += ExtractHotKey(GetLabel(VCPP32_ID_QUICKW_TXT_MEMBERS)));
	MST.DoKeys("{TAB}");
	
	char tmpstr[4];
	char *ptr;
	// Put the + sign in {} for expressions like "123.e+022" in order to prevent "+" treated as Shift
	if(NULL != (ptr = strstr(value, "+")))
	{
		strcpy(tmpstr, ptr + 1);
		strcpy(ptr, "{+}");
		strcat((char*) value, tmpstr);
	}

	MST.DoKeys(value);
					  
	// commit edit
	MST.DoKeys("{ENTER}");
	return TRUE;
	}

      
// BEGIN_HELP_COMMENT
// Function: BOOL UIQuickWatch::Recalc(void)
// Description: Recalcs the expression in the expression field.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIQuickWatch::Recalc(void)
	{
	if(!(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_ID_QUICKW_NEWEXPR)))) return FALSE;
	MST.WButtonClick(GetLabel(VCPP32_ID_QUICKW_NEWEXPR));
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIQuickWatch::AddWatch(void)
// Description: Adds the current expression in the Quick Watch Dialog to the Watch window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIQuickWatch::AddWatch(void)
	{
	if(!(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_ID_QUICKW_ADD)))) return FALSE;
	MST.WButtonClick(GetLabel(VCPP32_ID_QUICKW_ADD));
	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: HWND UIQuickWatch::Close(void)
// Description: Close the QuickWatch Dailog
// Return: HWND: NULL means all went well, otherwise return HWND of active window (alert)
// END_HELP_COMMENT
HWND UIQuickWatch::Close(void)
{
	if(!IsActive())
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
	
	if(MST.WFndWndWaitC(GetLocString(IDS_CLOSE), "Button", FW_PART, 1))        // WinslowF - Need this for Chicago
		MST.WButtonClick(GetLabel(IDCANCEL));	
	else	
		MST.WButtonClick(GetLabel(IDCANCEL));		// WinslowF - backward compatibility; use the close button if there is one

	if( WaitUntilGone(3000) )		// REVIEW(davidga) 3000 is too arbitrary
		return NULL;		// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}