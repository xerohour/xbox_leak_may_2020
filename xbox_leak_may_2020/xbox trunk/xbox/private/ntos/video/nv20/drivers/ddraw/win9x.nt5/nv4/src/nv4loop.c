/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4LOOP.C                                                         *
*   NV4 Inner loop include file. This file is referenced in nv4vx.c         *
*   ...it is also one mother of an #ifdef hell...                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 05/23/98 - created                      *
*                                                                           *
\***************************************************************************/

// macro        desc
// PROC_        Name of inner loop as string
// PROC         Name of inner loop
// VERTEX       Name of vertex macro
// DX6          defined for DX6 class triangles
// VXSIZE       size of a vertex in push buffer
// FOG          when defined it is either 1-ExpFog, 2-Exp2Fog or 3-LinearFog
// INDEXED      defined for indexed primitives
// FVF          defined for Flex vertices
// ANTIALIAS    defined when we want to capture geometry for antialiasing

//#define DPF_ON_ENTRY        // print entry point name
//#define BREAK_ON_ENTRY      // int3 on entry

/*
 * switches
 */

/*
 * constants & general macros
 */
#ifdef DX6
#define DRAW        nvglDx6TriangleDrawPrimitive
#define COUNT1      ((DWORD)((sizeDx6TriangleTLVertex * 1) + sizeDx6TriangleDrawPrimitive))
#define COUNT2      ((DWORD)((sizeDx6TriangleTLVertex * 2) + sizeDx6TriangleDrawPrimitive))
#define COUNT3      ((DWORD)((sizeDx6TriangleTLVertex * 3) + sizeDx6TriangleDrawPrimitive))
#define COUNT4      ((DWORD)((sizeDx6TriangleTLVertex * 4) + sizeDx6TriangleDrawPrimitive))
#define VC_MASK     7
#define VC_ADD      3
#else //!DX6
#define DRAW        nvglDx5TriangleDrawPrimitive
#define COUNT1      ((DWORD)((sizeDx5TriangleTLVertex * 1) + sizeDx5TriangleDrawPrimitive))
#define COUNT2      ((DWORD)((sizeDx5TriangleTLVertex * 2) + sizeDx5TriangleDrawPrimitive))
#define COUNT3      ((DWORD)((sizeDx5TriangleTLVertex * 3) + sizeDx5TriangleDrawPrimitive))
#define COUNT4      ((DWORD)((sizeDx5TriangleTLVertex * 4) + sizeDx5TriangleDrawPrimitive))
#define VC_MASK     15
#define VC_ADD      7
#endif //!DX6

#ifdef FVF
#define IF_FVF(x,y)  (x)
#define EMIT_VX(x,y) VERTEX (nvFifo, nvFreeCount, NV_DD_SPARE, (y), (x), fvfData, pTriangleState->dwUVOffset)
#else //!FVF
#define IF_FVF(x,y)  (y)
#define EMIT_VX(x,y) VERTEX (nvFifo, nvFreeCount, NV_DD_SPARE, (y), (x))
#endif //!FVF

/*
 * cull macro
 */
