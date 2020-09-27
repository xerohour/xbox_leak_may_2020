/*
 * Windows Media Audio (WMA) Decoder API (implementation)
 *
 * Copyright (c) Microsoft Corporation 1999.  All Rights Reserved.
 */

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#include <stdio.h>
#include "malloc.h"

#if defined _WIN32
#include <windows.h>
#define _BOOL_DEFINED
#endif
#pragma warning( disable : 4005 )
#if ( defined(BOOL) || defined(_WIN32_WCE) ) && !defined(_BOOL_DEFINED)
#define _BOOL_DEFINED
#endif
#include "wmaudio.h"
#include "loadstuff.h"
#include "wmaudio_type.h"
#include "wmaguids.h"
//#include "msaudiodec.h"
#include "AutoProfile.h"
#include "..\decoder\wmadecS_api.h"
#include "..\..\..\dsound\dsndver.h"


#ifndef MIN
#define MIN(x, y)  ((x) < (y) ? (x) : (y))
#endif /* MIN */

//extern int iSubFrm;

#define WMAAPI_DEMO_LIMIT 60
//static const WMARESULT WMA_E_ONHOLD         = 0x80040004; 



//BYTE bGlobalDataBuffer[256];
#if 0
/****************************************************************************/
extern "C" size_t
strlen(const char *p)
{
    int cnt = 0;
    while(*p++)
    {
        cnt++;
    }
    return cnt;
}
#endif


/****************************************************************************/
/*
static tWMAFileStatus
TranslateCode(tWMAStatus rc)
{
    switch(rc)
    {
    case kWMA_NoErr:
        return cWMA_NoErr;
    case kWMA_BrokenFrame:
        return cWMA_BrokenFrame;
    case kWMA_NoMoreFrames:
        return cWMA_NoMoreFrames;
    case kWMA_BadSamplingRate:
        return cWMA_BadSamplingRate;
    case kWMA_BadNumberOfChannels:
        return cWMA_BadNumberOfChannels;
    case kWMA_BadVersionNumber:
        return cWMA_BadVersionNumber;
    case kWMA_BadWeightingMode:
        return cWMA_BadWeightingMode;
    case kWMA_BadPacketisation:
        return cWMA_BadPacketization;
    }

#ifdef LOCAL_DEBUG
    while(1);
#else
    return cWMA_Internal;
#endif
}
*/

/****************************************************************************/
static tWMAFileStatus WMAF_UpdateNewPayload (tWMAFileStateInternal *pInt)
{
    /* internal call, so pInt should be okay */

    /* HongCho: separated out these parts from the parsing loop, because
     *          other parts (when a new payload is about to be retrieved)
     *          need to update the offsets and other states.
     *
     *          used in WMAFileDecodeData() and WMARawDecCBGetData ().
     */

    WMAERR wmarc;
#ifndef WMAAPI_NO_DRM
    tWMA_U32 cbRead;
    HRESULT hr;
    BYTE *pLast15;
#endif
#ifndef WMAAPI_NO_DRM_STREAM
    tWMA_U32 cbLeftover;
    BYTE *pbDRMPayload;
    WORD wDRMOffset;
#endif

    do
    {
        switch (pInt->parse_state)
        {
        case csWMA_NewAsfPacket:

#ifdef WMAAPI_DEMO
            if (pInt->nSampleCount >= WMAAPI_DEMO_LIMIT*pInt->hdr_parse.nSamplesPerSec)
            {
                return cWMA_DemoExpired;
            }
#endif /* WMAAPI_DEMO */

            if (pInt->hdr_parse.nextPacketOffset > pInt->hdr_parse.cbLastPacketOffset)
            {
                return cWMA_NoMoreFrames;
            }

            pInt->hdr_parse.currPacketOffset = pInt->hdr_parse.nextPacketOffset;
            pInt->hdr_parse.nextPacketOffset += pInt->hdr_parse.cbPacketSize;

            wmarc = WMA_ParsePacketHeader (pInt);

			if (wmarc == WMAERR_BUFFERTOOSMALL)
			{
				pInt->hdr_parse.nextPacketOffset = pInt->hdr_parse.currPacketOffset;
			//	pInt->hdr_parse.currPacketOffset -= pInt->hdr_parse.cbPacketSize;
				return cWMA_NoMoreDataThisTime;
			}


            if(wmarc != WMAERR_OK)
            {
                return cWMA_BadPacketHeader;
            }

            if (pInt->ppex.fEccPresent && pInt->ppex.fParityPacket)
            {
                /* HongCho: for some reason, ARM's code thinks a parity packet is
                 *          only at the end...  Here, I am not assuming that.
                 */
                break;
            }

            pInt->parse_state = csWMA_DecodePayloadStart;
            pInt->iPayload = 0;
            break;

        case csWMA_DecodePayloadStart:

            if (pInt->iPayload >= pInt->ppex.cPayloads)
            {
                pInt->parse_state = csWMA_NewAsfPacket;
                break;
            }

            wmarc = WMA_ParsePayloadHeader(pInt);
            if(wmarc != WMAERR_OK)
            {
                pInt->parse_state = csWMA_DecodePayloadEnd;
                break;
            }

            pInt->wPayStart = pInt->payload.cbPacketOffset + pInt->payload.cbTotalSize
                            - pInt->payload.cbPayloadSize;

#ifndef WMAAPI_NO_DRM_STREAM
            if( pInt->payload.bStreamId == pInt->hdr_parse.bDRMAuxStreamNum )
            {
                /* Read this payload's data (should be 7-15 bytes) */
                cbRead = WMAFileCBGetData(
                                    (tHWMAFileState *) pInt,
                                    ((tHWMAFileState *)pInt)->pCallbackContext,                                    
                                    pInt->hdr_parse.currPacketOffset + pInt->wPayStart,
                                    pInt->payload.cbPayloadSize,
                                    &pbDRMPayload );
                
                if (cbRead != pInt->payload.cbPayloadSize)
                    return (cWMA_NoMoreFrames);

                if (cbRead > sizeof (pInt->rgbNextPayloadEnd))
                    cbRead = sizeof (pInt->rgbNextPayloadEnd);

                cbLeftover = sizeof (pInt->rgbNextPayloadEnd) - cbRead;

                /* Copy this payload's data into our internal state data */
                pInt->cbNextPayloadEndValid = (BYTE) cbRead - 1;

                memset( pInt->rgbNextPayloadEnd, 0, 15 );
                memcpy( pInt->rgbNextPayloadEnd + 7, pbDRMPayload+1, cbRead-1 );

                /* Move on to the next payload, which should be the corresponding audio data */
                pInt->parse_state = csWMA_DecodePayloadEnd;
                break;
            }
#endif  /* WMAAPI_NO_DRM_STREAM */

            if( pInt->payload.cbRepData != 1 )
            {
                pInt->parse_state = csWMA_DecodePayload;
                pInt->payload.bIsCompressedPayload =0; // Amit to take care of compressed payloads.
                break;
            }
            else if(pInt->payload.cbRepData == 1)    // Amit to take care of compressed payloads.
            {
                pInt->parse_state = csWMA_DecodeCompressedPayload;
                pInt->payload.bIsCompressedPayload =1;
                break;
            }
            /* a payload has to be a multiple of audio "packets" */

            if(pInt->payload.cbPayloadSize%pInt->hdr_parse.nBlockAlign != 0)
                return cWMA_BrokenFrame;

            pInt->parse_state = csWMA_DecodePayloadEnd;
            break;

        case csWMA_DecodePayload:
            if (pInt->payload.bStreamId !=pInt->hdr_parse.wAudioStreamId) // Added by Amit to skip Video Payload
            {
                 pInt->parse_state = csWMA_DecodePayloadEnd;
                break;
           
            }
            pInt->cbPayloadOffset = pInt->hdr_parse.currPacketOffset + pInt->wPayStart;
            pInt->bBlockStart     = TRUE;
            pInt->cbBlockLeft     = pInt->hdr_parse.nBlockAlign;
            pInt->cbPayloadLeft   = pInt->payload.cbPayloadSize - pInt->cbBlockLeft;

            /* new payload, so take care of DRM */

            if(pInt->bHasDRM)
            {

#ifdef WMAAPI_NO_DRM

                return cWMA_DRMUnsupported;

#else  /* WMAAPI_NO_DRM */

#ifndef WMAAPI_NO_DRM_STREAM

                if( 0 != pInt->cbNextPayloadEndValid )
                {
                    /* We pre-cached the last bytes of this payload - no need to seek / read */
                    pLast15 = pInt->rgbNextPayloadEnd;

                    /* Move the bytes to the appropriate offset */
                    wDRMOffset = pInt->payload.cbPayloadSize % 8;

                    if( ( 0 != wDRMOffset ) && ( 8 == pInt->cbNextPayloadEndValid ) )
                    {
                        memmove( pLast15 + 7 - wDRMOffset, pLast15 + 7, 8 );
                        memset( pLast15 + 15 - wDRMOffset, 0, wDRMOffset );
                    }
                }
                else
                {

#endif  /* WMAAPI_NO_DRM_STREAM */

                    /* We need to seek & read the last data from the end of this payload */
                    cbRead = WMAFileCBGetData(
                                    (tHWMAFileState *)pInt,
                                    ((tHWMAFileState *)pInt)->pCallbackContext,
                                    pInt->cbPayloadOffset + pInt->payload.cbPayloadSize - 15,
                                    15,
                                    &pLast15 );

                    if (cbRead != 15)
                        return (cWMA_NoMoreFrames);

#ifndef WMAAPI_NO_DRM_STREAM
                }
#endif  /* WMAAPI_NO_DRM_STREAM */

                /* Initialize DRM so it can decrypt this payload properly */

                hr = CDrmPD_InitPacket(pInt->pDRM_state, pLast15, pInt->payload.cbPayloadSize);
                if(hr != S_OK)
                {
#ifdef LOCAL_DEBUG
                    SerialPrintf("++ WMA_UpdateNewPayload: CDrmPD_InitPacket failed (0x%08x).\n\r", hr);
#endif /* LOCAL_DEBUG */
                    return cWMA_DRMFailed;
                }

#ifndef WMAAPI_NO_DRM_STREAM
                pInt->cbNextPayloadEndValid = 0;
#endif  /* WMAAPI_NO_DRM_STREAM */

#endif /* WMAAPI_NO_DRM */

            }

            /* Done updating */
/*            if (pInt->bDecInWaitState == 1)
			{
				pInt->parse_state = csWMA_DecodeLoopStart;
				pInt->bDecInWaitState =0;
			
			}
			else */
			{
				pInt->parse_state = csWMA_DecodePayloadHeader;
			}
            return cWMA_NoErr;

        case csWMA_DecodePayloadEnd:
            pInt->iPayload++;
            pInt->parse_state = csWMA_DecodePayloadStart;
            break;

        case csWMA_DecodeCompressedPayload: // Added by Amit to take care of compressed payloads
            if (pInt->payload.bStreamId !=pInt->hdr_parse.wAudioStreamId) // Added by Amit to skip Video Payload
            {
                 pInt->parse_state = csWMA_DecodePayloadEnd;
                break;           
            }
            pInt->cbPayloadOffset = pInt->hdr_parse.currPacketOffset + pInt->wPayStart;
            pInt->bBlockStart     = TRUE;
            pInt->cbBlockLeft     = pInt->hdr_parse.nBlockAlign;
            pInt->payload.wBytesRead =0;
            pInt->payload.bSubPayloadState = 1;

/****************************************************************************************/    
            
            /* new payload, so take care of DRM */

            if(pInt->bHasDRM)
            {

#ifdef WMAAPI_NO_DRM

                return cWMA_DRMUnsupported;

#else  /* WMAAPI_NO_DRM */

#ifndef WMAAPI_NO_DRM_STREAM

                if( 0 != pInt->cbNextPayloadEndValid )
                {
                    /* We pre-cached the last bytes of this payload - no need to seek / read */
                    pLast15 = pInt->rgbNextPayloadEnd;

                    /* Move the bytes to the appropriate offset */
                    wDRMOffset = pInt->payload.cbPayloadSize % 8;

                    if( ( 0 != wDRMOffset ) && ( 8 == pInt->cbNextPayloadEndValid ) )
                    {
                        memmove( pLast15 + 7 - wDRMOffset, pLast15 + 7, 8 );
                        memset( pLast15 + 15 - wDRMOffset, 0, wDRMOffset );
                    }
                }
                else
                {

#endif  /* WMAAPI_NO_DRM_STREAM */

                    /* We need to seek & read the last data from the end of this payload */
                    cbRead = WMAFileCBGetData(
                                    (tHWMAFileState *)pInt,
                                    ((tHWMAFileState *)pInt)->pCallbackContext,
                                    pInt->cbPayloadOffset + pInt->payload.cbPayloadSize - 15,
                                    15,
                                    &pLast15 );

                    if (cbRead != 15)
                        return (cWMA_NoMoreFrames);

#ifndef WMAAPI_NO_DRM_STREAM
                }
#endif  /* WMAAPI_NO_DRM_STREAM */

                /* Initialize DRM so it can decrypt this payload properly */

                hr = CDrmPD_InitPacket(pInt->pDRM_state, pLast15, pInt->payload.cbPayloadSize);
                if(hr != S_OK)
                {
#ifdef LOCAL_DEBUG
                    SerialPrintf("++ WMA_UpdateNewPayload: CDrmPD_InitPacket failed (0x%08x).\n\r", hr);
#endif /* LOCAL_DEBUG */
                    return cWMA_DRMFailed;
                }

#ifndef WMAAPI_NO_DRM_STREAM
                pInt->cbNextPayloadEndValid = 0;
#endif  /* WMAAPI_NO_DRM_STREAM */

#endif /* WMAAPI_NO_DRM */

            }

        
            
            
            
            
            
            
            
            
            
            
            
            
            
            
/*******************************************************************************************/            
            pInt->parse_state = csWMA_DecodePayloadHeader;
            
            return cWMA_NoErr;
            break;

        default:
#ifdef LOCAL_DEBUG
            while(1);
#else  /* LOCAL_DEBUG */
            return cWMA_Internal;
#endif /* LOCAL_DEBUG */
        }

    } while(1);

    return cWMA_NoErr;
}

