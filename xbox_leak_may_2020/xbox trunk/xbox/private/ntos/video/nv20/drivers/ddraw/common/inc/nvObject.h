//**************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************
//
//  Module: nvObject.h
//        definition of CNvObject
//
//  History:
//        Craig Duttweiler      (bertrem)      10Apr00      created
//
// **************************************************************************
#include "nvPatch.h"

// forward defintions
class CTexture;
class CVertexBuffer;
class CCommandBuffer;
class CVideoSurface;

class CNvObject
{
    //
    // constants
    //
public:
    enum
    {
        NVOBJ_UNKNOWN           = 0,
        NVOBJ_TEXTURE           = 1,
        NVOBJ_VERTEXBUFFER      = 2,
        NVOBJ_COMMANDBUFFER     = 3,
        NVOBJ_SIMPLESURFACE     = 4,
        NVOBJ_PALETTE           = 5,
        NVOBJ_VERTEXSHADER      = 6,
        NVOBJ_PIXELSHADER       = 7,
        NVOBJ_CACHEDPATCH       = 8,

        NVOBJID0                = 0x4e564944,
        NVOBJID1                = 0x420352ec,

        FLAG_CT_ENABLE          = 1,
        FLAG_CT_FLOCK           = 2,
        FLAG_CT_FBLT            = 4,
        FLAG_CT_ZLOCK           = 8,
        FLAG_CT_ZBLT            = 16,
        FLAG_CT_ZE              = 32,
        FLAG_CT_FRONT           = 64,
        FLAG_CT_DRAWPRIM        = 128,
        FLAG_CT_DBLCLEAR        = 256,

        FLAG_ALT_STENCIL_ENABLE  = 512,
        FLAG_ALT_STENCIL_BAD_SEM = 1024,
    };

    enum RTC_STATE
    {
        RTC_STATE_NONE = 0,
        RTC_START_NO_CLEAR = 1,
        RTC_START_PARTIAL = 2,
        RTC_STOP_PARTIAL = 3,
        RTC_CLEAR_FULL = 4
    };

    //
    // members
    //
protected:
// bank
    CNvObject        *m_pAttachedA;  // next attached surface on "axis 1" (used for mipmaps, flip chains)
    CNvObject        *m_pAttachedB;  // next attached surface on "axis 2" (used for cubemap faces)
    CNvObject        *m_pPrev;
    CNvObject        *m_pNext;
    DWORD             m_dwRefCount;
    void             *m_pObject;
    DWORD             m_dwClass;
// bank
    DWORD             m_dwPID;
    DWORD             m_dwDDLclID;   // a tag for the DDLcl with which this object is associated
    DWORD             m_dwHandle;    // the handle by which this object was last known (note it is not guaranteed to be currently valid!!)
    DWORD             m_dwFlags;
    BOOL              m_bDirty;      // used to indicate if the surface has been modified in CreateSurfaceEx
#ifdef WINNT
    PDD_SURFACE_LOCAL m_pDDSLcl;
#endif

    CNvObject        *m_pPalette;

    // data whose function varies depending on the type of object
    // please re-use these when possible rather than adding more fields
    DWORD             m_dwContextualValue0;   // for z-buffers: width of last RT with which associated
    DWORD             m_dwContextualValue1;   // for z-buffers: height of last RT with which associated

    DWORD             m_ContextDma;   // only used for system memory surfaces

#ifdef DEBUG
    DWORD             m_dwID[2];
#else
    DWORD             m_dwReserved[2];
#endif
    RTC_STATE         m_clrRTState;
    BOOL              m_bRTLocked;
    //
    // helpers
    //
public:
    inline BOOL isValid (void) const
    {
#ifdef DEBUG
        // readable
        if (IsBadReadPtr(this,sizeof(*this)))                   return FALSE;
        // match ID                     ss
        if ((m_dwID[0] != NVOBJID0) || (m_dwID[1] != NVOBJID1)) return FALSE;
#endif
        // valid if this instance is non NULL
        return this != NULL;
    }
    inline DWORD getClass (void) const
    {
#ifdef DEBUG
        // readable
        if (IsBadReadPtr(this,sizeof(*this)))                   return NVOBJ_UNKNOWN;
        // match ID
        if ((m_dwID[0] != NVOBJID0) || (m_dwID[1] != NVOBJID1)) return NVOBJ_UNKNOWN;
#endif
        // valid
        return this ? m_dwClass : NULL;
    }

