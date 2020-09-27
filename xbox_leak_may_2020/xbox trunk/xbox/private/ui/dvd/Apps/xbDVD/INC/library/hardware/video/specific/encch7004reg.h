// FILE:      library\hardware\video\specific\encCH7004reg.h
// AUTHOR:    Sam Frantz
// COPYRIGHT: (c) 2000 Ravisent Technologies, Inc.  All Rights Reserved.
// CREATED:   10-MAR-2000
//
// PURPOSE: The class for the Chrontel 7004C video encoder
//
// HISTORY:

#ifndef ENC7004REG_H
#define ENC7004REG_H

// Device addresses
#define CH_DAB_ADDR0		(0xEC)		// ADDR pin low 
#define CH_DAB_ADDR1		(0xEA)		// ADDR pin high
// #define CH_DAB_ADDR0		(0x76)		// ADDR pin low 
// #define CH_DAB_ADDR1		(0x75)		// ADDR pin high

#define I2C_CH7004_ADDR_LOW	CH_DAB_ADDR0
#define I2C_CH7004_ADDR_HIGH	CH_DAB_ADDR1
 
// Register addresses
#define CH_DM			(0x00)		// Display Mode 
#define CH_FF			(0x01)		// Flicker Filter 
#define CH_VBW			(0x03)		// Video Bandwidth 
#define CH_IDF			(0x04)		// Input Data Format 
#define CH_CM			(0x06)		// Clock Mode 
#define CH_SAV			(0x07)		// Start Active Video 
#define CH_PO			(0x08)		// Position Overflow 
#define CH_BL			(0x09)		// Black Level 
#define CH_HP			(0x0A)		// Horizontal Position 
#define CH_VP			(0x0B)		// Vertical Position 
#define CH_SP			(0x0D)		// Sync Polarity 
#define CH_PM			(0x0E)		// Power Management 
#define CH_CD			(0x10)		// Connection Detect 
#define CH_CE			(0x11)		// Contrast Enhancement 
#define CH_MNE			(0x13)		// PLL M and N extra bits 
#define CH_PLLM			(0x14)		// PLL-M Value 
#define CH_PLLN			(0x15)		// PLL-N Value 
#define CH_BCO			(0x17)		// Buffered Clock 
#define CH_FSCI0		(0x18)		// Subcarrier Frequency Adjust FSCI[31:28] 
#define CH_FSCI1		(0x19)		// Subcarrier Frequency Adjust FSCI[27:24] 
#define CH_FSCI2		(0x1A)		// Subcarrier Frequency Adjust FSCI[23:20] 
#define CH_FSCI3		(0x1B)		// Subcarrier Frequency Adjust FSCI[19:16] 
#define CH_FSCI4		(0x1C)		// Subcarrier Frequency Adjust FSCI[15:12] 
#define CH_FSCI5		(0x1D)		// Subcarrier Frequency Adjust FSCI[11:8] 
#define CH_FSCI6		(0x1E)		// Subcarrier Frequency Adjust FSCI[7:4] 
#define CH_FSCI7		(0x1F)		// Subcarrier Frequency Adjust FSCI[3:0] 
#define CH_PLLC			(0x20)		// PLL and Memory Control 
#define CH_CIVC			(0x21)		// CIV Control 
#define CH_CIV0			(0x22)		// Calculated Fsc Increment Value CIV[23:16] 
#define CH_CIV1			(0x23)		// Calculated Fsc Increment Value CIV[15:8] 
#define CH_CIV2			(0x24)		// Calculated Fsc Increment Value CIV[7:0] 
#define CH_VID			(0x25)		// Version ID 
#define CH_TEST0		(0x26)		// Test - undocumented 
#define CH_TEST1		(0x27)		// Test - undocumented 
#define CH_TEST2		(0x28)		// Test - undocumented 
#define CH_TEST3		(0x29)		// Test - undocumented 
#define CH_A			(0x2A)		// Address 

