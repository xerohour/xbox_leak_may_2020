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

#ifndef DTSSTREAMDECODER_H
#define DTSSTREAMDECODER_H

#include "DTSDecoder.h"
#include "..\audio\AudioStreamDecoder.h"
#include "..\common\TimedFibers.h"

class DTSStreamDecoder : public AudioStreamDecoder,
							protected DTSDecoder,
							private TimedFiber
	{
	protected:
		bool								lowPriority;
		bool								streaming;

		StreamFlipCopy * streamFlipCopy;

		class Supervisor : public TimedFiber
			{
			protected:
				DTSStreamDecoder * decoder;
			public:
				Supervisor(DTSStreamDecoder * decoder);
				void FiberRoutine(void);
			} supervisor;

		void FiberRoutine(void);
		void SupervisorFiberRoutine(void);
		void InitLocalsNewStream(void);
		void ClosingCurrentStream(void);

		friend class DTSStreamDecoder::Supervisor;
	public:
		DTSStreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, GenericProfile * globalProfile, GenericProfile * profile, StreamFlipCopy * streamFlipCopy);
		~DTSStreamDecoder(void);

		void BeginStreaming(void);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);
		void DetachStreaming(void);
		void AttachStreaming(void);
		DWORD SendData(BYTE * ptr, DWORD num);

		void CompleteData(void);
	};

#endif
