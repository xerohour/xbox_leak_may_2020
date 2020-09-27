/*++ BUILD Version: 0013    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    io.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the NT I/O system.

Author:

    Darryl E. Havens (darrylh) 12-Apr-1989


Revision History:


--*/

#ifndef _IO_
#define _IO_

// begin_ntddk begin_wdm begin_nthal begin_ntifs begin_ntndis
//
// Define I/O system data structure type codes.  Each major data structure in
// the I/O system has a type code  The type field in each structure is at the
// same offset.  The following values can be used to determine which type of
// data structure a pointer refers to.
//

#define IO_TYPE_DEVICE                  0x00000003
#define IO_TYPE_DRIVER                  0x00000004
#define IO_TYPE_FILE                    0x00000005
#define IO_TYPE_IRP                     0x00000006
#define IO_TYPE_OPEN_PACKET             0x00000008
#define IO_TYPE_TIMER                   0x00000009

//
// Define the major function codes for IRPs.
//

#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CLOSE                    0x01
#define IRP_MJ_READ                     0x02
#define IRP_MJ_WRITE                    0x03
#define IRP_MJ_QUERY_INFORMATION        0x04
#define IRP_MJ_SET_INFORMATION          0x05
#define IRP_MJ_FLUSH_BUFFERS            0x06
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x07
#define IRP_MJ_DIRECTORY_CONTROL        0x08
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x09
#define IRP_MJ_DEVICE_CONTROL           0x0a
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0b
#define IRP_MJ_SHUTDOWN                 0x0c
#define IRP_MJ_CLEANUP                  0x0d
#define IRP_MJ_MAXIMUM_FUNCTION         0x0d

//
// Make the Scsi major code the same as internal device control.
//

#define IRP_MJ_SCSI                     IRP_MJ_INTERNAL_DEVICE_CONTROL

//
// Define the minor function codes for IRPs.  The lower 128 codes, from 0x00 to
// 0x7f are reserved to Microsoft.  The upper 128 codes, from 0x80 to 0xff, are
// reserved to customers of Microsoft.
//

// end_ntndis

//
// Define option flags for IoCreateFile.  Note that these values must be
// exactly the same as the SL_... flags for a create function.  Note also
// that there are flags that may be passed to IoCreateFile that are not
// placed in the stack location for the create IRP.  These flags start in
// the next byte.
//

#define IO_FORCE_ACCESS_CHECK           0x0001
#define IO_OPEN_TARGET_DIRECTORY        0x0004
// end_ntddk end_wdm end_nthal

//
// Flags not passed to driver
//

// begin_ntddk begin_wdm
#define IO_NO_PARAMETER_CHECKING        0x0100

//
// Define Information fields for whether or not a REPARSE or a REMOUNT has
// occurred in the file system.
//

#define IO_REPARSE                      0x0
#define IO_REMOUNT                      0x1

// end_ntddk end_wdm

#define IO_CHECK_CREATE_PARAMETERS      0x0200

// end_ntifs

// begin_ntddk begin_wdm begin_nthal begin_ntifs
//
// Define the structures used by the I/O system
//

//
// Define empty typedefs for the _IRP, _DEVICE_OBJECT, and _DRIVER_OBJECT
// structures so they may be referenced by function types before they are
// actually defined.
//
struct _DEVICE_DESCRIPTION;
struct _DEVICE_OBJECT;
struct _DMA_ADAPTER;
struct _DRIVER_OBJECT;
struct _DRIVE_LAYOUT_INFORMATION;
struct _DISK_PARTITION;
struct _FILE_OBJECT;
struct _IRP;
struct _SCSI_REQUEST_BLOCK;

// end_wdm

// begin_wdm begin_ntndis
//
// Define driver dispatch routine type.
//

typedef
NTSTATUS
(*PDRIVER_DISPATCH) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver start I/O routine type.
//

typedef
VOID
(*PDRIVER_STARTIO) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver device object deletion routine type.
//