// Macrovision registers
#define CH_MV_VERSION_ID		(0x32)		// If not this part, do not play macrovision-protected content
#define CH_MV_BPST1A			(0x01)		// Back Porch AGC Pulse Starting Point1
#define CH_MV_BPST1B			(0x02)		// Back Porch AGC Pulse Starting Point1
#define CH_MV_BPWD1			(0x04)		// Back Porch AGC Pulse Ending Pixel
#define CH_MV_BPWD2			(0x05)		// Back Porch AGC Pulse Ending Pixel
#define CH_MV_BPED1			(0x08)		// Back Porch AGC Pulse Ending Point1
#define CH_MV_BPST2A			(0x0E)		// Back Porch AGC Pulse Starting Point2
#define CH_MV_BPST2B			(0x0C)		// Back Porch AGC Pulse Starting Point2
#define CH_MV_BPED2A			(0x17)		// Back Porch AGC Pulse Ending Point2
#define CH_MV_BPED2B			(0x16)		// Back Porch AGC Pulse Ending Point2

#define CH_MV_PSLA_HI	(0x31)
#define CH_MV_Z1S	(0x2C)
#define CH_MV_Z2S	(0x2D)
#define CH_MV_ASB	(0x2E)
#define CH_MV_PSSA	(0x35)
#define CH_MV_PSSB	(0x36)
#define CH_MV_PSLA	(0x31)
#define CH_MV_PSLB	(0x32)

#define CH_MV_PZ		(0x11)
#define CH_MV_PXL		(0x12)
#define CH_MV_LPCS		(0x13)
#define CH_MV_CSPF		(0x0D)
#define CH_MV_CSSP		(0x0F)
#define CH_MV_FL1F		(0x2F)
#define CH_MV_FL2F		(0x30)
#define CH_MV_F1SPA		(0x19)
#define CH_MV_F1SPB		(0x18)
#define CH_MV_F2SPA		(0x19)
#define CH_MV_F2SPB		(0x1A)
#define CH_MV_AGCD		(0x2A)
#define CH_MV_PSDA		(0x33)
#define CH_MV_PSDB		(0x34)
#define CH_MV_PSCA		(0x3B)
#define CH_MV_PSCB		(0x3C)
#define CH_MV_PSON1		(0x37)
#define CH_MV_PSON2		(0x38)
#define CH_MV_PSFM1		(0x39)
#define CH_MV_PSFM2		(0x3A)
#define CH_MV_AGCY		(0x3D)

// Programmable bit AGCY determines either the normal or static mode for 
// AGC pulses. In the normal mode, all the 60 Active Line AGC pulses remain 
// at the high level (810 mV) for approximately 12 +2/-0.5 seconds, and then 
// decay to blanking level (zero) over 2.5 ñ0.5 seconds. Within 0 to 500 ms 
// of reaching blanking level, the AGC pulses commence the pulsation mode 
// (either A or B) and remain in that mode for 12 ñ1.0 seconds. Then the AGC 
// pulses rise to the high level over another 2.5 ñ0.5 seconds, commencing 
// from pulsation mode state B. The complete cycle takes about 30 seconds.
#define CH_MV_AGCY_CYCLING		(0x04)
#define CH_MV_AGCY_STATIC		(0x00)

// Display Mode - 0x00
#define CH_DM_SR		(0x07)		// Scale ratio 
#define CH_DM_SR_5_4		(0x00)		// Scale 5/4 
#define CH_DM_SR_1_1		(0x01)		// Scale 1/1 
#define CH_DM_SR_7_8		(0x02)		// Scale 7/8 
#define CH_DM_SR_5_6		(0x03)		// Scale 5/6 
#define CH_DM_SR_3_4		(0x04)		// Scale 3/4 
#define CH_DM_SR_7_10		(0x05)		// Scale 7/10 

#define CH_DM_VOS		(0x18)		// Output format 
#define CH_DM_VOS_PAL		(0x00)		// Output PAL 
#define CH_DM_VOS_NTSC		(0x08)		// Output NTSC 
#define CH_DM_VOS_PAL_M		(0x10)		// Output PAL-M 
#define CH_DM_VOS_NTSC_J	(0x18)		// Output NTSC-J 

