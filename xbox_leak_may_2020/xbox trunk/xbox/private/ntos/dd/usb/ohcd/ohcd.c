/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    ohcd.c

Abstract:
    
    Basic entry point implementation of the OpenHCI
    driver.
    
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-17-00 created by Mitchell Dernis (mitchd)

--*/


//
//  Pull in OS headers
//
#include <ntos.h>
#include <pci.h>


//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DCHO'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("OHCD");


//
//  Pull in usb headers
//
#include <hcdi.h>       //Interface between USBD and HCD
#include "ohcd.h"       //Private OHCD stuff
#include "devsys.h"     //checks for development systems

//----------------------------------------------------------------------------
//  Spew the build settings during compile
//----------------------------------------------------------------------------
#ifdef OHCD_XBOX_HARDWARE_ONLY
#pragma message("OHCD: Xbox Hardware Only Build")
#endif
#if(USB_HOST_CONTROLLER_CONFIGURATION==USB_SINGLE_HOST_CONTROLLER)
#pragma message("OHCD: Single Host Controller Support")
#endif
#if(USB_HOST_CONTROLLER_CONFIGURATION==USB_DUAL_HOST_CONTROLLER_XDK)
#pragma message("OHCD: Dual Host Controller for Silver XDK boxes")
#endif

//----------------------------------------------------------------------------
// Interrupt objects for each HC - this cannot come from the pool,
// because it has code in it, and the pool is not executable.
//----------------------------------------------------------------------------
KINTERRUPT OHCD_InterruptObject[HCD_MAX_HOST_CONTROLLERS];

//----------------------------------------------------------------------------
// Forward declaration of functions defined and used only this in this module
//----------------------------------------------------------------------------
USBD_STATUS
FASTCALL
OHCD_fOpenEndpoint(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    );

USBD_STATUS
FASTCALL
OHCD_fCloseEndpoint(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    );
USBD_STATUS
FASTCALL
OHCD_fGetEndpointState(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    );

USBD_STATUS
FASTCALL
OHCD_fSetEndpointState(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    );
USBD_STATUS
FASTCALL
OHCD_fAbortEndpoint(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    );
VOID
FASTCALL
OHCD_fSetResources(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    PPCI_DEVICE_DESCRIPTOR      PciDevice
    );
VOID
FASTCALL
OHCD_fDequeueControlTransfer(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN PURB Urb
    );
VOID
FASTCALL
OHCD_fDequeueBulkTransfer(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN PURB Urb
    );
VOID
FASTCALL
OHCD_fDequeueInterruptTransfer(
    IN POHCD_ENDPOINT Endpoint,
    IN PURB Urb
    );
VOID
FASTCALL
OHCD_fCancelQueuedUrbs(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT Endpoint
    );

VOID
OHCD_ShutDown(
    PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration
    );


//----------------------------------------------------------------------------
// Implementation of all entry points:
//      HCD_DriverEntry
//      HCD_NewHostController
//      HCD_SubmitRequest
//      HCD_CancelRequest
//----------------------------------------------------------------------------
#pragma code_seg(".XPPCINIT")
VOID
HCD_DriverEntry(
    IN PHCD_RESOURCE_REQUIREMENTS ResourceRequirements
    )
/*++

Routine Description:
    
    Performs one time initialization of driver.  Returns the size of the
    HCD portion of the extension.

Arguments:
    
    ResourceRequirements - tells the host controller how many resources
    to preallocate.

Return Value:

    None.

--*/
{
    USB_DBG_ENTRY_PRINT(("Entering HCD_DriverEntry"));
    USB_DBG_TRACE_PRINT(("Built on %s @%s", __DATE__,__TIME__));
    
    OHCD_fPoolInit(ResourceRequirements);
    
    USB_DBG_EXIT_PRINT(("Exiting HCD_DriverEntry"));
    return;
}

#ifdef SILVER
VOID
HCD_EnumHardware()
/*++
Routine Description:

  The HCD layer is responsible for calling HAL to find its hardware.
  This layer must do it, because only this layer knows the PCI PnP ID
  information about the proper host controller type.

  We call back USBD_NewHostController which allocates the USBD portion
  of the host controller and calls back HCD_NewHostController to
  initialize the hardware.

--*/

{
    PCI_DEVICE_DESCRIPTOR pciDevices[HCD_MAX_HOST_CONTROLLERS];
    ULONG hostControllerIndex=0;
    ULONG hostControllerCount=HCD_MAX_HOST_CONTROLLERS;
    //
    //  Initialize pciDevices array to HAL what we are looking for.
    //
    while(hostControllerCount--)
    {
        pciDevices[hostControllerCount].Bus=(ULONG)-1;
        pciDevices[hostControllerCount].Slot=(ULONG)-1;
        pciDevices[hostControllerCount].VendorID=(USHORT)-1;
		pciDevices[hostControllerCount].DeviceID=(USHORT)-1;
        pciDevices[hostControllerCount].BaseClass=PCI_CLASS_SERIAL_BUS_CTLR;
        pciDevices[hostControllerCount].SubClass=PCI_SUBCLASS_SB_USB;
        pciDevices[hostControllerCount].ProgIf=OHCI_PCI_PROGRAM_INTERFACE;
    }
    //
    //  Ask HAL to find our controllers
    //
    hostControllerCount=HCD_MAX_HOST_CONTROLLERS;
    HalSetupPciDevice(pciDevices, &hostControllerCount);
    
    //
    //  Kick off initialization by notifying USBD about each device.
    //  (Be sure not to change the order that we got from HAL.)
    //
    while(hostControllerIndex < hostControllerCount)
    {
        USBD_NewHostController(pciDevices+hostControllerIndex, sizeof(OHCD_DEVICE_EXTENSION));
        hostControllerIndex++;
    }
}
#else //!SILVER
VOID
HCD_EnumHardware()
/*++
Routine Description:

  On the real hardware the resource information is just hardcoded.
  HCD_MAX_HOST_CONTROLLERS is defined for 1 or 2.

  We call back USBD_NewHostController which allocates the USBD portion
  of the host controller and calls back HCD_NewHostController to
  initialize the hardware.

--*/

