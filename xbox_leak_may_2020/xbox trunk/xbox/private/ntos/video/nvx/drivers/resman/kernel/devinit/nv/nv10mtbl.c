 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/*****************************************************************************
*
*	Header: nv10mtbl.c
*
*	Description:
*		This file contains the initialization code tables used to POST an NV10 device.
*   The init code was taken from the BIOS source file, nvinit.inc.
*
*	Revision History:
*		Original -- 6/99 Jeff Westerinen
*
*   Steps to convert all tables
*
*		1.	convert ; to //
*		2.  remove all db, dw, and dd, and add comma separaters
*		3.  convert IFDEF to #ifdef
*		4.	convert ELSE to #else
*		5.	convert ENDIF to #endif
*		6.	convert "OR \" and "OR " to "| "
*		7.	convert asm hex numbers to C hex numbers: ([0-9A-Fa-f]+)h\, to 0x\1\,
*       8.  convert asm binary numbers to C hex numbers
*		9.	format -- at least line up data to left indent
*		10. comment out all table delimiters
*
*   Steps to convert sequence tables from memcfg.asm
*
*		1.	convert SDR_Sequence_Table data to: U032 Nv10_SDR_Sequence_Table[] = { <data> }; 
*		2.	convert DDR_Sequence_Table data to: U032 Nv10_DDR_Sequence_Table[] = { <data> }; 
*
*   Steps to convert mem init table from memcfg.asm
*
*		1.	convert bmp_MemInitTbl data to: MEM_INIT_VALS Nv10_bmp_MemInitTbl[] = { <data> }; 
*       2.  remove MEM_INIT_VALS for each entry
*       3.  add defaults to elements 4, 10, and 14
*       4.  separate records with commas
*
*	Steps to convert bmp_GenInitTbl in nvinit.inc
*
*		1.	convert bmp_GenInitTbl data to: U032 Nv04_bmp_GenInitTbl[] = { <data> }; 
*		2.  remove bmp_ExtraInitTbl
*
******************************************************************************/

#include <nv10_ref.h>
#include <nvrm.h>
#include <bios.h>

U032 Nv10_SDR_Sequence_Table[] = 
{
    NV_PFB_REFCTRL,
    NV_PFB_CFG,
    NV_PFB_TIMING0,
    NV_PFB_TIMING1,
    NV_PFB_TIMING2,
    NV_PBUS_DEBUG_CTRIM_4,
    NV_PFB_PRE,
    NV_PFB_PIN,
    NV_PFB_PAD,
    NV_PFB_EMRS,
    NV_PFB_MRS,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_PRE,
    NV_PFB_PRE,
    NV_PFB_PRE,
    NV_PFB_MRS,
    NV_PFB_REFCTRL,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF      // List terminator
};
        
U032 Nv10_DDR_Sequence_Table[] = 
{
    NV_PFB_REFCTRL,
    NV_PFB_CFG,
    NV_PFB_TIMING0,
    NV_PFB_TIMING1,
    NV_PFB_TIMING2,
    NV_PBUS_DEBUG_CTRIM_4,
    NV_PFB_PIN,
    NV_PFB_PAD,
    NV_PFB_PRE,
    NV_PFB_EMRS,
    NV_PFB_MRS,
    NV_PFB_PRE,
    NV_PFB_REF,
    NV_PFB_REF,
    NV_PFB_MRS,
    NV_PFB_REFCTRL,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF      // List terminator
};
   