typedef
VOID
(*PDRIVER_DELETEDEVICE) (
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Define driver dismount volume routine type.
//

typedef
NTSTATUS
(*PDRIVER_DISMOUNTVOLUME) (
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Define Device Object (DO) flags
//
// end_wdm end_ntddk end_nthal end_ntifs

#define DO_RAW_MOUNT_ONLY               0x00000001
#define DO_EXCLUSIVE                    0x00000002      // ntddk nthal ntifs wdm
#define DO_DIRECT_IO                    0x00000004      // ntddk nthal ntifs wdm
#define DO_DEVICE_HAS_NAME              0x00000008      // ntddk nthal ntifs
#define DO_DEVICE_INITIALIZING          0x00000010      // ntddk nthal ntifs wdm
#define DO_SCATTER_GATHER_IO            0x00000040

//
// Define Device Object StartIo flags
//

#define DO_STARTIO_BUSY                 0x01
#define DO_STARTIO_REQUESTED            0x02
#define DO_STARTIO_REQUESTED_BYKEY      0x04

// begin_wdm begin_ntddk begin_nthal begin_ntifs
//
// Device Object structure definition
//

typedef struct _DEVICE_OBJECT {
    CSHORT Type;
    USHORT Size;
    LONG ReferenceCount;
    struct _DRIVER_OBJECT *DriverObject;
    struct _DEVICE_OBJECT *MountedOrSelfDevice;
    struct _IRP *CurrentIrp;
    ULONG Flags;                                // See above:  DO_...
    PVOID DeviceExtension;
    UCHAR DeviceType;
    UCHAR StartIoFlags;
    CCHAR StackSize;
    BOOLEAN DeletePending;
    ULONG SectorSize;
    ULONG AlignmentRequirement;
    KDEVICE_QUEUE DeviceQueue;
    KEVENT DeviceLock;
    ULONG StartIoKey;
} DEVICE_OBJECT;
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT; // ntndis

typedef struct _DRIVER_OBJECT {

    //
    // The following section describes the entry points to this particular
    // driver.  Note that the major function dispatch table must be the last
    // field in the object so that it remains extensible.
    //

    PDRIVER_STARTIO DriverStartIo;
    PDRIVER_DELETEDEVICE DriverDeleteDevice;
    PDRIVER_DISMOUNTVOLUME DriverDismountVolume;
    PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];

} DRIVER_OBJECT;
typedef struct _DRIVER_OBJECT *PDRIVER_OBJECT; // ntndis

//
// Define the format of a completion message.
//

typedef struct _IO_COMPLETION_CONTEXT {
    PVOID Port;
    PVOID Key;
} IO_COMPLETION_CONTEXT, *PIO_COMPLETION_CONTEXT;

//
// Define File Object (FO) flags
//

#define FO_SYNCHRONOUS_IO               0x00000001
#define FO_ALERTABLE_IO                 0x00000002
#define FO_NO_INTERMEDIATE_BUFFERING    0x00000004
#define FO_SEQUENTIAL_ONLY              0x00000008
#define FO_CLEANUP_COMPLETE             0x00000010
#define FO_HANDLE_CREATED               0x00000020
#define FO_RANDOM_ACCESS                0x00000040

#include "pshpack4.h"

typedef struct _FILE_OBJECT {
    CSHORT Type;
    BOOLEAN DeletePending : 1;
    BOOLEAN ReadAccess : 1;
    BOOLEAN WriteAccess : 1;
    BOOLEAN DeleteAccess : 1;
    BOOLEAN SharedRead : 1;
    BOOLEAN SharedWrite : 1;
    BOOLEAN SharedDelete : 1;
    BOOLEAN Reserved : 1;
    UCHAR Flags;
    PDEVICE_OBJECT DeviceObject;
    PVOID FsContext;
    PVOID FsContext2;
    NTSTATUS FinalStatus;
    LARGE_INTEGER CurrentByteOffset;
    struct _FILE_OBJECT *RelatedFileObject;
    PIO_COMPLETION_CONTEXT CompletionContext;
    LONG LockCount;
    KEVENT Lock;
    KEVENT Event;
} FILE_OBJECT;
typedef struct _FILE_OBJECT *PFILE_OBJECT; // ntndis

#include "poppack.h"

//
// Define I/O Request Packet (IRP) flags
//

#define IRP_NOCACHE                     0x00000001
#define IRP_MOUNT_COMPLETION            0x00000002
#define IRP_SYNCHRONOUS_API             0x00000004
#define IRP_CREATE_OPERATION            0x00000008
#define IRP_READ_OPERATION              0x00000010
#define IRP_WRITE_OPERATION             0x00000020
#define IRP_CLOSE_OPERATION             0x00000040
// end_wdm

#define IRP_DEFER_IO_COMPLETION         0x00000080
#define IRP_OB_QUERY_NAME               0x00000100
#define IRP_UNLOCK_USER_BUFFER          0x00000200
#define IRP_SCATTER_GATHER_OPERATION    0x00000400
#define IRP_UNMAP_SEGMENT_ARRAY         0x00000800
#define IRP_NO_CANCELIO                 0x00001000

// begin_wdm
//
// I/O Request Packet (IRP) definition
//

typedef struct _IRP {
    CSHORT Type;
    USHORT Size;

    //
    // Define the common fields used to control the IRP.
    //

    //
    // Flags word - used to remember various flags.
    //

    ULONG Flags;

    //
    // Thread list entry - allows queueing the IRP to the thread pending I/O
    // request packet list.
    //

    LIST_ENTRY ThreadListEntry;

    //
    // I/O status - final status of operation.
    //

    IO_STATUS_BLOCK IoStatus;

    //
    // Stack state information.
    //

    CHAR StackCount;
    CHAR CurrentLocation;

    //
    // Pending returned - TRUE if pending was initially returned as the
    // status for this packet.
    //

    BOOLEAN PendingReturned;

    //
    // Cancel - packet has been canceled.
    //

    BOOLEAN Cancel;

    //
    // User parameters.
    //

    PIO_STATUS_BLOCK UserIosb;
    PKEVENT UserEvent;
    union {
        struct {
            PIO_APC_ROUTINE UserApcRoutine;
            PVOID UserApcContext;
        } AsynchronousParameters;
        LARGE_INTEGER AllocationSize;
    } Overlay;

    //
    // Note that the UserBuffer parameter is outside of the stack so that I/O
    // completion can copy data back into the user's address space without
    // having to know exactly which service was being invoked.  The length
    // of the copy is stored in the second half of the I/O status block. If
    // the UserBuffer field is NULL, then no copy is performed.
    //

    PVOID UserBuffer;

    //
    // If this is a scatter/gather I/O request, then the individual pages for
    // the request can be obtained via this array.
    //

    PFILE_SEGMENT_ELEMENT SegmentArray;

    //
    // Stores the number of bytes that were locked down for direct I/O.
    //

    ULONG LockedBufferLength;

    //
    // Kernel structures
    //
    // The following section contains kernel structures which the IRP needs
    // in order to place various work information in kernel controller system
    // queues.  Because the size and alignment cannot be controlled, they are
    // placed here at the end so they just hang off and do not affect the
    // alignment of other fields in the IRP.
    //

    union {

        struct {

            union {

                //
                // DeviceQueueEntry - The device queue entry field is used to
                // queue the IRP to the device driver device queue.
                //

                KDEVICE_QUEUE_ENTRY DeviceQueueEntry;

                struct {

                    //
                    // The following are available to the driver to use in
                    // whatever manner is desired, while the driver owns the
                    // packet.
                    //

                    PVOID DriverContext[5];
                } ;
            } ;

            //
            // Thread - pointer to caller's Thread Control Block.
            //

            PETHREAD Thread;

            //
            // The following unnamed structure must be exactly identical
            // to the unnamed structure used in the minipacket header used
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

            //
            // Original file object - pointer to the original file object
            // that was used to open the file.  This field is owned by the
            // I/O system and should not be used by any other drivers.
            //

            PFILE_OBJECT OriginalFileObject;

        } Overlay;

        //
        // APC - This APC control block is used for the special kernel APC as
        // well as for the caller's APC, if one was specified in the original
        // argument list.  If so, then the APC is reused for the normal APC for
        // whatever mode the caller was in and the "special" routine that is
        // invoked before the APC gets control simply deallocates the IRP.
        //

        KAPC Apc;

        //
        // CompletionKey - This is the key that is used to distinguish
        // individual I/O operations initiated on a single file handle.
        //

        PVOID CompletionKey;

    } Tail;

} IRP, *PIRP;

//
// Define completion routine types for use in stack locations in an IRP
//

typedef
NTSTATUS
(*PIO_COMPLETION_ROUTINE) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

//
// Define stack location control flags
//

#define SL_PENDING_RETURNED             0x01
#define SL_MUST_COMPLETE                0x02
#define SL_INVOKE_ON_CANCEL             0x20
#define SL_INVOKE_ON_SUCCESS            0x40
#define SL_INVOKE_ON_ERROR              0x80

//
// Define flags for various functions
//

//
// Create / Create Named Pipe
//
// The following flags must exactly match those in the IoCreateFile call's
// options.  The case sensitive flag is added in later, by the parse routine,
// and is not an actual option to open.  Rather, it is part of the object
// manager's attributes structure.
//

#define SL_FORCE_ACCESS_CHECK           0x01
#define SL_OPEN_TARGET_DIRECTORY        0x04

#define SL_CASE_SENSITIVE               0x80

//
// Read / Write
//

#define SL_OVERRIDE_VERIFY_VOLUME       0x02
#define SL_FSCACHE_REQUEST              0x80

//
// Device I/O Control
//
//
// Same SL_OVERRIDE_VERIFY_VOLUME as for read/write above.
//

//
// QueryDirectory / QueryEa / QueryQuota
//

#define SL_RESTART_SCAN                 0x01
#define SL_RETURN_SINGLE_ENTRY          0x02
#define SL_INDEX_SPECIFIED              0x04

//
// FileSystemControl
//
//    minor: mount/verify volume
//

#define SL_ALLOW_RAW_MOUNT              0x01

//
// Define I/O Request Packet (IRP) stack locations
//

#include "pshpack4.h"

#if defined(_WIN64)
#define POINTER_ALIGNMENT DECLSPEC_ALIGN(8)
#else
#define POINTER_ALIGNMENT
#endif

typedef struct _IO_STACK_LOCATION {
    UCHAR MajorFunction;
    UCHAR MinorFunction;
    UCHAR Flags;
    UCHAR Control;

    //
    // The following user parameters are based on the service that is being
    // invoked.  Drivers and file systems can determine which set to use based
    // on the above major and minor function codes.
    //

    union {

        //
        // System service parameters for:  NtCreateFile
        //

        struct {
            ACCESS_MASK DesiredAccess;
            ULONG Options;
            USHORT POINTER_ALIGNMENT FileAttributes;
            USHORT ShareAccess;
            POBJECT_STRING RemainingName;
        } Create;

        //
        // System service parameters for:  NtReadFile
        //

        struct {
            ULONG Length;
            union {
                ULONG BufferOffset;
                PVOID CacheBuffer;
            };
            LARGE_INTEGER ByteOffset;
        } Read;

        //
        // System service parameters for:  NtWriteFile
        //

        struct {
            ULONG Length;
            union {
                ULONG BufferOffset;
                PVOID CacheBuffer;
            };
            LARGE_INTEGER ByteOffset;
        } Write;

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtQueryDirectoryFile
        //

        struct {
            ULONG Length;
            POBJECT_STRING FileName;
            FILE_INFORMATION_CLASS FileInformationClass;
        } QueryDirectory;

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtQueryInformationFile
        //

        struct {
            ULONG Length;
            FILE_INFORMATION_CLASS POINTER_ALIGNMENT FileInformationClass;
        } QueryFile;

        //
        // System service parameters for:  NtSetInformationFile
        //

        struct {
            ULONG Length;
            FILE_INFORMATION_CLASS POINTER_ALIGNMENT FileInformationClass;
            PFILE_OBJECT FileObject;
        } SetFile;

        //
        // System service parameters for:  NtQueryVolumeInformationFile
        //

        struct {
            ULONG Length;
            FS_INFORMATION_CLASS POINTER_ALIGNMENT FsInformationClass;
        } QueryVolume;

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtSetVolumeInformationFile
        //

        struct {
            ULONG Length;
            FS_INFORMATION_CLASS POINTER_ALIGNMENT FsInformationClass;
        } SetVolume;

        //
        // System service parameters for:  NtFsControlFile
        //
        // Note that the user's output buffer is stored in the UserBuffer field
        // and the user's input buffer is stored in the SystemBuffer field.
        //

        struct {
            ULONG OutputBufferLength;
            PVOID InputBuffer;
            ULONG POINTER_ALIGNMENT InputBufferLength;
            ULONG POINTER_ALIGNMENT FsControlCode;
        } FileSystemControl;

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtFlushBuffersFile
        //
        // No extra user-supplied parameters.
        //

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtCancelIoFile
        //
        // No extra user-supplied parameters.
        //

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtDeviceIoControlFile
        //

        struct {
            ULONG OutputBufferLength;
            PVOID InputBuffer;
            ULONG POINTER_ALIGNMENT InputBufferLength;
            ULONG POINTER_ALIGNMENT IoControlCode;
        } DeviceIoControl;

        //
        // Parameters for Scsi with internal device contorl.
        //

        struct {
            struct _SCSI_REQUEST_BLOCK *Srb;
        } Scsi;

        //
        // Parameters for Cleanup
        //
        // No extra parameters supplied
        //

// end_ntddk end_wdm end_nthal

        //
        // Parameters for internally routed IDE IRPs.
        //

        struct {
            ULONG Length;
            PUCHAR Buffer;
            ULONG SectorNumber;
            ULONG BufferOffset;
        } IdexReadWrite;

// begin_ntddk begin_wdm begin_nthal

        //
        // Others - driver-specific
        //

        struct {
            PVOID Argument1;
            PVOID Argument2;
            PVOID Argument3;
            PVOID Argument4;
        } Others;

    } Parameters;

    //
    // Save a pointer to this device driver's device object for this request
    // so it can be passed to the completion routine if needed.
    //

    PDEVICE_OBJECT DeviceObject;

    //
    // The following location contains a pointer to the file object for this
    //

    PFILE_OBJECT FileObject;

    //
    // The following routine is invoked depending on the flags in the above
    // flags field.
    //

    PIO_COMPLETION_ROUTINE CompletionRoutine;

    //
    // The following is used to store the address of the context parameter
    // that should be passed to the CompletionRoutine.
    //

    PVOID Context;

} IO_STACK_LOCATION, *PIO_STACK_LOCATION;

#include "poppack.h"

//
// Define the share access structure used by file systems to determine
// whether or not another accessor may open the file.
//

typedef struct _SHARE_ACCESS {
    UCHAR OpenCount;
    UCHAR Readers;
    UCHAR Writers;
    UCHAR Deleters;
    UCHAR SharedRead;
    UCHAR SharedWrite;
    UCHAR SharedDelete;
} SHARE_ACCESS, *PSHARE_ACCESS;

// end_wdm end_ntddk end_nthal

// begin_ntddk begin_wdm begin_nthal
//
// Public I/O routine definitions
//

NTKERNELAPI
PIRP
IoAllocateIrp(
    IN CCHAR StackSize
    );

NTKERNELAPI
PIRP
IoBuildAsynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
    );

