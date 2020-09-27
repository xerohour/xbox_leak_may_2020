////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// ************************************************************************************
// *
// *	AMD ATHLON 3dNow! INSTRUCTION EXTENSION MACROS
// *
// *	FILE:			ATHLONMacros.h
// *	CONTEXT:		header file containing preprocessor macros for ATHLON (K7)
// *	PURPOSE:		preprocessor macros for generation of ISSE instructions
// *	START:		Friday, 23. July 1999
// *
// *	AUTHOR:		Roland Schaufler
// *	COPYRIGHT:	VIONA Development GmbH
// *
// ************************************************************************************
//
// THIS FILE IS COPYRIGHT OF VIONA DEVELOPMENT GMBH AND IS NOT LICENSED TO THE PUBLIC
// use amd 3d-now! macro file instead.
//
// This file depends on the file "MMXExtensionMacros.h" and will not compile without it.
// All register definitions are missing here...
//
// As it is not possible to overload a macro and to use it with different parameters
// every single addressing mode which is needed has to be "hard-coded". The convention used
// herefore is the following:
//
// instruction			meaning
// Suffix
// -------------------------------------------------------
// --						reg, reg
// MEM					reg, [base]
// IMM8					reg, [base + immediate8]  (this one is not used)
//	IMM32					reg, [base + immediate32] (this one is used rather!)
// SIB					reg, [base + scale*index]
//	STR					[base], reg  (memory store)
//
// explicite store or load instructions contain no extra suffixes
//

#include "MMXExtensionMacros.h"

#ifndef ATHLON_MACROS_H
#define ATHLON_MACROS_H

//////////////////////////////////////////////////////////////////////////////////////////////
// Macros only for use within other Macros
//////////////////////////////////////////////////////////////////////////////////////////////


// AMD 3D-NOW instructions all start with 0x0F 0x0F code!!
#define TDNSTART	_asm _emit 0x0F _asm _emit 0x0F _asm _emit


