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
// *	INTEL KATMAI ISSE EXTENSION MACROS
// *
// *	FILE:			KatmaiMacros
// *	CONTEXT:		header file containing preprocessor macros
// *	PURPOSE:		preprocessor macros for generation of ISSE instructions
// *	START:		Friday, 23. July 1999
// *
// *	AUTHOR:		Roland Schaufler
// *	COPYRIGHT:	VIONA Development GmbH
// *
// ************************************************************************************
//
// THIS FILE IS COPYRIGHT OF VIONA DEVELOPMENT GMBH AND IS NOT LICENSED TO THE PUBLIC
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


#ifndef KATMAI_MACROS_H
#define KATMAI_MACROS_H

// streaming SIMD MMX registers
#define Register_xmm0 0
#define Register_xmm1 1
#define Register_xmm2 2
#define Register_xmm3 3
#define Register_xmm4 4
#define Register_xmm5 5
#define Register_xmm6 6
#define Register_xmm7 7

// specials for ModR/M bytes: Mod
#define Mod_none   0
#define Mod_disp8	 1
#define Mod_disp32 2
#define Mod_reg    3

// specials for ModR/M bytes: R/M
#define Register_SIB    4
#define Register_disp32	5

// specials for SIB bytes
#define Register_noindx 4
#define Register_nobase 5

//////////////////////////////////////////////////////////////////////////////////////////////
// Macros only for use within other Macros
//////////////////////////////////////////////////////////////////////////////////////////////

#define START2	_asm _emit 0x0F _asm _emit
#define START3	_asm _emit 0xF3 _asm _emit 0x0F _asm _emit

//////////////////////////////////////////////////////////////////////////////////////////////

