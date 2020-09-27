/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    This module contains simple routines to poll and save coverage logging
    data to disk

Author:

    Sakphong Chanbai (schanbai) 22-Aug-2000

Environment:

    Xbox

Revision History:

--*/


#ifndef __COVERMON_H__
#define __COVERMON_H__

#include <nt.h>
#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>

#include <xtl.h>
#include <winioctl.h>
#include <xlog.h>

#include <stdio.h>

#include "coverdll.h"
#include "coversys.h"
#include "covfile.h"
#include "bbreg.h"

#ifndef bool
#define bool BOOL
#endif

#ifndef true
#define true TRUE
#endif

#ifndef false
#define false FALSE
#endif

#define COVERMON_POOLTAG 'rvoC'

ULONG
DebugPrint(
    PCHAR Format,
    ...
    );

#endif // __COVERMON_H__
