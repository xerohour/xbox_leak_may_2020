/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       drawprim.cpp
 *  Content:    Handles the invocation of rendering commands.
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// DOC: Behavior when an index is out-of-bounds

// 'DRAW_COUNT_BATCH' is the maximum number of vertices we spew on every
// call to DRAW_ARRAYS.  256 is the maximum allowable hardware limit.

#define DRAW_COUNT_BATCH 256

// 'ARRAY_ELEMENT16_BATCH' is the maximum number of *pairs* of indices we 
// spew in consecutive writes to ARRAY_ELEMENT16.  2047 is the maximum
// allowable hardware limit.

#define ARRAY_ELEMENT16_BATCH 511

// MMXMemCpyDwordBlock writes 60 bytes followed by X number of 64 byte blocks.
// This allows us to write the ELEMENT16 hdr then call MMXMemCpyDwordBlock
// and keep the dest 32 byte aligned. This define tells us the minimum number
// of indices MMXMemCpyDwordBlock can handle.

// 60 + 64 bytes, 15 + 16 dwords, or 30 + 32 indices
#define ARRAY_ELEMENT16_BLOCK_VERTICES_MIN  (30 + 32)

// 'ARRAY_ELEMENT16_SLOP' is the number of *pairs* of indices that we
// ensure are in last loop iteration.                              
                              
#define ARRAY_ELEMENT16_SLOP 2

#if DBG

    // Global for tracking the Begin/End state:
    
    INT g_CurrentPrimitiveType;

#endif

//------------------------------------------------------------------------------
// FastCopyToWC
//
// Helper function for copying from cacheable memory to write-combined memory.
// Much better than 'rep movsd' when the size is large and the source isn't
// already in the cache.

VOID FastCopyToWC(
    PPUSH pPush,    
    DWORD* pSource, 
    DWORD dwCount)  
{
    __asm
    {
        mov esi,pSource
        mov edi,pPush
        mov edx,dwCount

        mov ecx,edi
        neg ecx
        and ecx,0x1f
        shr ecx,2       // (-dest & 0x1f) / 4

        cmp edx,ecx     // Enough to align?
        jg do_align

        mov ecx,edx     // Not enough, use simple copy
        jmp finish

    do_align:
        // 32 byte align dest.
        sub edx,ecx

        rep movsd

        mov ecx,edx
        jmp enter_loop

        ALIGN 16

    copy_loop:
        prefetchnta [esi+64]
        prefetchnta [esi+96]

        // Move 16 dwords
        movq mm0,[esi]
        movq mm1,[esi+8]
        movq mm2,[esi+16]
        movq mm3,[esi+24]
        movq mm4,[esi+32]
        movq mm5,[esi+40]
        movq mm6,[esi+48]
        movq mm7,[esi+56]

        movntq [edi],mm0
        movntq [edi+8],mm1
        movntq [edi+16],mm2
        movntq [edi+24],mm3
        movntq [edi+32],mm4
        movntq [edi+40],mm5
        movntq [edi+48],mm6
        movntq [edi+56],mm7

        nop
        nop

        add esi,64
        add edi,64

    enter_loop:
        sub ecx,16
        jge copy_loop

        add ecx,16

    finish:
        // Finish any left over.
        rep movsd

        emms
    }
}

//------------------------------------------------------------------------------
// D3DDevice_DrawVerticesUP
//
// User-pointer version of DrawPrimitive.  This uses the CPU to copy vertex 
// data from the specified buffer directly into the push buffer.
//
// This is by no means optimal.  It is, however, small, and handles all
// permutations of vertex component types for vertex shader programs.
// On Xbox, there's no real reason to be using UP primitives given that
// we're UMA and have most excellent VB support, so this is a reasonable
// tradeoff.
//
// Note that this 'DrawVertices' version takes a vertex count, rather than 
// a primitive count as with the 'DrawPrimitive' version.  This makes it
// easier for the caller, and eliminates two lookups to calculate the vertex 
// count.  DrawPrimitive is converted in-line in the caller to DrawVertices.
//
// Analagous to nvKelvinDispatchNonIndexedPrimitiveInline
//
// DOC: Unlike DX8, we do not set the stream zero settings to NULL
//
// DOC: Every vertex attribute has to be DWORD aligned

