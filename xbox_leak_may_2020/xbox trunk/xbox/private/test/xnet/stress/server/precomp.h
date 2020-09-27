/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

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
#include <xnetref.h>
#include <xlog.h>

#else

#include <windows.h>
#include <winsock2.h>

#endif

#include <xmem.h>
#include <netsync.h>

#include "stress.h"
#include "server.h"
