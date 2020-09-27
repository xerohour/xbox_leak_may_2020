/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    wmvplay.cpp

Abstract:

    WMV playback sample application - main (decoder) thread

--*/

#include "wmvplay.h"
#include <stdio.h>

WMVVIDEOINFO VideoInfo;
TextureBufferQueue* TextureQ;
WAVEFORMATEX AudioInfo;
SoundPacketQueue* SoundQ;
LPWMVDECODER wmvdecoder;
REFERENCE_TIME PlayDuration, Preroll;
REFERENCE_TIME StartPresentationTime, CurrentPlayTime;
REFERENCE_TIME StartingSeekPosition;
BOOL FullScreenMode = TRUE;
INT DecodingFlag = DECODING_IDLE;
BOOL ContinuousMode = FALSE;
BOOL TestDVDFiles = FALSE;
INT TestFileIndex = 0;
CHAR CurrentFilename[MAX_PATH];
DWORD NumFilesPlayed;

//
// Process a decoded audio or video packet
//
VOID ProcessDecodedPacket(PacketQueue* pktq, Packet* pkt)
{
    if (pkt->completedSize) {
        UpdateDebugStats(pkt);

        if (pkt->timestamp < PlayDuration) {
            if (pkt->flags & PACKETFLAG_SOUND) {
                // NOTE: We ignore the sound timestamp here
                // and just assume the stream is continous.
                pkt->timestamp = 0;
            } else {
                pkt->timestamp = pkt->timestamp - StartingSeekPosition + StartPresentationTime;
            }

            pktq->ScheduleReadyPacket(pkt);
            return;
        }

        VERBOSE("Bogus timestamp: %c %d\n",
            (pkt->flags & PACKETFLAG_SOUND) ? 'a' : 'v',
            (LONG) (pkt->timestamp / 10000));
    }

    pktq->ReleaseFreePacket(pkt);
}

VOID SeekToStartingPosition()
{
    HRESULT hr;

    #if 0

    //
    // Seek directly by calling the decoder
    //
    REFERENCE_TIME seekpos = StartingSeekPosition;
    VERBOSE("Seeking to position: %d\n", (LONG) (seekpos / 10000));
    hr = wmvdecoder->Seek(&seekpos);
    VERBOSE("Actual position: %d\n", (LONG) (seekpos / 10000));

    #else

    //
    // Seek manually by decoding and skipping past unwanted packets
    //
    REFERENCE_TIME videotime, audiotime;
    DWORD videostatus, audiostatus;
    DWORD videosize, audiosize;
    XMEDIAPACKET videopkt, audiopkt;
    REFERENCE_TIME spewtime = 0;
    REFERENCE_TIME lasttime = 0;

    VERBOSE("Skipping");

    videopkt.dwMaxSize = VideoInfo.dwWidth * VideoInfo.dwOutputBitsPerPixel / 8;
    videopkt.dwMaxSize = (videopkt.dwMaxSize + 63) & ~63;
    videopkt.dwMaxSize = videopkt.dwMaxSize * VideoInfo.dwHeight;
    videopkt.pvBuffer = malloc(videopkt.dwMaxSize);
    videopkt.pdwCompletedSize = &videosize;
    videopkt.pdwStatus = &videostatus;
    videopkt.pContext = NULL;
    videopkt.prtTimestamp = &videotime;

    audiopkt.dwMaxSize = 512 * AudioInfo.nBlockAlign;
    audiopkt.pvBuffer = malloc(audiopkt.dwMaxSize);
    audiopkt.pdwCompletedSize = &audiosize;
    audiopkt.pdwStatus = &audiostatus;
    audiopkt.pContext = NULL;
    audiopkt.prtTimestamp = &audiotime;

    do {
        hr = wmvdecoder->ProcessMultiple(&videopkt, &audiopkt);
        ASSERT(hr == S_OK);

        if (videosize > 0) lasttime = videotime;
        if (audiosize > 0) lasttime = audiotime;

        if (lasttime >= spewtime) {
            VERBOSE(".");
            spewtime += 10000000;
        }

    } while (lasttime < StartingSeekPosition);
    StartingSeekPosition = lasttime;

    VERBOSE("\n");

    free(videopkt.pvBuffer);
    free(audiopkt.pvBuffer);

    #endif
}

