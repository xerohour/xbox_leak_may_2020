// CWaveCompressionManager Implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "WaveNode.h"
#include "wave.h"
#include "WaveCompressionManager.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#define _CRTDBG_MAP_ALLOC
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// 

/*****************************************************
// Constructs a Compression Manager for a Wave object
******************************************************/
CWaveCompressionManager::CWaveCompressionManager():
	m_bGotDrivers(false)
{}

CWaveCompressionManager::~CWaveCompressionManager()
{
	ClearFormatCollection();
	ClearDriverCollection();
}

/*******************************************************
// Gets all the supported drivers for the wave object.
// The drivers are collected in a CArray object.
********************************************************/
BOOL CWaveCompressionManager::GetSupportedDrivers()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (m_bGotDrivers)
		return TRUE;
	m_bGotDrivers = true;

	CWaitCursor cursor;
	MMRESULT mmr = acmDriverEnum(acmDriverEnumCallback, (DWORD)this, 0);
	
	if(mmr != MMSYSERR_NOERROR)
	{
		TRACE0("Failed to load one or more ACM drivers");
		return FALSE;
	}
	return TRUE;
}

/*******************************************************
// Gets the suported audio formats for a driver
********************************************************/
// The index must map to a valid index in the collection of supported drivers.
typedef struct _FormatEnumCallbackInfo {
	CWave *pWave;
	CTypedPtrArray<class CPtrArray,struct _SUPPORTED_FORMAT_INFO *> *parrSupportedFormats;
} FormatEnumCallbackInfo;


#ifdef DMP_XBOX
#define IMAADPCM_BITS_PER_SAMPLE        4
#define IMAADPCM_HEADER_LENGTH          4
#define XBOX_ADPCM_SAMPLES_PER_BLOCK    64

/****************************************************************************
 *
 *  CalculateEncodeAlignment
 *
 *  Description:
 *      Calculates an encoded data block alignment based on a PCM sample
 *      count and an alignment multiplier.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      WORD [in]: PCM samples per block.
 *
 *  Returns:  
 *      WORD: alignment, in bytes.
 *
 ****************************************************************************/

WORD CalculateEncodeAlignment
(
    WORD                    nChannels
)
{
    const WORD              nEncodedSampleBits  = nChannels * IMAADPCM_BITS_PER_SAMPLE;
    const WORD              nHeaderBytes        = nChannels * IMAADPCM_HEADER_LENGTH;
    WORD                    nBlockAlign;

    //
    // Calculate the raw block alignment that XBOX_ADPCM_SAMPLES_PER_BLOCK dictates.  This
    // value may include a partial encoded sample, so be sure to round up.
    //
    // Start with the samples-per-block, minus 1.  The first sample is actually
    // stored in the header.
    //

    nBlockAlign = XBOX_ADPCM_SAMPLES_PER_BLOCK - 1;

    //
    // Convert to encoded sample size
    //

    nBlockAlign *= nEncodedSampleBits;
    nBlockAlign += 7;
    nBlockAlign /= 8;

    //
    // The stereo encoder requires that there be at least two DWORDs to process
    //

    nBlockAlign += 7;
    nBlockAlign /= 8;
    nBlockAlign *= 8;

    //
    // Add the header
    //

    nBlockAlign += nHeaderBytes;

    return nBlockAlign;
}

void CreateXboxAdpcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    LPIMAADPCMWAVEFORMAT    pwfx
)
{
    pwfx->wfx.wFormatTag = XBOXADPCM_FORMAT_TAG;
    pwfx->wfx.nChannels = nChannels;
    pwfx->wfx.nSamplesPerSec = nSamplesPerSec;
    pwfx->wfx.nBlockAlign = CalculateEncodeAlignment(nChannels);
    pwfx->wfx.nAvgBytesPerSec = nSamplesPerSec * pwfx->wfx.nBlockAlign / XBOX_ADPCM_SAMPLES_PER_BLOCK;
    pwfx->wfx.wBitsPerSample = IMAADPCM_BITS_PER_SAMPLE;
    pwfx->wfx.cbSize = sizeof(*pwfx) - sizeof(pwfx->wfx);
    pwfx->wSamplesPerBlock = XBOX_ADPCM_SAMPLES_PER_BLOCK;
}

