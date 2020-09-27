/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    usbdev.cpp

Abstract:

    This module contains the implementation of IUsbDevice
    except for the routine involved with device enumeration.
    
Environment:

    XBOX kernel.

Notes:
  

Revision History:
    
    03-24-00: Create (mitchd)
    06-12-00: renamed to usbdev and changed to implement in c++.

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
//#include <xapidrv.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DBSU'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("USBD");


//
//  Pull in remaining usb headers
//
#include <usbd.h>

VOID
USBD_SubmitSynchronousRequestComplete(
    IN PURB     Urb,
    IN PKEVENT  Event
    )
/*++
Routine Description:

    Completion routine used by USBD_STATUS IUsbDevice::SubmitRequest,
    when blocking on an asynchronous request.

 Arguments:

    Urb     - URB that completed.
    Event   - Pointer to event that caller is blocking on.

Return Value:

    USBD_STATUS_SUCCESS - Or wide variety of possible errors.
    The possible errors for each URB can be found where the URB
    functions are defined.

--*/
{
    UNREFERENCED_PARAMETER(Urb);
    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
    return;
}


USBD_STATUS IUsbDevice::SubmitRequest(PURB Urb)
/*++
Routine Description:

    Class drivers call this function to submit URBs to the HCD.
    This routine picks off the URBs that it handles or preprocesses
    and then passes the URBs down to the HCD if necessary.

Arguments:

  Urb             -   URB to handle

Return Value:

    USBD_STATUS_SUCCESS - Or wide variety of possible errors.
    The possible errors for each URB can be found where the URB
    functions are defined.

--*/
{
    BOOLEAN block = FALSE;
    USBD_STATUS usbdStatus;
    KEVENT SyncEvent;
    BOOLEAN submitRequestHere = TRUE;
    //
    //  Check if this is an aynschrnous request and we need
    //  to block until done.
    //
    if(
        (Urb->Header.Function&URB_FUNCTION_ASYNCHRONOUS) &&
        (NULL == Urb->Header.CompleteProc)
    )
    {
        block = TRUE;
        ASSERT_LESS_THAN_DISPATCH_LEVEL();
        //
        //  Initialize an event, and setup completion
        //  routine that will set the event.
        //
        
        KeInitializeEvent(&SyncEvent, NotificationEvent, FALSE);
        Urb->Header.CompleteProc = (PURB_COMPLETE_PROC)USBD_SubmitSynchronousRequestComplete;
        Urb->Header.CompleteContext = (PVOID)&SyncEvent;
    }
    //
    //  There are couple of URBs we need to preprocess
    //
    switch(Urb->Header.Function)
    {
        case URB_FUNCTION_OPEN_DEFAULT_ENDPOINT:
            submitRequestHere = FALSE;
            usbdStatus = OpenDefaultEndpoint(Urb);
            break;
        case URB_FUNCTION_CLOSE_DEFAULT_ENDPOINT:
            submitRequestHere = FALSE;
            usbdStatus = CloseDefaultEndpoint(Urb);
            break;
        case URB_FUNCTION_OPEN_ENDPOINT:
            Urb->OpenEndpoint.FunctionAddress = m_Address;
            Urb->OpenEndpoint.DataToggleBits = &m_DataToggleBits;
            Urb->OpenEndpoint.LowSpeed = GetLowSpeed();
            break;
        case URB_FUNCTION_CLOSE_ENDPOINT:
            Urb->CloseEndpoint.DataToggleBits = &m_DataToggleBits;
            break;
        case URB_FUNCTION_ISOCH_OPEN_ENDPOINT:
            Urb->IsochOpenEndpoint.FunctionAddress = m_Address;
            break;
        //case URB_FUNCTION_RESET_PORT:
        case URB_FUNCTION_CONTROL_TRANSFER:
            //
            //  Resolve the endpoint handle for the default endpoint.
            //
            if(NULL == Urb->ControlTransfer.EndpointHandle)
            {
                ASSERT(m_DefaultEndpoint);
                Urb->ControlTransfer.EndpointHandle = m_DefaultEndpoint;
            }
            //
            //  If this is a set configuration, clear the cached toggle
            //  bits.
            //
            if(USB_REQUEST_SET_CONFIGURATION == Urb->ControlTransfer.SetupPacket.bRequest)
            {
                m_DataToggleBits = 0;
            }
            break;
    }
    //
    //  Pass URB on to HCD  (Open and close default endpoint, do it
    //  before getting here).
    if(submitRequestHere)
    {
        usbdStatus = HCD_SubmitRequest(USBD_GetHCDExtension(m_HostController), Urb);

    }
    //
    //  Check if we need to wait.
    //
    if(block)
    {
        if(USBD_PENDING(usbdStatus))
        {
            KeWaitForSingleObject(&SyncEvent, Executive, KernelMode, FALSE, NULL);
            usbdStatus = Urb->Header.Status;
        }
        Urb->Header.CompleteProc = NULL;
        Urb->Header.CompleteContext = NULL;
    }
    return usbdStatus;
}

