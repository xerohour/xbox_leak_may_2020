/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       swizzler.cpp
 *  Content:    Xbox swizzle helper utilities
 *
 ****************************************************************************/

#if XBOX
    #include <xgrphseg.h>
    #include "xtl.h"
#else
    #include "windows.h"
    #include "d3d8-xbox.h"
#endif

#include "xgraphics.h"
#include "debug.h"

namespace XGRAPHICS
{

//returns 1 if sse instructions are supported
extern "C" 
BOOL SupportsSSE () {
	__try { __asm {
			cpuid
	} }
	__except(EXCEPTION_EXECUTE_HANDLER) {
		return FALSE;
	}

	BOOL b;

	_asm {
        mov  eax,1           // Get feature flag in edx
        cpuid
		mov eax, edx
		and eax, 02000000h
		shr eax, 25
		mov [b], eax
	}
	return b;
}

typedef __int64 QWORD;

struct XGINTERNALSWIZZLE {
    LPCVOID pSource;
    DWORD   Pitch, SLeft, STop, RWidth, RHeight;
    LPVOID  pDest;
	DWORD Width, Height, DTop, DLeft;
	int xOff, yOff, uWidth, uHeight;
	int bpp;
	Swizzler swiz;
	DWORD mask[2];
};

/*
Log2 
 
  Purpose: to calculate the log(base 2) of a number that is a power of 2

  Parameters:
    IN Value: the number to take the Log of. 

  Returns: 
    (1 << returnvalue) == Value

  Note: Value must be a power of 2, otherwise strange results will occur.
*/
#pragma warning (disable: 4035)
inline DWORD __fastcall Log2 (DWORD Value)
{ 
	__asm { bsf eax, [Value] }; 
}
#pragma warning (default: 4035)

/*
GetMasks2

  Purpose: produces the coordinate masks for manuvering through swizzled textures

  Parameters:
	IN Width: the width of the texture to be swizzled
	IN Height: the height of the texture to be swizzled
	OUT pMaskU: the mask for the u-coodinate
	OUT pMaskV: the mask for the v-coordinate

  Notes: This is a much faster algorithm for getting the masks than the 
    more generic algorithm used in the Swizzler class defined in xgraphics.h.
	This algorithm works only for 2d textures. Swizzler's works for 2d and 3d.

*/
inline VOID GetMasks2(int Width, int Height, DWORD* pMaskU, DWORD* pMaskV) 
{
	DWORD LogWidth, LogHeight, Log;

	LogWidth = Log2(Width); 
	LogHeight = Log2(Height);

	Log = min(LogWidth, LogHeight);

    DWORD LowerMask = (1 << (Log << 1)) - 1;
    DWORD UpperMask = ~LowerMask;

    DWORD MaskU = (LogWidth > LogHeight) ? (0x55555555 | UpperMask)
                                        : (0x55555555 & LowerMask);

    DWORD MaskV = (LogWidth < LogHeight) ? (0xaaaaaaaa | UpperMask)
                                        : (0xaaaaaaaa & LowerMask);

	MaskU &= ((1 << (LogWidth + LogHeight)) - 1); //we're letting u & v just loop, so
	MaskV &= ((1 << (LogWidth + LogHeight)) - 1); //we need to limit the bits to the ones we need.

	*pMaskU = MaskU;
	*pMaskV = MaskV;
}


/*
Quick-Swizzling algorithm:

  The 2d swizzling/unswizzling code grabs a rectangular block of optimized size from the texture, 
  rearranges that, moves it to the destination, then repeats with the next block.

  swizzling,                  8-bit or 16-bit
  linear:                     swizzled:
  00 01 02 03 04 05 06 07     
  10 11 12 13 14 15 16 17     
  20 21 22 23 24 25 26 27     00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \
  30 31 32 33 34 35 36 37 =>  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37 \
  40 41 42 43 44 45 46 47     40 41 50 51 42 43 52 53 60 61 70 71 62 63 72 73 \
  50 51 52 53 54 55 56 57     44 45 54 55 46 47 56 57 64 65 74 75 66 67 76 77   
  60 61 62 63 64 65 66 67     
  70 71 72 73 74 75 76 77     

  swizzling,                  32-bit
  linear:                     swizzled:
  00 01 02 03 04 05 06 07     
  10 11 12 13 14 15 16 17 =>  00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \
  20 21 22 23 24 25 26 27     04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37  
  30 31 32 33 34 35 36 37

  unswizzling, 8-bit or 16-bit
  swizzled:                                             linear:
  00 01 10 11 02 03 12 13 20 21 30 31 22 21 32 31 \     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37... => 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
  08 09 18 19 0a 0b 1a 1b 28 29 38 39 2a 2b 3a 3b \     20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
  0c 0d 1c 1d 0e 0f 1e 1f 2c 2d 3c 3d 2e 2f 3e 3f       30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f

  unswizzling, 32-bit
  swizzled:	                                            linear:
                                                        00 01 02 03 04 05 06 07
  00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \  => 10 11 12 13 14 15 16 17
  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37       20 21 22 23 24 25 26 27
                                                        30 31 32 33 34 35 36 37

  The algorithm moves through the linear texture left->right, top->bottom,
  which means the swizzled array gets 64 or 128-byte blocks written to it in a
  seemingly random order.

  AddValU and AddValV are set to move to the next block. (-32), (-64), (-128), 
  and (-256) were precalculated using the following algorithm (using 
  Swizzler class methods):

    After finishing with one block, the U coordinate must be incrimented to
	the next block. Since all blocks are 4 texels wide, the value to add will
    be constant. Swizzing (4) for use with the U coordinate results in 16. 
	(This assumes that the texture is at least 4 texels wide and 2 texels tall). 
	Then, 16 is plugged into the AddU formula from the Swizzler class: 

	  m_u = (m_u - ((-num) & m_MaskU)) & m_MaskU

	AddValU is set equal to ((-num) & m_MaskU). AddValV is calculated in a 
	similar manner. 

  This algorithm only works with textures with width and height >= block_size.
  This means that special-cases must occur when the is smaller. XBSwizzleRect
  and XBUnswizzleRect take care of this. Textures with width < 4, or height < 2 
  actually look exactly the same in memory when they are swizzled as when they 
  are deswizzled. memcpy() is used in that condition. For sizes between (2,4) and 
  (blocksize_x, blocksize_y), the texture is swizzled in 2x1 blocks.
*/

VOID swiz2d_8bit (LPCVOID pSrc, LPVOID pDest, int Width, int Height) 
{
	DWORD MaskU, MaskV, AddValU, AddValV;

	GetMasks2(Width, Height, &MaskU, &MaskV);

	AddValU = (-64) & MaskU;
	AddValV = (-128) & MaskV;

	//eax = scratch
	//ebx = u
	//ecx = y
	//edx = pitch
	//esi = pSrc 
	//edi = pDest 

	_asm {
		mov esi, pSrc;					//source + (0, 0)
		mov edi, pDest;					//dest

		mov edx, [Width];				//edx = width
		xor ebx, ebx;					//swiz(u)
		xor ecx, ecx;					//swiz(v)

		align 16						//seems to help speed a little

Start:

			  movq mm0, [esi];				//00 01 02 03 04 05 06 07
			  movq mm1, [esi + edx];		//10 11 12 13 14 15 16 17
			  add esi, edx;					//pSrc + (u, v + 1)
			  mov eax, ebx;					//eax = swiz(u)
			  movq mm4, [esi + edx];		//20 21 22 23 24 25 26 27
			  movq mm5, [esi + edx * 2];	//30 31 32 33 34 35 36 37
			  or eax, ecx;					//eax = swiz(u) | swiz(b)
			  movq mm6, mm4;				//20 21 22 23 24 25 26 27
			  movq mm2, mm0;				//00 01 02 03 04 05 06 07
			  
			  punpckhwd mm6, mm5;			//24 25 34 35 26 27 36 37

			  lea esi, [esi + edx * 4];		//pSrc + (u, v + 5)
			  punpckhwd mm2, mm1			//04 05 14 15 06 07 16 17
			  punpcklwd mm4, mm5;			//20 21 30 31 22 23 30 33

			  movq mm3, [esi];				//50 51 52 53 54 55 56 57
			  movq mm5, [esi+edx];			//60 61 62 63 64 65 66 67
			  movq mm7, [esi+edx*2];		//70 71 72 73 74 75 76 77
			  sub esi, edx					//pSrc + (u, v + 4)
			  punpcklwd mm0, mm1;			//00 01 10 11 02 03 12 13
			  movq mm1, [esi];				//40 41 42 43 44 45 46 47

			  movq [edi + eax      ], mm0;	//00 01 10 11 02 03 12 13
			  movq [edi + eax +   8], mm4;	//20 21 30 31 22 23 30 33
			  movq [edi + eax +  16], mm2;  //04 05 14 15 06 07 16 17
			  movq [edi + eax +  24], mm6;	//24 25 34 35 26 27 36 37
		  
			  movq mm0, mm1					//40 41 42 43 44 45 46 47
			  movq mm4, mm5					//60 61 62 63 64 65 66 67

			  punpcklwd mm0, mm3			//40 41 50 51 42 43 52 53
			  punpcklwd mm4, mm7			//60 61 70 71 62 63 72 73
			  punpckhwd mm1, mm3			//44 45 54 55 46 47 56 57
			  punpckhwd mm5, mm7			//64 65 74 75 66 67 76 77

			  movq [edi + eax +  32], mm0;	//40 41 50 51 42 43 52 53
			  movq [edi + eax +  40], mm4;	//60 61 70 71 62 63 72 73
			  movq [edi + eax +  48], mm1;  //44 45 54 55 46 47 56 57
			  movq [edi + eax +  56], mm5;	//64 65 74 75 66 67 76 77

			  sub esi, edx
			  sub esi, edx
			  sub esi, edx
			  sub esi, edx					//pSrc + (u, v)

			  sub ebx, [AddValU];			//part 1 of: swiz(u) += 8

			  //this number of nops seems to be optimal.
			  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
			  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
			  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
			  _asm{nop} _asm{nop} _asm{nop} _asm{nop} //_asm{nop} //_asm{nop} _asm{nop} _asm{nop}

			  add esi, 8;					//u += 8
			  and ebx, [MaskU];				//(continued): swiz(u) += 8;
			jnz Start;						//if swiz(u) hasn't looped back to 0, repeat

		  sub ecx, [AddValV];			//part 1 of: swiz(v) += 8		

		  lea esi, [esi + edx * 8]		//pSrc + (u, v + 9) //(v has been incrimented by u looping)
		  sub esi, edx;					//pSrc + (u, v + 8)
		//(it has already looped around to 1 row below where we were. This moves
		//it to the second row down, since we are dealing with 8x8 blocks)

		  and ecx, [MaskV];				//(continued): swiz(v) += 8
		  jnz Start;					//if v is not done, keep going

		emms;		//done with mmx
	}
}


VOID swiz2d_16bit (LPCVOID pSrc, LPVOID pDest, int Width, int Height) 
{
	DWORD MaskU, MaskV, AddValU, AddValV;

    if (((DWORD)pDest) & 15)
        DXGRIP("Error: Destination memory must be 16-byte aligned");


	GetMasks2(Width, Height, &MaskU, &MaskV);

	AddValU = (-64) & MaskU;
	AddValV = (-128) & MaskV;

	//eax = scratch
	//ebx = u
	//ecx = y
	//edx = pitch
	//esi = pSrc (this is not changed)
	//edi = pDest (this is incrimented)

	_asm {
		mov esi, pSrc;					
		mov edi, pDest;

		mov edx, [Width];
		xor ebx, ebx;
		xor ecx, ecx;
		add edx, edx;					//edx = width * 2

		align 16
Start:
		  movups xmm0, [esi];			
		  movups xmm1, [esi + edx];		
		  add esi, edx;
		  mov eax, ebx;					//eax = u
		  movups xmm4, [esi + edx];
		  movups xmm5, [esi + edx * 2];
		  or eax, ecx;					//eax = u | v

		  movaps xmm6, xmm4;
		  movaps xmm2, xmm0;

		  unpckhps xmm6, xmm5;

          lea esi, [esi + edx * 4]; //esi = src + width*5
		  unpckhps xmm2, xmm1		
		  unpcklps xmm4, xmm5;

		  movups xmm3, [esi]; //5
		  movups xmm5, [esi+edx]; //6;
		  movups xmm7, [esi+edx*2]; //7
		  sub esi, edx
		  unpcklps xmm0, xmm1;			
		  movups xmm1, [esi]; //4

		  movntps [edi + eax*2      ], xmm0;		
		  movntps [edi + eax*2 +  16], xmm4;
		  movntps [edi + eax*2 +  32], xmm2;  
		  movntps [edi + eax*2 +  48], xmm6;
	  
		  movaps xmm0, xmm1
		  movaps xmm4, xmm5

		  unpcklps xmm0, xmm3
		  unpcklps xmm4, xmm7
		  unpckhps xmm1, xmm3
		  unpckhps xmm5, xmm7

		  movntps [edi + eax*2 +  64], xmm0;		
		  movntps [edi + eax*2 +  80], xmm4;
		  movntps [edi + eax*2 +  96], xmm1;  
		  movntps [edi + eax*2 + 112], xmm5;

		  sub esi, edx
		  sub esi, edx
		  sub esi, edx
		  sub esi, edx

		  sub ebx, [AddValU];				//part 1 of adding 8 to u

		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} //_asm{nop} //_asm{nop} _asm{nop} _asm{nop}

		  add esi, 16;						//move source pointer to next block
		  and ebx, [MaskU];				//ebx = the next u coordinate
		jnz Start;						//if u hasn't looped back to 0, repeat

		sub ecx, [AddValV];				//part 1 of adding 8 to v		
		lea esi, [esi + edx * 8]
		sub esi, edx;					//move src pointer to next row
		//(this moves it to the second row down, since it has already looped
		//to the first row down)

		and ecx, [MaskV];				//ecx = the next v coordinate
		jnz Start;						//if v is not done, keep going
	}
}