#define BLOCK2(opcode, mod, rm, dest)									\
	}																				\
   START2 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm																			\
		{

#define BLOCK2IMM8(opcode, mod, rm, dest, imm8)						\
	}																				\
   START2 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit imm8															\
	_asm																			\
		{

#define BLOCK2IMM32(opcode, mod, rm, dest, imm32)					\
	}																				\
   START2 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit BYTE0(imm32)													\
	_asm _emit BYTE1(imm32)													\
	_asm _emit BYTE2(imm32)													\
	_asm _emit BYTE3(imm32)													\
	_asm																			\
		{

//////////////////////////////////////////////////////////////////////////////////////////////

#define BLOCK2SIB(opcode, mod, rm, dest, scale, index, base)	\
	}																				\
   START2 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit SIBBYTE(scale, index, base)								\
	_asm																			\
		{

#define BLOCK2SIBIMM8(opc, mod, rm, dst, scal, ind, bas, imm8)	\
	}																				\
   START2 opc																	\
	_asm _emit MODRMBYTE(mod, rm, dst)									\
	_asm _emit SIBBYTE(scal, ind, bas)									\
	_asm _emit imm8															\
	_asm																			\
		{

#define BLOCK2SIBIMM32(opc, mod, rm, dest, scale, ind, base)	\
	}																				\
   START2 opc																	\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit SIBBYTE(scale, ind, base)								\
	_asm _emit BYTE0(imm32)													\
	_asm _emit BYTE1(imm32)													\
	_asm _emit BYTE2(imm32)													\
	_asm _emit BYTE3(imm32)													\
	_asm																			\
		{

//////////////////////////////////////////////////////////////////////////////////////////////

#define BLOCK3(opcode, mod, rm, dest)									\
	}																				\
   START3 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm																			\
		{

#define BLOCK3IMM8(opcode, mod, rm, dest, imm8)						\
	}																				\
   START3 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit imm8															\
	_asm																			\
		{

#define BLOCK3IMM32(opcode, mod, rm, dest, imm32)					\
	}																				\
   START3 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit BYTE0(imm32)													\
	_asm _emit BYTE1(imm32)													\
	_asm _emit BYTE2(imm32)													\
	_asm _emit BYTE3(imm32)													\
	_asm																			\
		{

//////////////////////////////////////////////////////////////////////////////////////////////

#define BLOCK3SIB(opcode, mod, rm, dest, scale, index, base)	\
	}																				\
   START3 opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit SIBBYTE(scale, index, base)								\
	_asm																			\
		{

#define BLOCK3SIBIMM8(opc, mod, rm, dst, scal, ind, bas, imm8)	\
	}																				\
   START3 opc																	\
	_asm _emit MODRMBYTE(mod, rm, dst)									\
	_asm _emit SIBBYTE(scal, ind, bas)									\
	_asm _emit imm8															\
	_asm																			\
		{

#define BLOCK3SIBIMM32(opc, mod, rm, dest, scale, ind, base)	\
	}																				\
   START3 opc																	\
	_asm _emit MODRMBYTE(mod, rm, dest)									\
	_asm _emit SIBBYTE(scale, ind, base)								\
	_asm _emit BYTE0(imm32)													\
	_asm _emit BYTE1(imm32)													\
	_asm _emit BYTE2(imm32)													\
	_asm _emit BYTE3(imm32)													\
	_asm																			\
		{


//////////////////////////////////////////////////////////////////////////////////////////////
// KATMAI STREAMING SIMD INSTRUCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// **** 2 BYTE OPCODES
// **** REGISTER to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////
#define addps(dest, src)					BLOCK2(0x58, reg, src, dest)
#define andnps(dest, src)					BLOCK2(0x55, reg, src, dest)
#define andps(dest, src)					BLOCK2(0x54, reg, src, dest)
#define comiss(dest, src)					BLOCK2(0x2F, reg, src, dest)
#define cvtpi2ps(dest, src)				BLOCK2(0x2A, reg, src, dest)
#define cvtps2pi(dest, src)				BLOCK2(0x2D, reg, src, dest)
#define cvttps2pi(dest, src)				BLOCK2(0x2C, reg, src, dest)
#define divps(dest, src)					BLOCK2(0x5E, reg, src, dest)
#define maxps(dest, src)					BLOCK2(0x5F, reg, src, dest)
#define minps(dest, src)					BLOCK2(0x5D, reg, src, dest)
#define movaps(dest, src)					BLOCK2(0x28, reg, src, dest)
#define movmskps(dest, src)				BLOCK2(0x50, reg, src, dest)
#define movups(dest, src)					BLOCK2(0x10, reg, src, dest)
#define mulps(dest, src)					BLOCK2(0x59, reg, src, dest)
#define orps(dest, src)						BLOCK2(0x56, reg, src, dest)
#define rcpps(dest, src)					BLOCK2(0x53, reg, src, dest)
#define rsqrtps(dest, src)					BLOCK2(0x52, reg, src, dest)
#define sqrtps(dest, src)					BLOCK2(0x51, reg, src, dest)
#define subps(dest, src)					BLOCK2(0x5C, reg, src, dest)
#define ucomiss(dest, src)					BLOCK2(0x2E, reg, src, dest)
#define unpckhps(dest, src)				BLOCK2(0x15, reg, src, dest)
#define unpcklps(dest, src)				BLOCK2(0x14, reg, src, dest)
#define xorps(dest, src)					BLOCK2(0x57, reg, src, dest)

//////////////////////////////////////////////////////////////////////////////////////////////
// **** 3 Byte opcodes
// **** REGISTER to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////
#define rcpss(dest, src)					BLOCK3(0x53, reg, src, dest)
#define rsqrtss(dest, src)					BLOCK3(0x52, reg, src, dest)
#define addss(dest, src)					BLOCK3(0x58, reg, src, dest)
#define cvtsi2ss(dest, src)				BLOCK3(0x2A, reg, src, dest)
#define cvtss2si(dest, src)				BLOCK3(0x2D, reg, src, dest)
#define cvttss2si(dest, src)				BLOCK3(0x2C, reg, src, dest)
#define divss(dest, src)					BLOCK3(0x5E, reg, src, dest)
#define maxss(dest, src)					BLOCK3(0x5F, reg, src, dest)
#define minss(dest, src)					BLOCK3(0x5D, reg, src, dest)
#define movss(dest, src)					BLOCK3(0x10, reg, src, dest)
#define mulss(dest, src)					BLOCK3(0x59, reg, src, dest)
#define rcpss(dest, src)					BLOCK3(0x53, reg, src, dest)
#define rsqrtss(dest, src)					BLOCK3(0x52, reg, src, dest)
#define sqrtss(dest, src)					BLOCK3(0x51, reg, src, dest)
#define subss(dest, src)					BLOCK3(0x5C, reg, src, dest)

//////////////////////////////////////////////////////////////////////////////////////////////
// **** 2 BYTE OPCODES
// **** REGISTER to REGISTER plus IMMEDIATE8
//////////////////////////////////////////////////////////////////////////////////////////////
#define cmpps(dest, src, imm8)         BLOCK2IMM8(0xC2, reg, src, dest, imm8)
#define shufps(dest, src, imm8)        BLOCK2IMM8(0xC6, reg, src, dest, imm8)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// **** 2 Byte opcodes
// **** MEMORY	to REGISTER
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER
#define cvtpi2psMEM(dest, src)										BLOCK2(0x2A, none, src, dest)
#define cvtps2piMEM(dest, src)										BLOCK2(0x2D, none, src, dest)
#define movapsMEM(dest, src)											BLOCK2(0x28, none, src, dest)           // src = single INTEGER Register
#define movhpsMEM(dest, src)											BLOCK2(0x16, none, src, dest)           // src = single INTEGER Register
#define movlpsMEM(dest, src)											BLOCK2(0x12, none, src, dest)           // src = single INTEGER Register
#define movupsMEM(dest, src)											BLOCK2(0x10, none, src, dest)
#define xorpsMEM(dest, src)											BLOCK2(0x57, none, src, dest)
// **** MEMORY to REGISTER	+ offset8
#define cvtpi2psIMM8(dest, src, imm8)								BLOCK2IMM8(0x2A, disp8, src, dest, imm8)
#define cvtps2piIMM8(dest, src, imm8)								BLOCK2IMM8(0x2D, disp8, src, dest, imm8)
#define movupsIMM8(dest, src, imm8)									BLOCK2IMM8(0x10, disp8, src, dest, imm8)  // src = single INTEGER Register
#define movhpsIMM8(dest, src, imm8)									BLOCK2IMM8(0x16, disp8, src, dest, imm8)  // src = single INTEGER Register
#define movlpsIMM8(dest, src, imm8)									BLOCK2IMM8(0x12, disp8, src, dest, imm8)  // src = single INTEGER Register
// **** MEMORY to REGISTER	+ offset32
#define cvtpi2psIMM32(dest, src, imm32)							BLOCK2IMM32(0x2A, disp32, src, dest, imm32)
#define cvtps2piIMM32(dest, src, imm32)							BLOCK2IMM32(0x2D, disp32, src, dest, imm32)
#define movhpsIMM32(dest, src, imm32)								BLOCK2IMM32(0x16, disp32, src, dest, imm32) // src = single INTEGER Register
#define movlpsIMM32(dest, src, imm32)								BLOCK2IMM32(0x12, disp32, src, dest, imm32) // src = single INTEGER Register
// **** MEMORY to REGISTER	complex adress (base + index)
#define cvtpi2psSIB(dest, scale, index, base)					BLOCK2SIB(0x2A, none, SIB, dest, scale, index, base)
#define cvtps2piSIB(dest, scale, index, base)					BLOCK2SIB(0x2D, none, SIB, dest, scale, index, base)
#define movhpsSIB(dest, scale, index, base)						BLOCK2SIB(0x16, none, SIB, dest, scale, index, base)
#define movlpsSIB(dest, scale, index, base)						BLOCK2SIB(0x12, none, SIB, dest, scale, index, base)
#define movupsSIB(dest, scale, index, base)						BLOCK2SIB(0x10, none, SIB, dest, scale, index, base)
// **** MEMORY to REGISTER	complex adress (base + index), offset8
#define cvtpi2psSIBIMM8(dest, scale, index, base, imm8)		BLOCK2SIBIMM8(0x2A, disp8, SIB, dest, scale, index, base, imm8)
#define cvtps2piSIBIMM8(dest, scale, index, base, imm8)		BLOCK2SIBIMM8(0x2D, disp8, SIB, dest, scale, index, base, imm8)
#define movhpsSIBIMM8(dest, scale, index, base, imm8)			BLOCK2SIBIMM8(0x16, disp8, SIB, dest, scale, index, base, imm8)
#define movlpsSIBIMM8(dest, scale, index, base, imm8)			BLOCK2SIBIMM8(0x12, disp8, SIB, dest, scale, index, base, imm8)
#define movupsSIBIMM8(dest, scale, index, base, imm8)			BLOCK2SIBIMM8(0x10, disp8, SIB, dest, scale, index, base, imm8)
// **** MEMORY to REGISTER	complex adress (base + index), offset32
#define cvtpi2psSIBIMM32(dest, scale, index, base, imm32)	BLOCK2SIBIMM32(0x2A, disp32, SIB, dest, scale, index, base, imm32)
#define cvtps2piSIBIMM32(dest, scale, index, base, imm32)	BLOCK2SIBIMM32(0x2D, disp32, SIB, dest, scale, index, base, imm32)
#define movhpsSIBIMM32(dest, scale, index, base, imm32)		BLOCK2SIBIMM32(0x16, disp32, SIB, dest, scale, index, base, imm32)
#define movlpsSIBIMM32(dest, scale, index, base, imm32)		BLOCK2SIBIMM32(0x12, disp32, SIB, dest, scale, index, base, imm32)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// **** 2 Byte opcodes
// **** REGISTER to MEMORY
//////////////////////////////////////////////////////////////////////////////////////////////
// **** REGISTER to MEMORY
#define movapsSTR(dest, src)              BLOCK2(0x29, none, src, dest)           // src = single INTEGER Register
#define movhpsSTR(dest, src)              BLOCK2(0x17, none, src, dest)           // src = single INTEGER Register
#define movlpsSTR(dest, src)              BLOCK2(0x13, none, src, dest)           // src = single INTEGER Register
#define movupsSTR(dest, src)              BLOCK2(0x11, none, src, dest)           // src = single INTEGER Register
// **** REGISTER to MEMORY	+ offset8
#define movhpsIMM8STR(dest, src, imm8)		BLOCK2IMM8(0x17, disp8, src, dest, imm8)  // src = single INTEGER Register
#define movlpsIMM8STR(dest, src, imm8)		BLOCK2IMM8(0x13, disp8, src, dest, imm8)  // src = single INTEGER Register
#define movupsIMM8STR(dest, src, imm8)		BLOCK2IMM8(0x11, disp8, src, dest, imm8)  // src = single INTEGER Register
// **** REGISTER to MEMORY	+ offset32
#define movhpsIMM32STR(dest, src, imm32)  BLOCK2IMM32(0x17, disp32, src, dest, imm32) // src = single INTEGER Register
#define movlpsIMM32STR(dest, src, imm32)  BLOCK2IMM32(0x13, disp32, src, dest, imm32) // src = single INTEGER Register
#define movupsIMM32STR(dest, src, imm32)  BLOCK2IMM32(0x11, disp32, src, dest, imm32) // src = single INTEGER Register
// **** REGISTER to MEMORY	complex adress (base + index)
#define movhpsSIBSTR(dest, scale, index, base)  BLOCK2SIB(0x17, none, SIB, dest, scale, index, base)
#define movlpsSIBSTR(dest, scale, index, base)  BLOCK2SIB(0x13, none, SIB, dest, scale, index, base)
#define movupsSIBSTR(dest, scale, index, base)  BLOCK2SIB(0x11, none, SIB, dest, scale, index, base)
// **** REGISTER to MEMORY	complex adress (base + index), offset8
#define movhpsSIBIMM8STR(dest, scale, index, base, imm8) BLOCK2SIBIMM8(0x17, disp8, SIB, dest, scale, index, base, imm8)
#define movlpsSIBIMM8STR(dest, scale, index, base, imm8) BLOCK2SIBIMM8(0x13, disp8, SIB, dest, scale, index, base, imm8)
#define movupsSIBIMM8STR(dest, scale, index, base, imm8) BLOCK2SIBIMM8(0x11, disp8, SIB, dest, scale, index, base, imm8)
// **** REGISTER to MEMORY	complex adress (base + index), offset32
#define movhpsSIBIMM32STR(dest, scale, index, base, imm32) BLOCK2SIBIMM32(0x17, disp32, SIB, dest, scale, index, base, imm32)
#define movlpsSIBIMM32STR(dest, scale, index, base, imm32) BLOCK2SIBIMM32(0x13, disp32, SIB, dest, scale, index, base, imm32)
#define movupsSIBIMM32STR(dest, scale, index, base, imm32) BLOCK2SIBIMM32(0x11, disp32, SIB, dest, scale, index, base, imm32)

//////////////////////////////////////////////////////////////////////////////////////////////
// **** 3 Byte opcodes
// **** REGISTER to MEMORY (Stores)
//////////////////////////////////////////////////////////////////////////////////////////////
#define movssSIBSTR(dest, scale, index, base)   BLOCK3SIB(0x11, none, SIB, dest, scale, index, base)

#endif