{
    PCI_DEVICE_DESCRIPTOR pciDevice;

    //Just return if this is Xbox MCP version A1.  USB cause interrupt storms on this
    //revision and doesn't work at all anyway.
    if (XboxHardwareInfo->McpRevision == 0xa1) return;

    pciDevice.ResourceData.Address[0].Type = CmResourceTypeMemory;
    pciDevice.ResourceData.Address[0].u.Memory.Length = XPCICFG_USB0_MEMORY_REGISTER_LENGTH_0;
    pciDevice.ResourceData.Address[0].u.Memory.TranslatedAddress = (PVOID)XPCICFG_USB0_MEMORY_REGISTER_BASE_0;
    pciDevice.ResourceData.Interrupt.Vector = HalGetInterruptVector(XPCICFG_USB0_IRQ, &pciDevice.ResourceData.Interrupt.Irql);
    USBD_NewHostController(&pciDevice, sizeof(OHCD_DEVICE_EXTENSION));
    if(1==HCD_MAX_HOST_CONTROLLERS) return;

    pciDevice.ResourceData.Address[0].Type = CmResourceTypeMemory;
    pciDevice.ResourceData.Address[0].u.Memory.Length = XPCICFG_USB1_MEMORY_REGISTER_LENGTH_0;
    pciDevice.ResourceData.Address[0].u.Memory.TranslatedAddress = (PVOID)XPCICFG_USB1_MEMORY_REGISTER_BASE_0;
    pciDevice.ResourceData.Interrupt.Vector = HalGetInterruptVector(XPCICFG_USB1_IRQ, &pciDevice.ResourceData.Interrupt.Irql);
    USBD_NewHostController(&pciDevice, sizeof(OHCD_DEVICE_EXTENSION));
}
#endif //!SILVER
    

NTSTATUS
HCD_NewHostController(
    IN PVOID  HcdExtension,
    IN UCHAR  HostControllerNumber,
    IN PPCI_DEVICE_DESCRIPTOR PciDevice
    )
/*++

Routine Description:
    
    When the USBD goes through HAL

Arguments:
    
    DeviceObject - device object associated with host controller.
    ResourceList - list of resources obtained from PCI bus.

Return Value:

    STATUS_SUCCESS.  Otherwise we will KeBugcheck before returning.

--*/
{
    HC_COMMAND_STATUS           HcCommandStatus;
    KIRQL                       oldIrql;
    POHCD_DEVICE_EXTENSION      deviceExtension = (POHCD_DEVICE_EXTENSION)HcdExtension;
    HC_FRAME_INTERVAL           frameInterval;
    BOOLEAN                     interruptConnected;
    POHCI_OPERATIONAL_REGISTERS operationalRegisters = NULL;

    //ULONG frameIntervalSniff1;
    //ULONG frameIntervalSniff2;
    //ULONG frameIntervalSniff3;
    //ULONG frameIntervalSniff4;
    //ULONG frameIntervalSniff5;

    
    USB_DBG_ENTRY_PRINT(("Entering HCD_NewHostController"));
    
    PROFILE_DECLARE_TIME_STAMP(ResetTiming);

    //
    //  Before we go initializing fields zero the whole extension.
    //  That way we can use a NULL check on pointers to see if something
    //  has been initialized yet.
    //  (Not necessary, USBD does this for us.)
    //RtlZeroMemory( (PVOID)deviceExtension, sizeof(OHCD_DEVICE_EXTENSION));

    //
    //  Remember the host controller number, it will come in
    //  handy when need to search the global TD pool for stuff
    //  that our hardware lost!
    //  (one based for USBD, but we want 0 based.)
    deviceExtension->HostControllerNumber = --HostControllerNumber;

    //
    //  Set the resources for the card.
    //
    OHCD_fSetResources(deviceExtension, PciDevice);

    //frameIntervalSniff1 = READ_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcFmInterval.ul);
    //
    //  Do a quick check of version and stuff.
    //  This should only happen on preliminary development
    //  system hardware. It is used to verify that
    //  a development system is using an XBOX compatible 
    //  USB driver.  It will also be used on XBOX
    //  to verify that the hardware is what we think it
    //  should be.
    //
    OHCD_DEVSYS_CHECK_HARDWARE(deviceExtension);

    //
    //  On a development system without the final XBOX
    //  BIOS, there may be an SMM driver or BIOS
    //  driver using the Host Controller.  We need
    //  to take control.
    //
    OHCD_DEVSYS_TAKE_CONTROL(deviceExtension);

    //
    //  Get the operational registers
    //
    operationalRegisters = deviceExtension->OperationalRegisters;

#ifdef OHCD_XBOX_HARDWARE_ONLY
    //
    //  Set up the few root hub registers that should be done while
    //  the host controller is in the reset state (or before reseting).
    //
    WRITE_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorA.ul, HC_RH_DESCRIPTOR_A_INIT_XBOX);
    WRITE_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorB.ul, HC_RH_DESCRIPTOR_B_INIT_XBOX);
    WRITE_REGISTER_ULONG(&operationalRegisters->HcRhStatus.ul, HC_RH_STATUS_INIT_XBOX);
