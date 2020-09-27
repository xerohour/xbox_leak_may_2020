/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    devsys.c

Abstract:
    
    Implementation of code that is conditionally compiled to perform
    operation on development systems (i.e. non-final development system
    hardware.)  This code may also be useful for verifying final hardware.
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-21-00 created by Mitchell Dernis (mitchd)

--*/

//
//  This file only compiles if development system
//  operations are enabled.
#ifdef PERFORM_DEVSYS_OPERATIONS

//
//  Pull in OS headers
//
#include <ntos.h>

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


#pragma code_seg(".XPPCINIT")

VOID
OHCD_DevSysCheckHardware(
    IN POHCD_DEVICE_EXTENSION DeviceExtension
    )


{
    ULONG Revision;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_DevSysCheckHardware"));
    //
    //  Read the OHCI version
    //
    Revision = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcRevision);
    USB_DBG_TRACE_PRINT(("OpenHCI controller version = %d.%d", ((0xF0 & Revision) >> 4), (0x0F & Revision)));
    
    //
    //  We only support version 1.0 of OpenHCI
    //
    ASSERT( (0x10 == (0xFF & Revision)) && "We only support version 1.0 of OpenHCI");
    USB_DBG_EXIT_PRINT(("Exiting OHCD_DevSysCheckHardware"));
}


VOID
OHCD_DevSysTakeControl(
    IN POHCD_DEVICE_EXTENSION DeviceExtension
    )
{
    LARGE_INTEGER DelayTime;
    HC_CONTROL_REGISTER HcControl;
    HC_COMMAND_STATUS   HcCommandStatus;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_DevSysTakeControl"));
    //
    //  Read control register to determine if we have an SMM or BIOS driver
    //  to steel control from
    HcControl.ul = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcControl.ul);
    
    //
    //  Check for SMM driver (spec. says to check InterruptRouting Bit)
    //
    if(HcControl.InterruptRouting)
    {
        USB_DBG_TRACE_PRINT(("Placing OwnerShipChangeRequest to get control from SMM driver."));
        //
        //  Put in an OwnerShipChangeRequest
        //
        HcCommandStatus.ul = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcCommandStatus.ul);
        HcCommandStatus.OwnerShipChangeRequest = 1;
        WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcCommandStatus.ul, HcCommandStatus.ul);

        //
        //  Wait for SMM driver to clear the interrupt routing bit
        //
        do
        {
            HcControl.ul = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcControl.ul);
        }while(HcControl.InterruptRouting);
        USB_DBG_TRACE_PRINT(("SMM driver has yielded control."));
    }
    //
    //  If not SMM driver check for BIOS driver
    //
    else if(HC_OPSTATE_USB_RESET != HcControl.HostControllerFunctionalState )
    {
        USB_DBG_TRACE_PRINT(("A BIOS driver was present, set to UsbResume to get control."));
        //
        //  If the state is not operational set it to resume and wait
        //
        if(HC_OPSTATE_USB_OPERATIONAL != HcControl.HostControllerFunctionalState)
        {
            HcControl.HostControllerFunctionalState = HC_OPSTATE_USB_RESUME;
            //
            //  Signal RESUME and WAIT 20 ms.
            //
            WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcControl.ul, HcControl.ul);
            DelayTime.QuadPart = -200000;
            KeDelayExecutionThread( KernelMode, FALSE, &DelayTime);
        }
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_DevSysTakeControl"));
}

#endif //PERFORM_DEVSYS_OPERATIONS
