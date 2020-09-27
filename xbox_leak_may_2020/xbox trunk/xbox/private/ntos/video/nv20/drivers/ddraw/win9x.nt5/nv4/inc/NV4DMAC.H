#ifndef _NV4DMAC_H_
#define _NV4DMAC_H_

#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4DMAC.H                                                         *
*   Define various macros used in the D3D driver.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       1/26/98 - Created                       *
*                                                                           *
\***************************************************************************/

/*
 * Defining this macro will cause the driver to use the PIO hardware registers
 * when the driver is compiled for PIO.
 * When this macro is not defined, a system memory buffer will be allocated and
 * used in place of the real hardware registers.
 */
#define D3D_HARDWARE
#define DDPUSH

/* 
 * NV Reference Manual register access definitions.
 */
#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d)
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#define REG_WR_DRF_NUM(d,r,f,n) REG_WR32(NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(d,r,f,c) REG_WR32(NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(d,r,f,n) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(d,r,f,c) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
#define REG_RD_DRF(d,r,f)       (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

/*
 * Fast floating point operations.
 */
#define nvFloor(fp,fretval)                             \
{                                                       \
    int fpbits;                                         \
    int expo;                                           \
                                                        \
    if (fp < 0.0f) fp -= 0.9999999f;                    \
    fpbits = *(int *)&fp;                               \
    if ((fpbits >= 0) && (fpbits < 0x3F800000)) {       \
        fretval = 0.0f;                                 \
    }                                                   \
    else {                                              \
        expo = 150 - ((fpbits >> 23) & 0xFF);           \
        if (expo < 1) {                                 \
            fretval = fp;                               \
        }                                               \
        else {                                          \
            fpbits &= 0xFFFFFFFF << expo;               \
            fretval = *(float *)&fpbits;                \
        }                                               \
    }                                                   \
}

/*
 * Number of DWORDS the push macros set.
 */
#define sizeSetSurfaceDmaPush   0
#define sizeSetZBufferDmaPush   0
#define sizeSetHeaderDmaPush    0

/*
 * --------------------------------------------------------------------------
 *  Define some handy macros.
 * --------------------------------------------------------------------------
 */

#ifdef  NVD3D_DX6
#define NEXTINSTRUCTION(ptr, type, num, extrabytes)                             \
    ptr = (LPD3DHAL_DP2COMMAND)((LPBYTE)ptr + sizeof(D3DHAL_DP2COMMAND) +       \
                                ((num) * sizeof(type)) + (extrabytes))

#define NEXTINSTRUCTION_S(ptr, typesize, num, extrabytes)                       \
    ptr = (LPD3DHAL_DP2COMMAND)((LPBYTE)ptr + sizeof(D3DHAL_DP2COMMAND) +       \
                                ((num) * (typesize)) + (extrabytes))

#define GET_FVF_POINTER(ptr, lpvert, mask, offset, index, stride)               \
    (ptr) = (DWORD)((((DWORD)(lpvert) + ((index) * (stride))) & (DWORD)(mask))  \
          + (offset));
#endif  // NVD3D_DX6

/*
 * Function index calculation
 *      4 fog modes - straight, supersampled, edgeAA (only for non-alphablended)
 */
#ifndef NV_FASTLOOPS
#ifdef FLOAT_W
#define _CALC_FUNCTION_INDEX(_ctx_ptr)                                                              \
                                   ((_ctx_ptr)->dwRenderState[D3DRENDERSTATE_FOGENABLE]             \
                                 ?  (_ctx_ptr)->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]          \
                                 :  0)
#else // fixed w
#define _CALC_FUNCTION_INDEX(_ctx_ptr)                                                              \
                                  (((_ctx_ptr)->dwRenderState[D3DRENDERSTATE_FOGENABLE]             \
                                 ?  (_ctx_ptr)->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]          \
                                 :  0)                                                              \
                                | (((_ctx_ptr)->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW)\
                                 ?  4                                                               \
                                 :  0))
#endif // fixed w
#define CALC_FUNCTION_INDEX(_ctx_ptr)                                                       \
{                                                                                           \
    (_ctx_ptr)->dwFunctionLookup = ((_ctx_ptr)->dwAntiAliasFlags & AA_ENABLED_MASK)         \
                                 ? NV_AA_FUNCTION_INDEX                                     \
                                 : (_CALC_FUNCTION_INDEX(_ctx_ptr));                        \
}
#else //NV_FASTLOOPS
/*
// basic flags
#define FIFLAG_FOGMODE          0x00000003
#define FIFLAG_INDEXED          0x00000004
#define FIFLAG_FVF              0x00000008
#define FIFLAG_DX6              0x00000010
#define FIFLAG_MMX              0x00000020
#define FIFLAG_MAXIMUM          0x0000003f

// flag macros
#define MODFLAGS_DX5(x)             (((x) & (FIFLAG_FOGMODE)))
#define MODFLAGS_DX5FVF(x)          (((x) & (FIFLAG_FOGMODE)) | (FIFLAG_FVF))
#define MODFLAGS_DX5INDEXED(x)      (((x) & (FIFLAG_FOGMODE)) | (FIFLAG_INDEXED))
#define MODFLAGS_DX5FVFINDEXED(x)   (((x) & (FIFLAG_FOGMODE)) | (FIFLAG_FVF | FIFLAG_INDEXED))

// aa not included
#define _CALC_FUNCTION_INDEX(_ctx,_indices,_storage)                    \
        (((_ctx)->dwRenderState[D3DRENDERSTATE_FOGENABLE] ? (_ctx)->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE] : 0)\
       | ((_inidices) ? FIFLAG_INDEXED : 0)                             \
       | ((fxfData.dwVertexType == D3DFVF_TLVERTEX) ? 0 : FIFLAG_FVF)   \
       | ((_ctx)->bUseDX6Class ? (FIFLAG_DX6 | FIFLAG_FVF) : 0)         \
       | (global.bIsMMX ? FIFLAG_MMX : 0))

// aa included
#define CALC_FUNCTION_INDEX(_ctx,_indices,_storage)              \
{                                                                \
    (_storage)  = ((_ctx)->dwAntiAliasFlags & AA_ENABLED_MASK)   \
                ? NV_AA_FUNCTION_INDEX                           \
                : _CALC_FUNCTION_INDEX(_ctx,_indices,_storage);  \
}
// code creation code
#define LOAD_FUNCTION_INDEX(_funcIndex)                                 \
{                                                                       \
    if (!fnDrawPrimitiveTabel[_funcIndex])                              \
        fnDrawPrimitiveTabel[_funcIndex] = ILCCompileCode(_funcIndex);  \
}
*/
#endif //NV_FASTLOOPS

/*
 * Need to make sure that an unfriendly mode switch didn't sneak and not cause
 * us to get re-enabled properly.
 */
#define DOS_FULLSCREEN_CHECK(pp)

/*
 * Set up all the current context state per call.  This should be as efficient as
 * possible as it is executed for every rendering entrypoint.
 */
#define SET_CURRENT_CONTEXT(pp)                                                 \
{                                                                               \
    if (!(pCurrentContext = (PNVD3DCONTEXT)(pp)->dwhContext))                   \
    {                                                                           \
        DPF("D3D: - Bad Context");                                              \
        (pp)->ddrval = D3DHAL_CONTEXT_BAD;                                      \
        return (DDHAL_DRIVER_HANDLED);                                          \
    }                                                                           \
    if (!pCurrentContext->lpLcl)                                                \
    {                                                                           \
        (pp)->ddrval = D3DHAL_CONTEXT_BAD;                                      \
        return (DDHAL_DRIVER_HANDLED);                                          \
    }                                                                           \
}

/*
 * Macro for fog table calculation. (others are elsewhere, should consolidate).
 */
#define FP_NORM_TO_BIT24(i,p)                                                \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    (i) = *(int *)&_n;                                                       \
    if ((i) >= 0x40000000)     (i) = 0xFF000000;                             \
    else if ((i) <=0x3F800000) (i) = 0;                                      \
    else (i) = ((i) << 9) & 0xFF000000;                                      \
}

/*
 * Some cool macros for use during texture swizzling.
 */

// the goal here is to interleave bits (pushing all higher order
// bits to the left as we go) until the texture width exceeds the
// height. at that point, we treat square chunks of the texture as 
// swizzled 'blocks' that are indexed linearly by the higher order
// bits above those that are interleaved. (thus, we freeze the value
// of 'shift' when bit == h) 
 
#define U_INTERLEAVE(u,uu,w,h)                              \
{                                                           \
    int bit,shift;                                          \
    u = 0;                                                  \
    if (uu != 0)                                            \
    {                                                       \
        shift = 0;                                          \
        for (bit = 1; bit < (int)(w); bit <<= 1)            \
        {                                                   \
            u |= (uu & bit) << shift;                       \
            if (bit < (int)(h)) shift++;                    \
        }                                                   \
    }                                                       \
}
#define U_UNINTERLEAVE(u,uu,w,h)                            \
{                                                           \
    int bit, shift;                                         \
    u = 0;                                                  \
    shift = 0;                                              \
    for (bit = 1; bit < (int)(w); bit << = 1)               \
    {                                                       \
        u |= (uu >> shift) & bit;                           \
        if (bit < (int)(h)) shift++;                        \
    }                                                       \
}
#define V_INTERLEAVE(v,vv,w,h)                              \
{                                                           \
    int bit,shift;                                          \
    v = 0;                                                  \
    if (vv != 0)                                            \
    {                                                       \
        shift = 0;                                          \
        for (bit = 1; bit < (int)(h); bit <<= 1)            \
        {                                                   \
            if (bit < (int)(w)) shift++;                    \
            v |= (vv & bit) << shift;                       \
        }                                                   \
    }                                                       \
}
#define V_UNINTERLEAVE(v,vv,w,h)                            \
{                                                           \
    int bit,shift;                                          \
    v = 0;                                                  \
    shift = 0;                                              \
    for (bit = 1; bit < (int)(h); bit <<= 1)                \
    {                                                       \
        if (bit < (int)(w)) shift++;                        \
        v |= (vv >> shift) & bit;                           \
    }                                                       \
}
#define U_INC_MASK(i,m,h)                                   \
{                                                           \
    int logh;                                               \
    int hh = (int)(h) - 1;                                  \
    for (logh = 0; hh != 0; hh >>= 1, logh++);              \
    m = ((1 << (2 * logh)) - 1);                            \
    i = (0xAAAAAAAA & m) + 1;                               \
    m = (0x55555555 & m) | (~m);                            \
}
#define U_INC2_MASK(i1,i2,m,h)                              \
{                                                           \
    int logh;                                               \
    int hh = (int)(h) - 1;                                  \
    for (logh = 0; hh != 0; hh >>= 1,logh++);               \
    m = ((1 << (2 * logh)) - 1);                            \
    i1 = (0xAAAAAAAA & m) + 1;                              \
    i2 = (0xAAAAAAAA & m) + 2;                              \
    m = (0x55555555 & m) | (~m);                            \
}
#define V_INC_MASK(i,m,w)                                   \
{                                                           \
    int logw;                                               \
    int ww = (int)(w) - 1;                                  \
    for (logw = 0; ww != 0; ww >>= 1, logw++);              \
    m = ((1 << (2 * logw)) - 1);                            \
    i = (0x55555555 & m) + 1;                               \
    m = (0xAAAAAAAA & m) | (~m);                            \
}
#define UV_INC(uv,i,m)      (((uv) + i) & m)
#define UV_OFFSET(uu,vv)    ((uu) | (vv))

/*
 *
 */
#define NV_D3D_GLOBAL_SETUP()                                               \
{                                                                           \
    nvDmaPushBase      = pDriverData->NvDmaPusherBufferBase;                \
    nvDmaPushOffsetMax = pDriverData->dwDmaPushOffsetMax;                   \
    nvFifo             = pDriverData->NvDmaPusherPutAddress;                \
    if (pDriverData->TwoDRenderingOccurred)                                 \
        nvDmaLastPutAddr = nvFifo;                                          \
}
#define NV_D3D_GLOBAL_SAVE()                                                \
{                                                                           \
    pDriverData->NvDmaPusherPutAddress = nvFifo;                            \
}

