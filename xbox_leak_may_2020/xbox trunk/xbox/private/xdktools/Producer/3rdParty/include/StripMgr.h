/************************************************************************
*                                                                       *
*   StripMgr.h -- This module contains the API for the                  *
*                 Strip Manager interfaces                              *
*                                                                       *
*   Copyright (c) 1998-1999, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef __StripMgr_h__
#define __StripMgr_h__

#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>

#ifdef __cplusplus
extern "C"{
#endif 

typedef long            MUSIC_TIME;

/* Forward Declarations */ 
interface IDMUSProdStrip;
interface IDMUSProdStripFunctionBar;
interface IDMUSProdStripMgr;
#ifndef __cplusplus 
typedef interface IDMUSProdStrip IDMUSProdStrip;
typedef interface IDMUSProdStripFunctionBar IDMUSProdStripFunctionBar;
typedef interface IDMUSProdStripMgr IDMUSProdStripMgr;
#endif /* __cplusplus */

typedef enum enumSTRIPVIEW
{
	SV_NORMAL				 = 0,
	SV_MINIMIZED			 = SV_NORMAL + 1,
	SV_FUNCTIONBAR_NORMAL	 = SV_MINIMIZED + 1,
	SV_FUNCTIONBAR_MINIMIZED = SV_FUNCTIONBAR_NORMAL + 1
} STRIPVIEW;

typedef enum enumSTRIPPROPERTY
{
	SP_NAME						 = 0,
	SP_GUTTERSELECTABLE			 = SP_NAME + 1,
	SP_GUTTERSELECT				 = SP_GUTTERSELECTABLE + 1,
	SP_BEGINSELECT				 = SP_GUTTERSELECT + 1,
	SP_ENDSELECT				 = SP_BEGINSELECT + 1,
	SP_CURSOR_HANDLE			 = SP_ENDSELECT + 1,
	SP_DEFAULTHEIGHT			 = SP_CURSOR_HANDLE + 1,
	SP_RESIZEABLE				 = SP_DEFAULTHEIGHT + 1,
	SP_MAXHEIGHT				 = SP_RESIZEABLE + 1,
	SP_MINHEIGHT				 = SP_MAXHEIGHT + 1,
	SP_MINMAXABLE				 = SP_MINHEIGHT + 1,
	SP_MINIMIZE_HEIGHT			 = SP_MINMAXABLE + 1,
	SP_STRIPMGR					 = SP_MINIMIZE_HEIGHT + 1,
	SP_FUNCTIONBAR_EXCLUDE_WIDTH = SP_STRIPMGR + 1,
	SP_USER						 = 1024
} STRIPPROPERTY;

typedef enum enumSTRIPMGRPROPERTY
{
	SMP_ITIMELINECTL		= 0,
	SMP_IDIRECTMUSICTRACK	= SMP_ITIMELINECTL + 1,
	SMP_IDMUSPRODFRAMEWORK	= SMP_IDIRECTMUSICTRACK + 1,
	SMP_DMUSIOTRACKHEADER	= SMP_IDMUSPRODFRAMEWORK + 1,
	SMP_USER				= 1024
} STRIPMGRPROPERTY;

/*////////////////////////////////////////////////////////////////////
// IDMUSProdStrip */
#undef  INTERFACE
#define INTERFACE  IDMUSProdStrip
DECLARE_INTERFACE_(IDMUSProdStrip, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdStrip */
    STDMETHOD(Draw)				(THIS_ HDC hDC,
                                       STRIPVIEW stripView,
                                       LONG lXOffset) PURE;
    STDMETHOD(GetStripProperty) (THIS_ STRIPPROPERTY stripProperty,
                                       VARIANT __RPC_FAR *pVariant) PURE;
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
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdStripFunctionBar */
    STDMETHOD(FBDraw)			(THIS_ HDC hDC,
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
    STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG,Release)		(THIS) PURE;

    /* IDMUSProdStripMgr */
    STDMETHOD(IsParamSupported)		(THIS_ REFGUID rguidType) PURE;
    STDMETHOD(GetParam)				(THIS_ REFGUID rguidType,
										   MUSIC_TIME mtTime,
										   MUSIC_TIME __RPC_FAR *pmtNext,
										   void __RPC_FAR *pData) PURE;
    STDMETHOD(SetParam)				(THIS_ REFGUID rguidType,
										   MUSIC_TIME mtTime,
										   void __RPC_FAR *pData) PURE;
    STDMETHOD(OnUpdate)				(THIS_ REFGUID rguidType,
										   DWORD dwGroupBits,
										   void __RPC_FAR *pData) PURE;
    STDMETHOD(GetStripMgrProperty)	(THIS_ STRIPMGRPROPERTY stripMgrProperty,
										   VARIANT __RPC_FAR *pVariant) PURE;
    STDMETHOD(SetStripMgrProperty)  (THIS_ STRIPMGRPROPERTY stripMgrProperty,
										   VARIANT variant) PURE;
};

/* IID's */
DEFINE_GUID(IID_IDMUSProdStrip, 0x893ee17a, 0x4a3, 0x11d3, 0x89, 0x4c, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdStripFunctionBar, 0x86d596cc, 0xb302, 0x11d1, 0x88, 0x8f, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(IID_IDMUSProdStripMgr, 0x893ee17b, 0x04a3, 0x11d3, 0x89, 0x4c, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
struct __declspec(uuid("{893EE17A-04A3-11d3-894C-00C04FBF8D15}")) IDMUSProdStrip;
struct __declspec(uuid("{86D596CC-B302-11d1-888F-00C04FBF8D15}")) IDMUSProdStripFunctionBar;
struct __declspec(uuid("{893EE17B-04A3-11d3-894C-00C04FBF8D15}")) IDMUSProdStripMgr;

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __StripMgr_h__ */
