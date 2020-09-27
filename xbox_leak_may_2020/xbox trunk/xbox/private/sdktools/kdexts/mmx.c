/*++

Copyright (c) 1992-2001  Microsoft Corporation

Module Name:

    mmx.c

Abstract:

    WinDbg Extension Api

--*/

#include "precomp.h"
#pragma hdrstop

PMMPTE
DbgGetPdeAddress(
    IN PVOID VirtualAddress
    )
{
    return MiGetPdeAddress(VirtualAddress);
}

PMMPTE
DbgGetPteAddress(
    IN PVOID VirtualAddress
    )
{
    return MiGetPteAddress(VirtualAddress);
}

ULONG
DbgGetPteSize(
    VOID
    )
{
    return sizeof(MMPTE);
}

ULONG
DbgGetPfnSize(
    VOID
    )
{
    return sizeof(MMPFN);
}

PVOID
DbgGetVirtualAddressMappedByPte(
    IN PMMPTE Pte
    )
{
    return MiGetVirtualAddressMappedByPte (Pte);
}

VOID
DumpPte(
    LPCSTR Label,
    PMMPTE Pte
    );

ULONG
MiGetFrameFromPte (
    PMMPTE lpte
    )
/*++

Routine Description:

    If the PTE is valid, returns the page frame number that
    the PTE maps.  Zero is returned otherwise.

Arguments:

    lpte - the PTE to examine.

--*/


{
    MMPTE Pte1;

    Pte1.Long = lpte->Long;

    if (Pte1.Hard.Valid) {
        return (ULONG)(Pte1.Hard.PageFrameNumber);
    }
    return(0);
}

ULONG
GetAddressState(
    IN PVOID VirtualAddress
    )
{
    ULONG   Address;
    ULONG   result;
    ULONG   flags = 0;
    PMMPTE  Pte;
    PMMPTE  Pde;
    ULONG   PdeContents;
    ULONG   PteContents;

    if (MI_IS_PHYSICAL_ADDRESS (VirtualAddress)) {
        return ADDRESS_VALID;
    }
    Address = (ULONG)VirtualAddress;

    Pde = DbgGetPdeAddress (VirtualAddress);
    Pte = DbgGetPteAddress (VirtualAddress);

    if ( !ReadMemory( (DWORD)Pde,
                      &PdeContents,
                      sizeof(ULONG),
                      &result) ) {
        dprintf("%08lx: Unable to get PDE\n",Pde);
        return ADDRESS_NOT_VALID;
    }

    if (PdeContents & MM_PTE_VALID_MASK) {
        if (PdeContents & MM_PTE_LARGE_PAGE_MASK) {
            return ADDRESS_VALID;
        }
        if ( !ReadMemory( (DWORD)Pte,
                          &PteContents,
                          sizeof(ULONG),
                          &result) ) {
            dprintf("%08lx: Unable to get PTE\n",Pte);
            return ADDRESS_NOT_VALID;
        }
        if (PteContents & MM_PTE_VALID_MASK) {
            return ADDRESS_VALID;
        }
    }
    return ADDRESS_NOT_VALID;
}


BOOLEAN
CopyPfnDatabaseLocal(
    OUT PMMPFN *PfnDatabaseLocalOutput,
    OUT ULONG *NumberOfPages
    )
{
    PFN_NUMBER HighestPhysicalPage;
    PMMPFN PfnDatabase;
    SIZE_T SizeOfPfnDatabase;
    PMMPFN PfnDatabaseLocal;
    PFN_NUMBER CurrentPage;
    PFN_NUMBER EntriesToRead;
    ULONG result;

    HighestPhysicalPage = GetNtDebuggerDataValue(MmHighestPhysicalPage);
    PfnDatabase = (PMMPFN)GetNtDebuggerDataValue(MmPfnDatabase);

    if (HighestPhysicalPage == 0 || PfnDatabase == 0) {
        return FALSE;
    }

    //  Make the value exclusive.
    HighestPhysicalPage++;

    SizeOfPfnDatabase = HighestPhysicalPage * sizeof(MMPFN);

    PfnDatabaseLocal = (PMMPFN)LocalAlloc(LPTR, SizeOfPfnDatabase);

    if (PfnDatabaseLocal == NULL) {
        return FALSE;
    }

    EntriesToRead = 1024;

    for (CurrentPage = 0; CurrentPage < HighestPhysicalPage; CurrentPage += EntriesToRead) {

        dprintf("loading (%d%% complete)\r", (CurrentPage * 100) / HighestPhysicalPage);

        if ((HighestPhysicalPage - CurrentPage) < EntriesToRead) {
            EntriesToRead = HighestPhysicalPage - CurrentPage;
        }

        if (!ReadMemory((DWORD)(&PfnDatabase[CurrentPage]),
                        (&PfnDatabaseLocal[CurrentPage]),
                        EntriesToRead * sizeof(MMPFN),
                        &result)) {
            dprintf("unable to read PFNs starting at index %x\n", CurrentPage);
            LocalFree(PfnDatabaseLocal);
            return FALSE;
        }

        if (CheckControlC()) {
            LocalFree(PfnDatabaseLocal);
            return FALSE;
        }
    }

    *PfnDatabaseLocalOutput = PfnDatabaseLocal;
    *NumberOfPages = HighestPhysicalPage;

    return TRUE;
}

