
#include "precomp.h"
#pragma hdrstop


//
//  Ideas:
//
//  * Mechanism to cancel outstanding writes, wait for outstanding writes
//    to complete, resume writing.
//
//  * Make high priority completion thread that only releases the buffer
//    (minimal necessary to allow new i/o to start), and an IDLE priority
//    cleaning up the other completion stuff.
//


typedef struct _WRITE_BLOCK
    WRITE_BLOCK,
    *PWRITE_BLOCK;

typedef struct _WRITER_CONTEXT
    WRITER_CONTEXT,
    *PWRITER_CONTEXT;

struct _WRITE_BLOCK {
    PWRITE_BLOCK        pNextBlock;         // must be first member of struct
    PVOID               pBuffer;
    DWORDLONG           dwlOffset;
    DWORD               dwValidSize;
    DWORD               dwWriteSize;
    LPOVERLAPPED        pOverlapped;
    WRITE_COMPLETION    pCompletion;
    PVOID               pUserContext;
    };

struct _WRITER_CONTEXT {
    HANDLE           hFile;
    LPCSTR           pFileName;
    DWORD            dwSectorSize;
    DWORDLONG        dwlSectorFileSize;
    DWORDLONG        dwlValidFileSize;
    DWORD            nMaxActiveWrites;
    PVOID            FakeEmptyHeadNode;      // initialize to NULL
    PWRITE_BLOCK     pQueuedWriteListHead;   // initialize to &FakeEmptyHeadNode
    PWRITE_BLOCK     pQueuedWriteListTail;   // initialize to &FakeEmptyHeadNode
    HANDLE           hSemQueuedWrites;       // initialize to zero
    HANDLE           hSemActiveWrites;       // initialize to zero
    HANDLE           hSemCanStartNewWrite;   // init to nMaxActiveWrites
    HANDLE           hEventThreadDone;
    CRITICAL_SECTION ActiveZoneCritSect;
    DWORDLONG        dwlActiveZoneLowerLimit;
    DWORDLONG        dwlActiveZoneUpperLimit;
    HANDLE           hEventActiveZoneLeave;
    LONG             nZoneWaiters;
    DWORD            nZoneEntries;
#ifdef DEBUG
    UINT             nCountSync;
    UINT             nCountAsync;
#endif
    PVOID            FakeMaybeHeadNode;      // initialize to NULL
    PWRITE_BLOCK     pMaybeWriteListHead;    // initialize to &FakeEmptyMaybeNode
    PWRITE_BLOCK     pMaybeWriteListTail;    // initialize to &FakeEmptyMaybeNode
    DWORD            dwQueuedMaybeBytes;     // initialize to zero
    BOOL             bAbortWriting;
    };


RECYCLE_LIST_HEAD RecycledWriterContextList;

PWRITER_CONTEXT
AllocateWriterContext(
    VOID
    )
    {
    PWRITER_CONTEXT pContext;

    pContext = GetObjectFromRecycleList( &RecycledWriterContextList );

    if ( pContext == NULL ) {
         pContext = MyAllocNeverFreeLocked( sizeof( WRITER_CONTEXT ));
         }
    else {
         ZeroMemory( pContext, sizeof( WRITER_CONTEXT ));
         }

    return pContext;
    }


VOID
INLINE
RecycleWriterContext(
    PWRITER_CONTEXT pContext
    )
    {
    RecycleObjectToList( &RecycledWriterContextList, pContext );
    }


RECYCLE_LIST_HEAD RecycledWriteBlockList;

PWRITE_BLOCK
AllocateWriteBlock(
    VOID
    )
    {
    PWRITE_BLOCK Block;

    Block = GetObjectFromRecycleList( &RecycledWriteBlockList );

    if ( Block == NULL ) {
         Block = MyAllocNeverFreeLocked( sizeof( WRITE_BLOCK ));
         }
    else {
         ZeroMemory( Block, sizeof( WRITE_BLOCK ));
         }

    return Block;
    }


VOID
INLINE
RecycleWriteBlock(
    PWRITE_BLOCK pBlock
    )
    {
    RecycleObjectToList( &RecycledWriteBlockList, pBlock );
    }


RECYCLE_LIST_HEAD RecycledOverlappedList;

LPOVERLAPPED
AllocateOverlappedWithEvent(
    VOID
    )
    {
    LPOVERLAPPED pOverlapped;

    pOverlapped = GetObjectFromRecycleList( &RecycledOverlappedList );

    if ( pOverlapped == NULL ) {
         pOverlapped = MyAllocNeverFreeLocked( sizeof( OVERLAPPED ));
         pOverlapped->hEvent = AllocateAutoEvent();
         }
    else {
         pOverlapped->Internal     = 0;
         pOverlapped->InternalHigh = 0;
         pOverlapped->Offset       = 0;
         pOverlapped->OffsetHigh   = 0;
         ASSERT( EventStateUnsignaled( pOverlapped->hEvent ));
         }

    return pOverlapped;
    }


