//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wmaparse.c
//
//--------------------------------------------------------------------------

#include "bldsetup.h"

#if defined(macintosh) || defined(HITACHI)
typedef unsigned long U32_WMC;
#define __DWORD_DEFINED__
#else
#ifdef _XBOX
#include "xtl.h"
#endif
#ifndef _Embedded_x86
#include <windows.h>
#endif // embedded_x86
#endif

#include <stdlib.h>
#include <string.h>
#ifdef UNDER_CE
#define assert ASSERT
#else
#include <assert.h>
#endif
#include "wmc_type.h"
#include "wmcdecode.h"
#include "wmfparse.h"
#include "wmf_loadstuff.h"
#include "wmf.h"
#include "wmfdec.h"
#ifdef HITACHI
#include "wmmacsp.h"
#endif

#include "wmcguids.h"

Void_WMC SwapWstr(U16Char_WMC *src, I32_WMC n);
Void_WMC SWAPDWORD(U8_WMC * pSrc);
Void_WMC SWAPWORD(U8_WMC * pSrc);
U32_WMC HnsQWORDtoMsDWORD( U64_WMC qw );
U32_WMC GetASFVarField( U8_WMC bType, const U8_WMC *p, U32_WMC *pcbOffset );


Void_WMC SWAPWORD(U8_WMC * pSrc)
{   
    U8_WMC Temp;
    Temp  = *pSrc;
    *pSrc = *(pSrc+1);
    *(pSrc+1)= Temp;
}

Void_WMC SWAPDWORD(U8_WMC * pSrc)
{   
    U8_WMC Temp;
    Temp      = *pSrc;
    *pSrc     = *(pSrc+3);
    *(pSrc+3) = Temp;
    Temp      = *(pSrc+1);
    *(pSrc+1) = *(pSrc+2);
    *(pSrc+2) = Temp;
}

Void_WMC SwapWstr(U16Char_WMC *src, I32_WMC n)
{
    I32_WMC j;
    U8_WMC *p = (U8_WMC *)src;
    for (j = 0; j < n; j++) {
        SWAPWORD(p);
        p += 2;
    }
}

/****************************************************************************/
U32_WMC HnsQWORDtoMsDWORD( U64_WMC qw )
{
    U32_WMC msLo, msHi;

    msLo = (*((U32_WMC*)&qw)) / 10000;
#ifndef UNDER_CE
    msHi = (U32_WMC)( (Float_WMC) (*(((U32_WMC*)&qw)+1)) * 429496.7296 );
#else
    msHi = (U32_WMC)( (Float_WMC) (*(((U32_WMC*)&qw)+1)) * 429496.7296f);
#endif

    return( msLo + msHi );
}


/****************************************************************************/
tWMCDecStatus LoadHeaderObject(HWMCDECODER* phDecoder)
{
    GUID_WMC objectId;
    U64_WMC qwSize;
    U32_WMC cHeaders;
    U8_WMC align;
    U8_WMC arch;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;

    WMFDecoderEx *pDecoder = NULL;

    if( NULL == phDecoder)
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);

 
    cbWanted = MIN_OBJECT_SIZE + sizeof(U32_WMC) + 2*sizeof(U8_WMC);
    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
        return WMCDec_BufferTooSmall;
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbActual;  
   
    
    LoadGUID( objectId, pData );
    LoadQWORD( qwSize, pData );
    LoadDWORD( cHeaders, pData );
    LoadBYTE( align, pData );
    LoadBYTE( arch, pData );

    if( !IsEqualGUID_WMC( &CLSID_CAsfHeaderObjectV0, &objectId ) || ( 1 != align ) || ( 2 != arch ) )
    {
        return  WMCDec_BadAsfHeader;
    }

    pDecoder->tHeaderInfo.cbHeader = *((U32_WMC*) &qwSize);

//    *pcbInternalSpace = MIN_OBJECT_SIZE + sizeof( U32_WMC ) + ( 2 * sizeof( U8_WMC ) );
//    *pcbHeader = *((U32_WMC*) &qwSize);

    return WMCDec_Succeeded;


}


/****************************************************************************/
tWMCDecStatus LoadPropertiesObject( /*const U8_WMC *pbBuffer,*/
                    U32_WMC cbBuffer,
                    HWMCDECODER* phDecoder)
{
    GUID_WMC mmsId;
    U64_WMC qwTotalSize;
    U64_WMC qwCreateTime;
    U64_WMC qwPackets;
    U64_WMC qwPlayDuration;
    U64_WMC qwSendDuration;
    U64_WMC qwPreroll;
    U32_WMC dwFlags;
    U32_WMC dwMinPacketSize;
    U32_WMC dwMaxPacketSize;
    U32_WMC dwMaxBitrate;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL; 
    U32_WMC cbWanted;
    U32_WMC cbActual;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    cbBuffer -= MIN_OBJECT_SIZE;
    
    cbWanted = sizeof( GUID_WMC ) + ( 6 * sizeof( U64_WMC ) ) + ( 4 * sizeof( U32_WMC ) );

    if(cbWanted > cbBuffer)
        return WMCDec_BufferTooSmall;

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
        return WMCDec_BufferTooSmall;
    
    
    LoadGUID( mmsId, pData );
    LoadQWORD( qwTotalSize, pData );
    LoadQWORD( qwCreateTime, pData );
    LoadQWORD( qwPackets, pData );
    LoadQWORD( qwPlayDuration, pData );
    LoadQWORD( qwSendDuration, pData );
    LoadQWORD( qwPreroll, pData );
    LoadDWORD( dwFlags, pData );
    LoadDWORD( dwMinPacketSize, pData );
    LoadDWORD( dwMaxPacketSize, pData );
    LoadDWORD( dwMaxBitrate, pData );
    pDecoder->tHeaderInfo.cbPacketSize = dwMaxPacketSize;
    pDecoder->tHeaderInfo.cPackets  = *((U32_WMC*) &qwPackets);
    pDecoder->tHeaderInfo.msDuration  = HnsQWORDtoMsDWORD( qwPlayDuration );
    pDecoder->tHeaderInfo.msSendDuration  = HnsQWORDtoMsDWORD( qwSendDuration );
    pDecoder->tHeaderInfo.msPreroll  = *((U32_WMC*) &qwPreroll);
    pDecoder->tHeaderInfo.dwMaxBitrate  = dwMaxBitrate;

    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object  

    return WMCDec_Succeeded;

}