extern "C"
void WINAPI D3DDevice_DrawVerticesUP(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride) 
{ 
    DWORD i;

    COUNT_API(API_D3DDEVICE_DRAWVERTICESUP);
    COUNT_ADD_PERF(PERF_VERTICES, VertexCount);
    COUNT_PERFEVENT(D3DPERFEvent_DrawVerticesUP, TRUE);

    CDevice* pDevice = g_pDevice;

    // Whack the hardware into the correct state, *before* doing our
    // parameter checks:

    pDevice->SetStateUP();

    // Now do some parameter checking.  We do this after calling SetStateUP
    // because we examine the current state in one of our checks.

    if (DBG_CHECK(TRUE))
    {
        if ((D3D__RenderState[D3DRS_POINTSPRITEENABLE]) &&
            (PrimitiveType != D3DPT_POINTLIST))
        {
            DPF_ERR("Can only do D3DPT_POINTLIST when point sprites enabled");
        }
        if ((D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
            && pDevice->m_pZBuffer
            && PixelJar::GetFormat(pDevice->m_pZBuffer) == D3DFMT_LIN_F24S8)
        {
            DPF_ERR("W buffering is not supported with a 24-bit floating point depth buffer");
        }
        if (VertexCount == 0)
        {
            DPF_ERR("Zero VertexCount");
        }
        if ((PrimitiveType <= 0) || (PrimitiveType >= D3DPT_MAX))
        {
            DPF_ERR("Invalid primitive type");
        }
        if (VertexStreamZeroStride == 0)
        {
            DPF_ERR("Invalid stride");
        }
        if (pVertexStreamZeroData == NULL)
        {
            DPF_ERR("NULL pointer");
        }

        // Since we do DWORD copies, enforcing DWORD alignment of every
        // attribute guarantees us that we'll never do an unaligned read 
        // of even a single byte past the end of an attribute that is less
        // than a DWORD in length - because inevitably that extra byte read
        // will be one byte past the end of the buffer on a page that isn't
        // there.  Those are nasty, nasty crashes.

        if ((((DWORD) pVertexStreamZeroData) & 3) || (VertexStreamZeroStride & 3))
        {
            DPF_ERR("DWORD alignment is required for the UP stream data");
        }
        for (i = 0; i < pDevice->m_InlineAttributeCount; i++)
        {
            if (pDevice->m_InlineAttributeData[i].UP_Delta & 3)
            {
                DPF_ERR("DWORD alignment is required for every UP attribute");
            }
        }
        if(pDevice->m_dwSnapshot)
        {
            HandleShaderSnapshot_DrawVerticesUP(PrimitiveType,
                                                VertexCount,
                                                pVertexStreamZeroData,
                                                VertexStreamZeroStride);
            return;
        }
    }

    PPUSH pPush = pDevice->StartPush();
    Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);
    pDevice->EndPush(pPush + 2);
    pDevice->StartBracket();
    
    InlineAttributeData* pAttributeData = &pDevice->m_InlineAttributeData[0];

    DWORD* pVertexStart = (DWORD*) ((BYTE*) pVertexStreamZeroData 
                                   + pDevice->m_InlineStartOffset);

    DWORD attributeCount = pDevice->m_InlineAttributeCount;

    // Adjust the last delta used in our inner loop so that it automatically
    // takes us from the end of the last attribute of the just-completed vertex
    // to the start of the first attribute of the next vertex:

    DWORD delta = (VertexStreamZeroStride + pDevice->m_InlineDelta);

    pAttributeData[attributeCount - 1].UP_Delta = delta;

    // We can send a maximum of 2047 dwords in a single push-buffer command.  
    // But our loop works by vertex, not by dword.  So figure out the number 
    // of vertices we can send in a single command:

    DWORD batchLimit = (VertexCount <= 16) 
                     ? 16 
                     : (MAX_PUSH_METHOD_SIZE / pDevice->m_InlineVertexDwords);

    while (TRUE)
    {
        // Number of vertices to do in this batch:

        DWORD batchCount = min(batchLimit, VertexCount);

        // Get count ready for next batch:

        VertexCount -= batchCount;

        // Count of DWORDs to do in this batch:

        DWORD count = batchCount * pDevice->m_InlineVertexDwords;

        // Reserve an extra DWORD for the INLINE_ARRAY command, and
        // an additional two DWORDs for the SET_BEGIN_END command:

        PPUSH pPush = pDevice->StartPush(count + 3);

        ASSERT((count) && (count < 2048));

        PushCount(pPush++, PUSHER_NOINC(NV097_INLINE_ARRAY), count);

        if ((count >= 128) && (attributeCount == 1) && (delta == 0))
        {
            // The data is nicely packed so that we can do one big memcpy.
            // The batch is also large enough that we're willing to pay
            // the fast-copy overhead:

            FastCopyToWC(pPush, pVertexStart, count);

            pVertexStart += count;
            pPush += count;
        }
        else
        {
            // D'oh, we have to copy attribute-by-attribute:

            _asm {
            
                mov edx, batchCount;
                mov esi, pVertexStart;
                mov edi, pPush;
            
            Vertex_Loop:
            
                mov ebx, pAttributeData;
                mov eax, attributeCount;
            
            Attribute_Loop:
            
                mov ecx, [ebx].UP_Count;
                rep movsd;
                add esi, [ebx].UP_Delta;
                add ebx, size InlineAttributeData;
                dec eax;
                jnz Attribute_Loop;
        
                dec edx;
                jnz Vertex_Loop;
        
                mov pPush, edi;
                mov pVertexStart, esi;
            }
        }

        // Are we done?

        if (VertexCount == 0)
        {
            Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
            pDevice->EndPush(pPush + 2);
            pDevice->EndBracket();

            return;
        }

        pDevice->EndPush(pPush);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_DrawIndexedVerticesUP
//
// This is by no means optimal.  It is, however, small, and handles all
// permutations of vertex component types for vertex shader programs.
// On Xbox, there's no real reason to be using UP primitives given that
// we're UMA and have most excellent VB support, so this is a reasonable
// tradeoff.
//
// DOC: Unlike as is speced for DX8, we do not set the stream zero settings 
//      to NULL, nor do we make SetIndices also set to NULL
//
// Note that this 'DrawVertices' version takes a vertex count, rather than 
// a primitive count as with the 'DrawPrimitive' version.  This makes it
// easier for the caller, and eliminates two lookups to calculate the vertex 
// count.  DrawPrimitive is converted in-line in the caller to DrawVertices.

extern "C"
void WINAPI D3DDevice_DrawIndexedVerticesUP(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST void* pIndexData,
    CONST void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride) 
{ 
    DWORD i;

    COUNT_API(API_D3DDEVICE_DRAWINDEXEDVERTICESUP);
    COUNT_ADD_PERF(PERF_VERTICES, VertexCount);
    COUNT_PERFEVENT(D3DPERFEvent_DrawIndexedVerticesUP, TRUE);

    CDevice* pDevice = g_pDevice;

    // Whack the hardware into the correct state:

    pDevice->SetStateUP();

    // Now do some parameter checking.  We do this after calling SetStateUP
    // because we examine the current state in one of our checks.

    if (DBG_CHECK(TRUE))
    {
        if ((D3D__RenderState[D3DRS_POINTSPRITEENABLE]) &&
            (PrimitiveType != D3DPT_POINTLIST))
        {
            DPF_ERR("Can only do D3DPT_POINTLIST when point sprites enabled");
        }
        if ((D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
            && pDevice->m_pZBuffer
            && PixelJar::GetFormat(pDevice->m_pZBuffer) == D3DFMT_LIN_F24S8)
        {
            DPF_ERR("W buffering is not supported with a 24-bit floating point depth buffer");
        }
        if (VertexCount == 0)
        {
            DPF_ERR("Zero VertexCount");
        }
        if ((PrimitiveType <= 0) || (PrimitiveType >= D3DPT_MAX))
        {
            DPF_ERR("Invalid primitive type");
        }
        if (VertexStreamZeroStride == 0)
        {
            DPF_ERR("Invalid stride");
        }
        if ((pIndexData == NULL) || (pVertexStreamZeroData == NULL))
        {
            DPF_ERR("NULL pointer");
        }

        // Since we do DWORD copies, enforcing DWORD alignment of every
        // attribute guarantees us that we'll never do an unaligned read 
        // of even a single byte past the end of an attribute that is less
        // than a DWORD in length - because inevitably that extra byte read
        // will be one byte past the end of the buffer on a page that isn't
        // there.  Those are nasty, nasty crashes.

        if ((((DWORD) pVertexStreamZeroData) & 3) || (VertexStreamZeroStride & 3))
        {
            DPF_ERR("DWORD alignment is required for the UP stream data");
        }
        for (i = 0; i < pDevice->m_InlineAttributeCount; i++)
        {
            if (pDevice->m_InlineAttributeData[i].UP_Delta & 3)
            {
                DPF_ERR("DWORD alignment is required for every UP attribute");
            }
        }
        if(pDevice->m_dwSnapshot)
        {
            HandleShaderSnapshot_DrawIndexedVerticesUP(PrimitiveType,
                                                       VertexCount,
                                                       pIndexData,
                                                       pVertexStreamZeroData,
                                                       VertexStreamZeroStride);
            return;
        }
    }

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);

    pDevice->EndPush(pPush + 2);
    pDevice->StartBracket();

    InlineAttributeData *pAttributeData = &pDevice->m_InlineAttributeData[0];

    DWORD* pVertexStart = (DWORD*) ((BYTE*) pVertexStreamZeroData 
                                   + pDevice->m_InlineStartOffset);

    DWORD attributeCount = pDevice->m_InlineAttributeCount;

    // We can send a maximum of 2047 dwords in a single push-buffer command.  
    // But our loop works by vertex, not by dword.  So figure out the number 
    // of vertices we can send in a single command:

    DWORD batchLimit = (VertexCount <= 16) 
                     ? 16 
                     : (MAX_PUSH_METHOD_SIZE / pDevice->m_InlineVertexDwords);

    while (TRUE)
    {
        // Number of vertices to do in this batch:

        DWORD batchCount = min(batchLimit, VertexCount);

        // Get count ready for next batch:

        VertexCount -= batchCount;

        // Count of DWORDs to do in this batch:

        DWORD count = batchCount * pDevice->m_InlineVertexDwords;

        // Reserve an extra DWORD for the INLINE_ARRAY command, and
        // an additional two DWORDs for the SET_BEGIN_END command:

        PPUSH pPush = pDevice->StartPush(count + 3);

        ASSERT((count) && (count < 2048));

        PushCount(pPush++, PUSHER_NOINC(NV097_INLINE_ARRAY), count);
    
        _asm {
        
            mov edx, pIndexData
            mov edi, pPush;
        
        Vertex_Loop:
    
            movzx esi, word ptr [edx];
            add edx, 2;
            mov eax, attributeCount;
            imul esi, VertexStreamZeroStride;
            mov ebx, pAttributeData;
            add esi, pVertexStart
        
        Attribute_Loop:
        
            mov ecx, [ebx].UP_Count;
            rep movsd;
            add esi, [ebx].UP_Delta;
            add ebx, size InlineAttributeData;
            dec eax;
            jnz Attribute_Loop;
    
            dec batchCount;
            jnz Vertex_Loop;
    
            mov pPush, edi;
            mov pIndexData, edx;
        }

        // Are we done?

        if (VertexCount == 0)
        {
            Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
            pDevice->EndPush(pPush + 2);
            pDevice->EndBracket();

            return;
        }

        pDevice->EndPush(pPush);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_DrawVertices
//
// Note that this 'DrawVertices' version takes a vertex count, rather than 
// a primitive count as with the 'DrawPrimitive' version.  This makes it
// easier for the caller, and eliminates two lookups to calculate the vertex 
// count.  DrawPrimitive is converted in-line in the caller to DrawVertices.

extern "C"
void WINAPI D3DDevice_DrawVertices(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT StartVertex,
    UINT VertexCount) 
{ 
    COUNT_API(API_D3DDEVICE_DRAWVERTICES);
    COUNT_ADD_PERF(PERF_VERTICES, VertexCount);
    COUNT_PERFEVENT(D3DPERFEvent_DrawVertices, TRUE);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if ((D3D__RenderState[D3DRS_POINTSPRITEENABLE]) &&
            (PrimitiveType != D3DPT_POINTLIST))
        {
            DPF_ERR("Can only do D3DPT_POINTLIST when point sprites enabled");
        }
        if ((D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
            && pDevice->m_pZBuffer
            && PixelJar::GetFormat(pDevice->m_pZBuffer) == D3DFMT_LIN_F24S8)
        {
            DPF_ERR("W buffering is not supported with a 24-bit floating point depth buffer");
        }
        if (VertexCount == 0)
        {
            DPF_ERR("Zero VertexCount");
        }
        if ((PrimitiveType <= 0) || (PrimitiveType >= D3DPT_MAX))
        {
            DPF_ERR("Invalid primitive type");
        }
        if (StartVertex > 0xffff)
        {
            DPF_ERR("Hardware can't take a StartVertex more than 0xffff");
        }
        if (StartVertex + VertexCount > 0xffff)
        {
            DPF_ERR("Hardware can't take more than 0xffff vertices");
        }
        if(pDevice->m_dwSnapshot)
        {
            HandleShaderSnapshot_DrawVertices(PrimitiveType,
                                              StartVertex,
                                              VertexCount);
            return;
        }
    }

    // Whack the hardware into the correct state:

    pDevice->SetStateVB(0);

    // Loop through the vertex count.  We have to loop because the chip can
    // take a 'Count' value of only 8 bits in size.

    DWORD drawArraysCount = ((VertexCount - 1) / DRAW_COUNT_BATCH) + 1;

    PPUSH pPush = pDevice->StartPush(drawArraysCount + 5);

    Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);

    PushCount(pPush + 2, PUSHER_NOINC(NV097_DRAW_ARRAYS), drawArraysCount);

    while (VertexCount > DRAW_COUNT_BATCH)
    {
        *(pPush + 3) = DRF_NUMFAST(097, _DRAW_ARRAYS, _COUNT, DRAW_COUNT_BATCH - 1)
                     | DRF_NUMFAST(097, _DRAW_ARRAYS, _START_INDEX, StartVertex);

        pPush++;
        StartVertex += DRAW_COUNT_BATCH;
        VertexCount -= DRAW_COUNT_BATCH;
        ASSERT(drawArraysCount-- > 1);
    }
    
    ASSERT(drawArraysCount == 1);
    ASSERT(VertexCount >= 1);

    *(pPush + 3) = DRF_NUMFAST(097, _DRAW_ARRAYS, _COUNT, VertexCount - 1) 
                 | DRF_NUMFAST(097, _DRAW_ARRAYS, _START_INDEX, StartVertex);

    Push1(pPush + 4, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

    pDevice->EndPush(pPush + 6);
}

//------------------------------------------------------------------------------
// MMXMemCpyDwords
//
// Copy count DWORDS from pSrc to pDst
//   Up to caller to handle emms.

// Disable warning: "No EMMS at end of function"
#pragma warning(push)
#pragma warning(disable:4799)

__forceinline void MMXMemCpyDwordBlock(
    const void *pDst,
    const void *pSrc,
    DWORD count)
{
    // We can handle a minimum of 31 dwords.
    ASSERT(count >= 15 + 16);

    // This routine only handles block copies.
    ASSERT( !((count - 15) % 16) );

    __asm
    {
        mov esi,pSrc
        mov edi,pDst

        mov ecx,count
        sub ecx,15
        shr ecx,4

        prefetchnta [esi+60]
        prefetchnta [esi+92]

        // Move 30 indices, 15 dwords
        mov eax,[esi+0]
        movq mm1,[esi+4]
        movq mm2,[esi+12]
        movq mm3,[esi+20]
        movq mm4,[esi+28]
        movq mm5,[esi+36]
        movq mm6,[esi+44]
        movq mm7,[esi+52]

        // The nops here drop the BUS_COMPLETED_PARTIAL_WRITES PIII counter
        // down a bit. Basically it allows the movs above to complete and
        // not interrupt the writes as much. At least that's the idea.
        nop
        nop

        mov [edi+4],eax
        movntq [edi+8],mm1
        movntq [edi+16],mm2
        movntq [edi+24],mm3
        movntq [edi+32],mm4
        movntq [edi+40],mm5
        movntq [edi+48],mm6
        movntq [edi+56],mm7

        add esi,60
        add edi,64

        align 16

    copy_loop:
        prefetchnta [esi+64]
        prefetchnta [esi+96]

        // Move 32 indices, 16 dwords
        movq mm0,[esi]
        movq mm1,[esi+8]
        movq mm2,[esi+16]
        movq mm3,[esi+24]
        movq mm4,[esi+32]
        movq mm5,[esi+40]
        movq mm6,[esi+48]
        movq mm7,[esi+56]

        nop
        nop
    
        movntq [edi],mm0
        movntq [edi+8],mm1
        movntq [edi+16],mm2
        movntq [edi+24],mm3
        movntq [edi+32],mm4
        movntq [edi+40],mm5
        movntq [edi+48],mm6
        movntq [edi+56],mm7

        add esi,64
        add edi,64

        dec ecx
        jnz copy_loop
    }
}

#pragma warning(pop)

//------------------------------------------------------------------------------
// D3DDevice_DrawIndexedVertices
//
// Note that this 'DrawVertices' version takes a vertex count, rather than 
// a primitive count as with the 'DrawPrimitive' version.  This makes it
// easier for the caller, and eliminates two lookups to calculate the vertex 
// count.  DrawPrimitive is converted in-line in the caller to DrawVertices.

extern "C"
void WINAPI D3DDevice_DrawIndexedVertices(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST WORD* pIndexData)
{ 
    COUNT_API(API_D3DDEVICE_DRAWINDEXEDVERTICES);
    COUNT_ADD_PERF(PERF_VERTICES, VertexCount);
    COUNT_PERFEVENT(D3DPERFEvent_DrawIndexedVertices, TRUE);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if ((D3D__RenderState[D3DRS_POINTSPRITEENABLE]) &&
            (PrimitiveType != D3DPT_POINTLIST))
        {
            DPF_ERR("Can only do D3DPT_POINTLIST when point sprites enabled");
        }
        if ((D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
            && pDevice->m_pZBuffer
            && PixelJar::GetFormat(pDevice->m_pZBuffer) == D3DFMT_LIN_F24S8)
        {
            DPF_ERR("W buffering is not supported with a 24-bit floating point depth buffer");
        }
        if ((VertexCount == 0) || (VertexCount > 32*1024*1024))
        {
            DPF_ERR("Invalid VertexCount");
        }
        if ((PrimitiveType <= 0) || (PrimitiveType >= D3DPT_MAX))
        {
            DPF_ERR("Invalid primitive type");
        }
        if (pIndexData == NULL)
        {
            DPF_ERR("Pointer to index data is NULL");
        }
        if(pDevice->m_dwSnapshot)
        {
            HandleShaderSnapshot_DrawIndexedVertices(PrimitiveType,
                                                     VertexCount,
                                                     pIndexData);
            return;
        }
    }

    // Whack the hardware into the correct state:
    pDevice->SetStateVB(pDevice->m_IndexBase);
    pDevice->StartBracket();

    // 2: set begin/end, 7: alignment, 1: array_element16, indices
    PPUSH pPush = pDevice->StartPush(2 + 7 + 1 + ARRAY_ELEMENT16_BATCH);

    Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);
    pPush += 2;

    // The mmx copy routines performs much better with a 32 byte aligned dest.
    // Calculate the number of DWORDS needed to align that bugger.
    DWORD dwAlign = (DWORD) (-((INT) pPush) & 0x1f) / 4;

    // The mmx routine can only handle blocks >= 62 vertices (31 dwords)
    if ((VertexCount >= ARRAY_ELEMENT16_BLOCK_VERTICES_MIN + 2*dwAlign))
    {
        // 32 byte align the pushbuffer.
        if (dwAlign)
        {
            // Should never need to write more than 7 dwords.
            ASSERT(dwAlign <= 7);

            // subtract the header.
            dwAlign--;

            PushCount(pPush, 
                      PUSHER_NOINC(NV097_ARRAY_ELEMENT16), 
                      dwAlign);

            //memcpy(pPush + 1, pIndexData, Align * sizeof(DWORD));
            __asm
            {
                mov esi,pIndexData
                mov edi,pPush
                add edi,4
                mov ecx,dwAlign
                rep movsd
            }

            pPush += dwAlign + 1;
            pIndexData += dwAlign * 2;
            VertexCount -= dwAlign * 2;

            ASSERT(!((DWORD)pPush & 0x1f));
            ASSERT((INT) VertexCount >= 0);
        }

        // Figure out how many large batches we can do.
        DWORD CountOfBatches = VertexCount / (ARRAY_ELEMENT16_BATCH * 2);

        // Calc the number of remaing vertices.
        DWORD RemainingVertices =
            VertexCount - CountOfBatches * ARRAY_ELEMENT16_BATCH * 2;

        // The first time through the loop we'll take care of the vertices
        // which aren't large enough for a full batch but can be handled
        // by the mmx block copy routine. 
        DWORD arrayCount =
            (RemainingVertices >= ARRAY_ELEMENT16_BLOCK_VERTICES_MIN) ?
                (((RemainingVertices >> 1) + 1) & ~0xf) - 1 :
                    ARRAY_ELEMENT16_BATCH;

        do
        {
            PushCount(pPush, 
                      PUSHER_NOINC(NV097_ARRAY_ELEMENT16), 
                      arrayCount);

            // Write 15 + 16*x number of dwords. This plus the hdr above
            // keeps the destination 32-byte aligned.
            MMXMemCpyDwordBlock((void*) pPush, pIndexData, arrayCount);

            pIndexData += arrayCount*2;
            VertexCount -= arrayCount*2;

            ASSERT((INT) VertexCount >= 0);

            pDevice->EndPush(pPush + 1 + arrayCount);
            pPush = pDevice->StartPush(1 + ARRAY_ELEMENT16_BATCH + 4);

            // Do big batches from here on out.
            arrayCount = ARRAY_ELEMENT16_BATCH;

        } while(VertexCount >= ARRAY_ELEMENT16_BATCH * 2);

        // For MMXMemCpyDwordBlock.
       __asm emms
    }

    // Note that it's okay if we send an Element16 command with zero entries
    // (for the case where the vertex count given to us is less than 3):
    DWORD arrayCount = VertexCount >> 1;

    PushCount(pPush, PUSHER_NOINC(NV097_ARRAY_ELEMENT16), arrayCount);
    //memcpy(pPush + 1, pIndexData, arrayCount * sizeof(DWORD));
    __asm
    {
        mov esi,pIndexData
        mov edi,pPush
        add edi,4
        mov ecx,arrayCount
        rep movsd
    }

    pIndexData += 2 * arrayCount;
    pPush += arrayCount + 1;

    // Handle the last odd index.  Since we only have one index left, we
    // obviously can't whack ELEMENT16, since that takes pairs of indices...

    if (VertexCount & 1)
    {
        // Note that we can't cast 'pIndexData' to a DWORD* and then AND with
        // 0xffff, as that could cause us to read a WORD past the end of
        // our allocation:

        Push1(pPush, NV097_ARRAY_ELEMENT32, (DWORD) *pIndexData);
        pPush += 2;
    }

    Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
    pDevice->EndPush(pPush + 2);
    pDevice->EndBracket();
}

//------------------------------------------------------------------------------
// D3DDevice_PrimeVertexCache
//
// This API is new to Xbox, and one we may actually remove once we have
// pre-compiled index buffers working.  
//
// Its function is to prime the cache for efficiently rendering meshes.
// It does T&L on the specified verticies and loads them into the vertex
// 'file' (to use Nvidia's terminology), without rendering anything.

extern "C"
void WINAPI D3DDevice_PrimeVertexCache(
    UINT VertexCount,
    CONST WORD *pIndexData)
{ 
    COUNT_API(API_D3DDEVICE_PRIMEVERTEXCACHE);
    COUNT_ADD_PERF(PERF_VERTICES, VertexCount);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if ((VertexCount == 0) || (VertexCount > 32))
        {
            DPF_ERR("Goofy VertexCount");
        }
    }

    // Whack the hardware into the correct state.  
    //
    // Note that technically we only have to do whatever would affect 
    // transform and lighting, but they'll probably do a DrawPrimitive 
    // right after this call, so any extra state set doesn't really matter.

    pDevice->SetStateVB(pDevice->m_IndexBase);

    CONST WORD* pIndex = pIndexData;
    DWORD array16Count = (VertexCount >> 1);

    PPUSH pPush = pDevice->StartPush();

    // Copy the indices.  Note that we don't actually have to special 
    // case a count of '1', since this will still work:

    PushCount(pPush, PUSHER_NOINC(NV097_ARRAY_ELEMENT16), array16Count);
    memcpy((void*) (pPush + 1), pIndex, array16Count * sizeof(DWORD));

    pPush += array16Count + 1;

    if (!(VertexCount & 1))
    {
        pDevice->EndPush(pPush);
    }
    else
    {
        // Take care of that one straggling vertex:

        Push1(pPush, NV097_ARRAY_ELEMENT32, (DWORD) *(pIndex + 2*array16Count));
        pDevice->EndPush(pPush + 2);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexData2f

extern "C"
void WINAPI D3DDevice_SetVertexData2f(
    INT Register,
    FLOAT a,
    FLOAT b)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXDATA2F);

    if (DBG_CHECK((Register < 0) || (Register > 15)))
    {
        DPF_ERR("Invalid register");
    }

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push2f(pPush, NV097_SET_VERTEX_DATA2F_M(Register, 0), a, b);

    pDevice->EndPush(pPush + 3);
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexData4f

extern "C"
void WINAPI D3DDevice_SetVertexData4f(
    INT Register,
    FLOAT a,
    FLOAT b,
    FLOAT c,
    FLOAT d)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXDATA4F);

    // Note: D3DVSDE_VERTEX is only valid for SetVertexData4f

    COUNT_ADD_PERF(PERF_VERTICES, ((Register==D3DVSDE_VERTEX) ? 1 : 0));

    if (DBG_CHECK(TRUE))
    {
        if ((Register < -1) || (Register > 15))
        {
            DPF_ERR("Invalid register");
        }
        if ((Register == D3DVSDE_VERTEX) &&
            !(g_pDevice->m_StateFlags & STATE_BEGINENDBRACKET))
        {
            DPF_ERR("D3DVSDE_VERTEX specified outside a Begin/End bracket");
        }
    }

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push4f(pPush, 
           (Register == D3DVSDE_VERTEX) 
                ? NV097_SET_VERTEX4F(0)
                : NV097_SET_VERTEX_DATA4F_M(Register, 0), 
           a, b, c, d);

    pDevice->EndPush(pPush + 5);
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexData2s

extern "C"
void WINAPI D3DDevice_SetVertexData2s(
    INT Register,
    SHORT a,
    SHORT b)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXDATA2S);

    if (DBG_CHECK((Register < 0) || (Register > 15)))
    {
        DPF_ERR("Invalid register");
    }

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, 
          NV097_SET_VERTEX_DATA2S(Register), 
          (USHORT) a | ((DWORD) b << 16));

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexData4s

