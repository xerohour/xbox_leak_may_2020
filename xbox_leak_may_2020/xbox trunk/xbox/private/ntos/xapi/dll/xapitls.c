/*
 *
 * xapitls.c
 *
 * Xapi TLS data
 *
 */

#include "dllp.h"

ULONG _tls_index = 0;

/* Special symbols to mark start and end of Thread Local Storage area. */

#pragma data_seg(".tls")

char _tls_start = 0;

#pragma data_seg(".tls$ZZZ")

char _tls_end = 0;

/*
 * Mark the .tls section as discardable so that IMAGEBLD throws the
 * section (non-zero Thread Local Storage data is moved to a different
 * location in the image).
 */

#pragma comment(linker, "/SECTION:.tls,D")

#pragma data_seg(".rdata$T")

const IMAGE_TLS_DIRECTORY _tls_used =
{
        (ULONG)(ULONG_PTR) &_tls_start, // start of tls data
        (ULONG)(ULONG_PTR) &_tls_end,   // end of tls data
        (ULONG)(ULONG_PTR) &_tls_index, // address of tls_index
        (ULONG)(ULONG_PTR) NULL,        // pointer to call back array
        (ULONG) 0,                      // size of tls zero fill
        (ULONG) 0                       // characteristics
};
