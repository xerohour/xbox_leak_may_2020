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
//  Module: nvD3DMac.h
//      Define various macros used in the D3D driver.
//
//      THESE SHOULD BE STAND-ALONE MACROS. THERE SHOULD BE NO
//      DEPENDENCIES ON OTHER NV HEADER FILES!!
//
// **************************************************************************
//
//  History:
//      Charles Inman (chasi)       01/31/98 - Created.
//
// **************************************************************************

#ifndef _NVD3DMAC_H_
#define _NVD3DMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

// swap two 4-byte values using temporary storage
#define NV_SWAP_1(a,b)                      \
{                                           \
    DWORD c;                                \
    c                = *(DWORD *)(&(a));    \
    *(DWORD *)(&(a)) = *(DWORD *)(&(b));    \
    *(DWORD *)(&(b)) = c;                   \
}

// swap two 4-byte values using XORs
#define NV_SWAP_2(a,b)                      \
{                                           \
    *(DWORD *)(&(a)) ^= *(DWORD *)(&(b));   \
    *(DWORD *)(&(b)) ^= *(DWORD *)(&(a));   \
    *(DWORD *)(&(a)) ^= *(DWORD *)(&(b));   \
}

//---------------------------------------------------------------------------

// Some FP macros used in table fog. Thanks Jim!
#define FP_INV(r,p)                                     \
{                                                       \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);            \
    r = FLOAT_FROM_DWORD(_i);                           \
    r = r * (2.0f - (p) * r);                           \
}
#define FP_EXP(e,p)                                     \
{                                                       \
    int _i;                                             \
    e = -1.44269504f * (float)0x00800000 * (p);         \
    _i = (int)e + 0x3F800000;                           \
    e = FLOAT_FROM_DWORD(_i);                           \
}
#define FP_NORM_TO_BYTE(i,p)                            \
{                                                       \
    float _n = (p) + 1.0f;                              \
    (i) = *(int *)&_n;                                  \
    if ((i) >= 0x40000000)     (i) = 0xFF;              \
    else if ((i) <=0x3F800000) (i) = 0;                 \
    else (i) = ((i) >> 15) & 0xFF;                      \
}
#define FP_NORM_TO_BIT24(i,p)                           \
{                                                       \
    float _n = (p) + 1.0f;                              \
    (i) = *(int *)&_n;                                  \
    if ((i) >= 0x40000000)     (i) = 0xFF000000;        \
    else if ((i) <=0x3F800000) (i) = 0;                 \
    else (i) = ((i) << 9) & 0xFF000000;                 \
}

//---------------------------------------------------------------------------

// Fast floating point operations.

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
            fretval = FLOAT_FROM_DWORD(fpbits);         \
        }                                               \
    }                                                   \
}

//---------------------------------------------------------------------------

// D3D state macros.

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

//---------------------------------------------------------------------------

// Memory management macros.

/* this is legacy (win3.1)

#define GlobalUnlockPtr32(lp)                                               \
        GlobalUnlock(GlobalHandle((LPCVOID)lp))

#define GlobalAllocPtr32(flags, cb)                                         \
        (GlobalLock(GlobalAlloc((flags), (cb))))

#define GlobalFreePtr32(lp)                                                 \
        (GlobalUnlockPtr32(lp), (BOOL)GlobalFree(GlobalHandle((LPCVOID)lp)))
*/

#define GlobalUnlockPtr32(lp)                                               \
        ((LPCVOID)lp)

#define GlobalAllocPtr32(flags, cb)                                         \
        (GlobalAlloc((flags), (cb)))

#define GlobalFreePtr32(lp)                                                 \
        ((BOOL)GlobalFree((LPCVOID)lp))


//---------------------------------------------------------------------------

// fog mode macros
// For NV4, table and vertex fog are handled identically, so reduce the two
// renderstates to one value. If both are something other than FOG_NONE,
// table fog wins
#define NV_FOG_MODE \
    (pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE] ?                    \
        (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader) ?                \
            pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE] :          \
            (pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE] ?         \
                pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE] :      \
                pContext->dwRenderState[D3DRENDERSTATE_FOGVERTEXMODE])) :   \
        D3DFOG_NONE)

//---------------------------------------------------------------------------

