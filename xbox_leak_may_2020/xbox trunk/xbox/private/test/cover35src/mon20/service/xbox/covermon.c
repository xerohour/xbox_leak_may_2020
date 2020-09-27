/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    covermon.c

Abstract:

    This module contains simple routines to poll and save coverage logging
    data to disk

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"

#define ROUNDUPN( x, n )    ((((x) + ((n) - 1 )) / (n)) * (n))
#define ROUNDUP2( x, n )    ((((ULONG)(x)) + (((ULONG)(n)) - 1 )) & ~(((ULONG)(n)) - 1 ))
#define ROUNDUP2_64( x, n ) ((((DWORDLONG)(x)) + (((DWORDLONG)(n)) - 1 )) & ~(((DWORDLONG)(n)) - 1 ))

typedef struct _SAVED_VECTOR SAVED_VECTOR, *PSAVED_VECTOR;

struct _SAVED_VECTOR {
	PVOID			pSavedBitVector;
	PCHAR			pszTestName;
	PSAVED_VECTOR	next;
};

typedef struct _COVMON_REGISTERED_BINARY COVMON_REGISTERED_BINARY, *PCOVMON_REGISTERED_BINARY;

struct _COVMON_REGISTERED_BINARY {
    PCOVMON_REGISTERED_BINARY pNext;                   // singly linked list
    DWORD              dwBlockCount;
    DWORD              dwCheckSumOfBinary;
    DWORD              dwUniquifier;
    HANDLE             hLogFileHandle;
    PCOVFILE_HEADER    pCachedCovFileHeader;
    DWORDLONG          dwlFileSize;
    bool               bKernelBinary;
	bool               bClearForShutdown;
	PSAVED_VECTOR      pLastSavedBitVectorList;     // bit vector for system shutdown
	DWORD              dwSavedVectorCount;
	MD5_HASH           md5;
////////////////////
	PCHAR			   mGuid;
	HANDLE			   ptrFuncDataBase ;
////////////////////
    CHAR               szBinaryName[ 1 ];       // variable length
};


HANDLE hKernelCoverageHandle;
HANDLE CoverMonHeapHandle;

CRITICAL_SECTION NameBufferCritSect;
CRITICAL_SECTION PollBufferCritSect;
CRITICAL_SECTION RegistrationCritSect;

CHAR szNameBuffer[ 65000 ];     // 64K buffer for list of names (not on stack)
LPSTR pCoverageDir;

DWORD  dwPollBufferSize = 0x100000;
PVOID  pPollBuffer;

volatile BOOL bShutdownInProgress = FALSE;
volatile BOOL bMediaWriteProtected = FALSE;
volatile BOOL bCacheAlways = FALSE;
volatile BOOL bCacheMRU = TRUE;

PCOVMON_REGISTERED_BINARY RegisteredBinaryList = (PCOVMON_REGISTERED_BINARY)&RegisteredBinaryList;


BOOL
CovMonIsRunning(
    VOID
    )
{
    return hKernelCoverageHandle != NULL;
}


BOOL
CoverageMonitorInit(
    VOID
    )
{
    hKernelCoverageHandle = KernelCoverageConnectToDriver();
		
    if ( hKernelCoverageHandle == NULL ) {
	    DebugPrint( "COVERMON: coverage.sys was not loaded\n" );
        return FALSE;
    }

    InitializeCriticalSection( &NameBufferCritSect );
    InitializeCriticalSection( &PollBufferCritSect );
    InitializeCriticalSection( &RegistrationCritSect );

    dwPollBufferSize = ROUNDUP2( dwPollBufferSize, 8192 );
    pPollBuffer = ExAllocatePoolWithTag( dwPollBufferSize, COVERMON_POOLTAG );

    if ( !pPollBuffer ) {
        return FALSE;
    }

    pCoverageDir = "z:\\coverage";
    CoverMonHeapHandle = HeapCreate( 0, 0, 0 );

    return CoverMonHeapHandle != NULL;
}


VOID
CDECL
CoverMonStartup(
    VOID
    )
{
    CoverageMonitorInit();
}


VOID
CDECL
CoverMonCleanup(
    VOID
    )
{
    HeapDestroy( CoverMonHeapHandle );
}


//
// Setup a startup pointer to be called by CRT
//
#pragma data_seg(".CRT$XIU")
PROC covermon_startup = (PROC)CoverMonStartup;
#pragma data_seg()


//
// Setup a cleanup pointer to be called by CRT
//
#pragma data_seg(".CRT$XTU")
PROC covermon_cleanup = (PROC)CoverMonCleanup;
#pragma data_seg()


#pragma comment( linker, "/include:_covermon_startup" )
#pragma comment( linker, "/include:_covermon_cleanup" )


VOID
UuidToString(
    GUID * guid,
    LPSTR * pszString
    )
{
    const LPCSTR pszTemplate = "%8.8lX-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X";

    *pszString = HeapAlloc( CoverMonHeapHandle, 0, sizeof("5bca35fd-5d73-40b9-b9a8-49c12c629168") );
    if ( *pszString == NULL ) {
        return;
    }

    sprintf(
        *pszString, 
        pszTemplate, 
        guid->Data1, guid->Data2, guid->Data3,
        guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
        guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]
        );
}


VOID
INLINE
AddRegisteredBinaryNodeToList(
    IN PCOVMON_REGISTERED_BINARY pPrev,
    IN PCOVMON_REGISTERED_BINARY pNode
    )
{
    pNode->pNext = pPrev->pNext;
    pPrev->pNext = pNode;
}


PCOVMON_REGISTERED_BINARY
INLINE
AllocateRegisteredBinaryNode(
    IN ULONG BinaryNameLength
    )
{
    return HeapAlloc( CoverMonHeapHandle, 0, sizeof(COVMON_REGISTERED_BINARY) + BinaryNameLength );
}


PCOVMON_REGISTERED_BINARY
INLINE
AllocateRegisteredBinaryNodeWithNameAndGUID(
    IN LPCSTR pszBinaryName,
	IN PMD5_HASH pMD5
    )
{
    ULONG NameLength = strlen( pszBinaryName );
    PCOVMON_REGISTERED_BINARY pNode = AllocateRegisteredBinaryNode( NameLength );
	
    if ( pNode != NULL ) {
        ZeroMemory( pNode, sizeof( COVMON_REGISTERED_BINARY ) - 1 );
        CopyMemory( pNode->szBinaryName, pszBinaryName, NameLength + 1 );
		CopyMemory( &(pNode->md5), pMD5, MD5_SIZE);
		UuidToString((UUID*)pMD5, &(pNode->mGuid));
		pNode->pLastSavedBitVectorList = NULL;
		pNode->bClearForShutdown = false;
		pNode->dwSavedVectorCount = 0;
	}
	
    return pNode;
}


