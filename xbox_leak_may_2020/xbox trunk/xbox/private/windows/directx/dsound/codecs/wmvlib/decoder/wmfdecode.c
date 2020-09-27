#include "bldsetup.h"

#if !defined(_Embedded_x86) && !defined(HITACHI)
#ifdef macintosh
#include <MacTypes.h>
#include <Errors.h>
#include <Endian.h>
#include <MacMemory.h>
#define BIGENDIAN
#elif defined(_XBOX)
#include <xtl.h>
#else
#include <windows.h>
#include <memory.h>
#include <mmsystem.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wmcdecode.h"
#ifdef _XBOX
#include "wmf.h"
#include "wmfdec.h"
#endif
#if defined(_Embedded_x86) || defined(_XBOX)
#define BYTE U8_WMC
#define VOID void
#define DWORD U32_WMC
#define HRESULT long
//typedef Bool_WMC BOOL;
#endif // linux
#ifdef UNDER_CE
#define assert ASSERT
#else
#include <assert.h>
#endif
#define NO_FRAMERATE

#ifdef HITACHI
#include "wmmacsp.h"
#endif

#include "wmfparse.h"

#ifndef _XBOX
#include "wmf.h"
#include "wmfdec.h"
#endif
#include "wmfparse.h"
#ifndef __NO_SCREEN__
#include "wmscreendec_api.h"
#endif

#ifndef __NO_SPEECH__
#include "wmsdec_api.h"
#endif

#ifndef _ASFPARSE_ONLY_
#include "wmvdec_api.h"
#else
typedef enum tagWMVDecodeStatus
{
    WMV_Succeeded = 0,
    WMV_Failed,
    WMV_BadMemory,
    WMV_NoKeyFrameDecoded,
    WMV_CorruptedBits,
    WMV_UnSupportedOutputPixelFormat,
    WMV_UnSupportedCompressedFormat,
    WMV_InValidArguments,
    WMV_BadSource,

    WMV_NoMoreOutput,
    WMV_EndOfFrame,
    WMV_BrokenFrame
} tWMVDecodeStatus;

#endif

#define MIN_HEADER_SIZE     0x80
#define NORMAL_HEADER_SIZE  0x8000
#define WMFDEMO_TIME        60000


#if( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

typedef struct tbitmapPalette
{
	BITMAPINFOHEADER	BitmapHdr;
    U8_WMC              bPlaette[1024];
}BITMAPPALETTE;

#ifndef __NO_SPEECH__
//BOOL InitNSPLibWMSP(ULONG fdwReason);
#include "intelFFT.h"
int MyInitNSP(int fdwReason);
#endif

const U8_WMC StandardPalette[3*256] =

    // These are the first ten standard VGA colours WARNING RGBQUAD defines
    // the fields in BGR ordering NOT RGB ! The odd looking entries further
    // down are entered to ensure that we get an identity palette with GDI
    // If we entered an all zero palette entry for example it would be taken
    // out and GDI would use a slow internal mapping table to generate it

    {   0,   0,   0 ,     // 0 Sys Black
       0,   0, 128 ,     // 1 Sys Dk Red
       0, 128,   0 ,     // 2 Sys Dk Green
       0, 128, 128 ,     // 3 Sys Dk Yellow
     128,   0,   0 ,     // 4 Sys Dk Blue
     128,   0, 128 ,     // 5 Sys Dk Violet
     128, 128,   0 ,     // 6 Sys Dk Cyan
     192, 192, 192 ,     // 7 Sys Lt Grey
     192, 220, 192 ,     // 8 Sys 8
     240, 202, 166 ,     // 9 Sys 9

       1,   1,   1 ,
       1,   1,  51 ,
       1,   1, 102 ,
       1,   1, 153 ,
       1,   1, 204 ,
       1,   1, 254 ,
       1,  51,   1 ,
       1,  51,  51 ,
       1,  51, 102 ,
       1,  51, 153 ,
       1,  51, 204 ,
       1,  51, 254 ,
       1, 102,   1 ,
       1, 102,  51 ,
       1, 102, 102 ,
       1, 102, 153 ,
       1, 102, 204 ,
       1, 102, 254 ,
       1, 153,   1 ,
       1, 153,  51 ,
       1, 153, 102 ,
       1, 153, 153 ,
       1, 153, 204 ,
       1, 153, 254 ,
       1, 204,   1 ,
       1, 204,  51 ,
       1, 204, 102 ,
       1, 204, 153 ,
       1, 204, 204 ,
       1, 204, 254 ,
       1, 254,   1 ,
       1, 254,  51 ,
       1, 254, 102 ,
       1, 254, 153 ,
       1, 254, 204 ,
       1, 254, 254 ,

      51,   1,   1 ,
      51,   1,  51 ,
      51,   1, 102 ,
      51,   1, 153 ,
      51,   1, 204 ,
      51,   1, 254 ,
      51,  51,   1 ,
      51,  51,  51 ,
      51,  51, 102 ,
      51,  51, 153 ,
      51,  51, 204 ,
      51,  51, 254 ,
      51, 102,   1 ,
      51, 102,  51 ,
      51, 102, 102 ,
      51, 102, 153 ,
      51, 102, 204 ,
      51, 102, 254 ,
      51, 153,   1 ,
      51, 153,  51 ,
      51, 153, 102 ,
      51, 153, 153 ,
      51, 153, 204 ,
      51, 153, 254 ,
      51, 204,   1 ,
      51, 204,  51 ,
      51, 204, 102 ,
      51, 204, 153 ,
      51, 204, 204 ,
      51, 204, 254 ,
      51, 254,   1 ,
      51, 254,  51 ,
      51, 254, 102 ,
      51, 254, 153 ,
      51, 254, 204 ,
      51, 254, 254 ,

     102,   1,   1 ,
     102,   1,  51 ,
     102,   1, 102 ,
     102,   1, 153 ,
     102,   1, 204 ,
     102,   1, 254 ,
     102,  51,   1 ,
     102,  51,  51 ,
     102,  51, 102 ,
     102,  51, 153 ,
     102,  51, 204 ,
     102,  51, 254 ,
     102, 102,   1 ,
     102, 102,  51 ,
     102, 102, 102 ,
     102, 102, 153 ,
     102, 102, 204 ,
     102, 102, 254 ,
     102, 153,   1 ,
     102, 153,  51 ,
     102, 153, 102 ,
     102, 153, 153 ,
     102, 153, 204 ,
     102, 153, 254 ,
     102, 204,   1 ,
     102, 204,  51 ,
     102, 204, 102 ,
     102, 204, 153 ,
     102, 204, 204 ,
     102, 204, 254 ,
     102, 254,   1 ,
     102, 254,  51 ,
     102, 254, 102 ,
     102, 254, 153 ,
     102, 254, 204 ,
     102, 254, 254 ,

     153,   1,   1 ,
     153,   1,  51 ,
     153,   1, 102 ,
     153,   1, 153 ,
     153,   1, 204 ,
     153,   1, 254 ,
     153,  51,   1 ,
     153,  51,  51 ,
     153,  51, 102 ,
     153,  51, 153 ,
     153,  51, 204 ,
     153,  51, 254 ,
     153, 102,   1 ,
     153, 102,  51 ,
     153, 102, 102 ,
     153, 102, 153 ,
     153, 102, 204 ,
     153, 102, 254 ,
     153, 153,   1 ,
     153, 153,  51 ,
     153, 153, 102 ,
     153, 153, 153 ,
     153, 153, 204 ,
     153, 153, 254 ,
     153, 204,   1 ,
     153, 204,  51 ,
     153, 204, 102 ,
     153, 204, 153 ,
     153, 204, 204 ,
     153, 204, 254 ,
     153, 254,   1 ,
     153, 254,  51 ,
     153, 254, 102 ,
     153, 254, 153 ,
     153, 254, 204 ,
     153, 254, 254 ,

     204,   1,   1 ,
     204,   1,  51 ,
     204,   1, 102 ,
     204,   1, 153 ,
     204,   1, 204 ,
     204,   1, 254 ,
     204,  51,   1 ,
     204,  51,  51 ,
     204,  51, 102 ,
     204,  51, 153 ,
     204,  51, 204 ,
     204,  51, 254 ,
     204, 102,   1 ,
     204, 102,  51 ,
     204, 102, 102 ,
     204, 102, 153 ,
     204, 102, 204 ,
     204, 102, 254 ,
     204, 153,   1 ,
     204, 153,  51 ,
     204, 153, 102 ,
     204, 153, 153 ,
     204, 153, 204 ,
     204, 153, 254 ,
     204, 204,   1 ,
     204, 204,  51 ,
     204, 204, 102 ,
     204, 204, 153 ,
     204, 204, 204 ,
     204, 204, 254 ,
     204, 254,   1 ,
     204, 254,  51 ,
     204, 254, 102 ,
     204, 254, 153 ,
     204, 254, 204 ,
     204, 254, 254 ,

     254,   1,   1 ,
     254,   1,  51 ,
     254,   1, 102 ,
     254,   1, 153 ,
     254,   1, 204 ,
     254,   1, 254 ,
     254,  51,   1 ,
     254,  51,  51 ,
     254,  51, 102 ,
     254,  51, 153 ,
     254,  51, 204 ,
     254,  51, 254 ,
     254, 102,   1 ,
     254, 102,  51 ,
     254, 102, 102 ,
     254, 102, 153 ,
     254, 102, 204 ,
     254, 102, 254 ,
     254, 153,   1 ,
     254, 153,  51 ,
     254, 153, 102 ,
     254, 153, 153 ,
     254, 153, 204 ,
     254, 153, 254 ,
     254, 204,   1 ,
     254, 204,  51 ,
     254, 204, 102 ,
     254, 204, 153 ,
     254, 204, 204 ,
     254, 204, 254 ,
     254, 254,   1 ,
     254, 254,  51 ,
     254, 254, 102 ,
     254, 254, 153 ,
     254, 254, 204 ,
     254, 254, 254 };

int g_iVideoFrameSize = 0;

tWMCDecStatus WMCDecInit_All (HWMCDECODER* phWMCDec, tVideoFormat_WMC tVideoOutputFormat, Bool_WMC bDiscardIndex, U32_WMC u32UserData, I32_WMC i32PostProc);

tWMCDecStatus WMCInitnParseHeader(HWMCDECODER *phDecoder
#ifdef _ZMDRM_
                    ,HDRMMGR *phDRM
                    ,U8_WMC *pAppCert
                    ,U32_WMC Rights
                    ,const U8_WMC *pbHwId
                    ,U32_WMC cbHwId
#endif
                    );

#define SAFEMEMFREE(x) { if (x != NULL_WMC) wmvfree(x); x= NULL_WMC; }	
tWMCDecStatus WMCDecGetMediaType (HWMCDECODER hWMCDec, U16_WMC wStreamId, tMediaType_WMC * pMediaType);
tWMCDecStatus WMCRawGetVideoData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength);
tWMCDecStatus WMCRawGetBinaryData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength);
tWMCDecStatus WMCReadIndex (HWMCDECODER  *hDecoder);
tWMCDecStatus WMCDecStorePayload( HWMCDECODER hDecoder, U32_WMC cbPayloadSize, tMediaType_WMC MediaType, U32_WMC PresTime, I32_WMC cbFrame, U16_WMC Index, Bool_WMC bWriteHeader, Bool_WMC bIsCompressed);
tWMCDecStatus WMCDecParseVirtualPacketHeader(HWMCDECODER hDecoder, U32_WMC cbCurrentPacketOffset, PACKET_PARSE_INFO_EX* pParseInfoEx);
tWMCDecStatus WMCDecParseVirtualPayloadHeader(HWMCDECODER hDecoder, U32_WMC cbCurrentPacketOffset, PACKET_PARSE_INFO_EX *pParseInfoEx, PAYLOAD_MAP_ENTRY_EX *pPayload);
tWMCDecStatus WMCDecGetPreviousFrameTime (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn);
tWMCDecStatus WMCDecUpdateNewPayload(HWMCDECODER hDecoder, tMediaType_WMC MediaType, U16_WMC Index);
tWMCDecStatus WMCDecGetAndDecode (HWMCDECODER hWMCDec, U32_WMC* pnStreamReadyForOutput, U32_WMC* pnNumberOfSamples);
tWMCDecStatus WMCDecGetStreamToSpecificPosition (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn);
#ifndef __NO_SCREEN__
tWMCDecStatus WMScDecodeData (HWMCDECODER hWMCDec, U16_WMC wIndex );
tWMCDecStatus WMScGetOutData (HWMCDECODER hWMCDec, U8_WMC *pu8VideoOutBuffer, U32_WMC u32VideoOutBufferSize, U16_WMC wIndex );
#endif

/******************************************************************************/

tWMCDecStatus WMCDecInit (HWMCDECODER* phWMCDec, tVideoFormat_WMC tVideoOutputFormat, Bool_WMC bDiscardIndex, I32_WMC i32PostProc)

{   
    WMFDecoderEx *pDecoder = NULL_WMC;

    if( (NULL_WMC == phWMCDec)||(i32PostProc < -1) || (i32PostProc >4))
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phWMCDec);

	pDecoder->i32PostProc = i32PostProc;
	
	return WMCDecInit_All (phWMCDec, tVideoOutputFormat, bDiscardIndex, pDecoder->u32UserData, i32PostProc);
}


/******************************************************************************/
tWMCDecStatus WMCDecGetNumberOfMarkers (HWMCDECODER hWMCDec, U16_WMC *pu16NumMarkers)
//U16_WMC WMCDecGetNumberOfMarkers (HWMCDECODER hWMCDec)  
{
 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;

    if((hWMCDec == NULL_WMC) || (pu16NumMarkers == NULL_WMC))
    {
        return WMCDec_InValidArguments;
    }

    if(pDecoder->m_pMarkers == NULL_WMC) 
	{
        *pu16NumMarkers = 0;
    }
    else  
	{
        *pu16NumMarkers = (U16_WMC)pDecoder->m_dwMarkerNum;
    }
	return WMCDec_Succeeded;
}

/*******************************************************************************/
tWMCDecStatus WMCDecGetMarker (HWMCDECODER hWMCDec, U16_WMC iIndex, WMCMarkerEntry *pEntry) 
{
 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;

    if((hWMCDec == NULL_WMC) || (pEntry == NULL_WMC))
    {
        return WMCDec_InValidArguments;
    }

    if (iIndex >= pDecoder->m_dwMarkerNum)
    {
        memset(pEntry, 0, sizeof(WMCCodecEntry));
        return WMCDec_InvalidIndex;
    }

    if(pDecoder->m_pMarkers == NULL_WMC) {
        memset(pEntry, 0, sizeof(WMCMarkerEntry));
        return WMCDec_Fail;
    }
    else  {
        *pEntry = *(pDecoder->m_pMarkers + iIndex);
        return WMCDec_Succeeded;
    }
}


/*******************************************************************************/
tWMCDecStatus WMCDecGetCodecEntry (HWMCDECODER hWMCDec, U16_WMC iIndex, WMCCodecEntry *pEntry) 
{
 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;

    if((hWMCDec == NULL_WMC) || (pEntry == NULL_WMC))
    {
        return WMCDec_InValidArguments;
    }

    if (iIndex >= pDecoder->m_dwNumCodec)
    {
        memset(pEntry, 0, sizeof(WMCCodecEntry));
        return WMCDec_InvalidIndex;
    }

    if(pDecoder->m_pCodecEntry == NULL_WMC)
    {
        memset(pEntry, 0, sizeof(WMCCodecEntry));
        return WMCDec_Fail;
    }
    else  {
        *pEntry = *(pDecoder->m_pCodecEntry + iIndex);
        return WMCDec_Succeeded;
    }
}

/*******************************************************************************/

tWMCDecStatus WMCDecGetCodecCount (HWMCDECODER hWMCDec, U32_WMC * pnNumCodec)
{
 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;

    if((hWMCDec == NULL_WMC) || (pnNumCodec == NULL_WMC))
    {
        return WMCDec_InValidArguments;
    }

    if(pDecoder->m_pCodecEntry != NULL_WMC)
	{
        *pnNumCodec = pDecoder->m_dwNumCodec; 
    }
	else
        *pnNumCodec = 0;
	
    return WMCDec_Succeeded;

}



/******************************************************************************/
tWMCDecStatus WMCDecGetContentDesc (HWMCDECODER hWMCDec, const WMCContentDescription **pDesc)
{

 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;

    if ((hWMCDec == NULL_WMC) || (pDesc == NULL_WMC))
        return WMCDec_InValidArguments;

    if(pDecoder->m_pContentDesc == NULL_WMC)
        *pDesc = NULL_WMC;
    else 
        *pDesc = pDecoder->m_pContentDesc;

    return WMCDec_Succeeded;
}

/******************************************************************************/
tWMCDecStatus WMCDecGetExtendedContentDesc (HWMCDECODER hWMCDec, const WMCExtendedContentDesc **pECDesc)
{

 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;

    if ((hWMCDec == NULL_WMC) || (pECDesc == NULL_WMC))
        return WMCDec_InValidArguments;

    if(pDecoder->m_pExtendedContentDesc == NULL_WMC)
        *pECDesc = NULL_WMC;
    else
        *pECDesc = pDecoder->m_pExtendedContentDesc;

    return WMCDec_Succeeded;
}

/******************************************************************************/
tWMCDecStatus WMCGetScriptCommand (HWMCDECODER hDecoder, const WMCScriptCommand **pSCObject)
{

 	WMFDecoderEx* pDecoder = (WMFDecoderEx*) hDecoder;

    if ((hDecoder == NULL_WMC) || (pSCObject == NULL_WMC))
        return WMCDec_InValidArguments;

    if(pDecoder->m_pScriptCommand == NULL_WMC)
        *pSCObject = NULL_WMC;
    else
        *pSCObject = pDecoder->m_pScriptCommand;

    return WMCDec_Succeeded;
}


//////////////////////////////////////////////////New Code ///////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************/



tWMCDecStatus WMCDecInit_All (HWMCDECODER* phWMCDec, tVideoFormat_WMC tVideoOutputFormat, Bool_WMC bDiscardIndex, U32_WMC u32UserData, I32_WMC i32PostProc)
{   
    tWMCDecStatus Status = WMCDec_Succeeded;
    U16_WMC i=0;
    WMFDecoderEx *pDecoder = NULL_WMC;

    if( NULL_WMC == phWMCDec )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phWMCDec);


    memset( pDecoder, 0, sizeof( WMFDecoderEx ) );

	pDecoder->u32UserData = u32UserData;
	pDecoder->i32PostProc = i32PostProc;


    do
    {
       Status = WMCInitnParseHeader (phWMCDec);
        if( WMCDec_Succeeded != Status )
			break;

        Status = WMCDecSetVideoOutputFormat (*phWMCDec, tVideoOutputFormat);
        if( WMCDec_Succeeded != Status )
			break;
        if (bDiscardIndex == FALSE_WMC)
		{
			Status = WMCReadIndex (phWMCDec);
			if( WMCDec_Succeeded != Status )
				break;
		}
        pDecoder->bParsed = TRUE_WMC;


 	}  while (0);

  
    return Status;
}
/******************************************************************************/

HWMCDECODER WMCDecCreate(U32_WMC u32UserData)
{   WMFDecoderEx *pDecoder;

    pDecoder = (WMFDecoderEx *) wmvalloc( sizeof( WMFDecoderEx ) );
    memset(pDecoder, 0, sizeof(WMFDecoderEx));
    pDecoder->u32UserData = u32UserData;

    return pDecoder;
}


/******************************************************************************/


tWMCDecStatus WMCDecGetStreamInfo (HWMCDECODER hWMCDec, U32_WMC* pnNumStreams, tStreamIdnMediaType_WMC** pptMediaTypeStreams)
{
    tWMCDecStatus Status = WMCDec_Succeeded;
    U16_WMC     wNoOfStreams =0;
    U16_WMC     wNoOfAudioStreams =0;
    U16_WMC     wNoOfVideoStreams =0;
    U16_WMC     wNoOfBinaryStreams =0;
    U16_WMC     i=0;
    tStreamIdnMediaType_WMC *pStream = NULL_WMC;
    WMFDecoderEx *pDecoder = NULL_WMC;
    
    if( (NULL_WMC == hWMCDec) || (NULL_WMC == pptMediaTypeStreams) || (NULL_WMC == pnNumStreams))
        return( WMCDec_InValidArguments );

    *pptMediaTypeStreams = NULL_WMC;

    pDecoder = (WMFDecoderEx *)(hWMCDec);
 
    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );
    
    wNoOfStreams = pDecoder->tHeaderInfo.wNoOfStreams;
    wNoOfAudioStreams = pDecoder->tHeaderInfo.wNoOfAudioStreams;
    wNoOfVideoStreams = pDecoder->tHeaderInfo.wNoOfVideoStreams;
    wNoOfBinaryStreams = pDecoder->tHeaderInfo.wNoOfBinaryStreams;

    assert(wNoOfStreams ==(wNoOfAudioStreams + wNoOfVideoStreams + wNoOfBinaryStreams));

    *pptMediaTypeStreams = (tStreamIdnMediaType_WMC*)wmvalloc(wNoOfStreams*sizeof(tStreamIdnMediaType_WMC));
    pStream = *pptMediaTypeStreams;
    pDecoder->pStreamIdnMediaType = *pptMediaTypeStreams; 

    *pnNumStreams = wNoOfStreams;

    for(i=0;  i< wNoOfAudioStreams; i++)
    {
        pStream->wStreamId =  pDecoder->tAudioStreamInfo[i]->wStreamId;
        pStream->MediaType = Audio_WMC;
        pStream++;
    }

    for(i=0; i< wNoOfVideoStreams; i++)
    {
        pStream->wStreamId =  pDecoder->tVideoStreamInfo[i]->wStreamId;
        pStream->MediaType = Video_WMC;
        pStream++;
    }

    for(i=0; i< wNoOfBinaryStreams; i++)
    {
        pStream->wStreamId =  pDecoder->tBinaryStreamInfo[i]->wStreamId;
        pStream->MediaType = Binary_WMC;
        pStream++;
    }

    return Status;

}

/******************************************************************************/

tWMCDecStatus WMCDecGetAudioInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strAudioInfo_WMC* pstrAudioInfo)
{
    tWMCDecStatus Status = WMCDec_Succeeded;
    U16_WMC     wNoOfAudioStreams =0;
    U16_WMC     i=0;
    Bool_WMC    bDone = FALSE_WMC;
    WMFDecoderEx *pDecoder = NULL_WMC;
    
    if( (NULL_WMC == hWMCDec) || (NULL_WMC == pstrAudioInfo))
        return( WMCDec_InValidArguments );


    pDecoder = (WMFDecoderEx *)(hWMCDec);

    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );
    
    memset(pstrAudioInfo, 0, sizeof(strAudioInfo_WMC));

    wNoOfAudioStreams = pDecoder->tHeaderInfo.wNoOfAudioStreams;

    for(i=0; i< wNoOfAudioStreams; i++)
    {
        if (pDecoder->tAudioStreamInfo[i]->wStreamId == (U16_WMC)nStreamNum)
        {
            pstrAudioInfo->u16FormatTag = pDecoder->tAudioStreamInfo[i]->wFormatTag;
            pstrAudioInfo->u32BitsPerSecond =  (pDecoder->tAudioStreamInfo[i]->nAvgBytesPerSec)*8;
            pstrAudioInfo->u16BitsPerSample = (U16_WMC)pDecoder->tAudioStreamInfo[i]->nBitsPerSample;
            pstrAudioInfo->u16NumChannels = (U16_WMC)pDecoder->tAudioStreamInfo[i]->nChannels;
            pstrAudioInfo->u16SamplesPerSecond = (U16_WMC)pDecoder->tAudioStreamInfo[i]->nSamplesPerSec;
            pstrAudioInfo->u32BlockSize = (U32_WMC)pDecoder->tAudioStreamInfo[i]->nBlockAlign;
            pstrAudioInfo->u16ValidBitsPerSample =  (U16_WMC)pDecoder->tAudioStreamInfo[i]->wValidBitsPerSample;
            pstrAudioInfo->u32ChannelMask = (U32_WMC)pDecoder->tAudioStreamInfo[i]->dwChannelMask;
            pstrAudioInfo->u16StreamId = pDecoder->tAudioStreamInfo[i]->wStreamId ;

            bDone = TRUE_WMC;
            break;
        }
    }

    if (bDone != TRUE_WMC)
        Status = WMCDec_InValidArguments;

    return Status;
}

/******************************************************************************/

tWMCDecStatus WMCDecGetVideoInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strVideoInfo_WMC* pstrVideoInfo)
{
    tWMCDecStatus Status = WMCDec_Succeeded;
    U16_WMC     wNoOfVideoStreams =0;
    U16_WMC     i=0;
    Bool_WMC    bDone = FALSE_WMC;
    WMFDecoderEx *pDecoder = NULL_WMC;
    
    if( (NULL_WMC == hWMCDec) || (NULL_WMC == pstrVideoInfo))
        return( WMCDec_InValidArguments );


    pDecoder = (WMFDecoderEx *)(hWMCDec);
    
    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );
    
    memset(pstrVideoInfo, 0, sizeof(strVideoInfo_WMC));
 
    wNoOfVideoStreams = pDecoder->tHeaderInfo.wNoOfVideoStreams;

    for(i=0; i< wNoOfVideoStreams; i++)
    {
        if (pDecoder->tVideoStreamInfo[i]->wStreamId == (U16_WMC)nStreamNum)
        {
            
            if (pDecoder->tHeaderInfo.bHasBitRateInfo == TRUE_WMC)
            {
                U16_WMC j=0;
                for (j=0; j< pDecoder->tHeaderInfo.wNoOfStreams; j++ )
                {
                    if (pDecoder->tHeaderInfo.tBitRateInfo[j].wStreamId == (U16_WMC)nStreamNum)
                    {
                        pstrVideoInfo->u32BitsPerSecond  = pDecoder->tHeaderInfo.tBitRateInfo[j].dwBitRate;
                        break;
                    }
                }
            }
            else
            {
                // Guess Bitrate
                pstrVideoInfo->u32BitsPerSecond = pDecoder->tHeaderInfo.dwMaxBitrate;
                if (pDecoder->tHeaderInfo.wNoOfAudioStreams >0)
                    pstrVideoInfo->u32BitsPerSecond -= ((pDecoder->tAudioStreamInfo[0]->nAvgBytesPerSec)*8);
                
                pstrVideoInfo->u32BitsPerSecond = pstrVideoInfo->u32BitsPerSecond*9/10; //10% for asf overhead
            }

            pstrVideoInfo->fltFramesPerSecond =0;
            pstrVideoInfo->i32Height = pDecoder->tVideoStreamInfo[i]->biHeight;
            pstrVideoInfo->i32Width  = pDecoder->tVideoStreamInfo[i]->biWidth;
            pstrVideoInfo->u32Compression  = pDecoder->tVideoStreamInfo[i]->biCompression;
            pstrVideoInfo->u16StreamId = pDecoder->tVideoStreamInfo[i]->wStreamId ;
            pstrVideoInfo->u32VideoOutputPixelType = pDecoder->tVideoStreamInfo[i]->biCompressionOut;
            pstrVideoInfo->u16VideoOutputBitCount  = pDecoder->tVideoStreamInfo[i]->biBitCountOut;
            if(8==pDecoder->tVideoStreamInfo[i]->biBitCountOut)
            {
               if (
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV2 ||
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV1 ||
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP4S ||
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP43 ||
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP42 
#ifndef __NO_SCREEN__
				
					||
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS1 ||
                    pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS2 
#endif					
					)
                    memcpy(pstrVideoInfo->bPalette, StandardPalette, 3*256);  
//                else if(pDecoder->biCompression == FOURCC_MSS1)
//                    memcpy(pstrVideoInfo->bPalette, pDecoder->tVideoStreamInfo[i]->bPalette, 3*256);  
            }

            bDone = TRUE_WMC;
            break;
        }
    }

    if (bDone != TRUE_WMC)
        Status = WMCDec_InValidArguments;

    return Status;
}
/******************************************************************************/

tWMCDecStatus WMCDecGetBinaryInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strBinaryInfo_WMC* pstrBinaryInfo)
{
    tWMCDecStatus Status = WMCDec_Succeeded;
    U16_WMC     wNoOfBinaryStreams =0;
    U16_WMC     i=0;
    Bool_WMC    bDone = FALSE_WMC;
    WMFDecoderEx *pDecoder = NULL_WMC;
   
    if( (NULL_WMC == hWMCDec) || (NULL_WMC == pstrBinaryInfo))
        return( WMCDec_InValidArguments );


    pDecoder = (WMFDecoderEx *)(hWMCDec);

    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );
    
    wNoOfBinaryStreams = pDecoder->tHeaderInfo.wNoOfBinaryStreams;
    for(i=0; i< wNoOfBinaryStreams; i++)
    {
        if (pDecoder->tBinaryStreamInfo[i]->wStreamId == (U16_WMC)nStreamNum)
        {
            pstrBinaryInfo->u16StreamId = pDecoder->tBinaryStreamInfo[i]->wStreamId ;
            bDone = TRUE_WMC;
            break;
        }
        
    }

    if (bDone != TRUE_WMC)
        Status = WMCDec_InValidArguments;

    return Status;
}

/******************************************************************************/

tWMCDecStatus WMCDecGetHeaderInfo (HWMCDECODER hWMCDec, strHeaderInfo_WMC* pstrHeaderInfo)
{
    tWMCDecStatus Status = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder = NULL_WMC;
    
    if( (NULL_WMC == hWMCDec) || (NULL_WMC == pstrHeaderInfo))
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hWMCDec);
    
    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );
    
    memset(pstrHeaderInfo, 0, sizeof(strHeaderInfo_WMC));

    pstrHeaderInfo->u32MaxBitrate = pDecoder->tHeaderInfo.dwMaxBitrate;
    pstrHeaderInfo->u32PlayDuration = pDecoder->tHeaderInfo.msDuration;
    pstrHeaderInfo->u32Preroll  = pDecoder->tHeaderInfo.msPreroll;
    pstrHeaderInfo->u32SendDuration  = pDecoder->tHeaderInfo.msSendDuration;
    pstrHeaderInfo->u8HasDRM = pDecoder->bHasDRM;
    pstrHeaderInfo->u32PacketSize = pDecoder->tHeaderInfo.cbPacketSize;


    return Status;
}

/******************************************************************************/

tWMCDecStatus WMCDecSetDecodePatternForStreams (HWMCDECODER* phWMCDec, const tStreamIdPattern_WMC* ptStreamDecodePattern)
{
    // Set Pattern and if the stream is decodable and user wants output create decoders and keep handles
    tWMCDecStatus Status = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder = NULL_WMC;
    tStreamIdPattern_WMC* pPattern = NULL_WMC;
    U16_WMC i=0, j=0;
    tMediaType_WMC MediaType;
#ifndef __NO_SCREEN__    
    Void_WMC* hSCdec =NULL_WMC;
#endif    

#ifdef WMC_NO_BUFFER_MODE
	U32_WMC Index=0;
	U32_WMC dwPresTime=0;
	U64_WMC u64TimeReturn=0;
#endif



    if( (NULL_WMC == phWMCDec) || (NULL_WMC == ptStreamDecodePattern))
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(*phWMCDec);
    
    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );



#ifdef WMC_NO_BUFFER_MODE

// Get the Maximum pres time if it is called while decoding


    for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
    {
        Index = pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex;

        switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
        {
        case Audio_WMC:
            if (dwPresTime < (U32_WMC)pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp)
                dwPresTime = (U32_WMC)pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp;
            break;
        case Video_WMC:
            if (dwPresTime < pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp )
                dwPresTime = pDecoder->tVideoStreamInfo[Index]->dwVideoTimeStamp;
            break;
        case Binary_WMC:
            if (dwPresTime < pDecoder->tBinaryStreamInfo[Index]->dwBinaryTimeStamp )
                dwPresTime = dwPresTime;
			break;
        }
    }
    
	if (dwPresTime >= pDecoder->tHeaderInfo.msPreroll)
		dwPresTime -= pDecoder->tHeaderInfo.msPreroll;

