
#include "precomp.h"
#pragma hdrstop


//
//  Ideas:
//
//  * Mechanism to cancel outstanding and remaining reads.
//



typedef struct _READ_BLOCK
    READ_BLOCK,
    *PREAD_BLOCK;

typedef struct _READER_CONTEXT
    READER_CONTEXT,
    *PREADER_CONTEXT;

struct _READ_BLOCK {
    PREAD_BLOCK     pNextBlock;             // must be first member
    PVOID           pBuffer;
    DWORD           dwSize;
    DWORD           dwStatus;
    LPOVERLAPPED    pOverlapped;
    };

struct _READER_CONTEXT {
    HANDLE         hFile;
    LPCWSTR        pUnicodeName;
    LPCSTR         pFileName;               // used if pUnicodeName is NULL
    DWORDLONG      dwlFileSize;
    DWORD          dwBufferSize;
    DWORD          dwSectorSize;
    DWORDLONG      dwlOffset;               // offset to start reading
    PVOID          FakeEmptyHeadNode;       // assumes pNext is first member
    PREAD_BLOCK    pReadListHead;
    PREAD_BLOCK    pReadListTail;
    HANDLE         hSemCanStartNewRead;
    HANDLE         hSemQueuedReads;
    HANDLE         hEventThreadFinished;
    HANDLE         hEventAllReadsIssued;
    HANDLE         hThreadReader;
    BOOL           bStopReading;
    };


RECYCLE_LIST_HEAD RecycledReaderContextList;

PREADER_CONTEXT
AllocateReaderContext(
    VOID
    )
    {
    PREADER_CONTEXT pContext;

    pContext = GetObjectFromRecycleList( &RecycledReaderContextList );

    if ( pContext == NULL ) {
         pContext = MyAllocNeverFreeLocked( sizeof( READER_CONTEXT ));
         }
    else {
         ZeroMemory( pContext, sizeof( READER_CONTEXT ));
         }

    return pContext;
    }

VOID
INLINE
RecycleReaderContext(
    PREADER_CONTEXT pContext
    )
    {
    RecycleObjectToList( &RecycledReaderContextList, pContext );
    }


RECYCLE_LIST_HEAD RecycledReadBlockList;

PREAD_BLOCK
AllocateReadBlock(
    VOID
    )
    {
    PREAD_BLOCK Block;

    Block = GetObjectFromRecycleList( &RecycledReadBlockList );

    if ( Block == NULL ) {
         Block = MyAllocNeverFreeLocked( sizeof( READ_BLOCK ));
         }
    else {
         ZeroMemory( Block, sizeof( READ_BLOCK ));
         }

    return Block;
    }

VOID
INLINE
RecycleReadBlock(
    PREAD_BLOCK pBlock
    )
    {
    RecycleObjectToList( &RecycledReadBlockList, pBlock );
    }



