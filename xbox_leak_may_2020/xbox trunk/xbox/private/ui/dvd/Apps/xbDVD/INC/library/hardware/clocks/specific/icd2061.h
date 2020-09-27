// FILE:			library\hardware\clocks\specific\icd2061.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		04.04.97
//
// PURPOSE:		Definitions and structures for PLLs of ICD2061 type
//
// HISTORY:

#ifndef ICD2061_H
#define ICD2061_H

//
// Registers for ICD2061 PLL
//
#define ICD2061REG_REG0			0
#define ICD2061REG_REG1			1
#define ICD2061REG_REG2			2
#define ICD2061REG_MREG			3
#define ICD2061REG_PWRDWN		4
#define ICD2061REG_RESERVED	5
#define ICD2061REG_CNTL			6

//
// Bits for MREGx/VREGx
//
#define ICD2061IDX_Q			0, 7
#define ICD2061IDX_M			7, 3
#define ICD2061IDX_P			10, 7
#define ICD2061IDX_I			17, 4

//
// Bits for control register CNTL
//
#define ICD2061IDX_PS0		12
#define ICD2061IDX_PS1		13
#define ICD2061IDX_PS2		14
#define ICD2061IDX_C0		15
#define ICD2061IDX_C1		16
#define ICD2061IDX_C2		17
#define ICD2061IDX_C3		18
#define ICD2061IDX_C4		19
#define ICD2061IDX_C5		20


#endif
