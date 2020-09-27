/************************************************************************
*                                                                       *
*   SegmentDesigner.h -- This module contains the API for the           *
*                        Segment Designer interfaces                    *
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef __SegmentDesigner_h__
#define __SegmentDesigner_h__

#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 
interface IDMUSProdSegmentEdit;
#ifndef __cplusplus 
typedef interface IDMUSProdSegmentEdit IDMUSProdSegmentEdit;
#endif /* __cplusplus */

typedef struct _DMUSProdFrameworkMsg
{
    GUID guidUpdateType;
    IUnknown __RPC_FAR *punkIDMUSProdNode;
    void __RPC_FAR *pData;
} DMUSProdFrameworkMsg;

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSegmentEdit */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSegmentEdit
DECLARE_INTERFACE_(IDMUSProdSegmentEdit, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG,Release)		(THIS) PURE;

    /* IDMUSProdSegmentEdit */
    STDMETHOD(AddStrip)				(THIS_ CLSID clsidTrackType,
										   DWORD dwGroupBits,
										   IUnknown __RPC_FAR *__RPC_FAR *ppIStripMgr) PURE;
    STDMETHOD(ContainsTempoStrip)	(THIS_ BOOL __RPC_FAR *pfTempoStripExists) PURE;
};

/* CLSID's */
DEFINE_GUID(CLSID_SegmentDesigner,0xDFCE860E,0xA6FA,0x11d1,0x88,0x81,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(CLSID_SegmentComponent, 0xdfce860b, 0xa6fa, 0x11d1, 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);

/* GUID's */
DEFINE_GUID(GUID_SegmentNode, 0xdfce8609, 0xa6fa, 0x11d1, 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);

/* IID's */
DEFINE_GUID(IID_IDMUSProdSegmentEdit,0x18250220,0xCFE0,0x11d2,0x85,0x46,0x00,0x10,0x5A,0x27,0x96,0xDE);

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __SegmentDesigner_h__ */
