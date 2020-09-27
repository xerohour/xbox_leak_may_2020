
#include "precomp.h"
#pragma hdrstop

CRITICAL_SECTION GlobalBufferCritSect;

UINT nGlobalBufferTotalMemory;
UINT nGlobalBufferGranularity;



typedef struct _NODE NODE, *PNODE;

struct _NODE {
    PNODE  pNextNode;
    PNODE  pPrevNode;
    PUCHAR pMem;
    UINT   nSize;
    };

#ifdef DEBUG
    BOOL bGlobalBufferAllocatorHasBeenInitialized;
#endif

NODE BusyList = { &BusyList, &BusyList, NULL, 0 };      // empty head node
NODE FreeList = { &FreeList, &FreeList, NULL, 0 };      // empty head node
NODE NodeList = { &NodeList, &NodeList, NULL, 0 };      // empty head node
NODE VirtList = { &VirtList, &VirtList, NULL, 0 };      // empty head node

typedef struct _WAITER WAITER, *PWAITER;

struct _WAITER {
    PWAITER pNextWaiter;
    PWAITER pPrevWaiter;
    HANDLE  hEvent;
    UINT    nRequestSize;
    PNODE   *ppNode;
    };


WAITER WaiterList = { &WaiterList, &WaiterList, NULL, 0, NULL };
WAITER WaiterPool = { &WaiterPool, &WaiterPool, NULL, 0, NULL };

VOID
LinkBefore(
    PNODE pListMember,
    PNODE pNode
    )
    {
    pNode->pNextNode            = pListMember;
    pNode->pPrevNode            = pListMember->pPrevNode;
    pNode->pNextNode->pPrevNode = pNode;
    pNode->pPrevNode->pNextNode = pNode;
    }


VOID
Unlink(
    PNODE pNode
    )
    {

    ASSERT( pNode != &BusyList );
    ASSERT( pNode != &FreeList );
    ASSERT( pNode != &NodeList );
    ASSERT( pNode != &VirtList );
    ASSERT( pNode != (PNODE) &WaiterList );
    ASSERT( pNode != (PNODE) &WaiterPool );

    pNode->pPrevNode->pNextNode = pNode->pNextNode;
    pNode->pNextNode->pPrevNode = pNode->pPrevNode;
    }


PNODE
AllocateNode(
    VOID
    )
    {
    PNODE pNode;

    pNode = NodeList.pNextNode;

    if ( pNode == &NodeList ) {
        pNode = MyAllocNeverFreeLocked( sizeof( NODE ));
        }
    else {
        Unlink( pNode );
        }

    return pNode;
    }


VOID
RecycleNode(
    PNODE pNode
    )
    {
    LinkBefore( NodeList.pNextNode, pNode );    // push at head of node list
    }


PNODE
FreeNodeToBusyNode(
    PNODE pFreeNode,
    UINT  nBusySize
    )
    {
    PNODE pBusyNode;

    ASSERT( pFreeNode->nSize >= nBusySize );

    if ( pFreeNode->nSize > nBusySize ) {

        //
        //  Split into two blocks
        //

        pBusyNode         = AllocateNode();
        pBusyNode->pMem   = pFreeNode->pMem;
        pBusyNode->nSize  = nBusySize;

        pFreeNode->pMem  += nBusySize;
        pFreeNode->nSize -= nBusySize;

        }
    else {

        //
        //  Using whole block
        //

        pBusyNode = pFreeNode;
        Unlink( pFreeNode );                // remove from free list

        }

    LinkBefore( &BusyList, pBusyNode );     // enqueue at tail of busy list

    return pBusyNode;
    }


BOOL
Adjacent(
    PNODE pLowerNode,
    PNODE pUpperNode
    )
    {
    return (( pLowerNode->pMem + pLowerNode->nSize ) == pUpperNode->pMem );
    }