#define CH_DM_IR		(0xE0)		// Input resolution 
#define CH_DM_IR_512_384	(0x00)		// Input 512x384 
#define CH_DM_IR_720_400	(0x20)		// Input 720x400 
#define CH_DM_IR_640_400	(0x40)		// Input 640x400 
#define CH_DM_IR_640_480	(0x60)		// Input 640x480 
#define CH_DM_IR_800x600	(0x80)		// Input 800x600 
#define CH_DM_IR_720x576	(0xA0)		// Input 720x576 (PAL) CH7004
#define CH_DM_IR_720x480	(0xA0)		// Input 720x480 (NTSC) CH7004

// Flicker Filter - 0x01
#define CH_FF_DEFAULT		(0x32)		// bits are 00110010
#define CH_FF_DVD		(0x02)		// bits are 00000010
#define CH_FF_WEB		(0x28)		// bits are 00101000

#define CH_FF_NONSCALED		(0x03)		// Non-scaled (1:1) modes 
#define CH_FF_0_1_0		(0x00)		// 0:1:0 - No flicker filtering 
#define CH_FF_1_2_1		(0x01)		// 1:2:1 - Moderate filtering 
#define CH_FF_1_3_1		(0x02)		// 1:3:1 - Low filtering 
#define CH_FF_1_1_1		(0x03)		// 1:1:1 - High filtering 

#define CH_FF_SCALED		(0x02)		// Scaled modes 
#define CH_FF_3_LINE		(0x00)		// 3-line filtering  
#define CH_FF_4_LINE		(0x02)		// 4-line filtering  


// Video Bandwidth - 0x03
#define CH_VBW_YCV		(0x01)		// Luma composite bandwidth 
#define CH_VBW_YCV_LOW		(0x00)		// Luma composite low bandwidth 
#define CH_VBW_YCV_HIGH		(0x01)		// Luma composite high bandwidth 

#define CH_VBW_YSV		(0x06)		// Luma S-video bandwidth 
#define CH_VBW_YSV_LOW		(0x00)		// Luma S-video low bandwidth 
#define CH_VBW_YSV_MEDIUM	(0x02)		// Luma S-video medium bandwidth 
#define CH_VBW_YSV_HIGH		(0x04)		// Luma S-video high bandwidth 

#define CH_VBW_YPEAK		(0x08)		// Luma S-video Y-peaking enabled 

#define CH_VBW_CBW		(0x30)		// Chroma composite bandwidth 
#define CH_VBW_CBW_LOW		(0x00)		// Chroma composite low bandwidth 
#define CH_VBW_CBW_MEDIUM	(0x10)		// Chroma composite medium bandwidth 
#define CH_VBW_CBW_MEDHIGH	(0x20)		// Chroma composite med-high bandwidth 
#define CH_VBW_CBW_HIGH		(0x30)		// Chroma composite high bandwidth 

#define CH_VBW_CVBW		(0x40)		// Composite black & white 

#define CH_VBW_FLFF		(0x80)		// 5-line filter in 7/10 scaling 


// Input Data Format - 0x04
#define CH_IDF_FORMAT		(0x0F)		// Input data format 
#define CH_IDF_16_NOMUX_RGB_565	(0x00)		// 16-bit non-muxed RGB (16-bit colour, 5-6-5) input 
#define CH_IDF_16_NOMUX_YCRCB	(0x01)		// 16-bit non-muxed YCrCb (24-bit colour) input (Y non-muxed, CrCb muxed) 
#define CH_IDF_16_MUX_RGB_24	(0x02)		// 16-bit muxed RGB (24-bit colour) input 
#define CH_IDF_15_NOMUX_RGB	(0x03)		// 15-bit non-muxed RGB (15-bit colour, 5-5-5) input 
#define CH_IDF_12_MUX_RGB_24_C	(0x04)		// 12-bit muxed RGB (24-bit colour) input ("C" mux scheme) 
#define CH_IDF_12_MUX_RGB_24_I	(0x05)		// 12-bit muxed RGB (24-bit colour) input ("I" mux scheme) 
#define CH_IDF_8_MUX_RGB_888	(0x06)		// 8-bit muxed RGB (24-bit colour, 8-8-8) input 
#define CH_IDF_8_MUX_RGB_565	(0x07)		// 8-bit muxed RGB (16-bit colour, 5-6-5) input 
#define CH_IDF_8_MUX_RGB_555	(0x08)		// 8-bit muxed RGB (15-bit colour, 5-5-5) input 
#define CH_IDF_8_MUX_YCRCB	(0x09)		// 8-bit muxed YCrCb (24-bit colour) input (Y, Cr and Cb are muxed 

