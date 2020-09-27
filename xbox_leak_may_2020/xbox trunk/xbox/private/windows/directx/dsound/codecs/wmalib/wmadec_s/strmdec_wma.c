/*************************************************************************

Copyright (C) 1996-1999  Microsoft Corporation

Module Name:

	strmdec_wma.c

Abstract:

	Decoder BitStream

Author:

	Craig Dowell (craigdo@microsoft.com) 10-December-1996
	Ming-Chieh Lee (mingcl@microsoft.com) 10-December-1996
	Bruce Lin (blin@microsoft.com) 10-December-1996

Revision History:
    Wei-ge Chen (wchen@microsoft.com) 20-July-1999
    Make it in C.


*************************************************************************/

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#include <stdio.h>
#include <stdlib.h>
#include "..\common\macros.h"
#include "..\decoder\strmdec_wma.h"
#include "..\decoder\msaudiodec.h"
#include "..\..\..\dsound\dsndver.h"

#ifndef S_SUNOS5
const UInt getMask[33] = {
    0x00000000,
    0x00000001,
    0x00000003,
    0x00000007,
    0x0000000f,
    0x0000001f,
    0x0000003f,
    0x0000007f,
    0x000000ff,
    0x000001ff,
    0x000003ff,
    0x000007ff,
    0x00000fff,
    0x00001fff,
    0x00003fff,
    0x00007fff,
    0x0000ffff,
    0x0001ffff,
    0x0003ffff,
    0x0007ffff,
    0x000fffff,
    0x001fffff,
    0x003fffff,
    0x007fffff,
    0x00ffffff,
    0x01ffffff,
    0x03ffffff,
    0x07ffffff,
    0x0fffffff,
    0x1fffffff,
    0x3fffffff,
    0x7fffffff,
    0xffffffff
};
#endif

Void ibstrmInit (CWMAInputBitStream* pibstrm, Bool fAllowPackets)
{
    //added for streaming mode
    pibstrm->m_pfnGetMoreData = NULL;
    pibstrm->m_dwUser  = 0;
    pibstrm->m_dwOwner = 0;    
    pibstrm->m_dwHeaderBuf   = 0;
    pibstrm->m_pBufferBegin  = NULL;
    pibstrm->m_cbBuflenBegin = 0;    // used in association of m_pBufferBegin	
    //end of streaming mode

    pibstrm->m_pBuffer = NULL;
    pibstrm->m_cbBuflen = 0;
    pibstrm->m_dwDot = 0;
    pibstrm->m_dwBitsLeft = 0;
    pibstrm->m_iPrevPacketNum = (1 << NBITS_PACKET_CNT) - 1; // Keep -1 spacing w/ curr pkt num
    pibstrm->m_fAllowPackets = fAllowPackets;
    pibstrm->m_fSuppressPacketLoss = WMAB_TRUE; // Suppress first packet from loss detection
}

Void ibstrmReset(CWMAInputBitStream *pibstrm)
{
    pibstrm->m_dwDot = 0;
    pibstrm->m_dwBitsLeft = 0;
    pibstrm->m_cbBuflen = 0;
    pibstrm->m_fSuppressPacketLoss = WMAB_TRUE;
}

Void ibstrmAttach(CWMAInputBitStream *pibstrm, U32 dwDot, U32 dwBitsLeft, U8* pbSrc, I32 cbSrc)
{
    //added for streaming mode
    pibstrm->m_dwHeaderBuf   = 0;
    pibstrm->m_pBufferBegin  = NULL;
    pibstrm->m_cbBuflenBegin = 0;    // used in association of m_pBufferBegin	
    //end of streaming mode

    pibstrm->m_pBuffer = pbSrc;
    pibstrm->m_cbBuflen = cbSrc;
    pibstrm->m_dwDot = dwDot;
    pibstrm->m_dwBitsLeft = dwBitsLeft;
}

