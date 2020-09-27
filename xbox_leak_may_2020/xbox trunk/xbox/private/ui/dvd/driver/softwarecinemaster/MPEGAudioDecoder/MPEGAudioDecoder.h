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

#ifndef MPEGAUDIODECODER_H
#define MPEGAUDIODECODER_H

#include "..\common\StreamTiming.h"
#include "..\audio\PCMWaveOut.h"
#include "..\audio\AudioBitStream.h"
#include "library\hardware\audio\generic\ac3setup.h"

#define MAX_SBLIMIT	32
#define MAX_MPEG_CHANNELS	2

class MPEGAudioDecoder
	{
	protected:
		AudioBitStream	*	strm;
		PCMWaveOut		*	pcmWaveOut;
		TimingSlave		*	timingSlave;
		GenericProfile *	globalProfile;
		GenericProfile	*	profile;

	private:
		float					nik2[128];
		PadOnQWord			pad0;
		short					shortWindowConsts[128][4];
	protected:
		//
		// header
		//
		WORD		syncword;
		BIT		id;
		BYTE		layer;
		BYTE		protection;

		int		frameSize;
		__int64	frameStart;

		BYTE		bitrate_index;
		BYTE		sampling_frequency;
		BIT		private_bit;
		BYTE		mode;
		BYTE		mode_extension;
		BIT		copyright;
		BIT		original;
		BYTE		emphasis;

		BYTE		sblimit;
		//
		// error check
		//
		WORD		crc;

		//
		// audio data
		//
		BYTE		baps[MAX_MPEG_CHANNELS][MAX_SBLIMIT];
		BYTE		scfsi[MAX_MPEG_CHANNELS][MAX_SBLIMIT];
		BYTE		scalefactor[MAX_MPEG_CHANNELS][MAX_SBLIMIT][3];
		float		sample[MAX_MPEG_CHANNELS][3][MAX_SBLIMIT];

		int		filterBase;
		int		nchannels;
		short		ivbuffer[MAX_MPEG_CHANNELS][1024];
		float		vbuffer[MAX_MPEG_CHANNELS][1024];


		void SynthesisFilter(void);

		bool ParseHeader(void);

		void ParseErrorCheck(void);

		void ParseAudioData(void);

		void ParseAncillaryData(void);

		void InitLocalsNewStream(void);

		int	lastTime;
		int time;
		bool	hurryUp;
		AC3DualModeConfig dualMode;

	public:
		MPEGAudioDecoder(AudioBitStream	*	strm,
			              PCMWaveOut		*	pcmWaveOut,
					        TimingSlave		*	timingSlave,
							  GenericProfile	*	globalProfile,
					        GenericProfile	*	profile);

		virtual ~MPEGAudioDecoder(void);

		void ParseFrame(void);

		void BeginStreaming(void);

		void StartStreaming(int playbackSpeed);

		void StopStreaming(void);

		void EndStreaming(bool fullReset);

		void DetachStreaming(void);
		void AttachStreaming(void);

		void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg);

	};

#endif
