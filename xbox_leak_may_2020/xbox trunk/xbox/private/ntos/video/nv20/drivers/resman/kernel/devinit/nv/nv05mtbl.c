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
*	Header: nv05mtbl.c
*
*	Description:
*		This file contains the initialization code used to POST an NV5 device.
*	The init code was taken from the BIOS source file, nvinit.inc.
*
*	Revision History:
*		Original -- 5/99 Jeff Westerinen
*
*
*	Steps to convert nvinit.inc to initcode.h
*
*		1.	enclose data in: U032 Nv04_bmp_GenInitTbl[] = { <data> }; 
*		2.	convert ; to //
*		3.  remove all db, dw, and dd, and add comma separaters
*		4.  convert IFDEF to #ifdef
*		5.	convert ELSE to #else
*		6.	convert ENDIF to #endif
*		7.	convert "OR \" and "OR " to |
*		8.	convert <num>h to 0x<num>
*		9 .	format -- at least line up data to left indent
*		10. comment out all table delimiters
*		11. remove bmp_ExtraInitTbl
*
******************************************************************************/

#include <nv4_ref.h>
#include <nvrm.h>
#include <bios.h>

bmpMemInitData Nv05_bmp_MemInitTbl[] = 
/*
{
	{
		0x24,
		0x00
	},

	{
		0x28,
		0x00
	},

	{
		0x24,
		0x01
	},

	{
		0x1F,
		0x00
	},

	{
		0x0F,
		0x00
	},

	{
		0x17,
		0x00
	},

	{
		0x06,
		0x00
	},

	{
		0x00,
		0x00
	}
};
*/
{
    {
        // 0000 - 2Mx32 64Mbit 16/32mb
        0x00000024,             // bits 0-2 vary
        0x00000000              // Special 32 field
    },
    
    {
        // 0001 - 4Mx16 64Mbit 32mb
        0x00000028,        
        0x00000000              // Special 32 field
    },

    {
        // 0010 - 1Mx32 32Mbit 32mb + special32
        0x00000024,
        SPECIAL_32              // Flag field
    },

    {
        // 0011 - 1Mx16 16Mbit 8/16mb
        0x0000001F,
        0x00000000              // Special 32 field
    },
    
    {
        // 0100 - 512Kx32 16Mbit (2 bank) 4/8/16mb 
        0x0000000F,  
        0x00000000              // Special 32 field
    },
    
    {
        // 0101 - 512Kx32 16Mbit (4 bank) 4/8/16mb
        0x00000017,
        0x00000000              // Special 32 field
    },
    
    {
        // 0110 - 4Mx16 64Mbit 16MB (looks like 32MB) - "hokey rams"
        0x00000028,               
        SCRAMBLE_RAM            // Flag field
    },
                
    {
        // 0111 - whatever 0110 used to be... some 8mbit 4/8mb type
        0x00000006,             // This looks like 8mbit - 4/8mb
        0x00000000              // Flag field          
    }
};
        
U032 Nv05_bmp_ScrambleTbl[] = 
{
    0x03020100,     // SCRAMBLE_RAM_0 value
    0x07060504,     // SCRAMBLE_RAM_1 value
    0x0B0A0908,     // SCRAMBLE_RAM_2 value
    0x0F0E0D0C,     // SCRAMBLE_RAM_3 value
    0x03020100,     // SCRAMBLE_RAM_4 value
    0x07060504,     // SCRAMBLE_RAM_5 value
    0x0B0A0908,     // SCRAMBLE_RAM_6 value
    0x0F0E0D0C      // SCRAMBLE_RAM_7 value
};
        