//this peekbits doens't generate ON_HOLD signal
WMARESULT ibstrmPeekBitsNonStop (CWMAInputBitStream *pibstrm, register UInt dwNumBits,
                          U32 *piRetBits)
{
    I16 cBitExtra;
    WMARESULT   wmaResult = WMA_OK;

    assert (dwNumBits <= 24); //only works for sure under this
    //make sure there is enougth data in dwDot for peek
    while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0) {
        pibstrm->m_dwDot <<= 8;
        pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
        --(pibstrm->m_cbBuflen);
        pibstrm->m_dwBitsLeft += 8;    
    }
    if (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen <= 0)
    {
        U32         iBufLen;
        TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData (pibstrm, &iBufLen));
    }

    //if enough take the data and go home; else take what ever is left
    if (pibstrm->m_dwBitsLeft < dwNumBits)
        dwNumBits = pibstrm->m_dwBitsLeft;
    cBitExtra = (I16) pibstrm->m_dwBitsLeft - (I16) dwNumBits;

    assert(NULL != piRetBits); // Avoid conditionals
    *piRetBits = (pibstrm->m_dwDot >> cBitExtra) << (32 - dwNumBits);

exit:
    return wmaResult;
}

WMARESULT ibstrmPeekBits (CWMAInputBitStream *pibstrm, register UInt dwNumBits,
                          U32 *piRetBits)
{
    I16 cBitExtra;
    WMARESULT   wmaResult = WMA_OK;

    assert (dwNumBits <= 24); //only works for sure under this
    //make sure there is enougth data in dwDot for peek
    while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0) {
        pibstrm->m_dwDot <<= 8;
        pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
        --(pibstrm->m_cbBuflen);
        pibstrm->m_dwBitsLeft += 8;    
    }
    if (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen <= 0)
    {
        U32         iBufLen;
        TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData (pibstrm, &iBufLen));
    }

    //if enough take the data and go home; else take what ever is left
    if (pibstrm->m_dwBitsLeft < dwNumBits)
    {
        wmaResult = WMA_E_ONHOLD;
        TraceResult(wmaResult);
        goto exit;
    }

    cBitExtra = (I16) pibstrm->m_dwBitsLeft - (I16) dwNumBits;
    assert(NULL != piRetBits); // Avoid conditionals
    *piRetBits = (pibstrm->m_dwDot >> cBitExtra) << (32 - dwNumBits);

exit:
    return wmaResult;
}

WMARESULT ibstrmLookForBits (CWMAInputBitStream *pibstrm, UInt dwNumBits)
{
    U32         iBufLen;
    WMARESULT   wmaResult = WMA_OK;

    assert (dwNumBits <= 32); //only case that works for certain
    if (dwNumBits > pibstrm->m_dwBitsLeft + pibstrm->m_cbBuflen * 8) 
    {
        //load up everything
        while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0) {
            pibstrm->m_dwDot <<= 8;
            pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
            --(pibstrm->m_cbBuflen);
            pibstrm->m_dwBitsLeft += 8;    
        }
        TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData (pibstrm, &iBufLen));
    }

    if (dwNumBits > pibstrm->m_dwBitsLeft + pibstrm->m_cbBuflen * 8 )
    {
        wmaResult = WMA_E_ONHOLD;
        TraceResult(wmaResult);
        goto exit;
    }

exit:
    return wmaResult;
}

WMARESULT ibstrmFlushBits (CWMAInputBitStream *pibstrm, register UInt dwNumBits) 
{
 
    WMARESULT wmaResult = WMA_OK;

#if defined(_DEBUG) || defined(BITRATE_REPORT)
    g_cBitGet += dwNumBits;
#endif  // _DEBUG || BITRATE_REPORT

    assert (dwNumBits <= 24); //only works for sure under this
    //make sure there is enougth data in dwDot for peek
    while (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen > 0) {
        pibstrm->m_dwDot <<= 8;
        pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
        --(pibstrm->m_cbBuflen);
        pibstrm->m_dwBitsLeft += 8;    
    }
    if (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen <= 0)
    {
        U32 iBufLen;

        TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData (pibstrm, &iBufLen));
    }

    //take the data and go home; or we have to pause
    if (dwNumBits <= pibstrm->m_dwBitsLeft)
        pibstrm->m_dwBitsLeft -= dwNumBits; 
    else 
        TRACEWMA_EXIT (wmaResult, WMA_E_ONHOLD);

