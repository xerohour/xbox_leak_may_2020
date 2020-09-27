/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    RootHub.c

Abstract:
    
    This file implements the OpenHCI root hub support.  Rather than following the model
    of writing an emulation of a real hub that supports transfers, this module instead
    acts as a driver for the root-hub.  It detects and reports devices just as a hub driver
    would.  Since hub drivers do not in the XBOX world expose external interfaces, it
    is much simpler this way.

    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    02-10-00 created by Mitchell Dernis (mitchd)

--*/
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


__inline UCHAR RootHubPortToPortNumber(UCHAR RootHubPort)
/*++
    Routine Description:
        The root hub internally uses ports 0 to 3.  However, this
        translates to ports 1 to 4 with a normal hub. 
--*/
{
    return RootHubPort+1;
}

__inline ULONG PortNumberToRootHubPort(ULONG PortNumber)
{
    return PortNumber-1;
}

//
//  Pull in usb headers
//
#include <hcdi.h>       //Interface between USBD and HCD
#include "ohcd.h"       //Private OHCD stuff


VOID
OHCD_RootHubResetDpc(
    PKDPC Dpc,
    PVOID DeviceExtensionPtr,
    PVOID Bogus1,
    PVOID Bogus2
    );

#pragma code_seg(".XPPCINIT")
VOID
FASTCALL
OHCD_RootHubInitialize(
    POHCD_DEVICE_EXTENSION      DeviceExtension
)
/*++

Routine Description:

    Reads basic info regarding the root-hub, and turns
    on the interrupts for the root-hub.

    1) Determine how many ports we have.
    2) Process initial connections.
    3) Turn on interrupts for hot-plug detection later

Arguments:

Return Value:
    
--*/               
{
    POHCI_OPERATIONAL_REGISTERS operationalRegisters = DeviceExtension->OperationalRegisters;
    POHCD_ROOT_HUB_OBJECT       rhObject = &DeviceExtension->RootHubObject;
#ifndef OHCD_XBOX_HARDWARE_ONLY
    HC_RH_DESCRIPTOR_A          rhDescriptorA;
    HC_RH_DESCRIPTOR_B          rhDescriptorB;
#endif //OHCD_XBOX_HARDWARE_ONLY    
    HC_RH_STATUS                rhStatus;
    HC_RH_PORT_STATUS           rhPortStatus;
    ULONG                       index;
    ULONG                       mask;
    OHCD_PORT_INFO              portInfo;
    KIRQL                       oldIrql;
    
    USB_DBG_ENTRY_PRINT(("Entering OHCD_RootHubInitialize"));
    
    
    //
    //  Initialize the watchdog timer and related DPC,
    //  for catching failed resets.
    //
    KeInitializeTimer(&rhObject->ResetTimeoutTimer);
    KeInitializeDpc(
        &rhObject->ResetTimeoutDPC,
        OHCD_RootHubResetDpc,
        (PVOID)DeviceExtension
        );


#ifdef OHCD_XBOX_HARDWARE_ONLY
    #if (USB_HOST_CONTROLLER_CONFIGURATION==USB_SINGLE_HOST_CONTROLLER)
    rhObject->NumberOfPorts = 4;
    #else
    rhObject->NumberOfPorts = 2;
    #endif
    USB_DBG_TRACE_PRINT(("  NumberDownstreamPorts     : %d", (ULONG)rhObject->NumberOfPorts));
#else
    //
    //  Get Information on root-hub, store number of ports.
    //
    rhDescriptorA.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorA.ul);
    rhObject->NumberOfPorts = rhDescriptorA.NumberDownstreamPorts;
    if(rhDescriptorA.NoPowerSwitch)
    {
        USB_DBG_WARN_PRINT(("USB Card Doesn't support power switching!!"));
    } else
    {
    
        //
        //  Calculate the relative PowerOnToGoodTime in 100 ns.  The negative sign is
        //  so the KeDelayExecutionThread will treat it as a relative time.
        //
        rhObject->PowerOnToGoodTime.QuadPart = -rhDescriptorA.PowerOnToPowerGoodTime*2*10000;
    
        //
        //  Build changes to write back to descriptor A
        //
        if(!rhDescriptorA.PowerSwitchingMode || !rhDescriptorA.OverCurrentProtectionMode)
        {
            USB_DBG_TRACE_PRINT(("Switch to per-port power and overcurrent protection:"));
            rhDescriptorA.PowerSwitchingMode = 1;   //  Switch the PowerSwitchingMode to Per-port
            rhDescriptorA.OverCurrentProtectionMode = 1; // Per port overcurrent protection
            WRITE_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorA.ul, rhDescriptorA.ul);
        }

        //
        //  As a diagnostic make sure that our changes were accepted
        //  we only support per-port power.
        //
        #if DBG
        rhDescriptorA.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorA.ul);
        if(!rhDescriptorA.PowerSwitchingMode || !rhDescriptorA.OverCurrentProtectionMode)
        {
            USB_DBG_ERROR_PRINT(("Root Hub didn't accept ganged -> per-port changes.  Incompatible hardware!"));
        }
        #endif
    }    


