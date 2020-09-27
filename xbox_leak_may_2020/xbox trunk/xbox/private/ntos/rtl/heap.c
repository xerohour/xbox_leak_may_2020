/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    heap.c

Abstract:

    This module implements a heap allocator.

Author:

    Steve Wood (stevewo) 20-Sep-1989 (Adapted from URTL\alloc.c)

Revision History:

--*/

#include "ntrtlp.h"
#include "heap.h"
#include "heappriv.h"

ULONG NtGlobalFlag;

#ifdef NTHEAP_ENABLED
#include "heapp.h"
#endif // NTHEAP_ENABLED

#ifdef NTOS_KERNEL_RUNTIME

SIZE_T MmHeapSegmentReserve = 1024 * 1024;
SIZE_T MmHeapSegmentCommit = PAGE_SIZE * 2;
SIZE_T MmHeapDeCommitTotalFreeThreshold = 64 * 1024;
SIZE_T MmHeapDeCommitFreeBlockThreshold = PAGE_SIZE;

#else  // NTOS_KERNEL_RUNTIME

ULONG RtlpDisableHeapLookaside = 0;

#endif // NTOS_KERNEL_RUNTIME

//
//  If any of these flags are set, the fast allocator punts
//  to the slow do-everything allocator.
//

#define HEAP_SLOW_FLAGS (HEAP_DEBUG_FLAGS           | \
                         HEAP_SETTABLE_USER_FLAGS   | \
                         HEAP_NEED_EXTRA_FLAGS      | \
                         HEAP_CREATE_ALIGN_16       | \
                         HEAP_FREE_CHECKING_ENABLED | \
                         HEAP_TAIL_CHECKING_ENABLED)

#if DBG

UCHAR CheckHeapFillPattern[ CHECK_HEAP_TAIL_SIZE ] = {
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
#if defined(_WIN64) || defined(_XBOX)
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
    CHECK_HEAP_TAIL_FILL,
#endif
    CHECK_HEAP_TAIL_FILL
};

#endif // DBG

//
//  An extra bitmap manipulation routine
//

#if i386

CCHAR
FASTCALL
RtlFindFirstSetRightMember(
    ULONG Set
    );

#else

#define RtlFindFirstSetRightMember(Set)                     \
    (((Set) & 0xFFFF) ?                                     \
        (((Set) & 0xFF) ?                                   \
            RtlpBitsClearLow[(Set) & 0xFF] :                \
            RtlpBitsClearLow[((Set) >> 8) & 0xFF] + 8) :    \
        ((((Set) >> 16) & 0xFF) ?                           \
            RtlpBitsClearLow[ ((Set) >> 16) & 0xFF] + 16 :  \
            RtlpBitsClearLow[ (Set) >> 24] + 24)            \
    )

#endif


//
//  These are procedure prototypes exported by heapdbg.c
//

#ifndef NTOS_KERNEL_RUNTIME

PVOID
RtlDebugCreateHeap (
    IN ULONG Flags,
    IN PVOID HeapBase OPTIONAL,
    IN SIZE_T ReserveSize OPTIONAL,
    IN SIZE_T CommitSize OPTIONAL,
    IN PVOID Lock OPTIONAL,
    IN PRTL_HEAP_PARAMETERS Parameters OPTIONAL
    );

BOOLEAN
RtlDebugDestroyHeap (
    IN PVOID HeapHandle
    );

PVOID
RtlDebugAllocateHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    );

BOOLEAN
RtlDebugFreeHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

ULONG
RtlDebugSizeHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

NTSTATUS
RtlDebugZeroHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags
    );

#endif // NTOS_KERNEL_RUNTIME


//
//  Local procedure prototypes
//

BOOLEAN
RtlpGrowBlockInPlace (
    IN PHEAP Heap,
    IN ULONG Flags,
    IN PHEAP_ENTRY BusyBlock,
    IN SIZE_T Size,
    IN SIZE_T AllocationIndex
    );

PHEAP_UNCOMMMTTED_RANGE
RtlpCreateUnCommittedRange (
    IN PHEAP_SEGMENT Segment
    );

VOID
RtlpDestroyUnCommittedRange (
    IN PHEAP_SEGMENT Segment,
    IN PHEAP_UNCOMMMTTED_RANGE UnCommittedRange
    );

VOID
RtlpInsertUnCommittedPages (
    IN PHEAP_SEGMENT Segment,
    IN ULONG_PTR Address,
    IN SIZE_T Size
    );

NTSTATUS
RtlpDestroyHeapSegment (
    IN PHEAP_SEGMENT Segment
    );

PHEAP_FREE_ENTRY
RtlpExtendHeap (
    IN PHEAP Heap,
    IN SIZE_T AllocationSize
    );


PVOID
RtlCreateHeap (
    IN ULONG Flags,
    IN PVOID HeapBase OPTIONAL,
    IN SIZE_T ReserveSize OPTIONAL,
    IN SIZE_T CommitSize OPTIONAL,
    IN PVOID Lock OPTIONAL,
    IN PRTL_HEAP_PARAMETERS Parameters OPTIONAL
    )

/*++

Routine Description:

    This routine initializes a heap.

Arguments:

    Flags - Specifies optional attributes of the heap.

        Valid Flags Values:

        HEAP_NO_SERIALIZE - if set, then allocations and deallocations on
                         this heap are NOT synchronized by these routines.

        HEAP_GROWABLE - if set, then the heap is a "sparse" heap where
                        memory is committed only as necessary instead of
                        being preallocated.

    HeapBase - if not NULL, this specifies the base address for memory
        to use as the heap.  If NULL, memory is allocated by these routines.

    ReserveSize - if not zero, this specifies the amount of virtual address
        space to reserve for the heap.

    CommitSize - if not zero, this specifies the amount of virtual address
        space to commit for the heap.  Must be less than ReserveSize.  If
        zero, then defaults to one page.

    Lock - if not NULL, this parameter points to the resource lock to
        use.  Only valid if HEAP_NO_SERIALIZE is NOT set.

    Parameters - optional heap parameters.

Return Value:

    PVOID - a pointer to be used in accessing the created heap.

--*/