#define NV_D3D_CONTEXT_SETUP()                                              \
{                                                                           \
}

#if D3D_PUSH
/*
 * Hardware connection macros.
 */
#ifdef  D3D_HARDWARE
#define nvglOpenChannel(handle, address, size, fifo)                        \
{                                                                           \
    if (pDriverData->GARTLinearBase > 0)                                    \
    {                                                                       \
        (handle) = (DWORD)AllocAGPHeap(pCurrentContext->lpLcl->lpGbl->lpDD, size); \
        (address) = (handle);                                               \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        (handle)  = (DWORD)HeapCreate(HEAP_SHARED, (size), 0);              \
        (address) = (DWORD)HeapAlloc((HANDLE)(handle), HEAP_ZERO_MEMORY, (size)); \
    }                                                                       \
    (fifo)    = NvOpenDma(0, NvGetFlatDataSelector(), (int)(address), (int)((size) - 1)); \
}
#define nvglCloseChannel(handle, address, fifo)                             \
{                                                                           \
    NvClose((NvChannel *)(fifo));                                           \
    if (pDriverData->GARTLinearBase > 0)                                    \
    {                                                                       \
        FreeAGPHeap(pCurrentContext->lpLcl->lpGbl->lpDD, (handle));         \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        HeapFree((HANDLE)(handle), 0, (PVOID)(address));                    \
        HeapDestroy((HANDLE)(handle));                                      \
    }                                                                       \
    (fifo)    = 0;                                                          \
    (handle)  = (DWORD)0;                                                   \
    (address) = (DWORD)0;                                                   \
}
#else   // D3D_EMULATION
#define nvglOpenChannel(handle, address, size, fifo)                        \
{                                                                           \
    (handle)  = (DWORD)HeapCreate(HEAP_SHARED, (size), 0);                  \
    (address) = (DWORD)HeapAlloc((HANDLE)(handle), HEAP_ZERO_MEMORY, (size)); \
    (fifo)    = 0;                                                          \
}
#define nvglCloseChannel(handle, address, fifo)                             \
{                                                                           \
    HeapFree((HANDLE)(handle), 0, (PVOID)(address));                        \
    HeapDestroy((HANDLE)(handle));                                          \
    (fifo)    = 0;                                                          \
    (handle)  = (DWORD)0;                                                   \
    (address) = (DWORD)0;                                                   \
}
#endif  // D3D_EMULATION

/*
 * DMA Pusher macros
 */
#define nvglGetFreeCount(fifo, freecount, ch, size)                         \
    (freecount) = nvGetDmaFreeCount((long)(size));

#define sizeSetObject                       2
#define nvglSetObject(fifo, freecount, ch, obj)                             \
{                                                                           \
    ((DWORD *)(fifo))[0] = (0x00040000 | ((ch) << 13));                     \
    ((DWORD *)(fifo))[1] = obj;                                             \
    (DWORD *)(fifo) += sizeSetObject;                                       \
    freecount -= sizeSetObject;                                             \
}
#define sizeSetStartMethod                  1
#define nvglSetStartMethod(fifo, freecount, ch, method, methodcount)        \
{                                                                           \
    ((DWORD *)(fifo))[0] = ((((methodcount) << 2) << 16) | ((ch) << 13) | (method)); \
    (DWORD *)(fifo) += sizeSetStartMethod;                                  \
    freecount -= sizeSetStartMethod;                                        \
}
#define sizeSetData                         1
#define nvglSetData(fifo, freecount, data)                                  \
{                                                                           \
    ((DWORD *)(fifo))[0] = (data);                                          \
    (DWORD *)(fifo) += sizeSetData;                                         \
    freecount -= sizeSetData;                                               \
}

/*
 * Create Object macro.
 */
#define sizeCreateObjectMthdCnt             1
#define sizeCreateObject                    2
#define nvglCreateObject(fifo, freecount, ch, obj)                          \
{                                                                           \
    ((DWORD *)(fifo))[0] = NVFFF_CREATE((ch)) | ((sizeCreateObjectMthdCnt << 2) << 16); \
    ((DWORD *)(fifo))[1] = (obj);                                           \
    (DWORD *)(fifo) += sizeCreateObject;                                    \
    freecount -= sizeCreateObject;                                          \
}

/*
 * Destroy Object macro.
 */
#define sizeDestroyObjectMthdCnt            1
#define sizeDestroyObject                   2
#define nvglDestroyObject(fifo, freecount, ch, obj)                         \
{                                                                           \
    ((DWORD *)(fifo))[0] = NVFFF_DESTROY((ch)) | ((sizeDestroyObjectMthdCnt << 2) << 16); \
    ((DWORD *)(fifo))[1] = (obj);                                           \
    (DWORD *)(fifo) += sizeDestroyObject;                                   \
    freecount -= sizeDestroyObject;                                         \
}

/*
 * Define DX5 Triangle class macros.
 */
#define sizeSetDx5TriangleContextsMthdCnt   2
#define sizeSetDx5TriangleContexts          3
#define nvglSetDx5TriangleContexts(fifo, freecount, ch, ctxa, ctxb)         \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDx5TriangleContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV054_SET_CONTEXT_DMA_A); \
    ((DWORD *)(fifo))[1] = (ctxa);                                          \
    ((DWORD *)(fifo))[2] = (ctxb);                                          \
    (DWORD *)(fifo) += sizeSetDx5TriangleContexts;                          \
    freecount -= sizeSetDx5TriangleContexts;                                \
}
#define sizeSetDx5TriangleSurfaceMthdCnt    1
#define sizeSetDx5TriangleSurface           2
#define nvglSetDx5TriangleSurface(fifo, freecount, ch, ctxs)                \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDx5TriangleSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV054_SET_CONTEXT_SURFACES); \
    ((DWORD *)(fifo))[1] = (ctxs);                                          \
    (DWORD *)(fifo) += sizeSetDx5TriangleSurface;                           \
    freecount -= sizeSetDx5TriangleSurface;                                 \
}
#define sizeDx5TriangleStateMthdCnt         7
#define sizeDx5TriangleState                8
#define nvglDx5TriangleState(fifo, freecount, ch,                           \
                             ckey, offset, format, filter,                  \
                             blend, control, fogcolor)                      \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx5TriangleStateMthdCnt << 2) << 16) | ((ch) << 13) | NV054_COLOR_KEY); \
    ((DWORD *)(fifo))[1] = (ckey);                                          \
    ((DWORD *)(fifo))[2] = (offset);                                        \
    ((DWORD *)(fifo))[3] = (format);                                        \
    ((DWORD *)(fifo))[4] = (filter);                                        \
    ((DWORD *)(fifo))[5] = (blend);                                         \
    ((DWORD *)(fifo))[6] = (control);                                       \
    ((DWORD *)(fifo))[7] = (fogcolor);                                      \
    (DWORD *)(fifo) += sizeDx5TriangleState;                                \
    freecount -= sizeDx5TriangleState;                                      \
}

/*
 * Unused Vertex Macros - good reference
 */
/*
#define sizeDx5TriangleTLVertexMthdCnt      8
#define sizeDx5TriangleTLVertex             9
#define nvglDx5TriangleTLVertex(fifo, freecount, ch, alias, vd)             \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx5TriangleTLVertexMthdCnt << 2) << 16) | ((ch) << 13) | NV054_TLVERTEX((alias))); \
    ((DWORD *)(fifo))[1] = *(U032 *)&(vd.sx);                               \
    ((DWORD *)(fifo))[2] = *(U032 *)&(vd.sy);                               \
    ((DWORD *)(fifo))[3] = *(U032 *)&(vd.sz);                               \
    ((DWORD *)(fifo))[4] = *(U032 *)&(vd.rhw);                              \
    ((DWORD *)(fifo))[5] = (vd.color);                                      \
    ((DWORD *)(fifo))[6] = (vd.specular);                                   \
    ((DWORD *)(fifo))[7] = *(U032 *)&(vd.tu);                               \
    ((DWORD *)(fifo))[8] = *(U032 *)&(vd.tv);                               \
    (DWORD *)(fifo) += sizeDx5TriangleTLVertex;                             \
    freecount -= sizeDx5TriangleTLVertex;                                   \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),  \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular, \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                      \
}
#define nvglDx5TriangleTLVertexExpFog(fifo, freecount, ch, alias, vd)       \
{                                                                           \
    float ffog, z;                                                          \
    int   ifog, specular;                                                   \
    specular  = (vd.specular) & 0x00FFFFFF;                                 \
    FP_INV(z, (vd.rhw));                                                    \
    ffog  = z * pCurrentContext->fFogTableDensity;                          \
    FP_EXP(ffog, ffog);                                                     \
    FP_NORM_TO_BIT24(ifog, ffog);                                           \
    specular |= ifog;                                                       \
    ((DWORD *)(fifo))[0] = (((sizeDx5TriangleTLVertexMthdCnt << 2) << 16) | ((ch) << 13) | NV054_TLVERTEX((alias))); \
    ((DWORD *)(fifo))[1] = *(U032 *)&(vd.sx);                               \
    ((DWORD *)(fifo))[2] = *(U032 *)&(vd.sy);                               \
    ((DWORD *)(fifo))[3] = *(U032 *)&(vd.sz);                               \
    ((DWORD *)(fifo))[4] = *(U032 *)&(vd.rhw);                              \
    ((DWORD *)(fifo))[5] = (vd.color);                                      \
    ((DWORD *)(fifo))[6] = specular;                                        \
    ((DWORD *)(fifo))[7] = *(U032 *)&(vd.tu);                               \
    ((DWORD *)(fifo))[8] = *(U032 *)&(vd.tv);                               \
    (DWORD *)(fifo) += sizeDx5TriangleTLVertex;                             \
    freecount -= sizeDx5TriangleTLVertex;                                   \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),  \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular, \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                      \
}
#define nvglDx5TriangleTLVertexExp2Fog(fifo, freecount, ch, alias, vd)      \
{                                                                           \
    float ffog, z;                                                          \
    int   ifog, specular;                                                   \
    specular  = (vd.specular) & 0x00FFFFFF;                                 \
    FP_INV(z, (vd.rhw));                                                    \
    ffog  = z * pCurrentContext->fFogTableDensity;                          \
    ffog *= ffog;                                                           \
    FP_EXP(ffog, ffog);                                                     \
    FP_NORM_TO_BIT24(ifog, ffog);                                           \
    specular |= ifog;                                                       \
    ((DWORD *)(fifo))[0] = (((sizeDx5TriangleTLVertexMthdCnt << 2) << 16) | ((ch) << 13) | NV054_TLVERTEX((alias))); \
    ((DWORD *)(fifo))[1] = *(U032 *)&(vd.sx);                               \
    ((DWORD *)(fifo))[2] = *(U032 *)&(vd.sy);                               \
    ((DWORD *)(fifo))[3] = *(U032 *)&(vd.sz);                               \
    ((DWORD *)(fifo))[4] = *(U032 *)&(vd.rhw);                              \
    ((DWORD *)(fifo))[5] = (vd.color);                                      \
    ((DWORD *)(fifo))[6] = specular;                                        \
    ((DWORD *)(fifo))[7] = *(U032 *)&(vd.tu);                               \
    ((DWORD *)(fifo))[8] = *(U032 *)&(vd.tv);                               \
    (DWORD *)(fifo) += sizeDx5TriangleTLVertex;                             \
    freecount -= sizeDx5TriangleTLVertex;                                   \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),  \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular, \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                      \
}
#define nvglDx5TriangleTLVertexLinearFog(fifo, freecount, ch, alias, vd)    \
{                                                                           \
    float ffog, z;                                                          \
    int   ifog, specular;                                                   \
    specular  = (vd.specular) & 0x00FFFFFF;                                 \
    FP_INV(z, (vd.rhw));                                                    \
    ffog = (pCurrentContext->fFogTableEnd - z) * pCurrentContext->fFogTableLinearScale; \
    FP_NORM_TO_BIT24(ifog, ffog);                                           \
    specular |= ifog;                                                       \
    ((DWORD *)(fifo))[0] = (((sizeDx5TriangleTLVertexMthdCnt << 2) << 16) | ((ch) << 13) | NV054_TLVERTEX((alias))); \
    ((DWORD *)(fifo))[1] = *(U032 *)&(vd.sx);                               \
    ((DWORD *)(fifo))[2] = *(U032 *)&(vd.sy);                               \
    ((DWORD *)(fifo))[3] = *(U032 *)&(vd.sz);                               \
    ((DWORD *)(fifo))[4] = *(U032 *)&(vd.rhw);                              \
    ((DWORD *)(fifo))[5] = (vd.color);                                      \
    ((DWORD *)(fifo))[6] = specular;                                        \
    ((DWORD *)(fifo))[7] = *(U032 *)&(vd.tu);                               \
    ((DWORD *)(fifo))[8] = *(U032 *)&(vd.tv);                               \
    (DWORD *)(fifo) += sizeDx5TriangleTLVertex;                             \
    freecount -= sizeDx5TriangleTLVertex;                                   \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),  \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular, \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                      \
}
*/

