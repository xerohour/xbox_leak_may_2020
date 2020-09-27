/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Author:

    Matt Bronder

Description:

    Direct3D Immediate-Mode Framework.

*******************************************************************************/

#define DXCONIO_NO_OVERRIDES

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include "xtestlib.h"
#include "profilep.h"
#endif // UNDER_XBOX

#ifdef DEBUG
extern "C" {
extern BOOL D3D__SingleStepPusher;
extern BOOL D3D__Parser;
}
#endif

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

#pragma data_seg(D3D_SECTION_NAME)
extern "C" int PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int *pnExitCode);
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm);
D3DTESTFUNCTIONDESC PREPEND_MODULE(_d3dtfd) = {
    TEXT(MODULE_STRING),
    PREPEND_MODULE(_ValidateDisplay),
    PREPEND_MODULE(_ExhibitScene)
};
#pragma data_seg()

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
BOOL WINAPI PREPEND_MODULE(_DllMain)(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {

    switch (dwReason) {

        case DLL_PROCESS_ATTACH: {

            // Allow global access to the instance handle
#ifndef UNDER_XBOX
            RegisterTestInstance(hInstance);
#endif // !UNDER_XBOX

            break;
        }

        case DLL_PROCESS_DETACH:

            break;
    }

    return TRUE;
}

//******************************************************************************
void WINAPI PREPEND_MODULE(_StartTest)(HANDLE hLog) {

    PD3DTESTFUNCTIONDESC    pd3dtfd;
    HANDLE                  hMutex;
    DWORD                   dwStress;
    DWORD                   dwTestMode;
    CDisplay*               pDisplay;
    D3DCREATIONDESC         d3dcd;
    LPTSTR                  pszLibName;
    DWORD                   dwValue;
    DWORD                   dwSetting;
    BOOL                    bDisplayOK;
    TCHAR                   szTest[96];
    int                     ret;

    // Find the test module
    for (pd3dtfd = (&D3D_BeginTestFunctionTable)+1;
        pd3dtfd < &D3D_EndTestFunctionTable; pd3dtfd++)
    {
        if (!_tcscmp(pd3dtfd->szModule, TEXT(MODULE_STRING))) {
            break;
        }
    }

    // Return immediately if the test wasn't found
    if (pd3dtfd == &D3D_EndTestFunctionTable) {
        return;
    }

    // If we are running in stress, sleep for a short period of time
    // in order to randomize the ordering of the tests
    dwStress = GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0xFFFFFFFF);
    if (dwStress == 0xFFFFFFFF) {
        return;
    }
    if (dwStress == 1) {
        dwStress = TSTART_STRESS;
        srand(GetTickCount());
        Sleep(rand() % 10000);
    }

    // If any other Direct3D test is running, block until it is finished
    // (if this test was started by oroborus, we are in the same
    // thread and will simply increment the ref count on the mutex
    // and continue)
#ifndef UNDER_XBOX
    hMutex = CreateMutex(NULL, FALSE, TEXT("XBox Direct3D Test"));
#else
    hMutex = CreateMutex(NULL, FALSE, "XBox Direct3D Test");
#endif // !UNDER_XBOX
    if (!hMutex) {
        OutputDebugString(TEXT("CreateMutex failed"));
        return;
    }

    if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) {
        OutputDebugString(TEXT("The test mutex was abandoned"));
        CloseHandle(hMutex);
        return;
    }

    wsprintf(szTest, TEXT("GRAPHICS: Entering test ") TEXT(MODULE_STRING) TEXT(" with stress == 0x%X\r\n"), dwStress);
    OutputDebugString(szTest);

#ifdef DEBUG
    D3D__Parser = GetProfileInt(TEXT("GeneralSettings"), TEXT("Parser"), 0);
    D3D__SingleStepPusher = GetProfileInt(TEXT("GeneralSettings"), TEXT("SingleStepPusher"), 0);
#endif

    VerifyTextureListIntegrity();

    // If the test has been loaded by oroborus, it will supply the display
    pDisplay = GetDisplay();
    if (pDisplay) {

        // Initialize logging
        CreateLog(hLog);

        // Render the 3D scene
        bDisplayOK = pd3dtfd->pfnExhibit(pDisplay, &ret);

        // Stop logging
        ReleaseLog();

        EnableDisplayStability(bDisplayOK);

        ReleaseMutex(hMutex);
        CloseHandle(hMutex);

        return;
    }

    // Register the test instance
#ifdef UNDER_XBOX
    RegisterTestInstance((HINSTANCE)&(PREPEND_MODULE(_d3dtfd)));
