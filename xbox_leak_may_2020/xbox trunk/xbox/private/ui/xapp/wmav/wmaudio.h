/*
 * Windows Media Audio (WMA) Decoder API
 *
 * Copyright (c) Microsoft Corporation 1999.  All Rights Reserved.
 */

#ifndef _WMAUDIO_API_H_
#define _WMAUDIO_API_H_

#define PACKED
/*
#ifdef _MSC_VER
#define USE_PACK_HEADERS
#define PACKED
#else
#undef USE_PACK_HEADERS
#define PACKED  __packed
#endif

#ifdef USE_PACK_HEADERS
#include <pshpack1.h>
#endif
*/

/* ...........................................................................
 *
 * Types and Constants
 * ===================
 */

#define CHECK_NONSDMI_LIC	1
#define CHECK_SDMI_LIC		2
#define CHECK_ALL_LICENSE	3

#define WMA_MAX_DATA_REQUESTED  128


/* 16-bit signed type  */
#ifndef _WMAI16_DEFINED
#define _WMAI16_DEFINED
typedef short tWMA_I16;
#endif /* _WMAI16_DEFINED */

/* 16-bit unsigned type  */
#ifndef _WMAU16_DEFINED
#define _WMAU16_DEFINED
typedef short tWMA_U16;
#endif /* _WMAU16_DEFINED */

/* 32-bit unsigned type  */
#ifndef _WMAU32_DEFINED
#define _WMAU32_DEFINED
typedef unsigned long tWMA_U32;
#endif /* _WMAU32_DEFINED */

#ifndef _QWORD_DEFINED
#define _QWORD_DEFINED
typedef PACKED struct tQWORD
{
    tWMA_U32   dwLo;
    tWMA_U32   dwHi;

}   QWORD;
#endif /* _QWORD_DEFINED */

typedef struct _MarkerEntry {
    QWORD   m_qOffset;
    QWORD   m_qtime;
    tWMA_U16    m_wEntryLen;
    tWMA_U32   m_dwSendTime;
    tWMA_U32   m_dwFlags;
    tWMA_U32   m_dwDescLen;
    tWMA_U16   *m_pwDescName;
} MarkerEntry;

/* status */
#ifndef _WMAFILESTATUS_DEFINED
#define _WMAFILESTATUS_DEFINED
typedef enum tagWMAFileStatus
{
    cWMA_NoErr,                 /* -> always first entry */
                                /* remaining entry order is not guaranteed */
    cWMA_Failed,
    cWMA_BadArgument,
    cWMA_BadAsfHeader,
    cWMA_BadPacketHeader,
    cWMA_BrokenFrame,
    cWMA_NoMoreFrames,
    cWMA_BadSamplingRate,
    cWMA_BadNumberOfChannels,
    cWMA_BadVersionNumber,
    cWMA_BadWeightingMode,
    cWMA_BadPacketization,

    cWMA_BadDRMType,
    cWMA_DRMFailed,
    cWMA_DRMUnsupported,

    cWMA_DemoExpired,

    cWMA_BadState,
    cWMA_Internal               /* really bad */

} tWMAFileStatus;
#endif /* _WMAFILESTATUS_DEFINED */

/* versions */
#ifndef _WMAFILEVERSION_DEFINED
#define _WMAFILEVERSION_DEFINED
typedef enum tagWMAFileVersion
{
    cWMA_V1 = 1,
    cWMA_V2 = 2

} tWMAFileVersion;
#endif /* _WMAFILEVERSION_DEFINED */

/* sample rates */
#ifndef _WMAFILESAMPLERATE_DEFINED
#define _WMAFILESAMPLERATE_DEFINED
typedef enum tagWMAFileSampleRate
{
    cWMA_SR_48kHz,
    cWMA_SR_44_1kHz,
    cWMA_SR_32kHz,
    cWMA_SR_22_05kHz,
    cWMA_SR_16kHz,
    cWMA_SR_11_025kHz,
    cWMA_SR_08kHz
} tWMAFileSampleRate;
#endif /* _WMAFILESAMPLERATE_DEFINED */

/* channels */
#ifndef _WMAFILECHANNELS_DEFINED
#define _WMAFILECHANNELS_DEFINED
typedef enum tagWMAFileChannels
{
    cWMA_C_Mono = 1,
    cWMA_C_Stereo = 2

} tWMAFileChannels;
#endif /* _WMAFILECHANNELS_DEFINED */


/* ...........................................................................
 *
 * Structures
 * ==========
 */

#ifdef _LINUX_x86
#pragma pack (1)
#endif

