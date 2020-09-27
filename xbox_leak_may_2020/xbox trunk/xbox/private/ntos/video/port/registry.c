/*++

Copyright (c) 1992-1999  Microsoft Corporation

Module Name:

    registry.c

Abstract:

    Registry support for the video port driver.

Author:

    Andre Vachon (andreva) 01-Mar-1992

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "videoprt.h"


//
// Local routines.
//

BOOLEAN
CheckIoEnabled(
    PVOID HwDeviceExtension,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges
    );

ULONG
GetCmResourceListSize(
    PCM_RESOURCE_LIST CmResourceList
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE,VpGetFlags)
#pragma alloc_text(PAGE,VideoPortGetAccessRanges)
#pragma alloc_text(PAGE,VideoPortVerifyAccessRanges)
#pragma alloc_text(PAGE,CheckIoEnabled)
#pragma alloc_text(PAGE,VpAppendToRequirementsList)
#pragma alloc_text(PAGE,VpIsLegacyAccessRange)
#pragma alloc_text(PAGE,GetCmResourceListSize)
#pragma alloc_text(PAGE,VpRemoveFromResourceList)
#pragma alloc_text(PAGE,VpIsVgaResource)
#endif

NTSTATUS
VpGetFlags(
    PUNICODE_STRING RegistryPath,
    PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    PULONG Flags
    )

/*++

Routine Description:

    Checks for the existance of the PnP key/value in the device's
    registry path.

Return Value:

    TRUE if the flag exists, FALSE otherwise.

--*/

{
    PWSTR    Path;
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
    ULONG    pnpEnabled = 0;
    ULONG    legacyDetect = 0;
    ULONG    defaultValue = 0;
    ULONG    bootDriver = 0;
    ULONG    reportDevice = 0;
    PWSTR    Table[] = {L"\\Vga", L"\\VgaSave", NULL};
    PWSTR    SubStr, *Item = Table;
    ULONG    Len;

    *Flags = 0;

    Path = ExAllocatePoolWithTag(PagedPool,
                                 RegistryPath->Length + sizeof(UNICODE_NULL),
                                 POOL_TAG);

    if (Path)
    {
        RtlCopyMemory(Path,
                      RegistryPath->Buffer,
                      RegistryPath->Length);

        *(Path + (RegistryPath->Length / sizeof(UNICODE_NULL))) = UNICODE_NULL;

        pVideoDebugPrint((1, "PnP path: %ws\n", Path));

        legacyDetect = FALSE;
        reportDevice = FALSE;

        //
        // If the PnP Entry points are present, then we will treat this
        // driver as a PnP driver.
        //

        if ( (HwInitializationData->HwInitDataSize >=
              FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwQueryInterface)) &&
             (HwInitializationData->HwSetPowerState != NULL)                &&
             (HwInitializationData->HwGetPowerState != NULL)                &&
             (HwInitializationData->HwGetVideoChildDescriptor != NULL) )
        {
            pVideoDebugPrint((1, "videoprt: The miniport is a PnP miniport."));

            pnpEnabled = TRUE;
        }

        //
        // REPORT_DEVICE is only valid if PNP_ENABLED is true.
        //
        // We don't want to report a device to the PnP system if
        // we don't have a PnP driver.
        //

        if (!pnpEnabled)
        {
            reportDevice = 0;
        }

        *Flags = (pnpEnabled   ? PNP_ENABLED   : 0) |
                 (legacyDetect ? LEGACY_DETECT : 0) |
                 (reportDevice ? REPORT_DEVICE : 0);

        //
        // Free the memory we allocated above.
        //

        ExFreePool(Path);


        //
        // Determine if the current miniport is the VGA miniport.
        //

        while (*Item) {

            Len = wcslen(*Item);

            SubStr = RegistryPath->Buffer + (RegistryPath->Length / 2) - Len;

            if (!_wcsnicmp(SubStr, *Item, Len)) {

                pVideoDebugPrint((1, "This IS the vga miniport\n"));
                *Flags |= VGA_DRIVER;
                break;
            }

            Item++;
        }

        pVideoDebugPrint((1, "Flags = %d\n", *Flags));

        ntStatus = STATUS_SUCCESS;
    }

    return ntStatus;
}


VIDEOPORT_API
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

