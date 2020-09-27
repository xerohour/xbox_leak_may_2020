/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    5lines.cpp

Abstract:

    Displays the 5 lines of info about Control Pads and MUs for Control Panel X

Author:

    Jason Gould (a-jasgou) May 2000

--*/
//Sample:
//"Pt ID Sz Btns b0 b1 b2 b3 b4 b5 b6 b7 Tp1x Tp1y Tp2x Tp2y S1 S2      TSec"
//"00 00 00 000C 00 10 20 30 40 50 60 70 7800 2900 8300 8100 02 00        74"
//"01 00 00 0010 00 10 20 30 40 50 60 70 8100 5E00 8100 8000 00 02         1"
//"02  ---No Device---                                                 12364"
//"03 00 00 0007 00 10 20 30 40 50 60 70 8700 7F00 2100 5600 03 02       100"

//#include "cpx.h"
#include <xtl.h>
#include <stdio.h>
#include "device.h"
#include "draw.h"
#include "mmu.h"

/*************************************************************************************
Function:	FormatString
Purpose:	set up a string that has all of the button-states, and other info about a given Duke
Params:		str is a pointer to the string to fill
			slot is the slot to get info about
*************************************************************************************/
VOID 
FormatString(
	OUT TCHAR * str, 
	IN	BYTE	slot
	)
{
	gpPGAMEPAD f;			//used to hold the info from the device
	TCHAR t[189] = {0};		//temporary string 
	
	f = gpGetState(slot);

	if(!gpDeviceExists(slot)) {					//if the device isn't plugged in ---
		swprintf(str,L"%02x  ---No Device---                                        %16I64d", 
			slot, gpTSecondAge(slot));
		return;
	} else {										//if there is a device there ---
		PUCHAR p = f->bAnalogButtons;

		int d1 = muGetDriveFromPortSlot(slot, XDEVICE_TOP_SLOT) ? 2 : 0;
		int d2 = muGetDriveFromPortSlot(slot, XDEVICE_BOTTOM_SLOT) ? 2 : 0;

		swprintf(str, 
			L"%02x %02x %02x %04x %02x %02x %02x %02x %02x %02x %02x %02x "     \
			L"%04hx %04hx %04hx %04hx %02x %02x %11d",
			slot, 0, sizeof(*f),
			f->wButtons, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
			f->sThumbLX,f->sThumbLY,f->sThumbRX,f->sThumbRY,
			d1, d2, gpTSecondAge(slot));

	}
}

/*************************************************************************************
Function:	Display5Lines
Purpose:	set up and display the 5 lines of info about the control pads and MUs
Params:		none
Notes:		Until d3d is ported, we're logging this, which also DebugPrints it, but it 
			isn't displayed on the test machine.
*************************************************************************************/
VOID 
Display5Lines(
	)
{
	
	TCHAR str[189];
	drPrintf(0,0,L"Pt ID Sz Btns b0 b1 b2 b3 b4 b5 b6 b7 TpLx TpLy TpRx TpRy S1 S2        TSec");

	for(BYTE i = 0; i < gpMaxSlots(); i++)
	{
		FormatString(str, i);	//display the current button states on the top of the screen
		drPrintf(0, (float)(16*i + 16), str); 
	}
	
}
