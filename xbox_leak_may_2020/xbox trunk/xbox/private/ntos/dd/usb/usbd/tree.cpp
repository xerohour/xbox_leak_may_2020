/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    tree.c

Abstract:

    This module contains the code to manage the device tree.
    
    The entry points to this module are:

        Called During DriverEntry:
            CDeviceTree::CDeviceTree

        Called by root-hubs to indicate devices hot-plug events:
            USBD_DeviceConnected
            USBD_DeviceDisconnected
        Called by hubs to indicate devices hot-plug events:
            IUsbDevice::DeviceConnected
            IUsbDevice::DeviceDisconnected
        Called by class drivers to indicate the end of initialization or cleanup:
            IUsbDevice::AddComplete
            IUsbDevice::RemoveComplete
    
Environment:

    kernel mode only

Notes:


Revision History:

    02-11-00 : create by Mitchell Dernis (mitchd)

--*/

//
//  Pull in OS headers
//
#define _XAPI_
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xapidrv.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DBSU'
#include <debug.h>

//
//  Pull in usbd headers
//
#include <usbd.h>

DEFINE_USB_DEBUG_FUNCTIONS("USBD");

//---------------------------------------------------------------------
// Forward declarations of routines defined and used only in this module
//---------------------------------------------------------------------
VOID USBD_DeviceEnumStage0();
VOID USBD_DeviceEnumStagePre1(USBD_STATUS UsbdStatus, PVOID Context);
VOID FASTCALL USBD_fDeviceEnumStage1(IUsbDevice  *Device);
VOID USBD_DeviceEnumStage2(PURB Urb, IUsbDevice *Device);
VOID USBD_DeviceEnumStage3(PURB Urb, IUsbDevice *Device);
VOID USBD_DeviceEnumStagePre4(PURB Urb, IUsbDevice *Device);
VOID FASTCALL USBD_fDeviceEnumStage4(PURB Urb, IUsbDevice *Device);
VOID USBD_DeviceEnumStage5(PURB Urb, IUsbDevice *Device);
VOID USBD_DeviceEnumStage6(PURB Urb, IUsbDevice *Device);
VOID USBD_DeviceAbortEnum(PURB Unreferenced, IUsbDevice *Device);
VOID FASTCALL USBD_fDeviceAbortEnum2(IUsbDevice  *Device);
VOID USBD_DeviceEnumNextPending(PURB Urb, IUsbDevice *Device);
VOID USBD_CompleteDeviceDisconnected(IUsbDevice *Device);
VOID USBD_SetEnumWatchDog();
VOID USBD_ClearEnumWatchDog();

VOID FASTCALL USBD_LoadClassDriver(IUsbDevice *Device, PNP_CLASS_ID ClassId);
UCHAR FASTCALL USBD_AllocateUsbAddress(PUSBD_HOST_CONTROLLER HostController);
VOID FASTCALL USBD_FreeUsbAddress(PUSBD_HOST_CONTROLLER HostController, UCHAR DeviceAddress);

//
//  We do not include the CRT, but we need new.
//  we declare it static, so we don't conflict on linking
//  with other modules overriding new.
//
#pragma warning(disable:4211)
static __inline void * __cdecl operator new(size_t memSize){return RTL_ALLOCATE_HEAP(memSize);}
#pragma warning(default:4211)
//--------------------------------------------
// Declare device tree. (Don't worry about
// the parameter, we call the C'tor explicitely
// from driver entry with the correct value.)
//--------------------------------------------

// HACK: Call the fake default constructor to avoid having the real
//  : constructor being called twice.
//
CDeviceTree g_DeviceTree;

#pragma code_seg(".XPPCINIT")
void 
CDeviceTree::Init(ULONG NodeCount, ULONG MaxCompositeInterfaces)
/*++
Routine Description:

    All of the initialization is done in this init function, because the
    c'tor cannot be called reliably in all link environments.

Arguments:
    
    DeviceExtension - Pointer to USBD device extension.

Return Value:

    None

--*/
{
    m_InProgress = FALSE;
    m_FirstFree = 0;
    m_FirstPendingEnum = NULL;
    m_CurrentEnum = NULL;
    m_NodeCount = NodeCount;
    m_Devices = new IUsbDevice[m_NodeCount];
    m_MaxCompositeInterfaces = MaxCompositeInterfaces;

    for(char nodeIndex = 0; nodeIndex < (m_NodeCount-1); nodeIndex++)
    {
        m_Devices[nodeIndex].m_NextFree = nodeIndex+1;
    }

    KeInitializeDpc(&m_EnumDpc, USBD_DeviceEnumTimerProc, NULL);
    KeInitializeTimer(&m_EnumTimer);
}
#pragma code_seg(".XPPCODE")

VOID
USBD_DeviceConnected(
    IN  PVOID    HcdExtension,
    IN  UCHAR    PortNumber
    )
/*++
    Routine Description:
        Called by the root-hub code in the HCD.  We find
        the device for the root-hub by looking in the DeviceObject,
        and then call the IUsbDevice::DeviceConntected.
--*/
{
    //
    //  Start with a retry count of 5.
    //
    USBD_HostControllerFromHCDExtension(HcdExtension)->RootHub->DeviceConnected(PortNumber, 5);
}

