/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xvmparser.c
 *  Content:    XMV file parser
 *
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fileio.h"
#include "xmvtool.h"

#include "..\inc\xmvformat.h"

//
// Manages the information needed to walk over an individual stream.
//

typedef struct _XsfAudioStream
{
    // The amount of audio data remaining in this stream.
    DWORD AudioDataSize;

    // The number of audio bytes remaining.
    DWORD AudioStreamRemaining;

    // The number of bytes to skip at the end of this buffer
    // to get to the next one.
    //
    DWORD BytesToSkip;
    
    // The total size of the next buffer / 4096.
    DWORD NextPacketSize;

    // The stream that contains the audio data.
    MediaStream Stream;
}
XsfAudioStream;

//
// Basic data structure we use to walk over the file
//

typedef struct _XsfFile
{
    // Number of frames in this buffer.
    DWORD FrameCount;

    // The amount of video data that remains.
    DWORD VideoStreamRemaining;

    // How many bytes to skip to get to the next stream.
    DWORD BytesToSkip;

    // Video stream.
    MediaStream VideoStream;

    // The total size of the next packet.
    DWORD NextPacketSize;

    // The number of audio streams.
    DWORD AudioStreamCount;

    // Helpers to iterate over all of the streams.
    XsfAudioStream AudioStreams[0];
}
XsfFile;

/*
 * Check the file to see if it's one of us.
 */

BOOL  XmvCheckFormat
(
    MediaFile *pMediaFile
)
{
    MediaStream Stream;
    XMVPacketHeader *pPacketHeader;
    XMVVideoDataHeader *pVideoDataHeader;

    OpenMediaStream(pMediaFile, &Stream);

    // Don't fall off the end.
    if (GetMediaSize(&Stream) < sizeof(XMVPacketHeader) + sizeof(XMVVideoDataHeader))
    {
        return FALSE;
    }

    // Read the first object.
    pPacketHeader = (XMVPacketHeader *)ReadStream(&Stream, sizeof(XMVPacketHeader));
    pVideoDataHeader = (XMVVideoDataHeader *)ReadStream(&Stream, sizeof(XMVVideoDataHeader));

    return pVideoDataHeader->MagicCookie == XMV_MAGIC_COOKIE
        && pVideoDataHeader->FileVersion == XMV_FILE_VERSION;
}

/*
 * Process the header.
 */

void *XmvOpenMedia
(
    MediaFile *pMediaFile, 
    FileContents *ContentsArray
)
{
    XsfFile *pFile;

    MediaStream Stream;
    DWORD i;

    XMVPacketHeader *pPacketHeader;
    XMVVideoDataHeader *pVideoDataHeader;
    XMVAudioDataHeader *pAudioDataHeader;

    DWORD AudioStreamCount; 

    DWORD VideoStreamSize;
    DWORD AudioStreamSize[MAX_STREAMS];
    
    DWORD FirstPacketSize;

    OpenMediaStream(pMediaFile, &Stream);

    // Read the video information.
    pVideoDataHeader = (XMVVideoDataHeader *)ReadStream(&Stream, sizeof(XMVVideoDataHeader));

    AudioStreamCount = pVideoDataHeader->AudioStreamCount;
    FirstPacketSize  = pVideoDataHeader->ThisPacketSize;

    // Fill the video info into the end of the contents array after where 
    // we'll put all of the audio stuff.  
    //
    if (pVideoDataHeader->Width != 0)
    {
        ContentsArray[AudioStreamCount].StreamType                  = STREAM_VIDEO;
        ContentsArray[AudioStreamCount].VideoStream.CompressionID   = '2VMW';
        ContentsArray[AudioStreamCount].VideoStream.FramesPerSecond = pVideoDataHeader->FramesPerSecond;
        ContentsArray[AudioStreamCount].VideoStream.Width           = pVideoDataHeader->Width;
        ContentsArray[AudioStreamCount].VideoStream.Height          = pVideoDataHeader->Height;
    }

    // Do all of the audio streams.
    for (i = 0; i < AudioStreamCount; i++)
    {
        pAudioDataHeader = (XMVAudioDataHeader *)ReadStream(&Stream, sizeof(XMVAudioDataHeader));

        ContentsArray[i].StreamType                   = STREAM_AUDIO;
        ContentsArray[i].AudioStream.WaveFormat       = pAudioDataHeader->WaveFormat;
        ContentsArray[i].AudioStream.ChannelCount     = pAudioDataHeader->ChannelCount;
        ContentsArray[i].AudioStream.SamplesPerSecond = pAudioDataHeader->SamplesPerSecond;
        ContentsArray[i].AudioStream.BitsPerSample    = pAudioDataHeader->BitsPerSample;
    }

    // Allocate the object.
    pFile = (XsfFile *)malloc(sizeof(XsfFile) + sizeof(XsfAudioStream) * AudioStreamCount);

    if (!pFile)
    {
        fprintf(stderr, "ERROR: Unable to allocate memory.\n");
        Done();
    }

    // Fill it in.
    pFile->FrameCount           = 0;
    pFile->BytesToSkip          = 0;
    pFile->VideoStreamRemaining = 0;
    pFile->NextPacketSize       = FirstPacketSize - (DWORD)GetStreamPosition(&Stream);

    CloneMediaStream(&Stream, &pFile->VideoStream);

    pFile->AudioStreamCount     = AudioStreamCount;

    for (i = 0; i < AudioStreamCount; i++)
    {
        pFile->AudioStreams[i].AudioDataSize        = 0;
        pFile->AudioStreams[i].BytesToSkip          = 0;
        pFile->AudioStreams[i].AudioStreamRemaining = 0;
        pFile->AudioStreams[i].NextPacketSize       = FirstPacketSize - (DWORD)GetStreamPosition(&Stream);

        CloneMediaStream(&Stream, &pFile->AudioStreams[i].Stream);
    }

    return pFile;
}