//
// Play a WMV file
//
VOID PlayWMVFile()
{
    if (TestDVDFiles) {
        sprintf(CurrentFilename, "A:\\test%02d.wmv", TestFileIndex);
    } else {
        sprintf(CurrentFilename, "D:\\movies\\test%02d.wmv", TestFileIndex);
    }

    NumFilesPlayed++;
    VERBOSE("(%d) Opening movie file: %s\n", NumFilesPlayed, CurrentFilename);

    wmvdecoder = NULL;
    HRESULT hr = WmvCreateDecoder(
                    CurrentFilename,
                    NULL,
                    WMVVIDEOFORMAT_YUY2,
                    NULL,
                    &wmvdecoder);
    if (FAILED(hr)) goto exit;

    hr = wmvdecoder->GetVideoInfo(&VideoInfo);
    if (hr == WMVERR_NO_VIDEO_STREAM) {
        // Audio only file - fake the video info
        memset(&VideoInfo, 0, sizeof(VideoInfo));
        VideoInfo.dwWidth = 160;
        VideoInfo.dwHeight = 120;
        VideoInfo.dwOutputBitsPerPixel = 16;
    } else if (FAILED(hr))
        goto exit;

    hr = wmvdecoder->GetAudioInfo(&AudioInfo);
    if (hr == WMVERR_NO_AUDIO_STREAM) {
        // Video only file - fake the audio info
        AudioInfo.wFormatTag = WAVE_FORMAT_PCM;
        AudioInfo.nChannels = 1;
        AudioInfo.nSamplesPerSec = 44100;
        AudioInfo.wBitsPerSample = 16;
        AudioInfo.nBlockAlign = 2;
        AudioInfo.nAvgBytesPerSec = 2*44100;
        AudioInfo.cbSize = 0;
    } else if (FAILED(hr))
        goto exit;

    hr = wmvdecoder->GetPlayDuration(&PlayDuration, &Preroll);
    if (FAILED(hr)) goto exit;

    VERBOSE(
        "Movie info:\n"
        "  video %dx%dx%d %dbps\n"
        "  sound %dx%d-bit %dHz\n"
        "  play duration %d msecs\n"
        "  preroll %d msecs\n",
        VideoInfo.dwWidth,
        VideoInfo.dwHeight,
        VideoInfo.dwOutputBitsPerPixel,
        VideoInfo.dwBitsPerSecond,
        AudioInfo.nChannels,
        AudioInfo.wBitsPerSample,
        AudioInfo.nSamplesPerSec,
        (LONG) (PlayDuration / 10000),
        (LONG) (Preroll / 10000));

    if (StartingSeekPosition != 0) {
        SeekToStartingPosition();
    }

    TextureQ = TextureBufferQueue::Create();
    SoundQ = SoundPacketQueue::Create();
    ASSERT(TextureQ && SoundQ);

    DWORD starttick = GetTickCount();

    DecodingFlag = DECODING_INPROGRESS;
    SoundQ->GetTime(&StartPresentationTime);

    do {
        //
        // Update the current play time
        //
        SoundQ->GetTime(&CurrentPlayTime);

        //
        // Handle controller input
        //
        PollInput();

        //
        // Keep decoding if there are both free sound and video packets available
        // NOTE: We don't decode more than 3 video frames per iteration.
        //
        INT videocnt = 0;

        while (DecodingInProgress() &&
               videocnt < 3 &&
               !SoundQ->freeList.IsEmpty() &&
               !TextureQ->freeList.IsEmpty()) {
            Packet* audiopkt = SoundQ->GetFreeListHead();
            Packet* videopkt = TextureQ->GetFreeListHead();

            hr = wmvdecoder->ProcessMultiple(&videopkt->xmp, &audiopkt->xmp);
            if (FAILED(hr)) {
                AbortDecoding();
                break;
            }

            // Decoding has finished:
            //  we'll now wait until the sound playback is stopped
            if (hr == S_FALSE) {
                VERBOSE("Decoding completed\n");
                DecodingFlag = DECODING_COMPLETED;
            }

            ProcessDecodedPacket(SoundQ, audiopkt);

            if (videopkt->completedSize) videocnt++;
            ProcessDecodedPacket(TextureQ, videopkt);
        }

        //
        // Handle sound playback
        //
        SoundQ->Process();

        //
        // Handle video playback
        //
        TextureQ->Process();
    } while (DecodingInProgress() || !SoundQ->busyList.IsEmpty());

    VERBOSE("Decode / play time: %d msecs\n", GetTickCount() - starttick);
    SoundQ->Shutdown();
    TextureQ->Shutdown();

    delete SoundQ;
    SoundQ = NULL;

    delete TextureQ;
    TextureQ = NULL;

exit:
    if (FAILED(hr)) {
        VERBOSE("Decoding failed: %08x\n", hr);
        DecodingFlag = DECODING_COMPLETED;
    }

    if (wmvdecoder) {
        wmvdecoder->Release();
    }

    //
    // If the decoding completes normally, decide what to do next:
    //  either pause or move on to the next file
    //
    if (DecodingFlag == DECODING_COMPLETED) {
        if (ContinuousMode) {
            if (FAILED(hr) && GetFileAttributes(CurrentFilename) == 0xffffffff)
                TestFileIndex = 0;
            else
                TestFileIndex++;
            DecodingFlag = DECODING_IDLE;
        } else {
            DecodingFlag = DECODING_PAUSED;
        }
    }
}

//
// Main program entrypoint - decode thread
//
void __cdecl main()
{
    VERBOSE("Test started\n");

    XInitDevices(0, NULL);
    XSetFileCacheSize(1024*1024);

    TextureBufferQueue::InitializeD3D();
    SoundPacketQueue::InitializeDSound();

    while (DecodingFlag != DECODING_EXIT) {
        if (DecodingFlag != DECODING_PAUSED) {
            //
            // Dynamically load/unload WMVDEC sections
            //
            VOID* wmvdec_section = XLoadSection("WMVDEC");
            ASSERT(wmvdec_section);

            StartDebugStats();
            PlayWMVFile();
            StopDebugStats();

            BOOL freeok = XFreeSection("WMVDEC");
            ASSERT(freeok);
        }
        PollInput();
    }

    TextureBufferQueue::CleanupD3D();
    SoundPacketQueue::CleanupDSound();

    VERBOSE("Test completed\n");
    Sleep(INFINITE);
}

