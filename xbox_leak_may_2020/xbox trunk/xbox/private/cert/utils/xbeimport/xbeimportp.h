/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbeimportp.h

Abstract:

    This is a pre-compiled header module for xbeimport utility.

--*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <xbeimage.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#ifndef ROUND_TO_PAGES
#define ROUND_TO_PAGES(Size) (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#endif