#if 0
/****************************************************************************/
WMARESULT WMARawDecCBGetData (U8_WMARawDec **ppBuffer, U32_WMARawDec *pcbBuffer, U32_WMARawDec dwUserData)
{
    tWMAFileStateInternal *pInt = (tWMAFileStateInternal *) dwUserData;
    tWMA_U32 num_bytes = WMA_MAX_DATA_REQUESTED;
    tWMA_U32 cbActual =0;
    tWMA_U32 cbWanted =0;
    BYTE *pbuff = NULL;
    tWMAFileStatus rc;
    tWMAParseState parse_state;

#ifndef WMAAPI_NO_DRM
    HRESULT hr;
#endif // WMAAPI_NO_DRM

    if(pInt == NULL || ppBuffer == NULL || pcbBuffer == NULL)
    {
        if(ppBuffer != NULL)
        {
            *ppBuffer = NULL;
        }
        if(pcbBuffer != NULL)
        {
            *pcbBuffer = 0;
        }

        return WMA_E_INVALIDARG;
    }

    *ppBuffer = NULL;
    *pcbBuffer = 0;

    /* If we used up the current payload, try to get the
     * next one.
     */
    
    // Added by Amit to take care of compressed payloads
    if (pInt->payload.bIsCompressedPayload == 1)
    {
        do
        {
            switch (pInt->payload.bSubPayloadState)
            {
            case 1: // Compressed payload just started
                cbWanted = 1; //to read subpayload length
                cbActual = WMAFileCBGetData ((tHWMAFileState *)pInt,
                                             ((tHWMAFileState *)pInt)->pCallbackContext,
                                             pInt->cbPayloadOffset, cbWanted, &pbuff);
                pInt->cbPayloadOffset += cbWanted;
                pInt->bBlockStart = TRUE;
                pInt->cbBlockLeft = pInt->hdr_parse.nBlockAlign;

                pInt->payload.bNextSubPayloadSize = pbuff[0];
                pInt->payload.wSubpayloadLeft = pInt->payload.bNextSubPayloadSize;
                if (pInt->payload.wSubpayloadLeft > 0)
                    pInt->payload.wSubpayloadLeft -= (WORD)pInt->cbBlockLeft;

			    if( pInt->payload.wTotalDataBytes > pInt->payload.bNextSubPayloadSize)
				    pInt->payload.wBytesRead = pInt->payload.bNextSubPayloadSize+1;
			    else if ( pInt->payload.wTotalDataBytes == pInt->payload.bNextSubPayloadSize)
				    pInt->payload.wBytesRead = pInt->payload.bNextSubPayloadSize;

                pInt->payload.bSubPayloadState = 2;
                break;
            case 2: // Subpayload started
                if (pInt->cbBlockLeft == 0 && pInt->payload.wSubpayloadLeft == 0)
                {
                    pInt->payload.bSubPayloadState =3;
                    break;
                }
                else
                {
                    if(pInt->cbBlockLeft == 0)
                    {
                        if (/*pInt->cbPayloadLeft*/pInt->payload.wSubpayloadLeft == 0) /* done with the file */
                            return WMA_S_NEWPACKET;

                        if (pInt->payload.wSubpayloadLeft > 0)
                            pInt->payload.wSubpayloadLeft -= (WORD) pInt->hdr_parse.nBlockAlign;
                        pInt->bBlockStart = TRUE;
                        pInt->cbBlockLeft = pInt->hdr_parse.nBlockAlign;
                    }
                    if(num_bytes > pInt->cbBlockLeft)
                        num_bytes = pInt->cbBlockLeft;

                    *pcbBuffer = (unsigned int)WMAFileCBGetData((tHWMAFileState *)pInt,
                                                                ((tHWMAFileState *)pInt)->pCallbackContext,
                                                                pInt->cbPayloadOffset, num_bytes, ppBuffer);
                    pInt->cbPayloadOffset+=*pcbBuffer;
                    //pInt->payload.wSubpayloadLeft -= *pcbBuffer;
                    pInt->cbBlockLeft     -= *pcbBuffer;
                
/****************************************************************************************/
        if (pInt->bHasDRM)
        {

#ifdef WMAAPI_NO_DRM
            return WMA_S_NEWPACKET;
#else  /* WMAAPI_NO_DRM */

            hr = CDrmPD_Decrypt (pInt->pDRM_state, *ppBuffer, *pcbBuffer);
            if (hr != S_OK) {
#ifdef LOCAL_DEBUG
                SerialPrintf("++ WMARawDecCBGetData: CDrmPD_Decrypt failed (0x%08x).\n\r", hr);
#endif /* LOCAL_DEBUG */
                *ppBuffer = NULL;
                *pcbBuffer = 0;
                return WMA_S_NEWPACKET;
            }

#endif /* WMAAPI_NO_DRM */

        }



/****************************************************************************************/
                    if (pInt->bBlockStart) {
                        pInt->bBlockStart = FALSE;
                        return WMA_S_NEWPACKET;
                    }


                    return WMA_OK;
                }

                break;
            case 3: // Subpayload finished
                if ( pInt->payload.wTotalDataBytes > pInt->payload.wBytesRead)
                { // there are payloads to decode
                    cbWanted = 1; //to read subpayload length
                    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                                ((tHWMAFileState *)pInt)->pCallbackContext,
                                                pInt->cbPayloadOffset, cbWanted, &pbuff);

                    pInt->cbPayloadOffset+=cbWanted;
                    pInt->bBlockStart     = TRUE;
                    pInt->cbBlockLeft     = pInt->hdr_parse.nBlockAlign;

                    
                    pInt->payload.bNextSubPayloadSize = pbuff[0];
                    pInt->payload.wSubpayloadLeft = pInt->payload.bNextSubPayloadSize;
                    if (pInt->payload.wSubpayloadLeft > 0)
                        pInt->payload.wSubpayloadLeft -= (WORD)pInt->cbBlockLeft;
					pInt->payload.wBytesRead+=pInt->payload.bNextSubPayloadSize+1;
                    pInt->payload.bSubPayloadState =2;                
                }
                else
                    pInt->payload.bSubPayloadState =4; // all subpayloads finished
                break;

            case 4: // All Subpayloads finished

                parse_state = pInt->parse_state;
                pInt->payload.bSubPayloadState =0;
                pInt->cbPayloadLeft =0;
                pInt->payload.bIsCompressedPayload =0;

                pInt->parse_state = csWMA_DecodePayloadEnd;
                rc = WMAF_UpdateNewPayload(pInt);
                pInt->parse_state = parse_state;  //restore 
                if((rc != cWMA_NoErr)) 
                    return WMA_S_NEWPACKET;
                break;
            }
        } while(1);


    }          
    else
    {
        if(pInt->cbBlockLeft == 0 && pInt->cbPayloadLeft == 0)
        {
            tWMAFileStatus rc;
            tWMAParseState parse_state;

            parse_state = pInt->parse_state;

            pInt->parse_state = csWMA_DecodePayloadEnd;
            rc = WMAF_UpdateNewPayload(pInt);
            pInt->parse_state = parse_state; /* restore */

            if(rc != cWMA_NoErr)
                return WMA_S_NEWPACKET;
        }

        /* return as much as we currently have left */

        if(pInt->cbBlockLeft == 0)
        {
            if(pInt->cbPayloadLeft == 0)
            {
                /* done with the file */
                return WMA_S_NEWPACKET;
            }

            pInt->cbPayloadLeft -= pInt->hdr_parse.nBlockAlign;
            pInt->bBlockStart = TRUE;
            pInt->cbBlockLeft = pInt->hdr_parse.nBlockAlign;
        }
        if(num_bytes > pInt->cbBlockLeft)
            num_bytes = pInt->cbBlockLeft;

        *pcbBuffer = (unsigned int)WMAFileCBGetData((tHWMAFileState *)pInt,
                                                    ((tHWMAFileState *)pInt)->pCallbackContext,
                                                    pInt->cbPayloadOffset, num_bytes, ppBuffer);
        pInt->cbPayloadOffset += *pcbBuffer;
        pInt->cbBlockLeft     -= *pcbBuffer;

        /* DRM decryption if necessary */

        if (pInt->bHasDRM)
        {

#ifdef WMAAPI_NO_DRM
            return WMA_S_NEWPACKET;
#else  /* WMAAPI_NO_DRM */

            hr = CDrmPD_Decrypt (pInt->pDRM_state, *ppBuffer, *pcbBuffer);
            if (hr != S_OK) {
#ifdef LOCAL_DEBUG
                SerialPrintf("++ WMARawDecCBGetData: CDrmPD_Decrypt failed (0x%08x).\n\r", hr);
#endif /* LOCAL_DEBUG */
                *ppBuffer = NULL;
                *pcbBuffer = 0;
                return WMA_S_NEWPACKET;
            }

#endif /* WMAAPI_NO_DRM */

        }

        if (pInt->bBlockStart) {
            pInt->bBlockStart = FALSE;
            return WMA_S_NEWPACKET;
        }

        return WMA_OK;
    }
    return WMA_OK;
}