VOID
RecycleOverlappedWithEvent(
    LPOVERLAPPED pOverlapped
    )
    {
    ASSERT( EventStateUnsignaled( pOverlapped->hEvent ));
    RecycleObjectToList( &RecycledOverlappedList, pOverlapped );
    }


VOID
EnterActiveZone(
    PWRITER_CONTEXT pContext,
    DWORDLONG       dwlOffset,
    DWORD           dwSize
    )
    {
    DWORDLONG dwlLower;
    DWORDLONG dwlUpper;
    DWORDLONG dwlExtent = dwlOffset + dwSize;
    BOOL      bReturn   = FALSE;

    for (;;) {

        EnterCriticalSection( &pContext->ActiveZoneCritSect );

        if ( pContext->nZoneEntries == 0 ) {
            pContext->dwlActiveZoneLowerLimit = dwlOffset;
            pContext->dwlActiveZoneUpperLimit = dwlExtent;
            pContext->nZoneEntries++;
            bReturn = TRUE;
            }
        else {

            dwlLower = pContext->dwlActiveZoneLowerLimit;
            dwlUpper = pContext->dwlActiveZoneUpperLimit;

            if (( dwlOffset >= dwlUpper ) || ( dwlExtent <= dwlLower )) {

                if ( dwlOffset < dwlLower )
                    pContext->dwlActiveZoneLowerLimit = dwlOffset;

                if ( dwlExtent > dwlUpper )
                    pContext->dwlActiveZoneUpperLimit = dwlExtent;

                pContext->nZoneEntries++;

                bReturn = TRUE;
                }
            else {
                InterlockedIncrement( &pContext->nZoneWaiters );
                }
            }

        LeaveCriticalSection( &pContext->ActiveZoneCritSect );

        if ( bReturn )
            return;

        WaitForSingleObject( pContext->hEventActiveZoneLeave, INFINITE );
        InterlockedDecrement( &pContext->nZoneWaiters );

        }
    }


VOID
LeaveActiveZone(
    PWRITER_CONTEXT pContext,
    DWORDLONG       dwlOffset,
    DWORD           dwSize
    )
    {
    DWORDLONG dwlLower;
    DWORDLONG dwlUpper;
    DWORDLONG dwlExtent = dwlOffset + dwSize;
    DWORD nZoneWaiters;

    EnterCriticalSection( &pContext->ActiveZoneCritSect );

    dwlLower = pContext->dwlActiveZoneLowerLimit;
    dwlUpper = pContext->dwlActiveZoneUpperLimit;

    if (( dwlOffset <= dwlLower ) && ( dwlExtent > dwlLower ))
        dwlLower = dwlExtent;

    if (( dwlExtent >= dwlUpper ) && ( dwlOffset < dwlUpper ))
        dwlUpper = dwlOffset;

    if ( dwlLower >= dwlUpper ) {
        pContext->dwlActiveZoneLowerLimit = 0xFFFFFFFFFFFFFFFF;
        pContext->dwlActiveZoneUpperLimit = 0;
        }

    pContext->nZoneEntries--;

    nZoneWaiters = pContext->nZoneWaiters;

    LeaveCriticalSection( &pContext->ActiveZoneCritSect );

    if ( nZoneWaiters )
        SetEvent( pContext->hEventActiveZoneLeave );

    }


