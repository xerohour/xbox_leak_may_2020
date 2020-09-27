/*
 * Windows Media Audio (WMA) Decoder API (parsering)
 *
 * Copyright (c) Microsoft Corporation 1999.  All Rights Reserved.
 */

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#if defined _WIN32
#include <windows.h>
#define _BOOL_DEFINED
#endif

#include <stdio.h>
#include "malloc.h"

#include "msaudio.h"
#include "wmaudio.h"
#include "wmaudio_type.h"
#include "loadstuff.h"
#define INITGUID
#include "wmaguids.h"
#include "wmamath.h"
#include "..\..\..\dsound\dsndver.h"

#ifndef WMAAPI_NO_DRM
#include "drmpd.h"
#endif /* WMAAPI_NO_DRM */


#ifndef MIN
#define MIN(x, y)  ((x) < (y) ? (x) : (y))
#endif /* MIN */


//
//  Routines for byte-swapping the members of various structs
//

#ifdef BIG_ENDIAN

void ByteSwapWaveFormatEx( WAVEFORMATEX& dst );
void ByteSwapAsfXAcmAudioErrorMaskingData( AsfXAcmAudioErrorMaskingData& dst );

#endif

//unsigned char g_Buffer[1024];
#define MAX_BUFSIZE 128

void SWAPWORD(tWMA_U8 * pSrc)
{
    tWMA_U8 Temp;
    Temp  = *pSrc;
    *pSrc = *(pSrc+1);
    *(pSrc+1)= Temp;
}

void SWAPDWORD(tWMA_U8 * pSrc)
{
    tWMA_U8 Temp;
    Temp      = *pSrc;
    *pSrc     = *(pSrc+3);
    *(pSrc+3) = Temp;
    Temp      = *(pSrc+1);
    *(pSrc+1) = *(pSrc+2);
    *(pSrc+2) = Temp;
}

void SwapWstr(tWMA_U16 *src, tWMA_U32 n)
{
    tWMA_U32 j;
    tWMA_U8 *p = (tWMA_U8 *)src;
    for (j = 0; j < n; j++) {
        SWAPWORD(p);
        p += 2;
    }
}

/****************************************************************************/
/*
tWMA_U32 WMAFileGetData (
    tHWMAFileState *state,
    tWMA_U32 offset,
    tWMA_U32 num_bytes,
    unsigned char **ppData)
{
    tWMA_U32 dwActual;
    unsigned char *pDst, *pSrc;
    if (num_bytes > 1024) 
    {
        WMADebugMessage("** WMAFileGetLicenseData: Requested too much (%lu).\n",
                num_bytes);
    }
    dwActual = 0;
    pDst = g_Buffer;
    while (MAX_BUFSIZE < num_bytes) {
        dwActual  += WMAFileCBGetData (state, offset, MAX_BUFSIZE,&pSrc);
        num_bytes -= MAX_BUFSIZE;
        memcpy(pDst,pSrc,MAX_BUFSIZE);
        pDst      += MAX_BUFSIZE;
        offset    += MAX_BUFSIZE;
    }
    dwActual += WMAFileCBGetData (state, offset, num_bytes,&pSrc);
    memcpy(pDst,pSrc,num_bytes);
    *ppData = g_Buffer;
    return dwActual;
}
*/
/****************************************************************************/
static DWORD HnsQWORDtoMsDWORD (QWORD qw)
{
    DWORD msLo, msHi;

    msLo = qw.dwLo/10000;
    msHi = (DWORD)ftoi((float)qw.dwHi*429496.7296f);

    return msLo + msHi;
}


/****************************************************************************/
DWORD WMA_GetBuffer (

    tWMAFileStateInternal *pInt,
    DWORD cbOffset,
    DWORD cbTotal,
    BYTE *pBuffer)
{
    DWORD cbReturn = 0;
    BYTE *pData;
    DWORD cbActual;
    DWORD cbWanted;

    if(pBuffer == NULL)
    {
        return 0;
    }

    while(cbTotal > 0)
    {
        cbWanted = MIN(WMA_MAX_DATA_REQUESTED, cbTotal);
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->hdr_parse.pCallbackContext,
                                    cbOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return 0;
        }

        memcpy(pBuffer, pData, (size_t)cbActual);

        pBuffer += cbActual;
        cbOffset += cbActual;
        cbTotal -= cbActual;
        cbReturn += cbActual;
    }

    return cbReturn;        
}


