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

// FILE:       Driver/Dvdpldrv/Common/MPEGAudioHeader.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    3-SEP-2000
//
// PURPOSE:    Common MPEG audio data types.
//
// HISTORY:

#ifndef MPEGAUDIOHEADER_H
#define MPEGAUDIONEADER_H

#include "Library/Common/Prelude.h"
#include "Library/Common/Gnerrors.h"
#include "Driver/Dvdpldrv/Common/AVF/MPEGAudioTypes.h"
#include "Driver/Dvdpldrv/Common/DVDTime.h"

// #define ONLY_EXTERNAL_VISIBLE
// #include "Library/Hardware/Mpegdec/Generic/Mpegdec.h"

#define MPEG_BITRATE_INDEX_BAD	(0x0F)
#define MPEG_BITRATE_INDEX_FREE	(0x00)	// also bad!
#define MPEG_SAMPLE_RATE_INDEX_BAD	(0x03)
#define MPEG_AUDIO_FRAME_HEADER_LEN		(4)

// operates upon first byte in header
#define IS_MPEG_FRAMEHEADER_SYNC_BYTE0(b)		(b == 0xFF)

// operates upon second byte in header
#define IS_MPEG_FRAMEHEADER_SYNC_BYTE1(b)		((b & 0xE0) == 0xE0)
#define MPEG_VERSION_FROM_BYTE1(b)				((MPEGAudioVersion)((b & 0x18) >> 3))
#define MPEG_LAYER_FROM_BYTE1(b)		((MPEGAudioLayerDescription)(4-((b & 0x06)>>1))) // 4 == RESERVED
#define IS_MPEG_CRC_PROTECTED_BYTE1(b)			((b & 0x01) == 0x00)

// operates upon third byte in header
#define MPEG_BITRATE_INDEX_FROM_BYTE2(b)		((b & 0xF0) >> 4)
#define MPEG_SAMPLE_RATE_INDEX_FROM_BYTE2(b)	((b & 0x0C) >> 2)
#define IS_MPEG_FRAME_PADDED_BYTE2(b)			((b & 0x02) == 0x02)
#define IS_MPEG_PRIVATE_BIT_SET_BYTE2(b)		((b & 0x01) == 0x01)

// operates upon fourth byte in header
#define MPEG_CHANNEL_MODE_FROM_BYTE3(b)		((MPEGAudioChannelMode)((b & 0xC0) >> 6))
#define MPEG_LAYER_I_AND_II_JOINT_STEREO_MODE_EXT_FROM_BYTE3(b) \
							((MPEGAudioJointStereoModeExtLayerIAndII)((b & 0x30) >> 4))
#define IS_MP3_INTENSITY_STEREO_BYTE3(b)		((b & 0x10) == 0x10)
#define IS_MP3_MS_STEREO_BYTE3(b)				((b & 0x20) == 0x20)
#define IS_MPEG_COPYRIGHTED_BYTE3(b)			((b & 0x08) == 0x08)
#define IS_MPEG_ORIGINAL_MEDIA_BYTE3(b)		((b & 0x04) == 0x04)
#define MPEG_EMPHASIS_MODE_FROM_BYTE3(b)		((MPEGAudioEmphasisMode)(b & 0x03))

// ID3V1 parsing macros
#define IS_MPEG_ID3V1_ID_BYTE0(b)				(b == 0x54)		// T as in TAG
#define IS_MPEG_ID3V1_ID_BYTE1(b)				(b == 0x41)		// A as in TAG
#define IS_MPEG_ID3V1_ID_BYTE2(b)				(b == 0x47)		// G as in TAG

#define MPEG_ID3V1_ID_LEN							(3)
#define MPEG_ID3V1_TITLE_LEN						(30)
#define MPEG_ID3V1_ARTIST_LEN						(30)
#define MPEG_ID3V1_ALBUM_LEN						(30)
#define MPEG_ID3V1_YEAR_LEN						(4)
#define MPEG_ID3V1_COMMENT_LEN					(29)	// tricky, ID3v1.1 allows track# as char 30