#endif	


    pPattern = (tStreamIdPattern_WMC*)ptStreamDecodePattern;
    
    for (i=0; i<pDecoder->tHeaderInfo.wNoOfStreams; i++)
    {
        Status = WMCDecGetMediaType(*phWMCDec,pPattern->wStreamId, &MediaType);
        if (WMCDec_Succeeded !=Status)
            return Status;

        switch(MediaType)
        {
        case Audio_WMC:
            for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
            {
                if(pPattern->wStreamId == pDecoder->tAudioStreamInfo[j]->wStreamId)
                {
                    switch(pPattern->tPattern)
                    {
                    case Discard_WMC:
						if (pDecoder->tAudioStreamInfo[j]->bWantOutput == FALSE_WMC)
						{
							pDecoder->tAudioStreamInfo[j]->bTobeDecoded = FALSE_WMC;
//							pDecoder->tAudioStreamInfo[j]->bWantOutput = FALSE_WMC;
						}
						pDecoder->tAudioStreamInfo[j]->bStopReading = TRUE_WMC;
                        break;
                    case Compressed_WMC:
						if (pDecoder->tAudioStreamInfo[j]->bWantOutput == FALSE_WMC)
						{

	#ifndef WMC_NO_BUFFER_MODE
							pDecoder->tAudioStreamInfo[j]->dwAudioBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0));
							if (pDecoder->tAudioStreamInfo[j]->dwAudioBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3)
								pDecoder->tAudioStreamInfo[j]->dwAudioBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3;

							pDecoder->tAudioStreamInfo[j]->pbAudioBuffer = (U8_WMC*) wmvalloc(pDecoder->tAudioStreamInfo[j]->dwAudioBufSize);
							if (pDecoder->tAudioStreamInfo[j]->pbAudioBuffer == NULL_WMC)
								Status = WMCDec_BadMemory;
	#endif

	#ifdef WMC_NO_BUFFER_MODE
							pDecoder->tAudioStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
							pDecoder->tAudioStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
							pDecoder->tAudioStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
							pDecoder->tAudioStreamInfo[j]->parse_state = csWMCNewAsfPacket;
							WMCDecGetStreamToSpecificPosition (*phWMCDec,pDecoder->tAudioStreamInfo[j]->wStreamId, (U64_WMC) dwPresTime, &u64TimeReturn);
	#endif
                        
							pDecoder->tAudioStreamInfo[j]->wmar = WMA_S_NO_MORE_FRAME;
							pDecoder->tAudioStreamInfo[j]->bTobeDecoded = FALSE_WMC;
							pDecoder->tAudioStreamInfo[j]->bWantOutput = TRUE_WMC;
						
							pDecoder->tAudioStreamInfo[j]->dwAudioBufCurOffset = 0;
							pDecoder->tAudioStreamInfo[j]->cbNbFramesAudBuf = 0;
							pDecoder->tAudioStreamInfo[j]->dwAudioBufDecoded = 0;
							pDecoder->tAudioStreamInfo[j]->bBlockStart = 0;
							pDecoder->tAudioStreamInfo[j]->dwBlockLeft = 0;
							pDecoder->tAudioStreamInfo[j]->dwPayloadLeft = 0;
							pDecoder->tAudioStreamInfo[j]->dwAudPayloadPresTime = 0;
							pDecoder->tAudioStreamInfo[j]->dwAudioTimeStamp = 0.0;
						}
						pDecoder->tAudioStreamInfo[j]->bStopReading = FALSE_WMC;

                        break;
#ifndef _ASFPARSE_ONLY_
                    case Decompressed_WMC:
						if (pDecoder->tAudioStreamInfo[j]->hMSA == NULL_WMC)
						{
							if(pDecoder->tAudioStreamInfo[j]->bIsDecodable == TRUE_WMC)
							{
								if (pDecoder->tAudioStreamInfo[j]->nVersion > 0 && pDecoder->tAudioStreamInfo[j]->nVersion <4)
								{
									WMARESULT wmar = WMA_OK ;
#ifdef _V9WMADEC_								
									WMAPlayerInfo tPlayerInfo;
									U16_WMC z =0;
									U8_WMC Peak[] = {'P',0x00,'e',0x00,'a',0x00,'k',0x00,'V',0x00,'a',0x00,'l',0x00,'u',0x00,'e', 0x00, 0x00, 0x00};
									U8_WMC Average[] = {'A',0x00,'v',0x00,'e',0x00,'r',0x00,'a',0x00,'g',0x00,'e',0x00,'L',0x00,'e', 0x00, 'v',0x00,'e', 0x00,'l',0x00, 0x00, 0x00};
									memset(&tPlayerInfo, 0, sizeof(WMAPlayerInfo));

									if (pDecoder->tMetaDataEntry.pDescRec !=NULL)
									{
										for(z = 0; z < (U16_WMC)pDecoder->tMetaDataEntry.m_wDescRecordsCount; z++)
										{ 
											if ((pDecoder->tMetaDataEntry.pDescRec[z].wStreamNumber == pDecoder->tAudioStreamInfo[j]->wStreamId) ||( pDecoder->tMetaDataEntry.pDescRec[i].wStreamNumber ==0))
											{
												if (0==memcmp(Peak,pDecoder->tMetaDataEntry.pDescRec[z].pwName, pDecoder->tMetaDataEntry.pDescRec[z].wNameLenth))
												{
													tPlayerInfo.wPeakAmplitude = (U16_WMARawDec) (*(U32_WMC *)pDecoder->tMetaDataEntry.pDescRec[z].pData);
												
												}
													
												else if (0==memcmp(Average,pDecoder->tMetaDataEntry.pDescRec[z].pwName, pDecoder->tMetaDataEntry.pDescRec[z].wNameLenth))
												{
													tPlayerInfo.wRmsAmplitude = (U16_WMARawDec) (*(U32_WMC *)pDecoder->tMetaDataEntry.pDescRec[z].pData);
												
												}
											
											
											}
										
										}
									
									}
								
#endif									
									wmar = WMARawDecInit (
										&(pDecoder->tAudioStreamInfo[j]->hMSA),
										(U32_WMARawDec) pDecoder,
										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->nVersion,
										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->nSamplesPerBlock,
										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->nSamplesPerSec,
										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->nChannels,
	#ifdef _V9WMADEC_
										(U32_WMARawDec) (pDecoder->tAudioStreamInfo[j]->nBitsPerSample/8),
										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->wValidBitsPerSample,
										(U32_WMARawDec) pDecoder->tAudioStreamInfo[j]->dwChannelMask,
	#endif

										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->nAvgBytesPerSec,
										(U16_WMARawDec) pDecoder->tAudioStreamInfo[j]->nBlockAlign,
										(U16_WMARawDec) (pDecoder->tAudioStreamInfo[j]->nEncodeOpt /*| 0x0080*/),
	#if defined (_V9WMADEC_)
										(WMAPlayerInfo*) &tPlayerInfo
	#else
										0
	#endif
									);
									
									if (wmar == WMA_E_NOTSUPPORTED)
									{
										Status =  WMADec_UnSupportedCompressedFormat;
									}
									if (wmar != WMA_OK)
									{
										Status = WMADec_Failed;
									}
                            
	#ifndef WMC_NO_BUFFER_MODE
									pDecoder->tAudioStreamInfo[j]->dwAudioBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0));
									if (pDecoder->tAudioStreamInfo[j]->dwAudioBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3)
										pDecoder->tAudioStreamInfo[j]->dwAudioBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3;

                                pDecoder->tAudioStreamInfo[j]->pbAudioBuffer = (U8_WMC*) wmvalloc(pDecoder->tAudioStreamInfo[j]->dwAudioBufSize);
                                if (pDecoder->tAudioStreamInfo[j]->pbAudioBuffer == NULL_WMC)
                                    Status =  WMCDec_BadMemory;
	#endif

									pDecoder->tAudioStreamInfo[j]->wmar = WMA_S_NO_MORE_FRAME;

									pDecoder->tAudioStreamInfo[j]->bTobeDecoded = TRUE_WMC;
									pDecoder->tAudioStreamInfo[j]->bWantOutput = TRUE_WMC;
									pDecoder->tAudioStreamInfo[j]->bStopReading = FALSE_WMC;
									
								
								}
	#ifndef __NO_SPEECH__

								else if(pDecoder->tAudioStreamInfo[j]->nVersion ==4) //Speech codec
								{
                                
									WMSRESULT wmsr = WMS_OK ;

									// Initialize the Intel Library
	//	                            if( !InitNSPLibWMSP(DLL_PROCESS_ATTACH) )
									if( !MyInitNSP(INTELFFT_INIT_PROCESS) )
									{
										Status = WMADec_Failed;;
									}

	//	                            if( !InitNSPLibWMSP(DLL_THREAD_ATTACH) )
									if( !MyInitNSP(INTELFFT_INIT_THREAD) )
									{
										Status =  WMADec_Failed;
									}
                                
									wmsr = WMSRawDecInit (
											(WMSRawDecHandle*) &(pDecoder->tAudioStreamInfo[j]->hMSA),
											(U32_WMSRawDec) pDecoder,
											(U32_WMSRawDec) pDecoder->tAudioStreamInfo[j]->nSamplesPerSec, 
											(U32_WMSRawDec) pDecoder->tAudioStreamInfo[j]->nAvgBytesPerSec, 
											(U16_WMSRawDec) pDecoder->tAudioStreamInfo[j]->nChannels, 
											(U16_WMSRawDec) 5
										);

                                    
									if (wmsr == WMS_E_NOTSUPPORTED)
									{
										Status = WMADec_UnSupportedCompressedFormat;
									}
									if (wmsr != WMS_OK)
									{
										Status = WMADec_Failed;
									}

	#ifndef WMC_NO_BUFFER_MODE
									pDecoder->tAudioStreamInfo[j]->dwAudioBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0));
									if (pDecoder->tAudioStreamInfo[j]->dwAudioBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3)
										pDecoder->tAudioStreamInfo[j]->dwAudioBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3;

									pDecoder->tAudioStreamInfo[j]->pbAudioBuffer = (U8_WMC*) wmvalloc(pDecoder->tAudioStreamInfo[j]->dwAudioBufSize);
									if (pDecoder->tAudioStreamInfo[j]->pbAudioBuffer == NULL_WMC)
										Status =  WMCDec_BadMemory;

	#endif

						   //         pDecoder->tAudioStreamInfo[j]->wmar = WMA_S_NO_MORE_FRAME;

									pDecoder->tAudioStreamInfo[j]->bTobeDecoded = TRUE_WMC;
									pDecoder->tAudioStreamInfo[j]->bWantOutput = TRUE_WMC;
									pDecoder->tAudioStreamInfo[j]->bStopReading = FALSE_WMC;
								}
	#endif
	#ifdef WMC_NO_BUFFER_MODE
								pDecoder->tAudioStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
								pDecoder->tAudioStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
								pDecoder->tAudioStreamInfo[j]->parse_state = csWMCNewAsfPacket;
								pDecoder->tAudioStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
								WMCDecGetStreamToSpecificPosition (*phWMCDec,pDecoder->tAudioStreamInfo[j]->wStreamId, (U64_WMC) dwPresTime, &u64TimeReturn);
	#endif
								pDecoder->tAudioStreamInfo[j]->dwAudioBufCurOffset = 0;
								pDecoder->tAudioStreamInfo[j]->cbNbFramesAudBuf = 0;
								pDecoder->tAudioStreamInfo[j]->dwAudioBufDecoded = 0;
								pDecoder->tAudioStreamInfo[j]->bBlockStart = 0;
								pDecoder->tAudioStreamInfo[j]->dwBlockLeft = 0;
								pDecoder->tAudioStreamInfo[j]->dwPayloadLeft = 0;
								pDecoder->tAudioStreamInfo[j]->dwAudPayloadPresTime = 0;
								pDecoder->tAudioStreamInfo[j]->dwAudioTimeStamp = 0.0;

							}
							else
							{
								pDecoder->tAudioStreamInfo[j]->bTobeDecoded = FALSE_WMC;
								pDecoder->tAudioStreamInfo[j]->bWantOutput = FALSE_WMC;
								pDecoder->tAudioStreamInfo[j]->bStopReading = TRUE_WMC;
								Status = WMADec_UnSupportedCompressedFormat;

							}
						}
                        break;
#endif //_ASFPARSE_ONLY_
                    default:
                        pDecoder->tAudioStreamInfo[j]->bTobeDecoded = FALSE_WMC;
                        pDecoder->tAudioStreamInfo[j]->bWantOutput = FALSE_WMC;
						pDecoder->tAudioStreamInfo[j]->bStopReading = TRUE_WMC;
                    }
                    
                    pDecoder->tAudioStreamInfo[j]->bFirstTime = FALSE_WMC;

					if (pDecoder->tAudioStreamInfo[j]->bWantOutput == TRUE_WMC )
                    {
                        U16_WMC k =0;
						Bool_WMC bExists = FALSE_WMC;

						for (k=0; k< pDecoder->tPlannedOutputInfo.wTotalOutput; k++)
						{
							if( (pDecoder->tPlannedOutputInfo.tPlannedId[k].wStreamIndex == j)&&(pDecoder->tPlannedOutputInfo.tPlannedId[k].tMediaType == Audio_WMC))
							{
								bExists = TRUE_WMC;
								break;
							}
						
						}
						
						
						if (bExists == FALSE_WMC)
						{
							pDecoder->tPlannedOutputInfo.tPlannedId[pDecoder->tPlannedOutputInfo.wTotalOutput].wStreamIndex = j;
							pDecoder->tPlannedOutputInfo.tPlannedId[pDecoder->tPlannedOutputInfo.wTotalOutput].tMediaType = Audio_WMC;
							pDecoder->tPlannedOutputInfo.wTotalOutput+=1;
						}
                    }
                    break;
                }
            
            }
            break;
        case Video_WMC:
            for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
            {
                if(pPattern->wStreamId == pDecoder->tVideoStreamInfo[j]->wStreamId)
                {
                    switch(pPattern->tPattern)
                    {
                    case Discard_WMC:
						if (pDecoder->tVideoStreamInfo[j]->bWantOutput == FALSE_WMC)
						{
							pDecoder->tVideoStreamInfo[j]->bTobeDecoded = FALSE_WMC;
//							pDecoder->tVideoStreamInfo[j]->bWantOutput = FALSE_WMC;
						}
						pDecoder->tVideoStreamInfo[j]->bStopReading = TRUE_WMC;
                        break;
                    case Compressed_WMC:
						if (pDecoder->tVideoStreamInfo[j]->bWantOutput == FALSE_WMC)
						{

	#ifndef WMC_NO_BUFFER_MODE
							pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0 ));
							if (pDecoder->tVideoStreamInfo[j]->dwVideoBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3)
								pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3;

							pDecoder->tVideoStreamInfo[j]->pbVideoBuffer = (U8_WMC*) wmvalloc(pDecoder->tVideoStreamInfo[j]->dwVideoBufSize);
							if (pDecoder->tVideoStreamInfo[j]->pbVideoBuffer == NULL_WMC)
								return WMCDec_BadMemory;            
	#endif
							pDecoder->tVideoStreamInfo[j]->bTobeDecoded = FALSE_WMC;
							pDecoder->tVideoStreamInfo[j]->bWantOutput = TRUE_WMC;
	#ifdef WMC_NO_BUFFER_MODE
							pDecoder->tVideoStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
							pDecoder->tVideoStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
							pDecoder->tVideoStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
							pDecoder->tVideoStreamInfo[j]->parse_state = csWMCNewAsfPacket;
							pDecoder->tVideoStreamInfo[j]->dwNextVideoTimeStamp = 0;
							WMCDecGetStreamToSpecificPosition (*phWMCDec,pDecoder->tVideoStreamInfo[j]->wStreamId, (U64_WMC) dwPresTime, &u64TimeReturn);
	#endif
							pDecoder->tVideoStreamInfo[j]->dwBlockLeft =0;
							pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset =0;
							pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf =0;
							pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded =0;
							pDecoder->tVideoStreamInfo[j]->bFirst = FALSE_WMC;
							pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
							pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
							pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
							pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
							pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
							pDecoder->tVideoStreamInfo[j]->cbFrame = 0;

						}
						pDecoder->tVideoStreamInfo[j]->bStopReading = FALSE_WMC;
                        break;
                    case Decompressed_WMC:
						if (pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != IYUV_WMV    &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != I420_WMV    &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != YUY2_WMV    &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != UYVY_WMV    &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != YVYU_WMV    &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != RGB24_WMV   &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != RGB555_WMV  &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != RGB565_WMV  &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != RGB32_WMV   &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != YVU9_WMV    &&
							pDecoder->tVideoStreamInfo[j]->tOutVideoFormat != RGB8_WMV)
						{
							Status = WMVDec_UnSupportedOutputFormat;
							break;
						}
						
						
						if(pDecoder->tVideoStreamInfo[j]->pVDec == NULL_WMC)
						{
							if(pDecoder->tVideoStreamInfo[j]->bIsDecodable == TRUE_WMC)
							{

								{
									if(
										pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_WMV2 ||
										pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_WMV1 ||
										pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_MP4S ||
										pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_MP43 ||
										pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_MP42 )
									{
                                    
										U32_WMC   dwBitRate = 0;
										U32_WMC x=0;
#ifndef _ASFPARSE_ONLY_
										tWMVDecodeStatus VideoStatus;
#endif
										if (pDecoder->tHeaderInfo.bHasBitRateInfo == TRUE_WMC)
										{
											for (x=0; x< pDecoder->tHeaderInfo.wNoOfStreams; x++)
											{
												if (pDecoder->tHeaderInfo.tBitRateInfo[x].wStreamId == pPattern->wStreamId)
												{
													dwBitRate = pDecoder->tHeaderInfo.tBitRateInfo[x].dwBitRate;
													break;
                                            
												}
											}
                                    
                                    
										}
                                    
                                    
	//                                    tWMVDecodeStatus VideoStatus;
									  //  hWMVDec = WMVideo8DecoderCreate ();
#ifndef _ASFPARSE_ONLY_
										 VideoStatus = WMVideoDecInit (
											(HWMVDecoder*)&(pDecoder->tVideoStreamInfo[j]->pVDec),
											(U32_WMC)pDecoder,
											pDecoder->tVideoStreamInfo[j]->biCompression,
											0.0,
											(Float_WMV)dwBitRate,
											pDecoder->tVideoStreamInfo[j]->biWidth,
											pDecoder->tVideoStreamInfo[j]->biHeight,
											(I32_WMV)pDecoder->i32PostProc
										);

										if (VideoStatus != WMV_Succeeded)
											Status = WMVDec_Failed;

#endif
#ifndef WMC_NO_BUFFER_MODE
										pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0));
										if (pDecoder->tVideoStreamInfo[j]->dwVideoBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*5)
											pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*5;
#else
										pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (U32_WMC)(pDecoder->tVideoStreamInfo[j]->biSizeImage);
#endif
										pDecoder->tVideoStreamInfo[j]->pbVideoBuffer = (U8_WMC*) wmvalloc(pDecoder->tVideoStreamInfo[j]->dwVideoBufSize);
										if (pDecoder->tVideoStreamInfo[j]->pbVideoBuffer == NULL_WMC)
											Status = WMCDec_BadMemory;            

                                    
										if (pDecoder->tVideoStreamInfo[j]->biCompression  == FOURCC_MP4S || pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_WMV2)
										{
											pDecoder->i32CurrentVideoStreamIndex = j;
											pDecoder->tVideoStreamInfo[j]->bInitStage = TRUE_WMC;
#ifndef WMC_NO_BUFFER_MODE
											memcpy(pDecoder->tVideoStreamInfo[j]->pbVideoBuffer, pDecoder->tVideoStreamInfo[j]->bAdditionalInfo, pDecoder->tVideoStreamInfo[j]->biSize - sizeof (BITMAPINFOHEADER) );
											pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset = pDecoder->tVideoStreamInfo[j]->biSize - sizeof (BITMAPINFOHEADER);
#endif
#ifndef _ASFPARSE_ONLY_
											VideoStatus =  WMVideoDecDecodeSequenceHeader ((HWMVDecoder) pDecoder->tVideoStreamInfo[j]->pVDec);

											if (VideoStatus != WMV_Succeeded)
												Status = WMVDec_Failed;
#endif
											//WMVideo8DecoderSetExtendedFormat (hWMVDec, pDecoder->bAdditionalInfo , pDecoder->biSize  - sizeof (BITMAPINFOHEADER)); //////////////////
										}
										// Reset all vars again    

										
										pDecoder->tVideoStreamInfo[j]->bInitStage = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->bBlockStart = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->dwBlockLeft =0;
										pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset =0;
										pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf =0;
										pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded =0;
										pDecoder->tVideoStreamInfo[j]->bFirst = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
										pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
										pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
										pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
										pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
										pDecoder->tVideoStreamInfo[j]->cbFrame = 0;
										


	#ifdef WMC_NO_BUFFER_MODE
										pDecoder->tVideoStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
										pDecoder->tVideoStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
										pDecoder->tVideoStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
										pDecoder->tVideoStreamInfo[j]->parse_state = csWMCNewAsfPacket;
										pDecoder->tVideoStreamInfo[j]->dwNextVideoTimeStamp = 0;
										WMCDecGetStreamToSpecificPosition (*phWMCDec,pDecoder->tVideoStreamInfo[j]->wStreamId, (U64_WMC) dwPresTime, &u64TimeReturn);
	#endif
                                  
										pDecoder->tVideoStreamInfo[j]->bTobeDecoded = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->bWantOutput = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->bStopReading = FALSE_WMC;
									}
	///////////////////////////////////////////////////
	#ifndef __NO_SCREEN__
									else if(pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_MSS1 ||
											pDecoder->tVideoStreamInfo[j]->biCompression == FOURCC_MSS2)
									{
										LPBITMAPINFOHEADER    lpbiSrc = NULL_WMC;
										BITMAPINFOHEADER      BitmapInfo;  
										HRESULT hr;

										hSCdec = WMScreenDecoderCreate();
										pDecoder->tVideoStreamInfo[j]->pVDec = hSCdec;
										pDecoder->tVideoStreamInfo[j]->bTobeDecoded = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->bWantOutput = TRUE_WMC;



										memset(&BitmapInfo,0,sizeof(BITMAPINFOHEADER));

										pDecoder->tVideoStreamInfo[j]->bIsDecodable = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->biSizeImageOut = (pDecoder->tVideoStreamInfo[j]->biWidth)*(pDecoder->tVideoStreamInfo[j]->biHeight)*(pDecoder->tVideoStreamInfo[j]->biBitCountOut)/8;

										BitmapInfo.biBitCount = pDecoder->tVideoStreamInfo[j]->biBitCountOut;
										BitmapInfo.biCompression  = pDecoder->tVideoStreamInfo[j]->biCompressionOut;
										BitmapInfo.biHeight  = pDecoder->tVideoStreamInfo[j]->biHeight;
										BitmapInfo.biWidth  = pDecoder->tVideoStreamInfo[j]->biWidth;
										BitmapInfo.biSizeImage = pDecoder->tVideoStreamInfo[j]->biSizeImageOut;
										BitmapInfo.biSize  = sizeof (BITMAPINFOHEADER);


										lpbiSrc = &BitmapInfo;
										hr = WMScreenDecoderInit( pDecoder->tVideoStreamInfo[j]->pVDec, 
											pDecoder->tVideoStreamInfo[j]->bAdditionalInfo,
											pDecoder->tVideoStreamInfo[j]->biSize  - sizeof (BITMAPINFOHEADER),
											lpbiSrc);

										if (FAILED(hr))
										{
											pDecoder->tVideoStreamInfo[j]->biBitCountOut = pDecoder->tVideoStreamInfo[i]->biBitCount;
											pDecoder->tVideoStreamInfo[j]->biCompressionOut = pDecoder->tVideoStreamInfo[i]->biCompression;
											pDecoder->tVideoStreamInfo[j]->biSizeImageOut = pDecoder->tVideoStreamInfo[i]->biSizeImage;

											pDecoder->tVideoStreamInfo[j]->bIsDecodable = FALSE_WMC;
											pDecoder->tVideoStreamInfo[j]->bTobeDecoded = FALSE_WMC;
											Status =  WMVDec_Failed;
										}

										pDecoder->tVideoStreamInfo[j]->pSCInBuffer = wmvalloc(pDecoder->tVideoStreamInfo[j]->biSizeImageOut);
										pDecoder->tVideoStreamInfo[j]->pSCOutBuffer = wmvalloc(pDecoder->tVideoStreamInfo[j]->biSizeImageOut);

										if(pDecoder->tVideoStreamInfo[j]->pSCInBuffer == NULL_WMC || pDecoder->tVideoStreamInfo[j]->pSCOutBuffer == NULL_WMC)
										{
											Status =  WMCDec_BadMemory;
                
										}

	#ifndef WMC_NO_BUFFER_MODE
										pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0));
										if (pDecoder->tVideoStreamInfo[j]->dwVideoBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*5)
											pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*5;

	#else
										pDecoder->tVideoStreamInfo[j]->dwVideoBufSize = (U32_WMC)(pDecoder->tVideoStreamInfo[j]->biSizeImage);
	#endif

										pDecoder->tVideoStreamInfo[j]->pbVideoBuffer = (U8_WMC*)wmvalloc(pDecoder->tVideoStreamInfo[j]->dwVideoBufSize);
										if (pDecoder->tVideoStreamInfo[j]->pbVideoBuffer == NULL_WMC)
											Status =  WMCDec_BadMemory;            

										pDecoder->tVideoStreamInfo[j]->bTobeDecoded = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->bWantOutput = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->bStopReading = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->bInitStage = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->bBlockStart = TRUE_WMC;
										pDecoder->tVideoStreamInfo[j]->dwBlockLeft =0;
										pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset =0;
										pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf =0;
										pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded =0;
										pDecoder->tVideoStreamInfo[j]->bFirst = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
										pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
										pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
										pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
										pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
										pDecoder->tVideoStreamInfo[j]->cbFrame = 0;

	#ifdef WMC_NO_BUFFER_MODE
										pDecoder->tVideoStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
										pDecoder->tVideoStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
										pDecoder->tVideoStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
										pDecoder->tVideoStreamInfo[j]->parse_state = csWMCNewAsfPacket;
										pDecoder->tVideoStreamInfo[j]->dwNextVideoTimeStamp = 0;
										WMCDecGetStreamToSpecificPosition (*phWMCDec,pDecoder->tVideoStreamInfo[j]->wStreamId, (U64_WMC) dwPresTime, &u64TimeReturn);
	#endif
									}
	#endif            
	//////////////////////////////////////////////////								
									
									else
									{
										pDecoder->tVideoStreamInfo[j]->bTobeDecoded = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->bStopReading = FALSE_WMC;
										pDecoder->tVideoStreamInfo[j]->bWantOutput = TRUE_WMC;
										Status = WMVDec_UnSupportedCompressedFormat;
									}
								}
                            
							}
							else
							{
								pDecoder->tVideoStreamInfo[j]->bTobeDecoded = FALSE_WMC;
								pDecoder->tVideoStreamInfo[j]->bWantOutput = FALSE_WMC;
								pDecoder->tVideoStreamInfo[j]->bStopReading = TRUE_WMC;
								Status = WMVDec_UnSupportedCompressedFormat;
							}
						}
                        break;
                    default:
                        pDecoder->tVideoStreamInfo[j]->bTobeDecoded = FALSE_WMC;
                        pDecoder->tVideoStreamInfo[j]->bWantOutput = FALSE_WMC;
						pDecoder->tVideoStreamInfo[j]->bStopReading = TRUE_WMC;
                    
                    }
                    if (pDecoder->tVideoStreamInfo[j]->bWantOutput == TRUE_WMC )
                    {
                        U16_WMC k =0;
						Bool_WMC bExists = FALSE_WMC;

						for (k=0; k< pDecoder->tPlannedOutputInfo.wTotalOutput; k++)
						{
							if ((pDecoder->tPlannedOutputInfo.tPlannedId[k].wStreamIndex == j)&&(pDecoder->tPlannedOutputInfo.tPlannedId[k].tMediaType == Video_WMC))
							{
								bExists = TRUE_WMC;
								break;
							}
						
						}
						if (bExists == FALSE_WMC)
						{
							pDecoder->tPlannedOutputInfo.tPlannedId[pDecoder->tPlannedOutputInfo.wTotalOutput].wStreamIndex = j;
							pDecoder->tPlannedOutputInfo.tPlannedId[pDecoder->tPlannedOutputInfo.wTotalOutput].tMediaType = Video_WMC;
							pDecoder->tPlannedOutputInfo.wTotalOutput+=1;
						}
                    }
                    break;
                }
				pDecoder->tVideoStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
				pDecoder->tVideoStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
            
            }
            break;
        case Binary_WMC:
            for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
            {
                if(pPattern->wStreamId == pDecoder->tBinaryStreamInfo[j]->wStreamId)
                {
                    switch(pPattern->tPattern)
                    {
                    case Discard_WMC:
//						if (pDecoder->tBinaryStreamInfo[j]->bWantOutput == FALSE_WMC)
//							pDecoder->tBinaryStreamInfo[j]->bWantOutput = FALSE_WMC;
						pDecoder->tBinaryStreamInfo[j]->bStopReading = TRUE_WMC;
                        break;
                    case Compressed_WMC:
                    case Decompressed_WMC:
						if (pDecoder->tBinaryStreamInfo[j]->bWantOutput != TRUE_WMC)
						{
	#ifndef WMC_NO_BUFFER_MODE
						pDecoder->tBinaryStreamInfo[j]->dwBinaryBufSize = (U32_WMC)((pDecoder->tHeaderInfo.dwMaxBitrate/8.0)*((Double_WMC)pDecoder->tHeaderInfo.msPreroll/1000.0));
						if (pDecoder->tBinaryStreamInfo[j]->dwBinaryBufSize < (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3)
							pDecoder->tBinaryStreamInfo[j]->dwBinaryBufSize = (pDecoder->tHeaderInfo.dwMaxBitrate/8)*3;

						pDecoder->tBinaryStreamInfo[j]->pbBinaryBuffer = (U8_WMC*)wmvalloc(pDecoder->tBinaryStreamInfo[j]->dwBinaryBufSize);
						if (pDecoder->tBinaryStreamInfo[j]->pbBinaryBuffer == NULL_WMC)
							Status = WMCDec_BadMemory;            
	#endif
							pDecoder->tBinaryStreamInfo[j]->bWantOutput = TRUE_WMC;
	#ifdef WMC_NO_BUFFER_MODE
								pDecoder->tBinaryStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
								pDecoder->tBinaryStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
								pDecoder->tBinaryStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
								pDecoder->tBinaryStreamInfo[j]->parse_state = csWMCNewAsfPacket;
								pDecoder->tBinaryStreamInfo[j]->dwNextBinaryTimeStamp = 0;
								WMCDecGetStreamToSpecificPosition (*phWMCDec,pDecoder->tBinaryStreamInfo[j]->wStreamId, (U64_WMC) dwPresTime, &u64TimeReturn);
	#endif
								pDecoder->tBinaryStreamInfo[j]->bWantOutput = TRUE_WMC;
								pDecoder->tBinaryStreamInfo[j]->bStopReading = FALSE_WMC;

								pDecoder->tBinaryStreamInfo[j]->dwBlockLeft =0;
								pDecoder->tBinaryStreamInfo[j]->dwBinaryBufCurOffset =0;
								pDecoder->tBinaryStreamInfo[j]->cbNbFramesBinBuf =0;
								pDecoder->tBinaryStreamInfo[j]->dwBinaryBufDecoded =0;
								pDecoder->tBinaryStreamInfo[j]->dwPayloadLeft = 0;
								pDecoder->tBinaryStreamInfo[j]->dwBinaryTimeStamp = 0;
								pDecoder->tBinaryStreamInfo[j]->dwNbFrames = 0;
								pDecoder->tBinaryStreamInfo[j]->cbUsed = 0;
								pDecoder->tBinaryStreamInfo[j]->dwFrameSize = 0; 
								pDecoder->tBinaryStreamInfo[j]->cbFrame = 0;

						}
                        break;
                    default:
                        pDecoder->tBinaryStreamInfo[j]->bWantOutput = FALSE_WMC;
						pDecoder->tBinaryStreamInfo[j]->bStopReading = TRUE_WMC;
                    
                    }
                    if (pDecoder->tBinaryStreamInfo[j]->bWantOutput == TRUE_WMC )
                    {
                        U16_WMC k =0;
						Bool_WMC bExists = FALSE_WMC;

						for (k=0; k< pDecoder->tPlannedOutputInfo.wTotalOutput; k++)
						{
							if ((pDecoder->tPlannedOutputInfo.tPlannedId[k].wStreamIndex == j)&&(pDecoder->tPlannedOutputInfo.tPlannedId[k].tMediaType == Binary_WMC))
							{
								bExists = TRUE_WMC;
								break;
							}
						
						}
						if (bExists == FALSE_WMC)
						{
							pDecoder->tPlannedOutputInfo.tPlannedId[pDecoder->tPlannedOutputInfo.wTotalOutput].wStreamIndex = j;
							pDecoder->tPlannedOutputInfo.tPlannedId[pDecoder->tPlannedOutputInfo.wTotalOutput].tMediaType = Binary_WMC;
							pDecoder->tPlannedOutputInfo.wTotalOutput+=1;
						}
                    }
                    
                    break;
                }
				pDecoder->tBinaryStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
				pDecoder->tBinaryStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
            }
            break;
        default:
            return WMCDec_InValidArguments;

        }
    
        pPattern++;
    }

/*	#ifdef WMC_NO_BUFFER_MODE

		if (dwPresTime >= pDecoder->tHeaderInfo.msPreroll)
			dwPresTime -= pDecoder->tHeaderInfo.msPreroll;

		Status = WMCDecSeek(*phWMCDec, (U64_WMC) dwPresTime, &u64TimeReturn );

	#endif

*/
	if ((0 == pDecoder->tPlannedOutputInfo.wTotalOutput) && (Status ==WMCDec_Succeeded))
	{
		Status = WMCDec_NoOutput;
	}
	else if ((Status == WMVDec_UnSupportedCompressedFormat) || (Status == WMADec_UnSupportedCompressedFormat))
	{
		Status = WMCDec_UnSupportedCompressedFormats;
	}

    return Status;
}


/******************************************************************************/

tWMCDecStatus WMCDecGetMediaType (HWMCDECODER hWMCDec, U16_WMC wStreamId, tMediaType_WMC * pMediaType)
{
    tWMCDecStatus Status = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder = NULL_WMC;
    tStreamIdnMediaType_WMC* pStreamIdnMediaType = NULL_WMC;
    U16_WMC i=0;
    Bool_WMC bDone = FALSE_WMC;

    if( (NULL_WMC == hWMCDec) || (NULL_WMC == pMediaType))
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hWMCDec);
    
    if (pDecoder->bParsed != TRUE_WMC) 
        return( WMCDec_InValidArguments );

    pStreamIdnMediaType = pDecoder->pStreamIdnMediaType;

    for (i=0; i<pDecoder->tHeaderInfo.wNoOfStreams; i++)
    {
        if(wStreamId == pStreamIdnMediaType->wStreamId)
        {
            *pMediaType = pStreamIdnMediaType->MediaType;
            bDone = TRUE_WMC;
            break;
        }

        pStreamIdnMediaType++;
    }

    if (bDone != TRUE_WMC)
        Status = WMCDec_InvalidStreamType;

    return Status;

}

/******************************************************************************/
typedef struct WmaGetDataParam
{
	U32_WMC  m_dwUser;
	Bool_WMC m_fTimeIsValid;
	I64_WMC  m_iTime;
} WmaGetDataParam;

