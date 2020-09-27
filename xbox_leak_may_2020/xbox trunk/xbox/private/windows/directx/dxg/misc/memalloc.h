/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memalloc.h
 *  Content:	header file for memory allocation
 *  History:
 *
 ***************************************************************************/
#ifndef __MEMALLOC_INCLUDED__
#define __MEMALLOC_INCLUDED__

// Note that RtlAllocateHeap(XapiProcessHeap()), which is what LocalAlloc
// amounts to on Xbox, has 8 byte alignment:

#define MemAlloc(size) ((VOID*) LocalAlloc(LMEM_ZEROINIT, size))
#define MemAllocNoZero(size) ((VOID*) LocalAlloc(0, size))
#define MemReAlloc(p, size) ((VOID*) LocalReAlloc(p, size, LMEM_ZEROINIT))
#define MemFree(p) LocalFree((p))

#endif