#endif
/* ===========================================================================
 * WMAFileIsWMA
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileIsWMA (tWMAFileHdrState *pstate)
{
    tWMAFileHdrStateInternal *pInt = (tWMAFileHdrStateInternal *)pstate;
    WMAERR wmarc;

    if(sizeof(tWMAFileHdrState) != sizeof(tWMAFileHdrStateInternal))
    {
        /* this should NOT happen */
        WMADebugMessage("** Internal Error: sizeof(tWMAFileHdrStateInternal) = %d.\n\r",
                sizeof(tWMAFileHdrStateInternal));
        // while(1);
        return cWMA_BadArgument;
    }
    if(pInt == NULL)
        return cWMA_BadArgument;

    /* parse ASF header */

    wmarc = WMA_ParseAsfHeader(pInt, 0);
    if(wmarc != WMAERR_OK)
        return cWMA_BadAsfHeader;

    return cWMA_NoErr;
}

/* ===========================================================================
 * WMAGetLicenseStore
--------------------------------------------------------------------------- */
BYTE * WMAGetLicenseStore (tWMAFileHdrState *pstate,tWMA_U32 *pLen)
{
    tWMAFileHdrStateInternal *pInt = (tWMAFileHdrStateInternal *)pstate;

    if(sizeof(tWMAFileHdrState) != sizeof(tWMAFileHdrStateInternal))
    {
        /* this should NOT happen */
        WMADebugMessage("** Internal Error: sizeof(tWMAFileHdrStateInternal) = %d.\n\r",
                sizeof(tWMAFileHdrStateInternal));
        // while(1);
        return NULL;
    }
    if(pInt == NULL)
        return NULL;

    *pLen = pInt->m_dwLicenseLen;

    return pInt->m_pLicData;
}

