//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wma.c
//
//--------------------------------------------------------------------------

#include "bldsetup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _HRESULT_DEFINED
#include "wmf.h"
#undef _HRESULT_DEFINED

#if defined (macintosh) || defined (HITACHI)
#ifndef __LONG_DEFINED__
#define __LONG_DEFINED__
typedef I32_WMC LONG; 
#endif
#include "wmMacSp.h" 
#endif //macintosh
#define __BASICTYPEDEFINED__

#include "wmfdec.h"
#include "wmcguids.h"
#include "wmfparse.h"
#include "wmf_loadstuff.h"
#include "wmadecS_api.h"
#ifndef __NO_SCREEN__
#include "wmscreendec_api.h"
#endif

#ifndef __NO_SPEECH__
#include "wmsdec_api.h"
#endif

#ifndef _ASFPARSE_ONLY_
#include "wmvdec_api.h"
#endif


#define CHECK_FRAMERATE		6

tWMCDecStatus WMCDecUpdateNewPayload(HWMCDECODER hDecoder, tMediaType_WMC MediaType, U16_WMC Index);
tWMCDecStatus WMCDecDecodeFrameX( HWMCDECODER hDecoder, U16_WMC * pbDecoded, U32_WMC* pnNumberOfSamples,tMediaType_WMC MediaType, U16_WMC Index );
tWMCDecStatus WMCDecParseVirtualPacketHeader(HWMCDECODER hDecoder, U32_WMC cbCurrentPacketOffset, PACKET_PARSE_INFO_EX* pParseInfoEx);
tWMCDecStatus WMCDecParseVirtualPayloadHeader(HWMCDECODER hDecoder, U32_WMC cbCurrentPacketOffset, PACKET_PARSE_INFO_EX *pParseInfoEx, PAYLOAD_MAP_ENTRY_EX *pPayload);
tWMCDecStatus WMScDecodeData (HWMCDECODER hWMCDec, U16_WMC wIndex );
tWMCDecStatus WMCDecGetMediaType (HWMCDECODER hWMCDec, U16_WMC wStreamId, tMediaType_WMC * pMediaType);
tWMCDecStatus WMCRawGetVideoData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength);
tWMCDecStatus WMCRawGetBinaryData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength);
tWMCDecStatus WMCRawGetData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength);


/****************************************************************************/
/*static Void_WMC WMADecoderCleanUp(WMFDecoderEx *pDecoder)
{
    U16_WMC i=0;

    if(pDecoder)
    {
        for(i=0; i<pDecoder->tHeaderInfo.wNoOfAudioStreams; i++ )
        {
            if(pDecoder->tAudioStreamInfo[i]->hMSA !=NULL)
            {
    			WMARawDecClose(&(pDecoder->tAudioStreamInfo[i]->hMSA));
                pDecoder->tAudioStreamInfo[i]->hMSA = NULL;
            }
            if(pDecoder->tAudioStreamInfo[i]->pbAudioBuffer !=NULL)
            {
    			wmvfree(pDecoder->tAudioStreamInfo[i]->pbAudioBuffer);
                pDecoder->tAudioStreamInfo[i]->pbAudioBuffer = NULL;
            }
            pDecoder->tAudioStreamInfo[i] = NULL;
        }
        
        if(pDecoder->pszType != NULL)
        {
            wmvfree(pDecoder->pszType);
        }

        if(pDecoder->pszKeyID != NULL)
        {
            wmvfree(pDecoder->pszKeyID);
        }
        if(pDecoder->m_pLicData != NULL)
        {
            wmvfree(pDecoder->m_pLicData);
        }
//        memset(pDecoder, 0, sizeof(WMFDecoderEx));
    }
}
*/

/****************************************************************************/

#ifdef _ZMDRM_

tWMCDecStatus WMCDecInitZMDRM(HWMCDECODER hDecoder ,U32_WMC Rights)
{
    HRESULT hr;
    WMFDecoderEx *pDecoder;

#ifdef GLOBAL_SECRET
        ERROR: GLOBAL_SECRET must not be defined for ZapMedia linux port
#endif

	printf("INFO: In %s \n", __FILE__ );	// rajn


    pDecoder = (WMFDecoderEx *) (hDecoder);

    if (pDecoder->cbEnhData == 0)
    {
		printf("ERROR: In %s:%d, cbEnhData is 0\n", __FILE__, __LINE__);	// rajn
        return WMCDec_DRMFail;
    }


    hr = WMFSetRights(pDecoder->hDRM, Rights);
    if ( hr != 0 )
    {
        printf("ERROR: WMFSetRights - hr = 0x%x\n", hr);
        return WMCDec_DRMFail;
    }


    hr = WMFSetEnhancedData(pDecoder->hDRM, pDecoder->cbEnhData, pDecoder->pbEnhData );
    if ( hr != 0 )
    {
        printf("ERROR: WMFSetEnhancedData - hr = 0x%x\n", hr);
        return WMCDec_DRMFail;
    }


	// rajn -- debug
	{
		U32_WMC i;	
		printf("\nINFO: ---< V2Hdr (%d bytes)>---\n", pDecoder->cbEnhData);
		for(i=0; i < pDecoder->cbEnhData; i += 2 )
		{
			// cheap unicode printf
			printf("%c", pDecoder->pbEnhData[i] );						
	
		}
		printf("\n---< End of V2Hdr >--\n");
	}




    hr = WMFCanDecrypt(pDecoder->hDRM);
//    hr = WMFCanDecrypt(pDecoder->hDRM,pDecoder->cbSecData, pDecoder->pbSecData);

    if ( hr == 0 ) // S_OK
    {
        return WMCDec_Succeeded;
    }

    if ( hr == 1 ) // S_FALSE
    {
        return WMCDec_DRMInitFail;
    }

    // hr == E_FAIL ...
    return WMCDec_DRMFail;

}
#else
U8_WMC pDrmBuffer[2048];
#endif

/****************************************************************************/
tWMCDecStatus WMFDecTimeToOffset(
                    HWMCDECODER hDecoder,
                    U32_WMC msSeek,
                    U64_WMC *pcbPacketOffset )
{
    U32_WMC nPacket;
    WMFDecoderEx *pDecoder;

    if( ( NULL == hDecoder ) || ( NULL == pcbPacketOffset ) )
    {
        return( WMCDec_InValidArguments );
    }


    pDecoder = (WMFDecoderEx *) hDecoder;

  if( msSeek > pDecoder->tHeaderInfo.msDuration )
    {
        return( WMCDec_InValidArguments );
    }

    nPacket = 0;
    if( pDecoder->tHeaderInfo.msDuration > 0 )
    {
        nPacket = ( msSeek * pDecoder->tHeaderInfo.cPackets ) / pDecoder->tHeaderInfo.msDuration;
    }

    *pcbPacketOffset = (U64_WMC)pDecoder->tHeaderInfo.cbHeader + ( (U64_WMC)nPacket * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize );

    return( WMCDec_Succeeded );
}


/****************************************************************************/
tWMCDecStatus WMFCloseDecoder (HWMCDECODER  *hDecoder)
{
    WMFDecoderEx *pDecoder;

    if( NULL == hDecoder )
    {
        return( WMCDec_InValidArguments );
    }

    pDecoder = (WMFDecoderEx *) (*hDecoder);

//    WMADecoderCleanUp(pDecoder);
    wmvfree( pDecoder );
    pDecoder = NULL;

    return( WMCDec_Succeeded );
}

#ifndef WMFAPI_NO_DRM
/****************************************************************************/
tWMCDecStatus WMFDecDRM (HWMCDECODER hDecoder, U32_WMC *pfDRM)
{
    if(hDecoder == NULL || pfDRM == NULL)
    {
        return WMCDec_InValidArguments;
    }

    *pfDRM = ((WMFDecoderEx *)hDecoder)->hDRM != NULL;

    return WMCDec_Succeeded;
}

#endif

