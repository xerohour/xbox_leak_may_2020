/*++

Copyright (c) 1990-2001  Microsoft Corporation

Module Name:

    perfctr.c

Abstract:

    This module contains the Win32 Performance Counter APIs

--*/

#include "basedll.h"

__declspec(naked)
BOOL
WINAPI
QueryPerformanceCounter(
    LARGE_INTEGER *lpPerformanceCount
    )
/*++

    QueryPerformanceCounter -   provides access to a high-resolution
                                counter; frequency of this counter
                                is supplied by QueryPerformanceFrequency

        Inputs:

            lpPerformanceCount  -   a pointer to variable which
                                    will receive the counter

        Outputs:

            lpPerformanceCount  -   the current value of the counter,
                                    or 0 if it is not available

        Returns:

            TRUE if the performance counter is supported by the
            hardware, or FALSE if the performance counter is not
            supported by the hardware.


--*/
{
    __asm {
        mov     ecx, DWORD PTR [esp+4]  // ecx = lpPerformanceCount
        rdtsc
        mov     DWORD PTR [ecx], eax
        mov     DWORD PTR [ecx+4], edx
        xor     eax, eax
        inc     eax                     // return TRUE;
        ret     4
    }
}

BOOL
WINAPI
QueryPerformanceFrequency(
    LARGE_INTEGER *lpFrequency
    )
/*++

    QueryPerformanceFrequency -   provides the frequency of the high-
                                  resolution counter returned by
                                  QueryPerformanceCounter

        Inputs:

            lpFrequency         -   a pointer to variable which
                                    will receive the frequency

        Outputs:

            lpPerformanceCount  -   the frequency of the counter,
                                    or 0 if it is not available

        Returns:

            TRUE if the performance counter is supported by the
            hardware, or FALSE if the performance counter is not
            supported by the hardware.

--*/
{
    lpFrequency->QuadPart = 733333333i64;

    return TRUE;
}
