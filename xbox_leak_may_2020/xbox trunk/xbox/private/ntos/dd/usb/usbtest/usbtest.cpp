#include <usbtest.h>


IUsbDevice *
GetParentInterface(
    IUsbDevice *pUsbDevice
    )
/*++
  Routine Description:
    Gets the parent of a USB device.  Returns parent of
    device.  
--*/
{
    IUsbDevice *pParent;
    pParent = &g_DeviceTree.m_Devices[pUsbDevice->m_Parent];
    return pParent;
}

UCHAR 
GetHubPort(
    IUsbDevice *pUsbDevice
    )
/*++
  Routine Description:
    Gets the hub port in the parent.
--*/
{
    return pUsbDevice->m_PortNumber;
}

void ResetDevice(IUsbDevice *Device, UCHAR MaxRetries)
/*++
  Routine Description:
    Resets a device forcing a reenumeration, just like
    DeviceNotResponding, but allows retries to be set.
--*/
{
    IUsbDevice *device;
    
    //
    //  If the device is UDN_TYPE_INTERFACE, then the
    //  real node is the parent
    //
    if(UDN_TYPE_INTERFACE == Device->m_Type)
    {
        device = Device->GetParent();
        ASSERT(device);
    } else
    {
        device = Device;
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
        parent->DeviceConnected(hubPort, MaxRetries);
    }
}