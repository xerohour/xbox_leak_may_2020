/*************************************************************************

Copyright (C) 1996, 1997  Microsoft Corporation

Module Name:

    strmdec.hpp

Abstract:

    Decoder BitStream

Author:

    Craig Dowell (craigdo@microsoft.com) 10-December-1996
    Ming-Chieh Lee (mingcl@microsoft.com) 10-December-1996
    Bruce Lin (blin@microsoft.com) 10-December-1996
    Chuang Gu (chuanggu@microsoft.com) 2-Feb-2000

Revision History:
    Bruce Lin (blin@microsoft.com) 02-May-1997
    Add getMaxBits()

*************************************************************************/

#ifndef __STRMDEC_HPP_
#define __STRMDEC_HPP_

#if DBG
extern DWORD g_TotalBits;
#endif DBG

Class istream;

// Hide Debugging/Analysis macros called HUFFMANGET_DBG... in another file
#include "strmdecdbg.hpp"

#ifdef __STREAMING_MODE_DECODE_
    extern  I32_WMV getVideoData(I32_WMV ioffset, U8_WMV** ppBuffer, U32_WMV* dwBufferLen, Bool_WMV* bNotEndOfFrame);
#endif

#define VALID               0
#define END_OF_FILE         1
#define ILLEGAL_PATTERN     3
//#define EOF                 (-1)

extern U32_WMV GetMask_WMV [33];

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

#if defined(OPT_HUFFMAN_GET_WMV) && defined(UNDER_CE) && !defined(DEBUG)
#   define FORCEINLINE __forceinline
#else
#   define FORCEINLINE inline
#endif

inline U32_WMV ReadIntNBO(U8_WMV *pos)
{
  U32_WMV tmp;
  tmp=*(U32_WMV *)pos;
  return (tmp<<24)|(tmp>>24)|((tmp&0xff00)<<8)|((tmp>>8)&0xff00);
}

//class CVideoObjectDecoder ;
//class CDCTTableInfo_Dec;

typedef struct tagCInputBitStream_WMV {


#   if defined(OPT_HUFFMAN_GET_WMV)
        // IMPORTANT: these components must be at the their exact offsets to the base of the class, or the implementation won't work
        // - weidongz Nov.3, 2000
        union
        {
            U8_WMV  *m_pBuffer;
            I32_WMV OPT_HUFFMAN_GET_WMV_CBSbase;
        };
        I32_WMV     m_cbBuflen;
        U32_WMV     m_currBits;
        U32_WMV     m_nextBits;

        union
        {
            I32_WMV m_BNS_base;
            struct 
            {
                I16_WMV m_BNS_base16L;
                I16_WMV m_BNS_base16H;
            } m_BNS16;
            struct 
            {
                U8_WMV  m_wBitsLeft;
                U8_WMV  m_wNextBitsSkip;
                I8_WMV  m_fStreamStaus;
                I8_WMV  m_stuff;
            } m_BMS8;
        };
#   else // defined(OPT_HUFFMAN_GET_WMV)
        U8_WMV      *m_pBuffer;
        I32_WMV     m_cbBuflen;
            U32_WMV    m_dwDot;
            U32_WMV    m_dwBitsLeft;
        I32_WMV     m_fStreamStaus;

#   endif // defined(OPT_HUFFMAN_GET_WMV)

#       ifdef __STREAMING_MODE_DECODE_
            Bool_WMV    m_bNotEndOfFrame;
#       endif 
    U32_WMV m_uiUserData;
}CInputBitStream_WMV;