VOID
WriterThread(
    PVOID pParam
    )
    {
    PWRITER_CONTEXT pContext = pParam;
    PWRITE_BLOCK    pBlock   = pContext->pQueuedWriteListHead;
    DWORD           dwActual;

    while ( pBlock ) {

        WaitForSingleObject( pContext->hSemQueuedWrites,     INFINITE );
        WaitForSingleObject( pContext->hSemCanStartNewWrite, INFINITE );

        pBlock = pBlock->pNextBlock;

        if ( pBlock ) {

            if ( pBlock->dwWriteSize > 0 ) {

                EnterActiveZone( pContext, pBlock->dwlOffset, pBlock->dwWriteSize );

                if ( bGlobalErrorExitInProgress )
                     pContext->bAbortWriting = TRUE;

                if ( pContext->bAbortWriting ) {

                    //
                    //  Don't issue new write, but allow completion thread
                    //  to continue processing blocks until they're all gone.
                    //

                    pBlock->pOverlapped->Internal = (DWORD)STATUS_CANCELLED;
                    SetEvent( pBlock->pOverlapped->hEvent );

                    }

                else {

                    if ( WriteFile( pContext->hFile,
                                    pBlock->pBuffer,
                                    pBlock->dwWriteSize,
                                    &dwActual,
                                    pBlock->pOverlapped )) {

                        //
                        //  Assume here that even though the WriteFile completed
                        //  synchronously, it was kind enough to set the
                        //  appropriate information in the overlapped structure
                        //  so that we can still call GetOverlappedResult.
                        //

                        ASSERT( pBlock->pOverlapped->Internal == 0 );
                        ASSERT( pBlock->pOverlapped->InternalHigh == dwActual );

                        DEBUGCODE( pContext->nCountSync++ );
                        }

                    else if ( GetLastError() == ERROR_IO_PENDING ) {
                        DEBUGCODE( pContext->nCountAsync++ );
                        }
                    else {
                        ErrorExit( GETLASTERROR,
                                   "WriteFile failed (%s, off=%I64X len=%X status=%X)\r\n",
                                   pContext->pFileName,
                                   pBlock->dwlOffset,
                                   pBlock->dwWriteSize,
                                   pBlock->pOverlapped->Internal
                                 );
                        }
                    }
                }
            }

        ReleaseSemaphore( pContext->hSemActiveWrites, 1, NULL );
        }

    ReleaseSemaphore( pContext->hSemActiveWrites, 1, NULL );
    }


VOID
CompletionThread(
    PVOID pParam
    )
    {
    PWRITER_CONTEXT pContext = pParam;
    PWRITE_BLOCK    pBlock   = pContext->pQueuedWriteListHead;
    PWRITE_BLOCK    pNext;
    DWORD           dwActual;

    while ( pBlock ) {

        WaitForSingleObject( pContext->hSemActiveWrites, INFINITE );

        pNext = pBlock->pNextBlock;

        if ( pBlock != (PWRITE_BLOCK)&pContext->FakeEmptyHeadNode )
            RecycleWriteBlock( pBlock );

        pBlock = pNext;

        if ( pBlock ) {

            if ( pBlock->dwWriteSize > 0 ) {

                //
                //  Wait for overlapped i/o to complete
                //

                WaitForSingleObject( pBlock->pOverlapped->hEvent, INFINITE );

                if ( bGlobalErrorExitInProgress )
                     pContext->bAbortWriting = TRUE;

                if ( ! pContext->bAbortWriting ) {

                    if ( ! GetOverlappedResult( NULL,
                                                pBlock->pOverlapped,
                                                &dwActual,
                                                FALSE )) {

                        //
                        //  Writer operation failed.
                        //

                        ErrorExit( GETLASTERROR,
                                   "WriteFile/GetOverlappedResult failed\r\n"
                                   "(%s, off=%I64X len=%X status=%X)\r\n",
                                   pContext->pFileName,
                                   pBlock->dwlOffset,
                                   pBlock->dwWriteSize,
                                   pBlock->pOverlapped->Internal
                                 );
                        }

                    ASSERT( dwActual == pBlock->dwWriteSize );
                    }

                LeaveActiveZone( pContext, pBlock->dwlOffset, pBlock->dwWriteSize );
                }

            if ( pBlock->pCompletion ) {

                //
                //  Call user-specified callback completion routine.
                //  NOTE:  If the user specified a callback completion
                //  routine, then the USER is responsible for releasing
                //  the buffer.
                //

                ( *pBlock->pCompletion )( pBlock->pBuffer,
                                          pBlock->dwlOffset,
                                          pBlock->dwValidSize,
                                          pBlock->pUserContext );
                }
            else {
                if ( pBlock->pBuffer ) {
                    ReleaseBuffer( pBlock->pBuffer );
                    }
                }

            if ( pBlock->pOverlapped )
                RecycleOverlappedWithEvent( pBlock->pOverlapped );

            ReleaseSemaphore( pContext->hSemCanStartNewWrite, 1, NULL );

            //
            //  Update file size stats
            //

            if ( pBlock->dwWriteSize > 0 ) {

                DWORDLONG dwlSize;

                dwlSize = pBlock->dwlOffset + pBlock->dwValidSize;
                if ( dwlSize > pContext->dwlValidFileSize )
                    pContext->dwlValidFileSize = dwlSize;

                dwlSize = pBlock->dwlOffset + pBlock->dwWriteSize;
                if ( dwlSize > pContext->dwlSectorFileSize )
                    pContext->dwlSectorFileSize = dwlSize;
                }
            }
        }

    SetEvent( pContext->hEventThreadDone );
    }


