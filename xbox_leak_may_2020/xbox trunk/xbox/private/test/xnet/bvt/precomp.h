/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    Pre-compiled header file

Author:

    Steven Kehrli (steveke) 5-Oct-2000

------------------------------------------------------------------------------*/

#ifdef _XBOX

#include <xtl.h>
#include <xtestlib.h>

#else

#include <windows.h>
#include <winsock2.h>

#endif

#include <xmem.h>
#include <xnetref.h>
#include <xlog.h>
#include <netsync.h>

#include "util.h"
#include "client.h"
#include "server.h"
