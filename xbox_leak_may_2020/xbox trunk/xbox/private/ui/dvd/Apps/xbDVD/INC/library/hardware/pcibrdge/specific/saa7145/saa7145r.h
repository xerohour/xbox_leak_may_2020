// FILE:			library\hardware\pcibridge\specific\saa7145\saa7145r.h
// AUTHOR:		U. Sigmund, S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		19.03.96
//
// PURPOSE:		Register definitions for Philips SAA 7145
//
// HISTORY:

#ifndef SAA7145R_H
#define SAA7145R_H

#include "..\common\prelude.h"

#define SAA7145REG_VDBA			0x00

#define SAA7145REG_VDPCTRL1	0x04
#define SAA7145IDX_VDPA			 2, 22
#define SAA7145IDX_VDPEN		 0

#define SAA7145REG_VDMAC		0x08
#define SAA7145IDX_EVID			31
#define SAA7145IDX_VBS			25, 3
#define SAA7145IDX_VTHR			21, 3
#define SAA7145IDX_VEND			19, 2
#define SAA7145IDX_PFMT			16, 3
#define SAA7145IDX_VFLIP		15
#define SAA7145IDX_VPCH			0, 13

#define SAA7145_PFMT_RGB32		0
#define SAA7145_PFMT_RGB24		1
#define SAA7145_PFMT_RGB16		2
#define SAA7145_PFMT_RGB15		3
#define SAA7145_PFMT_YUV16		4
#define SAA7145_PFMT_Y			5
#define SAA7145_PFMT_RGB16D	6
#define SAA7145_PFMT_RGB15D	7

#define SAA7145_VBS_1			0
#define SAA7145_VBS_2			1
#define SAA7145_VBS_4			2
#define SAA7145_VBS_8			3
#define SAA7145_VBS_16			4
#define SAA7145_VBS_32			5
#define SAA7145_VBS_64			6
#define SAA7145_VBS_ANY			7

#define SAA7145_VTHR_1			0
#define SAA7145_VTHR_16			1
#define SAA7145_VTHR_32			2
#define SAA7145_VTHR_48			3
#define SAA7145_VTHR_64			4
#define SAA7145_VTHR_80			5
#define SAA7145_VTHR_96			6
#define SAA7145_VTHR_112		7

#define SAA7145_VEND_NONE		0
#define SAA7145_VEND_2BYTE		1
#define SAA7145_VEND_4BYTE		2

#define SAA7145REG_VPCTL		0x0C
#define SAA7145IDX_VPCTL		0, 16
#define SAA7145IDX_TEST			15
#define SAA7145IDX_EVEND		11
#define SAA7145IDX_ODDD			10
#define SAA7145IDX_REVS			9
#define SAA7145IDX_REHS			8
#define SAA7145IDX_TWOS			7
#define SAA7145IDX_SF			6
#define SAA7145IDX_FODD			5
#define SAA7145IDX_FD			3, 2
#define SAA7145IDX_SOE			2
#define SAA7145IDX_XSS			1
#define SAA7145IDX_VPZ			0

#define SAA7145_FD_NORMAL		0
#define SAA7145_FD_NOISE		1
#define SAA7145_FD_FORCE		2
#define SAA7145_FD_EXTERNAL	3

#define SAA7145REG_XWC			0x10
#define SAA7145IDX_XWS			16, 10
#define SAA7145IDX_XWZ			0, 10

#define SAA7145REG_YWC			0x14
#define SAA7145IDX_YWS			16, 10
#define SAA7145IDX_YWZ			0, 10

#define SAA7145REG_FSCTL		0x18
#define SAA7145IDX_ILO			31
#define SAA7145IDX_FMODE		28, 3
#define SAA7145IDX_XSCI			16, 11
#define SAA7145IDX_YSCI			0, 11

#define SAA7145REG_YPR			0x1c
#define SAA7145IDX_YPO			16, 10
#define SAA7145IDX_YPE			0, 10

#define SAA7145REG_CDBA			0x20

#define SAA7145REG_CDF			0x24
#define SAA7145IDX_CPO			24, 9
#define SAA7145IDX_CPIX			16, 6
#define SAA7145IDX_CLINE		0, 10

#define SAA7145REG_CDMAC		0x28
#define SAA7145IDX_ECLIP		31
#define SAA7145IDX_CEND			19, 2
#define SAA7145IDX_CPOL			16
#define SAA7145IDX_CPCH			0, 10

#define SAA7145REG_LCR			0x2c
#define SAA7145IDX_SLCT			16, 10
#define SAA7145IDX_TLCT			0, 10

#define SAA7145REG_ADBA			0x30
#define SAA7145REG_ADPA			0x34
#define SAA7145IDX_ADPA			2, 15

#define SAA7145REG_ADMAC		0x38
#define SAA7145IDX_EAUD			31
#define SAA7145IDX_AEND			19
#define SAA7145IDX_AFMT			16, 2
#define SAA7145IDX_ARNG			0, 4

#define SAA7145_AFMT_16RISE	0
#define SAA7145_AFMT_8RISE		1
#define SAA7145_AFMT_16FALL	2
#define SAA7145_AFMT_8FALL		3

// RPS address/enable register
#define SAA7145REG_RPSREG		0x3c
#define SAA7145IDX_RPSPTR		2, 30
#define SAA7145IDX_ERPS			0