/*++

Routine Description:

    Walk the appropriate bus to get device information.
    Search for the appropriate device ID.
    Appropriate resources will be returned and automatically stored in the
    resourcemap.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    NumRequestedResources - Number of entries in the RequestedResources array.

    RequestedResources - Optional pointer to an array ofRequestedResources
        the miniport driver wants to access.

    NumAccessRanges - Maximum number of access ranges that can be returned
        by the function.

    AccessRanges - Array of access ranges that will be returned to the driver.

    VendorId - Pointer to the vendor ID. On PCI, this is a pointer to a 16 bit
        word.

    DeviceId - Pointer to the Device ID. On PCI, this is a pointer to a 16 bit
        word.

    Slot - Pointer to the starting slot number for this search.

Return Value:

    ERROR_MORE_DATA if the AccessRange structure is not large enough for the
       PCI config info.
    ERROR_DEV_NOT_EXIST is the card is not found.

    NO_ERROR if the function succeded.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    UNICODE_STRING unicodeString;
    ULONG i;
    ULONG j;

    PCM_RESOURCE_LIST cmResourceList;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmResourceDescriptor;


    VP_STATUS status;
    UCHAR bShare;

    PPCI_SLOT_NUMBER slotData = (PPCI_SLOT_NUMBER)Slot;

    //
    // This is the miniport drivers slot. Allocate the
    // resources.
    //

    // Hack Add extra R so the Device0 key does not get created as volatile
    // a screw up the subsequent driver install.

    *(LPWSTR) (((PUCHAR)fdoExtension->DriverRegistryPath) +
               fdoExtension->DriverRegistryPathLength) = L'R';

    RtlInitUnicodeString(&unicodeString, fdoExtension->DriverRegistryPath);

    //
    // Assert drivers do set those parameters properly
    //

#if DBG

    if ((NumRequestedResources == 0) != (RequestedResources == NULL)) {

        pVideoDebugPrint((0, "VideoPortGetDeviceResources: Parameters for requested resource are inconsistent\n"));

    }

#endif

    //
    // An empty requested resource list means we want to automatic behavoir.
    // Just call the HAL to get all the information
    //

    if (NumRequestedResources == 0) {

        //
        // If a PnP driver is requesting resources, then return what the
        // system passed in to us.
        //

        cmResourceList = fdoExtension->AllocatedResources;

        //
        // Return the slot number to the device.
        //

        if (Slot) {
            *Slot = fdoExtension->SlotNumber;
        }

        if (cmResourceList) {
#if DBG
            DumpResourceList(cmResourceList);
#endif
            status = NO_ERROR;

        } else {

            //
            // The system should always pass us resources.
            //

            ASSERT(FALSE);
            status = ERROR_INVALID_PARAMETER;
        }

    } else {

        PIO_RESOURCE_REQUIREMENTS_LIST requestedResources;
        ULONG requestedResourceSize;
        NTSTATUS ntStatus;

        status = NO_ERROR;

        //
        // The caller has specified some resources.
        // Lets call IoAssignResources with that and see what comes back.
        //

        requestedResourceSize = sizeof(IO_RESOURCE_REQUIREMENTS_LIST) +
                                   ((NumRequestedResources - 1) *
                                   sizeof(IO_RESOURCE_DESCRIPTOR));

        requestedResources = ExAllocatePoolWithTag(PagedPool,
                                                   requestedResourceSize,
                                                   POOL_TAG);

        if (requestedResources) {

            RtlZeroMemory(requestedResources, requestedResourceSize);

            requestedResources->ListSize = requestedResourceSize;
            requestedResources->InterfaceType = fdoExtension->AdapterInterfaceType;
            requestedResources->BusNumber = fdoExtension->SystemIoBusNumber;
            requestedResources->SlotNumber = slotData->u.bits.DeviceNumber;
            requestedResources->AlternativeLists = 1;

            requestedResources->List[0].Version  = 1;
            requestedResources->List[0].Revision = 1;
            requestedResources->List[0].Count    = NumRequestedResources;

            RtlMoveMemory(&(requestedResources->List[0].Descriptors[0]),
                          RequestedResources,
                          NumRequestedResources * sizeof(IO_RESOURCE_DESCRIPTOR));

            ntStatus = IoAssignResources(&unicodeString,
                                         &VideoClassName,
                                         fdoExtension->FunctionalDeviceObject->DriverObject,
                                         fdoExtension->FunctionalDeviceObject,
                                         requestedResources,
                                         &cmResourceList);

            ExFreePool(requestedResources);

            if (!NT_SUCCESS(ntStatus)) {

                status = ERROR_INVALID_PARAMETER;

            }

        } else {

            status = ERROR_NOT_ENOUGH_MEMORY;

        }

    }

    if (status == NO_ERROR) {

        VIDEO_ACCESS_RANGE TempRange;

        //
        // We now have a valid cmResourceList.
        // Lets translate it back to access ranges so the driver
        // only has to deal with one type of list.
        //

        //
        // NOTE: The resources have already been reported at this point in
        // time.
        //

        //
        // Walk resource list to update configuration information.
        //

        for (i = 0, j = 0;
             (i < cmResourceList->List->PartialResourceList.Count) &&
                 (status == NO_ERROR);
             i++) {

            //
            // Get resource descriptor.
            //

            cmResourceDescriptor =
                &cmResourceList->List->PartialResourceList.PartialDescriptors[i];

            //
            // Get the share disposition
            //

            if (cmResourceDescriptor->ShareDisposition == CmResourceShareShared) {

                bShare = 1;

            } else {

                bShare = 0;

            }

            switch (cmResourceDescriptor->Type) {

            case CmResourceTypePort:
            case CmResourceTypeMemory:

                //
                // common part
                //

                TempRange.RangeLength =
                    cmResourceDescriptor->u.Memory.Length;
                TempRange.RangeStart =
                    cmResourceDescriptor->u.Memory.Start;
                TempRange.RangeVisible = 0;
                TempRange.RangeShareable = bShare;
                TempRange.RangePassive = 0;

                //
                // separate part
                //

                if (cmResourceDescriptor->Type == CmResourceTypePort) {
                    TempRange.RangeInIoSpace = 1;
                } else {
                    TempRange.RangeInIoSpace = 0;
                }

                //
                // See if we need to return the resource to the driver.
                //

                if (!VpIsLegacyAccessRange(fdoExtension, &TempRange)) {

                    if (j == NumAccessRanges) {

                        status = ERROR_MORE_DATA;
                        break;

                    } else {

                        //
                        // Only modify the AccessRange array if we are writing
                        // valid data.
                        //

                        AccessRanges[j] = TempRange;
                        j++;
                    }

                }

                break;

            case CmResourceTypeInterrupt:

                fdoExtension->MiniportConfigInfo->BusInterruptVector =
                    cmResourceDescriptor->u.Interrupt.Vector;
                fdoExtension->MiniportConfigInfo->BusInterruptLevel =
                    cmResourceDescriptor->u.Interrupt.Level;
                fdoExtension->MiniportConfigInfo->InterruptShareable =
                    bShare;

                break;

            case CmResourceTypeDma:

                fdoExtension->MiniportConfigInfo->DmaChannel =
                    cmResourceDescriptor->u.Dma.Channel;
                fdoExtension->MiniportConfigInfo->DmaPort =
                    cmResourceDescriptor->u.Dma.Port;
                fdoExtension->MiniportConfigInfo->DmaShareable =
                    bShare;

                break;

            default:

                pVideoDebugPrint((1, "VideoPortGetAccessRanges: Unknown descriptor type %x\n",
                                 cmResourceDescriptor->Type ));

                break;

            }

        }
    }

    // Hack remove extra R

    *(LPWSTR) (((PUCHAR)fdoExtension->DriverRegistryPath) +
               fdoExtension->DriverRegistryPathLength) = UNICODE_NULL;


#if DBG

    if (status == NO_ERROR)
    {
        //
        // Indicates resources have been mapped properly
        //

        InterlockedIncrement(&VPResourcesReported);
    }

#endif

    return status;

} // VideoPortGetDeviceResources()

BOOLEAN
VpIsVgaResource(
    PVIDEO_ACCESS_RANGE AccessRange
    )

/*++

Routine Description:

    Indicates whether the given access range is a vga access range.

Arguments:

    AccessRange - The access range to examine.

Returns:

    TRUE if it is a VGA access range,
    FALSE otherwise.

Notes:

    This routine does not take into account the length of the access range.

--*/

