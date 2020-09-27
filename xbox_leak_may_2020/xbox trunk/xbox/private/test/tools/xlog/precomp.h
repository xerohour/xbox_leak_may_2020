/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    Pre-compiled header file

Author:

    Steven Kehrli (steveke) 13-Mar-2000

------------------------------------------------------------------------------*/

#ifndef _XLOG_KDX

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#ifdef _XBOX

#include <ntos.h>

#include <xtl.h>
#include <xboxp.h>
#include <ldr.h>
#include <xbeimage.h>

#include <xtestlib.h>
#include <xnetref.h>

#else

#include <windows.h>
#include <winsock2.h>
#include <rpc.h>

#endif // _XBOX

#include <stdio.h>

#include <xmem.h>
#include <xlog.h>

#include "util.h"
#include "xlogging.h"

#else

#include <windows.h>
#include <winsock2.h>
#include <rpc.h>
#include <wdbgexts.h>
#include <xboxverp.h>
#include <stdio.h>

#include <xlog.h>
#include "xlogging.h"

#endif // _XLOG_KDX
