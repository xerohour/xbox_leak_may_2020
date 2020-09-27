/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    device.h

Abstract:

    function declarations for device.cpp

Author:

    Jason Gould (a-jasgou)   May 2000

Revision History:

--*/
#ifndef __device_h_
#define __device_h_

//#include "xbox.h"      //we do need this to access the input format data.

//#define _LogStuff_	//define this to enable logging of control pads

//#include "cpx.h"
// #define HUMAN_READABLE_LOG //uncomment this to produce a "pretty" log file

#define AGE(slot) ((TscTicksToUSeconds(TscCurrentTime(gTimeStart)) - gLastUpdate[slot]))	//get the age of the last update of a slot

#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XINPUT_GAMEPAD_START            0x00000010
#define XINPUT_GAMEPAD_BACK             0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

typedef struct gpGAMEPAD{
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;
} gpGAMEPAD, *gpPGAMEPAD;

typedef void (*gpLOGFUNC)(UINT64 lastupdate, int slot, gpGAMEPAD * data, DWORD packet);

#define GPA                0
#define GPB                1
#define GPX                2
#define GPY                3
#define GPK                4
#define GPW                5
#define GPL                6
#define GPR                7


/*void BulkStorDeviceChangeProc(
	IN PCBULKSTOR_DEVICE_INFORMATION pdi,
	IN BOOL fAdd, 
	IN BOOL fEnum, 
	IN PVOID pvContext);
*/

INT64 gpTSecondAge(int slot);
void gpPoll();
void gpUpdateDeviceStateInfo();
DWORD gpInputSetState(int slot, IN OUT PXINPUT_FEEDBACK Feedback);
DWORD gpInit(BOOL DoMapping);
VOID gpDeinit();
_declspec(dllexport) BOOL __inline gpDeviceExists(int slot);
BOOL gpSetLogFunction(gpLOGFUNC func);
_declspec(dllexport) gpPGAMEPAD __inline gpGetState(int slot);
_declspec(dllexport) int __inline gpMaxSlots();

//*******************GLOBAL DUKE DATA*************************************************
//extern DWORD gDeviceHandle[MAX_SLOTS];				//array of deivce handles
//extern gpGAMEPAD gInputFormat[MAX_SLOTS];		//holds the 26 bytes of button states, etc.
//extern DWORD gPacket[MAX_SLOTS];                    //packet #
//extern UINT64 gLastUpdate[MAX_SLOTS];				//the last update of each slot
//extern UINT64 gTimeStart;							//beginning of time for this program
	
//*******************GLOBAL MU DATA***************************************************
//extern BULKSTOR_DEVICE_INFORMATION gMui[MAX_MUS];	//array of memory unit info
//extern DWORD gMuHandle[MAX_MUS];					//array of memory unit handles
//extern BYTE gMuID[MAX_MUS];						//IDs of the enumerated memory units


#endif