{
    NTSTATUS Status;
    PHEAP Heap = NULL;
    PHEAP_SEGMENT Segment = NULL;
    PLIST_ENTRY FreeListHead;
    ULONG SizeOfHeapHeader;
    ULONG SegmentFlags;
    PVOID CommittedBase;
    PVOID UnCommittedBase;
    MEMORY_BASIC_INFORMATION MemoryInformation;
    ULONG n;
    ULONG InitialCountOfUnusedUnCommittedRanges;
    SIZE_T MaximumHeapBlockSize;
    PVOID NextHeapHeaderAddress;
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange, *pp;
    RTL_HEAP_PARAMETERS TempParameters;

#ifndef NTOS_KERNEL_RUNTIME

    PPEB Peb;

#endif // NTOS_KERNEL_RUNTIME

    RTL_PAGED_CODE();

#ifndef NTOS_KERNEL_RUNTIME
#ifdef NTHEAP_ENABLED
    {
        if (Flags & NTHEAP_ENABLED_FLAG) {

            Heap = RtlCreateNtHeap( Flags, NULL );

            if (Heap != NULL) {

                return Heap;
            }

            Flags &= ~NTHEAP_ENABLED_FLAG;
        }
    }
#endif // NTHEAP_ENABLED
#endif // NTOS_KERNEL_RUNTIME

    //
    //  Check if we should be using the page heap code.  If not then turn
    //  off any of the page heap flags before going on
    //

#ifdef DEBUG_PAGE_HEAP

    if ( RtlpDebugPageHeap && ( HeapBase == NULL ) && ( Lock == NULL )) {

        PVOID PageHeap;

        PageHeap = RtlpDebugPageHeapCreate(

            Flags,
            HeapBase,
            ReserveSize,
            CommitSize,
            Lock,
            Parameters );

        if (PageHeap != NULL) {
            return PageHeap;
        }

        //
        // A `-1' value signals a recursive call from page heap
        // manager. We set this to null and continue creating
        // a normal heap. This small hack is required so that we
        // minimize the dependencies between the normal and the page
        // heap manager.
        //

        if ((SIZE_T)Parameters == (SIZE_T)-1) {

            Parameters = NULL;
        }
    }

    Flags &= ~( HEAP_PROTECTION_ENABLED |
        HEAP_BREAK_WHEN_OUT_OF_VM |
        HEAP_NO_ALIGNMENT );

#endif // DEBUG_PAGE_HEAP

#if DBG
    //
    //  If the caller does not want to skip heap validiation checks then we
    //  need to validate the rest of the flags but simply masking out only
    //  those flags that want on a create heap call
    //

    if (!(Flags & HEAP_SKIP_VALIDATION_CHECKS)) {

        if (Flags & ~HEAP_CREATE_VALID_MASK) {

            HeapDebugPrint(( "Invalid flags (%08x) specified to RtlCreateHeap\n", Flags ));
            HeapDebugBreak( NULL );

            Flags &= HEAP_CREATE_VALID_MASK;
        }
    }
#endif // DBG

    //
    //  The maximum heap block size is really 0x7f000 which is 0x80000 minus a
    //  page.  Maximum block size is 0xfe00 and granularity shift is 3.
    //

    MaximumHeapBlockSize = HEAP_MAXIMUM_BLOCK_SIZE << HEAP_GRANULARITY_SHIFT;

    //
    //  Assume we're going to be successful until we're shown otherwise
    //

    Status = STATUS_SUCCESS;

    //
    //  This part of the routine builds up local variable containing all the
    //  parameters used to initialize the heap.  First thing we do is zero
    //  it out.
    //

    RtlZeroMemory( &TempParameters, sizeof( TempParameters ) );

    //
    //  If our caller supplied the optional heap parameters then we'll
    //  make sure the size is good and copy over them over to our
    //  local copy
    //

    if (ARGUMENT_PRESENT( Parameters )) {

        try {

            if (Parameters->Length == sizeof( *Parameters )) {

                RtlMoveMemory( &TempParameters, Parameters, sizeof( *Parameters ) );
            }

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            Status = GetExceptionCode();
        }

        if (!NT_SUCCESS( Status )) {

            return NULL;
        }
    }

    //
    //  Set the parameter block to the local copy
    //

    Parameters = &TempParameters;

    //
    //  If nt global flags tells us to always do tail or free checking
    //  or to disable coalescing then force those bits set in the user
    //  specified flags
    //

#if DBG
    if (NtGlobalFlag & FLG_HEAP_ENABLE_TAIL_CHECK) {

        Flags |= HEAP_TAIL_CHECKING_ENABLED;
    }

    if (NtGlobalFlag & FLG_HEAP_ENABLE_FREE_CHECK) {

        Flags |= HEAP_FREE_CHECKING_ENABLED;
    }
#endif // DBG

    if (NtGlobalFlag & FLG_HEAP_DISABLE_COALESCING) {

        Flags |= HEAP_DISABLE_COALESCE_ON_FREE;
    }

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case we also check if we should
    //  validate parameters, validate all, or do stack backtraces
    //

    Peb = NtCurrentPeb();

#if DBG
    if (NtGlobalFlag & FLG_HEAP_VALIDATE_PARAMETERS) {

        Flags |= HEAP_VALIDATE_PARAMETERS_ENABLED;
    }

    if (NtGlobalFlag & FLG_HEAP_VALIDATE_ALL) {

        Flags |= HEAP_VALIDATE_ALL_ENABLED;
    }

    if (NtGlobalFlag & FLG_USER_STACK_TRACE_DB) {

        Flags |= HEAP_CAPTURE_STACK_BACKTRACES;
    }
#endif // DBG

    //
    //  Also in the non kernel case the PEB will have some state
    //  variables that we need to set if the user hasn't specified
    //  otherwise
    //

    if (Parameters->SegmentReserve == 0) {

        Parameters->SegmentReserve = Peb->HeapSegmentReserve;
    }

    if (Parameters->SegmentCommit == 0) {

        Parameters->SegmentCommit = Peb->HeapSegmentCommit;
    }

    if (Parameters->DeCommitFreeBlockThreshold == 0) {

        Parameters->DeCommitFreeBlockThreshold = Peb->HeapDeCommitFreeBlockThreshold;
    }

    if (Parameters->DeCommitTotalFreeThreshold == 0) {

        Parameters->DeCommitTotalFreeThreshold = Peb->HeapDeCommitTotalFreeThreshold;
    }
#else // NTOS_KERNEL_RUNTIME

    //
    //  In the kernel case Mm has some global variables that we set
    //  into the paramters if the user hasn't specified otherwise
    //

    if (Parameters->SegmentReserve == 0) {

        Parameters->SegmentReserve = MmHeapSegmentReserve;
    }

    if (Parameters->SegmentCommit == 0) {

        Parameters->SegmentCommit = MmHeapSegmentCommit;
    }

    if (Parameters->DeCommitFreeBlockThreshold == 0) {

        Parameters->DeCommitFreeBlockThreshold = MmHeapDeCommitFreeBlockThreshold;
    }

    if (Parameters->DeCommitTotalFreeThreshold == 0) {

        Parameters->DeCommitTotalFreeThreshold = MmHeapDeCommitTotalFreeThreshold;
    }
#endif // NTOS_KERNEL_RUNTIME

    //
    //  If the user hasn't said what the largest allocation size is then
    //  we should compute it as the difference between the highest and lowest
    //  address less one page
    //

    if (Parameters->MaximumAllocationSize == 0) {

        Parameters->MaximumAllocationSize = ((ULONG_PTR)MM_HIGHEST_USER_ADDRESS -
                                             (ULONG_PTR)MM_LOWEST_USER_ADDRESS -
                                             PAGE_SIZE );
    }

    //
    //  Set the virtual memory threshold to be non zero and not more than the
    //  maximum heap block size of 0x7f000.  If the user specified one that is
    //  too large we automatically and silently drop it down.
    //

    if ((Parameters->VirtualMemoryThreshold == 0) ||
        (Parameters->VirtualMemoryThreshold > MaximumHeapBlockSize)) {

        Parameters->VirtualMemoryThreshold = MaximumHeapBlockSize;
    }

    //
    //  The default commit size is one page and the default reserve size is
    //  64 pages.
    //
    //  **** this doesn't check that commit size if specified is less than
    //  **** reserved size if specified
    //

    if (!ARGUMENT_PRESENT( CommitSize )) {

        CommitSize = PAGE_SIZE;

        if (!ARGUMENT_PRESENT( ReserveSize )) {

            ReserveSize = 64 * CommitSize;

        } else {

            ReserveSize = ROUND_UP_TO_POWER2( ReserveSize, PAGE_SIZE );
        }

    } else {

        //
        //  The heap actually uses space that is reserved and commited
        //  to store internal data structures (the LOCK,
        //  the HEAP_PSEUDO_TAG, etc.). These structures can be larger than
        //  4K especially on a 64-bit build. So, make sure the commit
        //  is at least 8K in length which is the minimal page size for
        //  64-bit systems
        //

        CommitSize = ROUND_UP_TO_POWER2(CommitSize, PAGE_SIZE);

        if (!ARGUMENT_PRESENT( ReserveSize )) {

            ReserveSize = ROUND_UP_TO_POWER2( CommitSize, 16 * PAGE_SIZE );

        } else {

            ReserveSize = ROUND_UP_TO_POWER2( ReserveSize, PAGE_SIZE );
        }

    }

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case check if we are creating a debug heap
    //  the test checks that skip validation checks is false.
    //

    if (DEBUG_HEAP( Flags )) {

        return RtlDebugCreateHeap( Flags,
                                   HeapBase,
                                   ReserveSize,
                                   CommitSize,
                                   Lock,
                                   Parameters );
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  Compute the size of the heap which will be the
    //  heap struct itself and if we are to seralize with
    //  out own lock then add room for the lock.  If the
    //  user did not supply the lock then set the lock
    //  variable to -1.
    //

    SizeOfHeapHeader = sizeof( HEAP );

    if (!(Flags & HEAP_NO_SERIALIZE)) {

        if (ARGUMENT_PRESENT( Lock )) {

            Flags |= HEAP_LOCK_USER_ALLOCATED;

        } else {

            SizeOfHeapHeader += sizeof( HEAP_LOCK );
            Lock = (PHEAP_LOCK)-1;
        }

    } else if (ARGUMENT_PRESENT( Lock )) {

        //
        //  In this error case the call said not to seralize but also fed us
        //  a lock
        //

        return NULL;
    }

    //
    //  See if caller allocate the space for the heap.
    //

    if (ARGUMENT_PRESENT( HeapBase )) {

        //
        //  The call specified a heap base now check if there is
        //  a caller supplied commit routine
        //

        if (Parameters->CommitRoutine != NULL) {

            //
            //  The caller specified a commit routine so he caller
            //  also needs to have given us certain parameters and make
            //  sure the heap is not growable.  Otherwise it is an error
            //

            if ((Parameters->InitialCommit == 0) ||
                (Parameters->InitialReserve == 0) ||
                (Parameters->InitialCommit > Parameters->InitialReserve) ||
                (Flags & HEAP_GROWABLE)) {

                return NULL;
            }

            //
            //  Set the commited base and the uncommited base to the
            //  proper pointers within the heap.
            //

            CommittedBase = HeapBase;
            UnCommittedBase = (PCHAR)CommittedBase + Parameters->InitialCommit;
            ReserveSize = Parameters->InitialReserve;

            //
            //  Zero out a page of the heap where our first part goes
            //
            //  **** what if the size is less than a page
            //

            RtlZeroMemory( CommittedBase, PAGE_SIZE );

        } else {

            //
            //  The user gave us space but not commit routine
            //  So query the base to get its size
            //

            Status = ZwQueryVirtualMemory( HeapBase,
                                           &MemoryInformation );

            if (!NT_SUCCESS( Status )) {

                return NULL;
            }

            //
            //  Make sure the user gave us a base address for this block
            //  and that the memory is not free
            //

            if (MemoryInformation.BaseAddress != HeapBase) {

                return NULL;
            }

            if (MemoryInformation.State == MEM_FREE) {

                return NULL;
            }

            //
            //  Set our commit base to the start of the range
            //

            CommittedBase = MemoryInformation.BaseAddress;

            //
            //  If the memory is commmitted then
            //  we can zero out a page worth
            //

            if (MemoryInformation.State == MEM_COMMIT) {

                RtlZeroMemory( CommittedBase, PAGE_SIZE );

                //
                //  Set the commit size and uncommited base according
                //  to the start of the vm
                //

                CommitSize = MemoryInformation.RegionSize;
                UnCommittedBase = (PCHAR)CommittedBase + CommitSize;

                //
                //  Find out the uncommited base is reserved and if so
                //  the update the reserve size accordingly.
                //

                Status = ZwQueryVirtualMemory( UnCommittedBase,
                                               &MemoryInformation );

                ReserveSize = CommitSize;

                if ((NT_SUCCESS( Status )) &&
                    (MemoryInformation.State == MEM_RESERVE)) {

                    ReserveSize += MemoryInformation.RegionSize;
                }

            } else {

                //
                //  The memory the user gave us is not committed so dummy
                //  up these small nummbers
                //

                CommitSize = PAGE_SIZE;
                UnCommittedBase = CommittedBase;
            }
        }

        //
        //  This user gave us a base and we've just taken care of the committed
        //  bookkeeping.  So mark this segment as user supplied and set the
        //  heap
        //

        SegmentFlags = HEAP_SEGMENT_USER_ALLOCATED;
        Heap = (PHEAP)HeapBase;

    } else {

        //
        //  The user did not specify a heap base so we have to allocate the
        //  vm here.  First make sure the user did not give us a commit routine
        //

        if (Parameters->CommitRoutine != NULL) {

            return NULL;
        }

        //
        //  Reserve the amount of virtual address space requested.
        //

        Status = ZwAllocateVirtualMemory( (PVOID *)&Heap,
                                          0,
                                          &ReserveSize,
                                          MEM_RESERVE,
                                          PAGE_READWRITE );

        if (!NT_SUCCESS( Status )) {

            return NULL;
        }

        //
        //  Indicate that this segment is not user supplied
        //

        SegmentFlags = 0;

        //
        //  Set the default commit size to one page
        //

        if (!ARGUMENT_PRESENT( CommitSize )) {

            CommitSize = PAGE_SIZE;
        }

        //
        //  Set the committed and uncommitted base to be the same the following
        //  code will actually commit the page for us
        //

        CommittedBase = Heap;
        UnCommittedBase = Heap;
    }

    //
    //  At this point we have a heap pointer, committed base, uncommitted base,
    //  segment flags, commit size, and reserve size.  If the committed and
    //  uncommited base are the same then we need to commit the amount
    //  specified by the commit size
    //

    if (CommittedBase == UnCommittedBase) {

        Status = ZwAllocateVirtualMemory( (PVOID *)&CommittedBase,
                                          0,
                                          &CommitSize,
                                          MEM_COMMIT,
                                          PAGE_READWRITE );

        //
        //  In the non successful case we need to back out any vm reservation
        //  we did earlier
        //

        if (!NT_SUCCESS( Status )) {

            if (!ARGUMENT_PRESENT(HeapBase)) {

                //
                //  Return the reserved virtual address space.
                //

                ZwFreeVirtualMemory( (PVOID *)&Heap,
                                     &ReserveSize,
                                     MEM_RELEASE );

            }

            return NULL;
        }

        //
        //  The new uncommitted base is not adjusted above what we just
        //  committed
        //

        UnCommittedBase = (PVOID)((PCHAR)UnCommittedBase + CommitSize);
    }

    //
    //  At this point we have memory for the start of the heap committed and
    //  ready to be initialized.  So now we need initialize the heap
    //

    //
    //  Calculate the end of the heap header and make room for 8 uncommitted
    //  range structures.  Once we have the room for them then chain them
    //  together and null terminate the chain
    //

    NextHeapHeaderAddress = Heap + 1;

    UnCommittedRange = (PHEAP_UNCOMMMTTED_RANGE)ROUND_UP_TO_POWER2( NextHeapHeaderAddress,
                                                                    sizeof( QUAD ) );

    InitialCountOfUnusedUnCommittedRanges = 8;

    SizeOfHeapHeader += InitialCountOfUnusedUnCommittedRanges * sizeof( *UnCommittedRange );

    //
    //  **** what a hack Pp is really a pointer to the next field of the
    //  **** uncommmtted range structure.  So we set next by setting through Pp
    //

    pp = &Heap->UnusedUnCommittedRanges;

    while (InitialCountOfUnusedUnCommittedRanges--) {

        *pp = UnCommittedRange;
        pp = &UnCommittedRange->Next;
        UnCommittedRange += 1;
    }

    NextHeapHeaderAddress = UnCommittedRange;

    *pp = NULL;

    //
    //  Check if tagging is enabled in global flags.  This check is always true
    //  in a debug build.
    //
    //  If tagging is enabled then make room for 129 pseudo tag heap entry.
    //  Which is one more than the number of free lists.  Also point the heap
    //  header to this array of pseudo tags entries.
    //

    if (IS_HEAP_TAGGING_ENABLED()) {

        Heap->PseudoTagEntries = (PHEAP_PSEUDO_TAG_ENTRY)ROUND_UP_TO_POWER2( NextHeapHeaderAddress,
                                                                             sizeof( QUAD ) );

        SizeOfHeapHeader += HEAP_NUMBER_OF_PSEUDO_TAG * sizeof( HEAP_PSEUDO_TAG_ENTRY );

        //
        //  **** this advancement of the next heap address doesn't seem right
        //  **** given that a pseudo heap entry is 12 ulongs in length and not
        //  **** a single byte

        NextHeapHeaderAddress = Heap->PseudoTagEntries + HEAP_NUMBER_OF_PSEUDO_TAG;
    }

    //
    //  Round the size of the heap header to the next 8 byte boundary
    //

    SizeOfHeapHeader = (ULONG) ROUND_UP_TO_POWER2( SizeOfHeapHeader,
                                                   HEAP_GRANULARITY );

    //
    //  If the sizeof the heap header is larger than the native
    //  page size, you have a problem. Further, if the CommitSize passed
    //  in was smaller than the SizeOfHeapHeader, you may not even make it
    //  this far before death...
    //
    //  HeapDbgPrint() doesn't work for IA64 yet.
    //
    //  HeapDbgPrint(("Size of the heap header is %u bytes, commit was %u bytes\n", SizeOfHeapHeader, (ULONG) CommitSize));
    //

    //
    //  Fill in the heap header fields
    //

    Heap->Entry.Size = (USHORT)(SizeOfHeapHeader >> HEAP_GRANULARITY_SHIFT);
    Heap->Entry.Flags = HEAP_ENTRY_BUSY;

    Heap->Signature = HEAP_SIGNATURE;
    Heap->Flags = Flags;
    Heap->ForceFlags = (Flags & (HEAP_NO_SERIALIZE |
                                 HEAP_GENERATE_EXCEPTIONS |
                                 HEAP_ZERO_MEMORY |
                                 HEAP_REALLOC_IN_PLACE_ONLY |
                                 HEAP_VALIDATE_PARAMETERS_ENABLED |
                                 HEAP_VALIDATE_ALL_ENABLED |
                                 HEAP_TAIL_CHECKING_ENABLED |
                                 HEAP_CREATE_ALIGN_16 |
                                 HEAP_FREE_CHECKING_ENABLED));

    Heap->FreeListsInUseTerminate = 0xFFFF;
    Heap->HeaderValidateLength = (USHORT)((PCHAR)NextHeapHeaderAddress - (PCHAR)Heap);
    Heap->HeaderValidateCopy = NULL;

    //
    //  Initialize the free list to be all empty
    //

    FreeListHead = &Heap->FreeLists[ 0 ];
    n = HEAP_MAXIMUM_FREELISTS;

    while (n--) {

        InitializeListHead( FreeListHead );
        FreeListHead++;
    }

    //
    //  Make it so that there a no big block allocations
    //

    InitializeListHead( &Heap->VirtualAllocdBlocks );

    //
    //  Initialize the cricital section that controls access to
    //  the free list.  If the lock variable is -1 then the caller
    //  did not supply a lock so we need to make room for one
    //  and initialize it.
    //

    if (Lock == (PHEAP_LOCK)-1) {

        Lock = (PHEAP_LOCK)NextHeapHeaderAddress;

        Status = RtlInitializeLockRoutine( Lock );

        if (!NT_SUCCESS( Status )) {

            return NULL;
        }

        NextHeapHeaderAddress = (PHEAP_LOCK)Lock + 1;
    }

    Heap->LockVariable = Lock;


    //
    //  Initialize the first segment for the heap
    //

    if (!RtlpInitializeHeapSegment( Heap,
                                    (PHEAP_SEGMENT)((PCHAR)Heap + SizeOfHeapHeader),
                                    0,
                                    SegmentFlags,
                                    CommittedBase,
                                    UnCommittedBase,
                                    (PCHAR)CommittedBase + ReserveSize )) {

        return NULL;
    }

    //
    //  Fill in additional heap entry fields
    //

    Heap->ProcessHeapsListIndex = 0;
    Heap->SegmentReserve = Parameters->SegmentReserve;
    Heap->SegmentCommit = Parameters->SegmentCommit;
    Heap->DeCommitFreeBlockThreshold = Parameters->DeCommitFreeBlockThreshold >> HEAP_GRANULARITY_SHIFT;
    Heap->DeCommitTotalFreeThreshold = Parameters->DeCommitTotalFreeThreshold >> HEAP_GRANULARITY_SHIFT;
    Heap->MaximumAllocationSize = Parameters->MaximumAllocationSize;

    Heap->VirtualMemoryThreshold = (ULONG) (ROUND_UP_TO_POWER2( Parameters->VirtualMemoryThreshold,
                                                       HEAP_GRANULARITY ) >> HEAP_GRANULARITY_SHIFT);

    Heap->CommitRoutine = Parameters->CommitRoutine;

    //
    //  We either align the heap at 16 or 8 byte boundaries.  The AlignRound
    //  and AlignMask are used to bring allocation sizes up to the next
    //  boundary.  The align round includes the heap header and the optional
    //  check tail size
    //

    if (Flags & HEAP_CREATE_ALIGN_16) {

        Heap->AlignRound = 15 + sizeof( HEAP_ENTRY );
        Heap->AlignMask = (ULONG)~15;

    } else {

        Heap->AlignRound = HEAP_GRANULARITY - 1 + sizeof( HEAP_ENTRY );
        Heap->AlignMask = (ULONG)~(HEAP_GRANULARITY - 1);
    }

#if DBG
    if (Heap->Flags & HEAP_TAIL_CHECKING_ENABLED) {

        Heap->AlignRound += CHECK_HEAP_TAIL_SIZE;
    }
#endif // DBG

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case we need to add this heap to the processes heap
    //  list
    //

    RtlpAddHeapToProcessList( Heap );

    //
    //  Initialize the heap lookaide lists.  This is only for the user mode
    //  heap and the heap contains a pointer to the lookaside list array.
    //  The array is sized the same as the dedicated free list.  First we
    //  allocate space for the lookaside list and then we initialize each
    //  lookaside list.
    //
    //  But the caller asked for no serialize or asked for non growable
    //  heap then we won't enable the lookaside lists.
    //

    Heap->Lookaside = NULL;
    Heap->LookasideLockCount = 0;

    if ((!(Flags & HEAP_NO_SERIALIZE)) &&
        ( (Flags & HEAP_GROWABLE)) &&
        (!(RtlpDisableHeapLookaside))) {

        ULONG i;

        Heap->Lookaside = RtlAllocateHeap( Heap,
                                           Flags,
                                           sizeof(HEAP_LOOKASIDE) * HEAP_MAXIMUM_FREELISTS );

        if (Heap->Lookaside != NULL) {

            for (i = 0; i < HEAP_MAXIMUM_FREELISTS; i += 1) {

                RtlpInitializeHeapLookaside( &(((PHEAP_LOOKASIDE)(Heap->Lookaside))[i]),
                                             32 );
            }
        }
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  And return the fully initialized heap to our caller
    //

    return (PVOID)Heap;
}


PVOID
RtlDestroyHeap (
    IN PVOID HeapHandle
    )

/*++

Routine Description:

    This routine is the opposite of Rtl Create Heap.  It tears down an
    existing heap structure.

Arguments:

    HeapHandle - Supplies a pointer to the heap being destroyed

Return Value:

    PVOID - Returns null if the heap was destroyed completely and a
        pointer back to the heap if for some reason the heap could
        not be destroyed.

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    PHEAP_SEGMENT Segment;
    PHEAP_UCR_SEGMENT UCRSegments;
    PLIST_ENTRY Head, Next;
    PVOID BaseAddress;
    SIZE_T RegionSize;
    UCHAR SegmentIndex;

    //
    //  Validate that HeapAddress points to a HEAP structure.
    //

    RTL_PAGED_CODE();

    if (HeapHandle == NULL) {
        HeapDebugPrint(( "Ignoring RtlDestroyHeap( NULL )\n" ));

        return NULL;
    }

#ifndef NTOS_KERNEL_RUNTIME
#ifdef NTHEAP_ENABLED
    {
        if (Heap->Flags & NTHEAP_ENABLED_FLAG) {

            return RtlDestroyNtHeap( HeapHandle );
        }
    }
#endif // NTHEAP_ENABLED
#endif // NTOS_KERNEL_RUNTIME

    //
    //  Check if this is the debug version of heap using page allocation
    //  with guard pages
    //

    IF_DEBUG_PAGE_HEAP_THEN_RETURN( HeapHandle,
                                    RtlpDebugPageHeapDestroy( HeapHandle ));

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case check if this is the debug version of heap
    //  and of so then call the debug version to do the teardown
    //

    if (DEBUG_HEAP( Heap->Flags )) {

        if (!RtlDebugDestroyHeap( HeapHandle )) {

            return HeapHandle;
        }
    }

    //
    //  We are not allowed to destroy the process heap
    //

    if (HeapHandle == NtCurrentPeb()->ProcessHeap) {

        return HeapHandle;
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  For every big allocation we remove it from the list and free the
    //  vm
    //

    Head = &Heap->VirtualAllocdBlocks;
    Next = Head->Flink;

    while (Head != Next) {

        BaseAddress = CONTAINING_RECORD( Next, HEAP_VIRTUAL_ALLOC_ENTRY, Entry );

        Next = Next->Flink;
        RegionSize = 0;

        ZwFreeVirtualMemory( (PVOID *)&BaseAddress,
                             &RegionSize,
                             MEM_RELEASE );
    }

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case we need to destory any heap tags we have setup
    //  and remove this heap from the process heap list
    //

    RtlpDestroyTags( Heap );
    RtlpRemoveHeapFromProcessList( Heap );

#endif // NTOS_KERNEL_RUNTIME

    //
    //  If the heap is serialized, delete the critical section created
    //  by RtlCreateHeap.
    //

    if (!(Heap->Flags & HEAP_NO_SERIALIZE)) {

        if (!(Heap->Flags & HEAP_LOCK_USER_ALLOCATED)) {

            RtlDeleteLockRoutine( Heap->LockVariable );
        }

        Heap->LockVariable = NULL;
    }

    //
    //  For every uncommitted segment we free its vm
    //

    UCRSegments = Heap->UCRSegments;
    Heap->UCRSegments = NULL;

    while (UCRSegments) {

        BaseAddress = UCRSegments;
        UCRSegments = UCRSegments->Next;
        RegionSize = 0;

        ZwFreeVirtualMemory( &BaseAddress,
                             &RegionSize,
                             MEM_RELEASE );
    }

    //
    //  For every segment in the heap we call a worker routine to
    //  destory the segment
    //

    SegmentIndex = HEAP_MAXIMUM_SEGMENTS;

    while (SegmentIndex--) {

        Segment = Heap->Segments[ SegmentIndex ];

        if (Segment) {

            RtlpDestroyHeapSegment( Segment );
        }
    }

    //
    //  And we return to our caller
    //

    return NULL;
}


PVOID
RtlAllocateHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    )

/*++

Routine Description:

    This routine allocates a memory of the specified size from the specified
    heap.

Arguments:

    HeapHandle - Supplies a pointer to an initialized heap structure

    Flags - Specifies the set of flags to use to control the allocation

    Size - Specifies the size, in bytes, of the allocation

Return Value:

    PVOID - returns a pointer to the newly allocated block

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    PULONG FreeListsInUse;
    ULONG FreeListsInUseUlong;
    SIZE_T AllocationSize;
    SIZE_T FreeSize, AllocationIndex;
    PLIST_ENTRY FreeListHead, Next;
    PHEAP_ENTRY BusyBlock;
    PHEAP_FREE_ENTRY FreeBlock, SplitBlock, SplitBlock2;
    ULONG InUseIndex;
    UCHAR FreeFlags;
    NTSTATUS Status;
    EXCEPTION_RECORD ExceptionRecord;
    PVOID ReturnValue;
    BOOLEAN LockAcquired = FALSE;

    RTL_PAGED_CODE();


#ifndef NTOS_KERNEL_RUNTIME
#ifdef NTHEAP_ENABLED
    {
        if (Heap->Flags & NTHEAP_ENABLED_FLAG) {

            return RtlAllocateNtHeap( HeapHandle,
                                      Flags,
                                      Size);
        }
    }
#endif // NTHEAP_ENABLED
#endif // NTOS_KERNEL_RUNTIME


    //
    //  Take the callers flags and add in the flags that we must forcibly set
    //  in the heap
    //

    Flags |= Heap->ForceFlags;

    //
    //  Check for special features that force us to call the slow, do-everything
    //  version.  We do everything slow for any of the following flags.
    //
    //    HEAP_SLOW_FLAGS defined as           0x6f030f60
    //
    //      HEAP_DEBUG_FLAGS, defined as       0x69020000 (heappriv.h)
    //
    //        HEAP_VALIDATE_PARAMETERS_ENABLED 0x40000000 (heap.h)
    //
    //        HEAP_VALIDATE_ALL_ENABLED        0x20000000 (heap.h)
    //
    //        HEAP_CAPTURE_STACK_BACKTRACES    0x08000000 (heap.h)
    //
    //        HEAP_CREATE_ENABLE_TRACING       0x00020000 (ntrtl.h winnt obsolete)
    //
    //        HEAP_FLAG_PAGE_ALLOCS            0x01000000 (heappage.h)
    //
    //      HEAP_SETTABLE_USER_FLAGS           0x00000E00 (ntrtl.h)
    //
    //      HEAP_NEED_EXTRA_FLAGS              0x0f000100 (heap.h)
    //
    //      HEAP_CREATE_ALIGN_16               0x00010000 (ntrtl.h winnt obsolete)
    //
    //      HEAP_FREE_CHECKING_ENABLED         0x00000040 (ntrtl.h winnt)
    //
    //      HEAP_TAIL_CHECKING_ENABLED         0x00000020 (ntrtl.h winnt )
    //
    //  We also do everything slow if the size is greater than max long
    //

#if DBG
    if ((Flags & HEAP_SLOW_FLAGS) || (Size >= 0x80000000)) {

        return RtlAllocateHeapSlowly( HeapHandle, Flags, Size );
    }
#endif // DBG

    //
    //  At this point we know we are doing everything in this routine
    //  and not taking the slow route.
    //
    //  Round the requested size up to the allocation granularity.  Note
    //  that if the request is for 0 bytes, we still allocate memory, because
    //  we add in an extra 1 byte to protect ourselves from idiots.
    //
    //      Allocation size will be either 16, 24, 32, ...
    //      Allocation index will be 2, 3, 4, ...
    //
    //  Note that allocation size 8 is skipped and are indices 0 and 1
    //

    AllocationSize = ((Size ? Size : 1) + HEAP_GRANULARITY - 1 + sizeof( HEAP_ENTRY ))
        & ~(HEAP_GRANULARITY -1);
    AllocationIndex = AllocationSize >>  HEAP_GRANULARITY_SHIFT;

    //
    //  If there is a lookaside list and the index is within limits then
    //  try and allocate from the lookaside list.  We'll actually capture
    //  the lookaside pointer from the heap and only use the captured pointer.
    //  This will take care of the condition where a walk or lock heap can
    //  cause us to check for a non null pointer and then have it become null
    //  when we read it again.  If it is non null to start with then even if
    //  the user walks or locks the heap via another thread the pointer to
    //  still valid here so we can still try and do a lookaside list pop.
    //

#ifndef NTOS_KERNEL_RUNTIME

    {
        PHEAP_LOOKASIDE Lookaside = (PHEAP_LOOKASIDE)Heap->Lookaside;

        if ((Lookaside != NULL) &&
            (Heap->LookasideLockCount == 0) &&
            (AllocationIndex < HEAP_MAXIMUM_FREELISTS)) {

            //
            //  If the number of operation elapsed operations is 128 times the
            //  lookaside depth then it is time to adjust the depth
            //

            if ((LONG)(Lookaside[AllocationIndex].TotalAllocates - Lookaside[AllocationIndex].LastTotalAllocates) >=
                      (Lookaside[AllocationIndex].Depth * 128)) {

                RtlpAdjustHeapLookasideDepth(&(Lookaside[AllocationIndex]));
            }

            ReturnValue = RtlpAllocateFromHeapLookaside(&(Lookaside[AllocationIndex]));

            if (ReturnValue != NULL) {

                PHEAP_ENTRY BusyBlock;

                BusyBlock = ((PHEAP_ENTRY)ReturnValue) - 1;
                BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);
                BusyBlock->SmallTagIndex = 0;

                if (Flags & HEAP_ZERO_MEMORY) {

                    RtlZeroMemory( ReturnValue, Size );
                }

                return ReturnValue;
            }
        }
    }

#endif // NTOS_KERNEL_RUNTIME

    try {

        //
        //  Check if we need to serialize our access to the heap
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            //
            //  Lock the free list.
            //

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;
        }

        //
        //  If the allocation index is less than the maximum free list size
        //  then we can use the index to check the free list otherwise we have
        //  to either pull the entry off of the [0] index list or allocate
        //  memory directly for this request.
        //

        if (AllocationIndex < HEAP_MAXIMUM_FREELISTS) {

            //
            //  With a size that matches a free list size grab the head
            //  of the list and check if there is an available entry
            //

            FreeListHead = &Heap->FreeLists[ AllocationIndex ];

            if ( !IsListEmpty( FreeListHead ))  {

                //
                //  We're in luck the list has an entry so now get the free
                //  entry,  copy its flags, remove it from the free list
                //

                FreeBlock = CONTAINING_RECORD( FreeListHead->Blink,
                                               HEAP_FREE_ENTRY,
                                               FreeList );

                FreeFlags = FreeBlock->Flags;

                RtlpFastRemoveDedicatedFreeBlock( Heap, FreeBlock );

                //
                //  Adjust the total number of bytes free in the heap
                //

                Heap->TotalFreeSize -= AllocationIndex;

                //
                //  Mark the block as busy and and set the number of bytes
                //  unused and tag index.  Also if it is the last entry
                //  then keep that flag.
                //

                BusyBlock = (PHEAP_ENTRY)FreeBlock;
                BusyBlock->Flags = HEAP_ENTRY_BUSY | (FreeFlags & HEAP_ENTRY_LAST_ENTRY);
                BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);
                BusyBlock->SmallTagIndex = 0;

            } else {

                //
                //  The free list that matches our request is empty
                //
                //  Scan the free list in use vector to find the smallest
                //  available free block large enough for our allocations.
                //

                //
                //  Compute the index of the ULONG where the scan should begin
                //

                InUseIndex = (ULONG) (AllocationIndex >> 5);
                FreeListsInUse = &Heap->u.FreeListsInUseUlong[InUseIndex];

                //
                //  Mask off the bits in the first ULONG that represent allocations
                //  smaller than we need.
                //

                FreeListsInUseUlong = *FreeListsInUse++ & ~((1 << ((ULONG) AllocationIndex & 0x1f)) - 1);

                //
                //  Begin unrolled loop to scan bit vector.
                //

                switch (InUseIndex) {

                case 0:

                    if (FreeListsInUseUlong) {

                        FreeListHead = &Heap->FreeLists[0];
                        break;
                    }

                    FreeListsInUseUlong = *FreeListsInUse++;

                    //
                    //  deliberate fallthrough to next ULONG
                    //

                case 1:

                    if (FreeListsInUseUlong) {

                        FreeListHead = &Heap->FreeLists[32];
                        break;
                    }

                    FreeListsInUseUlong = *FreeListsInUse++;

                    //
                    //  deliberate fallthrough to next ULONG
                    //

                case 2:

                    if (FreeListsInUseUlong) {

                        FreeListHead = &Heap->FreeLists[64];
                        break;
                    }

                    FreeListsInUseUlong = *FreeListsInUse++;

                    //
                    //  deliberate fallthrough to next ULONG
                    //

                case 3:

                    if (FreeListsInUseUlong) {

                        FreeListHead = &Heap->FreeLists[96];
                        break;
                    }

                    //
                    //  deliberate fallthrough to non dedicated list
                    //

                default:

                    //
                    //  No suitable entry on the free list was found.
                    //

                    goto LookInNonDedicatedList;
                }

                //
                //  A free list has been found with a large enough allocation.
                //  FreeListHead contains the base of the vector it was found in.
                //  FreeListsInUseUlong contains the vector.
                //

                FreeListHead += RtlFindFirstSetRightMember( FreeListsInUseUlong );

                //
                //  Grab the free block and remove it from the free list
                //

                FreeBlock = CONTAINING_RECORD( FreeListHead->Blink,
                                               HEAP_FREE_ENTRY,
                                               FreeList );

                RtlpFastRemoveDedicatedFreeBlock( Heap, FreeBlock );

    SplitFreeBlock:

                //
                //  Save the blocks flags and decrement the amount of
                //  free space left in the heap
                //

                FreeFlags = FreeBlock->Flags;
                Heap->TotalFreeSize -= FreeBlock->Size;

                //
                //  Mark the block busy
                //

                BusyBlock = (PHEAP_ENTRY)FreeBlock;
                BusyBlock->Flags = HEAP_ENTRY_BUSY;

                //
                //  Compute the size (i.e., index) of the amount from this block
                //  that we don't need and can return to the free list
                //

                FreeSize = BusyBlock->Size - AllocationIndex;

                //
                //  Finish setting up the rest of the new busy block
                //

                BusyBlock->Size = (USHORT)AllocationIndex;
                BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);
                BusyBlock->SmallTagIndex = 0;

                //
                //  Now if the size that we are going to free up is not zero
                //  then lets get to work and to the split.
                //

                if (FreeSize != 0) {

                    //
                    //  But first we won't ever bother doing a split that only
                    //  gives us 8 bytes back.  So if free size is one then just
                    //  bump up the size of the new busy block
                    //

                    if (FreeSize == 1) {

                        BusyBlock->Size += 1;
                        BusyBlock->UnusedBytes += sizeof( HEAP_ENTRY );

                    } else {

                        //
                        //  Get a pointer to where the new free block will be.
                        //  When we split a block the first part goes to the new
                        //  busy block and the second part goes back to the free
                        //  list
                        //

                        SplitBlock = (PHEAP_FREE_ENTRY)(BusyBlock + AllocationIndex);

                        //
                        //  Reset the flags that we copied from the original free list
                        //  header, and set it other size fields.
                        //

                        SplitBlock->Flags = FreeFlags;
                        SplitBlock->PreviousSize = (USHORT)AllocationIndex;
                        SplitBlock->SegmentIndex = BusyBlock->SegmentIndex;
                        SplitBlock->Size = (USHORT)FreeSize;

                        //
                        //  If nothing else follows this entry then we will insert
                        //  this into the corresponding free list (and update
                        //  Segment->LastEntryInSegment)
                        //

                        if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) {

                            RtlpFastInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize);
                            Heap->TotalFreeSize += FreeSize;

                        } else {

                            //
                            //  Otherwise we need to check the following block
                            //  and if it is busy then update its previous size
                            //  before inserting our new free block into the
                            //  free list
                            //

                            SplitBlock2 = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize);

                            if (SplitBlock2->Flags & HEAP_ENTRY_BUSY) {

                                SplitBlock2->PreviousSize = (USHORT)FreeSize;

                                RtlpFastInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );
                                Heap->TotalFreeSize += FreeSize;

                            } else {

                                //
                                //  The following block is free so we'll merge
                                //  these to blocks. by first merging the flags
                                //

                                SplitBlock->Flags = SplitBlock2->Flags;

                                //
                                //  Removing the second block from its free list
                                //

                                RtlpFastRemoveFreeBlock( Heap, SplitBlock2 );

                                //
                                //  Updating the free total number of free bytes
                                //  in the heap and updating the size of the new
                                //  free block
                                //

                                Heap->TotalFreeSize -= SplitBlock2->Size;
                                FreeSize += SplitBlock2->Size;

                                //
                                //  If the new free block is still less than the
                                //  maximum heap block size then we'll simply
                                //  insert it back in the free list
                                //

                                if (FreeSize <= HEAP_MAXIMUM_BLOCK_SIZE) {

                                    SplitBlock->Size = (USHORT)FreeSize;

                                    //
                                    //  Again check if the new following block
                                    //  exists and if so then updsate is previous
                                    //  size
                                    //

                                    if (!(SplitBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                                        ((PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize))->PreviousSize = (USHORT)FreeSize;
                                    }

                                    //
                                    //  Insert the new free block into the free
                                    //  list and update the free heap size
                                    //

                                    RtlpFastInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );
                                    Heap->TotalFreeSize += FreeSize;

                                } else {

                                    //
                                    //  The new free block is pretty large so we
                                    //  need to call a private routine to do the
                                    //  insert
                                    //

                                    RtlpInsertFreeBlock( Heap, SplitBlock, FreeSize );
                                }
                            }
                        }

                        //
                        //  Now that free flags made it back into a free block
                        //  we can zero out what we saved.
                        //

                        FreeFlags = 0;

                        //
                        //  If splitblock now last, update LastEntryInSegment
                        //

                        if (SplitBlock->Flags & HEAP_ENTRY_LAST_ENTRY) {

                            PHEAP_SEGMENT Segment;

                            Segment = Heap->Segments[SplitBlock->SegmentIndex];
                            Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;
                        }
                    }
                }

                //
                //  If there are no following entries then mark the new block as
                //  such
                //

                if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) {

                    BusyBlock->Flags |= HEAP_ENTRY_LAST_ENTRY;
                }
            }

            //
            //  Return the address of the user portion of the allocated block.
            //  This is the byte following the header.
            //

            ReturnValue = BusyBlock + 1;

            //
            //  **** Release the lock before the zero memory call
            //

            if (LockAcquired) {

                RtlReleaseLockRoutine( Heap->LockVariable );

                LockAcquired = FALSE;
            }
            
            //
            //  If the flags indicate that we should zero memory then do it now
            //

            if (Flags & HEAP_ZERO_MEMORY) {

                RtlZeroMemory( ReturnValue, Size );
            }

            //
            //  And return the allocated block to our caller
            //

            leave;

        //
        //  Otherwise the allocation request is bigger than the last dedicated
        //  free list size.  Now check if the size is within our threshold.
        //  Meaning that it could be in the [0] free list
        //

        } else if (AllocationIndex <= Heap->VirtualMemoryThreshold) {

    LookInNonDedicatedList:

            //
            //  The following code cycles through the [0] free list until
            //  it finds a block that satisfies the request.  The list
            //  is sorted so the search is can be terminated early on success
            //

            FreeListHead = &Heap->FreeLists[0];
            
            //
            //  Check if the largest block in the list is smaller than the request
            //

            Next = FreeListHead->Blink;

            if (FreeListHead != Next) {
                
                FreeBlock = CONTAINING_RECORD( Next, HEAP_FREE_ENTRY, FreeList );

                if (FreeBlock->Size >= AllocationIndex) {

                    //
                    //  Here we are sure there is at least a block here larger than
                    //  the requested size. Start searching from the first block
                    //

                    Next = FreeListHead->Flink;
                    
                    while (FreeListHead != Next) {

                        FreeBlock = CONTAINING_RECORD( Next, HEAP_FREE_ENTRY, FreeList );

                        if (FreeBlock->Size >= AllocationIndex) {

                            //
                            //  We've found something that we can use so now remove
                            //  it from the free list and go to where we treat spliting
                            //  a free block.  Note that the block we found here might
                            //  actually be the exact size we need and that is why
                            //  in the split free block case we have to consider having
                            //  nothing free after the split
                            //

                            RtlpFastRemoveNonDedicatedFreeBlock( Heap, FreeBlock );

                            goto SplitFreeBlock;
                        }

                        Next = Next->Flink;
                    }
                }
            }

            //
            //  The [0] list is either empty or everything is too small
            //  so now extend the heap which should get us something less
            //  than or equal to the virtual memory threshold
            //

            FreeBlock = RtlpExtendHeap( Heap, AllocationSize );

            //
            //  And provided we got something we'll treat it just like the previous
            //  split free block cases
            //

            if (FreeBlock != NULL) {

                RtlpFastRemoveNonDedicatedFreeBlock( Heap, FreeBlock );

                goto SplitFreeBlock;
            }

            //
            //  We weren't able to extend the heap so we must be out of memory
            //

            Status = STATUS_NO_MEMORY;

        //
        //  At this point the allocation is way too big for any of the free lists
        //  and we can only satisfy this request if the heap is growable
        //

        } else if (Heap->Flags & HEAP_GROWABLE) {

            PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

            VirtualAllocBlock = NULL;

            //
            //  Compute how much memory we will need for this allocation which
            //  will include the allocation size plus a header, and then go
            //  get the committed memory
            //

            AllocationSize += FIELD_OFFSET( HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

            Status = ZwAllocateVirtualMemory( (PVOID *)&VirtualAllocBlock,
                                              0,
                                              &AllocationSize,
                                              MEM_COMMIT | ((Flags & HEAP_ZERO_MEMORY) ? 0 : MEM_NOZERO),
                                              PAGE_READWRITE );

            if (NT_SUCCESS(Status)) {

                //
                //  Just committed, already zero.  Fill in the new block
                //  and insert it in the list of big allocation
                //
                RtlZeroMemory(VirtualAllocBlock, sizeof(*VirtualAllocBlock));

                VirtualAllocBlock->BusyBlock.Size = (USHORT)(AllocationSize - Size);
                VirtualAllocBlock->BusyBlock.Flags = HEAP_ENTRY_VIRTUAL_ALLOC | HEAP_ENTRY_EXTRA_PRESENT | HEAP_ENTRY_BUSY;
                VirtualAllocBlock->CommitSize = AllocationSize;
                VirtualAllocBlock->ReserveSize = AllocationSize;

                InsertTailList( &Heap->VirtualAllocdBlocks, (PLIST_ENTRY)VirtualAllocBlock );

                //
                //  Return the address of the user portion of the allocated block.
                //  This is the byte following the header.
                //

                ReturnValue = (PHEAP_ENTRY)(VirtualAllocBlock + 1);

                leave;
            }

        } else {

            Status = STATUS_BUFFER_TOO_SMALL;
        }

        //
        //  This is the error return.
        //

        if (Flags & HEAP_GENERATE_EXCEPTIONS) {

            //
            //  Construct an exception record.
            //

            ExceptionRecord.ExceptionCode = STATUS_NO_MEMORY;
            ExceptionRecord.ExceptionRecord = (PEXCEPTION_RECORD)NULL;
            ExceptionRecord.NumberParameters = 1;
            ExceptionRecord.ExceptionFlags = 0;
            ExceptionRecord.ExceptionInformation[ 0 ] = AllocationSize;

            RtlRaiseException( &ExceptionRecord );
        }

        SET_LAST_STATUS(Status);

        ReturnValue = NULL;

    } finally {

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    return ReturnValue;
}


PVOID
RtlAllocateHeapSlowly (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    )

/*++

Routine Description:

    This routine does the equivalent of Rtl Allocate Heap but it does it will
    additional heap consistency checking logic and tagging.

Arguments:

    HeapHandle - Supplies a pointer to an initialized heap structure

    Flags - Specifies the set of flags to use to control the allocation

    Size - Specifies the size, in bytes, of the allocation

Return Value:

    PVOID - returns a pointer to the newly allocated block

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    BOOLEAN LockAcquired = FALSE;
    PVOID ReturnValue = NULL;
    PULONG FreeListsInUse;
    ULONG FreeListsInUseUlong;
    SIZE_T AllocationSize;
    SIZE_T FreeSize, AllocationIndex;
    UCHAR EntryFlags, FreeFlags;
    PLIST_ENTRY FreeListHead, Next;
    PHEAP_ENTRY BusyBlock;
    PHEAP_FREE_ENTRY FreeBlock, SplitBlock, SplitBlock2;
    PHEAP_ENTRY_EXTRA ExtraStuff;
    NTSTATUS Status;
    EXCEPTION_RECORD ExceptionRecord;
    SIZE_T ZeroSize = 0;

    RTL_PAGED_CODE();

    //
    //  Note that Flags has already been OR'd with Heap->ForceFlags.
    //

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case check if we should be using the debug version
    //  of heap allocation
    //

    if (DEBUG_HEAP( Flags )) {

        return RtlDebugAllocateHeap( HeapHandle, Flags, Size );
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  If the size is greater than maxlong then say we can't allocate that
    //  much and return the error to our caller
    //

    if (Size > 0x7fffffff) {

        SET_LAST_STATUS( STATUS_NO_MEMORY );

        return NULL;
    }

    //
    //  Round up the requested size to the allocation granularity.  Note
    //  that if the request is for zero bytes we will still allocate memory,
    //
    //      Allocation size will be either 16, 24, 32, ...
    //      Allocation index will be 2, 3, 4, ...
    //

    AllocationSize = ((Size ? Size : 1) + Heap->AlignRound) & Heap->AlignMask;

    //
    //  Generate the flags needed for this heap entry.  Mark it busy and add
    //  any user settable bits.  Also if the input flag indicates any entry
    //  extra fields and we have a tag to use then make room for the extra
    //  fields in the heap entry
    //

    EntryFlags = (UCHAR)(HEAP_ENTRY_BUSY | ((Flags & HEAP_SETTABLE_USER_FLAGS) >> 4));

    if ((Flags & HEAP_NEED_EXTRA_FLAGS) || (Heap->PseudoTagEntries != NULL)) {

        EntryFlags |= HEAP_ENTRY_EXTRA_PRESENT;
        AllocationSize += sizeof( HEAP_ENTRY_EXTRA );
    }

    AllocationIndex = AllocationSize >> HEAP_GRANULARITY_SHIFT;

    try {

        //
        //  Lock the free list.
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;
        }

        //
        //  Do all the actual heap work under the protection of a try-except clause
        //  to protect us from corruption
        //

        try {

            //
            //  If the allocation index is less than the maximum free list size
            //  then we can use the index to check the free list otherwise we have
            //  to either pull the entry off of the [0] index list or allocate
            //  memory directly for this request.
            //

            if (AllocationIndex < HEAP_MAXIMUM_FREELISTS) {

                //
                //  With a size that matches a free list size grab the head
                //  of the list and check if there is an available entry
                //

                FreeListHead = &Heap->FreeLists[ AllocationIndex ];

                if ( !IsListEmpty( FreeListHead ))  {

                    //
                    //  We're in luck the list has an entry so now get the free
                    //  entry,  copy its flags, remove it from the free list
                    //

                    FreeBlock = CONTAINING_RECORD( FreeListHead->Flink,
                                                   HEAP_FREE_ENTRY,
                                                   FreeList );

                    FreeFlags = FreeBlock->Flags;

                    RtlpRemoveFreeBlock( Heap, FreeBlock );

                    //
                    //  Adjust the total number of bytes free in the heap
                    //

                    Heap->TotalFreeSize -= AllocationIndex;

                    //
                    //  Mark the block as busy and and set the number of bytes
                    //  unused and tag index.  Also if it is the last entry
                    //  then keep that flag.
                    //

                    BusyBlock = (PHEAP_ENTRY)FreeBlock;
                    BusyBlock->Flags = EntryFlags | (FreeFlags & HEAP_ENTRY_LAST_ENTRY);
                    BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);

                } else {

                    //
                    //  The free list that matches our request is empty.  We know
                    //  that there are 128 free lists managed by a 4 ulong bitmap.
                    //  The next big if-else-if statement will decide which ulong
                    //  we tackle
                    //
                    //  Check if the requested allocation index within the first
                    //  quarter of the free lists.
                    //

                    if (AllocationIndex < (HEAP_MAXIMUM_FREELISTS * 1) / 4) {

                        //
                        //  Grab a pointer to the corresponding bitmap ulong, and
                        //  then get the bit we're actually interested in to be the
                        //  first bit of the ulong.
                        //

                        FreeListsInUse = &Heap->u.FreeListsInUseUlong[ 0 ];
                        FreeListsInUseUlong = *FreeListsInUse++ >> ((ULONG) AllocationIndex & 0x1F);

                        //
                        //  If the remaining bitmap has any bits set then we know
                        //  there is a non empty list that is larger than our
                        //  requested index so find that bit and compute the list
                        //  head of the next non empty list
                        //

                        if (FreeListsInUseUlong) {

                            FreeListHead += RtlFindFirstSetRightMember( FreeListsInUseUlong );

                        } else {

                            //
                            //  The rest of the first ulong is all zeros so we need
                            //  to move to the second ulong
                            //

                            FreeListsInUseUlong = *FreeListsInUse++;

                            //
                            //  Check if the second ulong has any bits set and if
                            //  so then compute the list head of the next non empty
                            //  list
                            //

                            if (FreeListsInUseUlong) {

                                FreeListHead += ((HEAP_MAXIMUM_FREELISTS * 1) / 4) -
                                    (AllocationIndex & 0x1F)  +
                                    RtlFindFirstSetRightMember( FreeListsInUseUlong );

                            } else {

                                //
                                //  Do the same test for the third ulong
                                //

                                FreeListsInUseUlong = *FreeListsInUse++;

                                if (FreeListsInUseUlong) {

                                    FreeListHead += ((HEAP_MAXIMUM_FREELISTS * 2) / 4) -
                                        (AllocationIndex & 0x1F) +
                                        RtlFindFirstSetRightMember( FreeListsInUseUlong );

                                } else {

                                    //
                                    //  Repeat the test for the forth ulong, and if
                                    //  that one is also empty then we need to grab
                                    //  the allocation off of the [0] index list
                                    //

                                    FreeListsInUseUlong = *FreeListsInUse++;

                                    if (FreeListsInUseUlong) {

                                        FreeListHead += ((HEAP_MAXIMUM_FREELISTS * 3) / 4) -
                                            (AllocationIndex & 0x1F)  +
                                            RtlFindFirstSetRightMember( FreeListsInUseUlong );

                                    } else {

                                        goto LookInNonDedicatedList;
                                    }
                                }
                            }
                        }

                    //
                    //  Otherwise check if the requested allocation index lies
                    //  within the second quarter of the free lists.  We repeat the
                    //  test just like we did above on the second, third, and forth
                    //  bitmap ulongs.
                    //

                    } else if (AllocationIndex < (HEAP_MAXIMUM_FREELISTS * 2) / 4) {

                        FreeListsInUse = &Heap->u.FreeListsInUseUlong[ 1 ];
                        FreeListsInUseUlong = *FreeListsInUse++ >> ((ULONG) AllocationIndex & 0x1F);

                        if (FreeListsInUseUlong) {

                            FreeListHead += RtlFindFirstSetRightMember( FreeListsInUseUlong );

                        } else {

                            FreeListsInUseUlong = *FreeListsInUse++;

                            if (FreeListsInUseUlong) {

                                FreeListHead += ((HEAP_MAXIMUM_FREELISTS * 1) / 4) -
                                    (AllocationIndex & 0x1F)  +
                                    RtlFindFirstSetRightMember( FreeListsInUseUlong );

                            } else {

                                FreeListsInUseUlong = *FreeListsInUse++;

                                if (FreeListsInUseUlong) {

                                    FreeListHead += ((HEAP_MAXIMUM_FREELISTS * 2) / 4) -
                                        (AllocationIndex & 0x1F)  +
                                        RtlFindFirstSetRightMember( FreeListsInUseUlong );

                                } else {

                                    goto LookInNonDedicatedList;
                                }
                            }
                        }

                    //
                    //  Otherwise check if the requested allocation index lies
                    //  within the third quarter of the free lists. We repeat the
                    //  test just like we did above on the third and forth bitmap
                    //  ulongs
                    //

                    } else if (AllocationIndex < (HEAP_MAXIMUM_FREELISTS * 3) / 4) {

                        FreeListsInUse = &Heap->u.FreeListsInUseUlong[ 2 ];
                        FreeListsInUseUlong = *FreeListsInUse++ >> ((ULONG) AllocationIndex & 0x1F);

                        if (FreeListsInUseUlong) {

                            FreeListHead += RtlFindFirstSetRightMember( FreeListsInUseUlong );

                        } else {

                            FreeListsInUseUlong = *FreeListsInUse++;

                            if (FreeListsInUseUlong) {

                                FreeListHead += ((HEAP_MAXIMUM_FREELISTS * 1) / 4) -
                                    (AllocationIndex & 0x1F)  +
                                    RtlFindFirstSetRightMember( FreeListsInUseUlong );

                            } else {

                                goto LookInNonDedicatedList;
                            }
                        }

                    //
                    //  Lastly the requested allocation index must lie within the
                    //  last quarter of the free lists.  We repeat the test just
                    //  like we did above on the forth ulong
                    //

                    } else {

                        FreeListsInUse = &Heap->u.FreeListsInUseUlong[ 3 ];
                        FreeListsInUseUlong = *FreeListsInUse++ >> ((ULONG) AllocationIndex & 0x1F);

                        if (FreeListsInUseUlong) {

                            FreeListHead += RtlFindFirstSetRightMember( FreeListsInUseUlong );

                        } else {

                            goto LookInNonDedicatedList;
                        }
                    }

                    //
                    //  At this point the free list head points to a non empty free
                    //  list that is of greater size than we need.
                    //

                    FreeBlock = CONTAINING_RECORD( FreeListHead->Flink,
                                                   HEAP_FREE_ENTRY,
                                                   FreeList );

    SplitFreeBlock:

                    //
                    //  Remember the flags that go with this block and remove it
                    //  from its list
                    //

                    FreeFlags = FreeBlock->Flags;

                    RtlpRemoveFreeBlock( Heap, FreeBlock );

                    //
                    //  Adjust the amount free in the heap
                    //

                    Heap->TotalFreeSize -= FreeBlock->Size;

                    //
                    //  Mark the block busy
                    //

                    BusyBlock = (PHEAP_ENTRY)FreeBlock;
                    BusyBlock->Flags = EntryFlags;

                    //
                    //  Compute the size (i.e., index) of the amount from this
                    //  block that we don't need and can return to the free list
                    //

                    FreeSize = BusyBlock->Size - AllocationIndex;

                    //
                    //  Finish setting up the rest of the new busy block
                    //

                    BusyBlock->Size = (USHORT)AllocationIndex;
                    BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);

                    //
                    //  Now if the size that we are going to free up is not zero
                    //  then lets get to work and to the split.
                    //

                    if (FreeSize != 0) {

                        //
                        //  But first we won't ever bother doing a split that only
                        //  gives us 8 bytes back.  So if free size is one then
                        //  just bump up the size of the new busy block
                        //

                        if (FreeSize == 1) {

                            BusyBlock->Size += 1;
                            BusyBlock->UnusedBytes += sizeof( HEAP_ENTRY );

                        } else {

                            //
                            //  Get a pointer to where the new free block will be.
                            //  When we split a block the first part goes to the
                            //  new busy block and the second part goes back to the
                            //  free list
                            //

                            SplitBlock = (PHEAP_FREE_ENTRY)(BusyBlock + AllocationIndex);

                            //
                            //  Reset the flags that we copied from the original
                            //  free list header, and set it other size fields.
                            //

                            SplitBlock->Flags = FreeFlags;
                            SplitBlock->PreviousSize = (USHORT)AllocationIndex;
                            SplitBlock->SegmentIndex = BusyBlock->SegmentIndex;
                            SplitBlock->Size = (USHORT)FreeSize;

                            //
                            //  If nothing else follows this entry then we will
                            //  insert this into the corresponding free list
                            //

                            if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) {

                                RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                                Heap->TotalFreeSize += FreeSize;

                            } else {

                                //
                                //  Otherwise we need to check the following block
                                //  and if it is busy then update its previous size
                                //  before inserting our new free block into the
                                //  free list
                                //

                                SplitBlock2 = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize);

                                if (SplitBlock2->Flags & HEAP_ENTRY_BUSY) {

                                    SplitBlock2->PreviousSize = (USHORT)FreeSize;

                                    RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                                    Heap->TotalFreeSize += FreeSize;

                                } else {

                                    //
                                    //  The following block is free so we'll merge
                                    //  these to blocks. by first merging the flags
                                    //

                                    SplitBlock->Flags = SplitBlock2->Flags;

                                    //
                                    //  Removing the second block from its free
                                    //  list
                                    //

                                    RtlpRemoveFreeBlock( Heap, SplitBlock2 );

                                    //
                                    //  Updating the free total number of free
                                    //  bytes in the heap and updating the size of
                                    //  the new free block
                                    //

                                    Heap->TotalFreeSize -= SplitBlock2->Size;
                                    FreeSize += SplitBlock2->Size;

                                    //
                                    //  If the new free block is still less than
                                    //  the maximum heap block size then we'll
                                    //  simply insert it back in the free list
                                    //

                                    if (FreeSize <= HEAP_MAXIMUM_BLOCK_SIZE) {

                                        SplitBlock->Size = (USHORT)FreeSize;

                                        //
                                        //  Again check if the new following block
                                        //  exists and if so then updsate is
                                        //  previous size
                                        //

                                        if (!(SplitBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                                            ((PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize))->PreviousSize = (USHORT)FreeSize;
                                        }

                                        //
                                        //  Insert the new free block into the free
                                        //  list and update the free heap size
                                        //

                                        RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                                        Heap->TotalFreeSize += FreeSize;

                                    } else {

                                        //
                                        //  The new free block is pretty large so
                                        //  we need to call a private routine to do
                                        //  the insert
                                        //

                                        RtlpInsertFreeBlock( Heap, SplitBlock, FreeSize );
                                    }
                                }
                            }

                            //
                            //  Now that free flags made it back into a free block
                            //  we can zero out what we saved.
                            //

                            FreeFlags = 0;

                            //
                            //  If splitblock now last, update LastEntryInSegment
                            //

                            if (SplitBlock->Flags & HEAP_ENTRY_LAST_ENTRY) {

                                PHEAP_SEGMENT Segment;

                                Segment = Heap->Segments[SplitBlock->SegmentIndex];
                                Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;
                            }

                        }
                    }

                    //
                    //  If there are no following entries then mark the new block
                    //  as such
                    //

                    if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) {

                        BusyBlock->Flags |= HEAP_ENTRY_LAST_ENTRY;
                    }
                }

                //
                //  Return the address of the user portion of the allocated block.
                //  This is the byte following the header.
                //

                ReturnValue = BusyBlock + 1;

                //
                //  If the flags indicate that we should zero memory then
                //  remember how much to zero.  We'll do the zeroing later
                //

                if (Flags & HEAP_ZERO_MEMORY) {

                    ZeroSize = Size;

                //
                //  Otherwise if the flags indicate that we should fill heap then
                //  it it now.
                //

                }
#if DBG
                else if (Heap->Flags & HEAP_FREE_CHECKING_ENABLED) {

                    RtlFillMemoryUlong( (PCHAR)(BusyBlock + 1), Size & ~0x3, ALLOC_HEAP_FILL );
                }

                //
                //  If the flags indicate that we should do tail checking then copy
                //  the fill pattern right after the heap block.
                //

                if (Heap->Flags & HEAP_TAIL_CHECKING_ENABLED) {

                    RtlFillMemory( (PCHAR)ReturnValue + Size,
                                   CHECK_HEAP_TAIL_SIZE,
                                   CHECK_HEAP_TAIL_FILL );

                    BusyBlock->Flags |= HEAP_ENTRY_FILL_PATTERN;
                }
#endif // DBG

                BusyBlock->SmallTagIndex = 0;

                //
                //  If the flags indicate that there is an extra block persent then
                //  we'll fill it in
                //

                if (BusyBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                    ExtraStuff = RtlpGetExtraStuffPointer( BusyBlock );

                    RtlZeroMemory( ExtraStuff, sizeof( *ExtraStuff ));

    #ifndef NTOS_KERNEL_RUNTIME

                //
                //  In the non kernel case the tagging goes in either the extra
                //  stuff of the busy block small tag index
                //

                    if (IS_HEAP_TAGGING_ENABLED()) {

                        ExtraStuff->TagIndex = RtlpUpdateTagEntry( Heap,
                                                                   (USHORT)((Flags & HEAP_TAG_MASK) >> HEAP_TAG_SHIFT),
                                                                   0,
                                                                   BusyBlock->Size,
                                                                   AllocationAction );
                    }

                } else if (IS_HEAP_TAGGING_ENABLED()) {

                    BusyBlock->SmallTagIndex = (UCHAR)RtlpUpdateTagEntry( Heap,
                                                                          (USHORT)((Flags & HEAP_SMALL_TAG_MASK) >> HEAP_TAG_SHIFT),
                                                                          0,
                                                                          BusyBlock->Size,
                                                                          AllocationAction );

    #endif // NTOS_KERNEL_RUNTIME

                }

                //
                //  Return the address of the user portion of the allocated block.
                //  This is the byte following the header.
                //

                leave;

            //
            //  Otherwise the allocation request is bigger than the last dedicated
            //  free list size.  Now check if the size is within our threshold.
            //  Meaning that it could be in the [0] free list
            //

            } else if (AllocationIndex <= Heap->VirtualMemoryThreshold) {

    LookInNonDedicatedList:

                //
                //  The following code cycles through the [0] free list until
                //  it finds a block that satisfies the request.  The list
                //  is sorted so the search is can be terminated early on success
                //

                FreeListHead = &Heap->FreeLists[ 0 ];
                Next = FreeListHead->Flink;

                while (FreeListHead != Next) {

                    FreeBlock = CONTAINING_RECORD( Next, HEAP_FREE_ENTRY, FreeList );

                    if (FreeBlock->Size >= AllocationIndex) {

                        //
                        //  We've found something that we can use so now go to
                        //  where we treat spliting a free block.  Note that
                        //  the block we found here might actually be the exact
                        //  size we need and that is why in the split free block
                        //  case we have to consider having nothing free after the
                        //  split
                        //

                        goto SplitFreeBlock;

                    } else {

                        Next = Next->Flink;
                    }
                }

                //
                //  The [0] list is either empty or everything is too small
                //  so now extend the heap which should get us something less
                //  than or equal to the virtual memory threshold
                //

                FreeBlock = RtlpExtendHeap( Heap, AllocationSize );

                //
                //  And provided we got something we'll treat it just like the
                //  previous split free block cases
                //

                if (FreeBlock != NULL) {

                    goto SplitFreeBlock;
                }

                //
                //  We weren't able to extend the heap so we must be out of memory
                //

                Status = STATUS_NO_MEMORY;

            //
            //  At this point the allocation is way too big for any of the free
            //  lists and we can only satisfy this request if the heap is growable
            //

            } else if (Heap->Flags & HEAP_GROWABLE) {

                PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

                VirtualAllocBlock = NULL;

                //
                //  Compute how much memory we will need for this allocation which
                //  will include the allocation size plus a header, and then go
                //  get the committed memory
                //

                AllocationSize += FIELD_OFFSET( HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

                Status = ZwAllocateVirtualMemory( (PVOID *)&VirtualAllocBlock,
                                                  0,
                                                  &AllocationSize,
                                                  MEM_COMMIT,
                                                  PAGE_READWRITE );

                if (NT_SUCCESS( Status )) {

                    //
                    //  Just committed, already zero.  Fill in the new block
                    //  and insert it in the list of big allocation
                    //

                    VirtualAllocBlock->BusyBlock.Size = (USHORT)(AllocationSize - Size);
                    VirtualAllocBlock->BusyBlock.Flags = EntryFlags | HEAP_ENTRY_VIRTUAL_ALLOC | HEAP_ENTRY_EXTRA_PRESENT;
                    VirtualAllocBlock->CommitSize = AllocationSize;
                    VirtualAllocBlock->ReserveSize = AllocationSize;

    #ifndef NTOS_KERNEL_RUNTIME

                    //
                    //  In the non kernel case see if we need to add heap tagging
                    //

                    if (IS_HEAP_TAGGING_ENABLED()) {

                        VirtualAllocBlock->ExtraStuff.TagIndex =
                            RtlpUpdateTagEntry( Heap,
                                                (USHORT)((Flags & HEAP_SMALL_TAG_MASK) >> HEAP_TAG_SHIFT),
                                                0,
                                                VirtualAllocBlock->CommitSize >> HEAP_GRANULARITY_SHIFT,
                                                VirtualAllocationAction );
                    }

    #endif // NTOS_KERNEL_RUNTIME

                    InsertTailList( &Heap->VirtualAllocdBlocks, (PLIST_ENTRY)VirtualAllocBlock );

                    //
                    //  Return the address of the user portion of the allocated
                    //  block.  This is the byte following the header.
                    //

                    ReturnValue = (PHEAP_ENTRY)(VirtualAllocBlock + 1);

                    leave;
                }

            //
            //  Otherwise we have an error condition
            //

            } else {

                Status = STATUS_BUFFER_TOO_SMALL;
            }

            SET_LAST_STATUS( Status );

            if (Flags & HEAP_GENERATE_EXCEPTIONS) {

                //
                //  Construct an exception record.
                //

                ExceptionRecord.ExceptionCode = STATUS_NO_MEMORY;
                ExceptionRecord.ExceptionRecord = (PEXCEPTION_RECORD)NULL;
                ExceptionRecord.NumberParameters = 1;
                ExceptionRecord.ExceptionFlags = 0;
                ExceptionRecord.ExceptionInformation[ 0 ] = AllocationSize;

                RtlRaiseException( &ExceptionRecord );
            }

        } except( GetExceptionCode() == STATUS_NO_MEMORY ? EXCEPTION_CONTINUE_SEARCH :
                                                           EXCEPTION_EXECUTE_HANDLER ) {

            SET_LAST_STATUS( GetExceptionCode() );
        }

        //
        //  Check if there is anything to zero out
        //

        if ( ZeroSize ) {

            RtlZeroMemory( ReturnValue, ZeroSize );
        }

    } finally {

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    //
    //  And return to our caller
    //

    return ReturnValue;
}



PVOID
RtlReAllocateHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN SIZE_T Size
    )

/*++

Routine Description:

    This routine will resize a user specified heap block.  The new size
    can either be smaller or larger than the current block size.

Arguments:

    HeapHandle - Supplies a pointer to the heap being modified

    Flags - Supplies a set of heap flags to augment those already
        enforced by the heap

    BaseAddress - Supplies the current address of a block allocated
        from heap.  We will try and resize this block at its current
        address, but it could possibly move if this heap structure
        allows for relocation

    Size - Supplies the size, in bytes, for the newly resized heap
        block

Return Value:

    PVOID - A pointer to the resized block.  If the block had to move
        then this address will not be equal to the input base address

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    SIZE_T AllocationSize;
    PHEAP_ENTRY BusyBlock, NewBusyBlock;
    PHEAP_ENTRY_EXTRA OldExtraStuff, NewExtraStuff;
    SIZE_T FreeSize;
    BOOLEAN LockAcquired = FALSE;
    PVOID NewBaseAddress;
    PHEAP_FREE_ENTRY SplitBlock, SplitBlock2;
    SIZE_T OldSize;
    SIZE_T AllocationIndex;
    SIZE_T OldAllocationIndex;
    UCHAR FreeFlags;
    NTSTATUS Status;
    PVOID DeCommitAddress;
    SIZE_T DeCommitSize;
    EXCEPTION_RECORD ExceptionRecord;

    //
    //  If there isn't an address to relocate the heap at then our work is done
    //

    if (BaseAddress == NULL) {

        SET_LAST_STATUS( STATUS_SUCCESS );

        return NULL;
    }

#ifndef NTOS_KERNEL_RUNTIME
#ifdef NTHEAP_ENABLED
    {
        if (Heap->Flags & NTHEAP_ENABLED_FLAG) {

            return RtlReAllocateNtHeap( HeapHandle, Flags, BaseAddress, Size );
        }
    }
#endif // NTHEAP_ENABLED
#endif

    //
    //  Augment the heap flags
    //

    Flags |= Heap->ForceFlags;

    //
    //  Check if we should simply call the debug version of heap to do the work
    //

#ifndef NTOS_KERNEL_RUNTIME
    if (DEBUG_HEAP( Flags)) {

        return RtlDebugReAllocateHeap( HeapHandle, Flags, BaseAddress, Size );
    }
#endif

    //
    //  Make sure we didn't get a negative heap size
    //

    if (Size > 0x7fffffff) {

        SET_LAST_STATUS( STATUS_NO_MEMORY );

        return NULL;
    }

    //
    //  Round the requested size up to the allocation granularity.  Note
    //  that if the request is for 0 bytes, we still allocate memory, because
    //  we add in an extra byte to protect ourselves from idiots.
    //

    AllocationSize = ((Size ? Size : 1) + Heap->AlignRound) & Heap->AlignMask;

    if ((Flags & HEAP_NEED_EXTRA_FLAGS) ||
        (Heap->PseudoTagEntries != NULL) ||
        ((((PHEAP_ENTRY)BaseAddress)-1)->Flags & HEAP_ENTRY_EXTRA_PRESENT)) {

        AllocationSize += sizeof( HEAP_ENTRY_EXTRA );
    }

    try {

        //
        //  Lock the heap
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;

            //
            //  Because it is now zero the following statement will set the no
            //  serialize bit
            //

            Flags ^= HEAP_NO_SERIALIZE;
        }

        try {

            //
            //  Compute the heap block address for user specified block
            //

            BusyBlock = (PHEAP_ENTRY)BaseAddress - 1;

            //
            //  Check if the block is not in use then it is an error
            //

            if (!(BusyBlock->Flags & HEAP_ENTRY_BUSY)) {

                SET_LAST_STATUS( STATUS_INVALID_PARAMETER );

                //
                //  Bail if not a busy block.
                //

                leave;

            //
            //  We need the current (i.e., old) size and allocation of the
            //  block.  Check if the block is a big allocation.  The size
            //  field of a big block is really the unused by count
            //

            } else if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

                OldSize = RtlpGetSizeOfBigBlock( BusyBlock );

                OldAllocationIndex = (OldSize + BusyBlock->Size) >> HEAP_GRANULARITY_SHIFT;

                //
                //  We'll need to adjust the new allocation size to account
                //  for the big block header and then round it up to a page
                //

                AllocationSize += FIELD_OFFSET( HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );
                AllocationSize = ROUND_UP_TO_POWER2( AllocationSize, PAGE_SIZE );

            //
            //  Otherwise the block is in use and is a small allocation
            //

            } else {

                OldAllocationIndex = BusyBlock->Size;

                OldSize = (OldAllocationIndex << HEAP_GRANULARITY_SHIFT) -
                          BusyBlock->UnusedBytes;
            }

            //
            //  Compute the new allocation index
            //

            AllocationIndex = AllocationSize >> HEAP_GRANULARITY_SHIFT;

            //
            //  At this point we have the old size and index, and the new size
            //  and index
            //
            //  See if new size less than or equal to the current size.
            //

            if (AllocationIndex <= OldAllocationIndex) {

                //
                //  If the new allocation index is only one less then the current
                //  index then make the sizes equal
                //

                if (AllocationIndex + 1 == OldAllocationIndex) {

                    AllocationIndex += 1;
                    AllocationSize += sizeof( HEAP_ENTRY );
                }

                //
                //  Calculate new residual (unused) amount
                //

                if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

                    //
                    //  In a big block the size is really the unused byte count
                    //

                    BusyBlock->Size = (USHORT)(AllocationSize - Size);

                } else if (BusyBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                    //
                    //  The extra stuff struct goes after the data.  So compute
                    //  the old and new extra stuff location and copy the data
                    //

                    OldExtraStuff = (PHEAP_ENTRY_EXTRA)(BusyBlock + BusyBlock->Size - 1);

                    NewExtraStuff = (PHEAP_ENTRY_EXTRA)(BusyBlock + AllocationIndex - 1);

                    *NewExtraStuff = *OldExtraStuff;

#ifndef NTOS_KERNEL_RUNTIME

                    //
                    //  If we're doing heap tagging then update the tag entry
                    //

                    if (IS_HEAP_TAGGING_ENABLED()) {

                        NewExtraStuff->TagIndex =
                            RtlpUpdateTagEntry( Heap,
                                                NewExtraStuff->TagIndex,
                                                OldAllocationIndex,
                                                AllocationIndex,
                                                ReAllocationAction );
                    }
#endif

                    BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);

                } else {

                    //
                    //  If we're doing heap tagging then update the tag entry
                    //

#ifndef NTOS_KERNEL_RUNTIME

                    if (IS_HEAP_TAGGING_ENABLED()) {

                        BusyBlock->SmallTagIndex = (UCHAR)
                            RtlpUpdateTagEntry( Heap,
                                                BusyBlock->SmallTagIndex,
                                                BusyBlock->Size,
                                                AllocationIndex,
                                                ReAllocationAction );
                    }

#endif

                    BusyBlock->UnusedBytes = (UCHAR)(AllocationSize - Size);
                }

                //
                //  Check if the block is getting bigger, then fill in the extra
                //  space.
                //
                //  **** how can this happen if the allocation index is less than or
                //  **** equal to the old allocation index
                //

                if (Size > OldSize) {

                    //
                    //  See if we should zero the extra space
                    //

                    if (Flags & HEAP_ZERO_MEMORY) {

                        RtlZeroMemory( (PCHAR)BaseAddress + OldSize,
                                       Size - OldSize );

                    //
                    //  Otherwise see if we should fill the extra space
                    //

                    }
#if DBG
                    else if (Heap->Flags & HEAP_FREE_CHECKING_ENABLED) {

                        SIZE_T PartialBytes, ExtraSize;

                        PartialBytes = OldSize & (sizeof( ULONG ) - 1);

                        if (PartialBytes) {

                            PartialBytes = 4 - PartialBytes;
                        }

                        if (Size > (OldSize + PartialBytes)) {

                            ExtraSize = (Size - (OldSize + PartialBytes)) & ~(sizeof( ULONG ) - 1);

                            if (ExtraSize != 0) {

                                RtlFillMemoryUlong( (PCHAR)(BusyBlock + 1) + OldSize + PartialBytes,
                                                    ExtraSize,
                                                    ALLOC_HEAP_FILL );
                            }
                        }
                    }
#endif // DBG
                }

#if DBG
                if (Heap->Flags & HEAP_TAIL_CHECKING_ENABLED) {

                    RtlFillMemory( (PCHAR)(BusyBlock + 1) + Size,
                                   CHECK_HEAP_TAIL_SIZE,
                                   CHECK_HEAP_TAIL_FILL );
                }
#endif // DBG
                //
                //  If amount of change is greater than the size of a free block,
                //  then need to free the extra space.  Otherwise, nothing else to
                //  do.
                //

                if (AllocationIndex != OldAllocationIndex) {

                    FreeFlags = BusyBlock->Flags & ~HEAP_ENTRY_BUSY;

                    if (FreeFlags & HEAP_ENTRY_VIRTUAL_ALLOC) {

                        PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

                        VirtualAllocBlock = CONTAINING_RECORD( BusyBlock, HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

#ifndef NTOS_KERNEL_RUNTIME

                        if (IS_HEAP_TAGGING_ENABLED()) {

                            VirtualAllocBlock->ExtraStuff.TagIndex =
                                RtlpUpdateTagEntry( Heap,
                                                    VirtualAllocBlock->ExtraStuff.TagIndex,
                                                    OldAllocationIndex,
                                                    AllocationIndex,
                                                    VirtualReAllocationAction );
                        }

#endif

                        DeCommitAddress = (PCHAR)VirtualAllocBlock + AllocationSize;

                        DeCommitSize = (OldAllocationIndex << HEAP_GRANULARITY_SHIFT) -
                                       AllocationSize;

                        Status = ZwFreeVirtualMemory( (PVOID *)&DeCommitAddress,
                                                      &DeCommitSize,
                                                      MEM_RELEASE );

                        if (!NT_SUCCESS( Status )) {

                            HeapDebugPrint(( "Unable to release memory at %p for %p bytes - Status == %x\n",
                                             DeCommitAddress, DeCommitSize, Status ));

                            HeapDebugBreak( NULL );

                        } else {

                            VirtualAllocBlock->CommitSize -= DeCommitSize;
                        }

                    } else {

                        //
                        //  Otherwise, shrink size of this block to new size, and make extra
                        //  space at end free.
                        //

                        SplitBlock = (PHEAP_FREE_ENTRY)(BusyBlock + AllocationIndex);

                        SplitBlock->Flags = FreeFlags;

                        SplitBlock->PreviousSize = (USHORT)AllocationIndex;

                        SplitBlock->SegmentIndex = BusyBlock->SegmentIndex;

                        FreeSize = BusyBlock->Size - AllocationIndex;

                        BusyBlock->Size = (USHORT)AllocationIndex;

                        BusyBlock->Flags &= ~HEAP_ENTRY_LAST_ENTRY;

                        //
                        //  If the following block is uncommitted then we only need to
                        //  add this new entry to its free list
                        //

                        if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) {

                            PHEAP_SEGMENT Segment;

                            Segment = Heap->Segments[SplitBlock->SegmentIndex];
                            Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;

                            SplitBlock->Size = (USHORT)FreeSize;

                            RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                            Heap->TotalFreeSize += FreeSize;

                        } else {

                            //
                            //  Otherwise get the next block and check if it is busy.  If it
                            //  is in use then add this new entry to its free list
                            //

                            SplitBlock2 = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize);

                            if (SplitBlock2->Flags & HEAP_ENTRY_BUSY) {

                                SplitBlock->Size = (USHORT)FreeSize;

                                ((PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize))->PreviousSize = (USHORT)FreeSize;

                                RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                                Heap->TotalFreeSize += FreeSize;

                            } else {

                                //
                                //  Otherwise the next block is not in use so we
                                //  should be able to merge with it.  Remove the
                                //  second free block and if the combined size is
                                //  still okay then merge the two blocks and add
                                //  the single block back in.  Otherwise call a
                                //  routine that will actually break it apart
                                //  before insertion.
                                //

                                SplitBlock->Flags = SplitBlock2->Flags;

                                RtlpRemoveFreeBlock( Heap, SplitBlock2 );

                                Heap->TotalFreeSize -= SplitBlock2->Size;

                                FreeSize += SplitBlock2->Size;

                                if (FreeSize <= HEAP_MAXIMUM_BLOCK_SIZE) {

                                    SplitBlock->Size = (USHORT)FreeSize;

                                    if (!(SplitBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                                        ((PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize))->PreviousSize = (USHORT)FreeSize;

                                    } else {

                                        PHEAP_SEGMENT Segment;

                                        Segment = Heap->Segments[SplitBlock->SegmentIndex];
                                        Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;
                                    }

                                    RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                                    Heap->TotalFreeSize += FreeSize;

                                } else {

                                    RtlpInsertFreeBlock( Heap, SplitBlock, FreeSize );
                                }
                            }
                        }
                    }
                }

            } else {

                //
                //  At this point the new size is greater than the current size
                //
                //  If the block is a big allocation or we're not able to grow
                //  the block in place then we have a lot of work to do
                //

                if ((BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) ||
                    !RtlpGrowBlockInPlace( Heap, Flags, BusyBlock, Size, AllocationIndex )) {

                    //
                    //  We're growing the block.  Allocate a new block with the bigger
                    //  size, copy the contents of the old block to the new block and then
                    //  free the old block.  Return the address of the new block.
                    //

                    if (Flags & HEAP_REALLOC_IN_PLACE_ONLY) {

                        // HeapDebugPrint(( "Failing ReAlloc because cant do it inplace.\n" ));

                        BaseAddress = NULL;

                    } else {

                        //
                        //  Clear the tag bits from the flags
                        //

                        Flags &= ~HEAP_TAG_MASK;

                        //
                        //  If there is an extra struct present then get the tag
                        //  index from the extra stuff and augment the flags with
                        //  the tag index.
                        //

                        if (BusyBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                            Flags &= ~HEAP_SETTABLE_USER_FLAGS;

                            Flags |= HEAP_SETTABLE_USER_VALUE |
                                     ((BusyBlock->Flags & HEAP_ENTRY_SETTABLE_FLAGS) << 4);

                            OldExtraStuff = RtlpGetExtraStuffPointer( BusyBlock );

                            try {

                                if ((OldExtraStuff->TagIndex != 0) &&
                                    !(OldExtraStuff->TagIndex & HEAP_PSEUDO_TAG_FLAG)) {

                                    Flags |= OldExtraStuff->TagIndex << HEAP_TAG_SHIFT;
                                }

                            } except (EXCEPTION_EXECUTE_HANDLER) {

                                BusyBlock->Flags &= ~HEAP_ENTRY_EXTRA_PRESENT;
                            }

                        } else if (BusyBlock->SmallTagIndex != 0) {

                            //
                            //  There is not an extra stuff struct, but block
                            //  does have a small tag index so now add this small
                            //  tag to the flags
                            //

                            Flags |= BusyBlock->SmallTagIndex << HEAP_TAG_SHIFT;
                        }

                        //
                        //  Allocate from the heap space for the reallocation
                        //

                        NewBaseAddress = RtlAllocateHeap( HeapHandle,
                                                          Flags & ~HEAP_ZERO_MEMORY,
                                                          Size );

                        if (NewBaseAddress != NULL) {

                            //
                            //  We were able to get the allocation so now back up
                            //  to the heap block and if the block has an extra
                            //  stuff struct then copy over the extra stuff
                            //

                            NewBusyBlock = (PHEAP_ENTRY)NewBaseAddress - 1;

                            if (NewBusyBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                                NewExtraStuff = RtlpGetExtraStuffPointer( NewBusyBlock );

                                if (BusyBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                                    OldExtraStuff = RtlpGetExtraStuffPointer( BusyBlock );

                                    NewExtraStuff->Settable = OldExtraStuff->Settable;

                                } else {

                                    RtlZeroMemory( NewExtraStuff, sizeof( *NewExtraStuff ));
                                }
                            }

                            //
                            //  Copy over the user's data area to the new block
                            //

                            RtlMoveMemory( NewBaseAddress, BaseAddress, Size < OldSize ? Size : OldSize );

                            //
                            //  Check if we grew the block and we should zero
                            //  the remaining part.
                            //
                            //  **** is this first test always true because we're
                            //  **** in the part that grows blocks
                            //

                            if (Size > OldSize && (Flags & HEAP_ZERO_MEMORY)) {

                                RtlZeroMemory( (PCHAR)NewBaseAddress + OldSize,
                                               Size - OldSize );
                            }

                            //
                            //  Release the old block
                            //

                            RtlFreeHeap( HeapHandle,
                                         Flags,
                                         BaseAddress );
                        }

                        BaseAddress = NewBaseAddress;
                    }
                }
            }

            if ((BaseAddress == NULL) && (Flags & HEAP_GENERATE_EXCEPTIONS)) {

                //
                //  Construct an exception record.
                //

                ExceptionRecord.ExceptionCode = STATUS_NO_MEMORY;
                ExceptionRecord.ExceptionRecord = (PEXCEPTION_RECORD)NULL;
                ExceptionRecord.NumberParameters = 1;
                ExceptionRecord.ExceptionFlags = 0;
                ExceptionRecord.ExceptionInformation[ 0 ] = AllocationSize;

                RtlRaiseException( &ExceptionRecord );
            }

        } except( GetExceptionCode() == STATUS_NO_MEMORY ? EXCEPTION_CONTINUE_SEARCH :
                                                           EXCEPTION_EXECUTE_HANDLER ) {

            SET_LAST_STATUS( GetExceptionCode() );
            BaseAddress = NULL;

        }

    } finally {

        //
        //  Unlock the heap
        //

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    //
    //  And return to our caller
    //

    return BaseAddress;
}


//
//  Local Support routine
//

BOOLEAN
RtlpGrowBlockInPlace (
    IN PHEAP Heap,
    IN ULONG Flags,
    IN PHEAP_ENTRY BusyBlock,
    IN SIZE_T Size,
    IN SIZE_T AllocationIndex
    )

/*++

Routine Description:

    This routine will try and grow a heap allocation block at its current
    location

Arguments:

    Heap - Supplies a pointer to the heap being modified

    Flags - Supplies a set of flags to augment those already enforced by
        the heap

    BusyBlock - Supplies a pointer to the block being resized

    Size - Supplies the size, in bytes, needed by the resized block

    AllocationIndex - Supplies the allocation index for the resized block
        Note that the size variable has not been rounded up to the next
        granular block size, but that allocation index has.

Return Value:

    BOOLEAN - TRUE if the block has been resized and FALSE otherwise

--*/

{
    SIZE_T FreeSize;
    SIZE_T OldSize;
    UCHAR EntryFlags, FreeFlags;
    PHEAP_FREE_ENTRY FreeBlock, SplitBlock, SplitBlock2;
    PHEAP_ENTRY_EXTRA OldExtraStuff, NewExtraStuff;

    //
    //  Check if the allocation index is too large for even the nondedicated
    //  free list (i.e., too large for list [0])
    //

    if (AllocationIndex > Heap->VirtualMemoryThreshold) {

        return FALSE;
    }

    //
    //  Get the flags for the current block and a pointer to the next
    //  block following the current block
    //

    EntryFlags = BusyBlock->Flags;

    FreeBlock = (PHEAP_FREE_ENTRY)(BusyBlock + BusyBlock->Size);

    //
    //  If the current block is the last entry before an uncommitted range
    //  we'll try and extend the uncommitted range to fit our new allocation
    //

    if (EntryFlags & HEAP_ENTRY_LAST_ENTRY) {

        //
        //  Calculate how must more we need beyond the current block
        //  size
        //

        FreeSize = (AllocationIndex - BusyBlock->Size) << HEAP_GRANULARITY_SHIFT;
        FreeSize = ROUND_UP_TO_POWER2( FreeSize, PAGE_SIZE );

        //
        //  Try and commit memory at the desired location
        //

        FreeBlock = RtlpFindAndCommitPages( Heap,
                                            Heap->Segments[ BusyBlock->SegmentIndex ],
                                            &FreeSize,
                                            (PHEAP_ENTRY)FreeBlock );

        //
        //  Check if the commit succeeded
        //

        if (FreeBlock == NULL) {

            return FALSE;
        }

        //
        //  New coalesce this newly committed space with whatever is free
        //  around it
        //

        FreeSize = FreeSize >> HEAP_GRANULARITY_SHIFT;

        FreeBlock = RtlpCoalesceFreeBlocks( Heap, FreeBlock, &FreeSize, FALSE );

        FreeFlags = FreeBlock->Flags;

        //
        //  If the newly allocated space plus the current block size is still
        //  not big enough for our resize effort then put this newly
        //  allocated block into the appropriate free list and tell our caller
        //  that a resize wasn't possible
        //

        if ((FreeSize + BusyBlock->Size) < AllocationIndex) {

            RtlpInsertFreeBlock( Heap, FreeBlock, FreeSize );

            Heap->TotalFreeSize += FreeSize;

#ifndef NTOS_KERNEL_RUNTIME
            if (DEBUG_HEAP(Flags)) {

                RtlpValidateHeapHeaders( Heap, TRUE );
            }
#endif

            return FALSE;
        }

        //
        //  We were able to generate enough space for the resize effort, so
        //  now free size will be the index for the current block plus the
        //  new free space
        //

        FreeSize += BusyBlock->Size;

    } else {

        //
        //  The following block is present so grab its flags and see if
        //  it is free or busy.  If busy then we cannot grow the current
        //  block
        //

        FreeFlags = FreeBlock->Flags;

        if (FreeFlags & HEAP_ENTRY_BUSY) {

            return FALSE;
        }

        //
        //  Compute the index if we combine current block with its following
        //  free block and check if it is big enough
        //

        FreeSize = BusyBlock->Size + FreeBlock->Size;

        if (FreeSize < AllocationIndex) {

            return FALSE;
        }

        //
        //  The two blocks together are big enough so now remove the free
        //  block from its free list, and update the heap's total free size
        //

        RtlpRemoveFreeBlock( Heap, FreeBlock );

        Heap->TotalFreeSize -= FreeBlock->Size;
    }

    //
    //  At this point we have a busy block followed by a free block that
    //  together have enough space for the resize.  The free block has been
    //  removed from its list and free size is the index of the two combined
    //  blocks.
    //
    //  Calculate the number of bytes in use in the old block
    //

    OldSize = (BusyBlock->Size << HEAP_GRANULARITY_SHIFT) - BusyBlock->UnusedBytes;

    //
    //  Calculate the index for whatever excess we'll have when we combine
    //  the two blocks
    //

    FreeSize -= AllocationIndex;

    //
    //  If the excess is not too much then put it back in our allocation
    //  (i.e., we don't want small free pieces left over)
    //

    if (FreeSize <= 2) {

        AllocationIndex += FreeSize;

        FreeSize = 0;
    }

    //
    //  If the busy block has an extra stuff struct present then copy over the
    //  extra stuff
    //

    if (EntryFlags & HEAP_ENTRY_EXTRA_PRESENT) {

        OldExtraStuff = (PHEAP_ENTRY_EXTRA)(BusyBlock + BusyBlock->Size - 1);
        NewExtraStuff = (PHEAP_ENTRY_EXTRA)(BusyBlock + AllocationIndex - 1);

        *NewExtraStuff = *OldExtraStuff;

        //
        //  If heap tagging is enabled then update the heap tag from the extra
        //  stuff struct
        //

#ifndef NTOS_KERNEL_RUNTIME
        if (IS_HEAP_TAGGING_ENABLED()) {

            NewExtraStuff->TagIndex =
                RtlpUpdateTagEntry( Heap,
                                    NewExtraStuff->TagIndex,
                                    BusyBlock->Size,
                                    AllocationIndex,
                                    ReAllocationAction );
        }
#endif

    //
    //  Otherwise extra stuff is not in use so see if heap tagging is enabled
    //  and if so then update small tag index
    //

    }
#ifndef NTOS_KERNEL_RUNTIME
         else if (IS_HEAP_TAGGING_ENABLED()) {

        BusyBlock->SmallTagIndex = (UCHAR)
            RtlpUpdateTagEntry( Heap,
                                BusyBlock->SmallTagIndex,
                                BusyBlock->Size,
                                AllocationIndex,
                                ReAllocationAction );
    }
#endif

    //
    //  Check if we will have any free space to give back.
    //

    if (FreeSize == 0) {

        //
        //  No following free space so update the flags, size and byte counts
        //  for the resized block.  If the free block was a last entry
        //  then the busy block must also now be a last entry.
        //

        BusyBlock->Flags |= FreeFlags & HEAP_ENTRY_LAST_ENTRY;

        BusyBlock->Size = (USHORT)AllocationIndex;

        BusyBlock->UnusedBytes = (UCHAR)
            ((AllocationIndex << HEAP_GRANULARITY_SHIFT) - Size);

        //
        //  Update the previous size field of the following block if it exists
        //

        if (!(FreeFlags & HEAP_ENTRY_LAST_ENTRY)) {

            (BusyBlock + BusyBlock->Size)->PreviousSize = BusyBlock->Size;

        } else {

            PHEAP_SEGMENT Segment;

            Segment = Heap->Segments[BusyBlock->SegmentIndex];
            Segment->LastEntryInSegment = BusyBlock;
        }

    //
    //  Otherwise there is some free space to return to the heap
    //

    } else {

        //
        //  Update the size and byte counts for the resized block.
        //

        BusyBlock->Size = (USHORT)AllocationIndex;

        BusyBlock->UnusedBytes = (UCHAR)
            ((AllocationIndex << HEAP_GRANULARITY_SHIFT) - Size);

        //
        //  Determine where the new free block starts and fill in its fields
        //

        SplitBlock = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)BusyBlock + AllocationIndex);

        SplitBlock->PreviousSize = (USHORT)AllocationIndex;

        SplitBlock->SegmentIndex = BusyBlock->SegmentIndex;

        //
        //  If this new free block will be the last entry then update its
        //  flags and size and put it into the appropriate free list
        //

        if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) {

            PHEAP_SEGMENT Segment;

            Segment = Heap->Segments[SplitBlock->SegmentIndex];
            Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;

            SplitBlock->Flags = FreeFlags;
            SplitBlock->Size = (USHORT)FreeSize;

            RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

            Heap->TotalFreeSize += FreeSize;

        //
        //  The free block is followed by another valid block
        //

        } else {

            //
            //  Point to the block following our new free block
            //

            SplitBlock2 = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)SplitBlock + FreeSize);

            //
            //  If the block following the new free block is busy then
            //  update the flags and size for the new free block, update
            //  the following blocks previous size, and put the free block
            //  into the appropriate free list
            //

            if (SplitBlock2->Flags & HEAP_ENTRY_BUSY) {

                SplitBlock->Flags = FreeFlags & (~HEAP_ENTRY_LAST_ENTRY);
                SplitBlock->Size = (USHORT)FreeSize;

                //
                //  **** note that this test must be true because we are
                //  **** already in the else clause of the
                //  **** if (FreeFlags & HEAP_ENTRY_LAST_ENTRY) statement
                //

                if (!(FreeFlags & HEAP_ENTRY_LAST_ENTRY)) {

                    ((PHEAP_ENTRY)SplitBlock + FreeSize)->PreviousSize = (USHORT)FreeSize;

                } else {

                    PHEAP_SEGMENT Segment;

                    Segment = Heap->Segments[SplitBlock->SegmentIndex];
                    Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;
                }

                RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                Heap->TotalFreeSize += FreeSize;

            //
            //  Otherwise the following block is also free so we can combine
            //  these two blocks
            //

            } else {

                //
                //  Remember the new free flags from the following block
                //

                FreeFlags = SplitBlock2->Flags;

                //
                //  Remove the following block from its free list
                //

                RtlpRemoveFreeBlock( Heap, SplitBlock2 );

                Heap->TotalFreeSize -= SplitBlock2->Size;

                //
                //  Calculate the size of the new combined free block
                //

                FreeSize += SplitBlock2->Size;

                //
                //  Give the new the its new flags
                //

                SplitBlock->Flags = FreeFlags;

                //
                //  If the combited block is not too large for the dedicated
                //  free lists then that where we'll put it
                //

                if (FreeSize <= HEAP_MAXIMUM_BLOCK_SIZE) {

                    SplitBlock->Size = (USHORT)FreeSize;

                    //
                    //  If present update the previous size for the following block
                    //

                    if (!(FreeFlags & HEAP_ENTRY_LAST_ENTRY)) {

                        ((PHEAP_ENTRY)SplitBlock + FreeSize)->PreviousSize = (USHORT)FreeSize;

                    } else {

                        PHEAP_SEGMENT Segment;

                        Segment = Heap->Segments[SplitBlock->SegmentIndex];
                        Segment->LastEntryInSegment = (PHEAP_ENTRY)SplitBlock;
                    }

                    //
                    //  Insert the new combined free block into the free list
                    //

                    RtlpInsertFreeBlockDirect( Heap, SplitBlock, (USHORT)FreeSize );

                    Heap->TotalFreeSize += FreeSize;

                } else {

                    //
                    //  Otherwise the new free block is too large to go into
                    //  a dedicated free list so put it in the general free list
                    //  which might involve breaking it apart.
                    //

                    RtlpInsertFreeBlock( Heap, SplitBlock, FreeSize );
                }
            }
        }
    }

    //
    //  At this point the block has been resized and any extra space has been
    //  returned to the free list
    //
    //  Check if we should zero out the new space
    //

    if (Flags & HEAP_ZERO_MEMORY) {

        //
        //  **** this test is sort of bogus because we're resizing and the new
        //  **** size by definition must be larger than the old size
        //

        if (Size > OldSize) {

            RtlZeroMemory( (PCHAR)(BusyBlock + 1) + OldSize,
                           Size - OldSize );
        }

    //
    //  Check if we should be filling in heap after it as
    //  been freed, and if so then fill in the newly allocated
    //  space beyond the old bytes.
    //

    }
