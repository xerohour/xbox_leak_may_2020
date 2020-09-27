/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    sysptes.c

Abstract:

    This module implements the functions to manipulate the system PTE space.
    These PTES are used to map kernel stacks, pool pages, and I/O devices.

--*/

#include "mi.h"

//
// Define a macro to access the size for free blocks larger than one entry.
//
#define MI_PTE_LIST_LARGE_BLOCK_SIZE(ppte) (((ppte)+1)->List.NextEntry)

//
// Define a macro to get the next entry from a PTE list.
//
#define MI_PTE_LIST_NEXT_ENTRY(ppte) ((PMMPTE)((ppte)->Long & MM_PTE_NEXT_ENTRY_MASK))

//
// Define a macro to encode the next entry value for the supplied PTE.
//
#define MI_PTE_LIST_ENCODE_NEXT_ENTRY(ppte) (((ULONG)(ppte)) >> 2)

//
// PTE range starting at MM_SYSTEM_PTE_BASE.
//
MMPTERANGE MmSystemPteRange = {
    { MM_EMPTY_PTE_LIST << 2 },
    MiGetPteAddress(MM_SYSTEM_PTE_BASE),
    MiGetPteAddress(MM_SYSTEM_PTE_BASE),
    MiGetPteAddress(MM_SYSTEM_PTE_END + 1),
    &MmAvailablePages,
    MiRemoveZeroPage
};

#ifdef DEVKIT
//
// PTE range starting at MM_DEVKIT_PTE_BASE.
//
// To allow consoles with only 64 megabytes to still be able to load the debug
// monitor, fall back on using the retail allocators.
//
// Arcade retail and DEVKIT units have the same amount of memory, so always use
// the retail allocators.
//
MMPTERANGE MmDeveloperKitPteRange = {
    { MM_EMPTY_PTE_LIST << 2 },
    MiGetPteAddress(MM_DEVKIT_PTE_BASE),
    MiGetPteAddress(MM_DEVKIT_PTE_BASE),
    MiGetPteAddress(MM_DEVKIT_PTE_END + 1),
#ifdef ARCADE
    &MmAvailablePages,
    MiRemoveZeroPage
#else
    &MmDeveloperKitPfnRegion.AvailablePages,
    MiRemoveDebuggerPage
#endif
};
#endif

PMMPTE
MiReserveSystemPtes(
    IN PMMPTERANGE PteRange,
    IN PFN_COUNT NumberOfPtes
    )
/*++

Routine Description:

    This routine allocates the specified number of contiguous PTE slots from the
    system page tables.

Arguments:

    PteRange - Supplies the PTE range to allocate the PTEs from.

    NumberOfPtes - Supplies the number of contiguous PTE slots to allocate.

Return Value:

    Returns a pointer to the first allocated PTE.

Environment:

    Kernel mode.

--*/
{
    KIRQL OldIrql;
    PMMPTE LastPointerPte;
    PMMPTE PointerPte;
    PFN_COUNT ExtraPtes;
    PFN_COUNT PtesToCommit;
    PFN_COUNT NumberOfPtesInBlock;
    MMPTE TempPte;
    PFN_COUNT PtesCommitted;
    PMMPTE PointerPde;

    ASSERT(NumberOfPtes != 0);

    MI_LOCK_MM(&OldIrql);

    //
    // Scan the free list for a block that can satisfy the requested size.
    //

RetryOperation:
    LastPointerPte = &PteRange->HeadPte;

    while (LastPointerPte->List.NextEntry != MM_EMPTY_PTE_LIST) {

        PointerPte = MI_PTE_LIST_NEXT_ENTRY(LastPointerPte);

        //
        // Verify that the list is ordered by free address.
        //

        if (LastPointerPte != &PteRange->HeadPte) {
            ASSERT(PointerPte > LastPointerPte);
        }

        if (PointerPte->List.OneEntry) {

            if (NumberOfPtes == 1) {
                LastPointerPte->List.NextEntry = PointerPte->List.NextEntry;
                goto ReturnPointerPte;
            }

        } else if (MI_PTE_LIST_LARGE_BLOCK_SIZE(PointerPte) == NumberOfPtes) {

            //
            // A single PTE can't be encoded in a format where Link.OneEntry is
            // clear.
            //

            ASSERT(NumberOfPtes != 1);

            LastPointerPte->List.NextEntry = PointerPte->List.NextEntry;
            goto ReturnPointerPte;

        } else if (MI_PTE_LIST_LARGE_BLOCK_SIZE(PointerPte) > NumberOfPtes) {

            //
            // The block has more PTEs that we need so shuffle the free count
            // into the next PTE or convert the PTE to the single PTE format.
            //

            ExtraPtes = MI_PTE_LIST_LARGE_BLOCK_SIZE(PointerPte) - NumberOfPtes;

            if (ExtraPtes > 1) {
                PointerPte[NumberOfPtes].List.OneEntry = 0;
                MI_PTE_LIST_LARGE_BLOCK_SIZE(&PointerPte[NumberOfPtes]) = ExtraPtes;
            } else {
                PointerPte[NumberOfPtes].List.OneEntry = 1;
            }

            PointerPte[NumberOfPtes].List.NextEntry = PointerPte->List.NextEntry;
            LastPointerPte->List.NextEntry += NumberOfPtes;
            goto ReturnPointerPte;
        }

        LastPointerPte = PointerPte;
    }

    //
    // The PTE list doesn't have a block that can satisfy the request.  See how
    // many PTEs we'll need to commit to make enough space.
    //

    PtesToCommit = NumberOfPtes;

    if (LastPointerPte != &PteRange->HeadPte) {

        NumberOfPtesInBlock = LastPointerPte->List.OneEntry ? 1 :
            MI_PTE_LIST_LARGE_BLOCK_SIZE(LastPointerPte);

        if (LastPointerPte + NumberOfPtesInBlock == PteRange->LastCommittedPte) {
            PtesToCommit -= NumberOfPtesInBlock;
        }
    }

    PtesToCommit = MI_ROUND_TO_SIZE(PtesToCommit, PTE_PER_PAGE);
    ASSERT(PtesToCommit > 0);

    //
    // Commit the additional page tables if there's enough virtual address space
    // to hold them.
    //

    if ((ULONG)(PteRange->LastReservedPte - PteRange->LastCommittedPte) >
        PtesToCommit) {

        if (*PteRange->AvailablePages >= (PtesToCommit / PTE_PER_PAGE)) {

            PointerPte = PteRange->LastCommittedPte;

            for (PtesCommitted = 0; PtesCommitted < PtesToCommit;
                PtesCommitted += PTE_PER_PAGE) {

                PointerPde = MiGetPteAddress(PteRange->LastCommittedPte);

                TempPte.Long = MiGetValidKernelPdeBits();
                TempPte.Hard.PageFrameNumber =
                    PteRange->RemovePageRoutine(MmSystemPageTableUsage, PointerPde);

                MI_WRITE_PTE(PointerPde, TempPte);

                PteRange->LastCommittedPte += PTE_PER_PAGE;
            }

            MiReleaseSystemPtes(PteRange, PointerPte, PtesToCommit);
            goto RetryOperation;
        }
    }

    PointerPte = NULL;

ReturnPointerPte:
    MI_UNLOCK_MM(OldIrql);

    return PointerPte;
}

