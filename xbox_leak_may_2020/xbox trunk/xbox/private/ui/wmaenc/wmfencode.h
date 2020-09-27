/*************************************************************************

Copyright (C) Microsoft Corporation, 2000

Module Name:

    wmfencode.h

Abstract:

    API's for light WMF SDK Encoder.

Revision History:


*************************************************************************/


#ifndef _WMF_ENCODE_H_
#define _WMF_ENCODE_H_

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "wmf_types.h"
typedef void* HWMFENCODER;


/*********  Video coding mode  *********/

typedef enum tagVideoCodingMode_WMF
{
    WMF_OnePass_CBR = 0,
    WMF_TwoPass_CBR, // not supported now
    WMF_OnePass_VBR,
    WMF_TwoPass_VBR
} VideoCodingMode_WMF;

/*****************************************/


/*************  Statistics  *************/

typedef struct strCodingStats_WMF
{
// video
    U32_WMF    dwVideoFourCC;
    Double_WMF dbVideoTotalNumBytes;
    Double_WMF dbVideoAverageBitrate;
    U32_WMF    dwVideoNumTotalFrame;
    U32_WMF    dwVideoNumCodedFrame;
    Double_WMF dbVideoAverageFrameRate;
    U32_WMF    dwVideoBufferDelay;
    U32_WMF    dwVideoKeyFrameDistance;
// audio
    Double_WMF dbAudioTotalNumBytes;
    U32_WMF    dwAudioPayloadSize;
    U32_WMF    dwAudioAverageBitrate;
    U32_WMF    dwAudioSamplingRate;
    U32_WMF    dwAudioChannelNum;

// overall
    U32_WMF    dwMinRateGivenBufferSize;
    U32_WMF    dwMinBufferSizeGivenAvgRate;
    Double_WMF dbTotalNumBytes;
    U32_WMF    dwPacketSize;
    U32_WMF    dwPaddingBytes;
} CodingStats_WMF;

/*****************************************/

/******  Video pre-processing types  *****/

typedef enum tagVideoPreProcessType_WMF
{
    WMF_NoPreProcess = 0,
    WMF_PreserveSize, // (e.g., 640x480x30fps interlaced to 640x480x30fps progressive)
    WMF_HalfSize, // (e.g., 640x480x30fps interlaced to 320x240x30fps progressive)
    WMF_HalfSizeAndDoubleFrameRate, // (e.g., 640x480x30fps interlaced to 320x240x60fps progressive)
    WMF_VerticalHalfSizeAndDoubleFrameRate, // (e.g., 320x480x30fps interlaced to 320x240x60fps progressive)
    WMF_InverseTelecine // (e.g., 640x480x30fps telecined to 640x480x24fps progressive)
} VideoPreProcessType_WMF;

/*****************************************/



/*********  Return status code  *********/
#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else 
#define EXTERN_C extern 
#endif
#endif

typedef enum tagWMFEncodeStatus
{
    WMF_Succeeded = 0,
    WMF_Failed,
    WMF_BadMemory,
    WMF_InValidArguments,
    WMF_EncPacketsDone,
    WMF_BadOutputFile,

    WMA_Failed,
    WMA_BadMemory,
    WMA_EncodeFailed,
    WMA_UnSupportedInputFormat,
    WMA_UnSupportedCompressedFormat,
    WMA_InValidArguments,
    WMA_BadSource,

    WMV_Failed,
    WMV_BadMemory,
    WMV_EncodeFailed,
    WMV_UnSupportedInputFormat,
    WMV_UnSupportedCompressedFormat,
    WMV_InValidArguments,
    WMV_BadSource,
    WMV_BadFileWrite

} tWMFEncodeStatus;

/*****************************************/


/*********  Encoding profiles  ***********/

