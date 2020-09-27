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

#ifndef DTSDECODER_H
#define DTSDECODER_H

#include "..\common\StreamTiming.h"
#include "..\audio\PCMWaveOut.h"
#include "..\audio\AudioBitStream.h"
#include "library\common\profiles.h"
#include "library\hardware\audio\generic\AC3setup.h"

class DTSDecoder
	{
	protected:
		AudioBitStream	*	strm;
		PCMWaveOut		*	pcmWaveOut;
		GenericProfile	*	globalProfile;
		GenericProfile	*	profile;

	private:

	protected:
		DWORD	syncword;
		int		numPCMSamplePairs;
		BYTE	dtsType;

		int		frameSize;
		__int64	frameStart;

		int		FrameCnt;

		short	leftRightBuffer[2048*2];
		short	encodedBuffer[2048*2];

		bool				dtsDigitalOutput;

		bool ParseSyncInfo(void);

		bool ParseBSI(void);

		int	lastTime;

		int	currentDataTime;

		bool	hurryUp;

		TimingSlave	*	timingSlave;

		void FrameDTSDigitalOutput(void);
		void InitLocalsNewStream(void);

	public:
		DTSDecoder(AudioBitStream	*	strm,
					PCMWaveOut		*	pcmWaveOut,
					TimingSlave		*	timingSlave,
					GenericProfile	*	globalProfile,
					GenericProfile	*	profile);

		virtual ~DTSDecoder(void);

		void ParseSyncFrame(void);

		void BeginStreaming(void);

		void StartStreaming(int playbackSpeed);

		void StopStreaming(void);

		void EndStreaming(bool fullReset);

		void DetachStreaming(void);
		void AttachStreaming(void);

		void SetDigitalOut(AudioTypeConfig dtsAudioType);
		void GetDigitalOut(AudioTypeConfig & dtsAudioType);
	};

#endif