/* ===========================================================================
 * WMAFileDecodeClose
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileDecodeClose (tHWMAFileState* phstate)
{
    tWMAFileStateInternal *pInt;
    unsigned int i;

    pInt = (tWMAFileStateInternal*) (*phstate);

    if (pInt != NULL ) {
        WMARawDecClose (&(pInt->hWMA));
#ifndef WMAAPI_NO_DRM
		if (pInt->pDRM_state != NULL) {
			free (pInt->pDRM_state);
            pInt->pDRM_state = NULL;
        }
#endif
#if 0
        if (pInt->hdr_parse.m_pMarkers) {
            for (i=0;i<pInt->hdr_parse.m_dwMarkerNum;i++)
                if ( pInt->hdr_parse.m_pMarkers[i].m_pwDescName != NULL )
                {
                    free (pInt->hdr_parse.m_pMarkers[i].m_pwDescName);
                    pInt->hdr_parse.m_pMarkers[i].m_pwDescName = NULL;
                }
            free (pInt->hdr_parse.m_pMarkers);
            pInt->hdr_parse.m_pMarkers = NULL;
        }
#endif 
        if (pInt->hdr_parse.m_pLicData) 
        {
    		free (pInt->hdr_parse.m_pLicData);
            pInt->hdr_parse.m_pLicData = NULL;
        }

        if(pInt->hdr_parse.m_pECDesc != NULL) {
            if(pInt->hdr_parse.m_pECDesc->cDescriptors > 0) {
                for (i = 0; i < (unsigned int) pInt->hdr_parse.m_pECDesc->cDescriptors; i++) {
                    free(pInt->hdr_parse.m_pECDesc->pDescriptors[i].uValue.pbBinary);
                    free(pInt->hdr_parse.m_pECDesc->pDescriptors[i].pwszName);
                    pInt->hdr_parse.m_pECDesc->pDescriptors[i].uValue.pbBinary = NULL;
                    pInt->hdr_parse.m_pECDesc->pDescriptors[i].pwszName = NULL;
                }
                free(pInt->hdr_parse.m_pECDesc->pDescriptors);
                pInt->hdr_parse.m_pECDesc->pDescriptors = NULL;
            }
            free(pInt->hdr_parse.m_pECDesc);
            pInt->hdr_parse.m_pECDesc = NULL;
        }

		free (pInt);
        *phstate = NULL;
    }
    phstate = NULL;
    return cWMA_NoErr;
}

/* ===========================================================================
 * WMAFileDecodeInit
--------------------------------------------------------------------------- */
BYTE pDrmBuffer[4096];
tWMAFileStatus WMAFileDecodeCreate (tHWMAFileState *phstate, void * pCallbackContext) // georgioc added pContext
{
    tWMAFileStateInternal *pInt;

#ifdef USE_WOW_FILTER
    WowControls         tempWowControls;
#endif

    // first try to close in case someone calls us without prior close
    WMAFileDecodeClose (phstate);

    // Now start to allocate and initialize

    pInt = (tWMAFileStateInternal*) malloc (sizeof (tWMAFileStateInternal));

    if (pInt == NULL)
        return cWMA_Failed;

    memset (pInt, 0, sizeof (tWMAFileStateInternal));

    /* initialize the some state */

    pInt->parse_state = csWMA_HeaderStart;
    // georgioc
    pInt->pCallbackContext = pCallbackContext;
    pInt->hdr_parse.pCallbackContext = pCallbackContext;

    /* parse ASF header */

    *phstate = (tHWMAFileState) pInt;

    return cWMA_NoErr;
}

tWMAFileStatus WMAFileDecodeInit (tHWMAFileState hstate)
{
    tWMAFileStateInternal *pInt;
    WMAERR wmarc;
    WMARESULT wmar;
#ifndef WMAAPI_NO_DRM
    HRESULT hr;
#endif // WMAAPI_NO_DRM

#ifdef USE_WOW_FILTER
    WowControls         tempWowControls;
#endif

    pInt = (tWMAFileStateInternal*) hstate;

    wmarc = WMA_ParseAsfHeader(&pInt->hdr_parse, 1);
    if(wmarc != WMAERR_OK)
        return cWMA_BadAsfHeader;

    /* Set up and initialize the WMA bitstreamd decoder */

    wmar = WMARawDecInit (
        &pInt->hWMA,
        (U32_WMARawDec) pInt,
        WMARawDecCBGetData,
        (U16_WMARawDec) pInt->hdr_parse.nVersion,
        (U16_WMARawDec) pInt->hdr_parse.nSamplesPerBlock,
        (U16_WMARawDec) pInt->hdr_parse.nSamplesPerSec,
        (U16_WMARawDec) pInt->hdr_parse.nChannels,
        (U16_WMARawDec) pInt->hdr_parse.nAvgBytesPerSec,
        (U16_WMARawDec) pInt->hdr_parse.nBlockAlign,
        (U16_WMARawDec) pInt->hdr_parse.nEncodeOpt,
        0
    );
 	if (wmar == 0x80040000)
        return cWMA_BadSamplingRate;

	if (wmar != WMA_OK)
        return cWMA_Failed;

    /* Set up the decryption if necessary */

    pInt->bHasDRM = (BOOL) 0;

    if(pInt->hdr_parse.cbSecretData > 0)
    {
        /* only for DRM now */
        char *p = (char *)pInt->hdr_parse.pbType;

        if(p[0] == 'D' && p[1] == 'R' && p[2] == 'M' && p[3] == '\0')
        {
            pInt->bHasDRM = (BOOL)( 1 );

#ifndef WMAAPI_NO_DRM
            pInt->pDRM_state = (CDrmPD*) malloc (sizeof (CDrmPD));
            if (pInt->pDRM_state == NULL)
                return cWMA_DRMFailed;
#ifndef WMAAPI_NO_DRM
#ifdef GLOBAL_SECRET
            SetSecret_1of7 (pInt->pDRM_state);
#endif // GLOBAL_SECRET
#endif /* WMAAPI_NO_DRM */
            hr = CDrmPD_Init(pInt->pDRM_state,pDrmBuffer,2048);
            if(hr != S_OK)
                return cWMA_DRMFailed;
#endif /* WMAAPI_NO_DRM */
        }
        else
            return cWMA_BadDRMType;
    }

    pInt->hdr_parse.cbLastPacketOffset = pInt->hdr_parse.cbFirstPacketOffset;
    if (pInt->hdr_parse.cPackets > 0)
        pInt->hdr_parse.cbLastPacketOffset += (pInt->hdr_parse.cPackets - 1)*pInt->hdr_parse.cbPacketSize;

	pInt->hdr_parse.currPacketOffset = pInt->hdr_parse.cbHeader;// Added by amit
    pInt->hdr_parse.nextPacketOffset = pInt->hdr_parse.cbHeader;
    pInt->parse_state = csWMA_NewAsfPacket;



#ifdef USE_WOW_FILTER
    tempWowControls.bEnableWow = TRUE;
    tempWowControls.bMonoInput = FALSE;
    tempWowControls.bHighBitRate = TRUE;
    tempWowControls.iSampleRate = k44100;
    tempWowControls.mBrightness = kHigh;
    tempWowControls.mSpeakerSize = kMedium;
    tempWowControls.dTruBass = 0.8;
    tempWowControls.dWidthControl = 0.6;

    switch(pInt->hdr_parse.nSamplesPerSec)
    {
        case 22050:
                tempWowControls.iSampleRate = k22050;
                break;
        case 32000:
                tempWowControls.iSampleRate = k32000;
                break;
        case 44100:
                tempWowControls.iSampleRate = k44100;
                break;
        default:
                break;
    }

    if ( pInt->hdr_parse.nChannels  == 1)
    {
            tempWowControls.bMonoInput          = TRUE;
    }

	if ( pInt->hdr_parse.nAvgBytesPerSec * 8 < 64000)
		tempWowControls.bHighBitRate    = FALSE;

    tempWowControls.bEnableWow      = TRUE;

    if ( InitWow(&(pInt->sWowChannel), &tempWowControls ) != 0) {
        return cWMA_BadDRMType;   //NQF_temp
    }

#endif  //USE_WOW_FILTER

    return cWMA_NoErr;
}


