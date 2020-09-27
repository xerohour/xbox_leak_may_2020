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


// FILE:      library\hardware\mpegdec\generic\mpegdec.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   10.04.95
//
// PURPOSE:
//
// HISTORY:

#ifndef MPEGDEC_H
#define MPEGDEC_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\hooks.h"
#include "library\common\tagunits.h"
#include "library\hardware\video\generic\vidtypes.h"
#include "library\hardware\mpgcodec\generic\mpgcodec.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "library\common\virtunit.h"
#endif

//
// Commands and parameters:
//
//		none                  : do nothing
//    start                 : start the operation
//    play([1..1000] speed) : play at specific speed (1000 = normal)
//    seek                  : stop playback, go standby
//    stop                  : stop playback
//    step(int frames)      : step some frames
//    freeze                : freeze current frame
//    resync                : resync after seek
//    end                   : end the operation
//
// State transition diagram
//
//	INITIAL:		reset
//
//	SRCSTATE		COMMAND/EVENT		DSTSTATE		ACTION
//
//	reset			start					initial		Init MPEG Decoder, start transfering data
// 				default				reset
//
//	initial		play					playing		Start playing the MPEG Stream
//					...					frozen		Freeze after first I-Frame
//
//	frozen		play					playing		Continue playing from current location
//					seek					seeking		Complete operation, and stop playback
//       		step					stepping		Go to step mode
//					end					reset
//					stop					stopped
//					default				frozen
//
//	playing		play					leaping		Adjust playback speed if required
//											playing
//					seek					seeking		Complete operation and stop playback
//					end					reset
//					stop					stopped
//					default				playing
//
// leaping     default           playing
//
//	stepping		play					playing
//					seek					seeking
//					end					reset
//					stop					stopped
//					default				stepping
//											frozen
//
//	seeking		resync				resyncing
//					resyncue				cued
//					end					reset
//
//	resyncing	play					playing
//					...					frozen
//
//	stopped		play					initial
//					end					reset
//					default				stopped
//

struct MPEGStreamMsg {
	DWORD		size;
	};

struct MPEGSignalMsg {
	DWORD		position;
	};

struct MPEGIdleMsg {
	};

struct MPEGErrorMsg {
	Error		error;
	};

MKHOOK(MPEGStream, MPEGStreamMsg)
MKHOOK(MPEGSignal, MPEGSignalMsg)
MKHOOK(MPEGIdle,   MPEGIdleMsg)
MKHOOK(MPEGError,  MPEGErrorMsg)

typedef MPEGStreamHook	__far *	MPEGStreamHookPtr;
typedef MPEGSignalHook	__far *	MPEGSignalHookPtr;
typedef MPEGIdleHook		__far *	MPEGIdleHookPtr;
typedef MPEGErrorHook	__far	*	MPEGErrorHookPtr;

enum MPEGStreamType {audioOnly, videoOnly, multiplexed};

enum MPEGStreamMode {singleStream, dualStream};

enum MPEGAudioLayer
	{
	MPAUDLAYER_1,
	MPAUDLAYER_2,
	MPAUDLAYER_3							// = MP3
	};

enum MPEGStereoMode
	{
	MPSS_NORMAL,							// Select output of first (normal) stereo program
	MPSS_DEFAULT = MPSS_NORMAL,
	MPSS_SECOND_STEREO					// Select output of second stereo program
	};