WMARESULT WMARawDecCBGetData (U8_WMARawDec **ppBuffer, U32_WMARawDec *pcbBuffer, U32_WMARawDec dwUserData)
{
    
	tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder =NULL_WMC;
	HWMCDECODER hWMCDec;
    U32_WMARawDec cbData = 128;//MIN_WANTED
    U16_WMC AudioStreamIndex = 0;
#ifdef WMC_NO_BUFFER_MODE
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    tWMCParseState  parse_state;
#endif

#ifdef _V9WMADEC_
    WmaGetDataParam *pWmaGetDataParam = (WmaGetDataParam *)dwUserData;
	hWMCDec = (HWMCDECODER)(pWmaGetDataParam->m_dwUser);

//    pDecoder = (WMFDecoderEx *)(pWmaGetDataParam->m_dwUser);
    pWmaGetDataParam->m_fTimeIsValid = 0;
#else
	hWMCDec = (HWMCDECODER)(dwUserData);
#endif

    pDecoder = (WMFDecoderEx *)hWMCDec;

    if(pDecoder == NULL_WMC || ppBuffer == NULL_WMC || pcbBuffer == NULL_WMC)
    {
        if(ppBuffer != NULL_WMC)
            *ppBuffer = NULL_WMC;
        if(pcbBuffer != NULL_WMC)
            *pcbBuffer = 0;

        return WMA_E_INVALIDARG;
    }

    *ppBuffer = NULL_WMC;
    *pcbBuffer = 0;
    
    AudioStreamIndex = (U16_WMC)pDecoder->i32CurrentAudioStreamIndex; 

#ifndef WMC_NO_BUFFER_MODE

	if ((pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft ==0) && (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)) // New Payload
	{

		if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded > 0)
		{
			if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded)
			{
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->cbNbFramesAudBuf-=1;
				memmove(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer, pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded,(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset - pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded));
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset - pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded;//pDecoder->dwAudioBufDecoded; 
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded=0;
			}

		}
		
		
		memcpy (&(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime),(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded), sizeof(U32_WMC));
		memcpy (&(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft),(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded += 2*sizeof(U32_WMC);
//		pDecoder->dwAudioTimeStamp = pDecoder->dwAudPayloadPresTime;
        if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bFirstTime == FALSE_WMC)
		{
			pDecoder->tAudioStreamInfo[AudioStreamIndex]->bFirstTime = TRUE_WMC;
		}
		else
			pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 

#ifdef _V9WMADEC_
        pDecoder = (WMFDecoderEx *)(pWmaGetDataParam->m_dwUser);
        pWmaGetDataParam->m_fTimeIsValid = 1;
        pWmaGetDataParam->m_iTime = (I64_WMC)(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime - pDecoder->tHeaderInfo.msPreroll)* 10000;
        pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = FALSE_WMC; 
#endif
    }

	if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
	{
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = 1;
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft - pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
	}

	if (cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft )
		cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

	if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded + cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset)
	{
		*ppBuffer = NULL_WMC;
		*pcbBuffer = 0;
		return WMA_S_NEWPACKET;
	}
	

	memcpy(pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer,  pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded, cbData);
	*ppBuffer = pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer; 
	*pcbBuffer = cbData;
	pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft-=cbData;

	pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded += (U32_WMC)cbData;

	if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart == 1)
	{
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart=0;
		return WMA_S_NEWPACKET;
	}

#else
	do
	{
		switch (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload)
		{

		case 1:
			{
				do
				{
					switch (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState)
					{
					case 1: // Compressed payload just started
						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hWMCDec, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset += cbWanted;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize = pData[0];
						
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

						if( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes > pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize+1;
						else if ( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState = 2;
				        pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 

						break;
					case 2: // Subpayload started
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0 && pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == 0)
						{
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =3;
							break;
						}
						else
						{
							if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
							{
								if (/*pInt->dwPayloadLeft*/pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == 0) /* done with the file */
									return WMA_S_NEWPACKET;

								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
									pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC) pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
							}
							if(cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft)
								cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

							*pcbBuffer = WMCDecCBGetData(hWMCDec, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);
                            if (*pcbBuffer != cbData )
                                return WMA_E_NOMOREINPUT;

							memcpy(pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer, pData, cbData);

							*ppBuffer = pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer;

							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset+=*pcbBuffer;
							//pInt->payload.wSubpayloadLeft -= *pcbBuffer;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     -= *pcbBuffer;
                

							if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart) 
							{
								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize - pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign)
								{
#ifdef _V9WMADEC_
									pWmaGetDataParam->m_fTimeIsValid = 1;
									pWmaGetDataParam->m_iTime = 10000 * (I64_WMC)( (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.msObjectPres - pDecoder->tHeaderInfo.msPreroll) + pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.dwDeltaPresTime * pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount);
#endif
									pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount++;
								}
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = FALSE_WMC;
								return WMA_S_NEWPACKET;
							}

							return WMA_OK;
						}

						break;
					case 3: // Subpayload finished
						if ( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes > pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead)
						{ // there are payloads to decode
							cbWanted = 1; //to read subpayload length
							cbActual = WMCDecCBGetData(hWMCDec, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								return WMCDec_BufferTooSmall;
							}

							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset += cbWanted;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart     = TRUE_WMC;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioTimeStamp += (Double_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.dwDeltaPresTime;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 

                    
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize = pData[0];
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;
							if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead+= (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize+1);
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =2;                
						}
						else
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =4; // all subpayloads finished
						break;

					case 4: // All Subpayloads finished

						parse_state = pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload =0;

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						
						err = WMCDecUpdateNewPayload(hWMCDec, Audio_WMC, AudioStreamIndex);
						if (err != WMCDec_Succeeded)
						{
							*pcbBuffer = 0;
                            pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =4;
							return WMA_OK;
						}

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = parse_state;  //restore 
						if((err != WMCDec_Succeeded)) 
							return WMA_S_NEWPACKET;
						break;
					default:
						break;

					}
				} while(pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload);

				break;
			}          
		default :
			{
				if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0 && pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == 0)
				{

					parse_state = pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state;

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload(hWMCDec, Audio_WMC, AudioStreamIndex);
					if (err != WMCDec_Succeeded)
					{
						*pcbBuffer = 0;
                        pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =4;
						return WMA_OK;
					}
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = parse_state; /* restore */
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 
					
					if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload ==1)
						break;
					if(err != WMCDec_Succeeded)
						return WMA_S_NEWPACKET;
				}

				/* return as much as we currently have left */

				if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
				{
					if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == 0)
					{
						/* done with the file */
						return WMA_S_NEWPACKET;
					}

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft -= pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
				}
				if(cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft)
					cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

				*pcbBuffer = WMCDecCBGetData(hWMCDec, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);

                if (*pcbBuffer != cbData )
                    return WMA_E_NOMOREINPUT;

				memcpy(pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer, pData, cbData);

				*ppBuffer = pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer;

				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset+=*pcbBuffer;
				//pInt->payload.wSubpayloadLeft -= *pcbBuffer;
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     -= *pcbBuffer;


				if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart) {
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = FALSE_WMC;
                    //hopefully the following test if the beginning of a payload
                    if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.cbPayloadSize - pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign)
                    {
#ifdef _V9WMADEC_
						pWmaGetDataParam->m_fTimeIsValid = 1;
						pWmaGetDataParam->m_iTime = 10000 * (I64_WMC)( (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.msObjectPres - pDecoder->tHeaderInfo.msPreroll) );
#endif
                    }
					return WMA_S_NEWPACKET;
				}

				return WMA_OK;
			}
		}


	}while(1);

#endif
	return WMA_OK;
}

/***********************************************************************/
// For Arm CE 
int WMA_GetMoreData(unsigned char **ppBuffer, unsigned int *pcbBufLen,
                    unsigned int user_data)
{
    WMARESULT      wmasResult;

    wmasResult = WMARawDecCBGetData (ppBuffer, (unsigned long*)pcbBufLen, (unsigned long)user_data);

    return wmasResult;

}
/******************************************************************************/
#ifndef __NO_SPEECH__
WMSRESULT WMSRawDecCBGetData( U8_WMSRawDec  **ppBuffer, U32_WMSRawDec *pcbBuffer, U32_WMSRawDec dwUserData)
{
    
	tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder =NULL_WMC;
	U32_WMARawDec cbData = MIN_WANTED;
    U16_WMC AudioStreamIndex = 0;
#ifdef WMC_NO_BUFFER_MODE
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    tWMCParseState  parse_state;
#endif
    pDecoder = (WMFDecoderEx *)dwUserData;


    if(pDecoder == NULL_WMC || ppBuffer == NULL_WMC || pcbBuffer == NULL_WMC)
    {
        if(ppBuffer != NULL_WMC)
            *ppBuffer = NULL_WMC;
        if(pcbBuffer != NULL_WMC)
            *pcbBuffer = 0;

        return WMS_E_INVALIDARG;
    }

    *ppBuffer = NULL_WMC;
    *pcbBuffer = 0;
    
    AudioStreamIndex = (U16_WMC)pDecoder->i32CurrentAudioStreamIndex; 

#ifndef WMC_NO_BUFFER_MODE

	if ((pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft ==0) && (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)) // New Payload
	{

		if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded > 0)
		{
			if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded)
			{
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->cbNbFramesAudBuf-=1;
				memmove(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer, pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded,(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset - pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded));
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset - pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded;//pDecoder->dwAudioBufDecoded; 
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded=0;
			}

		}
		
		
		memcpy (&(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime),(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded), sizeof(U32_WMC));
		memcpy (&(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft),(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded += 2*sizeof(U32_WMC);
//		pDecoder->dwAudioTimeStamp = pDecoder->dwAudPayloadPresTime;
        pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 
    }

	if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
	{
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = 1;
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft - pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
	}

	if (cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft )
		cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

	if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded + cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset)
	{
		*ppBuffer = NULL_WMC;
		*pcbBuffer = 0;
		return WMS_S_NEWPACKET;
	}
	

	memcpy(pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer,  pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded, cbData);
	*ppBuffer = pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer; 
	*pcbBuffer = cbData;
	pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft-=cbData;

	pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded += (U32_WMC)cbData;

	if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart == 1)
	{
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart=0;
		return WMS_OK;//WMA_S_NEWPACKET;
	}
#else
	do
	{
		switch (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload)
		{

		case 1:
			{
				do
				{
					switch (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState)
					{
					case 1: // Compressed payload just started
						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData((HWMCDECODER)dwUserData, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							return WMS_S_NEWPACKET;
						}

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset += cbWanted;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize = pData[0];
						
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

						if( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes > pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize+1;
						else if ( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState = 2;
				        pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 

						break;
					case 2: // Subpayload started
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0 && pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == 0)
						{
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =3;
							break;
						}
						else
						{
							if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
							{
								if (/*pInt->dwPayloadLeft*/pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == 0) /* done with the file */
									return WMS_S_NEWPACKET;

								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
									pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC) pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
							}
							if(cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft)
								cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

							*pcbBuffer = WMCDecCBGetData((HWMCDECODER)dwUserData, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);
                            if (*pcbBuffer != cbData )
                                return WMS_S_NO_MORE_SRCDATA;

							memcpy(pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer, pData, cbData);

							*ppBuffer = pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer;

							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset+=*pcbBuffer;
							//pInt->payload.wSubpayloadLeft -= *pcbBuffer;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     -= *pcbBuffer;
                

							if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart) 
							{
								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize - pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign)
								{
									pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount++;
								}
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = FALSE_WMC;
								return WMS_OK;
							}

							return WMS_OK;
						}

						break;
					case 3: // Subpayload finished
						if ( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes > pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead)
						{ // there are payloads to decode
							cbWanted = 1; //to read subpayload length
							cbActual = WMCDecCBGetData((HWMCDECODER)dwUserData, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								return WMS_S_NEWPACKET;
							}

							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset += cbWanted;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart     = TRUE_WMC;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioTimeStamp += (Double_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.dwDeltaPresTime;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 

                    
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize = pData[0];
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;
							if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead+= (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize+1);
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =2;                
						}
						else
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =4; // all subpayloads finished
						break;

					case 4: // All Subpayloads finished

						parse_state = pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload =0;

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						
						err = WMCDecUpdateNewPayload((HWMCDECODER)dwUserData, Audio_WMC, AudioStreamIndex);
						if (err != WMCDec_Succeeded)
						{
							*pcbBuffer = 0;
                            pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =4;
							return WMS_S_NEWPACKET;
						}

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = parse_state;  //restore 
						if((err != WMCDec_Succeeded)) 
							return WMS_S_NEWPACKET;
						break;
					default:
						break;

					}
				} while(pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload);

				break;
			}          
		default :
			{
				if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0 && pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == 0)
				{

					parse_state = pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state;

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload((HWMCDECODER)dwUserData, Audio_WMC, AudioStreamIndex);
					if (err != WMCDec_Succeeded)
					{
						*pcbBuffer = 0;
						return WMS_S_NEWPACKET;
					}
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = parse_state; /* restore */
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bTimeToChangex = TRUE_WMC; 
					
					if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload ==1)
						break;
					if(err != WMCDec_Succeeded)
						return WMS_S_NEWPACKET;
				}

				/* return as much as we currently have left */

				if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
				{
					if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == 0)
					{
						/* done with the file */
						return WMS_S_NEWPACKET;
					}

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft -= pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
				}
				if(cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft)
					cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

				*pcbBuffer = WMCDecCBGetData((HWMCDECODER)dwUserData, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);

                if (*pcbBuffer != cbData )
                    return WMS_S_NO_MORE_SRCDATA;

				memcpy(pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer, pData, cbData);

				*ppBuffer = pDecoder->tAudioStreamInfo[AudioStreamIndex]->bAudioBuffer;

				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset+=*pcbBuffer;
				//pInt->payload.wSubpayloadLeft -= *pcbBuffer;
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     -= *pcbBuffer;


				if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart) {
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = FALSE_WMC;
					return WMS_OK;//WMS_S_NEWPACKET;//WMA_S_NEWPACKET;
				}

				return WMS_OK;
			}
		}


	}while(1);


#endif

	
	return WMS_OK;

}
#endif
/******************************************************************************/

tWMCDecStatus WMCRawGetData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength)
{
    
    WMFDecoderEx *pDecoder =NULL_WMC;
	tWMCDecStatus err = WMCDec_Succeeded;

	U32_WMC cbData = cbDstBufferLength;
#ifdef WMC_NO_BUFFER_MODE
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    tWMCParseState  parse_state;
#endif
    U16_WMC AudioStreamIndex = 0;

    pDecoder = (WMFDecoderEx *)hDecoder;
    *pcbBuffer = 0;

    AudioStreamIndex = (U16_WMC)pDecoder->i32CurrentAudioStreamIndex; 

#ifndef WMC_NO_BUFFER_MODE

    if (((pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft ==1) && (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft==0)) || pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput == TRUE_WMC)
    {
        *pcbBuffer =0;
        pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft =0;
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput = TRUE_WMC;
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->bOutputisReady = FALSE_WMC;
        return WMCDec_Succeeded;
    }

    

    if ((pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft ==0) && (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft==0)) // New Payload
	{

		if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded > 0)
		{
			if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded)
			{
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->cbNbFramesAudBuf-=1;
				memmove(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer, pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded,(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset - pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded));
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset - pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded;//pDecoder->dwAudioBufDecoded; 
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded=0;
			}

		}
		
		
		memcpy (&(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime),(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded), sizeof(U32_WMC));
		memcpy (&(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft),(pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded += 2*sizeof(U32_WMC);
		pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioTimeStamp = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime;
        pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft =1;
	}

	if (cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft )
		cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

	if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded + cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufCurOffset)
	{
		*pcbBuffer = 0;
		return WMCDec_Fail;
	}
	

	memcpy(pBuffer,  pDecoder->tAudioStreamInfo[AudioStreamIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded, cbData);
	*pcbBuffer = cbData;
	pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft-=cbData;

	pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioBufDecoded += (U32_WMC)cbData;
#else

    cbData = MIN_WANTED;
    if (cbDstBufferLength >= MIN_WANTED)
        cbData = MIN_WANTED;
    else
        cbData = cbDstBufferLength;

	do
	{
		switch (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload)
		{

		case 1:
			{
				do
				{
					switch (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState)
					{
					case 1: // Compressed payload just started
						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hDecoder, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset += cbWanted;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize = pData[0];
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwFrameSize = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize ;
         				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioTimeStamp = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime;

						
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

						if( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes > pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize+1;
						else if ( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState = 2;
						break;
					case 2: // Subpayload started
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput == TRUE_WMC)
						{
							*pcbBuffer =0;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bOutputisReady = FALSE_WMC;
							return WMCDec_Succeeded;
						
						}
						if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0 && pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == 0)
						{
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =3;
							*pcbBuffer =0;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput = TRUE_WMC;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bOutputisReady = FALSE_WMC;

							//return WMCDec_Succeeded;
							break;
						}
						else
						{
							if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
							{
								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == 0) /* done with the file */
									return WMCDec_Succeeded;

								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
									pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC) pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE;
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
							}
							if(cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft)
								cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

							*pcbBuffer = WMCDecCBGetData(hDecoder, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);
                            if (*pcbBuffer != cbData )
                                return WMCDec_BufferTooSmall;

							memcpy(pBuffer, pData, cbData);

							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset+=*pcbBuffer;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     -= *pcbBuffer;
                

							if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart) 
							{
								if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft == pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize - pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign)
								{
									pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubCount++;
								}
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = FALSE_WMC;
								return WMCDec_Succeeded;
							}

							return WMCDec_Succeeded;
						}

						break;
					case 3: // Subpayload finished
						
						
						if ( pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wTotalDataBytes > pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead)
						{ // there are payloads to decode
							cbWanted = 1; //to read subpayload length
							cbActual = WMCDecCBGetData(hDecoder, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								return WMCDec_BufferTooSmall;
							}

							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset += cbWanted;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart     = TRUE_WMC;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioTimeStamp += (Double_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.dwDeltaPresTime;

                    
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize = pData[0];
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwFrameSize = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize ;
							
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize;
							if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft > 0)
								pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.wBytesRead+= (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bNextSubPayloadSize+1);
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =2;                
						}
						else
							pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =4; // all subpayloads finished
						break;

					case 4: // All Subpayloads finished

						parse_state = pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bSubPayloadState =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft =0;
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload =0;

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						
						err = WMCDecUpdateNewPayload(hDecoder, Audio_WMC, AudioStreamIndex);
						if (err == WMCDec_DecodeComplete)
						{
							*pcbBuffer = 0;
							return WMCDec_Succeeded;
						}
						if (err != WMCDec_Succeeded)
						{
							*pcbBuffer = 0;
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = parse_state;  //restore 
						break;
					default:
						break;

					}
				} while(pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload);

				break;
			}          
		default :
			{
				if ((pDecoder->tAudioStreamInfo[AudioStreamIndex]->bPayloadGiven == TRUE_WMC) || (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput == TRUE_WMC) )
				{
					*pcbBuffer =0;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bPayloadGiven = FALSE_WMC;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput = TRUE_WMC;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bOutputisReady = FALSE_WMC;

					return WMCDec_Succeeded;
				
				}
				
				if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0 && pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == 0)
				{

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bPayloadGiven = TRUE_WMC;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bGotCompOutput = TRUE_WMC;
					parse_state = pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state;

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload(hDecoder, Audio_WMC, AudioStreamIndex);
					if (err == WMCDec_DecodeComplete)
					{
						*pcbBuffer = 0;
						return WMCDec_Succeeded;
					}
					if (err != WMCDec_Succeeded)
					{
						*pcbBuffer = 0;
						return err;
					}
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->parse_state = parse_state; /* restore */
				
         			pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudioTimeStamp = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwAudPayloadPresTime;

					if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->payload.bIsCompressedPayload ==1)
						break;
					
					continue;

				}

				/* return as much as we currently have left */

				if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft == 0)
				{
					if(pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft == 0)
					{
						/* done with the file */
						return WMCDec_Succeeded;
					}

					pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft -= pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = TRUE_WMC;
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft = pDecoder->tAudioStreamInfo[AudioStreamIndex]->nBlockAlign;
					if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadLeft ==0)
						pDecoder->tAudioStreamInfo[AudioStreamIndex]->bPayloadGiven = TRUE_WMC;
				}
				if(cbData > pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft)
					cbData = pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft;

				*pcbBuffer = WMCDecCBGetData(hDecoder, pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);

                if (*pcbBuffer != cbData )
                    return WMCDec_BufferTooSmall;

				memcpy(pBuffer, pData, cbData);

				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwPayloadOffset+=*pcbBuffer;
				//pInt->payload.wSubpayloadLeft -= *pcbBuffer;
				pDecoder->tAudioStreamInfo[AudioStreamIndex]->dwBlockLeft     -= *pcbBuffer;


				if (pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart) {
					pDecoder->tAudioStreamInfo[AudioStreamIndex]->bBlockStart = FALSE_WMC;
					return WMCDec_Succeeded;//WMA_S_NEWPACKET;
				}

				return WMCDec_Succeeded;
			}
		}


	}while(1);


#endif

	return WMCDec_Succeeded;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _ASFPARSE_ONLY_
tWMVDecodeStatus WMVDecCBGetData ( U32_WMV uiUserData, U32_WMV uintPadBeginning, U8_WMV **ppchBuffer, U32_WMV uintUserDataLength, 
                                  U32_WMV* puintActualBufferLength, Bool_WMV*    pbNotEndOfFrame )
{

    WMFDecoderEx *pDecoder =NULL_WMC;
	U32_WMV cbData = MIN_WANTED;
    U16_WMC VideoStreamIndex = 0;
    tWMVDecodeStatus Status = WMV_Succeeded;
#ifdef WMC_NO_BUFFER_MODE
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    tWMCParseState  parse_state;
	tWMCDecStatus err = WMCDec_Succeeded;
#endif
    

    pDecoder = (WMFDecoderEx *)uiUserData;



    if(pDecoder == NULL_WMC || ppchBuffer == NULL_WMC || puintActualBufferLength == NULL_WMC || pbNotEndOfFrame == NULL_WMC)
    {
        if(ppchBuffer != NULL_WMC)
            *ppchBuffer = NULL_WMC;
        if(puintActualBufferLength != NULL_WMC)
            *puintActualBufferLength = 0;

        if(pbNotEndOfFrame != NULL_WMC)
            *pbNotEndOfFrame = FALSE_WMV;

        return WMV_InValidArguments;
    }

    *ppchBuffer = NULL_WMC;
    *puintActualBufferLength = 0;
    *pbNotEndOfFrame = TRUE_WMV;
    VideoStreamIndex = (U16_WMC)pDecoder->i32CurrentVideoStreamIndex; 

#ifndef WMC_NO_BUFFER_MODE

    if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bInitStage == TRUE_WMC)
    {
        if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft ==0) //First Time
        {
            pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->biSize -  sizeof (BITMAPINFOHEADER);
            pDecoder->tVideoStreamInfo[VideoStreamIndex]->bBlockStart = TRUE_WMC;
        }
	    if (cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft )
        {
		    cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
            *pbNotEndOfFrame = FALSE_WMV;
            Status = WMV_EndOfFrame;
        }

	    if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded + cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset)
	    {
            *ppchBuffer = NULL_WMC;
            *puintActualBufferLength = 0;
            *pbNotEndOfFrame = TRUE_WMV;
            return WMV_EndOfFrame;
	    }
	    

	    memcpy(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer + uintPadBeginning ,  pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded, cbData);
	    *ppchBuffer = pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer; 
	    *puintActualBufferLength = cbData;
	    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft-=cbData;
	    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (U32_WMC)cbData;
        return Status;
    }
    else
    {
        if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft ==0)
        {

		    if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded > 0)
		    {
			    if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset >pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded)
			    {
				    pDecoder->tVideoStreamInfo[VideoStreamIndex]->cbNbFramesVidBuf-=1;
				    memmove(pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer, pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded,(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset - pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded));
				    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset - pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded;//pDecoder->dwAudioBufDecoded; 
				    if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->pStoreFrameStartPointer != NULL_WMC)
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->pStoreFrameStartPointer -=pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded=0;
			    }

		    }
//			do
//			{
				memcpy (&(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp),(pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded), sizeof(U32_WMC));
				memcpy (&( pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft),( pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer +  pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded + 2*sizeof(U32_WMC) ), sizeof(U32_WMC));
g_iVideoFrameSize = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (3*sizeof(U32_WMC));
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->bBlockStart = TRUE_WMC;
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->bIsKeyFrame = pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer[pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded];
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += sizeof(U8_WMC);
//			}while (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft ==0);
			
        }
	    if (cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft )
        {
		    cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
            *pbNotEndOfFrame = FALSE_WMV;
            Status = WMV_EndOfFrame;
        }

	    if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded + cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset)
	    {
            *ppchBuffer = NULL_WMC;
            *puintActualBufferLength = 0;
            *pbNotEndOfFrame = TRUE_WMV;
		    return WMV_EndOfFrame;
	    }
	    

	    memcpy(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer + uintPadBeginning ,  pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded, cbData);
	    *ppchBuffer = pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer; 
	    *puintActualBufferLength = cbData + uintPadBeginning;
	    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft-=cbData;
        if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft ==0)
        {
            *pbNotEndOfFrame = FALSE_WMV;
            Status = WMV_EndOfFrame;
        }

	    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (U32_WMC)cbData;
        return Status;
    }
#else
    if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bInitStage == TRUE_WMC)
    {
        if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft ==0) //First Time
        {
            pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->biSize -  sizeof (BITMAPINFOHEADER);
            pDecoder->tVideoStreamInfo[VideoStreamIndex]->bBlockStart = TRUE_WMC;
        }
	    if (cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft )
        {
		    cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
            *pbNotEndOfFrame = FALSE_WMV;
            Status = WMV_EndOfFrame;
        }
	    

	    memcpy(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer + uintPadBeginning ,  pDecoder->tVideoStreamInfo[VideoStreamIndex]->bAdditionalInfo + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded, cbData);
	    *ppchBuffer = pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer; 
	    *puintActualBufferLength = cbData;
	    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft-=cbData;
	    pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (U32_WMC)cbData;
        return Status;
    }
	else
	{
		do
		{
			switch (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload)
			{

			case 1:
				{
					do
					{
						switch (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState)
						{
						case 1: // Compressed payload just started
							cbWanted = 1; //to read subpayload length
							cbActual = WMCDecCBGetData((HWMCDECODER)uiUserData, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								*ppchBuffer = NULL_WMC;
								*puintActualBufferLength = 0;
								*pbNotEndOfFrame = TRUE_WMV;
								return WMV_EndOfFrame;

							}

							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset += cbWanted;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize = pData[0];
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubCount =0;
g_iVideoFrameSize = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
							
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
							if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft > 0)
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

							if( pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wTotalDataBytes > pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize)
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize+1;
							else if ( pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wTotalDataBytes == pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize)
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;

							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState = 2;
							break;
						case 2: // Subpayload started
							if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft == 0 && pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft == 0)
							{
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =3;
								break;
							}
							else
							{
								if(cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft)
									cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

								cbActual = WMCDecCBGetData((HWMCDECODER)uiUserData, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);
								if (cbActual != cbData )
								{
									*ppchBuffer = NULL_WMC;
									*puintActualBufferLength = 0;
									*pbNotEndOfFrame = TRUE_WMV;
									return WMV_EndOfFrame;

								}


								memcpy(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer + uintPadBeginning , pData, cbData);
								*ppchBuffer = pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer; 
								*puintActualBufferLength = cbData + uintPadBeginning;

								pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset +=cbActual;
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft     -= cbActual;
                
								if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft == 0)
								{
									pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwNextVideoTimeStamp = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp + (U32_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.dwDeltaPresTime;
									*pbNotEndOfFrame = FALSE_WMV;
									return WMV_EndOfFrame;
								}
								return Status;

							}

							break;
						case 3: // Subpayload finished
							if ( pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wTotalDataBytes > pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead)
							{ // there are payloads to decode
								cbWanted = 1; //to read subpayload length
								cbActual = WMCDecCBGetData((HWMCDECODER)uiUserData, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

								if(cbActual != cbWanted)
								{
									*ppchBuffer = NULL_WMC;
									*puintActualBufferLength = 0;
									*pbNotEndOfFrame = TRUE_WMV;
									return WMV_EndOfFrame;
								}

								pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset += cbWanted;
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize = pData[0];
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft     = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize ;
g_iVideoFrameSize = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
							//	pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp += (U32_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.dwDeltaPresTime;

                    
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
								if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft > 0)
									pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead+= (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize+1);
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =2;                
							}
							else
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =4; // all subpayloads finished
							break;

						case 4: // All Subpayloads finished

							parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =0;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadLeft =0;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload =0;

							pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
							
							err = WMCDecUpdateNewPayload((HWMCDECODER)uiUserData, Video_WMC, VideoStreamIndex);
							if (err != WMCDec_Succeeded)
							{
                                if (err == WMCDec_DecodeComplete)
                                {
                                    err = WMCDec_Succeeded;
                                }
                                else
                                {
                                    *ppchBuffer = NULL_WMC;
                                    *puintActualBufferLength = 0;
                                    *pbNotEndOfFrame = TRUE_WMV;
                                    return WMV_EndOfFrame;
                                }
							}

							pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state;  //restore 
							break;
						default:
						//	*ppchBuffer = NULL_WMC;
						//	*puintActualBufferLength = 0;
						//	*pbNotEndOfFrame = TRUE_WMV;
						//	return WMV_EndOfFrame;
							break;

						}
					} while(pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload /*1*/);

					break;
				}          
			default :
				{

					if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft == 0)
					{

						parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						err = WMCDecUpdateNewPayload((HWMCDECODER)uiUserData, Video_WMC, VideoStreamIndex);
						if (err != WMCDec_Succeeded)
						{
                            if (err == WMCDec_DecodeComplete)
                            {
                                err = WMCDec_Succeeded;
                            }
                            else
                            {
                                *ppchBuffer = NULL_WMC;
                                *puintActualBufferLength = 0;
                                *pbNotEndOfFrame = TRUE_WMV;
                                return WMV_EndOfFrame;
                            }
						}

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state; /* restore */
						
						if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload ==1)
							break;
					}
g_iVideoFrameSize = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameSize;

					/* return as much as we currently have left */

					if(cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft)
						cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

					cbActual = WMCDecCBGetData((HWMCDECODER)uiUserData, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);

					if (cbActual != cbData )
					{
						*ppchBuffer = NULL_WMC;
						*puintActualBufferLength = 0;
						*pbNotEndOfFrame = TRUE_WMV;
						return WMV_EndOfFrame;

					}

					memcpy(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer + uintPadBeginning , pData, cbData);
					*ppchBuffer = pDecoder->tVideoStreamInfo[VideoStreamIndex]->bVideoBuffer; 
					*puintActualBufferLength = cbData + uintPadBeginning;

					pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset +=cbActual;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft     -= cbActual;

					pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft -=cbActual;
					
/*					if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft ==0)
					{
						*pbNotEndOfFrame = FALSE_WMV;
						return WMV_EndOfFrame;
					
					} */

					if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft ==0)
					{
					    U32_WMC    dwVideoTimeStamp =0;
						dwVideoTimeStamp = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp;

						parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						err = WMCDecUpdateNewPayload((HWMCDECODER)uiUserData, Video_WMC, VideoStreamIndex);

                        if (err != WMCDec_Succeeded)
                        {
                            if (err == WMCDec_DecodeComplete)
                            {
                                err = WMCDec_Succeeded;
                            }
                        }
                        else
                            pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state;

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwNextVideoTimeStamp = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp = dwVideoTimeStamp;


						*pbNotEndOfFrame = FALSE_WMV;
						return WMV_EndOfFrame;
					
					}



					return Status;
				}
			}


		}while(1);
	}


