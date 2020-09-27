/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1998

Module Name:

	header.h

Abstract:
	define number of bits and some information for encoder/decoder

Author:

	Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996

Revision History:

*************************************************************************/

#ifndef __MACROS_WMV_H_ 
#define __MACROS_WMV_H_

// for motion 
#define NOT_MV 10000

// macroblock
#define MB_SIZE 16
#define MB_SQUARE_SIZE 256

#define ALLBLOCK 0
#define SUB_BLOCK_SIZE 4 // for YVU9 color input
// block
#define BLOCK_SIZE 8
#define BLOCK_SIZE_TIMES2 16
#define BLOCK_SIZE_TIMES6 48
#define BLOCK_SIZE_TIMES2_TIMES2	32
#define BLOCK_SIZE_TIMES2_TIMES3	48
#define BLOCK_SIZE_TIMES2_TIMES4	64
#define BLOCK_SIZE_TIMES2_TIMES5	80
#define BLOCK_SIZE_TIMES2_TIMES6	96
#define BLOCK_SIZE_TIMES2_TIMES7	112

#define BLOCK_SQUARE_SIZE 64
#define BLOCK_SQUARE_SIZE_TIMES2 128
#define BLOCK_SQUARE_SIZE_TIMES3 192
#define BLOCK_SQUARE_SIZE_TIMES5 320
#define BLOCK_SQUARE_SIZE_TIMES6 384
#define BLOCK_SQUARE_SIZE_TIMES7 448
#define BLOCK_SQUARE_SIZE_TIMES9 576
#define BLOCK_SQUARE_SIZE_TIMES11 704
#define BLOCK_SQUARE_SIZE_TIMES12 768
#define BLOCK_SQUARE_SIZE_TIMES13 832

#ifdef _EMB_WMV2_
#define VECTORIZE_MOTIONCOMP_RNDCTRL_OFF 0x10000
#define VECTORIZE_MOTIONCOMP_RNDCTRL_ON 0x0
#endif

#if defined(DYNAMIC_EDGEPAD)
#       if defined(STATIC_PAD_0) 
#       define EXPANDY_REFVOP		    0
#       define EXPANDUV_REFVOP			0
#       else
#       define EXPANDY_REFVOP		    8
#       define EXPANDUV_REFVOP			4
#endif
#else
#define EXPANDY_REFVOP					32
#define EXPANDUV_REFVOP					16
#endif

#define INTER   1
#define INTRA   0

#define __huge 
#define own // used as "reserved word" to indicate ownership or transfer to distinguish from const
#define TRUE 1
#define FALSE 0

#define Sizeof(x) ((U32_WMV) sizeof (x))
#define DELETE_ARRAY(ptr) if (ptr) {delete [] ptr; ptr = NULL_WMV;};
#define DELETE_PTR(ptr) if (ptr) {delete ptr; ptr = NULL_WMV;};
#define FREE_PTR(ptr) if (ptr) {wmvfree(ptr); ptr = NULL_WMV;};

#define NUM_BLOCKS	6
#define VAR_FOR_HYBRID_MV 8
#define MIN_BITRATE_MB_TABLE 50
#define MAX_BITRATE_DCPred_IMBInPFrame 128 
#define MOTEST_VIDEO_CONFERENCE 1
#define MOTEST_PLATEAU 2


#ifndef ICERR_OK
#define ICERR_OK		0L
#define ICERR_ERROR		-100L
#define	ICERR_MEMORY		-3L
#define ICERR_BADFORMAT        	-2L
#endif

#define checkRange(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

#ifndef _MIN
#define _MIN
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#define max(a,b) (((a) > (b)) ? (a) : (b))

#endif // __HEADER_H_