NTKERNELAPI
PIRP
IoBuildDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTKERNELAPI
PIRP
IoBuildSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTKERNELAPI
NTSTATUS
FASTCALL
IofCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

#define IoCallDriver(a,b)   \
        IofCallDriver(a,b)

// end_ntddk end_wdm end_nthal end_ntifs

NTKERNELAPI
VOID
IoCancelThreadIo(
    IN PETHREAD Thread
    );

// begin_ntifs
// begin_ntddk begin_wdm begin_nthal

NTKERNELAPI
NTSTATUS
IoCheckShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess,
    IN BOOLEAN Update
    );

NTKERNELAPI
VOID
FASTCALL
IofCompleteRequest(
    IN PIRP Irp,
    IN CCHAR PriorityBoost
    );

#define IoCompleteRequest(a,b)  \
        IofCompleteRequest(a,b)

NTKERNELAPI
NTSTATUS
IoCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN POBJECT_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN BOOLEAN Exclusive,
    OUT PDEVICE_OBJECT *DeviceObject
    );

// end_nthal

NTKERNELAPI
NTSTATUS
IoCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG Disposition,
    IN ULONG CreateOptions,
    IN ULONG Options
    );

// end_ntddk end_wdm

// begin_nthal begin_ntddk begin_wdm