#define STRINGIZE(_name) #_name
#define DUMP_CASE(name) dprintf("%6d pages  " STRINGIZE(name) " (%d KB)\n", AllocatedPagesByUsageLocal[name], AllocatedPagesByUsageLocal[name] * 4096 / 1024)

DECLARE_API(memusage)
{
    PFN_COUNT value;
    PFN_COUNT *AllocatedPagesByUsage;
    PFN_COUNT AllocatedPagesByUsageLocal[MmMaximumUsage];
    ULONG result;

    value = GetNtDebuggerDataValue(MmNumberOfPhysicalPages);
    dprintf("%6d  Total number of pages (%d MB)\n", value, value * 4096 / (1024 * 1024));
    value = GetNtDebuggerDataValue(MmAvailablePages);
    dprintf("%6d  Available number of pages (%d KB)\n", value, value * 4096 / 1024);
    dprintf("\n");

    AllocatedPagesByUsage = (PFN_COUNT *)GetNtDebuggerData(MmAllocatedPagesByUsage);

    if (AllocatedPagesByUsage == NULL) {
        dprintf("cannot resolve MmAllocatedPagesByUsage\n");
        return;
    }

    if (!ReadMemory((DWORD)AllocatedPagesByUsage,
                    AllocatedPagesByUsageLocal,
                    sizeof(AllocatedPagesByUsageLocal),
                    &result)) {
        dprintf("cannot read MmAllocatedPagesByUsage\n");
        return;
    }

    DUMP_CASE(MmUnknownUsage);
    DUMP_CASE(MmStackUsage);
    DUMP_CASE(MmVirtualPageTableUsage);
    DUMP_CASE(MmSystemPageTableUsage);
    DUMP_CASE(MmPoolUsage);
    DUMP_CASE(MmVirtualMemoryUsage);
    DUMP_CASE(MmSystemMemoryUsage);
    DUMP_CASE(MmImageUsage);
    DUMP_CASE(MmFsCacheUsage);
    DUMP_CASE(MmContiguousUsage);
    DUMP_CASE(MmDebuggerUsage);
}

#define STRING_CASE(name) case Mm##name##Usage: TypeName = STRINGIZE(name); break