// RPS page register
#define SAA7145REG_RPSR			0x40
#define SAA7145IDX_RPSP			11, 21
#define SAA7145IDX_ERPSP		0

// RPS Timout register.
#define SAA7145REG_RPSTO		0x44
#define SAA7145IDX_TOSEL		28, 2
#define SAA7145IDX_TOV			24, 4
#define SAA7145IDX_TOP			0, 24


// Interrupt registers.
#define SAA7145REG_ISR			0x48
#define SAA7145REG_IER			0x4C
#define SAA7145REG_IMR			0x50

// Interrupt bits
#define SAA7145IDX_MASTER		0
#define SAA7145IDX_EC1			1
#define SAA7145IDX_EC2			2
#define SAA7145IDX_GP0			4
#define SAA7145IDX_GP1			5
#define SAA7145IDX_GP2			6
#define SAA7145IDX_GP3			7
#define SAA7145IDX_VFO			8
#define SAA7145IDX_VPE			9
#define SAA7145IDX_CFU			10
#define SAA7145IDX_AFO			11
#define SAA7145IDX_FID			12
#define SAA7145IDX_FIDN			13
#define SAA7145IDX_HBI			14
#define SAA7145IDX_VBI			15
#define SAA7145IDX_VFE			16
#define SAA7145IDX_RPS_PE		17
#define SAA7145IDX_RPS_L		18
#define SAA7145IDX_RPS_TO		19
#define SAA7145IDX_RPS_I		20
#define SAA7145IDX_RPS_S		21
#define SAA7145IDX_UPLD			22
#define SAA7145IDX_ARI			23
#define SAA7145IDX_IIC_D		24
#define SAA7145IDX_IIC_E		25
#define SAA7145IDX_DEBI_TO		26
#define SAA7145IDX_DEBI_I		27
#define SAA7145IDX_DEBI_D		28
#define SAA7145IDX_PPEW			29
#define SAA7145IDX_PABO			30
#define SAA7145IDX_PPER			31

// Interrupt groups for faster IRQ dispatch.
#define SAA7145IDX_IRQPCI		29, 3
#define SAA7145IDX_IRQDEBI		26, 3
#define SAA7145IDX_IRQI2C		24, 2
#define SAA7145IDX_IRQAUDIO	23, 1
#define SAA7145IDX_IRQRPS		17, 6
#define SAA7145IDX_IRQVIDEO	8, 9
#define SAA7145IDX_IRQGPIO		4, 4
#define SAA7145IDX_IRQEC		1, 2


#define SAA7145REG_ECR			0x54
#define SAA7145IDX_ECR			0, 20
#define SAA7145REG_ECT			0x58
#define SAA7145IDX_ECT			0, 20
#define SAA7145REG_ECC			0x5C
#define SAA7145IDX_ECC			0, 20

// General purpose IO port (GPIO)
#define SAA7145REG_GPIOC		0x60

#define SAA7145IDX_GPIOC(x)	(8*x),8
#define SAA7145REG_GPIO(x)		SAA7145REG_GPIOC, SAA7145IDX_GPIOC(x)

#define SAA7145IDX_IOS			0
#define SAA7145IDX_DIR			1
#define SAA7145IDX_MIO			2
#define SAA7145IDX_MSEL			3, 5

#define SAA7145IDX_IOSX(x)		(8*x)
#define SAA7145IDX_DIRX(x)		(8*x+1)
#define SAA7145IDX_MIOX(x)		(8*x+2)
#define SAA7145IDX_MSELX(x)	(8*x+3), 5

#define SAA7145REG_DEBICFG		0x64
#define SAA7145IDX_XIRQPOL		29
#define SAA7145IDX_SS_DS		28
#define SAA7145IDX_XIRQ_WAIT	27
#define SAA7145IDX_TIMEOUT		22, 4
#define SAA7145IDX_SWAP			20, 2
#define SAA7145IDX_SLAVE16		19
#define SAA7145IDX_INCR			18
#define SAA7145IDX_IN_MO		17
#define SAA7145IDX_TIEN			16

#define SAA7145REG_DEBIDAT		0x68
#define SAA7145REG_DEBICMD		0x6c
#define SAA7145IDX_BLOCKLEN	17,15
#define SAA7145IDX_WRITE_N		16
#define SAA7145IDX_A16_IN		0,16


// I²C transfer register
#define SAA7145REG_I2CTFR		0x70
#define SAA7145IDX_I2CB2		24,8
#define SAA7145IDX_I2CB1		16,8
#define SAA7145IDX_I2CB0		8,8
#define SAA7145IDX_I2CATTR2	6,2
#define SAA7145IDX_I2CATTR1	4,2
#define SAA7145IDX_I2CATTR0	2,2

#define SAA7145IDX_I2CBX(x)	(8*x+8),8

// control block.
#define SAA7145IDX_I2CCTL		0, 8

// I²C control/status register
#define SAA7145REG_I2CSC		0x74
#define SAA7145IDX_I2CCC		8, 3
#define SAA7145IDX_I2CSTA		0, 8
// Status bit definitions:
#define SAA7145IDX_I2CBUSY		0			// Also available in I2CTFR register!
#define SAA7145IDX_I2CERR		1			// Also available in I2CTFR register!
#define SAA7145IDX_I2CDTERR	4
#define SAA7145IDX_I2CAPERR	5
#define SAA7145IDX_I2CIRQ		7

#define SAA7145REG_UTIL			0x78
#define SAA7145REG_TURBO		0x7c

#endif