#   ifdef OPT_HUFFMAN_GET_WMV
inline Bool_WMV BS_invalid (CInputBitStream_WMV * pThis)  { return pThis->m_BMS8.m_fStreamStaus; }
inline  Void_WMV BS_setInvalid(CInputBitStream_WMV * pThis) { pThis->m_BMS8.m_fStreamStaus = ILLEGAL_PATTERN; }
inline  U32_WMV BS_BitsLeft (CInputBitStream_WMV * pThis)  { return pThis->m_BMS8.m_wBitsLeft; }
inline  Void_WMV BS_flush (CInputBitStream_WMV * pThis) { pThis->m_BMS8.m_wBitsLeft -= (((U8_WMV)pThis->m_BMS8.m_wBitsLeft) % 8u); }
#   else
inline Bool_WMV BS_invalid (CInputBitStream_WMV * pThis)  { return pThis->m_fStreamStaus; }
inline        Void_WMV BS_setInvalid(CInputBitStream_WMV * pThis) { pThis->m_fStreamStaus = ILLEGAL_PATTERN; }
inline        U32_WMV BS_BitsLeft (CInputBitStream_WMV * pThis)  { return pThis->m_dwBitsLeft; }
inline        Void_WMV BS_flush (CInputBitStream_WMV * pThis) { pThis->m_dwBitsLeft -= (pThis->m_dwBitsLeft % 8u); }
#   endif

#       ifdef OPT_HUFFMAN_GET_WMV
I32_WMV BS_getBits2(CInputBitStream_WMV * pThis, I32_WMV dwNumBits);
I32_WMV BS_getMaxBits2(CInputBitStream_WMV * pThis, I32_WMV dwNumBits, U8_WMV* rgCodeSymSize);
U32_WMV BS_peekBits2 (CInputBitStream_WMV * pThis, const U32_WMV numBits);
Void_WMV BS_updateCurrNext(CInputBitStream_WMV * pThis);
#else
U32_WMV BS_finalLoad( CInputBitStream_WMV * pThis, U32_WMV    dwRetval, U32_WMV       dwBitsToAdd);
Void_WMV
BS_addDataToBuffer(
    CInputBitStream_WMV * pThis,
    U8_WMV          *pBuffer,
    U32_WMV        dwBuflen,
    Bool_WMV        bNotEndOfFrame
    );
#endif