VOID
DumpPfnEntry(
    ULONG PageFrameNumber,
    PMMPFN PageFrame
    )
{
    ULONG Color;
    LPCSTR TypeName;

    if (PageFrame->Pte.Hard.Valid != 0) {

        dprintf("%05x ", PageFrameNumber);
        DumpPte("PTE", &PageFrame->Pte);

    } else if (PageFrame->Busy.Busy != 0) {

        switch (PageFrame->Busy.BusyType) {

            STRING_CASE(Unknown);
            STRING_CASE(Stack);
            STRING_CASE(VirtualPageTable);
            STRING_CASE(SystemPageTable);
            STRING_CASE(Pool);
            STRING_CASE(VirtualMemory);
            STRING_CASE(SystemMemory);
            STRING_CASE(Image);
            STRING_CASE(FsCache);
            STRING_CASE(Contiguous);
            STRING_CASE(Debugger);

            default: TypeName = "UNKNOWN";
        }

        dprintf("%05x %-20s lockcount=%04x ", PageFrameNumber, TypeName,
            PageFrame->Busy.LockCount >> 1);

        if (PageFrame->Busy.BusyType == MmVirtualPageTableUsage) {
            dprintf("ptecount=%d", PageFrame->Directory.NumberOfUsedPtes);
        } else if (PageFrame->Busy.BusyType == MmFsCacheUsage) {
            dprintf("fsindex=%d", PageFrame->FsCache.ElementIndex);
        } else {
            if (PageFrame->Busy.BusyType != MmUnknownUsage) {
                dprintf("pteindex=%03x ", PageFrame->Busy.PteIndex);
            }
        }

    } else {

        dprintf("%05x %-20s", PageFrameNumber, "Free");

        Color = MiGetPfnColor(PageFrameNumber);

        if (PageFrame->Free.PackedPfnBlink == MM_PACKED_PFN_NULL) {
            dprintf(" blink=<null>   ");
        } else {
            dprintf(" blink=%05x   ", MiUnpackFreePfn(PageFrame->Free.PackedPfnBlink, Color));
        }

        if (PageFrame->Free.PackedPfnFlink == MM_PACKED_PFN_NULL) {
            dprintf(" flink=<null>");
        } else {
            dprintf(" flink=%05x", MiUnpackFreePfn(PageFrame->Free.PackedPfnFlink, Color));
        }
    }

    dprintf("\n");
}

DECLARE_API(pfn)
{
    ULONG Address = 0;
    ULONG Flags = 0;
    PMMPFN PfnDatabaseLocal = NULL;
    ULONG NumberOfPages;
    PMMPFN Pfn;
    ULONG i;
    ULONG printed = 0;
    MMPFN PfnLocal;
    PMMPFN PfnDatabase;
    PMMPTE PointerPde;
    PMMPTE PointerPte;
    MMPTE TempPte;
    ULONG result;
    char ExprBuf[256] ;

    sscanf(args, "%s %lx", ExprBuf, &Flags);

    if (ExprBuf[0]) {
        if (IsHexNumber(ExprBuf) || ExprBuf[0] == '-') {
            sscanf(ExprBuf, "%lx", &Address) ;
        } else {
            Address = GetExpression(ExprBuf);
            if (Address==0) {
                dprintf("An error occured trying to evaluate the expression\n") ;
                return;
            }
        }
    }

    //
    // Dump the whole thing?
    //

    if (Address == (ULONG)-1 || Address == (ULONG)-2 || Address == (ULONG)-3 || Address == (ULONG)-4) {

        if (CopyPfnDatabaseLocal(&PfnDatabaseLocal, &NumberOfPages)) {

            if (((LONG)Flags) < 0) {
                Flags = (1 << ((ULONG)-((LONG)Flags)));
            }

            for (i = 0; i < NumberOfPages; i++) {

                if (Flags != 0) {

                    // Use flags to filter the busy type.
                    if (PfnDatabaseLocal[i].Busy.Busy == 0 ||
                        ((Flags & (1 << PfnDatabaseLocal[i].Busy.BusyType)) == 0)) {
                        continue;
                    }
                }

                // Only dump locked pages.
                if (Address == (ULONG)-2) {
                    if (PfnDatabaseLocal[i].Pte.Hard.Valid != 0) {
                        continue;
                    }
                    if ((PfnDatabaseLocal[i].Busy.Busy == 0) ||
                        (PfnDatabaseLocal[i].Busy.LockCount == 0)) {
                        continue;
                    }
                }

                // Only dump free pages.
                if (Address == (ULONG)-3) {
                    if (PfnDatabaseLocal[i].Busy.Busy != 0) {
                        continue;
                    }
                }

                // Only dump physically mapped pages.
                if (Address == (ULONG)-4) {
                    if (PfnDatabaseLocal[i].Pte.Hard.Valid == 0) {
                        continue;
                    }
                }

                DumpPfnEntry(i, &PfnDatabaseLocal[i]);
                printed++;

                if (CheckControlC()) {
                    break;
                }
            }

            dprintf("%08x entries printed\n", printed);

            LocalFree((HLOCAL)PfnDatabaseLocal);
        }

        return;
    }

    //
    // Dump an address's corresponding PFN?
    //

    if (Address >= 0x00010000) {

        PointerPde = DbgGetPdeAddress((PVOID)Address);

        if (!ReadMemory((DWORD)PointerPde,
                        &TempPte,
                        sizeof(MMPTE),
                        &result)) {
            dprintf("unable to read PDE for %p\n", Address);
            return;
        }

        if (TempPte.Hard.Valid == 0) {
            dprintf("PDE not marked valid for %p\n", Address);
            return;
        }

        if (TempPte.Hard.LargePage == 0) {

            PointerPte = DbgGetPteAddress((PVOID)Address);

            if (!ReadMemory((DWORD)PointerPte,
                            &TempPte,
                            sizeof(MMPTE),
                            &result)) {
                dprintf("unable to read PTE for %p\n", Address);
                return;
            }

            if (TempPte.Hard.Valid == 0) {
                dprintf("PTE not marked valid for %p\n", Address);
                return;
            }

            Address = TempPte.Hard.PageFrameNumber;

        } else {

            Address = TempPte.Hard.PageFrameNumber +
                (BYTE_OFFSET_LARGE(Address) >> PAGE_SHIFT);
        }
    }

    //
    // Dump an index.
    //

    PfnDatabase = (PMMPFN)GetNtDebuggerDataValue(MmPfnDatabase);

    if (PfnDatabase == 0) {
        return;
    }

    if (!ReadMemory((DWORD)(&PfnDatabase[Address]),
                    &PfnLocal,
                    sizeof(MMPFN),
                    &result)) {
        dprintf("unable to read PFN %x\n", Address);
        return;
    }

    DumpPfnEntry(Address, &PfnLocal);
}