#define sizeDx5TriangleDrawPrimitiveMthdCnt 1
#define sizeDx5TriangleDrawPrimitive        2
#define nvglDx5TriangleDrawPrimitive(fifo, freecount, ch, alias, vertices)  \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx5TriangleDrawPrimitiveMthdCnt << 2) << 16) | ((ch) << 13) | NV054_DRAW_PRIMITIVE((alias))); \
    ((DWORD *)(fifo))[1] = (vertices);                                      \
    (DWORD *)(fifo) += sizeDx5TriangleDrawPrimitive;                        \
    freecount -= sizeDx5TriangleDrawPrimitive;                              \
}
#define sizeDx5TriangleNotify               4
#define nvglDx5TriangleNotify(fifo, freecount, ch)                          \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((1 << 2) << 16) | ((ch) << 13) | NV054_NOTIFY);\
    ((DWORD *)(fifo))[1] = NV054_NOTIFY_WRITE_ONLY;                         \
    ((DWORD *)(fifo))[2] = (((1 << 2) << 16) | ((ch) << 13) | NV054_NO_OPERATION); \
    ((DWORD *)(fifo))[3] = 0x00000000;                                      \
    (DWORD *)(fifo) += sizeDx5TriangleNotify;                               \
    freecount -= sizeDx5TriangleNotify;                                     \
}

/*
 * Define DX6 Triangle class macros.
 */
#define sizeSetDx6TriangleContextsMthdCnt   2
#define sizeSetDx6TriangleContexts          3
#define nvglSetDx6TriangleContexts(fifo, freecount, ch, ctxa, ctxb)         \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDx6TriangleContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV055_SET_CONTEXT_DMA_A); \
    ((DWORD *)(fifo))[1] = (ctxa);                                          \
    ((DWORD *)(fifo))[2] = (ctxb);                                          \
    (DWORD *)(fifo) += sizeSetDx6TriangleContexts;                          \
    freecount -= sizeSetDx6TriangleContexts;                                \
}
#define sizeSetDx6TriangleSurfaceMthdCnt    1
#define sizeSetDx6TriangleSurface           2
#define nvglSetDx6TriangleSurface(fifo, freecount, ch, ctxs)                \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDx6TriangleSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV055_SET_CONTEXT_SURFACES); \
    ((DWORD *)(fifo))[1] = (ctxs);                                          \
    (DWORD *)(fifo) += sizeSetDx6TriangleSurface;                           \
    freecount -= sizeSetDx6TriangleSurface;                                 \
}
#define sizeDx6TriangleOffsetMthdCnt        1
#define sizeDx6TriangleOffset               2
#define nvglDx6TriangleOffset(fifo, freecount, ch,                          \
                              offset, buffer)                               \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx6TriangleOffsetMthdCnt << 2) << 16) | ((ch) << 13) | NV055_OFFSET(buffer)); \
    ((DWORD *)(fifo))[1] = (offset);                                        \
    (DWORD *)(fifo) += sizeDx6TriangleOffset;                               \
    freecount -= sizeDx6TriangleOffset;                                     \
}
#define sizeDx6TriangleFormatMthdCnt        1
#define sizeDx6TriangleFormat               2
#define nvglDx6TriangleFormat(fifo, freecount, ch,                          \
                              format, buffer)                               \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx6TriangleFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV055_FORMAT(buffer)); \
    ((DWORD *)(fifo))[1] = (format);                                        \
    (DWORD *)(fifo) += sizeDx6TriangleFormat;                               \
    freecount -= sizeDx6TriangleFormat;                                     \
}
#define sizeDx6TriangleFilterMthdCnt        1
#define sizeDx6TriangleFilter               2
#define nvglDx6TriangleFilter(fifo, freecount, ch,                          \
                              filter, buffer)                               \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx6TriangleFilterMthdCnt << 2) << 16) | ((ch) << 13) | NV055_FILTER(buffer)); \
    ((DWORD *)(fifo))[1] = (filter);                                        \
    (DWORD *)(fifo) += sizeDx6TriangleFilter;                               \
    freecount -= sizeDx6TriangleFilter;                                     \
}
#define sizeDx6TriangleStageState           12
#define nvglDx6TriangleStageState(fifo, freecount, ch,                      \
                                  c0alpha, c0color, c1alpha, c1color,       \
                                  factor, blend,                            \
                                  control0, control1, control2,             \
                                  fogcolor)                                 \
{                                                                           \
    ((DWORD *)(fifo))[0]  = (((2 << 2) << 16) | ((ch) << 13) | NV055_COMBINE_0_ALPHA); \
    ((DWORD *)(fifo))[1]  = (c0alpha);                                      \
    ((DWORD *)(fifo))[2]  = (c0color);                                      \
    ((DWORD *)(fifo))[3]  = (((8 << 2) << 16) | ((ch) << 13) | NV055_COMBINE_1_ALPHA); \
    ((DWORD *)(fifo))[4]  = (c1alpha);                                      \
    ((DWORD *)(fifo))[5]  = (c1color);                                      \
    ((DWORD *)(fifo))[6]  = (factor);                                       \
    ((DWORD *)(fifo))[7]  = (blend);                                        \
    ((DWORD *)(fifo))[8]  = (control0);                                     \
    ((DWORD *)(fifo))[9]  = (control1);                                     \
    ((DWORD *)(fifo))[10] = (control2);                                     \
    ((DWORD *)(fifo))[11] = (fogcolor);                                     \
    (DWORD *)(fifo) += sizeDx6TriangleStageState;                           \
    freecount -= sizeDx6TriangleStageState;                                 \
}
#define sizeDX6TriangleState    (((sizeDx6TriangleOffset + sizeDx6TriangleFormat + sizeDx6TriangleFilter) * 2) + sizeDx6TriangleStageState)

#define sizeDx6TriangleTLVertexMthdCnt      10
#define sizeDx6TriangleTLVertex1            11
#define nvglDx6TriangleTLVertex(fifo, freecount, ch, alias, vd)             \
{                                                                           \
    ((DWORD *)(fifo))[0]  = (((sizeDx6TriangleTLVertexMthdCnt << 2) << 16) | ((ch) << 13) | NV055_TLMTVERTEX((alias))); \
    ((DWORD *)(fifo))[1]  = *(U032 *)&(vd.sx);                              \
    ((DWORD *)(fifo))[2]  = *(U032 *)&(vd.sy);                              \
    ((DWORD *)(fifo))[3]  = *(U032 *)&(vd.sz);                              \
    ((DWORD *)(fifo))[4]  = *(U032 *)&(vd.rhw);                             \
    ((DWORD *)(fifo))[5]  = (vd.color);                                     \
    ((DWORD *)(fifo))[6]  = (vd.specular);                                  \
    ((DWORD *)(fifo))[7]  = *(U032 *)&(vd.tu);                              \
    ((DWORD *)(fifo))[8]  = *(U032 *)&(vd.tv);                              \
    ((DWORD *)(fifo))[9]  = 0;                                              \
    ((DWORD *)(fifo))[10] = 0;                                              \
    (DWORD *)(fifo) += sizeDx6TriangleTLVertex1;                            \
    freecount -= sizeDx6TriangleTLVertex1;                                  \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),  \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular, \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));          \
}

#define sizeDx6TriangleDrawPrimitiveMthdCnt 1
#define sizeDx6TriangleDrawPrimitive        2
#define nvglDx6TriangleDrawPrimitive(fifo, freecount, ch, alias, vertices)  \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDx6TriangleDrawPrimitiveMthdCnt << 2) << 16) | ((ch) << 13) | NV055_DRAW_PRIMITIVE((alias))); \
    ((DWORD *)(fifo))[1] = (vertices);                                      \
    (DWORD *)(fifo) += sizeDx6TriangleDrawPrimitive;                        \
    freecount -= sizeDx6TriangleDrawPrimitive;                              \
}

/*
 * Define NV4 Render Solid Rectangle class macros.
 */
#define sizeSetNv4SolidRectangleSurfaceMthdCnt  1
#define sizeSetNv4SolidRectangleSurface         2
#define nvglSetNv4SolidRectangleSurface(fifo, freecount, ch, ctx)           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetNv4SolidRectangleSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_CONTEXT_SURFACE); \
    ((DWORD *)(fifo))[1] = (ctx);                                           \
    (DWORD *)(fifo) += sizeSetNv4SolidRectangleSurface;                     \
    freecount -= sizeSetNv4SolidRectangleSurface;                           \
}

#define sizeSetNv4SolidRectangleOperationMthdCnt    1
#define sizeSetNv4SolidRectangleOperation           2
#define nvglSetNv4SolidRectangleOperation(fifo, freecount, ch, op)          \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetNv4SolidRectangleOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_OPERATION); \
    ((DWORD *)(fifo))[1] = (op);                                            \
    (DWORD *)(fifo) += sizeSetNv4SolidRectangleOperation;                   \
    freecount -= sizeSetNv4SolidRectangleOperation;                         \
}

#define sizeSetNv4SolidRectangleColorFormatMthdCnt  1
#define sizeSetNv4SolidRectangleColorFormat         2
#define nvglSetNv4SolidRectangleColorFormat(fifo, freecount, ch, cfmt)      \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetNv4SolidRectangleColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_SET_COLOR_FORMAT); \
    ((DWORD *)(fifo))[1] = (cfmt);                                          \
    (DWORD *)(fifo) += sizeSetNv4SolidRectangleColorFormat;                 \
    freecount -= sizeSetNv4SolidRectangleColorFormat;                       \
}

#define sizeNv4SolidRectangleColorMthdCnt           1
#define sizeNv4SolidRectangleColor                  2
#define nvglNv4SolidRectangleColor(fifo, freecount, ch, color)              \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeNv4SolidRectangleColorMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_COLOR); \
    ((DWORD *)(fifo))[1] = (color);                                         \
    (DWORD *)(fifo) += sizeNv4SolidRectangleColor;                          \
    freecount -= sizeNv4SolidRectangleColor;                                \
}

#define sizeNv4SolidRectanglePointSizeMthdCnt       2
#define sizeNv4SolidRectanglePointSize              3
#define nvglNv4SolidRectanglePointSize(fifo, freecount, ch,                 \
                                       x, y, width, height)                 \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeNv4SolidRectanglePointSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV05E_RECTANGLE(0)); \
    ((DWORD *)(fifo))[1] = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    ((DWORD *)(fifo))[2]  = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeNv4SolidRectanglePointSize;                      \
    freecount -= sizeNv4SolidRectanglePointSize;                            \
}

/*
 * Define Context Surfaces ARGB Zeta/Stencil macros.
 */
#define sizeSetRenderTargetContextsMthdCnt  2
#define sizeSetRenderTargetContexts         3
#define nvglSetRenderTargetContexts(fifo, freecount, ch,                    \
                                    ctxdmacolor, ctxdmazeta)                \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetRenderTargetContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV053_SET_CONTEXT_DMA_COLOR); \
    ((DWORD *)(fifo))[1] = (ctxdmacolor);                                   \
    ((DWORD *)(fifo))[2] = (ctxdmazeta);                                    \
    (DWORD *)(fifo) += sizeSetRenderTargetContexts;                         \
    freecount -= sizeSetRenderTargetContexts;                               \
}

