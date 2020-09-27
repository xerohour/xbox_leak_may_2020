
// FILE:      library\hardware\wavelet\specific\601regs.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   08.05.96
//
// PURPOSE:   Register definitions for the ADV601 wavelet chip.
// 
// HISTORY:

#ifndef ADV601REGS_H
#define ADV601REGS_H



#define ADV601_NBLOCKS	42





// Indirect register access

#define ADV601REG_INDIRECT		0x00
#define ADV601REG_REGDATA		0x04
#define ADV601REG_COMPDATA		0x08



// Interrupt mask and status register

#define ADV601REG_IRQ			0x0C
#define ADV601IDX_IE_MEMERROR	14
#define ADV601IDX_IE_FIFOSTP	13
#define ADV601IDX_IE_FIFOERR	12
#define ADV601IDX_IE_FIFOSRQ	11
#define ADV601IDX_IE_LCODE		10
#define ADV601IDX_IE_STATR		9
#define ADV601IDX_IE_CCIRER	8
#define ADV601IDX_MEMERROR		6
#define ADV601IDX_FIFOSTP		5
#define ADV601IDX_FIFOERR		4
#define ADV601IDX_FIFOSRQ		3
#define ADV601IDX_LCODE			2
#define ADV601IDX_STATR			1
#define ADV601IDX_CCIRER		0





// Now the indirect 16 bit registers:



// Mode control

#define ADV601REG_MODECONTROL	0x00
#define ADV601IDX_PHIRQ			13
#define ADV601IDX_PHSYNC		12
#define ADV601IDX_SWR			11
#define ADV601IDX_DSP			10
#define ADV601IDX_BUC			9
#define ADV601IDX_SPE			8
#define ADV601IDX_ED				7
#define ADV601IDX_PN				6
#define ADV601IDX_MS				5
#define ADV601IDX_VCLK2			4
#define ADV601IDX_VIF			0, 4

// video interface format (VIF) values
#define ADV601_CCIR656		0x0
#define ADV601_MLTPX			0x2
#define ADV601_PHILIPS		0x3
#define ADV601_GRAYSCALE	0x8



// FIFO control

#define ADV601REG_FIFOCONTROL	0x01
#define ADV601IDX_NES			4, 4
#define ADV601IDX_NFS			0, 4



// Video area registers

#define ADV601REG_HSTART		0x02
#define ADV601REG_HEND			0x03
#define ADV601REG_VSTART		0x04
#define ADV601REG_VEND			0x05



// Compression ratio

#define ADV601REG_COMPRATIO	0x06
#define ADV601IDX_COMPRATIO	0, 8



// 42 Reciprocal bin width and 42 bin width registers.

#define ADV601REG_RBW(i)		(0x100+2*(i))
#define ADV601REG_BW(i)			(0x101+2*(i))





// Now the indirect 32 bit registers:

#define ADV601REG_SUM_SQUARES(i)		(0x80+(i))   // 42 sum of squares registers
#define ADV601REG_SUM_LUMA		0xAA
#define ADV601REG_SUM_CB		0xAB
#define ADV601REG_SUM_CR		0xAC
#define ADV601REG_MIN_LUMA		0xAD
#define ADV601REG_MAX_LUMA		0xAE
#define ADV601REG_MIN_CB		0xAF
#define ADV601REG_MAX_CB		0xB0
#define ADV601REG_MIN_CR		0xB1
#define ADV601REG_MAX_CR		0xB2



#endif
