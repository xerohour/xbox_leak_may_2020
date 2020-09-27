/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    dukedisplay.cpp

Abstract:

    all functions necessary for DukeDisplay mode of CPX

Author:

    Jason Gould (a-jasgou) May 2000

--*/
//#include "cpx.h"
#include <xtl.h>
#include "cpx.h"
#include "modes.h"
//#include "ccl.h"
#include "device.h"
#include "5lines.h"
//#include "commands.h"

//static CL dispcommands;

//VOID DrawTheDuke()
//{
	//draw the duke here...
//}


/*************************************************************************************
Function:   DukeDisplay
Purpose:	the main function of DukeDisplay mode... calls all other necessary functions
Params:     context ---
			  STATUS_BOOT --- cpx just started
			  STATUS_ENTER --- we're entering dukedisplay mode
			  STATUS_NORMAL --- we're in dukedisplay mode, perform normal operations
			  STATUS_LEAVE --- we're leaving dukedisplay mode
			  STATUS_QUIT --- we're quitting cpx
Return:     none
*************************************************************************************/
VOID DukeDisplay(DWORD context, INT param)
{

	switch(context)
	{
	case STATUS_NORMAL:
		Display5Lines();						//update/show the 5 lines at the top
//		dispcommands.CheckControls();
//		DrawTheDuke();
		break;
	case STATUS_ENTER:
//		DebugPrint("Entering DukeDisplay mode\n");
		break;
	case STATUS_LEAVE:
//		DebugPrint("Leaving DukeDisplay mode\n");
		break;
	case STATUS_BOOT:
		{
//			BUTTONS b;
//			b.all = bSTART_PRESSED | bAB6_PRESSED;				//clear the buttons...
//			dispcommands.RegisterControl(b, SwitchController, CONTEXT_DUKEDISPLAY);//to switch controllers
		}
		break;
	case STATUS_QUIT:
//		dispcommands.~CL();
		break;
	}

}

