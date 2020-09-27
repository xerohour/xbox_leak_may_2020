/*************************************************************************

Copyright (C) Microsoft Corporation, 2001

Module Name:

    wmcdecode.h

Abstract:

    API's for WMC SDK Decoder.

Revision History:

*************************************************************************/

#ifndef _WMC_DECODE_H_
#define _WMC_DECODE_H_

#include "wmc_type.h"

#if defined (macintosh) || defined (HITACHI)
#include "wmMacSp.h"
#endif 

#ifdef __cplusplus
extern "C" {
#endif 


#define CHECK_NONSDMI_LIC	1
#define CHECK_SDMI_LIC		2
#define CHECK_ALL_LICENSE	3
#define WMC_LICENCE_DATA_REQUESTED  172


typedef struct tagWMCDecFileLicParams
{
    U8_WMC *pPMID;       /* portable media id */
    U32_WMC cbPMID;      /* length of the pPMID buffer */

} tWMCDecFileLicParams;

typedef enum tWMCDecStatus
{
    WMCDec_Succeeded = 0,
    WMCDec_BadAsfHeader,
    WMCDec_BadPacketHeader,
    WMCDec_BadMemory,
    WMCDec_InValidArguments,
    WMCDec_InproperOutputType,
	WMCDec_UnSupportedCompressedFormats,
	WMCDec_NoOutput,

    WMADec_Failed,
    WMADec_BadMemory,
    WMADec_UnSupportedCompressedFormat,
    WMADec_InValidArguments,
    WMADec_BadSource,
    WMADec_BrokenFrame,
    WMADec_NoMoreOutput,

    WMVDec_Failed,
    WMVDec_BadMemory,
    WMVDec_UnSupportedCompressedFormat,
    WMVDec_UnSupportedOutputFormat,
    WMVDec_InValidArguments,
    WMVDec_BadSource,
    WMVDec_BrokenFrame,
    WMVDec_NoMoreOutput,
    WMVDec_NoKeyFrameDecoded,
    WMVDec_CorruptedBits,

    WMBDec_Failed,
    WMBDec_BadMemory,
    
    WMCDec_DRMFail,
    WMCDec_DRMInitFail,
    WMCDec_Fail,
    WMCDec_BufferTooSmall,
    WMCDec_BadData,
    WMCDec_InvalidStreamType,
    WMCDec_DecodeComplete,
    WMCDec_InvalidIndex

} tWMCDecStatus;

typedef void * HWMCDECODER;
typedef void * HDRMMGR;

/*
 *        WMCDecCreate
 *        =============
 *
 * Description
 * -----------
 *   This function Create the decoder object. Must be called before calling any function.
 * 
 *
 * Syntax
 * ------
 *   HWMCDECODER WMCDecCreate (U32_WMC u32UserContext);
 *
 *
 * where:
 * 
 * u32UserContext   Anydata need to pass to WMCDecCBGetData
 *
 * Return Value
 * ------------
 *   WMC decoder object, NULL if there is an error.            
 *
 */
HWMCDECODER WMCDecCreate (U32_WMC u32UserContext);

/*
 *        WMCDecInit
 *        =============
 *
 * Description
 * -----------
 *   This function initializes the decoder object. Must be called before calling any function.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecInit (HWMCDECODER* phWMCDec, tVideoFormat_WMC tVideoOutputFormat, Bool_WMC bDiscardIndex,I32_WMC iPostProcLevel);
 *
 *
 * where:
 *
 *   phWMCDec		        Pointer to the decoder object HWMCDECODER..
 *   tVideoOutputFormat	    Output Video Format defined in wmc_types.h
 *   bDiscardIndex          If TRUE then Index will not be read. It should be true if you cannot seek to the end of file
 *							to read Index ( One case is when you are streaming from network ). If it is true, SEEK may 
 *							not work precisely.
 *   iPostProcLevel         Post proc level for Video
 *							-1 : Let decoder decide post processing
 *							0: No post processing
 *							1 -4 : different levels of post processing
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.          
 *
 */
tWMCDecStatus WMCDecInit (HWMCDECODER* phWMCDec, tVideoFormat_WMC tVideoOutputFormat, Bool_WMC bDiscardIndex,I32_WMC iPostProcLevel);

/*
 *        WMCDecGetHeaderInfo
 *        =============
 *
 * Description
 * -----------
 *   This function gives the header information of asf file.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetHeaderInfo (HWMCDECODER hWMCDec, strHeaderInfo_WMC* pstrHeaderInfo);
 *
 *
 * where:
 *
 *   phWMCDec		        Decoder object HWMCDECODER..
 *   pstrHeaderInfo			Pointer to a structure for getting header info. It is defined in wmc_types.h
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.          
 *
 */


tWMCDecStatus WMCDecGetHeaderInfo (HWMCDECODER hWMCDec, strHeaderInfo_WMC* pstrHeaderInfo);

/*
 *        WMCDecGetStreamInfo
 *        ======================
 *
 * Description
 * -----------
 *   Get the audio and video information of the ASF stream/file.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetStreamInfo (HWMCDECODER hWMCDec, U32_WMC* pnNumStreams, tStreamIdnMediaType_WMC** pptMediaTypeStreams);
 *
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   pnNumStreams	            Pointer to an indtger. The decoder fills in this value with number of streams.
 *   pptMediaTypeStreams        Pointer to the array pointer. The decoder return an array of tStreamIdnMediaType_WMC.
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.          
 *
 */
tWMCDecStatus WMCDecGetStreamInfo (HWMCDECODER hWMCDec, U32_WMC* pnNumStreams, tStreamIdnMediaType_WMC** pptMediaTypeStreams);

/*
 *        WMCDecSetDecodePatternForStreams
 *        ================================
 *
 * Description
 * -----------
 *   Sets the Decoding pattern for all multimedia streams in the file. Each stream can be set for Decompressed_WMC or 
 *   Compressed_WMC or Discard_WMC. Must be called before start decoding. If the codec is not supported, and application asked
 *   for Decompressed_WMC, it will not give any output.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecSetDecodePatternForStreams (HWMCDECODER* phWMCDec, const tStreamIdPattern_WMC* ptStreamDecodePattern);
 *
 *
 * where:
 *
 *   hWMCDec		            Pointer to the decoder object HWMCDECODER.
 *   ptStreamDecodePattern      Pointer to the array of tStreamIdPattern_WMC structure which contains pattern for each stream.
 *                              The structure tStreamIdPattern_WMC is defined in wmc_types.h.
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.          
 *
 */
tWMCDecStatus WMCDecSetDecodePatternForStreams (HWMCDECODER* phWMCDec, const tStreamIdPattern_WMC* ptStreamDecodePattern);

/*
 *        WMCDecGetAudioInfo
 *        =============
 *
 * Description
 * -----------
 *   This function gives the Audio Stream information.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetAudioInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strAudioInfo_WMC* pstrAudioInfo);
 *
 *
 * where:
 *
 *   phWMCDec		        Decoder object HWMCDECODER.
 *   nStreamNum				Stream Id
 *   pstrAudioInfo			Pointer to a structure for getting Audio info. It is defined in wmc_types.h
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.WMCDec_InValidArguments if nStreamNum is not Audio          
 *
 */

tWMCDecStatus WMCDecGetAudioInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strAudioInfo_WMC* pstrAudioInfo);

