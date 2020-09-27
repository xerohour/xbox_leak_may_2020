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


// FILE:      library\hardware\mpeg2dec\generic\mp2eldec.h
// AUTHOR:    U. Sigmund, S. Herr
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:
//
// PURPOSE:   MPEG2 elementary video/audio decoders and supporting classes.
//
// HISTORY:

#ifndef MP2ELDEC_H
#define MP2ELDEC_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"
#include "library/hardware/mpegdec/generic/mpegdec.h"
#include "library/hardware/audio/generic/ac3setup.h"
#include "library/hardware/audio/generic/karaokesetup.h"
#include "library/hardware/video/generic/ccapvenc.h"
#include "library/hardware/mpeg2dec/generic/MP2StreamReceiver.h"

enum SPUButtonState
	{
	SBS_DISABLED,
	SBS_SELECTED,
	SBS_ACTIVATED
	};

struct MPEG2SPUCommandMsg {
	DWORD		pts;
	int		command;
	};

MKHOOK(MPEG2SPUCommand, MPEG2SPUCommandMsg)

typedef MPEG2SPUCommandHook	__far *	MPEG2SPUCommandHookPtr;

//
//  Error definitions
//

#define GNR_DISPLAY_MODE_NOT_SUPPORTED					MKERR(ERROR, MPEG, FORMAT, 0x00)
// The display mode could not be set

#define GNR_AUDIO_TYPE_NOT_SUPPORTED					MKERR(ERROR, MPEG, FORMAT, 0x01)
// The audio format is not supported

#define GNR_NO_SPDIF_HARDWARE								MKERR(ERROR, MPEG, HARDWARE, 0x02)
// We do not SPDIF hardware

#define GNR_FRAME_ADVANCE_BOUNDARY_REACHED			MKERR(WARNING, MPEG, BOUNDS, 0x03)

enum MPEG2PresentationMode
	{
	MPM_FULLSIZE,
	MPM_LETTERBOXED,
	MPM_PANSCAN,
	MPM_FULLSIZE16by9
	};

enum SPDIFHandling
	{
	SPDIFH_DEFAULT,			// On when playback is running, off in all other cases
	SPDIFH_ON,					// Always physically on
	SPDIFH_OFF					// Always physically off (line idle)
	};

// This can be specified for each audio type
enum SPDIFOutputMode
	{
	SPDIFOM_DEFAULT,			// AC3: compressed, LPCM: decompressed, idle: NULL output
	SPDIFOM_DECOMPRESSED,	// Force decompressed output, even for AC3
	SPDIFOM_MUTE_NULL,		// Force mute with NULL output, if SPDIF_ON
	SPDIFOM_OFF					// Physically off
	};

enum SPDIFCopyMode
	{
	SPDIFCM_DEFAULT,			// Use system's default (transfer rights from source material to output)
	SPDIFCM_NO_COPIES,		// do not allow any copies
	SPDIFCM_ONE_GENERATION,	// allow one generation of copies
	SPDIFCM_UNRESTRICTED		// no copy restrictions
	};

enum MPEG2SplitStreamType
	{
	MP2SST_DVD_ENCRYPTED,
	MP2SST_PROGRAM,
	MP2SST_PES,
	MP2SST_ELEMENTARY
	};

// Basic Audio Source Type
enum MPEG2AudioType
	{
	MP2AUDTYP_DEFAULT,		// means: use MPEG2_AUDIO_AC3 and MPEG2_AUDIO_LPCM tags to determine audio type
	MP2AUDTYP_MPEG,			// MPEG(-1) all layers
	MP2AUDTYP_MPEG2,
	MP2AUDTYP_AC3,
	MP2AUDTYP_LPCM,			// includes CDDA (set # of bits and sample rate accordingly)
	MP2AUDTYP_DTS,
	MP2AUDTYP_SDDS,
	MP2AUDTYP_DTS_CDDA,		// CDDA with DTS information
	MP2AUDTYP_MLP,

	MP2AUDTYP_NUMBER_OF_TYPES
	};

// Virtual Surround ("Spatializer") modes
enum MPEG2AudioSpatializer
	{
	MP2AUDSPAT_DEFAULT,
	MP2AUDSPAT_NONE = MP2AUDSPAT_DEFAULT,
	MP2AUDSPAT_SRS_TS_3D,			// SRS True Surround or 3D sound, depending on source material
	MP2AUDSPAT_VMAX_TS_3D		// VMAX True Surround or 3D sound, depending on source material
	};

// PCM Output Configurations (= Bass Redirection Schemes)
enum PCMOutputConfig
	{
	PCMOCFG_DEFAULT,	// ALL, scaled, is default
	PCMOCFG_ALL = PCMOCFG_DEFAULT,
	PCMOCFG_LSW,
	PCMOCFG_LLR,
	PCMOCFG_SLP,
	PCMOCFG_SUM,		// subwoofer = sum of all input channels
	PCMOCFG_BYP,		// Bypass
	PCMOCFG_LSW_WF    // same as config 1 without filters
	};

// Downsample mode for 96kHz LPCM
enum PCM96DownsampleMode
	{
	PCM96DWNS_DEFAULT,
	PCM96DWNS_DOWN_48KHZ = PCM96DWNS_DEFAULT,	// sample down to 48kHz by default
	PCM96DWNS_NONE										// no downsampling, play back using 96kHz
	};