#endif // DMP_XBOX

/* fills m_arrSupportedFormats with formats supported by given driver, and for given wave */
void CWaveCompressionManager::GetSupportedFormats(const FORMAT_TAG_INF0* pFormatInfo, CWave *pWave)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_bGotDrivers);
	ASSERT(pWave);

	ASSERT(pFormatInfo != (const void *)-1);
	ASSERT(pFormatInfo != NULL);

	// Clear all the previously collected formats
	ClearFormatCollection();
	
	ACMFORMATDETAILS afd;
	ZeroMemory(&afd, sizeof(ACMFORMATDETAILS));
	afd.cbStruct = sizeof(afd);

	DWORD dwMaxFormatSize = 0;
	afd.dwFormatTag = pFormatInfo->dwFormatTag;

	MMRESULT mmr = acmMetrics((HACMOBJ)(pFormatInfo->hadid), ACM_METRIC_MAX_SIZE_FORMAT, &dwMaxFormatSize);

	HACMDRIVER had = NULL;
	if(mmr == MMSYSERR_NOERROR)
	{
		mmr = acmDriverOpen(&had, pFormatInfo->hadid, 0);
	}

	if(mmr == MMSYSERR_NOERROR)
	{
		afd.cbwfx = dwMaxFormatSize;
		afd.pwfx = (WAVEFORMATEX *) new	BYTE[dwMaxFormatSize];
		if(afd.pwfx)
		{
			ZeroMemory(afd.pwfx, dwMaxFormatSize);
			afd.dwFormatTag = (DWORD) (pFormatInfo->dwFormatTag);
			afd.pwfx->wFormatTag = (WORD) (pFormatInfo->dwFormatTag);
			
			FormatEnumCallbackInfo feci;
			feci.pWave = pWave;
			feci.parrSupportedFormats = &m_arrSupportedFormats;

			acmFormatEnum(had, &afd, acmFormatEnumCallback, (DWORD)&feci, ACM_FORMATENUMF_WFORMATTAG);
			delete[] afd.pwfx;
		}

#ifdef DMP_XBOX
		// If there are no supported formats, check to see if the wave's current sample rate is supported
		if( m_arrSupportedFormats.GetSize() == 0
		&&	XBOXADPCM_FORMAT_TAG == pFormatInfo->dwFormatTag )
		{
			// Source format
			LPWAVEFORMATEX pwfxSrc = &(pWave->m_rWaveformat);

			// Destination format
			IMAADPCMWAVEFORMAT wfxDst;
			ZeroMemory( &wfxDst, sizeof(IMAADPCMWAVEFORMAT) );

			// We don't want stereo formats if the wave is not a TRUE stereo wave
			// But we want the stereo formats if it's a stereo wave NOT in a collection
			bool bIsStereo = pWave->GetNode()->IsStereo();
			CCollection* pCollection = pWave->GetNode()->GetCollection();

			CreateXboxAdpcmFormat( (bIsStereo && pCollection == NULL) ? 2 : 1, pwfxSrc->nSamplesPerSec, &wfxDst );

			MMRESULT mmr = acmStreamOpen(NULL, NULL, pwfxSrc, (WAVEFORMATEX *)(&wfxDst), NULL, 0, 0, ACM_STREAMOPENF_QUERY);
			if(mmr == MMSYSERR_NOERROR)
			{
				SUPPORTED_FORMAT_INFO* pSupportedFormat = new SUPPORTED_FORMAT_INFO;

				if( wfxDst.wfx.nChannels == 1 )
				{
					pSupportedFormat->sSupportedFormatName.FormatMessage( IDS_XBADPCM_MONO, wfxDst.wfx.nSamplesPerSec );
				}
				else
				{
					pSupportedFormat->sSupportedFormatName.FormatMessage( IDS_XBADPCM_STEREO, wfxDst.wfx.nSamplesPerSec );
				}
				
				// Allocate exactly what size is required for this WAVEFORMATEX struct
				int nExtraBytes = wfxDst.wfx.cbSize;

				pSupportedFormat->pWaveFormatEx = (WAVEFORMATEX*) new BYTE[sizeof(WAVEFORMATEX) + nExtraBytes];
				CopyMemory(pSupportedFormat->pWaveFormatEx, &wfxDst, sizeof(WAVEFORMATEX) + nExtraBytes);

				m_arrSupportedFormats.Add(pSupportedFormat);
			}
		}
#endif // DMP_XBOX

		acmDriverClose(had, 0);
	}
}


