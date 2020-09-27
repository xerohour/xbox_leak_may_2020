/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    UHCD.c

Abstract:

    WinDbg Extension Api

Author:

    Kenneth D. Ray (kenray) June 1997

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#define _WDMDDK_
#include <pci.h>
#include <usbdi.h>
#include <hcdi.h>
#if DBG
   #include ".\usb\hcd\uhcd\uhcd.h"
#else
   #undef DBG
   #define DBG 1
   #include ".\usb\hcd\uhcd\uhcd.h"
   #undef DBG
#endif

typedef union _USB_FLAGS {
    struct {
        ULONG   FullListing         : 1;
        ULONG   Reserved            : 31;
    };
    ULONG Flags;
} USB_FLAGS;

#define PRINT_FLAGS(value, flag) \
    if ((value) & (flag)) { \
        dprintf (#flag " "); \
    }

VOID
UhciPortRegister(
    ULONG   PortNumber,
    ULONG   Value
);

VOID
DevExtUHCD(
    PVOID MemLocPtr
    )
{
    ULONG             MemLoc = (ULONG) MemLocPtr; // BUGBUG ??
    ULONG             result;
    ULONG             i;
    USBD_EXTENSION    usbd;
    DEVICE_EXTENSION  uhcd;

    dprintf ("Dump UHCD Extension: %x\n", MemLoc);

    if (!ReadMemory (MemLoc, &usbd, sizeof (usbd), &result)) {
        dprintf ("Could not read Usbd Extension\n");
        return;
    }

    if (NULL != usbd.TrueDeviceExtension) {
        MemLoc = (ULONG) usbd.TrueDeviceExtension;
    }

    if (!ReadMemory (MemLoc, &uhcd, sizeof (uhcd), &result)) {
        dprintf ("Could not read UHCD Extension\n");
        return;
    }

    dprintf("\n");
    dprintf("PhysicalDO:   %8x  TopOfStackDO: %8x  InterruptObject: %8x\n"
            "FrameListVA:  %8x  FrameListLA:  %8x  ",
            uhcd.PhysicalDeviceObject,
            uhcd.TopOfStackDeviceObject,
            uhcd.InterruptObject,
            uhcd.FrameListVirtualAddress,
            uhcd.FrameListLogicalAddress);

    dprintf("FrameListCopyVA: %08x\n",
             uhcd.FrameListCopyVirtualAddress);

    dprintf("\n");
    dprintf("PersistantQH: %8x  PQH_DescriptorList: %8x\n"
            "EndpointList          & %x = (%x %x) \n"
            "EndpointLookAsideList & %x = (%x %x) \n"
            "ClosedEndpointList    & %x = (%x %x) \n",
            uhcd.PersistantQueueHead,
            uhcd.PQH_DescriptorList,
            &(uhcd.EndpointList),
            uhcd.EndpointList.Flink,
            uhcd.EndpointList.Blink,
            &(uhcd.EndpointLookAsideList),
            uhcd.EndpointLookAsideList.Flink,
            uhcd.EndpointLookAsideList.Blink,
            &(uhcd.ClosedEndpointList),
            uhcd.ClosedEndpointList.Flink,
            uhcd.ClosedEndpointList.Blink);

    dprintf("\n");
    dprintf("InterruptSchedule: ");

    for (i = 0; i < MAX_INTERVAL; i++) 
    {
        dprintf("%8x  ", uhcd.InterruptSchedule[i]);

        if (3 == i % 4) 
        {
            dprintf("\n");
            dprintf("                   ");
        }
    }

    dprintf("\n");
    dprintf("PageList & %x = (%x %x) \n",
            &( uhcd.PageList),
            uhcd.PageList.Flink,
            uhcd.PageList.Blink);

    dprintf("\n");
    dprintf("BwTable: ");

    for (i = 0; i < MAX_INTERVAL; i++) 
    {
   
        dprintf("%5d   ", uhcd.BwTable[i]);

        if (7 == i % 8) 
        {
            dprintf("\n");
            dprintf("         ");
        }
    }

    dprintf("\n");
    dprintf("LastFrame:             %8x  FrameHighPart:       %8x\n"
            "LastIdleTime.High:     %8x  LastIdleTime.Low     %8x\n",
            "LastXferIdleTime.High: %8x  LastXferIdleTime.Low %8x\n",
            "IdleTime:              %8x  XferIdleTime         %8x\n",
            uhcd.LastFrame,
            uhcd.FrameHighPart,
            uhcd.LastIdleTime.HighPart,
            uhcd.LastIdleTime.LowPart,
            uhcd.LastXferIdleTime.HighPart,
            uhcd.LastXferIdleTime.LowPart,
            uhcd.IdleTime,
            uhcd.XferIdleTime);

    dprintf("\n");
    dprintf("TriggerList:      %08x\n"
            "LargeBufferPool:  %08x\n"
            "MediumBufferPool: %08x\n"
            "SmallBufferPool:  %08x\n",
            &(uhcd.TriggerTDList),
            & (uhcd.LargeBufferPool), 
            & (uhcd.MediumBufferPool), 
            & (uhcd.SmallBufferPool));

    dprintf("\nRootHub Variables\n");
    dprintf("DeviceAddress: %3d  RootHub:           %8x\n"  
            "TimersActive:  %3d  InterruptEndpoint: %8x\n",
            uhcd.RootHubDeviceAddress,
            uhcd.RootHub,
            uhcd.RootHubTimersActive,
            uhcd.RootHubInterruptEndpoint);

    dprintf("\n");
    dprintf("LastFrameProcessed:   %x\n"
            "AdapterObject:        %8x\n"
            "MapRegisters:         %d\n"
            "DeviceNameHandle:     %x\n"
            "FrameBabbleRecoverTD: %8x\n",
            uhcd.LastFrameProcessed,
            uhcd.AdapterObject,
            uhcd.NumberOfMapRegisters,
            uhcd.DeviceNameHandle,
            uhcd.FrameBabbleRecoverTD);

    dprintf("\nSaved Bios Info\n");
    dprintf("Cmd:             %x  IntMask:              %x\n"
            "FrameListBase:   %x  LegacySuppReg:        %x\n"
            "DeviceRegisters: %x  SavedInterruptEnable: %x\n"
            "SavedCommandReg: %x\n",
            uhcd.BiosCmd,
            uhcd.BiosIntMask,
            uhcd.BiosFrameListBase,
            uhcd.LegacySupportRegister,
            uhcd.DeviceRegisters[0],
            uhcd.SavedInterruptEnable,
            uhcd.SavedCommandReg);

    dprintf("\n");
    dprintf("PowerState: %x\n"
            "HcFlags %x: ",
            uhcd.CurrentDevicePowerState,
            uhcd.HcFlags);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_GOT_IO);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_UNMAP_REGISTERS);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_USBBIOS);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_BWRECLIMATION_ENABLED);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_NEED_CLEANUP);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_IDLE);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_ROLLOVER_IDLE);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_HCD_STOPPED);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_DISABLE_IDLE);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_WORK_ITEM_QUEUED);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_HCD_SHUTDOWN);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_LOST_POWER);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_RH_OFF);
            PRINT_FLAGS(uhcd.HcFlags, HCFLAG_MAP_SX_TO_D3);
    dprintf("\n");

    dprintf("\n");
    dprintf("SavedFrameNumber:     %8x  SavedFRBaseAdd: %8x\n"
            "Port:                 %8x  HcDma:          %8x\n"
            "RegRecClocksPerFrame: %8x  Piix4EP         %8x\n"
            "EndpointListBusy:     %8d  SteppingVer:    %8x\n"
            "SavedSOFModify:       %8x  ControllerType: %8x\n",
            uhcd.SavedFRNUM,
            uhcd.SavedFRBASEADD,
            uhcd.Port,
            uhcd.HcDma,
            uhcd.RegRecClocksPerFrame,
            uhcd.Piix4EP,
            uhcd.EndpointListBusy,
            uhcd.SteppingVersion,
            uhcd.SavedSofModify,
            uhcd.ControllerType);

    return;
}

