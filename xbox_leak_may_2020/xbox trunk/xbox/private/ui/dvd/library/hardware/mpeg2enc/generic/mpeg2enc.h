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

// FILE:       library\hardware\mpeg2enc\generic\mpeg2enc.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		25.10.1999
//
// PURPOSE: 	MPEG2 video/audio encoders and supporting classes. --- Header file
//
// HISTORY:
//

#ifndef MPEG2ENC_H
#define MPEG2ENC_H

#include "mp2elenc.h"

#ifndef ONLY_EXTERNAL_VISIBLE

class MPEG2AVEncoder : public MPEG2ElementaryEncoder
   {
   friend class VirtualMPEG2AVEncoder;

   protected:
		MPEGEncoderStreamType	outputType;

		virtual Error SetOutputStreamType(MPEGEncoderStreamType outputStreamType) = 0;
		virtual Error SetVideoSource(VideoSource source) = 0;
		virtual Error SetVideoStandard(VideoStandard	standard) = 0;

   public:
		VirtualUnit * CreateVirtual(void);

		virtual VirtualMPEG2AVEncoder * CreateVirtualMPEGEncoder(void) = 0;
   };

class VirtualMPEG2AVEncoder : public VirtualMPEG2ElementaryEncoder
	{
	private:
		MPEG2AVEncoder * encoder;

	protected:
		//MPEGStreamType				streamType;
		MPEGEncoderStreamType	outputStreamType;
		VideoStandard				videoStandard;
		VideoSource					videoSource;

		Error PreemptChange(VirtualUnit * previous);

	public:
		VirtualMPEG2AVEncoder(MPEG2AVEncoder * encoder);

		Error Configure(TAG __far * tags);
	};
#endif

#endif // MPEG2ENC_H

