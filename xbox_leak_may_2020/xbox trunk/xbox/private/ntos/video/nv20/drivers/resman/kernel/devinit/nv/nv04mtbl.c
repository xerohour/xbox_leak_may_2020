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
*	Header: nv04mtbl.c
*
*	Description:
*		This file contains the initialization code used to POST an NV4 device.
*	The init code was taken from the BIOS source file, nvinit.inc.
*
*	Revision History:
*		Original -- 2/99 Jeff Westerinen
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
*		7.	convert OR \ to |
*		8.	convert <num>h to 0x<num>
*		9 .	format -- at least line up data to left indent
*		10. comment out all table delimiters
*		11. remove bmp_ExtraInitTbl
*
******************************************************************************/

#include <nv4_ref.h>
#include <nvrm.h>
#include <bios.h>

U032 Nv04_bmp_GenInitTbl[] = 
{

	//IFNDEF NVIDIA_RESET
	//        // Reset engines
	//        INIT_ZM_REG
	//        NV_PMC_ENABLE      // NV 32 bit address.
	//        DRF_DEF(_PMC,_ENABLE,_PCRTC,_ENABLED) |
	//           DRF_DEF(_PMC,_ENABLE,_PPMI,_ENABLED) |
	//                DRF_DEF(_PMC,_ENABLE,_PFIFO,_ENABLED)
	//#endif // NVIDIA_RESET

	// Comment to fix assembler bug
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

	//#ifdef NVIDIA_RESET
	//        INIT_RESET
	//#else // NVIDIA_RESET
	//        // Reset engines
	//        INIT_ZM_REG
	//        NV_PMC_ENABLE      // NV 32 bit address.
	////#endif // NVIDIA_RESET
	//        DRF_DEF(_PMC,_ENABLE,_PFB,_ENABLED) |
	//                DRF_DEF(_PMC,_ENABLE,_PCRTC,_ENABLED) |
	//                DRF_DEF(_PMC,_ENABLE,_PPMI,_ENABLED) |
	//                DRF_DEF(_PMC,_ENABLE,_PFIFO,_ENABLED)

	// Comment to fix assembler bug

#ifdef DISABLE_SIDEBAND

	INIT_NV_REG,                 // Write register with Mask
	NV_PEXTDEV_BOOT_0,               // Strap Register for Memory
	0xFFFFFBFF,                  // AND Mask
	0x00000C00,                  // OR Mask
#else 
	INIT_NV_REG,                 // Write register with Mask
	NV_PEXTDEV_BOOT_0,               // Strap Register for Memory
	0xFFFFFBFF,                  // AND Mask
	0x00000800,                  // OR Mask

#endif  // DISABLE_SIDEBAND

	// Barry's memory fix for AGP
	INIT_ZM_REG,
	NV_PBUS_DEBUG_0,

	////        0E0F0000h    
	////        04050000h   // 7/23/98

	0x20F0000,    // 11/13/98 - New value from Dave Reed for 110Mhz MCLK

	// Comment to fix assembler bug

	// 13,500,000 MHz - MCLK = M:11:0B, N:163:A3, P:4:04 LOG2P:2 50 MHz
	// 14,318,180 MHz - MCLK = M:14:0E, N:196:C4, P:4:04 LOG2P:2 50 MHz
	INIT_PLL,
	NV_PRAMDAC_NVPLL_COEFF,       // Actually, this is NVPLL (680500)
#ifdef INTEL 
	8500,        // 85Mhz
#else                      
	9000,        // 90Mhz
#endif 


	// Comment to fix assembler bug

	// 13,500,000 MHz - MCLK = M:11:0B, N:163:A3, P:4:04 LOG2P:2 50 MHz
	// 14,318,180 MHz - MCLK = M:14:0E, N:196:C4, P:4:04 LOG2P:2 50 MHz
	INIT_PLL,
	NV_PRAMDAC_MPLL_COEFF,

#ifdef INTEL 
	10000,   // 100Mhz
#else                  
	11000,   // 110Mhz
#endif 


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
	//        20005000h            // IKOS value - turns on slow clock
	0x20000100,          // Per RussM on 10/14
#else   // IKOS                   
	0x20001D00,            // 7/23
#endif 

	// Comment to fix assembler bug

	INIT_ZM_REG,
	NV_PFB_CONFIG_1,
	0xC1016293,          // This value works for about everything

	// Comment to fix assembler bug

	INIT_ZM_REG,
	NV_PFB_RTL,
	0,

	// Comment to fix assembler bug

	// Per Dave Reed
	// Setup more optimal PCI interface
	INIT_ZM_REG,
	NV_PBUS_DEBUG_1,
	DRF_DEF(_PBUS,_DEBUG_1,_PCIM_THROTTLE,_DISABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIM_CMD,_SIZE_BASED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_PCIM_AGP,_IS_AGP) |			// !!!!!!!!!!!
		DRF_DEF(_PBUS,_DEBUG_1,_AGPM_CMD,_LP_ONLY) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_WRITE,_0_CYCLE) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_2_1,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RETRY,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RD_BURST,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_WR_BURST,_ENABLED) |
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_EARLY_RTY,_ENABLED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RMAIO,_DISABLED) |		// !!!!!!!!!!!
		DRF_DEF(_PBUS,_DEBUG_1,_PCIS_CPUQ,_ENABLED) |
		//DRF_DEF(_PBUS,_DEBUG_1,_SPARE1,_ZERO) |
		DRF_DEF(_PBUS,_DEBUG_1,_FBI_DIFFERENTIAL,_DISABLED) |	// !!!!!!!!!!!
		DRF_DEF(_PBUS,_DEBUG_1,_SPARE2,_ONE),

	// Comment to fix assembler bug

	INIT_COMPUTE_MEM,             // Compute frame buffer size

	//=============================================================
	//       Memory Dependent Initialization Table.
	//=============================================================

