/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbbt.h

Abstract:

    BBT support header file

--*/

#pragma once

#define IRTSYS
typedef ULONG DWORD;
typedef UCHAR BYTE;

#include <stdlib.h>
#include "bbtirt.h"
#include "idf.h"

//
// We allocate 32MB of memory for collecting BBT runtime data
//
#define IRT_BUFFER_SIZE (32*1024*1024)

//
// Background sweep timer frequency (in milliseconds)
//
#define IRT_SWEEP_INTERVAL 42

//
// Saved information about client-init parameters
//
typedef struct IRTCLIENTINFO {
    const IRTP* pirtp;
    DWORD*      rgCounts;
    BYTE*       rgTobCounts;
    DWORD*      rgSeqNums;
    IDFHDR*     pidfhdr;
    DWORD       currentTime;
    DWORD       tobSweepTime;
    DWORD       copyCountsTime;
    DWORD       intervalCount;
} IRTCLIENTINFO;

//
// Round up to a power of 2
//
#define CbRound(cb, cbAlign) (((cb) + ((cbAlign)-1)) & ~((cbAlign)-1))
#define CbRoundToPage(cb)    CbRound(cb, PAGE_SIZE)