VOID IUsbDevice::DeviceConnected(UCHAR PortNumber, UCHAR RetryCount)
/*++
Routine Description:

    Called by the hub driver or indirectly through USBD_DeviceConnected for the root-hub in the HCD.
    This routine should be called at DISPATCH_LEVEL.  To avoid blocking on device transfers this
    routine kicks off a cascade of routines that enumerate a device.

    1) IUsbDevice::DeviceConnected
        a) Allocate Device Node
        b) Acquire Enum Lock
            i) If not acquired, add node to enum pending list, and return.
        c) start 100ms timer, so that device can settle.
    2) USBD_DeviceEnumStage0
        a) reset port.
    3) USBD_DeviceEnumStage1
        a) open default endpoint
        b) get DeviceDescriptor (8 bytes)
    4) USBD_DeviceEnumStage2
        a) allocate address
        b) set address
    5) USBD_DeviceEnumStage3
        a) close default endpoint
    6) USBD_DeviceEnumStage4
        b) if device requires function driver
            i) load function driver
            ii) Class Driver calls IUsbDevice::AddComplete when it is done enumerating.
        c) if device requires interface driver(s)
            i) open default endpoint
            ii) get configuration descriptor
    7) USBD_DeviceEnumStage5
        a) Set the configuration
    8)  USBD_DeviceEnumStage6
        a) allocate nodes for each interfaces.
        b) load class driver for first interface found.
        c) Class Driver calls IUsbDevice::AddComplete when it is done enumerating.
    9) IUsbDevice::AddComplete
        a) if it is an interface and has more siblings, load a class driver for the next sibling.
            i) Class Driver calls IUsbDevice::AddComplete when it is done enumerating.
        b) Close the default endpoint when all the sibling drivers are loaded, cascading to USBD_DeviceEnumNextPending.
    10) USBD_DeviceEnumNextPending
        a) If there are devices pending enumeration, takes one off the pending list and goes vack to 3)

    Each of the routines ends by sending an asynchronous request down to the HCD.
    The completion routine is always the next routine in the cascade.

Arguments:
    
    PortNumber      - Port number on hub (temporarily high-bit can be set to indicate low speed)
    LowSpeed        - Set if a low speed device was detected.

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{
    ASSERT_DISPATCH_LEVEL();
    
    
    USB_DBG_ENTRY_PRINT(("Entering USBD_DeviceConnected"));
    USB_DBG_TRACE_PRINT(("%s device detected in Port %d of Hub 0x%0.8x", (LowSpeed) ? "LowSpeed" : "FullSpeed", PortNumber, this));
    ASSERT((UDN_TYPE_HUB == m_Type) || (UDN_TYPE_ROOT_HUB == m_Type));

    //
    //  Allocate a device.
    //
    IUsbDevice *newDevice = g_DeviceTree.AllocDevice();
    if(!newDevice)
    {
        USB_DBG_WARN_PRINT(("Number of Devices Attached, exceeds the limits of our tree!"));
        USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceConnected, tree full"));
        return;
    }

    //
    //  Initialize the couple of things we know
    //
    newDevice->m_Type = UDN_TYPE_PENDING_ENUM;
    newDevice->m_PortNumber = PortNumber;
    newDevice->m_ClassDriver = NULL;
	newDevice->m_HostController = m_HostController;
    //
    //  Plug the node into the tree.
    //
    InsertChild(newDevice);

    //
    //  Serialize Device Enumeration
    //
    if(g_DeviceTree.m_InProgress)
    {
        //*
        //* An enumeration is in progress, queue our new node
        //* onto the pending list.
        //*

        //
        //  Set the retry count in the node.
        //
        newDevice->m_RetryCount = RetryCount;
        //
        //  Record the earliest time that enumeration may start.
        //
        newDevice->m_EarliestEnumTime.QuadPart = KeQueryInterruptTime();  //Get a time stamp.
        newDevice->m_EarliestEnumTime.QuadPart += 100*1000;  //Add 100 ms for power up.

        //
        //  Add the new device to the end of the pending list.
        //
        newDevice->m_NextPending = NULL;
        if(!g_DeviceTree.m_FirstPendingEnum)
        {
            g_DeviceTree.m_FirstPendingEnum = newDevice;
        } else
        {
            IUsbDevice *lastPending = g_DeviceTree.m_FirstPendingEnum;
            while(lastPending->m_NextPending) lastPending = lastPending->m_NextPending;
            lastPending->m_NextPending = newDevice;
        }

        //
        //  It is queued, so we can do nothing more.
        //
        USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceConnected, queued pending enum node"));
        return;
    }

    //
    //  We are actually working on this node.
    //
    newDevice->m_Type = UDN_TYPE_ENUMERATING;
    //
    //  Mark the global enum block in progress.
    //  Store the node information we are working on.
    //  
    g_DeviceTree.m_InProgress = TRUE;
    g_DeviceTree.m_DeviceRemoved = FALSE;
    g_DeviceTree.m_CurrentEnum = newDevice;
    g_DeviceTree.m_RetryCount = RetryCount;
    g_DeviceTree.m_EnumStage = USBD_ENUM_DEVICE_CONNECTED;

    //
    //  Until we change it the address is 0
    //
    newDevice->m_Address = 0;

    //
    //  Give newly plugged device a change to settle, 100ms according to specification.
    //
    g_DeviceTree.m_TimerReason = USBD_TIMER_REASON_STAGE0;
    LARGE_INTEGER           waitTime;
    waitTime.QuadPart = -100*10000; //100ms wait
    KeSetTimer(&g_DeviceTree.m_EnumTimer, waitTime, &g_DeviceTree.m_EnumDpc);
}

__inline void USBD_SetEnumWatchDog()
{
    LARGE_INTEGER           waitTime;
    g_DeviceTree.m_TimerReason = USBD_TIMER_REASON_WATCHDOG;
    waitTime.QuadPart = -5000*(LONGLONG)10000; //5 seconds (long, but that is the USB 1.1 specification: section 9.2.6.4)
    KeSetTimer(&g_DeviceTree.m_EnumTimer, waitTime, &g_DeviceTree.m_EnumDpc);
}
__inline void USBD_ClearEnumWatchDog()
{
    KeCancelTimer(&g_DeviceTree.m_EnumTimer);
}
VOID   
USBD_DeviceEnumTimerProc(
    IN PKDPC Dpc,
    IN PVOID Unused1,
    IN PVOID Unused2,
    IN PVOID Unused3
    )
{
    switch(g_DeviceTree.m_TimerReason)
    {
        case USBD_TIMER_REASON_STAGE0:
            USBD_DeviceEnumStage0();
            break;
        case USBD_TIMER_REASON_WATCHDOG:
            USB_DBG_WARN_PRINT(("Enumeration Timed out"));
            HCD_CancelRequest(
                    USBD_GetHCDExtension(g_DeviceTree.m_CurrentEnum->m_HostController),
                    &g_DeviceTree.m_EnumUrb
                    );  //This should force the hung URB to complete.
            break;
        case USBD_TIMER_REASON_CONTINUE_STAGE1:
            USBD_fDeviceEnumStage1(g_DeviceTree.m_CurrentEnum);
            break;
        case USBD_TIMER_REASON_CONTINUE_STAGE4:
            USBD_fDeviceEnumStage4(&g_DeviceTree.m_EnumUrb,g_DeviceTree.m_CurrentEnum);
            break;
        
        default:
            // The reason should always be set before the timer.
            // So this is a bug in the code that set the timer if this case is hit.
            ASSERT(FALSE);
    }
}


void USBD_DeviceEnumStage0()
/*++
Routine Description:

    Zeroeth stage of device enumeration after IUsbDevice::DeviceConnected.  See IUsbDevice::DeviceConnected
    for explanation of stages.  In this routine we:
        - reset the port

Arguments:
    
    None    

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{    
    IUsbDevice  *device = g_DeviceTree.m_CurrentEnum;
    PVOID       hcdDeviceExtension = USBD_GetHCDExtension(device->m_HostController);
    
    USB_DBG_ENTRY_PRINT(("Entering USBD_DeviceEnumStage0"));
    
    //
    //  Verify that the device was not removed while we were waiting.
    //
    if(g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_ENTRY_PRINT(("Stage 0 detects device removal."));
        USBD_DeviceAbortEnum(NULL, device);
        return;
    }
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_0;
    //
    //  Reset Port - Either we need to ask HCD to do it for the RootHub,
    //               or we need to ask the USBHUB (there had better be one
    //               and only one linked to us.
    //
    IUsbDevice *parent = device->GetParent();
    if(UDN_TYPE_ROOT_HUB == parent->m_Type)
    {
        HCD_ResetRootHubPort(
                hcdDeviceExtension,
                device->GetHubPort(),
                (PFNHCD_RESET_COMPLETE)USBD_DeviceEnumStagePre1,
                (PVOID)device
                );
    }
    else
    {
        //
        //  Otherwise ask our hub driver component to handle the port reset.
        //
        USBHUB_DisableResetPort(parent, device->GetHubPort(), (PVOID)device, FALSE);
    }
    USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceEnumStage0"));
}

void IUsbDevice::ResetComplete(USBD_STATUS UsbdStatus, PVOID Context)
/*++
*   Wraps USBD_DeviceEnumStage1 for Hub Class Driver.
*   
--*/
{
    USBD_DeviceEnumStagePre1(UsbdStatus, Context);
}

VOID    
USBD_DeviceEnumStagePre1(
            USBD_STATUS UsbdStatus,
            PVOID Context
    )
/*++
    Routine Description:
        Section 9.2.6.2 stipulates a 10 ms recovery
        time after the 10 ms RESET.  This
        routine sets a 10 ms timer.
--*/
{
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_PRE1;

    
     //
    //  Check that the reset port worked, and that the
    //  device has not been ripped out between
    //  enumeration stages.
    //
    if(USBD_ERROR(UsbdStatus))
    {
        USB_DBG_WARN_PRINT(("Pre Stage 1 detects failue: UsbdStatus = 0x%0.8x", UsbdStatus));
        USBD_DeviceAbortEnum(NULL, (IUsbDevice  *)Context);
        return;
    }

    //
    //  Set the speed here.
    //
    IUsbDevice  *device = (IUsbDevice  *)Context;
    if(USBD_STATUS_LOWSPEED == UsbdStatus)
    {
        device->m_PortNumber |= UDN_LOWSPEED_PORT;
    } else
    {
        ASSERT(!(device->m_PortNumber&UDN_LOWSPEED_PORT));
    }

    //
    //  A ten millisecond wait is required after reset.
    //
    LARGE_INTEGER           waitTime;
    g_DeviceTree.m_TimerReason = USBD_TIMER_REASON_CONTINUE_STAGE1;
    waitTime.QuadPart = -10*10000; //10 ms wait
    KeSetTimer(&g_DeviceTree.m_EnumTimer, waitTime, &g_DeviceTree.m_EnumDpc);
}

void
FASTCALL
USBD_fDeviceEnumStage1(
    IUsbDevice  *Device
    )
