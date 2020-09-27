
// FILE:      library\hardware\jpeg\generic\yuv2rgb.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development. All Rights Reserved.
// CREATED:   09.02.96
//
// PURPOSE: Functions to convert RGB to YUV.
//
// HISTORY:

#ifndef RGB2YUV_H
#define RGB2YUV_H

#include "library\common\prelude.h"


extern "C"
	{
#if NT_KERNEL
	WORD __stdcall mapRGB24toYUV422Blocks (FPTR src, FPTR dst,
	                                          WORD width, WORD height,
	                                          BOOL singleField);
	
	WORD __stdcall mapRGB32toYUV422Blocks (FPTR src, FPTR dst,
	                                          WORD width, WORD height,
	                                          BOOL singleField);
#else
	WORD __far PASCAL mapRGB24toYUV422Blocks (FPTR src, FPTR dst,
	                                          WORD width, WORD height,
	                                          BOOL singleField);
	
	WORD __far PASCAL mapRGB32toYUV422Blocks (FPTR src, FPTR dst,
	                                          WORD width, WORD height,
	                                          BOOL singleField);
#endif
	};

#endif
