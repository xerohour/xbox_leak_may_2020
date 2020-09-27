/************************************************************************
*                                                                       *
*   DMUSProd.h -- This module contains the APIs for                     *
*                 DirectMusic Producer                                  *
*                                                                       *
*   Copyright (c) 1998-2001, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef __DMUSPROD_H__1FD3B972_F3E7_11D0_89AE_00A0C9054129__INCLUDED_
#define __DMUSPROD_H__1FD3B972_F3E7_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>

#ifdef __cplusplus
extern "C"{
#endif 

typedef __int64 REFERENCE_TIME;
typedef long    MUSIC_TIME;
struct _DMUS_AUDIOPARAMS;
#ifndef __cplusplus 
typedef struct _DMUS_AUDIOPARAMS _DMUS_AUDIOPARAMS;
#endif

#define CF_DMUSPROD_FILE "DMUSProd File v.1"
#define FOURCC_DMUSPROD_FILEREF mmioFOURCC('j','z','f','r')
#define COMPOSITION_PRIORITY_SIGNPOST_STRIP 1024
#define COMPOSITION_PRIORITY_MELODYGEN_STRIP 512
#define AUDIOPATH_UNLOAD_DOWNLOAD_WAVES 0x00000001

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


/* Forward Declarations */ 

interface IDMUSProdSegmentEdit;
interface IDMUSProdMidiInCPt;
interface IDMUSProdConductorTempo;
interface IDMUSProdPortNotify;
interface IDMUSProdTransport;
interface IDMUSProdSecondaryTransport;
interface IDMUSProdNotifyCPt;
interface IDMUSProdConductor;
interface IDMUSProdStrip;
interface IDMUSProdStripFunctionBar;
interface IDMUSProdStripMgr;
interface IDMUSProdFramework;
interface IDMUSProdFramework8;
interface IDMUSProdNode;
interface IDMUSProdPropPageManager;
interface IDMUSProdPropPageObject;
interface IDMUSProdProject;
interface IDMUSProdComponent;
interface IDMUSProdRIFFExt;
interface IDMUSProdPersistInfo;
interface IDMUSProdDocType;
interface IDMUSProdEditor;
interface IDMUSProdSortNode;
interface IDMUSProdMenu;
interface IDMUSProdToolBar;
interface IDMUSProdToolBar8;
interface IDMUSProdPropSheet;
interface IDMUSProdFileRefChunk;
interface IDMUSProdLoaderRefChunk;
interface IDMUSProdNodeDrop;
interface IDMUSProdNotifySink;
interface IDMUSProdPChannelName;
interface IDMUSProdTimelineDataObject;
interface IDMUSProdTimeline;
interface IDMUSProdTimelineCallback;
interface IDMUSProdTimelineEdit;
#ifndef __cplusplus 
typedef interface IDMUSProdSegmentEdit IDMUSProdSegmentEdit;
typedef interface IDMUSProdMidiInCPt IDMUSProdMidiInCPt;
typedef interface IDMUSProdConductorTempo IDMUSProdConductorTempo;
typedef interface IDMUSProdPortNotify IDMUSProdPortNotify;
typedef interface IDMUSProdTransport IDMUSProdTransport;
typedef interface IDMUSProdSecondaryTransport IDMUSProdSecondaryTransport;
typedef interface IDMUSProdNotifyCPt IDMUSProdNotifyCPt;
typedef interface IDMUSProdConductor IDMUSProdConductor;
typedef interface IDMUSProdStrip IDMUSProdStrip;
typedef interface IDMUSProdStripFunctionBar IDMUSProdStripFunctionBar;
typedef interface IDMUSProdStripMgr IDMUSProdStripMgr;
typedef interface IDMUSProdFramework IDMUSProdFramework;
typedef interface IDMUSProdFramework8 IDMUSProdFramework8;
typedef interface IDMUSProdNode IDMUSProdNode;
typedef interface IDMUSProdPropPageManager IDMUSProdPropPageManager;
typedef interface IDMUSProdPropPageObject IDMUSProdPropPageObject;
typedef interface IDMUSProdProject IDMUSProdProject;
typedef interface IDMUSProdComponent IDMUSProdComponent;
typedef interface IDMUSProdRIFFExt IDMUSProdRIFFExt;
typedef interface IDMUSProdPersistInfo IDMUSProdPersistInfo;
typedef interface IDMUSProdDocType IDMUSProdDocType;
typedef interface IDMUSProdEditor IDMUSProdEditor;
typedef interface IDMUSProdSortNode IDMUSProdSortNode;
typedef interface IDMUSProdMenu IDMUSProdMenu;
typedef interface IDMUSProdToolBar IDMUSProdToolBar;
typedef interface IDMUSProdToolBar8 IDMUSProdToolBar8;
typedef interface IDMUSProdPropSheet IDMUSProdPropSheet;
typedef interface IDMUSProdFileRefChunk IDMUSProdFileRefChunk;
typedef interface IDMUSProdLoaderRefChunk IDMUSProdLoaderRefChunk;
typedef interface IDMUSProdNodeDrop IDMUSProdNodeDrop;
typedef interface IDMUSProdNotifySink IDMUSProdNotifySink;
typedef interface IDMUSProdPChannelName IDMUSProdPChannelName;
typedef interface IDMUSProdTimelineDataObject IDMUSProdTimelineDataObject;
typedef interface IDMUSProdTimeline IDMUSProdTimeline;
typedef interface IDMUSProdTimelineCallback IDMUSProdTimelineCallback;
typedef interface IDMUSProdTimelineEdit IDMUSProdTimelineEdit;
#endif /* __cplusplus */


/* Enumarations */

typedef enum enumSTRIPVIEW
{
    SV_NORMAL                = 0,
    SV_MINIMIZED             = SV_NORMAL + 1,
    SV_FUNCTIONBAR_NORMAL    = SV_MINIMIZED + 1,
    SV_FUNCTIONBAR_MINIMIZED = SV_FUNCTIONBAR_NORMAL + 1
} STRIPVIEW;

typedef enum enumSTRIPPROPERTY
{
    SP_NAME                      = 0,
    SP_GUTTERSELECTABLE          = SP_NAME + 1,
    SP_GUTTERSELECT              = SP_GUTTERSELECTABLE + 1,
    SP_BEGINSELECT               = SP_GUTTERSELECT + 1,
    SP_ENDSELECT                 = SP_BEGINSELECT + 1,
    SP_CURSOR_HANDLE             = SP_ENDSELECT + 1,
    SP_DEFAULTHEIGHT             = SP_CURSOR_HANDLE + 1,
    SP_RESIZEABLE                = SP_DEFAULTHEIGHT + 1,
    SP_MAXHEIGHT                 = SP_RESIZEABLE + 1,
    SP_MINHEIGHT                 = SP_MAXHEIGHT + 1,
    SP_MINMAXABLE                = SP_MINHEIGHT + 1,
    SP_MINIMIZE_HEIGHT           = SP_MINMAXABLE + 1,
    SP_STRIPMGR                  = SP_MINIMIZE_HEIGHT + 1,
    SP_FUNCTIONBAR_EXCLUDE_WIDTH = SP_STRIPMGR + 1,
    /* Following added for DirectX 8 */
    SP_EARLY_TIME                = SP_FUNCTIONBAR_EXCLUDE_WIDTH + 1,
    SP_LATE_TIME                 = SP_EARLY_TIME + 1,
    SP_USER                      = 1024
} STRIPPROPERTY;

typedef enum enumSTRIPMGRPROPERTY
{
    SMP_ITIMELINECTL        = 0,
    SMP_IDIRECTMUSICTRACK   = SMP_ITIMELINECTL + 1,
    SMP_IDMUSPRODFRAMEWORK  = SMP_IDIRECTMUSICTRACK + 1,
    SMP_DMUSIOTRACKHEADER   = SMP_IDMUSPRODFRAMEWORK + 1,
    /* Following added for DirectX 8 */
    SMP_DMUSIOTRACKEXTRASHEADER = SMP_DMUSIOTRACKHEADER + 1,
    SMP_PRODUCERONLY_FLAGS  = SMP_DMUSIOTRACKEXTRASHEADER + 1,
    SMP_DMUSIOTRACKEXTRASHEADER_MASK = SMP_PRODUCERONLY_FLAGS + 1,
    SMP_USER                = 1024
} STRIPMGRPROPERTY;

typedef enum enumConductorTransitionFlags
{
    TRANS_END_OF_SEGMENT    = 0x1,
    TRANS_REGULAR_PATTERN   = 0x2,
    TRANS_NO_TRANSITION     = 0x4
} ConductorTransitionFlags;

typedef enum enumButtonStateFlags
{
    BS_AUTO_UPDATE  = 0,
    BS_NO_AUTO_UPDATE   = 0x1,
    BS_PLAY_ENABLED     = 0x2,
    BS_PLAY_CHECKED     = 0x4,
    BS_STOP_ENABLED     = 0x8,
    BS_STOP_CHECKED     = 0x10,
    BS_RECORD_ENABLED   = 0x20,
    BS_RECORD_CHECKED   = 0x40
} ButtonStateFlags;

typedef enum enumStatusBarStyle
{
    SBS_NOBORDERS   = 0,
    SBS_RAISED      = SBS_NOBORDERS + 1,
    SBS_SUNKEN      = SBS_RAISED + 1
} StatusBarStyle;

typedef enum enumStatusBarLifeSpan
{
    SBLS_APPLICATION    = 0,
    SBLS_EDITOR         = 1000,
    SBLS_CONTROL        = 2000,
    SBLS_MOUSEDOWNUP    = 3000
} StatusBarLifeSpan;

typedef enum enumFileType
{
    FT_UNKNOWN  = 0,
    FT_DESIGN   = FT_UNKNOWN + 1,
    FT_RUNTIME  = FT_DESIGN + 1
}FileType;

typedef enum enumWhichLoader
{
    WL_DIRECTMUSIC  = 0,
    WL_PRODUCER = WL_DIRECTMUSIC + 1
} WhichLoader;

typedef enum enumTIMETYPE
{
    TIMETYPE_CLOCKS = 0,
    TIMETYPE_MS     = TIMETYPE_CLOCKS + 1
} TIMETYPE;

typedef enum enumTIMELINE_PROPERTY
{
    TP_CLOCKLENGTH       = 1,
    TP_STRIPMOUSECAPTURE = TP_CLOCKLENGTH + 1,
    TP_DMUSPRODFRAMEWORK = TP_STRIPMOUSECAPTURE + 1,
    TP_TIMELINECALLBACK  = TP_DMUSPRODFRAMEWORK + 1,
    TP_ACTIVESTRIP       = TP_TIMELINECALLBACK + 1,
    TP_FUNCTIONBAR_WIDTH = TP_ACTIVESTRIP + 1,
    TP_MAXIMUM_HEIGHT    = TP_FUNCTIONBAR_WIDTH + 1,
    TP_ZOOM              = TP_MAXIMUM_HEIGHT + 1,
    TP_HORIZONTAL_SCROLL = TP_ZOOM + 1,
    TP_VERTICAL_SCROLL   = TP_HORIZONTAL_SCROLL + 1,
    TP_SNAPAMOUNT        = TP_VERTICAL_SCROLL + 1,
    TP_FREEZE_UNDO       = TP_SNAPAMOUNT + 1,
    TP_SNAP_TO           = TP_FREEZE_UNDO + 1,
    TP_FUNCTIONNAME_HEIGHT = TP_SNAP_TO + 1,
	TP_TOP_STRIP         = TP_FUNCTIONNAME_HEIGHT + 1
} TIMELINE_PROPERTY;