/****************************************************************************/
tWMCDecStatus LoadEncryptionObject(
                   // const U8_WMC *pbBuffer,
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    tWMCDecStatus we = WMCDec_Succeeded;
    U32_WMC cbType;
    U32_WMC cbKeyID;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;
    WMFDecoderEx *pDecoder = NULL;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;

    pDecoder->pszType = NULL;
    pDecoder->pszKeyID = NULL;
    pDecoder->cbSecData = 0;
    pDecoder->pbSecData = NULL;

    do
    {

        cbWanted = sizeof(U32_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(pDecoder->cbSecData, pData);

        if(pDecoder->cbSecData > 0)
        {
            cbWanted = pDecoder->cbSecData;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbObjectOffset += cbActual;

            
            pDecoder->pbSecData = (U8_WMC *)wmvalloc(pDecoder->cbSecData);
            if(pDecoder->pbSecData == NULL)
            {
                we = WMCDec_BadMemory;
                break;
            }
            memcpy(pDecoder->pbSecData, pData, (size_t)cbActual);
        }


        cbWanted = sizeof(U32_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset+ cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(cbType, pData);

        if(cbType > 0)
        {
            cbWanted = cbType;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbObjectOffset += cbActual;

            pDecoder->pszType = (U8_WMC *)wmvalloc(cbType);
            if(pDecoder->pszType == NULL)
            {
                we = WMCDec_BadMemory;
                break;
            }
            memcpy(pDecoder->pszType, pData, cbActual);

        }

        cbWanted = sizeof(U32_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset+ cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;
        
       
        LoadDWORD(cbKeyID, pData);


        if(cbKeyID > 0)
        {
            cbWanted = cbKeyID;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbObjectOffset += cbActual;

            pDecoder->pszKeyID = (U8_WMC *)wmvalloc(cbKeyID);
            if(pDecoder->pszKeyID == NULL)
            {
                we = WMCDec_BadMemory;
                break;
            }
            memcpy(pDecoder->pszKeyID, pData, cbActual);

        }

        /* ignore the license URL */

    }
    while (0);

    if(we != WMCDec_Succeeded)
    {
        /* clean up if necessary */

        if(pDecoder->pszType != NULL)
        {
            wmvfree(pDecoder->pszType);
        }
        if(pDecoder->pszKeyID != NULL)
        {
            wmvfree(pDecoder->pszKeyID);
        }
    }
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object  

    return we;

}

/****************************************************************************/
tWMCDecStatus LoadEncryptionObjectEx(
                  //  const U8_WMC *pbBuffer,
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    tWMCDecStatus we = WMCDec_Succeeded;
    U32_WMC cbEnhData;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;
    
    pDecoder->cbEnhData = 0;
    pDecoder->pbEnhData = NULL;

    do
    {
        cbWanted = sizeof(U32_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(cbEnhData, pData);

        if(cbEnhData > 0)
        {
            cbWanted = cbEnhData;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                we = WMCDec_BufferTooSmall;
                break;
            }
            cbObjectOffset += cbActual;

 
            pDecoder->pbEnhData = (U8_WMC *)wmvalloc(cbEnhData);
            if(pDecoder->pbEnhData == NULL)
            {
                we = WMCDec_BadMemory;
                break;
            }
            memcpy(pDecoder->pbEnhData, pData, cbActual);

        }
        pDecoder->cbEnhData = cbEnhData;

    }
    while (0);
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object  

    return we;
}

/****************************************************************************/
tWMCDecStatus LoadAudioObjectSize(
                  //  const U8_WMC *pbBuffer,
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    GUID_WMC streamType;
    GUID_WMC ecStrategy;
    U64_WMC qwOffset;
    U32_WMC cbTypeSpecific;
    U32_WMC cbErrConcealment;
    U16_WMC wStreamNum;
    U32_WMC dwJunk;
    U32_WMC nBlocksPerObject;
    U8_WMC *pbTypeSpecific;
    const AsfXAcmAudioErrorMaskingData *pScramblingData;
    BITMAPINFOHEADER * pbminfo = NULL;
    AsfXGenericImageTypeSpecific *pGen = NULL;
    U16_WMC  tw =0;
    U32_WMC tdw=0;
    U8_WMC tb=0;
    U8_WMC *tp = NULL;
    I32_WMC span = 0;
	U16_WMC uTemp=0;
    AUDIOSTREAMINFO * pAudStreamInfo = NULL;
    VIDEOSTREAMINFO * pVidStreamInfo = NULL;
    BINARYSTREAMINFO * pBinStreamInfo = NULL;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;
    const U8_WMC* pbECData;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;

    
/*    if( cbBuffer < MIN_OBJECT_SIZE + ( 2 * sizeof( GUID_WMC ) ) + sizeof( U64_WMC ) +
                   ( 3 * sizeof( U32_WMC ) ) + sizeof( U16_WMC ) )
    {
        return( WMCDec_BufferTooSmall );
    }
*/

    cbWanted = ( 2 * sizeof( GUID_WMC ) ) + sizeof( U64_WMC ) + ( 3 * sizeof( U32_WMC ) ) + sizeof( U16_WMC );
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;


    LoadGUID( streamType, pData );
    LoadGUID( ecStrategy, pData );
    LoadQWORD( qwOffset, pData );
    LoadDWORD( cbTypeSpecific, pData );
    LoadDWORD( cbErrConcealment, pData );
    LoadWORD( wStreamNum, pData );
    LoadDWORD( dwJunk, pData );

    wStreamNum &= 0x7F;

/*    if( cbBuffer < MIN_OBJECT_SIZE + ( 2 * sizeof( GUID_WMC ) ) + sizeof( U64_WMC ) +
                   ( 3 * sizeof( U32_WMC ) ) + sizeof( U16_WMC ) + cbTypeSpecific + cbErrConcealment )
    {
        return( WMCDec_BufferTooSmall );
    }
*/
// Amit Changed to accomodate Video stream

    if( IsEqualGUID_WMC( &CLSID_AsfXStreamTypeAcmAudio, &streamType ) )
    {
		
        pDecoder->tAudioStreamInfo[pDecoder->tHeaderInfo.wNoOfAudioStreams] = (AUDIOSTREAMINFO *) wmvalloc(sizeof(AUDIOSTREAMINFO));
        if (NULL == pDecoder->tAudioStreamInfo[pDecoder->tHeaderInfo.wNoOfAudioStreams])
            return WMCDec_BadMemory;
        memset(pDecoder->tAudioStreamInfo[pDecoder->tHeaderInfo.wNoOfAudioStreams], 0, sizeof(AUDIOSTREAMINFO));
        pAudStreamInfo = pDecoder->tAudioStreamInfo[pDecoder->tHeaderInfo.wNoOfAudioStreams];

        pDecoder->tHeaderInfo.wNoOfAudioStreams++;
        if (pDecoder->tHeaderInfo.bHasBitRateInfo !=TRUE_WMC)
            pDecoder->tHeaderInfo.wNoOfStreams++;

        pAudStreamInfo->wStreamId = wStreamNum;
        
        if (cbTypeSpecific >0)
        {
            cbWanted = cbTypeSpecific;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                return WMCDec_BufferTooSmall;
                
            }
            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
                
            }
            cbObjectOffset += cbActual;
        
		    tp = pData;
            pbTypeSpecific = pData;
            LoadWORD (tw ,tp);

		    switch(tw) {
            U16_WMC cbSizeWavHeader = 0;
            U8_WMC i=0;

#ifndef _ASFPARSE_ONLY_
            case WAVE_FORMAT_WMAUDIO3:
                if(cbTypeSpecific < 36)
                {
                    return WMCDec_Fail;
                }
            
                pAudStreamInfo->wFormatTag        = tw;
			    pAudStreamInfo->nVersion         = 3;
			    tp = pbTypeSpecific +  2; LoadWORD(tw,tp); 
			    pAudStreamInfo->nChannels        = tw;
			    tp = pbTypeSpecific +  4; LoadDWORD(tdw,tp); 
			    pAudStreamInfo->nSamplesPerSec   = tdw;
			    tp = pbTypeSpecific +  8; LoadDWORD(tdw,tp);
			    pAudStreamInfo->nAvgBytesPerSec  = tdw;
			    tp = pbTypeSpecific + 12; LoadWORD (tw ,tp);
                pAudStreamInfo->nBlockAlign      = tw;
                
                tp = pbTypeSpecific +  14; LoadWORD (tw ,tp);
                pAudStreamInfo->wValidBitsPerSample = tw;
                
                // Round up to the byte to get the container size
                pAudStreamInfo->nBitsPerSample      = 8 * ((tw + 7)/8);
                
                // Has V3 specific info
                // WORD at pbTypeSpecific + 18 is reserved
                tp = pbTypeSpecific + 20; LoadDWORD (tdw, tp);
                pAudStreamInfo->dwChannelMask = tdw;
            
                tp = pbTypeSpecific + 32; LoadWORD (tw ,tp);
                pAudStreamInfo->nEncodeOpt        = tw;
                pAudStreamInfo->bIsDecodable = TRUE_WMC;
                pAudStreamInfo->wmar = WMA_S_NO_MORE_FRAME;
                break;
		    case WAVE_FORMAT_WMAUDIO2:
			    if(cbTypeSpecific < 28 )
				    return WMCDec_Fail;
                pAudStreamInfo->wFormatTag        = tw;
			    pAudStreamInfo->nVersion         = 2;
			    tp = pbTypeSpecific +  2; LoadWORD(tw,tp); 
			    pAudStreamInfo->nChannels        = tw;
			    tp = pbTypeSpecific +  4; LoadDWORD(tdw,tp); 
			    pAudStreamInfo->nSamplesPerSec   = tdw;
			    tp = pbTypeSpecific +  8; LoadDWORD(tdw,tp);
			    pAudStreamInfo->nAvgBytesPerSec  = tdw;
			    tp = pbTypeSpecific + 12; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBlockAlign      = tw;
			    tp = pbTypeSpecific +  14; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBitsPerSample        = tw;
			    tp = pbTypeSpecific + 22; LoadWORD (tw ,tp);
			    tp = pbTypeSpecific + 18; LoadDWORD(tdw,tp);
			    pAudStreamInfo->nSamplesPerBlock = tdw;
			    pAudStreamInfo->nEncodeOpt       = tw;
    // Added for compatibility with V3
                pAudStreamInfo->wValidBitsPerSample = 16;
                 if (pAudStreamInfo->nChannels  == 2)
                    pAudStreamInfo->dwChannelMask = 3;
                 else if (pAudStreamInfo->nChannels  == 1)
                    pAudStreamInfo->dwChannelMask = 4;
                pAudStreamInfo->bIsDecodable = TRUE_WMC;
                pAudStreamInfo->wmar = WMA_S_NO_MORE_FRAME;

			    break;
		    case WAVE_FORMAT_MSAUDIO1:
			    if(cbTypeSpecific < 22 )
				    return WMCDec_Fail;
                pAudStreamInfo->wFormatTag        = tw;
			    pAudStreamInfo->nVersion         = 1;
			    tp = pbTypeSpecific +  4; LoadDWORD(tdw,tp); 
			    pAudStreamInfo->nSamplesPerSec  = tdw;
			    tp = pbTypeSpecific +  8; LoadDWORD(tdw,tp);
			    pAudStreamInfo->nAvgBytesPerSec  = tdw;
			    tp = pbTypeSpecific + 12; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBlockAlign      = tw;
			    tp = pbTypeSpecific +  2; LoadWORD (tw ,tp);
			    pAudStreamInfo->nChannels        = tw;
			    tp = pbTypeSpecific +  14; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBitsPerSample        = tw;
			    tp = pbTypeSpecific + 20; LoadWORD (tw,tp);
			    pAudStreamInfo->nEncodeOpt       = tw;
			    tp = pbTypeSpecific + 18; LoadWORD (tw,tp);
			    pAudStreamInfo->nSamplesPerBlock = tw;
    // Added for compatibility with V3
                pAudStreamInfo->wValidBitsPerSample = 16;
                 if (pAudStreamInfo->nChannels  == 2)
                    pAudStreamInfo->dwChannelMask = 3;
                 else if (pAudStreamInfo->nChannels  == 1)
                    pAudStreamInfo->dwChannelMask = 4;
                pAudStreamInfo->bIsDecodable = TRUE_WMC;
                pAudStreamInfo->wmar = WMA_S_NO_MORE_FRAME;

			    break;
#ifndef __NO_SPEECH__
			case WAVE_FORMAT_MSSPEECH:
			    if(cbTypeSpecific < 18 )
				    return WMCDec_Fail;
                pAudStreamInfo->wFormatTag        = tw;
			    pAudStreamInfo->nVersion         = 4;
			    tp = pbTypeSpecific +  4; LoadDWORD(tdw,tp); 
			    pAudStreamInfo->nSamplesPerSec   = tdw;
			    tp = pbTypeSpecific +  8; LoadDWORD(tdw,tp);
			    pAudStreamInfo->nAvgBytesPerSec  = tdw;
			    tp = pbTypeSpecific + 12; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBlockAlign      = tw;
			    tp = pbTypeSpecific +  2; LoadWORD (tw ,tp);
			    pAudStreamInfo->nChannels        = tw;
			    tp = pbTypeSpecific +  14; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBitsPerSample        = tw;
    // Added for compatibility with V3
                pAudStreamInfo->wValidBitsPerSample = 16;
                 if (pAudStreamInfo->nChannels  == 2)
                    pAudStreamInfo->dwChannelMask = 3;
                 else if (pAudStreamInfo->nChannels  == 1)
                    pAudStreamInfo->dwChannelMask = 4;
                pAudStreamInfo->bIsDecodable = TRUE_WMC;
                pAudStreamInfo->wmar = WMA_S_NO_MORE_FRAME;
                break;
#endif			    
#endif
		    default:
                pAudStreamInfo->wFormatTag         = tw;
			    pAudStreamInfo->nVersion         = 0;
			    tp = pbTypeSpecific +  4; LoadDWORD(tdw,tp); 
			    pAudStreamInfo->nSamplesPerSec   = tdw;
			    tp = pbTypeSpecific +  8; LoadDWORD(tdw,tp);
			    pAudStreamInfo->nAvgBytesPerSec  = tdw;
			    tp = pbTypeSpecific + 12; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBlockAlign      = tw;
			    tp = pbTypeSpecific +  2; LoadWORD (tw ,tp);
			    pAudStreamInfo->nChannels        = tw;
			    tp = pbTypeSpecific +  14; LoadWORD (tw ,tp);
			    pAudStreamInfo->nBitsPerSample        = tw;

                // Added for compatibility with V3
                pAudStreamInfo->wValidBitsPerSample = 16;
                 if (pAudStreamInfo->nChannels   == 2)
                    pAudStreamInfo->dwChannelMask = 3;
                 else if (pAudStreamInfo->nChannels   == 1)
                    pAudStreamInfo->dwChannelMask = 4;
                pAudStreamInfo->bIsDecodable = FALSE_WMC;
		    }
        }



/*        
        pbTypeSpecific = (U8_WMC *)(( cbTypeSpecific > 0 ) ? pbBuffer : NULL);
		pbECData = ( cbErrConcealment > 0 ) ? pbBuffer + cbTypeSpecific: NULL;
		
		if( IsEqualGUID_WMC( &CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy ) )
		{
			pAudStreamInfo->cbAudioSize = ((AsfXSignatureAudioErrorMaskingData *) pbECData)->maxObjectSize;
		}
		else if( IsEqualGUID_WMC( &CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy ) )
		{
			pScramblingData = (const AsfXAcmAudioErrorMaskingData *) pbECData;

#ifndef _Embedded_x86
			pAudStreamInfo->cbAudioSize = pScramblingData->virtualPacketLen * pScramblingData->span;
#else
			span = *pbECData;
			pbECData++;
                        LoadWORD(tw,pbECData); 
			pAudStreamInfo->cbAudioSize = tw * span;
#endif
			if (pScramblingData->span > 1)
				return (WMCDec_Fail );
		}
		else
		{
			return( WMCDec_Fail );
		}
		
		pAudStreamInfo->wStreamId  = wStreamNum;
		tp = pbTypeSpecific;
*/


    /* Error concealment - this can get as big as 400!!! */

        if(cbErrConcealment > 0)
        {
            if( IsEqualGUID_WMC( &CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy ) )
            {
                cbWanted = sizeof(AsfXSignatureAudioErrorMaskingData);
            }
            else if( IsEqualGUID_WMC( &CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy ) )
            {
                cbWanted = 8;
            }
            else
            {
                return WMCDec_Fail;
            }

            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                return WMCDec_BufferTooSmall;
            }
            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
                
            }
            cbObjectOffset += cbErrConcealment; // Skip over the rest - cbActual;

            if(IsEqualGUID_WMC(&CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy))
            {
                pAudStreamInfo->cbAudioSize = ((AsfXSignatureAudioErrorMaskingData *)pData)->maxObjectSize;
            
            }
            else if(IsEqualGUID_WMC(&CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy))
            {
                pScramblingData = (AsfXAcmAudioErrorMaskingData *)pData;
            
#ifndef _Embedded_x86
			    pAudStreamInfo->cbAudioSize = pScramblingData->virtualPacketLen * pScramblingData->span;
#else
			    pbECData = pData;
                span = *pbECData;
			    pbECData++;
                
                LoadWORD(tw,pbECData); 
			    pAudStreamInfo->cbAudioSize = tw * span;
#endif
			if (pScramblingData->span > 1)
				return (WMCDec_Fail );
            }
            else
            {
                return WMCDec_Fail;
            }
        }

		if (pAudStreamInfo->nVersion !=0)
            nBlocksPerObject = pAudStreamInfo->cbAudioSize / pAudStreamInfo->nBlockAlign;

		pAudStreamInfo->cbAudioSize = nBlocksPerObject * (pAudStreamInfo->nSamplesPerBlock) * (pAudStreamInfo->nChannels) * ((pAudStreamInfo->nBitsPerSample)/8); // Bad calculation fpr WMA3 format To be changed later after asling Navin

//        return( WMCDec_Succeeded );
    }
	else if( IsEqualGUID_WMC( &CLSID_AsfXStreamTypeIcmVideo, &streamType ) )
	{
        if (cbTypeSpecific >0)
        {
            U8_WMC cTempBuff[1024];
            U32_WMC dwDataRead =0;
            U32_WMC dwDataRemaining = cbTypeSpecific;

            if (cbTypeSpecific > MIN_WANTED)
            {
                do
                {
                    if(dwDataRemaining > MIN_WANTED)
                        cbWanted = MIN_WANTED;
                    else
                        cbWanted = dwDataRemaining;

                    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset + dwDataRead , cbWanted, &pData, pDecoder->u32UserData);
                    memcpy(cTempBuff + dwDataRead, pData, cbActual);
                    dwDataRemaining-= cbActual;   
                    dwDataRead+=cbActual;
                }while(dwDataRead < cbTypeSpecific);

                cbObjectOffset += dwDataRead;
                pData = cTempBuff;

            
            }
            else
            {
                cbWanted = cbTypeSpecific;
                if(cbObjectOffset + cbWanted > cbBuffer)
                {
                    return WMCDec_BufferTooSmall;
                
                }
                cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                
                }
                cbObjectOffset += cbActual;
            }
        

		    if (!IsEqualGUID_WMC( &CLSID_AsfXNullErrorMaskingStrategy, &ecStrategy))
		    {			
			    return WMCDec_Fail;
		    }

            pGen = (AsfXGenericImageTypeSpecific *)pData;

            if (NULL == pGen)
			    return WMCDec_Fail;
		    if (0x2 != pGen->flags)
			    return WMCDec_Fail;

            pDecoder->tVideoStreamInfo[pDecoder->tHeaderInfo.wNoOfVideoStreams] = (VIDEOSTREAMINFO *) wmvalloc(sizeof(VIDEOSTREAMINFO));
            if (NULL == pDecoder->tVideoStreamInfo[pDecoder->tHeaderInfo.wNoOfVideoStreams])
                return WMCDec_BadMemory;
            pVidStreamInfo = pDecoder->tVideoStreamInfo[pDecoder->tHeaderInfo.wNoOfVideoStreams];
            
            memset(pDecoder->tVideoStreamInfo[pDecoder->tHeaderInfo.wNoOfVideoStreams], 0, sizeof(VIDEOSTREAMINFO));

            pDecoder->tHeaderInfo.wNoOfVideoStreams++;    
            if (pDecoder->tHeaderInfo.bHasBitRateInfo !=TRUE_WMC)
                pDecoder->tHeaderInfo.wNoOfStreams++;

            tp = pData;
		
		    LoadDWORD(tdw,tp); 	pVidStreamInfo->windowWidth= tdw;
		    LoadDWORD(tdw,tp); 	pVidStreamInfo->windowHeight = tdw;
		    LoadBYTE(tb,tp); 	pVidStreamInfo->flags = tb ;
		    LoadWORD(tw, tp);	pVidStreamInfo->imageInfoLen = tw;
		    pVidStreamInfo->wStreamId = wStreamNum;

		    LoadDWORD(tdw,tp); 	pVidStreamInfo->biSize  = tdw;
		    LoadDWORD(tdw,tp);  pVidStreamInfo->biWidth = tdw; 
		    LoadDWORD(tdw,tp); 	pVidStreamInfo->biHeight = tdw; 
		    LoadWORD(tw,tp); 	pVidStreamInfo->biPlanes = tw; 
		    LoadWORD(tw,tp); 	pVidStreamInfo->biBitCount = tw; 
		    LoadDWORD(tdw,tp);  pVidStreamInfo->biCompression = tdw; 


		    pbminfo = (BITMAPINFOHEADER *)(&(pGen->imageInfo));

            LoadDWORD(tdw,tp); 	pVidStreamInfo->biSizeImage = tdw; 
		    LoadDWORD(tdw,tp);  pVidStreamInfo->biXPelsPerMeter = tdw; 
		    LoadDWORD(tdw,tp);  pVidStreamInfo->biYPelsPerMeter = tdw; 
		    LoadDWORD(tdw,tp); 	pVidStreamInfo->biClrUsed = tdw; 
		    LoadDWORD(tdw,tp);  pVidStreamInfo->biClrImportant = tdw; 
		    
		    if (pVidStreamInfo->imageInfoLen > sizeof(BITMAPINFOHEADER) )
		    {
			    memcpy(pVidStreamInfo->bAdditionalInfo, (pData+11+sizeof(BITMAPINFOHEADER)),pVidStreamInfo->imageInfoLen - sizeof(BITMAPINFOHEADER) );
		    }
       }       
        
	}

	else
    {
        // Binary Stream
        pDecoder->tBinaryStreamInfo[pDecoder->tHeaderInfo.wNoOfBinaryStreams] = (BINARYSTREAMINFO *) wmvalloc(sizeof(BINARYSTREAMINFO));
        if (NULL == pDecoder->tBinaryStreamInfo[pDecoder->tHeaderInfo.wNoOfBinaryStreams])
            return WMCDec_BadMemory;
        pBinStreamInfo = pDecoder->tBinaryStreamInfo[pDecoder->tHeaderInfo.wNoOfBinaryStreams];

        pDecoder->tHeaderInfo.wNoOfBinaryStreams++; 
        if (pDecoder->tHeaderInfo.bHasBitRateInfo !=TRUE_WMC)
            pDecoder->tHeaderInfo.wNoOfStreams++;
        
        pBinStreamInfo->wStreamId = wStreamNum;
//        pBinStreamInfo->bIsDecodable = FALSE_WMC;
//        pBinStreamInfo->bTobeDecoded = FALSE_WMC;
        pBinStreamInfo->bWantOutput = FALSE_WMC;
    }

    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object  

    return( WMCDec_Succeeded );
}


