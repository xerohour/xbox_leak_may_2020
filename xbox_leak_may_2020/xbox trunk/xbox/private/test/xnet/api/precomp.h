/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    Pre-compiled header file

Author:

    Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#ifdef _XBOX

#include <xtl.h>
#include <xtestlib.h>
#include <xnetref.h>

#else

#include <windows.h>
#include <winsock2.h>

#endif

#include <stdio.h>

#include <xmem.h>
#include <xlog.h>
#include <netsync.h>

#include "common.h"
#include "util.h"
#include "client.h"
#include "server.h"
