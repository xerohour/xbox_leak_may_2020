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

//#include "QFastMMXMotionComp.h"
#include "FastMMXSliceDecoder.h"

#pragma warning(disable : 4799)

static void CopyYUV(BYTE * src, BYTE * dst, int stride, int height)
	{
	__asm
		{
		mov		eax, [src]
		mov		ebx, [dst]

		movq		mm0, [eax]

		mov		ecx, [height]

		mov		edx, [stride]
		sub		ecx, 1

		movq		mm1, [eax+8]
loop1:
		movq		[ebx], mm0

		movq		mm0, [eax+32]

		add		eax, 32
		sub		ecx, 1

		movq		[ebx+8], mm1

		movq		mm1, [eax+8]

		lea		ebx, [ebx + edx]
		jne		loop1

		movq		[ebx], mm0

		movq		[ebx+8], mm1
		}
	}

static inline void CopyUV(BYTE * src, BYTE * dst, int stride)
	{
	__asm
		{
		mov			edx, [src]
		mov			ebx, [dst]
		mov			eax, [stride]
		mov			ecx, 4
loop1:
		movq			mm0, [edx+0]

		movq			mm2, [edx+8]
		movq			mm1, mm0

		movq			mm4, [edx+32]
		punpcklbw	mm0, mm2

		movq			mm6, [edx+40]
		punpckhbw	mm1, mm2

		movq			mm5, mm4
		sub			ecx, 1

		movq			[ebx+0], mm0
		punpcklbw	mm4, mm6

		movq			[ebx+8], mm1
		punpckhbw	mm5, mm6

		movq			[ebx+eax], mm4

		movq			[ebx+eax+8], mm5

		lea			edx, [edx + 64]
		lea			ebx, [ebx + 2*eax]
		jne			loop1
		}
	}

static void ICopyYUV(BYTE * src, BYTE * dst, int stride, int height)
	{
	__asm
		{
		mov		eax, [src]
		mov		ebx, [dst]

		movq		mm0, [eax]

		mov		ecx, [height]

		mov		edx, [stride]
		sub		ecx, 1

		movq		mm1, [eax+16]
loop1:
		movq		[ebx], mm0

		movq		mm0, [eax+32]

		add		eax, 32
		sub		ecx, 1

		movq		[ebx+8], mm1

		movq		mm1, [eax+16]

		lea		ebx, [ebx + edx]
		jne		loop1

		movq		[ebx], mm0

		movq		[ebx+8], mm1
		}
	}

static inline void ICopyUV(BYTE * src, BYTE * dst, int stride)
	{
	__asm
		{
		mov			edx, [src]
		mov			ebx, [dst]
		mov			eax, [stride]
		mov			ecx, 4
loop1:
		movq			mm0, [edx+0]

		movq			mm2, [edx+16]
		movq			mm1, mm0

		movq			mm4, [edx+32]
		punpcklbw	mm0, mm2

		movq			mm6, [edx+48]
		punpckhbw	mm1, mm2

		movq			mm5, mm4
		sub			ecx, 1

		movq			[ebx+0], mm0
		punpcklbw	mm4, mm6

		movq			[ebx+8], mm1
		punpckhbw	mm5, mm6

		movq			[ebx+eax], mm4

		movq			[ebx+eax+8], mm5

		lea			edx, [edx + 64]
		lea			ebx, [ebx + 2*eax]
		jne			loop1
		}
	}

static void MergeUV(BYTE * src)
	{
	__asm
		{
		mov	edx, [src]
		mov	ecx, 4
loop1:
		movq			mm0, [edx+0]

		movq			mm2, [edx+8]
		movq			mm1, mm0

		movq			mm4, [edx+32]
		punpcklbw	mm0, mm2

		movq			mm6, [edx+40]
		punpckhbw	mm1, mm2

		movq			mm5, mm4
		sub			ecx, 1

		movq			[edx+0], mm0
		punpcklbw	mm4, mm6

		movq			[edx+8], mm1
		punpckhbw	mm5, mm6

		movq			[edx+32], mm4

		movq			[edx+40], mm5

		lea			edx, [edx + 64]
		jne			loop1
		}
	}

static void MergeUV(short * src)
	{
	__asm
		{
		mov	edx, [src]
		mov	ecx, 8
loop1:
		movq			mm0, [edx+0]

		movq			mm2, [edx+16]
		movq			mm1, mm0

		movq			mm4, [edx+8]
		punpcklwd	mm0, mm2

		movq			mm6, [edx+24]
		punpckhwd	mm1, mm2

		movq			mm5, mm4
		sub			ecx, 1

		movq			[edx+0], mm0
		punpcklwd	mm4, mm6

		movq			[edx+8], mm1
		punpckhwd	mm5, mm6

		movq			[edx+16], mm4

		movq			[edx+24], mm5

		lea			edx, [edx + 32]
		jne			loop1
		}
	}

static inline int DIV2(int x)
	{
	return (x - (x >> 31)) >> 1;
	}

static inline void DirectMCopyYUV(BYTE * yp, BYTE * uvp, BYTE * fp, int stride)
	{
	__asm
		{
		mov	eax, [yp]
		mov	ebx, [uvp]
		mov	edi, [fp]
		mov	edx, [stride]
		mov	ecx, 8
loop1:
		movq			mm0, [eax]

		movq			mm2, [ebx]

		movq			mm6, [ebx+16]
		movq			mm7, mm2

		punpcklbw	mm2, mm6

		punpckhbw	mm7, mm6

		movq			mm3, [eax+32]

		movq			mm1, mm2
		movq			mm2, mm0

		punpcklbw	mm0, mm1

		movq			mm5, mm3
		punpckhbw	mm2, mm1

		movq			[edi], mm0

		punpcklbw	mm3, mm1

		movq			[edi+8], mm2

		punpckhbw	mm5, mm1

		movq			mm0, [eax+16]

		movq			mm4, [eax+48]

		movq			mm2, mm0

		punpcklbw	mm0, mm7

		movq			mm6, mm4
		punpckhbw	mm2, mm7

		movq			[edi+16], mm0

		punpcklbw	mm4, mm7
		lea			ebx, [ebx+32]

		movq			[edi+24], mm2

		punpckhbw	mm6, mm7
		sub			ecx, 1

		movq			[edi+edx], mm3

		movq			[edi+edx+8], mm5

		movq			[edi+edx+16], mm4

		lea			eax, [eax+64]

		movq			[edi+edx+24], mm5

		lea			edi, [edi+2*edx]
		jne			loop1
		}
	}

static inline void DirectMCopyYUV(BYTE * yp, BYTE * uvp, BYTE * fp, BYTE * ifp, int stride)
	{
	__asm
		{
		mov	eax, [yp]
		mov	ebx, [uvp]
		mov	edi, [fp]
		mov	esi, [ifp]
		mov	edx, [stride]
		mov	ecx, 8
loop1:
		movq			mm1, [ebx]
		movq			mm6, [ebx+16]
		movq			mm7, mm1
		punpcklbw	mm1, mm6
		punpckhbw	mm7, mm6
		sub			ecx, 2

		movq			mm0, [eax]
		movq			mm2, mm0
		punpcklbw	mm0, mm1
		punpckhbw	mm2, mm1
		movq			[edi], mm0

		movq			mm6, [eax+16]
		movq			[edi+8], mm2
		movq			mm2, mm6
		punpcklbw	mm6, mm7
		punpckhbw	mm2, mm7
		movq			[edi+16], mm6
		movq			[edi+24], mm2

		movq			mm0, [eax+64]
		movq			mm2, mm0
		punpcklbw	mm0, mm1
		punpckhbw	mm2, mm1
		movq			[edi+edx], mm0

		movq			mm6, [eax+64+16]
		movq			[edi+edx+8], mm2
		movq			mm2, mm6
		punpcklbw	mm6, mm7
		punpckhbw	mm2, mm7
		movq			[edi+edx+16], mm6

		movq			mm1, [ebx+32]
		movq			mm6, [ebx+48]
		movq			mm7, mm1
		punpcklbw	mm1, mm6
		punpckhbw	mm7, mm6

		movq			mm3, [eax+32]
		movq			[edi+edx+24], mm2
		movq			mm5, mm3
		punpcklbw	mm3, mm1
		punpckhbw	mm5, mm1
		movq			[esi], mm3
		movq			[esi+8], mm5

		movq			mm4, [eax+48]
		movq			mm0, mm4
		punpcklbw	mm4, mm7
		punpckhbw	mm0, mm7
		movq			[esi+16], mm4

		movq			mm3, [eax+64+32]
		movq			[esi+24], mm0
		movq			mm5, mm3
		punpcklbw	mm3, mm1
		punpckhbw	mm5, mm1
		movq			[esi+edx], mm3

		movq			mm4, [eax+64+48]
		movq			[esi+edx+8], mm5
		movq			mm0, mm4
		punpcklbw	mm4, mm7
		punpckhbw	mm0, mm7
		movq			[esi+edx+16], mm4

		lea			ebx, [ebx+64]
		lea			eax, [eax+128]
		lea			edi, [edi+2*edx]

		movq			[esi+edx+24], mm0
		lea			esi, [esi+2*edx]

		jne			loop1
		}
	}

