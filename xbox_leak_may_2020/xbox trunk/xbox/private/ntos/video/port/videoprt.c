/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

  videoprt.c

Abstract:

    This is the NT Video port driver.

Author:

    Andre Vachon (andreva) 18-Dec-1991

Environment:

    kernel mode only

Notes:

    This module is a driver which implements OS dependant functions on the
    behalf of the video drivers

Revision History:

--*/

#define INITGUID

#include "videoprt.h"

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT,DriverEntry)
#pragma alloc_text(PAGE,pVideoPortCreateDeviceName)
#pragma alloc_text(PAGE,pVideoPortDispatch)
#pragma alloc_text(PAGE,VideoPortFreeDeviceBase)
#pragma alloc_text(PAGE,pVideoPortFreeDeviceBase)
#pragma alloc_text(PAGE,pVideoPortGetDeviceBase)
#pragma alloc_text(PAGE,VideoPortGetDeviceBase)
#pragma alloc_text(PAGE,pVideoPortGetRegistryCallback)
#pragma alloc_text(PAGE,VideoPortGetRegistryParameters)
#pragma alloc_text(PAGE,pVPInit)
#pragma alloc_text(PAGE,VpAddDevice)
#pragma alloc_text(PAGE,VpCreateDevice)
#pragma alloc_text(PAGE,VideoPortInitialize)
#pragma alloc_text(PAGE,VideoPortFindAdapter2)
#pragma alloc_text(PAGE,VideoPortFindAdapter)
#pragma alloc_text(PAGE,pVideoPortMapToNtStatus)
#pragma alloc_text(PAGE,pVideoPortMapUserPhysicalMem)
#pragma alloc_text(PAGE,VideoPortMapMemory)
#pragma alloc_text(PAGE,VideoPortAllocateBuffer)
#pragma alloc_text(PAGE,VideoPortReleaseBuffer)
#pragma alloc_text(PAGE,VideoPortScanRom)
#pragma alloc_text(PAGE,VideoPortSetRegistryParameters)
#pragma alloc_text(PAGE,VideoPortUnmapMemory)
#pragma alloc_text(PAGE,VpEnableDisplay)
#pragma alloc_text(PAGE,VpWin32kCallout)
#pragma alloc_text(PAGE,VpAllowFindAdapter)
#if DBG
#pragma alloc_text(PAGE,BuildRequirements)
#pragma alloc_text(PAGE,DumpRequirements)
#pragma alloc_text(PAGE,DumpResourceList)
#pragma alloc_text(PAGE,DumpHwInitData)
#pragma alloc_text(PAGE,DumpUnicodeString)
#endif
#pragma alloc_text(PAGE,VideoPortQueryServices)
#pragma alloc_text(PAGE,VpInterfaceDefaultReference)
#pragma alloc_text(PAGE,VpInterfaceDefaultDereference)
#endif



NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    Temporary entry point needed to initialize the video port driver.

Arguments:

    DriverObject - Pointer to the driver object created by the system.

Return Value:

   STATUS_SUCCESS

--*/

{
    UNREFERENCED_PARAMETER(DriverObject);
    ASSERT(0);

    //
    //
    //
    //     WARNING !!!
    //
    //     This function is never called because we are loaded as a DLL by other video drivers !
    //
    //
    //
    //
    //

    //
    // We always return STATUS_SUCCESS because otherwise no video miniport
    // driver will be able to call us.
    //

    return STATUS_SUCCESS;

} // end DriverEntry()



NTSTATUS
pVideoPortCreateDeviceName(
    PWSTR           DeviceString,
    ULONG           DeviceNumber,
    PUNICODE_STRING UnicodeString,
    PWCHAR          UnicodeBuffer
    )

/*++

Routine Description:

    Helper function that does string manipulation to create a device name

--*/

{
    WCHAR          ntNumberBuffer[STRING_LENGTH];
    UNICODE_STRING ntNumberUnicodeString;

    //
    // Create the name buffer
    //

    UnicodeString->Buffer = UnicodeBuffer;
    UnicodeString->Length = 0;
    UnicodeString->MaximumLength = STRING_LENGTH;

    //
    // Create the miniport driver object name.
    //

    ntNumberUnicodeString.Buffer = ntNumberBuffer;
    ntNumberUnicodeString.Length = 0;
    ntNumberUnicodeString.MaximumLength = STRING_LENGTH;

    if (NT_SUCCESS(RtlIntegerToUnicodeString(DeviceNumber,
                                             10,
                                             &ntNumberUnicodeString))) {

        if (NT_SUCCESS(RtlAppendUnicodeToString(UnicodeString,
                                                DeviceString))) {

            if (NT_SUCCESS(RtlAppendUnicodeStringToString(UnicodeString,
                                                          &ntNumberUnicodeString))) {

                UnicodeString->MaximumLength = (USHORT)
                    (UnicodeString->Length + sizeof(UNICODE_NULL));

                return STATUS_SUCCESS;
            }
        }
    }

    return STATUS_INSUFFICIENT_RESOURCES;

} // pVideoPortCreateDeviceName()




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

VOID
VpEnableDisplay(
    BOOLEAN bState
    )

/*++

Routine Description:

    This routine enables/disables the current display so that we can execut
    the drivers FindAdapter code.

Arugments:

    bState - Should the display be enabled or disabled

Returns:

    none
--*/

{
    if (!InbvCheckDisplayOwnership()) {

        VIDEO_WIN32K_CALLBACKS_PARAMS calloutParams;

        //
        // The system is up and running.  Notify GDI to enable/disable
        // the current display.
        //

        calloutParams.CalloutType = VideoFindAdapterCallout;
        calloutParams.Param       = bState;

        VpWin32kCallout(&calloutParams);

    } else {

        //
        // The boot driver is still in control.  Modify the state of the
        // boot driver.
        //

        InbvEnableBootDriver(bState);
    }
}

VOID
VpWin32kCallout(
    PVIDEO_WIN32K_CALLBACKS_PARAMS calloutParams
    )

/*++

Routine Description:

    This routine makes a callout into win32k.  It attaches to csrss
    to guarantee that win32k is in the address space on hydra machines.

Arguments:

    calloutParams - a pointer to the callout struture.

Returns:

    none.

--*/

{

    if (Win32kCallout) {

        (*Win32kCallout)(calloutParams);
    }
}

BOOLEAN
VpAllowFindAdapter(
    PFDO_EXTENSION fdoExtension
    )

/*++

Routine Description:

    Determine if we allow this device to be used if part of a multi-function
    board.

Arguments;

    fdoExtension - The device extenstion for the object in question.

Returns:

    TRUE if the device is allowed as part of a multi-function board.
    FALSE otherwise.

--*/

{
    BOOLEAN bRet = TRUE;

    if ((fdoExtension->AdapterInterfaceType == PCIBus) &&
        ((fdoExtension->Flags & PNP_ENABLED) == PNP_ENABLED)) {

        PCI_COMMON_CONFIG ConfigSpace;

        if (PCI_COMMON_HDR_LENGTH ==
            VideoPortGetBusData(fdoExtension->HwDeviceExtension,
                                PCIConfiguration,
                                0,
                                &ConfigSpace,
                                0,
                                PCI_COMMON_HDR_LENGTH)) {


            if (PCI_MULTIFUNCTION_DEVICE(&ConfigSpace)) {

                bRet = FALSE;
            }
        }
    }

    return bRet;
}

NTSTATUS
pVideoPortDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the main dispatch routine for the video port driver.
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
    NTSTATUS finalStatus = -1 ;
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

    //
    // Synchronize execution of the dispatch routine by acquiring the device
    // event object. This ensures all request are serialized.
    // The synchronization must be done explicitly because the functions
    // executed in the dispatch routine use system services that cannot
    // be executed in the start I/O routine.
    //
    // The synchronization is done on the miniport's event so as not to
    // block commands coming in for another device.
    //

    pVideoDebugPrint((3, "pVideoPortDispatch(), wait for Sync Event, line %d\n", __LINE__));

    ACQUIRE_DEVICE_LOCK(fdoExtension);

    ASSERT(irpStack->MajorFunction != IRP_MJ_PNP);
    ASSERT(irpStack->MajorFunction != IRP_MJ_POWER);

    //
    // Case on the function being requested.
    // If the function is operating specific, intercept the operation and
    // perform directly. Otherwise, pass it on to the appropriate miniport.
    //

    switch (irpStack->MajorFunction) {

    //
    // Called by the display driver *or a user-mode application*
    // to get exclusive access to the device.
    // This access is given by the I/O system (based on a bit set during the
    // IoCreateDevice() call).
    //

    case IRP_MJ_CREATE:

        pVideoDebugPrint((2, "VideoPort - CREATE\n"));

        //
        // Don't let an old driver start during the upgrade
        //

        if (fdoExtension->Flags & UPGRADE_FAIL_START)
        {
            statusBlock->Status = STATUS_ACCESS_DENIED;
            break;
        }

        //
        // Special hack to succeed on Attach, but not do anything ...
        // That way on the close caused by the attach we will not actually
        // close the device and screw the HAL.
        //

        if (irpStack->Parameters.Create.SecurityContext->DesiredAccess ==
            FILE_READ_ATTRIBUTES) {

            statusBlock->Information = FILE_OPEN;
            statusBlock->Status = STATUS_SUCCESS;

            fdoExtension->bAttachInProgress = TRUE;

            break;

        }

        //
        // Only allow our device to be opened once!
        //

        if (fdoExtension->DeviceOpened) {

            pVideoDebugPrint((0, "Only one create allowed on this device.\n"));
            statusBlock->Status = STATUS_ACCESS_DENIED;
            break;
        }

        //
        // Mark the device as opened so we will fail future opens.
        //

        fdoExtension->DeviceOpened = TRUE;

        //
        // Tell the kernel we are now taking ownership the display.
        //

        InbvNotifyDisplayOwnershipLost(pVideoPortResetDisplay);

        //
        // Now perform basic initialization to allow the Windows display
        // driver to set up the device appropriately.
        //

        statusBlock->Information = FILE_OPEN;

        //
        // We will need to attach to the CSR process to do an int 10.
        // Save the value of that process so we can do an attach later on.
        //

        if ((fdoExtension->Flags & FINDADAPTER_SUCCEEDED) == 0) {

            statusBlock->Status = STATUS_DEVICE_CONFIGURATION_ERROR;
            pVideoDebugPrint((0, "VideoPortDispatch: START_DEVICE did not succeed\n"));

        } else if ((fdoExtension->HwInitStatus == HwInitNotCalled) &&
                   (fdoExtension->HwInitialize(fdoExtension->HwDeviceExtension) == FALSE))
        {
            statusBlock->Status = STATUS_DEVICE_CONFIGURATION_ERROR;
            fdoExtension->HwInitStatus = HwInitFailed;

            pVideoDebugPrint((0, "VideoPortDispatch: HwInitialize failed\n"));

        } else if (fdoExtension->HwInitStatus == HwInitFailed) {

            statusBlock->Status = STATUS_DEVICE_CONFIGURATION_ERROR;

        } else {

            fdoExtension->HwInitStatus = HwInitSucceeded;

            statusBlock->Status = STATUS_SUCCESS;
        }

        //
        // We don't want GDI to use any drivers other than display
        // or boot drivers during upgrade setup.
        //

        if (fdoExtension->Flags & UPGRADE_FAIL_HWINIT) {

            statusBlock->Status = STATUS_ACCESS_DENIED;
        }

        break;

    //
    // Called when the display driver wishes to give up it's handle to the
    // device.
    //

    case IRP_MJ_CLOSE:

        pVideoDebugPrint((2, "Videoprt - CLOSE\n"));

        //
        // Special hack to succeed on Attach, but not do anything ...
        // That way on the close caused by the attach we will not actually
        // close the device and screw the HAL.
        //

        if (fdoExtension->bAttachInProgress == TRUE) {

            fdoExtension->bAttachInProgress = FALSE;
            statusBlock->Status = STATUS_SUCCESS;

            break;

        }

        //
        // Allow the device to be opend again.
        //

        fdoExtension->DeviceOpened = FALSE;


        vrp.IoControlCode      = IOCTL_VIDEO_RESET_DEVICE;
        vrp.StatusBlock        = statusBlock;
        vrp.InputBuffer        = NULL;
        vrp.InputBufferLength  = 0;
        vrp.OutputBuffer       = NULL;
        vrp.OutputBufferLength = 0;

        //
        // Send the request to the miniport.
        //

        fdoExtension->HwStartIO(fdoExtension->HwDeviceExtension, &vrp);

        //
        // Override error from the miniport and return success.
        //

        statusBlock->Status = STATUS_SUCCESS;

        break;

    //
    // Device Controls are specific functions for the driver.
    // First check for calls that must be intercepted and hidden from the
    // miniport driver. These calls are hidden for simplicity.
    // The other control functions are passed down to the miniport after
    // the request structure has been filled out properly.
    //

    case IRP_MJ_DEVICE_CONTROL:

        //
        // Get the pointer to the input/output buffer and it's length
        //

        ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
        inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
        outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
        ioControlCode      = irpStack->Parameters.DeviceIoControl.IoControlCode;

        if (ioControlCode == IOCTL_VIDEO_INIT_WIN32K_CALLBACKS) {

            pVideoDebugPrint((2, "VideoPort - IOCTL_VIDEO_INIT_WIN32K_CALLBACKS\n"));

            fdoExtension->PhysDisp = ((PVIDEO_WIN32K_CALLBACKS)(ioBuffer))->PhysDisp;

            Win32kCallout = ((PVIDEO_WIN32K_CALLBACKS)(ioBuffer))->Callout;

            ((PVIDEO_WIN32K_CALLBACKS)ioBuffer)->bACPI             = FALSE;
            ((PVIDEO_WIN32K_CALLBACKS)ioBuffer)->pPhysDeviceObject = fdoExtension->PhysicalDeviceObject;

            statusBlock->Status = STATUS_SUCCESS;
            statusBlock->Information = sizeof(VIDEO_WIN32K_CALLBACKS);

        } else if (ioControlCode == IOCTL_VIDEO_IS_VGA_DEVICE) {

            pVideoDebugPrint((2, "VideoPort - IOCTL_VIDEO_IS_VGA_DEVICE\n"));

            *((PBOOLEAN)(ioBuffer)) = (BOOLEAN)(DeviceObject == DeviceOwningVga);

            statusBlock->Status = STATUS_SUCCESS;
            statusBlock->Information = sizeof(BOOLEAN);

        } else {

            //
            // All other request need to be passed to the miniport driver.
            //

            statusBlock->Status = STATUS_SUCCESS;

            switch (ioControlCode) {

#if _X86_
            case IOCTL_VIDEO_SAVE_HARDWARE_STATE:

                pVideoDebugPrint((2, "VideoPort - SaveHardwareState\n"));

                //
                // allocate the memory required by the miniport driver so it can
                // save its state to be returned to the caller.
                //

                if (fdoExtension->HardwareStateSize == 0) {

                    statusBlock->Status = STATUS_NOT_IMPLEMENTED;
                    break;

                }

                ((PVIDEO_HARDWARE_STATE)(ioBuffer))->StateLength =
                    fdoExtension->HardwareStateSize;

                statusBlock->Status =
                    ZwAllocateVirtualMemory(NtCurrentProcess(),
                                            (PVOID *) &(((PVIDEO_HARDWARE_STATE)(ioBuffer))->StateHeader),
                                            0L,
                                            &((PVIDEO_HARDWARE_STATE)(ioBuffer))->StateLength,
                                            MEM_COMMIT,
                                            PAGE_READWRITE);

                break;
#endif

            case IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES:

                pVideoDebugPrint((2, "VideoPort - QueryAccessRanges\n"));

                break;

            //
            // The default case is when the port driver does not handle the
            // request. We must then call the miniport driver.
            //

            default:

                break;


            } // switch (ioControlCode)


            //
            // All above cases call the miniport driver.
            //
            // only process it if no errors happened in the port driver
            // processing.
            //

            if (NT_SUCCESS(statusBlock->Status)) {

                pVideoDebugPrint((2, "VideoPort - default function %x\n", ioControlCode));

                vrp.IoControlCode      = ioControlCode;
                vrp.StatusBlock        = statusBlock;
                vrp.InputBuffer        = ioBuffer;
                vrp.InputBufferLength  = inputBufferLength;
                vrp.OutputBuffer       = ioBuffer;
                vrp.OutputBufferLength = outputBufferLength;

                //
                // Send the request to the miniport.
                //

                fdoExtension->HwStartIO(fdoExtension->HwDeviceExtension,
                                        &vrp);

                if (statusBlock->Status != NO_ERROR) {

                    //
                    // Make sure we don't tell the IO system to copy data
                    // on a real error.
                    //

                    if (statusBlock->Status != ERROR_MORE_DATA) {

                        ASSERT(statusBlock->Information == 0);
                        statusBlock->Information = 0;

                    }

                    pVideoPortMapToNtStatus(statusBlock);
                }
            }

        } // if (ioControlCode == ...

        break;

    //
    // Other major entry points in the dispatch routine are not supported.
    //

    default:

        statusBlock->Status = STATUS_SUCCESS;

        break;

    } // switch (irpStack->MajorFunction)

    //
    // save the final status so we can return it after the IRP is completed.
    //

    if (finalStatus == -1) {
        finalStatus = statusBlock->Status;
    }

    pVideoDebugPrint((3, "pVideoPortDispatch(), set Sync Event, line %d\n", __LINE__));

    RELEASE_DEVICE_LOCK(fdoExtension);

    if (finalStatus == STATUS_PENDING) {
        pVideoDebugPrint((1, "VideoPrt: Pending in pVideoPortDispatch.\n")) ;
        return STATUS_PENDING ;
    }

    pVideoDebugPrint((3, "Dispatch: calling IoCompleteRequest with Irp %x\n", Irp));

    IoCompleteRequest(Irp,
                      IO_VIDEO_INCREMENT);

    //
    // We never have pending operation so always return the status code.
    //

    pVideoDebugPrint((2, "VideoPort:  final IOCTL status: %08lx\n",
                     finalStatus));

    return finalStatus;

} // pVideoPortDispatch()