NTKERNELAPI
NTSTATUS
IoCreateSymbolicLink(
    IN POBJECT_STRING SymbolicLinkName,
    IN POBJECT_STRING DeviceName
    );

NTKERNELAPI
VOID
IoDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
NTSTATUS
IoDeleteSymbolicLink(
    IN POBJECT_STRING SymbolicLinkName
    );

NTKERNELAPI
NTSTATUS
IoDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
NTSTATUS
IoDismountVolumeByName(
    IN POBJECT_STRING DeviceName
    );

NTKERNELAPI
VOID
IoFreeIrp(
    IN PIRP Irp
    );

//++
//
// PIO_STACK_LOCATION
// IoGetCurrentIrpStackLocation(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to return a pointer to the current stack location
//     in an I/O Request Packet (IRP).
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     The function value is a pointer to the current stack location in the
//     packet.
//
//--

#define IoGetCurrentIrpStackLocation( Irp ) ( (Irp)->Tail.Overlay.CurrentStackLocation )

// end_nthal

//++
//
// ULONG
// IoGetFunctionCodeFromCtlCode(
//     IN ULONG ControlCode
//     )
//
// Routine Description:
//
//     This routine extracts the function code from IOCTL and FSCTL function
//     control codes.
//     This routine should only be used by kernel mode code.
//
// Arguments:
//
//     ControlCode - A function control code (IOCTL or FSCTL) from which the
//         function code must be extracted.
//
// Return Value:
//
//     The extracted function code.
//
// Note:
//
//     The CTL_CODE macro, used to create IOCTL and FSCTL function control
//     codes, is defined in ntioapi.h
//
//--