/* ===========================================================================
 * WMAFileDecodeInfo
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileDecodeInfo(tHWMAFileState hstate,
                  tWMAFileHeader *hdr)
{
    tWMAFileStateInternal *pInt; 
    pInt = (tWMAFileStateInternal*) hstate;

    if(pInt == NULL || hdr == NULL)
        return cWMA_BadArgument;

    /* Fill in the structure */

    hdr->version      = (tWMAFileVersion)pInt->hdr_parse.nVersion;
    hdr->num_channels = (tWMAFileChannels)pInt->hdr_parse.nChannels;

    switch(pInt->hdr_parse.nSamplesPerSec)
    {
    case 8000:
        hdr->sample_rate = cWMA_SR_08kHz;
        break;
    case 11025:
        hdr->sample_rate = cWMA_SR_11_025kHz;
        break;
    case 16000:
        hdr->sample_rate = cWMA_SR_16kHz;
        break;
    case 22050:
        hdr->sample_rate = cWMA_SR_22_05kHz;
        break;
    case 32000:
        hdr->sample_rate = cWMA_SR_32kHz;
        break;
    case 44100:
        hdr->sample_rate = cWMA_SR_44_1kHz;
        break;
    case 48000:
        hdr->sample_rate = cWMA_SR_48kHz;
        break;
    default:
        return cWMA_BadSamplingRate;
    }

    hdr->duration            = pInt->hdr_parse.msDuration;
    hdr->packet_size         = pInt->hdr_parse.cbPacketSize;
    hdr->first_packet_offset = pInt->hdr_parse.cbFirstPacketOffset;
    hdr->last_packet_offset  = pInt->hdr_parse.cbLastPacketOffset;

    hdr->has_DRM             = (tWMA_U32)pInt->bHasDRM;

    hdr->LicenseLength       = (tWMA_U32)pInt->hdr_parse.m_dwLicenseLen;

    hdr->bitrate             = pInt->hdr_parse.nAvgBytesPerSec*8;

    return cWMA_NoErr;
}


/* ===========================================================================
 * WMAFileContentDesc
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileContentDesc (tHWMAFileState hstate, tWMAFileContDesc *desc)
{
    tWMAFileStateInternal *pInt; 
    DWORD cbOffset;
    DWORD cbWanted;
    DWORD cbActual;

    pInt = (tWMAFileStateInternal*) hstate;
    if(pInt == NULL || desc == NULL)
        return cWMA_BadArgument;

    if(pInt->hdr_parse.cbCDOffset == 0)
    {
        /* no content description object in the header */

        desc->title_len       = 0;
        desc->author_len      = 0;
        desc->copyright_len   = 0;
        desc->description_len = 0;
        desc->rating_len      = 0;

        return cWMA_NoErr;
    }

    /* Title */

    if(desc->title_len > 0)
    {
        if(desc->pTitle != NULL)
        {
            if(pInt->hdr_parse.cbCDTitle > 0)
            {
                cbOffset = pInt->hdr_parse.cbCDOffset;
                cbWanted = MIN(pInt->hdr_parse.cbCDTitle, desc->title_len);
                cbActual = WMA_GetBuffer(pInt, cbOffset, cbWanted, desc->pTitle);
                if(cbActual != cbWanted)
                    return cWMA_BadAsfHeader;
            }
            else
                desc->pTitle[0] = desc->pTitle[1] = 0;
        }

        desc->title_len = pInt->hdr_parse.cbCDTitle;
    }

    /* Author */

    if (desc->author_len > 0)
    {
        if (desc->pAuthor != NULL)
        {
            if (pInt->hdr_parse.cbCDAuthor > 0)
            {
                cbOffset = pInt->hdr_parse.cbCDOffset
                         + pInt->hdr_parse.cbCDTitle;
                cbWanted = MIN(pInt->hdr_parse.cbCDAuthor, desc->author_len);
                cbActual = WMA_GetBuffer(pInt, cbOffset, cbWanted, desc->pAuthor);
                if(cbActual != cbWanted)
                    return cWMA_BadAsfHeader;
            }
            else
                desc->pAuthor[0] = desc->pAuthor[1] = 0;
        }
        desc->author_len = pInt->hdr_parse.cbCDAuthor;
    }

    /* Copyright */

    if(desc->copyright_len > 0)
    {
        if(desc->pCopyright != NULL)
        {
            if(pInt->hdr_parse.cbCDCopyright > 0)
            {
                cbOffset = pInt->hdr_parse.cbCDOffset
                         + pInt->hdr_parse.cbCDTitle + pInt->hdr_parse.cbCDAuthor;
                cbWanted = MIN(pInt->hdr_parse.cbCDCopyright, desc->copyright_len);
                cbActual = WMA_GetBuffer(pInt, cbOffset, cbWanted, desc->pCopyright);
                if(cbActual != cbWanted)
                    return cWMA_BadAsfHeader;
            }
            else
                desc->pCopyright[0] = desc->pCopyright[1] = 0;
        }
        desc->copyright_len = pInt->hdr_parse.cbCDCopyright;
    }

    /* Description */

    if(desc->description_len > 0)
    {
        if(desc->pDescription != NULL)
        {
            if(pInt->hdr_parse.cbCDDescription > 0)
            {
                cbOffset = pInt->hdr_parse.cbCDOffset
                         + pInt->hdr_parse.cbCDTitle + pInt->hdr_parse.cbCDAuthor
                         + pInt->hdr_parse.cbCDCopyright;
//                cbWanted = MIN(pInt->hdr_parse.cbCDCopyright, desc->copyright_len);
                cbWanted = MIN(pInt->hdr_parse.cbCDDescription, desc->description_len);
                cbActual = WMA_GetBuffer(pInt, cbOffset, cbWanted, desc->pDescription);
                if(cbActual != cbWanted)
                    return cWMA_BadAsfHeader;
            }
            else
                desc->pDescription[0] = desc->pDescription[1] = 0;
        }
        desc->description_len = pInt->hdr_parse.cbCDDescription;
    }

    /* Rating */

    if(desc->rating_len > 0)
    {
        if(desc->pRating != NULL)
        {
            if(pInt->hdr_parse.cbCDRating > 0)
            {
                cbOffset = pInt->hdr_parse.cbCDOffset
                         + pInt->hdr_parse.cbCDTitle + pInt->hdr_parse.cbCDAuthor
                         + pInt->hdr_parse.cbCDCopyright + pInt->hdr_parse.cbCDDescription;
                cbWanted = MIN(pInt->hdr_parse.cbCDRating, desc->rating_len);
                cbActual = WMA_GetBuffer(pInt, cbOffset, cbWanted, desc->pRating);

                if(cbActual != cbWanted)
                    return cWMA_BadAsfHeader;
            }
            else
                desc->pRating[0] = desc->pRating[1] = 0;
        }
        desc->rating_len = pInt->hdr_parse.cbCDRating;
    }

    return cWMA_NoErr;
}

/* ===========================================================================
 * WMAFileExtendedContentDesc
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileExtendedContentDesc (tHWMAFileState hstate, const tWMAExtendedContentDesc **pECDesc)
{
    tWMAFileStateInternal *pInt;

    pInt = (tWMAFileStateInternal*) hstate;
    if(pInt == NULL)
        return cWMA_BadArgument;

    if(pInt->hdr_parse.m_pECDesc == NULL) {
        *pECDesc = NULL;
    }
    else {
        *pECDesc = pInt->hdr_parse.m_pECDesc;
    }

    return cWMA_NoErr;
}

/******************************************************************************/

/* ===========================================================================
 * WMAGetNumberOfMarkers
--------------------------------------------------------------------------- */
int WMAGetNumberOfMarkers(tWMAFileHdrState *state)
{
    tWMAFileHdrStateInternal *pInt = (tWMAFileHdrStateInternal *)state;

    if(sizeof(tWMAFileHdrState) != sizeof(tWMAFileHdrStateInternal))
    {
        /* this should NOT happen */
        WMADebugMessage("** Internal Error: sizeof(tWMAFileHdrStateInternal) = %d.\n\r",
                sizeof(tWMAFileHdrStateInternal));

        return 0;
    }

    if(pInt == NULL)
    {
        return 0;
    }

    if (WMAERR_OK == WMA_GetMarkerObject(pInt, -1)) // filled up pInt->m_dwMarkerNum 
    {
        return pInt->m_dwMarkerNum;
    } else {
        return 0;  
    }
}


