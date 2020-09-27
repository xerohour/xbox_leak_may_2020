// FILE:			library\hardware\mpeg2dec\specific\tl850reg.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		14.06.99
//
// PURPOSE:		TL850 MPEG-2 decoder register settings
//
// HISTORY:	   

#ifndef TL850REG_H
#define TL850REG_H

/* Register definitions for TL850 mpeg-2 decoder chip */

#define TL850_REGISTER_OFFSET 0xac000000
#define TL850_MEMORY_OFFSET	0xa8000000

//HIF internal register
#define HIF_BASE 					0x00000				//HIF base address
#define HIF_INT0_STAT_REG 		0x00000				//HIF interrupt status register
#define HIF_INT0_MASK_REG 		0x00004				//HIF interrupt mask register
#define HIF_INT1_STAT_REG 		0x00008				//HIF interrupt status register
#define HIF_INT1_MASK_REG 		0x0000C				//HIF interrupt mask register
#define HIF_RAM_TEST_REG 		0x00010				//HIF RAM test register
#define HIF_HINTED_ADDR_REG 	0x00014				//

#define VSC_BASE 					0x01000				//video channel base address
#define VSC_CTRL_REG 			0x01000				//specifies the operation of video port
#define VSC_STAT_REG 			0x01004				//provides status information on the video port
#define VSC_INT_EN_REG 			0x01008				//provides the interrupt enables for the VSC
#define VSC_INT_STAT_REG 		0x0100C				//interrupt status register
#define VSC_MIF_REG 				0x01010				//memory interface control register
#define VSC_DDA_REG 				0x01014				//specifies the horizontal scaling factors
#define VSC_HS_REG 				0x01018				//horizontal scaler control register
#define VSC_VBI0_REG 			0x0101C				//defines the field 0 VBI data capture operation
#define VSC_VBI1_REG 			0x01020				//defines the field 1 VBI data capture operation
#define VSC_VBUF_SADR_REG 		0x01024				//start address of the VBI field buffer in external memory
#define VSC_VBUF_WADR_REG 		0x01028				//current write address of the VBI field buffer in external memory

#define AMR_BASE 					0x02000

#define APU_BASE 					0x03000				//APU base address
#define APU_IOC_CONFIG_REG 	0x03000				//input, output and clock configuration register
#define APU_CPB_CONFIG_REG 	0x03004				//capture and playback configuration register
#define APU_IEC_CONFIG_REG 	0x03008				//IEC958 configuration register
#define APU_IEC_CSB1_REG 		0x0300C				//IEC958	channel status buffer registers
#define APU_IEC_CSB2_REG 		0x03010				//IEC958 channel status buffer registers
#define APU_INT_CONFIG_REG 	0x03014				//interrupt configuration registers
#define APU_INT_STATUS_REG 	0x03018				//interrupt status register
#define APU_RESERVED_REG 		0x0301C				
#define APU_CPB_CBA_REG 		0x03020				//capture/playback begin address register
#define APU_CPB_CEA_REG 		0x03024				//capture/playback end address register
#define APU_CPB_CWA_REG 		0x03028				//capture/playback write address register
#define APU_CPB_CRA_REG 		0x0302C				//capture/playback read address register
#define APU_CPB_CMA_REG 		0x03030				//capture/playback match adress register
#define APU_IAU_IBA0_REG 		0x03034				//internal audio begin address registers
#define APU_IAU_IRA0_REG 		0x03038				//internal audio read address registers
#define APU_IAU_IEA0_REG 		0x0303C				//internal audio end address registers
#define APU_IAU_IBA1_REG 		0x03040				//internal audio begin address registers
#define APU_IAU_IRA1_REG 		0x03044				//internal audio read address registers
#define APU_IAU_IEA1_REG 		0x03048				//internal audio end address registers
#define APU_IAU_IBA2_REG 		0x0304C				//internal audio begin address registers
#define APU_IAU_IRA2_REG 		0x03050				//internal audio read address registers
#define APU_IAU_IEA2_REG 		0x03054				//internal audio end address registers
#define APU_MIX_CONFIG_REG 	0x03058				//mix configuration register
#define APU_MIX_XFC0_REG 		0x0305C				//cross-fade coefficient registers
#define APU_MIX_XFC1_REG 		0x03060				//cross-fade coefficient registers
#define APU_MIX_XFC2_REG 		0x03064				//cross-fade coefficient registers
#define APU_MIX_XFC3_REG 		0x03068				//cross-fade coefficient registers
#define APU_MIX_XFC4_REG 		0x0306C				//cross-fade coefficient registers
#define APU_MIX_XFC5_REG 		0x03070				//cross-fade coefficient registers
#define APU_MIX_XFC6_REG 		0x03074				//cross-fade coefficient registers
#define APU_MIX_XFC7_REG 		0x03078				//cross-fade coefficient registers
#define APU_MIX_XFC8_REG 		0x0307C				//cross-fade coefficient registers

#define BLT_BASE 					0x04000				//BLT base address
#define BLT_CHAN_S0_REG 		0x04000				//channel S0 configuration register
#define BLT_CHAN_S1_REG 		0x04004				//channel S1 configuration register
#define BLT_CHAN_D_REG 			0x04008				//destination channel configuration register
#define BLT_CMD_REG 				0x0400C				//BitBlt command register
#define BLT_STAT_REG 			0x04010				//status register
#define BLT_INTR_STAT_REG 		0x04014				//interrupt status register
#define BLT_LEFT_ADDRESS_S0_REG		0x04018		//channel S0, left address register of first line address processed
#define BLT_RIGHT_ADDRESS_S0_REG		0x0401C		//channel S0, right address register of first line address processed
#define BLT_LEFT_ADDRESS_S1_REG 		0x04020		//channel S1, left address register of first line address processed
#define BLT_RIGHT_ADDRESS_S1_REG 	0x04024		//channel S1, right address register of first line processed
#define BLT_LEFT_ADDRESS_D_REG 		0x04028		//channel D, left address register of first line processed
#define BLT_RIGHT_ADDRESS_D_REG 		0x0402C		//channel D, right address register of first line processed
#define BLT_S0_PITCH_SIZ_REG 	0x04030				//pitch size for channel S0
#define BLT_S1_PITCH_SIZ_REG 	0x04034				//pitch size for channel S1
#define BLT_D_PITCH_SIZ_REG 	0x04038				//pitch size for channel D
#define BLT_H_W_REG 				0x0403C				//bitmap size
#define BLT_LINK_ADDR_REG 		0x04040				//link list address
#define BLT_S0_FG_COLOR_REG 	0x04044				//foreground register
#define BLT_S0_BG_REG 			0x04048				//background register
#define BLT_S1_FG_COLOR_REG 	0x0404C				//global color for S0
#define BLT_S1_BG_REG 			0x04050				//global color for S1
#define BLT_ALPHA_REG 			0x04054				//alpha parameters
#define BLT_PROG_REG 			0x04058				//
#define BLT_PACE_LINE_REG 		0x0405C				//

#define PLL_SYSCLK_REG 			0x05000				//system clock PLL register
#define PLL_MIFCLK_REG 			0x05400				//MIF clock PLL register
#define PLL_DPCCLK_REG 			0x05800				//display processor clock PLL register

//delay line (DLL) registers
#define DLL_BASE 					0x05C00				//dll base address
#define DLL_IRAM 					0x05C00				//instruction RAM
#define DLL_FN_OFST_REG 		0x05D00				//fine offset delay registers
#define DLL_FN_REG 				0x05D04				//fine delay registers
#define DLL_GR_REG 				0x05D08				//gross delay registers
#define DLL_OFST_REG 			0x05D0C				//offset delay registers
#define DLL_RST_REG 				0x05D10				//reset registers
#define DLL_PCLK_DLY_REG 		0x05D14				//pclk delay registers
//end delay line registers

#define MCE_BASE 					0x06000		
#define MCE_CTRL_REG 			0x06000
#define MCE_STAT_REG 			0x06004
#define MCE_IE_REG 				0x06008
#define MCE_IS_REG 				0x0600C
#define MCE_IS_ALIAS_REG 		0x06010
#define MCE_SYNC_CTRL_REG 		0x06014
#define MCE_SYNC_STAT_REG 		0x06018
#define MCE_FNSSC_REG 			0x0601C
#define MCE_FSSC_REG 			0x06020				
#define MCE_VLD_CTRL_REG 		0x06024
#define MCE_TOC_RDATA_REG 		0x06028
#define MCE_Q_WDATA_REG 		0x0602C
#define MCE_MPIC_REG 			0x06030
#define MCE_CH_SADR_REG 		0x06034
#define MCE_CH_EADR_REG 		0x06038
#define MCE_CH_RADR_REG 		0x0603C

#define VPIP_BASE 				0x07000
#define VPIP_CONFIG_REG 		0x07000
#define VPIP_INT_EN_REG 		0x07004
#define VPIP_INT_STAT_REG 		0x07008
#define VPIP_PTR_REG 			0x0700C
#define VPIP_POS_X_REG 			0x07010
#define VPIP_STATUS_REG 		0x07014
#define VPIP_SIZE_REG 			0x07018
#define VPIP_ROW_CONFIG_REG 	0x0701C
#define VPIP_DDA_FACTOR_REG 	0x07020
#define VPIP_COEFF_ADDR_REG 	0x07024
#define VPIP_COEFF_DATA_REG 	0x07028
#define VPIP_ROW_CONFIG2_REG 	0x0702C
#define VPIP_STATUS2_REG 		0x07030

#define HPIP_BASE 				0x08000
#define HPIP_CONFIG_REG 		0x08000
#define HPIP_STATUS_REG 		0x08004
#define HPIP_INT_EN_REG 		0x08008
#define HPIP_INT_STAT_REG 		0x0800C
#define HPIP_PTR_REG 			0x08010
#define HPIP_X_FACTOR_REG 		0x08014
#define HPIP_Y_FACTOR_REG 		0x08018
#define HPIP_DDA_FACTOR_REG 	0x0801C
#define HPIP_COEFF_ADDR_REG 	0x08020
#define HPIP_COEFF_DATA_REG 	0x08024
#define HPIP_CMODEL_REG 		0x08028