#define CH_IDF_RGBBP		(0x20)		// RGB bypass mode enabled (100-pin package only) 

#define CH_IDF_DACG		(0x40)		// D/A converter gain 
#define CH_IDF_DACG_LOW		(0x00)		// 71uA for NTSC and PAL-M 
#define CH_IDF_DACG_HIGH	(0x40)		// 76uA for PAL and NTSC-J 


// Clock Mode - 0x06
#define CH_CM_CFRB_FREE_RUN	(0x00)		// When ACIV=1, p. 36
#define CH_CM_CFRB_LOCK_TO_VTV	(0x80)		// When ACIV=0, p. 36

#define CH_CM_PCM		(0x03)		// P-Out multiplier 
#define CH_CM_PCM_1X		(0x00)		// P-Out = 1 x pixel clock 
#define CH_CM_PCM_2X		(0x01)		// P-Out = 2 x pixel clock 
#define CH_CM_PCM_3X		(0x02)		// P-Out = 3 x pixel clock 

#define CH_CM_XCM		(0x0C)		// XCLK multiplier 
#define CH_CM_XCM_1X		(0x00)		// XCLK = 1 x pixel clock 
#define CH_CM_XCM_2X		(0x04)		// XCLK = 2 x pixel clock 
#define CH_CM_XCM_3X		(0x08)		// XCLK = 3 x pixel clock 

#define CH_CM_MCP		(0x10)		// Clock polarity 
#define CH_CM_MCP_NEG		(0x00)		// Latch input data on negative edge of clock 
#define CH_CM_MCP_POS		(0x10)		// Latch input data on positive edge of clock 

#define CH_CM_MS		(0x40)		// Master / slave 
#define CH_CM_MS_SLAVE		(0x00)		// Slave clock mode 
#define CH_CM_MS_MASTER		(0x40)		// Master clock mode 

#define CH_CM_CFRB		(0x80)		// Chroma subcarrier free-run or lock 
#define CH_CM_CFRB_FREE		(0x00)		// Chroma subcarrier free-runs              - use when ACIV = 1 
#define CH_CM_CFRB_LOCK		(0x80)		// Chroma subcarrier locked to video signal - use when ACIV = 0 


// Start Active Video - 0x07
 

// Position Overflow - 0x08
#define CH_PO_VP		(0x01)		// MSB of vertical position value 

   // Split overflow bit from 9-bit VP 	
#define CH_PO_VP_W(vp)	\
	(((vp) >> 8) & 0x01)			// CH_PO_VP = VP[8] 

   // Recombine byte and overflow bit 	
#define CH_VP_R(vpo,vp)		\
	((((int)(vpo) & 0x01) << 8)							\
	|(((int)(vp ) & 0xFF) << 0))

#define CH_PO_HP		(0x02)		// MSB of horizontal position value 

   // Split overflow bit from 9-bit HP 
#define CH_PO_HP_W(hp)	\
	(((hp) >> 7) & 0x02)			// CH_PO_HP = HP[8] 

   // Recombine byte and overflow bit
#define CH_HP_R(hpo,hp)	\
	((((int)(hpo) & 0x02) << 7)							\
	|(((int)(hp ) & 0xFF) << 0))

#define CH_PO_SAV		(0x04)		// MSB of start of active video value 

   // Split overflow bit from 9-bit SAV
#define CH_PO_SAV_W(sav)	\
	(((hp) >> 6) & 0x04)			// CH_PO_SAV = SAV[8] 

   // Recombine byte and overflow bit
#define CH_SAV_R(savo,sav)	\
	((((int)(savo) & 0x04) << 6)							\
	|(((int)(sav ) & 0xFF) << 0))


