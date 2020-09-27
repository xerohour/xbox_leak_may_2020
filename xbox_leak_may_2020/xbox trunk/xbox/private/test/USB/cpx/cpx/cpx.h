/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    cpx.h

Abstract:

    function declarations, type definitions, includes, etc, for Control Panel X

Author:

    Jason Gould (a-jasgou)   May 2000

Revision History:

--*/
#ifndef __cpx_h_
#define __cpx_h_

#include <xtl.h>
//#include "ccl.h"
#include "modes.h"

#ifndef UNICODE
#define UNICODE
#endif

#ifdef __cplusplus
extern "C" {
#endif

int DebugPrint(char* format, ...);

#ifdef __cplusplus
}
#endif

//the various modes... mainly used when switching control-pads
#define CONTEXT_GLOBAL       0	
#define CONTEXT_DUKEDISPLAY  1
#define CONTEXT_DUKECHECK    2 
#define CONTEXT_MEMORYFILE   3
#define CONTEXT_MEMORYSECTOR 4

//#define MAX_MODES   4
extern int MAX_MODES;

//*******************OTHER GLOBAL DATA************************************************
extern BOOL gQuit;									//should we quit now?
extern DWORD gMode;									//current mode (duke test, duke display, mem sector, mem file)
//extern CL gGlobalCommands;							//commands used globally: quit, switch mode, switch controller
extern ModeFunc gModeFunctions[];					//the main function for each mode
extern BYTE gCurrent;								//the index of the device we're looking at

#endif //!defined (__cpx_h_)

