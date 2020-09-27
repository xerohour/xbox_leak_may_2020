///////////////////////////////////////////////////////////////////////////////
//  UIDEBUG.CPP
//
//  Created by :            Date :
//      BrianCr             01/17/95
//
//  Description :
//      Implementation of the UIDebug class
//

#include "stdafx.h"
#include "uidebug.h"
#include "mstwrap.h"
#include "..\sym\qcqp.h"
#include "..\shl\uioutput.h"
#include "uiwatch.h"
#include "uireg.h"
#include "uilocals.h"
#include "uistack.h"
#include "uimem.h"
#include "uidam.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIDebug::ShowDockWindow(UINT id, BOOL bShow /*=TRUE*/)
// Description: Open the docking window with the given id.
// Return: The HWND of the opened window. NULL if the function fails.
// Param: id The id of the docking window to open (IDW_OUTPUT_WIN, IDW_WATCH_WIN, IDW_LOCALS_WIN, IDW_CPU_WIN, IDW_MEMORY_WIN, IDW_CALLS_WIN, IDW_DISASSY_WIN).
// Param: bShow TRUE to show the window; FALSE to hide it (default is TRUE).
// END_HELP_COMMENT
HWND UIDebug::ShowDockWindow(UINT id, BOOL bShow /*=TRUE*/)
{
	UIDockWindow* pudw;
	switch( id )
	{
		case IDW_OUTPUT_WIN:
			pudw = new UIOutput;
			break;
		case IDW_WATCH_WIN:
			pudw = new UIWatch;
			break;
		case IDW_LOCALS_WIN:
			pudw = new UILocals;
			break;
		case IDW_CPU_WIN:
			pudw = new UIRegisters;
			break;
		case IDW_MEMORY_WIN:
			pudw = new UIMemory;
			break;
		case IDW_CALLS_WIN:
			pudw = new UIStack;
			break;
		case IDW_DISASSY_WIN:
			pudw = new UIDAM;
			break;
		default:
			ASSERT(FALSE);		// never heard of that ID
			return NULL;
	}
	BOOL b = pudw->Activate();
	delete pudw;
	if( !b )					// attempt to show failed - who knows why
		return NULL;
	if( bShow )					// window was activated and has the focus
		return MST.WGetFocus();
	MST.DoKeys("+{escape}");		// shift escape hides the active docking-window/toolbar
	return NULL;				// should we return newly active window?
}
