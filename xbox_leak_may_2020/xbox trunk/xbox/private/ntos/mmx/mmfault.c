/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    mmfault.c

Abstract:

    This module contains the handlers for access check, page faults
    and write faults.

--*/

#include "mi.h"

NTSTATUS
MmAccessFault(
    IN BOOLEAN StoreInstruction,
    IN PVOID VirtualAddress,
    IN PVOID TrapInformation
    )
/*++

Routine Description:

    This function is called by the kernel on data or instruction
    access faults.  The access fault was detected due to either
    an access violation, a PTE with the present bit clear, or a
    valid PTE with the dirty bit clear and a write operation.

    Also note that the access violation and the page fault could
    occur because of the Page Directory Entry contents as well.

    This routine determines what type of fault it is and calls
    the appropriate routine to handle the page fault or the write
    fault.

Arguments:

    StoreInstruction - Supplies TRUE (1) if the operation causes a write into
                       memory.  Note this value must be 1 or 0.

    VirtualAddress - Supplies the virtual address which caused the fault.

    TrapInformation - Opaque information about the trap, interpreted by the
                      kernel, not Mm.  Needed to allow fast interlocked access
                      to operate correctly.

Return Value:

    Returns the status of the fault handling operation.  Can be one of:
        - Success.
        - Access Violation.
        - Guard Page Violation.
        - In-page Error.

Environment:

    Kernel mode, APCs disabled.

--*/
{
    NTSTATUS status;
    KIRQL OldIrql;
    PMMPTE PointerPte;
    MMPTE TempPte;
    ULONG PteProtectionMask;
    ULONG Protect;

    //
    // No page fault can be handled at DISPATCH_LEVEL or greater.
    //

    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {
        status = STATUS_IN_PAGE_ERROR | 0x10000000;
        goto ReturnStatusCode;
    }

    //
    // Determine if this is an access violation or a guard page violation.
    //

    status = STATUS_ACCESS_VIOLATION;

    if ((VirtualAddress >= MM_LOWEST_USER_ADDRESS) &&
        (VirtualAddress <= MM_HIGHEST_VAD_ADDRESS)) {

        MI_LOCK_ADDRESS_SPACE();

        MI_LOCK_MM(&OldIrql);

        if (MiGetPdeAddress(VirtualAddress)->Hard.Valid == 1) {

            PointerPte = MiGetPteAddress(VirtualAddress);
            TempPte = *PointerPte;

            //
            // Check if the PTE is marked committed.  Note that we don't just
            // check the Valid bit because the page may be a PAGE_NOACCESS or
            // PAGE_GUARD protected page.
            //

            if (TempPte.Long != 0) {

                PteProtectionMask = (TempPte.Long & MM_PTE_PROTECTION_MASK);
                Protect = MiDecodePteProtectionMask(PteProtectionMask);

                if (Protect & PAGE_GUARD) {

                    //
                    // This is a guard page.  Clear the guard page protection
                    // for this page and return the correct violation status.
                    //

                    MiMakePteProtectionMask(Protect & ~PAGE_GUARD,
                        &PteProtectionMask);

                    TempPte.Long = ((TempPte.Long & ~MM_PTE_PROTECTION_MASK) |
                        PteProtectionMask);

                    MI_WRITE_AND_FLUSH_PTE(PointerPte, TempPte);

                    status = STATUS_GUARD_PAGE_VIOLATION;
                }
            }
        }

        MI_UNLOCK_MM(OldIrql);

        MI_UNLOCK_ADDRESS_SPACE();
    }

ReturnStatusCode:
    if (!NT_SUCCESS(status) && status != STATUS_GUARD_PAGE_VIOLATION) {
        if (TrapInformation != NULL) {
            MiDbgPrint(("MM: page fault touching %p, trap frame %p, eip %p\n",
                VirtualAddress, TrapInformation,
                ((PKTRAP_FRAME)TrapInformation)->Eip));
        } else {
            MiDbgPrint(("MM: page fault touching %p, trap frame %p\n",
                VirtualAddress, TrapInformation));
        }
    }

    return status;
}