{
    if (AccessRange->RangeInIoSpace) {

        ULONGLONG Port = AccessRange->RangeStart.QuadPart;

        if (((Port >= 0x3b0) && (Port <= 0x3bb)) ||
            ((Port >= 0x3c0) && (Port <= 0x3df))) {

            return TRUE;

        }

    } else {

        if (AccessRange->RangeStart.QuadPart == 0xa0000) {
            return TRUE;
        }
    }

    return FALSE;
}

NTSTATUS
VpAppendToRequirementsList(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIO_RESOURCE_REQUIREMENTS_LIST *RequirementList,
    IN ULONG NumAccessRanges,
    IN PVIDEO_ACCESS_RANGE AccessRanges
    )

/*++

Routine Description:

    Builds a IoResourceRequirementsList for a given set of access ranges.

Arguments:

    ResourceList - Pointer to location of the requirments list.  Modified
        on completion to point to a new requirements list.

    NumAccessRanges - Number of access ranges in list.

    AccessRanges - List of resources.


Returns:

    STATUS_SUCCESS if successful, otherwise a status code.

Notes:

    This function free's the memory used by the original resource list,
    and allocates a new buffer for the appended resources list.

--*/

{
    PIO_RESOURCE_REQUIREMENTS_LIST OriginalRequirementList = *RequirementList;
    PIO_RESOURCE_DESCRIPTOR pioDescript;
    ULONG RequirementListSize;
    ULONG OriginalListSize;
    ULONG RequirementCount;
    ULONG i;

    RequirementCount = OriginalRequirementList->List[0].Count;
    OriginalListSize = OriginalRequirementList->ListSize;

    RequirementListSize = OriginalListSize +
                              NumAccessRanges * sizeof(IO_RESOURCE_DESCRIPTOR);

    *RequirementList =
        (PIO_RESOURCE_REQUIREMENTS_LIST) ExAllocatePool(PagedPool,
                                                        RequirementListSize);

    //
    // Return NULL if the structure could not be allocated.
    // Otherwise, fill it out.
    //

    if (*RequirementList == NULL) {

        *RequirementList = OriginalRequirementList;
        return STATUS_INSUFFICIENT_RESOURCES;

    } else {

        //
        // Copy the original resource list into the new one.
        //

        memcpy(*RequirementList, OriginalRequirementList, OriginalListSize);

        //
        // Free the original list
        //

        ExFreePool(OriginalRequirementList);

        //
        // Point to first free entry in requirements list
        //

        pioDescript =
            &((*RequirementList)->List[0].Descriptors[(*RequirementList)->List[0].Count]);

        //
        // For each entry in the access range, fill in an entry in the
        // resource list
        //

        for (i = 0; i < NumAccessRanges; i++) {

            //
            // We will never claim 0xC0000.
            //

            if ((AccessRanges->RangeStart.LowPart == 0xC0000) &&
                (AccessRanges->RangeInIoSpace == FALSE))
            {
                AccessRanges++;
                continue;
            }

            if (AccessRanges->RangeLength == 0) {

                AccessRanges++;
                continue;
            }

            //
            // Watch to see if the VGA resources get added to the
            // requirements list.  If so set a flag so that we know
            // we don't need to reclaim VGA resources in FindAdapter.
            //

            if (VpIsVgaResource(AccessRanges)) {
                DeviceOwningVga = DeviceObject;
            }

            if (AccessRanges->RangeInIoSpace) {
                pioDescript->Type = CmResourceTypePort;
                pioDescript->Flags = CM_RESOURCE_PORT_IO;

                //
                // Disable 10_BIT_DECODE.  This is causing problems for the
                // PnP folks.  If someone has bad hardware, we'll just
                // require them to report all the passive port explicitly.
                //
                //if (VpIsVgaResource(AccessRanges)) {
                //
                //    pioDescript->Flags |= CM_RESOURCE_PORT_10_BIT_DECODE;
                //}

            } else {

                pioDescript->Type = CmResourceTypeMemory;
                pioDescript->Flags = CM_RESOURCE_MEMORY_READ_WRITE;
            }

            if (AccessRanges->RangePassive & VIDEO_RANGE_PASSIVE_DECODE) {
                pioDescript->Flags |= CM_RESOURCE_PORT_PASSIVE_DECODE;
            }

            if (AccessRanges->RangePassive & VIDEO_RANGE_10_BIT_DECODE) {
                pioDescript->Flags |= CM_RESOURCE_PORT_10_BIT_DECODE;
            }

            pioDescript->ShareDisposition =
                    (AccessRanges->RangeShareable ?
                        CmResourceShareShared :
                        CmResourceShareDeviceExclusive);

            pioDescript->Option = IO_RESOURCE_PREFERRED;
            pioDescript->u.Memory.MinimumAddress = AccessRanges->RangeStart;
            pioDescript->u.Memory.MaximumAddress.QuadPart =
                                                   AccessRanges->RangeStart.QuadPart +
                                                   AccessRanges->RangeLength - 1;
            pioDescript->u.Memory.Alignment = 1;
            pioDescript->u.Memory.Length = AccessRanges->RangeLength;

            pioDescript++;
            AccessRanges++;
            RequirementCount++;
        }

        //
        // Update number of elements in list.
        //

        (*RequirementList)->List[0].Count = RequirementCount;
        (*RequirementList)->ListSize = RequirementListSize;
    }

    return STATUS_SUCCESS;
}