#if DBG
    if(0xFF==rhDescriptorA.PowerOnToPowerGoodTime)
    {
        //
        //  In truth, there may be other cards that require this long delay.
        //  The Entrega with the CMD chip is just the one we are worried about.
        //
        USB_DBG_WARN_PRINT(("CMD USB CARD DETECTED(requires long power-on delay)!"));
    }
#endif 

    USB_DBG_TRACE_PRINT(("Root Hub's Descriptor A:"));
    USB_DBG_TRACE_PRINT(("  NumberDownstreamPorts     : %d", (ULONG)rhObject->NumberOfPorts));
    USB_DBG_TRACE_PRINT(("  NoPowerSwitching          : %s", rhDescriptorA.NoPowerSwitch ? "TRUE" : "FALSE"));
    USB_DBG_TRACE_PRINT(("  PowerSwitchingMode        : %s", rhDescriptorA.PowerSwitchingMode ? "Per-Port" : "Ganged"));
    USB_DBG_TRACE_PRINT(("  DeviceType                : %s", rhDescriptorA.DeviceType ? "Compound" : "Simple"));
    USB_DBG_TRACE_PRINT(("  OverCurrentProtectionMode : %s", rhDescriptorA.OverCurrentProtectionMode ? "Per-Port" : "Overall"));
    USB_DBG_TRACE_PRINT(("  NoOverCurrentProtection   : %s", rhDescriptorA.NoOverCurrentProtection ? "TRUE" : "FALSE"));
    USB_DBG_TRACE_PRINT(("  PowerOnToPowerGoodTime    : %d ms", (ULONG)rhDescriptorA.PowerOnToPowerGoodTime*2));
#endif //OHCD_XBOX_HARDWARE_ONLY   
    
    //
    //  Display info in Descriptor B
    //
#ifndef OHCD_XBOX_HARDWARE_ONLY
    rhDescriptorB.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorB.ul);
    if(!rhDescriptorA.NoPowerSwitch)
    {
        //
        //  Make sure the PortPowerControlMask is set so that every port is per-port power.
        //
        mask = (rhObject->NumberOfPorts << 2) - 2;
        if(mask != (rhDescriptorB.PortPowerControlMask & mask))
        {
            rhDescriptorB.PortPowerControlMask |= mask;
            WRITE_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorB.ul, rhDescriptorB.ul);
            //
            //  As a diagnostic read back the changes, and make sure that they took.
            //
            #if DBG
            rhDescriptorB.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhDescriptorB.ul);
            if(mask != (rhDescriptorB.PortPowerControlMask & mask))
            {
                USB_DBG_ERROR_PRINT(("Root Hub didn't accept PortPowerControlMask changes.  Incompatible hardware!"));
            }
            #endif
        }

        #if DBG 
        USB_DBG_TRACE_PRINT(("Root Hub's Descriptor B:"));
        for(index = 1, mask = 2; index <= (int)rhObject->NumberOfPorts; index++, mask <<= 1)
        {
            USB_DBG_TRACE_PRINT(("  DeviceRemovable(%d)      : %s", index, 
                ((ULONG)rhDescriptorB.DeviceRemovable & mask)? "Not Removable" : "Removable" ));
            USB_DBG_TRACE_PRINT(("  PortPowerControlMask(%d) : %s", index, 
                ((ULONG)rhDescriptorB.PortPowerControlMask & mask)? "Per-port" : "Ganged" ));   
        }
        #endif

        //
        //  Power on ports
        //
        /*  Not necessary, everything is per-port.
        ASSERT(!rhDescriptorA.NoPowerSwitch); //We don't support cards that are always powered.
        rhStatus.HubStatus = HC_RH_STATUS_DeviceRemoteWakeupEnable; - we don't support remote wakeup.
        rhStatus.HubStatus = 0;
        rhStatus.HubStatusChange = HC_RH_STATUS_LocalPower;
        WRITE_REGISTER_ULONG(&operationalRegisters->HcRhStatus.ul, rhStatus.ul);
        */
    }
