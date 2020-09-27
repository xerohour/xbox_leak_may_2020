/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    videoprt.c

Abstract:

    This module implements a lightweight implementation of a video port driver.

--*/

#include "videoprt.h"

#if 1 // NUGOOP

extern BOOLEAN AgpUmaEnable();

#endif

PVIDEO_DEVICE_EXTENSION VideoGlobalDeviceExtension = NULL;

VOID
DriverEntry(
    IN PDRIVER_OBJECT DriverObject
    )
{
    //
    // We're actually an export library, not a real driver, so the DriverEntry
    // will never be called.  The routine only exists to satisfy the build
    // process.
    //

    ASSERT(FALSE);
}

ULONG VideoDebugLevel = 0;

VOID
VideoPortDebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    )

/*++

Routine Description:

    This routine allows the miniport drivers (as well as the port driver) to
    display error messages to the debug port when running in the debug
    environment.

    When running a non-debugged system, all references to this call are
    eliminated by the compiler.

Arguments:

    DebugPrintLevel - Debug print level between 0 and 3, with 3 being the
        most verbose.

Return Value:

    None.

--*/

{
    va_list ap;

    va_start(ap, DebugMessage);

    if (DebugPrintLevel <= VideoDebugLevel) {

        char buffer[256];

        vsprintf(buffer, DebugMessage, ap);

        DbgPrint(buffer);
    }

    va_end(ap);

} // VideoPortDebugPrint()

#if DBG

#define pVideoDebugPrint(arg) VideoPortDebugPrint arg

#else

#define pVideoDebugPrint(arg)

#endif

VOID
pVideoPortMapToNtStatus(
    IN PSTATUS_BLOCK StatusBlock
    )

/*++

Routine Description:

    This function maps a Win32 error code to an NT error code, making sure
    the inverse translation will map back to the original status code.

Arguments:

    StatusBlock - Pointer to the status block

Return Value:

    None.

--*/

{
    PNTSTATUS status = &StatusBlock->Status;

    switch (*status) {

    case ERROR_INVALID_FUNCTION:
        *status = STATUS_NOT_IMPLEMENTED;
        break;

    case ERROR_NOT_ENOUGH_MEMORY:
        *status = STATUS_INSUFFICIENT_RESOURCES;
        break;

    case ERROR_INVALID_PARAMETER:
        *status = STATUS_INVALID_PARAMETER;
        break;

    case ERROR_INSUFFICIENT_BUFFER:
        *status = STATUS_BUFFER_TOO_SMALL;

        //
        // Make sure we zero out the information block if we get an
        // insufficient buffer.
        //

        StatusBlock->Information = 0;
        break;

    case ERROR_MORE_DATA:
        *status = STATUS_BUFFER_OVERFLOW;
        break;

    case ERROR_DEV_NOT_EXIST:
        *status = STATUS_DEVICE_DOES_NOT_EXIST;
        break;

    case ERROR_IO_PENDING:
        ASSERT(FALSE);
        // Fall through.

    case NO_ERROR:
        *status = STATUS_SUCCESS;
        break;

    default:

        ASSERT(FALSE);

        //
        // Since the driver did not see fit to follow the
        // rules about returning correct error codes. Videoprt will do it for
        // them.
        //

        *status = STATUS_UNSUCCESSFUL;

        break;

    }

    return;

} // end pVideoPortMapToNtStatus()

