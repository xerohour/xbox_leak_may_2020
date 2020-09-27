#include <stdlib.h>
#include <stdio.h>
#include "eqp.h"
#include "eq.h"


const int nFcWidth	= 16;

const int nFcPrec	= 16;
const int nQcPrec	= 15;
const int nGPrec	= 13;
const int nFltSmoothPrec=nFcPrec+5;
const int nDataPrec=23;

const float fOneOverSampleRate = 1.0f/48000.0f;
const float fPi = 3.14159265f;
void DlsToChamberlin(float fCutoff,float fResonance,unsigned int *pF,unsigned int *pQ);

//
// implements a simple synchronous XMO and in-place
//

HRESULT
XCreateParametricEQMediaObject(
    XMediaObject **ppMediaObject,
    PWAVEFORMATEX pwfx,
    float fCutoff,
    float fResonance
    )
{   
    HRESULT hr;
    CEQXMO *pXmo;

    //
    // create net xmo
    //

    pXmo = new CEQXMO();
    ASSERT(pXmo);
    if (pXmo == NULL) {
        return E_OUTOFMEMORY;
    }

    hr = pXmo->Initialize(pwfx,
                          fCutoff,
                          fResonance);


    *ppMediaObject = NULL;

    if (SUCCEEDED(hr)) {
        *ppMediaObject = pXmo;
        pXmo->AddRef();
    }

    return hr;

}

HRESULT CEQXMO::Initialize(
    PWAVEFORMATEX pwfx,
    float fCutoff,
    float fResonance
    )
{

    ASSERT(pwfx->wFormatTag == WAVE_FORMAT_PCM);
    memcpy(&m_wfx,pwfx,sizeof(WAVEFORMATEX));

    //
    // convert cutoff and resonance values to chamberlin low pass filter params F_tp and Q_tp
    //

    DlsToChamberlin(fCutoff,fResonance,&m_F,&m_Q);

    return S_OK;

}

long FTOL(float ftmp1)
{
	unsigned long lTemp1;
	__asm
	{
		fld	ftmp1
		fistp lTemp1
	}
	return lTemp1;
}

//
// This is a simple approximation of the mapping from DLS2 filter
//   descriptors, Cutoff in Hz and Resonance in dB, to Chamberlin
//   filter coefficients, F and Q.  The approximation is close
//   for small Cutoffs (< 6kHz) but breaks down for larger cutoffs.
//   The function is consistent in that F and Q are monotonic with
//   Cutoff and Resonance, respectively.  Output filter coefficients
//   are suitable conditioned for use in the apu.
//
void DlsToChamberlin(float fCutoff,float fResonance,unsigned int *pF,unsigned int *pQ)
{

	if(fResonance<0)
		fResonance = 0;
	if(fResonance>22.5f)
		fResonance = 22.5f;

	float fNormCutoff = fCutoff*fOneOverSampleRate;

	if(fNormCutoff<0.0f) fNormCutoff = 0.0f;
	if(fNormCutoff>0.5)	fNormCutoff = 0.5f;

	// The approximation
    float fFc = (float)(2.0*sin(fPi*fNormCutoff));
    float fQc = (float)pow(10.0,-0.05*fResonance);

	// Limit and convert to integer format
	*pF = (unsigned int)FTOL(fFc*(1<<nFcPrec));
	if(*pF>((1<<nFcWidth)-1))
		*pF = (1<<nFcWidth)-1;
	*pQ = (unsigned int)FTOL(fQc*(1<<nQcPrec));
	if(*pQ>((1<<nFcWidth)-1))
		*pQ = (1<<nFcWidth)-1;

	// Still need to limit on the low side

	return;
}


HRESULT CEQXMO::SetParameters(
    float fCutoff,
    float fResonance
    )

{

    //
    // convert cutoff and resonance values to chamberlin low pass filter params F_tp and Q_tp
    //

    DlsToChamberlin(fCutoff,fResonance,&m_F,&m_Q);

    return S_OK;
}


HRESULT STDMETHODCALLTYPE CEQXMO::GetInfo( 
    PXMEDIAINFO pInfo
    )
{
    ASSERT(pInfo);

    //
    // we are a synchronous, in-place XMO
    //

    pInfo->dwFlags = XMO_STREAMF_IN_PLACE | XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE |
        XMO_STREAMF_FIXED_PACKET_ALIGNMENT;

    //
    // require at least 4 blocks of sound samples
    // 

    pInfo->dwInputSize = m_wfx.nBlockAlign*4;
    pInfo->dwOutputSize = m_wfx.nBlockAlign*4;

    return S_OK;

}

HRESULT STDMETHODCALLTYPE CEQXMO::GetStatus( 
        /* [out] */ DWORD __RPC_FAR *pdwFlags
        )
{

    //
    // let the caller know if we can accept any more data
    //

    ASSERT(pdwFlags);
    *pdwFlags = XMO_STATUSF_ACCEPT_INPUT_DATA;
    return S_OK;

}


