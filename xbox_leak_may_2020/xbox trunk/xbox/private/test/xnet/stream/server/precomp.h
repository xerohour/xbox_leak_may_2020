/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    Pre-compiled header file

Author:

    Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#ifdef _XBOX

#include <xtl.h>

#else

#include <windows.h>
#include <winsock2.h>

#endif

#include <xmem.h>
#include <netsync.h>

#include "server.h"