#define MPEG_ID3V1_TITLE_START					(MPEG_ID3V1_ID_LEN)
#define MPEG_ID3V1_ARTIST_START					(MPEG_ID3V1_TITLE_START+MPEG_ID3V1_TITLE_LEN)
#define MPEG_ID3V1_ALBUM_START					(MPEG_ID3V1_ARTIST_START+MPEG_ID3V1_ARTIST_LEN)
#define MPEG_ID3V1_YEAR_START						(MPEG_ID3V1_ALBUM_START+MPEG_ID3V1_ALBUM_LEN)
#define MPEG_ID3V1_COMMENT_START					(MPEG_ID3V1_YEAR_START+MPEG_ID3V1_YEAR_LEN)
#define MPEG_ID3V1_TRACKNUM_BYTE					(MPEG_ID3V1_COMMENT_START+MPEG_ID3V1_COMMENT_LEN)
#define MPEG_ID3V1_GENRE_BYTE						(MPEG_ID3V1_TRACKNUM_BYTE+1)
#define MPEG_ID3V1_FRAME_BYTES					(MPEG_ID3V1_GENRE_BYTE+1)	// 128
#define MPEG_ID3V1_FRAME_SKIP_BYTES				(MPEG_ID3V1_FRAME_BYTES - MPEG_ID3V1_ID_LEN)


// ID3V2 parsing macros
#define IS_MPEG_ID3V2_ID_BYTE0(b)		(b == 0x49)		// I as in TAG
#define IS_MPEG_ID3V2_ID_BYTE1(b)		(b == 0x44)		// D as in TAG
#define IS_MPEG_ID3V2_ID_BYTE2(b)		(b == 0x33)		// 3 as in TAG
#define MPEG_ID3V2_ID_LEN					(3)
#define MPEG_ID3V2_HEADER_BYTES			(10)	// plus some extra stuff
#define MPEG_ID3V2_FLAG_ZERO_LEN			(3)
#define MPEG_ID3V2_TAG_SIZE(a,b,c,d)	(a*2097152 + b*16384 + c*128 + d)
#define ID3V2_FOOTER_PRESENT_MASK		(0x10)

typedef struct
	{
	char		id[MPEG_ID3V2_ID_LEN+1];	// must be 'ID3'
	BYTE		majorVersion;
	BYTE		minorVersion;
	BYTE		flags;
	BYTE		zeros[MPEG_ID3V2_FLAG_ZERO_LEN]; // NOT null-terminated
	BYTE		sizeA;	// use MPEG_ID3V2_TAG_SIZE macro to get actual size
	BYTE		sizeB;
	BYTE		sizeC;
	BYTE		sizeD;
	BOOL		footerPresent;
	DWORD		totalTagSize; // does not take into account footer presence
	} MPEGAudioID3V2Header;

// VBR header parsing macros
#define IS_MPEG_VBR_ID_BYTE0(b)				(b == 0x58)		// X as in Xing
#define IS_MPEG_VBR_ID_BYTE1(b)				(b == 0x69)		// i as in Xing
#define IS_MPEG_VBR_ID_BYTE2(b)				(b == 0x6E)		// n as in Xing
#define IS_MPEG_VBR_ID_BYTE3(b)				(b == 0x67)		// g as in Xing

#define IS_VBR_HEADER(p)  (IS_MPEG_VBR_ID_BYTE0(p[0]) && IS_MPEG_VBR_ID_BYTE1(p[1]) && \
								   IS_MPEG_VBR_ID_BYTE2(p[2]) && IS_MPEG_VBR_ID_BYTE3(p[3]))

#define MPEG1_MONO_VBR_OFFSET				(18)
#define MPEG1_STEREO_VBR_OFFSET			(33)
#define MPEG2_MONO_VBR_OFFSET				(10)
#define MPEG2_STEREO_VBR_OFFSET			(18)

#define MPEG_VBR_ID_LEN						(4)
#define MPEG_VBR_FLAG_BYTES				(4)
#define MPEG_VBR_FRAMECOUNT_BYTES		(4)
#define MPEG_VBR_FILESIZE_BYTES			(4)
#define MPEG_VBR_TOC_BYTES					(100)
#define MPEG_VBR_SCALE_BYTES				(4)

#define MPEG_VBR_FRAMES_FLAG			(0x0001)
#define MPEG_VBR_BYTES_FLAG			(0x0002)
#define MPEG_VBR_TOC_FLAG				(0x0004)
#define MPEG_VBR_SCALE_FLAG			(0x0008)

