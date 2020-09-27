/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    precomp.h

Abstract:

    Pre-compile header file

--*/

#define _XBDM_ // we do this so we can poke at some debugger flags in the kernel

#include <ntos.h>
#include <bldr.h>
#include <bldr32.h>
#include <xapip.h>
#include <xboxp.h>
#include <xboxverp.h>

#include <stdio.h>
#include <xtl.h>
#include "xtestlib.h"
#include "xlog.h"
#include "harnessapi.h"

#define HARNESS_POOLTAG     'nraH'
#define ARRAY_SIZE( a )     (sizeof(a)/sizeof((a)[0]))
