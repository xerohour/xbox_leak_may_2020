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
    Bing Li         May 2001       -added better UI, less sensitive, print failed reason,and comments

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



gpGAMEPAD * gData;
extern HANDLE gDeviceHandle[MAX_SLOTS]; // global handles in cpx lib


typedef enum {LeftTrigger, RightTrigger, LeftThumb, DPad, StartSelect, RightThumb, Button, Slot, TimeOut} FAILEDREASON;
 
FAILEDREASON gReason;

WORD	gwControllerType;

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
	muTop = false;
	muBottom = false;
    XINPUT_CAPABILITIES capabilities;

	gData = ::gpGetState(slot);


    HANDLE hDuke = gDeviceHandle[slot];
    XInputGetCapabilities(hDuke, &capabilities);

	for(i = 0; i < countof(tests); i++) 
	{
		tests[i].stat = notyet;
		 
		tests[i].BoxFunc(tests[i].stat);
		
		// reset exceptions
		switch(i)
		{
			case 0: // left trigger
			{
				if(!capabilities.In.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
					tests[i].stat = never;
				break;
			}

			case 1: // right trigger
			{
				if(!capabilities.In.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
					tests[i].stat = never;
				break;
			}

			case 2: // Left Thumbstick
			{
				if(!capabilities.In.Gamepad.wButtons && XINPUT_GAMEPAD_LEFT_THUMB)
					tests[i].stat = never;
				break;
			}
			case 3: // DPad Test
			{
				// 0x0f is the bits for all four directions
				// only 4-direction pads will be tested
				if(!capabilities.In.Gamepad.wButtons && 0x0f)
					tests[i].stat = never;
				break;
			}
			case 4: // StartSelect
			{
				if(!capabilities.In.Gamepad.wButtons && ( XINPUT_GAMEPAD_START ||	XINPUT_GAMEPAD_BACK ))
					tests[i].stat = never;
				break;
			}
			case 5: // RightThumbstick
			{
				if(!capabilities.In.Gamepad.wButtons && XINPUT_GAMEPAD_RIGHT_THUMB)
					tests[i].stat = never;
				break;
			}
			case 6: // Buttons
			{	for(int j = 0; j < 6; j++)
				{
					if(!capabilities.In.Gamepad.bAnalogButtons[j])
						tests[i].stat = never;
				}
				break;
			}

		} // end switch(i)


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


			gwControllerType = capabilities.SubType;

			drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the color
			
			switch (gwControllerType)
			{
				case STANDARD_TYPE:
					drPrintf(200,150, L"DUKE");
					break;

				case ACHIBONO_TYPE:
					drPrintf(200,150, L"AKEBONO");
					break;

				case WHEEL_TYPE:
					drPrintf(200,150, L"ERROR!!!!!!");
					break;

				case ARCADE_STICK_TYPE:
					drPrintf(200,150, L"ERROR!!!!!!");
					break;

				default:
					drPrintf(200,150, L"ERROR!!!!!!!");
			}
		
            drSetSize(8.0, 15.0, 0xff00ff00, 0xff000000); //default color				  
			

			for(j = 0; j < countof(tests); j++) {
//				DebugPrint("draw in testme loop\n");
				tests[j].stat = tests[j].BoxFunc(tests[j].stat);
				
			}

            
			int d1 = muGetDriveFromPortSlot(slot, XDEVICE_TOP_SLOT);
			if(d1 != 0) muTop = true;
			int d2 = muGetDriveFromPortSlot(slot, XDEVICE_BOTTOM_SLOT);
			if(d2 != 0) muBottom = true;

			
            if((muTop == false) & (muBottom == false)) gReason = Slot;



			drBox(230,50,330,80, muTop ? 0xff00ff00 : 0xfff0f0f0);
			drBox(230,90,330,120, muBottom ? 0xff00ff00 : 0xfff0f0f0);
			// label boxes
			drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the color
			drPrintf(345,58, L"Slot 1");
			drPrintf(345,98, L"Slot 2");

			DWORD r;
			BYTE data;
			XINPUT_FEEDBACK feedback;
			memset(&feedback, 0, sizeof(XINPUT_FEEDBACK));
			if(tests[0].stat == testme)	// if LeftTriggerTest is going, rumble left
			{
				data = gData->bAnalogButtons[GPL];


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
				gReason = (FAILEDREASON) i;
				break;
			}
		}
	}
	return overall;
}