/*++
Routine Description:

    First stage of device enumeration after IUsbDevice::DeviceConnected.
    See IUsbDevice::DeviceConnected for explanation of stages.
    In this routine we:
        - open the default endpoint
        - get the DeviceDescriptor

Arguments:
    
    UsbdStatus - Status of reset port.

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{   
    PVOID  hcdDeviceExtension = USBD_GetHCDExtension(Device->m_HostController);
    
    ASSERT_DISPATCH_LEVEL();

    USB_DBG_ENTRY_PRINT(("Entering USBD_DeviceEnumStage1"));
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_1;
    if(g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_WARN_PRINT(("Stage 1 detects device removal."));
        USBD_DeviceAbortEnum(NULL, Device);
        return;
    }

    //
    //  Fill out URB for opening an endpoint
    //
    USB_BUILD_OPEN_ENDPOINT(
                &g_DeviceTree.m_EnumUrb.OpenEndpoint,
                0,  //Address is 0 - default address
                USB_ENDPOINT_TYPE_CONTROL,
                USBD_DEFAULT_MAXPACKET0,
                0,  //Interval
                );
    g_DeviceTree.m_EnumUrb.OpenEndpoint.LowSpeed = Device->GetLowSpeed();
    g_DeviceTree.m_EnumUrb.OpenEndpoint.FunctionAddress = 0;

    //
    //  This URB is required to be synchronous and not allowed to fail
    //  according to specification.  Why?
    //      * Control endpoint, so no bandwidth problems
    //      * Open endpoint does not communicate over wire.
    //
    HCD_SubmitRequest(
        hcdDeviceExtension,
        &g_DeviceTree.m_EnumUrb
        );

    //
    //  Retrieve the handle from the URB
    //
    Device->m_DefaultEndpoint = g_DeviceTree.m_EnumUrb.OpenEndpoint.EndpointHandle;
    ASSERT(Device->m_DefaultEndpoint);  //Though it is not suppose to fail under any circumstance, check in debug!

    //
    //  Now issue an asynchronous request for
    //  the device descriptor.  This will also
    //  move us on to the next stage.
    
    //
    //  Don't use USB_BUILD_GET_DESCRIPTOR macro, because
    //  it was intended for class drivers, which don't know the
    //  EndpointHandle.
    //
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Length = sizeof(URB_CONTROL_TRANSFER);
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceEnumStage2;
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteContext = (PVOID)Device;
    g_DeviceTree.m_EnumUrb.ControlTransfer.EndpointHandle = Device->m_DefaultEndpoint;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBuffer = (PVOID)&g_DeviceTree.m_DeviceDescriptor;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBufferLength = sizeof(USB_DEVICE_DESCRIPTOR8);
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferDirection = USB_TRANSFER_DIRECTION_IN;
    g_DeviceTree.m_EnumUrb.ControlTransfer.ShortTransferOK = FALSE;
    g_DeviceTree.m_EnumUrb.ControlTransfer.InterruptDelay = USBD_DELAY_INTERRUPT_0_MS;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bmRequestType = USB_DEVICE_TO_HOST;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wValue = (USB_DEVICE_DESCRIPTOR_TYPE << 8);
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wIndex = 0;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wLength = sizeof(USB_DEVICE_DESCRIPTOR8);
    
    //
    //  We don't worry about errors.  The completion
    //  routine will get called even for errors, and
    //  we will just let the next stage deal with it.
    //
    USBD_SetEnumWatchDog();
    HCD_SubmitRequest(
        hcdDeviceExtension,
        &g_DeviceTree.m_EnumUrb
        );
    
    USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceEnumStage1"));
    return;
}

VOID    
USBD_DeviceEnumStage2(
            PURB        Urb,
            IUsbDevice  *device
    )
/*++
Routine Description:

    Second stage of device enumeration fter IUsbDevice::DeviceConnected.
    See IUsbDevice::DeviceConnected for explanation of stages.
    In this routine we:
        - allocate address
        - set address
Arguments:
    
    Urb     - Pointer to URB (it had better be &g_DeviceTree.m_EnumUrb)
    Device  - Device (passed as the context for complete procedure)

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{   
    ASSERT_DISPATCH_LEVEL();
    USBD_ClearEnumWatchDog();
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_2;
    //
    //  Check that the get device descriptor worked, and that the
    //  device has not been ripped out between
    //  enumeration stages.
    //
    if(USBD_ERROR(Urb->Header.Status) ||g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_WARN_PRINT(("Stage 2 detects failue: Removed = %s, UsbdStatus = 0x%0.8x", g_DeviceTree.m_DeviceRemoved ? "TRUE":"FALSE", Urb->Header.Status));
        //
        //  USBD_DeviceEnumStage3 close the default endpoint.
        //  If it sees the error or DeviceRemoved
        //  it will redirect the enumeration sequence to abort.
        //
        USBD_DeviceEnumStage3(Urb, device);
        return;
    }

    //
    //  Do some sanity checks on the descriptor, and reject
    //  the device if they fail.
    //
    if(
        (Urb->ControlTransfer.TransferBufferLength < 8) ||
        (g_DeviceTree.m_DeviceDescriptor.bMaxPacketSize0 > 64) ||
        (USB_DEVICE_DESCRIPTOR_TYPE != g_DeviceTree.m_DeviceDescriptor.bDescriptorType) ||
        ((sizeof(USB_DEVICE_DESCRIPTOR8) != g_DeviceTree.m_DeviceDescriptor.bLength) &&
         (sizeof(USB_DEVICE_DESCRIPTOR) != g_DeviceTree.m_DeviceDescriptor.bLength))
    )
    {
        USB_DBG_WARN_PRINT(("A device returned a corrupt device descriptor.  Retries are allowed."));
        Urb->Header.Status = USBD_STATUS_REQUEST_FAILED;
        USBD_DeviceEnumStage3(Urb, device);
        return;
    }
    
    //
    //  Dump the device descriptor in debug builds
    //
    USB_DBG_TRACE_PRINT(("Device Descriptor:"));

    USB_DBG_TRACE_PRINT(("   bcdUSB             = 0x%0.4x",  g_DeviceTree.m_DeviceDescriptor.bcdUSB));
    USB_DBG_TRACE_PRINT(("   bDeviceClass       = %d", (long)g_DeviceTree.m_DeviceDescriptor.bDeviceClass));
    USB_DBG_TRACE_PRINT(("   bDeviceSubClass    = %d", (long)g_DeviceTree.m_DeviceDescriptor.bDeviceSubClass));
    USB_DBG_TRACE_PRINT(("   bDeviceProtocol    = %d", (long)g_DeviceTree.m_DeviceDescriptor.bDeviceProtocol));
    USB_DBG_TRACE_PRINT(("   bMaxPacketSize0    = %d", (long)g_DeviceTree.m_DeviceDescriptor.bMaxPacketSize0));

    //
    //  Record MaxPacket0 from device descriptor
    //
    device->m_MaxPacket0 = g_DeviceTree.m_DeviceDescriptor.bMaxPacketSize0;

    //
    //  Allocate an address 
    //
    device->m_Address = USBD_AllocateUsbAddress(device->m_HostController);

    //
    //  This should never happen, because our tree
    //  supports fewer devices than we have addresses.
    //  So if we hit this ASSERT it probably means
    //  that someone didn't free an address on removal.
    //
    ASSERT(device->m_Address);


    //
    //  Set the address - No Macro for this one, because
    //                    class drivers should never do it.
    //
    //  Items commented are either correct from last transfer, or not necessary.
    //
  //g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Length = sizeof(URB_CONTROL_TRANSFER);
  //g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceEnumStage3;
  //g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteContext = (PVOID)deviceNode;
  //g_DeviceTree.m_EnumUrb.ControlTransfer.EndpointHandle = g_DeviceTree.m_EndpointHandle;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBuffer = NULL;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBufferLength = 0;
  //g_DeviceTree.m_EnumUrb.ControlTransfer.TransferDirection = USB_TRANSFER_DIRECTION_IN;
  //g_DeviceTree.m_EnumUrb.ControlTransfer.ShortTransferOK = FALSE;
  //g_DeviceTree.m_EnumUrb.ControlTransfer.InterruptDelay = USBD_DELAY_INTERRUPT_0_MS;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bmRequestType = USB_HOST_TO_DEVICE;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bRequest = USB_REQUEST_SET_ADDRESS;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wValue = device->m_Address;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wIndex = 0;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wLength = 0;
    
    //
    //  We don't worry about errors.  The completion
    //  routine will get called even for errors, and
    //  we will just let the next stage deal with it.
    //
    USBD_SetEnumWatchDog();
    HCD_SubmitRequest(USBD_GetHCDExtension(device->m_HostController), &g_DeviceTree.m_EnumUrb);

    USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceEnumStage2"));
    return;
}

VOID    
USBD_DeviceEnumStage3(
            PURB        Urb,
            IUsbDevice  *device
    )
/*++
Routine Description:

    Third stage of device enumeration after IUsbDevice::DeviceConnected.
    See IUsbDevice::DeviceConnected for explanation of stages.
    In this routine we:
        - close default endpoint

    ***
    *** BECAUSE THIS STAGE CLOSES THE DEFAULT ENDPOINT WHETHER THE PREVIOUS
    *** OPERATION FAILED OR NOT, THIS STAGE IS CALLED BY OTHER STAGES
    *** TO ABORT ENUMERATION IF THE DEFAULT ENDPOINT IS OPEN.
    ***

Arguments:
    
    Urb     - Pointer to URB (it had better be &g_DeviceTree.m_EnumUrb)
    Device  - Device (passed as the context for complete procedure)

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{   
    USB_DBG_ENTRY_PRINT(("Entering USBD_DeviceEnumStage3"));
    ASSERT_DISPATCH_LEVEL();
    USBD_ClearEnumWatchDog();
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_3;
    //
    //  Check if the address was set properly, and that the
    //  device has not been ripped out between
    //  enumeration stages.
    //
    if(USBD_ERROR(Urb->Header.Status) || g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_ENTRY_PRINT(("Stage 3 detects failue: Removed = %s, UsbdStatus = 0x%0.8x", g_DeviceTree.m_DeviceRemoved ? "TRUE":"FALSE", Urb->Header.Status));
        //
        // Redirect the close the default endpoint normally done in this
        // stage, to abort enumeration instead.
        //
        g_DeviceTree.m_EnumUrb.Header.CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceAbortEnum;
    }
    else
    {
        g_DeviceTree.m_EnumUrb.Header.CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceEnumStagePre4;
    }
    g_DeviceTree.m_EnumUrb.Header.Length = sizeof(URB_CLOSE_ENDPOINT);
    g_DeviceTree.m_EnumUrb.Header.Function = URB_FUNCTION_CLOSE_ENDPOINT;
    //g_DeviceTree.m_EnumUrb.Header.CompleteContext = (PVOID)device;
    //g_DeviceTree.m_EnumUrb.CloseEndpoint.EndpointHandle = g_DeviceTree.m_EndpointHandle;
    
    //
    //  We don't worry about errors.  The completion
    //  routine will get called even for errors, and
    //  we will just let the next stage deal with it.
    //  A close cannot time out.
    //
    HCD_SubmitRequest(
        USBD_GetHCDExtension(device->m_HostController),
        &g_DeviceTree.m_EnumUrb
        );

    //
    //  We can just mark the default endpoint as closed right now.
    //
    device->m_DefaultEndpoint = NULL;
    USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceEnumStage3"));    
    return;
}

VOID    
USBD_DeviceEnumStagePre4(
            PURB        Urb,
            IUsbDevice  *Device
    )
/*++
    Routine Description:
        A fine point of the specification is that a device may require
        2 ms after a SET_ADDRESS request before it has to repsond to
        the new address.  In stage 2 we set the address.  In stage 3,
        we closed the endpoint.  However, testing shows that this is
        not enough time.

        So this "pre4" stage is introduced to insert an explicit
        2 ms delay before stage 4.

        USBD_TIMER_REASON_CONTINUE_STAGE4 tells the timer routine
        that it should continue with stage 4 when this times out.
--*/
{
    //
    //  Set Address take two millseconds to settle, so we
    //  set a timer before proceding to stage 4
    //
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_PRE4;
    LARGE_INTEGER           waitTime;
    g_DeviceTree.m_TimerReason = USBD_TIMER_REASON_CONTINUE_STAGE4;
    waitTime.QuadPart = -2*10000; //2ms wait
    KeSetTimer(&g_DeviceTree.m_EnumTimer, waitTime, &g_DeviceTree.m_EnumDpc);
}

