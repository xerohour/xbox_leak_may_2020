//
// FILE:      library\hardware\pcibridge\specific\zivapc\zivarg.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   17.03.98
//
// PURPOSE:   Register definitions for the ZiVA-PC chip
// 
// HISTORY:

#ifndef ZIVAPCRG_H
#define ZIVAPCRG_H                                            

#include "..\..\..\..\common\prelude.h"

//
// PCI Config space registers
//

#define ZIVAREG_CFG_COMMANDREG0	0x4
#define ZIVAIDX_CFG_IOS				0
#define ZIVAIDX_CFG_BUSM			2
#define ZIVAIDX_CFG_PEREN			6


#define ZIVAREG_CFG_COMMANDREG1	0x5
#define ZIVAIDX_CFG_SEREN			0

#define ZIVAREG_CFG_STATUS0		0x6

#define ZIVAREG_CFG_STATUS1		0x7
#define ZIVAIDX_CFG_PPERR			0
#define ZIVAIDX_CFG_DEVS			1, 2
#define ZIVAIDX_CFG_TABT			4
#define ZIVAIDX_CFG_MABT			5
#define ZIVAIDX_CFG_SERR			6
#define ZIVAIDX_CFG_PERR			7

#define ZIVAREG_CFG_REVISION		0x8

#define ZIVAREG_D_CFG_BASEADDR	0x10
#define ZIVAIDX_CFG_ADSS			0
#define ZIVAIDX_CFG_IOB				8, 24

#define ZIVAREG_W_CFG_SUBVENDOR	0x2c
#define ZIVAREG_W_CFG_SUBDEVICE	0x2e

#define ZIVAREG_CFG_PSCONT			0x44
#define ZIVAIDX_CFG_ZVEN			0
#define ZIVAIDX_CFG_DCNT			1
#define ZIVAIDX_CFG_CLKON			2
#define ZIVAIDX_CFG_DVDON			3
#define ZIVAIDX_CFG_PLLON			4
#define ZIVAIDX_CFG_SRESET			5
#define ZIVAIDX_CFG_BOFFZ			6
#define ZIVAIDX_CFG_PWROFF			7

#define ZIVAREG_CFG_MISCCONF		0x48
#define ZIVAIDX_CFG_ZVA3EN			0
#define ZIVAIDX_CFG_ZVAEN			1
#define ZIVAIDX_CFG_AUDCON			2, 2
#define ZIVAIDX_CFG_DMAM			4

#define ZIVAREG_CFG_DELAYTRANS	0x4c
#define ZIVAIDX_CFG_DTCYCLE		0, 4
#define ZIVAIDX_CFG_DTTO			7

#define ZIVAREG_CFG_PMCS0			0x64
#define ZIVAIDX_CFG_PWRS			0, 2

#define ZIVAREG_CFG_PMCS1			0x65
#define ZIVAIDX_CFG_PMEEN			0
#define ZIVAIDX_CFG_DSEL			1, 4
#define ZIVAIDX_CFG_DSCL			5, 2
#define ZIVAIDX_CFG_PMES			7

//
// Host I/O Area registers
//

#define ZIVAREG_IOCONT0				0x0
#define ZIVAIDX_MSTEN0				0
#define ZIVAIDX_MSTEN1				1
#define ZIVAIDX_MSTSEL				2
#define ZIVAIDX_STRNS				3
#define ZIVAIDX_AMUTE				6
#define ZIVAIDX_RSTA					7

#define ZIVAREG_IOCONT1				0x1
#define ZIVAIDX_STCIEN				0
#define ZIVAIDX_AMSTEN0				1
#define ZIVAIDX_AMSTEN1				2
#define ZIVAIDX_AMSTSEL				3

#define ZIVAREG_IOCONT2				0x2

#define ZIVAREG_IOCONT3				0x3
#define ZIVAIDX_I2SCN				0
#define ZIVAIDX_LRCN					1
#define ZIVAIDX_6CHEN				2
#define ZIVAIDX_AUDEN				3
#define ZIVAIDX_DAS					4, 4

#define ZIVAREG_IOINTF0				0x4
#define ZIVAIDX_VMSTI				0, 2
#define ZIVAIDX_VMSTABT				2
#define ZIVAIDX_VDI					3
#define ZIVAIDX_AMSTI				5, 2
#define ZIVAIDX_STCI					7