VP_STATUS
VideoPortVerifyAccessRanges(
    PVOID HwDeviceExtension,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges
    )

/*++

Routine Description:

    VideoPortVerifyAccessRanges


Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    NumAccessRanges - Number of entries in the AccessRanges array.

    AccessRanges - Pointer to an array of AccessRanges the miniport driver
        wants to access.

Return Value:

    ERROR_INVALID_PARAMETER in an error occured
    NO_ERROR if the call completed successfully

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    NTSTATUS status;
    BOOLEAN conflict;
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    //
    // If the device is not enabled then we won't allow the miniport
    // to claim resources for it.
    //

    if (!CheckIoEnabled(
            HwDeviceExtension,
            NumAccessRanges,
            AccessRanges)) {

        return ERROR_INVALID_PARAMETER;
    }

    //
    // All resources not passed in during the START_DEVICE irp should
    // be claimed on the FDO.  We will strip out the PDO resources
    // in pVideoPortReportResourceList if the miniport driver tries
    // to verify ranges acquired through VideoPortGetAccessRanges.
    //

    status = STATUS_SUCCESS;
    conflict = FALSE;

    if ((NT_SUCCESS(status)) && (!conflict)) {

#if DBG

        //
        // Indicates resources have been mapped properly
        //

        InterlockedIncrement(&VPResourcesReported);

#endif

        //
        // Track the resources owned by the VGA driver.
        //

        if (fdoExtension->Flags & VGA_DRIVER) {

            if (VgaAccessRanges != AccessRanges) {

                ULONG Size = NumAccessRanges * sizeof(VIDEO_ACCESS_RANGE);

                if (VgaAccessRanges) {
                    ExFreePool(VgaAccessRanges);
                    VgaAccessRanges = NULL;
                    NumVgaAccessRanges = 0;
                }

                if (NumAccessRanges) {
                    VgaAccessRanges = ExAllocatePoolWithTag(PagedPool, Size, POOL_TAG);

                    if (VgaAccessRanges) {
                        memcpy(VgaAccessRanges, AccessRanges, Size);
                        NumVgaAccessRanges = NumAccessRanges;
                    }
                }
            }
        }

        return NO_ERROR;

    } else {

        return ERROR_INVALID_PARAMETER;

    }

} // end VideoPortVerifyAccessRanges()

BOOLEAN
CheckIoEnabled(
    PVOID HwDeviceExtension,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges
    )

/*++

Routine Description:

    This routine ensures that IO is actually enabled if claiming
    IO ranges.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    NumAccessRanges - Number of entries in the AccessRanges array.

    AccessRanges - Pointer to an array of AccessRanges the miniport driver
        wants to access.

Return Value:

    TRUE if our IO access checks pass,
    FALSE otherwise.

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    if (fdoExtension->AdapterInterfaceType == PCIBus) {

        //
        // Check to see if there are any IO ranges in the
        // list or resources.
        //

        ULONG i;
        USHORT Command;

        //
        // Get the PCI Command register for this device.
        //

        VideoPortGetBusData(
            HwDeviceExtension,
            PCIConfiguration,
            0,
            &Command,
            FIELD_OFFSET(PCI_COMMON_CONFIG, Command),
            sizeof(USHORT));

        for (i=0; i<NumAccessRanges; i++) {

            if (AccessRanges[i].RangeInIoSpace) {

                if (!(Command & PCI_ENABLE_IO_SPACE))
                    return FALSE;

            } else {

                if (!(Command & PCI_ENABLE_MEMORY_SPACE))
                    return FALSE;
            }
        }

        return TRUE;

    } else {

        //
        // Non-pci devices will always decode IO operations.
        //

        return TRUE;
    }
}

BOOLEAN
VpIsLegacyAccessRange(
    PFDO_EXTENSION fdoExtension,
    PVIDEO_ACCESS_RANGE AccessRange
    )

/*++

Routine Description:

    This return determines whether a given access range is
    included in the list of legacy access ranges.

Arguments:

    fdoExtension - The FDO extension for the device using the access range.

    AccessRange - The access range to look for in the resource list.

Returns:

    TRUE if the given access range is included in the list of reported
    legacy resources, FALSE otherwise.

--*/