// Program Format Flags for tag MPEG2_AUDIO_PROGRAM_FORMAT
#define AUDPF_PROLOGIC			MKFLAG(0)	// Indicates if Pro Logic Decoding is active or not
#define AUDPF_LEFT				MKFLAG(1)	// Shows if program contains
#define AUDPF_RIGHT				MKFLAG(2)
#define AUDPF_CENTER				MKFLAG(3)
#define AUDPF_LFE					MKFLAG(4)
#define AUDPF_LEFT_SURROUND	MKFLAG(5)
#define AUDPF_RIGHT_SURROUND	MKFLAG(6)
#define AUDPF_MONO_SURROUND	MKFLAG(7)


//
//  CDDA Data Format
//  Describes formats supported by decoder
//

#define CDDADF_AUDIO_ONLY			MKFLAG(0)			// Only audio data (corresponds to LPCM, 2 ch, 16 Bit, 44.1 kHz), default
#define CDDADF_AUDIO_SUBCHANNEL	MKFLAG(1)			// For each block first audio data (2352 bytes), then subchannel (98 bytes)
#define CDDADF_SUBCHANNEL_AUDIO	MKFLAG(2)			// For each block first subchannel (98 bytes), then audio data (2352 bytes)
#define CDDADF_DEFAULT				CDDADF_AUDIO_ONLY

//
//  LPCM Data Format (Endianess)
//

enum LPCMDataFormat
	{
	LDF_BIG_ENDIAN,
	LDF_LITTLE_ENDIAN
	};

//
//  Audio Type Config
//

struct AudioTypeConfig
	{
	union
		{
		struct
			{
			SPDIFOutputMode	spdifOutMode : 3;
			// ...can be expanded up to size of a DWORD...
			} config;

		DWORD	dummy;
		};

	friend BOOL operator==(const AudioTypeConfig a, const AudioTypeConfig b);
	friend BOOL operator!=(const AudioTypeConfig a, const AudioTypeConfig b);

	operator DWORD (void) {return dummy;}

	AudioTypeConfig(DWORD arg) {dummy = arg;}
	AudioTypeConfig(void) {dummy = 0;}
	AudioTypeConfig(SPDIFOutputMode initMode)
		{
		config.spdifOutMode = initMode;
		// ...
		}
	};

inline BOOL operator==(const AudioTypeConfig a, const AudioTypeConfig b)
	{
	return (a.dummy == b.dummy);
	}

//static inline BOOL operator!=(const AudioTypeConfig a, const AudioTypeConfig b)
inline BOOL operator!=(const AudioTypeConfig a, const AudioTypeConfig b)
	{
	return (a.dummy != b.dummy);
	}

#if WDM_VERSION

struct MPEG2CCPacketMsg
	{
	BOOL startField;
	BYTE * data;
	int num;
	};

MKHOOK(MPEG2CCPacket, MPEG2CCPacketMsg)

typedef MPEG2CCPacketHook * MPEG2CCPacketHookPtr;

#endif	// WDM_VERSION

enum ForcedAspectRatio
	{
	FORCED_AR_DEFAULT	= 0,
	// 1 left out. Do not change the following two assignments:
	FORCED_AR_4_BY_3	= 2,
	FORCED_AR_16_BY_9	= 3
	};

enum FrameRateValue
	{
	FRV_23976 = 1,
	FRV_24000 = 2,
	FRV_25000 = 3,
	FRV_29970 = 4,
	FRV_30000 = 5,
	FRV_50000 = 6,
	FRV_59940 = 7,
	FRV_60000 = 8
	};


// Bits for audio speaker test bitfield (MPEG2_AUDIO_TEST_SPEAKER_SELECT)
#define AUDIO_TEST_LEFT_SPEAKER				0x01
#define AUDIO_TEST_RIGHT_SPEAKER				0x02
#define AUDIO_TEST_CENTER_SPEAKER			0x04
#define AUDIO_TEST_SUBWOOFER_SPEAKER		0x08
#define AUDIO_TEST_LEFT_SURROUND_SPEAKER	0x10
#define AUDIO_TEST_RIGHT_SURROUND_SPEAKER	0x20



MKTAG(MPEG2_AUDIO_AC3,						MPEG_DECODER_UNIT,	0x1001, BOOL)
MKTAG(MPEG2_AUDIO_AC3_STREAMID,			MPEG_DECODER_UNIT,	0x1002, BYTE)
MKTAG(MPEG2_CODING_STANDARD,	  		   MPEG_DECODER_UNIT,	0x1003, BOOL)

MKTAG(MPEG2_AC3_BASS_CONFIG,				MPEG_DECODER_UNIT,	0x1004, AC3BassConfig)
MKTAG(MPEG2_AC3_SPEAKER_CONFIG,			MPEG_DECODER_UNIT,	0x1005, AC3SpeakerConfig)
MKTAG(MPEG2_AC3_CENTER_DELAY,				MPEG_DECODER_UNIT,	0x1006, WORD)
MKTAG(MPEG2_AC3_SURROUND_DELAY,			MPEG_DECODER_UNIT,	0x1007, WORD)

MKTAG(MPEG2_AC3_CENTER_VOLUME,			MPEG_DECODER_UNIT,	0x1008, WORD)
MKTAG(MPEG2_AC3_LEFT_SURROUND_VOLUME,  MPEG_DECODER_UNIT,	0x1009, WORD)
MKTAG(MPEG2_AC3_RIGHT_SURROUND_VOLUME,	MPEG_DECODER_UNIT,	0x100a, WORD)
MKTAG(MPEG2_AC3_SUBWOOFER_VOLUME,		MPEG_DECODER_UNIT,	0x100b, WORD)