static inline void DirectMSCopyYUV(BYTE * fp, int stride, BYTE * yd, BYTE * uvd, int slstride, int height)
	{
	__asm
		{
		mov         edx, [uvd]
		mov			edi, [slstride]

		mov         ecx, [yd]
		mov         ebx, [fp]

		mov			esi, height
		mov         eax, [stride]

loop1:
		movq        mm6, [edx]

		movq        mm0, [8+edx]
		movq        mm5, mm6

		movq        mm4, [ecx]
		punpcklbw   mm6, mm0

		punpckhbw   mm5, mm0

		movq        [edx], mm6

		movq        [8+edx], mm5
		movq        mm0, mm4

		punpcklbw   mm4, mm6

		punpckhbw   mm0, mm6

		movq        mm2, [8+ecx]

		movq        [ebx], mm4
		movq        mm4, mm2

		movq        [8+ebx], mm0

		movq        mm3, [ecx+edi]
		punpcklbw   mm2, mm5

		punpckhbw   mm4, mm5

		movq        [16+ebx], mm2

		movq        [24+ebx], mm4

		movq        mm0, mm3

		punpcklbw   mm3, mm6

		punpckhbw   mm0, mm6

		movq        mm1, [ecx+edi+8]

		lea         edx, [edx+edi]

		sub         esi, 1

		movq        mm6, mm1

		movq        [ebx+eax], mm3
		punpcklbw   mm1, mm5

		movq        [8+ebx+eax], mm0

		punpckhbw   mm6, mm5
		lea         ecx, [ecx+2*edi]

		movq        [16+ebx+eax], mm1

		movq        [24+ebx+eax], mm6

		lea         ebx, [ebx+2*eax]

		jne         loop1

		}
	}

static inline void MSCopyYUV(BYTE * yd, BYTE * uvd, int slstride)
	{
	__asm
		{
//MMSSTART

		mov         edx, [uvd]
		mov			edi, [slstride]

		mov         ecx, [yd]
		mov			esi, 8

loop1:
		movq        mm6, [edx]

		movq        mm0, [8+edx]

		movq        mm5, mm6

		punpckhbw   mm5, mm0

		punpcklbw   mm6, mm0

		movq        [edx], mm6

		movq        [8+edx], mm5

		lea         ecx, [ecx+2*edi]

		lea         edx, [edx+edi]

		sub         esi, 1
		jne         loop1

//MMSEND
		}
	}

//////////////////////////////////////////////////////

void QFastMMXSliceDecoder::ClearBlocks(void)
	{
	if ((codedBlockPattern & 0x3c) != 0x00 && (codedBlockPattern & 0x3c) != 0x3c)
		memclr512(&mby);
	if ((codedBlockPattern & 0x03) != 0x00 && (codedBlockPattern & 0x03) != 0x03)
		memclr256(&mbuv);
	}

void QFastMMXSliceDecoder::ParseIntraMacroblock(int mx, int my)
	{
	BPTR yc, uvc;
	int stride;

	codedBlockPattern = 0x3f;

	if (directAccess)
		{
		BYTE * fbp = this->fbp+2*mx;

		if (!needsFrameData)
			{
			ParseIntraBlocks();

			if (ifbp)
				{
				BYTE * ifbp = this->ifbp+2*mx;

				DirectMCopyYUV((BYTE *)&(mby[0][0]), (BYTE *)&(mbuv[0][0]), fbp, ifbp, fbstride);
				}
			else
				{
				DirectMCopyYUV((BYTE *)&(mby[0][0]), (BYTE *)&(mbuv[0][0]), fbp, fbstride);
				}
			}
		else
			{
			if (pictureStructure == PS_FRAME_PICTURE)
				curFrame->GetMPP(mx, my, yc, uvc, stride);
			else
				curFrame->GetIMPP(currentFieldID, mx, my, yc, uvc, stride);

			iyuvbpoffset[0] = (int)yc;
			iyuvbpoffset[1] = (int)uvc;
			iyuvbpoffset[2] = (int)uvc;

			ParseIntraBlocks();

			if (ifbp)
				{
				BYTE * ifbp = this->ifbp+2*mx;

				DirectMSCopyYUV(fbp, fbstride, yc, uvc, stride * 2, 4);
				DirectMSCopyYUV(ifbp, fbstride, yc + stride, uvc + stride, stride * 2, 4);
				}
			else
				DirectMSCopyYUV(fbp, fbstride, yc, uvc, stride, 8);
			}
		}
	else
		{
		if (pictureStructure == PS_FRAME_PICTURE)
			curFrame->GetMPP(mx, my, yc, uvc, stride);
		else
			curFrame->GetIMPP(currentFieldID, mx, my, yc, uvc, stride);

		if (!needsFrameData)
			{
			ParseIntraBlocks();

			ICopyYUV((BYTE *)&(mby[0][0]), yc, stride, 16);
			ICopyUV((BYTE *)&(mbuv[0][0]), uvc, stride);
			}
		else
			{
			iyuvbpoffset[0] = (int)yc;
			iyuvbpoffset[1] = (int)uvc;
			iyuvbpoffset[2] = (int)uvc;

			ParseIntraBlocks();

			MSCopyYUV(yc, uvc, stride);
			}
		}

	}