#define IoGetFunctionCodeFromCtlCode( ControlCode ) (\
    ( ControlCode >> 2) & 0x00000FFF )

// begin_nthal

//++
//
// PIO_STACK_LOCATION
// IoGetNextIrpStackLocation(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to return a pointer to the next stack location
//     in an I/O Request Packet (IRP).
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     The function value is a pointer to the next stack location in the packet.
//
//--

#define IoGetNextIrpStackLocation( Irp ) (\
    (Irp)->Tail.Overlay.CurrentStackLocation - 1 )

// end_ntddk end_wdm end_nthal

// begin_ntddk begin_wdm begin_nthal

NTKERNELAPI
VOID
IoInitializeIrp(
    IN OUT PIRP Irp,
    IN USHORT PacketSize,
    IN CCHAR StackSize
    );

// end_ntddk end_wdm end_nthal end_ntifs

NTKERNELAPI
BOOLEAN
IoInitSystem(
    VOID
    );

// begin_ntddk begin_wdm begin_nthal begin_ntifs

NTKERNELAPI
NTSTATUS
IoInvalidDeviceRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//++
//
// BOOLEAN
// IoIsErrorUserInduced(
//     IN NTSTATUS Status
//     )
//
// Routine Description:
//
//     This routine is invoked to determine if an error was as a
//     result of user actions.  Typically these error are related
//     to removable media and will result in a pop-up.
//
// Arguments:
//
//     Status - The status value to check.
//
// Return Value:
//     The function value is TRUE if the user induced the error,
//     otherwise FALSE is returned.
//
//--
#define IoIsErrorUserInduced( Status ) ((BOOLEAN)  \
    (((Status) == STATUS_DEVICE_NOT_READY) ||      \
     ((Status) == STATUS_IO_TIMEOUT) ||            \
     ((Status) == STATUS_MEDIA_WRITE_PROTECTED) || \
     ((Status) == STATUS_NO_MEDIA_IN_DEVICE) ||    \
     ((Status) == STATUS_VERIFY_REQUIRED) ||       \
     ((Status) == STATUS_UNRECOGNIZED_MEDIA) ||    \
     ((Status) == STATUS_WRONG_VOLUME)))

