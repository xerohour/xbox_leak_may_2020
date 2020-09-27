/************************************************************************
*                                                                       *
*   conductor.h -- This module contains the API for the                 *
*                  DirectMusic Producer Conductor module.               *
*                                                                       *
*   Copyright (c) 1998-1999, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef __Conductor_h__
#define __Conductor_h__

#include <windows.h>
#include "DMUSProd.h"

#define COM_NO_WINDOWS_H
#include <objbase.h>

#ifdef __cplusplus
extern "C"{
#endif 

typedef __int64         REFERENCE_TIME;
typedef long            MUSIC_TIME;

/* Forward Declarations */ 
interface IDMUSProdMidiInCPt;
interface IDMUSProdConductorTempo;
interface IDMUSProdPortNotify;
interface IDMUSProdTransport;
interface IDMUSProdSecondaryTransport;
interface IDMUSProdNotifyCPt;
interface IDMUSProdConductor;
#ifndef __cplusplus 
typedef interface IDMUSProdMidiInCPt IDMUSProdMidiInCPt;
typedef interface IDMUSProdConductorTempo IDMUSProdConductorTempo;
typedef interface IDMUSProdPortNotify IDMUSProdPortNotify;
typedef interface IDMUSProdTransport IDMUSProdTransport;
typedef interface IDMUSProdSecondaryTransport IDMUSProdSecondaryTransport;
typedef interface IDMUSProdNotifyCPt IDMUSProdNotifyCPt;
typedef interface IDMUSProdConductor IDMUSProdConductor;
#endif  /* __cplusplus */

/* Macros for registering and unregistering to receive MIDI input. */
#define REGISTER_MIDI_IN(pconductor,cookie) \
IConnectionPointContainer *pICPC = NULL;\
if( SUCCEEDED( pconductor->QueryInterface( IID_IConnectionPointContainer, (void**)&pICPC )) )\
{IConnectionPoint *pICP = NULL;\
 if(SUCCEEDED( pICPC->FindConnectionPoint( IID_IDMUSProdMidiInCPt, &pICP )) )\
 {if( FAILED( pICP->Advise( (IUnknown*)((IDMUSProdMidiInCPt*)this), &cookie ) ) )\
  {cookie = 0;}\
  pICP->Release();}\
 pICPC->Release();}
#define UNREGISTER_MIDI_IN(pconductor,cookie) \
IConnectionPointContainer *pICPC = NULL;\
if( SUCCEEDED( pconductor->QueryInterface( IID_IConnectionPointContainer, (void**)&pICPC )) )\
{IConnectionPoint *pICP = NULL;\
 if(SUCCEEDED( pICPC->FindConnectionPoint( IID_IDMUSProdMidiInCPt, &pICP )) )\
 {if( SUCCEEDED( pICP->Unadvise( cookie ) ) )\
  {cookie = 0;}\
  pICP->Release();}\
 pICPC->Release();}
DEFINE_GUID(GUID_ConductorCountInBeatOffset, 0xb413282, 0xdc09, 0x11d2, 0xb0, 0xf1, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);

typedef struct _ConductorNotifyEvent
{
    long  m_cbSize;
    DWORD m_dwType;
    BYTE* m_pbData;
} ConductorNotifyEvent;

typedef enum enumConductorTransitionFlags
{
	TRANS_END_OF_SEGMENT	= 0x1,
	TRANS_REGULAR_PATTERN	= 0x2,
	TRANS_NO_TRANSITION		= 0x4
} ConductorTransitionFlags;

typedef struct _ConductorTransitionOptions
{
    DWORD dwBoundaryFlags;
    WORD wPatternType;
    DWORD dwFlags;
} ConductorTransitionOptions;

