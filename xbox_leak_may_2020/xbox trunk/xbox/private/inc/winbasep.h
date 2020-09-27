
/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1985-2001, Microsoft Corporation

Module Name:

    winbasep.h

Abstract:

    Private
    Procedure declarations, constant definitions and macros for the Base
    component.

--*/
#ifndef _WINBASEP_
#define _WINBASEP_
#ifdef __cplusplus
extern "C" {
#endif
WINBASEAPI
DWORD
WINAPI
HeapCreateTagsW(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPCWSTR lpTagPrefix,
    IN LPCWSTR lpTagNames
    );

typedef struct _HEAP_TAG_INFO {
    DWORD dwNumberOfAllocations;
    DWORD dwNumberOfFrees;
    DWORD dwBytesAllocated;
} HEAP_TAG_INFO, *PHEAP_TAG_INFO;
typedef PHEAP_TAG_INFO LPHEAP_TAG_INFO;

WINBASEAPI
LPCWSTR
WINAPI
HeapQueryTagW(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN WORD wTagIndex,
    IN BOOL bResetCounters,
    OUT LPHEAP_TAG_INFO TagInfo
    );

typedef struct _HEAP_SUMMARY {
    DWORD cb;
    SIZE_T cbAllocated;
    SIZE_T cbCommitted;
    SIZE_T cbReserved;
    SIZE_T cbMaxReserve;
} HEAP_SUMMARY, *PHEAP_SUMMARY;
typedef PHEAP_SUMMARY LPHEAP_SUMMARY;

BOOL
WINAPI
HeapSummary(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    OUT LPHEAP_SUMMARY lpSummary
    );

BOOL
WINAPI
HeapExtend(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPVOID lpBase,
    IN DWORD dwBytes
    );

typedef struct _HEAP_USAGE_ENTRY {
    struct _HEAP_USAGE_ENTRY *lpNext;
    PVOID lpAddress;
    DWORD dwBytes;
    DWORD dwReserved;
} HEAP_USAGE_ENTRY, *PHEAP_USAGE_ENTRY;

typedef struct _HEAP_USAGE {
    DWORD cb;
    SIZE_T cbAllocated;
    SIZE_T cbCommitted;
    SIZE_T cbReserved;
    SIZE_T cbMaxReserve;
    PHEAP_USAGE_ENTRY lpEntries;
    PHEAP_USAGE_ENTRY lpAddedEntries;
    PHEAP_USAGE_ENTRY lpRemovedEntries;
    DWORD Reserved[ 8 ];
} HEAP_USAGE, *PHEAP_USAGE;

BOOL
WINAPI
HeapUsage(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN BOOL bFirstCall,
    IN BOOL bLastCall,
    OUT PHEAP_USAGE lpUsage
    );




//
// filefind stucture shared with ntvdm, jonle
// see mvdm\dos\dem\demsrch.c
//
typedef struct _FINDFILE_HANDLE {
    HANDLE DirectoryHandle;
    PVOID FindBufferBase;
    PVOID FindBufferNext;
    ULONG FindBufferLength;
    ULONG FindBufferValidLength;
    RTL_CRITICAL_SECTION FindBufferLock;
} FINDFILE_HANDLE, *PFINDFILE_HANDLE;

#define BASE_FIND_FIRST_DEVICE_HANDLE (HANDLE)1


/* Max number of characters. Doesn't include termination character */
#define WSPRINTF_LIMIT 1024
#ifdef __cplusplus
}
#endif
#endif  // ndef _WINBASEP_
