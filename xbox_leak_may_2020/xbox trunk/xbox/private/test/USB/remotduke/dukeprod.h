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

//#include <windef.h>
//#include "ccl.h"
//#include "modes.h"

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


#define MAX_SLOTS XGetPortCount()
//typedef enum {none, tested} padstatus;
typedef enum {notyet, testme, pending, failed, passed} BOXSTATUS;
typedef BOXSTATUS (*BOXFUNC)(BOXSTATUS newstatus);
extern BOXSTATUS gPadStatus[MAX_SLOTS];

#include "dp.h"
#include "device.h"

extern BOXFUNC gBoxFuncs[];
extern gpGAMEPAD * gData;


#endif //!defined (__cpx_h_)

