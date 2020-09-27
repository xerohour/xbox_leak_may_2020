/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Author:

    Matt Bronder

Description:

    Application entry point.

*******************************************************************************/

#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <xgraphics.h>
#include <xgmath.h>
#include "util.h"

using namespace XBOX11113;

#include "textr.h"

#ifdef HARNESS

#include "xtestlib.h"

#define PREPEND_MODULE(Suffix) xbox11113##Suffix
#define MODULE_STRING "xbox11113"

//******************************************************************************
// Function exports to the harness
//******************************************************************************

BOOL WINAPI PREPEND_MODULE(_DllMain)(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved);
void WINAPI PREPEND_MODULE(_StartTest)(HANDLE hLog);
void WINAPI PREPEND_MODULE(_EndTest)(void);

#pragma data_seg(EXPORT_SECTION_NAME)
extern EXP_TABLE_ENTRY PREPEND_MODULE(_ExportTableEntry)[];
__EXTRN_C EXP_TABLE_DIRECTORY PREPEND_MODULE(_ExportTableDirectory) = {
    MODULE_STRING,
    PREPEND_MODULE(_ExportTableEntry)
};
#pragma data_seg()

__EXTRN_C EXP_TABLE_ENTRY PREPEND_MODULE(_ExportTableEntry)[] = {
    { "StartTest", (PROC)PREPEND_MODULE(_StartTest) },
    { "EndTest",   (PROC)PREPEND_MODULE(_EndTest) },
    { "DllMain",   (PROC)PREPEND_MODULE(_DllMain) },
    { NULL, NULL }
};

#pragma data_seg()

#else

//******************************************************************************
// Function prototypes
//******************************************************************************

typedef enum _FIRMWARE_REENTRY {
    HalHaltRoutine,
    HalRebootRoutine,
    HalQuickRebootRoutine,
    HalKdRebootRoutine,
    HalMaximumRoutine
} FIRMWARE_REENTRY, *PFIRMWARE_REENTRY;

extern "C" VOID HalReturnToFirmware(IN FIRMWARE_REENTRY Routine);

#endif // HARNESS

//******************************************************************************
// main
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     main
//
// Description:
//
//     Entry point for the application.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
#ifdef HARNESS
void WINAPI PREPEND_MODULE(_StartTest)(HANDLE hLog)
#else
void __cdecl main()
#endif // HARNESS
{
    CSample* pSample;
    UINT uStart;
    TCHAR szOut[64];
    UINT uPass;

#ifdef HARNESS

    HANDLE hMutex;

    // If any other Direct3D test is running, block until it is finished
    hMutex = CreateMutex(NULL, FALSE, "XBox Direct3D Test");
    if (!hMutex) {
        OutputDebugString(TEXT("CreateMutex failed\n"));
        return;
    }

    if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) {
        OutputDebugString(TEXT("The test mutex was abandoned\n"));
        CloseHandle(hMutex);
        return;
    }

    OutputDebugString(TEXT("GRAPHICS: Entering test ") TEXT(MODULE_STRING) TEXT("\r\n"));

#endif

    // Seed the pseudo-random number generator
    srand(GetTickCount());

    for (uPass = 0, uStart = GetTickCount(); ; uPass++) {
 
        pSample = new CSample();

        if (pSample) {

            if (pSample->Create()) {

                pSample->Render();
            }

            delete pSample;
        }

        if (!(uPass % 200)) {
            wsprintf(szOut, TEXT("TEXTR4: %d seconds have elapsed\n"), (GetTickCount() - uStart) / 1000);
            OutputDebugString(szOut);
        }
    }

#ifdef HARNESS
    OutputDebugString(TEXT("GRAPHICS: Leaving test ") TEXT(MODULE_STRING) TEXT("\r\n"));
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
#else
    HalReturnToFirmware(HalRebootRoutine);
#endif
}

#ifdef HARNESS

//******************************************************************************
BOOL WINAPI PREPEND_MODULE(_DllMain)(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {

    switch (dwReason) {

        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

//******************************************************************************
void WINAPI PREPEND_MODULE(_EndTest)(void) {
}

#endif // HARNESS
