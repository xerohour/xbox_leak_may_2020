//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wmf.h
//
//--------------------------------------------------------------------------

#ifndef WMF_H
#define WMF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wmcdecode.h"

/****************************************************************************/
#if( !defined(_MSC_VER) && !defined(__stdcall) )
#define __stdcall
#endif

#ifndef WMAAPI
#ifndef x86
#define WMAAPI  __stdcall
#else
#define WMAAPI
#endif
#endif

#define STREAMVIDEO 0x0002
#define STREAMAUDIO 0x0001
#define LARGEBUFFER 0x8000

typedef struct tPacketList
{
	I8_WMC				*pBuff;
	I32_WMC					cbUsed;
	I32_WMC					cbFinished;
	I32_WMC					cbFrame;
	struct tPacketList	*Next;
}PacketList;

#ifdef _ZMDRM_
tWMCDecStatus WMFOpenDecoder(const U8_WMC *pbHeader, U32_WMC cbHeader, HWMCDECODER *phDecoder, HDRMMGR *hDRM, U8_WMC *pAppCert, U32_WMC Rights, const U8_WMC* pbHwId, U32_WMC cbHwId);
#else
tWMCDecStatus WMFOpenDecoder(const U8_WMC *pbHeader, U32_WMC cbHeader, HWMCDECODER *phDecoder);
#endif


/*
 *        WMFCloseDecoder
 *        ===============
 *
 * Description
 * -----------
 * Close a WMA decoder instance.
 *
 * Syntax
 * ------
 *   WMAERR WMAAPI WMFCloseDecoder(HWMFDECODER hDecoder);
 *
 * where:
 *
 *   hDecoder       the handle to the WMA instance
 *
 * Return Value
 * ------------
 *   WMAERR_OK                 closed okay
 *   WMAERR_INVALIDARG         invalid handle
 *
 */

tWMCDecStatus WMFCloseDecoder(HWMCDECODER hDecoder);





/*
 *        WMFDecTimeToOffset
 *        ==================
 *
 * Description
 * -----------
 * Use the millisecond offset to locate the offset to the
 * corresponding packet.
 *
 * Syntax
 * ------
 *   WMAERR WMAAPI WMFDecTimeToOffset(HWMFDECODER hDecoder,
 *                                    U32_WMC msSeek,
 *                                    U64_WMC *pcbPacketOffset);
 *
 * where:
 *
 *   hDecoder         the handle from WMFOpenDecoder
 *   msSeek           the time offset in milliseconds
 *   pcbPacketOffset  the offset into the beginning of the
 *                    corresponding ASF packet
 *
 * Return Value
 * ------------
 *   WMAERR_OK                 retreived okay
 *   WMAERR_INVALIDARG         invalid handle
 *
 */
tWMCDecStatus WMFDecTimeToOffset(HWMCDECODER hDecoder,
                                 U32_WMC msSeek,
                                 U64_WMC *pcbPacketOffset);


/*
 *        WMFDecDRM
 *        =========
 *
 * Description
 * -----------
 * Is the content DRM'ed?
 *
 * Syntax
 * ------
 *   WMAERR WMAAPI WMFDecDRM(HWMFDECODER hDecoder,
 *                           U32_WMC *pfDRM);
 *
 * where:
 *
 *   hDecoder         the handle from WMFOpenDecoder
 *   pfDRM            pointer to the flag to indicate whether
 *                    the content is DRM'ed or not
 *
 * Return Value
 * ------------
 *   WMAERR_OK                 retreived okay
 *   WMAERR_INVALIDARG         invalid handle
 *
 */
tWMCDecStatus WMFDecDRM(HWMCDECODER hDecoder,
                        U32_WMC *pfDRM);




/*
 *        WMFDrmGetLicFileName
 *        ====================
 *
 * Description
 * -----------
 * Get the path to the license filename.
 *
 * Syntax
 * ------
 *   WMAERR WMAAPI WMFDrmGetLicFileName(HWMFDECODER hDecoder,
 *                                      I8_WMC *pszLicFileName,
 *                                      U32_WMC *pdwLicFileName);
 *
 * where:
 *
 *   hDecoder       the handle from WMFOpenDecoder
 *   pszLicFileName the pointer to hold the filename
 *   pdwLicFileName the pointer to hold the length of the filename
 *                  when pszLicFileName is set to NULL, it holds the
 *                  size necessary to hold the filename
 *
 * Return Value
 * ------------
 *   WMAERR_OK                 the filename retreived okay
 *   WMAERR_INVALIDARG         invalid pointers
 *   WMAERR_MOREDATA           the length is retrieved, need to call again
 *                             to get the name
 *   WMAERR_DRMFAIL            the DRM lib failed
 *
 */
tWMCDecStatus WMFDrmGetLicFileName(HWMCDECODER hDecoder,
                                   I8_WMC *pszLicFileName,
                                   U32_WMC *pdwLicFileName);


/*
 *        WMFDrmInitLicense
 *        =================
 *
 * Description
 * -----------
 * Initialize the DRM license.
 *
 * Syntax
 * ------
 *   WMAERR WMAAPI WMFDrmInitLicense(HWMFDECODER hDecoder,
 *                                   U8_WMC *pbPMID,
 *                                   U32_WMC dwPMID,
 *                                   U8_WMC *pbLicData,
 *                                   U32_WMC *pdwOffset,
 *                                   U32_WMC *pdwLen);
 *
 * where:
 *
 *   hDecoder       the handle from WMFOpenDecoder
 *   pbPMID         the pointer to a buffer that holds the portable
 *                  media id
 *   dwPMID         the size of the pbPMID buffer
 *   pbLicData      the pointer to the license file buffer
 *   pdwOffset      the pointer to hold the offset into the license
 *                  file, from the beginning
 *   pdwLen         the pointer to hold the length of the pbLicData
 *                  buffer
 *
 * Return Value
 * ------------
 *   WMAERR_OK                 license checked okay
 *   WMAERR_INVALIDARG         invalid pointers
 *   WMAERR_MOREDATA           need to provide next bytes starting
 *                             at *pdwOffset
 *   WMAERR_DRMFAIL            the DRM lib failed
 *
 */
tWMCDecStatus WMFDrmInitLicense(HWMCDECODER hDecoder,
                                U8_WMC *pbPMID,
                                U32_WMC dwPMID,
                                U8_WMC *pbLicData,
                                U32_WMC *pdwOffset,
                                U32_WMC *pdwLen);


tWMCDecStatus WMFDecodeInit_All (HWMCDECODER hDecoder,
                          U32_WMC u32VideoOutputPixelType,
                          U16_WMC u16VideoOutputBitCount
#ifdef _ZMDRM_
                          ,HDRMMGR *phDRM,
                          U8_WMC *pAppCert,
                          U32_WMC Rights,
                          const U8_WMC *pbHwId,
                          U32_WMC cbHwId
#endif
);

#ifdef __cplusplus
}
#endif


#endif  /* WMF_H */