HANDLE
CreateWriteContext(
    LPCSTR    pFileName,
    DWORD     dwSectorSize,         //  Optional
    DWORDLONG dwlInitialSize,       //  Optional -- grow file before writing
    DWORD     nMaxActiveWrites      //  How far can we get ahead of ourself
    )
    {
    PWRITER_CONTEXT pContext;
    HANDLE          hFile;
    DWORD           dwErrorCode;
    DWORDLONG       dwlSetSize;
    DWORD           dwSetSizeLow;
    DWORD           dwSetSizeHigh;

    ASSERT( ! bScanOnly );

    hFile = CreateFile( pFileName,
                        GENERIC_READ | GENERIC_WRITE | DELETE,
                        0,
                        NULL,
                        OPEN_ALWAYS,
                        FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                        NULL );

    if ( hFile == INVALID_HANDLE_VALUE ) {
        DEBUGCODE( printf( "CreateFile( %s ) failed, GLE=%d\r\n", pFileName, GetLastError() ));
        return NULL;
        }

    //
    //  If we fail anywhere, or if the user Ctrl-C's the app, we want
    //  the in-progress target file to go away.
    //
    //  NOTE: We can only do this on local targets.  A network open will
    //  see the delete-on-close as meaning "don't bother writing this
    //  file because we're going to delete it when we close it".
    //
    //  BUGBUG:  This doesn't work consistently (too bad), so don't
    //           use it.  Just grow the file to the initial size.  If
    //           the user Ctrl-C's the app, then the file system will
    //           grind on the file writing zeroes to all the unwritten
    //           portion of the file.  If the target file is several
    //           hundred megabytes, then this can take many minutes,
    //           and can even prevent other apps from starting until
    //           the whole file is zeroed.
    //

#ifdef DONTCOMPILE
    {
    CHAR szRootPath[ MAX_PATH_LENGTH ];

    if ( GetRootPath( pFileName, szRootPath )) {
        if ( GetDriveType( szRootPath ) == DRIVE_FIXED ) {
            MyDeleteFileOnClose( hFile, TRUE );
            }
        }
    }
#endif

    if ( dwSectorSize == 0 ) {
        dwSectorSize = GetSectorSize( hFile, NULL, pFileName );
        ASSERT( dwSectorSize != 0 );
        DEBUGCODE( printf( "Queried dwSectorSize=%d\r\n", dwSectorSize ));
        }
    else {

#ifdef DEBUG

        DWORD dwSystemSectorSize = GetSectorSize( hFile, NULL, pFileName );
        ASSERT( dwSectorSize == dwSystemSectorSize );
        // ASSERT( ISALIGN2( dwSectorSize, dwSystemSectorSize )); BUGBUG
#endif

        }

    ASSERT( ISPOWER2( dwSectorSize ));
    ASSERT( dwSectorSize >= 512 );

    if ( dwlInitialSize ) {

        dwlSetSize    = ROUNDUP2_64( dwlInitialSize, dwSectorSize );
        dwSetSizeLow  = (DWORD)( dwlSetSize );
        dwSetSizeHigh = (DWORD)( dwlSetSize >> 32 );

        if (( SetFilePointer( hFile, dwSetSizeLow, (LONG*)&dwSetSizeHigh, FILE_BEGIN ) == 0xFFFFFFFF ) ||
            ( ! SetEndOfFile( hFile ))) {

            dwErrorCode = GetLastError();

            if ( dwErrorCode == ERROR_DISK_FULL ) {
                ErrorExit( 0, "Insufficient disk space for %s (need %I64d bytes)\r\n",
                              pFileName,
                              dwlSetSize );
                }
            else {
                ErrorExit( dwErrorCode, "Unable to grow file %s to %I64d bytes\r\n",
                                        pFileName,
                                        dwlSetSize );
                }

            CloseHandle( hFile );
            return NULL;
            }
        }
    else {
        dwlInitialSize = GetFileSize64( hFile );
        }

    pContext                          = AllocateWriterContext();
    pContext->hFile                   = hFile;
    pContext->pFileName               = pFileName;
    pContext->dwSectorSize            = dwSectorSize;
    pContext->dwlValidFileSize        = dwlInitialSize;
    pContext->dwlSectorFileSize       = ROUNDUP2_64( dwlInitialSize, dwSectorSize );
    pContext->nMaxActiveWrites        = nMaxActiveWrites;
    pContext->pQueuedWriteListHead    = (PWRITE_BLOCK) &pContext->FakeEmptyHeadNode;
    pContext->pQueuedWriteListTail    = (PWRITE_BLOCK) &pContext->FakeEmptyHeadNode;
    pContext->hSemQueuedWrites        = MyCreateSemaphore( 0 );
    pContext->hSemActiveWrites        = MyCreateSemaphore( 0 );
    pContext->hSemCanStartNewWrite    = MyCreateSemaphore( nMaxActiveWrites );
    pContext->hEventThreadDone        = AllocateAutoEvent();
    pContext->hEventActiveZoneLeave   = AllocateAutoEvent();
    pContext->dwlActiveZoneLowerLimit = 0xFFFFFFFFFFFFFFFF;
    pContext->dwlActiveZoneUpperLimit = 0;
    pContext->pMaybeWriteListHead     = (PWRITE_BLOCK) &pContext->FakeMaybeHeadNode;
    pContext->pMaybeWriteListTail     = (PWRITE_BLOCK) &pContext->FakeMaybeHeadNode;
    pContext->dwQueuedMaybeBytes      = 0;
    pContext->bAbortWriting           = FALSE;

    InitializeCriticalSection( &pContext->ActiveZoneCritSect );

    StartThread( WriterThread,     pContext, THREAD_PRIORITY_ABOVE_NORMAL );
    StartThread( CompletionThread, pContext, THREAD_PRIORITY_LOWEST );

    return (HANDLE) pContext;
    }


