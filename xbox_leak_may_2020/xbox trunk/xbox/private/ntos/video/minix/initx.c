/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    initx.c

Abstract:

    This module contains the code to initialize the video hardware

--*/


#include "minixp.h"


//
// Global variable to hold driver extension
//

PMINIX_DRIVER_EXTENSION MxDriverExtension;


//
// Local functions
//

BOOLEAN
MxPortInterrupt(
    IN PKINTERRUPT InterruptObject,
    IN PVOID ServiceContext
    );

VOID
MxDpcDispatcher(
    IN PKDPC Dpc,
    IN PVOID HwDeviceExtension,
    IN PVOID DpcRoutine,
    IN PVOID Context
    );


//
// Functions defined else where
//

VP_STATUS
NVFindAdapter(
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    );

BOOLEAN
NVInitialize(
    PVOID HwDeviceExtension
    );

BOOLEAN
NVStartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

BOOLEAN
NVInterrupt(
    PVOID HwDeviceExtension
    );

BOOL    
RmInitRm(
    VOID
    );


LONG
MxSetVideoMode(
    IN PMINIX_MODE_INFO ModeInfo
    );


LONG
MxAllocContextDma(
    IN ULONG Dma,
    IN ULONG Class,
    IN ULONG Flags,
    IN PVOID Base,
    IN ULONG Limit
    );




VOID
MxDpcDispatcher(
    IN PKDPC Dpc,
    IN PVOID HwDeviceExtension,
    IN PVOID DpcRoutine,
    IN PVOID Context
    )

/*++

Routine Description:

    This routine handles DPCs and forwards them to the miniport callback
    routine.

Arguments:

    Dpc - The DPC which is executing.

    HwDeviceExtension - The HwDeviceExtension for the device which scheduled
        the DPC.

    DpcRoutine - The callback in the miniport which needs to be called.

    Context - The miniport supplied context.

Returns:

    None.

--*/

{
    ((PMINIPORT_DPC_ROUTINE)(ULONG_PTR)DpcRoutine)(HwDeviceExtension, Context);
}



BOOLEAN
MxPortInterrupt(
    IN PKINTERRUPT InterruptObject,
    IN PVOID ServiceContext
    )
{
    BOOLEAN Handled;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PHW_DEVICE_EXTENSION HwDeviceExtension;

    VideoDeviceExtension = &(MxDriverExtension->VideoExtension);
    HwDeviceExtension = &(MxDriverExtension->HwExtension);

    if (VideoDeviceExtension->InterruptsEnabled) {
        Handled = VideoDeviceExtension->HwInterrupt(HwDeviceExtension);
    } else {
        Handled = FALSE;
    }

    return Handled;
}





LONG
MxInitSystem(
    IN PVOID Argument1
    )
