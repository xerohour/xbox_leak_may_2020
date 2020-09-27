//
// FILE:      library\hardware\pcibridge\specific\bt848\bt848reg.h
// AUTHOR:    P.C. Straasheijm
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   9.10.96
//
// PURPOSE:   Register definitions for the bt848 chip
// 
// HISTORY:
//
//

#ifndef BT848REG_H
#define BT848REG_H                                            

#include "library\common\prelude.h"

//
//Device Status Register
//

#define BT848REG_DEVSTAT		0x00

#define BT848IDX_PRES			7
#define BT848IDX_HLOC			6
#define BT848IDX_FIELD			5
#define BT848IDX_NUML			4
#define BT848IDX_CSEL			3
#define BT848IDX_LOF          1
#define BT848IDX_COF          0
                                                       
//
//Input Format Register
//

#define BT848REG_INPUTFOR		0x04

#define BT848IDX_HACTIVE		7
#define BT848IDX_MUXSEL			5, 2
#define BT848IDX_XTSEL			3, 2
#define BT848IDX_FORMAT			0, 3


//
//Temporal Decimation Register
//
                              
                              
#define BT848REG_TEMDECR		0x08

#define BT848IDX_DEC_FIELD		7
#define BT848IDX_FLDALIGN		6
#define BT848IDX_DEC_RAT		0, 6


//
//MSB Cropping Register EVEN
//

#define BT848REG_MSB_CROPR_E	0x0C

#define BT848IDX_VDELAY_E		6, 2
#define BT848IDX_VACTIV_E		4, 2
#define BT848IDX_HDELAY_E		2, 2
#define BT848IDX_HAKTIV_E		0, 2


//
//MSB Cropping Register ODD
//

#define BT848REG_MSB_CROPR_O	0x8C

#define BT848IDX_VDELAY_O		6, 2
#define BT848IDX_VACTIV_O		4, 2
#define BT848IDX_HDELAY_O		2, 2
#define BT848IDX_HAKTIV_O		0, 2


//
//Vertical Delay Register EVEN
//

#define BT848REG_VDELAYL_E		0x10


//
//Vertical Delay Register ODD
//

#define BT848REG_VDELAYL_O		0x90


//
//Vertical Activ Register EVEN
//

#define BT848REG_VACTIVL_E		0x14


//
//Vertical Activ Register ODD
//

#define BT848REG_VACTIVL_O		0x94


//
//Horizontal Delay Register EVEN
//

#define BT848REG_HDELAYL_E		0x18


//
//Horizontal Delay Register ODD
//

#define BT848REG_HDELAYL_O		0x98


//
//Horizontal Activ Register EVEN
//

#define BT848REG_HACTIVL_E		0x1C


//
//Horizontal Activ Register ODD
//

#define BT848REG_HACTIVL_O		0x9C


//
//Horizontal Scaling Register EVEN UPPER
//

#define BT848REG_HSCALH_E		0x20


//
//Horizontal Scaling Register ODD UPPER
//

#define BT848REG_HSCALH_O		0xA0
                                  
                                  
//
//Horizontal Scaling Register EVEN LOWER
//

#define BT848REG_HSCALL_E		0x24


//
//Horizontal Scaling Register ODD LOWER
//

#define BT848REG_HSCALL_O		0xA4


//
//Brightness Control Register
//

#define BT848REG_BRCONTR		0x28


//
//Miscellaneous Control Register
//

#define BT848REG_MISCEL_O		0xAC

#define BT848IDX_LNOTCH_0		7
#define BT848IDX_COMP_O			6
#define BT848IDX_LDEC_O			5
#define BT848IDX_CBSENSE_O		4
#define BT848IDX_CON_MSB_O		2
#define BT848IDX_SAT_U_MSB_O	1
#define BT848IDX_SAT_V_MSB_O	0


#define BT848REG_MISCEL_E		0x2C

#define BT848IDX_LNOTCH_E		7
#define BT848IDX_COMP_E			6
#define BT848IDX_LDEC_E			5
#define BT848IDX_CBSENSE_E		4
#define BT848IDX_CON_MSB_E		2
#define BT848IDX_SAT_U_MSB_E	1
#define BT848IDX_SAT_V_MSB_E	0


//
//Luma Gain Register
//

#define BT848REG_CONTRAST_L	0x30

//
//Chroma Gain (U) Register
//

#define BT848REG_SAT_U_L      0x34

//                                
//Chroma Gain (V) Register
//

#define BT848REG_SAT_V_L		0x38

//
//HUE Control Register
//

#define BT848REG_HUE				0x3C


//
//SC Loop Control Register  EVEN
//

#define BT848REG_SCLOOP_E		0x40

#define BT848IDX_CAGC_E 		6
#define BT848IDX_CKILL_E		5
#define BT848IDX_HFILT_E		3, 2


//
//SC Loop Control Register  ODD
//

#define BT848REG_SCLOOP_O		0xC0

#define BT848IDX_CAGC_O 		6
#define BT848IDX_CKILL_O		5
#define BT848IDX_HFILT_O		3, 2


//
//Output Format Register
//


#define BT848REG_OUTFORM		0x48

#define BT848IDX_RANG			7
#define BT848IDX_CORE			5, 2

//
//Vertical Scaling Upper EVEN
//

#define BT848REG_VSCALE_HI_E	0x4C

#define BT848IDX_YCOMB_HI_E	7
#define BT848IDX_COMB_HI_E		6
#define BT848IDX_INT_HI_E		5
#define BT848IDX_VSCALE_HI_E	0, 5


//
//Vertical Scaling Upper ODD
//

#define BT848REG_VSCALE_HI_O	0xCC

