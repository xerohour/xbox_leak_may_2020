/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    memtests.cpp

Abstract:

    Memory throughput tests

--*/

#include "precomp.h"

#define KB          1024
#define MB          (1024*1024)
#define PAGESIZE    (4*KB)
#define MAXBUFSIZE  (4096*PAGESIZE)
#define MEMTESTSIZE (16*MAXBUFSIZE)

//
// Memory read/write access functions using SSE instructions
//  We assume all buffers are 256-byte aligned and
//  buffer size is also a multiple of 256.
//
#ifndef USE_SSE
#define ZeroMem(buf, bufsize) memset(buf, 0, bufsize)
#define CopyMem memcpy
#else // USE_SSE
VOID ZeroMem(VOID* buf, ULONG bufsize)
{
    __asm {
        xorps   xmm0, xmm0
        mov     ecx, buf
        mov     edx, bufsize
        shr     edx, 7
    L1: movdqa  [ecx+0*16], xmm0
        movdqa  [ecx+1*16], xmm0
        movdqa  [ecx+2*16], xmm0
        movdqa  [ecx+3*16], xmm0
        movdqa  [ecx+4*16], xmm0
        movdqa  [ecx+5*16], xmm0
        movdqa  [ecx+6*16], xmm0
        movdqa  [ecx+7*16], xmm0
        add     ecx, 8*16
        dec     edx
        jnz     L1
    }
}

VOID CopyMem(VOID* dstbuf, const VOID* srcbuf, ULONG bufsize)
{
    __asm {
        mov     ecx, srcbuf
        mov     ebx, dstbuf
        mov     edx, bufsize
        shr     edx, 7
    L1: movdqa  xmm0, [ecx+0*16]
        movdqa  xmm1, [ecx+1*16]
        movdqa  xmm2, [ecx+2*16]
        movdqa  xmm3, [ecx+3*16]
        movdqa  xmm4, [ecx+4*16]
        movdqa  xmm5, [ecx+5*16]
        movdqa  xmm6, [ecx+6*16]
        movdqa  xmm7, [ecx+7*16]
        movdqa  [ebx+0*16], xmm0
        movdqa  [ebx+1*16], xmm1
        movdqa  [ebx+2*16], xmm2
        movdqa  [ebx+3*16], xmm3
        movdqa  [ebx+4*16], xmm4
        movdqa  [ebx+5*16], xmm5
        movdqa  [ebx+6*16], xmm6
        movdqa  [ebx+7*16], xmm7
        add     ecx, 8*16
        add     ebx, 8*16
        dec     edx
        jnz     L1
    }
}
#endif // USE_SSE

VOID ReadMem(const VOID* buf, ULONG bufsize)
{
    __asm {
        mov     ecx, buf
        mov     edx, bufsize
        shr     edx, 7
    L1:
        movdqa  xmm0, [ecx+0*16]
        movdqa  xmm1, [ecx+1*16]
        movdqa  xmm2, [ecx+2*16]
        movdqa  xmm3, [ecx+3*16]
        movdqa  xmm4, [ecx+4*16]
        movdqa  xmm5, [ecx+5*16]
        movdqa  xmm6, [ecx+6*16]
        movdqa  xmm7, [ecx+7*16]
        add     ecx, 8*16
        dec     edx
        jnz     L1
    }
}

inline VOID LogMemoryThroughput(CHAR* testname, ULONG time, ULONG size=MEMTESTSIZE/MB) {
    ULONG result = (size * 1000000) / time;
    LogEntry(testname, result, "MB/s");
}