VOID
VideoPortFreeDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PVOID MappedAddress
    )

/*++

Routine Description:

    VideoPortFreeDeviceBase frees a block of I/O addresses or memory space
    previously mapped into the system address space by calling
    VideoPortGetDeviceBase.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    MappedAddress - Specifies the base address of the block to be freed. This
        value must be the same as the value returned by VideoPortGetDeviceBase.

Return Value:

    None.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    pVideoPortFreeDeviceBase(HwDeviceExtension, MappedAddress);
    return;
}


PVOID
pVideoPortFreeDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PVOID MappedAddress
    )
{
    PMAPPED_ADDRESS nextMappedAddress;
    PMAPPED_ADDRESS lastMappedAddress;
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    pVideoDebugPrint((2, "VPFreeDeviceBase at mapped address is %08lx\n",
                    MappedAddress));

    lastMappedAddress = NULL;
    nextMappedAddress = fdoExtension->MappedAddressList;

    while (nextMappedAddress) {

        if (nextMappedAddress->MappedAddress == MappedAddress) {

            //
            // Count up how much memory a miniport driver is really taking
            //

            if (nextMappedAddress->bNeedsUnmapping) {

                fdoExtension->MemoryPTEUsage -=
                    COMPUTE_PAGES_SPANNED(nextMappedAddress->MappedAddress,
                                          nextMappedAddress->NumberOfUchars);

            }

            //
            // BUGBUG use reference count temporarily since ATI maps too
            // large a buffer to do two maps of it.
            //

            if (!(--nextMappedAddress->RefCount)) {

                //
                // Unmap address, if necessary.
                //

                if (nextMappedAddress->bNeedsUnmapping) {

                    MmUnmapIoSpace(nextMappedAddress->MappedAddress,
                                   nextMappedAddress->NumberOfUchars);
                }

                //
                // Remove mapped address from list.
                //

                if (lastMappedAddress == NULL) {

                    fdoExtension->MappedAddressList =
                    nextMappedAddress->NextMappedAddress;

                } else {

                    lastMappedAddress->NextMappedAddress =
                    nextMappedAddress->NextMappedAddress;

                }

                ExFreePool(nextMappedAddress);

            }

            //
            // We just return the value to show that the call succeeded.
            //

            return (nextMappedAddress);

        } else {

            lastMappedAddress = nextMappedAddress;
            nextMappedAddress = nextMappedAddress->NextMappedAddress;

        }
    }

    return NULL;

} // end VideoPortFreeDeviceBase()


PVOID
VideoPortGetDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PHYSICAL_ADDRESS IoAddress,
    IN ULONG NumberOfUchars,
    IN UCHAR InIoSpace
    )

/*++

Routine Description:

    VideoPortGetDeviceBase maps a memory or I/O address range into the
    system (kernel) address space.  Access to this mapped address space
    must follow these rules:

        If the input value for InIoSpace is 1 (the address IS in I/O space),
        the returned logical address should be used in conjunction with
        VideoPort[Read/Write]Port[Uchar/Ushort/Ulong] functions.
                             ^^^^

        If the input value for InIoSpace is 0 (the address IS NOT in I/O
        space), the returned logical address should be used in conjunction
        with VideoPort[Read/Write]Register[Uchar/Ushort/Ulong] functions.
                                  ^^^^^^^^

    Note that VideoPortFreeDeviceBase is used to unmap a previously mapped
    range from the system address space.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    IoAddress - Specifies the base physical address of the range to be
        mapped in the system address space.

    NumberOfUchars - Specifies the number of bytes, starting at the base
        address, to map in system space. The driver must not access
        addresses outside this range.

    InIoSpace - Specifies that the address is in the I/O space if 1.
        Otherwise, the address is assumed to be in memory space.

Return Value:

    This function returns a base address suitable for use by the hardware
    access functions. VideoPortGetDeviceBase may be called several times
    by the miniport driver.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    //
    // We specify large page as FALSE for the default since the miniport could
    // be using the address at raise IRQL in an ISR.
    //

    return pVideoPortGetDeviceBase(HwDeviceExtension,
                                   IoAddress,
                                   NumberOfUchars,
                                   InIoSpace,
                                   FALSE);

}

PVOID
pVideoPortGetDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PHYSICAL_ADDRESS IoAddress,
    IN ULONG NumberOfUchars,
    IN UCHAR InIoSpace,
    IN BOOLEAN bLargePage
    )
{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);
    PHYSICAL_ADDRESS cardAddress = IoAddress;
    PVOID mappedAddress = NULL;
    PMAPPED_ADDRESS newMappedAddress;
    BOOLEAN bMapped;

    ULONG addressSpace;
    ULONG p6Caching = FALSE;

    pVideoDebugPrint((2, "VPGetDeviceBase reqested %08lx mem type. address is %08lx %08lx, length of %08lx\n",
                     InIoSpace, IoAddress.HighPart, IoAddress.LowPart, NumberOfUchars));

    //
    // Properly configure the flags for translation
    //

    addressSpace = InIoSpace & 0xFF;

#if defined(_X86_)

    //
    // On X86, determine if we will want to map the memory with the
    // special caching flag.
    //

    p6Caching = addressSpace & VIDEO_MEMORY_SPACE_P6CACHE;

#endif

    addressSpace &= ~VIDEO_MEMORY_SPACE_P6CACHE;

#if !defined(_ALPHA_)

    //
    // On non-alpha, this does'nt mean anything
    //

    addressSpace &= ~VIDEO_MEMORY_SPACE_DENSE;

#endif

    if (addressSpace & VIDEO_MEMORY_SPACE_USER_MODE) {
        ASSERT(FALSE);
        return NULL;
    }

    if ((((cardAddress.QuadPart >= 0x000C0000) && (cardAddress.QuadPart < 0x000C8000)) &&
         (InIoSpace == 0) &&
         (VpC0000Compatible == 2)) ||
        HalTranslateBusAddress(fdoExtension->AdapterInterfaceType,
                               fdoExtension->SystemIoBusNumber,
                               IoAddress,
                               &addressSpace,
                               &cardAddress)) {

        //
        // Use reference counting for addresses to support broken ATI !
        // Return the previously mapped address if we find the same physical
        // address.
        //

        PMAPPED_ADDRESS nextMappedAddress;

        pVideoDebugPrint((2, "VPGetDeviceBase requested %08lx mem type. physical address is %08lx %08lx, length of %08lx\n",
                         addressSpace, cardAddress.HighPart, cardAddress.LowPart, NumberOfUchars));

        nextMappedAddress = fdoExtension->MappedAddressList;

        while (nextMappedAddress) {

            if ((nextMappedAddress->InIoSpace == InIoSpace) &&
                (nextMappedAddress->NumberOfUchars == NumberOfUchars) &&
                (nextMappedAddress->PhysicalAddress.QuadPart == cardAddress.QuadPart)) {


                pVideoDebugPrint((2, "VPGetDeviceBase : refCount hit on address %08lx \n",
                                  nextMappedAddress->PhysicalAddress.LowPart));

                nextMappedAddress->RefCount++;

                //
                // Count up how much memory a miniport driver is really taking
                //

                if (nextMappedAddress->bNeedsUnmapping) {

                    fdoExtension->MemoryPTEUsage +=
                        COMPUTE_PAGES_SPANNED(nextMappedAddress->MappedAddress,
                                              nextMappedAddress->NumberOfUchars);

                }

                return (nextMappedAddress->MappedAddress);

            } else {

                nextMappedAddress = nextMappedAddress->NextMappedAddress;

            }
        }

        //
        // If the address is in IO space, don't do anything.
        // If the address is in memory space, map it and save the information.
        //

        if (addressSpace & VIDEO_MEMORY_SPACE_IO) {

            mappedAddress = (PVOID) cardAddress.QuadPart;
            bMapped = FALSE;

        } else {

            //
            // Map the device base address into the virtual address space
            //
            // NOTE: This routine is order dependant, and changing flags like
            // bLargePage will affect the caching of address we do earlier
            // on in this routine.
            //

            if (p6Caching && EnableUSWC) {

                mappedAddress = MmMapIoSpace(cardAddress,
                                             NumberOfUchars,
                                             MmFrameBufferCached);

                if (mappedAddress == NULL) {

                    mappedAddress = MmMapIoSpace(cardAddress,
                                                 NumberOfUchars,
                                                 FALSE);
                }

            } else {

                mappedAddress = MmMapIoSpace(cardAddress,
                                             NumberOfUchars,
                                             FALSE);
            }

            if (mappedAddress == NULL) {

                //
                // A failiure occured
                // BUGBUG we should log an error here !
                //

                pVideoDebugPrint((0, "VideoPort: MmMapIoSpace FAILED !!!\n"));

                return NULL;
            }

            bMapped = TRUE;

            fdoExtension->MemoryPTEUsage +=
                COMPUTE_PAGES_SPANNED(mappedAddress,
                                      NumberOfUchars);



        }

        //
        // Allocate memory to store mapped address for unmap.
        //

        newMappedAddress = ExAllocatePoolWithTag(NonPagedPool,
                                                 sizeof(MAPPED_ADDRESS),
                                                 'trpV');

        //
        // Save the reference if we can allocate memory for it. If we can
        // not, just don't save it ... it's not a big deal.
        //

        if (newMappedAddress) {

            //
            // Store mapped address information.
            //

            newMappedAddress->PhysicalAddress = cardAddress;
            newMappedAddress->RefCount = 1;
            newMappedAddress->MappedAddress = mappedAddress;
            newMappedAddress->NumberOfUchars = NumberOfUchars;
            newMappedAddress->InIoSpace = InIoSpace;
            newMappedAddress->bNeedsUnmapping = bMapped;
            newMappedAddress->bLargePageRequest = bLargePage;

            //
            // Link current list to new entry.
            //

            newMappedAddress->NextMappedAddress =
                fdoExtension->MappedAddressList;

            //
            // Point anchor at new list.
            //

            fdoExtension->MappedAddressList = newMappedAddress;

        }

    } else {

        pVideoDebugPrint((1, "HalTranslateBusAddress failed !!\n"));

    }

    pVideoDebugPrint((2, "VPGetDeviceBase mapped virtual address is %08lx\n",
                    mappedAddress));

    return mappedAddress;

} // end VideoPortGetDeviceBase()


NTSTATUS
pVideoPortGetRegistryCallback(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
)

/*++

Routine Description:

    This routine gets information from the system hive, user-specified
    registry (as opposed to the information gathered by ntdetect.

Arguments:


    ValueName - Pointer to a unicode String containing the name of the data
        value being searched for.

    ValueType - Type of the data value.

    ValueData - Pointer to a buffer containing the information to be written
        out to the registry.

    ValueLength - Size of the data being written to the registry.

    Context - Specifies a context parameter passed to the callback routine.

    EntryContext - Specifies a second context parameter passed with the
        request.

Return Value:

    STATUS_SUCCESS

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    PVP_QUERY_DEVICE queryDevice = Context;
    UNICODE_STRING unicodeString;
    OBJECT_ATTRIBUTES objectAttributes;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    HANDLE fileHandle = NULL;
    IO_STATUS_BLOCK ioStatusBlock;
    FILE_STANDARD_INFORMATION fileStandardInfo;
    PVOID fileBuffer = NULL;
    LARGE_INTEGER byteOffset;

    //
    // If the parameter was a file to be opened, perform the operation
    // here. Otherwise just return the data.
    //

    if (queryDevice->DeviceDataType == VP_GET_REGISTRY_FILE) {

        //
        // For the name of the file to be valid, we must first append
        // \DosDevices in front of it.
        //

        RtlInitUnicodeString(&unicodeString,
                             ValueData);

        InitializeObjectAttributes(&objectAttributes,
                                   &unicodeString,
                                   OBJ_CASE_INSENSITIVE,
                                   (HANDLE) NULL,
                                   (PSECURITY_DESCRIPTOR) NULL);

        ntStatus = ZwOpenFile(&fileHandle,
                              FILE_GENERIC_READ | SYNCHRONIZE,
                              &objectAttributes,
                              &ioStatusBlock,
                              0,
                              FILE_SYNCHRONOUS_IO_ALERT);

        if (!NT_SUCCESS(ntStatus)) {

            pVideoDebugPrint((1, "VideoPortGetRegistryParameters: Could not open file\n"));
            goto EndRegistryCallback;

        }

        ntStatus = ZwQueryInformationFile(fileHandle,
                                          &ioStatusBlock,
                                          &fileStandardInfo,
                                          sizeof(FILE_STANDARD_INFORMATION),
                                          FileStandardInformation);

        if (!NT_SUCCESS(ntStatus)) {

            pVideoDebugPrint((1, "VideoPortGetRegistryParameters: Could not get size of file\n"));
            goto EndRegistryCallback;

        }

        if (fileStandardInfo.EndOfFile.HighPart) {

            //
            // If file is too big, do not try to go further.
            //

            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto EndRegistryCallback;

        }

        ValueLength = fileStandardInfo.EndOfFile.LowPart;

        fileBuffer = ExAllocatePoolWithTag(PagedPool,
                                           ValueLength,
                                           POOL_TAG);

        if (!fileBuffer) {

            pVideoDebugPrint((1, "VideoPortGetRegistryParameters: Could not allocate buffer to read file\n"));
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;

            goto EndRegistryCallback;

        }

        ValueData = fileBuffer;

        //
        // Read the entire file for the beginning.
        //

        byteOffset.QuadPart = 0;

        ntStatus = ZwReadFile(fileHandle,
                              NULL,
                              NULL,
                              NULL,
                              &ioStatusBlock,
                              ValueData,
                              ValueLength,
                              &byteOffset,
                              NULL);

        if (!NT_SUCCESS(ntStatus)) {

            pVideoDebugPrint((1, "VideoPortGetRegistryParameters: Could not read file\n"));
            goto EndRegistryCallback;

        }

    }

    //
    // Call the miniport with the appropriate information.
    //

    queryDevice->MiniportStatus = ((PMINIPORT_GET_REGISTRY_ROUTINE)
               queryDevice->CallbackRoutine) (queryDevice->MiniportHwDeviceExtension,
                                              queryDevice->MiniportContext,
                                              ValueName,
                                              ValueData,
                                              ValueLength);

EndRegistryCallback:

    if (fileHandle) {

        ZwClose(fileHandle);

    }

    if (fileBuffer) {

        ExFreePool(fileBuffer);

    }

    return ntStatus;

} // end pVideoPortGetRegistryCallback()



VP_STATUS
VideoPortGetRegistryParameters(
    PVOID HwDeviceExtension,
    PWSTR ParameterName,
    UCHAR IsParameterFileName,
    PMINIPORT_GET_REGISTRY_ROUTINE CallbackRoutine,
    PVOID Context
    )

/*++

Routine Description:

    VideoPortGetRegistryParameters retrieves information from the
    CurrentControlSet in the registry.  The function automatically searches
    for the specified parameter name under the \Devicexxx key for the
    current driver.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    ParameterName - Points to a Unicode string that contains the name of the
        data value being searched for in the registry.

    IsParameterFileName - If 1, the data retrieved from the requested
        parameter name is treated as a file name.  The contents of the file are
        returned, instead of the parameter itself.

    CallbackRoutine - Points to a function that should be called back with
        the requested information.

    Context - Specifies a context parameter passed to the callback routine.

Return Value:

    This function returns the final status of the operation.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{

    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);
    RTL_QUERY_REGISTRY_TABLE queryTable[2];
    NTSTATUS ntStatus;
    VP_QUERY_DEVICE queryDevice;

    queryDevice.MiniportHwDeviceExtension = HwDeviceExtension;
    queryDevice.DeviceDataType = IsParameterFileName ? VP_GET_REGISTRY_FILE : VP_GET_REGISTRY_DATA;
    queryDevice.CallbackRoutine = CallbackRoutine;
    queryDevice.MiniportStatus = NO_ERROR;
    queryDevice.MiniportContext = Context;

    //
    // BUGBUG
    // Can be simplified now since we don't have to go down a directory.
    // It can be just one call.
    //

    queryTable[0].QueryRoutine = pVideoPortGetRegistryCallback;
    queryTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED;
    queryTable[0].Name = ParameterName;
    queryTable[0].EntryContext = NULL;
    queryTable[0].DefaultType = REG_NONE;
    queryTable[0].DefaultData = 0;
    queryTable[0].DefaultLength = 0;

    queryTable[1].QueryRoutine = NULL;
    queryTable[1].Flags = 0;
    queryTable[1].Name = NULL;
    queryTable[1].EntryContext = NULL;
    queryTable[1].DefaultType = REG_NONE;
    queryTable[1].DefaultData = 0;
    queryTable[1].DefaultLength = 0;

    ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE,
                                      fdoExtension->DriverRegistryPath,
                                      queryTable,
                                      &queryDevice,
                                      NULL);

    if (!NT_SUCCESS(ntStatus)) {

        queryDevice.MiniportStatus = ERROR_INVALID_PARAMETER;

    }

    return queryDevice.MiniportStatus;

} // end VideoPortGetRegistryParameters()


VOID
pVPInit(
    VOID
    )

/*++

Routine Description:

    First time initialization of the video port.

    Normally, this is the stuff we should put in the DriverEntry routine.
    However, the video port is being loaded as a DLL, and the DriverEntry
    is never called.  It would just be too much work to add it back to the hive
    and setup.

    This little routine works just as well.

--*/

