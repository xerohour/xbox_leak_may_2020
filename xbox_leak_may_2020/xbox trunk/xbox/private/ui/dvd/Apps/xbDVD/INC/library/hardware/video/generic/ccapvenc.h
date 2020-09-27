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

#ifndef CCAPVENC_H
#define CCAPVENC_H

#include "videnc.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define CCENCBUFFERSIZE	512

class CloseCaptionVideoEncoder : public VideoEncoder, public InterruptHandler
	{
	protected:
		virtual Error GetCCStatus(BOOL __far & first, BOOL __far & second) = 0;
		virtual Error PutCCData(BOOL field, BYTE d1, BYTE d2) = 0;
		virtual Error EnableCCTransfer(void) = 0;
		virtual Error DisableCCTransfer(void) = 0;

		char	ccbuffer[CCENCBUFFERSIZE];
		int	ccfirst, cclast;
		BOOL	cctransfer;
		BOOL	ccLastField;
		BOOL	ccflushed;
		BOOL	ccflushing;
		int	ccflushcnt;

	public:
		CloseCaptionVideoEncoder(GenericProfile * profile = NULL, int nIdleScreens = 0,
		                         VideoEncoderIdleEntry __far * idleEntries = NULL);

		// Dynamic type conversion routine to InterruptHandeler
		virtual InterruptHandler * ToInterruptHandler(void) {return this;}

		virtual void Interrupt(void);

		virtual Error AddCloseCaptionData(BOOL startField, BYTE __far * data, int num);
		virtual Error FlushCloseCaptionBuffer(void);
		virtual Error StartCloseCaptionTransfer(void);
		virtual Error StopCloseCaptionTransfer(void);
	};

class VirtualCloseCaptionVideoEncoder : public VirtualVideoEncoder, public Line21Receiver
	{
	private:
		CloseCaptionVideoEncoder	*	encoder;

	protected:
		BOOL	cctransfer;

		Error Preempt(VirtualUnit * previous);
	public:
		VirtualCloseCaptionVideoEncoder(CloseCaptionVideoEncoder * encoder);

		// Dynamic type conversion routine to Line21Receiver
		virtual Line21Receiver * ToLine21Receiver(void) {return this;}

		virtual Error AddCloseCaptionData(BOOL startField, BYTE __far * data, int num);
		virtual Error FlushCloseCaptionBuffer(void);
		virtual Error StartCloseCaptionTransfer(void);
		virtual Error StopCloseCaptionTransfer(void);
	};

#endif

#endif