VOID
CloseWriteContext(                  //  Flushes, sets EOF, cleans up
    HANDLE    hContext,             //  (does not close file handle)
    DWORDLONG dwlFileSize           //  Optional, sets file size
    )
    {
    PWRITER_CONTEXT pContext = (PWRITER_CONTEXT) hContext;
    DWORD dwSetSizeLow;
    DWORD dwSetSizeHigh;

    ASSERT(( pContext ) || ( bScanOnly ));

    if ( pContext ) {

        ReleaseSemaphore( pContext->hSemQueuedWrites, 1, NULL );

        WaitForSingleObject( pContext->hEventThreadDone, INFINITE );

        if ( ! pContext->bAbortWriting ) {

            if ( dwlFileSize == 0 ) {
                dwlFileSize = pContext->dwlValidFileSize;
                }

            dwSetSizeLow  = (DWORD)( dwlFileSize );
            dwSetSizeHigh = (DWORD)( dwlFileSize >> 32 );

            if (( SetFilePointer( pContext->hFile, dwSetSizeLow, (LONG*)&dwSetSizeHigh, FILE_BEGIN ) == 0xFFFFFFFF ) ||
                ( ! SetEndOfFile( pContext->hFile ))) {

                ErrorExit( GETLASTERROR, "Unable to set file size on %s to %I64d bytes\r\n",
                                         pContext->pFileName,
                                         dwlFileSize );
                }
            }

        RecycleAutoEvent( pContext->hEventThreadDone );
        RecycleAutoEvent( pContext->hEventActiveZoneLeave );
        CloseHandle( pContext->hSemQueuedWrites );
        CloseHandle( pContext->hSemActiveWrites );
        CloseHandle( pContext->hSemCanStartNewWrite );

        if ( pContext->hFile != NULL ) {
            CloseHandle( pContext->hFile );
            pContext->hFile = NULL;
            }

        DEBUGCODE( printf( "nCountSync=%d, nCountAsync=%d\r\n",
                           pContext->nCountSync,
                           pContext->nCountAsync ));

        RecycleWriterContext( pContext );
        }
    }


VOID
IssueWrite(
    HANDLE           hContext,      //  From CreateWriteContext
    PVOID            pBuffer,       //  From AllocateBuffer, we ReleaseBuffer
    DWORDLONG        dwlOffset,     //  Offset where write is to occur
    DWORD            dwSize,        //  Valid size of data in pBuffer
    WRITE_COMPLETION pCompletion,   //  Optional completion callback function
    PVOID            pUserContext   //  Passed to pCompletion routine
    )
    {
    PWRITER_CONTEXT pContext = (PWRITER_CONTEXT) hContext;
    PWRITE_BLOCK    pBlock;
    DWORD           dwSectorWriteSize;

    ASSERT(( pContext ) || ( bScanOnly ));

    if ( pContext ) {

        //
        //  Assert that write offset is sector-aligned.
        //

        ASSERT( ISALIGN2_64( dwlOffset, pContext->dwSectorSize ));

        //
        //  For unbuffered writes, we must write in sector-size multiple
        //  chunks, so if the valid data size is less than a multiple of
        //  the sector size, fill in the end of the last sector with zeros.
        //

        dwSectorWriteSize = ROUNDUP2( dwSize, pContext->dwSectorSize );

        if ( dwSectorWriteSize > dwSize ) {
            ZeroMemory( (PUCHAR) pBuffer + dwSize, dwSectorWriteSize - dwSize );
            }

        pBlock               = AllocateWriteBlock();
        pBlock->pNextBlock   = NULL;
        pBlock->dwlOffset    = dwlOffset;
        pBlock->dwValidSize  = dwSize;
        pBlock->dwWriteSize  = dwSectorWriteSize;
        pBlock->pBuffer      = pBuffer;
        pBlock->pCompletion  = pCompletion;
        pBlock->pUserContext = pUserContext;
        pBlock->pOverlapped  = NULL;

        if ( pBlock->dwWriteSize > 0 ) {
            pBlock->pOverlapped = AllocateOverlappedWithEvent();
            pBlock->pOverlapped->Offset     = (DWORD)( dwlOffset );
            pBlock->pOverlapped->OffsetHigh = (DWORD)( dwlOffset >> 32 );
            }

        pContext->pQueuedWriteListTail->pNextBlock = pBlock;
        pContext->pQueuedWriteListTail             = pBlock;

        ReleaseSemaphore( pContext->hSemQueuedWrites, 1, NULL );
        }

    else {

        //
        //  Don't actually issue write, but must release the buffer or notify
        //  the user's completion routine.
        //

        if ( pCompletion ) {
            ( *pCompletion )( pBuffer, dwlOffset, dwSize, pUserContext );
            }
        else {
            if ( pBuffer ) {
                ReleaseBuffer( pBuffer );
                }
            }
        }
    }


