/*************************************************************************

Copyright (C) 1996, 1997  Microsoft Corporation

Module Name:

    strmdec.cpp

Abstract:

    Decoder BitStream

Author:

    Craig Dowell (craigdo@microsoft.com) 10-December-1996
    Ming-Chieh Lee (mingcl@microsoft.com) 10-December-1996
    Bruce Lin (blin@microsoft.com) 10-December-1996

Revision History:

*************************************************************************/
#include "bldsetup.h"

#include <stdio.h>
#include <stdlib.h>
#include "xplatform.h"
#include "typedef.hpp"
#include "strmdec_wmv.hpp"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"

#ifdef OPT_HUFFMAN_DBGDMP
U32_WMV opt_huffman_dbgdmp[0x20000];
int opt_huffman_dbgdmp_idx;
#endif

#if DBG
DWORD g_TotalBits;
#endif

Void_WMV BS_construct (CInputBitStream_WMV * pThis, U32_WMV uiUserData)
{
    pThis->m_pBuffer = NULL_WMV;
    pThis->m_cbBuflen = 0;
#ifdef OPT_HUFFMAN_GET_WMV
    pThis->m_currBits=0;
    pThis->m_nextBits=0;
    pThis->m_BMS8.m_wBitsLeft = 0;
    pThis->m_BMS8.m_wNextBitsSkip=32;
    pThis->m_BMS8.m_fStreamStaus = VALID;
#else
    pThis->m_dwDot = 0;
    pThis->m_dwBitsLeft = 0;
    pThis->m_fStreamStaus = VALID;
#endif
    
    pThis->m_uiUserData = uiUserData;
}

Void_WMV BS_destruct (CInputBitStream_WMV * pThis)
{
}