PCOVMON_REGISTERED_BINARY
CovMonInternalFindOrAddRegistration(
									IN LPCSTR pszBinaryName,        // lowercase
									IN DWORD  dwBlockCount,
									IN DWORD  dwCheckSum,
									IN bool   bKernelBinary,
									IN PMD5_HASH pMD5
									)
{
    PCOVMON_REGISTERED_BINARY pNode;
    PCOVMON_REGISTERED_BINARY pPrev;
    bool bFound = false;
	
    EnterCriticalSection( &RegistrationCritSect );
	
    //
    //  Walk list of registered binaries looking for name
    //
	
    pPrev = (PCOVMON_REGISTERED_BINARY)&RegisteredBinaryList;
    pNode = RegisteredBinaryList;
	
    while ( pNode != (PCOVMON_REGISTERED_BINARY)&RegisteredBinaryList ) {
		
        if ( strcmp( pNode->szBinaryName, pszBinaryName ) == 0 ) {
            bFound = true;
            break;
		}
		
        pPrev = pNode;
        pNode = pNode->pNext;
		
	}
	
    if ( ! bFound ) {
		
        pNode = AllocateRegisteredBinaryNodeWithNameAndGUID( pszBinaryName, pMD5 );
		
        if ( pNode != NULL ) {

			DebugPrint("Name: %s\tGUID: %s\n", pNode->szBinaryName, pNode->mGuid);
			
            pNode->dwBlockCount       = dwBlockCount;
            pNode->dwCheckSumOfBinary = dwCheckSum;
            pNode->bKernelBinary      = bKernelBinary;
			
            AddRegisteredBinaryNodeToList( pPrev, pNode );
			
		}
	}
	
    LeaveCriticalSection( &RegistrationCritSect );
	
    return pNode;
}


DWORDLONG
GetFileSize64(
    IN HANDLE hFile
    )
{
    ULARGE_INTEGER FileSize;
	
    FileSize.LowPart = GetFileSize( hFile, &FileSize.HighPart );
	
    if (( FileSize.LowPart == 0xFFFFFFFF ) && ( GetLastError() != NO_ERROR )) {
        FileSize.HighPart = 0xFFFFFFFF;
	}
	
    return FileSize.QuadPart;
}


BOOL
SetFilePointer64(
    IN HANDLE    hFile,
    IN DWORDLONG dwlOffset
    )
{
    ULARGE_INTEGER Position;
	
    Position.QuadPart = dwlOffset;
    Position.LowPart  = SetFilePointer( hFile, (LONG)Position.LowPart, (PLONG)&Position.HighPart, FILE_BEGIN );
	
    if (( Position.LowPart == 0xFFFFFFFF ) && ( GetLastError() != NO_ERROR )) {
        return false;
	}
	
    return ( Position.QuadPart == dwlOffset );
}