VOID
BusyNodeToFreeNode(
    PNODE pBusyNode
    )
    {
    PNODE pNodeAfter;
    PNODE pNodeBefore;
    PNODE pFreeNode;

    Unlink( pBusyNode );    // remove from busy list

    pFreeNode = pBusyNode;  // just for namesake

    //
    // coalesce into free list
    //

    for ( pNodeAfter  = FreeList.pNextNode;
          pNodeAfter != &FreeList;
          pNodeAfter  = pNodeAfter->pNextNode ) {

        if ( pNodeAfter->pMem > pFreeNode->pMem ) {
            break;
            }
        }

    LinkBefore( pNodeAfter, pFreeNode );

    if ( Adjacent( pFreeNode, pNodeAfter )) {

        pFreeNode->nSize += pNodeAfter->nSize;
        Unlink( pNodeAfter );
        RecycleNode( pNodeAfter );

        }

    pNodeBefore = pFreeNode->pPrevNode;

    if ( Adjacent( pNodeBefore, pFreeNode )) {

        pNodeBefore->nSize += pFreeNode->nSize;
        Unlink( pFreeNode );
        RecycleNode( pFreeNode );

        }

    }


PNODE
FindFreeMem(
    UINT nRequestSize
    )
    {
    PNODE pProbeNode = FreeList.pNextNode;
    PNODE pFoundNode = NULL;
    UINT  nFoundSize = 0xFFFFFFFF;
    UINT  nProbeSize;

    while (( pProbeNode != &FreeList ) && ( nFoundSize > nRequestSize )) {

        nProbeSize = pProbeNode->nSize;

        if (( nProbeSize >= nRequestSize ) && ( nProbeSize < nFoundSize )) {
            pFoundNode = pProbeNode;
            nFoundSize = nProbeSize;
            }

        pProbeNode = pProbeNode->pNextNode;
        }

    if ( pFoundNode == NULL )
        return NULL;

    return FreeNodeToBusyNode( pFoundNode, nRequestSize );
    }


PNODE
FindBusyMem(
    PVOID pMem
    )
    {
    PNODE pNode = BusyList.pNextNode;

    while ( pNode != &BusyList ) {

        if ( pNode->pMem == pMem ) {
            return pNode;
            }

        pNode = pNode->pNextNode;
        }

    return NULL;
    }


PNODE
FindVirtMem(
    PVOID pMem
    )
    {
    PNODE pNode = VirtList.pNextNode;

    while ( pNode != &VirtList ) {

        if ( pNode->pMem == pMem ) {
            return pNode;
            }

        pNode = pNode->pNextNode;
        }

    return NULL;
    }


PWAITER
AllocateWaiter(
    VOID
    )
    {
    PWAITER pWaiter;

    pWaiter = WaiterPool.pNextWaiter;

    if ( pWaiter == &WaiterPool ) {
        pWaiter = MyAllocNeverFreeLocked( sizeof( WAITER ));
        }
    else {
        Unlink( (PNODE) pWaiter );
        }

    return pWaiter;
    }


VOID
RecycleWaiter(
    PWAITER pWaiter
    )
    {
    LinkBefore( (PNODE) WaiterPool.pNextWaiter, (PNODE) pWaiter );  // push
    }


VOID
DispatchAndDequeueWaiter(
    PWAITER pWaiter,
    PNODE   pNode
    )
    {
    *( pWaiter->ppNode ) = pNode;
    SetEvent( pWaiter->hEvent );
    Unlink( (PNODE) pWaiter );
    RecycleWaiter( pWaiter );
    }


