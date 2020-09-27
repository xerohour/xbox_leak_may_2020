//-----------------------------------------------------------------------------
// FILE: MEMCPY2.CPP
//
// Desc: stupid bandwidth tests
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <mmintrin.h>

#include "memcpy2.h"

// Size of block we're copying
DWORD g_blocksize = 256 * 1000;

// Chunk size of bytes to move during each loop
const DWORD g_movchunksize = 512;

// Whatever PIII counters you think might help you
#define XTAG(_x) { _x, #_x }
struct
{
    DWORD Ctr;
    const char *CtrName;
} rgCtrs[] =
{
    XTAG(PERF_BUS_COMPLETED_PARTIAL_WRITES),
    XTAG(PERF_DCU_CYCLES_MISS_OUTSTANDING),
};

// Silly macros to help start/end/time each routine

#define START_TIME_BLOCK()                                              \
    int oldprio = GetThreadPriority(GetCurrentThread());                \
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);     \
    Sleep(10);                                                          \
    CProfiler profiler(rgCtrs[0].Ctr, rgCtrs[1].Ctr);                   \
    CTimer timer;                                                       \
    DWORD passes = 1000;                                                \
    profiler.Start();                                                   \
    timer.Start();

#define END_TIME_BLOCK(_x)                                              \
    timer.Stop();                                                       \
    profiler.Stop();                                                    \
    SetThreadPriority(GetCurrentThread(), oldprio);                     \
    SpewResults(#_x, g_blocksize, passes, timer.getTime(), &profiler);

//=========================================================================
// Spew the results of whatever test we just ran
//=========================================================================
void SpewResults(char *test, DWORD blocksize, DWORD passes,
    float time, CProfiler *pprofiler = NULL)
{
    __int64 ctr0 = pprofiler ? pprofiler->getCtr0() : 0;
    __int64 ctr1 = pprofiler ? pprofiler->getCtr1() : 0;

    float time2 = time / 1000;
    dprintf("  %-12s: %7.2f MBytes/sec  %7.2f ms  ctr0:%7I64u ctr1:%7I64u\n",
        test, (blocksize * passes) / (time2 * 1024 * 1024), time, ctr0, ctr1);
}

//=========================================================================
// Standard memcpy test
//=========================================================================
void
memcpyTest(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    for(DWORD i = 0; i < passes; i++)
    {
        DWORD totbytes = g_blocksize;
        BYTE *Dst = (BYTE *)pvDst;
        BYTE *Src = (BYTE *)pvSrc;

        while(totbytes)
        {
            DWORD size = min(totbytes, g_movchunksize);

            memcpy(Dst, Src, size);
            totbytes -= size;
            Src += size;
            Dst += size;
        }
    }

    END_TIME_BLOCK(memcpy);
}

//=========================================================================
// rep movsd test
//=========================================================================
void
RepMovSdTest(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    for(DWORD i = 0; i < passes; i++)
    {
        DWORD totbytes = g_blocksize;
        DWORD *pdwDst = (DWORD *)pvDst;
        DWORD *pdwSrc = (DWORD *)pvSrc;

        while(totbytes)
        {
            DWORD numdwords = min(totbytes, g_movchunksize) / sizeof(DWORD);

            _asm
            {
                mov esi, pdwSrc
                mov edi, pdwDst
                mov ecx, numdwords
                rep movsd
            }

            pdwSrc += numdwords;
            pdwDst += numdwords;

            totbytes -= numdwords * sizeof(DWORD);
        }
    }

    END_TIME_BLOCK(repmovsd);
}

//=========================================================================
// movupsmemcpy
//=========================================================================
void
movupsmemcpy(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    for(DWORD i = 0; i < passes; i++)
    {
        DWORD totbytes = g_blocksize;
        BYTE *pdwDst = (BYTE *)pvDst;
        BYTE *pdwSrc = (BYTE *)pvSrc;

        while(totbytes)
        {
            DWORD numbytes = min(totbytes, g_movchunksize);

            _asm
            {
                // loop through 4 times copying quadwords
                mov ecx, numbytes
                shr ecx, 6
                mov edi, pdwDst
                mov esi, pdwSrc

                align 16

            loop1:
                prefetchnta [esi+64]
                prefetchnta [esi+96]

                movups xmm0, [esi]
                movups xmm1, [esi+16]
                movups xmm2, [esi+32]
                movups xmm3, [esi+48]

                movntps [edi], xmm0
                movntps [edi+16], xmm1
                movntps [edi+32], xmm2
                movntps [edi+48], xmm3

                // move to the next quadword
                add edi, 64
                add esi, 64

                dec ecx
                jnz loop1;
            }

            pdwDst += numbytes;
            pdwSrc += numbytes;

            totbytes -= numbytes;
        }
    }

    END_TIME_BLOCK(movupsmemcpy);
}


//=========================================================================
// movapsmemcpy
//=========================================================================
void
movapsmemcpy(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    for(DWORD i = 0; i < passes; i++)
    {
        DWORD totbytes = g_blocksize;
        BYTE *pdwDst = (BYTE *)pvDst;
        BYTE *pdwSrc = (BYTE *)pvSrc;

        while(totbytes)
        {
            DWORD numbytes = min(totbytes, g_movchunksize);

            _asm
            {
                // loop through 4 times copying quadwords
                mov ecx, numbytes
                shr ecx, 6
                mov edi, pdwDst
                mov esi, pdwSrc

                align 16

            loop1:
                prefetchnta [esi+64]
                prefetchnta [esi+96]

                movaps xmm0, [esi]
                movaps xmm1, [esi+16]
                movaps xmm2, [esi+32]
                movaps xmm3, [esi+48]

                movntps [edi], xmm0
                movntps [edi+16], xmm1
                movntps [edi+32], xmm2
                movntps [edi+48], xmm3

                // move to the next quadword
                add edi, 64
                add esi, 64

                dec ecx
                jnz loop1;
            }

            pdwDst += numbytes;
            pdwSrc += numbytes;

            totbytes -= numbytes;
        }
    }

    END_TIME_BLOCK(movapsmemcpy);
}

//=========================================================================
// movqmemcpy
//=========================================================================
void
movqmemcpy(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    for(DWORD i = 0; i < passes; i++)
    {
        DWORD totbytes = g_blocksize;
        BYTE *pdwDst = (BYTE *)pvDst;
        BYTE *pdwSrc = (BYTE *)pvSrc;

        while(totbytes)
        {
            DWORD numbytes = min(totbytes, g_movchunksize);

            _asm
            {
                // loop through 4 times copying quadwords
                mov ecx, numbytes
                shr ecx, 6
                mov edi, pdwDst
                mov esi, pdwSrc

                align 16

            loop1:
                prefetchnta [esi+64]
                prefetchnta [esi+96]

                movq mm0, [esi+0]
                movq mm1, [esi+8]
                movq mm2, [esi+16]
                movq mm3, [esi+24]
                movq mm4, [esi+32]
                movq mm5, [esi+40]
                movq mm6, [esi+48]
                movq mm7, [esi+56]

                movntq [edi+0], mm0
                movntq [edi+8], mm1
                movntq [edi+16], mm2
                movntq [edi+24], mm3
                movntq [edi+32], mm4
                movntq [edi+40], mm5
                movntq [edi+48], mm6
                movntq [edi+56], mm7

                // move to the next quadword
                add edi, 64
                add esi, 64

                nop
                nop
                nop

                dec ecx
                jnz loop1;
            }

            pdwDst += numbytes;
            pdwSrc += numbytes;

            totbytes -= numbytes;
        }
    }

    _asm emms

    END_TIME_BLOCK(movqmemcpy);
}

//=========================================================================
// movqmemcpy
//=========================================================================
void
movqmemcpynoprefetch(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    for(DWORD i = 0; i < passes; i++)
    {
        DWORD totbytes = g_blocksize;
        BYTE *pdwDst = (BYTE *)pvDst;
        BYTE *pdwSrc = (BYTE *)pvSrc;

        while(totbytes)
        {
            DWORD numbytes = min(totbytes, g_movchunksize);

            _asm
            {
                // loop through 4 times copying quadwords
                mov ecx, numbytes
                shr ecx, 6
                mov edi, pdwDst
                mov esi, pdwSrc

                align 16

            loop1:
                movq mm0, [esi+0]
                movq mm1, [esi+8]
                movq mm2, [esi+16]
                movq mm3, [esi+24]
                movq mm4, [esi+32]
                movq mm5, [esi+40]
                movq mm6, [esi+48]
                movq mm7, [esi+56]

                movntq [edi+0], mm0
                movntq [edi+8], mm1
                movntq [edi+16], mm2
                movntq [edi+24], mm3
                movntq [edi+32], mm4
                movntq [edi+40], mm5
                movntq [edi+48], mm6
                movntq [edi+56], mm7

                // move to the next quadword
                add edi, 64
                add esi, 64

                dec ecx
                jnz loop1;
            }

            pdwDst += numbytes;
            pdwSrc += numbytes;

            totbytes -= numbytes;
        }
    }

    _asm emms

    END_TIME_BLOCK(movqnopref)
}

//=========================================================================
// Get the cpu speed.
//=========================================================================
float GetCpuSpeed(void)
{
    __int64 start, end, freq;
    int     clocks;

    int oldprio = GetThreadPriority(GetCurrentThread());

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    Sleep(10);

    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&start);

    _asm
    {
        rdtsc
        mov     ecx, 900000
x:      dec     ecx
        jnz     x
        mov     ebx,eax
        rdtsc
        sub     eax,ebx
        mov     dword ptr clocks[0],eax
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&end);

    SetThreadPriority(GetCurrentThread(), oldprio);

    float time = (float)MulDiv((int)(end - start), 1000000, (int)freq);

    return clocks / time;
}