U032 Nv05_bmp_GenInitTbl[] = 
{

	INIT_RESET,
	NV_PMC_ENABLE,      // NV 32 bit address.
	// Off state
	DRF_DEF(_PMC,_ENABLE,_PCRTC,_ENABLED) |
		DRF_DEF(_PMC,_ENABLE,_PPMI,_ENABLED) |
		DRF_DEF(_PMC,_ENABLE,_PFIFO,_ENABLED),
	DRF_DEF(_PMC,_ENABLE,_PFB,_ENABLED) |
		DRF_DEF(_PMC,_ENABLE,_PCRTC,_ENABLED) |
		DRF_DEF(_PMC,_ENABLE,_PPMI,_ENABLED) |
		DRF_DEF(_PMC,_ENABLE,_PFIFO,_ENABLED),

	INIT_NV_REG,                          // Write register with Mask
	NV_PEXTDEV_BOOT_0,                    // Strap Register Update
	//
	//  +---------------------------------- OVERWRITE_ENABLED
	//  |++++++++++++++++------------------ UNUSED
	//  |||||||||||||||||+----------------- FB32MB
	//  ||||||||||||||||||++--------------- PCI_DEVID[1:0]
	//  ||||||||||||||||||||+-------------- RESERVED_11
	//  |||||||||||||||||||||+------------- AGP_SBA_DISABLED
	//  ||||||||||||||||||||||+------------ AGP_4X_DISABLED
	//  |||||||||||||||||||||||++---------- TVMODE[1:0]
	//  |||||||||||||||||||||||||+--------- CRYSTAL_14318180
	//  ||||||||||||||||||||||||||++++----- RAMCFG[3:0]
	//  ||||||||||||||||||||||||||||||+---- SUB_VENDOR_BIOS              
	//  |||||||||||||||||||||||||||||||+--- PCI_AD_NORMAL
	//  VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
	//  3322222222221111111111
	//  10987654321098765432109876543210
	//11111111111111111111111111111111b    // AND Mask  
	0xFFFFFFFF,    
	//10000000000000000000100000000000b    // OR  Mask      
	0x80000800,
	                               
	// Comment to fix assembler bug

	INIT_ZM_REG,
	NV_PBUS_DEBUG_PRIV_ASRC,
	//        00000000h                  // 2/26/99 per Quang - fixes TV shimmer
#ifdef ULTRA
	0xF0F000F0,                  // 4/07/99 per Dave Reed
#else                                                 
	0x00000000,                   // 4/07/99 per Dave Reed
#endif  // ULTRA                                       
                                          
	// Barry's memory fix for AGP
	INIT_ZM_REG,
	NV_PBUS_DEBUG_0,
	//        01020101h    // 1/29/99 - Per Dave Reed
#ifdef ULTRA
	0x01000105,           // 4/07/99 - Per Dave Reed
#else              
	0x01020101,           // 4/07/99 - Per Dave Reed
#endif  // ULTRA     

	INIT_ZM_REG,                 // Write with mask
	NV_PBUS_DEBUG_CTRIM_0,
	//       0F00BBA7h                   // 8x6 & 10x7 panels (as of 3/22/99)
	0x0800BBA7,                   // 04/07/99

	INIT_ZM_REG,                 // Write with mask
	NV_PBUS_DEBUG_CTRIM_1,
	//       0h                          // as of 3/22/99
	0x0EFE00000,                   // 04/07/99
	                                 
	INIT_ZM_REG,                 // Write with mask
	NV_PBUS_DEBUG_CTRIM_2,
	//       0F8C98200h                  // 8x6 & 10x7 panels (as of 3/22/99)
	0x088C98200,                  // 04/07/99
	                                
	INIT_ZM_REG,                 // Write with mask
	NV_PBUS_DEBUG_CTRIM_3,
	//       0FF988AA9h                  // prior to 3/22/99
#ifdef ULTRA
	0x0FF988DDC,                  // 4/07/99
#else                                    
	0x0FF988AA9,                  // 4/07/99
#endif  // ULTRA                          
                           
	// Comment to fix assembler bug

	// 13,500,000 MHz - MCLK = M:11:0B, N:163:A3, P:4:04 LOG2P:2 50 MHz
	// 14,318,180 MHz - MCLK = M:14:0E, N:196:C4, P:4:04 LOG2P:2 50 MHz
	INIT_PLL,
	NV_PRAMDAC_NVPLL_COEFF,       // Actually, this is NVPLL (680500)
#ifdef VANTA
	10000,       // 100Mhz
#else   // VANTA
#ifdef  ULTRA
	15000,       // 150Mhz
#else   // ULTRA
	12500,       // 125Mhz
#endif  // ULTRA           
#endif  // VANTA

	// In all of these cases, Model64 (/DM64) will use the TNT2 settings

	// Comment to fix assembler bug

	// 13,500,000 MHz - MCLK = M:11:0B, N:163:A3, P:4:04 LOG2P:2 50 MHz
	// 14,318,180 MHz - MCLK = M:14:0E, N:196:C4, P:4:04 LOG2P:2 50 MHz
	INIT_PLL,
	NV_PRAMDAC_MPLL_COEFF,
#ifdef VANTA
	12500,   // 125Mhz
#else   // VANTA
#ifdef ULTRA
	18300,   // 183Mhz
#else   // ULTRA
	15000,   // 150Mhz
#endif  // ULTRA
#endif  // VANTA

	// Comment to fix assembler bug

	// NV_PFB_CONFIG_0
	INIT_ZM_REG,
	NV_PFB_CONFIG_0,      // NV 32 bit address.
	DRF_DEF(_PFB,_CONFIG_0,_RESOLUTION,_DEFAULT) |
		DRF_DEF(_PFB,_CONFIG_0,_PIXEL_DEPTH,_DEFAULT) |
		DRF_DEF(_PFB,_CONFIG_0,_TILING,_DISABLED) |
		DRF_DEF(_PFB,_CONFIG_0,_TILING_DEBUG,_DISABLED),

	// Comment to fix assembler bug

	// VCLK.
	// 13,500,000 MHz - VCLK = M:12:0C, N:064:40, P:2:02 LOG2P:1 36.00 MHz
	// 14,318,180 MHz - VCLK = M:14:0E, N:141:8D, P:4:04 LOG2P:2 36.05 MHz
	INIT_PLL,
	NV_PRAMDAC_VPLL_COEFF,        // NV 32 bit address.
	3600,
	//        00001400Ch
	//        000028D0Eh

	// Comment to fix assembler bug

	// NV_PFB_GREEN_0
	INIT_ZM_REG,
	NV_PFB_GREEN_0,   // NV 32 bit address.
	DRF_DEF(_PFB,_GREEN_0,_LEVEL,_VIDEO_ENABLED),   // NV 32 bit data.

	// Comment to fix assembler bug

	INIT_ZM_REG,
	NV_PFB_DEBUG_0,
#ifdef IKOS
	0x20000100,          // Per RussM on 10/14
#else   // IKOS                   
	//      20001D00h            // 7/23
#ifdef ULTRA
	0x20002D00,            // Per Dave R on 4/7/99
#else
	0x20002500,            // Per Dave R on 4/7/99
#endif  // ULTRA 
#endif 

	// Comment to fix assembler bug

	// Default Value - pre-memory config
	INIT_ZM_REG,
	NV_PFB_CONFIG_1,
	0x41016293,          // This value works for about everything

	// Comment to fix assembler bug

	INIT_ZM_REG,
	NV_PFB_RTL,
	0x00000000,

	// Comment to fix assembler bug

	// Per Dave Reed
	// Setup more optimal PCI interface
	INIT_ZM_REG,
	NV_PBUS_DEBUG_1,
	DRF_DEF(_PBUS,_DEBUG_1,_PCIM_THROTTLE,_DISABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIM_CMD,_SIZE_BASED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_PCIM_AGP,_IS_AGP) |			// !! undefined for NV5
		DRF_DEF(_PBUS,_DEBUG_1,_AGPM_CMD,_LP_ONLY) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_WRITE,_0_CYCLE) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_2_1,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RETRY,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RD_BURST,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_WR_BURST,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_EARLY_RTY,_ENABLED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RMAIO,_DISABLED) |		// !! undefined for NV5
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_CPUQ,_ENABLED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_SPARE1,_ZERO) |				// !! exactly wrong
		DRF_DEF(_PBUS,_DEBUG_1,_FBI_DIFFERENTIAL,_DISABLED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_SPARE2,_ONE),					// !! exactly wrong
		DRF_DEF(_PBUS,_DEBUG_1,_SPARE2,_ZERO),

	// Comment to fix assembler bug

                               
	INIT_COMPUTE_MEM,             // Compute frame buffer size
          
	//=============================================================
	//       Memory Dependent Initialization Table.
	//=============================================================

	INIT_NV_REG,                 // Write register with Mask
	NV_PFB_BOOT_0,               // Strap Register for Memory
	0xFFFFFFFF,                  // AND Mask
	0x00000000,                  // OR Mask

	// Final value for running after sizing
	INIT_NV_REG,                 // Write register with Mask
	NV_PFB_CONFIG_1,
	0x00F00000,                  // AND Mask
	0x41016293,          // This value works for about everything
        
	//        INIT_MEM_RESTRICT
	//        018h                        // Strip to memory type (AND MASK)
	//        018h                        // Compare value = SDRAM
	//        
	//        INIT_RESUME                 // No more SDRAM stuff
	//                                         
	//        INIT_MEM_RESTRICT
	//        018h                        // Strip to memory type (AND MASK)
	//        010h                        // Compare value = SDRAM
	//             
	//        INIT_RESUME                 // No more SDRAM stuff
	        
	//------------------------------------------------------------
	//       Final Initialization Tables.
	//------------------------------------------------------------
	//
	//       Max allocated Final init space is
	//       INIT_ADDITION_REG_SPACE. This includes the FinalInit
	//       table and the FinalInit tables for all the FinalMem
	//       inits. OEM will utilize the BMP to maximize the
	//       utilization of addition table space.
	//

	// Comment to fix assembler bug

	//public  bmp_FinalInit
	//bmp_FinalInit:
	INIT_ZM_REG,
	NV_PRAMDAC_PLL_COEFF_SELECT,   // NV 32 bit address.
	0x00000500,                   // PROG_MPLL & PROG_NVPLL

	// Comment to fix assembler bug

	INIT_ZM_REG,
	NV_PBUS_DEBUG_2,
	0x000001F0,                // Per K^2 -  4/09/99
	//#ifdef OLD_BOARDS                                  
	//       000011F0h               // Per Jill S. -  5/12/98
	//#else   // OLD_BOARDS
	//       000001F1h               // Per Dave R. -  3/24/99
	//#endif  // OLD_BOARDS
                                               
	INIT_ZM_REG,
	NV_PBUS_DEBUG_3,
