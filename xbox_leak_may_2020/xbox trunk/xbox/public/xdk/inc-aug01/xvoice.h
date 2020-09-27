/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       voice.h
 *  Content:    Voice include file
 ***************************************************************************/

#ifndef __XVOICE__
#define __XVOICE__

#ifdef __cplusplus
extern "C" {
#endif	

typedef struct voicecodecwaveformat_tag 
{
    WAVEFORMATEX    wfx;                    // WAVEFORMATEX data
    WORD            dwReserved;             // Reserved
    BYTE            dwTrailFrames;          // Count of trail frames
    BYTE            dwMilliSecPerFrame;     // Count of milliseconds per frame
} VOICECODECWAVEFORMAT, *PVOICECODECWAVEFORMAT, *LPVOICECODECWAVEFORMAT;

typedef const VOICECODECWAVEFORMAT *LPCVOICECODECWAVEFORMAT;

//
// Create Microphone & Headphone(XppDeviceType parameter)
//
STDAPI XVoiceCreateMediaObject(PXPP_DEVICE_TYPE XppDeviceType, DWORD dwPort, DWORD dwMaxAttachedPackets, LPWAVEFORMATEX pwfxFormat, LPXMEDIAOBJECT *ppMediaObject);

//
// Create encoder xmo
//
STDAPI XVoiceEncoderCreateMediaObject(BOOL fAutoMode, DWORD dwCodecTag, LPWAVEFORMATEX pwfxEncoded, DWORD dwThreshold, LPXMEDIAOBJECT *ppMediaObject);

//
// Create decoder xmo
//
STDAPI XVoiceDecoderCreateMediaObject(DWORD dwMaxLatency, DWORD dwCodecTag, LPWAVEFORMATEX pwfxDecoded, LPXMEDIAOBJECT *ppMediaObject);


//
// Configuration structure
//
struct QueueXMOConfig
{

	BYTE	cbSize;					     // Size of the structure
	WORD    wVoiceSamplingRate;	         // Voice sampling rate
	DWORD   dwCodecTag;                  // Codec tag   
	WORD    wMsOfDataPerPacket;          // Miliseconds of voice per compressed packet
	WORD	wMaxDelay;				     // Maximum delay for the queue in miliseconds
	WORD	wMinDelay;				     // Minimum delay for the queue in miliseconds
	WORD	wInitialHighWaterMark;       // Initial HighWaterMark for the adaptive queueing algorithm
	                                     // The acceptable interval between logical and real output time(in miliseconds)
	DOUBLE  dIdealQuality;               // Ideal quality for high water mark 
	DOUBLE  dInitialFrameStrength;       // Initial value of frame strength
};

//
// Create queuing and decompression xmo
//
XBOXAPI 
HRESULT 
WINAPI
XQueueCreateMediaObject(QueueXMOConfig *pConfig , LPXMEDIAOBJECT *ppMediaObject);

typedef struct _VOICE_ENCODER_HEADER
{
	BYTE	bType;				// packet type
	BYTE	bMsgNum;			// Message # for message
	WORD	wSeqNum;			// Sequence # for message

} VOICE_ENCODER_HEADER, *PVOICE_ENCODER_HEADER;

#ifdef __cplusplus
}
#endif

#endif  // __XVOICE__