/*
 *        WMCDecGetVideoInfo
 *        =============
 *
 * Description
 * -----------
 *   This function gives the Video Stream information.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetVideoInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strVideoInfo_WMC* pstrVideoInfo);
 *
 *
 * where:
 *
 *   phWMCDec		        Decoder object HWMCDECODER.
 *   nStreamNum				Stream Id
 *   pstrVideoInfo			Pointer to a structure for getting Video info. It is defined in wmc_types.h
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.WMCDec_InValidArguments if nStreamNum is not Video          
 *
 */

tWMCDecStatus WMCDecGetVideoInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strVideoInfo_WMC* pstrVideoInfo);

/*
 *        WMCDecGetBinaryInfo
 *        =============
 *
 * Description
 * -----------
 *   This function gives the Binary Stream information.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetBinaryInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strBinaryInfo_WMC* pstrBinaryInfo);
 *
 *
 * where:
 *
 *   phWMCDec		        Decoder object HWMCDECODER.
 *   nStreamNum				Stream Id
 *   pstrBinaryInfo			Pointer to a structure for getting Binary info. It is defined in wmc_types.h
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.WMCDec_InValidArguments if nStreamNum is not Binary          
 *
 */


tWMCDecStatus WMCDecGetBinaryInfo (HWMCDECODER hWMCDec, U32_WMC nStreamNum, strBinaryInfo_WMC* pstrBinaryInfo);


