//
// FILE:      library\hardware\pcibridge\specific\i22\i22regs.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   10.04.95
//
// PURPOSE:   Additional register definitions for the I22 chip. The I22 has
//            some bits and registers more than the I20.
// 
// HISTORY:

#ifndef I22REGS_H
#define I22REGS_H

#include "..\..\..\..\common\prelude.h"
#include "..\i20\i20regs.h"



// Interrupt status register

#define I22IDX_JCODREPIRQ		27
#define I22REG_JCODREPIRQ		I20REG_INTSTATUS, I22IDX_JCODREPIRQ



// Interrupt control register

#define I22IDX_JCODREPIRQEN	27
#define I22REG_JCODREPIRQEN	I20REG_INTENABLE, I22IDX_JCODREPIRQEN



// JPEG mode and control

#define I22REG_JMODE				0x100
#define I22IDX_JPG				31
#define I22REG_JPG				I22REG_JMODE, I22IDX_JPG
#define I22IDX_JPGMODE			29,  2
#define I22REG_JPGMODE			I22REG_JMODE, I22IDX_JPGMODE
#define I22IDX_RTBSY_FB			6
#define I22REG_RTBSY_FB			I22REG_JMODE, I22IDX_RTBSY_FEEDBACK
#define I22IDX_GO_ENABLE		5
#define I22REG_GO_ENABLE		I22REG_JMODE, I22IDX_GO_ENABLE
#define I22IDX_SYNCMSTR			4
#define I22REG_SYNCMSTR			I22REG_JMODE, I22IDX_SYNCMSTR
#define I22IDX_FLD_PER_BUFF	3
#define I22REG_FLD_PER_BUFF	I22REG_JMODE, I22IDX_FLD_PER_BUFF
#define I22IDX_VFIFO_FB			2
#define I22REG_VFIFO_FB			I22REG_JMODE, I22IDX_VFIFO_FB
#define I22IDX_CFIFO_FB			1
#define I22REG_CFIFO_FB			I22REG_JMODE, I22IDX_CFIFO_FB
#define I22IDX_STILL_LENDIAN	0
#define I22REG_STILL_LENDIAN	I22REG_JMODE, I22IDX_STILL_LENDIAN

// JPG modes
#define I22_MOTION_COMPRESS	0x03
#define I22_MOTION_DECOMPRESS	0x02
#define I22_STILL_COMPRESS		0x01
#define I22_STILL_DECOMPRESS	0x00



// JPEG process control

#define I22REG_JPROCESS			0x104
#define I22IDX_P_RESET			7
#define I22REG_P_RESET			I22REG_JPROCESS, I22IDX_P_RESET
#define I22IDX_COD_TRNS_EN		5
#define I22REG_COD_TRNS_EN		I22REG_JPROCESS, I22IDX_COD_TRNS_EN
#define I22IDX_AVFE_RESET		3
#define I22REG_AVFE_RESET		I22REG_JPROCESS, I22IDX_AVFE_RESET
#define I22IDX_ACTIVE			0
#define I22REG_ACTIVE			I22REG_JPROCESS, I22IDX_ACTIVE



// Vertical sync parameters

#define I22REG_VERTSYNC			0x108
#define I22IDX_VSYNCSIZE		16,  8
#define I22REG_VSYNCSIZE		I22REG_VERTSYNC, I22IDX_VSYNCSIZE
#define I22IDX_FRMTOT			 0, 16
#define I22REG_FRMTOT			I22REG_VERTSYNC, I22IDX_FRMTOT



// Horizontal sync parameters

#define I22REG_HORZSYNC			0x10C
#define I22IDX_HSYNCSTART		16, 16
#define I22REG_HSYNCSTART		I22REG_HORZSYNC, I22IDX_HSYNCSTART
#define I22IDX_LINETOT			 0, 16
#define I22REG_LINETOT			I22REG_HORZSYNC, I22IDX_LINETOT



// Field horizontal active portion

#define I22REG_HORZACTIVE		0x110
#define I22IDX_NAX				16, 16
#define I22REG_NAX				I22REG_HORZACTIVE, I22IDX_NAX
#define I22IDX_PAX				 0, 16
#define I22REG_PAX				I22REG_HORZACTIVE, I22IDX_PAX



// Field vertical active portion

#define I22REG_VERTACTIVE		0x114
#define I22IDX_NAY				16, 16
#define I22REG_NAY				I22REG_VERTACTIVE, I22IDX_NAY
#define I22IDX_PAY				 0, 16
#define I22REG_PAY				I22REG_VERTACTIVE, I22IDX_PAY



// Field process parameters

#define I22REG_FIELDPARAS		0x118
#define I22IDX_VERDEC			1
#define I22REG_VERDEC			I22REG_FIELDPARAS, I22IDX_VERDEC
#define I22IDX_ODD_EVEN			0
#define I22REG_ODD_EVEN			I22REG_FIELDPARAS, I22IDX_ODD_EVEN



// JPEG code base address

#define I22REG_I_STAT_COM		0x11C



// JPEG CFIFO threshold

#define I22REG_THRESHOLD		0x120
#define I22IDX_COD_THRESHOLD	 0,  8
#define I22REG_COD_THRESHOLD	I22REG_THRESHOLD, I22IDX_COD_THRESHOLD



// JPEG coder guest ID

#define I22REG_CODERGUEST		0x124
#define I22IDX_GUESTID			 4,  3
#define I22REG_GUESTID			I22REG_CODERGUEST, I22IDX_GUESTID
#define I22IDX_GUESTREG			 0,  3
#define I22REG_GUESTREG			I22REG_CODERGUEST, I22IDX_GUESTREG



// JPEG code debug register

#define I22REG_CODEDEBUG		0x128
#define I22IDX_SERV_STATE		12,  4
#define I22REG_SERV_STATE		I22REG_CODEDEBUG, I22IDX_SERV_STATE
#define I22IDX_CODBUFFCNT		10,  2
#define I22REG_CODBUFFCNT		I22REG_CODEDEBUG, I22IDX_CODBUFFCNT
#define I22IDX_CODFRAGCNT		 0,  8
#define I22REG_CODFRAGCNT		I22REG_CODEDEBUG, I22IDX_CODFRAGCNT



// GuestBus control register 2. We have the same duration and recovery timing
// as GuestBus control register 1. In contrast to GuestBus control register 1,
// 4..7 is used as index here.

#define I22REG_GUESTCTRL_2		0x12C
#define I22IDX_GUESTDUR(x)		(2+4*(x-4)), 2
#define I22REG_GUESTDUR(x)		I22REG_GUESTCTRL_2, I22IDX_GUESTDUR(x)
#define I22IDX_GUESTREC(x)		(0+4*(x-4)), 2
#define I22REG_GUESTREC(x)		I22REG_GUESTCTRL_2, I22IDX_GUESTREC(x)



// Still transfer register assuming little endian format configuration

#define I22REG_STILL_TRANSFER	0x300
#define I22IDX_STILL_BSY		31
#define I22REG_STILL_BSY		I22REG_STILL_TRANSFER, I22IDX_STILL_BSY
#define I22IDX_STILL_PIXEL		 0, 24
#define I22REG_STILL_PIXEL		I22REG_STILL_TRANSFER, I22IDX_STILL_PIXEL



#endif
