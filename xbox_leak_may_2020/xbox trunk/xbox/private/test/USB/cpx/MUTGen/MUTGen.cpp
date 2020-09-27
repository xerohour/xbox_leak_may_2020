/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    MUTGen.cpp

Abstract:

    

Author:

    Dennis Krueger (a-denkru)

Environment:

    XBox

Comments:

  Note that a single read buffer is being shared by all the read/write threads.  This is in order to
    minimize memory commitment.  If r/w comparison is needed, seperate buffers will be needed

  Formatted MUs must be used.  XMount failure is not reported to the user.

  Only first 3 MUs can be used for testing due to memory requirements and screen formatting.

  L trigger + R trigger + black is used to abort processing

Revision History:
    07-21-2001  Created


*/

#include <xtl.h>
#include <stdio.h>
#include <process.h>
#include "MUTGen.h"
#include "draw.h"
#include "Tsc.h"


int ScreenInfo[4][2][2] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // ports,slots,read/write counters

bool fExitFlags[4][2] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
bool fRunning[4][2] = {0,0,0,0,0,0,0,0};

HANDLE hDuke[4] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[4][2] = {{ NULL, NULL},{ NULL, NULL}, { NULL, NULL},{ NULL, NULL }};



void _cdecl main(void)
{
 
	DWORD addDevice,removeDevice,dwResult,size;
	UINT port;
	Initialize();
    DebugPrint("\r\n\r\nTest App Started (Built on " __DATE__ " at " __TIME__ ")\r\n");
	bool fResult;

	drCls();

	drPrintf(200,40,L"Xbox MU Traffic Generator 1.00");

	drShowScreen();
 
    char *wbuffer = new char[MEGABYTE*3];
    if(!wbuffer)
    {
        DebugPrint("Unable to allocate memory\r\n");
		drCls();
		drPrintf(150,150,L"ERROR:  MEMORY ALLOCATION FAILURE!! - Quitting");
		drShowScreen();
		Sleep(10000);
        return;
    }

	drCls();

	drPrintf(200,40,L"Xbox MU Exercise Version 1.00");

	drPrintf(100,100,L"Initializing...");

	drShowScreen();

    memset(wbuffer, 0, MEGABYTE*2);
    unsigned count = 1;
    for(size=0; size<MEGABYTE*2; size+=64)
    {
        (*(unsigned*)&wbuffer[size]) = count++;
    }
    wbuffer[MEGABYTE*2-1] = 0x0F;
    wbuffer[MEGABYTE*2-2] = 0x0F;
    wbuffer[MEGABYTE*2-3] = 0x0F;
    wbuffer[MEGABYTE*2-4] = 0x00;
    wbuffer[MEGABYTE*2-5] = 0x01;
    wbuffer[MEGABYTE*2-6] = 0x01;
    wbuffer[MEGABYTE*2-7] = 0x01;
    wbuffer[MEGABYTE*2-8] = 0x00;
    wbuffer[MEGABYTE*2-9] = 0x0F;
    wbuffer[MEGABYTE*2-10] = 0x0F;
    wbuffer[MEGABYTE*2-11] = 0x0F;

    while(!InputCheckExitCombo())
    {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
		for(port=0; port<4; port++)
		{
			bool fRun = FALSE;
			if(hDuke[port])
			{
				// check for MU
				char drive = 0x00;
				UINT SlotNumber = 0;
				// check first MU
				if(hSlot[port][SlotNumber] && !fRunning[port][SlotNumber])
				{
					fRun = TRUE;
					
				} else if(hSlot[port][++SlotNumber] && !fRunning[port][SlotNumber]) // check 2nd MU
				{
					fRun = TRUE;
					
				}
				if(fRun == TRUE)
				{
					// got an idle MU
					// start dodiskio thread
					PTHREAD_PARAMS pParams = (PTHREAD_PARAMS) malloc(sizeof(THREAD_PARAMS));
					if(!pParams)
					{
						DebugPrint("Error:  Thread Params Malloc failed\n");
						break;
					}
					pParams->drive = (char) hSlot[port][SlotNumber];
					pParams->port  = port;
					pParams->slot  = SlotNumber;
					pParams->pSourceBuffer = wbuffer;
					fRunning[port][SlotNumber] = true;
					_beginthread((void (__cdecl*)(void *))DoDiskIO, NULL, pParams);
				}

			}
		} // for

		UpdateScreen();

	} // while

    XLaunchNewImage(NULL, NULL);
}

