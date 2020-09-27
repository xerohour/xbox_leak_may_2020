/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    ob.h

Abstract:

    This module contains the object manager structure public data
    structures and procedure prototypes to be used within the NT
    system.

--*/

#ifndef _OB_
#define _OB_

//
// Object manager handle table support.
//

#define OB_HANDLES_PER_TABLE_SHIFT      6
#define OB_HANDLES_PER_TABLE            (1 << OB_HANDLES_PER_TABLE_SHIFT)
#define OB_TABLES_PER_SEGMENT           8
#define OB_HANDLES_PER_SEGMENT          (OB_TABLES_PER_SEGMENT * OB_HANDLES_PER_TABLE)

typedef struct _OBJECT_HANDLE_TABLE {
    LONG HandleCount;
    LONG_PTR FirstFreeTableEntry;
    HANDLE NextHandleNeedingPool;
    PVOID **RootTable;
    PVOID *BuiltinRootTable[OB_TABLES_PER_SEGMENT];
} OBJECT_HANDLE_TABLE, *POBJECT_HANDLE_TABLE;

//
// Object type information structure.
//

typedef PVOID (*OB_ALLOCATE_METHOD)(
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

typedef VOID (*OB_FREE_METHOD)(
    IN PVOID Pointer
    );

typedef VOID (*OB_CLOSE_METHOD)(
    IN PVOID Object,
    IN ULONG SystemHandleCount
    );

typedef VOID (*OB_DELETE_METHOD)(
    IN PVOID Object
    );

typedef NTSTATUS (*OB_PARSE_METHOD)(
    IN PVOID ParseObject,
    IN struct _OBJECT_TYPE *ObjectType,
    IN ULONG Attributes,
    IN OUT POBJECT_STRING CompleteName,
    IN OUT POBJECT_STRING RemainingName,
    IN OUT PVOID Context OPTIONAL,
    OUT PVOID *Object
    );

typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;

//
// Object header structure.
//

typedef struct _OBJECT_HEADER {
    LONG PointerCount;
    LONG HandleCount;
    POBJECT_TYPE Type;
    ULONG Flags;
    QUAD Body;
} OBJECT_HEADER, *POBJECT_HEADER;

#define OB_FLAG_NAMED_OBJECT            0x01
#define OB_FLAG_PERMANENT_OBJECT        0x02
#define OB_FLAG_ATTACHED_OBJECT         0x04

#define OBJECT_TO_OBJECT_HEADER(Object) \
    CONTAINING_RECORD(Object, OBJECT_HEADER, Body)

#define OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object) \
    ((POBJECT_HEADER_NAME_INFO)OBJECT_TO_OBJECT_HEADER(Object) - 1)

#define OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo) \
    ((POBJECT_HEADER)((POBJECT_HEADER_NAME_INFO)(ObjectHeaderNameInfo) + 1))

#define OBJECT_HEADER_TO_OBJECT_HEADER_NAME_INFO(ObjectHeader) \
    ((POBJECT_HEADER_NAME_INFO)(ObjectHeader) - 1)

#define OBJECT_HEADER_NAME_INFO_TO_OBJECT(ObjectHeaderNameInfo) \
    (&OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo)->Body)

typedef struct _OBJECT_HEADER_NAME_INFO {
    struct _OBJECT_HEADER_NAME_INFO *ChainLink;
    struct _OBJECT_DIRECTORY *Directory;
    OBJECT_STRING Name;
} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;

//
// Object directory structure.
//

#define OB_NUMBER_HASH_BUCKETS          11

typedef struct _OBJECT_DIRECTORY {
    struct _OBJECT_HEADER_NAME_INFO *HashBuckets[OB_NUMBER_HASH_BUCKETS];
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

//
// Symbolic link object structure.
//

typedef struct _OBJECT_SYMBOLIC_LINK {
    PVOID LinkTargetObject;
    OBJECT_STRING LinkTarget;
} OBJECT_SYMBOLIC_LINK, *POBJECT_SYMBOLIC_LINK;

//
// The following global event can be used for types of objects that can never be
// signaled.
//

extern KEVENT ObpDefaultObject;

//
// Define the maximum number of KWAIT_BLOCKs that will be allocated on the stack
// for a NtWaitForMultipleObjectsEx call.
//
// This number is chosen such that:
//
// (OB_MAXIMUM_STACK_WAIT_BLOCKS * (sizeof(PVOID) + sizeof(KWAIT_BLOCK))) <=
//      sizeof(PVOID) * MAXIMUM_WAIT_OBJECTS
//

#define OB_MAXIMUM_STACK_WAIT_BLOCKS    9

//
// Define the predefined handle value for the \?? directory.
//

#define ObDosDevicesDirectory()         ((HANDLE)-3)

//
// Define the predefined handle value for the \Win32NamedObjects directory.
//

#define ObWin32NamedObjectsDirectory()  ((HANDLE)-4)

//
// Prototypes.
//

BOOLEAN
ObInitSystem(
    VOID
    );

VOID
ObDissectName (
    IN OBJECT_STRING Path,
    OUT POBJECT_STRING FirstName,
    OUT POBJECT_STRING RemainingName
    );

// begin_ntddk

NTKERNELAPI
NTSTATUS
ObCreateObject(
    IN POBJECT_TYPE ObjectType,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG ObjectBodySize,
    OUT PVOID *Object
    );

NTKERNELAPI
NTSTATUS
ObInsertObject(
    IN PVOID Object,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG ObjectPointerBias,
    OUT PHANDLE Handle
    );

NTKERNELAPI
NTSTATUS
ObReferenceObjectByHandle(
    IN HANDLE Handle,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    OUT PVOID *Object
    );

NTKERNELAPI
NTSTATUS
ObOpenObjectByName(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PHANDLE Handle
    );

NTKERNELAPI
NTSTATUS
ObOpenObjectByPointer(
    IN PVOID Object,
    IN POBJECT_TYPE ObjectType,
    OUT PHANDLE Handle
    );

NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
    IN POBJECT_STRING ObjectName,
    IN ULONG Attributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PVOID *Object
    );

NTKERNELAPI
VOID
ObMakeTemporaryObject(
    IN PVOID Object
    );

NTKERNELAPI
VOID
FASTCALL
ObfReferenceObject(
    IN PVOID Object
    );

#define ObReferenceObject(Object) ObfReferenceObject(Object)

NTKERNELAPI
NTSTATUS
ObReferenceObjectByPointer(
    IN PVOID Object,
    IN POBJECT_TYPE ObjectType
    );

NTKERNELAPI
VOID
FASTCALL
ObfDereferenceObject(
    IN PVOID Object
    );

#define ObDereferenceObject(Object) ObfDereferenceObject(Object)

// end_ntddk

#endif // OB