// Black Level - 0x09
#define CH_BL_MIN		(80)		// Minimum black level 
#define CH_BL_MAX		(208)		// Maximum black level 
#define CH_BL_NTSC		(127)		// Recommended value for NTSC 
#define CH_BL_PAL_M		(127)		// Recommended value for PAL-M 
#define CH_BL_PAL		(105)		// Recommended value for PAL 
#define CH_BL_NTSC_J		(100)		// Recommended value for NTSC-J 
#define CH_BL_SCART		(0)		// Must be zero in SCART mode 


// Sync Polarity - 0x0D
#define CH_SP_HSP		(0x01)		// Horizontal sync polarity 
#define CH_SP_HSP_LOW		(0x00)		// Horizontal sync active low 
#define CH_SP_HSP_HIGH		(0x01)		// Horizontal sync active high 

#define CH_SP_VSP		(0x02)		// Vertical sync polarity 
#define CH_SP_VSP_LOW		(0x00)		// Vertical sync active low 
#define CH_SP_VSP_HIGH		(0x02)		// Vertical sync active high 

#define CH_SP_SYO_OUTPUT	(0x04)		// H and V sync are output from CH7003 

#define CH_SP_DES		(0x08)		// Detect embedded sync in pixel stream - YCrCb only 


// Power Management - 0x0E
#define CH_PM_PD		(0x07)		// Power down modes 
#define CH_PM_PD_COMP_OFF	(0x00)		// Composite off - CVBS DAC is powered down 
#define CH_PM_PD_POWER_DOWN	(0x01)		// Power down - most pins and circuitry disabled 
#define CH_PM_PD_SVID_OFF	(0x02)		// S-video off - S-video DACs are powered down 
#define CH_PM_PD_NORMAL		(0x03)		// All circuits and pins are active 
#define CH_PM_PD_FULL_DOWN	(0x04)		// Full power down - all circuitry powered down 

#define CH_PM_NRESET		(0x08)		// Held in soft reset while zero 

#define CH_PM_SCART		(0x10)		// SCART mode - RGB output with composite sync 


// Connection Detect - 0x10
#define CH_CD_SENSE		(0x01)		// Output sense currents - write only 

#define CH_CD_CVBST		(0x02)		// S-video connection detect - reads 0 if connected 

#define CH_CD_CT		(0x04)		// C connection detect - reads 0 if connected 

#define CH_CD_YT		(0x08)		// Y connection detect - reads 0 if connected 


// Contrast Enhancement - 0x11
#define CH_CE_MASK		(0x07)		// Contrast enhancement value 
#define CH_CE_BLACK		(0x00)		// Minimum value - enhances black 
#define CH_CE_NORMAL		(0x03)		// Normal value - no enhancement 
#define CH_CE_WHITE		(0x07)		// Maximum value - enhances white 


// PLL M and N extra bits - 0x13
#define CH_MNE_M		(0x01)		// MSB of PLL M value 

  // Split overflow bit from 9-bit M 	
#define CH_MNE_M_W(m)	\
	(((m) >> 8) & 0x01)			// CH_MNE_M = M[8] 
  // Recombine byte and overflow bit 	
#define CH_M_R(mne,pllm)	\
	((((int)(mne)  & 0x01) << 8)							\
	|(((int)(pllm) & 0xFF) << 0))

#define CH_MNE_N		(0x06)		// MSBs of PLL N value 

  // Split overflow bits from 10-bit N 	
#define CH_MNE_N_W(n)	\
	(((n) >> 7) & 0x06)			// CH_MNE_N = N[9:8] 

  // Recombine byte and overflow bits 	
#define CH_N_R(mne,plln)	\
	((((int)(mne)  & 0x06) << 7)							\
	|(((int)(plln) & 0xFF) << 0))


// PLL-M Value - 0x14
// PLL-N Value - 0x15


// Buffered Clock Output - 0x17
#define CH_BCO_SCO		(0x07)		// Select clock source 
#define CH_BCO_SCO_14MHZ	(0x00)		// 14 MHz crystal 
#define CH_BCO_SCO_VCO_K3	(0x02)		// VCO divided by K3 
#define CH_BCO_SCO_FIELD	(0x03)		// Field ID signal 