MKTAG(MPEG2_AC3_DUAL_MODE_CONFIG,		MPEG_DECODER_UNIT,	0x100c, AC3DualModeConfig)
MKTAG(MPEG2_AC3_KARAOKE_CONFIG,			MPEG_DECODER_UNIT,	0x100d, AC3KaraokeConfig)

MKTAG(MPEG2_SPU_STREAMID,					MPEG_DECODER_UNIT,	0x1010, BYTE)
MKTAG(MPEG2_SPU_ENABLE,						MPEG_DECODER_UNIT,	0x1011, BOOL)

MKTAG(MPEG2_SPU_BUTTON_STATE,				MPEG_DECODER_UNIT,	0x1012, SPUButtonState)
MKTAG(MPEG2_SPU_BUTTON_LEFT,				MPEG_DECODER_UNIT,	0x1013, WORD)
MKTAG(MPEG2_SPU_BUTTON_TOP,				MPEG_DECODER_UNIT,	0x1014, WORD)
MKTAG(MPEG2_SPU_BUTTON_WIDTH,				MPEG_DECODER_UNIT,	0x1015, WORD)
MKTAG(MPEG2_SPU_BUTTON_HEIGHT,			MPEG_DECODER_UNIT,	0x1016, WORD)
MKTAG(MPEG2_SPU_BUTTON_SELECT_COLOR,	MPEG_DECODER_UNIT,	0x1017, DWORD)
MKTAG(MPEG2_SPU_BUTTON_ACTIVE_COLOR,	MPEG_DECODER_UNIT,	0x1018, DWORD)
MKTAG(MPEG2_SPU_PALETTE_ENTRY,			MPEG_DECODER_UNIT,	0x1019, DWORD)
	// Palette Bits 0..7 Pen, 8..15 V, 16..23 U, 24..31 Y
MKTAG(MPEG2_SPU_COMMAND_HOOK,				MPEG_DECODER_UNIT,	0x101a, MPEG2SPUCommandHookPtr)
MKTAG(MPEG2_SPU_BUTTON_ID,					MPEG_DECODER_UNIT,	0x101b, WORD)
MKTAG(MPEG2_SPU_BUTTON_STARTTIME,		MPEG_DECODER_UNIT,	0x101c, DWORD)
MKTAG(MPEG2_SPU_BUTTON_ENDTIME,			MPEG_DECODER_UNIT,	0x101d, DWORD)

MKTAG(MPEG2_AUDIO_LPCM,						MPEG_DECODER_UNIT,	0x1021, BOOL)
MKTAG(MPEG2_AUDIO_LPCM_STREAMID,			MPEG_DECODER_UNIT,	0x1022, BYTE)
MKTAG(MPEG2_LPCM_BITSPERSAMPLE,			MPEG_DECODER_UNIT,	0x1023, WORD)
MKTAG(MPEG2_LPCM_CHANNELS,					MPEG_DECODER_UNIT,	0x1024, WORD)

MKTAG(MPEG2_AUDIO_DTS_STREAMID,			MPEG_DECODER_UNIT,	0x1028, BYTE)
MKTAG(MPEG2_AUDIO_MLP_STREAMID,			MPEG_DECODER_UNIT,	0x1029, BYTE)

MKTAG(MPEG2_PRESENTATION_MODE,			MPEG_DECODER_UNIT,	0x1030, MPEG2PresentationMode)

MKTAG(MPEG2_DVD_STREAM_DEMUX,				MPEG_DECODER_UNIT,	0x1031, BOOL)
MKTAG(MPEG2_DVD_STREAM_ENCRYPTED,		MPEG_DECODER_UNIT,	0x1032, BOOL)

#if WDM_VERSION
// Do not use this tag from user mode!
MKTAG(MPEG2_CC_PACKET_HOOK,				MPEG_DECODER_UNIT,	0x1033, MPEG2CCPacketHookPtr)
#endif

// Use this for overriding the default handling of switching ON/OFF SPDIF output
// (on decoders that can control SPDIF out)
MKTAG(MPEG2_SPDIF_HANDLING,				MPEG_DECODER_UNIT,	0x1034, SPDIFHandling)
MKTAG(MPEG2_SPDIF_OUTPUT_MODE,			MPEG_DECODER_UNIT,	0x1035, SPDIFOutputMode)
MKTAG(MPEG2_SPDIF_COPY_MODE,				MPEG_DECODER_UNIT,	0x1036, SPDIFCopyMode)

// This supercedes tags MPEG2_AUDIO_AC3 and MPEG2_AUDIO_LPCM, if available
MKTAG(MPEG2_AUDIO_TYPE,						MPEG_DECODER_UNIT,	0x103a, MPEG2AudioType)

// Specifies audio spatializing algorithm (e.g. SRS True Surround)
MKTAG(MPEG2_AUDIO_SPATIALIZER,			MPEG_DECODER_UNIT,	0x103b, MPEG2AudioSpatializer)

MKTAG(MPEG2_POSITION_SCALE,				MPEG_DECODER_UNIT,	0x1040, DWORD)

// Tags for downscaling the MPEG image and positioning it at an offset.
MKTAG(MPEG2_DOWNSCALE_FACTOR,				MPEG_DECODER_UNIT,	0x1050, int)
MKTAG(MPEG2_DOWNSCALE_HOFFSET,			MPEG_DECODER_UNIT,	0x1051, int)
MKTAG(MPEG2_DOWNSCALE_VOFFSET,			MPEG_DECODER_UNIT,	0x1052, int)
MKTAG(MPEG2_DOWNSCALE_HOFFSET_LBOXED,	MPEG_DECODER_UNIT,	0x1053, int)
MKTAG(MPEG2_DOWNSCALE_VOFFSET_LBOXED,	MPEG_DECODER_UNIT,	0x1054, int)