// cull macro
#define IF_NOT_CULLED_BEGIN(v0,v1,v2,cv) {             \
    DWORD _sx0  = (DWORD)(v0) + 0;                     \
    DWORD _sy0  = (DWORD)(v0) + 4;                     \
    DWORD _sx1  = (DWORD)(v1) + 0;                     \
    DWORD _sy1  = (DWORD)(v1) + 4;                     \
    DWORD _sx2  = (DWORD)(v2) + 0;                     \
    DWORD _sy2  = (DWORD)(v2) + 4;                     \
    float _dx10 = *(float*)_sx1 - *(float*)_sx0;       \
    float _dy10 = *(float*)_sy1 - *(float*)_sy0;       \
    float _dx20 = *(float*)_sx2 - *(float*)_sx0;       \
    float _dy20 = *(float*)_sy2 - *(float*)_sy0;       \
           cv   = _dx10*_dy20  - _dx20*_dy10;          \
    if (((*(DWORD*)&cv) ^ dwCullMask1) & dwCullMask2) {

#define IF_NOT_CULLED_END } }

//---------------------------------------------------------------------------

#define NV_EVAL_CYLINDRICAL_WRAP(pContext, dwTCIndex, bWrapU, bWrapV, bWrapP, bWrapQ)                                   \
{                                                                                                                       \
    BOOL bIndexOK; DWORD dwStream;                                                                                      \
    if (pContext->pCurrentVShader->hasProgram()) {                                                                      \
        bIndexOK = (dwTCIndex < 8) ? TRUE : FALSE;                                                                      \
    }                                                                                                                   \
    else {                                                                                                              \
        dwStream = pContext->pCurrentVShader->getVAStream (defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex]);            \
        bIndexOK = ((dwStream != CVertexShader::VA_STREAM_NONE) && (pContext->ppDX8Streams[dwStream])) ? TRUE : FALSE;  \
    }                                                                                                                   \
    if (bIndexOK) {                                                                                                     \
        bWrapU = ((pContext->dwRenderState[D3DRENDERSTATE_WRAPU]) ||                                                    \
                  (pContext->dwRenderState[D3DRENDERSTATE_WRAP0 + dwTCIndex] & D3DWRAPCOORD_0)) ? TRUE : FALSE;         \
        bWrapV = ((pContext->dwRenderState[D3DRENDERSTATE_WRAPV]) ||                                                    \
                  (pContext->dwRenderState[D3DRENDERSTATE_WRAP0 + dwTCIndex] & D3DWRAPCOORD_1)) ? TRUE : FALSE;         \
        bWrapP = ((pContext->dwRenderState[D3DRENDERSTATE_WRAP0 + dwTCIndex] & D3DWRAPCOORD_2)) ? TRUE : FALSE;         \
        bWrapQ = ((pContext->dwRenderState[D3DRENDERSTATE_WRAP0 + dwTCIndex] & D3DWRAPCOORD_3)) ? TRUE : FALSE;         \
    }                                                                                                                   \
    else {                                                                                                              \
        bWrapU = bWrapV = bWrapP = bWrapQ = FALSE;                                                                      \
    }                                                                                                                   \
}

//---------------------------------------------------------------------------

#define NEXTINSTRUCTION(ptr, type, num, extrabytes)                             \
    ptr = (LPD3DHAL_DP2COMMAND)((LPBYTE)ptr + sizeof(D3DHAL_DP2COMMAND) +       \
                                ((num) * sizeof(type)) + (extrabytes))

#define GET_FVF_POINTER(ptr, lpvert, mask, offset, index, stride)               \
    (ptr) = (BYTE *) ((((DWORD)(lpvert) + ((index) * (stride))) & (DWORD)(mask)) + (offset));

//---------------------------------------------------------------------------

/*
 * Need to make sure that an unfriendly mode switch didn't sneak and not cause
 * us to get re-enabled properly.
 */
#define DOS_FULLSCREEN_CHECK(pp)

/*
 * Set up all the current context state per call.  This should be as efficient as
 * possible as it is executed for every rendering entrypoint.
 * NB - we can't check pContext->dwRTHandle since 0 is actually valid for DX5 apps running on dx7
 */
#define NV_SET_CONTEXT(pContext,pp)                                             \
{                                                                               \
    pContext = (PNVD3DCONTEXT)((pp)->dwhContext);                               \
    if (!pContext) {                                                            \
        DPF("D3D: - Bad Context");                                              \
        (pp)->ddrval = D3DHAL_CONTEXT_BAD;                                      \
        dbgTracePop();                                                          \
        return (DDHAL_DRIVER_HANDLED);                                          \
    }                                                                           \
}

//---------------------------------------------------------------------------

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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif  // _NVD3DMAC_H_

