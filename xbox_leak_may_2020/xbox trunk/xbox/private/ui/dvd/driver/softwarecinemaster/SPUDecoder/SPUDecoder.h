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

#ifndef SPUDECODER_H
#define SPUDECODER_H

#include "..\video\VideoDisplay.h"
#include "..\video\FrameStore.h"
#include "SPUBitStream.h"
#include "SPUDisplay.h"


class SPUDecoder
	{
	protected:
		SPUBitStream				*	stream;
		SPUDisplay					*	display;
		volatile bool					terminate;
		bool								enable;
	public:
		SPUDecoder(SPUBitStream	*	stream,
					  SPUDisplay	*	display);

		virtual ~SPUDecoder(void);

		void Parse(void);

		void BeginStreaming(bool highQuality);

		void StartStreaming(int playbackSpeed);

		void StopStreaming(void);

		void EndStreaming(bool fullReset);

		void EnableSPU(bool enable) {display->EnableSPU(enable);}
	};


#endif