#define CH_BCO_SHF		(0x38)		// Value of K3 for CH_BCO_SCO_VCO_K3, above 
#define CH_BCO_SHF_2_5		(0x00)		// K3 = 2.5 
#define CH_BCO_SHF_3_0		(0x08)		// K3 = 3.0 
#define CH_BCO_SHF_3_5		(0x10)		// K3 = 3.5 
#define CH_BCO_SHF_4_0		(0x18)		// K3 = 4.0 
#define CH_BCO_SHF_4_5		(0x20)		// K3 = 4.5 
#define CH_BCO_SHF_5_0		(0x28)		// K3 = 5.0 
#define CH_BCO_SHF_6_0		(0x30)		// K3 = 6.0 
#define CH_BCO_SHF_7_0		(0x38)		// K3 = 7.0 


// Subcarrier Frequency Adjust - 0x18..0x1F

// values from table 28 and table 29 on page 44 of CH7004C
#define CH_FSCI_INTERLACED_NTSC_NORMAL		569408543	// mode 26
#define CH_FSCI_INTERLACED_NTSC_NO_CRAWL	569410927	// mode 26
#define CH_FSCI_INTERLACED_PAL_M_NORMAL	568782819	// mode 26
#define CH_FSCI_INTERLACED_PAL_NORMAL		705268427	// mode 25
#define CH_FSCI_INTERLACED_PAL_N_NORMAL	569807942	// mode 25

#define CH_FSCI_PROGRESSIVE_NTSC_NORMAL_MODE_11	516986804
#define CH_FSCI_PROGRESSIVE_NTSC_NO_CRAWL_MODE_11	516988968
#define CH_FSCI_PROGRESSIVE_PAL_M_NORMAL_MODE_11	516418687

#define CH_FSCI_PROGRESSIVE_PAL_NORMAL_MODE_14		644816848
#define CH_FSCI_PROGRESSIVE_PAL_N_NORMAL_MODE_14	520967262

#define CH_FSCI_PROGRESSIVE_NTSC_NORMAL_MODE_17	545259520
#define CH_FSCI_PROGRESSIVE_NTSC_NO_CRAWL_MODE_17	545261803
#define CH_FSCI_PROGRESSIVE_PAL_M_NORMAL_MODE_17	544660334

#define CH_FSCI_PROGRESSIVE_NTSC_NORMAL_MODE_18	508908885
#define CH_FSCI_PROGRESSIVE_NTSC_NO_CRAWL_MODE_18	508911016
#define CH_FSCI_PROGRESSIVE_PAL_M_NORMAL_MODE_18	508349645

// for mode 25 & 26, see table 25, p. 42 of CH7004C
#define CH_PLLN_INTERLACED_NTSC		31
#define CH_PLLM_INTERLACED_NTSC		33
#define CH_PLLN_INTERLACED_PAL		31
#define CH_PLLM_INTERLACED_PAL		33

// for mode 25 & 26, see table 25, p. 42 of CH7004C
#define CH_PLLN_PROGRESSIVE_NTSC_MODE_11		22
#define CH_PLLM_PROGRESSIVE_NTSC_MODE_11		11
#define CH_PLLN_PROGRESSIVE_PAL_MODE_14			9
#define CH_PLLM_PROGRESSIVE_PAL_MODE_14			4
#define CH_PLLN_PROGRESSIVE_NTSC_MODE_17		126
#define CH_PLLM_PROGRESSIVE_NTSC_MODE_17		63
#define CH_PLLN_PROGRESSIVE_NTSC_MODE_18		190
#define CH_PLLM_PROGRESSIVE_NTSC_MODE_18		89

#define CH_PLLN_PROGRESSIVE_NTSC	CH_PLLN_PROGRESSIVE_NTSC_MODE_11
#define CH_PLLM_PROGRESSIVE_NTSC 	CH_PLLM_PROGRESSIVE_NTSC_MODE_11

#define CH_PLLN_PROGRESSIVE_PAL 	CH_PLLN_PROGRESSIVE_PAL_MODE_14
#define CH_PLLM_PROGRESSIVE_PAL 	CH_PLLM_PROGRESSIVE_PAL_MODE_14

 		// Split 32-bit FSCI value into nibbles
