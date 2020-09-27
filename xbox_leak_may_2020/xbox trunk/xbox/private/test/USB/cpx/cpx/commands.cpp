/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    commands.cpp

Abstract:

    contains the various control functions that can be accessed from any CPX mode

Author:

    Jason Gould (a-jasgou) May 2000

--*/

#include <xtl.h>
#include "cpx.h"
#include "commands.h"
#include "modes.h"
#include "log.h"
#include "device.h"

/*************************************************************************************
Function:   QuitFunction
Purpose:	tell CPX to quit
Quick-Key:  start+select
Params:     context --- irrelevant
			repeat --- whether start+select is being repeated (irrelevant)
Return:     none
*************************************************************************************/
VOID QuitFunction(DWORD context, BOOL repeat, BYTE slot)
{
	if(!repeat) {
//		DebugPrint("Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit Quit\n");
		logDumpToDebug();
	}

	//logDeinit();

//	Sleep (10000);

}

/*************************************************************************************
Function:   ModeFunction
Purpose:	tell CPX to change modes
Quick-Key:  start+right trigger
Params:     context --- irrelevant
			repeat --- whether start+right is being repeated 
			  (we don't want to switch if it is being repeated, so it is relevant)
Return:     none
*************************************************************************************/
VOID ModeFunction(DWORD context, BOOL repeat, BYTE slot)
{
	if(!repeat) {
		gModeFunctions[gMode](STATUS_LEAVE, 0);
		gMode = (gMode + 1) % MAX_MODES;
		gModeFunctions[gMode](STATUS_ENTER, 0);
	}
	DebugPrint("MODE MODe MOde Mode mode... context %d, repeat %d, Mode %d\n",
		context,repeat,gMode);
}

/*************************************************************************************
Function:   SwitchController
Purpose:	tell CPX to look at a different controller
Quick-Key:  start+left trigger
Params:     context --- irrelevant at this point
			repeat --- we don't want repeats... we only do something if repeat == FALSE
Return:     none
*************************************************************************************/
VOID SwitchController(DWORD context, BOOL repeat, BYTE slot)
{
	if(repeat == FALSE)
	{
		switch (context)
		{
		case CONTEXT_DUKEDISPLAY:
		case CONTEXT_DUKECHECK:
		case CONTEXT_MEMORYFILE:
		case CONTEXT_MEMORYSECTOR:
			gCurrent = (gCurrent + 1) % gpMaxSlots();
			break;
		}
	}
//	DebugPrint("Controller Switch... context %d, repeat %d, New Current pad: %d\n", context,repeat,gMode);
}