HANDLE
GetFileHandleFromWriteContext(
    HANDLE hContext
    )
    {
    ASSERT(( hContext ) || ( bScanOnly ));
    return ( hContext ? ((PWRITER_CONTEXT) hContext )->hFile : NULL );
    }


VOID
QueueMaybeWrite(
    HANDLE           hContext,
    PVOID            pBuffer,
    DWORDLONG        dwlOffset,
    DWORD            dwSize,
    WRITE_COMPLETION pCompletion,
    PVOID            pUserContext,
    DWORD            dwMaxQueuedBytes
    )
    {
    PWRITER_CONTEXT pContext = (PWRITER_CONTEXT) hContext;
    PWRITE_BLOCK    pBlock;

    ASSERT(( pContext ) || ( bScanOnly ));

    if ( pContext ) {

        pBlock               = AllocateWriteBlock();
        pBlock->pNextBlock   = NULL;
        pBlock->dwlOffset    = dwlOffset;
        pBlock->dwWriteSize  = dwSize;
        pBlock->dwValidSize  = dwSize;
        pBlock->pBuffer      = pBuffer;
        pBlock->pCompletion  = pCompletion;
        pBlock->pUserContext = pUserContext;
        pBlock->pOverlapped  = NULL;

        pContext->pMaybeWriteListTail->pNextBlock = pBlock;
        pContext->pMaybeWriteListTail             = pBlock;
        pContext->dwQueuedMaybeBytes += ROUNDUP2( dwSize, FX_BLOCKSIZE );

        while ( pContext->dwQueuedMaybeBytes > dwMaxQueuedBytes ) {

            DEBUGCODE( printf( "\r\nQueueMaybeWrite leaking write\r\n" ));

            pBlock = pContext->pMaybeWriteListHead->pNextBlock;

            ASSERT( pBlock != NULL );
            ASSERT( pBlock != pContext->pMaybeWriteListHead );

            IssueWrite( hContext,
                        pBlock->pBuffer,
                        pBlock->dwlOffset,
                        pBlock->dwWriteSize,
                        pBlock->pCompletion,
                        pBlock->pUserContext );

            pContext->pMaybeWriteListHead->pNextBlock = pBlock->pNextBlock;

            if ( pContext->pMaybeWriteListTail == pBlock )
                pContext->pMaybeWriteListTail = pContext->pMaybeWriteListHead;

            pContext->dwQueuedMaybeBytes -= ROUNDUP2( pBlock->dwWriteSize, FX_BLOCKSIZE );

            RecycleWriteBlock( pBlock );
            }
        }

    else {

        //
        //  Don't actually issue write, but must release the buffer or notify
        //  the user's completion routine.
        //

        if ( pCompletion ) {
            ( *pCompletion )( pBuffer, dwlOffset, dwSize, pUserContext );
            }
        else {
            if ( pBuffer ) {
                ReleaseBuffer( pBuffer );
                }
            }
        }
    }



