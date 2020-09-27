
// FILE:      library\hardware\jpeg\generic\yuv2rgb.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development. All Rights Reserved.
// CREATED:   09.02.96
//
// PURPOSE: Functions to convert YUV to RGB.
//
// HISTORY:

#ifndef YUV2RGB_H
#define YUV2RGB_H

#include "library\common\prelude.h"


extern "C"
	{
#if NT_KERNEL
	WORD __stdcall mapYUV422BlockstoBGR24 (FPTR src, FPTR dst, WORD width, WORD height, WORD fieldOrder);
	WORD __stdcall mapYUV422BlockstoRGB16 (FPTR src, FPTR dst, WORD width, WORD height, WORD fieldOrder);
	WORD __stdcall mapYUV422BlockstoBGR32 (FPTR src, FPTR dst, WORD width, WORD height, WORD fieldOrder);
	
	WORD __stdcall mapYUV422BlockstoBGR32Scale (FPTR src, FPTR dst, WORD width, WORD height, WORD dstWidth, WORD dstHeight);
#else
	WORD __far PASCAL mapYUV422BlockstoBGR24 (FPTR src, FPTR dst, WORD width, WORD height, WORD fieldOrder);
	WORD __far PASCAL mapYUV422BlockstoRGB16 (FPTR src, FPTR dst, WORD width, WORD height, WORD fieldOrder);
	WORD __far PASCAL mapYUV422BlockstoBGR32 (FPTR src, FPTR dst, WORD width, WORD height, WORD fieldOrder);
	
	WORD __far PASCAL mapYUV422BlockstoBGR24Scale (FPTR src, FPTR dst, WORD width, WORD height, WORD dstWidth, WORD dstHeight);
	WORD __far PASCAL mapYUV422BlockstoBGR32Scale (FPTR src, FPTR dst, WORD width, WORD height, WORD dstWidth, WORD dstHeight);
#endif
	};

#endif