inline Bool_WMV WMVDecCBGetDataWithPadding(CInputBitStream_WMV* pThis)
{
    Bool_WMV bNotEndOfFrame;
    U32_WMV  dwBufferLen;
    U8_WMV*  pBuffer;
    U8_WMV tmp[6];
    memcpy(tmp, pThis->m_pBuffer, pThis->m_cbBuflen);
    WMVDecCBGetData(pThis->m_uiUserData,
                    pThis->m_cbBuflen,
                    &pBuffer,
                    4,
                    &dwBufferLen,
                    &bNotEndOfFrame);

    if (pBuffer == NULL || (bNotEndOfFrame && dwBufferLen == 0)) {
        BS_setInvalid(pThis);
        return FALSE_WMV;
    }

    memcpy(pBuffer, tmp, pThis->m_cbBuflen);
    BS_addDataToBuffer(pThis, pBuffer, dwBufferLen, bNotEndOfFrame);
    return TRUE_WMV;
}

    FORCEINLINE U32_WMV BS_getBits (CInputBitStream_WMV * pThis, register U32_WMV     dwNumBits)
    {      
        assert((dwNumBits>0)&&(dwNumBits<32));

#if DBG
g_TotalBits += dwNumBits;
#endif

#       ifdef OPT_HUFFMAN_GET_WMV
            // assuming padding bytes always exists at the end of a frame, so don't have to worry about m_nextBits 
            // most common case
            I32_WMV wBitsLeft;
            if ((wBitsLeft=(pThis->m_BMS8.m_wBitsLeft-(I16_WMV)dwNumBits))>=0) 
            {
                // result=(m_currBits >> m_dwBitsLeft) & GetMask[dwNumBits];
                HUFFMANGET_DBG_DMP((pThis->m_currBits >> pThis->m_BMS8.m_wBitsLeft) & (((U32_WMV)0xffffffff)>>(32-dwNumBits)),0,0,0);
                pThis->m_BMS8.m_wBitsLeft=wBitsLeft;
                return (pThis->m_currBits >> wBitsLeft) & (((U32_WMV)0xffffffff)>>(32-dwNumBits));
            }
            int result=BS_getBits2( pThis, dwNumBits);
            HUFFMANGET_DBG_DMP(result,0,0,0);
            return result;
#       else
            //
            // This is a little unwieldly, but make sure the end-of-file test falls
            // through in the usual (not end-of-file) case to avoid CPU pipeline stalls
            // due to a branch (or mispredictions on more sophisticated processors).
            //
            int result;
            //
            // Do the most common case first.  If this doesn't play, we have one branch
            // to get to the next most common case (usually 1/32 of the time in the case
            // of the codec doing a huffman decode).  Note that we use a mask array to
            // avoid a special case branch when the bitcount is 32 (even though this is
            // relatively unlikely) since a left shift operation where the shift count
            // is equal to or greater than the number of bits in the destination is
            // undefined.
            //
            if (dwNumBits <= pThis->m_dwBitsLeft) {
                pThis->m_dwBitsLeft -= dwNumBits;
                int result= (pThis->m_dwDot >> pThis->m_dwBitsLeft) & GetMask_WMV[dwNumBits];
                HUFFMANGET_DBG_DMP(result,0,0,0);
                return result;
            }
            //
            // The next most common case is when we have lots of data left in the buffer.
            // and we can fully load (i.e., all 32-bits) our accumulator.  This is
            // hard-wired to allow an optimizer to go crazy with all of the constants.
            // Note that the data is byte-swapped on the way in.
            //
            register U32_WMV dwRetval = pThis->m_dwDot & GetMask_WMV[pThis->m_dwBitsLeft];
            register U32_WMV dwShift = dwNumBits - pThis->m_dwBitsLeft;
#               ifdef __STREAMING_MODE_DECODE_
                // 6 (instead of 4) is to make sure if not addData to buffer 
                // buffer still has at least 2 bytes left after taking 4 bytes away
                // 2 bytes is minimal for peeking (or showbits)
                if (pThis->m_cbBuflen <= 6 && pThis->m_bNotEndOfFrame) {
                    if (!WMVDecCBGetDataWithPadding(pThis))
                        return 0;
                }
#               endif
            if (pThis->m_cbBuflen >= 4) {
                register U32_WMV     dwAcc;

                dwAcc = (U32_WMV)pThis->m_pBuffer[3];
                dwAcc |= (U32_WMV)(pThis->m_pBuffer[2]) << 8;
                dwAcc |= (U32_WMV)(pThis->m_pBuffer[1]) << 16;
                dwAcc |= (U32_WMV)(pThis->m_pBuffer[0]) << 24;
                pThis->m_dwDot = dwAcc;
                pThis->m_pBuffer += 4;
                pThis->m_cbBuflen -= 4;
                dwRetval <<= dwShift;
                dwRetval |= (dwAcc >> (32 - dwShift));// & GetMask[dwShift];
                pThis->m_dwBitsLeft = 32 - dwShift;
                HUFFMANGET_DBG_DMP(dwRetval,0,0,0);
                return dwRetval;
            }
            //
            // The final, and least likely case, is when we're at the end of the buffer.
            // Since there's really no point in having this inlined since it'll only
            // happen once, I'll call a function to make it happen and save the space
            // in the inline instances of getBits().
            //
            result=BS_finalLoad(pThis, dwRetval, dwShift);
            HUFFMANGET_DBG_DMP(result,0,0,0);
                return result;
#       endif
    }
    
    inline U8_WMV BS_getMaxBits (CInputBitStream_WMV * pThis, register U32_WMV dwNumBits, U8_WMV* rgCodeSymSize) {

#if DBG
        g_TotalBits += dwNumBits;
#endif


        assert((dwNumBits>0)&&(dwNumBits<32));

#       ifdef OPT_HUFFMAN_GET_WMV
           I32_WMV iBitPattern, result;
           register I32_WMV bitsshift;
            // assuming padding bytes always exists at the end of a frame, so don't have to worry about m_nextBits 
            // most common case
            bitsshift=pThis->m_BMS8.m_wBitsLeft-dwNumBits ;
            if (bitsshift>=0) 
            {
                // iBitPattern = ((m_currBits >> bitsshift) & GetMask[dwNumBits])<<1; //0x00000fff;
                iBitPattern = ((pThis->m_currBits >> bitsshift) & (((U32_WMV)0xffffffff)>>(32-dwNumBits)))<<1; //0x00000fff;
                pThis->m_BMS8.m_wBitsLeft -= rgCodeSymSize[iBitPattern];
                result=rgCodeSymSize [iBitPattern+1];
                if (result == 255)
                {
                        pThis->m_BMS8.m_wBitsLeft=0;
                        pThis->m_BMS8.m_wNextBitsSkip=32;
                        pThis->m_cbBuflen=0;
                        pThis->m_BMS8.m_fStreamStaus = ILLEGAL_PATTERN;
                }
                HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern+1,2);
                HUFFMANGET_DBG_DMP(result,rgCodeSymSize,iBitPattern+1,0);
                return result;
            }
            result=BS_getMaxBits2(pThis, dwNumBits,rgCodeSymSize);
            HUFFMANGET_DBG_DMP(result,rgCodeSymSize,0,1);
            return result;