/****************************************************************************/
WMAERR
WMA_LoadObjectHeader(tWMAFileHdrStateInternal *pInt,
                     GUID *pObjectId,
                     QWORD *pqwSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbWanted = MIN_OBJECT_SIZE;
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    pInt->currPacketOffset += cbActual;

    LoadGUID(*pObjectId, pData);
    LoadQWORD(*pqwSize, pData);

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_LoadHeaderObject(tWMAFileHdrStateInternal *pInt,
                     int isFull)
{
    GUID objectId;
    QWORD qwSize;
    DWORD cHeaders;
    BYTE align;
    BYTE arch;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

#ifndef WMAAPI_NO_DRM
#ifdef GLOBAL_SECRET

    if(isFull)
    {
        SetSecret_4of7(((tWMAFileStateInternal *)pInt)->pDRM_state);
    }
#endif // GLOBAL_SECRET

#endif /* WMAAPI_NO_DRM */

    cbWanted = MIN_OBJECT_SIZE + sizeof(DWORD) + 2*sizeof(BYTE);
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    pInt->currPacketOffset += cbActual;

    LoadGUID(objectId, pData);
    LoadQWORD(qwSize, pData);
    LoadDWORD(cHeaders, pData);
    LoadBYTE(align, pData);
    LoadBYTE(arch, pData);

    if(!WMA_IsEqualGUID(&CLSID_CAsfHeaderObjectV0, &objectId)
       || align != 1
       || arch != 2)
    {
        return WMAERR_INVALIDHEADER;
    }

    /* use all */
    pInt->cbHeader = qwSize.dwLo;
    
    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_LoadPropertiesObject(tWMAFileHdrStateInternal *pInt,
                         DWORD cbSize,
                         int isFull)
{
    GUID mmsId;
    QWORD qwTotalSize;
    QWORD qwCreateTime;
    QWORD qwPackets;
    QWORD qwPlayDuration;
    QWORD qwSendDuration;
    QWORD qwPreroll;
    DWORD dwFlags;
    DWORD dwMinPacketSize;
    DWORD dwMaxPacketSize;
    DWORD dwMaxBitrate;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }
    cbSize -= MIN_OBJECT_SIZE;

#ifndef WMAAPI_NO_DRM
#ifdef GLOBAL_SECRET

    if(isFull)
    {
        SetSecret_5of7(((tWMAFileStateInternal *)pInt)->pDRM_state);
    }
#endif // GLOBAL_SECRET

#endif /* WMAAPI_NO_DRM */

    cbWanted = sizeof(GUID) + 6*sizeof(QWORD) + 4*sizeof(DWORD);
    if(cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }

    LoadGUID(mmsId, pData);
    LoadQWORD(qwTotalSize, pData);
    LoadQWORD(qwCreateTime, pData);
    LoadQWORD(qwPackets, pData);
    LoadQWORD(qwPlayDuration, pData);
    LoadQWORD(qwSendDuration, pData);
    LoadQWORD(qwPreroll, pData);
    LoadDWORD(dwFlags, pData);
    LoadDWORD(dwMinPacketSize, pData);
    LoadDWORD(dwMaxPacketSize, pData);
    LoadDWORD(dwMaxBitrate, pData);

    if(dwMinPacketSize != dwMaxPacketSize
       || (qwPackets.dwLo == 0 && qwPackets.dwHi == 0))
    {
        return WMAERR_FAIL;
    }

    pInt->cbPacketSize = dwMaxPacketSize;
    pInt->cPackets     = qwPackets.dwLo;
    pInt->msDuration   = HnsQWORDtoMsDWORD(qwSendDuration);
    pInt->msPreroll    = qwPreroll.dwLo;
    
    /* use all */
    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_LoadAudioObject(tWMAFileHdrStateInternal *pInt,
                    DWORD cbSize,
                    int isFull)
{
    GUID streamType;
    GUID ecStrategy;
    QWORD qwOffset;
    DWORD cbTypeSpecific;
    DWORD cbErrConcealment;
    WORD wStreamNum;
    DWORD dwJunk;
    DWORD nBlocksPerObject;
    AsfXAcmAudioErrorMaskingData *pScramblingData;
    // WAVEFORMATEX *pFmt;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

	    WORD  tw;
	    DWORD tdw;
	    const BYTE *tp;

    
    DWORD cbObjectOffset = 0;



    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

#ifndef WMAAPI_NO_DRM
#ifdef GLOBAL_SECRET

    if (isFull)
    {
        SetSecret_3of7(((tWMAFileStateInternal *)pInt)->pDRM_state);
    }
#endif // GLOBAL_SECRET

#endif /* WMAAPI_NO_DRM */

    cbWanted = 2*sizeof(GUID) + sizeof(QWORD) + 3*sizeof(DWORD) + sizeof(WORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadGUID(streamType, pData);
    LoadGUID(ecStrategy, pData);
    LoadQWORD(qwOffset, pData);
    LoadDWORD(cbTypeSpecific, pData);
    LoadDWORD(cbErrConcealment, pData);
    LoadWORD(wStreamNum, pData);
    LoadDWORD(dwJunk, pData);

    wStreamNum &= 0x7F;

    if( !WMA_IsEqualGUID( &CLSID_AsfXStreamTypeAcmAudio, &streamType ) )
    {
#ifndef WMAAPI_NO_DRM_STREAM
        /* Remember the DRM Aux Data stream number */
        if( WMA_IsEqualGUID( &CLSID_AsfXStreamTypeDRMAuxData, &streamType ) )
        {
            pInt->bDRMAuxStreamNum = (BYTE) wStreamNum;
        }
#endif  /* WMAAPI_NO_DRM_STREAM */

        /* Skip over the rest */
        pInt->currPacketOffset += cbSize;
        return WMAERR_OK;
    }

    /* Type specific */
    pInt->wAudioStreamId = wStreamNum; //Amit

    if(cbTypeSpecific > 0)
    {
        cbWanted = cbTypeSpecific;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        tp = pData;
        LoadWORD (tw ,tp);

        switch(tw)
        {
        case WAVE_FORMAT_WMAUDIO2:

            if(cbTypeSpecific < 28 /*sizeof(WMAUDIO2WAVEFORMAT)*/)
            {
                return WMAERR_FAIL;
            }

            pInt->nVersion         = 2;
		    tp = pData +  4; LoadDWORD(tdw,tp); 
            pInt->nSamplesPerSec   = tdw;
            tp = pData +  8; LoadDWORD(tdw,tp);
            pInt->nAvgBytesPerSec  = tdw;
            tp = pData + 12; LoadWORD (tw ,tp);
            pInt->nBlockAlign      = tw;
            tp = pData +  2; LoadWORD (tw ,tp);
            pInt->nChannels        = tw;
	        tp = pData + 22; LoadWORD (tw ,tp);
	        tp = pData + 18; LoadDWORD(tdw,tp);
            pInt->nSamplesPerBlock = tdw;
            pInt->nEncodeOpt       = tw;

            break;
        case WAVE_FORMAT_MSAUDIO1:
            if(cbTypeSpecific < 22 /*sizeof(MSAUDIO1WAVEFORMAT)*/)
            {
                return WMAERR_FAIL;
            }

            pInt->nVersion         = 1;
		    tp = pData +  4; LoadDWORD(tdw,tp); 
            pInt->nSamplesPerSec   = tdw;
            tp = pData +  8; LoadDWORD(tdw,tp);
            pInt->nAvgBytesPerSec  = tdw;
            tp = pData + 12; LoadWORD (tw ,tp);
            pInt->nBlockAlign      = tw;
            tp = pData +  2; LoadWORD (tw ,tp);
            pInt->nChannels        = tw;
	        tp = pData + 20; LoadWORD (tw,tp);
            pInt->nEncodeOpt       = tw;
	        tp = pData + 18; LoadWORD (tw,tp);
            pInt->nSamplesPerBlock = tw;

            break;
        default:
            // unknown...
            return WMAERR_FAIL;
        }
    }

    /* Error concealment - this can get as big as 400!!! */

    if(cbErrConcealment > 0)
    {
        if(WMA_IsEqualGUID(&CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy))
        {
            cbWanted = sizeof(AsfXSignatureAudioErrorMaskingData);
        }
        else if(WMA_IsEqualGUID(&CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy))
        {
//            cbWanted = sizeof(AsfXAcmAudioErrorMaskingData);
            cbWanted = 8;
        }
        else
        {
            return WMAERR_FAIL;
        }

        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbErrConcealment; // Skip over the rest - cbActual;

        if(WMA_IsEqualGUID(&CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy))
        {
            pInt->cbAudioSize = ((AsfXSignatureAudioErrorMaskingData *)pData)->maxObjectSize;
            
#ifdef BIG_ENDIAN
			// byte-swap the object size
			SWAP_DWORD(pInt->cbAudioSize);
#endif

        }
        else if(WMA_IsEqualGUID(&CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy))
        {
            pScramblingData = (AsfXAcmAudioErrorMaskingData *)pData;
            
#ifdef BIG_ENDIAN
			// byte-swap the struct
			ByteSwapAsfXAcmAudioErrorMaskingData( *pScramblingData );
#endif


//            pInt->cbAudioSize = (DWORD)(pScramblingData->virtualPacketLen*pScramblingData->span);
            pInt->cbAudioSize = (DWORD)(((WORD)(pData+1)) * ((WORD)(*pData)));

			if (pScramblingData->span >1)
				return WMAERR_FAIL;
            
#ifdef BIG_ENDIAN
			// byte-swap the object size
			SWAP_DWORD(pInt->cbAudioSize);
#endif
        }
        else
        {
            return WMAERR_FAIL;
        }
    }

    nBlocksPerObject = pInt->cbAudioSize/pInt->nBlockAlign;
    pInt->cbAudioSize = nBlocksPerObject*pInt->nSamplesPerBlock*pInt->nChannels*2;

#ifndef WMAAPI_NO_DRM_STREAM
    /* Remember which stream contains the audio payloads */
    pInt->bAudioStreamNum = (BYTE) wStreamNum;
#endif  /* WMAAPI_NO_DRM_STREAM */
    
    /* use all */
    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_LoadEncryptionObject(tWMAFileHdrStateInternal *pInt,
                         DWORD cbSize)
{
    DWORD cbBlock;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    DWORD cbObjectOffset = 0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    pInt->cbSecretData = 0;

    /* SecretData */

    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(pInt->cbSecretData, pData);

    if(pInt->cbSecretData)
    {
        cbWanted = pInt->cbSecretData;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        memcpy(pInt->pbSecretData, pData, (size_t)cbActual);
    }

    /* Type string */

    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(cbBlock, pData);

    if(cbBlock)
    {
        cbWanted = cbBlock;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        memcpy(pInt->pbType, pData, (size_t)cbActual);
    }

    /* Key ID */

    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(cbBlock, pData);

    if(cbBlock)
    {
        cbWanted = cbBlock;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        memcpy(pInt->pbKeyID, pData, (size_t)cbActual);
    }

    /* License URL */

    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(cbBlock, pData);

    if(cbBlock)
    {
        cbWanted = cbBlock;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
		while (cbWanted>0) {
			cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
										pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
			cbObjectOffset += cbActual;
			cbWanted -= cbActual;
		}

        /* ignore */
    }

    /* use all */
    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_LoadContentDescriptionObject(tWMAFileHdrStateInternal *pInt,
                                 DWORD cbSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    DWORD cbObjectOffset = 0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 5*sizeof(WORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadWORD(pInt->cbCDTitle, pData);
    LoadWORD(pInt->cbCDAuthor, pData);
    LoadWORD(pInt->cbCDCopyright, pData);
    LoadWORD(pInt->cbCDDescription, pData);
    LoadWORD(pInt->cbCDRating, pData);

    pInt->cbCDOffset = pInt->currPacketOffset + cbObjectOffset;

    /* use all */
    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}

/****************************************************************************/
WMAERR
WMA_LoadExtendedContentDescObject(tWMAFileHdrStateInternal *pInt,
                                 DWORD cbSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;
    DWORD cbBuffer;
    WORD i;
    tWMAExtendedContentDesc *pECDesc = NULL;
    tWMA_U16 cDescriptors;
    DWORD cbOffset=0;
    DWORD cbWanted1 =0;
//    unsigned char g_Buffer[1024];
//    #define MAX_BUFSIZE 128


    DWORD cbObjectOffset = 0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

/////////////////////////S//////////////////////
    cbBuffer = cbSize;

/*    cbWanted = cbSize;
    cbBuffer = cbSize;

    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;
*/
///////////////////////////////////////E//////////
    if(pInt->m_pECDesc != NULL) 
        goto SKIP;

    pInt->m_pECDesc = (tWMAExtendedContentDesc *)malloc(sizeof(tWMAExtendedContentDesc));
    if( pInt->m_pECDesc == NULL) {
        return( WMAERR_OUTOFMEMORY );
    }
    pECDesc = pInt->m_pECDesc;
////////////////////////////S//////////////
    cbWanted = sizeof(tWMA_U16);

    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;
    cbBuffer -= cbActual;
////////////////////////////E/////////////
    LoadWORD(cDescriptors, pData);
//    cbBuffer -= sizeof(tWMA_U16);

    pECDesc->cDescriptors = cDescriptors;
    pECDesc->pDescriptors = (ECD_DESCRIPTOR *)malloc(cDescriptors * sizeof(ECD_DESCRIPTOR));
   if(pECDesc->pDescriptors == NULL)
       return( WMAERR_OUTOFMEMORY );

   for(i = 0; i < cDescriptors; i++) {
  
///////////////////////////////////////////S/////////////////////////////
        cbWanted = sizeof(tWMA_U16);

        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;
///////////////////////////////////////////E///////////////////////////       
       LoadWORD(pECDesc->pDescriptors[i].cbName, pData);
       cbBuffer -= sizeof(tWMA_U16);
        if(cbBuffer < pECDesc->pDescriptors[i].cbName + sizeof(tWMA_U16) * 2)
            return( WMAERR_BUFFERTOOSMALL );
        pECDesc->pDescriptors[i].pwszName = NULL;
        pECDesc->pDescriptors[i].pwszName = (tWMA_U16 *)malloc(pECDesc->pDescriptors[i].cbName);
        if (pECDesc->pDescriptors[i].pwszName == NULL)
            return( WMAERR_OUTOFMEMORY );
        else
        {

    ///////////////////////////////////////////S/////////////////////////////
            cbWanted = pECDesc->pDescriptors[i].cbName + sizeof(tWMA_U16) * 2;
            
            if(cbWanted > MAX_BUFSIZE)
            {
                cbOffset=0;
                do
                {
                    cbWanted1 =  cbWanted > MAX_BUFSIZE ? MAX_BUFSIZE : cbWanted;
                    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                                pInt->pCallbackContext,
                                                pInt->currPacketOffset + cbObjectOffset, cbWanted1, &pData);
                    if(cbActual != cbWanted1)
                    {
                        return WMAERR_BUFFERTOOSMALL;
                    }
                    cbObjectOffset += cbActual;
                    cbWanted -=cbActual;
            ///////////////////////////////////////////E///////////////////////////       

                    memcpy(pECDesc->pDescriptors[i].pwszName+cbOffset, pData, cbActual);
                    cbOffset +=cbActual;
                }while(cbWanted >0);
            }
            else
            {
                cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                            pInt->pCallbackContext,
                                            pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
                if(cbActual != cbWanted)
                {
                    return WMAERR_BUFFERTOOSMALL;
                }
                cbObjectOffset += cbActual;
        ///////////////////////////////////////////E///////////////////////////       

                memcpy(pECDesc->pDescriptors[i].pwszName, pData, pECDesc->pDescriptors[i].cbName);
            }
        }
#ifndef LITTLE_ENDIAN
        SwapWstr((tWMA_U16 *)pECDesc->pDescriptors[i].pwszName, (tWMA_U32)(pECDesc->pDescriptors[i].cbName / sizeof(tWMA_U16)));
#endif
        pData += pECDesc->pDescriptors[i].cbName;
        cbBuffer -= pECDesc->pDescriptors[i].cbName;
        LoadWORD(pECDesc->pDescriptors[i].data_type, pData);
        cbBuffer -= sizeof(tWMA_U16);
        LoadWORD(pECDesc->pDescriptors[i].cbValue, pData);
        cbBuffer -= sizeof(tWMA_U16);
        if(cbBuffer < (DWORD)pECDesc->pDescriptors[i].cbValue)
            return( WMAERR_BUFFERTOOSMALL );
        pECDesc->pDescriptors[i].uValue.pbBinary = NULL;
        pECDesc->pDescriptors[i].uValue.pbBinary = (tWMA_U8 *)malloc (pECDesc->pDescriptors[i].cbValue);
        if (pECDesc->pDescriptors[i].uValue.pbBinary == NULL)
            return( WMAERR_OUTOFMEMORY );
        else
        {
    ///////////////////////////////////////////S/////////////////////////////
            cbWanted = pECDesc->pDescriptors[i].cbValue;

            if(cbWanted > MAX_BUFSIZE)
            {
                cbOffset=0;
                do
                {
                    cbWanted1 =  cbWanted > MAX_BUFSIZE ? MAX_BUFSIZE : cbWanted;
                    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                                pInt->pCallbackContext,
                                                pInt->currPacketOffset + cbObjectOffset, cbWanted1, &pData);
                    if(cbActual != cbWanted1)
                    {
                        return WMAERR_BUFFERTOOSMALL;
                    }
                    cbObjectOffset += cbActual;
                    cbWanted -=cbActual;
            ///////////////////////////////////////////E///////////////////////////       
                    memcpy(pECDesc->pDescriptors[i].uValue.pbBinary+cbOffset, pData, cbActual);
                    cbOffset +=cbActual;
                }while(cbWanted >0);
            }
            else
            {
                cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                            pInt->pCallbackContext,
                                            pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
                if(cbActual != cbWanted)
                {
                    return WMAERR_BUFFERTOOSMALL;
                }
                cbObjectOffset += cbActual;
        ///////////////////////////////////////////E///////////////////////////       

                memcpy(pECDesc->pDescriptors[i].uValue.pbBinary, pData, pECDesc->pDescriptors[i].cbValue);

            }
    }
#ifndef LITTLE_ENDIAN
        if((pECDesc->pDescriptors[i].data_type == ECD_DWORD) ||
        (pECDesc->pDescriptors[i].data_type == ECD_BOOL))
            SWAPDWORD((tWMA_U8 *)pECDesc->pDescriptors[i].uValue.pbBinary);
        else if(pECDesc->pDescriptors[i].data_type == ECD_STRING)
            SwapWstr((tWMA_U16 *)pECDesc->pDescriptors[i].uValue.pwszString, (tWMA_U32)(pECDesc->pDescriptors[i].cbValue / sizeof(tWMA_U16));
        else if(pECDesc->pDescriptors[i].data_type == ECD_WORD)
            SWAPWORD((tWMA_U8 *)pECDesc->pDescriptors[i].uValue.pbBinary);
#endif
        pData += pECDesc->pDescriptors[i].cbValue;
        cbBuffer -= pECDesc->pDescriptors[i].cbValue;
    }

SKIP:
    /* use all */
    pInt->currPacketOffset += cbSize;

    return( WMAERR_OK );
}

/****************************************************************************/
WMAERR
WMA_LoadLicenseStoreObject(tWMAFileHdrStateInternal *pInt,
                                 DWORD cbSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbDone;
    DWORD cbActual;

    DWORD m_dwFlag;

    DWORD cbObjectOffset = 0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 8;
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(m_dwFlag, pData);
    LoadDWORD(pInt->m_dwLicenseLen, pData);

    cbWanted = pInt->m_dwLicenseLen;
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_NOTDONE;
    }
    pInt->m_pLicData = malloc(cbWanted);

    cbDone = 0;
    while (cbWanted) {
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        memcpy(pInt->m_pLicData + cbDone, pData, cbActual);
        cbObjectOffset += cbActual;
        cbWanted -= cbActual;
        cbDone   += cbActual;
        if(cbActual == 0)
        {
            return WMAERR_FAIL;
        }
    }

//    pInt->cbCDOffset = pInt->currPacketOffset + cbObjectOffset;

    /* use all */
    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR  WMA_LoadMarkerObject(tWMAFileHdrStateInternal *pInt, DWORD cbSize, int iIndex)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    DWORD cbObjectOffset = 0;

    GUID    m_gMarkerStrategy;
    WORD    m_wAlignment;
    WORD    m_wNameLen;
    WORD    tw;
    unsigned int i, j;


    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }


    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 24;
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadGUID (m_gMarkerStrategy, pData);
    LoadDWORD(pInt->m_dwMarkerNum, pData);
    LoadWORD (m_wAlignment, pData);
    LoadWORD (m_wNameLen, pData);

    //pInt->m_pMarkers = (MarkerEntry *) malloc(sizeof(MarkerEntry)*pInt->m_dwMarkerNum);
	
//NQF+
	if( pInt->m_dwMarkerNum == 0)
	{
        /* use all */
        pInt->currPacketOffset += cbSize;
    	return WMAERR_OK;

    } else if ( iIndex < 0 ) {  //for query number of Markers
        
        /* use all */
        pInt->currPacketOffset += cbSize;
        return WMAERR_OK;

    } else if ( iIndex >= (int) pInt -> m_dwMarkerNum) {
        /* use all */
        pInt->currPacketOffset += cbSize;
        return WMAERR_BUFFERTOOSMALL;  //NQF_temp 
    }
//NQF-

    for (j = 0; j <= (unsigned int) iIndex; j++) {
        cbWanted = 18;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        LoadQWORD(pInt->m_pMarker->m_qOffset, pData);
        LoadQWORD(pInt->m_pMarker->m_qtime, pData);
        LoadWORD (pInt->m_pMarker->m_wEntryLen, pData);

        cbWanted = pInt->m_pMarker->m_wEntryLen;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(pInt->m_pMarker->m_dwSendTime, pData);
        LoadDWORD(pInt->m_pMarker->m_dwFlags, pData);
        LoadDWORD(pInt->m_pMarker->m_dwDescLen, pData);

        //pInt->m_pMarkers[j].m_pwDescName = (WORD *)malloc(sizeof(WORD)*pInt->m_pMarkers[j].m_dwDescLen);

        for (i=0;i<pInt->m_pMarker->m_dwDescLen && i < DESC_NAME_MAX_LENGTH; i++) {
            LoadWORD(tw, pData);
            pInt->m_pMarker->m_pwDescName[i] = tw;
        }
    }

    /* use all */
    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}

/****************************************************************************/
WMAERR WMA_GetMarkerObject(tWMAFileHdrStateInternal *pInt, int iIndex)
{
    WMAERR wmarc;
    GUID objId;
    QWORD qwSize;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    /* initialize the some state */

    pInt->currPacketOffset = 0;

    pInt->cbCDOffset      = 0;
    pInt->cbCDTitle       = 0;
    pInt->cbCDAuthor      = 0;
    pInt->cbCDCopyright   = 0;
    pInt->cbCDDescription = 0;
    pInt->cbCDRating      = 0;


    /* ASF Header Object */

    wmarc = WMA_LoadHeaderObject(pInt, 0);
    if(wmarc != WMAERR_OK)
    {
        return wmarc;
    }
    pInt->cbFirstPacketOffset = pInt->cbHeader += DATA_OBJECT_SIZE;

    /* Scan Header Objects */

    while(pInt->currPacketOffset < pInt->cbFirstPacketOffset)
    {
        wmarc = WMA_LoadObjectHeader(pInt, &objId, &qwSize);
        if(wmarc != WMAERR_OK)
        {
            return wmarc;
        }

        if(WMA_IsEqualGUID(&CLSID_CAsfMarkerObjectV0, &objId))
        {
            wmarc = WMA_LoadMarkerObject(pInt, qwSize.dwLo, iIndex);
            if(wmarc != WMAERR_OK)
            {
                return wmarc;            
            }
            break;  
        }
        else
        {
            /* skip over this object */
            pInt->currPacketOffset += qwSize.dwLo - MIN_OBJECT_SIZE;
        }
    }

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_ParseAsfHeader(tWMAFileHdrStateInternal *pInt,
                   int isFull)
{
    WMAERR wmarc;
    GUID objId;
    QWORD qwSize;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    /* initialize the some state */

    pInt->currPacketOffset = 0;

    pInt->cbCDOffset      = 0;
    pInt->cbCDTitle       = 0;
    pInt->cbCDAuthor      = 0;
    pInt->cbCDCopyright   = 0;
    pInt->cbCDDescription = 0;
    pInt->cbCDRating      = 0;

#ifndef WMAAPI_NO_DRM
#ifdef GLOBAL_SECRET

    if(isFull)
    {
        SetSecret_7of7(((tWMAFileStateInternal *)pInt)->pDRM_state);
    }

#endif // GLOBAL_SECRET

#endif /* WMAAPI_NO_DRM */

    /* ASF Header Object */

    wmarc = WMA_LoadHeaderObject(pInt, isFull);
    if(wmarc != WMAERR_OK)
    {
        return wmarc;
    }
    pInt->cbFirstPacketOffset = pInt->cbHeader += DATA_OBJECT_SIZE;

    /* Scan Header Objects */

    while(pInt->currPacketOffset < pInt->cbFirstPacketOffset)
    {
        wmarc = WMA_LoadObjectHeader(pInt, &objId, &qwSize);
        if(wmarc != WMAERR_OK)
        {
            return wmarc;
        }

        if(WMA_IsEqualGUID(&CLSID_CAsfPropertiesObjectV2, &objId))
        {
            wmarc = WMA_LoadPropertiesObject(pInt, qwSize.dwLo,
                                             isFull);
            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if( WMA_IsEqualGUID( &CLSID_CAsfStreamPropertiesObjectV1, &objId )
                 || WMA_IsEqualGUID( &CLSID_CAsfStreamPropertiesObjectV2, &objId ) )
        {
            wmarc = WMA_LoadAudioObject(pInt, qwSize.dwLo,
                                        isFull);
            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if(WMA_IsEqualGUID(&CLSID_CAsfContentEncryptionObject, &objId))
        {
            wmarc = WMA_LoadEncryptionObject(pInt, qwSize.dwLo);
            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if(WMA_IsEqualGUID(&CLSID_CAsfContentDescriptionObjectV0, &objId))
        {
            wmarc = WMA_LoadContentDescriptionObject(pInt, qwSize.dwLo);
            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if(WMA_IsEqualGUID(&CLSID_CAsfExtendedContentDescObject, &objId))
        {
            if(isFull) { // only in this case, we parse extended content desc
                wmarc = WMA_LoadExtendedContentDescObject(pInt, qwSize.dwLo);
                if(wmarc != WMAERR_OK) {
                    return wmarc;
                }
            }
            else
			{
				pInt->currPacketOffset += qwSize.dwLo - MIN_OBJECT_SIZE;
				wmarc = WMAERR_OK;
			}
        }
        //else if( 0 && WMA_IsEqualGUID(&CLSID_CAsfMarkerObjectV0, &objId))
        //{
        //   wmarc = WMA_LoadMarkerObject(pInt, qwSize.dwLo);
        //    if(wmarc != WMAERR_OK)
        //    {
        //        return wmarc;
        //    }
        //}
        else if(WMA_IsEqualGUID(&CLSID_CAsfLicenseStoreObject, &objId))
        {
            wmarc = WMA_LoadLicenseStoreObject(pInt, qwSize.dwLo);
            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else
        {
            /* skip over this object */
            pInt->currPacketOffset += qwSize.dwLo - MIN_OBJECT_SIZE;
        }
    }

    return WMAERR_OK;
}


/****************************************************************************/
static DWORD
GetASFVarField(BYTE bType,
               const BYTE *p,
               DWORD *pcbOffset)
{
    DWORD dwVar = 0;

    switch(bType)
    {
    case 0x01:
        dwVar = (DWORD)(*p);
        *pcbOffset += 1;
        break;
    case 0x02:
        {
            WORD w;
            GetUnalignedWord(p, w);
            dwVar = w;
            *pcbOffset += 2;
            break;
        }
    case 0x03:
        GetUnalignedDword(p, dwVar);
        *pcbOffset += 4;
        break;
    }

    return dwVar;
}




/****************************************************************************/
WMAERR
WMA_ParsePacketHeader(tWMAFileStateInternal *pInt)
{
    BYTE b;
    PACKET_PARSE_INFO_EX *pParseInfoEx;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;
    DWORD cbLocalOffset=0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    pParseInfoEx = &pInt->ppex;

//    cbWanted = 24;              /* at most */
    
    cbWanted = 1;              /* at least */
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->hdr_parse.currPacketOffset,
                                cbWanted, &pData);


    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }

    /* ParsePacket begins */

    pParseInfoEx->fParityPacket = FALSE;
    pParseInfoEx->cbParseOffset = 0;

//////////////////////////////////////////////////
//    b = pData[pParseInfoEx->cbParseOffset];

    b = pData[cbLocalOffset];
///////////////////////////////////////////////////
    
    pParseInfoEx->fEccPresent = (BOOL)( (b&0x80) == 0x80 );
    pParseInfoEx->bECLen = 0;

    if(pParseInfoEx->fEccPresent)
    {
        if(b&0x10)
        {
            pParseInfoEx->fParityPacket = TRUE;
            return WMAERR_OK;
        }

        if(b&0x60)
        {
            return WMAERR_FAIL;
        }

        pParseInfoEx->bECLen = (b&0x0f);
        if(pParseInfoEx->bECLen != 2)
        {
            return WMAERR_FAIL;
        }

        pParseInfoEx->cbParseOffset = (DWORD)(1 + pParseInfoEx->bECLen);
//////////////////////////////////////////////////////////////////////////////
        cbWanted = 1;              /* at least */
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                    cbWanted, &pData);

//        b = pData[pParseInfoEx->cbParseOffset];
          b = pData[cbLocalOffset];

//////////////////////////////////////////////////////////////////////////////
    }

    pParseInfoEx->cbPacketLenTypeOffset = pParseInfoEx->cbParseOffset;

    pParseInfoEx->bPacketLenType = (b&0x60)>>5;
    if(pParseInfoEx->bPacketLenType != 0
       && pParseInfoEx->bPacketLenType != 2)
    {
        return WMAERR_FAIL;
    }

    pParseInfoEx->bPadLenType = (b&0x18)>>3;
    if(pParseInfoEx->bPadLenType == 3)
    {
        return WMAERR_FAIL;
    }

    pParseInfoEx->bSequenceLenType = (b&0x06)>>1;

    pParseInfoEx->fMultiPayloads = (BOOL)(b&0x01);

    pParseInfoEx->cbParseOffset++;

//////////////////////////////////////////////////////////////////////////////
        cbWanted = 1;              /* at least */
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                    pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                    cbWanted, &pData);

//        b = pData[pParseInfoEx->cbParseOffset];
          b = pData[cbLocalOffset];

//////////////////////////////////////////////////////////////////////////////

    pParseInfoEx->bOffsetBytes = 4;
    pParseInfoEx->bOffsetLenType = 3;

    if(b != 0x5d)
    {
        if((b&0xc0) != 0x40)
        {
            return WMAERR_FAIL;
        }

        if((b&0x30) != 0x10)
        {
            return WMAERR_FAIL;
        }

        pParseInfoEx->bOffsetLenType = (b&0x0c)>>2;
        if(pParseInfoEx->bOffsetLenType == 0)
        {
            return WMAERR_FAIL;
        }
        else if(pParseInfoEx->bOffsetLenType < 3)
        {
            pParseInfoEx->bOffsetBytes = pParseInfoEx->bOffsetLenType;
        }

        if((b&0x03) != 0x01)
        {
            return WMAERR_FAIL;
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
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            pParseInfoEx->cbExplicitPacketLength =  (DWORD)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            WORD w;
            cbWanted =2;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            GetUnalignedWord(pData, w);
            pParseInfoEx->cbExplicitPacketLength =  (DWORD)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            DWORD dw;
            cbWanted =4;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            GetUnalignedDword(pData, dw);
            pParseInfoEx->cbExplicitPacketLength =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

/*    pParseInfoEx->cbPacketLenOffset = pParseInfoEx->cbParseOffset;
    pParseInfoEx->cbExplicitPacketLength = GetASFVarField(pParseInfoEx->bPacketLenType,
                                                          &pData[pParseInfoEx->cbParseOffset],
                                                          &pParseInfoEx->cbParseOffset);
*/

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
    pParseInfoEx->cbSequenceOffset = pParseInfoEx->cbParseOffset;
    switch(pParseInfoEx->bSequenceLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            pParseInfoEx->dwSequenceNum =  (DWORD)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            WORD w;
            cbWanted =2;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            GetUnalignedWord(pData, w);
            pParseInfoEx->dwSequenceNum =  (DWORD)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            DWORD dw;
            cbWanted =4;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            GetUnalignedDword(pData, dw);
            pParseInfoEx->dwSequenceNum =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

    /*    pParseInfoEx->cbSequenceOffset = pParseInfoEx->cbParseOffset;
    pParseInfoEx->dwSequenceNum = GetASFVarField(pParseInfoEx->bSequenceLenType,
                                                 &pData[pParseInfoEx->cbParseOffset],
                                                 &pParseInfoEx->cbParseOffset);
*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////    
    pParseInfoEx->cbPadLenOffset = pParseInfoEx->cbParseOffset;
    switch(pParseInfoEx->bPadLenType)
    {
    case 0x01:
        {
            cbWanted =1;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            pParseInfoEx->cbPadding =  (DWORD)(*pData);
            pParseInfoEx->cbParseOffset++;
            break;
        }
    case 0x02:
        {
            WORD w;
            cbWanted =2;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            GetUnalignedWord(pData, w);
            pParseInfoEx->cbPadding =  (DWORD)(w);
            pParseInfoEx->cbParseOffset+=2;
        
            break;

        }
    case 0x03:
        {
            DWORD dw;
            cbWanted =4;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                        cbWanted, &pData);
            GetUnalignedDword(pData, dw);
            pParseInfoEx->cbPadding =  dw;
            pParseInfoEx->cbParseOffset+=4;
            break;
        }
    default:
        cbWanted =0;
    }

    
    
    
    /*    pParseInfoEx->cbPadLenOffset = pParseInfoEx->cbParseOffset;
    pParseInfoEx->cbPadding = GetASFVarField(pParseInfoEx->bPadLenType,
                                             &pData[pParseInfoEx->cbParseOffset],
                                             &pParseInfoEx->cbParseOffset);
*/

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Now read 6 bytes
    
    cbWanted =6;
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                 pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                 cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
    GetUnalignedDword(&pData[cbLocalOffset], pParseInfoEx->dwSCR);
//    GetUnalignedDword(&pData[pParseInfoEx->cbParseOffset], pParseInfoEx->dwSCR);

/////////////////////////////////////////////////////////////////////////////////
    
    pParseInfoEx->cbParseOffset += 4;


//////////////////////////////////////////////////////////////////////////////////
    GetUnalignedWord(&pData[cbLocalOffset+4], pParseInfoEx->wDuration);
    
//    GetUnalignedWord(&pData[pParseInfoEx->cbParseOffset], pParseInfoEx->wDuration);

////////////////////////////////////////////////////////////////////////////////////
    
    pParseInfoEx->cbParseOffset += 2;


    /* ParsePacketEx begins */

    pParseInfoEx->cbPayLenTypeOffset = 0;
    pParseInfoEx->bPayLenType = 0;
    pParseInfoEx->bPayBytes = 0;
    pParseInfoEx->cPayloads = 1;

    if(pParseInfoEx->fMultiPayloads)
    {
//////////////////////////////////////////////////////
        cbWanted =1;
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                     pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                     cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }

//        b = pData[pParseInfoEx->cbParseOffset];
        b = pData[cbLocalOffset];
//////////////////////////////////////////////////////
        pParseInfoEx->cbPayLenTypeOffset = pParseInfoEx->cbParseOffset;

        pParseInfoEx->bPayLenType = (b&0xc0)>>6;
        if(pParseInfoEx->bPayLenType != 2
           && pParseInfoEx->bPayLenType != 1)
        {
            return WMAERR_FAIL;
        }

        pParseInfoEx->bPayBytes = pParseInfoEx->bPayLenType;

        pParseInfoEx->cPayloads = (DWORD)(b&0x3f);
        if(pParseInfoEx->cPayloads == 0)
        {
            return WMAERR_FAIL;
        }

        pParseInfoEx->cbParseOffset++;
    }

    return WMAERR_OK;
}


/****************************************************************************/
WMAERR
WMA_ParsePayloadHeader(tWMAFileStateInternal *pInt)
{
    DWORD cbDummy;
    DWORD cbParseOffset;
    DWORD cbRepDataOffset;
    DWORD dwPayloadSize;
    PACKET_PARSE_INFO_EX *pParseInfoEx;
    PAYLOAD_MAP_ENTRY_EX *pPayload;
    DWORD cbLocalOffset=0;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;
//	BYTE bFrameRate = 0;
	WORD wTotalDataBytes=0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    pParseInfoEx = &pInt->ppex;
    pPayload = &pInt->payload;

//    cbWanted = 24;              /* at most */

    cbWanted = 2;              /* at least */
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset,
                                cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }

    cbParseOffset = 0;

    /* Loop in ParsePacketAndPayloads */

    pPayload->cbPacketOffset = (WORD)pParseInfoEx->cbParseOffset;
//    pPayload->bStreamId = pData[cbParseOffset];
    pPayload->bStreamId = (pData[cbParseOffset])&0x03; // Amit to get correct Streamid 
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
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset +2,
                                        cbWanted, &pData);
            pPayload->cbObjectOffset =  (DWORD)(*pData);
            break;
        }
    case 0x02:
        {
            WORD w;
            cbWanted =2;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset+2,
                                        cbWanted, &pData);
            GetUnalignedWord(pData, w);
            pPayload->cbObjectOffset =  (DWORD)(w);
        
            break;

        }
    case 0x03:
        {
            DWORD dw;
            cbWanted =4;
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->pCallbackContext,
                                        pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset +2,
                                        cbWanted, &pData);
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
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->pCallbackContext,
                                pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset,
                                cbWanted, &pData);

//    pPayload->cbRepData = pData[cbRepDataOffset];
    pPayload->cbRepData = pData[cbLocalOffset];

///////////////////////////////////////////////////////////////////////////////////////
    pPayload->msObjectPres = 0xffffffff;

    if(pPayload->cbRepData == 1)
    {
        pPayload->msObjectPres = pPayload->cbObjectOffset;
        pPayload->cbObjectOffset = 0;
        pPayload->cbObjectSize = 0;
        pPayload->bIsCompressedPayload =1;
///////////////////////////////////////////////////////////////////////////////////////

        cbWanted =3;
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset +1,
                                cbWanted, &pData);

/********************Amit to take care of compressed payload**************************/
//       if(pData[cbRepDataOffset +1] >0)
       if(pData[cbLocalOffset] >0)
//			bFrameRate = 1000/pData[cbRepDataOffset +1];
		

///////////////////////////////////////////////////////////////////////////////////////
		if( pParseInfoEx->fMultiPayloads)
		{
			//GetUnalignedWord( &pData[ cbRepDataOffset + 2 ],wTotalDataBytes ); // changed by amit
            GetUnalignedWord( &pData[ cbLocalOffset + 1 ],wTotalDataBytes );
		}
		else
		{
			wTotalDataBytes = 0; //pData[ cbRepDataOffset + 2 ];
		}
/********************End of Amit to take care of compressed payload**************************/
    }
    else if(pPayload->cbRepData >= 8)
    {
///////////////////////////////////////////////////////////////////////////////////////

        cbWanted =8;
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->pCallbackContext,
                                pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset + cbRepDataOffset +1,
                                cbWanted, &pData);
        GetUnalignedDword(&pData[cbLocalOffset],
                          pPayload->cbObjectSize);
        GetUnalignedDword(&pData[cbLocalOffset+ 4],
                          pPayload->msObjectPres);

        
/*        GetUnalignedDword(&pData[cbRepDataOffset + 1],
                          pPayload->cbObjectSize);
        GetUnalignedDword(&pData[cbRepDataOffset + 1 + 4],
                          pPayload->msObjectPres);
*/
//////////////////////////////////////////////////////////////////////////////////////////        
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
                cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                            pInt->pCallbackContext,
                                            pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset +pPayload->cbTotalSize,
                                            cbWanted, &pData);
                dwPayloadSize =  (DWORD)(*pData);
                break;
            }
        case 0x02:
            {
                WORD w;
                cbWanted =2;
                cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                            pInt->pCallbackContext,
                                            pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset+pPayload->cbTotalSize,
                                            cbWanted, &pData);
                GetUnalignedWord(pData, w);
                dwPayloadSize =  (DWORD)(w);
        
                break;

            }
        case 0x03:
            {
                DWORD dw;
                cbWanted =4;
                cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                            pInt->pCallbackContext,
                                            pInt->hdr_parse.currPacketOffset + pParseInfoEx->cbParseOffset + pPayload->cbTotalSize,
                                            cbWanted, &pData);
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
        dwPayloadSize = pInt->hdr_parse.cbPacketSize
                      - pParseInfoEx->cbParseOffset
                      - pPayload->cbTotalSize
                      - pParseInfoEx->cbPadding;
    }
    if (0 == wTotalDataBytes)
        wTotalDataBytes = (WORD) dwPayloadSize;

    pPayload->cbPayloadSize = (WORD)dwPayloadSize;

    pPayload->cbTotalSize += pParseInfoEx->bPayBytes
                          + (WORD)pPayload->cbPayloadSize;

 	pPayload->wTotalDataBytes = wTotalDataBytes; // Amit

