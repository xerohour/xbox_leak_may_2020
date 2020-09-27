/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    mtldrp.h

Abstract:

    Pre-compile header file     for manufacturing test loader

--*/

#if _MSC_VER > 1000
#pragma once
#endif

#include <xtl.h>
#include <stdio.h>
#include "debug.h"
#include "sysload.h"

#ifndef EXTERN_C

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#endif // EXTERN_C
