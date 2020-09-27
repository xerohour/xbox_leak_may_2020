
/****************************************************************************
 *
 *  AUDIO.C
 *
 *  Simple routines to play audio using an AVIStream to get data.
 *
 *  Uses global variables, so only one instance at a time.
 *  (Usually, there's only one sound card, so this isn't so bad.
 *
 **************************************************************************/

/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
#include "strbasic.h"

#ifdef WIN95STUFF

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <vfw.h>
#include "audio.h"
#include "muldived.h"

/*--------------------------------------------------------------+
| ****************** AUDIO PLAYING SUPPORT ******************** |
+--------------------------------------------------------------*/

static	HWAVEOUT	shWaveOut = 0;	/* Current MCI device ID */
static	LONG		slBegin;
static	LONG		slWaveBegin;
static	LONG		slCurrent;
static	LONG		slEnd;
static	LONG		slWaveEnd;
static	BOOL		sfLooping;
static	BOOL		sfPlaying = FALSE;

static LONG sTotal;
static LONG sPlayed;

#define MAX_AUDIO_BUFFERS	16
#define MIN_AUDIO_BUFFERS	2
#define AUDIO_BUFFER_SIZE	16384

static	WORD		swBuffers;	    // total # buffers
static	WORD		swBuffersOut;	    // buffers device has
static	WORD		swNextBuffer;	    // next buffer to fill
static	LPWAVEHDR	salpAudioBuf[MAX_AUDIO_BUFFERS];

static	PAVISTREAM	spavi;		    // stream we're playing
static	LONG		slSampleSize;	    // size of an audio sample

static	LONG		sdwBytesPerSec;
static	LONG		sdwSamplesPerSec;

/*---------------------------------------------------------------+
| aviaudioCloseDevice -- close the open audio device, if any.    | 
+---------------------------------------------------------------*/
void NEAR aviaudioCloseDevice(void)
	{
    WORD	w;

    if (shWaveOut) {
		while (swBuffers > 0) {
		    --swBuffers;
		    waveOutUnprepareHeader(shWaveOut, salpAudioBuf[swBuffers],
					sizeof(WAVEHDR));
	    	GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
			}	
	
		w = waveOutClose(shWaveOut);

		shWaveOut = NULL;	
    	}
	}

/*--------------------------------------------------------------+
| aviaudioOpenDevice -- get ready to play waveform data.	|
+--------------------------------------------------------------*/
BOOL FAR aviaudioOpenDevice(HWND hwnd, PAVISTREAM pavi)
	{
    WORD		w;
    LPVOID		lpFormat;
    LONG		cbFormat;
    AVISTREAMINFO	strhdr;

    if (!pavi)		// no wave data to play
		return FALSE;
    
    if (shWaveOut)	// already something playing
		return TRUE;

    spavi = pavi;

    AVIStreamInfo(pavi, &strhdr, sizeof(strhdr));

    slSampleSize = (LONG) strhdr.dwSampleSize;
    if (slSampleSize <= 0 || slSampleSize > AUDIO_BUFFER_SIZE)
		return FALSE;
    
    AVIStreamFormatSize(pavi, 0, &cbFormat);

    lpFormat = GlobalAllocPtr(GHND, cbFormat);
    if (!lpFormat)
		return FALSE;

    AVIStreamReadFormat(pavi, 0, lpFormat, &cbFormat);

    sdwSamplesPerSec = ((LPWAVEFORMAT) lpFormat)->nSamplesPerSec;
    sdwBytesPerSec = ((LPWAVEFORMAT) lpFormat)->nAvgBytesPerSec;
    
    w = waveOutOpen(&shWaveOut, (UINT)WAVE_MAPPER, lpFormat,
			(DWORD) (UINT) hwnd, 0L, CALLBACK_WINDOW);

    //
    // Maybe we failed because someone is playing sound already.
    // Shut any sound off, and try once more before giving up.
    //
    if (w) {
		sndPlaySound(NULL, 0);
		w = waveOutOpen(&shWaveOut, (UINT)WAVE_MAPPER, lpFormat,
			(DWORD) (UINT)hwnd, 0L, CALLBACK_WINDOW);
    	}
		    
    if (w != 0) {
		/* Show error message here? */	
		return FALSE;
    	}
    
    for (swBuffers = 0; swBuffers < MAX_AUDIO_BUFFERS; swBuffers++) {
		if (!(salpAudioBuf[swBuffers] = 
			(LPWAVEHDR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE, 
				(DWORD)(sizeof(WAVEHDR) + AUDIO_BUFFER_SIZE))))
		    break;
		
		salpAudioBuf[swBuffers]->dwFlags = WHDR_DONE;
		salpAudioBuf[swBuffers]->lpData = (LPSTR) salpAudioBuf[swBuffers] 
							    + sizeof(WAVEHDR);
		salpAudioBuf[swBuffers]->dwBufferLength = AUDIO_BUFFER_SIZE;
		if (!waveOutPrepareHeader(shWaveOut, salpAudioBuf[swBuffers], 
						sizeof(WAVEHDR)))
		    continue;
	
		GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
		break;
    	}
    
    if (swBuffers < MIN_AUDIO_BUFFERS) {
		aviaudioCloseDevice();
		return FALSE;
    	}

    swBuffersOut = 0;
    swNextBuffer = 0;

    sfPlaying = FALSE;
    
    return TRUE;
	}