//=========================================================================
// Big move loop
//=========================================================================
void MovqCopy(void *pvDst, void *pvSrc)
{
    START_TIME_BLOCK();

    __asm
    {
        mov ebx, passes

test_loop:
        mov esi,[pvSrc]
        mov edi,[pvDst]

        ;add esi,4

        mov eax,g_blocksize
        shr eax,6

        ALIGN 16

copy_loop:
        prefetchnta [esi+64]
        prefetchnta [esi+96]

        movq mm0,[esi]
        movq mm1,[esi+8]
        movq mm2,[esi+16]
        movq mm3,[esi+24]

        movq mm4,[esi+32]
        movq mm5,[esi+40]
        movq mm6,[esi+48]
        movq mm7,[esi+56]

        movntq [edi],mm0
        movntq [edi+8],mm1
        movntq [edi+16],mm2
        movntq [edi+24],mm3

        movntq [edi+32],mm4
        movntq [edi+40],mm5
        movntq [edi+48],mm6
        movntq [edi+56],mm7

        add esi,64
        add edi,64

        dec eax
        jnz copy_loop

        dec ebx
        jnz test_loop

        emms
    }

    END_TIME_BLOCK(movqcopy);
}

//=========================================================================
//
//=========================================================================
void SpewMemoryFlags(DWORD dwFlags)
{
    #undef XTAG
    #define XTAG(_tag) { _tag, #_tag }
    static const struct
    {
        DWORD flag;
        const char *szDescr;
    } rgsz[] =
    {
        XTAG(PAGE_NOACCESS),            XTAG(PAGE_READONLY),
        XTAG(PAGE_READWRITE),           XTAG(PAGE_WRITECOPY),
        XTAG(PAGE_EXECUTE),             XTAG(PAGE_EXECUTE_READ),
        XTAG(PAGE_EXECUTE_READWRITE),   XTAG(PAGE_EXECUTE_WRITECOPY),
        XTAG(PAGE_GUARD),               XTAG(PAGE_NOCACHE),
        XTAG(PAGE_WRITECOMBINE),
    };
    static const int cFmts = sizeof(rgsz) / sizeof(rgsz[0]);

    for(int ifmt = 0; ifmt < cFmts; ifmt++)
    {
        if(rgsz[ifmt].flag & dwFlags)
            dprintf(" %s", rgsz[ifmt].szDescr);
    }
}

