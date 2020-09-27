#include <usbtest.h>
#include "xid.h"

PXID_DEVICE_NODE
FASTCALL
XID_fFindNode(
    IN PXID_TYPE_INFORMATION TypeInformation,
    IN ULONG  Port
    );


IUsbDevice *GetXidDeviceInterface(PXPP_DEVICE_TYPE XppDeviceType, DWORD dwPort, DWORD dwSlot)
{
    UCHAR xidType;
    PXID_TYPE_INFORMATION xidTypeInformation;

    xidTypeInformation = GetTypeInformation(XppDeviceType);

    if(xidTypeInformation)
    {
        xidType = XID_DEVTYPE_GAMECONTROLLER;
        PXID_DEVICE_NODE xidNode = XID_fFindNode(xidTypeInformation, (dwSlot != XDEVICE_BOTTOM_SLOT) ? dwPort : dwPort +16);
        if(xidNode)
        {
            return xidNode->Device;
        }
    }
    return NULL;
}