VOID
pVideoPortDpcDispatcher(
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
pVideoPortSynchronizeExecution(
    PVOID HwDeviceExtension,
    VIDEO_SYNCHRONIZE_PRIORITY Priority,
    PMINIPORT_SYNCHRONIZE_ROUTINE SynchronizeRoutine,
    PVOID Context
    )

/*++

Routine Description:

    VideoPortSynchronizeExecution synchronizes the execution of a miniport
    driver function in the following manner:

        - If Priority is equal to VpLowPriority, the current thread is
          raised to the highest non-interrupt-masking priority.  In
          other words, the current thread can only be pre-empted by an ISR.

        - If Priority is equal to VpMediumPriority and there is an
          ISR associated with the video device, then the function specified
          by SynchronizeRoutine is synchronized with the ISR.

          If no ISR is connected, synchronization is made at VpHighPriority
          level.

        - If Priority is equal to VpHighPriority, the current IRQL is
          raised to HIGH_LEVEL, which effectively masks out ALL interrupts
          in the system. This should be done sparingly and for very short
          periods -- it will completely freeze up the entire system.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    Priority - Specifies the type of priority at which the SynchronizeRoutine
        must be executed (found in VIDEO_SYNCHRONIZE_PRIORITY).

    SynchronizeRoutine - Points to the miniport driver function to be
        synchronized.

    Context - Specifies a context parameter to be passed to the miniport's
        SynchronizeRoutine.

Return Value:

    This function returns TRUE if the operation is successful.  Otherwise, it
    returns FALSE.

--*/

{
    BOOLEAN status;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    KIRQL oldIrql;

    VideoDeviceExtension = VIDEO_GET_VIDEO_DEVICE_EXTENSION(HwDeviceExtension);

    //
    // Switch on which type of priority.
    //

    switch (Priority) {

    case VpMediumPriority:

        //
        // This is synchronized with the interrupt object
        //

        if (VideoDeviceExtension->InterruptObject) {

            status = KeSynchronizeExecution(VideoDeviceExtension->InterruptObject,
                                            (PKSYNCHRONIZE_ROUTINE)
                                            SynchronizeRoutine,
                                            Context);

            ASSERT (status == TRUE);

            return status;
        }

        //
        // Fall through for Medium Priority
        //

    case VpLowPriority:

        //
        // Just normal level
        //

        status = SynchronizeRoutine(Context);

        return status;

    case VpHighPriority:

        //
        // This is like cli\sti where we mask out everything.
        //

        //
        // Get the current IRQL to catch re-entrant routines into synchronize.
        //

        oldIrql = KeGetCurrentIrql();

        if (oldIrql < POWER_LEVEL - 1) {

            KeRaiseIrql(POWER_LEVEL, &oldIrql);

        }

        status = SynchronizeRoutine(Context);

        if (oldIrql < POWER_LEVEL - 1) {

            KeLowerIrql(oldIrql);

        }

        return status;

        break;

    default:

        return FALSE;

    }
}

BOOLEAN
VideoPortResetDisplay(
    IN ULONG Columns,
    IN ULONG Rows
    )
{
    PVOID HwDeviceExtension;

    if (VideoGlobalDeviceExtension == NULL ||
        VideoGlobalDeviceExtension->HwResetHw == NULL) {
        return FALSE;
    }

    HwDeviceExtension = VIDEO_GET_HW_DEVICE_EXTENSION(VideoGlobalDeviceExtension);

    return VideoGlobalDeviceExtension->HwResetHw(HwDeviceExtension, Columns, Rows);
}

BOOLEAN
VideoPortInterrupt(
    IN PKINTERRUPT InterruptObject,
    IN PVOID ServiceContext
    )
{
    BOOLEAN Handled;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PVOID HwDeviceExtension;

    VideoDeviceExtension = (PVIDEO_DEVICE_EXTENSION)ServiceContext;
    HwDeviceExtension = VIDEO_GET_HW_DEVICE_EXTENSION(VideoDeviceExtension);

    if (VideoDeviceExtension->InterruptsEnabled) {
        Handled = VideoDeviceExtension->HwInterrupt(HwDeviceExtension);
    } else {
        Handled = FALSE;
    }

    return Handled;
}

NTSTATUS
VideoPortCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PVOID HwDeviceExtension;

    VideoDeviceExtension = (PVIDEO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    HwDeviceExtension = VIDEO_GET_HW_DEVICE_EXTENSION(VideoDeviceExtension);

    if (!VideoDeviceExtension->DeviceOpened) {

        //
        // Invoke the miniport's HwInitialize routine.
        //

        if (VideoDeviceExtension->HwInitialize(HwDeviceExtension)) {

            VideoDeviceExtension->DeviceOpened = TRUE;

            status = STATUS_SUCCESS;

        } else {
            status = STATUS_DEVICE_CONFIGURATION_ERROR;
        }

    } else {
        status = STATUS_ACCESS_DENIED;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
VideoPortDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PIO_STACK_LOCATION IrpSp;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PVOID HwDeviceExtension;
    PSTATUS_BLOCK StatusBlock;
    VIDEO_REQUEST_PACKET vrp;

    VideoDeviceExtension = (PVIDEO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    HwDeviceExtension = VIDEO_GET_HW_DEVICE_EXTENSION(VideoDeviceExtension);

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    StatusBlock = (PSTATUS_BLOCK)&Irp->IoStatus;

    vrp.IoControlCode = IrpSp->Parameters.DeviceIoControl.IoControlCode;
    vrp.StatusBlock = StatusBlock;
    vrp.InputBuffer = IrpSp->Parameters.DeviceIoControl.InputBuffer;
    vrp.InputBufferLength = IrpSp->Parameters.DeviceIoControl.InputBufferLength;
    vrp.OutputBuffer = Irp->UserBuffer;
    vrp.OutputBufferLength = IrpSp->Parameters.DeviceIoControl.OutputBufferLength;

    ACQUIRE_DEVICE_LOCK(VideoDeviceExtension);

    VideoDeviceExtension->HwStartIO(HwDeviceExtension, &vrp);

    RELEASE_DEVICE_LOCK(VideoDeviceExtension);

    if (StatusBlock->Status != NO_ERROR) {
        pVideoPortMapToNtStatus(StatusBlock);
    }

    status = Irp->IoStatus.Status;

    if (status == STATUS_PENDING) {
        return STATUS_PENDING;
    }

    IoCompleteRequest(Irp, IO_VIDEO_INCREMENT);

    return status;
}

NTSTATUS
VideoPortClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PVOID HwDeviceExtension;
    PSTATUS_BLOCK StatusBlock;
    VIDEO_REQUEST_PACKET vrp;

    VideoDeviceExtension = (PVIDEO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    HwDeviceExtension = VIDEO_GET_HW_DEVICE_EXTENSION(VideoDeviceExtension);

    StatusBlock = (PSTATUS_BLOCK)&Irp->IoStatus;

    vrp.IoControlCode = IOCTL_VIDEO_RESET_DEVICE;
    vrp.StatusBlock = StatusBlock;
    vrp.InputBuffer = NULL;
    vrp.InputBufferLength = 0;
    vrp.OutputBuffer = NULL;
    vrp.OutputBufferLength = 0;

    VideoDeviceExtension->HwStartIO(HwDeviceExtension, &vrp);

    VideoDeviceExtension->DeviceOpened = FALSE;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_VIDEO_INCREMENT);

    return STATUS_SUCCESS;
}

ULONG
VideoPortInitialize(
    IN PVOID Argument1,
    IN PVOID Argument2,
    IN PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    IN PVOID HwContext
    )
{
    NTSTATUS status;
    PDRIVER_OBJECT DriverObject;
    SYSTEM_BASIC_INFORMATION BasicInformation;
    PCI_SLOT_NUMBER SlotNumber;
    PCI_COMMON_CONFIG Configuration;
    OBJECT_STRING ObjectName;
    PDEVICE_OBJECT DeviceObject;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    PVOID HwDeviceExtension;
    PHYSICAL_ADDRESS ApertureBase;
    ULONG ApertureSizeInPages;
    VP_STATUS vpstatus;
    VIDEO_PORT_CONFIG_INFO ConfigInfo;
    UCHAR Again;
    ULONG InterruptVector;
    KIRQL InterruptIrql;

    DriverObject = (PDRIVER_OBJECT)Argument1;

    //
    // Fill in the driver object's dispatch routines.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE] = VideoPortCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = VideoPortDeviceControl;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = VideoPortClose;

    //
    // Figure out how much physical memory is in the system.
    //

    status = NtQuerySystemInformation(SystemBasicInformation, &BasicInformation,
        sizeof(SYSTEM_BASIC_INFORMATION), NULL);

    if (!NT_SUCCESS(status)) {
        return (ULONG)status;
    }

    //
    // Read the PCI configuration data for the adapter attached to the AGP440
    // bus.
    //

    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = AGP440_TARGET_DEVICE_NUMBER;
    SlotNumber.u.bits.FunctionNumber = AGP440_TARGET_FUNCTION_NUMBER;

    if (HalGetBusData(PCIConfiguration, 1, SlotNumber.u.AsULONG, &Configuration,
        sizeof(PCI_COMMON_CONFIG)) != sizeof(PCI_COMMON_CONFIG)) {
        return (ULONG)STATUS_DEVICE_DOES_NOT_EXIST;
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
    // Create the device object.
    //

    RtlInitObjectString(&ObjectName, OTEXT("\\Device\\Video0"));

    status = IoCreateDevice(DriverObject, sizeof(VIDEO_DEVICE_EXTENSION) +
        HwInitializationData->HwDeviceExtensionSize, &ObjectName,
        FILE_DEVICE_VIDEO, FILE_DEVICE_SECURE_OPEN, TRUE, &DeviceObject);

    if (!NT_SUCCESS(status)) {
        return (ULONG)status;
    }

    //
    // Initialize the device extension.
    //

    VideoDeviceExtension = (PVIDEO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    HwDeviceExtension = VIDEO_GET_HW_DEVICE_EXTENSION(VideoDeviceExtension);

    RtlZeroMemory(VideoDeviceExtension, sizeof(VIDEO_DEVICE_EXTENSION) +
        HwInitializationData->HwDeviceExtensionSize);

    VideoDeviceExtension->HwFindAdapter = HwInitializationData->HwFindAdapter;
    VideoDeviceExtension->HwInitialize = HwInitializationData->HwInitialize;
    VideoDeviceExtension->HwInterrupt = HwInitializationData->HwInterrupt;
    VideoDeviceExtension->HwStartIO = HwInitializationData->HwStartIO;
    VideoDeviceExtension->HwResetHw = HwInitializationData->HwResetHw;
    VideoDeviceExtension->HwTimer = HwInitializationData->HwTimer;

    RtlCopyMemory(VideoDeviceExtension->BaseAddresses, Configuration.u.type0.BaseAddresses,
        sizeof(ULONG) * PCI_TYPE0_ADDRESSES);

#if 0 // NUGOOP

    VideoDeviceExtension->AgpInterface.Size = sizeof(AGP_BUS_INTERFACE_STANDARD);
    VideoDeviceExtension->AgpInterface.Version = AGP_INTERFACE_VERSION;
    VideoDeviceExtension->AgpInterface.AgpContext = VideoDeviceExtension;
    VideoDeviceExtension->AgpInterface.InterfaceReference = AgpInterfaceReference;
    VideoDeviceExtension->AgpInterface.InterfaceDereference = AgpInterfaceDereference;
    VideoDeviceExtension->AgpInterface.ReserveMemory = AgpInterfaceReserveMemory;
    VideoDeviceExtension->AgpInterface.ReleaseMemory = AgpInterfaceReleaseMemory;
    VideoDeviceExtension->AgpInterface.CommitMemory = AgpInterfaceCommitMemory;
    VideoDeviceExtension->AgpInterface.FreeMemory = AgpInterfaceFreeMemory;
    VideoDeviceExtension->AgpInterface.GetMappedPages = AgpInterfaceGetMappedPages;

    ExInitializeFastMutex(&(VideoDeviceExtension->AgpInterfaceLock));

#endif 0

    KeInitializeDpc(&VideoDeviceExtension->Dpc, pVideoPortDpcDispatcher,
        HwDeviceExtension);

    KeInitializeMutex(&VideoDeviceExtension->SyncMutex, 0);

#if 1 // NUGOOP

    if (!AgpUmaEnable())
    {
        IoDeleteDevice(DeviceObject);
        return (ULONG)STATUS_UNSUCCESSFUL;
    }

#else 

    //
    // Initialize the AGP440.
    //

    status = AgpInitializeTarget(&VideoDeviceExtension->Agp440Extension);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return (ULONG)status;
    }

    status = AgpInitializeMaster(&VideoDeviceExtension->Agp440Extension,
        &(VideoDeviceExtension->AgpInterface.Capabilities));

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return (ULONG)status;
    }

    //
    // Query the current AGP aperture and immediately write it back out to the
    // hardware.  This is done to ensure that the AGP440 extension has all of
    // its fields set up properly.
    //

    status = AgpQueryAperture(&VideoDeviceExtension->Agp440Extension,
        &ApertureBase, &ApertureSizeInPages, NULL);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return (ULONG)status;
    }

    status = AgpSetAperture(&VideoDeviceExtension->Agp440Extension, ApertureBase,
        ApertureSizeInPages);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return (ULONG)status;
    }

#endif

    //
    // Invoke the miniport's HwFindAdapter routine.
    //

    RtlZeroMemory(&ConfigInfo, sizeof(VIDEO_PORT_CONFIG_INFO));

    ConfigInfo.Length = sizeof(VIDEO_PORT_CONFIG_INFO);
    ConfigInfo.BusInterruptLevel = 0xFF;
    ConfigInfo.BusInterruptVector = 0xFF;
    ConfigInfo.SystemMemorySize = /*(ULONGLONG)*/BasicInformation.NumberOfPhysicalPages << PAGE_SHIFT;

    vpstatus = VideoDeviceExtension->HwFindAdapter(HwDeviceExtension, NULL,
        NULL, &ConfigInfo, &Again);

    if (vpstatus != NO_ERROR) {
        IoDeleteDevice(DeviceObject);
        return (ULONG)STATUS_DEVICE_DOES_NOT_EXIST;
    }

    //
    // Connect to the device's interrupt.
    //

    if (VideoDeviceExtension->HwInterrupt != NULL) {

        VideoDeviceExtension->InterruptsEnabled = TRUE;

        InterruptVector = HalGetInterruptVector(PCIBus, 0,
            Configuration.u.type0.InterruptLine, Configuration.u.type0.InterruptLine,
            &InterruptIrql);

        status = IoConnectInterrupt(&(VideoDeviceExtension->InterruptObject),
            VideoPortInterrupt, VideoDeviceExtension, InterruptVector,
            InterruptIrql, InterruptIrql, LevelSensitive, TRUE);

        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(DeviceObject);
            return (ULONG)status;
        }
    }

    //
    // Initialize the timer if the miniport requests one.  If this fails, then
    // ignore the error.  This is the same as the old videoprt.sys.
    //