typedef enum enumButtonStateFlags
{
	BS_AUTO_UPDATE	= 0,
	BS_NO_AUTO_UPDATE	= 0x1,
	BS_PLAY_ENABLED		= 0x2,
	BS_PLAY_CHECKED		= 0x4,
	BS_STOP_ENABLED		= 0x8,
	BS_STOP_CHECKED		= 0x10,
	BS_RECORD_ENABLED	= 0x20,
	BS_RECORD_CHECKED	= 0x40
} ButtonStateFlags;

/*////////////////////////////////////////////////////////////////////
// IDMUSProdMidiInCPt */
#undef  INTERFACE
#define INTERFACE  IDMUSProdMidiInCPt
DECLARE_INTERFACE_(IDMUSProdMidiInCPt, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdMidiInCPt */
    STDMETHOD(OnMidiMsg)        (THIS_ REFERENCE_TIME rtTime,
                                       BYTE bStatus,
                                       BYTE bData1,
                                       BYTE bData2) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdConductorTempo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdConductorTempo
DECLARE_INTERFACE_(IDMUSProdConductorTempo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdConductorTempo */
    STDMETHOD(SetTempo)         (THIS_ double dblTempo) PURE;
    STDMETHOD(GetTempo)         (THIS_ double __RPC_FAR *pdblTempo) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPortNotify */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPortNotify
DECLARE_INTERFACE_(IDMUSProdPortNotify, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdPortNotify */
    STDMETHOD(OnOutputPortsChanged) (THIS) PURE;
    STDMETHOD(OnOutputPortsRemoved) (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTransport */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTransport
DECLARE_INTERFACE_(IDMUSProdTransport, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdTransport */
    STDMETHOD(Play)             (THIS_ BOOL fPlayFromStart) PURE;
    STDMETHOD(Stop)             (THIS_ BOOL fStopImmediate) PURE;
    STDMETHOD(Transition)       (THIS) PURE;
    STDMETHOD(Record)           (THIS_ BOOL fPressed) PURE;
    STDMETHOD(GetName)          (THIS_ BSTR __RPC_FAR *pbstrName) PURE;
    STDMETHOD(TrackCursor)      (THIS_ BOOL fTrackCursor) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSecondaryTransport */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSecondaryTransport
DECLARE_INTERFACE_(IDMUSProdSecondaryTransport, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)				(THIS) PURE;
    STDMETHOD_(ULONG,Release)				(THIS) PURE;

    /* IDMUSProdSecondaryTransport */
    STDMETHOD(GetSecondaryTransportSegment) (THIS_ IUnknown __RPC_FAR *__RPC_FAR *ppunkISegment) PURE;
    STDMETHOD(GetSecondaryTransportName)    (THIS_ BSTR __RPC_FAR *pbstrName) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNotifyCPt */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNotifyCPt
DECLARE_INTERFACE_(IDMUSProdNotifyCPt, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdNotifyCPt */
    STDMETHOD(OnNotify)         (THIS_ ConductorNotifyEvent __RPC_FAR *pConductorNotifyEvent) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdConductor */
#undef  INTERFACE
#define INTERFACE  IDMUSProdConductor
DECLARE_INTERFACE_(IDMUSProdConductor, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)				(THIS) PURE;
    STDMETHOD_(ULONG,Release)				(THIS) PURE;

    /* IDMUSProdConductor */
    STDMETHOD(GetPerformanceEngine)			(THIS_ IUnknown __RPC_FAR *__RPC_FAR *ppIPerformance) PURE;
    STDMETHOD(GetPort)						(THIS_ IUnknown __RPC_FAR *__RPC_FAR *ppIPort) PURE;
    STDMETHOD(RegisterTransport)			(THIS_ IDMUSProdTransport __RPC_FAR *pITransport,
												   DWORD dwFlags) PURE;
    STDMETHOD(UnRegisterTransport)			(THIS_ IDMUSProdTransport __RPC_FAR *pITransport) PURE;
    STDMETHOD(SetActiveTransport)			(THIS_ IDMUSProdTransport __RPC_FAR *pITransport,
												   DWORD dwFlags) PURE;
    STDMETHOD(IsTransportPlaying)			(THIS_ IDMUSProdTransport __RPC_FAR *pITransport) PURE;
    STDMETHOD(PlayMIDIEvent)				(THIS_ BYTE bStatus,
												   BYTE bData1,
												   BYTE bData2,
												   DWORD dwTime) PURE;
    STDMETHOD(SetBtnStates)					(THIS_ IDMUSProdTransport __RPC_FAR *pITransport,
												   DWORD dwFlags) PURE;
    STDMETHOD(SetTempo)						(THIS_ IDMUSProdTransport __RPC_FAR *pITransport,
												   double dblTempo,
												   BOOL fEnable) PURE;
    STDMETHOD(RegisterNotify)				(THIS_ IDMUSProdNotifyCPt __RPC_FAR *pINotifyCPt,
												   REFGUID rguidNotify) PURE;
    STDMETHOD(UnregisterNotify)				(THIS_ IDMUSProdNotifyCPt __RPC_FAR *pINotifyCPt,
												   REFGUID rguidNotify) PURE;
    STDMETHOD(SetTransportName)				(THIS_ IDMUSProdTransport __RPC_FAR *pITransport,
												   BSTR bstrName) PURE;
    STDMETHOD(TransportStopped)				(THIS_ IDMUSProdTransport __RPC_FAR *pITransport) PURE;
    STDMETHOD(GetTransitionOptions)			(THIS_ ConductorTransitionOptions __RPC_FAR *pTransitionOptions) PURE;
    STDMETHOD(RegisterSecondaryTransport)	(THIS_ IDMUSProdSecondaryTransport __RPC_FAR *pISecondaryTransport) PURE;
    STDMETHOD(UnRegisterSecondaryTransport)	(THIS_ IDMUSProdSecondaryTransport __RPC_FAR *pISecondaryTransport) PURE;
    STDMETHOD(SetSecondaryTransportName)	(THIS_ IDMUSProdSecondaryTransport __RPC_FAR *pISecondaryTransport,
												   BSTR bstrName) PURE;
    STDMETHOD(IsSecondaryTransportPlaying)	(THIS_ IDMUSProdSecondaryTransport __RPC_FAR *pISecondaryTransport) PURE;
    STDMETHOD(SetPChannelThru)				(THIS_ DWORD dwInputChannel,
												   DWORD dwPChannel) PURE;
    STDMETHOD(CancelPChannelThru)			(THIS_ DWORD dwInputChannel) PURE;
    STDMETHOD(IsTrackCursorEnabled)			(THIS) PURE;
};

/* CLSID's */
DEFINE_GUID(CLSID_CConductor, 0x36f6dde8, 0x46ce, 0x11d0, 0xb9, 0xdb, 0x0, 0xaa, 0x0, 0xc0, 0x81, 0x46);

/* IID's */
DEFINE_GUID(IID_IDMUSProdMidiInCPt, 0xaa349de0, 0xef0e, 0x11d0, 0xad, 0x22, 0x0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac);
DEFINE_GUID(IID_IDMUSProdConductorTempo, 0x32f40a16, 0x5f8f, 0x41a0, 0xb6, 0xbe, 0x51, 0x25, 0x7d, 0x1f, 0xe6, 0x88);
DEFINE_GUID(IID_IDMUSProdPortNotify, 0x9bd50920, 0x3c3c, 0x11d2, 0x8a, 0xa2, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTransport, 0x8c7b3a62, 0xd33d, 0x11d2, 0xb0, 0xf0, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(IID_IDMUSProdSecondaryTransport, 0x52d006fa, 0x2281, 0x11d2, 0x88, 0xf7, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdNotifyCPt, 0xfa273400, 0x9f23, 0x11d1, 0x88, 0x77, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdConductor, 0xe8a27a9, 0x473e, 0x4f48, 0x82, 0x21, 0x57, 0x5b, 0xe7, 0x5, 0x95, 0x65);

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __Conductor_h__ */
