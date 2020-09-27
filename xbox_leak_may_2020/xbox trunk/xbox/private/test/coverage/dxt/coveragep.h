/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    coveragep.h

Abstract:

    This is a pre-compiled header for Xbox code coverage debugger extension.
    It also contains various data strutures and procedure prototypes.

--*/

#include <ntos.h>
#include <ntrtl.h>
#include <xtl.h>
#include <xbdm.h>
#include <stdio.h>
#include "coverage.h"
#include "bbreg.h"
#include "covfile.h"

#ifndef INLINE
#define INLINE __inline
#endif

#define ROUNDUPN( x, n )    ((((x) + ((n) - 1 )) / (n)) * (n))
#define ROUNDUP2( x, n )    ((((ULONG)(x)) + (((ULONG)(n)) - 1 )) & ~(((ULONG)(n)) - 1 ))
#define ROUNDUP2_64( x, n ) ((((DWORDLONG)(x)) + (((DWORDLONG)(n)) - 1 )) & ~(((DWORDLONG)(n)) - 1 ))

typedef struct _REGISTERED_BINARY REGISTERED_BINARY, *PREGISTERED_BINARY;

struct _REGISTERED_BINARY {
    PREGISTERED_BINARY pNext;                   // singly linked list
    PCHAR              BaseOfLoggingRegion;
    ULONG              SizeOfLoggingRegion;
    ULONG              CheckSumOfBinary;
    PVOID              CapturedLogging;
    DWORD              dwUniquifier;
    HANDLE             LogFileHandle;
    PCOVFILE_HEADER    pCachedCovFileHeader;
    LARGE_INTEGER      FileSize;
    MD5_HASH           md5;                     // pointer to GUID which goes after binary name
    CHAR               BinaryName[ 1 ];         // variable length
};

VOID
NTAPI
RegisterNoImportCoverageBinary(
    IN PVOID ImageBase
    );

VOID
INLINE
SetResponseError(
    OUT PSTR   szResp,
    IN  SIZE_T cchResp,
    IN  DWORD  LastError
    )
/*++

Routine Description:

    This routine stores error value at the beginning of the response buffer.
    It can be used only if HRESULT of the send operation indicates error.

Arguments:

    szResp - Response buffer to be sent

    cchResp - Size in character of response buffer

Return Value:

    None

--*/
{
    if ( _snprintf(szResp, cchResp, "%x", LastError) < 0 && cchResp ) {
        *szResp = '\0';
    }
}

PREGISTERED_BINARY
GetRegisteredBinaryList(
    VOID
    );

PREGISTERED_BINARY
GetRegisteredBinaryListHead(
    VOID
    );

VOID
AcquireRegistrationLock(
    VOID
    );

VOID
ReleaseRegistrationLock(
    VOID
    );

NTSTATUS
NTAPI
CoverageFlushLogging(
    IN PCSTR BinaryName OPTIONAL
    );

VOID
CoverageInternalCloseLogFileAllBinaries(
    VOID
    );

BOOL
CoverageInitPoolBuffer(
    VOID
    );

