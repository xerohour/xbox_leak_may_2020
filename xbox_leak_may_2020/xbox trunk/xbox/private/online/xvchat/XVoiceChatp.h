//-----------------------------------------------------------------------------
// File: XVoiceChatp.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XVCHATP_H_INCLUDED
#define XVCHATP_H_INCLUDED

#include "queuexmop.h"

const BYTE   COMMUNICATOR_COUNT       = 4;
const BYTE   PCM_BUFFERS_COUNT        = 2;
const BYTE   MAX_PLAYERS              = 16;
const WORD   MAX_PCM_BUFFER           = 100;
const WORD   MIN_PCM_BUFFER           = 20;
const DWORD  MAX_BUFFERING_TIME       = 1000;

const WORD   CODEC_TAG                = WAVE_FORMAT_VOXWARE_SC03;
const WORD   VOICE_SAMPLING_RATE      = 8000;
const WORD   CODEC_SAMPLING_RATE      = 8000;
const WORD   PCM_TIME                 = 20;
const WORD   MAX_CHANNELS             = 2;
const WORD   MAX_ENCODERS             = 2;
const WORD   MAX_DECODERS_PER_CHANNEL = 2;    
const WORD   SEND_BUFFERING_TIME      = 100;
const WORD   RECV_BUFFERING_TIME      = 200;

const DWORD  CODEC_BUFFER_SIZE        = 20; // change this! - codec header should include player no

const BYTE   OUTSIDE_PLAYER           = 0xFF;

//----------------------------------------------------------------------------
//
// Classes
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// Class CVChatParameters - configuration class for voice chat
//
//----------------------------------------------------------------------------
class CVChatParameters
{

public:

	CVChatParameters() : m_wPCMBufferTime(PCM_TIME),
						 m_wCodecTag(WAVE_FORMAT_VOXWARE_SC03),
		                 m_wVoiceSamplingRate(VOICE_SAMPLING_RATE)
	{
		memset(&m_publicVChatConfig , 0 , sizeof(VOICE_CHAT_CONFIG));
	}

	VOID Initialize(LPVOICE_CHAT_CONFIG pVoiceChatConfig)
	{
		assert(NULL != pVoiceChatConfig);

		assert(pVoiceChatConfig->wMaxEncoders <= COMMUNICATOR_COUNT);
		assert(pVoiceChatConfig->wMaxChannels <= COMMUNICATOR_COUNT);
		assert(pVoiceChatConfig->wMaxDecodersPerChannel < MAX_PLAYERS);
		assert(pVoiceChatConfig->wSendBufferingTime <= MAX_BUFFERING_TIME);
		assert(pVoiceChatConfig->wRecvBufferingTime <= MAX_BUFFERING_TIME);

        m_publicVChatConfig.wMaxEncoders = (0 == pVoiceChatConfig->wMaxEncoders)?MAX_ENCODERS:(pVoiceChatConfig->wMaxEncoders);
		m_publicVChatConfig.wMaxChannels = (0 == pVoiceChatConfig->wMaxChannels)?MAX_CHANNELS:(pVoiceChatConfig->wMaxChannels);
		m_publicVChatConfig.wMaxDecodersPerChannel = (0 == pVoiceChatConfig->wMaxDecodersPerChannel)?MAX_DECODERS_PER_CHANNEL:(pVoiceChatConfig->wMaxDecodersPerChannel);
		m_publicVChatConfig.wSendBufferingTime = (0 == pVoiceChatConfig->wSendBufferingTime)?SEND_BUFFERING_TIME:(pVoiceChatConfig->wSendBufferingTime);
		m_publicVChatConfig.wRecvBufferingTime = (0 == pVoiceChatConfig->wRecvBufferingTime)?RECV_BUFFERING_TIME:(pVoiceChatConfig->wRecvBufferingTime);

	}

	WORD GetVoiceSamplingRate()
	{
		return (m_wVoiceSamplingRate);
	}

	WORD GetCodecTag()
	{
		return (m_wCodecTag);
	}

	WORD GetPCMBufferTime()
	{
		return (m_wPCMBufferTime);
	}

	WORD GetMaxEncoders()
	{
		return (m_publicVChatConfig.wMaxEncoders);
	}

	WORD GetMaxChannels()
	{
		return (m_publicVChatConfig.wMaxChannels);
	}

	WORD GetMaxDecodersPerChannel()
	{
        return (m_publicVChatConfig.wMaxDecodersPerChannel);
	}

	WORD GetSendBufferingTime()
	{
		return (m_publicVChatConfig.wSendBufferingTime);
	}

	WORD GetRecvBufferingTime()
	{
		return (m_publicVChatConfig.wRecvBufferingTime);
	}

private:
	//
    // Configurable part by the client through the API
    //
    VOICE_CHAT_CONFIG     m_publicVChatConfig;

	//
    // Non-configurable part by the client
    //
    WORD                  m_wPCMBufferTime;
	WORD                  m_wCodecTag;
	WORD                  m_wVoiceSamplingRate;

};

typedef CVChatParameters *LPCVChatParameters;


//----------------------------------------------------------------------------
//
// Class CHeadsetXMediaPacket - XMediaPacket wrapper
//
//----------------------------------------------------------------------------
class CHeadsetXMediaPacket

{

public:
	CHeadsetXMediaPacket() : m_dwStatus(0),
                             m_dwCompletedSize(0)
	{
		m_XMediaPacket.pvBuffer = NULL;
		m_XMediaPacket.pdwStatus = &m_dwStatus;
		m_XMediaPacket.pdwCompletedSize = &m_dwCompletedSize;
		m_XMediaPacket.prtTimestamp = NULL;
		m_XMediaPacket.hCompletionEvent = NULL;
	}

