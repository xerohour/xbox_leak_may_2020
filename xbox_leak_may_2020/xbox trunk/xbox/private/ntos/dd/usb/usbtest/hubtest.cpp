#include <usbtest.h>
extern "C" 
{
#include "ohcd.h"
}
#include "hub.h"


BOOL IsDeviceAttached(IUsbDevice *pHub, UCHAR uHubPort)
{
    UCHAR ucConnectedPorts;
    if(UDN_TYPE_ROOT_HUB==pHub->m_Type)
    {
        POHCD_DEVICE_EXTENSION deviceExtension;
        deviceExtension = (POHCD_DEVICE_EXTENSION)
                           USBD_GetHCDExtension(pHub->m_HostController);
        ucConnectedPorts = deviceExtension->RootHubObject.DeviceDetectedBitmap;
    } else
    {
        ASSERT(UDN_TYPE_HUB==pHub->m_Type);
        PUSBHUB_DEVICE_NODE pUsbHub;
        pUsbHub = (PUSBHUB_DEVICE_NODE)pHub->GetExtension();
        ucConnectedPorts = pUsbHub->PortConnectedBitmap;
    }
    if(ucConnectedPorts&(1<<(uHubPort-1)))
    {
        return TRUE;
    }
    return FALSE;
}


VOID
DisableHubPortSync(
	IN IUsbDevice *pHub,
	IN UCHAR	PortNumber
	)
{
    if(UDN_TYPE_ROOT_HUB==pHub->m_Type)
    {
        POHCD_DEVICE_EXTENSION deviceExtension = (POHCD_DEVICE_EXTENSION)
                                                 USBD_GetHCDExtension(pHub->m_HostController);
        HCD_DisableRootHubPort(
            USBD_GetHCDExtension(pHub->m_HostController),
            PortNumber
            );
    } else
    {
        URB Urb;
        USB_BUILD_CONTROL_TRANSFER(
		    &Urb.ControlTransfer,
		    NULL,
		    NULL,
		    0,
		    0,
		    NULL, //Make the call synchronously
		    NULL,
		    FALSE,
		    USB_HOST_TO_DEVICE | USB_CLASS_COMMAND | USB_COMMAND_TO_OTHER,
		    USB_REQUEST_CLEAR_FEATURE,
		    USBHUB_FEATURE_PORT_ENABLE,
		    PortNumber,
		    0
		    );
        pHub->SubmitRequest(&Urb);
    }
}