VOID swiz2d_32bit (LPCVOID pSrc, LPVOID pDest, int Width, int Height) 
{
	DWORD MaskU, MaskV, AddValU, AddValV;

    if (((DWORD)pDest) & 15)
        DXGRIP("Error: Destination memory must be 16-byte aligned");

	GetMasks2(Width, Height, &MaskU, &MaskV);

	AddValU = (-64) & MaskU;
	AddValV = (-32) & MaskV;

	//eax = scratch
	//ebx = u
	//ecx = y
	//edx = pitch
	//esi = pSrc (this is not changed)
	//edi = pDest (this is incrimented)

	_asm {
		mov esi, pSrc;
		mov edi, pDest;

		mov edx, [Width];
		xor ebx, ebx;
		xor ecx, ecx;
		shl edx, 2;						//edx = width * 4

		align 16
Start:
			mov eax, ebx;				//eax = u

			movups xmm0, [esi];			
			movups xmm4, [esi + 16];	
			movups xmm2, [esi + edx];   
			movups xmm6, [esi + edx + 16];

			movaps xmm1, xmm0
			movaps xmm5, xmm4

			lea esi, [esi + edx * 2]
			or eax, ecx;				//eax = u | v

			movlhps xmm0, xmm2
			movhlps xmm2, xmm1
			movlhps xmm4, xmm6
			movhlps xmm6, xmm5

			movups xmm1, [esi];			
			movups xmm5, [esi + 16];	
			movups xmm3, [esi + edx];   
			movups xmm7, [esi + edx + 16];

			movntps [edi + eax*4      ], xmm0;	
			movntps [edi + eax*4 +  16], xmm2;
			movntps [edi + eax*4 +  64], xmm4;
			movntps [edi + eax*4 +  80], xmm6;

			movaps xmm0, xmm1
			movaps xmm4, xmm5

			movlhps xmm1, xmm3
			movhlps xmm3, xmm0
			movlhps xmm5, xmm7
			movhlps xmm7, xmm4

			movntps [edi + eax*4 +  32], xmm1;
			movntps [edi + eax*4 +  48], xmm3;
			movntps [edi + eax*4 +  96], xmm5;
			movntps [edi + eax*4 + 112], xmm7;

			sub esi, edx;
			sub esi, edx;

		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
		  _asm{nop} _asm{nop} _asm{nop} _asm{nop} //_asm{nop} //_asm{nop} _asm{nop} _asm{nop}



			sub ebx, [AddValU];			//part 1 of adding 8 to u
			add esi, 32;				//move source pointer to next block
		
			and ebx, [MaskU];			//part 2 of "u += 8"
			jnz Start;					//if u hasn't looped to 0, keep going

		  sub ecx, [AddValV];			//part 1 of "v += 4"

		  lea esi, [esi + edx * 4];
		  sub esi, edx;					//move source pointer to 2 rows down

		  and ecx, [MaskV];				//part 1 of "v += 4"
		  jnz Start;					//if v hasn't looped to 0, repeat
	}
}

VOID unswiz2d_8bit (LPCVOID pSrc, LPVOID pDest, int Width, int Height) 
{
	DWORD MaskU, MaskV, AddValU, AddValV;

	GetMasks2(Width, Height, &MaskU, &MaskV);

	AddValU = (-256)& MaskU;
	AddValV = (-32) & MaskV;

	//eax = scratch
	//ebx = u
	//ecx = y
	//edx = pitch
	//esi = pSrc (this is not changed)
	//edi = pDest (this is incrimented)

	_asm {
		mov esi, pSrc;
		mov edi, pDest;

		mov edx, [Width];				//edx = width
		xor ebx, ebx;
		xor ecx, ecx;

Start:
			mov eax, ebx;					//eax = u
			sub ebx, [AddValU];				//part 1 of "u += 4"
			or eax, ecx;					//eax = u | v

			pshufw mm0, [esi + eax       ], 11011000b;
			pshufw mm2, [esi + eax +64   ], 11011000b;
			pshufw mm4, [esi + eax +   16], 11011000b;
			pshufw mm6, [esi + eax +64+16], 11011000b;

			movq mm1, mm0;
			movq mm3, mm2;

			punpckldq mm0, mm4;
			punpckhdq mm1, mm4;
			punpckldq mm2, mm6;
			punpckhdq mm3, mm6;

			movq [edi], mm0;
			movq [edi+8], mm2;
			movq [edi + edx], mm1;
			movq [edi + edx + 8], mm3;

			add edi, edx;

			pshufw mm4, [esi + eax +    8], 11011000b;
			pshufw mm5, [esi + eax +64+ 8], 11011000b;
			pshufw mm6, [esi + eax +   24], 11011000b;
			pshufw mm7, [esi + eax +64+24], 11011000b;

			movq mm0, mm4;
			movq mm1, mm5;

			punpckldq mm0, mm6;
			punpckhdq mm4, mm6;
			punpckldq mm1, mm7;
			punpckhdq mm5, mm7;


			movq [edi + edx], mm0;
			movq [edi + edx + 8], mm1;
			movq [edi + edx*2], mm4;
			movq [edi + edx*2 + 8], mm5;


			sub edi, edx;
			add edi, 16;
			and ebx, [MaskU];
			jnz Start;

  		  sub ecx, [AddValV];			//part 1 of "v += 2"

		  lea edi, [edi + edx*4];
		  sub edi, edx;						//move dest to next row of blocks

		  and ecx, [MaskV];				//part 2 of "v += 2"
		  jnz Start;					//if v hasn't looped back to 0, do this row

		emms;							//done with mmx instructions
	}
}

VOID unswiz2d_16bit (LPCVOID pSrc, LPVOID pDest, int Width, int Height) 
{
	DWORD MaskU, MaskV, AddValU, AddValV;

    if (((DWORD)pSrc) & 15)
        DXGRIP("16-bit unswizzle: Source memory must be 16-byte aligned");
	if (((DWORD)pDest) & 15)
		DXGRIP("16-bit unswizzle: Destination must be 16-byte aligned");

	GetMasks2(Width, Height, &MaskU, &MaskV);

	AddValU = (-256) & MaskU;
	AddValV = (-32) & MaskV;

	//eax = scratch
	//ebx = u
	//ecx = y
	//edx = pitch
	//esi = pSrc (this is not changed)
	//edi = pDest (this is incrimented)

	_asm {
		mov esi, pSrc;
		mov edi, pDest;

		mov edx, [Width];
		xor ebx, ebx;
		xor ecx, ecx;
		add edx, edx; 

		align 16;
Start:
			mov eax, ebx;					//eax = u
			sub ebx, [AddValU];				//part 1 of "u += 16"
			or eax, ecx;					//eax = u | v

			movaps xmm0, [esi + eax*2       ];
			movaps xmm4, [esi + eax*2 +   32];
			movaps xmm2, [esi + eax*2 +  128];
			movaps xmm6, [esi + eax*2 +128+32];

			movaps xmm1, xmm0;
			movaps xmm3, xmm2;

			shufps xmm0, xmm4, 10001000b;
			shufps xmm1, xmm4, 11011101b;
			shufps xmm2, xmm6, 10001000b;
			shufps xmm3, xmm6, 11011101b;

			movaps xmm4, [esi + eax*2 +   16];
			movaps xmm6, [esi + eax*2 +   48];
			movaps xmm5, [esi + eax*2 +128+16];
			movaps xmm7, [esi + eax*2 +128+48];

			movntps [edi], xmm0;
			movntps [edi+16], xmm2;
			movntps [edi + edx], xmm1;
			movntps [edi + edx + 16], xmm3;

			add edi, edx;
			movaps xmm0, xmm4;
			movaps xmm1, xmm5;

			shufps xmm0, xmm6, 10001000b;
			shufps xmm4, xmm6, 11011101b;
			shufps xmm1, xmm7, 10001000b;
			shufps xmm5, xmm7, 11011101b;

			movntps [edi + edx], xmm0;
			movntps [edi + edx + 16], xmm1;
			movntps [edi + edx*2], xmm4;
			movntps [edi + edx*2 + 16], xmm5;


			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} //_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 

			sub edi, edx;
			add edi, 32;
			and ebx, [MaskU];				//part 2 of "u += 16"
			jnz Start;

 		  sub ecx, [AddValV];				//part 1 of "v += 4"

		  lea edi, [edi + edx*4];
		  sub edi, edx;						//move dest to next row of blocks

		  and ecx, [MaskV];					//part 2 of "v += 4"
		  jnz Start;						//if v hasn't looped to 0, do this row
	}
}