#if 0
    if (VideoDeviceExtension->HwTimer != NULL) {
        IoInitializeTimer(DeviceObject, pVideoPortHwTimer, NULL);
    }
#endif

    //
    // There are some callbacks that don't pass a context parameter, so we need
    // to do this hack of storing our device extension in a global location.
    //

    ASSERT(VideoGlobalDeviceExtension == NULL);
    VideoGlobalDeviceExtension = VideoDeviceExtension;

    //
    // The device has finished initializing and is ready to accept requests.
    //

    DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return (ULONG)STATUS_SUCCESS;
}

PVOID
VideoPortGetAssociatedDeviceExtension(
    IN PVOID DeviceObject
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;

    VideoDeviceExtension =
        (PVIDEO_DEVICE_EXTENSION)((PDEVICE_OBJECT)DeviceObject)->DeviceExtension;

    return VIDEO_GET_HW_DEVICE_EXTENSION(VideoDeviceExtension);
}

ULONG
VideoPortGetBusData(
    PVOID HwDeviceExtension,
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    )
{
    ASSERT(BusDataType == PCIConfiguration);

    return HalGetBusDataByOffset(PCIConfiguration, 1, SlotNumber, Buffer,
        Offset, Length);
}

ULONG
VideoPortSetBusData(
    PVOID HwDeviceExtension,
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    )
{
    ASSERT(BusDataType == PCIConfiguration);

    return HalSetBusDataByOffset(PCIConfiguration, 1, SlotNumber, Buffer,
        Offset, Length);
}