// New smooth zoom interface:
MKTAG(MPEG2_ZOOM_FACTOR,					MPEG_DECODER_UNIT,	0x1060, int)
MKTAG(MPEG2_ZOOM_SPEED,						MPEG_DECODER_UNIT,	0x1061, int)
MKTAG(MPEG2_ZOOM_ACTION,					MPEG_DECODER_UNIT,	0x1062, int)
MKTAG(MPEG2_ZOOM_HOFFSET,					MPEG_DECODER_UNIT,	0x1063, int)
MKTAG(MPEG2_HOFFSET_SPEED,					MPEG_DECODER_UNIT,	0x1064, int)
MKTAG(MPEG2_HOFFSET_ACTION,				MPEG_DECODER_UNIT,	0x1065, int)
MKTAG(MPEG2_ZOOM_VOFFSET,					MPEG_DECODER_UNIT,	0x1066, int)
MKTAG(MPEG2_VOFFSET_SPEED,					MPEG_DECODER_UNIT,	0x1067, int)
MKTAG(MPEG2_VOFFSET_ACTION,				MPEG_DECODER_UNIT,	0x1068, int)
MKTAG(MPEG2_PIP_HOFFSET,					MPEG_DECODER_UNIT,	0x1069, int)
MKTAG(MPEG2_PIP_VOFFSET,					MPEG_DECODER_UNIT,	0x1070, int)


MKTAG(MPEG2_AC3_EQUALIZER_FRONT_ENABLE,MPEG_DECODER_UNIT,	0x1100, BOOL)
MKTAG(MPEG2_AC3_EQUALIZER_BACK_ENABLE,	MPEG_DECODER_UNIT,	0x1101, BOOL)
MKTAG(MPEG2_AC3_EQUALIZER_FRONT,			MPEG_DECODER_UNIT,	0x1104, DWORD)
MKTAG(MPEG2_AC3_EQUALIZER_BACK,			MPEG_DECODER_UNIT,	0x1105, DWORD)
MKTAG(MPEG2_STILL_FRAME_SEQUENCE,		MPEG_DECODER_UNIT,	0x1106, BOOL)
MKTAG(MPEG2_CURRENT_PLAYBACK_TIME,		MPEG_DECODER_UNIT,	0x1107, LONG)
MKTAG(MPEG2_AC3_VIRTUAL_3D_AUDIO,		MPEG_DECODER_UNIT,	0x1108, BOOL)

// Tags for split stream playback configuration
MKTAG(MPEG2_VIDEO_SPLIT_STREAM_TYPE,	MPEG_DECODER_UNIT,	0x1109, MPEG2SplitStreamType)
MKTAG(MPEG2_AUDIO_SPLIT_STREAM_TYPE,	MPEG_DECODER_UNIT,	0x1110, MPEG2SplitStreamType)
MKTAG(MPEG2_SPU_SPLIT_STREAM_TYPE,		MPEG_DECODER_UNIT,	0x1111, MPEG2SplitStreamType)

MKTAG(MPEG2_FORCED_SOURCE_ASPECT_RATIO, MPEG_DECODER_UNIT,	0x1120, ForcedAspectRatio)
// Switch to WDM playback (meaning: no navpacks in DVD data streams)
MKTAG(MPEG2_WDM_PLAYBACK,					MPEG_DECODER_UNIT,	0x1122, BOOL)
// Returns current value of the decoder's STC
MKTAG(MPEG2_CURRENT_STC,					MPEG_DECODER_UNIT,	0x1123, LONG)

MKTAG(MPEG2_REQUESTED_PLAYBACK_TIME,   MPEG_DECODER_UNIT,   0x1130, LONG)

// Closed Caption output on/off
MKTAG(MPEG2_CC_ENABLE,						MPEG_DECODER_UNIT,	0x1140, BOOL)

// Additional delay tags
MKTAG(MPEG2_AC3_LEFT_DELAY,				MPEG_DECODER_UNIT,	0x1200, WORD)
MKTAG(MPEG2_AC3_RIGHT_DELAY,				MPEG_DECODER_UNIT,	0x1201, WORD)
MKTAG(MPEG2_AC3_LEFT_SURROUND_DELAY,	MPEG_DECODER_UNIT,	0x1202, WORD)
MKTAG(MPEG2_AC3_RIGHT_SURROUND_DELAY,	MPEG_DECODER_UNIT,	0x1203, WORD)
MKTAG(MPEG2_AC3_SUBWOOFER_DELAY,			MPEG_DECODER_UNIT,	0x1204, WORD)

// Dolby Pro Logic Decoder config and mode
MKTAG(MPEG2_PRO_LOGIC_CONFIG,				MPEG_DECODER_UNIT,	0x1210, DolbyProLogicConfig)
MKTAG(MPEG2_PRO_LOGIC_MODE,				MPEG_DECODER_UNIT,	0x1211, DolbyProLogicMode)

// Low Frequency Enable (LFE). Applicable for AC3, MPEG2-MC and DTS stream playback
MKTAG(MPEG2_DECODE_LFE,						MPEG_DECODER_UNIT,	0x1220, BOOL)

// GET only tag that returns whether a LFE channel is present (and being decoded) or not
MKTAG(MPEG2_LFE_STATUS,						MPEG_DECODER_UNIT,	0x1221, BOOL)