typedef enum tagWMFEncodeProfile
{
    Profile_AudioVideo_Modem_28K = 0, // 20K actual bitrate
    Profile_AudioVideo_Modem_56K, // 32K actual bitrate
    Profile_AudioVideo_LAN_100K,
    Profile_AudioVideo_LAN_256K, // 225K 
    Profile_AudioVideo_LANDSL_384K, // 350K
    Profile_AudioVideo_LANDSL_768K, // 450K
    Profile_AudioVideo_NearBroadCast_700K,
    Profile_AudioVideo_NearBroadCast_1400K,
    Profile_AudioVideo_Broadband_PAL_384K, // 350K
    Profile_AudioVideo_NearBroadCast_PAL_700K,
    Profile_AudioVideo_LAN_100K_2Pass, // 100K
    Profile_AudioVideo_LANDSL_384K_2Pass, // 350K
    Profile_AudioVideo_FilmVBR_600K, // 600K
    Profile_AudioVideo_FilmVBR_Quality97,
    Profile_AudioOnly_FMRadioMono_28K, // for 288 modem
    Profile_AudioOnly_FMRadioStereo_28K, // for 288 modem
    Profile_AudioOnly_Modem_56K, // for 56K modem
    Profile_AudioOnly_NearCDQuality_48K,
    Profile_AudioOnly_CDQuality_64K,
    Profile_AudioOnly_CDAudiophileQuality_96K,
    Profile_AudioOnly_CDAudiophileQuality_128K,
    Profile_VideoOnly_Modem_28K, // 20K
    Profile_VideoOnly_Modem_56K, // 32K actual bitrate
    Profile_Total_Numbers
} WMFEncodeProfile;

/*****************************************/

/*********  Profile parameter details  *********/

typedef struct tpProfileParameters_WMF
{
    I32_WMF dwAudioBitRate;
    U32_WMF nDstChannels; 
    U32_WMF nDstSamplesPerSec;
#if defined (ENABLE_WMAV3)
    U32_WMF nDstChannelMask;
    U32_WMF nAudioBitstreamVersion;
#endif // ENABLE_WMAV3

    I32_WMF dwVideoBitRate;
    Double_WMF dbDstFrameRate;
    U32_WMF uiPictureQuality;
    U32_WMF uiDelayBuffer;
    U32_WMF biDstWidth; 
    U32_WMF biDstHeight; 
    U32_WMF biDstCompression; 
    U32_WMF uiKeyFrameDistance;
    VideoCodingMode_WMF tVideoCodingMode;
    VideoPreProcessType_WMF tVideoPreProcMode;
} ProfileParameters_WMF;

/*****************************************/

/***************************  Call back functions  *******************************/

/*
 *        WMFCBGetAudioData
 *        ===============
 *
 * Description
 * -----------
 * Call back funtion from the SDK to get the requested audio data from the app.
 * The app is responsible for implementing this funtion. SDK calls this funtion to get the audio data while encoding.
 *
 * Arguments
 * ---------
 *
 *   ppbAudioDataBuffer     The memory buffer that the app should write the audio data to. The app can change the pointer if necessary.
 *   nInputBufferLen        Length  of the buffer space (in bytes).
 *   iSampleOffset          Offset (from the beginning, in number of samples) that the requested audio data starts.
 *   iNumSamplesWanted      Number of audio samples requested to be filled in pbAudioDataBuffer.
 *
 * Return Value
 * ------------
 *   Int                    Number of samples returned from app.
 *
 */

EXTERN_C I32_WMF WMFCBGetAudioData (U8_WMF **pbAudioDataBuffer, U32_WMF nInputBufferLen, I64_WMF iSampleOffset, I32_WMF iNumSamplesWanted);


/*
 *        WMFCBGetVideoData
 *        ===============
 *
 * Description
 * -----------
 * Call back funtion from the SDK to get the requested video data from the app.
 * The app is responsible for implementing this funtion. SDK calls this funtion to get the video data while encoding.
 *
 * Arguments
 * ---------
 *
 *   ppbVideoDataBuffer     The memory buffer that the app should write the video data to. The app can change the pointer if necessary.
 *   nInputBufferLen        Length  of the buffer space (in bytes).
 *   i64DesiredTS           Desired Time Stamp
 *   pi64TimeStamp          Time Stamp of the return frame
 *
 * Return Value
 * ------------
 *   Int                    Number of number of bytes returned from app.
 *
 */