VP_STATUS
VideoPortGetAccessRanges(
    PVOID HwDeviceExtension,
    ULONG NumRequestedResources,
    PIO_RESOURCE_DESCRIPTOR RequestedResources OPTIONAL,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges,
    PVOID VendorId,
    PVOID DeviceId,
    PULONG Slot
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    ULONG Index;
    PCI_SLOT_NUMBER SlotNumber;

    VideoDeviceExtension = VIDEO_GET_VIDEO_DEVICE_EXTENSION(HwDeviceExtension);

    RtlZeroMemory(AccessRanges, sizeof(VIDEO_ACCESS_RANGE) * NumAccessRanges);

    if (NumAccessRanges > PCI_TYPE0_ADDRESSES) {
        NumAccessRanges = PCI_TYPE0_ADDRESSES;
    }

    for (Index = 0; Index < NumAccessRanges; Index++) {

        //
        // NVidia uses only addresses in the memory space, so don't bother to
        // handle port space.
        //

        ASSERT(VideoIsFlagClear(VideoDeviceExtension->BaseAddresses[Index],
            PCI_ADDRESS_IO_SPACE));

        //
        // NVidia only cares about seeing the base address, so don't bother
        // computing the length of the PCI BAR or any of the other attributes in
        // the VIDEO_ACCESS_RANGE.
        //

        AccessRanges[Index].RangeStart.QuadPart =
            (ULONGLONG)VideoDeviceExtension->BaseAddresses[Index] & PCI_ADDRESS_MEMORY_ADDRESS_MASK;
    }

    //
    // Fill in the PCI slot number for the display adapter.
    //

    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = AGP440_TARGET_DEVICE_NUMBER;
    SlotNumber.u.bits.FunctionNumber = AGP440_TARGET_FUNCTION_NUMBER;

    *Slot = SlotNumber.u.AsULONG;

    return NO_ERROR;
}

