/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    physical.c

Abstract:

    WinDbg Extension Api

Author:

    Wesley Witt (wesw) 15-Aug-1993

Environment:

    User Mode.

Revision History:

--*/


#include "precomp.h"
#pragma hdrstop


#define MAX_PHYSICAL_MEMORY_FRAGMENTS 20

typedef struct _PHYSICAL_MEMORY_RUN {
    PFN_NUMBER BasePage;
    PFN_NUMBER PageCount;
} PHYSICAL_MEMORY_RUN, *PPHYSICAL_MEMORY_RUN;

typedef struct _PHYSICAL_MEMORY_DESCRIPTOR {
    ULONG NumberOfRuns;
    PFN_NUMBER NumberOfPages;
    PHYSICAL_MEMORY_RUN Run[1];
} PHYSICAL_MEMORY_DESCRIPTOR, *PPHYSICAL_MEMORY_DESCRIPTOR;


DECLARE_API( db )

/*++

Routine Description:

    Does a read of 16 ULONGS from the physical memory of the target maching

Arguments:

    args - Supplies physical address

Return Value:

    None.

--*/

{
#define NumberBytesToRead 32*4

    static ULONG64 LastAddress = 0;
    ULONG64 Address = 0;
    UCHAR Buffer[NumberBytesToRead];
    ULONG ActualRead;
    UCHAR hexstring[80];
    UCHAR bytestring[40];
    UCHAR ch, *p;
    int cnt, d;

    if(*args == '\0') {
        Address=LastAddress;
    } else {
        sscanf(args,"%I64x",&Address);
        Address &= (~0x3);      // Truncate to dword boundary
        LastAddress=Address;
    }
    ReadPhysical(Address,Buffer,sizeof(Buffer),&ActualRead);
    if (ActualRead != sizeof(Buffer)) {
        dprintf("Physical memory read failed\n");
    } else {
        for(cnt=0;cnt<NumberBytesToRead;cnt+=16) {
            p = hexstring + sprintf(hexstring, "#%12I64x  ",Address+cnt);
            for (d=0; d < 16; d++) {
                ch = Buffer[cnt+d];
                p += sprintf (p, "%02lx", ch);
                *(p++) = d == 7 ? '-' : ' ';
                if (ch < 0x20 || ch > 0x7e) {
                    ch = '.';
                }
                bytestring[d] = ch;
            }

            bytestring[d] = '\0';
            dprintf("%s %s\n", hexstring, bytestring);
        }
        LastAddress += sizeof(Buffer);
    }
}

DECLARE_API( dd )

/*++

Routine Description:

    Does a read of 16 ULONGS from the physical memory of the target maching

Arguments:

    args - Supplies physical address

Return Value:

    None.

--*/

{
#define NumberToRead 32

    static ULONG64 LastAddress = 0;
    ULONG64 Address = 0;
    ULONG Buffer[NumberToRead];
    ULONG ActualRead;
    int cnt;

    if(*args == '\0') {
        Address=LastAddress;
    } else {
        sscanf(args,"%I64x",&Address);
        Address &= (~0x3);      // Truncate to dword boundary
        LastAddress=Address;
    }
    ReadPhysical(Address,Buffer,sizeof(Buffer),&ActualRead);
    if (ActualRead != sizeof(Buffer)) {
        dprintf("Physical memory read failed\n");
    } else {
        for(cnt=0;cnt<NumberToRead;cnt+=4) {
            dprintf("#%12I64x  ",Address+(cnt*sizeof(ULONG)));
            dprintf("%08lx ",Buffer[cnt]);
            dprintf("%08lx ",Buffer[cnt+1]);
            dprintf("%08lx ",Buffer[cnt+2]);
            dprintf("%08lx\n",Buffer[cnt+3]);
        }
        LastAddress+=sizeof(Buffer);
    }
}


DECLARE_API( ed )

/*++

Routine Description:

    Writes a sequence of ULONGs into a given physical address on the
    target machine.

Arguments:

    arg - Supplies both the target address and the data in the form of
          "PHYSICAL_ADDRESS ULONG [ULONG, ULONG,...]"

Return Value:

    None.

--*/