//display processor architecture (DPC) registers
#define DPC_BASE 					0x10000				//DPC base address
#define DPC_CONFIG_REG 			0x10000				//DPC configuration register
#define DPC_STATUS_REG 			0x10004				//DPC status 
#define DPC_INT_EN_REG 			0x10008				//DPC interrupt enable
#define DPC_INT_STAT_REG 		0x1000C				//DPC interrupt status register
#define DPC_DISP_COUNT_REG 	0x10010				//DPC display count
#define DPC_SYNC_PARAM1_REG 	0x10014				//DPC sync parameter 1
#define DPC_SYNC_PARAM2_REG 	0x10018				//DPC sync parameter 2
#define DPC_SYNC_PARAM3_REG 	0x1001C				//DPC sync parameter 3
#define DPC_SYNC_PARAM4_REG 	0x10020				//DPC sync parameter 4
#define DPC_SYNC_DELAY1_REG 	0x10024				//DPC sync delay 1
#define DPC_SYNC_DELAY2_REG 	0x10028				//DPC sync delay 2
#define DPC_BG_CUMLUMA_REG 	0x1002C				//accumulated luma of background outside of the overlay region
#define DPC_OVL_CUMLUMA_REG 	0x10030				//accumulated luma of active region of overlay after compositing with the background
#define DPC_PDISP_STALL_REG 	0x10034			   //used to control parameters of stall circuit for primary display FIFO	
#define DPC_BG_CONFIG_REG 		0x10040				//background graphics and video configuration register
#define DPC_BG_COLOR_REG 		0x10044				//default background graphics color in 24-bit YCbCr
#define DPC_BVDOSRC_CFG1_REG 	0x10048				//starting point of the region of interest within the memory map
#define DPC_BVDOSRC_CFG2_REG 	0x1004C				//configuration of field memory buffers
#define DPC_BVDOWIN_CFG1_REG 	0x10050				//configuration of video window
#define DPC_BVDOWIN_CFG2_REG 	0x10054				//configuration of video window
#define DPC_BVDO_SC_CFG1_REG 	0x10058				//left and right positions of the center region within a scan line
#define DPC_BVDO_SC_CFG2_REG 	0x1005C				//top and bottom line positions of the central display region within a frame or field
#define DPC_BVDO_HS_CTL1_REG 	0x10060				//horizontal scaling parameters for central region of the video window
#define DPC_BVDO_HS_CTL2_REG 	0x10064				//horizontal scaling parameters for outside region of the video window
#define DPC_BVDO_VS_CTL1_REG 	0x10068				//vertical scaling parameters for central region of the video window
#define DPC_BVDO_VS_CTL2_REG 	0x1006C				//vertical scaling parameters for outside region of the video window
#define DPC_BVDO_IVPH_REG 		0x10070				//initial phases or steps for the vertical DDA
#define DPC_OVL_CONFIG_REG 	0x10080				//DPC overlay configuration register
#define DPC_OVL_DLIST_SA_REG 	0x10084				//overlay display list start word address
#define DPC_OVL_SA_REG 			0x10088				//overlay plane starting word address
#define DPC_OVL_SIZE_REG 		0x1008C				//size register of overlay region of interest
#define DPC_OVL_POS_REG 		0x10090				//position of the currently active overlay window
#define DPC_GALPHA_IBASE_REG 	0x10094				//global alpha and index base register
#define DPC_OVL_MAX_KEY_REG 	0x10098				//maximum register used for color keying and the matching register for index keying
#define DPC_OVL_MIN_KEY_REG 	0x1009C				//minimum register used for color keying
#define DPC_OVL_FLTR_COEF_REG 0x100A0				//coefficients for the anti-flicker filter for overlay
#define DPC_OVL_DLIST1_REG 	0x100A4				//
#define DPC_OVL_DLIST2_REG 	0x100A8				//
#define DPC_AUX_CONFIG_REG 	0x100C0				//configuration register for auxiliary video output
#define DPC_AUX_POS_REG 		0x100C4				//starting position of the auxiliary video scaling window 
#define DPC_AUX_HS_CTL_REG 	0x100C8				//horizontal scaling parameters for auxiliary video output
#define DPC_AUX_VS_CTL_REG 	0x100CC				//vertical scaling parameters for auxiliary video output
#define DPC_AUX_PROG1_REG 		0x100D0				//part 1 of 3 of the programmable video timing register
#define DPC_AUX_PROG2_REG 		0x100D4				//part 2 of 3 of the programmable video timing register
#define DPC_AUX_PROG3_REG 		0x100D8				//part 3 of 3 of the programmable video timing register
#define DPC_CURS_SA_REG 		0x10100				//cursor starting word address in dynamic memory
#define DPC_CURS_POS_REG 		0x10104				//
#define DPC_CURS_OFFSET_REG 	0x10108				//
#define DPC_CURSCLUT0_REG 		0x10140				//
#define DPC_CURSCLUT1_REG 		0x10144				//
#define DPC_CURSCLUT2_REG 		0x10148
#define DPC_CURSCLUT3_REG 		0x1014C
#define DPC_CURSCLUT4_REG 		0x10150
#define DPC_CURSCLUT5_REG 		0x10154
#define DPC_CURSCLUT6_REG 		0x10158
#define DPC_CURSCLUT7_REG 		0x1015C
#define DPC_CURSCLUT8_REG 		0x10160
#define DPC_CURSCLUT9_REG 		0x10164
#define DPC_CURSCLUT10_REG 	0x10168
#define DPC_CURSCLUT11_REG 	0x1016C
#define DPC_CURSCLUT12_REG 	0x10170
#define DPC_CURSCLUT13_REG 	0x10174
#define DPC_CURSCLUT14_REG 	0x10178
#define DPC_CURSCLUT15_REG 	0x1017C
#define DPC_MEM_ADDR_REG 		0x10180				//memory address pointers for the next memory data access
#define DPC_MEM_DATA_REG 		0x10184				//
#define DPC_GPIO_EN_REG 		0x10190				//set to enable selected pins as GPIO pins
#define DPC_GPIO_OUT_EN_REG 	0x10194				//set pins selected by DPC_GPIO_EN register to output mode
#define DPC_GPIO_OUTDATA_REG 	0x10198				//set pins selected by DPC_GPIO_EN register to bit value
#define DPC_GPIO_INDATA_REG 	0x1019C				//read values of 28 pins
#define DPC_SPG_CONFIG_REG		0x101A0
#define DPC_SPG_HCONFIG_REG	0x101A4
#define DPC_SPG_VCONFIG_REG	0x101A8
#define DPC_SPG_PCONFIG_REG	0x101AC
//end display processor architecture registers

#define MCU_BASE 					0x18000
#define MCU_RF0 					0x1D000
#define MCU_RF1 					0x1D004
#define MCU_RF2 					0x1D008
#define MCU_RF3 					0x1D00C
#define MCU_RF4 					0x1D010
#define MCU_RF5 					0x1D014
#define MCU_RF6 					0x1D018
#define MCU_RF7 					0x1D01C
#define MCU_RF8 					0x1D020
#define MCU_RF9 					0x1D024
#define MCU_RF10 					0x1D028
#define MCU_RF11 					0x1D02C
#define MCU_RF12 					0x1D030
#define MCU_RF13 					0x1D034
#define MCU_RF14 					0x1D038
#define MCU_RF15 					0x1D03C
#define MCU_HALT_REG 			0x1D800
#define MCU_PC 					0x1D804
#define MCU_COND_CODE 			0x1D808
#define MCU_SPL_REG 				0x1D80C
#define MCU_RAM_SLICE_VERTICAL_POSITION	0x1C000
#define MCU_RAM_MBAINC 			0x1C004
#define MCU_RAM_MOTION_TYPE 	0x1C008
#define MCU_RAM_MVECTOR 		0x1C00C
#define MCU_RAM_DELTA_000 		0x1C010
#define MCU_RAM_DELTA_001 		0x1C014
#define MCU_RAM_DELTA_010 		0x1C018
#define MCU_RAM_DELTA_011 		0x1C01C
#define MCU_RAM_DELTA_100 		0x1C020
#define MCU_RAM_DELTA_101 		0x1C024
#define MCU_RAM_DELTA_110 		0x1C028
#define MCU_RAM_DELTA_111 		0x1C02C
#define MCU_RAM_PICTURE_CODING_STRUCTURE 	0x1C2C0
#define MCU_RAM_PICTURE_FIELD 0x1C2C4
#define MCU_RAM_MV_FRAME 		0x1C2C8
#define MCU_RAM_MB_WIDTH 		0x1C2CC
#define MCU_RAM_MBAMAX 			0x1C2D0
#define MCU_RAM_RANGE_00 		0x1C2D4
#define MCU_RAM_RANGE_01 		0x1C2D8
#define MCU_RAM_RANGE_10 		0x1C2DC
#define MCU_RAM_RANGE_11 		0x1C2E0
#define MCU_RAM_FBUF_BASE_FWD 0x1C2E4
#define MCU_RAM_FBUF_BASE_BWD 0x1C2E8
#define MCU_RAM_FBUF_BASE_DST 0x1C2EC
#define MCU_RAM_PICTURE_START 0x1C2F0

//Transport stream demux (TSD) configuration registers
#define TSD_BASE 					0x20000
#define TSD_IRAM_0 				0x20000
#define TSD_IRAM_1 				0x20004
#define TSD_IRAM_16 				0x20040
#define TSD_IRAM_4K_1 			0x21FFC
#define TSD_IRAM_4K 				0x22000
#define TSD_IRAM_5K_1 			0x227FC
#define TSD_DRAM_0 				0x24000
#define TSD_DRAM_1 				0x24004
#define TSD_DRAM_16 				0x24040
#define TSD_DRAM_2K_1 			0x24FFC
#define TSD_DRAM_2K 				0x25000
#define TSD_DRAM_3K_1 			0x257FC
#define TSD_DRAM_4K_1 			0x25FFC
#define TSD_MCU_RF0 				0x26000
#define TSD_MCU_RF1 				0x26004
#define TSD_MCU_RF2 				0x26008
#define TSD_MCU_RF3 				0x2600C
#define TSD_MCU_RF4 				0x26010
#define TSD_MCU_RF5 				0x26014
#define TSD_MCU_RF6 				0x26018
#define TSD_MCU_RF7 				0x2601C
#define TSD_MCU_RF8 				0x26020
#define TSD_MCU_RF9 				0x26024
#define TSD_MCU_RF10 			0x26028
#define TSD_MCU_RF11 			0x2602C
#define TSD_MCU_RF12 			0x26030
#define TSD_MCU_RF13 			0x26034
#define TSD_MCU_RF14 			0x26038
#define TSD_MCU_RF15 			0x2603C
#define TSD_MCU_HALT_REG 		0x26800
#define TSD_MCU_PC 				0x26804
#define TSD_MCU_COND_CODE 		0x26808
#define TSD_MCU_SPL_REG 		0x2680C
#define TSD_DMA_RD_CONFIG 		0x27000
#define TSD_DMA_WR_ADD 			0x27004
#define TSD_IO_BYTELIM_CNT 	0x27800
#define TSD_STC_CNT_LM 			0x27804				//STC counter low
#define TSD_STC_CNT_H 			0x27808				//STC counter high
#define TSD_CON_VCXO 			0x27810				
#define TSD_INT_STATUS 			0x27814				//TSD internal status
//end TSD configuration registers

//Memory interface (MIF) base address
#define MIF_BASE 					0x28000				
#define MIF_CONFIG_REG 			0x28000				//MIF configuration register - specifies the basic memory configuration parameters
// MIF field buffer 
#define MIF_FLD_DSCR_0_REG		0x28040				
#define MIF_FLD_DSCR_1_REG		0x28044
#define MIF_FLD_DSCR_2_REG 	0x28048
#define MIF_FLD_DSCR_3_REG 	0x2804C
#define MIF_FLD_DSCR_4_REG 	0x28050
#define MIF_FLD_DSCR_5_REG 	0x28054
#define MIF_FLD_DSCR_6_REG 	0x28058
#define MIF_FLD_DSCR_7_REG 	0x2805C
#define MIF_FLD_DSCR_8_REG 	0x28060
#define MIF_FLD_DSCR_9_REG 	0x28064
#define MIF_FLD_DSCR_10_REG 	0x28068
#define MIF_FLD_DSCR_11_REG 	0x2806C
#define MIF_FLD_DSCR_12_REG 	0x28070
#define MIF_FLD_DSCR_13_REG 	0x28074
#define MIF_FLD_DSCR_14_REG 	0x28078
#define MIF_FLD_DSCR_15_REG 	0x2807C
// MIF status for client N
// This register specifies the status of the memory operations for a given client. 
#define MIF_STATUS_HIF_RD 		0x28080
#define MIF_STATUS_APU_RD 		0x28084
#define MIF_STATUS_TOC 			0x28088
#define MIF_STATUS_BLT0_RD 	0x2808C
#define MIF_STATUS_VPIP_RD 	0x28090
#define MIF_STATUS_BVDO1 		0x28094
#define MIF_STATUS_CLUT 		0x28098
#define MIF_STATUS_MC_RD 		0x2809C
#define MIF_STATUS_OVL1 		0x280A0
#define MIF_STATUS_OVL2 		0x280A4
#define MIF_STATUS_BLT1_RD 	0x280A8
#define MIF_STATUS_BLT2_RD 	0x280AC
#define MIF_STATUS_BVDO2 		0x280B0
#define MIF_STATUS_BVDO3 		0x280B4
#define MIF_STATUS_CURS 		0x280B8
#define MIF_STATUS_DL 			0x280BC
#define MIF_STATUS_HIF_WR 		0x280C0
#define MIF_STATUS_APU_WR 		0x280C4
#define MIF_STATUS_TRANS 		0x280C8
#define MIF_STATUS_BLT_WR 		0x280CC
#define MIF_STATUS_VPIP_WR 	0x280D0
#define MIF_STATUS_HPIP 		0x280D4
#define MIF_STATUS_VSC 			0x280D8
#define MIF_STATUS_MC_WR 		0x280DC

