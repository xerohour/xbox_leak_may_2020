/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.c

Abstract:

  This module contains the server side exports

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

#ifdef __cplusplus
extern "C" {
#endif

// Netsync export table for syncsrv

#pragma data_seg(NETSYNC_EXPORT_SECTION_NAME)
DECLARE_NETSYNC_EXPORT_DIRECTORY(xnetstream_xbox)
#pragma data_seg()

BEGIN_NETSYNC_EXPORT_TABLE(xnetstream_xbox)
    NETSYNC_EXPORT_TABLE_ENTRY("StreamServerStart", StreamServerStart)
    NETSYNC_EXPORT_TABLE_ENTRY("StreamServerMain", StreamServerMain)
    NETSYNC_EXPORT_TABLE_ENTRY("StreamServerStop", StreamServerStop)
END_NETSYNC_EXPORT_TABLE(xnetstream_xbox)

#ifdef __cplusplus
}
#endif

#endif // XBOX
