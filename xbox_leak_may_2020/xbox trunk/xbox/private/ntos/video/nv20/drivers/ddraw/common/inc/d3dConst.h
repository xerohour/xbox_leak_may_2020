/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dConst.h                                                        *
*     General constants associate with D3D                                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)   22June99     created                *
*                                                                           *
\***************************************************************************/

#ifndef __D3D_CONST_
#define __D3D_CONST_

//---------------------------------------------------------------------------

// global constants

// these are missing from the MS's DX8 header files

#if (DIRECT3D_VERSION == 0x0800)

#define _FACD3D  0x876
#define MAKE_D3DHRESULT( code )  MAKE_HRESULT( 1, _FACD3D, code )

#define D3DERR_TEXTURE_CREATE_FAILED    MAKE_DDHRESULT(721)
#define D3DTSS_ADDRESS 12
#define D3DERR_WRONGTEXTUREFORMAT               MAKE_D3DHRESULT(2072)
#define D3DERR_UNSUPPORTEDCOLOROPERATION        MAKE_D3DHRESULT(2073)
#define D3DERR_UNSUPPORTEDCOLORARG              MAKE_D3DHRESULT(2074)
#define D3DERR_UNSUPPORTEDALPHAOPERATION        MAKE_D3DHRESULT(2075)
#define D3DERR_UNSUPPORTEDALPHAARG              MAKE_D3DHRESULT(2076)
#define D3DERR_TOOMANYOPERATIONS                MAKE_D3DHRESULT(2077)
#define D3DERR_CONFLICTINGTEXTUREFILTER         MAKE_D3DHRESULT(2078)
#define D3DERR_UNSUPPORTEDFACTORVALUE           MAKE_D3DHRESULT(2079)
#define D3DERR_CONFLICTINGRENDERSTATE           MAKE_D3DHRESULT(2081)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER         MAKE_D3DHRESULT(2082)
#define D3DERR_CONFLICTINGTEXTUREPALETTE        MAKE_D3DHRESULT(2086)
#define D3DERR_DRIVERINTERNALERROR              MAKE_D3DHRESULT(2087)

#define D3DERR_NOTFOUND                         MAKE_D3DHRESULT(2150)
#define D3DERR_MOREDATA                         MAKE_D3DHRESULT(2151)
#define D3DERR_DEVICELOST                       MAKE_D3DHRESULT(2152)
#define D3DERR_DEVICENOTRESET                   MAKE_D3DHRESULT(2153)
#define D3DERR_NOTAVAILABLE                     MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY                 MAKE_D3DHRESULT(380)
#define D3DERR_INVALIDDEVICE                    MAKE_D3DHRESULT(2155)
#define D3DERR_INVALIDCALL                      MAKE_D3DHRESULT(2156)
// end missing MS info

#endif // (DIRECT3D_VERSION == 0x0800)

// Look in D3DTYPES.H to determine what these values are.
#define D3D_RENDERSTATE_MAX     D3DRS_BLENDOP
#define D3D_DP2OP_MAX           D3DDP2OP_MULTIPLYTRANSFORM
#define D3D_TEXSTAGESTATE_MAX   D3DTSS_RESULTARG
#define D3D_TBLEND_MAX          D3DTBLEND_ADD

//---------------------------------------------------------------------------

// global helpers

// placement new and delete
inline void* operator new    (size_t nSize,void *pPlace) { return pPlace; }
//inline void  operator delete (void*,void*)               {}

//---------------------------------------------------------------------------

// global macros

#define OFFSETOF(s,m)           ((DWORD)(&(((s*)0)->m)))

// retrieve or set the pointer to the NvObject associated with a particular DDSLcl
#ifdef WINNT
#define GET_PNVOBJ(pDDSLcl)        ((CNvObject *)((pDDSLcl)->dwReserved1))
#define SET_PNVOBJ(pDDSLcl,value)  (pDDSLcl)->dwReserved1 = (DWORD)(value);
#else
// under win9x, we store NvObj pointer in GBL_MORE rather than DDSLcl b/c this field gets swapped on a flip
#define GET_PNVOBJ(pDDSLcl)        ((CNvObject *)((GET_LPDDRAWSURFACE_GBL_MORE((pDDSLcl)->lpGbl))->dwDriverReserved))
#define SET_PNVOBJ(pDDSLcl,value)  (GET_LPDDRAWSURFACE_GBL_MORE((pDDSLcl)->lpGbl))->dwDriverReserved = (DWORD)(value);
#endif

#define IS_VALID_PNVOBJ(value)     (((value) != NULL) && ((((DWORD)(value)) & MASK_FLOATING_CONTEXT_DMA_ID) != FLOATING_CONTEXT_DMA_ID))