VOID  
FASTCALL  
USBD_fDeviceEnumStage4(
            PURB        Urb,
            IUsbDevice  *Device
    )
/*++
Routine Description:

    Fourth stage of device enumeration after IUsbDevice::DeviceConnected.
    See IUsbDevice::DeviceConnected for explanation of stages.
    In this routine we have two possibilities:

        Device requires a function driver:
            - Load the class driver.
            - We will complete enumeration when the function Driver
              calls IUsbDevice::AddComplete

        Device is a composite device:
            - Reopen default endpoint - with correct address and bMaxPacket0.
            - Get the Configuration descriptor and go on to stage 5

Arguments:
    
    Urb       - Pointer to URB (it had better be &g_DeviceTree.m_EnumUrb)
    Device    - Device (passed as the context for complete procedure)

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{   
    ASSERT_DISPATCH_LEVEL();

    USB_DBG_ENTRY_PRINT(("Entering USBD_DeviceEnumStage4"));
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_4;
    //
    //  Closing an endpoint cannot fail.  So we don't need to check, but
    //  we do need to check to make sure the device has not been removed.
    //
    if(g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_ENTRY_PRINT(("Stage 4 detects device removal"));
        //
        //  The default endpoint is closed so just abort
        //
        USBD_DeviceAbortEnum(NULL, Device);
        return;
    }

    //
    //  This is a function driver
    //
    if(0 != g_DeviceTree.m_DeviceDescriptor.bDeviceClass)
    {
        //
        //  Change the node type (remembering that hubs are special)
        //
        if(USB_DEVICE_CLASS_HUB == g_DeviceTree.m_DeviceDescriptor.bDeviceClass)
        {
            Device->m_Type = UDN_TYPE_HUB;
        }
        else
        {
            Device->m_Type = UDN_TYPE_FUNCTION;
        }

        //
        // Load the class driver
        //
        PNP_CLASS_ID classId;
        classId.USB.bClass = g_DeviceTree.m_DeviceDescriptor.bDeviceClass;
        classId.USB.bSubClass = g_DeviceTree.m_DeviceDescriptor.bDeviceSubClass;
        classId.USB.bProtocol = g_DeviceTree.m_DeviceDescriptor.bDeviceProtocol;
        classId.USB.bClassSpecificType = PNP_DEVICE_LEVEL_CLASS;
        USBD_LoadClassDriver(Device, classId);
        return;
    }

    //
    //  OK, we are either a COMPOSITE_FUNCTION or INTERFACE_FUNCTION
    //  so we need to get the configuration descriptor and move on to the
    //  next stage.
    //
    
    //*
    //* Open the default endpoint, this time get the address
    //* and stuff correct.
    //*

    //
    //  Initialize the enum urb to reopen the endpoint with the correct address
    //  and the correct max packet
    USB_BUILD_OPEN_ENDPOINT(
            &g_DeviceTree.m_EnumUrb.OpenEndpoint,
            0,  //Endpoint Address is 0 - default endpoint
            USB_ENDPOINT_TYPE_CONTROL,
            g_DeviceTree.m_DeviceDescriptor.bMaxPacketSize0,
            0  //Interval
            );
    g_DeviceTree.m_EnumUrb.OpenEndpoint.LowSpeed = Device->GetLowSpeed();
    g_DeviceTree.m_EnumUrb.OpenEndpoint.FunctionAddress = Device->m_Address;

    //
    //  This URB is required to be synchronous and not allowed to fail
    //  according to specification.  Why?
    //      * Control endpoint, so no bandwidth problems
    //      * Open endpoint does not communicate over wire.
    //
    HCD_SubmitRequest(
        USBD_GetHCDExtension(Device->m_HostController),
        &g_DeviceTree.m_EnumUrb
        );
    
    //
    //  We want the default endpoint to be available to the class driver
    //  in later stages.
    //
    Device->m_DefaultEndpoint = g_DeviceTree.m_EnumUrb.OpenEndpoint.EndpointHandle; 
    ASSERT(Device->m_DefaultEndpoint);  //Though it is not suppose to fail under any circumstance, check in debug!
    
    //
    //  Now get the configuration descriptor and go on to next stage
    //
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Length = sizeof(URB_CONTROL_TRANSFER);
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceEnumStage5;
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteContext = (PVOID)Device;
    g_DeviceTree.m_EnumUrb.ControlTransfer.EndpointHandle = Device->m_DefaultEndpoint;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBuffer = (PVOID)&g_DeviceTree.m_ConfigurationDescriptorBuffer;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBufferLength = USBD_MAX_CONFIG_DESC_SIZE;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferDirection = USB_TRANSFER_DIRECTION_IN;
    g_DeviceTree.m_EnumUrb.ControlTransfer.ShortTransferOK = TRUE;
    g_DeviceTree.m_EnumUrb.ControlTransfer.InterruptDelay = USBD_DELAY_INTERRUPT_0_MS;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bmRequestType = USB_DEVICE_TO_HOST;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8);
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wIndex = 0;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wLength = USBD_MAX_CONFIG_DESC_SIZE;
    
    //
    //  We don't worry about errors.  The completion
    //  routine will get called even for errors, and
    //  we will just let the next stage deal with it.
    //
    USBD_SetEnumWatchDog();
    HCD_SubmitRequest(
        USBD_GetHCDExtension(Device->m_HostController),
        &g_DeviceTree.m_EnumUrb
        );
    return;
}

VOID    
USBD_DeviceEnumStage5(
            PURB        Urb,
            IUsbDevice  *Device
    )
/*++
Routine Description:

    Fifth stage of device enumeration after IUsbDevice::DeviceConnected.
    See IUsbDevice::DeviceConnected for explanation of stages.
    In this routine we:
        - set the configuration.

Arguments:
    
    Urb       - Pointer to URB (it had better be &g_DeviceTree.m_EnumUrb)
    Device    - Device (passed as the context for complete procedure)   

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{   
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;
    ASSERT_DISPATCH_LEVEL();
    USBD_ClearEnumWatchDog();
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_5;
    //
    //  Check if we got the configuration descriptor, and that the
    //  device has not been ripped out between enumeration stages.
    //
    if(USBD_ERROR(Urb->Header.Status) || g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_ENTRY_PRINT(("Stage 5 detects failue: Removed = %s, UsbdStatus = 0x%0.8x", g_DeviceTree.m_DeviceRemoved ? "TRUE":"FALSE", Urb->Header.Status));
        //
        //  USBD_DeviceEnumStage3 close the default endpoint.
        //  When it sees the error or DeviceRemoved,
        //  it will redirect the enumeration sequence to abort.
        //
        USBD_DeviceEnumStage3(Urb, Device);
        return;
    }
    
    //
    //  Set the configurationDescriptor.
    //
    configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)g_DeviceTree.m_ConfigurationDescriptorBuffer;
    
    //
    //  Verify the the configurationDescriptor does not exceed the Xbox
    //  limit.
    //
    if(USBD_MAX_CONFIG_DESC_SIZE < configurationDescriptor->wTotalLength)
    {
        g_DeviceTree.m_RetryCount = 0; //Don't retry this
        USB_DBG_WARN_PRINT(("Unsupported Hardware: Configuration Descriptor Length exceeds Xbox limit(%d bytes), disabling device", USBD_MAX_CONFIG_DESC_SIZE));
        Urb->Header.Status = USBD_STATUS_UNSUPPORTED_DEVICE;
        USBD_DeviceEnumStage3(Urb, Device);
        return;
    }

    //
    //  Verify that the entire descriptor was sent.
    //
    if(configurationDescriptor->wTotalLength != Urb->ControlTransfer.TransferBufferLength)
    {
        USB_DBG_WARN_PRINT(("Device did not return entire configuration descriptor, reenumerating"));
        Urb->Header.Status = USBD_STATUS_ERROR;
        //Retries will be allowed.
        USBD_DeviceEnumStage3(Urb, Device);
        return;
    }
    
    //
    //  Now set the configuration and go on to next stage
    //
    //g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Length = sizeof(URB_CONTROL_TRANSFER);
    //g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
    g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceEnumStage6;
    //g_DeviceTree.m_EnumUrb.ControlTransfer.Hdr.CompleteContext = (PVOID)deviceNode;
    //g_DeviceTree.m_EnumUrb.ControlTransfer.EndpointHandle = GLOBAL_USBD_EnumBlock.EndpointHandle;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBuffer = NULL;
    g_DeviceTree.m_EnumUrb.ControlTransfer.TransferBufferLength = 0;
    //g_DeviceTree.m_EnumUrb.ControlTransfer.TransferDirection = USB_TRANSFER_DIRECTION_IN;
    //g_DeviceTree.m_EnumUrb.ControlTransfer.ShortTransferOK = TRUE;
    //g_DeviceTree.m_EnumUrb.ControlTransfer.InterruptDelay = USBD_DELAY_INTERRUPT_0_MS;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bmRequestType = USB_HOST_TO_DEVICE;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.bRequest = USB_REQUEST_SET_CONFIGURATION;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wValue = configurationDescriptor->bConfigurationValue;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wIndex = 0;
    g_DeviceTree.m_EnumUrb.ControlTransfer.SetupPacket.wLength = 0;
    //
    //  We don't worry about errors.  The completion
    //  routine will get called even for errors, and
    //  we will just let the next stage deal with it.
    //
    USBD_SetEnumWatchDog();
    HCD_SubmitRequest(
        USBD_GetHCDExtension(Device->m_HostController),
        &g_DeviceTree.m_EnumUrb
        );
    return;
}

VOID    
USBD_DeviceEnumStage6(
            PURB        Urb,
            IUsbDevice  *Device
    )
/*++
Routine Description:

    Sixth stage of device enumeration after IUsbDevice::DeviceConnected.
    See IUsbDevice::DeviceConnected for explanation of stages.
    In this routine we have either a INTERACE_FUNCTION or COMPOSITE FUNCTION.
    
    If it is a COMPOSITE_FUNCTION we 
        - Allocate additional nodes for each of the interfaces and plug them into the tree.
        - Begin notifying the interface drivers

    If it is an INTERFACE_FUNCTION we
        - Notify the interface driver

Arguments:
    
    Urb         - Pointer to URB (it had better be &g_DeviceTree.m_EnumUrb)
    DeviceNode  - DeviceNode (passed as the context for complete procedure)

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{   
    ASSERT_DISPATCH_LEVEL();
    USBD_ClearEnumWatchDog();
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_6;
    //
    //  Check if we got the configuration descriptor, and that the
    //  device has not been ripped out between enumeration stages.
    //
    if(USBD_ERROR(Urb->Header.Status) || g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_ENTRY_PRINT(("Stage 6 detects failue: Removed = %s, UsbdStatus = 0x%0.8x", g_DeviceTree.m_DeviceRemoved ? "TRUE":"FALSE", Urb->Header.Status));
        //
        //  USBD_DeviceEnumStage3 close the default endpoint.
        //  When it sees the error or DeviceRemoved,
        //  it will redirect the enumeration sequence to abort.
        //
        USBD_DeviceEnumStage3(Urb, Device);
        return;
    }
    //
    //  After a successful set configuration clear the data toggle bits.
    //
    Device->m_DataToggleBits = 0;

    //
    //  Begin parsing the configuration descriptor
    //
    PUCHAR currentParseLocation = g_DeviceTree.m_ConfigurationDescriptorBuffer;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)g_DeviceTree.m_ConfigurationDescriptorBuffer;

    //
    //  Find the first interface, this will be needed for both
    //  UDN_TYPE_INTERFACE_FUNCTION and UDN_TYPE_COMPOSITE_FUNCTION
    //
    do
    {
        currentParseLocation += ((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bLength;
        //
        //  Check to make sure we haven't walked off the end of the conifugration descriptor,
        //  and that we are not in an endless loop.
        //
        if(
            (currentParseLocation >= (g_DeviceTree.m_ConfigurationDescriptorBuffer + USBD_MAX_CONFIG_DESC_SIZE)) ||
            (0 == ((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bLength)
        ){
            USB_DBG_WARN_PRINT(("Corrupt Configuration Descriptor."));
            g_DeviceTree.m_RetryCount = 0; //Don't retry this
            Urb->Header.Status = USBD_STATUS_UNSUPPORTED_DEVICE;
            USBD_DeviceEnumStage3(Urb, Device);
            return;
        }
    }while(((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE);
    g_DeviceTree.m_InterfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)currentParseLocation;

    //
    //  If it has only one interface than it is an UDN_TYPE_INTERFACE_FUNCTION
    //  
    if(1 == configurationDescriptor->bNumInterfaces || 0 == g_DeviceTree.m_MaxCompositeInterfaces)
    {
        //
        //  Fill out UDN_TYPE_INTERFACE_FUNCTION specific stuff
        //
        Device->m_Type = UDN_TYPE_INTERFACE_FUNCTION;
    }
    else
    {
        IUsbDevice *interfaceDevice;
        ULONG      interfaceIndex;
        
        //
        //  Change the main device node to UDN_TYPE_COMPOSITE_FUNCTION
        //
        Device->m_Type = UDN_TYPE_COMPOSITE_FUNCTION;
        Device->m_FirstChild = UDN_INVALID_NODE_INDEX;

        //
        //  Allocates device for all of the interfaces
        //
        //
        for(interfaceIndex = 0; interfaceIndex < configurationDescriptor->bNumInterfaces; interfaceIndex++)
        {
            //
            //  Refuse to enumerate more interfaces on a composite device
            //  then any of the drivers registered for
            if(g_DeviceTree.m_MaxCompositeInterfaces <= interfaceIndex)
            {
                break;
            }

            //
            //  Allocate a device on our tree
            //
            interfaceDevice = g_DeviceTree.AllocDevice();
            if(NULL == interfaceDevice)
            {
                USB_DBG_WARN_PRINT(("Number of Devices Attached, exceeds the limits of our tree!"));
                break;
            }
            //
            //  initialize basic information from the composite device
            //
            interfaceDevice->m_Type = UDN_TYPE_INTERFACE;
            interfaceDevice->m_PortNumber = (interfaceIndex+1) | (Device->m_PortNumber & UDN_LOWSPEED_PORT);
            interfaceDevice->m_Address = Device->m_Address;
            interfaceDevice->m_DefaultEndpoint = Device->m_DefaultEndpoint;
            interfaceDevice->m_HostController = Device->m_HostController;
            interfaceDevice->m_MaxPacket0 = Device->m_MaxPacket0;

            //
            //  Plug node into the tree as a child of the composite device
            //
            Device->InsertChild(interfaceDevice);
        }
        //
        //  The parents defaultendpoint has to appear closed, so
        //  right after a child interface is complete, it can
        //  immediately open the default endpoint.
        //
        Device->m_DefaultEndpoint = NULL;
        //
        //  We are done with the UDN_TYPE_COMPOSITE_FUNCTION
        //  move on to processing its first child.
        //  
        Device = Device->GetFirstChild();
    }
    //
    //  OK set the interface.
    //
    Device->m_bInterfaceNumber = g_DeviceTree.m_InterfaceDescriptor->bInterfaceNumber;
     
    //
    // Load the class driver
    //
    PNP_CLASS_ID classId;
    classId.USB.bClass = g_DeviceTree.m_InterfaceDescriptor->bInterfaceClass;
    classId.USB.bSubClass = g_DeviceTree.m_InterfaceDescriptor->bInterfaceSubClass;
    classId.USB.bProtocol = g_DeviceTree.m_InterfaceDescriptor->bInterfaceProtocol;
    classId.USB.bClassSpecificType = PNP_INTERFACE_LEVEL_CLASS;
    USBD_LoadClassDriver(Device, classId);
    return;
}

VOID
IUsbDevice::AddComplete(
    USBD_STATUS Status
    )
/*++
Routine Description:

    Last stage of enumeration after IUsbDevice::DeviceConnected. See IUsbDevice::DeviceConnected
    for explanation of stages.
    
    This routine is called a by a class driver to when it has completed the class specific enumeration
    steps.  This routine is necessary, because the class driver may have to make asynchronous requests
    in order to complete its initialization, and therefore USBD needs to be notified when the
    class driver is done.

    The different types of devices require some special handindling.

    1) For UDN_TYPE_INTERFACE devices if there is a sibling: it load the class driver for the sibling and return.  Control
    will come back to this routine when the class driver for the sibling has completed its enumeration steps.

    2) For the last UDN_TYPE_INTERFACE of a UDN_TYPE_COMPOSITE_FUNCTION, or for UDN_TYPE_INTERFACE_FUNCTION it closes
    the default endpoint.  This function cascades to USBD_DeviceEnumNextPending.

    3) For UDN_TYPE_FUNCTION it just calls USBD_DeviceEnumNextPending directly.

Arguments:
    
    Status - Status from the class driver.  Should be one of:
    
      USBD_STATUS_SUCCESS               - Device successfully initialized.
      USBD_STATUS_UNSUPPORTED_DEVICE    - Device not supported by the class driver.

	NotifylParam - lParam to send with Add and  Notifications.

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{
    ASSERT_DISPATCH_LEVEL();
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_ADD_COMPLETE;
    //
    //  If the class driver rejected the device, than
    //  the there is no class driver.
    //
    if(USBD_STATUS_UNSUPPORTED_DEVICE == Status)
    {
        USB_DBG_WARN_PRINT(("Unsupported Device or Class Driver not present."));
        m_ClassDriver = NULL;
    } else
    {
        //
        //  Notify XAPI
        //
        if(0xFF != m_ClassSpecificType)
        {
            PXPP_DEVICE_TYPE deviceType = m_ClassDriver->DeviceTypes[m_ClassSpecificType];
            if(deviceType)
            {
                XdReportDeviceInsertionRemoval(deviceType, m_ExternalPort, TRUE);
            }
        }
    }
    //
    //  Check for UDN_TYPE_FUNCTION and call USBD_DeviceEnumNextPending
    //
    if(
        (m_Type == UDN_TYPE_FUNCTION) ||
        (m_Type == UDN_TYPE_HUB)
    )
    {
        //
        //  If the class driver failed it, then abort enumeration.
        //
        if(USBD_ERROR(Status))
        {
            if(!m_ClassDriver) g_DeviceTree.m_RetryCount = 0;
            USBD_DeviceAbortEnum(NULL, this);
            return;
        }
        //
        //  Functions are completely enumerated, nothing left to do,
        //  except move on to the next pending.
        //
        USBD_DeviceEnumNextPending(NULL, this);
        return;
    }

    //
    //  The composite device is the parent of theis device if of type UDN_TYPE_INTERFACE_FUNCTION,
    //  and this device's parent if UDN_TYPE_INTERFACE.  Assume it is this
    //  device for now, and the case below will change it if necessary.
    //
    IUsbDevice *compositeDevice = this;
    //
    //  Mark the default endpoint as closed, but cache the endpointhandle
    //  so we can really close it.  We do this here because it applies to
    //  the last sibling if UDN_TYPE_INTERFACE, and to UDN_TYPE_INTERFACE_FUNCTION.
    //  Normally, the UDN_TYPE_COMPOSITE node is non-NULL if any of its children
    //  are non-NULL, but during enumeration is an exception.  This is so that
    //  immediately after a ClassDriver calls AddComplete it can open the default
    //  endpoint, and there is no race condition w.r.t. its sibilings completing enumeration.
    //  So before loaded even the first interface of the composite, we set the UDN_TYPE_COMPOSITE's
    //  m_DefaultEndpoint to NULL, while setting m_DefaultEndpoint for all the children to 
    //  the same endpoint which was opened for enumeration purposes.  Now, if the first
    //  child calls AddComplete and is immediately opened before its siblings finish enumeration
    //  it can reopen the default endpoint (getting a different handle).  This code cannot step on that.
    //  So if AddComplete is called for UDN_TYPE_INTERFACE and it is the last sibling we close the
    //  handle in UDN_TYPE_INTERFACE, and totally ignore what is in its parent (which may be NULL,
    //  or may be a different endpoint handle).  Note below, that we don't get as far as closing the
    //  endpoint if this is not the last sibling. If it is called for UDN_TYPE_INTERFACE_FUNCTION
    //  then there is nothing subtle going on.
    //
    PVOID endpointHandle = m_DefaultEndpoint;
    m_DefaultEndpoint = NULL;

    //
    //  There is some difference of type UDN_TYPE_INTERFACE.
    //  a) There may be siblings that have not have drivers loaded yet.
    //  b) The compositeDevice (the one that really owns the default endpoint)
    //     is our parent, not us.
    //
    PURB_COMPLETE_PROC CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceEnumNextPending;
    if(m_Type == UDN_TYPE_INTERFACE)
    {
        IUsbDevice *Sibling = GetSibling();
        //
        //  As long as this has siblings there must be more interfaces.
        //
        if(NULL != Sibling)
        {
            //
            //  Begin parsing the configuration descriptor
            //
            PUCHAR currentParseLocation = (PUCHAR)g_DeviceTree.m_InterfaceDescriptor;
            
            //
            //  Parse to the next interface
            //
            do
            {
                currentParseLocation += ((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bLength;
                ASSERT(currentParseLocation < (g_DeviceTree.m_ConfigurationDescriptorBuffer + USBD_MAX_CONFIG_DESC_SIZE));
            }while(((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE);
            g_DeviceTree.m_InterfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)currentParseLocation;
            //
            //  Now that we have the next interface set the siblings m_bInterfaceNumber.
            //
            Sibling->m_bInterfaceNumber = g_DeviceTree.m_InterfaceDescriptor->bInterfaceNumber;
            //
            //  Load a driver for the sibling,
            //  and return.
            //
            PNP_CLASS_ID classId;
            classId.USB.bClass = g_DeviceTree.m_InterfaceDescriptor->bInterfaceClass;
            classId.USB.bSubClass = g_DeviceTree.m_InterfaceDescriptor->bInterfaceSubClass;
            classId.USB.bProtocol = g_DeviceTree.m_InterfaceDescriptor->bInterfaceProtocol;
            classId.USB.bClassSpecificType = PNP_INTERFACE_LEVEL_CLASS;
            USBD_LoadClassDriver(Sibling, classId);
            return;
        }
        else
        {
            //
            //  The next have this routine needs to know the compositeDevice
            //  which is this device's parent.
            compositeDevice = GetParent();
        }
    } else
    {
        if(USBD_ERROR(Status))
        {
            if(!m_ClassDriver) g_DeviceTree.m_RetryCount = 0;
            CompleteProc = (PURB_COMPLETE_PROC)USBD_DeviceAbortEnum;
        }
    }

    //
    //  Close the temporary enumeration time default endpoint.
    //
    g_DeviceTree.m_EnumUrb.Header.Length = sizeof(URB_CLOSE_ENDPOINT);
    g_DeviceTree.m_EnumUrb.Header.Function = URB_FUNCTION_CLOSE_ENDPOINT;
    g_DeviceTree.m_EnumUrb.Header.CompleteProc = CompleteProc;
    g_DeviceTree.m_EnumUrb.Header.CompleteContext = (PVOID)compositeDevice;
    g_DeviceTree.m_EnumUrb.CloseEndpoint.EndpointHandle = endpointHandle;
    
    //
    //  Close endpoint is not allowed to fail.
    //
    HCD_SubmitRequest(
        USBD_GetHCDExtension(compositeDevice->m_HostController),
        &g_DeviceTree.m_EnumUrb
        );
}

VOID    
USBD_DeviceAbortEnum(
            PURB        Urb,
            IUsbDevice  *Device
    )
/*++
Routine Description:
    If enumeration failed we need to stop enumerating and continue with the
    next pending device.  By the time we get to this routine, you
    can be sure that the device is closed.  We call 
--*/
{
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_ABORT1;
    if(!g_DeviceTree.m_DeviceRemoved)
    //
    //  If the device was not removed, but failed, we need to
    //  disable it before continuing.
    //
    {
        USB_DBG_WARN_PRINT(("Enumeration Failed"));
        //
        //  Disable the port before completing abort, otherwise
        //  we could have an address conflict.
        //
        IUsbDevice *parent = Device->GetParent();
        if(UDN_TYPE_ROOT_HUB == parent->m_Type)
        {
            HCD_DisableRootHubPort(USBD_GetHCDExtension(Device->m_HostController), Device->GetHubPort());
            Device->DisableComplete(USBD_STATUS_SUCCESS, (PVOID)Device);
        } else
        {
            USBHUB_DisableResetPort(parent, Device->GetHubPort(), (PVOID)Device, TRUE);
        }
        return;
    } else
    {
        //
        //  Don't retry if the device was removed.
        //
        g_DeviceTree.m_RetryCount = 0;
    }
    USBD_fDeviceAbortEnum2(Device);
}

