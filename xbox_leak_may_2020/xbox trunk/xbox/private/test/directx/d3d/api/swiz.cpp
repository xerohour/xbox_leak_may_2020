#include "d3dapi.hpp"
#include "xgraphics.h"

/*
optimized 2d swizzler & deswizzler

  Requirements: 
	src and dest buffers must be 16-byte aligned. 32-byte is prefered.
	Width of the texture must be >= 4.
	Height of the texture must be >= 2.

	If width == 1 or 2, or Height == 1, the texture looks the same swizzled or linear, so don't bother with these routines

  the bpp parameter is bytes-per-pixel of the texture (1, 2, or 4)
*/


typedef unsigned long DWORD;
#define min(a,b) (((a) < (b)) ? (a) : (b))

/*
inline DWORD Log2 (DWORD Value)
{
//    ASSERT((Value & (Value - 1)) == 0);
    __asm 
    {
        mov eax,[Value]   
        bsf ecx,eax 
		mov eax, ecx //return code is in eax
    }
}
*/

inline void GetMasks(int w, int h, DWORD* pXAnd, DWORD* pYAnd) 
{
	DWORD LogWidth, LogHeight, Log;

	LogWidth = Log2(w); 
	LogHeight = Log2(h);

	Log = min(LogWidth, LogHeight);

    DWORD LowerMask = (1 << (Log << 1)) - 1;
    DWORD UpperMask = ~LowerMask;

    DWORD XAnd = (LogWidth > LogHeight) ? (0x55555555 | UpperMask)
                                        : (0x55555555 & LowerMask);

    DWORD YAnd = (LogWidth < LogHeight) ? (0xaaaaaaaa | UpperMask)
                                        : (0xaaaaaaaa & LowerMask);

	XAnd &= ((1 << (LogWidth + LogHeight)) - 1); //we're letting u & v just loop, so
	YAnd &= ((1 << (LogWidth + LogHeight)) - 1); //we need to limit the bits to the ones we need.

	*pXAnd = XAnd;
	*pYAnd = YAnd;
}

/*
Original code:
to Swizzle:
	do { 
		do {
			dest[u | v] = *src++;
		} while(0 != (u = (u - UAnd) & UAnd));
	} while(0 != (v = (v - VAnd) & VAnd));

to UnSwizzle:
	do { 
		do {
			*dest++ = src[u | v];
		} while(0 != (u = (u - UAnd) & UAnd));
	} while(0 != (v = (v - VAnd) & VAnd));

*/
	

void unswiz2_8 (void* src, void*dest, int w, int h) 
{
	DWORD XAnd, YAnd, XAddVal, YAddVal;

	GetMasks(w, h, &XAnd, &YAnd);

	XAddVal = (-16) & XAnd;
	YAddVal = (-8) & YAnd;

	//eax = scratch
	//ebx = x
	//ecx = y
	//edx = pitch
	//esi = src (this is not changed)
	//edi = dest (this is incrimented)

	_asm {
		mov esi, src;
		mov edi, dest;

		mov edx, [w];
		xor ebx, ebx;
		xor ecx, ecx;

Start:
		//do {
		//	do {
		mov eax, ebx;
		or eax, ecx;
		movq mm0, [esi + eax];
		pshufw mm0, mm0, 11011000b; //swap mm0's Word1 and Word2
		sub ebx, [XAddVal];
		movd [edi], mm0;

		psrl mm0, 32;
		movd [edi + edx], mm0;

		add edi, 4;
		
			//} while (0 != (x = (x - XAddVal) & XAnd)));
		and ebx, [XAnd];
		jnz Start;

		//} while (0 != (y = (y - YAddVal) & Yand)));
		sub ecx, [YAddVal];

		add edi, edx; //move edi to the next row (which is actually 2 rows down where we just were)

		and ecx, [YAnd];
		jnz Start;

		emms; //we're done with emms stuff now...
	}
}

void unswiz2_16 (void* src, void*dest, int w, int h) 
{
	DWORD XAnd, YAnd, XAddVal, YAddVal;

	GetMasks(w, h, &XAnd, &YAnd);

	XAddVal = (-16) & XAnd;
	YAddVal = (-8) & YAnd;


	//eax = scratch
	//ebx = x
	//ecx = y
	//edx = pitch
	//esi = src (this is not changed)
	//edi = dest (this is incrimented)

	_asm {
		mov esi, src;
		mov edi, dest;

		mov edx, [w];
		xor ebx, ebx;
		xor ecx, ecx;
		add edx, edx; //shift [w], since we're doing 2-byte stuff now

Start:
		//do {
			//do {
		mov eax, ebx;
		or eax, ecx;
		movaps xmm0, [esi + eax*2];
		shufps xmm0, xmm0, 11011000b; //swap mm0's Word1 and Word2
		sub ebx, [XAddVal];
		movlps [edi], xmm0;

		movhps [edi + edx], xmm0;

		add edi, 8;
		
			//} while (0 != (x = (x - XAddVal) & XAnd)));
		and ebx, [XAnd];
		jnz Start;

		//} while (0 != (y = (y - YAddVal) & Yand)));
		sub ecx, [YAddVal];

		add edi, edx; //move edi to the next row (which is actually 2 rows down where we just were)

		and ecx, [YAnd];
		jnz Start;
	}
}