#if DBG
    else if (Heap->Flags & HEAP_FREE_CHECKING_ENABLED) {

        SIZE_T PartialBytes, ExtraSize;

        PartialBytes = OldSize & (sizeof( ULONG ) - 1);

        if (PartialBytes) {

            PartialBytes = 4 - PartialBytes;
        }

        if (Size > (OldSize + PartialBytes)) {

            ExtraSize = (Size - (OldSize + PartialBytes)) & ~(sizeof( ULONG ) - 1);

            if (ExtraSize != 0) {

                RtlFillMemoryUlong( (PCHAR)(BusyBlock + 1) + OldSize + PartialBytes,
                                    ExtraSize,
                                    ALLOC_HEAP_FILL );
            }
        }
    }

    //
    //  If we are going tailing checking then fill in the space right beyond
    //  the new allocation
    //

    if (Heap->Flags & HEAP_TAIL_CHECKING_ENABLED) {

        RtlFillMemory( (PCHAR)(BusyBlock + 1) + Size,
                       CHECK_HEAP_TAIL_SIZE,
                       CHECK_HEAP_TAIL_FILL );
    }
#endif // DBG

    //
    //  Give the resized block any user settable flags send in by the
    //  caller
    //

    BusyBlock->Flags &= ~HEAP_ENTRY_SETTABLE_FLAGS;
    BusyBlock->Flags |= ((Flags & HEAP_SETTABLE_USER_FLAGS) >> 4);

    //
    //  And return to our caller
    //

    return TRUE;
}