{

    UNICODE_STRING UnicodeString;
    OBJECT_ATTRIBUTES ObjectAttributes;
    NTSTATUS ntStatus;
    HANDLE physicalMemoryHandle = NULL;

    HAL_DISPLAY_BIOS_INFORMATION HalBiosInfo;
    ULONG HalBiosInfoLen = sizeof(ULONG);

    SYSTEM_BASIC_INFORMATION basicInfo;

#if defined(_X86_)

    //
    // Check for USWC disabling
    //

    EnableUSWC = TRUE;

#else

    EnableUSWC = FALSE;

#endif

    //
    // Determine if we have a VGA compatible machine
    //

    ntStatus = STATUS_SUCCESS;
    HalBiosInfo = HalDisplayInt10Bios;

    if (NT_SUCCESS(ntStatus)) {

        if (HalBiosInfo == HalDisplayInt10Bios) {

            VpC0000Compatible = 2;

        } else {

            // == HalDisplayEmulatedBios,
            // == HalDisplayNoBios

            VpC0000Compatible = 0;
        }

    } else {

        //
        // In case of an error in the API call, we just assume it's an old HAL
        // and use the old behaviour of the video port which is to assume
        // there is a BIOS at C000
        //

        VpC0000Compatible = 1;
    }


    //
    // Lets open the physical memory section just once, for all drivers.
    //

    //
    // Get a pointer to physical memory so we can map the
    // video frame buffer (and possibly video registers) into
    // the caller's address space whenever he needs it.
    //
    // - Create the name
    // - Initialize the data to find the object
    // - Open a handle to the oject and check the status
    // - Get a pointer to the object
    // - Free the handle
    //

    RtlInitUnicodeString(&UnicodeString,
                         L"\\Device\\PhysicalMemory");

    InitializeObjectAttributes(&ObjectAttributes,
                               &UnicodeString,
                               OBJ_CASE_INSENSITIVE,
                               (HANDLE) NULL,
                               (PSECURITY_DESCRIPTOR) NULL);

    ntStatus = ZwOpenSection(&physicalMemoryHandle,
                             SECTION_ALL_ACCESS,
                             &ObjectAttributes);

    if (NT_SUCCESS(ntStatus)) {

        ntStatus = ObReferenceObjectByHandle(physicalMemoryHandle,
                                             SECTION_ALL_ACCESS,
                                             (POBJECT_TYPE) NULL,
                                             KernelMode,
                                             &PhysicalMemorySection,
                                             (POBJECT_HANDLE_INFORMATION) NULL);

        if (!NT_SUCCESS(ntStatus)) {

            pVideoDebugPrint((1, "pVPInit: Could not reference physical memory\n"));
            ASSERT(PhysicalMemorySection == NULL);

        }

        ZwClose(physicalMemoryHandle);
    }

    VpSystemMemorySize = 0;

    ntStatus = ZwQuerySystemInformation(SystemBasicInformation,
                                        &basicInfo,
                                        sizeof(basicInfo),
                                        NULL);

    if (NT_SUCCESS(ntStatus)) {

        VpSystemMemorySize
            = (ULONGLONG)basicInfo.NumberOfPhysicalPages * (ULONGLONG)basicInfo.PageSize;
    }
}


VP_STATUS
VpRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )

{
    if (ValueLength && ValueData) {

        *((PULONG)Context) = *((PULONG)ValueData);

        return NO_ERROR;

    } else {

        return ERROR_INVALID_PARAMETER;
    }
}

NTSTATUS
VpAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
)
{
    NTSTATUS            ntStatus;
    PDEVICE_OBJECT      functionalDeviceObject;
    PDEVICE_OBJECT      attachedTo;
    PFDO_EXTENSION      fdoExtension;
    ULONG               extensionAllocationSize;
    PVIDEO_PORT_DRIVER_EXTENSION DriverObjectExtension;
    PVIDEO_HW_INITIALIZATION_DATA HwInitializationData;

    pVideoDebugPrint((1, "VpAddDevice\n"));

    DriverObjectExtension = (PVIDEO_PORT_DRIVER_EXTENSION)
                      IoGetDriverObjectExtension(DriverObject,
                                                 DriverObject);

    HwInitializationData = &DriverObjectExtension->HwInitData;

    extensionAllocationSize = HwInitializationData->HwDeviceExtensionSize +
                                  sizeof(FDO_EXTENSION);

    ntStatus = VpCreateDevice(DriverObject,
                              extensionAllocationSize,
                              &functionalDeviceObject);

    if (NT_SUCCESS(ntStatus)) {

        VideoDeviceNumber++;

        fdoExtension = (PFDO_EXTENSION)functionalDeviceObject->DeviceExtension;

        //
        // Set any deviceExtension fields here that are PnP specific
        //

        fdoExtension->PhysicalDeviceObject = PhysicalDeviceObject;

        //
        // Since the pnp system is notifying us of our device, this is
        // not a legacy device.
        //

        fdoExtension->Flags = PNP_ENABLED;

        //
        // Now attach to the PDO we were given.
        //

        attachedTo = IoAttachDeviceToDeviceStack(functionalDeviceObject,
                                                 PhysicalDeviceObject);

        if (attachedTo == NULL) {

            ASSERT(attachedTo != NULL);

            //
            // Couldn't attach.  Delete the FDO.
            //

            // BUGBUG
            //IoDeleteDevice(functionalDeviceObject);
            //return STATUS_NO_SUCH_DEVICE;
        }

        fdoExtension->AttachedDeviceObject = attachedTo;

        //
        // Set the power management flag indicating that device mapping
        // has not been done yet.
        //

        fdoExtension->IsMappingReady = FALSE ;

        //
        // Clear DO_DEVICE_INITIALIZING flag.
        //

        functionalDeviceObject->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;
        functionalDeviceObject->Flags &= ~(DO_DEVICE_INITIALIZING | DO_POWER_INRUSH);

        //
        // Save the function pointers to the new 5.0 miniport driver callbacks.
        //

        if (HwInitializationData->HwInitDataSize >
            FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwQueryInterface)) {

            fdoExtension->HwSetPowerState  = HwInitializationData->HwSetPowerState;
            fdoExtension->HwGetPowerState  = HwInitializationData->HwGetPowerState;
            fdoExtension->HwQueryInterface = HwInitializationData->HwQueryInterface;
        }
    }

    pVideoDebugPrint((1, "*\n* VpAddDevice returned: 0x%x\n*\n", ntStatus));

    return ntStatus;
}


