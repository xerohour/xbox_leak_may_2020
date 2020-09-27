/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    coverage.h

Abstract:

    This module contains various data structures and procedure prototypes
    of code coverage debugger extension.  The extension is loaded by XBDM
    under Xbox development kit (XDK) only.

--*/

#ifndef _COVERAGE_INCLUDED
#define _COVERAGE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) && !defined(EXTERN_C)
#define EXTERN_C extern "C"
#endif

#define DM_COVERAGE_COMMAND     "BBCOVER"
#define COVERAGE_COMMAND_HEADER "!Cmd"

//
// List of commands available in debug extension
//

enum COVERAGE_COMMAND {
    CoverageCmdSave = 1,
    CoverageCmdReset,
    CoverageCmdPeriodic,
    CoverageCmdList,
    CoverageCmdMax
};

#define DM_RESPONSE_MSG_OFFSET  5
#define GET_RESPONSE_ERR( b )   strtoul(&(b)[DM_RESPONSE_MSG_OFFSET], 0, 16)
#define GET_RESPONSE( b )       (&(b)[DM_RESPONSE_MSG_OFFSET])

#ifdef __cplusplus
}
#endif

#endif // _COVERAGE_INCLUDED