VOID
MiReleaseSystemPtes(
    IN PMMPTERANGE PteRange,
    IN PMMPTE StartingPte,
    IN PFN_COUNT NumberOfPtes
    )
/*++

Routine Description:

    This routine frees the specified number of contiguous PTE slots from the
    system page tables.

Arguments:

    PteRange - Supplies the PTE range to free the PTEs to.

    StartingPte - Supplies the pointer to the first PTE to free.

    NumberOfPtes - Supplies the number of contiguous PTE slots to fre.

Return Value:

    None.

Environment:

    Kernel mode.

--*/
{
    KIRQL OldIrql;
    PMMPTE LastPointerPte;
    PMMPTE PointerPte;
    PFN_COUNT NumberOfPtesInBlock;

    ASSERT(NumberOfPtes != 0);
    ASSERT(StartingPte >= PteRange->FirstCommittedPte);
    ASSERT(StartingPte + NumberOfPtes <= PteRange->LastCommittedPte);

    MI_LOCK_MM(&OldIrql);

    //
    // Zero the PTEs.  The caller is responsible for deleting any pages attached
    // to these PTEs and flushing the TLB.
    //

    RtlFillMemoryUlong(StartingPte, NumberOfPtes * sizeof(MMPTE), 0);

    //
    // Scan the free list for the position to insert the PTE block.
    //

    LastPointerPte = &PteRange->HeadPte;

    while (LastPointerPte->List.NextEntry != MM_EMPTY_PTE_LIST) {

        PointerPte = MI_PTE_LIST_NEXT_ENTRY(LastPointerPte);

        //
        // Verify that the list is ordered by free address.
        //

        if (LastPointerPte != &PteRange->HeadPte) {
            ASSERT(PointerPte > LastPointerPte);
        }

        if (PointerPte > StartingPte) {

            //
            // Verify that the end of the block we're freeing up doesn't overlap
            // the start of the current block.
            //
            ASSERT(PointerPte >= StartingPte + NumberOfPtes);

            //
            // If the current block is contiguous with the block we're freeing
            // up, then combine the two blocks.
            //
            if (StartingPte + NumberOfPtes == PointerPte) {
                NumberOfPtesInBlock = PointerPte->List.OneEntry ? 1 :
                    MI_PTE_LIST_LARGE_BLOCK_SIZE(PointerPte);
                NumberOfPtes += NumberOfPtesInBlock;
                LastPointerPte->List.NextEntry = PointerPte->List.NextEntry;
            }

            break;
        }

        //
        // Verify that the end of the current block doesn't overlap the start of
        // the block we're freeing up.
        //

        ASSERT((PointerPte + (PointerPte->List.OneEntry ? 1 :
            MI_PTE_LIST_LARGE_BLOCK_SIZE(PointerPte))) <= StartingPte);

        LastPointerPte = PointerPte;
    }

    //
    // Set the number of PTEs in the freed block and link it into the free list.
    //

    if (NumberOfPtes > 1) {
        StartingPte->List.OneEntry = 0;
        StartingPte[1].List.NextEntry = NumberOfPtes;
    } else {
        StartingPte->List.OneEntry = 1;
    }

    StartingPte->List.NextEntry = LastPointerPte->List.NextEntry;
    LastPointerPte->List.NextEntry = MI_PTE_LIST_ENCODE_NEXT_ENTRY(StartingPte);

    //
    // If the previous block is contiguous with the block we're freeing up,
    // then combine the two blocks.
    //

    if (LastPointerPte != &PteRange->HeadPte) {

        NumberOfPtesInBlock = LastPointerPte->List.OneEntry ? 1 :
            MI_PTE_LIST_LARGE_BLOCK_SIZE(LastPointerPte);

        if (LastPointerPte + NumberOfPtesInBlock == StartingPte) {
            LastPointerPte->List.OneEntry = 0;
            LastPointerPte->List.NextEntry = StartingPte->List.NextEntry;
            MI_PTE_LIST_LARGE_BLOCK_SIZE(LastPointerPte) = NumberOfPtes +
                NumberOfPtesInBlock;
        }
    }

    MI_UNLOCK_MM(OldIrql);
}