#define CH_FSCI0_W(f)	(((f) >> 28) & 0x0F)	// CH_FSCI0 = FSCI[31:28] 
#define CH_FSCI1_W(f)	(((f) >> 24) & 0x0F)	// CH_FSCI0 = FSCI[27:24] 
#define CH_FSCI2_W(f)	(((f) >> 20) & 0x0F)	// CH_FSCI0 = FSCI[23:20] 
#define CH_FSCI3_W(f)	(((f) >> 16) & 0x0F)	// CH_FSCI0 = FSCI[19:16] 
#define CH_FSCI4_W(f)	(((f) >> 12) & 0x0F)	// CH_FSCI0 = FSCI[15:12] 
#define CH_FSCI5_W(f)	(((f) >>  8) & 0x0F)	// CH_FSCI0 = FSCI[11: 8] 
#define CH_FSCI6_W(f)	(((f) >>  4) & 0x0F)	// CH_FSCI0 = FSCI[ 7: 4] 
#define CH_FSCI7_W(f)	(((f) >>  0) & 0x0F)	// CH_FSCI0 = FSCI[ 3: 0] 

   // Recombine nibbles to make 32-bit word 
#define CH_FSCI_R(f0,f1,f2,f3,f4,f5,f6,f7)	\
	((((int)(f0) & 0x0F) << 28)								\
	|(((int)(f1) & 0x0F) << 24)								\
	|(((int)(f2) & 0x0F) << 20)								\
	|(((int)(f3) & 0x0F) << 16)								\
	|(((int)(f4) & 0x0F) << 12)								\
	|(((int)(f5) & 0x0F) <<  8)								\
	|(((int)(f6) & 0x0F) <<  4)								\
	|(((int)(f7) & 0x0F) <<  0))


// PLL and Memory Control - 0x20
#define CH_PLLC_MEM5V		(0x01)		// Memory supply is 5 volts 

#define CH_PLLC_PLL5VA		(0x02)		// PLL analog supply is 5 volts 

#define CH_PLLC_PLL5VD		(0x04)		// PLL digital supply is 5 volts 

#define CH_PLLC_PLLS		(0x08)		// PLL stages - set if PLL5VA is set 

#define CH_PLLC_PLLCAP		(0x10)		// PLL filter capacitor 

#define CH_PLLC_PLLCHI		(0x20)		// PLL charge pump current - for emergency use only 


// CIV Control - 0x21
#define CH_CIVC_ACIV		(0x01)		// Automatic calculated increment value 

#define CH_CIVC_CIVH_MASK	(0x06)		// CIV hysteresis 
#define CH_CIVC_CIVH_SHIFT	(1)		// Shift left by this and mask with CH_CIVC_CIVH_MASK 


// Calculated Increment Value - 0x22..0x24
			// Split 24-bit CIV value into bytes
#define CH_CIV0_W(civ)	(((civ) >> 16) & 0xFF)	// CH_CIV0 = CIV[23:16] 
#define CH_CIV1_W(civ)	(((civ) >>  8) & 0xFF)	// CH_CIV1 = CIV[15: 8] 
#define CH_CIV2_W(civ)	(((civ) >>  0) & 0xFF)	// CH_CIV2 = CIV[ 7: 0] 

   // Recombine bytes to make 24-bit value 
#define CH_CIV_R(civ0,civ1,civ2)	\
	((((int)(civ0) & 0xFF) << 16)								\
	|(((int)(civ1) & 0xFF) <<  8)								\
	|(((int)(civ2) & 0xFF) <<  0))


// Version ID - 0x25
#define CH_VID_CH7003		(0x00)
#define CH_VID_CH7003A		(0x01)

// macros added for to eliminate constants in Teknema source
#define INITIAL_PAL_POSX			(58)
#define INITIAL_PAL_POSY			(22)
#define INITIAL_NTSC_POSX			(56)	// depends on build settings
#define INITIAL_NTSC_POSY			(-2)	// ?investigate
#define INITIAL_NTSCJ_POSX			(48)	// ?investigate
#define INITIAL_NTSCJ_POSY			(15)	// ?investigate
#endif