/*********************************************************************/
/*																							*/
/*							Bit field definitions									*/
/*																							*/
/*********************************************************************/
/* DPC Configuration Register */
#define DPC_CURRENT_FIELD 		0
#define DPC_BVDO_EN 				1
#define DPC_OVL_EN 				2
#define DPC_CURS_EN 				3
#define DPC_AUX_EN 				4
#define DPC_PRM_VOUT_EN 		5
#define DPC_PRM_YCCOUT 			6
#define DPC_VDAC_EN 				7
#define DPC_VDAC_TPAT_EN 		8
#define DPC_VDAC_BLNK_SEL 		9
#define DPC_VDAC_SYNC_EN 		10
#define DPC_DIGITAL_OUT 		11, 2
#define DPC_STRM_DACSEL 		13, 2
#define DPC_DIGITAL_IN 			15
#define DPC_CURS_MODE 			16
#define DPC_PRM_CLIP_MODE 		17
#define DPC_AUX_CLIP_MODE 		18
#define DPC_CONFIG_RESERVE 	19, 13
/* end DPC Configuration Register */

/* DPC Status Register (DPC_STATUS_REG) */
#define DPC_PRM_VOUT_H 			0
#define DPC_PRM_VOUT_V 			1
#define DPC_PRM_VOUT_F 			2
#define DPC_AUX_VOUT_H 			3
#define DPC_AUX_VOUT_V 			4
#define DPC_AUX_VOUT_F 			5
#define DPC_BG_UFLOW 			6
#define DPC_OVL_UFLOW 			7
#define DPC_HCOUNT 				8, 11
#define DPC_VCOUNT 				20, 11
/* end DPC Status Register */

/* DPC Interrupt Enable Register (DPC_INT_EN_REG)*/
#define DPC_SYNCLOSS_INT 		0
#define DPC_SYNCACQ_INT 		1
#define DPC_HSYNCCNT_INT 		2
#define DPC_VSYNCCNT_INT 		3
#define DPC_HVSYNCCNT_INT 		4
#define DPC_EOF0_INT 			5
#define DPC_EOF1_INT 			6
#define DPC_DISPL_INT 			7
#define DPC_MEM_UF_INT 			8
#define DPC_AUX_UF_INT 			9
#define DPC_AUX_OF_INT 			10
#define DPC_IDX_OF_INT			11
/* end Interrupt Enable Register */

/* DPC Interrupt Status Register (DPC_INT_STAT_REG) */
#define DPC_INT_STAT 			0, 10
#define DPC_DIPL_INT_CNT 		24, 8
/* end Interrupt Status Register */

/* DPC Display Count Register (DPC_DISP_COUNT_REG) */
#define DPC_LINE_COUNT 			0, 11
#define DPC_FRAME_COUNT 		16, 8
/* end Display Count Register */

/* DPC Synchronization Parameter 1 Register (DPC_SYNC_PARAM1_REG) */
#define DPC_PRM_CLKSEL 			0
#define DPC_TRS_WIDTH 			1
#define DPC_SYNC_MODE 			2
#define DPC_SYNC_PROGSS 		3
#define DPC_SCAN_REPEAT 		4, 2
#define DPC_VSYNC_DELAY 		6, 10
#define DPC_PIX_PER_LINE 		16, 11
#define DPC_SYNCCODE_EN 		27
#define DPC_SYNCSIG_EN 			28
#define DPC_SYNCSIG_HVIND 		29
#define DPC_OUTPUT_FIELD 		30
#define DPC_DISP_RESET 			31
/* end Synchronization Parameter 1 Register */

/* DPC Synchronization Parameter 2 Register (DPC_SYNC_PARAM1_REG) */
#define DPC_FRONT_PORCH 		0, 9
#define DPC_HSYNC_INVERSE 		9
#define DPC_VSYNC_INVERSE 		10
#define DPC_HSYNC_WIDTH 		12, 7
#define DPC_BACK_PORCH 			20, 9
/* end Synchronization Parameter 2 Register */

/* DPC Synchronization Parameter 3 Register (DPC_SYNC_PARAM3_REG) */
#define DPC_FIELD0_VSYNC 		0, 3
#define DPC_FIELD0_TOP 			4, 8
#define DPC_FIELD0_ACTIVE 		12, 11
#define DPC_FIELD0_BOTTOM 		24, 4
#define DPC_FIELD0_NTSC 		28
/* end Synchronization Parameter 3 Register */

/* DPC Synchronization Parameter 4 Register (DPC_SYNC_PARAM4_REG) */
#define DPC_FIELD1_VSYNC 		0, 3
#define DPC_FIELD1_TOP 			4, 8
#define DPC_FIELD1_ACTIVE 		12, 11
#define DPC_FIELD1_BOTTOM 		24, 4
/* end Synchronization Parameter 4 Register */

/* DPC Synchronization Delay 1 Register (DPC_SYNC_DELAY1_REG) */
#define DPC_SYNC_DELAY_V1 		0, 22
#define DPC_SDELAY_EN1 			24
#define DPC_LINES_TO_SYNC 		28, 4 
/* end Synchronization Delay 1 Register */

/* DPC Synchronization Delay 2 Register (DPC_SYNC_DELAY2_REG) */
#define DPC_SYNC_DELAY_V2 		0, 12
#define DPC_SDELAY_EN2 			24
/* end Synchronization Delay 2 Register */

/* DPC Accumulated luma of Background Register (DPC_BG_CUMLUMA_REG) */
#define DPC_BG_CUMLUMA 			0, 28
/* end Accumulated luma of Background Register */

/* DPC Accumulated Luma of Active Region Register (DPC_OVL_CUMLUMA_REG) */
#define DPC_OVL_CUMLUMA 		0, 28
/* end Accumulated Luma of Active Region Register */

/* DPC Stall Circuit Register (DPC_PDISP_STALL_REG) */
#define DPC_STALL_CLOSE 		0, 8
#define DPC_STALL_FAR 			8, 8
#define DPC_STALL_CYCLES 		16, 5
/* end Stall Circuit Register */

/* DPC Background Register */
/* DPC Background Configuration Register (DPC_BG_CONFIG_REG) */
#define DPC_BVDO_FLD_CFG 		0, 2
#define DPC_BVDO_MPEG1 			2
#define DPC_BVDO_FMT422 		3
#define DPC_BVDO_HFLR_EN 		4
#define DPC_BVDO_VFLR_EN 		5
#define DPC_BVDO_VLUMOFF 		6
#define DPC_BVDO_NUI_EN 		7
#define DPC_BVDO_VFLR_TAP 		8, 4
#define DPC_THROUGHPUT 			12, 3
#define DPC_BG_BWMODE 			15
/* end Background Configuration Register */

/* DPC Background Color Register (DPC_BG_COLOR_REG) */
#define DPC_BG_COLOR 			0, 24
/* end Background Configuration Register */

/* DPC Configuration 1 Register (DPC_BVDOSRC_CFG1_REG) */
#define DPC_BVDO_TOPBASE 		0, 4
#define DPC_BVDO_BOTBASE 		4, 4
#define DPC_BVDOSRC_XPOS 		8, 10
#define DPC_BVDOSRC_YPOS 		20, 11
/* end Configuration 1 Register */

/* DPC Configuration 2 Register (DPC_BVDOSRC_CFG2_REG) */
#define DPC_BVDOSRC_WD 			0, 11
#define DPC_BVDOSRC_HT 			16, 11
/* end Configuration 2 Register */

/* DPC Configuration 1 Register (DPC_BVDOWIN_CFG1_REG) */
#define DPC_BVDO_FIRST_PIX 	0, 4
#define DPC_BVDOWIN_XPOS 		8, 11
#define DPC_BVDOWIN_YPOS 		20, 11
/* end Configuration 1 Register */

/* DPC Configuration 2 Register (DPC_BVDOWIN_CFG2_REG) */
#define DPC_BVDOWIN_WD 			0, 11
#define DPC_BVDOWIN_HT 			16, 11
/* end Configuration 1 Register */

/* DPC Configuration 1 Register (DPC_BVDO_SC_CFG1_REG) */
#define DPC_BVDO_L_XPOS 		0, 12
#define DPC_BVDO_R_XPOS 		16, 12
/* end Configuration 1 Register */

/* DPC Configuration 2 Register (DPC_BVDO_SC_CFG2_REG) */
#define DPC_BVDO_T_YPOS 		0, 12
#define DPC_BVDO_B_YPOS 		16, 12
/* end Configuration 2 Register */

/* DPC Horizontal Control 1 Register (DPC_BVDO_HS_CTL1_REG) */
#define DPC_BVDO_HFS1 			4, 4
#define DPC_BVDO_HFN1 			8, 11
#define DPC_BVDO_HFM1 			20, 12
/* end Horizontal Control 1 Register */
		
/* DPC Horizontal Control 2 Register (DPC_BVDO_HS_CTL2_REG) */
#define DPC_BVDO_HFS2 			4, 4
#define DPC_BVDO_HFN2 			8, 11
#define DPC_BVDO_HFM2 			20, 12
/* end Horizontal Control 2 Register */

/* DPC Vertical Control 1 Register (DPC_BVDO_VS_CTL1_REG) */
#define DPC_BVDO_VFS1 			3, 5
#define DPC_BVDO_VFN1 			8, 11
#define DPC_BVDO_VFM1 			20, 12
/* end Vertical Control 1 Register */

/* DPC Vertical Control 2 Register (DPC_BVDO_VS_CTL2_REG) */
#define DPC_BVDO_VFS2 			3, 5
#define DPC_BVDO_VFN2 			8, 11
#define DPC_BVDO_VFM2 			20, 12
/* end Vertical Control 2 Register */

/* DPC Initial Phases Register (DPC_BVDO_IVPH_REG) */
#define DPC_BVDO_IVPH3BOT 		20, 4 
#define DPC_BVDO_IVPH3TOP 		16, 4
#define DPC_BVDO_IVPH2BOT 		12, 4
#define DPC_BVDO_IVPH2TOP 		8, 4
#define DPC_BVDO_IVPH1BOT 		4, 4
#define DPC_BVDO_IVPH1TOP 		0, 4
/* end Initial Phases Register */

/* DPC Configuration Registers for Overlay */
/* DPC Overlay Configuration Register (DPC_OVL_CONFIG_REG) */
#define DPC_COLOR_KEY_EN 		0
#define DPC_INDEX_KEY_EN 		1
#define DPC_GALPHA_EN 			2
#define DPC_OVL_OPAQUE 			3
#define DPC_GALPHA_MODE1 		4, 2
#define DPC_GALPHA_MODE2 		6, 2
#define DPC_HFLTR_EN 			8
#define DPC_VFLTR_EN 			9
#define DPC_VFLTR_TH_EN 		10
#define DPC_OVL_ALPHA_PM 		11
#define DPC_VFLTR_THRESH 		12, 4
#define DPC_OVL_YCCUB 			16
#define DPC_OVL_BWMODE 			17
/* end Overlay Configuration Register */

