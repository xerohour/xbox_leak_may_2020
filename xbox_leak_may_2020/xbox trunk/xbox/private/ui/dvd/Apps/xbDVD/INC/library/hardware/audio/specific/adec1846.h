// FILE:			library\hardware\audio\specific\adec1846.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		07.12.95
//
// PURPOSE:		AD1846 audio decoder class
//
// HISTORY:

#ifndef ADEC1846_H
#define ADEC1846_H

#include "library\general\asncstrm.h"
#include "ad1846.h"
#include "..\generic\encdec.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define CHANGED_FORMAT			MKFLAG(0)
#define CHANGED_CALIBRATE		MKFLAG(1)

class AudioDecoder1846 : public ASyncStreamAudioDecoder {
	friend class VirtualAudioDecoder1846;
	private:
		ASyncOutStream		*	stream;
		AD1846StereoCodec *	codec;
		
		BYTE __huge	*	buffer;
		DWORD				bufferID;
		BYTE __huge *	pbuffer;
		DWORD				pbufferID;
		BYTE __huge *	ppbuffer;
		DWORD				ppbufferID;
		
		
		DWORD	bytesRemaining;
		DWORD bytesDone;

		BOOL	playbackEnabled;
		
		AudioSampleRate	sampleRate;
		AudioMode			mode;
		BOOL					calibrate;

		BOOL					paused;
	protected:
		DWORD	changed;

		AudioDataFormat	format;

		Error SetSampleRate(AudioSampleRate sampleRate);
		Error SetDataFormat(AudioDataFormat format);
		Error SetMode(AudioMode mode);

		virtual Error SetCalibrate(BOOL calibrate);
		
		Error SupplyBuffer(HPTR buffer, DWORD size, DWORD bufferID);

		Error GetPosition(DWORD __far * position);
		Error SetPosition(DWORD position);

		Error	StartPlayback();
		Error	StopPlayback();
		
		Error PausePlayback();
		Error ResumePlayback();

		Error ProcessChanges();
	public:
		AudioDecoder1846(ASyncOutStream		*	stream,
							  AD1846StereoCodec	*	codec);

		VirtualUnit * CreateVirtual(void);

		void	RefillRequest(DWORD free);
	};

class VirtualAudioDecoder1846 : public VirtualAudioDecoder {
	private:
		AudioDecoder1846		* 		unit;	
	protected:
 		BOOL	calibrate;

		Error Preempt(VirtualUnit * previous);
	public:
		VirtualAudioDecoder1846(AudioDecoder1846 * physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE


#endif
