// **************************************************************************
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
//  Module: nvTexManage.cpp
//        texture management routines
//
//  History:
//        Craig Duttweiler      (bertrem)       3Dec99      created
//
// **************************************************************************
#include "nvprecomp.h"
#pragma hdrstop


#if (NVARCH >= 0x04)

#ifdef TEX_MANAGE

//---------------------------------------------------------------------------

BOOL nvTexManageInit
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvTexManageInit");

    pContext->texManData.dwTextureListInUse = NV_TEXMAN_DEFAULT_CLASS;

    for (DWORD i=0; i<NV_TEXMAN_NUM_CLASSES; i++) {
        pContext->texManData.pTextureLRU[i] = NULL;
        pContext->texManData.pTextureMRU[i] = NULL;
    }

    pContext->texManData.pTextureToEvict = NULL;

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

// add a managed texture to pContext's list for class dwClass

BOOL nvTexManageAddTexture
(
    PNVD3DCONTEXT pContext,
    CTexture     *pTexture,
    DWORD         dwClass
)
{
    dbgTracePush ("nvTexManageAddTexture");

    if (!pContext)
    {
        pContext = nvGetPreapprovedContext();
    }
    nvAssert (pContext);
    nvAssert (pTexture && pTexture->isManaged() && pTexture->isBaseTexture());

    CTexture *pListTail = pContext->texManData.pTextureMRU[dwClass];

    pTexture->setPrev (pListTail);
    pTexture->setNext (NULL);

    if (pListTail) {
        pListTail->setNext (pTexture);
    }

    pContext->texManData.pTextureMRU[dwClass] = pTexture;

    if (pContext->texManData.pTextureLRU[dwClass] == NULL) {
        pContext->texManData.pTextureLRU[dwClass] = pTexture;
    }

    pTexture->setContext (pContext);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

// remove a texture from the class lists

BOOL nvTexManageRemoveTexture
(
    CTexture *pTexture
)
{
    dbgTracePush ("nvTexManageRemoveTexture");

    nvAssert (pTexture && pTexture->isManaged() && pTexture->isBaseTexture());

    PNVD3DCONTEXT pContext = pTexture->getContext();
    CTexture     *pNext    = pTexture->getNext();
    CTexture     *pPrev    = pTexture->getPrev();

    nvAssert (pContext);

    // if the eviction pointer was pointing to this texture, advance it
    if (pContext->texManData.pTextureToEvict == pTexture) {
        nvTexManageAdvancePtr (pContext);
    }

    if (pNext) {
        nvAssert (pNext->getPrev() == pTexture);  // sanity
        pNext->setPrev (pPrev);
    }
    else {
        // this was the end of the list. find the relevant list-end pointer and clean it up
        for (DWORD i=0; i<NV_TEXMAN_NUM_CLASSES; i++) {
            if (pContext->texManData.pTextureMRU[i] == pTexture) break;
        }
        nvAssert (i<NV_TEXMAN_NUM_CLASSES);
        pContext->texManData.pTextureMRU[i] = pPrev;
    }

    if (pPrev) {
        nvAssert (pPrev->getNext() == pTexture);  // sanity
        pPrev->setNext (pNext);
    }
    else {
        // this was the start of the list. find the relevant list-head pointer and clean it up
        for (DWORD i=0; i<NV_TEXMAN_NUM_CLASSES; i++) {
            if (pContext->texManData.pTextureLRU[i] == pTexture) break;
        }
        nvAssert (i<NV_TEXMAN_NUM_CLASSES);
        pContext->texManData.pTextureLRU[i] = pNext;
    }

    pTexture->setNext    (NULL);
    pTexture->setPrev    (NULL);
    pTexture->setContext (NULL);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

// advance to the next non-null list, after (and _inclusive_ of dwList)

BOOL nvTexManageAdvanceList
(
    PNVD3DCONTEXT pContext,
    DWORD         dwList
)
{
    dbgTracePush ("nvTexManageAdvanceList");

    // try to find a non-empty list
    while (dwList < NV_TEXMAN_NUM_CLASSES) {
        if (pContext->texManData.pTextureLRU[dwList] != NULL) {
            pContext->texManData.dwTextureListInUse = dwList;
            pContext->texManData.pTextureToEvict = pContext->texManData.pTextureLRU[dwList];
            dbgTracePop();
            return (TRUE);
        }
        dwList++;
    }

    pContext->texManData.dwTextureListInUse = 0;
    pContext->texManData.pTextureToEvict = NULL;

    dbgTracePop();
    return (FALSE);
}

//---------------------------------------------------------------------------

// reset the eviction pointer

BOOL nvTexManageResetPtr
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvTexManageResetPtr");
    BOOL bRV = nvTexManageAdvanceList (pContext, 0);
    dbgTracePop();
    return (bRV);
}

//---------------------------------------------------------------------------

// advance the eviction pointer

BOOL nvTexManageAdvancePtr
(
    PNVD3DCONTEXT pContext
)
{
    BOOL bSuccess;

    dbgTracePush ("nvTexManageAdvancePtr");

    if (pContext->texManData.pTextureToEvict == NULL) {
        // reset everything
        bSuccess = nvTexManageResetPtr (pContext);
    }

    else {
        pContext->texManData.pTextureToEvict = (pContext->texManData.pTextureToEvict)->getNext();
        if (pContext->texManData.pTextureToEvict == NULL) {
            // we've fallen off the end of the list, advance to the next nonempty list
            bSuccess = nvTexManageAdvanceList (pContext, pContext->texManData.dwTextureListInUse+1);
        }
        else {
            bSuccess = TRUE;
        }
    }

    dbgTracePop();
    return (bSuccess);
}

//---------------------------------------------------------------------------

// nvTexManageEvict
//
// evicts a texture from pContext (or elsewhere, contingent on the protocol).
// see nvTexManage.h for possible values of dwProtocol.
//
// returns TRUE upon successful eviction, else FALSE

BOOL nvTexManageEvict
(
    PNVD3DCONTEXT pContext,
    DWORD         dwProtocol
)
{
    CTexture *pTexture;

    dbgTracePush ("nvTexManageEvict");

    switch (dwProtocol) {

        case NV_TEXMAN_EVICTION_PROTOCOL_ANY:
            // recurse on all protocols
            dwProtocol = NV_TEXMAN_EVICTION_PROTOCOL_PASSIVE;
            while (dwProtocol <= NV_TEXMAN_EVICTION_PROTOCOL_SEVERE) {
                if (nvTexManageEvict (pContext, dwProtocol)) {
                    dbgTracePop();
                    return (TRUE);
                }
                dwProtocol++;
            }
            break;

        case NV_TEXMAN_EVICTION_PROTOCOL_SEVERE:
            // recurse on all contexts, beginning with this one
            PNVD3DCONTEXT pCtxt;
            pCtxt = pContext;
            while (pCtxt) {
                if (nvTexManageEvict (pCtxt, NV_TEXMAN_EVICTION_PROTOCOL_AGGRESSIVE)) {
                    dbgTracePop();
                    return (TRUE);
                }
                pCtxt = pCtxt->pContextNext;
            }
            pCtxt = pContext->pContextPrev;
            while (pCtxt) {
                if (nvTexManageEvict (pCtxt, NV_TEXMAN_EVICTION_PROTOCOL_AGGRESSIVE)) {
                    dbgTracePop();
                    return (TRUE);
                }
                pCtxt = pCtxt->pContextPrev;
            }
            break;

        default:
            pTexture = pContext->texManData.pTextureToEvict;

            if (!pTexture) {
                nvTexManageResetPtr (pContext);
                pTexture = pContext->texManData.pTextureToEvict;
            }

            while (pTexture) {
                if (dwProtocol == NV_TEXMAN_EVICTION_PROTOCOL_AGGRESSIVE) {
                    // force the HW to give up this texture
					DDLOCKINDEX(NVSTAT_LOCK_TEXMAN_EVICT);
                    pTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);
                }
                if (pTexture->evict()) {
                    nvTexManageRemoveTexture (pTexture);
                    DPF_LEVEL (NVDBG_LEVEL_TEXMAN, "texman: evicted %08x at address %08x of size %08x, protocol=%d", pTexture, 
                                                pTexture->getSwizzled()->getAddress,pTexture->getSize(),dwProtocol);
                    dbgTracePop();
                    return (TRUE);
                }
                nvTexManageAdvancePtr (pContext);
                pTexture = pContext->texManData.pTextureToEvict;
            }
            break;

    }  // switch

    DPF_LEVEL (NVDBG_LEVEL_TEXMAN, "texman: eviction FAILED, protocol=%d", dwProtocol);

    dbgTracePop();
    return (FALSE);
}