VOID
ReaderThread(
    PVOID pParam
    )
    {
    PREADER_CONTEXT pContext     = pParam;
    HANDLE          hFile        = pContext->hFile;
    DWORD           dwSectorSize = pContext->dwSectorSize;
    DWORD           dwBufferSize = pContext->dwBufferSize;
    DWORDLONG       dwlOffset    = pContext->dwlOffset;
    DWORDLONG       dwlFileSize  = pContext->dwlFileSize;
    DWORDLONG       dwlRemaining = dwlFileSize - dwlOffset;
    DWORD           dwReadSize;
    DWORD           dwExpected;
    DWORD           dwActual;
    BOOL            bSuccess;
    PREAD_BLOCK     pBlock;


    while ( dwlRemaining ) {

        WaitForSingleObject( pContext->hSemCanStartNewRead, INFINITE );

        if ( pContext->bStopReading )
            break;

        if ( dwlRemaining >= dwBufferSize ) {
            dwReadSize = dwBufferSize;
            dwExpected = dwBufferSize;
            }
        else {
            dwReadSize = ROUNDUP2( dwlRemaining, dwSectorSize );
            dwExpected = (DWORD)dwlRemaining;
            }

        dwlRemaining -= dwExpected;

        pBlock                          = AllocateReadBlock();
        ASSERT(pBlock != NULL);
        pBlock->pBuffer                 = AllocateBuffer( dwReadSize, FALSE );
        ASSERT(pBlock->pBuffer != NULL);
        pBlock->pOverlapped             = AllocateOverlappedWithEvent();
        ASSERT(pBlock->pOverlapped != NULL);
        pBlock->pOverlapped->Offset     = (DWORD)( dwlOffset );
        pBlock->pOverlapped->OffsetHigh = (DWORD)( dwlOffset >> 32 );
        pBlock->dwSize                  = dwExpected;
        pBlock->dwStatus                = dwlRemaining ? 0 : STATUS_END_OF_FILE;
        ASSERT(pBlock->pOverlapped->hEvent != NULL);

#ifdef UDF_DBG
        ASSERT(dwReadSize <= 2048);
        ASSERT(dwExpected <= 2048);
#endif

        if ( bGlobalErrorExitInProgress )   // don't issue new read
        {
            return;
        }

        bSuccess = ReadFile( hFile,
                             pBlock->pBuffer,
                             dwReadSize,
                             &dwActual,
                             pBlock->pOverlapped );

        if ( bSuccess ) {

            //
            //  The read completed synchronously, so we'll fill
            //  in the overlapped structure to mimic overlapped
            //  behavior so the completion routine doesn't have
            //  to know the difference.
            //

            pBlock->pOverlapped->Internal     = 0;          // STATUS_SUCCESS
            pBlock->pOverlapped->InternalHigh = dwActual;   // actually read

            //
            //  Assume here that even though the ReadFile completed
            //  synchronously, it was kind enough to set the overlapped
            //  event to signaled state.
            //

            }

        else if ( GetLastError() != ERROR_IO_PENDING ) {
            ErrorExit( GETLASTERROR,
                       pContext->pUnicodeName ?
                            "ReadFile failed (%S, off=%I64X len=%X status=%X)\r\n" :
                            "ReadFile failed (%s, off=%I64X len=%X status=%X)\r\n",
                       pContext->pUnicodeName ?
                            (PVOID) pContext->pUnicodeName :
                            (PVOID) pContext->pFileName,
                       dwlOffset,
                       dwReadSize,
                       pBlock->pOverlapped->Internal
                     );
            }

        //
        //  Now queue this read block on the read list and bump the
        //  reader semaphore to reflect the valid size of the list
        //

        pContext->pReadListTail->pNextBlock = pBlock;
        pContext->pReadListTail             = pBlock;

        ReleaseSemaphore( pContext->hSemQueuedReads, 1, NULL );

        dwlOffset += dwReadSize;

        }

    //
    //  Finished reading file.  Queue a null read block to indicate EOF.
    //

    pContext->pReadListTail->pNextBlock = NULL;
    ReleaseSemaphore( pContext->hSemQueuedReads, 1, NULL );
    if ( pContext->hEventAllReadsIssued != NULL )
        SetEvent( pContext->hEventAllReadsIssued );
    SetEvent( pContext->hEventThreadFinished );
    }