/****************************************************************************/
tWMCDecStatus WMCReadIndex (HWMCDECODER  *hDecoder)
{
    
    tWMCDecStatus err = WMCDec_Succeeded;
//    const U8_WMC *pbTemp;
    GUID objId;
    U64_WMC qwSize;
    U8_WMC *pbBuffer= NULL;
    U64_WMC dwIndexOffset =0;
    U8_WMC i=0;
    U32_WMC cbActual =0;
    U32_WMC cbWanted =0;
    U8_WMC *pData = NULL;
    U32_WMC dwSize =0;

    
    WMFDecoderEx *pDecoder;

    if( NULL == hDecoder )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *) (*hDecoder);
    if (pDecoder->tHeaderInfo.wNoOfVideoStreams == 0)
        return WMCDec_Succeeded;

    dwIndexOffset = pDecoder->tHeaderInfo.cbLastPacketOffset + pDecoder->tHeaderInfo.cbPacketSize;

    do
    {
 //       err = WMFCBDecodeGetData (*hDecoder, Header, 24, dwIndexOffset, &cbRead);
        cbWanted =24;

        cbActual = WMCDecCBGetData(*hDecoder, dwIndexOffset, cbWanted, &pData,pDecoder->u32UserData );

	    if (cbActual != cbWanted) 
        {
		    return WMCDec_Succeeded;
	    }

//        pbTemp = Header;

        LoadGUID( objId, pData );
        LoadQWORD( qwSize, pData );
        dwSize = *((U32_WMC*) &qwSize);

        if( IsEqualGUID_WMC( &CLSID_CAsfIndexObjectV2, &objId ) )
        {
/*            pbBuffer = (U8_WMC *)wmvalloc(*((U32_WMC*) &qwSize));
            if (pbBuffer == NULL)
                return WMCDec_BadMemory;
    
            err = WMFCBDecodeGetData (hDecoder, pbBuffer, *((U32_WMC*) &qwSize), dwIndexOffset, &cbRead);
	        if (cbRead != *((U32_WMC*) &qwSize) || err !=WMCDec_Succeeded) 
            {
                wmvfree(pbBuffer);
                pbBuffer = NULL;
                return WMCDec_Fail;
	        }
*/
            LoadIndexObject(hDecoder,dwIndexOffset,dwSize , pDecoder->tVideoStreamInfo[i]->wStreamId, &pDecoder->tIndexInfo[pDecoder->cTotalIndexs]);
            i++;
            pDecoder->cTotalIndexs+=1;

 //           wmvfree(pbBuffer);
 //           pbBuffer = NULL;

        }

        dwIndexOffset+=(*((U32_WMC*) &qwSize));

    }while(pDecoder->tHeaderInfo.wNoOfVideoStreams > pDecoder->cTotalIndexs );



    return( WMCDec_Succeeded );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************/
tWMCDecStatus WMCInitnParseHeader(HWMCDECODER *phDecoder
#ifdef _ZMDRM_
                    ,HDRMMGR *phDRM
                    ,U8_WMC *pAppCert
                    ,U32_WMC Rights
                    ,const U8_WMC *pbHwId
                    ,U32_WMC cbHwId
#endif
                    )
{
    WMFDecoderEx *pDecoder;
    tWMCDecStatus rc = WMCDec_Succeeded;
//    U32_WMC u32UserContext =0;
//	I32_WMC i32PostProc =0;

    pDecoder = NULL;

    do
    {
        pDecoder = (WMFDecoderEx *) (*phDecoder);

        if( NULL == pDecoder )
        {
            rc = WMCDec_BadMemory;
            break;
        }

      /*  u32UserContext = pDecoder->u32UserData;
		i32PostProc = pDecoder->i32PostProc;
        memset( pDecoder, 0, sizeof( WMFDecoderEx ) );

        pDecoder->u32UserData = u32UserContext;
		
		pDecoder->i32PostProc = i32PostProc;*/

        pDecoder->m_pExtendedContentDesc = NULL;
        pDecoder->m_pScriptCommand = NULL;
        pDecoder->m_pMarkers = NULL;
        pDecoder->m_pContentDesc = NULL;

        rc = ParseAsfHeader(phDecoder, (U8_WMC) 1);
        if( WMCDec_Succeeded != rc )
			break;

    }while(0);


    if( WMCDec_Succeeded != rc )
    {
        WMCDecClose(phDecoder);
        *phDecoder = NULL;
        // wmvfree( pDecoder );

		return rc;
    }

    pDecoder->tHeaderInfo.cbLastPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
    if (pDecoder->tHeaderInfo.cPackets > 0)
        pDecoder->tHeaderInfo.cbLastPacketOffset += (pDecoder->tHeaderInfo.cPackets - 1)*pDecoder->tHeaderInfo.cbPacketSize;

	pDecoder->tHeaderInfo.cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader;// Added by amit
    pDecoder->tHeaderInfo.cbNextPacketOffset = pDecoder->tHeaderInfo.cbHeader;
    pDecoder->parse_state = csWMCNewAsfPacket;

    return( rc );
}


/****************************************************************************/

#ifdef WMC_NO_BUFFER_MODE

