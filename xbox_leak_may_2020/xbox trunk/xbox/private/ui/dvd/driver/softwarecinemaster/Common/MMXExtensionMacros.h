////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// ************************************************************************************
// *
// *	MMX INSTRUCTION EXTENSION MACROS
// *
// *	FILE:			MMXExtensionMacros.h
// *	CONTEXT:		header file containing preprocessor macros for ATHLON (K7)
// *	PURPOSE:		preprocessor macros for generation of ISSE instructions
// *	START:		Wednesday, Aug. 25th, 1999
// *
// *	AUTHOR:		Roland Schaufler
// *	COPYRIGHT:	VIONA Development GmbH
// *
// ************************************************************************************
//
// THIS FILE IS COPYRIGHT OF VIONA DEVELOPMENT GMBH AND IS NOT LICENSED TO THE PUBLIC
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
//
//
// explicite store or load instructions contain no extra suffixes
//

#ifndef MMX_EXTENSION_MACROS_H
#define MMX_EXTENSION_MACROS_H

//////////////////////////////////////////////////////////////////////////////////////////////
// Definitions of CPU registers INTEGER and MMX
//////////////////////////////////////////////////////////////////////////////////////////////

// integer registers
#define Register_eax 0
#define Register_ecx 1
#define Register_edx 2
#define Register_ebx 3
#define Register_esp 4
#define Register_ebp 5
#define Register_esi 6
#define Register_edi 7

// integer MMX registers
#define Register_mm0 0
#define Register_mm1 1
#define Register_mm2 2
#define Register_mm3 3
#define Register_mm4 4
#define Register_mm5 5
#define Register_mm6 6
#define Register_mm7 7

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

// immediate values following opcode bytes
#define BYTE0(off)					(off & 0xff)
#define BYTE1(off)					((off & 0xff00) >> 8)
#define BYTE2(off)					((off & 0xff0000) >> 16)
#define BYTE3(off)					((off & 0xff000000) >> 24)

#define MODRMBYTE(mod, rm, dest)     ((Mod_##mod << 6) + (Register_##dest << 3)  + Register_##rm)

#define SIBBYTE(scale, index, base)  ((scale << 6)	    + (Register_##index << 3) + Register_##base)

// define register byte
#define REG(dest, src)	(0xC0 + (Register_##dest << 3) + Register_##src)

// Additional ATHLON MMX ops
#define MMXSTART	   _asm _emit 0x0F  _asm _emit

