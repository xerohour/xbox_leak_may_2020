/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xmvtool.c
 *  Content:    XMV conversion utility
 *
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <setjmp.h>
#include <xboxverp.h>

#include "..\inc\xmvformat.h"

#include "fileio.h"
#include "xmvtool.h"

/*
 * Parsed versions of the command-line parameters.
 */

#define COMMAND_INFO    1
#define COMMAND_CONVERT 2

DWORD g_Command;
char *g_szInputFile;
char *g_szOutputFile;

DWORD g_OutputBufferSize = 128 * 1024;

/*
 * Globals
 */

// The following array maps from the stream number in the output file
// to the method and stream number we need to use to get the stream data.
//
struct _StreamMap
{
    GetNextVideoFrame pfnGetNextVideoFrame;
    void *pFileHandle;
    DWORD VideoStream;

    VideoStream *pVideoStreamInfo;

    struct _AudioMap
    {
        GetNextAudioFrame pfnGetNextAudioFrame;
        void *pFileHandle;
        DWORD AudioStream;

        AudioStream *pAudioStreamInfo;
    }
    AudioMap[MAX_STREAMS];
}
g_StreamMap;

// Describes an input file.
#define FILEDESC_UNKNOWN    0
#define FILEDESC_ASF        1
#define FILEDESC_XMV        2

typedef struct _InputFile
{
    DWORD FileType;

    void *pFile;

    OpenMedia         pfnOpenMedia;
    GetNextVideoFrame pfnGetNextVideoFrame;
    GetNextAudioFrame pfnGetNextAudioFrame;
    CloseMedia        pfnCloseMedia;
}
InputFile;

MediaFile g_InputFile;
InputFile g_InputFileDesc;

HANDLE  g_hOutputFile = INVALID_HANDLE_VALUE;

// We use setjmp/longjump to avoid having to manually unwind
// the stack when we get an error or finish processing the file.  This
// works fine because we never store anything that needs to be destroyed
// in local variables so no function in this file has any cleanup code
// except in the main routine after the setjmp.

jmp_buf g_Done;

// Memory allocations for the stream buffer arrays and the actual
// stream buffers themselves.
//
void *g_pStreamBufferArrays;
void *g_pStreamBuffers;

/*
 * We jump out of here whenever anything bad happens to avoid having
 * to manually unwind the stack.
 */

void Done()
{
    longjmp(g_Done, -1);
}

/* 
 * Displays the usage for this tool. 
 */ 

void Usage()
{
    fprintf(stderr,
            "\nxmvtool version " VER_PRODUCTVERSION_STR "\n\n"
            "Usage: xmvtool <command> <parameters>\n\n"
            "    xmvtool info <input file> - dump information about the input file\n\n"
            "    xmvtool convert [options] <input file> <output file>\n\n"
            "        /b <size>  set the size of the decoder's input buffer, in Kilobytes (512 max, multiple of 4)\n"
            "\n");
}

/*
 * Parse the command line and initialize all of the option global variables.
 */