/* DPC Overlay Display List Start Register (DPC_OVL_DLIST_SA_REG) */
#define DPC_OVL_DLIST_SA 		4, 22
/* end Overlay Display List Start Register */

/* DPC Overlay Plane Starting Address Register (DPC_OVL_SA_REG) */
#define DPC_OVL_SA 				4, 22
/* end Overlay Plane Starting Address Register */

/* DPC Overlay Region Of Interest Register (DPC_OVL_SIZE_REG) */
#define DPC_OVL_PITCH 			0, 10
#define DPC_OVL_WIDTH 			10, 11
#define DPC_OVL_HEIGHT 			21, 11
/* end Overlay Region Of Interest Register */

/* DPC Overlay Position Register (DPC_OVL_POS_REG) */
#define DPC_OVL_XPOS 			0, 11
#define DPC_OVL_YPOS 			16, 11
/* end Overlay Position Register */

/* DPC Alpha and Index Base Register (DPC_GALPHA_IBASE_REG) */
#define DPC_OVL_CONTRST 		8, 8
#define DPC_OVL_BRIGHT 			16, 8
#define DPC_INDEX_BASE 			28, 4
/* end Alpha and Index Base Register */

/* DPC Maximum Color Keying Register (DPC_OVL_MAX_KEY_REG) */
#define DPC_MAX_BKEY 			0, 8
#define DPC_MAX_GKEY 			8, 8
#define DPC_MAX_RKEY 			16, 8
/* end Maximum Color Keying Register */

/* DPC Minimum Color Keying Register (DPC_OVL_MIN_KEY_REG) */
#define DPC_MIN_BKEY 			0, 8
#define DPC_MIN_GKEY 			8, 8
#define DPC_MIN_RKEY 			16, 8
/* end Minimum Color Keying Register */

/* (DPC_OVL_FLTR_COEF_REG) */
#define DPC_LUMA_COEFF0 		0, 6
#define DPC_LUMA_COEFF1 		8, 5
#define DPC_ALPHA_COEFF0 		16, 6
#define DPC_ALPHA_COEFF1 		24, 5

/* (DPC_OVL_DLIST1_REG) */
#define DPC_OVL_PIXMODE 		0, 4
#define DPC_HSCALE 				4, 2
#define DPC_VSCALE 				6, 2
#define DPC_OVL_FIRSTPIX 		8, 4
#define DPC_DLIST_INT 			12
#define DPC_DLIST_EOL 			13
#define DPC_GLOBAL_ALPHA 		16, 8
#define DPC_OVL_UNDR_VDO 		24

/* (DPC_OVL_DLIST2_REG) */
#define DPC_DLIST_LUTPTR 		0, 18
#define DPC_DLIST_MAP2 			18
#define DPC_DLIST_MAP4 			19
#define DPC_DLIST_PALLET4 		20
#define DPC_DLIST_PALLET8 		21

/* (DPC_AUX_CONFIG_REG) */
#define DPC_AUX_ITU60HZ 		0
#define DPC_AUX_SRC_INTL 		1
#define DPC_AUX_SRC_SEL 		2
#define DPC_AUX_BWMODE 			3
#define DPC_AUX_VDDA_PH 		8, 8
#define DPC_AUX_FIFO_TH 		16, 12

/* (DPC_AUX_POS_REG) */
#define DPC_AUX_XPOS 			0, 12
#define DPC_AUX_YPOS 			16, 11

/* (DPC_AUX_HS_CTL_REG) */
#define DPC_AUX_HFS 				0, 8
#define DPC_AUX_HFN 				8, 12
#define DPC_AUX_HFM 				20, 12

/* (DPC_AUX_VS_CTL_REG) */
#define DPC_AUX_VFS 				0, 8
#define DPC_AUX_VFN 				8, 12
#define DPC_AUX_VFM 				20, 12

/* DPC (DPC_AUX_PROG1_REG) */
#define DPC_AUX_HCNT_H0 		0, 11
#define DPC_AUX_HCNT_H1 		11, 11
#define DPC_AUX_PROG_FLG 		22

/* DPC (DPC_AUX_PROG2_REG) */
#define DPC_AUX_VCNT_MAX 		0, 10
#define DPC_AUX_VCNT_V0_F 		10, 10
#define DPC_AUX_VCNT_V1_F 		20, 10

/* DPC (DPC_AUX_PROG3_REG) */
#define DPC_AUX_VCNT_F1 		0, 10
#define DPC_AUX_VCNT_V0_S 		10, 10
#define DPC_AUX_VCNT_V1_S 		20, 10

/* DPC (DPC_CURS_SA_REG) */
#define DPC_CURS_SA 				4, 22

/* DPC (DPC_CURS_POS_REG) */
#define DPC_CURS_XPOS 			0, 11
#define DPC_CURS_YPOS 			16, 11

/* DPC (DPC_CURS_OFFSET_REG) */
#define DPC_CURS_XOFFSET 		0, 8
#define DPC_CURS_HSCALE 		8, 2
#define DPC_CURS_YOFFSET 		16, 8
#define DPC_CURS_VSCALE 		24, 2

/* DPC (DPC_CURSCLUTx_REG) */
#define DPC_CURSCLUT0 			0, 32
#define DPC_CURSCLUT1 			0, 32
#define DPC_CURSCLUT2 			0, 32
#define DPC_CURSCLUT3 			0, 32
#define DPC_CURSCLUT4 			0, 32
#define DPC_CURSCLUT5 			0, 32
#define DPC_CURSCLUT6 			0, 32
#define DPC_CURSCLUT7 			0, 32
#define DPC_CURSCLUT8 			0, 32
#define DPC_CURSCLUT9 			0, 32
#define DPC_CURSCLUT10 			0, 32
#define DPC_CURSCLUT11 			0, 32
#define DPC_CURSCLUT12 			0, 32
#define DPC_CURSCLUT13 			0, 32
#define DPC_CURSCLUT14 			0, 32
#define DPC_CURSCLUT15 			0, 32

/* DPC (DPC_MEM_ADDR_REG) */
#define DPC_MEM_ADDR 			0, 10
#define DPC_MEM_OVERRIDE 		16

/* DPC (DPC_MEM_DATA_REG) */
#define DPC_MEM_DATA 			0, 32

/* DPC (DPC_GPIO_EN_REG) */
#define DPC_GPIO_EN_RED			0, 8
#define DPC_GPIO_EN_BLU			8, 8
#define DPC_GPIO_EN_GRN			16, 8
#define DPC_GPIO_EN_HSYNC		24
#define DPC_GPIO_EN_VSYNC		25
#define DPC_GPIO_EN_DDC0		26
#define DPC_GPIO_EN_DDC1		27
#define DPC_GPIO_EN 				0, 28

/* DPC (DPC_GPIO_OUT_EN_REG) */
#define DPC_GPIO_OUT_EN_RED		0, 8
#define DPC_GPIO_OUT_EN_BLU		8, 8
#define DPC_GPIO_OUT_EN_GRN		16, 8
#define DPC_GPIO_OUT_EN_HSYNC		24
#define DPC_GPIO_OUT_EN_VSYNC		25
#define DPC_GPIO_OUT_EN_DDC0		26
#define DPC_GPIO_OUT_EN_DDC1		27
#define DPC_GPIO_OUT_EN		 		0, 28

/* DPC (DPC_GPIO_OUTDATA_REG) */
#define DPC_GPIO_OUTDATA_RED		0, 8
#define DPC_GPIO_OUTDATA_BLU		8, 8
#define DPC_GPIO_OUTDATA_GRN		16, 8
#define DPC_GPIO_OUTDATA_HSYNC	24
#define DPC_GPIO_OUTDATA_VSYNC	25
#define DPC_GPIO_OUTDATA_DDC0		26
#define DPC_GPIO_OUTDATA_DDC1		27
#define DPC_GPIO_OUTDATA 			0, 28

/* DPC (DPC_GPIO_INDATA_REG) */
#define DPC_GPIO_INDATA_RED		0, 8
#define DPC_GPIO_INDATA_BLU		8, 8
#define DPC_GPIO_INDATA_GRN		16, 8
#define DPC_GPIO_INDATA_HSYNC		24
#define DPC_GPIO_INDATA_VSYNC		25
#define DPC_GPIO_INDATA_DDC0		26
#define DPC_GPIO_INDATA_DDC1		27
#define DPC_GPIO_INDATA 			0, 28

/* DPC (MCE_CTRL_REG) */
#define MCE_TOC_FEN 				0
#define MCE_TOC_FLUSH_EN 		1
#define MCE_TIF_INDEX 			3, 5
#define MCE_MPEG1 				8
#define MCE_422_REG 				9, 2
#define MCE_DP_RST 				11
#define MCE_PWR_REG 				12
#define MCE_IBUF_FLOW_CNTL 	13
#define MCE_PIPE_STALL_EN 		14
#define MCE_TOC_BYTE_STUFF_EN	15
#define MCE_TOC_ONESHOT_EN 	16

/* DPC (MCE_STAT_REG) */
#define MCE_TOC_RDY 				0
#define MCE_TOC_FLUSH_DONE		1
#define MCE_TIF_FIFO_CNT 		2, 6
#define MCE_RZQ_BUSY 			8
#define MCE_DCT_BUSY 			9
#define MCE_MC_BUSY 				10
#define MCE_VLD_BUSY 			11
#define MCE_VLD_ERR_CODE 		12, 4

/* DPC (MCE_IE_REG) */
#define MCE_SYNC_DONE_IE 						0
#define MCE_VTOC_UF_IE 							1
#define MCE_VLD_NONSLICE_START_IE 			2
#define MCE_VLD_ERR_IE 							3
#define MCE_RZQ_RUN_IE 							4
#define MCE_MC_IENG_ERR_IE 					5
#define MCE_MC_IBUF_OV_IE 						6
#define MCE_TOC_RDY_IE 							7
#define MCE_TOC_FLUSH_DONE_IE					8
#define MCE_VLD_RLFIFO_UDF_IE 				9

/* DPC (MCE_IS_REG) */
#define MCE_SYNC_DONE_INT 						0
#define MCE_VTOC_UF_INT 						1
#define MCE_VLD_NONSLICE_START_INT 			2
#define MCE_VLD_ERR_INT 						3
#define MCE_RZQ_RUN_INT 						4
#define MCE_MC_IENG_ERR_INT 					5
#define MCE_MC_IBUF_OV_INT 					6
#define MCE_TOC_RDY_INT 						7
#define MCE_TOC_FLUSH_DONE_INT 				8
#define MCE_VLD_RLFIFO_UDF_INT 				9

/* DPC (MCE_IS_ALIAS_REG) */
#define MCE_SYNC_DONE_INT_ALIAS 				0
#define MCE_VTOC_UF_INT_ALIAS 				1
#define MCE_VLD_NONSLICE_START_INT_ALIAS 	2
#define MCE_VLD_ERR_INT_ALIAS 				3
#define MCE_RZQ_RUN_INT_ALIAS 				4
#define MCE_MC_IENG_ERR_INT_ALIAS 			5
#define MCE_MC_IBUF_OV_INT_ALIAS 			6
#define MCE_TOC_RDY_INT_ALIAS 				7
#define MCE_TOC_FLUSH_DONE_INT_ALIAS 		8
#define MCE_VLD_RLFIFO_UDF_INT_ALIAS 		9