/*
 *        WMCDecSetVideoOutputFormat
 *        ==========================
 *
 * Description
 * -----------
 *   This function sets the output format for all video streams. Must be set before decoding.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecSetVideoOutputFormat (HWMCDECODER hWMCDec, tVideoFormat_WMC tVideoOutputFormat);
 *
 *
 * where:
 *
 *   phWMCDec		        Decoder object HWMCDECODER.
 *   tVideoOutputFormat		Output Format.tVideoFormat_WMC is defined in wmc_types.h and 
 *							contains the valid uncompressed formats.
 *
 * Return Value
 * ------------
 *   Error code. WMCDec_Succeeded if succeeds.WMVDec_UnSupportedOutputFormat if output format is not supported          
 *
 */

tWMCDecStatus WMCDecSetVideoOutputFormat (HWMCDECODER hWMCDec, tVideoFormat_WMC tVideoOutputFormat);


/*
 *        WMCDecDecodeData
 *        =========
 *
 * Description
 * -----------
 * This function decodes the packets of the input file.
 * Parses the packet and calls Audio/Video decoders (depending on packet info). 
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecDecodeData (HWMCDECODER hWMCDec, U32_WMC* pnStreamReadyForOutput, U32_WMC* pnNumberOfSamples, I32_WMC iHurryUp);
 *
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   pnStreamReadyForOutput
 *   pnNumberOfSamples          the number of samples decoded for this call for pnStreamReadyForOutput.
 *                              For Video, it is in number of frames.
 *                              For Audio, it is in number of samples.
 *                              For binary stream or compressed output, it is in number of bytes.
 *
 *	iHurryUp					It is used for Video. Value is from -2 to +2. -2 is slower, 0 is no hurry up and +2 is faster.
 * Return Value
 * ------------
 *   WMCDec_Succeeded           OK and there are more packets to be decoded.
 *	 WMCDec_DecodeComplete	    When all packets are decoded.
 *   WMCDec_InValidArguments    bad argument(s)
 *	 WMCDec_Fail			    Error   
 *
 */
tWMCDecStatus WMCDecDecodeData (HWMCDECODER hWMCDec, U32_WMC* pnStreamReadyForOutput, U32_WMC* pnNumberOfSamples, I32_WMC iHurryUp);

/*
 *       WMCDecGetAudioOutput
 *       =============
 *
 * Description
 * -----------
 * Write PCM samples from the WMAudio bitstream previously decoded by
 * WMCDecDecodeData().
 *
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetAudioOutput (HWMCDECODER hWMCDec, I16_WMC *pi16Channel0, 
 *                        I16_WMC* pi16Channel1, U32_WMC nNumSamplesRequest, 
 *                       U32_WMC* nNumSamplesReturn, I64_WMC* ptPresentationTime);
 *
 *
 * where:
 *
 *   
 *   hWMCDec            Decoder object HWMCDECODER.
 *   pi16Channel0       pointer to output buffer for left-channel PCM, or the entire PCM output if it is inter-leaved form
 *   pi16Channel1       pointer to output buffer for right-channel PCM, NULL if the output PCM is in parallel form
 *   nNumSamplesRequest Number of samples request
 *   nNumSamplesReturn  Pointer to actual sample return
 *   ptPresentationTime TimeStamp for the output
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMADec_Failed				Error     
 *   WMCDec_InValidArguments    bad argument(s)
 */
