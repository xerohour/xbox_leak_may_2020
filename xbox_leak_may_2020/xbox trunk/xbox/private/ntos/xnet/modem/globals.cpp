/*++

Copyright (c) 2000 Microsoft Corporation

    globals.cpp

Abstract:
    
    Global variables for the USB modem.
    
Revision History:

    06-21-00    vadimg      created

--*/

#include "precomp.h"

USB_PNP_CLASS_DRIVER_DESCRIPTION gClassDescription;
USB_PNP_DEVICE_TYPE_DESCRIPTION  gDeviceDescription;

PMDL gpMdl;
URB gUrb;
BOOL gfUrbInUse;
BOOL gfRemoved;
KIRQL gIrql;

PMDL gpMdlRead;
URB gUrbRead;

URB_MEMORY gMem;

COMM_DEVICE_INFO gDevice;
READ_BUFFER gRead;

