
#ifndef _COVERAGE_H_
#define _COVERAGE_H_

#ifndef INLINE
#define INLINE __inline
#endif

#ifndef STDCALL
#define STDCALL __stdcall
#endif

#ifndef DECLSPEC
#define DECLSPEC __declspec( dllimport )
#endif

#include "bbreg.h"

typedef struct _REGISTERED_BINARY REGISTERED_BINARY, *PREGISTERED_BINARY;

struct _REGISTERED_BINARY {
    PREGISTERED_BINARY pNext;           // singly linked list
    PCHAR  pBaseOfLoggingRegion;
    PCHAR  pEndOfLoggingRegion;
    DWORD  dwCheckSumOfBinary;
	MD5_HASH	md5;
    CHAR   szBinaryName[ 1 ];           // variable length, multiple of 4 bytes
    };

//
//  Exported routines from coverage.dll
//

DECLSPEC
BOOL
STDCALL
CoverageRegisterBinary(
    IN LPCSTR pszBinaryName,            // must be lowercase or case consistent
    IN DWORD  dwCheckSumOfBinary,       // checksum of original binary
    IN PCHAR  pBaseOfLoggingRegion,     // instrumented logging region base
    IN DWORD  dwSizeOfLoggingRegion     // instrumented logging region size
    );

DECLSPEC
BOOL
STDCALL
CoverageRegisterBinaryNotSharedRegion(
    IN LPCSTR pszBinaryName,            // must be lowercase or case consistent
	IN LPCSTR pszGUID,
    IN DWORD  dwCheckSumOfBinary,       // checksum of original binary
    IN PCHAR  pBaseOfLoggingRegion,     // instrumented logging region base
    IN DWORD  dwSizeOfLoggingRegion     // instrumented logging region size
    );

DECLSPEC
BOOL
STDCALL
CoverageRegisterBinaryWithStruct(
	IN PBBREG_HEADER	pBBStruct
	);

DECLSPEC
BOOL
STDCALL
CoverageUnregisterBinary(
    IN LPCSTR pszBinaryName            // must be lowercase or case consistent
    );

DECLSPEC
BOOL
STDCALL
CoverageEnumerateBinaries(
    IN  DWORD  dwSizeOfBuffer,
    OUT PCHAR  pBuffer,
    OUT PDWORD pdwSizeOfData
    );

DECLSPEC
BOOL
STDCALL
CoveragePollLogging(
    IN  LPCSTR  pszBinaryName,                // must be lowercase
    IN  DWORD   dwSizeOfBuffer,
    OUT PVOID   pBuffer,
    OUT PDWORD  pdwSizeOfData,
    OUT PDWORD  pdwCheckSumOfBinary,
	OUT PMD5_HASH pMD5
    );

DECLSPEC
BOOL
STDCALL
CoverageFlushLogging(
    IN LPCSTR pszBinaryName OPTIONAL        // must be lowercase if specified
    );

DECLSPEC
VOID
STDCALL
CoverageGetSharedLoggingRegion(
    OUT PCHAR *ppBaseOfRegion,
    OUT DWORD *pdwSizeOfRegion
    );

DECLSPEC
DWORD
STDCALL
CoverageGetDllVersion(
     VOID
     );

#define COVERAGE_DLL_VERSION ( 0x00020002 ) // rev this if change interface

#endif /* _COVERAGE_H_ */