VOID
UHCD_HCRegisters(
    ULONG   MemLoc
)
{
    PDEVICE_OBJECT      hcObject;
    USBD_EXTENSION      usbd;
    ULONG               devExtAddr;
    DEVICE_EXTENSION    uhcd;

    ULONG               result;
    ULONG               regValue;
    ULONG               size;

    //
    // In this case, MemLoc points the the device object for the given
    //   host controller.
    //

    hcObject = (PDEVICE_OBJECT) MemLoc;

    //
    // Get the address of the device extension
    //

    devExtAddr = (ULONG) hcObject -> DeviceExtension;

    //
    // Read the USBD extension
    //

    if (!ReadMemory (devExtAddr, &usbd, sizeof (usbd), &result)) {
        dprintf ("Could not read Usbd Extension\n");
        return;
    }

    if (NULL != usbd.TrueDeviceExtension) {
        devExtAddr = (ULONG) usbd.TrueDeviceExtension;
    }

    if (!ReadMemory (devExtAddr, &uhcd, sizeof (uhcd), &result)) {
        dprintf ("Could not read UHCD Extension\n");
        return;
    }

    //
    // Get and display the command register (USBCMD)
    //

    size = 2;
    ReadIoSpace((ULONG) COMMAND_REG((&uhcd)), &regValue, &size);

    dprintf("\n");
    dprintf("Command Register: Run/Stop:       %x  HC reset:      %x  Global reset: %x\n"
            "                  Global Suspend: %x  Global Resume: %x  SW Debug:     %x\n"
            "                  Configure Flag: %x  Max Packet:    %x\n",
                               regValue & UHCD_CMD_RUN,
                               regValue & UHCD_CMD_RESET,
                               regValue & UHCD_CMD_GLOBAL_RESET,
                               regValue & UHCD_CMD_SUSPEND,
                               regValue & UHCD_CMD_FORCE_RESUME,
                               regValue & UHCD_CMD_SW_DEBUG,
                               regValue & UHCD_CMD_SW_CONFIGURED,
                               regValue & UHCD_CMD_MAXPKT_64);

    //
    // Get and display the status register (USBSTS)
    //

    ReadIoSpace((ULONG) STATUS_REG((&uhcd)), &regValue, &size);

    dprintf("\n");
    dprintf("Status Register:  Transfer Int: %x  Error Int: %x  Resume Detect: %x\n",
            "                  Host Error:   %x  HC Error:  %x  HC Halted: %x\n",
            regValue & UHCD_STATUS_USBINT,
            regValue & UHCD_STATUS_USBERR,
            regValue & UHCD_STATUS_RESUME,
            regValue & UHCD_STATUS_PCIERR,
            regValue & UHCD_STATUS_HCERR,
            regValue & UHCD_STATUS_HCHALT);

    //
    // Get and display the interrupt enable register (USBINTR)
    //

    ReadIoSpace((ULONG) INTERRUPT_MASK_REG((&uhcd)), &regValue, &size);

    dprintf("\n");
    dprintf("Interrupt Register: ");
    PRINT_FLAGS(regValue, UHCD_INT_MASK_TIMEOUT);
    PRINT_FLAGS(regValue, UHCD_INT_MASK_RESUME);
    PRINT_FLAGS(regValue, UHCD_INT_MASK_IOC);
    PRINT_FLAGS(regValue, UHCD_INT_MASK_SHORT);
    dprintf("\n");

    //
    // Get and display the frame number (FRNUM)
    //

    ReadIoSpace((ULONG) FRAME_LIST_CURRENT_INDEX_REG((&uhcd)), &regValue, &size);

    dprintf("\n");
    dprintf("Frame Number: %4x  ", regValue);

    //
    // Get and display the frame list base address (FRBASEADD)
    //

    size = 4;
    ReadIoSpace((ULONG) FRAME_LIST_BASE_REG((&uhcd)), &regValue, &size);

    dprintf("Frame List Base Address: %8x\n", regValue);
    
    //
    // Get and display the SOF Modify register (SOFMOD)
    //

    size = 2;
    ReadIoSpace((ULONG) SOF_MODIFY_REG((&uhcd)), &regValue, &size);

    dprintf("\n");
    dprintf("SOF Modify (%2x) --> Frame Length = %d\n",
            regValue,
            regValue + UHCD_12MHZ_SOF);

    //
    // Get and display the port status register for port 1
    //

    ReadIoSpace((ULONG) PORT1_REG((&uhcd)), &regValue, &size);

    UhciPortRegister(1, regValue);

    //
    // Get and display the port status register for port 2
    //

    ReadIoSpace((ULONG) PORT2_REG((&uhcd)), &regValue, &size);

    UhciPortRegister(2, regValue);

    return;
}

VOID
UhciPortRegister(
    ULONG   PortNumber,
    ULONG   Value
)
{
    dprintf("\n");
    dprintf("Port %2d: Device Connected: %1x  Connect Status Change: %1x\n"
            "          Port Enabled:     %1x  Port Enabled Changed:  %1x\n"
            "          Line Status D+:   %1x  Line Status D-         %1x\n"
            "          Resume Detect:    %1x  LS Device Attached:    %1x\n"
            "          Suspended (%1x):  ",
            PortNumber,
            Value & 0x01,
            Value & 0x02,
            Value & 0x04,
            Value & 0x08,
            Value & 0x10,
            Value & 0x20,
            Value & 0x40,
            Value & 0x100,
            Value & 0x400,
            Value & 0x1800);

    switch (Value & 0x1800)
    {
        case (0x00008000):
            dprintf("Enabled");
            break;

        case (0x00018000):
            dprintf("Suspend");
            break;

        default:
            dprintf("Disabled");
            break;
    }

    return;
}