#endif //OHCD_XBOX_HARDWARE_ONLY
    //
    //  Get the hub status
    //
    rhStatus.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhStatus.ul);
    USB_DBG_TRACE_PRINT(("Root Hub's Status:"));
    USB_DBG_TRACE_PRINT(("  LocalPowerStatus          : Not Readable"));
    USB_DBG_TRACE_PRINT(("  OverCurrentIndicator      : %s", 
                (rhStatus.HubStatus &  HC_RH_STATUS_OvercurrentIndicator )? "OverCurrent" : "OK"));
    USB_DBG_TRACE_PRINT(("  DeviceRemoteWakeupEnable  : %s",
                (rhStatus.HubStatus & HC_RH_STATUS_DeviceRemoteWakeupEnable) ? "ON" : "OFF"));
    //
    //  Clear the status change bits (so that we can get future notifications)
    //  You clear bits by writing a one to them.  Writing a one to the HubStatus
    //  actually changes settings which we don't want to.  So basically
    //  we clear the HubStatus back and write the status change bits back on themselves.
    //
    rhStatus.HubStatus = 0;
    WRITE_REGISTER_ULONG(&operationalRegisters->HcRhStatus.ul, rhStatus.ul);

#ifndef OHCD_XBOX_HARDWARE_ONLY    
    //
    //  Power on each port
    //
    if(!rhDescriptorA.NoPowerSwitch)
    {
        // 7 ms delay hack for some timing challenged cards
        LARGE_INTEGER   prePowerDelayHack;
        prePowerDelayHack.QuadPart = -7*10000;
        KeDelayExecutionThread(KernelMode, FALSE, &prePowerDelayHack);

        rhPortStatus.PortStatus = HC_RH_PORT_STATUS_SetPortPower;
        rhPortStatus.PortStatusChange = 0x0000;
        for(index=0; index < (int)rhObject->NumberOfPorts; index++)
        {
            WRITE_REGISTER_ULONG(&operationalRegisters->HcRhPortStatus[index].ul, rhPortStatus.ul);
        }
        //
        //  Wait the PowerOnToGoodTime
        //
        KeDelayExecutionThread(KernelMode, FALSE, &rhObject->PowerOnToGoodTime);
    }    
#endif //OHCD_XBOX_HARDWARE_ONLY

    //
    //  Retrieve the status of each port
    //
    RtlZeroMemory((PVOID)&portInfo, sizeof(OHCD_PORT_INFO));
    for(index=0, mask = 1; index < (int)rhObject->NumberOfPorts; index++, mask <<=1)
    {
        rhPortStatus.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhPortStatus[index].ul);
    
        //
        //  Traceouts, so we can see what is happening
        //
        USB_DBG_TRACE_PRINT(("Status of Port %d:", index+1));
        USB_DBG_TRACE_PRINT(("  CurrentConnectStatus      : %s",  
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_CurrentConnectStatus) ? "Connected" : "Disconnected"));
        USB_DBG_TRACE_PRINT(("  PortEnableStatus          : %s",
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_PortEnableStatus) ? "Enabled" : "Disabled"));
        USB_DBG_TRACE_PRINT(("  PortSuspendStatus         : %s",
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_PortSuspendStatus) ? "Suspended" : "Not Suspended"));
        USB_DBG_TRACE_PRINT(("  PortOverCurrentIndicator  : %s",
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_PortOverCurrentIndicator) ? "OverCurrent" : "OK"));
        USB_DBG_TRACE_PRINT(("  PortResetStatus           : %s",
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_PortResetStatus) ? "Reset Signal Active" : "Reset Signal Not Active"));
        USB_DBG_TRACE_PRINT(("  PortPowerStatus           : %s",
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_PortPowerStatus) ? "Power On" : "Power Off"));
        USB_DBG_TRACE_PRINT(("  LowSpeedDeviceAttached    : %s\n",
            (rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_LowSpeedDeviceAttach) ? "LowSpeed" : "FullSpeed"));

        //
        //  We are looking just for connected status.
        //
        if(rhPortStatus.PortStatus  & HC_RH_PORT_STATUS_CurrentConnectStatus)
        {
            portInfo.PortsConnected |= mask;
            portInfo.PortsConnectionChanged |= mask;
        }

        //
        //  Clear the status change bits
        //
        rhPortStatus.PortStatus = 0;
        WRITE_REGISTER_ULONG(&operationalRegisters->HcRhPortStatus[index].ul, rhPortStatus.ul);
    }

    //
    //  Start interrupt processing for root-hub.
    //
    WRITE_REGISTER_ULONG(&operationalRegisters->HcInterruptEnable, HCINT_RootHubStatusChange);

    //
    //  Now we will call a common routine to process any devices we found.
    //  Since it is also called from the DPC for the ISR it expects to be 
    //  DISPATCH_LEVEL.
    //
    oldIrql = KeRaiseIrqlToDpcLevel();
    OHCD_RootHubProcessHotPlug(DeviceExtension, &portInfo);
    KeLowerIrql(oldIrql);

    USB_DBG_EXIT_PRINT(("Exiting OHCD_RootHubInitialize"));
}

