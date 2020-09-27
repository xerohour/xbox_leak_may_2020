/*
    FILE:   osmode.c
    DATE:   4/8/99

    This file has os-specific entry points for the modeset DLLs.
*/

#include "cmntypes.h"
#include "mywin.h"
#include "debug.h"

// 16bit Windows DLLs screw up if there is not at least one piece of
// data in the data segment. This is just to make sure there is one.
int     dummy;


/*
    DllEntryPoint

    Purpose:    This function is called by the OS when the 16bit Dll
                loads. We don't need to do anything in this routine
                right now. It is just a stub function.

    Arguments:  OS defined arguments.
                dwReason    -
                hInstance   - The module handle
                wDS         - Data segment for this module
                wHeapSize   - Size specified in the .def file for the heap
                dwReserved1 - not used
                wReserved2  - not used

    Returns:    Always return TRUE
*/
int WINAPI __export DllEntryPoint
(
    ULONG   dwReason,
    USHORT  hInstance,
    USHORT  wDS,
    USHORT  wHeapSize,
    ULONG   dwReserved1,
    USHORT  wReserved2
)
{
    DBG_PRINT0(DL_0, "\r\nDllEntryPoint for 16bit ModeSet DLL called -- Dll Loaded.");
    return  (TRUE);
}


/*
    WEP

    Purpose:    This function is called by the OS when the 16bit Dll
                unloads. We don't need to do anything in this routine
                right now. It is just a stub function.

    Arguments:  OS defined arguments.
                nExitType   - why the Dll is unloading

    Returns:    Always return TRUE
*/
int WINAPI WEP(int nExitType)
{
    DBG_PRINT0(DL_0, "\r\nWEP for 16bit ModeSet DLL called -- Dll Unloaded.");
	return	(TRUE);
}

