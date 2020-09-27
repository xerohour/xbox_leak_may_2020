/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    crt.c

Abstract:

    This module contains minimum C runtime library startup for Ravisent's
    DVD playback library.

--*/

#include "dvdkeyp.h"

#if 0

ULONG _tls_index = 0;

//
// Special symbols to mark start and end of Thread Local Storage area.
//

#pragma data_seg(".tls")

char _tls_start = 0;

#pragma data_seg(".tls$ZZZ")

char _tls_end = 0;

//
// Start and end sections for Thread Local Storage CallBack Array.
// Actual array is constructed using .CRT$XLA, .CRT$XLC, .CRT$XLL,
// .CRT$XLU, .CRT$XLZ similar to the way global static initializers
// are done for C++.
//

#pragma data_seg(".CRT$XLA")

PIMAGE_TLS_CALLBACK __xl_a = 0;

#pragma data_seg(".CRT$XLZ")

PIMAGE_TLS_CALLBACK __xl_z = 0;

#pragma data_seg(".rdata$T")

#ifndef IMAGE_SCN_SCALE_INDEX
#define IMAGE_SCN_SCALE_INDEX   0x00000001  // Tls index is scaled
#endif

const IMAGE_TLS_DIRECTORY _tls_used =
{
    (ULONG)(ULONG_PTR)&_tls_start,  // start of tls data
    (ULONG)(ULONG_PTR)&_tls_end,    // end of tls data
    (ULONG)(ULONG_PTR)&_tls_index,  // address of tls_index
    (ULONG)(ULONG_PTR)&__xl_a,      // pointer to call back array
    (ULONG)0,                       // size of tls zero fill
    (ULONG)0                        // characteristics
};

#endif // 0

typedef void (__cdecl *PFV)(void);

#pragma data_seg(".CRT$XCA")
PFV __xc_a[] = { NULL };

#pragma data_seg(".CRT$XCZ")
PFV __xc_z[] = { NULL };

#pragma data_seg(".CRT$XIA")
PFV __xi_a[] = { NULL };

#pragma data_seg(".CRT$XIZ")
PFV __xi_z[] = { NULL };

#pragma data_seg(".CRT$RIA")
PFV __xri_a[] = { NULL };

#pragma data_seg(".CRT$RIZ")
PFV __xri_z[] = { NULL };

#if DBG
#pragma data_seg(".CRT$RII15")
extern void __cdecl _RTC_Initialize(void);
void *__rtc_init = _RTC_Initialize;
#endif

#pragma data_seg()

PFV _FPinit;

#pragma comment(linker, "/merge:.CRT=.data")

//
// This data is required for floating-point support
//

int _fltused = 0x9875;

void _initterm(PFV *a, PFV *z)
{
    for (; a < z; ++a)
        if(*a != NULL && *a != (PFV)-1)
            (**a)();
}

void _cinit(void)
{
    // Init floating point
    if (_FPinit) {
        (*_FPinit)();
    }

    // Do the initializers
    _initterm(__xi_a, __xi_z);

    // Now do the constructors
    _initterm(__xc_a, __xc_z);
}

void _rtinit(void)
{
    // Do the CRT initializers
    _initterm(__xri_a, __xri_z);
}
