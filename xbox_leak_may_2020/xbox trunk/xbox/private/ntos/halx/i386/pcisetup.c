/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    pcisetup.c

Abstract:

    This module implements routines to setup PCI at system boot.

--*/

#include "halp.h"
#include <pci.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, HalpSetupPCIDevices)
#endif

VOID
HalpSetupPCIDevices(
    VOID
    )
{
    PCI_SLOT_NUMBER PCISlotNumber;
    PCI_COMMON_CONFIG Configuration;
    ULONG interrupt;
    UCHAR pciint;
    UCHAR acpitimer;
    ULONG agpcmd;
    ULONG acicfg;
    ULONG epgpclock;
    ULONG mcpUsbCfg20Value;

    PCISlotNumber.u.AsULONG = 0;

    //
    // Interrupt routing.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_LPCBRIDGE_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_LPCBRIDGE_FUNCTION_ID;

    pciint = 0x3;
    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x6A, &pciint, sizeof(pciint));

    interrupt = 0x0E065491;
    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x6C, &interrupt, sizeof(interrupt));

    interrupt = 0x00000b0c;
    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x64, &interrupt, sizeof(interrupt));

    //
    // Select a 32-bit ACPI timer.
    //

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0x81, &acpitimer, sizeof(acpitimer));

    acpitimer |= 0x08;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x81, &acpitimer, sizeof(acpitimer));

    //
    // Configure the IDE controller.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_IDE_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_IDE_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER);
    Configuration.ProgIf &= ~0x05;
    Configuration.u.type0.BaseAddresses[4] = XPCICFG_IDE_IO_REGISTER_BASE_4 | PCI_ADDRESS_IO_SPACE;
    *((PULONG)((PUCHAR)&Configuration + 0x50)) = 2;
    *((PULONG)((PUCHAR)&Configuration + 0x58)) = 0x20202020;
    *((PULONG)((PUCHAR)&Configuration + 0x60)) = 0xC0C0C0C0;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // Configure the NIC.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_NIC_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_NIC_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);
    Configuration.u.type0.BaseAddresses[0] = XPCICFG_NIC_MEMORY_REGISTER_BASE_0;
    Configuration.u.type0.BaseAddresses[1] = XPCICFG_NIC_IO_REGISTER_BASE_1 | PCI_ADDRESS_IO_SPACE;
    Configuration.u.type0.InterruptLine = XPCICFG_NIC_IRQ;
    Configuration.u.type0.InterruptPin = 0;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // Configure USB0.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_USB0_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_USB0_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);
    Configuration.u.type0.BaseAddresses[0] = XPCICFG_USB0_MEMORY_REGISTER_BASE_0;
    Configuration.u.type0.InterruptLine = XPCICFG_USB0_IRQ;
    Configuration.u.type0.InterruptPin = 0;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // Assign peripheral ports 0 through 3 to USB0
    //

    mcpUsbCfg20Value = MCP_USB_CFG_20_PORT0 | MCP_USB_CFG_20_PORT1 | MCP_USB_CFG_20_PORT2 | MCP_USB_CFG_20_PORT3;
    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, MCP_USB_CFG_20, &mcpUsbCfg20Value, sizeof(mcpUsbCfg20Value));
    
    //
    // Configure USB1.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_USB1_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_USB1_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);
    Configuration.u.type0.BaseAddresses[0] = XPCICFG_USB1_MEMORY_REGISTER_BASE_0;
    Configuration.u.type0.InterruptLine = XPCICFG_USB1_IRQ;
    Configuration.u.type0.InterruptPin = 0;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // Assign peripheral ports 4 and 5 to USB1
    //
    mcpUsbCfg20Value = MCP_USB_CFG_20_PORT4 | MCP_USB_CFG_20_PORT5;
    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, MCP_USB_CFG_20, &mcpUsbCfg20Value, sizeof(mcpUsbCfg20Value));

    //
    // Configure audio.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_ACI_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_ACI_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);
    Configuration.u.type0.InterruptLine = XPCICFG_ACI_IRQ;
    Configuration.u.type0.InterruptPin = 0;
    Configuration.u.type0.BaseAddresses[0] = XPCICFG_ACI_IO_REGISTER_BASE_0 | PCI_ADDRESS_IO_SPACE;
    Configuration.u.type0.BaseAddresses[1] = XPCICFG_ACI_IO_REGISTER_BASE_1 | PCI_ADDRESS_IO_SPACE;
    Configuration.u.type0.BaseAddresses[2] = XPCICFG_ACI_MEMORY_REGISTER_BASE_2;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_APU_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_APU_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);
    Configuration.u.type0.InterruptLine = XPCICFG_APU_IRQ;
    Configuration.u.type0.InterruptPin = 0;
    Configuration.u.type0.BaseAddresses[0] = XPCICFG_APU_MEMORY_REGISTER_BASE_0;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // AUDIO: set GP and EP DSPs clock frequency
    //

    PCISlotNumber.u.AsULONG = 0;
    PCISlotNumber.u.bits.DeviceNumber = 1;
    PCISlotNumber.u.bits.FunctionNumber = 0;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0x8C, &epgpclock, sizeof(epgpclock));

    //
    // bits 26 and 27 set the clock frequency of the MCP
    // 0x00, 2b00  =  160
    // 0x01, 2b01  =  200
    // 0x02, 2b10  =  133
    // 0x03, 2b11  =  160
    //

    epgpclock &= ~(0x3 << 26);
    epgpclock |= 0x2 << 26;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x8C, &epgpclock, sizeof(epgpclock));

    //
    // Turn on S/PDIF
    //

    _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xCD, 8);

    //
    // Set up ACI and S/PDIF to always loop on the DMA, regardless of whether
    // interrupts have been handled or not.
    //

    HalReadPCISpace(0, XPCICFG_ACI_DEVICE_ID, 0x4C, &acicfg, sizeof(acicfg));

    acicfg |= (1UL << 16);
    acicfg |= (1UL << 24);

    HalWritePCISpace(0, XPCICFG_ACI_DEVICE_ID, 0x4C, &acicfg, sizeof(acicfg));

    //
    // Configure AGP.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_AGPBRIDGE_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_AGPBRIDGE_FUNCTION_ID;

    HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);

    Configuration.u.type1.PrimaryBus = 0;
    Configuration.u.type1.SecondaryBus = 1;
    Configuration.u.type1.SubordinateBus = 1;

    Configuration.u.type1.MemoryBase = 0xFD00;
    Configuration.u.type1.MemoryLimit = 0xFE70;
    Configuration.u.type1.PrefetchBase = 0xF000;

    if (XboxHardwareInfo.Flags & XBOX_HW_FLAG_ARCADE) {
        Configuration.u.type1.PrefetchLimit = 0xF7F0;
    } else {
        Configuration.u.type1.PrefetchLimit = 0xF3F0;
    }

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // Enable agp, 4x and fast writes on the host
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_GPU_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_GPU_FUNCTION_ID;

    agpcmd =  0x4 | 0x010 | 0x100;

    HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x48, &agpcmd, sizeof(agpcmd));

    //
    // Configure NV20.
    //

    PCISlotNumber.u.bits.DeviceNumber = XPCICFG_GPU_DEVICE_ID;
    PCISlotNumber.u.bits.FunctionNumber = XPCICFG_GPU_FUNCTION_ID;

    HalReadPCISpace(1, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    Configuration.Command |= (PCI_ENABLE_IO_SPACE | PCI_ENABLE_BUS_MASTER | PCI_ENABLE_MEMORY_SPACE);
    Configuration.u.type0.InterruptLine = XPCICFG_GPU_IRQ;
    Configuration.u.type0.InterruptPin = 1;

    HalWritePCISpace(1, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));

    //
    // Enable agp, 4x and fast writes on the card
    //

    agpcmd = 0x4 | 0x010 | 0x100;

    HalWritePCISpace(1, PCISlotNumber.u.AsULONG, 0x4C, &agpcmd, sizeof(agpcmd));
}