/****************************************************************************/
U32_WMC GetASFVarField( U8_WMC bType, const U8_WMC *p, U32_WMC *pcbOffset )
{
    U32_WMC dwVar = 0;

    switch( bType )
    {
    case 0x01:
        dwVar = (U32_WMC)(*p);
        *pcbOffset += 1;
        break;
    case 0x02:
    {
        U16_WMC w;
        GetUnalignedWord( p, w );
        dwVar = w;
        *pcbOffset += 2;
        break;
    }
    case 0x03:
        GetUnalignedDword( p, dwVar );
        *pcbOffset += 4;
        break;
    }

    return( dwVar );
}

/****************************************************************************/
tWMCDecStatus WMCDecParsePacketHeader(HWMCDECODER hDecoder)
{
    U8_WMC b;
    PACKET_PARSE_INFO_EX *pParseInfoEx;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbLocalOffset=0;
    WMFDecoderEx *pDecoder = NULL;

    if( NULL == hDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hDecoder);

    pParseInfoEx = &(pDecoder->tHeaderInfo.ppex);

//    cbWanted = 24;              /* at most */
    
    cbWanted = 1;
    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }

    /* ParsePacket begins */


    pParseInfoEx->fParityPacket = FALSE_WMC;
    pParseInfoEx->cbParseOffset = 0;

    b = pData[cbLocalOffset];
    
    pParseInfoEx->fEccPresent = (BOOL)( (b&0x80) == 0x80 );
    pParseInfoEx->bECLen = 0;

    if(pParseInfoEx->fEccPresent)
    {
        if(b&0x10)
        {
            pParseInfoEx->fParityPacket = TRUE;
            return WMCDec_Succeeded;
        }

        if(b&0x60)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->bECLen = (b&0x0f);
        if(pParseInfoEx->bECLen != 2)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->cbParseOffset = (U32_WMC)(1 + pParseInfoEx->bECLen);
//////////////////////////////////////////////////////////////////////////////
        cbWanted = 1;              /* at least */

        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        
        }

        b = pData[cbLocalOffset];

//////////////////////////////////////////////////////////////////////////////
    }

    pParseInfoEx->cbPacketLenTypeOffset = pParseInfoEx->cbParseOffset;

    pParseInfoEx->bPacketLenType = (b&0x60)>>5;
    if(pParseInfoEx->bPacketLenType != 0
       && pParseInfoEx->bPacketLenType != 2)
    {
        return WMCDec_Fail;
    }

    pParseInfoEx->bPadLenType = (b&0x18)>>3;
    if(pParseInfoEx->bPadLenType == 3)
    {
        return WMCDec_Fail;
    }

    pParseInfoEx->bSequenceLenType = (b&0x06)>>1;

    pParseInfoEx->fMultiPayloads = (BOOL)(b&0x01);

    pParseInfoEx->cbParseOffset++;

//////////////////////////////////////////////////////////////////////////////
        cbWanted = 1;              /* at least */
        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        
        }

        b = pData[cbLocalOffset];

//////////////////////////////////////////////////////////////////////////////

    pParseInfoEx->bOffsetBytes = 4;
    pParseInfoEx->bOffsetLenType = 3;

    if(b != 0x5d)
    {
        if((b&0xc0) != 0x40)
        {
            return WMCDec_Fail;
        }

        if((b&0x30) != 0x10)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->bOffsetLenType = (b&0x0c)>>2;
        if(pParseInfoEx->bOffsetLenType == 0)
        {
            return WMCDec_Fail;
        }
        else if(pParseInfoEx->bOffsetLenType < 3)
        {
            pParseInfoEx->bOffsetBytes = pParseInfoEx->bOffsetLenType;
        }

        if((b&0x03) != 0x01)
        {
            return WMCDec_Fail;
        }
    }

    pParseInfoEx->cbParseOffset++;

//////////////////////////////////////////////////////////////////////////////
    pParseInfoEx->cbPacketLenOffset = pParseInfoEx->cbParseOffset; 
    switch(pParseInfoEx->bPacketLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            pParseInfoEx->cbExplicitPacketLength =  (U32_WMC)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedWord(pData, w);
            pParseInfoEx->cbExplicitPacketLength =  (U32_WMC)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedDword(pData, dw);
            pParseInfoEx->cbExplicitPacketLength =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
    pParseInfoEx->cbSequenceOffset = pParseInfoEx->cbParseOffset;
    switch(pParseInfoEx->bSequenceLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            pParseInfoEx->dwSequenceNum =  (U32_WMC)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedWord(pData, w);
            pParseInfoEx->dwSequenceNum =  (U32_WMC)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedDword(pData, dw);
            pParseInfoEx->dwSequenceNum =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////    
    pParseInfoEx->cbPadLenOffset = pParseInfoEx->cbParseOffset;
    switch(pParseInfoEx->bPadLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            pParseInfoEx->cbPadding =  (U32_WMC)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedWord(pData, w);
            pParseInfoEx->cbPadding =  (U32_WMC)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedDword(pData, dw);
            pParseInfoEx->cbPadding =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

    
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Now read 6 bytes
    
    cbWanted =6;
    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;

    }

/////////////////////////////////////////////////////////////////////////////////
    GetUnalignedDword(&pData[cbLocalOffset], pParseInfoEx->dwSCR);

/////////////////////////////////////////////////////////////////////////////////
    
    pParseInfoEx->cbParseOffset += 4;

//////////////////////////////////////////////////////////////////////////////////
    GetUnalignedWord(&pData[cbLocalOffset+4], pParseInfoEx->wDuration);
    
////////////////////////////////////////////////////////////////////////////////////
    
    pParseInfoEx->cbParseOffset += 2;


    /* ParsePacketEx begins */
    if( pParseInfoEx->fEccPresent && pParseInfoEx->fParityPacket ) 
    {
        return WMCDec_Succeeded;
    }


    pParseInfoEx->cbPayLenTypeOffset = 0;
    pParseInfoEx->bPayLenType = 0;
    pParseInfoEx->bPayBytes = 0;
    pParseInfoEx->cPayloads = 1;

    if(pParseInfoEx->fMultiPayloads)
    {
//////////////////////////////////////////////////////
        cbWanted =1;
        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;

        }

        b = pData[cbLocalOffset];
//////////////////////////////////////////////////////
        pParseInfoEx->cbPayLenTypeOffset = pParseInfoEx->cbParseOffset;

        pParseInfoEx->bPayLenType = (b&0xc0)>>6;
        if(pParseInfoEx->bPayLenType != 2
           && pParseInfoEx->bPayLenType != 1)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->bPayBytes = pParseInfoEx->bPayLenType;

        pParseInfoEx->cPayloads = (U32_WMC)(b&0x3f);
        if(pParseInfoEx->cPayloads == 0)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->cbParseOffset++;
    }

    return WMCDec_Succeeded;
}


