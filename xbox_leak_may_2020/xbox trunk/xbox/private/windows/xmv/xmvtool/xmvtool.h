/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xmvtool.h
 *  Content:    XMV conversion utility
 *
 * This utility is currently only used for testing as we're not exactly
 * sure what conversions we want to do yet.
 *
 ****************************************************************************/

// 256 streams per input file should be more than enough.
#define MAX_STREAMS   256

/*
 * Structures
 */

//
// Describes the streams in a file.
//

#define STREAM_EMPTY    0
#define STREAM_VIDEO    1
#define STREAM_AUDIO    2

typedef struct _VideoStream
{
    DWORD   StreamType;

    DWORD   CompressionID;
    DWORD   Width;
    DWORD   Height;

    DWORD   FramesPerSecond;
    DWORD   Slices;

    BOOLEAN MixedPelMotionCompensationEnable; 
    BOOLEAN LoopFilterEnabled; 
    BOOLEAN VariableSizedTransformEnabled;
    BOOLEAN XIntra8IPictureCodingEnabled;
    BOOLEAN HybridMotionVectorEnabled;
    BOOLEAN DCTTableSwitchingEnabled;
}
VideoStream;

typedef struct _AudioStream
{
    DWORD   StreamType;

    DWORD WaveFormat;
    DWORD ChannelCount;
    DWORD SamplesPerSecond;
    DWORD BitsPerSample;
}
AudioStream;

typedef union _FileContents
{
    DWORD           StreamType;
    VideoStream     VideoStream;
    AudioStream     AudioStream;
}
FileContents;

/*
 * The following function prototypes are used to manage a file.
 */

typedef BOOL  (*CheckFormat)(MediaFile *pMediaFile);
typedef void *(*OpenMedia)(MediaFile *pMediaFile, FileContents *ContentsArray);
typedef BOOL  (*GetNextVideoFrame)(void *pFile, DWORD StreamNumber, BYTE **ppVideoBuffer, BYTE *pVideoBufferMax);
typedef void  (*GetNextAudioFrame)(void *pFile, DWORD StreamNumber, DWORD FPS, BYTE **ppAudioBuffer, BYTE *pAudioBufferMax);
typedef void  (*CloseMedia)(void *pFile);

/*
 * Panic method, call for any abnormal exit from the tool.  This method
 * never returns.
 */

void Done();

/*
 * ASF parser methods.
 */

BOOL  AsfCheckFormat(MediaFile *pMediaFile);
void *AsfOpenMedia(MediaFile *pMediaFile, FileContents *ContentsArray);
BOOL  AsfGetNextVideoFrame(void *pFile, DWORD StreamNumber, BYTE **ppVideoBuffer, BYTE *pVideoBufferMax);
void  AsfGetNextAudioFrame(void *pFile, DWORD StreamNumber, DWORD FPS, BYTE **ppAudioBuffer, BYTE *pAudioBufferMax);
void  AsfCloseMedia(void *pFile);

/*
 * XMV parser methods.
 */

BOOL  XmvCheckFormat(MediaFile *pMediaFile);
void *XmvOpenMedia(MediaFile *pMediaFile, FileContents *ContentsArray);
BOOL  XmvGetNextVideoFrame(void *pFile, DWORD StreamNumber, BYTE **ppVideoBuffer, BYTE *pVideoBufferMax);
void  XmvGetNextAudioFrame(void *pFile, DWORD StreamNumber, DWORD FPS, BYTE **ppAudioBuffer, BYTE *pAudioBufferMax);
void  XmvCloseMedia(void *pFile);