/*************************************************************************************
Function:	main()
Purpose:	the entry-point of cpx. Calls init, does the main loop, then de-inits
Params:		irrelevant
*************************************************************************************/
//DWORD WINAPI TestMain()
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

/*
				drCls();
				if(gPadStatus[i] == passed && muTop && muBottom) {
					//passed
					drQuad(250, 350, 0xff00ff00, 550,  50, 0xff00ff00, 600, 100, 0xff00ff00, 300, 400, 0xff00ff00);
					drQuad(250, 450, 0xff00ff00, 300, 400, 0xff00ff00, 150, 250, 0xff00ff00, 100, 300, 0xff00ff00);
				} else {
					//failed
					switch(gReason) {
					   case LeftTrigger:  drPrintf(50,30, L"LeftTrigger Failed"); break;
					   case	RightTrigger: drPrintf(50,30, L"RightTrigger Failed"); break;
					   case LeftThumb:    drPrintf(50,30, L"LeftThumb Failed");break;
					   case DPad:         drPrintf(50,30, L"DPad Failed");break; 
					   case StartSelect:  drPrintf(50,30, L"StartSelect Failed");break;
					   case RightThumb:   drPrintf(50,30, L"RightThumb Failed");break;
					   case Button:       drPrintf(50,30, L"Button Failed");break;
					   case Slot:         drPrintf(50,30, L"Slot Failed");break;
					   case TimeOut:      drPrintf(50,30, L"TimeOut");break;
                       default :          drPrintf(50,30, L"Failed with Unknown Reason");

					}

					drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
					drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);
				}
*/
				if(gPadStatus[i] == passed && muTop && muBottom) {
					//passed
					drSetSize(50, 100, 0xff00ff00, 0xff000000); //set the color
					drPrintf(240,180, L"PASSED");
					drQuad(240, 290, 0xff00ff00, 330,  220, 0xff00ff00, 340,235, 0xff00ff00, 250,300, 0xff00ff00);
					drQuad(250, 300, 0xff00ff00, 260,  290, 0xff00ff00, 230,260, 0xff00ff00, 220,270, 0xff00ff00);
				} else {
					//failed
					drSetSize(50, 100, 0xffff0000, 0xff000000); //set the color

					switch(gReason) {
					   case LeftTrigger:  drPrintf(210,180, L"LeftTrigger Failed"); break;
					   case	RightTrigger: drPrintf(210,180, L"RightTrigger Failed"); break;
					   case LeftThumb:    drPrintf(210,180, L"LeftThumb Failed");break;
					   case DPad:         drPrintf(210,180, L"DPad Failed");break; 
					   case StartSelect:  drPrintf(210,180, L"StartSelect Failed");break;
					   case RightThumb:   drPrintf(210,180, L"RightThumb Failed");break;
					   case Button:       drPrintf(210,180, L"Button Failed");break;
					   case Slot:         drPrintf(210,180, L"Slot Failed");break;
					   case TimeOut:      drPrintf(210,180, L"TimeOut");break;
                       default :          drPrintf(210,180, L"Failed with Unknown Reason");

					}

					drQuad(230, 295, 0xffff0000, 335,  220, 0xffff0000, 340, 225, 0xffff0000, 235, 300, 0xffff0000);
					drQuad(335, 300, 0xffff0000, 340,  295, 0xffff0000, 235, 220, 0xffff0000, 230, 225, 0xffff0000);
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

