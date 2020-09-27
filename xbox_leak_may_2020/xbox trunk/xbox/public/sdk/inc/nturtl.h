/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1989-1999  Microsoft Corporation

Module Name:

    ntrtl.h

Abstract:

    Include file for NT runtime routines that are callable by only
    user mode code in various.

Author:

    Steve Wood (stevewo) 10-Aug-1989

Environment:

    These routines are statically linked in the caller's executable and
    are callable in only from user mode.  They make use of Nt system
    services.

Revision History:

--*/

#ifndef _NTURTL_
#define _NTURTL_

#if _MSC_VER > 1000
#pragma once
#endif

//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_XAPI_)
#define NTURTLAPI DECLSPEC_IMPORT
#else
#define NTURTLAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined (_MSC_VER)
#if ( _MSC_VER >= 800 )
#pragma warning(disable:4514)
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4001)
#pragma warning(disable:4201)
#pragma warning(disable:4214)
#endif
#if (_MSC_VER >= 1020)
#pragma once
#endif
#endif

//
// Current Directory Stuff
//

typedef struct _RTL_RELATIVE_NAME {
    STRING RelativeName;
    HANDLE ContainingDirectory;
} RTL_RELATIVE_NAME, *PRTL_RELATIVE_NAME;

typedef enum _RTL_PATH_TYPE {
    RtlPathTypeUnknown,
    RtlPathTypeUncAbsolute,
    RtlPathTypeDriveAbsolute,
    RtlPathTypeDriveRelative,
    RtlPathTypeRooted,
    RtlPathTypeRelative,
    RtlPathTypeLocalDevice,
    RtlPathTypeRootLocalDevice
} RTL_PATH_TYPE;

NTURTLAPI
RTL_PATH_TYPE
NTAPI
RtlDetermineDosPathNameType_U(
    PCWSTR DosFileName
    );

NTURTLAPI
ULONG
NTAPI
RtlIsDosDeviceName_U(
    PWSTR DosFileName
    );

NTURTLAPI
ULONG
NTAPI
RtlGetFullPathName_U(
    PCWSTR lpFileName,
    ULONG nBufferLength,
    PWSTR lpBuffer,
    PWSTR *lpFilePart
    );