VOID unswiz2d_32bit (LPCVOID pSrc, LPVOID pDest, int Width, int Height) 
{
	DWORD MaskU, MaskV, AddValU, AddValV;

    if (((DWORD)pSrc) & 15)
        DXGRIP("32-bit unswizzle: Source memory must be 16-byte aligned");
	if (((DWORD)pDest) & 15)
		DXGRIP("32-bit unswizzle: Destination must be 16-byte aligned");



	GetMasks2(Width, Height, &MaskU, &MaskV);

	AddValU = (-64) & MaskU;
	AddValV = (-32) & MaskV;

	//eax = scratch
	//ebx = u
	//ecx = y
	//edx = pitch
	//esi = pSrc (this is not changed)
	//edi = pDest (this is incrimented)

	_asm {
		mov esi, pSrc;
		mov edi, pDest;

		mov edx, [Width];
		xor ebx, ebx;
		xor ecx, ecx;
		shl edx, 2;

Start:			
			mov eax, ebx;				//eax = u
			or eax, ecx;				//eax = u | v
			sub ebx, [AddValU];			//part 1 of "u += 8"

			movaps xmm0, [esi+eax*4];		
			movaps xmm2, [esi+eax*4+16];
			movaps xmm4, [esi+eax*4+64];
			movaps xmm6, [esi+eax*4+80];

			movaps xmm1, xmm0;
			movaps xmm5, xmm4;

			movlhps xmm0, xmm2;
			movhlps xmm2, xmm1;
			movlhps xmm4, xmm6;
			movhlps xmm6, xmm5;

			movaps xmm1, [esi+eax*4+32];
			movaps xmm3, [esi+eax*4+48];
			movaps xmm5, [esi+eax*4+96];
			movaps xmm7, [esi+eax*4+112];

			movntps [edi], xmm0;
			movntps [edi+16], xmm4;
			movntps [edi+edx], xmm2;
			movntps [edi+edx+16], xmm6;

			add edi, edx;
			movaps xmm0, xmm1;
			movaps xmm4, xmm5;

			movlhps xmm1, xmm3;
			movhlps xmm3, xmm0;
			movlhps xmm5, xmm7;
			movhlps xmm7, xmm4;

			movntps [edi+edx], xmm1;
			movntps [edi+edx+16], xmm5;
			movntps [edi+edx*2], xmm3;
			movntps [edi+edx*2+16], xmm7;

			sub edi, edx;

			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
			_asm {nop} _asm {nop} _asm {nop} _asm {nop} //_asm {nop} _asm {nop} _asm {nop} _asm {nop} 

			add edi, 32;				//move dest ptr to next block
				
			and ebx, [MaskU];			//part 2 of "u += 8"
			jnz Start;					//if u hasn't looped to 0, repeat for next block
	
		  sub ecx, [AddValV];			//part 1 of "v += 4"

		  lea edi, [edi + edx*4];
		  sub edi, edx;					//move dst ptr to next row

		  and ecx, [MaskV];				//part 2 of "v += 4"
		  jnz Start;					//if v hasn't looped to 0, repeat for next row
	}
}


//----------------------------------------------------------------------------
// Determine if a D3DFORMAT is a swizzled one
//
extern "C"
BOOL WINAPI XGIsSwizzledFormat(
    D3DFORMAT Format
    )
{
    switch(Format)
    {
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_A8:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_B8G8R8A8:
    case D3DFMT_R4G4B4A4:
    case D3DFMT_R5G5B5A1:
    case D3DFMT_R8G8B8A8:
    case D3DFMT_R8B8:
    case D3DFMT_G8B8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_A8L8:
    case D3DFMT_AL8:
    case D3DFMT_L16:
    case D3DFMT_L6V5U5:
    case D3DFMT_V16U16:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D24S8:
    case D3DFMT_F16:
    case D3DFMT_F24S8:
        return TRUE;
    }
    return FALSE;
}

//----------------------------------------------------------------------------
// Helper to get bytes per pixel from a D3DFORMAT
//
extern "C"
DWORD WINAPI XGBytesPerPixelFromFormat(
    D3DFORMAT Format
    )
{
    switch (Format)
    {
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_B8G8R8A8:
    case D3DFMT_R8G8B8A8:
    case D3DFMT_V16U16:
    case D3DFMT_D24S8:
    case D3DFMT_F24S8:
    case D3DFMT_UYVY:
    case D3DFMT_YUY2:
    case D3DFMT_LIN_A8B8G8R8:
    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_B8G8R8A8:
    case D3DFMT_LIN_R8G8B8A8:
    case D3DFMT_LIN_X8R8G8B8:
    case D3DFMT_LIN_D24S8:
    case D3DFMT_LIN_F24S8:
        return 4;

    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_R4G4B4A4:
    case D3DFMT_R5G5B5A1:
    case D3DFMT_R8B8:
    case D3DFMT_G8B8:
    case D3DFMT_A8L8:
    case D3DFMT_L16:
    case D3DFMT_L6V5U5:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_F16:
    case D3DFMT_LIN_A1R5G5B5:
    case D3DFMT_LIN_A4R4G4B4:
    case D3DFMT_LIN_G8B8:
    case D3DFMT_LIN_R4G4B4A4:
    case D3DFMT_LIN_R5G5B5A1:
    case D3DFMT_LIN_R5G6B5:
    case D3DFMT_LIN_R6G5B5:
    case D3DFMT_LIN_R8B8:
    case D3DFMT_LIN_X1R5G5B5:
    case D3DFMT_LIN_A8L8:
    case D3DFMT_LIN_L16:
    case D3DFMT_LIN_D16:
    case D3DFMT_LIN_F16:
        return 2;

    case D3DFMT_DXT2:
    case D3DFMT_DXT4:
    case D3DFMT_A8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_AL8:
    case D3DFMT_LIN_A8:
    case D3DFMT_LIN_AL8:
    case D3DFMT_LIN_L8:
        return 1;
    }

    return 0;
}

template <class T>
class XGInternalSwizzler {
	public:
	//for swizzling 1x* columns
    static VOID Swizzle1x1Column (XGINTERNALSWIZZLE* ps)
	{    
		SWIZNUM SwizU = ps->swiz.SwizzleU(ps->DLeft + ps->xOff);
		SWIZNUM SwizV = ps->swiz.SwizzleV(ps->DTop + ps->yOff);

	    ps->swiz.SetV(SwizV);
		ps->swiz.SetU(0);

		unsigned int v;
//        DWORD PitchAdjust = Pitch - RWidth * sizeof(T);

		T* src = (T*)((BYTE*)ps->pSource + (ps->Pitch * (ps->STop + ps->yOff)) + (sizeof(T) * (ps->SLeft + ps->xOff)));
		T* dest = &((T*)ps->pDest)[SwizU];

        for (v = ps->uHeight; v--;) 
        {
            dest[ps->swiz.m_v] = *src;
            ps->swiz.IncV();
            src = (T*)((BYTE *)src + ps->Pitch);
        }
	}

    static VOID Unswizzle1x1Column (XGINTERNALSWIZZLE* ps)
	{
		SWIZNUM SwizU = ps->swiz.SwizzleU(ps->SLeft + ps->xOff);
		SWIZNUM SwizV = ps->swiz.SwizzleV(ps->STop + ps->yOff);
		ps->swiz.SetV(SwizV);
		ps->swiz.SetU(0);
		unsigned int v;

		T* src = &((T*)ps->pSource)[SwizU];
		T* dest = (T*)((BYTE*)ps->pDest + (ps->Pitch * (ps->DTop + ps->yOff)) + (sizeof(T) * (ps->DLeft + ps->xOff)));

		for (v = ps->uHeight; v--;) 
		{
			*dest = src[ps->swiz.m_v];
			ps->swiz.IncV();
			dest = (T*)((BYTE*)dest + ps->Pitch);
		}
	}

	//note: call this with WORD, DWORD, and QWORD, instead of BYTE, WORD, DWORD.
	//also, only call this if the SLeft, STop, RWidth, and RHeight are multiples of 2.

    static VOID Swizzle2x1 (XGINTERNALSWIZZLE* ps)
	{	
		ps->mask[0] = ps->swiz.m_MaskU;
		ps->mask[1] = ps->swiz.m_MaskV;

		SWIZNUM SwizU = ps->swiz.SwizzleU(ps->DLeft + ps->xOff);
		SWIZNUM SwizV = ps->swiz.SwizzleV(ps->DTop + ps->yOff);
		
		ps->swiz.m_MaskU >>= 1;  ps->swiz.m_MaskV >>= 1;
		SwizU >>= 1;             SwizV >>= 1;

		unsigned int u, v, i;
        DWORD PitchAdjust = ps->Pitch - ps->uWidth * (sizeof(T) / 2);

		T* src = (T*)((BYTE*)ps->pSource + (ps->Pitch * (ps->STop + ps->yOff)) + ((sizeof(T) / 2) * (ps->SLeft + ps->xOff)));
		T* dest = (T*)ps->pDest;

	    ps->swiz.SetV(SwizV);

		for (v = ps->uHeight; v--;) 
        {
            ps->swiz.SetU(SwizU);

            for (u = ps->uWidth >> 1; u--;) 
            {
                dest[ps->swiz.Get2D()] = *(src++);
                ps->swiz.IncU();
            }

            ps->swiz.IncV();
            src = (T*)((BYTE *)src + PitchAdjust);
        }
		ps->swiz.m_MaskU = ps->mask[0];
		ps->swiz.m_MaskV = ps->mask[1];
	}