#       else
            //
            // This is a little unwieldly, but make sure the end-of-file test falls
            // through in the usual (not end-of-file) case to avoid CPU pipeline stalls
            // due to a branch (or mispredictions on more sophisticated processors).
            I32_WMV iBitPattern;
            //      //assert ( dwNumBits <=16);
                    //if (m_fEof == FALSE && ((U32_WMV) (m_cbBuflen << 3) >= dwNumBits || m_dwBitsLeft + (m_cbBuflen << 3) >= dwNumBits)) {
            //
            // Do the most common case first.  If this doesn't play, we have one branch
            // to get to the next most common case (usually 1/32 of the time in the case
            // of the codec doing a huffman decode).  Note that we use a mask array to
            // avoid a special case branch when the bitcount is 32 (even though this is
            // relatively unlikely) since a left shift operation where the shift count
            // is equal to or greater than the number of bits in the destination is
            // undefined.
            //
            if (dwNumBits <= pThis->m_dwBitsLeft) {
                iBitPattern = ((pThis->m_dwDot >> (pThis->m_dwBitsLeft - dwNumBits)) & GetMask_WMV[dwNumBits])<<1; //0x00000fff;
                pThis->m_dwBitsLeft -= rgCodeSymSize[iBitPattern];
                if (rgCodeSymSize [iBitPattern+1] == 255)
                    pThis->m_fStreamStaus = ILLEGAL_PATTERN;
                HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
                HUFFMANGET_DBG_DMP(rgCodeSymSize [iBitPattern+1],rgCodeSymSize,iBitPattern+1,1);
                return rgCodeSymSize [iBitPattern+1];
            }
            //
            // The next most common case is when we have lots of data left in the buffer.
            // and we can fully load (i.e., all 32-bits) our accumulator.  This is
            // hard-wired to allow an optimizer to go crazy with all of the constants.
            // Note that the data is byte-swapped on the way in.
            //

#           ifdef __STREAMING_MODE_DECODE_
                if (pThis->m_cbBuflen <= 4 && pThis->m_bNotEndOfFrame) {
                    if (!WMVDecCBGetDataWithPadding(pThis))
                        return 255;
                }