void IUsbDevice::DisableComplete(USBD_STATUS UsbdStatus, PVOID Context)
/*++
*   Only gets called as part of abort.
*   
--*/
{
    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_DISABLE_COMPLETE;
    IUsbDevice *device = (IUsbDevice *)Context;
    //
    //  If the disable failed and the device was not removed
    //  then we cannot free the address.  We fake out
    //  USBD_fDeviceAbortEnum2 by forgetting the address, this
    //  is an address leak;
    //
    if(USBD_ERROR(UsbdStatus) && !g_DeviceTree.m_DeviceRemoved)
    {
        //This is OK as long as it is extremely rare and difficult to induce.
        USB_DBG_WARN_PRINT(("Address %d is about to leak, a device failed enumeration, and then the port disable failed.", device->m_Address));
        device->m_Address = 0; //leak the address.
    }
    //
    //  Don't retry devices that have been removed.
    //
    if(g_DeviceTree.m_DeviceRemoved)
    {
        g_DeviceTree.m_RetryCount = 0;
    }
    USBD_fDeviceAbortEnum2(device);
}

VOID FASTCALL USBD_fDeviceAbortEnum2(IUsbDevice  *Device)
/*++
*   Finish up abort
*   
--*/
{
    UCHAR retryCount = 0;
    UCHAR retryPort = 0;
    IUsbDevice *parent = NULL;

    g_DeviceTree.m_EnumStage = USBD_ENUM_STAGE_ABORT2;
    //
    //  If the device is not removed
    //
    if(!g_DeviceTree.m_DeviceRemoved)
    {
        parent = Device->GetParent();
        ASSERT(parent);
        //
        //  Now remove it from its parent's tree
        //
        parent->RemoveChild(Device);
        //
        //  Resubmit the device for enumeration if
        //  the retry count is non-zero.
        //
        if(g_DeviceTree.m_RetryCount)
        {
            retryCount = g_DeviceTree.m_RetryCount;
            retryPort = Device->GetHubPort();
        }
    }

    //
    //  If the address is non-zero free it.
    //
    if(Device->m_Address)
    {
        USBD_FreeUsbAddress(Device->m_HostController, Device->m_Address);
    }
    //
    //  Free the node
    //
    g_DeviceTree.FreeDevice(Device);

    //
    //  If we detected a need for a retry, fire it here
    //
    if(retryCount)
    {
        parent->DeviceConnected(retryPort, --retryCount);
    }

    //
    //  Continue enumeration.
    //
    g_DeviceTree.m_DeviceRemoved = FALSE;
    USBD_DeviceEnumNextPending(NULL, Device);
}