#define sizeSetRenderTarget                 8
#define nvglSetRenderTarget(fifo, freecount, ch,                            \
                            cliphorizontal, clipvertical, format, pitch,    \
                            coloroffset, zetaoffset)                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((3 << 2) << 16) | ((ch) << 13) | NV053_SET_CLIP_HORIZONTAL); \
    ((DWORD *)(fifo))[1] = (cliphorizontal);                                \
    ((DWORD *)(fifo))[2] = (clipvertical);                                  \
    ((DWORD *)(fifo))[3] = (format);                                        \
    ((DWORD *)(fifo))[4] = (((3 << 2) << 16) | ((ch) << 13) | NV053_SET_PITCH); \
    ((DWORD *)(fifo))[5] = (pitch);                                         \
    ((DWORD *)(fifo))[6] = (coloroffset);                                   \
    ((DWORD *)(fifo))[7] = (zetaoffset);                                    \
    (DWORD *)(fifo) += sizeSetRenderTarget;                                 \
    freecount -= sizeSetRenderTarget;                                       \
}

#define sizeSetRenderTarget2MthdCnt         5
#define sizeSetRenderTarget2                6
#define nvglSetRenderTarget2(fifo, freecount, ch,                           \
                            format, clipsize, pitch,                        \
                            coloroffset, zetaoffset)                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetRenderTarget2MthdCnt << 2) << 16) | ((ch) << 13) | NV053_SET_FORMAT); \
    ((DWORD *)(fifo))[1] = (format);                                        \
    ((DWORD *)(fifo))[2] = (clipsize);                                      \
    ((DWORD *)(fifo))[3] = (pitch);                                         \
    ((DWORD *)(fifo))[4] = (coloroffset);                                   \
    ((DWORD *)(fifo))[5] = (zetaoffset);                                    \
    (DWORD *)(fifo) += sizeSetRenderTarget2;                                \
    freecount -= sizeSetRenderTarget2;                                      \
}

#define sizeSetRenderTargetViewportMthdCnt  2
#define sizeSetRenderTargetViewport         3
#define nvglSetRenderTargetViewport(fifo, freecount, ch,                    \
                            cliphorizontal, clipvertical)                   \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetRenderTargetViewportMthdCnt << 2) << 16) | ((ch) << 13) | NV053_SET_CLIP_HORIZONTAL); \
    ((DWORD *)(fifo))[1] = (cliphorizontal);                                \
    ((DWORD *)(fifo))[2] = (clipvertical);                                  \
    (DWORD *)(fifo) += sizeSetRenderTargetViewport;                         \
    freecount -= sizeSetRenderTargetViewport;                               \
}

/*
 * Swizzled surface context setup macros.
 */
#define sizeSetSwizzledSurfaceContextImageMthdCnt   1
#define sizeSetSwizzledSurfaceContextImage          2
#define nvglSetSwizzledSurfaceContextImage(fifo, freecount, ch,             \
                                     ctxdmaimage)                           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetSwizzledSurfaceContextImageMthdCnt << 2) << 16) | ((ch) << 13) | NV052_SET_CONTEXT_DMA_IMAGE); \
    ((DWORD *)(fifo))[1] = (ctxdmaimage);                                   \
    (DWORD *)(fifo) += sizeSetSwizzledSurfaceContextImage;                  \
    freecount -= sizeSetSwizzledSurfaceContextImage;                        \
}

#define sizeSetSwizzledSurfaceMthdCnt        2
#define sizeSetSwizzledSurface               3
#define nvglSetSwizzledSurface(fifo, freecount, ch,                         \
                              format, offset)                               \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetSwizzledSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV052_SET_FORMAT); \
    ((DWORD *)(fifo))[1] = (format);                                        \
    ((DWORD *)(fifo))[2] = (offset);                                        \
    (DWORD *)(fifo) += sizeSetSwizzledSurface;                              \
    freecount -= sizeSetSwizzledSurface;                                    \
}

/*
 * Indexed Image from CPU setup macros
 */
#define sizeSetIndexedImageDmaNotifiesMthdCnt           1
#define sizeSetIndexedImageDmaNotifies                  2
#define nvglSetIndexedImageDmaNotifies(fifo, freecount, ch,             \
                                      ctxdmanotify)                     \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageDmaNotifiesMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_CONTEXT_DMA_NOTIFIES); \
    ((DWORD *)(fifo))[1] = (ctxdmanotify);                              \
    (DWORD *)(fifo) += sizeSetIndexedImageDmaNotifies;                  \
    freecount -= sizeSetIndexedImageDmaNotifies;                        \
}

#define sizeSetIndexedImageContextDmaLUTMthdCnt           1
#define sizeSetIndexedImageContextDmaLUT                  2
#define nvglSetIndexedImageContextDmaLUT(fifo, freecount, ch,           \
                                         handle)                        \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageContextDmaLUTMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_CONTEXT_DMA_LUT); \
    ((DWORD *)(fifo))[1] = (handle);                                    \
    (DWORD *)(fifo) += sizeSetIndexedImageContextDmaLUT;                \
    freecount -= sizeSetIndexedImageContextDmaLUT;                      \
}

#define sizeSetIndexedImageContextSurfaceMthdCnt        1
#define sizeSetIndexedImageContextSurface               2
#define nvglSetIndexedImageContextSurface(fifo, freecount, ch,          \
                                          surface)                      \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageContextSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_CONTEXT_SURFACE); \
    ((DWORD *)(fifo))[1] = (surface);                                   \
    (DWORD *)(fifo) += sizeSetIndexedImageContextSurface;               \
    freecount -= sizeSetIndexedImageContextSurface;                     \
}

#define sizeSetIndexedImageOperationMthdCnt     1
#define sizeSetIndexedImageOperation            2
#define nvglSetIndexedImageOperation(fifo, freecount, ch,               \
                                     operation)                         \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_OPERATION); \
    ((DWORD *)(fifo))[1] = (operation);                                 \
    (DWORD *)(fifo) += sizeSetIndexedImageOperation;                    \
    freecount -= sizeSetIndexedImageOperation;                          \
}

#define sizeSetIndexedImageColorFormatMthdCnt   1
#define sizeSetIndexedImageColorFormat          2
#define nvglSetIndexedImageColorFormat(fifo, freecount, ch,             \
                                       colorformat)                     \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SET_COLOR_FORMAT); \
    ((DWORD *)(fifo))[1] = (colorformat);                               \
    (DWORD *)(fifo) += sizeSetIndexedImageColorFormat;                  \
    freecount -= sizeSetIndexedImageColorFormat;                        \
}

#define sizeSetIndexedImageIndexFormatMthdCnt   1
#define sizeSetIndexedImageIndexFormat          2
#define nvglSetIndexedImageIndexFormat(fifo, freecount, ch,             \
                                       indexformat)                     \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageIndexFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV060_INDEX_FORMAT); \
    ((DWORD *)(fifo))[1] = (indexformat);                               \
    (DWORD *)(fifo) += sizeSetIndexedImageIndexFormat;                  \
    freecount -= sizeSetIndexedImageIndexFormat;                        \
}

#define sizeSetIndexedImageLUTOffsetMthdCnt     1
#define sizeSetIndexedImageLUTOffset            2
#define nvglSetIndexedImageLUTOffset(fifo, freecount, ch,               \
                                     offset)                            \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageLUTOffsetMthdCnt << 2) << 16) | ((ch) << 13) | NV060_LUT_OFFSET); \
    ((DWORD *)(fifo))[1] = (offset);                                    \
    (DWORD *)(fifo) += sizeSetIndexedImageLUTOffset;                    \
    freecount -= sizeSetIndexedImageLUTOffset;                          \
}

#define sizeSetIndexedImagePointMthdCnt         1
#define sizeSetIndexedImagePoint                2
#define nvglSetIndexedImagePoint(fifo, freecount, ch,                   \
                                 x, y)                                  \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImagePointMthdCnt << 2) << 16) | ((ch) << 13) | NV060_POINT); \
    ((DWORD *)(fifo))[1] = ( (((x) & 0x0000ffff) << 0) |                \
                             (((y) & 0x0000ffff) << 16) );              \
    (DWORD *)(fifo) += sizeSetIndexedImagePoint;                        \
    freecount -= sizeSetIndexedImagePoint;                              \
}

#define sizeSetIndexedImageSizesMthdCnt         2
#define sizeSetIndexedImageSizes                3
#define nvglSetIndexedImageSizes(fifo, freecount, ch,                   \
                                 w_out, h_out, w_in, h_in)              \
{                                                                       \
    ((DWORD *)(fifo))[0] = (((sizeSetIndexedImageSizesMthdCnt << 2) << 16) | ((ch) << 13) | NV060_SIZE_OUT); \
    ((DWORD *)(fifo))[1] = ( (((w_out) & 0x0000ffff) << 0) |            \
                             (((h_out) & 0x0000ffff) << 16) );          \
    ((DWORD *)(fifo))[2] = ( (((w_in) & 0x0000ffff) << 0) |             \
                             (((h_in) & 0x0000ffff) << 16) );           \
    (DWORD *)(fifo) += sizeSetIndexedImageSizes;                        \
    freecount -= sizeSetIndexedImageSizes;                              \
}

#define nvglSetIndexedImageIndices(fifo, freecount, ch,                 \
                                   dataSizeInWords, indices)            \
{                                                                       \
    DWORD ii;                                                           \
    ((DWORD *)(fifo))[0] = (((dataSizeInWords << 2) << 16) | ((ch) << 13) | NV060_INDICES(0)); \
    (DWORD *)(fifo) += 1;                                               \
    for (ii=0; ii<dataSizeInWords; ii++) {                              \
        *(DWORD *)fifo = *(DWORD *)indices;                             \
        (DWORD *)fifo += 1;                                             \
        (DWORD *)indices += 1;                                          \
    }                                                                   \
    freecount -= (1 + dataSizeInWords);                                 \
}

/*
 * Scaled image from memory context setup macros.
 */
#define sizeSetScaledImageContextNotifiesMthdCnt    1
#define sizeSetScaledImageContextNotifies           2
#define nvglSetScaledImageContextNotifies(fifo, freecount, ch,              \
                                       ctxdmanotify)                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetScaledImageContextNotifiesMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_CONTEXT_DMA_NOTIFIES); \
    ((DWORD *)(fifo))[1] = (ctxdmanotify);                                  \
    (DWORD *)(fifo) += sizeSetScaledImageContextNotifies;                   \
    freecount -= sizeSetScaledImageContextNotifies;                         \
}

#define sizeSetScaledImageContextImageMthdCnt       1
#define sizeSetScaledImageContextImage              2
#define nvglSetScaledImageContextImage(fifo, freecount, ch,                 \
                                       ctxdmaimage)                         \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetScaledImageContextImageMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_CONTEXT_DMA_IMAGE); \
    ((DWORD *)(fifo))[1] = (ctxdmaimage);                                   \
    (DWORD *)(fifo) += sizeSetScaledImageContextImage;                      \
    freecount -= sizeSetScaledImageContextImage;                            \
}

#define sizeSetScaledImageContextSurfaceMthdCnt     1
#define sizeSetScaledImageContextSurface            2
#define nvglSetScaledImageContextSurface(fifo, freecount, ch,               \
                                       ctxdmasurface)                       \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetScaledImageContextSurfaceMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_CONTEXT_SURFACE); \
    ((DWORD *)(fifo))[1] = (ctxdmasurface);                                 \
    (DWORD *)(fifo) += sizeSetScaledImageContextSurface;                    \
    freecount -= sizeSetScaledImageContextSurface;                          \
}

#define sizeSetScaledImageFormatMthdCnt             1
#define sizeSetScaledImageFormat                    2
#define nvglSetScaledImageFormat(fifo, freecount, ch,                       \
                                 format)                                    \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetScaledImageFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_COLOR_FORMAT); \
    ((DWORD *)(fifo))[1] = (format);                                        \
    (DWORD *)(fifo) += sizeSetScaledImageFormat;                            \
    freecount -= sizeSetScaledImageFormat;                                  \
}