VP_STATUS
VideoPortGetRegistryParameters(
    PVOID HwDeviceExtension,
    PWSTR ParameterName,
    UCHAR IsParameterFileName,
    PMINIPORT_GET_REGISTRY_ROUTINE CallbackRoutine,
    PVOID Context
    )
{
    ASSERT(!IsParameterFileName);

//    VideoDbgPrint(("VideoPortGetRegistryParameters(%ws)\n", ParameterName));

    return ERROR_INVALID_PARAMETER;
}

VP_STATUS
VideoPortSetRegistryParameters(
    PVOID HwDeviceExtension,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )
{
//    VideoDbgPrint(("VideoPortSetRegistryParameters(%ws)\n", ValueName));

    return NO_ERROR;
}

BOOLEAN
VideoPortDDCMonitorHelper(
    IN PVOID pHwDeviceExtension,
    IN PVOID pDDCControl,
    IN OUT PUCHAR pucEdidBuffer,
    IN ULONG ulEdidBufferSize
    )
{
    //
    // The NVidia driver only tries to call this if we ask to enumerate the
    // devices attached to it (as in the monitor).  We won't do that, so don't
    // bother to implement this.
    //

    return FALSE;
}

BOOLEAN
VideoPortSynchronizeExecution(
    PVOID HwDeviceExtension,
    VIDEO_SYNCHRONIZE_PRIORITY Priority,
    PMINIPORT_SYNCHRONIZE_ROUTINE SynchronizeRoutine,
    PVOID Context
    )
{
    return pVideoPortSynchronizeExecution(HwDeviceExtension, Priority,
        SynchronizeRoutine, Context);
}