VOID    
USBD_DeviceEnumNextPending(
            PURB        Urb,
            IUsbDevice  *Device
    )
/*++
Routine Description:

    After enumeration of a device is complete, this function is called to enumerate the next
    device pendning enumeration.  If there are none it clears the InProgress flag, so that
    when a device is detected it can be enumerated.
    
    This routine is cascaded to from AddComplete, so g_DeviceTree.m_CurrentEnum
    will equal Device.  This would prevent IUsbDevice::DeviceDisconnected from completing.
    So we should also make one last check to see if the device was removed.

Arguments:
    
      Urb       - Could be NULL.  So don't use it.
      Device    - Device which was just enumerated.

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{
    ASSERT_DISPATCH_LEVEL();

    //
    //  One last check to make sure the device we were working on is not gone.
    //
    if(g_DeviceTree.m_DeviceRemoved)
    {
        USB_DBG_ENTRY_PRINT(("USBD_DeviceEnumNextPending detects device removal"));
        //
        //  We are essentially in the running stage now, so just finish off the device disconnected
        //  operation.
        USBD_CompleteDeviceDisconnected(Device);
    }
    g_DeviceTree.m_DeviceRemoved = FALSE;
    
    //
    //  If nothing else is pending, clear the InProgress flag and
    //  we are done.
    //
    if(NULL == g_DeviceTree.m_FirstPendingEnum)
    {
        g_DeviceTree.m_CurrentEnum = NULL;
        g_DeviceTree.m_InProgress = FALSE;
        return;
    }
    
    //---------------------------------------------
    // One or more devices are pending enumeration
    //---------------------------------------------
    //
    //  Pop a device off the pending list, and into current enum.
    //
    g_DeviceTree.m_CurrentEnum = g_DeviceTree.m_FirstPendingEnum;
    g_DeviceTree.m_FirstPendingEnum = g_DeviceTree.m_CurrentEnum->m_NextPending;
    g_DeviceTree.m_RetryCount =  g_DeviceTree.m_CurrentEnum->m_RetryCount;
    g_DeviceTree.m_EnumStage = USBD_ENUM_DEVICE_CONNECTED;
    g_DeviceTree.m_CurrentEnum->m_Type = UDN_TYPE_ENUMERATING;
    g_DeviceTree.m_CurrentEnum->m_ClassDriver = NULL;
    g_DeviceTree.m_CurrentEnum->m_Address = 0;
    
    
    //
    //  If it is too early to enumerate than wait until
    //  it is time, otherwise start stage 0 now.
    LARGE_INTEGER currentTime;
    currentTime.QuadPart = KeQueryInterruptTime();
    if(currentTime.QuadPart > g_DeviceTree.m_CurrentEnum->m_EarliestEnumTime.QuadPart)
    {
        USBD_DeviceEnumStage0();
    }
    else
    {

        g_DeviceTree.m_TimerReason = USBD_TIMER_REASON_STAGE0;
        KeSetTimer(&g_DeviceTree.m_EnumTimer, g_DeviceTree.m_CurrentEnum->m_EarliestEnumTime, &g_DeviceTree.m_EnumDpc);
        USB_DBG_TRACE_PRINT(("Current Tick = %d, Earliest Enum = %d", currentTime.LowPart, g_DeviceTree.m_CurrentEnum->m_EarliestEnumTime.LowPart));
    }
}


VOID
USBD_DeviceDisconnected(
    IN  PVOID HcdExtension,
    IN  UCHAR PortNumber
    )
/*++
    Routine Description:
        Called by the root-hub code in the HCD.  We find
        the device for the root-hub by looking in the DeviceObject,
        and then call the IUsbDevice::DeviceDisconntected.
--*/
{
    USBD_HostControllerFromHCDExtension(HcdExtension)->RootHub->DeviceDisconnected(PortNumber);
}