/* ===========================================================================
 * WMAGetMarker
--------------------------------------------------------------------------- */
tWMAFileStatus  WMAGetMarker(tWMAFileHdrState *state, int iIndex, MarkerEntry *pEntry)  
{
    tWMAFileHdrStateInternal *pInt = (tWMAFileHdrStateInternal *)state;

    if(sizeof(tWMAFileHdrState) != sizeof(tWMAFileHdrStateInternal))
    {
        /* this should NOT happen */
        WMADebugMessage("** Internal Error: sizeof(tWMAFileHdrStateInternal) = %d.\n\r",
                sizeof(tWMAFileHdrStateInternal));

        return cWMA_Failed; 
    }

    if(pInt == NULL)
    {
       return cWMA_Failed; 
    }
 
    if (WMAERR_OK != WMA_GetMarkerObject(pInt, -1)) // filled up pInt->m_dwMarkerNum
    {
        return cWMA_Failed;  
    } 

    if ( pInt->m_dwMarkerNum == 0 ) 
    {
        return cWMA_Failed;  
    } 

    if ( pEntry == NULL ) 
    {
        return cWMA_Failed;  
    } 

    pInt->m_pMarker = pEntry;  

    if ( 0 <= iIndex < (int) pInt->m_dwMarkerNum )
    {
        if (WMAERR_OK == WMA_GetMarkerObject(pInt, iIndex))
        {
            return cWMA_NoErr; 
        } else {
            return cWMA_Failed;  
        }

    } else {

        return cWMA_Failed; 
        
    }
}


/* ===========================================================================
 * WMAFileLicenseInit
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileLicenseInit (tHWMAFileState hstate, tWMAFileLicParams *lic_params, tWMA_U16 CheckLicTypes)
{
    tWMAFileStateInternal *pInt; 
#ifndef WMAAPI_NO_DRM
   	tWMA_U16 i;
#endif // WMAAPI_NO_DRM
    pInt = (tWMAFileStateInternal*) hstate;

    if(pInt == NULL || lic_params == NULL)
        return cWMA_BadArgument;

    if(pInt->bHasDRM)
    {

#ifdef WMAAPI_NO_DRM

        return cWMA_DRMUnsupported;

#else  /* WMAAPI_NO_DRM */

        HRESULT hr;
        PMLICENSE pmlic;
        DWORD dwRight;

        BYTE *pData;
        DWORD dwOffset;
        DWORD dwWanted;
        DWORD dwActual;

        /* set up for InitDecrypt */

        memset (&pmlic, 0, sizeof(pmlic));
        memcpy (pmlic.ld.KID, (char *)pInt->hdr_parse.pbKeyID,
               strlen((const char *)pInt->hdr_parse.pbKeyID) + 1);
        memcpy (pmlic.ld.appSec, APPSEC_1000, APPSEC_LEN);

	    CheckLicTypes = CheckLicTypes & 3;
        for (i = 1; i < 3; i++) {
		    if (CheckLicTypes & i) { 
				if (i == CHECK_NONSDMI_LIC)
			        dwRight = DRM_RIGHT_NONSDMI;
			    else    
			        dwRight = DRM_RIGHT_SDMI;

                memcpy (pmlic.ld.rights, (BYTE *)&dwRight, RIGHTS_LEN);

                dwOffset = 0;
                dwActual = WMA_MAX_DATA_REQUESTED;

                do
                {
                    dwWanted = dwActual;

                    if (pInt->hdr_parse.m_dwLicenseLen) 
                    {
                        dwActual = ((dwOffset +dwWanted) > pInt->hdr_parse.m_dwLicenseLen) ? 0: dwWanted;
                        pData = pInt->hdr_parse.m_pLicData + dwOffset;
                    } else {
                        return cWMA_DRMFailed;    
                        //dwActual = WMAFileCBGetLicenseData((tWMAFileState *)pInt,
                        //                                   dwOffset, dwWanted, &pData);
                    }
    
                    if(dwActual != dwWanted)
                        return cWMA_DRMFailed;

                    hr = CDrmPD_InitDecrypt(pInt->pDRM_state,
                                            &pmlic,
                                            lic_params->pPMID,
                                            lic_params->cbPMID,
                                            pData,
                                            &dwOffset,
                                            &dwActual);

#ifdef LOCAL_DEBUG
                    SerialPrintf("++ WMAFileLicenseInit: CDrmPD_InitDecrypt returned 0x%08x: next [%u @ %u].\n\r",
                                 hr, dwActual, dwOffset);
#endif /* LOCAL_DEBUG */

                } while (hr == 0x80041006 || hr == E_DRM_MORE_DATA);

                if (hr == S_OK)
                    return cWMA_NoErr;
            }
        }
#endif /* WMAAPI_NO_DRM */
    }

    return cWMA_DRMFailed;
}


/* ===========================================================================
 * WMAFileSeek
--------------------------------------------------------------------------- */
tWMA_U32 WMAFileSeek (tHWMAFileState hstate, tWMA_U32 msSeek)
{
    tWMAFileStateInternal *pInt; 
    tWMA_U32 msActual;
    tWMA_U32 nPacket;
    WMARESULT wmar;

    pInt = (tWMAFileStateInternal*) hstate;
    if(pInt == NULL || pInt->hWMA == NULL )
    {
        return cWMA_BadArgument;
    }

    /* which packet? */

    nPacket = 0;
    if(pInt->hdr_parse.msDuration > 0)
    {
        /* rounding down because I want the requested time frame to be
         * played */

        nPacket = msSeek / (pInt->hdr_parse.msDuration / pInt->hdr_parse.cPackets);
		//NQF, 061800, temp fix for the seek() bug reported by Creative 
		if ((nPacket) &&( nPacket % 16 == 0 )){
			nPacket++;  
		}
    }

    /* see if it's within the bounds */

    if(nPacket < pInt->hdr_parse.cPackets)
    {
        /* parse the packet and the payload header
         *
         * a bit of a duplication from WMAF_UpdateNewPayload...
         */

        pInt->hdr_parse.currPacketOffset = pInt->hdr_parse.cbHeader
                                         + nPacket*pInt->hdr_parse.cbPacketSize;

        if(WMA_ParsePacketHeader(pInt) != WMAERR_OK)
        {
            msActual = pInt->hdr_parse.msDuration;
            goto lexit;
        }

        if(pInt->ppex.fEccPresent && pInt->ppex.fParityPacket)
        {
            msActual = pInt->hdr_parse.msDuration;
            goto lexit;
        }

        pInt->iPayload = 0;

        if(WMA_ParsePayloadHeader(pInt) != WMAERR_OK)
        {
            msActual = pInt->hdr_parse.msDuration;
            goto lexit;
        }

        if(pInt->payload.cbRepData == 1)
        {
            msActual = pInt->hdr_parse.msDuration;
            goto lexit;
        }

        /* whew... finally got here */

        msActual = pInt->payload.msObjectPres - pInt->hdr_parse.msPreroll;
    }
    else
    {
        nPacket = pInt->hdr_parse.cPackets;
        msActual = pInt->hdr_parse.msDuration;
    }

lexit:

    /* reset the states */

    wmar = WMARawDecReset (pInt->hWMA);
    if (wmar != WMA_OK)
        return cWMA_Failed;
    pInt->hdr_parse.nextPacketOffset = pInt->hdr_parse.cbHeader
                                     + nPacket*pInt->hdr_parse.cbPacketSize;
    pInt->parse_state = csWMA_NewAsfPacket;

    return msActual;
}


