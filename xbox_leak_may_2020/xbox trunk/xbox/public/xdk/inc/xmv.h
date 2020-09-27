/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       decoder.h
 *  Content:    The main definitions for the XMV decoder.
 *
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XMVDecoder XMVDecoder;

/*
 * Just use an enum for the result, only 4 possibilities anyway.
 */

typedef enum _XMVRESULT
{
    XMV_NOFRAME,
    XMV_NEWFRAME,
    XMV_ENDOFFILE,
    XMV_FAIL
}
XMVRESULT;

/*
 * Describes the general attributes of the XMV file.
 */

typedef struct _XMVVideoDescriptior
{
    // The geometry of the video.  The surface that each frame is rendered
    // onto must be exactly this size.  If width and height are zero then 
    // there is no video stream in this file.
    //
    DWORD Width;
    DWORD Height;

    // The frame rate of the file.
    DWORD FramesPerSecond;

    // The number of audio streams encoded in this file.
    DWORD AudioStreamCount;
}
XMVVideoDescriptor;

/*
 * The audio descriptor of the XMV files.
 */

typedef struct _XMVAudioDescriptor
{
    // The WAVE_FORMAT tag that describes how the audio data in the stream is
    // encoded.  This can be either WAVE_FORMAT_PCM or WAVE_FORMAT_ADPCM.
    //
    DWORD WaveFormat;

    // The number of channels in the audio stream.  Can be 1, 2, 4 or 6.
    DWORD ChannelCount;

    // The number of samples per second (Hz) in the audio stream.
    DWORD SamplesPerSecond;

    // The number of bits in each sample.  
    DWORD BitsPerSample;
}
XMVAudioDescriptor;

/*
 * Create the decoder, load the headers from the movie file and create
 * all of the video decoding buffers.
 */

HRESULT XMVCreateDecoder(char *szFileName, XMVDecoder **ppDecoder);

/*
 * Destroy decoder, freeing all memory and closing the input file.
 */

void XMVCloseDecoder(XMVDecoder *pDecoder);

/*
 * Get the general information about the file.
 */

void XMVGetVideoDescriptor(XMVDecoder *pDecoder, XMVVideoDescriptor *pVideoDescriptor);

/*
 * Get information about a specific audio stream.
 */

void XMVGetAudioDescriptor(XMVDecoder *pDecoder, DWORD AudioStream, XMVAudioDescriptor *pAudioDescriptor);

/*
 * Tell the decoder to play one of the audio tracks in the XMV file.  The 
 * caller can optionally get the IDirectSoundStream interface to manually
 * adjust the sound parameters.
 *
 * This API can be called multiple times for different streams to allow
 * them to be played at the same time.
 *
 * The Flags and pMixBins parameters are used when creating the stream.  
 * These can be safely set to zero and NULL, respectively.
 *
 * ppStream may be NULL if the caller does not want direct access to the
 * stream.
 */

HRESULT XMVEnableAudioStream(XMVDecoder *pDecoder, DWORD AudioStream, DWORD Flags, DSMIXBINS *pMixBins, IDirectSoundStream **ppStream);

/*
 * Get the next frame to display.
 *
 * This method has four possible return values:
 *
 *   XMV_NOFRAME - There is no new data and the passed-in surface
 *       is unchanged.
 *
 *   XMV_NEWFRAME - The surface contains a new frame that needs to be
 *       displayed.
 *
 *   XMV_ENDOFILE - The movie is over, throw your trash away and go home.
 *
 *   XMV_FAIL - Some major catastrophy happened that prevents the video from
 *       being decoded any further.  This only happens with major data 
 *       corruption or some kind of read error.
 *
 * The Width and Height of the surface must match that of the video.
 *
 * This method must be called occasionally even if there is no video in the
 * input file, simply pass NULL for the surface.
 */

XMVRESULT XMVGetNextFrame(XMVDecoder *pDecoder, D3DSurface *pSurface);

/*
 * Get the current frame.  This will not decode the next frame.
 *
 * This method cannot fail.  If it is called before the first call to 
 * GetNextFrame then the surface will be black.  If it's called after the end
 * of the file then it will return the very last frame.
 *
 * The passed-in surface does not have to have the same format as the one
 * passed into GetNextFrame.
 */

void XMVGetCurrentFrame(XMVDecoder *pDecoder, D3DSurface *pSurface);

#ifdef __cplusplus
}
#endif