MEM_INIT_TABLE_NV10 Nv10_bmp_MemInitTbl = 
{
    // Note - PFB_TIMING2: 61A = 100Mhz
    //                     91F = 150Mhz
    //                     **A2C = 166Mhz** old value
    //                     9FF = 166Mhz
    //                     B1F = 183Mhz
    //                       
    // 0000 = 64MBit x 32 SDR 166Mhz
    // Strap 0000 - 2M x 32 SDR @ 166Mhz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08B11110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL,0xFFFFFFFF, 
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0000
    // 0001 =                                 
    // Strap 0001 - 4Mx16 SDR @ 166MHz 
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08C11110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,    
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0001
    // 0010 =                                                       
    // Strap 0010 - 1Mx32 SDR @ 166MHz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08B01110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0010
    // 0011 = 16MBit x 16 SDR 200Mhz
    // Strap 0011 - 16MBit x 16 SDR @ 166Mhz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08B01110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0011
    // 0100 =            
    // Strap 0100 - 512Kx32(2-bank) SDR SGRAM @ 166Mhz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08A01110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0100
    // 0101 =            
    // Strap 0101 - 512Kx32(2-bank) SDR SGRAM @ 166Mhz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08911110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0101
    // 0110 =            
    // Strap 0110 - 8Mx16 SDR @ 100 Mhz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x09C11110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0110
    // 0111 =            
    // Strap 0111 - 8M x 8 SDR SDRAM @ 166MHz
    {0, 16600, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x09C11110, 
                     0x33070A0A, 0x02211150, 0x000009FF,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0111
                     
    // 1000 = 64MBit x 32 SDR 150Mhz
    // Strap 0000 - 2M x 32 SDR @ 150Mhz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08B11110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL,0xFFFFFFFF, 
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0000
    // 1001 =                                 
    // Strap 0001 - 4Mx16 SDR @ 150MHz 
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08C11110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,    
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0001
    // 1010 =                                                       
    // Strap 0010 - 1Mx32 SDR @ 150MHz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08B01110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0010
    // 1011 = 16MBit x 16 SDR 200Mhz
    // Strap 0011 - 16MBit x 16 SDR @ 150Mhz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08B01110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0011
    // 1100 =            
    // Strap 0100 - 512Kx32(2-bank) SDR SGRAM @ 150Mhz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08A01110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0100
    // 1101 =            
    // Strap 0101 - 512Kx32(2-bank) SDR SGRAM @ 150Mhz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x08911110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0101
    // 1110 =            
    // Strap 0110 - 8Mx16 SDR @ 150 Mhz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x09C11110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}},   // 0110
    // 1111 =            
    // Strap 0111 - 8M x 8 SDR SDRAM @ 150MHz
    {0, 15000, 12000,
                   { NV_PFB_REFCTRL_VALID_0, 0x09C11110, 
                     0x33070A0A, 0x02211150, 0x0000091F,
                     0x58761433, NV_PFB_PIN_CKE_NORMAL | NV_PFB_PIN_DQM_NORMAL, 0xFFFFFFFF,  
                     0x00000030, 0x00000030, NV_PFB_REFCTRL_VALID_1}}   // 0111
};
                         
U032 Nv10_bmp_GenInitTbl[] = 
{
#if 0
    //-----------------------------------------------------
    // Do a software reset here
    //-----------------------------------------------------
    INIT_RESET,                          // Sequence ID byte
    NV_PMC_ENABLE,                       // NV 32 bit address.
    // 
    // Off state
    NV_PMC_ENABLE_BUF_RESET_DISABLE | 
       NV_PMC_ENABLE_PMEDIA_DISABLED   | 
       NV_PMC_ENABLE_PFIFO_DISABLED    | 
       NV_PMC_ENABLE_PGRAPH_DISABLED   | 
       NV_PMC_ENABLE_PPMI_DISABLED     | 
       NV_PMC_ENABLE_PFB_DISABLED      | 
       NV_PMC_ENABLE_PCRTC_ENABLED     | 
       NV_PMC_ENABLE_PVIDEO_DISABLED,
                                  
    //
    // On state
    NV_PMC_ENABLE_BUF_RESET_DISABLE  | 
       NV_PMC_ENABLE_PMEDIA_ENABLED     | 
       NV_PMC_ENABLE_PFIFO_ENABLED      | 
       NV_PMC_ENABLE_PGRAPH_DISABLED    | 
       NV_PMC_ENABLE_PPMI_ENABLED       | 
       NV_PMC_ENABLE_PFB_ENABLED        | 
       NV_PMC_ENABLE_PCRTC_ENABLED      | 
       NV_PMC_ENABLE_PVIDEO_DISABLED,
       
    //-----------------------------------------------------
    // Configure the strap fields here
    //-----------------------------------------------------
    INIT_NV_REG,                          // Write with Mask
    NV_PEXTDEV_BOOT_0,                    // Strap Register Update
    // 
    //   +---------------------------------- OVERWRITE_ENABLED
    //   |++++++++++++++++------------------ UNUSED
    //   |||||||||||||||||+----------------- FB32MB
    //   ||||||||||||||||||++--------------- PCI_DEVID[1:0]
    //   ||||||||||||||||||||+-------------- RESERVED_11 (NV5)/AGP_FASTWR (NV10)
    //   |||||||||||||||||||||+------------- AGP_SBA_DISABLED
    //   ||||||||||||||||||||||+------------ AGP_4X_DISABLED
    //   |||||||||||||||||||||||++---------- TVMODE[1:0]
    //   |||||||||||||||||||||||||+--------- CRYSTAL_14318180
    //   ||||||||||||||||||||||||||++++----- RAMCFG[3:0]
    //   ||||||||||||||||||||||||||||||+---- SUB_VENDOR_BIOS              
    //   |||||||||||||||||||||||||||||||+--- PCI_AD_NORMAL
    //   VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
    //   3322222222221111111111
    //   10987654321098765432109876543210
    //11111111111111111111011111111111b    // AND Mask      
    0xfffff7ff,    // AND Mask      
    //10000000000000000000000000000000b    // |  Mask - force write
    0x80000000,    // |  Mask - force write

    INIT_CONFIGURE_PREINIT,               // Read RAM type and Ref Freq
                                           // This also inits CR3C

    //-----------------------------------------------------
    // Make sure the NV10 fan is running
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x035,0x0FE,0x001,
                   
    //-----------------------------------------------------
    // Set Scratch4 to let the VESA interface know the fan is on
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x03C,0x0F7,0x008,
                                              
    // Disable Framebuffer
    INIT_ZM_REG,                         // Write register
    NV_PMC_ENABLE,                       // NV 32 bit address.
    NV_PMC_ENABLE_BUF_RESET_ENABLE   | 
       NV_PMC_ENABLE_PMEDIA_ENABLED     | 
       NV_PMC_ENABLE_PFIFO_ENABLED      | 
       NV_PMC_ENABLE_PGRAPH_DISABLED    | 
       NV_PMC_ENABLE_PPMI_ENABLED       | 
       NV_PMC_ENABLE_PFB_DISABLED       | 
       NV_PMC_ENABLE_PCRTC_ENABLED      | 
       NV_PMC_ENABLE_PVIDEO_DISABLED,
       
    INIT_INDEX_IO,
    SEQ_INDEX,
    0x001,0x0FF,0x020,           // FullCPU Access On/CRTC Refresh off

    // Note: CRTC Refresh and the Framebuffer need to be disabled
    //       while programming NVCLK and MCLK
       
    INIT_PLL,
    NV_PRAMDAC_NVPLL_COEFF,              // Program default NVCLOCK
    10000,                               // 100Mhz   
                                              
    INIT_PLL,
    NV_PRAMDAC_MPLL_COEFF,               // Program default MCLOCK
    8300,                                // 83Mhz        
                                              
    //-----------------------------------------------------
    // Set per MK on 5/26/99
    //-----------------------------------------------------
    INIT_ZM_REG,                         // Write register
    NV_PBUS_DEBUG_CTRIM_0,                       
    0x03115223,                           // 05/26/99

    //-----------------------------------------------------
    //-----------------------------------------------------
    INIT_NV_REG,                         // Write with Mask
    NV_PBUS_DEBUG_CTRIM_1,                       
    0x0FFFFFFFF,                          // AND Mask
    0x000000000,                          // | Mask

    //-----------------------------------------------------
    //-----------------------------------------------------
    INIT_ZM_REG,                         // Write register
    NV_PBUS_DEBUG_CTRIM_2,                       
    0x000000000,
           
    //-----------------------------------------------------
    //-----------------------------------------------------
    INIT_ZM_REG,                         // Write register
    NV_PBUS_DEBUG_CTRIM_3,                       
    0x0FF223003,

    //-----------------------------------------------------
    // Set per MK on 5/26/99
    //-----------------------------------------------------
    // INIT_ZM_REG                         // Write register
    // NV_PBUS_DEBUG_CTRIM_4               
    // 9CAB1433h                           // 05/26/99
    //
    // above now done in memcfg.asm
    //

    //-----------------------------------------------------
    //-----------------------------------------------------
    INIT_NV_REG,                         // Write with Mask
    NV_PBUS_DEBUG_CTRIM_5,                       
    0x0FFFFFFFF,                          // AND Mask
    0x000000000,                          // | Mask

    //-----------------------------------------------------
    //-----------------------------------------------------
    INIT_NV_REG,                         // Write with Mask
    NV_PBUS_DEBUG_CTRIM_6,                       
    0x0FFFFFFFF,                          // AND Mask
    0x000000000,                          // | Mask

    INIT_ZM_REG,                         // Write register
    NV_PRAMDAC_PLL_COEFF_SELECT,         // NV 32 bit address.
    0x00000500,                           // PROG_MPLL & PROG_NVPLL

    INIT_CONFIGURE_CLK,                  // Set MCLK/NVCLK

    // Enable Framebuffer
    INIT_ZM_REG,                         // Write register
    NV_PMC_ENABLE,                       // NV 32 bit address.
    NV_PMC_ENABLE_BUF_RESET_ENABLE   | 
       NV_PMC_ENABLE_PMEDIA_ENABLED     | 
       NV_PMC_ENABLE_PFIFO_ENABLED      | 
       NV_PMC_ENABLE_PGRAPH_DISABLED    | 
       NV_PMC_ENABLE_PPMI_ENABLED       | 
       NV_PMC_ENABLE_PFB_ENABLED        | 
       NV_PMC_ENABLE_PCRTC_ENABLED      | 
       NV_PMC_ENABLE_PVIDEO_DISABLED,

    //-----------------------------------------------------
    // Set the drive strengh register. The default AND/OR
    // mask causes the BIOS to initialize with the power
    // on default value.
    //-----------------------------------------------------
    INIT_NV_REG,                         // Write with Mask
    NV_PBUS_DEBUG_PRIV_ASRC,                      
    0x0FFFFFFFF,                          // AND Mask
    0x000000000,                          // | Mask
       
    //-----------------------------------------------------
    //-----------------------------------------------------

    // 8/25/99
    // Bit 7 (PCIS_RETRY is SPARE0, and has changed from 1 to 0 here)
    //
    INIT_ZM_REG,                         // Write register
    NV_PBUS_DEBUG_1,
    // 
    //   ++++++++++++++++------------------- SPARES
    //   ||||||||||||||||+------------------ AGPFW_DWOD_DISABLED
    //   |||||||||||||||||+----------------- FBI_DIFFERENTIAL_DISABLED
    //   ||||||||||||||||||+---------------- DPSH_DECODE_NV4
    //   |||||||||||||||||||+--------------- PCIS_CPUQ_ENABLED
    //   ||||||||||||||||||||+-------------- UNDEFINED
    //   |||||||||||||||||||||+------------- PCIS_EARLY_RTY_ENABLED
    //   ||||||||||||||||||||||+------------ PCIS_WR_BURST_ENABLED
    //   |||||||||||||||||||||||+----------- PCIS_RD_BURST_ENABLED
    //   ||||||||||||||||||||||||+---------- SPARE0_ZERO
    //   |||||||||||||||||||||||||+--------- PCIS_2_1_ENABLED
    //   ||||||||||||||||||||||||||+-------- PCIS_WRITE_0_CYCLE
    //   |||||||||||||||||||||||||||++------ AGPM_CMD_LP_ONLY
    //   |||||||||||||||||||||||||||||+----- HASH_DECODE_1FF
    //   ||||||||||||||||||||||||||||||+---- PCIM_CMD_SIZE_BASED
    //   |||||||||||||||||||||||||||||||+--- PCIM_THROTTLE_DISABLED
    //   VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV    
    //   3322222222221111111111
    //   10987654321098765432109876543210
    //00000000000000000101011101001000b,   // Write Value
    0x00005748,   // Write Value
                                   
    INIT_ZM_REG,                         // Write register
    NV_PFB_DEBUG_0,
    0,

    // New "Configure Memory" token
    INIT_CONFIGURE_MEM,                  // Configure the memory type

    INIT_COMPUTE_MEM,                    // Size the memory

    INIT_INDEX_IO,
    SEQ_INDEX,
    0x001,0x0DF,0x000,           // FullCPU Access Off

    //-----------------------------------------------------
    // Program VCLOCK
    //         
    // 13,500,000 MHz - VCLK = M:12:0C, N:064:40, P:2:02 LOG2P:1 36.00 MHz
    // 14,318,180 MHz - VCLK = M:14:0E, N:141:8D, P:4:04 LOG2P:2 36.05 MHz
    //-----------------------------------------------------
    INIT_PLL,
    NV_PRAMDAC_VPLL_COEFF,               // Program default VCLOCK
    3600,                                // 36Mhz   

    //-----------------------------------------------------
    // Set to NV5 values f| now
    //-----------------------------------------------------
    INIT_ZM_REG,
    NV_PBUS_DEBUG_2,
    0x000011F0,

    //-----------------------------------------------------
    // Set to NV5 values f| now
    //-----------------------------------------------------
    INIT_ZM_REG,
    NV_PBUS_DEBUG_3,
    NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_120MHZ | NV_PBUS_DEBUG_3_AGP_MAX_SIZE_32_BYTES,

    //-----------------------------------------------------
    // Do CRTC IO default programming
    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    // Force real mode window default state to off
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
	//NVRM_ACCESS,0x000,0x000,				// !! undefined
	NV_CIO_CRE_RMA__INDEX,0x000,0x000,

    //-----------------------------------------------------
    // Make sure hsync and vsync are on (we only set them in DPMS calls)
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x01A,0x000,0x03F,
                   
    //-----------------------------------------------------
    // Boot with TV off, and set f| NTSC if it was on.
    // Also set horizontal pixel adjustment
    // Sets pixel format to VGA
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x028,0x000,0x000,

    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x033,0x000,0x000,         // Clear slave registers

    //-----------------------------------------------------
    // Force Lock register to default state
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x021,0x000,0x0FA,         // Force Lock register to 0FAh

    //-----------------------------------------------------
    // Clear scratch registers - TV Selection
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x02B,0x000,0x000,         // Clear all scratch bits f| default
                                        
    //-----------------------------------------------------
    // Clear scratch registers to default state
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x02C,0x000,0x000,         // Clear all scratch bits f| default

    //-----------------------------------------------------
    // Clear scratch registers to default state
    //-----------------------------------------------------
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x03A,0x000,0x000,         // Clear all scratch bits f| default
    
    INIT_INDEX_IO,
    CRTC_INDEX_COLOR,
    0x03B,0x000,0x000,         // Clear all scratch bits f| default

    //-----------------------------------------------------
    // End of the list
    //-----------------------------------------------------
    INIT_DONE
    
#else

    // This data was extracted directly from a GeForce reference card.
    // It differs from the translated source code above and is the only
    // version between the two that actually works.  If necessary, we
    // must either create a development path from the actual BIOS source
    // code or create a tool that extracts the tables from a BIOS binary.
    0x00000065, 0x00000200, 0x01000000, 0x01110110,
    0x0000006e, 0x00101000, 0xfffff7ff, 0x80000000,
    0x00000068, 0x00000078, 0x000003d4, 0x00000035,
    0x000000fe, 0x00000001, 0x00000078, 0x000003d4,
    0x0000003a, 0x00000000, 0x00000001, 0x0000007a,
    0x00000200, 0x01010111, 0x00000078, 0x000003c4,
    0x00000001, 0x000000ff, 0x00000020, 0x00000079,
    0x00680500, 0x00002710, 0x00000079, 0x00680504,

    0x0000206c, 0x0000007a, 0x000010b0, 0x03115223,
    0x0000006e, 0x000010b4, 0xffffffff, 0x00000000,
    0x0000007a, 0x000010b8, 0x00000000, 0x0000007a,
    0x000010bc, 0xff223003, 0x0000006e, 0x000010c8,
    0xffffffff, 0x00000000, 0x0000006e, 0x000010cc,
    0xffffffff, 0x00000000, 0x0000007a, 0x0068050c,
    0x00000500, 0x00000067, 0x0000007a, 0x00000200,
    0x01110111, 0x0000006e, 0x000010e0, 0xffffffff,

    0x00000000, 0x0000007a, 0x00001084, 0x00005748,
    0x0000007a, 0x00100080, 0x00000000, 0x00000066,
    0x00000063, 0x00000078, 0x000003c4, 0x00000001,
    0x000000df, 0x00000000, 0x00000079, 0x00680508,
    0x00000e10, 0x0000007a, 0x00001088, 0x000011f0,
    0x0000007a, 0x0000108c, 0x000000d1, 0x00000078,
    0x000003d4, 0x00000038, 0x00000000, 0x00000000,
    0x00000078, 0x000003d4, 0x0000001a, 0x00000000,

    0x0000003f, 0x00000078, 0x000003d4, 0x00000028,
    0x00000000, 0x00000000, 0x00000078, 0x000003d4,
    0x00000033, 0x00000000, 0x00000000, 0x00000078,
    0x000003d4, 0x00000021, 0x00000000, 0x000000fa,
    0x00000078, 0x000003d4, 0x0000002b, 0x00000000,
    0x00000000, 0x00000078, 0x000003d4, 0x0000002c,
    0x00000000, 0x00000000, 0x00000078, 0x000003d4,
    0x0000003b, 0x00000000, 0x00000000, 0x00000071

#endif

};


