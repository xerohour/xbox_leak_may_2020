/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright Microsoft Corporation

Module Name:

  server.c

Abstract:

  This modules contains the server side exports

Author:

  Steven Kehrli (steveke) 17-Apr-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef __cplusplus
extern "C" {
#endif

// Netsync export table for syncsrv

#pragma data_seg(NETSYNC_EXPORT_SECTION_NAME)
DECLARE_NETSYNC_EXPORT_DIRECTORY(xboxkeys_xbox)
#pragma data_seg()

BEGIN_NETSYNC_EXPORT_TABLE(xboxkeys_xbox)
    NETSYNC_EXPORT_TABLE_ENTRY("XboxKeysTestServer", XboxKeysTestServer)
END_NETSYNC_EXPORT_TABLE(xboxkeys_xbox)

#ifdef __cplusplus
}
#endif
