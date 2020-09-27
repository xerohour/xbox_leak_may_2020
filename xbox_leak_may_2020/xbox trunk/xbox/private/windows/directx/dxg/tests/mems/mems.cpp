//-----------------------------------------------------------------------------
// FILE: FILLRATE.CPP
//
// Desc: a stupid fillrate test
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <mmintrin.h>

#include "mems.h"

void *g_pvCachedMem;
void *g_pvWCMem;

//=========================================================================
// Formatted debug squirty
//=========================================================================
void _cdecl dprintf(LPCSTR lpFmt, ...)
{
    va_list arglist;
    char lpOutput[256];

    va_start(arglist, lpFmt);
    _vsnprintf(lpOutput, sizeof(lpOutput), lpFmt, arglist);
    va_end(arglist);

    OutputDebugStringA(lpOutput);
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

void SpewResults(DWORD chunksize, DWORD passes, float time)
{
    dprintf("  %.2f MBytes/sec \t %ld KBytes * %ld / %.2f ms\n",
        chunksize * passes / (time * 1000 / 1024),
        chunksize, passes, time);
}

void
MovntqTest(void *pvDst)
{
    dprintf("movntq\n");

    CTimer timer;
    DWORD passes = 0;

    timer.Start();

    for(int i = 0; i < 10000; i++)
    {
        DWORD totbytes = 100 * 1024;

        while(totbytes)
        {
            _asm
            {
                mov ecx, 64
                mov edi, pvDst

            loop1:
                movntq [edi], mm0
                add edi, 8
                dec ecx
                jnz loop1;
            }

            totbytes -= 512;
        }

        passes++;

    }

    timer.Stop();

    _asm emms;

    SpewResults(100, passes, timer.getTime());
}

void
RepStoSdTest(void *pvDst)
{
    dprintf("rep stosd\n");

    CTimer timer;
    DWORD passes = 0;

    timer.Start();

    for(int i = 0; i < 10000; i++)
    {
        DWORD totbytes = 100 * 1024;

        while(totbytes)
        {
            _asm
            {
                mov edi, pvDst
                mov ecx, 128
                rep stosd
            }

            totbytes -= 512;
        }

        passes++;

    }

    timer.Stop();

    SpewResults(100, passes, timer.getTime());
}

void
memcpyTest(void *pvDst, void *pvSrc, DWORD chunksize)
{
    dprintf("memcpy chunksize: %d\n", chunksize);

    DWORD rgSizes[] = { 1, 100, 1000 };

    for(int iSize = 0; iSize < ARRAYSIZE(rgSizes); iSize++)
    {
        CTimer timer;
        DWORD passes = 0;

        timer.Start();

        for(int i = 0; i < 10000; i++)
        {
            DWORD totbytes = rgSizes[iSize] * 1024;
            BYTE *Dst = (BYTE *)pvDst;
            BYTE *Src = (BYTE *)pvSrc;

            while(totbytes)
            {
                DWORD size = min(totbytes, chunksize);

                memcpy(Dst, Src, size);
                totbytes -= size;
                Src += size;
                Dst += size;
            }

            passes++;
        }

        timer.Stop();

        SpewResults(rgSizes[iSize], passes, timer.getTime());

        // Break if current iteration was more than 5 seconds.
        if (timer.getTime() > 5000)
            break;
    }
}

void
RepMovSdTest(void *pvDst, void *pvSrc, DWORD chunksize)
{
    dprintf("rep movsd chunksize: %d\n", chunksize);

    DWORD rgSizes[] = { 1, 100, 1000 };

    for(int iSize = 0; iSize < ARRAYSIZE(rgSizes); iSize++)
    {
        CTimer timer;
        DWORD passes = 0;

        timer.Start();

        for(int i = 0; i < 10000; i++)
        {
            DWORD totbytes = rgSizes[iSize] * 1024;
            DWORD *pdwDst = (DWORD *)pvDst;
            DWORD *pdwSrc = (DWORD *)pvSrc;

            while(totbytes)
            {
                DWORD numdwords = min(totbytes, chunksize) / sizeof(DWORD);

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

            passes++;
        }

        timer.Stop();

        SpewResults(rgSizes[iSize], passes, timer.getTime());

        // Break if current iteration was more than 5 seconds.
        if (timer.getTime() > 5000)
            break;
    }

}

void
memcpy2(void *pvDst, void *pvSrc, DWORD chunksize)
{
    dprintf("memcpy2 chunksize: %d\n", chunksize);

    DWORD rgSizes[] = { 1, 100, 1000 };

    for(int iSize = 0; iSize < ARRAYSIZE(rgSizes); iSize++)
    {
        CTimer timer;
        DWORD passes = 0;

        timer.Start();

        for(int i = 0; i < 10000; i++)
        {
            DWORD totbytes = rgSizes[iSize] * 1024;
            BYTE *pdwDst = (BYTE *)pvDst;
            BYTE *pdwSrc = (BYTE *)pvSrc;

            while(totbytes)
            {
                // we're doing 256 byte blocks
                DWORD numblocks = min(totbytes, chunksize) / 256;

                for(DWORD j = 0; j < numblocks; j++)
                {
                    _asm
                    {
                        // loop through 4 times copying quadwords
                        mov ecx, 4
                        mov edi, pvDst
                        mov esi, pvSrc

                    loop1:
                        movq mm0, [esi+ 00]
                        movq mm1, [esi+ 32]
                        movq mm2, [esi+ 64]
                        movq mm3, [esi+ 96]
                        movq mm4, [esi+128]
                        movq mm5, [esi+160]
                        movq mm6, [esi+192]
                        movq mm7, [esi+224]

                        movq [edi+ 00], mm0
                        movq [edi+ 32], mm1
                        movq [edi+ 64], mm2
                        movq [edi+ 96], mm3
                        movq [edi+128], mm4
                        movq [edi+160], mm5
                        movq [edi+192], mm6
                        movq [edi+224], mm7

                        // move to the next quadword
                        add edi, 8
                        add esi, 8

                        dec ecx
                        jnz loop1;
                    }

                    pdwDst += 256;
                    pdwSrc += 256;
                }

                totbytes -= numblocks * 256;
            }

            passes++;
        }

        timer.Stop();

        _asm emms;

        SpewResults(rgSizes[iSize], passes, timer.getTime());

        // Break if current iteration was more than 5 seconds.
        if (timer.getTime() > 5000)
            break;
    }

}

//=========================================================================
// main
//=========================================================================
void __cdecl main()
{
    float cpuspeed = GetCpuSpeed();

    dprintf("\ncpuspeed: %f\n\n", cpuspeed);

    for(int times = 0; times < 1; times++)
    {
        Sleep(1000);

        g_pvCachedMem = malloc(1000 * 1024);

        g_pvWCMem = XPhysicalAlloc(1000 * 1024, MAXULONG_PTR, PAGE_SIZE,
                                   PAGE_READWRITE | PAGE_WRITECOMBINE);

        for(int type = 0; type < 3; type++)
        {
            void* pvWCMem;

            switch (type)
            {
                case 0:
                    pvWCMem = g_pvWCMem;
                    dprintf("--> Physical address range 0 - 64 MB:\n");
                    break;

                case 1:
                    pvWCMem = (void*) ((DWORD) g_pvWCMem | 0xA0000000);
                    dprintf("--> 1 GB Alias:\n");
                    break;

                case 2:
                    pvWCMem = (void*) ((DWORD) g_pvWCMem | 0xf0000000);
                    dprintf("--> Memory buffer aperture:\n");
                    break;

            }

            dprintf("pvWCMem: %lx\n", pvWCMem);

            memcpyTest(pvWCMem, g_pvCachedMem, 512);

            memcpyTest(pvWCMem, g_pvCachedMem, 0xffffffff);

            RepMovSdTest(pvWCMem, g_pvCachedMem, 512);

            dprintf("read from wc ");
            RepMovSdTest(g_pvCachedMem, pvWCMem, 0xffffffff);

            dprintf("read from wc ");
            memcpy2(g_pvCachedMem, pvWCMem, 0xffffffff);

            RepStoSdTest(pvWCMem);

            MovntqTest(pvWCMem);

            dprintf("\n");
        }

        XPhysicalFree(g_pvWCMem);
        free(g_pvCachedMem);
    }

    dprintf("Waiting forever now.\n");
    for(;;) ;
}

