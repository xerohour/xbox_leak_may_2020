/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVDETECT.CPP                                                      *
*   Detects CPU family and capabilities                                     *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 10/20/98 - wrote it                     *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "nvutil.h"
#include "nvdetect.h"

/*
 * macros
 */
#define LATCH_VALUE(x,y)    if (fabs(((x)-(y))/(y)) < 0.05) return (DWORD)(y)


/*
 * nvGetCPUSpeed
 *
 * detects CPU speed
 */
__int64 _clock_ticks;
DWORD nvGetCPUSpeed
(
    void
)
{
    double      speed;
    DWORD       dwCaps;
    SYSTEM_INFO si;

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return 1; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return 1; // not pentium

    __asm
    {
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx
    }
    if (!(dwCaps & 0x00000010)) return 1; // does not have tsc


    SetPriorityClass  (GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    GetTickCount(); // break in cache
    {
        DWORD start,stop;

        start = GetTickCount();
        __asm {
            pushad

            // rdtsc
            _emit 0x0f
            _emit 0x31
            mov ebp,eax
            mov ebx,edx
#ifdef NV_PROFILE
            mov ecx,8000000h
#else
            mov ecx,1000000h
#endif
        here:
            loop here

            // rdtsc
            _emit 0x0f
            _emit 0x31
            sub eax,ebp
            sbb edx,ebx

            mov [dword ptr _clock_ticks  ],eax
            mov [dword ptr _clock_ticks+4],edx

            popad
        }
        stop = GetTickCount();
        speed = ((double)_clock_ticks)/(1000.0*(double)(stop-start));
    }
    SetPriorityClass  (GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_NORMAL);

// normalize to known values
    LATCH_VALUE (speed, 90);
    LATCH_VALUE (speed,100);
    LATCH_VALUE (speed,166);
    LATCH_VALUE (speed,200);
    LATCH_VALUE (speed,233);
    LATCH_VALUE (speed,266);
    LATCH_VALUE (speed,300);
    LATCH_VALUE (speed,333);
    LATCH_VALUE (speed,400);
    LATCH_VALUE (speed,450);
    LATCH_VALUE (speed,500);
    LATCH_VALUE (speed,550);
    LATCH_VALUE (speed,600);
// did not latch - return unaltered (can be up to 10% off)
    return (DWORD)speed;
}

/*
 * nvDetectMMX
 *
 * detect MMX capability
 */
BOOL nvDetectMMX
(
    void
)
{
    DWORD       dwCaps;
    SYSTEM_INFO si;

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return FALSE; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return FALSE; // not pentium

    __asm
    {
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx
    }

    return (dwCaps & 0x00800000) ? TRUE : FALSE;
}

/*
 * nvDetectKATMAI
 *
 * detect P3 capability and OS support for it
 */
BOOL nvDetectKATMAI
(
    void
)
{
    DWORD         dwCaps;
    SYSTEM_INFO   si;
    OSVERSIONINFO osvi;
    BOOL          bIsWindows98orLater;

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return FALSE; // unknown
    if (si.dwProcessorType != PROCESSOR_INTEL_PENTIUM) return FALSE; // not pentium

    __asm
    {
        mov eax,1
        // cpuid
        _emit 0x0f
        _emit 0xa2
        mov [dwCaps],edx

/*
        //mov eax,cr4
        _emit 0x0f
        _emit 0x20
        _emit 0xe0
        mov [dwCR4],eax
*/
    }

    /*
     * detect at least win 98
     */
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx (&osvi);
    bIsWindows98orLater = (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
                       && ((osvi.dwMajorVersion > 4)
                        || ((osvi.dwMajorVersion == 4)
                         && (osvi.dwMinorVersion > 0)));

    return ((dwCaps & 0x02000000) && bIsWindows98orLater/*&& (dwCR4 & 0x200)*/) ? TRUE : FALSE;
}

