/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    covermon.h

Abstract:

    This module contains routine to poll coverage data logging

Environment:

    Xbox

Revision History:

--*/

#ifndef __COVERMON_H__
#define __COVERMON_H__


#ifdef __cplusplus
extern "C" {
#endif


//
// These are exported from covermon.dll
//

DWORD
WINAPI
CovMonInternalCloseLogFileAllBinaries(
    VOID
    );


DWORD
WINAPI
CovMonInternalPollLoggingAllBinaries(
    LPCSTR TestName
    );


BOOL
WINAPI
CovMonRegisterNoImportCoverageBinary(
    HANDLE ImageBase
    );


BOOL
WINAPI
CovMonRegisterAllNoImportCoverageBinaries(
    VOID
    );


#ifdef __cplusplus
}
#endif


#endif // __COVERMON_H__