/* DPC (MCE_SYNC_CTRL_REG) */
#define MCE_SYNC_SEN 			0
#define MCE_SYNC_PEN 			1
#define MCE_SYNC_RANGE 			2, 6
#define MCE_SYNC_PAT 			8, 8

/* DPC (MCE_SYNC_STAT_REG) */
#define MCE_SYNC_DONE 			0
#define MCE_SYNC_DET 			1
#define MCE_SYNCWORD 			8, 8

/* DPC (MCE_FNSSC_REG) */
#define MCE_FNSSC_RADR 			0, 17
#define MCE_FNSSC_INDEX 		18, 6
#define MCE_FNSSC_SC 			24, 8

/* DPC (MCE_FSSC_REG) */
#define MCE_FSSC_RADR 			0, 17
#define MCE_FSSC_INDEX 			18, 6
#define MCE_FSSC_SC 				24, 8

/* DPC (MCE_VLD_CTRL_REG) */
#define MCE_VLD_SHIFT_AMT 		0, 5
#define MCE_VLD_SS 				5
#define MCE_VLD_XTRC_HDR 		6
#define MCE_VLD_MBSKIP_SETUP 	7
#define MCE_VLD_GOTO_IDLE 		8

/* DPC (MCE_TOC_RDATA_REG) */
#define MCE_TOC_RDATA 			0, 32
		 
/* DPC (MCE_Q_WDATA_REG) */
#define MCE_QTAB_WDATA 			0, 8
#define MCE_QTAB_SEL 			8, 2
#define MCE_QADR_RST 			10

/* DPC (MCE_MPIC_REG) */
#define MCE_ALTERNATE_SCAN 					0
#define MCE_INTRA_VLC_FORMAT 					1
#define MCE_Q_SCALE_TYPE 						2
#define MCE_FRAME_PRED_FRAME_DCT 			3
#define MCE_INTRA_DC_PRECISION 				4, 2
#define MCE_PICTURE_STRUCTURE 				6, 2
#define MCE_PICTURE_CODING_TYPE 				8, 3
#define MCE_CONCEALMENT_MV 					11
#define MCE_F_CODE11 							16, 4
#define MCE_F_CODE10 							20, 4
#define MCE_F_CODE01 							24, 4
#define MCE_F_CODE00 							28, 4

/* DPC (MCE_CH_SADR_REG) */
#define MCE_CH_SADR 				0, 17

/* DPC (MCE_CH_EADR_REG) */
#define MCE_CH_EADR 				0, 17

/* DPC (MCE_CH_RADR_REG) */
#define MCE_CH_RADR 				0, 17

/* DPC (HPIP_CONFIG_REG) */
#define HPIP_EN 					0
#define HPIP_LP_BYPASS 			1
#define HPIP_I_BYPASS 			2

/* DPC (HPIP_STATUS_REG) */
#define HPIP_SRC_XPOS 			0, 7
#define HPIP_ROI_XPOS 			8, 7
#define HPIP_YPOS 				16, 7
#define HPIP_DONE_STATUS 		24

/* DPC (HPIP_INT_EN_REG) */
#define HPIP_DONE_INT_EN 		0

/* DPC (HPIP_INT_STAT_REG) */
#define HPIP_DONE_INT_STAT 	0

/* DPC (HPIP_PTR_REG) */
#define HPIP_DST_BASE_TOP 		0, 4
#define HPIP_DST_BASE_BOT 		8, 5

/* DPC (HPIP_X_FACTOR_REG) */
#define HPIP_SRC_WIDTH 			0, 7
#define HPIP_ROI_WIDTH 			8, 7
#define HPIP_ROI_XOFFSET 		16, 7
#define HPIP_DST_WIDTH 			24, 7

/* DPC (HPIP_Y_FACTOR_REG) */
#define HPIP_SRC_HEIGHT 		0, 7

/* DPC (HPIP_DDA_FACTOR_REG) */
#define HPIP_HFS 					0, 10
#define HPIP_HFN 					10, 11
#define HPIP_HFM 					21, 11

/* DPC (HPIP_COEFF_ADDR_REG) */
#define HPIP_COEFF_INDEX 		0, 3

/* DPC (HPIP_COEFF_DATA_REG) */
#define HPIP_COEFF_DATA1 		0, 12
#define HPIP_COEFF_DATA2 		16, 12

/* DPC (HPIP_CMODEL_REG) */
#define HPIP_SRC_BASE_TOP 		0, 5
#define HPIP_SRC_BASE_BOT 		8, 5
#define HPIP_PROGRESSIVE 		16
#define HPIP_FRAME_MODE 		20
#define HPIP_FIELD_SELECT 		24, 2

/* DPC (VPIP_CONFIG_REG) */
#define VPIP_LUM_SAME_FLD 		0, 2
#define VPIP_CHM_SAME_FLD 		2, 4
#define VPIP_MOA_EN 				4
#define VPIP_MOA_VERT 			5
#define VPIP_SAVE_BWIDTH 		6
#define VPIP_SHWMO 				7
#define VPIP_MOA_TH 				8, 4
#define VPIP_MODE 				12, 2
#define VPIP_SRC_FMT422 		14
#define VPIP_DST_FMT422 		15
#define VPIP_SRC_VALID_CNT 	16, 6
#define VPIP_CUR_FLD 			22

/* DPC (VPIP_INT_EN_REG) */
#define VPIP_DONE_INT_EN 		0

/* DPC VPIP_INT_STAT_REG) */
#define VPIP_DONE_INT_STAT 	0

/* DPC (VPIP_PTR_REG) */
#define VPIP_SRC_BASE_TOP 		0, 4
#define VPIP_SRC_BASE_BOT 		8, 4
#define VPIP_DST_BASE_TOP 		16, 4
#define VPIP_DST_BASE_BOT 		24, 4

/* DPC (VPIP_POS_X_REG) */
#define VPIP_SRC_X_LEFT 		3, 9
#define VPIP_DST_X_LEFT 		19, 9

/* DPC (VPIP_STATUS_REG) */
#define VPIP_SRC_X 				3, 9
#define VPIP_DST_X 				19, 9

/* DPC (VPIP_SIZE_REG) */
#define VPIP_WIDTH 				4, 8

/* DPC (VPIP_ROW_CONFIG_REG) */
#define VPIP_IDLE 				0
#define VPIP_LUMA_PASS 			1
#define VPIP_RD_BOT_FIRST 		2
#define VPIP_WR_BOT_FIRST 		3
#define VPIP_REPL_COUNT 		4, 4
#define VPIP_FIRST_PATCH 	   8
#define VPIP_LAST_PATCH 		9
#define VPIP_SRC_Y 				10, 11
#define VPIP_DST_Y 				21, 11

/* DPC (VPIP_DDA_FACTOR_REG) */
#define VPIP_VFS 					0, 10
#define VPIP_VFN					10, 11
#define VPIP_VFM 					21, 11

/* DPC  (VPIP_COEFF_ADDR_REG) */
#define VPIP_COEFF_INDEX 		0, 3

/* DPC  (VPIP_COEFF_DATA_REG) */
#define VPIP_COEFF_DATA1 		0, 12
#define VPIP_COEFF_DATA2 		16, 12

/* DPC  (VPIP_ROW_CONFIG2_REG) */
#define VPIP_DDA_ERROR 			0, 12
#define VPIP_DDA_PHASE 			12, 11
#define VPIP_DDA_LUMA_OW 		31

/* DPC  (VPIP_STATUS2_REG) */
#define VPIP_PATCH_HEIGHT 		0, 6

/* DPC  (MIF_CONFIG_REG) */
#define MIF_MEM_WIDTH 			0
#define MIF_NUM_RANK 			2
#define MIF_RANK_SIZE 			4, 3
#define MIF_NUM_BANK 			8
#define MIF_BANK_SELECT 		12, 3
#define MIF_NUM_COL 				16, 3
#define MIF_TRAS 					20, 2
#define MIF_REFRESH_PERIOD 	24, 8

/* DPC  (MIF_FLD_DSCR_0_REG) */
#define FLD_HEIGHT_0 			0, 7
#define FLD_FORMAT_0 			7
#define FLD_WIDTH_0 				8, 6
#define FLD_AMR_MODE_0 			14, 2
#define FLD_BASE_ADDR_0 		16, 15

/* DPC  (MIF_FLD_DSCR_1_REG) */
#define FLD_HEIGHT_1 			0, 7
#define FLD_FORMAT_1 			7
#define FLD_WIDTH_1 				8, 6
#define FLD_AMR_MODE_1 			14, 2
#define FLD_BASE_ADDR_1 		16, 15

/* DPC  (MIF_FLD_DSCR_2_REG) */
#define FLD_HEIGHT_2 			0, 7
#define FLD_FORMAT_2 			7
#define FLD_WIDTH_2 				8, 6
#define FLD_AMR_MODE_2 			14, 2
#define FLD_BASE_ADDR_2 		16, 15

/* DPC  (MIF_FLD_DSCR_3_REG) */
#define FLD_HEIGHT_3 			0, 7
#define FLD_FORMAT_3 			7
#define FLD_WIDTH_3 				8, 6
#define FLD_AMR_MODE_3 			14, 2
#define FLD_BASE_ADDR_3 		16, 15

/* DPC  (MIF_FLD_DSCR_4_REG) */
#define FLD_HEIGHT_4 			0, 7
#define FLD_FORMAT_4 			7
#define FLD_WIDTH_4 				8, 6
#define FLD_AMR_MODE_4 			14, 2
#define FLD_BASE_ADDR_4 		16, 15

/* DPC  (MIF_FLD_DSCR_5_REG) */
#define FLD_HEIGHT_5 			0, 7
#define FLD_FORMAT_5 			7
#define FLD_WIDTH_5 				8, 6
#define FLD_AMR_MODE_5 			14, 2
#define FLD_BASE_ADDR_5 		16, 15

/* DPC  (MIF_FLD_DSCR_6_REG) */
#define FLD_HEIGHT_6 			0, 7
#define FLD_FORMAT_6 			7
#define FLD_WIDTH_6 				8, 6
#define FLD_AMR_MODE_6 			14, 2
#define FLD_BASE_ADDR_6 		16, 15

/* DPC  (MIF_FLD_DSCR_7_REG) */
#define FLD_HEIGHT_7 			0, 7
#define FLD_FORMAT_7 			7
#define FLD_WIDTH_7 				8, 6
#define FLD_AMR_MODE_7 			14, 2
#define FLD_BASE_ADDR_7 		16, 15

/* DPC  (MIF_FLD_DSCR_8_REG) */
#define FLD_HEIGHT_8 			0, 7
#define FLD_FORMAT_8 			7
#define FLD_WIDTH_8 				8, 6
#define FLD_AMR_MODE_8 			14, 2
#define FLD_BASE_ADDR_8 		16, 15

/* DPC  (MIF_FLD_DSCR_9_REG) */
#define FLD_HEIGHT_9 			0, 7
#define FLD_FORMAT_9 			7
#define FLD_WIDTH_9 				8, 6
#define FLD_AMR_MODE_9 			14, 2
#define FLD_BASE_ADDR_9 		16, 15

/* DPC  (MIF_FLD_DSCR_10_REG) */
#define FLD_HEIGHT_10 			0, 7
#define FLD_FORMAT_10 			7
#define FLD_WIDTH_10 			8, 6
#define FLD_AMR_MODE_10 		14, 2
#define FLD_BASE_ADDR_10 		16, 15

/* DPC  (MIF_FLD_DSCR_11_REG) */
#define FLD_HEIGHT_11 			0, 7
#define FLD_FORMAT_11 			7
#define FLD_WIDTH_11 			8, 6
#define FLD_AMR_MODE_11 		14, 2
#define FLD_BASE_ADDR_11 		16, 15