HRESULT STDMETHODCALLTYPE CEQXMO::Process( 
    const XMEDIAPACKET __RPC_FAR *pSrcBuffer,
    const XMEDIAPACKET __RPC_FAR *pDstBuffer
    )
{
    SHORT history[4];
    PSHORT srcBuffer,dstBuffer;
    DWORD dwSize;

    //
    // we need both a destination and source buffer to operate
    //

    ASSERT(pSrcBuffer);
    ASSERT(pDstBuffer);

    //
    // make sure we have enough data
    //

    ASSERT(pSrcBuffer->dwMaxSize >= (DWORD)m_wfx.nBlockAlign*4);
    ASSERT(pDstBuffer->dwMaxSize >= (DWORD)m_wfx.nBlockAlign*4);

    //
    // ensure both buffers are the same size
    //

    ASSERT(pSrcBuffer->dwMaxSize == pDstBuffer->dwMaxSize);

    ASSERT(pSrcBuffer->pvBuffer);
    ASSERT(pDstBuffer->pvBuffer);

    dwSize = pDstBuffer->dwMaxSize/(m_wfx.wBitsPerSample >> 3);
    dstBuffer = (PSHORT) pDstBuffer->pvBuffer;
    srcBuffer = (PSHORT) pSrcBuffer->pvBuffer;

    //
    // if the stream is stereo we have to pass each channel through the filter in parallel.
    // If the stream is mono we need to cascade the filter (2 in a row)
    //

    if (m_wfx.nChannels == 1) {

        //
        // save the last 2 samples of this packet in case the SrcBuffer and DstBuffer are the same
        //
    
        history[0] = srcBuffer[dwSize-3];
        history[1] = srcBuffer[dwSize-2];

        //
        // mono processing
        //

        ChamberlinFilter(srcBuffer,
                         dstBuffer,
                         dwSize,
                         1,
                         0);

        //
        // second pass
        //

        ChamberlinFilter(dstBuffer,
                         dstBuffer,
                         dwSize,
                         1,
                         0);

        //
        // cache the last two samples of this packet
        //
    
        m_PreviousSamples[0] = history[0];
        m_PreviousSamples[1] = history[1];

    } else {

        ULONG i;

        for (i=0;i<EQ_XMO_REQUIRED_SAMPLES;i++) {

            history[i] = m_PreviousSamples[i];

        }

        //
        // stereo processing. Do left channel first
        //

        ChamberlinFilter(srcBuffer,
                         dstBuffer,
                         pDstBuffer->dwMaxSize/(m_wfx.wBitsPerSample >> 3),
                         2,
                         0);

        //
        // right channel
        //

        ChamberlinFilter(srcBuffer,
                         dstBuffer,
                         pDstBuffer->dwMaxSize/(m_wfx.wBitsPerSample >> 3),
                         2,
                         1);

        //
        // cache the last two samples of this packet
        //
    
        for (i=0;i<EQ_XMO_REQUIRED_SAMPLES;i++) {

            m_PreviousSamples[i] = history[i];

        }


    }


    if (pDstBuffer->pdwCompletedSize) {
        *pDstBuffer->pdwCompletedSize = pSrcBuffer->dwMaxSize;
    }

    if (pDstBuffer->pdwStatus) {
        *pSrcBuffer->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
    }

    if (pSrcBuffer->pdwCompletedSize) {
        *pDstBuffer->pdwCompletedSize = pSrcBuffer->dwMaxSize;
    }

    if (pSrcBuffer->pdwStatus) {
        *pSrcBuffer->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
    }

    return S_OK;

}

VOID CEQXMO::ChamberlinFilter( 
    PSHORT srcBuffer,
    PSHORT dstBuffer,
    ULONG nStep,
    ULONG nLength,
    ULONG nOffset
    )
{
    float fN, fD, fTemp, fF = (float)m_F, fQ = (float)m_Q, fPrevSample;

    //
    // divide nLength (in samples) by nStep (also in samples)
    //

    nLength /= nStep;

    //
    // implement the modified chamberlin filter (boost/cut filter)
    // the index counter i is in samples
    //

    for (ULONG i=nLength-nStep;i>=0;i-=nStep) {

        //
        // numerator
        //

        if (i==nStep) {

            fPrevSample = (float)srcBuffer[i*nStep-nStep*1 +nOffset];

        } else {

            fPrevSample = (float)m_PreviousSamples[1*nStep+nOffset];

        }
        
        fN = fF*fF*fPrevSample;

        // 
        // denominator
        //

        fTemp = 1 - (2-fF*fQ - fF*fF)*fPrevSample;

        //
        // get 2 samples back
        //

        if (i>nStep) {

            fPrevSample = (float)srcBuffer[i*nStep-2*nStep +nOffset];

        } else {

            fPrevSample = (float)m_PreviousSamples[0+nOffset];

        }

        fD = fTemp + (1 - fF*fQ)*fPrevSample;

        //
        // transfer function result
        //

        fTemp = fN/fD;

        //
        // convert to SHORT

        dstBuffer[i*nStep+nOffset] = (SHORT)fTemp;
    }
}