BOOLEAN
RtlFreeHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    )

/*++

Routine Description:

    This routine returns a previously allocated block back to its heap

Arguments:

    HeapHandle - Supplies a pointer to the owning heap structure

    Flags - Specifies the set of flags to use in the deallocation

    BaseAddress - Supplies a pointer to the block being freed

Return Value:

    BOOLEAN - TRUE if the block was properly freed and FALSE otherwise

--*/

{
    NTSTATUS Status;
    PHEAP Heap = (PHEAP)HeapHandle;
    PHEAP_ENTRY BusyBlock;
    PHEAP_ENTRY_EXTRA ExtraStuff;
    SIZE_T FreeSize;
    BOOLEAN LockAcquired = FALSE;
    BOOLEAN ReturnValue = TRUE;

    RTL_PAGED_CODE();

    //
    //  First check if the address we're given is null and if so then
    //  there is really nothing to do so just return success
    //

    if (BaseAddress == NULL) {

        return TRUE;
    }

#ifndef NTOS_KERNEL_RUNTIME
#ifdef NTHEAP_ENABLED
    {
        if (Heap->Flags & NTHEAP_ENABLED_FLAG) {

            return RtlFreeNtHeap( HeapHandle,
                                  Flags,
                                  BaseAddress);
        }
    }
#endif // NTHEAP_ENABLED
#endif // NTOS_KERNEL_RUNTIME


    //
    //  Compliment the input flags with those enforced by the heap
    //

    Flags |= Heap->ForceFlags;

    //
    //  Now check if we should go the slow route
    //

#if DBG
    if (Flags & HEAP_SLOW_FLAGS) {

        return RtlFreeHeapSlowly(HeapHandle, Flags, BaseAddress);
    }
#endif // DBG

    //
    //  We can do everything in this routine. So now backup to get
    //  a pointer to the start of the block
    //

    BusyBlock = (PHEAP_ENTRY)BaseAddress - 1;

#if DBG
    //
    //  Protect ourselves from idiots by refusing to free blocks
    //  that do not have the busy bit set.
    //
    //  Also refuse to free blocks that are not eight-byte aligned.
    //  The specific idiot in this case is Office95, which likes
    //  to free a random pointer when you start Word95 from a desktop
    //  shortcut.
    //
    //  As further insurance against idiots, check the segment index
    //  to make sure it is less than HEAP_MAXIMUM_SEGMENTS (16). This
    //  should fix all the dorks who have ASCII or Unicode where the
    //  heap header is supposed to be.
    //

    try {
        if ((!(BusyBlock->Flags & HEAP_ENTRY_BUSY)) ||
            (((ULONG_PTR)BaseAddress & 0x7) != 0) ||
            (BusyBlock->SegmentIndex >= HEAP_MAXIMUM_SEGMENTS)) {

            //
            //  Not a busy block, or it's not aligned or the segment is
            //  to big, meaning it's corrupt
            //

            ASSERTMSG("Bad pointer in RtlFreeHeap()\n", 0);
        }
    } except(EXCEPTION_EXECUTE_HANDLER) {

        ASSERTMSG("Bad pointer in RtlFreeHeap()\n", 0);
    }
#endif // DBG

    //
    //  If there is a lookaside list and the block is not a big allocation
    //  and the index is for a dedicated list then free the block to the
    //  lookaside list.  We'll actually capture
    //  the lookaside pointer from the heap and only use the captured pointer.
    //  This will take care of the condition where a walk or lock heap can
    //  cause us to check for a non null pointer and then have it become null
    //  when we read it again.  If it is non null to start with then even if
    //  the user walks or locks the heap via another thread the pointer to
    //  still valid here so we can still try and do a lookaside list push
    //

#ifndef NTOS_KERNEL_RUNTIME

    {
        PHEAP_LOOKASIDE Lookaside = (PHEAP_LOOKASIDE)Heap->Lookaside;

        if ((Lookaside != NULL) &&
            (Heap->LookasideLockCount == 0) &&
            (!(BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC)) &&
            ((FreeSize = BusyBlock->Size) < HEAP_MAXIMUM_FREELISTS)) {

            if (RtlpFreeToHeapLookaside( &Lookaside[FreeSize], BaseAddress)) {

                return TRUE;
            }
        }
    }

#endif // NTOS_KERNEL_RUNTIME

    try {

        //
        //  Check if we need to lock the heap
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;
        }

        //
        //  Check if this is not a virtual block allocation meaning
        //  that we it is part of the heap free list structure and not
        //  one huge allocation that we got from vm
        //

        if (!(BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC)) {

            //
            //  This block is not a big allocation so we need to
            //  to get its size, and coalesce the blocks note that
            //  the user mode heap does this conditionally on a heap
            //  flag.  The coalesce function returns the newly formed
            //  free block and the new size.
            //

            FreeSize = BusyBlock->Size;

    #ifdef NTOS_KERNEL_RUNTIME

            BusyBlock = (PHEAP_ENTRY)RtlpCoalesceFreeBlocks( Heap,
                                                             (PHEAP_FREE_ENTRY)BusyBlock,
                                                             &FreeSize,
                                                             FALSE );

    #else // NTOS_KERNEL_RUNTIME

            if (!(Heap->Flags & HEAP_DISABLE_COALESCE_ON_FREE)) {

                BusyBlock = (PHEAP_ENTRY)RtlpCoalesceFreeBlocks( Heap,
                                                                 (PHEAP_FREE_ENTRY)BusyBlock,
                                                                 &FreeSize,
                                                                 FALSE );
            }

    #endif // NTOS_KERNEL_RUNTIME

            //
            //  Check for a small allocation that can go on a freelist
            //  first, these should never trigger a decommit.
            //

            HEAPASSERT(HEAP_MAXIMUM_FREELISTS < Heap->DeCommitFreeBlockThreshold);

            //
            //  If the allocation fits on a free list then insert it on
            //  the appropriate free list.  If the block is not the last
            //  entry then make sure that the next block knows our correct
            //  size, and update the heap free space counter.
            //

            if (FreeSize < HEAP_MAXIMUM_FREELISTS) {

                RtlpFastInsertDedicatedFreeBlockDirect( Heap,
                                                        (PHEAP_FREE_ENTRY)BusyBlock,
                                                        (USHORT)FreeSize );

                if (!(BusyBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                    HEAPASSERT((BusyBlock + FreeSize)->PreviousSize == (USHORT)FreeSize);
                }

                Heap->TotalFreeSize += FreeSize;

            //
            //  Otherwise the block is to big for one of the dedicated free list so
            //  see if the free size is under the decommit threshold by itself
            //  or the total free in the heap is under the decomit threshold then
            //  we'll put this into a free list
            //

            } else if ((FreeSize < Heap->DeCommitFreeBlockThreshold) ||
                       ((Heap->TotalFreeSize + FreeSize) < Heap->DeCommitTotalFreeThreshold)) {

                //
                //  Check if the block can go into the [0] index free list, and if
                //  so then do the insert and make sure the following block is
                //  needed knows our correct size, and update the heaps free space
                //  counter
                //

                if (FreeSize <= (ULONG)HEAP_MAXIMUM_BLOCK_SIZE) {

                    RtlpFastInsertNonDedicatedFreeBlockDirect( Heap,
                                                               (PHEAP_FREE_ENTRY)BusyBlock,
                                                               (USHORT)FreeSize );

                    if (!(BusyBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                        HEAPASSERT((BusyBlock + FreeSize)->PreviousSize == (USHORT)FreeSize);
                    }

                    Heap->TotalFreeSize += FreeSize;

                } else {

                    //
                    //  The block is too big to go on a free list in its
                    //  entirety but we don't want to decommit anything so
                    //  simply call a worker routine to hack up the block
                    //  into pieces that will fit on the free lists.
                    //

                    RtlpInsertFreeBlock( Heap, (PHEAP_FREE_ENTRY)BusyBlock, FreeSize );
                }

            //
            //  Otherwise the block is to big for any lists and we should decommit
            //  the block
            //

            } else {

                RtlpDeCommitFreeBlock( Heap, (PHEAP_FREE_ENTRY)BusyBlock, FreeSize );
            }

        } else {

            //
            //  This is a big virtual block allocation.  To free it we only have to
            //  remove it from the heaps list of virtual allocated blocks, unlock
            //  the heap, and return the block to vm
            //

            PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

            VirtualAllocBlock = CONTAINING_RECORD( BusyBlock, HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

            RemoveEntryList( &VirtualAllocBlock->Entry );

            //
            //  Release lock here as there is no reason to hold it across
            //  the system call.
            //

            if (LockAcquired) {

                RtlReleaseLockRoutine( Heap->LockVariable );
                LockAcquired = FALSE;
            }

            FreeSize = 0;

            Status = ZwFreeVirtualMemory( (PVOID *)&VirtualAllocBlock,
                                          &FreeSize,
                                          MEM_RELEASE );


            //
            //  Check if we had trouble freeing the block back to vm
            //  and return an error if necessary
            //

            if (!NT_SUCCESS( Status )) {

                SET_LAST_STATUS( Status );

                ReturnValue = FALSE;
            }
        }

    } finally {

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    //
    //  The block was freed successfully so return success to our
    //  caller
    //

    return ReturnValue;
}


BOOLEAN
RtlFreeHeapSlowly (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    )

/*++

Routine Description:

    This routine returns a previously allocated block back to its heap.
    It is the slower version of Rtl Free Heap and does more checking and
    tagging control.

Arguments:

    HeapHandle - Supplies a pointer to the owning heap structure

    Flags - Specifies the set of flags to use in the deallocation

    BaseAddress - Supplies a pointer to the block being freed

Return Value:

    BOOLEAN - TRUE if the block was properly freed and FALSE otherwise

--*/

{
    NTSTATUS Status;
    PHEAP Heap = (PHEAP)HeapHandle;
    PHEAP_ENTRY BusyBlock;
    PHEAP_ENTRY_EXTRA ExtraStuff;
    SIZE_T FreeSize;
    BOOLEAN Result;
    BOOLEAN LockAcquired = FALSE;

#ifndef NTOS_KERNEL_RUNTIME

    USHORT TagIndex;

#endif // NTOS_KERNEL_RUNTIME

    RTL_PAGED_CODE();

    //
    //  Note that Flags has already been OR'd with Heap->ForceFlags.
    //

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case see if we should be calling the debug version to
    //  free the heap
    //

    if (DEBUG_HEAP( Flags )) {

        return RtlDebugFreeHeap( HeapHandle, Flags, BaseAddress );
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  Until we figure out otherwise we'll assume that this call will fail
    //

    Result = FALSE;

    try {

        //
        //  Lock the heap
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;
        }

        try {

            //
            //  Backup to get a pointer to the start of the block
            //

            BusyBlock = (PHEAP_ENTRY)BaseAddress - 1;

            //
            //  Protect ourselves from idiots by refusing to free blocks
            //  that do not have the busy bit set.
            //
            //  Also refuse to free blocks that are not eight-byte aligned.
            //  The specific idiot in this case is Office95, which likes
            //  to free a random pointer when you start Word95 from a desktop
            //  shortcut.
            //
            //  As further insurance against idiots, check the segment index
            //  to make sure it is less than HEAP_MAXIMUM_SEGMENTS (16). This
            //  should fix all the dorks who have ASCII or Unicode where the
            //  heap header is supposed to be.
            //
            //  Note that this test is just opposite from the test used in
            //  Rtl Free Heap
            //

            if ((BusyBlock->Flags & HEAP_ENTRY_BUSY) &&
                (((ULONG_PTR)BaseAddress & 0x7) == 0) &&
                (BusyBlock->SegmentIndex < HEAP_MAXIMUM_SEGMENTS)) {

                //
                //  Check if this is a virtual block allocation
                //

                if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

                    PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

                    //
                    //  This is a big virtual block allocation.  To free it
                    //  we only have to remove it from the heaps list of
                    //  virtual allocated blocks, unlock the heap, and return
                    //  the block to vm
                    //

                    VirtualAllocBlock = CONTAINING_RECORD( BusyBlock, HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

                    RemoveEntryList( &VirtualAllocBlock->Entry );

    #ifndef NTOS_KERNEL_RUNTIME

                    //
                    //  In the non kernel case see if we need to free the tag
                    //

                    if (IS_HEAP_TAGGING_ENABLED()) {

                        RtlpUpdateTagEntry( Heap,
                                            VirtualAllocBlock->ExtraStuff.TagIndex,
                                            VirtualAllocBlock->CommitSize >> HEAP_GRANULARITY_SHIFT,
                                            0,
                                            VirtualFreeAction );
                    }

    #endif // NTOS_KERNEL_RUNTIME

                    FreeSize = 0;

                    Status = ZwFreeVirtualMemory( (PVOID *)&VirtualAllocBlock,
                                                  &FreeSize,
                                                  MEM_RELEASE );

                    //
                    //  Check if everything worked okay, if we had trouble freeing
                    //  the block back to vm return an error if necessary,
                    //

                    if (NT_SUCCESS( Status )) {

                        Result = TRUE;

                    } else {

                        SET_LAST_STATUS( Status );
                    }

                } else {

                    //
                    //  This block is not a big allocation so we need to
                    //  to get its size, and coalesce the blocks note that
                    //  the user mode heap does this conditionally on a heap
                    //  flag.  The coalesce function returns the newly formed
                    //  free block and the new size.
                    //

    #ifndef NTOS_KERNEL_RUNTIME

                    //
                    //  First in the non kernel case remove any tagging we might
                    //  have been using.  Note that the will either be in
                    //  the heap header, or in the extra block if present
                    //

                    if (IS_HEAP_TAGGING_ENABLED()) {

                        if (BusyBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                            ExtraStuff = (PHEAP_ENTRY_EXTRA)(BusyBlock + BusyBlock->Size - 1);

                            TagIndex = RtlpUpdateTagEntry( Heap,
                                                           ExtraStuff->TagIndex,
                                                           BusyBlock->Size,
                                                           0,
                                                           FreeAction );

                        } else {

                            TagIndex = RtlpUpdateTagEntry( Heap,
                                                           BusyBlock->SmallTagIndex,
                                                           BusyBlock->Size,
                                                           0,
                                                           FreeAction );
                        }

                    } else {

                        TagIndex = 0;
                    }

    #endif // NTOS_KERNEL_RUNTIME

                    //
                    //  This is the size of the block we are freeing
                    //

                    FreeSize = BusyBlock->Size;

    #ifndef NTOS_KERNEL_RUNTIME

                    //
                    //  In the non kernel case see if we should coalesce on free
                    //

                    if (!(Heap->Flags & HEAP_DISABLE_COALESCE_ON_FREE)) {

    #endif // NTOS_KERNEL_RUNTIME

                        //
                        //  In kernel case and in the tested user mode case we
                        //  now coalesce free blocks
                        //

                        BusyBlock = (PHEAP_ENTRY)RtlpCoalesceFreeBlocks( Heap, (PHEAP_FREE_ENTRY)BusyBlock, &FreeSize, FALSE );

    #ifndef NTOS_KERNEL_RUNTIME

                    }

    #endif // NTOS_KERNEL_RUNTIME

                    //
                    //  If the block should not be decommit then try and put it
                    //  on a free list
                    //

                    if ((FreeSize < Heap->DeCommitFreeBlockThreshold) ||
                        ((Heap->TotalFreeSize + FreeSize) < Heap->DeCommitTotalFreeThreshold)) {

                        //
                        //  Check if the block can fit on one of the dedicated free
                        //  lists
                        //

                        if (FreeSize <= (ULONG)HEAP_MAXIMUM_BLOCK_SIZE) {

                            //
                            //  It can fit on a dedicated free list so insert it on
                            //

                            RtlpInsertFreeBlockDirect( Heap, (PHEAP_FREE_ENTRY)BusyBlock, (USHORT)FreeSize );

                            //
                            //  If there is a following entry then make sure the
                            //  sizes agree
                            //

                            if (!(BusyBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                                HEAPASSERT((BusyBlock + FreeSize)->PreviousSize == (USHORT)FreeSize);
                            }

                            //
                            //  Update the heap with the amount of free space
                            //  available
                            //

                            Heap->TotalFreeSize += FreeSize;

                        } else {

                            //
                            //  The block goes on the non dedicated free list
                            //

                            RtlpInsertFreeBlock( Heap, (PHEAP_FREE_ENTRY)BusyBlock, FreeSize );
                        }

    #ifndef NTOS_KERNEL_RUNTIME

                        //
                        //  In the non kernel case see if the there was tag and if
                        //  so then update the entry to show that it's been freed
                        //

                        if (TagIndex != 0) {

                            PHEAP_FREE_ENTRY_EXTRA FreeExtra;

                            BusyBlock->Flags |= HEAP_ENTRY_EXTRA_PRESENT;

                            FreeExtra = (PHEAP_FREE_ENTRY_EXTRA)(BusyBlock + BusyBlock->Size) - 1;

                            FreeExtra->TagIndex = TagIndex;
                            FreeExtra->FreeBackTraceIndex = 0;

    #if i386 && DBG

                            //
                            //  In the x86 case we can also capture the stack
                            //  backtrace
                            //

                            if (Heap->Flags & HEAP_CAPTURE_STACK_BACKTRACES) {

                                FreeExtra->FreeBackTraceIndex = (USHORT)RtlLogStackBackTrace();
                            }

    #endif // i386 && DBG

                        }

    #endif // NTOS_KERNEL_RUNTIME

                    } else {

                        //
                        //  Otherwise the block is big enough to decommit so have a
                        //  worker routine to do the decommit
                        //

                        RtlpDeCommitFreeBlock( Heap, (PHEAP_FREE_ENTRY)BusyBlock, FreeSize );
                    }

                    //
                    //  And say the free worked fine
                    //

                    Result = TRUE;
                }

            } else {

                //
                //  Not a busy block, or it's not aligned or the segment is
                //  to big, meaning it's corrupt
                //

                SET_LAST_STATUS( STATUS_INVALID_PARAMETER );
            }

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            SET_LAST_STATUS( GetExceptionCode() );

            Result = FALSE;
        }

    } finally {

        //
        //  Unlock the heap
        //

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    //
    //  And return to our caller
    //

    return Result;
}


SIZE_T
RtlSizeHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    )

/*++

Routine Description:

    This routine returns the size, in bytes, of the indicated block
    of heap storage.  The size only includes the number of bytes the
    original caller used to allocate the block and not any unused
    bytes at the end of the block.

Arguments:

    HeapHandle - Supplies a pointer to the heap that owns the block
        being queried

    Flags - Supplies a set of flags used to allocate the block

    BaseAddress - Supplies the address of the block being queried

Return Value:

    SIZE_T - returns the size, in bytes, of the queried block, or -1
        if the block is not in use.

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    PHEAP_ENTRY BusyBlock;
    SIZE_T BusySize;

    //
    //  Compliment the input flags with those enforced by the heap
    //

    Flags |= Heap->ForceFlags;

    //
    //  Check if this is the nonkernel debug version of heap
    //

#ifndef NTOS_KERNEL_RUNTIME

    if (DEBUG_HEAP( Flags )) {

        return RtlDebugSizeHeap( HeapHandle, Flags, BaseAddress );
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  No lock is required since nothing is modified and nothing
    //  outside the busy block is read.  Backup to get a pointer
    //  to the heap entry
    //

    BusyBlock = (PHEAP_ENTRY)BaseAddress - 1;

    //
    //  If the block is not in use then the answer is -1 and
    //  we'll set the error status for the user mode thread
    //

    if (!(BusyBlock->Flags & HEAP_ENTRY_BUSY)) {

        BusySize = -1;

        SET_LAST_STATUS( STATUS_INVALID_PARAMETER );

    //
    //  Otherwise if the block is from our large allocation then
    //  we'll get the result from that routine
    //

    } else if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

        BusySize = RtlpGetSizeOfBigBlock( BusyBlock );

    //
    //  Otherwise the block must be one that we can handle so
    //  calculate its block size and then subtract what's not being
    //  used by the caller.
    //
    //  **** this seems to include the heap entry header in its
    //  **** calculation.  Is that what we really want?
    //

    } else {

        BusySize = (BusyBlock->Size << HEAP_GRANULARITY_SHIFT) -
                   BusyBlock->UnusedBytes;
    }

    //
    //  And return to our caller
    //

    return BusySize;
}


NTSTATUS
RtlZeroHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags
    )

/*++

Routine Description:

    This routine zero's (or fills) in all the free blocks in a heap.
    It does not touch big allocations.

Arguments:

    HeapHandle - Supplies a pointer to the heap being zeroed

    Flags - Supplies a set of heap flags to compliment those already
        set in the heap

Return Value:

    NTSTATUS - An appropriate status code

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    NTSTATUS Status;
    BOOLEAN LockAcquired = FALSE;
    PHEAP_SEGMENT Segment;
    ULONG SegmentIndex;
    PHEAP_ENTRY CurrentBlock;
    PHEAP_FREE_ENTRY FreeBlock;
    SIZE_T Size;
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange;

    RTL_PAGED_CODE();

    //
    //  Compliment the input flags with those enforced by the heap
    //

    Flags |= Heap->ForceFlags;

    //
    //  Check if this is the nonkernel debug version of heap
    //

#ifndef NTOS_KERNEL_RUNTIME

    if (DEBUG_HEAP( Flags )) {

        return RtlDebugZeroHeap( HeapHandle, Flags );
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  Unless something happens otherwise we'll assume that we'll
    //  be successful
    //

    Status = STATUS_SUCCESS;

    try {

        //
        //  Lock the heap
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;
        }

        try {

            //
            //  Zero fill all the free blocks in all the segements
            //

            for (SegmentIndex=0; SegmentIndex<HEAP_MAXIMUM_SEGMENTS; SegmentIndex++) {

                Segment = Heap->Segments[ SegmentIndex ];

                if (!Segment) {

                    continue;
                }

                UnCommittedRange = Segment->UnCommittedRanges;
                CurrentBlock = Segment->FirstEntry;

                //
                //  With the current segment we'll zoom through the
                //  blocks until we reach the end
                //

                while (CurrentBlock < Segment->LastValidEntry) {

                    Size = CurrentBlock->Size << HEAP_GRANULARITY_SHIFT;

                    //
                    //  If the block is not in use then we'll either zero
                    //  it or fill it.
                    //

                    if (!(CurrentBlock->Flags & HEAP_ENTRY_BUSY)) {

                        FreeBlock = (PHEAP_FREE_ENTRY)CurrentBlock;

#if DBG
                        if ((Heap->Flags & HEAP_FREE_CHECKING_ENABLED) &&
                            (CurrentBlock->Flags & HEAP_ENTRY_FILL_PATTERN)) {

                            RtlFillMemoryUlong( FreeBlock + 1,
                                                Size - sizeof( *FreeBlock ),
                                                FREE_HEAP_FILL );

                        } else
#endif // DBG
                                {

                            RtlFillMemoryUlong( FreeBlock + 1,
                                                Size - sizeof( *FreeBlock ),
                                                0 );
                        }
                    }

                    //
                    //  If the following entry is uncommited then we need to
                    //  skip over it.  This code strongly implies that the
                    //  uncommitted range list is in perfect sync with the
                    //  blocks in the segement
                    //

                    if (CurrentBlock->Flags & HEAP_ENTRY_LAST_ENTRY) {

                        CurrentBlock += CurrentBlock->Size;

                        //
                        //  Check if the we've reached the end of the segment
                        //  and should just break out of the while loop
                        //
                        //
                        //  **** "break;" would probably be more clear here
                        //

                        if (UnCommittedRange == NULL) {

                            CurrentBlock = Segment->LastValidEntry;

                        //
                        //  Otherwise skip over the uncommitted range
                        //

                        } else {

                            CurrentBlock = (PHEAP_ENTRY)
                                ((PCHAR)UnCommittedRange->Address + UnCommittedRange->Size);

                            UnCommittedRange = UnCommittedRange->Next;
                        }

                    //
                    //  Otherwise the next block exists so advance to it
                    //

                    } else {

                        CurrentBlock += CurrentBlock->Size;
                    }
                }
            }

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            Status = GetExceptionCode();
        }

    } finally {

        //
        //  Unlock the heap
        //

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    return Status;
}


//
//  Local Support Routine
//

PHEAP_UNCOMMMTTED_RANGE
RtlpCreateUnCommittedRange (
    IN PHEAP_SEGMENT Segment
    )

/*++

Routine Description:

    This routine add a new uncommitted range structure to the specified heap
    segment.  This routine works by essentially doing a pop of the stack of
    unused uncommitted range structures located off the heap structure.  If
    the stack is empty then we'll create some more before doing the pop.

Arguments:

    Segment - Supplies the heap segment being modified

Return Value:

    PHEAP_UNCOMMITTED_RANGE - returns a pointer to the newly created
        uncommitted range structure

--*/

{
    NTSTATUS Status;
    PVOID FirstEntry, LastEntry;
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange, *pp;
    SIZE_T ReserveSize, CommitSize;
    PHEAP_UCR_SEGMENT UCRSegment;

    RTL_PAGED_CODE();

    //
    //  Get a pointer to the unused uncommitted range structures for
    //  the specified heap
    //

    pp = &Segment->Heap->UnusedUnCommittedRanges;

    //
    //  If the list is null then we need to allocate some more to
    //  put on the list
    //

    if (*pp == NULL) {

        //
        //  Get the next uncommitted range segment from the heap
        //

        UCRSegment = Segment->Heap->UCRSegments;

        //
        //  If there are no more uncommitted range segments or
        //  the segemtns commited and reserved sizes are equal (meaning
        //  it's all used up) then we need to allocate another uncommitted
        //  range segment
        //

        if ((UCRSegment == NULL) ||
            (UCRSegment->CommittedSize == UCRSegment->ReservedSize)) {

            //
            //  We'll reserve 16 pages of memory and commit at this
            //  time one page of it.
            //

            ReserveSize = PAGE_SIZE * 16;
            UCRSegment = NULL;

            Status = ZwAllocateVirtualMemory( &UCRSegment,
                                              0,
                                              &ReserveSize,
                                              MEM_RESERVE,
                                              PAGE_READWRITE );

            if (!NT_SUCCESS( Status )) {

                return NULL;
            }

            CommitSize = PAGE_SIZE;

            Status = ZwAllocateVirtualMemory( &UCRSegment,
                                              0,
                                              &CommitSize,
                                              MEM_COMMIT,
                                              PAGE_READWRITE );

            if (!NT_SUCCESS( Status )) {

                ZwFreeVirtualMemory( &UCRSegment,
                                     &ReserveSize,
                                     MEM_RELEASE );

                return NULL;
            }

            //
            //  Add this new segment to the front of the UCR segments
            //

            UCRSegment->Next = Segment->Heap->UCRSegments;
            Segment->Heap->UCRSegments = UCRSegment;

            //
            //  Set the segments commit and reserve size
            //

            UCRSegment->ReservedSize = ReserveSize;
            UCRSegment->CommittedSize = CommitSize;

            //
            //  Point to the first free spot in the segment
            //

            FirstEntry = (PCHAR)(UCRSegment + 1);

        } else {

            //
            //  We have an existing UCR segment with available space
            //  So now try and commit another PAGE_SIZE bytes.  When we are done
            //  FirstEntry will point to the newly committed space
            //

            CommitSize = PAGE_SIZE;
            FirstEntry = (PCHAR)UCRSegment + UCRSegment->CommittedSize;

            Status = ZwAllocateVirtualMemory( &FirstEntry,
                                              0,
                                              &CommitSize,
                                              MEM_COMMIT,
                                              PAGE_READWRITE );

            if (!NT_SUCCESS( Status )) {

                return NULL;
            }

            //
            //  And update the amount committed in the segment
            //

            UCRSegment->CommittedSize += CommitSize;
        }

        //
        //  At this point UCR segment exists and First Entry points to the
        //  start of the available committed space.  We'll make Last Entry
        //  point to the end of the committed space
        //

        LastEntry = (PCHAR)UCRSegment + UCRSegment->CommittedSize;

        //
        //  Now the task is to push all of this new space unto the
        //  unused uncommitted range list off the heap, then we can
        //  do a regular pop
        //

        UnCommittedRange = (PHEAP_UNCOMMMTTED_RANGE)FirstEntry;

        pp = &Segment->Heap->UnusedUnCommittedRanges;

        while ((PCHAR)UnCommittedRange < (PCHAR)LastEntry) {

            *pp = UnCommittedRange;
            pp = &UnCommittedRange->Next;
            UnCommittedRange += 1;
        }

        //
        //  Null terminate the list
        //

        *pp = NULL;

        //
        //  And have Pp point the new top of the list
        //

        pp = &Segment->Heap->UnusedUnCommittedRanges;
    }

    //
    //  At this point the Pp points to a non empty list of unused uncommitted
    //  range structures.  So we pop the list and return the top to our caller
    //

    UnCommittedRange = *pp;
    *pp = UnCommittedRange->Next;

    return UnCommittedRange;
}


//
//  Local Support Routine
//

VOID
RtlpDestroyUnCommittedRange (
    IN PHEAP_SEGMENT Segment,
    IN PHEAP_UNCOMMMTTED_RANGE UnCommittedRange
    )

/*++

Routine Description:

    This routine returns an uncommitted range structure back to the unused
    uncommitted range list

Arguments:

    Segment - Supplies any segment in the heap being modified.  Most likely but
        not necessarily the segment containing the uncommitted range structure

    UnCommittedRange - Supplies a pointer to the uncommitted range structure
        being decommissioned.

Return Value:

    None.

--*/

{
    RTL_PAGED_CODE();

    //
    //  This routine simply does a "push" of the uncommitted range structure
    //  onto the heap's stack of unused uncommitted ranges
    //

    UnCommittedRange->Next = Segment->Heap->UnusedUnCommittedRanges;
    Segment->Heap->UnusedUnCommittedRanges = UnCommittedRange;

    //
    //  For safety sake we'll also zero out the fields in the decommissioned
    //  structure
    //

    UnCommittedRange->Address = 0;
    UnCommittedRange->Size = 0;

    //
    //  And return to our caller
    //

    return;
}


//
//  Local Support Routine
//

VOID
RtlpInsertUnCommittedPages (
    IN PHEAP_SEGMENT Segment,
    IN ULONG_PTR Address,
    IN SIZE_T Size
    )

/*++

Routine Description:

    This routine adds the specified range to the list of uncommitted pages
    in the segment.  When done the information will hang off the segments
    uncommitted ranges list.

Arguments:

    Segment - Supplies a segment whose uncommitted range is being modified

    Address - Supplies the base (start) address for the uncommitted range

    Size - Supplies the size, in bytes, of the uncommitted range

Return Value:

    None.

--*/

{
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange, *pp;

    RTL_PAGED_CODE();

    //
    //  Get a pointer to the front of the segments uncommitted range list
    //  The list is sorted by ascending address
    //

    pp = &Segment->UnCommittedRanges;

    //
    //  While we haven't reached the end of the list we'll zoom through
    //  trying to find a fit
    //

    while (UnCommittedRange = *pp) {

        //
        //  If address we want is less than what we're pointing at then
        //  we've found where this new entry goes
        //

        if (UnCommittedRange->Address > Address) {

            //
            //  If the new block matches right up to the existing block
            //  then we can simply backup the existing block and add
            //  to its size
            //

            if ((Address + Size) == UnCommittedRange->Address) {

                UnCommittedRange->Address = Address;
                UnCommittedRange->Size += Size;

                //
                //  Check if we need to update our notion of what the
                //  largest uncommitted range is
                //

                if (UnCommittedRange->Size > Segment->LargestUnCommittedRange) {

                    Segment->LargestUnCommittedRange = UnCommittedRange->Size;
                }

                //
                //  And return to our caller
                //

                return;
            }

            //
            //  Pp is the address of the block right before us, and *Pp is the
            //  address of the block right after us.  So now fall out to where
            //  the insertion takes place.
            //

            break;

        //
        //  Otherwise if this existing block stops right where the new block
        //  starts then we get to modify this entry.
        //

        } else if ((UnCommittedRange->Address + UnCommittedRange->Size) == Address) {

            //
            //  Remember the starting address and compute the new larger size
            //

            Address = UnCommittedRange->Address;
            Size += UnCommittedRange->Size;

            //
            //  Remove this entry from the list and then return it to the
            //  unused uncommitted list
            //

            *pp = UnCommittedRange->Next;

            RtlpDestroyUnCommittedRange( Segment, UnCommittedRange );

            //
            //  Modify the segemnt counters and largest size state.  The next
            //  time through the loop should hit the first case above where
            //  we'll either merge with a list following us or add a new
            //  entry
            //

            Segment->NumberOfUnCommittedRanges -= 1;

            if (Size > Segment->LargestUnCommittedRange) {

                Segment->LargestUnCommittedRange = Size;
            }

        //
        //  Otherwise we'll continue search down the list
        //

        } else {

            pp = &UnCommittedRange->Next;
        }
    }

    //
    //  If we reach this point that means we've either fallen off the end of the
    //  list, or the list is empty, or we've located the spot where a new uncommitted
    //  range structure belongs.  So allocate a new uncommitted range structure,
    //  and make sure we got one.
    //
    //  Pp is the address of the block right before us and *Pp is the address of the
    //  block right after us
    //

    UnCommittedRange = RtlpCreateUnCommittedRange( Segment );

    if (UnCommittedRange == NULL) {

        HeapDebugPrint(( "Abandoning uncommitted range (%x for %x)\n", Address, Size ));
        // HeapDebugBreak( NULL );

        return;
    }

    //
    //  Fill in the new uncommitted range structure
    //

    UnCommittedRange->Address = Address;
    UnCommittedRange->Size = Size;

    //
    //  Insert it in the list for the segment
    //

    UnCommittedRange->Next = *pp;
    *pp = UnCommittedRange;

    //
    //  Update the segment counters and notion of the largest uncommitted range
    //

    Segment->NumberOfUnCommittedRanges += 1;

    if (Size >= Segment->LargestUnCommittedRange) {

        Segment->LargestUnCommittedRange = Size;
    }

    //
    //  And return to our caller
    //

    return;
}


//
//  Declared in heappriv.h
//

PHEAP_FREE_ENTRY
RtlpFindAndCommitPages (
    IN PHEAP Heap,
    IN PHEAP_SEGMENT Segment,
    IN OUT PSIZE_T Size,
    IN PVOID AddressWanted OPTIONAL
    )

/*++

Routine Description:

    This function searches the supplied segment for an uncommitted range that
    satisfies the specified size.  It commits the range and returns a heap entry
    for the range.

Arguments:

    Heap - Supplies the heap being maniuplated

    Segment - Supplies the segment being searched

    Size - Supplies the size of what we need to look for, on return it contains
        the size of what we're just found and committed.

    AddressWanted - Optionally gives an address where we would like the pages
        based.  If supplied the entry must start at this address

Return Value:

    PHEAP_FREE_ENTRY - Returns a pointer to the newly committed range that
        satisfies the given size requirement, or NULL if we could not find
        something large enough and/or based at the address wanted.

--*/

{
    NTSTATUS Status;
    PHEAP_ENTRY FirstEntry, LastEntry, PreviousLastEntry;
    PHEAP_UNCOMMMTTED_RANGE PreviousUnCommittedRange, UnCommittedRange, *pp;
    ULONG_PTR Address;
    SIZE_T Length;

    RTL_PAGED_CODE();

    //
    //  What the outer loop does is cycle through the uncommited ranges
    //  stored in in the specified segment
    //

    PreviousUnCommittedRange = NULL;
    pp = &Segment->UnCommittedRanges;

    while (UnCommittedRange = *pp) {

        //
        //  Check for the best of worlds, where the size of this current
        //  uncommitted range satisfies our size request and either the user
        //  didn't specify an address or the address match
        //

        if ((UnCommittedRange->Size >= *Size) &&
            (!ARGUMENT_PRESENT( AddressWanted ) || (UnCommittedRange->Address == (ULONG_PTR)AddressWanted ))) {

            //
            //  Calculate an address
            //

            Address = UnCommittedRange->Address;

            //
            //  Commit the memory.  If the heap doesn't have a commit
            //  routine then use the default mm supplied routine.
            //

            if (Heap->CommitRoutine != NULL) {

                Status = (Heap->CommitRoutine)( Heap,
                                                (PVOID *)&Address,
                                                Size );

            } else {

                Status = ZwAllocateVirtualMemory( (PVOID *)&Address,
                                                  0,
                                                  Size,
                                                  MEM_COMMIT,
                                                  PAGE_READWRITE );

            }

            if (!NT_SUCCESS( Status )) {

                return NULL;
            }

            //
            //  At this point we have some committed memory, with Address and Size
            //  giving us the necessary details
            //
            //  Update the number of uncommitted pages in the segment and if necessary
            //  mark down the largest uncommitted range
            //

            Segment->NumberOfUnCommittedPages -= (ULONG) (*Size / PAGE_SIZE);

            if (Segment->LargestUnCommittedRange == UnCommittedRange->Size) {

                Segment->LargestUnCommittedRange = 0;
            }

            //
            //  First entry is the start of the newly committed range
            //

            FirstEntry = (PHEAP_ENTRY)Address;

            //
            //  We want last entry to point to the last real entry before
            //  this newly committed spot.  To do this we start by
            //  setting last entry to either the first entry for the
            //  segment or (if we can do better), to right after the last
            //  uncommitted range we examined.  Either way it points to
            //  some committed range
            //

            if ((Segment->LastEntryInSegment->Flags & HEAP_ENTRY_LAST_ENTRY) &&
                (ULONG_PTR)(Segment->LastEntryInSegment + Segment->LastEntryInSegment->Size) == UnCommittedRange->Address) {

                LastEntry = Segment->LastEntryInSegment;

            } else {

                if (PreviousUnCommittedRange == NULL) {

                    LastEntry = Segment->FirstEntry;

                } else {

                    LastEntry = (PHEAP_ENTRY)(PreviousUnCommittedRange->Address +
                                              PreviousUnCommittedRange->Size);
                }

                //
                //  Now we zoom through the entries until we find the one
                //  marked last
                //

                while (!(LastEntry->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                    PreviousLastEntry = LastEntry;
                    LastEntry += LastEntry->Size;

                    if (((PCHAR)LastEntry >= (PCHAR)Segment->LastValidEntry) || (LastEntry->Size == 0)) {

                        //
                        //  Check for the situation where the last entry in the
                        //  segment isn't marked as a last entry but does put
                        //  us right where the have a new committed range
                        //

                        if (LastEntry == (PHEAP_ENTRY)Address) {

                            LastEntry = PreviousLastEntry;

                            break;
                        }

                        HeapDebugPrint(( "Heap missing last entry in committed range near %x\n", PreviousLastEntry ));
                        HeapDebugBreak( PreviousLastEntry );

                        return NULL;
                    }
                }
            }

            //
            //  Turn off the last bit on this entry because what's following
            //  is no longer uncommitted
            //

            LastEntry->Flags &= ~HEAP_ENTRY_LAST_ENTRY;

            //
            //  Shrink the uncommited range by the size we've committed
            //

            UnCommittedRange->Address += *Size;
            UnCommittedRange->Size -= *Size;

            //
            //  Now if the size is zero then we've committed everything that there
            //  was in the range.  Otherwise make sure the first entry of what
            //  we've just committed knows that an uncommitted range follows.
            //

            if (UnCommittedRange->Size == 0) {

                //
                //  This uncommitted range is about to vanish.  Base on if the
                //  range is the last one in the segemnt then we know how to
                //  mark the committed range as being last or not.
                //

                if (UnCommittedRange->Address == (ULONG_PTR)Segment->LastValidEntry) {

                    FirstEntry->Flags = HEAP_ENTRY_LAST_ENTRY;

                    Segment->LastEntryInSegment = FirstEntry;

                } else {

                    FirstEntry->Flags = 0;

                    Segment->LastEntryInSegment = Segment->FirstEntry;
                }

                //
                //  Remove this zero sized range from the uncommitted range
                //  list, and update the segment counters
                //

                *pp = UnCommittedRange->Next;

                RtlpDestroyUnCommittedRange( Segment, UnCommittedRange );

                Segment->NumberOfUnCommittedRanges -= 1;

            } else {

                //
                //  Otherwise the range is not empty so we know what we committed
                //  is immediately followed by an uncommitted range
                //

                FirstEntry->Flags = HEAP_ENTRY_LAST_ENTRY;

                Segment->LastEntryInSegment = FirstEntry;
            }

            //
            //  Update the fields in the first entry, and optional
            //  following entry.
            //

            FirstEntry->SegmentIndex = LastEntry->SegmentIndex;
            FirstEntry->Size = (USHORT)(*Size >> HEAP_GRANULARITY_SHIFT);
            FirstEntry->PreviousSize = LastEntry->Size;

            if (!(FirstEntry->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                (FirstEntry + FirstEntry->Size)->PreviousSize = FirstEntry->Size;
            }

            //
            //  Now if we adjusted the largest uncommitted range to zero then
            //  we need to go back and find the largest uncommitted range
            //  To do that we simply zoom down the uncommitted range list
            //  remembering the largest one
            //

            if (Segment->LargestUnCommittedRange == 0) {

                UnCommittedRange = Segment->UnCommittedRanges;

                while (UnCommittedRange != NULL) {

                    if (UnCommittedRange->Size >= Segment->LargestUnCommittedRange) {

                        Segment->LargestUnCommittedRange = UnCommittedRange->Size;
                    }

                    UnCommittedRange = UnCommittedRange->Next;
                }
            }

            //
            //  And return the heap entry to our caller
            //

            return (PHEAP_FREE_ENTRY)FirstEntry;

        } else {

            //
            //  Otherwise the current uncommited range is too small or
            //  doesn't have the right address so go to the next uncommitted
            //  range entry
            //

            PreviousUnCommittedRange = UnCommittedRange;
            pp = &UnCommittedRange->Next;
        }
    }

    //
    //  At this point we did not find an uncommitted range entry that satisfied
    //  our requirements either because of size and/or address.  So return null
    //  to tell the user we didn't find anything.
    //

    return NULL;
}


//
//  Declared in heappriv.h
//

BOOLEAN
RtlpInitializeHeapSegment (
    IN PHEAP Heap,
    IN PHEAP_SEGMENT Segment,
    IN UCHAR SegmentIndex,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN PVOID UnCommittedAddress,
    IN PVOID CommitLimitAddress
    )

/*++

Routine Description:

    This routines initializes the internal structures for a heap segment.
    The caller supplies the heap and the memory for the segment being
    initialized

Arguments:

    Heap - Supplies the address of the heap owning this segment

    Segment - Supplies a pointer to the segment being initialized

    SegmentIndex - Supplies the segement index within the heap that this
        new segment is being assigned

    Flags - Supplies flags controlling the initialization of the segment
        Valid flags are:

            HEAP_SEGMENT_USER_ALLOCATED

    BaseAddress - Supplies the base address for the segment

    UnCommittedAddress - Supplies the address where the uncommited range starts

    CommitLimitAddress - Supplies the top address available to the segment

Return Value:

    BOOLEAN - TRUE if the initialization is successful and FALSE otherwise

--*/

{
    NTSTATUS Status;
    PHEAP_ENTRY FirstEntry;
    USHORT PreviousSize, Size;
    ULONG NumberOfPages;
    ULONG NumberOfCommittedPages;
    ULONG NumberOfUnCommittedPages;
    SIZE_T CommitSize;

    RTL_PAGED_CODE();

    //
    //  Compute the total number of pages possible in this segment
    //

    NumberOfPages = (ULONG) (((PCHAR)CommitLimitAddress - (PCHAR)BaseAddress) / PAGE_SIZE);

    //
    //  First entry points to the first possible segment entry after
    //  the segment header
    //

    FirstEntry = (PHEAP_ENTRY)ROUND_UP_TO_POWER2( Segment + 1,
                                                  HEAP_GRANULARITY );

    //
    //  Now if the heap is equal to the base address for the segment which
    //  it the case for the segment zero then the previous size is the
    //  heap header.  Otherwise there isn't a previous entry
    //

    if ((PVOID)Heap == BaseAddress) {

        PreviousSize = Heap->Entry.Size;

    } else {

        PreviousSize = 0;
    }

    //
    //  Compute the index size of the segement header
    //

    Size = (USHORT)(((PCHAR)FirstEntry - (PCHAR)Segment) >> HEAP_GRANULARITY_SHIFT);

    //
    //  If the first available heap entry is not committed and
    //  it is beyond the heap limit then we cannot initialize
    //

    if ((PCHAR)(FirstEntry + 1) >= (PCHAR)UnCommittedAddress) {

        if ((PCHAR)(FirstEntry + 1) >= (PCHAR)CommitLimitAddress) {

            return FALSE;
        }

        //
        //  Enough of the segment has not been committed so we
        //  will commit enough now to handle the first entry
        //

        CommitSize = (PCHAR)(FirstEntry + 1) - (PCHAR)UnCommittedAddress;

        Status = ZwAllocateVirtualMemory( (PVOID *)&UnCommittedAddress,
                                          0,
                                          &CommitSize,
                                          MEM_COMMIT,
                                          PAGE_READWRITE );

        if (!NT_SUCCESS( Status )) {

            return FALSE;
        }

        //
        //  Because we had to commit some memory we need to adjust
        //  the uncommited address
        //

        UnCommittedAddress = (PVOID)((PCHAR)UnCommittedAddress + CommitSize);
    }

    //
    //  At this point we know there is enough memory committed to handle the
    //  segment header and one heap entry
    //
    //  Now compute the number of uncommited pages and the number of committed
    //  pages
    //

    NumberOfUnCommittedPages = (ULONG)(((PCHAR)CommitLimitAddress - (PCHAR)UnCommittedAddress) / PAGE_SIZE);
    NumberOfCommittedPages = NumberOfPages - NumberOfUnCommittedPages;

    //
    //  Initialize the heap segment heap entry.  We
    //  calculated earlier if there was a previous entry
    //

    Segment->Entry.PreviousSize = PreviousSize;
    Segment->Entry.Size = Size;
    Segment->Entry.Flags = HEAP_ENTRY_BUSY;
    Segment->Entry.SegmentIndex = SegmentIndex;

#if i386 && !NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel x86 case see if we need to capture the callers stack
    //  backtrace
    //

    if (NtGlobalFlag & FLG_USER_STACK_TRACE_DB) {

        Segment->AllocatorBackTraceIndex = (USHORT)RtlLogStackBackTrace();
    }

#endif // i386 && !NTOS_KERNEL_RUNTIME

    //
    //  Now initializes the heap segment
    //

    Segment->Signature = HEAP_SEGMENT_SIGNATURE;
    Segment->Flags = Flags;
    Segment->Heap = Heap;
    Segment->BaseAddress = BaseAddress;
    Segment->FirstEntry = FirstEntry;
    Segment->LastValidEntry = (PHEAP_ENTRY)((PCHAR)BaseAddress + (NumberOfPages * PAGE_SIZE));
    Segment->NumberOfPages = NumberOfPages;
    Segment->NumberOfUnCommittedPages = NumberOfUnCommittedPages;

    //
    //  If there are uncommitted pages then we need to insert them
    //  into the uncommitted ranges list
    //

    if (NumberOfUnCommittedPages) {

        RtlpInsertUnCommittedPages( Segment,
                                    (ULONG_PTR)UnCommittedAddress,
                                    NumberOfUnCommittedPages * PAGE_SIZE );
    }

    //
    //  Have the containing heap point to this segment via the specified index
    //

    Heap->Segments[ SegmentIndex ] = Segment;

    //
    //  Initialize the first free heap entry after the heap segment header and
    //  put it in the free list.  This first entry will be for whatever is left
    //  of the committed range
    //

    PreviousSize = Segment->Entry.Size;
    FirstEntry->Flags = HEAP_ENTRY_LAST_ENTRY;

    Segment->LastEntryInSegment = FirstEntry;

    FirstEntry->PreviousSize = PreviousSize;
    FirstEntry->SegmentIndex = SegmentIndex;

    RtlpInsertFreeBlock( Heap,
                         (PHEAP_FREE_ENTRY)FirstEntry,
                         (PHEAP_ENTRY)UnCommittedAddress - FirstEntry);

    //
    //  And return to our caller
    //

    return TRUE;
}


//
//  Local Support Routine
//

NTSTATUS
RtlpDestroyHeapSegment (
    IN PHEAP_SEGMENT Segment
    )

/*++

Routine Description:

    This routine removes an existing heap segment.  After the call it
    is as if the segment never existed

Arguments:

    Segment - Supplies a pointer to the heap segment being destroyed

Return Value:

    NTSTATUS - An appropriate status value

--*/

{
    PVOID BaseAddress;
    SIZE_T BytesToFree;

    RTL_PAGED_CODE();

    //
    //  We actually only have work to do if the segment is not
    //  user allocated.  If the segement is user allocated then
    //  we'll assume knows how to get rid of the memory
    //

    if (!(Segment->Flags & HEAP_SEGMENT_USER_ALLOCATED)) {

        BaseAddress = Segment->BaseAddress;
        BytesToFree = 0;

        //
        //  Free all the virtual memory for the segment and return
        //  to our caller.
        //

        return ZwFreeVirtualMemory( (PVOID *)&BaseAddress,
                                    &BytesToFree,
                                    MEM_RELEASE );

    } else {

        //
        //  User allocated segments are a noop
        //

        return STATUS_SUCCESS;
    }
}



//
//  Local Support Routine
//

PHEAP_FREE_ENTRY
RtlpExtendHeap (
    IN PHEAP Heap,
    IN SIZE_T AllocationSize
    )

/*++

Routine Description:

    This routine is used to extend the amount of committed memory in a heap

Arguments:

    Heap - Supplies the heap being modified

    AllocationSize - Supplies the size, in bytes, that we need to extend the
        heap

Return Value:

    PHEAP_FREE_ENTRY - Returns a pointer to the newly created heap entry
        of the specified size, or NULL if we weren't able to extend the heap

--*/

{
    NTSTATUS Status;
    PHEAP_SEGMENT Segment;
    PHEAP_FREE_ENTRY FreeBlock;
    UCHAR SegmentIndex, EmptySegmentIndex;
    ULONG NumberOfPages;
    SIZE_T CommitSize;
    SIZE_T ReserveSize;
    SIZE_T FreeSize;

    RTL_PAGED_CODE();

    //
    //  Compute the number of pages need to hold this extension
    //  And then compute the real free, still in bytes, based on
    //  the page count
    //

    NumberOfPages = (ULONG) ((AllocationSize + PAGE_SIZE - 1) / PAGE_SIZE);
    FreeSize = NumberOfPages * PAGE_SIZE;

    //
    //  For every segment we're either going to look for an existing
    //  heap segment that we can get some pages out of or we will
    //  identify a free heap segment index where we'll try and create a new
    //  segment
    //

    EmptySegmentIndex = HEAP_MAXIMUM_SEGMENTS;

    for (SegmentIndex=0; SegmentIndex<HEAP_MAXIMUM_SEGMENTS; SegmentIndex++) {

        Segment = Heap->Segments[ SegmentIndex ];

        //
        //  If the segment exists and number of uncommitted pages will
        //  satisfy our reguest and the largest uncommitted range will
        //  also satisfy our request then we'll try and segment
        //
        //  **** note that this second test seems unnecessary given that
        //  **** the largest uncommitted range is also being tested
        //

        if ((Segment) &&
            (NumberOfPages <= Segment->NumberOfUnCommittedPages) &&
            (FreeSize <= Segment->LargestUnCommittedRange)) {

            //
            //  Looks like a good segment so try and commit the
            //  amount we need
            //

            FreeBlock = RtlpFindAndCommitPages( Heap,
                                                Segment,
                                                &FreeSize,
                                                NULL );

            //
            //  If we were successful the we will coalesce it with adjacent
            //  free blocks and put it in the free list then return the
            //  the free block
            //

            if (FreeBlock != NULL) {

                //
                //  **** gdk ****
                //  **** this doesn't seem right given that coalesece should take
                //  **** byte count and not heap entry count
                //

                FreeSize = FreeSize >> HEAP_GRANULARITY_SHIFT;

                FreeBlock = RtlpCoalesceFreeBlocks( Heap, FreeBlock, &FreeSize, FALSE );

                RtlpInsertFreeBlock( Heap, FreeBlock, FreeSize );

                return FreeBlock;
            }

        //
        //  Otherwise if the segment index is not in use and we haven't
        //  yet identified a unused segment index then remembeer this
        //  index
        //

        } else if ((Segment == NULL) &&
                   (EmptySegmentIndex == HEAP_MAXIMUM_SEGMENTS)) {

            EmptySegmentIndex = SegmentIndex;
        }
    }

    //
    //  At this point we weren't able to get the memory from an existing
    //  heap segment so now check if we found an unused segment index
    //  and if we're alowed to grow the heap.
    //

    if ((EmptySegmentIndex != HEAP_MAXIMUM_SEGMENTS) &&
        (Heap->Flags & HEAP_GROWABLE)) {

        Segment = NULL;

        //
        //  Calculate a reserve size for the new segment, we might
        //  need to fudge it up if the allocation size we're going for
        //  right now is already beyond the default reserve size
        //

        if ((AllocationSize + PAGE_SIZE) > Heap->SegmentReserve) {

            ReserveSize = AllocationSize + PAGE_SIZE;

        } else {

            ReserveSize = Heap->SegmentReserve;
        }

        //
        //  Try and reserve some vm
        //

        Status = ZwAllocateVirtualMemory( (PVOID *)&Segment,
                                          0,
                                          &ReserveSize,
                                          MEM_RESERVE,
                                          PAGE_READWRITE );

        //
        //  If we get back status no memory then we should trim back the
        //  request to something reasonable and try again.  We'll half
        //  the amount until we it either succeeds or until we reach
        //  the allocation size.  In the latter case we are really
        //  out of memory.
        //

        while ((!NT_SUCCESS( Status )) && (ReserveSize != (AllocationSize + PAGE_SIZE))) {

            ReserveSize = ReserveSize / 2;

            if( ReserveSize < (AllocationSize + PAGE_SIZE) ) {

                ReserveSize = (AllocationSize + PAGE_SIZE);
            }

            Status = ZwAllocateVirtualMemory( (PVOID *)&Segment,
                                              0,
                                              &ReserveSize,
                                              MEM_RESERVE,
                                              PAGE_READWRITE );
        }

        if (NT_SUCCESS( Status )) {

            //
            //  Adjust the heap state information
            //

            Heap->SegmentReserve += ReserveSize;

            //
            //  Compute the commit size to be either the default, or if
            //  that's not big enough then make it big enough to handle
            //  this current request
            //

            if ((AllocationSize + PAGE_SIZE) > Heap->SegmentCommit) {

                CommitSize = AllocationSize + PAGE_SIZE;

            } else {

                CommitSize = Heap->SegmentCommit;
            }

            //
            //  Try and commit the memory
            //

            Status = ZwAllocateVirtualMemory( (PVOID *)&Segment,
                                              0,
                                              &CommitSize,
                                              MEM_COMMIT,
                                              PAGE_READWRITE );

            //
            //  If the commit is successful but we were not able to
            //  initialize the heap segment then still make the status
            //  and error value
            //

            if (NT_SUCCESS( Status ) &&
                !RtlpInitializeHeapSegment( Heap,
                                            Segment,
                                            EmptySegmentIndex,
                                            0,
                                            Segment,
                                            (PCHAR)Segment + CommitSize,
                                            (PCHAR)Segment + ReserveSize)) {

                Status = STATUS_NO_MEMORY;
            }

            //
            //  If we've been successful so far then we're done and we
            //  can return the first entry in the segment to our caller
            //

            if (NT_SUCCESS(Status)) {

                return (PHEAP_FREE_ENTRY)Segment->FirstEntry;
            }

            //
            //  Otherwise either the commit or heap segment initialization failed
            //  so we'll release the memory which will also decommit it if necessary
            //

            ZwFreeVirtualMemory( (PVOID *)&Segment,
                                 &ReserveSize,
                                 MEM_RELEASE );
        }
    }

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  In the non kernel case we disabled coaleseing on free then what we'll
    //  do as a last resort is coalesce the heap and see if a block comes out
    //  that we can use
    //

    if (Heap->Flags & HEAP_DISABLE_COALESCE_ON_FREE) {

        FreeBlock = RtlpCoalesceHeap( Heap );

        if ((FreeBlock != NULL) && (FreeBlock->Size >= AllocationSize)) {

            return FreeBlock;
        }
    }

#endif // NTOS_KERNEL_RUNTIME

    //
    //  Either the heap cannot grow or we out of resources of some type
    //  so we're going to return null
    //

    return NULL;
}


//
//  Declared in heappriv.h
//

PHEAP_FREE_ENTRY
RtlpCoalesceFreeBlocks (
    IN PHEAP Heap,
    IN PHEAP_FREE_ENTRY FreeBlock,
    IN OUT PSIZE_T FreeSize,
    IN BOOLEAN RemoveFromFreeList
    )

/*++

Routine Description:

    This routine coalesces the free block together.

Arguments:

    Heap - Supplies a pointer to the heap being manipulated

    FreeBlock - Supplies a pointer to the free block that we want coalesced

    FreeSize - Supplies the size, in bytes, of the free block.  On return it
        contains the size, in bytes, of the of the newly coalesced free block

    RemoveFromFreeList - Indicates if the input free block is already on a
        free list and needs to be removed to before coalescing

Return Value:

    PHEAP_FREE_ENTRY - returns a pointer to the newly coalesced free block

--*/

{
    PHEAP_FREE_ENTRY FreeBlock1, NextFreeBlock;

    RTL_PAGED_CODE();

    //
    //  Point to the preceding block
    //

    FreeBlock1 = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)FreeBlock - FreeBlock->PreviousSize);

    //
    //  Check if there is a preceding block, and if it is free, and the two sizes
    //  put together will still fit on a free lists.
    //

    if ((FreeBlock1 != FreeBlock) &&
        !(FreeBlock1->Flags & HEAP_ENTRY_BUSY) &&
        ((*FreeSize + FreeBlock1->Size) <= HEAP_MAXIMUM_BLOCK_SIZE)) {

        //
        //  We are going to merge ourselves with the preceding block
        //

        HEAPASSERT(FreeBlock->PreviousSize == FreeBlock1->Size);

        //
        //  Check if we need to remove the input block from the free list
        //

        if (RemoveFromFreeList) {

            RtlpRemoveFreeBlock( Heap, FreeBlock );

            Heap->TotalFreeSize -= FreeBlock->Size;

            //
            //  We're removed so we don't have to do it again
            //

            RemoveFromFreeList = FALSE;
        }

        //
        //  Remove the preceding block from its free list
        //

        RtlpRemoveFreeBlock( Heap, FreeBlock1 );

        //
        //  Copy over the last entry flag if necessary from what we're freeing
        //  to the preceding block
        //

        FreeBlock1->Flags = FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY;

        if( FreeBlock1->Flags & HEAP_ENTRY_LAST_ENTRY ) {

            PHEAP_SEGMENT Segment;

            Segment = Heap->Segments[FreeBlock1->SegmentIndex];
            Segment->LastEntryInSegment = (PHEAP_ENTRY)FreeBlock1;
        }

        //
        //  Point to the preceding block, and adjust the sizes for the
        //  new free block.  It is the total of both blocks.
        //

        FreeBlock = FreeBlock1;

        *FreeSize += FreeBlock1->Size;

        Heap->TotalFreeSize -= FreeBlock1->Size;

        FreeBlock->Size = (USHORT)*FreeSize;

        //
        //  Check if we need to update the previous size of the next
        //  entry
        //

        if (!(FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

            ((PHEAP_ENTRY)FreeBlock + *FreeSize)->PreviousSize = (USHORT)*FreeSize;
        }
    }

    //
    //  Check if there is a following block.
    //

    if (!(FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

        //
        //  There is a following block so now get a pointer to it
        //  and check if it is free and if putting the two blocks together
        //  still fits on a free list
        //

        NextFreeBlock = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)FreeBlock + *FreeSize);

        if (!(NextFreeBlock->Flags & HEAP_ENTRY_BUSY) &&
            ((*FreeSize + NextFreeBlock->Size) <= HEAP_MAXIMUM_BLOCK_SIZE)) {

            //
            //  We are going to merge ourselves with the following block
            //

            HEAPASSERT(*FreeSize == NextFreeBlock->PreviousSize);

            //
            //  Check if we need to remove the input block from the free list
            //

            if (RemoveFromFreeList) {

                RtlpRemoveFreeBlock( Heap, FreeBlock );

                Heap->TotalFreeSize -= FreeBlock->Size;

                //
                //  **** this assignment isn't necessary because there isn't
                //  **** any more merging after this one
                //

                RemoveFromFreeList = FALSE;
            }

            //
            //  Copy up the last entry flag if necessary from the following
            //  block to our input block
            //

            FreeBlock->Flags = NextFreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY;

            if( FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY ) {

                PHEAP_SEGMENT Segment;

                Segment = Heap->Segments[FreeBlock->SegmentIndex];
                Segment->LastEntryInSegment = (PHEAP_ENTRY)FreeBlock;
            }

            //
            //  Remove the following block from its free list
            //

            RtlpRemoveFreeBlock( Heap, NextFreeBlock );

            //
            //  Adjust the size for the newly combined block
            //

            *FreeSize += NextFreeBlock->Size;

            Heap->TotalFreeSize -= NextFreeBlock->Size;

            FreeBlock->Size = (USHORT)*FreeSize;

            //
            //  Check if we need to update the previous size of the next block
            //

            if (!(FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

                ((PHEAP_ENTRY)FreeBlock + *FreeSize)->PreviousSize = (USHORT)*FreeSize;
            }
        }
    }

    //
    //  And return the free block to our caller
    //

    return FreeBlock;
}


//
//  Declared in heappriv.h
//

VOID
RtlpDeCommitFreeBlock (
    IN PHEAP Heap,
    IN PHEAP_FREE_ENTRY FreeBlock,
    IN SIZE_T FreeSize
    )

/*++

Routine Description:

    This routine takes a free block and decommits it.  This is usually called
    because the block is beyond the decommit threshold

Arguments:

    Heap - Supplies a pointer to the heap being manipulated

    FreeBlock - Supplies a pointer to the block being decommitted

    FreeSize - Supplies the size, in bytes, of the free block being decommitted

Return Value:

    None.

--*/

{
    NTSTATUS Status;
    ULONG_PTR DeCommitAddress;
    SIZE_T DeCommitSize;
    USHORT LeadingFreeSize, TrailingFreeSize;
    PHEAP_SEGMENT Segment;
    PHEAP_FREE_ENTRY LeadingFreeBlock, TrailingFreeBlock;
    PHEAP_ENTRY LeadingBusyBlock, TrailingBusyBlock;
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange;

    RTL_PAGED_CODE();

    //
    //  If the heap has a user specified decommit routine then we won't really
    //  decommit anything instead we'll call a worker routine to chop it up
    //  into pieces that will fit on the free lists
    //

    if (Heap->CommitRoutine != NULL) {

        RtlpInsertFreeBlock( Heap, FreeBlock, FreeSize );

        return;
    }

    //
    //  Get a pointer to the owning segment
    //

    Segment = Heap->Segments[ FreeBlock->SegmentIndex ];

    //
    //  The leading busy block identifies the preceding in use block before
    //  what we are trying to decommit.  It is only used if what we are trying
    //  decommit is right on a page boundary and then it is the block right
    //  before us if it exists.
    //
    //  The leading free block is used to identify whatever space is needed
    //  to round up the callers specified address to a page address.  If the
    //  caller already gave us a page aligned address then the free block
    //  address is identical to what the caller supplied.
    //

    LeadingBusyBlock = NULL;
    LeadingFreeBlock = FreeBlock;

    //
    //  Make sure the block we are trying to decommit start on the next full
    //  page boundary.  The leading free size is the size of whatever it takes
    //  to round up the free block to the next page specified in units of
    //  heap entries.
    //

    DeCommitAddress = ROUND_UP_TO_POWER2( LeadingFreeBlock, PAGE_SIZE );
    LeadingFreeSize = (USHORT)((PHEAP_ENTRY)DeCommitAddress - (PHEAP_ENTRY)LeadingFreeBlock);

    //
    //  If we leading free size only has space for one heap entry then we'll
    //  bump it up to include the next page, because we don't want to leave
    //  anything that small laying around.  Otherwise if we have a preceding
    //  block and the leading free size is zero then identify the preceding
    //  block as the leading busy block
    //

    if (LeadingFreeSize == 1) {

        DeCommitAddress += PAGE_SIZE;
        LeadingFreeSize += PAGE_SIZE >> HEAP_GRANULARITY_SHIFT;

    } else if (LeadingFreeBlock->PreviousSize != 0) {

        if (DeCommitAddress == (ULONG_PTR)LeadingFreeBlock) {

            LeadingBusyBlock = (PHEAP_ENTRY)LeadingFreeBlock - LeadingFreeBlock->PreviousSize;
        }
    }

    //
    //  The trailing busy block identifies the block immediately after the one
    //  we are trying to decommit provided what we are decommitting ends right
    //  on a page boundary otherwise the trailing busy block stays null and
    //  the trailing free block value is used.
    //
    //  **** gdk ****
    //  **** the assignment of tailing free block doesn't seem right because
    //  **** Free size should be in bytes, and not heap entries
    //

    TrailingBusyBlock = NULL;
    TrailingFreeBlock = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)FreeBlock + FreeSize);

    //
    //  Make sure the block we are trying to decommit ends on a page boundary.
    //
    //  And compute how many heap entries we had to backup to make it land on a
    //  page boundary.
    //

    DeCommitSize = ROUND_DOWN_TO_POWER2( (ULONG_PTR)TrailingFreeBlock, PAGE_SIZE );
    TrailingFreeSize = (USHORT)((PHEAP_ENTRY)TrailingFreeBlock - (PHEAP_ENTRY)DeCommitSize);

    //
    //  If the trailing free size is exactly one heap in size then we will
    //  nibble off a bit more from the decommit size because free block of
    //  exactly one heap entry in size are useless.  Otherwise if we actually
    //  ended on a page boundary and there is a block after us then indicate
    //  that we have a trailing busy block
    //

    if (TrailingFreeSize == (sizeof( HEAP_ENTRY ) >> HEAP_GRANULARITY_SHIFT)) {

        DeCommitSize -= PAGE_SIZE;
        TrailingFreeSize += PAGE_SIZE >> HEAP_GRANULARITY_SHIFT;

    } else if ((TrailingFreeSize == 0) && !(FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY)) {

        TrailingBusyBlock = (PHEAP_ENTRY)TrailingFreeBlock;
    }

    //
    //  Now adjust the trailing free block to compensate for the trailing free size
    //  we just computed.
    //

    TrailingFreeBlock = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)TrailingFreeBlock - TrailingFreeSize);

    //
    //  Right now DeCommit size is really a pointer.  If it points at is beyond
    //  the decommit address then make the size really be just the byte count
    //  to decommit.  Otherwise the decommit size is zero.
    //

    if (DeCommitSize > DeCommitAddress) {

        DeCommitSize -= DeCommitAddress;

    } else {

        DeCommitSize = 0;
    }

    //
    //  **** this next test is bogus given the if-then-else that just preceded it
    //
    //  Now check if we still have something to decommit
    //

    if (DeCommitSize != 0) {

        //
        //  Before freeing the memory to MM we have to be sure we can create 
        //  a PHEAP_UNCOMMMTTED_RANGE later. So we do it right now
        //

        UnCommittedRange = RtlpCreateUnCommittedRange(Segment);

        if (UnCommittedRange == NULL) {
            
            HeapDebugPrint(( "Failing creating uncommitted range (%x for %x)\n", DeCommitAddress, DeCommitSize ));

            //
            //  We weren't successful in the decommit so now simply
            //  add the leading free block to the free list
            //

            RtlpInsertFreeBlock( Heap, LeadingFreeBlock, FreeSize );

            return;
        }

        //
        //  Decommit the memory
        //

        Status = ZwFreeVirtualMemory( (PVOID *)&DeCommitAddress,
                                      &DeCommitSize,
                                      MEM_DECOMMIT );

        //
        //  Push back the UnCommittedRange structure. Now the insert cannot fail
        //

        RtlpDestroyUnCommittedRange( Segment, UnCommittedRange );

        if (NT_SUCCESS( Status )) {

            //
            //  Insert information regarding the pages we just decommitted
            //  to the lsit of uncommited pages in the segment
            //

            RtlpInsertUnCommittedPages( Segment,
                                        DeCommitAddress,
                                        DeCommitSize );
            //
            //  Adjust the segments count of uncommitted pages
            //

            Segment->NumberOfUnCommittedPages += (ULONG)(DeCommitSize / PAGE_SIZE);

            //
            //  If we have a leading free block then mark its proper state
            //  update the heap, and put it on the free list
            //

            if (LeadingFreeSize != 0) {

                LeadingFreeBlock->Flags = HEAP_ENTRY_LAST_ENTRY;
                LeadingFreeBlock->Size = LeadingFreeSize;
                Heap->TotalFreeSize += LeadingFreeSize;

                Segment->LastEntryInSegment = (PHEAP_ENTRY)LeadingFreeBlock;

                RtlpInsertFreeBlockDirect( Heap, LeadingFreeBlock, LeadingFreeSize );

            //
            //  Otherwise if we actually have a leading busy block then
            //  make sure the busy block knows we're uncommitted
            //

            } else if (LeadingBusyBlock != NULL) {

                LeadingBusyBlock->Flags |= HEAP_ENTRY_LAST_ENTRY;

                Segment->LastEntryInSegment = LeadingBusyBlock;

            } else if ((Segment->LastEntryInSegment >= (PHEAP_ENTRY)DeCommitAddress)
                            &&
                       ((PCHAR)Segment->LastEntryInSegment < ((PCHAR)DeCommitAddress + DeCommitSize))) {

                     Segment->LastEntryInSegment = Segment->FirstEntry;
            }

            //
            //  If there is a trailing free block then sets its state,
            //  update the heap, and insert it on a free list
            //

            if (TrailingFreeSize != 0) {

                TrailingFreeBlock->PreviousSize = 0;
                TrailingFreeBlock->SegmentIndex = Segment->Entry.SegmentIndex;
                TrailingFreeBlock->Flags = 0;
                TrailingFreeBlock->Size = TrailingFreeSize;

                ((PHEAP_FREE_ENTRY)((PHEAP_ENTRY)TrailingFreeBlock + TrailingFreeSize))->PreviousSize = (USHORT)TrailingFreeSize;

                RtlpInsertFreeBlockDirect( Heap, TrailingFreeBlock, TrailingFreeSize );

                Heap->TotalFreeSize += TrailingFreeSize;

            //
            //  Otherwise if we actually have a succeeding block then
            //  make it know we are uncommitted
            //

            } else if (TrailingBusyBlock != NULL) {

                TrailingBusyBlock->PreviousSize = 0;
            }

        } else {

            //
            //  We weren't successful in the decommit so now simply
            //  add the leading free block to the free list
            //

            RtlpInsertFreeBlock( Heap, LeadingFreeBlock, FreeSize );
        }

    } else {

        //
        //  There is nothing left to decommit to take our leading free block
        //  and put it on a free list
        //

        RtlpInsertFreeBlock( Heap, LeadingFreeBlock, FreeSize );
    }

    //
    //  And return to our caller
    //

    return;
}


//
//  Declared in heappriv.h
//

VOID
RtlpInsertFreeBlock (
    IN PHEAP Heap,
    IN PHEAP_FREE_ENTRY FreeBlock,
    IN SIZE_T FreeSize
    )

/*++

Routine Description:

    This routines take a piece of committed memory and adds to the
    the appropriate free lists for the heap.  If necessary this
    routine will divide up the free block to sizes that fit
    on the free list


Arguments:

    Heap - Supplies a pointer to the owning heap

    FreeBlock - Supplies a pointer to the block being freed

    FreeSize - Supplies the size, in bytes, of the block being freed

Return Value:

    None.

--*/

{
    USHORT PreviousSize, Size;
    UCHAR Flags;
    UCHAR SegmentIndex;
    PHEAP_SEGMENT Segment;

    RTL_PAGED_CODE();

    //
    //  Get the size of the previous block, the index of the segment
    //  containing this block, and the flags specific to the block
    //

    PreviousSize = FreeBlock->PreviousSize;

    SegmentIndex = FreeBlock->SegmentIndex;
    Segment = Heap->Segments[ SegmentIndex ];

    Flags = FreeBlock->Flags;

    //
    //  Adjust the total amount free in the heap
    //

    Heap->TotalFreeSize += FreeSize;

    //
    //  Now, while there is still something left to add to the free list
    //  we'll process the information
    //

    while (FreeSize != 0) {

        //
        //  If the size is too big for our free lists then we'll
        //  chop it down.
        //

        if (FreeSize > (ULONG)HEAP_MAXIMUM_BLOCK_SIZE) {

            Size = HEAP_MAXIMUM_BLOCK_SIZE;

            //
            //  This little adjustment is so that we don't have a remainder
            //  that is too small to be useful on the next iteration
            //  through the loop
            //

            if (FreeSize == ((ULONG)HEAP_MAXIMUM_BLOCK_SIZE + 1)) {

                Size -= 16;
            }

            //
            //  Guarantee that Last entry does not get set in this
            //  block.
            //

            FreeBlock->Flags = 0;

        } else {

            Size = (USHORT)FreeSize;

            //
            //  This could propagate the last entry flag
            //

            FreeBlock->Flags = Flags;
        }

        //
        //  Update the block sizes and then insert this
        //  block into a free list
        //

        FreeBlock->PreviousSize = PreviousSize;
        FreeBlock->SegmentIndex = SegmentIndex;
        FreeBlock->Size = Size;

        RtlpInsertFreeBlockDirect( Heap, FreeBlock, Size );

        //
        //  Note the size of what we just freed, and then update
        //  our state information for the next time through the
        //  loop
        //

        PreviousSize = Size;

        FreeSize -= Size;
        FreeBlock = (PHEAP_FREE_ENTRY)((PHEAP_ENTRY)FreeBlock + Size);

        //
        //  Check if we're done with the free block based on the
        //  segment information, otherwise go back up and check size
        //  Note that is means that we can get called with a very
        //  large size and still work.
        //

        if ((PHEAP_ENTRY)FreeBlock >= Segment->LastValidEntry) {

            return;
        }
    }

    //
    //  If the block we're freeing did not think it was the last entry
    //  then tell the next block our real size.
    //

    if (!(Flags & HEAP_ENTRY_LAST_ENTRY)) {

        FreeBlock->PreviousSize = PreviousSize;
    }

    //
    //  And return to our caller
    //

    return;
}


//
//  Declared in heappriv.h
//

PHEAP_ENTRY_EXTRA
RtlpGetExtraStuffPointer (
    PHEAP_ENTRY BusyBlock
    )

/*++

Routine Description:

    This routine calculates where the extra stuff record will be given
    the busy block and returns a pointer to it.  The caller must have
    already checked that the entry extry field is present

Arguments:

    BusyBlock - Supplies the busy block whose extra stuff we are seeking

Return Value:

    PHEAP_ENTRY_EXTRA - returns a pointer to the extra stuff record.

--*/

{
    ULONG AllocationIndex;

    //
    //  On big blocks the extra stuff is automatically part of the
    //  block
    //

    if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

        PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

        VirtualAllocBlock = CONTAINING_RECORD( BusyBlock, HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

        return &VirtualAllocBlock->ExtraStuff;

    } else {

        //
        //  On non big blocks the extra stuff follows immediately after
        //  the allocation itself.
        //
        //  **** What a hack
        //  **** We do some funny math here because the busy block
        //  **** stride is 8 bytes we know we can stride it by its
        //  **** index minus one to get to the end of the allocation
        //

        AllocationIndex = BusyBlock->Size;

        return (PHEAP_ENTRY_EXTRA)(BusyBlock + AllocationIndex - 1);
    }
}


//
//  Declared in heappriv.h
//

SIZE_T
RtlpGetSizeOfBigBlock (
    IN PHEAP_ENTRY BusyBlock
    )

/*++

Routine Description:

    This routine returns the size, in bytes, of the big allocation block

Arguments:

    BusyBlock - Supplies a pointer to the block being queried

Return Value:

    SIZE_T - Returns the size, in bytes, that was allocated to the big
        block

--*/

{
    PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

    RTL_PAGED_CODE();

    //
    //  Get a pointer to the block header itself
    //

    VirtualAllocBlock = CONTAINING_RECORD( BusyBlock, HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

    //
    //  The size allocated to the block is actually the difference between the
    //  commit size stored in the virtual alloc block and the size stored in
    //  in the block.
    //

    return VirtualAllocBlock->CommitSize - BusyBlock->Size;
}


//
//  Declared in heappriv.h
//

BOOLEAN
RtlpCheckBusyBlockTail (
    IN PHEAP_ENTRY BusyBlock
    )

/*++

Routine Description:

    This routine checks to see if the bytes beyond the user specified
    allocation have been modified.  It does this by checking for a tail
    fill pattern

Arguments:

    BusyBlock - Supplies the heap block being queried

Return Value:

    BOOLEAN - TRUE if the tail is still okay and FALSE otherwise

--*/

{
    PCHAR Tail;
    SIZE_T Size, cbEqual;

    RTL_PAGED_CODE();

    //
    //  Compute the user allocated size of the input heap block
    //

    if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

        Size = RtlpGetSizeOfBigBlock( BusyBlock );

    } else {

        Size = (BusyBlock->Size << HEAP_GRANULARITY_SHIFT) - BusyBlock->UnusedBytes;
    }

    //
    //  Compute a pointer to the tail of the input block.  This would
    //  be the space right after the user allocated portion
    //

    Tail = (PCHAR)(BusyBlock + 1) + Size;

    //
    //  Check if the tail fill pattern is still there
    //

    cbEqual = RtlCompareMemory( Tail,
                                CheckHeapFillPattern,
                                CHECK_HEAP_TAIL_SIZE );

    //
    //  If the number we get back isn't equal to the tail size then
    //  someone modified the block beyond its user specified allocation
    //  size
    //

    if (cbEqual != CHECK_HEAP_TAIL_SIZE) {

        //
        //  Do some debug printing
        //

        HeapDebugPrint(( "Heap block at %p modified at %p past requested size of %lx\n",
                         BusyBlock,
                         Tail + cbEqual,
                         Size ));

        HeapDebugBreak( BusyBlock );

        //
        //  And tell our caller there was an error
        //

        return FALSE;

    } else {

        //
        //  And return to our caller that the tail is fine
        //

        return TRUE;
    }
}

#if DBG

BOOLEAN RtlpValidateHeapHdrsEnable = FALSE; // Set to TRUE if headers are being corrupted
BOOLEAN RtlpValidateHeapTagsEnable;         // Set to TRUE if tag counts are off and you want to know why

HEAP_STOP_ON_VALUES RtlpHeapStopOn;


const struct {

    ULONG Offset;
    LPSTR Description;

} RtlpHeapHeaderFieldOffsets[] = {

    FIELD_OFFSET( HEAP, Entry ),                        "Entry",
    FIELD_OFFSET( HEAP, Signature ),                    "Signature",
    FIELD_OFFSET( HEAP, Flags ),                        "Flags",
    FIELD_OFFSET( HEAP, ForceFlags ),                   "ForceFlags",
    FIELD_OFFSET( HEAP, VirtualMemoryThreshold ),       "VirtualMemoryThreshold",
    FIELD_OFFSET( HEAP, SegmentReserve ),               "SegmentReserve",
    FIELD_OFFSET( HEAP, SegmentCommit ),                "SegmentCommit",
    FIELD_OFFSET( HEAP, DeCommitFreeBlockThreshold ),   "DeCommitFreeBlockThreshold",
    FIELD_OFFSET( HEAP, DeCommitTotalFreeThreshold ),   "DeCommitTotalFreeThreshold",
    FIELD_OFFSET( HEAP, TotalFreeSize ),                "TotalFreeSize",
    FIELD_OFFSET( HEAP, MaximumAllocationSize ),        "MaximumAllocationSize",
    FIELD_OFFSET( HEAP, ProcessHeapsListIndex ),        "ProcessHeapsListIndex",
    FIELD_OFFSET( HEAP, HeaderValidateLength ),         "HeaderValidateLength",
    FIELD_OFFSET( HEAP, HeaderValidateCopy ),           "HeaderValidateCopy",
    FIELD_OFFSET( HEAP, NextAvailableTagIndex ),        "NextAvailableTagIndex",
    FIELD_OFFSET( HEAP, MaximumTagIndex ),              "MaximumTagIndex",
    FIELD_OFFSET( HEAP, TagEntries ),                   "TagEntries",
    FIELD_OFFSET( HEAP, UCRSegments ),                  "UCRSegments",
    FIELD_OFFSET( HEAP, UnusedUnCommittedRanges ),      "UnusedUnCommittedRanges",
    FIELD_OFFSET( HEAP, AlignRound ),                   "AlignRound",
    FIELD_OFFSET( HEAP, AlignMask ),                    "AlignMask",
    FIELD_OFFSET( HEAP, VirtualAllocdBlocks ),          "VirtualAllocdBlocks",
    FIELD_OFFSET( HEAP, Segments ),                     "Segments",
    FIELD_OFFSET( HEAP, u ),                            "FreeListsInUse",
    FIELD_OFFSET( HEAP, FreeListsInUseTerminate ),      "FreeListsInUseTerminate",
    FIELD_OFFSET( HEAP, AllocatorBackTraceIndex ),      "AllocatorBackTraceIndex",
    FIELD_OFFSET( HEAP, Reserved1 ),                    "Reserved1",
    FIELD_OFFSET( HEAP, PseudoTagEntries ),             "PseudoTagEntries",
    FIELD_OFFSET( HEAP, FreeLists ),                    "FreeLists",
    FIELD_OFFSET( HEAP, LockVariable ),                 "LockVariable",
    FIELD_OFFSET( HEAP, Lookaside ),                    "Lookaside",
    FIELD_OFFSET( HEAP, LookasideLockCount ),           "LookasideLockCount",
    sizeof( HEAP ),                                     "Uncommitted Ranges",
    0xFFFF, NULL
};

//
//  Declared in heappriv.h
//

BOOLEAN
RtlpCheckHeapSignature (
    IN PHEAP Heap,
    IN PCHAR Caller
    )

/*++

Routine Description:

    This routine verifies that it is being called with a properly identified
    heap.

Arguments:

    Heap - Supplies a pointer to the heap being checked

    Caller - Supplies a string that can be used to identify the caller

Return Value:

    BOOLEAN - TRUE if the heap signature is present, and FALSE otherwise

--*/

{
    //
    //  If the heap signature matches then that is the only
    //  checking we do
    //

    if (Heap->Signature == HEAP_SIGNATURE) {

        return TRUE;

    } else {

        //
        //  We have a bad heap signature.  Print out some information, break
        //  into the debugger, and then return false
        //

        HeapDebugPrint(( "Invalid heap signature for heap at %x", Heap ));

        if (Caller != NULL) {

            DbgPrint( ", passed to %s", Caller );
        }

        DbgPrint( "\n" );

        HeapDebugBreak( &Heap->Signature );

        return FALSE;
    }
}



BOOLEAN
RtlpValidateHeapHeaders (
    IN PHEAP Heap,
    IN BOOLEAN Recompute
    )

/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    ULONG i;
    SIZE_T n;
    SIZE_T nEqual;
    NTSTATUS Status;

    if (!RtlpValidateHeapHdrsEnable) {

        return TRUE;
    }

    if (Heap->HeaderValidateCopy == NULL) {

        n = Heap->HeaderValidateLength;

        Status = NtAllocateVirtualMemory( &Heap->HeaderValidateCopy,
                                          0,
                                          &n,
                                          MEM_COMMIT,
                                          PAGE_READWRITE );

        if (!NT_SUCCESS( Status )) {

            return TRUE;
        }

        Recompute = TRUE;
    }

    n = Heap->HeaderValidateLength;

    if (!Recompute) {

        nEqual = RtlCompareMemory( Heap,
                                   Heap->HeaderValidateCopy,
                                   n );

    } else {

        RtlMoveMemory( Heap->HeaderValidateCopy,
                       Heap,
                       n );

        nEqual = n;
    }

    if (n != nEqual) {

        HeapDebugPrint(( "Heap %x - headers modified (%x is %x instead of %x)\n",
                         Heap,
                         (PCHAR)Heap + nEqual,
                         *(PULONG)((PCHAR)Heap + nEqual),
                         *(PULONG)((PCHAR)Heap->HeaderValidateCopy + nEqual)));

        for (i=0; RtlpHeapHeaderFieldOffsets[ i ].Description != NULL; i++) {

            if ((nEqual >= RtlpHeapHeaderFieldOffsets[ i ].Offset) &&
                (nEqual < RtlpHeapHeaderFieldOffsets[ i+1 ].Offset)) {

                DbgPrint( "    This is located in the %s field of the heap header.\n",
                                 RtlpHeapHeaderFieldOffsets[ i ].Description );

                break;
            }
        }

        return FALSE;

    } else {

        return TRUE;
    }
}



