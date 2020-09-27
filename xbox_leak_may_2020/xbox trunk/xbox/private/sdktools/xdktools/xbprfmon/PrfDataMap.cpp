/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    prfdatamap.cpp

Abstract:

    This module implements PerfMon's plugin that polls various performance
    counters from Xbox using debug channel

--*/

#include "xbprfmonp.h"

CPrfData g_PrfData( L"Xbox PerfMon", 0 );
