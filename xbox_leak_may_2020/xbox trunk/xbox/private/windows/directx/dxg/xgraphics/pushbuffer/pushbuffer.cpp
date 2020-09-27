/*==========================================================================;
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pushbuffer.c
 *  Content:    push buffer compiler.
 *
 ****************************************************************************/

#if XBOX
    #include "xtl.h"
#else 
    #include "windows.h"
    #include "d3d8-xbox.h"
#endif

#include "xgraphics.h"
#include "debug.h"

namespace XGRAPHICS
{

// PUSHBUFFER_RETURN_RESERVE is the number of bytes reserved by D3D at
// the end of the push-buffer for the return-JUMP instruction:

#define PUSHBUFFER_RETURN_RESERVE 4

// ARRAY_ELEMENT16_BATCH is the maximum number of dwords that can be
// encoded by D3DPUSH_ENCODE:

#define ARRAY_ELEMENT16_BATCH 2047

#define D3DPUSH_ARRAY_ELEMENT16         0x00001800
#define D3DPUSH_ARRAY_ELEMENT32         0x00001808
#define D3DPUSH_NO_OPERATION            0x00000100

//------------------------------------------------------------------------------
// XGCompileDrawIndexedVertices

extern "C"
HRESULT WINAPI XGCompileDrawIndexedVertices(
    void *pBuffer,
    DWORD *pSize, // In-out
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST WORD *pIndexData
    )
{
    BOOL writeBuffer = FALSE;
    DWORD* pPush = NULL;
    DWORD countCopy = VertexCount;
    CONST WORD *pDataCopy = pIndexData;

    for (DWORD loop = 0; loop < 2; loop++)
    {
        DWORD slots = 0;
    
        if (writeBuffer)
        {
            pPush[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
            pPush[1] = PrimitiveType;
        }
        pPush += 2;
        slots += 1;
    
        while (VertexCount > 2 * ARRAY_ELEMENT16_BATCH)
        {
            if (writeBuffer)
            {
                pPush[0] = D3DPUSH_NOINCREMENT_FLAG
                         | D3DPUSH_ENCODE(D3DPUSH_ARRAY_ELEMENT16, ARRAY_ELEMENT16_BATCH);
        
                memcpy(pPush + 1, pIndexData, ARRAY_ELEMENT16_BATCH * sizeof(DWORD));
            }
    
            pIndexData += 2 * ARRAY_ELEMENT16_BATCH;
            VertexCount -= 2 * ARRAY_ELEMENT16_BATCH;
    
            pPush += ARRAY_ELEMENT16_BATCH + 1;
            slots += ARRAY_ELEMENT16_BATCH;
        }
    
        // Note that it's okay if we send an Element16 command with zero entries
        // (for the case where the vertex count given to us is less than 3):
    
        DWORD arrayCount = VertexCount >> 1;
        if (writeBuffer)
        {
            pPush[0] = D3DPUSH_NOINCREMENT_FLAG 
                     | D3DPUSH_ENCODE(D3DPUSH_ARRAY_ELEMENT16, arrayCount);
        
            memcpy(pPush + 1, pIndexData, arrayCount * sizeof(DWORD));
        }
    
        pIndexData += 2 * arrayCount;

        pPush += arrayCount + 1;
        slots += arrayCount;
    
        // Handle the last odd index.  Since we only have one index left, we
        // obviously can't whack ELEMENT16, since that takes pairs of indices...
    
        if (VertexCount & 1)
        {
            if (writeBuffer)
            {
                pPush[0] = D3DPUSH_ENCODE(D3DPUSH_ARRAY_ELEMENT32, 1);
                pPush[1] = (DWORD) (*pIndexData);
            }
    
            pPush += 2;
            slots += 1;
        }
    
        if (writeBuffer)
        {
            pPush[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
            pPush[1] = 0;
        }

        pPush += 2;
        slots += 1;

        // 'totalSlots' is the total number of instruction pairs (offset, value)
        // that fit in the GPU's read-ahead FIFO cache.
        //
        // (1024 + 32) should be the same as PUSHER_MINIMUM_RUN_OFFSET:
    
        const INT totalSlots = (1024 + 32)/8;
        INT nops = totalSlots - slots;
        if (nops > 0)
        {
            // We need to stick in some NOPs so that the GPU read-ahead hasn't
            // already read the return-JUMP before we've had a chance to
            // do its fixup:

            if (writeBuffer)
            {
                pPush[0] = D3DPUSH_NOINCREMENT_FLAG
                         | D3DPUSH_ENCODE(D3DPUSH_NO_OPERATION, nops);
                ZeroMemory(pPush + 1, nops * sizeof(DWORD));
            }
    
            pPush += nops + 1;
            slots += nops;
        }

        if (slots < totalSlots)
            DXGRIP("Invalid internal padding");

        if (loop == 0)
        {
            DWORD sizeNeeded = (DWORD) pPush + PUSHBUFFER_RETURN_RESERVE;
            DWORD givenSize = *pSize;

            *pSize = sizeNeeded;

            if (pBuffer == NULL)
                return S_OK;

            if (givenSize < sizeNeeded)
                return D3DERR_BUFFERTOOSMALL;

            writeBuffer = TRUE;

            pPush = (DWORD*) pBuffer;
            VertexCount = countCopy;
            pIndexData = pDataCopy;
        }
    }

    return S_OK;
}

}