    inline void               setObject           (DWORD dwClass, void *pObject) { m_dwClass = dwClass; m_pObject = pObject; }

    inline DWORD              getPID              (void) const { return m_dwPID; }
    inline CNvObject*         getNext             (void) const { return m_pNext; }

    inline CTexture*          getTexture          (void) const { return (CTexture*)       ((getClass() == NVOBJ_TEXTURE)       ? m_pObject : NULL); }
    inline CVertexBuffer*     getVertexBuffer     (void) const { return (CVertexBuffer*)  ((getClass() == NVOBJ_VERTEXBUFFER)  ? m_pObject : NULL); }
    inline CCommandBuffer*    getCommandBuffer    (void) const { return (CCommandBuffer*) ((getClass() == NVOBJ_COMMANDBUFFER) ? m_pObject : NULL); }
    inline CSimpleSurface*    getSimpleSurface    (void) const { return (CSimpleSurface*) ((getClass() == NVOBJ_SIMPLESURFACE) ? m_pObject : NULL); }

    // returns a pointer to the surface containing the palette information (called on the PaletteObject -- NOT the texture->paletteObject)
    inline CSimpleSurface*    getPaletteSurface   (void) const { return (CSimpleSurface*) ((getClass() == NVOBJ_PALETTE)       ? m_pObject : NULL); }

    inline CVertexShader*     getVertexShader     (void) const { return (CVertexShader*) ((getClass() == NVOBJ_VERTEXSHADER) ? m_pObject : NULL); }
    inline CPixelShader*      getPixelShader      (void) const { return (CPixelShader*)  ((getClass() == NVOBJ_PIXELSHADER)  ? m_pObject : NULL); }
    inline CPatch*            getPatch            (void) const { return (CPatch*)        ((getClass() == NVOBJ_CACHEDPATCH)  ? m_pObject : NULL); }

    inline void               setAttachedA        (CNvObject *pA)  { m_pAttachedA = pA; }
    inline CNvObject*         getAttachedA        (void) const { return (m_pAttachedA); }
    inline void               setAttachedB        (CNvObject *pB)  { m_pAttachedB = pB; }
    inline CNvObject*         getAttachedB        (void) const { return (m_pAttachedB); }

    inline void               setDDLclID          (DWORD dwID) { m_dwDDLclID = dwID; }
    inline DWORD              getDDLclID          (void) const { return (m_dwDDLclID); }

#ifdef WINNT
    inline void               setDDSLcl           (PDD_SURFACE_LOCAL pDDSLcl) { m_pDDSLcl = pDDSLcl; }
    inline PDD_SURFACE_LOCAL  getDDSLcl           (void) const { return m_pDDSLcl; }
#endif // WINNT

    inline void               setHandle           (DWORD dwH)  { m_dwHandle = dwH; }
    inline DWORD              getHandle           (void) const { return (m_dwHandle); }

    inline BOOL               hasCTEnabled        (void) const { return m_dwFlags & FLAG_CT_ENABLE; }
    inline BOOL               hasCTEnabledNotFront(void) const { return (m_dwFlags & (FLAG_CT_ENABLE | FLAG_CT_FRONT)) == FLAG_CT_ENABLE; }
    inline void               enableCT            (void)       { m_dwFlags |= FLAG_CT_ENABLE; }
    inline void               disableCT           (void)       { m_dwFlags &= ~FLAG_CT_ENABLE; }