// All this stuff is temporary until I decide exactly how to handle the nasty
// command lines we're going to use.
//
BOOL ParseCommandLine
(
    int argc, 
    char **argv
)
{
    int i;
    DWORD BufferSize;

    if (argc < 2)
    {
        return FALSE;
    }

    // The first option must be a command...
    if (!_stricmp(argv[1], "info"))
    {
        g_Command = COMMAND_INFO;

        if (argc != 3)
        {
            return FALSE;
        }

        g_szInputFile = argv[2];
    }
    else if (!strcmp(argv[1], "convert"))
    {
        g_Command = COMMAND_CONVERT;

        i = 2;

        if (argc == 6)
        {
            if (!_stricmp(argv[2], "/b") || !_stricmp(argv[2], "-b"))
            {
                BufferSize = atol(argv[3]);

                if (BufferSize == 0 || BufferSize > 512 || BufferSize % 4)
                {
                    return FALSE;
                }

                g_OutputBufferSize = BufferSize * 1024;
            }

            i = 4;
        }

        if (argc - i != 2)
        {
            return FALSE;
        }

        g_szInputFile = argv[i];
        g_szOutputFile = argv[i + 1];
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*
 * Open the files and prepares for reading.
 */

void OpenFiles()
{
    //
    // Open the input file.
    //

    OpenMediaFile(&g_InputFile, g_szInputFile);

    if (AsfCheckFormat(&g_InputFile))
    {
        g_InputFileDesc.FileType             = FILEDESC_ASF;
        g_InputFileDesc.pfnOpenMedia         = AsfOpenMedia;
        g_InputFileDesc.pfnGetNextVideoFrame = AsfGetNextVideoFrame;
        g_InputFileDesc.pfnGetNextAudioFrame = AsfGetNextAudioFrame;
        g_InputFileDesc.pfnCloseMedia        = AsfCloseMedia;
    }
    else if (XmvCheckFormat(&g_InputFile))
    {
        g_InputFileDesc.FileType             = FILEDESC_XMV;
        g_InputFileDesc.pfnOpenMedia         = XmvOpenMedia;
        g_InputFileDesc.pfnGetNextVideoFrame = XmvGetNextVideoFrame;
        g_InputFileDesc.pfnGetNextAudioFrame = XmvGetNextAudioFrame;
        g_InputFileDesc.pfnCloseMedia        = XmvCloseMedia;
    }
    else
    {
        g_InputFileDesc.FileType = FILEDESC_UNKNOWN;
    }

    // Make sure this is an ASF file.
    if (g_InputFileDesc.FileType == FILEDESC_UNKNOWN)
    {
        fprintf(stderr, "ERROR: Unknown media type\n");
        Done();
    }

    //
    // Open the output file.
    //

    if (g_szOutputFile)
    {
        // Open the file.
        g_hOutputFile = CreateFileA(g_szOutputFile, 
                                    GENERIC_WRITE, 
                                    0, 
                                    NULL, 
                                    CREATE_ALWAYS, 
                                    FILE_ATTRIBUTE_NORMAL, 
                                    NULL);

        if (g_hOutputFile == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, 
                    "ERROR: Unable to open ouput file '%s'.\n", 
                    g_szOutputFile);

            // Does not return.
            Done();
        }
    }
}

/*
 * Dump the information about the input file
 */

void Info()
{
    FileContents ContentsArray[MAX_STREAMS];
    DWORD i;

    // Get the file information.
    memset(ContentsArray, 0, sizeof(ContentsArray));

    g_InputFileDesc.pFile = g_InputFileDesc.pfnOpenMedia(&g_InputFile, ContentsArray);

    if (g_InputFileDesc.FileType == FILEDESC_ASF)
    {
        printf("%s - Asf File:\n\n", g_szInputFile);
    }
    else
    {
        printf("%s - Xmv File:\n\n", g_szInputFile);
    }

    // Dump it.
    for (i = 0; i < MAX_STREAMS; i++)
    {
        if (ContentsArray[i].StreamType == STREAM_VIDEO)
        {
            printf("    ---- Stream        : %d\n", i);
            printf("    Compresssion ID    : %c%c%c%c\n", ContentsArray[i].VideoStream.CompressionID, ContentsArray[i].VideoStream.CompressionID >> 8, ContentsArray[i].VideoStream.CompressionID >> 16, ContentsArray[i].VideoStream.CompressionID >> 24);
            printf("    Frames per second  : %d\n", ContentsArray[i].VideoStream.FramesPerSecond);
            printf("    Width              : %d\n", ContentsArray[i].VideoStream.Width);
            printf("    Height             : %d\n\n", ContentsArray[i].VideoStream.Height);
        }
        else if (ContentsArray[i].StreamType == STREAM_AUDIO)
        {
            printf("    ---- Stream        : %d\n", i);
            printf("    Wave Format        : %X\n", ContentsArray[i].AudioStream.WaveFormat);
            printf("    Channel count      : %d\n", ContentsArray[i].AudioStream.ChannelCount);
            printf("    Samples Per Second : %d\n", ContentsArray[i].AudioStream.SamplesPerSecond);
            printf("    Bits Per Sample    : %d\n\n", ContentsArray[i].AudioStream.BitsPerSample);
        }
    }
}

/*
 * Process each frame, pack them into a buffer and write to disk.
 *
 * A video packet gets built through the following process:
 *
 *  1) The video and audio data for a single frame get collected and stored
 *     in the appropriate video/audio stream staging buffers.  These buffers
 *     are considerably oversized.
 *
 *  2) We calculate whether the current video packet would overflow the
 *    output buffer size if this frame were included.  If not, then we
 *     move on to step 1 for the next frame.
 *
 *  3) If there is an existing pending packet then it gets flushed to disk
 *     (step 5) and the current staging buffers become pending.
 *
 *  4) The next video packet is assembled.
 *
 *  5) Update the size of the next video packet in this one and write it
 *     to disk.
 *
 * To do this, we need two buffers for the video stream and each audio
 * stream.  The video and audio streams need to be oversized to handle
 * storing audio/video data until we figure out whether it actually fits
 * in this packet...otherwise there is a chance that the buffer could
 * overflow.
 *
 */

void ProcessFrames
(
    DWORD VideoStreamCount,
    DWORD AudioStreamCount
)
{
    BYTE *pVideoBufferHeaderNext;

    XMVPacketHeader      *pPacketHeader;
    XMVPacketHeader      *pPendingPacketHeader;
    XMVVideoDataHeader   *pVideoDataHeader;
    XMVAudioDataHeader   *pAudioDataHeader;
    XMVVideoPacketHeader *pVideoPacketHeader;
    XMVVideoPacketHeader *pPendingVideoPacketHeader;

    DWORD CurrentPacketSize;
    DWORD PotentialPacketSize;
    DWORD PaddedCurrentPacketSize;

    DWORD VideoBufferSize;
    DWORD AudioBufferSize;
    DWORD PendingVideoBufferSize;
    DWORD PendingVideoPaddingSize;

    DWORD iStream, Size, Written;
    BYTE *pb;

    BYTE *pVideoBufferStart;
    BYTE *pVideoBufferPending;
    BYTE *pVideoBufferPrevious;
    BYTE *pVideoBufferCurrent;
    BYTE *pVideoBufferSwap;

    BYTE **pAudioBufferStart;
    BYTE **pAudioBufferPending;
    BYTE **pAudioBufferPrevious;
    BYTE **pAudioBufferCurrent;
    BYTE **pAudioBufferSwap;

    DWORD FrameCount;
    DWORD ThisFrameSize;
    DWORD StreamCount;
    DWORD OutputBufferWriteCount;
    DWORD FramesPerSecond;
    DWORD PaddingSize;

    ThisFrameSize = 0;
    OutputBufferWriteCount = 0;
    VideoBufferSize = 0;
    AudioBufferSize = 0;

    StreamCount = VideoStreamCount + AudioStreamCount;

    if (!VideoStreamCount)
    {
        FramesPerSecond = 30;
    }
    else
    {
        FramesPerSecond = g_StreamMap.pVideoStreamInfo->FramesPerSecond;
    }

    //
    // Allocate the output buffers.  The memory must be stored in globals
    // in order to be destroyed correctly.
    //

    assert(!g_pStreamBufferArrays && !g_pStreamBuffers);

    g_pStreamBufferArrays = malloc(sizeof(DWORD) * AudioStreamCount * 4);

    if (!g_pStreamBufferArrays)
    {
        fprintf(stderr, "ERROR: Unable to allocate an output buffers.\n");

        // Does not return.
        Done();
    }

    pAudioBufferStart    = (BYTE **)g_pStreamBufferArrays;
    pAudioBufferPending  = pAudioBufferStart + StreamCount;
    pAudioBufferPrevious = pAudioBufferPending + StreamCount;
    pAudioBufferCurrent  = pAudioBufferPrevious + StreamCount;

    g_pStreamBuffers = VirtualAlloc(NULL,
                                    g_OutputBufferSize * 2 * (AudioStreamCount + 1) * 2,
                                    MEM_COMMIT,
                                    PAGE_READWRITE);

    if (!g_pStreamBuffers)
    {
        fprintf(stderr, "ERROR: Unable to allocate an output buffers.\n");

        // Does not return.
        Done();
    }

    // Set up the video buffers.
    pb = (BYTE *)g_pStreamBuffers;

    pVideoBufferStart = pb;
    pb += g_OutputBufferSize * 2;

    pVideoBufferPending = pb;
    pb += g_OutputBufferSize * 2;

    // Set up the audio buffers.
    for (iStream = 0; iStream < AudioStreamCount; iStream++)
    {
        pAudioBufferStart[iStream] = pb;
        pb += g_OutputBufferSize * 2;

        pAudioBufferPending[iStream] = pb;
        pb += g_OutputBufferSize * 2;
    }

    do
    {
#if DBG
        memset(pVideoBufferStart, 0x69, g_OutputBufferSize * 2);

        for (iStream = 0; iStream < AudioStreamCount; iStream++)
        {
            memset(pAudioBufferStart[iStream], 0x69, g_OutputBufferSize * 2);
        }
#endif

        FrameCount = 0;

        // Use locals until we can transfer any tail frames out of the pending
        // buffers.
        //
        pVideoBufferHeaderNext = pVideoBufferStart;

        // The first packet contains a whole buncha-buncha nifty information
        if (OutputBufferWriteCount == 0)
        {
            pVideoDataHeader        = (XMVVideoDataHeader *)pVideoBufferHeaderNext;
            pVideoBufferHeaderNext += sizeof(XMVVideoDataHeader);

            // Fill it in.
            pVideoDataHeader->MagicCookie                      = XMV_MAGIC_COOKIE;
            pVideoDataHeader->FileVersion                      = XMV_FILE_VERSION;
            pVideoDataHeader->FramesPerSecond                  = FramesPerSecond;
            pVideoDataHeader->Slices                           = g_StreamMap.pVideoStreamInfo->Slices;
            pVideoDataHeader->MixedPelMotionCompensationEnable = g_StreamMap.pVideoStreamInfo->MixedPelMotionCompensationEnable;
            pVideoDataHeader->LoopFilterEnabled                = g_StreamMap.pVideoStreamInfo->LoopFilterEnabled;
            pVideoDataHeader->VariableSizedTransformEnabled    = g_StreamMap.pVideoStreamInfo->VariableSizedTransformEnabled;
            pVideoDataHeader->XIntra8IPictureCodingEnabled     = g_StreamMap.pVideoStreamInfo->XIntra8IPictureCodingEnabled;
            pVideoDataHeader->HybridMotionVectorEnabled        = g_StreamMap.pVideoStreamInfo->HybridMotionVectorEnabled;
            pVideoDataHeader->DCTTableSwitchingEnabled         = g_StreamMap.pVideoStreamInfo->DCTTableSwitchingEnabled;
            pVideoDataHeader->Width                            = VideoStreamCount ? g_StreamMap.pVideoStreamInfo->Width : 0;
            pVideoDataHeader->Height                           = VideoStreamCount ? g_StreamMap.pVideoStreamInfo->Height : 0;
            pVideoDataHeader->RequiredBufferSize               = g_OutputBufferSize;
            pVideoDataHeader->AudioStreamCount                 = AudioStreamCount;

            // Save the audio stream information.
            for (iStream = 0; iStream < AudioStreamCount; iStream++)
            {
                pAudioDataHeader        = (XMVAudioDataHeader *)pVideoBufferHeaderNext;
                pVideoBufferHeaderNext += sizeof(XMVAudioDataHeader);

                pAudioDataHeader->WaveFormat       = (WORD)g_StreamMap.AudioMap[iStream].pAudioStreamInfo->WaveFormat;
                pAudioDataHeader->ChannelCount     = (WORD)g_StreamMap.AudioMap[iStream].pAudioStreamInfo->ChannelCount;
                pAudioDataHeader->SamplesPerSecond = g_StreamMap.AudioMap[iStream].pAudioStreamInfo->SamplesPerSecond;
                pAudioDataHeader->BitsPerSample    = g_StreamMap.AudioMap[iStream].pAudioStreamInfo->BitsPerSample;
            }
        }

        // Save space for the packet headers.
        pPacketHeader = (XMVPacketHeader *)pVideoBufferHeaderNext;
        pVideoBufferHeaderNext += sizeof(XMVPacketHeader);

        // Save space for the video packet header.  This will get filled in
        // just before this packet gets written to disk.
        //
        pVideoPacketHeader = (XMVVideoPacketHeader *)pVideoBufferHeaderNext;
        pVideoBufferHeaderNext += sizeof(XMVVideoPacketHeader) + sizeof(XMVAudioStreamHeader) * AudioStreamCount;

        // If there is a frame hanging off the end of the previous packet then
        // we need to copy it to the beginning of this packet then zero pad
        // out what we copied.
        //
        if (ThisFrameSize)
        {
            // Our state is:
            //
            //  pVideoBufferStart hold the start of the current video stream
            //  pVideoBufferHeaderNext holds where to write the next data to 
            //      the video stream
            //  pAudioBufferStart[0..] holds empty audio stream buffers
            //  p*BufferPrevious holds a pointer to the beginning of the last
            //      frame data for each buffer
            //  p*BufferCurrent holds a pointer to the end of the last frame 
            //      data for each buffer
            //
            Size = pVideoBufferCurrent - pVideoBufferPrevious;

            memcpy(pVideoBufferHeaderNext, pVideoBufferPrevious, Size);

            pVideoBufferPrevious = pVideoBufferHeaderNext;
            pVideoBufferCurrent  = pVideoBufferHeaderNext + Size;

            for (iStream = 0; iStream < AudioStreamCount; iStream++)
            {
                Size = pAudioBufferCurrent[iStream] - pAudioBufferPrevious[iStream];

                memcpy(pAudioBufferStart[iStream], pAudioBufferPrevious[iStream], Size);

                pAudioBufferPrevious[iStream] = pAudioBufferStart[iStream];
                pAudioBufferCurrent[iStream]  = pAudioBufferStart[iStream] + Size;

                ThisFrameSize += Size;
            }

            FrameCount++;
        }
        else
        {
            pVideoBufferPrevious = pVideoBufferHeaderNext;
            pVideoBufferCurrent  = pVideoBufferHeaderNext;

            for (iStream = 0; iStream < AudioStreamCount; iStream++)
            {
                pAudioBufferPrevious[iStream] = pAudioBufferStart[iStream];
                pAudioBufferCurrent[iStream]  = pAudioBufferStart[iStream];
            }
        }

        // Remember the size of what we've done so far.
        CurrentPacketSize = pVideoBufferHeaderNext - pVideoBufferStart + ThisFrameSize;

        // Loop through the input stream until we have enough data to fill
        // the output buffer.
        //
        for(;;)
        {            
            // Remember the frame we just finished.
            pVideoBufferPrevious = pVideoBufferCurrent;

            memcpy(pAudioBufferPrevious, pAudioBufferCurrent, sizeof(DWORD) * AudioStreamCount);

            // Reset.
            ThisFrameSize = 0;

            // Get the next video frame.
            if (VideoStreamCount)
            {
                // Save room for the header.
                pVideoBufferCurrent += sizeof(XMVVideoFrame);

                if (!g_StreamMap.pfnGetNextVideoFrame(g_StreamMap.pFileHandle,
                                                      g_StreamMap.VideoStream,
                                                      &pVideoBufferCurrent,
                                                      pVideoBufferStart + g_OutputBufferSize * 2))
                {
                    break;
                }

                ThisFrameSize += pVideoBufferCurrent - pVideoBufferPrevious;

                PaddingSize = (ThisFrameSize + 3) & ~3 - ThisFrameSize;
                memset(pVideoBufferCurrent, 0, PaddingSize);

                ThisFrameSize += PaddingSize;
                pVideoBufferCurrent += PaddingSize;

                ((XMVVideoFrame *)pVideoBufferPrevious)->FrameSize = ThisFrameSize - sizeof(XMVVideoFrame);
            }

            // Get the next audio frame.
            for (iStream = 0; iStream < AudioStreamCount; iStream++)
            {
                g_StreamMap.AudioMap[iStream].pfnGetNextAudioFrame(g_StreamMap.AudioMap[iStream].pFileHandle,
                                                                   g_StreamMap.AudioMap[iStream].AudioStream,
                                                                   FramesPerSecond,
                                                                   &pAudioBufferCurrent[iStream],
                                                                   pAudioBufferStart[iStream] + g_OutputBufferSize * 2);

                ThisFrameSize += (pAudioBufferCurrent[iStream] - pAudioBufferPrevious[iStream]);
            }

            // Make sure it just isn't too danged big.
            if (ThisFrameSize > g_OutputBufferSize)
            {
                fprintf(stderr, "ERROR: A frame can't be encoded in the size of a single buffer.  Increase the buffer size.\n");

                // Never returns.
                Done();
            }

            // Audio streams need to always be DWORD aligned.  This means we
            // might have to insert up to 3 bytes of padding for each stream and
            // is a bit hard to keep track of when maintaining a running total.
            // Try to calculate the size and, in the process, figure out if
            // we've overrun our buffer.
            //
            // We always need to pad the very last frame with 3 bytes to avoid
            // running off the end of the buffer in the decoder.
            //
            VideoBufferSize = pVideoBufferCurrent - pVideoBufferStart;

            for (iStream = 0; iStream < AudioStreamCount; iStream++)
            {
                AudioBufferSize += (pAudioBufferCurrent[iStream] - pAudioBufferStart[iStream] + 3) & ~3;
            }

            PotentialPacketSize = VideoBufferSize + AudioBufferSize;

            if (PotentialPacketSize > g_OutputBufferSize)
            {
                break;
            }

            if (FrameCount == 255)
            {
                fprintf(stderr, "WARNING: A buffer can only contain 255 frames and the current buffer still has room.  Consider reducing its size.\n");
                break;
            }

            CurrentPacketSize = PotentialPacketSize;

            FrameCount++;
        }

        // We're done.
        if (FrameCount == 0)
        {
            CurrentPacketSize = 0;
        }

        // The packets are always padded out to the nearest page, get that size.
        PaddedCurrentPacketSize = (CurrentPacketSize + 4095) & ~4095;

        // Fill out the rest of the video packet header.  The video stream must
        // contain any padding we need.
        //
        pVideoPacketHeader->VideoStreamLength 
            = (pVideoBufferPrevious - pVideoBufferHeaderNext) + (PaddedCurrentPacketSize - CurrentPacketSize);

        pVideoPacketHeader->FrameCount = FrameCount;

        for (iStream = 0; iStream < AudioStreamCount; iStream++)
        {
            pVideoPacketHeader->AudioStreams[iStream].AudioStreamSize
                = pAudioBufferPrevious[iStream] - pAudioBufferStart[iStream];
        }
        
        // If this isn't the first buffer then spit out the pending buffer
        // now the we know the sizes.
        //
        if (OutputBufferWriteCount != 0)
        {
            pPendingPacketHeader->NextPacketSize = PaddedCurrentPacketSize;

            // Pad and write out the video packet.
            memset(pVideoBufferPending + PendingVideoBufferSize, 0, PendingVideoPaddingSize);

            Size = PendingVideoBufferSize + PendingVideoPaddingSize;

            if (!WriteFile(g_hOutputFile, pVideoBufferPending, Size, &Written, NULL)
                || Written != Size)
            {
                fprintf(stderr, "ERROR: Unable to write to the output file.\n");

                // Never returns.
                Done();
            }

            // Write out the audio packets.
            for (iStream = 0; iStream < AudioStreamCount; iStream++)
            {
                // Pad and write the audio streams.
                Size = pPendingVideoPacketHeader->AudioStreams[iStream].AudioStreamSize;
                
                memset(pAudioBufferPending[iStream] + Size, 0, 3);

                Size = (Size + 3) & ~3;

                if (!WriteFile(g_hOutputFile, 
                               pAudioBufferPending[iStream], 
                               Size, 
                               &Written, 
                               NULL)
                    || Written != Size)
                {
                    fprintf(stderr, "ERROR: Unable to write to the output file.\n");

                    // Never returns.
                    Done();
                }
            }           
        }
        else
        {
            // Save the size of the current packet in the header.
            pVideoDataHeader->ThisPacketSize = PaddedCurrentPacketSize;
        }

        // Swap the current buffer to the pending buffer.
        pVideoBufferSwap    = pVideoBufferStart;
        pVideoBufferStart   = pVideoBufferPending;
        pVideoBufferPending = pVideoBufferSwap;

        pAudioBufferSwap    = pAudioBufferStart;
        pAudioBufferStart   = pAudioBufferPending;
        pAudioBufferPending = pAudioBufferSwap;

        PendingVideoBufferSize    = pVideoBufferPrevious - pVideoBufferPending;
        PendingVideoPaddingSize   = PaddedCurrentPacketSize - CurrentPacketSize;
        pPendingVideoPacketHeader = pVideoPacketHeader;
        pPendingPacketHeader      = pPacketHeader;

        OutputBufferWriteCount++;
    }
    while(FrameCount);
}

/*
 * Converts the input file into a XMV file.
 */

void Convert()
{
    FileContents ContentsArray[MAX_STREAMS];
    DWORD VideoStreamCount, AudioStreamCount;

    DWORD i;

    g_InputFileDesc.pFile = g_InputFileDesc.pfnOpenMedia(&g_InputFile, ContentsArray);

    // Get the file information.
    memset(ContentsArray, 0, sizeof(ContentsArray));

    g_InputFileDesc.pFile = g_InputFileDesc.pfnOpenMedia(&g_InputFile, ContentsArray);

    // Build the stream mapping table.
    VideoStreamCount = 0;
    AudioStreamCount = 0;

    for (i = 0; i < MAX_STREAMS; i++)
    {
        if (ContentsArray[i].StreamType == STREAM_VIDEO)
        {
            if (VideoStreamCount > 0)
            {
                fprintf(stderr, "ERROR: XMV files only support one video stream and more than one has been specified.\n");
                Done();
            }
        
            if (ContentsArray[i].VideoStream.CompressionID != '2VMW')
            {
                fprintf(stderr, "ERROR: XMV only supports WMV2 encoded video streams.\n");
                Done();
            }

            if (ContentsArray[i].VideoStream.Width % 16 || ContentsArray[i].VideoStream.Height % 16)
            {
                fprintf(stderr, "ERROR: The height and width of the video stream must be a multiple of 16.\n");
                Done();
            }

            g_StreamMap.pfnGetNextVideoFrame = g_InputFileDesc.pfnGetNextVideoFrame;
            g_StreamMap.pFileHandle          = g_InputFileDesc.pFile;
            g_StreamMap.VideoStream          = i;
            g_StreamMap.pVideoStreamInfo     = &ContentsArray[i].VideoStream;

            VideoStreamCount++;
        }
        else if (ContentsArray[i].StreamType == STREAM_AUDIO)
        {
            g_StreamMap.AudioMap[i].pfnGetNextAudioFrame = g_InputFileDesc.pfnGetNextAudioFrame;
            g_StreamMap.AudioMap[i].pFileHandle          = g_InputFileDesc.pFile;
            g_StreamMap.AudioMap[i].AudioStream          = i;
            g_StreamMap.AudioMap[i].pAudioStreamInfo     = &ContentsArray[i].AudioStream;
        }
    }

    // NYI: We don't do audio yet.
    ProcessFrames(VideoStreamCount, 0);
}

/*
 * Main entrypoint
 */

int __cdecl main
(
    int argc, 
    char **argv
)
{
    if (!ParseCommandLine(argc, argv))
    {
        Usage();
        return 1;
    }

    // This app only has one mode of error recovery: QUIT!  We use
    // setjmp/longjmp to avoid manually unwinding the stack.  This only
    // affects the optimizations of this routine and of "Done".
    //
    // The one side-effect is that all resource destruction must happen
    // at the end of this routine.
    //
    if (setjmp(g_Done) == 0)
    {
        OpenFiles();

        //
        // Parse the file.
        //

        switch(g_Command)
        {
        case COMMAND_INFO:
            Info();
            break;

        case COMMAND_CONVERT:
            Convert();
            break;
        }
    }
    
    //
    // Clean up.
    //

    if (g_pStreamBufferArrays)
    {
        free(g_pStreamBufferArrays);
    }

    if (g_pStreamBuffers)
    {
        VirtualFree(g_pStreamBuffers, 0, MEM_RELEASE);
    }

    if (g_hOutputFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hOutputFile);
    }

    if (g_InputFileDesc.FileType != FILEDESC_UNKNOWN && g_InputFileDesc.pFile)
    {
        g_InputFileDesc.pfnCloseMedia(g_InputFileDesc.pFile);
    }

    CloseMediaFile(&g_InputFile);

    return 0;
}

