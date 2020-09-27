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

// FILE:       library\hardware\mpeg2enc\generic\mp2elenc.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		25.10.1999
//
// PURPOSE: 	MPEG2 elementary video/audio encoders and supporting classes. --- Header file
//
// HISTORY:
//

#ifndef MP2ELENC_H
#define MP2ELENC_H

#include "library/hardware/mpegenc/generic/mpegenc.h"

#ifndef ONLY_EXTERNAL_VISIBLE

class MPEG2ElementaryEncoder : public PhysicalUnit
   {
   friend class VirtualMPEG2ElementaryEncoder;

   protected:
		Error TransferDoneHook(void);
		Error BufferStarvingHook(void);

		virtual Error DoCommand(MPEGCommand com, DWORD param) = 0;
		virtual Error AddBuffer(HPTR data, DWORD size) = 0;
		virtual MPEGState CurrentState(void) = 0;

	public:
		MPEG2ElementaryEncoder(void) {;}

	};

class VirtualMPEG2ElementaryEncoder : public VirtualUnit
	{
	friend class MPEG2ElementaryEncoder;

	private:
		MPEG2ElementaryEncoder	*	encoder;

	protected:
#if !DRVDLL
		MPEGTransferDoneHookHandle				doneHook;
		MPEGEncoderBufferStarvingHookHandle bufferStarvingHook;
#endif
		Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);

	public:
		VirtualMPEG2ElementaryEncoder(MPEG2ElementaryEncoder * unit);
		~VirtualMPEG2ElementaryEncoder(void);

		Error Configure (TAG __far *tags);
		virtual Error AddBuffer(HPTR data, DWORD size);
		virtual Error AllocateBuffer (DWORD size, FPTR __far &info, FPTR __far &data) { GNRAISE(GNR_UNIMPLEMENTED); }
		virtual Error FreeBuffer (FPTR info) { GNRAISE(GNR_UNIMPLEMENTED); }
		virtual Error DoCommand(MPEGCommand com, DWORD param);

	};

class MPEG2VideoEncoder : public MPEG2ElementaryEncoder
	{
	friend class VirtualMPEG2VideoEncoder;

	protected:
		virtual Error SetOutputStreamType(MPEGEncoderStreamType outputStreamType) = 0;
		virtual Error SetVideoSource(VideoSource source) = 0;
		virtual Error SetVideoStandard(VideoStandard	standard) = 0;
		virtual Error SetAspectRatio(WORD aspectRatio) = 0;
		virtual Error SetFrameRate(DWORD frameRate) = 0;
		virtual Error SetVideoBitrate(DWORD videoBitrate) = 0;
		virtual Error SetAudioBitrate(DWORD audioBitrate) = 0;
		virtual Error SetVideoWidth(WORD width) = 0;
		virtual Error SetVideoHeight(WORD height) = 0;

	public:

	};

class VirtualMPEG2VideoEncoder : public VirtualMPEG2ElementaryEncoder
	{
	private:
		MPEG2VideoEncoder	*		encoder;

	protected:
		MPEGEncoderStreamType	outputStreamType;
		VideoSource					videoSource;
		VideoStandard				videoStandard;

		Error PreemptChange(VirtualUnit * previous);

	public:
		VirtualMPEG2VideoEncoder(MPEG2VideoEncoder * unit) : VirtualMPEG2ElementaryEncoder(unit)
			{encoder = unit; outputStreamType = MST_UNKNOWN; videoSource = VIDEOSOURCE_MIN;
			 videoStandard = VIDEOSTANDARD_MIN;}

		Error Configure(TAG __far * tags);
	};

class MPEG2AudioEncoder : public MPEG2ElementaryEncoder
	{
	public:

	};

class VirtualMPEG2AudioEncoder : public VirtualMPEG2ElementaryEncoder
	{
	private:
		MPEG2AudioEncoder	* encoder;
	protected:
		Error PreemptChange(VirtualUnit * previous);
	public:
		VirtualMPEG2AudioEncoder(MPEG2AudioEncoder	* encoder) : VirtualMPEG2ElementaryEncoder(encoder)
			{this->encoder = encoder;}

		Error Configure (TAG __far *tags);
	};


#endif // ONLY_EXTERNAL_VISIBLE

#endif // MP2ELENC_H

