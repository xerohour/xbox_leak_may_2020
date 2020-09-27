/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1989-1999 Microsoft Corporation

Module Name:

    ntmmapi.h

Abstract:

    This is the include file for the Memory Management sub-component of NTOS

Author:

    Lou Perazzoli (loup) 10-May-1989

Revision History:

--*/

#ifndef _NTMMAPI_
#define _NTMMAPI_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Memory information structures.
//
// begin_winnt begin_ntddk
typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    ULONG AllocationProtect;
    SIZE_T RegionSize;
    ULONG State;
    ULONG Protect;
    ULONG Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
// end_winnt end_ntddk

// begin_ntddk begin_wdm
typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

//
// Section Access Rights.
//

// begin_winnt
#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)
// end_winnt

#define SEGMENT_ALL_ACCESS SECTION_ALL_ACCESS

#define PAGE_NOACCESS          0x01     // winnt
#define PAGE_READONLY          0x02     // winnt
#define PAGE_READWRITE         0x04     // winnt
#define PAGE_WRITECOPY         0x08     // winnt
#define PAGE_EXECUTE           0x10     // winnt
#define PAGE_EXECUTE_READ      0x20     // winnt
#define PAGE_EXECUTE_READWRITE 0x40     // winnt
#define PAGE_EXECUTE_WRITECOPY 0x80     // winnt
#define PAGE_GUARD            0x100     // winnt
#define PAGE_NOCACHE          0x200     // winnt
#define PAGE_WRITECOMBINE     0x400     // winnt
#define PAGE_VIDEO              0x0     // winnt
#define PAGE_OLD_VIDEO        0x800

// end_ntddk end_wdm

#define MEM_COMMIT           0x1000     // winnt ntddk wdm
#define MEM_RESERVE          0x2000     // winnt ntddk wdm
#define MEM_DECOMMIT         0x4000     // winnt ntddk wdm
#define MEM_RELEASE          0x8000     // winnt ntddk wdm
#define MEM_FREE            0x10000     // winnt ntddk wdm
#define MEM_PRIVATE         0x20000     // winnt ntddk wdm
#define MEM_MAPPED          0x40000     // ntddk wdm
#define MEM_RESET           0x80000     // winnt ntddk wdm
#define MEM_TOP_DOWN       0x100000     // winnt ntddk wdm
#define MEM_WRITE_WATCH    0x200000
#define MEM_PHYSICAL       0x400000
#define MEM_NOZERO         0x800000     // winnt ntddk wdm
#define MEM_LARGE_PAGES  0x20000000     // ntddk wdm
#define MEM_DOS_LIM      0x40000000
#define MEM_4MB_PAGES    0x80000000     // winnt ntddk wdm

#define SEC_BASED          0x200000
#define SEC_NO_CHANGE      0x400000
#define SEC_FILE           0x800000
#define SEC_IMAGE         0x1000000
#define SEC_RESERVE       0x4000000     // ntddk wdm
#define SEC_COMMIT        0x8000000
#define SEC_NOCACHE      0x10000000
#define SEC_GLOBAL       0x20000000

#define MEM_IMAGE         SEC_IMAGE

#define WRITE_WATCH_FLAG_RESET 0x01

#define MAP_PROCESS 1L
#define MAP_SYSTEM  2L

// begin_ntsrv

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSection (
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN PLARGE_INTEGER MaximumSize OPTIONAL,
    IN ULONG SectionPageProtection,
    IN ULONG AllocationAttributes,
    IN HANDLE FileHandle OPTIONAL
    );

// end_ntsrv

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );


NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN SIZE_T CommitSize,
    IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
    IN OUT PSIZE_T ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    );

// begin_ntsrv

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN OUT PSIZE_T RegionSize,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtFreeVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG FreeType
    );

// end_ntsrv

NTSYSCALLAPI
NTSTATUS
NTAPI
NtProtectVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG NewProtect,
    OUT PULONG OldProtect
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryVirtualMemory(
    IN PVOID BaseAddress,
    OUT PMEMORY_BASIC_INFORMATION MemoryInformation
    );

#ifdef __cplusplus
}
#endif

#endif  // _NTMMAPI_