BOOLEAN
RtlpValidateHeapSegment (
    IN PHEAP Heap,
    IN PHEAP_SEGMENT Segment,
    IN UCHAR SegmentIndex,
    IN OUT PULONG CountOfFreeBlocks,
    IN OUT PSIZE_T TotalFreeSize,
    OUT PVOID *BadAddress,
    IN OUT PSIZE_T ComputedTagEntries,
    IN OUT PSIZE_T ComputedPseudoTagEntries
    )

/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    PHEAP_ENTRY CurrentBlock, PreviousBlock;
    SIZE_T Size;
    USHORT PreviousSize, TagIndex;
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange;
    PHEAP_ENTRY_EXTRA ExtraStuff;
    ULONG NumberOfUnCommittedPages;
    ULONG NumberOfUnCommittedRanges;

    RTL_PAGED_CODE();

    NumberOfUnCommittedPages = 0;
    NumberOfUnCommittedRanges = 0;

    UnCommittedRange = Segment->UnCommittedRanges;

    if (Segment->BaseAddress == Heap) {

        CurrentBlock = &Heap->Entry;

    } else {

        CurrentBlock = &Segment->Entry;
    }

    while (CurrentBlock < Segment->LastValidEntry) {

        *BadAddress = CurrentBlock;

        if ((UnCommittedRange != NULL) &&
            ((ULONG_PTR)CurrentBlock >= UnCommittedRange->Address)) {

            HeapDebugPrint(( "Heap entry %lx is beyond uncommited range [%x .. %x)\n",
                             CurrentBlock,
                             UnCommittedRange->Address,
                             (PCHAR)UnCommittedRange->Address + UnCommittedRange->Size ));

            return FALSE;
        }

        PreviousSize = 0;

        while (CurrentBlock < Segment->LastValidEntry) {

            *BadAddress = CurrentBlock;

            if (PreviousSize != CurrentBlock->PreviousSize) {

                HeapDebugPrint(( "Heap entry %lx has incorrect PreviousSize field (%04x instead of %04x)\n",
                                 CurrentBlock, CurrentBlock->PreviousSize, PreviousSize ));

                return FALSE;
            }

            PreviousSize = CurrentBlock->Size;
            Size = (ULONG_PTR)CurrentBlock->Size << HEAP_GRANULARITY_SHIFT;

            if (CurrentBlock->Flags & HEAP_ENTRY_BUSY) {

                if (ComputedTagEntries != NULL) {

                    if (CurrentBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                        ExtraStuff = RtlpGetExtraStuffPointer( CurrentBlock );
                        TagIndex = ExtraStuff->TagIndex;

                    } else {

                        TagIndex = CurrentBlock->SmallTagIndex;
                    }

                    if (TagIndex != 0) {

                        if (TagIndex & HEAP_PSEUDO_TAG_FLAG) {

                            TagIndex &= ~HEAP_PSEUDO_TAG_FLAG;

                            if (TagIndex < HEAP_NUMBER_OF_PSEUDO_TAG) {

                                ComputedPseudoTagEntries[ TagIndex ] += CurrentBlock->Size;
                            }

                        } else if (TagIndex & HEAP_GLOBAL_TAG) {

                            //
                            //  Ignore these since they are global across more than
                            //  one heap.
                            //

                        } else if (TagIndex < Heap->NextAvailableTagIndex) {

                            ComputedTagEntries[ TagIndex ] += CurrentBlock->Size;
                        }
                    }
                }

                if (CurrentBlock->Flags & HEAP_ENTRY_FILL_PATTERN) {

                    if (!RtlpCheckBusyBlockTail( CurrentBlock )) {

                        return FALSE;
                    }
                }

            } else {

                *CountOfFreeBlocks += 1;
                *TotalFreeSize += CurrentBlock->Size;

                if ((Heap->Flags & HEAP_FREE_CHECKING_ENABLED) &&
                    (CurrentBlock->Flags & HEAP_ENTRY_FILL_PATTERN)) {

                    SIZE_T cb, cbEqual;

                    cb = Size - sizeof( HEAP_FREE_ENTRY );

                    if ((CurrentBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) &&
                        (cb > sizeof( HEAP_FREE_ENTRY_EXTRA ))) {

                        cb -= sizeof( HEAP_FREE_ENTRY_EXTRA );
                    }

                    cbEqual = RtlCompareMemoryUlong( (PCHAR)((PHEAP_FREE_ENTRY)CurrentBlock + 1),
                                                     cb,
                                                     FREE_HEAP_FILL );

                    if (cbEqual != cb) {

                        HeapDebugPrint(( "Free Heap block %lx modified at %lx after it was freed\n",
                                         CurrentBlock,
                                         (PCHAR)(CurrentBlock + 1) + cbEqual ));

                        return FALSE;
                    }
                }
            }

            if (CurrentBlock->SegmentIndex != SegmentIndex) {

                HeapDebugPrint(( "Heap block at %lx has incorrect segment index (%x)\n",
                                 CurrentBlock,
                                 SegmentIndex ));

                return FALSE;
            }

            if (CurrentBlock->Flags & HEAP_ENTRY_LAST_ENTRY) {

                CurrentBlock = (PHEAP_ENTRY)((PCHAR)CurrentBlock + Size);

                if (UnCommittedRange == NULL) {

                    if (CurrentBlock != Segment->LastValidEntry) {

                        HeapDebugPrint(( "Heap block at %lx is not last block in segment (%x)\n",
                                         CurrentBlock,
                                         Segment->LastValidEntry ));

                        return FALSE;
                    }

                } else if ((ULONG_PTR)CurrentBlock != UnCommittedRange->Address) {

                    HeapDebugPrint(( "Heap block at %lx does not match address of next uncommitted address (%x)\n",
                                     CurrentBlock,
                                     UnCommittedRange->Address ));

                    return FALSE;

                } else {

                    NumberOfUnCommittedPages += (ULONG) (UnCommittedRange->Size / PAGE_SIZE);
                    NumberOfUnCommittedRanges += 1;

                    CurrentBlock = (PHEAP_ENTRY)
                        ((PCHAR)UnCommittedRange->Address + UnCommittedRange->Size);

                    UnCommittedRange = UnCommittedRange->Next;
                }

                break;
            }

            CurrentBlock = (PHEAP_ENTRY)((PCHAR)CurrentBlock + Size);
        }
    }

    *BadAddress = Segment;

    if (Segment->NumberOfUnCommittedPages != NumberOfUnCommittedPages) {

        HeapDebugPrint(( "Heap Segment at %lx contains invalid NumberOfUnCommittedPages (%x != %x)\n",
                         Segment,
                         Segment->NumberOfUnCommittedPages,
                         NumberOfUnCommittedPages ));

        return FALSE;
    }

    if (Segment->NumberOfUnCommittedRanges != NumberOfUnCommittedRanges) {

        HeapDebugPrint(( "Heap Segment at %lx contains invalid NumberOfUnCommittedRanges (%x != %x)\n",
                         Segment,
                         Segment->NumberOfUnCommittedRanges,
                         NumberOfUnCommittedRanges ));

        return FALSE;
    }

    return TRUE;
}