HANDLE
CreateReadContext(
    LPCWSTR   pUnicodeName,             // primary name
    LPCSTR    pFileName,                // secondary name (if primary is NULL)
    HANDLE    hFile,                    // optional
    DWORD     dwSectorSize,             // optional
    DWORD     dwBufferSize,             // optional
    DWORD     nMaxReadAhead,            // optional
    HANDLE    hEventAllReadsIssued,     // optional
    DWORDLONG dwlInitialOffset,         // optional
    DWORDLONG dwlFileSize               // optional
    )
    {
    PREADER_CONTEXT pContext;
    DWORD dwShare;

    if(bOpenReadWrite)
        dwShare=FILE_SHARE_READ | FILE_SHARE_WRITE;
    else
        dwShare=FILE_SHARE_READ;

    if ( hFile == NULL ) {

        if ( pUnicodeName ) {

            hFile = CreateFileW( pUnicodeName,
                                 GENERIC_READ,
                                 dwShare,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                                 NULL );
            }

        else {

            hFile = CreateFileA( pFileName,
                                 GENERIC_READ,
                                 dwShare,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                                 NULL );

            if (( hFile == INVALID_HANDLE_VALUE ) &&
                ( GetLastError() == ERROR_FILE_NOT_FOUND )) {

                WarnFailedSourceOpen(
                    0,
                    "Could not open file \"%s\"\r\n"
                    "The specific error code (file not found) could indicate that the file was\r\n"
                    "deleted by another process after the directory scan, or it could indicate\r\n"
                    "that the filename contains some Unicode characters that do not have a\r\n"
                    "corresponding %s character mapping (try %susing -c, or use -j1 or -j2\r\n"
                    "for full Unicode names).\r\n",
                    pFileName,
                    bUseAnsiFileNames ? "ANSI" : "OEM",
                    bUseAnsiFileNames ? "not " : ""
                    );

                return NULL;
                }
            }

        if ( hFile == INVALID_HANDLE_VALUE ) {

            WarnFailedSourceOpen(
                GETLASTERROR,
                pUnicodeName ?
                    "Could not open file \"%S\"\r\n" :
                    "Could not open file \"%s\"\r\n",
                pUnicodeName ?
                    (PVOID) pUnicodeName :
                    (PVOID) pFileName
                );

            return NULL;

            }
        }

    if ( dwSectorSize == 0 ) {

        dwSectorSize = GetSectorSize( hFile, pUnicodeName, pFileName );
        ASSERT( dwSectorSize != 0 );

        }

    ASSERT( ISPOWER2( dwSectorSize ));
    ASSERT( dwSectorSize >= 512 );

    if ( dwBufferSize == 0 )
        dwBufferSize = dwSectorSize;

    ASSERT( ISALIGN2( dwBufferSize, dwSectorSize ));

    if ( nMaxReadAhead == 0 )
        nMaxReadAhead = 0x7FFF0000;

    if ( dwlFileSize == 0 )
        dwlFileSize = GetFileSize64( hFile );

    ASSERT( dwlFileSize != 0xFFFFFFFF );
    ASSERT( dwlFileSize != 0 );
    ASSERT( dwlFileSize > dwlInitialOffset );
    ASSERT( dwlFileSize == GetFileSize64(hFile));

    if ( dwlFileSize == 0 )
        return NULL;

    if(dwlFileSize != GetFileSize64(hFile))
    {

        if ( bContinueAfterFailedSourceOpen )
        {

            printf("%s: File size doesn't match orignal scanned size\r\n",
                   pFileName);
        }
        else
        {
            ErrorExit(0,
                      "%s: File size doesn't match orignal scanned size\r\n",
                      pFileName);
        }
    }

    pContext                       = AllocateReaderContext();
    pContext->hFile                = hFile;
    pContext->pUnicodeName         = pUnicodeName;
    pContext->pFileName            = pFileName;
    pContext->dwlFileSize          = dwlFileSize;
    pContext->dwlOffset            = dwlInitialOffset;
    pContext->dwBufferSize         = dwBufferSize;
    pContext->dwSectorSize         = dwSectorSize;
    pContext->pReadListHead        = (PREAD_BLOCK) &pContext->FakeEmptyHeadNode;
    pContext->pReadListTail        = (PREAD_BLOCK) &pContext->FakeEmptyHeadNode;
    pContext->hSemCanStartNewRead  = MyCreateSemaphore( nMaxReadAhead );
    pContext->hSemQueuedReads      = MyCreateSemaphore( 0 );
    pContext->hEventThreadFinished = AllocateAutoEvent();
    pContext->hEventAllReadsIssued = hEventAllReadsIssued;
    pContext->bStopReading         = FALSE;

    StartThread( ReaderThread, pContext, THREAD_PRIORITY_HIGHEST );

    return (HANDLE) pContext;
    }


DWORD
WaitForRead(
    HANDLE hContext,
    PVOID  *pBuffer,
    DWORD  *dwSize
    )
    {
    PREADER_CONTEXT pContext = (PREADER_CONTEXT) hContext;
    PREAD_BLOCK pBlock;
    DWORD dwActual;
    DWORD dwStatus;

    //
    //  Wait for ReaderThread to enqueue an active read.
    //

    WaitForSingleObject( pContext->hSemQueuedReads, INFINITE );

    //
    //  Now pReadListHead->pNextBlock is valid.  Unless pReadListHead still
    //  points to our FakeEmptyHeadNode, we can now free the old block that
    //  was at the head of the list.
    //

    pBlock = pContext->pReadListHead->pNextBlock;

    ASSERT(pBlock != NULL);
    ASSERT(pBlock->pOverlapped != NULL);
    ASSERT(pBlock->pOverlapped->hEvent != NULL);

    if ( pContext->pReadListHead != (PREAD_BLOCK)&pContext->FakeEmptyHeadNode )
        RecycleReadBlock( pContext->pReadListHead );

    pContext->pReadListHead = pBlock;

    //
    //  GetOverlappedResult will wait for the hEvent to be signaled
    //  indicating the read has been completed.
    //

    //
    //  Rather than letting GetOverlappedResult do the wait, do it
    //  ourselves and then tell GetOverlappedResult not to wait.
    //  GetOverlappedResult will not do the wait call if the i/o
    //  has already completed, so it might leave our event signaled.
    //

    ASSERT(pBlock != NULL);
    ASSERT(pBlock->pOverlapped != NULL);
    ASSERT(pBlock->pOverlapped->hEvent != NULL);

    WaitForSingleObject( pBlock->pOverlapped->hEvent, INFINITE );

    if ( ! GetOverlappedResult( NULL, pBlock->pOverlapped, &dwActual, FALSE )) {
        ErrorExit(
            GETLASTERROR,
            pContext->pUnicodeName ?
                "ReadFile/GetOverlappedResult failed (%S, off=%I64X len=%X status=%X)\r\n" :
                "ReadFile/GetOverlappedResult failed (%s, off=%I64X len=%X status=%X)\r\n",
            pContext->pUnicodeName ?
                (PVOID) pContext->pUnicodeName :
                (PVOID) pContext->pFileName,
            ((DWORDLONG)( pBlock->pOverlapped->OffsetHigh ) << 32 ) | pBlock->pOverlapped->Offset,
            ROUNDUP2( pBlock->dwSize, pContext->dwSectorSize ),
            pBlock->pOverlapped->Internal
            );
        }

    *dwSize  = dwActual;
    *pBuffer = pBlock->pBuffer;
    dwStatus = pBlock->dwStatus;

    ASSERT((  dwActual == pBlock->dwSize ) ||
           (( dwActual >= pBlock->dwSize ) && ( dwStatus != 0 )));

    //
    //  Allow the ReaderThread to begin its next read.
    //

    ReleaseSemaphore( pContext->hSemCanStartNewRead, 1, NULL );

    //
    //  No longer need this overlapped structure or associated event.
    //

    RecycleOverlappedWithEvent( pBlock->pOverlapped );

    //
    //  Leave pBlock->pBuffer active -- caller must ReleaseBuffer
    //  when done with it.
    //
    //  Also must leave pBlock at the pReadListHead postition so
    //  the ReaderThread can update its pNextBlock member.  We'll
    //  free it next time through.
    //

    // BUGBUG
//    printf("WaitForRead - Memory = %p\r\n",*pBuffer);
    return dwStatus;

    }


