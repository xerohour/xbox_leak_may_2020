//
// FILE:      library\hardware\mpeg2enc\generic\mp2avmen.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   22.11.1999
//
// PURPOSE:   MPEG2 Audio/Video Multiplex Encoder Class - Interface
//
// HISTORY:
//

#ifndef MP2AVMEN_H
#define MP2AVMEN_H

#include "mpeg2enc.h"

class MPEG2AVMuxEncoder : public MPEG2AVEncoder
	{               
	friend class VirtualMPEG2AVMuxEncoder;

	protected:
		MPEG2VideoEncoder 			*	videoEncoder;
		MPEG2AudioEncoder 			*	audioEncoder;

		VirtualMPEG2VideoEncoder	*	virtualVideoEncoder;
		VirtualMPEG2AudioEncoder	*	virtualAudioEncoder;

		Error StartCapture();
		Error StopCapture();
		Error DoCommand(MPEGCommand com, DWORD param);
		Error AddBuffer(HPTR data, DWORD size);
		MPEGState CurrentState(void);

		Error SetOutputStreamType(MPEGEncoderStreamType outputStreamType);
		Error SetVideoSource(VideoSource source);
		Error SetVideoStandard(VideoStandard standard);

	public:
		MPEG2AVMuxEncoder(MPEG2VideoEncoder 	* videoEncoder,
		                    MPEG2AudioEncoder 	* audioEncoder);
		~MPEG2AVMuxEncoder(void);		

	   VirtualMPEG2AVEncoder * CreateVirtualMPEGEncoder(void);
		};

	
class VirtualMPEG2AVMuxEncoder : public VirtualMPEG2AVEncoder
	{  
	private:
		MPEG2AVMuxEncoder			*	encoder;
	protected:	
		VirtualMPEG2VideoEncoder	*	videoEncoder;
		VirtualMPEG2AudioEncoder	*	audioEncoder;
		
		Error PreemptStopPrevious(VirtualUnit * previous);
		Error PreemptChange(VirtualUnit * previous);
		Error PreemptStartNew(VirtualUnit * previous);
	public:
		VirtualMPEG2AVMuxEncoder(MPEG2AVMuxEncoder * encoder, BOOL idle = FALSE);
		~VirtualMPEG2AVMuxEncoder(void);

		Error DoCommand(MPEGCommand com, DWORD param);
		Error Configure(TAG __far * tags);
	};


#endif