    static VOID Unswizzle2x1 (XGINTERNALSWIZZLE* ps)
	{
		ps->mask[0] = ps->swiz.m_MaskU;
		ps->mask[1] = ps->swiz.m_MaskV;

		SWIZNUM SwizU = ps->swiz.SwizzleU(ps->SLeft + ps->xOff);
		SWIZNUM SwizV = ps->swiz.SwizzleV(ps->STop + ps->yOff);
		ps->swiz.m_MaskU >>= 1;  ps->swiz.m_MaskV >>= 1;
		SwizU >>= 1;             SwizV >>= 1;

		unsigned int u,v,i;
		DWORD PitchAdjust = ps->Pitch - ps->uWidth * (sizeof(T) / 2);

		T* src = (T*)ps->pSource;
		T* dest = (T*)((BYTE*)ps->pDest + (ps->Pitch * (ps->DTop + ps->yOff)) + ((sizeof(T) / 2) * (ps->DLeft + ps->xOff)));
		
		ps->swiz.SetV(SwizV);
		for(v = ps->uHeight; v--;)
		{
			ps->swiz.SetU(SwizU);
			for(u = ps->uWidth >> 1; u--;)
			{
				*(dest++) = src[ps->swiz.Get2D()];
				ps->swiz.IncU();
			}
			ps->swiz.IncV();
			dest = (T*)((BYTE*)dest + PitchAdjust);
		}

		ps->swiz.m_MaskU = ps->mask[0];
		ps->swiz.m_MaskV = ps->mask[1];
	}
};

__forceinline VOID XGInternalSwizzle1x1Column(XGINTERNALSWIZZLE* ps)
{
	if(ps->bpp == 4) {
		XGInternalSwizzler<DWORD>::Swizzle1x1Column(ps);
	} else if (ps->bpp == 2) {
		XGInternalSwizzler<WORD>::Swizzle1x1Column(ps);
	} else {
		XGInternalSwizzler<BYTE>::Swizzle1x1Column(ps);
	}
}
__forceinline VOID XGInternalSwizzle2x1(XGINTERNALSWIZZLE* ps)
{
	if(ps->bpp == 4) {
		XGInternalSwizzler<QWORD>::Swizzle2x1(ps);
	} else if (ps->bpp == 2) {
		XGInternalSwizzler<DWORD>::Swizzle2x1(ps);
	} else {
		XGInternalSwizzler<WORD>::Swizzle2x1(ps);
	}
}
__forceinline VOID XGInternalUnswizzle1x1Column(XGINTERNALSWIZZLE* ps)
{
	if(ps->bpp == 4) {
		XGInternalSwizzler<DWORD>::Unswizzle1x1Column(ps);
	} else if (ps->bpp == 2) {
		XGInternalSwizzler<WORD>::Unswizzle1x1Column(ps);
	} else {
		XGInternalSwizzler<BYTE>::Unswizzle1x1Column(ps);
	}
}
__forceinline VOID XGInternalUnswizzle2x1(XGINTERNALSWIZZLE* ps)
{
	if(ps->bpp == 4) {
		XGInternalSwizzler<QWORD>::Unswizzle2x1(ps);
	} else if (ps->bpp == 2) {
		XGInternalSwizzler<DWORD>::Unswizzle2x1(ps);
	} else {
		XGInternalSwizzler<WORD>::Unswizzle2x1(ps);
	}
}



