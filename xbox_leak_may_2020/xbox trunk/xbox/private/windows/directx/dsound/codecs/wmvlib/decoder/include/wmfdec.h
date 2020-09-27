//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wmadec.h
//
//--------------------------------------------------------------------------

#ifndef WMADEC_H
#define WMADEC_H

#if defined(_XBOX)
#include <xtl.h>
#endif

#if !defined(_Embedded_x86) && !defined(HITACHI)
#ifdef macintosh
#include <MacTypes.h>
#include <Errors.h>
#include <Endian.h>
#include <MacMemory.h>
#define BIGENDIAN
#else
#include <windows.h>
#include <memory.h>
#include <mmsystem.h>
#endif 
#endif

#include "wmftypes.h"

//#include "wmadec_api.h"
#include "wmadecS_api.h"
#ifndef WMFAPI_NO_DRM
#ifdef _ZMDRM_
#include "drmmanager_api.h"
#else
#include "drmbase.h"
#include "drmpd.h"
#endif
#endif
#include "wmc_type.h"
#include "wmcdecode.h"
#include "wmcguids.h"
typedef struct tPacketInfo
{
	U32_WMC	nSamplesPerSec;
	U64_WMC	cbPacketOffset;	
	U64_WMC	cbLastPacketOffset;	
	U32_WMC	cbPacketSize;	
	U32_WMC	cbMaxAudioSize;
	U16_WMC	nChannels;	
	U32_WMC	fccHandler;
	U32_WMC	dwScale;
	U32_WMC	dwRate;
	BITMAPINFOHEADER BitmapInfo;
	WAVEFORMATEX		tWaveFormat;	
	U8_WMC	bHasValidVideo;
	U8_WMC	bHasValidAudio;
	U8_WMC	bInit;

} WMINITINFO;

typedef enum tagWMCParseState
{
    csWMC_NotValid = 0,

    csWMCHeaderStart,
    csWMCHeaderError,

    csWMCNewAsfPacket,

    /* packet parsing states */

    csWMCDecodePayloadStart,
    csWMCDecodePayload,
    csWMCDecodePayloadHeader,
    csWMCDecodeLoopStart,
    csWMCDecodePayloadEnd,
    csWMCDecodeCompressedPayload,

    csWMCEnd
} tWMCParseState;



typedef struct tBitRateInfo
{
    U16_WMC   wStreamId;
    U32_WMC   dwBitRate;

} BITRATEINFO;


typedef struct tHeaderInfo
{
	U64_WMC	    cbPacketOffset;	
	U64_WMC	    cbLastPacketOffset;	
	U64_WMC	    cbCurrentPacketOffset;	
	U64_WMC	    cbFirstPacketOffset;	
	U64_WMC	    cbNextPacketOffset;	

    U32_WMC     cbHeader;
    U32_WMC     cbPacketSize;
    U32_WMC     cPackets;
    U32_WMC     msDuration;
    U32_WMC     msPreroll;
    U32_WMC     msSendDuration;
	U32_WMC     dwMaxBitrate;
    U16_WMC     wNoOfStreams;
    U16_WMC     wNoOfAudioStreams;
    U16_WMC     wNoOfVideoStreams;
    U16_WMC     wNoOfBinaryStreams;
    
    BITRATEINFO tBitRateInfo[127];
    PACKET_PARSE_INFO_EX ppex;

    PAYLOAD_MAP_ENTRY_EX payload;
    U32_WMC     iPayload;
    U16_WMC     wPayStart;
    Bool_WMC    bHasBitRateInfo;
    U32_WMC     dwPayloadOffset;
    

} HEADERINFO;