MKTAG(MPEG_AUDIO_BITRATE,					MPEG_DECODER_UNIT,	0x0001, DWORD)
MKTAG(MPEG_AUDIO_STREAMID,					MPEG_DECODER_UNIT,	0x0002, BYTE)
MKTAG(MPEG_VIDEO_STREAMID,					MPEG_DECODER_UNIT,	0x0003, BYTE)
MKTAG(MPEG_VIDEO_WIDTH,						MPEG_DECODER_UNIT,	0x0004, WORD)
MKTAG(MPEG_VIDEO_HEIGHT,   				MPEG_DECODER_UNIT,   0x0005, WORD)
MKTAG(MPEG_VIDEO_FPS,      				MPEG_DECODER_UNIT,   0x0006, WORD)
MKTAG(MPEG_STREAM_TYPE,						MPEG_DECODER_UNIT,	0x0007, MPEGStreamType)
MKTAG(MPEG_STREAM_HOOK,						MPEG_DECODER_UNIT,	0x0008, MPEGStreamHookPtr)
MKTAG(MPEG_LEFT_VOLUME,    				MPEG_DECODER_UNIT,   0x0009, WORD)
MKTAG(MPEG_RIGHT_VOLUME,   				MPEG_DECODER_UNIT,   0x000A, WORD)
MKTAG(MPEG_AUDIO_MUTE,     				MPEG_DECODER_UNIT,   0x000B, BOOL)
MKTAG(MPEG_SIGNAL_HOOK,						MPEG_DECODER_UNIT,	0x000C, MPEGSignalHookPtr)
MKTAG(MPEG_SIGNAL_POSITION,				MPEG_DECODER_UNIT,	0x000D, DWORD)
MKTAG(MPEG_DONE_HOOK,						MPEG_DECODER_UNIT,	0x000E, MPEGSignalHookPtr)
MKTAG(MPEG_VIDEO_BITRATE,					MPEG_DECODER_UNIT,   0x000F, DWORD)
MKTAG(MPEG_STREAM_BITRATE,					MPEG_DECODER_UNIT,   0x0010, DWORD)
MKTAG(MPEG_INTRA_QUANTIZER_MATRIX,		MPEG_DECODER_UNIT,	0x0011, BYTE __far *)
MKTAG(MPEG_NON_INTRA_QUANTIZER_MATRIX,	MPEG_DECODER_UNIT,	0x0012, BYTE __far *)
MKTAG(MPEG_VIDEOSTANDARD,					MPEG_DECODER_UNIT,	0x0013, VideoStandard)
#if TEST_PAL_TO_NTSC
MKTAG(MPEG_APP_VIDEOSTANDARD,				MPEG_DECODER_UNIT,	0x0014, VideoStandard)
MKTAG(MPEG_MOVING_IMG_STD,					MPEG_DECODER_UNIT,	0x0015, MovingImageStandard)
#endif
MKTAG(MPEG_STREAM_MODE,						MPEG_DECODER_UNIT,	0x0016, MPEGStreamMode)

MKTAG(MPEG_DISPLAY_WIDTH,					MPEG_DECODER_UNIT,	0x0020, WORD)
MKTAG(MPEG_DISPLAY_HEIGHT,					MPEG_DECODER_UNIT,	0x0021, WORD)

MKTAG(MPEG_VIDEO_BRIGHTNESS,				MPEG_DECODER_UNIT,	0x0022, WORD)
MKTAG(MPEG_VIDEO_CONTRAST,					MPEG_DECODER_UNIT,	0x0023, WORD)

MKTAG(MPEG_AUDIO_SAMPLERATE,				MPEG_DECODER_UNIT,	0x0024, WORD)
MKTAG(MPEG_IDLE_HOOK,						MPEG_DECODER_UNIT,	0x0025, MPEGIdleHookPtr)

MKTAG(MPEG_ASPECT_RATIO,					MPEG_DECODER_UNIT,	0x0026, WORD)
MKTAG(MPEG_SUPPORTS_HIRES_STILL,			MPEG_DECODER_UNIT,	0x0027, BOOL)

MKTAG(MPEG_CURRENT_STREAM_SEGMENT,		MPEG_DECODER_UNIT,	0x0028, DWORD)
MKTAG(MPEG_FIRST_STREAM_SEGMENT,			MPEG_DECODER_UNIT,	0x0029, DWORD)
MKTAG(MPEG_LAST_STREAM_SEGMENT,			MPEG_DECODER_UNIT,	0x002a, DWORD)

MKTAG(MPEG_DISPLAY_MODE,					MPEG_DECODER_UNIT,   0x0030, VideoMode)

// Determines the audio layer contained in the MPEG data/to be decoded
MKTAG(MPEG_AUDIO_LAYER,						MPEG_DECODER_UNIT,	0x0031, MPEGAudioLayer)

// CRC check enable/disable for MPEG (e.g. MP3) decoding (default: FALSE (off))
MKTAG(MPEG_CRC_CHECK,						MPEG_DECODER_UNIT,	0x0032, BOOL)