//----------------------------------------------------------------------------
// Swizzle a subrectangle from a buffer into a larger texture.  The 
// destination rectangle must be completely contained within the destination 
// texture (no clipping).
//
// If pRect is NULL, pPoint is NULL and Pitch == 0, this routine will
// assume that the source buffer is exactly the same size as the destination
// texture and will swizzle the whole thing.  This routine will run
// considerably faster in that case.
//
extern "C"
VOID WINAPI XGSwizzleRect(
    LPCVOID pSource,      // The buffer that contains the source rectangle
    DWORD   Pitch,        // The pitch of the buffer that contains the source
    LPCRECT pRect,        // The rectangle within the buffer to copy.
    LPVOID  pDest,        // The destination texture.
    DWORD   Width,        // The width of the entire destination texture.
    DWORD   Height,       // The height of the entire destination texture.
    CONST LPPOINT pPoint, // Where to put the rectangle in the texture.
    DWORD   BytesPerPixel
    )
{
#ifndef XBOX
	//if SSE instructions are not supported, XGSwizzleBox with depth==1 does the same thing as 
	  //XGSwizzleRect, but doesn't use pentium-3-specific instructions
	if (!SupportsSSE()) {
		D3DBOX Box, *pBox = &Box;
		XGPOINT3D Point3, *pPoint3 = &Point3;
		if(!pRect) {
			pBox = NULL;
		} else {
			Box.Left = pRect->left;
			Box.Right = pRect->right;
			Box.Top = pRect->top;
			Box.Bottom = pRect->bottom;
			Box.Front = 0;
			Box.Back = 1;
		}

		if(!pPoint) {
			pPoint3 = NULL;
		} else {
			Point3.u = pPoint->x;
			Point3.v = pPoint->y;
			Point3.w = 0;
		}

		XGSwizzleBox(pSource, Pitch, Pitch * Width, pBox, pDest, Width, Height, 1, pPoint3, BytesPerPixel);
		return;
	}
#endif

	unsigned int MinBlockWidth[3]  = {16,16, 8};
	unsigned int MinBlockHeight[3] = { 8, 8, 8};

	unsigned int WidthMasks[3]     = { 7, 7, 7};
	unsigned int HeightMasks[3]    = { 7, 7, 3};

    if (DBG_CHECK(1))
    {
		if (((Width - 1) & Width) || ((Height - 1) & Height))
        {
			DXGRIP("XGSwizzleRect - Width and Height must be power of 2!");
        }
		if ((Width < 1) || (Width > 4096) || (Height < 1) || (Height > 4096))
        {
			DXGRIP("XGSwizzleRect - Width and Height must be between 1 and 4096");
        }
		if ((BytesPerPixel != 4) && (BytesPerPixel != 2) && (BytesPerPixel != 1)) 
        {
			DXGRIP("XGSwizzleRect - BytesPerPixel must be 1, 2, or 4!");
        }
    }

	//tiny textures
	if (Width <= 2 || Height <= 1) 
	{		
		memcpy(pDest, pSource, Width * Height * BytesPerPixel);
		return;
	} 

	//full-texture swizzles
	if (!pRect && !pPoint && Pitch == 0)
	{
		if (BytesPerPixel == 4) 
		{
			if(Width >= MinBlockWidth[2] && Height >= MinBlockHeight[2]) {
				//32-bit swizzler does 8x4 blocks
				swiz2d_32bit(pSource, pDest, Width, Height);
				return;
			} 
		} 
		else if (BytesPerPixel == 2) 
		{
			if(Width >= MinBlockWidth[1] && Height >= MinBlockHeight[1]) {
				//16-bit swizzler does 8x8 blocks
				swiz2d_16bit(pSource, pDest, Width, Height);
				return;
			} 
		}
		else 
		{
			if(Width >= MinBlockWidth[0] && Height >= MinBlockHeight[0]) {
				//8-bit swizzler does 8x8 blocks
				swiz2d_8bit(pSource, pDest, Width, Height);
				return;
			} 
		}

		//too small for the full-texture swizzler. Use the smaller swizzlers instead.
	} 
	else if (DBG_CHECK(1)) 
	{
        if (!pRect || !pPoint || Pitch == 0)
        {
//            DXGRIP("XGSwizzleRect - Either all of pRect, pPoint and Pitch == 0 or none.");
        }
	}

	XGINTERNALSWIZZLE s;
	s.bpp = BytesPerPixel;
	s.Height = Height;
	s.Width = Width;
	if(pPoint == NULL) {
		s.DLeft = 0;
		s.DTop = 0;
	} else {
		s.DLeft = pPoint->x;
		s.DTop = pPoint->y;
	}
	s.pDest = pDest;
	s.pSource = pSource;
	if(pRect == NULL) {
		s.RHeight = Height;
		s.RWidth = Width;
		s.SLeft = 0;
		s.STop = 0;
	} else {
		s.RHeight = pRect->bottom - pRect->top;
		s.RWidth = pRect->right - pRect->left;
		s.SLeft = pRect->left;
		s.STop = pRect->top;
	}
	if(Pitch == 0) {
		s.Pitch = s.Width * s.bpp;
	} else {
		s.Pitch = Pitch;
	}
	s.swiz.Init(Width, Height, 0);
	s.mask[0] = s.swiz.m_MaskU;
	s.mask[1] = s.swiz.m_MaskV;

 	if (DBG_CHECK(1))
    {
        if (s.DLeft + s.RWidth > Width
            || s.DTop + s.RHeight > Height)
        {
            DXGRIP("XGSwizzleRect - The swizzled rectangle must fit completely inside of the destination texture, this routine does no clipping.");
        }

        if (s.Pitch < s.RWidth * BytesPerPixel)
        {
            DXGRIP("XGSwizzleRect - The pitch of the source rectangle must be at least as big as the rectangle width * BytesPerPixel.");
        }
    }

	if(!s.RHeight || !s.RWidth) return; //width == 0, or height == 0

	DWORD WMask = WidthMasks [(BytesPerPixel >> 1)]; //7,7,7
	DWORD HMask = HeightMasks[(BytesPerPixel >> 1)]; //7,7,3
	
	//if the left edge isn't 2-pixel aligned, deal with that first.
	if(s.DLeft & 1) {
		s.xOff = 0;  s.yOff = 0;
		s.uWidth = 1; s.uHeight = s.RHeight;
		XGInternalSwizzle1x1Column(&s);
		++s.SLeft; 
		++s.DLeft;
		if(!--s.RWidth) return; //if width == 0, we're done
	}

	//if the right edge isn't 2-pixel aligned, do that too...
	if(s.RWidth & 1) {
		s.RWidth--;
		s.xOff = s.RWidth;   s.yOff = 0;
		s.uWidth = 1;
		s.uHeight = s.RHeight;
		XGInternalSwizzle1x1Column(&s);
		if(!s.RWidth) return; //if width == 0, we're done.
	}

	//if there aren't going to be any 8x8's, just fill the rest using 2x1s.
	//if left edge (aligned right) is >= right edge (aligned left), there is no opportunity for 8x8.
	//if top edge, (aligned downward) is lower than bottom edge (aligned upward), there is no opportunity for 8x8.
	if((((s.DLeft + WMask) & ~WMask) >= ((s.DLeft + s.RWidth ) & ~WMask)) || 
	   (((s.DTop  + HMask) & ~HMask) >= ((s.DTop  + s.RHeight) & ~HMask)) ||
	   ((s.Width < MinBlockWidth[(BytesPerPixel >> 1)])) ||
	   ((s.Height < MinBlockHeight[(BytesPerPixel >> 1)]))) {
		s.xOff = 0;              s.yOff = 0;
		s.uHeight = s.RHeight;   s.uWidth = s.RWidth;
		XGInternalSwizzle2x1(&s);
		return;
	}

	//if the top isn't 8-pixel aligned, deal with that.
	if(s.DTop & HMask) {
		DWORD h = (0-s.DTop) & HMask;
		s.xOff = 0;           s.yOff = 0;
		s.uWidth = s.RWidth;  s.uHeight = h;
		XGInternalSwizzle2x1(&s);
		s.STop += h;
		s.DTop += h;
		s.RHeight -= h;
	}

	//if the bottom isn't 8-pixel aligned, deal with that.
	if(s.RHeight & HMask) {
		DWORD h = (s.RHeight & HMask);
		s.RHeight -= h;
		s.xOff = 0; 	       s.yOff = s.RHeight;
		s.uWidth = s.RWidth;   s.uHeight = h;
		XGInternalSwizzle2x1(&s);
	}

	//if the left isn't 8-pixel aligned, do that...
	if(s.DLeft & WMask) {
		DWORD w = (0-s.DLeft) & WMask;
		s.xOff = 0;     s.yOff = 0;
		s.uWidth = w;   s.uHeight = s.RHeight;
		XGInternalSwizzle2x1(&s);
		s.DLeft += w;
		s.SLeft += w;
		s.RWidth -= w;
	}

	//if the right isn't 8-pixel aligned, do that too.
	if(s.RWidth & WMask) {
		DWORD w = (s.RWidth & WMask);
		s.RWidth -= w;
		s.xOff = s.RWidth;  s.yOff = 0;
		s.uWidth = w;       s.uHeight = s.RHeight; 
		XGInternalSwizzle2x1(&s);
	}

	//now, do the 8x8's!
	DWORD AddValU, AddValV;
	DWORD StartU,  StartV;
	DWORD EsiStore;
	DWORD WidthStore;
	
	if(s.bpp == 4) {
		AddValU = (-64) & s.swiz.m_MaskU;
		AddValV = (-32) &  s.swiz.m_MaskV;
		StartU = s.swiz.SwizzleU(s.DLeft);
		StartV = s.swiz.SwizzleV(s.DTop);
		s.pSource = ((BYTE*)s.pSource) + (s.SLeft * s.bpp) + (s.STop * s.Pitch);
		s.RWidth  >>= 3; // width /= 8;
		s.RHeight >>= 2; //height /= 4;
		WidthStore = s.RWidth;
		if((DWORD)(s.pDest) & 15) {
			DXGRIP("XBSwizzleRect: pDest must be 16-byte aligned");
		}

		
		_asm {
			mov esi, [s.pSource]
			mov edi, [s.pDest]
			mov edx, [s.Width]
			shl edx, 2 //size of each destination row
			mov ebx, [StartU]
			mov ecx, [StartV]

			mov EsiStore, esi;
			align 16;

Start32:	
				mov eax, ebx
				movups xmm0, [esi];
				movups xmm4, [esi + 16];
				movups xmm2, [esi + edx];
				movups xmm6, [esi + edx + 16];

				movaps xmm1, xmm0;
				movaps xmm5, xmm4;

				lea esi, [esi + edx * 2];
				or eax, ecx;

				movlhps xmm0, xmm2
				movhlps xmm2, xmm1
				movlhps xmm4, xmm6
				movhlps xmm6, xmm5

				movups xmm1, [esi];			
				movups xmm5, [esi + 16];	
				movups xmm3, [esi + edx];   
				movups xmm7, [esi + edx + 16];

				movntps [edi + eax*4      ], xmm0;	//dest[u | v] = {a,b,c,d}
				movntps [edi + eax*4 +  16], xmm2;
				movntps [edi + eax*4 +  64], xmm4;
				movntps [edi + eax*4 +  80], xmm6;

				movaps xmm0, xmm1
				movaps xmm4, xmm5

				movlhps xmm1, xmm3
				movhlps xmm3, xmm0
				movlhps xmm5, xmm7
				movhlps xmm7, xmm4

				movntps [edi + eax*4 +  32], xmm1;
				movntps [edi + eax*4 +  48], xmm3;
				movntps [edi + eax*4 +  96], xmm5;
				movntps [edi + eax*4 + 112], xmm7;

				sub esi, edx;
				sub esi, edx;

				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} //_asm{nop} //_asm{nop} _asm{nop} _asm{nop}

				sub ebx, [AddValU];
				add esi, 32;
				and ebx, [s.swiz.m_MaskU];
				
				dec [s.RWidth];
 			  jnz Start32;

			  mov esi, [WidthStore];
			  mov ebx, [StartU];
			  mov [s.RWidth], esi;
			  mov esi, [EsiStore];
			  mov eax, [s.Pitch];
			  sub ecx, [AddValV];
		      lea esi, [esi + eax * 4];
			  and ecx, [s.swiz.m_MaskV];
			  mov [EsiStore], esi;

			  dec [s.RHeight];
			jnz Start32;
		}
	} else if ( s.bpp == 2) {
		AddValU = (-64) & s.swiz.m_MaskU;
		AddValV = (-128) &  s.swiz.m_MaskV;
		StartU = s.swiz.SwizzleU(s.DLeft);
		StartV = s.swiz.SwizzleV(s.DTop);
		s.pSource = ((BYTE*)s.pSource) + (s.SLeft * s.bpp) + (s.STop * s.Pitch);
		s.RWidth  >>= 3; // width /= 8;
		s.RHeight >>= 3; //height /= 8;
		WidthStore = s.RWidth;
		if((DWORD)(s.pDest) & 15) {
			DXGRIP("XBSwizzleRect: pDest must be 16-byte aligned");
		}

		_asm {
			mov esi, [s.pSource]
			mov edi, [s.pDest]
			mov edx, [s.Width]
			shl edx, 1 //size of each destination row
			mov ebx, [StartU]
			mov ecx, [StartV]

			mov EsiStore, esi;
			align 16;
Start16:
				movups xmm0, [esi];				//xmm0 = {a, b, e, f, ?, ?, ?, ?}
				movups xmm1, [esi + edx];		//xmm1 = {c, d, g, h, ?, ?, ?, ?}
				add esi, edx;
				mov eax, ebx;					//eax = u
				movups xmm4, [esi + edx];
				movups xmm5, [esi + edx * 2];
				or eax, ecx;					//eax = u | v
				movaps xmm2, xmm0;
				movaps xmm6, xmm4;
				unpckhps xmm6, xmm5;

				lea esi, [esi + edx * 4]; //esi = src + width*5
				unpckhps xmm2, xmm1		
				unpcklps xmm4, xmm5;

				movups xmm3, [esi]; //5
				movups xmm5, [esi+edx]; //6;
				movups xmm7, [esi+edx*2]; //7
				sub esi, edx
				unpcklps xmm0, xmm1;			//xmm1 = {a, b, c, d, e, f, g, h}
				movups xmm1, [esi]; //4

				movntps [edi + eax*2      ], xmm0;		//dest[u | v] = {a, b, c, d, e, f, g, h}
				movntps [edi + eax*2 +  16], xmm4;
				movntps [edi + eax*2 +  32], xmm2;  
				movntps [edi + eax*2 +  48], xmm6;

				movaps xmm0, xmm1
				movaps xmm4, xmm5

				unpcklps xmm0, xmm3
				unpcklps xmm4, xmm7
				unpckhps xmm1, xmm3
				unpckhps xmm5, xmm7

				movntps [edi + eax*2 +  64], xmm0;		//dest[u | v] = {a, b, c, d, e, f, g, h}
				movntps [edi + eax*2 +  80], xmm4;
				movntps [edi + eax*2 +  96], xmm1;  
				movntps [edi + eax*2 + 112], xmm5;

				sub esi, edx
				sub esi, edx
				sub esi, edx
				sub esi, edx

				sub ebx, [AddValU];				//part 1 of adding 4 to u

				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} //_asm{nop} //_asm{nop} _asm{nop} _asm{nop}

				add esi, 16;						//move source pointer to next block
				and ebx, [s.swiz.m_MaskU];				//ebx = the next u coordinate
				dec s.RWidth;
			  jnz Start16;

			  mov esi, [WidthStore];
			  mov ebx, [StartU];
			  mov [s.RWidth], esi;
			  mov esi, [EsiStore];
			  mov eax, [s.Pitch];
			  sub ecx, [AddValV];
		      lea esi, [esi + eax * 8];
			  and ecx, [s.swiz.m_MaskV];
			  mov [EsiStore], esi;

			  dec [s.RHeight];
			jnz Start16;
		}
	} else {
		AddValU = (-64) & s.swiz.m_MaskU;
		AddValV = (-128) &  s.swiz.m_MaskV;
		StartU = s.swiz.SwizzleU(s.DLeft);
		StartV = s.swiz.SwizzleV(s.DTop);
		s.pSource = ((BYTE*)s.pSource) + (s.SLeft * s.bpp) + (s.STop * s.Pitch);
		s.RWidth  >>= 3; // width /= 8;
		s.RHeight >>= 3; //height /= 8;
		WidthStore = s.RWidth;

		_asm {
			mov esi, [s.pSource]
			mov edi, [s.pDest]
			mov edx, [s.Width]
			mov ebx, [StartU]
			mov ecx, [StartV]

			mov EsiStore, esi;
			align 16;

Start8:
				movq mm0, [esi];				//xmm0 = {a, b, e, f, ?, ?, ?, ?}
				movq mm1, [esi + edx];		//xmm1 = {c, d, g, h, ?, ?, ?, ?}
				add esi, edx;
				mov eax, ebx;					//eax = u
				movq mm4, [esi + edx];
				movq mm5, [esi + edx * 2];
				or eax, ecx;					//eax = u | v
				movq mm2, mm0;
				movq mm6, mm4;
				punpckhwd mm6, mm5;

				lea esi, [esi + edx * 4]; //esi = src + width*5
				punpckhwd mm2, mm1		
				punpcklwd mm4, mm5;

				movq mm3, [esi]; //5
				movq mm5, [esi+edx]; //6;
				movq mm7, [esi+edx*2]; //7
				sub esi, edx
				punpcklwd mm0, mm1;			//xmm1 = {a, b, c, d, e, f, g, h}
				movq mm1, [esi]; //4

				movq [edi + eax      ], mm0;		//dest[u | v] = {a, b, c, d, e, f, g, h}
				movq [edi + eax +   8], mm4;
				movq [edi + eax +  16], mm2;  
				movq [edi + eax +  24], mm6;

				movq mm0, mm1
				movq mm4, mm5
				punpcklwd mm0, mm3
				punpcklwd mm4, mm7
				punpckhwd mm1, mm3
				punpckhwd mm5, mm7
				movq [edi + eax +  32], mm0;		//dest[u | v] = {a, b, c, d, e, f, g, h}
				movq [edi + eax +  40], mm4;
				movq [edi + eax +  48], mm1;  
				movq [edi + eax +  56], mm5;

				sub esi, edx
				sub esi, edx
				sub esi, edx
				sub esi, edx

				sub ebx, [AddValU];				//part 1 of adding 4 to u

				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop} _asm{nop}
				_asm{nop} _asm{nop} _asm{nop} _asm{nop} //_asm{nop} //_asm{nop} _asm{nop} _asm{nop}

				add esi, 8;						//move source pointer to next block
				and ebx, [s.swiz.m_MaskU];				//ebx = the next u coordinate
				dec s.RWidth;
			  jnz Start8;

			  mov eax, [WidthStore];
			  mov ebx, [StartU];
			  mov [s.RWidth], eax;
			  mov esi, [EsiStore];
			  mov eax, [s.Pitch];
			  sub ecx, [AddValV];
		      lea esi, [esi + eax * 8];
			  and ecx, [s.swiz.m_MaskV];
			  mov [EsiStore], esi;

			  dec [s.RHeight];
			jnz Start8;
			emms 
		}

	}

	return;

}

