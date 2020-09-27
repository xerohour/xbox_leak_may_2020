/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    xpp.c

Abstract:

    Xbox Peripheral Port support

    The USB stack reports the insertion and removal of all devices here.  This module converts the PNP_CLASS_ID
    to the Xbox Device Type and vice versa.

    The Xbox device type is a simple 0 based index into a table of devices allowing an efficient implementation
    of XGetDevices, XGetDeviceChanges.

Environment:

    XAPI

Notes:

Revision History:

    06-26-00 created by Mitchell Dernis (mitchd)

--*/

#include "basedll.h"
#pragma warning( push, 4 )
#pragma warning( disable: 4213)
#include <usbxapi.h>

typedef struct _XPP_DEVICE_TYPE_INTERNAL
{
    ULONG CurrentConnected;
    ULONG ChangeConnected;
    ULONG PreviousConnected;
} XPP_DEVICE_TYPE_INTERNAL, *PXPP_DEVICE_TYPE_INTERNAL;

BOOL XPP_XInitDevicesHasBeenCalled = FALSE;

VOID
XdReportDeviceInsertionRemoval(
    PXPP_DEVICE_TYPE XppDeviceType,
    ULONG    PortBit,
    BOOLEAN  fInserted
    )

/*++
Routine Description:

    Called by the USB stack to report the insertion and removal of devices.
    Keeps the XPP_GlobalDeviceTable up-to-date.

Comments:
    This routine is called at DPC level

Parameters:
    XppDeviceType - Pointer to XPP type.
    PortBit   - Port bit position in bitmap.
    fInserted - TRUE on device insertion, FALSE on removal.

--*/
{
    DWORD dwPortMask = 1 << PortBit;
    PXPP_DEVICE_TYPE_INTERNAL pXppTypeInternal =
        (PXPP_DEVICE_TYPE_INTERNAL) XppDeviceType;
    ASSERT(sizeof(PXPP_DEVICE_TYPE_INTERNAL)==sizeof(PXPP_DEVICE_TYPE));

    //
    //  Set the change flag
    //
    pXppTypeInternal->ChangeConnected |=dwPortMask;
    //
    //  Set or clear the CurrentConnected bit
    //
    if(fInserted)
    {
        pXppTypeInternal->CurrentConnected |= dwPortMask;
    } else
    {
        pXppTypeInternal->CurrentConnected &= ~dwPortMask;
    }
}

//------------------------------------------------------------------
//  Internal API
//------------------------------------------------------------------
DWORD
WINAPI
XPeekDevices(
    IN  PXPP_DEVICE_TYPE DeviceType,
    IN  OUT PDWORD pLastGotten,
    IN  OUT PDWORD pStale
    )
/*++
  Routine Description:
    This is an internal routine which can be used to determine which devices are connected.
    It is designed not to interfere with the game which calles XGetDevices and XGetDeviceChanges.
    It also allows the caller to track what the game knows about.
  Arguments:
    DeviceType  - the device for which information is requested.
    pLastGotten - If non-NULL, filled out on exit with a bitmap of connected devices
                  as seen by the game.
    pState      - If non-NULL, filled out on exit with a bitmap of devices which have
                  been removed and reinserted since the last time the game called
                  XGetDevices or XGetDeviceChanges.
--*/
{
    KIRQL oldIrql;
    DWORD dwRetVal;
    PXPP_DEVICE_TYPE_INTERNAL pXppTypeInternal = 
        (PXPP_DEVICE_TYPE_INTERNAL) DeviceType;

    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XGetDevices: XInitDevices must be called first!");
  
    //
    //  Raise to DPC for sync.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Return all devices
    //
    dwRetVal = pXppTypeInternal->CurrentConnected;

    //
    //  Fill out *pLastGotten.
    //
    if(pLastGotten)
    {
        *pLastGotten = pXppTypeInternal->PreviousConnected;
    }

    //
    //  Fill out *pStale.
    //
    if(pStale)
    {
        *pStale = pXppTypeInternal->CurrentConnected & 
                  pXppTypeInternal->PreviousConnected &
                  pXppTypeInternal->ChangeConnected;
    }

    //
    //  Done with syncronization.
    //
    KeLowerIrql(oldIrql);
    return dwRetVal;
}


//------------------------------------------------------------------
//  Public API
//------------------------------------------------------------------

VOID
WINAPI
XInitDevices(DWORD NumDeviceTypes, PXDEVICE_PREALLOC_TYPE DeviceTypes)
{
#if DBG
    if(XPP_XInitDevicesHasBeenCalled)
    {
        RIP("XInitDevices() is called more than once.  Fatal Error.");
    }
    else
    {
        XPP_XInitDevicesHasBeenCalled = TRUE;
    }
#endif

    USBD_Init(NumDeviceTypes, DeviceTypes);
}


DWORD
WINAPI
XGetDevices(
    IN  PXPP_DEVICE_TYPE DeviceType
    )
{
    KIRQL oldIrql;
    DWORD dwRetVal;
    PXPP_DEVICE_TYPE_INTERNAL pXppTypeInternal = 
        (PXPP_DEVICE_TYPE_INTERNAL) DeviceType;

    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XGetDevices: XInitDevices must be called first!");
  
    //
    //  Raise to DPC for sync.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();
    //
    //  Fill in insertion bitmap with all devices
    //
    dwRetVal = pXppTypeInternal->CurrentConnected;
    //
    //  Erase changed and reset previous
    //
    pXppTypeInternal->ChangeConnected = 0;
    pXppTypeInternal->PreviousConnected = pXppTypeInternal->CurrentConnected;

    //
    //  Done with syncronization.
    //
    KeLowerIrql(oldIrql);
    return dwRetVal;
}

BOOL
WINAPI
XGetDeviceChanges(
    IN  IN  PXPP_DEVICE_TYPE DeviceType,
    OUT PDWORD pInsertions,
    OUT PDWORD pRemovals
    )
{
    KIRQL oldIrql;

    PXPP_DEVICE_TYPE_INTERNAL pXppTypeInternal = 
        (PXPP_DEVICE_TYPE_INTERNAL) DeviceType;

    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XGetDeviceChanges: XInitDevices must be called first!");
    
    if(!pXppTypeInternal->ChangeConnected)
    {
        //
        //  Nothing has changed since the last call.  Get out quickly.
        //
        *pInsertions = 0;
        *pRemovals = 0;
        return FALSE;
    }
    else
    {
      //
      //  Something has changed, so calculate the full info.
      //

      ULONG RemoveInsert;

      //
      //  Raise to DPC for sync.
      //

      oldIrql = KeRaiseIrqlToDpcLevel();

      //
      //  Construct the simple case of add or remove.
      //

      *pInsertions = (pXppTypeInternal->CurrentConnected & ~pXppTypeInternal->PreviousConnected);
      *pRemovals = (pXppTypeInternal->PreviousConnected & ~pXppTypeInternal->CurrentConnected);

      //
      // Now add more insertions and removals for the case of remove and reinsert
      //

      RemoveInsert = pXppTypeInternal->ChangeConnected &
                     pXppTypeInternal->CurrentConnected &
                     pXppTypeInternal->PreviousConnected;
      *pRemovals |= RemoveInsert;
      *pInsertions |= RemoveInsert;

      //
      //  Record that we retrieved info
      //
      
      pXppTypeInternal->ChangeConnected = 0;
      pXppTypeInternal->PreviousConnected = pXppTypeInternal->CurrentConnected;
      
      //
      //  Done with syncronization.
      //

      KeLowerIrql(oldIrql);
      return (*pInsertions|*pRemovals) ? TRUE : FALSE;
    }
}

