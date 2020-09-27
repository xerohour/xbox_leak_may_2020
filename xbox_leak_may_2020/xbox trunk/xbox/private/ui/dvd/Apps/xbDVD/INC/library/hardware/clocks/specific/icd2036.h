// FILE:			library\hardware\clocks\specific\icd2036.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		04.04.97
//
// PURPOSE:		Definitions and structures for PLLs of ICD2036 type
//
// HISTORY:

#ifndef ICD2036_H
#define ICD2036_H

struct FrequencyParameters
	{
	DWORD	mClkP,
			mClkQ,
			mClkPre,
			mClkPost,
			vClkP,
			vClkQ,
			vClkPre,
			vClkPost;
	};
	

//
// Registers for ICD2036 PLL
//
#define ICD2036REG_FVCLKF1	0
#define ICD2036REG_FVCLKF2	1
#define ICD2036REG_VREG0	2
#define ICD2036REG_MREG0	3
#define ICD2036REG_PWRDWN	4
#define ICD2036REG_VREG1	5
#define ICD2036REG_CNTL		6
#define ICD2036REG_MREG1	7

//
// Bits for MREGx/VREGx
//
#define ICD2036IDX_Q			0, 7
#define ICD2036IDX_M			7, 3
#define ICD2036IDX_P			10, 7
#define ICD2036IDX_I			17, 4

//
// Bits for control register CNTL
//
#define ICD2036IDX_PM0		11
#define ICD2036IDX_PM1		12
#define ICD2036IDX_PV0		13
#define ICD2036IDX_PV1		14
#define ICD2036IDX_C0		15
#define ICD2036IDX_C1		16
#define ICD2036IDX_C2		17
#define ICD2036IDX_C3		18
#define ICD2036IDX_C4		19
#define ICD2036IDX_C5		20

//
// Bits for powerdown register PWRDWN
//
#define ICD2036IDX_P0		17
#define ICD2036IDX_P1		18
#define ICD2036IDX_P2		19
#define ICD2036IDX_P3		20

#endif