#endif

    //
    //  Initialize the IsrDPC
    //
    KeInitializeDpc(&deviceExtension->IsrDpc, OHCD_IsrDpc, deviceExtension);
    
    //
    //  Get the HCCA and the physical address of it
    //
    deviceExtension->HCCA = OHCD_PoolGetHcca(HostControllerNumber);
    
    //
    //  Reset the host controller.
    //  
    //  TIMING NOTES:
    //      After writing the reset bit we must wait 10us before setting the hardware registers.
    //      However, we must complete setup and set it back to operational within 2 ms.
    //      We can easily do this in under 2ms as along as our quantum doesn't run out so
    //      raise IRQL to prevent it.
    //
    //
    HcCommandStatus.ul = READ_REGISTER_ULONG(&operationalRegisters->HcCommandStatus.ul);
    HcCommandStatus.HostControllerReset = 1;
    PROFILE_BEGIN_TIMING(ResetTiming);
    oldIrql = KeRaiseIrqlToDpcLevel();
    WRITE_REGISTER_ULONG(&operationalRegisters->HcCommandStatus.ul, HcCommandStatus.ul);
    
    //
    //  Delay for 10 us  
    //
    KeStallExecutionProcessor(10);

    //frameIntervalSniff2 = READ_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcFmInterval.ul);

    //
    //  Set up periodic schedule and HCCA
    //
    OHCD_ScheduleInitialize(deviceExtension);

    //frameIntervalSniff3 = READ_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcFmInterval.ul);
    //
    //  See header file with HC_CONTROL_REGISTER_START
    //  it saves a few instruction rather than using the
    //  bit fields to construct the ULONG.
    //
    //  Basically, we set the state to operational, disable all of the lists,
    //  and initialize things like wakeup behavior.
    //
    WRITE_REGISTER_ULONG(&operationalRegisters->HcControl.ul,HC_CONTROL_REGISTER_START|HC_CONTROL_ISOCH_ENABLE_STATE);
    
    
    //
    //  Apparently the FullSpeedMaximumPacketSize gets reset when moving to the
    //  operational state. This is slightly different then what the OpenHCI 
    //  specification seems to imply.  I read it has reseting this value to its
    //  default of 0x2edf when the
    //  So we moved these couple of lines out of OHCD_ScheduleInitialize
    //  and put them here.
    //
    
    
    frameInterval.ul = READ_REGISTER_ULONG(&operationalRegisters->HcFmInterval.ul);
    //frameIntervalSniff4 = frameInterval.ul;
    frameInterval.FrameInterval = OHCI_DEFAULT_FRAME_INTERVAL;
    frameInterval.FullSpeedMaximumPacketSize = OHCI_CLOCKS_TO_BITS(OHCI_DEFAULT_FRAME_INTERVAL);
    frameInterval.FrameIntervalToggle ^= 1;
    WRITE_REGISTER_ULONG(&operationalRegisters->HcFmInterval.ul, frameInterval.ul);
    

    //
    //  The 2ms time limit is up.  If profiling
    //  is turned let's make sure we made it
    //  within a comfortable margin.
    //  We are not worried about being interrupt
    //  so we can lower the IRQL again.
    //
    PROFILE_END_TIMING(ResetTiming);
    KeLowerIrql(oldIrql);

    //frameIntervalSniff5 = READ_REGISTER_ULONG(&deviceExtension->OperationalRegisters->HcFmInterval.ul);

    //
    // Setup the interrupt vector
    //
    KeInitializeInterrupt (
        &OHCD_InterruptObject[HostControllerNumber],
        (PKSERVICE_ROUTINE) OHCD_InterruptService,            
        (PVOID) deviceExtension,                        
        PciDevice->ResourceData.Interrupt.Vector,
        PciDevice->ResourceData.Interrupt.Irql,
        LevelSensitive,
        TRUE
        );

    interruptConnected = KeConnectInterrupt(&OHCD_InterruptObject[HostControllerNumber]);
    
    ASSERT(interruptConnected && "Failed to connect to interrupt");
    
    USB_DBG_TRACE_PRINT(("Interrupt Resource Set:"));
    USB_DBG_TRACE_PRINT(("  Irql(Level) = %d", PciDevice->ResourceData.Interrupt.Irql));
    USB_DBG_TRACE_PRINT(("  Vector      = %d", PciDevice->ResourceData.Interrupt.Vector));
    USB_DBG_TRACE_PRINT(("  Assigned InterruptObject at 0x%0.8x", &OHCD_InterruptObject[HostControllerNumber]));
    USB_DBG_TRACE_PRINT(("  IoConnectInterrupt returned 0x%0.8x", ntStatus));

    //
    //  Register for shutdown notification (now that we have an interrupt object,
    //  but before enabling interrupts.
    //

    deviceExtension->ShutdownRegistration.NotificationRoutine = OHCD_ShutDown;
    deviceExtension->ShutdownRegistration.Priority = 1;
    InitializeListHead(&deviceExtension->ShutdownRegistration.ListEntry);
    HalRegisterShutdownNotification(&deviceExtension->ShutdownRegistration, TRUE);

    //
    //  Enable Interrupts
    //
    WRITE_REGISTER_ULONG(&operationalRegisters->HcInterruptEnable, 
                            HCINT_SchedulingOverrun |
                            HCINT_WritebackDoneHead |
                            HCINT_UnrecoverableError |
                            HCINT_FrameNumberOverflow |
                            HCINT_MasterInterruptEnable
                            );

    //USB_DBG_WARN_PRINT(("frameIntervalSniff1 = 0x%0.8x", frameIntervalSniff1));
    //USB_DBG_WARN_PRINT(("frameIntervalSniff2 = 0x%0.8x", frameIntervalSniff2));
    //USB_DBG_WARN_PRINT(("frameIntervalSniff3 = 0x%0.8x", frameIntervalSniff3));
    //USB_DBG_WARN_PRINT(("frameIntervalSniff4 = 0x%0.8x", frameIntervalSniff4));
    //USB_DBG_WARN_PRINT(("frameIntervalSniff5 = 0x%0.8x", frameIntervalSniff5));

    //
    //  Now kick of detection of devices, by initializing the root hub.
    //
    OHCD_RootHubInitialize(deviceExtension);


    USB_DBG_EXIT_PRINT(("Exiting HCD_NewHostController"));
    return STATUS_SUCCESS;
}
#pragma code_seg(".XPPCODE")

USBD_STATUS
HCD_SubmitRequest(
    IN PVOID        HcdDeviceExtension,
    IN PURB         Urb
    )