// AC3 Audio Coding Mode (acmod property). GET only.
MKTAG(MPEG2_AC3_AUDIO_CODING_MODE,		MPEG_DECODER_UNIT,	0x1225, AC3AudioCodingMode)

// Returns TRUE if Pro Logic decoding is active
MKTAG(MPEG2_PROLOGIC_STATUS,				MPEG_DECODER_UNIT,	0x1226, BOOL)

// PCM output configuration
MKTAG(MPEG2_PCM_OUTPUT_CONFIG,			MPEG_DECODER_UNIT,	0x1230, PCMOutputConfig)

// Dynamic range
MKTAG(MPEG2_AC3_DYNAMIC_RANGE,			MPEG_DECODER_UNIT,	0x1236, AC3DynamicRange)
MKTAG(MPEG2_AC3_HIGH_DYNAMIC_RANGE,		MPEG_DECODER_UNIT,	0x1237, WORD)
MKTAG(MPEG2_AC3_LOW_DYNAMIC_RANGE,		MPEG_DECODER_UNIT,	0x1238, WORD)

MKTAG(MPEG2_AC3_DIALOG_NORM,			MPEG_DECODER_UNIT,	0x1239, AC3DialogNorm)
MKTAG(MPEG2_AC3_OPERATIONAL_MODE,		MPEG_DECODER_UNIT,	0x123a, AC3OperationalMode)

// Audio type specific configurations
MKTAG(MPEG2_AC3_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1240, AudioTypeConfig)
MKTAG(MPEG2_LPCM_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1241, AudioTypeConfig)
MKTAG(MPEG2_DTS_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1242, AudioTypeConfig)
MKTAG(MPEG2_MPEG_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1243, AudioTypeConfig)
MKTAG(MPEG2_MPEG2_AUDIO_TYPE_CONFIG,	MPEG_DECODER_UNIT,	0x1244, AudioTypeConfig)
MKTAG(MPEG2_SDDS_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1245, AudioTypeConfig)
MKTAG(MPEG2_CDDA_DTS_AUDIO_TYPE_CONFIG,MPEG_DECODER_UNIT,	0x1246, AudioTypeConfig)
MKTAG(MPEG2_MLP_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1247, AudioTypeConfig)

MKTAG(MPEG2_PCM96_DOWNSAMPLE_MODE_DAC,		MPEG_DECODER_UNIT,	0x1300, PCM96DownsampleMode)
MKTAG(MPEG2_PCM96_DOWNSAMPLE_MODE_SPDIF,	MPEG_DECODER_UNIT,	0x1301, PCM96DownsampleMode)

MKTAG(MPEG2_VIDEO_MIN_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1330, WORD)
MKTAG(MPEG2_VIDEO_MAX_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1331, WORD)

MKTAG(MPEG2_AUDIO_MIN_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1334, WORD)
MKTAG(MPEG2_AUDIO_MAX_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1335, WORD)

MKTAG(MPEG2_VIDEO_MIN_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1340, WORD)
MKTAG(MPEG2_VIDEO_MAX_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1341, WORD)

MKTAG(MPEG2_AUDIO_MIN_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1344, WORD)
MKTAG(MPEG2_AUDIO_MAX_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1345, WORD)

// Use this tag to damp volume during scan modes
MKTAG(MPEG2_AUDIO_VOLUME_DAMP,			MPEG_DECODER_UNIT,	0x1346, BOOL)

// Speaker selection for audio tests (e.g. pink noise test). Bitfield defined above.
MKTAG(MPEG2_AUDIO_TEST_SPEAKER_SELECT,	MPEG_DECODER_UNIT,	0x1350, WORD)

// Data format for CDDA decoder, should be Get/Set
MKTAG(CDDA_DATA_FORMAT,						MPEG_DECODER_UNIT,	0x1400, DWORD)

// Endianess of LPCM decoder (Qry/Get)
MKTAG(LPCM_DATA_FORMAT,						MPEG_DECODER_UNIT,	0x1401, LPCMDataFormat)

MKTAG(MPEG2_TIMEOUT_MSG,					MPEG_DECODER_UNIT,	0x1500, WORD)
MKTAG(MPEG2_REFILL_MSG,						MPEG_DECODER_UNIT,	0x1501, WORD)
MKTAG(MPEG2_DONE_MSG,						MPEG_DECODER_UNIT,	0x1502, WORD)
MKTAG(MPEG2_SIGNAL_MSG,						MPEG_DECODER_UNIT,	0x1503, WORD)
MKTAG(MPEG2_ERROR_MSG,						MPEG_DECODER_UNIT,	0x1504, WORD)

// Karaoke control tags
#define KARAOKE_TAGVALUE_BASE  0x2000

MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE,								MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  0, BOOL)
MKTAG(MPEG2_AUDIO_KARAOKE_LEFT_CHANNEL_MUSIC_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  1, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_RIGHT_CHANNEL_MUSIC_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  2, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_MUSIC_CHANNEL_MUTE,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  3, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_LEFT_CHANNEL_VOICE_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  4, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_RIGHT_CHANNEL_VOICE_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  5, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_VOICE_CHANNEL_MUTE,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  6, BOOL)


MKTAG(MPEG2_AUDIO_KARAOKE_PITCH_SHIFT,							MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  7, int)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_PITCH_SHIFT,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  8, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_CANCELLATION,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  9, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_VOICE_CANCELLATION,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 10, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_DUET_THRESHOLD,						MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 11, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_DUET,							MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 12, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_EFFECT,						MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 13, KaraokeVoiceEffectType )
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_EFFECT_DELAY,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 14, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_EFFECT_BALANCE,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 15, WORD)