//----------------------------------------------------------------------------
// Unswizzle a subrectangle from a texture into a buffer.
//
// If pRect is NULL, pPoint is NULL and Pitch == 0, this routine will
// assume that the source texture is exactly the same size as the destination
// buffer and will unswizzle the whole thing.  This routine will run
// considerably faster in that case.
//
extern "C"
VOID WINAPI XGUnswizzleRect(
    LPCVOID pSource,      // The source texture.
    DWORD   Width,        // The width of the entire source texture.
    DWORD   Height,       // The height of the entire source texture.
    LPCRECT pRect,        // The rectangle within the texture to copy.
    LPVOID  pDest,        // The destination buffer
    DWORD   Pitch,        // The pitch of the destination buffer
    CONST LPPOINT pPoint, // Where to copy the rectangle to
    DWORD   BytesPerPixel
    )
{
#ifndef XBOX
	//if SSE instructions are not supported, XGUnswizzleBox with depth==1 does the same thing as 
	  //XGUnswizzleRect, but doesn't use pentium-3-specific instructions
	if (!SupportsSSE()) {
		D3DBOX Box, *pBox = &Box;
		XGPOINT3D Point3, *pPoint3 = &Point3;
		if(!pRect) {
			pBox = NULL;
		} else {
			Box.Left = pRect->left;
			Box.Right = pRect->right;
			Box.Top = pRect->top;
			Box.Bottom = pRect->bottom;
			Box.Front = 0;
			Box.Back = 1;
		}

		if(!pPoint) {
			pPoint3 = NULL;
		} else {
			Point3.u = pPoint->x;
			Point3.v = pPoint->y;
			Point3.w = 0;
		}

		XGUnswizzleBox(pSource, Width, Height, 1, pBox, pDest, Pitch, Pitch * Height, pPoint3, BytesPerPixel);
		return;
	}
#endif

	unsigned int MinBlockWidth[3] =  {16,16,8};
	unsigned int MinBlockHeight[3] = {16,16,8}; //height isn't actually 16, but is required to be at least 8.
	unsigned int WidthMasks[3] =  {15,15,7}; //used in aligning block-unswizzles
	unsigned int HeightMasks[3] = {3, 3, 3}; //used in aligning for block-unswizzles

    if (DBG_CHECK(1))
    {
		if (((Width - 1) & Width) || ((Height - 1) & Height))
        {
			DXGRIP("XGSwizzleRect - Width and Height must be power of 2!");
        }
		if ((Width < 1) || (Width > 4096) || (Height < 1) || (Height > 4096))
        {
			DXGRIP("XGSwizzleRect - Width and Height must be between 1 and 4096");
        }
		if ((BytesPerPixel != 4) && (BytesPerPixel != 2) && (BytesPerPixel != 1)) 
        {
			DXGRIP("XGSwizzleRect - BytesPerPixel must be 1, 2, or 4!");
        }
    }

	//tiny textures
	if (Width <= 2 || Height <= 1) 
	{		
		memcpy(pDest, pSource, Width * Height * BytesPerPixel);
		return;
	} 

	//full-texture swizzles
	if (!pRect && !pPoint && Pitch == 0)
	{
		if (BytesPerPixel == 4) 
		{
			if(Width >= MinBlockWidth[2] && Height >= MinBlockHeight[2]) {
				//32-bit unswizzler does 8x4 blocks
				unswiz2d_32bit(pSource, pDest, Width, Height);
				return;
			} 
		} 
		else if (BytesPerPixel == 2) 
		{
			if(Width >= MinBlockWidth[1] && Height >= MinBlockHeight[1]) {
				//16-bit unswizzler does 16x4 blocks
				unswiz2d_16bit(pSource, pDest, Width, Height);
				return;
			} 
		}
		else 
		{
			if(Width >= MinBlockWidth[0] && Height >= MinBlockHeight[0]) {
				//8-bit unswizzler does 16x4 blocks
				unswiz2d_8bit(pSource, pDest, Width, Height);
				return;
			} 
		}

		//too small for the full-texture unswizzler. Use the smaller swizzlers instead.
	} 
	else if (DBG_CHECK(1)) 
	{
        if (!pRect || !pPoint || Pitch == 0)
        {
//            DXGRIP("XGUnwizzleRect - Either all of pRect, pPoint and Pitch == 0 or none.");
        }
	}

	XGINTERNALSWIZZLE s;
	s.bpp = BytesPerPixel;
	s.Height = Height;
	s.Width = Width;
	if(pPoint == NULL) {
		s.DLeft = 0;
		s.DTop = 0;
	} else { 
		s.DLeft = pPoint->x;
		s.DTop = pPoint->y;
	}
	s.pDest = pDest;
	s.pSource = pSource;
	if(pRect == NULL) {
		s.RHeight = Height; 
		s.RWidth = Width;
		s.SLeft = 0;
		s.STop = 0;
	} else {
		s.RHeight = pRect->bottom - pRect->top;
		s.RWidth = pRect->right - pRect->left;

		s.SLeft = pRect->left;
		s.STop = pRect->top;
	}
	if(Pitch == 0) {
		s.Pitch = s.Width * s.bpp;
	} else {
		s.Pitch = Pitch;
	}
	s.swiz.Init(Width, Height, 0);
	s.mask[0] = s.swiz.m_MaskU;
	s.mask[1] = s.swiz.m_MaskV;

 	if (DBG_CHECK(1))
    {
        if (s.SLeft + s.RWidth > Width
            || s.STop + s.RHeight > Height)
        {
            DXGRIP("XGUnwizzleRect - The swizzled rectangle must fit completely inside of the source texture, this routine does no clipping.");
        }

        if (s.Pitch < s.RWidth * BytesPerPixel)
        {
            DXGRIP("XGUnwizzleRect - The pitch of the destination rectangle must be at least as big as the rectangle width * BytesPerPixel.");
        }
    }

	if(!s.RHeight || !s.RWidth) return; //width == 0, or height == 0

	DWORD WMask = WidthMasks[(BytesPerPixel >> 1)]; //15,15,7
	DWORD HMask = HeightMasks[(BytesPerPixel >> 1)]; //3,3,3
	
	//if the left edge isn't 2-pixel aligned, deal with that first.
	if(s.SLeft & 1) {
		s.xOff = 0;  s.yOff = 0;
		s.uWidth = 1; s.uHeight = s.RHeight;
		XGInternalUnswizzle1x1Column(&s);
		++s.SLeft; 
		++s.DLeft;
		if(!--s.RWidth) return; //if width == 0, we're done
	}

	//if the right edge isn't 2-pixel aligned, do that too...
	if(s.RWidth & 1) {
		s.RWidth--;
		s.xOff = s.RWidth;   s.yOff = 0;
		s.uWidth = 1;
		s.uHeight = s.RHeight;
		XGInternalUnswizzle1x1Column(&s);
		if(!s.RWidth) return; //if width == 0, we're done.
	}

	//if there aren't going to be any 8x8's, just fill the rest using 2x1s.
	//if left edge (aligned right) is >= right edge (aligned left), there is no opportunity for 8x8.
	//if top edge, (aligned downward) is lower than bottom edge (aligned upward), there is no opportunity for 8x8.
	if((((s.SLeft + WMask) & ~WMask) >= ((s.SLeft + s.RWidth ) & ~WMask)) || 
	   (((s.STop  + HMask) & ~HMask) >= ((s.STop  + s.RHeight) & ~HMask)) ||
	   ((s.Width < MinBlockWidth[(BytesPerPixel >> 1)])) ||
	   ((s.Height < MinBlockHeight[(BytesPerPixel >> 1)])))
	{
		s.xOff = 0;              s.yOff = 0;
		s.uHeight = s.RHeight;   s.uWidth = s.RWidth;
		XGInternalUnswizzle2x1(&s);
		return;
	}

	//if the top isn't 8-pixel aligned, deal with that.
	if(s.STop & HMask) {
		DWORD h = (0-s.STop) & HMask; //number of texels from top to aligned top
		s.xOff = 0;           s.yOff = 0;
		s.uWidth = s.RWidth;  s.uHeight = h;
		XGInternalUnswizzle2x1(&s);
		s.STop += h;
		s.DTop += h;
		s.RHeight -= h;
	}

	//if the bottom isn't 8-pixel aligned, deal with that.
	if(s.RHeight & HMask) {
		DWORD h = (s.RHeight & HMask);
		s.RHeight -= h;
		s.xOff = 0; 	       s.yOff = s.RHeight;
		s.uWidth = s.RWidth;   s.uHeight = h;
		XGInternalUnswizzle2x1(&s);
	}

	//if the left isn't 8-pixel aligned, do that...
	if(s.SLeft & WMask) {
		DWORD w = (0-s.SLeft) & WMask;
		s.xOff = 0;     s.yOff = 0;
		s.uWidth = w;   s.uHeight = s.RHeight;
		XGInternalUnswizzle2x1(&s);
		s.DLeft += w;
		s.SLeft += w;
		s.RWidth -= w;
	}

	//if the right isn't 8-pixel aligned, do that too.
	if(s.RWidth & WMask) {
		DWORD w = (s.RWidth & WMask);
		s.RWidth -= w;
		s.xOff = s.RWidth;  s.yOff = 0;
		s.uWidth = w;       s.uHeight = s.RHeight; 
		XGInternalUnswizzle2x1(&s);
	}

	//now, do the 8x8's!
	DWORD AddValU, AddValV;
	DWORD StartU,  StartV;
	DWORD EdiStore;
	DWORD WidthStore;
	
	if(s.bpp == 4) {
		AddValU = (-64) & s.swiz.m_MaskU;
		AddValV = (-32) & s.swiz.m_MaskV;
		StartU = s.swiz.SwizzleU(s.SLeft);
		StartV = s.swiz.SwizzleV(s.STop);
		s.pDest = ((BYTE*)s.pDest) + (s.DLeft * s.bpp) + (s.DTop * s.Pitch);
		s.RWidth  >>= 3; // width /= 8;
		s.RHeight >>= 2; //height /= 4;
		WidthStore = s.RWidth;
		if(/*((DWORD)(s.pDest) & 15) ||*/ ((DWORD)(s.pSource) & 15)) {
			DXGRIP("XGUnswizzleRect: pSource" /*" and pDest"*/ " must be 16-byte aligned");
		}

		
		_asm {
			mov esi, [s.pSource]
			mov edi, [s.pDest]
			mov edx, [s.Width]
			shl edx, 2 //size of each destination row
			mov ebx, [StartU]
			mov ecx, [StartV]

			mov EdiStore, edi;
			align 16;

Start32:	
				mov eax, ebx;				//eax = u
				or eax, ecx;				//eax = u | v
				sub ebx, [AddValU];			//part 1 of "u += 8"

				movaps xmm0, [esi+eax*4];		
				movaps xmm2, [esi+eax*4+16];
				movaps xmm4, [esi+eax*4+64];
				movaps xmm6, [esi+eax*4+80];

				movaps xmm1, xmm0;
				movaps xmm5, xmm4;

				movlhps xmm0, xmm2;
				movhlps xmm2, xmm1;
				movlhps xmm4, xmm6;
				movhlps xmm6, xmm5;

				movaps xmm1, [esi+eax*4+32];
				movaps xmm3, [esi+eax*4+48];
				movaps xmm5, [esi+eax*4+96];
				movaps xmm7, [esi+eax*4+112];

				movups [edi], xmm0;
				movups [edi+16], xmm4;
				movups [edi+edx], xmm2;
				movups [edi+edx+16], xmm6;

				add edi, edx;
				movaps xmm0, xmm1;
				movaps xmm4, xmm5;

				movlhps xmm1, xmm3;
				movhlps xmm3, xmm0;
				movlhps xmm5, xmm7;
				movhlps xmm7, xmm4;

				movups [edi+edx], xmm1;
				movups [edi+edx+16], xmm5;
				movups [edi+edx*2], xmm3;
				movups [edi+edx*2+16], xmm7;

				sub edi, edx;
/*
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} //_asm {nop} _asm {nop} _asm {nop} _asm {nop} 
*/
				add edi, 32;				//move dest ptr to next block
				and ebx, [s.swiz.m_MaskU];
				
				dec [s.RWidth];
 			  jnz Start32;

			  mov eax, [WidthStore];
			  mov ebx, [StartU];
			  mov [s.RWidth], eax;
			  mov edi, [EdiStore];
			  mov eax, [s.Pitch];
			  sub ecx, [AddValV];
		      lea edi, [edi + eax * 4];
			  and ecx, [s.swiz.m_MaskV];
			  mov [EdiStore], edi;

			  dec [s.RHeight];
			jnz Start32;
		}
	} else if ( s.bpp == 2) {
		AddValU = (-256) & s.swiz.m_MaskU;
		AddValV = (-32) & s.swiz.m_MaskV;
		StartU = s.swiz.SwizzleU(s.SLeft);
		StartV = s.swiz.SwizzleV(s.STop);
		s.pDest = ((BYTE*)s.pDest) + (s.DLeft * s.bpp) + (s.DTop * s.Pitch);
		s.RWidth  >>= 4; // width /= 16;
		s.RHeight >>= 2; //height /= 4;
		WidthStore = s.RWidth;
		if(/*((DWORD)(s.pDest) & 15) ||*/ ((DWORD)(s.pSource) & 15)) {
			DXGRIP("XGUnswizzleRect: pSource "/*"and pDest "*/"must be 16-byte aligned");
		}

		_asm {
			mov esi, [s.pSource]
			mov edi, [s.pDest]
			mov edx, [s.Width]
			shl edx, 1 //size of each destination row
			mov ebx, [StartU]
			mov ecx, [StartV]

			mov EdiStore, edi;
			align 16;
Start16:
				mov eax, ebx;					//eax = u
				sub ebx, [AddValU];				//part 1 of "u += 16"
				or eax, ecx;					//eax = u | v

				movaps xmm0, [esi + eax*2       ];
				movaps xmm4, [esi + eax*2 +   32];
				movaps xmm2, [esi + eax*2 +  128];
				movaps xmm6, [esi + eax*2 +128+32];

				movaps xmm1, xmm0;
				movaps xmm3, xmm2;

				shufps xmm0, xmm4, 10001000b;
				shufps xmm1, xmm4, 11011101b;
				shufps xmm2, xmm6, 10001000b;
				shufps xmm3, xmm6, 11011101b;

				movaps xmm4, [esi + eax*2 +   16];
				movaps xmm6, [esi + eax*2 +   48];
				movaps xmm5, [esi + eax*2 +128+16];
				movaps xmm7, [esi + eax*2 +128+48];

				movups [edi], xmm0;
				movups [edi+16], xmm2;
				movups [edi + edx], xmm1;
				movups [edi + edx + 16], xmm3;

				add edi, edx;
				movaps xmm0, xmm4;
				movaps xmm1, xmm5;

				shufps xmm0, xmm6, 10001000b;
				shufps xmm4, xmm6, 11011101b;
				shufps xmm1, xmm7, 10001000b;
				shufps xmm5, xmm7, 11011101b;

				movups [edi + edx], xmm0;
				movups [edi + edx + 16], xmm1;
				movups [edi + edx*2], xmm4;
				movups [edi + edx*2 + 16], xmm5;

/*
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
				_asm {nop} _asm {nop} //_asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} _asm {nop} 
*/
				sub edi, edx;
				add edi, 32;
				and ebx, [s.swiz.m_MaskU];		//ebx = the next u coordinate

				dec [s.RWidth];
			  jnz Start16;

			  mov eax, [WidthStore];
			  mov ebx, [StartU];
			  mov [s.RWidth], eax;
			  mov edi, [EdiStore];
			  mov eax, [s.Pitch];
			  sub ecx, [AddValV];
		      lea edi, [edi + eax * 4];
			  and ecx, [s.swiz.m_MaskV];
			  mov [EdiStore], edi;

			  dec [s.RHeight];
			jnz Start16;
		}
	} else {
		AddValU = (-256) & s.swiz.m_MaskU;
		AddValV = (-32) & s.swiz.m_MaskV;
		StartU = s.swiz.SwizzleU(s.SLeft);
		StartV = s.swiz.SwizzleV(s.STop);
		s.pDest = ((BYTE*)s.pDest) + (s.DLeft * s.bpp) + (s.DTop * s.Pitch);
		s.RWidth  >>= 4; // width /= 16;
		s.RHeight >>= 2; //height /= 4;
		WidthStore = s.RWidth;

		_asm {
			mov esi, [s.pSource]
			mov edi, [s.pDest]
			mov edx, [s.Width]
			mov ebx, [StartU]
			mov ecx, [StartV]

			mov EdiStore, edi;
			align 16;

Start8:
				mov eax, ebx;					//eax = u
				sub ebx, [AddValU];				//part 1 of "u += 4"
				or eax, ecx;					//eax = u | v

				pshufw mm0, [esi + eax       ], 11011000b;
				pshufw mm2, [esi + eax +64   ], 11011000b;
				pshufw mm4, [esi + eax +   16], 11011000b;
				pshufw mm6, [esi + eax +64+16], 11011000b;

				movq mm1, mm0;
				movq mm3, mm2;

				punpckldq mm0, mm4;
				punpckhdq mm1, mm4;
				punpckldq mm2, mm6;
				punpckhdq mm3, mm6;

				movq [edi], mm0;             //row 0
				movq [edi+8], mm2;           //row 0
				movq [edi + edx], mm1;       //row 1
				movq [edi + edx + 8], mm3;   //row 1

				add edi, edx;

				pshufw mm4, [esi + eax +    8], 11011000b;
				pshufw mm5, [esi + eax +64+ 8], 11011000b;
				pshufw mm6, [esi + eax +   24], 11011000b;
				pshufw mm7, [esi + eax +64+24], 11011000b;

				movq mm0, mm4;
				movq mm1, mm5;

				punpckldq mm0, mm6;
				punpckhdq mm4, mm6;
				punpckldq mm1, mm7;
				punpckhdq mm5, mm7;


				movq [edi + edx], mm0;       //row 2
				movq [edi + edx + 8], mm1;   //row 2
                movq [edi + edx*2], mm4;     //row 3
                movq [edi + edx*2 + 8], mm5; //row 3


				sub edi, edx;
				add edi, 16;
				and ebx, [s.swiz.m_MaskU];				//ebx = the next u coordinate

				dec s.RWidth;
			  jnz Start8;

			  mov eax, [WidthStore];
			  mov ebx, [StartU];
			  mov [s.RWidth], eax;
			  mov edi, [EdiStore];
			  mov eax, [s.Pitch];
			  sub ecx, [AddValV];
		      lea edi, [edi + eax * 4];
			  and ecx, [s.swiz.m_MaskV];
			  mov [EdiStore], edi;

			  dec [s.RHeight];
			jnz Start8;
			emms 
		}

	}

	return;
}