#pragma code_seg(".XPPCODE")

VOID
FASTCALL
OHCD_RootHubProcessInterrupt(
    POHCD_DEVICE_EXTENSION      DeviceExtension
)
/*++

Routine Description:

    Process interrupts destined for the Root Hub.
    This routine is called LEVEL_DISPATCH from within
    the IsrDPC.
    
Arguments:

Return Value:
    
--*/          
{
    POHCI_OPERATIONAL_REGISTERS operationalRegisters = DeviceExtension->OperationalRegisters;
    HC_RH_STATUS                rhStatus;
    HC_RH_PORT_STATUS           rhPortStatus;
    ULONG                       index;
    ULONG                       mask;
    OHCD_PORT_INFO              portInfo;
    ULONG                       numPorts = DeviceExtension->RootHubObject.NumberOfPorts;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_RootHubProcessInterrupt"));

    //
    //  Check to see if we have an overcurrent on the hub.
    //  This is the only global (i.e. NOT per port) cause of a root hub interrupt.
    //
    rhStatus.ul = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcRhPortStatus->ul);
    if(rhStatus.HubStatusChange & HC_RH_STATUS_OvercurrentIndicator)
    {
        USB_DBG_TRACE_PRINT(("The root-hub is indicating over-current!"));
    }
    //
    //  Now retrieve the status of each port
    //
    RtlZeroMemory((PVOID)&portInfo, sizeof(OHCD_PORT_INFO));
    for(index=0, mask = 1; index < numPorts; index++, mask <<=1)
    {
        rhPortStatus.ul = READ_REGISTER_ULONG(&operationalRegisters->HcRhPortStatus[index].ul);
        //
        //  If reset status change
        //
        if(rhPortStatus.PortStatusChange & HC_RH_PORT_STATUS_PortResetStatus)
        {
            
            PFNHCD_RESET_COMPLETE   CompleteProc = DeviceExtension->RootHubObject.ResetComplete;
            PVOID                   Context = DeviceExtension->RootHubObject.ResetContext;
            
            //
            //  If the complete proc has not been called (due to a timeout)
            //  then stop the watchdog and call it now.
            //
            if(CompleteProc)
            {
                
                //
                //  Stop the reset's watchdog timer
                //
                KeCancelTimer(&DeviceExtension->RootHubObject.ResetTimeoutTimer);
                USB_DBG_TRACE_PRINT(("ResetCompleted: Port = %d, Context = 0X%0.8x", index+1, Context));

                //
                //  Call the completion routine  (setting the status according to 
                //  low speed or not).
                //
                DeviceExtension->RootHubObject.ResetComplete = NULL;
                DeviceExtension->RootHubObject.ResetContext = 0;
                CompleteProc(
                    (rhPortStatus.PortStatus & HC_RH_PORT_STATUS_LowSpeedDeviceAttach) ? 
                    USBD_STATUS_LOWSPEED : USBD_STATUS_SUCCESS,
                    Context);
            }
        }

        //
        //  If the connect status changed, we need to process it.
        //
        if(rhPortStatus.PortStatusChange & HC_RH_PORT_STATUS_CurrentConnectStatus)
        {
            //
            //  Set the connect change bit.
            //
            portInfo.PortsConnectionChanged |= mask;
            //
            //  If connected, set the connect bit.
            //
            if(rhPortStatus.PortStatus & HC_RH_PORT_STATUS_CurrentConnectStatus)
            {
                portInfo.PortsConnected |= mask;
            }
        }
        //
        //  Check for overcurrent indicators
        //
        if(rhPortStatus.PortStatusChange & HC_RH_PORT_STATUS_PortOverCurrentIndicator)
        {
            //
            //  It appears that this can just be ignored, other things will happen (like the
            //  port being disabled) that we will deal with properly.
            //
            USB_DBG_WARN_PRINT(("Port %d of the root hub is indicating over-current!", index));
        }
        //
        //  Check for automatic disabling of a port
        //
        if(rhPortStatus.PortStatusChange & HC_RH_PORT_STATUS_PortEnableStatus)
        {
            USB_DBG_TRACE_PRINT(("The root-hub disabled port %d!  Usually a remove. Usually it was removed.", index));
        }

        //
        //  Clear the status change bits, so we can get notifications of further changes
        //
        rhPortStatus.PortStatus = 0;
        WRITE_REGISTER_ULONG(&operationalRegisters->HcRhPortStatus[index].ul, rhPortStatus.ul);
    }

    //
    //  Notify USBD about hot-plug events.
    //
    OHCD_RootHubProcessHotPlug(DeviceExtension, &portInfo);

    USB_DBG_EXIT_PRINT(("Exiting OHCD_RootHubProcessInterrupt"));
}


