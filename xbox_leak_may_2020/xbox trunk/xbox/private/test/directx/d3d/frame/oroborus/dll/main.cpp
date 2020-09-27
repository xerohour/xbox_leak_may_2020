    /*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Author:

    Matt Bronder

Description:

    The translated inscription at the center of an Ouroboros from the
    Chrysopoeia of Cleopatra during the Alexandrian Period in Egypt reads
    "The All is One".  This test library allows Direct3D tests to all
    draw from the same device during the running of a suite rather than
    releasing an recreating the device for each individual test.  This
    allows for greater stress of the device itself.

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
// Data types
//******************************************************************************

typedef VOID (WINAPI * PTEST_STARTUP_PROC)( HANDLE );
typedef VOID (WINAPI * PTEST_CLEANUP_PROC)( VOID );
typedef BOOL (WINAPI * PTEST_DLLMAIN_PROC)( HINSTANCE, DWORD, PVOID );

//******************************************************************************
// Globals
//******************************************************************************

namespace Oroborus {
BOOL g_bInitModules = TRUE;
}

//******************************************************************************
// Function prototypes
//******************************************************************************

// Functions to obtain module entries in the export directory
// (from loader.c in the harness)
extern "C" HANDLE   FindModuleHandle(LPCWSTR ModuleName);
extern "C" PROC     FindProcAddress(PWCHAR ModuleName, PWCHAR ProcName);

LPTSTR              LoadModuleList(LPTSTR szSection);
void                IsolateModuleName(LPTSTR szLibName);
void                IsolateLibName(LPTSTR szLibName);

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

        case DLL_PROCESS_ATTACH:

            break;

        case DLL_PROCESS_DETACH:

            break;
    }

    return TRUE;
}

//******************************************************************************
void WINAPI PREPEND_MODULE(_StartTest)(HANDLE hLog) {

    HANDLE                  hMutex;
    DWORD                   dwTestMode;
    TCHAR                   szLibName[512];
    TCHAR                   szModuleName[512];
    LPTSTR                  szLibList = NULL;
    TCHAR*                  psz;
    CDisplay*               pDisplay;
    D3DCREATIONDESC         d3dcd;
    HINSTANCE               hTestLib;
    PTEST_STARTUP_PROC      pfnStartTest;
    PTEST_CLEANUP_PROC      pfnEndTest;
    D3DPRESENT_PARAMETERS   d3dpp;
    D3DMULTISAMPLE_TYPE     mst;
    DWORD                   dwValue;
    DWORD                   dwStress;
    int                     ret;

    // If we are running in stress, sleep for a short period of time
    // in order to randomize the ordering of the tests
    dwStress = GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0);
    if (dwStress == 1) {
        dwStress = TSTART_STRESS;
        Sleep(rand() % 500);
    }

    // If any other Direct3D test is running, block until it is finished
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

    // Set the test startup information
    dwTestMode = GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 1);
    if (dwTestMode == 0) {
        RegisterStartupContext(TSTART_HARNESS | TSTART_SEQUENTIAL | dwStress);
    }
    else {
        RegisterStartupContext(TSTART_HARNESS | TSTART_SEQUENTIAL | TSTART_PARALLEL | dwStress);
    }

    // Seed the pseudo-random number generator
    srand(GetTickCount());

    // Create a heap for the test
    if (!CreateHeap()) {
        return;
    }

    // Read in the list of test libs
    szLibList = LoadModuleList(TEXT("oroborus"));
    if (!szLibList) {
        ReleaseHeap();
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }

    // If this is the first time we've been called by the harness,
    // call DllMain on any modules present in the d3d test list that
    // are not present in the harness module list
    if (Oroborus::g_bInitModules) {

        PTEST_DLLMAIN_PROC  pfnDllMain;
        LPTSTR              szHarnessList;
        TCHAR*              pszH;
        HMODULE             hModule;

        // Read in the list of harness modules
        szHarnessList = LoadModuleList(TEXT("TestList"));
        if (!szHarnessList) {
            MemFree(szLibList);
            ReleaseHeap();
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
            return;
        }

        for (psz = szLibList; *psz; psz += _tcslen(psz) + 1) {

            for (pszH = szHarnessList; *pszH; pszH += _tcslen(pszH) + 1) {
                if (!_tcscmp(psz, pszH)) {
                    break;
                }
            }
            if (!*pszH) {

                _tcscpy(szModuleName, psz);

                IsolateModuleName(szModuleName);

                hModule = (HMODULE)FindModuleHandle(szModuleName);
                pfnDllMain = (PTEST_DLLMAIN_PROC)FindProcAddress(szModuleName, TEXT("DllMain"));
                if (!hModule || !pfnDllMain) {
                    DebugString(TEXT("Unable to locate test module %s in the export directory"), szModuleName);
                    __asm int 3;
                    continue;
                }

                pfnDllMain(hModule, DLL_PROCESS_ATTACH, NULL);
            }
        }

        MemFree(szHarnessList);
    }

    Oroborus::g_bInitModules = FALSE;

#ifdef DEBUG
    D3D__Parser = GetProfileInt(TEXT("oroborus.lib"), TEXT("Parser"), 0);
    D3D__SingleStepPusher = GetProfileInt(TEXT("oroborus.lib"), TEXT("SingleStepPusher"), 0);
#endif

    // Initialize creation parameters
    memset(&d3dcd, 0, sizeof(D3DCREATIONDESC));
    d3dcd.d3ddt = D3DDEVTYPE_HAL;
    d3dcd.bWindowed = TRUE;

//        d3dcd.d3ddm.Width = 640;
//        d3dcd.d3ddm.Height = 480;
//        d3dcd.d3ddm.Format = D3DFMT_UNKNOWN_C5;
//        SetRect(&d3dcd.rectWnd, 20, 20, 320, 320);
//        ##HACK
//        d3dcd.bDepthBuffer = TRUE;
    d3dcd.bDepthBuffer = FALSE;
//        #HACK END
//        d3dcd.fmtd = D3DFMT_UNKNOWN_D16;
    d3dcd.mst = D3DMULTISAMPLE_NONE;

    // Create the display
    pDisplay = new CDisplay();
    if (!pDisplay) {
        MemFree(szLibList);
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
        MemFree(szLibList);
        ReleaseHeap();
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return;
    }

    // Iterate through the lib list specified in the .ini file
    for (psz = szLibList; *psz && IsDisplayStable(); psz += _tcslen(psz) + 1) {

        _tcscpy(szLibName, psz);
        _tcscpy(szModuleName, psz);

        IsolateLibName(szLibName);
        IsolateModuleName(szModuleName);

        // Get the entry and exit points
        pfnStartTest = (PTEST_STARTUP_PROC)FindProcAddress(szModuleName, TEXT("StartTest"));
        pfnEndTest = (PTEST_CLEANUP_PROC)FindProcAddress(szModuleName, TEXT("EndTest"));
        if (!pfnStartTest || !pfnEndTest) {
            DebugString(TEXT("Unable to locate test module %s in the export directory"), szModuleName);
            __asm int 3;
            continue;
        }

        pDisplay->GetPresentParameters(&d3dpp);
        dwValue = GetProfileInt(szLibName, TEXT("DisplayModeWidth"), 0);
        if (dwValue) {
            d3dpp.BackBufferWidth = dwValue;
        }
        dwValue = GetProfileInt(szLibName, TEXT("DisplayModeHeight"), 0);
        if (dwValue) {
            d3dpp.BackBufferHeight = dwValue;
        }
        dwValue = GetProfileInt(szLibName, TEXT("DisplayModeFormat"), 0);
        if (dwValue) {
            d3dpp.BackBufferFormat = (D3DFORMAT)dwValue;
        }
        dwValue = GetProfileInt(szLibName, TEXT("DisplayModeRefresh"), 0xFFFFFFFF);
        if (dwValue != 0xFFFFFFFF) {
            d3dpp.FullScreen_RefreshRateInHz = dwValue;
        }
        dwValue = GetProfileInt(szLibName, TEXT("DisplayMultiSampleType"), 0xFFFFFFFF);
        if (dwValue != 0xFFFFFFFF) {
            d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)dwValue;
        }

        pDisplay->Reset(&d3dpp);

        // Run the test
        pfnStartTest(hLog);
        pfnEndTest();
    }

    MemFree(szLibList);

    // Clean up
    pDisplay->Release();

    // Release the heap
    ReleaseHeap();

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

//******************************************************************************
void WINAPI PREPEND_MODULE(_EndTest)(void) {
}

//******************************************************************************
LPTSTR LoadModuleList(LPTSTR szSection) {

    LPTSTR szList = NULL;
    UINT   uNumChars;
    UINT   uNumListChars = 0;

    do {
        uNumListChars += 2048;
        if (szList) {
            MemFree(szList);
        }
        szList = (LPTSTR)MemAlloc(uNumListChars * sizeof(TCHAR));
        if (!szList) {
            return NULL;
        }
        uNumChars = GetProfileSection(szSection, szList, uNumListChars);
    } while (uNumChars == uNumListChars - 2);

    return szList;
}

//******************************************************************************
void IsolateModuleName(LPTSTR szLibName) {

    TCHAR* pszC;

    // Remove comments and whitespace
    for (pszC = szLibName; *pszC && *pszC != TEXT(';'); pszC++);
    if (*pszC == TEXT(';')) *pszC = TEXT('\0');
    for (pszC--; pszC >= szLibName && *pszC == TEXT(' '); pszC--);
    pszC[1] = TEXT('\0');

    // Remove the .lib extension (if present)
    pszC -= 3;
    if (!_tcscmp(pszC, TEXT(".lib"))) {
        *pszC = TEXT('\0');
    }
}

//******************************************************************************
void IsolateLibName(LPTSTR szLibName) {

    TCHAR* pszC;

    // Remove comments and whitespace
    for (pszC = szLibName; *pszC && *pszC != TEXT(';'); pszC++);
    if (*pszC == TEXT(';')) *pszC = TEXT('\0');
    for (pszC--; pszC >= szLibName && *pszC == TEXT(' '); pszC--);
    pszC[1] = TEXT('\0');
}

//******************************************************************************
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