EXTERN_C I32_WMF WMFCBGetVideoData (U8_WMF **ppbVideoDataBuffer, U32_WMF nInputBufferLen, I64_WMF i64DesiredTS, I64_WMF *pi64TimeStamp);


/*********************************************************************************/

/***************************  FUNCTIONS  *******************************/

/*
 *        WMFGetRecommendedVideoFrameRate
 *        ===============================
 *
 * Description
 * -----------
 * Get a recommended frame rate (frames per second) from the SDK. This is an OPTIONAL funtion call.
 * This function is expected to be called before WMFCreateEncoderUsingCustom of WMFCreateEncoderUsingProfile.
 *
 * Arguments
 * ---------
 *
 *   piRecommendedFPS       Pointer to the value recommended (set) by SDK.
 *   dwVideoBitRate         Bitrate for video to be encoded.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Funtion succeeds.
 *   WMV_InValidArguments   Bad argument passed in (bitrate out of range).
 *
 */

tWMFEncodeStatus WMFGetRecommendedVideoFrameRate (I32_WMF *piRecommendedFPS, I32_WMF dwVideoBitRate);


/***************************  FUNCTIONS  *******************************/

/*
 *        WMFGetProfileDetails
 *        ====================
 *
 * Description
 * -----------
 * Get the parameter details for a profile.
 *
 * Arguments
 * ---------
 *
 *   tProfile               Profile number.
 *   ptProfilePara          Profile structure to be filled in.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Funtion succeeds.
 *   WMV_InValidArguments   Bad argument passed in (bitrate out of range).
 *
 */

tWMFEncodeStatus WMFGetProfileDetails (WMFEncodeProfile tProfile, ProfileParameters_WMF *ptProfilePara);

/*
 *        WMFIsThereAudioStream
 *        =====================
 *
 * Description
 * -----------
 * Return the property of the encoder event. Return if the event contains an audio stream.
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   bIsThereAudio          Pointer to the value set by SDK.
 *                          * TRUE: There is an audio stream.
 *                          * FALSE: There is no audio stream.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Funtion succeeds.
 *   WMF_BadMemory          hWMFEnc is bad.
 *
 */

tWMFEncodeStatus WMFIsThereAudioStream (Bool_WMF *bIsThereAudio, HWMFENCODER hWMFEnc);

/*
 *        WMFIsThereVideoStream
 *        =====================
 *
 * Description
 * -----------
 * Return the property of the encoder event. Return if the event contains a video stream.
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   bIsThereVideo          Pointer to the value set by SDK.
 *                          * TRUE: There is a video stream.
 *                          * FALSE: There is no video stream.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Funtion succeeds.
 *   WMF_BadMemory          hWMFEnc is bad.
 *
 */

tWMFEncodeStatus WMFIsThereVideoStream (Bool_WMF *bIsThereVideo, HWMFENCODER hWMFEnc);