HANDLE
GetFileHandleFromReadContext(
    HANDLE hContext
    )
    {
    return ((PREADER_CONTEXT) hContext )->hFile;
    }


VOID
StopIssuingReads(
    HANDLE hContext
    )
    {
    ((PREADER_CONTEXT) hContext )->bStopReading = TRUE;
    }


VOID
CloseReadContext(
    HANDLE hContext,
    BOOL   bCloseFile
    )
    {
    PREADER_CONTEXT pContext = (PREADER_CONTEXT) hContext;
    PREAD_BLOCK     pBlock;
    PREAD_BLOCK     pNext;
    DWORD           dwActual;

    //
    //  Tell ReaderThread to stop issuing new reads and release
    //  ReaderThread to detect this in case it's blocked.
    //

    pContext->bStopReading = TRUE;

    ReleaseSemaphore( pContext->hSemCanStartNewRead, 1, NULL );

    //
    //  Now wait for reader thread to exit.  Once it exits, we
    //  know the ReadList is static so we can wait/close all
    //  the outstanding reads.
    //

    WaitForSingleObject( pContext->hEventThreadFinished, INFINITE );

    pBlock = pContext->pReadListHead;

    if ( pBlock == (PREAD_BLOCK)&pContext->FakeEmptyHeadNode ) {
        pBlock = pBlock->pNextBlock;
        }
    else {

        //
        //  Head node already has overlapped and event recycled,
        //  so just need to recycle the block itself.
        //

        pNext = pBlock->pNextBlock;
        RecycleReadBlock( pBlock );
        pBlock = pNext;
        }

    while ( pBlock ) {
        pNext = pBlock->pNextBlock;
        WaitForSingleObject( pBlock->pOverlapped->hEvent, INFINITE );
        GetOverlappedResult( NULL, pBlock->pOverlapped, &dwActual, FALSE );
        ReleaseBuffer( pBlock->pBuffer );
        RecycleOverlappedWithEvent( pBlock->pOverlapped );
        RecycleReadBlock( pBlock );
        pBlock = pNext;
        }

    RecycleAutoEvent( pContext->hEventThreadFinished );
    CloseHandle( pContext->hSemCanStartNewRead );
    CloseHandle( pContext->hSemQueuedReads );

    if ( bCloseFile )
        CloseHandle( pContext->hFile );

    RecycleReaderContext( pContext );

    }


DWORDLONG
GetFileSizeFromReadContext(
    HANDLE hContext
    )
    {
    return ((PREADER_CONTEXT) hContext )->dwlFileSize;
    }


DWORDLONG
GetFileSize64(
    IN HANDLE hFile
    )
    {
    DWORD dwFileSizeHigh = 0;
    DWORD dwFileSizeLow  = GetFileSize( hFile, &dwFileSizeHigh );

    if ( dwFileSizeLow == 0xFFFFFFFF ) {
        if ( GetLastError() != NO_ERROR ) {
            ErrorExit( GETLASTERROR, "GetFileSize failed\r\n" );
            }
        }

    return ((DWORDLONG)dwFileSizeHigh << 32 ) | dwFileSizeLow;
    }