void QFastMMXSliceDecoder::MotionCompensation(int mx, int my, bool copyY, bool copyUV)
	{
	int px[2], sx[2];
	int py[2], sy[2];
	int cpx[2], csx[2];
	int cpy[2], csy[2];
	BPTR yp, uvp;
	BPTR ys, uvs;
	BPTR yc, uvc;
	int stride;

	BYTE * fbp = this->fbp+2*mx;

//if (!copyUV) MergeUV(&(mbuv[0][0]));


	if (directAccess && ifbp)
		{

		int ifbpdist = ifbp - this->fbp;

		curFrame->GetMPP(mx, my, yc, uvc, stride);

		if (predictionType == PT_FRAME_BASED)
			{
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				sx[0] = (mx * 2 + vectors[0][1][0]) & motionVectorMask;
				sy[0] = (my * 2 + vectors[0][1][1]) & motionVectorMask;
				csx[0] = mx + DIV2(vectors[0][1][0]);
				csy[0] = my + DIV2(vectors[0][1][1]);

				if (succFrame->GetYPP(sx[0] >> 1, sy[0] >> 1, ys))
					{
					succFrame->GetUVPP(csx[0] >> 1, csy[0] >> 1, uvs);

					if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
						{
						px[0] = (mx * 2 + vectors[0][0][0]) & motionVectorMask;
						py[0] = (my * 2 + vectors[0][0][1]) & motionVectorMask;
						cpx[0] = mx + DIV2(vectors[0][0][0]);
						cpy[0] = my + DIV2(vectors[0][0][1]);

						if (predFrame->GetYPP(px[0] >> 1, py[0] >> 1, yp))
							{
							predFrame->GetUVPP(cpx[0] >> 1, cpy[0] >> 1, uvp);

							if (needsFrameData)
								{
								motionComp->SBiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->ISDBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[16][0]),
										uvc, fbp, fbstride, ifbpdist);
								}
							else
								{
								motionComp->BiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										&(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->IDBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										&(mby[0][0]), 32, &(mby[16][0]),
										(BYTE *)&(mbuv[0][0]), fbp, fbstride, ifbpdist);
								}
							}
						else
							errorInSlice = true;
						}
					else
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->ISDMotionCompensationY(
									sy[0], sx[0],
									ys, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[16][0]),
									uvc, fbp, fbstride, ifbpdist);
							}
						else
							{
							motionComp->MotionCompensationUV(
									csy[0], csx[0],
									uvs, stride, copyUV,
									&(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->IDMotionCompensationY(
									sy[0], sx[0],
									ys, stride, copyY,
									&(mby[0][0]), 32, &(mby[16][0]),
									(BYTE *)&(mbuv[0][0]), fbp, fbstride, ifbpdist);
							}
						}
					}
				else
					errorInSlice = true;
				}
			else
				{
				px[0] = (mx * 2 + vectors[0][0][0]) & motionVectorMask;
				py[0] = (my * 2 + vectors[0][0][1]) & motionVectorMask;
				cpx[0] = mx + DIV2(vectors[0][0][0]);
				cpy[0] = my + DIV2(vectors[0][0][1]);

				if (predFrame->GetYPP(px[0] >> 1, py[0] >> 1, yp))
					{
					predFrame->GetUVPP(cpx[0] >> 1, cpy[0] >> 1, uvp);

					if (needsFrameData)
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, stride, copyUV,
								uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
						motionComp->ISDMotionCompensationY(
								py[0], px[0],
								yp, stride, copyY,
								yc, &(mby[0][0]), 32, &(mby[16][0]),
								uvc, fbp, fbstride, ifbpdist);
						}
					else
						{
						motionComp->MotionCompensationUV(
								cpy[0], cpx[0],
								uvp, stride, copyUV,
								&(mbuv[0][0]), 32, &(mbuv[8][0]));
						motionComp->IDMotionCompensationY(
								py[0], px[0],
								yp, stride, copyY,
								&(mby[0][0]), 32, &(mby[16][0]),
								(BYTE *)&(mbuv[0][0]), fbp, fbstride, ifbpdist);
						}
					}
				else
					errorInSlice = true;
				}
			}
		else if (predictionType == PT_DUAL_PRIME)
			{
			sx[0] = mx * 2 + vectors[0][0][0];
			sy[0] = my     + vectors[0][0][1];
			csx[0] = mx        + DIV2(vectors[0][0][0]);
			csy[0] = (my >> 1) + DIV2(vectors[0][0][1]);

			px[0] = mx * 2 + vectors[2][0][0];
			py[0] = my     + vectors[2][0][1];
			px[1] = mx * 2 + vectors[3][0][0];
			py[1] = my     + vectors[3][0][1];
			cpx[0] = mx        + DIV2(vectors[2][0][0]);
			cpy[0] = (my >> 1) + DIV2(vectors[2][0][1]);
			cpx[1] = mx        + DIV2(vectors[3][0][0]);
			cpy[1] = (my >> 1) + DIV2(vectors[3][0][1]);

			if (predFrame->GetYPPF(FALSE, sx[0] >> 1, sy[0] >> 1, ys) &&
				 predFrame->GetYPPF(TRUE, px[0] >> 1, py[0] >> 1, yp))
				{
				predFrame->GetUVPPF(FALSE, csx[0] >> 1, csy[0] >> 1, uvs);
				predFrame->GetUVPPF(TRUE, cpx[0] >> 1, cpy[0] >> 1, uvp);

				if (needsFrameData)
					{
					motionComp->SBiMotionCompensationUV(
							cpy[0], cpx[0], csy[0], csx[0],
							uvp, uvs, 2 * stride, copyUV,
							uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
					motionComp->SDBiMotionCompensationY(
							py[0], px[0], sy[0], sx[0],
							yp, ys, 2 * stride, copyY,
							yc, &(mby[0][0]), 64, &(mby[16][0]),
							uvc, fbp, fbstride);
					}
				else
					{
					motionComp->BiMotionCompensationUV(
							cpy[0], cpx[0], csy[0], csx[0],
							uvp, uvs, 2 * stride, copyUV,
							&(mbuv[0][0]), 64, &(mbuv[8][0]));
					motionComp->DBiMotionCompensationY(
							py[0], px[0], sy[0], sx[0],
							yp, ys, 2 * stride, copyY,
							&(mby[0][0]), 64, &(mby[16][0]),
							(BYTE *)&(mbuv[0][0]), fbp, fbstride);
					}
				}
			else
				errorInSlice = true;

			if (predFrame->GetYPPF(TRUE, sx[0] >> 1, sy[0] >> 1, ys) &&
			    predFrame->GetYPPF(FALSE, px[1] >> 1, py[1] >> 1, yp))
				{
				predFrame->GetUVPPF(TRUE, csx[0] >> 1, csy[0] >> 1, uvs);
				predFrame->GetUVPPF(FALSE, cpx[1] >> 1, cpy[1] >> 1, uvp);

				if (needsFrameData)
					{
					motionComp->SBiMotionCompensationUV(
							cpy[1], cpx[1], csy[0], csx[0],
							uvp, uvs, 2 * stride, copyUV,
							uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
					motionComp->SDBiMotionCompensationY(
							py[1], px[1], sy[0], sx[0],
							yp, ys, 2 * stride, copyY,
							yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
							uvc + stride, fbp + ifbpdist, fbstride);
					}
				else
					{
					motionComp->BiMotionCompensationUV(
							cpy[1], cpx[1], csy[0], csx[0],
							uvp, uvs, 2 * stride, copyUV,
							&(mbuv[1][0]), 64, &(mbuv[9][0]));
					motionComp->DBiMotionCompensationY(
							py[1], px[1], sy[0], sx[0],
							yp, ys, 2 * stride, copyY,
							&(mby[1][0]), 64, &(mby[17][0]),
							(BYTE *)&(mbuv[1][0]), fbp + ifbpdist, fbstride);
					}
				}
			else
				errorInSlice = true;
			}
		else
			{
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				sx[0] = (mx * 2 + vectors[0][1][0]) & motionVectorMask;
				sy[0] = (my     + vectors[0][1][1]) & motionVectorMask;
				sx[1] = (mx * 2 + vectors[1][1][0]) & motionVectorMask;
				sy[1] = (my     + vectors[1][1][1]) & motionVectorMask;
				csx[0] = mx        + DIV2(vectors[0][1][0]);
				csy[0] = (my >> 1) + DIV2(vectors[0][1][1]);
				csx[1] = mx        + DIV2(vectors[1][1][0]);
				csy[1] = (my >> 1) + DIV2(vectors[1][1][1]);

				if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
					{
					px[0] = (mx * 2 + vectors[0][0][0]) & motionVectorMask;
					py[0] = (my     + vectors[0][0][1]) & motionVectorMask;
					px[1] = (mx * 2 + vectors[1][0][0]) & motionVectorMask;
					py[1] = (my     + vectors[1][0][1]) & motionVectorMask;
					cpx[0] = mx        + DIV2(vectors[0][0][0]);
					cpy[0] = (my >> 1) + DIV2(vectors[0][0][1]);
					cpx[1] = mx        + DIV2(vectors[1][0][0]);
					cpy[1] = (my >> 1) + DIV2(vectors[1][0][1]);

					if (predFrame->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp) &&
						 succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys))
						{
						predFrame->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);
						succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);

						if (needsFrameData)
							{
							motionComp->SBiMotionCompensationUV(
									cpy[0], cpx[0], csy[0], csx[0],
									uvp, uvs, 2 * stride, copyUV,
									uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->SDBiMotionCompensationY(
									py[0], px[0], sy[0], sx[0],
									yp, ys, 2 * stride, copyY,
									yc, &(mby[0][0]), 64, &(mby[16][0]),
									uvc, fbp, fbstride);
							}
						else
							{
							motionComp->BiMotionCompensationUV(
									cpy[0], cpx[0], csy[0], csx[0],
									uvp, uvs, 2 * stride, copyUV,
									&(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->DBiMotionCompensationY(
									py[0], px[0], sy[0], sx[0],
									yp, ys, 2 * stride, copyY,
									&(mby[0][0]), 64, &(mby[16][0]),
									(BYTE *)&(mbuv[0][0]), fbp, fbstride);
							}
						}
					else
						errorInSlice = true;

					if (predFrame->GetYPPF(motionVerticalFieldSelect[1][0], px[1] >> 1, py[1] >> 1, yp) &&
						 succFrame->GetYPPF(motionVerticalFieldSelect[1][1], sx[1] >> 1, sy[1] >> 1, ys))
						{
						predFrame->GetUVPPF(motionVerticalFieldSelect[1][0], cpx[1] >> 1, cpy[1] >> 1, uvp);
						succFrame->GetUVPPF(motionVerticalFieldSelect[1][1], csx[1] >> 1, csy[1] >> 1, uvs);

						if (needsFrameData)
							{
							motionComp->SBiMotionCompensationUV(
									cpy[1], cpx[1], csy[1], csx[1],
									uvp, uvs, 2 * stride, copyUV,
									uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->SDBiMotionCompensationY(
									py[1], px[1], sy[1], sx[1],
									yp, ys, 2 * stride, copyY,
									yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
									uvc + stride, fbp + ifbpdist, fbstride);
							}
						else
							{
							motionComp->BiMotionCompensationUV(
									cpy[1], cpx[1], csy[1], csx[1],
									uvp, uvs, 2 * stride, copyUV,
									&(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->DBiMotionCompensationY(
									py[1], px[1], sy[1], sx[1],
									yp, ys, 2 * stride, copyY,
									&(mby[1][0]), 64, &(mby[17][0]),
									(BYTE *)&(mbuv[1][0]), fbp + ifbpdist, fbstride);
							}
						}
					else
						errorInSlice = true;
					}
				else
					{
					if (succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);

						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, 2 * stride, copyUV,
									uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->SDMotionCompensationY(
									sy[0], sx[0],
									ys, 2 * stride, copyY,
									yc, &(mby[0][0]), 64, &(mby[16][0]),
									uvc, fbp, fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									csy[0], csx[0],
									uvs, 2 * stride, copyUV,
									&(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->DMotionCompensationY(
									sy[0], sx[0],
									ys, 2 * stride, copyY,
									&(mby[0][0]), 64, &(mby[16][0]),
									(BYTE *)&(mbuv[0][0]), fbp, fbstride);
							}
						}
					else
						errorInSlice = true;

					if (succFrame->GetYPPF(motionVerticalFieldSelect[1][1], sx[1] >> 1, sy[1] >> 1, ys))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[1][1], csx[1] >> 1, csy[1] >> 1, uvs);

						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									csy[1], csx[1],
									uvs, 2 * stride, copyUV,
									uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->SDMotionCompensationY(
									sy[1], sx[1],
									ys, 2 * stride, copyY,
									yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
									uvc + stride, fbp + ifbpdist, fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									csy[1], csx[1],
									uvs, 2 * stride, copyUV,
									&(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->DMotionCompensationY(
									sy[1], sx[1],
									ys, 2 * stride, copyY,
									&(mby[1][0]), 64, &(mby[17][0]),
									(BYTE *)&(mbuv[1][0]), fbp + ifbpdist, fbstride);
							}
						}
					else
						errorInSlice = true;
					}
				}
			else
				{
				px[0] = (mx * 2 + vectors[0][0][0]) & motionVectorMask;
				py[0] = (my +     vectors[0][0][1]) & motionVectorMask;
				px[1] = (mx * 2 + vectors[1][0][0]) & motionVectorMask;
				py[1] = (my +     vectors[1][0][1]) & motionVectorMask;
				cpx[0] = mx        + DIV2(vectors[0][0][0]);
				cpy[0] = (my >> 1) + DIV2(vectors[0][0][1]);
				cpx[1] = mx        + DIV2(vectors[1][0][0]);
				cpy[1] = (my >> 1) + DIV2(vectors[1][0][1]);

				if (predFrame->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp))
					{
					predFrame->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);

					if (needsFrameData)
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, 2 * stride, copyUV,
								uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
						motionComp->SDMotionCompensationY(
								py[0], px[0],
								yp, 2 * stride, copyY,
								yc, &(mby[0][0]), 64, &(mby[16][0]),
								uvc, fbp, fbstride);
						}
					else
						{
						motionComp->MotionCompensationUV(
								cpy[0], cpx[0],
								uvp, 2 * stride, copyUV,
								&(mbuv[0][0]), 64, &(mbuv[8][0]));
						motionComp->DMotionCompensationY(
								py[0], px[0],
								yp, 2 * stride, copyY,
								&(mby[0][0]), 64, &(mby[16][0]),
								(BYTE *)&(mbuv[0][0]), fbp, fbstride);
						}
					}
				else
					errorInSlice = true;

				if (predFrame->GetYPPF(motionVerticalFieldSelect[1][0], px[1] >> 1, py[1] >> 1, yp))
					{
					predFrame->GetUVPPF(motionVerticalFieldSelect[1][0], cpx[1] >> 1, cpy[1] >> 1, uvp);

					if (needsFrameData)
						{
						motionComp->SMotionCompensationUV(
								cpy[1], cpx[1],
								uvp, 2 * stride, copyUV,
								uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
						motionComp->SDMotionCompensationY(
								py[1], px[1],
								yp, 2 * stride, copyY,
								yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
								uvc + stride, fbp + ifbpdist, fbstride);
						}
					else
						{
						motionComp->MotionCompensationUV(
								cpy[1], cpx[1],
								uvp, 2 * stride, copyUV,
								&(mbuv[1][0]), 64, &(mbuv[9][0]));
						motionComp->DMotionCompensationY(
								py[1], px[1],
								yp, 2 * stride, copyY,
								&(mby[1][0]), 64, &(mby[17][0]),
								(BYTE *)&(mbuv[1][0]), fbp + ifbpdist, fbstride);
						}
					}
				else
					errorInSlice = true;
				}
			}
		}
	else if (pictureStructure == PS_FRAME_PICTURE)
		{
		curFrame->GetMPP(mx, my, yc, uvc, stride);

		if (predictionType == PT_FRAME_BASED)
			{
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				sx[0] = mx * 2 + vectors[0][1][0] & motionVectorMask;
				sy[0] = my * 2 + vectors[0][1][1] & motionVectorMask;
				csx[0] = mx + DIV2(vectors[0][1][0]);
				csy[0] = my + DIV2(vectors[0][1][1]);

				if (succFrame->GetYPP(sx[0] >> 1, sy[0] >> 1, ys))
					{
					succFrame->GetUVPP(csx[0] >> 1, csy[0] >> 1, uvs);

					if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
						{
						px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
						py[0] = my * 2 + vectors[0][0][1] & motionVectorMask;
						cpx[0] = mx + DIV2(vectors[0][0][0]);
						cpy[0] = my + DIV2(vectors[0][0][1]);

						if (predFrame->GetYPP(px[0] >> 1, py[0] >> 1, yp))
							{
							predFrame->GetUVPP(cpx[0] >> 1, cpy[0] >> 1, uvp);

							if (directAccess)
								{
								if (needsFrameData)
									{
									motionComp->SBiMotionCompensationUV(
											cpy[0], cpx[0], csy[0], csx[0],
											uvp, uvs, stride, copyUV,
											uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
									motionComp->SDBiMotionCompensationY(
											py[0], px[0], sy[0], sx[0],
											yp, ys, stride, copyY,
											yc, &(mby[0][0]), 32, &(mby[16][0]),
											uvc, fbp, fbstride);
									}
								else
									{
									motionComp->BiMotionCompensationUV(
											cpy[0], cpx[0], csy[0], csx[0],
											uvp, uvs, stride, copyUV,
											&(mbuv[0][0]), 32, &(mbuv[8][0]));
									motionComp->DBiMotionCompensationY(
											py[0], px[0], sy[0], sx[0],
											yp, ys, stride, copyY,
											&(mby[0][0]), 32, &(mby[16][0]),
											(BYTE *)&(mbuv[0][0]), fbp, fbstride);
									}
								}
							else
								{
								motionComp->SBiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->SBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[16][0]));
								}
							}
						else
							errorInSlice = true;
						}
					else
						{
						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SMotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->SDMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[16][0]),
										uvc, fbp, fbstride);
								}
							else
								{
								motionComp->MotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										&(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->DMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										&(mby[0][0]), 32, &(mby[16][0]),
										(BYTE *)&(mbuv[0][0]), fbp, fbstride);
								}
							}
						else
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->SMotionCompensationY(
									sy[0], sx[0],
									ys, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[16][0]));
							}
						}
					}
				else
					errorInSlice = true;
				}
			else
				{
				px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
				py[0] = my * 2 + vectors[0][0][1] & motionVectorMask;
				cpx[0] = mx + DIV2(vectors[0][0][0]);
				cpy[0] = my + DIV2(vectors[0][0][1]);

				if (predFrame->GetYPP(px[0] >> 1, py[0] >> 1, yp))
					{
					predFrame->GetUVPP(cpx[0] >> 1, cpy[0] >> 1, uvp);

					if (directAccess)
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->SDMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[16][0]),
									uvc, fbp, fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									&(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->DMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									&(mby[0][0]), 32, &(mby[16][0]),
									(BYTE *)&(mbuv[0][0]), fbp, fbstride);
							}
						}
					else
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, stride, copyUV,
								uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
						motionComp->SMotionCompensationY(
								py[0], px[0],
								yp, stride, copyY,
								yc, &(mby[0][0]), 32, &(mby[16][0]));
						}
					}
				else
					errorInSlice = true;
				}
			}
		else if (predictionType == PT_DUAL_PRIME)
			{
			sx[0] = mx * 2 + vectors[0][0][0];
			sy[0] = my     + vectors[0][0][1];
			csx[0] = mx        + DIV2(vectors[0][0][0]);
			csy[0] = (my >> 1) + DIV2(vectors[0][0][1]);

			px[0] = mx * 2 + vectors[2][0][0];
			py[0] = my     + vectors[2][0][1];
			px[1] = mx * 2 + vectors[3][0][0];
			py[1] = my     + vectors[3][0][1];
			cpx[0] = mx        + DIV2(vectors[2][0][0]);
			cpy[0] = (my >> 1) + DIV2(vectors[2][0][1]);
			cpx[1] = mx        + DIV2(vectors[3][0][0]);
			cpy[1] = (my >> 1) + DIV2(vectors[3][0][1]);

			if (predFrame->GetYPPF(FALSE, sx[0] >> 1, sy[0] >> 1, ys) &&
				 predFrame->GetYPPF(TRUE, px[0] >> 1, py[0] >> 1, yp))
				{
				predFrame->GetUVPPF(FALSE, csx[0] >> 1, csy[0] >> 1, uvs);
				predFrame->GetUVPPF(TRUE, cpx[0] >> 1, cpy[0] >> 1, uvp);

				if (directAccess)
					{
					if (needsFrameData)
						{
						motionComp->SBiMotionCompensationUV(
								cpy[0], cpx[0], csy[0], csx[0],
								uvp, uvs, 2 * stride, copyUV,
								uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
						motionComp->SDBiMotionCompensationY(
								py[0], px[0], sy[0], sx[0],
								yp, ys, 2 * stride, copyY,
								yc, &(mby[0][0]), 64, &(mby[16][0]),
								uvc, fbp, fbstride * 2);
						}
					else
						{
						motionComp->BiMotionCompensationUV(
								cpy[0], cpx[0], csy[0], csx[0],
								uvp, uvs, 2 * stride, copyUV,
								&(mbuv[0][0]), 64, &(mbuv[8][0]));
						motionComp->DBiMotionCompensationY(
								py[0], px[0], sy[0], sx[0],
								yp, ys, 2 * stride, copyY,
								&(mby[0][0]), 64, &(mby[16][0]),
								(BYTE *)&(mbuv[0][0]), fbp, fbstride * 2);
						}
					}
				else
					{
					motionComp->SBiMotionCompensationUV(
							cpy[0], cpx[0], csy[0], csx[0],
							uvp, uvs, 2 * stride, copyUV,
							uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
					motionComp->SBiMotionCompensationY(
							py[0], px[0], sy[0], sx[0],
							yp, ys, 2 * stride, copyY,
							yc, &(mby[0][0]), 64, &(mby[16][0]));
					}
				}
			else
				errorInSlice = true;

			if (predFrame->GetYPPF(TRUE, sx[0] >> 1, sy[0] >> 1, ys) &&
			    predFrame->GetYPPF(FALSE, px[1] >> 1, py[1] >> 1, yp))
				{
				predFrame->GetUVPPF(TRUE, csx[0] >> 1, csy[0] >> 1, uvs);
				predFrame->GetUVPPF(FALSE, cpx[1] >> 1, cpy[1] >> 1, uvp);

				if (directAccess)
					{
					if (needsFrameData)
						{
						motionComp->SBiMotionCompensationUV(
								cpy[1], cpx[1], csy[0], csx[0],
								uvp, uvs, 2 * stride, copyUV,
								uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
						motionComp->SDBiMotionCompensationY(
								py[1], px[1], sy[0], sx[0],
								yp, ys, 2 * stride, copyY,
								yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
								uvc + stride, fbp + fbstride, fbstride * 2);
						}
					else
						{
						motionComp->BiMotionCompensationUV(
								cpy[1], cpx[1], csy[0], csx[0],
								uvp, uvs, 2 * stride, copyUV,
								&(mbuv[1][0]), 64, &(mbuv[9][0]));
						motionComp->DBiMotionCompensationY(
								py[1], px[1], sy[0], sx[0],
								yp, ys, 2 * stride, copyY,
								&(mby[1][0]), 64, &(mby[17][0]),
								(BYTE *)&(mbuv[1][0]), fbp + fbstride, fbstride * 2);
						}
					}
				else
					{
					motionComp->SBiMotionCompensationUV(
							cpy[1], cpx[1], csy[0], csx[0],
							uvp, uvs, 2 * stride, copyUV,
							uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
					motionComp->SBiMotionCompensationY(
							py[1], px[1], sy[0], sx[0],
							yp, ys, 2 * stride, copyY,
							yc + stride, &(mby[1][0]), 64, &(mby[17][0]));
					}
				}
			else
				errorInSlice = true;
			}
		else
			{
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				sx[0] = mx * 2 + vectors[0][1][0] & motionVectorMask;
				sy[0] = my     + vectors[0][1][1] & motionVectorMask;
				sx[1] = mx * 2 + vectors[1][1][0] & motionVectorMask;
				sy[1] = my     + vectors[1][1][1] & motionVectorMask;
				csx[0] = mx        + DIV2(vectors[0][1][0]);
				csy[0] = (my >> 1) + DIV2(vectors[0][1][1]);
				csx[1] = mx        + DIV2(vectors[1][1][0]);
				csy[1] = (my >> 1) + DIV2(vectors[1][1][1]);

				if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
					{
					px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
					py[0] = my     + vectors[0][0][1] & motionVectorMask;
					px[1] = mx * 2 + vectors[1][0][0] & motionVectorMask;
					py[1] = my     + vectors[1][0][1] & motionVectorMask;
					cpx[0] = mx        + DIV2(vectors[0][0][0]);
					cpy[0] = (my >> 1) + DIV2(vectors[0][0][1]);
					cpx[1] = mx        + DIV2(vectors[1][0][0]);
					cpy[1] = (my >> 1) + DIV2(vectors[1][0][1]);

					if (predFrame->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp) &&
						 succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys))
						{
						predFrame->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);
						succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SBiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, 2 * stride, copyUV,
										uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
								motionComp->SDBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, 2 * stride, copyY,
										yc, &(mby[0][0]), 64, &(mby[16][0]),
										uvc, fbp, fbstride * 2);
								}
							else
								{
								motionComp->BiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, 2 * stride, copyUV,
										&(mbuv[0][0]), 64, &(mbuv[8][0]));
								motionComp->DBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, 2 * stride, copyY,
										&(mby[0][0]), 64, &(mby[16][0]),
										(BYTE *)&(mbuv[0][0]), fbp, fbstride * 2);
								}
							}
						else
							{
							motionComp->SBiMotionCompensationUV(
									cpy[0], cpx[0], csy[0], csx[0],
									uvp, uvs, 2 * stride, copyUV,
									uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->SBiMotionCompensationY(
									py[0], px[0], sy[0], sx[0],
									yp, ys, 2 * stride, copyY,
									yc, &(mby[0][0]), 64, &(mby[16][0]));
							}
						}
					else
						errorInSlice = true;

					if (predFrame->GetYPPF(motionVerticalFieldSelect[1][0], px[1] >> 1, py[1] >> 1, yp) &&
						 succFrame->GetYPPF(motionVerticalFieldSelect[1][1], sx[1] >> 1, sy[1] >> 1, ys))
						{
						predFrame->GetUVPPF(motionVerticalFieldSelect[1][0], cpx[1] >> 1, cpy[1] >> 1, uvp);
						succFrame->GetUVPPF(motionVerticalFieldSelect[1][1], csx[1] >> 1, csy[1] >> 1, uvs);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SBiMotionCompensationUV(
										cpy[1], cpx[1], csy[1], csx[1],
										uvp, uvs, 2 * stride, copyUV,
										uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
								motionComp->SDBiMotionCompensationY(
										py[1], px[1], sy[1], sx[1],
										yp, ys, 2 * stride, copyY,
										yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
										uvc + stride, fbp + fbstride, fbstride * 2);
								}
							else
								{
								motionComp->BiMotionCompensationUV(
										cpy[1], cpx[1], csy[1], csx[1],
										uvp, uvs, 2 * stride, copyUV,
										&(mbuv[1][0]), 64, &(mbuv[9][0]));
								motionComp->DBiMotionCompensationY(
										py[1], px[1], sy[1], sx[1],
										yp, ys, 2 * stride, copyY,
										&(mby[1][0]), 64, &(mby[17][0]),
										(BYTE *)&(mbuv[1][0]), fbp + fbstride, fbstride * 2);
								}
							}
						else
							{
							motionComp->SBiMotionCompensationUV(
									cpy[1], cpx[1], csy[1], csx[1],
									uvp, uvs, 2 * stride, copyUV,
									uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->SBiMotionCompensationY(
									py[1], px[1], sy[1], sx[1],
									yp, ys, 2 * stride, copyY,
									yc + stride, &(mby[1][0]), 64, &(mby[17][0]));
							}
						}
					else
						errorInSlice = true;
					}
				else
					{
					if (succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SMotionCompensationUV(
										csy[0], csx[0],
										uvs, 2 * stride, copyUV,
										uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
								motionComp->SDMotionCompensationY(
										sy[0], sx[0],
										ys, 2 * stride, copyY,
										yc, &(mby[0][0]), 64, &(mby[16][0]),
										uvc, fbp, 2 * fbstride);
								}
							else
								{
								motionComp->MotionCompensationUV(
										csy[0], csx[0],
										uvs, 2 * stride, copyUV,
										&(mbuv[0][0]), 64, &(mbuv[8][0]));
								motionComp->DMotionCompensationY(
										sy[0], sx[0],
										ys, 2 * stride, copyY,
										&(mby[0][0]), 64, &(mby[16][0]),
										(BYTE *)&(mbuv[0][0]), fbp, 2 * fbstride);
								}
							}
						else
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, 2 * stride, copyUV,
									uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->SMotionCompensationY(
									sy[0], sx[0],
									ys, 2 * stride, copyY,
									yc, &(mby[0][0]), 64, &(mby[16][0]));
							}
						}
					else
						errorInSlice = true;

					if (succFrame->GetYPPF(motionVerticalFieldSelect[1][1], sx[1] >> 1, sy[1] >> 1, ys))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[1][1], csx[1] >> 1, csy[1] >> 1, uvs);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SMotionCompensationUV(
										csy[1], csx[1],
										uvs, 2 * stride, copyUV,
										uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
								motionComp->SDMotionCompensationY(
										sy[1], sx[1],
										ys, 2 * stride, copyY,
										yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
										uvc + stride, fbp + fbstride, 2 * fbstride);
								}
							else
								{
								motionComp->MotionCompensationUV(
										csy[1], csx[1],
										uvs, 2 * stride, copyUV,
										&(mbuv[1][0]), 64, &(mbuv[9][0]));
								motionComp->DMotionCompensationY(
										sy[1], sx[1],
										ys, 2 * stride, copyY,
										&(mby[1][0]), 64, &(mby[17][0]),
										(BYTE *)&(mbuv[1][0]), fbp + fbstride, 2 * fbstride);
								}
							}
						else
							{
							motionComp->SMotionCompensationUV(
									csy[1], csx[1],
									uvs, 2 * stride, copyUV,
									uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->SMotionCompensationY(
									sy[1], sx[1],
									ys, 2 * stride, copyY,
									yc + stride, &(mby[1][0]), 64, &(mby[17][0]));
							}
						}
					else
						errorInSlice = true;
					}
				}
			else
				{
				px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
				py[0] = my + vectors[0][0][1] & motionVectorMask;
				px[1] = mx * 2 + vectors[1][0][0] & motionVectorMask;
				py[1] = my + vectors[1][0][1] & motionVectorMask;
				cpx[0] = mx        + DIV2(vectors[0][0][0]);
				cpy[0] = (my >> 1) + DIV2(vectors[0][0][1]);
				cpx[1] = mx        + DIV2(vectors[1][0][0]);
				cpy[1] = (my >> 1) + DIV2(vectors[1][0][1]);

				if (predFrame->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp))
					{
					predFrame->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);

					if (directAccess)
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									cpy[0], cpx[0],
									uvp, 2 * stride, copyUV,
									uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->SDMotionCompensationY(
									py[0], px[0],
									yp, 2 * stride, copyY,
									yc, &(mby[0][0]), 64, &(mby[16][0]),
									uvc, fbp, 2 * fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									cpy[0], cpx[0],
									uvp, 2 * stride, copyUV,
									&(mbuv[0][0]), 64, &(mbuv[8][0]));
							motionComp->DMotionCompensationY(
									py[0], px[0],
									yp, 2 * stride, copyY,
									&(mby[0][0]), 64, &(mby[16][0]),
									(BYTE *)&(mbuv[0][0]), fbp, 2 * fbstride);
							}
						}
					else
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, 2 * stride, copyUV,
								uvc, &(mbuv[0][0]), 64, &(mbuv[8][0]));
						motionComp->SMotionCompensationY(
								py[0], px[0],
								yp, 2 * stride, copyY,
								yc, &(mby[0][0]), 64, &(mby[16][0]));
						}
					}
				else
					errorInSlice = true;

				if (predFrame->GetYPPF(motionVerticalFieldSelect[1][0], px[1] >> 1, py[1] >> 1, yp))
					{
					predFrame->GetUVPPF(motionVerticalFieldSelect[1][0], cpx[1] >> 1, cpy[1] >> 1, uvp);

					if (directAccess)
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									cpy[1], cpx[1],
									uvp, 2 * stride, copyUV,
									uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->SDMotionCompensationY(
									py[1], px[1],
									yp, 2 * stride, copyY,
									yc + stride, &(mby[1][0]), 64, &(mby[17][0]),
									uvc + stride, fbp + fbstride, 2 * fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									cpy[1], cpx[1],
									uvp, 2 * stride, copyUV,
									&(mbuv[1][0]), 64, &(mbuv[9][0]));
							motionComp->DMotionCompensationY(
									py[1], px[1],
									yp, 2 * stride, copyY,
									&(mby[1][0]), 64, &(mby[17][0]),
									(BYTE *)&(mbuv[1][0]), fbp + fbstride, 2 * fbstride);
							}
						}
					else
						{
						motionComp->SMotionCompensationUV(
								cpy[1], cpx[1],
								uvp, 2 * stride, copyUV,
								uvc + stride, &(mbuv[1][0]), 64, &(mbuv[9][0]));
						motionComp->SMotionCompensationY(
								py[1], px[1],
								yp, 2 * stride, copyY,
								yc + stride, &(mby[1][0]), 64, &(mby[17][0]));
						}
					}
				else
					errorInSlice = true;
				}
			}
		}
	else
		{
		curFrame->GetIMPP(currentFieldID, mx, my, yc, uvc, stride);

		if (predictionType == PT_FIELD_BASED)
			{
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				sx[0] = mx * 2 + vectors[0][1][0] & motionVectorMask;
				sy[0] = my * 2 + vectors[0][1][1] & motionVectorMask;
				csx[0] = mx + DIV2(vectors[0][1][0]);
				csy[0] = my + DIV2(vectors[0][1][1]);

				if (succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys))
					{
					succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);

					if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
						{
						px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
						py[0] = my * 2 + vectors[0][0][1] & motionVectorMask;
						cpx[0] = mx + DIV2(vectors[0][0][0]);
						cpy[0] = my + DIV2(vectors[0][0][1]);

						if (predFrame->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp))
							{
							predFrame->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);

							if (directAccess)
								{
								if (needsFrameData)
									{
									motionComp->SBiMotionCompensationUV(
											cpy[0], cpx[0], csy[0], csx[0],
											uvp, uvs, stride, copyUV,
											uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
									motionComp->SDBiMotionCompensationY(
											py[0], px[0], sy[0], sx[0],
											yp, ys, stride, copyY,
											yc, &(mby[0][0]), 32, &(mby[16][0]),
											uvc, fbp, fbstride);
									}
								else
									{
									motionComp->BiMotionCompensationUV(
											cpy[0], cpx[0], csy[0], csx[0],
											uvp, uvs, stride, copyUV,
											&(mbuv[0][0]), 32, &(mbuv[8][0]));
									motionComp->DBiMotionCompensationY(
											py[0], px[0], sy[0], sx[0],
											yp, ys, stride, copyY,
											&(mby[0][0]), 32, &(mby[16][0]),
											(BYTE *)&(mbuv[0][0]), fbp, fbstride);
									}
								}
							else
								{
								motionComp->SBiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->SBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[16][0]));
								}
							}
						else
							errorInSlice = true;
						}
					else
						{
						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SMotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->SDMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[16][0]),
										uvc, fbp, fbstride);
								}
							else
								{
								motionComp->MotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										&(mbuv[0][0]), 32, &(mbuv[8][0]));
								motionComp->DMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										&(mby[0][0]), 32, &(mby[16][0]),
										(BYTE *)&(mbuv[0][0]), fbp, fbstride);
								}
							}
						else
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->SMotionCompensationY(
									sy[0], sx[0],
									ys, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[16][0]));
							}
						}
					}
				else
					errorInSlice = true;
				}
			else
				{
				px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
				py[0] = my * 2 + vectors[0][0][1] & motionVectorMask;
				cpx[0] = mx + DIV2(vectors[0][0][0]);
				cpy[0] = my + DIV2(vectors[0][0][1]);

				if (predFields[motionVerticalFieldSelect[0][0]]->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp))
					{
					predFields[motionVerticalFieldSelect[0][0]]->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);

					if (directAccess)
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->SDMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[16][0]),
									uvc, fbp, fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									&(mbuv[0][0]), 32, &(mbuv[8][0]));
							motionComp->DMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									&(mby[0][0]), 32, &(mby[16][0]),
									(BYTE *)&(mbuv[0][0]), fbp, fbstride);
							}
						}
					else
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, stride, copyUV,
								uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
						motionComp->SMotionCompensationY(
								py[0], px[0],
								yp, stride, copyY,
								yc, &(mby[0][0]), 32, &(mby[16][0]));
						}
					}
				else
					errorInSlice = true;
				}
			}
		else if (predictionType == PT_DUAL_PRIME)
			{
			sx[0] = mx * 2 + vectors[0][0][0];
			sy[0] = my * 2 + vectors[0][0][1];
			csx[0] = mx + DIV2(vectors[0][0][0]);
			csy[0] = my + DIV2(vectors[0][0][1]);
			px[0] = mx * 2 + vectors[2][0][0];
			py[0] = my * 2 + vectors[2][0][1];
			cpx[0] = mx + DIV2(vectors[2][0][0]);
			cpy[0] = my + DIV2(vectors[2][0][1]);

			if (predFields[!currentFieldID]->GetYPPF(!currentFieldID, px[0] >> 1, py[0] >> 1, yp) &&
			    predFields[currentFieldID]->GetYPPF(currentFieldID, sx[0] >> 1, sy[0] >> 1, ys))
				{
				predFields[!currentFieldID]->GetUVPPF(!currentFieldID, cpx[0] >> 1, cpy[0] >> 1, uvp);
				predFields[currentFieldID]->GetUVPPF(currentFieldID, csx[0] >> 1, csy[0] >> 1, uvs);

				if (directAccess)
					{
					if (needsFrameData)
						{
						motionComp->SBiMotionCompensationUV(
								cpy[0], cpx[0], csy[0], csx[0],
								uvp, uvs, stride, copyUV,
								uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
						motionComp->SDBiMotionCompensationY(
								py[0], px[0], sy[0], sx[0],
								yp, ys, stride, copyY,
								yc, &(mby[0][0]), 32, &(mby[16][0]),
								uvc, fbp, fbstride);
						}
					else
						{
						motionComp->BiMotionCompensationUV(
								cpy[0], cpx[0], csy[0], csx[0],
								uvp, uvs, stride, copyUV,
								&(mbuv[0][0]), 32, &(mbuv[8][0]));
						motionComp->DBiMotionCompensationY(
								py[0], px[0], sy[0], sx[0],
								yp, ys, stride, copyY,
								&(mby[0][0]), 32, &(mby[16][0]),
								(BYTE *)&(mbuv[0][0]), fbp, fbstride);
						}
					}
				else
					{
					motionComp->SBiMotionCompensationUV(
							cpy[0], cpx[0], csy[0], csx[0],
							uvp, uvs, stride, copyUV,
							uvc, &(mbuv[0][0]), 32, &(mbuv[8][0]));
					motionComp->SBiMotionCompensationY(
							py[0], px[0], sy[0], sx[0],
							yp, ys, stride, copyY,
							yc, &(mby[0][0]), 32, &(mby[16][0]));
					}
				}
			else
				errorInSlice = true;
			}
		else
			{
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				sx[0] = mx * 2 + vectors[0][1][0] & motionVectorMask;
				sy[0] = my * 2 + vectors[0][1][1] & motionVectorMask;
				sx[1] = mx * 2 + vectors[1][1][0] & motionVectorMask;
				sy[1] = my * 2 + (vectors[1][1][1] + 16)  & motionVectorMask;
				csx[0] = mx + DIV2(vectors[0][1][0]);
				csy[0] = my + DIV2(vectors[0][1][1]);
				csx[1] = mx + DIV2(vectors[1][1][0]);
				csy[1] = my + DIV2(vectors[1][1][1]) + 8;

				if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
					{
					px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
					py[0] = my * 2 + vectors[0][0][1] & motionVectorMask;
					px[1] = mx * 2 + vectors[1][0][0] & motionVectorMask;
					py[1] = my * 2 + (vectors[1][0][1] + 16)  & motionVectorMask;
					cpx[0] = mx + DIV2(vectors[0][0][0]);
					cpy[0] = my + DIV2(vectors[0][0][1]);
					cpx[1] = mx + DIV2(vectors[1][0][0]);
					cpy[1] = my + DIV2(vectors[1][0][1]) + 8;

					if (succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys) &&
						 predFrame->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);
						predFrame->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SBiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[4][0]));
								motionComp->SDBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[8][0]),
										uvc, fbp, fbstride);
								}
							else
								{
								motionComp->BiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										&(mbuv[0][0]), 32, &(mbuv[4][0]));
								motionComp->DBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										&(mby[0][0]), 32, &(mby[8][0]),
										(BYTE *)&(mbuv[0][0]), fbp, fbstride);
								}
							}
						else
							{
							motionComp->SBiMotionCompensationUV(
									cpy[0], cpx[0], csy[0], csx[0],
									uvp, uvs, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[4][0]));
							motionComp->SBiMotionCompensationY(
									py[0], px[0], sy[0], sx[0],
									yp, ys, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[8][0]));
							}
						}
					else
						errorInSlice = true;

					if (succFrame->GetYPPF(motionVerticalFieldSelect[1][1], sx[1] >> 1, sy[1] >> 1, ys) &&
						 predFrame->GetYPPF(motionVerticalFieldSelect[1][0], px[1] >> 1, py[1] >> 1, yp))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[1][1], csx[1] >> 1, csy[1] >> 1, uvs);
						predFrame->GetUVPPF(motionVerticalFieldSelect[1][0], cpx[1] >> 1, cpy[1] >> 1, uvp);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SBiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										uvc + 4 * stride, &(mbuv[4][0]), 32, &(mbuv[8][0]));
								motionComp->SDBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										yc + 8 * stride, &(mby[8][0]), 32, &(mby[16][0]),
										uvc + 4 * stride, fbp + 8 * fbstride, fbstride);
								}
							else
								{
								motionComp->BiMotionCompensationUV(
										cpy[0], cpx[0], csy[0], csx[0],
										uvp, uvs, stride, copyUV,
										&(mbuv[4][0]), 32, &(mbuv[8][0]));
								motionComp->DBiMotionCompensationY(
										py[0], px[0], sy[0], sx[0],
										yp, ys, stride, copyY,
										&(mby[8][0]), 32, &(mby[16][0]),
										(BYTE *)&(mbuv[4][0]), fbp + 8 * fbstride, fbstride);
								}
							}
						else
							{
							motionComp->SBiMotionCompensationUV(
									cpy[0], cpx[0], csy[0], csx[0],
									uvp, uvs, stride, copyUV,
									uvc + 4 * stride, &(mbuv[4][0]), 32, &(mbuv[8][0]));
							motionComp->SBiMotionCompensationY(
									py[0], px[0], sy[0], sx[0],
									yp, ys, stride, copyY,
									yc + 8 * stride, &(mby[8][0]), 32, &(mby[16][0]));
							}
						}
					else
						errorInSlice = true;
					}
				else
					{
					if (succFrame->GetYPPF(motionVerticalFieldSelect[0][1], sx[0] >> 1, sy[0] >> 1, ys))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[0][1], csx[0] >> 1, csy[0] >> 1, uvs);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SMotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										uvc, &(mbuv[0][0]), 32, &(mbuv[4][0]));
								motionComp->SDMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										yc, &(mby[0][0]), 32, &(mby[8][0]),
										uvc, fbp, fbstride);
								}
							else
								{
								motionComp->MotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										&(mbuv[0][0]), 32, &(mbuv[4][0]));
								motionComp->DMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										&(mby[0][0]), 32, &(mby[8][0]),
										(BYTE *)&(mbuv[0][0]), fbp, fbstride);
								}
							}
						else
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[4][0]));
							motionComp->SMotionCompensationY(
									sy[0], sx[0],
									ys, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[8][0]));
							}
						}

					if (succFrame->GetYPPF(motionVerticalFieldSelect[1][1], sx[1] >> 1, sy[1] >> 1, ys))
						{
						succFrame->GetUVPPF(motionVerticalFieldSelect[1][1], csx[1] >> 1, csy[1] >> 1, uvs);

						if (directAccess)
							{
							if (needsFrameData)
								{
								motionComp->SMotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										uvc + 4 * stride, &(mbuv[4][0]), 32, &(mbuv[8][0]));
								motionComp->SDMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										yc + 8 * stride, &(mby[8][0]), 32, &(mby[16][0]),
										uvc + 4 * stride, fbp + 8 * fbstride, fbstride);
								}
							else
								{
								motionComp->MotionCompensationUV(
										csy[0], csx[0],
										uvs, stride, copyUV,
										&(mbuv[4][0]), 32, &(mbuv[8][0]));
								motionComp->DMotionCompensationY(
										sy[0], sx[0],
										ys, stride, copyY,
										&(mby[8][0]), 32, &(mby[16][0]),
										(BYTE *)&(mbuv[4][0]), fbp + 8 * fbstride, fbstride);
								}
							}
						else
							{
							motionComp->SMotionCompensationUV(
									csy[0], csx[0],
									uvs, stride, copyUV,
									uvc + 4 * stride, &(mbuv[4][0]), 32, &(mbuv[8][0]));
							motionComp->SMotionCompensationY(
									sy[0], sx[0],
									ys, stride, copyY,
									yc + 8 * stride, &(mby[8][0]), 32, &(mby[16][0]));
							}
						}
					else
						errorInSlice = true;
					}
				}
			else
				{
				px[0] = mx * 2 + vectors[0][0][0] & motionVectorMask;
				py[0] = my * 2 + vectors[0][0][1] & motionVectorMask;
				px[1] = mx * 2 + vectors[1][0][0] & motionVectorMask;
				py[1] = my * 2 + (vectors[1][0][1] + 16)  & motionVectorMask;
				cpx[0] = mx + DIV2(vectors[0][0][0]);
				cpy[0] = my + DIV2(vectors[0][0][1]);
				cpx[1] = mx + DIV2(vectors[1][0][0]);
				cpy[1] = my + DIV2(vectors[1][0][1]) + 8;

				if (predFields[motionVerticalFieldSelect[0][0]]->GetYPPF(motionVerticalFieldSelect[0][0], px[0] >> 1, py[0] >> 1, yp))
					{
					predFields[motionVerticalFieldSelect[0][0]]->GetUVPPF(motionVerticalFieldSelect[0][0], cpx[0] >> 1, cpy[0] >> 1, uvp);

					if (directAccess)
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									uvc, &(mbuv[0][0]), 32, &(mbuv[4][0]));
							motionComp->SDMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									yc, &(mby[0][0]), 32, &(mby[8][0]),
									uvc, fbp, fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									&(mbuv[0][0]), 32, &(mbuv[4][0]));
							motionComp->DMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									&(mby[0][0]), 32, &(mby[8][0]),
									(BYTE *)&(mbuv[0][0]), fbp, fbstride);
							}
						}
					else
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, stride, copyUV,
								uvc, &(mbuv[0][0]), 32, &(mbuv[4][0]));
						motionComp->SMotionCompensationY(
								py[0], px[0],
								yp, stride, copyY,
								yc, &(mby[0][0]), 32, &(mby[8][0]));
						}
					}
				else
					errorInSlice = true;

				if (predFields[motionVerticalFieldSelect[1][0]]->GetYPPF(motionVerticalFieldSelect[1][0], px[1] >> 1, py[1] >> 1, yp))
					{
					predFields[motionVerticalFieldSelect[1][0]]->GetUVPPF(motionVerticalFieldSelect[1][0], cpx[1] >> 1, cpy[1] >> 1, uvp);

					if (directAccess)
						{
						if (needsFrameData)
							{
							motionComp->SMotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									uvc + 4 * stride, &(mbuv[4][0]), 32, &(mbuv[8][0]));
							motionComp->SDMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									yc + 8 * stride, &(mby[8][0]), 32, &(mby[16][0]),
									uvc + 4 * stride, fbp + 8 * fbstride, fbstride);
							}
						else
							{
							motionComp->MotionCompensationUV(
									cpy[0], cpx[0],
									uvp, stride, copyUV,
									&(mbuv[4][0]), 32, &(mbuv[8][0]));
							motionComp->DMotionCompensationY(
									py[0], px[0],
									yp, stride, copyY,
									&(mby[8][0]), 32, &(mby[16][0]),
									(BYTE *)&(mbuv[4][0]), fbp + 8 * fbstride, fbstride);
							}
						}
					else
						{
						motionComp->SMotionCompensationUV(
								cpy[0], cpx[0],
								uvp, stride, copyUV,
								uvc + 4 * stride, &(mbuv[4][0]), 32, &(mbuv[8][0]));
						motionComp->SMotionCompensationY(
								py[0], px[0],
								yp, stride, copyY,
								yc + 8 * stride, &(mby[8][0]), 32, &(mby[16][0]));
						}
					}
				else
					errorInSlice = true;
				}
			}
		}
	}