MKTAG(MPEG2_AUDIO_KARAOKE_MUSIC_GAIN,							MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 16, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_GAIN,							MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 17, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_MUSIC_IN_SHIFT, 					MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 18, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_MUSIC_OUT_SHIFT,					MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 19, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_IN_SHIFT, 					MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 20, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_OUT_SHIFT, 					MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 21, WORD)

MKTAG(MPEG2_AUDIO_KARAOKE_ECHO_FEEDBACK,						MPEG_DECODER_UNIT,  KARAOKE_TAGVALUE_BASE + 22, WORD)



#ifndef ONLY_EXTERNAL_VISIBLE

#include "library/common/virtunit.h"
#include "library/general/queues.h"
#include "library/common/krnlint.h"

#define INVALID_PTS	0xffffffff   // must be largest DWORD

class PTSAssoc {
	private:
		struct Assoc {DWORD pos, pts;} * assoc;
		int first;
		int last;
		int num;
	public:
		PTSAssoc(int num);
		~PTSAssoc(void);

		void PutPTS(DWORD pos, DWORD pts);
		DWORD GetPTS(DWORD pos);
		DWORD GetAvgPTS(DWORD pos);
		void Reset(void);
	};

class PTSCallback : Node
	{
	friend class PTSCaller;
	private:
		DWORD	callPTS;
		BOOL	needSequenceStartCallback;

		BOOL HigherPriorityThan(Node * n) {return callPTS < ((PTSCallback *)n)->callPTS;}
	public:
		PTSCallback(void) {callPTS = INVALID_PTS; needSequenceStartCallback = FALSE;}

		virtual Error PTSCallbackReached(DWORD currentPTS) = 0;
		virtual Error PTSCallbackCanceled(void);

		virtual Error SequenceStartReached (void);
	};

class PTSCaller
	{
	private:
		DWORD	lastPTS;

		List	callbacks;
	public:
		PTSCaller(void) {lastPTS = 0;}

		Error AddCallback(DWORD pts, PTSCallback * callback);
		Error RemCallback(PTSCallback * callback);

		Error CheckCallbacks(DWORD currentPTS);
		Error CancelCallbacks(void);

		Error AddSequenceStartCallback (PTSCallback *callback);
		Error RemSequenceStartCallback (PTSCallback *callback);
		Error CallSequenceStartCallbacks (void);
	};

class PTSCounter
	{
	private:
		BOOL running;

		long ptsDelay;

		DWORD basePTS;

		DWORD baseTimeOffset;
		DWORD stopTimeOffset;

		WORD playbackSpeed;

	public:
		PTSCounter(void);
		~PTSCounter(void);

		Error Reset(void);
		Error SetDecoderDelay(long delay); // in milliseconds

		Error Start (WORD playbackSpeed = 1000);
		Error Stop(void);

		Error SyncPTS(DWORD pts);
		Error SetPTS(DWORD pts);
		DWORD GetPTS(void);
	};


class MPEG2CommandQueue
	{
	private:
		struct {MPEGCommand com; DWORD param;} queue[16];
		DWORD	first, last, next;
	public:
		MPEG2CommandQueue(void) {last = 0; Reset();}

		void Reset(void) {first = next = last;}

		BOOL CommandPending(DWORD tag) {if (tag != 0xffffffff) return first <= tag; else return FALSE;}

		Error InsertCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL PeekCommand(MPEGCommand __far &com, DWORD __far &param);
		void SkipCommand(void) {first++; next++;}
		BOOL GetCommand(MPEGCommand __far &com, DWORD __far &param);
		Error CompleteCommand(void);
	};


class MPEG2ElementaryDecoder : public PhysicalUnit, public MPEG2StreamReceiver
   {
   friend class VirtualMPEG2ElementaryDecoder;
   protected:
   	DWORD		streamBitrate;
   	DWORD		positionScale, positionBaseScale;

      BOOL		dvdStreamDemux;
      BOOL		dvdStreamEncrypted;

		Error StreamHook(DWORD size);
   	Error SignalHook(DWORD position);
   	Error SignalHook(KernelInt64 position);
   	Error DoneHook(void);
		Error IdleHook(void);

		virtual DWORD RecoverStreamData(HPTR data, DWORD size) {return 0;}
		virtual DWORD StreamDataAvailable(void) {return 0;}
   public:
		MPEG2ElementaryDecoder	*	syncSlave;
      BOOL								slaveMode;

   	MPEG2ElementaryDecoder(void) {positionScale = positionBaseScale = MP2SR_SCALE_BYTES; syncSlave = NULL; slaveMode = FALSE;}

	   virtual Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag) = 0;
		virtual BOOL CommandPending(DWORD tag) = 0;
      virtual Error DoCommand(MPEGCommand com, DWORD param);
		virtual Error CompleteCommand(DWORD tag);

		virtual DWORD CurrentLocation(void) = 0;
		virtual MPEGState CurrentState(void) = 0;

	   virtual DWORD GetPTS() = 0;
		virtual Error ConformToPTS(DWORD pts) {GNRAISE_OK;}
		virtual Error SetPlaybackTime(LONG time) {GNRAISE_OK;}

	   virtual Error SetSignalPosition(DWORD position) = 0;
	   virtual Error SetStreamBitRate(DWORD rate) {streamBitrate = rate; GNRAISE_OK;}
	   virtual Error SetPositionScale(DWORD scale) {positionScale = scale; GNRAISE_OK;}

   	virtual Error SetDVDStreamDemux(BOOL dvdDemux) {this->dvdStreamDemux = dvdDemux; GNRAISE_OK;}
   	virtual Error SetDVDStreamEncrypted(BOOL dvdEncrypted) {this->dvdStreamEncrypted = dvdEncrypted; GNRAISE_OK;}

		virtual Error SyncPlay(void) {GNRAISE_OK;}

		virtual Error NotifySiblingBufferFull(void) {GNRAISE_OK;}
		virtual Error NotifyTransferStopped(void) {GNRAISE_OK;}
		virtual Error VOBUBorderReached(void) {GNRAISE_OK;}
   };

