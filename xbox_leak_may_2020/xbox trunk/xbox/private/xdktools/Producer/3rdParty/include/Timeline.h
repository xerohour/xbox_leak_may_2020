/************************************************************************
*                                                                       *
*   Timeline.h -- This module contains the API for the                  *
*                 Timeline interfaces                                   *
*                                                                       *
*   Copyright (c) 1998-1999, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef __Timeline_h__
#define __Timeline_h__

#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>

#include "stripmgr.h"

#ifdef __cplusplus
extern "C"{
#endif 

typedef __int64         REFERENCE_TIME;
typedef long            MUSIC_TIME;

/* Forward Declarations */ 
interface IDMUSProdTimelineDataObject;
interface IDMUSProdTimeline;
interface IDMUSProdTimelineCallback;
interface IDMUSProdTimelineEdit;
#ifndef __cplusplus 
typedef interface IDMUSProdTimelineDataObject IDMUSProdTimelineDataObject;
typedef interface IDMUSProdTimeline IDMUSProdTimeline;
typedef interface IDMUSProdTimelineCallback IDMUSProdTimelineCallback;
typedef interface IDMUSProdTimelineEdit IDMUSProdTimelineEdit;
#endif /* __cplusplus */

typedef enum enumTIMETYPE
{
	TIMETYPE_CLOCKS	= 0,
	TIMETYPE_MS		= TIMETYPE_CLOCKS + 1
} TIMETYPE;

typedef enum enumTIMELINE_PROPERTY
{
	TP_CLOCKLENGTH		 = 1,
	TP_STRIPMOUSECAPTURE = TP_CLOCKLENGTH + 1,
	TP_DMUSPRODFRAMEWORK = TP_STRIPMOUSECAPTURE + 1,
	TP_TIMELINECALLBACK  = TP_DMUSPRODFRAMEWORK + 1,
	TP_ACTIVESTRIP		 = TP_TIMELINECALLBACK + 1,
	TP_FUNCTIONBAR_WIDTH = TP_ACTIVESTRIP + 1,
	TP_MAXIMUM_HEIGHT	 = TP_FUNCTIONBAR_WIDTH + 1,
	TP_ZOOM				 = TP_MAXIMUM_HEIGHT + 1,
	TP_HORIZONTAL_SCROLL = TP_ZOOM + 1,
	TP_VERTICAL_SCROLL	 = TP_HORIZONTAL_SCROLL + 1,
	TP_SNAPAMOUNT		 = TP_VERTICAL_SCROLL + 1,
	TP_FREEZE_UNDO		 = TP_SNAPAMOUNT + 1,
	TP_SNAP_TO			 = TP_FREEZE_UNDO + 1
} TIMELINE_PROPERTY;

typedef enum enumDMUSPROD_TIMELINE_SNAP_TO
{
	DMUSPROD_TIMELINE_SNAP_NONE	= 0,
	DMUSPROD_TIMELINE_SNAP_GRID	= 1,
	DMUSPROD_TIMELINE_SNAP_BEAT	= 2,
	DMUSPROD_TIMELINE_SNAP_BAR	= 3
} DMUSPROD_TIMELINE_SNAP_TO;

typedef enum enumSTRIP_TIMELINE_PROPERTY
{
	STP_VERTICAL_SCROLL	  = 0,
	STP_GET_HDC			  = STP_VERTICAL_SCROLL + 1,
	STP_HEIGHT			  = STP_GET_HDC + 1,
	STP_STRIPVIEW		  = STP_HEIGHT + 1,
	STP_STRIP_RECT		  = STP_STRIPVIEW + 1,
	STP_FBAR_RECT		  = STP_STRIP_RECT + 1,
	STP_POSITION		  = STP_FBAR_RECT + 1,
	STP_GUTTER_SELECTED	  = STP_POSITION + 1,
	STP_ENTIRE_STRIP_RECT = STP_GUTTER_SELECTED + 1
} STRIP_TIMELINE_PROPERTY;