#define BT848IDX_YCOMB_HI_O	7
#define BT848IDX_COMB_HI_O		6
#define BT848IDX_INT_HI_O		5
#define BT848IDX_VSCALE_HI_O	0, 5


//
//Vertical Scaling Register EVEN
//


#define BT848REG_VSCALE_LO_E	0x50


//
//Vertical Scaling Register ODD
//


#define BT848REG_VSCALE_LO_O	0xD0


//
//Test Control Register
//


#define BT848REG_TEST			0x54


//
//AGC Delay Register
//

#define BT848REG_ADELAY			0x60


//
//Burst Delay Register
//

#define BT848REG_BDELAY			0x5D


//
//ADC Interface Register
//

#define BT848REG_ADC				0x68

#define BT848IDX_SYNC_T			5
#define BT848IDX_AGC_EN			4
#define BT848IDX_CLK_SLEEP		3
#define BT848IDX_Y_SLEEP		2
#define BT848IDX_C_SLEEP		1
#define BT848IDX_CRUSH			0


//
//Video Timing Control EVEN
//

#define BT848REG_E_VTC   		0x6C

#define BT848IDX_HSFMT_E		7
#define BT848IDX_VFILT_E		0, 2


//
//Video Timing Control ODD
//

#define BT848REG_O_VTC   		0xEC

#define BT848IDX_HSFMT_O		7
#define BT848IDX_VFILT_O		0, 2


//
//Software Reset Register
//

#define BT848REG_SRESET			0x7C 
#define BT848IDX_SRESET			0


//
//Color Format Register
//

#define BT848REG_COLOR_FMT		0xD4

#define BT848IDX_COLOR_ODD		4, 4
#define BT848IDX_COLOR_EVEN	0, 0


//
// Color control register
//

#define BT848REG_COLOR_CTL		0xD8

#define BT848IDX_EXT_FRMRATE	7
#define BT848IDX_COLOR_BARS	6
#define BT848IDX_RGB_DED		5
#define BT848IDX_GAMMA			4
#define BT848IDX_WSWAP_ODD		3
#define BT848IDX_WSWAP_EVEN	2
#define BT848IDX_BSWAP_ODD		1
#define BT848IDX_BSWAP_EVEN	0


//
//Capture Control
//

#define BT848REG_CAP_CTL		0xDC

#define BT848IDX_DITH_FRAME	4
#define BT848IDX_CAP_VBI_ODD	3
#define BT848IDX_CAP_VBI_EVEN	2
#define BT848IDX_CAPTURE_ODD	1
#define BT848IDX_CAPTURE_EVEN 0


//
//VBI Packet size
//

#define BT848REG_VBI_PCK_SIZE	0xE0


//
//VBI Packet Size / DELAY
//


#define BT848REG_VBI_PCK_DEL	0xE4

#define BT848IDX_VBI_HDELAY	2, 6
#define BT848IDX_EXT_FRAME		1
#define BT848IDX_VBI_PKT_HI	0

//
//Interupt Status
//

#define BT848REG_INT_STAT		0x100

#define BT848IDX_RISCS			28, 4
#define BT848IDX_RISC_EN_I		27
#define BT848IDX_RACK			25
#define BT848IDX_FIELD_I		24
#define BT848IDX_SCERR			19
#define BT848IDX_OCERR			18
#define BT848IDX_PABORT			17
#define BT848IDX_RIPERR			16
#define BT848IDX_PPERR			15
#define BT848IDX_FDSR			14
#define BT848IDX_FTRGT			13
#define BT848IDX_FBUS			12
#define BT848IDX_RISCI			11
#define BT848IDX_GPINT			9
#define BT848IDX_I2CDONE		8
#define BT848IDX_VPRES			5
#define BT848IDX_HLOCK			4
#define BT848IDX_OFLOW			3
#define BT848IDX_HSYNC			2
#define BT848IDX_VSYNC			1
#define BT848IDX_FMTCHG			0


//
//Interupt Mask
//

#define BT848REG_INT_MASK		0x104


//
//RISC Program Counter
//

#define BT848REG_RISC_COUNT	0x120


//
//Risc Program Start Address
//

#define BT848REG_RISC_STRT_ADD	0x114


//
//GPIO and DMA Control
//


#define BT848REG_GPIO_DMA_CTL	0x10C

#define BT848IDX_GPINTC			15
#define BT848IDX_GPINTI			14
#define BT848IDX_GPWEC			13
#define BT848IDX_GPIOMODE		11, 2
#define BT848IDX_GPCLKMODE		10
#define BT848IDX_PLTP23			6, 2
#define BT848IDX_PLTP1			4, 2
#define BT848IDX_PKTP			2, 2
#define BT848IDX_RISC_EN		1
#define BT848IDX_FIFO_EN		0


//
//GPIO Output enable Control
//


#define BT848REG_GPIO_OUT_EN	0x118


//
//GPIO Registered Input Control
//


#define BT848REG_GPIO_REG_INP	0x11C


//
//GPIO Data I/O
//


#define BT848REG_GPIO_DATA		0x200  // till 0x2FF

#define GPIO_DATA_SIZE			0x100


//
//I2C Data/Control
//


#define BT848REG_I2C_DATA_CTL 0x110

#define BT848IDX_I2CCDB0		24, 8
#define BT848IDX_I2CCDB1		16, 8
#define BT848IDX_I2CCDB2		8,  8
#define BT848IDX_I2CDIV			4,  4
#define BT848IDX_I2CSYNC		3
#define BT848IDX_I2CW3B			2
#define BT848IDX_I2CSCL			1
#define BT848IDX_I2CSDA			0


//
// For YUV2RGB
//
#define BT848_YUV422				0x00
#define BT848_RGB24				0x01
#define BT848_RGB16				0x02
#define BT848_RGB15				0x03

//__________________________________________________________

#endif   