/*    if( 1 == pPayload->cbRepData )
    {
        pPayload->cbPayloadSize--;
    }
*/
    pParseInfoEx->cbParseOffset += pPayload->cbTotalSize;

    if(pParseInfoEx->cbParseOffset > pInt->hdr_parse.cbPacketSize
       || (pParseInfoEx->cbParseOffset == pInt->hdr_parse.cbPacketSize
           && pInt->iPayload < pParseInfoEx->cPayloads - 1))
    {
        return WMAERR_CORRUPTDATA;
    }

    return WMAERR_OK;
}

//
//  Routines for byte-swapping the members of various structs
//

#ifdef BIG_ENDIAN

void ByteSwapWaveFormatEx( WAVEFORMATEX& dst )
{
	// swap the struct elements
	SWAP_WORD( dst.wFormatTag );
	SWAP_WORD( dst.nChannels );
	SWAP_DWORD( dst.nSamplesPerSec );
	SWAP_DWORD( dst.nAvgBytesPerSec );
	SWAP_WORD( dst.nBlockAlign );
	SWAP_WORD( dst.wBitsPerSample );
	SWAP_WORD( dst.cbSize );
}

void ByteSwapAsfXAcmAudioErrorMaskingData( AsfXAcmAudioErrorMaskingData& dst )
{
	// swap the struct elements
	SWAP_WORD( dst.virtualPacketLen );
	SWAP_WORD( dst.virtualChunkLen );
	SWAP_WORD( dst.silenceLen );
}

#endif
