/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    device.cpp

Abstract:

    all functions necessary to get input about control pads and MUs for CPX

Author:

    Jason Gould (a-jasgou) May 2000

--*/

#include <xtl.h>
//#include <stdio.h>
#include <time.h>			//for keeping track of how old device states are
//#include "xinput.h"			//Mitch's input (control pad, etc) routines

#include "tsc.h"

#include "device.h"
#define MAX_SLOTS (XGetPortCount())

#define BADSLOT (-1)

extern "C" ULONG DebugPrint(PCHAR Format, ...);	//prints stuff to the debugger computer


//*******************GLOBAL DUKE DATA*************************************************
HANDLE					gDeviceHandle	[MAX_SLOTS];		//array of deivce handles
static XINPUT_STATE				gInputFormat	[MAX_SLOTS];		//holds the 26 bytes of button states, etc.
static int						gRealSlot		[MAX_SLOTS];
static gpLOGFUNC				gLogFunc					= NULL;
//*******************time stuff*******************************************************
       UINT64					gLastUpdate		[MAX_SLOTS];			//last update of each slot
       UINT64					gTimeStart;								//beginning of time for this program

static BOOL						gDoMapping = 0;


INT64 gpTSecondAge(int slot) { return AGE(slot) / 100000; }


///*************************************************************************************
//Function:	gpPoll
//Purpose:	Poll all devices
//Params:	none
//*************************************************************************************/
void gpPoll()
{

	for(int i=0;i<MAX_SLOTS; i++) {
		if(gDeviceHandle[i]) {
			XInputPoll(gDeviceHandle[i]);	//poll all devices...
		}
	}

}



/*************************************************************************************
Function:	gpGetDeviceMessages
Purpose:	deal with messaging system to figure out what devices are inserted/removed
Params:		none
Return:		none
*************************************************************************************/
void gpGetDeviceMessages()
{
	DWORD insert;
	DWORD remove;
	XINPUT_POLLING_PARAMETERS pp = {FALSE, TRUE, 0, 1, 1, 0};

	int i, j;
	int temp;

	if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insert, &remove)) {
		for (i = 0; i < MAX_SLOTS; i++) {
			if(remove & (1 << i)) { 
				if(gDoMapping) {
					for(j = 0; j < MAX_SLOTS; j++) {
						if(gRealSlot[j] == i) {
							DebugPrint("Device removed from 'slot' %d, which is really %d \n", j, i);
							XInputClose(gDeviceHandle[j]);
							gDeviceHandle[j] = 0;
							gRealSlot[j] = BADSLOT;
							gLastUpdate[j] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
							break;
						}
						if(j >= MAX_SLOTS - 1) { 
							DebugPrint("Device removed from real slot %d, but not found!\n", i);
						}
					}
				} else {
					DebugPrint("Device removed from slot %d.\n", i);
					gLastUpdate[i] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
					if(gDeviceHandle[i]) XInputClose(gDeviceHandle[i]);
					gDeviceHandle[i] = 0;
					gRealSlot[i] = BADSLOT;
				}
			}
			if(insert & (1 << i)) {
				if(gDoMapping) {
					for(j = 0; j < MAX_SLOTS; j++) {
						if(gRealSlot[j] == BADSLOT) {
							gLastUpdate[j] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
							DebugPrint("Device inserted to %d, mapped to %d\n", i, j);
							gDeviceHandle[j] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, &pp);
							if (gDeviceHandle[j] == NULL) {
								DebugPrint("XInputOpenGameController returned %d!!!", GetLastError());
							} else {
								gRealSlot[j] = i;
							}
							break;
						}
						if(j == MAX_SLOTS) {
							DebugPrint("Device inserted to slot %d, no avalible positions??", i);
							for(j = 0; j < MAX_SLOTS; j++) {
								DebugPrint(" %d", gRealSlot[j]);
							}
							break;
						}
					}
				} else {
					gLastUpdate[i] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
					DebugPrint("Device inserted in slot %d\n", i);
					gDeviceHandle[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, &pp);
					if (NULL == gDeviceHandle[i]) {
						DebugPrint("XInputOpen returned %d!!!", GetLastError());
					} else {
						gRealSlot[i] = i;
					}
				}
			}
		}
	}
}

/*************************************************************************************
Function:	gpUpdateDeviceStateInfo
Purpose:	update all info for all plugged-in devices
Params:		none
*************************************************************************************/

void gpUpdateDeviceStateInfo()
{
	DWORD r;
	int i;
	const unsigned char * pc;
	int c;

	gpGetDeviceMessages();

	for(BYTE slot = 0; slot < MAX_SLOTS; slot++)	//go through all slots...
	{
		if(gDeviceHandle[slot] != NULL)	//get info if the device is plugged in
		{
			DWORD packet = gInputFormat[slot].dwPacketNumber;
			r = XInputGetState(gDeviceHandle[slot], &gInputFormat[slot]);
			if(ERROR_SUCCESS != r) {
				DebugPrint("InputGetDeviceState returned %d for slot %d.\n", r, slot);
				return;
			}

			if(packet != gInputFormat[slot].dwPacketNumber) {
				if(gLogFunc) {
					gLastUpdate[slot] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
					gLogFunc(gLastUpdate[slot], slot, (gpGAMEPAD*)&gInputFormat[slot].Gamepad, gInputFormat[slot].dwPacketNumber);
				}

			}
		}
	}
}

/*************************************************************************************
Function:	gpInputSetState
Purpose:	set the input state (Rumble, etc...)
*************************************************************************************/
DWORD gpInputSetState(int slot, IN OUT PXINPUT_FEEDBACK Feedback)
{
	return XInputSetState(gDeviceHandle[slot], Feedback);
}

/*************************************************************************************
Function:	gpInit
Purpose:	initialize the device stuff in this file
Params:		none
*************************************************************************************/
DWORD gpInit(BOOL DoMapping)
{
	int i;

	gpDeinit();

	gDoMapping = DoMapping;

	TscInit(0);
	gTimeStart = TscBegin();

	return 0;
}

/*************************************************************************************
Function:	gpDeinit
Purpose:	deinitialize stuff
Params:		none
*************************************************************************************/
VOID gpDeinit()
{
	int i;

	for(i = 0; i < MAX_SLOTS; i++) {
		if(gDeviceHandle[i]) {
			DebugPrint("Closing controller %d\n", i);
			XInputClose(gDeviceHandle[i]);
			gDeviceHandle[i] = 0;
		}
		gRealSlot[i] = BADSLOT;
	}


}

/*************************************************************************************
Function:	gpDeviceExists
Purpose:	return TRUE if a device in the given slot is plugged in
Params:		slot ---
return:		TRUE or FALSE.
*************************************************************************************/
_declspec(dllexport) BOOL __inline gpDeviceExists(int slot)
{
	return (gDeviceHandle[slot] ? TRUE : FALSE);
}


/*************************************************************************************
Function:	gpSetLogFunction
Purpose:	set the log function to a user-defined routine
Params:		func --- the function to call, or NULL if nothing to call
*************************************************************************************/
BOOL gpSetLogFunction(gpLOGFUNC func) 
{
	gLogFunc = func;
	return TRUE;
}

_declspec(dllexport) gpPGAMEPAD __inline gpGetState(int slot)
{
	return (gpGAMEPAD*)&gInputFormat[slot].Gamepad;
}

_declspec(dllexport) int __inline gpMaxSlots() { return MAX_SLOTS; }

