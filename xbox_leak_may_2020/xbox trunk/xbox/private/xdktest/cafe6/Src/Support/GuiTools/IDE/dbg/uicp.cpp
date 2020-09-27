///////////////////////////////////////////////////////////////////////////////
//  UICP.CPP
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Implementation of the UICoProcessor class
//

#include "stdafx.h"
#include "uicp.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\shl\uwbframe.h"
#include "..\sym\reg.h"
#include "..\sym\qcqp.h"


#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: CString UICoProcessor::GetCPRegister(int reg)
// Description: Get the value of one of the coprocessor's registers.
// Return: A CString that contains the value of the specified coprocessor's register.
// Param: reg An integer that specifies the 1-based index into the set of coprocessor registers.
// END_HELP_COMMENT
CString  UICoProcessor::GetCPRegister(int reg)
	{
	ASSERT(reg <= MaxReg);
	char buffer[10];
	CString count=itoa(reg+REG_OFFSET,buffer,10);
	CString Keys = "^{HOME}{TAB "+count+"}+{END}^(c)";
	MST.DoKeys(Keys);
	CString cstr;
	GetClipText(cstr);
	return cstr;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL UICoProcessor::SetCPRegister(int reg, LPCSTR value)
// Description: Set the value of one of the coprocessor's registers.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: reg An integer that specifies the 1-based index into the set of coprocessor registers.
// Param: value A pointer to a string that contains the value to set the specified register to.
// END_HELP_COMMENT
BOOL UICoProcessor::SetCPRegister(int reg, LPCSTR value)
	{
	ASSERT(reg <= MaxReg);
	char buffer[10];
	CString count=itoa(reg+REG_OFFSET,buffer,10);
	CString Keys = "^{HOME}{TAB "+count+"}";
	MST.DoKeys(Keys);
	// call func that excapes the dokeys controls chars use TRUE flag
	MST.DoKeys(value,TRUE);
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL UICoProcessor::IsValid(void) const
// Description: Determine if the register's window is valid by checking that the HWND is valid and is a toolbar HWND.
// Return: A Boolean value that indicates whether the register's window is valid. TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UICoProcessor::IsValid(void) const
	{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
	return UIWindow::IsValid() && (HWnd() == UIWB.GetToolbar(GetID(),pkgId));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UICoProcessor::AttachActive(void)
// Description: Attach to the registers window if it's the active window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UICoProcessor::AttachActive(void)
	{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}

	HWND hwndFocus = MST.WGetFocus();
	HWND hwndDock = UIWB.GetToolbar(GetID(),pkgId);
	if( (hwndFocus == hwndDock) || (::IsChild(hwndDock, hwndFocus)) )
		return Attach(hwndDock);
	else
		return Attach(hwndFocus);		// will return FALSE, but the HWnd() will still point to something.
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UICoProcessor::Find(void)
// Description: Find the registers window and attach to it
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UICoProcessor::Find(void)
	{
	int pkgId ;
	if (! GetID())
		 pkgId = 0 ;
	else
		pkgId = PACKAGE_VPROJ ;

	return Attach(UIWB.GetToolbar(GetID(),pkgId));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UICoProcessor::IsActive(void)
// Description: Determine if the registers window is active.
// Return: A Boolean value that indicates whether the register's window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UICoProcessor::IsActive(void)
	{
	HWND hwnd = MST.WGetFocus();
	return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
	}