#define MPEG_VBR_FLAGS_START				(MPEG_VBR_ID_LEN)
#define MPEG_VBR_FRAMECOUNT_START		(MPEG_VBR_FLAGS_START+MPEG_VBR_FLAG_BYTES)
#define MPEG_VBR_FILESIZE_START			(MPEG_VBR_FRAMECOUNT_START+MPEG_VBR_FRAMECOUNT_BYTES)
#define MPEG_VBR_TOC_START					(MPEG_VBR_FILESIZE_START+MPEG_VBR_FILESIZE_BYTES)
#define MPEG_VBR_SCALE_START				(MPEG_VBR_TOC_START+MPEG_VBR_TOC_BYTES)
#define MPEG_VBR_FRAME_BYTES				(MPEG_VBR_SCALE_START+MPEG_VBR_SCALE_BYTES)	// 120
#define MPEG_VBR_FRAME_SKIP_BYTES		(MPEG_VBR_FRAME_BYTES - MPEG_VBR_ID_LEN)

#define BIG_ENDIAN_DWORD(p)	((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3])

typedef struct
	{
	DWORD		flags;
	DWORD		frames;	// use macro to get actual frame count
	DWORD		fileSize;	// use macro to get actual file size
	BYTE		toc[MPEG_VBR_TOC_BYTES];				// table of contents, for searching
	DWORD		scale;
	} MPEGAudioVBRHeader;

#define MPEG_AUDIO_UNKNOWN_TAG_ID_SIZE	4	// max(MPEG_ID3V1_ID_LEN, MPEG_ID3V2_ID_LEN, MPEG_VBR_ID_LEN)
enum MPEGNonAudioFrameType
	{
	MPEG_NONAUDIO_FRAME_NONE		= 0,
	MPEG_NONAUDIO_ID3V1_FRAME		= 1,
	MPEG_NONAUDIO_ID3V2_FRAME		= 2,
	MPEG_NONAUDIO_VBR_FRAME			= 3,
	MPEG_NONAUDIO_LYRICS3_FRAME	= 4
	};

enum MPEGNonAudioParseMode
	{
	MPEG_NONAUDIO_PARSE_QUICK		= 0,
	MPEG_NONAUDIO_PARSE_FULL		= 1
	};

enum MPEGAudioFileParseState
	{
	MP3PS_BEGINNING_OF_FILE = 0,
	MP3PS_BEGINNING_FRAME_SEARCH = 1,
	MP3PS_AFTER_FIRST_HEADER_BYTE = 2,
	MP3PS_AFTER_SECOND_HEADER_BYTE = 3,
	MP3PS_AFTER_THIRD_HEADER_BYTE = 4,
	MP3PS_BEFORE_CRC_BYTE0 = 5, // if CRC not used go straight to "AFTER_HEADER"
	MP3PS_BEFORE_CRC_BYTE1 = 6, // if CRC not used go straight to "AFTER_HEADER"
	MP3PS_AFTER_HEADER = 7,
	MP3PS_IN_DATA = 8,
	MP3PS_AFTER_DATA = 9,
	// new states required by ID3v2 tags
	MP3PS_AFTER_FIRST_ID3V2_HEADER_BYTE = 10,
	MP3PS_AFTER_SECOND_ID3V2_HEADER_BYTE = 11,
	MP3PS_AFTER_THIRD_ID3V2_HEADER_BYTE = 12,
	MP3PS_AFTER_ID3V2_MAJOR_VERSION_BYTE = 13,
	MP3PS_AFTER_ID3V2_MINOR_VERSION_BYTE = 14,
	MP3PS_AFTER_ID3V2_FLAGS_BYTE = 15,
	MP3PS_AFTER_FIRST_ID3V2_SIZE_BYTE = 16,
	MP3PS_AFTER_SECOND_ID3V2_SIZE_BYTE = 17,
	MP3PS_AFTER_THIRD_ID3V2_SIZE_BYTE = 18
	};

//
// class MPEGNonAudioFrame
//

class MPEGNonAudioFrame
	{
	public:
		MPEGAudioID3V2Header			id3v2Frame;
		MPEGAudioVBRHeader			vbrFrame;
		BOOL								id3v2FrameAvailable;
		BOOL								vbrFrameAvailable;
		MPEGNonAudioFrame(void);
		~MPEGNonAudioFrame(void);
		void Cleanup(void);
	};

//
// class MPEGAudioFrameHeader
//

