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

// FILE:			library\hardware\audio\generic\aucodecs.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		25.06.96
//
// PURPOSE:		Base class definition for audio codecs.
//
// HISTORY:

#ifndef AUCODECS_H
#define AUCODECS_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"

// Error definition
#define GNR_AUD_CODEC_HALTED	MKERR(ERROR, AUDIO, GENERAL, 0x0666)

#define GNR_NO_AUDIO_HARDWARE	MKERR(ERROR, AUDIO, GENERAL, 0x0667)

class AudioCodec
	{
	protected:
		BOOL	playback;
		BOOL	capture;

		BOOL	playbackResume;
		BOOL	captureResume;

		int	haltCount;

	public:
		AudioCodec();

		virtual Error InitPlayback		(void);
		virtual Error EndPlayback		(void);

		virtual Error InitCapture		(void);
		virtual Error EndCapture		(void);

		virtual Error HaltTransfers	(void);			// Stop all transfers (capture & playback).
		virtual Error ResumeTransfers	(void);			// Resume all transfers.
	};

#endif
