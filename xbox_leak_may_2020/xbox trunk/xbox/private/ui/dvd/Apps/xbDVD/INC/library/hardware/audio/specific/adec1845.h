// FILE:			library\hardware\audio\specific\adec1845.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		10.03.97
//
// PURPOSE:		AD 1845 audio decoder class
//
// HISTORY:

#ifndef ADEC1845_H
#define ADEC1845_H

#include "ad1845.h"
#include "..\generic\encdec.h"

#ifndef ONLY_EXTERNAL_VISIBLE


class AudioDecoder1845 : public BufferStreamAudioDecoder {
	friend class VirtualAudioDecoder1845;
	private:
		BufferOutputStream	*	stream;
		AD1845StereoCodec		*	codec;

		AudioSampleRate	sampleRate;
		AudioMode			mode;
		BOOL					calibrate;
		WORD					sampleFreq;

		int					state;

		DWORD					bytesPerSample;
		DWORD					channels;
		BYTE					dummyFill;
	protected:
		DWORD	changed;

		AudioDataFormat	format;

		Error SetSampleRate(AudioSampleRate sampleRate);
		Error SetDataFormat(AudioDataFormat format);
		Error SetMode(AudioMode mode);

		virtual Error SetCalibrate(BOOL calibrate);
		virtual Error SetSampleFrequency(WORD sampleFreq);
		
		Error SupplyBuffer(HPTR buffer, DWORD size, DWORD bufferID);

		Error GetPosition(DWORD __far * position);
		Error SetPosition(DWORD position);

		Error BeginPlayback();
		Error EndPlayback();

		Error	StartPlayback();
		Error	StopPlayback();

		Error CompletePlayback();
		Error Flush();

		Error PausePlayback();
		Error ResumePlayback();

		Error ProcessChanges();

		Error GetDataPeak(DWORD __far & info) {GNRAISE(stream->GetDataPeak(info));}
	public:
		AudioDecoder1845(BufferOutputStream	*	stream,
							  AD1845StereoCodec	*	codec,
							  DWORD bufferSize);

		VirtualUnit * CreateVirtual(void);

		void BufferCompletionRequest(HBPTR buff, DWORD size, DWORD bufferID, DWORD done);
	};

class VirtualAudioDecoder1845 : public VirtualAudioDecoder {
	private:
		AudioDecoder1845		* 		unit;	
	protected:
 		BOOL	calibrate;
		WORD	sampleFreq;
		
		Error Preempt(VirtualUnit * previous);
	public:
		VirtualAudioDecoder1845(AudioDecoder1845 * physical);

		Error Configure(TAG __far * tags);

	};

#endif

#endif