void unswiz2_32 (void* src, void*dest, int w, int h) 
{
	DWORD XAnd, YAnd, XAddVal, YAddVal;

	GetMasks(w, h, &XAnd, &YAnd);

	XAddVal = (-4) & XAnd;
	YAddVal = (-8) & YAnd;


	//eax = scratch
	//ebx = x
	//ecx = y
	//edx = pitch
	//esi = src (this is not changed)
	//edi = dest (this is incrimented)

	_asm {
		mov esi, src;
		mov edi, dest;

		mov edx, [w];
		xor ebx, ebx;
		xor ecx, ecx;
		shl edx, 2; //shift [w], since we're doing 4-byte stuff now

Start:
		//do {
			//do {
		mov eax, ebx;
		or eax, ecx;

		movaps xmm0, [esi+eax*4];		
		sub ebx, [XAddVal];

		movlps [edi], xmm0;
		movhps [edi + edx], xmm0;

		add edi, 8;
		
			//} while (0 != (x = (x - XAddVal) & XAnd)));
		and ebx, [XAnd];
		jnz Start;

		//} while (0 != (y = (y - YAddVal) & Yand)));
		sub ecx, [YAddVal];

		add edi, edx; //move edi to the next row (which is actually 2 rows from where we just were)

		and ecx, [YAnd];
		jnz Start;
	}
}




void swiz2_8 (void* src, void*dest, int w, int h) 
{
	DWORD XAnd, YAnd, XAddVal, YAddVal;

	GetMasks(w, h, &XAnd, &YAnd);

	XAddVal = (-16) & XAnd;
	YAddVal = (-8) & YAnd;


	//eax = scratch
	//ebx = x
	//ecx = y
	//edx = pitch
	//esi = src (this is incrimented)
	//edi = dest (this is not changed)

	_asm {
		mov esi, src;
		mov edi, dest;

		mov edx, [w];
		xor ebx, ebx;
		xor ecx, ecx;

Start:
		//do {
			//do {

		mov eax, ebx;
		movd mm0, [esi];
		or eax, ecx;
		punpcklwd mm0, [esi + edx]
		sub ebx, [XAddVal];

		movq [edi + eax], mm0;

		add esi, 4;
		
			//} while (0 != (x = (x - XAddVal) & XAnd)));
		and ebx, [XAnd];
		jnz Start;

		//} while (0 != (y = (y - YAddVal) & Yand)));
		sub ecx, [YAddVal];

		add esi, edx; //move esi to the next row (which is actually 2 rows down where we just were)

		and ecx, [YAnd];
		jnz Start;

		emms; //we're done with emms stuff now...
	}
}

void swiz2_16 (void* src, void*dest, int w, int h) 
{
	DWORD XAnd, YAnd, XAddVal, YAddVal;

	GetMasks(w, h, &XAnd, &YAnd);

	XAddVal = (-16) & XAnd;
	YAddVal = (-8) & YAnd;


	//eax = scratch
	//ebx = x
	//ecx = y
	//edx = pitch
	//esi = src (this is incrimented)
	//edi = dest (this is not changed)

	_asm {
		mov esi, src;
		mov edi, dest;

		mov edx, [w];
		xor ebx, ebx;
		xor ecx, ecx;
		add edx, edx; //shift [w], since we're doing 2-byte stuff now

Start:
		//do {
			//do {
				//dest[(u | v) + {0, 1, 2, 3}] = src[{0, 1, 4, 5}]
		mov eax, ebx;
		movlps xmm0, [esi];
		or eax, ecx;
		movlps xmm1, [esi + edx];  
		unpcklps xmm0, xmm1;  
		sub ebx, [XAddVal];

		movaps [edi + eax*2], xmm0

		add esi, 8;
		
			//} while (0 != (x = (x - XAddVal) & XAnd)));
		and ebx, [XAnd];
		jnz Start;

		//} while (0 != (y = (y - YAddVal) & Yand)));
		sub ecx, [YAddVal];

		add esi, edx; //move esi to the next row (which is actually 2 rows down where we just were)

		and ecx, [YAnd];
		jnz Start;
	}
}

