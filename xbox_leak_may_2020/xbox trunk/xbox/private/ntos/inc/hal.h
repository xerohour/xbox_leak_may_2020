/*++ BUILD Version: 0011    // Increment this if a change has global effects

Copyright (c) 1991  Microsoft Corporation

Module Name:

    hal.h

Abstract:

    This header file defines the Hardware Architecture Layer (HAL) interfaces
    that are exported by a system vendor to the NT system.

Author:

    David N. Cutler (davec) 25-Apr-1991


Revision History:

--*/

#ifndef _HAL_
#define _HAL_

VOID
HalInitSystemPhase0(
    VOID
    );

VOID
HalInitSystemPhase0Quick(
    VOID
    );

VOID
HalInitSystemPhase1(
    VOID
    );

VOID
HalPulseHardwareMonitorPin(
    VOID
    );

//
// Non maskable interrupt routine.
//

VOID
HalHandleNMI(
    VOID
    );

//
// Profiling functions.
//

NTHALAPI
ULONG_PTR
HalSetProfileInterval(
    IN ULONG Interval
    );

NTHALAPI
VOID
HalStartProfileInterrupt(
    ULONG unused
    );

NTHALAPI
VOID
HalStopProfileInterrupt(
    ULONG unused
    );

//
// Timer and interrupt functions.
//

NTHALAPI
BOOLEAN
HalQueryRealTimeClock(
    OUT PTIME_FIELDS TimeFields
    );

NTHALAPI
BOOLEAN
HalSetRealTimeClock(
    IN PTIME_FIELDS TimeFields
    );

NTHALAPI
VOID
FASTCALL
HalRequestSoftwareInterrupt(
    KIRQL RequestIrql
    );

//
// CMOS validity functions.
//

BOOLEAN
HalIsCmosValid(
    VOID
    );

VOID
HalMarkCmosValid(
    VOID
    );

//
// Firmware interface functions.
//

DECLSPEC_NORETURN
NTHALAPI
VOID
HalReturnToFirmware(
    IN FIRMWARE_REENTRY Routine
    );

//
// Shutdown notification support.
//

typedef
VOID
(*PHAL_SHUTDOWN_NOTIFICATION)(
    IN struct _HAL_SHUTDOWN_REGISTRATION *ShutdownRegistration
    );

typedef struct _HAL_SHUTDOWN_REGISTRATION {
    PHAL_SHUTDOWN_NOTIFICATION NotificationRoutine;
    LONG Priority;
    LIST_ENTRY ListEntry;
} HAL_SHUTDOWN_REGISTRATION, *PHAL_SHUTDOWN_REGISTRATION;

NTHALAPI
VOID
HalRegisterShutdownNotification(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
    IN BOOLEAN Register
    );

NTHALAPI
BOOLEAN
HalIsResetOrShutdownPending(
    VOID
    );

VOID
HalMustCompletePacketsFinished(
    VOID
    );

NTHALAPI
VOID
HalInitiateShutdown(
    VOID
    );

//
// System interrupts functions.
//

NTHALAPI
VOID
HalDisableSystemInterrupt(
    IN ULONG BusInterruptLevel
    );

NTHALAPI
VOID
HalEnableSystemInterrupt(
    IN ULONG BusInterruptLevel,
    IN KINTERRUPT_MODE InterruptMode
    );

// begin_ntddk
//
// I/O driver configuration functions.
//

NTHALAPI
VOID
HalReadWritePCISpace(
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN ULONG RegisterNumber,
    IN PVOID Buffer,
    IN ULONG Length,
    IN BOOLEAN WritePCISpace
    );

#define HalReadPCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length) \
    HalReadWritePCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length, FALSE)

#define HalWritePCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length) \
    HalReadWritePCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length, TRUE)

NTHALAPI
ULONG
HalGetInterruptVector(
    IN ULONG BusInterruptLevel,
    OUT PKIRQL Irql
    );

#include <pshpack1.h>