NTSTATUS
VpCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    OUT PDEVICE_OBJECT *FunctionalDeviceObject
)
{
    WCHAR deviceNameBuffer[STRING_LENGTH];
    UNICODE_STRING deviceNameUnicodeString;
    DEVICE_OBJECT fdo;
    NTSTATUS ntStatus;
    PFDO_EXTENSION fdoExtension;

    ntStatus = pVideoPortCreateDeviceName(L"\\Device\\Video",
                                          VideoDeviceNumber,
                                          &deviceNameUnicodeString,
                                          deviceNameBuffer);

    //
    // Create a device object to represent the Video Adapter.
    //

    if (NT_SUCCESS(ntStatus)) {

        ntStatus = IoCreateDevice(DriverObject,
                                  DeviceExtensionSize,
                                  &deviceNameUnicodeString,
                                  FILE_DEVICE_VIDEO,
                                  0,
                                  TRUE,
                                  FunctionalDeviceObject);

        if (NT_SUCCESS(ntStatus)) {

            (*FunctionalDeviceObject)->DeviceType = FILE_DEVICE_VIDEO;
            fdoExtension = (*FunctionalDeviceObject)->DeviceExtension;

            //
            // Set any deviceExtension fields here
            //

            fdoExtension->DeviceNumber = VideoDeviceNumber;
            fdoExtension->FunctionalDeviceObject = *FunctionalDeviceObject;

            KeInitializeMutex(&fdoExtension->SyncMutex,
                              0);

#if DBG
            fdoExtension->AllocationHead = (PALLOC_ENTRY) NULL;
            ExInitializePagedLookasideList(&fdoExtension->AllocationList,
                                           NULL,
                                           NULL,
                                           0,
                                           sizeof(ALLOC_ENTRY),
                                           'LdiV',
                                           0);
#endif

        }
    }

    return ntStatus;
}

ULONG
VideoPortInitialize(
    IN PVOID Argument1,  // DriverObject
    IN PVOID Argument2,  // RegistryPath
    IN PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    IN PVOID HwContext
    )
{
    PDRIVER_OBJECT driverObject = Argument1;
    NTSTATUS ntStatus;
    PUNICODE_STRING   registryPath = (PUNICODE_STRING) Argument2;
    ULONG PnpFlags;

    if (VPFirstTime)
    {
        VPFirstTime = FALSE;
        pVPInit();
    }

    //
    // Check if the size of the pointer, or the size of the data passed in
    // are OK.
    //

    ASSERT(HwInitializationData != NULL);

    if (HwInitializationData->HwInitDataSize >
        sizeof(VIDEO_HW_INITIALIZATION_DATA) ) {

        pVideoDebugPrint((0, "VideoPortInitialize: Invalid initialization data size\n"));
        return ((ULONG) STATUS_REVISION_MISMATCH);

    }

    //
    // Check that each required entry is not NULL.
    //

    if ((!HwInitializationData->HwFindAdapter) ||
        (!HwInitializationData->HwInitialize) ||
        (!HwInitializationData->HwStartIO)) {

        pVideoDebugPrint((1, "VideoPortInitialize: miniport missing required entry\n"));
        return ((ULONG)STATUS_REVISION_MISMATCH);

    }

    //
    // Set up the device driver entry points.
    //

    driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = pVideoPortDispatch;
    driverObject->MajorFunction[IRP_MJ_CREATE]         = pVideoPortDispatch;
    driverObject->MajorFunction[IRP_MJ_CLOSE]          = pVideoPortDispatch;

    //
    // Check that the device extension size is reasonable.
    //

#if DBG
    if (HwInitializationData->HwDeviceExtensionSize > 0x4000) {
        pVideoDebugPrint((0, "Warning: Device Extension is stored in non-paged pool\n"
                             "         Do you need a 0x%x byte device extension?\n",
                             HwInitializationData->HwDeviceExtensionSize));
    }
#endif

    //
    // Check the registry for PnP Flags.  Currently we recongnize the
    // following values:
    //
    // PnPEnabled -   If this value is set with a non-zero value, we
    //                will treat behave like a PnP driver.
    //
    // LegacyDetect - If this value is non-zero, we will report
    //                a non-pci device to the system via
    //                IoReportDetectedDevice.
    //
    // If we don't get the flags, we don't know how to run this driver.
    // return failure
    //

    if (!(NT_SUCCESS(VpGetFlags(registryPath,
                                HwInitializationData,
                                &PnpFlags))))
    {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // PnP drivers have new rules.
    //

    if (PnpFlags & PNP_ENABLED)
    {
        pVideoDebugPrint((1, "videoprt : VideoPortInitialize with PNP_ENABLED\n"));

        //
        // We also can't be plug and play compatible if the driver passes
        // info in HwContext.  This is because we can't store this.
        //

        if ((PnpFlags & VGA_DRIVER) ||
            (HwContext != NULL))
        {
            pVideoDebugPrint((0, "videoprt : This video driver can not be PnP !\n"));
            ASSERT(FALSE);
            return STATUS_INVALID_PARAMETER;
        }

    } else {

        //
        // Only allow a non-pnp driver to install before win32k has started.
        //

        if (!InbvCheckDisplayOwnership()) {

            pVideoDebugPrint((0, "We can't dynamically start a non PnP device.\n"));
#if defined STATUS_REBOOT_REQUIRED
            return STATUS_REBOOT_REQUIRED;
#else
            return STATUS_INVALID_PARAMETER;
#endif
        }
    }

    //
    // Never do legacy detection of PnP drivers on the PCI Bus.
    //

    if (HwInitializationData->AdapterInterfaceType == PCIBus) {

        pVideoDebugPrint((1, "videoprt : VideoPortInitialize on PCI Bus\n"));

        if ( (PnpFlags & PNP_ENABLED) &&
             ((PnpFlags & LEGACY_DETECT) ||
              (PnpFlags & REPORT_DEVICE)) ) {

            pVideoDebugPrint((0, "videoprt : Trying to detect PnP driver on PCI - fail\n"));
            return STATUS_INVALID_PARAMETER;
        }
    }


    //
    // Set this information for all PnP Drivers
    //
    // Special !!! - we cannot do this in the LEGACY_DETECT because the system
    // will think we failed to load and return a failure code.
    //

    if ( (PnpFlags & PNP_ENABLED) &&
         (!(PnpFlags & LEGACY_DETECT)) )
    {
        PVIDEO_PORT_DRIVER_EXTENSION DriverObjectExtension;

        pVideoDebugPrint((1, "videoprt : We have a PnP Device.\n"));

        //
        // Fill in the new PnP entry points.
        //

        driverObject->DriverExtension->AddDevice  = VpAddDevice;
        driverObject->MajorFunction[IRP_MJ_PNP]   = pVideoPortPnpDispatch;

        //
        // we'll do findadapter during the START_DEVICE irp
        //
        // Store away arguments, so we can retrieve them when we need them.
        //
        // Try to create a DriverObjectExtension
        //

        if (DriverObjectExtension = (PVIDEO_PORT_DRIVER_EXTENSION)
                      IoGetDriverObjectExtension(driverObject,
                                                 driverObject))
        {
            DriverObjectExtension->HwInitData = *HwInitializationData;
            ntStatus = STATUS_SUCCESS;
        }
        else if (NT_SUCCESS(IoAllocateDriverObjectExtension(
                                driverObject,
                                driverObject,
                                sizeof(VIDEO_PORT_DRIVER_EXTENSION),
                                &DriverObjectExtension)))
        {

            DriverObjectExtension->RegistryPath = *registryPath;
            DriverObjectExtension->RegistryPath.MaximumLength += sizeof(WCHAR);
            DriverObjectExtension->RegistryPath.Buffer =
                ExAllocatePoolWithTag(PagedPool,
                                      DriverObjectExtension->RegistryPath.MaximumLength,
                                      'trpV');

            ASSERT(DriverObjectExtension->RegistryPath.Buffer);

            RtlCopyUnicodeString(&(DriverObjectExtension->RegistryPath),
                                 registryPath);

            DriverObjectExtension->HwInitData = *HwInitializationData;
            ntStatus = STATUS_SUCCESS;
        }
        else
        {
            //
            // Something went wrong.  We should have a
            // DriverObjectExtension by now.
            //

            pVideoDebugPrint((0, "IoAllocateDriverExtensionObject failed!\n"));
            pVideoDebugPrint((0, "for registry path %ws!\n", registryPath->Buffer));

            ASSERT(FALSE);

            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

    }


    //
    // If we are doing legacy detection or reporting, create the FDO
    // right now ...
    //

    if ((!(PnpFlags & PNP_ENABLED))  ||
         (PnpFlags & LEGACY_DETECT)  ||
         (PnpFlags & VGA_DRIVER)     ||
         (PnpFlags & REPORT_DEVICE)  ||
         (HwContext != NULL)) {

        ntStatus = STATUS_NO_SUCH_DEVICE;
    }

    return ntStatus;
}


NTSTATUS
VideoPortFindAdapter(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID Argument2,
    IN PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    IN PVOID HwContext,
    PDEVICE_OBJECT DeviceObject,
    PUCHAR nextMiniport
    )
{
    NTSTATUS status;
    PFDO_EXTENSION fdoExtension = DeviceObject->DeviceExtension;

    status = VideoPortFindAdapter2(DriverObject,
                                   Argument2,
                                   HwInitializationData,
                                   HwContext,
                                   DeviceObject,
                                   nextMiniport);

    if (NT_SUCCESS(status))
    {
        //
        // Mark this object as supporting buffered I/O so that the I/O system
        // will only supply simple buffers in IRPs.
        //
        // Set and clear the two power fields to ensure we only get called
        // as passive level to do power management operations.
        //
        // Finally, tell the system we are done with Device Initialization
        //

        DeviceObject->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;
        DeviceObject->Flags &= ~(DO_DEVICE_INITIALIZING | DO_POWER_INRUSH);

        fdoExtension->Flags |= FINDADAPTER_SUCCEEDED;
    }

    return status;
}


NTSTATUS
VideoPortFindAdapter2(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID Argument2,
    IN PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    IN PVOID HwContext,
    PDEVICE_OBJECT DeviceObject,
    PUCHAR nextMiniport
    )

{
    WCHAR deviceNameBuffer[STRING_LENGTH];
    POBJECT_NAME_INFORMATION deviceName;
    ULONG strLength;

    NTSTATUS ntStatus;
    WCHAR deviceSubpathBuffer[STRING_LENGTH];
    UNICODE_STRING deviceSubpathUnicodeString;
    WCHAR deviceLinkBuffer[STRING_LENGTH];
    UNICODE_STRING deviceLinkUnicodeString;
    KAFFINITY affinity;

    PVIDEO_PORT_CONFIG_INFO miniportConfigInfo = NULL;
    PDEVICE_OBJECT deviceObject;
    PFDO_EXTENSION fdoExtension;
    VP_STATUS findAdapterStatus = ERROR_DEV_NOT_EXIST;
    ULONG driverKeySize;
    PWSTR driverKeyName = NULL;
    BOOLEAN symbolicLinkCreated = FALSE;

    PDEVICE_OBJECT pdo;

    ntStatus = STATUS_NO_SUCH_DEVICE;

    deviceObject = DeviceObject;
    fdoExtension = deviceObject->DeviceExtension;

    pdo = fdoExtension->PhysicalDeviceObject;

    deviceName = (POBJECT_NAME_INFORMATION) deviceNameBuffer;

    ObQueryNameString(deviceObject,
                      deviceName,
                      STRING_LENGTH * sizeof(WCHAR),
                      &strLength);

    //
    // Allocate the buffer in which the miniport driver will store all the
    // configuration information.
    //

    miniportConfigInfo = (PVIDEO_PORT_CONFIG_INFO)
                             ExAllocatePoolWithTag(PagedPool,
                                                   sizeof(VIDEO_PORT_CONFIG_INFO),
                                                   POOL_TAG);

    if (miniportConfigInfo == NULL) {

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto EndOfInitialization;
    }

    RtlZeroMemory((PVOID) miniportConfigInfo,
                  sizeof(VIDEO_PORT_CONFIG_INFO));

    miniportConfigInfo->Length = sizeof(VIDEO_PORT_CONFIG_INFO);

    //
    // Put in the BusType specified within the HW_INITIALIZATION_DATA
    // structure by the miniport and the bus number inthe miniport config info.
    //

    miniportConfigInfo->SystemIoBusNumber = fdoExtension->SystemIoBusNumber;
    miniportConfigInfo->AdapterInterfaceType = fdoExtension->AdapterInterfaceType;

    //
    // Initialize the pointer to VpGetProcAddress.
    //

    miniportConfigInfo->VideoPortGetProcAddress = VpGetProcAddress;

    //
    // Initialize the type of interrupt based on the bus type.
    //

    switch (miniportConfigInfo->AdapterInterfaceType) {

    case Internal:
    case MicroChannel:
    case PCIBus:

        miniportConfigInfo->InterruptMode = LevelSensitive;
        break;

    default:

        miniportConfigInfo->InterruptMode = Latched;
        break;

    }

    //
    // Set up device extension pointers and sizes
    //

    fdoExtension->HwDeviceExtension = (PVOID)(fdoExtension + 1);
    fdoExtension->HwDeviceExtensionSize =
        HwInitializationData->HwDeviceExtensionSize;
    fdoExtension->MiniportConfigInfo = miniportConfigInfo;

    //
    // Save the dependent driver routines in the device extension.
    //

    fdoExtension->HwFindAdapter = HwInitializationData->HwFindAdapter;
    fdoExtension->HwInitialize = HwInitializationData->HwInitialize;
    fdoExtension->HwInterrupt = HwInitializationData->HwInterrupt;
    fdoExtension->HwStartIO = HwInitializationData->HwStartIO;

    if (HwInitializationData->HwInitDataSize >
        FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwLegacyResourceCount)) {

        fdoExtension->HwLegacyResourceList = HwInitializationData->HwLegacyResourceList;
        fdoExtension->HwLegacyResourceCount = HwInitializationData->HwLegacyResourceCount;
    }

    if (HwInitializationData->HwInitDataSize >
        FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, AllowEarlyEnumeration)) {

        fdoExtension->AllowEarlyEnumeration = HwInitializationData->AllowEarlyEnumeration;
    }

    //
    // Create the name we will be storing in the \DeviceMap.
    // This name is a PWSTR, not a unicode string
    // This is the name of the driver with an appended device number
    //

    if (!NT_SUCCESS(pVideoPortCreateDeviceName(L"\\Device",
                                               HwInitializationData->StartingDeviceNumber,
                                               &deviceSubpathUnicodeString,
                                               deviceSubpathBuffer))) {

        pVideoDebugPrint((1, "VideoPortInitialize: Could not create device subpath number\n"));
        goto EndOfInitialization;

    }

    fdoExtension->DriverRegistryPathLength =
        ((PUNICODE_STRING)Argument2)->Length +
        deviceSubpathUnicodeString.Length;

    driverKeySize = fdoExtension->DriverRegistryPathLength +
                    2 * sizeof(UNICODE_NULL);

    if ( (driverKeyName = (PWSTR) ExAllocatePoolWithTag(PagedPool,
                                                        driverKeySize,
                                                        POOL_TAG) ) == NULL) {

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto EndOfInitialization;
    }

    RtlMoveMemory(driverKeyName,
                  ((PUNICODE_STRING)Argument2)->Buffer,
                  ((PUNICODE_STRING)Argument2)->Length);

    RtlMoveMemory((PWSTR)( (ULONG_PTR)driverKeyName +
                           ((PUNICODE_STRING)Argument2)->Length ),
                  deviceSubpathBuffer,
                  deviceSubpathUnicodeString.Length);

    //
    // Put two NULLs at the end so we can play around with the string later.
    //

    *((PWSTR) ((ULONG_PTR)driverKeyName +
        fdoExtension->DriverRegistryPathLength)) = UNICODE_NULL;
    *((PWSTR) ((ULONG_PTR)driverKeyName +
        (fdoExtension->DriverRegistryPathLength + sizeof(UNICODE_NULL)))) =
                                                      UNICODE_NULL;

    //
    // There is a bug in Lotus Screen Cam where it will only work if our
    // reg path is \REGISTRY\Machine\System not \REGISTRY\MACHINE\SYSTEM.
    // so replace the appropriate strings.
    //

    if (wcsstr(driverKeyName, L"MACHINE")) {
        wcsncpy(wcsstr(driverKeyName, L"MACHINE"), L"Machine", sizeof("Machine")-1);
    }

    if (wcsstr(driverKeyName, L"SYSTEM")) {
        wcsncpy(wcsstr(driverKeyName, L"SYSTEM"), L"System", sizeof("System")-1);
    }

    //
    // Store the path name of the location of the driver in the registry.
    //

    fdoExtension->DriverRegistryPath = driverKeyName;
    miniportConfigInfo->DriverRegistryPath = driverKeyName;

    //
    // Let the driver know how much system memory is present.
    //

    miniportConfigInfo->SystemMemorySize = VpSystemMemorySize;

    //
    // Turn on the debug level based on the miniport driver entry
    //

    VideoPortGetRegistryParameters(fdoExtension->HwDeviceExtension,
                                   L"VideoDebugLevel",
                                   FALSE,
                                   VpRegistryCallback,
                                   &VideoDebugLevel);

    //
    // Obtain the override value for memory allocation from the registry
    // if present. Otherwise, go with the default.
    //

#if DBG
    //
    // Turn on the debug level based on the miniport driver entry
    //

    fdoExtension->FreeAllocation = MAXIMUM_MEM_LIMIT_K * 1024 ;
    fdoExtension->AllocationHead = (PALLOC_ENTRY) NULL ;

    //
    // Set up the default maximum allocation.  This will change
    // if there is a value in the registry.
    //

    fdoExtension->FreeAllocation = MAXIMUM_MEM_LIMIT_K * 1024 ;

    VideoPortGetRegistryParameters(fdoExtension->HwDeviceExtension,
                                   L"MaxAllocationLimit",
                                   FALSE,
                                   VpRegistryCallback,
                                   &(fdoExtension->FreeAllocation));

#endif

    if (VpAllowFindAdapter(fdoExtension)) {

        //
        // Notify the boot driver that we will be accessing the display
        // hardware.
        //

        VpEnableDisplay(FALSE);

#if DBG
        //
        // Mark the resources as not reported so we can detect if a miniport
        // uses an access range before reporting it.
        //

        InterlockedExchange(&VPResourcesReported, FALSE);

        //
        // The driver doesn't have to aquire resources if it claimed
        // them during DriverEntry.
        //

        if (fdoExtension->HwLegacyResourceCount) {
            InterlockedIncrement(&VPResourcesReported);
        }
#endif

        findAdapterStatus =
            fdoExtension->HwFindAdapter(fdoExtension->HwDeviceExtension,
                                        HwContext,
                                        NULL, // BUGBUG What is this string?
                                        miniportConfigInfo,
                                        nextMiniport);

#if DBG
        //
        // By now, the resources should have been claimed.
        //

        InterlockedIncrement(&VPResourcesReported);
#endif

        VpEnableDisplay(TRUE);
    }

    //
    // If the adapter is not found, display an error.
    //

    if (findAdapterStatus != NO_ERROR) {

        pVideoDebugPrint((1, "VideoPortFindAdapter: Find adapter failed\n"));

        ntStatus = STATUS_UNSUCCESSFUL;
        goto EndOfInitialization;

    }

    //
    // Store the required information in the device extension for later use.
    //

    fdoExtension->VdmPhysicalVideoMemoryAddress =
        miniportConfigInfo->VdmPhysicalVideoMemoryAddress;

    fdoExtension->VdmPhysicalVideoMemoryLength =
        miniportConfigInfo->VdmPhysicalVideoMemoryLength;

    fdoExtension->HardwareStateSize =
        miniportConfigInfo->HardwareStateSize;

    //
    // If the device supplies an interrupt service routine, we must
    // set up all the structures to support interrupts. Otherwise,
    // they can be ignored.
    //

    if (fdoExtension->HwInterrupt &&
        ((miniportConfigInfo->BusInterruptLevel != 0) ||
         (miniportConfigInfo->BusInterruptVector != 0)) ) {

        affinity = fdoExtension->InterruptAffinity;

        fdoExtension->InterruptMode = miniportConfigInfo->InterruptMode;

        fdoExtension->InterruptsEnabled = TRUE;

        ntStatus = IoConnectInterrupt(&fdoExtension->InterruptObject,
                                      (PKSERVICE_ROUTINE) pVideoPortInterrupt,
                                      deviceObject,
                                      NULL,
                                      fdoExtension->InterruptVector,
                                      fdoExtension->InterruptIrql,
                                      fdoExtension->InterruptIrql,
                                      fdoExtension->InterruptMode,
                                      (BOOLEAN) ((miniportConfigInfo->InterruptMode ==
                                          LevelSensitive) ? TRUE : FALSE),
                                      affinity,
                                      FALSE);

        if (!NT_SUCCESS(ntStatus)) {

            pVideoDebugPrint((0, "VideoPortInitialize: Can't connect interrupt\n"));
            goto EndOfInitialization;
        }

    } else {

        fdoExtension->HwInterrupt = NULL;

    }

    //
    // Initialize DPC Support
    //

    KeInitializeDpc(&fdoExtension->Dpc,
                    pVideoPortDpcDispatcher,
                    fdoExtension->HwDeviceExtension);

    //
    // New, Optional.
    // Setup the timer if it is specified by a driver.
    //

    if (HwInitializationData->HwInitDataSize >
        FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwTimer)){

        fdoExtension->HwTimer = HwInitializationData->HwTimer;

        if (fdoExtension->HwTimer) {
            ntStatus = IoInitializeTimer(deviceObject,
                                         pVideoPortHwTimer,
                                         NULL);

            //
            // If we fail forget about the timer !
            //

            if (!NT_SUCCESS(ntStatus)) {

                ASSERT(FALSE);
                fdoExtension->HwTimer = NULL;

            }
        }
    }

    //
    // New, Optional.
    // Reset Hw function.
    //

    if (HwInitializationData->HwInitDataSize >
        FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwResetHw)) {

        ULONG iReset;

        for (iReset=0; iReset<6; iReset++) {

            if (HwResetHw[iReset].ResetFunction == NULL) {

                HwResetHw[iReset].ResetFunction = HwInitializationData->HwResetHw;
                HwResetHw[iReset].HwDeviceExtension = fdoExtension->HwDeviceExtension;

                break;
            }
        }
    }

    //
    // NOTE:
    //
    // We only want to reinitialize the device once the Boot sequence has
    // been completed and the HAL does not need to access the device again.
    // So the initialization entry point will be called when the device is
    // opened.
    //


    if (!NT_SUCCESS(pVideoPortCreateDeviceName(L"\\DosDevices\\DISPLAY",
                                               fdoExtension->DeviceNumber + 1,
                                               &deviceLinkUnicodeString,
                                               deviceLinkBuffer))) {

        pVideoDebugPrint((1, "VideoPortInitialize: Could not create device subpath number\n"));
        goto EndOfInitialization;

    }

    ntStatus = IoCreateSymbolicLink(&deviceLinkUnicodeString,
                                    &deviceName->Name);


    if (!NT_SUCCESS(ntStatus)) {

        pVideoDebugPrint((0, "VideoPortInitialize: SymbolicLink Creation failed\n"));
        goto EndOfInitialization;

    }

    symbolicLinkCreated = TRUE;

    //
    // Save the function pointers to the new 5.0 miniport driver callbacks.
    //

    if (HwInitializationData->HwInitDataSize >
        FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwQueryInterface)) {

        fdoExtension->HwSetPowerState  = HwInitializationData->HwSetPowerState;
        fdoExtension->HwGetPowerState  = HwInitializationData->HwGetPowerState;
        fdoExtension->HwQueryInterface = HwInitializationData->HwQueryInterface;
    }

    if (HwInitializationData->HwInitDataSize >
        FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwChildDeviceExtensionSize)) {

        fdoExtension->HwChildDeviceExtensionSize =
            HwInitializationData->HwChildDeviceExtensionSize;
    }


