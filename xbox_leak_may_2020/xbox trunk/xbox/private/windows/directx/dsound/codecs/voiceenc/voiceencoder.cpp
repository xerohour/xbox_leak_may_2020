/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       agcva1.cpp
 *  Content:    Concrete class that implements CAutoGainControl
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/01/99    pnewson Created it
 *  2/9/2001    georgioc re-created it in xmo format + turned it in standalone mini filter graph
 *
 ***************************************************************************/

/*

How this voice activation code works:

The idea is this. The power of the noise signal is pretty much constant over
time. The power of a voice signal varies considerably over time. The power of
a voice signal is not always high however. Weak frictive noises and such do not
generate much power, but since they are part of a stream of speech, they represent
a dip in the power, not a constant low power like the noise signal. We therefore 
associate changes in power with the presence of a voice signal.

If it works as expected, this will allow us to detect voice activity even
when the input volume, and therefore the total power of the signal, is very
low. This in turn will allow the auto gain control code to be more effective.

To estimate the power of the signal, we run the absolute value of the input signal
through a recursive digital low pass filter. This gives us the "envelope" signal.
[An alternative way to view this is a low frequency envelope signal modulated by a 
higher frequency carrier signal. We're extracting the low frequency envelope signal.]

*/

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
#include "voiceencoderp.h"

#include "xvocver.h"


EXTERN_C HRESULT WINAPI VoxwareCreateConverter(WORD wEncodeFormatTag, BOOL fCreateEncoder, LPVOICECODECWAVEFORMAT pwfxEncoded, LPWAVEFORMATEX pwfxDecoded, LPXMEDIAOBJECT *ppMediaObject);

#if DBG

    ULONG g_VeXmoDebugLevel = DEBUG_LEVEL_INFO;

#endif

XBOXAPI
EXTERN_C  HRESULT WINAPI
XVoiceEncoderCreateMediaObject(
    BOOL fAutoMode,
    DWORD dwCodecTag,
    LPWAVEFORMATEX pwfx,
    DWORD dwThreshold,
    LPXMEDIAOBJECT *ppXMO
    )
{
    XMediaObject *pXmo;
    HRESULT hr;

    pXmo = new CVoiceEncoderXmo();
    MY_ASSERT(pXmo);

    if (pXmo == NULL) {
        return E_OUTOFMEMORY;
    }

    hr = ((CVoiceEncoderXmo *)pXmo)->Init(
             fAutoMode,
             dwCodecTag,
             pwfx,
             dwThreshold);

    if (SUCCEEDED(hr)) {

        *ppXMO = pXmo;

    }

    return hr;

}



CVoiceEncoderXmo::~CVoiceEncoderXmo()
{

    if (m_pEncoderXmo) {

        m_pEncoderXmo->Release();
        m_pEncoderXmo = NULL;

    }

}



#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceEncoderXmo::Init"
//
// Init - initializes the AGC and VA algorithms, including loading saved
// values from registry.
//
// dwFlags - the dwFlags from the dvClientConfig structure
// guidCaptureDevice - the capture device we're performing AGC for
// plInitVolume - the initial volume level is written here
//
HRESULT CVoiceEncoderXmo::Init(
    DWORD fAutoMode, 
    DWORD dwCodecTag,
    PWAVEFORMATEX pwfx,
    DWORD dwSensitivity)
{

    MY_ASSERT(pwfx);

    m_fAutoMode = fAutoMode;
    m_dwSensitivity = dwSensitivity;
    m_dwSampleRate = pwfx->nSamplesPerSec;

    //
    // Figure out the shift constants for this sample rate
    //

    m_iShiftConstantFast = (DV_log_2((m_dwSampleRate * 2) / 1000) + 1);

    //
    // This gives the slow filter a cutoff frequency 1/4 of 
    // the fast filter
    //

    m_iShiftConstantSlow = m_iShiftConstantFast + 2;

    // Start the envelope signal at zero
    Flush();

   
    AddRef();

    //
    // instantiate a codec
    //

    //
    // instantiate the proper vox compressor
    //
    
    return VoxwareCreateConverter((WORD)dwCodecTag,
                                  TRUE,
                                  NULL,
                                  pwfx,
                                  &m_pEncoderXmo);

   
}

