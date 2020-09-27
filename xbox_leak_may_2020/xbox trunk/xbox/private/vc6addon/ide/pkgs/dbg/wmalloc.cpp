/*** wmalloc.c - Memory management entry points not implemented by WINDOWS 386
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose: To supply the C runtime memory management entry points,
*      which are defective in the WINDOWS 386 runtime library.
*
*   Revision History:
*     24-Apr-1989 ArthurC Created
*   [1]     09-Jul-1989 ArthurC Assume near pointer is local handle
*   [2]     21-Jul-1989 ArthurC Fixed definition of _cvw3_hmalloc
*   [3]     22-Aug-1989 ArthurC Fixed order of if evaluation to prevent GP
*   [4]     02-Oct-1989 ArthurC Removed unused local variable
*	[5]		03-May-1994 DanS	Rewrote to use Win32 heaps
*
*************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
	// For the release build, we *definately* want to use the old 
	// blow-away-the-heap-each-session method

// Uncomment this line to use the CRT heap.
// #define USE_REGULAR_HEAP_TO_FIND_LEAKS 1
#endif

#ifdef HOST32
#ifndef _HUGE_
        #define _HUGE_
#endif
#else	// !HOST32
    #define _HUGE_	_huge
#endif	// HOST32

extern "C" {	// rest of file

#if defined( USE_REGULAR_HEAP_TO_FIND_LEAKS ) 
	// We map these calls back onto the CRT memory management
	// code to help us track leaks.

void cvw3_minit (void) {}
void cvw3_mcleanup(void) {}
void *	CDECL cvw3_fmalloc (size_t cb) { return _fmalloc (cb); }
void *	CDECL cvw3_frealloc (void * pv, size_t cb) { return _frealloc (pv, cb); }
void	CDECL cvw3_ffree (void * pv) { _ffree (pv); }

#else

// handle to our debugger heap
static HANDLE	hDbgHeap = 0;

// self-tuning initial size reservation for HeapCreate call
const DWORD		cbInitialHeapMin = (512 * 1024) ;		// minimum heap size is 512K.
DWORD			cbInitialHeap = cbInitialHeapMin;	// give ourselves cbInitialHeapMin reserved
DWORD			cbAlloc = 0;						// current allocation size of heap

#if defined (_DEBUG) && !(defined (DBG_MEM_STATS))
// turn on debugger memory allocator stats for all debug builds
# define DBG_MEM_STATS	1
#endif

#if defined (DBG_MEM_STATS)
struct MEMSTATS {
	DWORD	cbAlloc;
	DWORD	cAllocs;
	DWORD	cbFree;
	DWORD	cFrees;
	DWORD	cReallocs;
	DWORD	cbHeap;
	DWORD	cbMinAlloc;
	DWORD	cbMaxAlloc;
	};

MEMSTATS	ms = {0};

#endif

__inline size_t CbAlign ( size_t cb ) {
	return (cb + 7) & ~7;
	}
	
BOOL cvw3_minit(void)
{
	if ( hDbgHeap == 0 )
	{
		hDbgHeap = HeapCreate(0, cbInitialHeap, 0);
		if (hDbgHeap==NULL)
			return FALSE;
		// reset our allocation counter so that we can self-tune
		//	the initial reserved memory of the heap to be roughly
		//	the size last time.
		cbInitialHeap = cbInitialHeapMin;
		cbAlloc = 0;
#if defined (DBG_MEM_STATS)
		ms.cbAlloc = ms.cAllocs = ms.cbFree = ms.cFrees = ms.cReallocs = ms.cbHeap = 0;
		ms.cbMaxAlloc = 0;
		ms.cbMinAlloc = 0xffffffff;
#endif
	}
	return TRUE;
}

void cvw3_mcleanup(void)
{
	if ( hDbgHeap != 0 )
	{
		VERIFY(HeapDestroy(hDbgHeap));
	}
	hDbgHeap = 0;
}

void *	CDECL cvw3_fmalloc(size_t cb)
{
#if defined (DBG_MEM_STATS)
	ms.cbAlloc += cb;
	ms.cbHeap += cb;
	ms.cAllocs++;
	if ( cb < ms.cbMinAlloc ) ms.cbMinAlloc = cb;
	if ( cb > ms.cbMaxAlloc ) ms.cbMaxAlloc = cb;
#endif
	cbAlloc += CbAlign ( cb );
	if ( cbAlloc > cbInitialHeap )
	{
		cbInitialHeap = cbAlloc;
	}	
	return HeapAlloc(hDbgHeap, HEAP_ZERO_MEMORY, cb);
}

size_t	CDECL cvw3_fmsize(void * pv)
{
	return HeapSize(hDbgHeap, 0, pv);
}

void *	CDECL cvw3_frealloc(void * pv, size_t cb)
{
	if (pv == NULL)
	{
		return cvw3_fmalloc(cb);
	}
#if defined (DBG_MEM_STATS)
	ms.cReallocs++;
	ms.cbHeap -= cvw3_fmsize(pv);
	ms.cbAlloc += cb;
	ms.cbHeap += cb;
#endif

	// Adjust and verify cbAlloc
	DWORD cbChunk = CbAlign(cvw3_fmsize(pv));
	if (cbAlloc >= cbChunk)
	{
		cbAlloc -= cbChunk;
	}
	else
	{
		// It is possible we're freeing more memory than we thought we ever
		// allocated, because HeapAlloc can alloc more than the requested amount.
		// Set cbAlloc to 0 so we don't call HeapCreate with some HUGE number
		// like 0xFFFFFFB8.
		cbAlloc = 0;
	}
	cbAlloc += CbAlign ( cb );
	return HeapReAlloc(hDbgHeap, 0, pv, cb);
}

void	CDECL cvw3_ffree(void * pv)
{
	// Chicago's HeapFree doesn't allow you to free NULL pointers
	if (pv == NULL)	return;

#if defined (DBG_MEM_STATS)
	ms.cbFree += cvw3_fmsize(pv);
	ms.cFrees++;
	ms.cbHeap -= cvw3_fmsize(pv);
#endif

	// Adjust and verify cbAlloc
	DWORD cbChunk = CbAlign(cvw3_fmsize(pv));
	if (cbAlloc >= cbChunk)
	{
		cbAlloc -= cbChunk;
	}
	else
	{
		// It is possible we're freeing more memory than we thought we ever
		// allocated, because HeapAlloc can alloc more than the requested amount.
		// Set cbAlloc to 0 so we don't call HeapCreate with some HUGE number
		// like 0xFFFFFFB8.
		cbAlloc = 0;
	}
	VERIFY(HeapFree(hDbgHeap, 0, pv));
}

#endif // USE_REGULAR_HEAP_TO_FIND_LEAKS

} // extern "C"

