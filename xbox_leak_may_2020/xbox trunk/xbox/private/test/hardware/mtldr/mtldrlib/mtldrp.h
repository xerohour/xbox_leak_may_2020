/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    mtldrp.h

Abstract:

    Pre-compile header file for mtldr library

--*/

#if _MSC_VER > 1000
#pragma once
#endif

#include <ntos.h>
#include <xtl.h>
#include <ldr.h>
#include <dm.h>
#include "sysload.h"

extern PLDR_CURRENT_IMAGE XeLdrImage;
extern IMAGE_TLS_DIRECTORY _tls_used;
