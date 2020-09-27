/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xmemp.h

Abstract:

  This module contains the private definitions for xmem.c

Author:

  Steven Kehrli (steveke) 1-Nov-2001

------------------------------------------------------------------------------*/

#pragma once



namespace xMemNamespace {

// Memory allocation structure

typedef struct _XMEM_ALLOCATION {
    LPVOID                   lpMem;                // Pointer to the memory allocation
    INT64                    *lpMemTail;           // Pointer to the memory allocation tail
    SIZE_T                   dwBytes;              // Specifies the size of the allocation
    CHAR                     szFile[MAX_PATH];     // Specifies the file name of the allocation
    DWORD                    dwLine;               // Specifies the line number of the allocation
    DWORD                    dwThreadId;           // Specifies the thread id of the allocation
    SYSTEMTIME               LocalTime;            // Specifies the time of the allocation
    struct _XMEM_ALLOCATION  *pPrevMemAllocation;  // Pointer to the previous memory allocation in the list
    struct _XMEM_ALLOCATION  *pNextMemAllocation;  // Pointer to the next memory allocation in the list
} XMEM_ALLOCATION, *PXMEM_ALLOCATION;

#define XMEM_ALLOCATION_TAIL  0x083E546BF7C1AB94

typedef struct _XMEM_OBJECT {
    HANDLE                   hHeap;                // Handle to the private heap
    PCRITICAL_SECTION        pcsMem;               // Pointer to the critical section to synchronize access to this object
    PXMEM_ALLOCATION         pMemAllocation;       // Pointer to the list of memory allocations
    CHAR                     szFile[MAX_PATH];     // Specifies the file name of the object
    DWORD                    dwLine;               // Specifies the line number of the object
    DWORD                    dwThreadId;           // Specifies the thread if of the object
    SYSTEMTIME               LocalTime;            // Specifies the time of the object
    struct _XMEM_OBJECT      *pPrevMemObject;      // Pointer to the previous object in the list
    struct _XMEM_OBJECT      *pNextMemObject;      // Pointer to the next object in the list
} XMEM_OBJECT, *PXMEM_OBJECT;

} // namespace xMemNamespace
