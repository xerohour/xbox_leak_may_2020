/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       bits.c
 *  Content:    walks the video frame portion of the input buffer
 *
 ****************************************************************************/

#include <xtl.h>
#include <xmv.h>
#include <xdbg.h>

#include "decoder.h"

#if DBG

DWORD g_TotalBitsRead;

#endif DBG

/*
 * Read one bit 
 */

DWORD ReadOneBit
(
    XMVDecoder *pDecoder
)
{

#if DBG
    g_TotalBitsRead += 1;
#endif DBG

    // Do the trivial thing if all of the bits fit into our little cache.
    if (pDecoder->BitsRemaining)
    {
        pDecoder->BitsRemaining--;

        return (pDecoder->BitCache >> pDecoder->BitsRemaining) & 1;
    }

    // Otherwise the read is split up between two blocks.
    else
    {
        // Load up the next part, swapping the bytes as we go.
        *((BYTE *)&pDecoder->BitCache + 3) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 2) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 1) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 0) = *pDecoder->pDecodingPosition++;

        // Remember how much we'll have left.
        pDecoder->BitsRemaining = 31;

        // Return the result.
        return (pDecoder->BitCache >> 31) & 1;
    }
}

/*
 * Read a tri-state bit encoding.
 */

DWORD ReadTriStateBits
(
    XMVDecoder *pDecoder
)
{
    DWORD Bits;

    Bits = ReadOneBit(pDecoder);

    if (Bits)
    {
        return ReadOneBit(pDecoder) + 1;
    }
    else
    {
        return 0;
    }
}

/*
 * Read some bits from the buffer
 */

DWORD ReadBits
(   
    XMVDecoder *pDecoder, 
    DWORD Bits
)
{
    DWORD FirstPart;
    DWORD SecondPartBits;

    ASSERT(Bits > 0 && Bits <= 32);

#if DBG
    g_TotalBitsRead += Bits;
#endif DBG

    // Do the trivial thing if all of the bits fit into our little cache.
    if (Bits <= pDecoder->BitsRemaining)
    {
        pDecoder->BitsRemaining -= Bits;

        return (pDecoder->BitCache >> pDecoder->BitsRemaining) & (0xFFFFFFFF >> (32 - Bits));
    }

    // Otherwise the read is split up between two blocks.
    else
    {
        // Save what we have.  Note the second multiplication is because the
        // intel processor only knows about shifts between 0 and 31 bits, not
        // 32.
        //
        FirstPart  = pDecoder->BitCache & (0xFFFFFFFF >> (32 - pDecoder->BitsRemaining));
        FirstPart *= !!pDecoder->BitsRemaining;

        SecondPartBits = Bits - pDecoder->BitsRemaining;
    
        // Load up the next part, swapping the bytes as we go.
        *((BYTE *)&pDecoder->BitCache + 3) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 2) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 1) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 0) = *pDecoder->pDecodingPosition++;

        // Remember how much we'll have left.
        pDecoder->BitsRemaining = 32 - SecondPartBits;

        // Return the result.
        return (FirstPart << SecondPartBits) | (pDecoder->BitCache >> pDecoder->BitsRemaining);
    }
}

/*
 * Read some bits from the buffer without consuming them.
 */

DWORD PeekBits
(
    XMVDecoder *pDecoder, 
    DWORD Bits
)
{
    DWORD FirstPart;
    DWORD SecondPartBits;
    DWORD TemporaryCache;

    ASSERT(Bits > 0 && Bits <= 32);

    // Do the trivial thing if all of the bits fit into our little cache.
    if (Bits <= pDecoder->BitsRemaining)
    {
        return (pDecoder->BitCache >> (pDecoder->BitsRemaining - Bits)) & (0xFFFFFFFF >> (32 - Bits));
    }

    // Otherwise the read is split up between two blocks.
    else
    {
        // Save what we have.  Note the second multiplication is because the
        // intel processor only knows about shifts between 0 and 31 bits, not
        // 32.  If BitsRemaining is zero then we need to do extra work to
        // ensure that FirstPart is also zero.
        //
        FirstPart  = pDecoder->BitCache & (0xFFFFFFFF >> (32 - pDecoder->BitsRemaining));
        FirstPart *= !!pDecoder->BitsRemaining;

        SecondPartBits = Bits - pDecoder->BitsRemaining;
    
        // Load up the next part, swapping the bytes as we go.
        *((BYTE *)&TemporaryCache + 3) = *(pDecoder->pDecodingPosition + 0);
        *((BYTE *)&TemporaryCache + 2) = *(pDecoder->pDecodingPosition + 1);
        *((BYTE *)&TemporaryCache + 1) = *(pDecoder->pDecodingPosition + 2);
        *((BYTE *)&TemporaryCache + 0) = *(pDecoder->pDecodingPosition + 3);

        // Return the result.
        return (FirstPart << SecondPartBits) | (TemporaryCache >> (32 - SecondPartBits));
    }
}

/*
 * Consume some bits from the buffer without reading them.
 */

void SkipBits
(
    XMVDecoder *pDecoder, 
    DWORD Bits
)
{
#if DBG
    g_TotalBitsRead += Bits;
#endif DBG

    // Do the trivial thing if all of the bits are in our little cache.
    if (Bits <= pDecoder->BitsRemaining)
    {
        pDecoder->BitsRemaining -= Bits;
    }

    // Otherwise we need to skip along to another buffer.
    else
    {
        // Forget about the bits we currently have loaded.
        Bits -= pDecoder->BitsRemaining;

        // If we're skipping more than 8 bits then just zoom on past.
        pDecoder->pDecodingPosition += Bits / 8;

        // Load up the next part, swapping the bytes as we go.
        *((BYTE *)&pDecoder->BitCache + 3) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 2) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 1) = *pDecoder->pDecodingPosition++;
        *((BYTE *)&pDecoder->BitCache + 0) = *pDecoder->pDecodingPosition++;

        // Remember how much we'll have left.
        pDecoder->BitsRemaining = 32 - (Bits % 8);
    }
}