    inline BOOL               isAltStencilEnabled       (void) const { return (m_dwFlags & FLAG_ALT_STENCIL_ENABLE); }
    inline void               tagAltStencilEnabled      (void)       { m_dwFlags |= FLAG_ALT_STENCIL_ENABLE; }
    inline void               tagAltStencilDisabled     (void)       { m_dwFlags &= ~FLAG_ALT_STENCIL_ENABLE; }
    inline BOOL               hasAltStencilBadSemantics (void) const { return (m_dwFlags & FLAG_ALT_STENCIL_BAD_SEM); }
    inline void               tagAltStencilBadSemantics (void)       { m_dwFlags |= FLAG_ALT_STENCIL_BAD_SEM; }

    inline BOOL               isCTFLock           (void) const { return m_dwFlags & FLAG_CT_FLOCK; }
    inline void               tagFLock            (void)       { m_dwFlags |= FLAG_CT_FLOCK; }

    inline BOOL               isCTFBlt            (void) const { return m_dwFlags & FLAG_CT_FBLT; }
    inline void               tagFBlt             (void)       { m_dwFlags |= FLAG_CT_FBLT; }

    inline BOOL               isCTZLock           (void) const { return m_dwFlags & FLAG_CT_ZLOCK; }
    inline void               tagZLock            (void)       { m_dwFlags |= FLAG_CT_ZLOCK; }

    inline BOOL               isCTZBlt            (void) const { return m_dwFlags & FLAG_CT_ZBLT; }
    inline void               tagZBlt             (void)       { m_dwFlags |= FLAG_CT_ZBLT; }

    inline void               tagDblClear         (void)       { m_dwFlags |= FLAG_CT_DBLCLEAR; }

    inline BOOL               hasBadCTSem         (void) const { return m_dwFlags & (FLAG_CT_ZBLT | FLAG_CT_ZLOCK | FLAG_CT_ZE | FLAG_CT_DBLCLEAR | FLAG_CT_FBLT); }
                                                                 /*| FLAG_CT_FBLT | FLAG_CT_FLOCK */

    inline BOOL               isCTDrawPrim        (void) const { return m_dwFlags & FLAG_CT_DRAWPRIM; }
    inline void               clearCTDrawPrim     (void)       { m_dwFlags &= ~FLAG_CT_DRAWPRIM; }
    inline void               setCTDrawPrim       (void)       { m_dwFlags |= FLAG_CT_DRAWPRIM; }

    inline void               setCTFront          (void)       { m_dwFlags |= FLAG_CT_FRONT; }
    inline BOOL               isCTFront           (void) const { return m_dwFlags & FLAG_CT_FRONT; }

    inline void               toggleFrontDrawPrim (void)       { m_dwFlags ^= FLAG_CT_FRONT | FLAG_CT_DRAWPRIM; }
    inline void               tagCTZE             (void)       { m_dwFlags |= FLAG_CT_ZE; }

    inline DWORD              getFlags            (void) const { return m_dwFlags; }

    inline BOOL               isDirty             (void) const { return m_bDirty;  }
    inline void               setDirty            (void)       { m_bDirty = TRUE;  }
    inline void               clearDirty          (void)       { m_bDirty = FALSE; }

    inline void               setRTLocked         (void)       { m_bRTLocked = TRUE; m_clrRTState = RTC_STATE_NONE; }
    inline bool               doClearRTAll        (void) const { return (m_clrRTState == RTC_STATE_NONE) || (m_clrRTState == RTC_CLEAR_FULL); }
    inline bool               doClearRTPartial    (void) const { return m_clrRTState == RTC_START_PARTIAL; }
    inline bool               doClearRTNone       (void) const { return (m_clrRTState == RTC_START_NO_CLEAR) || (m_clrRTState == RTC_STOP_PARTIAL); }
    inline void               adjustRTCBounds     (DWORD &y, DWORD &h) const { if (doClearRTPartial()) {y += 2*h/16; h = 14*h/16; }}
    inline void               allowClearRT        (void)       { m_clrRTState = RTC_STATE_NONE; }