// Controls second stereo mode for MPEG audio material
MKTAG(MPEG_AUDIO_STEREO_MODE,				MPEG_DECODER_UNIT,	0x0033, MPEGStereoMode)

// Controls Deemphasis on DAC output
MKTAG(MPEG_AUDIO_DEEMPHASIS,				MPEG_DECODER_UNIT,	0x0034, BOOL)

MKTAG(MPEG_ERROR_HOOK,						MPEG_DECODER_UNIT,	0x0035, MPEGErrorHookPtr)


#ifndef ONLY_EXTERNAL_VISIBLE

#define MPEG_BUFF_SIZE	0x40000
#define MPEG_BUFF_ADD	0x01000

class MPEGParams {
	public:
		DWORD					audioBitRate;
		DWORD					videoBitRate;
		DWORD					streamBitRate;
		BYTE					audioStreamID;
		BYTE					videoStreamID;
		WORD					videoWidth;
		WORD					videoHeight;
		WORD					videoFPS;
		MPEGStreamType  	streamType;
		BYTE					intraQuantizerMatrix[64];
		BYTE					nonIntraQuantizerMatrix[64];
		VideoStandard		videoStandard;
		MPEGParams(void);
	};

class MPEGDecoder : public PhysicalUnit {
	friend class VirtualMPEGDecoder;
	protected:
		Error StreamHook(DWORD size);
		Error SignalHook(DWORD position);
		Error DoneHook(void);

		virtual DWORD RecoverData(HPTR data, DWORD size) = 0;
	public:
		virtual Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag) = 0;
		Error DoCommand(MPEGCommand com, DWORD param);
		virtual Error CompleteCommand(DWORD tag) = 0;
		virtual DWORD SendData(HPTR data, DWORD size) = 0;
		virtual void CompleteData(void) = 0;
		virtual DWORD CurrentLocation(void) = 0;
		virtual MPEGState CurrentState(void) = 0;

		virtual Error SetSignal(DWORD position) = 0;

		virtual Error SetLeftVolume(WORD volume) = 0;
		virtual Error SetRightVolume(WORD volume) = 0;
		virtual Error MuteAudio(BOOL mute) = 0;

		virtual Error SetVideoStreamID(BYTE id) = 0;
		virtual Error SetAudioStreamID(BYTE id) = 0;
	};

class VirtualMPEGDecoder : public VirtualUnit {
	friend class MPEGDecoder;
	protected:
		HPTR					buffer;
		DWORD					buffPos;
		DWORD					buffStart;
		DWORD					buffFill;

		DWORD					signalPosition;

		MPEGParams			params;

#if !DRVDLL
		MPEGStreamHookHandle	streamHook;
		MPEGSignalHookHandle	signalHook;
		MPEGSignalHookHandle	doneHook;
#endif

		WORD					leftVolume;
		WORD					rightVolume;
		BOOL					audioMute;
		BOOL					completed;

		DWORD	FindNextValidHeader(void);

		virtual Error GetDisplaySize(WORD __far &width, WORD __far &height);
	private:
		MPEGDecoder		*	decoder;
	public:
		VirtualMPEGDecoder(MPEGDecoder * unit);
		~VirtualMPEGDecoder(void);

		Error Configure (TAG __far *tags);

		Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		Error DoCommand(MPEGCommand com, DWORD param);
		Error CompleteCommand(DWORD tag);

		MPEGState CurrentState(void);

		Error SendBuffer(void);
		Error RecoverBuffer(void);

		DWORD SendData(HPTR data, DWORD size);
		void CompleteData(void);

		DWORD CurrentLocation(void);
	};

inline Error MPEGDecoder::DoCommand(MPEGCommand com, DWORD param)
	{
	DWORD tag;

	GNREASSERT(SendCommand(com, param, tag));
	GNREASSERT(CompleteCommand(tag));

	GNRAISE_OK;
	}

inline Error VirtualMPEGDecoder::DoCommand(MPEGCommand com, DWORD param)
	{
	DWORD tag;

	GNREASSERT(SendCommand(com, param, tag));
	GNREASSERT(CompleteCommand(tag));

	GNRAISE_OK;
	}
#endif

#endif