class MPEGAudioFrameHeader
	{
	public:
		BYTE	header[4];
		// data is accessed directly
		MPEGAudioVersion				mpegVersion;
		MPEGAudioLayerDescription	mpegLayer;
		BOOL								hasCRC;
		BOOL								validCRC;
		WORD								actualCRC;
		BYTE								bitrateIndex;
		WORD								bitrate;
		BYTE								sampleRateIndex;
		WORD								sampleRate;
		WORD								frameLength;
		BOOL								privateBit;
		MPEGAudioChannelMode								channelMode;
		MPEGAudioJointStereoModeExtLayerIAndII		modeExtension;
		BOOL								copyrighted;
		BOOL								originalMedia;
		MPEGAudioEmphasisMode		emphasisMode;
		BOOL								validHeader;
		BOOL								bitrateSwitchingInUse;
		BOOL								bitReservoirInUse;
		BOOL								frameIsPadded;
		WORD								padBytesSlotSize;
		BOOL								intensityStereo;
		BOOL								msStereo;
		BOOL								headerBytesLoaded;
		static const WORD				v1L1Bitrate[15];
		static const WORD				v1L2Bitrate[15];
		static const WORD				v1L3Bitrate[15];
		static const WORD				v2L1Bitrate[15];
		static const WORD				v2L2or3Bitrate[15];
		static const WORD				ver1SampleRate[3];
		static const WORD				ver2SampleRate[3];
		static const WORD				ver25SampleRate[3];
		static DWORD					halfBitRate;	// for calculation efficiency

		// member functions
		MPEGAudioFrameHeader(void) { validHeader = FALSE; headerBytesLoaded = FALSE; validCRC = FALSE; }
		void LoadHeader(BYTE first, BYTE second, BYTE third, BYTE fourth)
			{
			header[0] = first;
			header[1] = second;
			header[2] = third;
			header[3] = fourth;
			headerBytesLoaded = TRUE;
			validHeader = FALSE;
			validCRC = FALSE;
			}
		Error LoadHeader(BYTE *buffer)
			{
			if (buffer != (BYTE *) NULL)
				{
				header[0] = buffer[0];
				header[1] = buffer[1];
				header[2] = buffer[2];
				header[3] = buffer[3];
				headerBytesLoaded = TRUE;
				validHeader = FALSE;
				validCRC = FALSE;
				GNRAISE_OK;
				}
			else
				GNRAISE(GNR_OBJECT_EMPTY);
			}
		Error ParseHeader(const MPEGAudioFrameInfo *pMpegInfo = NULL);
		Error VerifyByte0(BYTE header_0);
		Error VerifyByte1(BYTE header_1);
		Error VerifyByte2(BYTE header_2);
		Error VerifyByte3(BYTE header_3);
		BYTE * GetHeaderPtr(void) { return &header[0]; }
		void SetCRC(BYTE byte0, BYTE byte1)
			{
			actualCRC = (byte0 << 8) + byte1;
			if (hasCRC)
				validCRC = TRUE;
			}
		DWORD	CalculateMilliSeconds(DWORD dataSize);

		Error	ValidateTagHeaderChar (BYTE b, int num, BOOL &matches);

		void InvalidateHeader(void) { validHeader = FALSE; headerBytesLoaded = FALSE; validCRC = FALSE; }
	};

//
// class MPEGNonAudioParseState
//

class MPEGNonAudioParseState
	{
	public:
		MPEGNonAudioFrame				info;		// this contains the actual data

		char								nonAudioFrameHeader[MPEG_AUDIO_UNKNOWN_TAG_ID_SIZE];
		char								yearStr[MPEG_ID3V1_YEAR_LEN+1];

		DWORD								nonAudioFrameHeaderCount;

		BOOL								inNonAudioFrame;

		MPEGNonAudioFrameType		currentFrameType;

		BOOL								id3v1FrameBeingBuilt;
		BOOL								id3v2FrameBeingBuilt;
		BOOL								vbrFrameBeingBuilt;

		DWORD								id3v1FrameOffset;
		DWORD								id3v2FrameOffset;
		DWORD								vbrFrameOffset;

		DWORD								nonAudioFrameParseOffset;
		DWORD								nonAudioFrameParseFieldOffset;
		DWORD								nonAudioFrameParseHeaderSize;

		MPEGNonAudioParseState(void);
		~MPEGNonAudioParseState(void);
		void Cleanup(void);
		Error ParseVBRHeader(BYTE *pVBR);
		void ResetByteParser(void);
	};

//
//	This is local to the MPEG Audio Filter
//

class MPEGAudioParseState
	{
	public:
		// File parse data
		MPEGAudioFileParseState		state;
		MPEGNonAudioParseState		nonAudio;
		DWORD								dataBytesRead;

		MPEGAudioParseState(void);
		~MPEGAudioParseState(void);
	};

#endif

