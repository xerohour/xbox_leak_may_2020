/*************************************************************************

Copyright (C) Microsoft Corporation, 1998 - 1998

Module Name:

	opcodes.h
         
Abstract:

    Header for unsupported assembly instructions.

Author:

	Marc Holder (marchold@microsoft.com) 16-July-1998

Revision History:

*************************************************************************/

// MSVC Now understands conditional move opcodes
//Conditional move opcode macros
#define cmoves  cmoves
#define cmovz   cmovz
#define cmove   cmove
#define cmovne  cmovne
#define cmovnz  cmovnz
#define cmovge  cmovge
#define cmovg   cmovg

//Registers encoding opcode macros
#define edx_edi edx,edi
#define edx_eax edx,eax
#define ebx_eax ebx,eax
#define edi_eax edi,eax
#define esi_eax esi,eax
#define eax_edi eax,edi
#define eax_edx eax,edx



#ifndef _USE_INTEL_COMPILER

//Katmai new instructions opcodes
#define pavgb       _asm _emit 0x0F _asm _emit 0xE0  //packed byte avarage, takes 2 mmx registers  
#define psadbw      _asm _emit 0x0F _asm _emit 0xF6  //packed byte to word sun absolute diffrences, takes 2 mmx registers
#define pshufw      _asm _emit 0x0F _asm _emit 0x70  //pshufw takes 2 mmx registers and 1 byte which must be emited  
#define pmulhuw     _asm _emit 0x0F _asm _emit 0xE4  //pmulhuw packed unsigned multiply keep high words takes 2 mmx registers
#define pinsrw      _asm _emit 0x0F _asm _emit 0xC4
#define pmovmskb    _asm _emit 0x0F _asm _emit 0xD7  
#define pminub      _asm _emit 0x0F _asm _emit 0xDA
#define pmaxub      _asm _emit 0x0F _asm _emit 0xDE
#define pminsw      _asm _emit 0x0F _asm _emit 0xEA

//Katmai SIMD FP instruction opcodes
#define movaps  _asm _emit 0x0F _asm _emit 0x28  //128 bit aligned move
#define movups  _asm _emit 0x0F _asm _emit 0x10  //128 bit unaligned move
#define shufps(reg,funct) _asm emit 0x0F _asm _emit 0xC6 reg _asm _emit funct
#define unpcklps _asm _emit 0x0F _asm _emit 0x14
#define unpckhps _asm _emit 0x0F _asm _emit 0x15
#define mulps _asm _emit 0x0F _asm _emit 0x59
#define cvtps2pi _asm _emit 0x0F _asm _emit 0x2D 
#define xorps _asm _emit 0x0F _asm _emit 0x57
#define subps _asm _emit 0x0F _asm _emit 0x5C
#define addps _asm _emit 0x0F _asm _emit 0x58
#define addss _asm _emit 0xF3 _asm _emit 0x0F _asm _emit 0x58
#define mulss _asm _emit 0xF3 _asm _emit 0x0F _asm _emit 0x59
#define movssFROMMEM _asm _emit 0xF3 _asm _emit 0x0F _asm _emit 0x10
#define movssTOMEM _asm _emit 0xF3 _asm _emit 0x0F _asm _emit 0x11
#define movhlps _asm _emit 0x0F _asm _emit 0x12
#define movlhps _asm _emit 0x0F _asm _emit 0x16

#define xmm3_ebx _asm _emit 0x1B
#define xmm4_ecx _asm _emit 0x21
#define xmm2_edx _asm _emit 0x12 

#define xmm0_esiBASE    _asm _emit 0x06     // xmm0, [esi]
#define xmm1_esiBASE    _asm _emit 0x0E     // xmm1, [esi]
#define xmm2_esiBASE    _asm _emit 0x16     // xmm2, [esi]
#define xmm3_esiBASE    _asm _emit 0x1E     // xmm3, [esi]
#define xmm4_esiBASE    _asm _emit 0x26     // xmm4, [esi]
#define xmm5_esiBASE    _asm _emit 0x2E     // xmm5, [esi]
#define xmm6_esiBASE    _asm _emit 0x36     // xmm6, [esi]
#define xmm7_esiBASE    _asm _emit 0x3E     // xmm6, [esi]

