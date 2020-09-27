/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    dukeprod2.h

Abstract:

    function declarations, type definitions, includes, etc, for dukeprod2

Author:

    Dennis Krueger <a-denkru> Oct 2001

Revision History:

--*/
#ifndef __dukeprod2_h_
#define __dukeprod2_h_


#ifndef UNICODE
#define UNICODE
#endif

#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif

#define MAX_SLOTS XGetPortCount()


#endif //!defined (__cpx_h_)