tWMCDecStatus WMCDecGetAudioOutput (
    HWMCDECODER hWMCDec,
    I16_WMC *pi16Channel0, I16_WMC* pi16Channel1,
    U32_WMC nNumSamplesRequest, U32_WMC* nNumSamplesReturn,
    I64_WMC* ptPresentationTime
);

/*
 *       WMCDecGetVideoOutput
 *       =============
 *
 * Description
 * -----------
 * Write Video Frame from the Video bitstream previously decoded by
 * WMCDecDecodeData().
 *
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetVideoOutput (HWMCDECODER hWMCDec, U8_WMC *pu8VideoOutBuffer, U32_WMC u32VideoOutBufferSize, U32_WMC * pu32OutDataSize, I64_WMC* ptPresentationTime, Bool_WMC *bIsKeyFrame, tWMCDecodeDispRotateDegree tDispRotateDegree);
 *
 *
 * where:
 *
 *   
 *   
 *   hWMCDec                Decoder object HWMCDECODER.
 *   pu8VideoOutBuffer      pointer to output buffer.
 *   u32VideoOutBufferSize  Size of output buffer
 *   pu32OutDataSize        Point to return data size
 *   ptPresentationTime     TimeStamp for the output
 *   bIsKeyFrame            Pointer to get whether the current output farme is key frame or not
 *	 tDispRotateDegree      Video rotataion. It is defined in wmc_types.h 
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMVDec_Failed				Error     
 *   WMCDec_InValidArguments    bad argument(s)
 *   WMVDec_BrokenFrame         If frame is broken.
 */
tWMCDecStatus WMCDecGetVideoOutput (HWMCDECODER hWMCDec, U8_WMC *pu8VideoOutBuffer, U32_WMC u32VideoOutBufferSize, U32_WMC * pu32OutDataSize, I64_WMC* ptPresentationTime, Bool_WMC *bIsKeyFrame, tWMCDecodeDispRotateDegree tDispRotateDegree);

/*
 *       WMCDecGetBinaryOutput
 *       =============
 *
 * Description
 * -----------
 * Write Binary samples from the Binary bitstream previously decoded by
 * WMCDecDecodeData().
 *
 *
 * Syntax
 * ------
 * tWMCDecStatus WMCDecGetBinaryOutput (HWMCDECODER hWMCDec, U8_WMC *pu8BinaryOutBuffer,
 *               U32_WMC nSizeRequest, U32_WMC* pnSizeReturn, I64_WMC* ptPresentationTime);
 *
 *
 * where:
 *
 *   
 *   
 *   hWMCDec                Decoder object HWMCDECODER.
 *   pu8BinaryOutBuffer     pointer to output buffer.
 *   nSizeRequest           Size of data request
 *   pnSizeReturn           Point to return data size
 *   ptPresentationTime     TimeStamp for the output
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *
 */

tWMCDecStatus WMCDecGetBinaryOutput (
    HWMCDECODER hWMCDec, 
    U8_WMC *pu8BinaryOutBuffer,
    U32_WMC nSizeRequest, U32_WMC* pnSizeReturn,
    I64_WMC* ptPresentationTime
);

/*
 *        WMCDecodeSeek
 *        =============
 *
 * Description
 * -----------
 * Seek to the location of time specified by u64TimeRequest.  
 * 
 *
 * Syntax
 * ------
 *  tWMCDecStatus WMCDecSeek (HWMCDECODER hWMCDec, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn);
 *
 * where:
 *
 *   hWMCDec                   Decoder object HWMCDECODER 
 *   u64TimeRequest            Request time in ms.
 *   pu64TimeReturn            Actual time in ms.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *
 */
