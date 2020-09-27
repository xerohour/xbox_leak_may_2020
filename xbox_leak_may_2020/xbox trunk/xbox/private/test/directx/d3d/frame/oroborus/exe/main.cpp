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

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include "xtestlib.h"
#include "profilep.h"
#endif // UNDER_XBOX

//******************************************************************************
// Data types
//******************************************************************************

typedef VOID (WINAPI * PTEST_STARTUP_PROC)( HANDLE );
typedef VOID (WINAPI * PTEST_CLEANUP_PROC)( VOID );
typedef BOOL (WINAPI * PTEST_DLLMAIN_PROC)( HINSTANCE, DWORD, PVOID );

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
// WinMain
//******************************************************************************

#ifndef UNDER_XBOX
#define RETURN(x)   return x
#else
#define RETURN(x)   return
#endif // UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     WinMain
//
// Description:
//
//     Entry point for the application.
//
// Arguments:
//
//     HINSTANCE hInstance      - Application instance handle
//
//     HINSTANCE hPrevInstance  - Previous instance of the application (always
//                                NULL for Win32 applications)
//
//     LPSTR szCmdLine          - Pointer to a string specifying the command
//                                line used in launching the application
//                                (excluding the program name)
//
//     int nCmdShow             - State specifying how the window is to be 
//                                displayed on creation
//
// Return Value:
//
//     0 on success, -1 on failure.
//
//******************************************************************************
#ifndef UNDER_XBOX
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow) 
#else
void __cdecl main()
#endif // UNDER_XBOX
{
    DWORD                   dwTestMode;
    DWORD                   dwContext = TSTART_SEQUENTIAL;
    TCHAR                   szLibName[512];
    TCHAR                   szModuleName[512];
    LPTSTR                  szLibList = NULL;
    TCHAR*                  psz;
    UINT                    uNumChars;
    CDisplay*               pDisplay;
    D3DCREATIONDESC         d3dcd;
    HINSTANCE               hTestLib;
    PTEST_STARTUP_PROC      pfnStartTest;
    PTEST_CLEANUP_PROC      pfnEndTest;
    D3DPRESENT_PARAMETERS   d3dpp;
    D3DMULTISAMPLE_TYPE     mst;
    DWORD                   dwValue;
    BOOL                    bDisplayOK = TRUE;
    int                     ret;

    // Set the test startup information
    dwTestMode = GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0);
    if (dwTestMode == 1) {
        dwContext |= TSTART_STRESS;
    }

    dwTestMode = GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 1);
    if (dwTestMode != 0) {
        dwContext |= TSTART_PARALLEL;
    }

    RegisterStartupContext(dwContext);

    // Seed the pseudo-random number generator
    srand(GetTickCount());

    // Create a heap for the test
    if (!CreateHeap()) {
        RETURN(0);
    }

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
        ReleaseHeap();
        RETURN(0);
    }

    // Disable DirectInput if we are running in parallel
    if (dwTestMode != 0) {
        pDisplay->EnableInput(FALSE);
    }

    // Initialize the display
    if (!pDisplay->Create(&d3dcd)) {
        DebugString(TEXT("CDisplay::Create failed"));
        pDisplay->Release();
        ReleaseHeap();
        RETURN(0);
    }

    // Read in the list of test libs
    szLibList = LoadModuleList(TEXT("oroborus"));
    if (!szLibList) {
        pDisplay->Release();
        ReleaseHeap();
        RETURN(0);
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
        pfnStartTest(NULL);
        pfnEndTest();
    }

    MemFree(szLibList);

    // Clean up
    pDisplay->Release();

    // Release the heap
    ReleaseHeap();

    RETURN(0);
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