VOID
FASTCALL
OHCD_RootHubProcessHotPlug(
    IN POHCD_DEVICE_EXTENSION   DeviceExtension,
    IN POHCD_PORT_INFO          PortInfo
)
/*++

Routine Description:

    Does the work of reporting connect and disconnect
    events to USBD and the higher driver layers.  It is
    called from two different contexts.

    1) Just a straight call from OHCD_RootHubInitialize
        to process the connected devices found initially.
    2) As a worker thread, from a work item queued by OHCD_RootProcessInterrupt
        which runs in the context of IsrDPC.

    This routine needs to operate at LEVEL_PASSIVE, because the higher level need to synchronously
    process each detected device, which may be time consuming.  The reason behind this is that a newly
    detected device reports at the default USB address (0).  Only one device reporting at address zero
    should be enabled at a time.  Once USBD sets the address, we can then move on to enabling the next
    device.

Arguments:

Return Value:
    
--*/          
{
    UCHAR portIndex = 0;
    UCHAR portMask = 1;
    USB_DBG_ENTRY_PRINT(("Entering OHCD_RootHubProcessHotPlug"));

    ASSERT_DISPATCH_LEVEL();

    //
    //  Keep walking through port indices as long as there is
    //  a change we haven't processed yet.
    //
    while(portIndex < DeviceExtension->RootHubObject.NumberOfPorts)
    {
        //
        //  If this port has change, notify USBD of the change.
        //
        if(PortInfo->PortsConnectionChanged&portMask)
        {
            //
            //  Handle new connect
            //
            if(PortInfo->PortsConnected&portMask)
            {
                //
                //  Check for rapid fire remove\add, in which
                //  case do a remove before the add.
                //
                if(DeviceExtension->RootHubObject.DeviceDetectedBitmap&portMask)
                {
                    USBD_DeviceDisconnected(
                        (PVOID)DeviceExtension,
                        RootHubPortToPortNumber(portIndex)
                        );
                    USB_DBG_WARN_PRINT(("Remove and Add in one interrupt."));
                } else
                {
                    DeviceExtension->RootHubObject.DeviceDetectedBitmap |= portMask; //record that this
                                                                                     //port has a device.
                }
                //
                //  Report the device to USBD.  That is all we must do.
                //  (USBD expects a base of 1 for port numbers)
                USBD_DeviceConnected( (PVOID)DeviceExtension, RootHubPortToPortNumber(portIndex));
            }
            else
            //
            //  Handle Disconnect
            //
            {
                //
                //  Check for rapid fire add\remove, only call device connected
                //  if we called device connected at some point.
                //
                if(DeviceExtension->RootHubObject.DeviceDetectedBitmap&portMask)
                {
                    //
                    //  Tell USBD, which will synchronously cleanup
                    //
                    //  (USBD expects a base of 1 for port numbers)
                    DeviceExtension->RootHubObject.DeviceDetectedBitmap &= ~portMask;
                    USBD_DeviceDisconnected(
                        (PVOID)DeviceExtension,
                        RootHubPortToPortNumber(portIndex)
                        );
                } else
                {
                    USB_DBG_TRACE_PRINT(("Add and Remove in one interrupt."));
                }
            }
        }
        //
        //  Move on to next port
        //
        portIndex++;
        portMask <<= 1;
    }
    
    USB_DBG_EXIT_PRINT(("Exiting OHCD_RootHubProcessHotPlug"));
}


