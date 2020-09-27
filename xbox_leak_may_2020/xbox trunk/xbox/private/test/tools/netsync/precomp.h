/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    Pre-compiled header file

Author:

    Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#ifdef _XBOX

#include <ntos.h>

#include <xtl.h>

#include <xnetref.h>
#include <xtestlib.h>

#else

#include <windows.h>
#include <winsock2.h>
#include <wsockntp.h>
#include <rpc.h>

#endif

#include <stdlib.h>
#include <stdio.h>

#include <xmem.h>
#include <netsync.h>

#include "util.h"
#include "netsyncp.h"
