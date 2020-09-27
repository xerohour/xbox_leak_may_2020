/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    dukeprod.cpp

Abstract:

    initializes all global variables, contains main loop, init, and stuff for duke production test

Author:

    Jason Gould (jgould) Aug 2000

--*/

#include "xtl.h"
#include <stdio.h>
#include "muprod.h"
//#include "ccl.h"
#include "draw.h"
#include "Tsc.h"

#define MAX_MUS 32

BOXSTATUS gMuStatus[MAX_MUS];

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
	DWORD insert;

    DebugPrint("Welcome to MU Production Test: Built on %s at %s\n", __DATE__,__TIME__);

    //Initialize core peripheral port support
    XInitDevices(0,NULL);

	drInit();

	TscInit(0);

	DebugPrint("Getting MU devices...\n");
	insert = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

	for(i = 0; i < MAX_MUS; i++) {
		if(insert & (1 << i)) {
			gMuStatus[i] = testme;
		} else {
			gMuStatus[i] = notyet;
		}
	}

	DebugPrint("Done with initialization!\n");
}

void CheckMus()
{
	DWORD insert, remove;
	int i;
	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insert, &remove)) {
		for (i = 0; i < MAX_MUS; i++) {
			if(remove & (1 << i)) { 
				gMuStatus[i] = notyet;
			}

			if(insert & (1 << i)) {
				gMuStatus[i] = testme;
			}
		}
	}
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

	drDeinit();

	DebugPrint("Deinit is finished.\n");

	_asm int 3;
}


/*************************************************************************************
Function:	wWinMain (...)
Purpose:	the entry-point of cpx. Calls init, does the main loop, then de-inits
Params:		irrelevant
Notes:		until d3d is ported, it can't display the console...
*************************************************************************************/
//int __cdecl main(){return 0;}
//DWORD WINAPI TestMain()


BOXSTATUS test(int port, int slot, int &BB1, int &BB2, int &BB3)
{
	__int64 tim;
	CHAR drive;
	BOXSTATUS overall = passed;
	int temp;

    if(failed == LowLevelTest(port, slot, BB1, BB2, BB3))
    {
        overall = failed;
		goto nothing;
    }

	if(failed == FormatTest(port, slot)) {
		overall = failed;
		goto nothing;
	}

	if(failed == VerifyTest(port, slot)) {
		overall = failed;
		goto nothing;
	}

	temp = XMountMU(port, slot, &drive);
	DebugPrint("XMountMU: %d -- ", temp);
	DebugPrint("Drive: %c\n", drive);
	if(temp != 0) {
		DebugPrint("XMountMU failed: 0x%x\n", GetLastError());
		overall = failed;
		goto nothing;
	}

	Sleep(1000);

	if(failed == CountSectorWriteTest(drive)) {
		DebugPrint("CountSectorWriteTest(%c:) failed.\n", drive);
		overall = failed;
		goto unmount;
	}

	Sleep(1000);

	if(failed == WriteReadTest(drive)) {
		DebugPrint("WriteReadTest(%c:) failed.\n", drive);
		overall = failed;
		goto unmount;
	}


	overall = passed;

unmount:
	temp = XUnmountMU(port, slot);
	DebugPrint("XUnmountMU: %d\n", temp);
	if(temp != 0) {
		overall = failed;
	}


	if(failed == FormatTest(port, slot)) {
		overall = failed;
		goto nothing;
	}

	if(failed == VerifyTest(port, slot)) {
		overall = failed;
		goto nothing;
	}




nothing:
	return overall;
}

void __cdecl main()
{
//	_asm int 3;

	int i,t,p,f,BB1 = 0,BB2 = 0, BB3 = 0;
	init();
	int port = 0, slot = 0;

	Sleep(16);

	drCls();
	drPrintf(0,0,L"Xbox MU Test Version 1.09 - MU ASIC!");
	drShowScreen();

	while(1) //loop until we're supposed to quit...
	{
		CheckMus();
		for(i = 0; i < MAX_MUS; i++) {
			port = i &  15;
			slot = i < 16 ? 0 : 1;

			if(gMuStatus[i] == testme) {
						DebugPrint("Testing MU at port %d, slot %d\n", port, slot);

						drCls();
						drPrintf(0,0,L"Xbox MU Test Version 1.09 - MU ASIC!");
						drPrintf(0,20,L"Running test...");
						drShowScreen();
						gMuStatus[i] = test(port, slot, BB1, BB2, BB3);
						drCls();
						if(gMuStatus[i] == passed) {
							DebugPrint("Test passed.\n");
							drQuad(250, 350, 0xff00ff00, 550,  50, 0xff00ff00, 600, 100, 0xff00ff00, 300, 400, 0xff00ff00);
							drQuad(250, 450, 0xff00ff00, 300, 400, 0xff00ff00, 150, 250, 0xff00ff00, 100, 300, 0xff00ff00);
						} else {
							DebugPrint("Test failed.\n");
							drQuad(200, 400, 0xffff0000, 550,  50, 0xffff0000, 600, 100, 0xffff0000, 250, 450, 0xffff0000);
							drQuad(600, 400, 0xffff0000, 250,  50, 0xffff0000, 200, 100, 0xffff0000, 550, 450, 0xffff0000);
						}
						drPrintf(0,20,L"First bad block count   %d", BB1);
						drPrintf(0,40,L"Second bad block count  %d", BB2);
						drPrintf(0,60,L"Third bad block count   %d", BB3);
						drShowScreen();
			}
		}
	}

	//we should theoretically never get here on xbox......
	deinit();
}