#endif
    
	return Status;

}
#endif //_ASFPARSE_ONLY_
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tWMCDecStatus WMCDecGetAudioOutput (HWMCDECODER hWMCDec, I16_WMC *pi16Channel0, I16_WMC* pi16Channel1,U32_WMC nNumSamplesRequest, 
                                    U32_WMC* nNumSamplesReturn, I64_WMC* ptPresentationTime )
{

    WMFDecoderEx *pDecoder;
	tWMCDecStatus rc = WMCDec_Succeeded;
    WMARESULT wmar = WMA_OK;
#ifndef __NO_SPEECH__
    WMSRESULT wmsr = WMS_OK;
#endif
    U16_WMC i=0;
    U32_WMC BlockAlign =0;

    if ((hWMCDec == NULL_WMC)||(pi16Channel0 == NULL_WMC) ||(nNumSamplesReturn == NULL_WMC))
        return WMCDec_InValidArguments;

//    if (cbDstBufferLength <1024 )
//        return WMPERR_BUFTOOSMALL;

    pDecoder = (WMFDecoderEx *) hWMCDec;

    i = (U16_WMC)pDecoder->i32CurrentAudioStreamIndex; 
    
    BlockAlign = pDecoder->tAudioStreamInfo[i]->nChannels*pDecoder->tAudioStreamInfo[i]->nBitsPerSample/8;
#ifndef _ASFPARSE_ONLY_
    if (pDecoder->tAudioStreamInfo[i]->hMSA != NULL)
    {
        
        if(pDecoder->tAudioStreamInfo[i]->nVersion > 0 && pDecoder->tAudioStreamInfo[i]->nVersion < 4)
        {
        
        
            if (pi16Channel1 == NULL_WMC || pi16Channel1 == pi16Channel0 + 1 || pDecoder->tAudioStreamInfo[i]->nChannels <= 1 )
            {		// caller wants interleaved data or this sound is mono

                if (ptPresentationTime != NULL_WMC)
					*ptPresentationTime = (U64_WMC)pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp - pDecoder->tHeaderInfo.msPreroll;
				*nNumSamplesReturn = 2048;

                wmar = WMARawDecGetPCM (pDecoder->tAudioStreamInfo[i]->hMSA,
                    (U16_WMARawDec*)nNumSamplesReturn,
                    (U8_WMARawDec*)pi16Channel0,
                    (U32_WMARawDec) nNumSamplesRequest*BlockAlign
    #ifdef _V9WMADEC_
				    ,ptPresentationTime
    #endif
                    );

//				if (nNumSamplesReturn ==0)
//					pDecoder->tAudioStreamInfo[i]->bOutputisReady = FALSE_WMC;


    #ifdef _V9WMADEC_
                if (ptPresentationTime != NULL_WMC)
					*ptPresentationTime = *ptPresentationTime/10000;
				
				pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp = (Double_WMC)*ptPresentationTime + pDecoder->tHeaderInfo.msPreroll;;
	#else

                if ((pDecoder->tAudioStreamInfo[i]->bTimeToChange == TRUE_WMC) && (pDecoder->tAudioStreamInfo[i]->bTimeToChangex == TRUE_WMC)&& (*nNumSamplesReturn ==0))
                {
			        pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp =  pDecoder->tAudioStreamInfo[i]->dwAudPayloadPresTime;
                    pDecoder->tAudioStreamInfo[i]->bTimeToChange = FALSE_WMC;
                    pDecoder->tAudioStreamInfo[i]->bTimeToChangex = FALSE_WMC;
                }

                else
                    pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp += ((Double_WMC)(*nNumSamplesReturn)*1000.0/pDecoder->tAudioStreamInfo[i]->nSamplesPerSec);

	#endif
 
                if (wmar != WMA_OK)
                    return WMADec_Failed;
       
            }
            else
            {
		        U16_WMC j;
		        I16_WMC *pi16C0src = pi16Channel0;
		        I16_WMC *pi16C0dst = pi16Channel0;
		        I16_WMC *pi16C1    = pi16Channel1;

                if (ptPresentationTime != NULL_WMC)
					*ptPresentationTime = (U64_WMC)pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp - pDecoder->tHeaderInfo.msPreroll;

                wmar = WMARawDecGetPCM (pDecoder->tAudioStreamInfo[i]->hMSA,
                    (U16_WMARawDec*)nNumSamplesReturn,
                    (U8_WMARawDec*)pi16Channel0,
                    (U32_WMARawDec) nNumSamplesRequest*BlockAlign
    #ifdef _V9WMADEC_
				    ,ptPresentationTime
    #endif
                    );

//				if (nNumSamplesReturn ==0)
//					pDecoder->tAudioStreamInfo[i]->bOutputisReady = FALSE_WMC;



    #ifdef _V9WMADEC_
                if (ptPresentationTime != NULL_WMC)
					*ptPresentationTime = *ptPresentationTime/10000;
  
				pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp = (Double_WMC)*ptPresentationTime +pDecoder->tHeaderInfo.msPreroll;;

	#else
                if ((pDecoder->tAudioStreamInfo[i]->bTimeToChange == TRUE_WMC)&& (pDecoder->tAudioStreamInfo[i]->bTimeToChangex == TRUE_WMC) && (*nNumSamplesReturn ==0))
                {
			        pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp =  pDecoder->tAudioStreamInfo[i]->dwAudPayloadPresTime;
                    pDecoder->tAudioStreamInfo[i]->bTimeToChange = FALSE_WMC;
                    pDecoder->tAudioStreamInfo[i]->bTimeToChangex = FALSE_WMC;
                }
                else
                    pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp += ((Double_WMC)(*nNumSamplesReturn)*1000.0/pDecoder->tAudioStreamInfo[i]->nSamplesPerSec);

	#endif

                if (wmar == WMA_OK) 
                {
			        for (j = 0; j < *nNumSamplesReturn; j++) 
                    {
				        *pi16C0dst++ = *pi16C0src++;
				        *pi16C1++ = *pi16C0src++;
			        }
		        }
      
                if (wmar != WMA_OK)
                    return WMADec_Failed;

            }
        }
#ifndef __NO_SPEECH__
        else if (pDecoder->tAudioStreamInfo[i]->nVersion == 4) // Speech Codec Version
        {

            if (ptPresentationTime != NULL_WMC)
	            *ptPresentationTime = (U64_WMC)pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp - pDecoder->tHeaderInfo.msPreroll;
            *nNumSamplesReturn =2048;
            wmsr = WMSRawDecGetPCM ((WMSRawDecHandle) pDecoder->tAudioStreamInfo[i]->hMSA, (U16_WMSRawDec*)nNumSamplesReturn, 
                                  (U8_WMSRawDec*)pi16Channel0, (U32_WMARawDec) nNumSamplesRequest*BlockAlign);
            
			if( wmsr != WMS_OK )
            {
                return WMADec_Failed;
            }
//			if (nNumSamplesReturn ==0)
//				pDecoder->tAudioStreamInfo[i]->bOutputisReady = FALSE_WMC;

        
            if ((pDecoder->tAudioStreamInfo[i]->bTimeToChange == TRUE_WMC) && (pDecoder->tAudioStreamInfo[i]->bTimeToChangex == TRUE_WMC) && (*nNumSamplesReturn ==0))
            {
			    pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp =  pDecoder->tAudioStreamInfo[i]->dwAudPayloadPresTime;
                pDecoder->tAudioStreamInfo[i]->bTimeToChange = FALSE_WMC;
                pDecoder->tAudioStreamInfo[i]->bTimeToChangex = FALSE_WMC;
            }
            else
                pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp += ((Double_WMC)(*nNumSamplesReturn)*1000.0/pDecoder->tAudioStreamInfo[i]->nSamplesPerSec);
        }
#endif          
        
    }
    else
#endif //_ASFPARSE_ONLY_
    {

        rc = WMCRawGetData ((U8_WMC*)pi16Channel0, (U32_WMC *) nNumSamplesReturn, hWMCDec, nNumSamplesRequest*BlockAlign);
        if (ptPresentationTime != NULL_WMC)
	        *ptPresentationTime = (U64_WMC)pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp - pDecoder->tHeaderInfo.msPreroll;
 //       *nNumSamplesReturn = *nNumSamplesReturn/BlockAlign; // Amit 09/20/01 In case of compressed, it should be bytes, not samples
//        pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp += ((*nNumSamplesReturn)*1000/pDecoder->tAudioStreamInfo[i]->nSamplesPerSec);
   
    }

    return rc;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tWMCDecStatus WMCDecGetVideoOutput (HWMCDECODER hWMCDec, U8_WMC *pu8VideoOutBuffer, U32_WMC u32VideoOutBufferSize, U32_WMC * pu32OutDataSize, I64_WMC* ptPresentationTime, Bool_WMC *bIsKeyFrame, tWMCDecodeDispRotateDegree tDispRotateDegree)
{

    WMFDecoderEx *pDecoder;
	tWMCDecStatus rc = WMCDec_Succeeded;
#ifndef _ASFPARSE_ONLY_
    tWMVDecodeStatus VidStatus;
#endif
    U16_WMC i = 0;

    

    if ((hWMCDec == NULL_WMC)/*||(pu8VideoOutBuffer == NULL_WMC)||(ptPresentationTime == NULL_WMC)*/ || (pu32OutDataSize == NULL))
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *) hWMCDec;
    *pu32OutDataSize =0;
	if (bIsKeyFrame != NULL_WMC)
		*bIsKeyFrame  = FALSE_WMC;
// Output of which stream? Do it tomarrow.....................
    i = (U16_WMC)pDecoder->i32CurrentVideoStreamIndex; 

#ifndef _ASFPARSE_ONLY_
    if (pDecoder->tVideoStreamInfo[i]->pVDec != NULL) // Not a compressed output
    {
        
        if(
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV2 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV1 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMS2 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMS1 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_M4S2 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP4S ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP43 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP42 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MPG4 )
        {
           
			
		//	if (pu8VideoOutBuffer !=NULL)
		//	{			
			pDecoder->tDispRotateDegree = tDispRotateDegree;	
			
			VidStatus = WMVideoDecGetOutput (
					(HWMVDecoder)pDecoder->tVideoStreamInfo[i]->pVDec,
					(U32_WMV)pDecoder->tVideoStreamInfo[i]->biCompressionOut,
#if defined (_ARM_) || defined (_SH3_) 
                    (U16_WMV)((pDecoder->tVideoStreamInfo[i]->biCompressionOut == FOURCC_BI_BITFIELDS && 
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut == 16)? 12 : pDecoder->tVideoStreamInfo[i]->biBitCountOut),
#else
					(U16_WMV)pDecoder->tVideoStreamInfo[i]->biBitCountOut,
#endif //(_ARM_) || defined (_SH3_)
                    // 0,
					// (tWMVDecodeDispRotateDegree)pDecoder->tDispRotateDegree,
					pu8VideoOutBuffer
                    );

				switch(VidStatus)
				{
				case WMV_NoMoreOutput:
					*pu32OutDataSize = 0;;
					rc = WMCDec_Succeeded;
					break;
				case WMV_EndOfFrame:
					break;
				case WMV_BrokenFrame:
					rc = WMVDec_BrokenFrame;
					break;
				case WMV_Succeeded:
					*pu32OutDataSize = (pDecoder->tVideoStreamInfo[i]->biBitCountOut*pDecoder->tVideoStreamInfo[i]->biWidth*pDecoder->tVideoStreamInfo[i]->biHeight)/8;
					if (bIsKeyFrame !=NULL_WMC)
						*bIsKeyFrame = TRUE_WMC;
					rc = WMCDec_Succeeded;
					break;
				default:
					rc = WMVDec_Failed;
				}
		//	}
		//	else
		//	{
		//		*pu32OutDataSize = 0;;
		//		rc = WMCDec_Succeeded;
		//	}
            if (ptPresentationTime != NULL_WMC)
				*ptPresentationTime = pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp - pDecoder->tHeaderInfo.msPreroll;
        }
#ifndef __NO_SCREEN__
        else if(pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS1 ||
				pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS2)
        {
	        tWMCDecStatus TempStatus = WMCDec_Succeeded;
        
            TempStatus = WMScGetOutData(hWMCDec, pu8VideoOutBuffer, u32VideoOutBufferSize, i );

            switch(TempStatus)
            {
            case WMVDec_NoMoreOutput:
                *pu32OutDataSize = 0;;
                rc = WMCDec_Succeeded;
                break;
            case WMCDec_Succeeded:
                *pu32OutDataSize = (pDecoder->tVideoStreamInfo[i]->biBitCountOut*pDecoder->tVideoStreamInfo[i]->biWidth*pDecoder->tVideoStreamInfo[i]->biHeight)/8;
				if (bIsKeyFrame !=NULL_WMC)
					*bIsKeyFrame = TRUE_WMC;
                rc = WMCDec_Succeeded;
                break;
            default:
                rc = WMVDec_Failed;
            }
        
            if (ptPresentationTime != NULL_WMC)
				*ptPresentationTime = pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp - pDecoder->tHeaderInfo.msPreroll;
        }
#endif
    }
    else // Compressed or Raw output
#endif //_ASFPARSE_ONLY_
    {
    
        rc = WMCRawGetVideoData (pu8VideoOutBuffer, (U32_WMC *) pu32OutDataSize, hWMCDec, u32VideoOutBufferSize);
        
        if (ptPresentationTime != NULL_WMC)
			*ptPresentationTime = pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp  - pDecoder->tHeaderInfo.msPreroll;

		if (bIsKeyFrame !=NULL_WMC)
			*bIsKeyFrame = pDecoder->tVideoStreamInfo[i]->bIsKeyFrame;

    }


    return rc;
}


/******************************************************************************/

tWMCDecStatus WMCRawGetVideoData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength)
{
    
    WMFDecoderEx *pDecoder =NULL;
	U32_WMC cbData = cbDstBufferLength;
    U16_WMC VideoStreamIndex = 0;

#ifdef WMC_NO_BUFFER_MODE
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    tWMCParseState  parse_state;
	tWMCDecStatus err = WMCDec_Succeeded;
#endif

    pDecoder = (WMFDecoderEx *)hDecoder;
    *pcbBuffer = 0;

    VideoStreamIndex = (U16_WMC)pDecoder->i32CurrentVideoStreamIndex; 

#ifndef WMC_NO_BUFFER_MODE
    if (((pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadLeft ==1) && (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft==0)) ||(pDecoder->tVideoStreamInfo[VideoStreamIndex]->bGotCompOutput == TRUE_WMC))
    {
        *pcbBuffer =0;
        pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadLeft =0;
		pDecoder->tVideoStreamInfo[VideoStreamIndex]->bGotCompOutput = TRUE_WMC;
		pDecoder->tVideoStreamInfo[VideoStreamIndex]->bOutputisReady = FALSE_WMC;
        return WMCDec_Succeeded;
    }

    if ((pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadLeft ==0) && (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft==0)) // New Payload
	{

		if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded > 0)
		{
			if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded)
			{
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->cbNbFramesVidBuf-=1;
				memmove(pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer, pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded,(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset - pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded));
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset - pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded;//pDecoder->dwVideoBufDecoded; 
				if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->pStoreFrameStartPointer != NULL_WMC)
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->pStoreFrameStartPointer -=pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded;
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded=0;
			}

		}
		
		
		memcpy (&(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp),(pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded), sizeof(U32_WMC));
		memcpy (&(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft),(pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded + 2*sizeof(U32_WMC) ), sizeof(U32_WMC));

		pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (3*sizeof(U32_WMC));
		pDecoder->tVideoStreamInfo[VideoStreamIndex]->bIsKeyFrame = pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer[pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded];
		pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (sizeof(U8_WMC)) ;
        pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadLeft =1;
	}

	if (cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft )
		cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

	if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded + cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufCurOffset)
	{
		*pcbBuffer = 0;
		return WMCDec_Fail;
	}
	

	if (pBuffer !=NULL)
		memcpy(pBuffer,  pDecoder->tVideoStreamInfo[VideoStreamIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded, cbData);
	*pcbBuffer = cbData;
	pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft-=cbData;

	pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoBufDecoded += (U32_WMC)cbData;
#else

    cbData = MIN_WANTED;
    if (cbDstBufferLength >= MIN_WANTED)
        cbData = MIN_WANTED;
    else
        cbData = cbDstBufferLength;

	do
	{
		switch (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload)
		{
		case 1:
			{
				do
				{
					switch (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState)
					{
					case 1: // Compressed payload just started
						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hDecoder, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							*pcbBuffer = 0;
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset += cbWanted;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize = pData[0];
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameSize = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubCount =0;
						
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
						if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft > 0)
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

						if( pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wTotalDataBytes > pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize+1;
						else if ( pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wTotalDataBytes == pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState = 2;
						break;
					case 2: // Subpayload started
						// if it is coming from normal payload to compressed payload
						if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->bGotCompOutput == TRUE_WMC)
						{
							*pcbBuffer =0;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->bOutputisReady = FALSE_WMC;
							return WMCDec_Succeeded;
						
						}
						
						if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft == 0 && pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft == 0)
						{
							*pcbBuffer = 0;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->bGotCompOutput = TRUE_WMC;						
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =3;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->bOutputisReady = FALSE_WMC;
						//	pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwNextVideoTimeStamp = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp + (U32_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.dwDeltaPresTime;
							//return WMCDec_Succeeded;
							break;
						}
						else
						{
							if(cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft)
								cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

							cbActual = WMCDecCBGetData(hDecoder, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);
							if (cbActual != cbData )
							{
								*pcbBuffer = 0;
								return WMCDec_BufferTooSmall;

							}

							memcpy(pBuffer, pData, cbData);
							*pcbBuffer = cbData;

							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset +=cbActual;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft     -= cbActual;
        
							return WMCDec_Succeeded;

						}

						break;
					case 3: // Subpayload finished
					
						
						if ( pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wTotalDataBytes > pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead)
						{ // there are payloads to decode
							cbWanted = 1; //to read subpayload length
							cbActual = WMCDecCBGetData(hDecoder, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								*pcbBuffer = 0;
								return WMCDec_BufferTooSmall;

							}

							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset += cbWanted;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize = pData[0];
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft     = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize ;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameSize = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp += (U32_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.dwDeltaPresTime;

            
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft = pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize;
							if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft > 0)
								pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.wBytesRead+= (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bNextSubPayloadSize+1);
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =2;                
						}
						else
							pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =4; // all subpayloads finished
						break;

					case 4: // All Subpayloads finished

						parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bSubPayloadState =0;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadLeft =0;
						pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload =0;

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						
						err = WMCDecUpdateNewPayload(hDecoder, Video_WMC, VideoStreamIndex);
						if (err == WMCDec_DecodeComplete)
						{
							*pcbBuffer = 0;
							return WMCDec_Succeeded;
						}
						if (err != WMCDec_Succeeded)
						{
							*pcbBuffer = 0;
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state;  //restore 
						break;
					default:
						break;

					}
				} while(pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload /*1*/);

				break;
			}          
		default :
			{
				if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->bGotCompOutput == TRUE_WMC)
				{
					*pcbBuffer =0;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->bOutputisReady = FALSE_WMC;
					return WMCDec_Succeeded;
				
				}


				if((pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft == 0) && (pDecoder->tVideoStreamInfo[VideoStreamIndex]->bFrameGiven == TRUE_WMC))
				{
					*pcbBuffer = 0;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->bGotCompOutput = TRUE_WMC;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->bFrameGiven = FALSE_WMC;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->bOutputisReady = FALSE_WMC;

					if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft == 0)
					{

						parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						err = WMCDecUpdateNewPayload(hDecoder, Video_WMC, VideoStreamIndex);
						if (err != WMCDec_Succeeded)
						{
							*pcbBuffer = 0;
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state; /* restore */
						
						if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload ==1)
							break;
					}
				
				
					return WMCDec_Succeeded;
				
				
				}
				
				
				
				if(pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft == 0)
				{

					parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;

					pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload(hDecoder, Video_WMC, VideoStreamIndex);
					if (err == WMCDec_DecodeComplete)
					{
						*pcbBuffer = 0;
						return WMCDec_Succeeded;
					}
					if (err != WMCDec_Succeeded)
					{
						*pcbBuffer = 0;
						return WMCDec_BufferTooSmall;
					}

					pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state; /* restore */
					
					if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->payload.bIsCompressedPayload ==1)
						break;
				}

				/* return as much as we currently have left */

				if(cbData > pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft)
					cbData = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft;

				cbActual = WMCDecCBGetData(hDecoder, pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);

				if (cbActual != cbData )
				{
					*pcbBuffer = 0;
					return WMCDec_BufferTooSmall;

				}

				memcpy(pBuffer, pData, cbData);
				*pcbBuffer = cbData;

				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwPayloadOffset +=cbActual;
				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwBlockLeft     -= cbActual;

				pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft -=cbActual;
				
				if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft ==0)
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->bFrameGiven = TRUE_WMC;
/*				
				if (pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwFrameLeft ==0)
				{
				
					U32_WMC    dwVideoTimeStamp =0;
					dwVideoTimeStamp = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp;

					parse_state = pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state;

					pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload((HWMCDECODER)hDecoder, Video_WMC, VideoStreamIndex);
					if (err != WMCDec_Succeeded)
						return err;
					
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwNextVideoTimeStamp = pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp;
					pDecoder->tVideoStreamInfo[VideoStreamIndex]->dwVideoTimeStamp = dwVideoTimeStamp;


					pDecoder->tVideoStreamInfo[VideoStreamIndex]->parse_state = parse_state; 

					pDecoder->tVideoStreamInfo[VideoStreamIndex]->bFrameGiven = TRUE_WMC;
				}*/
			
				
				
				return WMCDec_Succeeded;
			}
		}


	}while(1);

#endif
	return WMCDec_Succeeded;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tWMCDecStatus WMCDecGetBinaryOutput (HWMCDECODER hWMCDec, U8_WMC *pu8BinaryOutBuffer, U32_WMC nSizeRequest, U32_WMC * nSizeReturn, I64_WMC* ptPresentationTime)
{

    WMFDecoderEx *pDecoder;
	tWMCDecStatus rc = WMCDec_Succeeded;
    U16_WMC i = 0;
    

    if ((hWMCDec == NULL_WMC)||(pu8BinaryOutBuffer == NULL_WMC)/*||(ptPresentationTime == NULL_WMC)*/ || (nSizeReturn == NULL))
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *) hWMCDec;

    i = (U16_WMC)pDecoder->i32CurrentBinaryStreamIndex; 

    rc = WMCRawGetBinaryData (pu8BinaryOutBuffer, (U32_WMC *) nSizeReturn, hWMCDec, nSizeRequest);
    if (ptPresentationTime != NULL_WMC)
		*ptPresentationTime = pDecoder->tBinaryStreamInfo[i]->dwBinaryTimeStamp - pDecoder->tHeaderInfo.msPreroll;


    return rc;
}

/******************************************************************************/

tWMCDecStatus WMCRawGetBinaryData (U8_WMC *pBuffer, U32_WMC *pcbBuffer, HWMCDECODER hDecoder, U32_WMC cbDstBufferLength)
{
    
    WMFDecoderEx *pDecoder =NULL;
	U32_WMC cbData = cbDstBufferLength;
    U16_WMC BinaryStreamIndex = 0;

#ifdef WMC_NO_BUFFER_MODE
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    tWMCParseState  parse_state;
	tWMCDecStatus err = WMCDec_Succeeded;
#endif


    pDecoder = (WMFDecoderEx *)hDecoder;
    *pcbBuffer = 0;

    BinaryStreamIndex = (U16_WMC)pDecoder->i32CurrentBinaryStreamIndex; 

#ifndef WMC_NO_BUFFER_MODE
    if (((pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadLeft ==1) && (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft==0)) || (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bGotCompOutput == TRUE_WMC))
    {
        *pcbBuffer =0;
        pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadLeft =0;
 		pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bGotCompOutput = TRUE_WMC;
		pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bOutputisReady = FALSE_WMC;
       return WMCDec_Succeeded;
    }

    if ((pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadLeft ==0) && (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft==0)) // New Payload
	{

		if(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded > 0)
		{
			if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufCurOffset > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded)
			{
				pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->cbNbFramesBinBuf-=1;
				memmove(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->pbBinaryBuffer, pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded,(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufCurOffset - pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded));
				pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufCurOffset = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufCurOffset - pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded;//pDecoder->dwBinaryBufDecoded; 
				pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded=0;
			}

		}
		
		
		memcpy (&(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryTimeStamp),(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded), sizeof(U32_WMC));
		memcpy (&(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft),(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));
		pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded += 2*sizeof(U32_WMC);
        pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadLeft =1;
	}

	if (cbData > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft )
		cbData = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft;

	if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded + cbData > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufCurOffset)
	{
		*pcbBuffer = 0;
		return WMCDec_Fail;
	}
	

	memcpy(pBuffer,  pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded, cbData);
	*pcbBuffer = cbData;
	pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft-=cbData;

	pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryBufDecoded += (U32_WMC)cbData;

#else

    cbData = MIN_WANTED;
    if (cbDstBufferLength >= MIN_WANTED)
        cbData = MIN_WANTED;
    else
        cbData = cbDstBufferLength;

	do
	{
		switch (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bIsCompressedPayload)
		{
		case 1:
			{
				do
				{
					switch (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bSubPayloadState)
					{
					case 1: // Compressed payload just started
						cbWanted = 1; //to read subpayload length
						cbActual = WMCDecCBGetData(hDecoder, pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

						if(cbActual != cbWanted)
						{
							*pcbBuffer = 0;
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset += cbWanted;
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize = pData[0];
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwFrameSize = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize;
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize;
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubCount =0;
						
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize;
						if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft > 0)
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft;

						if( pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wTotalDataBytes > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wBytesRead = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize+1;
						else if ( pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wTotalDataBytes == pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize)
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wBytesRead = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize;

						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bSubPayloadState = 2;
						break;
					case 2: // Subpayload started
						// if it is coming from normal payload to compressed payload
						if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bGotCompOutput == TRUE_WMC)
						{
							*pcbBuffer =0;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bOutputisReady = FALSE_WMC;
							return WMCDec_Succeeded;
						
						}
						if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft == 0 && pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft == 0)
						{
							*pcbBuffer = 0;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bSubPayloadState =3;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bGotCompOutput = TRUE_WMC;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bOutputisReady = FALSE_WMC;
						//	pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwNextBinaryTimeStamp = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryTimeStamp + (U32_WMC)pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.dwDeltaPresTime;

						//	return WMCDec_Succeeded;
							break;
						}
						else
						{
							if(cbData > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft)
								cbData = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft;

							cbActual = WMCDecCBGetData(hDecoder, pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);
							if (cbActual != cbData )
							{
								*pcbBuffer = 0;
								return WMCDec_BufferTooSmall;

							}

							memcpy(pBuffer, pData, cbData);
							*pcbBuffer = cbData;

							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset +=cbActual;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft     -= cbActual;
        
							return WMCDec_Succeeded;

						}

						break;
					case 3: // Subpayload finished
						
						if ( pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wTotalDataBytes > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wBytesRead)
						{ // there are payloads to decode
							cbWanted = 1; //to read subpayload length
							cbActual = WMCDecCBGetData(hDecoder, pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								*pcbBuffer = 0;
								return WMCDec_BufferTooSmall;

							}

							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset += cbWanted;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize = pData[0];
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft     = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize ;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwFrameSize = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft ;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryTimeStamp += (U32_WMC)pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.dwDeltaPresTime;

            
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize;
							if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft > 0)
								pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wSubpayloadLeft -= (U16_WMC)pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft;
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.wBytesRead+= (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bNextSubPayloadSize+1);
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bSubPayloadState =2;                
						}
						else
							pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bSubPayloadState =4; // all subpayloads finished
						break;

					case 4: // All Subpayloads finished

						parse_state = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state;
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bSubPayloadState =0;
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadLeft =0;
						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bIsCompressedPayload =0;

						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state = csWMCDecodePayloadEnd;
						
						err = WMCDecUpdateNewPayload(hDecoder, Binary_WMC, BinaryStreamIndex);
						
						if (err == WMCDec_DecodeComplete)
						{
							*pcbBuffer = 0;
							return WMCDec_Succeeded;
						}
						
						if (err != WMCDec_Succeeded)
						{
							*pcbBuffer = 0;
							return WMCDec_BufferTooSmall;
						}

						pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state = parse_state;  //restore 
						break;
					default:
						break;

					}
				} while(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bIsCompressedPayload /*1*/);

				break;
			}          
		default :
			{
				if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bGotCompOutput == TRUE_WMC)
				{
					*pcbBuffer =0;
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bOutputisReady = FALSE_WMC;
					return WMCDec_Succeeded;
				
				}


				if((pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwFrameLeft == 0) && (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bFrameGiven == TRUE_WMC));
				{
					*pcbBuffer = 0;
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bOutputisReady = FALSE_WMC;
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bFrameGiven = FALSE_WMC;
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bGotCompOutput = TRUE_WMC;
					return WMCDec_Succeeded;
				}
				
				
				
				if(pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft == 0)
				{

					parse_state = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state;

					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload(hDecoder, Binary_WMC, BinaryStreamIndex);
					if (err == WMCDec_DecodeComplete)
					{
						*pcbBuffer = 0;
						return WMCDec_Succeeded;
					}

					if (err != WMCDec_Succeeded)
					{
						*pcbBuffer = 0;
						return WMCDec_BufferTooSmall;
					}

					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state = parse_state; /* restore */
					
					if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->payload.bIsCompressedPayload ==1)
						break;
				}

				/* return as much as we currently have left */

				if(cbData > pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft)
					cbData = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft;

				cbActual = WMCDecCBGetData(hDecoder, pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset, cbData, &pData, pDecoder->u32UserData);

				if (cbActual != cbData )
				{
					*pcbBuffer = 0;
					return WMCDec_BufferTooSmall;

				}

				memcpy(pBuffer, pData, cbData);
				*pcbBuffer = cbData;

				pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwPayloadOffset +=cbActual;
				pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBlockLeft     -= cbActual;

				pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwFrameLeft -=cbActual;
				
				if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwFrameLeft ==0)
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bFrameGiven = TRUE_WMC;

/*				if (pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwFrameLeft ==0)
				{
					U32_WMC    dwBinaryTimeStamp =0;
					dwBinaryTimeStamp = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryTimeStamp;

					parse_state = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state;

					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state = csWMCDecodePayloadEnd;
					err = WMCDecUpdateNewPayload((HWMCDECODER)hDecoder, Binary_WMC, BinaryStreamIndex);
					if (err != WMCDec_Succeeded)
						return err;
					
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwNextBinaryTimeStamp = pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryTimeStamp;
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->dwBinaryTimeStamp = dwBinaryTimeStamp;

					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->parse_state = parse_state; 
					
					pDecoder->tBinaryStreamInfo[BinaryStreamIndex]->bFrameGiven = TRUE_WMC;
				}*/

				
				return WMCDec_Succeeded;
			}
		}


	}while(1);




#endif

	return WMCDec_Succeeded;
}

