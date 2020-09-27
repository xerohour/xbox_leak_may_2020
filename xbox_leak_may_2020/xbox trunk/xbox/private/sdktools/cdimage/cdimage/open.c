
#include "precomp.h"
#pragma hdrstop

typedef struct _OPEN_BLOCK
    OPEN_BLOCK,
    *POPEN_BLOCK;

typedef struct _OPENER_CONTEXT
    OPENER_CONTEXT,
    *POPENER_CONTEXT;

struct _OPEN_BLOCK {
    POPEN_BLOCK     pNextBlock;             // must be first member
    LPCWSTR         pszUnicodeName;
    LPCSTR          pszFileName;            // used if pszUnicodeName is NULL
    DWORD           dwBufferSize;
    DWORD           dwReadAhead;
    PVOID           pUserContext;
    HANDLE          hReadContext;
    DWORDLONG       dwlFileSize;
    };

struct _OPENER_CONTEXT {
    PVOID          FakeEmptyHeadNode;       // assumes pNext is first member
    POPEN_BLOCK    pOpenListHead;
    POPEN_BLOCK    pOpenListTail;
    HANDLE         hSemCanStartNewOpen;
    HANDLE         hSemQueuedOpens;
    HANDLE         hEventAllReadsIssued;
    HANDLE         hEventQueueEmpty;
    };


RECYCLE_LIST_HEAD RecycledOpenerContextList;

POPENER_CONTEXT
AllocateOpenerContext(
    VOID
    )
    {
    POPENER_CONTEXT pContext;

    pContext = GetObjectFromRecycleList( &RecycledOpenerContextList );

    if ( pContext == NULL ) {
         pContext = MyAllocNeverFreeLocked( sizeof( OPENER_CONTEXT ));
         }
    else {
         ZeroMemory( pContext, sizeof( OPENER_CONTEXT ));
         }

    return pContext;
    }

VOID
INLINE
RecycleOpenerContext(
    POPENER_CONTEXT pContext
    )
    {
    RecycleObjectToList( &RecycledOpenerContextList, pContext );
    }


RECYCLE_LIST_HEAD RecycledOpenBlockList;

POPEN_BLOCK
AllocateOpenBlock(
    VOID
    )
    {
    POPEN_BLOCK Block;

    Block = GetObjectFromRecycleList( &RecycledOpenBlockList );

    if ( Block == NULL ) {
         Block = MyAllocNeverFreeLocked( sizeof( OPEN_BLOCK ));
         }
    else {
         ZeroMemory( Block, sizeof( OPEN_BLOCK ));
         }

    return Block;
    }

VOID
INLINE
RecycleOpenBlock(
    POPEN_BLOCK pBlock
    )
    {
    RecycleObjectToList( &RecycledOpenBlockList, pBlock );
    }



HANDLE
CreateOpenContext(
    UINT nMaxOpenAhead
    )
    {
    POPENER_CONTEXT pContext;

    pContext                       = AllocateOpenerContext();
    pContext->pOpenListHead        = (POPEN_BLOCK) &pContext->FakeEmptyHeadNode;
    pContext->pOpenListTail        = (POPEN_BLOCK) &pContext->FakeEmptyHeadNode;
    pContext->hSemCanStartNewOpen  = MyCreateSemaphore( nMaxOpenAhead );
    pContext->hSemQueuedOpens      = MyCreateSemaphore( 0 );
    pContext->hEventQueueEmpty     = AllocateAutoEvent();
    pContext->hEventAllReadsIssued = AllocateAutoEvent();

    SetEvent( pContext->hEventAllReadsIssued );

    return (HANDLE) pContext;
    }


BOOL
QueueOpenFile(
    HANDLE    hOpenContext,
    LPCWSTR   pszUnicodeName,
    LPCSTR    pszFileName,
    DWORD     dwSectorSize,
    DWORD     dwBufferSize,
    DWORDLONG dwlFileSize,
    UINT      nMaxReadAhead,
    PVOID     pUserContext
    )
    {
    POPENER_CONTEXT pContext = (POPENER_CONTEXT) hOpenContext;
    POPEN_BLOCK     pBlock;

    pBlock                 = AllocateOpenBlock();
    pBlock->pNextBlock     = NULL;
    pBlock->pszUnicodeName = pszUnicodeName;
    pBlock->pszFileName    = pszFileName;
    pBlock->dwBufferSize   = dwBufferSize;
    pBlock->dwlFileSize    = dwlFileSize;
    pBlock->dwReadAhead    = nMaxReadAhead;
    pBlock->pUserContext   = pUserContext;

    WaitForSingleObject( pContext->hSemCanStartNewOpen,  INFINITE );
    WaitForSingleObject( pContext->hEventAllReadsIssued, INFINITE );

    if ( bGlobalErrorExitInProgress )   // don't issue new open
        return FALSE;

    pBlock->hReadContext = CreateReadContext( pszUnicodeName,
                                              pszFileName,
                                              NULL,
                                              dwSectorSize,
                                              dwBufferSize,
                                              nMaxReadAhead,
                                              pContext->hEventAllReadsIssued,
                                              0,
                                              dwlFileSize );

    if ( pBlock->hReadContext == NULL ) {

        //
        //  Failed to open file, so clean up to allow continue.
        //

        SetEvent( pContext->hEventAllReadsIssued );
        ReleaseSemaphore( pContext->hSemCanStartNewOpen, 1, NULL );
        RecycleOpenBlock( pBlock );
        return FALSE;

        }

    pContext->pOpenListTail->pNextBlock = pBlock;
    pContext->pOpenListTail             = pBlock;

    ReleaseSemaphore( pContext->hSemQueuedOpens, 1, NULL );
    return TRUE;
    }


HANDLE
WaitForOpen(
    HANDLE  hOpenContext,
    LPCWSTR *pszUnicodeName,
    LPCSTR  *pszFileName,
    PVOID   *ppUserContext
    )
    {
    POPENER_CONTEXT pContext = (POPENER_CONTEXT) hOpenContext;
    POPEN_BLOCK pBlock;

    WaitForSingleObject( pContext->hSemQueuedOpens, INFINITE );

    pBlock = pContext->pOpenListHead->pNextBlock;

    if ( pContext->pOpenListHead != (POPEN_BLOCK)&pContext->FakeEmptyHeadNode )
        RecycleOpenBlock( pContext->pOpenListHead );

    if ( pBlock == NULL ) {
        SetEvent( pContext->hEventQueueEmpty );
        return NULL;
        }

    pContext->pOpenListHead = pBlock;

    ReleaseSemaphore( pContext->hSemCanStartNewOpen, 1, NULL );

    if ( pszUnicodeName )
        *pszUnicodeName = pBlock->pszUnicodeName;

    if ( pszFileName )
        *pszFileName = pBlock->pszFileName;

    if ( ppUserContext )
        *ppUserContext = pBlock->pUserContext;

    return pBlock->hReadContext;

    }


VOID
CloseOpenContext(
    HANDLE hOpenContext
    )
    {
    POPENER_CONTEXT pContext = (POPENER_CONTEXT) hOpenContext;

    ReleaseSemaphore( pContext->hSemQueuedOpens, 1, NULL );
    WaitForSingleObject( pContext->hEventQueueEmpty, INFINITE );

    CloseHandle( pContext->hSemCanStartNewOpen );
    CloseHandle( pContext->hSemQueuedOpens );
    RecycleAutoEvent( pContext->hEventQueueEmpty );
    ResetEvent( pContext->hEventAllReadsIssued );
    RecycleAutoEvent( pContext->hEventAllReadsIssued );

    RecycleOpenerContext( pContext );
    }
