/*++

Copyright (c) Microsoft Corporation

Module Name:

    dmxbox.h

Abstract:
    
    Declarations for the Xbox Debug Monitor Plug-in.
    dmxbox plugs directly into the em without a real
    transport layer.
    
--*/

#ifndef _DMXBOX_H_
#define _DMXBOX_H_

//-----------------------------------------------------------------
//  Compile time constants and Macros
//-----------------------------------------------------------------
#define WAIT_FOR_REBOOT_TIMEOUT 12000 //Wait up to 12 seconds for a box to reboot
#define WAIT_FOR_LAUNCH_TIMEOUT 60000 //Wait up to 60 seconds for title to hit initial breakpoint (after DmGo)

//-----------------------------------------------------------------
//  Globals used by the Xbox Debug Monitor.
//-----------------------------------------------------------------
struct DM_GLOBALS
{
    //Communicate with EM
    TLCALLBACKTYPE   pfnCallEm;                                // Function to call EM
    PFNEMGETWBBUFFER pfnGetEmWBBuffer;                         // Function to get thread specific WB buffer from EM
    LPDBF            pDebugFunctions;                          // Debug Helper Functions
    LONG             cbEmWBBuffer;                             // Size of buffers EM returns from pfnGetEmWBBuffer. 
    char             bReceiveBuffer[INITIAL_DMSENDBUFFERSIZE]; // Buffer for storing replies from the EM.
};
extern DM_GLOBALS g_DmGlobals;

//-----------------------------------------------------------------
//  Exported Methods
//-----------------------------------------------------------------
int  WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, DWORD dwReserved);
XOSD WINAPI TLFunc(TLF wCommand, HPID hpid, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------
//  Internal Methods
//-----------------------------------------------------------------
XOSD DMInit(TLCALLBACKTYPE pfnEntryPoint, LPDBF pdbf);
XOSD DispatchRequest(LPDBB lpdbb, DWORD cb);
void GetDebuggerInfo(OUT DMINFO *pDebuggerInfo);

// Process Management (process.cpp)
void ProcessSpawnOprhanCmd(LPDBB lpdbb);
HRESULT ProcessSetLaunchTitle(PROGLOAD *pProgramLoadInfo);
HRESULT EnsurePendingExecState();

//Notification Management (notify.cpp)
HRESULT StopNotifications();

#endif // _DMXBOX_H_

