#include <xtl.h>
#include "samprate.h"

HRESULT XSrcCreateMediaObject( XMediaObject **ppMediaObject, PWAVEFORMATEX pWaveFormat )
{
	HRESULT hr;
	CSrcXMO* pXMO = new CSrcXMO();

	if ( NULL == pXMO )
		return E_OUTOFMEMORY;

	hr = pXMO->Initialize( pWaveFormat );

	*ppMediaObject = NULL;

	if ( SUCCEEDED( hr ) )
	{
		*ppMediaObject = pXMO;
		pXMO->AddRef();
	}

	return hr;
}


BOOL CSrcXMO::IsValidPcmFormat( LPCWAVEFORMATEX pwfx )
{
    if ( NULL == pwfx )
        return FALSE;

    if ( WAVE_FORMAT_PCM != pwfx->wFormatTag )
        return FALSE;

    if ( 1 != pwfx->nChannels )
        return FALSE;

    if ( 16 != pwfx->wBitsPerSample )
        return FALSE;

    if ( pwfx->wBitsPerSample * pwfx->nChannels / 8 != pwfx->nBlockAlign )
        return FALSE;

    if( pwfx->nSamplesPerSec * pwfx->nBlockAlign != pwfx->nAvgBytesPerSec )
        return FALSE;

    if ( pwfx->nSamplesPerSec != 8000 && pwfx->nSamplesPerSec != 16000 )
        return FALSE;

    return TRUE;
}

HRESULT CSrcXMO::Initialize( PWAVEFORMATEX pWaveFormat )
{
	if ( ! IsValidPcmFormat( pWaveFormat ) )
		return E_INVALIDARG;

	m_pWaveFormat = new WAVEFORMATEX;

	if ( NULL == m_pWaveFormat )
		return E_OUTOFMEMORY;

	memcpy( m_pWaveFormat, pWaveFormat, sizeof( WAVEFORMATEX ) );

	if ( 8000 == pWaveFormat->nSamplesPerSec )
	{
		pWaveFormat->nSamplesPerSec = 16000;
	}
	else if ( 16000 == pWaveFormat->nSamplesPerSec )
	{
		pWaveFormat->nSamplesPerSec = 8000;
	}

	pWaveFormat->nAvgBytesPerSec = pWaveFormat->nSamplesPerSec * pWaveFormat->nBlockAlign;

	return S_OK;
}
    
HRESULT STDMETHODCALLTYPE CSrcXMO::GetInfo( LPXMEDIAINFO pXMediaInfo )
{
	if ( NULL == pXMediaInfo )
		return E_POINTER;

    pXMediaInfo->dwFlags = XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_SINGLE_SAMPLE_PER_PACKET | XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_FIXED_PACKET_ALIGNMENT | XMO_STREAMF_IN_PLACE;

	if ( 0 == pXMediaInfo->dwInputSize && 0 == pXMediaInfo->dwOutputSize )
	{
		pXMediaInfo->dwInputSize = MIN_INPUT;
		pXMediaInfo->dwOutputSize = MIN_INPUT;
	}

	else if ( 0 == pXMediaInfo->dwInputSize )
	{
		pXMediaInfo->dwOutputSize = pXMediaInfo->dwOutputSize - pXMediaInfo->dwOutputSize % MIN_INPUT;
		pXMediaInfo->dwInputSize = pXMediaInfo->dwOutputSize;
	}

	else if ( 0 == pXMediaInfo->dwOutputSize )
	{
		pXMediaInfo->dwInputSize = pXMediaInfo->dwInputSize + pXMediaInfo->dwInputSize % MIN_INPUT;
		pXMediaInfo->dwOutputSize = pXMediaInfo->dwInputSize;
	}

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CSrcXMO::Process( LPCXMEDIAPACKET pSrcBuffer, LPCXMEDIAPACKET pDstBuffer )
{

	if ( NULL == pDstBuffer || NULL == pSrcBuffer )
		return E_POINTER;

	short*                  pSrc        = (short*)pSrcBuffer->pvBuffer;
    DWORD                   cSrc        = pSrcBuffer->dwMaxSize / 2;
    short*                  pDst        = (short*)pDstBuffer->pvBuffer;
    DWORD                   cDst        = pDstBuffer->dwMaxSize / 2;
	DWORD                   cSamples    = min( cSrc, cDst );
    HRESULT                 hr          = S_OK;

    if( pSrcBuffer->pdwCompletedSize )
    {
        pSrc += *pSrcBuffer->pdwCompletedSize;
        cSrc -= *pSrcBuffer->pdwCompletedSize;
    }

    if( pDstBuffer->pdwCompletedSize )
    {
        pDst += *pDstBuffer->pdwCompletedSize;
        cDst -= *pDstBuffer->pdwCompletedSize;
    }

	if ( 8000 == m_pWaveFormat->nSamplesPerSec )
	{
		while ( cSamples-- )
		{
			pDst[0] = pSrc[0];
			pDst[1] = pSrc[0];
			pSrc++;
			pDst += 2;
		}
	}
	else if ( 16000 == m_pWaveFormat->nSamplesPerSec )
	{
		while ( cSamples-- )
		{
			pDst[0] = pSrc[0];
			pDst++;
			pSrc += 2;
		}
	}

	if( pSrcBuffer->pdwCompletedSize )
    {
        (*pSrcBuffer->pdwCompletedSize) += cSrc;
    }

    if( pDstBuffer->pdwCompletedSize )
    {
        (*pDstBuffer->pdwCompletedSize) += cDst;
    }

	return hr;
}