NTURTLAPI
ULONG
NTAPI
RtlGetCurrentDirectory_U(
    ULONG nBufferLength,
    PWSTR lpBuffer
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlSetCurrentDirectory_U(
    PUNICODE_STRING PathName
    );

NTURTLAPI
ULONG
NTAPI
RtlGetLongestNtPathLength( VOID );

BOOLEAN
NTAPI
RtlDosPathNameToNtPathName_U(
    PCWSTR DosFileName,
    PUNICODE_STRING NtFileName,
    PWSTR *FilePart OPTIONAL,
    PRTL_RELATIVE_NAME RelativeName OPTIONAL
    );

NTURTLAPI
ULONG
NTAPI
RtlDosSearchPath_U(
    PWSTR lpPath,
    PWSTR lpFileName,
    PWSTR lpExtension,
    ULONG nBufferLength,
    PWSTR lpBuffer,
    PWSTR *lpFilePart
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlDoesFileExists_U(
    PCWSTR FileName
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlInitializeProfile (
    BOOLEAN KernelToo
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlStartProfile (
    VOID
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlStopProfile (
    VOID
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlAnalyzeProfile (
    VOID
    );

//
// User mode only security Rtl routines
//

//
// Structure to hold information about an ACE to be created
//
#ifdef _MAC
#pragma warning( disable : 4121)
#endif

typedef struct {
    UCHAR AceType;
    UCHAR InheritFlags;
    UCHAR AceFlags;
    ACCESS_MASK Mask;
    PSID *Sid;
} RTL_ACE_DATA, *PRTL_ACE_DATA;

#ifdef _MAC
#pragma warning( default : 4121 )
#endif

NTURTLAPI
NTSTATUS
NTAPI
RtlNewSecurityObject(
    PSECURITY_DESCRIPTOR ParentDescriptor,
    PSECURITY_DESCRIPTOR CreatorDescriptor,
    PSECURITY_DESCRIPTOR * NewDescriptor,
    BOOLEAN IsDirectoryObject,
    HANDLE Token,
    PGENERIC_MAPPING GenericMapping
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlNewSecurityObjectEx (
    IN PSECURITY_DESCRIPTOR ParentDescriptor OPTIONAL,
    IN PSECURITY_DESCRIPTOR CreatorDescriptor OPTIONAL,
    OUT PSECURITY_DESCRIPTOR * NewDescriptor,
    IN GUID *ObjectType OPTIONAL,
    IN BOOLEAN IsDirectoryObject,
    IN ULONG AutoInheritFlags,
    IN HANDLE Token,
    IN PGENERIC_MAPPING GenericMapping
    );

// Values for AutoInheritFlags
#define SEF_DACL_AUTO_INHERIT             0x01
#define SEF_SACL_AUTO_INHERIT             0x02
#define SEF_DEFAULT_DESCRIPTOR_FOR_OBJECT 0x04
#define SEF_AVOID_PRIVILEGE_CHECK         0x08
#define SEF_AVOID_OWNER_CHECK             0x10
#define SEF_DEFAULT_OWNER_FROM_PARENT     0x20
#define SEF_DEFAULT_GROUP_FROM_PARENT     0x40

NTSTATUS
RtlConvertToAutoInheritSecurityObject(
    IN PSECURITY_DESCRIPTOR ParentDescriptor OPTIONAL,
    IN PSECURITY_DESCRIPTOR CurrentSecurityDescriptor,
    OUT PSECURITY_DESCRIPTOR *NewSecurityDescriptor,
    IN GUID *ObjectType OPTIONAL,
    IN BOOLEAN IsDirectoryObject,
    IN PGENERIC_MAPPING GenericMapping
    );


NTURTLAPI
NTSTATUS
NTAPI
RtlSetSecurityObject (
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR ModificationDescriptor,
    PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    PGENERIC_MAPPING GenericMapping,
    HANDLE Token
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlSetSecurityObjectEx (
    IN SECURITY_INFORMATION SecurityInformation,
    IN PSECURITY_DESCRIPTOR ModificationDescriptor,
    IN OUT PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    IN ULONG AutoInheritFlags,
    IN PGENERIC_MAPPING GenericMapping,
    IN HANDLE Token OPTIONAL
    );


NTURTLAPI
NTSTATUS
NTAPI
RtlQuerySecurityObject (
     PSECURITY_DESCRIPTOR ObjectDescriptor,
     SECURITY_INFORMATION SecurityInformation,
     PSECURITY_DESCRIPTOR ResultantDescriptor,
     ULONG DescriptorLength,
     PULONG ReturnLength
     );

NTURTLAPI
NTSTATUS
NTAPI
RtlDeleteSecurityObject (
    PSECURITY_DESCRIPTOR * ObjectDescriptor
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlNewInstanceSecurityObject(
    BOOLEAN ParentDescriptorChanged,
    BOOLEAN CreatorDescriptorChanged,
    PLUID OldClientTokenModifiedId,
    PLUID NewClientTokenModifiedId,
    PSECURITY_DESCRIPTOR ParentDescriptor,
    PSECURITY_DESCRIPTOR CreatorDescriptor,
    PSECURITY_DESCRIPTOR * NewDescriptor,
    BOOLEAN IsDirectoryObject,
    HANDLE Token,
    PGENERIC_MAPPING GenericMapping
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlCopySecurityDescriptor(
    PSECURITY_DESCRIPTOR InputSecurityDescriptor,
    PSECURITY_DESCRIPTOR *OutputSecurityDescriptor
    );

//
// list canonicalization
//

NTURTLAPI
NTSTATUS
NTAPI
RtlConvertUiListToApiList(
    PUNICODE_STRING UiList OPTIONAL,
    PUNICODE_STRING ApiList,
    BOOLEAN BlankIsDelimiter
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlCreateAndSetSD(
    IN  PRTL_ACE_DATA AceData,
    IN  ULONG AceCount,
    IN  PSID OwnerSid OPTIONAL,
    IN  PSID GroupSid OPTIONAL,
    OUT PSECURITY_DESCRIPTOR *NewDescriptor
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlCreateUserSecurityObject(
    IN  PRTL_ACE_DATA AceData,
    IN  ULONG AceCount,
    IN  PSID OwnerSid,
    IN  PSID GroupSid,
    IN  BOOLEAN IsDirectoryObject,
    IN  PGENERIC_MAPPING GenericMapping,
    OUT PSECURITY_DESCRIPTOR *NewDescriptor
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlDefaultNpAcl(
    OUT PACL * pAcl
    );

//
// Per-Thread Curdir Support
//

typedef struct _RTL_PERTHREAD_CURDIR {
    PRTL_DRIVE_LETTER_CURDIR CurrentDirectories;
    PUNICODE_STRING ImageName;
    PVOID Environment;
} RTL_PERTHREAD_CURDIR, *PRTL_PERTHREAD_CURDIR;

#define RtlAssociatePerThreadCurdir(BLOCK,CURRENTDIRECTORIES,IMAGENAME,ENVIRONMENT)\
        (BLOCK)->CurrentDirectories = (CURRENTDIRECTORIES); \
        (BLOCK)->ImageName = (IMAGENAME);                   \
        (BLOCK)->Environment = (ENVIRONMENT);               \
        NtCurrentTeb()->NtTib.SubSystemTib = (PVOID)(BLOCK) \

#define RtlDisAssociatePerThreadCurdir() \
        NtCurrentTeb()->NtTib.SubSystemTib = NULL;

#define RtlGetPerThreadCurdir() \
    ((PRTL_PERTHREAD_CURDIR)(NtCurrentTeb()->NtTib.SubSystemTib))

//
//  Heap Allocator
//

NTURTLAPI
NTSTATUS
NTAPI
RtlInitializeHeapManager(
    VOID
    );

typedef NTSTATUS
(NTAPI * PRTL_HEAP_COMMIT_ROUTINE)(
    IN PVOID Base,
    IN OUT PVOID *CommitAddress,
    IN OUT PSIZE_T CommitSize
    );

typedef struct _RTL_HEAP_PARAMETERS {
    ULONG Length;
    SIZE_T SegmentReserve;
    SIZE_T SegmentCommit;
    SIZE_T DeCommitFreeBlockThreshold;
    SIZE_T DeCommitTotalFreeThreshold;
    SIZE_T MaximumAllocationSize;
    SIZE_T VirtualMemoryThreshold;
    SIZE_T InitialCommit;
    SIZE_T InitialReserve;
    PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
    SIZE_T Reserved[ 2 ];
} RTL_HEAP_PARAMETERS, *PRTL_HEAP_PARAMETERS;

NTURTLAPI
PVOID
NTAPI
RtlCreateHeap(
    IN ULONG Flags,
    IN PVOID HeapBase OPTIONAL,
    IN SIZE_T ReserveSize OPTIONAL,
    IN SIZE_T CommitSize OPTIONAL,
    IN PVOID Lock OPTIONAL,
    IN PRTL_HEAP_PARAMETERS Parameters OPTIONAL
    );

#define HEAP_NO_SERIALIZE               0x00000001      // winnt
#define HEAP_GROWABLE                   0x00000002      // winnt
#define HEAP_GENERATE_EXCEPTIONS        0x00000004      // winnt
#define HEAP_ZERO_MEMORY                0x00000008      // winnt
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010      // winnt
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020      // winnt
#define HEAP_FREE_CHECKING_ENABLED      0x00000040      // winnt
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080      // winnt

#define HEAP_CREATE_ALIGN_16            0x00010000      // winnt Create heap with 16 byte alignment (obsolete)
#define HEAP_CREATE_ENABLE_TRACING      0x00020000      // winnt Create heap call tracing enabled (obsolete)

#define HEAP_SETTABLE_USER_VALUE        0x00000100
#define HEAP_SETTABLE_USER_FLAG1        0x00000200
#define HEAP_SETTABLE_USER_FLAG2        0x00000400
#define HEAP_SETTABLE_USER_FLAG3        0x00000800
#define HEAP_SETTABLE_USER_FLAGS        0x00000E00

#define HEAP_CLASS_0                    0x00000000      // process heap
#define HEAP_CLASS_1                    0x00001000      // private heap
#define HEAP_CLASS_2                    0x00002000      // Kernel Heap
#define HEAP_CLASS_3                    0x00003000      // GDI heap
#define HEAP_CLASS_4                    0x00004000      // User heap
#define HEAP_CLASS_5                    0x00005000      // Console heap
#define HEAP_CLASS_6                    0x00006000      // User Desktop heap
#define HEAP_CLASS_7                    0x00007000      // Csrss Shared heap
#define HEAP_CLASS_8                    0x00008000      // Csr Port heap
#define HEAP_CLASS_MASK                 0x0000F000

#define HEAP_MAXIMUM_TAG                0x0FFF              // winnt
#define HEAP_GLOBAL_TAG                 0x0800
#define HEAP_PSEUDO_TAG_FLAG            0x8000              // winnt
#define HEAP_TAG_SHIFT                  18                  // winnt
#define HEAP_MAKE_TAG_FLAGS( b, o ) ((ULONG)((b) + ((o) << 18)))  // winnt
#define HEAP_TAG_MASK                  (HEAP_MAXIMUM_TAG << HEAP_TAG_SHIFT)

#define HEAP_CREATE_VALID_MASK         (HEAP_NO_SERIALIZE |             \
                                        HEAP_GROWABLE |                 \
                                        HEAP_GENERATE_EXCEPTIONS |      \
                                        HEAP_ZERO_MEMORY |              \
                                        HEAP_REALLOC_IN_PLACE_ONLY |    \
                                        HEAP_TAIL_CHECKING_ENABLED |    \
                                        HEAP_FREE_CHECKING_ENABLED |    \
                                        HEAP_DISABLE_COALESCE_ON_FREE | \
                                        HEAP_CLASS_MASK |               \
                                        HEAP_CREATE_ALIGN_16 |          \
                                        HEAP_CREATE_ENABLE_TRACING)

PVOID
NTAPI
RtlDestroyHeap(
    IN PVOID HeapHandle
    );

PVOID
NTAPI
RtlAllocateHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    );

PVOID
NTAPI
RtlReAllocateHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN SIZE_T Size
    );

BOOLEAN
NTAPI
RtlFreeHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

SIZE_T
NTAPI
RtlSizeHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

NTSTATUS
NTAPI
RtlZeroHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags
    );

NTURTLAPI
VOID
NTAPI
RtlProtectHeap(
    IN PVOID HeapHandle,
    IN BOOLEAN MakeReadOnly
    );

#define RtlProcessHeap() (NtCurrentPeb()->ProcessHeap)

NTURTLAPI
BOOLEAN
NTAPI
RtlLockHeap(
    IN PVOID HeapHandle
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlUnlockHeap(
    IN PVOID HeapHandle
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlGetUserInfoHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    OUT PVOID *UserValue OPTIONAL,
    OUT PULONG UserFlags OPTIONAL
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlSetUserValueHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN PVOID UserValue
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlSetUserFlagsHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress,
    IN ULONG UserFlagsReset,
    IN ULONG UserFlagsSet
    );

typedef struct _RTL_HEAP_TAG_INFO {
    ULONG NumberOfAllocations;
    ULONG NumberOfFrees;
    SIZE_T BytesAllocated;
} RTL_HEAP_TAG_INFO, *PRTL_HEAP_TAG_INFO;


NTURTLAPI
ULONG
NTAPI
RtlCreateTagHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PWSTR TagPrefix OPTIONAL,
    IN PWSTR TagNames
    );

#define RTL_HEAP_MAKE_TAG HEAP_MAKE_TAG_FLAGS

NTURTLAPI
PWSTR
NTAPI
RtlQueryTagHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN USHORT TagIndex,
    IN BOOLEAN ResetCounters,
    OUT PRTL_HEAP_TAG_INFO TagInfo OPTIONAL
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlExtendHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID Base,
    IN SIZE_T Size
    );

NTURTLAPI
SIZE_T
NTAPI
RtlCompactHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlValidateHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlValidateProcessHeaps( VOID );

NTURTLAPI
ULONG
NTAPI
RtlGetProcessHeaps(
    ULONG NumberOfHeaps,
    PVOID *ProcessHeaps
    );


typedef
NTSTATUS (NTAPI * PRTL_ENUM_HEAPS_ROUTINE)(
    PVOID HeapHandle,
    PVOID Parameter
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlEnumProcessHeaps(
    PRTL_ENUM_HEAPS_ROUTINE EnumRoutine,
    PVOID Parameter
    );

typedef struct _RTL_HEAP_USAGE_ENTRY {
    struct _RTL_HEAP_USAGE_ENTRY *Next;
    PVOID Address;
    SIZE_T Size;
    USHORT AllocatorBackTraceIndex;
    USHORT TagIndex;
} RTL_HEAP_USAGE_ENTRY, *PRTL_HEAP_USAGE_ENTRY;

typedef struct _RTL_HEAP_USAGE {
    ULONG Length;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    SIZE_T BytesReserved;
    SIZE_T BytesReservedMaximum;
    PRTL_HEAP_USAGE_ENTRY Entries;
    PRTL_HEAP_USAGE_ENTRY AddedEntries;
    PRTL_HEAP_USAGE_ENTRY RemovedEntries;
    ULONG_PTR Reserved[ 8 ];
} RTL_HEAP_USAGE, *PRTL_HEAP_USAGE;

#define HEAP_USAGE_ALLOCATED_BLOCKS HEAP_REALLOC_IN_PLACE_ONLY
#define HEAP_USAGE_FREE_BUFFER      HEAP_ZERO_MEMORY

NTURTLAPI
NTSTATUS
NTAPI
RtlUsageHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN OUT PRTL_HEAP_USAGE Usage
    );

typedef struct _RTL_HEAP_WALK_ENTRY {
    PVOID   DataAddress;
    SIZE_T   DataSize;
    UCHAR   OverheadBytes;
    UCHAR   SegmentIndex;
    USHORT  Flags;
    union   {
        struct {
            SIZE_T Settable;
            USHORT TagIndex;
            USHORT AllocatorBackTraceIndex;
            ULONG Reserved[ 2 ];
        } Block;
        struct {
            ULONG CommittedSize;
            ULONG UnCommittedSize;
            PVOID FirstEntry;
            PVOID LastEntry;
        } Segment;
    };
} RTL_HEAP_WALK_ENTRY, *PRTL_HEAP_WALK_ENTRY;

NTURTLAPI
NTSTATUS
NTAPI
RtlWalkHeap(
    IN PVOID HeapHandle,
    IN OUT PRTL_HEAP_WALK_ENTRY Entry
    );

typedef struct _RTL_HEAP_ENTRY {
    SIZE_T Size;
    USHORT Flags;
    USHORT AllocatorBackTraceIndex;
    union {
        struct {
            SIZE_T Settable;
            ULONG Tag;
        } s1;   // All other heap entries
        struct {
            SIZE_T CommittedSize;
            PVOID FirstBlock;
        } s2;   // RTL_SEGMENT
    } u;
} RTL_HEAP_ENTRY, *PRTL_HEAP_ENTRY;

#define RTL_HEAP_BUSY               (USHORT)0x0001
#define RTL_HEAP_SEGMENT            (USHORT)0x0002
#define RTL_HEAP_SETTABLE_VALUE     (USHORT)0x0010
#define RTL_HEAP_SETTABLE_FLAG1     (USHORT)0x0020
#define RTL_HEAP_SETTABLE_FLAG2     (USHORT)0x0040
#define RTL_HEAP_SETTABLE_FLAG3     (USHORT)0x0080
#define RTL_HEAP_SETTABLE_FLAGS     (USHORT)0x00E0
#define RTL_HEAP_UNCOMMITTED_RANGE  (USHORT)0x0100
#define RTL_HEAP_PROTECTED_ENTRY    (USHORT)0x0200

typedef struct _RTL_HEAP_TAG {
    ULONG NumberOfAllocations;
    ULONG NumberOfFrees;
    SIZE_T BytesAllocated;
    USHORT TagIndex;
    USHORT CreatorBackTraceIndex;
    WCHAR TagName[ 24 ];
} RTL_HEAP_TAG, *PRTL_HEAP_TAG;

typedef struct _RTL_HEAP_INFORMATION {
    PVOID BaseAddress;
    ULONG Flags;
    USHORT EntryOverhead;
    USHORT CreatorBackTraceIndex;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    ULONG NumberOfTags;
    ULONG NumberOfEntries;
    ULONG NumberOfPseudoTags;
    ULONG PseudoTagGranularity;
    ULONG Reserved[ 5 ];
    PRTL_HEAP_TAG Tags;
    PRTL_HEAP_ENTRY Entries;
} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

typedef struct _RTL_PROCESS_HEAPS {
    ULONG NumberOfHeaps;
    RTL_HEAP_INFORMATION Heaps[ 1 ];
} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS;

//
// Debugging support
//

typedef struct _RTL_DEBUG_INFORMATION {
    HANDLE SectionHandleClient;
    PVOID ViewBaseClient;
    PVOID ViewBaseTarget;
    ULONG_PTR ViewBaseDelta;
    HANDLE EventPairClient;
    HANDLE EventPairTarget;
    HANDLE TargetProcessId;
    HANDLE TargetThreadHandle;
    ULONG Flags;
    SIZE_T OffsetFree;
    SIZE_T CommitSize;
    SIZE_T ViewSize;
    PRTL_PROCESS_MODULES Modules;
    PRTL_PROCESS_BACKTRACES BackTraces;
    PRTL_PROCESS_HEAPS Heaps;
    PRTL_PROCESS_LOCKS Locks;
    PVOID SpecificHeap;
    HANDLE TargetProcessHandle;
    PVOID Reserved[ 6 ];
} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

NTURTLAPI
PRTL_DEBUG_INFORMATION
NTAPI
RtlCreateQueryDebugBuffer(
    IN ULONG MaximumCommit OPTIONAL,
    IN BOOLEAN UseEventPair
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlDestroyQueryDebugBuffer(
    IN PRTL_DEBUG_INFORMATION Buffer
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlQueryProcessDebugInformation(
    IN HANDLE UniqueProcessId,
    IN ULONG Flags,
    IN OUT PRTL_DEBUG_INFORMATION Buffer
    );

#define RTL_QUERY_PROCESS_MODULES       0x00000001
#define RTL_QUERY_PROCESS_BACKTRACES    0x00000002
#define RTL_QUERY_PROCESS_HEAP_SUMMARY  0x00000004
#define RTL_QUERY_PROCESS_HEAP_TAGS     0x00000008
#define RTL_QUERY_PROCESS_HEAP_ENTRIES  0x00000010
#define RTL_QUERY_PROCESS_LOCKS         0x00000020

NTSTATUS
NTAPI
RtlQueryProcessModuleInformation(
    IN OUT PRTL_DEBUG_INFORMATION Buffer
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlQueryProcessBackTraceInformation(
    IN OUT PRTL_DEBUG_INFORMATION Buffer
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlQueryProcessHeapInformation(
    IN OUT PRTL_DEBUG_INFORMATION Buffer
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlQueryProcessLockInformation(
    IN OUT PRTL_DEBUG_INFORMATION Buffer
    );



//
// Routines for manipulating user mode handle tables.  Used for Atoms
// and Local/Global memory allocations.
//

typedef struct _RTL_HANDLE_TABLE_ENTRY {
    union {
        ULONG Flags;                                // Allocated entries have low bit set
        struct _RTL_HANDLE_TABLE_ENTRY *NextFree;   // Free entries use this word for free list
    };
} RTL_HANDLE_TABLE_ENTRY, *PRTL_HANDLE_TABLE_ENTRY;

#define RTL_HANDLE_ALLOCATED    (USHORT)0x0001

typedef struct _RTL_HANDLE_TABLE {
    ULONG MaximumNumberOfHandles;
    ULONG SizeOfHandleTableEntry;
    ULONG Reserved[ 2 ];
    PRTL_HANDLE_TABLE_ENTRY FreeHandles;
    PRTL_HANDLE_TABLE_ENTRY CommittedHandles;
    PRTL_HANDLE_TABLE_ENTRY UnCommittedHandles;
    PRTL_HANDLE_TABLE_ENTRY MaxReservedHandles;
} RTL_HANDLE_TABLE, *PRTL_HANDLE_TABLE;

NTURTLAPI
void
NTAPI
RtlInitializeHandleTable(
    IN ULONG MaximumNumberOfHandles,
    IN ULONG SizeOfHandleTableEntry,
    OUT PRTL_HANDLE_TABLE HandleTable
    );

NTURTLAPI
NTSTATUS
NTAPI
RtlDestroyHandleTable(
    IN OUT PRTL_HANDLE_TABLE HandleTable
    );

NTURTLAPI
PRTL_HANDLE_TABLE_ENTRY
NTAPI
RtlAllocateHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    OUT PULONG HandleIndex OPTIONAL
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlFreeHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    IN PRTL_HANDLE_TABLE_ENTRY Handle
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlIsValidHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    IN PRTL_HANDLE_TABLE_ENTRY Handle
    );

NTURTLAPI
BOOLEAN
NTAPI
RtlIsValidIndexHandle(
    IN PRTL_HANDLE_TABLE HandleTable,
    IN ULONG HandleIndex,
    OUT PRTL_HANDLE_TABLE_ENTRY *Handle
    );



//
// Routines for thread pool.
//

#define WT_EXECUTEDEFAULT       0x00000000                           // winnt
#define WT_EXECUTEINIOTHREAD    0x00000001                           // winnt
#define WT_EXECUTEINUITHREAD    0x00000002                           // winnt
#define WT_EXECUTEINWAITTHREAD  0x00000004                           // winnt
#define WT_EXECUTEONLYONCE      0x00000008                           // winnt
#define WT_EXECUTEINTIMERTHREAD 0x00000020                           // winnt
#define WT_EXECUTELONGFUNCTION  0x00000010                           // winnt
#define WT_EXECUTEINPERSISTENTIOTHREAD  0x00000040                   // winnt
#define WT_EXECUTEINPERSISTENTTHREAD 0x00000080                      // winnt

typedef VOID (NTAPI * WAITORTIMERCALLBACKFUNC) (PVOID, BOOLEAN );   // winnt
typedef VOID (NTAPI * WORKERCALLBACKFUNC) (PVOID );                 // winnt
typedef VOID (NTAPI * APC_CALLBACK_FUNCTION) (NTSTATUS, PVOID, PVOID); // winnt


typedef NTSTATUS (NTAPI RTLP_START_THREAD)(
            PUSER_THREAD_START_ROUTINE,
            HANDLE *);

typedef NTSTATUS (NTAPI RTLP_EXIT_THREAD)(
            NTSTATUS );

typedef RTLP_START_THREAD * PRTLP_START_THREAD ;
typedef RTLP_EXIT_THREAD * PRTLP_EXIT_THREAD ;

NTSTATUS
NTAPI
RtlSetThreadPoolStartFunc(
    PRTLP_START_THREAD StartFunc,
    PRTLP_EXIT_THREAD ExitFunc
    );


NTURTLAPI
NTSTATUS
NTAPI
RtlRegisterWait (
    OUT PHANDLE WaitHandle,
    IN  HANDLE  Handle,
    IN  WAITORTIMERCALLBACKFUNC Function,
    IN  PVOID Context,
    IN  ULONG  Milliseconds,
    IN  ULONG  Flags
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlDeregisterWait(
    IN HANDLE WaitHandle
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlDeregisterWaitEx(
    IN HANDLE WaitHandle,
    IN HANDLE Event
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlQueueWorkItem(
    IN  WORKERCALLBACKFUNC Function,
    IN  PVOID Context,
    IN  ULONG  Flags
    ) ;

NTSTATUS
RtlSetIoCompletionCallback (
    IN  HANDLE  FileHandle,
    IN  APC_CALLBACK_FUNCTION  CompletionProc,
    IN  ULONG Flags
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlCreateTimerQueue(
    OUT PHANDLE TimerQueueHandle
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlCreateTimer(
    IN  HANDLE TimerQueueHandle,
    OUT HANDLE *Handle,
    IN  WAITORTIMERCALLBACKFUNC Function,
    IN  PVOID Context,
    IN  ULONG  DueTime,
    IN  ULONG  Period,
    IN  ULONG  Flags
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlUpdateTimer(
    IN HANDLE TimerQueueHandle,
    IN HANDLE TimerHandle,
    IN ULONG  DueTime,
    IN ULONG  Period
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlDeleteTimer(
    IN HANDLE TimerQueueHandle,
    IN HANDLE TimerToCancel,
    IN HANDLE Event
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlDeleteTimerQueue(
    IN HANDLE TimerQueueHandle
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlDeleteTimerQueueEx(
    IN HANDLE TimerQueueHandle,
    IN HANDLE Event
    ) ;

//--------OBSOLUTE FUNCTIONS: DONT USE----------//
VOID
RtlDebugPrintTimes (
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlCancelTimer(
    IN HANDLE TimerQueueHandle,
    IN HANDLE TimerToCancel
    ) ;

NTURTLAPI
NTSTATUS
NTAPI
RtlSetTimer(
    IN  HANDLE TimerQueueHandle,
    OUT HANDLE *Handle,
    IN  WAITORTIMERCALLBACKFUNC Function,
    IN  PVOID Context,
    IN  ULONG  DueTime,
    IN  ULONG  Period,
    IN  ULONG  Flags
    ) ;

//todo:delete below: dont use the below flag. will be deleted
#define WT_EXECUTEINLONGTHREAD  0x00000010                           // winnt
#define WT_EXECUTEDELETEWAIT    0x00000008                           // winnt

//------end OBSOLUTE FUNCTIONS: DONT USE--------//

//
// Generic LPC server code that uses the thread queue
//

typedef
NTSTATUS
(NTAPI LPCSVR_CONNECT)(
    PVOID Context,
    PPORT_MESSAGE Message,
    PBOOLEAN Accept
    );
typedef LPCSVR_CONNECT * PLPCSVR_CONNECT ;

typedef
NTSTATUS
(NTAPI LPCSVR_REQUEST)(
    PVOID Context,
    PPORT_MESSAGE Input,
    PPORT_MESSAGE Reply
    );

typedef LPCSVR_REQUEST * PLPCSVR_REQUEST ;

typedef
NTSTATUS
(NTAPI LPCSVR_RUNDOWN)(
    PVOID Context,
    PPORT_MESSAGE ClientDiedMsg
    );

typedef LPCSVR_RUNDOWN * PLPCSVR_RUNDOWN ;

typedef enum _LPCSVR_FILTER_RESULT {
    LpcFilterAsync,
    LpcFilterSync,
    LpcFilterDrop
} LPCSVR_FILTER_RESULT ;

typedef
LPCSVR_FILTER_RESULT
(NTAPI LPCSVR_FILTER)(
    PVOID Context,
    PPORT_MESSAGE Message
    );

typedef LPCSVR_FILTER * PLPCSVR_FILTER ;


typedef struct _LPCSVR_INITIALIZE {
    ULONG ContextSize ;
    PLPCSVR_CONNECT ConnectFn ;
    PLPCSVR_REQUEST RequestFn ;
    PLPCSVR_RUNDOWN RundownFn ;
    PLPCSVR_FILTER  FilterFn ;
} LPCSVR_INITIALIZE, * PLPCSVR_INITIALIZE ;

NTSTATUS
NTAPI
RtlCreateLpcServer(
    POBJECT_ATTRIBUTES PortName,
    PLPCSVR_INITIALIZE Init,
    PLARGE_INTEGER IdleTimeout,
    ULONG MessageSize,
    ULONG Options,
    PVOID * Server
    );

NTSTATUS
NTAPI
RtlShutdownLpcServer(
    PVOID Server
    );

NTSTATUS
NTAPI
RtlImpersonateLpcClient(
    PVOID Context,
    PPORT_MESSAGE Message
    );

NTSTATUS
NTAPI
RtlCallbackLpcClient(
    PVOID Context,
    PPORT_MESSAGE Request,
    PPORT_MESSAGE Callback
    );




#if defined (_MSC_VER) && ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4001)
#pragma warning(default:4201)
#pragma warning(default:4214)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif  // _NTURTL_