PVOID
VideoPortGetDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PHYSICAL_ADDRESS IoAddress,
    IN ULONG NumberOfUchars,
    IN UCHAR InIoSpace
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    MEMORY_CACHING_TYPE CacheType;
    PVOID MappedAddress;
    ULONG Index;

    VideoDeviceExtension = VIDEO_GET_VIDEO_DEVICE_EXTENSION(HwDeviceExtension);

    if ((InIoSpace & VIDEO_MEMORY_SPACE_P6CACHE) != 0) {
        CacheType = MmWriteCombined;
    } else {
        CacheType = MmNonCached;
    }

    MappedAddress = MmMapIoSpace(IoAddress.LowPart, NumberOfUchars, CacheType);

    if (MappedAddress == NULL) {
        return NULL;
    }

    //
    // MmUnmapIoSpace requires the size of the mapping and the miniport doesn't
    // pass that to VideoPortFreeDeviceBase, so we need to store the size in the
    // device extension.
    //

    for (Index = 0; Index < NUMBER_OF_VIDEO_MAPPED_ADDRESSES; Index++) {

        if (VideoDeviceExtension->MappedAddresses[Index].MappedAddress == NULL) {

            VideoDeviceExtension->MappedAddresses[Index].MappedAddress = MappedAddress;
            VideoDeviceExtension->MappedAddresses[Index].NumberOfUchars = NumberOfUchars;
            break;
        }
    }

    //
    // If we can't find a free slot for the entry, give up and let the mapping
    // leak.
    //

    ASSERT(Index < NUMBER_OF_VIDEO_MAPPED_ADDRESSES);

    return MappedAddress;
}