USBD_STATUS IUsbDevice::CancelRequest(PURB Urb)
{
    return HCD_CancelRequest(USBD_GetHCDExtension(m_HostController), Urb);
}


PVOID IUsbDevice::GetExtension() const
/*++
Routine Description:
    Retrieves the value of a the class driver defined
    extension.
Return Value:
    Previously set class driver extension.
--*/
{
    return m_ClassDriverExtension;
}
PVOID IUsbDevice::SetExtension(PVOID Extension)
/*++
Routine Description:
    Allows a the class driver to set an extension.
Return Value:
    Previous value for class driver extension.
--*/
{
    PVOID temp = m_ClassDriverExtension;
    m_ClassDriverExtension = Extension;
    return temp;
}

BOOLEAN IUsbDevice::IsHardwareConnected() const
/*++
Routine Description:

    Retrieves whether the hardware is still connected.
    
    The implmentation depends on the fact that the very first
    step when getting DeviceDisconnected call from the HUB
    is to orphan the device node.

Return Value:

    Previously set class driver extension.

--*/
{
    if(UDN_TYPE_INTERFACE == m_Type)
    {
        return (GetParent()->GetParent()) ? TRUE : FALSE;
    } else
    {
        return GetParent() ? TRUE : FALSE;
    }
}

UCHAR IUsbDevice::GetInterfaceNumber() const
/*++
Routine Description:
    Returns the interface number for a device
Return Value:
    The interface number that was originally
    in the interface descriptor.
--*/
{
    ASSERT((UDN_TYPE_INTERFACE == m_Type) || (UDN_TYPE_INTERFACE_FUNCTION == m_Type));
    return this->m_bInterfaceNumber;
}

void IUsbDevice::SetClassSpecificType(UCHAR ClassSpecificType)
/*++
Routine Description:
	Class driver calls this to set the class specific byte in the USB_DEVICE_TYPE
	returned for this device.
Return Value:
    none.
--*/
{
	m_ClassSpecificType = ClassSpecificType;
}

ULONG IUsbDevice::Win32FromUsbdStatus(USBD_STATUS UsbdStatus)
/*++
Routine Description:
    Returns a Win32 error code (NOT NT_STATUS) given a USBD_STATUS code.
Comment:
    This makes good sense because XID exposes an API directly,
    we may need to provide a separate function to convert to NT_STATUS
    for class drivers with layers above them.
--*/
{
    switch(UsbdStatus)
    {
        case USBD_STATUS_SUCCESS:
            return ERROR_SUCCESS;
        case USBD_STATUS_PENDING:
            return ERROR_IO_PENDING;
        case USBD_STATUS_ERROR:
        case USBD_STATUS_HALTED:
        case USBD_STATUS_CRC:
        case USBD_STATUS_BTSTUFF:
        case USBD_STATUS_DATA_TOGGLE_MISMATCH:
        case USBD_STATUS_STALL_PID:
        case USBD_STATUS_DEV_NOT_RESPONDING:
        case USBD_STATUS_PID_CHECK_FAILURE:
        case USBD_STATUS_UNEXPECTED_PID:
        case USBD_STATUS_DATA_OVERRUN:
        case USBD_STATUS_DATA_UNDERRUN:
        case USBD_STATUS_RESERVED1:
        case USBD_STATUS_RESERVED2:
        case USBD_STATUS_BUFFER_OVERRUN:
        case USBD_STATUS_BUFFER_UNDERRUN:
        case USBD_STATUS_NOT_ACCESSED:
        case USBD_STATUS_FIFO:
            return ERROR_IO_DEVICE;
        case USBD_STATUS_NO_MEMORY:
            return ERROR_OUTOFMEMORY;
        case USBD_STATUS_NO_BANDWIDTH:
            return ERROR_NO_SYSTEM_RESOURCES;
        case USBD_STATUS_CANCELED:
            return ERROR_CANCELLED;
        default:
            return ERROR_GEN_FAILURE;
    }
}