/*++

Routine Description:

    This routine initializes the I/O system.

Arguments:

    Argument1 - Supplies a pointer to the driver object

Return Value:

    Status of the operation

--*/
{
    NTSTATUS status;
    SYSTEM_BASIC_INFORMATION BasicInformation;
    PCI_SLOT_NUMBER SlotNumber;
    PCI_COMMON_CONFIG Configuration;
    VP_STATUS vpstatus;
    VIDEO_PORT_CONFIG_INFO ConfigInfo;
    UCHAR Again;
    ULONG InterruptVector;
    KIRQL InterruptIrql;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PHW_DEVICE_EXTENSION HwDeviceExtension;
    PDRIVER_OBJECT DriverObject;

    DriverObject = (PDRIVER_OBJECT)Argument1;

    //
    // Initialize the Driver Object 
    //
    
    DriverObject->MajorFunction[IRP_MJ_MINIX_CREATE] = (PDRIVER_DISPATCH)MxCreatePort;
    DriverObject->MajorFunction[IRP_MJ_MINIX_SETVIDEOMODE] = (PDRIVER_DISPATCH)MxSetVideoMode;
    DriverObject->MajorFunction[IRP_MJ_MINIX_ALLOCCONTEXTDMA] = (PDRIVER_DISPATCH)MxAllocContextDma;

    MxDriverExtension = ExAllocatePoolWithTag(NonPagedPool, sizeof(MINIX_DRIVER_EXTENSION), 'rDxM');

    if (MxDriverExtension == NULL) {
        return (LONG)STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(MxDriverExtension, sizeof(MINIX_DRIVER_EXTENSION));

    VideoDeviceExtension = &(MxDriverExtension->VideoExtension);
    HwDeviceExtension = &(MxDriverExtension->HwExtension);
    
    //
    // Figure out how much physical memory is in the system.
    //

    status = NtQuerySystemInformation(SystemBasicInformation, &BasicInformation,
        sizeof(SYSTEM_BASIC_INFORMATION), NULL);

    if (!NT_SUCCESS(status)) {
        return (LONG)status;
    }

    //
    // Read the PCI configuration data for the adapter attached to the AGP440 bus.
    //

    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = AGP440_TARGET_DEVICE_NUMBER;
    SlotNumber.u.bits.FunctionNumber = AGP440_TARGET_FUNCTION_NUMBER;

    if (HalGetBusData(PCIConfiguration, 1, SlotNumber.u.AsULONG, &Configuration,
        sizeof(PCI_COMMON_CONFIG)) != sizeof(PCI_COMMON_CONFIG)) {
        return (LONG)STATUS_DEVICE_DOES_NOT_EXIST;
    }

    //
    // Verify that this looks something like an NVidia display controller and
    // that the configuration is in the state that we expect.
    //

    ASSERT(Configuration.BaseClass == PCI_CLASS_DISPLAY_CTLR);
    ASSERT(Configuration.VendorID == 0x10DE);
    ASSERT(VideoIsFlagSet(Configuration.Command, PCI_ENABLE_IO_SPACE));
    ASSERT(VideoIsFlagSet(Configuration.Command, PCI_ENABLE_BUS_MASTER));

    //
    // Initialize the video device extension.
    //

    VideoDeviceExtension->HwFindAdapter = NVFindAdapter;
    VideoDeviceExtension->HwInitialize = NVInitialize;
    VideoDeviceExtension->HwInterrupt = NVInterrupt;
    VideoDeviceExtension->HwStartIO = NVStartIO;

    RtlCopyMemory(VideoDeviceExtension->BaseAddresses, Configuration.u.type0.BaseAddresses,
        sizeof(ULONG) * PCI_TYPE0_ADDRESSES);

    KeInitializeDpc(&VideoDeviceExtension->Dpc, MxDpcDispatcher, HwDeviceExtension);  

    KeInitializeMutex(&VideoDeviceExtension->SyncMutex, 0);

    if (!AgpUmaEnable()) {
        return (LONG)STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Invoke the miniport's HwFindAdapter routine.
    //

    RtlZeroMemory(&ConfigInfo, sizeof(VIDEO_PORT_CONFIG_INFO));

    ConfigInfo.Length = sizeof(VIDEO_PORT_CONFIG_INFO);
    ConfigInfo.BusInterruptLevel = 0xFF;
    ConfigInfo.BusInterruptVector = 0xFF;
    ConfigInfo.SystemMemorySize = (ULONGLONG)BasicInformation.NumberOfPhysicalPages << PAGE_SHIFT;

    vpstatus = VideoDeviceExtension->HwFindAdapter(HwDeviceExtension, NULL,
        NULL, &ConfigInfo, &Again);

    if (vpstatus != NO_ERROR) {
        return (LONG)STATUS_DEVICE_DOES_NOT_EXIST;
    }

    //
    // Connect to the device's interrupt.
    //

    VideoDeviceExtension->InterruptsEnabled = TRUE;

    InterruptVector = HalGetInterruptVector(PCIBus, 0,
        Configuration.u.type0.InterruptLine, Configuration.u.type0.InterruptLine,
        &InterruptIrql);

    status = IoConnectInterrupt(&(VideoDeviceExtension->InterruptObject),
        MxPortInterrupt, VideoDeviceExtension, InterruptVector,
        InterruptIrql, InterruptIrql, LevelSensitive, TRUE);

    if (!NT_SUCCESS(status)) {
        return (LONG)status;
    }
                         
    //
    // Initialize resource manager
    //
    if (!RmInitRm()) {
        return (LONG)STATUS_UNSUCCESSFUL;
    }


    return (LONG)STATUS_SUCCESS;
}



LONG
MxCreatePort(
    VOID
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PHW_DEVICE_EXTENSION HwDeviceExtension;

    VideoDeviceExtension = &(MxDriverExtension->VideoExtension);
    HwDeviceExtension = &(MxDriverExtension->HwExtension);

    if (VideoDeviceExtension->DeviceOpened) {
        // 
        // Calling this function more than once has no effect
        //
        return (LONG)STATUS_SUCCESS;
    }

    //
    // Invoke the miniport's HwInitialize routine.
    //

    if (!VideoDeviceExtension->HwInitialize(HwDeviceExtension)) {
        return (LONG)STATUS_UNSUCCESSFUL;
    }

    VideoDeviceExtension->DeviceOpened = TRUE;
    
    return (LONG)STATUS_SUCCESS;
}