PVOID
AllocateBuffer(
    UINT nRequestSize,
    BOOL bZeroMemory
    )
    {
    HANDLE  hEvent;
    PNODE   pNode;
    PWAITER pWaiter;
    UINT    nBufferSize;

    ASSERT( bGlobalBufferAllocatorHasBeenInitialized );

    nBufferSize = ROUNDUP2( nRequestSize, nGlobalBufferGranularity );

    if ( nBufferSize > nGlobalBufferTotalMemory ) {

        //
        //  This request cannot be met with buffer pool.
        //  Allocate a new VM for this request.
        //

        DEBUGCODE(
            printf(
                "Buffer request is %d (0x%X) bytes, allocating VM\r\n",
                nBufferSize,
                nBufferSize
                )
            );

        EnterCriticalSection( &GlobalBufferCritSect );

        pNode        = AllocateNode();
        ASSERT(pNode != NULL);
        pNode->pMem  = MyVirtualAlloc( nBufferSize );
        ASSERT(pNode->pMem != NULL);
        pNode->nSize = nBufferSize;

        LinkBefore( &VirtList, pNode );

        LeaveCriticalSection( &GlobalBufferCritSect );

        //
        //  MyVirtualAlloc provides already-zeroed memory.
        //

        }

    else {

        if ( nBufferSize == 0 ) {
            ASSERT( nBufferSize > 0 );
            ErrorExit( 0, "Requested buffer size is zero\r\n" );
            }

        EnterCriticalSection( &GlobalBufferCritSect );

        if ( WaiterList.pNextWaiter != &WaiterList ) {  // another waiter already waiting
            pNode = NULL;
            }
        else {
            pNode = FindFreeMem( nBufferSize );
            }

        if ( pNode == NULL ) {


            // BUGBUG
//            printf("Making a waiter for %u\r\n",GetCurrentThreadId());

            pWaiter               = AllocateWaiter();
            ASSERT(pWaiter != NULL);
            pWaiter->hEvent       = AllocateAutoEvent();
            ASSERT(pWaiter->hEvent != NULL);
            pWaiter->nRequestSize = nBufferSize;
            pWaiter->ppNode       = &pNode;

            LinkBefore( (PNODE) &WaiterList, (PNODE) pWaiter ); // enqueue at tail

            hEvent = pWaiter->hEvent;

            }
        else {
            hEvent = NULL;
            }

        LeaveCriticalSection( &GlobalBufferCritSect );

        if ( hEvent ) {
            WaitForSingleObject( hEvent, INFINITE );

            // BUGBUG
  //          printf("Done waiting %u\r\n",GetCurrentThreadId());

            RecycleAutoEvent( hEvent );
            ASSERT( pNode != NULL );
            }

        if ( bZeroMemory ) {
            ZeroMemory( pNode->pMem, nRequestSize );
            }

        }

    /*
#ifdef DEBUG

    ASSERT( pNode->nSize >= nRequestSize );

    if ( bZeroMemory ) {

        UINT TailBytes = pNode->nSize - nRequestSize;

        if ( TailBytes ) {
            memset( pNode->pMem + nRequestSize, 0xEE, TailBytes );
            }
        }

    else {

        memset( pNode->pMem, 0xEE, pNode->nSize );

        }

#endif // DEBUG
    printf("Allocating buffer at %p - %u\r\n",pNode->pMem,GetCurrentThreadId());
    */

    return pNode->pMem;
    }


VOID
ReleaseBuffer(
    PVOID pBuffer
    )
    {
    PNODE   pNode;
    PWAITER pWaiter;

    ASSERT( bGlobalBufferAllocatorHasBeenInitialized );

    EnterCriticalSection( &GlobalBufferCritSect );

    /*
#ifdef DEBUG
    printf("Releasing memory at %p - %u\r\n",pBuffer,GetCurrentThreadId());
#endif
    */

    pNode = FindVirtMem( pBuffer );

    if ( pNode != NULL ) {

        MyVirtualFree( pNode->pMem );
        Unlink( pNode );
        RecycleNode( pNode );

        }

    else {

        pNode = FindBusyMem( pBuffer );

        ASSERT( pNode != NULL );

        pWaiter = WaiterList.pNextWaiter;

        if (( pWaiter != &WaiterList ) && ( pWaiter->nRequestSize == pNode->nSize )) {
            DispatchAndDequeueWaiter( pWaiter, pNode );
            }
        else {

            BusyNodeToFreeNode( pNode );

            if ( pWaiter != &WaiterList ) {

                pNode = FindFreeMem( pWaiter->nRequestSize );

                if ( pNode != NULL ) {

                    DispatchAndDequeueWaiter( pWaiter, pNode );

                    }
                }
            }
        }

    LeaveCriticalSection( &GlobalBufferCritSect );
    }


