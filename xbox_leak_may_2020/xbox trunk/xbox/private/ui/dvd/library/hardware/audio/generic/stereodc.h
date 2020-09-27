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

// FILE:			library\hardware\audio\generic\stereodc.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1996 Viona Development.  All Rights Reserved.
// CREATED:		22.08.95
//
// PURPOSE:		Base class for stereo decoders (FM/TV stereo decoders)
//
// HISTORY:

#ifndef STEREODC_H
#define STEREODC_H

enum SignalMode	{ST_MONO,					// Either TV or FM is received mono only
						 ST_STEREO,					//    "    "  "  "  "     "    stereo
						 ST_SAP,						// Second audio program (SAP) received for TV
						 ST_UNDEFINED,				// Mode could not be determined (i.e. no signal)
						 ST_AUTOMATIC,				// Only used when setting the mode.
						 ST_STEREO_SAP				// For NTSC TV: Stereo and SAP at the same time.
						 };


#ifndef ONLY_EXTERNAL_VISIBLE

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"
#include "library/common/virtunit.h"
#include "library/common/tagunits.h"
//
// Stereo Decoder tags
//
MKTAG(STEREODEC_MODESELECT,	STEREO_DECODER_UNIT,	0x0001, SignalMode)
MKTAG(STEREODEC_MODEDETECT,	STEREO_DECODER_UNIT,	0x0002, SignalMode)

class StereoDecoder : public PhysicalUnit	{
	friend class VirtualStereoDecoder;
	protected:
		virtual Error GetSignalMode(SignalMode __far & mode) = 0;
		virtual Error SetSignalMode(SignalMode mode) = 0;

		virtual Error ProcessChanges() = 0;
	public:
	};

class VirtualStereoDecoder : public VirtualUnit {
	private:
		StereoDecoder	*	decoder;
	protected:
		SignalMode		mode;
	public:
		VirtualStereoDecoder(StereoDecoder * decoder);
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);
	};

#endif

#endif
