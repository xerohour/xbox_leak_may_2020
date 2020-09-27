// FILE:			library\hardware\audio\specific\aenc1846.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		07.12.95
//
// PURPOSE:		AD1846 audio encoder class
//
// HISTORY:

#ifndef AENC1846_H
#define AENC1846_H

#include "library\general\asncstrm.h"
#include "ad1846.h"
#include "..\generic\encdec.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define CHANGED_FORMAT			MKFLAG(0)
#define CHANGED_CALIBRATE		MKFLAG(1)

class AudioEncoder1846 : public ASyncStreamAudioEncoder {
	friend class VirtualAudioEncoder1846;
	private:
		ASyncInStream		*	stream;
		AD1846StereoCodec	*	codec;

		BYTE __huge *	buffer;
		DWORD				bufferID;

		DWORD	bytesRemaining;
		DWORD bytesDone;
		
		BOOL	captureEnabled;

		AudioSampleRate	sampleRate;
		AudioMode			mode;
		BOOL					calibrate;
	protected:
		AudioDataFormat	format;
		DWORD	changed;

		Error SetSampleRate(AudioSampleRate sampleRate);
		Error SetDataFormat(AudioDataFormat format);
		Error SetMode(AudioMode mode);

		virtual Error GetOverrange(AudioChannel channel, AudioOverrange __far & overrange);
		virtual Error SetCalibrate(BOOL calibrate);

		Error SupplyBuffer(HPTR buffer, DWORD size, DWORD bufferID);

		Error GetPosition(DWORD __far * position);
		Error SetPosition(DWORD position);

		Error	StartCapture();
		Error	StopCapture();

		Error PauseCapture();
		Error ResumeCapture();
		
		Error ProcessChanges();
	public:
		AudioEncoder1846(ASyncInStream		*	stream,
							  AD1846StereoCodec	*	codec);

		VirtualUnit * CreateVirtual(void);

		void EmptyRequest(DWORD avail);
	};

class VirtualAudioEncoder1846 : public VirtualAudioEncoder {
	private:
		AudioEncoder1846		* 		unit;	
	protected:
		BOOL	calibrate;

		Error Preempt(VirtualUnit * previous);
	public:
		VirtualAudioEncoder1846(AudioEncoder1846 * physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE


#endif
