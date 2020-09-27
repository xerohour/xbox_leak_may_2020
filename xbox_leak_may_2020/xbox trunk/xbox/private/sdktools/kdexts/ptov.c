/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    ptov.c

Abstract:

    Kernel debugger extension for dumping all physical to
    virtual translations for a given process.

Author:

    John Vert (jvert) 25-Jul-1995

Revision History:

--*/
#include "precomp.h"

BOOL
ReadPhysicalPage(
    IN ULONG PageNumber,
    OUT PVOID Buffer
    );

DECLARE_API( ptov )

/*++

Routine Description:

    Dumps all physical to virtual translations for a given process

Arguments:

    args - supplies physical address of PDE

Return Value:

    None.

--*/

{
    ULONG PdeAddress;
    ULONG ActualRead;
    PHARDWARE_PTE PageDirectory;
    PHARDWARE_PTE PageTable;
    ULONG i,j;
    ULONG VirtualPage=0;

    if (sscanf(args,"%lx",&PdeAddress) != 1 || PdeAddress == 0) {
        dprintf("usage: ptov PFNOfPDE\n");
        return;
    }

    PageDirectory = LocalAlloc(LMEM_FIXED, PAGE_SIZE);
    if (PageDirectory == NULL) {
        dprintf("Couldn't allocate %d bytes for page directory\n",PAGE_SIZE);
        return;
    }
    PageTable = LocalAlloc(LMEM_FIXED, PAGE_SIZE);
    if (PageTable == NULL) {
        dprintf("Couldn't allocate %d bytes for page table\n",PAGE_SIZE);
        LocalFree(PageTable);
    }

    __try {
        if (ReadPhysicalPage(PdeAddress,PageDirectory)) {
            for (i=0;i<PAGE_SIZE/sizeof(HARDWARE_PTE);i++) {
                if (CheckControlC()) {
                    return;
                }
                if (PageDirectory[i].Valid == 1) {
                    if (!ReadPhysicalPage((ULONG)PageDirectory[i].PageFrameNumber,PageTable)) {
                        break;
                    }
                    for (j=0;j<PAGE_SIZE/sizeof(HARDWARE_PTE);j++) {
                        if ( CheckControlC() ) {
                            return;
                        }
                        if (PageTable[j].Valid == 1) {
                            dprintf("%lx %lx\n",PageTable[j].PageFrameNumber*PAGE_SIZE,VirtualPage);
                        }
                        VirtualPage+=PAGE_SIZE;
                    }
                } else {
                    VirtualPage += PAGE_SIZE * (PAGE_SIZE/sizeof(HARDWARE_PTE));
                }
            }
        }
    } __finally {
        LocalFree(PageDirectory);
        LocalFree(PageTable);
    }
}

BOOL
ReadPhysicalPage(
    IN ULONG PageNumber,
    OUT PVOID Buffer
    )
{
    ULONG i;
    ULONG64 Address;
    ULONG ActualRead;

    //
    // do the read 1k at a time to avoid overflowing the packet maximum.
    //
    Address = PageNumber << PAGE_SHIFT;
    for (i=0; i<PAGE_SIZE/1024; i++) {
        ReadPhysical(Address, Buffer, 1024, &ActualRead);
        if (ActualRead != 1024) {
            dprintf("physical read at %d failed\n",Address);
            return(FALSE);
        }
        Address += 1024;
        Buffer = (PVOID)((ULONG)Buffer + 1024);
    }
    return(TRUE);
}

DECLARE_API( vtop )

/*++

Routine Description:

    Dumps the virtual to physical translation for a page

Arguments:

    args - supplies physical address of PDE

Return Value:

    None.

--*/

{
    ULONG ActualRead;
    PHARDWARE_PTE PageDirectory;
    PHARDWARE_PTE PageTable;
    MMPTE Pte;
    ULONG i,j;
    ULONG PdeAddress = 0;
    ULONG VirtualPage= 0;

    sscanf(args,"%lx %lx",&PdeAddress,&VirtualPage);

    if (PdeAddress == 0) {
        dprintf("usage: vtop PFNOfPDE VA\n");
        return;
    }

    // Common mistake, typed in full 32 bit address, not pfn
    if( PdeAddress & ~((1 << (32-PAGE_SHIFT)) - 1) ) {
        PdeAddress >>= PAGE_SHIFT;
    }

    PageDirectory = LocalAlloc(LMEM_FIXED, PAGE_SIZE);
    if (PageDirectory == NULL) {
        dprintf("Couldn't allocate %d bytes for page directory\n",PAGE_SIZE);
        return;
    }
    PageTable = LocalAlloc(LMEM_FIXED, PAGE_SIZE);
    if (PageTable == NULL) {
        dprintf("Couldn't allocate %d bytes for page table\n",PAGE_SIZE);
        LocalFree(PageTable);
    }

    i = VirtualPage / (PAGE_SIZE*PTE_PER_PAGE);
    j = (VirtualPage % (PAGE_SIZE*PTE_PER_PAGE)) / PAGE_SIZE;

    dprintf("Pdi %x Pti %x\n",i,j);

    __try {
        if (ReadPhysicalPage(PdeAddress,PageDirectory)) {

	    if (CheckControlC()) {
                return;
            }

	    if (PageDirectory[i].Valid == 1) {
                if (!ReadPhysicalPage(PageDirectory[i].PageFrameNumber,PageTable)) {
                    return;
                }

		if (PageTable[j].Valid == 1) {
                    dprintf("%08lx %08lx pfn(%05lx)\n",
		        VirtualPage,
			PageTable[j].PageFrameNumber*PAGE_SIZE,
			PageTable[j].PageFrameNumber
			);
                }
                else {
    		    Pte.Hard = PageTable[j];
		        dprintf("%08lx Not present (%lx)\n",VirtualPage,Pte.Long);
                }
            }
            else {
                dprintf("PageDirectory Entry %u not valid, try another process\n",i);
	    }
	}
    } __finally {
        LocalFree(PageDirectory);
        LocalFree(PageTable);
    }
}