/*--------------------------------------------------------------+
| aviaudioTime -						|
| Return the time in milliseconds corresponding to the		|
| currently playing audio sample, or -1 if no audio is playing.	|
|								|
| WARNING: Some sound cards are pretty inaccurate!		|
+--------------------------------------------------------------*/
LONG FAR aviaudioTime(void)
	{
    MMTIME	mmtime;
    
    if (!sfPlaying)
	return -1;

    mmtime.wType = TIME_SAMPLES;
    
    waveOutGetPosition(shWaveOut, &mmtime, sizeof(mmtime));

    if (mmtime.wType == TIME_SAMPLES)
		return AVIStreamSampleToTime(spavi, slBegin)
			+ muldiv32(mmtime.u.sample, 1000, sdwSamplesPerSec);
    else if (mmtime.wType == TIME_BYTES)
		return AVIStreamSampleToTime(spavi, slBegin)
			+ muldiv32(mmtime.u.cb, 1000, sdwBytesPerSec);
    else
		return -1;
	}


/*--------------------------------------------------------------+
| aviaudioiFillBuffers -					|
| Fill up any empty audio buffers and ship them out to the	|
| device.							|
+--------------------------------------------------------------*/
BOOL NEAR aviaudioiFillBuffers(void)
	{
    LONG		lRead;
    WORD		w;
    LONG		lSamplesToPlay;
    
    /* We're not playing, so do nothing. */
    if (!sfPlaying)
		return TRUE;
    
    while (swBuffersOut < swBuffers) {		
		if (sPlayed >= sTotal) {
			if (sfLooping) {
				/* Looping, so go to the beginning. */
				slCurrent = slBegin;
				sPlayed   = 0;
			} else {
				break;
				}
			}

		if (slCurrent >= slWaveEnd) {
	    	slCurrent = slWaveBegin;			
			}

		/* Figure out how much data should go in this buffer */
		lSamplesToPlay = slWaveEnd - slCurrent;
		if (lSamplesToPlay + sPlayed > sTotal) {
			lSamplesToPlay = sTotal - sPlayed;
			}

		if (lSamplesToPlay > AUDIO_BUFFER_SIZE / slSampleSize)
	    	lSamplesToPlay = AUDIO_BUFFER_SIZE / slSampleSize;


		AVIStreamRead(spavi, slCurrent, lSamplesToPlay,
			      salpAudioBuf[swNextBuffer]->lpData,
			      AUDIO_BUFFER_SIZE,
			      &salpAudioBuf[swNextBuffer]->dwBufferLength,
			      &lRead);
	
		if (lRead != lSamplesToPlay) {
		    return FALSE;
			}
		slCurrent += lRead;
		sPlayed   += lRead;

		w = waveOutWrite(shWaveOut, salpAudioBuf[swNextBuffer],sizeof(WAVEHDR));
	
		if (w != 0) {
	    	return FALSE;
			}
	
		++swBuffersOut;
		++swNextBuffer;
		if (swNextBuffer >= swBuffers) {
		    swNextBuffer = 0;
			}
   		}

    if (swBuffersOut == 0 && sPlayed >= sTotal)
		aviaudioStop();
    
    /* We've filled all of the buffers we can or want to. */
    return TRUE;
	}