#           endif
            if (pThis->m_cbBuflen >= 2) {
                pThis->m_dwDot <<= 16;
                pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[1]);
                pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[0]) << 8;
                pThis->m_pBuffer += 2;
                pThis->m_cbBuflen -= 2;
                pThis->m_dwBitsLeft += 16;
                iBitPattern = ((pThis->m_dwDot >> (pThis->m_dwBitsLeft - dwNumBits)) & GetMask_WMV[dwNumBits])<<1;
                pThis->m_dwBitsLeft -= rgCodeSymSize[iBitPattern];
                if (rgCodeSymSize[iBitPattern+1] == 255)
                    pThis->m_fStreamStaus = ILLEGAL_PATTERN;
                HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
                HUFFMANGET_DBG_DMP(rgCodeSymSize [iBitPattern+1],rgCodeSymSize,iBitPattern+1,1);
                return rgCodeSymSize[iBitPattern+1];
            }
            //
            // The final, and least likely case, is when we're at the end of the buffer.
            // Since there's really no point in having this inlined since it'll only
            // happen once, I'll call a function to make it happen and save the space
            // in the inline instances of getBits().
            //
            for (; pThis->m_cbBuflen;) {
                pThis->m_dwDot <<= 8;
                pThis->m_dwDot |= *pThis->m_pBuffer++;
                --pThis->m_cbBuflen;
                pThis->m_dwBitsLeft += 8;
            }
            if (pThis->m_dwBitsLeft >= dwNumBits){ 
                iBitPattern = ((pThis->m_dwDot >> (pThis->m_dwBitsLeft - dwNumBits)) & GetMask_WMV[dwNumBits])<<1;
                pThis->m_dwBitsLeft -= rgCodeSymSize[iBitPattern];
                if (rgCodeSymSize[iBitPattern+1] == 255)
                    pThis->m_fStreamStaus = ILLEGAL_PATTERN;
                HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
                HUFFMANGET_DBG_DMP(rgCodeSymSize [iBitPattern+1],rgCodeSymSize,iBitPattern+1,1);
                return rgCodeSymSize[iBitPattern+1];
            }
            else {
                iBitPattern = (pThis->m_dwDot & GetMask_WMV[pThis->m_dwBitsLeft]) << (dwNumBits - pThis->m_dwBitsLeft + 1);
                if (pThis->m_dwBitsLeft >= (U32_WMV) rgCodeSymSize[iBitPattern]){
                    pThis->m_dwBitsLeft -= rgCodeSymSize[iBitPattern];
                    if (rgCodeSymSize[iBitPattern+1] == 255)
                        pThis->m_fStreamStaus = ILLEGAL_PATTERN;
                    HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
                    HUFFMANGET_DBG_DMP(rgCodeSymSize [iBitPattern+1],rgCodeSymSize,iBitPattern+1,1);
                    return rgCodeSymSize[iBitPattern+1];
                }
                else {
                    pThis->m_fStreamStaus = END_OF_FILE;
                    HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
                    HUFFMANGET_DBG_DMP(255,rgCodeSymSize,iBitPattern+1,1);
                    return 255;
                }
            }
#       endif
    }

    //inline U32_WMV getBit () {
    FORCEINLINE U32_WMV BS_getBit (CInputBitStream_WMV * pThis) {

#if DBG
g_TotalBits += 1;
#endif

#       ifdef OPT_HUFFMAN_GET_WMV
            // assuming padding bytes always exists at the end of a frame, so don't have to worry about m_nextBits 
            I32_WMV wBitsLeft;
            if ((wBitsLeft=(pThis->m_BMS8.m_wBitsLeft-1))>=0) 
            {
                // result=(m_currBits >> m_dwBitsLeft) & GetMask[dwNumBits];
                pThis->m_BMS8.m_wBitsLeft=wBitsLeft;
                return (pThis->m_currBits >> wBitsLeft) & 1;
            }
            return BS_getBits2(pThis, 1);
#       else
            int result;
            //
            // This is a little unwieldly, but make sure the end-of-file test falls
            // through in the usual (not end-of-file) case to avoid CPU pipeline stalls
            // due to a branch. 
            //
            //
            // Do the most common case first.  If this doesn't play, we have one branch
            // to get to the next most common case (usually 1/32 of the time in the case
            // of the codec doing a huffman decode).  Note that we use a mask array to
            // avoid a special case branch when the bitcount is 32 (even though this is
            // relatively unlikely) since a left shift operation where the shift count
            // is equal to or greater than the number of bits in the destination is
            // undefined.
            //
            if (pThis->m_dwBitsLeft >= 1) {
                pThis->m_dwBitsLeft--;
                result=(pThis->m_dwDot >> pThis->m_dwBitsLeft) & 0x00000001;
                HUFFMANGET_DBG_DMP(result,0,0,0);
                return result;
            }
            //
            // The next most common case is when we have lots of data left in the buffer.
            // and we can fully load (i.e., all 32-bits) our accumulator.  This is
            // hard-wired to allow an optimizer to go crazy with all of the constants.
            // Note that the data is byte-swapped on the way in.
            //
#               ifdef __STREAMING_MODE_DECODE_
                // 6 (instead of 4) is to make sure if not addData to buffer 
                // buffer still has at least 2 bytes left after taking 4 bytes away
                // 2 bytes is minimal for peeking (or showbits)
                if (pThis->m_cbBuflen <= 6 && pThis->m_bNotEndOfFrame) {
                    if (!WMVDecCBGetDataWithPadding(pThis))
                        return 0;
                }
#               endif
            if (pThis->m_cbBuflen >= 4) {
                pThis->m_dwDot = (U32_WMV)pThis->m_pBuffer[3];
                pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[2]) << 8;
                pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[1]) << 16;
                pThis->m_dwDot |= (U32_WMV)(pThis->m_pBuffer[0]) << 24;
                pThis->m_pBuffer += 4;
                pThis->m_cbBuflen -= 4;
                pThis->m_dwBitsLeft = 31;
                HUFFMANGET_DBG_DMP((pThis->m_dwDot >> 31) & 0x00000001,0,0,0);
                return (pThis->m_dwDot >> 31) & 0x00000001;
            }
            //
            // The final, and least likely case, is when we're at the end of the buffer.
            // Since there's really no point in having this inlined since it'll only
            // happen once, I'll call a function to make it happen and save the space
            // in the inline instances of getBits().
            //
            result=BS_finalLoad (pThis, 0, 1);
            HUFFMANGET_DBG_DMP(result,0,0,0);
            return result;