bool QFastMMXSliceDecoder::AttemptDirectAccess(void)
	{
	if (pictureStructure == PS_FRAME_PICTURE)
		return display->AttemptDirectStripeYUVAccess(width, height, macroblockY * 16, fbp, ifbp, fbstride);
	else
		{
		if (display->AttemptDirectStripeYUVAccess(width, height, macroblockY * 32, fbp, ifbp, fbstride))
			{
			if (ifbp)
				{
				if (pictureStructure == PS_BOTTOM_FIELD) fbp = ifbp;
				ifbp = NULL;
				}
			else
				{
				if (pictureStructure == PS_BOTTOM_FIELD) fbp += fbstride;
				fbstride *= 2;
				}

			return TRUE;
			}
		else
			return FALSE;
		}
	}

void QFastMMXSliceDecoder::DoneDirectAccess(void)
	{
	display->DoneDirectStripeAccess(pictureStructure);
	}

bool QFastMMXSliceDecoder::BeginFrameDecoding(FrameStore * pred, FrameStore * succ, FrameStore * curr, PictureDisplayBuffer * display, bool hurryUp)
	{
	needsFrameData = !display ||
		               pictureCodingType != BIDIRECTIONAL_PREDICTIVE_CODED ||
							display->NeedsFrameDataWithDirectYUVAccess();

	if (GenericPackedIntraMMXSliceDecoder::BeginFrameDecoding(pred, succ, curr, display, hurryUp))
		{
		if (this->hurryUp)
			motionVectorMask = ~1;
		else
			motionVectorMask = ~0;

		if (needsFrameData)
			{
			if (pictureStructure == PS_FRAME_PICTURE)
				{
				iyuvbd[0][0] = curr->Stride();
				iyuvbd[0][1] = curr->Stride();
				iyuvbd[0][2] = curr->Stride();

				iyuvbp[0][0] = (BYTE *)0;
				iyuvbp[0][1] = (BYTE *)8;
				iyuvbp[0][2] = (BYTE *)(8 * curr->Stride());
				iyuvbp[0][3] = (BYTE *)(8 * curr->Stride() + 8);
				iyuvbp[0][4] = (BYTE *)0;
				iyuvbp[0][5] = (BYTE *)8;

				iyuvbd[1][0] = curr->Stride() * 2;
				iyuvbd[1][1] = curr->Stride();
				iyuvbd[1][2] = curr->Stride();

				iyuvbp[1][0] = (BYTE *)0;
				iyuvbp[1][1] = (BYTE *)8;
				iyuvbp[1][2] = (BYTE *)(curr->Stride());
				iyuvbp[1][3] = (BYTE *)(curr->Stride() + 8);
				iyuvbp[1][4] = (BYTE *)0;
				iyuvbp[1][5] = (BYTE *)8;
				}
			else
				{
				iyuvbd[0][0] = iyuvbd[1][0] = 2 * curr->Stride();
				iyuvbd[0][1] = iyuvbd[1][1] = 2 * curr->Stride();
				iyuvbd[0][2] = iyuvbd[1][2] = 2 * curr->Stride();

				iyuvbp[0][0] = iyuvbp[1][0] = (BYTE *)0;
				iyuvbp[0][1] = iyuvbp[1][1] = (BYTE *)8;
				iyuvbp[0][2] = iyuvbp[1][2] = (BYTE *)(16 * curr->Stride());
				iyuvbp[0][3] = iyuvbp[1][3] = (BYTE *)(16 * curr->Stride() + 8);
				iyuvbp[0][4] = iyuvbp[1][4] = (BYTE *)0;
				iyuvbp[0][5] = iyuvbp[1][5] = (BYTE *)8;
				}
			}
		else
			{
			iyuvbp[0][0] = (BYTE *)yuvp[0][0];
			iyuvbp[0][1] = (BYTE *)yuvp[0][1];
			iyuvbp[0][2] = (BYTE *)yuvp[0][2];
			iyuvbp[0][3] = (BYTE *)yuvp[0][3];
			iyuvbp[0][4] = (BYTE *)yuvp[0][4];
			iyuvbp[0][5] = (BYTE *)yuvp[0][5];

			iyuvbd[0][0] = yuvd[0][0];
			iyuvbd[0][1] = yuvd[0][1];
			iyuvbd[0][2] = yuvd[0][2];

			iyuvbp[1][0] = (BYTE *)yuvp[1][0];
			iyuvbp[1][1] = (BYTE *)yuvp[1][1];
			iyuvbp[1][2] = (BYTE *)yuvp[1][2];
			iyuvbp[1][3] = (BYTE *)yuvp[1][3];
			iyuvbp[1][4] = (BYTE *)yuvp[1][4];
			iyuvbp[1][5] = (BYTE *)yuvp[1][5];

			iyuvbd[1][0] = yuvd[1][0];
			iyuvbd[1][1] = yuvd[1][1];
			iyuvbd[1][2] = yuvd[1][2];

			iyuvbpoffset[0] = 0;
			iyuvbpoffset[1] = 0;
			iyuvbpoffset[2] = 0;
			}

		__asm {pxor	mm0, mm0}

		return TRUE;
		}
	else
		return FALSE;
	}

