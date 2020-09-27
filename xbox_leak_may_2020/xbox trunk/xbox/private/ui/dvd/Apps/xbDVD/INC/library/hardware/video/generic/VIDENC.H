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


// FILE:      library\hardware\video\generic\videnc.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   24.03.95
//
// PURPOSE: The abstract class for video encoder programming.
//
// HISTORY:

#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include "library\common\gnerrors.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "library\common\profiles.h"
#include "library\lowlevel\intrctrl.h"
#endif

#include "vidchip.h"
#include "l21recvr.h"



MKTAG (ENC_SAMPLEMODE,						VIDEO_ENCODER_UNIT, 0x1001, VideoSampleMode)
MKTAG (ENC_HOFFSET,							VIDEO_ENCODER_UNIT, 0x1002, int)
MKTAG (ENC_VOFFSET,							VIDEO_ENCODER_UNIT, 0x1003, int)

// The idle screen mapping depends on the specific video encoder. First ask
// for the number of entries via GET_ENC_IDLE_NUMBER, then allocate enough
// space to hold ENC_IDLE_NUMBER times a struct VideoEncoderIdleEntry,
// then call GET_ENC_IDLE_ENTRY to get an array of these structs filled
// with ENC_IDLE_NUMBER entries.

#define VIDENC_IdleNameMaxSize  40

struct __far VideoEncoderIdleEntry
	{
	int number;   // in increasing order
	TCHAR name[VIDENC_IdleNameMaxSize];
	};

MKTAG (ENC_IDLESCREEN,						VIDEO_ENCODER_UNIT, 0x1004, int)   // global for all units
MKTAG (ENC_IDLE_NUMBER,						VIDEO_ENCODER_UNIT, 0x1005, int)   // global for all units
MKTAG (ENC_IDLE_ENTRY,						VIDEO_ENCODER_UNIT, 0x1006, VideoEncoderIdleEntry __far *)   // global for all units

MKTAG (ENC_COPY_PROTECTION,				VIDEO_ENCODER_UNIT, 0x1007, int)

MKTAG (ENC_CHROMA_FILTER,					VIDEO_ENCODER_UNIT, 0x1008, DWORD)

MKTAG (ENC_SVIDEO_ACTIVE,					VIDEO_ENCODER_UNIT, 0x1009, BOOL)

enum OutputSelect
	{
	ENC_TV,
	ENC_VCR,
	VCR_TV
	};

MKTAG (ENC_SET_OUTPUT,						VIDEO_ENCODER_UNIT, 0x1000a, OutputSelect)
MKTAG (ENC_SET_PRESENTATION_MODE,		VIDEO_ENCODER_UNIT, 0x1000b, VideoPresentationMode)

enum ComponentOutputMode
	{
	VCOMPOM_DEFAULT,	// let the Video Encoder implementation decide
	VCOMPOM_DISABLE,	// force to no output
	VCOMPOM_YUV,
	VCOMPOM_RGB
	};

// Selects which type of component output to use
MKTAG (ENC_COMPONENT_OUTPUT,				VIDEO_ENCODER_UNIT, 0x1020, ComponentOutputMode)

// Used for CGMS information
MKTAG (ENC_COPY_MODE,						VIDEO_ENCODER_UNIT, 0x1025, VideoCopyMode)

// Set the following from 0 to 10000. 5000 is neutral (default) setting
MKTAG (ENC_BRIGHTNESS,						VIDEO_ENCODER_UNIT, 0x1030, WORD)
MKTAG (ENC_CONTRAST,							VIDEO_ENCODER_UNIT, 0x1031, WORD)
MKTAG (ENC_SATURATION,						VIDEO_ENCODER_UNIT, 0x1032, WORD)
MKTAG (ENC_HUE,								VIDEO_ENCODER_UNIT, 0x1033, WORD)

