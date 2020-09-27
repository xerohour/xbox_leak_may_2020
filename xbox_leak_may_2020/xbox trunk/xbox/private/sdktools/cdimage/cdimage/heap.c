
#include "precomp.h"
#pragma hdrstop


HANDLE hGlobalHeap;

void MyHeapInit( void ) {
    hGlobalHeap = GetProcessHeap();
    ASSERT( hGlobalHeap != NULL );
    }


PVOID MyHeapAlloc( DWORD dwSize ) {

    PVOID pMem;

    ASSERT( hGlobalHeap );

    pMem = HeapAlloc( hGlobalHeap, HEAP_ZERO_MEMORY, dwSize );

    if ( pMem == NULL ) {
        ErrorExit( 0, "Out of memory\r\n" );
        }

    return pMem;
    }


PVOID MyHeapReAlloc( PVOID pMem, DWORD dwSize ) {

    PVOID pNewMem;

    ASSERT( hGlobalHeap );

    pNewMem = HeapReAlloc( hGlobalHeap, HEAP_ZERO_MEMORY, pMem, dwSize );
    if ( pNewMem == NULL ) {
        ErrorExit( 0, "Out of memory\r\n" );
        }

    return pNewMem;
    }


PVOID MyVirtualAlloc( DWORD dwSize ) {

    PVOID pMem;

    pMem = VirtualAlloc( NULL, dwSize, MEM_COMMIT, PAGE_READWRITE );
    if ( pMem == NULL ) {
        ErrorExit( 0, "Out of memory\r\n" );
        }

    return pMem;
    }


VOID MyVirtualFree( PVOID pMem ) {

    if ( ! VirtualFree( pMem, 0, MEM_RELEASE )) {
        ErrorExit( GETLASTERROR, "Failed to free virtual memory\r\n" );
        }
    }


BOOL MyVirtualLock( PVOID pMem, DWORD dwSize ) {

    HANDLE CurrentProcess;
    DWORD  MinWorkingSet;
    DWORD  MaxWorkingSet;
    BOOL   Success;

    Success = VirtualLock( pMem, dwSize );

    if ( ! Success ) {

        //
        //  Attempt to grow working set and retry.
        //

        CurrentProcess = GetCurrentProcess();

        Success = GetProcessWorkingSetSize(
                      CurrentProcess,
                      &MinWorkingSet,
                      &MaxWorkingSet
                      );

        ASSERT( Success == TRUE );

        if ( Success ) {

            MinWorkingSet += dwSize;

            if ( MaxWorkingSet < MinWorkingSet )
                 MaxWorkingSet = MinWorkingSet;

            Success = SetProcessWorkingSetSize(
                          CurrentProcess,
                          MinWorkingSet,
                          MaxWorkingSet
                          );

            if ( Success ) {

                Success = VirtualLock( pMem, dwSize );

                }
            }
        }

    if ( ! Success ) {
        DEBUGCODE( printf( "\rFailed to lock virtual memory (GLE=%d)\r\n", GetLastError() ));
        }

    return Success;
    }



#define GRANULARITY 8
#define GROW_SIZE   0x10000


PVOID MyAllocNeverFree( UINT nSize ) {

    static PUCHAR pVirtualRemaining = NULL;             // remembered
    static UINT   nVirtualRemaining = 0;                // remembered
    static CRITICAL_SECTION MyAllocNeverFreeCritSect;   // remembered

    PUCHAR pMem;
    UINT   nGrow;

    if ( pVirtualRemaining == NULL ) {
        InitializeCriticalSection( &MyAllocNeverFreeCritSect );
        }

    EnterCriticalSection( &MyAllocNeverFreeCritSect );

    nSize = ROUNDUP2( nSize, GRANULARITY );

    if ( nSize > nVirtualRemaining ) {

        nGrow = ROUNDUP2( nSize, GROW_SIZE );

        pMem = MyVirtualAlloc( nGrow );

        if ( pMem == ( pVirtualRemaining + nVirtualRemaining )) {

            //
            //  New VM is contiguous with remaining VM.
            //

            nVirtualRemaining += nGrow;
            }
        else {

            //
            //  Wasting nVirtualRemaining bytes at end of old section.
            //

            pVirtualRemaining = pMem;
            nVirtualRemaining = nGrow;
            }
        }

    pMem = pVirtualRemaining;

    pVirtualRemaining += nSize;
    nVirtualRemaining -= nSize;

    LeaveCriticalSection( &MyAllocNeverFreeCritSect );

    return pMem;

    }


