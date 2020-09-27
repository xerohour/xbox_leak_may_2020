/*************************************************************************

Copyright (C) Microsoft Corporation, 2000

Module Name:

    wmfdecode.h

Abstract:

    API's for light WMF SDK Decoder.

Revision History:


*************************************************************************/

#ifndef _WMF_DECODE_H_
#define _WMF_DECODE_H_

#include "wmf_types.h"

typedef void * HWMFDECODER;

#if defined (macintosh) || defined (HITACHI)
#include "wmMacSp.h"
#endif //macintosh


#ifdef __cplusplus
extern "C" {
#endif 

#define CHECK_NONSDMI_LIC	1
#define CHECK_SDMI_LIC		2
#define CHECK_ALL_LICENSE	3
#define WMF_LICENCE_DATA_REQUESTED  172


typedef struct tagWMAFileLicParams
{
    U8_WMF *pPMID;       /* portable media id */
    U32_WMF cbPMID;      /* length of the pPMID buffer */

} tWMAFileLicParams;


typedef struct tagAudioStreamInfo
{
    Bool_WMF    bHasValidAudio; // if there is a valid audio stream 
    U16_WMF     u16WMAVersion; // WMA version number, 1 or 2.
    U32_WMF     u32BitRate; // bitrate, in bits per second
    U32_WMF     u32SamplesPerSec; // sampling rate
    U16_WMF     u16Channels; // number of channels, 1 or 2.
    U16_WMF     u16BitsPerSample; // bits per sample, always 16
    U32_WMF     u32Duration; // duration in miliseconds
    Bool_WMF    bHasDRM; // if there is a DRM
#ifdef _ASFPARSE_ONLY_
	U16_WMF		wFormatTag;
#endif

} AudioStreamInfo;

#define NO_FRAMERATE
typedef struct tagVideoStreamInfo
{
    Bool_WMF    bHasValidVideo; // if there is a valid video stream 
    U32_WMF     u32CompressionType; // the compression type if the video stream, e.g., FOURCC_WMV1
    U32_WMF     u32Width; // image width
    U32_WMF     u32Height; // image height
    U32_WMF     u32VideoOutputPixelType;
    U16_WMF     u16VideoOutputBitCount; // Output bitcount
#ifndef NO_FRAMERATE
    Double_WMF  dbFrameRate; // video frame rate
#endif
    U8_WMF    bPalette[3*256]; // Palette
    Bool_WMF    bHasDRM; // if there is a DRM
} VideoStreamInfo;

typedef enum {
    WMPERR_OK = 0,     // no error, success
    WMPERR_FAIL,            // (un-reasoned, generic) failure
	WMPERR_INVALIDARG,      // invalid argument
    WMPERR_OUTOFMEMORY,     // out of memory
	WMPERR_BADDATA,
	WMPERR_BADHEADER,       // header is bad
    WMPERR_INVALIDHANDLE,   // invalid handle
    WMPERR_INVALIDPOINTER,  // invalid memory pointer
	WMPERR_UNINITIALIZED,  // invalid memory pointer
	WMPERR_INITERR,
	WMPERR_INVALIDHEADER,  // invalid memory pointer
    WMPERR_INVALIDPROPERTIES,
	WMPERR_INVALIDOBJECT,
	WMPERR_INVALIDENCRYPT,
	WMPERR_AUDIODECINITFAIL,
	WMPERR_DRMFAIL,
	WMPERR_DRMINITFAIL,
	WMPERR_PACKETSIZE,
	WMPERR_AUDIOINFO,
	WMPERR_TIMEOFFSET,
	WMPERR_LASTPACKET,
	WMPERR_FRAMERATE,
	WMPERR_BISIZE,
	WMPERR_BIWIDTH,
	WMPERR_BIHEIGHT,
	WMPERR_BICOMPRESSION,
	WMPERR_BISIZEIMAZE,
	WMPERR_EOF,             // end of file
    WMPERR_FILEREAD,        // failed in a file read operation

    WMPERR_BUFTOOSMALL,     // buffer is too small for the operation
    WMPERR_BUFTOOLARGE,     // buffer is too large for the operation
    WMPERR_DECODER_ERROR,     // packet decoder signals something is wrong
    WMPERR_VIDEO_DECODER_ERROR, // either screen or video decoder signals err   
    WMPERR_DECODE_COMPLETE,
    WMPERR_DRMNOTSUPPORTED,
    WMPERR_DEMOEXPIRED,
	WMPERR_NOTSUPPORTED,
} WMPERR;

typedef struct tagWMFExtendedContentDescription
{
    U16_WMF cDescriptors;             // number of descriptors
    ECD_DESCRIPTOR *pDescriptors;  // pointer to all the descriptors
} WMFExtendedContentDesc;

typedef struct tagWMFScriptCommand
{
    U16_WMF num_commands;      // number of script commands
    U16_WMF num_types;         // number of types of commands
    U16Char_WMF **type_names;     // command names, URL, FILENAME, CAPTION, etc
    I32_WMF *type_name_len;     // length of the command name
    I32_WMF *command_param_len; // length of command parameters
    CommandEntry *commands; // pointer to all the commands
} WMFScriptCommand;

// #ifdef _ZMDRM_
typedef void * HDRMMGR;
// #endif

/*
 *        WMFDecodeCreate
 *        =============
 *
 * Description
 * -----------
 *   This function Create the decoder object. Must be called before calling any function.
 * 
 *
 * Syntax
 * ------
 *   HWMFDECODER WMFDecodeCreate ();
 *
 *
 *
 * Return Value
 * ------------
 *   WMF decoder object, NULL if there is an error.            
 *
 */

HWMFDECODER WMFDecodeCreate (PVOID pCallbackContext);

/*
 *        WMFDecodeInit
 *        =============
 *
 * Description
 * -----------
 *   This function initializes the decoder object. Must be called before calling any function.
 * 
 *
 * Syntax
 * ------
 *   WMPERR WMFDecodeInit (HWMFDECODER hDecoder, U32_WMF u32VideoOutputPixelType, U16_WMF u16VideoOutputBitCount);
 *
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER..
 *   u32VideoOutputPixelType	Pixel color format of the decoded video.
 *   u16VideoOutputBitCount     Bit count (bits per pixel) of the decoded video.
 *
 * Return Value
 * ------------
 *   Error code. WMPERR_OK if succeeds.          
 *
 */

WMPERR WMFDecodeInit (HWMFDECODER hDecoder, U32_WMF u32VideoOutputPixelType, U16_WMF u16VideoOutputBitCount);

/*
 *        WMFDecodeGetStreamInfo
 *        ======================
 *
 * Description
 * -----------
 *   Get the audio and video information of the ASF stream/file.
 * 
 *
 * Syntax
 * ------
 *   WMPERR WMFDecodeGetStreamInfo (HWMFDECODER hDecoder, AudioStreamInfo *pAudioInfo, VideoStreamInfo *pVideoInfo);
 *
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *   pAudioInfo	                Pointer to the audio info structure. The decoder fills in this structure.
 *   pVideoInfo                 Pointer to the video info structure. The decoder fills in this structure.
 *
 * Return Value
 * ------------
 *   Error code. WMPERR_OK if succeeds.          
 *
 */

WMPERR WMFDecodeGetStreamInfo (HWMFDECODER hDecoder, AudioStreamInfo *pAudioInfo, VideoStreamInfo *pVideoInfo);


/*
 *       WMFGetContentDesc
 *       =================
 * Description
 * -----------
 * Retrieves the content descriptions for the ASF file.  Not all ASF files have script commands.
 *
 * Syntax
 * ------
 * WMFGetContentDesc (HWMFDECODER hDecoder, const WMFContentDescription **pDesc);
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *   pDesc                      pointer of the pointer to the structure that
 *                              holds the ASF file content description info
 *                              (defined in wmf_types.h)
 *
 * Return Value
 * ------------
 *   WMPERR_OK                  description retrieved ok
 *   WMPERR_INVALIDARG          bad argument(s)
 *   pDesc is the pointer of pointer to the ASF extended content desc object. 
 ***** DO NOT RELEASE THIS POINTER *************   
 *
 */

WMPERR WMFGetContentDesc (HWMFDECODER hDecoder, const WMFContentDescription **pDesc);


/* WMFGetExtendedContentDesc 
 *
 * Description
 * -----------
 * Retrieves the extended content descriptions for the ASF file.
 * Not all ASF files have extended content descriptions.
 *
 * Syntax
 * ------
 * WMFGetExtendedContentDesc (HWMFDECODER hDecoder, const WMFExtendedContentDesc **pECDesc);
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *   pECDesc                    pointer of pointer to the structure that holds
 *                              in the ASF file extended content description
 *                              info (defined in wmf_types.h)
 *
 * Return Value
 * ------------
 *   WMPERR_OK                  description retrieved ok
 *   WMPERR_INVALIDARG          bad argument(s)
 *   pECDesc is the pointer of pointer to the ASF extended content desc object. 
 ***** DO NOT RELEASE THIS POINTER *************   
 */

WMPERR WMFGetExtendedContentDesc (HWMFDECODER hDecoder, const WMFExtendedContentDesc **pECDesc);

/*
 *        WMFGetScriptCommand
 *
 * Description
 * -----------
 * Retrieves the script command for the ASF file.
 * Not all ASF files have script commands.
 *
 *
 * Syntax
 * ------
 * WMFGetScriptCommand (HWMFDECODER hDecoder, const WMFScriptCommand **pSCObject);
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *   pSCObject                  pointer of pointer to the structure that holds
 *                              the ASF file script command info (defined in
 *                              wmf_types.h)
 *
 * Return Value
 * ------------
 *   WMPERR_OK                  description retrieved ok
 *   WMPERR_INVALIDARG          bad argument(s)
 *   pSCObject is the pointer of pointer to the ASF script command object  
 ***** DO NOT RELEASE THIS POINTER *************   
 *
 */

WMPERR WMFGetScriptCommand (HWMFDECODER hDecoder, const WMFScriptCommand **pSCObject);


/*
 *       WMFGetNumberOfMarkers  
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
 *   I32_WMF WMFGetNumberOfMarkers(HWMFDECODER hDecoder)
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *
 * Return Value
 * ------------
 *   number of markers is returned, 0 if no markers, 
 *
 ***** DO NOT RELEASE THIS POINTER *************   
 */

I32_WMF WMFGetNumberOfMarkers(HWMFDECODER hDecoder);  


/*
 *       WMFGetMarker
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
 *   WMPERR WMFGetMarker(HWMFDECODER hDecoder, I32_WMF iIndex,  MarkerEntry *pEntry);
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *   iIndex                     index specifying which Marker to get
 *   pEntry                     pointer the structure allocated by the application for holding the
 *                              marker spefified by iIndex.
 *
 * Return Value
 * ------------
 *   WMAERR_OK                  OK 
 *	 WMAERR_FAIL		        Error  
 *
 ***** DO NOT RELEASE THIS POINTER *************   
 */

WMPERR WMFGetMarker(HWMFDECODER hDecoder, I32_WMF iIndex,  MarkerEntry *pEntry);



/*
 *        WMFDecode
 *        =========
 *
 * Description
 * -----------
 * This function decodes the packets of the input file.
 * Parses the packet and calls Audio/Video decoders (depending on packet info). It also calls
 * WMFCBOutputDecodedAudio and WMFCBOutputDecodedVideo for Audio/Video decoded data.
 * 
 *
 * Syntax
 * ------
 *   WMPERR WMFDecode (HWMFDECODER hDecoder);
 *
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *
 * Return Value
 * ------------
 *   WMAERR_OK                  OK and there are more packets to be decoded.
 *	 WMPERR_DECODE_COMPLETE	    When all packets are decoded.
 *   WMPERR_INVALIDARG          bad argument(s)
 *	 WMAERR_FAIL		        Error   
 *
 */

WMPERR WMFDecode (HWMFDECODER hDecoder);

extern WMPERR WMFCBGetVideoOutputBuffer (HWMFDECODER hDecoder,PVOID pContext, U8_WMF** ppucVideoOutoutBuffer);
extern WMPERR WMFCBGetAudioOutputBuffer (HWMFDECODER hDecoder,PVOID pContext, U8_WMF** ppucAudioOutoutBuffer);

/*
 *        WMFCBOutputDecodedAudio
 *        =================
 *
 * Description
 * -----------
 * Its a callback function which is called after decoding Audio data in each packet. 
 * This is not implemented by the SDK. It should be implemented by the app that calls the SDK.
 * 
 *
 * Syntax
 * ------
 *   extern WMPERR WMFCBOutputDecodedAudio (HWMFDECODER hDecoder,const U8_WMF* pbAudio, U32_WMF cbAudioSize, U32_WMF cbSamples, U32_WMF cbTotAudioOut, U32_WMF dwAudioTimeStamp);
 *
 *
 * where:
 *
 *   pbAudio		address of buffer where decoded Audio data resides
 *   cbAudioSize    size of Audio data 
 *   cbSamples      no. of samples 
 *   cbTotAudioOut  Total no. of samples decoded till now 
 *   dwAudioTimeStamp Time stamp in ms
 * Return Value
 * ------------
 *   WMPERR_OK for successful completion            
 *   
 *
 */

extern WMPERR WMFCBOutputDecodedAudio (HWMFDECODER hDecoder,PVOID pContext, const U8_WMF* pbAudio, U32_WMF cbAudioSize, U32_WMF cbSamples, U32_WMF cbTotAudioOut, U32_WMF dwAudioTimeStamp);
/*
 *        WMFCBOutputDecodedVideo
 *        ================
 *
 * Description
 * -----------
 * Its a callback function which is called after decoding each Video Frame.
 * This is not implemented by the SDK. It should be implemented by the app that calls the SDK.
 * 
 *
 * Syntax
 * ------
 *   extern WMPERR WMFCBOutputDecodedVideo (HWMFDECODER hDecoder, const U8_WMF *pbVideo, U32_WMF cbFrame, U32_WMF  biSizeImage, U32_WMF dwVideoTimeStamp);
 *
 *
 * where:
 *
 *   pbVideo		address of buffer where decoded Video data resides 
 *   cbFrame		Frame no. 
 *   biSizeImage	image size 
 *   dwVideoTimeStamp Time stamp in ms
 * Return Value
 * ------------
 *   WMPERR_OK for successful completion            
 *   
 *
 */

extern WMPERR WMFCBOutputDecodedVideo (HWMFDECODER hDecoder, PVOID pContext, const U8_WMF *pbVideo, U32_WMF cbFrame, U32_WMF biSizeImage, U32_WMF dwVideoTimeStamp);



/*
 *        WMFDecodeClose
 *        ================
 *
 * Description
 * -----------
 * Closes the decoder object and releases all the buffers.
 * 
 *
 * Syntax
 * ------
 *   WMPERR WMFDecodeClose (HWMFDECODER *hDecoder);
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *
 * Return Value
 * ------------
 *   WMAERR_OK                  OK.
 *   WMPERR_INVALIDARG          bad argument(s)
 *   
 *
 */

WMPERR WMFDecodeClose (HWMFDECODER *hDecoder);

/*
 *        WMFDecodeSeek
 *        =============
 *
 * Description
 * -----------
 * Seek to the location of time specified by cdMs.  
 * 
 *
 * Syntax
 * ------
 *  WMPERR WMFDecodeSeek (HWMFDECODER hDecoder, U32_WMF cbMs);
 *
 * where:
 *
 *   hDecoder                   address of HWMADECODER 
 *   cbMs                       offset time in ms.
 *
 * Return Value
 * ------------
 *   WMAERR_OK                  OK.
 *   WMPERR_INVALIDARG          bad argument(s)
 *
 */

WMPERR WMFDecodeSeek (HWMFDECODER hDecoder, U32_WMF cbMs);


/*
 *        WMFCBDecodeGetData
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
 *  extern WMPERR WMFCBDecodeGetData (HWMFDECODER hDecoder, I8_WMF* pbuffer, I32_WMF cbSize, U64_WMF cbOffset, I32_WMF *cbRead);
 *
 *
 * where:
 *
 *   pContext                   Pointer to user context (georgioc added)
 *   pbuffer                    Pointer to the buffer for filling in the ASF data.
 *   cbSize                     Size of the requested data (in bytes).
 *   cbOffset                   Offset of the requested data (from beginning).
 *   cbRead                     The number of bytes actually being filled in.
 *
 * Return Value
 * ------------
 *   WMPERR_OK for successful completion            
 *   
 *
 */

extern WMPERR WMFCBDecodeGetData (HWMFDECODER hDecoder, PVOID pContext, I8_WMF* pbuffer, U32_WMF cbSize, U64_WMF cbOffset, U32_WMF *cbRead);



/*
 *        WMFCBGetLicenseData
 *        ==================
 *
 * Description
 * -----------
 * Its a callback function from the SDK to get the Licence data for decoding if licence data is not found in asf header.
 * This is not implemented by the SDK. It should be implemented by the app that calls the SDK.
 * 
 *
 * Syntax
 * ------
 *  extern WMPERR WMFCBGetLicenseData (HWMFDECODER hDecoder, U8_WMF **pData, U32_WMF cbSize, U32_WMF cbOffset, U32_WMF *cbRead);
 *
 *
 *
 * where:
 *
 *   pData                      Pointer to pointer the buffer where licence data is lying.
 *   cbSize                     Size of the requested data (in bytes).
 *   cbOffset                   Offset of the requested data (from beginning).
 *   cbRead                     The number of bytes actually being filled in.
 *
 * Return Value
 * ------------
 *   WMPERR_OK for successful completion            
 *   
 *
 */

extern WMPERR WMFCBGetLicenseData (HWMFDECODER hDecoder, U8_WMF **pData, U32_WMF cbSize, U32_WMF cbOffset, U32_WMF *cbRead);

/*
 *        WMFDrmLicenseInit
 *        =========
 *
 * Description
 * -----------
 * This function init the DRM licence. This should be called if stream is encrypted.
 *  
 *
 * Syntax
 * ------
 *   extern WMPERR WMFDrmLicenseInit (HWMFDECODER hDecoder, tWMAFileLicParams *lic_params, U32_WMF CheckLicTypes);
 *
 *
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER.
 *	 lic_params					Licence parameters
 *   CheckLicTypes				CHECK_ALL_LICENSE.
 * Return Value
 * ------------
 *   WMAERR_OK                  OK .
 *   WMPERR_INVALIDARG          bad argument(s)
 *	 WMAERR_DRMFAIL		        Error   
 *
 */

WMPERR WMFDrmLicenseInit (HWMFDECODER hDecoder, tWMAFileLicParams *lic_params, U32_WMF CheckLicTypes);

WMPERR WMFFlushBuffers(HWMFDECODER hDecoder);

/*
 *       WMFDebugMessage
 *       ===============
 *
 * Description
 * -----------
 * Display a debugging message. This function needs to be implemented by the app that calls WMEPAK.
 * These messages are meant for the device/application developer, not the end user.
 * Display the message using fprintf, messagebox, or whatever method is suitable to your platform.
 * See the sample code for an example implementation.
 * Make it a empty-body function if the intent is to skip this operation.
 *
 * Syntax
 * ------
 *		WMFDebugMessage(const char* pszFmt,
 *						...);
 *
 * where:
 *
 *   pszFmt		    pointer to a null terminated printf like format string
 *   ...			variable number of arguments referenced by the format string
 *
 * Return Value
 * ------------
 *   none
 *
 */
extern void WMFDebugMessage (const char*pszFmt, ...);

#ifdef _ZMDRM_
//#define LPSTR char*
//#define HRESULT long
typedef I32_WMF HRESULT;
typedef I8_WMF * LPSTR;
typedef const char * LPCSTR;
#define DRM_CHALLENGE_STRING 	0
#define DRM_CHALLENGE_FILE		1 // not yet used/ implemented
// #endif _ZMDRM_


/*
 *        WMFDecodeInitDRMV7
 *        ==================
 *
 * Description
 * -----------
 *   This function initializes the decoder object. Must be called before calling any function.
 * 
 *
 * Syntax
 * ------
 *   WMPERR WMFDecodeInit (HWMFDECODER hDecoder, U32_WMF u32VideoOutputPixelType, U16_WMF u16VideoOutputBitCount);
 *
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER..
 *   u32VideoOutputPixelType	Pixel color format of the decoded video.
 *   u16VideoOutputBitCount     Bit count (bits per pixel) of the decoded video.
 *   phDRM                      Pointer to DRM Manager
 *   pAppCert                   Pointer to application certificate
 *   Rights                     Set the right that corresponds to the way in
 *                              which the content will be used.
 *
 * Return Value
 * ------------
 *   Error code. WMPERR_OK if succeeds.          
 *
 */

WMPERR WMFDecodeInitDRMV7 (HWMFDECODER hDecoder, U32_WMF u32VideoOutputPixelType, U16_WMF u16VideoOutputBitCount,HDRMMGR *phDRM, U8_WMF *pAppCert, U32_WMF Rights, const U8_WMF* pbHwId, U32_WMF cbHwId);

//extern I32_WMF WMFInitZMDRM(HWMFDECODER hDecoder ,U32_WMF Rights);

/*
 *        WMFCanDecryptNewLic
 *        ===================
 *
 * Description
 * -----------
 *   This function verifies whether the new license exists that grants the
 *   access rights. User has to generate a challenge and obtain a response
 *   before calling this function.
 *
 * Syntax
 * ------
 *   I32_WMF WMFCanDecryptNewLic (HWMFDECODER hDecoder);
 *
 *
 * where:
 *
 *   hDecoder		            Pointer to the decoder object HWMFDECODER..
 *
 * Return Value
 * ------------
 *   Error code. 0 if succeeds.          
 */

extern I32_WMF WMFCanDecryptNewLic(HWMFDECODER hDecoder);

/*
 *        WMFGenerateChallenge
 *        ====================
 *
 * Description
 * -----------
 *   This function generates a challenge string to send to a DRM license server.
 *   The string will uniquely identify the content, the hardware acquiring
 *   the license, and commuicate over other V7 metadata.
 *
 * Syntax
 * ------
 *   I32_WMF WMFGenerateChallenge(HDRMMGR hDRM, LPSTR *ppszUrl, LPSTR *ppszChallenge );
 *
 *
 * where:
 *
 *   hDRM                       DRM Manager
 *   ppszUrl                    pointer to LPSTR, URL value upon completion
 *   ppszChallenge              pointer to LPSTR, challenge string upon
 *                              completion
 *
 * Return Value 0, if success
 */

extern HRESULT WMFGenerateChallenge(HDRMMGR hDRM, LPSTR *ppszUrl, LPSTR *ppszChallenge );

/*
 *        WMFProcessResponse
 *        ==================
 *
 * Description
 * -----------
 *   This function processes the response from the challenge that was posted
 *   using HTTP to the license server.
 *
 * Syntax
 * ------
 * I32_WMF WMFProcessResponse(HDRMMGR hDRM, LPCSTR pszResponse );
 *
 *
 * where:
 *
 *   hDRM                       DRM Manager
 *   pszResponse                LPCSTR, response from the challenge
 *
 * Return Value 0, if success
 */

extern HRESULT WMFProcessResponse(HDRMMGR hDRM, LPCSTR pszResponse );
#endif
#ifdef __cplusplus
}
#endif 

#endif // _WMF_ENCODE_H_