class VirtualMPEG2ElementaryDecoder : public VirtualUnit, public MPEG2StreamReceiver
	{
	friend class MPEG2ElementaryDecoder;
	private:
		MPEG2ElementaryDecoder	*	decoder;
	protected:
#if !DRVDLL
		MPEGStreamHookHandle	streamHook;
		MPEGSignalHookHandle	signalHook;
		MPEGSignalHookHandle	doneHook;
	   MPEGIdleHookHandle	idleHook;
#else
		MPEGStreamHook	* streamHook;
		MPEGSignalHook	* signalHook;
		MPEGSignalHook	* doneHook;
	   MPEGIdleHook	* idleHook;
#endif

   	DWORD		streamBitrate;
		DWORD		signalPosition;
   	DWORD		positionScale;

		HBPTR		buffer;
		DWORD		buffStart, buffEnd, buffSize;
		DWORD		streamPosition;
		LONG		playbackTime;

		BOOL							dvdStreamDemux;
		BOOL							dvdStreamEncrypted;

		FixedDualIntQueue	ptsQueue;

      virtual Error RecoverStreamData(void);
      virtual Error ProvideStreamData(void);

      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);
	public:
		VirtualMPEG2ElementaryDecoder(MPEG2ElementaryDecoder * unit);
		~VirtualMPEG2ElementaryDecoder(void);

      virtual Error AllocateStreamBuffer(DWORD size);
		virtual Error FlushStreamBuffer(void);

		Error Configure (TAG __far *tags);

	   virtual Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		virtual BOOL CommandPending(DWORD tag);
      virtual Error DoCommand(MPEGCommand com, DWORD param);
		virtual Error CompleteCommand(DWORD tag);

		virtual DWORD CurrentLocation(void);
		virtual MPEGState CurrentState(void);

	   virtual DWORD SendData(HPTR data, DWORD size);
	   virtual void CompleteData(void);
		virtual DWORD LastTransferLocation(DWORD scale);

	   virtual Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);

	   virtual DWORD GetPTS();
	   virtual void PutPTS(DWORD pts);
		virtual Error ConformToPTS(DWORD pts);

		virtual Error NotifySiblingBufferFull(void);
		virtual Error NotifyTransferStopped(void);
		virtual Error VOBUBorderReached(void);
	};


class MPEG2VideoDecoder : public MPEG2ElementaryDecoder
	{
	friend class VirtualMPEG2VideoDecoder;
	protected:
		virtual Error SetVideoBitrate(DWORD videoBitrate) = 0;
		virtual Error SetVideoWidth(WORD width) = 0;
		virtual Error SetVideoHeight(WORD height) = 0;
		virtual Error SetVideoStandard(VideoStandard	standard) = 0;
#if TEST_PAL_TO_NTSC
		virtual Error SetApplicationVideoStandard(VideoStandard	standard) = 0;	//aa3
		virtual MovingImageStandard GetEncoderMovingImageStandard () = 0;
#endif
		virtual Error SetVideoFPS(WORD fps) = 0;
		virtual Error SetMPEG2Coding(BOOL mpeg2Coding) = 0;
		virtual Error SetAspectRatio(WORD aspectRatio) = 0;
		virtual Error SetPresentationMode(MPEG2PresentationMode presentationMode) = 0;

	public:

	};

class VirtualMPEG2VideoDecoder : public VirtualMPEG2ElementaryDecoder
	{
	private:
		MPEG2VideoDecoder	*		decoder;
	protected:
		DWORD							videoBitrate;
		WORD							videoWidth;
		WORD							videoHeight;
		WORD							videoFPS;
		WORD							aspectRatio;
		VideoStandard				videoStandard;
#if TEST_PAL_TO_NTSC
		VideoStandard				diskVideoStandard;
		VideoStandard				applicationVideoStandard;
		MovingImageStandard		decoderMovingImageStandard;
#endif
		BOOL							mpeg2Coding;
		MPEG2PresentationMode	presentationMode;

		virtual Error GetDisplaySize(WORD __far& width, WORD __far& height);

		Error PreemptChange(VirtualUnit * previous);
	public:
		VirtualMPEG2VideoDecoder(MPEG2VideoDecoder * unit) : VirtualMPEG2ElementaryDecoder(unit)
			{decoder = unit; videoBitrate = 0; videoWidth = 0; videoHeight = 0;
			 videoFPS = 0; mpeg2Coding = FALSE;
			 presentationMode = MPM_FULLSIZE;
#if TEST_PAL_TO_NTSC

			applicationVideoStandard = VSTD_PAL;
#endif
			}

		Error Configure(TAG __far * tags);

		virtual Error SetLine21Receiver(Line21Receiver * line21Receiver);
	};

