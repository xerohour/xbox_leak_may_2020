/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    pool.c

Abstract:

    WinDbg Extension Api

Author:

    Lou Perazzoli (Loup) 5-Nov-1993

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

#include <limits.h>

typedef struct _POOL_BLOCK_HEAD {
    POOL_HEADER Header;
    LIST_ENTRY  List;
} POOL_BLOCK_HEAD, *PPOOL_BLOCK_HEADER;

typedef struct _POOL_HACKER {
    POOL_HEADER Header;
    ULONG Contents[8];
} POOL_HACKER;


#define TAG 0
#define NONPAGED_ALLOC 1
#define NONPAGED_FREE 2
#define PAGED_ALLOC 3
#define PAGED_FREE 4
#define NONPAGED_USED 5

ULONG SortBy;

typedef struct _FILTER {
    ULONG Tag;
    BOOLEAN Exclude;
} FILTER, *PFILTER;

#define MAX_FILTER 64
FILTER Filter[MAX_FILTER];

ULONG PageSize = 0x1000;

PPOOL_TRACKER_BIG_PAGES PoolBigTableAddress;

#define DecodeLink(Pool)    ((PUCHAR)((ULONG)Pool & ~1))

#define PP(x) isprint(((x)&0xff))?((x)&0xff):('.')

#define SPECIAL_POOL_BLOCK_SIZE(PoolHeader) (PoolHeader->Ulong1 & (MI_SPECIAL_POOL_VERIFIER - 1))

BOOLEAN
CheckSingleFilter (
    PCHAR Tag,
    PCHAR Filter
    );

BOOLEAN
CopyPfnDatabaseLocal(
    OUT PMMPFN *PfnDatabaseLocalOutput,
    OUT ULONG *NumberOfPages
    );

int __cdecl
ulcomp(const void *e1,const void *e2)
{
    ULONG u1;

    switch (SortBy) {
        case TAG:

            u1 = ((PUCHAR)e1)[0] - ((PUCHAR)e2)[0];
            if (u1 != 0) {
                return u1;
            }
            u1 = ((PUCHAR)e1)[1] - ((PUCHAR)e2)[1];
            if (u1 != 0) {
                return u1;
            }
            u1 = ((PUCHAR)e1)[2] - ((PUCHAR)e2)[2];
            if (u1 != 0) {
                return u1;
            }
            u1 = ((PUCHAR)e1)[3] - ((PUCHAR)e2)[3];
            return u1;
            break;

        case NONPAGED_ALLOC:
            u1 = ((PPOOL_TRACKER_TABLE)e2)->NonPagedAllocs -
                        ((PPOOL_TRACKER_TABLE)e1)->NonPagedAllocs;
            return (u1);
            break;

        case NONPAGED_FREE:
            u1 = ((PPOOL_TRACKER_TABLE)e2)->NonPagedFrees -
                        ((PPOOL_TRACKER_TABLE)e1)->NonPagedFrees;
            return (u1);
            break;

        case NONPAGED_USED:
            u1 = ((PPOOL_TRACKER_TABLE)e2)->NonPagedBytes -
                        ((PPOOL_TRACKER_TABLE)e1)->NonPagedBytes;
            return (u1);
            break;

        default:
            return(0);
            break;
    }
}


DECLARE_API( frag )

/*++

Routine Description:

    Dump pool fragmentation

Arguments:

    args - Flags

Return Value:

    None

--*/

{
    ULONG Flags;
    ULONG result;
    ULONG i;
    ULONG count;
    PUCHAR Pool;
    ULONG PoolLoc1;
    ULONG TotalFrag;
    ULONG TotalCount;
    ULONG Frag;
    ULONG PoolStart;
    POOL_DESCRIPTOR PoolDesc;
    PPOOL_DESCRIPTOR PoolLoc;
    POOL_BLOCK_HEAD PoolBlock;

    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    dprintf("\n  NonPaged Pool Fragmentation\n\n");
    Flags = 0;
    PoolStart = 0;

    sscanf(args,"%lx %lx", &Flags, &PoolStart);

    if (PoolStart != 0) {
        PoolStart += POOL_OVERHEAD;

        Pool = (PUCHAR)DecodeLink(PoolStart);
        do {

            Pool = Pool - POOL_OVERHEAD;
            if ( !ReadMemory( (DWORD)Pool,
                              &PoolBlock,
                              sizeof(PoolBlock),
                              &result) ) {
                dprintf("%08lx: Unable to get contents of pool block\n", Pool );
                return;
            }

            dprintf(" %lx size: %4lx previous size: %4lx  %c%c%c%c links: %8lx %8lx\n",
                    (ULONG)Pool,
                    (ULONG)PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT,
                    (ULONG)PoolBlock.Header.PreviousSize << POOL_BLOCK_SHIFT,
                    (ULONG)PoolBlock.Header.PoolTag,
                    (ULONG)PoolBlock.Header.PoolTag >> 8,
                    (ULONG)PoolBlock.Header.PoolTag >> 16,
                    (ULONG)PoolBlock.Header.PoolTag >> 24,
                    (ULONG)PoolBlock.List.Flink,
                    (ULONG)PoolBlock.List.Blink);

            if (Flags != 3) {
                Pool = (PUCHAR)PoolBlock.List.Flink;
            } else {
                Pool = (PUCHAR)PoolBlock.List.Blink;
            }

            Pool = DecodeLink(Pool);

            if (CheckControlC()) {
                return;
            }

        } while ( ((ULONG)Pool & 0xfffffff0) != (PoolStart & 0xfffffff0) );

        return;
    }

    PoolLoc1 = GetNtDebuggerData( NonPagedPoolDescriptor );

    if (PoolLoc1 == 0) {
        dprintf ("unable to get nonpaged pool head\n");
        return;
    }

    PoolLoc = (PPOOL_DESCRIPTOR)PoolLoc1;

    if ( !ReadMemory( (DWORD)PoolLoc,
                      &PoolDesc,
                      sizeof(POOL_DESCRIPTOR),
                      &result) ) {
        dprintf ("%08lx: Unable to get pool descriptor\n", PoolLoc1);
        return;
    }

    TotalFrag   = 0;
    TotalCount  = 0;

    for (i = 0; i < POOL_LIST_HEADS; i += 1) {

        Frag  = 0;
        count = 0;
        Pool  = (PUCHAR)PoolDesc.ListHeads[i].Flink;
        Pool = DecodeLink(Pool);

        while (Pool != (PUCHAR)(&PoolLoc->ListHeads[i])) {

            Pool = Pool - POOL_OVERHEAD;
            if ( !ReadMemory( (DWORD)Pool,
                              &PoolBlock,
                              sizeof(PoolBlock),
                              &result) ) {
                dprintf("%08lx: Unable to get contents of pool block\n", Pool );
                return;
            }

            Frag  += (ULONG)PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT;
            count += 1;

            if (Flags & 2) {
                dprintf(" ListHead[%x]: %lx size: %4lx previous size: %4lx  %c%c%c%c\n",
                        i,
                        (ULONG)Pool,
                        (ULONG)PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT,
                        (ULONG)PoolBlock.Header.PreviousSize << POOL_BLOCK_SHIFT,
                        (ULONG)PoolBlock.Header.PoolTag,
                        (ULONG)PoolBlock.Header.PoolTag >> 8,
                        (ULONG)PoolBlock.Header.PoolTag >> 16,
                        (ULONG)PoolBlock.Header.PoolTag >> 24);
            }
            Pool = (PUCHAR)PoolBlock.List.Flink;
            Pool = DecodeLink(Pool);

            if (CheckControlC()) {
                return;
            }
        }
        if (Flags & 1) {
            dprintf("index: %2ld number of fragments: %5ld  bytes: %6ld\n",
                i,count,Frag);
        }
        TotalFrag  += Frag;
        TotalCount += count;
    }

    dprintf("\n Number of fragments: %7ld consuming %7ld bytes\n",
            TotalCount,TotalFrag);
    dprintf(  " NonPagedPool Usage:  %7ld bytes\n",(PoolDesc.TotalPages + PoolDesc.TotalBigPages)*PageSize);
    return;
}


PRTL_BITMAP
GetBitmap(
    ULONG pBitmap
    )
{
    ULONG Result;
    RTL_BITMAP Bitmap;
    PRTL_BITMAP p;

    if ( !ReadMemory( (DWORD)pBitmap,
                      &Bitmap,
                      sizeof(Bitmap),
                      &Result) ) {
        dprintf("%08lx: Unable to get contents of bitmap\n", pBitmap );
        return NULL;
    }

    p = HeapAlloc( GetProcessHeap(), 0, sizeof( *p ) + (Bitmap.SizeOfBitMap / 8) );
    if (p) {
        p->SizeOfBitMap = Bitmap.SizeOfBitMap;
        p->Buffer = (PULONG)(p + 1);
        if ( !ReadMemory( (DWORD)Bitmap.Buffer,
                          p->Buffer,
                          Bitmap.SizeOfBitMap / 8,
                          &Result) ) {
            dprintf("%08lx: Unable to get contents of bitmap buffer\n", Bitmap.Buffer );
            HeapFree( GetProcessHeap(), 0, p );
            p = NULL;
        }
    }

    return p;
}


VOID
DumpPool(
    VOID
    )
{
#if 1
    // The new memory manager doesn't track paged pool the same way.  The code
    // needs to be rewritten to walk the PFN database to find pool pages.
    dprintf("DumpPool is not implemented.\n");
#else
    PCHAR p, pStart;
    ULONG Size;
    ULONG BusyFlag;
    ULONG CurrentPage;
    PRTL_BITMAP StartMap;
    PRTL_BITMAP EndMap;
    PVOID PagedPoolStart;
    PVOID PagedPoolEnd;
    ULONG Result;
    MM_PAGED_POOL_INFO PagedPoolInfo;
    PMM_PAGED_POOL_INFO PagedPoolInfoPointer;

    PagedPoolInfoPointer = (PMM_PAGED_POOL_INFO) GetNtDebuggerData( MmPagedPoolInformation );

    if ( !ReadMemory( (DWORD)PagedPoolInfoPointer,
                      &PagedPoolInfo,
                      sizeof(PagedPoolInfo),
                      &Result) ) {
        dprintf("%08lx: Unable to get contents of paged pool information\n",
            PagedPoolInfoPointer );
        return;
    }

    StartMap = GetBitmap( (ULONG) PagedPoolInfo.PagedPoolAllocationMap );
    EndMap = GetBitmap( (ULONG) PagedPoolInfo.EndOfPagedPoolBitmap );

    PagedPoolStart = (PVOID)GetNtDebuggerDataValue( MmPagedPoolStart );
    PagedPoolEnd = (PVOID)GetNtDebuggerDataValue( MmPagedPoolEnd );

    if (StartMap && EndMap) {
        p = PagedPoolStart;
        CurrentPage = 0;
        dprintf( "Paged Pool: %x .. %x\n", PagedPoolStart, PagedPoolEnd );

        while (p < (PCHAR)PagedPoolEnd) {
            if ( CheckControlC() ) {
                return;
            }
            pStart = p;
            BusyFlag = RtlCheckBit( StartMap, CurrentPage );
            while ( ~(BusyFlag ^ RtlCheckBit( StartMap, CurrentPage )) ) {
                p += PageSize;
                if (RtlCheckBit( EndMap, CurrentPage )) {
                    CurrentPage++;
                    break;
                    }

                CurrentPage++;
                if (p > (PCHAR)PagedPoolEnd) {
                   break;
                   }
                }

            Size = p - pStart;
            dprintf( "%08x: %x - %s\n", pStart, Size, BusyFlag ? "busy" : "free" );
            }
        }

    HeapFree( GetProcessHeap(), 0, StartMap );
    HeapFree( GetProcessHeap(), 0, EndMap );
#endif
}

DECLARE_API( pool )

/*++

Routine Description:

    Dump kernel mode heap

Arguments:

    args - Page Flags

Return Value:

    None

--*/

{
    DWORD       PoolTableAddress;
    ULONG       PoolTag;
    ULONG       Flags;
    ULONG       Result;
    PVOID       PoolPageToDump;
    PVOID       StartPage;
    PUCHAR      Pool;
    POOL_HACKER PoolBlock;
    ULONG       PoolBlockSize;
    PPOOL_HEADER PoolHeader;
    ULONG       Previous;
    UCHAR       c;
    PUCHAR      DataPage;
    PUCHAR      DataStart;
    LOGICAL     Pagable;
    LOGICAL     FirstBlock;
    ULONG       BlockType;
    ULONG       i;
    ULONG       j;
    ULONG       ct;
    ULONG       PoolBigPageTableSize;
    PPOOL_TRACKER_BIG_PAGES BigTable;

    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    DataPage = (PUCHAR) malloc (PageSize);

    if (DataPage == NULL) {
        dprintf ("Not enough memory\n");
        return;
    }

    __try {

        PoolPageToDump = (PVOID)-1;
        Flags = 0;
        sscanf(args,"%lx %lx",&PoolPageToDump,&Flags);
        if (PoolPageToDump == (PVOID)-1) {
            DumpPool();
            __leave;
        }

        Pool        = (PUCHAR)PAGE_ALIGN (PoolPageToDump);
        StartPage   = (PVOID)Pool;
        Previous    = 0;

        FirstBlock = TRUE;

        while ((PVOID)PAGE_ALIGN(Pool) == StartPage) {
            if ( CheckControlC() ) {
                __leave;
            }

            if ( !ReadMemory( (DWORD)Pool,
                              &PoolBlock,
                              sizeof(POOL_HACKER),
                              &Result) ) {
                dprintf("%08lx: Unable to get contents of pool block\n", Pool );
                __leave;
            }
    
            if ((ULONG)PoolPageToDump >= (ULONG)Pool &&
                (ULONG)PoolPageToDump < ((ULONG)Pool + ((ULONG)PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT))
               ) {
                c = '*';
            } else {
                c = ' ';
            }

#ifdef TARGET_ALPHA
            if (PoolBlock.Header.BlockSize == 0 &&
                PoolBlock.Header.PreviousSize == 0 &&
                (PVOID)PAGE_ALIGN(Pool+0x20) != StartPage
               ) {
                dprintf("%c%lx size:   20 previous size:    0 (wasted)\n",
                        c,
                        Pool
                       );
                break;
            }
#endif
    
            BlockType = 0;
    
            if ((PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT) >= PAGE_SIZE) {
                BlockType = 1;
            }
            else if (PoolBlock.Header.BlockSize == 0) {
                BlockType = 2;
            }
            else if (PoolBlock.Header.PreviousSize != Previous) {
                BlockType = 3;
            }

            if (BlockType != 0) {

                //
                // See if this is a big block allocation.  Iff we have not
                // parsed any other small blocks in here already.
                //
    
                if (FirstBlock == TRUE) {
    
                    if (!PoolBigTableAddress) {
                        PoolBigTableAddress = (PPOOL_TRACKER_BIG_PAGES)GetUlongValue ("xboxkrnl!PoolBigPageTable");
                    }

                    PoolTableAddress = (DWORD)PoolBigTableAddress;

                    if (PoolTableAddress) {

                        PoolBigPageTableSize = GetUlongValue ("xboxkrnl!PoolBigPageTableSize");
                        //
                        // Scan the table looking for a match.
                        //

                        i = 0;
                        ct = PageSize / sizeof (POOL_TRACKER_BIG_PAGES);
    
                        while (i < PoolBigPageTableSize) {
    
                            if (PoolBigPageTableSize - i < ct) {
                                ct = PoolBigPageTableSize - i;
                            }
    
                            if ( !ReadMemory( PoolTableAddress,
                                              DataPage,
                                              ct * sizeof (POOL_TRACKER_BIG_PAGES),
                                              &Result) ) {
                                dprintf("%08lx: Unable to get contents of pool block\n", PoolTableAddress );
                                __leave;
                            }
        
                            BigTable = (PPOOL_TRACKER_BIG_PAGES)DataPage;

                            for (j = 0; j < ct; j += 1) {
    
                                if (BigTable[j].Va == PAGE_ALIGN(Pool)) {

                                    //
                                    // Match !
                                    //
                                    PoolTag = BigTable[j].Key;
                                    dprintf("*%lx : large page allocation, Tag is %c%c%c%c, size is 0x%x bytes\n",
                                        Pool,
                                        PP(PoolTag),
                                        PP(PoolTag >> 8),
                                        PP(PoolTag >> 16),
                                        PP(PoolTag >> 24),
                                        BigTable[j].NumberOfPages * PageSize
                                    );
                                    __leave;
                                }
                            }
                            i += ct;
                            PoolTableAddress += (ct * sizeof (POOL_TRACKER_BIG_PAGES));
                        }
    
                        //
                        // No match in small or large pool, must be
                        // freed or corrupt pool
                        //
    
                        dprintf("%lx is freed (or corrupt) pool\n", Pool);
                        __leave;
                    }
    
                    dprintf("unable to get pool big page table - either wrong symbols or pool tagging is disabled\n");
                }
    
                if (BlockType == 1) {
                    dprintf("Bad allocation size @%lx, too large\n", Pool);
                    __leave;
                }
                else if (BlockType == 2) {
                    dprintf("Bad allocation size @%lx, zero is invalid\n", Pool);
                    __leave;
                }
                else if (BlockType == 3) {
                    dprintf("Bad previous allocation size @%lx, last size was %lx\n",
                            Pool, Previous);
                    __leave;
                }
            }

            if (!(Flags & 2) || c == '*') {
              dprintf("%c%lx size: %4lx previous size: %4lx ",
                    c,
                    (ULONG)Pool,
                    (ULONG)PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT,
                    (ULONG)PoolBlock.Header.PreviousSize << POOL_BLOCK_SHIFT);
    
              if (PoolBlock.Header.PoolType == 0) {

                dprintf(" (Free)");

                dprintf("      %c%c%c%c%c\n",
                        c,
                        PP(PoolBlock.Header.PoolTag),
                        PP(PoolBlock.Header.PoolTag >> 8),
                        PP(PoolBlock.Header.PoolTag >> 16),
                        PP(PoolBlock.Header.PoolTag >> 24)
                        );
              }
              else {

                if (PoolBlock.Header.PoolIndex & 0x80) {
                    dprintf(" (Allocated)");
                } else {
                    dprintf(" (Lookaside)");
                }
                PoolTag = PoolBlock.Header.PoolTag;

                dprintf(" %c%c%c%c%c\n",
                    c,
                    PoolTag,
                    PoolTag >> 8,
                    PoolTag >> 16,
                    PoolTag >> 24
                    );
              }
            }
    
    
            if (Flags & 1) {
                dprintf("    %08lx  %08lx %08lx %08lx %08lx\n",
                    Pool+sizeof(POOL_HEADER),
                    PoolBlock.Contents[0],
                    PoolBlock.Contents[1],
                    PoolBlock.Contents[2],
                    PoolBlock.Contents[3]);
    
                dprintf("    %08lx  %08lx %08lx %08lx %08lx\n",
                    Pool+sizeof(POOL_HEADER)+16,
                    PoolBlock.Contents[4],
                    PoolBlock.Contents[5],
                    PoolBlock.Contents[6],
                    PoolBlock.Contents[7]);
                dprintf("\n");
            }
    
            Previous = PoolBlock.Header.BlockSize;
            Pool += (Previous << POOL_BLOCK_SHIFT);
            FirstBlock = FALSE;
        }

    } __finally {
    }

    free (DataPage);

    return;
}



DECLARE_API( poolused )

/*++

Routine Description:

    Dump usage by pool tag

Arguments:

    args -

Return Value:

    None

--*/

{
    ULONG PoolTrackTableSize;
    SIZE_T PoolTrackTableSizeInBytes;
    PPOOL_TRACKER_TABLE p;
    PPOOL_TRACKER_TABLE PoolTrackTableData;
    ULONG Flags;
    ULONG i;
    ULONG result;
    ULONG ct;
    DWORD PoolTableAddress;
    ULONG TagName;
    CHAR TagNameX[4] = {'*','*','*','*'};
    PPOOL_TRACKER_TABLE PoolTrackTable;

    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    Flags = 0;
    sscanf(args,"%lx %c%c%c%c", &Flags, &TagNameX[0],
        &TagNameX[1], &TagNameX[2], &TagNameX[3]);

    TagName = TagNameX[0] | (TagNameX[1] << 8) | (TagNameX[2] << 16) | (TagNameX[3] << 24);

    if ((PVOID)GetNtDebuggerData( PoolTrackTable ) == NULL) {
        dprintf ("unable to get PoolTrackTable - either pool tagging is off or bad symbols\n");
        return;
    }

    PoolTrackTable = (PPOOL_TRACKER_TABLE)GetNtDebuggerDataValue( PoolTrackTable );
    PoolTrackTableSize = GetNtDebuggerDataValue( PoolTrackTableSize );
    PoolTrackTableSizeInBytes = PoolTrackTableSize * sizeof(POOL_TRACKER_TABLE);

    PoolTrackTableData = malloc (PoolTrackTableSizeInBytes);
    if (PoolTrackTableData == NULL) {
        dprintf("unable to allocate memory for tag table.\n");
        return;
    }

    if (Flags & 2) {
        SortBy = NONPAGED_USED;
        dprintf("   Sorting by NonPaged Pool Consumed\n");
    } else {
        SortBy = TAG;
        dprintf("   Sorting by Tag\n");
    }

    dprintf("\n  Pool Used:\n");
    if (!(Flags & 1)) {
        dprintf(" Tag    Allocs     Used\n");

    } else {
        dprintf(" Tag    Allocs    Frees     Diff     Used\n");
    }

    ct = PageSize / sizeof (POOL_TRACKER_TABLE);
    i = 0;
    PoolTableAddress = (DWORD)PoolTrackTable;

    while (i < PoolTrackTableSize) {

        if ( CheckControlC() ) {
            free (PoolTrackTableData);
            return;
        }

        if (PoolTrackTableSize - i < ct) {
            ct = PoolTrackTableSize - i;
        }

        if ( !ReadMemory( PoolTableAddress,
                          &PoolTrackTableData[i],
                          ct * sizeof (POOL_TRACKER_TABLE),
                          &result) ) {
            dprintf("%08lx: Unable to get contents of pool block\n", PoolTableAddress );
            free (PoolTrackTableData);
            return;
        }

        PoolTableAddress += (ct * sizeof(POOL_TRACKER_TABLE));
        i += ct;
    }

    qsort((void *)PoolTrackTableData,
          (size_t)PoolTrackTableSize,
          (size_t)sizeof(POOL_TRACKER_TABLE),
          ulcomp);

    i = 0;
    p = &PoolTrackTableData[i];

    for ( ; i < PoolTrackTableSize; i += 1, p += 1) {

        if ((p->Key != 0) &&
            (CheckSingleFilter ((PCHAR)&p->Key, (PCHAR)&TagName))) {

            if (!(Flags & 1)) {
                if (p->NonPagedBytes != 0) {
                    dprintf(" %c%c%c%c %8ld %8ld\n",
                            p->Key,
                            p->Key >> 8,
                            p->Key >> 16,
                            p->Key >> 24,
                            p->NonPagedAllocs - p->NonPagedFrees,
                            p->NonPagedBytes);
                }

            } else {
                dprintf(" %c%c%c%c %8ld %8ld %8ld %8ld\n",
                        p->Key,
                        p->Key >> 8,
                        p->Key >> 16,
                        p->Key >> 24,
                        p->NonPagedAllocs,
                        p->NonPagedFrees,
                        p->NonPagedAllocs - p->NonPagedFrees,
                        p->NonPagedBytes);
            }
        }

    }

    free (PoolTrackTableData);
    return;
}


