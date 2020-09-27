/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    debugstats.cpp

Abstract:

    Debug information and statistics

--*/

#include "wmvplay.h"

#ifdef DEBUGSTATS

ULONGLONG TotalAudioData;
ULONG MaxAudioPacketSize, MinAudioPacketSize;
ULONG TotalVideoPackets, TotalAudioPackets;
ULONG LongestVideoStretch, LongestAudioStretch;
ULONG LastStretchType, LastStretchLength;
REFERENCE_TIME NextSpewTime;
MEMORYSTATUS StartMemStatus, StopMemStatus;

struct HEAPSTATUS {
    ULONG AllocedElementCount;
    ULONG AllocedElementBytes;
};
HEAPSTATUS StartHeapStatus, StopHeapStatus;

inline VOID UpdateStretch(ULONG type) {
    if (type != LastStretchType) {
        if (LastStretchType & PACKETFLAG_SOUND)
            LongestAudioStretch = max(LongestAudioStretch, LastStretchLength);
        if (LastStretchType & PACKETFLAG_VIDEO)
            LongestVideoStretch = max(LongestVideoStretch, LastStretchLength);
        LastStretchType = type;
        LastStretchLength = 0;
    }
}

//
// Obtain current process heap stats
//
VOID GetProcessHeapStatus(HEAPSTATUS* heapstats)
{
    HANDLE heap = GetProcessHeap();
    PROCESS_HEAP_ENTRY entry;

    memset(heapstats, 0, sizeof(*heapstats));
    memset(&entry, 0, sizeof(entry));

    BOOL lockok = HeapLock(heap);
    ASSERT(lockok);
    
    while (HeapWalk(heap, &entry)) {
        if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
            heapstats->AllocedElementCount++;
            heapstats->AllocedElementBytes += entry.cbData;
        }
    }

    lockok = HeapUnlock(heap);
    ASSERT(lockok);
}

//
// Start collecting debug stats
//
VOID StartDebugStats()
{
    TotalAudioData = 0;
    MaxAudioPacketSize = 0;
    MinAudioPacketSize = ULONG_MAX;
    LongestAudioStretch = LongestVideoStretch = 0;
    TotalAudioPackets = TotalVideoPackets = 0;
    LastStretchType = 0;
    LastStretchLength = 0;
    NextSpewTime = 0;

    StartMemStatus.dwLength = sizeof(StartMemStatus);
    GlobalMemoryStatus(&StartMemStatus);
    GetProcessHeapStatus(&StartHeapStatus);
}

//
// Stop collecting debug stats and dump out the results
//
VOID StopDebugStats()
{
    UpdateStretch(0);

    VERBOSE("Total # of video packets: %d\n", TotalVideoPackets);
    VERBOSE("  longest stretch: %d\n", LongestVideoStretch);
    VERBOSE("Total # of audio packets: %d\n", TotalAudioPackets);
    VERBOSE("  longest stretch: %d\n", LongestAudioStretch);
    if (TotalAudioPackets) {
        VERBOSE("  average size: %d bytes\n", (ULONG) (TotalAudioData / TotalAudioPackets));
        VERBOSE("  max size: %d bytes\n", MaxAudioPacketSize);
        VERBOSE("  min size: %d bytes\n", MinAudioPacketSize);
    }

    StopMemStatus.dwLength = sizeof(StopMemStatus);
    GlobalMemoryStatus(&StopMemStatus);

    if (StopMemStatus.dwAvailPhys != StartMemStatus.dwAvailPhys) {
        VERBOSE("Physical memory delta: 0x%x - 0x%x = 0x%x\n",
            StartMemStatus.dwAvailPhys,
            StopMemStatus.dwAvailPhys,
            StartMemStatus.dwAvailPhys - StopMemStatus.dwAvailPhys);
    }
    
    GetProcessHeapStatus(&StopHeapStatus);
    if (StartHeapStatus.AllocedElementBytes != StopHeapStatus.AllocedElementBytes) {
        VERBOSE("Heap state before: %d elements, %d bytes\n",
                StartHeapStatus.AllocedElementCount,
                StartHeapStatus.AllocedElementBytes);
        VERBOSE("Heap state after: %d elements, %d bytes\n",
                StopHeapStatus.AllocedElementCount,
                StopHeapStatus.AllocedElementBytes);
        VERBOSE("Total delta: %d elements, %d bytes\n",
                StopHeapStatus.AllocedElementCount - StartHeapStatus.AllocedElementCount,
                StopHeapStatus.AllocedElementBytes - StartHeapStatus.AllocedElementBytes);
    }
}

//
// Update debug stats
//
VOID UpdateDebugStats(Packet* pkt)
{
    UpdateStretch(pkt->flags);
    LastStretchLength++;

    if (pkt->flags & PACKETFLAG_SOUND) {
        TotalAudioPackets++;
        TotalAudioData += pkt->completedSize;
        MaxAudioPacketSize = max(MaxAudioPacketSize, pkt->completedSize);
        MinAudioPacketSize = min(MinAudioPacketSize, pkt->completedSize);
    } else {
        TotalVideoPackets++;
    }

    if (pkt->timestamp >= NextSpewTime) {
        VERBOSE("%c %d\n",
                (pkt->flags & PACKETFLAG_SOUND) ? 'a' : 'v',
                (LONG) (pkt->timestamp / 10000));
        NextSpewTime += 1000*10000;
    }
}

#endif // DEBUGSTATS