typedef struct tAudioStreamInfo
{
    U16_WMC    nVersion;
    U16_WMC    wFormatTag;
    U32_WMC    nSamplesPerSec;
    U32_WMC    nAvgBytesPerSec;
    U32_WMC    nBlockAlign;
    U16_WMC    nChannels;


    U32_WMC    nSamplesPerBlock;
    U16_WMC    nEncodeOpt;

    I32_WMC	   nBitsPerSample;
    U16_WMC    wValidBitsPerSample; // actual valid data depth in the decoded stream
    U32_WMC    dwChannelMask;
    GUID_WMC   SubFormat;
    U16_WMC    wOriginalBitDepth; // actual valid data depth in the original source (informational)
    U16_WMC    wStreamId;
    U32_WMC    cbAudioSize;
    WMARawDecHandle     hMSA;
    Bool_WMC   bTobeDecoded;
    Bool_WMC   bIsDecodable;
    Bool_WMC   bWantOutput;
    WMARESULT  wmar;
    Bool_WMC   bTimeToChange;
    Bool_WMC   bTimeToChangex;
    Bool_WMC   bFirstTime;
    U8_WMC     bAudioBuffer[MIN_WANTED];
	U8_WMC     *pbAudioBuffer;
    U32_WMC    dwFrameSize; 
    U32_WMC    dwAudioBufSize;
    U32_WMC    dwAudioBufCurOffset;
	U32_WMC    cbNbFramesAudBuf;
    U32_WMC    dwAudioBufDecoded;
    U32_WMC    dwAudioPayloadSize;
	U8_WMC     bBlockStart;
	U32_WMC    dwBlockLeft;
	U32_WMC    dwPayloadLeft;
	U32_WMC    dwAudPayloadPresTime;
    Double_WMC dwAudioTimeStamp;
	U8_WMC*	   pStoreFrameStartPointer;			
	U8_WMC*	   pStoreNextFrameStartPointer;			
	U8_WMC*	   pDecodeFrameStartPointer;			
	U8_WMC*	   pDecodeNextFrameStartPointer;
    Bool_WMC   bBufferIncreased;
    Bool_WMC   bStopReading;
	Bool_WMC   bGotCompOutput;	
	Bool_WMC   bOutputisReady;	
//  for non buffer mode
#ifdef WMC_NO_BUFFER_MODE
	U64_WMC	    cbPacketOffset;	
	U64_WMC	    cbCurrentPacketOffset;	
	U64_WMC	    cbNextPacketOffset;	
    PACKET_PARSE_INFO_EX ppex;
    PAYLOAD_MAP_ENTRY_EX payload;
    U32_WMC     iPayload;
    U16_WMC     wPayStart;
    U32_WMC     dwPayloadOffset;
    tWMCParseState         parse_state;
    Bool_WMC   bPayloadGiven;
#endif
	
} AUDIOSTREAMINFO;

typedef struct tVideoStreamInfo
{
	U32_WMC    windowWidth;
    U32_WMC    windowHeight;
    U8_WMC     flags;
    U16_WMC    imageInfoLen;

	U32_WMC    biSize; 
	I32_WMC    biWidth; 
	I32_WMC    biHeight; 
	U16_WMC    biPlanes; 
	U16_WMC    biBitCount; 
	U32_WMC    biCompression; 
	U32_WMC    biSizeImage; 
	I32_WMC    biXPelsPerMeter; 
	I32_WMC    biYPelsPerMeter; 
	U32_WMC    biClrUsed; 
	U32_WMC    biClrImportant;

    U16_WMC    biBitCountOut; 
	U32_WMC    biCompressionOut; 
	U32_WMC    biSizeImageOut; 

	void	   *pVDec;
    U16_WMC    wStreamId;
    Bool_WMC   bTobeDecoded;
    Bool_WMC   bIsDecodable;
    Bool_WMC   bWantOutput;
    Bool_WMC   bFirst;
	U8_WMC     *pbVideoBuffer;
    U32_WMC    dwVideoBufSize;
    U32_WMC    dwVideoBufCurOffset;
	U8_WMC     bBlockStart;
	U32_WMC    dwBlockLeft;
	U32_WMC    dwPayloadLeft;
	U32_WMC    dwFrameLeft;
	U32_WMC    cbNbFramesVidBuf;
    U32_WMC    dwVideoBufDecoded;
//	U8_WMC	   *pVideoBuffer;
    U32_WMC    dwVideoTimeStamp;
    U32_WMC    dwNbFrames;
	U8_WMC     bAdditionalInfo[1024];
    U8_WMC     bPalette[3*256];
    U8_WMC     bVideoBuffer[MIN_WANTED+10];
    Bool_WMC   bInitStage; 
	U32_WMC    cbUsed;
    U32_WMC    dwFrameSize; 
	U32_WMC    cbFrame;
    Bool_WMC   bIsKeyFrame;
    U8_WMC*    pSCInBuffer;
    U8_WMC*    pSCOutBuffer;
    Bool_WMC   bScOutGiven; 
	U8_WMC*	   pStoreFrameStartPointer;			
	U8_WMC*	   pStoreNextFrameStartPointer;			
	U8_WMC*	   pDecodeFrameStartPointer;			
	U8_WMC*	   pDecodeNextFrameStartPointer;			
    Bool_WMC   bBufferIncreased;
    Bool_WMC   bStopReading;
	Bool_WMC   bGotCompOutput;	
	Bool_WMC   bOutputisReady;
	Bool_WMC   bHasGivenAnyOutput;
	Bool_WMC   bNowStopReadingAndDecoding;

	tVideoFormat_WMC tOutVideoFormat;

#ifdef WMC_NO_BUFFER_MODE
	U64_WMC	    cbPacketOffset;	
	U64_WMC	    cbCurrentPacketOffset;	
	U64_WMC	    cbNextPacketOffset;	
    PACKET_PARSE_INFO_EX ppex;
    PAYLOAD_MAP_ENTRY_EX payload;
    U32_WMC     iPayload;
    U16_WMC     wPayStart;
    U32_WMC     dwPayloadOffset;
    tWMCParseState         parse_state;
    U32_WMC    dwNextVideoTimeStamp;
    Bool_WMC   bFrameGiven;
#endif    

} VIDEOSTREAMINFO;