/* DPC  (MIF_FLD_DSCR_12_REG) */
#define FLD_HEIGHT_12 			0, 7
#define FLD_FORMAT_12 			7
#define FLD_WIDTH_12 			8, 6
#define FLD_AMR_MODE_12 		14, 2
#define FLD_BASE_ADDR_12 		16, 15

/* DPC  (MIF_FLD_DSCR_13_REG) */
#define FLD_HEIGHT_13 			0, 7
#define FLD_FORMAT_13 			7
#define FLD_WIDTH_13 			8, 6
#define FLD_AMR_MODE_13 		14, 2
#define FLD_BASE_ADDR_13 		16, 15

/* DPC  (MIF_FLD_DSCR_14_REG) */
#define FLD_HEIGHT_14 			0, 7
#define FLD_FORMAT_14 			7
#define FLD_WIDTH_14 			8, 6
#define FLD_AMR_MODE_14 		14, 2
#define FLD_BASE_ADDR_14 		16, 15

/* DPC  (MIF_FLD_DSCR_15_REG) */
#define FLD_HEIGHT_15 			0, 7
#define FLD_FORMAT_15 			7
#define FLD_WIDTH_15 			8, 6
#define FLD_AMR_MODE_15 		14, 2
#define FLD_BASE_ADDR_15 		16, 15

/* DPC  (MIF_STATUS_HIF_RD) */
#define MIF_STATUS_HIF_RD_MREQ 				0
#define MIF_STATUS_HIF_RD_BUSY 				1
#define MIF_STATUS_HIF_RD_READY0 			4
#define MIF_STATUS_HIF_RD_READY1 			5
#define MIF_STATUS_HIF_RD_READY2 			6
#define MIF_STATUS_HIF_RD_READY3 			7
#define MIF_STATUS_HIF_RD_DESC_CNT 			8, 4
#define MIF_STATUS_HIF_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_APU_RD) */
#define MIF_STATUS_APU_RD_MREQ 				0
#define MIF_STATUS_APU_RD_BUSY 				1
#define MIF_STATUS_APU_RD_READY0 			4
#define MIF_STATUS_APU_RD_READY1 			5
#define MIF_STATUS_APU_RD_READY2 			6
#define MIF_STATUS_APU_RD_READY3 			7
#define MIF_STATUS_APU_RD_DESC_CNT 			8, 4
#define MIF_STATUS_APU_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_TOC) */
#define MIF_STATUS_TOC_MREQ 					0
#define MIF_STATUS_TOC_BUSY 					1
#define MIF_STATUS_TOC_READY0 				4
#define MIF_STATUS_TOC_READY1 				5
#define MIF_STATUS_TOC_READY2 				6
#define MIF_STATUS_TOC_READY3 				7
#define MIF_STATUS_TOC_DESC_CNT 				8, 4
#define MIF_STATUS_TOC_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_BLT0_RD) */
#define MIF_STATUS_BLT0_RD_MREQ 				0
#define MIF_STATUS_BLT0_RD_BUSY 				1
#define MIF_STATUS_BLT0_RD_READY0 			4
#define MIF_STATUS_BLT0_RD_READY1 			5
#define MIF_STATUS_BLT0_RD_READY2 			6
#define MIF_STATUS_BLT0_RD_READY3 			7
#define MIF_STATUS_BLT0_RD_DESC_CNT 		8, 4
#define MIF_STATUS_BLT0_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_VPIP_RD) */
#define MIF_STATUS_VPIP_RD_MREQ 				0
#define MIF_STATUS_VPIP_RD_BUSY 				1
#define MIF_STATUS_VPIP_RD_READY0 			4
#define MIF_STATUS_VPIP_RD_READY1 			5
#define MIF_STATUS_VPIP_RD_READY2 			6
#define MIF_STATUS_VPIP_RD_READY3 			7
#define MIF_STATUS_VPIP_RD_DESC_CNT 		8, 4
#define MIF_STATUS_VPIP_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_BVDO1) */
#define MIF_STATUS_BVDO1_MREQ 				0
#define MIF_STATUS_BVDO1_BUSY 				1
#define MIF_STATUS_BVDO1_READY0 				4
#define MIF_STATUS_BVDO1_READY1 				5
#define MIF_STATUS_BVDO1_READY2 				6
#define MIF_STATUS_BVDO1_READY3 				7
#define MIF_STATUS_BVDO1_DESC_CNT 			8, 4
#define MIF_STATUS_BVDO1_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_CLUT) */
#define MIF_STATUS_CLUT_MREQ 					0
#define MIF_STATUS_CLUT_BUSY 					1
#define MIF_STATUS_CLUT_READY0 				4
#define MIF_STATUS_CLUT_READY1 				5
#define MIF_STATUS_CLUT_READY2 				6
#define MIF_STATUS_CLUT_READY3 				7
#define MIF_STATUS_CLUT_DESC_CNT 			8, 4
#define MIF_STATUS_CLUT_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_MC_RD) */
#define MIF_STATUS_MC_RD_MREQ 				0
#define MIF_STATUS_MC_RD_BUSY 				1
#define MIF_STATUS_MC_RD_READY0 				4
#define MIF_STATUS_MC_RD_READY1 				5
#define MIF_STATUS_MC_RD_READY2 				6
#define MIF_STATUS_MC_RD_READY3 				7
#define MIF_STATUS_MC_RD_DESC_CNT 			8, 4
#define MIF_STATUS_MC_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_OVL1) */
#define MIF_STATUS_OVL1_MREQ 					0
#define MIF_STATUS_OVL1_BUSY 					1
#define MIF_STATUS_OVL1_READY0 				4
#define MIF_STATUS_OVL1_READY1 				5
#define MIF_STATUS_OVL1_READY2 				6
#define MIF_STATUS_OVL1_READY3 				7
#define MIF_STATUS_OVL1_DESC_CNT 			8, 4
#define MIF_STATUS_OVL1_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_OVL2) */
#define MIF_STATUS_OVL2_MREQ 					0
#define MIF_STATUS_OVL2_BUSY 					1
#define MIF_STATUS_OVL2_READY0 				4
#define MIF_STATUS_OVL2_READY1 				5
#define MIF_STATUS_OVL2_READY2 				6
#define MIF_STATUS_OVL2_READY3 				7
#define MIF_STATUS_OVL2_DESC_CNT 			8, 4
#define MIF_STATUS_OVL2_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_BLT1_RD) */
#define MIF_STATUS_BLT1_RD_MREQ 				0
#define MIF_STATUS_BLT1_RD_BUSY 				1
#define MIF_STATUS_BLT1_RD_READY0 			4
#define MIF_STATUS_BLT1_RD_READY1 			5
#define MIF_STATUS_BLT1_RD_READY2 			6
#define MIF_STATUS_BLT1_RD_READY3 			7
#define MIF_STATUS_BLT1_RD_DESC_CNT 		8, 4 
#define MIF_STATUS_BLT1_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_BLT2_RD) */
#define MIF_STATUS_BLT2_RD_MREQ 				0
#define MIF_STATUS_BLT2_RD_BUSY 				1
#define MIF_STATUS_BLT2_RD_READY0 			4
#define MIF_STATUS_BLT2_RD_READY1 			5
#define MIF_STATUS_BLT2_RD_READY2 			6
#define MIF_STATUS_BLT2_RD_READY3 			7
#define MIF_STATUS_BLT2_RD_DESC_CNT 		8, 4
#define MIF_STATUS_BLT2_RD_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_BVDO2) */
#define MIF_STATUS_BVDO2_MREQ 				0
#define MIF_STATUS_BVDO2_BUSY 				1
#define MIF_STATUS_BVDO2_READY0 				4
#define MIF_STATUS_BVDO2_READY1 				5
#define MIF_STATUS_BVDO2_READY2 				6
#define MIF_STATUS_BVDO2_READY3 				7
#define MIF_STATUS_BVDO2_DESC_CNT 			8, 4
#define MIF_STATUS_BVDO2_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_BVDO3) */
#define MIF_STATUS_BVDO3_MREQ 				0
#define MIF_STATUS_BVDO3_BUSY 				1
#define MIF_STATUS_BVDO3_READY0 				4
#define MIF_STATUS_BVDO3_READY1 				5
#define MIF_STATUS_BVDO3_READY2 				6
#define MIF_STATUS_BVDO3_READY3 				7
#define MIF_STATUS_BVDO3_DESC_CNT 			8, 4
#define MIF_STATUS_BVDO3_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_CURS) */
#define MIF_STATUS_CURS_MREQ 					0
#define MIF_STATUS_CURS_BUSY 					1
#define MIF_STATUS_CURS_READY0 				4
#define MIF_STATUS_CURS_READY1 				5
#define MIF_STATUS_CURS_READY2 				6
#define MIF_STATUS_CURS_READY3 				7
#define MIF_STATUS_CURS_DESC_CNT 			8, 4
#define MIF_STATUS_CURS_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_DL) */
#define MIF_STATUS_DL_MREQ 					0
#define MIF_STATUS_DL_BUSY 					1
#define MIF_STATUS_DL_READY0 					4
#define MIF_STATUS_DL_READY1 					5
#define MIF_STATUS_DL_READY2 					6
#define MIF_STATUS_DL_READY3 					7
#define MIF_STATUS_DL_DESC_CNT 				8, 4
#define MIF_STATUS_DL_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_HIF_WR) */
#define MIF_STATUS_HIF_WR_MREQ 				0
#define MIF_STATUS_HIF_WR_BUSY 				1
#define MIF_STATUS_HIF_WR_READY0 			4
#define MIF_STATUS_HIF_WR_READY1 			5
#define MIF_STATUS_HIF_WR_READY2 			6
#define MIF_STATUS_HIF_WR_READY3 			7
#define MIF_STATUS_HIF_WR_DESC_CNT 			8, 4
#define MIF_STATUS_HIF_WR_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_APU_WR) */
#define MIF_STATUS_APU_WR_MREQ 				0
#define MIF_STATUS_APU_WR_BUSY 				1
#define MIF_STATUS_APU_WR_READY0 			4
#define MIF_STATUS_APU_WR_READY1 			5
#define MIF_STATUS_APU_WR_READY2 			6
#define MIF_STATUS_APU_WR_READY3 			7
#define MIF_STATUS_APU_WR_DESC_CNT 			8, 4
#define MIF_STATUS_APU_WR_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_TRANS) */
#define MIF_STATUS_TRANS_MREQ 				0
#define MIF_STATUS_TRANS_BUSY 				1
#define MIF_STATUS_TRANS_READY0 				4
#define MIF_STATUS_TRANS_READY1 				5
#define MIF_STATUS_TRANS_READY2 				6
#define MIF_STATUS_TRANS_READY3 				7
#define MIF_STATUS_TRANS_DESC_CNT 			8, 4
#define MIF_STATUS_TRANS_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_BLT_WR) */
#define MIF_STATUS_BLT_WR_MREQ 				0
#define MIF_STATUS_BLT_WR_BUSY 				1
#define MIF_STATUS_BLT_WR_READY0 			4
#define MIF_STATUS_BLT_WR_READY1 			5
#define MIF_STATUS_BLT_WR_READY2 			6
#define MIF_STATUS_BLT_WR_READY3 			7
#define MIF_STATUS_BLT_WR_DESC_CNT 			8, 4
#define MIF_STATUS_BLT_WR_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_VPIP_WR) */
#define MIF_STATUS_VPIP_WR_MREQ 				0
#define MIF_STATUS_VPIP_WR_BUSY 				1
#define MIF_STATUS_VPIP_WR_READY0 			4
#define MIF_STATUS_VPIP_WR_READY1 			5
#define MIF_STATUS_VPIP_WR_READY2 			6
#define MIF_STATUS_VPIP_WR_READY3 			7
#define MIF_STATUS_VPIP_WR_DESC_CNT 		8, 4
#define MIF_STATUS_VPIP_WR_BUF_CNT 			12, 4