/******************************************************************************/
 tWMCDecStatus WMCDecDecodeSeqPacket( HWMCDECODER hDecoder)
{

	tWMCDecStatus err = WMCDec_Succeeded;
    tMediaType_WMC MediaType;
    U16_WMC i=0;
    WMFDecoderEx *pDecoder =NULL;
     
    if (NULL == hDecoder)
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *) (hDecoder);
    
    
    do
    {
        switch(pDecoder->parse_state)
        {
            case csWMCNewAsfPacket:

    //#ifdef WMAAPI_DEMO
    //            if (pInt->nSampleCount >= WMAAPI_DEMO_LIMIT*pInt->hdr_parse.nSamplesPerSec)
    //                return cWMA_DemoExpired;
    //#endif /* WMAAPI_DEMO */

                if(pDecoder->tHeaderInfo.cbNextPacketOffset > pDecoder->tHeaderInfo.cbLastPacketOffset)
                {
                    return WMCDec_DecodeComplete;
                }

                pDecoder->tHeaderInfo.cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
                pDecoder->tHeaderInfo.cbNextPacketOffset += pDecoder->tHeaderInfo.cbPacketSize;

                err = WMCDecParsePacketHeader (hDecoder);

/*			    if (err == WMCDec_BufferTooSmall)
			    {
				    pDecoder->tHeaderInfo.cbNextPacketOffset = pDecoder->tHeaderInfo.cbCurrentPacketOffset;
			    //	pInt->hdr_parse.cbCurrentPacketOffset -= pInt->hdr_parse.cbPacketSize;
				    return cWMCNoMoreDataThisTime;
			    }
*/
                if(err != WMCDec_Succeeded)
                {
                    return WMCDec_BadPacketHeader;
                }

                if (pDecoder->tHeaderInfo.ppex.fEccPresent && pDecoder->tHeaderInfo.ppex.fParityPacket)
                {
                    break;
                }

                pDecoder->parse_state = csWMCDecodePayloadStart;
                pDecoder->tHeaderInfo.iPayload = 0;
                break;

            case csWMCDecodePayloadStart:

                if (pDecoder->tHeaderInfo.iPayload >= pDecoder->tHeaderInfo.ppex.cPayloads)
                {
                    pDecoder->parse_state = csWMCNewAsfPacket;
                    return WMCDec_Succeeded; // Return after taking full packet
                }

                err = WMCDecParsePayloadHeader(hDecoder);
                if(err != WMCDec_Succeeded)
                {
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                }

                pDecoder->tHeaderInfo.wPayStart = pDecoder->tHeaderInfo.payload.cbPacketOffset + pDecoder->tHeaderInfo.payload.cbTotalSize  - pDecoder->tHeaderInfo.payload.cbPayloadSize;

                if( pDecoder->tHeaderInfo.payload.cbRepData != 1 )
                {
                    pDecoder->parse_state = csWMCDecodePayload;
                    pDecoder->tHeaderInfo.payload.bIsCompressedPayload =0; // Amit to take care of compressed payloads.
                    break;
                }
                else if( pDecoder->tHeaderInfo.payload.cbRepData == 1)    // Amit to take care of compressed payloads.
                {
                    pDecoder->parse_state = csWMCDecodeCompressedPayload;
                    pDecoder->tHeaderInfo.payload.bIsCompressedPayload =1;
                    break;
                }
 
            case csWMCDecodePayload:
    // In this case store payload in buffer and dont return. Return only after one packet is finished

                err = WMCDecGetMediaType (hDecoder,  pDecoder->tHeaderInfo.payload.bStreamId, &MediaType);
                if (err != WMCDec_Succeeded)
                {
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                }
            
                switch(MediaType)
                {
                case Audio_WMC:
                    // Now get index
                    for (i=0; i< pDecoder->tHeaderInfo.wNoOfAudioStreams; i++)
                    {
                        if (pDecoder->tAudioStreamInfo[i]->wStreamId == pDecoder->tHeaderInfo.payload.bStreamId )
                        {
                            // check whether user wants to output that
//                            if(pDecoder->tAudioStreamInfo[i]->bWantOutput == TRUE_WMC)
                            if((pDecoder->tAudioStreamInfo[i]->bWantOutput == TRUE_WMC) && (pDecoder->tAudioStreamInfo[i]->bStopReading == FALSE_WMC))
                            {
                                if(pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp == 0)
                                    pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp = (U32_WMC) pDecoder->tHeaderInfo.payload.msObjectPres;

                                pDecoder->tHeaderInfo.dwPayloadOffset = (U32_WMC)pDecoder->tHeaderInfo.cbCurrentPacketOffset + pDecoder->tHeaderInfo.wPayStart;

                                err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Audio_WMC,(U32_WMC)pDecoder->tHeaderInfo.payload.msObjectPres, 0, i, TRUE_WMC, FALSE_WMC);
                                if (err != WMCDec_Succeeded)
                                {
                                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                case Video_WMC:
                    // In these cases take care of whole frame as frame may be divided in several packets/payloads
                    for (i=0; i< pDecoder->tHeaderInfo.wNoOfVideoStreams; i++)
                    {
                        if (pDecoder->tVideoStreamInfo[i]->wStreamId == pDecoder->tHeaderInfo.payload.bStreamId )
                        {
                            // check whether user wants to output that
//                            if(pDecoder->tVideoStreamInfo[i]->bWantOutput == TRUE_WMC)
                            if((pDecoder->tVideoStreamInfo[i]->bWantOutput == TRUE_WMC) &&(pDecoder->tVideoStreamInfo[i]->bNowStopReadingAndDecoding == FALSE_WMC))
                            {
                                pDecoder->tHeaderInfo.dwPayloadOffset = (U32_WMC) (pDecoder->tHeaderInfo.cbCurrentPacketOffset + pDecoder->tHeaderInfo.wPayStart);
                                
                                if(pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp == 0)
                                    pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp = (U32_WMC) pDecoder->tHeaderInfo.payload.msObjectPres;

                                // Keep data about frame size, offset, frame used etc... 
                                if (0 == pDecoder->tHeaderInfo.payload.cbObjectOffset) 
                                { // New Frame starts
					                if (!(pDecoder->tVideoStreamInfo[i]->bFirst)) 
                                    {
						                if (pDecoder->tHeaderInfo.payload.bIsKeyFrame == 0) 
                                        {
							                pDecoder->parse_state = csWMCDecodePayloadEnd;
                                            break;
						                }
						                pDecoder->tVideoStreamInfo[i]->bFirst = 1;
					                
					                }

                                /*    if (pDecoder->tVideoStreamInfo[i]->dwFrameSize != pDecoder->tVideoStreamInfo[i]->cbUsed)
                                    { // Error Previous frame is not completed..... So remove it from buffer...
                                         pDecoder->tVideoStreamInfo[i]->dwVideoBufCurOffset -=(pDecoder->tVideoStreamInfo[i]->cbUsed + 3*sizeof(U32_WMC) + sizeof(U8_WMC)); 
                                    }
					              */  
                                    pDecoder->tVideoStreamInfo[i]->cbUsed = pDecoder->tHeaderInfo.payload.cbPayloadSize;
					                pDecoder->tVideoStreamInfo[i]->cbFrame = pDecoder->tVideoStreamInfo[i]->dwNbFrames;
                                    pDecoder->tVideoStreamInfo[i]->dwNbFrames += 1;
                                    pDecoder->tVideoStreamInfo[i]->dwFrameSize = pDecoder->tHeaderInfo.payload.cbObjectSize; 
                                    pDecoder->tVideoStreamInfo[i]->bIsKeyFrame = pDecoder->tHeaderInfo.payload.bIsKeyFrame;
                                    //store payload
                                    err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Video_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, pDecoder->tVideoStreamInfo[i]->cbFrame, i, TRUE_WMC, FALSE_WMC);
                                    if (err != WMCDec_Succeeded) // No problem. No pointers to be adjusted
                                    {
                                        pDecoder->parse_state = csWMCDecodePayloadEnd;
                                        break;
                                    }
                            
				                } 
                                else 
                                {   // old frame cont....
					                if ( pDecoder->tVideoStreamInfo[i]->cbUsed ==0)
                                    {
							            pDecoder->parse_state = csWMCDecodePayloadEnd;
                                        break;
                                    }

                                    if (pDecoder->tVideoStreamInfo[i]->cbUsed == pDecoder->tHeaderInfo.payload.cbObjectOffset) 
                                    { // Packet cont...				
						                // store payload
                                        err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Video_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, pDecoder->tVideoStreamInfo[i]->cbFrame, i, FALSE_WMC, FALSE_WMC);
                                        if (err != WMCDec_Succeeded) // Problem...... Pointers to be adjusted as part of the frame is in buffer. Do it tomarrow....
                                        {
                                            pDecoder->parse_state = csWMCDecodePayloadEnd;
                                            pDecoder->tVideoStreamInfo[i]->dwVideoBufCurOffset -=(pDecoder->tVideoStreamInfo[i]->cbUsed + 3*sizeof(U32_WMC) + sizeof(U8_WMC));
                                            pDecoder->tVideoStreamInfo[i]->dwFrameSize = 0;
                                            pDecoder->tVideoStreamInfo[i]->cbUsed = 0;
                                            break;
                                        }
						                pDecoder->tVideoStreamInfo[i]->cbUsed += pDecoder->tHeaderInfo.payload.cbPayloadSize;
					                }
					                else // Broken packet
						                pDecoder->tVideoStreamInfo[i]->cbUsed = 0;
                                }
                            
                                if (err != WMCDec_Succeeded)
                                {
                                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                case Binary_WMC:
                    // In these cases take care of whole frame as frame may be divided in several packets/payloads
                    for (i=0; i< pDecoder->tHeaderInfo.wNoOfBinaryStreams; i++)
                    {
                        if (pDecoder->tBinaryStreamInfo[i]->wStreamId == pDecoder->tHeaderInfo.payload.bStreamId )
                        {
                            // check whether user wants to output that
                          //  if(pDecoder->tBinaryStreamInfo[i]->bWantOutput == TRUE_WMC)
                            if((pDecoder->tBinaryStreamInfo[i]->bWantOutput == TRUE_WMC)&&(pDecoder->tBinaryStreamInfo[i]->bNowStopReadingAndDecoding == FALSE_WMC))
                            {
                                pDecoder->tHeaderInfo.dwPayloadOffset = (U32_WMC)pDecoder->tHeaderInfo.cbCurrentPacketOffset + pDecoder->tHeaderInfo.wPayStart;
                                if(pDecoder->tBinaryStreamInfo[i]->dwBinaryTimeStamp == 0)
                                    pDecoder->tBinaryStreamInfo[i]->dwBinaryTimeStamp = (U32_WMC) pDecoder->tHeaderInfo.payload.msObjectPres;

                                // Keep data about frame size, offset, frame used etc... 
                                if (0 == pDecoder->tHeaderInfo.payload.cbObjectOffset) 
                                { // New Frame starts

                                    if (pDecoder->tBinaryStreamInfo[i]->dwFrameSize != pDecoder->tBinaryStreamInfo[i]->cbUsed)
                                    { // Error Previous frame is not completed..... So remove it from buffer...
                                         pDecoder->tBinaryStreamInfo[i]->dwBinaryBufCurOffset -=(pDecoder->tBinaryStreamInfo[i]->cbUsed + 2*sizeof(U32_WMC)); 
                                    }
					                
                                    pDecoder->tBinaryStreamInfo[i]->cbUsed = pDecoder->tHeaderInfo.payload.cbPayloadSize;
					                pDecoder->tBinaryStreamInfo[i]->cbFrame = pDecoder->tBinaryStreamInfo[i]->dwNbFrames;
                                    pDecoder->tBinaryStreamInfo[i]->dwNbFrames += 1;
                                    pDecoder->tBinaryStreamInfo[i]->dwFrameSize = pDecoder->tHeaderInfo.payload.cbObjectSize; 
                                    //store payload
                                    err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Binary_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, pDecoder->tBinaryStreamInfo[i]->cbFrame, i, TRUE_WMC, FALSE_WMC);
                                    if (err != WMCDec_Succeeded) // No problem. No pointers to be adjusted
                                    {
                                        pDecoder->parse_state = csWMCDecodePayloadEnd;
                                        break;
                                    }
                            
				                } 
                                else 
                                {   // old frame cont....
					                if ( pDecoder->tBinaryStreamInfo[i]->cbUsed ==0)
                                    {
							            pDecoder->parse_state = csWMCDecodePayloadEnd;
                                        break;
                                    }

                                    if (pDecoder->tBinaryStreamInfo[i]->cbUsed == pDecoder->tHeaderInfo.payload.cbObjectOffset) 
                                    { // Packet cont...				
						                // store payload
                                        err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Binary_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, pDecoder->tBinaryStreamInfo[i]->cbFrame, i, FALSE_WMC, FALSE_WMC);
                                        if (err != WMCDec_Succeeded) // Problem...... Pointers to be adjusted as part of the frame is in buffer. Do it tomarrow....
                                        {
                                            pDecoder->parse_state = csWMCDecodePayloadEnd;
                                            pDecoder->tBinaryStreamInfo[i]->dwBinaryBufCurOffset -=(pDecoder->tBinaryStreamInfo[i]->cbUsed + 2*sizeof(U32_WMC));
                                            pDecoder->tBinaryStreamInfo[i]->dwFrameSize = 0;
                                            pDecoder->tBinaryStreamInfo[i]->cbUsed = 0;
                                            break;
                                        }
						                pDecoder->tBinaryStreamInfo[i]->cbUsed += pDecoder->tHeaderInfo.payload.cbPayloadSize;
					                }
					                else // Broken packet
						                pDecoder->tBinaryStreamInfo[i]->cbUsed = 0;
                                }
                            
                                if (err != WMCDec_Succeeded)
                                {
                                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
            
                }
     		    pDecoder->parse_state = csWMCDecodePayloadEnd;
                break;
     //           return cWMA_NoErr;

            case csWMCDecodePayloadEnd:
                pDecoder->tHeaderInfo.iPayload++;
                pDecoder->parse_state = csWMCDecodePayloadStart;
                break;

            case csWMCDecodeCompressedPayload: // Added by Amit to take care of compressed payloads
                // In this case store payload in buffer and dont return. Return only after one packet is finished
                err = WMCDecGetMediaType (hDecoder,  pDecoder->tHeaderInfo.payload.bStreamId, &MediaType);
                if (err != WMCDec_Succeeded)
                {
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                }
            
                switch(MediaType)
                {
                case Audio_WMC:
                    // Now get index
                    for (i=0; i< pDecoder->tHeaderInfo.wNoOfAudioStreams; i++)
                    {
                        if (pDecoder->tAudioStreamInfo[i]->wStreamId == pDecoder->tHeaderInfo.payload.bStreamId )
                        {
                            // check whether user wants to output that
                          //  if(pDecoder->tAudioStreamInfo[i]->bWantOutput == TRUE_WMC)
                            if((pDecoder->tAudioStreamInfo[i]->bWantOutput == TRUE_WMC) && (pDecoder->tAudioStreamInfo[i]->bStopReading == FALSE_WMC))
                            {
                                if(pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp == 0)
                                    pDecoder->tAudioStreamInfo[i]->dwAudioTimeStamp = (U32_WMC) pDecoder->tHeaderInfo.payload.msObjectPres;

                                pDecoder->tHeaderInfo.dwPayloadOffset = (U32_WMC)pDecoder->tHeaderInfo.cbCurrentPacketOffset + pDecoder->tHeaderInfo.wPayStart;

                                err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Audio_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, 0, i, TRUE_WMC, TRUE_WMC);
                                if (err != WMCDec_Succeeded)
                                {
                                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                case Video_WMC:
                    for (i=0; i< pDecoder->tHeaderInfo.wNoOfVideoStreams; i++)
                    {
                        if (pDecoder->tVideoStreamInfo[i]->wStreamId == pDecoder->tHeaderInfo.payload.bStreamId )
                        {
                            // check whether user wants to output that
                         //   if(pDecoder->tVideoStreamInfo[i]->bWantOutput == TRUE_WMC)
                            if((pDecoder->tVideoStreamInfo[i]->bWantOutput == TRUE_WMC) &&(pDecoder->tVideoStreamInfo[i]->bNowStopReadingAndDecoding == FALSE_WMC))
                            {
					            if (!(pDecoder->tVideoStreamInfo[i]->bFirst)) 
                                {
						            if (pDecoder->tHeaderInfo.payload.bIsKeyFrame == 0) 
                                    {
							            pDecoder->parse_state = csWMCDecodePayloadEnd;
                                        break;
						            }
						            pDecoder->tVideoStreamInfo[i]->bFirst = 1;
					            
					            }
                                
                                if(pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp == 0)
                                    pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp = (U32_WMC) pDecoder->tHeaderInfo.payload.msObjectPres;
                                pDecoder->tHeaderInfo.dwPayloadOffset = (U32_WMC)pDecoder->tHeaderInfo.cbCurrentPacketOffset + pDecoder->tHeaderInfo.wPayStart;
                                pDecoder->tVideoStreamInfo[i]->cbUsed = 0;
					            pDecoder->tVideoStreamInfo[i]->cbFrame = pDecoder->tVideoStreamInfo[i]->dwNbFrames;
                                pDecoder->tVideoStreamInfo[i]->dwFrameSize = 0; 
								pDecoder->tVideoStreamInfo[i]->bIsKeyFrame = pDecoder->tHeaderInfo.payload.bIsKeyFrame;

                                err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Video_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, pDecoder->tVideoStreamInfo[i]->cbFrame, i, FALSE_WMC, TRUE_WMC);
                                if (err != WMCDec_Succeeded) // Problem...... Pointers to be adjusted as part of the frame is in buffer. Do it tomarrow....
                                {
                                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                case Binary_WMC:
                    // In these cases take care of whole frame as frame may be divided in several packets/payloads
                    for (i=0; i< pDecoder->tHeaderInfo.wNoOfBinaryStreams; i++)
                    {
                        if (pDecoder->tBinaryStreamInfo[i]->wStreamId == pDecoder->tHeaderInfo.payload.bStreamId )
                        {
                            // check whether user wants to output that
                        //    if(pDecoder->tBinaryStreamInfo[i]->bWantOutput == TRUE_WMC)
                            if((pDecoder->tBinaryStreamInfo[i]->bWantOutput == TRUE_WMC)&&(pDecoder->tBinaryStreamInfo[i]->bNowStopReadingAndDecoding == FALSE_WMC))
                            {
                                if(pDecoder->tBinaryStreamInfo[i]->dwBinaryTimeStamp == 0)
                                    pDecoder->tBinaryStreamInfo[i]->dwBinaryTimeStamp = (U32_WMC) pDecoder->tHeaderInfo.payload.msObjectPres;
                                pDecoder->tHeaderInfo.dwPayloadOffset = (U32_WMC)pDecoder->tHeaderInfo.cbCurrentPacketOffset + pDecoder->tHeaderInfo.wPayStart;
                                pDecoder->tBinaryStreamInfo[i]->cbUsed = 0;
					            pDecoder->tBinaryStreamInfo[i]->cbFrame = pDecoder->tBinaryStreamInfo[i]->dwNbFrames;
                                pDecoder->tBinaryStreamInfo[i]->dwFrameSize = 0; 
                            
                                err = WMCDecStorePayload( hDecoder, pDecoder->tHeaderInfo.payload.cbPayloadSize, Video_WMC,pDecoder->tHeaderInfo.payload.msObjectPres, pDecoder->tVideoStreamInfo[i]->cbFrame, i, FALSE_WMC, TRUE_WMC);
                                if (err != WMCDec_Succeeded) 
                                {
                                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                
                    pDecoder->parse_state = csWMCDecodePayloadEnd;
                    break;
                }
     		    pDecoder->parse_state = csWMCDecodePayloadEnd;
                break;

//            default:
        }
        
    }while(1);

    return err;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tWMCDecStatus WMCDecStorePayload( HWMCDECODER hDecoder, U32_WMC cbPayloadSize, tMediaType_WMC MediaType, U32_WMC PresTime, I32_WMC cbFrame, U16_WMC Index, Bool_WMC bWriteHeader, Bool_WMC bIsCompressed)
{
    tWMCDecStatus rc = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder;
    U32_WMC dwPayloadLeft = 0;
    U32_WMC dwOffset = 0;
    U32_WMC dwLocalOffset=0;
    U32_WMC cbWanted=0;
    U32_WMC cbActual=0;
    U8_WMC *pData= NULL;
    U16_WMC wNextSubPayloadSize =0;
    U32_WMC dwDelta=0;
    U16_WMC wBytesRead =0;
    I32_WMC dwPayloadSize=0;
	U32_WMC dwFrameSize;



    if (NULL == hDecoder)
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *)(hDecoder);


	if(Audio_WMC==MediaType) // Audio
	{
        if (bIsCompressed !=TRUE_WMC)
        {
//            assert((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + 2*sizeof(U32_WMC) + cbPayloadSize) <= pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize);
			// If buffer is less, Increase it Once only

		    
            if (((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + 2*sizeof(U32_WMC) + cbPayloadSize) > pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize) && (FALSE_WMC == pDecoder->tAudioStreamInfo[Index]->bBufferIncreased) )
			{
				pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer = realloc(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer, pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize *2 );
				if (pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer == NULL)
					return WMCDec_BadMemory;
				pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize *=2;
				pDecoder->tAudioStreamInfo[Index]->bBufferIncreased = TRUE_WMC;
			
			}
			
            assert((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + 2*sizeof(U32_WMC) + cbPayloadSize) <= pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize);
			
			if((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + cbPayloadSize +2*sizeof(U32_WMC)) <= pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize)
		    {
                dwOffset = pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset;

                memcpy(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + dwOffset, &PresTime , sizeof(U32_WMC));
			    memcpy(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + dwOffset + sizeof(U32_WMC),  &cbPayloadSize , sizeof(U32_WMC));
                dwOffset+=2*sizeof(U32_WMC);

                dwPayloadLeft = cbPayloadSize;
                do
                {
                    cbWanted = MIN_WANTED;
                    if (cbWanted > dwPayloadLeft)
                        cbWanted = dwPayloadLeft;

                    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                    if(cbActual != cbWanted)
                    {
                        return WMCDec_BufferTooSmall;
                    }
                    memcpy(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + dwOffset, pData , cbActual);
                    dwOffset+=cbActual;
                    dwPayloadLeft-=cbActual;
                    dwLocalOffset+=cbActual;
                }
                while(dwPayloadLeft>0);

			    pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset = dwOffset;
			    pDecoder->tAudioStreamInfo[Index]->cbNbFramesAudBuf++;
		    }
            else
                return WMCDec_BufferTooSmall;
        }
        else
        {
            cbWanted = 1;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }

            wNextSubPayloadSize = pData[0]; 
            dwLocalOffset+=1;

			if (pDecoder->tHeaderInfo.payload.wTotalDataBytes < wNextSubPayloadSize)
				return WMCDec_BadPacketHeader;

            if( pDecoder->tHeaderInfo.payload.wTotalDataBytes > wNextSubPayloadSize)
				wBytesRead = wNextSubPayloadSize+1;
			else if (pDecoder->tHeaderInfo.payload.wTotalDataBytes  == wNextSubPayloadSize)
				wBytesRead = wNextSubPayloadSize;
          
            while (pDecoder->tHeaderInfo.payload.wTotalDataBytes >= wBytesRead) 
			{

				// If buffer is less, Increase it Once only

				
				if (((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + 2*sizeof(U32_WMC) + cbPayloadSize) > pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize)&&(pDecoder->tAudioStreamInfo[Index]->bBufferIncreased == FALSE_WMC) )
				{
					pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer = realloc(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer, pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize *2 );
					if (pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer == NULL)
						return WMCDec_BadMemory;
					pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize *=2;
					pDecoder->tAudioStreamInfo[Index]->bBufferIncreased = TRUE_WMC;
				
				}
                
				
				assert((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + 2*sizeof(U32_WMC) + wNextSubPayloadSize) <= pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize);

		        if((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset + wNextSubPayloadSize +2*sizeof(U32_WMC)) <= pDecoder->tAudioStreamInfo[Index]->dwAudioBufSize)
		        {
                    dwOffset = pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset;
                    PresTime+=dwDelta;
                    dwPayloadSize = (I32_WMC)wNextSubPayloadSize;

                    memcpy(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + dwOffset, &PresTime , sizeof(U32_WMC));
			        memcpy(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + dwOffset + sizeof(U32_WMC),  &dwPayloadSize , sizeof(U32_WMC));
                    dwOffset+=2*sizeof(U32_WMC);

                    dwPayloadLeft = wNextSubPayloadSize;
                    do
                    {
                        cbWanted = MIN_WANTED;
                        if (cbWanted > dwPayloadLeft)
                            cbWanted = dwPayloadLeft;

                        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                        if(cbActual != cbWanted)
                        {
                            return WMCDec_BufferTooSmall;
                        }
                        memcpy(pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + dwOffset, pData , cbActual);
                        dwOffset+=cbActual;
                        dwPayloadLeft-=cbActual;
                        dwLocalOffset+=cbActual;
                    }
                    while(dwPayloadLeft>0);

			        pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset = dwOffset;
			        pDecoder->tAudioStreamInfo[Index]->cbNbFramesAudBuf++;
                    
                    if (pDecoder->tHeaderInfo.payload.wTotalDataBytes > wBytesRead)
                    {
                        cbWanted = 1;
                        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                        if(cbActual != cbWanted)
                        {
                            return WMCDec_BufferTooSmall;
                        }

                        wNextSubPayloadSize = pData[0]; 
					    dwDelta = pDecoder->tHeaderInfo.payload.dwDeltaPresTime;
                        dwLocalOffset+=1;
					    wBytesRead+=wNextSubPayloadSize+1;
                    }
                    else
                        break;
		        }
                else
                    return WMCDec_BufferTooSmall;

			}// end of while
        }
	}
	else if(Video_WMC==MediaType) // Video
	{
        if (bIsCompressed !=TRUE_WMC)
        {
		// If buffer is less increase it only one time;	
       //     assert((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)+ sizeof(I32_WMC) + sizeof(U8_WMC))<= pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize);

			if (((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)+ sizeof(I32_WMC) + sizeof(U8_WMC)) > pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize)&&(pDecoder->tVideoStreamInfo[Index]->bBufferIncreased == FALSE_WMC) )
			{
				pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer = realloc(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer, pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize *2 );
				if (pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer == NULL)
					return WMCDec_BadMemory;
				pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize *=2;
				pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer = NULL_WMC;
				pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer = NULL_WMC;
				pDecoder->tVideoStreamInfo[Index]->bBufferIncreased = TRUE_WMC;
			
			}
                

            assert((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)+ sizeof(I32_WMC) + sizeof(U8_WMC))<= pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize);

		    if((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC) + sizeof(I32_WMC)  + sizeof(U8_WMC)) <= pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize)
		    {
			    dwOffset = pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset;

                if (bWriteHeader == TRUE_WMC)
                {
                    if (pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer != NULL)
					{
						pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset;
						
						memcpy (&dwFrameSize, pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer + 2*sizeof(U32_WMC), sizeof(U32_WMC));
						
						// Last Frame Error
						if (pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer - (pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer + 3*sizeof(U32_WMC) + sizeof(U8_WMC)) != (I32_WMC)dwFrameSize)
						{
							// Last Frame is not complete
							pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset = pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset - (pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer - pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer);
							dwOffset = pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset;
							pDecoder->tVideoStreamInfo[Index]->cbNbFramesVidBuf--;
						}						
						
						pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset;
					}
					else // First Frame
					{
						pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset;
					}

					memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset, &PresTime , sizeof(U32_WMC));
			        memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset + sizeof(U32_WMC) , &cbFrame , sizeof(I32_WMC));
			        memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset + sizeof(U32_WMC) + sizeof(I32_WMC), &(pDecoder->tVideoStreamInfo[Index]->dwFrameSize) , sizeof(I32_WMC));
//g_iVideoFrameSize = pDecoder->tVideoStreamInfo[Index]->dwFrameSize;
                    dwOffset+=(2*sizeof(U32_WMC) + sizeof(I32_WMC));
    			    pDecoder->tVideoStreamInfo[Index]->cbNbFramesVidBuf++;
                    pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer[dwOffset] = (U8_WMC)(pDecoder->tVideoStreamInfo[Index]->bIsKeyFrame);
                    dwOffset += sizeof(U8_WMC);

                    
                }

                dwPayloadLeft = cbPayloadSize;
                do
                {
                    cbWanted = MIN_WANTED;
                    if (cbWanted > dwPayloadLeft)
                        cbWanted = dwPayloadLeft;

                    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                    if(cbActual != cbWanted)
                    {
                        return WMCDec_BufferTooSmall;
                        pDecoder->tVideoStreamInfo[Index]->cbNbFramesVidBuf--;
                    }
                    memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset, pData , cbActual);
                    dwOffset+=cbActual;
                    dwPayloadLeft-=cbActual;
                    dwLocalOffset+=cbActual;
                }
                while(dwPayloadLeft>0);

                pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset = dwOffset;
		    }
            else
                return WMCDec_BufferTooSmall;
        }
        else
        {
            cbWanted = 1;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }

            wNextSubPayloadSize = pData[0]; 
            dwLocalOffset+=1;

			if (pDecoder->tHeaderInfo.payload.wTotalDataBytes < wNextSubPayloadSize)
				return WMCDec_BadPacketHeader;

            if( pDecoder->tHeaderInfo.payload.wTotalDataBytes > wNextSubPayloadSize)
				wBytesRead = wNextSubPayloadSize+1;
			else if (pDecoder->tHeaderInfo.payload.wTotalDataBytes == wNextSubPayloadSize)
				wBytesRead = wNextSubPayloadSize;
          
            while (pDecoder->tHeaderInfo.payload.wTotalDataBytes >= wBytesRead) 
			{
				if (((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)+ sizeof(I32_WMC) + sizeof(U8_WMC)) > pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize)&&(pDecoder->tVideoStreamInfo[Index]->bBufferIncreased == FALSE_WMC) )
				{
					pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer = realloc(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer, pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize *2 );
					if (pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer == NULL)
						return WMCDec_BadMemory;
					pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize *=2;
					pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer = NULL_WMC;
					pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer = NULL_WMC;
					pDecoder->tVideoStreamInfo[Index]->bBufferIncreased = TRUE_WMC;
				
				}

                assert((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + 3*sizeof(U32_WMC) + sizeof (U8_WMC) + wNextSubPayloadSize) <= pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize);

		        if((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset + wNextSubPayloadSize +3*sizeof(U32_WMC) + sizeof(U8_WMC)) <= pDecoder->tVideoStreamInfo[Index]->dwVideoBufSize)
		        {
                    dwOffset = pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset;
                    PresTime+=dwDelta;
                    pDecoder->tVideoStreamInfo[Index]->dwNbFrames = pDecoder->tVideoStreamInfo[Index]->dwNbFrames +1;
                    cbFrame = pDecoder->tVideoStreamInfo[Index]->dwNbFrames;
                    dwPayloadSize = (I32_WMC)wNextSubPayloadSize;
					if (dwPayloadSize > 0)
					{

						if (pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer != NULL)
						{
							pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset;
							
							memcpy (&dwFrameSize, pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer + 2*sizeof(U32_WMC), sizeof(U32_WMC));
							
							// Last Frame Error
							if (pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer - (pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer + 3*sizeof(U32_WMC) + sizeof(U8_WMC)) != (I32_WMC)dwFrameSize)
							{
								// Last Frame is not complete
								pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset = pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset - (pDecoder->tVideoStreamInfo[Index]->pStoreNextFrameStartPointer - pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer);
								dwOffset = pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset;
								pDecoder->tVideoStreamInfo[Index]->cbNbFramesVidBuf--;
							}						
							
							pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset;
						}
						else // First Frame
						{
							pDecoder->tVideoStreamInfo[Index]->pStoreFrameStartPointer = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset;
						}
						
						
						memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset, &PresTime , sizeof(U32_WMC));
						memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset + sizeof(U32_WMC) , &cbFrame , sizeof(I32_WMC));
						memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset + sizeof(U32_WMC) + sizeof(I32_WMC), &dwPayloadSize , sizeof(I32_WMC));
//g_iVideoFrameSize = dwPayloadSize;
						dwOffset+=(2*sizeof(U32_WMC) + sizeof(I32_WMC));
						//pDecoder->tVideoStreamInfo[Index]->bIsKeyFrame = pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer[dwOffset];
						pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer[dwOffset] = (U8_WMC)(pDecoder->tVideoStreamInfo[Index]->bIsKeyFrame);
						dwOffset+=(sizeof(I8_WMC));
	//    			    pDecoder->tVideoStreamInfo[Index]->cbNbFramesVidBuf++;

						dwPayloadLeft = wNextSubPayloadSize;
						do
						{
							cbWanted = MIN_WANTED;
							if (cbWanted > dwPayloadLeft)
								cbWanted = dwPayloadLeft;

							cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

							if(cbActual != cbWanted)
							{
								return WMCDec_BufferTooSmall;
							}
							memcpy(pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + dwOffset, pData , cbActual);
							dwOffset+=cbActual;
							dwPayloadLeft-=cbActual;
							dwLocalOffset+=cbActual;
						}
						while(dwPayloadLeft>0);

						pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset = dwOffset;
						pDecoder->tVideoStreamInfo[Index]->cbNbFramesVidBuf++;
					}
                    
                    if (pDecoder->tHeaderInfo.payload.wTotalDataBytes > wBytesRead)
                    {
                        cbWanted = 1;
                        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                        if(cbActual != cbWanted)
                        {
                            return WMCDec_BufferTooSmall;
                        }

                        wNextSubPayloadSize = pData[0]; 
					    dwDelta = pDecoder->tHeaderInfo.payload.dwDeltaPresTime;
                        dwLocalOffset+=1;
					    wBytesRead+=wNextSubPayloadSize+1;
                    }
                    else
                        break;
		        }
                else
                    return WMCDec_BufferTooSmall;

			}// end of while
        }

	}
	else if(Binary_WMC==MediaType) // Video
	{

        if (bIsCompressed !=TRUE_WMC)
        {

			if (((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)) > pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize)&&(pDecoder->tBinaryStreamInfo[Index]->bBufferIncreased == FALSE_WMC) )
			{
				pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer = realloc(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer, pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize *2 );
				if (pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer == NULL)
					return WMCDec_BadMemory;
				pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize *=2;
				pDecoder->tBinaryStreamInfo[Index]->bBufferIncreased = TRUE_WMC;
			
			}
            assert((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC))<= pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize);

		    if((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)) <= pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize)
		    {
			    dwOffset = pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset;

                if (bWriteHeader == TRUE_WMC)
                {
                    memcpy(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + dwOffset, &PresTime , sizeof(U32_WMC));
			        memcpy(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + dwOffset + sizeof(U32_WMC), &(pDecoder->tBinaryStreamInfo[Index]->dwFrameSize), sizeof(I32_WMC));
                    dwOffset+=(2*sizeof(U32_WMC));
    			    pDecoder->tBinaryStreamInfo[Index]->cbNbFramesBinBuf++;
                }

                dwPayloadLeft = cbPayloadSize;
                do
                {
                    cbWanted = MIN_WANTED;
                    if (cbWanted > dwPayloadLeft)
                        cbWanted = dwPayloadLeft;

                    cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                    if(cbActual != cbWanted)
                    {
                        return WMCDec_BufferTooSmall;
                        pDecoder->tBinaryStreamInfo[Index]->cbNbFramesBinBuf--;
                    }
                    memcpy(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + dwOffset, pData , cbActual);
                    dwOffset+=cbActual;
                    dwPayloadLeft-=cbActual;
                    dwLocalOffset+=cbActual;
                }
                while(dwPayloadLeft>0);

                pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset = dwOffset;
		    }
            else
                return WMCDec_BufferTooSmall;
        }
        else
        {
            cbWanted = 1;
            cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

            if(cbActual != cbWanted)
            {
                return WMCDec_BufferTooSmall;
            }

            wNextSubPayloadSize = pData[0]; 
            dwLocalOffset+=1;

			if (pDecoder->tHeaderInfo.payload.wTotalDataBytes < wNextSubPayloadSize)
				return WMCDec_BadPacketHeader;

            if( pDecoder->tHeaderInfo.payload.wTotalDataBytes > wNextSubPayloadSize)
				wBytesRead = wNextSubPayloadSize+1;
			else if (pDecoder->tHeaderInfo.payload.wTotalDataBytes == wNextSubPayloadSize)
				wBytesRead = wNextSubPayloadSize;
          
            while (pDecoder->tHeaderInfo.payload.wTotalDataBytes >= wBytesRead) 
			{

				if (((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset + cbPayloadSize + 2*sizeof(U32_WMC)) > pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize)&&(pDecoder->tBinaryStreamInfo[Index]->bBufferIncreased == FALSE_WMC) )
				{
					pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer = realloc(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer, pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize *2 );
					if (pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer == NULL)
						return WMCDec_BadMemory;
					pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize *=2;
					pDecoder->tBinaryStreamInfo[Index]->bBufferIncreased = TRUE_WMC;
				
				}
                assert((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset + 2*sizeof(U32_WMC) + wNextSubPayloadSize) <= pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize);

		        if((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset + wNextSubPayloadSize +2*sizeof(U32_WMC)) <= pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufSize)
		        {
                    dwOffset = pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset;
                    PresTime+=dwDelta;

                    dwPayloadSize = (I32_WMC)wNextSubPayloadSize;
                    memcpy(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + dwOffset, &PresTime , sizeof(U32_WMC));
			        memcpy(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + dwOffset + sizeof(U32_WMC),  &dwPayloadSize , sizeof(U32_WMC));
                    dwOffset+=2*sizeof(U32_WMC);

                    dwPayloadLeft = wNextSubPayloadSize;
                    do
                    {
                        cbWanted = MIN_WANTED;
                        if (cbWanted > dwPayloadLeft)
                            cbWanted = dwPayloadLeft;

                        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                        if(cbActual != cbWanted)
                        {
                            return WMCDec_BufferTooSmall;
                        }
                        memcpy(pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + dwOffset, pData , cbActual);
                        dwOffset+=cbActual;
                        dwPayloadLeft-=cbActual;
                        dwLocalOffset+=cbActual;
                    }
                    while(dwPayloadLeft>0);

			        pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset = dwOffset;
			        pDecoder->tBinaryStreamInfo[Index]->cbNbFramesBinBuf++;

                    if (pDecoder->tHeaderInfo.payload.wTotalDataBytes > wBytesRead)
                    {

                        cbWanted = 1;
                        cbActual = WMCDecCBGetData(hDecoder, pDecoder->tHeaderInfo.dwPayloadOffset + dwLocalOffset, cbWanted, &pData, pDecoder->u32UserData);

                        if(cbActual != cbWanted)
                        {
                            return WMCDec_BufferTooSmall;
                        }

                        wNextSubPayloadSize = pData[0]; 
					    dwDelta = pDecoder->tHeaderInfo.payload.dwDeltaPresTime;
                        dwLocalOffset+=1;
					    wBytesRead+=wNextSubPayloadSize+1;
                    }
                    else
                        break;
		        }
                else
                    return WMCDec_BufferTooSmall;

			}// end of while
        
        }
	}
		
    return rc;
}



/******************************************************************************************************************/