/****************************************************************************/
tWMCDecStatus WMCDecParsePayloadHeader(HWMCDECODER hDecoder)
{
    U32_WMC cbDummy;
    U32_WMC cbParseOffset;
    U32_WMC cbRepDataOffset;
    U32_WMC dwPayloadSize;
    PACKET_PARSE_INFO_EX *pParseInfoEx;
    PAYLOAD_MAP_ENTRY_EX *pPayload;
    U32_WMC cbLocalOffset=0;
    WMFDecoderEx *pDecoder = NULL;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;

    U16_WMC wTotalDataBytes=0;

    if( NULL == hDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hDecoder);

    pParseInfoEx = &(pDecoder->tHeaderInfo.ppex);
    pPayload = &(pDecoder->tHeaderInfo.payload);



//    cbWanted = 24;              /* at most */

    cbWanted = 2;              /* at least */
    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }

    cbParseOffset = 0;

    /* Loop in ParsePacketAndPayloads */

    pPayload->cbPacketOffset = (U16_WMC)pParseInfoEx->cbParseOffset;
    pPayload->bStreamId = (pData[cbParseOffset])&0x7f; // Amit to get correct Streamid 
    pPayload->bIsKeyFrame = (((pData[cbParseOffset]) &0x80) >> 7);
    pPayload->bObjectId = pData[cbParseOffset + 1];
    cbDummy = 0;
//////////////////////////////////////////////////////////////////////////////////////
/*    pPayload->cbObjectOffset = GetASFVarField(pParseInfoEx->bOffsetLenType,
                                              &pData[cbParseOffset + 2],
                                              &cbDummy);

  */  switch(pParseInfoEx->bOffsetLenType)
    {
    case 0x01:
        {
            cbWanted =1;

            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset+2, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
            pPayload->cbObjectOffset =  (U32_WMC)(*pData);
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset+2, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
             GetUnalignedWord(pData, w);
            pPayload->cbObjectOffset =  (U32_WMC)(w);
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset+2, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
            GetUnalignedDword(pData, dw);
            pPayload->cbObjectOffset =  dw;
            break;
        }
    default:
        cbWanted =0;
    }

//////////////////////////////////////////////////////////////////////////////////////

    cbRepDataOffset = cbParseOffset + 2 + pParseInfoEx->bOffsetBytes;

///////////////////////////////////////////////////////////////////////////////////////

    cbWanted =1;
    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }
    pPayload->cbRepData = pData[cbLocalOffset];

///////////////////////////////////////////////////////////////////////////////////////
    pPayload->msObjectPres = 0xffffffff;

    if(pPayload->cbRepData == 1)
    {
        pPayload->msObjectPres = pPayload->cbObjectOffset;
        pPayload->cbObjectOffset = 0;
        pPayload->cbObjectSize = 0;
        pPayload->bIsCompressedPayload =1;

        cbWanted =3;
        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset + 1, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
		pPayload->dwDeltaPresTime = pData[0/*cbRepDataOffset +1*/];

		if( pParseInfoEx->fMultiPayloads)
		{
            GetUnalignedWord( &pData[ cbLocalOffset + 1 ],wTotalDataBytes );
		}
		else
		{
			wTotalDataBytes = 0;
		}
    }
    else if(pPayload->cbRepData >= 8)
    {
        cbWanted =8;
        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset + 1, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
        GetUnalignedDword(&pData[cbLocalOffset],
                          pPayload->cbObjectSize);
        GetUnalignedDword(&pData[cbLocalOffset+ 4],
                          pPayload->msObjectPres);

        pPayload->bIsCompressedPayload =0;
    }

    pPayload->cbTotalSize = 1 + 1 + pParseInfoEx->bOffsetBytes + 1 + pPayload->cbRepData;

    if(pParseInfoEx->fMultiPayloads)
    {
        cbDummy = 0;

//////////////////////////////////////////////////////////////////////////////////////
      /*  dwPayloadSize = GetASFVarField(pParseInfoEx->bPayLenType,
                                       &pData[cbParseOffset + pPayload->cbTotalSize],
                                       &cbDummy);
*/
        switch(pParseInfoEx->bPayLenType)
        {
        case 0x01:
            {
                cbWanted =1;
                cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize, cbWanted, &pData, pDecoder->u32UserData);
                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                }

                dwPayloadSize =  (U32_WMC)(*pData);
                break;
            }
        case 0x02:
            {
                U16_WMC w;
                cbWanted =2;
                cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize, cbWanted, &pData, pDecoder->u32UserData);
                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                }
                GetUnalignedWord(pData, w);
                dwPayloadSize =  (U32_WMC)(w);
        
                break;

            }
        case 0x03:
            {
                U32_WMC dw;
                cbWanted =4;
                cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize, cbWanted, &pData, pDecoder->u32UserData);
                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                }
                GetUnalignedDword(pData, dw);
                dwPayloadSize =  dw;
                break;
            }
        default:
            cbWanted =0;
        }

//////////////////////////////////////////////////////////////////////////////////////

    
    }
    else if(pParseInfoEx->cbExplicitPacketLength > 0)
    {
        dwPayloadSize = pParseInfoEx->cbExplicitPacketLength
                      - pParseInfoEx->cbParseOffset
                      - pPayload->cbTotalSize
                      - pParseInfoEx->cbPadding;
    }
    else
    {
        dwPayloadSize = pDecoder->tHeaderInfo.cbPacketSize
                      - pParseInfoEx->cbParseOffset
                      - pPayload->cbTotalSize
                      - pParseInfoEx->cbPadding;
    }
    if (0 == wTotalDataBytes)
        wTotalDataBytes = (U16_WMC) dwPayloadSize;

    pPayload->cbPayloadSize = (U16_WMC)dwPayloadSize;

    pPayload->cbTotalSize += pParseInfoEx->bPayBytes
                          + (U16_WMC)pPayload->cbPayloadSize;

 	pPayload->wTotalDataBytes = wTotalDataBytes; // Amit

/*    if( 1 == pPayload->cbRepData )
    {
        pPayload->cbPayloadSize--;
    }
*/
    pParseInfoEx->cbParseOffset += pPayload->cbTotalSize;

    if(pParseInfoEx->cbParseOffset > pDecoder->tHeaderInfo.cbPacketSize
       || (pParseInfoEx->cbParseOffset == pDecoder->tHeaderInfo.cbPacketSize
           && pDecoder->tHeaderInfo.iPayload < pParseInfoEx->cPayloads - 1))
    {
        return WMCDec_BadData;
    }

    return WMCDec_Succeeded;
}

/****************************************************************************/
tWMCDecStatus LoadContentDescriptionObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)

{


    U16_WMC uiTitle_len = 0;
    U16_WMC uiAuthor_len = 0;
    U16_WMC uiCopyright_len = 0;
    U16_WMC uiDescription_len = 0; 
    U16_WMC uiRating_len = 0;
//    U8_WMC *offset = NULL;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;

    cbWanted = 5 * sizeof( U16_WMC );
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;

    LoadWORD( uiTitle_len, pData );
    LoadWORD( uiAuthor_len, pData );
    LoadWORD( uiCopyright_len, pData );
    LoadWORD( uiDescription_len, pData );
    LoadWORD( uiRating_len, pData );

    pDecoder->m_pContentDesc = (WMCContentDescription *)wmvalloc(sizeof(WMCContentDescription));
    if (pDecoder->m_pContentDesc == NULL)
        return (WMCDec_BadMemory);
    
    pDecoder->m_pContentDesc->uiTitle_len = uiTitle_len / sizeof(U16Char_WMC);
    pDecoder->m_pContentDesc->uiAuthor_len = uiAuthor_len / sizeof(U16Char_WMC);
    pDecoder->m_pContentDesc->uiCopyright_len = uiCopyright_len / sizeof(U16Char_WMC);
    pDecoder->m_pContentDesc->uiDescription_len = uiDescription_len / sizeof(U16Char_WMC);
    pDecoder->m_pContentDesc->uiRating_len = uiRating_len / sizeof(U16Char_WMC);



    pDecoder->m_pContentDesc->pchTitle = NULL;
    pDecoder->m_pContentDesc->pchAuthor = NULL;
    pDecoder->m_pContentDesc->pchCopyright = NULL;
    pDecoder->m_pContentDesc->pchDescription = NULL;
    pDecoder->m_pContentDesc->pchRating = NULL;

    
    
//    offset = (U8_WMC*) pbBuffer;
    if (uiTitle_len > 0)
    {
        cbWanted = uiTitle_len;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;
        
        pDecoder->m_pContentDesc->pchTitle = (U16Char_WMC *)wmvalloc(uiTitle_len);
        if (pDecoder->m_pContentDesc->pchTitle == NULL)
            return( WMCDec_BadMemory );
        else
            memcpy(pDecoder->m_pContentDesc->pchTitle, pData, cbActual );
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pContentDesc->pchTitle, uiTitle_len / sizeof(U16Char_WMC));
#endif
    }
    
    if (uiAuthor_len > 0)
    {
        cbWanted = uiAuthor_len;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        
        pDecoder->m_pContentDesc->pchAuthor = (U16Char_WMC *)wmvalloc (uiAuthor_len);
        if (pDecoder->m_pContentDesc->pchAuthor == NULL)
            return( WMCDec_BadMemory );
        else
            memcpy(pDecoder->m_pContentDesc->pchAuthor, pData, cbActual );
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pContentDesc->pchAuthor, uiAuthor_len / sizeof(U16Char_WMC));
#endif
    }
    if (uiCopyright_len > 0)
    {
        cbWanted = uiCopyright_len;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        pDecoder->m_pContentDesc->pchCopyright = (U16Char_WMC *)wmvalloc (uiCopyright_len);
        if (pDecoder->m_pContentDesc->pchCopyright == NULL)
            return( WMCDec_BadMemory );
        else
            memcpy(pDecoder->m_pContentDesc->pchCopyright, pData, cbActual );
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pContentDesc->pchCopyright, uiCopyright_len / sizeof(U16Char_WMC));
#endif
    }
    if (uiDescription_len > 0)
    {
        cbWanted = uiDescription_len;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        pDecoder->m_pContentDesc->pchDescription = (U16Char_WMC *)wmvalloc (uiDescription_len);
        if (pDecoder->m_pContentDesc->pchDescription == NULL)
            return( WMCDec_BadMemory );
        else
            memcpy(pDecoder->m_pContentDesc->pchDescription, pData, cbActual );
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pContentDesc->pchDescription, uiDescription_len / sizeof(U16Char_WMC));
#endif
    }
    if (uiRating_len > 0)
    {
        cbWanted = uiRating_len;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        pDecoder->m_pContentDesc->pchRating = (U16Char_WMC *)wmvalloc (uiRating_len);
        if (pDecoder->m_pContentDesc->pchRating == NULL)
            return( WMCDec_BadMemory );
        else
            memcpy(pDecoder->m_pContentDesc->pchRating, pData, cbActual );
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pContentDesc->pchRating, uiRating_len / sizeof(U16Char_WMC));
#endif
        
    }
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object  
    
    return( WMCDec_Succeeded );

}
/****************************************************************************/
tWMCDecStatus
LoadMarkerObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    U32_WMC cbObjectOffset = 0;

    GUID_WMC m_gMarkerStrategy;
    U16_WMC m_wAlignment;
    U16_WMC m_wNameLen;
    U16_WMC tw;
    U32_WMC i, j;
    WMFDecoderEx *pDecoder = NULL;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
	U64_WMC qTemp;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;

    cbWanted = 24;
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;

    LoadGUID (m_gMarkerStrategy , pData);
    LoadDWORD(pDecoder->m_dwMarkerNum   , pData);
    LoadWORD (m_wAlignment      , pData);
    LoadWORD (m_wNameLen        , pData);

    if (pDecoder->m_dwMarkerNum == 0)
    {
        pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
        return( WMCDec_Succeeded );
    }
    
    
	if (m_wNameLen > 0) // Bug in Specs
	{
		cbWanted = m_wNameLen;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;
	
	}

 //   pbBuffer += m_wNameLen;

    (pDecoder->m_pMarkers) = (WMCMarkerEntry *)wmvalloc(sizeof(WMCMarkerEntry)*(pDecoder->m_dwMarkerNum));
    if (NULL == (pDecoder->m_pMarkers))
        return (WMCDec_BadMemory);

    for (j = 0; j < pDecoder->m_dwMarkerNum; j++) 
    {

        cbWanted = 18;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;
        
		
        LoadQWORD(qTemp, pData);
//		(pDecoder->m_pMarkers)[j].m_qOffset.dwHi = qTemp.dwHi; 
//		(pDecoder->m_pMarkers)[j].m_qOffset.dwLo = qTemp.dwLo;
		(pDecoder->m_pMarkers)[j].m_qOffset = qTemp; 

        LoadQWORD(qTemp, pData);
//		(pDecoder->m_pMarkers)[j].m_qtime.dwHi = qTemp.dwHi; 
//		(pDecoder->m_pMarkers)[j].m_qtime.dwLo = qTemp.dwLo;
		(pDecoder->m_pMarkers)[j].m_qtime = qTemp; 


        LoadWORD ((pDecoder->m_pMarkers)[j].m_wEntryLen , pData);

        cbWanted = (pDecoder->m_pMarkers)[j].m_wEntryLen;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;
        
        LoadDWORD((pDecoder->m_pMarkers)[j].m_dwSendTime, pData);
        LoadDWORD((pDecoder->m_pMarkers)[j].m_dwFlags, pData);
        LoadDWORD((pDecoder->m_pMarkers)[j].m_dwDescLen, pData);

        (pDecoder->m_pMarkers)[j].m_pwDescName = (U16Char_WMC *)wmvalloc(sizeof(U16Char_WMC)*((pDecoder->m_pMarkers)[j].m_dwDescLen));
        if (NULL == (pDecoder->m_pMarkers)[j].m_pwDescName)
            return (WMCDec_BadMemory);

        for (i=0;i<(pDecoder->m_pMarkers)[j].m_dwDescLen;i++) {
            LoadWORD(tw, pData);
            (pDecoder->m_pMarkers)[j].m_pwDescName[i] = tw;
        }
/*		if ((pDecoder->m_pMarkers)[j].m_wEntryLen > (12+ 2*((pDecoder->m_pMarkers)[j].m_dwDescLen)))
		{
			pbBuffer += ((pDecoder->m_pMarkers)[j].m_wEntryLen - (12+ 2*((pDecoder->m_pMarkers)[j].m_dwDescLen)));	
		
		}
*/
    }
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    

    return WMCDec_Succeeded;
}