/* ===========================================================================
 * WMAFileDecodeData
--------------------------------------------------------------------------- */
tWMAFileStatus WMAFileDecodeData(tHWMAFileState hstate)
{
    tWMAFileStateInternal *pInt;
    WMARESULT wmar;
    tWMAFileStatus rc;
    FUNCTION_PROFILE(fp);

    pInt = (tWMAFileStateInternal*) hstate;

    if(pInt == NULL || pInt->hWMA == NULL)
        return cWMA_BadArgument;

	FUNCTION_PROFILE_START(&fp,WMA_FILE_DECODE_DATA_PROFILE);

    do
    {
        switch(pInt->parse_state)
        {
        case csWMA_DecodePayloadHeader:


#ifdef WMAAPI_DEMO
            if(pInt->nSampleCount >= WMAAPI_DEMO_LIMIT*pInt->hdr_parse.nSamplesPerSec)
            {
				FUNCTION_PROFILE_STOP(&fp);
                return cWMA_DemoExpired;
            }
#endif /* WMAAPI_DEMO */
/*
			pInt->parse_state = csWMA_DecodeLoopStart;
			break;
*/
			if (pInt->bDecInWaitState == 1)
			{
				pInt->parse_state = csWMA_DecodeLoopStart;
				break;
			}

			if (pInt->bAllFramesDone ==1 || pInt->bDecInWaitState == 2 || pInt->bFirst ==0)
            {
				if (pInt->bDecInWaitState == 2)
					pInt->bDecInWaitState =0;
				else if (pInt->bAllFramesDone == 1)
					pInt->bAllFramesDone =0;

				pInt->bFirst =1;
				

				wmar = WMARawDecStatus (pInt->hWMA);
				

				if (wmar == WMA_E_ONHOLD) 
				{
					
					if (pInt->hdr_parse.nextPacketOffset <= pInt->hdr_parse.cbLastPacketOffset)
					{
						wmar = WMA_OK;
						pInt->bDecInWaitState =2;
						return cWMA_NoErr;
					}
					else
						return cWMA_NoMoreFrames;

				}

				if (wmar == WMA_E_LOSTPACKET)
					wmar = WMA_OK;

				if (WMARAW_FAILED (wmar)) {
					FUNCTION_PROFILE_STOP (&fp);
					return cWMA_Failed;
				}
			}
            pInt->parse_state = csWMA_DecodeLoopStart;
            break;

        case csWMA_DecodeLoopStart:
            {
#ifdef WMAAPI_DEMO
                if(pInt->nSampleCount >= WMAAPI_DEMO_LIMIT*pInt->hdr_parse.nSamplesPerSec)
                {
					FUNCTION_PROFILE_STOP(&fp);
                    return cWMA_DemoExpired;
                }
#endif /* WMAAPI_DEMO */
			if (pInt->bDecInWaitState == 1)
			{
				pInt->parse_state = csWMA_DecodePayloadHeader;
				pInt->bDecInWaitState =0;
			}


                wmar = WMARawDecDecodeData (pInt->hWMA);
				if (wmar == 4)
				{
					pInt->bAllFramesDone = 1;
				}

				if (wmar == WMA_E_LOSTPACKET)
				{
					wmar = WMA_OK;
					pInt->bAllFramesDone = 1;
					pInt->parse_state = csWMA_DecodePayloadHeader;

				}

                if (wmar == WMA_E_BROKEN_FRAME) {
#ifdef LOCAL_DEBUG
                    SerialSendString("\r** WMAFileDecodeData: WMADecodeData: broken frame\n");
#endif /* LOCAL_DEBUG */
                    // reset is recommended 
                    // then go to the next payload 
                    WMARawDecReset (pInt->hWMA);
                    pInt->bAllFramesDone = 1;
                    pInt->parse_state = csWMA_DecodePayloadEnd;
                    break;
                }

                if (wmar == WMA_E_ONHOLD) 
				{
					if (pInt->hdr_parse.nextPacketOffset <= pInt->hdr_parse.cbLastPacketOffset)
					{
						wmar = WMA_OK;
						pInt->bDecInWaitState =1;
						return cWMA_NoErr;
					}
					else
						return cWMA_NoMoreFrames;
				}

                if (wmar != WMA_OK) {
                    if (wmar == WMA_S_NO_MORE_SRCDATA) {
#ifdef LOCAL_DEBUG
                        SerialSendString("\r** WMAFileDecodeData: WMADecodeData: no more data\n");
#endif /* LOCAL_DEBUG */
						FUNCTION_PROFILE_STOP(&fp);
                        return cWMA_NoMoreFrames;
                    }
                    
					if (WMA_E_LOSTPACKET == wmar)
						return cWMA_NoErr;
					
					if (WMARAW_FAILED (wmar)) // Wei-ge recommends resetting after any error
                        WMARawDecReset (pInt->hWMA);
                    pInt->parse_state = csWMA_DecodePayloadHeader;
					FUNCTION_PROFILE_STOP(&fp);
					return cWMA_NoErr;
                }
            }

            /* WMAGetPCM will be done separately in WMAFileGetPCM */
			FUNCTION_PROFILE_STOP(&fp);
            return cWMA_NoErr;

        default:
            /* All other state operation is done in this function
             *   so that it can be done somewhere else as well
             */

            {
                rc = WMAF_UpdateNewPayload(pInt);
                if(rc != cWMA_NoErr)
                {
#ifdef LOCAL_DEBUG
                    SerialSendString("\r** WMAFileDecodeData: WMA_UpdateNewPlayload failed\n");
#endif /* LOCAL_DEBUG */
					FUNCTION_PROFILE_STOP(&fp);
                    return rc;
                }
            }
            break;
        }

    } while(1);

	FUNCTION_PROFILE_STOP(&fp);
    return cWMA_NoErr;
}


/* ===========================================================================
 * WMAFileGetPCM
--------------------------------------------------------------------------- */
tWMA_U32 WMAFileGetPCM (
    tHWMAFileState hstate,
    tWMA_I16 *pi16Channel0, tWMA_I16 *pi16Channel1,
    tWMA_U32 max_nsamples)
{
    tWMAFileStateInternal *pInt; 
    WMARESULT wmar;
    U16_WMARawDec samples_available = (U16_WMARawDec) max_nsamples;
	FUNCTION_PROFILE(fp);

    pInt = (tWMAFileStateInternal*) hstate;

    if (pInt == NULL || pInt->hWMA == NULL || pi16Channel0 == NULL)
        return 0;

	FUNCTION_PROFILE_START (&fp,WMA_FILE_GET_PCM_PROFILE);

    if (pi16Channel1 == NULL || pi16Channel1 == pi16Channel0 + 1 || pInt->hdr_parse.nChannels <= 1 ) {
		// caller wants interleaved data or this sound is mono
	    WMARawDecGetPCM (
            pInt->hWMA, 
            (U16_WMARawDec*) &samples_available, 
            (U8_WMARawDec*) pi16Channel0, 
            (U32_WMARawDec) max_nsamples * pInt->hdr_parse.nChannels * sizeof (I16_WMARawDec)
        );
    } 
    else {
		// caller wants de-interleaved stereo data
		U16_WMARawDec i;
		tWMA_I16 *pi16C0src = pi16Channel0;
		tWMA_I16 *pi16C0dst = pi16Channel0;
		tWMA_I16 *pi16C1    = pi16Channel1;
//		assert (pInt->hdr_parse.nChannels == 2);  // if it is  greather than two, this needs work...
		samples_available >>= 1;  // half the stereo samples in one buffer then they will get split into both
        wmar = WMARawDecGetPCM (
            pInt->hWMA, 
            (U16_WMARawDec*) &samples_available, 
            (U8_WMARawDec*) pi16Channel0, 
            (U32_WMARawDec) max_nsamples * pInt->hdr_parse.nChannels * sizeof (I16_WMARawDec)
        );
        if (wmar == WMA_OK) {
			for (i = 0; i < samples_available; i++) {
				*pi16C0dst++ = *pi16C0src++;
				*pi16C1++ = *pi16C0src++;
			}
		}
	}

#ifdef USE_WOW_FILTER
    wowFilter(&(pInt->sWowChannel), pi16Channel0, pi16Channel1, samples_available);
#endif //USE_WOW_FILTER

    pInt->nSampleCount += samples_available;

	FUNCTION_PROFILE_STOP(&fp);
    return samples_available;
}