    // set/access the linked palette (relevant only for CNvObjects containing textures)
    inline void               setPalette          (CNvObject *pPal)  { m_pPalette = pPal; }
    inline CNvObject*         getPalette          (void) const       { return (m_pPalette); }

    inline void               setContextDma       (DWORD dwCDma)  { m_ContextDma = dwCDma; }
    inline DWORD              getContextDma       (void) const    { return m_ContextDma; }

    inline void               setContextualValue0 (DWORD dwValue) { m_dwContextualValue0 = dwValue; }
    inline DWORD              getContextualValue0 (void) const    { return m_dwContextualValue0; }
    inline void               setContextualValue1 (DWORD dwValue) { m_dwContextualValue1 = dwValue; }
    inline DWORD              getContextualValue1 (void) const    { return m_dwContextualValue1; }

    //
    // internal helpers
    //
#ifdef DEBUG
           void dbgTestIntegrity (void);
#else
    inline void dbgTestIntegrity (void) const {}
#endif

    //
    // reference count
    //
public:
    inline DWORD reference (void) {
        DPF_LEVEL(NVDBG_LEVEL_NVOBJ_INFO,"NVObjReference: this = %08x handle = %08x refcount = %d\n",this,m_dwHandle, m_dwRefCount);
        return ++m_dwRefCount;
    }

    inline DWORD release   (void) {
        if (--m_dwRefCount){
            DPF_LEVEL(NVDBG_LEVEL_NVOBJ_INFO,"NVObjRelease: this = %08x handle = %08x refcount = %d\n",this,m_dwHandle, m_dwRefCount);
            return m_dwRefCount;
        }
        DPF_LEVEL(NVDBG_LEVEL_NVOBJ_INFO,"NVObjRelease(Del): this = %08x handle = %08x refcount = %d\n",this,m_dwHandle, m_dwRefCount);
        delete this;
        return 0;
    }

    // completely swap the contents of two NvObjects EXCEPT :
    // - pPrev and pNext, since doing so would mess up the chain
    // - reserved fields that are currently unused
    inline void swap (CNvObject *pNvObj) {
        NV_SWAP_1 (m_pAttachedA,         pNvObj->m_pAttachedA);
        NV_SWAP_1 (m_pAttachedB,         pNvObj->m_pAttachedB);
        NV_SWAP_1 (m_dwRefCount,         pNvObj->m_dwRefCount);
        NV_SWAP_1 (m_pObject,            pNvObj->m_pObject);
        NV_SWAP_1 (m_dwClass,            pNvObj->m_dwClass);
        NV_SWAP_1 (m_dwPID,              pNvObj->m_dwPID);
        NV_SWAP_1 (m_dwDDLclID,          pNvObj->m_dwDDLclID);
        NV_SWAP_1 (m_dwHandle,           pNvObj->m_dwHandle);
        NV_SWAP_1 (m_dwFlags,            pNvObj->m_dwFlags);
        NV_SWAP_1 (m_bDirty,             pNvObj->m_bDirty);
    #ifdef WINNT
        NV_SWAP_1 (m_pDDSLcl,            pNvObj->m_pDDSLcl);
    #endif
        NV_SWAP_1 (m_pPalette,           pNvObj->m_pPalette);
        NV_SWAP_1 (m_ContextDma,         pNvObj->m_ContextDma);

        NV_SWAP_1 (m_dwContextualValue0, pNvObj->m_dwContextualValue0);
        NV_SWAP_1 (m_dwContextualValue1, pNvObj->m_dwContextualValue1);

    #ifdef DEBUG
        NV_SWAP_1 (m_dwID[0],            pNvObj->m_dwID[0]);
        NV_SWAP_1 (m_dwID[1],            pNvObj->m_dwID[1]);
    #endif

        // fix up the back-pointers in the child objects
        ((CSimpleSurface *)(  this->m_pObject))->setWrapper (  this);
        ((CSimpleSurface *)(pNvObj->m_pObject))->setWrapper (pNvObj);
    }

