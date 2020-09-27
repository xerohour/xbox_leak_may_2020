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
//  Module: nvTexManage.h
//        definitions and prototypes associated with texture management
//
//  History:
//        Craig Duttweiler      (bertrem)       3Dec99      created
//
// **************************************************************************

#ifdef TEX_MANAGE

// Texture Management Scheme Overview
// we keep multiple texture lists to allow different "classes"  of textures to be defined
// for eviction purposes. class 0 is evicted first; class N is evicted last. within each class,
// the LRU texture is evicted first and the MRU texture is evicted last. textures can be moved
// from one class to another (or entire class lists can be swapped/combined) as needed.

// the currect implementation will classify textures according to the frame in which they were
// most recently used:

// class 0 : textures used in neither the current nor previous frames
// class 1 : textures most recently used in the current frame
// class 2 : textures most recently used in the last frame

#define NV_TEXMAN_NUM_CLASSES   3
#define NV_TEXMAN_DEFAULT_CLASS 1  // the class to which new and "touched" textures get added

// eviction protocols

#define NV_TEXMAN_EVICTION_PROTOCOL_PASSIVE     0  // evict only what we can without syncing or changing contexts
#define NV_TEXMAN_EVICTION_PROTOCOL_AGGRESSIVE  1  // force hardware to sync and relinquish control of each texture before eviction is attempted
#define NV_TEXMAN_EVICTION_PROTOCOL_SEVERE      2  // try to evict from other contexts too
#define NV_TEXMAN_EVICTION_PROTOCOL_ANY         3  // try all protocols in order of increasing severity until eviction succeeds

typedef struct _NV_TEXMAN_DATA {

    DWORD     dwTextureListInUse;

    // pointers to the the ends of the class lists
    CTexture *pTextureLRU[NV_TEXMAN_NUM_CLASSES];
    CTexture *pTextureMRU[NV_TEXMAN_NUM_CLASSES];

    // the next texture to evict. this pointer should ALWAYS be non-NULL unless
    // all texture lists are empty and there is truly nothing to evict.
    CTexture *pTextureToEvict;

} NV_TEXMAN_DATA, *PNV_TEXMAN_DATA;

// prototypes

BOOL nvTexManageInit          (PNVD3DCONTEXT pContext);
BOOL nvTexManageAddTexture    (PNVD3DCONTEXT pContext, CTexture *pTex, DWORD dwClass);
BOOL nvTexManageRemoveTexture (CTexture *pTex);
BOOL nvTexManageAdvanceList   (PNVD3DCONTEXT pContext, DWORD dwList);
BOOL nvTexManageResetPtr      (PNVD3DCONTEXT pContext);
BOOL nvTexManageAdvancePtr    (PNVD3DCONTEXT pContext);
BOOL nvTexManageEvict         (PNVD3DCONTEXT pContext, DWORD dwProtocol);
BOOL nvTexManageEvictAll      (PNVD3DCONTEXT pContext);
BOOL nvTexManageFetch         (PNVD3DCONTEXT pContext, CTexture *pTexture);
BOOL nvTexManageTouch         (PNVD3DCONTEXT pContext, CTexture *pTexture);
BOOL nvTexManageSceneEnd      (PNVD3DCONTEXT pContext);

#endif  // TEX_MANAGE

