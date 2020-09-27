/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Implementation of IDirectInput8 for XBOX, 
	contains init code, guids and new and delete.

Environment:

	XAPI

Revision History:

    05-10-00 : created

--*/
#define DEBUG_MODULE_NAME  _MODULE_DI_MISC_
#define DEBUG_TRACE_NAME "DI"
#include <initguid.h>
#include "dinputi.h"
DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

BOOL WINAPI
DllMain(
    IN HINSTANCE hinstDll,
    IN DWORD fdwReason,
    IN LPVOID lpvReserved
    )

/*++

Routine Description:

    DLL entrypoint - refer to XAPI SDK documentation

--*/

{
    //TRACE(("Entering DINPUT.DLL entrypoint: %d\n", fdwReason));
    switch (fdwReason)
	{
	    case DLL_PROCESS_ATTACH:
	    case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
	    case DLL_PROCESS_DETACH:
		;
    }
    return TRUE;
}

/*++
 Implementation of new and delete in-terms of LocalAlloc and FreeAlloc,
 we should revisit where this memory comes from
--*/

void * _cdecl operator new(size_t size)
{
	if(0==size) size = 1;
	return LocalAlloc(LMEM_FIXED, size);
}

void _cdecl operator delete(void *pvMem)
{
	if(pvMem) LocalFree(pvMem);
}