{
    ULONG64 Address = 0;
    ULONG Buffer;
    ULONG ActualWritten;
    PUCHAR NextToken;

    sscanf(args,"%I64x",&Address);

    strtok((PSTR)args," \t,");      // The first token is the address

    // Since we're picking off one ULONG at a time, we'll make
    // one DbgKdWritePhysicalMemoryAddress call per ULONG.  This
    // is slow, but easy to code.
    while((NextToken=strtok(NULL," \t,")) != NULL) {
        sscanf(NextToken,"%lx",&Buffer);
        WritePhysical(Address,&Buffer,sizeof(Buffer),&ActualWritten);
        Address+=sizeof(Buffer);
    }
}


DECLARE_API( eb )

/*++

Routine Description:

    Writes a sequence of BYTEs into a given physical address on the
    target machine.

Arguments:

    arg - Supplies both the target address and the data in the form of
          "PHYSICAL_ADDRESS ULONG [ULONG, ULONG,...]"

Return Value:

    None.

--*/

{
    ULONG64 Address = 0;
    ULONG Buffer;
    UCHAR c;
    ULONG ActualWritten;
    PUCHAR NextToken;

    sscanf(args,"%I64x",&Address);

    strtok((PSTR)args," \t,");      // The first token is the address

    // Since we're picking off one BYTE at a time, we'll make
    // one DbgKdWritePhysicalMemoryAddress call per BYTE.  This
    // is slow, but easy to code.
    while((NextToken=strtok(NULL," \t,")) != NULL) {
        sscanf(NextToken,"%lx",&Buffer);
        c = (UCHAR)Buffer;
        WritePhysical(Address,&c,sizeof(UCHAR),&ActualWritten);
        Address+=sizeof(UCHAR);
    }
}


DECLARE_API( chklowmem )

/*++

Routine Description:

    Calls an Mm function that checks if the physical pages
    below 4Gb have a required fill pattern for PAE systems
    booted with /LOWMEM switch.

Arguments:

    None.

Return Value:

    None.

--*/
{
    dprintf ("Checking the low 4GB of RAM for required fill pattern. \n");
    dprintf ("Please wait (verification takes approx. 20s) ...\n");

    Ioctl (IG_LOWMEM_CHECK, NULL, 0);

    dprintf ("Lowmem check done.\n");
}


/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// !search
/////////////////////////////////////////////////////////////////////

//
//  Kernel variable modification functions.
//


ULONG 
ReadValue (
    ULONG Address
    )
{
    ULONG Value = 0;
    ULONG BytesRead;

    if (! ReadMemory (Address, &Value, sizeof Value, &BytesRead)) {
        dprintf ("Search: read error \n");
    }

    return Value;
}

VOID
WriteValue (
    ULONG Address,
    ULONG Value
    )
{
    ULONG BytesWritten; 

    if (! WriteMemory (Address, &Value, sizeof Value, &BytesWritten)) {
        dprintf ("Search: write error \n");
    }
}
            
ULONG_PTR 
ReadPointer (
    ULONG_PTR Address
    )
{
    ULONG_PTR Value = 0;
    ULONG BytesRead;

    if (! ReadMemory (Address, &Value, sizeof Value, &BytesRead)) {
        dprintf ("Search: read error \n");
    }

    return Value;
}

VOID
WritePointer (
    ULONG_PTR Address,
    ULONG_PTR Value
    )
{
    ULONG BytesWritten;

    if (! WriteMemory (Address, &Value, sizeof Value, &BytesWritten)) {
        dprintf ("Search: write error \n");
    }
}

BOOLEAN
SearchIsSystemPageFrame (
    PFN_NUMBER PageFrameIndex
    );

PPHYSICAL_MEMORY_DESCRIPTOR
SearchGetSystemMemoryDescriptor (
    VOID
    );

ULONG_PTR 
SearchConvertPageFrameToVa (
    PFN_NUMBER PageFrameIndex,
    PULONG Flags
    );

#define SEARCH_VA_PROTOTYPE_ADDRESS     0x0001
#define SEARCH_VA_NORMAL_ADDRESS        0x0002
#define SEARCH_VA_LARGE_PAGE_ADDRESS    0x0004
#define SEARCH_VA_UNKNOWN_TYPE_ADDRESS  0x0008