exit:
    return wmaResult;
}

WMARESULT prvFinalLoad(CWMAInputBitStream* pibstrm,
                       UInt             dwRetval,
                       UInt             dwBitsToAdd,
                       U32             *piResult)
{
    WMARESULT   wmaResult = WMA_OK;
    U32         iBufLen;
    UInt        dwPosition = 4;

    pibstrm->m_dwDot = 0;
    pibstrm->m_dwBitsLeft = 0;

    for (; pibstrm->m_cbBuflen>0;) {
        pibstrm->m_dwDot <<= 8;
        pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
        --(pibstrm->m_cbBuflen);
        pibstrm->m_dwBitsLeft += 8;
    }

    TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData (pibstrm, &iBufLen));

    if (iBufLen != 0) 
    { 
        while (pibstrm->m_dwBitsLeft < dwBitsToAdd) 
        {
            pibstrm->m_dwDot <<= 8;
            pibstrm->m_dwDot |= *(pibstrm->m_pBuffer)++;
            --(pibstrm->m_cbBuflen);
            pibstrm->m_dwBitsLeft += 8;
            assert (pibstrm->m_dwBitsLeft <= 32);
        }
    }

exit:
    dwRetval <<= dwBitsToAdd;
    dwRetval |= (pibstrm->m_dwDot >> (pibstrm->m_dwBitsLeft - dwBitsToAdd)) & getMask[dwBitsToAdd];
    pibstrm->m_dwBitsLeft -= dwBitsToAdd;
    *piResult = dwRetval;

    return wmaResult; // bugbug: propagating the error screws up many things. FIX LATER
    //return WMA_OK;
}


