/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dvdkeyp.cpp

Abstract:

    This module contains DVD playback libraries for the Xbox Dashboard.
    The code is actually located in hardware key.

--*/

#include <ntos.h>
#include <xtl.h>
#include <xboxp.h>
#include <xcrypt.h>

#include "dvdlib.h"

#ifndef IS_ERROR
#define IS_ERROR(Status) ((unsigned long)(Status) >> 31 == SEVERITY_ERROR)
#endif