/* header */
#ifndef _WMAFILEHEADER_DEFINED
#define _WMAFILEHEADER_DEFINED
typedef PACKED struct tagWMAFileHeader
{
    tWMAFileVersion version;         /* version of the codec */
    tWMAFileSampleRate sample_rate;  /* sampling rate */
    tWMAFileChannels num_channels;   /* number of audio channels */
    tWMA_U32 duration;               /* of the file in milliseconds */
    tWMA_U32 packet_size;            /* size of an ASF packet */
    tWMA_U32 first_packet_offset;    /* byte offset to the first ASF packet */
    tWMA_U32 last_packet_offset;     /* byte offset to the last ASF packet */

    tWMA_U32 has_DRM;                /* does it have DRM encryption? */

    tWMA_U32 bitrate;                /* bit-rate of the WMA bitstream */

    /* HongCho: what else? */

} tWMAFileHeader;
#endif /* _WMAFILEHEADER_DEFINED */


/* content description */
#ifndef _WMAFILECONTDESC_DEFINED
#define _WMAFILECONTDESC_DEFINED
typedef PACKED struct tagWMAFileContDesc
{
    /* *_len: as [in], they specify how large the corresponding
     *        buffers below are.
     *        as [out], they specify how large the returned
     *        buffers actually are.
     */

    tWMA_U16 title_len;
    tWMA_U16 author_len;
    tWMA_U16 copyright_len;
    tWMA_U16 description_len;   /* rarely used */
    tWMA_U16 rating_len;        /* rarely used */

    /* these are two-byte strings
     *   for ASCII-type character set, to convert these
     *   into a single-byte characters, simply skip
     *   over every other bytes.
     */

    unsigned char *pTitle;
    unsigned char *pAuthor;
    unsigned char *pCopyright;
    unsigned char *pDescription;
    unsigned char *pRating;

} tWMAFileContDesc;
#endif /* _WMAFILECONTDESC_DEFINED */

/* license params */
#ifndef _WMAFILELICPARAMS_DEFINED
#define _WMAFILELICPARAMS_DEFINED
typedef PACKED struct tagWMAFileLicParams
{
    unsigned char *pPMID;       /* portable media id */
    tWMA_U32 cbPMID;            /* length of the pPMID buffer */

} tWMAFileLicParams;
#endif /* _WMAFILELICPARAMS_DEFINED */


typedef void * tHWMAFileState;

/* another internal state */
#ifndef _WMAFILEHDRSTATE_DEFINED
#define _WMAFILEHDRSTATE_DEFINED
typedef PACKED struct tagWMAFileHdrState
{
    unsigned char internal[188];

} tWMAFileHdrState;
#endif /* _WMAFILEHDRSTATE_DEFINED */

#ifdef _LINUX_x86
#pragma pack ()
#endif

/* ........................................................................... 
 *
 * Functions
 * =========
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*
 *        WMAFileIsWMA
 *        ============
 *
 * Description
 * -----------
 * Checks to see if the file is decodable.  Uses WMAFileCBGetData(),
 * but the first argument to it is not really of type tHWMAFileState,
 * but tWMAFileIdState.
 *
 * To decode the file, the app should call WMAFileDecodeInit()
 * instead.
 *
 * This function was added in case the app wants to determine the
 * type of the file without fully allocating tHWMAFileState.
 *
 * Syntax
 * ------
 *   tWMAFileStatus WMAFileIsWMA(tWMAFileHdrState *state);
 *
 * where:
 *
 *   state          pointer to the smaller structure that contains
 *                  state of the checking function
 *
 * Return Value
 * ------------
 *   cWMA_NoErr                yes, it's a WMA file
 *   cWMA_BadArgument          bad argument passed in
 *   cWMA_BadAsfHeader         bad ASF header
 *
 */
tWMAFileStatus __stdcall WMAFileIsWMA (tWMAFileHdrState *state);

/*
 *        WMAGetMarkers
 *        ============
 *
 * Description
 * -----------
 * Get the marker list so the main program can seek the file freely.
 *
 * Syntax
 * ------
 * int WMAGetMarkers(tWMAFileHdrState *state,MarkerEntry **ppEntry);
 *
 * where:
 *
 *   state          pointer to the smaller structure that contains
 *                  state of the checking function
 *   ppEntry        Address of the poitner to store the array of MarkerEntry
 *
 * Return Value
 * ------------
 * number of Marker Entries
 *
 */
int __stdcall WMAGetMarkers(tWMAFileHdrState *state,MarkerEntry **ppEntry);

/*
 *        WMAGetLicenseStore
 *        ============
 *
 * Description
 * -----------
 * Get the marker list so the main program can seek the file freely.
 *
 * Syntax
 * ------
 * BYTE * WMAGetLicenseStore(tWMAFileHdrState *pstate,tWMA_U32 *pLen)
 *
 * where:
 *
 *   state          pointer to the smaller structure that contains
 *                  state of the checking function
 *   pLen           Address of the poitner to store the size of license.
 *
 * Return Value
 * ------------
 * Pointer of data chunck
 *
 */
