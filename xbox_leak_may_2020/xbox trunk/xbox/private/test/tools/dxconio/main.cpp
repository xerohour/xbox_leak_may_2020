/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Description:

    Direct3D Immediate-Mode Framework.

*******************************************************************************/

#define _DXCONIO_EXPORT_
#define DXCONIO_NO_OVERRIDES

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <d3dx8.h>
#include "d3denum.h"
#include "d3dinit.h"
#include "main.h"
#include "util.h"
#include "conio.h"
#include "dxconio.h"

//******************************************************************************
// Globals
//******************************************************************************

namespace DXCONIO {

BOOL g_bInitialized;

}

using namespace DXCONIO;

//******************************************************************************
// Functions
//******************************************************************************

//******************************************************************************
BOOL __cdecl ConsoleStartup() {

    g_bInitialized = FALSE;
    return LoadConsole();
}

//******************************************************************************
void __cdecl ConsoleShutdown() {

    UnloadConsole();
}

#ifndef UNDER_XBOX

//******************************************************************************
// DllMain
//******************************************************************************

//******************************************************************************
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {

    switch (dwReason) {

        case DLL_PROCESS_ATTACH: {

            if (!ConsoleStartup()) {
                return FALSE;
            }

            break;
        }

        case DLL_PROCESS_DETACH:

            ConsoleShutdown();
            break;
    }

    return TRUE;
}

#endif // !UNDER_XBOX