tWMCDecStatus WMCDecSeek (HWMCDECODER hWMCDec, U64_WMC u64TimeRequest, U64_WMC* pu64TimeReturn);

/*
 *        WMCDecodeClose
 *        ================
 *
 * Description
 * -----------
 * Closes the decoder object and releases all the buffers.
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecClose (HWMCDECODER* phWMCDec);
 *
 * where:
 *
 *   phWMCDec		            Pointer to the decoder object HWMCDECODER.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *   
 *
 */
tWMCDecStatus WMCDecClose (HWMCDECODER* phWMCDec);

/*
 *       WMCDecGetContentDesc
 *       =================
 * Description
 * -----------
 * Retrieves the content descriptions for the ASF file.  Not all ASF files have Content Descriptions.
 *
 * Syntax
 * ------
 * tWMCDecStatus WMCDecGetContentDesc (HWMCDECODER hWMCDec, const WMCContentDescription **ppDesc);
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   ppDesc                     Pointer of the pointer to the structure that
 *                              holds the ASF file content description info
 *                              (defined in wmc_type.h)
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *   pDesc is the pointer of pointer to the ASF extended content desc object. 
 ***** DO NOT RELEASE THIS POINTER *************   
 *
 */

tWMCDecStatus WMCDecGetContentDesc (HWMCDECODER hWMCDec, const WMCContentDescription **ppDesc);


/*		WMCGetExtendedContentDesc
 *      ==========================   
 *
 * Description
 * -----------
 * Retrieves the extended content descriptions for the ASF file.
 * Not all ASF files have extended content descriptions.
 *
 * Syntax
 * ------
 * tWMCDecStatus WMCDecGetExtendedContentDesc (HWMCDECODER hWMCDec, const WMCExtendedContentDesc **ppECDesc);
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMFDECODER.
 *   ppECDesc                   Pointer of pointer to the structure that holds
 *                              in the ASF file extended content description
 *                              info (defined in wmc_type.h)
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *   pECDesc is the pointer of pointer to the ASF extended content desc object. 
 ***** DO NOT RELEASE THIS POINTER *************   
 */
tWMCDecStatus WMCDecGetExtendedContentDesc (HWMCDECODER hWMCDec, const WMCExtendedContentDesc **ppECDesc);

/*
 *        WMCGetScriptCommand
 *
 * Description
 * -----------
 * Retrieves the script command for the ASF file.
 * Not all ASF files have script commands.
 *
 *
 * Syntax
 * ------
 * tWMCDecStatus WMCGetScriptCommand (HWMCDECODER hWMCDec, const WMCScriptCommand **ppSCObject);
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMFDECODER.
 *   ppSCObject                 Pointer of pointer to the structure that holds
 *                              the ASF file script command info (defined in
 *                              wmc_type.h)
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *   pSCObject is the pointer of pointer to the ASF script command object  
 ***** DO NOT RELEASE THIS POINTER *************   
 *
 */
tWMCDecStatus WMCGetScriptCommand (HWMCDECODER hWMCDec, const WMCScriptCommand **ppSCObject);

/*
 *       WMCGetNumberOfMarkers  
 *       =======================
 *
 * Description
 * -----------
 * Return the number of Markers for the ASF file.  
 *
 * Not all ASF files have content descriptions.
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetNumberOfMarkers (HWMCDECODER hWMCDec, U16_WMC *pu16NumMarkers);  
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   pu16NumMarkers				Pointer to U16_WMC where it will receive the number of markers.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *
 */
tWMCDecStatus WMCDecGetNumberOfMarkers (HWMCDECODER hWMCDec, U16_WMC *pu16NumMarkers);