extern "C"
void WINAPI D3DDevice_SetVertexData4s(
    INT Register,
    SHORT a,
    SHORT b,
    SHORT c,
    SHORT d)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXDATA4S);

    if (DBG_CHECK((Register < 0) || (Register > 15)))
    {
        DPF_ERR("Invalid register");
    }

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push2(pPush, 
          NV097_SET_VERTEX_DATA4S_M(Register, 0), 
          (USHORT) a | ((DWORD) b << 16),
          (USHORT) c | ((DWORD) d << 16));

    pDevice->EndPush(pPush + 3);
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexData4ub

extern "C"
void WINAPI D3DDevice_SetVertexData4ub(
    INT Register,
    BYTE a,
    BYTE b,
    BYTE c,
    BYTE d)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXDATA4UB);

    if (DBG_CHECK((Register < 0) || (Register > 15)))
    {
        DPF_ERR("Invalid register");
    }

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, 
          NV097_SET_VERTEX_DATA4UB(Register), 
          a | ((DWORD) b << 8) | ((DWORD) c << 16) | ((DWORD) d << 24));

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexDataColor

extern "C"
void WINAPI D3DDevice_SetVertexDataColor(
    INT Register,
    D3DCOLOR Color)
{
    COUNT_API(API_D3DDEVICE_SETVERTEXDATACOLOR);

    if (DBG_CHECK((Register < 0) || (Register > 15)))
    {
        DPF_ERR("Invalid register");
    }

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_VERTEX_DATA4UB(Register), SwapRgb(Color));

    pDevice->EndPush(pPush + 2);
}