tWMCDecStatus WMCDecDecodeFrame( HWMCDECODER hDecoder, U8_WMC bType, U16_WMC * pbDecoded, U32_WMC* pnNumberOfSamples)
{
	tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder;
	U8_WMC* pComData = NULL;
    U16_WMC     i=0;

    U16_WMC     wMinAudioIndex = 0xffff;
    U16_WMC     wMinVideoIndex = 0xffff;
    U16_WMC     wMinBinaryIndex = 0xffff;
    U32_WMC     dwPresTime=0;
    U32_WMC     dwMinPresTime=0xffffffff;
    U32_WMC     dwMinAudioPresTime=0xffffffff;
    U32_WMC     dwMinVideoPresTime=0xffffffff;
    U32_WMC     dwMinBinaryPresTime=0xffffffff;
    tMediaType_WMC MediaType = Audio_WMC;
	Bool_WMC	bGotAtLeastOneVideoOutput = FALSE_WMC;	
	Bool_WMC	bGotAtLeastOneBinaryOutput = FALSE_WMC;
	Bool_WMC	bGotAtLeastOneVideoStreamToOutput = FALSE_WMC;	
	Bool_WMC	bGotAtLeastOneBinaryStreamToOutput = FALSE_WMC;

#ifndef _ASFPARSE_ONLY_
    tWMVDecodeStatus  VidStatus;
#endif
	Bool_WMC	bAllDone = FALSE_WMC;

    WMARESULT wmar = WMA_OK;
#ifndef __NO_SPEECH__
    WMSRESULT wmsr = WMS_OK;
#endif
    U16_WMC Index =0;
    *pnNumberOfSamples =0;
   // U16_WMV uiNumDecodedFrames=0;
    
    if (NULL == hDecoder)
        return WMCDec_InValidArguments;

    pDecoder = (WMFDecoderEx *) (hDecoder);



    for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
    {
        Index = pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex;

        switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
        {
        case Audio_WMC:
			if (pDecoder->tAudioStreamInfo[Index]->bTobeDecoded == FALSE_WMC)
			{
				if (pDecoder->tAudioStreamInfo[Index]->dwBlockLeft !=0)
				{
					// problem. Either no output is taken or there is a video error
					pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded += pDecoder->tAudioStreamInfo[Index]->dwBlockLeft;
					pDecoder->tAudioStreamInfo[Index]->dwBlockLeft =0;
					pDecoder->tAudioStreamInfo[Index]->bOutputisReady = FALSE_WMC;
				}
				
			    pComData =  pDecoder->tAudioStreamInfo[Index]->pbAudioBuffer + pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded;   
			    memcpy(&dwPresTime,pComData,sizeof(U32_WMC));
				
				pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp = dwPresTime;
			}
		/*	else
			{
				if (pDecoder->tAudioStreamInfo[Index]->bOutputisReady == TRUE_WMC)
				{
					I16_WMC TempOutBuff[4096];
					U32_WMC cbBuffer=0;
					I64_WMC  tPresentationTime=0;
					do
					{
						WMCDecGetAudioOutput(hDecoder, (I16_WMC*)&TempOutBuff[0], (I16_WMC*)NULL_WMC,(U32_WMC)1024, &cbBuffer, &tPresentationTime )	;					WMCRawGetData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
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
			
			if (pDecoder->tVideoStreamInfo[Index]->dwBlockLeft !=0)
			{
				// problem. Either no output is taken or there is a video error
				pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded += pDecoder->tVideoStreamInfo[Index]->dwBlockLeft;
				pDecoder->tVideoStreamInfo[Index]->dwBlockLeft =0;
				pDecoder->tVideoStreamInfo[Index]->bOutputisReady = FALSE_WMC;
			}
            break;
        case Binary_WMC:
			if ((pDecoder->tBinaryStreamInfo[Index]->bHasGivenAnyOutput == TRUE_WMC)&&(pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC))
				bGotAtLeastOneBinaryOutput = TRUE_WMC;

			if ((pDecoder->tBinaryStreamInfo[Index]->bWantOutput == TRUE_WMC)&&(pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC))
				bGotAtLeastOneBinaryStreamToOutput = TRUE_WMC;

			if (pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft !=0)
			{
				// problem. Either no output is taken 
				pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded += pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft;
				pDecoder->tBinaryStreamInfo[Index]->dwBlockLeft =0;
				pDecoder->tBinaryStreamInfo[Index]->bOutputisReady = FALSE_WMC;
			}

        }
    }

	Index =0;

	if (bGotAtLeastOneVideoStreamToOutput == FALSE_WMC)
		bGotAtLeastOneVideoOutput = TRUE_WMC;

	if (bGotAtLeastOneBinaryStreamToOutput == FALSE_WMC)
		bGotAtLeastOneBinaryOutput = TRUE_WMC;


    if(3== bType)
    {
        for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
        {
            Index = pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex;

            switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
            {
            case Audio_WMC:
                if ((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset > pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded))
                {
					if (dwMinAudioPresTime > (U32_WMC)pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp)
					{
						dwMinAudioPresTime = (U32_WMC)pDecoder->tAudioStreamInfo[Index]->dwAudioTimeStamp;
						wMinAudioIndex = Index;
					}
				}
				else
				{
					dwMinAudioPresTime = 0xffffffff;
					*pbDecoded = 0xfe;
				
				}
                break;
            case Video_WMC:
                if ((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset > pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded))
                {
					pComData =  pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded;   
					memcpy(&dwPresTime,pComData,sizeof(U32_WMC));
					
					if ((dwMinVideoPresTime > dwPresTime )&&((pDecoder->tVideoStreamInfo[Index]->bStopReading == FALSE_WMC) || (bGotAtLeastOneVideoOutput == FALSE_WMC)))
					{
						dwMinVideoPresTime = dwPresTime;
						wMinVideoIndex = Index;
					}
					else if((dwMinVideoPresTime == dwPresTime ) && (pDecoder->tVideoStreamInfo[Index]->bStopReading == FALSE_WMC))
					{
						dwMinVideoPresTime = dwPresTime;
						wMinVideoIndex = Index;
					}
				}
				else
				{
					dwMinVideoPresTime = 0xffffffff;
					*pbDecoded = 0xfe;
				
				}
				if ((pDecoder->tVideoStreamInfo[Index]->bStopReading == TRUE_WMC) && (bGotAtLeastOneVideoOutput == TRUE_WMC))
				{
					//Dont Read and decode this stream.
					pDecoder->tVideoStreamInfo[Index]->bNowStopReadingAndDecoding = TRUE_WMC;
					
					dwMinVideoPresTime = 0xffffffff;
					*pbDecoded = 0xfe;
					pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset =0;
					pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded =0;
				}

                break;
            case Binary_WMC:
                if ((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset > pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded))
                {
					pComData =  pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded;   
					memcpy(&dwPresTime,pComData,sizeof(U32_WMC));
					if ((dwMinBinaryPresTime > dwPresTime )&&((pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC) || (bGotAtLeastOneBinaryOutput == FALSE_WMC)))
					{
						dwMinBinaryPresTime = dwPresTime;
						wMinBinaryIndex = Index;
					}
					else if((dwMinBinaryPresTime == dwPresTime ) && (pDecoder->tBinaryStreamInfo[Index]->bStopReading == FALSE_WMC))
					{
						dwMinBinaryPresTime = dwPresTime;
						wMinBinaryIndex = Index;
					}

				}
				else
				{
					dwMinBinaryPresTime = 0xffffffff;
					*pbDecoded = 0xfe;
				
				}
				if ((pDecoder->tBinaryStreamInfo[Index]->bStopReading == TRUE_WMC) && (bGotAtLeastOneBinaryOutput == TRUE_WMC))
				{
					//Dont Read and decode this stream.
					pDecoder->tBinaryStreamInfo[Index]->bNowStopReadingAndDecoding = TRUE_WMC;
					
					dwMinBinaryPresTime = 0xffffffff;
					*pbDecoded = 0xfe;
					pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset =0;
					pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded =0;
				}


            }
        }
    

    // Findout Minimum of all 3..... and select which stream to decode

        dwMinPresTime = dwMinAudioPresTime;
        MediaType = Audio_WMC;

        if (dwMinVideoPresTime < dwMinPresTime)
        {
            dwMinPresTime = dwMinVideoPresTime;
            MediaType = Video_WMC;
        }
        if (dwMinBinaryPresTime < dwMinPresTime)
        {
            dwMinPresTime = dwMinBinaryPresTime;
            MediaType = Binary_WMC;
        }
 
        if (dwMinPresTime == 0xffffffff)
        {
         // No stream is ready to get decoded;
        
            *pbDecoded = 0xff;
            return WMCDec_Succeeded;
        }
    }
    else
    {
        for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
        {
            
			pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = FALSE_WMC;
			Index = pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex;
            wMinAudioIndex =0;
            wMinVideoIndex=0;
            wMinBinaryIndex=0;


            switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
            {
            case Audio_WMC:
                if ((pDecoder->tAudioStreamInfo[Index]->dwAudioBufCurOffset > pDecoder->tAudioStreamInfo[Index]->dwAudioBufDecoded))
                {
                    wMinAudioIndex = Index;
                    MediaType = Audio_WMC;
                }
                else
                {
					pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;
				//	*pbDecoded = 0xff;
                 //   return WMCDec_Succeeded;
                }
                break;
            case Video_WMC:
                if ((pDecoder->tVideoStreamInfo[Index]->dwVideoBufCurOffset > pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded))
                {
				    pComData =  pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer + pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded;   
				    memcpy(&dwPresTime,pComData,sizeof(U32_WMC));
					dwMinVideoPresTime = dwPresTime;
                    wMinVideoIndex = Index;
                    MediaType = Video_WMC;
                }
                else
                {
					pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;
//					*pbDecoded = 0xff;
//                    return WMCDec_Succeeded;
                }
                break;
            case Binary_WMC:
                if ((pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufCurOffset > pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded))
                {
					pComData =  pDecoder->tBinaryStreamInfo[Index]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[Index]->dwBinaryBufDecoded;   
					memcpy(&dwPresTime,pComData,sizeof(U32_WMC));
					dwMinBinaryPresTime = dwPresTime;
                    wMinBinaryIndex = Index;
                    MediaType = Binary_WMC;
                }
                else
                {
                    
					pDecoder->tPlannedOutputInfo.tPlannedId[i].bDone = TRUE_WMC;
//					*pbDecoded = 0xff;
//                    return WMCDec_Succeeded;
                }

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
			*pbDecoded = 0xff;
            return WMCDec_Succeeded;
		}


    }

    if ((3== bType) || (4== bType)) // Normal decode
	{
        switch(MediaType)
        {
        case Audio_WMC:
            if ((pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufCurOffset > pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded))
            {
                pDecoder->i32CurrentAudioStreamIndex = wMinAudioIndex;
#ifndef _ASFPARSE_ONLY_			   
                if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->bTobeDecoded == TRUE_WMC)
                {
            
                    if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->nVersion >0 && pDecoder->tAudioStreamInfo[wMinAudioIndex]->nVersion <4)
                    {
                        if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmar != WMA_OK)
				        {
					        if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmar > 0)
					        {
                                if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmar == WMA_S_NO_MORE_FRAME)
                                {
                                 //   _asm emms;
                                   // pDecoder->tAudioStreamInfo[wMinAudioIndex]->bTimeToChange = TRUE_WMC; 
 
									wmar = WMARawDecStatus (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
                                }
					        }
						    else
					        {
						        WMARawDecReset (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
						        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft;
						        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft =0;
						        wmar = WMARawDecStatus (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
					        }
				        }
                        *pnNumberOfSamples = 8000;
                       // _asm emms;
                        wmar = WMARawDecDecodeData (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA, (U32_WMARawDec *)pnNumberOfSamples
    #ifdef _V9WMADEC_
                            , (I32_WMARawDec *)0
    #endif
                            );
                        
						if (wmar == WMA_S_NO_MORE_FRAME)
						{
                           pDecoder->tAudioStreamInfo[wMinAudioIndex]->bTimeToChange = TRUE_WMC; 

						}
						
						if (wmar == WMA_E_BROKEN_FRAME) 
				        {
					        WMARawDecReset (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
					        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft;
					        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft =0;
					        wmar = WMARawDecStatus(pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
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
        
                        *pbDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->wStreamId;
                        pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmar = wmar;
                    }
#ifndef __NO_SPEECH__
                    else if(pDecoder->tAudioStreamInfo[wMinAudioIndex]->nVersion ==4 )//Speech Codec
                    {
 /*                       if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmsr != WMS_OK)
				        {
					        if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmsr > 0)
					        {
                                if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmsr == WMS_S_NO_MORE_FRAME)
                                {
                                    wmsr = WMSRawDecStatus (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
                                }
					        }
						    else
					        {
						        WMSRawDecReset (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
						        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft;
						        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft =0;
						        wmsr = WMSRawDecStatus (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
					        }
				        }
*/                        *pnNumberOfSamples = 0;
                        wmsr = WMSRawDecDecodeData (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA, (U32_WMARawDec *)pnNumberOfSamples);
                        if (wmsr == WMS_E_BROKEN_FRAME) 
				        {
					        WMSRawDecReset (pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
					        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded + pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft;
					        pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwBlockLeft =0;
					        wmsr = WMARawDecStatus(pDecoder->tAudioStreamInfo[wMinAudioIndex]->hMSA);
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
        
                        *pbDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->wStreamId;
//                        pDecoder->tAudioStreamInfo[wMinAudioIndex]->wmsr = wmsr;
                    }
#endif
                }
                else
#endif //_ASFPARSE_ONLY_
                {
                    *pbDecoded = pDecoder->tAudioStreamInfo[wMinAudioIndex]->wStreamId;
					
					if (pDecoder->tAudioStreamInfo[wMinAudioIndex]->bOutputisReady == TRUE_WMC) // Last output is not taken out
					{
						//Fix buffers
						U8_WMC TempOutBuff[1024];
						U32_WMC cbBuffer=0;
						do
						{
							WMCRawGetData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
						}while(cbBuffer>0);
					
					}
					
					
            		pDecoder->tAudioStreamInfo[wMinAudioIndex]->bGotCompOutput = FALSE_WMC;
					pDecoder->tAudioStreamInfo[wMinAudioIndex]->bOutputisReady = TRUE_WMC;
					memcpy (pnNumberOfSamples,(pDecoder->tAudioStreamInfo[wMinAudioIndex]->pbAudioBuffer + pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));

//                    *pnNumberOfSamples =1;  // To be changed. Should be no of bytes
                }
            }
            else
            {	
				*pbDecoded = 0xfe;
                pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudPayloadPresTime = 0xffffffff;
			    pDecoder->tAudioStreamInfo[wMinAudioIndex]->dwAudioTimeStamp = (Double_WMC)0xffffffff;
	//*pbDecoded = 0xff;
            }
            break;
        case Video_WMC:
            if ((pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoBufCurOffset > pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoBufDecoded))
            {
                pDecoder->i32CurrentVideoStreamIndex = wMinVideoIndex;
#ifndef _ASFPARSE_ONLY_
			    if (pDecoder->tVideoStreamInfo[wMinVideoIndex]->bTobeDecoded == TRUE_WMC)
                {
                    if ((pDecoder->bSkipToNextI) && (pDecoder->tVideoStreamInfo[Index]->pbVideoBuffer[pDecoder->tVideoStreamInfo[Index]->dwVideoBufDecoded + 12])) {
                            pDecoder->bSkipToNextI = FALSE_WMC;
                    };
		                            
                    if (pDecoder->bSkipToNextI) {
                        Bool_WMV bNotEndOfFrame = TRUE_WMV;
                        U8_WMV * pchBuffer = NULL;
                        U32_WMV uintActualBufferLength =0 ;
                       // U32_WMV uintTotalBufferLength =0;

                        while (bNotEndOfFrame == TRUE_WMV) {
                            WMVDecCBGetData ( (U32_WMV) pDecoder, 0,
                                &pchBuffer,
                                4,
                                &uintActualBufferLength,
                                &bNotEndOfFrame );

                            if ((pchBuffer == NULL) || (uintActualBufferLength ==0) )
                                bNotEndOfFrame = FALSE_WMV;

                        //  uintTotalBufferLength+=uintActualBufferLength;
                        }
                        *pnNumberOfSamples = 0;
                        *pbDecoded = pDecoder->tVideoStreamInfo[wMinVideoIndex]->wStreamId;
                    }
                    else 
                    {

                        if(
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_WMV2 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_WMV1 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_WMS2 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_WMS1 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_M4S2 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_MP4S ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_MP43 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_MP42 ||
                            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_MPG4 )
                        
                        {    
                        
                        
                            VidStatus = WMVideoDecDecodeData ((HWMVDecoder)pDecoder->tVideoStreamInfo[wMinVideoIndex]->pVDec, (U16_WMV *)pnNumberOfSamples);
                            if (WMV_Succeeded == VidStatus)
                            {
                                *pbDecoded = pDecoder->tVideoStreamInfo[wMinVideoIndex]->wStreamId;
                                *pnNumberOfSamples =1;
                            }
                            else
                            {
                               // WMVideoDecReset((HWMVDecoder)pDecoder->tVideoStreamInfo[wMinVideoIndex]->pVDec);
						        pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoBufDecoded += pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwBlockLeft;
						        pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwBlockLeft =0;
								*pbDecoded = pDecoder->tVideoStreamInfo[wMinVideoIndex]->wStreamId;
								
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
                        else if(pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_MSS1 ||
					            pDecoder->tVideoStreamInfo[wMinVideoIndex]->biCompression == FOURCC_MSS2)
                        {
                            
                            tWMCDecStatus TempStatus;
                            TempStatus = WMScDecodeData (hDecoder, wMinVideoIndex);
                            if (WMCDec_Succeeded == TempStatus)
                            {
                                *pbDecoded = pDecoder->tVideoStreamInfo[wMinVideoIndex]->wStreamId;
                                *pnNumberOfSamples =1;
                            }
                            else
                            {
                            
                                *pbDecoded = pDecoder->tVideoStreamInfo[wMinVideoIndex]->wStreamId;
						        pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoBufDecoded += pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwBlockLeft;
						        pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwBlockLeft =0;
                                return WMVDec_Failed; 
                            }
                        
                        }
#endif
                    }
                }
                else
#endif //_ASFPARSE_ONLY_
                {
                    *pbDecoded = pDecoder->tVideoStreamInfo[wMinVideoIndex]->wStreamId;
 //                   *pnNumberOfSamples =1;  // To be changed. Should be no of bytes
					
					if (pDecoder->tVideoStreamInfo[wMinVideoIndex]->bOutputisReady == TRUE_WMC) // Last output is not taken out
					{
						//Fix buffers
						U8_WMC TempOutBuff[1024];
						U32_WMC cbBuffer=0;
						do
						{
							WMCRawGetVideoData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
						}while(cbBuffer>0);
					
					}
					
            		pDecoder->tVideoStreamInfo[wMinVideoIndex]->bGotCompOutput = FALSE_WMC;
					
					pDecoder->tVideoStreamInfo[wMinVideoIndex]->bOutputisReady = TRUE_WMC;
		            memcpy (pnNumberOfSamples,(pDecoder->tVideoStreamInfo[wMinVideoIndex]->pbVideoBuffer + pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoBufDecoded + 2*sizeof(U32_WMC) ), sizeof(U32_WMC));
                }


            }
            else
            {
//                *pbDecoded = 0xff;
                *pbDecoded = 0xfe;
				pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoTimeStamp = 0xffffffff;
            }
            
			pDecoder->tVideoStreamInfo[wMinVideoIndex]->dwVideoTimeStamp = dwMinVideoPresTime;
			pDecoder->tVideoStreamInfo[wMinVideoIndex]->bHasGivenAnyOutput = TRUE_WMC;

            break;
        case Binary_WMC:
            if ((pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->dwBinaryBufCurOffset > pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->dwBinaryBufDecoded))
            {
                *pbDecoded = pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->wStreamId;
                pDecoder->i32CurrentBinaryStreamIndex = wMinBinaryIndex;
//                *pnNumberOfSamples =1;  // To be changed. Should be no of bytes
				if (pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->bOutputisReady == TRUE_WMC) // Last output is not taken out
				{
					//Fix buffers
					U8_WMC TempOutBuff[1024];
					U32_WMC cbBuffer=0;
					do
					{
						WMCRawGetBinaryData (TempOutBuff, &cbBuffer, hDecoder, (U32_WMC) 1024);
					}while(cbBuffer>0);
				
				}
				
				
            	pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->bGotCompOutput = FALSE_WMC;
				pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->bOutputisReady = TRUE_WMC;
		        memcpy (pnNumberOfSamples,(pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->pbBinaryBuffer + pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->dwBinaryBufDecoded + sizeof(U32_WMC) ), sizeof(U32_WMC));
            }
            else
            {
//                *pbDecoded = 0xff;
				*pbDecoded = 0xfe;
                pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->dwBinaryTimeStamp = 0xffffffff;
            }
			pDecoder->tBinaryStreamInfo[wMinBinaryIndex]->bHasGivenAnyOutput = TRUE_WMC;

            break;
        default:
            *pbDecoded = 0xff;        
        }
    }
    return WMCDec_Succeeded;
}

/*******************************************************************************************************************************/
tWMCDecStatus WMCDecClose (HWMCDECODER* phWMCDec)
{
    I32_WMC i;

 	WMFDecoderEx* pDecoder = (WMFDecoderEx *)(*phWMCDec);
    if (pDecoder == NULL)
        return WMCDec_InValidArguments;

    for(i=0; i< pDecoder->cTotalIndexs ; i++)
    {
        if (pDecoder->tIndexInfo[i].pIndexEntries !=NULL )
            wmvfree (pDecoder->tIndexInfo[i].pIndexEntries);
        pDecoder->tIndexInfo[i].pIndexEntries = NULL;
    
    }

    for(i=0; i<pDecoder->tHeaderInfo.wNoOfVideoStreams; i++ )
    {

        if(
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV2 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV1 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMS2 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMS1 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_M4S2 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP4S ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP43 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP42 ||
            pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MPG4 )
        {
        
#ifndef _ASFPARSE_ONLY_        
            if(pDecoder->tVideoStreamInfo[i]->pVDec !=NULL)
            {
                WMVideoDecClose ((HWMVDecoder) pDecoder->tVideoStreamInfo[i]->pVDec);
                pDecoder->tVideoStreamInfo[i]->pVDec = NULL;
            }
#endif
        }
#ifndef __NO_SCREEN__
        else if(pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS1 ||
				pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS2)
        {
            WMScreenDecoderDestroy ((LPVOID) pDecoder->tVideoStreamInfo[i]->pVDec);
            pDecoder->tVideoStreamInfo[i]->pVDec = NULL;
        }
#endif
        if (pDecoder->tVideoStreamInfo[i]->pbVideoBuffer != NULL)
        {
            wmvfree(pDecoder->tVideoStreamInfo[i]->pbVideoBuffer);
            pDecoder->tVideoStreamInfo[i]->pbVideoBuffer = NULL;


        }
#ifndef __NO_SCREEN__
       
        if (pDecoder->tVideoStreamInfo[i]->pSCInBuffer != NULL)
        {
            wmvfree(pDecoder->tVideoStreamInfo[i]->pSCInBuffer);
            pDecoder->tVideoStreamInfo[i]->pSCInBuffer = NULL;
        }
        if (pDecoder->tVideoStreamInfo[i]->pSCOutBuffer != NULL)
        {
            wmvfree(pDecoder->tVideoStreamInfo[i]->pSCOutBuffer);
            pDecoder->tVideoStreamInfo[i]->pSCOutBuffer = NULL;
        }
#endif        
	    if (pDecoder->tVideoStreamInfo[i] !=NULL)    
			wmvfree(pDecoder->tVideoStreamInfo[i]);
        pDecoder->tVideoStreamInfo[i] = NULL;
    }

    for(i=0; i<pDecoder->tHeaderInfo.wNoOfBinaryStreams; i++ )
    {
        if (pDecoder->tBinaryStreamInfo[i]->pbBinaryBuffer != NULL)
        {
            wmvfree(pDecoder->tBinaryStreamInfo[i]->pbBinaryBuffer);
            pDecoder->tBinaryStreamInfo[i]->pbBinaryBuffer = NULL;
        }
	    if (pDecoder->tBinaryStreamInfo[i] !=NULL)    
			wmvfree(pDecoder->tBinaryStreamInfo[i]);
        pDecoder->tBinaryStreamInfo[i] = NULL;
    }

    for(i=0; i<pDecoder->tHeaderInfo.wNoOfAudioStreams; i++ )
    {
#ifndef _ASFPARSE_ONLY_
        if(pDecoder->tAudioStreamInfo[i]->hMSA !=NULL)
        {
            if (pDecoder->tAudioStreamInfo[i]->nVersion > 0 && pDecoder->tAudioStreamInfo[i]->nVersion <4)
                WMARawDecClose(&(pDecoder->tAudioStreamInfo[i]->hMSA));
#ifndef __NO_SPEECH__
            else if (pDecoder->tAudioStreamInfo[i]->nVersion == 4) // Speech Codec
            {
                WMSRawDecClose(&(pDecoder->tAudioStreamInfo[i]->hMSA));
//                InitNSPLibWMSP(DLL_PROCESS_DETACH);
                MyInitNSP(INTELFFT_FREE_PROCESS);
            }
#endif
            pDecoder->tAudioStreamInfo[i]->hMSA = NULL;
        }
#endif //_ASFPARSE_ONLY_
        if(pDecoder->tAudioStreamInfo[i]->pbAudioBuffer !=NULL)
        {
    		wmvfree(pDecoder->tAudioStreamInfo[i]->pbAudioBuffer);
            pDecoder->tAudioStreamInfo[i]->pbAudioBuffer = NULL;
        }

	    if (pDecoder->tAudioStreamInfo[i] !=NULL)    
			wmvfree(pDecoder->tAudioStreamInfo[i]);
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

    if (pDecoder->pStreamIdnMediaType != NULL)
        wmvfree(pDecoder->pStreamIdnMediaType);
    pDecoder->pStreamIdnMediaType = NULL;
    

    if(pDecoder->m_pContentDesc != NULL) {
        if (pDecoder->m_pContentDesc->pchTitle !=NULL)
        {
            wmvfree (pDecoder->m_pContentDesc->pchTitle );
            pDecoder->m_pContentDesc->pchTitle  = NULL;
        }
        if (pDecoder->m_pContentDesc->pchAuthor !=NULL)
        {
            wmvfree (pDecoder->m_pContentDesc->pchAuthor );
            pDecoder->m_pContentDesc->pchAuthor  = NULL;
        }
        if (pDecoder->m_pContentDesc->pchCopyright !=NULL)
        {
            wmvfree (pDecoder->m_pContentDesc->pchCopyright );
            pDecoder->m_pContentDesc->pchCopyright  = NULL;
        }
        if (pDecoder->m_pContentDesc->pchDescription !=NULL)
        {
            wmvfree (pDecoder->m_pContentDesc->pchDescription );
            pDecoder->m_pContentDesc->pchDescription  = NULL;
        }
        if (pDecoder->m_pContentDesc->pchRating !=NULL)
        {
            wmvfree (pDecoder->m_pContentDesc->pchRating );
            pDecoder->m_pContentDesc->pchRating  = NULL;
        }
        wmvfree(pDecoder->m_pContentDesc);
        pDecoder->m_pContentDesc = NULL;
      
    }
  
    if(pDecoder->m_pCodecEntry  != NULL)
	{
        for(i = 0; i < (I32_WMC)pDecoder->m_dwNumCodec ; i++) 
		{
            if (NULL != pDecoder->m_pCodecEntry[i].m_pwCodecName)
			{
				wmvfree(pDecoder->m_pCodecEntry[i].m_pwCodecName );
				pDecoder->m_pCodecEntry[i].m_pwCodecName = NULL;
			}
            if (NULL != pDecoder->m_pCodecEntry[i].m_pwCodecDescription)
			{
				wmvfree(pDecoder->m_pCodecEntry[i].m_pwCodecDescription );
				pDecoder->m_pCodecEntry[i].m_pwCodecDescription = NULL;
			}
            if (NULL != pDecoder->m_pCodecEntry[i].m_pbCodecInfo)
			{
				wmvfree(pDecoder->m_pCodecEntry[i].m_pbCodecInfo );
				pDecoder->m_pCodecEntry[i].m_pbCodecInfo = NULL;
			}
        }
        wmvfree(pDecoder->m_pCodecEntry);
        pDecoder->m_pCodecEntry = NULL;
    }


	
    if(pDecoder->m_pMarkers != NULL) {
        for(i = 0; i < (I32_WMC)pDecoder->m_dwMarkerNum; i++) {
            wmvfree(pDecoder->m_pMarkers[i].m_pwDescName);
            pDecoder->m_pMarkers[i].m_pwDescName = NULL;
        }
        wmvfree(pDecoder->m_pMarkers);
        pDecoder->m_pMarkers = NULL;
    }

    if(pDecoder->tMetaDataEntry.pDescRec != NULL) {
        for(i = 0; i < (I32_WMC)pDecoder->tMetaDataEntry.m_wDescRecordsCount; i++) {
            SAFEMEMFREE(pDecoder->tMetaDataEntry.pDescRec[i].pwName );
            pDecoder->tMetaDataEntry.pDescRec[i].pwName  = NULL;
            SAFEMEMFREE(pDecoder->tMetaDataEntry.pDescRec[i].pData );
            pDecoder->tMetaDataEntry.pDescRec[i].pData  = NULL;
        }
        SAFEMEMFREE(pDecoder->tMetaDataEntry.pDescRec);
        pDecoder->tMetaDataEntry.pDescRec = NULL;
    }


    if (pDecoder->m_pExtendedContentDesc != NULL)
    {
         if(pDecoder->m_pExtendedContentDesc->cDescriptors > 0) {
            for (i = 0; i < pDecoder->m_pExtendedContentDesc->cDescriptors; i++) {
                wmvfree(pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary);
                wmvfree(pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName);
                pDecoder->m_pExtendedContentDesc->pDescriptors[i].uValue.pbBinary = NULL;
                pDecoder->m_pExtendedContentDesc->pDescriptors[i].pwszName = NULL;
            }
            wmvfree(pDecoder->m_pExtendedContentDesc->pDescriptors);
            pDecoder->m_pExtendedContentDesc->pDescriptors = NULL;
        }
        wmvfree (pDecoder->m_pExtendedContentDesc);
        pDecoder->m_pExtendedContentDesc = NULL;
    }    

    if(pDecoder->m_pScriptCommand != NULL) {
        if(pDecoder->m_pScriptCommand->num_types > 0 ) {
            wmvfree(pDecoder->m_pScriptCommand->type_name_len);
            pDecoder->m_pScriptCommand->type_name_len = NULL;
            for (i = 0; i < pDecoder->m_pScriptCommand->num_types; i++) {
                wmvfree(pDecoder->m_pScriptCommand->type_names[i]);
                pDecoder->m_pScriptCommand->type_names[i] = NULL;
            }
            wmvfree(pDecoder->m_pScriptCommand->type_names);
            pDecoder->m_pScriptCommand->type_names = NULL;
        }
        if(pDecoder->m_pScriptCommand->num_commands > 0 ) {
            wmvfree(pDecoder->m_pScriptCommand->command_param_len);
            pDecoder->m_pScriptCommand->command_param_len = NULL;
            for (i = 0; i < pDecoder->m_pScriptCommand->num_commands; i++) {
                if(pDecoder->m_pScriptCommand->commands[i].param) {
                    wmvfree(pDecoder->m_pScriptCommand->commands[i].param);
                    pDecoder->m_pScriptCommand->commands[i].param = NULL;
				}
            }
            wmvfree(pDecoder->m_pScriptCommand->commands);
            pDecoder->m_pScriptCommand->commands = NULL;
        }
        wmvfree(pDecoder->m_pScriptCommand);
        pDecoder->m_pScriptCommand = NULL;
    }    

    WMFCloseDecoder (phWMCDec);

	*phWMCDec = NULL;


    return WMCDec_Succeeded;

}

/*********************************************************************************************************************/
tWMCDecStatus WMCSkipToNextKey (HWMCDECODER hWMCDec)
{
    tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder = NULL_WMC;    
  
	if(hWMCDec==NULL)
        return WMCDec_InValidArguments;


    pDecoder = (WMFDecoderEx*) hWMCDec;
#ifndef _ASFPARSE_ONLY_
    pDecoder->bSkipToNextI = TRUE_WMV;
#endif
    return err;
}

/*********************************************************************************************************************/
tWMCDecStatus WMCDecDecodeData (HWMCDECODER hWMCDec, U32_WMC* pnStreamReadyForOutput, U32_WMC* pnNumberOfSamples, I32_WMC iHurryUp)
{
    tWMCDecStatus err = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder = NULL;    
    U64_WMC cbLastPacketOffset; 
//    U32_WMC cbRead;
    I32_WMC lBytesWritten =0;
    U64_WMC cbPacketOffset=0;
    I32_WMC i;
    Bool_WMC bReadyForOutput = FALSE_WMC;
	U16_WMC wOutputStreamCount =0;

    if(hWMCDec==NULL_WMC || pnStreamReadyForOutput == NULL_WMC || (iHurryUp <-2) || (iHurryUp >2) || (pnNumberOfSamples == NULL_WMC) )
        return WMCDec_InValidArguments;
    
    pDecoder = (WMFDecoderEx*) hWMCDec;

    *pnStreamReadyForOutput = 0xff;

    if (pDecoder->bParsed != TRUE_WMC)
        return WMCDec_InValidArguments;
	
	pDecoder->iHurryUp = iHurryUp;

    if (pDecoder->tPlannedOutputInfo.wTotalOutput == 0)
        return WMCDec_DecodeComplete;
	wOutputStreamCount = pDecoder->tPlannedOutputInfo.wTotalOutput;
#ifndef WMC_NO_BUFFER_MODE    
    do
    {
		cbPacketOffset = pDecoder->tHeaderInfo.cbCurrentPacketOffset;
		cbLastPacketOffset =pDecoder->tHeaderInfo.cbLastPacketOffset;
        
        if( cbPacketOffset < cbLastPacketOffset )
        {

            for (i=0; i< pDecoder->tPlannedOutputInfo.wTotalOutput; i++ )
            {
                switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
                {
                case Audio_WMC:
                    if (pDecoder->tAudioStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->cbNbFramesAudBuf > 2 )
                        bReadyForOutput = TRUE_WMC;
                    else
                        bReadyForOutput = FALSE_WMC;
                    break;
                case Video_WMC:
                    if (pDecoder->tVideoStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->cbNbFramesVidBuf > 3 )
                        bReadyForOutput = TRUE_WMC;
                    else
                        bReadyForOutput = FALSE_WMC;
                    break;
                case Binary_WMC:
                    if (pDecoder->tBinaryStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->cbNbFramesBinBuf > 3 )
                        bReadyForOutput = TRUE_WMC;
                    else
                        bReadyForOutput = FALSE_WMC;
                    break;
                default:
                    bReadyForOutput = FALSE_WMC;

                }
                if (bReadyForOutput == FALSE_WMC)
                    break;
            }
            
            if ( bReadyForOutput == TRUE_WMC)
            {	
                err = WMCDecDecodeFrame( hWMCDec, 3, (U16_WMC *)pnStreamReadyForOutput, pnNumberOfSamples);
                if (*pnStreamReadyForOutput != 0xff)
                    return err;
	        }
        

            err = WMCDecDecodeSeqPacket(hWMCDec);
            
/*			if( WMPERR_DEMOEXPIRED == err )
            {
                err = WMPERR_DECODE_COMPLETE;
				break;
            }		
*/			if( WMCDec_Succeeded != err )
            {
				break;
            }		
		}
		else
        {
			*pnStreamReadyForOutput = 0xfe;
		    err = WMCDecDecodeFrame( hWMCDec, 3, (U16_WMC *)pnStreamReadyForOutput, pnNumberOfSamples);
            
			while ((*pnStreamReadyForOutput == 0xfe)&&(wOutputStreamCount>0))
			{
				err = WMCDecDecodeFrame( hWMCDec, 3, (U16_WMC *)pnStreamReadyForOutput, pnNumberOfSamples);
				wOutputStreamCount--; 
			}
			
			if (*pnStreamReadyForOutput != 0xff)
                return err;
            else
                return WMCDec_DecodeComplete;

        }

    }
    while( 1 );

#else

	err =  WMCDecGetAndDecode (hWMCDec, (U32_WMC*) pnStreamReadyForOutput, (U32_WMC*) pnNumberOfSamples);
	if (*pnStreamReadyForOutput != 0xff)
        return err;
    else
        return WMCDec_DecodeComplete;
	
	
#endif
    return( err );
}

/*******************************************************************************************************************************************/
tWMCDecStatus WMCDecSetVideoOutputFormat (HWMCDECODER hWMCDec, tVideoFormat_WMC tVideoOutputFormat)
{

    tWMCDecStatus Status = WMCDec_Succeeded;
    U16_WMC i=0;
    WMFDecoderEx *pDecoder = NULL_WMC;

    if( NULL_WMC == hWMCDec )
        return( WMCDec_InValidArguments );

    pDecoder = (WMFDecoderEx *)(hWMCDec);


	do
    {
        if (tVideoOutputFormat != IYUV_WMV    &&
            tVideoOutputFormat != I420_WMV    &&
            tVideoOutputFormat != YUY2_WMV    &&
            tVideoOutputFormat != UYVY_WMV    &&
            tVideoOutputFormat != YVYU_WMV    &&
            tVideoOutputFormat != RGB24_WMV   &&
            tVideoOutputFormat != RGB555_WMV  &&
            tVideoOutputFormat != RGB565_WMV  &&
            tVideoOutputFormat != RGB32_WMV   &&
            tVideoOutputFormat != YVU9_WMV    &&
            tVideoOutputFormat != RGB8_WMV    &&
			tVideoOutputFormat != IGNORE_VIDEO)
        {
            Status = WMVDec_UnSupportedOutputFormat;
            break;
        }

        for (i=0; i< pDecoder->tHeaderInfo.wNoOfVideoStreams; i++)
        {
            pDecoder->tVideoStreamInfo[i]->tOutVideoFormat = tVideoOutputFormat;
			if(
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV2 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMV1 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMS2 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_WMS1 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_M4S2 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP4S ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP43 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MP42 ||
                pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MPG4 )
            {

                pDecoder->tVideoStreamInfo[i]->bIsDecodable = TRUE_WMC;
                switch (tVideoOutputFormat)
                {
                case IYUV_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_IYUV;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 12;
                    break;
                
                case I420_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_I420;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 12;
                    break;

                case YUY2_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_YUY2;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case UYVY_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_UYVY;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case YVYU_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_YVYU;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case RGB24_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 24;
                    break;
                
                case RGB555_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case RGB565_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_BITFIELDS;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case RGB32_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 32;
                    break;

                case YVU9_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_YVU9;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 9;
                    break;

                case RGB8_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 8;
                    break;
                case IGNORE_VIDEO:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = pDecoder->tVideoStreamInfo[i]->biCompression;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = pDecoder->tVideoStreamInfo[i]->biBitCount;
					pDecoder->tVideoStreamInfo[i]->bTobeDecoded = FALSE_WMC;
                    break;
                default:
                    return WMVDec_UnSupportedOutputFormat;

                }
                pDecoder->tVideoStreamInfo[i]->biSizeImageOut = (pDecoder->tVideoStreamInfo[i]->biWidth)*(pDecoder->tVideoStreamInfo[i]->biHeight)*(pDecoder->tVideoStreamInfo[i]->biBitCountOut)/8;

            }
///////////////////////////////////////////////////
#ifndef __NO_SCREEN__
			else if(pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS1 ||
					pDecoder->tVideoStreamInfo[i]->biCompression == FOURCC_MSS2)
			{
                LPBITMAPINFOHEADER    lpbiSrc = NULL_WMC;
                BITMAPINFOHEADER      BitmapInfo;  

                memset(&BitmapInfo,0,sizeof(BITMAPINFOHEADER));

                pDecoder->tVideoStreamInfo[i]->bIsDecodable = TRUE_WMC;
                switch (tVideoOutputFormat)
                {
                case RGB24_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 24;
                    break;
                
                case RGB555_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case RGB565_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_BITFIELDS;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 16;
                    break;
                
                case RGB32_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 32;
                    break;

                case RGB8_WMV:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = FOURCC_BI_RGB;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = 8;
                    break;
                case IGNORE_VIDEO:
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = pDecoder->tVideoStreamInfo[i]->biCompression;
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = pDecoder->tVideoStreamInfo[i]->biBitCount;
					pDecoder->tVideoStreamInfo[i]->bTobeDecoded = FALSE_WMC;
                    break;
                default:
                    return WMVDec_UnSupportedOutputFormat;

                }
                pDecoder->tVideoStreamInfo[i]->biSizeImageOut = (pDecoder->tVideoStreamInfo[i]->biWidth)*(pDecoder->tVideoStreamInfo[i]->biHeight)*(pDecoder->tVideoStreamInfo[i]->biBitCountOut)/8;

/*                BitmapInfo.biBitCount = pDecoder->tVideoStreamInfo[i]->biBitCountOut;
                BitmapInfo.biCompression  = pDecoder->tVideoStreamInfo[i]->biCompressionOut;
                BitmapInfo.biHeight  = pDecoder->tVideoStreamInfo[i]->biHeight;
                BitmapInfo.biWidth  = pDecoder->tVideoStreamInfo[i]->biWidth;
                BitmapInfo.biSizeImage = pDecoder->tVideoStreamInfo[i]->biSizeImageOut;
                BitmapInfo.biSize  = sizeof (BITMAPINFOHEADER);


                lpbiSrc = &BitmapInfo;
                hr = WMScreenDecoderInit( pDecoder->tVideoStreamInfo[i]->pVDec, 
                    pDecoder->tVideoStreamInfo[i]->bAdditionalInfo,
                    pDecoder->tVideoStreamInfo[i]->biSize  - sizeof (BITMAPINFOHEADER),
                    lpbiSrc);

                if (FAILED(hr))
                {
                    pDecoder->tVideoStreamInfo[i]->biBitCountOut = pDecoder->tVideoStreamInfo[i]->biBitCount;
                    pDecoder->tVideoStreamInfo[i]->biCompressionOut = pDecoder->tVideoStreamInfo[i]->biCompression;
                    pDecoder->tVideoStreamInfo[i]->biSizeImageOut = pDecoder->tVideoStreamInfo[i]->biSizeImage;

                    pDecoder->tVideoStreamInfo[i]->bIsDecodable = FALSE_WMC;
                    pDecoder->tVideoStreamInfo[i]->bTobeDecoded = FALSE_WMC;
                    return WMVDec_Failed;
                }

                pDecoder->tVideoStreamInfo[i]->pSCInBuffer = wmvalloc(pDecoder->tVideoStreamInfo[i]->biSizeImageOut);
                pDecoder->tVideoStreamInfo[i]->pSCOutBuffer = wmvalloc(pDecoder->tVideoStreamInfo[i]->biSizeImageOut);

                if(pDecoder->tVideoStreamInfo[i]->pSCInBuffer == NULL_WMC || pDecoder->tVideoStreamInfo[i]->pSCOutBuffer == NULL_WMC)
                {
                    return WMCDec_BadMemory;
                
                }

  */              


            }
#endif            
//////////////////////////////////////////////////								

            
            
            else
            {
                
                
                pDecoder->tVideoStreamInfo[i]->biBitCountOut = pDecoder->tVideoStreamInfo[i]->biBitCount;
                pDecoder->tVideoStreamInfo[i]->biCompressionOut = pDecoder->tVideoStreamInfo[i]->biCompression;
                pDecoder->tVideoStreamInfo[i]->biSizeImageOut = pDecoder->tVideoStreamInfo[i]->biSizeImage;

                pDecoder->tVideoStreamInfo[i]->bIsDecodable = FALSE_WMC;
                pDecoder->tVideoStreamInfo[i]->bTobeDecoded = FALSE_WMC;
            }
        
        }

 	}  while (0);
  
    return Status;

}


/****************************************************************************************************************/
tWMCDecStatus WMCDecSeek (HWMCDECODER hWMCDec, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn)
{

	U64_WMC cbPacketOffset =0;
    U8_WMC i=0, j=0;
	tWMCDecStatus rc = WMCDec_Succeeded;
    U32_WMC dwToIndexEntry =0;
    WMCINDEXENTRIES* pTempIndexEntry = NULL_WMC;
    U32_WMC dwPacketToGo =0;
    U8_WMC bAudioVideo =0;
    U32_WMC cbVideoSize =0;
    U32_WMC dwVideoTimeStamp=0;
    U16_WMC cbAudioOut =0;
    U32_WMC dwAudioTimeStamp =0;
    U8_WMC* pucVidBufTemp = NULL_WMC;
    U32_WMC dwMinimumPacketToGo = 0xffffffff;
    tMediaType_WMC MediaType;
	U32_WMC nStreamReadyForOutput;
	U32_WMC nNumberOfSamples;
	I16_WMC OutBuff[4096*4];
    U32_WMC nNumSamplesReturn =0;
    I64_WMC tPresentationTime =0;
    U8_WMC *pu8VideoOutBuffer = NULL_WMC;
    U32_WMC u32VideoOutBufferSize =0;
	U32_WMC u32OutDataSize =0;
	Bool_WMC bIsKeyFrame;
	U64_WMC u64LocalTimeRequest=0;
	Bool_WMC bVideoThere = FALSE_WMC;
	U64_WMC u64MyTimeReturn =0;

    PACKET_PARSE_INFO_EX ParseInfoEx;
	PAYLOAD_MAP_ENTRY_EX Payload;
	U32_WMC iPayload =0;
	Bool_WMC bDone = FALSE_WMC;
	Bool_WMC bHasIndexAndOutput = FALSE_WMC;
	Bool_WMC bAudioToBeDecoded = FALSE_WMC;
	Bool_WMC bVideoToBeDecoded = FALSE_WMC;
	U16_WMC wIdToGetOutput =0;
	U16_WMC wVideoIdTobedecoded =0;
	U16_WMC wAudioIdTobeOutput =0;
	U16_WMC wVideoIdTobeOutput =0;
	Bool_WMC bFirst = FALSE_WMC;
	U64_WMC u64LastPresTime=0;


 
    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC || pu64TimeReturn == NULL_WMC)
        return WMCDec_InValidArguments;

	memset(&ParseInfoEx, 0, sizeof (PACKET_PARSE_INFO_EX));
	memset(&Payload, 0, sizeof (PAYLOAD_MAP_ENTRY_EX));



	if (0==pDecoder->tPlannedOutputInfo.wTotalOutput)
        return WMCDec_InValidArguments;

	for (i=0; i < pDecoder->tPlannedOutputInfo.wTotalOutput; i++)
	{
		switch(pDecoder->tPlannedOutputInfo.tPlannedId[i].tMediaType)
		{
        case Audio_WMC:
			if (pDecoder->tAudioStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->bTobeDecoded)
			{
				bAudioToBeDecoded = TRUE_WMC;
//				wAudioIdTobedecoded = pDecoder->tAudioStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->wStreamId;
			}
			wAudioIdTobeOutput = pDecoder->tAudioStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->wStreamId;
			break;

        case Video_WMC:
			if (pDecoder->tVideoStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->bTobeDecoded)
			{
				wVideoIdTobedecoded = pDecoder->tVideoStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->wStreamId;
				bVideoToBeDecoded = TRUE_WMC;
			}
			wVideoIdTobeOutput = pDecoder->tVideoStreamInfo[pDecoder->tPlannedOutputInfo.tPlannedId[i].wStreamIndex]->wStreamId;
			break;
		}
	
	
	}



    dwMinimumPacketToGo = (U32_WMC)pDecoder->tHeaderInfo.cbLastPacketOffset;

	*pu64TimeReturn = 0;
	
	u64TimeRequest= u64TimeRequest + (U64_WMC)pDecoder->tHeaderInfo.msPreroll ;

	if ((u64TimeRequest < (U64_WMC)pDecoder->tHeaderInfo.msPreroll ) || (u64TimeRequest >(U64_WMC)pDecoder->tHeaderInfo.msDuration))
		return WMCDec_InValidArguments;

    if (pDecoder->cTotalIndexs >0)
    {
        for(i=0; i< pDecoder->cTotalIndexs ; i++)
        {
            rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) pDecoder->tIndexInfo[i].nStreamId, &MediaType);

            if (WMCDec_Succeeded !=rc)
                return rc;

            switch(MediaType)
            {
            case Audio_WMC:
                for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
                {
                    if( pDecoder->tIndexInfo[i].nStreamId == pDecoder->tAudioStreamInfo[j]->wStreamId)
                    {
                        if (pDecoder->tAudioStreamInfo[j]->bWantOutput == TRUE_WMC )
                        {
							u64LocalTimeRequest = u64TimeRequest /*+ (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
                            if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
                                return WMCDec_InValidArguments;
                            pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
                            pTempIndexEntry+=dwToIndexEntry;
                            dwPacketToGo = pTempIndexEntry->dwPacket;
                            if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
                                return WMCDec_InValidArguments;
                            if (dwMinimumPacketToGo > dwPacketToGo)
                            {
                                dwMinimumPacketToGo = dwPacketToGo;
                                pDecoder->tHeaderInfo.cbCurrentPacketOffset = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)dwPacketToGo * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize);
                                pDecoder->tHeaderInfo.cbNextPacketOffset  = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)(dwPacketToGo) * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize );
                            }
                             // To go to precise point
                            pDecoder->tAudioStreamInfo[j]->dwAudioBufCurOffset = 0;
	                        pDecoder->tAudioStreamInfo[j]->cbNbFramesAudBuf = 0;
                            pDecoder->tAudioStreamInfo[j]->dwAudioBufDecoded = 0;
	                        pDecoder->tAudioStreamInfo[j]->bBlockStart = 0;
	                        pDecoder->tAudioStreamInfo[j]->dwBlockLeft = 0;
	                        pDecoder->tAudioStreamInfo[j]->dwPayloadLeft = 0;
	                        pDecoder->tAudioStreamInfo[j]->dwAudPayloadPresTime = 0;
                            pDecoder->tAudioStreamInfo[j]->dwAudioTimeStamp = 0.0;
                            pDecoder->tAudioStreamInfo[j]->bFirstTime = FALSE_WMC;
							bHasIndexAndOutput = TRUE_WMC;
                        }
                    }
            
                }
                break;
            case Video_WMC:
                for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
                {
                    if(pDecoder->tIndexInfo[i].nStreamId == pDecoder->tVideoStreamInfo[j]->wStreamId)
                    {
                        if (pDecoder->tVideoStreamInfo[j]->bWantOutput == TRUE_WMC )
                        {
							WMCDecGetPreviousFrameTime (hWMCDec, pDecoder->tVideoStreamInfo[j]->wStreamId, u64TimeRequest -  (U64_WMC)pDecoder->tHeaderInfo.msPreroll , &u64MyTimeReturn);

							u64LocalTimeRequest = u64TimeRequest/* + (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;

							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
                            if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
                                return WMCDec_InValidArguments;
                            pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
                            pTempIndexEntry+=dwToIndexEntry;
                            dwPacketToGo = pTempIndexEntry->dwPacket;
                            if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
                                return WMCDec_InValidArguments;
                            if (dwMinimumPacketToGo > dwPacketToGo)
                            {
                                dwMinimumPacketToGo = dwPacketToGo;
								if (dwMinimumPacketToGo >0)
									dwMinimumPacketToGo -=1;
                                pDecoder->tHeaderInfo.cbCurrentPacketOffset = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)dwPacketToGo * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize);
                                pDecoder->tHeaderInfo.cbNextPacketOffset  = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)(dwPacketToGo) * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize );
                            }
                             // To go to precise point
                            pDecoder->tVideoStreamInfo[j]->bFirst =  FALSE_WMC;
                            pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset = 0;
	                        pDecoder->tVideoStreamInfo[j]->bBlockStart = 0;
	                        pDecoder->tVideoStreamInfo[j]->dwBlockLeft = 0;
	                        pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
	                        pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf = 0;
                            pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded = 0;
                            pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
                            pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
	                        pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
                            pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
	                        pDecoder->tVideoStreamInfo[j]->cbFrame = 0;
							bVideoThere = TRUE_WMC;
							bHasIndexAndOutput = TRUE_WMC;

                        }
                    }
            
                }
                break;
            case Binary_WMC:
                for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
                {
                    if(pDecoder->tIndexInfo[i].nStreamId == pDecoder->tBinaryStreamInfo[j]->wStreamId)
                    {
                        if (pDecoder->tBinaryStreamInfo[j]->bWantOutput == TRUE_WMC )
                        {
							u64LocalTimeRequest = u64TimeRequest /*+ (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
                            if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
                                return WMCDec_InValidArguments;
                            pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
                            pTempIndexEntry+=dwToIndexEntry;
                            dwPacketToGo = pTempIndexEntry->dwPacket;
                            if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
                                return WMCDec_InValidArguments;
                            if (dwMinimumPacketToGo > dwPacketToGo)
                            {
                                dwMinimumPacketToGo = dwPacketToGo;
                                pDecoder->tHeaderInfo.cbCurrentPacketOffset = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)dwPacketToGo * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize);
                                pDecoder->tHeaderInfo.cbNextPacketOffset  = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)(dwPacketToGo) * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize );
                            }
                             // To go to precise point
                            pDecoder->tBinaryStreamInfo[j]->dwBinaryBufCurOffset = 0;
                            pDecoder->tBinaryStreamInfo[j]->dwBinaryTimeStamp = 0;
	                        pDecoder->tBinaryStreamInfo[j]->cbNbFramesBinBuf = 0;
	                        pDecoder->tBinaryStreamInfo[j]->dwBlockLeft = 0;
	                        pDecoder->tBinaryStreamInfo[j]->dwPayloadLeft = 0;
   	                        pDecoder->tBinaryStreamInfo[j]->cbUsed = 0;
                            pDecoder->tBinaryStreamInfo[j]->dwFrameSize = 0; 
	                        pDecoder->tBinaryStreamInfo[j]->cbFrame = 0;
                            pDecoder->tBinaryStreamInfo[j]->dwNbFrames = 0;
                            pDecoder->tBinaryStreamInfo[j]->dwBinaryBufDecoded = 0;
							bHasIndexAndOutput = TRUE_WMC;
                        }
                    }
                }
                break;
            default:
                return WMCDec_InValidArguments;

            }

        } 
        
    }
    
	
	if (FALSE_WMC == bHasIndexAndOutput)
    {
        u64MyTimeReturn = u64TimeRequest - pDecoder->tHeaderInfo.msPreroll;
        rc = WMFDecTimeToOffset (hWMCDec, (U32_WMC)u64TimeRequest - pDecoder->tHeaderInfo.msPreroll, &cbPacketOffset);

	    if (rc == 0)
        {
			do
			
			{
			//	cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);
				rc = WMCDecParseVirtualPacketHeader(hWMCDec, (U32_WMC)cbPacketOffset, &ParseInfoEx);
				if (rc != WMCDec_Succeeded)
					return rc;

				for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
				{
        
					rc = WMCDecParseVirtualPayloadHeader(hWMCDec, (U32_WMC)cbPacketOffset, &ParseInfoEx, &Payload);
					if (rc != WMCDec_Succeeded)
						return rc;

					if (bVideoToBeDecoded)
					{
                        // Find out the start of key frame before this  
	
						if (Payload.bIsKeyFrame && (Payload.cbObjectOffset ==0)&&(Payload.msObjectPres < u64TimeRequest)&&(wVideoIdTobedecoded == (U16_WMC)Payload.bStreamId))
						{
							wIdToGetOutput = (U16_WMC)Payload.bStreamId;
							bDone = TRUE_WMC;
							break;
						}
					}
					else if (
						((Payload.cbRepData == 8) || (Payload.cbRepData == 1))&&
						(((wVideoIdTobeOutput == (U16_WMC)Payload.bStreamId)&&(Payload.cbObjectOffset ==0))||
						(wAudioIdTobeOutput == (U16_WMC)Payload.bStreamId))		
						) // Any valid payload will do
				
					
					
					{
						if ((Payload.msObjectPres > u64TimeRequest) && (cbPacketOffset > pDecoder->tHeaderInfo.cbHeader))
							break;//cbPacketOffset -= pDecoder->tHeaderInfo.cbPacketSize;
						else
						{
							bDone = TRUE_WMC;
							wIdToGetOutput = (U16_WMC)Payload.bStreamId;

						}

						break;
					}

				}
				if ((cbPacketOffset > pDecoder->tHeaderInfo.cbHeader) &&(bDone == FALSE_WMC))
					cbPacketOffset -= pDecoder->tHeaderInfo.cbPacketSize;

			}while((bDone == FALSE_WMC) && (cbPacketOffset > pDecoder->tHeaderInfo.cbHeader));

            
           pDecoder->tHeaderInfo.cbCurrentPacketOffset = cbPacketOffset;
           pDecoder->tHeaderInfo.cbNextPacketOffset =  cbPacketOffset;
        }
	    else
		    return WMCDec_InValidArguments;
		

		if ((bVideoToBeDecoded == FALSE_WMC)&&(bAudioToBeDecoded == FALSE_WMC))
		{

			if (cbPacketOffset > pDecoder->tHeaderInfo.cbHeader)
				cbPacketOffset -=pDecoder->tHeaderInfo.cbPacketSize;
			bDone = FALSE_WMC;
			bFirst = FALSE_WMC;
			
			do
			
			{
				
				rc = WMCDecParseVirtualPacketHeader(hWMCDec, (U32_WMC)cbPacketOffset, &ParseInfoEx);
				if (rc != WMCDec_Succeeded)
					return rc;


				for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
				{
        
					rc = WMCDecParseVirtualPayloadHeader(hWMCDec, (U32_WMC)cbPacketOffset, &ParseInfoEx, &Payload);
					if (rc != WMCDec_Succeeded)
						return rc;

					if ((wIdToGetOutput == Payload.bStreamId))
					{
						if (bFirst == FALSE_WMC)
						{
							u64LastPresTime = (U64_WMC) Payload.msObjectPres;
							bFirst = TRUE_WMC;
						}
						
						if (Payload.msObjectPres >= u64TimeRequest)
						{
							u64MyTimeReturn = u64LastPresTime - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
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
				cbPacketOffset +=pDecoder->tHeaderInfo.cbPacketSize;
			}while((bDone == FALSE_WMC) && (cbPacketOffset < (pDecoder->tHeaderInfo.cbHeader +pDecoder->tHeaderInfo.cbPacketSize*pDecoder->tHeaderInfo.cPackets)));
		}
    
    }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
    {
#ifndef _ASFPARSE_ONLY_
        if ((pDecoder->tAudioStreamInfo[j]->bWantOutput == TRUE_WMC)
			&&(pDecoder->tAudioStreamInfo[j]->bIsDecodable == TRUE_WMC)
			&&(pDecoder->tAudioStreamInfo[j]->nVersion >0 && pDecoder->tAudioStreamInfo[j]->nVersion <4)
			&&(pDecoder->tAudioStreamInfo[j]->hMSA !=NULL_WMC) 
			)
        {
			WMARawDecReset (pDecoder->tAudioStreamInfo[j]->hMSA);
//			WMARawDecStatus(pDecoder->tAudioStreamInfo[j]->hMSA);
		}
#endif        
		pDecoder->tAudioStreamInfo[j]->dwAudioBufCurOffset = 0;
	    pDecoder->tAudioStreamInfo[j]->cbNbFramesAudBuf = 0;
        pDecoder->tAudioStreamInfo[j]->dwAudioBufDecoded = 0;
	    pDecoder->tAudioStreamInfo[j]->bBlockStart = 0;
	    pDecoder->tAudioStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tAudioStreamInfo[j]->dwPayloadLeft = 0;
	    pDecoder->tAudioStreamInfo[j]->dwAudPayloadPresTime = 0;
        pDecoder->tAudioStreamInfo[j]->dwAudioTimeStamp = 0.0;
        pDecoder->tAudioStreamInfo[j]->wmar = WMA_S_NO_MORE_FRAME;
		pDecoder->tAudioStreamInfo[j]->bFirstTime = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tAudioStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tAudioStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tAudioStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tAudioStreamInfo[j]->parse_state = csWMCNewAsfPacket;

#endif

     }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
    {
        pDecoder->tVideoStreamInfo[j]->bFirst =  FALSE_WMC;
        pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset = 0;
	    pDecoder->tVideoStreamInfo[j]->bBlockStart = 0;
	    pDecoder->tVideoStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
	    pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf = 0;
        pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded = 0;
        pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
        pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
	    pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
        pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
	    pDecoder->tVideoStreamInfo[j]->cbFrame = 0;
		pDecoder->tVideoStreamInfo[j]->pStoreFrameStartPointer = NULL_WMC;
		pDecoder->tVideoStreamInfo[j]->pStoreNextFrameStartPointer = NULL_WMC;
		pDecoder->tVideoStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
		pDecoder->tVideoStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tVideoStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tVideoStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tVideoStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tVideoStreamInfo[j]->parse_state = csWMCNewAsfPacket;
        pDecoder->tVideoStreamInfo[j]->dwNextVideoTimeStamp = 0;

#endif


    }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
    {
        pDecoder->tBinaryStreamInfo[j]->dwBinaryBufCurOffset = 0;
        pDecoder->tBinaryStreamInfo[j]->dwBinaryTimeStamp = 0;
	    pDecoder->tBinaryStreamInfo[j]->cbNbFramesBinBuf = 0;
	    pDecoder->tBinaryStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tBinaryStreamInfo[j]->dwPayloadLeft = 0;
   	    pDecoder->tBinaryStreamInfo[j]->cbUsed = 0;
        pDecoder->tBinaryStreamInfo[j]->dwFrameSize = 0; 
	    pDecoder->tBinaryStreamInfo[j]->cbFrame = 0;
        pDecoder->tBinaryStreamInfo[j]->dwNbFrames = 0;
        pDecoder->tBinaryStreamInfo[j]->dwBinaryBufDecoded = 0;
		pDecoder->tBinaryStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
		pDecoder->tBinaryStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tBinaryStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->parse_state = csWMCNewAsfPacket;
        pDecoder->tBinaryStreamInfo[j]->dwNextBinaryTimeStamp = 0;

#endif
    }


    pDecoder->parse_state = csWMCNewAsfPacket;

	if (u64TimeRequest == pDecoder->tHeaderInfo.msPreroll)
	{
		*pu64TimeReturn =0;
		return rc;

	}


	pDecoder->u64SeekTimeRequest = u64TimeRequest;
	pDecoder->bInSeekState = TRUE;

	do
	{
		rc = WMCDecDecodeData (hWMCDec, &nStreamReadyForOutput, &nNumberOfSamples, (I32_WMC)0);
    
		if (nNumberOfSamples >0)
		{
			cbAudioOut = 0;
			for (i=0; i<pDecoder->tHeaderInfo.wNoOfStreams; i++ )
			{
				if (pDecoder->pStreamIdnMediaType[i].wStreamId == (U16_WMC)nStreamReadyForOutput)
				{
            
					switch(pDecoder->pStreamIdnMediaType[i].MediaType)
					{
					case Audio_WMC:
						for (j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams ; j++)
						{
							 if ( pDecoder->tAudioStreamInfo[j]->wStreamId == (U16_WMC)nStreamReadyForOutput)
							 {
 								do
								{
									cbAudioOut = 2048;
									rc = WMCDecGetAudioOutput (hWMCDec, (I16_WMC *)OutBuff, NULL_WMC, cbAudioOut, &nNumSamplesReturn, &tPresentationTime );
									if (nNumSamplesReturn > 0)
									{
										if ((tPresentationTime >= (I64_WMC)u64MyTimeReturn) && bVideoThere == FALSE_WMC)
										{
											pDecoder->bInSeekState = FALSE;
											*pu64TimeReturn = tPresentationTime;
										//	break;
										}
									
									}
								}while(nNumSamplesReturn >0);

								break;
							 }
						}
						break;

					case Video_WMC:
						for (j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
						{
							 if ( pDecoder->tVideoStreamInfo[j]->wStreamId== (U16_WMC)nStreamReadyForOutput)
							 {
                         
        						do
								{
									 rc = WMCDecGetVideoOutput (hWMCDec, NULL_WMC /*pu8VideoOutBuffer*/, 0/*u32VideoOutBufferSize*/, &u32OutDataSize, &tPresentationTime, &bIsKeyFrame, WMC_DispRotate0);
									if ((tPresentationTime >= (I64_WMC)u64MyTimeReturn))
									{
										pDecoder->bInSeekState = FALSE_WMC;
										*pu64TimeReturn = tPresentationTime;
									//	break;
									}
								}while(u32OutDataSize >0);
								break;
							 }
						}
						break;
					}
					break;
				}
			}
		}
	}while ((rc == 0) && (pDecoder->bInSeekState == TRUE));

	pDecoder->bInSeekState = FALSE;

	return rc;
}