tWMCDecStatus WMCDecGetAndDecode (HWMCDECODER hWMCDec, U32_WMC* pnStreamReadyForOutput, U32_WMC* pnNumberOfSamples)
{
    tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder = NULL;    
    I32_WMC lBytesWritten =0;
    U64_WMC cbPacketOffset=0;
    I32_WMC i;
	Bool_WMC	bAllDone = FALSE_WMC;
    U32_WMC     dwMinPresTime=0xffffffff;
    U32_WMC     dwMinAudioPresTime=0xffffffff;
    U32_WMC     dwMinVideoPresTime=0xffffffff;
    U32_WMC     dwMinBinaryPresTime=0xffffffff;
    U16_WMC     wMinAudioIndex = 0xffff;
    U16_WMC     wMinVideoIndex = 0xffff;
    U16_WMC     wMinBinaryIndex = 0xffff;
    U16_WMC		Index =0;
    tMediaType_WMC MediaType = Audio_WMC;

	Bool_WMC	bGotAtLeastOneVideoOutput = FALSE_WMC;	
	Bool_WMC	bGotAtLeastOneBinaryOutput = FALSE_WMC;
	Bool_WMC	bGotAtLeastOneVideoStreamToOutput = FALSE_WMC;	
	Bool_WMC	bGotAtLeastOneBinaryStreamToOutput = FALSE_WMC;


    if(hWMCDec==NULL_WMC)
        return WMCDec_InValidArguments;
    
    pDecoder = (WMFDecoderEx*) hWMCDec;

    if (pDecoder->bParsed != TRUE_WMC)
        return WMCDec_InValidArguments;

    if (pDecoder->tPlannedOutputInfo.wTotalOutput == 0)
        return WMCDec_DecodeComplete;


    for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
    {
        Index = pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex;

        switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
        {
        case Audio_WMC:
			if (pDecoder->tAudioStreamInfo[Index]->bTobeDecoded == FALSE_WMC)
			{
			
				if (pDecoder->tAudioStreamInfo[Index]->bOutputisReady == TRUE_WMC) // Last output is not taken out
				{
					//Fix buffers
					U8_WMC TempOutBuff[1024];
					U32_WMC cbBuffer=0;
					do
					{
						WMCRawGetData (TempOutBuff, &cbBuffer, hWMCDec, (U32_WMC) 1024);
					}while(cbBuffer>0);

					pDecoder->tAudioStreamInfo[Index]->bOutputisReady = FALSE_WMC;
				}
			}
/*			else
			{
				if (pDecoder->tAudioStreamInfo[Index]->bOutputisReady == TRUE_WMC)
				{
					I16_WMC TempOutBuff[4096];
					U32_WMC cbBuffer=0;
					I64_WMC  tPresentationTime=0;
					do
					{
						WMCDecGetAudioOutput(hWMCDec, (I16_WMC*)&TempOutBuff[0], (I16_WMC*)NULL_WMC,(U32_WMC)1024, &cbBuffer, &tPresentationTime )	;					WMCRawGetData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
					}while(cbBuffer>0);
				
					pDecoder->tAudioStreamInfo[Index]->bOutputisReady = FALSE_WMC;
				}
			
			}*/

            break;
        case Video_WMC:
			if ((pDecoder->tVideoStreamInfo[Index]->bHasGivenAnyOutput == TRUE_WMC)&&(pDecoder->tVideoStreamInfo[Index]->bStopReading == FALSE_WMC))
				bGotAtLeastOneVideoOutput = TRUE_WMC;
			
			if ((pDecoder->tVideoStreamInfo[Index]->bWantOutput == TRUE_WMC)&&(pDecoder->tVideoStreamInfo[Index]->bStopReading == FALSE_WMC))
				bGotAtLeastOneVideoStreamToOutput = TRUE_WMC;

			if (pDecoder->tVideoStreamInfo[Index]->bTobeDecoded == FALSE_WMC)
			{
				if (pDecoder->tVideoStreamInfo[Index]->bOutputisReady == TRUE_WMC) // Last output is not taken out
				{
					//Fix buffers
					U8_WMC TempOutBuff[1024];
					U32_WMC cbBuffer=0;
					do
					{
						WMCRawGetVideoData (TempOutBuff, &cbBuffer, hWMCDec, (U32_WMC) 1024);
					}while(cbBuffer>0);
	
					pDecoder->tVideoStreamInfo[Index]->bOutputisReady = FALSE_WMC;
				
				}
			}
            break;
        case Binary_WMC:
			if ((pDecoder->tBinaryStreamInfo[Index]->bHasGivenAnyOutput == TRUE_WMC)&&(pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC))
				bGotAtLeastOneBinaryOutput = TRUE_WMC;

			if ((pDecoder->tBinaryStreamInfo[Index]->bWantOutput == TRUE_WMC)&&(pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC))
				bGotAtLeastOneBinaryStreamToOutput = TRUE_WMC;

			if (pDecoder->tBinaryStreamInfo[Index]->bOutputisReady == TRUE_WMC) // Last output is not taken out
			{
				//Fix buffers
				U8_WMC TempOutBuff[1024];
				U32_WMC cbBuffer=0;
				do
				{
					WMCRawGetBinaryData (TempOutBuff, &cbBuffer, hWMCDec, (U32_WMC) 1024);
				}while(cbBuffer>0);
			
				pDecoder->tBinaryStreamInfo[Index]->bOutputisReady = FALSE_WMC;
			}
        }
    }


	Index =0;

	do
	{
		
		for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
		{
		   Index = pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex;
		   
		   MediaType = pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType;
		   
		   switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
			{
			case Audio_WMC:
				if ((pDecoder->tAudioStreamInfo[Index]->parse_state != csWMCEnd )/*&&(pDecoder->tAudioStreamInfo[Index]->cbNextPacketOffset <=pDecoder->tHeaderInfo.cbLastPacketOffset)*/)
				{
					if (pDecoder->tAudioStreamInfo[Index]->parse_state != csWMCDecodeLoopStart)
					{
						err = WMCDecUpdateNewPayload(hWMCDec, MediaType, Index);
					}
					if (pDecoder->tAudioStreamInfo[Index]->parse_state != csWMCEnd )
					{
						if (dwMinAudioPresTime > (U32_WMC)pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp)
						{
							dwMinAudioPresTime = (U32_WMC)(pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp);
							wMinAudioIndex = Index;
						}
					}
					else
						pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;

				}
				else
					pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;

                if (pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone == TRUE_WMC)
                {
                    dwMinAudioPresTime = 0xffffffff;
                    pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset =0;
                    pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded =0;
                    pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCEnd;
                }


				break;
			case Video_WMC:
				if ((pDecoder->tVideoStreamInfo[Index]->parse_state != csWMCEnd )/*&&(pDecoder->tVideoStreamInfo[Index]->cbNextPacketOffset <= pDecoder->tHeaderInfo.cbLastPacketOffset)*/)
				{
					if (pDecoder->tVideoStreamInfo[Index]->dwNextVideoTimeStamp > pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp)
						pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = pDecoder->tVideoStreamInfo[Index]->dwNextVideoTimeStamp;
					if (pDecoder->tVideoStreamInfo[Index]->parse_state != csWMCDecodeLoopStart)
					{
						err = WMCDecUpdateNewPayload(hWMCDec, MediaType, Index);
					}
					if (pDecoder->tVideoStreamInfo[Index]->parse_state != csWMCEnd )
					{
						if ((dwMinVideoPresTime > (U32_WMC)pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp)&&((pDecoder->tVideoStreamInfo[Index]->bStopReading == FALSE_WMC) || (bGotAtLeastOneVideoOutput == FALSE_WMC)))
						{
							dwMinVideoPresTime = (U32_WMC)(pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp);
							wMinVideoIndex = Index;
						}
						else if((dwMinVideoPresTime == (U32_WMC)pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp ) && (pDecoder->tVideoStreamInfo[Index]->bStopReading == FALSE_WMC))
						{
							dwMinVideoPresTime = (U32_WMC)pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp;
							wMinVideoIndex = Index;
						}

					}
					else
						pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;

				}
				else
					pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;

                if ((pDecoder->tVideoStreamInfo[Index]->bStopReading == TRUE_WMC) && (bGotAtLeastOneVideoOutput == TRUE_WMC) || (pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone == TRUE_WMC))
				{
					//Dont Read and decode this stream.
					pDecoder->tVideoStreamInfo[Index]->bNowStopReadingAndDecoding = TRUE_WMC;
					
					dwMinVideoPresTime = 0xffffffff;
					pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset =0;
					pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded =0;
					pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCEnd; 
				}

				break;
			case Binary_WMC:
			   if ((pDecoder->tBinaryStreamInfo[Index]->parse_state != csWMCEnd )/* && (pDecoder->tBinaryStreamInfo[Index]->cbNextPacketOffset <=pDecoder->tHeaderInfo.cbLastPacketOffset)*/)
				{
					if (pDecoder->tBinaryStreamInfo[Index]->dwNextBinaryTimeStamp > pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp)
						pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp = pDecoder->tBinaryStreamInfo[Index]->dwNextBinaryTimeStamp;
					if (pDecoder->tBinaryStreamInfo[Index]->parse_state != csWMCDecodeLoopStart)
					{
						err = WMCDecUpdateNewPayload(hWMCDec, MediaType, Index);
					}
					if (pDecoder->tBinaryStreamInfo[Index]->parse_state != csWMCEnd )
					{
						if ((dwMinBinaryPresTime > (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp)&&((pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC) || (bGotAtLeastOneBinaryOutput == FALSE_WMC)))
						{
							dwMinBinaryPresTime = (U32_WMC)(pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp);
							wMinBinaryIndex = Index;
						}
						else if((dwMinBinaryPresTime == (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp ) && (pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC))
						{
							dwMinBinaryPresTime = (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp;
							wMinBinaryIndex = Index;
						}
					}
					else
						pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;
				}
			   else
				   pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;

                if ((pDecoder->tBinaryStreamInfo[Index]->bStopReading == TRUE_WMC) && (bGotAtLeastOneBinaryOutput == TRUE_WMC)|| (pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone == TRUE_WMC))
				{
					//Dont Read and decode this stream.
					pDecoder->tBinaryStreamInfo[Index]->bNowStopReadingAndDecoding = TRUE_WMC;
					
					dwMinBinaryPresTime = 0xffffffff;
					pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset =0;
					pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded =0;
					pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCEnd; 
				}

				break;
	//        default:
	//            bReadyForOutput = FALSE_WMC;

			}
		}



		for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
		{
			if (pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone == TRUE_WMC)
			{
				bAllDone = TRUE_WMC;
			}
			else
			{
				bAllDone = FALSE_WMC;
				break;
			}

		}

		if (bAllDone == TRUE_WMC)
		{
			*pnStreamReadyForOutput = 0xff;
			return WMCDec_Succeeded;
		}
   
		
		// Findout Minimum of all 3..... and select which stream to decode
		
		
		dwMinPresTime = dwMinAudioPresTime;
		MediaType = Audio_WMC;
		Index = wMinAudioIndex;

		if (dwMinVideoPresTime < dwMinPresTime)
		{
			dwMinPresTime = dwMinVideoPresTime;
			MediaType = Video_WMC;
	 		Index = wMinVideoIndex;
	   }
		if (dwMinBinaryPresTime < dwMinPresTime)
		{
			dwMinPresTime = dwMinBinaryPresTime;
			MediaType = Binary_WMC;
	 		Index = wMinBinaryIndex;
		}

		if (dwMinPresTime == 0xffffffff)
		{
		 // No stream is ready to get decoded;
    
			*pnStreamReadyForOutput = 0xff;
			return WMCDec_Succeeded;
		}

		err = WMCDecDecodeFrameX(hWMCDec, (U16_WMC*)pnStreamReadyForOutput, pnNumberOfSamples, MediaType, Index );
		if ((err == WMCDec_Succeeded)&&(*pnStreamReadyForOutput == 0xff))
			continue;
		
		return err;
	
	}while(1);
            
}
/***********************************************************************************************************/
tWMCDecStatus WMCDecUpdateNewPayload(HWMCDECODER hDecoder, tMediaType_WMC MediaType, U16_WMC Index)
{

	tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder =NULL;
	tMediaType_WMC LocalMediaType;
	U32_WMC cbWanted =0;
	U32_WMC cbActual =0;
	U8_WMC *pData = NULL;
	
    if (NULL == hDecoder)
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *) (hDecoder);
    
	switch(MediaType)
	{
	
	case Audio_WMC:
		do
		{
			switch(pDecoder->tAudioStreamInfo[Index]->parse_state)
			{
				case csWMCNewAsfPacket:

					if(pDecoder->tAudioStreamInfo[Index]->cbNextPacketOffset > pDecoder->tHeaderInfo.cbLastPacketOffset)
					{
						pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCEnd;
						return WMCDec_DecodeComplete;
					}

					pDecoder->tAudioStreamInfo[Index]->cbCurrentPacketOffset = pDecoder->tAudioStreamInfo[Index]->cbNextPacketOffset;
					pDecoder->tAudioStreamInfo[Index]->cbNextPacketOffset += pDecoder->tHeaderInfo.cbPacketSize;

					err = WMCDecParseVirtualPacketHeader( hDecoder, (U32_WMC)pDecoder->tAudioStreamInfo[Index]->cbCurrentPacketOffset, &(pDecoder->tAudioStreamInfo[Index]->ppex));

					if(err != WMCDec_Succeeded)
					{
						return WMCDec_BadPacketHeader;
					}

					if (pDecoder->tAudioStreamInfo[Index]->ppex.fEccPresent && pDecoder->tAudioStreamInfo[Index]->ppex.fParityPacket)
					{
						break;
					}

					pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodePayloadStart;
					pDecoder->tAudioStreamInfo[Index]->iPayload = 0;
					break;

				case csWMCDecodePayloadStart:

					if (pDecoder->tAudioStreamInfo[Index]->iPayload >= pDecoder->tAudioStreamInfo[Index]->ppex.cPayloads)
					{
						pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCNewAsfPacket;
						break;//return WMCDec_Succeeded; // Return after taking full packet
					}

					err = WMCDecParseVirtualPayloadHeader(hDecoder, (U32_WMC)pDecoder->tAudioStreamInfo[Index]->cbCurrentPacketOffset, &(pDecoder->tAudioStreamInfo[Index]->ppex), &(pDecoder->tAudioStreamInfo[Index]->payload));
					if(err != WMCDec_Succeeded)
					{
						pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
						break;
					}
					err = WMCDecGetMediaType (hDecoder,  pDecoder->tAudioStreamInfo[Index]->payload.bStreamId, &LocalMediaType);
					if (err != WMCDec_Succeeded)
					{
						pDecoder->parse_state = csWMCDecodePayloadEnd;
						break;
					}
					if ((LocalMediaType != Audio_WMC)|| (pDecoder->tAudioStreamInfo[Index]->wStreamId != pDecoder->tAudioStreamInfo[Index]->payload.bStreamId))
					{
						pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
						break;
					}

					pDecoder->tAudioStreamInfo[Index]->wPayStart = pDecoder->tAudioStreamInfo[Index]->payload.cbPacketOffset + pDecoder->tAudioStreamInfo[Index]->payload.cbTotalSize  - pDecoder->tAudioStreamInfo[Index]->payload.cbPayloadSize;

					if( pDecoder->tAudioStreamInfo[Index]->payload.cbRepData != 1 )
					{
						pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodePayload;
						pDecoder->tAudioStreamInfo[Index]->payload.bIsCompressedPayload =0; // Amit to take care of compressed payloads.
						break;
					}
					else if( pDecoder->tAudioStreamInfo[Index]->payload.cbRepData == 1)    // Amit to take care of compressed payloads.
					{
						pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodeCompressedPayload;
						pDecoder->tAudioStreamInfo[Index]->payload.bIsCompressedPayload =1;
						break;
					}
 
				case csWMCDecodePayload:
					if((pDecoder->tAudioStreamInfo[Index]->bWantOutput == TRUE_WMC) &&(pDecoder->tAudioStreamInfo[Index]->bStopReading == FALSE_WMC))
					{
						if(pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp == 0)
							pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp = (U32_WMC) pDecoder->tAudioStreamInfo[Index]->payload.msObjectPres;
						pDecoder->tAudioStreamInfo[Index]->dwAudPayloadPresTime = (U32_WMC) pDecoder->tAudioStreamInfo[Index]->payload.msObjectPres;
						pDecoder->tAudioStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tAudioStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tAudioStreamInfo[Index]->wPayStart;
						pDecoder->tAudioStreamInfo[Index]->bBlockStart     = TRUE_WMC;
						pDecoder->tAudioStreamInfo[Index]->dwBlockLeft     = pDecoder->tAudioStreamInfo[Index]->nBlockAlign;
						pDecoder->tAudioStreamInfo[Index]->dwFrameSize = pDecoder->tAudioStreamInfo[Index]->payload.cbObjectSize; 
						pDecoder->tAudioStreamInfo[Index]->dwPayloadLeft = pDecoder->tAudioStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
					}
     				pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodeLoopStart ;//csWMCDecodePayloadEnd;
		//			break;
		            return WMCDec_Succeeded;

				case csWMCDecodePayloadEnd:
					pDecoder->tAudioStreamInfo[Index]->iPayload++;
					pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodePayloadStart;
					break;

				case csWMCDecodeCompressedPayload: // Added by Amit to take care of compressed payloads
					if((pDecoder->tAudioStreamInfo[Index]->bWantOutput == TRUE_WMC) &&(pDecoder->tAudioStreamInfo[Index]->bStopReading == FALSE_WMC))
					{
						if(pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp == 0)
							pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp = (U32_WMC) pDecoder->tAudioStreamInfo[Index]->payload.msObjectPres;
						pDecoder->tAudioStreamInfo[Index]->dwAudPayloadPresTime = (U32_WMC) pDecoder->tAudioStreamInfo[Index]->payload.msObjectPres;

						pDecoder->tAudioStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tAudioStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tAudioStreamInfo[Index]->wPayStart;

						pDecoder->tAudioStreamInfo[Index]->bBlockStart     = TRUE_WMC;
						pDecoder->tAudioStreamInfo[Index]->dwBlockLeft     = pDecoder->tAudioStreamInfo[Index]->nBlockAlign;
//						pDecoder->tAudioStreamInfo[Index]->dwPayloadLeft = pDecoder->tAudioStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
						pDecoder->tAudioStreamInfo[Index]->payload.wBytesRead =0;
						pDecoder->tAudioStreamInfo[Index]->payload.bSubPayloadState =1;

						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hDecoder, pDecoder->tAudioStreamInfo[Index]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							return WMCDec_BufferTooSmall;
						}
						
						
						pDecoder->tAudioStreamInfo[Index]->dwFrameSize = pData[0];

					}

     				pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCDecodeLoopStart ;//csWMCDecodePayloadEnd;
		//			break;
		            return WMCDec_Succeeded;
//				default:
			}
            
        
		}while(1);

		break;

	case Video_WMC:
		do
		{
			switch(pDecoder->tVideoStreamInfo[Index]->parse_state)
			{
				case csWMCNewAsfPacket:

					if(pDecoder->tVideoStreamInfo[Index]->cbNextPacketOffset > pDecoder->tHeaderInfo.cbLastPacketOffset)
					{
						pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCEnd;
						return WMCDec_DecodeComplete;
					}

					pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset = pDecoder->tVideoStreamInfo[Index]->cbNextPacketOffset;
					pDecoder->tVideoStreamInfo[Index]->cbNextPacketOffset += pDecoder->tHeaderInfo.cbPacketSize;

					err = WMCDecParseVirtualPacketHeader( hDecoder, (U32_WMC)pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset, &(pDecoder->tVideoStreamInfo[Index]->ppex));

					if(err != WMCDec_Succeeded)
					{
						return WMCDec_BadPacketHeader;
					}

					if (pDecoder->tVideoStreamInfo[Index]->ppex.fEccPresent && pDecoder->tVideoStreamInfo[Index]->ppex.fParityPacket)
					{
						break;
					}

					pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadStart;
					pDecoder->tVideoStreamInfo[Index]->iPayload = 0;
					break;

				case csWMCDecodePayloadStart:

					if (pDecoder->tVideoStreamInfo[Index]->iPayload >= pDecoder->tVideoStreamInfo[Index]->ppex.cPayloads)
					{
						pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCNewAsfPacket;
						break; //return WMCDec_Succeeded; // Return after taking full packet
					}

					err = WMCDecParseVirtualPayloadHeader(hDecoder, (U32_WMC)pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset, &(pDecoder->tVideoStreamInfo[Index]->ppex), &(pDecoder->tVideoStreamInfo[Index]->payload));
					if(err != WMCDec_Succeeded)
					{
						pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
						break;
					}


					err = WMCDecGetMediaType (hDecoder,  pDecoder->tVideoStreamInfo[Index]->payload.bStreamId, &LocalMediaType);
					if (err != WMCDec_Succeeded)
					{
						pDecoder->parse_state = csWMCDecodePayloadEnd;
						break;
					}
					if ((LocalMediaType != Video_WMC) || (pDecoder->tVideoStreamInfo[Index]->wStreamId != pDecoder->tVideoStreamInfo[Index]->payload.bStreamId))
					{
						pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
						break;
					}

					pDecoder->tVideoStreamInfo[Index]->wPayStart = pDecoder->tVideoStreamInfo[Index]->payload.cbPacketOffset + pDecoder->tVideoStreamInfo[Index]->payload.cbTotalSize  - pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize;

					if( pDecoder->tVideoStreamInfo[Index]->payload.cbRepData != 1 )
					{
						pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayload;
						pDecoder->tVideoStreamInfo[Index]->payload.bIsCompressedPayload =0; // Amit to take care of compressed payloads.
						break;
					}
					else if( pDecoder->tVideoStreamInfo[Index]->payload.cbRepData == 1)    // Amit to take care of compressed payloads.
					{
						pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodeCompressedPayload;
						pDecoder->tVideoStreamInfo[Index]->payload.bIsCompressedPayload =1;
						break;
					}
 
				case csWMCDecodePayload:
		// In this case store payload in buffer and dont return. Return only after one packet is finished

					if((pDecoder->tVideoStreamInfo[Index]->bWantOutput == TRUE_WMC) &&(pDecoder->tVideoStreamInfo[Index]->bNowStopReadingAndDecoding == FALSE_WMC))
					{
						pDecoder->tVideoStreamInfo[Index]->dwPayloadOffset = (U32_WMC) (pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tVideoStreamInfo[Index]->wPayStart);
                    
//						if(pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp == 0)
							pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = (U32_WMC) pDecoder->tVideoStreamInfo[Index]->payload.msObjectPres;

						// Keep data about frame size, offset, frame used etc... 
						if (0 == pDecoder->tVideoStreamInfo[Index]->payload.cbObjectOffset) 
						{ // New Frame starts
							if (!(pDecoder->tVideoStreamInfo[Index]->bFirst)) 
							{
								if (pDecoder->tVideoStreamInfo[Index]->payload.bIsKeyFrame == 0) 
								{
									pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
									break;
								}
								pDecoder->tVideoStreamInfo[Index]->bFirst = 1;
							
							}

							pDecoder->tVideoStreamInfo[Index]->cbUsed = pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize;
							pDecoder->tVideoStreamInfo[Index]->cbFrame = pDecoder->tVideoStreamInfo[Index]->dwNbFrames;
							pDecoder->tVideoStreamInfo[Index]->dwNbFrames += 1;
							pDecoder->tVideoStreamInfo[Index]->dwFrameSize = pDecoder->tVideoStreamInfo[Index]->payload.cbObjectSize; 
							pDecoder->tVideoStreamInfo[Index]->bIsKeyFrame = pDecoder->tVideoStreamInfo[Index]->payload.bIsKeyFrame;
							pDecoder->tVideoStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tVideoStreamInfo[Index]->wPayStart;
							pDecoder->tVideoStreamInfo[Index]->dwBlockLeft     = pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize;
							pDecoder->tVideoStreamInfo[Index]->dwPayloadLeft = pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tVideoStreamInfo[Index]->dwBlockLeft;
							pDecoder->tVideoStreamInfo[Index]->dwFrameLeft = pDecoder->tVideoStreamInfo[Index]->dwFrameSize;
						} 
						else 
						{   // old frame cont....
							if ( pDecoder->tVideoStreamInfo[Index]->cbUsed ==0)
							{
								pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
								break;
							}

							if (pDecoder->tVideoStreamInfo[Index]->cbUsed == pDecoder->tVideoStreamInfo[Index]->payload.cbObjectOffset) 
							{ // Packet cont...				
								// store payload
								pDecoder->tVideoStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tVideoStreamInfo[Index]->wPayStart;
								pDecoder->tVideoStreamInfo[Index]->dwBlockLeft     = pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize;
								pDecoder->tVideoStreamInfo[Index]->dwPayloadLeft = pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tVideoStreamInfo[Index]->dwBlockLeft;

								pDecoder->tVideoStreamInfo[Index]->cbUsed += pDecoder->tVideoStreamInfo[Index]->payload.cbPayloadSize;
							}
							else // Broken packet
							{
								pDecoder->tVideoStreamInfo[Index]->cbUsed = 0;
								pDecoder->tVideoStreamInfo[Index]->dwFrameLeft =0;
							}
						}
                
					}


					pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodeLoopStart ;//csWMCDecodePayloadEnd;
				//	break;
					return WMCDec_Succeeded;

				case csWMCDecodePayloadEnd:
					pDecoder->tVideoStreamInfo[Index]->iPayload++;
					pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadStart;
					break;

				case csWMCDecodeCompressedPayload: // Added by Amit to take care of compressed payloads
					// In this case store payload in buffer and dont return. Return only after one packet is finished
					
					if((pDecoder->tVideoStreamInfo[Index]->bWantOutput == TRUE_WMC) &&(pDecoder->tVideoStreamInfo[Index]->bNowStopReadingAndDecoding == FALSE_WMC))
					{
						if (!(pDecoder->tVideoStreamInfo[Index]->bFirst)) 
						{
							if (pDecoder->tVideoStreamInfo[Index]->payload.bIsKeyFrame == 0) 
							{
								pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
								break;
							}
							pDecoder->tVideoStreamInfo[Index]->bFirst = 1;
						
						}
                    
//						if(pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp == 0)
							pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = (U32_WMC) pDecoder->tVideoStreamInfo[Index]->payload.msObjectPres;
						pDecoder->tVideoStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tVideoStreamInfo[Index]->wPayStart;
						pDecoder->tVideoStreamInfo[Index]->cbUsed = 0;
						pDecoder->tVideoStreamInfo[Index]->cbFrame = pDecoder->tVideoStreamInfo[Index]->dwNbFrames;
						pDecoder->tVideoStreamInfo[Index]->dwFrameSize = 0; 
						pDecoder->tVideoStreamInfo[Index]->bIsKeyFrame = pDecoder->tVideoStreamInfo[Index]->payload.bIsKeyFrame;

						pDecoder->tVideoStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tVideoStreamInfo[Index]->wPayStart;

//						pDecoder->tAudioStreamInfo[Index]->dwBlockLeft     = pDecoder->tAudioStreamInfo[Index]->nBlockAlign;
//						pDecoder->tAudioStreamInfo[Index]->dwPayloadLeft = pDecoder->tAudioStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
						pDecoder->tVideoStreamInfo[Index]->payload.wBytesRead =0;
						pDecoder->tVideoStreamInfo[Index]->payload.bSubPayloadState =1;
					
						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hDecoder, pDecoder->tVideoStreamInfo[Index]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							return WMCDec_BufferTooSmall;
						}
						
						
						pDecoder->tVideoStreamInfo[Index]->dwFrameSize = pData[0];

					}
					
					pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCDecodeLoopStart;//csWMCDecodePayloadEnd;
				//	break;
					return WMCDec_Succeeded;

//				default:

			}
		}while(1);

		break;

	case Binary_WMC:
		do
		{
			switch(pDecoder->tBinaryStreamInfo[Index]->parse_state)
			{
				case csWMCNewAsfPacket:

					if(pDecoder->tBinaryStreamInfo[Index]->cbNextPacketOffset > pDecoder->tHeaderInfo.cbLastPacketOffset)
					{
						pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCEnd;
						return WMCDec_DecodeComplete;
					}

					pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset = pDecoder->tBinaryStreamInfo[Index]->cbNextPacketOffset;
					pDecoder->tBinaryStreamInfo[Index]->cbNextPacketOffset += pDecoder->tHeaderInfo.cbPacketSize;

					err = WMCDecParseVirtualPacketHeader( hDecoder, (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset, &(pDecoder->tBinaryStreamInfo[Index]->ppex));

					if(err != WMCDec_Succeeded)
					{
						return WMCDec_BadPacketHeader;
					}

					if (pDecoder->tBinaryStreamInfo[Index]->ppex.fEccPresent && pDecoder->tBinaryStreamInfo[Index]->ppex.fParityPacket)
					{
						break;
					}

					pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodePayloadStart;
					pDecoder->tBinaryStreamInfo[Index]->iPayload = 0;
					break;

				case csWMCDecodePayloadStart:

					if (pDecoder->tBinaryStreamInfo[Index]->iPayload >= pDecoder->tBinaryStreamInfo[Index]->ppex.cPayloads)
					{
						pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCNewAsfPacket;
						break;//return WMCDec_Succeeded; // Return after taking full packet
					}

					err = WMCDecParseVirtualPayloadHeader(hDecoder, (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset, &(pDecoder->tBinaryStreamInfo[Index]->ppex), &(pDecoder->tBinaryStreamInfo[Index]->payload));
					if(err != WMCDec_Succeeded)
					{
						pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
						break;
					}

					err = WMCDecGetMediaType (hDecoder,  pDecoder->tBinaryStreamInfo[Index]->payload.bStreamId, &LocalMediaType);
					if (err != WMCDec_Succeeded)
					{
						pDecoder->parse_state = csWMCDecodePayloadEnd;
						break;
					}
					if ((LocalMediaType != Binary_WMC)|| (pDecoder->tBinaryStreamInfo[Index]->wStreamId != pDecoder->tBinaryStreamInfo[Index]->payload.bStreamId))
					{
						pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
						break;
					}
					pDecoder->tBinaryStreamInfo[Index]->wPayStart = pDecoder->tBinaryStreamInfo[Index]->payload.cbPacketOffset + pDecoder->tBinaryStreamInfo[Index]->payload.cbTotalSize  - pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize;

					if( pDecoder->tBinaryStreamInfo[Index]->payload.cbRepData != 1 )
					{
						pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodePayload;
						pDecoder->tBinaryStreamInfo[Index]->payload.bIsCompressedPayload =0; // Amit to take care of compressed payloads.
						break;
					}
					else if( pDecoder->tBinaryStreamInfo[Index]->payload.cbRepData == 1)    // Amit to take care of compressed payloads.
					{
						pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodeCompressedPayload;
						pDecoder->tBinaryStreamInfo[Index]->payload.bIsCompressedPayload =1;
						break;
					}
 
				case csWMCDecodePayload:
		// In this case store payload in buffer and dont return. Return only after one packet is finished

					if((pDecoder->tBinaryStreamInfo[Index]->bWantOutput == TRUE_WMC) &&(pDecoder->tBinaryStreamInfo[Index]->bNowStopReadingAndDecoding == FALSE_WMC))
					{
						pDecoder->tBinaryStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tBinaryStreamInfo[Index]->wPayStart;
//						if(pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp == 0)
							pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp = (U32_WMC) pDecoder->tBinaryStreamInfo[Index]->payload.msObjectPres;

						// Keep data about frame size, offset, frame used etc... 
						if (0 == pDecoder->tBinaryStreamInfo[Index]->payload.cbObjectOffset) 
						{ // New Frame starts

							
							pDecoder->tBinaryStreamInfo[Index]->cbUsed = pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize;
							pDecoder->tBinaryStreamInfo[Index]->cbFrame = pDecoder->tBinaryStreamInfo[Index]->dwNbFrames;
							pDecoder->tBinaryStreamInfo[Index]->dwNbFrames += 1;
							pDecoder->tBinaryStreamInfo[Index]->dwFrameSize = pDecoder->tBinaryStreamInfo[Index]->payload.cbObjectSize; 
							pDecoder->tBinaryStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tBinaryStreamInfo[Index]->wPayStart;
							pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft     = pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize;
							pDecoder->tBinaryStreamInfo[Index]->dwPayloadLeft = pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft;
							pDecoder->tBinaryStreamInfo[Index]->dwFrameLeft = pDecoder->tBinaryStreamInfo[Index]->dwFrameSize;
						} 
						else 
						{   // old frame cont....
							if ( pDecoder->tBinaryStreamInfo[Index]->cbUsed ==0)
							{
								pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodePayloadEnd;
								break;
							}

							if (pDecoder->tBinaryStreamInfo[Index]->cbUsed == pDecoder->tBinaryStreamInfo[Index]->payload.cbObjectOffset) 
							{ // Packet cont...				
								pDecoder->tBinaryStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tBinaryStreamInfo[Index]->wPayStart;
								pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft     = pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize;
								pDecoder->tBinaryStreamInfo[Index]->dwPayloadLeft = pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft;
								pDecoder->tBinaryStreamInfo[Index]->cbUsed += pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize;
							}
							else // Broken packet
							{
								pDecoder->tBinaryStreamInfo[Index]->cbUsed = 0;
								pDecoder->tBinaryStreamInfo[Index]->dwFrameLeft =0;
							}
						}
                
					}
            
     				pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodeLoopStart;//csWMCDecodePayloadEnd;
			//		break;
		            return WMCDec_Succeeded;

				case csWMCDecodePayloadEnd:
					pDecoder->tBinaryStreamInfo[Index]->iPayload++;
					pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodePayloadStart;
					break;

				case csWMCDecodeCompressedPayload: // Added by Amit to take care of compressed payloads
					// In this case store payload in buffer and dont return. Return only after one packet is finished
            
					if((pDecoder->tBinaryStreamInfo[Index]->bWantOutput == TRUE_WMC) &&(pDecoder->tBinaryStreamInfo[Index]->bNowStopReadingAndDecoding == FALSE_WMC))
					{
//						if(pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp == 0)
							pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp = (U32_WMC) pDecoder->tBinaryStreamInfo[Index]->payload.msObjectPres;
						pDecoder->tBinaryStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tBinaryStreamInfo[Index]->wPayStart;
						pDecoder->tBinaryStreamInfo[Index]->cbUsed = 0;
						pDecoder->tBinaryStreamInfo[Index]->cbFrame = pDecoder->tBinaryStreamInfo[Index]->dwNbFrames;
						pDecoder->tBinaryStreamInfo[Index]->dwFrameSize = 0; 

						pDecoder->tBinaryStreamInfo[Index]->dwPayloadOffset = (U32_WMC)pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset + pDecoder->tBinaryStreamInfo[Index]->wPayStart;

//						pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft     = pDecoder->tBinaryStreamInfo[Index]->nBlockAlign;
//						pDecoder->tBinaryStreamInfo[Index]->dwPayloadLeft = pDecoder->tBinaryStreamInfo[Index]->payload.cbPayloadSize - pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft;
						pDecoder->tBinaryStreamInfo[Index]->payload.wBytesRead =0;
						pDecoder->tBinaryStreamInfo[Index]->payload.bSubPayloadState =1;

						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hDecoder, pDecoder->tBinaryStreamInfo[Index]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							return WMCDec_BufferTooSmall;
						}
						
						
						pDecoder->tBinaryStreamInfo[Index]->dwFrameSize = pData[0];
                
					}
     				pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCDecodeLoopStart;//csWMCDecodePayloadEnd;
			//		break;
		            return WMCDec_Succeeded;

	//            default:
			}
        
		}while(1);
		break;
	}

    return err;
}


/*********************************************************************************************************/

tWMCDecStatus WMCDecDecodeFrameX( HWMCDECODER hDecoder, U16_WMC * pbDecoded, U32_WMC* pnNumberOfSamples,tMediaType_WMC MediaType, U16_WMC Index )
{
	tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder;
    tWMVDecodeStatus  VidStatus;

    WMARESULT wmar = WMA_OK;
#ifndef __NO_SPEECH__
    WMSRESULT wmsr = WMS_OK;
#endif

    *pnNumberOfSamples =0;
    
    if (NULL == hDecoder)
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *) (hDecoder);

    switch(MediaType)
    {
    case Audio_WMC:
        if (pDecoder->tAudioStreamInfo[Index]->parse_state != csWMCEnd)
        {
            pDecoder->i32CurrentAudioStreamIndex = Index;
		   
            if (pDecoder->tAudioStreamInfo[Index]->bTobeDecoded == TRUE_WMC)
            {
        
                if (pDecoder->tAudioStreamInfo[Index]->nVersion >0 && pDecoder->tAudioStreamInfo[Index]->nVersion <4)
                {
                    if (pDecoder->tAudioStreamInfo[Index]->wmar != WMA_OK)
				    {
					    if (pDecoder->tAudioStreamInfo[Index]->wmar > 0)
					    {
                            if (pDecoder->tAudioStreamInfo[Index]->wmar == WMA_S_NO_MORE_FRAME)
                            {
                                wmar = WMARawDecStatus (pDecoder->tAudioStreamInfo[Index]->hMSA);
                            }
					    }
						else
					    {
						    WMARawDecReset (pDecoder->tAudioStreamInfo[Index]->hMSA);
						    pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
						    pDecoder->tAudioStreamInfo[Index]->dwBlockLeft =0;
						    wmar = WMARawDecStatus (pDecoder->tAudioStreamInfo[Index]->hMSA);
					    }
				    }
                    *pnNumberOfSamples = 8000;
                    wmar = WMARawDecDecodeData (pDecoder->tAudioStreamInfo[Index]->hMSA, (U32_WMARawDec *)pnNumberOfSamples
#ifdef _V9WMADEC_
                        , (I32_WMARawDec *)0
#endif
                        );
                    if (wmar == WMA_E_BROKEN_FRAME) 
				    {
					    WMARawDecReset (pDecoder->tAudioStreamInfo[Index]->hMSA);
					    pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
					    pDecoder->tAudioStreamInfo[Index]->dwBlockLeft =0;
					    wmar = WMARawDecStatus(pDecoder->tAudioStreamInfo[Index]->hMSA);
					    wmar = WMA_OK;
                    }

				    if (wmar == WMA_E_ONHOLD) 
				    {
					    wmar = WMA_OK;
                    }
                    if (wmar == WMA_E_LOSTPACKET) 
				    {
					    wmar = WMA_OK;
                    }
    
                    *pbDecoded = pDecoder->tAudioStreamInfo[Index]->wStreamId;
                    pDecoder->tAudioStreamInfo[Index]->wmar = wmar;
                }
#ifndef __NO_SPEECH__
                else if(pDecoder->tAudioStreamInfo[Index]->nVersion ==4 )//Speech Codec
                {
/*                       if (pDecoder->tAudioStreamInfo[Index]->wmsr != WMS_OK)
				    {
					    if (pDecoder->tAudioStreamInfo[Index]->wmsr > 0)
					    {
                            if (pDecoder->tAudioStreamInfo[Index]->wmsr == WMS_S_NO_MORE_FRAME)
                            {
                                wmsr = WMSRawDecStatus (pDecoder->tAudioStreamInfo[Index]->hMSA);
                            }
					    }
						else
					    {
						    WMSRawDecReset (pDecoder->tAudioStreamInfo[Index]->hMSA);
						    pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
						    pDecoder->tAudioStreamInfo[Index]->dwBlockLeft =0;
						    wmsr = WMSRawDecStatus (pDecoder->tAudioStreamInfo[Index]->hMSA);
					    }
				    }
*/                  
					*pnNumberOfSamples = 0;
                    wmsr = WMSRawDecDecodeData (pDecoder->tAudioStreamInfo[Index]->hMSA, (U32_WMARawDec *)pnNumberOfSamples);
                    if (wmsr == WMS_E_BROKEN_FRAME) 
				    {
					    WMSRawDecReset (pDecoder->tAudioStreamInfo[Index]->hMSA);
					    pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
					    pDecoder->tAudioStreamInfo[Index]->dwBlockLeft =0;
					    wmsr = WMARawDecStatus(pDecoder->tAudioStreamInfo[Index]->hMSA);
					    wmsr = WMS_OK;
                    }

				    if (wmsr == WMS_E_ONHOLD) 
				    {
					    wmsr = WMS_OK;
                    }
                    if (wmsr == WMS_E_LOSTPACKET) 
				    {
					    wmsr = WMS_OK;
                    }
    
                    *pbDecoded = pDecoder->tAudioStreamInfo[Index]->wStreamId;
//                        pDecoder->tAudioStreamInfo[Index]->wmsr = wmsr;
                }
#endif
            }
            else
            {
                *pbDecoded = pDecoder->tAudioStreamInfo[Index]->wStreamId;
//            	memcpy (pnNumberOfSamples,(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));

				if (pDecoder->tAudioStreamInfo[Index]->bOutputisReady == TRUE_WMC) // Last output is not taken out
				{
					//Fix buffers
					U8_WMC TempOutBuff[1024];
					U32_WMC cbBuffer=0;
					do
					{
						WMCRawGetData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
					}while(cbBuffer>0);
				
				}
				
				pDecoder->tAudioStreamInfo[Index]->bOutputisReady = TRUE_WMC;
				pDecoder->tAudioStreamInfo[Index]->bGotCompOutput = FALSE_WMC;

                *pnNumberOfSamples =pDecoder->tAudioStreamInfo[Index]->dwFrameSize;  
            }
        }
        else
        {
            *pbDecoded = 0xff;
			pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp = (Double_WMC)0xffffffff;
			pDecoder->tAudioStreamInfo[Index]->dwAudPayloadPresTime = 0xffffffff;

        }
        break;
    case Video_WMC:
        if (pDecoder->tVideoStreamInfo[Index]->parse_state != csWMCEnd)
        {
            pDecoder->i32CurrentVideoStreamIndex = Index;
			if (pDecoder->tVideoStreamInfo[Index]->bTobeDecoded == TRUE_WMC)
            {
                if ((pDecoder->bSkipToNextI) && (pDecoder->tVideoStreamInfo[Index]->payload.bIsKeyFrame)) {
                        pDecoder->bSkipToNextI = FALSE_WMC;
                };
		                        
                if (pDecoder->bSkipToNextI) {
                    Bool_WMV bNotEndOfFrame = TRUE_WMV;
                    U8_WMV * pchBuffer = NULL;
                    U32_WMV uintActualBufferLength = 0;
                    while ((bNotEndOfFrame == TRUE_WMV)) {
                        WMVDecCBGetData ( (U32_WMV) pDecoder, 0,
                            &pchBuffer,
                            4,
                            &uintActualBufferLength,
                            &bNotEndOfFrame );
                        if ((pchBuffer == NULL) || (uintActualBufferLength ==0) )
                            bNotEndOfFrame = FALSE_WMV;
                    }
                    *pnNumberOfSamples = 0;
                    *pbDecoded = pDecoder->tVideoStreamInfo[Index]->wStreamId;
                    if (pDecoder->tVideoStreamInfo[Index]->parse_state == csWMCEnd)
                    {
                        *pbDecoded =0xff;
                        pDecoder->tVideoStreamInfo[Index]->dwNextVideoTimeStamp = 0xffffffff;
                        pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = 0xffffffff;
                        return WMCDec_Succeeded;
                    }
                }
                else 
                {

                    if(
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_WMV2 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_WMV1 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_WMS2 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_WMS1 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_M4S2 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_MP4S ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_MP43 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_MP42 ||
                        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_MPG4 )
                    
                    {    
                    
                    
                        VidStatus = WMVideoDecDecodeData ((HWMVDecoder)pDecoder->tVideoStreamInfo[Index]->pVDec, (U16_WMV *)pnNumberOfSamples);
                        if (WMV_Succeeded == VidStatus)
                        {
                            *pbDecoded = pDecoder->tVideoStreamInfo[Index]->wStreamId;
                            *pnNumberOfSamples =1;
                        }
                        else
                        {
                            if (pDecoder->tVideoStreamInfo[Index]->parse_state == csWMCEnd)
							{
								*pbDecoded =0xff;
								pDecoder->tVideoStreamInfo[Index]->dwNextVideoTimeStamp = 0xffffffff;
								pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = 0xffffffff;
								return WMCDec_Succeeded;
							}
										

							//WMVideoDecReset((HWMVDecoder)pDecoder->tVideoStreamInfo[Index]->pVDec);
						    pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded += pDecoder->tVideoStreamInfo[Index]->dwBlockLeft;
						    pDecoder->tVideoStreamInfo[Index]->dwBlockLeft =0;
							*pbDecoded = pDecoder->tVideoStreamInfo[Index]->wStreamId;
							switch(VidStatus)
							{
							case WMV_Failed :
								return WMVDec_Failed;
							case WMV_BadMemory:
								return WMVDec_BadMemory;
							case WMV_NoKeyFrameDecoded:
								return WMVDec_NoKeyFrameDecoded;
							case WMV_CorruptedBits:
								return WMVDec_CorruptedBits;
							case WMV_BrokenFrame:
								return WMVDec_BrokenFrame;
							default:
								return WMVDec_Failed; 
							}
                        }
                    }
#ifndef __NO_SCREEN__
                    else if(pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_MSS1 ||
					        pDecoder->tVideoStreamInfo[Index]->biCompression == FOURCC_MSS2)
                    {
                        
                        tWMCDecStatus TempStatus;
                        TempStatus = WMScDecodeData (hDecoder, Index);
                        if (WMCDec_Succeeded == TempStatus)
                        {
                            *pbDecoded = pDecoder->tVideoStreamInfo[Index]->wStreamId;
                            *pnNumberOfSamples =1;
                        }
                        else
                        {
                            if (pDecoder->tVideoStreamInfo[Index]->parse_state == csWMCEnd)
							{
								*pbDecoded =0xfe;
								pDecoder->tVideoStreamInfo[Index]->dwNextVideoTimeStamp = 0xffffffff;
								pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = 0xffffffff;
								return WMCDec_Succeeded;
							}
                        
                            *pbDecoded = pDecoder->tVideoStreamInfo[Index]->wStreamId;
						    pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded += pDecoder->tVideoStreamInfo[Index]->dwBlockLeft;
						    pDecoder->tVideoStreamInfo[Index]->dwBlockLeft =0;
                            return WMVDec_Failed; 
                        }
                    
                    }
#endif
                }
            }
            else
            {
                *pbDecoded = pDecoder->tVideoStreamInfo[Index]->wStreamId;
				if (pDecoder->tVideoStreamInfo[Index]->bOutputisReady == TRUE_WMC) // Last output is not taken out
				{
					//Fix buffers
					U8_WMC TempOutBuff[1024];
					U32_WMC cbBuffer=0;
					do
					{
						WMCRawGetVideoData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
					}while(cbBuffer>0);
				
				}
				
				pDecoder->tVideoStreamInfo[Index]->bGotCompOutput = FALSE_WMC;
				pDecoder->tVideoStreamInfo[Index]->bOutputisReady = TRUE_WMC;

				//                   *pnNumberOfSamples =1;  // To be changed. Should be no of bytes
//		        memcpy (pnNumberOfSamples,(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded + 2*sizeof(U32_WMC) ), sizeof(U32_WMC));
                *pnNumberOfSamples = pDecoder->tVideoStreamInfo[Index]->dwFrameSize;//pDecoder->tVideoStreamInfo[Index]->payload.cbObjectSize;
            }


        }
        else
        {
            *pbDecoded = 0xff;
			pDecoder->tVideoStreamInfo[Index]->dwNextVideoTimeStamp = 0xffffffff;
			pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = 0xffffffff;
        }
        
		pDecoder->tVideoStreamInfo[Index]->bHasGivenAnyOutput = TRUE_WMC;
//		pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp = dwMinVideoPresTime;
        break;
    case Binary_WMC:
        if (pDecoder->tBinaryStreamInfo[Index]->parse_state != csWMCEnd)
        {
            *pbDecoded = pDecoder->tBinaryStreamInfo[Index]->wStreamId;
            pDecoder->i32CurrentBinaryStreamIndex = Index;
//                *pnNumberOfSamples =1;  // To be changed. Should be no of bytes
//		    memcpy (pnNumberOfSamples,(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));

			if (pDecoder->tBinaryStreamInfo[Index]->bOutputisReady == TRUE_WMC) // Last output is not taken out
			{
				//Fix buffers
				U8_WMC TempOutBuff[1024];
				U32_WMC cbBuffer=0;
				do
				{
					WMCRawGetBinaryData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
				}while(cbBuffer>0);
			
			}
			
			
			pDecoder->tBinaryStreamInfo[Index]->bGotCompOutput = FALSE_WMC;
			pDecoder->tBinaryStreamInfo[Index]->bOutputisReady = TRUE_WMC;
            *pnNumberOfSamples = pDecoder->tBinaryStreamInfo[Index]->dwFrameSize;

        }
        else
        {
			pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp = 0xffffffff;
            *pbDecoded = 0xff;
        }
		pDecoder->tBinaryStreamInfo[Index]->bHasGivenAnyOutput = TRUE_WMC;
        break;
    default:
        *pbDecoded = 0xff;        
    }
    return WMCDec_Succeeded;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



tWMCDecStatus WMCDecGetStreamToSpecificPosition (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn)
{
	U64_WMC cbPacketOffset =0;
    U8_WMC i=0, j=0;
	tWMCDecStatus rc = WMCDec_Succeeded;
    U32_WMC dwToIndexEntry =0;
    WMCINDEXENTRIES* pTempIndexEntry = NULL_WMC;
    U32_WMC dwPacketToGo =0;
    U32_WMC dwNewPacketToGo =0;
    tMediaType_WMC MediaType;
	U32_WMC cbCurrentPacketOffset =0;
	PACKET_PARSE_INFO_EX ParseInfoEx;
	PAYLOAD_MAP_ENTRY_EX Payload;
	U32_WMC iPayload =0;
	U64_WMC u64LocalTimeRequest =0;
	U64_WMC u64LastPresTime = 0;
	Bool_WMC bFirst = FALSE_WMC;
	Bool_WMC bDone = FALSE_WMC;
	U16_WMC Index = 0;
	Bool_WMC bGotIndex = FALSE_WMC;


    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC || pu64TimeReturn == NULL_WMC)
        return WMCDec_InValidArguments;


	rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) wStreamId, &MediaType);
	if (WMCDec_Succeeded !=rc)
		return WMCDec_InValidArguments;
	
	
	switch(MediaType)
	{
	case Audio_WMC:
		for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
		{
			if( pDecoder->tAudioStreamInfo[j]->wStreamId == wStreamId)
			{
				Index =j;
				bGotIndex = TRUE_WMC;
			}
		}
		break;
	case Video_WMC:
		for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
		{
			if(pDecoder->tVideoStreamInfo[j]->wStreamId == wStreamId)
			{
				Index = j;
				bGotIndex = TRUE_WMC;
			}

		}
		break;
	case Binary_WMC:
		for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
		{
			if(pDecoder->tBinaryStreamInfo[j]->wStreamId == wStreamId)
			{
				Index = j;
				bGotIndex = TRUE_WMC;
			}
		}
		break;
	default:
		return WMCDec_InValidArguments;
	}

	if (bGotIndex == FALSE_WMC )
		return WMCDec_InValidArguments;


	memset(&ParseInfoEx, 0, sizeof (PACKET_PARSE_INFO_EX));
	memset(&Payload, 0, sizeof (PAYLOAD_MAP_ENTRY_EX));

	*pu64TimeReturn = 0;
	
	u64TimeRequest= u64TimeRequest + (U64_WMC)pDecoder->tHeaderInfo.msPreroll ;
    if (pDecoder->cTotalIndexs >0)
    {
		for(i=0; i< pDecoder->cTotalIndexs ; i++)
		{
			if (pDecoder->tIndexInfo[i].nStreamId == wStreamId)
			{
				rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) wStreamId, &MediaType);
				if (WMCDec_Succeeded !=rc)
					return rc;
 
				switch(MediaType)
				{
				case Audio_WMC:
					for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
					{
					
						if( pDecoder->tIndexInfo[i].nStreamId == wStreamId)
						{
                    
							u64LocalTimeRequest = u64TimeRequest;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
						}
					}
					break;
				case Video_WMC:
					for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
					{
						if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
						{
							u64LocalTimeRequest = u64TimeRequest /*+(U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
						}
        
					}
					break;
				case Binary_WMC:
					for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
					{
						if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
						{
							u64LocalTimeRequest = u64TimeRequest;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
						}
					}
					break;
				default:
					return WMCDec_InValidArguments;

				}
			}
		}
		
		if (dwPacketToGo >0)
			dwPacketToGo-=1;
    }


	do
	
	{
		U32_WMC dwCompPresTime =0;
	    U8_WMC *pData = NULL;
		U32_WMC dwOffset =0;
		U8_WMC bNextSubPayloadSize=0;
		U16_WMC wBytesRead=0;
		U32_WMC cbActual =0;
		
		cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);
		rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);


		for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
		{
        
			rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
			if (rc != WMCDec_Succeeded)
				return rc;

			if (wStreamId == Payload.bStreamId)
			{
				if (bFirst == FALSE_WMC)
				{
					u64LastPresTime = (U64_WMC) Payload.msObjectPres;
					bFirst = TRUE_WMC;
				}
				
				
				if (Payload.cbRepData == 1)
				{
				
					dwOffset = Payload.cbPacketOffset + Payload.cbTotalSize - Payload.cbPayloadSize;
				    cbActual = WMCDecCBGetData(hWMCDec, cbCurrentPacketOffset + dwOffset, Payload.cbPayloadSize, &pData, pDecoder->u32UserData);

					dwCompPresTime = Payload.msObjectPres;
					bNextSubPayloadSize = pData [0];
					pData +=1;
//					if (pPay->wTotalDataBytes < bNextSubPayloadSize)
//						return WMPERR_FAIL;

					if( Payload.wTotalDataBytes > bNextSubPayloadSize)
						wBytesRead = bNextSubPayloadSize+1;
					else if (Payload.wTotalDataBytes == bNextSubPayloadSize)
						wBytesRead = bNextSubPayloadSize;
										
					while (Payload.wTotalDataBytes >= wBytesRead) 
					{

						if (dwCompPresTime >= u64TimeRequest)
						{
							*pu64TimeReturn = u64LastPresTime - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
							bDone = TRUE_WMC;
							break;
						}
						else
						{
							u64LastPresTime = (U64_WMC) dwCompPresTime;
						//	continue;
						}
						pData += bNextSubPayloadSize;
						bNextSubPayloadSize = pData[0];
						wBytesRead+=bNextSubPayloadSize+1;
						pData+=1;
						dwCompPresTime+=Payload.dwDeltaPresTime;
					}
					
					if (bDone == TRUE_WMC)
						break;
				}				
				else
				{
					if (Payload.msObjectPres >= u64TimeRequest)
					{
						*pu64TimeReturn = u64LastPresTime - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
						bDone = TRUE_WMC;
						break;
					}
					else
					{
						u64LastPresTime = (U64_WMC) Payload.msObjectPres;
						continue;
					}
				}
			}

		}
		dwPacketToGo++;
	}while((bDone == FALSE_WMC) && (dwNewPacketToGo <=pDecoder->tHeaderInfo.cPackets));


	if (bDone == TRUE_WMC)
	{

		switch(MediaType)
		{
		case Audio_WMC:

//			pDecoder->tAudioStreamInfo[Index]->cbPacketOffset = cbCurrentPacketOffset;
			pDecoder->tAudioStreamInfo[Index]->cbCurrentPacketOffset = cbCurrentPacketOffset;
			pDecoder->tAudioStreamInfo[Index]->cbNextPacketOffset = cbCurrentPacketOffset;
			pDecoder->tAudioStreamInfo[Index]->parse_state = csWMCNewAsfPacket;
			break;
		case Video_WMC:
//			pDecoder->tVideoStreamInfo[Index]->cbPacketOffset = cbCurrentPacketOffset;
			pDecoder->tVideoStreamInfo[Index]->cbCurrentPacketOffset = cbCurrentPacketOffset;
			pDecoder->tVideoStreamInfo[Index]->cbNextPacketOffset = cbCurrentPacketOffset;
			pDecoder->tVideoStreamInfo[Index]->parse_state = csWMCNewAsfPacket;
			break;
		case Binary_WMC:
//			pDecoder->tBinaryStreamInfo[Index]->cbPacketOffset = cbCurrentPacketOffset;
			pDecoder->tBinaryStreamInfo[Index]->cbCurrentPacketOffset = cbCurrentPacketOffset;
			pDecoder->tBinaryStreamInfo[Index]->cbNextPacketOffset = cbCurrentPacketOffset;
			pDecoder->tBinaryStreamInfo[Index]->parse_state = csWMCNewAsfPacket;
			break;
		default:
			return WMCDec_InValidArguments;
		}
	}
	
	return rc;

}









#endif