/*--------------------------------------------------------------+
| aviaudioPlay -- Play audio, starting at a given frame		|
|								|
+--------------------------------------------------------------*/
BOOL FAR aviaudioPlay(
	HWND hwnd, PAVISTREAM pavi, 
	LONG lCur, LONG lStart, LONG lEnd, 
	BOOL fWait, BOOL repeat)
	{
    LONG aviStart = AVIStreamStartTime(pavi);
    LONG aviEnd   = AVIStreamEndTime(pavi);
	LONG aviLen   = aviEnd-aviStart;
	
    if (!aviaudioOpenDevice(hwnd,pavi)) return FALSE;    

	if (!sfPlaying) {
		waveOutPause(shWaveOut);
		}
	
	if (lStart < aviStart) {
		LONG d = (aviStart-lStart)/aviLen + 1;
		lStart += d * aviLen;
		lEnd   += d * aviLen;
		lCur   += d * aviLen;
		}
	if (lStart > aviEnd) {
		LONG d = (lStart-aviEnd)/aviLen + 1;
		lStart -= d * aviLen;
		lEnd   -= d * aviLen;
		lCur   -= d * aviLen;
		}
	
	//sTotal  = ((lEnd-lStart)*sdwSamplesPerSec)/1000;
	//sPlayed = ((lCur-lStart)*sdwSamplesPerSec)/1000;
	sTotal  = MulDiv32((lEnd-lStart), sdwSamplesPerSec, 1000);
	sPlayed = MulDiv32((lCur-lStart), sdwSamplesPerSec, 1000);
	
	slBegin     = AVIStreamTimeToSample(pavi,lStart%aviLen);
	slCurrent   = AVIStreamTimeToSample(pavi,lCur%aviLen);
	slEnd       = AVIStreamTimeToSample(pavi,lEnd%aviLen);
	slWaveBegin = AVIStreamStart(pavi);
	slWaveEnd   = AVIStreamEnd(pavi);
	    
    sfLooping = repeat;

	if (!sfPlaying) {
    	sfPlaying = TRUE;
    	aviaudioiFillBuffers();		
		}

    //
    // Now unpause the audio and away it goes!
    //
    waveOutRestart(shWaveOut);
    
    //
    // Caller wants us not to return until play is finished
    //
    if (fWait) {
		while (swBuffersOut > 0) {
		    MSG msg;
			while (PeekMessage(&msg,hwnd,0,0,TRUE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
    	}
    
    return TRUE;
	}

/*--------------------------------------------------------------+
| aviaudioMessage -- handle wave messages received by		|
| window controlling audio playback.  When audio buffers are	|
| done, this routine calls aviaudioiFillBuffers to fill them	|
| up again.							|
+--------------------------------------------------------------*/
void FAR aviaudioMessage(HWND hwnd, unsigned msg, WORD wParam, LONG lParam)
	{
    if (msg == MM_WOM_DONE) {
		--swBuffersOut;
		aviaudioiFillBuffers();
    	}
	}


/*--------------------------------------------------------------+
| aviaudioStop -- stop playing, close the device.		|
+--------------------------------------------------------------*/
void FAR aviaudioStop(void)
	{
    WORD	w;

    if (shWaveOut != 0) {
		w = waveOutReset(shWaveOut);
		sfPlaying = FALSE;	
		aviaudioCloseDevice();
    	}    
	}

#endif // WIN95STUFF