VOID IUsbDevice::DeviceDisconnected(UCHAR PortNumber)
/*++
Routine Description:

    Called by the hub driver or through USBD_DeviceDisconnected by the root-hub code in the HCD
    to report the disconnection of a USB device.  This routine must be called at DISPATCH_LEVEL.
    
    A device can be in one of three states when it is removed:
    
      1) Pending Enumeration - it is in the list of devices waiting to be enumerated.
      2) Enumerating - it is currently being enumerated.
      3) Functioning - it has completed enumeration and is being used.

    The handling depends on the state.
    
      1) Pending Enumeration:
            * Remove the device from its parents list of children.
            * Remove the device from the list of devices waiting enumeration
            * Put the device back on the free list.
            It is all done synchronously.
      2) Enumerating:
            *Remove the device from its parents list of children.
            *Mark the g_DeviceTree.m_DeviceRemoved.  The next time an enumeration stage is started it will:
                If the class driver has not been loaded:
                    * Close any open endpoints if necessary (shunting to stage 3).
                    * Call USBD_AbortEnumDevice which calls IUsbDevice::RemoveComplete.
                If the class driver(s) are loaded, it calls USBD_CompleteDeviceDisconnected
      3) Functioning:
            This routine:
                * Remove it from its parents list of children.
            USBD_CompleteDeviceDisconnected:
                * If it is a UDN_TYPE_HUB, UDN_TYPE_FUNCTION, UDN_TYPE_INTERFACE_FUNCTION, notify the class driver
                that it is gone.
                * For UDN_TYPE_COMPOSITE_FUNCTION we need to walk all of its children and notify each class driver that it
                is gone.
            Class Driver:
                * Close all endpoints.
                * The hub driver must call USBD_DeviceDisconnected (this routine) for each of its children.
                * Call IUsbDevice::RemoveComplete for the device.
                * It should notify its clients that the device is gone, and perform any additional cleanup it
                  needs to.
            IUsbDevice::RemoveComplete:
                * Finishes reclaiming the node.

Arguments:
    
    PortNumber      - Port number on hub (temporarily high-bit can be set to indicate low speed)

Return Value:

    None.

    This code returns from multiple places!

Context:
    Must be called at DISPATCH_LEVEL

--*/
{

    USB_DBG_ENTRY_PRINT(("Entering USBD_DeviceDisconnected"));
    USB_DBG_TRACE_PRINT(("Device removed from Port %d of Hub 0x%0.8x", (ULONG)GetHubPort(), this));

    ASSERT_DISPATCH_LEVEL();
    ASSERT((UDN_TYPE_HUB == m_Type) || (UDN_TYPE_ROOT_HUB == m_Type));

    //
    //  Find the node, among its parents children.
    //
    IUsbDevice *removedDevice = FindChild(PortNumber);
    if(NULL == removedDevice)
    {
        USB_DBG_TRACE_PRINT(("USBD_DeviceDisconnected called for a device which does not exist."));
        USB_DBG_TRACE_PRINT(("This will happen when an unsupported (or in some cases failed) device is removed."));
        USB_DBG_TRACE_PRINT(("It also may happen as a result of some device failures."));
        return;
    }
    //
    //  Remove the device from this hub's list of children.
    //
    RemoveChild(removedDevice);
    
    //  
    //  Handle disconnect if node is pending enumeration.
    //
    if(UDN_TYPE_PENDING_ENUM == removedDevice->m_Type)
    {
        //
        //  Remove the node from the pending list
        //  
        if(g_DeviceTree.m_FirstPendingEnum == removedDevice)
        {
            g_DeviceTree.m_FirstPendingEnum = removedDevice->m_NextPending;
        } else
        //
        //  Not the first one, so we need to walk the pending list until we find it.
        //
        {
            IUsbDevice *prevPending = g_DeviceTree.m_FirstPendingEnum;
            while(removedDevice != prevPending->m_NextPending)
            {
                prevPending = prevPending->m_NextPending;
                ASSERT(prevPending);
            }
            prevPending->m_NextPending = removedDevice->m_NextPending;
        }
        removedDevice->m_NextPending = NULL;
        //
        //  Return the node to the free list.
        //
        g_DeviceTree.FreeDevice(removedDevice);
        //
        //  The device is removed we are done
        //
        return;
    }
    
    //  
    //  Handle disconnect if node is enumerating.
    //
    if( 
        g_DeviceTree.m_InProgress &&
        (g_DeviceTree.m_CurrentEnum == removedDevice)
    )
    {
        //
        //  Just tell the enumeration code that the device
        //  it is working on is now gone.
        //
        g_DeviceTree.m_DeviceRemoved = TRUE;
        return;
    }
    
    //  
    //  Handle disconnect if node is functioning
    //
    USBD_CompleteDeviceDisconnected(removedDevice);

    USB_DBG_EXIT_PRINT(("Exiting USBD_DeviceDisonnected"));
}

