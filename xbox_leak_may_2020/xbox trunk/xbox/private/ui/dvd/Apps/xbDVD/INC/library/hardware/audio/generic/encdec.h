// FILE:			library\hardware\audio\generic\encdec.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		12.01.95
//
// PURPOSE:		Base class for audio encoders/decoders/codecs.
//
// HISTORY:

#ifndef ENCDEC_H
#define ENCDEC_H

#include "austrdev.h"
#include "bufstrms.h"

//
// Special tags common for audio encoders and decoders
//
MKTAG (AUDIO_CALIBRATE,			AUDIO_STREAMDEV_UNIT, 0x0010, BOOL)

//
// Special audio encoder tags
//
MKTAG (AUDIO_ENCODER_HOOK,		AUDIO_ENCODER_UNIT, 0x0101, AudioBufferHookPtr)
MKTAG (AUDIO_OVERRANGE_LEFT,	AUDIO_ENCODER_UNIT, 0x0102, AudioOverrange)
MKTAG (AUDIO_OVERRANGE_RIGHT,	AUDIO_ENCODER_UNIT, 0x0103, AudioOverrange)

//
// Organization in this DWORD: (in big endian) MINL MINR MAXL MAXR
//
MKTAG (AUDIO_ENCODER_DATA_PEAK_INFO,	AUDIO_ENCODER_UNIT, 0x0104, DWORD)


// Special audio decoder tags
MKTAG (AUDIO_DECODER_HOOK,		AUDIO_DECODER_UNIT, 0x00000201, AudioBufferHookPtr)

MKTAG (AUDIO_DECODER_DATA_PEAK_INFO,	AUDIO_DECODER_UNIT, 0x0202, DWORD)


//
// Error definitions for encoder
//
#define GNR_AUDENC_CAPTURE_ENABLED	MKERR(ERROR, AUDIO, GENERAL, 0x0300)
#define GNR_AUDENC_CAPTURE_DISABLED	MKERR(ERROR, AUDIO, GENERAL, 0x0301)

//
// Error definitions for decoder
//
#define GNR_AUDEC_PLAYBACK_ENABLED	MKERR(ERROR, AUDIO, GENERAL, 0x0302)
#define GNR_AUDEC_PLAYBACK_DISABLED	MKERR(ERROR, AUDIO, GENERAL, 0x0303)


// 
// Commands for Audio Capture
//
enum AudioCaptureCommand {CAPTURE_START,
								  CAPTURE_STOP,
								  CAPTURE_PAUSE,
								  CAPTURE_RESUME,
								  CAPTURE_SET_POSITION,
								  CAPTURE_GET_POSITION,
								  CAPTURE_BEGIN,
								  CAPTURE_COMPLETE,
								  CAPTURE_END,
								  CAPTURE_FLUSH
								 };

//
// Commands for Audio Playback
//
enum AudioPlaybackCommand {PLAYBACK_START,
									PLAYBACK_STOP,
									PLAYBACK_PAUSE,
									PLAYBACK_RESUME,
									PLAYBACK_SET_POSITION,
									PLAYBACK_GET_POSITION,
									PLAYBACK_BEGIN,
									PLAYBACK_COMPLETE,
									PLAYBACK_END,
									PLAYBACK_FLUSH
									};



#ifndef ONLY_EXTERNAL_VISIBLE


/////////////////////////////////////////////////////////////////////////////////
// Audio Encoder Device Classes (encodes from analog to digital)
/////////////////////////////////////////////////////////////////////////////////


class AudioEncoder : public AudioStreamDevice
	{
	friend class VirtualAudioEncoder;
	protected:
		virtual Error StartCapture() = 0;
		virtual Error StopCapture() = 0;
		virtual Error PauseCapture() = 0;
		virtual Error ResumeCapture() = 0;
		virtual Error BeginCapture() {GNRAISE_OK;}
		virtual Error CompleteCapture() {GNRAISE_OK;}
		virtual Error EndCapture() {GNRAISE_OK;}
		virtual Error Flush() {GNRAISE_OK;}

		virtual Error GetDataPeak(DWORD __far & info) {info = 0L; GNRAISE_OK;}
	};


class ASyncStreamAudioEncoder : public AudioEncoder, protected ASyncEmptyRequest 
	{
	private:
		ASyncInStream	*	stream;
	public:
		ASyncStreamAudioEncoder(ASyncInStream * stream) : AudioEncoder() {this->stream = stream;}
	};


class BufferStreamAudioEncoder : public AudioEncoder, protected BufferStreamCompletionRequest
	{
	private:
		BufferInputStream	*	stream;
	protected:
		DWORD bufferSize;		
	public:
		BufferStreamAudioEncoder(BufferInputStream * stream, DWORD bufferSize) : AudioEncoder()
			{this->stream = stream; this->bufferSize = bufferSize;}
	};


class VirtualAudioEncoder : public VirtualAudioStreamDevice
	{
	private:
		AudioEncoder	*	encoder;
	public:
		VirtualAudioEncoder(AudioEncoder * encoder) : VirtualAudioStreamDevice(encoder) 
			{this->encoder = encoder;}

		virtual Error DoCommand(AudioCaptureCommand com, DWORD param);
	};



/////////////////////////////////////////////////////////////////////////////////
// Audio Decoder Device Classes (decodes from digital to analog)
/////////////////////////////////////////////////////////////////////////////////

class AudioDecoder : public AudioStreamDevice
	{
	friend class VirtualAudioDecoder;
	protected:
		virtual Error StartPlayback() = 0;
		virtual Error StopPlayback() = 0;
		virtual Error PausePlayback() = 0;
		virtual Error ResumePlayback() = 0;
		virtual Error BeginPlayback() {GNRAISE_OK;}
		virtual Error CompletePlayback() {GNRAISE_OK;}
		virtual Error EndPlayback() {GNRAISE_OK;}
		virtual Error Flush() {GNRAISE_OK;}

		virtual Error GetDataPeak(DWORD __far & info) {info = 0L; GNRAISE_OK;}
	};

class ASyncStreamAudioDecoder : public AudioDecoder, protected ASyncRefillRequest
	{
	private:
		ASyncOutStream	*	stream;
	public:
		ASyncStreamAudioDecoder(ASyncOutStream * stream) : AudioDecoder() {this->stream = stream;}
	};


class BufferStreamAudioDecoder : public AudioDecoder, protected BufferStreamCompletionRequest
	{
	private:
		BufferOutputStream	*	stream;
	protected:
		DWORD bufferSize;		
	public:
		BufferStreamAudioDecoder(BufferOutputStream * stream, DWORD bufferSize) : AudioDecoder()
			{this->stream = stream; this->bufferSize = bufferSize;}
	};


class VirtualAudioDecoder : public VirtualAudioStreamDevice {
	private:
		AudioDecoder	*	decoder;
	public:
		VirtualAudioDecoder(AudioDecoder * decoder) : VirtualAudioStreamDevice(decoder)
			{this->decoder = decoder;}

		virtual Error DoCommand(AudioPlaybackCommand com, DWORD param);
	};

#endif // ONLY_EXTERNAL_VISIBLE


#endif