DWORD
GetLargestFreeBufferSize(
    VOID
    )
    {
    UINT  nSize = 0;
    PNODE pNode;

    ASSERT( bGlobalBufferAllocatorHasBeenInitialized );

    EnterCriticalSection( &GlobalBufferCritSect );

    if ( WaiterList.pNextWaiter == &WaiterList ) {  // no waiters

        for ( pNode  = FreeList.pNextNode;
              pNode != &FreeList;
              pNode = pNode->pNextNode ) {

            if ( pNode->nSize > nSize )
                nSize = pNode->nSize;

            }
        }

    LeaveCriticalSection( &GlobalBufferCritSect );

    return nSize;
    }


DWORD
AdjustWorkingSetAndLockImage(
    DWORD MinimumRingBufferSize,
    DWORD IndividualBufferSize
    )
    {
    SYSTEM_INFO           SysInfo;
    MEMORYSTATUS          MemStatus      = { sizeof( MEMORYSTATUS ) };
    HANDLE                CurrentProcess = GetCurrentProcess();
    PVOID                 BaseOfImage    = GetModuleHandle( NULL );     // hopefully, base address of mapped cdimage.exe
    PIMAGE_DOS_HEADER     DosHeader;
    PIMAGE_NT_HEADERS     NtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG                 SectionSize;
    ULONG                 SectionCount;
    PVOID                 SectionBase;
    DWORD                 ImageLockSize;
    DWORD                 RingBufferSize;
    DWORD                 MinWorkingSet;
    DWORD                 MaxWorkingSet;
    DWORD                 BaseWorkingSet;
    BOOL                  bSuccess;

    GetSystemInfo( &SysInfo );

    GetProcessWorkingSetSize( CurrentProcess, &BaseWorkingSet, &MaxWorkingSet );

#ifdef DEBUG

    ASSERT( ISPOWER2( IndividualBufferSize ));
    ASSERT( ISALIGN2( IndividualBufferSize,  SysInfo.dwPageSize ));
    ASSERT( ISALIGN2( MinimumRingBufferSize, IndividualBufferSize ));

    printf( "InitialProcessWorkingSet: dwMin=%X, dwMax=%X\r\n", BaseWorkingSet, MaxWorkingSet );

#endif // DEBUG


    //
    //  Walk image section headers and determine size we want to lock down
    //  so we can increase the working set accordingly.
    //

    ImageLockSize = 0;

    try {

        DosHeader = BaseOfImage;

        if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {

            NtHeader = (PVOID)((PCHAR)BaseOfImage + DosHeader->e_lfanew );

            if ( NtHeader->Signature == IMAGE_NT_SIGNATURE ) {

                SectionHeader = (PVOID)((PCHAR)NtHeader +
                                        sizeof(ULONG)   +
                                        sizeof(IMAGE_FILE_HEADER) +
                                        NtHeader->FileHeader.SizeOfOptionalHeader
                                        );

                SectionCount = NtHeader->FileHeader.NumberOfSections;

                while ( SectionCount-- ) {

                    if (( memcmp( SectionHeader->Name, ".text",  6 ) == 0 ) ||
                        ( memcmp( SectionHeader->Name, ".data",  6 ) == 0 ) ||
                        ( memcmp( SectionHeader->Name, ".rdata", 7 ) == 0 ) ||
                        ( memcmp( SectionHeader->Name, ".idata", 7 ) == 0 )) {

                        SectionSize = SectionHeader->Misc.VirtualSize;

                        ImageLockSize += ROUNDUP2( SectionSize, SysInfo.dwPageSize );

                        }

                    SectionHeader++;

                    }
                }
            }
        }

    except( EXCEPTION_EXECUTE_HANDLER ) {

        DEBUGCODE( printf( "Exception walking module info\r\n" ));

        }

    if ( ImageLockSize == 0 )
         ImageLockSize = 0x40000;   // guestimate 256K if fail to walk image


    //
    //  Assume OS needs 8MB of physical memory, take remaining memory and
    //  use 1/4 of it for ring buffer (in case need to run multiple instances
    //  of cdimage).  On a 16MB system this will result in 2MB, 4MB on 24MB,
    //  6MB on 32MB, 8MB on 40MB, 14MB on 64MB, etc.
    //
    //  NOTE: On NT 4.0 Server system, the cache manager is a pig and keeps at
    //  least half of RAM to itself.
    //

    GlobalMemoryStatus( &MemStatus );

    if (( bOptimizeStorage ) && ( MemStatus.dwTotalPhys > 0x800000 )) {

        RingBufferSize = ROUNDUP2(
                             ( MemStatus.dwTotalPhys - 0x800000 ) / 4,
                             IndividualBufferSize
                             );

        if ( RingBufferSize < MinimumRingBufferSize ) {
             RingBufferSize = MinimumRingBufferSize;
             }
        }

    else {

        RingBufferSize = MinimumRingBufferSize;

        }

    MaxWorkingSet = MemStatus.dwTotalPhys / 2;      // don't exceed half of physical RAM

    MinWorkingSet = RingBufferSize + ImageLockSize + 0x10000;

    bSuccess = SetProcessWorkingSetSize(
                   CurrentProcess,
                   MinWorkingSet,
                   MaxWorkingSet
                   );

    if ( ! bSuccess ) {

        if ( GetLastError() == ERROR_PRIVILEGE_NOT_HELD ) {

            DEBUGCODE( printf( "Insufficient privilege to grow working set\r\n" ));

            printf(
                "\r\n"
                "WARNING: Current user does not have privilege to grow the process working set\r\n"
                "         which can increase the performance of creating an image.  To acquire\r\n"
                "         this privilege, the user can be made a member of the Administrators\r\n"
                "         group on the local machine, or the Adminstrator can grant the\r\n"
                "         \"Increase Scheduling Priority\" privilege to \"Everyone\" on the\r\n"
                "         local machine via MUSRMGR.EXE's \"Policy\", \"User Rights\" dialog.\r\n"
                "\r\n"
                );
            fflush( stdout );

            return MinimumRingBufferSize;
            }

        DEBUGCODE( printf( "SetWorkingSet( %X, %X ) failed (GLE=%d), trying incremental\r\n",
                           MinWorkingSet,
                           MaxWorkingSet,
                           GetLastError() ));

        bSuccess = TRUE;

        while (( bSuccess ) && ( BaseWorkingSet < MinWorkingSet )) {

            BaseWorkingSet += 0x10000;

            bSuccess = SetProcessWorkingSetSize(
                           CurrentProcess,
                           BaseWorkingSet,
                           MaxWorkingSet
                           );
            }

        BaseWorkingSet -= 0x10000;

        if ( BaseWorkingSet > MinimumRingBufferSize ) {

            RingBufferSize = BaseWorkingSet - MinimumRingBufferSize;

            }
        else {

            RingBufferSize = MinimumRingBufferSize;

            }
        }

    GetProcessWorkingSetSize( CurrentProcess, &MinWorkingSet, &MaxWorkingSet );

#ifdef DEBUG

    printf( "NegotiatedProcessWorkingSet: dwMin=%X, dwMax=%X\r\n", MinWorkingSet, MaxWorkingSet );
    printf( "BufferAllocation=%X\r\n", RingBufferSize );

#endif // DEBUG

    //
    //  Now try to lock down our code and static image data.
    //

    if ( MinWorkingSet > ( RingBufferSize + ImageLockSize )) {

        try {

            DosHeader = BaseOfImage;

            if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {

                NtHeader = (PVOID)((PCHAR)BaseOfImage + DosHeader->e_lfanew );

                if ( NtHeader->Signature == IMAGE_NT_SIGNATURE ) {

                    SectionHeader = (PVOID)((PCHAR)NtHeader +
                                            sizeof(ULONG)   +
                                            sizeof(IMAGE_FILE_HEADER) +
                                            NtHeader->FileHeader.SizeOfOptionalHeader
                                            );

                    SectionCount  = NtHeader->FileHeader.NumberOfSections;

                    while ( SectionCount-- ) {

                        if (( memcmp( SectionHeader->Name, ".text",  6 ) == 0 ) ||
                            ( memcmp( SectionHeader->Name, ".data",  6 ) == 0 ) ||
                            ( memcmp( SectionHeader->Name, ".rdata", 7 ) == 0 ) ||
                            ( memcmp( SectionHeader->Name, ".idata", 7 ) == 0 )) {

                            SectionBase = (PCHAR)BaseOfImage + SectionHeader->VirtualAddress;
                            SectionSize = SectionHeader->Misc.VirtualSize;

                            MyVirtualLock( SectionBase, SectionSize );

                            }

                        SectionHeader++;

                        }
                    }
                }
            }

        except( EXCEPTION_EXECUTE_HANDLER ) {

            DEBUGCODE( printf( "Exception walking module info\r\n" ));

            }
        }

    return RingBufferSize;
    }