DECLARE_TEST_FUNCTION(MemoryTests)
{
    DWORD protect = 0;
    DWORD ormask = 0;
    if (testparam) {
        if (strcmp(testparam, "WC") == 0) {
            // write-combined memory
            protect = PAGE_WRITECOMBINE;
        } else if (strcmp(testparam, "NC") == 0) {
            // uncached memory
            protect = PAGE_NOCACHE;
        } else if (strcmp(testparam, "AGP") == 0) {
            // write-combined memory accessed through AGP aperture
            protect = PAGE_WRITECOMBINE;
            ormask = 0xf0000000;
        }
    }

    #ifdef USE_SSE
    DbgPrint("MemoryTests: use SSE functions...\n");
    #endif

    Srand('MEMX');
    VOID* membuf = (BYTE*) XPhysicalAlloc(MAXBUFSIZE, MAXULONG_PTR, 0, PAGE_READWRITE|protect);
    ASSERT(membuf != NULL);

    BYTE* buf = (BYTE*) ((DWORD) membuf | ormask);

    // Read 4KB memory buffer
    ULONG time = ReadTimestamp();
    INT count = MEMTESTSIZE / PAGESIZE;
    while (count--) {
        ReadMem(buf, PAGESIZE);
    }
    LogMemoryThroughput("MemRead4K", ReadTimestamp() - time);

    // Read 16MB memory buffer
    time = ReadTimestamp();
    count = MEMTESTSIZE / MAXBUFSIZE;
    while (count--) {
        ReadMem(buf, MAXBUFSIZE);
    }
    LogMemoryThroughput("MemRead16M", ReadTimestamp() - time);

    // Fill 4KB memory buffer
    time = ReadTimestamp();
    count = MEMTESTSIZE / PAGESIZE;
    while (count--) {
        ZeroMem(buf, PAGESIZE);
    }
    LogMemoryThroughput("MemFill4K", ReadTimestamp() - time);

    // Fill 16MB memory buffer
    time = ReadTimestamp();
    count = MEMTESTSIZE / MAXBUFSIZE;
    while (count--) {
        ZeroMem(buf, MAXBUFSIZE);
    }
    LogMemoryThroughput("MemFill16M", ReadTimestamp() - time);

    // Copy 4KB memory buffer
    time = ReadTimestamp();
    count = MEMTESTSIZE / PAGESIZE;
    while (count--) {
        CopyMem(buf, buf + PAGESIZE, PAGESIZE);
    }
    LogMemoryThroughput("MemCopy4K", ReadTimestamp() - time);

    // Copy 8MB memory buffer
    #define COPYMEMSIZE (MAXBUFSIZE/2)
    time = ReadTimestamp();
    count = MEMTESTSIZE / COPYMEMSIZE;
    while (count--) {
        CopyMem(buf, buf+COPYMEMSIZE, COPYMEMSIZE);
    }
    LogMemoryThroughput("MemCopy8M", ReadTimestamp() - time);

    // Random reads
    #define RANDRWCOUNT (MAXBUFSIZE/sizeof(ULONG))
    ULONG x, *p;
    ULONG* end = (ULONG*) (buf + MAXBUFSIZE);
    time = ReadTimestamp();
    count = 0;
    do {
        x = Rand();
        p = (ULONG*) buf + (x & (MAXBUFSIZE/sizeof(ULONG)-1));
        x &= 31;
        if (p+x > end) x = end-p;
        count += x;

        while (x--) {
            *((volatile ULONG*) p);
            p++;
        }
    } while (count < RANDRWCOUNT);
    LogMemoryThroughput("MemReadRandom", ReadTimestamp() - time, count*sizeof(ULONG)/MB);

    // Random writes
    time = ReadTimestamp();
    count = 0;
    do {
        x = Rand();
        p = (ULONG*) buf + (x & (MAXBUFSIZE/sizeof(ULONG)-1));
        x &= 31;
        if (p+x > end) x = end-p;
        count += x;
        while (x--) *p++ = 0;
    } while (count < RANDRWCOUNT);
    LogMemoryThroughput("MemWriteRandom", ReadTimestamp() - time, count*sizeof(ULONG)/MB);

    XPhysicalFree(membuf);
    return 0;
}

