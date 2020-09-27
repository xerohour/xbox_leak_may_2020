/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    ccl.cpp

Abstract:

    defines all methods of the class CL, which deals with lists of controls on CPX,
	and also the functions they use

Author:

    Jason Gould (a-jasgou) May 2000

--*/
//#include "cpx.h"

//#define UNICODE

#include <xtl.h>
#include <stdio.h>
#include "device.h"
#include "ccl.h"

BYTE gDontRead	= 93;	//the index of a controlpad to not read
//extern "C" ULONG DebugPrint(PCHAR Format, ...);	//prints stuff to the debugger computer

/*************************************************************************************
Method:     CL::RegisterControl
Purpose:	add a button combination and related function to call to the control list
Params:		but --- holds the combo of buttons that should call this function
			func --- the function to call when the button combo is pressed
			context --- a variable to pass func when the button combo is pressed
Return:     0 if it worked, non-zero if there was a problem
*************************************************************************************/
DWORD CL::RegisterControl (BUTTONS but, ControlFunc func, DWORD context)
{
//	DebugPrint("Entering RegisterControl, now walking controls list...\n");
	for(CF * w = &head; w->next; w=w->next) //go to the end of the list
	{}
//	DebugPrint("Done walking the list...\n");

	w->next = new CF; //crate a new element in the list
	if(!w->next)      //if it couldn't be created... that's bad.
		return 1;

	w=w->next;        //move to the new element
	w->but = but;       //set the data appropriately
	w->func = func;
	w->context = context;
	w->next = 0;
	w->repeat = 0;
	return 0;
}

/*************************************************************************************
Method:     CL::CL
Purpose:	initialize an instance of a CL class
Params:     none
Return:     none
*************************************************************************************/
CL::CL()  //initialization
{
	head.next = 0; 
	num = 0;
}

/*************************************************************************************
Method:     CL::~CL
Purpose:	deinitialize an instance of a CL class--deallocate the list of commands
Params:     none
Return:     none
*************************************************************************************/
CL::~CL() //deinitialization
{
	CF * w, *w2;
	for(w = head.next; w; w = w2)  //remove all elements from the list
	{
		w2 = w->next;
		delete w;
	}
	head.next = 0;
	num = 0;
}

/*************************************************************************************
Function:   GetButtons
Purpose:	get the button states of the control pad at a specific slot
Params:     slot --- the slot to read
Return:     a Buttons union that uses a lot of flags to check if any button is pressed
*************************************************************************************/
BUTTONS GetButtons(DWORD slot) 
{
	WORD d;
	BUTTONS r = {0};
	gpPGAMEPAD pgp;

	if(!gpDeviceExists(slot)) {
		r.all = 0;
		return r;
	} else {
		pgp = ::gpGetState(slot);
	}
	
	r.Ab0 = pgp->bAnalogButtons[0] >= 1;
	r.Ab1 = pgp->bAnalogButtons[1] >= 1;
	r.Ab2 = pgp->bAnalogButtons[2] >= 1;
	r.Ab3 = pgp->bAnalogButtons[3] >= 1;
	r.Ab4 = pgp->bAnalogButtons[4] >= 1;
	r.Ab5 = pgp->bAnalogButtons[5] >= 1;
	r.Ab6 = pgp->bAnalogButtons[6] >= 1;
	r.Ab7 = pgp->bAnalogButtons[7] >= 1;

	r.Db = pgp->wButtons;

//	d = gInputFormat[slot].GameReport.wDPad;

	//bugbug hammerhead doesn't report this properly... 
	//remove this when real duke comes!
	//also, hammerhead dpad doesn't go to the middle... 
	//it thinks it's a steering wheel.
/*
	d *= 10;	

	if(d >= 36000) {
		r.DpDn = r.DpUp = r.DpLt = r.DpRt = 0;
	} else {
		r.DpDn = (d > 9000 + 2250 && d < 27000 - 2250);
		r.DpLt = (d > 18000 + 2250 && d < 36000 - 2250);
		r.DpRt = (d > 2250 && d < 18000 - 2250);
		r.DpUp = (d > 27000 + 2250 || d < 9000 - 2250);
	}
*/
	r.LtUp = (pgp->sThumbLY > 0);
	r.LtDn = (pgp->sThumbLY < 0);
	r.LtLt = (pgp->sThumbLX < 0);
	r.LtRt = (pgp->sThumbLX > 0);

	r.RtUp = (pgp->sThumbRY > 0);
	r.RtDn = (pgp->sThumbRY < 0);
	r.RtLt = (pgp->sThumbRX < 0);
	r.RtRt = (pgp->sThumbRX > 0);

//	DebugPrint("Slot: %d, r: %016I64X\n", slot, r.all);

	return r;

}

/*************************************************************************************
Method:     CL::CheckControls
Purpose:	see if any control pattern is being pressed, and call the appropriate 
			function if it is
Params:     none
Return:     the number of functions called
*************************************************************************************/
DWORD CL::CheckControls()
{
	int count = 0;
	int count2;
	for(CF * w = head.next; w; w = w->next)
	{
		count2 = count;
		for(int i = 0; i < gpMaxSlots(); i++)
		{
			if(i == gDontRead) {
				continue;
			}

			if (cclCheckButtons(i, w->but.all)) {
				count++;
				if(w->func) {
					w->func(w->context, w->repeat, (BYTE)i);
					w->repeat = TRUE;
				}
			}
		}
		if (count2 == count) {
			w->repeat = FALSE;
		}
	}

	num = count;

	return count;
}

/*************************************************************************************
Function:   cclCheckButtons
Purpose:	see if the specified pattern is being pressed on the specified control pad
Params:     slot --- the slot to check for button presses
			b --- the button pattern to check for
Return:     TRUE if it is being pressed, FALSE otherwise
*************************************************************************************/
BOOL cclCheckButtons(int slot, UINT64 all)
{
	if(slot == -1) {
		for(int i = 0; i < gpMaxSlots(); i++) {
			if(cclCheckButtons(i, all)) {
				return TRUE;
			}
		}
		return FALSE;
	}

	if(all & bANY) {
		if((GetButtons(slot).all & all) != 0) {
			return TRUE;
		}
	} else {
		if((GetButtons(slot).all & all) == all) {
			return TRUE;
		}
	}
	return FALSE;
}