PVOID MyAllocNeverFreeLocked( UINT nSize ) {

    static PUCHAR pVirtualRemainingLocked = NULL;               // remembered
    static UINT   nVirtualRemainingLocked = 0;                  // remembered
    static CRITICAL_SECTION MyAllocNeverFreeLockedCritSect;     // remembered

    PUCHAR pMem;
    UINT   nGrow;

    if ( pVirtualRemainingLocked == NULL ) {
        InitializeCriticalSection( &MyAllocNeverFreeLockedCritSect );
        }

    EnterCriticalSection( &MyAllocNeverFreeLockedCritSect );

    nSize = ROUNDUP2( nSize, GRANULARITY );

    if ( nSize > nVirtualRemainingLocked ) {

        nGrow = ROUNDUP2( nSize, GROW_SIZE );

        pMem = MyVirtualAlloc( nGrow );

        MyVirtualLock( pMem, nGrow );

        if ( pMem == ( pVirtualRemainingLocked + nVirtualRemainingLocked )) {

            //
            //  New VM is contiguous with remaining VM.
            //

            nVirtualRemainingLocked += nGrow;
            }
        else {

            //
            //  Wasting nVirtualRemainingLocked bytes at end of old section.
            //

            pVirtualRemainingLocked = pMem;
            nVirtualRemainingLocked = nGrow;
            }
        }

    pMem = pVirtualRemainingLocked;

    pVirtualRemainingLocked += nSize;
    nVirtualRemainingLocked -= nSize;

    LeaveCriticalSection( &MyAllocNeverFreeLockedCritSect );

    return pMem;

    }





PBNODE
INLINE
NewNode(
    PHASH_AND_LENGTH pHashAndLength,
    PVOID            pContext
    )
    {
    PBNODE pNode = MyAllocNeverFreeLocked( sizeof( BNODE ));

    pNode->HashAndLength = *pHashAndLength;
    pNode->pContext      = pContext;

    return pNode;
    }


#ifdef DEBUG
    UINT nMaxBtreeDepth;
#endif


BOOL
LookupOrAddBnode(
    PBNODE           *ppTree,
    PHASH_AND_LENGTH  pHashAndLength,
    PVOID             pContext,
    PBNODE           *ppBnode
    )
    {
    PBNODE pNode = *ppTree;
    int    i;

#ifdef DEBUG
    UINT nDepth = 0;
#endif

    //
    //  Find Hash in Btree.
    //
    //  If found, set *ppBnode = the node and return TRUE.
    //
    //  If not found, add the node, set *ppBnode to the new node, return FALSE
    //

    if ( pNode == NULL ) {
        *ppTree = *ppBnode = NewNode( pHashAndLength, pContext );
        return FALSE;
        }

    for (;;) {

        DEBUGCODE( ++nDepth );

        i = memcmp( &pNode->HashAndLength, pHashAndLength, sizeof( HASH_AND_LENGTH ));

        if ( i == 0 ) {                     // found it
            *ppBnode = pNode;
            return TRUE;                    // found, not added
            }
        else if ( i > 0 ) {                 // go left
            if ( pNode->pLeft ) {
                pNode = pNode->pLeft;
                }
            else {
                *ppBnode = pNode->pLeft = NewNode( pHashAndLength, pContext );
                break;
                }
            }
        else /* ( i < 0 ) */ {              // go right
            if ( pNode->pRight ) {
                pNode = pNode->pRight;
                }
            else {
                *ppBnode = pNode->pRight = NewNode( pHashAndLength, pContext );
                break;
                }
            }
        }

    DEBUGCODE( nMaxBtreeDepth = UMAX( nMaxBtreeDepth, nDepth ));

    return FALSE;                           // did not find, added
    }



