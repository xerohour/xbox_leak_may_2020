/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    xpcicfg.h

Abstract:

    This module defines the static PCI configuration for an XBOX.

--*/

#ifndef _XPCICFG_
#define _XPCICFG_

//
// Note that the Advanced Programmable Interrupt Controller (APIC) defaults to
// location 0xFEE00000 and consumes one page.  The kernel turns off the APIC,
// but debugger extensions (VTune) may reenable it.
//

// Bus 0, device 0, function 0.
#define XPCICFG_HOSTBRIDGE_DEVICE_ID                        0
#define XPCICFG_HOSTBRIDGE_FUNCTION_ID                      0

// Bus 0, device 1, function 0.
#define XPCICFG_LPCBRIDGE_DEVICE_ID                         1
#define XPCICFG_LPCBRIDGE_FUNCTION_ID                       0
#define XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0                0x8000
#define XPCICFG_LPCBRIDGE_IO_REGISTER_LENGTH_0              0x0100

// Bus 0, device 1, function 1.
#define XPCICFG_SMBUS_DEVICE_ID                             1
#define XPCICFG_SMBUS_FUNCTION_ID                           1
#define XPCICFG_SMBUS_IO_REGISTER_BASE_1                    0xC000
#define XPCICFG_SMBUS_IO_REGISTER_LENGTH_1                  0x0010
#define XPCICFG_SMBUS_IO_REGISTER_BASE_2                    0xC200
#define XPCICFG_SMBUS_IO_REGISTER_LENGTH_2                  0x0020

// Bus 0, device 2, function 0.
#define XPCICFG_USB0_DEVICE_ID                              2
#define XPCICFG_USB0_FUNCTION_ID                            0
#define XPCICFG_USB0_IRQ                                    1
#define XPCICFG_USB0_MEMORY_REGISTER_BASE_0                 0xFED00000
#define XPCICFG_USB0_MEMORY_REGISTER_LENGTH_0               0x00001000

// Bus 0, device 3, function 0.
#define XPCICFG_USB1_DEVICE_ID                              3
#define XPCICFG_USB1_FUNCTION_ID                            0
#define XPCICFG_USB1_IRQ                                    9
#define XPCICFG_USB1_MEMORY_REGISTER_BASE_0                 0xFED08000
#define XPCICFG_USB1_MEMORY_REGISTER_LENGTH_0               0x00001000

// Bus 0, device 4, function 0.
#define XPCICFG_NIC_DEVICE_ID                               4
#define XPCICFG_NIC_FUNCTION_ID                             0
#define XPCICFG_NIC_IRQ                                     4
#define XPCICFG_NIC_MEMORY_REGISTER_BASE_0                  0xFEF00000
#define XPCICFG_NIC_MEMORY_REGISTER_LENGTH_0                0x00000400
#define XPCICFG_NIC_IO_REGISTER_BASE_1                      0xE000
#define XPCICFG_NIC_IO_REGISTER_LENGTH_1                    0x0008

// Bus 0, device 5, function 0.
#define XPCICFG_APU_DEVICE_ID                               5
#define XPCICFG_APU_FUNCTION_ID                             0
#define XPCICFG_APU_IRQ                                     5
#define XPCICFG_APU_MEMORY_REGISTER_BASE_0                  0xFE800000
#define XPCICFG_APU_MEMORY_REGISTER_LENGTH_0                0x00080000

// Bus 0, device 6, function 0.
#define XPCICFG_ACI_DEVICE_ID                               6
#define XPCICFG_ACI_FUNCTION_ID                             0
#define XPCICFG_ACI_IRQ                                     6
#define XPCICFG_ACI_IO_REGISTER_BASE_0                      0xD000
#define XPCICFG_ACI_IO_REGISTER_LENGTH_0                    0x0100
#define XPCICFG_ACI_IO_REGISTER_BASE_1                      0xD200
#define XPCICFG_ACI_IO_REGISTER_LENGTH_1                    0x0080
#define XPCICFG_ACI_MEMORY_REGISTER_BASE_2                  0xFEC00000
#define XPCICFG_ACI_MEMORY_REGISTER_LENGTH_2                0x00001000

// Bus 0, device 9, function 0.
#define XPCICFG_IDE_DEVICE_ID                               9
#define XPCICFG_IDE_FUNCTION_ID                             0
#define XPCICFG_IDE_IRQ                                     14
#define XPCICFG_IDE_IO_REGISTER_BASE_4                      0xFF60
#define XPCICFG_IDE_IO_REGISTER_LENGTH_4                    0x0010

// Bus 0, device 30, function 0.
#define XPCICFG_AGPBRIDGE_DEVICE_ID                         30
#define XPCICFG_AGPBRIDGE_FUNCTION_ID                       0

// Bus 1, device 0, device 0.
#define XPCICFG_GPU_DEVICE_ID                               0
#define XPCICFG_GPU_FUNCTION_ID                             0
#define XPCICFG_GPU_IRQ                                     3
#define XPCICFG_GPU_MEMORY_REGISTER_BASE_0                  0xFD000000
#define XPCICFG_GPU_MEMORY_REGISTER_LENGTH_0                0x01000000

// Map a device name to PCI slot number
#define XPCI_SLOT_NUMBER(device) \
        ((XPCICFG_##device##_DEVICE_ID) | ((XPCICFG_##device##_FUNCTION_ID) << 5))

#endif  // XPCICFG