EndOfInitialization:

    //
    // If we are doing detection, then don't save all of these objects.
    // We just want to see if the driver would load or not
    //

    if ( (fdoExtension->Flags & LEGACY_DETECT) ||
         (!NT_SUCCESS(ntStatus)) )
    {
        //
        // Free the miniport config info buffer.
        //

        if (miniportConfigInfo) {
            ExFreePool(miniportConfigInfo);
        }

        //
        // These are the things we want to delete if they were created and
        // the initialization *FAILED* at a later time.
        //

        if (fdoExtension->InterruptObject) {
            IoDisconnectInterrupt(fdoExtension->InterruptObject);
        }

        if (driverKeyName) {
            ExFreePool(driverKeyName);
        }

        fdoExtension->DriverRegistryPath = NULL;

        if (symbolicLinkCreated) {
            IoDeleteSymbolicLink(&deviceLinkUnicodeString);
        }

        //
        // Free up any memory mapped in by the miniport using
        // VideoPort GetDeviceBase.
        //

        while (fdoExtension->MappedAddressList != NULL)
        {
            pVideoDebugPrint((0, "VideoPortInitialize: unfreed address %08lx, physical %08lx, size %08lx\n",
                                 fdoExtension->MappedAddressList->MappedAddress,
                                 fdoExtension->MappedAddressList->PhysicalAddress.LowPart,
                                 fdoExtension->MappedAddressList->NumberOfUchars));

            pVideoDebugPrint((0, "VideoPortInitialize: unfreed refcount %d, unmapping %d\n\n",
                                 fdoExtension->MappedAddressList->RefCount,
                                 fdoExtension->MappedAddressList->bNeedsUnmapping));

            VideoPortFreeDeviceBase(fdoExtension->HwDeviceExtension,
                                    fdoExtension->MappedAddressList->MappedAddress);
        }

        //
        // Remove any HwResetHw function we may have added for this device.
        //

        if (HwInitializationData->HwInitDataSize >
            FIELD_OFFSET(VIDEO_HW_INITIALIZATION_DATA, HwResetHw)) {

            ULONG iReset;

            for (iReset=0; iReset<6; iReset++) {

                if (HwResetHw[iReset].HwDeviceExtension ==
                    fdoExtension->HwDeviceExtension) {

                    HwResetHw[iReset].ResetFunction = NULL;
                    break;
                }
            }
        }

    } else {

        HwInitializationData->StartingDeviceNumber++;

    }

#if DBG
    if ((!NT_SUCCESS(ntStatus)) || (findAdapterStatus != NO_ERROR)) {
      if (fdoExtension->AllocationHead != NULL) {
        pVideoDebugPrint((0, "VIDEOPRT: CANNOT DELETE DEVICE WITH")) ;
        pVideoDebugPrint((0, "          OUTSTANDING ALLOCATIONS."));
        ASSERT(FALSE);
      }
      ExDeletePagedLookasideList (&fdoExtension->AllocationList);
    }
#endif

    return ntStatus;
}