//---------------------------------------------------------------------------

// nvTexManageEvictAll
//
// evict all managed textures associated with pContext.
// if pContext is NULL, evict all textures in all contexts

BOOL nvTexManageEvictAll
(
    PNVD3DCONTEXT pContext
)
{
    CTexture *pTexture;
    BOOL bSuccess = TRUE;

    dbgTracePush ("nvTexManageEvictAll: pContext=%08x", pContext);

    if (!pContext) {
        pContext = (PNVD3DCONTEXT) getDC()->dwContextListHead;
        while (pContext) {
            if (!nvTexManageEvictAll (pContext)) {
                bSuccess = FALSE;
            }
            pContext = pContext->pContextNext;
        }
        dbgTracePop();
        return (bSuccess);
    }

    nvTexManageResetPtr (pContext);
    pTexture = pContext->texManData.pTextureToEvict;

    while (pTexture) {
        // force the HW to give up this texture
		DDLOCKINDEX(NVSTAT_LOCK_TEXMAN_EVICT_ALL);
        pTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);
        if (pTexture->evict()) {
            nvTexManageRemoveTexture (pTexture);
            DPF_LEVEL (NVDBG_LEVEL_TEXMAN, "texman: evictAll evicted %08x at address %08x of size %08x from context %08x", 
                                            pTexture, pTexture->getSwizzled()->getAddress(),pTexture->getSize(),pContext);
        }
        else {
            bSuccess = FALSE;
            DPF ("texman: evictAll unable to evict %08x", pTexture);
            dbgD3DError();
            nvTexManageAdvancePtr (pContext);
        }
        pTexture->cpuUnlockSwz();
        pTexture = pContext->texManData.pTextureToEvict;
    }

    dbgTracePop();
    return (bSuccess);
}

