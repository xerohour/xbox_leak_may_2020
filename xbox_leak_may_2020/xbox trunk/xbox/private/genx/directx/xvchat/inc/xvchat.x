/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xvchat.h
 *  Content:    Voice chat include file
//@@BEGIN_MSINTERNAL
 *  History:
 *  Date        By          Reason
 *  =====       =======     ==================================================
 //@@END_MSINTERNAL
 ***************************************************************************/

#ifndef __XVOICECHAT__
#define __XVOICECHAT__

#ifdef __cplusplus
extern "C" {
#endif

const HRESULT QUEUE_S_OK             = S_OK;
const HRESULT QUEUE_E_FAIL           = E_FAIL;
const HRESULT QUEUE_E_OUTOFMEMORY    = E_OUTOFMEMORY;
const HRESULT QUEUE_E_NOINPUTYET     = 0x8000000A;
const HRESULT QUEUE_E_HWMNOTREACHED  = 0x8000000B;
const HRESULT QUEUE_E_FULL           = 0x8000000C;

XBOXAPI
HRESULT
WINAPI 
XVoiceCreateMediaObject(
       IN PXPP_DEVICE_TYPE XppDeviceType, 
	   IN DWORD dwPort, 
	   IN DWORD dwMaxAttachedPackets,
	   IN LPWAVEFORMATEX pwfxFormat, 
	   OUT LPXMEDIAOBJECT *ppMediaObject
	   );

XBOXAPI
HRESULT
WINAPI 
XVoiceEncoderCreateMediaObject(
       IN BOOL fAutoMode, 
	   IN DWORD dwCodecTag, 
	   IN LPWAVEFORMATEX pwfxEncoded, 
	   IN DWORD dwThreshold, 
	   OUT LPXMEDIAOBJECT *ppMediaObject
	   );

XBOXAPI
HRESULT
WINAPI 
XVoiceDecoderCreateMediaObject(
	   IN DWORD dwCodecTag, 
	   IN LPWAVEFORMATEX pwfxDecoded, 
	   OUT LPXMEDIAOBJECT *ppMediaObject
	   );

typedef struct _QUEUE_XMO_CONFIG {

	BYTE	cbSize;
	WORD    wVoiceSamplingRate;    
	WORD    wMsOfDataPerPacket;   
	WORD    wBufferSize;
	WORD	wMaxDelay;				     
	WORD	wMinDelay;				     
	WORD	wInitialHighWaterMark;       	                                     
	DOUBLE  dIdealQuality;               
	DOUBLE  dInitialFrameStrength;       	                                     
}QUEUE_XMO_CONFIG , *LPQUEUE_XMO_CONFIG;


XBOXAPI 
HRESULT 
WINAPI 
XVoiceQueueCreateMediaObject(
       IN LPQUEUE_XMO_CONFIG pConfig , 
	   OUT LPXMEDIAOBJECT *ppMediaObject
	   );

XBOXAPI
HRESULT
WINAPI 
XVoiceGetCodecBufferSize( 
       IN const LPXMEDIAOBJECT pCodecMediaObject, 
	   IN WORD wPCMBufferSize , 
	   OUT WORD *pwCodecBufferSize
	   );

//////////////////////////////////////////////////////////////////////////////

typedef struct _VOICE_CHAT_CONFIG
{
    
	WORD           wMaxEncoders;
	WORD           wMaxChannels;
    WORD           wMaxDecodersPerChannel;
	WORD           wSendBufferingTime;
	WORD           wRecvBufferingTime;

}VOICE_CHAT_CONFIG, *LPVOICE_CHAT_CONFIG;

typedef enum _VOICE_EFFECTS
{
	NO_EFFECT

}VOICE_EFFECTS;

const BYTE COMMUNICATORS[] =  {0x01, 0x02, 0x04, 0x08};

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatStartup(
       IN  LPVOICE_CHAT_CONFIG     pConfig, 
	   OUT PHANDLE                 phVChatMgr
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatChatterAdd(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bTeamNo, 
	   IN BYTE                     bPlayerNo, 
	   IN BYTE                     bHeadsetPort
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatChatterRemove(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bPlayerNo
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatChatterGetStates(
       IN HANDLE                   hVChatMgr, 
	   OUT PBYTE                   pbStatus
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatMute(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bMutedPlayerNo, 
	   IN BYTE                     bMutingPlayerNo
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatVoiceEffect(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bPlayerNo, 
	   IN VOICE_EFFECTS            voiceEffect
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatPacketReceive(
       IN HANDLE                   hVChatMgr, 
	   IN PVOID                    pvNetworkPacketIn, 
	   IN BYTE                     bNetworkPacketInSize
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatPacketSend(
       IN HANDLE                   hVChatMgr, 
	   IN OUT PVOID                pvNetworkPacketOut, 
	   IN BYTE                     bNetworkPacketOutSize
	   );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatProcess(
       IN HANDLE                   hVChatMgr
	  );

XBOXAPI
HRESULT
WINAPI 
XOnlineVChatCleanup(
       IN HANDLE                   hVChatMgr
	   );

#ifdef __cplusplus
}
#endif

#endif  // __XVOICECHAT__
