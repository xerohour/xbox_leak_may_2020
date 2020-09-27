
// FILE:      library\hardware\jpeg\generic\jpegdefs.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   12.01.96
//
// PURPOSE: Common JPEG definitions.
//
// HISTORY:

#ifndef JPEGDEFS_H
#define JPEGDEFS_H

#include "library\common\prelude.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\hooks.h"
#include "library\common\gnerrors.h"
#include "library\hardware\video\generic\vidtypes.h"



typedef struct JPEGCaptureMsgStruct
	{
	FPTR info;
	FPTR userData;
	DWORD frameNum;
	DWORD byteSize;   // of compressed data or buffer byte size
	DWORD captureTime;
	} JPEGCaptureMsg;

MKHOOK(JPEGCapture, JPEGCaptureMsg);



typedef struct JPEGPlaybackMsgStruct
	{
	FPTR userData;
	DWORD nowTime;   // system time for frame in milliseconds
	DWORD baseTime;  // base system time for first IRQ
	} JPEGPlaybackMsg;

MKHOOK(JPEGPlayback, JPEGPlaybackMsg);



typedef struct JPEGMsgStruct
	{
	FPTR userData;
	} JPEGMsg;

MKHOOK(JPEGMessage, JPEGMsg);



// Allowed compression video standards.

enum AllowedJPEGVStandard
	{
	AVSTD_NTSC_ONLY, AVSTD_PAL_ONLY, AVSTD_NTSC_PAL_BOTH
	};



#define GNR_JPEG_PIPE_FULL	MKERR(WARNING, JPEG, GENERAL, 0x01)   // cannot store more buffers

#define GNR_JPEG_BOUNDS		MKERR(ERROR, JPEG, BOUNDS,   0x10)
#define GNR_JPEG_ERROR		MKERR(ERROR, JPEG, GENERAL,  0x11)   // very general error type
#define GNR_JPEG_TIMEOUT	MKERR(ERROR, JPEG, TIMEOUT,  0x12)



// All parameters are accessible via tag calls.

MKTAG (JPG_VIDEO_STANDARD, JPEG_CODEC_UNIT, 0x0000001, VideoStandard)
MKTAG (JPG_VIDEO_FORMAT,   JPEG_CODEC_UNIT, 0x0000002, VideoFormat)
MKTAG (JPG_VIDEO_FMT_LIST, JPEG_CODEC_UNIT, 0x0000003, DWORD)
MKTAG (JPG_COMPRESS,       JPEG_CODEC_UNIT, 0x0000004, BOOL)
MKTAG (JPG_STILLMODE,      JPEG_CODEC_UNIT, 0x0000005, BOOL)
MKTAG (JPG_TOP,            JPEG_CODEC_UNIT, 0x0000006, int)
MKTAG (JPG_LEFT,           JPEG_CODEC_UNIT, 0x0000007, int)
MKTAG (JPG_WIDTH,          JPEG_CODEC_UNIT, 0x0000008, WORD)
MKTAG (JPG_HEIGHT,         JPEG_CODEC_UNIT, 0x0000009, WORD)
MKTAG (JPG_THUMBNAIL,      JPEG_CODEC_UNIT, 0x000000A, BOOL)
MKTAG (JPG_FIELD_ORDER,    JPEG_CODEC_UNIT, 0x000000B, VideoField)
MKTAG (JPG_QTABLE,         JPEG_CODEC_UNIT, 0x000000C, BYTE __far *)
MKTAG (JPG_HUFFTABLE,      JPEG_CODEC_UNIT, 0x000000D, BYTE __far *)
MKTAG (JPG_MSEC_PER_FRAME, JPEG_CODEC_UNIT, 0x000000E, DWORD)
MKTAG (JPG_SINGLEFRAME,    JPEG_CODEC_UNIT, 0x000000F, BOOL)   // it's single frame grab

MKTAG (JPG_TOTALCODEVOL,   JPEG_CODEC_UNIT, 0x0000010, DWORD)
MKTAG (JPG_MAXBLOCKVOL,    JPEG_CODEC_UNIT, 0x0000011, WORD)
MKTAG (JPG_SCALEFACTOR,    JPEG_CODEC_UNIT, 0x0000012, WORD)
MKTAG (JPG_ENABLE_DRI,     JPEG_CODEC_UNIT, 0x0000013, BOOL)
MKTAG (JPG_DRI,            JPEG_CODEC_UNIT, 0x0000014, WORD)

MKTAG (JPG_FILTERS_ON,     JPEG_CODEC_UNIT, 0x0000020, BOOL)
MKTAG (JPG_USE_DMA,        JPEG_CODEC_UNIT, 0x0000021, BOOL)
MKTAG (JPG_HAS_CODEMEMORY, JPEG_CODEC_UNIT, 0x0000022, BOOL)  // read-only; do we have on-board code buffers ?
MKTAG (JPG_HOFFSET,        JPEG_CODEC_UNIT, 0x0000023, int)   // operation-dependent horizontal offset, read-only
MKTAG (JPG_VOFFSET,        JPEG_CODEC_UNIT, 0x0000024, int)   // operation-dependent horizontal offset, read-only
MKTAG (JPG_WANTS_EXT_FI,   JPEG_CODEC_UNIT, 0x0000025, BOOL)  // operation-dependent external field indication, read-only
MKTAG (JPG_ALLOWED_VSTD,   JPEG_CODEC_UNIT, 0x0000026, AllowedJPEGVStandard)  // read-only
MKTAG (JPG_MAX_FIELDSIZE,  JPEG_CODEC_UNIT, 0x0000027, DWORD) // read-only; maximum field size

MKTAG (JPG_PREEMPT_HOOK,   JPEG_CODEC_UNIT, 0x0000030, JPEGMessageHook __far *)
MKTAG (JPG_CCIR_LEVELS,    JPEG_CODEC_UNIT, 0x0000031, BOOL)

MKTAG (JPG_VBUS_WIDTH,     JPEG_CODEC_UNIT, 0x0000040, WORD)   // read only, video bus image width
MKTAG (JPG_VBUS_HEIGHT,    JPEG_CODEC_UNIT, 0x0000041, WORD)   // read only, video bus image height



#endif