typedef enum enumDMUSPROD_TIMELINE_SNAP_TO
{
    DMUSPROD_TIMELINE_SNAP_NONE = 0,
    DMUSPROD_TIMELINE_SNAP_GRID = 1,
    DMUSPROD_TIMELINE_SNAP_BEAT = 2,
    DMUSPROD_TIMELINE_SNAP_BAR  = 3
} DMUSPROD_TIMELINE_SNAP_TO;

typedef enum enumSTRIP_TIMELINE_PROPERTY
{
    STP_VERTICAL_SCROLL   = 0,
    STP_GET_HDC           = STP_VERTICAL_SCROLL + 1,
    STP_HEIGHT            = STP_GET_HDC + 1,
    STP_STRIPVIEW         = STP_HEIGHT + 1,
    STP_STRIP_RECT        = STP_STRIPVIEW + 1,
    STP_FBAR_RECT         = STP_STRIP_RECT + 1,
    STP_POSITION          = STP_FBAR_RECT + 1,
    STP_GUTTER_SELECTED   = STP_POSITION + 1,
    STP_ENTIRE_STRIP_RECT = STP_GUTTER_SELECTED + 1,
    /* Following added for DirectX 8 */
    STP_FBAR_CLIENT_RECT  = STP_ENTIRE_STRIP_RECT + 1,
    STP_STRIP_INFO        = STP_FBAR_CLIENT_RECT + 1
} STRIP_TIMELINE_PROPERTY;

typedef enum enumMARKERID
{
    MARKER_CURRENTTIME  = 0,
    MARKER_BEGINSELECT  = MARKER_CURRENTTIME + 1,
    MARKER_ENDSELECT    = MARKER_BEGINSELECT + 1,
    MARKER_LEFTDISPLAY  = MARKER_ENDSELECT + 1,
    MARKER_RIGHTDISPLAY  = MARKER_LEFTDISPLAY + 1
} MARKERID;

typedef enum enumMUSICLINE_PROPERTY
{
    ML_DRAW_MEASURE_BEAT_GRID = 0,
    ML_DRAW_MEASURE_BEAT      = ML_DRAW_MEASURE_BEAT_GRID + 1
} MUSICLINE_PROPERTY;

typedef enum enumTIMELINE_PASTE_TYPE
{
    TL_PASTE_MERGE     = 0,
    TL_PASTE_OVERWRITE = TL_PASTE_MERGE + 1
} TIMELINE_PASTE_TYPE;


/* Structures */

typedef struct _DMUSProdFrameworkMsg
{
    GUID guidUpdateType;
    IUnknown *punkIDMUSProdNode;
    void *pData;
} DMUSProdFrameworkMsg;

typedef struct _ConductorNotifyEvent
{
    long  m_cbSize;
    DWORD m_dwType;
    BYTE* m_pbData;
} ConductorNotifyEvent;

typedef struct _ConductorTransitionOptions
{
    DWORD dwBoundaryFlags;
    WORD wPatternType;
    DWORD dwFlags;
    /* Following added for DirectX 8 */
    IUnknown *pDMUSProdNodeSegmentTransition;
} ConductorTransitionOptions;

typedef struct _DMUSProdListInfo
{
    WORD wSize;
    BSTR bstrName;
    BSTR bstrDescriptor;
    GUID guidObject;
} DMUSProdListInfo;

typedef struct _DMUSProdStreamInfo
{
    FileType ftFileType;
    GUID guidDataFormat;
    IDMUSProdNode *pITargetDirectoryNode;
} DMUSProdStreamInfo;

/* Following structures added for DirectX 8 */
typedef struct DMUSProdFileRefInfo
{
    WORD    wSize;
    GUID    guidFile;
    GUID    guidDocRootNodeId;
} DMUSProdFileRefInfo;

typedef struct _DMUSProdTimelineStripInfo
{
    CLSID   clsidType;
    DWORD   dwGroupBits;
    DWORD   dwIndex;
} DMUSProdTimelineStripInfo;
#define DMUSPROD_TIMELINE_STRIP_INFO DMUSProdTimelineStripInfo

typedef struct DMUSProdDMOInfo
{
    DWORD	dwSize;
	CLSID	clsidDMO;
	DWORD	dwStage;
	DWORD	dwPChannel;
	DWORD	dwBufferIndex;
	DWORD	dwEffectIndex;
	WCHAR	awchPChannelText[65];
	WCHAR	awchMixGroupName[65];
	WCHAR	awchBufferText[65];
	WCHAR	awchDMOName[65];
	GUID	guidDesignGUID;
} DMUSProdDMOInfo;

typedef struct _DMUSPROD_WAVE_PARAM
{
    REFERENCE_TIME  rtTimePhysical; /* Either reference time or music time */
    DWORD			dwPChannel;		/* Used to identify part */ 
    DWORD			dwIndex;		/* Used to identify part */ 
    IDMUSProdNode*  pIWaveNode;
} DMUSPROD_WAVE_PARAM;

typedef struct _DMUSPROD_WAVE_PARAM2
{
    REFERENCE_TIME  rtTimePhysical; /* Either reference time or music time */
    DWORD			dwPChannel;		/* Used to identify part */ 
    DWORD			dwIndex;		/* Used to identify part */ 
    IDMUSProdNode*  pIWaveNode;
	DWORD			dwVariation;
} DMUSPROD_WAVE_PARAM2;

typedef struct _DMUSPROD_INTERFACEFORPCHANNEL
{
	DWORD			dwPChannel;
    IUnknown*       punkInterface;
} DMUSPROD_INTERFACEFORPCHANNEL;

