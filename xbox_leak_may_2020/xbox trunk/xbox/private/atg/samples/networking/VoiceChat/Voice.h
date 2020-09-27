//-----------------------------------------------------------------------------
// File: Voice.h
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

#include <XBSocket.h>
#include <XBSockAddr.h>
#include <XBUtil.h> 
#include <cassert>
#include <vector>
#include "xvoice.h"
#include "Player.h"

const WORD  VOICE_PORT        = 10985;  // any port other than the first two
const BYTE  COMMUNICATOR_COUNT = 4;

//-----------------------------------------------------------------------------
// Utility functions and macros
//-----------------------------------------------------------------------------
extern "C" { ULONG DebugPrint( PCHAR Format, ... ); }

#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}

#ifdef _WRITE_LOG
    void OpenFile(const char* szFileName , HANDLE *phFile);
    void CloseFile(HANDLE *phFile);
    void WriteToLogFile(HANDLE hFile , BYTE *pbBuffer , DWORD dwSize);
#endif




//-----------------------------------------------------------------------------
// Structures & classes
//-----------------------------------------------------------------------------


    
//-----------------------------------------------------------------------------
// Name: struct MEDIA_PACKET_LIST
// Desc: Linked list of XMEDIAPACKET structures to help manage packet
//          management between XMOs
//-----------------------------------------------------------------------------
struct MEDIA_PACKET_LIST
{
    MEDIA_PACKET_LIST   *pNext;
    XMEDIAPACKET        MediaPacket;
    DWORD               dwStatus;
    DWORD               dwCompletedSize;

