/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    create.c

Abstract

    This module contains the code to implement the NtCreateFile
    and NtOpenFile system services.

Author:

    Darryl E. Havens (darrylh) 14-Apr-1989

Environment:

    Kernel mode

Revision History:


--*/

#include "iop.h"

NTSTATUS
NtCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions
    )

/*++

Routine Description:

    This service opens or creates a file, or opens a device.  It is used to
    establish a file handle to the open device/file that can then be used
    in subsequent operations to perform I/O operations on.  For purposes of
    readability, files and devices are treated as "files" throughout the
    majority of this module and the system service portion of the I/O system.
    The only time a distinction is made is when it is important to determine
    which is really being accessed.  Then a distinction is also made in the
    comments.

Arguments:

    FileHandle - A pointer to a variable to receive the handle to the open file.

    DesiredAccess - Supplies the types of access that the caller would like to
        the file.

    ObjectAttributes - Supplies the attributes to be used for file object (name,
        SECURITY_DESCRIPTOR, etc.)

    IoStatusBlock - Specifies the address of the caller's I/O status block.

    AllocationSize - Initial size that should be allocated to the file.  This
        parameter only has an affect if the file is created.  Further, if
        not specified, then it is taken to mean zero.

    FileAttributes - Specifies the attributes that should be set on the file,
        if it is created.

    ShareAccess - Supplies the types of share access that the caller would like
        to the file.

    CreateDisposition - Supplies the method for handling the create/open.

    CreateOptions - Caller options for how to perform the create/open.

Return Value:

    The function value is the final status of the create/open operation.

--*/

{
    //
    // Simply invoke the common I/O file creation routine to do the work.
    //

    PAGED_CODE();

    return IoCreateFile( FileHandle,
                         DesiredAccess,
                         ObjectAttributes,
                         IoStatusBlock,
                         AllocationSize,
                         FileAttributes,
                         ShareAccess,
                         CreateDisposition,
                         CreateOptions,
                         0 );
}

NTSTATUS
NtOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
    )

/*++

Routine Description:

    This service opens a file or a device.  It is used to establish a file
    handle to the open device/file that can then be used in subsequent
    operations to perform I/O operations on.

Arguments:

    FileHandle - A pointer to a variable to receive the handle to the open file.

    DesiredAccess - Supplies the types of access that the caller would like to
        the file.

    ObjectAttributes - Supplies the attributes to be used for file object (name,
        SECURITY_DESCRIPTOR, etc.)

    IoStatusBlock - Specifies the address of the caller's I/O status block.

    ShareAccess - Supplies the types of share access that the caller would like
        to the file.

    OpenOptions - Caller options for how to perform the open.

Return Value:

    The function value is the final completion status of the open/create
    operation.

--*/

{
    //
    // Simply invoke the common I/O file creation routine to perform the work.
    //

    PAGED_CODE();

    return IoCreateFile( FileHandle,
                         DesiredAccess,
                         ObjectAttributes,
                         IoStatusBlock,
                         (PLARGE_INTEGER) NULL,
                         0L,
                         ShareAccess,
                         FILE_OPEN,
                         OpenOptions,
                         0 );
}
