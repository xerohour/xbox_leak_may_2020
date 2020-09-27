/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    misc.c

Abstract:

    Miscellaneous routines

Author:

    Sakphong Chanbai (schanbai) 13-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"
#include <stdio.h>
#include "misc.h"


VOID
NTAPI
FlushDiskCache(
    VOID
    )

/*++

Routine Description:

    This routine will flush disk cache

Arguments:

    None

Return Value:

    No return value

--*/

{
    ULONG Volume, Disk;
    HANDLE Handle;
    NTSTATUS Status;
    OCHAR VolumeBuffer[MAX_PATH];
    OBJECT_STRING VolumeString;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;


    for ( Volume=1; Volume<6; Volume++ ) {

        soprintf( VolumeBuffer, OTEXT("\\Device\\Harddisk0\\Partition%d"), Volume );

        RtlInitObjectString( &VolumeString,  VolumeBuffer );

        InitializeObjectAttributes(
            &ObjA,
            &VolumeString,
            OBJ_CASE_INSENSITIVE,
            NULL,
            NULL
        );

        Status = NtCreateFile(
                    &Handle,
                    SYNCHRONIZE | GENERIC_READ,
                    &ObjA,
                    &IoStatusBlock,
                    0,
                    FILE_ATTRIBUTE_NORMAL,
                    FILE_SHARE_READ,
                    FILE_OPEN,
                    FILE_SYNCHRONOUS_IO_NONALERT
                 );

        if ( !NT_SUCCESS(Status) ) {
            continue;
        }

        Status = NtFlushBuffersFile( Handle, &IoStatusBlock );
        NtClose( Handle );
    }
}


VOID
NTAPI
RebootSystem(
    VOID
    )

/*++

Routine Description:

    This routine will flush disk cache and reboot the system

Arguments:

    None

Return Value:

    No return value

--*/

{
    FlushDiskCache();
    HalReturnToFirmware( HalRebootRoutine );
}


NTSTATUS
xxx_NtQueryPerformanceCounter (
    OUT PLARGE_INTEGER PerformanceCounter,
    OUT PLARGE_INTEGER PerformanceFrequency OPTIONAL
    )

/*++

Routine Description:

    This function returns current value of performance counter and,
    optionally, the frequency of the performance counter.

    Performance frequency is the frequency of the performance counter
    in Hertz, i.e., counts/second.  Note that this value is implementation
    dependent.  If the implementation does not have hardware to support
    performance timing, the value returned is 0.

Arguments:

    PerformanceCounter - supplies the address of a variable to receive
        the current Performance Counter value.

    PerformanceFrequency - Optionally, supplies the address of a
        variable to receive the performance counter frequency.

Return Value:

    STATUS_ACCESS_VIOLATION or STATUS_SUCCESS.

--*/

{
    *PerformanceCounter = KeQueryPerformanceCounter();
    if (ARGUMENT_PRESENT(PerformanceFrequency)) {
        *PerformanceFrequency = KeQueryPerformanceFrequency();
    }

    return STATUS_SUCCESS;
}


ULONG
NTAPI
xxx_RtlGetLongestNtPathLength(
    VOID
    )
{
    //
    // BUGBUG: RtlGetLongestNtPathLength is not available right now
    //         It might not available at all, for now, we can safely
    //         use the size of maximum DOS path plus "\??\UNC\"

    return sizeof( "\\??\\UNC\\" ) + DOS_MAX_PATH_LENGTH + 1;
}


BOOLEAN
NTAPI
xxx_RtlIsTextUnicode(
    IN PVOID Buffer,
    IN ULONG Size,
    IN OUT PULONG Result OPTIONAL
    )
{
    BOOLEAN b = FALSE;

    if ( Size >= 2 ) {

        ASSERT( Buffer != NULL );

        try {
            if ( *(PWSTR)Buffer == 0xFEFF) {
                b = TRUE;
            }
        } except(EXCEPTION_EXECUTE_HANDLER) {
            ;
        }
    }

    return b;
}


PVOID
AllocateKernelPool(
    SIZE_T Size
    )
{
    PVOID p = ExAllocatePoolWithTag( Size, XTESTLIB_POOLTAG );
    if ( p ) {
        RtlZeroMemory( p, Size );
    } else {
        KdPrint(( "XTESTLIB: ExAllocatePoolWithTag returned NULL, size = %lu\n", Size ));
    }
    return p;
}


VOID
FreeKernelPool(
    PVOID p
    )
{
    ExFreePool( p );
}


VOID
QueryCpuTimesInformation(
    PLARGE_INTEGER KernelTime,
    PLARGE_INTEGER IdleTime,
    PLARGE_INTEGER DpcTime OPTIONAL,
    PLARGE_INTEGER InterruptTime OPTIONAL,
    PLARGE_INTEGER UserTime OPTIONAL
    )
{
    PKPRCB Prcb = KeGetCurrentPrcb();

    KernelTime->QuadPart = UInt32x32To64(
                                KeQueryTickCount() - Prcb->IdleThread->KernelTime,
                                *KeTimeIncrement
                                );

    IdleTime->QuadPart = UInt32x32To64(
                            Prcb->IdleThread->KernelTime,
                            *KeTimeIncrement
                            );

    if ( ARGUMENT_PRESENT(DpcTime) ) {
        DpcTime->QuadPart = UInt32x32To64( Prcb->DpcTime, *KeTimeIncrement );
    }

    if ( ARGUMENT_PRESENT(InterruptTime) ) {
        InterruptTime->QuadPart = UInt32x32To64( Prcb->InterruptTime, *KeTimeIncrement );
    }

    if ( ARGUMENT_PRESENT(UserTime) ) {
        UserTime->QuadPart = 0;
    }
}
