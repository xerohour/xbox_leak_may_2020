#ifndef _NVD3DMAC_H_
#define _NVD3DMAC_H_
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVD3DMAC.H                                                        *
*   Define various macros used in the D3D driver.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/98 - Created.                     *
*                                                                           *
\***************************************************************************/
#ifdef  NV4
#include    "nv4dmac.h"
#endif

/*
 * Global memory allocation macros.
 */
#ifdef  WINNT
#define HEAPCREATE(nop)                 (!NULL)
#define HEAPDESTROY(heap)               ((heap) = NULL)
#define HEAPALLOC(nop, size)            EngAllocMem(FL_ZERO_MEMORY, (size), 'amxD')
#define HEAPFREE(nop, ptr)              { \
    if (ptr != NULL)            \
    {                           \
    EngFreeMem((ptr));          \
    (ptr) = NULL;               \
    }                           \
}
#define HEAPREALLOC(nop, ptr, size)     {   \
                                            HEAPFREE(nop, (ptr)); \
                                            (ptr) = HEAPALLOC(nop, size); \
                                        }
/*
 * User mode memory allocation functions. There are no Globalxxx fcts in NT
 * kernel mode.
 */
#define GlobalAlloc(nop, size)  \
    EngAllocUserMem((size), 'amxD')
#define GlobalLock(ptr)  (ptr)
#define GlobalUnlock(nop)
#define GlobalFree(ptr)         \
    {                           \
    if (ptr != NULL)            \
    {                           \
    EngFreeUserMem(ptr);        \
    (ptr) = NULL;               \
    }                           \
    }
#else   // !WINNT
#define HEAPCREATE(size)                HeapCreate(HEAP_SHARED, (size), 0)
#define HEAPDESTROY(handle)             HeapDestroy((HANDLE)(handle))
#define HEAPALLOC(handle, size)         HeapAlloc((HANDLE)(handle), HEAP_ZERO_MEMORY, (size))
#define HEAPFREE(handle, ptr)           HeapFree((HANDLE)(handle), 0, (VOID *)(ptr))
#define HEAPREALLOC(handle, ptr, size)  (ptr) = HeapReAlloc((HANDLE)(handle), 0, (ptr), (size))

#endif  // !WINNT

/*
 * Convert th FP representation into a DWORD.  Used for magnitude comparisons.
 */
#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000
/*
 * Some FP macros used in table fog. Thanks Jim!
 */
#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}
#define FP_EXP(e,p)                                                          \
{                                                                            \
    int _i;                                                                  \
    e = -1.44269504f * (float)0x00800000 * (p);                              \
    _i = (int)e + 0x3F800000;                                                \
    e = *(float *)&_i;                                                       \
}
#define FP_NORM_TO_BYTE(i,p)                                                 \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    (i) = *(int *)&_n;                                                       \
    if ((i) >= 0x40000000)     (i) = 0xFF;                                   \
    else if ((i) <=0x3F800000) (i) = 0;                                      \
    else (i) = ((i) >> 15) & 0xFF;                                           \
}

/*
 * D3D state macros.
 */

#define IS_OVERRIDE(type)       ((DWORD)(type) > D3DSTATE_OVERRIDE_BIAS)
#define GET_OVERRIDE(type)      ((DWORD)(type) - D3DSTATE_OVERRIDE_BIAS)

#define STATESET_MASK(set, state)                                           \
        (set).bits[((state) - 1) >> DWORD_SHIFT]

#define STATESET_BIT(state)     (1 << (((state) - 1) & (DWORD_BITS - 1)))

#define STATESET_ISSET(set, state)                                          \
        STATESET_MASK(set, state) & STATESET_BIT(state)

#define STATESET_SET(set, state)                                            \
        STATESET_MASK(set, state) |= STATESET_BIT(state)

#define STATESET_CLEAR(set, state)                                          \
        STATESET_MASK(set, state) &= ~STATESET_BIT(state)

#define STATESET_INIT(set)      memset(&(set), 0, sizeof(set))