QFastMMXSliceDecoder::QFastMMXSliceDecoder(VideoBitStream * bitStream, GenericMMXMotionComp * motionComp, GenericMMXMacroBlockIDCTDecoder * idctDecoder, GenericMMXYUVVideoConverter	*	yuvConverter)
	: GenericPackedIntraMMXSliceDecoder(bitStream, idctDecoder)
	{
	yuvp[0][0] = &(mby[0][0]);
	yuvp[0][1] = &(mby[0][8]);
	yuvp[0][2] = &(mby[8][0]);
	yuvp[0][3] = &(mby[8][8]);
	yuvp[0][4] = &(mbuv[0][0]);
	yuvp[0][5] = &(mbuv[0][8]);

	yuvd[0][0] = 32;
	yuvd[0][1] = 32;
	yuvd[0][2] = 32;

	yuvp[1][0] = &(mby[0][0]);
	yuvp[1][1] = &(mby[0][8]);
	yuvp[1][2] = &(mby[1][0]);
	yuvp[1][3] = &(mby[1][8]);
	yuvp[1][4] = &(mbuv[0][0]);
	yuvp[1][5] = &(mbuv[0][8]);

	yuvd[1][0] = 64;
	yuvd[1][1] = 32;
	yuvd[1][2] = 32;

	if (yuvConverter)
		this->yuvConverter = yuvConverter;
	else
		this->yuvConverter = new GenericMMXYUVVideoConverter();

	if (motionComp)
		this->motionComp = motionComp;
//	else
//		this->motionComp = new FastMMXMotionComp();
	}

QFastMMXSliceDecoder::~QFastMMXSliceDecoder(void)
	{
	delete motionComp;
	delete yuvConverter;
	}

FrameStore * QFastMMXSliceDecoder::AllocFrameStore(PictureCodingType type, bool stripeOptimized)
	{
#if !_DEBUG
	if (type == BIDIRECTIONAL_PREDICTIVE_CODED && vcs == VCS_MPEG_2 && stripeOptimized)
		return new FrameStore(yuvConverter, width, height, TRUE);
	else
#endif
		return new FrameStore(yuvConverter, width, height);
	}