/* DPC  (MIF_STATUS_HPIP) */
#define MIF_STATUS_HPIP_MREQ 					0
#define MIF_STATUS_HPIP_BUSY 					1
#define MIF_STATUS_HPIP_READY0 				4
#define MIF_STATUS_HPIP_READY1 				5
#define MIF_STATUS_HPIP_READY2 				6
#define MIF_STATUS_HPIP_READY3 				7
#define MIF_STATUS_HPIP_DESC_CNT 			8, 4
#define MIF_STATUS_HPIP_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_VSC) */
#define MIF_STATUS_VSC_MREQ 					0
#define MIF_STATUS_VSC_BUSY 					1
#define MIF_STATUS_VSC_READY0 				4
#define MIF_STATUS_VSC_READY1 				5
#define MIF_STATUS_VSC_READY2 				6
#define MIF_STATUS_VSC_READY3 				7
#define MIF_STATUS_VSC_DESC_CNT 				8, 4
#define MIF_STATUS_VSC_BUF_CNT 				12, 4

/* DPC  (MIF_STATUS_MC_WR) */
#define MIF_STATUS_MC_WR_MREQ 				0
#define MIF_STATUS_MC_WR_BUSY 				1
#define MIF_STATUS_MC_WR_READY0 				4
#define MIF_STATUS_MC_WR_READY1 				5
#define MIF_STATUS_MC_WR_READY2 				6
#define MIF_STATUS_MC_WR_READY3 				7
#define MIF_STATUS_MC_WR_DESC_CNT 			8, 4
#define MIF_STATUS_MC_WRBUF_CNT 				12, 4

/* DPC  (MCU_HALT_REG) */
#define MCU_SS 									1
#define MCU_HLT 									0

/* DPC  (MCU_COND_CODE) */
#define MCU_CC_V 									3
#define MCU_CC_N 									2
#define MCU_CC_C 									1
#define MCU_CC_Z 									0

/* DPC  (MCU_RAM_SLICE_VERTICAL_POSITION) */
#define MCU_SLICE_VERTICAL_POSITION 		0, 8

/* DPC  (MCU_RAM_MBAINC) */
#define MCU_MBAINC 								0, 8

/* DPC  (MCU_RAM_MOTION_TYPE) */
#define MCU_MOTION_TYPE 						0, 2
#define MCU_MACROBLOCK_MOTION_FORWARD 		10
#define MCU_MACROBLOCK_MOTION_BACKWARD 	9
#define MCU_MACROBLOCK_INTRA 					8

/* DPC  (MCU_RAM_MVECTOR) */
#define MCU_MOTION_VERTICAL_FIELD_SELECT_00 		0
#define MCU_MOTION_VERTICAL_FIELD_SELECT_01 		1
#define MCU_MOTION_VERTICAL_FIELD_SELECT_10 		2
#define MCU_MOTION_VERTICAL_FIELD_SELECT_11 		3
#define MCU_DMVECTOR_0 									8, 2
#define MCU_DMVECTOR_1 									12, 2

/* DPC  (MCU_RAM_DELTA_xxx) */
#define MCU_DELTA_R_S_T_000 					0, 11
#define MCU_DELTA_R_S_T_001 					0, 11
#define MCU_DELTA_R_S_T_010 					0, 11
#define MCU_DELTA_R_S_T_011 					0, 11
#define MCU_DELTA_R_S_T_100 					0, 11
#define MCU_DELTA_R_S_T_101 					0, 11
#define MCU_DELTA_R_S_T_110 					0, 11
#define MCU_DELTA_R_S_T_111 					0, 11

/* DPC  (MCU_RAM_PICTURE_CODING_STRUCTURE) */
#define MCU_PICTURE_CODING_TYPE 				0, 2
#define MCU_PICTURE_STRUCTURE 				8, 2

/* DPC  (MCU_RAM_PICTURE_FIELD) */
#define MCU_TOP_FIELD_FIRST 					0
#define MCU_SECOND_FIELD 						8

/* DPC  (MCU_RAM_MV_FRAME) */
#define MCU_FRAME_PRED_FRAME_DCT 			0
#define MCU_CONCEALMENT_MOTION_VECTORS 	8

/* DPC  (MCU_RAM_MB_WIDTH) */
#define MCU_MB_WIDTH 							0, 8

/* DPC  (MCU_RAM_MBAMAX) */
#define MCU_MBAMAX 								0, 14

/* DPC  (MCU_RAM_RANGE_xx) */
#define MCU_RANGE_S_T_00 						0, 13
#define MCU_RANGE_S_T_01 						0, 13
#define MCU_RANGE_S_T_10 						0, 13
#define MCU_RANGE_S_T_11 						0, 134

/* DPC  (MCU_RAM_FBUF_BASE_FWD) */
#define MCU_FBUF_BASE_FWD_EVEN 				0, 4
#define MCU_FBUF_BASE_FWD_ODD 				8, 4

/* DPC  (MCU_RAM_FBUF_BASE_BWD) */
#define MCU_FBUF_BASE_BWD_EVEN 				0, 4
#define MCU_FBUF_BASE_BWD_ODD 				8, 4

/* DPC  (MCU_RAM_FBUF_BASE_DST) */
#define MCU_FBUF_BASE_DST_EVEN 				0, 4
#define MCU_FBUF_BASE_DST_ODD 				8, 4

/* DPC  (MCU_RAM_PICTURE_START) */
#define MCU_PICTURE_START 						0

/* DPC  (APU_IOC_CONFIG_REG) */
#define APU_IJFY 									0
#define APU_ACE 									1
#define APU_IDLY 									2
#define APU_IWL0 									3, 2
#define APU_IWL1 									5, 2
#define APU_IWL2 									7, 2
#define APU_IOC_OWL 								9, 2
#define APU_BYP 									15
#define APU_ICM 									16, 2
#define APU_OCM 									18, 2
#define APU_BCX 									20, 2
#define APU_ACM 									22, 2
#define APU_ACX 									24, 2

/* DPC  (APU_CPB_CONFIG_REG) */
#define APU_CFR 									0
#define APU_CMSK 									1, 6
#define APU_CEN 									7
#define APU_PEN 									8
#define APU_PBM0 									9, 2
#define APU_PBM1 									11, 2
#define APU_PBM2 									13, 2
#define APU_IAM0 									16, 2
#define APU_IAM1 									18, 2
#define APU_IAM2 									20,2 

/* DPC  (APU_CPB_CONFIG_REG) */
#define APU_ENB 									0
#define APU_IEC_OWL 								1, 2
#define APU_MOD0 									3, 2
#define APU_MOD1 									5, 2
#define APU_CSM 									7
#define APU_SEL0 									8, 3
#define APU_SEL1 									12, 3
#define APU_IECAUX 								16, 4
#define APU_U 										20
#define APU_V 										21
#define APU_P 										22
#define APU_FMC 									24, 8

/* DPC  (APU_IEC_CSB1_REG) */
#define APU_CSB1 									0, 32

/* DPC  (APU_IEC_CSB2_REG) */
#define APU_CSB2 									0, 32

/* DPC  (APU_INT_CONFIG_REG) */
#define APU_MATINT 								0, 3
#define APU_CAPOVF_EN 							5
#define APU_CAPUNF_EN 							6
#define APU_IAUWRP_EN 							8, 3
#define APU_IECFRM_EN 							11, 2

/* DPC  (APU_INT_STATUS_REG) */
#define APU_MATINT_STAT 						0
#define APU_CAPOVF_STAT 						5
#define APU_CAPUNF_STAT 						6
#define APU_IAUWRP_STAT							8, 3
#define APU_IECFRM_STAT 						11

/* DPC  (APU_CPB_CxA_REG) */
#define APU_CBA 			5, 20
#define APU_CEA 			5, 20
#define APU_CWA 			5, 20
#define APU_CRA 			5, 20
#define APU_CMA 			5, 20

/* DPC  (APU_IAU_IBAx_REG) */
#define APU_IBA0 			5, 20
#define APU_IBA1 			5, 20
#define APU_IBA2 			5, 20

/* DPC  (APU_IAU_IRAx_REG) */
#define APU_IRA0 			5, 20
#define APU_IRA1 			5, 20
#define APU_IRA2 			5, 20

/* DPC  (APU_IAU_IEAx_REG) */
#define APU_IEA0 			5, 20
#define APU_IEA1 			5, 20
#define APU_IEA2 			5, 20

/* DPC  (APU_MIX_CONFIG_REG) */
#define APU_MIXM0 		0, 2
#define APU_MIXM1 		2, 2
#define APU_MIXM2 		4, 2
#define APU_MIXC0 		8, 8
#define APU_MIXC1 		16, 8
#define APU_MIXC2 		24, 8

/* DPC  (APU_MIX_XFC0_REG) */
#define APU_XFC11 		24, 8
#define APU_XFC10 		16, 8
#define APU_XFC01 		8, 8
#define APU_XFC00 		0, 8

/* DPC  (APU_MIX_XFC1_REG) */
#define APU_XFC13 		24, 8
#define APU_XFC12 		16, 8
#define APU_XFC03 		8, 8
#define APU_XFC02 		0, 8

/* DPC  (APU_MIX_XFC2_REG) */
#define APU_XFC15 		24, 8
#define APU_XFC14 		16, 8
#define APU_XFC05 		8, 8
#define APU_XFC04 		0, 8

/* DPC  (APU_MIX_XFC3_REG) */
#define APU_XFC31 		24, 8
#define APU_XFC30 		16, 8
#define APU_XFC21 		8, 8
#define APU_XFC20 		0, 8

/* DPC  (APU_MIX_XFC4_REG) */
#define APU_XFC33 		24, 8
#define APU_XFC32 		16, 8
#define APU_XFC23 		8, 8
#define APU_XFC22 		0, 8

/* DPC  (APU_MIX_XFC5_REG) */
#define APU_XFC35 		24, 8
#define APU_XFC34 		16, 8
#define APU_XFC25 		8, 8
#define APU_XFC24 		0, 8

/* DPC  (APU_MIX_XFC6_REG) */
#define APU_XFC51 		24, 8
#define APU_XFC50 		16, 8
#define APU_XFC41 		8, 8
#define APU_XFC40 		0, 8

/* DPC  (APU_MIX_XFC7_REG) */
#define APU_XFC53 		24, 8
#define APU_XFC52 		16, 8
#define APU_XFC43 		8, 8
#define APU_XFC42 		0, 8

/* DPC  (APU_MIX_XFC8_REG) */
#define APU_XFC55 		24, 8
#define APU_XFC54 		16, 8
#define APU_XFC45 		8, 8
#define APU_XFC44 		0, 8

/* DPC  (VSC_CTRL_REG) */
#define VSC_EN 					0
#define VSC_VBI_EN0 				1
#define VSC_VBI_EN1 				2
#define VSC_VBI_VACT_ON 		3
#define VSC_VACT_POLARITY 		4
#define VSC_CTYPE 				5
#define VSC_VLINE_ICNT 			6, 10

/* DPC  (VSC_STAT__REG) */
#define VSC_V_FLAG 				0
#define VSC_F_FLAG 				1

/* DPC  (VSC_INT_EN_REG) */
#define VSC_IBUF_OV_IE 			0
#define VSC_VLINE_IE 			1
#define VSC_VBLANK0_IE 			2
#define VSC_VBLANK1_IE 			3
#define VSC_F0_IE 				4
#define VSC_F1_IE 				5
#define VSC_VBI_IE 				6