// define bodies for macros
#define TDNBLOCK(opcode, mod, rm, dest)									\
	}																					\
   TDNSTART																			\
	MODRMBYTE(mod, rm, dest)													\
	_asm _emit opcode																\
	_asm																				\
		{

#define TDNBLOCKIMM8(opcode, mod, rm, dest, imm8)						\
	}																					\
   TDNSTART																			\
	MODRMBYTE(mod, rm, dest)													\
	_asm _emit imm8																\
	_asm _emit opcode																\
	_asm																				\
		{

#define TDNBLOCKIMM32(opcode, mod, rm, dest, imm32)					\
	}																					\
   TDNSTART																			\
	MODRMBYTE(mod, rm, dest)													\
	_asm _emit BYTE0(imm32)														\
	_asm _emit BYTE1(imm32)														\
	_asm _emit BYTE2(imm32)														\
	_asm _emit BYTE3(imm32)														\
	_asm _emit opcode																\
	_asm																				\
		{

//////////////////////////////////////////////////////////////////////////////////////////////

#define TDNBLOCKSIB(opcode, mod, rm, dest, scale, index, base)		\
	}																					\
   TDNSTART																			\
	MODRMBYTE(mod, rm, dest)													\
	_asm _emit SIBBYTE(scale, index, base)									\
	_asm _emit opcode																\
	_asm																				\
		{

#define TDNBLOCKSIBIMM8(opc, mod, rm, dest, scal, ind, bas, imm8)	\
	}																					\
   TDNSTART																			\
	MODRMBYTE(mod, rm, dest)													\
	_asm _emit SIBBYTE(scal, ind, bas)										\
	_asm _emit imm8																\
	_asm _emit opc																	\
	_asm																				\
		{

#define TDNBLOCKSIBIMM32(opc, mod, rm, dest, scale, ind, base)		\
	}																					\
   TDNSTART																			\
	MODRMBYTE(mod, rm, dest)													\
	_asm _emit SIBBYTE(scale, ind, base)									\
	_asm _emit BYTE0(imm32)														\
	_asm _emit BYTE1(imm32)														\
	_asm _emit BYTE2(imm32)														\
	_asm _emit BYTE3(imm32)														\
	_asm _emit opc																	\
	_asm																				\
		{


//////////////////////////////////////////////////////////////////////////////////////////////
// ATHLON NEW INSTRUCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// **** REGISTER to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////

// standard 3D-now
#define pavgusb(dest,src)					TDNBLOCK(0xBF, reg, src, dest)
#define pf2id(dest,src)						TDNBLOCK(0x1D, reg, src, dest)
#define pfacc(dest,src)						TDNBLOCK(0xAE, reg, src, dest)
#define pfadd(dest,src)						TDNBLOCK(0x9E, reg, src, dest)
#define pfcmpeq(dest,src)					TDNBLOCK(0xB0, reg, src, dest)
#define pfcmpge(dest,src)					TDNBLOCK(0x90, reg, src, dest)
#define pfcmpgt(dest,src)					TDNBLOCK(0xA0, reg, src, dest)
#define pfmax(dest,src)						TDNBLOCK(0xA4, reg, src, dest)
#define pfmin(dest,src)						TDNBLOCK(0x94, reg, src, dest)
#define pfmul(dest,src)						TDNBLOCK(0xB4, reg, src, dest)
#define pfrcp(dest,src)						TDNBLOCK(0x96, reg, src, dest)
#define pfrcpit1(dest,src)					TDNBLOCK(0xA6, reg, src, dest)
#define pfrcpit2(dest,src)					TDNBLOCK(0xB6, reg, src, dest)
#define pfrsqit1(dest,src)					TDNBLOCK(0xA7, reg, src, dest)
#define pfrsqrt(dest,src)					TDNBLOCK(0x97, reg, src, dest)
#define pfsub(dest,src)						TDNBLOCK(0x9A, reg, src, dest)
#define pfsubr(dest,src)					TDNBLOCK(0xAA, reg, src, dest)
#define pi2fd(dest,src)						TDNBLOCK(0x0D, reg, src, dest)
#define pmulhrw(dest,src)					TDNBLOCK(0xB7, reg, src, dest)

#define femms									} _asm _emit 0x0F _asm _emit 0x0E _asm {

// NEW ATHLON 3D-now
#define pf2iw(dest,src)						TDNBLOCK(0x1C, reg, src, dest)
#define pfnacc(dest, src)					TDNBLOCK(0x8A, reg, src, dest)
#define pfpnacc(dest, src)					TDNBLOCK(0x8E, reg, src, dest)
#define pi2fw(dest,src)						TDNBLOCK(0x0C, reg, src, dest)
#define pswapd(dest, src)					TDNBLOCK(0xBB, reg, src, dest)

//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////

// standard 3D-now
#define pavgusbMEM(dest,src)					TDNBLOCK(0xBF, none, src, dest)
#define pf2idMEM(dest,src)						TDNBLOCK(0x1D, none, src, dest)
#define pfaccMEM(dest,src)						TDNBLOCK(0xAE, none, src, dest)
#define pfaddMEM(dest,src)						TDNBLOCK(0x9E, none, src, dest)
#define pfcmpeqMEM(dest,src)					TDNBLOCK(0xB0, none, src, dest)
#define pfcmpgeMEM(dest,src)					TDNBLOCK(0x90, none, src, dest)
#define pfcmpgtMEM(dest,src)					TDNBLOCK(0xA0, none, src, dest)
#define pfmaxMEM(dest,src)						TDNBLOCK(0xA4, none, src, dest)
#define pfminMEM(dest,src)						TDNBLOCK(0x94, none, src, dest)
#define pfmulMEM(dest,src)						TDNBLOCK(0xB4, none, src, dest)
#define pfrcpMEM(dest,src)						TDNBLOCK(0x96, none, src, dest)
#define pfrcpit1MEM(dest,src)					TDNBLOCK(0xA6, none, src, dest)
#define pfrcpit2MEM(dest,src)					TDNBLOCK(0xB6, none, src, dest)
#define pfrsqit1MEM(dest,src)					TDNBLOCK(0xA7, none, src, dest)
#define pfrsqrtMEM(dest,src)					TDNBLOCK(0x97, none, src, dest)
#define pfsubMEM(dest,src)						TDNBLOCK(0x9A, none, src, dest)
#define pfsubrMEM(dest,src)					TDNBLOCK(0xAA, none, src, dest)
#define pi2fdMEM(dest,src)						TDNBLOCK(0x0D, none, src, dest)
#define pmulhrwMEM(dest,src)					TDNBLOCK(0xB7, none, src, dest)

// NEW ATHLON 3D-now
#define pf2iwMEM(dest,src)						TDNBLOCK(0x1C, none, src, dest)
#define pfnaccMEM(dest, src)					TDNBLOCK(0x8A, none, src, dest)
#define pfpnaccMEM(dest, src)					TDNBLOCK(0x8E, none, src, dest)
#define pi2fwMEM(dest,src)						TDNBLOCK(0x0C, none, src, dest)
#define pswapdMEM(dest, src)					TDNBLOCK(0x0B, none, src, dest)



//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER plus IMMEDIATE8
//////////////////////////////////////////////////////////////////////////////////////////////

// standard 3D-now
#define pavgusbIMM08(dest,src, imm8)	TDNBLOCKIMM8(0xBF, disp8, src, dest, imm8)
#define pf2idIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x1D, disp8, src, dest, imm8)
#define pfaccIMM08(dest,src, imm8)		TDNBLOCKIMM8(0xAE, disp8, src, dest, imm8)
#define pfaddIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x9E, disp8, src, dest, imm8)
#define pfcmpeqIMM08(dest,src, imm8)	TDNBLOCKIMM8(0xB0, disp8, src, dest, imm8)
#define pfcmpgeIMM08(dest,src, imm8)	TDNBLOCKIMM8(0x90, disp8, src, dest, imm8)
#define pfcmpgtIMM08(dest,src, imm8)	TDNBLOCKIMM8(0xA0, disp8, src, dest, imm8)
#define pfmaxIMM08(dest,src, imm8)		TDNBLOCKIMM8(0xA4, disp8, src, dest, imm8)
#define pfminIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x94, disp8, src, dest, imm8)
#define pfmulIMM08(dest,src, imm8)		TDNBLOCKIMM8(0xB4, disp8, src, dest, imm8)
#define pfrcpIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x96, disp8, src, dest, imm8)
#define pfrcpit1IMM08(dest,src, imm8)	TDNBLOCKIMM8(0xA6, disp8, src, dest, imm8)
#define pfrcpit2IMM08(dest,src, imm8)	TDNBLOCKIMM8(0xB6, disp8, src, dest, imm8)
#define pfrsqit1IMM08(dest,src, imm8)	TDNBLOCKIMM8(0xA7, disp8, src, dest, imm8)
#define pfrsqrtIMM08(dest,src, imm8)	TDNBLOCKIMM8(0x97, disp8, src, dest, imm8)
#define pfsubIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x9A, disp8, src, dest, imm8)
#define pfsubrIMM08(dest,src, imm8)		TDNBLOCKIMM8(0xAA, disp8, src, dest, imm8)
#define pi2fdIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x0D, disp8, src, dest, imm8)
#define pmulhrwIMM08(dest,src, imm8)	TDNBLOCKIMM8(0xB7, disp8, src, dest, imm8)

// NEW ATHLON 3D-now
#define pf2iwIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x1C, disp8, src, dest, imm8)
#define pfnaccIMM08(dest, src, imm8)	TDNBLOCKIMM8(0x8A, disp8, src, dest, imm8)
#define pfpnaccIMM08(dest, src, imm8)	TDNBLOCKIMM8(0x8E, disp8, src, dest, imm8)
#define pi2fwIMM08(dest,src, imm8)		TDNBLOCKIMM8(0x0C, disp8, src, dest, imm8)
#define pswapdIMM08(dest, src, imm8)	TDNBLOCKIMM8(0x0B, disp8, src, dest, imm8)


//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER plus IMMEDIATE32
//////////////////////////////////////////////////////////////////////////////////////////////

// standard 3D-now
#define pavgusbIMM32(dest,src, imm32)		TDNBLOCKIMM32(0xBF, disp32, src, dest, imm32)
#define pf2idIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x1D, disp32, src, dest, imm32)
#define pfaccIMM32(dest,src, imm32)			TDNBLOCKIMM32(0xAE, disp32, src, dest, imm32)
#define pfaddIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x9E, disp32, src, dest, imm32)
#define pfcmpeqIMM32(dest,src, imm32)		TDNBLOCKIMM32(0xB0, disp32, src, dest, imm32)
#define pfcmpgeIMM32(dest,src, imm32)		TDNBLOCKIMM32(0x90, disp32, src, dest, imm32)
#define pfcmpgtIMM32(dest,src, imm32)		TDNBLOCKIMM32(0xA0, disp32, src, dest, imm32)
#define pfmaxIMM32(dest,src, imm32)			TDNBLOCKIMM32(0xA4, disp32, src, dest, imm32)
#define pfminIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x94, disp32, src, dest, imm32)
#define pfmulIMM32(dest,src, imm32)			TDNBLOCKIMM32(0xB4, disp32, src, dest, imm32)
#define pfrcpIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x96, disp32, src, dest, imm32)
#define pfrcpit1IMM32(dest,src, imm32)		TDNBLOCKIMM32(0xA6, disp32, src, dest, imm32)
#define pfrcpit2IMM32(dest,src, imm32)		TDNBLOCKIMM32(0xB6, disp32, src, dest, imm32)
#define pfrsqit1IMM32(dest,src, imm32)		TDNBLOCKIMM32(0xA7, disp32, src, dest, imm32)
#define pfrsqrtIMM32(dest,src, imm32)		TDNBLOCKIMM32(0x97, disp32, src, dest, imm32)
#define pfsubIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x9A, disp32, src, dest, imm32)
#define pfsubrIMM32(dest,src, imm32)		TDNBLOCKIMM32(0xAA, disp32, src, dest, imm32)
#define pi2fdIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x0D, disp32, src, dest, imm32)
#define pmulhrwIMM32(dest,src, imm32)		TDNBLOCKIMM32(0xB7, disp32, src, dest, imm32)

// NEW ATHLON 3D-now
#define pf2iwIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x1C, disp32, src, dest, imm32)
#define pfnaccIMM32(dest, src, imm32)		TDNBLOCKIMM32(0x8A, disp32, src, dest, imm32)
#define pfpnaccIMM32(dest, src, imm32)		TDNBLOCKIMM32(0x8E, disp32, src, dest, imm32)
#define pi2fwIMM32(dest,src, imm32)			TDNBLOCKIMM32(0x0C, disp32, src, dest, imm32)
#define pswapdIMM32(dest, src, imm32)		TDNBLOCKIMM32(0x0B, disp32, src, dest, imm32)


//////////////////////////////////////////////////////////////////////////////////////////////
// **** COMPLEX INDEX VERSIONS ****
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY+SCALED*MEMORY to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////
#define pfmulSIB(dest, scale, index, base)		TDNBLOCKSIB(0xB4, none, SIB, dest, scale, index, base)


#endif