	VOID Initialize(PBYTE pbBuffer , DWORD dwSize , DWORD dwStatus, HANDLE hWorkEvent); 

	LPXMEDIAPACKET GetXMP()
	{
		return (&m_XMediaPacket);
	}

	VOID Dump();

private:

	XMEDIAPACKET        m_XMediaPacket;
    DWORD               m_dwStatus;
    DWORD               m_dwCompletedSize;


};

typedef CHeadsetXMediaPacket *LPCHeadsetXMediaPacket;

//----------------------------------------------------------------------------
//
// Class CHeadsetXMediaQueue - the XMP queues for microphone/headphone
//
//----------------------------------------------------------------------------
class CHeadsetXMediaPacketQueue
{

public:

	CHeadsetXMediaPacketQueue() : m_dwXMPCount(0),
		                          m_dwSize(0),
		                          m_pXMPQueue(NULL),
		                          m_pbBufferSpace(NULL),
								  m_dwCurrentIndex(0)
	{
	}

    ~CHeadsetXMediaPacketQueue()
	{
		Cleanup();
	}

	HRESULT CreateHeadsetQueue(DWORD dwHMPCount , DWORD dwSize, DWORD dwStatus, HANDLE hWorkEvent);

	LPXMEDIAPACKET GetCurrentXMP();

	VOID Cleanup();

	VOID Dump();

	VOID Verify();


private:

	DWORD                    m_dwXMPCount;
	DWORD                    m_dwSize;

	LPCHeadsetXMediaPacket   m_pXMPQueue;
	PBYTE                    m_pbBufferSpace;

	DWORD                    m_dwCurrentIndex;

};

typedef CHeadsetXMediaPacketQueue *LPCHeadsetXMediaPacketQueue;

//----------------------------------------------------------------------------
//
// Class CComunicator - implements the functionalities associated with 
//                      an Xbox Communicator
//
//----------------------------------------------------------------------------
class CCommunicator
{
public:
    //
    // Constructor / destructor
    //
    CCommunicator();
    ~CCommunicator();

    //
    // Insert/Remove
    //
    HRESULT Inserted(
		        LPCVChatParameters pVChatParams,
		        BYTE  bHeadsetPort,
		        HANDLE hEvtMicInput
				);

    VOID Removed();

	//
    // Status function
    //
	BOOL IsCommunicatorActive()
	{
		return (m_bCommActive);
	}

	BYTE GetPlayerNo()
	{
		return (m_bPlayerNo);
	}

	LPXMEDIAPACKET GetMicrophoneOutput();


    HRESULT EncodeXMP(LPXMEDIAPACKET pSrcXMP, LPXMEDIAPACKET pDstXMP , BOOL *pvbVoiceDetected );
	VOID EnqueueMicrophoneBuffer(LPXMEDIAPACKET pMicrophoneXMP);
	VOID EnqueueHeadphoneBuffer(LPXMEDIAPACKET pHeadphoneXMP);

private:    

    VOID Cleanup();

private:

    //
    // XMO objects created on call to Inserted()
    //
    XMediaObject                *m_pMicrophone;
    XMediaObject                *m_pHeadphone;
    XMediaObject                *m_pEncoder;

    //
    // Port number for headset - 0 , 1 , 2 , 3
    //
    BYTE                        m_bHeadsetPort; 
	BYTE                        m_bChannel;
	BYTE                        m_bPlayerNo;

	//
    // Status in chat
    //
	BOOL                        m_bCommActive;

    //
    // The 2 queues for the buffers that are sent to the microphone/headphone
    //
    CHeadsetXMediaPacketQueue   m_MicrophoneQueue;
    CHeadsetXMediaPacketQueue   m_HeadphoneQueue;

};

//----------------------------------------------------------------------------
//
// Class CCommunicatorMgr ; manages the array of CCommunicators on an XBOX
//
//----------------------------------------------------------------------------
class CCommunicatorMgr
{
public:
    CCommunicatorMgr();
    ~CCommunicatorMgr();

    
    HRESULT Initialize(LPVOICE_CHAT_CONFIG pVoiceChatConfig);

private:
    //
    // Check status for the voice units
    //
    HRESULT CheckForHotPlugs();
    HRESULT CheckInitialState();

	//
    // Encoding & decoding processing
    //
	HRESULT ProcessMicrophones();
	

private:
	//
    // Configuration structure
    //
	CVChatParameters    m_VoiceParams;

    //
    // The array of voice units
    //
    CCommunicator       m_CommArray[COMMUNICATOR_COUNT];

    //
    // Event for microphone
    //
    HANDLE              m_hMicrophoneWorkEvent;

	//
    // Event for network packet ready
    //
	HANDLE              m_hNetworkPacketReadyEvent;

	//
    // Network packet
    //
	PBYTE               m_pbNetworkPacket;
	DWORD               m_dwNetworkPacketSize;
	DWORD               m_dwNetworkPacketIndex;

	//
    // Priority list for encoding
    //
	BYTE                m_bEncodePriorityArray[COMMUNICATOR_COUNT];

	//
    // Working buffer for codec
    //
	XMEDIAPACKET        m_CodecMediaBuffer;

};

typedef CCommunicatorMgr *XONVCHAT_MGR_HANDLE;

#endif  // XVCHATP_H_INCLUDED
