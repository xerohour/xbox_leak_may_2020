/*************************************************************************

Copyright (c) Microsoft Corporation 1996 - 1999

Module Name:

	typedef.hpp

Abstract:

    Basic types:
		CVector2D, CPixel, CMotionVector, 

Author:

	Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
	Wei-ge Chen (wchen@microsoft.com) 20-March-1996
	Bruce Lin	(blin@microsoft.com) 14-April-1996
	Chuang Gu (chuanggu@microsoft.com) 5-June-1996

Revision History:


*************************************************************************/


#ifndef __TYPEDEF_HPP_
#define __TYPEDEF_HPP_

#include "wmvdec_api.h"

#include "macros_wmv.h"

#ifdef UNDER_CE
#ifdef DEBUG
#include <windows.h>
#include <vfw_ce.h>
#define assert(x) if((x)==0) DebugBreak();
#else
#define assert(x)
#endif
#else
#include "assert.h"
#endif

#include <string.h>


/////////////////////////////////////////////
//
//  Typedefs for basic types
//
/////////////////////////////////////////////
#define Class		class

//typedef void *LPVoid_WMV;
//typedef unsigned long U32;
//typedef int I32;
//typedef unsigned short U16;
//typedef short I16;
//typedef double Double;
//typedef float Float;
//typedef float Float32;
//typedef float F32;
//typedef unsigned char U8;
//typedef signed char I8;
//typedef void Void_WMV;
//typedef char Char;
typedef __int64 Int64;
typedef __int64 I64;
typedef unsigned __int64 U64;

typedef I16_WMV PixelI;
typedef U8_WMV PixelC;
//typedef U32 UInt;
//typedef I32 Int;
//typedef I32 Long;
//typedef U32 ULong;
//typedef I32 Bool;
typedef I32_WMV CoordI;
typedef I32_WMV Wide;
typedef U32_WMV UWide;
typedef I32_WMV PixelI32;

#ifdef macintosh
typedef long long Time;
#else
typedef __int64 Time;
#endif

typedef enum {MP4S, MPG4, MP42, MP43, WMV1, WMV2} CodecVersion;
enum {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8};
typedef enum BlockNum {
	ALL_Y_BLOCKS	= 0,
	Y_BLOCK1		= 1,
	Y_BLOCK2		= 2,
	Y_BLOCK3		= 3,
	Y_BLOCK4		= 4,
	U_BLOCK			= 5,
	V_BLOCK			= 6,
	A_BLOCK1		= 7,
	A_BLOCK2		= 8,
	A_BLOCK3		= 9,
	A_BLOCK4		= 10,
	ALL_A_BLOCKS	= 11
} BlockNum;


/////////////////////////////////////////////
// Control flag for varius compiler
//
/////////////////////////////////////////////

//some platforms might not support inline assemb
#ifdef _WMV_TARGET_X86_
#define _SCRUNCH_INLINEASM
#else
#undef _SCRUNCH_INLINEASM
#endif // _WMV_TARGET_X86_

/////////////////////////////////////////////
//
//  Motion Vectors
//
/////////////////////////////////////////////

union Buffer {
    I16_WMV i16[64];
    I32_WMV i32[64];
};

typedef struct tagCVector
{
	I8_WMV x;
	I8_WMV y;
} CVector;

typedef struct tagCWMVMBMode // MacroBlock Mode
{
	// Some extra data member
    U32_WMV m_bSkip : 1;	// is the Macroblock skiped. = COD in H.263
    U32_WMV m_bCBPAllZero : 1;	// are the CBP for 6 blocks all zero?
    U32_WMV m_bZeroMV : 1; //whether the MB has four motion vectors (for forward vectors)
    U32_WMV m_bBlkXformSwitchOn : 1;   // Boolean which indicates whether the MB or block-based transform switching is chosen
    U32_WMV m_iMBXformMode : 2;         // Transform type for the macroblock
    U32_WMV m_iDCTTable_MB_Index : 2;
    U32_WMV m_iMVPredDirection : 2; // 2: use original pred direction; 0: from left MB; 1: from up MB.
    U32_WMV m_dctMd : 1; // is the Macroblock inter- or intra- coded    
    U8_WMV m_rgbCodedBlockPattern;// bitfield(Y1,Y2,Y3,Y4,U,V=0) for each block, 1 = some non-DC components are coded
    U8_WMV m_rgbDCTCoefPredPattern; //bitfield(V,U,Y4,Y3,Y2,Y1,ALLBLOCK=0) [NUM_BLOCKS + 1]; //for each block, 1 = some non-DC components are coded

} CWMVMBMode;

typedef struct tagPackHuffmanCode_WMV
{
    U32_WMV code   : 26;
    U32_WMV length : 6;
} tPackHuffmanCode_WMV;
#endif // __BASIC_HPP_