    MEDIA_PACKET_LIST(DWORD dwSize , DWORD dwStatusIn) : pNext(NULL) , dwStatus(dwStatusIn) , dwCompletedSize(0)
    {
        MediaPacket.dwMaxSize = dwSize;

        MediaPacket.pvBuffer = new BYTE[dwSize];
        if( NULL != MediaPacket.pvBuffer )
        {
            memset(MediaPacket.pvBuffer , 0 , dwSize);
            MediaPacket.pdwStatus = &dwStatus;
            MediaPacket.pdwCompletedSize = &dwCompletedSize;
        }
        else
        {
            MediaPacket.pdwStatus = NULL;
            MediaPacket.pdwCompletedSize = NULL;
        }
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



//-----------------------------------------------------------------------------
// Name: struct QUEUEXMO_PER_CHATTER
// Desc: Handles queueing incoming voice packets from other voice 
//          communicators.  Each listener has one queue for each remotespeaker
//          and then output is mixed from each queue
//-----------------------------------------------------------------------------
struct QUEUEXMO_PER_CHATTER
{

    // Voice queuing XMO
    XVoiceQueueMediaObject         *m_pVoiceQueueXMO;

    // The IP address for the chatter
    SOCKADDR_IN                     m_SockAddrDest;

    // If the queue ever provided output
    BOOL                            m_bProvidedOutput;

    // Pointer to next node in list
    QUEUEXMO_PER_CHATTER            *m_pNext;

    QUEUEXMO_PER_CHATTER() : m_pVoiceQueueXMO(NULL) , m_bProvidedOutput(FALSE) , m_pNext(NULL) {}
    QUEUEXMO_PER_CHATTER(const QUEUEXMO_PER_CHATTER& queue)
    {
        m_pVoiceQueueXMO = queue.m_pVoiceQueueXMO;

        m_SockAddrDest.sin_family = AF_INET;
        m_SockAddrDest.sin_addr = queue.m_SockAddrDest.sin_addr;
        m_SockAddrDest.sin_port = queue.m_SockAddrDest.sin_port;

        m_bProvidedOutput = FALSE;
        m_pNext = NULL;
    }

    ~QUEUEXMO_PER_CHATTER()
    {
        if( NULL != m_pVoiceQueueXMO )
        {
            m_pVoiceQueueXMO->Release();
            m_pVoiceQueueXMO = NULL;
        }
    }


    BOOL IsSameCommunicator(const IN_ADDR &inAddr , WORD wVoicePort)
    {
        if( (!memcmp(&m_SockAddrDest.sin_addr , &inAddr , sizeof(in_addr))) &&
                (m_SockAddrDest.sin_port == htons(wVoicePort)))
        {
            return TRUE;
        }
        return FALSE;
    }

    BOOL IsSameCommunicator(const SOCKADDR_IN &addr)
    {
        if( (!memcmp(&m_SockAddrDest.sin_addr , &addr.sin_addr , sizeof(in_addr))) &&
                (m_SockAddrDest.sin_port == addr.sin_port))
        {
            return TRUE;
        }
        return FALSE;
    }

    BOOL IsSameXbox(const IN_ADDR &inAddr)
    {
        if( (!memcmp(&m_SockAddrDest.sin_addr , &inAddr , sizeof(in_addr))))
        {
            return TRUE;
        }
        return FALSE;
    }

};




//-----------------------------------------------------------------------------
// Name: class CCommunicator
// Desc: Abstract base class - Derived from to provide Xbox Communicator
//          functionality
//-----------------------------------------------------------------------------
class CCommunicator
{
public:
    //
    // Constructor / destructor
    //
    CCommunicator(
                WORD   wHeadsetPort = 0,
                HANDLE hEvtMicInput = INVALID_HANDLE_VALUE,
                WORD   wCodecTag = WAVE_FORMAT_VOXWARE_SC06,
                WORD   wVoiceSamplingRate = 8000,
                WORD   wCodecSamplingRate = 8000,
                WORD   wVoiceBufferTime = 40,
                WORD   wVoiceBufferCount = 4
                );

    virtual ~CCommunicator();

    virtual HRESULT Inserted(); // Handle insertion
    virtual void Removed();     // Handle removal

    // Input code
    virtual HRESULT OnMicrophoneInput();
    virtual HRESULT OnCompressedInput( XMEDIAPACKET *pInput , BOOL bVoiceDetected = TRUE) = 0;

    // This routine gets called every voice loop , to give derived classes a
    // place to look for new output data (e.g. read from the network)
    virtual HRESULT CheckForOutputData() = 0;

    virtual HRESULT QueuePCMForOutput( XMEDIAPACKET *pOutput );

    // Status function
    virtual BOOL IsCommunicatorActive()
    {
        return (NULL != m_pMicrophone);
    }

protected:

    // These functions manage the queues of XMEDIAPACKETS for the XMOs
    void CreateQueue(MEDIA_PACKET_LIST **pQueue , WORD wBufferSize, WORD wCount , DWORD dwStatus);
    void DeleteQueue(MEDIA_PACKET_LIST *pQueue);
    void QueueMicrophoneBuffer(MEDIA_PACKET_LIST *pBuffer);
    void QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pBuffer);

    HRESULT CompressMediaBuffer(XMEDIAPACKET *pSrc, XMEDIAPACKET *pDst , BOOL *pvbVoiceDetected );
    
    virtual void Cleanup();

    // The following XMO objects are created on call to Inserted()
    XMediaObject             *m_pMicrophone;
    XMediaObject             *m_pHeadphone;
    XMediaObject             *m_pEncoder;

    // Port number for headset - 0 , 1 , 2 , 3
    WORD                     m_wHeadsetPort; 

    // The 2 queues for the buffers that are sent to the microphone/headphone
    MEDIA_PACKET_LIST        *m_pBeginMicrophoneQueuedBuffers;
    MEDIA_PACKET_LIST        *m_pBeginHeadphoneQueuedBuffers;

    // The Media packet used by the codec
    XMEDIAPACKET             m_CodecMediaBuffer;
    
    // Events for completion on microphone/headphone work
    HANDLE                   m_hMicrophoneWorkEvent; 
    HANDLE                   m_hHeadphoneWorkEvent; 
    BOOL                     m_bCreateMicrophoneEvent;


    // Settings for the headset and codecs
    WORD                     m_wVoiceSamplingRate;
    WORD                     m_wCodecSamplingRate;
    WORD                     m_wVoiceBufferTime;
    WORD                     m_wVoiceBufferSize;
    WORD                     m_wCodecBufferSize;
    WORD                     m_wVoiceBuffersCount;
    WORD                     m_wCodecTag;


#ifdef _WRITE_LOG
    // Diagnosis stuff
    HANDLE                   m_hFileRawPCMData;
    HANDLE                   m_hFileCompressed;
    HANDLE                   m_hFileDecompressed;
#endif

};




//-----------------------------------------------------------------------------
// Name: class CVoiceChatCommunicator
// Desc: Class definition for Communicator to be used for chat between
//          several boxes
//-----------------------------------------------------------------------------
class CVoiceChatCommunicator : public CCommunicator
{
public:
    CVoiceChatCommunicator(
                WORD   wHeadsetPort = 0,
                HANDLE hEvtMicInput = INVALID_HANDLE_VALUE,
                WORD   wCodecTag = WAVE_FORMAT_VOXWARE_SC06,
                WORD   wVoicePort = VOICE_PORT,
                WORD   wVoiceSamplingRate = 8000,
                WORD   wCodecSamplingRate = 8000,
                WORD   wVoiceBufferTime = 40,
                WORD   wVoiceBufferCount = 4
                );
    virtual ~CVoiceChatCommunicator();

