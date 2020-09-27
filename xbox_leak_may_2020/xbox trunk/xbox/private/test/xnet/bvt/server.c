/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  server.c

Abstract:

  This modules contains the server side exports

Author:

  Steven Kehrli (steveke) 5-Oct-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

#ifdef __cplusplus
extern "C" {
#endif

// Netsync export table for syncsrv

#pragma data_seg(NETSYNC_EXPORT_SECTION_NAME)
DECLARE_NETSYNC_EXPORT_DIRECTORY(xnetbvt_xbox)
#pragma data_seg()

BEGIN_NETSYNC_EXPORT_TABLE(xnetbvt_xbox)
    NETSYNC_EXPORT_TABLE_ENTRY("BVT_UDPTestServer", BVT_UDPTestServer)
    NETSYNC_EXPORT_TABLE_ENTRY("BVT_TCPTestServer", BVT_TCPTestServer)
END_NETSYNC_EXPORT_TABLE(xnetbvt_xbox)

#ifdef __cplusplus
}
#endif

#endif // XBOX