/* DPC  (VSC_INT_STAT_REG) */
#define VSC_IBUF_OV_INT 		0
#define VSC_VLINE_INT 			1
#define VSC_VBLANK0_INT 		2
#define VSC_VBLANK1_INT 		3
#define VSC_F0_INT 				4
#define VSC_F1_INT 				5
#define VSC_VBI_INT 				6

/* DPC  (VSC_MIF_REG) */
#define VSC_BASE0 				0, 4
#define VSC_F0_OFFSET 			7
#define VSC_BASE1 				8, 4

/* DPC  (VSC_DDA_REG) */
#define VSC_HSTEP_REG 			0, 20
#define VSC_LPHASE 				24, 8

/* DPC  (VSC_HS_REG) */
#define VSC_LINE_WIDTH 			0, 10
#define VSC_SV_HSIZE 			16, 10

/* DPC  (VSC_VBI0_REG) */
#define VSC_VBI0_VOFFSET 		0, 10
#define VSC_VBI0_VSIZE 			16, 9

/* DPC  (VSC_VBI1_REG) */
#define VSC_VBI1_VOFFSET 		0, 10
#define VSC_VBI1_VSIZE 			16, 9

/* DPC  (VSC_VBUF_SADR_REG) */
#define VSC_VBUF_SADR 			3, 23

/* DPC  (VSC_VBUF_WADR_REG) */
#define VSC_VBUF_WADR 			3, 23

/* DPC  (BLT_CHAN_S0_REG) */
#define BLT_X_DIR0 				0
#define BLT_Y_DIR0 				1
#define BLT_PIX_FORMAT0 		2, 4
#define BLT_BLOCK_PAINT0 		6
#define BLT_REDUCE_PIX0 		7, 2
#define BLT_MASK0 				9

/* DPC  (BLT_CHAN_S1_REG) */
#define BLT_X_DIR1 				0
#define BLT_Y_DIR1 				1
#define BLT_PIX_FORMAT1 		2, 4
#define BLT_BLOCK_PAINT1 		6
#define BLT_REDUCE_PIX1 		7, 2
#define BLT_MASK1 				9

/* DPC  (BLT_CHAN_D_REG) */
#define BLT_XY_DITHER 			6, 2

/* DPC  (BLT_CMD_REG) */
#define BLT_BLTOP 				0, 2
#define BLT_BLENDON 				2
#define BLT_ENABLE_S0 			3
#define BLT_ENABLE_S1 			4
#define BLT_ENABLE_D 			5
#define BLT_LINK_LIST_ENABLE 	6
#define BLT_BLTROP 				8, 8
#define BLT_BOOL_SEL_ALPHA 	16, 2

/* DPC  (BLT_STAT_REG) */
#define PACE_BLT 					0, 2
#define SUSPEND_BLT 				2
#define BLT_MASK_FIELD 			3
#define BLT_BUSY 					4
#define END_BLT 					5

/* DPC  (BLT_INTR_STAT_REG) */
#define BLT_END_STATUS 					0
#define BLT_END_OF_CHAIN_STATUS 		1
#define BLT_LATE 							2
#define MASK_BLT_END 					3
#define MASK_END_CHAIN_BLT				4
#define MASK_BLT_LATE 					5

/* DPC  (BLT_LEFT_ADDRESS_Sx_REG) */
#define BLT_LEFT_ADDRESS0 		0, 28
#define BLT_LEFT_ADDRESS1 		0, 28

/* DPC  (BLT_RIGHT_ADDRESS_Sx_REG) */
#define BLT_RIGHT_ADDRESS0 	0, 28
#define BLT_RIGHT_ADDRESS1 	0, 28

/* DPC  (BLT_Sx_PITCH_SIZ_REG) */
#define BLT_PITCH0 				0, 16
#define BLT_PITCH1 				0, 16

/* DPC  (BLT_H_W_REG) */
#define BLT_WIDTH 				0, 16
#define BLT_HEIGHT 				16, 16

/* DPC  (BLT_LINK_ADDR_REG) */
#define BLT_LINK_ADDR 			0, 23

/* DPC  (BLT_S0_FG_COLOR_REG) */
#define BLT_S0_FG_COL 			0, 32

/* DPC  (BLT_S0_BG_REG) */
#define BLT_S0_BG 				0, 32

/* DPC  (BLT_S1_FG_COLOR_REG) */
#define BLT_S1_FG_COL 			0, 32

/* DPC  (BLT_S1_BG_REG) */
#define BLT_S1_BG 				0, 32

/* DPC  (BLT_ALPHA_REG) */
#define BLT_GLO_ALPHA_S 		0, 8
#define BLT_GLO_ALPHA_D 		8, 8
#define BLT_SEL_ALPHA_S 		16, 3
#define BLT_SEL_ALPHA_D 		19, 3
#define BLT_SEL_NEW_ALPHA_D 	22, 4

/* DPC  (BLT_PACE_LINE_REG) */
#define BLT_PACE_LINE 			0, 11

/* DPC  (HIF_INIT_REG) */
#define HIF_BUS_WIDTH 			0
#define HIF_ENDIAN 				1
#define HIF_BYTE_EN 				2
#define HIF_RW_POL 				3
#define HIF_WAIT_POL 			4
#define HIF_SIZE_TR 				5
#define HIF_BURST_IND 			6
#define HIF_STATE 				8, 4

/* DPC  (HIF_STAT_REG) */
#define HIF_WRB_EMPTY 			0

/* DPC  (HIF_INT0_STAT_REG) */
#define HIF_APU_INTR0 			0
#define HIF_BLT_INTR0 			1
#define HIF_MIF_INTR0 			2
#define HIF_DPC_INTR0 			3
#define HIF_VSC_INTR0 			4
#define HIF_HIF_INTR0 			5
#define HIF_MCE_INTR0 			6
#define HIF_VPIP_INTR0 			7
#define HIF_HPIP_INTR0 			8
#define HIF_MCU_INTR0 			9
#define HIF_TS_INTR0 			10
#define HIF_AMR_INTR0 			11

/* DPC  (HIF_INT0_MASK_REG) */
#define HIF_APU_INTR0_EN 		0
#define HIF_BLT_INTR0_EN 		1
#define HIF_MIF_INTR0_EN 		2
#define HIF_DPC_INTR0_EN 		3
#define HIF_VSC_INTR0_EN 		4
#define HIF_HIF_INTR0_EN 		5
#define HIF_MCE_INTR0_EN 		6
#define HIF_VPIP_INTR0_EN 		7
#define HIF_HPIP_INTR0_EN 		8
#define HIF_MCU_INTR0_EN 		9
#define HIF_TS_INTR0_EN 		10
#define HIF_AMR_INTR0_EN 		11

/* DPC  (HIF_INT1_STAT_REG) */
#define HIF_APU_INTR1 			0
#define HIF_BLT_INTR1 			1
#define HIF_MIF_INTR1 			2
#define HIF_DPC_INTR1 			3
#define HIF_VSC_INTR1 			4
#define HIF_HIF_INTR1 			5
#define HIF_MCE_INTR1 			6
#define HIF_VPIP_INTR1 			7
#define HIF_HPIP_INTR1 			8
#define HIF_MCU_INTR1 			9
#define HIF_TS_INTR1 			10
#define HIF_AMR_INTR1 			11
										
/* DPC  (HIF_INT1_MASK_REG) */
#define HIF_APU_INTR1_EN 		0
#define HIF_BLT_INTR1_EN 		1
#define HIF_MIF_INTR1_EN 		2
#define HIF_DPC_INTR1_EN 		3
#define HIF_VSC_INTR1_EN 		4
#define HIF_HIF_INTR1_EN 		5
#define HIF_MCE_INTR1_EN 		6
#define HIF_VPIP_INTR1_EN 		7
#define HIF_HPIP_INTR1_EN 		8
#define HIF_MCU_INTR1_EN 		9
#define HIF_TS_INTR1_EN 		10
#define HIF_AMR_INTR1_EN 		11

/* DPC  (PLL_SYSCLK_REG) */
#define SYSCLK_PLL_RESET 		0
#define SYSCLK_PLL_OEN 			1
#define SYSCLK_PLL_PD 			2
#define SYSCLK_PLL_BP 			3
#define SYSCLK_PLL_OD 			4, 2
#define SYSCLK_PLL_DIV2 		6
#define SYSCLK_PLL_GP_CTRL 	7
#define SYSCLK_PLL_RD 			8, 5
#define SYSCLK_PLL_CLKIN_SEL 	14
#define SYSCLK_PLL_CLKOUT_SEL 15
#define SYSCLK_PLL_FD 			16, 9

/* DPC  (PLL_MIFCLK_REG) */
#define MIFCLK_PLL_RESET 		0
#define MIFCLK_PLL_OEN 			1
#define MIFCLK_PLL_PD 			2
#define MIFCLK_PLL_BP 			3
#define MIFCLK_PLL_OD 			4, 2
#define MIFCLK_PLL_DIV2 		6
#define MIFCLK_PLL_GP_CTRL 	7
#define MIFCLK_PLL_RD 			8, 5
#define MIFCLK_PLL_CLKIN_SEL 	14
#define MIFCLK_PLL_CLKOUT_SEL 15
#define MIFCLK_PLL_FD 			16, 9

/* DPC  (PLL_DPCCLK_REG) */
#define DPCCLK_PLL_RESET 		0
#define DPCCLK_PLL_OEN 			1
#define DPCCLK_PLL_PD 			2
#define DPCCLK_PLL_BP 			3
#define DPCCLK_PLL_OD 			4, 2
#define DPCCLK_PLL_DIV2			6
#define DPCCLK_PLL_GP_CTRL 	7
#define DPCCLK_PLL_RD 			8, 5
#define DPCCLK_PLL_CLKIN_SEL 	14
#define DPCCLK_PLL_CLKOUT_SEL 15
#define DPCCLK_PLL_FD 			16, 9

/* DPC  (TSD_DMA_RD_CONFIG) */
#define TSD_DMA_LENGTH 			0, 8
#define TSD_DMA_VALID 			8
#define TSD_CAE 					9
#define TSD_DMA_DONE3 			13
#define TSD_DMA_DONE2 			14
#define TSD_DMA_DONE1 			15
#define TSD_DMA_RD_ADD 			16, 16

/* DPC  (TSD_DMA_WR_ADD) */
#define TSD_DMA_WRADDL 			0, 16
#define TSD_DMA_WRADDH 			16, 10

/* DPC  (TSD_IO_BYTELIM_CNT) */
#define TSD_BYTELIMIT_COUNTER 0, 16
#define TSD_BYTESTOP_COUNTER 	16, 16

/* DPC  (TSD_STC_CNT_LM) */
#define TSD_STC_COUNTER_L 		0, 16
#define TSD_STC_COUNTER_M 		16, 16

/* DPC  (TSD_STC_CNT_H) */
#define TSD_STC_COUNTER_H 		0, 10

/* DPC  (TSD_CON_VCXO) */
#define TSD_PWM_VALUE 			0, 16
#define TSD_DRAM_OFFSET 		16, 10
#define TSD_DRAM_FULL 			26
#define TSD_ENDIAN 				27
#define TSD_SOURCE 				29
#define TSD_EXTSYNC 				30
#define TSD_XPORT_ON 			31

/* DPC  (TSD_INT_STATUS) */
#define TSD_INT_ISQ 						4
#define TSD_INT_SYNC_ACQUIRED			5
#define TSD_INT_SYNC_LOST 				6
#define TSD_INT_ERROR_PACKET 			7
#define TSD_INT_PACKET_RECEIVED 		8
#define TSD_INT_PCR_RECEIVED 			9


#endif