#if PROFILE

D3DPERFEvent *g_pD3DPerfEventBeginEnd = NULL;

#endif // PROFILE

extern "C"
void WINAPI D3DDevice_Begin(
    D3DPRIMITIVETYPE PrimitiveType)
{
    COUNT_API(API_D3DDEVICE_BEGIN);

    if (DBG_CHECK((PrimitiveType > D3DPT_POLYGON) || (PrimitiveType <= 0)))
    {
        DPF_ERR("Invalid primitive type");
    }

#if DBG
    if (g_CurrentPrimitiveType != 0)
    {
        DPF_ERR("D3DDevice_Begin - Begin before a previous Begin has been Ended.");
    }

    g_CurrentPrimitiveType = PrimitiveType;
#endif

    CDevice* pDevice = g_pDevice;
    PPUSH pPush;

#if PROFILE
    ASSERT(g_pD3DPerfEventBeginEnd == NULL);
    g_pD3DPerfEventBeginEnd = D3DPERF_PerfEventStart(D3DPERFEvent_BeginEnd, TRUE);
#endif

    SetState();
    pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);

    pDevice->EndPush(pPush + 2);
    pDevice->StartBracket();
}

//------------------------------------------------------------------------------
// D3DDevice_End

extern "C"
void WINAPI D3DDevice_End()
{
    COUNT_API(API_D3DDEVICE_END);

    CDevice* pDevice = g_pDevice;
    PPUSH pPush;

#if DBG 
    if (g_CurrentPrimitiveType == 0)
    { 
        DXGRIP("D3DDevice_End - End without a matching begin.");
    }

    g_CurrentPrimitiveType = 0;
#endif 
    
    pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

    pDevice->EndPush(pPush + 2);
    pDevice->EndBracket();

#if PROFILE
    D3DPERF_PerfEventEnd(g_pD3DPerfEventBeginEnd, TRUE);
    g_pD3DPerfEventBeginEnd = NULL;
#endif
}

} // end of namespace
