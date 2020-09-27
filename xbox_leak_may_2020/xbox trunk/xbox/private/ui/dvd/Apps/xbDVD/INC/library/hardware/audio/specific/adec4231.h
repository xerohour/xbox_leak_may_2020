// FILE:			library\hardware\audio\specific\adec4231.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995, 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		21.02.96
//
// PURPOSE:		CS4231 audio decoder class
//
// HISTORY:

#ifndef ADEC4231_H
#define ADEC4231_H

#include "adec1846.h"

class AudioDecoder4231 : public AudioDecoder1846 {
	friend class VirtualAudioDecoder4231;
	protected:
		Error SetDataFormat(AudioDataFormat format);
	public:
		AudioDecoder4231(ASyncOutStream		*	stream,
							  AD1846StereoCodec	*	codec) : AudioDecoder1846(stream, codec) {;}
	};

class VirtualAudioDecoder4231 : public VirtualAudioDecoder1846 {
	private:
		AudioDecoder4231		* 		unit;	
	public:
		VirtualAudioDecoder4231(AudioDecoder4231 * physical) : VirtualAudioDecoder1846(physical)
			{this->unit = physical;}
	};

#endif