VOID
HCD_ResetRootHubPort(
    IN PVOID                    HcdDeviceExtension,
    IN ULONG                    PortNumber,
    IN PFNHCD_RESET_COMPLETE    ResetCompleteProc,
    IN PVOID                    CompleteContext
    )
/*++

Routine Description:

    Resets a port on the root hub.  Only one reset is allowed at a time.
    
Arguments:
    HcdDeviceExtension  - our device extension (must cast)
    PortNumber          - number of port to reset (0 based)
Return Value:
    
--*/
{
    POHCD_DEVICE_EXTENSION  deviceExtension = (POHCD_DEVICE_EXTENSION)HcdDeviceExtension;
    HC_RH_PORT_STATUS       rhPortStatus;
    LARGE_INTEGER           resetTimeOut;

    USB_DBG_TRACE_PRINT(("HCD_ResetRootHubPort: Port = %d, Context = 0X%0.8x", PortNumber, CompleteContext));
    //
    //  Convert port number to root hub port number
    //
    PortNumber=PortNumberToRootHubPort(PortNumber);

    //
    //  Validate Range
    //
    ASSERT(deviceExtension->RootHubObject.NumberOfPorts > PortNumber);
    
    //
    //  There should not be a reset currently pending.
    //
    ASSERT(NULL == deviceExtension->RootHubObject.ResetComplete);
    
    //
    //  Record the completion information.
    //
    deviceExtension->RootHubObject.ResetComplete = ResetCompleteProc;
    deviceExtension->RootHubObject.ResetContext = CompleteContext;
    
    //
    //  Reset the port
    //
    rhPortStatus.PortStatus = HC_RH_PORT_STATUS_SetPortReset;
    WRITE_REGISTER_ULONG(
        &deviceExtension->OperationalRegisters->HcRhPortStatus[PortNumber].ul,
        rhPortStatus.ul
        );

    //
    //  Set a watchdog timer.  Resets have been known to fail.
    //
    resetTimeOut.QuadPart = -10000 * 100; //allow 100 ms, it should take only 10 ms according to spec.
                                           //20 ms wasn't long enough for some cards.
    KeSetTimer(
        &deviceExtension->RootHubObject.ResetTimeoutTimer,
        resetTimeOut,
        &deviceExtension->RootHubObject.ResetTimeoutDPC
        );
}

VOID
HCD_DisableRootHubPort(
    IN PVOID                    HcdExtension,
    IN ULONG                    PortNumber
    )
{
    POHCD_DEVICE_EXTENSION deviceExtension = (POHCD_DEVICE_EXTENSION)HcdExtension;
    HC_RH_PORT_STATUS       rhPortStatus;
    //
    //  Convert port number to root hub port number
    //
    PortNumber=PortNumberToRootHubPort(PortNumber);
    //
    //  Validate Range
    //
    ASSERT(deviceExtension->RootHubObject.NumberOfPorts > PortNumber);
    //
    //  Disable the port
    //
    rhPortStatus.PortStatus = HC_RH_PORT_STATUS_ClearPortEnable;
    WRITE_REGISTER_ULONG(
        &deviceExtension->OperationalRegisters->HcRhPortStatus[PortNumber].ul,
        rhPortStatus.ul
        );
}


VOID
OHCD_RootHubResetDpc(
    PKDPC Dpc,
    PVOID DeviceExtensionPtr,
    PVOID Bogus1,
    PVOID Bogus2
    )
{
    POHCD_DEVICE_EXTENSION deviceExtension = (POHCD_DEVICE_EXTENSION) DeviceExtensionPtr;
    PFNHCD_RESET_COMPLETE   CompleteProc = deviceExtension->RootHubObject.ResetComplete;
    PVOID                   Context = deviceExtension->RootHubObject.ResetContext;

    if(CompleteProc)
    {
        //
        //  This is usually nothing, but a device that was removed during the reset.
        //
        //  It has been found in the past to be indicative of other serious problems, though.
        //  Like the host being locked up because of bad register values.
        //
        USB_DBG_WARN_PRINT(("A port reset timed out on the root hub!"));

        //
        //  Call the completion routine with the bad news,
        //  it is up to the caller to recover.
        //
        USB_DBG_WARN_PRINT(("Reset Timed Out: Context = 0X%0.8x",  Context));
        deviceExtension->RootHubObject.ResetComplete = NULL;
        deviceExtension->RootHubObject.ResetContext = NULL;
        CompleteProc(USBD_STATUS_REQUEST_FAILED, Context);
    }
}

