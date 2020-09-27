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

#ifndef LPCMDECODER_H
#define LPCMDECODER_H

#include "..\common\StreamTiming.h"
#include "..\audio\PCMWaveOut.h"
#include "..\audio\AudioBitStream.h"

// NTAP must be even
#define NTAP 36

// table layout
//  coef0,		coef1	(left0  left1)
//  coef0,		coef1	(right0 right1)
//	coef2,		coef3	.....
//	coef2,		coef3	.....
static MMXShort<NTAP*2> LPF96FIRTable =
	{
         11,          22,
         11,          22,
        -68,        -348,
        -68,        -348,
       -617,        -423,
       -617,        -423,
        252,         585,
        252,         585,
        -81,        -885,
        -81,        -885,
       -303,        1206,
       -303,        1206,
       1055,       -1489,
       1055,       -1489,
      -2662,        1684,
      -2662,        1684,
       9555,       13580,
       9555,       13580,
       9555,        1684,
       9555,        1684,
      -2662,       -1489,
      -2662,       -1489,
       1055,        1206,
       1055,        1206,
       -303,        -885,
       -303,        -885,
        -81,         585,
        -81,         585,
        252,        -423,
        252,        -423,
       -617,        -348,
       -617,        -348,
        -68,          22,
        -68,          22,
         11, 		   0,
         11, 		   0
	};


class LPCMDecoder
	{
	protected:
		MMXShort<NTAP*2>		lastSamples;
		MMXShort<160*2>			pcmBuffer;

		AudioBitStream		*	strm;
		PCMWaveOut			*	pcmWaveOut;
		TimingSlave			*	timingSlave;
		bool					hurryUp;

		int						lastTime;
		int						frameSize;

		int						channels;
		int						bitsPerSample;
		int						samplesPerSecond;

		int						skip0, skip1;
		int						targetBufSize;

		void	lpf96kpcmBuffer(int qwCnt);
		void	InitLocalsNewStream(void);


	public:
		LPCMDecoder(AudioBitStream  *	strm,
			        PCMWaveOut		*	pcmWaveOut,
					TimingSlave		*	timingSlave);

		virtual ~LPCMDecoder(void);

		void ParseFrame(void);

		void SetAudioCodingMode(int samplesPerSecond,
										int bitsPerSample,
										int channels);

		void BeginStreaming(void);

		void StartStreaming(int playbackSpeed);

		void StopStreaming(void);

		void EndStreaming(bool fullReset);

		void DetachStreaming(void);
		void AttachStreaming(void);
	};

#endif
