/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xvoice.h
 *  Content:    Voice include file
//@@BEGIN_MSINTERNAL
 *  History:
 *  Date        By          Reason
 *  =====       =======     ==================================================
 //@@END_MSINTERNAL
 ***************************************************************************/

#ifndef __XVOICE__
#define __XVOICE__

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
	DWORD   dwCodecTag;    
	WORD    wMsOfDataPerPacket;   
	WORD	wMaxDelay;				     
	WORD	wMinDelay;				     
	WORD	wInitialHighWaterMark;       	                                     
	DOUBLE  dIdealQuality;               
	DOUBLE  dInitialFrameStrength;       	                                     
}QUEUE_XMO_CONFIG , *LPQUEUE_XMO_CONFIG;

typedef struct _PCM_INFO {

	BYTE	cbSize;		
	INT     nMaxPower;  
}PCM_INFO , *LPPCM_INFO;

typedef struct XVoiceQueueMediaObject XVoiceQueueMediaObject , *LPXVOICEQUEUEMEDIAOBJECT;

XBOXAPI 
HRESULT 
WINAPI 
XVoiceQueueCreateMediaObject(
       IN LPQUEUE_XMO_CONFIG pConfig , 
	   OUT LPXVOICEQUEUEMEDIAOBJECT *ppVoiceQueueMediaObject
	   );

XBOXAPI
HRESULT
WINAPI 
XVoiceGetCodecBufferSize( 
       IN const LPXMEDIAOBJECT pCodecMediaObject, 
	   IN WORD wPCMBufferSize , 
	   OUT WORD *pwCodecBufferSize
	   );

#undef INTERFACE
#define INTERFACE XVoiceQueueMediaObject

DECLARE_INTERFACE_(XVoiceQueueMediaObject, XMediaObject)
{
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // XMediaObject methods
    STDMETHOD(GetInfo)(THIS_ LPXMEDIAINFO pInfo) PURE;
    STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus) PURE;
    STDMETHOD(Process)(THIS_ LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket) PURE;
    STDMETHOD(Discontinuity)(THIS) PURE;
    STDMETHOD(Flush)(THIS) PURE;

    // XVoiceQueueMediaObject methods
    STDMETHOD(GetLastOutputInfo)(THIS_ LPPCM_INFO pPCMInfo) PURE;
};

#define XVoiceQueueMediaObject_AddRef             IUnknown_AddRef
#define XVoiceQueueMediaObject_Release            IUnknown_Release

#define XVoiceQueueMediaObject_GetInfo            XMediaObject_GetInfo
#define XVoiceQueueMediaObject_GetStatus          XMediaObject_GetStatus
#define XVoiceQueueMediaObject_Process            XMediaObject_Process
#define XVoiceQueueMediaObject_Discontinuity      XMediaObject_Discontinuity
#define XVoiceQueueMediaObject_Flush              XMediaObject_Flush

#if defined(__cplusplus) && !defined(CINTERFACE)

#define XVoiceQueueMediaObject_GetLastOutputInfo(p, a)    p->GetLastOutputInfo(a)

#else // defined(__cplusplus) && !defined(CINTERFACE)

#define XVoiceQueueMediaObject_GetLastOutputInfo(p, a)    p->lpVtbl->GetLastOutputInfo(p, a)

#endif // defined(__cplusplus) && !defined(CINTERFACE)

#ifdef __cplusplus
}
#endif

#endif  // __XVOICE__