HRESULT STDMETHODCALLTYPE CVoiceEncoderXmo::Flush()
{


    // Start the envelope signal at zero
    m_iCurEnvelopeValueFast = 0;
    m_iCurEnvelopeValueSlow = 0;
    m_iHangoverSamples = 2;
    return S_OK;

}

HRESULT STDMETHODCALLTYPE CVoiceEncoderXmo::Process( 
    LPCXMEDIAPACKET pSrc,
    LPCXMEDIAPACKET pDst
    )
{
    int iMaxValue;
    int iValueAbs;
    int iIndex;
    int iMaxPossiblePeak;
    int iNumberOfSamples;
    PVOICE_ENCODER_HEADER pHeader;
    HRESULT hr = S_OK;

    DWORD fVoiceDetectedPrevFrame = m_fVoiceDetectedThisFrame;

    MY_ASSERT(pSrc);
    MY_ASSERT(pDst);

    MY_ASSERT(pSrc->dwMaxSize >= 2);
    MY_ASSERT(pDst->dwMaxSize >= m_dwMinOutputSize);

    MY_ASSERT(pDst->pvBuffer);
    MY_ASSERT(pSrc->pvBuffer);

	//
    // Compute the upper bound for number of sequences in a message
    //
	if( 0 == m_wMaxSeqInMsg )
    {
		//
        // Normal talkspurt is about 30s long
        // We'll consider the maximum talkpurt to be 3 minutes 
		//
		m_wMaxSeqInMsg = (WORD)(MAX_MSG_TIME / (pSrc->dwMaxSize));
    }		

    // cast the audio data to signed 16 bit integers
    signed short* psiAudioData = (signed short *)pSrc->pvBuffer;

    // 16 bits per sample assumed!!
    iNumberOfSamples = pSrc->dwMaxSize / 2;

    //
    // BUGBUG the voice peripheral certainly regulates the volume so the max value is never above 600
    // we got to figure this out tho
    //

    iMaxPossiblePeak = 500;
    iMaxValue = 0;
    m_fVoiceDetectedThisFrame = FALSE;

    for (iIndex = 0; iIndex < (int)iNumberOfSamples; ++iIndex)
    {
        iValueAbs = DV_ABS((int)psiAudioData[iIndex]);

        // see if it is the new peak value
        iMaxValue = DV_MAX(iValueAbs, iMaxValue);

        // do the low pass filtering, but only if we are in autosensitivity mode
        int iNormalizedCurEnvelopeValueFast;
        int iNormalizedCurEnvelopeValueSlow;
        if (m_fAutoMode)
        {
            m_iCurEnvelopeValueFast = 
                iValueAbs + 
                (m_iCurEnvelopeValueFast - (m_iCurEnvelopeValueFast >> m_iShiftConstantFast));
            iNormalizedCurEnvelopeValueFast = m_iCurEnvelopeValueFast >> m_iShiftConstantFast;

            m_iCurEnvelopeValueSlow = 
                iValueAbs + 
                (m_iCurEnvelopeValueSlow - (m_iCurEnvelopeValueSlow >> m_iShiftConstantSlow));
            iNormalizedCurEnvelopeValueSlow = m_iCurEnvelopeValueSlow >> m_iShiftConstantSlow;

            // check to see if we consider this voice
            if (iNormalizedCurEnvelopeValueFast > VA_LOW_ENVELOPE &&
                (iNormalizedCurEnvelopeValueFast > VA_HIGH_ENVELOPE ||
                CALC_HIGH_PERCENT_RANGE( iNormalizedCurEnvelopeValueSlow ) ||
                CALC_LOW_PERCENT_RANGE( iNormalizedCurEnvelopeValueSlow ) ) )
            {
                m_fVoiceDetectedThisFrame = TRUE;
                m_iCurHangoverSamples = 0;
            }
            else
            {
                ++m_iCurHangoverSamples;
                if (m_iCurHangoverSamples > m_iHangoverSamples)
                {
                    m_fVoiceDetectedThisFrame = FALSE;
                }
                else
                {
                    m_fVoiceDetectedThisFrame = TRUE;
                }
            }
        }
    }

    // Normalize the peak value to the range DVINPUTLEVEL_MIN to DVINPUTLEVEL_MAX
    // This is what is returned for caller's peak meters...
    m_bPeak = (BYTE)(VE_INPUTLEVEL_MIN + 
        ((iMaxValue * (VE_INPUTLEVEL_MAX - VE_INPUTLEVEL_MIN)) / iMaxPossiblePeak));

    //
    // if we are in manual VA mode (not autovolume) check the peak against
    // the sensitivity threshold
    //

    if (!m_fAutoMode)
    {
        if (m_bPeak > m_dwSensitivity)
        {
            m_fVoiceDetectedThisFrame = TRUE;
            m_iCurHangoverSamples = 0;
        } 
		else 
		{
                ++m_iCurHangoverSamples;
                if (m_iCurHangoverSamples > m_iHangoverSamples)
                {
                    m_fVoiceDetectedThisFrame = FALSE;
                }
                else
                {
                    m_fVoiceDetectedThisFrame = TRUE;
                }
        }
    }

    //
    // now that we have performed the VA, create a packet header 
    //

    pHeader = (PVOICE_ENCODER_HEADER) pDst->pvBuffer;
    pHeader->bType = (m_fVoiceDetectedThisFrame ? VOICE_MESSAGE_TYPE_VOICE : VOICE_MESSAGE_TYPE_SILENCE);

    if ((!m_fVoiceDetectedThisFrame && fVoiceDetectedPrevFrame) ||
		 (m_wCurrentMsgSeqCount > m_wMaxSeqInMsg))
	{

        pHeader->bMsgNum = m_bMsgNum++;
		m_wCurrentMsgSeqCount = 0;

    }
	else
	{
	    pHeader->bMsgNum = m_bMsgNum;
	}

    pHeader->wSeqNum = m_wSeqNum++;
	m_wCurrentMsgSeqCount++;

    //
    // now invoke the encoder XMO to compress the data
    //

    if (m_fVoiceDetectedThisFrame) {

        XMEDIAPACKET * pXmp;

        pXmp = (XMEDIAPACKET *)pDst;

        //
        // change the destination packet to not stomp on the voice header we just inserted
        //

        pXmp->pvBuffer = (PUCHAR)pHeader + sizeof(VOICE_ENCODER_HEADER);

        hr = m_pEncoderXmo->Process(pSrc,
                                    pDst);


        //
        // re adjust destination packet buffer and size
        //

        pXmp->pvBuffer = pHeader;
  
        //
        // the intermediate codec XMO has already updated pdwstatus, etc
        //

		if (pDst->pdwCompletedSize) {

            *pDst->pdwCompletedSize += sizeof(VOICE_ENCODER_HEADER);

        }

    } else {

        //
        // if voice was not detected we just send a zeroed packet
        //

		memset(pDst->pvBuffer, 0 , pDst->dwMaxSize);

        if (pDst->pdwCompletedSize) {

            *pDst->pdwCompletedSize = 0;
        }

        if (pDst->pdwStatus) {

            *pDst->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;

        }

    }

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


HRESULT STDMETHODCALLTYPE CVoiceEncoderXmo::GetInfo( LPXMEDIAINFO pInfo )
{
    HRESULT hr;

    MY_ASSERT(pInfo);

    //
    // use the info from the encoder xmo
    //
    hr = m_pEncoderXmo->GetInfo(pInfo);
    if (FAILED(hr)) {
        return hr;
    }


	//
    // our minimum output size has to account for the header
    //
    pInfo->dwOutputSize += sizeof(VOICE_ENCODER_HEADER); 
    m_dwMinOutputSize = pInfo->dwOutputSize;

    return S_OK;
}
                                                   
HRESULT STDMETHODCALLTYPE CVoiceEncoderXmo::Discontinuity(void){
   return S_OK;
}

HRESULT STDMETHODCALLTYPE CVoiceEncoderXmo::GetStatus( 
    LPDWORD pdwStatus
    )
{

    if (pdwStatus) {

        *pdwStatus = XMO_STATUSF_ACCEPT_OUTPUT_DATA | XMO_STATUSF_ACCEPT_INPUT_DATA;

    }

    return S_OK;
}