VOID
FlushMaybeQueue(
    HANDLE hContext
    )
    {
    PWRITER_CONTEXT pContext = (PWRITER_CONTEXT) hContext;
    PWRITE_BLOCK    pBlock;
    PWRITE_BLOCK    pNext;

    ASSERT(( pContext ) || ( bScanOnly ));

    if ( pContext ) {

        pBlock = pContext->pMaybeWriteListHead->pNextBlock;

        while ( pBlock != NULL ) {

            IssueWrite( hContext,
                        pBlock->pBuffer,
                        pBlock->dwlOffset,
                        pBlock->dwWriteSize,
                        pBlock->pCompletion,
                        pBlock->pUserContext );


            pNext = pBlock->pNextBlock;

            pContext->pMaybeWriteListHead->pNextBlock = pNext;

            RecycleWriteBlock( pBlock );

            pBlock = pNext;

            }

        pContext->pMaybeWriteListTail = pContext->pMaybeWriteListHead;
        pContext->dwQueuedMaybeBytes  = 0;
        }
    }


VOID
DiscardMaybeQueue(
    HANDLE hContext
    )
    {
    PWRITER_CONTEXT pContext = (PWRITER_CONTEXT) hContext;
    PWRITE_BLOCK    pBlock;
    PWRITE_BLOCK    pNext;

    ASSERT(( pContext ) || ( bScanOnly ));

    if ( pContext ) {

        pBlock = pContext->pMaybeWriteListHead->pNextBlock;

        while ( pBlock != NULL ) {

            if ( pBlock->pCompletion ) {

                //
                //  Call user-specified callback completion routine.
                //  NOTE:  If the user specified a callback completion
                //  routine, then the USER is responsible for releasing
                //  the buffer.
                //

                ( *pBlock->pCompletion )( pBlock->pBuffer,
                                          pBlock->dwlOffset,
                                          pBlock->dwWriteSize,
                                          pBlock->pUserContext );
                }
            else {
                if ( pBlock->pBuffer ) {
                    ReleaseBuffer( pBlock->pBuffer );
                    }
                }

            pNext = pBlock->pNextBlock;

            pContext->pMaybeWriteListHead->pNextBlock = pNext;

            RecycleWriteBlock( pBlock );

            pBlock = pNext;

            }

        pContext->pMaybeWriteListTail = pContext->pMaybeWriteListHead;
        pContext->dwQueuedMaybeBytes  = 0;
        }
    }



VOID
FlushCompletion(
    PVOID     pBuffer,
    DWORDLONG dwlOffset,
    DWORD     dwSize,
    PVOID     pUserContext
    )
    {
    SetEvent( (HANDLE) pUserContext );

    UNREFERENCED( pBuffer );
    UNREFERENCED( dwlOffset );
    UNREFERENCED( dwSize );
    }


VOID
FlushWriter(
    HANDLE hContext
    )
    {
    HANDLE hEvent;

    ASSERT(( hContext ) || ( bScanOnly ));

    if ( hContext ) {

        hEvent = AllocateAutoEvent();

        IssueWrite( hContext,
                    NULL,
                    0,
                    0,
                    FlushCompletion,
                    hEvent );

        WaitForSingleObject( hEvent, INFINITE );

        RecycleAutoEvent( hEvent );

        }
    }


VOID
AbortWriter(
    HANDLE hContext
    )
    {
    PWRITER_CONTEXT pContext = (PWRITER_CONTEXT) hContext;

    ASSERT(( pContext ) || ( bScanOnly ));

    if ( pContext ) {

        pContext->bAbortWriting = TRUE;

        FlushWriter( hContext );

        SetFilePointer( pContext->hFile, 0, NULL, FILE_BEGIN );
        SetEndOfFile( pContext->hFile );

//      MySetFileSize( pContext->hFile, 0 );
//      MyDeleteFileOnClose( pContext->hFile, TRUE );

        CloseHandle( pContext->hFile );
        pContext->hFile = NULL;

        DeleteFile( pContext->pFileName );
        }
    }


LPSTR
GetRootPath(
    LPCSTR pszRelativeFileName,
    LPSTR  pszRootPathBuffer
    )
    {
    CHAR szFullPath[ MAX_PATH_LENGTH ];
    LPSTR pReturn;

    try {

        *pszRootPathBuffer = 0;

        GetFullPathName( pszRelativeFileName,
                         sizeof( szFullPath ),
                         szFullPath,
                         &pReturn );

        strcat( szFullPath, "\\" );

        if ( *(UNALIGNED WORD *) szFullPath == '\\\\' ) {

            //
            //  Format is either \\server\share\..., or \\.\device\...,
            //  either of which need to find second backslash after the
            //  initial two and insert terminator after it.
            //

            *( strchr( strchr( szFullPath + 2, '\\' ) + 1, '\\' ) + 1 ) = 0;
            }

        else {

            //
            //  Only remaining format is d:\... so find first backslash
            //  and insert terminator after it.
            //

            *( strchr( szFullPath, '\\' ) + 1 ) = 0;
            }

        pReturn = strcpy( pszRootPathBuffer, szFullPath );
        }

    except( EXCEPTION_EXECUTE_HANDLER ) {
        pReturn = NULL;
        }

    return pReturn;
    }


