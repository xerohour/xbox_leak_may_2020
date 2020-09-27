/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    dukeprod.cpp

Abstract:

    initializes all global variables, contains main loop, init, and stuff for duke production test

Author:

    Jason Gould (jgould) Aug 2000

Modified: 
	Jeff Keely		december 2000  -added mu code
	Jason Gould		january 2001   -added comments, renamed variables


--*/

#include "xtl.h"
#include <stdio.h>
#include "dukeprod.h"
#include "ccl.h"
#include "device.h"
#include "draw.h"
#include "mmu.h"
#include "Tsc.h"

#define countof(_x_) (sizeof(_x_) / sizeof(_x_[0]))

BOXSTATUS gPadStatus[MAX_SLOTS];

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

    DebugPrint("Welcome to Duke Production Test: Built on %s at %s\n", __DATE__,__TIME__);

    //Initialize core peripheral port support
    XInitDevices(0,NULL);
    

	gpInit(0);
	drInit();
	muInit();

	for(i = 0; i < MAX_SLOTS; i++) {
		gPadStatus[i] = notyet;
	}
	
	TscInit(0);

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

//	gModeFunctions[gMode](STATUS_LEAVE, 0);
//	for(int i = 0; i < MAX_MODES; i++) {
//		gModeFunctions[i](STATUS_QUIT, 0);
//	}

	gpDeinit();
	drDeinit();
	muDeinit();

	DebugPrint("Deinit is finished.\n");
	
}


/*************************************************************************************
Function:	wWinMain (...)
Purpose:	the entry-point of cpx. Calls init, does the main loop, then de-inits
Params:		irrelevant
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/
//DWORD WINAPI TestMain()

gpGAMEPAD * gData;

typedef struct {
	BOXFUNC BoxFunc;
	float secs;
	BOXSTATUS stat;
} TESTSTRUCT;

TESTSTRUCT tests[] = {
	{LeftTriggerTest     ,20, notyet},
	{RightTriggerTest    ,20, notyet},
	{LeftThumbStick1Test ,20, notyet},
	{DPadTest            ,20, notyet},
	{StartSelectTest     ,20, notyet},
	{RightThumbStick1Test,20, notyet},
	{ButtonTest          ,20, notyet}
};


bool muTop, muBottom;

BOXSTATUS test(int slot)
{
	int i, j;
	__int64 tim;
	BOXSTATUS overall = passed;
	muTop = true;
	muBottom = true;

	gData = ::gpGetState(slot);
	for(i = 0; i < countof(tests); i++) {
		tests[i].stat = notyet;
		tests[i].BoxFunc(tests[i].stat);
	}


	/*
	Please pardon the odd code here. 
	It was originally designed to run simultaneously with other tests that also used d3d.
	Since d3d isn't multithreaded, all drawing routine calls had to occur in the same thread,
	yet had to occur nearly simultaneously with other test code - Hence the repeated calling 
	& returning from functions...
  	*/ 
	
	for(i = 0; i < countof(tests); i++) {
		tim = TscBegin();
		tests[i].stat = testme;
		while(1) {
			drCls();

			gpUpdateDeviceStateInfo();				//update the button state info...
			if(!::gpDeviceExists(slot))	{
				return failed;
			}
			muGetDeviceMessages();

			gpPoll();
			gData = ::gpGetState(slot);

			for(j = 0; j < countof(tests); j++) {
				tests[j].stat = tests[j].BoxFunc(tests[j].stat);
			}

            /*
			int d1 = muGetDriveFromPortSlot(slot, XDEVICE_TOP_SLOT);
			if(d1 != 0) muTop = true;
			int d2 = muGetDriveFromPortSlot(slot, XDEVICE_BOTTOM_SLOT);
			if(d2 != 0) muBottom = true;
            */

			drBox(230,20,330,50, muTop ? 0xff00ff00 : 0xfff0f0f0);
			drBox(230,60,330,90, muBottom ? 0xff00ff00 : 0xfff0f0f0);

			DWORD r;
			BYTE data;
			XINPUT_FEEDBACK feedback;
			memset(&feedback, 0, sizeof(XINPUT_FEEDBACK));
			if(tests[0].stat == testme)	// if LeftTriggerTest is going, rumble left
			{
				data = gData->bAnalogButtons[GPL];

				DebugPrint("data=%d \n", data);

				feedback.Rumble.wLeftMotorSpeed= (data<<8) +data;
				r = gpInputSetState(slot, &feedback);
				if(r != ERROR_IO_PENDING)
					DebugPrint("gpInputSetState returned error %d\n", r);
				while(feedback.Header.dwStatus == ERROR_IO_PENDING)
					;
				if(feedback.Header.dwStatus != ERROR_SUCCESS)
					DebugPrint("Error: %d\n", feedback.Header.dwStatus);
			}
			if(tests[1].stat == testme)	// if RightTriggerTest is going, rumble right
			{
				data = gData->bAnalogButtons[GPR];
				feedback.Rumble.wRightMotorSpeed= (data<<8) +data;
				r = gpInputSetState(slot, &feedback);
				if(r != ERROR_IO_PENDING)
					DebugPrint("gpInputSetState returned error %d\n", r);
				while(feedback.Header.dwStatus == ERROR_IO_PENDING)
					;
				if(feedback.Header.dwStatus != ERROR_SUCCESS)
					DebugPrint("Error: %d\n", feedback.Header.dwStatus);
			}

		//	_asm int 3;
			drShowScreen();							

			if (TscTicksToFSeconds(TscCurrentTime(tim)) > tests[i].secs) {
				tests[i].stat = failed;
			} else {
				if(tests[i].stat == passed) {
					break;
				}
			}

			if(tests[i].stat == failed) {
				overall = failed;
				break;
			}
		}
	}
	return overall;
}

//int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow)
int __cdecl main()
{
//	_asm int 3;

	int i;
	init();

	gpPoll();
	Sleep(16);

	drCls();
	drShowScreen();

	while(1) //loop until we're supposed to quit...
	{
		gpUpdateDeviceStateInfo();				//update the button state info...
		muGetDeviceMessages();

		for(i = 0; i < MAX_SLOTS; i++) {
			if(!gpDeviceExists(i)) {
				::gPadStatus[i] = notyet;
			} else if (gPadStatus[i] == notyet) {
				gPadStatus[i] = test(i);

				drCls();
				if(gPadStatus[i] == passed && muTop && muBottom) {
					//passed
					drQuad(250, 350, 0xff00ff00, 550,  50, 0xff00ff00, 600, 100, 0xff00ff00, 300, 400, 0xff00ff00);
					drQuad(250, 450, 0xff00ff00, 300, 400, 0xff00ff00, 150, 250, 0xff00ff00, 100, 300, 0xff00ff00);
				} else {
					//failed
					drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
					drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);
				}
				drShowScreen();
			}
		}
	}
//		gpPoll();
//		gGlobalCommands.CheckControls();		//see if any commands are being presseed
//		gModeFunctions[gMode](STATUS_NORMAL, 0);	//go to the appropriate mode
//		drShowScreen();							//and draw the screen
	

	//we should theoretically never get here on xbox......

	deinit();


	return 0;
}