typedef struct tBinaryStreamInfo
{
//    Bool_WMC   bTobeDecoded;
    U16_WMC    wStreamId;
    Bool_WMC   bWantOutput;
    U32_WMC    dwBinaryBufSize;
	U8_WMC     *pbBinaryBuffer;
    U32_WMC    dwBinaryBufCurOffset;
    U32_WMC    dwBinaryTimeStamp;
	U32_WMC    cbNbFramesBinBuf;
	U32_WMC    dwBlockLeft;
	U32_WMC    dwPayloadLeft;
	U32_WMC    dwFrameLeft;
   	U32_WMC    cbUsed;
    U32_WMC    dwFrameSize; 
	U32_WMC    cbFrame;
    U32_WMC    dwNbFrames;
    U32_WMC    dwBinaryBufDecoded;
	U8_WMC*	   pStoreFrameStartPointer;			
	U8_WMC*	   pStoreNextFrameStartPointer;			
	U8_WMC*	   pDecodeFrameStartPointer;			
	U8_WMC*	   pDecodeNextFrameStartPointer;			
    Bool_WMC   bBufferIncreased;
    Bool_WMC   bStopReading;
	Bool_WMC   bGotCompOutput;	
	Bool_WMC   bOutputisReady;	

	Bool_WMC   bHasGivenAnyOutput;
	Bool_WMC   bNowStopReadingAndDecoding;

#ifdef WMC_NO_BUFFER_MODE
	U64_WMC	    cbPacketOffset;	
	U64_WMC	    cbCurrentPacketOffset;	
	U64_WMC	    cbNextPacketOffset;	
    PACKET_PARSE_INFO_EX ppex;
    PAYLOAD_MAP_ENTRY_EX payload;
    U32_WMC     iPayload;
    U16_WMC     wPayStart;
    U32_WMC     dwPayloadOffset;
    tWMCParseState         parse_state;
    U32_WMC    dwNextBinaryTimeStamp;
    Bool_WMC   bFrameGiven;
#endif    

    
} BINARYSTREAMINFO;

typedef struct tWMFDecoderEx
{
    HEADERINFO            tHeaderInfo;
    AUDIOSTREAMINFO *     tAudioStreamInfo[127];
    VIDEOSTREAMINFO *     tVideoStreamInfo[127];
    BINARYSTREAMINFO *    tBinaryStreamInfo[127];

#ifndef WMFAPI_NO_DRM
#ifdef _ZMDRM_
    HDRMMGR hDRM;
#else
    CDrmPD *hDRM;
#endif
#endif

    U8_WMC*                pszType;
    U8_WMC*                pszKeyID;
 
    U32_WMC                cbEnhData;
    U8_WMC*                pbEnhData;
    U32_WMC                cbSecData;
    U8_WMC*                pbSecData;
    tWMCParseState         parse_state;
    /* Marker */
    U32_WMC                m_dwMarkerNum;
    WMCContentDescription  *m_pContentDesc;
    WMCMarkerEntry            *m_pMarkers;
    WMCExtendedContentDesc *m_pExtendedContentDesc;
    WMCScriptCommand       *m_pScriptCommand;
        /* License Store */
    U32_WMC                m_dwLicenseLen;
    U8_WMC                 *m_pLicData;
    U8_WMC                 bHasDRM;
    // Adding for index entry
    U8_WMC                 cTotalIndexs;
    WMCINDEXINFO           tIndexInfo[127];
    Bool_WMC               bParsed;
    I32_WMC                i32CurrentAudioStreamIndex;
    I32_WMC                i32CurrentVideoStreamIndex;
    I32_WMC                i32CurrentBinaryStreamIndex;
    tPlannedOutputWMC      tPlannedOutputInfo;
    tStreamIdnMediaType_WMC *pStreamIdnMediaType;
    U32_WMC                 u32UserData;
    Bool_WMC               bSkipToNextI;
	U32_WMC				   m_dwNumCodec;
	WMCCodecEntry		   *m_pCodecEntry;
	U64_WMC				   u64SeekTimeRequest;
	Bool_WMC			   bInSeekState;
	tWMCDecodeDispRotateDegree tDispRotateDegree;
	I32_WMC					i32PostProc;
	I32_WMC					iHurryUp;
	Bool_WMC				bIsNoBufferMode;
	WMCMetaDataEntry        tMetaDataEntry;
}WMFDecoderEx;





#endif
