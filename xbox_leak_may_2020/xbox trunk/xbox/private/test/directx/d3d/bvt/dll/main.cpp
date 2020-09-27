/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Description:

    Direct3D Build Verification Tests.

*******************************************************************************/

#include <xtl.h>
#ifdef UNDER_XBOX
#include "xtestlib.h"
#endif // UNDER_XBOX
#include "..\log.h"

using namespace D3DBVT;

//******************************************************************************
// Globals
//******************************************************************************

HINSTANCE                   g_hInst;
LONG                        g_lNumThreads = 0;

//******************************************************************************
// Function prototypes
//******************************************************************************

extern void                 ExecuteTests(HINSTANCE hInstance, HANDLE hLog);

//******************************************************************************
// DllMain
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     DllMain
//
// Description:
//
//     Entry point for the library.
//
// Arguments:
//
//     HINSTANCE hInstance      - Library instance handle
//
//     DWORD dwReason           - Reason the function is being called
//
//     LPVOID pvReserved        - Reserved
//
// Return Value:
//
//     When DllMain is called with DLL_PROCESS_ATTACH, TRUE is returned if
//     initialization succeeded and FALSE is returned if it failed.  The
//     return value is ignored for all other reasons.
//
//******************************************************************************
BOOL WINAPI d3dbvt_DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {

    switch (dwReason) {

        case DLL_PROCESS_ATTACH: {
            // Allow global access to the instance handle
            g_hInst = hInstance;
            break;
        }

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

//******************************************************************************
void WINAPI d3dbvt_StartTest(HANDLE hLog) {

    HANDLE hMutex;

    // If any other Direct3D test is running, block until it is finished
#ifndef UNDER_XBOX
    hMutex = CreateMutex(NULL, FALSE, TEXT("XBox Direct3D Test"));
#else
    hMutex = CreateMutex(NULL, FALSE, "XBox Direct3D Test");
#endif // UNDER_XBOX
    if (!hMutex) {
        DebugString(TEXT("CreateMutex failed [%d]"), GetLastError());
        return;
    }

    if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) {
        DebugString(TEXT("The test mutex was abandoned"));
        CloseHandle(hMutex);
        return;
    }

    // Ensure no other threads can run this test
    if (InterlockedIncrement(&g_lNumThreads) == 1) {
        ExecuteTests(g_hInst, hLog);
        InterlockedDecrement(&g_lNumThreads);
    }
    else {
        OutputDebugString(TEXT("The Direct3D BVTs are single-threaded only\n"));
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

//******************************************************************************
void WINAPI d3dbvt_EndTest(void) {
}

#pragma data_seg(EXPORT_SECTION_NAME)
extern EXP_TABLE_ENTRY d3dbvt_ExportTableEntry[];
__EXTRN_C EXP_TABLE_DIRECTORY d3dbvt_ExportTableDirectory = {
    "d3dbvt",
    d3dbvt_ExportTableEntry
};
#pragma data_seg()

__EXTRN_C EXP_TABLE_ENTRY d3dbvt_ExportTableEntry[] = {
    { "StartTest", (PROC)d3dbvt_StartTest },
    { "EndTest",   (PROC)d3dbvt_EndTest },
    { "DllMain",   (PROC)d3dbvt_DllMain },
    { NULL, NULL }
};