BOOL
CovMonInternalOpenLogFile(
    IN OUT PCOVMON_REGISTERED_BINARY pNode
	)
{
    PCOVFILE_HEADER pCovFileHeader;
    CHAR      szUniquifier[ 16 ];
    CHAR      szLogFileName[ 256 ];
    CHAR      szNewFileName[ 256 ];
    DWORD     dwLastError;
    DWORDLONG dwlFileSize;
    DWORD     dwActual;
    DWORD     dwHeaderSize;
    DWORD     dwAttr;
	DWORD     dwGUIDLength;
    HANDLE    hFile;
	
    strcpy( szLogFileName, pCoverageDir );
    strcat( szLogFileName, "\\" );

    //
    // Create coverage directory first
    //
    CreateDirectory( szLogFileName, NULL );

    strcat( szLogFileName, pNode->szBinaryName );
    dwAttr = GetFileAttributes( szLogFileName );
	
    if ( ! ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )) {
		
        //
        //  There's a file, not a directory, by this name already.  We'll
        //  have to rename the file before we can create the directory.
        //
		
        strcpy( szNewFileName, pCoverageDir );
        strcat( szNewFileName, "\\~" );
        strcat( szNewFileName, szLogFileName );
        DeleteFile( szNewFileName );
        MoveFile( szLogFileName, szNewFileName );
        dwAttr = GetFileAttributes( szLogFileName );
	}
	
    if (( dwAttr == 0xFFFFFFFF ) || ( ! ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ))) {
		
        if ( ! CreateDirectory( szLogFileName, NULL )) {
			
            dwLastError = GetLastError();
			
            DebugPrint(
                "COVERMON: LOST DATA for %s because failed to access or create\n"
                "          directory %s (error %d)\n",
                pNode->szBinaryName,
                szLogFileName,
                dwLastError
                );
			
            SetLastError( dwLastError );
            return false;
		}
	}
	
    strcat( szLogFileName, "\\" );
    strcat( szLogFileName, pNode->szBinaryName );
	
    if ( pNode->dwUniquifier != 0 ) {
        sprintf( szUniquifier, ".%08X.%d", pNode->dwCheckSumOfBinary, pNode->dwUniquifier );
        strcat( szLogFileName, szUniquifier );
	}

    strcat( szLogFileName, ".covdata" );
	
    hFile = CreateFile(
		szLogFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH,
		NULL
		);
	
    if ( hFile == INVALID_HANDLE_VALUE ) {
		
        dwLastError = GetLastError();
		
        DebugPrint(
            "COVERMON: LOST DATA for %s because failed to open or create\n"
            "          file %s (error %d)\n",
            pNode->szBinaryName,
            szLogFileName,
            dwLastError
            );
		
        SetLastError( dwLastError );
        return false;
	}
	
    dwlFileSize = GetFileSize64( hFile );
	
    if ( dwlFileSize == (DWORDLONG) -1 ) {
		
        dwLastError = GetLastError();
		
        DebugPrint(
            "COVERMON: LOST DATA for %s because failed to query file %s (error %d)\n",
            pNode->szBinaryName,
            szLogFileName,
            dwLastError
            );
		
        CloseHandle( hFile );
        SetLastError( dwLastError );
        return false;
	}
	
    pCovFileHeader = HeapAlloc( CoverMonHeapHandle, 0, COVFILE_HEADER_MIN_SIZE );
	
    if ( pCovFileHeader == NULL ) {
		
        DebugPrint(
            "COVERMON: LOST DATA for %s because failed to allocate memory (%d bytes).\n",
            pNode->szBinaryName,
            COVFILE_HEADER_MIN_SIZE
            );
		
        CloseHandle( hFile );
        SetLastError( ERROR_OUTOFMEMORY );
        return false;
	}
	
    dwActual = 0;
	
    if ( dwlFileSize >= COVFILE_HEADER_MIN_SIZE ) {
		
        if (( ! ReadFile( hFile, pCovFileHeader, COVFILE_HEADER_MIN_SIZE, &dwActual, NULL )) &&
            (( dwLastError = GetLastError() ) != ERROR_HANDLE_EOF )) {
			
            DebugPrint(
                "COVERMON: LOST DATA for %s because failed to read file %s (error %d)\n",
                pNode->szBinaryName,
                szLogFileName,
                dwLastError
                );
			
            CloseHandle( hFile );
            SetLastError( dwLastError );
            return false;
		}
	}
	
    if ( dwActual != COVFILE_HEADER_MIN_SIZE ) {
		
        //
        //  New file being created.
        //
		
        DebugPrint( "COVERMON: New file: %s.\n", szLogFileName );
		
        ZeroMemory( pCovFileHeader, COVFILE_HEADER_MIN_SIZE );

		dwGUIDLength = MD5_SIZE;
		
        pCovFileHeader->dwSignature    = COVFILE_SIGNATURE;
        pCovFileHeader->dwVersion      = COVFILE_VERSION;
        pCovFileHeader->dwCheckSum     = pNode->dwCheckSumOfBinary;
        pCovFileHeader->dwBlockCount   = pNode->dwBlockCount;

		// Header length is defined to be COVFILE_HEADER + GUID + one DIR_ENTRY,
		// rounded up to 512
        pCovFileHeader->dwHeaderLength =
			ROUNDUP2(
			sizeof(COVFILE_HEADER) + MD5_SIZE + sizeof(COVFILE_DIRECTORY),
			COVFILE_HEADER_MIN_SIZE
			);

		// Put GUID at end of header
		pCovFileHeader->dwOffsetOfGUID = sizeof( COVFILE_HEADER );

		// Put the first dir entry after the GUID
		pCovFileHeader->dwOffsetOfFirstDir = ROUNDUP2(sizeof( COVFILE_HEADER ) + dwGUIDLength, 4);

		CopyMemory( (PCHAR)pCovFileHeader + pCovFileHeader->dwOffsetOfGUID, &(pNode->md5), dwGUIDLength);
		
        dwlFileSize = COVFILE_HEADER_MIN_SIZE;
	}
	
    else if (( pCovFileHeader->dwSignature  != COVFILE_SIGNATURE ) ||
		( pCovFileHeader->dwCheckSum   != pNode->dwCheckSumOfBinary ) ||
		( pCovFileHeader->dwBlockCount != pNode->dwBlockCount )) {
		
        //
        //  Existing file does not match checksum, close existing and open new.
        //
		
        DebugPrint(
            "COVERMON: Binary %s contains different checksum than %s\n",
            pNode->szBinaryName,
            szLogFileName
            );
		
        CloseHandle( hFile );
		
        pNode->dwUniquifier++;
		
        return CovMonInternalOpenLogFile( pNode );
		
	}
	
    else if ( pCovFileHeader->dwVersion > COVFILE_VERSION ) {
		
        //
        //  File is of newer version than we understand.
        //
		
        DebugPrint(
            "COVERMON: %s is %s version than covermon understands\n",
            szLogFileName,
            "newer"
            );
		
        CloseHandle( hFile );
		
        pNode->dwUniquifier++;
		
        return CovMonInternalOpenLogFile( pNode );
		
	}
	
    else if ( pCovFileHeader->dwVersion < COVFILE_VERSION ) {
		
        //
        //  File is of older version than we understand.
        //
		
        DebugPrint(
            "COVERMON: %s is %s version than covermon understands\n",
            szLogFileName,
            "older"
            );
		
        CloseHandle( hFile );
		
        pNode->dwUniquifier++;
		
        return CovMonInternalOpenLogFile( pNode );
		
	}
	
    else if ( pCovFileHeader->dwHeaderLength > COVFILE_HEADER_MIN_SIZE ) {
		
        //
        //  More header needs to be read and cached.
        //
		
        dwHeaderSize = pCovFileHeader->dwHeaderLength;
		
        pCovFileHeader = HeapReAlloc( CoverMonHeapHandle, 0, pCovFileHeader, dwHeaderSize );
		
        if ( pCovFileHeader == NULL ) {
			
            DebugPrint(
                "COVERMON: LOST DATA for %s because failed to allocate memory (%d bytes).\n",
                pNode->szBinaryName,
                dwHeaderSize
                );
			
            CloseHandle( hFile );
            SetLastError( ERROR_OUTOFMEMORY );
            return false;
		}
		
        dwActual = 0;
		
        if ( ! ReadFile( hFile,
			(PCHAR)pCovFileHeader + COVFILE_HEADER_MIN_SIZE,
			dwHeaderSize - COVFILE_HEADER_MIN_SIZE,
			&dwActual,
			NULL )) {
			
            dwLastError = GetLastError();
			
            DebugPrint(
                "COVERMON: LOST DATA for %s because failed to read file %s (error %d)\n",
                pNode->szBinaryName,
                szLogFileName,
                dwLastError
                );
			
            CloseHandle( hFile );
            SetLastError( dwLastError );
            return false;
		}
	}
	
    pNode->pCachedCovFileHeader = pCovFileHeader;
    pNode->hLogFileHandle = hFile;
    pNode->dwlFileSize = dwlFileSize;
	
    return true;
}