/*
 *       WMCGetMarker
 *       =======================
 *
 * Description
 * -----------
 * Retrieves the Marker in the ASF file spicifed by an index .  
 *
 * Not all ASF files have content descriptions.
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetMarker (HWMCDECODER hWMCDec, U16_WMC iIndex, WMCMarkerEntry *pEntry); 
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   iIndex                     index specifying which Marker to get
 *   pEntry                     pointer the structure allocated by the application for holding the
 *                              marker spefified by iIndex.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *   WMCDec_Fail				Error
 *
 ***** DO NOT RELEASE THIS POINTER *************   
 */
tWMCDecStatus WMCDecGetMarker (HWMCDECODER hWMCDec, U16_WMC iIndex, WMCMarkerEntry *pEntry); 

/*
 *       WMCSkipToNextKey
 *       =======================
 *
 * Description
 * -----------
 * Skips to next Key Frame while decoding  
 *
 * 
 * Syntax
 * ------
 *   tWMCDecStatus WMCSkipToNextKey (HWMCDECODER hWMCDec);
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *
 */

tWMCDecStatus WMCSkipToNextKey (HWMCDECODER hWMCDec);

/*
 *        WMCDecCBGetData
 *        ==================
 *
 * Description
 * -----------
 * Its a callback function from the SDK to get the ASF data for decoding.
 * This is not implemented by the SDK. It should be implemented by the app that calls the SDK.
 * 
 *
 * Syntax
 * ------
 *  extern U32_WMC WMCDecCBGetData (HWMCDECODER hWMCDec, U64_WMC nOffset, U32_WMC nNumBytesRequest
 *                                  U8_WMC **ppData,    U32_WMC u32UserData);
 *
 *
 * where:
 *
 *   hWMCDec				 Decoder object HWMCDECODER.
 *   U64_WMC nOffset         Offset of the requested data (from beginning),
 *   nNumBytesRequest        Size of the requested data (in bytes).
 *   ppData                  Pointer to the buffer for filling in the ASF data
 *   u32UserData             Extra info From App. Given by user in WMCDecCreate().
 *
 * Return Value
 * ------------
 *   The number of bytes actually being filled in            
 *   
 *
 */
extern U32_WMC WMCDecCBGetData (
    HWMCDECODER hWMCDec,
    U64_WMC nOffset,
    U32_WMC nNumBytesRequest/*, U32_WMC* pnNumBytesReturn*/,
    U8_WMC **ppData,
    U32_WMC u32UserData
);

/*
 *       WMCDecGetCodecCount  
 *       =======================
 *
 * Description
 * -----------
 * Return the number of Codecs Used in the ASF file.  
 *
 * All ASF files may not contain CodecListObject.
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetCodecCount (HWMCDECODER hWMCDec, U32_WMC * pnNumCodec);
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   pnNumCodec				    Pointer to U32_WMC where it will receive the number of codecs.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *
 */

tWMCDecStatus WMCDecGetCodecCount (HWMCDECODER hWMCDec, U32_WMC * pnNumCodec);

/*
 *       WMCDecGetCodecEntry
 *       =======================
 *
 * Description
 * -----------
 * Retrieves the Codecs Description in the ASF file spicifed by an index .  
 *
 * 
 *
 * Syntax
 * ------
 *   tWMCDecStatus WMCDecGetCodecEntry (HWMCDECODER hWMCDec, U16_WMC iIndex, WMCCodecEntry *pEntry); 
 *
 * where:
 *
 *   hWMCDec		            Decoder object HWMCDECODER.
 *   iIndex                     index specifying which Codec Description to get
 *   pEntry                     pointer the structure allocated by the application for holding the
 *                              Codec Description spefified by iIndex.
 *
 * Return Value
 * ------------
 *   WMCDec_Succeeded			OK
 *   WMCDec_InValidArguments    bad argument(s)
 *   WMCDec_Fail				Error
 *
 ***** DO NOT RELEASE THIS POINTER *************   
 */

tWMCDecStatus WMCDecGetCodecEntry (HWMCDECODER hWMCDec, U16_WMC iIndex, WMCCodecEntry *pEntry); 

#ifdef __cplusplus
}
#endif 




#endif // _WMC_DECODE_H_