#define ZIVAIDX_W_VMSTI0			0
#define ZIVAIDX_W_VMSTI1			1
#define ZIVAIDX_W_VMSTI				0, 2
#define ZIVAIDX_W_VMSTABT			2
#define ZIVAIDX_W_VDI				3
#define ZIVAIDX_W_VSI				4
#define ZIVAIDX_W_AMSTI0			5
#define ZIVAIDX_W_AMSTI1			6
#define ZIVAIDX_W_AMSTI				5, 2
#define ZIVAIDX_W_STCI				7

#define ZIVAREG_IOINTF1				0x5
#define ZIVAIDX_VMBAT				0
#define ZIVAIDX_STCRFI				1
#define ZIVAIDX_AMSTABT				2
#define ZIVAIDX_AMBAT				3
#define ZIVAIDX_ATHRI				4
#define ZIVAIDX_AEMPI				5
#define ZIVAIDX_VTHRI				6
#define ZIVAIDX_VEMPI				7

#define ZIVAIDX_W_VMBAT				8
#define ZIVAIDX_W_STCRFI			9
#define ZIVAIDX_W_AMSTABT			0xa
#define ZIVAIDX_W_AMBAT				0xb
#define ZIVAIDX_W_ATHRI				0xc
#define ZIVAIDX_W_AEMPI				0xd
#define ZIVAIDX_W_VTHRI				0xe
#define ZIVAIDX_W_VEMPI				0xf


#define ZIVAREG_IODMAINT0			0x6
#define ZIVAIDX_VDMATHR				0, 3
#define ZIVAIDX_VDMACNT				4, 4

#define ZIVAREG_IODMAINT1			0x7
#define ZIVAIDX_ADMATHR				0, 3
#define ZIVAIDX_ADMACNT				4, 4

#define ZIVAREG_D_IOMADR			0x8

#define ZIVAREG_D_IOMTC				0xc

#define ZIVAREG_IOCPLT				0x10

#define ZIVAREG_IOCPCNT				0x11
#define ZIVAIDX_PSEL					0, 2
#define ZIVAIDX_CPCLR				2

#define ZIVAREG_AVCONT0				0x12
#define ZIVAIDX_FMSK					1
#define ZIVAIDX_FMSKEN				2

#define ZIVAREG_AVCONT1				0x13
#define ZIVAIDX_GPOUT				0, 2
#define ZIVAIDX_GPOUT0				0
#define ZIVAIDX_GPOUT1				1

#define ZIVAREG_IOVMODE				0x14
#define ZIVAIDX_VMOD					0, 4

#define ZIVAREG_IOHSCNT				0x15
#define ZIVAIDX_HSCNT				1, 7

#define ZIVAREG_IOVPCNT				0x16
#define ZIVAIDX_VPCNT				0, 4

#define ZIVAREG_IOPOL				0x17
#define ZIVAIDX_VSPOL				0
#define ZIVAIDX_HSPOL				1


#define ZIVAREG_IOI2C0				0x18

#define ZIVAREG_IOI2C1				0x19
                     
#define ZIVAREG_IOI2C2				0x1a
#define ZIVAIDX_RWZ					0
#define ZIVAIDX_DA					1, 7

#define ZIVAREG_IOI2C3				0x1b
#define ZIVAIDX_ACK					0
#define ZIVAIDX_NOSP					1
#define ZIVAIDX_NODT					2
#define ZIVAIDX_NOSA					3
#define ZIVAIDX_NODA					4
#define ZIVAIDX_NOST					5
#define ZIVAIDX_RES					6
#define ZIVAIDX_BUSY					7

#define ZIVAREG_IOI2CERR			0x1c
#define ZIVAIDX_I2CERR				0

#define ZIVAREG_IOEEPROM			0x20
#define ZIVAIDX_DW					0
#define ZIVAIDX_DR					1
#define ZIVAIDX_CS					2
#define ZIVAIDX_SK					3

#define ZIVAREG_IOPSCNT				0x22
#define ZIVAIDX_ASYN1				0
#define ZIVAIDX_ASYN2				1
#define ZIVAIDX_PSSCLR				2
#define ZIVAIDX_STMOD				3

#define ZIVAREG_RSTCONT				0x27
#define ZIVAIDX_PRST					2

#define ZIVAREG_D_STCCNT			0x28

#define ZIVAREG_STCCONT0			0x2c
#define ZIVAIDX_STCSTR				0
#define ZIVAIDX_STCCLR				1

#define ZIVAREG_STCCONT1			0x2d
#define ZIVAIDX_SINTS				0, 6
#define ZIVAIDX_STCTSTA				7

#define ZIVAREG_D_STCREF			0x3c
                  
#endif
