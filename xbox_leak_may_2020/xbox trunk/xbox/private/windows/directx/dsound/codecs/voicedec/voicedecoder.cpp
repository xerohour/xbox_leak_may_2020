/*==========================================================================
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/20/2001 georgioc created
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include "dsoundp.h"

#include "voiceencoder.h"
#include "voicedecoderp.h"

#include "xvocver.h"


EXTERN_C HRESULT WINAPI VoxwareCreateConverter(WORD wEncodeFormatTag, BOOL fCreateEncoder, LPVOICECODECWAVEFORMAT pwfxEncoded, LPWAVEFORMATEX pwfxDecoded, LPXMEDIAOBJECT *ppMediaObject);

#if DBG

    ULONG g_VdXmoDebugLevel = DEBUG_LEVEL_INFO;

#endif

XBOXAPI
EXTERN_C  HRESULT WINAPI
XVoiceDecoderCreateMediaObject(
    DWORD dwCodecTag,
    PWAVEFORMATEX pwfx,
    XMediaObject **ppXMO
    )
{
    XMediaObject *pXmo;
    HRESULT hr;

    pXmo = new CVoiceDecoderXmo();
    MY_ASSERT(pXmo);

    if (pXmo == NULL) {
        return E_OUTOFMEMORY;
    }

    hr = ((CVoiceDecoderXmo *)pXmo)->Init(
             dwCodecTag,
             pwfx);

    if (SUCCEEDED(hr)) {

        *ppXMO = pXmo;

    }

    return hr;

}



CVoiceDecoderXmo::~CVoiceDecoderXmo()
{

    if (m_pDecoderXmo) {

        m_pDecoderXmo->Release();
        m_pDecoderXmo = NULL;

    }

}

HRESULT CVoiceDecoderXmo::Init(
    DWORD dwCodecTag,
    PWAVEFORMATEX pwfx)
{

    MY_ASSERT(pwfx);


    AddRef();

    //
    // instantiate the proper vox compressor
    //
    
    return VoxwareCreateConverter((WORD)dwCodecTag,
                                  FALSE,
                                  NULL,
                                  pwfx,
                                  &m_pDecoderXmo);

    
}

HRESULT STDMETHODCALLTYPE CVoiceDecoderXmo::Flush()
{

    return S_OK;

}

HRESULT STDMETHODCALLTYPE CVoiceDecoderXmo::Process( 
    LPCXMEDIAPACKET pSrc,
    LPCXMEDIAPACKET pDst
    )
{
    PVOICE_ENCODER_HEADER pHeader;
    HRESULT hr = S_OK;

    MY_ASSERT(pSrc);
    MY_ASSERT(pDst);

    MY_ASSERT(pSrc->dwMaxSize >= m_dwMinInputSize);
    MY_ASSERT(pDst->dwMaxSize >= m_dwMinOutputSize);

    MY_ASSERT(pDst->pvBuffer);
    MY_ASSERT(pSrc->pvBuffer);

    //
    // get packet header form source packet
    //

    pHeader = (PVOICE_ENCODER_HEADER) pSrc->pvBuffer;
    

    //
    // now invoke the decoder XMO to decompress the data
    //

    if (pHeader->bType == VOICE_MESSAGE_TYPE_VOICE) {

        XMEDIAPACKET * pXmp;
        MY_ASSERT(pSrc->dwMaxSize >= sizeof(VOICE_ENCODER_HEADER));

        pXmp = (XMEDIAPACKET *)pSrc;

        //
        // change the src packet buffer so the decoder does not try to munge the header
        //

        pXmp->pvBuffer = (PUCHAR)pHeader + sizeof(VOICE_ENCODER_HEADER);
        pXmp->dwMaxSize -= sizeof(VOICE_ENCODER_HEADER);

        hr = m_pDecoderXmo->Process(pSrc,
                                    pDst);


        //
        // re adjust src packet buffer and size
        //

        pXmp->pvBuffer = (PUCHAR)pHeader;
        pXmp->dwMaxSize += sizeof(VOICE_ENCODER_HEADER);

        //
        // the intermediate codec XMO has already updated pdwstatus, etc
        // 


    } else {

        //
        // if not voice was send, return zero
        //

        if (pDst->pdwCompletedSize) {

            *pDst->pdwCompletedSize = 0;

        }

        if (pDst->pdwStatus) {

            *pDst->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;

        }

    }

    //
    // set input packet results
    //

    if (pSrc->pdwCompletedSize) {

        *pSrc->pdwCompletedSize = pSrc->dwMaxSize;

    }

    if (pSrc->pdwStatus) {

        if (FAILED(hr)) {

            *pSrc->pdwStatus = XMEDIAPACKET_STATUS_FAILURE;

        } else {

            *pSrc->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;

        }
        
    }

    return hr;
}


HRESULT STDMETHODCALLTYPE CVoiceDecoderXmo::GetInfo( LPXMEDIAINFO pInfo )
{
    HRESULT hr;
    MY_ASSERT(pInfo);

    //
    // use the info from the encoder xmo
    //

    hr = m_pDecoderXmo->GetInfo(pInfo);
    if (FAILED(hr)) {
        return hr;
    }

    //
    // our minimum size has to account for the header
    //
    pInfo->dwInputSize += sizeof(VOICE_ENCODER_HEADER); 
    m_dwMinOutputSize = pInfo->dwOutputSize;

    return S_OK;
}
                                                   
HRESULT STDMETHODCALLTYPE CVoiceDecoderXmo::Discontinuity(void){
   return S_OK;
}

HRESULT STDMETHODCALLTYPE CVoiceDecoderXmo::GetStatus( 
    LPDWORD pdwStatus
    )
{

    if (pdwStatus) {

        *pdwStatus = XMO_STATUSF_ACCEPT_OUTPUT_DATA | XMO_STATUSF_ACCEPT_INPUT_DATA;

    }

    return S_OK;
}

