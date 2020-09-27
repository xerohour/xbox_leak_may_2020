/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dvdlibp.cpp

Abstract:

    This module contains definition of procudures and data structures used
    by DVD playback library located in hardware key.

--*/

#include <ntos.h>
#include <xtl.h>
#include <stddef.h>

extern "C" {
#include <ldi.h>
}

#include <dm.h>
#include <xboxp.h>
#include <xbeimage.h>

#include "dvdlib.h"

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif
