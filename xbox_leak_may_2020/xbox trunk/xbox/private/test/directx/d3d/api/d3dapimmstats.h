#ifndef _d3dapiMMstats_h_
#define _d3dapiMMstats_h_

/*
You'd think that we'd be able to have headers arranged in such a way that we 
could just #include "mm.h" and "ps.h", or "nt.h" or something similar, along 
with "xtl.h", in order to get at some of the more useful kernel features and 
everything would be happy & compile correctly. You'd think, anyway...
*/

typedef struct _MM_STATISTICS {
    ULONG Length;
    ULONG TotalPhysicalPages;
    ULONG AvailablePages;
    ULONG VirtualMemoryBytesCommitted;
    ULONG VirtualMemoryBytesReserved;
    ULONG CachePagesCommitted;
    ULONG PoolPagesCommitted;
    ULONG StackPagesCommitted;
    ULONG ImagePagesCommitted;
} MM_STATISTICS, *PMM_STATISTICS;

#define NTKERNELAPI DECLSPEC_IMPORT         // wdm
typedef LONG NTSTATUS;

EXTERN_C
NTKERNELAPI
NTSTATUS
MmQueryStatistics(
    IN OUT PMM_STATISTICS MemoryStatistics
    );

typedef struct _PS_STATISTICS {
    ULONG Length;
    ULONG ThreadCount;
    ULONG HandleCount;
} PS_STATISTICS, *PPS_STATISTICS;

EXTERN_C
NTKERNELAPI
NTSTATUS
PsQueryStatistics(
    IN OUT PPS_STATISTICS ProcessStatistics
    );

#endif