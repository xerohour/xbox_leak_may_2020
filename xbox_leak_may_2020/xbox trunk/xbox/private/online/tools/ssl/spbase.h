/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/

#ifdef _WINDOWS

#include <windows.h>
#include <winsock2.h>
#ifndef UNDER_CE
#include <wchar.h>
#endif // !UNDER_CE
//#include <wincrypt.h>

#else
// #include <malloc.h>
#include <xtl.h>
// #include <netinet/in.h>
#endif

#include <stdlib.h>
#ifndef UNDER_CE
#include <stdio.h>
#endif // !UNDER_CE
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define SECURITY_WIN32
#define MULTI_THREADED
#endif

#include "hack.h"

/*
///////////////////////////////////////////////////////
//
// Private Headers
//
///////////////////////////////////////////////////////
*/

#include "spreg.h"
#include "debug.h"



#include "schnlsp.h"
#include "sperr.h"
#include "spdefs.h"
#include "bulk.h"

#include "keyexch.h"
#include "sigsys.h"
#include "cert.h"
#include <certmap.h>
#include "cred.h"
#include "specmap.h"
#include "protocol.h"
#include "cache.h"
#include "context.h"

#include "protos.h"
#include "rng.h"

#include "encode.h"
// #include "sslsock2.h"


#define PCT_INVALID_MAGIC       *(DWORD *)"eerF"

#if DBG

void SPAssert(
    void *FailedAssertion,
    void *FileName,
    unsigned long LineNumber,
    char * Message);

#define SP_ASSERT(x) \
	    if (!(x)) \
		SPAssert(#x, __FILE__, __LINE__, NULL); else

#else // DBG

#define SP_ASSERT(x)

#endif // DBG


#ifdef SECURITY_LINUX

#define SPExternalAlloc(cb)    malloc(cb)
#define SPExternalFree(pv)     free(pv);

#else // SECURITY_LINUX

#if DBG
VOID *SPExternalAlloc(DWORD cb);
VOID SPExternalFree(VOID *pv);
#else // DBG
#define SPExternalAlloc(cb)    LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, cb)
#define SPExternalFree(pv)     LocalFree(pv)
#endif // DBG

#endif // SECURITY_LINUX

#undef RtlMoveMemory

NTSYSAPI
VOID
NTAPI
RtlMoveMemory (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   ULONG Length
   );

#ifdef DEBUG

#define ZONE_INIT        DEBUGZONE(0)
#define ZONE_SERVERCACHE DEBUGZONE(1)
#define ZONE_RNG         DEBUGZONE(2)
#define ZONE_CERT        DEBUGZONE(3)
#define ZONE_FUNCTION    DEBUGZONE(13)
#define ZONE_WARN        DEBUGZONE(14)
#define ZONE_ERROR       DEBUGZONE(15)

#endif // DEBUG

#ifdef __cplusplus
}
#endif