BOOLEAN
pVideoPortInterrupt(
    IN PKINTERRUPT Interrupt,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This function is the main interrupt service routine. If finds which
    miniport driver the interrupt was for and forwards it.

Arguments:

    Interrupt -

    DeviceObject -

Return Value:

    Returns TRUE if the interrupt was expected.

--*/

{
    PFDO_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    BOOLEAN bRet;

    UNREFERENCED_PARAMETER(Interrupt);

    //
    // If there is no interrupt routine, fail the assertion
    //

    ASSERT (fdoExtension->HwInterrupt);

#if DBG
    InterlockedIncrement(&VPResourcesReported);
#endif

    if (fdoExtension->InterruptsEnabled) {
        bRet = fdoExtension->HwInterrupt(fdoExtension->HwDeviceExtension);
    } else {
        bRet = FALSE;  // this device did not handle the interrupt
    }

#if DBG
    InterlockedDecrement(&VPResourcesReported);
#endif

    return bRet;

} // pVideoPortInterrupt()


VOID
VideoPortLogError(
    IN PVOID HwDeviceExtension,
    IN PVIDEO_REQUEST_PACKET Vrp OPTIONAL,
    IN VP_STATUS ErrorCode,
    IN ULONG UniqueId
    )

/*++

Routine Description:

    This routine saves the error log information so it can be processed at
    any IRQL.

Arguments:

    HwDeviceExtension - Supplies the HBA miniport driver's adapter data storage.

    Vrp - Supplies an optional pointer to a video request packet if there is
        one.

    ErrorCode - Supplies an error code indicating the type of error.

    UniqueId - Supplies a unique identifier for the error.

Return Value:

    None.

--*/

{
} // end VideoPortLogError()


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

        pVideoDebugPrint((0, "Invalid return value from HwStartIo!\n"));
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


NTSTATUS
pVideoPortMapUserPhysicalMem(
    IN PFDO_EXTENSION FdoExtension,
    IN HANDLE ProcessHandle OPTIONAL,
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN OUT PULONG Length,
    IN OUT PULONG InIoSpace,
    IN OUT PVOID *VirtualAddress
    )

/*++

Routine Description:

    This function maps a view of a block of physical memory into a process'
    virtual address space.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    ProcessHandle - Optional handle to the process into which the memory must
        be mapped.

    PhysicalAddress - Offset from the beginning of physical memory, in bytes.

    Length - Pointer to a variable that will receive that actual size in
        bytes of the view. The length is rounded to a page boundary. THe
        length may not be zero.

    InIoSpace - Specifies if the address is in the IO space if TRUE; otherwise,
        the address is assumed to be in memory space.

    VirtualAddress - Pointer to a variable that will receive the base
        address of the view. If the initial value is not NULL, then the view
        will be allocated starting at teh specified virtual address rounded
        down to the next 64kb addess boundary.

Return Value:

    STATUS_UNSUCCESSFUL if the length was zero.
    STATUS_SUCCESS otherwise.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    NTSTATUS ntStatus;
    HANDLE physicalMemoryHandle;
    PHYSICAL_ADDRESS physicalAddressBase;
    PHYSICAL_ADDRESS physicalAddressEnd;
    PHYSICAL_ADDRESS viewBase;
    PHYSICAL_ADDRESS mappedLength;
    HANDLE processHandle;
    BOOLEAN translateBaseAddress;
    BOOLEAN translateEndAddress;
    ULONG inIoSpace2;
    ULONG inIoSpace1;
    ULONG MapViewFlags;

    //
    // Check for a length of zero. If it is, the entire physical memory
    // would be mapped into the process' address space. An error is returned
    // in this case.
    //

    if (!*Length) {

        return STATUS_INVALID_PARAMETER_4;

    }

    if (!(*InIoSpace & VIDEO_MEMORY_SPACE_USER_MODE)) {

        return STATUS_INVALID_PARAMETER_5;

    }

    //
    // Get a handle to the physical memory section using our pointer.
    // If this fails, return.
    //

    ntStatus = ObOpenObjectByPointer(PhysicalMemorySection,
                                     0L,
                                     (PACCESS_STATE) NULL,
                                     SECTION_ALL_ACCESS,
                                     (POBJECT_TYPE) NULL,
                                     KernelMode,
                                     &physicalMemoryHandle);

    if (!NT_SUCCESS(ntStatus)) {

        return ntStatus;

    }

#ifdef _ALPHA_

    //
    // All flags are necessary for translation on ALPHA, except the P6 FLAG
    //

    inIoSpace1 = *InIoSpace & ~VIDEO_MEMORY_SPACE_P6CACHE;
    inIoSpace2 = *InIoSpace & ~VIDEO_MEMORY_SPACE_P6CACHE;

#else

    //
    // No flags are used in translation on non-alpha
    //

    inIoSpace1 = *InIoSpace & VIDEO_MEMORY_SPACE_IO;
    inIoSpace2 = *InIoSpace & VIDEO_MEMORY_SPACE_IO;

#endif

    //
    // Initialize the physical addresses that will be translated
    //

    physicalAddressEnd.QuadPart = PhysicalAddress.QuadPart + (*Length - 1);

    //
    // Translate the physical addresses.
    //

    translateBaseAddress =
        HalTranslateBusAddress(FdoExtension->AdapterInterfaceType,
                               FdoExtension->SystemIoBusNumber,
                               PhysicalAddress,
                               &inIoSpace1,
                               &physicalAddressBase);

    translateEndAddress =
        HalTranslateBusAddress(FdoExtension->AdapterInterfaceType,
                               FdoExtension->SystemIoBusNumber,
                               physicalAddressEnd,
                               &inIoSpace2,
                               &physicalAddressEnd);

    if ( !(translateBaseAddress && translateEndAddress) ) {

        ZwClose(physicalMemoryHandle);

        return STATUS_DEVICE_CONFIGURATION_ERROR;

    }

    ASSERT(inIoSpace1 == inIoSpace2);

    //
    // Calcualte the length of the memory to be mapped
    //

    mappedLength.QuadPart = physicalAddressEnd.QuadPart -
                            physicalAddressBase.QuadPart + 1;

    pVideoDebugPrint((3, "mapped Length = %x\n", mappedLength));

#ifndef _ALPHA_

    //
    // On all systems except ALPHA the mapped length should be the same as
    // the translated length.
    //

    ASSERT (((ULONG_PTR)mappedLength.QuadPart) == *Length);

#endif

    //
    // If the mappedlength is zero, somthing very weird happened in the HAL
    // since the Length was checked against zero.
    //

    ASSERT (mappedLength.QuadPart != 0);

    //
    // If the address is in io space, just return the address, otherwise
    // go through the mapping mechanism
    //

    if ( (*InIoSpace) & (ULONG)0x01 ) {

        (ULONG_PTR) *VirtualAddress = (ULONG_PTR) physicalAddressBase.QuadPart;

    } else {


        //
        // If no process handle was passed, get the handle to the current
        // process.
        //

        if (ProcessHandle) {

            processHandle = ProcessHandle;

        } else {

            processHandle = NtCurrentProcess();

        }

        //
        // initialize view base that will receive the physical mapped
        // address after the MapViewOfSection call.
        //

        viewBase = physicalAddressBase;

        //
        // Map the section
        //

        //
        // BUGBUG - what to do with already cached memory ???
        //

        if ((*InIoSpace) & VIDEO_MEMORY_SPACE_P6CACHE) {
            MapViewFlags = PAGE_READWRITE | PAGE_WRITECOMBINE;
        } else {
            MapViewFlags = PAGE_READWRITE | PAGE_NOCACHE;
        }

        ntStatus = ZwMapViewOfSection(physicalMemoryHandle,
                                      processHandle,
                                      VirtualAddress,
                                      0L,
                                      (ULONG_PTR) mappedLength.QuadPart,
                                      &viewBase,
                                      (PULONG_PTR) (&(mappedLength.QuadPart)),
                                      ViewUnmap,
                                      0,
                                      MapViewFlags);

        //
        // Close the handle since we only keep the pointer reference to the
        // section.
        //

        ZwClose(physicalMemoryHandle);

        //
        // Mapping the section above rounded the physical address down to the
        // nearest 64 K boundary. Now return a virtual address that sits where
        // we wnat by adding in the offset from the beginning of the section.
        //


        (ULONG_PTR) *VirtualAddress += (ULONG_PTR) (physicalAddressBase.QuadPart -
                                                  viewBase.QuadPart);
    }

#ifdef _ALPHA_

    //
    // Return the proper set of modified flags.
    //

    *InIoSpace = inIoSpace1 | *InIoSpace & VIDEO_MEMORY_SPACE_P6CACHE;

#else

    //
    // Restore all the other FLAGS
    // BUGBUG P6 flag may be affected !!
    //

    *InIoSpace = inIoSpace1 | *InIoSpace & ~VIDEO_MEMORY_SPACE_IO;

#endif

    *Length = mappedLength.LowPart;

    return ntStatus;

} // end pVideoPortMapUserPhysicalMem()

VP_STATUS
VideoPortAllocateBuffer(
    IN PVOID HwDeviceExtension,
    IN ULONG Size,
    OUT PVOID *Buffer
    )
{
    //
    // This routine attempts to allocate a paged pool buffer on behalf of a given
    // video miniport driver.
    //

    PFDO_EXTENSION fdoExtension = GET_FDO_EXT (HwDeviceExtension) ;
    PALLOC_ENTRY allocEntry, currentEntry ;

#if DBG
    if ((fdoExtension->FreeAllocation - (LONG)Size) < 0) {
        *Buffer = NULL ;
        pVideoDebugPrint ((0, "VIDEOPRT: Failing excessive allocation.\n"));
        return ERROR_INSUFFICIENT_BUFFER ;
    }
#endif

    *Buffer = ExAllocatePoolWithTag (PagedPool, Size, 'RdiV');
    if (*Buffer == NULL) {
        return ERROR_NOT_ENOUGH_MEMORY ;
    }

#if DBG
    fdoExtension->FreeAllocation -= Size ;
    allocEntry = ExAllocateFromPagedLookasideList (&fdoExtension->AllocationList);
    allocEntry->Address = *Buffer ;
    allocEntry->Size = Size;
    allocEntry->Next = fdoExtension->AllocationHead ;
    fdoExtension->AllocationHead = allocEntry ;
#endif

    return NO_ERROR ;
}

VOID
VideoPortReleaseBuffer(
    IN PVOID HwDeviceExtension,
    IN PVOID Buffer
    )
{
  //
  // This routine releases a buffer allocated for a miniport driver by
  // VideoPortReleaseBuffer.
  //

#if DBG
    PALLOC_ENTRY currentEntry, prevEntry ;
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT (HwDeviceExtension) ;
    ASSERT (fdoExtension->AllocationHead != NULL) ;

    prevEntry = NULL ;
    currentEntry = fdoExtension->AllocationHead ;
    while (currentEntry->Address != Buffer) {
        prevEntry = currentEntry ;
        currentEntry = currentEntry->Next ;
        if (currentEntry == NULL) {
            pVideoDebugPrint ((0, "VIDEOPRT: Freeing memory not allocated!\n"));
            ASSERT (FALSE) ;
        }
    }

    if (prevEntry != NULL) {
        prevEntry->Next = currentEntry->Next ;
    } else {
        fdoExtension->AllocationHead = currentEntry->Next ;
    }
    fdoExtension->FreeAllocation += currentEntry->Size ;

    ExFreeToPagedLookasideList (&(fdoExtension->AllocationList),
                                currentEntry) ;
#endif

    ExFreePool (Buffer) ;
}


VP_STATUS
VideoPortMapMemory(
    PVOID HwDeviceExtension,
    PHYSICAL_ADDRESS PhysicalAddress,
    PULONG Length,
    PULONG InIoSpace,
    PVOID *VirtualAddress
    )

/*++

Routine Description:

    VideoPortMapMemory allows the miniport driver to map a section of
    physical memory (either memory or registers) into the calling process'
    address space (eventhough we are in kernel mode, this function is
    executed within the same context as the user-mode process that initiated
    the call).

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    PhysicalAddress - Specifies the physical address to be mapped.

    Length - Points to the number of bytes of physical memory to be mapped.
        This argument returns the actual amount of memory mapped.

    InIoSpace - Points to a variable that is 1 if the address is in I/O
        space.  Otherwise, the address is assumed to be in memory space.

    VirtualAddress - A pointer to a location containing:

        on input: An optional handle to the process in which the memory must
            be mapped. 0 must be used to map the memory for the display
            driver (in the context of the windows server process).

        on output:  The return value is the virtual address at which the
            physical address has been mapped.

Return Value:

    VideoPortMapMemory returns the status of the operation.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{

    NTSTATUS ntStatus;
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);
    HANDLE processHandle;

    //
    // Check for valid pointers.
    //

    if (!(ARGUMENT_PRESENT(Length)) ||
        !(ARGUMENT_PRESENT(InIoSpace)) ||
        !(ARGUMENT_PRESENT(VirtualAddress)) ) {

        ASSERT(FALSE);
        return ERROR_INVALID_PARAMETER;

    }

    //
    // Let's handle the special memory types here.
    //
    // NOTE
    // Large pages is automatic - the caller need not specify this attribute
    // since it does not affect the device.

    //
    // Save the process handle and zero out the Virtual address field
    //

    if (*VirtualAddress == NULL) {

        if (*InIoSpace & VIDEO_MEMORY_SPACE_USER_MODE)
        {
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
        }

        pVideoDebugPrint((3, "VideoPortMapMemory: Map Physical Address %08lx\n",
                         PhysicalAddress));

        ntStatus = STATUS_SUCCESS;

        //
        // We specify TRUE for large pages since we know the addrses will only
        // be used in the context of the display driver, at normal IRQL.
        //

        *VirtualAddress = pVideoPortGetDeviceBase(HwDeviceExtension,
                                                  PhysicalAddress,
                                                  *Length,
                                                  (UCHAR) (*InIoSpace),
                                                  TRUE);

        //
        // Zero can only be success if the driver is calling to MAP
        // address 0.  Otherwise, it is an error.
        // BUGBUG - is this really robust.
        //

        if (*VirtualAddress == NULL) {

            //
            // Only on X86 can the logical address also be 0.
            //

#if defined (_X86_) || defined (_ALPHA_) || defined(_IA64_)
            if (PhysicalAddress.QuadPart != 0)
#endif
                ntStatus = STATUS_INVALID_PARAMETER;
        }

    } else {

        if (!(*InIoSpace & VIDEO_MEMORY_SPACE_USER_MODE))
        {
            //
            // We can not assert since this is an existing path and old
            // drivers will not have this flag set.
            //
            // ASSERT(FALSE);
            // return ERROR_INVALID_PARAMETER;
            //

            *InIoSpace |= VIDEO_MEMORY_SPACE_USER_MODE;
        }

        processHandle = (HANDLE) *VirtualAddress;
        *VirtualAddress = NULL;

        pVideoDebugPrint((3, "VideoPortMapMemory: Map Physical Address %08lx\n",
                         PhysicalAddress));

        ntStatus = pVideoPortMapUserPhysicalMem(fdoExtension,
                                                processHandle,
                                                PhysicalAddress,
                                                Length,
                                                InIoSpace,
                                                VirtualAddress);

    }

    if (!NT_SUCCESS(ntStatus)) {

        *VirtualAddress = NULL;

        pVideoDebugPrint((0, "VideoPortMapMemory failed with NtStatus = %08lx\n",
                         ntStatus));
        ASSERT(FALSE);

        return ERROR_INVALID_PARAMETER;

    } else {

        pVideoDebugPrint((3, "VideoPortMapMemory succeded with Virtual Address = %08lx\n",
                         *VirtualAddress));

        return NO_ERROR;

    }

} // end VideoPortMapMemory()



VOID
pVideoPortPowerCompletionIoctl(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )

/*++

Routine Description:

    Completion routine that is called when one of our power irps has been
    comeplted.  This allows us to fill out the status code for the request.

Arguments:

    DeviceObject  - Pointer to the device object

    MinorFunction - Minor function of the IRP

    PowerState    - Power state that was set

    Context       - Context paramter

    IoStatus      - Status block for that IRP

Return Value:

    VOID

Environment:

--*/

{
    PPOWER_BLOCK powerContext = (PPOWER_BLOCK) Context;

    if (powerContext->FinalFlag == TRUE) {
        powerContext->Irp->IoStatus.Status = IoStatus->Status;
        IoCompleteRequest (powerContext->Irp, IO_VIDEO_INCREMENT);
    }

    ExFreePool(Context);

    return;
}




BOOLEAN
pVideoPortResetDisplay(
    IN ULONG Columns,
    IN ULONG Rows
    )

/*++

Routine Description:

    Callback for the HAL that calls the miniport driver.

Arguments:

    Columns - The number of columns of the video mode.

    Rows - The number of rows for the video mode.

Return Value:

    We always return FALSE so the HAL will always reste the mode afterwards.

Environment:
                        mep videoprt.cod
    Non-paged only.
    Used in BugCheck and soft-reset calls.

--*/

{

    ULONG iReset;
    BOOLEAN bRetVal = FALSE;

    for (iReset=0;
         (iReset < 6) && (HwResetHw[iReset].HwDeviceExtension);
         iReset++) {

        PFDO_EXTENSION fdoExtension =
            GET_FDO_EXT(HwResetHw[iReset].HwDeviceExtension);

        if (HwResetHw[iReset].ResetFunction &&
            fdoExtension->HwInitStatus == HwInitSucceeded) {

            bRetVal &= HwResetHw[iReset].ResetFunction(HwResetHw[iReset].HwDeviceExtension,
                                                       Columns,
                                                       Rows);
        }
    }

    return bRetVal;

} // end pVideoPortResetDisplay()



BOOLEAN
VideoPortScanRom(
    PVOID HwDeviceExtension,
    PUCHAR RomBase,
    ULONG RomLength,
    PUCHAR String
    )

/*++

Routine Description:

    Does a case *SENSITIVE* search for a string in the ROM.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    RomBase - Base address at which the search should start.

    RomLength - Size, in bytes, of the ROM area in which to perform the
        search.

    String - String to search for

Return Value:

    Returns TRUE if the string was found.
    Returns FALSE if it was not found.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    ULONG stringLength, length;
    ULONG_PTR startOffset;
    PUCHAR string1, string2;
    BOOLEAN match;

    UNREFERENCED_PARAMETER(HwDeviceExtension);

    stringLength = strlen(String);

    for (startOffset = 0;
         startOffset < RomLength - stringLength + 1;
         startOffset++) {

        length = stringLength;
        string1 = RomBase + startOffset;
        string2 = String;
        match = TRUE;

        IS_ACCESS_RANGES_DEFINED()

        while (length--) {

            if (READ_REGISTER_UCHAR(string1++) - (*string2++)) {

                match = FALSE;
                break;

            }
        }

        if (match) {

            return TRUE;
        }
    }

    return FALSE;

} // end VideoPortScanRom()



VP_STATUS
VideoPortSetRegistryParameters(
    PVOID HwDeviceExtension,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )

/*++

Routine Description:

    VideoPortSetRegistryParameters writes information to the CurrentControlSet
    in the registry.  The function automatically searches for or creates the
    specified parameter name under the parameter key of the current driver.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    ValueName - Points to a Unicode string that contains the name of the
        data value being written in the registry.

    ValueData - Points to a buffer containing the information to be written
        to the registry.

    ValueLength - Specifies the size of the data being written to the registry.

Return Value:

    This function returns the final status of the operation.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    NTSTATUS ntStatus;
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    //
    // Don't let people store as DefaultSettings anymore ...
    // Must still work for older drivers through.
    //

    if (wcsncmp(ValueName,
                L"DefaultSettings.",
                sizeof(L"DefaultSettings.")) == 0) {

        ASSERT(FALSE);

        //
        // check for NT 5.0
        //

        if (fdoExtension->HwGetPowerState) {

            return ERROR_INVALID_PARAMETER;
        }
    }

    //
    // BUGBUG What happens for files ... ?
    //

    ntStatus = RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE,
                                     fdoExtension->DriverRegistryPath,
                                     ValueName,
                                     REG_BINARY,
                                     ValueData,
                                     ValueLength);

    if (!NT_SUCCESS(ntStatus)) {

        return ERROR_INVALID_PARAMETER;

    } else {

        return NO_ERROR;

    }

} // end VideoPortSetRegistryParamaters()



VOID
pVideoPortHwTimer(
    IN PDEVICE_OBJECT DeviceObject,
    PVOID Context
    )

/*++

Routine Description:

    This function is the main entry point for the timer routine that we then
    forward to the miniport driver.

Arguments:

    DeviceObject -

    Context - Not needed

Return Value:

    None.

--*/

{
    PFDO_EXTENSION fdoExtension = DeviceObject->DeviceExtension;

    UNREFERENCED_PARAMETER(Context);

#if DBG
    InterlockedIncrement(&VPResourcesReported);
#endif

    fdoExtension->HwTimer(fdoExtension->HwDeviceExtension);

#if DBG
    InterlockedDecrement(&VPResourcesReported);
#endif

    return;

} // pVideoPortInterrupt()



VOID
VideoPortStartTimer(
    PVOID HwDeviceExtension
    )

/*++

Routine Description:

    Enables the timer specified in the HW_INITIALIZATION_DATA structure
    passed to the video port driver at init time.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

Return Value:

    None

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    if (fdoExtension->HwTimer == NULL) {

        ASSERT(fdoExtension->HwTimer != NULL);

    } else {

        IoStartTimer(fdoExtension->FunctionalDeviceObject);

    }

    return;
}



VOID
VideoPortStopTimer(
    PVOID HwDeviceExtension
    )

/*++

Routine Description:

    Disables the timer specified in the HW_INITIALIZATION_DATA structure
    passed to the video port driver at init time.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

Return Value:

    None

--*/

{
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    if (fdoExtension->HwTimer == NULL) {

        ASSERT(fdoExtension->HwTimer != NULL);

    } else {

        IoStopTimer(fdoExtension->FunctionalDeviceObject);

    }

    return;
}



BOOLEAN
VideoPortSynchronizeExecution(
    PVOID HwDeviceExtension,
    VIDEO_SYNCHRONIZE_PRIORITY Priority,
    PMINIPORT_SYNCHRONIZE_ROUTINE SynchronizeRoutine,
    PVOID Context
    )

/*++

    Stub so we can allow the miniports to link directly

--*/

{
    return pVideoPortSynchronizeExecution(HwDeviceExtension,
                                          Priority,
                                          SynchronizeRoutine,
                                          Context);
} // end VideoPortSynchronizeExecution()

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
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);
    KIRQL oldIrql;

    //
    // Switch on which type of priority.
    //

    switch (Priority) {

    case VpMediumPriority:

        //
        // This is synchronized with the interrupt object
        //

        if (fdoExtension->InterruptObject) {

            status = KeSynchronizeExecution(fdoExtension->InterruptObject,
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



VP_STATUS
VideoPortUnmapMemory(
    PVOID HwDeviceExtension,
    PVOID VirtualAddress,
    HANDLE ProcessHandle
    )

/*++

Routine Description:

    VideoPortUnmapMemory allows the miniport driver to unmap a physical
    address range previously mapped into the calling process' address space
    using the VideoPortMapMemory function.

Arguments:

    HwDeviceExtension - Points to the miniport driver's device extension.

    VirtualAddress - Points to the virtual address to unmap from the
        address space of the caller.

    // InIoSpace - Specifies whether the address is in I/O space (1) or memory
    //     space (0).

    ProcessHandle - Handle to the process from which memory must be unmapped.

Return Value:

    This function returns a status code of NO_ERROR if the operation succeeds.
    It returns ERROR_INVALID_PARAMETER if an error occurs.

Environment:

    This routine cannot be called from a miniport routine synchronized with
    VideoPortSynchronizeRoutine or from an ISR.

--*/

{
    NTSTATUS ntstatus;
    VP_STATUS vpStatus = NO_ERROR;
    PFDO_EXTENSION fdoExtension = GET_FDO_EXT(HwDeviceExtension);

    //
    // Backwards compatibility to when the ProcessHandle was actually
    // ULONG InIoSpace.
    //

    if (((ULONG_PTR)(ProcessHandle)) == 1) {

        pVideoDebugPrint((0,"\n\n*** VideoPortUnmapMemory - interface change *** Must pass in process handle\n\n"));
        DbgBreakPoint();

        return NO_ERROR;

    }

    if (((ULONG_PTR)(ProcessHandle)) == 0) {

        //
        // If the process handle is zero, it means it was mapped by the display
        // driver and is therefore in kernel mode address space.
        //

        if (!pVideoPortFreeDeviceBase(HwDeviceExtension, VirtualAddress)) {

            ASSERT(FALSE);

            vpStatus = ERROR_INVALID_PARAMETER;

        }

    } else {

        //
        // A process handle is passed in.
        // This ms it was mapped for use by an application (DCI \ DirectDraw).
        //

#ifdef _ALPHA_
        //
        // On Alpha, VirtualAddress may not be a true VA if this was
        // a sparse memory or IO space.   Transform the QVA to a VA,
        // if necessary.
        //

        VirtualAddress = HalDereferenceQva(VirtualAddress,
                                           fdoExtension->AdapterInterfaceType,
                                           fdoExtension->SystemIoBusNumber);

#endif

        ntstatus = ZwUnmapViewOfSection ( ProcessHandle,
            (PVOID) ( ((ULONG_PTR)VirtualAddress) & (~(PAGE_SIZE - 1)) ) );

        if ( (!NT_SUCCESS(ntstatus)) &&
             (ntstatus != STATUS_PROCESS_IS_TERMINATING) ) {

            ASSERT(FALSE);

            vpStatus = ERROR_INVALID_PARAMETER;

        }
    }

    return NO_ERROR;

} // end VideoPortUnmapMemory()


/////////////////////////////////////////////////////////
//
//  Video port/miniport interface for ENG_EVENTs
//
//  All Events are created or mapped by display driver. If the miniport wants
//  them, the display driver must convey them to the miniport and the miniport
//  must treat them as type VIDEO_PORT_EVENT, which must be the same as
//  ENG_EVENT from winddi.h. Because KEVENTs cannot be set from ISRs, the
//  mechanism provided for SetEvent queues a DPC, which actually sets the
//  event.
//
//

//
// PRIVATE structure. NOTE: this must be same as for ENG/GDI.
//
//
//typedef struct _VIDEO_PORT_EVENT {
//  PKEVENT pKEvent;
//  ULONG   fFlags;
//  } VIDEO_PORT_EVENT, *PVIDEO_PORT_EVENT;
//
//

VOID
pVideoPortSetEvent(
    PVIDEO_PORT_EVENT   pVPEvent
    )
/*++

Routine Description:


Arguments:

    pVPEvent - a PVIDEO_PORT_EVENT

Return Value:

    None.

    Note: this is called via a dpc queued via VideoSetEvent(). It's possible
    that this routine executes after the associated KEVENT is no longer valid
    (either the display driver deleted it or it's DriverObject is being
    deleted via EngUnmapEvent. In either case, the containing VIDEO_PORT_EVENT
    has been marked as both "invalid" and "in a dpc". In this case, we free it,
    possibly ObDereferencing it if it's mapped.

--*/
{
    if ((pVPEvent->pKEvent) &&
        (!(pVPEvent->fFlags & ENG_EVENT_FLAG_IS_INVALID))) {

        pVideoDebugPrint((2, "Set the event at %xin VP DPC\n", pVPEvent));
        KeSetEvent(pVPEvent->pKEvent, 0, FALSE);
        pVPEvent->fFlags &= ~ENG_EVENT_FLAG_IN_DPC;

    } else {

        //
        //  If this is marked as invalid, then the GDI Engine is trying to
        //  delete it. The delete routine in gre will wait until it's the
        //  ENG_EVENT_FLAG_IN_DPC bit is cleared.
        //

        if (pVPEvent->fFlags & ENG_EVENT_FLAG_IS_INVALID) {

            pVPEvent->fFlags &= ~ENG_EVENT_FLAG_IN_DPC;

        } else {

            pVideoDebugPrint((0, "Bad PEVENT\n"));
        }
    }

}

#if DBG

PIO_RESOURCE_REQUIREMENTS_LIST
BuildRequirements(
    PCM_RESOURCE_LIST pcmResourceList
    )
{
    ULONG i;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pcmDescript;

    PIO_RESOURCE_REQUIREMENTS_LIST Requirements;
    PIO_RESOURCE_DESCRIPTOR pioDescript;

    ULONG RequirementsListSize;
    ULONG RequirementCount;

    pVideoDebugPrint((1, "Building Requirements List...\n"));

    RequirementCount = pcmResourceList->List[0].PartialResourceList.Count;

    RequirementsListSize = sizeof(IO_RESOURCE_REQUIREMENTS_LIST) +
                              ((RequirementCount - 1) *
                              sizeof(IO_RESOURCE_DESCRIPTOR));

    Requirements = (PIO_RESOURCE_REQUIREMENTS_LIST) ExAllocatePool(
                                                        PagedPool,
                                                        RequirementsListSize);

    Requirements->ListSize         = RequirementsListSize;
    Requirements->InterfaceType    = pcmResourceList->List[0].InterfaceType;
    Requirements->BusNumber        = pcmResourceList->List[0].BusNumber;
    Requirements->SlotNumber       = -1; // ???
    Requirements->AlternativeLists = 0; // ???

    Requirements->List[0].Version  = pcmResourceList->List[0].PartialResourceList.Version;
    Requirements->List[0].Revision = pcmResourceList->List[0].PartialResourceList.Revision;
    Requirements->List[0].Count    = RequirementCount;

    pcmDescript = &(pcmResourceList->List[0].PartialResourceList.PartialDescriptors[0]);
    pioDescript = &(Requirements->List[0].Descriptors[0]);

    for (i=0; i<RequirementCount; i++) {

        pioDescript->Option = IO_RESOURCE_PREFERRED;
        pioDescript->Type   = pcmDescript->Type;
        pioDescript->ShareDisposition = pcmDescript->ShareDisposition;
        pioDescript->Flags  = pcmDescript->Flags;

        switch (pcmDescript->Type) {
        case CmResourceTypePort:
            pioDescript->u.Port.Length = pcmDescript->u.Port.Length;
            pioDescript->u.Port.Alignment = 1;
            pioDescript->u.Port.MinimumAddress =
            pioDescript->u.Port.MaximumAddress = pcmDescript->u.Port.Start;
            break;

        case CmResourceTypeMemory:
            pioDescript->u.Memory.Length = pcmDescript->u.Memory.Length;
            pioDescript->u.Memory.Alignment = 1;
            pioDescript->u.Memory.MinimumAddress =
            pioDescript->u.Memory.MaximumAddress = pcmDescript->u.Memory.Start;
            break;

        default:

            //
            // We don't have to handle the other stuff, because we only
            // want to report Ports and Memory to the system.
            //

            break;
        }

        pioDescript++;
        pcmDescript++;
    }

    return Requirements;
}

VOID
DumpRequirements(
    PIO_RESOURCE_REQUIREMENTS_LIST Requirements
    )
{
    ULONG i;

    PIO_RESOURCE_DESCRIPTOR pioDescript;

    ULONG RequirementsListSize;
    ULONG RequirementCount = Requirements->List[0].Count;

    char *Table[] = { "Internal",
                      "Isa",
                      "Eisa",
                      "MicroChannel",
                      "TurboChannel",
                      "PCIBus",
                      "VMEBus",
                      "NuBus",
                      "PCMCIABus",
                      "CBus",
                      "MPIBus",
                      "MPSABus",
                      "ProcessorInternal",
                      "InternalPowerBus",
                      "PNPISABus",
                      "MaximumInterfaceType"
                    };

    pVideoDebugPrint((1, "ListSize:         0x%x\n"
                         "InterfaceType:    %s\n"
                         "BusNumber:        0x%x\n"
                         "SlotNumber:       0x%x\n"
                         "AlternativeLists: 0x%x\n",
                         Requirements->ListSize,
                         Table[Requirements->InterfaceType],
                         Requirements->BusNumber,
                         Requirements->SlotNumber,
                         Requirements->AlternativeLists));

    pVideoDebugPrint((1, "List[0].Version:  0x%x\n"
                         "List[0].Revision: 0x%x\n"
                         "List[0].Count:    0x%x\n",
                         Requirements->List[0].Version,
                         Requirements->List[0].Revision,
                         Requirements->List[0].Count));

    pioDescript = &(Requirements->List[0].Descriptors[0]);

    for (i=0; i<RequirementCount; i++) {

        pVideoDebugPrint((1, "\n"
                             "Option:           0x%x\n"
                             "Type:             0x%x\n"
                             "ShareDisposition: 0x%x\n"
                             "Flags:            0x%x\n",
                             pioDescript->Option,
                             pioDescript->Type,
                             pioDescript->ShareDisposition,
                             pioDescript->Flags));

        switch (pioDescript->Type) {
        case CmResourceTypePort:

            pVideoDebugPrint((1, "\nPort...\n"
                                 "\tLength:         0x%x\n"
                                 "\tAlignment:      0x%x\n"
                                 "\tMinimumAddress: 0x%x\n"
                                 "\tMaximumAddress: 0x%x\n",
                                 pioDescript->u.Port.Length,
                                 pioDescript->u.Port.Alignment,
                                 pioDescript->u.Port.MinimumAddress,
                                 pioDescript->u.Port.MaximumAddress));

            break;

        case CmResourceTypeMemory:

            pVideoDebugPrint((1, "\nMemory...\n"
                                 "\tLength:         0x%x\n"
                                 "\tAlignment:      0x%x\n"
                                 "\tMinimumAddress: 0x%x\n"
                                 "\tMaximumAddress: 0x%x\n",
                                 pioDescript->u.Memory.Length,
                                 pioDescript->u.Memory.Alignment,
                                 pioDescript->u.Memory.MinimumAddress,
                                 pioDescript->u.Memory.MaximumAddress));
            break;

        case CmResourceTypeInterrupt:

            pVideoDebugPrint((1, "\nInterrupt...\n"
                                 "\tMinimum Vector: 0x%x\n"
                                 "\tMaximum Vector: 0x%x\n",
                                 pioDescript->u.Interrupt.MinimumVector,
                                 pioDescript->u.Interrupt.MaximumVector));

            break;

        default:

            //
            // We don't have to handle the other stuff, because we only
            // want to report Ports and Memory to the system.
            //

            break;
        }

        pioDescript++;
    }

    return;
}

VOID
DumpResourceList(
    PCM_RESOURCE_LIST pcmResourceList)
{
    ULONG i, j;
    PCM_FULL_RESOURCE_DESCRIPTOR    pcmFull;
    PCM_PARTIAL_RESOURCE_LIST       pcmPartial;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pcmDescript;

    pVideoDebugPrint((1, "...StartResourceList...\n"));

    pcmFull = &(pcmResourceList->List[0]);
    for (i=0; i<pcmResourceList->Count; i++) {

        pVideoDebugPrint((1, "List[%d]\n", i));

        pVideoDebugPrint((1, "InterfaceType = 0x%x\n", pcmFull->InterfaceType));
        pVideoDebugPrint((1, "BusNumber = 0x%x\n", pcmFull->BusNumber));

        pcmPartial = &(pcmFull->PartialResourceList);

        pVideoDebugPrint((1, "Version = 0x%x\n", pcmPartial->Version));
        pVideoDebugPrint((1, "Revision = 0x%x\n", pcmPartial->Revision));

        pcmDescript = &(pcmPartial->PartialDescriptors[0]);

        for (j=0; j<pcmPartial->Count; j++) {

            switch (pcmDescript->Type) {
            case CmResourceTypePort:
                pVideoDebugPrint((1, "Port: 0x%x Length: 0x%x\n",
                                  pcmDescript->u.Port.Start.LowPart,
                                  pcmDescript->u.Port.Length));

                break;

            case CmResourceTypeInterrupt:
                pVideoDebugPrint((1, "Interrupt: 0x%x Level: 0x%x\n",
                                  pcmDescript->u.Interrupt.Vector,
                                  pcmDescript->u.Interrupt.Level));
                break;

            case CmResourceTypeMemory:
                pVideoDebugPrint((1, "Start: 0x%x Length: 0x%x\n",
                                  pcmDescript->u.Memory.Start.LowPart,
                                  pcmDescript->u.Memory.Length));
                break;

            case CmResourceTypeDma:
                pVideoDebugPrint((1, "Dma Channel: 0x%x Port: 0x%x\n",
                                  pcmDescript->u.Dma.Channel,
                                  pcmDescript->u.Dma.Port));
                break;
            }

            pcmDescript++;
        }

        pcmFull = (PCM_FULL_RESOURCE_DESCRIPTOR) pcmDescript;
    }

    pVideoDebugPrint((1, "...EndResourceList...\n"));
}

VOID
DumpHwInitData(
    IN PVIDEO_HW_INITIALIZATION_DATA p
    )

/*++

Routine Description:

    Dump enough of the HwInitData to visually see if it is valid.

Arguments:

    Pointer to HwInitializationData

Return Value:

    none

--*/


{
    pVideoDebugPrint((0, "HwInitializationData...\n"));

    pVideoDebugPrint((0, "  HwInitDataSize: 0x%x\n", p->HwInitDataSize));


    pVideoDebugPrint((0, "  AdapterInterfaceType: %s\n",
                         BusType[p->AdapterInterfaceType]));

    pVideoDebugPrint((0, "  HwDeviceExtensionSize: 0x%x\n",
                         p->HwDeviceExtensionSize));

    pVideoDebugPrint((0, "  StartingDeviceNumber: 0x%x\n",
                         p->StartingDeviceNumber));

    pVideoDebugPrint((0, "  HwStartIO: 0x%x\n",
                         p->HwStartIO));

}

VOID
DumpUnicodeString(
    IN PUNICODE_STRING p
    )
{
    PUSHORT pus = p->Buffer;
    UCHAR buffer[256];       // the string better not be longer than 255 chars!
    PUCHAR puc = buffer;
    ULONG i;

    for (i = 0; i < p->Length; i++) {

        *puc++ = (UCHAR) *pus++;

    }

    *puc = 0;  // null terminate the string

    pVideoDebugPrint((0, "UNICODE STRING: %s\n", buffer));
}

#endif

VIDEOPORT_API
VP_STATUS
VideoPortQueryServices(
    IN PVOID pHwDeviceExtension,
    IN VIDEO_PORT_SERVICES servicesType,
    IN OUT PINTERFACE pInterface
    )

/*++

Routine Description:

    This routine exposes interfaces to services supported by the videoprt.

Arguments:

    pHwDeviceExtension - Points to per-adapter device extension.
    servicesType       - Requested services type.
    pInterface         - Points to services interface structure.

Returns:

    NO_ERROR   - Valid interface in the pInterface.
    Error code - Unsupported / unavailable services.

--*/

{
    VP_STATUS vpStatus;

    PAGED_CODE();
    ASSERT(NULL != pHwDeviceExtension);
    ASSERT(NULL != pInterface);
    ASSERT(IS_HW_DEVICE_EXTENSION(pHwDeviceExtension) == TRUE);

    if (VideoPortServicesAGP == servicesType)
    {
        PVIDEO_PORT_AGP_INTERFACE pAgpInterface = (PVIDEO_PORT_AGP_INTERFACE)pInterface;

        if ((pAgpInterface->Size != sizeof (VIDEO_PORT_AGP_INTERFACE)) ||
            (pAgpInterface->Version != 1))
        {
            pVideoDebugPrint((0, "VIDEOPRT!VideoPortQueryServices: Unsupported interface version\n"));
            ASSERT(FALSE);
            vpStatus = ERROR_INVALID_PARAMETER;
        }
        else
        {
            pAgpInterface->Context              = pHwDeviceExtension;
            pAgpInterface->InterfaceReference   = VpInterfaceDefaultReference;
            pAgpInterface->InterfaceDereference = VpInterfaceDefaultDereference;

            if (VideoPortGetAgpServices(pHwDeviceExtension,
                (PVIDEO_PORT_AGP_SERVICES)&(pAgpInterface->AgpReservePhysical)) == TRUE)
            {
                vpStatus = NO_ERROR;
            }
            else
            {
                vpStatus = ERROR_DEV_NOT_EXIST;
            }
        }
    }
    else if (VideoPortServicesI2C == servicesType)
    {
        PVIDEO_PORT_I2C_INTERFACE pI2CInterface = (PVIDEO_PORT_I2C_INTERFACE)pInterface;

        if ((pI2CInterface->Size != sizeof (VIDEO_PORT_I2C_INTERFACE)) ||
            (pI2CInterface->Version != 1))
        {
            pVideoDebugPrint((0, "VIDEOPRT!VideoPortQueryServices: Unsupported interface version\n"));
            ASSERT(FALSE);
            vpStatus = ERROR_INVALID_PARAMETER;
        }
        else
        {
            pI2CInterface->Context              = pHwDeviceExtension;
            pI2CInterface->InterfaceReference   = VpInterfaceDefaultReference;
            pI2CInterface->InterfaceDereference = VpInterfaceDefaultDereference;
            pI2CInterface->I2CStart             = I2CStart;
            pI2CInterface->I2CStop              = I2CStop;
            pI2CInterface->I2CWrite             = I2CWrite;
            pI2CInterface->I2CRead              = I2CRead;

            vpStatus = NO_ERROR;
        }
    }
    else
    {
        pVideoDebugPrint((0, "VIDEOPRT!VideoPortQueryServices: Unsupported service type\n"));
        ASSERT(FALSE);
        vpStatus = ERROR_INVALID_PARAMETER;
    }

    return vpStatus;
}   // VideoPortQueryServices()

VOID
VpInterfaceDefaultReference(
    IN PVOID pContext
    )

/*++

Routine Description:

    This routine is default callback for interfaces exposed from the videoprt.
    Should be called by the client before it starts using an interface.

Arguments:

    pContext - Context returned by the VideoPortQueryServices() in the
               pInterface->Context field.

--*/

{
    PAGED_CODE();
}   // VpInterfaceDefaultReference()

VOID
VpInterfaceDefaultDereference(
    IN PVOID pContext
    )

/*++

Routine Description:

    This routine is default callback for interfaces exposed from the videoprt.
    Should be called by the client when it stops using an interface.

Arguments:

    pContext - Context returned by the VideoPortQueryServices() in the
               pInterface->Context field.

--*/

{
    PAGED_CODE();
}   // VpInterfaceDefaultDereference()