/****************************************************************************/
tWMCDecStatus LoadExtendedContentDescriptionObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{


    U16_WMC cDescriptors;

    I32_WMC i;
    WMFDecoderEx *pDecoder = NULL;
    U32_WMC cbTrack;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;
    cbTrack = cbBuffer;

    cbWanted = sizeof(U16_WMC);
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;
    cbTrack -=cbActual;


    LoadWORD( cDescriptors, pData );
//    cbBuffer -= sizeof(U16_WMC);


    pDecoder->m_pExtendedContentDesc = (WMCExtendedContentDesc *)wmvalloc(sizeof(WMCExtendedContentDesc));
    if (pDecoder->m_pExtendedContentDesc == NULL)
        return (WMCDec_BadMemory);

    pDecoder->m_pExtendedContentDesc->cDescriptors = cDescriptors;
    pDecoder->m_pExtendedContentDesc->pDescriptors = NULL;
    pDecoder->m_pExtendedContentDesc->pDescriptors = (ECD_DESCRIPTOR *)wmvalloc(cDescriptors * sizeof(ECD_DESCRIPTOR));
    if(pDecoder->m_pExtendedContentDesc->pDescriptors == NULL)
        return( WMCDec_BadMemory );

    
    if(cbBuffer < pDecoder->m_pExtendedContentDesc->cDescriptors * sizeof(U16_WMC))
        return( WMCDec_BufferTooSmall );

    for(i = 0; i < cDescriptors; i++) 
    {

        cbWanted = sizeof(U16_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        LoadWORD(pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName, pData);
        cbTrack -=cbActual;

        if(cbTrack < pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName + sizeof(U16_WMC) * 2)
            return( WMCDec_BufferTooSmall );
        pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName = NULL;
        pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName = (U16Char_WMC *)wmvalloc(pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName);
        if (pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName == NULL)
            return( WMCDec_BadMemory );
        else
        {
            cbWanted = pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName + sizeof(U16_WMC) * 2;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                return WMCDec_BufferTooSmall;
                
            }

            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
                
            }
            cbObjectOffset += cbActual;
            memcpy(pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName, pData, pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName);
        }
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName, pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName / sizeof(U16Char_WMC));
#endif
        pData += pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName;
        cbTrack -= pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbName;
        LoadWORD(pDecoder->m_pExtendedContentDesc->pDescriptors[i].data_type, pData);
        cbTrack -= sizeof(U16_WMC);
        LoadWORD(pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue, pData);
        cbTrack -= sizeof(U16_WMC);

        if(cbTrack < pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue)
            return( WMCDec_BufferTooSmall );
        pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary = NULL;
        pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary = (U8_WMC *)wmvalloc(pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue);
        if (pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary == NULL)
            return( WMCDec_BadMemory );
        else
        {
            cbWanted =  pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue;
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                return WMCDec_BufferTooSmall;
                
            }

            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
            cbObjectOffset += cbActual;
            memcpy(pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary, pData,pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue);
        }
#ifndef LITTLE_ENDIAN
        if((pDecoder->m_pExtendedContentDesc->pDescriptors[i].data_type == ECD_DWORD) ||
        (pDecoder->m_pExtendedContentDesc->pDescriptors[i].data_type == ECD_BOOL))
            SWAPDWORD((U8_WMC *)pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary);
        else if(pDecoder->m_pExtendedContentDesc->pDescriptors[i].data_type == ECD_STRING)
            SwapWstr((U16Char_WMC *)pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pwszString, pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue / sizeof(U16Char_WMC));
        else if(pDecoder->m_pExtendedContentDesc->pDescriptors[i].data_type == ECD_WORD)
            SWAPWORD((U8_WMC *)pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary);
#endif
        pData += pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue;
        cbTrack -= pDecoder->m_pExtendedContentDesc->pDescriptors[i].cbValue;
    }
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
    
    return( WMCDec_Succeeded );
}
/***************************************************************************************************************/
tWMCDecStatus LoadScriptCommandObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)

{


    U16_WMC num_commands, num_types;
    GUID_WMC command_id;
    I32_WMC i;
    U32_WMC cbTrack;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;
    cbTrack = cbBuffer;


    cbWanted = /*sizeof(GUID_WMC) + */sizeof(GUID_WMC) + 2 * sizeof(U16_WMC);
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
       return WMCDec_BufferTooSmall;
    }
	cbObjectOffset+=cbActual;


    LoadGUID( command_id, pData );
    LoadWORD( num_commands, pData );
   
	LoadWORD( num_types, pData );

    pDecoder->m_pScriptCommand = (WMCScriptCommand *)wmvalloc(sizeof(WMCScriptCommand));
    if (pDecoder->m_pScriptCommand == NULL)
        return (WMCDec_BadMemory);
	memset(pDecoder->m_pScriptCommand, 0, sizeof(WMCScriptCommand));


    pDecoder->m_pScriptCommand->type_names = NULL;
    pDecoder->m_pScriptCommand->type_name_len = NULL;
    pDecoder->m_pScriptCommand->commands = NULL;
    pDecoder->m_pScriptCommand->command_param_len = NULL;
    pDecoder->m_pScriptCommand->num_types = num_types;
    if(num_types) 
    {
        pDecoder->m_pScriptCommand->type_names = (U16Char_WMC **)wmvalloc(num_types * sizeof(U16Char_WMC *));
        pDecoder->m_pScriptCommand->type_name_len = (I32_WMC *)wmvalloc(num_types * sizeof(I32_WMC));
    }
	for (i = 0; i < num_types; i++) 
    {
        pDecoder->m_pScriptCommand->type_names[i] = NULL;
        pDecoder->m_pScriptCommand->type_name_len[i] = 0;
	} 
	
	for (i = 0; i < num_types; i++) 
    {
        U16_WMC wChars = 0;
        cbWanted = sizeof(U16_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
        cbObjectOffset += cbActual;
        LoadWORD(wChars, pData);
        cbTrack -=cbActual;

        if(cbTrack < (U32_WMC)wChars * sizeof(U16Char_WMC))
		{
			return( WMCDec_BufferTooSmall );
		}

        pDecoder->m_pScriptCommand->type_names[i] = NULL;
        pDecoder->m_pScriptCommand->type_names[i] = (U16Char_WMC *)wmvalloc(wChars * sizeof(U16Char_WMC));
		memset(pDecoder->m_pScriptCommand->type_names[i], 0, wChars * sizeof(U16Char_WMC));
        pDecoder->m_pScriptCommand->type_name_len[i] = (I32_WMC) wChars;
        if(pDecoder->m_pScriptCommand->type_names[i] == NULL)
		{
			return( WMCDec_BadMemory );
		}
        else
        {
            cbWanted = wChars * sizeof(U16Char_WMC);
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                return WMCDec_BufferTooSmall;
            }

            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
            cbObjectOffset += cbActual;
            
            memcpy(pDecoder->m_pScriptCommand->type_names[i], pData, wChars * sizeof(U16Char_WMC));
        }
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pScriptCommand->type_names[i], wChars);
#endif
        pData +=  (wChars * sizeof(U16Char_WMC));
        cbTrack -=  (wChars * sizeof(U16Char_WMC));
    }
    pDecoder->m_pScriptCommand->num_commands = num_commands;
    if(num_commands)  
	{
        pDecoder->m_pScriptCommand->commands = (WMCCommandEntry *)wmvalloc(num_commands * sizeof(WMCCommandEntry));
        pDecoder->m_pScriptCommand->command_param_len = (I32_WMC *)wmvalloc(num_commands * sizeof(I32_WMC));
    }
	for (i = 0; i < num_commands; i++) 
    {
        pDecoder->m_pScriptCommand->commands[i].param = NULL;
        pDecoder->m_pScriptCommand->command_param_len[i] = 0;
	} 
    
	for (i = 0; i < num_commands; i++) 
    {
        U16_WMC wChars = 0;
        cbWanted = sizeof(U32_WMC) + 2 * sizeof(U16_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        LoadDWORD(pDecoder->m_pScriptCommand->commands[i].time, pData);
        cbTrack -= sizeof(U32_WMC);
        LoadWORD(pDecoder->m_pScriptCommand->commands[i].type, pData);
        cbTrack -= sizeof(U16_WMC);
        LoadWORD(wChars, pData);
        cbTrack -= sizeof(U16_WMC);
        if(cbTrack < (U32_WMC)wChars * sizeof(U16Char_WMC))
            return( WMCDec_BufferTooSmall );

        pDecoder->m_pScriptCommand->commands[i].param = NULL;
        pDecoder->m_pScriptCommand->commands[i].param = (U16Char_WMC *)wmvalloc(wChars * sizeof(U16Char_WMC));
        pDecoder->m_pScriptCommand->command_param_len[i] = (I32_WMC)wChars;
        if( pDecoder->m_pScriptCommand->commands[i].param == NULL)
            return( WMCDec_BadMemory );
        else
        {
            cbWanted = wChars * sizeof(U16Char_WMC);
            if(cbObjectOffset + cbWanted > cbBuffer)
            {
                return WMCDec_BufferTooSmall;
                
            }

            cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
                
            }
            cbObjectOffset += cbActual;

            memcpy(pDecoder->m_pScriptCommand->commands[i].param, pData, (wChars * sizeof(U16Char_WMC)));
        }
#ifndef LITTLE_ENDIAN
        SwapWstr((U16Char_WMC *)pDecoder->m_pScriptCommand->commands[i].param, wChars);
#endif
        cbTrack -= (wChars * sizeof(U16Char_WMC));
        pData += (wChars * sizeof(U16Char_WMC));
    }
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
    return( WMCDec_Succeeded );
}
/***************************************************************************************************************/