typedef struct _PCI_DEVICE_ADDRESS {
    UCHAR     Type;  //CmResourceType
    UCHAR     Valid;
    UCHAR     Reserved[2];

    union {

        struct {
            PVOID TranslatedAddress;
            ULONG Length;
        } Port;

        struct {
            PVOID TranslatedAddress;
            ULONG Length;
        } Memory;

    } u;

} PCI_DEVICE_ADDRESS, *PPCI_DEVICE_ADDRESS;

typedef struct _PCI_DEVICE_DESCRIPTOR {
    ULONG     Bus;
    ULONG     Slot;
    USHORT    VendorID;
    USHORT    DeviceID;

    UCHAR     BaseClass;
    UCHAR     SubClass;
    UCHAR     ProgIf;
    UCHAR     Reserved[1];

    struct {

        PCI_DEVICE_ADDRESS Address[1];

        struct {
            ULONG Vector;
            ULONG Affinity;
            KIRQL Irql;         // KIRQL is uchar!

        } Interrupt;

    } ResourceData;

} PCI_DEVICE_DESCRIPTOR, *PPCI_DEVICE_DESCRIPTOR;

#include <poppack.h>

// end_ntddk

//
// The following function prototypes are for HAL routines with a prefix of Ke.
//
// begin_ntddk begin_wdm
//
// Performance counter function.
//

NTHALAPI
LARGE_INTEGER
KeQueryPerformanceCounter(
    VOID
    );

NTHALAPI
LARGE_INTEGER
KeQueryPerformanceFrequency(
    VOID
    );

//
// Stall processor execution function.
//

NTHALAPI
VOID
KeStallExecutionProcessor(
    IN ULONG MicroSeconds
    );

// end_wdm

DECLSPEC_NORETURN
NTHALAPI
VOID
HalHaltSystem(
    VOID
    );

// end_ntddk

//
// SMBus functions.
//

NTHALAPI
NTSTATUS
HalReadSMBusValue(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN ReadWordValue,
    OUT ULONG *DataValue
    );

#define HalReadSMBusByte(SlaveAddress, CommandCode, DataValue) \
    HalReadSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)

#define HalReadSMBusWord(SlaveAddress, CommandCode, DataValue) \
    HalReadSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)

NTHALAPI
NTSTATUS
HalWriteSMBusValue(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN WriteWordValue,
    IN ULONG DataValue
    );

#define HalWriteSMBusByte(SlaveAddress, CommandCode, DataValue) \
    HalWriteSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)

#define HalWriteSMBusWord(SlaveAddress, CommandCode, DataValue) \
    HalWriteSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)

VOID
HalWriteSMCLEDStates(
    IN ULONG LEDStates
    );

NTHALAPI
NTSTATUS
HalWriteSMCScratchRegister(
    IN ULONG ScratchRegister
    );

//
// Super I/O controller functions.
//

BOOLEAN
HalInitializeSuperIo(
    VOID
    );

//
// DVD tray functions.
//

NTHALAPI
NTSTATUS
HalReadSMCTrayState(
    OUT PULONG TrayState,
    OUT PULONG TrayStateChangeCount OPTIONAL
    );

BOOLEAN
HalEnableTrayEjectRequiresReboot(
    IN BOOLEAN EnterNonSecureMode
    );

NTHALAPI
VOID
HalEnableSecureTrayEject(
    VOID
    );

//
// AV pack functions.
//

VOID
HalBlockIfNoAVPack(
    VOID
    );

#if !defined(_NTSYSTEM_)
extern PULONG HalDiskCachePartitionCount;
extern PSTRING HalDiskModelNumber;
extern PSTRING HalDiskSerialNumber;
extern PULONG HalBootSMCVideoMode;
#else
extern ULONG HalDiskCachePartitionCount;
extern STRING HalDiskModelNumber;
extern STRING HalDiskSerialNumber;
extern ULONG HalBootSMCVideoMode;
#endif

#endif // _HAL_