/*++

Routine Description:

   Process URBs from the USBD layer.

Arguments:

   DeviceObject - pointer to a device object

   HcdUrb - pointer to a USB Request Block

Return Value:


--*/
{
    USBD_STATUS             usbdStatus = USBD_STATUS_SUCCESS;
    POHCD_DEVICE_EXTENSION  deviceExtension = (POHCD_DEVICE_EXTENSION)HcdDeviceExtension;
    
    USB_DBG_ENTRY_PRINT(("Entering HCD_SubmitRequest"));
    switch (Urb->Header.Function)
    {
        //
        // Open Endpoint and Close Endpoint IRPs are serialized
        // within USBD so we can execute them now.
        //
        case URB_FUNCTION_OPEN_ENDPOINT:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_HCD_OPEN_ENDPOINT"));
            usbdStatus = OHCD_fOpenEndpoint(deviceExtension, Urb);
            break;
        case URB_FUNCTION_CLOSE_ENDPOINT:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_HCD_CLOSE_ENDPOINT"));
            usbdStatus = OHCD_fCloseEndpoint(deviceExtension, Urb);
            break;
        case URB_FUNCTION_CONTROL_TRANSFER:
        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_<XXXX>_TRANSFER"));
            ASSERT(Urb->CommonTransfer.EndpointHandle);
            usbdStatus = OHCD_fQueueTransferRequest(deviceExtension, Urb);
            break;
        case URB_FUNCTION_GET_FRAME_NUMBER:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_GET_CURRENT_FRAME_NUMBER"));
            Urb->GetFrame.FrameNumber = OHCD_Get32BitFrameNumber(deviceExtension);
            usbdStatus = USBD_STATUS_SUCCESS;
            break;
        case URB_FUNCTION_GET_ENDPOINT_STATE:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_HCD_GET_ENDPOINT_STATE"));
            usbdStatus=OHCD_fGetEndpointState(deviceExtension, Urb);
            break;
        case URB_FUNCTION_SET_ENDPOINT_STATE:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_HCD_SET_ENDPOINT_STATE"));
            usbdStatus=OHCD_fSetEndpointState(deviceExtension, Urb);
            break;
        case URB_FUNCTION_ABORT_ENDPOINT:
            USB_DBG_TRACE_PRINT(("URB_FUNCTION_HCD_ABORT_ENDPOINT"));
            usbdStatus=OHCD_fAbortEndpoint(deviceExtension, Urb);
            break;
        case URB_FUNCTION_ISOCH_OPEN_ENDPOINT:
            usbdStatus=OHCD_ISOCH_OPEN_ENDPOINT(deviceExtension, Urb);
            break;
        case URB_FUNCTION_ISOCH_CLOSE_ENDPOINT:
            usbdStatus=OHCD_ISOCH_CLOSE_ENDPOINT(deviceExtension, Urb);
            break;                                
        case URB_FUNCTION_ISOCH_ATTACH_BUFFER:
            usbdStatus=OHCD_ISOCH_ATTACH_BUFFERS(deviceExtension, Urb);
            break;
        case URB_FUNCTION_ISOCH_START_TRANSFER:
            usbdStatus=OHCD_ISOCH_START_TRANSFER(deviceExtension, Urb);
            break;
        case URB_FUNCTION_ISOCH_STOP_TRANSFER:
            usbdStatus=OHCD_ISOCH_STOP_TRANSFER(deviceExtension, Urb);
            break;
        default:
            ASSERT(FALSE && "Unsupported URB");
            usbdStatus = USBD_STATUS_INVALID_URB_FUNCTION;
    }

    //
    //  If we are not returning a pending status
    //  than we should call CompleteUsbRequest.
    //  
    if(!USBD_PENDING(usbdStatus ))
    {
        Urb->Header.Status = usbdStatus;
        USBD_CompleteRequest(Urb);
    }
    USB_DBG_EXIT_PRINT(("Exiting HCD_SubmitRequest: usbdStatus = 0x%0.8x", usbdStatus));
    return usbdStatus;
}


USBD_STATUS
HCD_CancelRequest(
    IN PVOID    HcdDeviceExtension,
    IN PURB     Urb
    )
/*++

Routine Description:

    Entry point called by USBD to cancel a transfer request.
    
    The URB could be either in a queue waiting to be programmed,
    or already programmed.
    
    If it is in a queue, we can find it, remove it, and complete
    as canceled right here.

    If it has been programmed, then we need to pause the endpoint,
    and place the Urb on the PendingCancels list.  On the next
    frame, we can access the endpoint from the DPC and cancel the URB.

Arguments:
    
    DeviceExtension - DeviceObject for this USB controller.

    HcdUrb          - Pointer to previously submitted URB to cancel

  Return Value:
        USBD_STATUS_CANCELED    - Urb has been canceled.
        USBD_STATUS_CANCELING   - Urb is in process of being canceled.
--*/
{
    POHCD_ENDPOINT          endpoint;
    USBD_STATUS             status = USBD_STATUS_CANCELED;
    POHCD_DEVICE_EXTENSION  deviceExtension = (POHCD_DEVICE_EXTENSION)HcdDeviceExtension;
    KIRQL                   oldIrql;

    USB_DBG_ENTRY_PRINT(("Entering HCD_CancelRequest"));
    //
    //  Only control, bulk and interrupt transfers can be cancelled.
    //  if the URB is not a transfer (or an isochronous transfer).
    //  There is a bug in the caller's code (USBD, or class driver).
    //
    ASSERT( 
        (URB_FUNCTION_CONTROL_TRANSFER == Urb->Header.Function) ||
        (URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER == Urb->Header.Function)
        );

    //
    //  It is not inconceivable that an URB would legitimately get canceled twice.
    //  Just ignore it if it happens.
    //
    if(Urb->CommonTransfer.Hca.HcdUrbFlags & OHCD_URB_FLAG_CANCELED)
    {
        return USBD_STATUS_CANCELING;
    }

    //
    //  We need to be at dispatch level while handling
    //  queues and stuff.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  If the URB has been queued, but not yet programmed
    //  we can handle everything here and now.
    endpoint = (POHCD_ENDPOINT)Urb->CommonTransfer.EndpointHandle;
    
    //
    //  If the endpoint is in the process of being closed
    //  ignore the cancel request.
    //
    if(OHCD_ENDPOINT_FLAG_CLOSING&endpoint->Flags)
    {
        status = USBD_STATUS_CANCELING;
        goto ExitHCD_CancelRequest;
    }

    if(Urb->CommonTransfer.Hca.HcdUrbFlags & OHCD_URB_FLAG_QUEUED)
    {
        //
        //  Dispatch to the proper routine to dequeue the URB
        //
        switch(endpoint->EndpointType)
        {
            case USB_ENDPOINT_TYPE_CONTROL:
                OHCD_fDequeueControlTransfer(deviceExtension, Urb);
                break;
            case USB_ENDPOINT_TYPE_BULK:
                OHCD_fDequeueBulkTransfer(deviceExtension, Urb);
                break;
            case USB_ENDPOINT_TYPE_INTERRUPT:
                OHCD_fDequeueInterruptTransfer(endpoint, Urb);
                break;
            default:
                //ISOCH endpoint never expected here.
                USB_DBG_ERROR_PRINT(("Unrecognized or Unsupported endpoint type in HCD_CancelRequest"));
                //Execution should be able to continue.
                status = USBD_STATUS_REQUEST_FAILED;
                goto ExitHCD_CancelRequest;
        }
        endpoint->QueuedUrbCount--;
        Urb->Header.Status = USBD_STATUS_CANCELED;
        Urb->CommonTransfer.Hca.HcdUrbFlags |= OHCD_URB_FLAG_CANCELED;
        USBD_CompleteRequest(Urb);
        status = USBD_STATUS_SUCCESS;
    } else 
    {
        //
        //  If we are not queued than we ought to be programmed.
        //
        ASSERT(Urb->CommonTransfer.Hca.HcdUrbFlags & OHCD_URB_FLAG_PROGRAMMED);

        //
        //  Mark the URB canceled.
        //
        Urb->CommonTransfer.Hca.HcdUrbFlags |= OHCD_URB_FLAG_CANCELED;

        //
        //  Add the URB to the PendingCancels list.
        //
        Urb->CommonTransfer.Hca.HcdUrbLink = deviceExtension->PendingCancels;
        deviceExtension->PendingCancels = Urb;

        //
        //  Pause the endpoint, the IsrDPC
        //  will take care of the rest.
        //
        OHCD_fPauseEndpoint(deviceExtension, endpoint);
        status = USBD_STATUS_CANCELING;
    }
    
    //
    //  Done with queues and stuff
    //
ExitHCD_CancelRequest:
    KeLowerIrql(oldIrql);
    
    USB_DBG_EXIT_PRINT(("Exiting HCD_CancelRequest: status = 0x%0.8x", status));
    return status;
}