BOOLEAN WINAPI
CheckSingleFilterAndPrint (
    PCHAR Tag,
    PCHAR Filter,
    ULONG Flags,
    PPOOL_HEADER PoolHeader,
    ULONG BlockSize,
    PVOID Data,
    PVOID Context
    )

/*++

Routine Description:

    Callback to check a piece of pool and print out information about it
    if it matches the specified tag.

Arguments:

    Tag - Supplies the tag to search for.

    Filter - Supplies the filter string to match against.

    Flags - Supplies 0 if a nonpaged pool search is desired.
            Supplies 1 if a paged pool search is desired.
            Supplies 2 if a special pool search is desired.

    PoolHeader - Supplies the pool header.

    BlockSize - Supplies the size of the pool block in bytes.

    Data - Supplies the address of the pool block.

    Context - Unused.

Return Value:

    TRUE for a match, FALSE if not.

--*/

{
    ULONG UTag;

    UNREFERENCED_PARAMETER (Context);
    
    UTag = *((PULONG)Tag);

    if (CheckSingleFilter (Tag, Filter) == FALSE) {
        return FALSE;
    }

    if (BlockSize >= PageSize) {
        dprintf("*%lx :%slarge page allocation, Tag is %c%c%c%c, size is 0x%x bytes\n",
            (ULONG_PTR)Data | MINLONG_PTR,
            ((ULONG_PTR)Data & MINLONG_PTR) ? " " : "Free ",
            PP(UTag),
            PP(UTag >> 8),
            PP(UTag >> 16),
            PP(UTag >> 24),
            BlockSize
            );
    } else if (Flags & 0x2) {
        
        dprintf("*%lx size: %4lx %s special pool, Tag is %c%c%c%c\n",
            Data,
            BlockSize,
            "non-paged",
            PP(UTag),
            PP(UTag >> 8),
            PP(UTag >> 16),
            PP(UTag >> 24)
            );
    } else {

        dprintf("%lx size: %4lx previous size: %4lx ",
                (ULONG)Data - sizeof( POOL_HEADER ),
                (ULONG)PoolHeader->BlockSize << POOL_BLOCK_SHIFT,
                (ULONG)PoolHeader->PreviousSize << POOL_BLOCK_SHIFT);

        if (PoolHeader->PoolType == 0) {
            dprintf(" (Free)");
            dprintf("      %c%c%c%c\n",
                    PP(UTag),
                    PP(UTag >> 8),
                    PP(UTag >> 16),
                    PP(UTag >> 24) 
                   );
        } else {

            if (PoolHeader->PoolIndex & 0x80) {
                dprintf(" (Allocated)");
            } else {
                dprintf(" (Lookaside)");
            }
            UTag = PoolHeader->PoolTag;

            dprintf(" %c%c%c%c\n",
                PP(UTag),
                PP(UTag >> 8),
                PP(UTag >> 16),
                PP(UTag >> 24)
                );
        }
    }

    return TRUE;
}