MKTAG (ENC_ACTIVE_MUX_SRC,					VIDEO_ENCODER_UNIT, 0x3000, VideoMuxSource)
	// VIDEOMUXSRC_INTERNAL or VIDEOMUXSRC_EXTERNAL
MKTAG (ENC_LINE_MODE,						VIDEO_ENCODER_UNIT, 0x3001, VideoLineMode)
	// VLM_PROGRESSIVE or VLM_INTERLACED
MKTAG (ENC_PIXCLK_MODE,						VIDEO_ENCODER_UNIT, 0x3002, VideoPixClockMode)
	// VIDEOPIXCLK_INTERNAL or VIDEOPIXCLK_EXTERNAL
MKTAG (ENC_EMBEDDED_SYNC,					VIDEO_ENCODER_UNIT, 0x3003, BOOL)
MKTAG (ENC_HSYNC_POLARITY,					VIDEO_ENCODER_UNIT, 0x3005, BOOL)
MKTAG (ENC_VSYNC_POLARITY,					VIDEO_ENCODER_UNIT, 0x3006, BOOL)
MKTAG (ENC_STARTACTIVEPIXEL_DELAY,		VIDEO_ENCODER_UNIT, 0x3007, int)

// Errors from the video encoder.

#define GNR_VENC_PARAMS						MKERR(ERROR, VENCODER, PARAMS,  0x01)
	// illegal parameters
#define GNR_MACROVISION_NOT_SUPPORTED	MKERR(ERROR, VENCODER, GENERAL, 0x02)

#define GNR_CC_NOT_SUPPORTED				MKERR(ERROR, VENCODER, GENERAL, 0x03)

#ifndef ONLY_EXTERNAL_VISIBLE


class VideoEncoder : public VideoChip
	{
	friend class VirtualVideoEncoder;

	protected:
		GenericProfile *profile;
		int idleScreen;
		int nIdleScreens;
		VideoEncoderIdleEntry __far *idleEntries;
		BOOL svideoActive;

	public:
		VideoEncoder (GenericProfile *profile = NULL, int nIdleScreens = 0,
		              VideoEncoderIdleEntry __far *idleEntries = NULL);

		// Dynamic type conversion routine to InterruptHandeler
		virtual InterruptHandler * ToInterruptHandler(void)	{return NULL;}
#if TEST_PAL_TO_NTSC
		virtual Error SetVideoStandard (VideoStandard standard)			{GNRAISE_OK;}
		virtual Error SetMovingImageStandard (MovingImageStandard mis)	{GNRAISE_OK;}
#endif
		virtual Error SetSampleMode (VideoSampleMode mode) = 0;
		virtual Error SetHOffset (int offset) = 0;
		virtual Error SetVOffset (int offset) = 0;
		virtual Error SetIdleScreen (int idleScreen);
		virtual Error SetCopyProtection(int protection)					{GNRAISE_OK;}
		virtual Error SetChromaFilter(DWORD frequency)					{GNRAISE_OK;}
		virtual Error SetSVideoActive (BOOL active)						{GNRAISE_OK;}
		virtual Error SetComponentOut (ComponentOutputMode compOut)	{GNRAISE_OK;}
		virtual void DeactivateOutputs (void) = 0;
		virtual void ReactivateOutputs (void) = 0;
	};


class VirtualVideoEncoder : public VirtualVideoChip
	{
	protected:
		VideoEncoder			*encoder;
		VideoSampleMode		sampleMode;
		int						hOffset, vOffset;   // from 7110/7111 timing
		int						copyProtection;
		DWORD						chromaFilterFrequency;
		ComponentOutputMode	compOut;

		Error Preempt (VirtualUnit *previous);
	public:
		VirtualVideoEncoder (VideoEncoder *encoder);
		Error Configure (TAG __far * tags);

		// Dynamic type conversion routine to Line21Receiver
		virtual Line21Receiver * ToLine21Receiver(void) {return NULL;}
	};

#endif // ONLY_EXTERNAL_VISIBILE

#endif