VOID
CovMonInternalClearCache(
	PCOVMON_REGISTERED_BINARY pNode
	)
{
	PSAVED_VECTOR pTmpVectorNode;
	PSAVED_VECTOR pNxtVectorNode;

	for ( pTmpVectorNode = pNode->pLastSavedBitVectorList;
	      pTmpVectorNode != NULL;
	      pTmpVectorNode = pNxtVectorNode) {
		pNxtVectorNode = pTmpVectorNode->next;
		HeapFree( CoverMonHeapHandle, 0, pTmpVectorNode->pszTestName);
		HeapFree( CoverMonHeapHandle, 0, pTmpVectorNode->pSavedBitVector);
		HeapFree( CoverMonHeapHandle, 0, pTmpVectorNode);
		pNode->dwSavedVectorCount--;
	}

	pNode->pLastSavedBitVectorList = NULL;
}


BOOL
CovMonInternalUpdateLogFile(
    IN PCOVMON_REGISTERED_BINARY pNode,
    IN LPCSTR pszTestName
    )
{
    PCOVFILE_HEADER pHeader;
    PCOVFILE_DIRECTORY pDirEntry;
    PCOVFILE_DIRECTORY_201 pDirEntry201;
    HANDLE hFile;
    DWORD dwNameHash;
    DWORD dwVectorSize;
    PVOID pNewBitVector;
    PVOID pOldBitVector;
    PVOID pMoveBuffer;
    bool  bFound;
    DWORD dwLastError;
    DWORD dwActual;
    DWORDLONG dwlOffset;
    DWORDLONG dwlFileSize;
    DWORDLONG dwlVectorOffset;
    DWORDLONG dwlMoveBias;
    DWORDLONG dwlExtent;
    PVOID pNewHeader;
    DWORD dwNameLength;
    DWORD dwEntryLength;
    DWORD dwGrowSize;
	DWORD dwOffsetOfFirstDir;
    bool  bSuccess;
    LPSTR pszName;
	bool  bAlreadyHasSavedVector;
	bool  bDataHasChanged;
	DWORD i;
	PSAVED_VECTOR pTmpVectorNode;
	PSAVED_VECTOR pPrvVectorNode;
	PSAVED_VECTOR pNxtVectorNode;
	PSAVED_VECTOR pNewVectorNode;
//	LPSTR pszVectorTestName;
	bool bFoundNode;
	
    //
    //  Note pPollBuffer is a global that is an implied [IN] parameter.
    //  Assume PollBufferCritSect is held.
    //
	
    pNewBitVector = NULL;
    pOldBitVector = NULL;
	pTmpVectorNode= NULL;
	pPrvVectorNode= NULL;
	pNxtVectorNode= NULL;
	pNewVectorNode= NULL;
    pMoveBuffer   = NULL;
    bSuccess      = false;
    dwLastError   = 0;
	bAlreadyHasSavedVector = false;
	bDataHasChanged = true;
	
    ASSERT( DoesBufferContainOnlyHex01s( pPollBuffer, pNode->dwBlockCount ));

    try {
		
        dwVectorSize = BITVECTOR_STORAGE_SIZE( pNode->dwBlockCount );

        pNewBitVector = HeapAlloc( CoverMonHeapHandle, 0, dwVectorSize );
		
        if ( pNewBitVector == NULL ) {
			
            DebugPrint(
                "COVERMON: LOST DATA for %s because failed to allocate memory (%d bytes).\n",
                pNode->szBinaryName,
                dwVectorSize
                );
			
            dwLastError = ERROR_OUTOFMEMORY;
            leave;
		}
		
        *(UNALIGNED DWORD*)((PCHAR)pNewBitVector + dwVectorSize - 4 ) = 0;

        PackByteVectorToBitVector(
            pNode->dwBlockCount,
            pPollBuffer,
            pNewBitVector
            );
		
        ASSERT( CountBitsSetInBuffer( pPollBuffer, pNode->dwBlockCount ) ==
			CountBitsSetInBuffer( pNewBitVector, dwVectorSize ));

		// If the first test name is not "(system shutdown)", then clear out the cache
		if( (bCacheAlways || bCacheMRU) && 
			bShutdownInProgress &&
			(pNode->bClearForShutdown != true)
			){

			// Iterate over the saved vectors and free memory back to the heap
			CovMonInternalClearCache( pNode );

			pNode->bClearForShutdown = true;
		}

        bFound     = false;
        hFile      = pNode->hLogFileHandle;
        dwNameHash = HashName( pszTestName );
        pHeader    = pNode->pCachedCovFileHeader;
		dwOffsetOfFirstDir = pHeader->dwOffsetOfFirstDir;
//        pDirEntry  = (PCOVFILE_DIRECTORY)((PCHAR)pHeader + sizeof( COVFILE_HEADER ));
        pDirEntry  = (PCOVFILE_DIRECTORY)((PCHAR)pHeader + dwOffsetOfFirstDir);
		
        while ( pDirEntry->dwLengthOfEntry != 0 ) {
			
            if ( pDirEntry->dwHashOfName == dwNameHash ) {
				
                //
                //  Offset of pDirEntry->szName is different for versions
                //  201 and 202 of covdata file.
                //
				
                if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {
                    pszName = ((PCOVFILE_DIRECTORY_201)pDirEntry)->szName;
				}
                else {
                    ASSERT( pHeader->dwVersion == COVFILE_VERSION );
                    pszName = pDirEntry->szName;
				}

                if ( _stricmp( pszName, pszTestName ) == 0 ) {
					
                    bFound = true;
                    break;
				}
			}
			
            pDirEntry = (PVOID)((PCHAR)pDirEntry + pDirEntry->dwLengthOfEntry );
			
		}
#pragma warning(disable: 4127)
        if ( bFound ) {
			
            //
            //  If test name found, read existing vector, OR with new vector,
            //  write back to file.
            //

			if( bCacheAlways || bCacheMRU ){
				bFoundNode = false;
				// See if we already have the vector stored in memory
				pTmpVectorNode = pNode->pLastSavedBitVectorList;

				// Only search through list if we are *always* caching
				// Otherwise, we only cache the MRU
				if( bCacheAlways ){
					for( i=0; i < pNode->dwSavedVectorCount; i++ ){
						if( _stricmp( pTmpVectorNode->pszTestName, pszTestName ) == 0 ){
							bFoundNode = true;
							break;
						}
						pPrvVectorNode = pTmpVectorNode;
						pTmpVectorNode = pTmpVectorNode->next;
					}

				} else {
					// If Cache MRU, then check if the MRU test name matches
					if( (pTmpVectorNode != NULL) &&
						_stricmp( pTmpVectorNode->pszTestName, pszTestName ) == 0 ){
						bFoundNode = true;

					// If the test names don't match, then delete the node
					} else if( pTmpVectorNode != NULL ){
						CovMonInternalClearCache( pNode );

						bFoundNode = false;
					}
				}

				if( !bFoundNode ){

					pNewVectorNode = HeapAlloc( CoverMonHeapHandle, 0, sizeof(SAVED_VECTOR) );

					if( pNewVectorNode == NULL ){
						DebugPrint( "COVERMON: Failed to allocate memory for vector node (%s, %d bytes).\n", pNode->szBinaryName, sizeof(SAVED_VECTOR) );
						dwLastError = ERROR_OUTOFMEMORY;
						leave;
					}

					pNewVectorNode->next = NULL;
					
					pNewVectorNode->pSavedBitVector = HeapAlloc( CoverMonHeapHandle, 0, dwVectorSize );

					if( pNewVectorNode->pSavedBitVector == NULL ){
						DebugPrint( "COVERMON: Failed to allocate memory for vector (%s, %d bytes).\n", pNode->szBinaryName, dwVectorSize );
						dwLastError = ERROR_OUTOFMEMORY;
						leave;
					}

					pNewVectorNode->pszTestName = HeapAlloc( CoverMonHeapHandle, 0, strlen(pszTestName)+1 );
					
					if( pNewVectorNode->pszTestName == NULL ){
						DebugPrint( "COVERMON: Failed to allocate memory for vector name (%s, %d bytes).\n", pNode->szBinaryName, strlen(pszTestName)+1 );
						dwLastError = ERROR_OUTOFMEMORY;
						leave;
					}

					strcpy( pNewVectorNode->pszTestName, pszTestName );

					if( pNode->pLastSavedBitVectorList == NULL ){
						pNode->pLastSavedBitVectorList = pNewVectorNode;
						
					} else if( bCacheAlways ){
						pPrvVectorNode->next = pNewVectorNode;

					}

					pNode->dwSavedVectorCount++;
					
					bAlreadyHasSavedVector = false;

					pOldBitVector = pNewVectorNode->pSavedBitVector;

				} else {
					bAlreadyHasSavedVector = true;

					pOldBitVector = pTmpVectorNode->pSavedBitVector; // Used for the ReadFile down below
					
				}

			} else {
				pOldBitVector = HeapAlloc( CoverMonHeapHandle, 0, dwVectorSize );

				if( pOldBitVector == NULL ){
					DebugPrint( "COVERMON: LOST DATA for %s because failed to allocate memory (%d bytes).\n", pNode->szBinaryName, dwVectorSize );
					dwLastError = ERROR_OUTOFMEMORY;
					leave;
				}
			}
			
            if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {
                dwlVectorOffset = ((PCOVFILE_DIRECTORY_201)pDirEntry)->dwOffsetOfData;
			}
            else {
                ASSERT( pHeader->dwVersion == COVFILE_VERSION );
                dwlVectorOffset = pDirEntry->dwlOffsetOfData;
			}
			
            dwActual = 0;

			// If there is already a saved vector, or we are not caching, then read from covdata file
			if( !bAlreadyHasSavedVector ||
				!(bCacheAlways || bCacheMRU) ){

				if (( ! SetFilePointer64( hFile, dwlVectorOffset )) ||
					( ! ReadFile( hFile, pOldBitVector, dwVectorSize, &dwActual, NULL )) ||
					( dwActual != dwVectorSize )) {
					
					dwLastError = GetLastError();
					
					DebugPrint(
						"COVERMON: LOST DATA for %s because failed to read covdata file (error %d)\n",
						pNode->szBinaryName,
						dwLastError
						);
					
					leave;
				}
			}

			//
			// 1) NewBuf = NewBuf OR OldBuf
			// 2) OldBuf = NewBuf XOR OldBuf
			// 3) If any 1's in OldBuf, then there were changes; save data
			//    otherwise, no changes, so don't save
			//
            OrBufferWithBuffer( pNewBitVector, pOldBitVector, dwVectorSize );
			
            ASSERT( CountBitsSetInBuffer( pNewBitVector, dwVectorSize ) >=
				CountBitsSetInBuffer( pOldBitVector, dwVectorSize ));

			XorBufferWithBuffer( pOldBitVector, pNewBitVector, dwVectorSize );

			if( DoesBufferContainAllZeros( pOldBitVector, dwVectorSize ) ){
				bDataHasChanged = false;
			} else {
				bDataHasChanged = true;
			}

			//////////////////////////////////////////////////
			
			//
			// Save the logging info for the purposes of shutdown
			//

			if( bCacheAlways || bCacheMRU ){
				CopyMemory( pOldBitVector, pNewBitVector, dwVectorSize );
			}
			
            dwActual = 0;
			
			if( bDataHasChanged ){
				if (( ! SetFilePointer64( hFile, dwlVectorOffset )) ||
					( ! WriteFile( hFile, pNewBitVector, dwVectorSize, &dwActual, NULL )) ||
					( dwActual != dwVectorSize )) {
					
					dwLastError = GetLastError();
					
					if( dwLastError == ERROR_WRITE_PROTECT ){
						DebugPrint(
							"COVERMON: Unable to save further data because the filesystem has been shutdown.\n"
							);
						bMediaWriteProtected = true;

					} else {
						DebugPrint(
							"COVERMON: LOST DATA for %s because failed to write covdata file (error %d)\n",
							pNode->szBinaryName,
							dwLastError
							);
						
					}
					
					leave;
				}
			}
			
            FlushFileBuffers( hFile );
		}
		
        else {
			
            //
            //  If test not found, write new vector then update header (grow
            //  header if necessary).  We write new vector first in case later
            //  fail to write updated header -- file will still be valid but
            //  without new vector.  If wrote header first then failed to write
            //  vector, file would left in invalid state.
            //

            dwlVectorOffset = ROUNDUP2_64( pNode->dwlFileSize, COVFILE_VECTOR_BOUNDARY );
            dwActual = 0;
			
            if (( ! SetFilePointer64( hFile, dwlVectorOffset )) ||
                ( ! WriteFile( hFile, pNewBitVector, dwVectorSize, &dwActual, NULL )) ||
                ( dwActual != dwVectorSize )) {
				
                dwLastError = GetLastError();
				
				if( dwLastError == ERROR_WRITE_PROTECT ){
					bMediaWriteProtected = true;
					DebugPrint(
						"COVERMON: Unable to save further data because the filesystem has been shutdown.\n"
						);

				} else {
					DebugPrint(
						"COVERMON: LOST DATA for %s because failed to write covdata file (error %d)\n",
						pNode->szBinaryName,
						dwLastError
						);
					
				}

                leave;
			}
			
            FlushFileBuffers( hFile );
			
            dwlFileSize = dwlVectorOffset + dwActual;
			
            //
            //  Now update the header.
            //
			
            dwNameLength  = strlen( pszTestName ) + 1;
            dwEntryLength = ROUNDUP2( sizeof( COVFILE_DIRECTORY ) + dwNameLength, 4 );
			
            if ((((PCHAR)pDirEntry - (PCHAR)pHeader ) + dwEntryLength + 4 ) > pHeader->dwHeaderLength ) {
				
                //
                //  Not enough room in existing header, need to grow it by multiple
                //  of ROUNDUP2( dwVectorSize, COVFILE_VECTOR_BOUNDARY ).
                //
				
                dwGrowSize  = ROUNDUPN( dwEntryLength, ROUNDUP2( dwVectorSize, COVFILE_VECTOR_BOUNDARY ));
				
                dwlFileSize = ROUNDUP2_64( dwlFileSize, COVFILE_VECTOR_BOUNDARY );
				
                dwlOffset   = ROUNDUP2_64( pHeader->dwHeaderLength, COVFILE_VECTOR_BOUNDARY );
				
                dwlExtent   = dwlOffset + dwGrowSize;
				
                dwlMoveBias = dwlFileSize - dwlOffset;
				
                dwActual    = 0;
				
                if (( pHeader->dwVersion == COVFILE_VERSION_201 ) &&
                    ( dwlFileSize + dwGrowSize > 0xF0000000 )) {
					
                    DebugPrint(
                        "COVERMON: LOST DATA for %s because covdata file is too large.\n"
                        "    Use covdata.exe tool to upgrade covdata file to new version.\n",
                        pNode->szBinaryName
                        );
					
                    leave;
				}
				
                pMoveBuffer = ExAllocatePoolWithTag( dwGrowSize, COVERMON_POOLTAG );
				
                if ( pMoveBuffer == NULL ) {
					
                    DebugPrint(
                        "COVERMON: LOST DATA for %s because failed to allocate buffer.\n",
                        pNode->szBinaryName
                        );
					
                    leave;
				}
				
                if (( ! SetFilePointer64( hFile, dwlOffset )) ||
                    ( ! ReadFile( hFile, pMoveBuffer, dwGrowSize, &dwActual, NULL ))) {
					
                    dwLastError = GetLastError();
					
                    DebugPrint(
                        "COVERMON: LOST DATA for %s because failed to read covdata file (error %d)\n",
                        pNode->szBinaryName,
                        dwLastError
                        );
					
                    leave;
				}
				
                if (( ! SetFilePointer64( hFile, dwlFileSize )) ||
                    ( ! WriteFile( hFile, pMoveBuffer, dwActual, &dwActual, NULL ))) {
					
                    dwLastError = GetLastError();
					
					if( dwLastError == ERROR_WRITE_PROTECT ){
						bMediaWriteProtected = true;
						DebugPrint(
							"COVERMON: Unable to save further data because the filesystem has been shutdown.\n"
							);

					} else {
						DebugPrint(
							"COVERMON: LOST DATA for %s because failed to write covdata file (error %d)\n",
							pNode->szBinaryName,
							dwLastError
							);
						
					}

                    leave;
				}
				
                FlushFileBuffers( hFile );
				
                dwlFileSize = dwlFileSize + dwActual;
				
                pNewHeader = HeapReAlloc(
					CoverMonHeapHandle,
					HEAP_ZERO_MEMORY,
					pHeader,
					pHeader->dwHeaderLength + dwGrowSize
					);
				
                if ( pNewHeader == NULL ) {
					
                    DebugPrint(
                        "COVERMON: LOST DATA for %s because failed to allocate memory (%d bytes).\n",
                        pNode->szBinaryName,
                        pHeader->dwHeaderLength + dwGrowSize
                        );
					
                    dwLastError = ERROR_OUTOFMEMORY;
                    leave;
				}
				
                pHeader = pNewHeader;
				
                pNode->pCachedCovFileHeader = pHeader;
				
                pHeader->dwHeaderLength += dwGrowSize;
				
//                pDirEntry = (PCOVFILE_DIRECTORY)((PCHAR)pHeader + sizeof( COVFILE_HEADER ));
                pDirEntry = (PCOVFILE_DIRECTORY)((PCHAR)pHeader + dwOffsetOfFirstDir);
				
                while ( pDirEntry->dwLengthOfEntry != 0 ) {
					
                    if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {
						
                        pDirEntry201 = (PCOVFILE_DIRECTORY_201) pDirEntry;
						
                        if (( pDirEntry201->dwOffsetOfData >= dwlOffset ) &&
                            ( pDirEntry201->dwOffsetOfData <  dwlExtent )) {
							
                            ASSERT((( pDirEntry201->dwOffsetOfData + dwlMoveBias ) >> 32 ) == 0 );
							
                            pDirEntry201->dwOffsetOfData += (DWORD) dwlMoveBias;
							
						}
						
					}
                    else {
						
                        ASSERT( pHeader->dwVersion == COVFILE_VERSION );
						
                        if (( pDirEntry->dwlOffsetOfData >= dwlOffset ) &&
                            ( pDirEntry->dwlOffsetOfData <  dwlExtent )) {
							
                            pDirEntry->dwlOffsetOfData += dwlMoveBias;
							
						}
					}
					
                    pDirEntry = (PVOID)((PCHAR)pDirEntry + pDirEntry->dwLengthOfEntry );
					
				}
            }
			
            pDirEntry->dwLengthOfEntry = dwEntryLength;
            pDirEntry->dwHashOfName    = dwNameHash;
			
            if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {
				
                pDirEntry201 = (PCOVFILE_DIRECTORY_201) pDirEntry;
				
                ASSERT(( dwlVectorOffset >> 32 ) == 0 );
				
                pDirEntry201->dwOffsetOfData = (DWORD)dwlVectorOffset;
                strcpy( pDirEntry201->szName, pszTestName );
				
			}
			
            else {
				
                ASSERT( pHeader->dwVersion == COVFILE_VERSION );
				
                pDirEntry->dwlOffsetOfData = dwlVectorOffset;
                strcpy( pDirEntry->szName, pszTestName );
				
			}
			
            if (( ! SetFilePointer64( hFile, 0 )) ||
                ( ! WriteFile( hFile, pHeader, pHeader->dwHeaderLength, &dwActual, NULL ))) {
				
                dwLastError = GetLastError();
				
				if( dwLastError == ERROR_WRITE_PROTECT ){
					bMediaWriteProtected = true;
					DebugPrint(
						"COVERMON: Unable to save further data because the filesystem has been shutdown.\n"
						);

				} else {
					DebugPrint(
						"COVERMON: LOST DATA for %s because failed to write covdata file (error %d)\n",
						pNode->szBinaryName,
						dwLastError
						);
				}

                leave;
			}
			
            FlushFileBuffers( hFile );
			
            pNode->dwlFileSize = dwlFileSize;   // only if everything successful

			// Now we need to cache this info
			if( bCacheAlways || bCacheMRU ){

				// If Cache MRU and the test names don't match, then delete the node
				if( bCacheMRU && 
					(pNode->pLastSavedBitVectorList != NULL) &&
					(_stricmp( pNode->pLastSavedBitVectorList->pszTestName, pszTestName ) != 0)
					){
					CovMonInternalClearCache( pNode );
				}

				// Only allocate new Node if necessary
				if( bCacheAlways || (bCacheMRU && (pNode->pLastSavedBitVectorList == NULL)) ){
					
					pNewVectorNode = HeapAlloc( CoverMonHeapHandle, 0, sizeof(SAVED_VECTOR) );
					
					if( pNewVectorNode == NULL ){
						DebugPrint( "COVERMON: Failed to allocate memory for vector node (%s, %d bytes).\n", pNode->szBinaryName, sizeof(SAVED_VECTOR) );
						dwLastError = ERROR_OUTOFMEMORY;
						leave;
					}
					
					pNewVectorNode->next = NULL;
					
					pNewVectorNode->pSavedBitVector = HeapAlloc( CoverMonHeapHandle, 0, dwVectorSize );
					
					if( pNewVectorNode->pSavedBitVector == NULL ){
						DebugPrint( "COVERMON: Failed to allocate memory for vector (%s, %d bytes).\n", pNode->szBinaryName, dwVectorSize );
						dwLastError = ERROR_OUTOFMEMORY;
						leave;
					}
					
					CopyMemory( pNewVectorNode->pSavedBitVector, pNewBitVector, dwVectorSize );
					
					pNewVectorNode->pszTestName = HeapAlloc( CoverMonHeapHandle, 0, strlen(pszTestName)+1 );
					
					if( pNewVectorNode->pszTestName == NULL ){
						DebugPrint( "COVERMON: Failed to allocate memory for vector name (%s, %d bytes).\n", pNode->szBinaryName, strlen(pszTestName)+1 );
						dwLastError = ERROR_OUTOFMEMORY;
						leave;
					}
					
					strcpy( pNewVectorNode->pszTestName, pszTestName );
				}

				if( pNode->pLastSavedBitVectorList == NULL ){
					pNode->pLastSavedBitVectorList = pNewVectorNode;
					
				} else if( bCacheAlways ){
					for( pTmpVectorNode = pNode->pLastSavedBitVectorList;
						pTmpVectorNode != NULL;
						pTmpVectorNode = pTmpVectorNode->next ){
						pPrvVectorNode = pTmpVectorNode;
					}
					pPrvVectorNode->next = pNewVectorNode;
					
				}
				
				pNode->dwSavedVectorCount++;
			}
        }
		
        bSuccess = true;
    }
	
    except( EXCEPTION_EXECUTE_HANDLER ) {
		
        DebugPrint(
            "COVERMON: LOST DATA for %s because an exception occurred while "
            "updating the file.  This probably indicates a corrupt data file.\n",
            pNode->szBinaryName
            );
		
        dwLastError = ERROR_FILE_CORRUPT;
	}
	
    if ( pMoveBuffer ) {
        ExFreePool( pMoveBuffer );
	}

	if( !(bCacheAlways || bCacheMRU) ){
		HeapFree( CoverMonHeapHandle, 0, pOldBitVector );
	}
    HeapFree( CoverMonHeapHandle, 0, pNewBitVector );
	
    SetLastError( dwLastError );
	
    return bSuccess;
}


