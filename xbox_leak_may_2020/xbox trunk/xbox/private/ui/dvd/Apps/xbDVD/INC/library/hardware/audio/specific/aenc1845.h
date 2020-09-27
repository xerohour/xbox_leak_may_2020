// FILE:			library\hardware\audio\specific\aenc1845.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		10.03.97
//
// PURPOSE:		AD 1845 audio encoder class
//
// HISTORY:

#ifndef AENC1845_H
#define AENC1845_H

#include "ad1845.h"
#include "..\generic\encdec.h"

class AudioEncoder1845 : public BufferStreamAudioEncoder {
	friend class VirtualAudioEncoder1845;
	private:
		BufferInputStream		*	stream;
		AD1845StereoCodec 	* codec;

		AudioSampleRate	sampleRate;
		AudioMode			mode;
		BOOL					calibrate;
		WORD					sampleFreq;
		
		int					state;

		DWORD					bytesPerSample;
		DWORD					channels;
	protected:
		AudioDataFormat	format;
		DWORD	changed;

		Error SetSampleRate(AudioSampleRate sampleRate);
		Error SetDataFormat(AudioDataFormat format);
		Error SetMode(AudioMode mode);

		virtual Error SetSampleFrequency(WORD sampleFreq);
		virtual Error GetOverrange(AudioChannel channel, AudioOverrange __far & overrange);
		virtual Error SetCalibrate(BOOL calibrate);

		Error SupplyBuffer(HPTR buffer, DWORD size, DWORD bufferID);

		Error GetPosition(DWORD __far * position);
		Error SetPosition(DWORD position);

		Error BeginCapture();
		Error EndCapture();

		Error	StartCapture();
		Error	StopCapture();

		Error CompleteCapture();
		Error Flush();

		Error PauseCapture();
		Error ResumeCapture();
		
		Error ProcessChanges();

		Error GetDataPeak(DWORD __far & info) {GNRAISE(stream->GetDataPeak(info));}
	public:
		AudioEncoder1845(BufferInputStream		*	stream,
							  AD1845StereoCodec		*	codec,
							  DWORD 	bufferSize);

		VirtualUnit * CreateVirtual(void);

		void BufferCompletionRequest(HBPTR buff, DWORD size, DWORD bufferID, DWORD done);
	};

class VirtualAudioEncoder1845 : public VirtualAudioEncoder {
	private:
		AudioEncoder1845		* 		unit;	
	protected:
		WORD	sampleFreq;
		BOOL	calibrate;
		
		Error Preempt(VirtualUnit * previous);		
	public:
		VirtualAudioEncoder1845(AudioEncoder1845 * physical);

		Error Configure(TAG __far * tags);
	};

#endif