//---------------------------------------------------------------------------

// nvTexManageFetch
//
// fetches a texture from system memory into video or AGP mempory.
// other textures are evicted from pContext as necessary.

BOOL nvTexManageFetch
(
    PNVD3DCONTEXT pContext,  // may be NULL!!
    CTexture     *pTexture
)
{
    BOOL bFetchSuccess, bEvictionSuccess;

    dbgTracePush ("nvTexManageFetch");

    DPF_LEVEL (NVDBG_LEVEL_TEXMAN, "texman: fetching %08x", pTexture);

    nvAssert (pTexture && pTexture->isManaged());

    if (!pContext) {
        // totally arbitrary. for lack of any context with which to
        // associate this texture, just stick it in the first
        pContext = (PNVD3DCONTEXT) getDC()->dwContextListHead;
    }

    bFetchSuccess = pTexture->fetch();

    if (!bFetchSuccess) {
        nvTexManageResetPtr (pContext);
        do {
            // evict something and try again
            bEvictionSuccess = nvTexManageEvict (pContext, NV_TEXMAN_EVICTION_PROTOCOL_ANY);
            if (bEvictionSuccess) {
                bFetchSuccess = pTexture->fetch();
            }
        } while (bEvictionSuccess && !bFetchSuccess);
    }

    if (bFetchSuccess) {
        nvAssert ((pTexture->getPrev() == NULL) &&
                  (pTexture->getNext() == NULL));
        nvTexManageAddTexture (pContext, pTexture, NV_TEXMAN_DEFAULT_CLASS);
    }

#ifdef DEBUG
    if (bFetchSuccess) {
        DWORD dwHeap = pTexture->getSwizzled()->getHeapLocation();
        DPF_LEVEL (NVDBG_LEVEL_TEXMAN, "texman: fetched %08x at address %08x of size %08x into %s memory",
                   pTexture,
                   pTexture->getSwizzled()->getAddress(),
                   pTexture->getSize(),
                   (dwHeap == CSimpleSurface::HEAP_VID) ? "video" :
                   (dwHeap == CSimpleSurface::HEAP_AGP) ? "agp"   :
                   (dwHeap == CSimpleSurface::HEAP_PCI) ? "pci"   : "unknown");
    }
    else {
        DPF_LEVEL (NVDBG_LEVEL_TEXMAN, "texman: fetch of %08x FAILED", pTexture);
    }
#endif

    dbgTracePop();
    return (bFetchSuccess);
}

