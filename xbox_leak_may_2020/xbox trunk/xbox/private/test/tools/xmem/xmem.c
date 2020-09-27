/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xmem.c

Abstract:

  This module contains the memory functions

Author:

  Steven Kehrli (steveke) 1-Nov-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace xMemNamespace;

namespace xMemNamespace {

PXMEM_OBJECT  g_pMemObject = NULL;       // g_pMemObjectList is a pointer to the list of memory objects
HANDLE        g_hMemObjectMutex = NULL;  // g_hMemObjectMutex is the object to synchronize access to the list of memory objects



PXMEM_ALLOCATION
xSetMemAllocation(
    IN  LPVOID  lpBuffer,
    IN  SIZE_T  dwBytes,
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sets the XMEM_ALLOCATION structure for a block of memory

Arguments:

  lpBuffer - pointer to the allocated memory block
  dwBytes - number of bytes allocated for the user
  lpszFile - pointer to a NULL-terminated string (ANSI) that specifies the file name of the allocation
  dwLine - specifies the line of the allocation

Return Value:

  PXMEM_ALLOCATION:
    Pointer to the XMEM_ALLOCATION structure

------------------------------------------------------------------------------*/
{
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    // Set pMemAllocation
    pMemAllocation = (PXMEM_ALLOCATION) lpBuffer;

    // Set the pMemAllocation members
    pMemAllocation->lpMem = (LPVOID) ((UINT_PTR) lpBuffer + sizeof(XMEM_ALLOCATION));
    pMemAllocation->lpMemTail = (INT64 *) ((UINT_PTR) pMemAllocation->lpMem + dwBytes);
    *pMemAllocation->lpMemTail = XMEM_ALLOCATION_TAIL;
    pMemAllocation->dwBytes = dwBytes;
    strncpy(pMemAllocation->szFile, lpszFile, sizeof(pMemAllocation->szFile) - sizeof(CHAR));
    pMemAllocation->dwLine = dwLine;
    pMemAllocation->dwThreadId = GetCurrentThreadId();
    GetLocalTime(&pMemAllocation->LocalTime);

    return pMemAllocation;
}



VOID
xAddMemAllocationToList(
    IN  PXMEM_OBJECT      pMemObject,
    IN  PXMEM_ALLOCATION  pMemAllocation
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Adds a memory allocation to the list of memory allocations

Arguments:

  pMemObject - pointer to the memory object
  pMemAllocation - pointer to the memory allocation

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Wait for access to the list of memory allocations
    EnterCriticalSection(pMemObject->pcsMem);

    // Add pMemAllocation to the list
    pMemAllocation->pNextMemAllocation = pMemObject->pMemAllocation;
    if (NULL != pMemObject->pMemAllocation) {
        pMemObject->pMemAllocation->pPrevMemAllocation = pMemAllocation;
    }
    pMemObject->pMemAllocation = pMemAllocation;

    // Release access to the list of memory allocations
    LeaveCriticalSection(pMemObject->pcsMem);
}



PXMEM_ALLOCATION
xRemoveMemAllocationFromList(
    IN  PXMEM_OBJECT  pMemObject,
    IN  LPVOID        lpMem
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Adds a memory allocation to the list of memory allocations

Arguments:

  pMemObject - pointer to the memory object
  lpMem - pointer to the memory block for the user

Return Value:

  PXMEM_ALLOCATION:
    Pointer to the XMEM_ALLOCATION structure

------------------------------------------------------------------------------*/
{
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    // Wait for access to the list of memory allocations
    EnterCriticalSection(pMemObject->pcsMem);

    // Set pMemAllocation to the head of the list of memory allocations
    pMemAllocation = pMemObject->pMemAllocation;

    // Search the list of memory allocations for the current buffer
    while ((NULL != pMemAllocation) && ((PXMEM_ALLOCATION) ((UINT_PTR) lpMem - sizeof(XMEM_ALLOCATION)) != pMemAllocation)) {
        pMemAllocation = pMemAllocation->pNextMemAllocation;
    }

    // Remove pMemAllocation from the list
    if ((NULL == pMemAllocation->pNextMemAllocation) && (NULL == pMemAllocation->pPrevMemAllocation)) {
        // Only allocation in the list
        pMemObject->pMemAllocation = NULL;
    }
    else if (NULL == pMemAllocation->pNextMemAllocation) {
        // Allocation is tail of the list
        pMemAllocation->pPrevMemAllocation->pNextMemAllocation = NULL;
    }
    else if (NULL == pMemAllocation->pPrevMemAllocation) {
        // Allocation is head of the list
        pMemAllocation->pNextMemAllocation->pPrevMemAllocation = NULL;
        pMemObject->pMemAllocation = pMemAllocation->pNextMemAllocation;
    }
    else {
        // Allocation is middle of the list
        pMemAllocation->pPrevMemAllocation->pNextMemAllocation = pMemAllocation->pNextMemAllocation;
        pMemAllocation->pNextMemAllocation->pPrevMemAllocation = pMemAllocation->pPrevMemAllocation;
    }

    // Release access to the list of memory allocations
    LeaveCriticalSection(pMemObject->pcsMem);

#ifdef _DEBUG
    if (XMEM_ALLOCATION_TAIL != *pMemAllocation->lpMemTail) {
        DbgPrint("Memory Allocation Tail has been changed.  Probable overflow in block 0x%08x.\r\n", pMemAllocation);
        DbgPrint("      lpMem:                   0x%08x\n", pMemAllocation->lpMem);
        DbgPrint("      lpMemTail:               0x%08x\n", pMemAllocation->lpMemTail);
        DbgPrint("      dwBytes:                 %u\n", pMemAllocation->dwBytes);
        DbgPrint("      szFile:                  %s\n", pMemAllocation->szFile);
        DbgPrint("      dwLine:                  %u\n", pMemAllocation->dwLine);
        DbgPrint("      dwThreadId:              0x%08x\n", pMemAllocation->dwThreadId);
        DbgPrint("      LocalTime:               %02d/%02d/%04d %02d:%02d:%02d\n", pMemAllocation->LocalTime.wMonth, pMemAllocation->LocalTime.wDay, pMemAllocation->LocalTime.wYear, pMemAllocation->LocalTime.wHour, pMemAllocation->LocalTime.wMinute, pMemAllocation->LocalTime.wSecond);
        DebugBreak();
    }
#endif

    return pMemAllocation;
}

} // namespace xMemNamespace



LPVOID
WINAPI
xMemAllocLocal(
    IN  HANDLE  hMemObject,
    IN  SIZE_T  dwBytes,
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Allocates a block of memory from the private heap.

Arguments:

  hMemObject - handle to the memory object
  dwBytes - number of bytes to allocate for the user
  lpszFile - pointer to a NULL-terminated string (ANSI) that specifies the file name of the allocation
  dwLine - specifies the line of the allocation

Return Value:

  LPVOID:
    If the function succeeds, the return value is a pointer to the memory block for the user
    If the function fails, the return value is NULL

------------------------------------------------------------------------------*/
{
    // pMemObject is a pointer to the memory object
    PXMEM_OBJECT      pMemObject = (PXMEM_OBJECT) hMemObject;
    // dwBufferSize is the size of the memory allocation buffer, in bytes
    DWORD             dwBufferSize = dwBytes + sizeof(XMEM_ALLOCATION) + sizeof(XMEM_ALLOCATION_TAIL);
    // lpBuffer is a pointer to the memory allocation buffer
    LPVOID            lpBuffer = NULL;
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    if ((INVALID_HANDLE_VALUE == hMemObject) || (NULL == hMemObject)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return NULL;
    }

    // Allocate the memory allocation buffer
    lpBuffer = HeapAlloc(pMemObject->hHeap, HEAP_ZERO_MEMORY, dwBufferSize);

    if (NULL == lpBuffer) {
        return NULL;
    }

    // Set the XMEM_ALLOCATION structure
    pMemAllocation = xSetMemAllocation(lpBuffer, dwBytes, lpszFile, dwLine);

    // Add the memory allocation to the list
    xAddMemAllocationToList(pMemObject, pMemAllocation);

    return pMemAllocation->lpMem;
}



LPVOID
WINAPI
xMemReAllocLocal(
    IN  HANDLE  hMemObject,
    IN  LPVOID  lpMem,
    IN  SIZE_T  dwBytes,
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Reallocates a block of memory from the private heap.

Arguments:

  hMemObject - handle to the memory object
  lpMem - pointer to the memory block for the user
  dwBytes - number of bytes to allocate for the user
  lpszFile - pointer to a NULL-terminated string (ANSI) that specifies the file name of the allocation
  dwLine - specifies the line of the allocation

Return Value:

  LPVOID:
    If the function succeeds, the return value is a pointer to the memory block for the user
    If the function fails, the return value is NULL

------------------------------------------------------------------------------*/
{
    // pMemObject is a pointer to the memory object
    PXMEM_OBJECT      pMemObject = (PXMEM_OBJECT) hMemObject;
    // dwBufferSize is the size of the memory allocation buffer, in bytes
    DWORD             dwBufferSize = dwBytes + sizeof(XMEM_ALLOCATION) + sizeof(XMEM_ALLOCATION_TAIL);
    // lpBuffer is a pointer to the memory allocation buffer
    LPVOID            lpBuffer = NULL;
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    if ((INVALID_HANDLE_VALUE == hMemObject) || (NULL == hMemObject)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return NULL;
    }

    // Remove the memory allocation from the list
    pMemAllocation = xRemoveMemAllocationFromList(pMemObject, lpMem);

    if (NULL == pMemAllocation) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return NULL;
    }

    // Reallocate memory
    lpBuffer = HeapReAlloc(pMemObject->hHeap, 0, pMemAllocation, dwBufferSize);

    if (NULL == lpBuffer) {
        return NULL;
    }

    // Zero the memory
    ZeroMemory(lpBuffer, dwBufferSize);

    // Set the XMEM_ALLOCATION structure
    pMemAllocation = xSetMemAllocation(lpBuffer, dwBytes, lpszFile, dwLine);

    // Add the memory allocation to the list
    xAddMemAllocationToList(pMemObject, pMemAllocation);

    return pMemAllocation->lpMem;
}



BOOL
WINAPI
xMemFree(
    IN  HANDLE  hMemObject,
    IN  LPVOID  lpMem
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Frees a block of memory allocated from the private heap.

Arguments:

  lpMem - pointer to the memory block for the user

Return Value:

  BOOL:
    If the function succeeds, the return value is a nonzero
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pMemObject is a pointer to the memory object
    PXMEM_OBJECT      pMemObject = (PXMEM_OBJECT) hMemObject;
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    if ((INVALID_HANDLE_VALUE == hMemObject) || (NULL == hMemObject)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Remove the memory allocation from the list
    pMemAllocation = xRemoveMemAllocationFromList(pMemObject, lpMem);

    if (NULL == pMemAllocation) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Free the memory allocation
    HeapFree(pMemObject->hHeap, 0, pMemAllocation);

    return TRUE;
}



HANDLE
WINAPI
xMemCreateLocal(
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates the memory object

Arguments:

  lpszFile - pointer to a NULL-terminated string (ANSI) that specifies the file name of the allocation
  dwLine - specifies the line of the allocation

Return Value:

  HANDLE:
    If the function succeeds, the return value is a handle to the XMEM_OBJECT.
    If the function fails, the return value is INVALID_HANDLE_VALUE.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // hHeap is a handle to the private heap
    HANDLE            hHeap = NULL;
    // pMemObject is a pointer to the memory object
    PXMEM_OBJECT      pMemObject = NULL;
    // dwBufferSize is the size of the memory allocation buffer, in bytes
    DWORD             dwBufferSize = sizeof(XMEM_OBJECT) + sizeof(CRITICAL_SECTION) + sizeof(XMEM_ALLOCATION) + sizeof(XMEM_ALLOCATION_TAIL);
    // lpBuffer is a pointer to the memory allocation buffer
    LPVOID            lpBuffer = NULL;
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    // Create the private heap
    hHeap = HeapCreate(0, 0, 0);

    if (NULL == hHeap) {
        return INVALID_HANDLE_VALUE;
    }

    // Allocate the memory allocation buffer
    lpBuffer = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwBufferSize);

    if (NULL == lpBuffer) {
        // Destroy the private heap
        HeapDestroy(hHeap);

        return INVALID_HANDLE_VALUE;
    }

    // Set the XMEM_ALLOCATION structure
    pMemAllocation = xSetMemAllocation(lpBuffer, sizeof(XMEM_OBJECT) + sizeof(CRITICAL_SECTION), __FILE__, __LINE__);

    // Get the memory object
    pMemObject = (PXMEM_OBJECT) pMemAllocation->lpMem;

    // Set the private heap
    pMemObject->hHeap = hHeap;

    // Set the critical section
    pMemObject->pcsMem = (CRITICAL_SECTION *) ((UINT_PTR) pMemObject + sizeof(XMEM_OBJECT));

    // Initialize the critical section
    InitializeCriticalSection(pMemObject->pcsMem);

    // Set the pMemAllocation members
    strncpy(pMemObject->szFile, lpszFile, sizeof(pMemObject->szFile) - sizeof(CHAR));
    pMemObject->dwLine = dwLine;
    pMemObject->dwThreadId = GetCurrentThreadId();
    GetLocalTime(&pMemObject->LocalTime);

    // Create the memory object mutex
    g_hMemObjectMutex = CreateMutex(NULL, FALSE, "xMemoryObjectMutex");

    // Wait for access to the list of memory objects
    WaitForSingleObject(g_hMemObjectMutex, INFINITE);

    // Add pMemObject to the list
    pMemObject->pNextMemObject = g_pMemObject;
    if (NULL != g_pMemObject) {
        g_pMemObject->pPrevMemObject = pMemObject;
    }
    g_pMemObject = pMemObject;

    // Release access to the list of memory objects
    ReleaseMutex(g_hMemObjectMutex);

    // Add the memory allocation to the list
    xAddMemAllocationToList(pMemObject, pMemAllocation);

    return (HANDLE) pMemObject;
}



BOOL
WINAPI
xMemClose(
    IN  HANDLE  hMemObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes the memory object

Arguments:

  hMemObject - handle to the memory object

Return Value:

  BOOL:
    If the function succeeds, the return value is a nonzero
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // hHeap is a handle to the private heap
    HANDLE            hHeap = NULL;
    // pMemObject is a pointer to the memory object
    PXMEM_OBJECT      pMemObject = NULL;
    // pMemAllocation is a pointer to the XMEM_ALLOCATION structure
    PXMEM_ALLOCATION  pMemAllocation = NULL;



    if ((INVALID_HANDLE_VALUE == hMemObject) || (NULL == hMemObject)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Wait for access to the list of memory objects
    WaitForSingleObject(g_hMemObjectMutex, INFINITE);

    // Set pMemObject to the head of the list of memory objects
    pMemObject = g_pMemObject;

    // Search the list of memory objects for the current object
    while ((NULL != pMemObject) && ((PXMEM_OBJECT) hMemObject != pMemObject)) {
        pMemObject = pMemObject->pNextMemObject;
    }

    if (NULL == pMemObject) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Remove pMemObject from the list
    if ((NULL == pMemObject->pNextMemObject) && (NULL == pMemObject->pPrevMemObject)) {
        // Only object in the list
        g_pMemObject = NULL;
    }
    else if (NULL == pMemObject->pNextMemObject) {
        // Object is tail of the list
        pMemObject->pPrevMemObject->pNextMemObject = NULL;
    }
    else if (NULL == pMemObject->pPrevMemObject) {
        // Object is head of the list
        pMemObject->pNextMemObject->pPrevMemObject = NULL;
        g_pMemObject = pMemObject->pNextMemObject;
    }
    else {
        // Object is middle of the list
        pMemObject->pPrevMemObject->pNextMemObject = pMemObject->pNextMemObject;
        pMemObject->pNextMemObject->pPrevMemObject = pMemObject->pPrevMemObject;
    }

    // Release access to the list of memory objects
    ReleaseMutex(g_hMemObjectMutex);

    // Remove the memory allocation from the list
    pMemAllocation = xRemoveMemAllocationFromList(pMemObject, pMemObject);

    // Get the private heap
    hHeap = pMemObject->hHeap;

#ifdef _DEBUG
    // Wait for access to the list of memory allocations
    EnterCriticalSection(pMemObject->pcsMem);

    if (NULL != pMemObject->pMemAllocation) {
        // Set pMemAllocation to the head of the list of memory allocations
        pMemAllocation = pMemObject->pMemAllocation;

        // Enumerate the remaining memory allocations
        while (NULL != pMemAllocation) {
            DbgPrint("Memory Allocation block 0x%08x.\r\n", pMemAllocation);
            DbgPrint("      lpMem:                   0x%08x\n", pMemAllocation->lpMem);
            DbgPrint("      lpMemTail:               0x%08x\n", pMemAllocation->lpMemTail);
            DbgPrint("      dwBytes:                 %u\n", pMemAllocation->dwBytes);
            DbgPrint("      szFile:                  %s\n", pMemAllocation->szFile);
            DbgPrint("      dwLine:                  %u\n", pMemAllocation->dwLine);
            DbgPrint("      dwThreadId:              0x%08x\n", pMemAllocation->dwThreadId);
            DbgPrint("      LocalTime:               %02d/%02d/%04d %02d:%02d:%02d\n", pMemAllocation->LocalTime.wMonth, pMemAllocation->LocalTime.wDay, pMemAllocation->LocalTime.wYear, pMemAllocation->LocalTime.wHour, pMemAllocation->LocalTime.wMinute, pMemAllocation->LocalTime.wSecond);
            DbgPrint("\n");

            pMemAllocation = pMemAllocation->pNextMemAllocation;
        }

        DebugBreak();

        // Free the remaining memory allocations
        while (NULL != pMemObject->pMemAllocation) {
            pMemAllocation = pMemObject->pMemAllocation->pNextMemAllocation;

            xMemFree(hHeap, pMemObject->pMemAllocation->lpMem);

            pMemObject->pMemAllocation = pMemAllocation;
        }
    }

    // Release access to the list of memory allocations
    LeaveCriticalSection(pMemObject->pcsMem);
#endif

    // Delete the critical section
    DeleteCriticalSection(pMemObject->pcsMem);

    // Free the memory allocation
    HeapFree(hHeap, 0, pMemAllocation);

    // Destroy the private heap
    HeapDestroy(hHeap);

    return TRUE;
}
