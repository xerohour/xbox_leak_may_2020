//-----------------------------------------------------------------------------
// File: DelayVoice.h
//
// Hist: 08.08.01 - New for Aug M1 release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef VOICE_H_INCLUDED
#define VOICE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <xtl.h>
#include <cassert>
#include <stdio.h>
#include "xvoice.h"

const int VOICE_SAMPLING_RATE = 8000;
const int XVOICE_BYTES_PER_MS = (((ULONG)( VOICE_SAMPLING_RATE / 1000.0 )) * 2 );

const int XVOICE_BUFFER_REGION_TIME = 40; 
const int XVOICE_BUFFER_REGION_SIZE = ( XVOICE_BYTES_PER_MS * XVOICE_BUFFER_REGION_TIME );
const int XVOICE_PREBUFFER_REGIONS = 20;
const int XVOICE_TOTAL_REGIONS = ( XVOICE_PREBUFFER_REGIONS + 2 );
const int XVOICE_BUFFER_SIZE = ( XVOICE_BUFFER_REGION_SIZE * XVOICE_TOTAL_REGIONS );

const int VOICE_DEVICES_COUNT = 2; //Microphone & Headphone
const int MICROPHONE = 0;
const int HEADPHONE = 1;


//////////////////////////////////////////////////////////////////////////////
// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
    ULONG
    DebugPrint(
        PCHAR Format,
        ...
        );
}

#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}
//////////////////////////////////////////////////////////////////////////////

struct MEDIA_PACKET_LIST
{
    MEDIA_PACKET_LIST   *pNext;
    XMEDIAPACKET        MediaPacket;
    DWORD               dwStatus;
    DWORD               dwCompletedSize;

    MEDIA_PACKET_LIST(DWORD dwStatusIn) : pNext(NULL) , dwStatus(dwStatusIn) , dwCompletedSize(0)
    {
        MediaPacket.dwMaxSize = XVOICE_BUFFER_REGION_SIZE;
        MediaPacket.pvBuffer = new BYTE[XVOICE_BUFFER_REGION_SIZE];
        memset(MediaPacket.pvBuffer , 0 , XVOICE_BUFFER_REGION_SIZE);
        MediaPacket.pdwCompletedSize = NULL;
        MediaPacket.pdwStatus = NULL;
        MediaPacket.prtTimestamp = NULL;
        MediaPacket.hCompletionEvent = NULL;
    }
    ~MEDIA_PACKET_LIST()
    {
        if( NULL !=  MediaPacket.pvBuffer)
        {
            delete [] MediaPacket.pvBuffer;
        }
    }
};

//
// CVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////
class CDelayVoiceUnit
{
public:
    virtual HRESULT PlayToHeadphone();
    virtual HRESULT OnMicrophoneReadComplete();
    
    virtual void SetSockAddr(const IN_ADDR &addrPlayer);

    virtual HRESULT Inserted();
    virtual void Removed();
    virtual void Process();
    virtual void Init(DWORD dwPortNumber);

    virtual void SendPacketToRenderer();
    virtual BOOL IsPacketPreparedForRender();

  CDelayVoiceUnit();
  virtual ~CDelayVoiceUnit();

  DWORD GetPortNumber() { return m_dwPortNumber; }


protected:
    void CreateQueue(MEDIA_PACKET_LIST **pQueue , WORD wSize , DWORD dwStatus);
    void DeleteQueue(MEDIA_PACKET_LIST *pQueue);
    void QueueMicrophoneBuffer(MEDIA_PACKET_LIST *pBuffer);
    void QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pBuffer);

    //
    // XMO objects created on call to Inserted()
    //
    XMediaObject *m_pMicrophone;
    XMediaObject *m_pHeadphone;

    //
    // Stuff given to us at init
    //
    DWORD m_dwPortNumber; // Port number for headset 


    //
    // The 2 queues for the buffers that are sent to the microphone/headphone
    //
    MEDIA_PACKET_LIST *m_pBeginMicrophoneQueuedBuffers;
    MEDIA_PACKET_LIST *m_pBeginHeadphoneQueuedBuffers;


    //
    // Events for completion on microphone/headphone work
    //
    HANDLE m_hMicrophoneWorkEvent; 
    HANDLE m_hHeadphoneWorkEvent; 

    virtual void Cleanup();

    //
    //  Status
    //
    BOOL   m_fPreBuffering;
    DWORD  m_dwPreBufferRegions;

};

#endif  // VOICE_H_INCLUDED
