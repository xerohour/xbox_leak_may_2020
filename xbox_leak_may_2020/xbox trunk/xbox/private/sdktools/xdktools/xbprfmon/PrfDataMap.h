/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    prfdatamap.h

Abstract:

    This module implements PerfMon's plugin that polls various performance
    counters from Xbox using debug channel

--*/

#include "PrfData.h"

//
// ID of string resource
//

#define IDS_USAGE       0x100
#define IDS_PER_SECOND  0x101

//
// ID of dialog
//

#define IDD_CONNECTING  0x200
#define IDC_STATIC_TEXT 0x201
