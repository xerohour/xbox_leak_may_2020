/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    vad.c

Abstract:

    WinDbg Extension Api

Author:

    Lou Perazzoli (loup) 12-Jun-1992

Environment:

    User Mode.

Revision History:

    Converted to WinDbg extension:
    Ramon J San Andres (ramonsa) 8-Nov-1993

--*/

#include "precomp.h"
#pragma hdrstop

DECLARE_API( vad )

/*++

Routine Description:

    Dumps all vads for process.

Arguments:

    args - Address Flags

Return Value:

    None

--*/

{
    ULONG   Result;
    PMMADDRESS_NODE  Next;
    PMMADDRESS_NODE  VadToDump;
    PMMADDRESS_NODE  Parent;
    PMMADDRESS_NODE  First;
    PMMADDRESS_NODE  Left;
    MMVAD   CurrentVad;
    ULONG   Flags;
    ULONG   Done;
    ULONG   Level = 0;
    ULONG   Count = 0;
    ULONG   AverageLevel = 0;
    ULONG   MaxLevel = 0;

    VadToDump = (PVOID)0xFFFFFFFF;
    Flags     = 0;
    sscanf(args,"%lx %lx",&VadToDump,&Flags);

    if (VadToDump == (PVOID)0xFFFFFFFF) {
        VadToDump = (PMMADDRESS_NODE)GetNtDebuggerDataValue(MmVadRoot);
    }

    First = VadToDump;
    if (First == NULL) {
        return;
    }

    RtlZeroMemory (&CurrentVad, sizeof(MMVAD));

    if ( !ReadMemory( (DWORD)First,
                      &CurrentVad,
                      sizeof(MMVAD),
                      &Result) ) {
        dprintf("%08lx: Unable to get contents of VAD\n",First );
        return;
    }

    if (Flags) {

        //
        // Dump only this vad.
        //

        dprintf("\nVAD @ %8lx\n",VadToDump);
        dprintf("  Start VPN:      %8lx  End VPN: %8lx\n",
            CurrentVad.StartingVpn,
            CurrentVad.EndingVpn);

        dprintf ("\n\n");

        return;
    }

    while (CurrentVad.LeftChild != NULL) {
        if ( CheckControlC() ) {
            return;
        }
        First = CurrentVad.LeftChild;
        Level += 1;
        if (Level > MaxLevel) {
            MaxLevel = Level;
        }
        if ( !ReadMemory( (DWORD)First,
                          &CurrentVad,
                          sizeof(MMVAD),
                          &Result) ) {
            dprintf("%08lx:%lx Unable to get contents of VAD\n",First, CurrentVad );
            return;
        }
    }

    dprintf("VAD     level      start      end\n");
    dprintf("%lx (%2ld)   %8lx %8lx\n",
            First,
            Level,
            CurrentVad.StartingVpn,
            CurrentVad.EndingVpn
            );
    Count += 1;
    AverageLevel += Level;

    Next = First;
    while (Next != NULL) {

        if ( CheckControlC() ) {
            return;
        }

        if (CurrentVad.RightChild == NULL) {

            Done = TRUE;
            while ((Parent = CurrentVad.Parent) != NULL) {
                if ( CheckControlC() ) {
                    return;
                }

                Level -= 1;

                //
                // Locate the first ancestor of this node of which this
                // node is the left child of and return that node as the
                // next element.
                //

                if ( !ReadMemory( (DWORD)Parent,
                                  &CurrentVad,
                                  sizeof(MMVAD),
                                  &Result) ) {
                    dprintf("%08lx:%lx Unable to get contents of VAD\n",Parent, CurrentVad);
                    return;
                }

                if (CurrentVad.LeftChild == Next) {
                    Next = Parent;
                    dprintf("%lx (%2ld)   %8lx %8lx\n",
                            Next,
                            Level,
                            CurrentVad.StartingVpn,
                            CurrentVad.EndingVpn
                           );
                    Done = FALSE;
                    Count += 1;
                    AverageLevel += Level;
                    break;
                }
                Next = Parent;
            }
            if (Done) {
                Next = NULL;
                break;
            }
        } else {

            //
            // A right child exists, locate the left most child of that right child.
            //

            Next = CurrentVad.RightChild;
            Level += 1;
            if (Level > MaxLevel) {
                MaxLevel = Level;
            }

            if ( !ReadMemory( (DWORD)Next,
                              &CurrentVad,
                              sizeof(MMVAD),
                              &Result) ) {
                dprintf("%08lx:%lx Unable to get contents of VAD\n",Next, CurrentVad);
                return;
            }

            while ((Left = CurrentVad.LeftChild) != NULL) {
                if ( CheckControlC() ) {
                    return;
                }
                Level += 1;
                if (Level > MaxLevel) {
                    MaxLevel = Level;
                }
                Next = Left;
                if ( !ReadMemory( (DWORD)Next,
                                  &CurrentVad,
                                  sizeof(MMVAD),
                                  &Result) ) {
                    dprintf("%08lx:%lx Unable to get contents of VAD\n",Next, CurrentVad);
                    return;
                }
            }

            dprintf("%lx (%2ld)   %8lx %8lx\n",
                      Next,
                      Level,
                      CurrentVad.StartingVpn,
                      CurrentVad.EndingVpn
                   );
                    Count += 1;
                    AverageLevel += Level;
        }
    }
    dprintf("\nTotal VADs: %5ld  average level: %4ld  maximum depth: %ld\n",
            Count, 1+(AverageLevel/Count),MaxLevel);
    return;
}