tWMCDecStatus
LoadLicenseStoreObject(U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    tWMCDecStatus we = WMCDec_Succeeded;
    U32_WMC m_dwFlag;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;


  

    do
    {

        cbWanted = sizeof(U32_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(m_dwFlag, pData);

        cbWanted = sizeof(U32_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(pDecoder->m_dwLicenseLen, pData);

        cbWanted = pDecoder->m_dwLicenseLen;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            we = WMCDec_BufferTooSmall;
            break;
        }
        cbObjectOffset += cbActual;

        pDecoder->m_pLicData = (U8_WMC *)wmvalloc(pDecoder->m_dwLicenseLen);
        if(NULL == pDecoder->m_pLicData)
        {
            we = WMCDec_BadMemory;
            break;
        }

        memcpy(pDecoder->m_pLicData, pData, pDecoder->m_dwLicenseLen);

    } while(0);

    if(we != WMCDec_Succeeded)
    {
        /* clean up if necessary */

        if(pDecoder->m_pLicData != NULL)
        {
            wmvfree(pDecoder->m_pLicData);
        }
    }
   pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
   return we;
}

/****************************************************************************/
tWMCDecStatus LoadExtendedStreamPropertiesObject(
               //     const U8_WMC *pbBuffer,
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    U32_WMC dwTemp =0;
    U16_WMC wNoOfStreams =0;
    U16_WMC i =0;
    U32_WMC dwBitRate =0;
    U16_WMC wStreamNo =0;
    U32_WMC dwOldBitRate =0;
    WMFDecoderEx *pDecoder = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;

    cbWanted = sizeof(U16_WMC);
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }
    cbObjectOffset += cbActual;


    LoadWORD( wNoOfStreams, pData );

    if( cbBuffer < (sizeof( U16_WMC ) + wNoOfStreams *(sizeof( U16_WMC ) + sizeof( U32_WMC )) ))
    {
        return( WMCDec_BufferTooSmall );
    }
    pDecoder->tHeaderInfo.wNoOfStreams = wNoOfStreams;
	
	if (wNoOfStreams >127)
        return( WMCDec_BufferTooSmall );

	for (i =0; i < wNoOfStreams; i++)
	{

        cbWanted = sizeof( U16_WMC ) + sizeof( U32_WMC ) ;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
        cbObjectOffset += cbActual;
 
        LoadWORD( wStreamNo, pData );
 	    LoadDWORD( dwBitRate, pData );
        pDecoder->tHeaderInfo.tBitRateInfo[i].wStreamId = wStreamNo;
        pDecoder->tHeaderInfo.tBitRateInfo[i].dwBitRate = dwBitRate;
        
	}
    pDecoder->tHeaderInfo.bHasBitRateInfo = TRUE_WMC;
    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
    return WMCDec_Succeeded;
}


/****************************************************************************/
tWMCDecStatus LoadIndexObject(HWMCDECODER *phDecoder,
                    U64_WMC cbOffset,
                    U32_WMC cbBuffer,
                    U16_WMC wStreamId,
					WMCINDEXINFO *pIndexInfo
                    )
{
    U32_WMC dwTemp =0;
    U64_WMC time_delta = 0;
    U32_WMC i=0;
    U16_WMC wTemp=0;
    WMCINDEXENTRIES *tpTempEntry = NULL;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbObjectOffset = 0;

    WMFDecoderEx *pDecoder = NULL;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    



    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

//	cbBuffer -= MIN_OBJECT_SIZE;
//	cbBuffer -= sizeof(GUID_WMC);

    if( cbBuffer < sizeof( U64_WMC ) +2*sizeof( U32_WMC ))
        return( WMCDec_BufferTooSmall );

    cbObjectOffset = MIN_OBJECT_SIZE + sizeof(GUID_WMC);


    cbWanted = sizeof(U64_WMC) + 2*sizeof( U32_WMC ) ;

    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
    }

    cbActual = WMCDecCBGetData(*phDecoder, cbOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }
    cbObjectOffset += cbActual;
    
	
   LoadQWORD(time_delta,pData);
   pIndexInfo->time_deltaMs = (*((U32_WMC*)&time_delta))/10000;
   LoadDWORD( dwTemp,pData);
   pIndexInfo->max_packets = dwTemp;
   LoadDWORD( dwTemp,pData);
   pIndexInfo->num_entries = dwTemp;
   pIndexInfo->nStreamId = wStreamId;

   pIndexInfo->pIndexEntries = (WMCINDEXENTRIES *) wmvalloc(pIndexInfo->num_entries*sizeof(WMCINDEXENTRIES));

   if (NULL == pIndexInfo->pIndexEntries)
        return( WMCDec_BadMemory );
   tpTempEntry =  pIndexInfo->pIndexEntries;
   
   for (i=0; i< pIndexInfo->num_entries; i++)
   {
      
        cbWanted = sizeof(U16_WMC) + sizeof( U32_WMC ) ;

        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
        }

        cbActual = WMCDecCBGetData(*phDecoder, cbOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
        cbObjectOffset += cbActual;
       
        LoadDWORD(dwTemp,pData);
        tpTempEntry->dwPacket = dwTemp;

        LoadWORD(wTemp,pData);
        tpTempEntry->wSpan = wTemp;
        tpTempEntry++;        
   }

   return WMCDec_Succeeded;
}


/****************************************************************************/

tWMCDecStatus LoadObjectHeader(HWMCDECODER *phDecoder, GUID_WMC *pObjectId, U64_WMC *pqwSize)
{
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    WMFDecoderEx *pDecoder = NULL;
    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);

    cbWanted = MIN_OBJECT_SIZE;
    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }

    pDecoder->tHeaderInfo.cbCurrentPacketOffset += cbActual;

    LoadGUID((*pObjectId), pData);
    LoadQWORD((*pqwSize), pData);

    return WMCDec_Succeeded;
}

/****************************************************************************/

tWMCDecStatus LoadVirtualObjectHeader(HWMCDECODER *phDecoder, GUID_WMC *pObjectId, U64_WMC *pqwSize, U32_WMC Offset)
{
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    WMFDecoderEx *pDecoder = NULL;
    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);

    cbWanted = MIN_OBJECT_SIZE;
    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + Offset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }

    LoadGUID((*pObjectId), pData);
    LoadQWORD((*pqwSize), pData);

    return WMCDec_Succeeded;
}


/****************************************************************************/

tWMCDecStatus ParseAsfHeader(HWMCDECODER *phDecoder, U8_WMC isFull)
{
    tWMCDecStatus rc;
    GUID_WMC objId;
    U64_WMC qwSize;

    WMFDecoderEx *pDecoder = NULL;
    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);

    /* initialize the some state */

    pDecoder->tHeaderInfo.cbCurrentPacketOffset = 0;


    /* ASF Header Object */

    rc = LoadHeaderObject(phDecoder);
    if(rc != WMCDec_Succeeded)
    {
        return rc;
    }
    pDecoder->tHeaderInfo.cbFirstPacketOffset = pDecoder->tHeaderInfo.cbHeader += DATA_OBJECT_SIZE;

    /* Scan Header Objects */

    while(pDecoder->tHeaderInfo.cbCurrentPacketOffset < pDecoder->tHeaderInfo.cbFirstPacketOffset)
    {
        rc = LoadObjectHeader(phDecoder, &objId, &qwSize);
        if(rc != WMCDec_Succeeded)
        {
            return rc;
        }

        if( IsEqualGUID_WMC( &CLSID_CAsfPropertiesObjectV2, &objId ) )
        {
            rc = LoadPropertiesObject(
                     //           pbBuffer,
                                *((U32_WMC*) &qwSize),
                                phDecoder);
            if (WMCDec_Succeeded != rc)
				goto abort;
        }
        else if( IsEqualGUID_WMC( &CLSID_CAsfStreamPropertiesObjectV1, &objId ))
        {
            rc = LoadAudioObjectSize(
                          //      pbBuffer,
                                *((U32_WMC*) &qwSize),
                                phDecoder);

            if (WMCDec_Succeeded != rc)
				goto abort;
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfContentEncryptionObject, &objId))
        {
            rc = LoadEncryptionObject( //pbBuffer,
                                      *((U32_WMC*) &qwSize),
                                      phDecoder);
            if (rc != WMCDec_Succeeded)
				goto abort;
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfContentEncryptionObjectEx, &objId))
        {
            rc = LoadEncryptionObjectEx( // pbBuffer,
                                      *((U32_WMC*) &qwSize),
                                      phDecoder);
            if (rc != WMCDec_Succeeded)
				goto abort;
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfContentDescriptionObjectV0, &objId))
        {
            if(isFull)
            {
                rc = LoadContentDescriptionObject( //pbBuffer,
                                          *((U32_WMC*) &qwSize),
                                          phDecoder);
                if (rc != WMCDec_Succeeded)
				    goto abort;
            }
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfMarkerObjectV0, &objId))
        {
            rc = LoadMarkerObject( // pbBuffer,
                                 *((U32_WMC*) &qwSize),
                                      phDecoder);
            if(rc != WMCDec_Succeeded)
				goto abort;
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfExtendedContentDescObject, &objId))
        {
            if(isFull)
            {
                rc = LoadExtendedContentDescriptionObject(//pbBuffer,
                                     *((U32_WMC*) &qwSize),
                                          phDecoder);
                if(rc != WMCDec_Succeeded)
				    goto abort;
            }
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfScriptCommandObjectV0, &objId))
        {

            rc = LoadScriptCommandObject(//pbBuffer,
                                 *((U32_WMC*) &qwSize),
                                      phDecoder);
            if(rc != WMCDec_Succeeded)
				goto abort;
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfLicenseStoreObject, &objId))
        {
            rc = LoadLicenseStoreObject(//pbBuffer,
                                 *((U32_WMC*) &qwSize),
                                      phDecoder);
            if(rc != WMCDec_Succeeded)
				goto abort;
        }
        else if(IsEqualGUID_WMC(&CLSID_CAsfExtendedStreamPropertiesObject, &objId))
        {
            rc = LoadExtendedStreamPropertiesObject(//pbBuffer,
                                 *((U32_WMC*) &qwSize),
                                      phDecoder);
            if(rc != WMCDec_Succeeded)
				goto abort;
       

		}
        else if(IsEqualGUID_WMC(&CLSID_CAsfCodecObjectV0, &objId))
        {
            rc = LoadCodecListObject(//pbBuffer,
                                 *((U32_WMC*) &qwSize),
                                      phDecoder);
            if(rc != WMCDec_Succeeded)
				goto abort;
       

		}
        else if(IsEqualGUID_WMC(&CLSID_CAsfClockObjectV0, &objId))
        {
            rc = LoadClockObject(*((U32_WMC*) &qwSize),
                                      phDecoder);
            if(rc != WMCDec_Succeeded)
				goto abort;
       

		}
        else
        {
            /* skip over this object */
            pDecoder->tHeaderInfo.cbCurrentPacketOffset += (qwSize - MIN_OBJECT_SIZE);
        }
    }

abort:

return rc;
}