DWORD
InitializeBufferAllocator(
    DWORD RequestedRingBufferSize,
    DWORD MinimumRingBufferSize,
    DWORD IndividualBufferSize
    )
    {
    PUCHAR pMem;
    PUCHAR pEnd;
    PUCHAR pSection;
    ULONG  SectionSize;
    ULONG  TotalLocked;
    ULONG  ActualRingBufferSize;
    BOOL   bSuccess;
    PNODE  pNode;

    InitializeCriticalSection( &GlobalBufferCritSect );

    ActualRingBufferSize = RequestedRingBufferSize;

    pMem = MyVirtualAlloc( RequestedRingBufferSize );

    if ( ! MyVirtualLock( pMem, RequestedRingBufferSize )) {

        DEBUGCODE( printf( "Failed to lock ring buffer all at once, trying incremental\r\n" ));

        TotalLocked = 0;
        pSection    = pMem;
        pEnd        = pMem + RequestedRingBufferSize;
        bSuccess    = TRUE;

        while (( bSuccess ) && ( pSection < pEnd )) {

            SectionSize = IndividualBufferSize;

            if (( pSection + SectionSize ) > pEnd )
                SectionSize = ( pEnd - pSection );

            bSuccess = MyVirtualLock( pSection, SectionSize );

            if ( bSuccess ) {
                TotalLocked += SectionSize;
                pSection    += SectionSize;
                }
            }

        ActualRingBufferSize = TotalLocked;

        if ( ActualRingBufferSize < MinimumRingBufferSize )
             ActualRingBufferSize = MinimumRingBufferSize;

        if ( ActualRingBufferSize < RequestedRingBufferSize )
             VirtualFree( pMem + ActualRingBufferSize, 0, MEM_DECOMMIT );

        DEBUGCODE( printf( "Locked %d bytes of %d byte ring buffer\r\n", TotalLocked, ActualRingBufferSize ));

        }

    pNode        = AllocateNode();
    pNode->pMem  = pMem;
    pNode->nSize = ActualRingBufferSize;

    LinkBefore( &FreeList, pNode );

    nGlobalBufferTotalMemory = ActualRingBufferSize;
    nGlobalBufferGranularity = IndividualBufferSize;

    DEBUGCODE( bGlobalBufferAllocatorHasBeenInitialized = TRUE );

    return ActualRingBufferSize;
    }
