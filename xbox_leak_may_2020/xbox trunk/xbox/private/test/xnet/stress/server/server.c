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



#ifdef _XBOX

#ifdef __cplusplus
extern "C" {
#endif

//
// Netsync export table for syncsrv
//
#pragma data_seg(NETSYNC_EXPORT_SECTION_NAME)
DECLARE_NETSYNC_EXPORT_DIRECTORY(xnetstress_xbox)
#pragma data_seg()

BEGIN_NETSYNC_EXPORT_TABLE(xnetstress_xbox)
    NETSYNC_EXPORT_TABLE_ENTRY( "XNetStressMain",        XNetStressMain        )
    NETSYNC_EXPORT_TABLE_ENTRY( "XNetStressStart",       XNetStressStart       )
    NETSYNC_EXPORT_TABLE_ENTRY( "XNetStressStop",        XNetStressStop        )
    NETSYNC_EXPORT_TABLE_ENTRY( "XNetStressStatusMain",  XNetStressStatusMain  )
    NETSYNC_EXPORT_TABLE_ENTRY( "XNetStressStatusStart", XNetStressStatusStart )
    NETSYNC_EXPORT_TABLE_ENTRY( "XNetStressStatusStop",  XNetStressStatusStop  )
END_NETSYNC_EXPORT_TABLE(xnetstress_xbox)

#ifdef __cplusplus
}
#endif

#endif // XBOX
