/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	MusicHeapTrack.cpp

Abstract:

	User-defined DirectMusic heap object 

Author:

	Dan Haffner(danhaff) 05-Jul-2001

Environment:

	Xbox only

Revision History:

	05-Jul-2001 danhaff
		Initial Version

--*/

#pragma once

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "xtl.h"
#include "dmusici.h"


#undef INTERFACE
#define INTERFACE  IMusicHeap
DECLARE_INTERFACE_(IMusicHeap, IDirectMusicHeap)
{
    /*  IUnknown */
    STDMETHOD(QueryInterface)               (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)                (THIS) PURE;
    STDMETHOD_(ULONG,Release)               (THIS) PURE;

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* pData) PURE;
    STDMETHOD(Free)       (THIS_ PVOID pData) PURE;
    STDMETHOD(GetSize)    (THIS_ PVOID pData, LPDWORD pcbSize) PURE;

    /* IDirectMusicHeap Info */
    STDMETHOD(GetHeapAllocation          )(THIS_ DWORD *pdwHeapAllocation) PURE;
    STDMETHOD(GetHeapAllocationPeak      )(THIS_ DWORD *pdwHeapAllocationPeak) PURE;
    STDMETHOD(GetHeapAllocationBlocks    )(THIS_ DWORD *pdwHeapAllocationBlocks) PURE;
    STDMETHOD(GetHeapAllocationBlocksPeak)(THIS_ DWORD *pdwHeapAllocationBlocksPeak) PURE;
    STDMETHOD(SetSize                    )(THIS_ DWORD dwLimit) PURE;

};                                  


    HRESULT CreateMusicHeapTrack(IMusicHeap** ppHeap, bool bPhysical);
    HRESULT CreateMusicHeapFixed(IMusicHeap** ppHeap, bool bPhysical, DWORD dwSize);
    HRESULT CreateMusicHeapFixedCache(IMusicHeap** ppHeap, bool bPhysical, DWORD dwSize);