#       endif
    }

inline   I32_WMV BS_bytesLeft (CInputBitStream_WMV * pThis) {return pThis->m_cbBuflen;}

inline Void_WMV BS_flushMPEG4 (CInputBitStream_WMV * pThis, I32_WMV nExtraBits=0) {
#       ifdef OPT_HUFFMAN_GET_WMV
            if((((U8_WMV)pThis->m_BMS8.m_wBitsLeft) % 8u)==0) {
                BS_getBits(pThis, nExtraBits);
            }
            else {
                pThis->m_BMS8.m_wBitsLeft -= (((U8_WMV)pThis->m_BMS8.m_wBitsLeft) % 8u);
            }
#       else
            if((pThis->m_dwBitsLeft % 8u)==0) {
                BS_getBits(pThis, nExtraBits);
            }
            else {
                pThis->m_dwBitsLeft -= (pThis->m_dwBitsLeft % 8u);
            }
#       endif
    }
#   ifdef OPT_HUFFMAN_GET_WMV
        FORCEINLINE U32_WMV BS_peekBits (CInputBitStream_WMV * pThis, const U32_WMV numBits)
        {
            assert((numBits>0)&&(numBits<32));

            I32_WMV tmp_dwBitsLeft;
            if ((tmp_dwBitsLeft=(pThis->m_BMS8.m_wBitsLeft-numBits))>=0) 
            {
                // result= (m_currBits >> tmp_dwBitsLeft) & GetMask[numBits];
                return (pThis->m_currBits >> tmp_dwBitsLeft) & (((U32_WMV)0xffffffff)>>(32-numBits));

            }

            return BS_peekBits2 (pThis, numBits);
        }

        U32_WMV BS_peekBits2 (CInputBitStream_WMV * pThis, const U32_WMV numBits);

        // following a peekbits and numBits is guaranteed to be smaller than m_wBitsLeft
        FORCEINLINE Void_WMV BS_adjustBits(CInputBitStream_WMV * pThis, const U8_WMV numBits)
        {
#if DBG
g_TotalBits += dwNumBits;
#endif

            pThis->m_BMS8.m_wBitsLeft-=numBits;
        }
#   else
        U32_WMV BS_peekBits (CInputBitStream_WMV * pThis, const U32_WMV numBits);            //peek bits
#   endif
    U32_WMV BS_peekBitsTillByteAlign (CInputBitStream_WMV * pThis, I32_WMV & nBitsToPeek);
    U32_WMV BS_peekBitsFromByteAlign (CInputBitStream_WMV * pThis, I32_WMV numBits); // peek from byte bdry
 inline   I32_WMV BS_eof(CInputBitStream_WMV * pThis)  {
#       ifdef OPT_HUFFMAN_GET_WMV
            if (pThis->m_BMS8.m_fStreamStaus == END_OF_FILE) 
                return EOF;
#       else
            if (pThis->m_fStreamStaus == END_OF_FILE) 
                return EOF;
#       endif
        return (~EOF); 
    }
#endif // __STRMDEC_HPP_