#define IF_NOT_CULLED_BEGIN(v0,v1,v2,cv) {             \
    DWORD _sx0  = v0 + 0;                              \
    DWORD _sy0  = v0 + 4;                              \
    DWORD _sx1  = v1 + 0;                              \
    DWORD _sy1  = v1 + 4;                              \
    DWORD _sx2  = v2 + 0;                              \
    DWORD _sy2  = v2 + 4;                              \
    float _dx10 = *(float*)_sx1 - *(float*)_sx0;       \
    float _dy10 = *(float*)_sy1 - *(float*)_sy0;       \
    float _dx20 = *(float*)_sx2 - *(float*)_sx0;       \
    float _dy20 = *(float*)_sy2 - *(float*)_sy0;       \
           cv   = _dx10*_dy20  - _dx20*_dy10;          \
    if (((*(DWORD*)&cv) ^ dwCullMask1) & dwCullMask2) {
#define IF_NOT_CULLED_END } }

/*
 * statistics macro
 */
#ifdef NV_STATS
#define UPDATE_STATS                                                    \
{                                                                       \
    extern DWORD dwPrimCount;                                           \
    extern DWORD dwPrimLegacyCount;                                     \
    extern DWORD dwPrimListCount;                                       \
    extern DWORD dwPrimStripCount;                                      \
    extern DWORD dwPrimFanCount;                                        \
                                                                        \
    dwPrimCount += nPrimCount;                                          \
    switch (dwStrides)                                                  \
    {                                                                   \
        case LEGACY_STRIDES: dwPrimLegacyCount += nPrimCount;           \
                             break;                                     \
        case LIST_STRIDES:   dwPrimListCount   += nPrimCount;           \
                             break;                                     \
        case STRIP_STRIDES:  dwPrimStripCount  += nPrimCount;           \
                             break;                                     \
        case FAN_STRIDES:    dwPrimFanCount    += nPrimCount;           \
                             break;                                     \
    }                                                                   \
}
#else
#define UPDATE_STATS
#endif


//////////////////////////////////////////////////////////////////////////////
// Entry Point
#ifdef INDEXED
#ifdef FVF
void PROC (DWORD nPrimCount,LPWORD pIndices,DWORD dwStrides,LPBYTE        pVertices ) {
#else //!FVF
void PROC (DWORD nPrimCount,LPWORD pIndices,DWORD dwStrides,LPD3DTLVERTEX pVertices) {
#endif //!FVF
#else //!INDEXED
#ifdef FVF
void PROC ( WORD nPrimCount,                DWORD dwStrides,LPBYTE        pVertices ) {
#else //!FVF
void PROC ( WORD nPrimCount,                DWORD dwStrides,LPD3DTLVERTEX pVertices) {
#endif //!FVF
#endif //!INDEXED

#ifdef ANTIALIAS
    UPDATE_STATS
#ifdef INDEXED
    nvAACapturePrimitive (nPrimCount,pIndices,dwStrides,(LPBYTE)pVertices);
    //aatodo - render edge
#else //!INDEXED
    nvAACapturePrimitive (nPrimCount,NULL    ,dwStrides,(LPBYTE)pVertices);
    //aatodo - render edge
#endif
#else //!ANTIALIAS

//////////////////////////////////////////////////////////////////////////////
// Common code
    DWORD v0,v1,v2;         // vertices
    DWORD dwCullMask1;
    DWORD dwCullMask2;
    float dwCullValue;

#ifdef FVF
    DWORD dwVertexStride = fvfData.dwVertexStride;
    PNVD3DMULTITEXTURESTATE pTriangleState
        = (PNVD3DMULTITEXTURESTATE)&pCurrentContext->mtsState;
#endif //FVF

#ifdef  SPEC_HACK
#ifndef DX6
    DWORD   dwSpec0, dwSpec1, dwSpec2;
    DWORD   dwNoSpecCount     = pCurrentContext->ctxInnerLoop.dwNoSpecularTriangleCount;
    DWORD   dwSpecularState   = pCurrentContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE];
    DWORD   dwSpecularCurrent = pCurrentContext->ctxInnerLoop.dwCurrentSpecularState;
    DWORD   dwBlend           = pCurrentContext->ctxInnerLoop.dwCurrentBlend;
#endif  // !DX6
#endif  // SPEC_HACK

    UPDATE_STATS

#ifdef DPF_ON_ENTRY
    DPF (PROC_);
    DPF ("  dwStrides = %08x",dwStrides);
#endif
#ifdef BREAK_ON_ENTRY
    __asm int 3;
#endif

    dwCullMask1 = (pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW)  ? 0x80000000 : 0x00000000;
    dwCullMask2 = (pCurrentContext->dwRenderState[D3DRENDERSTATE_CULLMODE] == D3DCULL_NONE) ? 0xffffffff : 0x80000000;

    switch (dwStrides)
    {
//////////////////////////////////////////////////////////////////////////////
// List
//////////////////////////////////////////////////////////////////////////////
        case LIST_STRIDES:
        case LEGACY_STRIDES:
        {
#ifdef INDEXED
            DWORD dwVertexSNBits = (dwVertexSN++) << 16;
#else //!INDEXED
            /*
             * setup vertices (non-indexed)
             */
            v0 = ((DWORD)pVertices) + 0 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            v1 = ((DWORD)pVertices) + 1 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            v2 = ((DWORD)pVertices) + 2 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
#ifdef FVF
            dwVertexStride *= 3;
#endif //!FVF
#endif //!INDEXED

            dwStrides &= 255; // strip off uninteresting strip info

            while (nPrimCount) 
            {
#ifdef INDEXED
                DWORD i0,i1,i2;
                /*
                 * fetch indices
                 */
                i0  = ((DWORD*)pIndices)[0];
                i1  = i0 >> 16;
                i0 &= 0xffff;
                i2  = (( WORD*)pIndices)[2];
#endif //INDEXED

#ifdef INDEXED
                /*
                 * fetch vertices
                 */
                v0 = ((DWORD)pVertices) + i0 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
                v1 = ((DWORD)pVertices) + i1 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
                v2 = ((DWORD)pVertices) + i2 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
#endif //INDEXED

                /*
                 * cull triangle
                 */
                IF_NOT_CULLED_BEGIN(v0,v1,v2,dwCullValue)
#ifdef INDEXED
                    DWORD x0,x1,x2,tmp;
#endif //INDEXED

#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
#ifdef  FVF
                    dwSpec0 = *(DWORD*)((v0 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
                    dwSpec1 = *(DWORD*)((v1 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
                    dwSpec2 = *(DWORD*)((v2 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
#else   !FVF
                    dwSpec0 = (*(DWORD*)(v0 + 20));
                    dwSpec1 = (*(DWORD*)(v1 + 20));
                    dwSpec2 = (*(DWORD*)(v2 + 20));
#endif  // !FVF
                    SPEC_CHECK(dwSpecularState, dwSpecularCurrent, dwSpec0, dwSpec1, dwSpec2, dwNoSpecCount, dwBlend);
#endif  // !DX6
#endif  // SPEC_HACK

                    /*
                     * Send tri
                     */
                    while (nvFreeCount < COUNT3) 
                        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT3);
#ifdef INDEXED
                // vertex 0
                    tmp = i0 | dwVertexSNBits;
                    x0  = i0 & VC_MASK;
                    if (tmp != dwVertexCache[x0]) { // cache miss
                        dwVertexCache[x0] = tmp;
                        EMIT_VX (v0,x0);
                    }
                // vertex 1
                    tmp = i1 | dwVertexSNBits;
                    x1  = i1 & VC_MASK;
                    while (x1 == x0) x1 = (x1+VC_ADD) & VC_MASK;
                    if (tmp != dwVertexCache[x1]) { // cache miss
                        dwVertexCache[x1] = tmp;
                        EMIT_VX (v1,x1);
                    }
                // vertex 2
                    tmp = i2 | dwVertexSNBits;
                    x2  = i2 & VC_MASK;
                    while ((x2 == x1)||(x2 == x0)) x2 = (x2+VC_ADD) & VC_MASK;
                    if (tmp != dwVertexCache[x2]) { // cache miss
                        dwVertexCache[x2] = tmp;
                        EMIT_VX (v2,x2);
                    }
                    DRAW    (nvFifo, nvFreeCount, NV_DD_SPARE, 0, ((x2<<8)|(x1<<4)|x0));
#else //!INDEXED
                    EMIT_VX (v0,0);
                    EMIT_VX (v1,1);
                    EMIT_VX (v2,2);
                    DRAW    (nvFifo, nvFreeCount, NV_DD_SPARE, 0, 0x210);
#endif //!INDEXED
                IF_NOT_CULLED_END

                /*
                 * next triangle
                 */
                nPrimCount--;

#ifdef INDEXED
                pIndices += dwStrides;
#else //!INDEXED
                v0 += IF_FVF(dwVertexStride,3 * sizeof(D3DTLVERTEX));
                v1 += IF_FVF(dwVertexStride,3 * sizeof(D3DTLVERTEX));
                v2 += IF_FVF(dwVertexStride,3 * sizeof(D3DTLVERTEX));
#endif //!INDEXED
                /*
                 * The following code is only enabled for the DEBUG driver build.
                 * Flush each triangle.
                 */            
                dbgFlushTriangle(pCurrentContext);
            }
            break;
        }
//////////////////////////////////////////////////////////////////////////////
// Strip
//////////////////////////////////////////////////////////////////////////////
        case STRIP_STRIDES:
        {
            static DWORD table[6] = { 0x210,0x201,0x102,0x120,0x021,0x012 };
            DWORD funky  = 0;
            DWORD launch = 0x210;
            DWORD place  = 0x808182;

#ifdef INDEXED
            v0 = ((DWORD)pVertices) + ((WORD*)pIndices)[0] * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            v1 = ((DWORD)pVertices) + ((WORD*)pIndices)[1] * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            pIndices += 2;
#else //!INDEXED
            /*
             * setup vertices (non-indexed)
             */
            v0 = ((DWORD)pVertices) + 0 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            v1 = ((DWORD)pVertices) + 1 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            ((DWORD)pVertices) += 2 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
#endif //!INDEXED

#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
#ifdef  FVF
            dwSpec0 = *(DWORD*)((v0 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
            dwSpec1 = *(DWORD*)((v1 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
#else   !FVF
            dwSpec0 = (*(DWORD*)(v0 + 20));
            dwSpec1 = (*(DWORD*)(v1 + 20));
#endif  // !FVF
#endif  // !DX6
#endif  // SPEC_HACK

            while (nPrimCount) 
            {
#ifdef INDEXED
                v2 = ((DWORD)pVertices) + ((WORD*)pIndices)[0] * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
                pIndices++;
#else //!INDEXED
                v2 = ((DWORD)pVertices);
                ((DWORD)pVertices) += IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
#endif //!INDEXED
#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
#ifdef  FVF
                    dwSpec2 = *(DWORD*)((v2 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
#else   !FVF
                    dwSpec2 = (*(DWORD*)(v2 + 20));
#endif  // !FVF
#endif  // !DX6
#endif  // SPEC_HACK

                /*
                 * cull triangle
                 */
                IF_NOT_CULLED_BEGIN(v0,v1,v2,dwCullValue)
                    DWORD p;

#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
                    SPEC_CHECK(dwSpecularState, dwSpecularCurrent, dwSpec0, dwSpec1, dwSpec2, dwNoSpecCount, dwBlend);
#endif  // !DX6
#endif  // SPEC_HACK

                    /*
                     * Send tri
                     */
                    if ((place & 0x800000) == 0x800000) {
                        p      = (place >> 16) & 127;
                        place &= ~0x800000;
                        while (nvFreeCount < COUNT1) 
                            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT1);
                        EMIT_VX (v0,p);
                    }
                    if ((place & 0x008000) == 0x008000) {
                        p      = (place >> 8) & 127;
                        place &= ~0x008000;
                        while (nvFreeCount < COUNT1) 
                            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT1);
                        EMIT_VX (v1,p);
                    }
                    p      = place & 127;
                    place &= ~0x000080;
                    while (nvFreeCount < COUNT1) 
                        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT1);
                    EMIT_VX (v2,p);
                    DRAW (nvFifo, nvFreeCount, NV_DD_SPARE, 0, launch);
                IF_NOT_CULLED_END

                /*
                 * next triangle
                 */
                v0      = v1; 
                v1      = v2;
#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
                dwSpec0 = dwSpec1;
                dwSpec1 = dwSpec2;
#endif  // !DX6
#endif  // SPEC_HACK
                {
                    DWORD x = place & 0x7f;
                    if (x == 2) x = 0x80; else x+=0x81;
                    place = (place<<8) | x;
                }
                funky        = (funky == 5) ? 0 : (funky + 1);
                launch       = table[funky];
                dwCullMask1 ^= 0x80000000;
                nPrimCount--;

                /*
                 * The following code is only enabled for the DEBUG driver build.
                 * Flush each triangle.
                 */            
                dbgFlushTriangle(pCurrentContext);
            }
            break;
        }
//////////////////////////////////////////////////////////////////////////////
// Fan
//////////////////////////////////////////////////////////////////////////////
        case FAN_STRIDES:
        {
            DWORD place  = 0x8182;
            DWORD launch = 0x210;

#ifdef INDEXED
            v0 = ((DWORD)pVertices) + ((WORD*)pIndices)[0] * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            v1 = ((DWORD)pVertices) + ((WORD*)pIndices)[1] * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            pIndices += 2;
#else //!INDEXED
            /*
             * setup vertices (non-indexed)
             */
            v0 = ((DWORD)pVertices) + 0 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            v1 = ((DWORD)pVertices) + 1 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
            ((DWORD)pVertices) += 2 * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
#endif //!INDEXED
#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
#ifdef  FVF
            dwSpec0 = *(DWORD*)((v0 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
            dwSpec1 = *(DWORD*)((v1 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
#else   !FVF
            dwSpec0 = (*(DWORD*)(v0 + 20));
            dwSpec1 = (*(DWORD*)(v1 + 20));
#endif  // !FVF
#endif  // !DX6
#endif  // SPEC_HACK

            while (nvFreeCount < COUNT1)
                nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT1);
            EMIT_VX (v0,0);

            while (nPrimCount) 
            {
#ifdef INDEXED
                v2 = ((DWORD)pVertices) + ((WORD*)pIndices)[0] * IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
                pIndices++;
#else //!INDEXED
                v2 = ((DWORD)pVertices);
                ((DWORD)pVertices) += IF_FVF(dwVertexStride,sizeof(D3DTLVERTEX));
#endif //!INDEXED
#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
#ifdef  FVF
                dwSpec2 = *(DWORD*)((v2 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
#else   !FVF
                dwSpec2 = (*(DWORD*)(v2 + 20));
#endif  // !FVF
#endif  // !DX6
#endif  // SPEC_HACK

                /*
                 * cull triangle
                 */
                IF_NOT_CULLED_BEGIN(v0,v1,v2,dwCullValue)
                    DWORD p;

#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
                    SPEC_CHECK(dwSpecularState, dwSpecularCurrent, dwSpec0, dwSpec1, dwSpec2, dwNoSpecCount, dwBlend);
#endif  // !DX6
#endif  // SPEC_HACK

                    /*
                     * Send tri
                     */
                    if ((place & 0x008000) == 0x008000) {
                        p      = (place >> 8) & 127;
                        place &= ~0x008000;
                        while (nvFreeCount < COUNT1) 
                            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT1);
                        EMIT_VX (v1,p);
                    }
                    p      = place & 127;
                    place &= ~0x000080;
                    while (nvFreeCount < COUNT1) 
                        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, COUNT1);
                    EMIT_VX (v2,p);
                    DRAW (nvFifo, nvFreeCount, NV_DD_SPARE, 0, launch);
                IF_NOT_CULLED_END

                /*
                 * next triangle
                 */
                v1 ^= v2;
                v2 ^= v1;
                v1 ^= v2;

#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
#ifdef  FVF
                dwSpec1 = *(DWORD*)((v1 & fvfData.dwSpecularMask) + fvfData.dwSpecularOffset);
#else   !FVF
                dwSpec1 = (*(DWORD*)(v1 + 20));
#endif  // !FVF
#endif  // !DX6
#endif  // SPEC_HACK
                {
                    DWORD x = place & 0x7f;
                    x      ^= 0x81 ^ 2; // toggle 1 <-> 2 & or 0x80
                    place   = (place<<8) | x;
                }
                launch ^= 0x210 ^ 0x120;
                nPrimCount--;

                /*
                 * The following code is only enabled for the DEBUG driver build.
                 * Flush each triangle.
                 */            
                dbgFlushTriangle(pCurrentContext);
            }

            break;
        }
    }

#ifdef  SPEC_HACK   // check for contributing specular highlight
#ifndef DX6         // when using the DX5 class only.
     pCurrentContext->ctxInnerLoop.dwNoSpecularTriangleCount = dwNoSpecCount;
     pCurrentContext->ctxInnerLoop.dwCurrentSpecularState    = dwSpecularCurrent;
     pCurrentContext->ctxInnerLoop.dwCurrentBlend            = dwBlend;
#endif  // !DX6
#endif  // SPEC_HACK

#endif //!ANTIALIAS
}

#undef IF_NOT_CULLED_BEGIN
#undef IF_NOT_CULLED_END
#undef VC_ADD
#undef VC_MASK
#undef IF_FVF
#undef EMIT_VX
#undef COUNT4
#undef COUNT3
#undef COUNT2
#undef COUNT1
#undef DRAW