#define xmm0_ediBASE    _asm _emit 0x07     // xmm0, [edi]
#define xmm1_ediBASE    _asm _emit 0x0F     // xmm1, [edi]
#define xmm2_ediBASE    _asm _emit 0x17     // xmm2, [edi]
#define xmm3_ediBASE    _asm _emit 0x1F     // xmm3, [edi]
#define xmm4_ediBASE    _asm _emit 0x27     // xmm4, [edi]
#define xmm5_ediBASE    _asm _emit 0x2F     // xmm5, [edi]
#define xmm6_ediBASE    _asm _emit 0x37     // xmm6, [edi]

#define xmm0_edxBASE    _asm _emit 0x02     // xmm0, [edx]
#define xmm1_edxBASE    _asm _emit 0x0A     // xmm1, [edx]
#define xmm2_edxBASE    _asm _emit 0x12     // xmm2, [edx]
#define xmm3_edxBASE    _asm _emit 0x1A     // xmm3, [edx]
#define xmm4_edxBASE    _asm _emit 0x22     // xmm4, [edx]
#define xmm5_edxBASE    _asm _emit 0x2A     // xmm5, [edx]
#define xmm6_edxBASE    _asm _emit 0x32     // xmm6, [edx]


#define xmm3_espBASE    _asm _emit 0x1c _asm _emit 0x24 // xmm3, [esp]
#define xmm4_espBASE    _asm _emit 0x24 _asm _emit 0x24 // xmm4, [esp]
#define eaxBASE_xmm3    _asm _emit 0x18     // [eax], xmm3


//defines opcodes for mm?,[esi+offset],imm to use with pinsrw
#define mm0_esi(offset,imm) _asm _emit 0x46 _asm _emit offset _asm _emit imm
#define mm1_esi(offset,imm) _asm _emit 0x4E _asm _emit offset _asm _emit imm
#define mm2_esi(offset,imm) _asm _emit 0x56 _asm _emit offset _asm _emit imm

//For mmx,[edi]
#define mm0_edi _asm _emit 0x07
#define mm1_edi _asm _emit 0x0F
#define mm2_edi _asm _emit 0x17
#define mm3_edi _asm _emit 0x1F
#define mm4_edi _asm _emit 0x27
#define mm6_edi _asm _emit 0x37 
#define mm7_edi _asm _emit 0x3F

//For mmx,[esi+edx+offset]
#define mm1_esi_edx_(offset) _asm emit 0x4C _asm _emit 0x16 _asm _emit offset 
#define mm7_esi_edx_(offset) _asm emit 0x7C _asm _emit 0x16 _asm _emit offset 

//For mmx,[esi+offset]
#define mm1_esi_(offset)    _asm _emit 0x4E _asm _emit offset

//For mmx,[edi+offset]
#define mm5_edi_(offset)    _asm _emit 0x6F _asm _emit offset
#define mm6_edi_(offset)    _asm _emit 0x77 _asm _emit offset
#define mm7_edi_(offset)    _asm _emit 0x7F _asm _emit offset

//for cmov
#define eax_ebx             _asm _emit 0xC3
#define ecx_ebx             _asm _emit 0xCB
#define edx_ebx             _asm _emit 0xD3

//for eax,mm4
#define eax_mm4             _asm _emit 0xC4
#define eax_mm7             _asm _emit 0xC7