//
// PAE independent functions from p_i386\pte.c
//

#if defined(_X86_) || defined(_X86PAE_)
PMMPTE
DbgGetPdeAddress(
    IN PVOID VirtualAddress
    );

PMMPTE
DbgGetPteAddress(
    IN PVOID VirtualAddress
    );
#endif // #if defined(_X86_) || defined(_X86PAE_)



DECLARE_API( search )

/*++

Routine Description:

    This routine triggers a search within a given physical
    memory range for a pointer. The hits are defined by
    an interval (below and above the pointer value) and also
    by a Hamming distance equal to one (only on bit different).

Arguments:

    None.
    
Return Value:

    None.

--*/
{
    const ULONG SEARCH_SYMBOL_CHECK = 0xABCDDCBA;

    ULONG_PTR KdpSearchPageHits;
    ULONG_PTR KdpSearchPageHitOffsets;
    ULONG_PTR KdpSearchPageHitIndex;

    ULONG_PTR KdpSearchCheckPoint;
    ULONG_PTR KdpSearchInProgress;

    ULONG_PTR KdpSearchStartPageFrame;
    ULONG_PTR KdpSearchEndPageFrame;

    ULONG_PTR KdpSearchAddressRangeStart;
    ULONG_PTR KdpSearchAddressRangeEnd;

    ULONG_PTR MmLowestPhysicalPage;
    ULONG_PTR MmHighestPhysicalPage;

    ULONG_PTR PageFrame;
    ULONG_PTR StartPage;
    ULONG_PTR EndPage;
    ULONG_PTR RunStartPage;
    ULONG_PTR RunEndPage;
    ULONG RunIndex;

    BOOLEAN RequestForInterrupt;
    BOOLEAN RequestAllOffsets;
    BOOLEAN RawSearch;
    ULONG Hits;
    ULONG LastHits;
    ULONG Index;
    ULONG_PTR PfnHit;
    ULONG_PTR VaHit;
    ULONG VaFlags;
    ULONG PfnOffset;
    ULONG PfnValue;
    ULONG_PTR AddressStart;
    ULONG_PTR AddressEnd;
    ULONG DefaultRange;
    PPHYSICAL_MEMORY_DESCRIPTOR MemoryDescriptor;
    
    ULONG_PTR ParamAddress;
    ULONG_PTR ParamDelta;
    ULONG_PTR ParamStart;
    ULONG_PTR ParamEnd;

    RequestForInterrupt = FALSE;
    RequestAllOffsets = FALSE;
    RawSearch = FALSE;

    DefaultRange = 128;

    ParamAddress = 0;
    ParamDelta = 0;
    ParamStart = 0;
    ParamEnd = 0;
    
    //
    // Help requested ?
    //

    if (strstr (args, "?") != 0) {

        dprintf ("!search ADDRESS [DELTA [START_PFN END_PFN]]                     \n");
        dprintf ("                                                                \n");
        dprintf ("Search the physical pages in range [START_PFN..END_PFN]         \n");
        dprintf ("for ULONG_PTRs with values in range ADDRESS+/-DELTA or values   \n");
        dprintf ("that differ in only one bit position from ADDRESS.              \n");
        dprintf ("                                                                \n");
        dprintf ("The default value for DELTA is 0. For START/END_PFN the default \n");
        dprintf ("values are lowest physical page and highest physical page.      \n");
        dprintf ("                                                                \n");
        dprintf ("Examples:                                                       \n");
        dprintf ("                                                                \n");
        dprintf ("!search AABBCCDD 0A                                             \n");
        dprintf ("                                                                \n");
        dprintf ("    Search all physical memory for values in range AABBCCD3 -   \n");
        dprintf ("    AABBCCE8 or with only one bit different than AABBCCDD.      \n");
        dprintf ("                                                                \n");
        dprintf ("!search AABBCCDD 0A 13F 240                                     \n");
        dprintf ("                                                                \n");
        dprintf ("    Search page frames in range 13F - 240 for values in range   \n");
        dprintf ("    AABBCCD3 - AABBCCE8 or with only one bit different          \n");
        dprintf ("    than AABBCCDD.                                              \n");
        dprintf ("                                                                \n");
        dprintf ("By default only the first hit in the page is detected. If all   \n");
        dprintf ("hits within the page are needed the START_PFN and END_PFN       \n");
        dprintf ("must have the same value.                                       \n");
        dprintf ("                                                                \n");
        return;
        
    }
    
    //
    // Get command line arguments.
    //

    {
        PCHAR Current = (PCHAR)args;
        CHAR Buffer [64];
        ULONG Index;
        ULONG BufferIndex;
        BOOLEAN PhysicalMemorySearch;

        PhysicalMemorySearch = FALSE;

        //
        // Identify subcommand used. 
        //

        {
            while (*Current == ' ' || *Current == '\t') {
                Current++;
            }

            if (*Current == '-' && *(Current + 1) == 'm') {

                PhysicalMemorySearch = TRUE;
                Current += 2;
            }
            else if (*Current == '-' && *(Current + 1) == 'r') {

                PhysicalMemorySearch = TRUE;
                RawSearch = TRUE;
                Current += 2;
            }
            else {

                //
                // Default is equivalent with `-m'
                //

                PhysicalMemorySearch = TRUE;
            }

            if (!PhysicalMemorySearch) {
                
                dprintf ("Use `!search ?' for help. \n");
                return;
            }
        }

        //
        // Get the 4 numeric arguments.
        //

        for (Index = 0; Index < 4; Index++) {

            //
            // Get rid of any leading spaces.
            //

            while (*Current == ' ' || *Current == '\t') {
                Current++;
            }
            
            if (*Current == 0) {

                if (Index == 0) {
                    
                    dprintf ("Use `!search ?' for help. \n");
                    return;
                }
                else {

                    break;
                }
            }

            //
            // Get the digits from the Index-th parameter.
            //

            Buffer [0] = '0';
            Buffer [1] = 'x';
            BufferIndex = 2;

            while ((*Current >= '0' && *Current <= '9')
                   || (*Current >= 'a' && *Current <= 'f')
                   || (*Current >= 'A' && *Current <= 'F')) {

                
                Buffer[BufferIndex] = *Current;
                Buffer[BufferIndex + 1] = 0;

                Current++;
                BufferIndex++;
            }

            switch (Index) {
                
#if defined(_IA64_)
            case 0: sscanf (Buffer, "%I64x", &ParamAddress); break;
            case 1: sscanf (Buffer, "%I64x", &ParamDelta); break;
            case 2: sscanf (Buffer, "%I64x", &ParamStart); break;
            case 3: sscanf (Buffer, "%I64x", &ParamEnd); break;
#else
            case 0: sscanf (Buffer, "%x", &ParamAddress); break;
            case 1: sscanf (Buffer, "%x", &ParamDelta); break;
            case 2: sscanf (Buffer, "%x", &ParamStart); break;
            case 3: sscanf (Buffer, "%x", &ParamEnd); break;
#endif
            default: return;
            }
        }
    }

    //
    // Verify that we have the right symbols.
    //

    KdpSearchCheckPoint = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchCheckPoint");

    if (KdpSearchCheckPoint == 0 
        || ReadValue (KdpSearchCheckPoint) != SEARCH_SYMBOL_CHECK) {

        dprintf ("Search error: Incorrect symbols for kernel\n");
        return;
    }

    //
    // Get all symbol values so that we can manipulate only addresses
    // from now on.
    //

    KdpSearchPageHits = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchPageHits");
    KdpSearchPageHitOffsets = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchPageHitOffsets");
    KdpSearchPageHitIndex = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchPageHitIndex");

    KdpSearchCheckPoint = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchCheckPoint");
    KdpSearchInProgress = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchInProgress");

    KdpSearchStartPageFrame = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchStartPageFrame");
    KdpSearchEndPageFrame = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchEndPageFrame");

    KdpSearchAddressRangeStart = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchAddressRangeStart");
    KdpSearchAddressRangeEnd = (ULONG_PTR) GetExpression ("xboxkrnl!KdpSearchAddressRangeEnd");
    
    //
    // Perform some sanity checks on the values.
    //


    if (ReadValue (KdpSearchInProgress) != 0) {
        dprintf ("Search error: Inconsistent value for xboxkrnl!KdpSearchInProgress \n");
        return;
    }

    //
    // Reset the search engine
    //

    WriteValue (KdpSearchPageHitIndex, 0);
    WriteValue (KdpSearchInProgress, 1);

    //
    // Read physical memory limits.
    //

    MmLowestPhysicalPage = (ULONG_PTR) GetExpression ("xboxkrnl!MmLowestPhysicalPage");
    MmHighestPhysicalPage = (ULONG_PTR) GetExpression ("xboxkrnl!MmHighestPhysicalPage");

    //
    // Figure out proper search parameters.
    //

    AddressStart = ParamAddress - ParamDelta;
    AddressEnd = ParamAddress + ParamDelta;

    WritePointer (KdpSearchAddressRangeStart, AddressStart);
    WritePointer (KdpSearchAddressRangeEnd, AddressEnd);

    if (ParamStart == 0) {
        StartPage = ReadPointer (MmLowestPhysicalPage);
        ParamStart = StartPage;
    }
    else {
        StartPage = ParamStart;
    }

    if (ParamEnd == 0) {
        EndPage = ReadPointer (MmHighestPhysicalPage);
        ParamEnd = EndPage;
    }
    else {
        EndPage = ParamEnd;
    }

    dprintf ("Searching pfn's in range %08X - %08X for [%08X - %08X]\n\n", 
             StartPage, EndPage, 
             AddressStart, AddressEnd);

    dprintf ("%-8s %-8s %-8s \n", "Pfn","Offset", "Va");
    dprintf ("--------------------------------\n");
    
    //
    // Get system memory description to figure out what ranges
    // should we skip. This is important for sparse PFN database
    // and for pages managed by drivers.
    //

    MemoryDescriptor = SearchGetSystemMemoryDescriptor ();

    if (MemoryDescriptor == NULL) {
        dprintf ("Search error: cannot allocate system memory descriptor \n");
        return;
    }

    //
    // Search all physical memory in the specified range.
    //

    WriteValue (KdpSearchPageHitIndex, 0);

    if (StartPage == EndPage) {

        EndPage += 1;
        RequestAllOffsets = TRUE;
    }

    if (RawSearch) {
        DefaultRange = 4096;
    }
    
    for (PageFrame = StartPage; PageFrame < EndPage; PageFrame += DefaultRange) {

        for (RunIndex = 0; RunIndex < MemoryDescriptor->NumberOfRuns; RunIndex++) {

            WriteValue (KdpSearchPageHitIndex, 0);

            //
            // Figure out real start and end page.
            //

            RunStartPage = PageFrame;
            RunEndPage = PageFrame + DefaultRange;

            if (RunEndPage < MemoryDescriptor->Run[RunIndex].BasePage) {
                continue;
            }
            
            if (RunStartPage >= MemoryDescriptor->Run[RunIndex].BasePage + MemoryDescriptor->Run[RunIndex].PageCount) {
                continue;
            }
            
            if (RunStartPage < MemoryDescriptor->Run[RunIndex].BasePage) {
                RunStartPage = MemoryDescriptor->Run[RunIndex].BasePage;
            }

            if (RunEndPage > MemoryDescriptor->Run[RunIndex].BasePage + MemoryDescriptor->Run[RunIndex].PageCount) {
                RunEndPage = MemoryDescriptor->Run[RunIndex].BasePage + MemoryDescriptor->Run[RunIndex].PageCount;
            }


            WritePointer (KdpSearchStartPageFrame, RunStartPage);

            if (RequestAllOffsets) {

                //
                // If the search is in only one page then we
                // will try to get all offsets with a hit.
                //

                WritePointer (KdpSearchEndPageFrame, RunStartPage);
            }
            else {

                WritePointer (KdpSearchEndPageFrame, RunEndPage);
            }

            //
            // Invalidate kd cache
            //

            WriteValue (KdpSearchPageHits, 0);
            WriteValue (KdpSearchPageHitOffsets, 0);

            //
            // This is the trigger for memory search. We piggy back on the same
            // code as for !chklowmem and the logic in kernel detects what 
            // we really want to do.
            //

            Ioctl (IG_LOWMEM_CHECK, NULL, 0);

            //
            // Display results
            //

            Hits = ReadValue (KdpSearchPageHitIndex);

            for (Index = 0; Index < Hits; Index++) {

                PfnHit = ReadPointer (KdpSearchPageHits + Index * sizeof (PFN_NUMBER));
                PfnOffset = ReadValue (KdpSearchPageHitOffsets + Index * sizeof (ULONG));

                if (!RawSearch) {

                    VaHit = SearchConvertPageFrameToVa (PfnHit, &VaFlags);
                }
                else {

                    VaHit = 0;
                }

#if defined(_IA64_)
                dprintf ("%I64X %I64X %I64X \n",
#else
                dprintf ("%08X %08X %08X \n", 
#endif
                         PfnHit,
                         (PfnOffset & 0xFFFF), 
                         (VaHit == 0 ? 0 : VaHit + PfnOffset));
            }

            //
            // check for ctrl-c
            //

            if (CheckControlC()) {

                dprintf ("Search interrupted \n");
                RequestForInterrupt = TRUE;
                break;
            }
        }

        if (RequestForInterrupt) {
            break;
        }
    }
    
    //
    // Reset the search engine state
    //

    WriteValue (KdpSearchInProgress, 0);
    
    //
    // Print final result
    //

    free (MemoryDescriptor);

    if (RequestForInterrupt) {
        
        return;
    }
    else {

        dprintf ("Search done.\n");
    }
}


BOOLEAN
SearchIsSystemPageFrame (
    PFN_NUMBER PageFrameIndex
    )
/*++

Routine Description:

    This routine verifies if a page frame is among the ones managed by the
    system. This is the way we can deal with sparse PFN database were certain
    ranges of pages are taken for private use by drivers.

Arguments:

    PageFrameIndex - PFN to verify

Return Value:

    TRUE if page frame is managed by the system. FALSE otherwise.

Environment:

    Call triggered only from !search Kd extension.

--*/

{
    PPHYSICAL_MEMORY_RUN MemoryRun;
    PPHYSICAL_MEMORY_DESCRIPTOR MemoryDescriptor;
    
    ULONG MemoryDescriptorAddress;
    ULONG MemoryDescriptorSize;
    ULONG MemoryRunAddress;
    ULONG NumberOfRuns;
    ULONG BytesRead;
    ULONG Index;
    BOOLEAN Found;

    Found = FALSE;

    MemoryDescriptorAddress = ReadPointer (GetExpression ("xboxkrnl!MmPhysicalMemoryBlock"));
    NumberOfRuns = ReadValue (MemoryDescriptorAddress);

    if (NumberOfRuns == 0) {
        return FALSE;
    }

    MemoryDescriptorSize = sizeof (PHYSICAL_MEMORY_DESCRIPTOR) 
        + (NumberOfRuns - 1) * sizeof (PHYSICAL_MEMORY_RUN);

    MemoryDescriptor = (PPHYSICAL_MEMORY_DESCRIPTOR) malloc (MemoryDescriptorSize);
    
    if (MemoryDescriptor == NULL) {
        return FALSE;
    }

    BytesRead = 0;

    ReadMemory (MemoryDescriptorAddress,
                MemoryDescriptor,
                MemoryDescriptorSize,
                &BytesRead);

    if (BytesRead == 0) {
        
        free (MemoryDescriptor);
        return FALSE;
    }

    for (Index = 0; Index < MemoryDescriptor->NumberOfRuns; Index++) {

        MemoryRun = &(MemoryDescriptor->Run[Index]);

        if (PageFrameIndex >= MemoryRun->BasePage
            && PageFrameIndex < MemoryRun->BasePage + MemoryRun->PageCount) {
            
            Found = TRUE;
            break;
        }
    }

    free (MemoryDescriptor);
    
    return Found;
}


PPHYSICAL_MEMORY_DESCRIPTOR
SearchGetSystemMemoryDescriptor (
    VOID
    )
/*++

Routine Description:


Arguments:

    None.
    
Return Value:

    A malloc'd PHYSICAL_MEMORY_DESCRIPTOR structure.
    Caller is responsible of freeing.

Environment:

    Call triggered only from !search Kd extension.

--*/

{
    PPHYSICAL_MEMORY_RUN MemoryRun;
    PPHYSICAL_MEMORY_DESCRIPTOR MemoryDescriptor;
    
    ULONG MemoryDescriptorAddress;
    ULONG MemoryDescriptorSize;
    ULONG NumberOfRuns;
    ULONG BytesRead;

    MemoryDescriptorAddress = ReadPointer (GetExpression ("xboxkrnl!MmPhysicalMemoryBlock"));
    NumberOfRuns = ReadValue (MemoryDescriptorAddress);

    if (NumberOfRuns == 0) {
        return NULL;
    }

    MemoryDescriptorSize = sizeof (PHYSICAL_MEMORY_DESCRIPTOR) 
        + (NumberOfRuns - 1) * sizeof (PHYSICAL_MEMORY_RUN);

    MemoryDescriptor = (PPHYSICAL_MEMORY_DESCRIPTOR) malloc (MemoryDescriptorSize);
    
    if (MemoryDescriptor == NULL) {
        return NULL;
    }

    BytesRead = 0;

    ReadMemory (MemoryDescriptorAddress,
                MemoryDescriptor,
                MemoryDescriptorSize,
                &BytesRead);

    if (BytesRead == 0) {

        free (MemoryDescriptor);
        return NULL;
    }

    return MemoryDescriptor;
}


ULONG_PTR 
SearchConvertPageFrameToVa (
    PFN_NUMBER PageFrameIndex,
    PULONG Flags
    )
/*++

Routine Description:

    This routine returnes the virtual address corresponding to a
    PFN index if the reverse mapping is easy to figure out. For all
    other cases (e.g. prototype PTE) the result is null.

Arguments:

    PageFrameIndex - PFN index to convert.

Return Value:

    The corresponding virtual address or null in case the PFN index
    cannot be easily converted to a virtual address.

Environment:

    Call triggered only from Kd extension.

--*/

{
    PMMPTE PdeCurrent;
    PMMPTE PdeStart, PdeEnd;
    MMPTE PdeValue;
    ULONG_PTR Delta;
    ULONG_PTR Va;
    MMPFN Pfn;
    ULONG_PTR PfnAddress;
    ULONG BytesRead;

    PfnAddress = (ULONG_PTR)(ReadPointer(GetExpression("xboxkrnl!MmPfnDatabase")))
        + PageFrameIndex * sizeof (MMPFN);

    BytesRead = 0;
    *Flags = 0;

    ReadMemory (PfnAddress,
                &Pfn,
                sizeof(Pfn),
                &BytesRead);

    if (BytesRead < sizeof(Pfn)) {

        return 0;
    }


    //
    // bugbug (silviuc): should check if MI_IS_PFN_DELETED(Pfn)
    //

    //
    // Try to figure out Va if possible.
    //

    PdeStart = DbgGetPdeAddress ((PVOID)(MM_SYSTEM_PHYSICAL_MAP));
    PdeEnd = DbgGetPdeAddress ((PVOID)(MM_SYSTEM_PHYSICAL_MAP + MM_BYTES_IN_PHYSICAL_MAP));
    Va = MM_SYSTEM_PHYSICAL_MAP;

    for (PdeCurrent = PdeStart; PdeCurrent < PdeEnd; PdeCurrent++) {

        BytesRead = 0;

        ReadMemory ((ULONG_PTR)PdeCurrent,
                    &PdeValue,
                    sizeof PdeValue,
                    &BytesRead);

        if (BytesRead < sizeof(PdeValue)) {

            return 0;
        }

        if (PdeValue.Hard.Valid && PdeValue.Hard.LargePage) {

            Delta = (ULONG_PTR)PageFrameIndex - (ULONG_PTR)(PdeValue.Hard.PageFrameNumber);

            if (Delta < 1024 * PAGE_SIZE) {

                *Flags |= SEARCH_VA_LARGE_PAGE_ADDRESS;
                return (Va + Delta * PAGE_SIZE);
            }
        }

        Va += 1024 * PAGE_SIZE;
    }

    *Flags |= SEARCH_VA_UNKNOWN_TYPE_ADDRESS;
    return 0;
}