typedef enum enumMARKERID
{
	MARKER_CURRENTTIME	= 0,
	MARKER_BEGINSELECT	= MARKER_CURRENTTIME + 1,
	MARKER_ENDSELECT	= MARKER_BEGINSELECT + 1,
	MARKER_LEFTDISPLAY	= MARKER_ENDSELECT + 1
} MARKERID;

typedef enum enumMUSICLINE_PROPERTY
{
	ML_DRAW_MEASURE_BEAT_GRID = 0,
	ML_DRAW_MEASURE_BEAT	  = ML_DRAW_MEASURE_BEAT_GRID + 1
} MUSICLINE_PROPERTY;

typedef enum enumTIMELINE_PASTE_TYPE
{
	TL_PASTE_MERGE	   = 0,
	TL_PASTE_OVERWRITE = TL_PASTE_MERGE + 1
} TIMELINE_PASTE_TYPE;

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimelineDataObject */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimelineDataObject
DECLARE_INTERFACE_(IDMUSProdTimelineDataObject, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdTimelineDataObject */
    STDMETHOD(AddInternalClipFormat)	(THIS_ UINT uClipFormat,
											   IStream __RPC_FAR *pIStream) PURE;
    STDMETHOD(AddExternalClipFormat)	(THIS_ UINT uClipFormat,
											   IStream __RPC_FAR *pIStream) PURE;
    STDMETHOD(IsClipFormatAvailable)	(THIS_ UINT uClipFormat) PURE;
    STDMETHOD(AttemptRead)				(THIS_ UINT uClipFormat,
											   IStream __RPC_FAR *__RPC_FAR *ppIStream) PURE;
    STDMETHOD(GetBoundaries)			(THIS_ long __RPC_FAR *plStartTime,
											   long __RPC_FAR *plEndTime) PURE;
    STDMETHOD(SetBoundaries)			(THIS_ long lStartTime,
											   long lEndTime) PURE;
    STDMETHOD(Import)					(THIS_ IDataObject __RPC_FAR *pIDataObject) PURE;
    STDMETHOD(Export)					(THIS_ IDataObject __RPC_FAR *__RPC_FAR *ppIDataObject) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimeline */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimeline
DECLARE_INTERFACE_(IDMUSProdTimeline, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdTimeline */
    STDMETHOD(AddStripMgr)				(THIS_ IDMUSProdStripMgr __RPC_FAR *pIStripMgr,
											   DWORD dwGroupBits) PURE;
    STDMETHOD(AddStrip)					(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip) PURE;
    STDMETHOD(SetMarkerTime)			(THIS_ MARKERID idMarkerType,
											   TIMETYPE ttType,
											   long lClocks) PURE;
    STDMETHOD(GetMarkerTime)			(THIS_ MARKERID idMarkerType,
											   TIMETYPE ttType,
											   long __RPC_FAR *plClocks) PURE;
    STDMETHOD(ClocksToPosition)			(THIS_ long lClocks,
											   long __RPC_FAR *plPosition) PURE;
    STDMETHOD(PositionToClocks)			(THIS_ long lPosition,
											   long __RPC_FAR *plClocks) PURE;
    STDMETHOD(DrawMusicLines)			(THIS_ HDC hdc,
											   MUSICLINE_PROPERTY musicLineProperty,
											   DWORD dwGroupBits,
											   DWORD dwIndex,
											   LONG lXOffset) PURE;
    STDMETHOD(SetTimelineProperty)		(THIS_ TIMELINE_PROPERTY timelineProperty,
											   VARIANT variant) PURE;
    STDMETHOD(GetTimelineProperty)		(THIS_ TIMELINE_PROPERTY timelineProperty,
											   VARIANT __RPC_FAR *pVariant) PURE;
    STDMETHOD(Refresh)					(THIS) PURE;
    STDMETHOD(ClocksToMeasureBeat)		(THIS_ DWORD dwGroupBits,
											   DWORD dwIndex,
											   long lClocks,
											   long __RPC_FAR *plMeasure,
											   long __RPC_FAR *plBeat) PURE;
    STDMETHOD(PositionToMeasureBeat)	(THIS_ DWORD dwGroupBits,
											   DWORD dwIndex,
											   long lPosition,
											   long __RPC_FAR *plMeasure,
											   long __RPC_FAR *plBeat) PURE;
    STDMETHOD(MeasureBeatToClocks)		(THIS_ DWORD dwGroupBits,
											   DWORD dwIndex,
											   long lMeasure,
											   long lBeat,
											   long __RPC_FAR *plClocks) PURE;
    STDMETHOD(MeasureBeatToPosition)	(THIS_ DWORD dwGroupBits,
											   DWORD dwIndex,
											   long lMeasure,
											   long lBeat,
											   long __RPC_FAR *plPosition) PURE;
    STDMETHOD(StripInvalidateRect)		(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   RECT __RPC_FAR *pRect,
											   BOOL fErase) PURE;
    STDMETHOD(SetPropertyPage)			(THIS_ IUnknown __RPC_FAR *punkPropPageMgr,
											   IUnknown __RPC_FAR *punkPropPageObj) PURE;
    STDMETHOD(RemovePropertyPageObject)	(THIS_ IUnknown __RPC_FAR *punkPropPageObj) PURE;
    STDMETHOD(StripSetTimelineProperty)	(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   STRIP_TIMELINE_PROPERTY stripTimelineProperty,
											   VARIANT variant) PURE;
    STDMETHOD(OnDataChanged)			(THIS_ IUnknown __RPC_FAR *punkStripManager) PURE;
    STDMETHOD(TrackPopupMenu)			(THIS_ HMENU hMenu,
											   long lXPos,
											   long lYPos,
											   IDMUSProdStrip __RPC_FAR *pIStrip,
											   BOOL fEditMenu) PURE;
    STDMETHOD(ClocksToRefTime)			(THIS_ long lClocks,
											   REFERENCE_TIME __RPC_FAR *prtRefTime) PURE;
    STDMETHOD(PositionToRefTime)		(THIS_ long lPosition,
											   REFERENCE_TIME __RPC_FAR *prtRefTime) PURE;
    STDMETHOD(MeasureBeatToRefTime)		(THIS_ DWORD dwGroupBits,
											   DWORD dwIndex,
											   long lMeasure,
											   long lBeat,
											   REFERENCE_TIME __RPC_FAR *prtRefTime) PURE;
    STDMETHOD(RefTimeToClocks)			(THIS_ REFERENCE_TIME rtRefTime,
											   long __RPC_FAR *pTime) PURE;
    STDMETHOD(RefTimeToPosition)		(THIS_ REFERENCE_TIME rtRefTime,
											   long __RPC_FAR *plPosition) PURE;
    STDMETHOD(RefTimeToMeasureBeat)		(THIS_ DWORD dwGroupBits,
											   DWORD dwIndex,
											   REFERENCE_TIME rtRefTime,
											   long __RPC_FAR *plMeasure,
											   long __RPC_FAR *plBeat) PURE;
    STDMETHOD(ScreenToStripPosition)	(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   POINT __RPC_FAR *pPoint) PURE;
    STDMETHOD(StripGetTimelineProperty)	(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   STRIP_TIMELINE_PROPERTY stripTimelineProperty,
											   VARIANT __RPC_FAR *pVariant) PURE;
    STDMETHOD(RemoveStripMgr)			(THIS_ IDMUSProdStripMgr __RPC_FAR *pIStripMgr) PURE;
    STDMETHOD(RemoveStrip)				(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip) PURE;
    STDMETHOD(GetParam)					(THIS_ REFGUID rguidType,
											   DWORD dwGroupBits,
											   DWORD dwIndex,
											   MUSIC_TIME mtTime,
											   MUSIC_TIME __RPC_FAR *pmtNext,
											   void __RPC_FAR *pData) PURE;
    STDMETHOD(SetParam)					(THIS_ REFGUID rguidType,
											   DWORD dwGroupBits,
											   DWORD dwIndex,
											   MUSIC_TIME mtTime,
											   void __RPC_FAR *pData) PURE;
    STDMETHOD(GetStripMgr)				(THIS_ REFGUID rguidType,
											   DWORD dwGroupBits,
											   DWORD dwIndex,
											   IDMUSProdStripMgr __RPC_FAR *__RPC_FAR *ppIStripMgr) PURE;
    STDMETHOD(InsertStripAtDefaultPos)	(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   REFCLSID clsidType,
											   DWORD dwGroupBits,
											   DWORD dwIndex) PURE;
    STDMETHOD(EnumStrip)				(THIS_ DWORD dwEnum,
											   IDMUSProdStrip __RPC_FAR *__RPC_FAR *ppIStrip) PURE;
    STDMETHOD(InsertStripAtPos)			(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   DWORD dwPosition) PURE;
    STDMETHOD(StripToWindowPos)			(THIS_ IDMUSProdStrip __RPC_FAR *pIStrip,
											   POINT __RPC_FAR *pPoint) PURE;
    STDMETHOD(AddToNotifyList)			(THIS_ IDMUSProdStripMgr __RPC_FAR *pIStripMgr,
											   REFGUID rguidType,
											   DWORD dwGroupBits) PURE;
    STDMETHOD(RemoveFromNotifyList)		(THIS_ IDMUSProdStripMgr __RPC_FAR *pIStripMgr,
											   REFGUID rguidType,
											   DWORD dwGroupBits) PURE;
    STDMETHOD(NotifyStripMgrs)			(THIS_ REFGUID rguidType,
											   DWORD dwGroupBits,
											   void __RPC_FAR *pData) PURE;
    STDMETHOD(AllocTimelineDataObject)	(THIS_ IDMUSProdTimelineDataObject __RPC_FAR *__RPC_FAR *ppITimelineDataObject) PURE;
    STDMETHOD(GetPasteType)				(THIS_ TIMELINE_PASTE_TYPE __RPC_FAR *ptlptPasteType) PURE;
    STDMETHOD(SetPasteType)				(THIS_ TIMELINE_PASTE_TYPE tlptPasteType) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimelineCallback */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimelineCallback
DECLARE_INTERFACE_(IDMUSProdTimelineCallback, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdTimelineCallback */
    STDMETHOD(OnDataChanged)	(THIS_ IUnknown __RPC_FAR *punkStripMgr) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTimelineEdit */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTimelineEdit
DECLARE_INTERFACE_(IDMUSProdTimelineEdit, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdTimelineEdit */
    STDMETHOD(Cut)				(THIS_ IDMUSProdTimelineDataObject __RPC_FAR *pIDataObject) PURE;
    STDMETHOD(Copy)				(THIS_ IDMUSProdTimelineDataObject __RPC_FAR *pIDataObject) PURE;
    STDMETHOD(Paste)			(THIS_ IDMUSProdTimelineDataObject __RPC_FAR *pIDataObject) PURE;
    STDMETHOD(Insert)			(THIS) PURE;
    STDMETHOD(Delete)			(THIS) PURE;
    STDMETHOD(SelectAll)		(THIS) PURE;
    STDMETHOD(CanCut)			(THIS) PURE;
    STDMETHOD(CanCopy)			(THIS) PURE;
    STDMETHOD(CanPaste)			(THIS_ IDMUSProdTimelineDataObject __RPC_FAR *pIDataObject) PURE;
    STDMETHOD(CanInsert)		(THIS) PURE;
    STDMETHOD(CanDelete)		(THIS) PURE;
    STDMETHOD(CanSelectAll)		(THIS) PURE;
};


/* CLSID's */
DEFINE_GUID(CLSID_TimelineCtl, 0xDB838A7C, 0xB4F5, 0x11d0, 0xa9, 0x7f, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);

/* IID's */
DEFINE_GUID(IID_IDMUSProdTimelineDataObject, 0xD7D8A772, 0x3171, 0x11d2, 0x89, 0x0, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTimeline, 0x22B5869D, 0x523E, 0x11d2, 0x89, 0x13, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdTimelineCallback, 0x8FE7E6D5, 0xF331, 0x11d0, 0xbc, 0x9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(IID_IDMUSProdTimelineEdit, 0x8640F4B2, 0x2B01, 0x11d2, 0x88, 0xf9, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __Timeline_h__ */