//MMX redister encoding opcode macros
#define mm0_mm0 _asm _emit 0xC0
#define mm0_mm1 _asm _emit 0xC1
#define mm0_mm2 _asm _emit 0xC2
#define mm0_mm3 _asm _emit 0xC3
#define mm0_mm4 _asm _emit 0xC4
#define mm0_mm5 _asm _emit 0xC5
#define mm0_mm6 _asm _emit 0xC6
#define mm0_mm7 _asm _emit 0xC7
#define mm1_mm1 _asm _emit 0xC9
#define mm1_mm2 _asm _emit 0xCA
#define mm1_mm3 _asm _emit 0xCB
#define mm1_mm4 _asm _emit 0xCC
#define mm1_mm5 _asm _emit 0xCD
#define mm1_mm6 _asm _emit 0xCE
#define mm1_mm7 _asm _emit 0xCF
#define mm2_mm0 _asm _emit 0xD0
#define mm2_mm1 _asm _emit 0xD1
#define mm2_mm2 _asm _emit 0xD2
#define mm2_mm3 _asm _emit 0xD3
#define mm2_mm4 _asm _emit 0xD4
#define mm2_mm5 _asm _emit 0xD5
#define mm2_mm6 _asm _emit 0xD6
#define mm2_mm7 _asm _emit 0xD7
#define mm3_mm0 _asm _emit 0xD8
#define mm3_mm1 _asm _emit 0xD9
#define mm3_mm2 _asm _emit 0xDA
#define mm3_mm3 _asm _emit 0xDB
#define mm3_mm4 _asm _emit 0xDC
#define mm3_mm7 _asm _emit 0xDF
#define mm4_mm0 _asm _emit 0xE0
#define mm4_mm1 _asm _emit 0xE1
#define mm4_mm2 _asm _emit 0xE2
#define mm4_mm4 _asm _emit 0xE4
#define mm4_mm5 _asm _emit 0xE5
#define mm4_mm6 _asm _emit 0xE6
#define mm4_mm7 _asm _emit 0xE7
#define mm5_mm0 _asm _emit 0xE8
#define mm5_mm1 _asm _emit 0xE9
#define mm5_mm2 _asm _emit 0xEA
#define mm5_mm3 _asm _emit 0xEB
#define mm5_mm4 _asm _emit 0xEC
#define mm5_mm5 _asm _emit 0xED
#define mm5_mm6 _asm _emit 0xEE
#define mm5_mm7 _asm _emit 0xEF
#define mm6_mm0 _asm _emit 0xF0
#define mm6_mm1 _asm _emit 0xF1
#define mm6_mm2 _asm _emit 0xF2
#define mm6_mm3 _asm _emit 0xF3
#define mm6_mm4 _asm _emit 0xF4
#define mm6_mm5 _asm _emit 0xF5
#define mm6_mm6 _asm _emit 0xF6
#define mm6_mm7 _asm _emit 0xF7
#define mm7_mm0 _asm _emit 0xF8
#define mm7_mm1 _asm _emit 0xF9
#define mm7_mm2 _asm _emit 0xFA
#define mm7_mm3 _asm _emit 0xFB
#define mm7_mm4 _asm _emit 0xFC
#define mm7_mm5 _asm _emit 0xFD
#define mm7_mm6 _asm _emit 0xFE
#define mm7_mm7 _asm _emit 0xFF

