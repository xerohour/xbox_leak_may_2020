/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    vadtree.c

Abstract:

    This module implements the routines to manipulate the virtual address
    descriptor tree.

--*/

#include "mi.h"

VOID
MiInsertVad (
    IN PMMVAD Vad
    )
/*++

Routine Description:

    This function inserts a virtual address descriptor into the tree and
    reorders the splay tree as appropriate.

Arguments:

    Vad - Supplies a pointer to a virtual address descriptor


Return Value:

    None - An exception is raised if quota is exceeded.

--*/
{
    PMMADDRESS_NODE *Root;

    ASSERT (Vad->EndingVpn >= Vad->StartingVpn);

    Root = &MmVadRoot;

    //
    // Set the hint field in the process to this Vad.
    //

    MmVadHint = (PMMADDRESS_NODE)Vad;

    if (MmVadFreeHint != NULL) {
        if (((ULONG)((PMMVAD)MmVadFreeHint)->EndingVpn +
                MI_VA_TO_VPN (X64K)) >=
                Vad->StartingVpn) {
            MmVadFreeHint = (PMMADDRESS_NODE)Vad;
        }
    }

    MiInsertNode ( (PMMADDRESS_NODE)Vad, Root);
    return;
}

VOID
MiRemoveVad (
    IN PMMVAD Vad
    )
/*++

Routine Description:

    This function removes a virtual address descriptor from the tree and
    reorders the splay tree as appropriate.  If any quota or commitment
    was charged by the VAD (as indicated by the CommitCharge field) it
    is released.

Arguments:

    Vad - Supplies a pointer to a virtual address descriptor.

Return Value:

    None.

--*/
{
    PMMADDRESS_NODE *Root;

    if (Vad == (PMMVAD)MmVadFreeHint) {
        MmVadFreeHint = (PMMADDRESS_NODE)MiGetPreviousVad (Vad);
    }

    Root = &MmVadRoot;

    MiRemoveNode ( (PMMADDRESS_NODE)Vad, Root);

    //
    // If the VadHint was the removed Vad, change the Hint.

    if (MmVadHint == (PMMADDRESS_NODE)Vad) {
        MmVadHint = MmVadRoot;
    }

    return;
}

PMMVAD
FASTCALL
MiLocateAddress (
    IN PVOID VirtualAddress
    )
/*++

Routine Description:

    The function locates the virtual address descriptor which describes
    a given address.

Arguments:

    VirtualAddress - Supplies the virtual address to locate a descriptor
                     for.

Return Value:

    Returns a pointer to the virtual address descriptor which contains
    the supplied virtual address or NULL if none was located.

--*/
{
    PMMVAD FoundVad;
    ULONG_PTR Vpn;

    if (MmVadHint == NULL) {
        return NULL;
    }

    Vpn = MI_VA_TO_VPN (VirtualAddress);
    if ((Vpn >= MmVadHint->StartingVpn) &&
        (Vpn <= MmVadHint->EndingVpn)) {

        return (PMMVAD)MmVadHint;
    }

    FoundVad = (PMMVAD)MiLocateAddressInTree ( Vpn, &MmVadRoot );

    if (FoundVad != NULL) {
        MmVadHint = (PMMADDRESS_NODE)FoundVad;
    }
    return FoundVad;
}

PVOID
MiFindEmptyAddressRange (
    IN SIZE_T SizeOfRange,
    IN ULONG_PTR Alignment,
    IN ULONG QuickCheck
    )
/*++

Routine Description:

    The function examines the virtual address descriptors to locate
    an unused range of the specified size and returns the starting
    address of the range.

Arguments:

    SizeOfRange - Supplies the size in bytes of the range to locate.

    Alignment - Supplies the alignment for the address.  Must be
                 a power of 2 and greater than the page_size.

    QuickCheck - Supplies a zero if a quick check for free memory
                 after the VadFreeHint exists, non-zero if checking
                 should start at the lowest address.

Return Value:

    Returns the starting address of a suitable range.

--*/
{
    PMMVAD NextVad;
    PMMVAD FreeHint;
    PVOID StartingVa;
    PVOID EndingVa;

    FreeHint = (PMMVAD)MmVadFreeHint;

    if ((QuickCheck == 0) && (FreeHint != NULL)) {

        EndingVa = MI_VPN_TO_VA_ENDING (FreeHint->EndingVpn);
        NextVad = MiGetNextVad (FreeHint);
        if (NextVad == NULL) {

            if (SizeOfRange <
                (((ULONG_PTR)MM_HIGHEST_USER_ADDRESS + 1) -
                         MI_ROUND_TO_SIZE((ULONG_PTR)EndingVa, Alignment))) {
                return (PMMADDRESS_NODE)MI_ROUND_TO_SIZE((ULONG_PTR)EndingVa,
                                                         Alignment);
            }
        } else {
            StartingVa = MI_VPN_TO_VA (NextVad->StartingVpn);

            if (SizeOfRange <
                ((ULONG_PTR)StartingVa -
                         MI_ROUND_TO_SIZE((ULONG_PTR)EndingVa, Alignment))) {

                //
                // Check to ensure that the ending address aligned upwards
                // is not greater than the starting address.
                //

                if ((ULONG_PTR)StartingVa >
                         MI_ROUND_TO_SIZE((ULONG_PTR)EndingVa,Alignment)) {
                    return (PMMADDRESS_NODE)MI_ROUND_TO_SIZE((ULONG_PTR)EndingVa,
                                                           Alignment);
                }
            }
        }
    }

    return (PMMVAD)MiFindEmptyAddressRangeInTree (
                   SizeOfRange,
                   Alignment,
                   MmVadRoot,
                   &MmVadFreeHint);
}