unsigned char * __cdecl WMAGetLicenseStore(tWMAFileHdrState *pstate,tWMA_U32 *pLen);

/*
 *        WMAFileCBGetData
 *        ================
 *
 * Description
 * -----------
 * Supplies more data to the WMAudio decoder.  Not implemented by
 * the WMAudio decoder library.
 *
 * It is a callback function implemented by the application.  When
 * the WMAudio library needs more data to process (both to parse
 * informatio and to decode the actual data bitstream), the library
 * will call this function.
 *
 * Called by WMAFileDecodeInit(), WMAFileContentDesc() and
 * WMAFileDecodeData().
 *
 * Syntax
 * ------
 *   tWMA_U32 WMAFileCBGetData(tHWMAFileState hstate,
 *                             tWMA_U32 offset,
 *                             tWMA_U32 num_bytes,
 *                             unsigned char **ppData);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   offset         offset into the WMA file to get the data from
 *   num_bytes      how many bytes to get
 *   ppData         pointer to contain the returned buffer with more
 *                  data
 *                  The WMA library may over-write this buffer
 *
 * Return Value
 * ------------
 *   tWMA_U32       how many actually returned
 *                  should be same as num_bytes except for at the
 *                  very end of the file
 *
 */

/*
extern tWMA_U32 __stdcall WMAFileCBGetData (
    tHWMAFileState hstate,
    tWMA_U32 offset,
    tWMA_U32 num_bytes,
    unsigned char **ppData);
*/

/*
 *        WMAFileCBGetLicenseData
 *        =======================
 *
 * Description
 * -----------
 * Supplies more data to the WMAudio DRM decryption function.  Not
 * implemented by the WMAudio decoder library.
 *
 * It is a callback function implemented by the application.  When
 * the WMAudio's DRM decryption library needs more data to process
 * the library will call this function.
 *
 * Called by WMAFileLicenseInit().
 *
 * Syntax
 * ------
 *   tWMA_U32 WMAFileCBGetLicenseData(tHWMAFileState *state,
 *                                    tWMA_U32 offset,
 *                                    tWMA_U32 num_bytes,
 *                                    unsigned char **ppData);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   offset         offset into the license file to get the data from
 *   num_bytes      how many bytes to get
 *   ppData         pointer to contain the returned buffer with more
 *                  data
 *
 * Return Value
 * ------------
 *   tWMA_U32       how many actually returned
 *                  should be same as num_bytes except for at the
 *                  very end of the file
 *
 */

/*
extern tWMA_U32 WMAFileCBGetLicenseData (
    tHWMAFileState *pstate,
    tWMA_U32 offset,
    tWMA_U32 num_bytes,
    unsigned char **ppData);
*/

/*
 *        WMAFileDecodeInit
 *        =================
 *
 * Description
 * -----------
 * Initializes the WMAudio decoder.
 *
 * Must be called before starting to decode a WMA file.
 *
 * Syntax
 * ------
 *   tWMAFileStatus WMAFileDecodeInit(tHWMAFileState *state);
 *
 * where:
 *
 *   phstate        pointer to the handle that holds the internal
 *                  state of the WMAudio decoder
 *
 * Return Value
 * ------------
 *   cWMA_NoErr                decoder initialised
 *   cWMA_BadArgument          bad argument passed in
 *   cWMA_BadAsfHeader         bad ASF header
 *   cWMA_BadSamplingRate      invalid or unsupported sampling rate
 *   cWMA_BadNumberOfChannels  invalid or unsupported number of channels
 *   cWMA_BadVersionNumber     invalid or unsupported version number
 *   cWMA_BadWeightingMode     invalid or unsupported weighting mode
 *   cWMA_BadPacketisation     invalid or unsupported packetisation
 *   cWMA_BadDRMType           unknown encryption type
 *   cWMA_DRMFailed            DRM failed
 *   cWMA_DRMUnsupported       DRM is not supported for this version
 *
 */
tWMAFileStatus __stdcall WMAFileDecodeInit (tHWMAFileState* phstate);


tWMAFileStatus __stdcall WMAFileDecodeClose (tHWMAFileState* phstate);

/*
 *       WMAFileDecodeInfo
 *       =================
 *
 * Description
 * -----------
 * Retrieves the header information for the WMA file.
 *
 * Syntax
 * ------
 *   tWMAFileStatus WMAFileDecodeInfo(tHWMAFileState hstate,
 *                                    tWMAFileHeader *hdr);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   hdr            pointer to the structure that holds the WMA file
 *                  header info
 *
 * Return Value
 * ------------
 *   cWMA_NoErr                header retrieved ok
 *   cWMA_BadArgument          bad argument(s)
 *
 */

tWMAFileStatus __stdcall WMAFileDecodeInfo (tHWMAFileState hstate, tWMAFileHeader *hdr);


