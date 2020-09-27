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
#ifndef _USE_INTEL_COMPILER

//Conditional move opcode macros
#define cmoves  _asm _emit 0x0F  _asm _emit 0x48
#define cmovz   _asm _emit 0x0F  _asm _emit 0x44
#define cmove   _asm _emit 0x0F  _asm _emit 0x44
#define cmovne  _asm _emit 0x0F  _asm _emit 0x45
#define cmovnz  _asm _emit 0x0F  _asm _emit 0x45
#define cmovge  _asm _emit 0x0F  _asm _emit 0x4D
#define cmovg   _asm _emit 0x0F  _asm _emit 0x4F

//Registers encoding opcode macros
#define edx_edi _asm _emit 0xD7
#define edx_eax _asm _emit 0xD0
#define ebx_eax _asm _emit 0xD8
#define edi_eax _asm _emit 0xF8
#define esi_eax _asm _emit 0xF0
#define eax_edi _asm _emit 0xC7
#define eax_edx _asm _emit 0xC2



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
#define pextrw      _asm _emit 0x0F _asm _emit 0xC5
#define pshufw      _asm _emit 0x0F _asm _emit 0x70
#define prefetchnta _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x44
#define prefetcht0  _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x4C
#define prefetcht1  _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x54
#define prefetcht2  _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x5C

//Katmai SIMD FP instruction opcodes
#define movaps  _asm _emit 0x0F _asm _emit 0x28  //128 bit aligned move

//defines opcodes for mm?,[esi+offset],imm to use with pinsrw
#define mm0_esi(offset,imm) _asm _emit 0x46 _asm _emit offset _asm _emit imm
#define mm1_esi(offset,imm) _asm _emit 0x4E _asm _emit offset _asm _emit imm
#define mm2_esi(offset,imm) _asm _emit 0x56 _asm _emit offset _asm _emit imm

//For mmx,eax,offset to use with pinsrw
#define mm0_eax_(offset) _asm _emit 0xC0 _asm _emit offset
#define mm1_eax_(offset) _asm _emit 0xC8 _asm _emit offset
#define mm2_eax_(offset) _asm _emit 0xD0 _asm _emit offset
#define mm3_eax_(offset) _asm _emit 0xD8 _asm _emit offset
#define mm4_eax_(offset) _asm _emit 0xE0 _asm _emit offset
#define mm5_eax_(offset) _asm _emit 0xE8 _asm _emit offset
#define mm6_eax_(offset) _asm _emit 0xF0 _asm _emit offset
#define mm7_eax_(offset) _asm _emit 0xF8 _asm _emit offset

//For mmx,ebx,offset to use with pinsrw
#define mm0_ebx_(offset) _asm _emit 0xC3 _asm _emit offset
#define mm1_ebx_(offset) _asm _emit 0xCB _asm _emit offset
#define mm2_ebx_(offset) _asm _emit 0xD3 _asm _emit offset
#define mm3_ebx_(offset) _asm _emit 0xDB _asm _emit offset
#define mm4_ebx_(offset) _asm _emit 0xE3 _asm _emit offset
#define mm5_ebx_(offset) _asm _emit 0xEB _asm _emit offset
#define mm6_ebx_(offset) _asm _emit 0xF3 _asm _emit offset
#define mm7_ebx_(offset) _asm _emit 0xFB _asm _emit offset

//For mmx,ecx,offset to use with pinsrw
#define mm0_ecx_(offset) _asm _emit 0xC1 _asm _emit offset
#define mm1_ecx_(offset) _asm _emit 0xC9 _asm _emit offset
#define mm2_ecx_(offset) _asm _emit 0xD1 _asm _emit offset
#define mm3_ecx_(offset) _asm _emit 0xD9 _asm _emit offset
#define mm4_ecx_(offset) _asm _emit 0xE1 _asm _emit offset
#define mm5_ecx_(offset) _asm _emit 0xE9 _asm _emit offset
#define mm6_ecx_(offset) _asm _emit 0xF1 _asm _emit offset
#define mm7_ecx_(offset) _asm _emit 0xF9 _asm _emit offset