void UpdateScreen()
{
	float LineInc;
	drCls(); // clear screen
	drPrintf(200,40,L"Xbox MU Traffic Generator 1.00");
	drPrintf(180,60,L"L Trigger + R Trigger + black to Quit");
	// check each port
	for(int port = 0;port<XGetPortCount(); port++)
	{
		LineInc = 100 + (float) (100 * port);
		if(fRunning[port][0] || fRunning[port][1])
		{
			drPrintf(60,LineInc,L"GamePad %d",port);
			if(fRunning[port][0])
			{
				LineInc+=20;
				drPrintf(100,LineInc,L"MU 0");
				LineInc+=20;
				drPrintf(120,LineInc,L"Write Passes = %d",ScreenInfo[port][0][0]);
				drPrintf(300,LineInc,L"Read Passes  = %d",ScreenInfo[port][0][1]);
			}
			if(fRunning[port][1])
			{
				LineInc+=20;
				drPrintf(100,LineInc/*+(60*(float)port)*/,L"MU 1");
				LineInc+=20;
				drPrintf(120,LineInc/*+(60*(float)port)*/,L"Write Passes = %d",ScreenInfo[port][1][0]);
				drPrintf(300,LineInc/*+(60*(float)port)*/,L"Read Passes  = %d",ScreenInfo[port][1][1]);
			}
		}
	}
	drShowScreen();
}


void Initialize(void)
{
    DWORD error;

    //
    // Misc
    //
    DWORD dataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    error = XGetLaunchInfo(&dataType, &launchInfo);

    //
    // USB
    //
    XInitDevices(0, NULL);

	// initialize graphics support

	drInit();

	TscInit(0);

}

bool InputCheckExitCombo(void)
    {
    XINPUT_STATE state;

    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(hDuke[port])
            {
            XInputGetState(hDuke[port], &state);
            if(state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 128 &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 128 &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 128)
                {
                DebugPrint("\r\n\r\nFound Exit Combo on gamepad %u - rebooting\r\n", port+1);
                return true;
                }
            }
        }

    return false;
    }


void InputDukeInsertions(DWORD add, DWORD remove/*=0*/)
    {
    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(remove & USBPortMasks[port])
            {
            if(hDuke[port]) XInputClose(hDuke[port]);
            hDuke[port] = NULL;
            }
        if(add & USBPortMasks[port])
            {
            hDuke[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, 0, NULL);
            DebugPrint("Adding Duke in Port %u\n", port+1);
            }
        }
    }

void InputMUInsertions(DWORD add, DWORD remove/*=0*/)
{
    for(unsigned i=0; i<XGetPortCount()*2; i++)
    {
        DWORD port = i/2;
        DWORD slot = i%2==0?XDEVICE_BOTTOM_SLOT:XDEVICE_TOP_SLOT;
        if(remove & USBSlotMasks[i])
            {
            if(hSlot[port][slot]) XUnmountMU(XMUPortFromDriveLetter((char)hSlot[port][slot]), XMUSlotFromDriveLetter((char)hSlot[port][slot]));
            hSlot[port][slot] = (HANDLE) NULL;
            }
        if(add & USBSlotMasks[i])
        {
            char drive;
            if(XMountMU(port, slot, &drive) == ERROR_SUCCESS)
            {
                hSlot[port][slot] = (HANDLE)drive;
                DebugPrint("Adding MU in Port %u, Slot %u\n", XMUPortFromDriveLetter((char)hSlot[port][slot]), XMUSlotFromDriveLetter((char)hSlot[port][slot])+1);
            }
        }
     }
}

void DoDiskIO(PTHREAD_PARAMS pParams)
    {
    // DISK "IN/OUT" Test for hardware guys
    bool exit = false;
	char cInputDrive = pParams->drive;
	int port = pParams->port;
	int slot = pParams->slot;
    DWORD wSize, rSize,size;
    char *wbuffer = pParams->pSourceBuffer;
 
    char filename[64];
    char drive;

	size = MEGABYTE*2;
    if(!cInputDrive)
    {
        drive = 'F';
    }
    else
    {
        drive = cInputDrive;
    }
    sprintf(filename, "%c:\\IOtest.txt", drive);

    HANDLE hFile = CreateFile(filename, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        DebugPrint("Unable to open file: %s (ec: %ld)\r\n", filename, GetLastError());
        return ;
    }

    DebugPrint("Writing initial file: %s...\r\n", filename);

    if(! (WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size))
    {
        DebugPrint("Unable to write the file (bytes: %u) (ec: %ld)\r\n", wSize, GetLastError());
        CloseHandle(hFile);
        return ;
    }
    int iPasscounter = 0;

    DebugPrint("Running DISKIO \r\n");

    while(!exit)
    {
 		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) != TRUE)
		{
            DebugPrint("Error: WriteFile[%d] (ec: %ld)\r\n", wSize, GetLastError());
			break;
		}
		// update counter for screen display
		ScreenInfo[port][slot][0]++;
		
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

		if(ReadFile(hFile, wbuffer, size, &rSize, NULL) != TRUE)
		{
            DebugPrint("Error: ReadFile[%d] (ec: %ld)\r\n", rSize, GetLastError());
			break;
		}
		ScreenInfo[port][slot][1]++;

 

        // check for user interaction
        exit = fExitFlags[port][slot];
	}

    CloseHandle(hFile);
	fRunning[port][slot] = FALSE; // signal end of running state
	ScreenInfo[port][slot][0] = 0; // clear w counter
	ScreenInfo[port][slot][1] = 0; // clear r counter
	_endthread();
	return;
}