PVOID
GetPoolVirtualAddress(
    PFN_NUMBER PageFrameNumber,
    ULONG PteIndex
    )
{
    PMMPTE PointerPde;
    MMPTE PdeContents;
    PMMPTE PointerPte;
    MMPTE PteContents;
    ULONG Result;

    PointerPde = MiGetPdeAddress(MM_SYSTEM_PTE_BASE);

    while (PointerPde <= MiGetPdeAddress(MM_SYSTEM_PTE_END)) {

        if (ReadMemory((ULONG)PointerPde, &PdeContents, sizeof(PdeContents), &Result)) {

            if (PdeContents.Hard.Valid != 0 &&
                PdeContents.Hard.LargePage == 0) {

                PointerPte = (PMMPTE)MiGetVirtualAddressMappedByPte(PointerPde) + PteIndex;

                if (ReadMemory((ULONG)PointerPte, &PteContents, sizeof(PteContents), &Result)) {

                    if (PteContents.Hard.Valid != 0 &&
                        PteContents.Hard.PageFrameNumber == PageFrameNumber) {
                        return MiGetVirtualAddressMappedByPte(PointerPte);
                    }
                }
            }
        }

        PointerPde++;
    }

    //
    // Give up and return NULL.
    //

    return NULL;
}

VOID
SearchPool(
    ULONG TagName,
    ULONG RestartAddr,
    POOLFILTER Filter,
    PVOID Context
)

/*++

Routine Description:

    Engine to search the pool.

Arguments:

    TagName - Supplies the tag to search for.

    RestartAddr - Supplies the address to restart the search from.

    Filter - Supplies the filter routine to use.

    Context - Supplies the user defined context blob.

Return Value:

    None.

--*/

{
    ULONG       PoolBlockSize;
    PPOOL_HEADER PoolHeader;
    ULONG       PoolTag;
    ULONG       Result;
    PVOID       PoolPage;
    PVOID       StartPage;
    PUCHAR      Pool;
    POOL_HACKER PoolBlock;
    ULONG       Previous;
    ULONG       SkipSize;
    PUCHAR      DataPage;
    PUCHAR      DataStart;
    LOGICAL     Found;
    ULONG       i;
    ULONG       j;
    ULONG       ct;
    ULONG       PoolBigPageTableSize;
    DWORD       PoolTableAddress;
    UCHAR       FastTag[4];
    ULONG       TagLength;
    PPOOL_TRACKER_BIG_PAGES BigTable;
    PMMPFN      PfnDatabaseLocal = NULL;
    ULONG       NumberOfPages;
    ULONG       z;

    DataPage = malloc( PageSize );

    if (DataPage == NULL) {
        dprintf( "Out of memory\n" );
        return;
    }

    if (!CopyPfnDatabaseLocal(&PfnDatabaseLocal, &NumberOfPages)) {
        free(DataPage);
        return;
    }

    __try {

        if (!PoolBigTableAddress) {
            PoolBigTableAddress = (PPOOL_TRACKER_BIG_PAGES)GetUlongValue ("xboxkrnl!PoolBigPageTable");
        }

        PoolTableAddress = (DWORD)PoolBigTableAddress;

        if (PoolTableAddress) {

            dprintf("\nScanning large pool allocation table for Tag: %c%c%c%c\n\n",
                                                TagName,
                                                TagName >> 8,
                                                TagName >> 16,
                                                TagName >> 24);

            PoolBigPageTableSize = GetUlongValue ("xboxkrnl!PoolBigPageTableSize");

            //
            // Scan the table looking for a match.
            //

            i = 0;
            ct = PageSize / sizeof (POOL_TRACKER_BIG_PAGES);

            BigTable = (PPOOL_TRACKER_BIG_PAGES)DataPage;

            while (i < PoolBigPageTableSize) {

                if (PoolBigPageTableSize - i < ct) {
                    ct = PoolBigPageTableSize - i;
                }

                if ( !ReadMemory( PoolTableAddress,
                                  DataPage,
                                  ct * sizeof (POOL_TRACKER_BIG_PAGES),
                                  &Result) ) {
                    dprintf("%08lx: Unable to get contents of pool block\n", PoolTableAddress );
                    __leave;
                }

                for (j = 0; j < ct; j += 1) {

                    Filter( (PCHAR)&BigTable[j].Key,
                            (PCHAR)&TagName,
                            0,
                            NULL,
                            BigTable[j].NumberOfPages * PageSize,
                            BigTable[j].Va,
                            Context );
                }
                i += ct;
                PoolTableAddress += (ct * sizeof (POOL_TRACKER_BIG_PAGES));
            }
        }
        else {
            dprintf("unable to get large pool allocation table - either wrong symbols or pool tagging is disabled\n");
        }

        dprintf("\nSearching pool for Tag: %c%c%c%c\n\n",
                                                TagName,
                                                TagName >> 8,
                                                TagName >> 16,
                                                TagName >> 24);

        for (z = 0; z < NumberOfPages; z++) {

            if (PfnDatabaseLocal[z].Busy.Busy == 0) {
                continue;
            }

            if (PfnDatabaseLocal[z].Busy.BusyType != MmPoolUsage) {
                continue;
            }

            PoolPage = GetPoolVirtualAddress(z, PfnDatabaseLocal[z].Busy.PteIndex);

            if (PoolPage == NULL) {
                dprintf("Failed to find virtual address for pool page %08x\n", z);
                continue;
            }

            Pool        = (PUCHAR)PAGE_ALIGN (PoolPage);
            StartPage   = (PVOID)Pool;
            Previous    = 0;

            while ((PVOID)PAGE_ALIGN(Pool) == StartPage) {
                if ( !ReadMemory( (DWORD)Pool,
                                  &PoolBlock,
                                  sizeof(POOL_HACKER),
                                  &Result) ) {
                    goto nextpage;
                }

                if ((PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT) > PAGE_SIZE) {
                    //dprintf("Bad allocation size @%lx, too large\n", Pool);
                    break;
                }

                if (PoolBlock.Header.BlockSize == 0) {
                    //dprintf("Bad allocation size @%lx, zero is invalid\n", Pool);
                    break;
                }

                if (PoolBlock.Header.PreviousSize != Previous) {
                    //dprintf("Bad previous allocation size @%lx, last size was %lx\n",Pool, Previous);
                    break;
                }

                PoolTag = PoolBlock.Header.PoolTag;

                Filter((PCHAR)&PoolTag,
                       (PCHAR)&TagName,
                       0,
                       &PoolBlock.Header,
                       PoolBlock.Header.BlockSize << POOL_BLOCK_SHIFT,
                       Pool + sizeof( POOL_HEADER ),
                       Context );

                Previous = PoolBlock.Header.BlockSize;
                Pool += (Previous << POOL_BLOCK_SHIFT);
                if ( CheckControlC() ) {
                    dprintf("\n...terminating - searched pool to %lx\n",
                            PoolPage);
                    __leave;
                }
            }
            PoolPage = (PVOID)((PCHAR)PoolPage + PageSize);
nextpage:
            if ( CheckControlC() ) {
                dprintf("\n...terminating - searched pool to %lx\n",
                        PoolPage);
                __leave;
            }
        }


    }  __finally {
    }

    LocalFree((HLOCAL)PfnDatabaseLocal);
    free (DataPage);
    return;
}