//---------------------------------------------------------------------------

// nvTexManageTouch
//
// "touches" a texture. texture is moved from its current location in the
// eviction lists to the MRU slot.

BOOL nvTexManageTouch
(
    PNVD3DCONTEXT pContext,  // may be NULL!
    CTexture     *pTexture
)
{
    dbgTracePush ("nvTexManageTouch");

    nvAssert (pTexture && pTexture->isManaged() && pTexture->isBaseTexture());

    if (!nvTexManageRemoveTexture (pTexture)) {
        DPF ("failed to remove a managed texture from the LRU lists");
        dbgTracePop();
        return (FALSE);
    }

    if (!pContext) {
        // totally arbitray. for lack of any context with which to
        // associate this texture, just stick it in the first
        pContext = (PNVD3DCONTEXT) getDC()->dwContextListHead;
    }

    if (!nvTexManageAddTexture (pContext, pTexture, NV_TEXMAN_DEFAULT_CLASS)) {
        DPF ("failed to add a managed texture to the LRU lists");
        dbgTracePop();
        return (FALSE);
    }

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

// nvTexManageSceneEnd
//
// texture management done whenever scene end is called.
// under current scheme, we basically just shuffle some eviction lists around

BOOL nvTexManageSceneEnd
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvTexManageSceneEnd");

    // list 2 (previous frame) goes onto the end of list 0 (old frames)
    CTexture *pList0Tail = pContext->texManData.pTextureMRU[0];
    CTexture *pList2Head = pContext->texManData.pTextureLRU[2];

    if (pList2Head) {
        if (pList0Tail) {
            // list0 exists. attach list2 at the end
            pList0Tail->setNext (pList2Head);
            pList2Head->setPrev (pList0Tail);
        }
        else {
            // there is no list0. list0 <= list2
            pContext->texManData.pTextureLRU[0] = pList2Head;
        }
        pContext->texManData.pTextureMRU[0] = pContext->texManData.pTextureMRU[2];
    }

    // list 1 (most recent frame) becomes list 2 (previous frame)
    pContext->texManData.pTextureLRU[2] = pContext->texManData.pTextureLRU[1];
    pContext->texManData.pTextureMRU[2] = pContext->texManData.pTextureMRU[1];

    // list 1 (most recent frame) is now empty
    pContext->texManData.pTextureLRU[1] = NULL;
    pContext->texManData.pTextureMRU[1] = NULL;

    dbgTracePop();
    return (TRUE);
}

#endif

#endif // NVARCH >= 0x04

