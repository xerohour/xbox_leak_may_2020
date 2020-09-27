///////////////////////////////////////////////////////////////////////////////
//  UILOCALS.CPP
//
//  Created by :            Date :
//      MichMa              	1/17/94
//
//  Description :
//      Implementation of the UILocals class
//

#include "stdafx.h"
#include "uilocals.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
   

// BEGIN_HELP_COMMENT
// Function: BOOL UILocals::Activate(void)
// Description: Bring up the locals window.
// Return: A Boolean value that indicates whether the register's window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UILocals::Activate(void)
{
	UIWB.DoCommand(IDM_VIEW_VARS, DC_MNEMONIC);
	return AttachActive();
}