NTSTATUS IUsbDevice::NtStatusFromUsbdStatus(USBD_STATUS UsbdStatus)
/*++
Routine Description:
    Returns a NTSTATUS error code given a USBD_STATUS code.
Comment:
--*/
{
    switch(UsbdStatus)
    {
        case USBD_STATUS_SUCCESS:
            return STATUS_SUCCESS;
        case USBD_STATUS_PENDING:
            return STATUS_PENDING;
        case USBD_STATUS_ERROR:
        case USBD_STATUS_HALTED:
        case USBD_STATUS_CRC:
        case USBD_STATUS_BTSTUFF:
        case USBD_STATUS_DATA_TOGGLE_MISMATCH:
        case USBD_STATUS_STALL_PID:
        case USBD_STATUS_DEV_NOT_RESPONDING:
        case USBD_STATUS_PID_CHECK_FAILURE:
        case USBD_STATUS_UNEXPECTED_PID:
        case USBD_STATUS_DATA_OVERRUN:
        case USBD_STATUS_DATA_UNDERRUN:
        case USBD_STATUS_RESERVED1:
        case USBD_STATUS_RESERVED2:
        case USBD_STATUS_BUFFER_OVERRUN:
        case USBD_STATUS_BUFFER_UNDERRUN:
        case USBD_STATUS_NOT_ACCESSED:
        case USBD_STATUS_FIFO:
            return STATUS_IO_DEVICE_ERROR;
        case USBD_STATUS_NO_BANDWIDTH:
        case USBD_STATUS_NO_MEMORY:
            return STATUS_INSUFFICIENT_RESOURCES;
        case USBD_STATUS_CANCELED:
            return STATUS_CANCELLED;
        default:
            return STATUS_UNSUCCESSFUL;
    }
}

const USB_DEVICE_DESCRIPTOR8 *IUsbDevice::GetDeviceDescriptor() const
/*++
Routine Description:
    Provides Class Driver with access to first eight bytes of device descriptor.
--*/
{
    ASSERT(IsEnumTime());
    return &g_DeviceTree.m_DeviceDescriptor;
}

const USB_CONFIGURATION_DESCRIPTOR *IUsbDevice::GetConfigurationDescriptor() const
/*++
Routine Description:
    Provides Class Driver with access to Configuration descriptor.
    Only available to Interface Class Drivers at enum time.
--*/
{
    ASSERT(IsEnumTime());
    ASSERT((UDN_TYPE_INTERFACE == m_Type)||(UDN_TYPE_INTERFACE_FUNCTION == m_Type));
    return (PUSB_CONFIGURATION_DESCRIPTOR)g_DeviceTree.m_ConfigurationDescriptorBuffer;
}

const USB_INTERFACE_DESCRIPTOR *IUsbDevice::GetInterfaceDescriptor() const
/*++
Routine Description:
    Provides Class Driver with access to Configuration descriptor.
    Only available to Interface Class Drivers at enum time.
--*/
{
    ASSERT(IsEnumTime());
    ASSERT((UDN_TYPE_INTERFACE == m_Type)||(UDN_TYPE_INTERFACE_FUNCTION == m_Type));
    return g_DeviceTree.m_InterfaceDescriptor;
}