{
    ULONG i;
    PVIDEO_ACCESS_RANGE CurrResource;

    if (fdoExtension->HwLegacyResourceList) {

        CurrResource = fdoExtension->HwLegacyResourceList;

        for (i=0; i<fdoExtension->HwLegacyResourceCount; i++) {

            if ((CurrResource->RangeStart.QuadPart ==
                 AccessRange->RangeStart.QuadPart) &&
                (CurrResource->RangeLength == AccessRange->RangeLength)) {

                return TRUE;
            }

            CurrResource++;
        }
    }

    return FALSE;
}

ULONG
GetCmResourceListSize(
    PCM_RESOURCE_LIST CmResourceList
    )

/*++

Routine Description:

    Get the size in bytes of a CmResourceList.

Arguments:

    CmResourceList - The list for which to get the size.

Returns:

    Size in bytes of the CmResourceList.

--*/

{
    PCM_FULL_RESOURCE_DESCRIPTOR    pcmFull;
    PCM_PARTIAL_RESOURCE_LIST       pcmPartial;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pcmDescript;
    ULONG i, j;

    pcmFull = &(CmResourceList->List[0]);
    for (i=0; i<CmResourceList->Count; i++) {

        pcmPartial = &(pcmFull->PartialResourceList);
        pcmDescript = &(pcmPartial->PartialDescriptors[0]);
        pcmDescript += pcmPartial->Count;
        pcmFull = (PCM_FULL_RESOURCE_DESCRIPTOR) pcmDescript;
    }

    return (ULONG)(((ULONG_PTR)pcmFull) - ((ULONG_PTR)CmResourceList));
}