// template classes for general purpose use
typedef void* NVPOSITION;

//---------------------------------------------------------------------------

// CNVLinkedList

template<class T,class R> class CNVLinkedList
{
protected:
    class CEntry
    {
    public:
        T       m_Object;
        CEntry *m_pNext;
        CEntry *m_pPrev;
    };

    CEntry *m_pHead;
    CEntry *m_pTail;

public:
    inline NVPOSITION getFirstPosition (void) const { return (NVPOSITION)m_pHead; }
    inline NVPOSITION getLastPosition  (void) const { return (NVPOSITION)m_pTail; }

    inline R getNextPosition (NVPOSITION& pos)
    {
        CEntry *pEntry = (CEntry*)pos;
        if (pEntry)
        {
            pos = pEntry->m_pNext;
            return pEntry->m_Object;
        }
        return NULL;
    }

    inline R getPreviousPosition (NVPOSITION& pos)
    {
        CEntry *pEntry = (CEntry*)pos;
        if (pEntry)
        {
            pos = pEntry->m_pPrev;
            return pEntry->m_Object;
        }
        return NULL;
    }

    inline int addFront (R obj)
    {
        CEntry *pEntry = new CEntry;
        if (!pEntry) return 0;
        pEntry->m_Object = obj;
        pEntry->m_pPrev  = NULL;
        pEntry->m_pNext  = m_pHead;
        if (m_pHead) m_pHead->m_pPrev = pEntry;
        m_pHead = pEntry;
        if (!m_pTail) m_pTail = pEntry;
        return 1;
    }

    inline int addBack (R obj)
    {
        CEntry *pEntry = new CEntry;
        if (!pEntry) return 0;
        pEntry->m_Object = obj;
        pEntry->m_pPrev  = m_pTail;
        pEntry->m_pNext  = NULL;
        if (m_pHead) m_pTail->m_pNext = pEntry;
        m_pTail = pEntry;
        if (!m_pHead) m_pHead = pEntry;
        return 1;
    }

    inline void remove (NVPOSITION pos)
    {
        CEntry *pEntry = (CEntry*)pos;
        if (pEntry->m_pNext) pEntry->m_pNext->m_pPrev = pEntry->m_pPrev;
                        else m_pTail = pEntry->m_pPrev;
        if (pEntry->m_pPrev) pEntry->m_pPrev->m_pNext = pEntry->m_pNext;
                        else m_pHead = pEntry->m_pNext;
        delete pEntry;
    }

    inline void removeAll (void)
    {
        while (m_pHead)
        {
            CEntry *pNext = m_pHead;
            delete m_pHead;
            m_pHead = pNext;
        }
        m_pHead = m_pTail = NULL;
    }

    inline NVPOSITION find (R obj) const
    {
        for (CEntry *pEntry = m_pHead; pEntry; pEntry = pEntry->m_pNext)
        {
            if (pEntry->m_Object == obj)
            {
                return (NVPOSITION)pEntry;
            }
        }
        return (NVPOSITION)0;
    }

    inline  CNVLinkedList (void) { m_pHead = m_pTail = NULL; }
    inline ~CNVLinkedList (void)
    {
#ifdef DEBUG
        if (m_pHead) { DPF ("Warning: CLinkedList<> not empty at destruction time"); }
#endif
    }
};

//---------------------------------------------------------------------------

// CNVArray
//    cannot store classes with constructors/destructors or with '=' operators
template<class T,class R,const int ciGrowBy = 128> class CNVArray
{
protected:
    T     *m_pData;
    DWORD  m_dwMax;
    DWORD  m_dwCount;

protected:
    void init (void)
    {
        m_pData = NULL;
        m_dwMax = m_dwCount = 0;
    }
    void grow (DWORD dwNewSize)
    {
        dwNewSize = (dwNewSize + ciGrowBy - 1) & ~(ciGrowBy - 1);
        if (dwNewSize > m_dwMax)
        {
            ReallocIPM(m_pData,sizeof(T) * dwNewSize,(void**)&m_pData);
            m_dwMax = dwNewSize;
        }
    }
    void kill (void)
    {
        if (m_pData) FreeIPM (m_pData);
        init();
    }

public:
    void append (R data)
    {
        if (m_dwCount >= m_dwMax) grow (m_dwCount + 1);
        m_pData[m_dwCount] = data;
        m_dwCount++;
    }

    void removeAll (void)
    {
        m_dwCount = 0;
    }

          DWORD getCount (void) const { return m_dwCount; }
    const T*    getData  (void) const { return m_pData; }

public:
    CNVArray  (void) { init(); }
    ~CNVArray (void) { kill(); }
};

#endif  // __D3D_CONST_