/*
 *       WMAFileContentDesc
 *       ==================
 *
 * Description
 * -----------
 * Retrieves the content description for the WMA file.  Content
 * descriptions are such as the song title, the author, the
 * copyright info, the song description, and the rating info.
 * All these are part of the standard ASF description.
 *
 * Not all songs have content descriptions.
 *
 * Syntax
 * ------
 *   tWMAFileStatus WMAFileContentDesc(tHWMAFileState hstate,
 *                                     tWMAFileContDesc *desc);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   desc           pointer to the structure that holds the WMA file
 *                  content description info
 *                  as [in], must provide the buffer(s) and the size(s)
 *
 * Return Value
 * ------------
 *   cWMA_NoErr                description retrieved ok
 *   cWMA_BadArgument          bad argument(s)
 *   cWMA_BadAsfHeader         bad ASF header
 *
 */
tWMAFileStatus __stdcall WMAFileContentDesc (tHWMAFileState hstate, tWMAFileContDesc *desc);


/*
 *       WMAFileLicenseInit
 *       ==================
 *
 * Description
 * -----------
 * Reads the DRM license file and check the license for the file.
 *
 * Uses WMAFileCBGetLicenseData to retrieve the license file.
 *
 * Syntax
 * ------
 *   tWMAFileStatus WMAFileLicenseInit(tHWMAFileState hstate,
 *                                     tWMAFileLicParams *lic_params);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   lic_params     pointer to the structure that passes in the
 *                  parameters for the license initialization.
 *
 * Return Value
 * ------------
 *   cWMA_NoErr                license checked ok
 *   cWMA_BadArgument          bad argument
 *   cWMA_DRMFailed            no license
 *   cWMA_DRMUnsupported       DRM is not supported for this version
 *
 */
tWMAFileStatus WMAFileLicenseInit (tHWMAFileState hstate, tWMAFileLicParams *lic_params, tWMA_U16 CheckLicTypes);


/*
 *       WMAFileDecodeData
 *       =================
 *
 * Description
 * -----------
 * Decodes WMAudio bitstream.
 *
 * Syntax
 * ------
 *   tWMAFileStatus WMAFileDecodeData(tHWMAFileState hstate);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *
 * Return Value
 * ------------
 *   cWMA_NoErr                frame decoded ok
 *   cWMA_BadArgument          bad argument
 *   cWMA_BrokenFrame          data is inconsistent
 *   cWMA_NoMoreFrames         no more data to decode
 *
 */
tWMAFileStatus __stdcall WMAFileDecodeData (tHWMAFileState hstate);


/*
 *       WMAFileGetPCM
 *       =============
 *
 * Description
 * -----------
 * Write PCM samples from the WMAudio bitstream previously decoded by
 * WMAFileDecodeData().
 *
 * WMAFileGetPCM() may have to be called more than once to obtain all
 * of the samples for the frame. The frame is complete when the number
 * of samples written is less than the number of samples requested.
 *
 * Syntax
 * ------
 *   tWMA_U32 WMAFileGetPCM(tHWMAFileState hstate,
 *                          tWMA_I16 *left, tWMA_I16 *right,
 *                          tWMA_U32 max_nsamples);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   pi16Channel0   pointer to output buffer for left-channel PCM, or the entire PCM output if it is inter-leaved form
 *   pi16Channel1   pointer to output buffer for right-channel PCM, NULL if the output PCM is in parallel form
 *   max_nsamples   maximum number of samples to write per channel
 *
 * Return Value
 * ------------
 *   tWMA_U32       number of samples written
 *                  < max_nsamples when frame complete
 *
 */
tWMA_U32 __stdcall WMAFileGetPCM (
    tHWMAFileState hstate,
    tWMA_I16 *pi16Channel0, tWMA_I16 *pi16Channel1,
    tWMA_U32 max_nsamples);


/*
 *       WMAFileSeek
 *       ===========
 *
 * Description
 * -----------
 * Seek to a time into the WMA file.  It seeks to the nearest seek
 * point.
 *
 * Syntax
 * ------
 *   tWMA_32 WMAFileSeek(tHWMAFileState hstate,
 *                       tWMA_U32 msSeek);
 *
 * where:
 *
 *   state          pointer to the structure that holds the internal
 *                  state of the WMAudio decoder
 *   msSeek         time position to seek from the beginning of the
 *                  clip, in milliseconds
 *
 * Return Value
 * ------------
 *   tWMA_32        actual time position from the beginning of the
 *                  clip, in milliseconds
 *
 */
tWMA_U32 WMAFileSeek (tHWMAFileState hstate, tWMA_U32 msSeek);


#ifdef __cplusplus
}
#endif /* __cplusplus */

/*
#ifdef USE_PACK_HEADERS
#include <poppack.h>
#endif
*/

#endif /* _WMAUDIO_API_H_ */
