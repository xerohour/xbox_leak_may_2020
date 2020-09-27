/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  client.c

Abstract:

  Entry points for xnetstream.dll

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetStreamNamespace;

namespace XNetStreamNamespace {

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
    // Run the test
    StreamTest(hLog);
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

} // namespace XNetStreamNamespace



#ifdef __cplusplus
extern "C" {
#endif

// Export table for harness

#pragma data_seg(EXPORT_SECTION_NAME)
DECLARE_EXPORT_DIRECTORY(xnetstream)
#pragma data_seg()

BEGIN_EXPORT_TABLE(xnetstream)
    EXPORT_TABLE_ENTRY("StartTest", StartTest)
    EXPORT_TABLE_ENTRY("EndTest", EndTest)
END_EXPORT_TABLE(xnetstream)

#ifdef __cplusplus
}
#endif