//SIMD_FP registers
#define xmm0_xmm0 _asm _emit 0xC0
#define xmm0_xmm1 _asm _emit 0xC1
#define xmm0_xmm2 _asm _emit 0xC2
#define xmm0_xmm3 _asm _emit 0xC3
#define xmm0_xmm4 _asm _emit 0xC4
#define xmm0_xmm5 _asm _emit 0xC5
#define xmm0_xmm6 _asm _emit 0xC6
#define xmm0_xmm7 _asm _emit 0xC7
#define xmm1_xmm0 _asm _emit 0xC8
#define xmm1_xmm1 _asm _emit 0xC9
#define xmm1_xmm2 _asm _emit 0xCA
#define xmm1_xmm3 _asm _emit 0xCB
#define xmm1_xmm4 _asm _emit 0xCC
#define xmm1_xmm5 _asm _emit 0xCD
#define xmm1_xmm6 _asm _emit 0xCE
#define xmm1_xmm7 _asm _emit 0xCF
#define xmm2_xmm0 _asm _emit 0xD0
#define xmm2_xmm1 _asm _emit 0xD1
#define xmm2_xmm2 _asm _emit 0xD2
#define xmm2_xmm3 _asm _emit 0xD3
#define xmm2_xmm4 _asm _emit 0xD4
#define xmm2_xmm5 _asm _emit 0xD5
#define xmm2_xmm6 _asm _emit 0xD6
#define xmm2_xmm7 _asm _emit 0xD7
#define xmm3_xmm0 _asm _emit 0xD8
#define xmm3_xmm1 _asm _emit 0xD9
#define xmm3_xmm2 _asm _emit 0xDA
#define xmm3_xmm3 _asm _emit 0xDB
#define xmm3_xmm4 _asm _emit 0xDC
#define xmm3_xmm5 _asm _emit 0xDD
#define xmm3_xmm7 _asm _emit 0xDF
#define xmm4_xmm0 _asm _emit 0xE0
#define xmm4_xmm1 _asm _emit 0xE1
#define xmm4_xmm2 _asm _emit 0xE2
#define xmm4_xmm3 _asm _emit 0xE3
#define xmm4_xmm4 _asm _emit 0xE4
#define xmm4_xmm5 _asm _emit 0xE5
#define xmm4_xmm6 _asm _emit 0xE6
#define xmm4_xmm7 _asm _emit 0xE7
#define xmm5_xmm0 _asm _emit 0xE8
#define xmm5_xmm1 _asm _emit 0xE9
#define xmm5_xmm2 _asm _emit 0xEA
#define xmm5_xmm3 _asm _emit 0xEB
#define xmm5_xmm4 _asm _emit 0xEC
#define xmm5_xmm5 _asm _emit 0xED
#define xmm5_xmm6 _asm _emit 0xEE
#define xmm5_xmm7 _asm _emit 0xEF
#define xmm6_xmm0 _asm _emit 0xF0
#define xmm6_xmm1 _asm _emit 0xF1
#define xmm6_xmm2 _asm _emit 0xF2
#define xmm6_xmm3 _asm _emit 0xF3
#define xmm6_xmm4 _asm _emit 0xF4
#define xmm6_xmm5 _asm _emit 0xF5
#define xmm6_xmm6 _asm _emit 0xF6
#define xmm6_xmm7 _asm _emit 0xF7
#define xmm7_xmm0 _asm _emit 0xF8
#define xmm7_xmm1 _asm _emit 0xF9
#define xmm7_xmm2 _asm _emit 0xFA
#define xmm7_xmm3 _asm _emit 0xFB
#define xmm7_xmm4 _asm _emit 0xFC
#define xmm7_xmm5 _asm _emit 0xFD
#define xmm7_xmm6 _asm _emit 0xFE
#define xmm7_xmm7 _asm _emit 0xFF

#define mm0_xmm4 _asm _emit 0xC4
#define mm1_xmm0 _asm _emit 0xC8
#define mm1_xmm2 _asm _emit 0xCA
#define mm2_xmm4 _asm _emit 0xD4
#define mm3_xmm0 _asm _emit 0xD8
#define mm3_xmm2 _asm _emit 0xDA

#define emit _asm _emit

#define CPU_ID _asm _emit 0x0f _asm _emit 0xa2
#define RDTSC  _asm _emit 0x0f _asm _emit 0x31



#else   // _USE_INTEL_COMPILER

//Katmai new instructions opcodes
#define pavgb  pavgb
#define psadbw psadbw
#define pshufw pshufw
#define shufps shufps