RECYCLE_LIST_HEAD EmptyNodeList;
CRITICAL_SECTION  RecycleCritSect;

#ifdef DEBUG
    BOOL RecyclePackageInitialized;
#endif


VOID
InitializeRecyclePackage(
    VOID
    )
    {
    InitializeCriticalSection( &RecycleCritSect );
    DEBUGCODE( RecyclePackageInitialized = TRUE );
    }


PVOID
GetObjectFromRecycleList(
    PRECYCLE_LIST_HEAD ListHead
    )
    {
    PRECYCLE_LIST_NODE Node;
    PVOID Object = NULL;

    ASSERT( RecyclePackageInitialized );

    EnterCriticalSection( &RecycleCritSect );

    Node = ListHead->FirstNode;

    if ( Node != NULL ) {

        ListHead->FirstNode = Node->NextNode;

        Object = Node->Object;

        DEBUGCODE( Node->Object = NULL );

        Node->NextNode = EmptyNodeList.FirstNode;
        EmptyNodeList.FirstNode = Node;

        }

    LeaveCriticalSection( &RecycleCritSect );

    return Object;
    }


VOID
RecycleObjectToList(
    PRECYCLE_LIST_HEAD ListHead,
    PVOID              Object
    )
    {
    PRECYCLE_LIST_NODE Node;

    ASSERT( RecyclePackageInitialized );

    EnterCriticalSection( &RecycleCritSect );

    Node = EmptyNodeList.FirstNode;

    if ( Node != NULL ) {
        EmptyNodeList.FirstNode = Node->NextNode;
        }
    else {
        Node = MyAllocNeverFreeLocked( sizeof( RECYCLE_LIST_NODE ));
        }

    Node->Object        = Object;
    Node->NextNode      = ListHead->FirstNode;
    ListHead->FirstNode = Node;

    LeaveCriticalSection( &RecycleCritSect );
    }


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  AutoEventAllocator                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    LONG nAutoEventsCreated;
    LONG nAutoEventsAvailable;
#endif // DEBUG

RECYCLE_LIST_HEAD RecycledAutoEventList;

HANDLE
AllocateAutoEvent(
    VOID
    )
    {
    HANDLE hEvent;

    hEvent = (HANDLE) GetObjectFromRecycleList( &RecycledAutoEventList );

    if ( hEvent == NULL ) {
         hEvent = MyCreateEvent( FALSE, FALSE );
         DEBUGCODE( InterlockedIncrement( &nAutoEventsCreated ));
         }
    else {
         ASSERT( EventStateUnsignaled( hEvent ));
         DEBUGCODE( InterlockedDecrement( &nAutoEventsAvailable ));
         }

    return hEvent;
    }

VOID
RecycleAutoEvent(
    HANDLE hEvent
    )
    {
    ASSERT( EventStateUnsignaled( hEvent ));

    RecycleObjectToList( &RecycledAutoEventList, (PVOID) hEvent );

    DEBUGCODE( InterlockedIncrement( &nAutoEventsAvailable ));
    }


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  ManualEventAllocator                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    LONG nManualEventsCreated;
    LONG nManualEventsAvailable;
#endif

RECYCLE_LIST_HEAD RecycledManualEventList;

HANDLE
AllocateManualEvent(
    VOID
    )
    {
    HANDLE hEvent;

    hEvent = (HANDLE) GetObjectFromRecycleList( &RecycledManualEventList );

    if ( hEvent == NULL ) {
         hEvent = MyCreateEvent( TRUE, FALSE );
         DEBUGCODE( InterlockedIncrement( &nManualEventsCreated ));
         }
    else {
         ResetEvent( hEvent );
         DEBUGCODE( InterlockedDecrement( &nManualEventsAvailable ));
         }

    return hEvent;
    }

VOID
RecycleManualEvent(
    HANDLE hEvent
    )
    {
    RecycleObjectToList( &RecycledManualEventList, (PVOID) hEvent );

    DEBUGCODE( InterlockedIncrement( &nManualEventsAvailable ));
    }