/****************************************************************************/
tWMCDecStatus
LoadCodecListObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    U32_WMC cbObjectOffset = 0;

    GUID_WMC m_gCodecList;
    U16_WMC tw;
    U32_WMC i, j;
    WMFDecoderEx *pDecoder = NULL;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;

    cbWanted = 20;
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;

    LoadGUID (m_gCodecList , pData);
    LoadDWORD(pDecoder->m_dwNumCodec   , pData);

	if (pDecoder->m_dwNumCodec > 0)
	{
		pDecoder->m_pCodecEntry = (WMCCodecEntry *)wmvalloc((pDecoder->m_dwNumCodec)* sizeof(WMCCodecEntry));
		if (NULL == (pDecoder->m_pCodecEntry))
			return (WMCDec_BadMemory);
	}
	else
	{
	    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
		return WMCDec_Succeeded;
	}

    
    for (j = 0; j < pDecoder->m_dwNumCodec; j++) 
    {

		cbWanted = 4;
		if(cbObjectOffset + cbWanted > cbBuffer)
		{
			return WMCDec_BufferTooSmall;
        
		}

		cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

		if(cbActual != cbWanted)
		{
			return WMCDec_BufferTooSmall;
        
		}
		cbObjectOffset += cbActual;
        
        LoadWORD(tw, pData);
		if (tw == 0x0002)
		{
			(pDecoder->m_pCodecEntry)[j].m_wCodecType = Audio_WMC;
		}
		else if(tw == 0x0001)
		{
			(pDecoder->m_pCodecEntry)[j].m_wCodecType = Video_WMC;
		}
		else
			(pDecoder->m_pCodecEntry)[j].m_wCodecType = Binary_WMC;

        LoadWORD((pDecoder->m_pCodecEntry)[j].m_wCodecNameLength , pData);

        (pDecoder->m_pCodecEntry)[j].m_pwCodecName = (U16Char_WMC *)wmvalloc(sizeof(U16Char_WMC)*((pDecoder->m_pCodecEntry)[j].m_wCodecNameLength));
        if (NULL == (pDecoder->m_pCodecEntry)[j].m_pwCodecName)
            return (WMCDec_BadMemory);
        
		
		
		
		cbWanted = (pDecoder->m_pCodecEntry)[j].m_wCodecNameLength * sizeof(U16Char_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        for (i=0;i<(pDecoder->m_pCodecEntry)[j].m_wCodecNameLength;i++) 
		{
            LoadWORD(tw, pData);
            (pDecoder->m_pCodecEntry)[j].m_pwCodecName[i] = tw;
        }

		cbWanted = 2;
		if(cbObjectOffset + cbWanted > cbBuffer)
		{
			return WMCDec_BufferTooSmall;
        
		}

		cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

		if(cbActual != cbWanted)
		{
			return WMCDec_BufferTooSmall;
        
		}
		cbObjectOffset += cbActual;
        
        LoadWORD((pDecoder->m_pCodecEntry)[j].m_wCodecDescLength, pData);


        (pDecoder->m_pCodecEntry)[j].m_pwCodecDescription  = (U16Char_WMC *)wmvalloc(sizeof(U16Char_WMC)*((pDecoder->m_pCodecEntry)[j].m_wCodecDescLength ));
        if (NULL == (pDecoder->m_pCodecEntry)[j].m_pwCodecDescription)
            return (WMCDec_BadMemory);
        
		
		cbWanted = (pDecoder->m_pCodecEntry)[j].m_wCodecDescLength * sizeof(U16Char_WMC);
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        for (i=0;i<(pDecoder->m_pCodecEntry)[j].m_wCodecDescLength;i++) 
		{
            LoadWORD(tw, pData);
            (pDecoder->m_pCodecEntry)[j].m_pwCodecDescription[i] = tw;
        }


		cbWanted = 2;
		if(cbObjectOffset + cbWanted > cbBuffer)
		{
			return WMCDec_BufferTooSmall;
        
		}

		cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

		if(cbActual != cbWanted)
		{
			return WMCDec_BufferTooSmall;
        
		}
		cbObjectOffset += cbActual;
        
        LoadWORD((pDecoder->m_pCodecEntry)[j].m_wCodecInfoLen, pData);


        (pDecoder->m_pCodecEntry)[j].m_pbCodecInfo = (U8_WMC *)wmvalloc(((pDecoder->m_pCodecEntry)[j].m_wCodecInfoLen));
        if (NULL == (pDecoder->m_pCodecEntry)[j].m_pbCodecInfo)
            return (WMCDec_BadMemory);
        
		
		cbWanted = (pDecoder->m_pCodecEntry)[j].m_wCodecInfoLen;
        if(cbObjectOffset + cbWanted > cbBuffer)
        {
            return WMCDec_BufferTooSmall;
            
        }

        cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
            
        }
        cbObjectOffset += cbActual;

        memcpy((pDecoder->m_pCodecEntry)[j].m_pbCodecInfo,pData,(pDecoder->m_pCodecEntry)[j].m_wCodecInfoLen);

    }

    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    

    return WMCDec_Succeeded;
}

/****************************************************************************/

/****************************************************************************/
tWMCDecStatus WMCDecParseVirtualPayloadHeader(HWMCDECODER hDecoder, U32_WMC cbCurrentPacketOffset, PACKET_PARSE_INFO_EX *pParseInfoEx, PAYLOAD_MAP_ENTRY_EX *pPayload)
{
    U32_WMC cbDummy;
    U32_WMC cbParseOffset;
    U32_WMC cbRepDataOffset;
    U32_WMC dwPayloadSize;
    U32_WMC cbLocalOffset=0;
    WMFDecoderEx *pDecoder = NULL;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;

    U16_WMC wTotalDataBytes=0;

    if( NULL == hDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hDecoder);



    cbWanted = 2;              /* at least */
    cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }

    cbParseOffset = 0;

    /* Loop in ParsePacketAndPayloads */

    pPayload->cbPacketOffset = (U16_WMC)pParseInfoEx->cbParseOffset;
    pPayload->bStreamId = (pData[cbParseOffset])&0x7f; // Amit to get correct Streamid 
    pPayload->bIsKeyFrame = (((pData[cbParseOffset]) &0x80) >> 7);
    pPayload->bObjectId = pData[cbParseOffset + 1];
    cbDummy = 0;
	
	switch(pParseInfoEx->bOffsetLenType)
    {
    case 0x01:
        {
            cbWanted =1;

            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset+2, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
            pPayload->cbObjectOffset =  (U32_WMC)(*pData);
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset+2, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
             GetUnalignedWord(pData, w);
            pPayload->cbObjectOffset =  (U32_WMC)(w);
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset+2, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }
            GetUnalignedDword(pData, dw);
            pPayload->cbObjectOffset =  dw;
            break;
        }
    default:
        cbWanted =0;
    }


    cbRepDataOffset = cbParseOffset + 2 + pParseInfoEx->bOffsetBytes;


    cbWanted =1;
    cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
    }
    pPayload->cbRepData = pData[cbLocalOffset];

    pPayload->msObjectPres = 0xffffffff;

    if(pPayload->cbRepData == 1)
    {
        pPayload->msObjectPres = pPayload->cbObjectOffset;
        pPayload->cbObjectOffset = 0;
        pPayload->cbObjectSize = 0;
        pPayload->bIsCompressedPayload =1;

        cbWanted =3;
        cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset + 1, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
		pPayload->dwDeltaPresTime = pData[0/*cbRepDataOffset +1*/];

		if( pParseInfoEx->fMultiPayloads)
		{
            GetUnalignedWord( &pData[ cbLocalOffset + 1 ],wTotalDataBytes );
		}
		else
		{
			wTotalDataBytes = 0;
		}
    }
    else if(pPayload->cbRepData >= 8)
    {
        cbWanted =8;
        cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset + 1, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        }
        GetUnalignedDword(&pData[cbLocalOffset],
                          pPayload->cbObjectSize);
        GetUnalignedDword(&pData[cbLocalOffset+ 4],
                          pPayload->msObjectPres);

        pPayload->bIsCompressedPayload =0;
    }

    pPayload->cbTotalSize = 1 + 1 + pParseInfoEx->bOffsetBytes + 1 + pPayload->cbRepData;

    if(pParseInfoEx->fMultiPayloads)
    {
        cbDummy = 0;

        switch(pParseInfoEx->bPayLenType)
        {
        case 0x01:
            {
                cbWanted =1;
                cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize, cbWanted, &pData, pDecoder->u32UserData);
                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                }

                dwPayloadSize =  (U32_WMC)(*pData);
                break;
            }
        case 0x02:
            {
                U16_WMC w;
                cbWanted =2;
                cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize, cbWanted, &pData, pDecoder->u32UserData);
                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                }
                GetUnalignedWord(pData, w);
                dwPayloadSize =  (U32_WMC)(w);
        
                break;

            }
        case 0x03:
            {
                U32_WMC dw;
                cbWanted =4;
                cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize, cbWanted, &pData, pDecoder->u32UserData);
                if(cbActual != cbWanted)
                {
                    return WMCDec_BufferTooSmall;
                }
                GetUnalignedDword(pData, dw);
                dwPayloadSize =  dw;
                break;
            }
        default:
            cbWanted =0;
        }

    }
    else if(pParseInfoEx->cbExplicitPacketLength > 0)
    {
        dwPayloadSize = pParseInfoEx->cbExplicitPacketLength
                      - pParseInfoEx->cbParseOffset
                      - pPayload->cbTotalSize
                      - pParseInfoEx->cbPadding;
    }
    else
    {
        dwPayloadSize = pDecoder->tHeaderInfo.cbPacketSize
                      - pParseInfoEx->cbParseOffset
                      - pPayload->cbTotalSize
                      - pParseInfoEx->cbPadding;
    }
    if (0 == wTotalDataBytes)
        wTotalDataBytes = (U16_WMC) dwPayloadSize;

    pPayload->cbPayloadSize = (U16_WMC)dwPayloadSize;

    pPayload->cbTotalSize += pParseInfoEx->bPayBytes
                          + (U16_WMC)pPayload->cbPayloadSize;

 	pPayload->wTotalDataBytes = wTotalDataBytes; // Amit

    pParseInfoEx->cbParseOffset += pPayload->cbTotalSize;

    if(pParseInfoEx->cbParseOffset > pDecoder->tHeaderInfo.cbPacketSize)
    {
        return WMCDec_BadData;
    }

    return WMCDec_Succeeded;
}

/****************************************************************************/

tWMCDecStatus WMCDecParseVirtualPacketHeader(HWMCDECODER hDecoder, U32_WMC cbCurrentPacketOffset, PACKET_PARSE_INFO_EX* pParseInfoEx)
{
    U8_WMC b;
    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
    U32_WMC cbLocalOffset=0;
    WMFDecoderEx *pDecoder = NULL;

    if( NULL == hDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hDecoder);


    cbWanted = 1;
    cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }

    /* ParsePacket begins */


    pParseInfoEx->fParityPacket = FALSE_WMC;
    pParseInfoEx->cbParseOffset = 0;

    b = pData[cbLocalOffset];
    
    pParseInfoEx->fEccPresent = (BOOL)( (b&0x80) == 0x80 );
    pParseInfoEx->bECLen = 0;

    if(pParseInfoEx->fEccPresent)
    {
        if(b&0x10)
        {
            pParseInfoEx->fParityPacket = TRUE;
            return WMCDec_Succeeded;
        }

        if(b&0x60)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->bECLen = (b&0x0f);
        if(pParseInfoEx->bECLen != 2)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->cbParseOffset = (U32_WMC)(1 + pParseInfoEx->bECLen);
//////////////////////////////////////////////////////////////////////////////
        cbWanted = 1;              /* at least */

        cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        
        }

        b = pData[cbLocalOffset];

//////////////////////////////////////////////////////////////////////////////
    }

    pParseInfoEx->cbPacketLenTypeOffset = pParseInfoEx->cbParseOffset;

    pParseInfoEx->bPacketLenType = (b&0x60)>>5;
    if(pParseInfoEx->bPacketLenType != 0
       && pParseInfoEx->bPacketLenType != 2)
    {
        return WMCDec_Fail;
    }

    pParseInfoEx->bPadLenType = (b&0x18)>>3;
    if(pParseInfoEx->bPadLenType == 3)
    {
        return WMCDec_Fail;
    }

    pParseInfoEx->bSequenceLenType = (b&0x06)>>1;

    pParseInfoEx->fMultiPayloads = (BOOL)(b&0x01);

    pParseInfoEx->cbParseOffset++;

//////////////////////////////////////////////////////////////////////////////
        cbWanted = 1;              /* at least */
        cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;
        
        }

        b = pData[cbLocalOffset];

