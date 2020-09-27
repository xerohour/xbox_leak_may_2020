/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbprfdll.cpp

Abstract:

    This module implements PerfMon's plugin that polls various performance
    counters from Xbox using debug channel

--*/

#include "xbprfmonp.h"

#if !defined(PRFDATA_COLLECT_SUPPORTED)
#error PRFDATA_COLLECT_SUPPORTED must be defined for this project
#endif

BOOL
WINAPI
DllMain(
    HINSTANCE hInstance,
    DWORD     dwReason,
    LPVOID    /*lpReserved*/
    )
{
    if ( dwReason == DLL_PROCESS_ATTACH ) {
        DisableThreadLibraryCalls( hInstance );
        return CPrfData::Activate() == ERROR_SUCCESS;
    }

    return TRUE;
}
