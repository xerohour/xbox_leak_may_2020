/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  client.c

Abstract:

  Entry points for xnetbvt.dll

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetBVTNamespace;

namespace XNetBVTNamespace {

VOID
WINAPI
StartTest(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Test entry point

Arguments:

  hLog - handle to the xLog log object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is the mem object
    HANDLE  hMemObject = INVALID_HANDLE_VALUE;
    // pNetsyncRemoteNt is a pointer to the remote netsync server
    LPSTR   pNetsyncRemote = NULL;



    // Create the private heap
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        return;
    }

    // Set the component and subcomponent
    xSetComponent(hLog, "Network", "XNet");

    // Get the remote netsync addr
    pNetsyncRemote = GetIniString(hMemObject, "xnetbvt", "NetsyncRemote");

    // Test UDP
    BVT_UDPTest(hLog, pNetsyncRemote);

    // Test TCP
    BVT_TCPTest(hLog, pNetsyncRemote);

    if (NULL != pNetsyncRemote) {
        xMemFree(hMemObject, pNetsyncRemote);
    }

    // Free the private heap
    xMemClose(hMemObject);
}



VOID
WINAPI
EndTest(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Test exit point

Return Value:

  None

------------------------------------------------------------------------------*/
{
}

} // namespace XNetBVTNamespace



#ifdef __cplusplus
extern "C" {
#endif

// Export table for harness

#pragma data_seg(EXPORT_SECTION_NAME)
DECLARE_EXPORT_DIRECTORY(xnetbvt)
#pragma data_seg()

BEGIN_EXPORT_TABLE(xnetbvt)
    EXPORT_TABLE_ENTRY("StartTest", StartTest)
    EXPORT_TABLE_ENTRY("EndTest", EndTest)
END_EXPORT_TABLE(xnetbvt)

#ifdef __cplusplus
}
#endif