/****************************************************************
// Gets the suported audio formats for a driver for a Format Tag
*****************************************************************/
BOOL CWaveCompressionManager::GetSupportedFormats(WORD wFormatTag, CWave *pWave)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_bGotDrivers);

	int nDrivers = m_arrFormatTags.GetSize();

	for(int nCount = 0; nCount < nDrivers; nCount++)
	{
		if(m_arrFormatTags[nCount]->dwFormatTag == wFormatTag)
		{
			GetSupportedFormats(m_arrFormatTags[nCount], pWave);
			return TRUE;
		}
	}

	return FALSE;
}


/*******************************************************
// Deletes all the pointers from the format collection
********************************************************/
void CWaveCompressionManager::ClearFormatCollection()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	while(m_arrSupportedFormats.GetSize() > 0)
	{
		SUPPORTED_FORMAT_INFO* pSupportedFormatInfo = (SUPPORTED_FORMAT_INFO*)m_arrSupportedFormats[0];
		m_arrSupportedFormats.RemoveAt(0);
		
		if(pSupportedFormatInfo->pWaveFormatEx)
			delete pSupportedFormatInfo->pWaveFormatEx;
		delete  pSupportedFormatInfo;
		pSupportedFormatInfo = NULL;
	}
}

/*******************************************************
// Deletes all the pointers from the drivers collection
********************************************************/
void CWaveCompressionManager::ClearDriverCollection()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	while(m_arrFormatTags.GetSize() > 0)
	{
		FORMAT_TAG_INF0* pDriverInfo = (FORMAT_TAG_INF0*)m_arrFormatTags[0];
		m_arrFormatTags.RemoveAt(0);
		delete pDriverInfo;
		pDriverInfo = NULL;
	}

	while(m_arrPCMFormatTags.GetSize() > 0)
	{
		FORMAT_TAG_INF0* pDriverInfo = (FORMAT_TAG_INF0*)m_arrPCMFormatTags[0];
		m_arrPCMFormatTags.RemoveAt(0);
		delete pDriverInfo;
		pDriverInfo = NULL;
	}
}


BOOL CALLBACK CWaveCompressionManager::acmDriverEnumCallback(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	HACMDRIVER had = NULL;
	MMRESULT mmr1 = acmDriverOpen(&had, hadid, 0);
	MMRESULT mmr2 = MMSYSERR_NOERROR; 					
	
	if(mmr1 == MMSYSERR_NOERROR)
	{
		mmr1 = EnumFormatTags(&had, dwInstance);
		mmr2 = acmDriverClose(had, 0);
	}
	
	if(mmr1 != MMSYSERR_NOERROR || mmr2 != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	return TRUE;
}

MMRESULT CWaveCompressionManager::EnumFormatTags(HACMDRIVER *phad, DWORD dwInstance)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(phad);

	ACMFORMATTAGDETAILS aftd;
	ZeroMemory(&aftd, sizeof(ACMFORMATTAGDETAILS));
	aftd.cbStruct = sizeof(aftd);
	
	MMRESULT mmr = acmFormatTagEnum(*phad, &aftd, CWaveCompressionManager::acmFormatTagEnumCallback, dwInstance,	0);

	return mmr;
}

BOOL CALLBACK CWaveCompressionManager::acmFormatTagEnumCallback(HACMDRIVERID hadid, LPACMFORMATTAGDETAILS paftd,  
									   DWORD dwInstance, DWORD fdwSupport)

