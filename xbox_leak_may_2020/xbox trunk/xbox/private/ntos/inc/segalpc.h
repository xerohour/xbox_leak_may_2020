/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    segalpc.h

Abstract:

    This module defines the constants used for accessing the SEGA LPC register
    space.

--*/

#ifndef _SEGALPC_
#define _SEGALPC_

//
// Define the base address of the SEGA LPC register space.
//

#define SEGA_REGISTER_BASE                          0x4000

//
// Define the layout of the SEGA LPC register space.
//

#define SEGA_REGISTER_RUAP                          (SEGA_REGISTER_BASE + 0x00)
#define SEGA_REGISTER_RLAP                          (SEGA_REGISTER_BASE + 0x02)
#define SEGA_REGISTER_RDP                           (SEGA_REGISTER_BASE + 0x04)
#define SEGA_REGISTER_DMUAP                         (SEGA_REGISTER_BASE + 0x06)
#define SEGA_REGISTER_DMLAP                         (SEGA_REGISTER_BASE + 0x08)
#define SEGA_REGISTER_LAN_GR0                       (SEGA_REGISTER_BASE + 0x1E)
#define SEGA_REGISTER_LAN_GR1                       (SEGA_REGISTER_BASE + 0x20)
#define SEGA_REGISTER_LAN_GR2                       (SEGA_REGISTER_BASE + 0x22)
#define SEGA_REGISTER_LAN_GR3                       (SEGA_REGISTER_BASE + 0x24)
#define SEGA_REGISTER_LAN_CNT0                      (SEGA_REGISTER_BASE + 0x26)
#define SEGA_REGISTER_INTERRUPT_MASK                (SEGA_REGISTER_BASE + 0x80)
#define SEGA_REGISTER_LED                           (SEGA_REGISTER_BASE + 0x82)
#define SEGA_REGISTER_DIP_SWITCH                    (SEGA_REGISTER_BASE + 0x84)
#define SEGA_REGISTER_G1_PIO_CTRL                   (SEGA_REGISTER_BASE + 0x86)
#define SEGA_REGISTER_G1_DMA_CTRL                   (SEGA_REGISTER_BASE + 0x88)
#define SEGA_REGISTER_DMAAP_WRITE_MASK              (SEGA_REGISTER_BASE + 0x8A)
#define SEGA_REGISTER_INTERRUPT_SELECT              (SEGA_REGISTER_BASE + 0x8E)
#define SEGA_REGISTER_DIMM_RESET                    (SEGA_REGISTER_BASE + 0x90)
#define SEGA_REGISTER_CHIP_REVISION                 (SEGA_REGISTER_BASE + 0xF0)
#define SEGA_REGISTER_DIMM_SIZE                     (SEGA_REGISTER_BASE + 0xF4)

//
// Define the register masks for SEGA_REGISTER_CHIP_REVISION.
//

#define SEGA_CHIP_REVISION_CHIP_ID_MASK             0xFF00
#define SEGA_CHIP_REVISION_REVISION_ID_MASK         0x00FF

//
// Define the chip identifier codes.
//

#define SEGA_CHIP_REVISION_FPGA_CHIP_ID             0x0000
#define SEGA_CHIP_REVISION_ASIC_CHIP_ID             0x0100

//
// Define the register masks for SEGA_REGISTER_DIMM_SIZE.
//

#define SEGA_DIMM_SIZE_SIZE_MASK                    0x0003

//
// Define the memory size codes.
//

#define SEGA_DIMM_SIZE_128M                         0x0000
#define SEGA_DIMM_SIZE_256M                         0x0001
#define SEGA_DIMM_SIZE_512M                         0x0002
#define SEGA_DIMM_SIZE_1024M                        0x0003

#endif  // SEGALPC