USBD_STATUS
FASTCALL
OHCD_fOpenEndpoint(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    )
/*++

Routine Description:

    Opens an endpoint.  This is a multistep process:

    1)  Get an endpoint

    2)  Zero the memory.
      
    3)  Initialize the endpoint header
    
    4)  Insert Endpoint in schedule.
	5)  Update the handle, or cleanup if something failed.

Arguments:
    
    DeviceExtension - DeviceObject for this USB controller.

    HcdUrb          - Pointer to URB which contains an open endpoint 
                        request

  Return Value:

    Error code several failure cases:
    1) Cannot allocate Endpoint.
    2) Cannot reserved descriptor blocks
    3) Not enough bandwidth in schedule (isoch and interrupt only)

--*/
{
    POHCD_ENDPOINT  newEndpoint = NULL;
    USBD_STATUS     status = USBD_STATUS_SUCCESS;
    KIRQL           oldIrql;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_OpenEndpoint"));

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  1) Allocate an endpoint from the pool.
    //
    newEndpoint = OHCD_PoolAllocateEndpoint();
    if(!newEndpoint)
    {
        USB_DBG_WARN_PRINT(("Attempt to exceed registered endpoints."));
        status = USBD_STATUS_NO_MEMORY;
        goto ExitOpenEndpoint;
    }

    //
    // 2) Zero memory and set flags
    //
    USB_DBG_ENTRY_PRINT(("Zeroing structure."));
    RtlZeroMemory( (PVOID)newEndpoint, sizeof(OHCD_ENDPOINT));

    //
    //  It is helpful to cache our physical address
    //
    newEndpoint->PhysicalAddress = OHCD_PoolGetPhysicalAddress(newEndpoint);

    //
    //  Save off the endpoint type
    //
    newEndpoint->EndpointType = Urb->OpenEndpoint.EndpointType;

    //
    //  Fill out polling interval, and bandwidth
    //
    newEndpoint->PollingInterval = Urb->OpenEndpoint.Interval;
    newEndpoint->Bandwidth = USBD_CalculateUsbBandwidth(
                                    Urb->OpenEndpoint.MaxPacketSize,
                                    newEndpoint->EndpointType,
                                    Urb->OpenEndpoint.LowSpeed
                                    );

    //
    //  Fill out everything in the control structure.
    //  Note the Direction here is set correctly for non-control endpoints.
    //  in the next step we have an if USB_ENDPOINT_TYPE_CONTROL and we fix it
    //  for the control endpoints.
    //
    newEndpoint->HcEndpointDescriptor.Control.FunctionAddress = Urb->OpenEndpoint.FunctionAddress;
    newEndpoint->HcEndpointDescriptor.Control.EndpointAddress = Urb->OpenEndpoint.EndpointAddress;
    if(USB_ENDPOINT_TYPE_CONTROL == newEndpoint->EndpointType)
    {
        newEndpoint->HcEndpointDescriptor.Control.Direction = 0;
    }else
    {
        newEndpoint->HcEndpointDescriptor.Control.Direction = (USB_ENDPOINT_DIRECTION_MASK & Urb->OpenEndpoint.EndpointAddress) ? 2 : 1;
    }
    newEndpoint->HcEndpointDescriptor.Control.Speed =  Urb->OpenEndpoint.LowSpeed;
    newEndpoint->HcEndpointDescriptor.Control.Skip = 1;
    newEndpoint->HcEndpointDescriptor.Control.Format = 0;
    newEndpoint->HcEndpointDescriptor.Control.MaximumPacketSize = Urb->OpenEndpoint.MaxPacketSize;

    //
    //  We do not have any URBs now either.
    //
    /*  Unneccessary, we zeroed the structure right after allocating it.
    newEndpoint->PendingUrbHeadP =
    newEndpoint->PendingUrbTailP =
    newEndpoint->PendingUrbCurrentP = NULL;
    */

    //
    //  3) Initialize Endpoint descriptor.
    //
    newEndpoint->HcEndpointDescriptor.NextED = 0;
    newEndpoint->HcEndpointDescriptor.HeadPHaltCarry = 0;
    newEndpoint->HcEndpointDescriptor.TailP = 0;

    //
    //  Set the initial data toggle
    //
    if(Urb->OpenEndpoint.DataToggleBits)
    {
        ULONG dataToggleMask = 1 << newEndpoint->HcEndpointDescriptor.Control.EndpointAddress;
        if(newEndpoint->HcEndpointDescriptor.Control.Direction == 2)
        {
            dataToggleMask <<= 16;
        }
        if((*Urb->OpenEndpoint.DataToggleBits)&dataToggleMask)
        {
            SET_TOGGLECARRY(&newEndpoint->HcEndpointDescriptor);
        }
    }

    //
    //  4) Insert endpoint into schedule, and
    //
    if( USB_ENDPOINT_TYPE_CONTROL == newEndpoint->EndpointType ||
        USB_ENDPOINT_TYPE_BULK == newEndpoint->EndpointType )
    //
    //  Control or bulk
    //
    {
        OHCD_ScheduleAddEndpointControlOrBulk(DeviceExtension, newEndpoint);
    }
    else
    //
    //  Interrupt
    //
    {
        ASSERT(USB_ENDPOINT_TYPE_INTERRUPT == newEndpoint->EndpointType);
        //
        //  Scheduling Periodic Endpoints can fail due to lack of bandwidth.
        //
        status = OHCD_ScheduleAddEndpointPeriodic(DeviceExtension, newEndpoint);
    }

    //
    //  5) Udpate URB handle
    //
    if(USBD_SUCCESS(status))
    {
        Urb->OpenEndpoint.EndpointHandle = newEndpoint;
    }
    else
    {
        Urb->OpenEndpoint.EndpointHandle = NULL;
        OHCD_PoolFreeEndpoint(newEndpoint);
    }

ExitOpenEndpoint:
    Urb->Header.Status = status;
    KeLowerIrql(oldIrql);
    USB_DBG_EXIT_PRINT(("Exiting OHCD_OpenEndpoint: usbdStatus = 0x%0.8x", status));
    return status;
}

