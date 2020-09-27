/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    iop.h

Abstract:

    This module contains the private structure definitions and APIs used by
    the NT I/O system.

--*/

#ifndef _IOP_
#define _IOP_

#ifndef FAR
#define FAR
#endif

#include "ntos.h"

//
// Define the global data for the I/O system.
//

extern GENERIC_MAPPING IopFileMapping;

//
// Define a dummy file object for use on stack for fast open operations.
//

typedef struct _DUMMY_FILE_OBJECT {
    OBJECT_HEADER ObjectHeader;
    CHAR FileObjectBody[ sizeof( FILE_OBJECT ) ];
} DUMMY_FILE_OBJECT, *PDUMMY_FILE_OBJECT;

//
// Define the structures private to the I/O system.
//

#define OPEN_PACKET_PATTERN  0xbeaa0251

//
// Define an Open Packet (OP).  An OP is used to communicate information
// between the NtCreateFile service executing in the context of the caller
// and the device object parse routine.  It is the parse routine who actually
// creates the file object for the file.
//

typedef struct _OPEN_PACKET {
    CSHORT Type;
    CSHORT Size;
    PFILE_OBJECT FileObject;
    NTSTATUS FinalStatus;
    ULONG_PTR Information;
    ULONG ParseCheck;
    PFILE_OBJECT RelatedFileObject;

    //
    // The following are the open-specific parameters.  Notice that the desired
    // access field is passed through to the parse routine via the object
    // management architecture, so it does not need to be repeated here.  Also
    // note that the same is true for the file name.
    //

    LARGE_INTEGER AllocationSize;
    ULONG CreateOptions;
    USHORT FileAttributes;
    USHORT ShareAccess;
    ULONG Options;
    ULONG Disposition;
    ULONG DesiredAccess;

    //
    // The following is used when performing a fast network query during open
    // to get back the network file attributes for a file.
    //

    PFILE_NETWORK_OPEN_INFORMATION NetworkInformation;

    //
    // The following pointer is used when a fast open operation for a fast
    // delete or fast query attributes call is being made rather than a
    // general file open.  The dummy file object is actually stored on the
    // the caller's stack rather than allocated pool to speed things up.
    //

    PDUMMY_FILE_OBJECT LocalFileObject;

    //
    // The following is used to indicate that a file is being opened for the
    // sole purpose of querying its attributes.  This causes a considerable
    // number of shortcuts to be taken in the parse, query, and close paths.
    //

    BOOLEAN QueryOnly;

    //
    // The following is used to indicate that a file is being opened for the
    // sole purpose of deleting it.  This causes a considerable number of
    // shortcurs to be taken in the parse and close paths.
    //

    BOOLEAN DeleteOnly;

} OPEN_PACKET, *POPEN_PACKET;

//
// Define I/O completion packet types.
//

typedef enum _COMPLETION_PACKET_TYPE {
    IopCompletionPacketIrp,
    IopCompletionPacketMini
} COMPLETION_PACKET_TYPE, *PCOMPLETION_PACKET_TYPE;

//
// Define the type for completion packets inserted onto completion ports when
// there is no full I/O request packet that was used to perform the I/O
// operation.  This occurs when the fast I/O path is used, and when the user
// directly inserts a completion message.
//
typedef struct _IOP_MINI_COMPLETION_PACKET {

    //
    // The following unnamed structure must be exactly identical
    // to the unnamed structure used in the IRP overlay section used
    // for completion queue entries.
    //

    struct {

        //
        // List entry - used to queue the packet to completion queue, among
        // others.
        //

        LIST_ENTRY ListEntry;

        union {

            //
            // Current stack location - contains a pointer to the current
            // IO_STACK_LOCATION structure in the IRP stack.  This field
            // should never be directly accessed by drivers.  They should
            // use the standard functions.
            //

            struct _IO_STACK_LOCATION *CurrentStackLocation;

            //
            // Minipacket type.
            //

            ULONG PacketType;
        };
    };

    PVOID KeyContext;
    PVOID ApcContext;
    NTSTATUS IoStatus;
    ULONG_PTR IoStatusInformation;
} IOP_MINI_COMPLETION_PACKET, *PIOP_MINI_COMPLETION_PACKET;

extern const UCHAR IopQueryOperationLength[];
extern const UCHAR IopSetOperationLength[];
extern const ULONG IopQueryOperationAccess[];
extern const ULONG IopSetOperationAccess[];
extern const UCHAR IopQueryFsOperationLength[];
extern const ULONG IopQueryFsOperationAccess[];

#define IopAcquireCompletionLock()          KeRaiseIrqlToDpcLevel()
#define IopReleaseCompletionLock(OldIrql)   KeLowerIrql(OldIrql)

//
// Define routines private to the I/O system.
//

VOID
FASTCALL
IopAcquireFileObjectLock(
    IN PFILE_OBJECT FileObject
    );

NTSTATUS
IopAllocateIrpCleanup(
    IN PFILE_OBJECT FileObject,
    IN PKEVENT EventObject OPTIONAL
    );

PIRP
IopAllocateIrpMustSucceed(
    IN CCHAR StackSize
    );

VOID
IopCloseFile(
    IN PVOID Object,
    IN ULONG SystemHandleCount
    );

VOID
IopCompleteUnloadOrDelete(
    IN PDEVICE_OBJECT DeviceObject,
    IN KIRQL Irql
    );

VOID
IopCompleteRequest(
    IN PKAPC Apc,
    IN PKNORMAL_ROUTINE *NormalRoutine,
    IN PVOID *NormalContext,
    IN PVOID *SystemArgument1,
    IN PVOID *SystemArgument2
    );