/****************************************************************************/
WMARESULT WMARawDecCBGetData (U8_WMARawDec **ppBuffer, U32_WMARawDec *pcbBuffer, U32_WMARawDec dwUserData, U8_WMARawDec* pBuffer)
{
    tWMAFileStateInternal *pInt = (tWMAFileStateInternal *) dwUserData;
    tWMA_U32 num_bytes = WMA_MAX_DATA_REQUESTED;
    tWMA_U32 cbActual =0;
    tWMA_U32 cbWanted =0;
    BYTE *pbuff = NULL;
    tWMAFileStatus rc;
    tWMAParseState parse_state;

	BYTE *bGlobalDataBuffer = pBuffer;

#ifndef WMAAPI_NO_DRM
    HRESULT hr;
#endif // WMAAPI_NO_DRM

    if(pInt == NULL || ppBuffer == NULL || pcbBuffer == NULL)
    {
        if(ppBuffer != NULL)
        {
            *ppBuffer = NULL;
        }
        if(pcbBuffer != NULL)
        {
            *pcbBuffer = 0;
        }

        return WMA_E_INVALIDARG;
    }

    *ppBuffer = NULL;
    *pcbBuffer = 0;

    /* If we used up the current payload, try to get the
     * next one.
     */
    
    // Added by Amit to take care of compressed payloads
	do
	{
		switch (pInt->payload.bIsCompressedPayload)
		{


		case 1:
			{
				do
				{
					switch (pInt->payload.bSubPayloadState)
					{
					case 1: // Compressed payload just started
						cbWanted = 1; //to read subpayload length
						cbActual = WMAFileCBGetData ((tHWMAFileState *)pInt, 
                                                     (tHWMAFileState *)pInt->pCallbackContext,
                                                     pInt->cbPayloadOffset, cbWanted, &pbuff);

						pInt->cbPayloadOffset += cbWanted;
						pInt->bBlockStart = TRUE;
						pInt->cbBlockLeft = pInt->hdr_parse.nBlockAlign;

						pInt->payload.bNextSubPayloadSize = pbuff[0];
						pInt->payload.wSubpayloadLeft = pInt->payload.bNextSubPayloadSize;
						if (pInt->payload.wSubpayloadLeft > 0)
							pInt->payload.wSubpayloadLeft -= (WORD)pInt->cbBlockLeft;

						if( pInt->payload.wTotalDataBytes > pInt->payload.bNextSubPayloadSize)
							pInt->payload.wBytesRead = pInt->payload.bNextSubPayloadSize+1;
						else if ( pInt->payload.wTotalDataBytes == pInt->payload.bNextSubPayloadSize)
							pInt->payload.wBytesRead = pInt->payload.bNextSubPayloadSize;

						pInt->payload.bSubPayloadState = 2;
						break;
					case 2: // Subpayload started
						if (pInt->cbBlockLeft == 0 && pInt->payload.wSubpayloadLeft == 0)
						{
							pInt->payload.bSubPayloadState =3;
							break;
						}
						else
						{
							if(pInt->cbBlockLeft == 0)
							{
								if (/*pInt->cbPayloadLeft*/pInt->payload.wSubpayloadLeft == 0) /* done with the file */
									return WMA_S_NEWPACKET;

								if (pInt->payload.wSubpayloadLeft > 0)
									pInt->payload.wSubpayloadLeft -= (WORD) pInt->hdr_parse.nBlockAlign;
								pInt->bBlockStart = TRUE;
								pInt->cbBlockLeft = pInt->hdr_parse.nBlockAlign;
							}
							if(num_bytes > pInt->cbBlockLeft)
								num_bytes = pInt->cbBlockLeft;

							*pcbBuffer = (unsigned int)WMAFileCBGetData((tHWMAFileState *)pInt,
                                                                        (tHWMAFileState *)pInt->pCallbackContext,
																		pInt->cbPayloadOffset, num_bytes, ppBuffer);

							memcpy(&bGlobalDataBuffer[0], *ppBuffer,*pcbBuffer);
							*ppBuffer = &bGlobalDataBuffer[0];

							pInt->cbPayloadOffset+=*pcbBuffer;
							//pInt->payload.wSubpayloadLeft -= *pcbBuffer;
							pInt->cbBlockLeft     -= *pcbBuffer;
                
							if (pInt->bHasDRM)
							{
#ifdef WMAAPI_NO_DRM
								return WMA_S_NEWPACKET;
#else  /* WMAAPI_NO_DRM */
								hr = CDrmPD_Decrypt (pInt->pDRM_state, *ppBuffer, *pcbBuffer);
								if (hr != S_OK)
								{
#ifdef LOCAL_DEBUG
									SerialPrintf("++ WMARawDecCBGetData: CDrmPD_Decrypt failed (0x%08x).\n\r", hr);
#endif /* LOCAL_DEBUG */
									*ppBuffer = NULL;
									*pcbBuffer = 0;
									return WMA_S_NEWPACKET;
								}

#endif /* WMAAPI_NO_DRM */

							}

							if (pInt->bBlockStart) {
								pInt->bBlockStart = FALSE;
								return WMA_S_NEWPACKET;
							}

							return WMA_OK;
						}

						break;
					case 3: // Subpayload finished
						if ( pInt->payload.wTotalDataBytes > pInt->payload.wBytesRead)
						{ // there are payloads to decode
							cbWanted = 1; //to read subpayload length
							cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                                        (tHWMAFileState *)pInt->pCallbackContext,
														pInt->cbPayloadOffset, cbWanted, &pbuff);

							pInt->cbPayloadOffset+=cbWanted;
							pInt->bBlockStart     = TRUE;
							pInt->cbBlockLeft     = pInt->hdr_parse.nBlockAlign;

                    
							pInt->payload.bNextSubPayloadSize = pbuff[0];
							pInt->payload.wSubpayloadLeft = pInt->payload.bNextSubPayloadSize;
							if (pInt->payload.wSubpayloadLeft > 0)
								pInt->payload.wSubpayloadLeft -= (WORD)pInt->cbBlockLeft;
							pInt->payload.wBytesRead+=pInt->payload.bNextSubPayloadSize+1;
							pInt->payload.bSubPayloadState =2;                
						}
						else
							pInt->payload.bSubPayloadState =4; // all subpayloads finished
						break;

					case 4: // All Subpayloads finished

						parse_state = pInt->parse_state;
						pInt->payload.bSubPayloadState =0;
						pInt->cbPayloadLeft =0;
						pInt->payload.bIsCompressedPayload =0;

						pInt->parse_state = csWMA_DecodePayloadEnd;
						rc = WMAF_UpdateNewPayload(pInt);
						if (rc == cWMA_NoMoreDataThisTime)
						{
							*pcbBuffer = 0;
							return WMA_OK;
						}

						pInt->parse_state = parse_state;  //restore 
						if((rc != cWMA_NoErr)) 
							return WMA_S_NEWPACKET;
						break;
					default:
						return WMA_S_NEWPACKET;

					}
				} while(1);

				break;
			}          
		default :
			{
				if(pInt->cbBlockLeft == 0 && pInt->cbPayloadLeft == 0)
				{
					tWMAFileStatus rc;
					tWMAParseState parse_state;

					parse_state = pInt->parse_state;

					pInt->parse_state = csWMA_DecodePayloadEnd;
					rc = WMAF_UpdateNewPayload(pInt);
					if (rc == cWMA_NoMoreDataThisTime)
					{
						*pcbBuffer = 0;
						return WMA_OK;
					}
					pInt->parse_state = parse_state; /* restore */
					
					if (pInt->payload.bIsCompressedPayload ==1)
						break;
					if(rc != cWMA_NoErr)
						return WMA_S_NEWPACKET;
				}

				/* return as much as we currently have left */

				if(pInt->cbBlockLeft == 0)
				{
					if(pInt->cbPayloadLeft == 0)
					{
						/* done with the file */
						return WMA_S_NEWPACKET;
					}

					pInt->cbPayloadLeft -= pInt->hdr_parse.nBlockAlign;
					pInt->bBlockStart = TRUE;
					pInt->cbBlockLeft = pInt->hdr_parse.nBlockAlign;
				}
				if(num_bytes > pInt->cbBlockLeft)
					num_bytes = pInt->cbBlockLeft;

				*pcbBuffer = (unsigned int)WMAFileCBGetData((tHWMAFileState *)pInt,
                                                            (tHWMAFileState *)pInt->pCallbackContext,
															pInt->cbPayloadOffset, num_bytes, ppBuffer);

				memcpy(&bGlobalDataBuffer[0], *ppBuffer,*pcbBuffer);
				*ppBuffer = &bGlobalDataBuffer[0];

				pInt->cbPayloadOffset += *pcbBuffer;
				pInt->cbBlockLeft     -= *pcbBuffer;

				/* DRM decryption if necessary */

				if (pInt->bHasDRM)
				{

#ifdef WMAAPI_NO_DRM
					return WMA_S_NEWPACKET;
#else  /* WMAAPI_NO_DRM */

					hr = CDrmPD_Decrypt (pInt->pDRM_state, *ppBuffer, *pcbBuffer);
					if (hr != S_OK) {
#ifdef LOCAL_DEBUG
						SerialPrintf("++ WMARawDecCBGetData: CDrmPD_Decrypt failed (0x%08x).\n\r", hr);
#endif /* LOCAL_DEBUG */
						*ppBuffer = NULL;
						*pcbBuffer = 0;
						return WMA_S_NEWPACKET;
					}

#endif /* WMAAPI_NO_DRM */

				}

				if (pInt->bBlockStart) {
					pInt->bBlockStart = FALSE;
					return WMA_S_NEWPACKET;
				}

				return WMA_OK;
			}
		}


	}while(1);
    
	return WMA_OK;
}
