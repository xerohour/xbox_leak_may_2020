/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    sound.cpp

Abstract:

    WMV playback sample application - sound related functions

--*/

#include "..\XDemos.h"

BOOL SoundPacketQueue::Initialize()
{
    maxCount = MAX_SOUND_PACKETS;
    maxPacketSize = MAX_SAMPLES_PER_SOUND_PACKET * AudioInfo.nBlockAlign;

    DWORD pktsize = sizeof(Packet) + maxPacketSize;
    BYTE* buf = (BYTE*) malloc(pktsize * MAX_SOUND_PACKETS);
    if (!buf) return FALSE;

    packetArray = buf;
    for (LONG i=0; i < maxCount; i++) {
        Packet* pkt = (Packet*) (buf + i * pktsize);
        pkt->flags = PACKETFLAG_SOUND;
        pkt->xmp.pvBuffer = pkt + 1;
        freeList.AddTail(pkt);
    }

    if (!PacketQueue::Initialize())
        return FALSE;

    DSSTREAMDESC ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwMaxAttachedPackets = MAX_SOUND_PACKETS;
    ddsd.lpwfxFormat = &AudioInfo;

    HRESULT hr = DirectSoundCreateStream(&ddsd, &soundStream);
    return SUCCEEDED(hr);
}

//
// Check if any of the actively playing sound packets are finished
//
BOOL SoundPacketQueue::RetireBusyList()
{
    #ifndef DONT_PLAY_SOUND
    DirectSoundDoWork();
    #endif

    Packet* pkt;
    while (pkt = busyList.head) {
        if (pkt->status == XMEDIAPACKET_STATUS_PENDING)
            return TRUE;

        busyList.RemoveHead();
        ReleaseFreePacket(pkt);
    }

    return FALSE;
}

//
// Process the sound packets
//
VOID SoundPacketQueue::Process()
{
    //
    // Check if any sound packets are ready to be scheduled
    //
    while (!readyList.IsEmpty()) {
        Packet* pkt = readyList.RemoveHead();

        #ifndef DONT_PLAY_SOUND
        soundStream->Process(&pkt->xmp, NULL);
        #else
        pkt->status = XMEDIAPACKET_STATUS_SUCCESS;
        #endif

        busyList.AddTail(pkt);
    }

    RetireBusyList();
}

