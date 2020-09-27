///////////////////////////////////////////////////////////////////////////////
//	UOUTPUT.CPP
//
//	Created by :			Date :
//		DavidGa					12/10/93
//
//	Description :
//		Implementation of the various UIDockWindow derived classes
//

#include "stdafx.h"
#include "..\sym\cmdids.h"
#include "uioutput.h"
#include "uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIOutput::Activate(void)
// Description: Display the Output window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIOutput::Activate(void)
{
	UIWB.DoCommand(IDM_WINDOW_ERRORS, DC_MNEMONIC);
	return AttachActive();
}