/*
 *        WMFCreateEncoderUsingCustom
 *        ===========================
 *
 * Description
 * -----------
 * Create the WMF encoder using the specified custom settings for audio and video.
 *
 * Arguments
 * ---------
 *
 *   ptStatus               Pointer to the status of encoder creation.  It can be one of the value in tagWMFEncodeStatus.
 *   pchOutFilename         Pointer to the name of the encoded ASF file.
 *
 *********  The following are the audio properties.
 *
 *   dwAudioBitRate         Bitrate for the audio (in bits per second).
 *                          * The SDK enumerates this rate and pick the closest one in the list.
 *                          * Value 0 is for video only (no audio) ASF file. The properties (arguments) for audio will be ignored.
 *   nSrcChannels           Number of channels in the source file (valid number: 1 or 2).
 *   nSrcSamplesPerSec      Sampling rate in the source file.  The SDK enumerates this rate and pick the closest one in the list.
 *   wSrcBitsPerSample      Bits per PCM sample (resolution) in the source file.  The SDK only accepts 16-bit input.
 *   nSrcDataLen            Data length (in bytes) of the source audio file.
 *   puiAudioSrcSizeNeeded  Pointer to the value that SDK returns to app for the source size needed to encode audio. 
 *                          The app is recommended to use this value to allocate a memory space for audio source data.
 *   nDstChannels           Number of channels for the encoded (destination) output (valid number: 1 or 2).
 *   nDstSamplesPerSec      Sampling rate for the encoded output.  The SDK enumerates this rate and pick the closest one in the list.
 *   pdwActualAudioBitrate
 *   pnActualDstChannels
 *   pnActualDstSamplesPerSec
 *
 *********  The following are the video properties.
 *
 *   dwVideoBitRate         Bitrate for the video (in bits per second).
 *                          * Value 0 is for audio only (no vidio) ASF file.
 *                          * dwAudioBitRate and dwVideoBitRate can't be zero at the same time.
 *   dbSrcFrameRate         Frame rate of the source video.
 *   biSrcWidth             Width of the source bitmap.
 *   biSrcHeight            Height of the source bitmap.
 *   prctClipRegion         The region (Rect) to be encoded in each frame. It is ignored if the region is not valid or biggeer than the original.
 *   biSrcBitCount          Bits per pixel of the source bitmap.
 *   biSrcCompression       The pixel (color) format of the source bitmap.  The SDK accepts the following types (in preference order). (See definitions above for the types.)
 *                          * FOURCC_I420		        
 *                          * FOURCC_IYUV		        
 *                          * FOURCC_YV12		        
 *                          * FOURCC_YUY2		        
 *                          * FOURCC_UYVY		        
 *                          * FOURCC_YVYU		        
 *                          * FOURCC_YVU9
 *                          * FOURCC_BI_RGB   	    
 *                          * FOURCC_BI_BITFIELDS	    
 *   qwDuration             Encoding Duration in ms  .
 *   nColor                 Number of colors in palette, Used for 8 bits case only.
 *   pbColors               Pointer to the start of Palette. Used for 8 bits case only. 			
 *   puiVideoSrcSizeNeeded  Pointer to the value that SDK returns to app for the source size needed to encode video. 
 *                          The app is recommended to use this value to allocate a memory space for video source data.
 *   dbDstFrameRate         Encoded (output) frame rate. It is recommended to call WMFGetRecommendedVideoFrameRate to get a proper rate.
 *   biDstWidth             Width of the encoded bitmap.
 *   biDstHeight            Height of the encoded bitmap.
 *   biDstCompression       Compression type of the encoded video.  The SDK encoded to one of the following types (in preference order). (See definitions above for the types.)
 *                          * FOURCC_WMV1   	    
 *                          * FOURCC_WMV2   	    
 *   uiPictureQuality       The degree of smoothness/picture quality of the encoded video. This is a tradeoff between the individual image frame and number of encoded frames.
 *                          The valid range of uiSmoothness is 0 to 100, 0 being the smoothest. 0 is recommended for most of the cases unless image quality is specifically emphasized.
 *   uiKeyFrameDistance     The maximum key frame distance of the encoded video.  The unit is in seconds.
 *                          Note that this is the maximum distance since the encoder might decide to add key frames anytime.
 *                          But it is guaranteed that there must be a key frame in 8s.
 *   uiDelayBuffer          The delay/buffer time of the video.  The unit is in milliseconds.
 *                          This determines the pre-roll time when playing back the file in an ASF player.

 *
 * Return Value
 * ------------
 *   HWMFENCODER            Handle to the encoder. NULL if anything goes wrong during the creation.
 *
 */


