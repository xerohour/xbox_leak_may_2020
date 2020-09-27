/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    USBTEST.H

Abstract:

    USBTEST.LIB

    Header file for USB Test Utility Methods

    This header declares all the methods exported
    from USBTEST.LIB

Environment:

    kernel mode

Revision History:

    05-04-2001 : mitchd
    

--*/

//*****************************************************************************
// Includes
//*****************************************************************************
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xdbg.h>
#include <usbd.h>


IUsbDevice *GetXidDeviceInterface(PXPP_DEVICE_TYPE XppDeviceType, DWORD dwPort, DWORD dwSlot);
/*
    Walks a XID handle to get the underlying IUsbDevice.
    If the XID device is not connected (stranded handle)
    then it return NULL.
*/

IUsbDevice *GetParentInterface(IUsbDevice *pUsbDevice);
/*
    Gets the parent of a USB device.  Returns parent of
    device.  
*/

UCHAR GetHubPort(IUsbDevice *pUsbDevice);
/*
    Gets the hub port from an IUsbDevice, so that
    IsDeviceAttached can be called.
*/

BOOL IsDeviceAttached(IUsbDevice *pHub, UCHAR uHubPort);
/*
    Returns TRUE if the hub thinks a device is attached to 
    the port.   This is the real port of the hub.
*/

VOID
DisableHubPortSync(
	IN IUsbDevice *pHub,
	IN UCHAR	PortNumber
	);
/*++
  Routine Description:
   Disables a port on hub.  Transparently handles root hub
   versus real hub.  This does not use the hub driver.
   This routine is not safe in an actual hot-plug situation.
   It is good for forcing the reenumeration of devices.

   On like reset device is doesn't suffer from the address
   recycling problem, so you can hit multiple devices
   at once with it.

   Additionally, it tests the ability of class drivers to
   detect and recover from devices that stop responding.
--*/

void ResetDevice(IUsbDevice *Device, UCHAR MaxRetries);
/*++
  Routine Description:
    Resets a device forcing a reenumeration, just like
    DeviceNotResponding, but allows retries to be set.
--*/