{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(paftd);

	CWaveCompressionManager* pCompressionManager = (CWaveCompressionManager*)dwInstance;
	ASSERT(pCompressionManager);
	if (NULL == pCompressionManager)
	{
		return FALSE;
	}

	FORMAT_TAG_INF0* pfi = new FORMAT_TAG_INF0;
	
	if(pfi == NULL)
	{
		return FALSE;
	}

	pfi->sDriverName = paftd->szFormatTag;
	pfi->hadid = hadid;
	pfi->dwFormatTag = paftd->dwFormatTag;

#ifdef DMP_XBOX
	// We only support Xbox ADPCM
	if(paftd->dwFormatTag != XBOXADPCM_FORMAT_TAG)
	{
		delete pfi;
		return TRUE;
	}
#endif // DMP_XBOX

	// We do not want PCM in our list
	if(lstrcmp(paftd->szFormatTag, "PCM") == 0)
	{
		pCompressionManager->m_arrPCMFormatTags.Add(pfi);
	}
	else
	{
		pCompressionManager->m_arrFormatTags.Add(pfi);
	}

	if(paftd->dwFormatTag == MSAUDIO_FORMAT_TAG)
    {
		pCompressionManager->m_hMSAudioV1BetaDriverID = hadid;
    }
	
	return TRUE;
}

BOOL CALLBACK CWaveCompressionManager::acmFormatEnumCallback(HACMDRIVERID hadid,
	LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pafd);
	FormatEnumCallbackInfo *pfeci = (FormatEnumCallbackInfo *)dwInstance;
	ASSERT(pfeci);
	
	if (pfeci->pWave == NULL)
		return FALSE;

	LPWAVEFORMATEX pwfxSrc = &((pfeci->pWave)->m_rWaveformat);
	LPWAVEFORMATEX pwfxDst = pafd->pwfx;

	// We don't want stereo formats if the wave is not a TRUE stereo wave
	// But we want the stereo formats if it's a stereo wave NOT in a collection
	CWaveNode* pWaveNode = pfeci->pWave->GetNode();
	ASSERT(pWaveNode);
	CCollection* pCollection = pWaveNode->GetCollection();
	bool bIsStereo = pWaveNode->IsStereo();

	if((bIsStereo == false || pCollection) && pwfxDst->nChannels > 1)
		return TRUE;
	
	// If this is a TRUE stereo wave use only stereo wave formats
	if(bIsStereo && pCollection == NULL && pwfxDst->nChannels != 2)
		return TRUE;

	if(pwfxDst->wFormatTag != MSAUDIO_FORMAT_TAG && pwfxDst->wFormatTag != WMAUDIO_FORMAT_TAG)
	{
		MMRESULT mmr = acmStreamOpen(NULL, NULL, pwfxSrc, pwfxDst, NULL, 0, 0, ACM_STREAMOPENF_QUERY);
		if(mmr != MMSYSERR_NOERROR)
		{
			if(mmr == ACMERR_NOTPOSSIBLE)
			{
				return TRUE;
			}
			return FALSE;
		}
	}

	SUPPORTED_FORMAT_INFO* pSupportedFormat = new SUPPORTED_FORMAT_INFO;

	pSupportedFormat->sSupportedFormatName = pafd->szFormat;
	
	// Allocate exactly what size is required for this WAVEFORMATEX struct
	int nExtraBytes = 0;
	if(pafd->pwfx->wFormatTag != WAVE_FORMAT_PCM)
		nExtraBytes = pafd->pwfx->cbSize;

	pSupportedFormat->pWaveFormatEx = (WAVEFORMATEX*) new BYTE[sizeof(WAVEFORMATEX) + nExtraBytes];
	CopyMemory(pSupportedFormat->pWaveFormatEx, pafd->pwfx, sizeof(WAVEFORMATEX) + nExtraBytes);

	pfeci->parrSupportedFormats->Add(pSupportedFormat);

	return TRUE;
}