VOID
IopDecrementDeviceObjectRef(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
IopDeleteFile(
    IN PVOID Object
    );

VOID
IopDeleteIoCompletion(
    IN PVOID Object
    );

//+
//
// VOID
// IopDequeueThreadIrp(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine dequeues the specified I/O Request Packet (IRP) from the
//     thread IRP queue which it is currently queued.
//
//     In checked we set Flink == Blink so we can assert free's of queue'd IRPs
//
// Arguments:
//
//     Irp - Specifies the IRP that is dequeued.
//
// Return Value:
//
//     None.
//
//-

#define IopDequeueThreadIrp( Irp ) \
   { \
   RemoveEntryList( &Irp->ThreadListEntry ); \
   InitializeListHead( &Irp->ThreadListEntry ) ; \
   }


#ifdef  _WIN64
#define IopApcRoutinePresent(ApcRoutine)    ARGUMENT_PRESENT((ULONG_PTR)(ApcRoutine) & ~1)
#else
#define IopApcRoutinePresent(ApcRoutine)    ARGUMENT_PRESENT((ULONG_PTR)(ApcRoutine))
#endif

#define IopApcRoutineMode(ApcRoutine) \
    ((KPROCESSOR_MODE)((ApcRoutine == NtUserIoApcDispatcher) ? UserMode : KernelMode))

VOID
IopDisassociateThreadIrp(
    VOID
    );

VOID
IopDropIrp(
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject
    );

VOID
IopExceptionCleanup(
    IN PFILE_OBJECT FileObject,
    IN PIRP Irp,
    IN PKEVENT EventObject OPTIONAL
    );

NTSTATUS
IopGetFileName(
    IN PFILE_OBJECT FileObject,
    IN ULONG Length,
    OUT PVOID FileInformation,
    OUT PULONG ReturnedLength
    );

//++
//
// VOID
// IopInitializeIrp(
//     IN OUT PIRP Irp,
//     IN USHORT PacketSize,
//     IN CCHAR StackSize
//     )
//
// Routine Description:
//
//     Initializes an IRP.
//
// Arguments:
//
//     Irp - a pointer to the IRP to initialize.
//
//     PacketSize - length, in bytes, of the IRP.
//
//     StackSize - Number of stack locations in the IRP.
//
// Return Value:
//
//     None.
//
//--

#define IopInitializeIrp( Irp, PacketSize, StackSize ) {          \
    RtlZeroMemory( (Irp), (PacketSize) );                         \
    (Irp)->Type = (CSHORT) IO_TYPE_IRP;                           \
    (Irp)->Size = (USHORT) ((PacketSize));                        \
    (Irp)->StackCount = (CCHAR) ((StackSize));                    \
    (Irp)->CurrentLocation = (CCHAR) ((StackSize) + 1);           \
    InitializeListHead (&(Irp)->ThreadListEntry);                 \
    (Irp)->Tail.Overlay.CurrentStackLocation =                    \
        ((PIO_STACK_LOCATION) ((UCHAR *) (Irp) +                  \
            sizeof( IRP ) +                                       \
            ( (StackSize) * sizeof( IO_STACK_LOCATION )))); }

NTSTATUS
IopOpenRenameTarget(
    OUT PHANDLE TargetHandle,
    IN PIRP Irp,
    IN PFILE_RENAME_INFORMATION RenameBuffer,
    IN PFILE_OBJECT FileObject
    );

NTSTATUS
IopParseFile(
    IN PVOID ParseObject,
    IN POBJECT_TYPE ObjectType,
    IN ULONG Attributes,
    IN OUT POBJECT_STRING CompleteName,
    IN OUT POBJECT_STRING RemainingName,
    IN OUT PVOID Context OPTIONAL,
    OUT PVOID *Object
    );

NTSTATUS
IopQueryName(
    IN PVOID Object,
    IN BOOLEAN HasObjectName,
    OUT POBJECT_NAME_INFORMATION ObjectNameInfo,
    IN ULONG Length,
    OUT PULONG ReturnLength
    );

NTSTATUS
IopQueryXxxInformation(
    IN PFILE_OBJECT FileObject,
    IN ULONG InformationClass,
    IN ULONG Length,
    OUT PVOID Information,
    OUT PULONG ReturnedLength,
    IN BOOLEAN FileInformation
    );

//+
// VOID
// IopQueueThreadIrp(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine queues the specified I/O Request Packet (IRP) to the thread
//     whose TCB address is stored in the packet.
//
// Arguments:
//
//     Irp - Supplies the IRP to be queued for the specified thread.
//
// Return Value:
//
//     None.
//
//-

#define IopQueueThreadIrp( Irp ) {                      \
    KIRQL irql;                                         \
    KeRaiseIrql( APC_LEVEL, &irql );                    \
    InsertHeadList( &Irp->Tail.Overlay.Thread->IrpList, \
                    &Irp->ThreadListEntry );            \
    KeLowerIrql( irql );                                \
    }

BOOLEAN
FASTCALL
IopReleaseFileObjectLock(
    IN PFILE_OBJECT FileObject
    );

NTSTATUS
IopSynchronousApiServiceTail(
    IN NTSTATUS ReturnedStatus,
    IN PKEVENT Event,
    IN PIRP Irp,
    IN PIO_STATUS_BLOCK LocalIoStatus,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTSTATUS
IopSynchronousServiceTail(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN DeferredIoCompletion,
    IN BOOLEAN SynchronousIo
    );

VOID
IopUserCompletion(
    IN PKAPC Apc,
    IN PKNORMAL_ROUTINE *NormalRoutine,
    IN PVOID *NormalContext,
    IN PVOID *SystemArgument1,
    IN PVOID *SystemArgument2
    );

NTSTATUS
IopXxxControlFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN DeviceIoControl
    );

#endif // _IOP_
