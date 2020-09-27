/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    wmvplay.cpp

Abstract:

    WMV playback sample application - main (decoder) thread

--*/

#include "..\XDemos.h"
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

//
// Play a WMV file
//
bool PlayWMVFile(char *szFileName)
{
	bool bReturn = false;

    wmvdecoder = NULL;
    HRESULT hr = WmvCreateDecoder(
                    szFileName,
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
    if (FAILED(hr)) goto exit;

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

    /*if (StartingSeekPosition != 0) {
        REFERENCE_TIME seekpos = StartingSeekPosition;
        VERBOSE("Seeking to position: %d\n", (LONG) (seekpos / 10000));
        hr = wmvdecoder->Seek(&seekpos);
        VERBOSE("Actual position: %d\n", (LONG) (seekpos / 10000));
    }*/

    TextureQ = TextureBufferQueue::Create();
    SoundQ = SoundPacketQueue::Create();

	if (!TextureQ || !SoundQ)
	{
		goto exit;
	}
	else
	{
		bReturn = true;
	}

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
		if (CheckForInput())
		{
			bReturn = true;
			g_dwNextButton = (DWORD)-1;
			break;
		}

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
				bReturn = false;
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

	if (SoundQ)
	{
		SoundQ->Shutdown();
		delete SoundQ;
		SoundQ = NULL;
	}

	if (TextureQ)
	{
		TextureQ->Shutdown();
		delete TextureQ;
		TextureQ = NULL;
	}

exit:
    if (FAILED(hr)) {
        VERBOSE("Decoding failed: %08x\n", hr);
        DecodingFlag = DECODING_COMPLETED;
    }

    if (wmvdecoder) {
        wmvdecoder->Release();
    }

	return bReturn;
}