VOID
DumpPte(
    LPCSTR Label,
    PMMPTE Pte
    )
{
    char *CachingType;

    if (Pte->Hard.Valid != 0) {

        if (Pte->Hard.CacheDisable != 0) {
            CachingType = "UC";
        } else if (Pte->Hard.WriteThrough == 1) {
            CachingType = "WC";
        } else {
            CachingType = "WB";
        }

        dprintf("%s %05x %c%c%c%c%c%c%c%c%c%c%c %s  ",
            Label,
            Pte->Hard.PageFrameNumber,
            Pte->Hard.PersistAllocation ? 'K' : '-',
            Pte->Hard.GuardOrEndOfAllocation ? 'E' : '-',
            Pte->Hard.Global ? 'G' : '-',
            Pte->Hard.LargePage ? 'L' : '-',
            Pte->Hard.Dirty ? 'D' : '-',
            Pte->Hard.Accessed ? 'A' : '-',
            Pte->Hard.CacheDisable ? 'C' : '-',
            Pte->Hard.WriteThrough ? 'T' : '-',
            Pte->Hard.Owner ? 'O' : '-',
            Pte->Hard.Write ? 'W' : 'R',
            Pte->Hard.Valid ? 'V' : '-',
            CachingType);

    } else {
        dprintf("%s invalid %08x   ", Label, Pte->Long);
    }
}

DECLARE_API(pte)
{
    ULONG Address = 0;
    MMPTE TempPte;
    ULONG result;
    char ExprBuf[256] ;

    sscanf(args, "%s", ExprBuf);

    if (ExprBuf[0]) {
        if (IsHexNumber(ExprBuf)) {
            sscanf(ExprBuf, "%lx", &Address) ;
        } else {
            Address = GetExpression(ExprBuf);
            if (Address==0) {
                dprintf("An error occured trying to evaluate the expression\n") ;
                return;
            }
        }
    }

    //
    // Convert a non-PTE section address to it's PTE address.
    //

    if (Address < (ULONG)MiGetPteAddress(0) || Address > (ULONG)MiGetPteAddress(0xFFFFFFFF)) {
        Address = (ULONG)MiGetPteAddress(Address);
    }

    dprintf("va %p:  ", MiGetVirtualAddressMappedByPte(Address));

    if (!ReadMemory((DWORD)MiGetPteAddress(Address),
                    &TempPte,
                    sizeof(MMPTE),
                    &result)) {
        dprintf("\nunable to read PDE for %p\n", Address);
        return;
    }

    DumpPte("PDE", &TempPte);

    if (TempPte.Hard.Valid != 0 && TempPte.Hard.LargePage == 0) {

        if (!ReadMemory((DWORD)Address,
                        &TempPte,
                        sizeof(MMPTE),
                        &result)) {
            dprintf("\nunable to read PTE for %p\n", Address);
            return;
        }

        DumpPte("PTE", &TempPte);
    }

    dprintf("\n");
}
