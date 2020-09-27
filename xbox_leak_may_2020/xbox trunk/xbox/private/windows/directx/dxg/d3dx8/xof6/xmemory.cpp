//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xmemory.cpp
//
//  Description:    Implement memory alloc/free functions.
//
//  History:
//      03/02/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

#if !defined(DBG)

    #undef DPF_MODNAME
    #define DPF_MODNAME "XMalloc"
    
    HRESULT XMalloc(void** ppvOut, size_t size)
    {
        if (!(*ppvOut = MemAlloc(size))) {
            DPF_ERR("XMalloc failed");
            return DXFILEERR_BADALLOC;
        }
    
        return DXFILE_OK;
    }
    
    #undef DPF_MODNAME
    #define DPF_MODNAME "XRealloc"
    
    HRESULT XRealloc(void** ppvInOut, size_t size)
    {
        void *p = *ppvInOut;
    
        if (p == NULL)
            return XMalloc(ppvInOut, size);
    
        if (!(*ppvInOut = MemReAlloc(p, size))) {
            DPF_ERR("XRealloc failed");
            return DXFILEERR_BADALLOC;
        }
    
        return DXFILE_OK;
    }
    
    #undef DPF_MODNAME
    #define DPF_MODNAME "XFree"
    
    void XFree(void *p)
    {
        MemFree(p);
    }

#else
    
    #define ALLOCATED_STAMP 0xfedcba98
    #define FREED_STAMP     0xf00df00d
    
    struct MemorySignature {
        ULONG ulSize;             
        ULONG ulSignature2;
        ULONG ulSignature3;
        ULONG ulSignature4;
    };
    
    #undef DPF_MODNAME
    #define DPF_MODNAME "XMalloc"
    
    HRESULT XMalloc(void** ppvOut, size_t size)
    {
        void* p = NULL;
        MemorySignature* pHeader = (MemorySignature*) MemAlloc(size + 2*sizeof(MemorySignature));
    
        if (pHeader)
        {
            p = (VOID*) (pHeader + 1);
    
            // The trailer signature may very well be unaligned, but do I look
            // like I care?
            
            MemorySignature* pTrailer = (MemorySignature*) ((BYTE*) p + size);
    
            pHeader->ulSize = size;
            pHeader->ulSignature2 = ALLOCATED_STAMP;
            pHeader->ulSignature3 = ALLOCATED_STAMP;
            pHeader->ulSignature4 = ALLOCATED_STAMP;
    
            pTrailer->ulSize = size;
            pTrailer->ulSignature2 = ALLOCATED_STAMP;
            pTrailer->ulSignature3 = ALLOCATED_STAMP;
            pTrailer->ulSignature4 = ALLOCATED_STAMP;
        }
    
        *ppvOut = p;
    
        return (p != NULL) ? DXFILE_OK : DXFILEERR_BADALLOC;
    }
    
    #undef DPF_MODNAME
    #define DPF_MODNAME "XRealloc"
    
    HRESULT XRealloc(void** ppvInOut, size_t size)
    {
        void *pOld = *ppvInOut;
        void *pNew;
    
        if (pOld == NULL)
            return XMalloc(ppvInOut, size);
    
        if (SUCCEEDED(XMalloc(&pNew, size)))
        {
            memcpy(pNew, pOld, ((MemorySignature *)pOld - 1)->ulSize);

            XFree(pOld);
            *ppvInOut = pNew;
            return DXFILE_OK;
        }
    
        DPF_ERR("XRealloc failed");
        return DXFILEERR_BADALLOC;
    }
    
    #undef DPF_MODNAME
    #define DPF_MODNAME "XFree"
    
    void XFree(void *p)
    {
        if (p != NULL)
        {
            MemorySignature* pHeader = ((MemorySignature*) p) - 1;
            ASSERT(pHeader->ulSignature2 == ALLOCATED_STAMP);
            ASSERT(pHeader->ulSignature3 == ALLOCATED_STAMP);
            ASSERT(pHeader->ulSignature4 == ALLOCATED_STAMP);
    
            MemorySignature* pTrailer = (MemorySignature*) ((BYTE*) p + pHeader->ulSize);
            ASSERT(pHeader->ulSize == pTrailer->ulSize);
            ASSERT(pTrailer->ulSignature2 == ALLOCATED_STAMP);
            ASSERT(pTrailer->ulSignature3 == ALLOCATED_STAMP);
            ASSERT(pTrailer->ulSignature4 == ALLOCATED_STAMP);
    
            pHeader->ulSize = 0;
            pHeader->ulSignature2 = FREED_STAMP;
            pHeader->ulSignature3 = FREED_STAMP;
            pHeader->ulSignature4 = FREED_STAMP;
    
            pTrailer->ulSize = 0;
            pTrailer->ulSignature2 = FREED_STAMP;
            pTrailer->ulSignature3 = FREED_STAMP;
            pTrailer->ulSignature4 = FREED_STAMP;
        
            MemFree(pHeader);
        }
    }

#endif