#ifdef VANTA
	NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_94MHZ | NV_PBUS_DEBUG_3_AGP_MAX_SIZE_32_BYTES,
#else   // VANTA
#ifdef  ULTRA
	NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_133MHZ | NV_PBUS_DEBUG_3_AGP_MAX_SIZE_32_BYTES,
#else   // ULTRA
	NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_120MHZ | NV_PBUS_DEBUG_3_AGP_MAX_SIZE_32_BYTES,
#endif  // ULTRA
#endif  // VANTA

	// Comment to fix assembler bug

	// Force real mode window default state to off
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	//NVRM_ACCESS,0x000,0x000,				// !! undefined
	NV_CIO_CRE_RMA__INDEX,0x000,0x000,

	// Make sure hsync and vsync are on (we only set them in DPMS calls)
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x01A,0x000,0x03F,
                       
	// Comment to fix assembler bug

	// Boot with TV off, and set for NTSC if it was on.
	// Also set horizontal pixel adjustment
	// Sets pixel format to VGA
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x028,0x000,0x000,

	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x033,0x000,0x000,         // Clear slave registers

	// Comment to fix assembler bug
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x021,0x000,0x0FA,         // Force Lock register to 0FAh
        
   // Clear TV device selection
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	//        02Bh,000h,000h        // 
	0x02B,0x007,0x000,         // Amask to retain Strap or override
        
        // Replace the above line with one of the following
        // to override the default settings for TV
	//       02Bh,000h,000h         // Force NTSC_M (Std)
	//       02Bh,000h,001h         // Force NTSC_J (BT869 Only)
	//       02Bh,000h,002h         // Force PAL-M (NTSC w/PAL encoding) (BT869 Only)
	//       02Bh,000h,003h         // Force PAL_BDGHI (Std)
	//       02Bh,000h,004h         // Force PAL-N (BT869 Only)
	//       02Bh,000h,005h         // Force PAL-NC (BT869 Only)
                                                           
	// Clear scratch to default state
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x02C,0x000,0x000,         // Clear all scratch bits for default
        
        // Old values for TV override selection
	//        02Ch,030h,000h         // Use this line to use straps
	//        02Ch,000h,010h          // Use this line to force NTSC Boot
	//        02Ch,000h,020h          // Use this line to force PAL Boot
	//        02Ch,000h,000h         // Use this line to force NOTV Boot
                                                       
        // Float DDC pins high (not touched during a mode set or save/restore state)
	//        INIT_INDEX_IO
	//        CRTC_INDEX_COLOR
	//        03Fh,000h,033h

        // Disable DDC
	//        INIT_INDEX_IO
	//        CRTC_INDEX_COLOR
	//        03Fh,000h,032h

	 // Comment to fix assembler bug

	INIT_DONE
	//        EOL //END OF LIST
};