//MMX redister encoding opcode macros
#define mm0_mm0 mm0,mm0
#define mm0_mm1 mm0,mm1
#define mm0_mm7 mm0,mm7
#define mm0_mm3 mm0,mm3
#define mm1_mm1 mm1,mm1
#define mm1_mm2 mm1,mm2
#define mm1_mm7 mm1,mm7
#define mm2_mm2 mm2,mm2
#define mm2_mm3 mm2,mm3
#define mm2_mm7 mm2,mm7
#define mm3_mm2 mm3,mm2
#define mm3_mm4 mm3,mm4
#define mm3_mm7 mm3,mm7
#define mm4_mm2 mm4,mm2
#define mm4_mm5 mm4,mm5
#define mm4_mm7 mm4,mm7
#define mm5_mm6 mm5,mm6
#define mm5_mm7 mm5,mm7
#define mm6_mm7 mm6,mm7
#define mm7_mm0 mm7,mm0
#define mm7_mm1 mm7,mm1

//Katmai SIMD FP instruction opcodes
#define movaps movaps
#define movups movups
#define shufps(reg,funct) shufps reg,funct
#define unpcklps unpcklps
#define mulps mulps
#define cvtps2pi cvtps2pi
#define unpckhps unpckhps
#define xorps xorps
#define subps subps
#define addps addps
#define addss addss
#define mulss mulss
#define movssFROMMEM    movss
#define movssTOMEM      movss
#define movhlps         movhlps

#define xmm3_ebx xmm3,[ebx]
#define xmm4_ecx xmm4,[ecx]
#define xmm2_edx xmm2,[edx]

#define xmm0_esiBASE    xmm0, [esi]
#define xmm1_esiBASE    xmm1, [esi]
#define xmm2_esiBASE    xmm2, [esi]
#define xmm3_esiBASE    xmm3, [esi]
#define xmm4_esiBASE    xmm4, [esi]
#define xmm5_esiBASE    xmm5, [esi]
#define xmm6_esiBASE    xmm6, [esi]
#define xmm7_esiBASE    xmm7, [esi]

#define xmm0_ediBASE    xmm0, [edi]
#define xmm1_ediBASE    xmm1, [edi]
#define xmm2_ediBASE    xmm2, [edi]
#define xmm3_ediBASE    xmm3, [edi]
#define xmm4_ediBASE    xmm4, [edi]
#define xmm5_ediBASE    xmm5, [edi]
#define xmm6_ediBASE    xmm6, [edi]

#define xmm0_edxBASE    xmm0, [edx]
#define xmm1_edxBASE    xmm1, [edx]
#define xmm2_edxBASE    xmm2, [edx]
#define xmm3_edxBASE    xmm3, [edx]
#define xmm4_edxBASE    xmm4, [edx]
#define xmm5_edxBASE    xmm5, [edx]
#define xmm6_edxBASE    xmm6, [edx]


#define xmm3_espBASE    xmm3, [esp]
#define xmm4_espBASE    xmm4, [esp]
#define eaxBASE_xmm3    [eax], xmm3

#define xmm0_xmm0 xmm0,xmm0
#define xmm0_xmm1 xmm0,xmm1
#define xmm1_xmm1 xmm1,xmm1
#define xmm0_xmm3 xmm0,xmm3
#define xmm1_xmm2 xmm1,xmm2
#define xmm2_xmm2 xmm2,xmm2
#define xmm1_xmm4 xmm1,xmm4
#define xmm3_xmm0 xmm3,xmm0
#define xmm4_xmm4 xmm4,xmm4

#define xmm1_xmm0 xmm1,xmm0
#define xmm3_xmm5 xmm3,xmm5

#define mm0_xmm4 mm0,xmm4
#define mm1_xmm0 mm1,xmm0
#define mm1_xmm2 mm1,xmm2
#define mm2_xmm4 mm2,xmm4
#define mm3_xmm0 mm3,xmm0
#define mm3_xmm2 mm3,xmm2

#define emit ,

#define CPU_ID cpuid

#endif  // _USE_INTEL_COMPILER