VOID
VideoPortFreeDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PVOID MappedAddress
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    ULONG Index;

    VideoDeviceExtension = VIDEO_GET_VIDEO_DEVICE_EXTENSION(HwDeviceExtension);

    for (Index = 0; Index < NUMBER_OF_VIDEO_MAPPED_ADDRESSES; Index++) {

        if (VideoDeviceExtension->MappedAddresses[Index].MappedAddress == MappedAddress) {

            MmUnmapIoSpace(MappedAddress,
                VideoDeviceExtension->MappedAddresses[Index].NumberOfUchars);

            VideoDeviceExtension->MappedAddresses[Index].MappedAddress = NULL;
            break;
        }
    }

    ASSERT(Index < NUMBER_OF_VIDEO_MAPPED_ADDRESSES);
}

VP_STATUS
VideoPortAllocateBuffer(
    IN PVOID HwDeviceExtension,
    IN ULONG Size,
    OUT PVOID *Buffer
    )
{
    ASSERT(Size != 0);

    *Buffer = ExAllocatePoolWithTag(PagedPool, Size, 'xdiV');

    return (*Buffer != NULL) ? NO_ERROR : ERROR_NOT_ENOUGH_MEMORY;
}

VOID
VideoPortReleaseBuffer(
    IN PVOID HwDeviceExtension,
    IN PVOID Buffer
    )
{
    if (Buffer != NULL) {
        ExFreePool(Buffer);
    }
}

VP_STATUS
VideoPortMapMemory(
    PVOID HwDeviceExtension,
    PHYSICAL_ADDRESS PhysicalAddress,
    PULONG Length,
    PULONG InIoSpace,
    PVOID *VirtualAddress
    )
{
    *VirtualAddress = VideoPortGetDeviceBase(HwDeviceExtension, PhysicalAddress,
        *Length, (UCHAR)*InIoSpace);

    return (*VirtualAddress != NULL) ? NO_ERROR : ERROR_NOT_ENOUGH_MEMORY;
}

VP_STATUS
VideoPortUnmapMemory(
    PVOID HwDeviceExtension,
    PVOID VirtualAddress,
    HANDLE ProcessHandle
    )
{
    VideoPortFreeDeviceBase(HwDeviceExtension, VirtualAddress);

    return NO_ERROR;
}

BOOLEAN
VideoPortQueueDpc(
    IN PVOID HwDeviceExtension,
    IN PMINIPORT_DPC_ROUTINE CallbackRoutine,
    IN PVOID Context
    )
{
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;

    VideoDeviceExtension = VIDEO_GET_VIDEO_DEVICE_EXTENSION(HwDeviceExtension);

    return KeInsertQueueDpc(&VideoDeviceExtension->Dpc,
        (PVOID)(ULONG_PTR)CallbackRoutine, (PVOID)Context);
}

VP_STATUS
VideoPortInt10(
    PVOID HwDeviceExtension,
    PVIDEO_X86_BIOS_ARGUMENTS BiosArguments
    )
{
    return ERROR_NOT_ENOUGH_MEMORY;
}

VP_STATUS
VideoPortSetTrappedEmulatorPorts(
    PVOID HwDeviceExtension,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRange
    )
{
    return NO_ERROR;
}


VIDEOPORT_API
VOID
VideoPortStartTimer(
    PVOID HwDeviceExtension
    )
{
    //
    // The NVIDIA driver tries to call this only for cursors.  Since
    // we're not using cursors (yet), it's safe to fail this.
    //
}

VIDEOPORT_API
VOID
VideoPortStopTimer(
    PVOID HwDeviceExtension
    )
{
}