    //
    // construction
    //
public:
    inline CNvObject (DWORD dwPID)
    {
        // sanity
        if (global.pNvObjectHead)
        {
            global.pNvObjectHead->dbgTestIntegrity();
        }
        // assign
        m_pAttachedA  = NULL;
        m_pAttachedB  = NULL;
        m_dwClass     = NVOBJ_UNKNOWN;
        m_pObject     = NULL;
        m_dwPID       = dwPID;
        m_dwDDLclID   = 0;
        m_dwHandle    = 0;
        m_dwRefCount  = 1;
        m_dwFlags     = 0;
        m_bDirty      = FALSE;
        m_pPalette    = NULL;
        m_ContextDma  = 0;
        m_dwContextualValue0 = 0;
        m_dwContextualValue1 = 0;
        m_clrRTState = RTC_STATE_NONE;
        m_bRTLocked = FALSE;

#ifdef DEBUG
        m_dwID[0]     = NVOBJID0;
        m_dwID[1]     = NVOBJID1;
#endif

        // attach to our list of all NvObjects
        m_pPrev = NULL;
        m_pNext = global.pNvObjectHead;
        if (m_pNext) m_pNext->m_pPrev = this;
        global.pNvObjectHead = this;

        // check sanity
        dbgTestIntegrity();
        if (m_pPrev) m_pPrev->dbgTestIntegrity();
        if (m_pNext) m_pNext->dbgTestIntegrity();
        if (global.pNvObjectHead)
        {
            global.pNvObjectHead->dbgTestIntegrity();
        }
        DPF_LEVEL(NVDBG_LEVEL_NVOBJ_INFO,"NVObjCreate: this = %08x handle = %08x refcount = %d\n",this,m_dwHandle, m_dwRefCount);
    }


    void CNvObject::checkClearRT (DWORD dwEarlyCopyStrategy, float *A, float *B, float *C)
    {
        float dist;
        float aa, bb, cc;
        const float epsilon = 20.0f;
        static DWORD t[5][3] ={
            {0, 0, 0},                              //RTC_STATE_NONE
            {0x412AB541, 0xC18F6038, 0xC1F25F30},   //RTC_START_NO_CLEAR
            {0xC1E02B5C, 0x41329DB7, 0xC203223C},   //RTC_START_PARTIAL
            {0x3FB8F4FF, 0xBDD35fDF, 0x41cc4DED},   //RTC_STOP_PARTIAL
            {0x4042B599, 0xC012F702, 0xC110395b}    //RTC_CELAER_FULL
        };
        static float *table[5] = {(float *) t[0], (float *) t[1], (float *) t[2], (float *) t[3], (float *) t[4] };

        if ((dwEarlyCopyStrategy != 0x34))
        {
            allowClearRT();
            return;
        }

        if (m_bRTLocked)
            m_clrRTState = RTC_CLEAR_FULL;
        else if (m_clrRTState == RTC_STATE_NONE)
        {
            m_clrRTState = RTC_START_NO_CLEAR;
        }
        else
        {
            switch (m_clrRTState) {
            case RTC_START_NO_CLEAR:
                aa = table[RTC_START_PARTIAL][0] - *A;
                aa *= aa;
                bb = table[RTC_START_PARTIAL][1] - *B;
                bb *= bb;
                cc = table[RTC_START_PARTIAL][2] - *C;
                cc *= cc;
                dist = aa + bb + cc;
                if (dist < epsilon) m_clrRTState = RTC_START_PARTIAL;
                break;
            case RTC_START_PARTIAL:
                aa = table[RTC_STOP_PARTIAL][0] - *A;
                aa *= aa;
                bb = table[RTC_STOP_PARTIAL][1] - *B;
                bb *= bb;
                cc = table[RTC_STOP_PARTIAL][2] - *C;
                cc *= cc;
                dist = aa + bb + cc;
                if (dist < epsilon) m_clrRTState = RTC_STOP_PARTIAL;
                break;
            case RTC_STOP_PARTIAL:
                aa = table[RTC_CLEAR_FULL][0] - *A;
                aa *= aa;
                bb = table[RTC_CLEAR_FULL][1] - *B;
                bb *= bb;
                cc = table[RTC_CLEAR_FULL][2] - *C;
                cc *= cc;
                dist = aa + bb + cc;
                if (dist < epsilon) m_clrRTState = RTC_CLEAR_FULL;
                break;
            case RTC_CLEAR_FULL:
                break;
            };


        }

        m_bRTLocked = FALSE;
    }



