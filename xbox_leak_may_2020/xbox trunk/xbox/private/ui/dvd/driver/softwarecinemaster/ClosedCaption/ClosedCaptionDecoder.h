//depot/xbox-aug01-final/private/ui/dvd/driver/softwarecinemaster/ClosedCaption/ClosedCaptionDecoder.h#4 - edit change 19203 (text)
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

#ifndef CLOSEDCAPTIONDECODER_H
#define CLOSEDCAPTIONDECODER_H

#include "..\video\XBoxDisplay.h"
#include "..\MPEGDecoder\MPEGUserDataDecoder.h"

#define LINE21_FIELD_XDS 0
#define LINE21_FIELD_CLOSEDCAPTION 1

class ClosedCaptionDecoder : public Line21Decoder, protected TimingClient, protected TimedFiber
	{
	protected:
		TimedFiberEvent			event;
		TimedFiberMutex			lock;
		TimingSlave				*	timer;

		struct CCDataBuffer
			{
            bool field;
			int	displayTime;
			BYTE	data[128];
			int	size, pos;
			};


		CCDataBuffer				buffer[64];
		int							first, last;
		volatile bool				streaming, running, terminate;
		bool						cc_ok;

		int iMacrovisionLevel;
		VideoCopyMode eCGMSMode;
		int iNextCGMSPacket;


		bool DecodeCCCommand(bool initial, BYTE cc0, BYTE cc1);

		void FiberRoutine(void);

		void SendLine21Data(int displayTime, BYTE * data, int size);
		bool NeedsPureData(void) {return true;}

        void SendCGMSPacket();
		void SendLine21DataEx(bool field, int displayTime, BYTE * data, int size);
        

	public:
		ClosedCaptionDecoder(TimingSlave * timer);
		~ClosedCaptionDecoder(void);

		void BeginStreaming(void);
		void EndStreaming(void);
		void StartStreaming(void);
		void StopStreaming(void);
		void Reset();

		void SetMacrovisionLevel(int iMacrovisionLevel) ;

		void SetCGMSMode(VideoCopyMode eCGMSMode) ;

	};


#endif