// end_ntddk end_wdm end_nthal

//++
//
// BOOLEAN
// IoIsFileOpenedExclusively(
//     IN PFILE_OBJECT FileObject
//     )
//
// Routine Description:
//
//     This routine is invoked to determine whether the file open represented
//     by the specified file object is opened exclusively.
//
// Arguments:
//
//     FileObject - Pointer to the file object that represents the open instance
//         of the target file to be tested for exclusive access.
//
// Return Value:
//
//     The function value is TRUE if the open instance of the file is exclusive;
//     otherwise FALSE is returned.
//
//--

#define IoIsFileOpenedExclusively( FileObject ) (\
    (BOOLEAN) !((FileObject)->SharedRead || (FileObject)->SharedWrite || (FileObject)->SharedDelete))

VOID
IoLockUserBuffer(
    IN OUT PIRP Irp,
    IN ULONG Length
    );

// begin_ntddk begin_nthal

//  begin_wdm

NTKERNELAPI
VOID
IoMarkIrpMustComplete(
    IN OUT PIRP Irp
    );

//++
//
// VOID
// IoMarkIrpPending(
//     IN OUT PIRP Irp
//     )
//
// Routine Description:
//
//     This routine marks the specified I/O Request Packet (IRP) to indicate
//     that an initial status of STATUS_PENDING was returned to the caller.
//     This is used so that I/O completion can determine whether or not to
//     fully complete the I/O operation requested by the packet.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet to be marked pending.
//
// Return Value:
//
//     None.
//
//--