#ifdef  NV5
	INIT_NV_REG,                 // Write register with Mask
	NV_PFB_BOOT_0,               // Strap Register for Memory
	0xFFFFFF00,                  // AND Mask
	0x00000024,                  // OR Mask - 32MB SDRAM
	//        00000001Fh                  // OR Mask - 16MB SDRAM
#else               
	INIT_NV_REG,                 // Write register with Mask
	NV_PFB_BOOT_0,               // Strap Register for Memory
	0xFFFFFFFF,                  // AND Mask
	0x00000000,                  // OR Mask
#endif     
	INIT_MEM_RESTRICT,
	0x18,                        // Strip to memory type (AND MASK)
	0x18,                        // Compare value = SDRAM

	INIT_ZM_REG,
	NV_PFB_CONFIG_1,

#ifdef INTEL 
	0xC1016293,          // This value works for about everything
#else 
	0x31012132,                  // Set DRAM value
#endif   

	INIT_RESUME,                 // No more SDRAM stuff
	     
	INIT_MEM_RESTRICT,
	0x18,                        // Strip to memory type (AND MASK)
	0x10,                        // Compare value = SDRAM

	INIT_ZM_REG,
	NV_PFB_CONFIG_1,

#ifdef  INTEL 
	0xC1016293,          // This value works for about everything
#else 
	0x31012132,                  // Set DRAM value
#endif 

	INIT_RESUME,                 // No more SDRAM stuff

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

	INIT_ZM_REG,
	NV_PRAMDAC_PLL_COEFF_SELECT,   // NV 32 bit address.
	0x0000500,                   // PROG_MPLL & PROG_NVPLL

	// Comment to fix assembler bug

	// tbo
	INIT_ZM_REG,
	NV_PBUS_DEBUG_2,
	0x00011F0,               // Per Jill S. -  5/12

	INIT_ZM_REG,
	NV_PBUS_DEBUG_3,
	NV_PBUS_DEBUG_3_AGP_MAX_SIZE_32_BYTES,
	// tbo

	// Comment to fix assembler bug

	// Force real mode window default state to off
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	NV_CIO_CRE_RMA__INDEX,0x00,0x00,

	// Make sure hsync and vsync are on (we only set them in DPMS calls)
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x1A,0x00,0x3F,

	// Comment to fix assembler bug

	// Boot with TV off, and set for NTSC if it was on.
	// Also set horizontal pixel adjustment
	// Sets pixel format to VGA
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x28,0x00,0x00,

	// Comment to fix assembler bug
#ifdef NV5 
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x21,0x00,0xFA,         // Force Lock register to 0FAh
#endif  // NV5                        

	// Clear scratch to default state
	INIT_INDEX_IO,
	CRTC_INDEX_COLOR,
	0x2C,0x30,0x00,         // Use this line to use straps

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