/* Interfaces */

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSegmentEdit */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSegmentEdit
DECLARE_INTERFACE_(IDMUSProdSegmentEdit, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdSegmentEdit */
    STDMETHOD(AddStrip)             (THIS_ CLSID clsidTrackType,
                                           DWORD dwGroupBits,
                                           IUnknown **ppIStripMgr) PURE;
    STDMETHOD(ContainsTempoStrip)   (THIS_ BOOL *pfTempoStripExists) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSegmentEdit8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSegmentEdit8
DECLARE_INTERFACE_(IDMUSProdSegmentEdit8, IDMUSProdSegmentEdit)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdSegmentEdit */
    STDMETHOD(AddStrip)             (THIS_ CLSID clsidTrackType,
                                           DWORD dwGroupBits,
                                           IUnknown **ppIStripMgr) PURE;
    STDMETHOD(ContainsTempoStrip)   (THIS_ BOOL *pfTempoStripExists) PURE;

    /* IDMUSProdSegmentEdit8 */
    STDMETHOD(GetObjectDescriptor)	(THIS_ void* pObjectDesc ) PURE;
    STDMETHOD(RemoveStripMgr)		(THIS_ IUnknown* pIStripMgr ) PURE;
    STDMETHOD(EnumTrack)			(THIS_ DWORD dwIndex,
										   DWORD dwTrackHeaderSize,
										   void *pioTrackHeader,
										   DWORD dwTrackExtrasHeaderSize,
										   void *pioTrackExtrasHeader ) PURE;
    STDMETHOD(ReplaceTrackInStrip)	(THIS_ IUnknown* pIStripMgr,
										   IUnknown* pITrack ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdMidiInCPt */
#undef  INTERFACE
#define INTERFACE  IDMUSProdMidiInCPt
DECLARE_INTERFACE_(IDMUSProdMidiInCPt, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
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
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdConductorTempo */
    STDMETHOD(SetTempo)         (THIS_ double dblTempo) PURE;
    STDMETHOD(GetTempo)         (THIS_ double *pdblTempo) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPortNotify */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPortNotify
DECLARE_INTERFACE_(IDMUSProdPortNotify, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
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
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdTransport */
    STDMETHOD(Play)             (THIS_ BOOL fPlayFromStart) PURE;
    STDMETHOD(Stop)             (THIS_ BOOL fStopImmediate) PURE;
    STDMETHOD(Transition)       (THIS) PURE;
    STDMETHOD(Record)           (THIS_ BOOL fPressed) PURE;
    STDMETHOD(GetName)          (THIS_ BSTR *pbstrName) PURE;
    STDMETHOD(TrackCursor)      (THIS_ BOOL fTrackCursor) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSecondaryTransport */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSecondaryTransport
DECLARE_INTERFACE_(IDMUSProdSecondaryTransport, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)               (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)                (THIS) PURE;
    STDMETHOD_(ULONG,Release)               (THIS) PURE;

    /* IDMUSProdSecondaryTransport */
    STDMETHOD(GetSecondaryTransportSegment) (THIS_ IUnknown **ppunkISegment) PURE;
    STDMETHOD(GetSecondaryTransportName)    (THIS_ BSTR *pbstrName) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNotifyCPt */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNotifyCPt
DECLARE_INTERFACE_(IDMUSProdNotifyCPt, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdNotifyCPt */
    STDMETHOD(OnNotify)         (THIS_ ConductorNotifyEvent *pConductorNotifyEvent) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdConductor */
#undef  INTERFACE
#define INTERFACE  IDMUSProdConductor
DECLARE_INTERFACE_(IDMUSProdConductor, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)               (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)                (THIS) PURE;
    STDMETHOD_(ULONG,Release)               (THIS) PURE;

    /* IDMUSProdConductor */
    STDMETHOD(GetPerformanceEngine)         (THIS_ IUnknown **ppIPerformance) PURE;
    STDMETHOD(GetPort)                      (THIS_ IUnknown **ppIPort) PURE;
    STDMETHOD(RegisterTransport)            (THIS_ IDMUSProdTransport *pITransport,
                                                   DWORD dwFlags) PURE;
    STDMETHOD(UnRegisterTransport)          (THIS_ IDMUSProdTransport *pITransport) PURE;
    STDMETHOD(SetActiveTransport)           (THIS_ IDMUSProdTransport *pITransport,
                                                   DWORD dwFlags) PURE;
    STDMETHOD(IsTransportPlaying)           (THIS_ IDMUSProdTransport *pITransport) PURE;
    STDMETHOD(PlayMIDIEvent)                (THIS_ BYTE bStatus,
                                                   BYTE bData1,
                                                   BYTE bData2,
                                                   DWORD dwTime) PURE;
    STDMETHOD(SetBtnStates)                 (THIS_ IDMUSProdTransport *pITransport,
                                                   DWORD dwFlags) PURE;
    STDMETHOD(SetTempo)                     (THIS_ IDMUSProdTransport *pITransport,
                                                   double dblTempo,
                                                   BOOL fEnable) PURE;
    STDMETHOD(RegisterNotify)               (THIS_ IDMUSProdNotifyCPt *pINotifyCPt,
                                                   REFGUID rguidNotify) PURE;
    STDMETHOD(UnregisterNotify)             (THIS_ IDMUSProdNotifyCPt *pINotifyCPt,
                                                   REFGUID rguidNotify) PURE;
    STDMETHOD(SetTransportName)             (THIS_ IDMUSProdTransport *pITransport,
                                                   BSTR bstrName) PURE;
    STDMETHOD(TransportStopped)             (THIS_ IDMUSProdTransport *pITransport) PURE;
    STDMETHOD(GetTransitionOptions)         (THIS_ ConductorTransitionOptions *pTransitionOptions) PURE;
    STDMETHOD(RegisterSecondaryTransport)   (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport) PURE;
    STDMETHOD(UnRegisterSecondaryTransport) (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport) PURE;
    STDMETHOD(SetSecondaryTransportName)    (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport,
                                                   BSTR bstrName) PURE;
    STDMETHOD(IsSecondaryTransportPlaying)  (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport) PURE;
    STDMETHOD(SetPChannelThru)              (THIS_ DWORD dwInputChannel,
                                                   DWORD dwPChannel) PURE;
    STDMETHOD(CancelPChannelThru)           (THIS_ DWORD dwInputChannel) PURE;
    STDMETHOD(IsTrackCursorEnabled)         (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdConductor8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdConductor8
DECLARE_INTERFACE_(IDMUSProdConductor8, IDMUSProdConductor)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)               (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)                (THIS) PURE;
    STDMETHOD_(ULONG,Release)               (THIS) PURE;

    /* IDMUSProdConductor */
    STDMETHOD(GetPerformanceEngine)         (THIS_ IUnknown **ppIPerformance) PURE;
    STDMETHOD(GetPort)                      (THIS_ IUnknown **ppIPort) PURE;
    STDMETHOD(RegisterTransport)            (THIS_ IDMUSProdTransport *pITransport,
                                                   DWORD dwFlags) PURE;
    STDMETHOD(UnRegisterTransport)          (THIS_ IDMUSProdTransport *pITransport) PURE;
    STDMETHOD(SetActiveTransport)           (THIS_ IDMUSProdTransport *pITransport,
                                                   DWORD dwFlags) PURE;
    STDMETHOD(IsTransportPlaying)           (THIS_ IDMUSProdTransport *pITransport) PURE;
    STDMETHOD(PlayMIDIEvent)                (THIS_ BYTE bStatus,
                                                   BYTE bData1,
                                                   BYTE bData2,
                                                   DWORD dwTime) PURE;
    STDMETHOD(SetBtnStates)                 (THIS_ IDMUSProdTransport *pITransport,
                                                   DWORD dwFlags) PURE;
    STDMETHOD(SetTempo)                     (THIS_ IDMUSProdTransport *pITransport,
                                                   double dblTempo,
                                                   BOOL fEnable) PURE;
    STDMETHOD(RegisterNotify)               (THIS_ IDMUSProdNotifyCPt *pINotifyCPt,
                                                   REFGUID rguidNotify) PURE;
    STDMETHOD(UnregisterNotify)             (THIS_ IDMUSProdNotifyCPt *pINotifyCPt,
                                                   REFGUID rguidNotify) PURE;
    STDMETHOD(SetTransportName)             (THIS_ IDMUSProdTransport *pITransport,
                                                   BSTR bstrName) PURE;
    STDMETHOD(TransportStopped)             (THIS_ IDMUSProdTransport *pITransport) PURE;
    STDMETHOD(GetTransitionOptions)         (THIS_ ConductorTransitionOptions *pTransitionOptions) PURE;
    STDMETHOD(RegisterSecondaryTransport)   (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport) PURE;
    STDMETHOD(UnRegisterSecondaryTransport) (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport) PURE;
    STDMETHOD(SetSecondaryTransportName)    (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport,
                                                   BSTR bstrName) PURE;
    STDMETHOD(IsSecondaryTransportPlaying)  (THIS_ IDMUSProdSecondaryTransport *pISecondaryTransport) PURE;
    STDMETHOD(SetPChannelThru)              (THIS_ DWORD dwInputChannel,
                                                   DWORD dwPChannel) PURE;
    STDMETHOD(CancelPChannelThru)           (THIS_ DWORD dwInputChannel) PURE;
    STDMETHOD(IsTrackCursorEnabled)         (THIS) PURE;

    /* IDMUSProdConductor8 */
    STDMETHOD(RegisterAudiopath)            (THIS_ IDMUSProdNode *pAudiopathNode,
                                                   BSTR bstrName ) PURE;
    STDMETHOD(UnRegisterAudiopath)          (THIS_ IDMUSProdNode *pAudiopathNode ) PURE;
    STDMETHOD(SetAudiopathName)             (THIS_ IDMUSProdNode *pAudiopathNode,
                                                   BSTR bstrName ) PURE;
    STDMETHOD(GetDownloadCustomDLSStatus)   (THIS_ BOOL *pfDownloadCustomDLS ) PURE;
    STDMETHOD(GetDownloadGMStatus)          (THIS_ BOOL *pfDownloadGM ) PURE;
    STDMETHOD(SetupMIDIAndWaveSave)         (THIS_ IUnknown *punkSegmentState ) PURE;
    STDMETHOD(GetAudioParams)               (THIS_ _DMUS_AUDIOPARAMS *pDMUSAudioParams ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdStrip */
#undef  INTERFACE
#define INTERFACE  IDMUSProdStrip
DECLARE_INTERFACE_(IDMUSProdStrip, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdStrip */
    STDMETHOD(Draw)             (THIS_ HDC hDC,
                                       STRIPVIEW stripView,
                                       LONG lXOffset) PURE;
    STDMETHOD(GetStripProperty) (THIS_ STRIPPROPERTY stripProperty,
                                       VARIANT *pVariant) PURE;
    STDMETHOD(SetStripProperty) (THIS_ STRIPPROPERTY stripProperty,
                                       VARIANT variant) PURE;
    STDMETHOD(OnWMMessage)      (THIS_ UINT nMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       LONG lXPos,
                                       LONG lYPos) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdStripFunctionBar */
#undef  INTERFACE
#define INTERFACE  IDMUSProdStripFunctionBar
DECLARE_INTERFACE_(IDMUSProdStripFunctionBar, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdStripFunctionBar */
    STDMETHOD(FBDraw)           (THIS_ HDC hDC,
                                       STRIPVIEW stripView) PURE;
    STDMETHOD(FBOnWMMessage)    (THIS_ UINT nMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       LONG lXPos,
                                       LONG lYPos) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdStripMgr */
#undef  INTERFACE
#define INTERFACE  IDMUSProdStripMgr
DECLARE_INTERFACE_(IDMUSProdStripMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdStripMgr */
    STDMETHOD(IsParamSupported)     (THIS_ REFGUID rguidType) PURE;
    STDMETHOD(GetParam)             (THIS_ REFGUID rguidType,
                                           MUSIC_TIME mtTime,
                                           MUSIC_TIME *pmtNext,
                                           void *pData) PURE;
    STDMETHOD(SetParam)             (THIS_ REFGUID rguidType,
                                           MUSIC_TIME mtTime,
                                           void *pData) PURE;
    STDMETHOD(OnUpdate)             (THIS_ REFGUID rguidType,
                                           DWORD dwGroupBits,
                                           void *pData) PURE;
    STDMETHOD(GetStripMgrProperty)  (THIS_ STRIPMGRPROPERTY stripMgrProperty,
                                           VARIANT *pVariant) PURE;
    STDMETHOD(SetStripMgrProperty)  (THIS_ STRIPMGRPROPERTY stripMgrProperty,
                                           VARIANT variant) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdFramework */
#undef  INTERFACE
#define INTERFACE  IDMUSProdFramework
DECLARE_INTERFACE_(IDMUSProdFramework, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdFramework */
    STDMETHOD(FindComponent)            (THIS_ REFCLSID rclsid,
                                               IDMUSProdComponent **ppIComponent) PURE;
    STDMETHOD(GetFirstComponent)        (THIS_ IDMUSProdComponent **ppIFirstComponent) PURE;
    STDMETHOD(GetNextComponent)         (THIS_ IDMUSProdComponent *pIComponent,
                                               IDMUSProdComponent **ppINextComponent) PURE;
    STDMETHOD(AddDocType)               (THIS_ IDMUSProdDocType *pIDocType) PURE;
    STDMETHOD(FindDocTypeByExtension)   (THIS_ BSTR bstrExt,
                                               IDMUSProdDocType **ppIDocType) PURE;
    STDMETHOD(FindDocTypeByNodeId)      (THIS_ REFGUID rguid,
                                               IDMUSProdDocType **ppIDocType) PURE;
    STDMETHOD(GetFirstDocType)          (THIS_ IDMUSProdDocType **ppIFirstDocType) PURE;
    STDMETHOD(GetNextDocType)           (THIS_ IDMUSProdDocType *pIDocType,
                                               IDMUSProdDocType **ppINextDocType) PURE;
    STDMETHOD(CreateNewFile)            (THIS_ IDMUSProdDocType *pIDocType,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(OpenFile)                 (THIS_ IDMUSProdDocType *pIDocType,
                                               BSTR bstrTitle,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(CopyFile)                 (THIS_ IDMUSProdNode *pIDocRootNode,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(ShowFile)                 (THIS_ BSTR bstrFileName) PURE;
    STDMETHOD(RegisterClipFormatForFile)(THIS_ UINT uClipFormat,
                                               BSTR bstrExt) PURE;
    STDMETHOD(SaveClipFormat)           (THIS_ UINT uClipFormat,
                                               IDMUSProdNode *pINode,
                                               IStream **ppIStream) PURE;
    STDMETHOD(GetDocRootNodeFromData)   (THIS_ IDataObject *pIDataObject,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(AllocFileStream)          (THIS_ BSTR bstrFileName,
                                               DWORD dwDesiredAccess,
                                               FileType ftFileType,
                                               GUID guidDataFormat,
                                               IDMUSProdNode *pITargetFolderNode,
                                               IStream **ppIStream) PURE;
    STDMETHOD(AllocMemoryStream)        (THIS_ FileType ftFileType,
                                               GUID guidDataFormat,
                                               IStream **ppIStream) PURE;
    STDMETHOD(GetFirstProject)          (THIS_ IDMUSProdProject **ppIFirstProject) PURE;
    STDMETHOD(GetNextProject)           (THIS_ IDMUSProdProject *pIProject,
                                               IDMUSProdProject **ppINextProject) PURE;
    STDMETHOD(FindProject)              (THIS_ IDMUSProdNode *pINode,
                                               IDMUSProdProject **ppIProject) PURE;
    STDMETHOD(IsProjectEqual)           (THIS_ IDMUSProdNode *pIDocRootNode1,
                                               IDMUSProdNode *pIDocRootNode2) PURE;
    STDMETHOD(AddNodeImageList)         (THIS_ HANDLE hImageList,
                                               short *pnNbrFirstImage) PURE;
    STDMETHOD(GetNodeFileName)          (THIS_ IDMUSProdNode *pINode,
                                               BSTR *pbstrFileName) PURE;
    STDMETHOD(GetNodeFileGUID)          (THIS_ IDMUSProdNode *pINode,
                                               GUID *pguidFile) PURE;
    STDMETHOD(RevertFileToSaved)        (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(SaveNode)                 (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(SaveNodeAsNewFile)        (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(AddNode)                  (THIS_ IDMUSProdNode *pINode,
                                               IDMUSProdNode *pIParentNode) PURE;
    STDMETHOD(RemoveNode)               (THIS_ IDMUSProdNode *pINode,
                                               BOOL fPromptUser) PURE;
    STDMETHOD(FindDocRootNode)          (THIS_ GUID guidProject,
                                               IDMUSProdDocType *pIDocType,
                                               BSTR bstrNodeName,
                                               BSTR bstrNodeDescriptor,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(FindDocRootNodeByFileGUID)(THIS_ GUID guidFile,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(GetBestGuessDocRootNode)  (THIS_ IDMUSProdDocType *pIDocType,
                                               BSTR bstrNodeName,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(GetSelectedNode)          (THIS_ IDMUSProdNode **ppINode) PURE;
    STDMETHOD(SetSelectedNode)          (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(RefreshNode)              (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(SortChildNodes)           (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(EditNodeLabel)            (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(OpenEditor)               (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(CloseEditor)              (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(AddToNotifyList)          (THIS_ IDMUSProdNode *pIDocRootNode,
                                               IDMUSProdNode *pINotifyThisNode) PURE;
    STDMETHOD(RemoveFromNotifyList)     (THIS_ IDMUSProdNode *pIDocRootNode,
                                               IDMUSProdNode *pINotifyThisNode) PURE;
    STDMETHOD(NotifyNodes)              (THIS_ IDMUSProdNode *pIChangedDocRootNode,
                                               GUID guidUpdateType,
                                               void *pData) PURE;
    STDMETHOD(AddMenuItem)              (THIS_ IDMUSProdMenu *pIMenu) PURE;
    STDMETHOD(RemoveMenuItem)           (THIS_ IDMUSProdMenu *pIMenu) PURE;
    STDMETHOD(AddToolBar)               (THIS_ IDMUSProdToolBar *pIToolBar) PURE;
    STDMETHOD(RemoveToolBar)            (THIS_ IDMUSProdToolBar *pIToolBar) PURE;
    STDMETHOD(SetNbrStatusBarPanes)     (THIS_ int nCount,
                                               short nLifeSpan,
                                               HANDLE *phKey) PURE;
    STDMETHOD(SetStatusBarPaneInfo)     (THIS_ HANDLE hKey,
                                               int nIndex,
                                               StatusBarStyle sbStyle,
                                               int nMaxChars) PURE;
    STDMETHOD(SetStatusBarPaneText)     (THIS_ HANDLE hKey,
                                               int nIndex,
                                               BSTR bstrText,
                                               BOOL bUpdate) PURE;
    STDMETHOD(RestoreStatusBar)         (THIS_ HANDLE hKey) PURE;
    STDMETHOD(StartProgressBar)         (THIS_ int nLower,
                                               int nUpper,
                                               BSTR bstrPrompt,
                                               HANDLE *phKey) PURE;
    STDMETHOD(SetProgressBarPos)        (THIS_ HANDLE hKey,
                                               int nPos) PURE;
    STDMETHOD(SetProgressBarStep)       (THIS_ HANDLE hKey,
                                               UINT nValue) PURE;
    STDMETHOD(StepProgressBar)          (THIS_ HANDLE hKey) PURE;
    STDMETHOD(EndProgressBar)           (THIS_ HANDLE hKey) PURE;
    STDMETHOD(GetSharedObject)          (THIS_ REFCLSID rclsid,
                                               REFIID riid,
                                               void **ppvObject) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdFramework8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdFramework8
DECLARE_INTERFACE_(IDMUSProdFramework8, IDMUSProdFramework)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdFramework */
    STDMETHOD(FindComponent)            (THIS_ REFCLSID rclsid,
                                               IDMUSProdComponent **ppIComponent) PURE;
    STDMETHOD(GetFirstComponent)        (THIS_ IDMUSProdComponent **ppIFirstComponent) PURE;
    STDMETHOD(GetNextComponent)         (THIS_ IDMUSProdComponent *pIComponent,
                                               IDMUSProdComponent **ppINextComponent) PURE;
    STDMETHOD(AddDocType)               (THIS_ IDMUSProdDocType *pIDocType) PURE;
    STDMETHOD(FindDocTypeByExtension)   (THIS_ BSTR bstrExt,
                                               IDMUSProdDocType **ppIDocType) PURE;
    STDMETHOD(FindDocTypeByNodeId)      (THIS_ REFGUID rguid,
                                               IDMUSProdDocType **ppIDocType) PURE;
    STDMETHOD(GetFirstDocType)          (THIS_ IDMUSProdDocType **ppIFirstDocType) PURE;
    STDMETHOD(GetNextDocType)           (THIS_ IDMUSProdDocType *pIDocType,
                                               IDMUSProdDocType **ppINextDocType) PURE;
    STDMETHOD(CreateNewFile)            (THIS_ IDMUSProdDocType *pIDocType,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(OpenFile)                 (THIS_ IDMUSProdDocType *pIDocType,
                                               BSTR bstrTitle,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(CopyFile)                 (THIS_ IDMUSProdNode *pIDocRootNode,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(ShowFile)                 (THIS_ BSTR bstrFileName) PURE;
    STDMETHOD(RegisterClipFormatForFile)(THIS_ UINT uClipFormat,
                                               BSTR bstrExt) PURE;
    STDMETHOD(SaveClipFormat)           (THIS_ UINT uClipFormat,
                                               IDMUSProdNode *pINode,
                                               IStream **ppIStream) PURE;
    STDMETHOD(GetDocRootNodeFromData)   (THIS_ IDataObject *pIDataObject,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(AllocFileStream)          (THIS_ BSTR bstrFileName,
                                               DWORD dwDesiredAccess,
                                               FileType ftFileType,
                                               GUID guidDataFormat,
                                               IDMUSProdNode *pITargetFolderNode,
                                               IStream **ppIStream) PURE;
    STDMETHOD(AllocMemoryStream)        (THIS_ FileType ftFileType,
                                               GUID guidDataFormat,
                                               IStream **ppIStream) PURE;
    STDMETHOD(GetFirstProject)          (THIS_ IDMUSProdProject **ppIFirstProject) PURE;
    STDMETHOD(GetNextProject)           (THIS_ IDMUSProdProject *pIProject,
                                               IDMUSProdProject **ppINextProject) PURE;
    STDMETHOD(FindProject)              (THIS_ IDMUSProdNode *pINode,
                                               IDMUSProdProject **ppIProject) PURE;
    STDMETHOD(IsProjectEqual)           (THIS_ IDMUSProdNode *pIDocRootNode1,
                                               IDMUSProdNode *pIDocRootNode2) PURE;
    STDMETHOD(AddNodeImageList)         (THIS_ HANDLE hImageList,
                                               short *pnNbrFirstImage) PURE;
    STDMETHOD(GetNodeFileName)          (THIS_ IDMUSProdNode *pINode,
                                               BSTR *pbstrFileName) PURE;
    STDMETHOD(GetNodeFileGUID)          (THIS_ IDMUSProdNode *pINode,
                                               GUID *pguidFile) PURE;
    STDMETHOD(RevertFileToSaved)        (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(SaveNode)                 (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(SaveNodeAsNewFile)        (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(AddNode)                  (THIS_ IDMUSProdNode *pINode,
                                               IDMUSProdNode *pIParentNode) PURE;
    STDMETHOD(RemoveNode)               (THIS_ IDMUSProdNode *pINode,
                                               BOOL fPromptUser) PURE;
    STDMETHOD(FindDocRootNode)          (THIS_ GUID guidProject,
                                               IDMUSProdDocType *pIDocType,
                                               BSTR bstrNodeName,
                                               BSTR bstrNodeDescriptor,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(FindDocRootNodeByFileGUID)(THIS_ GUID guidFile,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(GetBestGuessDocRootNode)  (THIS_ IDMUSProdDocType *pIDocType,
                                               BSTR bstrNodeName,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(GetSelectedNode)          (THIS_ IDMUSProdNode **ppINode) PURE;
    STDMETHOD(SetSelectedNode)          (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(RefreshNode)              (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(SortChildNodes)           (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(EditNodeLabel)            (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(OpenEditor)               (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(CloseEditor)              (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(AddToNotifyList)          (THIS_ IDMUSProdNode *pIDocRootNode,
                                               IDMUSProdNode *pINotifyThisNode) PURE;
    STDMETHOD(RemoveFromNotifyList)     (THIS_ IDMUSProdNode *pIDocRootNode,
                                               IDMUSProdNode *pINotifyThisNode) PURE;
    STDMETHOD(NotifyNodes)              (THIS_ IDMUSProdNode *pIChangedDocRootNode,
                                               GUID guidUpdateType,
                                               void *pData) PURE;
    STDMETHOD(AddMenuItem)              (THIS_ IDMUSProdMenu *pIMenu) PURE;
    STDMETHOD(RemoveMenuItem)           (THIS_ IDMUSProdMenu *pIMenu) PURE;
    STDMETHOD(AddToolBar)               (THIS_ IDMUSProdToolBar *pIToolBar) PURE;
    STDMETHOD(RemoveToolBar)            (THIS_ IDMUSProdToolBar *pIToolBar) PURE;
    STDMETHOD(SetNbrStatusBarPanes)     (THIS_ int nCount,
                                               short nLifeSpan,
                                               HANDLE *phKey) PURE;
    STDMETHOD(SetStatusBarPaneInfo)     (THIS_ HANDLE hKey,
                                               int nIndex,
                                               StatusBarStyle sbStyle,
                                               int nMaxChars) PURE;
    STDMETHOD(SetStatusBarPaneText)     (THIS_ HANDLE hKey,
                                               int nIndex,
                                               BSTR bstrText,
                                               BOOL bUpdate) PURE;
    STDMETHOD(RestoreStatusBar)         (THIS_ HANDLE hKey) PURE;
    STDMETHOD(StartProgressBar)         (THIS_ int nLower,
                                               int nUpper,
                                               BSTR bstrPrompt,
                                               HANDLE *phKey) PURE;
    STDMETHOD(SetProgressBarPos)        (THIS_ HANDLE hKey,
                                               int nPos) PURE;
    STDMETHOD(SetProgressBarStep)       (THIS_ HANDLE hKey,
                                               UINT nValue) PURE;
    STDMETHOD(StepProgressBar)          (THIS_ HANDLE hKey) PURE;
    STDMETHOD(EndProgressBar)           (THIS_ HANDLE hKey) PURE;
    STDMETHOD(GetSharedObject)          (THIS_ REFCLSID rclsid,
                                               REFIID riid,
                                               void **ppvObject) PURE;

    /* IDMUSProdFramewor8 */
    STDMETHOD(GetNodeRuntimeFileName)   (THIS_ IDMUSProdNode* pINode,
                                               BSTR* pbstrRuntimeFileName) PURE;
    STDMETHOD(ResolveBestGuessWhenLoadFinished) (THIS_ IDMUSProdDocType* pIDocType,
                                               BSTR bstrNodeName,
                                               IDMUSProdNode *pITreePositionNode,
                                               IDMUSProdNotifySink* pINotifySink,
                                               GUID* pguidFile) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSetPersistInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSetPersistInfo
DECLARE_INTERFACE_(IDMUSProdSetPersistInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdSetPersistInfo */
    STDMETHOD(SetTargetDirectory)(THIS_ IDMUSProdNode *pITargetDirectoryNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNode */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNode
DECLARE_INTERFACE_(IDMUSProdNode, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdNode */
    STDMETHOD(GetNodeImageIndex)        (THIS_ short *pnNbrFirstImage) PURE;

    STDMETHOD(UseOpenCloseImages)       (THIS_ BOOL *pfUseOpenCloseImages) PURE;
    STDMETHOD(GetFirstChild)            (THIS_ IDMUSProdNode **ppIFirstChildNode) PURE;
    STDMETHOD(GetNextChild)             (THIS_ IDMUSProdNode *pIChildNode,
                                               IDMUSProdNode **ppINextChildNode) PURE;
    STDMETHOD(GetComponent)             (THIS_ IDMUSProdComponent **ppIComponent) PURE;
    STDMETHOD(GetDocRootNode)           (THIS_ IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(SetDocRootNode)           (THIS_ IDMUSProdNode *pIDocRootNode) PURE;
    STDMETHOD(GetParentNode)            (THIS_ IDMUSProdNode **ppIParentNode) PURE;
    STDMETHOD(SetParentNode)            (THIS_ IDMUSProdNode *pIParentNode) PURE;
    STDMETHOD(GetNodeId)                (THIS_ GUID *pguid) PURE;
    STDMETHOD(GetNodeName)              (THIS_ BSTR *pbstrName) PURE;
    STDMETHOD(GetNodeNameMaxLength)     (THIS_ short *pnMaxLength) PURE;
    STDMETHOD(ValidateNodeName)         (THIS_ BSTR bstrName) PURE;
    STDMETHOD(SetNodeName)              (THIS_ BSTR bstrName) PURE;
    STDMETHOD(GetNodeListInfo)          (THIS_ DMUSProdListInfo *pListInfo) PURE;
    STDMETHOD(GetEditorClsId)           (THIS_ CLSID *pclsidEditor) PURE;
    STDMETHOD(GetEditorTitle)           (THIS_ BSTR *pbstrName) PURE;
    STDMETHOD(GetEditorWindow)          (THIS_ HWND *hWndEditor) PURE;
    STDMETHOD(SetEditorWindow)          (THIS_ HWND hWndEditor) PURE;
    STDMETHOD(GetRightClickMenuId)      (THIS_ HINSTANCE *phInstance,
                                               UINT *pnResourceId) PURE;
    STDMETHOD(OnRightClickMenuInit)     (THIS_ HMENU hMenu) PURE;
    STDMETHOD(OnRightClickMenuSelect)   (THIS_ long lCommandId) PURE;
    STDMETHOD(DeleteChildNode)          (THIS_ IDMUSProdNode *pIChildNode,
                                               BOOL fPromptUser) PURE;
    STDMETHOD(InsertChildNode)          (THIS_ IDMUSProdNode *pIChildNode) PURE;
    STDMETHOD(DeleteNode)               (THIS_ BOOL fPromptUser) PURE;
    STDMETHOD(OnNodeSelChanged)         (THIS_ BOOL fSelected) PURE;
    STDMETHOD(CreateDataObject)         (THIS_ IDataObject **ppIDataObject) PURE;
    STDMETHOD(CanCut)                   (THIS) PURE;
    STDMETHOD(CanCopy)                  (THIS) PURE;
    STDMETHOD(CanDelete)                (THIS) PURE;
    STDMETHOD(CanDeleteChildNode)       (THIS_ IDMUSProdNode *pIChildNode) PURE;
    STDMETHOD(CanPasteFromData)         (THIS_ IDataObject *pIDataObject,
                                               BOOL *pfWillSetReference) PURE;
    STDMETHOD(PasteFromData)            (THIS_ IDataObject *pIDataObject) PURE;
    STDMETHOD(CanChildPasteFromData)    (THIS_ IDataObject *pIDataObject,
                                               IDMUSProdNode *pIChildNode,
                                               BOOL *pfWillSetReference) PURE;
    STDMETHOD(ChildPasteFromData)       (THIS_ IDataObject *pIDataObject,
                                               IDMUSProdNode *pIChildNode) PURE;
    STDMETHOD(GetObject)                (THIS_ REFCLSID rclsid,
                                               REFIID riid,
                                               void **ppvObject) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageManager */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPropPageManager
DECLARE_INTERFACE_(IDMUSProdPropPageManager, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdPropPageManager */
    STDMETHOD(GetPropertySheetTitle)    (THIS_ BSTR *pbstrTitle,
                                               BOOL *pfAddPropertiesText) PURE;
    STDMETHOD(GetPropertySheetPages)    (THIS_ IDMUSProdPropSheet *pIPropSheet,
                                               LONG *hPropSheetPage[  ],
                                               short *pnNbrPages) PURE;
    STDMETHOD(OnRemoveFromPropertySheet)(THIS) PURE;
    STDMETHOD(SetObject)                (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(RemoveObject)             (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(IsEqualObject)            (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(RefreshData)              (THIS) PURE;
    STDMETHOD(IsEqualPageManagerGUID)   (THIS_ REFGUID rguidPageManager) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPropPageObject
DECLARE_INTERFACE_(IDMUSProdPropPageObject, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdPropPageObject */
    STDMETHOD(GetData)                  (THIS_ void **ppData) PURE;
    STDMETHOD(SetData)                  (THIS_ void *pData) PURE;
    STDMETHOD(OnShowProperties)         (THIS) PURE;
    STDMETHOD(OnRemoveFromPageManager)  (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdProject */
#undef  INTERFACE
#define INTERFACE  IDMUSProdProject
DECLARE_INTERFACE_(IDMUSProdProject, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdProject */
    STDMETHOD(GetName)                  (THIS_ BSTR *pbstrName) PURE;
    STDMETHOD(GetGUID)                  (THIS_ GUID *pguid) PURE;
    STDMETHOD(GetFirstFileByDocType)    (THIS_ IDMUSProdDocType *pIDocType,
                                               IDMUSProdNode **ppIFirstFileNode) PURE;
    STDMETHOD(GetNextFileByDocType)     (THIS_ IDMUSProdNode *pIFileNode,
                                               IDMUSProdNode **ppINextFileNode) PURE;
    /* Following added for DirectX 8 */
    STDMETHOD(GetFirstFile)             (THIS_ IDMUSProdNode** ppIFirstFileNode ) PURE;
    STDMETHOD(GetNextFile)              (THIS_ IDMUSProdNode* pIFileNode,
                                               IDMUSProdNode** ppINextFileNode ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdComponent */
#undef  INTERFACE
#define INTERFACE  IDMUSProdComponent
DECLARE_INTERFACE_(IDMUSProdComponent, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdComponent */
    STDMETHOD(Initialize)           (THIS_ IDMUSProdFramework *pIFramework,
                                           BSTR *pbstrErrMsg) PURE;
    STDMETHOD(CleanUp)              (THIS) PURE;
    STDMETHOD(GetName)              (THIS_ BSTR *pbstrName) PURE;
    STDMETHOD(AllocReferenceNode)   (THIS_ GUID guidRefNodeId,
                                           IDMUSProdNode **ppIRefNode) PURE;
    STDMETHOD(OnActivateApp)        (THIS_ BOOL fActivate) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdRIFFExt */
#undef  INTERFACE
#define INTERFACE  IDMUSProdRIFFExt
DECLARE_INTERFACE_(IDMUSProdRIFFExt, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdRIFFExt */
    STDMETHOD(LoadRIFFChunk)    (THIS_ IStream *pIStream,
                                       IDMUSProdNode **ppINode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPersistInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPersistInfo
DECLARE_INTERFACE_(IDMUSProdPersistInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdPersistInfo */
    STDMETHOD(GetFileName)          (THIS_ BSTR *pbstrFileName) PURE;
    STDMETHOD(GetStreamInfo)        (THIS_ DMUSProdStreamInfo *pStreamInfo) PURE;
    /* Following added for DirectX 8 */
    STDMETHOD(IsInEmbeddedFileList) (THIS_ IDMUSProdNode* pIDocRootNode ) PURE;
    STDMETHOD(AddToEmbeddedFileList)(THIS_ IDMUSProdNode* pIDocRootNode ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdDocType */
#undef  INTERFACE
#define INTERFACE  IDMUSProdDocType
DECLARE_INTERFACE_(IDMUSProdDocType, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdDocType */
    STDMETHOD(GetResourceId) (THIS_ HINSTANCE *phInstance,
        UINT *pnResourceId) PURE;
    STDMETHOD(DoesExtensionMatch)   (THIS_ BSTR bstrExt) PURE;
    STDMETHOD(DoesIdMatch)          (THIS_ REFGUID rguid) PURE;
    STDMETHOD(AllocNode)            (THIS_ REFGUID rguid,
                                           IDMUSProdNode **ppINode) PURE;
    STDMETHOD(OnFileNew)            (THIS_ IDMUSProdProject *pITargetProject,
                                           IDMUSProdNode *pITargetDirectoryNode,
                                           IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(OnFileOpen)           (THIS_ IStream *pIStream,
                                           IDMUSProdProject *pITargetProject,
                                           IDMUSProdNode *pITargetDirectoryNode,
                                           IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(OnFileSave)           (THIS_ IStream *pIStream,
                                           IDMUSProdNode *pIDocRootNode) PURE;
    STDMETHOD(GetListInfo)          (THIS_ IStream *pIStream,
                                           DMUSProdListInfo *pListInfo) PURE;
    STDMETHOD(IsFileTypeExtension)  (THIS_ FileType ftFileType,
                                           BSTR bstrExt) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdDocType8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdDocType8
DECLARE_INTERFACE_(IDMUSProdDocType8, IDMUSProdDocType)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdDocType */
    STDMETHOD(GetResourceId)        (THIS_ HINSTANCE *phInstance,
                                           UINT *pnResourceId) PURE;
    STDMETHOD(DoesExtensionMatch)   (THIS_ BSTR bstrExt) PURE;
    STDMETHOD(DoesIdMatch)          (THIS_ REFGUID rguid) PURE;
    STDMETHOD(AllocNode)            (THIS_ REFGUID rguid,
                                           IDMUSProdNode **ppINode) PURE;
    STDMETHOD(OnFileNew)            (THIS_ IDMUSProdProject *pITargetProject,
                                           IDMUSProdNode *pITargetDirectoryNode,
                                           IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(OnFileOpen)           (THIS_ IStream *pIStream,
                                           IDMUSProdProject *pITargetProject,
                                           IDMUSProdNode *pITargetDirectoryNode,
                                           IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(OnFileSave)           (THIS_ IStream *pIStream,
                                           IDMUSProdNode *pIDocRootNode) PURE;
    STDMETHOD(GetListInfo)          (THIS_ IStream *pIStream,
                                           DMUSProdListInfo *pListInfo) PURE;
    STDMETHOD(IsFileTypeExtension)  (THIS_ FileType ftFileType,
                                           BSTR bstrExt) PURE;

    /* IDMUSProdDocType8 */
    STDMETHOD(GetObjectDescriptorFromNode)(THIS_ IDMUSProdNode* pIDocRootNode,
                                           void* pObjectDesc ) PURE;
    STDMETHOD(GetObjectRiffId)      (THIS_ GUID guidNodeId,
                                           DWORD* pckid,
                                           DWORD* pfccType ) PURE;
    STDMETHOD(GetObjectExt)         (THIS_ GUID guidNodeId,
                                           FileType ftFileType,
                                           BSTR* pbstrExt ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdEditor */
#undef  INTERFACE
#define INTERFACE  IDMUSProdEditor
DECLARE_INTERFACE_(IDMUSProdEditor, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)               (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)                (THIS) PURE;
    STDMETHOD_(ULONG,Release)               (THIS) PURE;

    /* IDMUSProdEditor */
    STDMETHOD(AttachObjects)                (THIS_ IDMUSProdNode *pINode) PURE;
    STDMETHOD(OnInitMenuFilePrint)          (THIS_ HMENU hMenu,
                                                   UINT nMenuID) PURE;
    STDMETHOD(OnInitMenuFilePrintPreview)   (THIS_ HMENU hMenu,
                                                   UINT nMenuID) PURE;
    STDMETHOD(OnFilePrint)                  (THIS) PURE;
    STDMETHOD(OnFilePrintPreview)           (THIS) PURE;
    STDMETHOD(OnViewProperties)             (THIS) PURE;
    STDMETHOD(OnF1Help)                     (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdReferenceNode */
#undef  INTERFACE
#define INTERFACE  IDMUSProdReferenceNode
DECLARE_INTERFACE_(IDMUSProdReferenceNode, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdReferenceNode */
    STDMETHOD(GetReferencedFile)    (THIS_ IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(SetReferencedFile)    (THIS_ IDMUSProdNode *pIDocRootNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSortNode */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSortNode
DECLARE_INTERFACE_(IDMUSProdSortNode, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdSortNode */
    STDMETHOD(CompareNodes)     (THIS_ IDMUSProdNode *pINode1,
                                       IDMUSProdNode *pINode2,
                                       int *pnResult) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdMenu */
#undef  INTERFACE
#define INTERFACE  IDMUSProdMenu
DECLARE_INTERFACE_(IDMUSProdMenu, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdMenu */
    STDMETHOD(GetMenuText)      (THIS_ BSTR *pbstrText) PURE;
    STDMETHOD(GetMenuHelpText)  (THIS_ BSTR *pbstrHelpText) PURE;
    STDMETHOD(OnMenuInit)       (THIS_ HMENU hMenu,
                                       UINT nMenuID) PURE;
    STDMETHOD(OnMenuSelect)     (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdToolBar */
#undef  INTERFACE
#define INTERFACE  IDMUSProdToolBar
DECLARE_INTERFACE_(IDMUSProdToolBar, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdToolBar */
    STDMETHOD(GetInfo)          (THIS_ HWND *phWndOwner,
                                       HINSTANCE *phInstance,
                                       UINT *pnResourceId,
                                       BSTR *pbstrTitle) PURE;
    STDMETHOD(GetMenuText)      (THIS_ BSTR *pbstrText) PURE;
    STDMETHOD(GetMenuHelpText)  (THIS_ BSTR *pbstrHelpText) PURE;
    STDMETHOD(Initialize)       (THIS_ HWND hWndToolBar) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdToolBar8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdToolBar8
DECLARE_INTERFACE_(IDMUSProdToolBar8, IDMUSProdToolBar)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdToolBar */
    STDMETHOD(GetInfo)          (THIS_ HWND *phWndOwner,
                                       HINSTANCE *phInstance,
                                       UINT *pnResourceId,
                                       BSTR *pbstrTitle) PURE;
    STDMETHOD(GetMenuText)      (THIS_ BSTR *pbstrText) PURE;
    STDMETHOD(GetMenuHelpText)  (THIS_ BSTR *pbstrHelpText) PURE;
    STDMETHOD(Initialize)       (THIS_ HWND hWndToolBar) PURE;

    /* IDMUSProdToolBar8 */
    STDMETHOD(ShowToolBar)      (THIS_ BOOL bShowToolBar) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPropSheet */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPropSheet
DECLARE_INTERFACE_(IDMUSProdPropSheet, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)               (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)                (THIS) PURE;
    STDMETHOD_(ULONG,Release)               (THIS) PURE;

    /* IDMUSProdPropSheet */
    STDMETHOD(SetPageManager)               (THIS_ IDMUSProdPropPageManager *pINewPageManager) PURE;
    STDMETHOD(RemovePageManager)            (THIS_ IDMUSProdPropPageManager *pIPageManager) PURE;
    STDMETHOD(RemovePageManagerByObject)    (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(RefreshTitleByObject)         (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(RefreshActivePageByObject)    (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(IsEqualPageManagerObject)     (THIS_ IDMUSProdPropPageObject *pIPropPageObject) PURE;
    STDMETHOD(RefreshTitle)                 (THIS) PURE;
    STDMETHOD(RefreshActivePage)            (THIS) PURE;
    STDMETHOD(GetActivePage)                (THIS_ short *pnIndex) PURE;
    STDMETHOD(SetActivePage)                (THIS_ short nIndex) PURE;
    STDMETHOD(Show)                         (THIS_ BOOL fShow) PURE;
    STDMETHOD(IsShowing)                    (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdFileRefChunk */
#undef  INTERFACE
#define INTERFACE  IDMUSProdFileRefChunk
DECLARE_INTERFACE_(IDMUSProdFileRefChunk, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdFileRefChunk */
    STDMETHOD(LoadRefChunk)     (THIS_ IStream *pIStream,
                                       IDMUSProdNode **ppIDocRootNode) PURE;
    STDMETHOD(SaveRefChunk)     (THIS_ IStream *pIStream,
                                       IDMUSProdNode *pIRefNode) PURE;
    STDMETHOD(GetChunkSize)     (THIS_ DWORD *pdwSize) PURE;

    /* Following added for DirectX 8 */
    STDMETHOD(ResolveWhenLoadFinished)(THIS_ IStream* pIStream,
                                       IDMUSProdNotifySink* pINotifySink,
                                       GUID* pguidFile ) PURE;
    STDMETHOD(GetFileRefInfo)   (THIS_ IStream* pIStream,
                                       DMUSProdFileRefInfo* pFileRefInfo ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdLoaderRefChunk */
#undef  INTERFACE
#define INTERFACE  IDMUSProdLoaderRefChunk
DECLARE_INTERFACE_(IDMUSProdLoaderRefChunk, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdLoaderRefChunk */
    STDMETHOD(SaveRefChunkForLoader)    (THIS_ IStream *pIStream,
                                               IDMUSProdNode *pIRefNode,
                                               REFCLSID rclsid,
                                               void *pObjectDesc,
                                               WhichLoader wlWhichLoader) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNodeDrop */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNodeDrop
DECLARE_INTERFACE_(IDMUSProdNodeDrop, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdNodeDrop */
    STDMETHOD(OnDropFiles)      (THIS_ HANDLE hDropInfo) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNotifySink */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNotifySink
DECLARE_INTERFACE_(IDMUSProdNotifySink, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdNotifySink */
    STDMETHOD(OnUpdate)         (THIS_ IDMUSProdNode *pIDocRootNode,
                                       GUID guidUpdateType,
                                       void *pData) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPChannelName */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPChannelName
DECLARE_INTERFACE_(IDMUSProdPChannelName, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdPChannelName */
    STDMETHOD(GetPChannelName)  (THIS_ DWORD dwPChannel, WCHAR *pwszName) PURE;
    STDMETHOD(SetPChannelName)  (THIS_ DWORD dwPChannel, WCHAR *pwszName) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimelineDataObject */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimelineDataObject
DECLARE_INTERFACE_(IDMUSProdTimelineDataObject, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdTimelineDataObject */
    STDMETHOD(AddInternalClipFormat)    (THIS_ UINT uClipFormat,
                                               IStream *pIStream) PURE;
    STDMETHOD(AddExternalClipFormat)    (THIS_ UINT uClipFormat,
                                               IStream *pIStream) PURE;
    STDMETHOD(IsClipFormatAvailable)    (THIS_ UINT uClipFormat) PURE;
    STDMETHOD(AttemptRead)              (THIS_ UINT uClipFormat,
                                               IStream **ppIStream) PURE;
    STDMETHOD(GetBoundaries)            (THIS_ long *plStartTime,
                                               long *plEndTime) PURE;
    STDMETHOD(SetBoundaries)            (THIS_ long lStartTime,
                                               long lEndTime) PURE;
    STDMETHOD(Import)                   (THIS_ IDataObject *pIDataObject) PURE;
    STDMETHOD(Export)                   (THIS_ IDataObject **ppIDataObject) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimeline */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimeline
DECLARE_INTERFACE_(IDMUSProdTimeline, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)           (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)            (THIS) PURE;
    STDMETHOD_(ULONG,Release)           (THIS) PURE;

    /* IDMUSProdTimeline */
    STDMETHOD(AddStripMgr)              (THIS_ IDMUSProdStripMgr *pIStripMgr,
                                               DWORD dwGroupBits) PURE;
    STDMETHOD(AddStrip)                 (THIS_ IDMUSProdStrip *pIStrip) PURE;
    STDMETHOD(SetMarkerTime)            (THIS_ MARKERID idMarkerType,
                                               TIMETYPE ttType,
                                               long lClocks) PURE;
    STDMETHOD(GetMarkerTime)            (THIS_ MARKERID idMarkerType,
                                               TIMETYPE ttType,
                                               long *plClocks) PURE;
    STDMETHOD(ClocksToPosition)         (THIS_ long lClocks,
                                               long *plPosition) PURE;
    STDMETHOD(PositionToClocks)         (THIS_ long lPosition,
                                               long *plClocks) PURE;
    STDMETHOD(DrawMusicLines)           (THIS_ HDC hdc,
                                               MUSICLINE_PROPERTY musicLineProperty,
                                               DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               LONG lXOffset) PURE;
    STDMETHOD(SetTimelineProperty)      (THIS_ TIMELINE_PROPERTY timelineProperty,
                                               VARIANT variant) PURE;
    STDMETHOD(GetTimelineProperty)      (THIS_ TIMELINE_PROPERTY timelineProperty,
                                               VARIANT *pVariant) PURE;
    STDMETHOD(Refresh)                  (THIS) PURE;
    STDMETHOD(ClocksToMeasureBeat)      (THIS_ DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               long lClocks,
                                               long *plMeasure,
                                               long *plBeat) PURE;
    STDMETHOD(PositionToMeasureBeat)    (THIS_ DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               long lPosition,
                                               long *plMeasure,
                                               long *plBeat) PURE;
    STDMETHOD(MeasureBeatToClocks)      (THIS_ DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               long lMeasure,
                                               long lBeat,
                                               long *plClocks) PURE;
    STDMETHOD(MeasureBeatToPosition)    (THIS_ DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               long lMeasure,
                                               long lBeat,
                                               long *plPosition) PURE;
    STDMETHOD(StripInvalidateRect)      (THIS_ IDMUSProdStrip *pIStrip,
                                               RECT *pRect,
                                               BOOL fErase) PURE;
    STDMETHOD(SetPropertyPage)          (THIS_ IUnknown *punkPropPageMgr,
                                               IUnknown *punkPropPageObj) PURE;
    STDMETHOD(RemovePropertyPageObject) (THIS_ IUnknown *punkPropPageObj) PURE;
    STDMETHOD(StripSetTimelineProperty) (THIS_ IDMUSProdStrip *pIStrip,
                                               STRIP_TIMELINE_PROPERTY stripTimelineProperty,
                                               VARIANT variant) PURE;
    STDMETHOD(OnDataChanged)            (THIS_ IUnknown *punkStripManager) PURE;
    STDMETHOD(TrackPopupMenu)           (THIS_ HMENU hMenu,
                                               long lXPos,
                                               long lYPos,
                                               IDMUSProdStrip *pIStrip,
                                               BOOL fEditMenu) PURE;
    STDMETHOD(ClocksToRefTime)          (THIS_ long lClocks,
                                               REFERENCE_TIME *prtRefTime) PURE;
    STDMETHOD(PositionToRefTime)        (THIS_ long lPosition,
                                               REFERENCE_TIME *prtRefTime) PURE;
    STDMETHOD(MeasureBeatToRefTime)     (THIS_ DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               long lMeasure,
                                               long lBeat,
                                               REFERENCE_TIME *prtRefTime) PURE;
    STDMETHOD(RefTimeToClocks)          (THIS_ REFERENCE_TIME rtRefTime,
                                               long *pTime) PURE;
    STDMETHOD(RefTimeToPosition)        (THIS_ REFERENCE_TIME rtRefTime,
                                               long *plPosition) PURE;
    STDMETHOD(RefTimeToMeasureBeat)     (THIS_ DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               REFERENCE_TIME rtRefTime,
                                               long *plMeasure,
                                               long *plBeat) PURE;
    STDMETHOD(ScreenToStripPosition)    (THIS_ IDMUSProdStrip *pIStrip,
                                               POINT *pPoint) PURE;
    STDMETHOD(StripGetTimelineProperty) (THIS_ IDMUSProdStrip *pIStrip,
                                               STRIP_TIMELINE_PROPERTY stripTimelineProperty,
                                               VARIANT *pVariant) PURE;
    STDMETHOD(RemoveStripMgr)           (THIS_ IDMUSProdStripMgr *pIStripMgr) PURE;
    STDMETHOD(RemoveStrip)              (THIS_ IDMUSProdStrip *pIStrip) PURE;
    STDMETHOD(GetParam)                 (THIS_ REFGUID rguidType,
                                               DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               MUSIC_TIME mtTime,
                                               MUSIC_TIME *pmtNext,
                                               void *pData) PURE;
    STDMETHOD(SetParam)                 (THIS_ REFGUID rguidType,
                                               DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               MUSIC_TIME mtTime,
                                               void *pData) PURE;
    STDMETHOD(GetStripMgr)              (THIS_ REFGUID rguidType,
                                               DWORD dwGroupBits,
                                               DWORD dwIndex,
                                               IDMUSProdStripMgr **ppIStripMgr) PURE;
    STDMETHOD(InsertStripAtDefaultPos)  (THIS_ IDMUSProdStrip *pIStrip,
                                               REFCLSID clsidType,
                                               DWORD dwGroupBits,
                                               DWORD dwIndex) PURE;
    STDMETHOD(EnumStrip)                (THIS_ DWORD dwEnum,
                                               IDMUSProdStrip **ppIStrip) PURE;
    STDMETHOD(InsertStripAtPos)         (THIS_ IDMUSProdStrip *pIStrip,
                                               DWORD dwPosition) PURE;
    STDMETHOD(StripToWindowPos)         (THIS_ IDMUSProdStrip *pIStrip,
                                               POINT *pPoint) PURE;
    STDMETHOD(AddToNotifyList)          (THIS_ IDMUSProdStripMgr *pIStripMgr,
                                               REFGUID rguidType,
                                               DWORD dwGroupBits) PURE;
    STDMETHOD(RemoveFromNotifyList)     (THIS_ IDMUSProdStripMgr *pIStripMgr,
                                               REFGUID rguidType,
                                               DWORD dwGroupBits) PURE;
    STDMETHOD(NotifyStripMgrs)          (THIS_ REFGUID rguidType,
                                               DWORD dwGroupBits,
                                               void *pData) PURE;
    STDMETHOD(AllocTimelineDataObject)  (THIS_ IDMUSProdTimelineDataObject **ppITimelineDataObject) PURE;
    STDMETHOD(GetPasteType)             (THIS_ TIMELINE_PASTE_TYPE *ptlptPasteType) PURE;
    STDMETHOD(SetPasteType)             (THIS_ TIMELINE_PASTE_TYPE tlptPasteType) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimelineCallback */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimelineCallback
DECLARE_INTERFACE_(IDMUSProdTimelineCallback, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdTimelineCallback */
    STDMETHOD(OnDataChanged)    (THIS_ IUnknown *punkStripMgr) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimelineEdit */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimelineEdit
DECLARE_INTERFACE_(IDMUSProdTimelineEdit, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdTimelineEdit */
    STDMETHOD(Cut)              (THIS_ IDMUSProdTimelineDataObject *pIDataObject) PURE;
    STDMETHOD(Copy)             (THIS_ IDMUSProdTimelineDataObject *pIDataObject) PURE;
    STDMETHOD(Paste)            (THIS_ IDMUSProdTimelineDataObject *pIDataObject) PURE;
    STDMETHOD(Insert)           (THIS) PURE;
    STDMETHOD(Delete)           (THIS) PURE;
    STDMETHOD(SelectAll)        (THIS) PURE;
    STDMETHOD(CanCut)           (THIS) PURE;
    STDMETHOD(CanCopy)          (THIS) PURE;
    STDMETHOD(CanPaste)         (THIS_ IDMUSProdTimelineDataObject *pIDataObject) PURE;
    STDMETHOD(CanInsert)        (THIS) PURE;
    STDMETHOD(CanDelete)        (THIS) PURE;
    STDMETHOD(CanSelectAll)     (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdDLSNotify8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdDLSNotify8
DECLARE_INTERFACE_(IDMUSProdDLSNotify8, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdDLSNotify8 */
    STDMETHOD(OnDownloadCustomDLS)(THIS_ BOOL fDownloadCustomDLS ) PURE;
    STDMETHOD(OnDownloadGM)     (THIS_ BOOL fDownloadGM ) PURE;
};
#define IDMUSProdDLSNotify IDMUSProdDLSNotify8

/*////////////////////////////////////////////////////////////////////
// IDMUSProdAudioPathInUse */
#undef  INTERFACE
#define INTERFACE  IDMUSProdAudioPathInUse
DECLARE_INTERFACE_(IDMUSProdAudioPathInUse, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdAudioPathInUse */
    STDMETHOD(UsingAudioPath)	(THIS_ IUnknown *punkAudioPath,
									   BOOL fActive) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdDMOInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdDMOInfo
DECLARE_INTERFACE_(IDMUSProdDMOInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdDMOInfo */
    STDMETHOD(EnumDMOInfo)		(THIS_ DWORD dwIndex,
									   DMUSProdDMOInfo *pDMOInfo) PURE;
};

/* CLSIDs */

DEFINE_GUID(CLSID_SegmentDesigner, 0xdfce860e, 0xa6fa, 0x11d1, 0x88, 0x81, 0x00, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(CLSID_SegmentComponent, 0xdfce860b, 0xa6fa, 0x11d1, 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(CLSID_CConductor, 0x36f6dde8, 0x46ce, 0x11d0, 0xb9, 0xdb, 0x0, 0xaa, 0x0, 0xc0, 0x81, 0x46);
DEFINE_GUID(CLSID_TimelineCtl, 0xdb838a7c, 0xb4f5, 0x11d0, 0xa9, 0x7f, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(CLSID_AudioPathComponent, 0x4adc2ad, 0x7ea5, 0x4260, 0xa4, 0x5b, 0x75, 0xa6, 0xef, 0x85, 0x6e, 0x99);


/* GUIDs */

DEFINE_GUID(GUID_SegmentNode, 0xdfce8609, 0xa6fa, 0x11d1, 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_SegmentRefNode, 0xdfce860a, 0xa6fa, 0x11d1, 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_ConductorCountInBeatOffset, 0xb413282, 0xdc09, 0x11d2, 0xb0, 0xf1, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_ChordIndividualChordSharpsFlats, 0x7013c793, 0xfca, 0x11d3, 0xbc, 0xb5, 0x0, 0xc0, 0x4f, 0xa3, 0x72, 0x6e);
DEFINE_GUID(GUID_ChordKey, 0x6f590ffb, 0xeb69, 0x11d1, 0x88, 0xc9, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_ChordSharpsFlats, 0x6f590ffc, 0xeb69, 0x11d1, 0x88, 0xc9, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_Segment_AllTracksAdded, 0x96a0a26c, 0xf4e7, 0x11d1, 0x88, 0xcb, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_DocRootNode, 0xf9a03440, 0x38f3, 0x11d2, 0x89, 0xb5, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_Segment_Undo_BSTR, 0x178633a6, 0x4452, 0x11d2, 0x89, 0xc, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_Segment_Length_Change, 0xa2aca78b, 0x4461, 0x11d2, 0xbc, 0x6d, 0x0, 0xc0, 0x4f, 0xa3, 0x72, 0x6e);
DEFINE_GUID(GUID_Segment_CreateTrack, 0xb53892d4, 0x63b4, 0x11d2, 0x89, 0x18, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_Segment_DeletedTrack, 0x1c3840d2, 0xe39, 0x11d3, 0xa7, 0xc, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_DisplayContextMenu, 0xcf0c97fa, 0x679f, 0x11d2, 0xb0, 0x64, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_RecordButton, 0x11cfe612, 0xa757, 0x11d2, 0xb0, 0xd2, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_WindowActive, 0x18d7f8f0, 0xa757, 0x11d2, 0xb0, 0xd2, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_Start, 0x71754743, 0xa98d, 0x11d2, 0xb0, 0xd3, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_Stop, 0x71754744, 0xa98d, 0x11d2, 0xb0, 0xd3, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_Set_Tempo, 0x1528eab8, 0xc518, 0x11d2, 0xb0, 0xe7, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_Segment_NewStyleSelected, 0x7cfd1ee0, 0xcbef, 0x11d2, 0x85, 0x45, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(GUID_Segment_FrameworkMsg, 0x15cc2460, 0xdd85, 0x11d2, 0xb4, 0x3e, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(GUID_Segment_BandTrackChange, 0x9b3f0be0, 0xedff, 0x11d2, 0xb4, 0x3f, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(GUID_TimelineShowTimeSig, 0xf811ce10, 0x42a1, 0x11d2, 0x89, 0xb, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(GUID_TimelineShowRealTime, 0x9f879f8a, 0xfda3, 0x11d2, 0xa6, 0xf8, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_TimelineSetSegStartTime, 0x3e2c8b0, 0xc2f4, 0x11d2, 0xb0, 0xe7, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(GUID_TimelineSetCursor, 0xcb715b92, 0xc549, 0x11d2, 0xb0, 0xe7, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(FRAMEWORK_FileDeleted, 0xd6e0ada0, 0x30ce, 0x11d1, 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(FRAMEWORK_FileReplaced, 0xd6e0ada1, 0x30ce, 0x11d1, 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(FRAMEWORK_FileClosed, 0x8743aec0, 0x3338, 0x11d1, 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(FRAMEWORK_FileNameChange, 0xdd581b01, 0x5463, 0x11d2, 0x89, 0xb7, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(DOCROOT_GuidChange, 0x592f8420, 0x643b, 0x11d2, 0x89, 0xb7, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_DirectMusicObject, 0x102125e0, 0x98b7, 0x11d1, 0x89, 0xaf, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(GUID_CurrentVersion, 0x5cbdd400, 0x35cc, 0x11d1, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(GUID_CurrentVersion_OnlyUI, 0x0bf78e00, 0x4484, 0x11d1, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(GUID_Bookmark, 0xed259580, 0xb1ea, 0x11d2, 0x85, 0x3a, 0x00, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(GUID_AllZeros, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    /* Following added for DirectX 8 */
DEFINE_GUID(GUID_Segment_Name_Change, 0xb7c2f208, 0xb965, 0x4a93, 0x99, 0x57, 0x66, 0x54, 0xa3, 0xf7, 0xf2, 0x8f);
DEFINE_GUID(GUID_Segment_ExtensionLength, 0xb8d915e6, 0x5c3a, 0x42e2, 0xaf, 0xde, 0x1c, 0xf2, 0xa7, 0x85, 0xb0, 0x55);
DEFINE_GUID(GUID_Segment_PickupLength, 0x6839c902, 0x70b6, 0x44f7, 0xb6, 0x5a, 0x85, 0xbc, 0xb7, 0x73, 0x37, 0xe0);
DEFINE_GUID(GUID_Segment_WindowClosing, 0xec8a00a0, 0x5724, 0x43cc, 0x88, 0xe6, 0x54, 0x86, 0x3f, 0x4d, 0x9, 0x4d);
DEFINE_GUID(GUID_TimelineSetSnapTo, 0x465b3b9b, 0x17fa, 0x4a3a, 0x86, 0xfe, 0xe8, 0xa2, 0xf, 0x44, 0xdc, 0x8f);
DEFINE_GUID(GUID_TimelineUserSetCursor, 0x3d4423bc, 0x9ad, 0x4bd2, 0xa8, 0xa1, 0x79, 0x9d, 0x34, 0xc3, 0x4d, 0xad);
DEFINE_GUID(GUID_Conductor_OutputPortsChanged, 0x67a4f713, 0xabcc, 0x4a96, 0xbe, 0xbe, 0x58, 0x76, 0xac, 0x22, 0xec, 0xc5);
DEFINE_GUID(GUID_Conductor_OutputPortsRemoved, 0x71ebee10, 0x225f, 0x4f10, 0xae, 0x2, 0xdf, 0x47, 0x6c, 0xa3, 0xc1, 0xa7);
DEFINE_GUID(GUID_ConductorUnloadWaves, 0x95295d93, 0x7ff3, 0x4048, 0x82, 0xa, 0x90, 0xad, 0x9e, 0x28, 0x4, 0x43);
DEFINE_GUID(GUID_ConductorDownloadWaves, 0x599531bb, 0x9c5b, 0x410f, 0xbb, 0x37, 0x81, 0x4a, 0x2f, 0x11, 0x9d, 0xa3);
DEFINE_GUID(GUID_ConductorFlushWaves, 0x60715ab9, 0x43af, 0x4708, 0x92, 0xee, 0xa1, 0x31, 0x34, 0x3b, 0x47, 0xfc);
DEFINE_GUID(FRAMEWORK_BeforeFileNameChange, 0xdd5bd15, 0xa0f0, 0x4520, 0xb6, 0x2d, 0xa7, 0x77, 0xd4, 0xd9, 0x53, 0x8c);
DEFINE_GUID(FRAMEWORK_AbortFileNameChange, 0x9788ecad, 0x71cd, 0x4195, 0xb8, 0xea, 0x6b, 0xe1, 0x57, 0x52, 0xeb, 0xb8);
DEFINE_GUID(FRAMEWORK_BeforeFileSave, 0x9289551e, 0x6af7, 0x49ad, 0xbc, 0xc1, 0x88, 0xc5, 0xd4, 0x5a, 0x19, 0xa);
DEFINE_GUID(FRAMEWORK_AfterFileSave, 0x61d31e68, 0xb39e, 0x4e60, 0xa7, 0xd9, 0xd7, 0xad, 0xee, 0xfd, 0x34, 0x93);
DEFINE_GUID(FRAMEWORK_AfterFileOpen, 0x6a8655c4, 0xec48, 0x4d7a, 0x88, 0xdf, 0xb3, 0xe5, 0x71, 0x51, 0xfe, 0xe4);
DEFINE_GUID(FRAMEWORK_FileLoadFinished, 0x607a3140, 0xd964, 0x42a3, 0xa1, 0xd5, 0x6a, 0x48, 0x6d, 0x74, 0x9f, 0x76);
DEFINE_GUID(DOCROOT_SyncDirectMusicObject, 0x6a91adc5, 0x2a11, 0x4dda, 0xaa, 0x37, 0x84, 0x93, 0xe, 0xd9, 0xe8, 0xcb);
DEFINE_GUID(GUID_AudioPathNode, 0x2a2620e2, 0x2622, 0x4c12, 0xaa, 0x77, 0xb3, 0xf3, 0x43, 0xd3, 0x3a, 0xa0);
DEFINE_GUID(GUID_AudioPathRefNode, 0xa9ecf224, 0x7863, 0x4f41, 0xaa, 0xc1, 0x22, 0x1a, 0x85, 0x6e, 0xee, 0x22);
DEFINE_GUID(AUDIOPATH_NameChange, 0xc662f3ad, 0x423a, 0x417a, 0xb8, 0xcc, 0x68, 0xa7, 0x46, 0x82, 0x84, 0x8e);
DEFINE_GUID(AUDIOPATH_DirtyNotification, 0x6d720e15, 0x3546, 0x44c3, 0xae, 0xe8, 0x95, 0x6b, 0xcd, 0xc3, 0x36, 0xc1);
DEFINE_GUID(AUDIOPATH_NeedToRebuildNotification, 0x87ead57a, 0x1418, 0x4427, 0x96, 0xba, 0x2b, 0x60, 0x36, 0xde, 0x5e, 0x15);
DEFINE_GUID(GUID_WaveParam, 0x817082fb, 0x4180, 0x4298, 0x94, 0xd2, 0xb1, 0x8e, 0x69, 0xca, 0x52, 0xa7);
DEFINE_GUID(GUID_WaveParam2, 0x9f1f28b, 0xa922, 0x4999, 0x9f, 0x38, 0x5f, 0x3c, 0x7c, 0xbf, 0x1c, 0x8b);
	/* Following added for DirectX 8a */
DEFINE_GUID(GUID_BandInterfaceForPChannel, 0x6e73b997, 0xb12d, 0x4fcd, 0x85, 0x96, 0x68, 0x24, 0xa5, 0x77, 0xef, 0x76);

/* IIDs */

DEFINE_GUID(IID_IDMUSProdSegmentEdit, 0x18250220, 0xcfe0, 0x11d2, 0x85, 0x46, 0x00, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(IID_IDMUSProdSegmentEdit8,0xd5ccd1da,0xf8ae,0x4bc4,0x9e,0x6c,0x34,0xd0,0x8b,0x3a,0xdd,0x3c);
DEFINE_GUID(IID_IDMUSProdMidiInCPt, 0xaa349de0, 0xef0e, 0x11d0, 0xad, 0x22, 0x0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac);
DEFINE_GUID(IID_IDMUSProdConductorTempo, 0x32f40a16, 0x5f8f, 0x41a0, 0xb6, 0xbe, 0x51, 0x25, 0x7d, 0x1f, 0xe6, 0x88);
DEFINE_GUID(IID_IDMUSProdPortNotify, 0x9bd50920, 0x3c3c, 0x11d2, 0x8a, 0xa2, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTransport, 0x8c7b3a62, 0xd33d, 0x11d2, 0xb0, 0xf0, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);
DEFINE_GUID(IID_IDMUSProdSecondaryTransport, 0x52d006fa, 0x2281, 0x11d2, 0x88, 0xf7, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdNotifyCPt, 0xfa273400, 0x9f23, 0x11d1, 0x88, 0x77, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdConductor, 0xe8a27a9, 0x473e, 0x4f48, 0x82, 0x21, 0x57, 0x5b, 0xe7, 0x5, 0x95, 0x65);
DEFINE_GUID(IID_IDMUSProdConductor8, 0x207c586f, 0x848a, 0x445f, 0x84, 0xa4, 0xc4, 0xa1, 0x63, 0xb7, 0xda, 0xa1);
#ifdef __cplusplus
struct __declspec(uuid("{0E8A27A9-473E-4f48-8221-575BE7059565}")) IDMUSProdConductor;
struct __declspec(uuid("{207C586F-848A-445f-84A4-C4A163B7DAA1}")) IDMUSProdConductor8;
struct __declspec(uuid("{9BD50920-3C3C-11d2-8AA2-00C04FBF8D15}")) IDMUSProdPortNotify;
struct __declspec(uuid("{FA273400-9F23-11d1-8877-00C04FBF8D15}")) IDMUSProdNotifyCPt;
#endif
DEFINE_GUID(IID_IDMUSProdDLSNotify8, 0x9f936199, 0x4316, 0x4eae, 0x80, 0xcb, 0x94, 0x45, 0x40, 0x81, 0x74, 0x0d);
#define IID_IDMUSProdDLSNotify IID_IDMUSProdDLSNotify8
DEFINE_GUID(IID_IDMUSProdStrip, 0x893ee17a, 0x4a3, 0x11d3, 0x89, 0x4c, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdStripFunctionBar, 0x86d596cc, 0xb302, 0x11d1, 0x88, 0x8f, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdStripMgr, 0x893ee17b, 0x04a3, 0x11d3, 0x89, 0x4c, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
#ifdef __cplusplus
struct __declspec(uuid("{86D596CC-B302-11d1-888F-00C04FBF8D15}")) IDMUSProdStripFunctionBar;
struct __declspec(uuid("{893EE17A-04A3-11d3-894C-00C04FBF8D15}")) IDMUSProdStrip;
struct __declspec(uuid("{893EE17B-04A3-11d3-894C-00C04FBF8D15}")) IDMUSProdStripMgr;
#endif
DEFINE_GUID(IID_IDMUSProdFramework, 0x3b8d0e01, 0x46b1, 0x11d0, 0x89, 0xAC, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdFramework8, 0x91188780, 0x8cbe, 0x11d3, 0xb4, 0x73, 0x00, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(IID_IDMUSProdNode, 0xda821fc1, 0x4cef, 0x11d0, 0x89, 0xAC, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdSetPersistInfo,0xd22b1160,0x3915,0x11d2,0x89,0xb5,0x00,0xc0,0x4f,0xd9,0x12,0xc8);
DEFINE_GUID(IID_IDMUSProdPropPageManager, 0x3095f6e1, 0xc160, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdPropPageObject, 0x3095f6e2, 0xc160, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
#ifdef __cplusplus
struct __declspec(uuid("{3095F6E2-C160-11d0-89AE-00A0C9054129}")) IDMUSProdPropPageObject;
struct __declspec(uuid("{9F3ED901-46B7-11d0-89AC-00A0C9054129}")) IDMUSProdComponent;
struct __declspec(uuid("{DA821FC1-4CEF-11D0-89AC-00A0C9054129}")) IDMUSProdNode;
struct __declspec(uuid("{28275880-2E9F-11d1-89AE-00A0C9054129}")) IDMUSProdNotifySink;
struct __declspec(uuid("{44D1A761-C5FE-11d0-89AE-00A0C9054129}")) IDMUSProdToolBar;
struct __declspec(uuid("{B3776E9C-97FB-41fa-B352-F332E5C3B0D3}")) IDMUSProdToolBar8;
#endif
DEFINE_GUID(IID_IDMUSProdProject, 0xa03aa040, 0xe63b, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdComponent, 0x9f3ed901, 0x46b7, 0x11d0, 0x89, 0xac, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdRIFFExt, 0xd913ac41, 0x8411, 0x11d0, 0x89, 0xac, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdPersistInfo, 0xa8ae1161, 0x99fd, 0x11d0, 0x89, 0xac, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdDocType, 0x5c9a32a1, 0x4c6d, 0x11d0, 0x89, 0xAC, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdDocType8, 0x165a8ee0, 0x8190, 0x11d3, 0xb4, 0x73, 0x00, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(IID_IDMUSProdEditor, 0xee3402a1, 0x5405, 0x11d0, 0x89, 0xac, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdReferenceNode, 0xc483efc0, 0xec89, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdSortNode, 0x5662f480, 0x65d9, 0x11d2, 0x89, 0xb7, 0x00, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(IID_IDMUSProdMenu, 0xf5d7ce01, 0x4cef, 0x11d0, 0x89, 0xAC, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdToolBar, 0x44d1a761, 0xc5fe, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdToolBar8, 0xb3776e9c, 0x97fb, 0x41fa, 0xb3, 0x52, 0xf3, 0x32, 0xe5, 0xc3, 0xb0, 0xd3);
DEFINE_GUID(IID_IDMUSProdPropSheet, 0x3095f6e0, 0xc160, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdFileRefChunk, 0x7b0fc840, 0xe66d, 0x11d0, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdLoaderRefChunk, 0xa6403b00, 0xb1d6, 0x11d1, 0x89, 0xaf, 0x00, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(IID_IDMUSProdNodeDrop, 0x24990b00, 0xc287, 0x11d2, 0x85, 0x45, 0x00, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(IID_IDMUSProdNotifySink, 0x28275880, 0x2e9f, 0x11d1, 0x89, 0xae, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29);
DEFINE_GUID(IID_IDMUSProdPChannelName, 0x69509a6b, 0x1ff8, 0x11d2, 0x88, 0xf4, 0x00, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTimelineDataObject, 0xd7d8a772, 0x3171, 0x11d2, 0x89, 0x0, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTimeline, 0x22b5869d, 0x523e, 0x11d2, 0x89, 0x13, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTimelineCallback, 0x8fe7e6d5, 0xf331, 0x11d0, 0xbc, 0x9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(IID_IDMUSProdTimelineEdit, 0x8640f4b2, 0x2b01, 0x11d2, 0x88, 0xf9, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
#ifdef __cplusplus
struct __declspec(uuid("{22B5869D-523E-11d2-8913-00C04FBF8D15}")) IDMUSProdTimeline;
struct __declspec(uuid("{8640F4B2-2B01-11d2-88F9-00C04FBF8D15}")) IDMUSProdTimelineEdit;
#endif

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __DMUSPROD_H__1FD3B972_F3E7_11D0_89AE_00A0C9054129__INCLUDED_ */