DWORD
CovMonInternalPollAndSaveLogging(
    IN LPCSTR pszBinaryName,            // must be non-NULL, lowercase
    IN LPCSTR pszTestName               // must be non-NULL (case insensitive)
    )
{
    PCOVMON_REGISTERED_BINARY pNode;
    DWORD  dwReturnCode = 0;
    DWORD  dwDataSize = 0;
    DWORD  dwCheckSum = 0;
	MD5_HASH md5;
    bool   bSuccess;
    bool   bKernel;
	
    EnterCriticalSection( &PollBufferCritSect );

    try {
		
        bKernel  = false;
        bSuccess = false;

		if( bShutdownInProgress && bMediaWriteProtected ){
//			DebugPrint("COVERMON: Nope! No more! %s\n", pszBinaryName);
			leave;
		}

        try {

            bSuccess = false;
            /*bSuccess = CoveragePollLogging(
				pszBinaryName,
				dwPollBufferSize,
				pPollBuffer,
				&dwDataSize,
				&dwCheckSum,
				&md5
				);*/
			
            if (( ! bSuccess ) && ( hKernelCoverageHandle )) {
				
                bKernel = true;
				
                bSuccess = KernelCoveragePollLogging(
					hKernelCoverageHandle,
					pszBinaryName,
					dwPollBufferSize,
					pPollBuffer,
					&dwDataSize,
					&dwCheckSum
					);
			}
		}
        except( EXCEPTION_EXECUTE_HANDLER ) {
            SetLastError( GetExceptionCode() );
		}
		
        if ( ! bSuccess ) {
            dwReturnCode = GetLastError();
            DebugPrint(
                "COVERMON: Poll %s failed (error %d)\n",
                pszBinaryName,
                dwReturnCode
                );
            leave;
		}
		
        if ( DoesBufferContainAllZeros( pPollBuffer, dwDataSize )) {
            if ( ! bShutdownInProgress ) {
                DebugPrint(
                    "COVERMON: No new data for %s.\n",
                    pszBinaryName
                    );
			}
            leave;
		}
		
        ASSERT( DoesBufferContainOnlyHex01s( pPollBuffer, dwDataSize ));

		// If this is a kernel mode binary, then extract the MD5 hash
		// from the end of the logging buffer
		if( bKernel ){
			CopyMemory(&md5, (PCHAR)pPollBuffer + dwDataSize, MD5_SIZE);
		}

        //
        //  If we get to here, we polled some data and we need to save it.
        //
		
        DebugPrint( "COVERMON: Poll %s, save as \"%s\"\n", pszBinaryName, pszTestName );
		
        //
        //  Now find or add registration entry.
        //
		
        pNode = CovMonInternalFindOrAddRegistration( pszBinaryName, dwDataSize, dwCheckSum, bKernel, &md5 );
		
        if ( pNode == NULL ) {
            dwReturnCode = GetLastError();
            DebugPrint(
                "COVERMON: LOST DATA for %s because failed to allocate memory (error %d)\n",
                pszBinaryName,
                dwReturnCode
                );
            leave;
		}
		
        if ( pNode->hLogFileHandle != NULL ) {
			
            if (( pNode->dwCheckSumOfBinary != dwCheckSum ) ||
                ( pNode->dwBlockCount != dwDataSize )) {
				
                DebugPrint(
                    "COVERMON: Closing covdata file for %s (checksums don't match)\n",
                    pszBinaryName
                    );
				
                pNode->dwUniquifier = 0;
				
                HeapFree( CoverMonHeapHandle, 0, pNode->pCachedCovFileHeader );
                pNode->pCachedCovFileHeader = NULL;
				
                CloseHandle( pNode->hLogFileHandle );
                pNode->hLogFileHandle = NULL;
				
                pNode->dwCheckSumOfBinary = dwCheckSum;
                pNode->dwBlockCount       = dwDataSize;
				
			}
		}
		
        if ( pNode->hLogFileHandle == NULL ) {
            if ( ! CovMonInternalOpenLogFile( pNode )) {
                dwReturnCode = GetLastError();
                leave;
			}
		}
		
		if ( ! bMediaWriteProtected ){
			if ( ! CovMonInternalUpdateLogFile( pNode, pszTestName )) {
				dwReturnCode = GetLastError();
				leave;
			}
		}
    }
	
    finally {
		
        LeaveCriticalSection( &PollBufferCritSect );
		
	}
	
    return dwReturnCode;
}