/*
 * Destroy the file object.  The file will get closed by the main routine.
 */

void  XmvCloseMedia
(
    void *pFile
)
{
    free(pFile);
}

/*
 * Get one video frame from a video stream.
 */

BOOL XmvGetNextVideoFrame
(
    void *pFileHandle, 
    DWORD StreamNumber, 
    BYTE **ppVideoBuffer, 
    BYTE *pVideoBufferMax
)
{
    XsfFile *pFile;
    MediaStream *pStream;

    XMVPacketHeader *pPacketHeader;
    XMVVideoPacketHeader *pVideoPacketHeader;

    DWORD ThisPacketSize;
    DWORD NextPacketSize;
    DWORD VideoFrameSize;
    DWORD ReadSize;

    BYTE *pVideoFrame;

    LONGLONG InitialPacketOffset;

    pFile = (XsfFile *)pFileHandle;
    pStream = &pFile->VideoStream;

    if (!pFile->FrameCount)
    {
        // Skip the rest of this packet.
        SkipStreamData(pStream, pFile->BytesToSkip + pFile->VideoStreamRemaining);

        // Save our current spot in the stream.
        InitialPacketOffset = GetStreamPosition(pStream);

        // Figure out the size of the next packet.
        if (!pFile->NextPacketSize)
        {
            return FALSE;
        }

        ThisPacketSize = pFile->NextPacketSize;

        // Read the next packet size and save it.
        pPacketHeader = (XMVPacketHeader *)ReadStream(pStream, sizeof(XMVPacketHeader));
        pFile->NextPacketSize = pPacketHeader->NextPacketSize;

        // Load the video packet header.
        pVideoPacketHeader = (XMVVideoPacketHeader *)ReadStream(pStream, sizeof(XMVVideoPacketHeader));

        // Save the video stream length and the frame count.
        pFile->VideoStreamRemaining = pVideoPacketHeader->VideoStreamLength;
        pFile->FrameCount           = pVideoPacketHeader->FrameCount;

        // Skip past the audio stream info.
        SkipStreamData(pStream, pFile->AudioStreamCount * sizeof(XMVAudioStreamHeader));

        // Figure out how much goo to skip at the end of the video stream to
        // get to the next packet.
        //
        pFile->BytesToSkip = ThisPacketSize - (DWORD)(GetStreamPosition(pStream) - InitialPacketOffset) - pFile->VideoStreamRemaining;
    }

    // Save the current offset so we can remember how much space we've
    // consumed.
    //
    InitialPacketOffset = GetStreamPosition(pStream);

    // Read the frame size.
    VideoFrameSize = ((XMVVideoFrame *)ReadStream(pStream, sizeof(XMVVideoFrame)))->FrameSize;

    if (*ppVideoBuffer + VideoFrameSize > pVideoBufferMax)
    {
        fprintf(stderr, "ERROR: The buffer is too small to encode this data, increase its size.\n");
        Done();
    }

    // Read the frame.
    while (VideoFrameSize)
    {
        ReadSize = VideoFrameSize > FILE_MAPPING_GRANULARITY ? FILE_MAPPING_GRANULARITY : VideoFrameSize;

        pVideoFrame = ReadStream(pStream, ReadSize);

        // Copy it.
        memcpy(*ppVideoBuffer, pVideoFrame, ReadSize);

        *ppVideoBuffer += ReadSize;

        VideoFrameSize -= ReadSize;
    }


    // Remember what we did.
    pFile->FrameCount--;
    pFile->VideoStreamRemaining -= (DWORD)(GetStreamPosition(pStream) - InitialPacketOffset);

    return TRUE;
}

/*
 * Get one frame's worth of audio
 */

void XmvGetNextAudioFrame
(
    void *pFile, 
    DWORD StreamNumber, 
    DWORD FPS, 
    BYTE **ppAudioBuffer, 
    BYTE *pAudioBufferMax
)
{
}