const USB_ENDPOINT_DESCRIPTOR *IUsbDevice::GetEndpointDescriptor(UCHAR EndpointType, BOOLEAN InDirection, UCHAR Index) const
/*++
Routine Description:
    Searches the class drivers interface for an endpoint.
    Only available to Interface Class Drivers at enum time.
Arguments:
    EndpointType - from usb100.h one of:
                    USB_ENDPOINT_TYPE_CONTROL
                    USB_ENDPOINT_TYPE_ISOCHRONOUS
                    USB_ENDPOINT_TYPE_BULK
                    USB_ENDPOINT_TYPE_INTERRUPT
    Direction - True if the endpoint direction is in.  Ignored for USB_ENDPOINT_TYPE_CONTROL.
    Index - Zero based index to endpoint of that type and direction.
    e.g. Find the first bulk-out endpoint: GetEndpointDescriptor(USB_ENDPOINT_TYPE_BULK, FALSE, 0).
         Find the second bulk-out endpoint: GetEndpointDescriptor(USB_ENDPOINT_TYPE_BULK, FALSE, 1).
Return Value:
    Pointer to endpoint descriptor for matching endpoint.
    NULL if endpoint not found.
--*/
{
    ASSERT(IsEnumTime());
    ASSERT((UDN_TYPE_INTERFACE == m_Type)||(UDN_TYPE_INTERFACE_FUNCTION == m_Type));

    PUCHAR currentParseLocation = (PUCHAR)g_DeviceTree.m_InterfaceDescriptor;
    PUCHAR endOfCongifuration = g_DeviceTree.m_ConfigurationDescriptorBuffer + 
            ((PUSB_CONFIGURATION_DESCRIPTOR)g_DeviceTree.m_ConfigurationDescriptorBuffer)->wTotalLength;
    PUSB_ENDPOINT_DESCRIPTOR endpointDescriptor = NULL;
    do
    {
        if(0==((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bLength) break;
        currentParseLocation += ((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bLength;
        if(currentParseLocation >= endOfCongifuration) break;
        
        if(((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE)
        {
            PUSB_ENDPOINT_DESCRIPTOR temp = (PUSB_ENDPOINT_DESCRIPTOR)currentParseLocation;
            if((temp->bmAttributes&USB_ENDPOINT_TYPE_MASK) == EndpointType)
            {
                if( 
                    (EndpointType != USB_ENDPOINT_TYPE_CONTROL) &&
					//When comparing BOOLEANs, compare their logical NOT's
                    (!USB_ENDPOINT_DIRECTION_IN(temp->bEndpointAddress) == !InDirection) 
                ){
                    if(0 == Index--)
                    {
                        endpointDescriptor = temp;
                        break;
                    }
                }
            }
        }   
    } while(((PUSB_COMMON_DESCRIPTOR)currentParseLocation)->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE);
    return endpointDescriptor;
}

ULONG IUsbDevice::GetPort() const
{
    return m_ExternalPort;
}

USBD_STATUS IUsbDevice::OpenDefaultEndpoint(PURB Urb)
/*++
Routine Description:

    Default endpoints are owned by USBD not by the class
    drivers.  However, USBD only uses the default endpoint
    during enumeration and then closes it.  If a class driver
    needs to send commands over the default endpoint is must
    send an URB_FUNCTION_OPEN_DEFAULT_ENDPOINT, which ends
    up in this routine.

    For UDN_TYPE_INTERFACE, there is one default endpoint
    shared between multiple interfaces (i.e. class drivers).
    The default endpoint may be open for the parent ofd type
    UDN_TYPE_COMPOSITE_FUNCTION, and therefore for at least one
    of the siblings.  In this case, we can handle the URB competely
    here.  If it is not open for the parent we need to open the
    default endpoint for the parent first.

    For UDN_TYPE_INTERFACE_FUNCTION, UDN_TYPE_FUNCTION, and
    UDN_TYPE_HUB the default endpoint is not shared.  We just
    need to copy some info out of the node into the URB before
    passing it down.

    Fortunately, OpenDefaultEndpoint is inherently synchronous so
    we can do post-processing without blocking.

    This routine does not use preallocating, because it
    should never be called during enumeration.
    
Arguments:
    
    Urb             -   URB to handle

Return Value:

    USBD_STATUS_SUCCESS
    USBD_STATUS_NO_MEMORY - If there is not enough memory for the endpoint.

--*/
{
    USBD_STATUS status;
    IUsbDevice *parent=NULL;
    //
    //  If this assertion is hit, someone is trying to open
    //  a default endpoint that is already open.
    //
    ASSERT(NULL == m_DefaultEndpoint);

    //
    //  Handle UDN_TYPE_INTERFACE.
    //
    if(UDN_TYPE_INTERFACE == m_Type)
    {
        parent = GetParent();
        ASSERT(parent);
        if(parent->m_DefaultEndpoint)
        {
            //
            //  The parent was open just copy the endpoint handle.
            //
            m_DefaultEndpoint = parent->m_DefaultEndpoint;
            USB_DBG_TRACE_PRINT(("Open Default endpoint for UDN_TYPE_INTERFACE already open using parent's"));
            //
            //  Even inherently synchronous URBs need completion
            //
            Urb->Header.Status = USBD_STATUS_SUCCESS;
            USBD_CompleteRequest(Urb);
            return USBD_STATUS_SUCCESS;
        }
    }

    //
    //  We actually need to open an endpoint, which means we need
    //  to finish filling out the URB with stuff from the node.
    //
    Urb->OpenEndpoint.FunctionAddress = m_Address;
    Urb->OpenEndpoint.EndpointAddress = 0;
    Urb->OpenEndpoint.EndpointType = USB_ENDPOINT_TYPE_CONTROL;
    Urb->OpenEndpoint.MaxPacketSize = m_MaxPacket0;
    Urb->OpenEndpoint.LowSpeed = GetLowSpeed();
    Urb->OpenEndpoint.Hdr.Function = URB_FUNCTION_OPEN_ENDPOINT;
    Urb->OpenEndpoint.DataToggleBits = NULL;

    //
    //  Pass URB on to HCD
    //
    status = HCD_SubmitRequest(USBD_GetHCDExtension(m_HostController), Urb);
    
    //
    //  If the endpoint successfully opened, store away the handle.
    //
    if(USBD_SUCCESS(status))
    {
        m_DefaultEndpoint = Urb->OpenEndpoint.EndpointHandle;
        if(parent)
        {
             parent->m_DefaultEndpoint = Urb->OpenEndpoint.EndpointHandle;
        }
        ASSERT(m_DefaultEndpoint);
    } else
    {
        ASSERT(FALSE);
    }

    //
    //  Don't show caller default endpoint.
    //
    Urb->OpenEndpoint.EndpointHandle = NULL;
    //
    //  HCD completed the URB so we shouldn't.
    //
    return status;
}


USBD_STATUS IUsbDevice::CloseDefaultEndpoint(PURB Urb)
/*++
Routine Description:

    This routine is called in response to URB_CLOSE_DEFAULT_ENDPOINT and it
    undoes IUsbDevice::OpenDefaultEndpoint.
    
Arguments:
    
    Urb             -   URB to handle

Return Value:

    USBD_STATUS_SUCCESS or USBD_STATUS_PENDING

--*/
{
    //
    //  Mark the default endpoint close, but cache the handle
    //  in a local variable.
    //
    PVOID endpoint = m_DefaultEndpoint;
    m_DefaultEndpoint = NULL;
    //
    //  Handle UDN_TYPE_INTERFACE.
    //
    if(UDN_TYPE_INTERFACE == m_Type)
    {
        IUsbDevice *sibling;
        //
        //  Now see if any of the siblings still have the endpoint open.
        //  If even one has it open, we can complete the URB
        //  and exit.
        //
        sibling = GetParent()->GetFirstChild();
        while(sibling)
        {
            //
            //  We don't check for NULL.  Why?  Look at the note in IUsbDevice::AddComplete.
            //
            if(sibling->m_DefaultEndpoint == endpoint)
            {
                Urb->Header.Status = USBD_STATUS_SUCCESS;
                USBD_CompleteRequest(Urb);
                return USBD_STATUS_SUCCESS;
            }
            //
            //  Try next sibling
            //
            sibling = sibling->GetSibling();
        }
        //
        // If we made it through the loop then close
        // endpoint for real
        //
        GetParent()->m_DefaultEndpoint = NULL;
    }

    //
    //  Munge URB and pass on to HCD
    //
    Urb->CloseEndpoint.Hdr.Function = URB_FUNCTION_CLOSE_ENDPOINT;
    Urb->CloseEndpoint.EndpointHandle = endpoint;
    Urb->OpenEndpoint.DataToggleBits = NULL;
    return HCD_SubmitRequest(USBD_GetHCDExtension(m_HostController), Urb);
}

IUsbDevice *IUsbDevice::FindChild(UCHAR PortNumber) const
/*++
    Returns a pointer to a child device given its port number.  NULL if not found.
--*/
{
    
    //  This should only be called for hubs, root_hubs and composite devices.
    ASSERT( (UDN_TYPE_HUB == m_Type) || (UDN_TYPE_ROOT_HUB == m_Type) || (UDN_TYPE_COMPOSITE_FUNCTION == m_Type));

    IUsbDevice *device = GetFirstChild();
    while( device && (device->m_PortNumber & ~UDN_LOWSPEED_PORT) != (PortNumber & ~UDN_LOWSPEED_PORT))
    {
        device = device->GetSibling();
    }
    return device;
}

void IUsbDevice::InsertChild(IUsbDevice *child)
/*++
    Inserts a device as a child of this device.
    It is important to make it the last child,
    because FindChild finds the first matching
    child.  If there is a remove and a reinsert
    between enumeration stages, there could be
    two matching children, one pending removal.
    So it is imporant to add from the opposite end
    as removal (i.e. finding , since we remove
    what we find).
--*/
{
    //  This should only be called for hubs, root_hubs and composite devices.
    ASSERT( (UDN_TYPE_HUB == m_Type) || (UDN_TYPE_ROOT_HUB == m_Type) || (UDN_TYPE_COMPOSITE_FUNCTION == m_Type));
    ASSERT(child >= g_DeviceTree.m_Devices);
    //
    //  Get the nodeIndex and debug assert that it is valid
    //
    UCHAR nodeIndex = child - g_DeviceTree.m_Devices;
    ASSERT(UDN_INVALID_NODE_INDEX > nodeIndex);

    //
    // We know the childs parent and sibling now, so fill it in.
    //
    child->m_Sibling = UDN_INVALID_NODE_INDEX;
    child->m_Parent = this - g_DeviceTree.m_Devices;
	

    IUsbDevice *existingChild = GetFirstChild();
    
	if(!existingChild)
    //
    //  If there are no children, then it is the first.
    //
    {
        m_FirstChild = nodeIndex;
    } else
    //
    //  Otherwise, we need to insert at the end
    //
    {
        //
        //	Make sure we are not inserting the child twice
        //
        ASSERT(existingChild!=child);
        
        //
        //  walk to the end.
        //
        while(UDN_INVALID_NODE_INDEX != existingChild->m_Sibling)
        {
            existingChild = existingChild->GetSibling();
            //
            //	Make sure we are not inserting the child twice
            //
            ASSERT(existingChild!=child);
        }
        //
        //  attach to the end
        //
        existingChild->m_Sibling = nodeIndex;
    }
}


BOOLEAN IUsbDevice::RemoveChild(IUsbDevice *child)
/*++
    Removes a device from the list of children.
    Returns TRUE if the are still more children.
--*/
{
    //  This should only be called for hubs, root_hubs and composite devices.
    ASSERT( (UDN_TYPE_HUB == m_Type) || (UDN_TYPE_ROOT_HUB == m_Type) || (UDN_TYPE_COMPOSITE_FUNCTION == m_Type));

    IUsbDevice *prevChild = GetFirstChild();
    BOOLEAN moreChildren = TRUE;
    if(prevChild == child)
    {
        m_FirstChild = child->m_Sibling;
        if(m_FirstChild == UDN_INVALID_NODE_INDEX) moreChildren = FALSE;
    } else
    {
        
        while(prevChild && (prevChild->GetSibling() != child))
        {                                   
            prevChild = prevChild->GetSibling();
        }
        ASSERT(prevChild);
        if(prevChild)
        {
            prevChild->m_Sibling = child->m_Sibling;
        }
    }
    child->m_Sibling = UDN_INVALID_NODE_INDEX;
    child->m_Parent = UDN_INVALID_NODE_INDEX;
    return moreChildren;
}
 
PNP_CLASS_ID IUsbDevice::GetClassId() const
{
	PNP_CLASS_ID classId;
	if(0xFF != m_ClassSpecificType)
	{
		classId.AsLong = m_ClassDriver->ClassId.AsLong;
		classId.USB.bClassSpecificType = m_ClassSpecificType;
	} else {
		classId.AsLong = -1;
	}
	return classId;
}


BOOLEAN IUsbDevice::IsEnumTime() const
/*++
    Test to see if this device is currently being enumerated.
    This routine gets called in DEBUG mode for lots of calls
    to IUsbDevice from the ClassDriver.  It is also used
    in the retail build to decide whether or not to use a pre-allocated
    an endpoint on open.
--*/
{
    //
    //  If you are not at DISPATCH_LEVEL you could
    //  leave EnumTime whenever, so effectively it
    //  is not EnumTime.
    //
    if(DISPATCH_LEVEL != KeGetCurrentIrql()) return FALSE;
    //
    // If nobody is being enumerated than we are not
    // being enumerated.
    //
    if(!g_DeviceTree.m_InProgress) return FALSE;
    
    //
    //  If we are UDN_TYPE_INTERFACE, then really it
    //  is our parent being enumerated.  We are than at enum
    //  time if:
    //  a) Our default endpoint non-NULL and different from
    //     our parents.
    //  b) Our previous sibling default endpoint is NULL or the same
    //      same as our parents.
    //  This condition implies that our parent is g_DeviceTree.m_CurrentEnum
    //  we will double check this in debug builds only.
    //
    //  We will decline to check b), because it is tough, and it would
    //  imply that a class driver got a handle to us prior to receiving
    //  AddDevice.  This driver doesn't call IsEnumTime for its own
    //  purposes anyway.
    //
    if(UDN_TYPE_INTERFACE == m_Type)
    {
        ASSERT(GetParent()); //Serious error for UDN_TYPE_INTERFACE not to have a parent.
        if( 
            (NULL == m_DefaultEndpoint) ||
            (GetParent()->m_DefaultEndpoint == m_DefaultEndpoint)
        ) return FALSE;
        ASSERT(g_DeviceTree.m_CurrentEnum == GetParent());
        return TRUE;
    }

    //
    //  So we are not of type UDN_TYPE_INTERFACE, which means
    //  that we are at EnumTime iff we are g_DeviceTree.m_CurrentEnum.
    //
    if(g_DeviceTree.m_CurrentEnum != this) return FALSE;
    return TRUE;
}


#ifndef SILVER  //SetExternalPort for BLACK builds

#define MAX_TREE_INDEX 5
void IUsbDevice::SetExternalPort()
/*++
  This routine calculates the external port number which includes
  the slot.

  0-3 is for a direct connect device or a top slot, and 16-19 if for a device
  that connects through the bottom slot.
--*/
{
    //
    //  Build Parent Device Array
    //
    IUsbDevice *pParentArrayBuffer[MAX_TREE_INDEX+1];
    IUsbDevice **pParentArray;
    UINT parentIndex = MAX_TREE_INDEX;
    UINT deviceIndex;
    pParentArrayBuffer[MAX_TREE_INDEX] = this;
    do
    {
        parentIndex--;
        ASSERT(parentIndex >= 0);
        pParentArrayBuffer[parentIndex] = pParentArrayBuffer[parentIndex+1]->GetParent();
    } while(UDN_TYPE_ROOT_HUB != pParentArrayBuffer[parentIndex]->m_Type);
    
    //
    //  Adjust pParentArray so that index 0 is the root hub
    //  and deviceIndex is the device itself.
    //
    pParentArray = pParentArrayBuffer + parentIndex;
    deviceIndex = MAX_TREE_INDEX - parentIndex;

    // This should not be possible
    ASSERT(0!=deviceIndex);

    //
    //  Call the appropriate configuration routine
    //
    if(XBOX_HW_FLAG_INTERNAL_USB_HUB & XboxHardwareInfo->Flags)
    {
        SetExternalPortWithHub(pParentArray, deviceIndex);
    } else
    {
        SetExternalPortWithoutHub(pParentArray, deviceIndex);
    }
}    


void IUsbDevice::SetExternalPortWithHub(IUsbDevice **pParentArray, UINT DeviceIndex)
{
    
    //
    //  We expect the device at index 1 to be a hub.
    //  Otherwise, someone is screwing with the configuration.
    //
    if(UDN_TYPE_HUB!=pParentArray[1]->m_Type)
    {
        m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        return;
    }

    //
    //  That first hub must be plugged into port 1 of the 
    //  root hub.
    //
    if(1!=pParentArray[1]->GetHubPort())
    {
        m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        return;
    }

    //If this is the internal hub we are done,
    if(1==DeviceIndex)
    {
        m_ExternalPort = 0;
        return;
    }

    //
    //  Otherwise, we can call the SetExternalPortWithoutHub
    //  after splicing the internal hub out of the topology.
    //
    pParentArray[1] = pParentArray[0];
    SetExternalPortWithoutHub(pParentArray+1, DeviceIndex-1);
}

void IUsbDevice::SetExternalPortWithoutHub(IUsbDevice **pParentArray, UINT DeviceIndex)
{
    //
    //  In general the port is the hub port that the device at index 1
    //  is plugged into. 
    //
    m_ExternalPort = (pParentArray[1]->GetHubPort()-1);
    if(m_ExternalPort >= XGetPortCount())
    {
        m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        return;
    }
    // A board layout problem on the motherboard (and on the daughter
    // hub to work around a USB drive level issue on the MCP rev B01)
    // requires us to swap ports 0 and 1 with ports 2 and 3. We can
    // do this by XOR with 2.  This assumes that there is no port 5.
    //
    m_ExternalPort ^= 2;

    //
    //  If the device is a hub, its parent must be the root hub.
    //
    if(UDN_TYPE_HUB==pParentArray[DeviceIndex]->m_Type)
    {
        if(1 == DeviceIndex) return;
        m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        return;
    }

    //
    //  If the device index is not plugged directly into the box
    //  (equivalent to the DeviceIndex being greater than 1)
    //  then the slot is determined by the HubPort that the device at
    //  DeviceIndex 2 is inserted into.  Port 1 is direct connect, port 2
    //  is top slot and port 3 is bottom slot.  Only port 3 changes m_ExternalPort
    //  (We need to reject anything plugged into another slot.)
    //
    if(DeviceIndex>1)
    {
        ULONG SlotPort = (pParentArray[2]->GetHubPort())-1;
        //
        //  Adjust m_ExternalPort with the slot information
        //
        if(SlotPort > 3)
        {
            m_ExternalPort = XDEVICE_ILLEGAL_PORT;    
        } else if(SlotPort == 2)
        {
            m_ExternalPort += 16;
        }
    }
    return;
}

#else  //SetExternalPort for SILVER builds
void IUsbDevice::SetExternalPort() 
/*++
   This routine calculates the external port number.  The external
   port number is 0-3 for devices that plug directly in to XBOX.
   This corresponds to the labels 1 through 4 on the outside of
   the box.  If the device is in the bottom slot of a game controller
   16 is added to the external port number.

   This code supports three configuration based on #define's
   
   USB_HOST_CONTROLLER_CONFIGURATION is to one of four
   values.

   USB_SINGLE_HOST_CONTROLLER
        Ports one to four on thre root host, become external ports 0-3.
        If externa; port four check for external hub, thus the external
        port could be greater.

   USB_DUAL_HOST_CONTROLLER_XDK
        The first Host Controllers' ports one and two, become external ports
        0 and 1.  The second Host Controllers' ports one and two
        become external ports 2 and 3.

        If external port three check for external hub, thus
        external port could be greater.

--*/
{
    //
    //  Build list of device.
    //
    IUsbDevice *pParentArrayBuffer[5];
    IUsbDevice **pParentArray;
    int parentIndex = 4;
    pParentArrayBuffer[4] = this;
    do
    {
        parentIndex--;
        ASSERT(parentIndex >= 0);
        pParentArrayBuffer[parentIndex] = pParentArrayBuffer[parentIndex+1]->GetParent();
    } while(UDN_TYPE_ROOT_HUB != pParentArrayBuffer[parentIndex]->m_Type);
    
    //
    //  Adjust pParentArray so that index 0, is the Root Hub
    //

    //pParentArray = &pParentArrayBuffer[parentIndex];
    pParentArray = pParentArrayBuffer + parentIndex;
    int maxIndex = 4 - parentIndex;
    
    //
    //  If the index is less than two, this is
    //  a hub, or some strange configuration
    //

    if(maxIndex < 1) return;
    
    //
    //  USB_SINGLE_HOST_CONTROLLER
    //

    #if (USB_HOST_CONTROLLER_CONFIGURATION==USB_SINGLE_HOST_CONTROLLER)
    ASSERT(1==pParentArray[0]->m_HostController->ControllerNumber); //It should always be host controller 1
    m_ExternalPort = pParentArray[1]->GetHubPort()-1;
    #endif //(USB_HOST_CONTROLLER_CONFIGURATION = USB_SINGLE_HOST_CONTROLLER)

    //
    //  USB_DUAL_HOST_CONTROLLER_XDK
    //

    #if (USB_HOST_CONTROLLER_CONFIGURATION==USB_DUAL_HOST_CONTROLLER_XDK)
    m_ExternalPort = pParentArray[1]->GetHubPort()-1;
    if(2==pParentArray[0]->m_HostController->ControllerNumber)
    {
        m_ExternalPort += 2;
    }
    #endif //(USBD_HOST_CONTROLLER_CONFIGURATION = USB_DUAL_HOST_CONTROLLER_XDK)

    int functionIndex = 2;

    //
    //  At this point the external port should be 0-3, (or 4 if it is a dual
    //  host controller for DVT) if not it is in an illegal port.
    //
    if(m_ExternalPort > 3)
    {
        m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        return;
    }
    
    //
    //  Deal with the possiblity of an external hub
    //
    if(
        (UDN_TYPE_HUB == pParentArray[1]->m_Type) &&
        (UDN_HUB_TYPE_EXTERNAL == pParentArray[1]->m_ClassSpecificType)
    )
    {
        if(3 == m_ExternalPort)
        {
            //
            //  If there are no more nodes, we are being
            //  called on the external hub, we are done.
            //

            if(maxIndex < 2)  return;
            m_ExternalPort = pParentArray[2]->GetHubPort()+2;
            functionIndex = 3;
        } else
        {
            m_ExternalPort = XDEVICE_ILLEGAL_PORT;
            return;
        }
    }
    
    //
    //  Exclude functions directly
    //  connected to the box.
    //
    #ifndef USB_ENABLE_DIRECT_CONNECT
    if(UDN_TYPE_ROOT_HUB == pParentArray[functionIndex-1]->m_Type)
    {
        m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        return;
    }
    if(UDN_TYPE_COMPOSITE_FUNCTION == pParentArray[functionIndex-1]->m_Type)
    {
        if(UDN_TYPE_ROOT_HUB == pParentArray[functionIndex-2]->m_Type)
        {
            m_ExternalPort = XDEVICE_ILLEGAL_PORT;
        }
        return;
    }
    #endif

    //
    //  We may be evaluating the parent of 
    //  a function, or a device plugged
    //  directly into the box.
    //

    if(functionIndex > maxIndex)
    {
        return;
    }
    
    //
    //  Check for bottom slot (or illegal slot)
    //
    BOOLEAN fBottomSlot = FALSE;
    //
    //  If the parent is a hub, then the slot is our port number
    //  1 or 2 = top, 3 = bottom, > 3 is illegal.
    if(UDN_TYPE_HUB == pParentArray[functionIndex-1]->m_Type)
    {
        if(pParentArray[functionIndex]->GetHubPort() == 3)
        {
            fBottomSlot = TRUE;
        } else if(pParentArray[functionIndex]->GetHubPort() > 3)
        {
           m_ExternalPort = XDEVICE_ILLEGAL_PORT;
           return;
        }
    }

    //
    //  If the parent is a composite, e.g. Hawk, then the slot
    //  is based on our parent port.
    //
    else if(UDN_TYPE_COMPOSITE_FUNCTION == pParentArray[functionIndex-1]->m_Type)
    {
        if(UDN_TYPE_HUB == pParentArray[functionIndex-2]->m_Type)
        {
            if(pParentArray[functionIndex-1]->GetHubPort() == 3)
            {
                fBottomSlot = TRUE;
            } else if(pParentArray[functionIndex-1]->GetHubPort() > 3)
            {
                m_ExternalPort = XDEVICE_ILLEGAL_PORT;
                return;
            }
        }
    }

    if(fBottomSlot)
    {
        m_ExternalPort += 16;
    }
    return;
}

#endif

    