#define sizeSetScaledImageOperationMthdCnt          1
#define sizeSetScaledImageOperation                 2
#define nvglSetScaledImageOperation(fifo, freecount, ch,                    \
                                    operation)                              \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetScaledImageOperationMthdCnt << 2) << 16) | ((ch) << 13) | NV077_SET_OPERATION); \
    ((DWORD *)(fifo))[1] = (operation);                                     \
    (DWORD *)(fifo) += sizeSetScaledImageOperation;                         \
    freecount -= sizeSetScaledImageOperation;                               \
}

#define sizeScaledImageClipMthdCnt                  2
#define sizeScaledImageClip                         3
#define nvglScaledImageClip(fifo, freecount, ch,                            \
                            x, y, width, height)                            \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageClipMthdCnt << 2) << 16) | ((ch) << 13) | NV077_CLIP_POINT); \
    ((DWORD *)(fifo))[1] = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    ((DWORD *)(fifo))[2] = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeScaledImageClip;                                 \
    freecount -= sizeScaledImageClip;                                       \
}

#define sizeScaledImageOutMthdCnt                   2
#define sizeScaledImageOut                          3
#define nvglScaledImageOut(fifo, freecount, ch,                             \
                           x, y, width, height)                             \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageOutMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_OUT_POINT); \
    ((DWORD *)(fifo))[1] = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    ((DWORD *)(fifo))[2] = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeScaledImageOut;                                  \
    freecount -= sizeScaledImageOut;                                        \
}

#define sizeScaledImageDeltaDuDxDvDyMthdCnt         2
#define sizeScaledImageDeltaDuDxDvDy                3
#define nvglScaledImageDeltaDuDxDvDy(fifo, freecount, ch,                   \
                                     dudx, dvdy)                            \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageDeltaDuDxDvDyMthdCnt << 2) << 16) | ((ch) << 13) | NV077_DELTA_DU_DX); \
    ((DWORD *)(fifo))[1] = (dudx);                                          \
    ((DWORD *)(fifo))[2] = (dvdy);                                          \
    (DWORD *)(fifo) += sizeScaledImageDeltaDuDxDvDy;                        \
    freecount -= sizeScaledImageDeltaDuDxDvDy;                              \
}

#define sizeScaledImageInSizeMthdCnt                1
#define sizeScaledImageInSize                       2
#define nvglScaledImageInSize(fifo, freecount, ch,                          \
                              width, height)                                \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageInSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN_SIZE); \
    ((DWORD *)(fifo))[1] = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeScaledImageInSize;                               \
    freecount -= sizeScaledImageInSize;                                     \
}

#define sizeScaledImageInFormatMthdCnt              1
#define sizeScaledImageInFormat                     2
#define nvglScaledImageInFormat(fifo, freecount, ch,                        \
                                format)                                     \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageInFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN_FORMAT); \
    ((DWORD *)(fifo))[1] = (format);                                        \
    (DWORD *)(fifo) += sizeScaledImageInFormat;                             \
    freecount -= sizeScaledImageInFormat;                                   \
}

#define sizeScaledImageInOffsetMthdCnt              1
#define sizeScaledImageInOffset                     2
#define nvglScaledImageInOffset(fifo, freecount, ch,                        \
                                offset)                                     \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageInOffsetMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN_OFFSET); \
    ((DWORD *)(fifo))[1] = (offset);                                        \
    (DWORD *)(fifo) += sizeScaledImageInOffset;                             \
    freecount -= sizeScaledImageInOffset;                                   \
}

#define sizeScaledImageInPointMthdCnt               1
#define sizeScaledImageInPoint                      2
#define nvglScaledImageInPoint(fifo, freecount, ch,                         \
                               x, y)                                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeScaledImageInPointMthdCnt << 2) << 16) | ((ch) << 13) | NV077_IMAGE_IN); \
    ((DWORD *)(fifo))[1] = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeScaledImageInPoint;                              \
    freecount -= sizeScaledImageInPoint;                                    \
}

#define sizeScaledImageNotify                       4
#define nvglScaledImageNotify(fifo, freecount, ch)                          \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((1 << 2) << 16) | ((ch) << 13) | NV077_NOTIFY); \
    ((DWORD *)(fifo))[1] = NV077_NOTIFY_WRITE_ONLY;                         \
    ((DWORD *)(fifo))[2] = (((1 << 2) << 16) | ((ch) << 13) | NV077_NO_OPERATION); \
    ((DWORD *)(fifo))[3] = 0;                                               \
    (DWORD *)(fifo) += sizeScaledImageNotify;                               \
    freecount -= sizeScaledImageNotify;                                     \
}

/*
 * Context DMA from memory setup macros.
 */
#define sizeSetDmaFromMemoryMthdCnt                 3
#define sizeSetDmaFromMemory                        4
#define nvglSetDmaFromMemory(fifo, freecount, ch,                           \
                            base, selector, limit)                          \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDmaFromMemoryMthdCnt << 2) << 16) | ((ch) << 13) | 0x300); \
    ((DWORD *)(fifo))[1] = (base);                                          \
    ((DWORD *)(fifo))[2] = (selector);                                      \
    ((DWORD *)(fifo))[3] = (limit);                                         \
    (DWORD *)(fifo) += sizeSetDmaFromMemory;                                \
    freecount -= sizeSetDmaFromMemory;                                      \
}

#define sizeSetDmaFromMemoryContextNotifyMthdCnt    1
#define sizeSetDmaFromMemoryContextNotify           2
#define nvglSetDmaFromMemoryContextNotify(fifo, freecount, ch,              \
                                       ctxdmanotify)                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDmaFromMemoryContextNotifyMthdCnt << 2) << 16) | ((ch) << 13) | 0x180); \
    ((DWORD *)(fifo))[1] = (ctxdmanotify);                                  \
    (DWORD *)(fifo) += sizeSetDmaFromMemoryContextNotify;                   \
    freecount -= sizeSetDmaFromMemoryContextNotify;                         \
}

#define sizeSetDmaFromMemoryNotify                  7
#define nvglSetDmaFromMemoryNotify(fifo, freecount, ch,                     \
                                   base, selector, limit, notify)           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((2 << 2) << 16) | ((ch) << 13) | 0x300);       \
    ((DWORD *)(fifo))[1] = (base);                                          \
    ((DWORD *)(fifo))[2] = (selector);                                      \
    ((DWORD *)(fifo))[3] = (((1 << 2) << 16) | ((ch) << 13) | 0x104);       \
    ((DWORD *)(fifo))[4] = (notify);                                        \
    ((DWORD *)(fifo))[5] = (((1 << 2) << 16) | ((ch) << 13) | 0x308);       \
    ((DWORD *)(fifo))[6] = (limit);                                         \
    (DWORD *)(fifo) += sizeSetDmaFromMemoryNotify;                          \
    freecount -= sizeSetDmaFromMemoryNotify;                                \
}

/*
 * Context DMA to memory setup macros.
 */
#define sizeSetDmaToMemoryMthdCnt                 3
#define sizeSetDmaToMemory                        4
#define nvglSetDmaToMemory(fifo, freecount, ch,                             \
                           base, selector, limit)                           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDmaToMemoryMthdCnt << 2) << 16) | ((ch) << 13) | 0x300); \
    ((DWORD *)(fifo))[1] = (base);                                          \
    ((DWORD *)(fifo))[2] = (selector);                                      \
    ((DWORD *)(fifo))[3] = (limit);                                         \
    (DWORD *)(fifo) += sizeSetDmaToMemory;                                  \
    freecount -= sizeSetDmaToMemory;                                        \
}

#define sizeSetDmaToMemoryContextNotifyMthdCnt    1
#define sizeSetDmaToMemoryContextNotify           2
#define nvglSetDmaToMemoryContextNotify(fifo, freecount, ch,                \
                                       ctxdmanotify)                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDmaToMemoryContextNotifyMthdCnt << 2) << 16) | ((ch) << 13) | 0x180); \
    ((DWORD *)(fifo))[1] = (ctxdmanotify);                                  \
    (DWORD *)(fifo) += sizeSetDmaToMemoryContextNotify;                     \
    freecount -= sizeSetDmaToMemoryContextNotify;                           \
}

#define sizeSetDmaToMemoryNotify                  7
#define nvglSetDmaToMemoryNotify(fifo, freecount, ch,                       \
                                 base, selector, limit, notify)             \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((2 << 2) << 16) | ((ch) << 13) | 0x300);       \
    ((DWORD *)(fifo))[1] = (base);                                          \
    ((DWORD *)(fifo))[2] = (selector);                                      \
    ((DWORD *)(fifo))[3] = (((1 << 2) << 16) | ((ch) << 13) | 0x104);       \
    ((DWORD *)(fifo))[4] = (notify);                                        \
    ((DWORD *)(fifo))[5] = (((1 << 2) << 16) | ((ch) << 13) | 0x308);       \
    ((DWORD *)(fifo))[6] = (limit);                                         \
    (DWORD *)(fifo) += sizeSetDmaToMemoryNotify;                            \
    freecount -= sizeSetDmaToMemoryNotify;                                  \
}

/*
 * Context DMA in memory setup macros.
 */
#define sizeSetDmaInMemoryMthdCnt                 3
#define sizeSetDmaInMemory                        4
#define nvglSetDmaInMemory(fifo, freecount, ch,                             \
                           base, selector, limit)                           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDmaInMemoryMthdCnt << 2) << 16) | ((ch) << 13) | 0x300); \
    ((DWORD *)(fifo))[1] = (base);                                          \
    ((DWORD *)(fifo))[2] = (selector);                                      \
    ((DWORD *)(fifo))[3] = (limit);                                         \
    (DWORD *)(fifo) += sizeSetDmaInMemory;                                  \
    freecount -= sizeSetDmaInMemory;                                        \
}

#define sizeSetDmaInMemoryContextNotifyMthdCnt    1
#define sizeSetDmaInMemoryContextNotify           2
#define nvglSetDmaInMemoryContextNotify(fifo, freecount, ch,                \
                                       ctxdmanotify)                        \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetDmaInMemoryContextNotifyMthdCnt << 2) << 16) | ((ch) << 13) | 0x180); \
    ((DWORD *)(fifo))[1] = (ctxdmanotify);                                  \
    (DWORD *)(fifo) += sizeSetDmaInMemoryContextNotify;                     \
    freecount -= sizeSetDmaInMemoryContextNotify;                           \
}

#define sizeSetDmaInMemoryNotify                  7
#define nvglSetDmaInMemoryNotify(fifo, freecount, ch,                       \
                                 base, selector, limit, notify)             \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((2 << 2) << 16) | ((ch) << 13) | 0x300);       \
    ((DWORD *)(fifo))[1] = (base);                                          \
    ((DWORD *)(fifo))[2] = (selector);                                      \
    ((DWORD *)(fifo))[3] = (((1 << 2) << 16) | ((ch) << 13) | 0x104);       \
    ((DWORD *)(fifo))[4] = (notify);                                        \
    ((DWORD *)(fifo))[5] = (((1 << 2) << 16) | ((ch) << 13) | 0x308);       \
    ((DWORD *)(fifo))[6] = (limit);                                         \
    (DWORD *)(fifo) += sizeSetDmaInMemoryNotify;                            \
    freecount -= sizeSetDmaInMemoryNotify;                                  \
}

/*
 * DrawRopRectAndText macros.
 */
#define sizeDrawRopRectAndTextColor1AMthdCnt        1 
#define sizeDrawRopRectAndTextColor1A               2
#define nvglDrawRopRectAndTextColor1A(fifo, freecount, ch, color)           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDrawRopRectAndTextColor1AMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_COLOR1_A); \
    ((DWORD *)(fifo))[1] = (color);                                         \
    (DWORD *)(fifo) += sizeDrawRopRectAndTextColor1A;                       \
    freecount -= sizeDrawRopRectAndTextColor1A;                             \
}