BOOLEAN
RtlpValidateHeapEntry (
    IN PHEAP Heap,
    IN PHEAP_ENTRY BusyBlock,
    IN PCHAR Reason
    )

/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    PHEAP_SEGMENT Segment;
    UCHAR SegmentIndex;
    BOOLEAN Result;

    if ((BusyBlock == NULL)

            ||

        ((ULONG_PTR)BusyBlock & (HEAP_GRANULARITY-1))

            ||

        ((BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) &&
         ((ULONG_PTR)BusyBlock & (PAGE_SIZE-1)) != FIELD_OFFSET( HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock ))

            ||

        (!(BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) &&
         ((BusyBlock->SegmentIndex >= HEAP_MAXIMUM_SEGMENTS) ||
          !(Segment = Heap->Segments[ BusyBlock->SegmentIndex ]) ||
          (BusyBlock < Segment->FirstEntry) ||
          (BusyBlock >= Segment->LastValidEntry)))

            ||

        !(BusyBlock->Flags & HEAP_ENTRY_BUSY)

            ||

        ((BusyBlock->Flags & HEAP_ENTRY_FILL_PATTERN) && !RtlpCheckBusyBlockTail( BusyBlock ))) {

InvalidBlock:

        HeapDebugPrint(( "Invalid Address specified to %s( %lx, %lx )\n",
                         Reason,
                         Heap,
                         BusyBlock + 1 ));

        HeapDebugBreak( BusyBlock );

        return FALSE;

    } else {

        if (BusyBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

            Result = TRUE;

        } else {

            for (SegmentIndex=0; SegmentIndex<HEAP_MAXIMUM_SEGMENTS; SegmentIndex++) {

                Segment = Heap->Segments[ SegmentIndex ];

                if (Segment) {

                    if ((BusyBlock >= Segment->FirstEntry) &&
                        (BusyBlock < Segment->LastValidEntry)) {

                        Result = TRUE;
                        break;
                    }
                }
            }
        }

        if (!Result) {

            goto InvalidBlock;
        }

        return TRUE;
    }
}



