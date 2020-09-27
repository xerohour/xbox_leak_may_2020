///////////////////////////////////////////////////////////////////////////////
//  UIWBMSG.CPP
//
//  Created by :            Date :
//      DavidGa                 1/21/94
//
//  Description :
//      Implementation of the UIWBMessageBox class
//

#include "stdafx.h"
#include "uiwbmsg.h"
#include "wbutil.h"
#include "uwbframe.h"
#include "Strings.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//  UIWBMessageBox class

/*	IsValid
 *		I'd like a better way to verify this, but at the moment this will have to do.
 *		A Valid MSVC messagebox is a popup window with the exact caption,
 *		"Microsoft Visual C++", and it isn't the IDE.
 */
// BEGIN_HELP_COMMENT
// Function: BOOL UIWBMessageBox::IsValid(void) const
// Description: Determine if the active message box is a valid IDE message box.
// Return: TRUE if the message box is valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBMessageBox::IsValid(void) const
{
	if( !UIMessageBox::IsValid() )
		return FALSE;
	return (GetText() == GetLocString(IDSS_WORKBENCH_TITLEBAR)) && (HWnd() != UIWB.HWnd());
}