#define sizeDrawRopRectAndTextPointSizeMthdCnt      2
#define sizeDrawRopRectAndTextPointSize             3
#define nvglDrawRopRectAndTextPointSize(fifo, freecount, ch,                \
                                        x, y, width, height)                \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeDrawRopRectAndTextPointSizeMthdCnt << 2) << 16) | ((ch) << 13) | NV04A_UNCLIPPED_RECTANGLE_POINT(0)); \
    ((DWORD *)(fifo))[1] = ((((x) & 0x0000FFFF) << 16) | ((y) & 0x0000FFFF)); \
    ((DWORD *)(fifo))[2] = ((((width) & 0x0000FFFF) << 16) | ((height) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeDrawRopRectAndTextPointSize;                     \
    freecount -= sizeDrawRopRectAndTextPointSize;                           \
}

/*
 * Surfaces2D macros.
 */
#define sizeSet2DSurfacesContextsMthdCnt            2
#define sizeSet2DSurfacesContexts                   3
#define nvglSet2DSurfacesContexts(fifo, freecount, ch, ctxsrc, ctxdest)     \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSet2DSurfacesContextsMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_CONTEXT_DMA_IMAGE_SOURCE); \
    ((DWORD *)(fifo))[1] = (ctxsrc);                                        \
    ((DWORD *)(fifo))[2] = (ctxdest);                                       \
    (DWORD *)(fifo) += sizeSet2DSurfacesContexts;                           \
    freecount -= sizeSet2DSurfacesContexts;                                 \
}

#define sizeSet2DSurfacesColorFormatMthdCnt         1
#define sizeSet2DSurfacesColorFormat                2
#define nvglSet2DSurfacesColorFormat(fifo, freecount, ch, format)           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSet2DSurfacesColorFormatMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_COLOR_FORMAT); \
    ((DWORD *)(fifo))[1] = (format);                                        \
    (DWORD *)(fifo) += sizeSet2DSurfacesColorFormat;                        \
    freecount -= sizeSet2DSurfacesColorFormat;                              \
}

#define sizeSet2DSurfacesDestinationMthdCnt         1
#define sizeSet2DSurfacesDestination                2
#define nvglSet2DSurfacesDestination(fifo, freecount, ch, offset)           \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSet2DSurfacesDestinationMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_OFFSET_DESTIN); \
    ((DWORD *)(fifo))[1] = (offset);                                        \
    (DWORD *)(fifo) += sizeSet2DSurfacesDestination;                        \
    freecount -= sizeSet2DSurfacesDestination;                              \
}

#define sizeSet2DSurfacesPitchMthdCnt               1
#define sizeSet2DSurfacesPitch                      2
#define nvglSet2DSurfacesPitch(fifo, freecount, ch,                         \
                               srcpitch, dstpitch)                          \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSet2DSurfacesPitchMthdCnt << 2) << 16) | ((ch) << 13) | NV042_SET_PITCH); \
    ((DWORD *)(fifo))[1] = ((((dstpitch) & 0x0000FFFF) << 16) | ((srcpitch) & 0x0000FFFF)); \
    (DWORD *)(fifo) += sizeSet2DSurfacesPitch;                              \
    freecount -= sizeSet2DSurfacesPitch;                                    \
}


/*
 * Set image black rectangle
 */
#define sizeSetClipMthdCnt                          2
#define sizeSetClip                                 3
#define nvglSetClip(fifo, freecount, ch,                                    \
                    x, y, w, h)                                             \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetClipMthdCnt << 2) << 16) | ((ch) << 13) | NV019_SET_POINT); \
    ((DWORD *)(fifo))[1] = ((DWORD)y << 16) | (DWORD)x;                     \
    ((DWORD *)(fifo))[2] = ((DWORD)h << 16) | (DWORD)w;                     \
    (DWORD *)(fifo) += sizeSetClip;                                         \
    freecount -= sizeSetClip;                                               \
}

/*
 * Rop5 macros.
 */
#define sizeSetRop5MthdCnt                          1
#define sizeSetRop5                                 2
#define nvglSetRop5(fifo, freecount, ch, rop5)                              \
{                                                                           \
    ((DWORD *)(fifo))[0] = (((sizeSetRop5MthdCnt << 2) << 16) | ((ch) << 13) | NV043_SET_ROP5); \
    ((DWORD *)(fifo))[1] = (rop5);                                          \
    (DWORD *)(fifo) += sizeSetRop5;                                         \
    freecount -= sizeSetRop5;                                               \
}

/*
 * Define the DMA jump operation.
 */
#define sizeDmaJump         1
#define nvglDmaJump(fifo, freecount, ch, offset)                            \
{                                                                           \
    ((DWORD *)(fifo))[0] = (0x20000000 | (offset));                         \
    (DWORD *)(fifo) += sizeDmaJump;                                         \
    freecount -= sizeDmaJump;                                               \
}

/*
 *
 */
#define nvglIdleKickoff(minsize)                                            \
{                                                                           \
}



#define DMA_PUSHER_SYNC()                                                   \
{                                                                           \
}
#else   // D3D_CPU
/*
 * Hardware connection macros.
 */
#ifdef  D3D_HARDWARE
#define nvglOpenChannel(handle, address, size, fifo)                        \
{                                                                           \
    (address) = (DWORD)NvOpen(0);                                           \
    (fifo)    = (Nv4ControlDma *)0;                                         \
}
#define nvglCloseChannel(handle, address, fifo)                             \
{                                                                           \
    NvClose((NvChannel *)(address));                                        \
    (fifo)    = 0;                                                          \
    (handle)  = (DWORD)0;                                                   \
    (address) = (DWORD)0;                                                   \
}
#else   // D3D_EMULATION
#define nvglOpenChannel(handle, address, size, fifo)                        \
{                                                                           \
    (handle)  = (DWORD)HeapCreate(HEAP_SHARED, sizeof(Nv03ChannelPio), 0);  \
    (address) = (DWORD)HeapAlloc((HANDLE)(handle), HEAP_ZERO_MEMORY, sizeof(Nv03ChannelPio)); \
    (fifo)    = (Nv4ControlDma *)0;                                         \
}
#define nvglCloseChannel(handle, address, fifo)                             \
{                                                                           \
    HeapFree((HANDLE)(handle), 0, (PVOID)(address));                        \
    HeapDestroy((HANDLE)(handle));                                          \
    (fifo)    = 0;                                                          \
    (handle)  = (DWORD)0;                                                   \
    (address) = (DWORD)0;                                                   \
}
#endif  // D3D_EMULATION

/*
 * CPU Pushing macros.
 */
#ifdef  D3D_HARDWARE
#define nvglGetFreeCount(fifo, freecount, ch, size, retval)                 \
{                                                                           \
    (freecount) = ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].control.Free;\
    (retval) = FALSE;                                                       \
}
#else   // D3D_EMULATION
#define nvglGetFreeCount(fifo, freecount, ch, size, retval)                 \
{                                                                           \
    nvGetDmaFreeCount(size);                                                \
    (retval) = FALSE;                                                       \
}
#endif  // D3D_EMULATION.

#define sizeSetObjectMthdCnt    1
#define sizeSetObject           (sizeSetObjectMthdCnt * 4)
#define nvglSetObject(fifo, freecount, ch, obj)                             \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].SetObject = (obj);         \
    freecount -= sizeSetObject;                                             \
}

/*
 * Multiple data setting macros don't convert well from DMA to CPU pushing
 * methodologies.  These will need to be special cased.
 */
#define sizeSetStartMethod   0
#define nvglSetStartMethod(fifo, freecount, ch, method, methodcount)
#define sizeSetData          0
#define nvglSetData(fifo, freecount, data)

/*
 * Create Object macro.
 */
#define sizeCreateObjectMthdCnt             1
#define sizeCreateObject                    (sizeCreateObjectMthdCnt * 4)
#define nvglCreateObject(fifo, freecount, ch, obj)                          \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nvClass.Create = (obj);    \
    freecount -= sizeCreateObject;                                          \
}

/*
 * Destroy Object macro.
 */
#define sizeDestroyObjectMthdCnt            1
#define sizeDestroyObject                   (sizeDestroyObjectMthdCnt * 4)
#define nvglDestroyObject(fifo, freecount, ch, obj)                         \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nvClass.Destroy = (obj);   \
    freecount -= sizeDestroyObject;                                         \
}

/*
 * Define DX5 Triangle class macros.
 */
#define sizeSetDx5TriangleContextsMthdCnt   2
#define sizeSetDx5TriangleContexts          (sizeSetDx5TriangleContextsMthdCnt * 4)
#define nvglSetDx5TriangleContexts(fifo, freecount, ch, ctxa, ctxb)         \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.SetContextDmaA = (ctxa); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.SetContextDmaB = (ctxb); \
    freecount -= sizeSetDx5TriangleContexts;                                \
}
#define sizeSetDx5TriangleSurfaceMthdCnt    1
#define sizeSetDx5TriangleSurface           (sizeSetDx5TriangleSurfaceMthdCnt * 4)
#define nvglSetDx5TriangleSurface(fifo, freecount, ch, ctxs)                \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.SetContextSurfaces = (ctxs); \
    freecount -= sizeSetDx5TriangleSurface;                                 \
}
#define sizeDx5TriangleStateMthdCnt         7
#define sizeDx5TriangleState                (sizeDx5TriangleStateMthdCnt * 4)
#define nvglDx5TriangleState(fifo, freecount, ch,                           \
                             ckey, offset, format, filter,                  \
                             blend, control, fogcolor)                      \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.ColorKey = (ckey);     \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Offset   = (offset);   \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Format   = (format);   \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Filter   = (filter);   \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Blend    = (blend);    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Control  = (control);  \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.FogColor = (fogcolor); \
    freecount -= sizeDx5TriangleState;                                      \
}
#define sizeDx5TriangleTLVertexMthdCnt      8
#define sizeDx5TriangleTLVertex             (sizeDx5TriangleTLVertexMthdCnt * 4)
#define nvglDx5TriangleTLVertex(fifo, freecount, ch, alias, vd)                                                   \
{                                                                                                                 \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sx       = vd.sx;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sy       = vd.sy;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sz       = vd.sz;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].rhw      = vd.rhw;      \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].color    = vd.color;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].specular = vd.specular; \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tu       = vd.tu;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tv       = vd.tv;       \
    freecount -= sizeDx5TriangleTLVertex;                                                                         \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),                            \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular,                           \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                                                \
}
#define nvglDx5TriangleTLVertexExpFog(fifo, freecount, ch, alias, vd)                                             \
{                                                                                                                 \
    float ffog, z;                                                                                                \
    int   ifog, specular;                                                                                         \
    specular  = (vd.specular) & 0x00FFFFFF;                                                                       \
    FP_INV(z, (vd.rhw));                                                                                          \
    ffog  = z * pCurrentContext->fFogTableDensity;                                                                \
    FP_EXP(ffog, ffog);                                                                                           \
    FP_NORM_TO_BIT24(ifog, ffog);                                                                                 \
    specular |= ifog;                                                                                             \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sx       = vd.sx;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sy       = vd.sy;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sz       = vd.sz;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].rhw      = vd.rhw;      \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].color    = vd.color;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].specular = specular;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tu       = vd.tu;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tv       = vd.tv;       \
    freecount -= sizeDx5TriangleTLVertex;                                                                         \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),                            \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular,                           \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                                                \
}
#define nvglDx5TriangleTLVertexExp2Fog(fifo, freecount, ch, alias, vd)                                            \
{                                                                                                                 \
    float ffog, z;                                                                                                \
    int   ifog, specular;                                                                                         \
    specular  = (vd.specular) & 0x00FFFFFF;                                                                       \
    FP_INV(z, (vd.rhw));                                                                                          \
    ffog  = z * pCurrentContext->fFogTableDensity;                                                                \
    ffog *= ffog;                                                                                                 \
    FP_EXP(ffog, ffog);                                                                                           \
    FP_NORM_TO_BIT24(ifog, ffog);                                                                                 \
    specular |= ifog;                                                                                             \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sx       = vd.sx;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sy       = vd.sy;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sz       = vd.sz;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].rhw      = vd.rhw;      \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].color    = vd.color;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].specular = specular;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tu       = vd.tu;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tv       = vd.tv;       \
    freecount -= sizeDx5TriangleTLVertex;                                                                         \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),                            \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular,                           \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                                                \
}
#define nvglDx5TriangleTLVertexLinearFog(fifo, freecount, ch, alias, vd)                                          \
{                                                                                                                 \
    float ffog, z;                                                                                                \
    int   ifog, specular;                                                                                         \
    specular  = (vd.specular) & 0x00FFFFFF;                                                                       \
    FP_INV(z, (vd.rhw));                                                                                          \
    ffog = (pCurrentContext->fFogTableEnd - z) * pCurrentContext->fFogTableLinearScale;                           \
    FP_NORM_TO_BIT24(ifog, ffog);                                                                                 \
    specular |= ifog;                                                                                             \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sx       = vd.sx;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sy       = vd.sy;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].sz       = vd.sz;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].rhw      = vd.rhw;      \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].color    = vd.color;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].specular = specular;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tu       = vd.tu;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Tlvertex[(alias)].tv       = vd.tv;       \
    freecount -= sizeDx5TriangleTLVertex;                                                                         \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),                            \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular,                           \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                                                \
}
#define sizeDx5TriangleDrawPrimitiveMthdCnt 1
#define sizeDx5TriangleDrawPrimitive        (sizeDx5TriangleDrawPrimitiveMthdCnt * 4)
#define nvglDx5TriangleDrawPrimitive(fifo, freecount, ch, alias, vertices)  \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.DrawPrimitive[(alias)] = (vertices); \
    freecount -= sizeDx5TriangleDrawPrimitive;                              \
}
#define sizeDx5TriangleNotifyMthdCnt        2
#define sizeDx5TriangleNotify               (sizeDx5TriangleNotifyMthdCnt * 4)
#define nvglDx5TriangleNotify(fifo, freecount, ch)                          \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.Notify      = NV054_NOTIFY_WRITE_ONLY; \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx5TexturedTriangle.NoOperation = 0x00000000;              \
    freecount -= sizeDx5TriangleNotify;                                     \
}