PCM_RESOURCE_LIST
VpRemoveFromResourceList(
    PCM_RESOURCE_LIST OriginalList,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges
    )

/*++

Routine Description:

    Creates a new CmResourceList with the given access ranges
    removed.

Arguments:

    OriginalList - The original CmResourceList to operate on.

    NumAccessRanges - The number of entries in the remove list.

    AccessRanges - The list of ranges which should be removed from
        the list.

Returns:

    A pointer to the new CmResourceList.

Notes:

    The caller is responsible for freeing the memory returned by this
    function.

--*/

{
    PCM_RESOURCE_LIST FilteredList;
    ULONG Size = GetCmResourceListSize(OriginalList);
    ULONG remainingLength;
    ULONG ResourcesRemoved;

    FilteredList = ExAllocatePoolWithTag(PagedPool, Size, POOL_TAG);

    if (FilteredList) {

        ULONG i, j, k;
        PCM_FULL_RESOURCE_DESCRIPTOR    pcmFull;
        PCM_PARTIAL_RESOURCE_LIST       pcmPartial;
        PCM_PARTIAL_RESOURCE_DESCRIPTOR pcmDescript;

        //
        // Make a copy of the original list.
        //

        memcpy(FilteredList, OriginalList, Size);
        remainingLength = Size - sizeof(CM_RESOURCE_LIST);

        pcmFull = &(FilteredList->List[0]);
        for (i=0; i<FilteredList->Count; i++) {

            pcmPartial = &(pcmFull->PartialResourceList);
            pcmDescript = &(pcmPartial->PartialDescriptors[0]);

            ResourcesRemoved = 0;

            for (j=0; j<pcmPartial->Count; j++) {

                //
                // See if the current resource is in our legacy list.
                //

                for (k=0; k<NumAccessRanges; k++) {

                    if ((pcmDescript->u.Memory.Start.LowPart ==
                         AccessRanges[k].RangeStart.LowPart) &&
                        (AccessRanges[k].RangeStart.LowPart != 0xC0000)) {

                        //
                        // Remove the resource.
                        //

                        memmove(pcmDescript,
                                pcmDescript + 1,
                                remainingLength);

                        pcmDescript--;
                        ResourcesRemoved++;

                        break;
                    }
                }

                remainingLength -= sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
                pcmDescript++;
            }

            //
            // Update the resource count in the partial resource list
            //

            pcmPartial->Count -= ResourcesRemoved;
            if (pcmPartial->Count == 0) {
                FilteredList->Count--;
            }

            remainingLength -= sizeof(CM_PARTIAL_RESOURCE_LIST);
            pcmFull = (PCM_FULL_RESOURCE_DESCRIPTOR) pcmDescript;
        }

    } else {

        //
        // Make sure we always return a list.
        //

        ASSERT(FALSE);
        FilteredList = OriginalList;
    }

    return FilteredList;
}
