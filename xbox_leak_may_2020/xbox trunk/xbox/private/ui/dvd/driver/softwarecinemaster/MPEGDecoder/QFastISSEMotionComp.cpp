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

#include "QFastISSEMotionComp.h"

#pragma warning (disable : 4799)


static __forceinline void BiMotionCopyUV0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm1, mm2
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm1, mm2
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm1, mm2
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyUV1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompUV1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::BiMotionCompensationUV(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				BiMotionCopyUV0000(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				BiMotionCopyUV0010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				BiMotionCopyUV1010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				BiMotionCopyUV0001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				BiMotionCopyUV1001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				BiMotionCopyUV0011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				BiMotionCopyUV1011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 12:
				BiMotionCopyUV0101(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				BiMotionCopyUV0111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 15:
				BiMotionCopyUV1111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				BiMotionCompUV0000(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				BiMotionCompUV0010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				BiMotionCompUV1010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				BiMotionCompUV0001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				BiMotionCompUV1001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				BiMotionCompUV0011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				BiMotionCompUV1011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 12:
				BiMotionCompUV0101(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				BiMotionCompUV0111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 15:
				BiMotionCompUV1111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void SBiMotionCopyUV0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm1, mm2
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm1, mm2
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm1, mm2
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyUV1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompUV1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x02
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x02
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm0, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x0a
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x0a
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm1, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm3, mm7
	pxor mm7, mm7
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x0a
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x0a
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	movq	mm7, [ebx + 8]
	punpcklwd	mm7, [ebx + 16 + 8]
	paddw	mm6, mm7
	movq	mm7, [ebx + 8]
	punpckhwd	mm7, [ebx + 16 + 8]
	paddw	mm2, mm7
	pxor mm7, mm7
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x02
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x02
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	movq	mm7, [ebx + 0]
	punpcklwd	mm7, [ebx + 16 + 0]
	paddw	mm4, mm7
	movq	mm7, [ebx + 0]
	punpckhwd	mm7, [ebx + 16 + 0]
	paddw	mm5, mm7
	pxor mm7, mm7
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::SBiMotionCompensationUV(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				SBiMotionCopyUV0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				SBiMotionCopyUV0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SBiMotionCopyUV1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				SBiMotionCopyUV0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				SBiMotionCopyUV1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				SBiMotionCopyUV0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				SBiMotionCopyUV1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 12:
				SBiMotionCopyUV0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				SBiMotionCopyUV0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 15:
				SBiMotionCopyUV1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				SBiMotionCompUV0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				SBiMotionCompUV0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SBiMotionCompUV1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				SBiMotionCompUV0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				SBiMotionCompUV1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				SBiMotionCompUV0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				SBiMotionCompUV1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 12:
				SBiMotionCompUV0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				SBiMotionCompUV0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 15:
				SBiMotionCompUV1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void BiMotionCopyY0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCopyY1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void BiMotionCompY1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[ebx], mm4
	movq	[ebx + 8], mm6
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::BiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				BiMotionCopyY0000(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				BiMotionCopyY0010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				BiMotionCopyY1010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				BiMotionCopyY0001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				BiMotionCopyY1001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				BiMotionCopyY0011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				BiMotionCopyY1011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 12:
				BiMotionCopyY0101(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				BiMotionCopyY0111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 15:
				BiMotionCopyY1111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				BiMotionCompY0000(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				BiMotionCompY0010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				BiMotionCompY1010(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				BiMotionCompY0001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				BiMotionCompY1001(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				BiMotionCompY0011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				BiMotionCompY1011(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 12:
				BiMotionCompY0101(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				BiMotionCompY0111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			case 15:
				BiMotionCompY1111(prevp, succp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void SBiMotionCopyY0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCopyY1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SBiMotionCompY1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [destp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	[eax], mm0
	movq	[eax + 8], mm1
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	[eax], mm4
	movq	[eax + 8], mm6
	add	eax, esi
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::SBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				SBiMotionCopyY0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				SBiMotionCopyY0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SBiMotionCopyY1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				SBiMotionCopyY0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				SBiMotionCopyY1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				SBiMotionCopyY0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				SBiMotionCopyY1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 12:
				SBiMotionCopyY0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				SBiMotionCopyY0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 15:
				SBiMotionCopyY1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				SBiMotionCompY0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				SBiMotionCompY0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SBiMotionCompY1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				SBiMotionCompY0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				SBiMotionCompY1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				SBiMotionCompY0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				SBiMotionCompY1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 12:
				SBiMotionCompY0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				SBiMotionCompY0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 15:
				SBiMotionCompY1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void DBiMotionCopyY0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCopyY1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DBiMotionCompY1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::DBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				DBiMotionCopyY0000(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				DBiMotionCopyY0010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				DBiMotionCopyY1010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				DBiMotionCopyY0001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				DBiMotionCopyY1001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				DBiMotionCopyY0011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				DBiMotionCopyY1011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 12:
				DBiMotionCopyY0101(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				DBiMotionCopyY0111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 15:
				DBiMotionCopyY1111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				DBiMotionCompY0000(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				DBiMotionCompY0010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				DBiMotionCompY1010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				DBiMotionCompY0001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				DBiMotionCompY1001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				DBiMotionCompY0011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				DBiMotionCompY1011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 12:
				DBiMotionCompY0101(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				DBiMotionCompY0111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 15:
				DBiMotionCompY1111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	}

static __forceinline void SDBiMotionCopyY0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCopyY1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDBiMotionCompY1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	ebx, [deltap]
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::SDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				SDBiMotionCopyY0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				SDBiMotionCopyY0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				SDBiMotionCopyY1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				SDBiMotionCopyY0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				SDBiMotionCopyY1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				SDBiMotionCopyY0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				SDBiMotionCopyY1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 12:
				SDBiMotionCopyY0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				SDBiMotionCopyY0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 15:
				SDBiMotionCopyY1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				SDBiMotionCompY0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				SDBiMotionCompY0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				SDBiMotionCompY1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				SDBiMotionCompY0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				SDBiMotionCompY1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				SDBiMotionCompY0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				SDBiMotionCompY1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 12:
				SDBiMotionCompY0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				SDBiMotionCompY0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 15:
				SDBiMotionCompY1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	}

static __forceinline void IDBiMotionCopyY0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY0000(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY0010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY1010(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY0001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY1001(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY0011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY1011(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY0101(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY0111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCopyY1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDBiMotionCompY1111(BYTE * prevp, BYTE * succp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	mov	edx, [succp]
	mov	esi, [sbpr]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
loop1:
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [dbpr]
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm4, [ecx + 0]
//// pavgb	mm4, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x61
	_emit	0x01
////-------------
	movq	mm5, [edx + 0]
//// pavgb	mm5, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6a
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + 8]
//// pavgb	mm6, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x71
	_emit	0x09
////-------------
	movq	mm2, [edx + 8]
//// pavgb	mm2, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [dbpr]
	add	edi, [ifbdist]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	pxor	mm7, mm7
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	ecx, esi
	add	edx, esi
	add	ebx, [dbpr]
	add	eax, [dbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::IDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				IDBiMotionCopyY0000(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				IDBiMotionCopyY0010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				IDBiMotionCopyY1010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				IDBiMotionCopyY0001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				IDBiMotionCopyY1001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				IDBiMotionCopyY0011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				IDBiMotionCopyY1011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 12:
				IDBiMotionCopyY0101(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				IDBiMotionCopyY0111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 15:
				IDBiMotionCopyY1111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				IDBiMotionCompY0000(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				IDBiMotionCompY0010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				IDBiMotionCompY1010(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				IDBiMotionCompY0001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				IDBiMotionCompY1001(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				IDBiMotionCompY0011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				IDBiMotionCompY1011(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 12:
				IDBiMotionCompY0101(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				IDBiMotionCompY0111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 15:
				IDBiMotionCompY1111(prevp, succp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	}

static __forceinline void ISDBiMotionCopyY0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY0000(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY0010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY1010(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	pxor	mm7, mm7
loop1:
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xca
////-------------
	movq	mm2, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm2, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm1, mm2
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm5, [eax]
	movq	mm3, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm3, mm5
	movq [edi], mm0
	movq [edi + 8], mm3
	movq	mm6, [eax + 8]
	movq	mm3, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm3, mm6
	movq [edi + 16], mm1
	movq [edi + 24], mm3
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY0001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY1001(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY0011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY1011(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY0101(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
	movq	mm2, [edx + 0]
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY0111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
	movq	mm0, [ecx + 0]
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCopyY1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDBiMotionCompY1111(BYTE * prevp, BYTE * succp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	ebx, [deltap]
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	esi, 0
	mov	eax, [uvdp]
	mov	edi, [fbp]
	mov	ecx, [prevp]
	movq	mm1, [ecx + 8]
//// pavgb	mm1, [ecx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x49
	_emit	0x09
////-------------
	movq	mm0, [ecx + 0]
//// pavgb	mm0, [ecx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x41
	_emit	0x01
////-------------
	add	ecx, [sbpr]
	mov	[prevp], ecx
	mov	edx, [succp]
	movq	mm3, [edx + 8]
//// pavgb	mm3, [edx + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5a
	_emit	0x09
////-------------
	movq	mm2, [edx + 0]
//// pavgb	mm2, [edx + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x52
	_emit	0x01
////-------------
	add	edx, [sbpr]
	mov	[succp], edx
loop1:
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	sub	eax, [sbpr]
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm4, [ecx + esi + 0]
//// pavgb	mm4, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x64
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm5, [edx + esi + 0]
//// pavgb	mm5, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x6c
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm0, mm4
	_emit	0x0f
	_emit	0xe0
	_emit	0xc4
////-------------
//// pavgb	mm2, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xd5
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm2, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm2, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm2, [ebx + 8 + 0]
	packuswb	mm0, mm2
	movq	mm6, [ecx + esi + 8]
//// pavgb	mm6, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x74
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm2, [edx + esi + 8]
//// pavgb	mm2, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm1, mm6
	_emit	0x0f
	_emit	0xe0
	_emit	0xce
////-------------
//// pavgb	mm3, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xda
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	movq	mm3, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm3, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm3, [ebx + 8 + 16]
	packuswb	mm1, mm3
	mov	edx, [destp]
	movq	[edx + esi], mm0
	movq	[edx + esi + 8], mm1
	movq	mm3, [eax]
	movq	mm7, mm0
	punpcklbw	mm0, mm3
	punpckhbw	mm7, mm3
	movq [edi], mm0
	movq [edi + 8], mm7
	movq	mm3, [eax + 8]
	movq	mm7, mm1
	punpcklbw	mm1, mm3
	punpckhbw	mm7, mm3
	movq [edi + 16], mm1
	movq [edi + 24], mm7
	add	eax, [sbpr]
	add	edi, [ifbdist]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	pxor	mm7, mm7
	mov	ecx, [prevp]
	mov	edx, [succp]
	movq	mm1, [ecx + esi + 8]
//// pavgb	mm1, [ecx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x4c
	_emit	0x31
	_emit	0x09
////-------------
	movq	mm3, [edx + esi + 8]
//// pavgb	mm3, [edx + esi + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x5c
	_emit	0x32
	_emit	0x09
////-------------
//// pavgb	mm6, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xf1
////-------------
//// pavgb	mm2, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xd3
////-------------
//// pavgb	mm6, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xf2
////-------------
	movq	mm2, mm6
	punpcklbw	mm6, mm7
	punpckhbw	mm2, mm7
	paddw	mm6, [ebx + 16]
	paddw	mm2, [ebx + 8 + 16]
	packuswb	mm6, mm2
	movq	mm0, [ecx + esi + 0]
//// pavgb	mm0, [ecx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x44
	_emit	0x31
	_emit	0x01
////-------------
	movq	mm2, [edx + esi + 0]
//// pavgb	mm2, [edx + esi + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x54
	_emit	0x32
	_emit	0x01
////-------------
//// pavgb	mm4, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xe0
////-------------
//// pavgb	mm5, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xea
////-------------
//// pavgb	mm4, mm5
	_emit	0x0f
	_emit	0xe0
	_emit	0xe5
////-------------
	movq	mm5, mm4
	punpcklbw	mm4, mm7
	punpckhbw	mm5, mm7
	paddw	mm4, [ebx + 0]
	paddw	mm5, [ebx + 8 + 0]
	packuswb	mm4, mm5
	mov	edx, [destp]
	movq	[edx + esi], mm4
	movq	[edx + esi + 8], mm6
	movq	mm5, [eax]
	movq	mm7, mm4
	punpcklbw	mm4, mm5
	punpckhbw	mm7, mm5
	movq [edi], mm4
	movq [edi + 8], mm7
	movq	mm5, [eax + 8]
	movq	mm7, mm6
	punpcklbw	mm6, mm5
	punpckhbw	mm7, mm5
	movq [edi + 16], mm6
	movq [edi + 24], mm7
	add	edi, [fbbpr]
	add	esi, [sbpr]
	add	ebx, [dbpr]
	add	eax, [sbpr]
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::ISDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	int f = ((ys & 1) << 3) | ((yp & 1) << 2) | ((xs & 1) << 1) | xp & 1;
	BYTE * temp;

	if (copy)
		{
		switch (f)
			{
			case 0:
				ISDBiMotionCopyY0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				ISDBiMotionCopyY0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				ISDBiMotionCopyY1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				ISDBiMotionCopyY0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				ISDBiMotionCopyY1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				ISDBiMotionCopyY0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				ISDBiMotionCopyY1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 12:
				ISDBiMotionCopyY0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				ISDBiMotionCopyY0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 15:
				ISDBiMotionCopyY1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				ISDBiMotionCompY0000(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				temp = prevp; prevp = succp; succp = temp;
			case 2:
				ISDBiMotionCompY0010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				ISDBiMotionCompY1010(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 4:
				temp = prevp; prevp = succp; succp = temp;
			case 8:
				ISDBiMotionCompY0001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 6:
				temp = prevp; prevp = succp; succp = temp;
			case 9:
				ISDBiMotionCompY1001(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 5:
				temp = prevp; prevp = succp; succp = temp;
			case 10:
				ISDBiMotionCompY0011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 7:
				temp = prevp; prevp = succp; succp = temp;
			case 11:
				ISDBiMotionCompY1011(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 12:
				ISDBiMotionCompY0101(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 13:
				temp = prevp; prevp = succp; succp = temp;
			case 14:
				ISDBiMotionCompY0111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 15:
				ISDBiMotionCompY1111(prevp, succp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	}

static __forceinline void MotionCopyUV00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompUV00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCopyUV10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompUV10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCopyUV01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompUV01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm2, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm3, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm3, mm4
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCopyUV11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x0a
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompUV11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x0a
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm2, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm3, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm3, mm4
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::MotionCompensationUV(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				MotionCopyUV00(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				MotionCopyUV10(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 2:
				MotionCopyUV01(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				MotionCopyUV11(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				MotionCompUV00(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				MotionCompUV10(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 2:
				MotionCompUV01(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				MotionCompUV11(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void SMotionCopyUV00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompUV00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCopyUV10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompUV10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCopyUV01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompUV01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm2, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm3, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCopyUV11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x0a
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompUV11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x02
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm0, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x0a
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm1, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x02
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 0]
	movq	mm6, [ebx + 16 + 0]
	punpcklwd	mm5, mm6
	paddw	mm2, mm5
	movq	mm5, [ebx + 0]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 2]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x0a
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	movq	mm5, [ebx + 8]
	movq	mm6, [ebx + 16 + 8]
	punpcklwd	mm5, mm6
	paddw	mm3, mm5
	movq	mm5, [ebx + 8]
	punpckhwd	mm5, mm6
	paddw	mm4, mm5
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::SMotionCompensationUV(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				SMotionCopyUV00(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				SMotionCopyUV10(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 2:
				SMotionCopyUV01(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SMotionCopyUV11(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				SMotionCompUV00(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				SMotionCompUV10(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 2:
				SMotionCompUV01(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SMotionCompUV11(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void MotionCopyY00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompY00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCopyY10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompY10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCopyY01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompY01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCopyY11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void MotionCompY11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	movq	[ebx], mm0
	movq	[ebx + 8], mm1
	add	ebx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	movq	[ebx], mm2
	movq	[ebx + 8], mm3
	add	ebx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::MotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				MotionCopyY00(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				MotionCopyY10(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 2:
				MotionCopyY01(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				MotionCopyY11(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				MotionCompY00(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 1:
				MotionCompY10(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 2:
				MotionCompY01(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			case 3:
				MotionCompY11(prevp, sbpr, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void SMotionCopyY00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompY00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCopyY10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompY10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCopyY01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompY01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCopyY11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SMotionCompY11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::SMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				SMotionCopyY00(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				SMotionCopyY10(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 2:
				SMotionCopyY01(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SMotionCopyY11(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				SMotionCompY00(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 1:
				SMotionCompY10(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 2:
				SMotionCompY01(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			case 3:
				SMotionCompY11(prevp, sbpr, destp, deltap, dbpr, dfinal);
				break;
			}
		}
	}

static __forceinline void DMotionCopyY00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCompY00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCopyY10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCompY10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCopyY01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCompY01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCopyY11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void DMotionCompY11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::DMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				DMotionCopyY00(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				DMotionCopyY10(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 2:
				DMotionCopyY01(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				DMotionCopyY11(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				DMotionCompY00(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				DMotionCompY10(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 2:
				DMotionCompY01(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				DMotionCompY11(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	}

static __forceinline void SDMotionCopyY00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCompY00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCopyY10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCompY10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCopyY01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCompY01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCopyY11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void SDMotionCompY11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::SDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				SDMotionCopyY00(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				SDMotionCopyY10(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 2:
				SDMotionCopyY01(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				SDMotionCopyY11(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				SDMotionCompY00(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 1:
				SDMotionCompY10(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 2:
				SDMotionCompY01(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			case 3:
				SDMotionCompY11(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr);
				break;
			}
		}
	}

static __forceinline void IDMotionCopyY00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm2, [eax + 0]
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCompY00(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm2, [eax + 0]
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCopyY10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCompY10(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCopyY01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCompY01(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCopyY11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void IDMotionCompY11(BYTE * prevp, int sbpr, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [dbpr]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, edi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, edi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, edi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, edi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, edi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::IDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				IDMotionCopyY00(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				IDMotionCopyY10(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 2:
				IDMotionCopyY01(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				IDMotionCopyY11(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				IDMotionCompY00(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				IDMotionCompY10(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 2:
				IDMotionCompY01(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				IDMotionCompY11(prevp, sbpr, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	}

static __forceinline void ISDMotionCopyY00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm2, [eax + 0]
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm0, [eax + 0]
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCompY00(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm2, [eax + 0]
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm0, [eax + 0]
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCopyY10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCompY10(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCopyY01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCompY01(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
	movq	mm1, [eax+8]
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm2, [eax + 0]
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCopyY11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

static __forceinline void ISDMotionCompY11(BYTE * prevp, int sbpr, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	__asm {
	mov	eax, [prevp]
	mov	ebx, [deltap]
	mov	ecx, [uvdp]
	mov	edx, [fbp]
	pxor	mm7, mm7
	mov esi, [fbbpr]
	sub esi, [ifbdist]
	mov [fbbpr], esi
	movq	mm0, [eax]
//// pavgb	mm0, [eax + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
	movq	mm1, [eax+8]
//// pavgb	mm1, [eax+8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
	add	eax, [sbpr]
	mov	edi, [destp]
	mov	esi, [sbpr]
loop1:
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	edx, [ifbdist]
	add	ecx, esi
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	sub	ecx, esi
	movq	mm2, [eax + 0]
//// pavgb	mm2, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x50
	_emit	0x01
////-------------
//// pavgb	mm0, mm2
	_emit	0x0f
	_emit	0xe0
	_emit	0xc2
////-------------
	movq	mm4, mm0
	punpcklbw	mm0, mm7
	punpckhbw	mm4, mm7
	paddw	mm0, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm0, mm4
	movq	mm3, [eax + 8]
//// pavgb	mm3, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x58
	_emit	0x09
////-------------
//// pavgb	mm1, mm3
	_emit	0x0f
	_emit	0xe0
	_emit	0xcb
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm1
	punpcklbw	mm1, mm7
	punpckhbw	mm4, mm7
	paddw	mm1, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm1, mm4
	add	ebx, [dbpr]
	movq	[edi], mm0
	movq	[edi + 8], mm1
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm0
	punpcklbw	mm0, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm0
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm1
	punpcklbw	mm1, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm1
	movq [edx + 24], mm4
	add	ecx, esi
	add	edx, [ifbdist]
	movq	mm0, [eax + 0]
//// pavgb	mm0, [eax + 0 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x40
	_emit	0x01
////-------------
//// pavgb	mm2, mm0
	_emit	0x0f
	_emit	0xe0
	_emit	0xd0
////-------------
	movq	mm4, mm2
	punpcklbw	mm2, mm7
	punpckhbw	mm4, mm7
	paddw	mm2, [ebx + 0]
	paddw	mm4, [ebx + 8 + 0]
	packuswb	mm2, mm4
	movq	mm1, [eax + 8]
//// pavgb	mm1, [eax + 8 + 1]
	_emit	0x0f
	_emit	0xe0
	_emit	0x48
	_emit	0x09
////-------------
//// pavgb	mm3, mm1
	_emit	0x0f
	_emit	0xe0
	_emit	0xd9
////-------------
	add	eax, esi
//// prefetch	[eax]
	_emit	0x0f
	_emit	0x18
	_emit	0x08
////-------------
	movq	mm4, mm3
	punpcklbw	mm3, mm7
	punpckhbw	mm4, mm7
	paddw	mm3, [ebx + 16]
	paddw	mm4, [ebx + 8 + 16]
	packuswb	mm3, mm4
	add	ebx, [dbpr]
	movq	[edi], mm2
	movq	[edi + 8], mm3
	add	edi, esi
	movq	mm5, [ecx]
	movq	mm4, mm2
	punpcklbw	mm2, mm5
	punpckhbw	mm4, mm5
	movq [edx], mm2
	movq [edx + 8], mm4
	movq	mm6, [ecx + 8]
	movq	mm4, mm3
	punpcklbw	mm3, mm6
	punpckhbw	mm4, mm6
	movq [edx + 16], mm3
	movq [edx + 24], mm4
	add	edx, [fbbpr]
	add	ecx, esi
	cmp	ebx, [dfinal]
	jne	loop1
	}
	}

void FastISSEMotionComp::ISDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist)
	{
	int f = ((yp & 1) << 1) | xp & 1;

	if (copy)
		{
		switch (f)
			{
			case 0:
				ISDMotionCopyY00(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				ISDMotionCopyY10(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 2:
				ISDMotionCopyY01(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				ISDMotionCopyY11(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	else
		{
		switch (f)
			{
			case 0:
				ISDMotionCompY00(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 1:
				ISDMotionCompY10(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 2:
				ISDMotionCompY01(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			case 3:
				ISDMotionCompY11(prevp, sbpr, destp, deltap, dbpr, dfinal, uvdp, fbp, fbbpr, ifbdist);
				break;
			}
		}
	}