/****************************************************************************************************************/
#ifndef __NO_SCREEN__

tWMCDecStatus WMScDecodeData (HWMCDECODER hWMCDec, U16_WMC wIndex )
{


    tWMVDecodeStatus CBStatus = WMV_Succeeded;
    U8_WMV  *pSCBuffer = NULL_WMC;
    U32_WMV uintActualBufferLength =0;
    Bool_WMV    bNotEndOfFrame = TRUE_WMV; 
    U32_WMC dwCompressedFrameSize =0;
    WMFDecoderEx *pDecoder =NULL_WMC;
    BITMAPPALETTE      BitmapInfo;  
    HRESULT hr;
    tWMCDecStatus tStatus = WMCDec_Succeeded;

    memset(&BitmapInfo,0,sizeof(BITMAPPALETTE));

   
    pDecoder = (WMFDecoderEx *)hWMCDec;

        
    do
    {
        CBStatus = WMVDecCBGetData ( (U32_WMV) hWMCDec, (U32_WMV) 0, &pSCBuffer, (U32_WMV) 0, 
                                  &uintActualBufferLength, &bNotEndOfFrame );

        if (pSCBuffer == NULL_WMC)
            return WMCDec_BadData;



        if (dwCompressedFrameSize + uintActualBufferLength <= pDecoder->tVideoStreamInfo[wIndex]->biSizeImageOut)
        {
            memcpy(pDecoder->tVideoStreamInfo[wIndex]->pSCInBuffer + dwCompressedFrameSize, pSCBuffer, uintActualBufferLength);
        }
        else
        {
            return WMCDec_BufferTooSmall;
        }
        dwCompressedFrameSize+=uintActualBufferLength;

    }while((CBStatus == WMV_Succeeded) && (bNotEndOfFrame = TRUE_WMV));

    if ((CBStatus == WMV_EndOfFrame)&&(bNotEndOfFrame == FALSE_WMV))
    {

        BitmapInfo.BitmapHdr.biBitCount = pDecoder->tVideoStreamInfo[wIndex]->biBitCountOut;
        BitmapInfo.BitmapHdr.biCompression  = pDecoder->tVideoStreamInfo[wIndex]->biCompressionOut;
        BitmapInfo.BitmapHdr.biHeight  = pDecoder->tVideoStreamInfo[wIndex]->biHeight;
        BitmapInfo.BitmapHdr.biWidth  = pDecoder->tVideoStreamInfo[wIndex]->biWidth;
        BitmapInfo.BitmapHdr.biSizeImage = pDecoder->tVideoStreamInfo[wIndex]->biSizeImageOut;
        
        if (pDecoder->tVideoStreamInfo[wIndex]->biBitCountOut ==8)
        {
            BitmapInfo.BitmapHdr.biSize  = sizeof (BITMAPINFOHEADER) + 256*3;
            memcpy(BitmapInfo.bPlaette, StandardPalette, 256*3);
        }
        else
            BitmapInfo.BitmapHdr.biSize  = sizeof (BITMAPINFOHEADER);

        pDecoder->tVideoStreamInfo[wIndex]->bScOutGiven = FALSE_WMC;

        hr = WMScreenDecoderDecode(
							    (Void_WMC*)pDecoder->tVideoStreamInfo[wIndex]->pVDec,
							    (U8_WMC *)pDecoder->tVideoStreamInfo[wIndex]->pSCInBuffer, 
							    dwCompressedFrameSize, 
							    pDecoder->tVideoStreamInfo[wIndex]->biWidth ,
							    pDecoder->tVideoStreamInfo[wIndex]->biHeight,
							    (BITMAPINFOHEADER *)&(BitmapInfo),
							    pDecoder->tVideoStreamInfo[wIndex]->pSCOutBuffer,
							    0,
							    0,
							    pDecoder->tVideoStreamInfo[wIndex]->biWidth ,
							    pDecoder->tVideoStreamInfo[wIndex]->biHeight,
							    0);


        if (FAILED (hr))
	        tStatus = WMVDec_Failed;
    }
    else
        tStatus = WMCDec_BadData;

	return tStatus;
}