HWMFENCODER WMFCreateEncoderUsingCustom (
        tWMFEncodeStatus*      ptStatus,
        I8_WMF                 *pchOutFilename,
// audio properties
        I32_WMF                dwAudioBitRate,
        U32_WMF                nSrcChannels,
        U32_WMF                nSrcSamplesPerSec,  
        U32_WMF                wSrcBitsPerSample,  
        U64_WMF                nSrcDataLen,
        U32_WMF*               puiAudioSrcSizeNeeded,
        U32_WMF                nDstChannels,
        U32_WMF                nDstSamplesPerSec,  
        U32_WMF*               pdwActualAudioBitrate,
        U32_WMF*               pnActualDstChannels,
        U32_WMF*               pnActualDstSamplesPerSec,
#if defined (ENABLE_WMAV3)
        U32_WMF                nSrcValidBitsPerSample,
        U32_WMF                nSrcChannelMask,
        U32_WMF                nDstChannelMask,
		U32_WMF                nAudioBitstreamVersion,
#endif // ENABLE_WMAV3
// video properties
    // source/input
        Double_WMF             dbSrcFrameRate,
        U32_WMF                biSrcWidth,
        U32_WMF                biSrcHeight,
        Rect_WMF               *prctClipRegion,
        U32_WMF                biSrcBitCount,
        U32_WMF                biSrcCompression,
        U64_WMF                qwDuration,
        U32_WMF                nColors,
        U8_WMF                 *pbColors,
        U32_WMF                *puiVideoSrcSizeNeeded,
        VideoPreProcessType_WMF tVideoProcess,
    // destination (output)
        Double_WMF             dbDstFrameRate,
        U32_WMF                biDstWidth,
        U32_WMF                biDstHeight,
        U32_WMF                biDstCompression,

        VideoCodingMode_WMF    tVideoCodingMode, // VBR, CBR, etc.
        I32_WMF                dwVideoBitRate, // in bits per second
        U32_WMF                uiPictureQuality, // 0 to 100, 0 being the smoothest
        U32_WMF                uiKeyFrameDistance, // in seconds
        U32_WMF                uiDelayBuffer, // in milliseconds
        I8_WMF *             chModifiedFileName

        // source is interlaced
//        U32_WMF                biDeInterlace // DeInterlace

);


/*
 *        WMFCreateEncoderUsingProfile
 *        ============================
 *
 * Description
 * -----------
 * Create the WMF encoder using one of the profiles specified in tagWMFEncodeProfile.
 *
 * Arguments
 * ---------
 *
 *   ptStatus               Pointer to the status of encoder creation.  It can be one of the value in tagWMFEncodeStatus.
 *   pchOutFilename         Pointer to the name of the encoded ASF file.
 *   tProfile               One of the encoding profile specified in tagWMFEncodeProfile
 *
 *********  The following are the audio properties.
 *
 *   nSrcChannels           Number of channels in the source file (valid number: 1 or 2).
 *   nSrcSamplesPerSec      Sampling rate in the source file.  The SDK enumerates this rate and pick the closest one in the list.
 *   wSrcBitsPerSample      Bits per PCM sample (resolution) in the source file.  The SDK only accepts 16-bit input.
 *   nSrcDataLen            Data length (in bytes) of the source audio file.
 *   puiAudioSrcSizeNeeded  Pointer to the value (in bytes) that SDK sets for the app for the source size needed to encode audio.
 *
 *********  The following are the video properties.
 *
 *   dbSrcFrameRate         Frame rate of the source video.
 *   biSrcWidth             Width of the source bitmap.
 *   biSrcHeight            Height of the source bitmap.
 *   prctClipRegion         The region (Rect) to be encoded in each frame. It is ignored if the region is not valid or biggeer than the original.
 *   biSrcBitCount          Bits per pixel of the source bitmap.
 *   biSrcCompression       The pixel (color) format of the source bitmap.  The SDK accepts the following types (in preference order). (See definitions above for the types.)
 *                          * FOURCC_I420		        
 *                          * FOURCC_IYUV		        
 *                          * FOURCC_YV12		        
 *                          * FOURCC_YUY2		        
 *                          * FOURCC_UYVY		        
 *                          * FOURCC_YVYU		        
 *                          * FOURCC_YVU9
 *                          * FOURCC_BI_RGB   	    
 *                          * FOURCC_BI_BITFIELDS	    
 *   qwDuration             Encoding Duration in ms  .
 *   puiVideoSrcSizeNeeded  Pointer to the value that SDK returns to app for the source size needed to encode video. 
 *                          The app is recommended to use this value to allocate a memory space for video source data.
 *
 *
 * Return Value
 * ------------
 *   HWMFENCODER            Handle to the encoder. NULL if anything goes wrong during the creation.
 *
 */

