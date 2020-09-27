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

#ifndef SPUSTREAMDECODER_H
#define SPUSTREAMDECODER_H

#include "SPUDecoder.h"
#include "..\common\TimedFibers.h"

class SPUStreamDecoder : public SPUBitStream,
                         protected SPUDecoder,
								 private TimedFiber
	{
	protected:
		TimedFiberEvent				bufferRefillEvent;

		bool								completing;
		bool								terminate;
		bool								streamingCompleted;
		bool								streaming;

		int								requestedBits;

		void FiberRoutine(void);

		bool InternalIsBufferComplete(void);
		bool InternalRefillBuffer(int requestBits, bool lowDataPanic);
	public:
		SPUStreamDecoder(SPUDisplay * display);
		~SPUStreamDecoder(void);

		void BeginStreaming(bool highQuality);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);

			// Hack for MS Navigator
		void VideoDisplayBeginStreaming(void) {display->VideoDisplayBeginStreaming();}

		void EnableSPU(bool enable) {SPUDecoder::EnableSPU(enable);}

		bool StreamingCompleted(void);

		DWORD SendData(BYTE * ptr, DWORD num);
		void CompleteData(void);

		virtual void RefillRequest(void) = 0;
		virtual void StreamCompleted(void) {}
	};

#endif