tWMCDecStatus WMScGetOutData (HWMCDECODER hWMCDec, U8_WMC *pu8VideoOutBuffer, U32_WMC u32VideoOutBufferSize, U16_WMC wIndex )
{


    tWMCDecStatus tStatus = WMCDec_Succeeded;
    WMFDecoderEx *pDecoder =NULL_WMC;

    pDecoder = (WMFDecoderEx *)hWMCDec;

    if ((pu8VideoOutBuffer != NULL_WMC) && (u32VideoOutBufferSize < pDecoder->tVideoStreamInfo[wIndex]->biSizeImageOut))
        return WMCDec_InValidArguments;
    
    if (pDecoder->tVideoStreamInfo[wIndex]->bScOutGiven == TRUE_WMC)
        return WMVDec_NoMoreOutput;
    else
    {
        if (pu8VideoOutBuffer !=NULL_WMC)
			memcpy(pu8VideoOutBuffer, pDecoder->tVideoStreamInfo[wIndex]->pSCOutBuffer, pDecoder->tVideoStreamInfo[wIndex]->biSizeImageOut);
        pDecoder->tVideoStreamInfo[wIndex]->bScOutGiven = TRUE_WMC;
    }

	return tStatus;
}


#endif
/************************************************************************************************/

tWMCDecStatus WMCDecGetIFrameTime (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn, U32_WMC* pdwFirstPacketOffset, U32_WMC* pdwPacketSize, U32_WMC* pdwPackets, U64_WMC* pu64FirstPayloadPresTime, U32_WMC * pFirstPayloadOffset, U8_WMC * pFirstPayloadOID, U32_WMC * pSendTime, U64_WMC *pFirstAudioPTime, U8_WMC * pFirstAudioPayloadOID)
{

	U64_WMC cbPacketOffset =0;
    U8_WMC i=0, j=0;
	tWMCDecStatus rc = WMCDec_Succeeded;
    U32_WMC dwToIndexEntry =0;
    WMCINDEXENTRIES* pTempIndexEntry = NULL_WMC;
    U32_WMC dwPacketToGo =0;
    tMediaType_WMC MediaType;
	U32_WMC cbCurrentPacketOffset =0;
	PACKET_PARSE_INFO_EX ParseInfoEx;
	PAYLOAD_MAP_ENTRY_EX Payload;
	U32_WMC iPayload =0;
	U64_WMC u64LocalTimeRequest=0;
	Bool_WMC bFirst = FALSE_WMC;
 	Bool_WMC bFirstAudio = FALSE_WMC;

    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC || pu64TimeReturn == NULL_WMC)
        return WMCDec_InValidArguments;


	memset(&ParseInfoEx, 0, sizeof (PACKET_PARSE_INFO_EX));
	memset(&Payload, 0, sizeof (PAYLOAD_MAP_ENTRY_EX));

	*pu64TimeReturn = 0;
	if (pFirstAudioPTime !=NULL_WMC)
		*pFirstAudioPTime =0;
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
							u64LocalTimeRequest = u64TimeRequest /*+ (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;
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
							u64LocalTimeRequest = u64TimeRequest /*+ (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
							if (pdwPackets !=NULL_WMC)
							*pdwPackets = dwPacketToGo;

						}
        
					}
					break;
				case Binary_WMC:
					for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
					{
						if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
						{
							u64LocalTimeRequest = u64TimeRequest /*+ (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000*/;
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
	}


    if (pdwFirstPacketOffset !=NULL_WMC)
		*pdwFirstPacketOffset = pDecoder->tHeaderInfo.cbHeader;

    if (pdwPacketSize !=NULL_WMC)
		*pdwPacketSize = pDecoder->tHeaderInfo.cbPacketSize;

	cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);


	rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);

	if (pSendTime !=NULL_WMC)
		*pSendTime = ParseInfoEx.dwSCR;

    for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
    {
        
		rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
		if (rc != WMCDec_Succeeded)
			return rc;
		if (wStreamId == Payload.bStreamId)
		{
			if (bFirst == FALSE)
			{
				if (pu64FirstPayloadPresTime !=NULL_WMC)
					*pu64FirstPayloadPresTime = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;

				if (pFirstPayloadOffset !=NULL_WMC)
					*pFirstPayloadOffset = Payload.cbObjectOffset;
				
				if (pFirstPayloadOID !=NULL_WMC)
					*pFirstPayloadOID = Payload.bObjectId;
				bFirst = TRUE;

			}
			
			if (Payload.bIsKeyFrame == 0)
				continue;
			else
		        *pu64TimeReturn = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
		}
		else
		{
			if ((pDecoder->tHeaderInfo.wNoOfAudioStreams >0) && (bFirstAudio == FALSE_WMC))
			{
				for (j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
				{
					if (Payload.bStreamId == pDecoder->tAudioStreamInfo[j]->wStreamId)
					{
						if (pFirstAudioPTime !=NULL_WMC)
							*pFirstAudioPTime = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
						bFirstAudio = TRUE_WMC;
                        if (pFirstAudioPayloadOID !=NULL_WMC)
                            *pFirstAudioPayloadOID = Payload.bObjectId;
						break;
					}
				}
			}
		}

	}

	while ((pDecoder->tHeaderInfo.wNoOfAudioStreams >0) && (bFirstAudio == FALSE_WMC) && (dwPacketToGo < pDecoder->tHeaderInfo.cPackets))
	{
		dwPacketToGo ++;
		cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);

		rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);

		for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
		{
        
			rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
			if (rc != WMCDec_Succeeded)
				return rc;
			if ((pDecoder->tHeaderInfo.wNoOfAudioStreams >0) && (bFirstAudio == FALSE_WMC))
			{
				for (j=0; j<(U8_WMC)pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
				{
					if (Payload.bStreamId == pDecoder->tAudioStreamInfo[j]->wStreamId)
					{
						if (pFirstAudioPTime !=NULL_WMC)
							*pFirstAudioPTime = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
                        if (pFirstAudioPayloadOID !=NULL_WMC)
                            *pFirstAudioPayloadOID = Payload.bObjectId;
						bFirstAudio = TRUE_WMC;
						break;
					}
				}
			}
			if (bFirstAudio == TRUE_WMC)
				break;

		}
	}
	

	return rc;
}

/****************************************************************************************************************/


tWMCDecStatus WMCDecGetNextIFrameTime (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn, U32_WMC* pdwPackets, U32_WMC* pdwRemainingPackets, U64_WMC* pFirstAudioPTime, U32_WMC* pdwSCR )
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
 	Bool_WMC bFirstAudio = FALSE_WMC;
 
    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC || pu64TimeReturn == NULL_WMC)
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
                    
							u64LocalTimeRequest = u64TimeRequest + (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
							dwNewPacketToGo = dwPacketToGo;
							while (dwNewPacketToGo == dwPacketToGo)
							{
								pTempIndexEntry++;
								dwNewPacketToGo = pTempIndexEntry->dwPacket;
								if (dwNewPacketToGo > pDecoder->tHeaderInfo.cPackets)
									return WMCDec_InValidArguments;
							}
							if (pdwPackets !=NULL)
								*pdwPackets = dwNewPacketToGo;

						}
					}
					break;
				case Video_WMC:
					for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
					{
						if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
						{
							u64LocalTimeRequest = u64TimeRequest + (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
							dwNewPacketToGo = dwPacketToGo;
							while (dwNewPacketToGo == dwPacketToGo)
							{
								pTempIndexEntry++;
								dwNewPacketToGo = pTempIndexEntry->dwPacket;
								if (dwNewPacketToGo > pDecoder->tHeaderInfo.cPackets)
									return WMCDec_InValidArguments;
							}
							if (pdwPackets !=NULL)
								*pdwPackets = dwNewPacketToGo;

						}
        
					}
					break;
				case Binary_WMC:
					for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
					{
						if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
						{
							u64LocalTimeRequest = u64TimeRequest + (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000;
							dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
							if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
								return WMCDec_InValidArguments;
							pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
							pTempIndexEntry+=dwToIndexEntry;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
							dwNewPacketToGo = dwPacketToGo;
							while (dwNewPacketToGo == dwPacketToGo)
							{
								pTempIndexEntry++;
								dwNewPacketToGo = pTempIndexEntry->dwPacket;
								if (dwNewPacketToGo > pDecoder->tHeaderInfo.cPackets)
									return WMCDec_InValidArguments;
							}
							if (pdwPackets !=NULL)
								*pdwPackets = dwNewPacketToGo;

						}
					}
					break;
				default:
					return WMCDec_InValidArguments;

				}

        
			}
		}
	}
	
	if (pdwRemainingPackets !=NULL)
		*pdwRemainingPackets = pDecoder->tHeaderInfo.cPackets -  dwNewPacketToGo;

    cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwNewPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);


	rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);


    for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
    {
        
		rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
		if (rc != WMCDec_Succeeded)
			return rc;

		if (wStreamId == Payload.bStreamId)
		{
			if (Payload.bIsKeyFrame == 0)
				continue;
			else
			{
		        *pu64TimeReturn = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
				
				if (pdwSCR != NULL)
					*pdwSCR = ParseInfoEx.dwSCR;
			}
		}
		else
		{
			if ((pDecoder->tHeaderInfo.wNoOfAudioStreams >0) && (bFirstAudio == FALSE_WMC))
			{
				for (j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
				{
					if (Payload.bStreamId == pDecoder->tAudioStreamInfo[j]->wStreamId)
					{
						if (pFirstAudioPTime !=NULL_WMC)
							*pFirstAudioPTime = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
						bFirstAudio = TRUE_WMC;
						break;
					}
				}
			}
		}

	}


	while ((pDecoder->tHeaderInfo.wNoOfAudioStreams >0) && (bFirstAudio == FALSE_WMC) && (dwNewPacketToGo < pDecoder->tHeaderInfo.cPackets))
	{
		dwNewPacketToGo ++;
		cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwNewPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);

		rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);

		for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
		{
        
			rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
			if (rc != WMCDec_Succeeded)
				return rc;
			if ((pDecoder->tHeaderInfo.wNoOfAudioStreams >0) && (bFirstAudio == FALSE_WMC))
			{
				for (j=0; j<(U8_WMC)pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
				{
					if (Payload.bStreamId == pDecoder->tAudioStreamInfo[j]->wStreamId)
					{
						if (pFirstAudioPTime !=NULL_WMC)
							*pFirstAudioPTime = (U64_WMC) Payload.msObjectPres - (U64_WMC)pDecoder->tHeaderInfo.msPreroll;
						bFirstAudio = TRUE_WMC;
						break;
					}
				}
			}
			if (bFirstAudio == TRUE_WMC)
				break;

		}
	}
	


	
	return rc;
}

/****************************************************************************************************************/

tWMCDecStatus WMCDecGetPreviousFrameTime (HWMCDECODER hWMCDec, U16_WMC wStreamId, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn)
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

    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC || pu64TimeReturn == NULL_WMC)
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
	return rc;
}

/****************************************************************************************************************/

/****************************************************************************************************************/
tWMCDecStatus WMCDecSeekToNextI (HWMCDECODER hWMCDec, U16_WMC wStreamId)
{

	U64_WMC cbPacketOffset =0;
    U8_WMC i=0, j=0;
	tWMCDecStatus rc = WMCDec_Succeeded;
    U32_WMC dwToIndexEntry =0;
    WMCINDEXENTRIES* pTempIndexEntry = NULL_WMC;
    U32_WMC dwPacketToGo =0;
    U8_WMC bAudioVideo =0;
    U32_WMC cbVideoSize =0;
    U32_WMC dwVideoTimeStamp=0;
    U16_WMC cbAudioOut =0;
    U32_WMC dwAudioTimeStamp =0;
    U8_WMC* pucVidBufTemp = NULL_WMC;
    U32_WMC dwMinimumPacketToGo = 0xffffffff;
    tMediaType_WMC MediaType;
    U32_WMC nNumSamplesReturn =0;
    I64_WMC tPresentationTime =0;
    U8_WMC *pu8VideoOutBuffer = NULL_WMC;
    U32_WMC u32VideoOutBufferSize =0;
	U32_WMC u32OutDataSize =0;
	U64_WMC u64LocalTimeRequest=0;
	Bool_WMC bVideoThere = FALSE_WMC;
	U64_WMC u64MyTimeReturn =0;
	U32_WMC dwNewPacketToGo=0;
    I64_WMC tPrevPresentationTime =0;
	U32_WMC cbCurrentPacketOffset =0;
	PACKET_PARSE_INFO_EX ParseInfoEx;
	PAYLOAD_MAP_ENTRY_EX Payload;
	U32_WMC iPayload =0;
	Bool_WMC bDone = FALSE_WMC;


 
    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC)
        return WMCDec_InValidArguments;

    dwMinimumPacketToGo = (U32_WMC)pDecoder->tHeaderInfo.cbLastPacketOffset;

	rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) wStreamId, &MediaType);

    if (WMCDec_Succeeded !=rc)
        return rc;

	if (MediaType != Video_WMC)
		return WMCDec_InValidArguments;

	memset(&ParseInfoEx, 0, sizeof (PACKET_PARSE_INFO_EX));
	memset(&Payload, 0, sizeof (PAYLOAD_MAP_ENTRY_EX));



    for (i =0; i < pDecoder->tHeaderInfo.wNoOfVideoStreams; i++)
	{
		if (pDecoder->tVideoStreamInfo[i]->wStreamId == wStreamId)
		{
			tPrevPresentationTime = (I64_WMC)pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp;
			break;
		}
	}



    if (pDecoder->cTotalIndexs >0)
    {
		for(i=0; i< pDecoder->cTotalIndexs ; i++)
		{
			if (pDecoder->tIndexInfo[i].nStreamId == wStreamId)
			{
				rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) wStreamId, &MediaType);
				if (WMCDec_Succeeded !=rc)
					return rc;
				for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
				{
					if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
					{
						u64LocalTimeRequest = (U64_WMC)tPrevPresentationTime + (U64_WMC)pDecoder->tIndexInfo[i].time_deltaMs*999/1000;
						dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
						if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
							return WMCDec_InValidArguments;
						pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
						pTempIndexEntry+=dwToIndexEntry;
						dwPacketToGo = pTempIndexEntry->dwPacket;
						if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
							return WMCDec_InValidArguments;
						dwNewPacketToGo = dwPacketToGo;
						while ((dwNewPacketToGo == dwPacketToGo) && (dwToIndexEntry < pDecoder->tIndexInfo[i].num_entries -1))
						{
							dwToIndexEntry++;
							pTempIndexEntry++;
							dwNewPacketToGo = pTempIndexEntry->dwPacket;
							if (dwNewPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;

							if (dwNewPacketToGo > dwPacketToGo )
							{
								bDone = TRUE_WMC;
							}
						}

						if (bDone == FALSE_WMC)
							dwNewPacketToGo = pDecoder->tHeaderInfo.cPackets;
					}
    
				}
			}
		}

	}
	else
		return WMCDec_Fail;

	

/*

		rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);


		for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
		{
        
			rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
			if (rc != WMCDec_Succeeded)
				return rc;

			if (wStreamId == Payload.bStreamId)
			{
				
				if (Payload.msObjectPres > tPrevPresentationTime)
				{
					break;
				}
			}

		}


*/


    pDecoder->parse_state = csWMCNewAsfPacket;


    pDecoder->tHeaderInfo.cbCurrentPacketOffset = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)dwNewPacketToGo * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize);
    pDecoder->tHeaderInfo.cbNextPacketOffset  = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)(dwNewPacketToGo) * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize );


    for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
    {
#ifndef _ASFPARSE_ONLY_
        if ((pDecoder->tAudioStreamInfo[j]->bWantOutput == TRUE_WMC)
			&&(pDecoder->tAudioStreamInfo[j]->bIsDecodable == TRUE_WMC)
			&&(pDecoder->tAudioStreamInfo[j]->nVersion >0 && pDecoder->tAudioStreamInfo[j]->nVersion <4)
			&&(pDecoder->tAudioStreamInfo[j]->hMSA !=NULL_WMC) 
			)
        {
			WMARawDecReset (pDecoder->tAudioStreamInfo[j]->hMSA);
		}
#endif        
		pDecoder->tAudioStreamInfo[j]->dwAudioBufCurOffset = 0;
	    pDecoder->tAudioStreamInfo[j]->cbNbFramesAudBuf = 0;
        pDecoder->tAudioStreamInfo[j]->dwAudioBufDecoded = 0;
	    pDecoder->tAudioStreamInfo[j]->bBlockStart = 0;
	    pDecoder->tAudioStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tAudioStreamInfo[j]->dwPayloadLeft = 0;
	    pDecoder->tAudioStreamInfo[j]->dwAudPayloadPresTime = 0;
        pDecoder->tAudioStreamInfo[j]->dwAudioTimeStamp = 0.0;
        pDecoder->tAudioStreamInfo[j]->wmar = WMA_S_NO_MORE_FRAME;
		pDecoder->tAudioStreamInfo[j]->bFirstTime = FALSE_WMC;

#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tAudioStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tAudioStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tAudioStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tAudioStreamInfo[j]->parse_state = csWMCNewAsfPacket;

#endif
     }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
    {
        pDecoder->tVideoStreamInfo[j]->bFirst =  FALSE_WMC;
        pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset = 0;
	    pDecoder->tVideoStreamInfo[j]->bBlockStart = 0;
	    pDecoder->tVideoStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
	    pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf = 0;
        pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded = 0;
        pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
        pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
	    pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
        pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
	    pDecoder->tVideoStreamInfo[j]->cbFrame = 0;
		pDecoder->tVideoStreamInfo[j]->pStoreFrameStartPointer = NULL_WMC;
		pDecoder->tVideoStreamInfo[j]->pStoreNextFrameStartPointer = NULL_WMC;
		pDecoder->tVideoStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
		pDecoder->tVideoStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tVideoStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tVideoStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tVideoStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tVideoStreamInfo[j]->parse_state = csWMCNewAsfPacket;
        pDecoder->tVideoStreamInfo[j]->dwNextVideoTimeStamp = 0;

#endif
    }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
    {
        pDecoder->tBinaryStreamInfo[j]->dwBinaryBufCurOffset = 0;
        pDecoder->tBinaryStreamInfo[j]->dwBinaryTimeStamp = 0;
	    pDecoder->tBinaryStreamInfo[j]->cbNbFramesBinBuf = 0;
	    pDecoder->tBinaryStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tBinaryStreamInfo[j]->dwPayloadLeft = 0;
   	    pDecoder->tBinaryStreamInfo[j]->cbUsed = 0;
        pDecoder->tBinaryStreamInfo[j]->dwFrameSize = 0; 
	    pDecoder->tBinaryStreamInfo[j]->cbFrame = 0;
        pDecoder->tBinaryStreamInfo[j]->dwNbFrames = 0;
        pDecoder->tBinaryStreamInfo[j]->dwBinaryBufDecoded = 0;
		pDecoder->tBinaryStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
		pDecoder->tBinaryStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tBinaryStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->parse_state = csWMCNewAsfPacket;
        pDecoder->tBinaryStreamInfo[j]->dwNextBinaryTimeStamp = 0;

#endif
    }





/*	do
	{
		rc = WMCDecDecodeData (hWMCDec, &nStreamReadyForOutput, &nNumberOfSamples);
        
        if (nNumberOfSamples >0)
        {
            cbAudioOut = 0;
            for (i=0; i<pDecoder->tHeaderInfo.wNoOfStreams; i++ )
            {
                if (pDecoder->pStreamIdnMediaType[i].wStreamId == (U16_WMC)nStreamReadyForOutput)
                {
                
                    switch(pDecoder->pStreamIdnMediaType[i].MediaType)
                    {
                    case Audio_WMC:
                        for (j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams ; j++)
                        {
                             if ( pDecoder->tAudioStreamInfo[j]->wStreamId == (U16_WMC)nStreamReadyForOutput)
                             {
 			                    do
                                {
				                    cbAudioOut = 2048;
                                    rc = WMCDecGetAudioOutput (hWMCDec, (I16_WMC *)OutBuff, NULL_WMC, cbAudioOut, &nNumSamplesReturn, &tPresentationTime );
                                    if (nNumSamplesReturn > 0)
									{
										if ((tPresentationTime >= (I64_WMC)(pDecoder->u64SeekTimeRequest - 10 -pDecoder->tHeaderInfo.msPreroll)) && bVideoThere == FALSE)
										{
											pDecoder->bInSeekState = FALSE;
											*pu64TimeReturn = tPresentationTime;
											break;
										}
									
									}
                                }while(nNumSamplesReturn >0);

                                break;
                             }
                        }
                        break;

                    case Video_WMC:
                        for (j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
                        {
                             if ( pDecoder->tVideoStreamInfo[j]->wStreamId== (U16_WMC)nStreamReadyForOutput)
                             {
                             
        	                    do
                                {
                                     rc = WMCDecGetVideoOutput (hWMCDec, NULL_WMC , 0, &u32OutDataSize, &tPresentationTime, &bIsKeyFrame);
									if (tPresentationTime >= (I64_WMC)u64MyTimeReturn)
									{
										pDecoder->bInSeekState = FALSE;
										*pu64TimeReturn = tPresentationTime;
										break;
									}
                                }while(u32OutDataSize >0);
                                break;
                             }
                        }
                        break;
                    }
                    break;
                }
            }
        }
    }while ((rc == 0) && (pDecoder->bInSeekState == TRUE));


	if (pu8VideoOutBuffer != NULL_WMC)
		wmvfree(pu8VideoOutBuffer);
	pu8VideoOutBuffer = NULL_WMC;

	pDecoder->bInSeekState = FALSE;
*/
	return rc;
}


/****************************************************************************************************************/

tWMCDecStatus WMCDecSeekToPrevI (HWMCDECODER hWMCDec, U16_WMC wStreamId)
{

	U64_WMC cbPacketOffset =0;
    U8_WMC i=0, j=0;
	tWMCDecStatus rc = WMCDec_Succeeded;
    U32_WMC dwToIndexEntry =0;
    WMCINDEXENTRIES* pTempIndexEntry = NULL_WMC;
    U32_WMC dwPacketToGo =0;
    U8_WMC bAudioVideo =0;
    U32_WMC cbVideoSize =0;
    U32_WMC dwVideoTimeStamp=0;
    U16_WMC cbAudioOut =0;
    U32_WMC dwAudioTimeStamp =0;
    U8_WMC* pucVidBufTemp = NULL_WMC;
    U32_WMC dwMinimumPacketToGo = 0xffffffff;
    tMediaType_WMC MediaType;
    U32_WMC nNumSamplesReturn =0;
    I64_WMC tPresentationTime =0;
    U8_WMC *pu8VideoOutBuffer = NULL_WMC;
    U32_WMC u32VideoOutBufferSize =0;
	U32_WMC u32OutDataSize =0;
	U64_WMC u64LocalTimeRequest=0;
	Bool_WMC bVideoThere = FALSE_WMC;
	U64_WMC u64MyTimeReturn =0;
	U32_WMC dwNewPacketToGo=0;
    I64_WMC tPrevPresentationTime =0;
	U32_WMC cbCurrentPacketOffset =0;
	PACKET_PARSE_INFO_EX ParseInfoEx;
	PAYLOAD_MAP_ENTRY_EX Payload;
	U32_WMC iPayload =0;
	Bool_WMC bDone = FALSE_WMC;



 
    WMFDecoderEx* pDecoder = (WMFDecoderEx*) hWMCDec;
    if (pDecoder == NULL_WMC)
        return WMCDec_InValidArguments;

    dwMinimumPacketToGo = (U32_WMC)pDecoder->tHeaderInfo.cbLastPacketOffset;


	memset(&ParseInfoEx, 0, sizeof (PACKET_PARSE_INFO_EX));
	memset(&Payload, 0, sizeof (PAYLOAD_MAP_ENTRY_EX));

    rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) wStreamId, &MediaType);

    if (WMCDec_Succeeded !=rc)
        return rc;

	if (MediaType != Video_WMC)
		return WMCDec_InValidArguments;

    for (i =0; i < pDecoder->tHeaderInfo.wNoOfVideoStreams; i++)
	{
		if (pDecoder->tVideoStreamInfo[i]->wStreamId == wStreamId)
		{
			tPrevPresentationTime = (I64_WMC)pDecoder->tVideoStreamInfo[i]->dwVideoTimeStamp;
		}
	}

    if (pDecoder->cTotalIndexs >0)
    {

		for(i=0; i< pDecoder->cTotalIndexs ; i++)
		{
			if (pDecoder->tIndexInfo[i].nStreamId == wStreamId)
			{
				rc = WMCDecGetMediaType (hWMCDec, (U16_WMC) wStreamId, &MediaType);
				if (WMCDec_Succeeded !=rc)
					return rc;
				for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
				{
					if(pDecoder->tIndexInfo[i].nStreamId == wStreamId)
					{
						u64LocalTimeRequest = tPrevPresentationTime;
						dwToIndexEntry = (U32_WMC)(u64LocalTimeRequest/pDecoder->tIndexInfo[i].time_deltaMs);
						if (dwToIndexEntry > pDecoder->tIndexInfo[i].num_entries )
							return WMCDec_InValidArguments;
						pTempIndexEntry = pDecoder->tIndexInfo[i].pIndexEntries;
						pTempIndexEntry+=dwToIndexEntry;
						dwPacketToGo = pTempIndexEntry->dwPacket;
						if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
							return WMCDec_InValidArguments;
						dwNewPacketToGo = dwPacketToGo;


						cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbHeader  + (dwNewPacketToGo * pDecoder->tHeaderInfo.cbPacketSize);

						rc = WMCDecParseVirtualPacketHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx);


						for (iPayload = 0; iPayload < ParseInfoEx.cPayloads; iPayload++)
						{
        
							rc = WMCDecParseVirtualPayloadHeader(hWMCDec, cbCurrentPacketOffset, &ParseInfoEx, &Payload);
							if (rc != WMCDec_Succeeded)
								return rc;

							if (wStreamId == Payload.bStreamId)
							{
								
								if ((Payload.msObjectPres < tPrevPresentationTime)&&(Payload.bIsKeyFrame))
								{
									bDone = TRUE_WMC;
									break;
								}
							}

						}

					    if ((bDone == FALSE_WMC) && (dwToIndexEntry >=2))
						{
							pTempIndexEntry-=2;
							dwPacketToGo = pTempIndexEntry->dwPacket;
							if (dwPacketToGo > pDecoder->tHeaderInfo.cPackets)
								return WMCDec_InValidArguments;
							dwNewPacketToGo = dwPacketToGo;
						}

					

					}
    
				}
			
			}
		}
	}
	else
		return WMCDec_Fail;

    pDecoder->parse_state = csWMCNewAsfPacket;

    pDecoder->tHeaderInfo.cbCurrentPacketOffset = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)dwNewPacketToGo * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize);
    pDecoder->tHeaderInfo.cbNextPacketOffset  = (U64_WMC)pDecoder->tHeaderInfo.cbHeader  + ( (U64_WMC)(dwNewPacketToGo) * (U64_WMC)pDecoder->tHeaderInfo.cbPacketSize );



    for(j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams; j++)
    {
#ifndef _ASFPARSE_ONLY_
        if ((pDecoder->tAudioStreamInfo[j]->bWantOutput == TRUE_WMC)
			&&(pDecoder->tAudioStreamInfo[j]->bIsDecodable == TRUE_WMC)
			&&(pDecoder->tAudioStreamInfo[j]->nVersion >0 && pDecoder->tAudioStreamInfo[j]->nVersion <4)
			&&(pDecoder->tAudioStreamInfo[j]->hMSA !=NULL_WMC) 
			)
        {
			WMARawDecReset (pDecoder->tAudioStreamInfo[j]->hMSA);
		}
#endif        
		pDecoder->tAudioStreamInfo[j]->dwAudioBufCurOffset = 0;
	    pDecoder->tAudioStreamInfo[j]->cbNbFramesAudBuf = 0;
        pDecoder->tAudioStreamInfo[j]->dwAudioBufDecoded = 0;
	    pDecoder->tAudioStreamInfo[j]->bBlockStart = 0;
	    pDecoder->tAudioStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tAudioStreamInfo[j]->dwPayloadLeft = 0;
	    pDecoder->tAudioStreamInfo[j]->dwAudPayloadPresTime = 0;
        pDecoder->tAudioStreamInfo[j]->dwAudioTimeStamp = 0.0;
        pDecoder->tAudioStreamInfo[j]->wmar = WMA_S_NO_MORE_FRAME;
		pDecoder->tAudioStreamInfo[j]->bFirstTime = FALSE_WMC;

#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tAudioStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tAudioStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tAudioStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tAudioStreamInfo[j]->parse_state = csWMCNewAsfPacket;

#endif

     }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
    {
        pDecoder->tVideoStreamInfo[j]->bFirst =  FALSE_WMC;
        pDecoder->tVideoStreamInfo[j]->dwVideoBufCurOffset = 0;
	    pDecoder->tVideoStreamInfo[j]->bBlockStart = 0;
	    pDecoder->tVideoStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tVideoStreamInfo[j]->dwPayloadLeft = 0;
	    pDecoder->tVideoStreamInfo[j]->cbNbFramesVidBuf = 0;
        pDecoder->tVideoStreamInfo[j]->dwVideoBufDecoded = 0;
        pDecoder->tVideoStreamInfo[j]->dwVideoTimeStamp = 0;
        pDecoder->tVideoStreamInfo[j]->dwNbFrames = 0;
	    pDecoder->tVideoStreamInfo[j]->cbUsed = 0;
        pDecoder->tVideoStreamInfo[j]->dwFrameSize = 0; 
	    pDecoder->tVideoStreamInfo[j]->cbFrame = 0;
		pDecoder->tVideoStreamInfo[j]->pStoreFrameStartPointer = NULL_WMC;
		pDecoder->tVideoStreamInfo[j]->pStoreNextFrameStartPointer = NULL_WMC;
		pDecoder->tVideoStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
		pDecoder->tVideoStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tVideoStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tVideoStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tVideoStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tVideoStreamInfo[j]->parse_state = csWMCNewAsfPacket;
        pDecoder->tVideoStreamInfo[j]->dwNextVideoTimeStamp = 0;

#endif
    }

    for(j=0; j<pDecoder->tHeaderInfo.wNoOfBinaryStreams; j++)
    {
        pDecoder->tBinaryStreamInfo[j]->dwBinaryBufCurOffset = 0;
        pDecoder->tBinaryStreamInfo[j]->dwBinaryTimeStamp = 0;
	    pDecoder->tBinaryStreamInfo[j]->cbNbFramesBinBuf = 0;
	    pDecoder->tBinaryStreamInfo[j]->dwBlockLeft = 0;
	    pDecoder->tBinaryStreamInfo[j]->dwPayloadLeft = 0;
   	    pDecoder->tBinaryStreamInfo[j]->cbUsed = 0;
        pDecoder->tBinaryStreamInfo[j]->dwFrameSize = 0; 
	    pDecoder->tBinaryStreamInfo[j]->cbFrame = 0;
        pDecoder->tBinaryStreamInfo[j]->dwNbFrames = 0;
        pDecoder->tBinaryStreamInfo[j]->dwBinaryBufDecoded = 0;
		pDecoder->tBinaryStreamInfo[j]->bHasGivenAnyOutput = FALSE_WMC;
		pDecoder->tBinaryStreamInfo[j]->bNowStopReadingAndDecoding = FALSE_WMC;
#ifdef WMC_NO_BUFFER_MODE
		pDecoder->tBinaryStreamInfo[j]->cbPacketOffset = pDecoder->tHeaderInfo.cbPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->cbCurrentPacketOffset = pDecoder->tHeaderInfo.cbFirstPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->cbNextPacketOffset = pDecoder->tHeaderInfo.cbNextPacketOffset;
		pDecoder->tBinaryStreamInfo[j]->parse_state = csWMCNewAsfPacket;
        pDecoder->tBinaryStreamInfo[j]->dwNextBinaryTimeStamp = 0;

#endif
    }





/*	do
	{
		rc = WMCDecDecodeData (hWMCDec, &nStreamReadyForOutput, &nNumberOfSamples);
        
        if (nNumberOfSamples >0)
        {
            cbAudioOut = 0;
            for (i=0; i<pDecoder->tHeaderInfo.wNoOfStreams; i++ )
            {
                if (pDecoder->pStreamIdnMediaType[i].wStreamId == (U16_WMC)nStreamReadyForOutput)
                {
                
                    switch(pDecoder->pStreamIdnMediaType[i].MediaType)
                    {
                    case Audio_WMC:
                        for (j=0; j<pDecoder->tHeaderInfo.wNoOfAudioStreams ; j++)
                        {
                             if ( pDecoder->tAudioStreamInfo[j]->wStreamId == (U16_WMC)nStreamReadyForOutput)
                             {
 			                    do
                                {
				                    cbAudioOut = 2048;
                                    rc = WMCDecGetAudioOutput (hWMCDec, (I16_WMC *)OutBuff, NULL_WMC, cbAudioOut, &nNumSamplesReturn, &tPresentationTime );
                                    if (nNumSamplesReturn > 0)
									{
										if ((tPresentationTime >= (I64_WMC)(pDecoder->u64SeekTimeRequest - 10 -pDecoder->tHeaderInfo.msPreroll)) && bVideoThere == FALSE)
										{
											pDecoder->bInSeekState = FALSE;
											*pu64TimeReturn = tPresentationTime;
											break;
										}
									
									}
                                }while(nNumSamplesReturn >0);

                                break;
                             }
                        }
                        break;

                    case Video_WMC:
                        for (j=0; j<pDecoder->tHeaderInfo.wNoOfVideoStreams; j++)
                        {
                             if ( pDecoder->tVideoStreamInfo[j]->wStreamId== (U16_WMC)nStreamReadyForOutput)
                             {
                             
        	                    do
                                {
                                     rc = WMCDecGetVideoOutput (hWMCDec, NULL_WMC , 0, &u32OutDataSize, &tPresentationTime, &bIsKeyFrame);
									if (tPresentationTime >= (I64_WMC)u64MyTimeReturn)
									{
										pDecoder->bInSeekState = FALSE;
										*pu64TimeReturn = tPresentationTime;
										break;
									}
                                }while(u32OutDataSize >0);
                                break;
                             }
                        }
                        break;
                    }
                    break;
                }
            }
        }
    }while ((rc == 0) && (pDecoder->bInSeekState == TRUE));


    wmvfree(pu8VideoOutBuffer);
	pu8VideoOutBuffer = NULL_WMC;

	pDecoder->bInSeekState = FALSE;
*/
	return rc;
}


/****************************************************************************************************************/