HWMFENCODER WMFCreateEncoderUsingProfile (
        tWMFEncodeStatus     *ptStatus,
        I8_WMF               *pchOutFilename,
        WMFEncodeProfile     tProfile,

// audio input source properties
        U32_WMF              nSrcChannels,
        U32_WMF              nSrcSamplesPerSec,  
        U32_WMF              wSrcBitsPerSample,  
        U64_WMF              nSrcDataLen,
        U32_WMF              *puiAudioSrcSizeNeeded,
#if defined (ENABLE_WMAV3)
        U32_WMF              nSrcValidBitsPerSample,
        U32_WMF              nSrcChannelMask,
#endif // ENABlE_WMAV3

// video properties
        Double_WMF           dbSrcFrameRate,
        U32_WMF              biSrcWidth,
        U32_WMF              biSrcHeight,
        Rect_WMF             *prctClipRegion,
        U32_WMF              biSrcBitCount,
        U32_WMF              biSrcCompression,
        U64_WMF              qwDuration,
        U32_WMF              nColors,
        U8_WMF               *pbColors,
        U32_WMF              *puiVideoSrcSizeNeeded,
        VideoPreProcessType_WMF VideoProcess,
        I8_WMF *             chModifiedFileName

);

/*
 *        WMFPutContentDescriptions
 *        =========================
 *
 * Description
 * -----------
 * Put content descriptions in the encoded ASF file (in overhead).
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   pchTitle               Title info.
 *   nLengthTitle           Length of title info.
 *   pchAuthor              Author info,
 *   nLengthAuthor          Length of author info.
 *   pchDescription         Description info,
 *   nLengthDescription     Length of Description info.
 *   pchCopyrigtht          Copyrigtht info,
 *   nLengthCopyrigtht      Length of Copyrigtht info.
 *   pchRating              Rating info,
 *   nLengthRating          Length of Rating info.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Function succeeds.
 *   WMF_Failed            Function fails.
 *
 */

tWMFEncodeStatus WMFPutContentDescriptions (
        HWMFENCODER hWMFEnc, 
//        const WMFContentDescription* pContentDesc
        const U16Char_WMF *pchTitle, U32_WMF nLengthTitle,
        const U16Char_WMF *pchAuthor, U32_WMF nLengthAuthor,
        const U16Char_WMF *pchDescription, U32_WMF nLengthDescription,
        const U16Char_WMF *pchCopyrigtht, U32_WMF nLengthCopyrigtht,
        const U16Char_WMF *pchRating, U32_WMF nLengthRating
);

/*            WMFPutScriptCommands
 * Description
 * -----------
 * Put script commands in the encoded ASF file
 * arguments:
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   nCommands              number of commands
 *   nTypes                 number of types of commands, such as URL
 *   ppTypeNames            type names
 *   CommandEntry           command entries

 * Return Value
 * ------------
 *   WMF_Succeeded          Function succeeds.
 *   WMF_InValidArgument    Function fails due to invalid arguments
 */

tWMFEncodeStatus WMFPutScriptCommands (				  
        HWMFENCODER         hWMFEnc,
        U16_WMF             nCommands,
        U16_WMF             nTypes,
        U16Char_WMF         **ppTypeNames,
        const CommandEntry  *pCommands
);

/*
 *        WMFPutExtendedContentDesc
 *        =========================
 *
 * Description
 * -----------
 * Put extended content descriptions in the encoded ASF file (in overhead).
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   nDescriptors           number of descriptors
 *   pDescriptors           pointer to the descriptor structure

 * Return Value
 * ------------
 *   WMF_Succeeded          Function succeeds.
 *   WMF_InValidArguments   Invalid arguments
 *   WMF_BadMemory          memory allocation problem
 */