#endif // UNDER_XBOX

    // Set the test startup information
    dwTestMode = GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 0xFFFFFFFF);
    if (dwTestMode == 0xFFFFFFFF) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }
    else if (dwTestMode == 0) {
        RegisterStartupContext(TSTART_HARNESS | TSTART_SEQUENTIAL | dwStress);
    }
    else {
        RegisterStartupContext(TSTART_HARNESS | TSTART_SEQUENTIAL | TSTART_PARALLEL | dwStress);
    }

    // Seed the pseudo-random number generator
    srand(GetTickCount());

    // Create a heap for the test
    if (!CreateHeap()) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }

    // Initialize logging
    CreateLog(hLog);

    // Set the validation procedure for the device
    InitCheckDisplayProc(pd3dtfd->pfnValidate);

    // Initialize creation parameters
    memset(&d3dcd, 0, sizeof(D3DCREATIONDESC));
    d3dcd.d3ddt = D3DDEVTYPE_HAL;
    d3dcd.bWindowed = FALSE;
    d3dcd.d3ddm.Width = 640;
    d3dcd.d3ddm.Height = 480;
    d3dcd.d3ddm.Format = D3DFMT_X8R8G8B8;
    d3dcd.bDepthBuffer = TRUE;
    d3dcd.fmtd = D3DFMT_D24S8;
    d3dcd.mst = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
    d3dcd.bAntialias = TRUE;
    d3dcd.uPresentInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    pszLibName = (LPTSTR)MemAlloc(MAX_PATH * sizeof(TCHAR));
    if (!pszLibName) {
        ReleaseLog();
        ReleaseHeap();
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }

    if (GetModuleName(GetTestInstance(), pszLibName, MAX_PATH)) {
        dwValue = GetProfileInt(pszLibName, TEXT("DisplayModeWidth"), 0);
        if (dwValue) {
            d3dcd.d3ddm.Width = dwValue;
        }
        dwValue = GetProfileInt(pszLibName, TEXT("DisplayModeHeight"), 0);
        if (dwValue) {
            d3dcd.d3ddm.Height = dwValue;
        }
        dwValue = GetProfileInt(pszLibName, TEXT("DisplayModeFormat"), 0);
        if (dwValue) {
            d3dcd.d3ddm.Format = (D3DFORMAT)dwValue;
        }
        dwValue = GetProfileInt(pszLibName, TEXT("DisplayModeRefresh"), 0xFFFFFFFF);
        if (dwValue != 0xFFFFFFFF) {
            d3dcd.d3ddm.RefreshRate = dwValue;
        }
        dwValue = GetProfileInt(pszLibName, TEXT("DisplayMultiSampleType"), 0xFFFFFFFF);
        if (dwValue != 0xFFFFFFFF) {
            d3dcd.mst = (D3DMULTISAMPLE_TYPE)dwValue;
        }
#ifdef DEBUG
        dwSetting = GetProfileInt(pszLibName, TEXT("Parser"), 0xFFFFFFFF);
        if (dwSetting != 0xFFFFFFFF) {
            D3D__Parser = (BOOL)dwSetting;
        }
        dwSetting = GetProfileInt(pszLibName, TEXT("SingleStepPusher"), 0xFFFFFFFF);
        if (dwSetting != 0xFFFFFFFF) {
            D3D__SingleStepPusher = (BOOL)dwSetting;
        }
#endif
    }

    MemFree(pszLibName);

    // Create the display
    pDisplay = new CDisplay();
    if (!pDisplay) {
        ReleaseLog();
        ReleaseHeap();
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }

    // Disable DirectInput if we are running in parallel
    if (dwTestMode != 0) {
        pDisplay->EnableInput(FALSE);
    }

    // Initialize the display
    if (!pDisplay->Create(&d3dcd)) {
        DebugString(TEXT("CDisplay::Create failed"));
        pDisplay->Release();
        ReleaseLog();
        ReleaseHeap();
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }

    VerifyTextureListIntegrity();

    // Render the 3D scene
    pd3dtfd->pfnExhibit(pDisplay, &ret);

    VerifyTextureListIntegrity();

    // Clean up
    pDisplay->Release();

    VerifyTextureListIntegrity();

    // Stop logging
    ReleaseLog();

    // Release the heap
    ReleaseHeap();

    VerifyTextureListIntegrity();

    wsprintf(szTest, TEXT("GRAPHICS: Leaving test ") TEXT(MODULE_STRING) TEXT("\r\n"));
    OutputDebugString(szTest);

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

//******************************************************************************
void WINAPI PREPEND_MODULE(_EndTest)(void) {
}
