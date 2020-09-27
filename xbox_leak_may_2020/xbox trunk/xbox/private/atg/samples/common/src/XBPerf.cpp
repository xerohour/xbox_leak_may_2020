//-----------------------------------------------------------------------------
// File: XBPerf.cpp
//
// Desc: Sample to show off tri-stripping performance results
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: XBPerf_CalcCacheHits()
// Desc: Calculate the number of cache hits and degenerate triangles
//-----------------------------------------------------------------------------
HRESULT XBPerf_CalcCacheHits( D3DPRIMITIVETYPE dwPrimType, DWORD dwVertexSize,
						      WORD* pIndices, DWORD dwNumIndices,
						      DWORD* pdwNumDegenerateTris,
						      DWORD* pdwNumCacheHits,
 						      DWORD* pdwNumPagesCrossed )
{
	// Check arguments
	if( NULL == pdwNumDegenerateTris || NULL == pdwNumCacheHits || 
		NULL == pdwNumPagesCrossed )
		return E_INVALIDARG;

	// Initialize results
    (*pdwNumDegenerateTris) = 0;
    (*pdwNumCacheHits)      = 0;
    (*pdwNumPagesCrossed)   = 1;

	// Simulate a vertex cache
    static const int CACHE_SIZE = 12;
	static const int PAGE_SIZE  = 4096;
    DWORD rgdwCache[CACHE_SIZE];
    INT   iCachePtr      = 0;
    BOOL  bIsTriStrip    = (dwPrimType == D3DPT_TRIANGLESTRIP);
    DWORD dwLastPageAddr = 0;
    memset( rgdwCache, 0xff, sizeof(rgdwCache) );

	// Run all vertices through the sumilated vertex cache, tallying cache hits,
	// degenerate triangles, and pages crossed.
    for( DWORD i = 0; i < dwNumIndices; i++ )
    {
        // This makes all kinds of assumptions such as page size is 4k,
        // page across then back is ok, etc etc. Seems to be an ok
        // estimate on data locality though.
        DWORD dwPage = dwVertexSize * pIndices[i] / PAGE_SIZE;

        if( ( dwPage > dwLastPageAddr ) || ( dwPage+1 < dwLastPageAddr ) )
        {
            (*pdwNumPagesCrossed)++;
            dwLastPageAddr = dwVertexSize * pIndices[i] / PAGE_SIZE;
        }

        // Update our count of degenerate tris
        if( bIsTriStrip && (i > 1) )
			if( ( pIndices[i-0] == pIndices[i-1] ) ||
			    ( pIndices[i-0] == pIndices[i-2] ) ||
			    ( pIndices[i-1] == pIndices[i-2] ) )
				(*pdwNumDegenerateTris)++;

		// Check to see if the vertex would be in the cache
		BOOL bVertexInCache = FALSE;
        for( int cache_index = 0; cache_index < CACHE_SIZE; cache_index++ )
        {
            if( pIndices[i] == rgdwCache[cache_index] )
            {
				bVertexInCache = TRUE;
                break;
            }
        }

        if( bVertexInCache )
        {
			// Keep track of cache hits
            (*pdwNumCacheHits)++;
		}
        else 
        {
            // Add vertex to simulated cache
            rgdwCache[iCachePtr] = pIndices[i];
            iCachePtr = (iCachePtr + 1) % CACHE_SIZE;
        }
    }

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBPerf_GetCpuSpeed()
// Desc: Returns a calculation of the CPU speed.
//-----------------------------------------------------------------------------
#pragma optimize("g", on)
double XBPerf_GetCpuSpeed()
{
    __int64 start, end, freq;
    int     clocks;
	HANDLE  hThread;
    int     iOldThreadPriority;

    hThread = GetCurrentThread();

    // save the current so it can be set back later
    iOldThreadPriority = GetThreadPriority(hThread);

    // boost this thread to do the calculations, making sure no other
    // thread interrupts the calculations
    SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);

    Sleep(10);

    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&start);
    _asm
    {
        rdtsc
        mov     ecx, 100000
x:      dec     ecx
        jnz     x
        mov     ebx,eax
        rdtsc
        sub     eax,ebx
        mov     dword ptr clocks[0],eax
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&end);

    // restore the thread priority
    SetThreadPriority(hThread, iOldThreadPriority);
    CloseHandle(hThread);

    int time = MulDiv((int)(end - start), 1000000, (int)freq);

    return ((clocks + time / 2) / time);
}
#pragma optimize("g", off)