/*
 * Define DX6 Triangle class macros.
 */
#define sizeSetDx6TriangleContextsMthdCnt   2
#define sizeSetDx6TriangleContexts          (sizeSetDx6TriangleContextsMthdCnt * 4)
#define nvglSetDx6TriangleContexts(fifo, freecount, ch, ctxa, ctxb)         \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6MultiTexturedTriangle.SetContextDmaA = (ctxa); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6MultiTexturedTriangle.SetContextDmaB = (ctxb); \
    freecount -= sizeSetDx6TriangleContexts;                                \
}
#define sizeSetDx6TriangleSurfaceMthdCnt    1
#define sizeSetDx6TriangleSurface           (sizeSetDx6TriangleSurfaceMthdCnt * 4)
#define nvglSetDx6TriangleSurface(fifo, freecount, ch, ctxs)                \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6MultiTexturedTriangle.SetContextSurfaces = (ctxs); \
    freecount -= sizeSetDx6TriangleSurface;                                 \
}
#define sizeDx6TriangleOffsetMthdCnt        1
#define sizeDx6TriangleOffset               (sizeDx6TriangleOffsetMthdCnt * 4)
#define nvglDx6TriangleOffset(fifo, freecount, ch,                          \
                              offset, buffer)                               \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Offset[buffer] = (offset); \
    freecount -= sizeDx6TriangleOffset;                                     \
}
#define sizeDx6TriangleFormatMthdCnt        1
#define sizeDx6TriangleFormat               (sizeDx6TriangleFormatMthdCnt * 4)
#define nvglDx6TriangleFormat(fifo, freecount, ch,                          \
                              format, buffer)                               \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Format[buffer] = (format); \
    freecount -= sizeDx6TriangleFormat;                                     \
}
#define sizeDx6TriangleFilterMthdCnt        1
#define sizeDx6TriangleFilter               (sizeDx6TriangleFormatMthdCnt * 4)
#define nvglDx6TriangleFilter(fifo, freecount, ch,                          \
                              filter, buffer)                               \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Filter[buffer] = (filter); \
    freecount -= sizeDx6TriangleFilter;                                     \
}


#define sizeDx6TriangleStageStateMthdCnt    1
#define sizeDx6TriangleStageState           (sizeDx6TriangleStageStateMthdCnt * 4)
#define nvglDx6TriangleStageState(fifo, freecount, ch,                      \
                                  c0alpha, c0color, c1alpha, c1color,       \
                                  factor, blend,                            \
                                  control0, control1, control2,             \
                                  fogcolor)                                 \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Combine0Alpha = (c0alpha); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Combine0Color = (c0color); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Combine1Alpha = (c1alpha); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Combine1Color = (c1color); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.CombineFactor = (factor);  \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Blend = (blend);           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Control0 = (control0);     \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Control1 = (control1);     \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Control2 = (control2);     \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.FogColor = (fogcolor);     \
    freecount -= sizeDx6TriangleStageState;                                 \
}
#define sizeDx6TriangleTLVertexMthdCnt      8
#define sizeDx6TriangleTLVertex             (sizeDx6TriangleTLVertexMthdCnt * 4)
#define nvglDx6TriangleTLVertex(fifo, freecount, ch, alias, vd)                                                   \
{                                                                                                                 \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].sx       = vd.sx;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].sy       = vd.sy;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].sz       = vd.sz;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].rhw      = vd.rhw;      \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].color    = vd.color;    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].specular = vd.specular; \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].tu0      = vd.tu;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].tv0      = vd.tv;       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].tu1      = 0;           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.Tlvertex[(alias)].tv1      = 0;           \
    freecount -= sizeDx6TriangleTLVertex;                                                                         \
    dbgDisplayVertexData(*(DWORD *)&(vd.sx),  *(DWORD *)&(vd.sy),  *(DWORD *)&(vd.sz),                            \
                         *(DWORD *)&(vd.rhw),  (DWORD)vd.color,     (DWORD)vd.specular,                           \
                         *(DWORD *)&(vd.tu),  *(DWORD *)&(vd.tv));                                                \
}
#define sizeDx6TriangleDrawPrimitiveMthdCnt 1
#define sizeDx6TriangleDrawPrimitive        (sizeDx6TriangleDrawPrimitiveMthdCnt * 4)
#define nvglDx6TriangleDrawPrimitive(fifo, freecount, ch, alias, vertices)  \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4Dx6TexturedTriangle.DrawPrimitive[(alias)] = (vertices); \
    freecount -= sizeDx6TriangleDrawPrimitive;                              \
}

/*
 * Define NV4 Render Solid Rectangle class macros.
 */
#define sizeSetNv4SolidRectangleSurfaceMthdCnt  1
#define sizeSetNv4SolidRectangleSurface         (sizeSetNv4SolidRectangleSurfaceMthdCnt * 4)
#define nvglSetNv4SolidRectangleSurface(fifo, freecount, ch, ctx)           \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4RenderSolidRectangle.SetContextSurface = (ctx); \
    freecount -= sizeSetNv4SolidRectangleSurface;                           \
}

#define sizeSetNv4SolidRectangleOperationMthdCnt    1
#define sizeSetNv4SolidRectangleOperation           (sizeSetNv4SolidRectangleOperationMthdCnt * 4)
#define nvglSetNv4SolidRectangleOperation(fifo, freecount, ch, op)          \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4RenderSolidRectangle.SetOperation = (op); \
    freecount -= sizeSetNv4SolidRectangleOperation;                         \
}

#define sizeSetNv4SolidRectangleColorFormatMthdCnt  1
#define sizeSetNv4SolidRectangleColorFormat         (sizeSetNv4SolidRectangleColorFormatMthdCnt * 4)
#define nvglSetNv4SolidRectangleColorFormat(fifo, freecount, ch, cfmt)      \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4RenderSolidRectangle.SetColorFormat = (cfmt); \
    freecount -= sizeSetNv4SolidRectangleColorFormat;                       \
}

#define sizeNv4SolidRectangleColorMthdCnt           1
#define sizeNv4SolidRectangleColor                  (sizeNv4SolidRectangleColorMthdCnt * 4)
#define nvglNv4SolidRectangleColor(fifo, freecount, ch, color)              \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4RenderSolidRectangle.Color = (color); \
    freecount -= sizeNv4SolidRectangleColor;                                \
}

#define sizeNv4SolidRectanglePointSizeMthdCnt       2
#define sizeNv4SolidRectanglePointSize              (sizeNv4SolidRectanglePointSizeMthdCnt * 4)
#define nvglNv4SolidRectanglePointSize(fifo, freecount, ch,                 \
                                       x, y, width, height)                 \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4RenderSolidRectangle.Rectangle[0].point = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4RenderSolidRectangle.Rectangle[0].size  = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    freecount -= sizeNv4SolidRectanglePointSize;                            \
}

/*
 * Define Context Surfaces ARGB Zeta/Stencil macros.
 */
#define sizeSetRenderTargetContextsMthdCnt  2
#define sizeSetRenderTargetContexts         (sizeSetRenderTargetContextsMthdCnt * 4)
#define nvglSetRenderTargetContexts(fifo, freecount, ch,                    \
                                    ctxdmacolor, ctxdmazeta)                \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetContextDmaColor = (ctxdmacolor);                        \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetContextDmaZeta  = (ctxdmazeta);                         \
    freecount -= sizeSetRenderTargetContexts;                               \
}

#define sizeSetRenderTargetMthdCnt          5
#define sizeSetRenderTarget                 (sizeSetRenderTargetMthdCnt * 4)
#define nvglSetRenderTarget(fifo, freecount, ch,                            \
                            format, clipsize, pitch,                        \
                            coloroffset, zetaoffset)                        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetFormat      = (format);      \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetClipSize    = (clipsize);    \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetPitch       = (pitch);       \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetOffsetColor = (coloroffset); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfacesArgbZs.SetOffsetZeta  = (zetaoffset);  \
    freecount -= sizeSetRenderTarget;                                       \
}

/*
 * Swizzled surface context setup macros.
 */
#define sizeSetSwizzledSurfaceContextImageMthdCnt   1
#define sizeSetSwizzledSurfaceContextImage      (sizeSetSwizzledSurfaceContextImageMthdCnt * 4)
#define nvglSetSwizzledSurfaceContextImage(fifo, freecount, ch,             \
                                          ctxdmaimage)                      \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaceSwizzled.SetContextDmaImage = (ctxdmaimage); \
    freecount -= sizeSetSwizzledSurfaceContextImage;                        \
}

#define sizeSetSwizzledSurfaceMthdCnt               2
#define sizeSetSwizzledSurface                      (sizeSetSwizzledSurfaceMthdCnt * 4)
#define nvglSetSwizzledSurface(fifo, freecount, ch,                         \
                              format, offset)                               \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaceSwizzled.SetFormat = (format); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaceSwizzled.SetOffset = (offset); \
    freecount -= sizeSetSwizzledSurface;                                    \
}

/*
 * Scaled image from memory context setup macros.
 */
#define sizeSetScaledImageContextNotifiesMthdCnt     1
#define sizeSetScaledImageContextNotifies           (sizeSetScaledImageContextNotifiesMthdCnt * 4)
#define nvglSetScaledImageContextNotifies(fifo, freecount, ch,              \
                                       ctxdmanotify)                        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.SetContextDmaNotifies = (ctxdmanotify); \
    freecount -= sizeSetScaledImageContextNotifies;                         \
}

#define sizeSetScaledImageContextImageMthdCnt       1
#define sizeSetScaledImageContextImage              (sizeSetScaledImageContextImageMthdCnt * 4)
#define nvglSetScaledImageContextImage(fifo, freecount, ch,                 \
                                       ctxdmaimage)                         \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.SetContextDmaImage = (ctxdmaimage); \
    freecount -= sizeSetScaledImageContextImage;                            \
}

#define sizeSetScaledImageContextSurfaceMthdCnt     1
#define sizeSetScaledImageContextSurface            (sizeSetScaledImageContextSurfaceMthdCnt * 4)
#define nvglSetScaledImageContextSurface(fifo, freecount, ch,               \
                                       ctxdmasurface)                       \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.SetContextSurface = (ctxdmasurface); \
    freecount -= sizeSetScaledImageContextSurface;                          \
}

#define sizeSetScaledImageFormatMthdCnt             1
#define sizeSetScaledImageFormat                    (sizeSetScaledImageFormatMthdCnt * 4)
#define nvglSetScaledImageFormat(fifo, freecount, ch,                       \
                                 format)                                    \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.SetColorFormat = (format); \
    freecount -= sizeSetScaledImageFormat;                                  \
}