DWORD
WINAPI
CovMonInternalPollLoggingAllBinaries(
    LPCSTR TestName
    )
{
    DWORD dwReturnOne;
    DWORD dwReturnAny;
    DWORD dwDataSize;
    DWORD nBinaries;
    bool  bSuccess;
    PCHAR p;

    if ( !CovMonIsRunning() ) {
        return ERROR_INVALID_HANDLE;
    }

    EnterCriticalSection( &NameBufferCritSect );

    if (( TestName == NULL ) || ( *TestName == 0 )) {
        TestName = "(no test name)";
	}
	
    dwReturnAny = 0;
    nBinaries = 0;
	
    if ( hKernelCoverageHandle ) {
		
        bSuccess = KernelCoverageEnumerateBinaries(
			hKernelCoverageHandle,
			sizeof( szNameBuffer ),
			szNameBuffer,
			&dwDataSize
			);
		
        if ( bSuccess ) {

            for ( p = szNameBuffer; *p; p += (strlen(p)+MD5_SIZE+2) ) {
				
                dwReturnOne = CovMonInternalPollAndSaveLogging( p, TestName );
				
                if ( ! dwReturnAny ) {
                    dwReturnAny = dwReturnOne;
				}
				
                nBinaries++;
			}
		}
	}
	
    LeaveCriticalSection( &NameBufferCritSect );

    return ( dwReturnAny ? dwReturnAny : ( nBinaries ? 0 : ERROR_FILE_NOT_FOUND ));
}