WMARESULT ibstrmGetMoreData(CWMAInputBitStream *pibs, U32 *piBufLen)
{
    //a new packet
    I32 iFirstBit = 0;
    U8 temp;

    WMARESULT   wmaResult;
    Bool        fNewPacket;

    assert (pibs->m_cbBuflen == 0);
    TRACEWMA_EXIT(wmaResult, pibs->m_pfnGetMoreData(&pibs->m_pBuffer,
        (U32 *)&pibs->m_cbBuflen, pibs->m_dwUser, pibs->m_pBufferExtra));
	pibs->m_pBuffer = pibs->m_pBufferExtra;
    fNewPacket = (wmaResult == WMA_S_NEWPACKET);

    assert(NULL != piBufLen); // Cut down on expensive conditionals
    if(pibs->m_cbBuflen == 0)
    {
        WMA_set_fNoMoreData(pibs->m_dwOwner, 1);
        *piBufLen = 0;
        wmaResult = WMA_OK;
        goto exit;
    }
    WMA_set_fNoMoreData(pibs->m_dwOwner, 0);

#ifdef WMA2CMP
    {   // write out bits to a .cmp file
        extern FILE *pfWma2Cmp;       // file (and flag) to output .cmp file
        if (pfWma2Cmp != NULL)
        {
            size_t uWritten = fwrite(pibs->m_pBuffer,pibs->m_cbBuflen,1,pfWma2Cmp);
            // Asserts are a lousy way to report errors, but this is for internal test purposes only
            assert( uWritten == 1 );
        }
    }
#endif

    if (fNewPacket)
    {
        pibs->m_pBufferBegin = pibs->m_pBuffer;
        pibs->m_cbBuflenBegin = pibs->m_cbBuflen;
    }

    if (WMAB_FALSE == pibs->m_fAllowPackets)
    {
        // In non-superframe mode, provide a running packet count that wraps around (to avoid I32 vs. U32 issues)
        if (fNewPacket)
        {
            assert(((~(NONSUPER_WRAPAROUND - 1) << 1) & NONSUPER_WRAPAROUND) == 0); // assert Pwr of 2
            pibs->m_dwHeaderBuf = (pibs->m_dwHeaderBuf + 1) & (NONSUPER_WRAPAROUND - 1);
        }
    }
    else if (fNewPacket) //set up packet header in superframe mode
    {
        Int iPrevPacketNum, iNextPacketNum;
        unsigned char nHdrBits = WMA_get_nHdrBits(pibs->m_dwOwner); // this only updates per file
        const int cPacketNumBitsRS = (BITS_PER_DWORD - NBITS_PACKET_CNT);

        iFirstBit = nHdrBits % 8;
        assert (pibs->m_pBuffer != NULL); //always get a valid one
        pibs->m_dwHeaderBuf = 0;
        pibs->m_dwHeaderBuf = (pibs->m_pBuffer[0]<<24)|(pibs->m_pBuffer[1]<<16)|(pibs->m_pBuffer[2]<<8)|pibs->m_pBuffer[3];
        pibs->m_pBuffer += nHdrBits / 8;
        pibs->m_cbBuflen -= nHdrBits / 8;
        assert (nHdrBits < BITS_PER_DWORD);
        assert (pibs->m_dwBitsLeft <= 24);  //so that we have enough to save the fractional byte that would otherwise be lost

        // Now we should check that we didn't lose a packet
        iNextPacketNum = (pibs->m_dwHeaderBuf >> cPacketNumBitsRS);
        iPrevPacketNum = pibs->m_iPrevPacketNum;
        pibs->m_iPrevPacketNum = iNextPacketNum;
        if (WMAB_FALSE == pibs->m_fSuppressPacketLoss)
        {
            if (WMAB_FALSE == (iNextPacketNum - iPrevPacketNum == 1 || 
//                iNextPacketNum - iPrevPacketNum == 0 ||
                iNextPacketNum - iPrevPacketNum + (1 << NBITS_PACKET_CNT) == 1))
            {
                // PACKET LOSS: Return error. Next call to DecodeInfo will cue to next frame
                *piBufLen = 0;
				pibs->m_pBuffer = pibs->m_pBufferBegin + 4;
				pibs->m_cbBuflen = pibs->m_cbBuflenBegin - 4;

				pibs->m_dwDot = pibs->m_dwHeaderBuf;
				assert (nHdrBits <= 32);
				pibs->m_dwBitsLeft = 32 - nHdrBits;

                TRACEWMA_EXIT (wmaResult, WMA_E_LOSTPACKET);
            }
        }
        else
            // Avoid checking for packet loss, eg, after a seek
            pibs->m_fSuppressPacketLoss = WMAB_FALSE;
    }

    while (pibs->m_cbBuflen > 0 && pibs->m_dwBitsLeft <= 24)    {
        temp = (*pibs->m_pBuffer++);
        pibs->m_cbBuflen--;

        //zeros out the top (not valid) bits
        temp = temp<<iFirstBit;
        temp = temp>>iFirstBit;
        pibs->m_dwDot = (pibs->m_dwDot<<(8 - iFirstBit))|temp;

        pibs->m_dwBitsLeft += (8 - iFirstBit);
        iFirstBit = 0;
    }
    *piBufLen = pibs->m_cbBuflen;

exit:
    return wmaResult;
}

Void ibstrmResetPacket(CWMAInputBitStream *pibs)
{
    U8 nHdrBits = WMA_get_nHdrBits(pibs->m_dwOwner); // this only updates per file
    I32 iFirstBit;
    U8 temp;

    assert (pibs->m_pBuffer != 0);
    assert (WMA_get_fNoMoreData(pibs->m_dwOwner) == WMAB_FALSE);
    //skip the packet header
    iFirstBit = nHdrBits % 8;
    pibs->m_pBuffer = pibs->m_pBufferBegin;
    pibs->m_pBuffer += nHdrBits / 8;
    pibs->m_cbBuflen = pibs->m_cbBuflenBegin - nHdrBits / 8;

    temp = (*pibs->m_pBuffer++);
    pibs->m_cbBuflen--;

    //zeros out the top (not valid) bits
    temp = temp<<iFirstBit;
    temp = temp>>iFirstBit;
    pibs->m_dwDot = temp;
    pibs->m_dwBitsLeft = (8 - iFirstBit);
}

