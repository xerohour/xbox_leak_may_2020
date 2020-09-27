//=============================================================================
// File: wmvplay.h
//
// Desc: WMV playback - header file
// Created: 09/12/2001
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================

#pragma once

#include <xtl.h>
#include <wmvxmo.h>

extern IDirectSound8* g_pDSnd;

//
// Debug macros
//
#if _DEBUG
extern "C" ULONG __cdecl DbgPrint(CHAR*, ...);
#define VERBOSE DbgPrint
#define ASSERT(cond) do { \
            if (!(cond)) { \
                DbgPrint("Error on line %d: %s\n", __LINE__, #cond); \
                DebugBreak(); \
            } \
        } while (0)
#else
#define VERBOSE 1 ? (void)0 : (void)
#define ASSERT(cond)
#endif

//
// Maximum number of active sound packets
//  and the max size of each packets (in samples)
//
#define MAX_SOUND_PACKETS 256
#define MAX_SAMPLES_PER_SOUND_PACKET 512

//
// Maximum number of textures (must be >= 2)
//
#define MAX_TEXTURES            12
#define SCREENWIDTH             640
#define SCREENHEIGHT            480

//
// Packet queue structures
//
#define PACKETFLAG_SOUND        0x01
#define PACKETFLAG_VIDEO        0x02

struct Packet {
    Packet* next;
    DWORD flags;
    DWORD completedSize;
    DWORD status;
    REFERENCE_TIME timestamp;
    XMEDIAPACKET xmp;
};

struct PacketList {
    Packet* head;
    Packet* tail;

    PacketList() { head = tail = NULL; }

    BOOL IsEmpty() { return head == NULL; }

    Packet* RemoveHead() {
        Packet* pkt = head;

        if ((head = pkt->next) == NULL)
            tail = NULL;
        else
            pkt->next = NULL;

        return pkt;
    }
    
    VOID AddTail(Packet* pkt) {
        if (tail)
            tail->next = pkt;
        else
            head = pkt;

        tail = pkt;
        pkt->next = NULL;
    }
};

class PacketQueue {
public:
    //
    // Return the first packet in the free packet list to the decoder
    //
    Packet* GetFreeListHead() {
        Packet* pkt = freeList.RemoveHead();

        pkt->completedSize = 0;
        pkt->xmp.dwMaxSize = maxPacketSize;
        return pkt;
    }

    //
    // Return the packet to the free packet list
    //
    VOID ReleaseFreePacket(Packet* pkt) {
        freeList.AddTail(pkt);
    }

    //
    // This is called by the decoder when it has a packet ready
    // for the render threads.
    //
    VOID ScheduleReadyPacket(Packet* pkt) {
        pkt->xmp.dwMaxSize = pkt->completedSize;
        readyList.AddTail(pkt);
    }

    PacketList freeList;
    PacketList readyList;
    PacketList busyList;

protected:
    PacketQueue() {
        maxCount = 0;
        maxPacketSize = 0;
    }

    BOOL Initialize() {
        Packet* pkt = freeList.head;
        do {
            pkt->xmp.pdwCompletedSize = &pkt->completedSize;
            pkt->xmp.pdwStatus = &pkt->status;
            pkt->xmp.prtTimestamp = &pkt->timestamp;
            pkt = pkt->next;
        } while (pkt);

        return TRUE;
    }

    LONG maxCount;
    DWORD maxPacketSize;
};

//
// Sound packet queue
//
class SoundPacketQueue : public PacketQueue {
public:
    static SoundPacketQueue* Create() {
        SoundPacketQueue* soundq = new SoundPacketQueue();
        if (soundq && soundq->Initialize())
            return soundq;

        delete soundq;
        return NULL;
    }

    ~SoundPacketQueue() {
        if (packetArray) {
            free(packetArray);
        }

        if (soundStream) {
            soundStream->Release();
        }
    }

    VOID GetTime(REFERENCE_TIME* time) {
        HRESULT hr = g_pDSnd->GetTime(time);
        ASSERT(SUCCEEDED(hr));
    }

    VOID Process();
    VOID Shutdown() {
         while (!busyList.IsEmpty()) {
            RetireBusyList();
        }
    }

private:
    SoundPacketQueue() {
        soundStream = NULL;
        packetArray = NULL;
    }

    BOOL Initialize();
    BOOL RetireBusyList();

    LPDIRECTSOUNDSTREAM soundStream;
    VOID* packetArray;
};

//
// Texture buffer queue
//
struct TexturePacket : public Packet {
    IDirect3DTexture8* d3dtex;
};

class TextureBufferQueue : public PacketQueue {
public:
    static VOID InitializeD3D();

    static TextureBufferQueue* Create() {
        TextureBufferQueue* textureq = new TextureBufferQueue();
        if (textureq && textureq->Initialize())
            return textureq;
        delete textureq;
        return NULL;
    }

    ~TextureBufferQueue() {
        for (INT i=0; i < MAX_TEXTURES; i++) {
            if (textures[i].d3dtex) {
                textures[i].d3dtex->Release();
            }
        }
    }

    VOID Process();
    VOID Shutdown();

private:
    TextureBufferQueue() {
        memset(textures, 0, sizeof(textures));
    }

    BOOL Initialize();
    IDirect3DVertexBuffer8* CreateVertexBuffer(BOOL fullscreen);
    VOID RenderFrame(TexturePacket* pkt);
    
    TexturePacket textures[MAX_TEXTURES];
    LONG discardedFrames, overlayBlocks;

    static LONG refreshInterval;
};

extern WAVEFORMATEX AudioInfo;
extern SoundPacketQueue* SoundQ;
extern WMVVIDEOINFO VideoInfo;
extern TextureBufferQueue* TextureQ;
extern REFERENCE_TIME StartPresentationTime, CurrentPlayTime;
extern BOOL FullScreenMode;
extern INT DecodingFlag;
extern BOOL ContinuousMode;
extern BOOL TestDVDFiles;
extern INT TestFileIndex;

//
// Poll controllers for input
//
VOID PollInput();

//
// Decoding flags
//
enum {
    DECODING_IDLE,
    DECODING_PAUSED,
    DECODING_ABORTED,
    DECODING_INPROGRESS,
    DECODING_COMPLETED,
    DECODING_EXIT,
};

inline VOID AbortDecoding() { DecodingFlag = DECODING_ABORTED; }
inline BOOL DecodingInProgress() { return DecodingFlag == DECODING_INPROGRESS; }

//
// Debug information and statistics
//
#if DBG && defined(DONT_PLAY_SOUND) && defined(DONT_PLAY_VIDEO)
#define DEBUGSTATS
#endif

#ifdef DEBUGSTATS
VOID StartDebugStats();
VOID StopDebugStats();
VOID UpdateDebugStats(Packet* pkt);
#else
#define StartDebugStats()
#define StopDebugStats()
#define UpdateDebugStats(pkt)
#endif

bool PlayWMVFile(char *szFileName);
