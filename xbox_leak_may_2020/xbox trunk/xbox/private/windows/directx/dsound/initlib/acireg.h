/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       acireg.h
 *  Content:    ACI registers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  09/27/00    dereks  Created based on NVidia code.
 *
 ****************************************************************************/

#ifndef __ACIREG_H__
#define __ACIREG_H__

//
// Memory-mapped IO offsets
//

#define AC97_MIO_BASE       0x0000      // AC'97 base offset
#define ACI_MIO_BASE        0x0100      // ACI base offset

//
// DMA channel relative offsets.
//

#define AO_BL_BASE          0x0010      // audio out buffer list offset
#define SO_BL_BASE          0x0070      // SP/DIF out buffer list offset

//
// Defines for base-relative bus master register access
//

#define X_BL_BASE           0x0000      // Buffer List Base Address
#define X_CUR_IDX           0x0004      // Current Index
#define X_LST_IDX           0x0005      // Last Valid Index
#define X_STATUS            0x0006      // Status Register
#define X_CUR_POS           0x0008      // Current Position
#define X_NXT_IDX           0x000a      // Next Index
#define X_CTRL              0x000b      // Control
#define X_STWD              0x000c      // Channel status word (SPDIF only)

//
// DMA Status Register bit defines
//

#define X_STATUS_FIFOE      0x0010      // FIFO Error
#define X_STATUS_BCI        0x0008      // Buffer Completion Interrupt
#define X_STATUS_LVBCI      0x0004      // Last Valid Buffer Completion Interrupt
#define X_STATUS_CIELV      0x0002      // Current Index Equals Last Valid
#define X_STATUS_DCH        0x0001      // DMA Controller Halted

//
// DMA Control Register bit defines
//

#define X_CTRL_IOCE         0x10        // Interrupt On Completion Enable
#define X_CTRL_FEIE         0x08        // FIFO Error Interrupt Enable
#define X_CTRL_LVBIE        0x04        // Last Valid Buffer Interrupt Enable
#define X_CTRL_RBMR         0x02        // Reset Bus Master Registers
#define X_CTRL_RPBM         0x01        // Run/Pause Bus Master

//
// Global Control Register
//

#define GLB_CTRL            0x002c

//
// Global Control bit defines
//

#define GLB_CTRL_SRIE       0x00000020  // Secondary Resume Interrupt Enable
#define GLB_CTRL_PRIE       0x00000010  // Primary Resume Interrupt Enable
#define GLB_CTRL_ACSD       0x00000008  // AC-link Shut Down
#define GLB_CTRL_WARM       0x00000004  // AC97 Warm Reset
#define GLB_CTRL_COLD       0x00000002  // AC97 Cold Reset
#define GLB_CTRL_GIE        0x00000001  // GPIO Interrupt Enable

//
// Global Status Register
//

#define GLB_STATUS          0x0030

//
// Global Status bit defines
//

#define GLB_STATUS_MD3      0x00020000  // Power Down Semaphore for Modem
#define GLB_STATUS_AD3      0x00010000  // Power Down Semaphore for Audio
#define GLB_STATUS_RCS      0x00008000  // Read Completion Status
#define GLB_STATUS_B3S12    0x00004000  // Bit 3 Slot 12
#define GLB_STATUS_B2S12    0x00002000  // Bit 2 Slot 12
#define GLB_STATUS_B1S12    0x00001000  // Bit 1 Slot 12
#define GLB_STATUS_SRI      0x00000800  // Secondary Resume Interrupt
#define GLB_STATUS_PRI      0x00000400  // Primary Resume Interrupt
#define GLB_STATUS_SCR      0x00000200  // Secondary Codec Ready
#define GLB_STATUS_PCR      0x00000100  // Primary Codec Ready
#define GLB_STATUS_AOINT    0x00000040  // Audio Out Interrupt
#define GLB_STATUS_SOINT    0x00000010  // SP/DIF Out Interrupt
#define GLB_STATUS_GPINT    0x00000001  // GPIO Interrupt

//
// AC97 Register Semaphore
//

#define AC97_SEM4           0x0034

//
// AC97 Register Semaphore bit defines
//

#define AC97_SEM4_OWNED     0x01        // AC97 Semaphore Owned

//
// Physical region descriptor
//

typedef struct
{
    DWORD   dwPhysicalAddress;
    DWORD   wLength                 : 16;
    DWORD   wReserved               : 14;
    BOOL    fBufferUnderrunPolicy   : 1;
    BOOL    fInterruptOnCompletion  : 1;
} ACIPRD, *LPACIPRD;

//
// Physical region descriptor list
//

#define ACIPRDL_ENTRY_COUNT 32
#define ACIPRDL_ENTRY_SHIFT 5
#define ACIPRDL_ENTRY_MASK  0x1F

//
// SP/DIF STWD values
//

#define SPDIF_STWD_SURROUND 0x02000000
#define SPDIF_STWD_AC3      0x02000002

#endif // __ACIREG_H__