WMARESULT ibstrmGetBits (CWMAInputBitStream* pibstrm, register UInt dwNumBits,
                         U32 *piResult)
{
    WMARESULT   wmaResult = WMA_OK;
    register UInt dwRetval;
    register UInt dwShift;

#if defined(_DEBUG) || defined(BITRATE_REPORT)
    g_cBitGet += dwNumBits;
#endif  // _DEBUG || BITRATE_REPORT

    if (pibstrm->m_dwBitsLeft <= 24 && pibstrm->m_cbBuflen <= 0)
    {
        U32         iBufLen;
        TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData (pibstrm, &iBufLen));
    }

    //
    // This is a little unwieldly, but make sure the end-of-file test falls
    // through in the usual (not end-of-file) case to avoid CPU pipeline stalls
    // due to a branch (or mispredictions on more sophisticated processors).
    //
    if (((UInt) (pibstrm->m_cbBuflen << 3) >= dwNumBits || 
        pibstrm->m_dwBitsLeft + (pibstrm->m_cbBuflen << 3) >= dwNumBits)) {
        //
        // Do the most common case first.  If this doesn't play, we have one branch
        // to get to the next most common case (usually 1/32 of the time in the case
        // of the codec doing a huffman decode).  Note that we use a mask array to
        // avoid a special case branch when the bitcount is 32 (even though this is
        // relatively unlikely) since a left shift operation where the shift count
        // is equal to or greater than the number of bits in the destination is
        // undefined.
        //
        if (dwNumBits <= pibstrm->m_dwBitsLeft) {
            pibstrm->m_dwBitsLeft -= dwNumBits;
			*piResult = (pibstrm->m_dwDot >> pibstrm->m_dwBitsLeft) & getMask[dwNumBits];
            wmaResult = WMA_OK;
            goto exit;
        }
        //
        // The next most common case is when we have lots of data left in the buffer.
        // and we can fully load (i.e., all 32-bits) our accumulator.  This is
        // hard-wired to allow an optimizer to go crazy with all of the constants.
        // Note that the data is byte-swapped on the way in.
        //
        dwRetval = pibstrm->m_dwDot & getMask[pibstrm->m_dwBitsLeft];
        dwShift = dwNumBits - pibstrm->m_dwBitsLeft;

        if (pibstrm->m_cbBuflen >= 4) {
            register UInt     dwAcc;

            dwAcc = (UInt)pibstrm->m_pBuffer[3];
            dwAcc |= (UInt)(pibstrm->m_pBuffer[2]) << 8;
            dwAcc |= (UInt)(pibstrm->m_pBuffer[1]) << 16;
            dwAcc |= (UInt)(pibstrm->m_pBuffer[0]) << 24;
            pibstrm->m_dwDot = dwAcc;
            pibstrm->m_pBuffer += 4;
            pibstrm->m_cbBuflen -= 4;
            dwRetval <<= dwShift;
            dwRetval |= (dwAcc >> (32 - dwShift));// & getMask[dwShift];
            pibstrm->m_dwBitsLeft = 32 - dwShift;
            *piResult = dwRetval;
            wmaResult = WMA_OK;
            goto exit;
        }
        //
        // The final, and least likely case, is when we're at the end of the buffer.
        // Since there's really no point in having this inlined since it'll only
        // happen once, I'll call a function to make it happen and save the space
        // in the inline instances of getBits().
        //
		wmaResult = prvFinalLoad(pibstrm, dwRetval, dwShift, piResult);
        TraceError(wmaResult);
        goto exit;
    }
	else {
        *piResult = 0;
        TRACEWMA_EXIT (wmaResult, WMA_E_ONHOLD);
    }

exit:
    return wmaResult;
}