#if 0
/*
 * Memory management macros.
 */
#define GlobalUnlockPtr32(lp)                                               \
        GlobalUnlock(GlobalHandle((LPCVOID)lp))

#define GlobalAllocPtr32(flags, cb)                                         \
        (GlobalLock(GlobalAlloc((flags), (cb))))

#define GlobalFreePtr32(lp)                                                 \
        (GlobalUnlockPtr32(lp), (BOOL)GlobalFree(GlobalHandle((LPCVOID)lp)))
#endif

#ifdef  DO_XF_LIGHT
// TODO move dwCurrentPipelineHandle into pDriverData structure
// TODO move dwCurrentPipelinePtr into pDriverData structure
#define GET_PIPELINE_PTR(ptr, harray, hpipeline)                            \
{                                                                           \
    if ((hpipeline) != dwCurrentPipelineHandle)                             \
    {                                                                       \
        (ptr) = (PNVD3DPIPELINE)DDrvItemArrayGetPtr((harray), (HDDRVITEM)(hpipeline));\
        dwCurrentPipelineHandle = (DWORD)(hpipeline);                       \
        dwCurrentPipelinePtr    = (DWORD)(ptr);                             \
    }                                                                       \
    else                                                                    \
        (ptr) = (PNVD3DPIPELINE)dwCurrentPipelinePtr;                       \
}

#define DESTROY_PIPELINES_FROM_PROCESS(pid)                                 \
{                                                                           \
    DDrvItemArrayApplyValid(pDriverData->hPipelines_gbl,                    \
                            DestroyPipelineProcessCallback,                 \
                            pid);                                           \
}

#define DESTROY_PIPELINES_FROM_CONTEXT(hctx)                                \
{                                                                           \
    DDrvItemArrayApplyValid(pDriverData->hPipelines_gbl,                    \
                            DestroyPipelineContextCallback,                 \
                            hctx);                                          \
}

// TODO move dwCurrentMaterialHandle into pDriverData structure
// TODO move dwCurrentMaterialPtr into pDriverData structure
#define GET_MATERIAL_PTR(ptr, harray, hmaterial)                            \
{                                                                           \
    if ((hmaterial) != dwCurrentMaterialHandle)                             \
    {                                                                       \
        (ptr) = (PNVD3DMATERIAL)DDrvItemArrayGetPtr((harray), (HDDRVITEM)(hmaterial));\
        dwCurrentMaterialHandle = (DWORD)(hmaterial);                       \
        dwCurrentMaterialPtr    = (DWORD)(ptr);                             \
    }                                                                       \
    else                                                                    \
        (ptr) = (PNVD3DMATERIAL)dwCurrentMaterialPtr;                       \
}

#define DESTROY_MATERIALS_FROM_PROCESS(pid)                                 \
{                                                                           \
    DDrvItemArrayApplyValid(pDriverData->hMaterials_gbl,                    \
                            DestroyMaterialProcessCallback,                 \
                            pid);                                           \
}

#define DESTROY_MATERIALS_FROM_CONTEXT(hctx)                                \
{                                                                           \
    DDrvItemArrayApplyValid(pDriverData->hMaterials_gbl,                    \
                            DestroyMaterialContextCallback,                 \
                            hctx);                                          \
}

#endif  // DO_XF_LIGHT

#ifndef WINNT
#define NV_LOCK_TEXTURE_SURFACE(pTex)
#define NV_UNLOCK_TEXTURE_SURFACE(pTex)
#else
#define NV_LOCK_TEXTURE_SURFACE(pTex) \
    pTex->lpLcl = EngLockDirectDrawSurface(pTex->hDDS)

#define NV_UNLOCK_TEXTURE_SURFACE(pTex) \
{                                            \
    EngUnlockDirectDrawSurface(pTex->lpLcl); \
    pTex->lpLcl = NULL;                      \
}
#endif // #ifdef WINNT

#endif  // _NVD3DMAC_H_