USBD_STATUS
FASTCALL
OHCD_fCloseEndpoint(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    )
{
    POHCD_ENDPOINT  endpoint = (POHCD_ENDPOINT)Urb->CloseEndpoint.EndpointHandle;
    KIRQL           oldIrql;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_CloseEndpoint"));

    oldIrql = KeRaiseIrqlToDpcLevel();
    endpoint->Flags |= OHCD_ENDPOINT_FLAG_CLOSING;

    //
    //  Cancel any queued URBs (but not programmed) right now.
    //
    OHCD_fCancelQueuedUrbs(DeviceExtension, endpoint);

    //
    //  Remove endpoint from schedule
    //
    ASSERT(USB_ENDPOINT_TYPE_ISOCHRONOUS != endpoint->EndpointType);
    if( USB_ENDPOINT_TYPE_CONTROL == endpoint->EndpointType ||
        USB_ENDPOINT_TYPE_BULK == endpoint->EndpointType )
    //
    //  Control or bulk
    //
    {
        OHCD_ScheduleRemoveEndpointControlOrBulk(DeviceExtension, endpoint);
    }
    else
    //
    //  Interrupt
    //
    {
        OHCD_ScheduleRemoveEndpointPeriodic(DeviceExtension, endpoint);
    }

    //
    //  Add to endpoint reclamation queue, we do not need to pause it,
    //  but the pause routine is convenient.
    //
    OHCD_fPauseEndpoint(DeviceExtension, endpoint);
    
    //
    //  Link the URB into the list of pending closes.
    //

    ASSERT(DeviceExtension->PendingCloses != Urb);
    
    Urb->CloseEndpoint.HcdNextClose = DeviceExtension->PendingCloses;
    DeviceExtension->PendingCloses = Urb;
    
    KeLowerIrql(oldIrql);

    USB_DBG_EXIT_PRINT(("Exiting OHCD_CloseEndpoint: usbdStatus = USBD_STATUS_PENDING"));
    return USBD_STATUS_PENDING;
}

USBD_STATUS
FASTCALL
OHCD_fGetEndpointState(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    )
/*++

Routine Description:

    Gets the endpoint state.  The state consists of two flags:

        USB_ENDPOINT_TRANSFERS_QUEUED
        USB_ENDPOINT_HALTED

Arguments:

    DeviceExtension - DeviceObject for this USB controller.

    HcdUrb          - Pointer to URB that we are servicing.

  Return Value:

    USBD_STATUS_SUCCESS

--*/