void swiz2_32 (void* src, void*dest, int w, int h) 
{
	DWORD XAnd, YAnd, XAddVal, YAddVal;

	GetMasks(w, h, &XAnd, &YAnd);

	XAddVal = (-4) & XAnd;
	YAddVal = (-8) & YAnd;


	//eax = scratch
	//ebx = x
	//ecx = y
	//edx = pitch
	//esi = src (this is incrimented)
	//edi = dest (this is not changed)

	_asm {
		mov esi, src;
		mov edi, dest;

		mov edx, [w];
		xor ebx, ebx;
		xor ecx, ecx;
		shl edx, 2; //shift [w], since we're doing 4-byte stuff now

Start:
		//do {
			//do {
		mov eax, ebx;

		movlps xmm0, [esi];		

		or eax, ecx;

		movhps xmm0, [esi+edx];

		sub ebx, [XAddVal];

		movaps [edi + eax*4], xmm0;

		add esi, 8;
		
			//} while (0 != (x = (x - XAddVal) & XAnd)));
		and ebx, [XAnd];
		jnz Start;

		//} while (0 != (y = (y - YAddVal) & Yand)));
		sub ecx, [YAddVal];

		add esi, edx; //move src to the next row (which is actually 2 rows from where we just were)

		and ecx, [YAnd];
		jnz Start;
	}
}



void unswiz2 (void* src, void* dest, int bpp, int w, int h)
{
	switch(bpp) {
	case 1:
		unswiz2_8 (src, dest, w, h);
		break;
	case 2:
		unswiz2_16 (src, dest, w, h);
		break;
	case 4:
		unswiz2_32 (src, dest, w, h);
		break;
	default:
		_asm { int 3 };
	}
}

void swiz2 (void* src, void* dest, int bpp, int w, int h)
{
	switch(bpp) {
	case 1:
		swiz2_8 (src, dest, w, h);
		break;
	case 2:
		swiz2_16 (src, dest, w, h);
		break;
	case 4:
		swiz2_32 (src, dest, w, h);
		break;
	default:
		_asm { int 3 };
	}
}

LINKME(15)
#pragma data_seg(".d3dapi$test026") 

BEGINTEST(swiztest) 
{
	const int X=256;
	const int Y=256;
	const int times=16384;

	DWORD* BigSrc = new DWORD[X*Y*4+16];
	DWORD* BigDest = new DWORD[X*Y*4+16];
	DWORD* BigDest2 = new DWORD[X*Y*4+16];

	DWORD* src = (DWORD*)(((DWORD)BigSrc) & ~15);
	DWORD* dest = (DWORD*)(((DWORD)BigDest) & ~15);
	DWORD* dest2 = (DWORD*)(((DWORD)BigDest2) & ~15);
	
	__int64 Start, Finish, New, Old;

	int i;

//	__try {
		xStartVariation(hlog, "swiz 8bit");
		{
			for(i = 0; i < X*Y*4; i++) {
				((char*)src)[i] = (char)i;
				((char*)dest)[i]= (char)-i;
				((char*)dest2)[i]= (char)-i;
			}

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				swiz2(src, dest, 1, X, Y);
			}
			Finish = GetTsc();
			New = Finish - Start;

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				Swizzler_Swizzle2D(src, dest2, X, Y, 1);
			}
			Finish = GetTsc();
			Old = Finish - Start;

			xLog(hlog, XLL_INFO, "Old Time: %I64dms, New Time: %I64dms", Old / 733000, New / 733000);

			for(i = 0; i < X*Y; i++) {
				if(((char*)dest)[i] != ((char*)dest2)[i]) 
				{
					xLog(hlog, XLL_FAIL, "not equal. i=0x%x, dest=%x, dest2=%x", i, ((char*)dest)[i], ((char*)dest2)[i]); 
				}
			}

		}
		xEndVariation(hlog);
//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "blah!");
//		xEndVariation(hlog);
//	}
//	__try {
		xStartVariation(hlog, "swiz 16bit");
		{
			for(i = 0; i < X*Y*4; i++) {
				((char*)src)[i] = (char)i;
				((char*)dest)[i]= (char)-i;
				((char*)dest2)[i]= (char)-i;
			}

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				swiz2(src, dest, 2, X, Y);
			}
			Finish = GetTsc();
			New = Finish - Start;

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				Swizzler_Swizzle2D(src, dest2, X, Y, 2);
			}
			Finish = GetTsc();
			Old = Finish - Start;

			xLog(hlog, XLL_INFO, "Old Time: %I64dms, New Time: %I64dms", Old / 733000, New / 733000);

			for(i = 0; i < X*Y; i++) {
				if(((short*)dest)[i] != ((short*)dest2)[i]) 
				{
					xLog(hlog, XLL_FAIL, "not equal. i=0x%x, dest=%x, dest2=%x", i, ((short*)dest)[i], ((short*)dest2)[i]); 
				}
			}
		}
		xEndVariation(hlog);