//=========================================================================
//
//=========================================================================
#define GETFIELD(value, upper, lower) \
    ((value >> lower) & ((2L << (upper-lower))-1))

//=========================================================================
// Dump our memory dest and src values
//=========================================================================
void SpewMemLocs(void *pvDst, void *pvSrc)
{
    DWORD dwAlign = (DWORD)pvSrc & 0x1f;

    static const char *rgHdrs[] =
        { "Source", "Dest" };
    void *rgMems[] = { pvSrc, pvDst };

    dprintf("\n");
    for(int i = 0; i < 2; i++)
    {
        DWORD dwAddr = (DWORD)rgMems[i];

        dprintf("%-14s: 0x%08lx (+%2d)  bank:%d  (",
            rgHdrs[i], dwAddr,  dwAddr & 0x1f, GETFIELD(dwAddr, 13, 12));

        SpewMemoryFlags(XQueryMemoryProtect(rgMems[i]));

        dprintf(" )\n");
    }
}

//=========================================================================
// main
//=========================================================================
void __cdecl main()
{
    // Calc cpu speed

    float cpuspeed = GetCpuSpeed();

    dprintf("\ncpuspeed: %f\n\n", cpuspeed);

    for(int ctr = 0; ctr < 2; ctr++)
    {
        dprintf("ctr%d: %s\n", ctr, rgCtrs[ctr].CtrName);
    }


    // Alloc memory

    Sleep(1000);

    byte *pvCachedMem = (byte *)XPhysicalAlloc(g_blocksize * 2,
        MAXULONG_PTR, PAGE_SIZE, PAGE_READWRITE);

    byte *pvWCMem = (byte *)XPhysicalAlloc(g_blocksize * 2,
        MAXULONG_PTR, PAGE_SIZE, PAGE_READWRITE | PAGE_WRITECOMBINE);

#ifdef NEVER
    // The following tests a routine which doesn't use the chunk
    // copy mechanism - it just blasts the entire block over.
    // Currently #ifdef'd NEVER as the times are virtually identical
    // to the below routines and I got sick of the extra noise.

    void *pvCachedMem2 = (byte *)malloc(g_blocksize * 2);

    // large block tests - first with XPhysicalAlloc block

    SpewMemLocs(pvWCMem, pvCachedMem);

    for(int i = 0; i < 4; i++)
        MovqCopy(pvWCMem, pvCachedMem);

    // second with malloc'd block

    SpewMemLocs(pvWCMem, pvCachedMem2);

    for(int i = 0; i < 4; i++)
        MovqCopy(pvWCMem, pvCachedMem2);
#endif

    // run standard memcpy vs. movq tests
    for(int align_dst = 0; align_dst <= 16; align_dst += 16)
    {
        for(int align_src = 1; align_src < 32; align_src <<= 1)
        {
            void *pvSrc = pvCachedMem + ((align_src == 1) ? 0 : align_src);
            void *pvDst = pvWCMem + align_dst;

            SpewMemLocs(pvDst, pvSrc);

            memcpyTest(pvDst, pvSrc);

            movqmemcpy(pvDst, pvSrc);

            movqmemcpynoprefetch(pvDst, pvSrc);

            // dest must be 16 byte aligned for movntps
            if(!((DWORD)pvDst & 0xf))
            {
                movupsmemcpy(pvDst, pvSrc);

                // src must be 16 byte aligned for movaps
                if(!((DWORD)pvSrc & 0xf))
                    movapsmemcpy(pvDst, pvSrc);
            }
        }
    }

    dprintf("\n\nWaiting forever now.\n");
    for(;;) ;
}

