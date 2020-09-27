// FILE:			library\hardware\audio\specific\ad1846rg.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		09.06.97
//
// PURPOSE:		Register definitions for AD1846 audio codec
//
// HISTORY:

#ifndef AD1846RG_H
#define AD1846RG_H

///////////////////////////////////////////////////////////////////////////////
// Definitions for AD 1846
///////////////////////////////////////////////////////////////////////////////

//
// Direct registers
//

// Index Address Register (R0)
#define AD1846REG_IDXADR	0

#define AD1846IDX_INIT		7
#define AD1846IDX_MCE		6
#define AD1846IDX_TRD		5
#define AD1846IDX_IXA		0, 5

// Indexed Data Register (R1)
#define AD1846REG_IDXDDATA	1

#define AD1846IDX_CUL		7
#define AD1846IDX_CLR		6
#define AD1846IDX_CRDY		5
#define AD1846IDX_SOUR		4
#define AD1846IDX_PUL		3
#define AD1846IDX_PLR		2
#define AD1846IDX_PRDY		1
#define AD1846IDX_INT		0

// Status Register (R2)
#define AD1846REG_STATUS	2

// Capture/Playback IO Data Register (R3)
#define AD1846REG_IODAT		3

//
// Indirect registers
// 

// Left input control register (I0)
#define AD1846REG_LIC		0

#define AD1846IDX_LSS		6, 2
#define AD1846IDX_LMGE		5
#define AD1846IDX_LIG		0, 4

// Right input control register (I1)
#define AD1846REG_RIC		1

#define AD1846IDX_RSS		6, 2
#define AD1846IDX_RMGE		5
#define AD1846IDX_RIG		0, 4

// Left auxiliary #1 input control (I2)
#define AD1846REG_LAUX1		2

#define AD1846IDX_LMX1		7
#define AD1846IDX_LX1A		0, 5

// Right auxiliary #1 input control (I3)
#define AD1846REG_RAUX1		3

#define AD1846IDX_RMX1		7
#define AD1846IDX_RX1A		0, 5

// Left auxiliary #2 input control (I4)
#define AD1846REG_LAUX2		4

#define AD1846IDX_LMX2		7
#define AD1846IDX_LX2A		0, 5

// Right auxiliary #2 input control (I5)
#define AD1846REG_RAUX2		5

#define AD1846IDX_RMX2		7
#define AD1846IDX_RX2A		0, 5

// Left DAC control (I6)
#define AD1846REG_LDAC		6

#define AD1846IDX_LDM		7
#define AD1846IDX_LDA		0, 6

// Right DAC control (I7)
#define AD1846REG_RDAC		7

#define AD1846IDX_RDM		7
#define AD1846IDX_RDA		0, 6

// Clock and data format register (I8)
#define AD1846REG_CLOCKFMT	8

#define AD1846IDX_FMT		6, 2		// MSB should be always set to 0!
#define AD1846IDX_CL			5
#define AD1846IDX_SM			4
#define AD1846IDX_CFS		1, 3
#define AD1846IDX_CSS		0

// Interface configuration register (I9)
#define AD1846REG_IFCFG		9

#define AD1846IDX_CPIO		7
#define AD1846IDX_PPIO		6
#define AD1846IDX_ACAL		3
#define AD1846IDX_SDC		2
#define AD1846IDX_CEN		1
#define AD1846IDX_PEN		0

// Pin control register (I10)
#define AD1846REG_PINCTRL	10

#define AD1846IDX_XCTL		6,2
#define AD1846IDX_IEN		1

// Test and initialization register (I11)
#define AD1846REG_TESTINI	11

#define AD1846IDX_COR		7
#define AD1846IDX_PUR		6
#define AD1846IDX_ACI		5
#define AD1846IDX_DRS		4
#define AD1846IDX_ORR		2, 2
#define AD1846IDX_ORL		0, 2

// Miscellaneous control register (I12)
#define AD1846REG_MISC		12

#define AD1846IDX_ID			0, 4

// Digital mix control register (I13)
#define AD1846REG_DMIXCTRL	13

#define AD1846IDX_DMA		2, 6
#define AD1846IDX_DME		0

// Upper Base count register (I14)
#define AD1846REG_UBCOUNT	14

// Lower Base count register (I15)
#define AD1846REG_LBCOUNT	15

#endif