#define IoMarkIrpPending( Irp ) ( \
    IoGetCurrentIrpStackLocation( (Irp) )->Control |= SL_PENDING_RETURNED )

// end_ntddk end_wdm end_nthal end_ntifs

NTSTATUS
IoParseDevice(
    IN PVOID ParseObject,
    IN struct _OBJECT_TYPE *ObjectType,
    IN ULONG Attributes,
    IN OUT POBJECT_STRING CompleteName,
    IN OUT POBJECT_STRING RemainingName,
    IN OUT PVOID Context OPTIONAL,
    OUT PVOID *Object
    );

// begin_ntifs

NTKERNELAPI
NTSTATUS
IoQueryFileInformation(
    IN PFILE_OBJECT FileObject,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN ULONG Length,
    OUT PVOID FileInformation,
    OUT PULONG ReturnedLength
    );

NTKERNELAPI
NTSTATUS
IoQueryVolumeInformation(
    IN PFILE_OBJECT FileObject,
    IN FS_INFORMATION_CLASS FsInformationClass,
    IN ULONG Length,
    OUT PVOID FsInformation,
    OUT PULONG ReturnedLength
    );

// end_ntifs

NTKERNELAPI                                             // ntsrv
VOID                                                    // ntsrv
IoQueueThreadIrp(                                       // ntsrv
    IN PIRP Irp                                         // ntsrv
    );                                                  // ntsrv

// begin_ntddk begin_nthal begin_ntifs

NTKERNELAPI
VOID
IoRemoveShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    );

//  begin_wdm

//++
//
// VOID
// IoSetCompletionRoutine(
//     IN PIRP Irp,
//     IN PIO_COMPLETION_ROUTINE CompletionRoutine,
//     IN PVOID Context,
//     IN BOOLEAN InvokeOnSuccess,
//     IN BOOLEAN InvokeOnError,
//     IN BOOLEAN InvokeOnCancel
//     )
//
// Routine Description:
//
//     This routine is invoked to set the address of a completion routine which
//     is to be invoked when an I/O packet has been completed by a lower-level
//     driver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet itself.
//
//     CompletionRoutine - Address of the completion routine that is to be
//         invoked once the next level driver completes the packet.
//
//     Context - Specifies a context parameter to be passed to the completion
//         routine.
//
//     InvokeOnSuccess - Specifies that the completion routine is invoked when the
//         operation is successfully completed.
//
//     InvokeOnError - Specifies that the completion routine is invoked when the
//         operation completes with an error status.
//
//     InvokeOnCancel - Specifies that the completion routine is invoked when the
//         operation is being canceled.
//
// Return Value:
//
//     None.
//
//--

#define IoSetCompletionRoutine( Irp, Routine, CompletionContext, Success, Error, Cancel ) { \
    PIO_STACK_LOCATION irpSp;                                               \
    ASSERT( (Success) | (Error) | (Cancel) ? (Routine) != NULL : TRUE );    \
    irpSp = IoGetNextIrpStackLocation( (Irp) );                             \
    irpSp->CompletionRoutine = (Routine);                                   \
    irpSp->Context = (CompletionContext);                                   \
    irpSp->Control = 0;                                                     \
    if ((Success)) { irpSp->Control = SL_INVOKE_ON_SUCCESS; }               \
    if ((Error)) { irpSp->Control |= SL_INVOKE_ON_ERROR; }                  \
    if ((Cancel)) { irpSp->Control |= SL_INVOKE_ON_CANCEL; } }

