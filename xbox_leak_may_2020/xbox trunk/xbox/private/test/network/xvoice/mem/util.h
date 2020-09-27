/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 2-Jan-2002

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#ifndef _UTIL_H
#define _UTIL_H

#include "precomp.h"



// Function prototypes

// Stats

VOID
GetStats(
    IN OUT MM_STATISTICS  *mmStats,
    IN OUT PS_STATISTICS  *psStats
);

VOID
CheckStats(
    IN  HANDLE         hLog,
    IN  MM_STATISTICS  *mmStatsInitial,
    IN  MM_STATISTICS  *mmStatsFinal,
    IN  PS_STATISTICS  *psStatsInitial,
    IN  PS_STATISTICS  *psStatsFinal
);



// Logging

VOID
Log_va(
    IN  HANDLE  hLog,
    IN  BOOL    bEcho,
    IN  LPSTR   lpszFormatString,
    IN  ...
);

#endif
