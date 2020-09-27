/*************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

	codehead.h

Abstract:
	define number of bits and some information for encoder/decoder

Author:

	Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996

Revision History:

*************************************************************************/

#ifndef __CODEHEAD_H_ 
#define __CODEHEAD_H_

#define MARKER_BIT						1

#define START_CODE_PREFIX				1
#define NUMBITS_START_CODE_PREFIX		24
#define NUMBITS_START_CODE_SUFFIX		8
#define USER_DATA_START_CODE			0xB2



// VOL overhead information
#define NUMBITS_VOL_START_CODE			4
#define VOL_START_CODE					2
#define NUMBITS_VOL_ID					4
#define NUMBITS_VOL_SHAPE				2
#define NUMBITS_VOL_WIDTH				13
#define NUMBITS_VOL_HEIGHT				13
#define NUMBITS_TIME_RESOLUTION			16
// #define NUMBITS_VOL_FCODE				2
#define NUMBITS_SEP_MOTION_TEXTURE		1
#define NUMBITS_QMATRIX					8
#define NUMBITS_SLICE_SIZE				5 // maximum 32 MB's
#define NUMBITS_SLICE_SIZE_WMV2         3 // To indicate Processor's #
// sprite data
#define NUMBITS_NUM_SPRITE_POINTS		6
#define NUMBITS_X_COORDINATE			12
#define NUMBITS_Y_COORDINATE			12
#define SPRITE_MV_ESC					2
#define NUMBITS_SPRITE_MV_ESC			13
	

// VOP overhead information
#define VOP_START_CODE					0xB6
#define NUMBITS_VOP_START_CODE			8
#define NUMBITS_VOP_TIME				5
#define NUMBITS_VOP_HORIZONTAL_SPA_REF	10
#define NUMBITS_VOP_VERTICAL_SPA_REF	10
#define NUMBITS_VOP_WIDTH				13
#define NUMBITS_VOP_HEIGHT				13
#define NUMBITS_VOP_PRED_TYPE			2
#define NUMBITS_VOP_PRED_TYPE_WMV2		1
#define NUMBITS_VOP_QUANTIZER			5
#define NUMBITS_VOP_DBQUANT				2

// Video Packet overhead, added by Toshiba
#define NUMBITS_VP_RESYNC_MARKER        17
#define RESYNC_MARKER                   0x1
#define NUMBITS_VP_QUANTIZER            NUMBITS_VOP_QUANTIZER
#define NUMBITS_VP_HEC                  1
#define NUMBITS_VP_PRED_TYPE            NUMBITS_VOP_PRED_TYPE
#define NUMBITS_VP_INTRA_DC_SWITCH_THR  3

// for MB ovrehead information
#define NUMBITS_MB_SKIP					1

// for motion estimation
#define ADD_DISP						0

// B-VOP parameters
#define I_INDEX							0
#define P_INDEX							1
#define B1_INDEX						2
#define B2_INDEX						3

// Block DCT parameters
#define NUMBITS_ESC_RUN 6
#define NUMBITS_ESC_LEVEL 8

// for MPEG4-compliant

// VO overhead information
#define NUMBITS_VO_START_CODE			3
#define VO_START_CODE					0
#define NUMBITS_VO_ID					5

// VOL overhead information
#define NUMBITS_VOL_FCODE				3

// sprite data
#ifdef __VERIFICATION_MODEL_
#define NUMBITS_SPRITE_USAGE			2
#else
#define NUMBITS_SPRITE_USAGE			1
#endif //__VERIFICATION_MODEL_

// VOP overhead information
#define NUMBITS_VOP_TIMEINCR			10
#define NUMBITS_VOP_FCODE				3

// for MB ovrehead information
#define NUMBITS_MB_SKIP					1

//#define MAX_DISP						3
// #define ADD_DISP						2

#endif // __CODEHEAD_H_