VOID
USBD_CompleteDeviceDisconnected(
    IUsbDevice *Device
    )
/*++
Routine Description:
    Utility function to complete the last step of hte USBD_DeviceDisconnected routine:
    notifying the class drivers that the device has been removed.  This has been abstracted,
    because it is called in two places:
        1) End of IUsbDevice::DeviceDisconnected.
        2) From USBD_DeviceEnumNextPending.

    The latter cases arises if the device is physically removed and DeviceDeviceDisconnected is called for
    a device which has completed USBD_DeviceEnumStage6(for UDN_TYPE_COMPOSITE_FUNCTION, or UDN_TYPE_INTERFACE_FUNCTION) or
    USBD_DeviceEnumStage4(for UDN_TYPE_FUNCITON and UDN_TYPE_HUB), but the AddComplete has not yet been called
    by the class driver (or all the class drivers - in the case of UDN_TYPE_COMPOSITE_FUNCTION).
--*/
{
    if(UDN_TYPE_COMPOSITE_FUNCTION == Device->m_Type)
    {
        //
        //  Walk children (i.e. interface and notify each driver.  There is no need for serialization
        //  here so we just call them all.
        //
        IUsbDevice *child = Device->GetFirstChild();
        while(child)
        {
            //Cache the next child, because RemoveComplete,
            //removes it from the parent's list of children.
            IUsbDevice *nextChild = child->GetSibling(); 
            if(child->m_ClassDriver)
            {
                child->m_ClassDriver->RemoveDevice(child);
            } else
            {
                child->RemoveComplete();
            }
            child = nextChild;  //prepare for next interation
        }
    }
    else
    {
        if(Device->m_ClassDriver)
        {
            Device->m_ClassDriver->RemoveDevice(Device);
        } else
        {
            Device->RemoveComplete();
        }
    }
}


VOID IUsbDevice::RemoveComplete()
/*++
Routine Description:

    When a device is disconnected, USBD_DeviceDisconnected calls each of the class drivers to give
    them a chance to cleanup.  Since they may need to perform asynchronous operations to complete their
    cleanup (such as closing endpoints), completion of removal cannot proceed when the class driver returns
    from the notification.  Therefore class drivers must call this routine when they are done cleaning up.

    Likewise, if removal occurs during enumeration, enumeration eventually ends up here.  At this point, there are
    two tasks left: return the node to the free list, and free the USB address.
      
    However, UDN_TYPE_INTERFACE nodes have a little more work.  They are children of UDN_TYPE_COMPOSITE_FUNCTION.
    Since the parent UDN_TYPE_COMPOSITE_FUNCTION is needed for closing endpoints, it is still attached.  So this routine
    must remove the node from its parents' child list, and return the node to the free list.  Upon removing the
    last child, it should free the parent too.  The USB address should be freed with parent, not the children.
    
Arguments:
    
    DeviceObject    - DeviceObject of host controller for detecting hub
    DetectingHub    - Device Node for detecting hub
    PortNumber      - Port number on hub (temporarily high-bit can be set to indicate low speed)

Return Value:

    None.

Context:
    Must be called at DISPATCH_LEVEL

--*/
{
    ASSERT_DISPATCH_LEVEL();
	
   
    //
    //  Notify XAPI
    //
    if(0xFF != m_ClassSpecificType)
    {
        PXPP_DEVICE_TYPE deviceType = m_ClassDriver->DeviceTypes[m_ClassSpecificType];
        if(deviceType)
        {
            XdReportDeviceInsertionRemoval(deviceType, m_ExternalPort, FALSE);
        }
    }

    //
    //  For UDN_TYPE_INTERFACE, remove the node from its parents' child list,
    //  and free parent if no more children
    //
    if(UDN_TYPE_INTERFACE == m_Type)
    {
        //
        //  Remove the parents list of children
        //  and free parent if no more children
        IUsbDevice *parent = GetParent();
        if(FALSE == parent->RemoveChild(this))
        {
            //
            //  Free the USB address
            //
            USBD_FreeUsbAddress(parent->m_HostController, parent->m_Address);
            //
            //  Free the device.
            //
            g_DeviceTree.FreeDevice(parent);
        }
    }
    else
    {
        //
        //  For non-UDN_TYPE_INTERFACE nodes
        //
        if(m_Address)  //we have to check in case, we were aborted before a successful SET_ADDRESS
        {
            USBD_FreeUsbAddress(m_HostController, m_Address);
        }
    }
    //
    //  Free node.
    //
    g_DeviceTree.FreeDevice(this);
    return;
}

void IUsbDevice::DeviceNotResponding()
/*++
Routine Description:

    If a device stops responding for any reason, a class driver    
Arguments:
    
    DeviceObject    - DeviceObject of host controller for detecting hub
    DetectingHub    - Device Node for detecting hub
    PortNumber      - Port number on hub (temporarily high-bit can be set to indicate low speed)

Return Value:

    None.

Context:
    Must be called at DISPATCH_LEVEL

--*/
{
    ASSERT_DISPATCH_LEVEL();
    IUsbDevice *device;
    
    //
    //  If the device is UDN_TYPE_INTERFACE, then the
    //  real node is the parent
    //
    if(UDN_TYPE_INTERFACE == m_Type)
    {
        device = GetParent();
        ASSERT(device);
    } else
    {
        device = this;
    }
    

    //
    //  If it has a parent, it is not pending
    //  remove, so we should reenumerate it.
    //
    IUsbDevice *parent = device->GetParent();
    if(parent)
    {
        
        UCHAR   hubPort = device->GetHubPort();
        
        //
        //  Report the device as disconnected
        //
        parent->DeviceDisconnected(hubPort);

        //
        //  Report the device as connected.
        //
        parent->DeviceConnected(hubPort, 5);
    }
}

VOID
FASTCALL
USBD_LoadClassDriver(IUsbDevice *Device, PNP_CLASS_ID ClassId)
/*++
Routine Description:

    This routine is a helper function for device enumeration.  It is used three times
    so it is extracted.  It kicks off the Class Driver portion of enumeration.
    
    Given a class ID it calls the USBD_FindClassDriver.  If it finds one
    it calls the class drivers AddDevice.  Otherwise, it calls Device->AddComplete 
    with an error code.

Arguments:
    
    Device - The device for which to load a driver.
    ClassId = PnP Class ID for the driver to load.

Return Value:

    None.

Context:
    Must be called at DISPATCH_LEVEL

--*/
{
    //
    //  Sets up the external port number
    //
    Device->SetExternalPort();
    if(XDEVICE_ILLEGAL_PORT == Device->m_ExternalPort)
    {
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    Device->m_ClassDriver = USBD_FindClassDriver(ClassId);

    //
    //  Call AddDevice if we found a class driver
    //
    if(Device->m_ClassDriver)
    {
        Device->m_ClassDriver->AddDevice(Device);
        //
        //  The class driver will call AddComplete, when it
        //  has completed its enumeration steps.
        //
    } else
    //
    //  We don't have a class driver, so call AddComplete
    //  with a failure code.
    //
    {
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
    }
    return;
}

UCHAR
FASTCALL
USBD_AllocateUsbAddress(
    IN PUSBD_HOST_CONTROLLER HostController
    )
/*++

Routine Description:


Arguments:

Return Value:

    Valid USB address (1..127) to use for this device,
    returns 0 if no device address available.

--*/
{
    UCHAR   address     = 0;
    UCHAR   dwordIndex  = 0;
    ULONG   bitMask     = 1;
    
    //
    //  The AddressList consists of four DWORDS.
    //  Together they form a 128 bit bitfield.
    //  bits that are set, are address that are
    //  taken.
    //
        
    //
    //  Search for a free address
    //
    while(++address < 128)
    {
        //
        //  If the address is available, claim it and break
        //  out of loop.
        //
        if(!(HostController->AddressList[dwordIndex] & bitMask))
        {
            HostController->AddressList[dwordIndex] |= bitMask;
            break;
        }
        bitMask <<= 1;
        if(!bitMask)
        {
            dwordIndex++;
            bitMask = 1;
        }
    }
    //
    //  Handle case of out of addresses (we fell through the loop).
    //
    //  if(address == 128) address = 0;
    address &= 0x007F;  //This is more efficient than above.

    return address;
}

VOID
FASTCALL
USBD_FreeUsbAddress(
    IN PUSBD_HOST_CONTROLLER HostController,
    IN UCHAR DeviceAddress
    )
/*++

Routine Description:


Arguments:

Return Value:

    Valid USB address (1..127) to use for this device,
    returns 0 if no device address available.

--*/
{
    UCHAR                   dwordIndex  = 0;

    //
    // Sanity check address range
    //
    ASSERT(DeviceAddress > 0 && DeviceAddress < 128);
    
    //
    //  Reduce the DeviceAddress to a dwordIndex and
    //  a bitIndex (DeviceAddress will be the bitIndex
    //  when we leave this loop.
    //
    DeviceAddress--;
    while(DeviceAddress > 31)
    {
        dwordIndex++;
        DeviceAddress -= 32;
    }
    
    //
    //  Now clear the bit
    //
    HostController->AddressList[dwordIndex] &= ~(1 << DeviceAddress);
}