//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "blah!");
//		xEndVariation(hlog);
//	}
//	__try {
		xStartVariation(hlog, "swiz 32bit");
		{
			for(i = 0; i < X*Y*4; i++) {
				((char*)src)[i] = (char)i;
				((char*)dest)[i]= (char)-i;
				((char*)dest2)[i]= (char)-i;
			}

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				swiz2(src, dest, 4, X, Y);
			}
			Finish = GetTsc();
			New = Finish - Start;

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				Swizzler_Swizzle2D(src, dest2, X, Y, 4);
			}
			Finish = GetTsc();
			Old = Finish - Start;

			xLog(hlog, XLL_INFO, "Old Time: %I64dms, New Time: %I64dms", Old / 733000, New / 733000);

			for(i = 0; i < X*Y; i++) {
				if(dest[i] != dest2[i]) 
				{
					xLog(hlog, XLL_FAIL, "not equal. i=0x%x, dest=%x, dest2=%x", i, dest[i], dest2[i]); 
				}
			}
		}
		xEndVariation(hlog);

//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "blah!");
//		xEndVariation(hlog);
//	}
//	__try {

		xStartVariation(hlog, "unswiz 8bit");
		{
			for(i = 0; i < X*Y*4; i++) {
				((char*)src)[i] = (char)i;
				((char*)dest)[i]= (char)-i;
				((char*)dest2)[i]= (char)-i;
			}

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				unswiz2(src, dest, 1, X, Y);
			}
			Finish = GetTsc();
			New = Finish - Start;

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				Swizzler_Unswizzle2D(src, dest2, X, Y, 1);
			}
			Finish = GetTsc();
			Old = Finish - Start;

			xLog(hlog, XLL_INFO, "Old Time: %I64dms, New Time: %I64dms", Old / 733000, New / 733000);

			for(i = 0; i < X*Y; i++) {
				if(((char*)dest)[i] != ((char*)dest2)[i]) 
				{
					xLog(hlog, XLL_FAIL, "not equal. i=0x%x, dest=%x, dest2=%x", i, ((char*)dest)[i], ((char*)dest2)[i]); 
				}
			}
		}
		xEndVariation(hlog);
//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "blah!");
//		xEndVariation(hlog);
//	}
//	__try {
		xStartVariation(hlog, "unswiz 16bit");
		{
			for(i = 0; i < X*Y*4; i++) {
				((char*)src)[i] = (char)i;
				((char*)dest)[i]= (char)-i;
				((char*)dest2)[i]= (char)-i;
			}

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				unswiz2(src, dest, 2, X, Y);
			}
			Finish = GetTsc();
			New = Finish - Start;

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				Swizzler_Unswizzle2D(src, dest2, X, Y, 2);
			}
			Finish = GetTsc();
			Old = Finish - Start;

			xLog(hlog, XLL_INFO, "Old Time: %I64dms, New Time: %I64dms", Old / 733000, New / 733000);

			for(i = 0; i < X*Y; i++) {
				if(((short*)dest)[i] != ((short*)dest2)[i]) 
				{
					xLog(hlog, XLL_FAIL, "not equal. i=0x%x, dest=%x, dest2=%x", i, ((short*)dest)[i], ((short*)dest2)[i]); 
				}
			}
		}
		xEndVariation(hlog);
//	} __except(1) {
//		xLog(hlog, XLL_EXCEPTION, "blah!");
//		xEndVariation(hlog);
//	}
//	__try {
		xStartVariation(hlog, "unswiz 32bit");
		{
			for(i = 0; i < X*Y*4; i++) {
				((char*)src)[i] = (char)i;
				((char*)dest)[i]= (char)-i;
				((char*)dest2)[i]= (char)-i;
			}

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				unswiz2(src, dest, 4, X, Y);
			}
			Finish = GetTsc();
			New = Finish - Start;

			Start = GetTsc();
			for(i = 0; i < times; i++) {
				Swizzler_Unswizzle2D(src, dest2, X, Y, 4);
			}
			Finish = GetTsc();
			Old = Finish - Start;

			xLog(hlog, XLL_INFO, "Old Time: %I64dms, New Time: %I64dms", Old / 733000, New / 733000);

			for(i = 0; i < X*Y; i++) {
				if(dest[i] != dest2[i]) 
				{
					xLog(hlog, XLL_FAIL, "not equal. i=0x%x, dest=%x, dest2=%x", i, dest[i], dest2[i]); 
				}
			}
		}
		xEndVariation(hlog);
//	} __except(1) { 
//		xLog(hlog, XLL_EXCEPTION, "blah!");
//		xEndVariation(hlog);
//	}

} 
ENDTEST()