tWMFEncodeStatus WMFPutExtendedContentDesc(
        HWMFENCODER         hWMFEnc,
        U16_WMF             nDescriptors,
        ECD_DESCRIPTOR      *pDescriptors
);

/*
 *        WMFPutMarkers
 *        =============
 *
 * Description
 * -----------
 * Put Markers in the encoded ASF file (in overhead).
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                    Handle of the encoder created in WMFCreateEncoder.
 *   wNumMarkers                number of MarkerEntry
 *   WORD  wNameLen             length of marker object name in wchar unit includeing terminating zero  
 *   const wchar *pchMarkerName pointer to the name of this Marker object
 *   pMarkers                   pointer to the MarkerEntry structure
 *                              For MarkerEntry,  You just need to fill in the following fields
 *      m_qtime                 Presentation time (Unit 1/10000000 sec)
 *      m_dwDescLen             Length of Description name in wchar unit includeing terminating zero
 *      m_pwDescName            Description name
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Function succeeds.
 *   WMF_InValidArguments   Invalid arguments
 *   WMF_BadMemory          memory allocation problem
 */

tWMFEncodeStatus WMFPutMarkers(
    HWMFENCODER hWMFEnc, 
    U32_WMF dwNumMarkers,  
    U16_WMF  wNameLen,
    const U16Char_WMF *pchMarkerName,
    MarkerEntry *pMarkers
);

/*
 *        WMFEncode
 *        =========
 *
 * Description
 * -----------
 * Encode the input audio and video data to ASF file using the specified settings in WMFCreateEncoderUsingCustom.
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   pbAudioBuffer          Pointer to the memory allocated by the app to hold the audio data to be encoded.
 *                          The SDK has a call back function to the app to request to fill in this memory with audio data to be encoded.
 *   pnAudioBufferSize      Size (in bytes) of the pbAudioBuffer.
 *   pbVideoBuffer          Pointer to the memory allocated by the app to hold the video data to be encoded.
 *                          The SDK has a call back function to the app to request to fill in this memory with video data to be encoded.
 *   pnVideoBufferSize      Size (in bytes) of the pbVideoBuffer.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Encode succeeds.
 *   
 *   WMA_Failed             Audio fails.
 *   WMA_BadMemory          Audio part has bad memory.
 *   WMA_EncodeFailed       Audio encode fails.
 *   WMA_InValidArguments   Audio part has invalid arguments.
 *   WMA_BadSource          Audio source data is bad.

 *   WMV_Failed             Video fails.
 *   WMV_BadMemory          Video part has bad memory.
 *   WMV_EncodeFailed       Video encode fails.
 *   WMV_InValidArguments   Video part has invalid arguments.
 *   WMV_BadSource          Video source data is bad.
 *
 */

tWMFEncodeStatus WMFEncode (
    HWMFENCODER hWMFEnc, 
    U8_WMF* pbAudioBuffer, U32_WMF pnAudioBufferSize, 
    U8_WMF* pbVideoBuffer, U32_WMF pnVideoBufferSize
);


/*
 *        WMFClose
 *        ========
 *
 * Description
 * -----------
 * Close the encoder.
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Close succeeds.
 *   WMF_BadMemory          Memory is bad.
 *
 */

tWMFEncodeStatus WMFClose (HWMFENCODER hWMFEnc);

/*
 *        WMFGetStats
 *        ===========
 *
 * Description
 * -----------
 * Get the Eencoding Status.
 *
 * Arguments
 * ---------
 *
 *   hWMFEnc                Handle of the encoder created in WMFCreateEncoder.
 *   pCodingStats           Pointer to a CodingStats_WMF to hold the status
 *
 * Return Value
 * ------------
 *   WMF_Succeeded          Succeeds.
 *   WMF_Failed             Fail.
 *
 */

tWMFEncodeStatus WMFGetStats (HWMFENCODER hWMFEnc, CodingStats_WMF* pCodingStats);

#ifdef __cplusplus
}
#endif

#endif // _WMF_ENCODE_H_