//----------------------------------------------------------------------------
// Swizzle a box from a buffer into a larger texture.  The destination box 
// must be completely contained within the destination texture (no clipping).
//
extern "C"
VOID WINAPI XGSwizzleBox(
    LPCVOID     pSource,      // The buffer that contains the source rectangle
    DWORD       RowPitch,     // Byte offset from the left edge of one row to
                                // the left edge of the next row
    DWORD       SlicePitch,   // Byte offset from the top-left of one slice to
                                // the top-left of the next deepest slice
    CONST D3DBOX * pBox,      // The box within the buffer to copy.
    LPVOID      pDest,        // The destination texture.
    DWORD       Width,        // The width of the entire destination texture.
    DWORD       Height,       // The height of the entire destination texture.
    DWORD       Depth,        // The depth of the entire destination texture.
    CONST XGPOINT3D * pPoint, // Where to put the rectangle in the texture.
    DWORD       BytesPerPixel
    )
{
	//PERF: swizzling/unswizzling when Width==1 || Height==1 || Depth==1 is the same as 2d 
	//PERF: if 2 of the following are true: Width==2, Height==1, Depth==1: use memcpy

	//PERF: optimal swiz/unswiz of 32-bit should be done with 8x2x2. (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 16-bit unswiz is 8x2x2 movups (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 16-bit swiz is 8x2x2 movntps (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 8-bit swiz is 4x2x2 movq 

    DWORD RWidth;
    DWORD RHeight;
    DWORD RDepth;

    DWORD UOffset;
    DWORD VOffset;
    DWORD WOffset;

    if (!pBox && !pPoint && !RowPitch && !SlicePitch)
    {
        RWidth = Width;
        RHeight = Height;
        RDepth = Depth;

        UOffset = 0;
        VOffset = 0;
        WOffset = 0;

        RowPitch = Width * BytesPerPixel;
        SlicePitch = RowPitch * Height;
    }
    else
    {
        if (DBG_CHECK(!pBox || !pPoint || !RowPitch || !SlicePitch))
        {
            DXGRIP("XGSwizzleBox - either all of pBox, pPoint, RowPitch and SlicePitch can be zero or none.");
        }

        pSource = (void *)((BYTE *)pSource + pBox->Left + pBox->Top * RowPitch + pBox->Front * SlicePitch);

        RWidth = pBox->Right - pBox->Left;
        RHeight = pBox->Bottom - pBox->Top;
        RDepth = pBox->Back - pBox->Front;

        UOffset = pPoint->u;
        VOffset = pPoint->v;
        WOffset = pPoint->w;

        if (DBG_CHECK(TRUE))
        {
            if (pPoint->u + RWidth > Width
                || pPoint->v + RHeight > Height
                || pPoint->w + RDepth > Depth)
            {
                DXGRIP("XGSwizzleBox - The swizzled rectangle must fit completely inside of the destination texture, this routine does no clipping.");
            }

            if (RowPitch < RWidth * BytesPerPixel)
            {
                DXGRIP("XGSwizzleBox - The row pitch of the source rectangle must be at least as big as the rectangle width * BytesPerPixel.");
            }

            if (SlicePitch < RowPitch * RHeight)
            {
                DXGRIP("XGSwizzleBox - The slice pitch of the source rectangle must be at least as big as the row pitch * height.");
            }
        }
    }

    unsigned int u, v, w;
    Swizzler swiz(Width, Height, Depth);

    SWIZNUM SwizU = swiz.SwizzleU(UOffset);
    SWIZNUM SwizV = swiz.SwizzleV(VOffset);
    SWIZNUM SwizW = swiz.SwizzleW(WOffset);

    swiz.SetW(SwizW);

    if (BytesPerPixel == 4)
    {
        DWORD *pSrc = (DWORD *)pSource;
        DWORD *pDst = (DWORD *)pDest;
        DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
        DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

        for (w = RDepth; w--;) 
        {
            swiz.SetV(SwizV);

            for (v = RHeight; v--;) 
            {
                swiz.SetU(SwizU);

                for (u = RWidth; u--;) 
                {
                    pDst[swiz.Get3D()] = *(pSrc++);
                    swiz.IncU();
                }

                pSrc = (DWORD *)((BYTE *)pSrc + RowPitchAdjust);
                swiz.IncV();
            }

            pSrc = (DWORD *)((BYTE *)pSrc + SlicePitchAdjust);
            swiz.IncW();
        }
    }
    else if (BytesPerPixel == 2)
    {
        WORD *pSrc = (WORD *)pSource;
        WORD *pDst = (WORD *)pDest;
        DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
        DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

        for (w = RDepth; w--;) 
        {
            swiz.SetV(SwizV);

            for (v = RHeight; v--;) 
            {
                swiz.SetU(SwizU);

                for (u = RWidth; u--;) 
                {
                    pDst[swiz.Get3D()] = *(pSrc++);
                    swiz.IncU();
                }

                pSrc = (WORD *)((BYTE *)pSrc + RowPitchAdjust);
                swiz.IncV();
            }

            pSrc = (WORD *)((BYTE *)pSrc + SlicePitchAdjust);
            swiz.IncW();
        }
    }
    else
    {
        BYTE *pSrc = (BYTE *)pSource;
        BYTE *pDst = (BYTE *)pDest;
        DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
        DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

        for (w = RDepth; w--;) 
        {
            swiz.SetV(SwizV);

            for (v = RHeight; v--;) 
            {
                swiz.SetU(SwizU);

                for (u = RWidth; u--;) 
                {
                    pDst[swiz.Get3D()] = *(pSrc++);
                    swiz.IncU();
                }

                pSrc = pSrc + RowPitchAdjust;
                swiz.IncV();
            }

            pSrc = pSrc + SlicePitchAdjust;
            swiz.IncW();
        }
    }
}