#define sizeSetScaledImageOperationMthdCnt          1
#define sizeSetScaledImageOperation                 (sizeSetScaledImageOperationMthdCnt * 4)
#define nvglSetScaledImageOperation(fifo, freecount, ch,                    \
                                    operation)                              \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.SetOperation = (operation); \
    freecount -= sizeSetScaledImageOperation;                               \
}



#define sizeScaledImageClipMthdCnt                  2
#define sizeScaledImageClip                         (sizeScaledImageClipMthdCnt * 4)
#define nvglScaledImageClip(fifo, freecount, ch,                            \
                            x, y, width, height)                            \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ClipPoint = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ClipSize  = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    freecount -= sizeScaledImageClip;                                       \
}

#define sizeScaledImageOutMthdCnt                   2
#define sizeScaledImageOut                          (sizeScaledImageOutMthdCnt * 4)
#define nvglScaledImageOut(fifo, freecount, ch,                             \
                           x, y, width, height)                             \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ImageOutPoint = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ImageOutSize  = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    freecount -= sizeScaledImageOut;                                        \
}

#define sizeScaledImageDeltaDuDxDvDyMthdCnt         2
#define sizeScaledImageDeltaDuDxDvDy                (sizeScaledImageDeltaDuDxDvDyMthdCnt * 4)
#define nvglScaledImageDeltaDuDxDvDy(fifo, freecount, ch,                   \
                                     dudx, dvdy)                            \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.DeltaDuDx = (dudx); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.DeltaDvDy = (dvdy); \
    freecount -= sizeScaledImageDeltaDuDxDvDy;                              \
}

#define sizeScaledImageInSizeMthdCnt                1
#define sizeScaledImageInSize                       (sizeScaledImageInSizeMthdCnt * 4)
#define nvglScaledImageInSize(fifo, freecount, ch,                          \
                              width, height)                                \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ImageInSize = ((((height) & 0x0000FFFF) << 16) | ((width) & 0x0000FFFF)); \
    freecount -= sizeScaledImageInSize;                                     \
}

#define sizeScaledImageInFormatMthdCnt              1
#define sizeScaledImageInFormat                     (sizeScaledImageInFormatMthdCnt * 4)
#define nvglScaledImageInFormat(fifo, freecount, ch,                        \
                                format)                                     \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ImageInFormat = (format); \
    freecount -= sizeScaledImageInFormat;                                   \
}

#define sizeScaledImageInOffsetMthdCnt              1
#define sizeScaledImageInOffset                     (sizeScaledImageInOffsetMthdCnt * 4)
#define nvglScaledImageInOffset(fifo, freecount, ch,                        \
                                offset)                                     \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ImageInOffset = (offset); \
    freecount -= sizeScaledImageInOffset;                                   \
}

#define sizeScaledImageInPointMthdCnt               1
#define sizeScaledImageInPoint                      (sizeScaledImageInPointMthdCnt * 4)
#define nvglScaledImageInPoint(fifo, freecount, ch,                         \
                               x, y)                                        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.ImageInPoint = ((((y) & 0x0000FFFF) << 16) | ((x) & 0x0000FFFF)); \
    freecount -= sizeScaledImageInPoint;                                    \
}

#define sizeScaledImageNotifyMthdCnt                2
#define sizeScaledImageNotify                       (sizeScaledImageNotifyMthdCnt * 4)
#define nvglScaledImageNotify(fifo, freecount, ch)                          \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.Notify = NV077_NOTIFY_WRITE_ONLY; \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ScaledImageFromMemory.NoOperation = 0; \
    freecount -= sizeScaledImageNotify;                                     \
}

/*
 * Context DMA from memory setup macros.
 */
#define sizeSetDmaFromMemoryMthdCnt                 3
#define sizeSetDmaFromMemory                        (sizeSetDmaFromMemoryMthdCnt * 4)
#define nvglSetDmaFromMemory(fifo, freecount, ch,                           \
                            base, selector, ctxlimit)                       \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetDmaSpecifier.address[0] = (base); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetDmaSpecifier.address[1] = (selector); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetDmaSpecifier.limit = (ctxlimit); \
    freecount -= sizeSetDmaFromMemory;                                      \
}

#define sizeSetDmaFromMemoryContextNotifyMthdCnt    1
#define sizeSetDmaFromMemoryContextNotify           (sizeSetDmaFromMemoryContextNotifyMthdCnt * 4)
#define nvglSetDmaFromMemoryContextNotify(fifo, freecount, ch,              \
                                       ctxdmanotify)                        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetNotifyCtxDma = (ctxdmanotify); \
    freecount -= sizeSetDmaFromMemoryContextNotify;                         \
}

#define sizeSetDmaFromMemoryNotifyMthdCnt           4
#define sizeSetDmaFromMemoryNotify                  (sizeSetDmaFromMemoryNotifyMthdCnt * 4)
#define nvglSetDmaFromMemoryNotify(fifo, freecount, ch,                     \
                                   base, selector, ctxlimit, notify)        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetDmaSpecifier.address[0] = (base); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetDmaSpecifier.address[1] = (selector); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetNotify = (notify); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaFromMemory.SetDmaSpecifier.limit = (ctxlimit); \
    freecount -= sizeSetDmaFromMemoryNotify;                                \
}

/*
 * Context DMA to memory setup macros.
 */
#define sizeSetDmaToMemoryMthdCnt                   3
#define sizeSetDmaToMemory                          (sizeSetDmaToMemoryMthdCnt * 4)
#define nvglSetDmaToMemory(fifo, freecount, ch,                             \
                            base, selector, ctxlimit)                       \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetDmaSpecifier.address[0] = (base); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetDmaSpecifier.address[1] = (selector); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetDmaSpecifier.limit = (ctxlimit); \
    freecount -= sizeSetDmaToMemory;                                        \
}

#define sizeSetDmaToMemoryContextNotifyMthdCnt      1
#define sizeSetDmaToMemoryContextNotify             (sizeSetDmaToMemoryContextNotifyMthdCnt * 4)
#define nvglSetDmaToMemoryContextNotify(fifo, freecount, ch,                \
                                       ctxdmanotify)                        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetNotifyCtxDma = (ctxdmanotify); \
    freecount -= sizeSetDmaToMemoryContextNotify;                           \
}

#define sizeSetDmaToMemoryNotifyMthdCnt             4
#define sizeSetDmaToMemoryNotify                    (sizeSetDmaToMemoryNotifyMthdCnt * 4)
#define nvglSetDmaToMemoryNotify(fifo, freecount, ch,                       \
                                   base, selector, ctxlimit, notify)        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetDmaSpecifier.address[0] = (base); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetDmaSpecifier.address[1] = (selector); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetNotify = (notify); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaToMemory.SetDmaSpecifier.limit = (ctxlimit); \
    freecount -= sizeSetDmaToMemoryNotify;                                  \
}

/*
 * Context DMA in memory setup macros.
 */
#define sizeSetDmaInMemoryMthdCnt                   3
#define sizeSetDmaInMemory                          (sizeSetDmaInMemoryMthdCnt * 4)
#define nvglSetDmaInMemory(fifo, freecount, ch,                             \
                            base, selector, ctxlimit)                       \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetDmaSpecifier.address[0] = (base); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetDmaSpecifier.address[1] = (selector); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetDmaSpecifier.limit = (ctxlimit); \
    freecount -= sizeSetDmaToMemory;                                        \
}

#define sizeSetDmaInMemoryContextNotifyMthdCnt      1
#define sizeSetDmaInMemoryContextNotify             (sizeSetDmaInMemoryContextNotifyMthdCnt * 4)
#define nvglSetDmaInMemoryContextNotify(fifo, freecount, ch,                \
                                       ctxdmanotify)                        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetNotifyCtxDma = (ctxdmanotify); \
    freecount -= sizeSetDmaInMemoryContextNotify;                           \
}

#define sizeSetDmaInMemoryNotifyMthdCnt             4
#define sizeSetDmaInMemoryNotify                    (sizeSetDmaInMemoryNotifyMthdCnt * 4)
#define nvglSetDmaInMemoryNotify(fifo, freecount, ch,                       \
                                   base, selector, ctxlimit, notify)        \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetDmaSpecifier.address[0] = (base); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetDmaSpecifier.address[1] = (selector); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetNotify = (notify); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].contextDmaInMemory.SetDmaSpecifier.limit = (ctxlimit); \
    freecount -= sizeSetDmaInMemoryNotify;                                  \
}

/*
 * DrawRopRectAndText macros.
 */
#define sizeDrawRopRectAndTextColor1AMthdCnt        1 
#define sizeDrawRopRectAndTextColor1A               (sizeDrawRopRectAndTextColor1AMthdCnt * 4)
#define nvglDrawRopRectAndTextColor1A(fifo, freecount, ch, color)           \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4GdiRectangleText.Color1A = (color); \
    freecount -= sizeDrawRopRectAndTextColor1A;                             \
}

#define sizeDrawRopRectAndTextPointSizeMthdCnt      2
#define sizeDrawRopRectAndTextPointSize             (sizeDrawRopRectAndTextPointSizeMthdCnt * 4)
#define nvglDrawRopRectAndTextPointSize(fifo, freecount, ch,                \
                                        x, y, width, height)                \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4GdiRectangleText.UnclippedRectangle[0].point = ((((x) & 0x0000FFFF) << 16) | ((y) & 0x0000FFFF)); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4GdiRectangleText.UnclippedRectangle[0].size  = ((((width) & 0x0000FFFF) << 16) | ((height) & 0x0000FFFF)); \
    freecount -= sizeDrawRopRectAndTextPointSize;                           \
}

/*
 * Surfaces2D macros.
 */
#define sizeSet2DSurfacesContextsMthdCnt            2
#define sizeSet2DSurfacesContexts                   (sizeSet2DSurfacesContextsMthdCnt * 4)
#define nvglSet2DSurfacesContexts(fifo, freecount, ch, ctxsrc, ctxdest)     \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaces2D.SetContextDmaImageSource = (ctxsrc); \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaces2D.SetContextDmaImageDestin = (ctxdest); \
    freecount -= sizeSet2DSurfacesContexts;                                 \
}

#define sizeSet2DSurfacesColorFormatMthdCnt         1
#define sizeSet2DSurfacesColorFormat                (sizeSet2DSurfacesColorFormatMthdCnt * 4)
#define nvglSet2DSurfacesColorFormat(fifo, freecount, ch, format)           \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaces2D.SetColorFormat = (format); \
    freecount -= sizeSet2DSurfacesColorFormat;                              \
}

#define sizeSet2DSurfacesDestinationMthdCnt         1
#define sizeSet2DSurfacesDestination                (sizeSet2DSurfacesDestinationMthdCnt * 4)
#define nvglSet2DSurfacesDestination(fifo, freecount, ch, offset)           \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaces2D.SetOffsetDestin = (offset); \
    freecount -= sizeSet2DSurfacesDestination;                              \
}

#define sizeSet2DSurfacesPitchMthdCnt               1
#define sizeSet2DSurfacesPitch                      (sizeSet2DSurfacesPitchMthdCnt * 4)
#define nvglSet2DSurfacesPitch(fifo, freecount, ch,                         \
                               srcpitch, dstpitch)                          \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv4ContextSurfaces2D.SetPitch = ((((dstpitch) & 0x0000FFFF) << 16) | ((srcpitch) & 0x0000FFFF)); \
    freecount -= sizeSet2DSurfacesPitch;                                    \
}

/*
 * Rop5 macros.
 */
#define sizeSetRop5MthdCnt                          1
#define sizeSetRop5                                 (sizeSetRop5MthdCnt * 4)
#define nvglSetRop5(fifo, freecount, ch, rop5)                              \
{                                                                           \
    ((Nv03ChannelPio *)(fifo))->subchannel[(ch)].nv3ContextRop.SetRop5 = (rop5); \
    freecount -= sizeSetRop5;                                               \
}

/*
 * Define the DMA jump operation.
 */
#define sizeDmaJump         0
#define nvglDmaJump(fifo, freecount, ch, offset)                            \
{                                                                           \
}

/*
 *
 */
#define nvglIdleKickoff(minsize)                                            \
{                                                                           \
}

#define DMA_PUSHER_SYNC()                                                   \
{                                                                           \
}
#endif  // D3D_CPU

#endif  // NV4
#endif  // _NV4DMAC_H_
