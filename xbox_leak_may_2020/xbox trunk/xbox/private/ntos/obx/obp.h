/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    obp.h

Abstract:

    This module contains the private data structures and procedure
    prototypes for the object management system.

--*/

#ifndef _OBP_
#define _OBP_

#include <ntos.h>

//
// Bit flag macros.
//

#define ObpIsFlagSet(flagset, flag)             (((flagset) & (flag)) != 0)
#define ObpIsFlagClear(flagset, flag)           (((flagset) & (flag)) == 0)

//
// Define macros to lock and unlock the object manager.
//

#define ObpAcquireObjectManagerLock(OldIrql) \
    *(OldIrql) = KeRaiseIrqlToDpcLevel()

#define ObpReleaseObjectManagerLock(OldIrql) \
    KeLowerIrql(OldIrql)

#define ObpAssertObjectManagerLock() \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

//
// Routines which operate on directory objects.
//

ULONG
FASTCALL
ObpComputeHashIndex(
    IN POBJECT_STRING ElementName
    );

BOOLEAN
ObpLookupElementNameInDirectory(
    IN POBJECT_DIRECTORY Directory,
    IN POBJECT_STRING ElementName,
    IN BOOLEAN ResolveSymbolicLink,
    OUT PVOID *ReturnedObject
    );

NTSTATUS
ObpReferenceObjectByName(
    IN HANDLE RootDirectoryHandle,
    IN POBJECT_STRING ObjectName,
    IN ULONG Attributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PVOID *ReturnedObject
    );

//
// Routines which operate on the handle table.
//

VOID
ObpInitializeObjectHandleTable(
    VOID
    );

HANDLE
ObpCreateObjectHandle(
    PVOID Object
    );

PVOID
ObpGetObjectHandleReference(
    HANDLE Handle
    );

PVOID
ObpGetObjectHandleContents(
    HANDLE Handle
    );

PVOID
ObpDestroyObjectHandle(
    HANDLE Handle
    );

//
// External symbols.
//

extern POBJECT_DIRECTORY ObpRootDirectoryObject;
extern POBJECT_DIRECTORY ObpDosDevicesDirectoryObject;
extern POBJECT_DIRECTORY ObpIoDevicesDirectoryObject;
extern POBJECT_DIRECTORY ObpWin32NamedObjectsDirectoryObject;
extern PVOID ObpDosDevicesDriveLetterMap['Z' - 'A' + 1];
extern OBJECT_HANDLE_TABLE ObpObjectHandleTable;

#endif  // OBP