// define bodies for macros
#define MMXBLOCK(opcode, mod, rm, dest)									\
	}																					\
   MMXSTART opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm																				\
		{

#define MMXBLOCKIMM8(opcode, mod, rm, dest, imm8)						\
	}																					\
   MMXSTART opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm _emit imm8																\
	_asm																				\
		{

#define MMXBLOCKIMM32(opcode, mod, rm, dest, imm32)					\
	}																					\
   MMXSTART opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm _emit BYTE0(imm32)														\
	_asm _emit BYTE1(imm32)														\
	_asm _emit BYTE2(imm32)														\
	_asm _emit BYTE3(imm32)														\
	_asm																				\
		{

#define MMXBLOCKIMM32IMM8(opcode, mod, rm, dest, imm32, imm8)		\
	}																					\
   MMXSTART opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm _emit BYTE0(imm32)														\
	_asm _emit BYTE1(imm32)														\
	_asm _emit BYTE2(imm32)														\
	_asm _emit BYTE3(imm32)														\
	_asm _emit imm8																\
	_asm																				\
		{

//////////////////////////////////////////////////////////////////////////////////////////////

#define MMXBLOCKSIB(opcode, mod, rm, dest, scale, index, base)		\
	}																					\
   MMXSTART opcode																\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm _emit SIBBYTE(scale, index, base)									\
	_asm																				\
		{

#define MMXBLOCKSIBIMM8(opc, mod, rm, dest, scal, ind, bas, imm8)	\
	}																					\
   MMXSTART opc																	\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm _emit SIBBYTE(scal, ind, bas)										\
	_asm _emit imm8																\
	_asm																				\
		{

#define MMXBLOCKSIBIMM32(opc, mod, rm, dest, scale, ind, base)		\
	}																					\
   MMXSTART opc																	\
	_asm _emit MODRMBYTE(mod, rm, dest)										\
	_asm _emit SIBBYTE(scale, ind, base)									\
	_asm _emit BYTE0(imm32)														\
	_asm _emit BYTE1(imm32)														\
	_asm _emit BYTE2(imm32)														\
	_asm _emit BYTE3(imm32)														\
	_asm																				\
		{


//////////////////////////////////////////////////////////////////////////////////////////////
// **** REGISTER to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////

#define maskmovq(dest, src)				MMXBLOCK(0xF7, reg, src, dest)
#define pavgb(dest, src)					MMXBLOCK(0xE0, reg, src, dest)
#define pavgw(dest, src)					MMXBLOCK(0xE3, reg, src, dest)
#define pextrw(dest, src)					MMXBLOCK(0xC5, reg, src, dest)
#define pinsrw(dest, src)					MMXBLOCK(0xC4, reg, src, dest)
#define pmaxsw(dest, src)					MMXBLOCK(0xEE, reg, src, dest)
#define pmaxub(dest, src)					MMXBLOCK(0xDE, reg, src, dest)
#define pminsw(dest, src)					MMXBLOCK(0xEA, reg, src, dest)
#define pminub(dest, src)					MMXBLOCK(0xDA, reg, src, dest)
#define pmovmskb(dest, src)				MMXBLOCK(0xD7, reg, src, dest)
#define pmulhw(dest, src)					MMXBLOCK(0xE4, reg, src, dest)
#define psadbw(dest, src)					MMXBLOCK(0xF6, reg, src, dest)


//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////

#define maskmovqMEM(dest, src)				MMXBLOCK(0xF7, none, src, dest)
#define pavgbMEM(dest, src)					MMXBLOCK(0xE0, none, src, dest)
#define pavgwMEM(dest, src)					MMXBLOCK(0xE3, none, src, dest)
#define pextrwMEM(dest, src)					MMXBLOCK(0xC5, none, src, dest)
#define pinsrwMEM(dest, src)					MMXBLOCK(0xC4, none, src, dest)
#define pmaxswMEM(dest, src)					MMXBLOCK(0xEE, none, src, dest)
#define pmaxubMEM(dest, src)					MMXBLOCK(0xDE, none, src, dest)
#define pminswMEM(dest, src)					MMXBLOCK(0xEA, none, src, dest)
#define pminubMEM(dest, src)					MMXBLOCK(0xDA, none, src, dest)
#define pmovmskbMEM(dest, src)				MMXBLOCK(0xD7, none, src, dest)
#define pmulhwMEM(dest, src)					MMXBLOCK(0xE4, none, src, dest)
#define psaddbwMEM(dest, src)					MMXBLOCK(0xF6, none, src, dest)


//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER plus IMMEDIATE8
//////////////////////////////////////////////////////////////////////////////////////////////

#define maskmovqIMM08(dest, src)			MMXBLOCKIMM8(0xF7, disp8, src, dest, imm8)
#define pavgbIMM08(dest, src, imm8)		MMXBLOCKIMM8(0xE0, disp8, src, dest, imm8)
#define pavgwIMM08(dest, src, imm8)		MMXBLOCKIMM8(0xE3, disp8, src, dest, imm8)
#define pextrwIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xC5, disp8, src, dest, imm8)
#define pinsrwIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xC4, disp8, src, dest, imm8)
#define pmaxswIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xEE, disp8, src, dest, imm8)
#define pmaxubIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xDE, disp8, src, dest, imm8)
#define pminswIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xEA, disp8, src, dest, imm8)
#define pminubIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xDA, disp8, src, dest, imm8)
#define pmovmskbIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xD7, disp8, src, dest, imm8)
#define pmulhwIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xE4, disp8, src, dest, imm8)
#define psaddbwIMM08(dest, src, imm8)	MMXBLOCKIMM8(0xF6, disp8, src, dest, imm8)


//////////////////////////////////////////////////////////////////////////////////////////////
// **** MEMORY to REGISTER plus IMMEDIATE32
//////////////////////////////////////////////////////////////////////////////////////////////
// NEW ATHLON MMX
#define maskmovqIMM32(dest, src, imm32)	MMXBLOCKIMM32(0xF7, disp32, src, dest, imm23)
#define pavgbIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xE0, disp32, src, dest, imm32)
#define pavgwIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xE3, disp32, src, dest, imm32)
#define pextrwIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xC5, disp32, src, dest, imm32)
#define pinsrwIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xC4, disp32, src, dest, imm32)
#define pmaxswIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xEE, disp32, src, dest, imm32)
#define pmaxubIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xDE, disp32, src, dest, imm32)
#define pminswIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xEA, disp32, src, dest, imm32)
#define pminubIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xDA, disp32, src, dest, imm32)
#define pmovmskbIMM32(dest, src, imm32)	MMXBLOCKIMM32(0xD7, disp32, src, dest, imm32)
#define pmulhwIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xE4, disp32, src, dest, imm32)
#define psaddbwIMM32(dest, src, imm32)		MMXBLOCKIMM32(0xF6, disp32, src, dest, imm32)

//////////////////////////////////////////////////////////////////////////////////////////////
// **** REGISTER to MEMORY
//////////////////////////////////////////////////////////////////////////////////////////////

#define movntq(src, dest)							MMXBLOCK(0xE7, none, dest, src)
#define movntqIMM8(src, dest, imm8)				MMXBLOCKIMM8(0xE7, disp8, dest, src, imm8)
#define movntqIMM32(src, dest, imm32)			MMXBLOCKIMM32(0xE7, disp32, dest, src, imm32)
#define movntqSIB(src, scale, index, base)	MMXBLOCKSIB(0xE7, none, SIB, src, scale, index, base)



//////////////////////////////////////////////////////////////////////////////////////////////
// special instruciotns
//////////////////////////////////////////////////////////////////////////////////////////////

// pshufw
#define pshufw(dest, src, imm8)				MMXBLOCKIMM8(0x70, reg, src, dest, imm8)

#define pshufwMEM(dest, src, imm8)								\
				}															\
				MMXSTART 0x70											\
				_asm _emit MODRMBYTE(none, src, dest)			\
				_asm _emit imm8										\
				_asm														\
				{

#define pshufwIMM8(dest, src, mem8, imm8)						\
				}															\
				MMXSTART 0x70											\
				_asm _emit MODRMBYTE(disp8, src, dest)			\
				_asm _emit mem8										\
				_asm _emit imm8										\
				_asm														\
				{


#define pshufwIMM32(dest, src, imm32, imm8)					\
				}															\
				MMXSTART 0x70											\
				_asm _emit MODRMBYTE(disp32, src, dest)		\
				_asm _emit BYTE0(imm32)								\
				_asm _emit BYTE1(imm32)								\
				_asm _emit BYTE2(imm32)								\
				_asm _emit BYTE3(imm32)								\
				_asm _emit imm8										\
				_asm														\
				{



// prefetch

// MOD/RM Bytes
#define MOD_OPC_RM(mod, opc, rm)		((mod << 6) + (opc << 3) + rm)
#define MOD_OPC_REG(mod, opc, reg)	((mod << 6) + (opc << 3) + Register_##reg)

// SIB (Scale/index/base) Bytes
#define SIB(scale, index, base)		((scale << 6) + (Register_##index << 3) + Register_##base)
#define SIN(scale, index, base)		((scale << 6) + (Register_##index << 3) + base)

// **** prefetch [reg]
// mode = opcode extension bit 5..8: prefetch-op,  = { 1, 2, 3, 0 }
// reg  = base register
#define prefetch_REG(mode, reg)											\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_REG(0x00, mode, reg)							\
	_asm																			\
		{

// **** prefetch [reg + reg]
// mode = opcode extension bit 5..8: prefetch-op
// reg1 = unscaled register
// reg2 = scaled register
// scale = reg*1=0, reg*2=1, reg*4=2, reg*8=3
#define prefetch_REGpsREG(mode, reg1, scale, reg2)					\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_RM(0x00, mode, 0x04)							\
   _asm _emit SIB(scale, reg2, reg1)									\
	_asm																			\
		{

// **** prefetch [reg + off8]
// mode = opcode extension bit 5..8: prefetch-op  = { 1, 2, 3, 0 }
// reg  = base register
// off8 = 8-bit offset, is sign extended and added for address	generation
#define prefetch_REGpOFF8(mode, reg, off8)							\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_REG(0x01, mode, reg)							\
	_asm _emit off8															\
	_asm																			\
		{

// **** prefetch [reg1 + s*reg2 + off8]
// mode = opcode extension bit 5..8: prefetch-op  = { 1, 2, 3, 0 }
// reg1 = unscaled register
// reg2 = scaled register
// off8 = 8-bit offset, is sign extended and added for address	generation
// scale = reg*1=0, reg*2=1, reg*4=2, reg*8=3
#define prefetch_REGpsREGpOFF8(mode, reg1, scale, reg2, off8)	\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_RM(0x01, mode, 0x04)							\
	_asm _emit SIB(scale, reg2, reg1)									\
	_asm _emit off8															\
	_asm																			\
		{

// **** prefetch [reg + off32]
// mode  = opcode extension bit 5..8: prefetch-op  = { 1, 2, 3, 0 }
// reg   = base register
// off32 = 32-bit offset
#define prefetch_REGpOFF32(mode, reg, off32)							\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_REG(0x02, mode, reg)							\
	_asm _emit BYTE0(off32)													\
	_asm _emit BYTE1(off32)													\
	_asm _emit BYTE2(off32)													\
	_asm _emit BYTE3(off32)													\
	_asm																			\
		{

// **** prefetch [s*reg + off32]
// mode  = opcode extension bit 5..8: prefetch-op  = { 1, 2, 3, 0 }
// reg   = base register
// off32 = 32-bit offset
// scale = reg*1=0, reg*2=1, reg*4=2, reg*8=3
#define prefetch_sREGpOFF32(mode, scale, reg1, off32)				\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_RM(0x00, mode, 0x04)							\
	_asm _emit SIN(scale, reg1, 0x05)									\
	_asm _emit BYTE0(off32)													\
	_asm _emit BYTE1(off32)													\
	_asm _emit BYTE2(off32)													\
	_asm _emit BYTE3(off32)													\
	_asm																			\
		{

// **** prefetch [reg1 + s*reg2 + off32]
// mode = opcode extension bit 5..8: prefetch-op  = { 1, 2, 3, 0 }
// reg1 = unscaled register
// reg2 = scaled register
// off32 = 32-bit offset
// scale = reg*1=0, reg*2=1, reg*4=2, reg*8=3
#define prefetch_REGpsREGpOFF32(mode, reg1, scale, reg2, off32)\
	}																				\
	_asm _emit 0x0F															\
	_asm _emit 0x18															\
	_asm _emit MOD_OPC_RM(0x02, mode, 0x04)							\
	_asm _emit SIB(scale, reg2, reg1)									\
	_asm _emit BYTE0(off32)													\
	_asm _emit BYTE1(off32)													\
	_asm _emit BYTE2(off32)													\
	_asm _emit BYTE3(off32)													\
	_asm																			\
		{



#endif