//++
//
// VOID
// IoSetNextIrpStackLocation (
//     IN OUT PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to set the current IRP stack location to
//     the next stack location, i.e. it "pushes" the stack.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet (IRP).
//
// Return Value:
//
//     None.
//
//--

#define IoSetNextIrpStackLocation( Irp ) {      \
    (Irp)->CurrentLocation--;                   \
    (Irp)->Tail.Overlay.CurrentStackLocation--; }

//++
//
// VOID
// IoCopyCurrentIrpStackLocationToNext(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to copy the IRP stack arguments and file
//     pointer from the current IrpStackLocation to the next
//     in an I/O Request Packet (IRP).
//
//     If the caller wants to call IoCallDriver with a completion routine
//     but does not wish to change the arguments otherwise,
//     the caller first calls IoCopyCurrentIrpStackLocationToNext,
//     then IoSetCompletionRoutine, then IoCallDriver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     None.
//
//--

#define IoCopyCurrentIrpStackLocationToNext( Irp ) { \
    PIO_STACK_LOCATION irpSp; \
    PIO_STACK_LOCATION nextIrpSp; \
    irpSp = IoGetCurrentIrpStackLocation( (Irp) ); \
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) ); \
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine)); \
    nextIrpSp->Control = 0; }

//++
//
// VOID
// IoSkipCurrentIrpStackLocation (
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to increment the current stack location of
//     a given IRP.
//
//     If the caller wishes to call the next driver in a stack, and does not
//     wish to change the arguments, nor does he wish to set a completion
//     routine, then the caller first calls IoSkipCurrentIrpStackLocation
//     and the calls IoCallDriver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     None
//
//--

#define IoSkipCurrentIrpStackLocation( Irp ) \
    (Irp)->CurrentLocation++; \
    (Irp)->Tail.Overlay.CurrentStackLocation++;

NTKERNELAPI
VOID
IoSetShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    OUT PSHARE_ACCESS ShareAccess
    );

// end_ntddk end_wdm end_nthal end_ntifs

NTKERNELAPI
VOID
IoShutdownSystem(
    IN ULONG Phase
    );

// begin_ntddk begin_wdm begin_nthal begin_ntifs

//++
//
// USHORT
// IoSizeOfIrp(
//     IN CCHAR StackSize
//     )
//
// Routine Description:
//
//     Determines the size of an IRP given the number of stack locations
//     the IRP will have.
//
// Arguments:
//
//     StackSize - Number of stack locations for the IRP.
//
// Return Value:
//
//     Size in bytes of the IRP.
//
//--

#define IoSizeOfIrp( StackSize ) \
    ((USHORT) (sizeof( IRP ) + ((StackSize) * (sizeof( IO_STACK_LOCATION )))))

// end_ntifs


NTKERNELAPI
VOID
IoStartNextPacket(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
VOID
IoStartNextPacketByKey(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG Key
    );

NTKERNELAPI
VOID
IoStartPacket(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PULONG Key OPTIONAL
    );

// begin_ntifs

// end_ntddk end_wdm end_nthal

NTKERNELAPI
NTSTATUS
IoSynchronousDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    OUT PULONG ReturnedOutputBufferLength OPTIONAL,
    IN BOOLEAN InternalDeviceIoControl
    );

NTKERNELAPI
NTSTATUS
IoSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL
    );

// end_ntifs

NTKERNELAPI
NTSTATUS
IoSetIoCompletion (
    IN PVOID IoCompletion,
    IN PVOID KeyContext,
    IN PVOID ApcContext,
    IN NTSTATUS IoStatus,
    IN ULONG_PTR IoStatusInformation
    );

//
// Common file system support routines.
//

typedef struct _DIRECTORY_ENUM_CONTEXT {
    ULONG QueryOffset;
    OBJECT_STRING TemplateFileName;
} DIRECTORY_ENUM_CONTEXT, *PDIRECTORY_ENUM_CONTEXT;

NTSTATUS
IoCreateDirectoryEnumContext(
    IN POBJECT_STRING TemplateFileName,
    OUT PDIRECTORY_ENUM_CONTEXT *ReturnedDirectoryEnumContext
    );

BOOLEAN
IoIsNameInExpression(
    IN POBJECT_STRING TemplateFileName,
    IN POBJECT_STRING FileName
    );

extern ULONG IoPendingMustCompletePackets;

#endif // _IO_
