/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

    pnp.c

Abstract:

    This is the pnp portion of the video port driver.

Environment:

    kernel mode only

Revision History:

--*/

#include "videoprt.h"

#pragma alloc_text(PAGE,pVideoPortSendIrpToLowerDevice)
#pragma alloc_text(PAGE,pVideoPortPnpDispatch)


NTSTATUS
VpSetEventCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT Event
    )

{
    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
pVideoPortSendIrpToLowerDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine will forward the start request to the next lower device and
    block until it's completion.

Arguments:

    DeviceObject - the device to which the start request was issued.

    Irp - the start request

Return Value:

    status

--*/

{
    PFDO_EXTENSION fdoExtension = DeviceObject->DeviceExtension;

    PKEVENT event;
    NTSTATUS status;

    event = ExAllocatePoolWithTag(NonPagedPool,
                                  sizeof(KEVENT),
                                  POOL_TAG);

    if (event == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    KeInitializeEvent(event, SynchronizationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(Irp);

    IoSetCompletionRoutine(Irp,
                           VpSetEventCompletion,
                           event,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(fdoExtension->AttachedDeviceObject, Irp);

    if(status == STATUS_PENDING) {

        KeWaitForSingleObject(event, Executive, KernelMode, FALSE, NULL);

        status = Irp->IoStatus.Status;
    }

    ExFreePool(event);

    return status;

}


ULONG
VpGetDeviceAddress(
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine will get the address of a device (ie. slot number).

Arguments:

    DeviceObject - Object for which to retrieve the address

Returns:

    The address of the given device.

--*/

{
    KEVENT              Event;
    PIRP                QueryIrp = NULL;
    IO_STATUS_BLOCK     IoStatusBlock;
    PIO_STACK_LOCATION  NextStack;
    NTSTATUS            Status;
    DEVICE_CAPABILITIES Capabilities;
    PFDO_EXTENSION      FdoExtension = DeviceObject->DeviceExtension;

    RtlZeroMemory(&Capabilities, sizeof(DEVICE_CAPABILITIES));
    Capabilities.Size = sizeof(DEVICE_CAPABILITIES);
    Capabilities.Version = 1;
    Capabilities.Address = Capabilities.UINumber = (ULONG) -1;

    KeInitializeEvent(&Event, SynchronizationEvent, FALSE);

    QueryIrp = IoBuildSynchronousFsdRequest(IRP_MJ_FLUSH_BUFFERS,
                                            FdoExtension->AttachedDeviceObject,
                                            NULL,
                                            0,
                                            NULL,
                                            &Event,
                                            &IoStatusBlock);

    if (QueryIrp == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    QueryIrp->IoStatus.Status = IoStatusBlock.Status = STATUS_NOT_SUPPORTED;

    NextStack = IoGetNextIrpStackLocation(QueryIrp);

    //
    // Set up for a QueryInterface Irp.
    //

    NextStack->MajorFunction = IRP_MJ_PNP;
    NextStack->MinorFunction = IRP_MN_QUERY_CAPABILITIES;

    NextStack->Parameters.DeviceCapabilities.Capabilities = &Capabilities;

    Status = IoCallDriver(FdoExtension->AttachedDeviceObject, QueryIrp);

    if (Status == STATUS_PENDING) {
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
        Status = IoStatusBlock.Status;
    }


    ASSERT(NT_SUCCESS(Status));

    return (Capabilities.Address >> 16) | ((Capabilities.Address & 0x7) << 5);
}


NTSTATUS
pVideoPortPnpDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the PnP dispatch routine for the video port driver.
    It accepts an I/O Request Packet, transforms it to a video Request
    Packet, and forwards it to the appropriate miniport dispatch routine.
    Upon returning, it completes the request and return the appropriate
    status value.

Arguments:

    DeviceObject - Pointer to the device object of the miniport driver to
        which the request must be sent.

    Irp - Pointer to the request packet representing the I/O request.

Return Value:

    The function value os the status of the operation.

--*/

{
    PFDO_EXTENSION fdoExtension;
    PIO_STACK_LOCATION irpStack;
    PVOID ioBuffer;
    ULONG inputBufferLength;
    ULONG outputBufferLength;
    PSTATUS_BLOCK statusBlock;
    NTSTATUS finalStatus;
    ULONG ioControlCode;
    VIDEO_REQUEST_PACKET vrp;

    //
    // Get pointer to the port driver's device extension.
    //

    fdoExtension = DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current location in the Irp. This is where
    // the function codes and parameters are located.
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get the pointer to the status buffer.
    // Assume SUCCESS for now.
    //

    statusBlock = (PSTATUS_BLOCK) &Irp->IoStatus;

    ASSERT(irpStack->MajorFunction == IRP_MJ_PNP);

    pVideoDebugPrint((2, "VIDEO_TYPE_FDO : IRP_MJ_PNP: "));

    switch (irpStack->MinorFunction) {

    case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
        {
        PVIDEO_PORT_DRIVER_EXTENSION DriverObjectExtension;
        PIO_RESOURCE_REQUIREMENTS_LIST requirements;
        ULONG Length;

        pVideoDebugPrint((2, "IRP_MN_QUERY_RESOURCE_REQUIREMENTS\n"));

        DriverObjectExtension = (PVIDEO_PORT_DRIVER_EXTENSION)
                                IoGetDriverObjectExtension(
                                    DeviceObject->DriverObject,
                                    DeviceObject->DriverObject);

        //
        // We must first pass the Irp down to the PDO.
        //

        pVideoPortSendIrpToLowerDevice(DeviceObject, Irp);

        //
        // Determine the bus type and bus number
        //

        IoGetDeviceProperty(fdoExtension->PhysicalDeviceObject,
                            DevicePropertyLegacyBusType,
                            sizeof(fdoExtension->AdapterInterfaceType),
                            &fdoExtension->AdapterInterfaceType,
                            &Length);

        IoGetDeviceProperty(fdoExtension->PhysicalDeviceObject,
                            DevicePropertyBusNumber,
                            sizeof(fdoExtension->SystemIoBusNumber),
                            &fdoExtension->SystemIoBusNumber,
                            &Length);

        //
        // Get bus interface so we can use Get/SetBusData.
        //

        fdoExtension->ValidBusInterface =
            NT_SUCCESS(VpGetBusInterface(fdoExtension));

        requirements = irpStack->Parameters.FilterResourceRequirements.IoResourceRequirementList;

        if (requirements) {

            //
            // Append any legacy resources decoded by the device.
            //

            if (DriverObjectExtension->HwInitData.HwInitDataSize >
                FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwLegacyResourceCount)) {

                if( requirements->InterfaceType == PCIBus )
                {

                    PCI_COMMON_CONFIG ConfigSpace;

                    VideoPortGetBusData((PVOID)(fdoExtension + 1), //HwDeviceExtension
                                        PCIConfiguration,
                                        0,
                                        &ConfigSpace,
                                        0,
                                        PCI_COMMON_HDR_LENGTH);

                    if (((ConfigSpace.BaseClass == PCI_CLASS_PRE_20) &&
                         (ConfigSpace.SubClass  == PCI_SUBCLASS_PRE_20_VGA)) ||
                        ((ConfigSpace.BaseClass == PCI_CLASS_DISPLAY_CTLR) &&
                         (ConfigSpace.SubClass  == PCI_SUBCLASS_VID_VGA_CTLR))) {
    
                        if (pVideoPortGetVgaStatusPci((PVOID)(fdoExtension + 1) )) {

                            if (DriverObjectExtension->HwInitData.HwInitDataSize >
                                FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwGetLegacyResources)) {

                                if (DriverObjectExtension->HwInitData.HwGetLegacyResources) {

                                    //
                                    // If the miniport supplied a HwGetLegacyResources routine
                                    // it wasn't able to give us a list of resources at
                                    // DriverEntry time.  We'll give it a vendor/device id now
                                    // and see if it can give us a list of resources.
                                    //

                                    DriverObjectExtension->HwInitData.HwGetLegacyResources(
                                        ConfigSpace.VendorID,
                                        ConfigSpace.DeviceID,
                                        &DriverObjectExtension->HwInitData.HwLegacyResourceList,
                                        &DriverObjectExtension->HwInitData.HwLegacyResourceCount
                                        );
                                }
                            }

                            if (DriverObjectExtension->HwInitData.HwLegacyResourceList) {

                                if (VgaHwDeviceExtension) {

                                    ULONG Count;
                                    PVIDEO_ACCESS_RANGE AccessRange;

                                    Count       = DriverObjectExtension->HwInitData.HwLegacyResourceCount;
                                    AccessRange = DriverObjectExtension->HwInitData.HwLegacyResourceList;

                                    //
                                    // Mark VGA resources as shared if the vga driver is
                                    // already loaded.  Otherwise the PnP driver won't
                                    // be able to start.
                                    //

                                    while (Count--) {

                                        if (VpIsVgaResource(AccessRange)) {
                                            AccessRange->RangeShareable = TRUE;
                                        }

                                        AccessRange++;
                                    }
                                }

                                VpAppendToRequirementsList(
                                    DeviceObject,
                                    &requirements,
                                    DriverObjectExtension->HwInitData.HwLegacyResourceCount,
                                    DriverObjectExtension->HwInitData.HwLegacyResourceList);

                            } else if (!IsNEC_98) {

                                //
                                // The driver didn't specify legacy resources, but we
                                // know that it is a VGA, so add in the vga resources.
                                //

                                pVideoDebugPrint((0, "VGA device didn't specify legacy resources.\n"));

                                DriverObjectExtension->HwInitData.HwLegacyResourceCount = NUM_VGA_LEGACY_RESOURCES;
                                DriverObjectExtension->HwInitData.HwLegacyResourceList = VgaLegacyResources;

                                VpAppendToRequirementsList(
                                    DeviceObject,
                                    &requirements,
                                    NUM_VGA_LEGACY_RESOURCES,
                                    VgaLegacyResources);
                            }
                        }
                    }
                }
            }

            //
            // Now if there is an interrupt in the list, but
            // the miniport didn't register an ISR, then
            // release our claim on the interrupt.
            //

#if 1
            {
                PIO_RESOURCE_LIST resourceList;
                ULONG i;

                //
                // Scan the IO_RESOURCE_REQUIREMENTS_LIST for an
                // interrupt.
                //

                resourceList = requirements->List;

                for (i=0; i<resourceList->Count; i++) {

                    if (resourceList->Descriptors[i].Type == CmResourceTypeInterrupt) {

                        if( requirements->InterfaceType == PCIBus )
                        {

                            PCI_COMMON_CONFIG ConfigSpace;

                            VideoPortGetBusData((PVOID)(fdoExtension + 1), //HwDeviceExtension
                                                PCIConfiguration,
                                                0,
                                                &ConfigSpace,
                                                0,
                                                PCI_COMMON_HDR_LENGTH);

#if 0
                            if (ConfigSpace.VendorID == ???)

                                //
                                // We found an interrupt resource swap with last
                                // element in list, and decrement structure size and
                                // list count.
                                //

                                resourceList->Descriptors[i].u.Interrupt.MinimumVector = 0x0B;
                                resourceList->Descriptors[i].u.Interrupt.MaximumVector = 0x0B;
                                DbgPrint("VideoPort Requesting IRQ 11.\n");
                            }
#endif
                        }
                    }
                }
            }
#else
            if (!DriverObjectExtension->HwInitData.HwInterrupt) {

                PIO_RESOURCE_LIST resourceList;
                ULONG i;

                //
                // Scan the IO_RESOURCE_REQUIREMENTS_LIST for an
                // interrupt.
                //

                resourceList = requirements->List;

                for (i=0; i<resourceList->Count; i++) {

                    if (resourceList->Descriptors[i].Type == CmResourceTypeInterrupt) {

                        //
                        // We found an interrupt resource swap with last
                        // element in list, and decrement structure size and
                        // list count.
                        //

                        resourceList->Descriptors[i].Type = CmResourceTypeNull;

                        pVideoDebugPrint((1, "Removing Int from requirements list.\n"));
                    }
                }
            }
#endif

        } else {

            pVideoDebugPrint((0, "We expected a list of resources!\n"));
            ASSERT(FALSE);
        }


        statusBlock->Information = (ULONG_PTR) requirements;
        statusBlock->Status = STATUS_SUCCESS;

        }

        break;

    case IRP_MN_START_DEVICE:
        {
        PVIDEO_PORT_DRIVER_EXTENSION DriverObjectExtension;
        PCM_RESOURCE_LIST allocatedResources;
        PCM_RESOURCE_LIST translatedResources;
        UCHAR nextMiniport = FALSE;
        ULONG RawListSize;
        ULONG TranslatedListSize;

        pVideoDebugPrint((2, "IRP_MN_START_DEVICE\n"));

        //
        // Retrieve the data we cached away during VideoPortInitialize.
        //

        DriverObjectExtension = (PVIDEO_PORT_DRIVER_EXTENSION)
                                IoGetDriverObjectExtension(
                                    DeviceObject->DriverObject,
                                    DeviceObject->DriverObject);

        ASSERT(DriverObjectExtension);

        //
        // Grab the allocated resource the system gave us.
        //

        allocatedResources =
            irpStack->Parameters.StartDevice.AllocatedResources;
        translatedResources =
            irpStack->Parameters.StartDevice.AllocatedResourcesTranslated;

        //
        // Filter out any resources that we added to the list
        // before passing the irp on to PCI.
        //

        if (DriverObjectExtension->HwInitData.HwInitDataSize >
            FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwLegacyResourceCount)) {

            if (DriverObjectExtension->HwInitData.HwLegacyResourceList) {

                if (allocatedResources) {
                    irpStack->Parameters.StartDevice.AllocatedResources =
                        VpRemoveFromResourceList(
                            allocatedResources,
                            DriverObjectExtension->HwInitData.HwLegacyResourceCount,
                            DriverObjectExtension->HwInitData.HwLegacyResourceList);

                }

                if ((irpStack->Parameters.StartDevice.AllocatedResources !=
                     allocatedResources) && translatedResources) {

                    irpStack->Parameters.StartDevice.AllocatedResourcesTranslated =
                        VpRemoveFromResourceList(
                            translatedResources,
                            DriverObjectExtension->HwInitData.HwLegacyResourceCount,
                            DriverObjectExtension->HwInitData.HwLegacyResourceList);

                }
            }
        }

        //
        // The first thing we need to do is send the START_DEVICE
        // irp on to our parent.
        //

        pVideoPortSendIrpToLowerDevice(DeviceObject, Irp);

        //
        // Restore the original resources.
        //

        if (irpStack->Parameters.StartDevice.AllocatedResources !=
            allocatedResources) {

            ExFreePool(irpStack->Parameters.StartDevice.AllocatedResources);
            irpStack->Parameters.StartDevice.AllocatedResources
                = allocatedResources;
        }

        if (irpStack->Parameters.StartDevice.AllocatedResourcesTranslated !=
            translatedResources) {

            ExFreePool(irpStack->Parameters.StartDevice.AllocatedResourcesTranslated);
            irpStack->Parameters.StartDevice.AllocatedResourcesTranslated
                = translatedResources;
        }

        if (allocatedResources) {

            ASSERT(translatedResources);

            //
            // Cache assigned and translated resources.
            //

            RawListSize = GetCmResourceListSize(allocatedResources);
            TranslatedListSize = GetCmResourceListSize(translatedResources);

            ASSERT(RawListSize == TranslatedListSize);

            fdoExtension->RawResources = ExAllocatePoolWithTag(PagedPool,
                                                               RawListSize +
                                                               TranslatedListSize,
                                                               POOL_TAG);

            fdoExtension->TranslatedResources = (PCM_RESOURCE_LIST)
                ((PUCHAR)fdoExtension->RawResources + RawListSize);

            if (fdoExtension->RawResources == NULL) {

                statusBlock->Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            memcpy(fdoExtension->RawResources,
                   allocatedResources,
                   RawListSize);

            memcpy(fdoExtension->TranslatedResources,
                   translatedResources,
                   TranslatedListSize);
        }

        //
        // Get slot/function number
        //

        fdoExtension->SlotNumber = VpGetDeviceAddress(DeviceObject);

        //
        // Store the allocatedResources. This will allow us to
        // assign these resources when VideoPortGetAccessRanges
        // routines are called.
        //
        // NOTE: We do not actually have to copy the data, because
        //       we are going to call FindAdapter in the context
        //       of this function.  So, this data will be intact
        //       until we complete.
        //

        if ((allocatedResources != NULL) && (translatedResources != NULL)) {

            ULONG Count;
            PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptDesc;

            Count = 0;
            InterruptDesc = RtlUnpackPartialDesc(CmResourceTypeInterrupt,
                                                 translatedResources,
                                                 &Count);
            
            fdoExtension->AllocatedResources = allocatedResources;
            fdoExtension->SystemIoBusNumber =
                allocatedResources->List->BusNumber;
            fdoExtension->AdapterInterfaceType =
                allocatedResources->List->InterfaceType;
            
            //
            // Tuck away the giblets we need for PnP interrupt support!
            //
            if (InterruptDesc) {
                fdoExtension->InterruptVector =
                    InterruptDesc->u.Interrupt.Vector;
                fdoExtension->InterruptIrql =
                    (KIRQL)InterruptDesc->u.Interrupt.Level;
                fdoExtension->InterruptAffinity =
                    InterruptDesc->u.Interrupt.Affinity;
            }
            
        } else {

            //
            // The system doesn't have resource information for this
            // legacy (ex. isa) pnp card.  The user may have incorrectly
            // installed the driver.
            //

            pVideoDebugPrint((0, "The system should pass us resources!\n"));

            ASSERT(FALSE);
        }

        ACQUIRE_DEVICE_LOCK (fdoExtension);

        if (VideoPortFindAdapter(DeviceObject->DriverObject,
                                 (PVOID)&(DriverObjectExtension->RegistryPath),
                                 &(DriverObjectExtension->HwInitData),
                                 NULL,
                                 DeviceObject,
                                 &nextMiniport) == NO_ERROR) {

            //
            // BUGBUG
            // This should not return TRUE for nextMiniport. (???)
            //

            // ASSERT(nextMiniport != TRUE);

            statusBlock->Status = STATUS_SUCCESS;

            //
            // If the system is already up and running, lets call
            // HwInitialize now.  This will allow us to enumerate
            // children.
            //

            if (!InbvCheckDisplayOwnership() ) {

                VpEnableDisplay(FALSE);

                if (fdoExtension->HwInitialize(fdoExtension->HwDeviceExtension)) {
                    fdoExtension->HwInitStatus = HwInitSucceeded;
                } else {
                    fdoExtension->HwInitStatus = HwInitFailed;
                }

                VpEnableDisplay(TRUE);
            }

        } else {

            statusBlock->Status = STATUS_UNSUCCESSFUL;

            if (fdoExtension->RawResources) {
                ExFreePool(fdoExtension->RawResources);
            }
        }

        RELEASE_DEVICE_LOCK (fdoExtension);

        }

        break;

    case IRP_MN_QUERY_INTERFACE:

        //
        // Normally I would only expect to get this IRP heading for
        // an PDO.  However, AndrewGo wants to be able to send down
        // these IRP's and he only has an FDO.  Instead of forcing
        // him to get a PDO somehow, we'll just handle the irp for
        // a FDO as well.
        //

        pVideoDebugPrint((2, "IRP_MN_QUERY_INTERFACE\n"));

        ACQUIRE_DEVICE_LOCK (fdoExtension);

        if ((fdoExtension->HwQueryInterface) &&
            (fdoExtension->HwDeviceExtension) &&
            (NO_ERROR == fdoExtension->HwQueryInterface(
                                  fdoExtension->HwDeviceExtension,
                                  (PQUERY_INTERFACE)
                                  &irpStack->Parameters.QueryInterface)))
        {
            statusBlock->Status = STATUS_SUCCESS;
        }
        else if (!NT_SUCCESS(statusBlock->Status))
        {
            //
            // The miniport didn't handle the QueryInterface request, see
            // if its an interface the videoprt supports.
            //

            PQUERY_INTERFACE qi = (PQUERY_INTERFACE)
                                  &irpStack->Parameters.QueryInterface;

            //
            // If we are responding to a known private GUID, expose
            // the known GUID interface ourselves.  Otherwise, pass
            // on to the miniport driver.
            //

            if (IsEqualGUID(qi->InterfaceType, &GUID_AGP_INTERFACE)) {

                PAGP_INTERFACE AgpInterface = (PAGP_INTERFACE)qi->Interface;

                AgpInterface->Size    = sizeof(AGP_INTERFACE);
                AgpInterface->Version = AGP_INTERFACE_VERSION;
                AgpInterface->Context = fdoExtension->HwDeviceExtension;

                if (VideoPortGetAgpServices(fdoExtension->HwDeviceExtension,
                                            &AgpInterface->AgpServices)) {

                    statusBlock->Status = STATUS_SUCCESS;
                }
            }
        }

        RELEASE_DEVICE_LOCK (fdoExtension);

        IoCopyCurrentIrpStackLocationToNext(Irp);
        return IoCallDriver(fdoExtension->AttachedDeviceObject, Irp);

    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    
        statusBlock->Status = STATUS_SUCCESS;
        statusBlock->Information |= PNP_DEVICE_NOT_DISABLEABLE ;
        IoCopyCurrentIrpStackLocationToNext (Irp) ;
        return IoCallDriver(fdoExtension->AttachedDeviceObject, Irp);

        break;

    default:

        pVideoDebugPrint((2, "PNP minor function %x not supported - forwarding \n", irpStack->MinorFunction ));

        IoCopyCurrentIrpStackLocationToNext(Irp);
        return IoCallDriver(fdoExtension->AttachedDeviceObject, Irp);
    }

    //
    // save the final status so we can return it after the IRP is completed.
    //

    finalStatus = statusBlock->Status;

    IoCompleteRequest(Irp,
                      IO_VIDEO_INCREMENT);

    return finalStatus;
}


PCM_PARTIAL_RESOURCE_DESCRIPTOR
RtlUnpackPartialDesc(
    IN UCHAR Type,
    IN PCM_RESOURCE_LIST ResList,
    IN OUT PULONG Count
    )
/*++

Routine Description:

    Pulls out a pointer to the partial descriptor you're interested in

Arguments:

    Type - CmResourceTypePort, ...
    ResList - The list to search
    Count - Points to the index of the partial descriptor you're looking
            for, gets incremented if found, i.e., start with *Count = 0,
            then subsequent calls will find next partial, make sense?

Return Value:

    Pointer to the partial descriptor if found, otherwise NULL

--*/
{
    ULONG i, j, hit;

    hit = 0;
    for (i = 0; i < ResList->Count; i++) {
        for (j = 0; j < ResList->List[i].PartialResourceList.Count; j++) {
            if (ResList->List[i].PartialResourceList.PartialDescriptors[j].Type == Type) {
                if (hit == *Count) {
                    (*Count)++;
                    return &ResList->List[i].PartialResourceList.PartialDescriptors[j];
                } else {
                    hit++;
                }
            }
        }
    }

    return NULL;
}