VOID
CovMonInternalCloseLogFile(
    PCOVMON_REGISTERED_BINARY pNode
    )
{
    if ( pNode->hLogFileHandle != NULL ) {

		if( (bCacheAlways || bCacheMRU) &&
			(pNode->pLastSavedBitVectorList != NULL)
			){
			DebugPrint("COVERMON: Clearing cache for %s\n", pNode->szBinaryName);
			CovMonInternalClearCache( pNode );
		}
		
        DebugPrint(
            "COVERMON: Closing covdata file for %s\n",
            pNode->szBinaryName
            );
		
        pNode->dwUniquifier = 0;
		
        CloseHandle( pNode->hLogFileHandle );
        pNode->hLogFileHandle = NULL;
		
        if ( pNode->pCachedCovFileHeader != NULL ) {
            HeapFree( CoverMonHeapHandle, 0, pNode->pCachedCovFileHeader );
            pNode->pCachedCovFileHeader = NULL;
		}
	}
}


DWORD
WINAPI
CovMonInternalCloseLogFileAllBinaries(
    VOID
    )
{
    PCOVMON_REGISTERED_BINARY pNode;

    if ( !CovMonIsRunning() ) {
        return ERROR_INVALID_HANDLE;
    }
	
    EnterCriticalSection( &RegistrationCritSect );
	
    //
    //  Walk list of registered binaries
    //
	
    pNode = RegisteredBinaryList;
	
    while ( pNode != (PCOVMON_REGISTERED_BINARY)&RegisteredBinaryList ) {
		
        CovMonInternalCloseLogFile( pNode );
		
        pNode = pNode->pNext;
		
	}
	
    LeaveCriticalSection( &RegistrationCritSect );
	
    return ERROR_SUCCESS;
}


BOOL
WINAPI
CovMonRegisterNoImportCoverageBinary(
    HANDLE ImageBase
    )
{
    if ( !CovMonIsRunning() ) {
        SetLastError( ERROR_INVALID_HANDLE );
        return FALSE;
    }

    return KernelRegisterNoImportCoverageBinary( hKernelCoverageHandle, ImageBase );
}


BOOL
WINAPI
CovMonRegisterAllNoImportCoverageBinaries(
    VOID
    )
{
    if ( !CovMonIsRunning() ) {
        SetLastError( ERROR_INVALID_HANDLE );
        return FALSE;
    }

    return KernelRegisterAllNoImportCoverageBinaries( hKernelCoverageHandle );
}