//----------------------------------------------------------------------------
// Unswizzle a box from a texture into a buffer.
//
extern "C"
VOID WINAPI XGUnswizzleBox(
    LPCVOID     pSource,      // The source texture.
    DWORD       Width,        // The width of the entire source texture.
    DWORD       Height,       // The height of the entire source texture.
    DWORD       Depth,        // The depth of the entire destination texture.
    CONST D3DBOX * pBox,      // The rectangle within the texture to copy.
    LPVOID      pDest,        // The destination buffer
    DWORD       RowPitch,     // Byte offset from the left edge of one row to
                                // the left edge of the next row
    DWORD       SlicePitch,   // Byte offset from the top-left of one slice to
                                // the top-left of the next deepest slice
    CONST XGPOINT3D * pPoint, // Where to copy the rectangle to
    DWORD       BytesPerPixel
    )
{
	//PERF: swizzling/unswizzling when Width==1 || Height==1 || Depth==1 is the same as 2d 
	//PERF: if 2 of the following are true: Width==2, Height==1, Depth==1: use memcpy

	//PERF: optimal swiz/unswiz of 32-bit should be done with 8x2x2. (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 16-bit unswiz is 8x2x2 movups (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 16-bit swiz is 8x2x2 movntps (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 8-bit swiz is 4x2x2 movq 



    DWORD RWidth;
    DWORD RHeight;
    DWORD RDepth;

    DWORD UOffset;
    DWORD VOffset;
    DWORD WOffset;

    if (!pBox && !pPoint && !RowPitch && !SlicePitch)
    {
        RWidth = Width;
        RHeight = Height;
        RDepth = Depth;

        UOffset = 0;
        VOffset = 0;
        WOffset = 0;

        RowPitch = Width * BytesPerPixel;
        SlicePitch = RowPitch * Height;
    }
    else
    {
        if (DBG_CHECK(!pBox || !pPoint || !RowPitch || !SlicePitch))
        {
            DXGRIP("XGSwizzleBox - either all of pBox, pPoint, RowPitch and SlicePitch can be zero or none.");
        }

        pDest = (void *)((BYTE *)pDest + pPoint->u + pPoint->v * RowPitch + pPoint->w * SlicePitch);

        RWidth = pBox->Right - pBox->Left;
        RHeight = pBox->Bottom - pBox->Top;
        RDepth = pBox->Back - pBox->Front;

        UOffset = pBox->Left;
        VOffset = pBox->Top;
        WOffset = pBox->Front;

        if (DBG_CHECK(TRUE))
        {
            if (pPoint->u + RWidth > Width
                || pPoint->v + RHeight > Height
                || pPoint->w + RDepth > Depth)
            {
                DXGRIP("XGUnswizzleBox - The swizzled rectangle must fit completely inside of the source texture, this routine does no clipping.");
            }

            if (RowPitch < RWidth * BytesPerPixel)
            {
                DXGRIP("XGUnswizzleBox - The row pitch of the source rectangle must be at least as big as the rectangle width * BytesPerPixel.");
            }

            if (SlicePitch < RowPitch * RHeight)
            {
                DXGRIP("XGUnswizzleBox - The slice pitch of the source rectangle must be at least as big as the row pitch * height.");
            }
        }
    }

    unsigned int u, v, w;
    Swizzler swiz(Width, Height, Depth);

    SWIZNUM SwizU = swiz.SwizzleU(UOffset);
    SWIZNUM SwizV = swiz.SwizzleV(VOffset);
    SWIZNUM SwizW = swiz.SwizzleW(WOffset);

    swiz.SetW(SwizW);

    if (BytesPerPixel == 4)
    {
        DWORD *pSrc = (DWORD *)pSource;
        DWORD *pDst = (DWORD *)pDest;
        DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
        DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

        for (w = RDepth; w--;) 
        {
            swiz.SetV(SwizV);

            for (v = RHeight; v--;) 
            {
                swiz.SetU(SwizU);

                for (u = RWidth; u--;) 
                {
                    *(pDst++) = pSrc[swiz.Get3D()];
                    swiz.IncU();
                }

                pDst = (DWORD *)((BYTE *)pDst + RowPitchAdjust);
                swiz.IncV();
            }

            pDst = (DWORD *)((BYTE *)pDst + SlicePitchAdjust);
            swiz.IncW();
        }
    }
    else if (BytesPerPixel == 2)
    {
        WORD *pSrc = (WORD *)pSource;
        WORD *pDst = (WORD *)pDest;
        DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
        DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

        for (w = RDepth; w--;) 
        {
            swiz.SetV(SwizV);

            for (v = RHeight; v--;) 
            {
                swiz.SetU(SwizU);

                for (u = RWidth; u--;) 
                {
                    *(pDst++) = pSrc[swiz.Get3D()];
                    swiz.IncU();
                }

                pDst = (WORD *)((BYTE *)pDst + RowPitchAdjust);
                swiz.IncV();
            }

            pDst = (WORD *)((BYTE *)pDst + SlicePitchAdjust);
            swiz.IncW();
        }
    }
    else
    {
        BYTE *pSrc = (BYTE *)pSource;
        BYTE *pDst = (BYTE *)pDest;
        DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
        DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

        for (w = RDepth; w--;) 
        {
            swiz.SetV(SwizV);

            for (v = RHeight; v--;) 
            {
                swiz.SetU(SwizU);

                for (u = RWidth; u--;) 
                {
                    *(pDst++) = pSrc[swiz.Get3D()];
                    swiz.IncU();
                }

                pDst = pDst + RowPitchAdjust;
                swiz.IncV();
            }

            pDst = pDst + SlicePitchAdjust;
            swiz.IncW();
        }
    }
}

}