BOOLEAN
RtlpValidateHeap (
    IN PHEAP Heap,
    IN BOOLEAN AlwaysValidate
    )

/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    NTSTATUS Status;
    PHEAP_SEGMENT Segment;
    PLIST_ENTRY Head, Next;
    PHEAP_FREE_ENTRY FreeBlock;
    BOOLEAN EmptyFreeList;
    ULONG NumberOfFreeListEntries;
    ULONG CountOfFreeBlocks;
    SIZE_T TotalFreeSize;
    SIZE_T Size;
    USHORT PreviousSize;
    UCHAR SegmentIndex;
    PVOID BadAddress;
    PSIZE_T ComputedTagEntries = NULL;
    PSIZE_T ComputedPseudoTagEntries = NULL;
    PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;
    USHORT TagIndex;

    RTL_PAGED_CODE();

    BadAddress = Heap;

    if (!RtlpValidateHeapHeaders( Heap, FALSE )) {

        goto errorExit;
    }

    if (!AlwaysValidate && !(Heap->Flags & HEAP_VALIDATE_ALL_ENABLED)) {

        goto exit;
    }

    NumberOfFreeListEntries = 0;
    Head = &Heap->FreeLists[ 0 ];

    for (Size = 0; Size < HEAP_MAXIMUM_FREELISTS; Size++) {

        if (Size != 0) {

            EmptyFreeList = (BOOLEAN)(IsListEmpty( Head ));
            BadAddress = &Heap->u.FreeListsInUseBytes[ Size / 8 ];

            if (Heap->u.FreeListsInUseBytes[ Size / 8 ] & (1 << (Size & 7)) ) {

                if (EmptyFreeList) {

                    HeapDebugPrint(( "dedicated (%04x) free list empty but marked as non-empty\n",
                                     Size ));

                    goto errorExit;
                }

            } else {

                if (!EmptyFreeList) {

                    HeapDebugPrint(( "dedicated (%04x) free list non-empty but marked as empty\n",
                                     Size ));

                    goto errorExit;
                }
            }
        }

        Next = Head->Flink;
        PreviousSize = 0;

        while (Head != Next) {

            FreeBlock = CONTAINING_RECORD( Next, HEAP_FREE_ENTRY, FreeList );
            Next = Next->Flink;

            BadAddress = FreeBlock;

            if (FreeBlock->Flags & HEAP_ENTRY_BUSY) {

                HeapDebugPrint(( "dedicated (%04x) free list element %lx is marked busy\n",
                                 Size,
                                 FreeBlock ));

                goto errorExit;
            }

            if ((Size != 0) && (FreeBlock->Size != Size)) {

                HeapDebugPrint(( "Dedicated (%04x) free list element %lx is wrong size (%04x)\n",
                                 Size,
                                 FreeBlock,
                                 FreeBlock->Size ));

                goto errorExit;

            } else if ((Size == 0) && (FreeBlock->Size < HEAP_MAXIMUM_FREELISTS)) {

                HeapDebugPrint(( "Non-Dedicated free list element %lx with too small size (%04x)\n",
                                 FreeBlock,
                                 FreeBlock->Size ));

                goto errorExit;

            } else if ((Size == 0) && (FreeBlock->Size < PreviousSize)) {

                HeapDebugPrint(( "Non-Dedicated free list element %lx is out of order\n",
                                 FreeBlock ));

                goto errorExit;

            } else {

                PreviousSize = FreeBlock->Size;
            }

            NumberOfFreeListEntries++;
        }

        Head++;
    }

    Size = (HEAP_NUMBER_OF_PSEUDO_TAG + Heap->NextAvailableTagIndex + 1) * sizeof( SIZE_T );

    if ((RtlpValidateHeapTagsEnable) && (Heap->PseudoTagEntries != NULL)) {

        Status = NtAllocateVirtualMemory( &ComputedPseudoTagEntries,
                                          0,
                                          &Size,
                                          MEM_COMMIT,
                                          PAGE_READWRITE );

        if (NT_SUCCESS( Status )) {

            ComputedTagEntries = ComputedPseudoTagEntries + HEAP_NUMBER_OF_PSEUDO_TAG;
        }
    }

    Head = &Heap->VirtualAllocdBlocks;
    Next = Head->Flink;

    while (Head != Next) {

        VirtualAllocBlock = CONTAINING_RECORD( Next, HEAP_VIRTUAL_ALLOC_ENTRY, Entry );

        if (ComputedTagEntries != NULL) {

            TagIndex = VirtualAllocBlock->ExtraStuff.TagIndex;

            if (TagIndex != 0) {

                if (TagIndex & HEAP_PSEUDO_TAG_FLAG) {

                    TagIndex &= ~HEAP_PSEUDO_TAG_FLAG;

                    if (TagIndex < HEAP_NUMBER_OF_PSEUDO_TAG) {

                        ComputedPseudoTagEntries[ TagIndex ] +=
                            VirtualAllocBlock->CommitSize >> HEAP_GRANULARITY_SHIFT;
                    }

                } else if (TagIndex & HEAP_GLOBAL_TAG) {

                    //
                    //  Ignore these since they are global across more than
                    //  one heap.
                    //

                } else if (TagIndex < Heap->NextAvailableTagIndex) {

                    ComputedTagEntries[ TagIndex ] +=
                        VirtualAllocBlock->CommitSize >> HEAP_GRANULARITY_SHIFT;
                }
            }
        }

        if (VirtualAllocBlock->BusyBlock.Flags & HEAP_ENTRY_FILL_PATTERN) {

            if (!RtlpCheckBusyBlockTail( &VirtualAllocBlock->BusyBlock )) {

                return FALSE;
            }
        }

        Next = Next->Flink;
    }

    CountOfFreeBlocks = 0;
    TotalFreeSize = 0;

    for (SegmentIndex=0; SegmentIndex<HEAP_MAXIMUM_SEGMENTS; SegmentIndex++) {

        Segment = Heap->Segments[ SegmentIndex ];

        if (Segment) {

            if (!RtlpValidateHeapSegment( Heap,
                                          Segment,
                                          SegmentIndex,
                                          &CountOfFreeBlocks,
                                          &TotalFreeSize,
                                          &BadAddress,
                                          ComputedTagEntries,
                                          ComputedPseudoTagEntries )) {

                goto errorExit;
            }
        }
    }

    BadAddress = Heap;

    if (NumberOfFreeListEntries != CountOfFreeBlocks) {

        HeapDebugPrint(( "Number of free blocks in arena (%ld) does not match number in the free lists (%ld)\n",
                         CountOfFreeBlocks,
                         NumberOfFreeListEntries ));

        goto errorExit;
    }

    if (Heap->TotalFreeSize != TotalFreeSize) {

        HeapDebugPrint(( "Total size of free blocks in arena (%ld) does not match number total in heap header (%ld)\n",
                         TotalFreeSize,
                         Heap->TotalFreeSize ));

        goto errorExit;
    }

    if (ComputedPseudoTagEntries != NULL) {

        PHEAP_PSEUDO_TAG_ENTRY PseudoTagEntries;
        PHEAP_TAG_ENTRY TagEntries;
        USHORT TagIndex;

        PseudoTagEntries = Heap->PseudoTagEntries;

        if (PseudoTagEntries != NULL) {

            for (TagIndex=1; TagIndex<HEAP_NUMBER_OF_PSEUDO_TAG; TagIndex++) {

                PseudoTagEntries += 1;

                if (ComputedPseudoTagEntries[ TagIndex ] != PseudoTagEntries->Size) {

                    HeapDebugPrint(( "Pseudo Tag %04x size incorrect (%x != %x) %x\n",
                                     TagIndex,
                                     PseudoTagEntries->Size,
                                     ComputedPseudoTagEntries[ TagIndex ]
                                     &ComputedPseudoTagEntries[ TagIndex ] ));

                    goto errorExit;
                }
            }
        }

        TagEntries = Heap->TagEntries;

        if (TagEntries != NULL) {

            for (TagIndex=1; TagIndex<Heap->NextAvailableTagIndex; TagIndex++) {

                TagEntries += 1;

                if (ComputedTagEntries[ TagIndex ] != TagEntries->Size) {

                    HeapDebugPrint(( "Tag %04x (%ws) size incorrect (%x != %x) %x\n",
                                     TagIndex,
                                     TagEntries->TagName,
                                     TagEntries->Size,
                                     ComputedTagEntries[ TagIndex ],
                                     &ComputedTagEntries[ TagIndex ] ));

                    goto errorExit;
                }
            }
        }

        Size = 0;

        NtFreeVirtualMemory( &ComputedPseudoTagEntries,
                             &Size,
                             MEM_RELEASE );
    }