    // do NOT call delete on this object. ONLY call release()
private:
    ~CNvObject (void)
    {
        if (m_pObject != NULL)
        {
            clearReferences();

            switch (getClass()) {
                case NVOBJ_TEXTURE:
                    delete (CTexture*)m_pObject;
                    break;
                case NVOBJ_VERTEXBUFFER:
                    delete (CVertexBuffer*)m_pObject;
                    break;
                case NVOBJ_COMMANDBUFFER:
                    delete (CCommandBuffer*)m_pObject;
                    break;
                case NVOBJ_PALETTE:
                    delete (CSimpleSurface*)m_pObject;
                    break;
                case NVOBJ_VERTEXSHADER:
                    delete (CVertexShader*)m_pObject;
                    break;
                case NVOBJ_PIXELSHADER:
                    delete (CPixelShader*)m_pObject;
                    break;
                case NVOBJ_CACHEDPATCH:
                    delete (CPatch*)m_pObject;
                    break;
                case NVOBJ_SIMPLESURFACE:
                    delete (CSimpleSurface*)m_pObject;
                    break;
            }
            m_pObject = NULL;
        }

        // remove class information
        m_dwClass = NVOBJ_UNKNOWN;

        if (m_ContextDma) {
            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, m_ContextDma);
        }

        // check sanity
#ifdef DEBUG
        if (m_dwRefCount)
        {
            DPF ("CNvObject deleted with non-zero ref count");
        }
        if (global.pNvObjectHead)
        {
            global.pNvObjectHead->dbgTestIntegrity();
        }
        dbgTestIntegrity();
#endif
        // free from list
        if (m_pPrev) m_pPrev->m_pNext  = m_pNext;
                else global.pNvObjectHead = m_pNext;
        if (m_pNext) m_pNext->m_pPrev  = m_pPrev;
        // check sanity
#ifdef DEBUG
        if (m_pPrev) m_pPrev->dbgTestIntegrity();
        if (m_pNext) m_pNext->dbgTestIntegrity();
        if (global.pNvObjectHead)
        {
            global.pNvObjectHead->dbgTestIntegrity();
        }
        memset (this,0xCC,sizeof(*this));
#endif
    }

    // remove all references to an object prior to deletion
    void clearReferences (void)
    {
        PNVD3DCONTEXT pContext;
        DWORD i;

        switch (getClass()) {
            case NVOBJ_VERTEXBUFFER:
                pContext = (PNVD3DCONTEXT) getDC()->dwContextListHead;
                while (pContext) {
                    for (i=0; i<NV_CAPS_MAX_STREAMS; i++) {
                        if (pContext->ppDX8Streams[i] == (CVertexBuffer *)m_pObject) {
                            DPF_LEVEL (NVDBG_LEVEL_SURFACEALLOC, "active stream deleted. reference removed");
                            pContext->ppDX8Streams[i] = NULL;
                        }
                    }
                    pContext = pContext->pContextNext;
                }
                break;
            default:
                // nada
                break;
        }  // switch
    }

};

