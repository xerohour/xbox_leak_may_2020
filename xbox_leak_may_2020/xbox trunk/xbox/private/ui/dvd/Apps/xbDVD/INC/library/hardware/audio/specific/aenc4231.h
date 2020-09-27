// FILE:			library\hardware\audio\specific\aenc4231.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995, 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		21.02.96
//
// PURPOSE:		CS4231 audio encoder class
//
// HISTORY:

#ifndef AENC4231_H
#define AENC4231_H

#include "aenc1846.h"

class AudioEncoder4231 : public AudioEncoder1846 {
	friend class VirtualAudioEncoder4231;
	protected:
		Error SetDataFormat(AudioDataFormat format);
	public:
		AudioEncoder4231(ASyncInStream		*	stream,
							  AD1846StereoCodec	*	codec) : AudioEncoder1846(stream, codec) {;}
	};

class VirtualAudioEncoder4231 : public VirtualAudioEncoder1846 {
	private:
		AudioEncoder4231		* 		unit;	
	public:
		VirtualAudioEncoder4231(AudioEncoder4231 * physical) : VirtualAudioEncoder1846(physical)
			{this->unit = physical;}
	};

#endif