class MPEG2AudioDecoder : public MPEG2ElementaryDecoder
	{
	public:
		virtual Error SetAudioBitrate(DWORD rate) = 0;
		virtual Error SetSampleRate(WORD rate) = 0; // in samples per second;
		virtual Error SetLeftVolume(WORD volume) = 0;
		virtual Error SetRightVolume(WORD volume) = 0;
		virtual Error SetMute(BOOL mute) = 0;
		virtual Error SetAudioType(MPEG2AudioType audioType) {GNRAISE_OK;}
		virtual Error SetAudioLayer(MPEGAudioLayer audioLayer) {GNRAISE_OK;}
		virtual Error SetLPCM(BOOL lpcm) {GNRAISE_OK;}
		virtual Error SetBitsPerSample(WORD samples) {GNRAISE_OK;}
		virtual Error SetChannels(WORD channels) {GNRAISE_OK;}
	};

class VirtualMPEG2AudioDecoder : public VirtualMPEG2ElementaryDecoder
	{
	private:
		MPEG2AudioDecoder	* decoder;
	protected:
		DWORD	audioBitrate;
		WORD	sampleRate;
		WORD	leftVolume;
		WORD	rightVolume;
		BOOL	audioMute;
		WORD	bitsPerSample;
		WORD	channels;
		BOOL	lpcm;

		MPEGAudioLayer	audioLayer;
		MPEG2AudioType	audioType;

		Error PreemptChange(VirtualUnit * previous);
	public:
		VirtualMPEG2AudioDecoder(MPEG2AudioDecoder	* decoder) : VirtualMPEG2ElementaryDecoder(decoder)
			{this->decoder = decoder;audioBitrate = 0; sampleRate = 48000;
			 leftVolume = 10000; rightVolume = 10000; audioMute = TRUE;
			 audioType = MP2AUDTYP_DEFAULT; lpcm = FALSE; bitsPerSample = 16; channels = 2; audioLayer = MPAUDLAYER_2;
			 }

		Error Configure (TAG __far *tags);
	};

class MPEG2AC3Decoder : public MPEG2AudioDecoder
	{
	public:
		virtual Error SetAC3(BOOL ac3) = 0;

		virtual Error SetBassConfig(AC3BassConfig config) = 0;
		virtual Error SetSpeakerConfig(AC3SpeakerConfig config) = 0;
		virtual Error SetDualModeConfig(AC3DualModeConfig config) = 0;
		virtual Error SetKaraokeConfig(AC3KaraokeConfig config) = 0;
		virtual Error SetCenterDelay(WORD delay) = 0;
		virtual Error SetSurroundDelay(WORD delay) = 0;
		virtual Error SetCenterVolume(WORD volume) = 0;
		virtual Error SetLeftSurroundVolume(WORD volume) = 0;
		virtual Error SetRightSurroundVolume(WORD volume) = 0;
		virtual Error SetSubwooferVolume(WORD volume) = 0;
		virtual Error SetLeftDelay(WORD leftDelay) {GNRAISE_OK;}
		virtual Error SetRightDelay(WORD rightDelay) {GNRAISE_OK;}
		virtual Error SetLeftSurroundDelay(WORD leftSurroundDelay) {GNRAISE_OK;}
		virtual Error SetRightSurroundDelay(WORD rightSurroundDelay) {GNRAISE_OK;}
		virtual Error SetSubwooferDelay(WORD subwooferDelay) {GNRAISE_OK;}
		virtual Error SetPrologicConfig(DolbyProLogicConfig prologicConfig) {GNRAISE_OK;}
		virtual Error SetPrologicMode(DolbyProLogicMode prologicMode) {GNRAISE_OK;}
		virtual Error SetDecodeLFE(BOOL decodeLFE) {GNRAISE_OK;}
		virtual Error SetPCMOutputConfig(PCMOutputConfig pcmOCFG) {GNRAISE_OK;}
		virtual Error SetDynamicRange(AC3DynamicRange dynamicRange) {GNRAISE_OK;}
		virtual Error SetAudioTypeConfig(MPEG2AudioType type, AudioTypeConfig config) {GNRAISE_OK;}
	};

class VirtualMPEG2AC3Decoder : public VirtualMPEG2AudioDecoder
	{
	private:
		MPEG2AC3Decoder	*	decoder;
	protected:
		BOOL						ac3;
		AC3BassConfig  		bassConfig;
		AC3SpeakerConfig		speakerConfig;
		AC3DualModeConfig		dualModeConfig;
		AC3KaraokeConfig		karaokeConfig;
		WORD						centerDelay;
		WORD						surroundDelay;
		WORD						centerVolume;
		WORD						leftSurroundVolume;
		WORD						rightSurroundVolume;
		WORD						subwooferVolume;
		WORD						leftDelay;
		WORD						rightDelay;
		WORD						leftSurroundDelay;
		WORD						rightSurroundDelay;
		WORD						subwooferDelay;
		DolbyProLogicConfig	prologicConfig;
		DolbyProLogicMode		prologicMode;
		BOOL						decodeLFE;
		PCMOutputConfig		pcmOCFG;
		AC3DynamicRange		dynamicRange;
		AudioTypeConfig		ac3Config;
		AudioTypeConfig		dtsConfig;
		AudioTypeConfig		lpcmConfig;
		AudioTypeConfig		mpegConfig;
		AudioTypeConfig		mpeg2Config;
		AudioTypeConfig		cddaDtsConfig;

		Error PreemptChange(VirtualUnit * previous);
	public:
		VirtualMPEG2AC3Decoder(MPEG2AC3Decoder * decoder);
		Error Configure(TAG __far * tags);
	};

//
// Inlines
//

inline Error MPEG2CommandQueue::CompleteCommand(void)
	{
	first++;

	GNRAISE_OK;
	}

#endif

#endif