#ifndef OPT_HUFFMAN_GET_WMV
U32_WMV GetMask_WMV[33] = {
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
#ifdef OPT_HUFFMAN_GET_WMV

#ifndef _EMB_ASM_ARM_STRMHUFF_


I32_WMV BS_getBits2(CInputBitStream_WMV * pThis, I32_WMV dwNumBits)
{

#if DBG
g_TotalBits += dwNumBits;
#endif

    BS_updateCurrNext(pThis);

    I32_WMV wBitsLeft;
    
    if ((wBitsLeft=(pThis->m_BMS8.m_wBitsLeft-(I16_WMV)dwNumBits))>=0) 
    {
                // result=(m_currBits >> m_dwBitsLeft) & GetMask[dwNumBits];
                //HUFFMANGET_DBG_DMP((m_currBits >> m_BMS8.m_wBitsLeft) & (((U32)0xffffffff)>>(32-dwNumBits)),0,0,0);
        pThis->m_BMS8.m_wBitsLeft=wBitsLeft;
        I32_WMV result=(pThis->m_currBits >> wBitsLeft) & (((U32_WMV)0xffffffff)>>(32-dwNumBits));
     //   BS_updateCurrNext(pThis);
        return result;
    }
    else
    {
        pThis->m_BMS8.m_wBitsLeft=0;
        pThis->m_BMS8.m_fStreamStaus = END_OF_FILE;
        pThis->m_cbBuflen=0;
        pThis->m_currBits=pThis->m_nextBits=0;
        pThis->m_BMS8.m_wNextBitsSkip=32;


    }

	return 0;

                    


}

#endif
Void_WMV BS_updateCurrNextEof(CInputBitStream_WMV * pThis, I32_WMV nextbits_shift, I32_WMV currBits);

#ifndef _EMB_ASM_ARM_STRMHUFF_
Void_WMV BS_updateCurrNext(CInputBitStream_WMV * pThis)
{


    register I32_WMV currBits, wBitsLeft, wNextBitsSkip, minus_nextbits_shift, minus1=-1;

    wBitsLeft=pThis->m_BNS16.m_BNS_base16L&0xff;
    wNextBitsSkip=pThis->m_BNS16.m_BNS_base16L>>8;
  
    currBits=(pThis->m_currBits<<(31-wBitsLeft))<<1;
    I32_WMV nextbits_shift=wBitsLeft-wNextBitsSkip;
    if(nextbits_shift>0) // nextBits contains enough bits for this shift.
    {
        pThis->m_currBits=currBits|((pThis->m_nextBits>>nextbits_shift)&(minus1>>wBitsLeft));
        pThis->m_BNS16.m_BNS_base16L=0x2020-(nextbits_shift<<8);
        return;
    }

    minus_nextbits_shift=-nextbits_shift;
    currBits|=(pThis->m_nextBits&(((U32_WMV)minus1)>>wNextBitsSkip))<<(minus_nextbits_shift);
    if(pThis->m_cbBuflen>=4)
    {           

        pThis->m_nextBits = ReadIntNBO(pThis->m_pBuffer);
        pThis->m_pBuffer += 4;
        pThis->m_cbBuflen-=4;
	
	    pThis->m_currBits=currBits|((pThis->m_nextBits>>(31+nextbits_shift))>>1); 

        pThis->m_BNS16.m_BNS_base16L=((minus_nextbits_shift)<<8)+32;
	
        return;
    }
 

    BS_updateCurrNextEof(pThis, nextbits_shift,currBits);


  return;
}
#endif
#ifdef __STREAMING_MODE_DECODE_
I32_WMV BS_streaming_update(CInputBitStream_WMV * pThis);
#endif

Void_WMV BS_updateCurrNextEof(CInputBitStream_WMV * pThis, I32_WMV nextbits_shift, I32_WMV  currBits)
{

            int i, num_bits;

            pThis->m_nextBits=0;
            nextbits_shift=-nextbits_shift;
            pThis->m_currBits=currBits;

            if((pThis->m_BMS8.m_wBitsLeft<0)||(pThis->m_BMS8.m_wBitsLeft>=128))
            {
                assert(0);
                pThis->m_BMS8.m_wBitsLeft=0;
                pThis->m_BMS8.m_fStreamStaus = END_OF_FILE;
                pThis->m_cbBuflen=0;
                pThis->m_currBits=pThis->m_nextBits=0;
                pThis->m_BMS8.m_wNextBitsSkip=32;
                return;


            }

            for(i=0;i<pThis->m_cbBuflen;i++)
            {
                pThis->m_nextBits<<=8;
                pThis->m_nextBits|=*pThis->m_pBuffer++;
            }
                
            num_bits=(pThis->m_cbBuflen<<3);
            if(num_bits>=nextbits_shift)
            {
                pThis->m_currBits|=pThis->m_nextBits>>(num_bits-nextbits_shift);
                pThis->m_BMS8.m_wNextBitsSkip=32-(num_bits-nextbits_shift);
                pThis->m_BMS8.m_wBitsLeft=32;
                pThis->m_cbBuflen=0;
			
#ifdef __STREAMING_MODE_DECODE_
				BS_streaming_update(pThis);
#endif
            }
            else
            {
                pThis->m_currBits|=pThis->m_nextBits<<(nextbits_shift-num_bits);
                pThis->m_currBits=pThis->m_currBits>>(nextbits_shift-num_bits);
				int tmp=32-(nextbits_shift-num_bits);
				
                pThis->m_BMS8.m_wBitsLeft=tmp;
                pThis->m_cbBuflen=0;
                pThis->m_BMS8.m_wNextBitsSkip=32;
				
#ifdef __STREAMING_MODE_DECODE_
				if(TRUE_WMV==BS_streaming_update(pThis))
				{
					BS_updateCurrNext(pThis);
				}
#endif
            }

}

I32_WMV BS_getMaxBits2(CInputBitStream_WMV * pThis, I32_WMV dwNumBits, U8_WMV* rgCodeSymSize)
{



    U32_WMV iBitPattern;
    I32_WMV bitsshift, result;
    
    BS_updateCurrNext(pThis);

    bitsshift=pThis->m_BMS8.m_wBitsLeft-dwNumBits;
    if(bitsshift>=0)
    {

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

        HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
        HUFFMANGET_DBG_DMP(result,rgCodeSymSize,iBitPattern+1,1);
        return result;
    }

    assert(pThis->m_BMS8.m_wNextBitsSkip==32);
    assert(pThis->m_cbBuflen==0);

    
    iBitPattern = ((pThis->m_currBits << (-bitsshift)) & (((U32_WMV)0xffffffff)>>(32-dwNumBits)))<<1; //0x00000fff;
    pThis->m_BMS8.m_wBitsLeft -= rgCodeSymSize[iBitPattern];
    result=rgCodeSymSize [iBitPattern+1];
    if (result == 255)
    {
        pThis->m_BMS8.m_wBitsLeft=0;
        pThis->m_BMS8.m_wNextBitsSkip=32;
        pThis->m_cbBuflen=0;
        pThis->m_BMS8.m_fStreamStaus = ILLEGAL_PATTERN;
    }
    else if((pThis->m_BMS8.m_wBitsLeft<0)||(pThis->m_BMS8.m_wBitsLeft>=128))
    {
        pThis->m_BMS8.m_wBitsLeft=0;
        pThis->m_BMS8.m_fStreamStaus = END_OF_FILE;
        result=255;
    }

    HUFFMANGET_DBG_CACHE(rgCodeSymSize,iBitPattern,2);
    HUFFMANGET_DBG_DMP(result,rgCodeSymSize,iBitPattern+1,1);
    return result;

}


 
#endif

#ifndef OPT_HUFFMAN_GET_WMV
U32_WMV BS_finalLoad( CInputBitStream_WMV * pThis, U32_WMV    dwRetval, U32_WMV       dwBitsToAdd)
{
    if ((pThis->m_cbBuflen << 3) < (I32_WMV)dwBitsToAdd) {
        pThis->m_fStreamStaus = END_OF_FILE;
        return 0;
    }

    U32_WMV              dwPosition = 4;
    pThis->m_dwDot = 0;
    pThis->m_dwBitsLeft = 0;

    for (; pThis->m_cbBuflen;) {
        pThis->m_dwDot <<= 8;
        pThis->m_dwDot |= *pThis->m_pBuffer++;
        --pThis->m_cbBuflen;
        pThis->m_dwBitsLeft += 8;
    }

    dwRetval <<= dwBitsToAdd;
    dwRetval |= (pThis->m_dwDot >> (pThis->m_dwBitsLeft - dwBitsToAdd)) & GetMask_WMV[dwBitsToAdd];
    pThis->m_dwBitsLeft -= dwBitsToAdd;
    return dwRetval;

}

#endif
#ifdef __STREAMING_MODE_DECODE_

#ifdef OPT_HUFFMAN_GET_WMV
I32_WMV BS_streaming_update(CInputBitStream_WMV * pThis)
{
	if (pThis->m_bNotEndOfFrame) 
	{
        Bool_WMV bNotEndOfFrame;
        U32_WMV  dwBufferLen;
        U8_WMV*  pBuffer;
        WMVDecCBGetData (pThis->m_uiUserData, 0, (U8_WMV**) &pBuffer, 4, &dwBufferLen, &bNotEndOfFrame);
                    //getVideoData(m_cbBuflen, &pBuffer, &dwBufferLen, &bNotEndOfFrame);
		if (bNotEndOfFrame){
			if (dwBufferLen == 0 || pBuffer == NULL){
				BS_setInvalid(pThis);
				return FALSE_WMV;
			}
		}

		pThis->m_pBuffer = pBuffer;
		pThis->m_cbBuflen = dwBufferLen;
		pThis->m_BMS8.m_fStreamStaus = VALID;
		pThis->m_bNotEndOfFrame = bNotEndOfFrame;

		if(pThis->m_BMS8.m_wNextBitsSkip==32)
		{
			if(pThis->m_cbBuflen>=4)
			{
				pThis->m_nextBits=ReadIntNBO(pThis->m_pBuffer);
				pThis->m_pBuffer+=4;
				pThis->m_cbBuflen-=4;
				pThis->m_BMS8.m_wNextBitsSkip=0;
			}
			else
			{
				int i;

				pThis->m_nextBits=0;
				for(i=0;i<pThis->m_cbBuflen;i++)
				{
					pThis->m_nextBits<<=8;
					pThis->m_nextBits|=*pThis->m_pBuffer++;
				}
				pThis->m_BMS8.m_wNextBitsSkip=32-(pThis->m_cbBuflen<<3);
                        
				pThis->m_cbBuflen=0;   
			}
        }

		return TRUE_WMV;
	}

	return FALSE_WMV;

}
#else
Void_WMV
BS_addDataToBuffer(
    CInputBitStream_WMV * pThis,
    U8_WMV          *pBuffer,
    U32_WMV        dwBuflen,
    Bool_WMV        bNotEndOfFrame
    )
{
    pThis->m_pBuffer = pBuffer;
    pThis->m_cbBuflen = dwBuflen;
    pThis->m_fStreamStaus = VALID;
    pThis->m_bNotEndOfFrame = bNotEndOfFrame;
}
#endif


#endif


Void_WMV
BS_reset(
    CInputBitStream_WMV * pThis,
    U8_WMV          *pBuffer,
    U32_WMV        dwBuflen,
    Bool_WMV        bNotEndOfFrame
    )
{
    pThis->m_pBuffer = pBuffer;
    pThis->m_cbBuflen = dwBuflen;


#ifdef OPT_HUFFMAN_GET_WMV

    pThis->m_BMS8.m_fStreamStaus = VALID;
	pThis->m_BMS8.m_wBitsLeft=0;
	pThis->m_BMS8.m_wNextBitsSkip=32;


    if(pThis->m_cbBuflen>=4)
    {
        pThis->m_currBits=ReadIntNBO(pBuffer);
        pThis->m_pBuffer+=4;
        pThis->m_cbBuflen-=4;

        pThis->m_BMS8.m_wBitsLeft=32;
        if(pThis->m_cbBuflen>=4)
        {
            pThis->m_nextBits=ReadIntNBO(pThis->m_pBuffer);
            pThis->m_pBuffer+=4;
            pThis->m_cbBuflen-=4;
            pThis->m_BMS8.m_wNextBitsSkip=0;
        }
        else
        {
            int i;

            pThis->m_nextBits=0;
            for(i=0;i<pThis->m_cbBuflen;i++)
            {
                pThis->m_nextBits<<=8;
                pThis->m_nextBits|=*pThis->m_pBuffer++;
            }
            pThis->m_BMS8.m_wNextBitsSkip=32-(pThis->m_cbBuflen<<3);
                        
            pThis->m_cbBuflen=0;   
        }
    }
    else
    {
        int i;
        pThis->m_currBits=0;
        for(i=0;i<pThis->m_cbBuflen;i++)
        {
            pThis->m_currBits<<=8;
            pThis->m_currBits|=*pThis->m_pBuffer++;
        }
        pThis->m_BMS8.m_wBitsLeft=(pThis->m_cbBuflen<<3);

        pThis->m_BMS8.m_wNextBitsSkip=32;
                        
        pThis->m_cbBuflen=0;   


    }


#else
    pThis->m_fStreamStaus = VALID;
    pThis->m_dwDot = 0;
    pThis->m_dwBitsLeft = 0;
#endif
    
#ifdef __STREAMING_MODE_DECODE_
    pThis->m_bNotEndOfFrame = bNotEndOfFrame;
#endif
}

U32_WMV BS_peekBitsTillByteAlign (CInputBitStream_WMV * pThis, I32_WMV& nBitsToPeek)
{
#ifdef OPT_HUFFMAN_GET_WMV
    register U32_WMV uiBitsLeftMode8 = ((U8_WMV)pThis->m_BMS8.m_wBitsLeft) % 8u;
#else
    register U32_WMV uiBitsLeftMode8 = pThis->m_dwBitsLeft % 8u;
#endif
    nBitsToPeek = (uiBitsLeftMode8 == 0) ? 8 : uiBitsLeftMode8;
    return BS_peekBits(pThis, nBitsToPeek);
}

#ifdef OPT_HUFFMAN_GET_WMV
U32_WMV BS_peekBits2 (CInputBitStream_WMV * pThis, const U32_WMV numBits)
{
#ifndef _CASIO_VIDEO_
    assert (numBits <= 32);
#endif

    I32_WMV tmp_dwBitsLeft;

    BS_updateCurrNext(pThis);

    if ((tmp_dwBitsLeft=(pThis->m_BMS8.m_wBitsLeft-numBits))>=0) 
    {
            // result= (m_currBits >> tmp_dwBitsLeft) & GetMask[numBits];
            return (pThis->m_currBits >> tmp_dwBitsLeft) & (((U32_WMV)0xffffffff)>>(32-numBits));
    }
        
    return  (pThis->m_currBits << (-tmp_dwBitsLeft)) & (((U32_WMV)0xffffffff)>>(32-numBits));

}
#else
U32_WMV BS_finalLoadPeek(
    CInputBitStream_WMV * pThis,
    U32_WMV              dwRetval,
    U32_WMV              dwBitsToAdd);

U32_WMV BS_peekBits (CInputBitStream_WMV * pThis, const U32_WMV numBits)
{
#ifndef _CASIO_VIDEO_
    assert (numBits <= 32);
#endif
    int result;

    U32_WMV iBitsToRet;

    if (pThis->m_fStreamStaus == VALID && ((U32_WMV) (pThis->m_dwBitsLeft + (pThis->m_cbBuflen << 3)) >= numBits)) {
        if (numBits <= pThis->m_dwBitsLeft)
            iBitsToRet = (pThis->m_dwDot >> (pThis->m_dwBitsLeft-numBits)) & GetMask_WMV[numBits];
        else {
            U32_WMV nBitsToPeek = numBits - pThis->m_dwBitsLeft;
            iBitsToRet = (pThis->m_dwDot) & GetMask_WMV[pThis->m_dwBitsLeft];
         
            if (pThis->m_cbBuflen >= 4) { // assume always true  for MPEG4-compliance
                register U32_WMV     dwAcc;

                dwAcc = (U32_WMV)pThis->m_pBuffer[3];
                dwAcc |= (U32_WMV)(pThis->m_pBuffer[2]) << 8;
                dwAcc |= (U32_WMV)(pThis->m_pBuffer[1]) << 16;
                dwAcc |= (U32_WMV)(pThis->m_pBuffer[0]) << 24;
                U32_WMV dwDotTmp = dwAcc;
                iBitsToRet = (iBitsToRet << nBitsToPeek) | ((dwDotTmp >> (32 - nBitsToPeek)) & GetMask_WMV[nBitsToPeek]);
                HUFFMANGET_DBG_DMP(iBitsToRet,0,0,0);
                return iBitsToRet;
            }
            result= BS_finalLoadPeek (pThis, iBitsToRet, nBitsToPeek);
            HUFFMANGET_DBG_DMP(result,0,0,0);
            return result;
        }
    }
    else {
        HUFFMANGET_DBG_DMP(0,0,0,0);
        return 0;
    }
    HUFFMANGET_DBG_DMP(iBitsToRet,0,0,0);
    return iBitsToRet;

}
#endif

U32_WMV  BS_peekBitsFromByteAlign(CInputBitStream_WMV * pThis, I32_WMV nBitsToPeek)
{
#ifdef OPT_HUFFMAN_GET_WMV
    Bool_WMV bAligned = ((((U8_WMV)pThis->m_BMS8.m_wBitsLeft) % 8u)==0);
    I32_WMV iBitsToPeek;
    if(bAligned)
        iBitsToPeek = nBitsToPeek + 8;
    else
        iBitsToPeek = nBitsToPeek + (((U8_WMV)pThis->m_BMS8.m_wBitsLeft) % 8u);
    U32_WMV nBitsRet = BS_peekBits(pThis, iBitsToPeek);
#else
    Bool_WMV bAligned = ((pThis->m_dwBitsLeft % 8)==0);
    I32_WMV iBitsToPeek;
    if(bAligned)
        iBitsToPeek = nBitsToPeek + 8;
    else
        iBitsToPeek = nBitsToPeek + (pThis->m_dwBitsLeft % 8u);
    U32_WMV nBitsRet = BS_peekBits(pThis, iBitsToPeek);
#endif
#ifdef OPT_HUFFMAN_GET_WMV
    return (nBitsRet & (((U32_WMV)0xffffffff)>>(32-nBitsToPeek)));
#else
    return (nBitsRet & GetMask_WMV[nBitsToPeek]);
#endif
}

#ifndef OPT_HUFFMAN_GET_WMV
U32_WMV BS_finalLoadPeek(
    CInputBitStream_WMV * pThis,
    U32_WMV              dwRetval,
    U32_WMV              dwBitsToAdd)
{
    U32_WMV              dwPosition = 4;

    U32_WMV dwDot = 0;
    U32_WMV dwBitsLeft = 0;
    I32_WMV cbBuflen = pThis->m_cbBuflen;
    U8_WMV *pBuffer = pThis->m_pBuffer;

    for (; cbBuflen;) {
        dwDot <<= 8;
        dwDot |= *pBuffer++;
        --cbBuflen;
        dwBitsLeft += 8;
    }

    dwRetval <<= dwBitsToAdd;
    dwRetval |= (dwDot >> (dwBitsLeft - dwBitsToAdd)) & GetMask_WMV[dwBitsToAdd];
    dwBitsLeft -= dwBitsToAdd;
    return dwRetval;
}
#endif