{
    POHCD_ENDPOINT endpoint = (POHCD_ENDPOINT)Urb->GetSetEndpointState.EndpointHandle;
    
    USB_DBG_ENTRY_PRINT(("Entering OHCD_GetEndpointState"));
    USB_DBG_TRACE_PRINT(("Endpoint state flags:"));
    //
    //  Get whether or not is halted
    //
    Urb->GetSetEndpointState.EndpointState = 
        GET_HALTED(&endpoint->HcEndpointDescriptor) ? USB_ENDPOINT_STATE_HALTED : 0;

    USB_DBG_TRACE_PRINT(("%s",(Urb->GetSetEndpointState.EndpointState&USB_ENDPOINT_STATE_HALTED)?"     USB_ENDPOINT_HALTED":NULL));

    //
    //  Now set the USB_ENDPOINT_TRANSFERS_QUEUED flag.
    //
    
    if(endpoint->QueuedUrbCount || endpoint->ProgrammedUrbCount)
    {
        Urb->GetSetEndpointState.EndpointState |= USB_ENDPOINT_STATE_TRANSFERS_QUEUED;
        USB_DBG_TRACE_PRINT(("     USB_ENDPOINT_STATE_TRANSFERS_QUEUED"));
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_GetEndpointState"));
    return USBD_STATUS_SUCCESS;
}

USBD_STATUS
FASTCALL
OHCD_fSetEndpointState(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    )
/*++

Routine Description:

    Sets the endpoint state.  Basically, it can do only do things.
    Restart a halted endpoint.  Reset the data toggle on an endpoint.

    1) If USB_ENDPOINT_RESET_DATA_TOGGLE is set, we will reset the data toggle.
    2) If USB_ENDPOINT_HALTED is clear, we clear the halt bit.

Arguments:
    
    DeviceExtension - DeviceObject for this USB controller.

    HcdUrb          - Pointer to URB that we are servicing.

  Return Value:

    USBD_STATUS_SUCCESS

--*/
{
    POHCD_ENDPOINT endpoint = (POHCD_ENDPOINT)Urb->GetSetEndpointState.EndpointHandle;
    ULONG state = Urb->GetSetEndpointState.EndpointState;
    
    USB_DBG_ENTRY_PRINT(("Entering OHCD_SetEndpointState"));
    if(state & USB_ENDPOINT_STATE_DATA_TOGGLE_RESET)
    {
        USB_DBG_TRACE_PRINT(("Clearing the toggle carry."));
        CLEAR_TOGGLECARRY(&endpoint->HcEndpointDescriptor);
    }
    if(state & USB_ENDPOINT_STATE_DATA_TOGGLE_SET)
    {
        USB_DBG_TRACE_PRINT(("Setting the toggle carry."));
        SET_TOGGLECARRY(&endpoint->HcEndpointDescriptor);
    }
    if(!(state & USB_ENDPOINT_STATE_KEEP_HALT) && GET_HALTED(&endpoint->HcEndpointDescriptor))
    {
        USB_DBG_TRACE_PRINT(("Clearing a halt condition."));
        CLEAR_HALTED(&endpoint->HcEndpointDescriptor);
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_SetEndpointState"));
    return USBD_STATUS_SUCCESS;
}

USBD_STATUS
FASTCALL
OHCD_fAbortEndpoint(
    IN      POHCD_DEVICE_EXTENSION  DeviceExtension,
    IN OUT  PURB                    Urb
    )
/*++

Routine Description:

   Cancels all of the requests associated with a particular
   endpoint.

Arguments:
    
    DeviceExtension - DeviceObject for this USB controller.

    Urb             - Pointer to URB that we are servicing.

  Return Value:

    USBD_STATUS_SUCCESS

--*/
{
//
    POHCD_ENDPOINT  endpoint = (POHCD_ENDPOINT)Urb->AbortEndpoint.EndpointHandle;
    KIRQL           oldIrql;
    USBD_STATUS     status = USBD_STATUS_SUCCESS;
    
    oldIrql = KeRaiseIrqlToDpcLevel();
    //
    //  Cancel any queued URBs (but not programmed) right now.
    //
    OHCD_fCancelQueuedUrbs(DeviceExtension, endpoint);

    //
    //  If there are urbs programmed, we need to queue
    //  the abort urb, and pause the endpoint.
    //
    if(endpoint->ProgrammedUrbCount)
    {
        //
        //  Queue Abort Urb
        //
        Urb->AbortEndpoint.HcdNextAbort = DeviceExtension->PendingAborts;
        DeviceExtension->PendingAborts = Urb;
        OHCD_fPauseEndpoint(DeviceExtension, endpoint);
        status = USBD_STATUS_PENDING;
    }
    KeLowerIrql(oldIrql);

    return status;
}


#pragma code_seg(".XPPCINIT")
VOID
FASTCALL
OHCD_fSetResources(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    PPCI_DEVICE_DESCRIPTOR      PciDevice
    )
/*++

Routine Description:

    This routines uses the Hal routines to configure
    the PCI bus for the card.  And store those settings.
    
    This is two stages:
    1) Initialize out device extension with proper base address.
    2) Create an NT interrupt object.

Arguments:

   DeviceExtension      - DeviceExtension for this USB controller.
   PciDevice            - PCI BAR for device

Return Value:

    None

Failures:
    Assume everything works.  Assert failures.  Basically
    any possible errors are configuration, these shouldn't
    happen.

--*/
{
    USB_DBG_ENTRY_PRINT(("Entering OHCD_SetResources"));
    //
    //  Record memory resource
    //
    ASSERT(PciDevice->ResourceData.Address[0].Type == CmResourceTypeMemory);
    ASSERT(PciDevice->ResourceData.Address[0].u.Memory.Length >= sizeof(OHCI_OPERATIONAL_REGISTER_SIZE));
    DeviceExtension->OperationalRegistersLength = PciDevice->ResourceData.Address[0].u.Memory.Length;
    DeviceExtension->OperationalRegisters = 
        (POHCI_OPERATIONAL_REGISTERS)PciDevice->ResourceData.Address[0].u.Memory.TranslatedAddress;
    USB_DBG_TRACE_PRINT(("Virtual Register Address = 0x%0.8x", DeviceExtension->OperationalRegisters));
    
    USB_DBG_EXIT_PRINT(("Exiting OHCD_SetResources"));
    return;
}
#pragma code_seg(".XPPCODE")

VOID
FASTCALL
OHCD_fDequeueControlTransfer(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN PURB Urb
    )
{
    //
    //  Walk the control transfer queue and find the URB
    //
    PURB urbOnQueue = DeviceExtension->ControlUrbHeadP;
    PURB prevUrbOnQueue = NULL;
    while(urbOnQueue != Urb)
    {
        //
        //  Serious error, if we got to the end of the queue and didn't find it.
        //
        ASSERT(urbOnQueue != DeviceExtension->ControlUrbTailP);
        //
        //  Walk link for next iteration
        //
        prevUrbOnQueue = urbOnQueue;
        urbOnQueue = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    }
    
    //
    //  Remove the Urb
    //
    if(NULL == prevUrbOnQueue)
    {
        DeviceExtension->ControlUrbHeadP = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    } else
    {
        prevUrbOnQueue->CommonTransfer.Hca.HcdUrbLink = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    }
    //
    //  If it was the last URB, update the tail pointer too.
    //
    if(Urb == DeviceExtension->ControlUrbTailP)
    {
        DeviceExtension->ControlUrbTailP = prevUrbOnQueue;
    }
}

VOID
FASTCALL
OHCD_fDequeueBulkTransfer(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    IN PURB Urb
    )
{
    //
    //  Walk the control transfer queue and find the URB
    //
    PURB urbOnQueue = DeviceExtension->BulkUrbHeadP;
    PURB prevUrbOnQueue = NULL;
    while(urbOnQueue != Urb)
    {
        //
        //  Serious error, if we got to the end of the queue and didn't find it.
        //
        ASSERT(urbOnQueue != DeviceExtension->BulkUrbTailP);
        //
        //  Walk link for next iteration
        //
        prevUrbOnQueue = urbOnQueue;
        urbOnQueue = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    }
    
    //
    //  Remove the Urb
    //
    if(NULL == prevUrbOnQueue)
    {
        DeviceExtension->BulkUrbHeadP = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    } else
    {
        prevUrbOnQueue->CommonTransfer.Hca.HcdUrbLink = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    }
    //
    //  If it was the last URB, update the tail pointer too.
    //
    if(Urb == DeviceExtension->BulkUrbTailP)
    {
        DeviceExtension->BulkUrbTailP = prevUrbOnQueue;
    }
}

VOID
FASTCALL
OHCD_fDequeueInterruptTransfer(
    IN POHCD_ENDPOINT Endpoint,
    IN PURB Urb
    )
{
    //
    //  Walk the control transfer queue and find the URB
    //
    PURB urbOnQueue = Endpoint->PendingUrbHeadP;
    PURB prevUrbOnQueue = NULL;
    while(urbOnQueue != Urb)
    {
        //
        //  Serious error, if we got to the end of the queue and didn't find it.
        //
        ASSERT(urbOnQueue != Endpoint->PendingUrbTailP);
        //
        //  Walk link for next iteration
        //
        prevUrbOnQueue = urbOnQueue;
        urbOnQueue = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    }
    
    //
    //  Remove the Urb
    //
    if(NULL == prevUrbOnQueue)
    {
        Endpoint->PendingUrbHeadP = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    } else
    {
        prevUrbOnQueue->CommonTransfer.Hca.HcdUrbLink = urbOnQueue->CommonTransfer.Hca.HcdUrbLink;
    }
    //
    //  If it was the last URB, update the tail pointer too.
    //
    if(Urb == Endpoint->PendingUrbTailP)
    {
        Endpoint->PendingUrbTailP = prevUrbOnQueue;
    }
}

VOID
FASTCALL
OHCD_fCancelQueuedUrbs(
    IN POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT Endpoint
    )
/*++

 Routine Description:

    Called by OHCD_AbortEndpoint and OHCD_CloseEndpoint, to
    cancel any Urbs which have been queued, but not yet programmed.
   
    Different types of endpoints have URBs queued in different
    places.  We first switch on the endpoint type, to initialize
    pointers to the queue pointers.

    Then we walk the queue using pointers to pointers.  I will
    be the first to admit that this is style of code is not
    terribly readable.  However, it is efficient, so I tried to
    just comment well.
    
Arguments:

   DeviceExtension      - DeviceExtension for this USB controller.

   Endpoint             - Endpoint to cancel URBs for.

Return Value:

    None

--*/
{

    PURB    *currentUrbP;                       //Pointer to official pointer to the URB we are currently processing
    PURB    *tailUrbP;                          //Pointer to official pointer to the tail URB in queue
    PURB    currentUrb = NULL;                  //Local pointer to current URB we are working.
    PURB    previousUrb;                        //Local pointer to the last URB prior to the current one that was not canceled.
    PVOID   endpointHandle = (PVOID) Endpoint;

    //
    //  We need to synchronize.
    //
    ASSERT_DISPATCH_LEVEL();
        
    //
    //  If there are any Urbs in the queue . . .
    //
    if(Endpoint->QueuedUrbCount)
    {
        //
        //  Where the heads and tails are stored
        //  depends on the endpoint type.
        //
        switch(Endpoint->EndpointType)
        {
            case USB_ENDPOINT_TYPE_CONTROL:
                currentUrbP = &DeviceExtension->ControlUrbHeadP;
                tailUrbP = &DeviceExtension->ControlUrbTailP;
                break;
            case USB_ENDPOINT_TYPE_BULK:
                currentUrbP = &DeviceExtension->BulkUrbHeadP;
                tailUrbP = &DeviceExtension->BulkUrbTailP;
                break;
            case USB_ENDPOINT_TYPE_INTERRUPT:
                currentUrbP = &Endpoint->PendingUrbHeadP;
                tailUrbP = &Endpoint->PendingUrbTailP;
                break;
			default:
                //ISOCH endpoint never expected here.
                USB_DBG_ERROR_PRINT(("Unrecognized endpoint type in OHCD_CancelQueuedUrbs"));
                //This is a serious error, continued execution might be feasible.
                return;
        }
        //
        //  Walk queue and remove any URB associated with Endpoint
        //
        do
        {
            currentUrb = *currentUrbP;
            ASSERT(currentUrb); //A serious coding error if we hit this.
            //
            //  If the URB belongs to this endpoint cancel it.
            //
            if(currentUrb->CommonTransfer.EndpointHandle == endpointHandle)
            {
                //
                //  Whoever was pointing at this urb, should now point to whoever
                //  this urb is pointing to.
                //
                *currentUrbP = currentUrb->CommonTransfer.Hca.HcdUrbLink;
                //
                //  Cancel the Urb
                //
                currentUrb->Header.Status = USBD_STATUS_CANCELED;
                Endpoint->QueuedUrbCount--;
                USBD_CompleteRequest(currentUrb);
            }
            //
            //  Otherwise skip the urb.
            //
            else
            {
                //
                //  This urb is now the official pointer to the next current URB.
                //  Since we didn't cancel it, it also the new previousUrb.
                //
                previousUrb = currentUrb;
                currentUrbP = &currentUrb->CommonTransfer.Hca.HcdUrbLink;
            }
        }while(*tailUrbP != currentUrb);
        //
        //  The official pointer to the tail URB, should now
        //  point to the last URB we didn't cancel.
        //
        *tailUrbP = previousUrb;
        //
        //  That should have dequeued every urb associated with the endpoint.
        //
        ASSERT(0 == Endpoint->QueuedUrbCount);
    }
}

VOID
FASTCALL
OHCD_fPauseEndpoint(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT  Endpoint
    )
/*++

 Routine Description:

    This routines makes sure that endpoint gets paused if it is not already.

Arguments:

   DeviceExtension      - DeviceExtension for this USB controller.

   Endpoint             - Endpoint to pause.

Return Value:

    None

--*/
{
    
    USB_DBG_ENTRY_PRINT(("Entering OHCD_PauseEndpoint"));
    
    ASSERT_DISPATCH_LEVEL();
    
    //
    // We reference count pauses
    //
    Endpoint->PendingPauseCount++;
    //
    //  If a pause is not already in progess,
    //  then pause it.
    if(!(Endpoint->Flags & OHCD_ENDPOINT_FLAG_PAUSING))
    {
        //
        //  Set the Skip bit
        //
        Endpoint->HcEndpointDescriptor.Control.Skip = TRUE;
        //
        //  Set the frame that we need to wait for.
        //
        Endpoint->PauseFrame = OHCD_Get32BitFrameNumber(DeviceExtension) + 1;
        
        //
        // Decide if we need to set the delay pause flag,
        // if are called between an interrupt and the DPC
        // to handle that interupt.  THIS MUST BE AFTER SETTING
        // THE SKIP BIT. The call to OHCD_Get32BitFrameNumber
        // should ensure that the compiler doesn't reorder this.
        //
        if(DeviceExtension->IsrDpc_Context.InterruptsSignaled)
        {
            Endpoint->Flags |= OHCD_ENDPOINT_FLAG_DELAY_PAUSE;
        }

        //
        //  Clear the status indicator for StartOfFrame.
        //
        WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcInterruptStatus, HCINT_StartOfFrame);
        //
        //  Enable the StartOfFrame interrupt
        //
        WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcInterruptEnable, HCINT_StartOfFrame);
        //
        //  Set the OHCD_ENDPOINT_FLAG_PAUSING_FLAG;
        //
        Endpoint->Flags |= OHCD_ENDPOINT_FLAG_PAUSING;
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_PauseEndpoint"));
}


VOID
OHCD_ShutDown(
    PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration
    )
/*++
    Routine Description: 
        Called by the HAL in order to do a quick reboot.  Basically,
        our job is to shut off the host controller, so that no extraneous
        calls happen during shutdown.
    Arguments:
        ShutdownRegistration - Shutdown registration that we passed to 
            HalRegisterShutdown.  We can use CONTAINING_RECORD to
            our device extension.
--*/
{
    POHCD_DEVICE_EXTENSION deviceExtension;
    KIRQL oldIrql;
    
    deviceExtension = CONTAINING_RECORD(
                            ShutdownRegistration,
                            OHCD_DEVICE_EXTENSION,
                            ShutdownRegistration
                            );

    KeRaiseIrql((KIRQL)OHCD_InterruptObject[deviceExtension->HostControllerNumber].Irql, &oldIrql);

    //
    //  Disable all interrupts
    //
    WRITE_REGISTER_ULONG(
        &deviceExtension->OperationalRegisters->HcInterruptDisable,
        HCINT_SchedulingOverrun |
        HCINT_WritebackDoneHead |
        HCINT_UnrecoverableError |
        HCINT_FrameNumberOverflow |
        HCINT_MasterInterruptEnable
       );

    //
    //  Reset the Host Controller
    //
    WRITE_REGISTER_ULONG(
        &deviceExtension->OperationalRegisters->HcControl.ul,
        HC_CONTROL_REGISTER_STOP
        );

    KeLowerIrql(oldIrql);
}