exit:

    return TRUE;

errorExit:

    HeapDebugBreak( BadAddress );

    if (ComputedPseudoTagEntries != NULL) {

        Size = 0;

        NtFreeVirtualMemory( &ComputedPseudoTagEntries,
                             &Size,
                             MEM_RELEASE );
    }

    return FALSE;

}



BOOLEAN
RtlDebugWalkHeap (
    IN PVOID HeapHandle,
    IN OUT PRTL_HEAP_WALK_ENTRY Entry
    )

/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    BOOLEAN Result;

    //
    //  Assumed the caller has serialized via RtlLockHeap or their own locking mechanism.
    //

    Result = FALSE;

    try {

        if (RtlpCheckHeapSignature( Heap, "RtlWalkHeap" )) {

            Result = RtlpValidateHeap( Heap, FALSE );
        }

    } except( EXCEPTION_EXECUTE_HANDLER ) {

        SET_LAST_STATUS( GetExceptionCode() );
    }

    return Result;
}



//
//  Declared in nturtl.h
//

NTSTATUS
RtlWalkHeap (
    IN PVOID HeapHandle,
    IN OUT PRTL_HEAP_WALK_ENTRY Entry
    )

/*++

Routine Description:

    This routine is used to enumerate all the entries within a heap.  For each
    call it returns a new information in entry.

Arguments:

    HeapHandle - Supplies a pointer to the heap being queried

    Entry - Supplies storage for the entry information.  If the DataAddress field
        is null then the enumeration starts over from the beginning otherwise it
        resumes from where it left off

Return Value:

    NTSTATUS - An appropriate status value

--*/

{
    NTSTATUS Status;
    PHEAP Heap = (PHEAP)HeapHandle;
    PHEAP_SEGMENT Segment;
    UCHAR SegmentIndex;
    PHEAP_ENTRY CurrentBlock;
    PHEAP_ENTRY_EXTRA ExtraStuff;
    PHEAP_UNCOMMMTTED_RANGE UnCommittedRange, *pp;
    PLIST_ENTRY Next, Head;
    PHEAP_VIRTUAL_ALLOC_ENTRY VirtualAllocBlock;

#if 0
    //
    //  Check if we should be using the guard page verion of heap
    //

    IF_DEBUG_PAGE_HEAP_THEN_RETURN( HeapHandle,
                                    RtlpDebugPageHeapWalk( HeapHandle, Entry ));
                                    
#endif // 0

    //
    //  If this is the debug version of heap then validate the heap
    //  before we go on
    //

    if (DEBUG_HEAP( Heap->Flags )) {

        if (!RtlDebugWalkHeap( HeapHandle, Entry )) {

            return STATUS_INVALID_PARAMETER;
        }
    }

    Status = STATUS_SUCCESS;

#ifndef NTOS_KERNEL_RUNTIME

    //
    //  If there is an active lookaside list then drain and remove it.
    //  By setting the lookaside field in the heap to null we guarantee
    //  that the call the free heap will not try and use the lookaside
    //  list logic.
    //
    //  We'll actually capture the lookaside pointer from the heap and
    //  only use the captured pointer.  This will take care of the
    //  condition where another walk or lock heap can cause us to check
    //  for a non null pointer and then have it become null when we read
    //  it again.  If it is non null to start with then even if the
    //  user walks or locks the heap via another thread the pointer to
    //  still valid here so we can still try and do a lookaside list pop.
    //

    {
        PHEAP_LOOKASIDE Lookaside = (PHEAP_LOOKASIDE)Heap->Lookaside;

        if (Lookaside != NULL) {

            ULONG i;
            PVOID Block;

            Heap->Lookaside = NULL;

            for (i = 0; i < HEAP_MAXIMUM_FREELISTS; i += 1) {

                while ((Block = RtlpAllocateFromHeapLookaside(&(Lookaside[i]))) != NULL) {

                    RtlFreeHeap( HeapHandle, 0, Block );
                }
            }
        }
    }
    
#endif // NTOS_KERNEL_RUNTIME

    //
    //  Check if this is the first time we've been called to walk the heap
    //

    if (Entry->DataAddress == NULL) {

        //
        //  Start with the first segement in the heap
        //

        SegmentIndex = 0;

nextSegment:

        CurrentBlock = NULL;

        //
        //  Now find the next in use segment for the heap
        //

        Segment = NULL;

        while ((SegmentIndex < HEAP_MAXIMUM_SEGMENTS) &&
               ((Segment = Heap->Segments[ SegmentIndex ]) == NULL)) {

            SegmentIndex += 1;
        }

        //
        //  If there are no more valid segments then we'll try the big
        //  allocation
        //

        if (Segment == NULL) {

            Head = &Heap->VirtualAllocdBlocks;
            Next = Head->Flink;

            if (Next == Head) {

                Status = STATUS_NO_MORE_ENTRIES;

            } else {

                VirtualAllocBlock = CONTAINING_RECORD( Next, HEAP_VIRTUAL_ALLOC_ENTRY, Entry );

                CurrentBlock = &VirtualAllocBlock->BusyBlock;
            }

        //
        //  Otherwise we'll grab information about the segment.  Note that
        //  the current block is still null so when we fall out of this
        //  block we'll return directly to our caller with this segment
        //  information
        //

        } else {

            Entry->DataAddress = Segment;

            Entry->DataSize = 0;

            Entry->OverheadBytes = sizeof( *Segment );

            Entry->Flags = RTL_HEAP_SEGMENT;

            Entry->SegmentIndex = SegmentIndex;

            Entry->Segment.CommittedSize = (Segment->NumberOfPages -
                                            Segment->NumberOfUnCommittedPages) * PAGE_SIZE;

            Entry->Segment.UnCommittedSize = Segment->NumberOfUnCommittedPages * PAGE_SIZE;

            Entry->Segment.FirstEntry = (Segment->FirstEntry->Flags & HEAP_ENTRY_BUSY) ?
                ((PHEAP_ENTRY)Segment->FirstEntry + 1) :
                (PHEAP_ENTRY)((PHEAP_FREE_ENTRY)Segment->FirstEntry + 1);

            Entry->Segment.LastEntry = Segment->LastValidEntry;
        }

    //
    //  This is not the first time through.  Check if last time we gave back
    //  an heap segement or an uncommitted range
    //

    } else if (Entry->Flags & (RTL_HEAP_SEGMENT | RTL_HEAP_UNCOMMITTED_RANGE)) {

        //
        //  Check that the segment index is still valid
        //

        if ((SegmentIndex = Entry->SegmentIndex) >= HEAP_MAXIMUM_SEGMENTS) {

            Status = STATUS_INVALID_ADDRESS;

            CurrentBlock = NULL;

        } else {

            //
            //  Check that the segment is still in use
            //

            Segment = Heap->Segments[ SegmentIndex ];

            if (Segment == NULL) {

                Status = STATUS_INVALID_ADDRESS;

                CurrentBlock = NULL;

            //
            //  The segment is still in use if what we returned last time
            //  as the segment header then this time we'll return the
            //  segments first entry
            //

            } else if (Entry->Flags & RTL_HEAP_SEGMENT) {

                CurrentBlock = (PHEAP_ENTRY)Segment->FirstEntry;

            //
            //  Otherwise what we returned last time as an uncommitted
            //  range so now we need to get the next block
            //

            } else {

                CurrentBlock = (PHEAP_ENTRY)((PCHAR)Entry->DataAddress + Entry->DataSize);

                //
                //  Check if we are beyond this segment and need to get the
                //  next one
                //

                if (CurrentBlock >= Segment->LastValidEntry) {

                    SegmentIndex += 1;

                    goto nextSegment;
                }
            }
        }

    //
    //  Otherwise this is not the first time through and last time we gave back a
    //  valid heap entry
    //

    } else {

        //
        //  Check if the last entry we gave back was in use
        //

        if (Entry->Flags & HEAP_ENTRY_BUSY) {

            //
            //  Get the last entry we returned
            //

            CurrentBlock = ((PHEAP_ENTRY)Entry->DataAddress - 1);

            //
            //  If the last entry was for a big allocation then
            //  get the next big block if there is one otherwise
            //  say there are no more entries
            //

            if (CurrentBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

                Head = &Heap->VirtualAllocdBlocks;

                VirtualAllocBlock = CONTAINING_RECORD( CurrentBlock, HEAP_VIRTUAL_ALLOC_ENTRY, BusyBlock );

                Next = VirtualAllocBlock->Entry.Flink;

                if (Next == Head) {

                    Status = STATUS_NO_MORE_ENTRIES;

                } else {

                    VirtualAllocBlock = CONTAINING_RECORD( Next, HEAP_VIRTUAL_ALLOC_ENTRY, Entry );

                    CurrentBlock = &VirtualAllocBlock->BusyBlock;
                }

            //
            //  Our previous result is a busy normal block
            //

            } else {

                //
                //  Get the segment and make sure it it still valid and in use
                //
                //  **** this should also check that segment index is not
                //  **** greater than HEAP MAXIMUM SEGMENTS
                //

                Segment = Heap->Segments[ SegmentIndex = CurrentBlock->SegmentIndex ];

                if (Segment == NULL) {

                    Status = STATUS_INVALID_ADDRESS;

                    CurrentBlock = NULL;

                //
                //  The segment is still in use, check if what we returned
                //  previously was a last entry
                //

                } else if (CurrentBlock->Flags & HEAP_ENTRY_LAST_ENTRY) {

findUncommittedRange:

                    //
                    //  We are at a last entry so now if the segment is done
                    //  then go get another segment
                    //

                    CurrentBlock += CurrentBlock->Size;

                    if (CurrentBlock >= Segment->LastValidEntry) {

                        SegmentIndex += 1;

                        goto nextSegment;
                    }

                    //
                    //  Otherwise we will find the uncommitted range entry that
                    //  immediately follows this last entry
                    //

                    pp = &Segment->UnCommittedRanges;

                    while ((UnCommittedRange = *pp) && UnCommittedRange->Address != (ULONG_PTR)CurrentBlock ) {

                        pp = &UnCommittedRange->Next;
                    }

                    if (UnCommittedRange == NULL) {

                        Status = STATUS_INVALID_PARAMETER;

                    } else {

                        //
                        //  Now fill in the entry to denote that uncommitted
                        //  range information
                        //

                        Entry->DataAddress = (PVOID)UnCommittedRange->Address;

                        Entry->DataSize = UnCommittedRange->Size;

                        Entry->OverheadBytes = 0;

                        Entry->SegmentIndex = SegmentIndex;

                        Entry->Flags = RTL_HEAP_UNCOMMITTED_RANGE;
                    }

                    //
                    //  Null out the current block because we've just filled in
                    //  the entry
                    //

                    CurrentBlock = NULL;

                } else {

                    //
                    //  Otherwise the entry has a following entry so now
                    //  advance to the next entry
                    //

                    CurrentBlock += CurrentBlock->Size;
                }
            }

        //
        //  Otherwise the previous entry we returned is not in use
        //

        } else {

            //
            //  Get the last entry we returned
            //

            CurrentBlock = (PHEAP_ENTRY)((PHEAP_FREE_ENTRY)Entry->DataAddress - 1);

            //
            //  Get the segment and make sure it it still valid and in use
            //
            //  **** this should also check that segment index is not
            //  **** greater than HEAP MAXIMUM SEGMENTS
            //

            Segment = Heap->Segments[ SegmentIndex = CurrentBlock->SegmentIndex ];

            if (Segment == NULL) {

                Status = STATUS_INVALID_ADDRESS;

                CurrentBlock = NULL;

            //
            //  If the block is the last entry then go find the next uncommitted
            //  range or segment
            //

            } else if (CurrentBlock->Flags & HEAP_ENTRY_LAST_ENTRY) {

                goto findUncommittedRange;

            //
            //  Otherwise we'll just move on to the next entry
            //

            } else {

                CurrentBlock += CurrentBlock->Size;
            }
        }
    }

    //
    //  At this point if current block is not null then we've found another
    //  entry to return.  We could also have found a segment or uncommitted
    //  range but those are handled separately above and keep current block
    //  null
    //

    if (CurrentBlock != NULL) {

        //
        //  Check if the block is in use
        //

        if (CurrentBlock->Flags & HEAP_ENTRY_BUSY) {

            //
            //  Fill in the entry field for this block
            //

            Entry->DataAddress = (CurrentBlock+1);

            if (CurrentBlock->Flags & HEAP_ENTRY_VIRTUAL_ALLOC) {

                Entry->DataSize = RtlpGetSizeOfBigBlock( CurrentBlock );

                Entry->OverheadBytes = (UCHAR)( sizeof( *VirtualAllocBlock ) + CurrentBlock->Size);

                Entry->SegmentIndex = HEAP_MAXIMUM_SEGMENTS;

                Entry->Flags = RTL_HEAP_BUSY |  HEAP_ENTRY_VIRTUAL_ALLOC;

            } else {

                Entry->DataSize = (CurrentBlock->Size << HEAP_GRANULARITY_SHIFT) -
                                  CurrentBlock->UnusedBytes;

                Entry->OverheadBytes = CurrentBlock->UnusedBytes;

                Entry->SegmentIndex = CurrentBlock->SegmentIndex;

                Entry->Flags = RTL_HEAP_BUSY;
            }

            if (CurrentBlock->Flags & HEAP_ENTRY_EXTRA_PRESENT) {

                ExtraStuff = RtlpGetExtraStuffPointer( CurrentBlock );

                Entry->Block.Settable = ExtraStuff->Settable;
#if i386

                Entry->Block.AllocatorBackTraceIndex = ExtraStuff->AllocatorBackTraceIndex;

#endif // i386

                if (!IS_HEAP_TAGGING_ENABLED()) {

                    Entry->Block.TagIndex = 0;

                } else {

                    Entry->Block.TagIndex = ExtraStuff->TagIndex;
                }

                Entry->Flags |= RTL_HEAP_SETTABLE_VALUE;

            } else {

                if (!IS_HEAP_TAGGING_ENABLED()) {

                    Entry->Block.TagIndex = 0;

                } else {

                    Entry->Block.TagIndex = CurrentBlock->SmallTagIndex;
                }
            }

            Entry->Flags |= CurrentBlock->Flags & HEAP_ENTRY_SETTABLE_FLAGS;

        //
        //  Otherwise the block is not in use
        //

        } else {

            Entry->DataAddress = ((PHEAP_FREE_ENTRY)CurrentBlock+1);

            Entry->DataSize = (CurrentBlock->Size << HEAP_GRANULARITY_SHIFT) -
                              sizeof( HEAP_FREE_ENTRY );

            Entry->OverheadBytes = sizeof( HEAP_FREE_ENTRY );

            Entry->SegmentIndex = CurrentBlock->SegmentIndex;

            Entry->Flags = 0;
        }
    }

    //
    //  And return to our caller
    //

    return Status;
}



//
//  Declared in nturtl.h
//

BOOLEAN
RtlLockHeap (
    IN PVOID HeapHandle
    )

/*++

Routine Description:

    This routine is used by lock access to a specific heap structure

Arguments:

    HeapHandle - Supplies a pointer to the heap being locked

Return Value:

    BOOLEAN - TRUE if the heap is now locked and FALSE otherwise (i.e.,
        the heap is ill-formed).  TRUE is returned even if the heap is
        not lockable.

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;

    RTL_PAGED_CODE();

#if 0

    //
    //  Check for the heap protected by guard pages
    //

    IF_DEBUG_PAGE_HEAP_THEN_RETURN( HeapHandle,
                                    RtlpDebugPageHeapLock( HeapHandle ));

#endif // 0

    //
    //  Validate that HeapAddress points to a HEAP structure.
    //

    if (!RtlpCheckHeapSignature( Heap, "RtlLockHeap" )) {

        return FALSE;
    }

    //
    //  Lock the heap.  And disable the lookaside list by incrementing
    //  its lock count.
    //

    if (!(Heap->Flags & HEAP_NO_SERIALIZE)) {

        RtlAcquireLockRoutine( Heap->LockVariable );

#ifndef NTOS_KERNEL_RUNTIME
        Heap->LookasideLockCount += 1;
#endif // NTOS_KERNEL_RUNTIME
    }

    return TRUE;
}



//
//  Declared in nturtl.h
//

BOOLEAN
RtlUnlockHeap (
    IN PVOID HeapHandle
    )

/*++

Routine Description:

    This routine is used to unlock access to a specific heap structure

Arguments:

    HeapHandle - Supplies a pointer to the heep being unlocked

Return Value:

    BOOLEAN - TRUE if the heap is now unlocked and FALSE otherwise (i.e.,
        the heap is ill-formed).  TRUE is also returned if the heap was
        never locked to begin with because it is not seralizable.

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;

    RTL_PAGED_CODE();

#if 0

    //
    //  Check for the heap protected by guard pages
    //

    IF_DEBUG_PAGE_HEAP_THEN_RETURN( HeapHandle,
                                    RtlpDebugPageHeapUnlock( HeapHandle ));

#endif // 0

    //
    //  Validate that HeapAddress points to a HEAP structure.
    //

    if (!RtlpCheckHeapSignature( Heap, "RtlUnlockHeap" )) {

        return FALSE;
    }

    //
    //  Unlock the heap.  And enable the lookaside logic by decrementing
    //  its lock count
    //

    if (!(Heap->Flags & HEAP_NO_SERIALIZE)) {

#ifndef NTOS_KERNEL_RUNTIME
        Heap->LookasideLockCount -= 1;
#endif // NTOS_KERNEL_RUNTIME

        RtlReleaseLockRoutine( Heap->LockVariable );
    }

    return TRUE;
}



//
//  Declared in nturtl.h
//

BOOLEAN
RtlValidateHeap (
    PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    )

/*++

Routine Description:

    This routine verifies the structure of a heap and/or heap block

Arguments:

    HeapHandle - Supplies a pointer to the heap being queried

    Flags - Supplies a set of flags used to augment those already
        enforced by the heap

    BaseAddress - Optionally supplies a pointer to the heap block
        that should be individually validated

Return Value:

    BOOLEAN - TRUE if the heap/block is okay and FALSE otherwise

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    BOOLEAN LockAcquired = FALSE;
    BOOLEAN Result;

    try {

        try {

#if 0
            //
            //  Check for the guard page version of heap
            //

            if ( IS_DEBUG_PAGE_HEAP_HANDLE( HeapHandle )) {

                Result = RtlpDebugPageHeapValidate( HeapHandle, Flags, BaseAddress );

            } else
            
#endif // 0

            {

#ifndef NTOS_KERNEL_RUNTIME

                //
                //  If there is an active lookaside list then drain and remove it.
                //  By setting the lookaside field in the heap to null we guarantee
                //  that the call the free heap will not try and use the lookaside
                //  list logic.
                //
                //  We'll actually capture the lookaside pointer from the heap and
                //  only use the captured pointer.  This will take care of the
                //  condition where another walk or lock heap can cause us to check
                //  for a non null pointer and then have it become null when we read
                //  it again.  If it is non null to start with then even if the
                //  user walks or locks the heap via another thread the pointer to
                //  still valid here so we can still try and do a lookaside list pop.
                //

                PHEAP_LOOKASIDE Lookaside = (PHEAP_LOOKASIDE)Heap->Lookaside;

                if (Lookaside != NULL) {

                    ULONG i;
                    PVOID Block;

                    Heap->Lookaside = NULL;

                    for (i = 0; i < HEAP_MAXIMUM_FREELISTS; i += 1) {

                        while ((Block = RtlpAllocateFromHeapLookaside(&(Lookaside[i]))) != NULL) {

                            RtlFreeHeap( HeapHandle, 0, Block );
                        }
                    }
                }

#endif // NTOS_KERNEL_RUNTIME

                Result = FALSE;

                //
                //  Validate that HeapAddress points to a HEAP structure.
                //

                if (RtlpCheckHeapSignature( Heap, "RtlValidateHeap" )) {

                    Flags |= Heap->ForceFlags;

                    //
                    //  Lock the heap
                    //

                    if (!(Flags & HEAP_NO_SERIALIZE)) {

                        RtlAcquireLockRoutine( Heap->LockVariable );

                        LockAcquired = TRUE;
                    }

                    //
                    //  If the user did not supply a base address then verify
                    //  the complete heap otherwise just do a single heap
                    //  entry
                    //

                    if (BaseAddress == NULL) {

                        Result = RtlpValidateHeap( Heap, TRUE );

                    } else {

                        Result = RtlpValidateHeapEntry( Heap, (PHEAP_ENTRY)BaseAddress - 1, "RtlValidateHeap" );
                    }
                }
            }

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            SET_LAST_STATUS( GetExceptionCode() );

            Result = FALSE;
        }

    } finally {

        //
        //  Unlock the heap
        //

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    //
    //  And return to our caller
    //

    return Result;
}



//
//  Declared in heappriv.h
//

PHEAP_FREE_ENTRY
RtlpCoalesceHeap (
    IN PHEAP Heap
    )

/*++

Routine Description:

    This routine scans through heap and coalesces its free blocks

Arguments:

    Heap - Supplies a pointer to the heap being modified

Return Value:

    PHEAP_FREE_ENTRY - returns a pointer to the largest free block
        in the heap

--*/

{
    SIZE_T OldFreeSize;
    SIZE_T FreeSize;
    ULONG n;
    PHEAP_FREE_ENTRY FreeBlock, LargestFreeBlock;
    PLIST_ENTRY FreeListHead, Next;

    RTL_PAGED_CODE();

    LargestFreeBlock = NULL;

    //
    //  For every free list in the heap, going from smallest to
    //  largest and skipping the zero index one we will
    //  scan the free list coalesceing the free blocks
    //

    FreeListHead = &Heap->FreeLists[ 1 ];

    n = HEAP_MAXIMUM_FREELISTS;

    while (n--) {

        //
        //  Scan the individual free list
        //

        Next = FreeListHead->Blink;

        while (FreeListHead != Next) {

            //
            //  Get a pointer to the current free list entry, and remember its
            //  next and size
            //

            FreeBlock = CONTAINING_RECORD( Next, HEAP_FREE_ENTRY, FreeList );

            Next = Next->Flink;
            OldFreeSize = FreeSize = FreeBlock->Size;

            //
            //  Coalesce the block
            //

            FreeBlock = RtlpCoalesceFreeBlocks( Heap,
                                                FreeBlock,
                                                &FreeSize,
                                                TRUE );

            //
            //  If the new free size is not equal to the old free size
            //  then we actually did some changes otherwise the coalesce
            //  calll was essentialy a noop
            //

            if (FreeSize != OldFreeSize) {

                //
                //  Check if we should decommit this block because it is too
                //  large and it is either at the beginning or end of a
                //  committed run.  Otherwise just insert the new sized
                //  block into its corresponding free list.  We'll hit this
                //  block again when we visit larger free lists.
                //

                if (FreeBlock->Size >= (PAGE_SIZE >> HEAP_GRANULARITY_SHIFT)

                        &&

                    (FreeBlock->PreviousSize == 0 ||
                     (FreeBlock->Flags & HEAP_ENTRY_LAST_ENTRY))) {

                    RtlpDeCommitFreeBlock( Heap, FreeBlock, FreeSize );

                } else {

                    RtlpInsertFreeBlock( Heap, FreeBlock, FreeSize );
                }

                Next = FreeListHead->Blink;

            } else {

                //
                //  Remember the largest free block we've found so far
                //

                if ((LargestFreeBlock == NULL) ||
                    (LargestFreeBlock->Size < FreeBlock->Size)) {

                    LargestFreeBlock = FreeBlock;
                }
            }
        }

        //
        //  Go to the next free list.  When we hit the largest dedicated
        //  size free list we'll fall back to the [0] index list
        //

        if (n == 1) {

            FreeListHead = &Heap->FreeLists[ 0 ];

        } else {

            FreeListHead++;
        }
    }

    //
    //  And return to our caller
    //

    return LargestFreeBlock;
}



SIZE_T
RtlDebugCompactHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags
    )

/*++

Routine Description:

Arguments:

Return Value:

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    BOOLEAN LockAcquired = FALSE;
    SIZE_T LargestFreeSize;

#if 0

    IF_DEBUG_PAGE_HEAP_THEN_RETURN( HeapHandle,
                                    RtlpDebugPageHeapCompact( HeapHandle, Flags ));

#endif // 0

    LargestFreeSize = 0;

    try {

        try {

            //
            //  Validate that HeapAddress points to a HEAP structure.
            //

            if (!RtlpCheckHeapSignature( Heap, "RtlCompactHeap" )) {

                LargestFreeSize = 0;
                leave;
            }

            Flags |= Heap->ForceFlags | HEAP_SKIP_VALIDATION_CHECKS;

            //
            //  Lock the heap
            //

            if (!(Flags & HEAP_NO_SERIALIZE)) {

                RtlAcquireLockRoutine( Heap->LockVariable );

                LockAcquired = TRUE;

                Flags |= HEAP_NO_SERIALIZE;
            }

            RtlpValidateHeap( Heap, FALSE );

            LargestFreeSize = RtlCompactHeap( HeapHandle, Flags );

            RtlpValidateHeapHeaders( Heap, TRUE );

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            SET_LAST_STATUS( GetExceptionCode() );
        }

    } finally {

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    return LargestFreeSize;
}



//
//  Declared in nturtl.h
//

SIZE_T
NTAPI
RtlCompactHeap (
    IN PVOID HeapHandle,
    IN ULONG Flags
    )

/*++

Routine Description:

    This routine compacts the specified heap by coalescing all the free block.
    It also determines the size of the largest available free block and
    returns its, in bytes, back to the caller.

Arguments:

    HeapHandle - Supplies a pointer to the heap being modified

    Flags - Supplies a set of flags used to augment those already
        enforced by the heap

Return Value:

    SIZE_T - Returns the size, in bytes, of the largest free block
        available in the heap

--*/

{
    PHEAP Heap = (PHEAP)HeapHandle;
    PHEAP_FREE_ENTRY FreeBlock;
    PHEAP_SEGMENT Segment;
    UCHAR SegmentIndex;
    SIZE_T LargestFreeSize;
    BOOLEAN LockAcquired = FALSE;

    //
    //  Augment the heap flags
    //

    Flags |= Heap->ForceFlags;

    //
    //  Check if this is a debug version of heap
    //

    if (DEBUG_HEAP( Flags )) {

        return RtlDebugCompactHeap( HeapHandle, Flags );
    }

    try {

        //
        //  Lock the heap
        //

        if (!(Flags & HEAP_NO_SERIALIZE)) {

            RtlAcquireLockRoutine( Heap->LockVariable );

            LockAcquired = TRUE;
        }

        LargestFreeSize = 0;

        try {

            //
            //  Coalesce the heap into its largest free blocks possible
            //  and get the largest free block in the heap
            //

            FreeBlock = RtlpCoalesceHeap( (PHEAP)HeapHandle );

            //
            //  If there is a free block then compute its byte size
            //

            if (FreeBlock != NULL) {

                LargestFreeSize = FreeBlock->Size << HEAP_GRANULARITY_SHIFT;
            }

            //
            //  Scan every segment in the heap looking at its largest uncommitted
            //  range.  Remember the largest range if its bigger than anything
            //  we've found so far
            //

            for (SegmentIndex=0; SegmentIndex<HEAP_MAXIMUM_SEGMENTS; SegmentIndex++) {

                Segment = Heap->Segments[ SegmentIndex ];

                if (Segment && Segment->LargestUnCommittedRange > LargestFreeSize) {

                    LargestFreeSize = Segment->LargestUnCommittedRange;
                }
            }

        } except( EXCEPTION_EXECUTE_HANDLER ) {

            SET_LAST_STATUS( GetExceptionCode() );
        }

    } finally {

        //
        //  Unlock the heap
        //

        if (LockAcquired) {

            RtlReleaseLockRoutine( Heap->LockVariable );
        }
    }

    //
    //  And return the largest free size to our caller
    //

    return LargestFreeSize;
}


#endif // DBG