//For eax,mmx,offset for use with pextrw
#define eax_mm0_(offset) _asm _emit 0xC0 _asm _emit offset
#define eax_mm1_(offset) _asm _emit 0xC1 _asm _emit offset
#define eax_mm2_(offset) _asm _emit 0xC2 _asm _emit offset
#define eax_mm3_(offset) _asm _emit 0xC3 _asm _emit offset
#define eax_mm4_(offset) _asm _emit 0xC4 _asm _emit offset
#define eax_mm5_(offset) _asm _emit 0xC5 _asm _emit offset
#define eax_mm6_(offset) _asm _emit 0xC6 _asm _emit offset
#define eax_mm7_(offset) _asm _emit 0xC7 _asm _emit offset

//For ebx,mmx,offset for use with pextrw
#define ebx_mm0_(offset) _asm _emit 0xD8 _asm _emit offset
#define ebx_mm1_(offset) _asm _emit 0xD9 _asm _emit offset
#define ebx_mm2_(offset) _asm _emit 0xDA _asm _emit offset
#define ebx_mm3_(offset) _asm _emit 0xDB _asm _emit offset
#define ebx_mm4_(offset) _asm _emit 0xDC _asm _emit offset
#define ebx_mm5_(offset) _asm _emit 0xDD _asm _emit offset
#define ebx_mm6_(offset) _asm _emit 0xDE _asm _emit offset
#define ebx_mm7_(offset) _asm _emit 0xDF _asm _emit offset

//For ecx,mmx,offset for use with pextrw
#define ecx_mm0_(offset) _asm _emit 0xC8 _asm _emit offset
#define ecx_mm1_(offset) _asm _emit 0xC9 _asm _emit offset
#define ecx_mm2_(offset) _asm _emit 0xCA _asm _emit offset
#define ecx_mm3_(offset) _asm _emit 0xCB _asm _emit offset
#define ecx_mm4_(offset) _asm _emit 0xCC _asm _emit offset
#define ecx_mm5_(offset) _asm _emit 0xCD _asm _emit offset
#define ecx_mm6_(offset) _asm _emit 0xCE _asm _emit offset
#define ecx_mm7_(offset) _asm _emit 0xCF _asm _emit offset

//For edx,mmx,offset for use with pextrw
#define edx_mm0_(offset) _asm _emit 0xD0 _asm _emit offset
#define edx_mm1_(offset) _asm _emit 0xD1 _asm _emit offset
#define edx_mm2_(offset) _asm _emit 0xD2 _asm _emit offset
#define edx_mm3_(offset) _asm _emit 0xD3 _asm _emit offset
#define edx_mm4_(offset) _asm _emit 0xD4 _asm _emit offset
#define edx_mm5_(offset) _asm _emit 0xD5 _asm _emit offset
#define edx_mm6_(offset) _asm _emit 0xD6 _asm _emit offset
#define edx_mm7_(offset) _asm _emit 0xD7 _asm _emit offset

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

// for prefetch
#define esi_eax_(scale,offset)  _asm _emit 0x6|scale<<6 _asm _emit offset

//MMX redister encoding opcode macros
#define mm0_mm0 _asm _emit 0xC0
#define mm0_mm1 _asm _emit 0xC1
#define mm0_mm2 _asm _emit 0xC2
#define mm0_mm3 _asm _emit 0xC3
#define mm0_mm4 _asm _emit 0xC4
#define mm0_mm5 _asm _emit 0xC5
#define mm0_mm6 _asm _emit 0xC6
#define mm0_mm7 _asm _emit 0xC7
#define mm1_mm0 _asm _emit 0xC8
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
#define mm3_mm5 _asm _emit 0xDD
#define mm3_mm7 _asm _emit 0xDF
#define mm4_mm0 _asm _emit 0xE0
#define mm4_mm1 _asm _emit 0xE1
#define mm4_mm2 _asm _emit 0xE2
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

#define emit _asm _emit

#define CPU_ID _asm _emit 0x0f _asm _emit 0xa2
#define RDTSC  _asm _emit 0x0f _asm _emit 0x31

#else

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

//Katmai new instructions opcodes
#define pavgb  pavgb
#define psadbw psadbw
#define pshufw pshufw

//MMX redister encoding opcode macros
#define mm0_mm1 mm0,mm1
#define mm0_mm7 mm0,mm7
#define mm0_mm3 mm0,mm3
#define mm1_mm2 mm1,mm2
#define mm1_mm7 mm1,mm7
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


#define emit ,

#endif