// raw BMP table for TNT2 Pro 
U032 Nv05Pro_bmp_GenInitTbl[] = 
{
    0x00000065, 0x00000200, 0x01010100, 0x01110100,
    0x0000006e, 0x00101000, 0xffff4c43, 0x8000018c,
    0x0000007a, 0x000010e0, 0xf0f000f0, 0x0000007a,
    0x00001080, 0x04040000, 0x0000007a, 0x000010b0,
    0x0800bba7, 0x0000007a, 0x000010b4, 0xefe00000,
    0x0000007a, 0x000010b8, 0x88c98200, 0x0000007a,
    0x000010bc, 0xff988aa9, 0x00000079, 0x00680500,
    0x00003778, 0x00000079, 0x00680504, 0x00003fac,

    0x0000007a, 0x00100200, 0x00001114, 0x00000079,
    0x00680508, 0x00000e10, 0x0000007a, 0x001000c0,
    0x00000000, 0x0000007a, 0x00100080, 0x20002800,
    0x0000007a, 0x00100204, 0x41016293, 0x0000007a,
    0x00100300, 0x00000000, 0x0000007a, 0x00001084,
    0x000057c8, 0x00000063, 0x0000006e, 0x00100000,
    0xffffffff, 0x00000000, 0x0000006e, 0x00100204,
    0x00f00000, 0x42216293, 0x0000007a, 0x0068050c,

    0x00000500, 0x0000007a, 0x00001088, 0x000011f0,
    0x0000007a, 0x0000108c, 0x000000e1, 0x00000078,
    0x000003d4, 0x00000038, 0x00000000, 0x00000000,
    0x00000078, 0x000003d4, 0x0000001a, 0x00000000,
    0x0000003f, 0x00000078, 0x000003d4, 0x00000028,
    0x00000000, 0x00000000, 0x00000078, 0x000003d4,
    0x00000033, 0x00000000, 0x00000000, 0x00000078,
    0x000003d4, 0x00000021, 0x00000000, 0x000000fa,

    0x00000078, 0x000003d4, 0x0000002b, 0x00000007,
    0x00000000, 0x00000078, 0x000003d4, 0x0000002c,
    0x00000000, 0x00000000, 0x00000071
};