    virtual HRESULT Inserted();
    virtual void Removed();

    virtual HRESULT OnCompressedInput( XMEDIAPACKET *pInput , BOOL bVoiceDetected = TRUE);
    virtual HRESULT CheckForOutputData();

    HRESULT UpdateChatList(BOOL bAdd  , const IN_ADDR &inAddr , WORD wVoicePort = 0);
    HRESULT RemoveChatters(const IN_ADDR &inAddr);

    WORD    GetVoicePort(){return m_wVoicePort;}

protected:
    void Cleanup();

    // Routines to handle sending and receiving voice packets over
    // the network
    void SendVoicePacket( XMEDIAPACKET *pMediaPacket );
    void ReadNetwork( );

    // Routines for mixing output from multiple remote chatters
    HRESULT ProcessQueues();
    void SumUpPacket( XMEDIAPACKET *pMediaPacket );


    // Queue handling functions
    void AddToQueueXMOList(const QUEUEXMO_PER_CHATTER& queue);
    QUEUEXMO_PER_CHATTER* FindInQueueXMOList( const IN_ADDR &inAddr , WORD wVoicePort);
    QUEUEXMO_PER_CHATTER* FindInQueueXMOList( const SOCKADDR_IN &addr);
    void DeleteFromQueueXMOList(QUEUEXMO_PER_CHATTER *pQueue);
    void DeleteQueueXMOList();

    // Voice queuing XMOs - one for each chatter in the game
    QUEUEXMO_PER_CHATTER     *m_VoiceQueueXMOList;
    WORD                     m_nChattersCount;


    // Media packet for getting the output from the queuing XMO
    XMEDIAPACKET             m_MixedSoundMediaBuffer;

    // MediaPacket for getting the input from the network
    XMEDIAPACKET             m_NetworkMediaBuffer;

    // Buffer for mixing the sound packets
    float                    *m_fMixingBuffer;
    WORD                     m_wMixingBufferSize;

    // The point where the mixing was resumed
    QUEUEXMO_PER_CHATTER     *m_pResumePoint;

    // Count of mixed packets
    WORD                     m_nMixedPacketsCount;


    // Network data
    SOCKET                   m_VoiceSock;
    WORD                     m_wVoicePort;

    // Interval for output
    DWORD                    m_IntervalForOutput;
    DWORD                    m_LastOutputTimeStamp;


};




//-----------------------------------------------------------------------------
// Name: class CCommunicatorMgr
// Desc: Manages the array of CCommunicators on an Xbox
//-----------------------------------------------------------------------------
class CCommunicatorMgr
{
public:
    CCommunicatorMgr();
    ~CCommunicatorMgr();

    HRESULT Process(MsgVoicePort *msgCommunicatorStatus);

    HRESULT Initialize(PlayerList& playerlist , WORD wCodecTag , MsgVoicePort *msgCommunicatorStatus);
    HRESULT UpdateChatList(BOOL bAdd  , const IN_ADDR &inAddr , WORD wVoicePort = 0);

private:
    // Check status for the voice units
    HRESULT CheckForHotPlugs(MsgVoicePort *msgCommunicatorStatus);
    HRESULT CheckInitialState();

    // The array of voice units
    CVoiceChatCommunicator* m_pVoiceUnits[COMMUNICATOR_COUNT];

    // Events for the voice units
    HANDLE                  m_hCommunicatorWorkEvent; 

    DWORD                   m_dwMicrophoneState;
    DWORD                   m_dwHeadphoneState;
    DWORD                   m_dwConnectedCommunicators;
};

#endif  // VOICE_H_INCLUDED