DECLARE_API( poolfind )

/*++

Routine Description:

    Find all allocations with the specified tag in the system pool.

Arguments:

    Tag - Supplies the tag to be found.

    Flags - Supplies 0 if a nonpaged pool search is desired.
            Supplies 1 if a paged pool search is desired.
            Supplies 2 if a special pool search is desired.

Return Value:

    None.

--*/

{
    CHAR        TagNameX[4] = {' ',' ',' ',' '};
    ULONG       TagName;

    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    sscanf (args,"%c%c%c%c", &TagNameX[0],
                             &TagNameX[1],
                             &TagNameX[2],
                             &TagNameX[3]);

    if (TagNameX[0] == '0' && TagNameX[1] == 'x') {
        sscanf (args, "%lx %lx", &TagName);
    } else {
        TagName = TagNameX[0] | (TagNameX[1] << 8) | (TagNameX[2] << 16) | (TagNameX[3] << 24);
    }

    SearchPool (TagName, 0, CheckSingleFilterAndPrint, NULL);
}


BOOLEAN
CheckSingleFilter (
    PCHAR Tag,
    PCHAR Filter
    )

/*++

Routine Description:

    Check the tag for a filter match.

Arguments:

    Tag - Supplies the tag to be examined.

    Filter - Supplies the filter to use for matching purposes.

Return Value:

    TRUE if the Tag matches, FALSE if not.

--*/

{
    ULONG i;
    UCHAR tc;
    UCHAR fc;

    for (i = 0; i < 4; i += 1) {

        tc = (UCHAR) *Tag;

        Tag += 1;

        fc = (UCHAR) *Filter;

        Filter += 1;

        if (fc == '*') {
            break;
        }

        if (fc == '?') {
            continue;
        }

        if (tc != fc) {
            return FALSE;
        }
    }
    return TRUE;
}
