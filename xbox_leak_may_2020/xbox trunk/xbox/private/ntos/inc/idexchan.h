/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    idexchan.h

Abstract:

    This module contains the public definition of the IDEX channel object.

--*/

#ifndef _IDEXCHAN_
#define _IDEXCHAN_

#include <pshpack4.h>

//
// Function prototype for interrupt service routines.
//

typedef
VOID
(*PIDE_INTERRUPT_ROUTINE)(
    VOID
    );

//
// Function prototype for channel post interrupt or timeout DPC routines.
//

typedef
VOID
(*PIDE_FINISHIO_ROUTINE)(
    VOID
    );

//
// Function prototype for routines to poll if a device reset has completed.
//

typedef
BOOLEAN
(*PIDE_POLL_RESET_COMPLETE_ROUTINE)(
    VOID
    );

//
// Function prototype for timeout expired routines.
//

typedef
VOID
(*PIDE_TIMEOUT_EXPIRED_ROUTINE)(
    VOID
    );

//
// Function prototype for reset device routines.
//

typedef
VOID
(*PIDE_RESET_DEVICE_ROUTINE)(
    VOID
    );

//
// Function prototype for starting or queueing the supplied I/O request packet.
//

typedef
VOID
(*PIDE_START_PACKET_ROUTINE)(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Function prototype for starting the next queued I/O request packet.
//

typedef
VOID
(*PIDE_START_NEXT_PACKET_ROUTINE)(
    VOID
    );

//
// Channel object.
//

typedef struct _IDE_CHANNEL_OBJECT {
    PIDE_INTERRUPT_ROUTINE InterruptRoutine;
    PIDE_FINISHIO_ROUTINE FinishIoRoutine;
    PIDE_POLL_RESET_COMPLETE_ROUTINE PollResetCompleteRoutine;
    PIDE_TIMEOUT_EXPIRED_ROUTINE TimeoutExpiredRoutine;
    PIDE_START_PACKET_ROUTINE StartPacketRoutine;
    PIDE_START_NEXT_PACKET_ROUTINE StartNextPacketRoutine;
    KIRQL InterruptIrql;
    BOOLEAN ExpectingBusMasterInterrupt;
    BOOLEAN StartPacketBusy;
    BOOLEAN StartPacketRequested;
    UCHAR Timeout;
    UCHAR IoRetries;
    UCHAR MaximumIoRetries;
    PIRP CurrentIrp;
    KDEVICE_QUEUE DeviceQueue;
    ULONG PhysicalRegionDescriptorTablePhysical;
    KDPC TimerDpc;
    KDPC FinishDpc;
    KTIMER Timer;
    KINTERRUPT InterruptObject;
} IDE_CHANNEL_OBJECT, *PIDE_CHANNEL_OBJECT;

//
// External symbols.
//

#if !defined(_NTSYSTEM_)
extern IDE_CHANNEL_OBJECT *IdexChannelObject;
#else
extern IDE_CHANNEL_OBJECT IdexChannelObject;
#endif

#include <poppack.h>

#endif  // IDEXCHAN