//////////////////////////////////////////////////////////////////////////////

    pParseInfoEx->bOffsetBytes = 4;
    pParseInfoEx->bOffsetLenType = 3;

    if(b != 0x5d)
    {
        if((b&0xc0) != 0x40)
        {
            return WMCDec_Fail;
        }

        if((b&0x30) != 0x10)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->bOffsetLenType = (b&0x0c)>>2;
        if(pParseInfoEx->bOffsetLenType == 0)
        {
            return WMCDec_Fail;
        }
        else if(pParseInfoEx->bOffsetLenType < 3)
        {
            pParseInfoEx->bOffsetBytes = pParseInfoEx->bOffsetLenType;
        }

        if((b&0x03) != 0x01)
        {
            return WMCDec_Fail;
        }
    }

    pParseInfoEx->cbParseOffset++;

//////////////////////////////////////////////////////////////////////////////
    pParseInfoEx->cbPacketLenOffset = pParseInfoEx->cbParseOffset; 
    switch(pParseInfoEx->bPacketLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            pParseInfoEx->cbExplicitPacketLength =  (U32_WMC)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedWord(pData, w);
            pParseInfoEx->cbExplicitPacketLength =  (U32_WMC)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedDword(pData, dw);
            pParseInfoEx->cbExplicitPacketLength =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
    pParseInfoEx->cbSequenceOffset = pParseInfoEx->cbParseOffset;
    switch(pParseInfoEx->bSequenceLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            pParseInfoEx->dwSequenceNum =  (U32_WMC)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedWord(pData, w);
            pParseInfoEx->dwSequenceNum =  (U32_WMC)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedDword(pData, dw);
            pParseInfoEx->dwSequenceNum =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////    
    pParseInfoEx->cbPadLenOffset = pParseInfoEx->cbParseOffset;
    switch(pParseInfoEx->bPadLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            pParseInfoEx->cbPadding =  (U32_WMC)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            U16_WMC w;
            cbWanted =2;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedWord(pData, w);
            pParseInfoEx->cbPadding =  (U32_WMC)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            U32_WMC dw;
            cbWanted =4;
            cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
        
            }
            GetUnalignedDword(pData, dw);
            pParseInfoEx->cbPadding =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

    
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Now read 6 bytes
    
    cbWanted =6;
    cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;

    }

/////////////////////////////////////////////////////////////////////////////////
    GetUnalignedDword(&pData[cbLocalOffset], pParseInfoEx->dwSCR);

/////////////////////////////////////////////////////////////////////////////////
    
    pParseInfoEx->cbParseOffset += 4;

//////////////////////////////////////////////////////////////////////////////////
    GetUnalignedWord(&pData[cbLocalOffset+4], pParseInfoEx->wDuration);
    
////////////////////////////////////////////////////////////////////////////////////
    
    pParseInfoEx->cbParseOffset += 2;


    /* ParsePacketEx begins */
    if( pParseInfoEx->fEccPresent && pParseInfoEx->fParityPacket ) 
    {
        return WMCDec_Succeeded;
    }


    pParseInfoEx->cbPayLenTypeOffset = 0;
    pParseInfoEx->bPayLenType = 0;
    pParseInfoEx->bPayBytes = 0;
    pParseInfoEx->cPayloads = 1;

    if(pParseInfoEx->fMultiPayloads)
    {
//////////////////////////////////////////////////////
        cbWanted =1;
        cbActual = WMCDecCBGetData(hDecoder, cbCurrentPacketOffset + pParseInfoEx->cbParseOffset, cbWanted, &pData, pDecoder->u32UserData);

        if(cbActual != cbWanted)
        {
            return WMCDec_BufferTooSmall;

        }

        b = pData[cbLocalOffset];
//////////////////////////////////////////////////////
        pParseInfoEx->cbPayLenTypeOffset = pParseInfoEx->cbParseOffset;

        pParseInfoEx->bPayLenType = (b&0xc0)>>6;
        if(pParseInfoEx->bPayLenType != 2
           && pParseInfoEx->bPayLenType != 1)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->bPayBytes = pParseInfoEx->bPayLenType;

        pParseInfoEx->cPayloads = (U32_WMC)(b&0x3f);
        if(pParseInfoEx->cPayloads == 0)
        {
            return WMCDec_Fail;
        }

        pParseInfoEx->cbParseOffset++;
    }

    return WMCDec_Succeeded;
}


/****************************************************************************/
tWMCDecStatus
LoadClockObject(U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder)
{
    U32_WMC cbObjectOffset = 0;

    GUID_WMC PacketClockType;
    U16_WMC PacketClockSize;
    U32_WMC dwHeaderExtDataSize =0;
    WMFDecoderEx *pDecoder = NULL;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;
	tWMCDecStatus rc = WMCDec_Succeeded;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;




    cbWanted = 18;
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;

    LoadGUID (PacketClockType , pData);

	if (IsEqualGUID_WMC( &CLSID_CAsfPacketClock1, &PacketClockType ) ==0)
	{
        pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
		return WMCDec_BadAsfHeader;
	}

    LoadWORD (PacketClockSize      , pData);

    if (PacketClockSize != 6)
    {
        pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    
		return WMCDec_BadAsfHeader;
    }
    
    cbWanted = 4;
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;

    LoadDWORD (dwHeaderExtDataSize    , pData);

	if (dwHeaderExtDataSize >= MIN_OBJECT_SIZE )
	{
		// There are more objects. parse them.
	
		GUID_WMC sobjId;
		U64_WMC sqwSize;
	
	
		while(cbObjectOffset < cbBuffer)
		{
			rc = LoadVirtualObjectHeader(phDecoder, &sobjId, &sqwSize, cbObjectOffset);
			if(rc != WMCDec_Succeeded)
			{
				return rc;
			}

			if( IsEqualGUID_WMC( &CLSID_AsfXMetaDataObject, &sobjId ) )
			{
				rc = LoadVirtualMetaDataObject(
						 //           pbBuffer,
									*((U32_WMC*) &sqwSize),
									phDecoder, cbObjectOffset );
				
				cbObjectOffset += (*((U32_WMC*)&sqwSize));
				if (WMCDec_Succeeded != rc)
					goto sabort;
			}
			else
			{
				/* skip over this object */
				cbObjectOffset += (*((U32_WMC*)&sqwSize));
			}
		}
	
	}

sabort:

    pDecoder->tHeaderInfo.cbCurrentPacketOffset +=(U64_WMC)cbBuffer; // Use whole Object    

    return WMCDec_Succeeded;
}

/****************************************************************************/
tWMCDecStatus
LoadVirtualMetaDataObject(
                    U32_WMC cbBuffer,
                    HWMCDECODER *phDecoder, U32_WMC Offset)
{
    U32_WMC cbObjectOffset = 0;

    U16_WMC DescRecCount=0;
    WMFDecoderEx *pDecoder = NULL;
    U16_WMC i=0;

    U8_WMC *pData = NULL;
    U32_WMC cbWanted;
    U32_WMC cbActual;

	WMCMetaDataDescRecords *tDesc = NULL;

    if( NULL == phDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phDecoder);
    
    if(cbBuffer < MIN_OBJECT_SIZE)
        return WMCDec_BufferTooSmall;

    cbBuffer -= MIN_OBJECT_SIZE;
	Offset+=MIN_OBJECT_SIZE;
//	cbObjectOffset+= MIN_OBJECT_SIZE;

    cbWanted = 2;
    if(cbObjectOffset + cbWanted > cbBuffer)
    {
        return WMCDec_BufferTooSmall;
        
    }

    cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + Offset+ cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

    if(cbActual != cbWanted)
    {
        return WMCDec_BufferTooSmall;
        
    }
    cbObjectOffset += cbActual;

    LoadWORD(DescRecCount  , pData);

	if (DescRecCount >0)
	{
		pDecoder->tMetaDataEntry.m_wDescRecordsCount = DescRecCount;	
		pDecoder->tMetaDataEntry.pDescRec = (WMCMetaDataDescRecords *)wmvalloc(DescRecCount*sizeof(WMCMetaDataDescRecords));	
		
		if (NULL == pDecoder->tMetaDataEntry.pDescRec)
			return WMCDec_BadMemory;
		tDesc = pDecoder->tMetaDataEntry.pDescRec;	

		for (i=0; i<DescRecCount; i++)
		{
			cbWanted = 12;
			if(cbObjectOffset + cbWanted > cbBuffer)
			{
				return WMCDec_BufferTooSmall;
        
			}

			cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + Offset+ cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

			if(cbActual != cbWanted)
			{
				return WMCDec_BufferTooSmall;
        
			}
			cbObjectOffset += cbActual;
			
			LoadWORD(pDecoder->tMetaDataEntry.pDescRec[i].wLangIdIndex  , pData);
			LoadWORD(pDecoder->tMetaDataEntry.pDescRec[i].wStreamNumber   , pData);
			LoadWORD(pDecoder->tMetaDataEntry.pDescRec[i].wNameLenth  , pData);
			LoadWORD(pDecoder->tMetaDataEntry.pDescRec[i].wDataType  , pData);
			LoadWORD(pDecoder->tMetaDataEntry.pDescRec[i].wDataLength  , pData);
			
			if (pDecoder->tMetaDataEntry.pDescRec[i].wNameLenth >0)  // Length is in bytes
			{
				pDecoder->tMetaDataEntry.pDescRec[i].pwName = (U16Char_WMC *)wmvalloc(pDecoder->tMetaDataEntry.pDescRec[i].wNameLenth);	
				if(NULL == pDecoder->tMetaDataEntry.pDescRec[i].pwName)
					return WMCDec_BadMemory;
			
				cbWanted = (pDecoder->tMetaDataEntry.pDescRec[i].wNameLenth);
				if(cbObjectOffset + cbWanted > cbBuffer)
				{
					return WMCDec_BufferTooSmall;
        
				}

				cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + Offset+ cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

				if(cbActual != cbWanted)
				{
					return WMCDec_BufferTooSmall;
        
				}
				cbObjectOffset += cbActual;

				memcpy(pDecoder->tMetaDataEntry.pDescRec[i].pwName, pData, pDecoder->tMetaDataEntry.pDescRec[i].wNameLenth);
			}


			if (pDecoder->tMetaDataEntry.pDescRec[i].wDataLength >0)
			{
				U32_WMC dwMemLength =0;

				dwMemLength = pDecoder->tMetaDataEntry.pDescRec[i].wDataLength;
			/*	switch(pDecoder->tMetaDataEntry.pDescRec->wDataType)
				{
				case 0x0000:
				case 0x8000:
					dwMemLength = sizeof(U16Char_WMC)*pDecoder->tMetaDataEntry.pDescRec->wDataLength;
					break;
				case 0x0001:
				case 0x8001:
					dwMemLength = pDecoder->tMetaDataEntry.pDescRec->wDataLength;
					break;
				case 0x0002:
				case 0x8002:
					dwMemLength = 2;
					break;
				case 0x0003:
				case 0x8003:
					dwMemLength = 4;
					break;
				case 0x0004:
				case 0x8004:
					dwMemLength = 8;
					break;
				case 0x0005:
				case 0x8005:
					dwMemLength = 2;
					break;
				case 0x0006:
				case 0x8006:
					dwMemLength = 8;
					break;
				default:
					dwMemLength = pDecoder->tMetaDataEntry.pDescRec->wDataLength;
				} */

				pDecoder->tMetaDataEntry.pDescRec[i].pData = (Void_WMC *)wmvalloc(dwMemLength);	
				if(NULL == pDecoder->tMetaDataEntry.pDescRec[i].pData)
					return WMCDec_BadMemory;
			
				cbWanted = dwMemLength;
				if(cbObjectOffset + cbWanted > cbBuffer)
				{
					return WMCDec_BufferTooSmall;
        
				}

				cbActual = WMCDecCBGetData(*phDecoder, pDecoder->tHeaderInfo.cbCurrentPacketOffset + Offset+ cbObjectOffset, cbWanted, &pData, pDecoder->u32UserData);

				if(cbActual != cbWanted)
				{
					return WMCDec_BufferTooSmall;
        
				}
				cbObjectOffset += cbActual;

				memcpy(pDecoder->tMetaDataEntry.pDescRec[i].pData, pData, dwMemLength);
			}

			tDesc++;	
		}
	}



    return WMCDec_Succeeded;
}

/****************************************************************************/
