/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    cpx.cpp

Abstract:

    initializes all global variables, contains main loop, init, and de-init for 
	Control Panel X

Author:

    Jason Gould (a-jasgou) May 2000

--*/

#ifdef NUGOOP
// #error nugoop defined!!
#endif

#include <xtl.h>
#include <stdio.h>
#include "cpx.h"
#include "ccl.h"
#include "5lines.h"
#include "commands.h"
#include "device.h"
#include "draw.h"
#include "dukedisplay.h"
#include "log.h"
//nclude "memoryfile.h"
//nclude "memorysector.h"
//#include "xpong.h"
#include "mmu.h"



//*******************OTHER GLOBAL DATA************************************************
BYTE						gCurrent					= 0;	//the index of the device we're looking at
BOOL						gQuit						= FALSE;
DWORD						gMode						= 0;
CL							gGlobalCommands;						//global commands, like "Quit", etc.
                                                                    //the modes of cpx

ModeFunc                    gModeFunctions  [] = {
									DukeDisplay
//									, 
//									DukeCheck, MemoryFile, MemorySector
//									#ifdef _UseXpong_
  //                                     , XPong
	//								#endif
							};
		

int							MAX_MODES = sizeof(gModeFunctions) / sizeof(gModeFunctions[0]);


/*************************************************************************************
Function:	DeviceLog
Purpose:	log changes in the control pads...
Params:		lastupdate --- the number of microseconds since we started reading the device
			slot --- the slot the device is in
			packet --- the packet number just received
			data --- the data that came from the device
*************************************************************************************/
void DeviceLog(UINT64 lastupdate, int slot, gpGAMEPAD* data, DWORD packet)
{
    CHAR c[100];
	const BYTE * pc;
	int i;

    sprintf(c, "%12I64u %02x %8x", lastupdate, slot, packet);
    for(pc = (BYTE*)data, i = 0; i < sizeof(*data); i++)
	{
        sprintf(c, "%s %02x", c, (unsigned int)(pc[i]));
	}
//  DebugPrint("%s\n", c);
	logLog("%s\r\n", c);

}

/*************************************************************************************
Function:	init
Purpose:	performs all initialization routines for CPX
Params:		none
Out:		sets up the console window, control-pads, memory units, and cpx modes
Notes:		until direct3d is ported, the console window stuff is irrelevant...
*************************************************************************************/
void init()
{
	int i;

	DWORD dwRet;

    DebugPrint("Welcome to Control Panel X: Built on %s at %s\n", __DATE__,__TIME__);
	logInit("t:\\cpxlog.log");

    //Initialize core peripheral port support
    XInitDevices(0,NULL);

	gpInit(0);
	gpSetLogFunction(DeviceLog);

	drInit();

	muInit();
	
//	BUTTONS b;
//	b.all = bSTART_PRESSED | bBACK_PRESSED;		//start + select 
//	gGlobalCommands.RegisterControl(b, QuitFunction, CONTEXT_GLOBAL);	//trigger the save/quit function

//	b.all = bSTART_PRESSED | bAB7_PRESSED;			//start + right button
//	gGlobalCommands.RegisterControl(b, ModeFunction, CONTEXT_GLOBAL);	//trigger the change-mode function
	
	DebugPrint("Initializing modes\n");
	for(i = 0; i < MAX_MODES; i++) {
		gModeFunctions[i](STATUS_BOOT, 0);
	}
	gModeFunctions[gMode](STATUS_ENTER, 0);

	DebugPrint("Done with initialization!\n");
}



/*************************************************************************************
Function:	deinit
Purpose:	de-initialize all cpx stuff (NONE, since we never should quit!)
Params:		none
Out:		un-registers the callbacks for controlpads and MUs, tells all cpx modes to quit
Notes:		We shouldn't ever go here, since xbox only has one process, & this is it...
*************************************************************************************/
void deinit()
{
	DebugPrint("Quitting???? ...");
	logDeinit();

	gModeFunctions[gMode](STATUS_LEAVE, 0);
	for(int i = 0; i < MAX_MODES; i++) {
		gModeFunctions[i](STATUS_QUIT, 0);
	}

	gpDeinit();
	drDeinit();
	muDeinit();

	DebugPrint("gGlobalCommands.~CL()\n");
	gGlobalCommands.~CL();

	DebugPrint("Deinit is finished.\n");
	
}


/*************************************************************************************
Function:	wWinMain (...)
Purpose:	the entry-point of cpx. Calls init, does the main loop, then de-inits
Params:		irrelevant
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/
//DWORD WINAPI TestMain()
//int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow)

int __cdecl main()
{
//	_asm int 3;

	BOOL isinserted;
	BOOL repeat = 0;
	int i;
	init();

	gpPoll();
	Sleep(16);

	while(gQuit == FALSE) //loop until we're supposed to quit...
	{
		drCls();
		gpUpdateDeviceStateInfo();				//update the button state info...
		muGetDeviceMessages();

		isinserted = FALSE;
		for(i = 0; i < ::gpMaxSlots(); i++)	{
			if(::gpDeviceExists(i)) {
				isinserted = TRUE;
				repeat = FALSE;
			}
		}
		if(!isinserted && !repeat) {
			QuitFunction(0, FALSE, 0);
			repeat = TRUE;
		}

		gpPoll();
		gGlobalCommands.CheckControls();		//see if any commands are being presseed
		gModeFunctions[gMode](STATUS_NORMAL, 0);	//go to the appropriate mode
		drShowScreen();							//and draw the screen
	} 

	//we should theoretically never get here on xbox......

	deinit();


	return 0;
}