DWORD
GetSectorSizeByName(
    IN LPCSTR pszAnyPathName
    )
    {
    CHAR  szRootPath[ MAX_PATH_LENGTH ];
    DWORD SectorsPerCluster;
    DWORD BytesPerSector;
    DWORD FreeClusters;
    DWORD TotalClusters;

    if ( GetRootPath( pszAnyPathName, szRootPath ) != NULL ) {

        if ( GetDiskFreeSpace( szRootPath,
                               &SectorsPerCluster,
                               &BytesPerSector,
                               &FreeClusters,
                               &TotalClusters )) {

            return BytesPerSector;
            }
        }

    return 0;
    }


LPWSTR
GetRootPathW(
    LPCWSTR pszRelativeFileName,
    LPWSTR  pszRootPathBuffer
    )
    {
    WCHAR    szFullPath[ MAX_PATH_LENGTH ];
    LPWSTR   pReturn;

    try {

        *pszRootPathBuffer = 0;

        GetFullPathNameW( pszRelativeFileName,
                          MAX_PATH_LENGTH - 1,
                          szFullPath,
                          &pReturn );

        //                                                                  //
        //  Form of szFullPath is now one of following, and each form       //
        //  needs to be mapped as shown:                                    //
        //                                                                  //
        //    d:\dir\file.ext                    -> d:\                     //
        //    \\server\share\dir\file.ext        -> \\server\share\         //
        //    \\.\d:\dir\file.ext                -> \\.\d:\                 //
        //    \\?\d:\dir\file.ext                -> \\?\d:\                 //
        //    \\?\UNC\server\share\dir\file.ext  -> \\?\UNC\server\share\   //
        //                                                                  //

        lstrcatW( szFullPath, (LPCWSTR) L"\\" );

        if ( *(UNALIGNED DWORD *) szFullPath == *(UNALIGNED DWORD *) L"\\\\" ) {

            //
            //  For cases...
            //
            //    \\server\share\dir\file.ext
            //    \\.\d:\dir\file.ext
            //    \\?\d:\dir\file.ext
            //
            //  ...we want to terminate the string after the second backslash
            //  after the initial "\\" (length 2).
            //
            //  For the case...
            //
            //    \\?\UNC\server\share\dir\file.ext
            //
            //  ...we want to terminate the string after the second backslash
            //  after the initial "\\?\UNC\" (length 8).
            //

            unsigned Skip = wcsnicmp( szFullPath, (LPCWSTR) L"\\\\?\\UNC\\", 8 ) ? 2 : 8;

            *( wcschr( wcschr( szFullPath + Skip, L'\\' ) + 1, L'\\' ) + 1 ) = 0;
            }

        else {

            //
            //  Only remaining format is d:\... so find first backslash
            //  and insert terminator after it.
            //

            *( wcschr( szFullPath, L'\\' ) + 1 ) = 0;
            }

        pReturn = lstrcpyW( pszRootPathBuffer, szFullPath );
        }

    except( EXCEPTION_EXECUTE_HANDLER ) {
        pReturn = NULL;
        }

    return pReturn;
    }


DWORD
GetSectorSizeByNameW(
    LPCWSTR pszAnyPathName
    )
    {
    WCHAR szRootPath[ MAX_PATH_LENGTH ];
    DWORD SectorsPerCluster;
    DWORD BytesPerSector;
    DWORD FreeClusters;
    DWORD TotalClusters;

    if ( GetRootPathW( pszAnyPathName, szRootPath ) != NULL ) {

        if ( GetDiskFreeSpaceW( szRootPath,
                                &SectorsPerCluster,
                                &BytesPerSector,
                                &FreeClusters,
                                &TotalClusters )) {

            return BytesPerSector;
            }
        }

    return 0;
    }


DWORD
GetSectorSize(
    IN HANDLE  hFile           OPTIONAL,
    IN LPCWSTR pszAnyPathNameW OPTIONAL,
    IN LPCSTR  pszAnyPathName  OPTIONAL
    )
    {
    DWORD SectorSize = 0;
    DWORD ClusterSize;

    if ( hFile != NULL )
        MyGetFileSectorInfo( hFile, &SectorSize, &ClusterSize );

    if (( SectorSize == 0 ) && ( pszAnyPathNameW != NULL )) {
        SectorSize = GetSectorSizeByNameW( pszAnyPathNameW );
        }

    if (( SectorSize == 0 ) && ( pszAnyPathName != NULL )) {
        SectorSize = GetSectorSizeByName( pszAnyPathName );
        }

    return SectorSize;
    }



